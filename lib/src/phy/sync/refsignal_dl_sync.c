/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <complex.h>
#include <math.h>
#include <memory.h>
#include <srslte/phy/sync/refsignal_dl_sync.h>
#include <srslte/phy/utils/vector.h>
#include <srslte/srslte.h>

/*
 * Constants
 * --------------
 * These constants have been optimized for passing scell_search_test for a number of scenarios.
 * - 6 PRB. 6 cells distributed uniformly 10ms delay. Brut-forced search.
 *     srsue/test/phy/scell_search_test --duration=5 --cell.nof_prb=6 --active_cell_list=all
 *       --simulation_cell_list=1,2,3,4,5,6 --channel_period_s=30 --channel.hst.fd=750 --channel.delay_max=10000
 *
 * - 6 PRB. 6 cells distributed uniformly 10ms delay. With PCI set list.
 *     srsue/test/phy/scell_search_test --duration=30 --cell.nof_prb=6 --active_cell_list=2,3,4,5,6
 *       --simulation_cell_list=1,2,3,4,5,6 --channel_period_s=30 --channel.hst.fd=750 --channel.delay_max=10000
 *
 * - 6 PRB. 6 cell PSS, SSS overlapped. Brut-forced search.
 *     srsue/test/phy/scell_search_test --duration=30 --cell.nof_prb=6 --active_cell_list=all
 *       --simulation_cell_list=1,2,3,4,5,6 --channel_period_s=30 --channel.hst.fd=750 --channel.delay_max=0
 *
 * - 100 PRB, 6 cell, distributed around 1ms. With PCI set list.
 *     srsue/test/phy/scell_search_test --duration=30 --cell.nof_prb=100 --active_cell_list=2,3,4,5,6
 *       --simulation_cell_list=1,2,3,4,5,6 --channel_period_s=30 --channel.hst.fd=750 --channel.delay_max=1000
 */
#define REFSIGNAL_DL_SYNC_CORRELATION_THR (4.0f) /* Correlation threashold normalised in Peak / RMS */
#define REFSIGNAL_DL_CFO_MIN_MAX_MILD (1000.0f)  /* Maximum difference between sub-frame CFO estimation in Hz */
#define REFSIGNAL_DL_CFO_MIN_MAX_SEVERE (2000.0f)
#define REFSIGNAL_DL_RSRP_MIN_MAX_MILD (6.0f) /* Maximum difference between sub-frame RSRP estimation in dB */
#define REFSIGNAL_DL_RSRP_MIN_MAX_SEVERE (10.0f)
#define REFSIGNAL_DL_RSRP_FALSE_RATIO_MILD (4.0f) /* Minimum RSRP ratio between real and false in dB */
#define REFSIGNAL_DL_RSRP_FALSE_RATIO_SEVERE (3.0f)
#define REFSIGNAL_DL_SSS_FALSE_RATIO_MILD (15.0f) /* Minimum SSS strength between real and false (linear) */
#define REFSIGNAL_DL_SSS_FALSE_RATIO_SEVERE (1.5f)
#define REFSIGNAL_DL_MAX_FAULT_CHECK (1)      /* Number of mild faults without declaring false */
#define REFSIGNAL_DL_CFO_LOW_WEIGHT (0.5f)    /* Weight for low CFO estimation (2 kHz)      */
#define REFSIGNAL_DL_CFO_MEDIUM_WEIGHT (0.3f) /* Weight for medium CFO estimation (3.5 kHz) */
#define REFSIGNAL_DL_CFO_HIGH_WEIGHT (0.2f)   /* Weight for high CFO estimation (4.66 kHz)  */

/*
 * Local Helpers
 */
static inline void refsignal_set_results_not_found(srslte_refsignal_dl_sync_t* q)
{
  q->found      = false;
  q->rsrp_dBfs  = NAN;
  q->rssi_dBfs  = NAN;
  q->rsrq_dB    = NAN;
  q->cfo_Hz     = NAN;
  q->peak_index = UINT32_MAX;
}

static inline void refsignal_sf_prepare_correlation(srslte_refsignal_dl_sync_t* q)
{
  uint32_t sf_len   = q->ifft.sf_sz;
  cf_t*    ptr_filt = q->conv_fft_cc.filter_fft;
  memcpy(ptr_filt, q->sequences[0], sizeof(cf_t) * sf_len);
  srslte_vec_cf_zero(&ptr_filt[sf_len], sf_len);
  srslte_dft_run_c(&q->conv_fft_cc.filter_plan, ptr_filt, ptr_filt);
}

static inline void
refsignal_sf_correlate(srslte_refsignal_dl_sync_t* q, cf_t* ptr_in, float* peak_value, uint32_t* peak_idx, float* rms)
{
  // Correlate
  srslte_corr_fft_cc_run_opt(&q->conv_fft_cc, ptr_in, q->conv_fft_cc.filter_fft, q->correlation);

  // Find maximum, calculate RMS and peak
  uint32_t imax = srslte_vec_max_abs_ci(q->correlation, q->ifft.sf_sz);

  if (peak_idx) {
    *peak_idx = imax;
  }

  if (peak_value) {
    *peak_value = cabsf(q->correlation[imax]);
  }

  if (rms) {
    *rms = sqrtf(srslte_vec_avg_power_cf(q->correlation, q->ifft.sf_sz));
  }
}

static inline void refsignal_dl_pss_sss_strength(srslte_refsignal_dl_sync_t* q,
                                                 cf_t*                       buffer,
                                                 uint32_t                    sf_idx,
                                                 float*                      pss_strength,
                                                 float*                      sss_strength,
                                                 float*                      sss_strength_false)
{
  uint32_t symbol_sz = q->ifft.cfg.symbol_sz;
  uint32_t cp_len0   = SRSLTE_CP_LEN_NORM(0, symbol_sz);
  uint32_t cp_len1   = SRSLTE_CP_LEN_NORM(1, symbol_sz);

  uint32_t pss_n = cp_len0 + cp_len1 * 6 + symbol_sz * 6;
  uint32_t sss_n = cp_len0 + cp_len1 * 5 + symbol_sz * 5;

  sf_idx %= SRSLTE_NOF_SF_X_FRAME;

  srslte_dl_sf_cfg_t dl_sf_cfg = {};
  dl_sf_cfg.tti                = sf_idx;

  float k = (float)(srslte_refsignal_cs_nof_re(&q->refsignal, &dl_sf_cfg, 0)) / (float)SRSLTE_PSS_LEN;

  if (pss_strength) {
    cf_t corr     = srslte_vec_dot_prod_conj_ccc(&buffer[pss_n], &q->sequences[sf_idx][pss_n], symbol_sz);
    *pss_strength = k * __real__(corr * conjf(corr));
  }

  if (sss_strength) {
    cf_t corr     = srslte_vec_dot_prod_conj_ccc(&buffer[sss_n], &q->sequences[sf_idx][sss_n], symbol_sz);
    *sss_strength = k * __real__(corr * conjf(corr));
  }

  if (sss_strength_false) {
    uint32_t sf_idx2    = (sf_idx + SRSLTE_NOF_SF_X_FRAME / 2) % SRSLTE_NOF_SF_X_FRAME;
    cf_t     corr       = srslte_vec_dot_prod_conj_ccc(&buffer[sss_n], &q->sequences[sf_idx2][sss_n], symbol_sz);
    *sss_strength_false = k * __real__(corr * conjf(corr));
  }
}

int srslte_refsignal_dl_sync_init(srslte_refsignal_dl_sync_t* q)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q) {
    // Initialise internals
    memset(q, 0, sizeof(srslte_refsignal_dl_sync_t));

    // Initialise Reference signals
    ret = srslte_refsignal_cs_init(&q->refsignal, SRSLTE_MAX_PRB);

    // Allocate time buffers
    for (int i = 0; i < SRSLTE_NOF_SF_X_FRAME; i++) {
      q->sequences[i] = srslte_vec_cf_malloc(SRSLTE_SF_LEN_MAX);
      if (!q->sequences[i]) {
        perror("Allocating sequence\n");
      }
    }

    // Allocate Temporal OFDM buffer
    q->ifft_buffer_in = srslte_vec_cf_malloc(SRSLTE_SF_LEN_MAX);
    if (!q->ifft_buffer_in) {
      perror("Allocating ifft_buffer_in\n");
    }

    q->ifft_buffer_out = srslte_vec_cf_malloc(SRSLTE_SF_LEN_MAX);
    if (!q->ifft_buffer_out) {
      perror("Allocating ifft_buffer_out\n");
    }

    // Allocate correlation
    q->correlation = srslte_vec_cf_malloc(SRSLTE_SF_LEN_MAX * 2);
    if (!q->correlation) {
      perror("Allocating correlation\n");
    }

    // Initiate OFDM modulator
    if (!ret) {
      ret = srslte_ofdm_tx_init(&q->ifft, SRSLTE_CP_NORM, q->ifft_buffer_in, q->ifft_buffer_out, SRSLTE_MAX_PRB);
    }

    // Set PRB
    if (!ret) {
      ret = srslte_ofdm_tx_set_prb(&q->ifft, SRSLTE_CP_NORM, SRSLTE_MAX_PRB);
    }

    // Initiate FFT Convolution
    if (!ret) {
      ret = srslte_conv_fft_cc_init(&q->conv_fft_cc, q->ifft.sf_sz, q->ifft.sf_sz);
    }

    // Set default results to not found
    refsignal_set_results_not_found(q);
  }

  return ret;
}

int srslte_refsignal_dl_sync_set_cell(srslte_refsignal_dl_sync_t* q, srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q) {
    cf_t  pss_signal[SRSLTE_PSS_LEN];
    float sss_signal0[SRSLTE_SSS_LEN];
    float sss_signal5[SRSLTE_SSS_LEN];

    // Generate Synchronization signals
    srslte_pss_generate(pss_signal, cell.id % 3);
    srslte_sss_generate(sss_signal0, sss_signal5, cell.id);

    // Set cell for Ref signals
    ret = srslte_refsignal_cs_set_cell(&q->refsignal, cell);

    // Resize OFDM
    if (!ret && q->ifft.nof_re != cell.nof_prb * SRSLTE_NRE) {
      ret = srslte_ofdm_tx_set_prb(&q->ifft, cell.cp, cell.nof_prb);
    }

    // Replan convolution
    if (q->conv_fft_cc.filter_len != q->ifft.sf_sz) {
      srslte_conv_fft_cc_replan(&q->conv_fft_cc, q->ifft.sf_sz, q->ifft.sf_sz);
    }

    // Generate frame with references
    if (!ret) {
      for (int i = 0; i < SRSLTE_NOF_SF_X_FRAME && ret == SRSLTE_SUCCESS; i++) {
        uint32_t nof_re = 0;

        // Default Subframe configuration
        srslte_dl_sf_cfg_t dl_sf_cfg = {};
        dl_sf_cfg.tti                = i;

        // Reset OFDM buffer
        srslte_vec_cf_zero(q->ifft_buffer_in, q->ifft.sf_sz);

        // Put Synchronization signals
        if (i == 0 || i == 5) {
          srslte_pss_put_slot(pss_signal, q->ifft_buffer_in, cell.nof_prb, cell.cp);
          srslte_sss_put_slot(i ? sss_signal5 : sss_signal0, q->ifft_buffer_in, cell.nof_prb, cell.cp);
        }

        // Put Reference signals
        for (int p = 0; p < cell.nof_ports; p++) {
          ret = srslte_refsignal_cs_put_sf(&q->refsignal, &dl_sf_cfg, p, q->ifft_buffer_in);

          // Increment number of resource elements
          if (p == 0) {
            nof_re += srslte_refsignal_cs_nof_re(&q->refsignal, &dl_sf_cfg, p);
          }
        }

        // Run OFDM modulator
        srslte_ofdm_tx_sf(&q->ifft);

        // Undo scaling and normalize overall power to 1
        float scale = 1.0f / nof_re;

        // Copy time domain signal, normalized by number of RE
        srslte_vec_sc_prod_cfc(q->ifft_buffer_out, scale, q->sequences[i], q->ifft.sf_sz);
      }
    }
  }

  return ret;
}

void srslte_refsignal_dl_sync_free(srslte_refsignal_dl_sync_t* q)
{
  if (q) {
    srslte_refsignal_free(&q->refsignal);

    srslte_ofdm_tx_free(&q->ifft);

    if (q->ifft_buffer_in) {
      free(q->ifft_buffer_in);
    }

    if (q->ifft_buffer_out) {
      free(q->ifft_buffer_out);
    }

    if (q->correlation) {
      free(q->correlation);
    }

    for (int i = 0; i < SRSLTE_NOF_SF_X_FRAME; i++) {
      if (q->sequences[i]) {
        free(q->sequences[i]);
      }
    }

    srslte_conv_fft_cc_free(&q->conv_fft_cc);
  }
}

int srslte_refsignal_dl_sync_find_peak(srslte_refsignal_dl_sync_t* q, cf_t* buffer, uint32_t nsamples)
{
  int      ret        = SRSLTE_ERROR;
  float    peak_value = 0.0f;
  int      peak_idx   = 0;
  float    rms_avg    = 0;
  uint32_t sf_len     = q->ifft.sf_sz;

  // Load correlation sequence and convert to frequency domain
  refsignal_sf_prepare_correlation(q);

  // Limit correlate for a frame or less
  nsamples = SRSLTE_MIN(nsamples - sf_len, SRSLTE_NOF_SF_X_FRAME * sf_len);

  // Correlation
  for (int n = 0; n < nsamples; n += sf_len) {
    // Correlate, find maximum, calculate RMS and peak
    uint32_t imax = 0;
    float    peak = 0.0f;
    float    rms  = 0.0f;
    refsignal_sf_correlate(q, &buffer[n], &peak, &imax, &rms);

    rms_avg += rms;

    // Found bigger peak
    if (peak > peak_value) {
      peak_value = peak;
      peak_idx   = imax + n;
    }
  }

  // Condition of peak detection
  rms_avg /= floorf((float)nsamples / sf_len);
  if (peak_value > rms_avg * REFSIGNAL_DL_SYNC_CORRELATION_THR) {
    ret = peak_idx;
  }

  // Double check sub-frame selection failure due to high PSS
  if (ret > 0) {
    float sss_strength       = 0.0f;
    float sss_strength_false = 0.0f;
    refsignal_dl_pss_sss_strength(q, &buffer[peak_idx], 0, NULL, &sss_strength, &sss_strength_false);

    float rsrp_lin       = 0.0f;
    float rsrp_lin_false = 0.0f;
    srslte_refsignal_dl_sync_measure_sf(q, &buffer[peak_idx], 0, &rsrp_lin, NULL, NULL);
    srslte_refsignal_dl_sync_measure_sf(q, &buffer[peak_idx], 5, &rsrp_lin_false, NULL, NULL);

    // Change base sub-frame
    if (sss_strength_false > sss_strength && rsrp_lin_false > rsrp_lin) {
      ret += (q->ifft.sf_sz * SRSLTE_NOF_SF_X_FRAME) / 2;
    }
  }

  INFO("pci=%03d; sf_len=%d; imax=%d; peak=%.3f; rms=%.3f; peak/rms=%.3f\n",
       q->refsignal.cell.id,
       sf_len,
       peak_idx,
       peak_value,
       rms_avg,
       peak_value / rms_avg);

  // Return the peak position if found, -1 otherwise
  return ret;
}

void srslte_refsignal_dl_sync_run(srslte_refsignal_dl_sync_t* q, cf_t* buffer, uint32_t nsamples)
{
  if (q) {
    uint32_t sf_len                 = q->ifft.sf_sz;
    uint32_t sf_count               = 0;
    float    rsrp_lin               = 0.0f;
    float    rsrp_lin_min           = +INFINITY;
    float    rsrp_lin_max           = -INFINITY;
    float    rssi_lin               = 0.0f;
    float    cfo_acc                = 0.0f;
    float    cfo_min                = +INFINITY;
    float    cfo_max                = -INFINITY;
    float    sss_strength_avg       = 0.0f;
    float    sss_strength_false_avg = 0.0f;
    float    rsrp_false_avg         = 0.0f;
    bool     false_alarm            = false;

    // Stage 1: find peak
    int peak_idx = srslte_refsignal_dl_sync_find_peak(q, buffer, nsamples);

    // Stage 2: Proccess subframes
    if (peak_idx >= 0) {
      // Calculate initial subframe index and sample
      uint32_t sf_idx_init = (2 * SRSLTE_NOF_SF_X_FRAME - peak_idx / sf_len) % SRSLTE_NOF_SF_X_FRAME;
      uint32_t n_init      = peak_idx % sf_len;

      for (int sf_idx = sf_idx_init, n = n_init; n < (nsamples - sf_len + 1);
           sf_idx = (sf_idx + 1) % SRSLTE_NOF_SF_X_FRAME, n += sf_len) {
        cf_t* buf = &buffer[n];

        // Measure subframe rsrp, rssi and accumulate
        float rsrp = 0.0f, rssi = 0.0f, cfo = 0.0f;
        srslte_refsignal_dl_sync_measure_sf(q, buf, sf_idx, &rsrp, &rssi, &cfo);

        // Update measurements
        rsrp_lin += rsrp;
        rsrp_lin_min = SRSLTE_MIN(rsrp_lin_min, rsrp);
        rsrp_lin_max = SRSLTE_MAX(rsrp_lin_max, rsrp);

        rssi_lin += rssi;

        cfo_acc += cfo;
        cfo_min = SRSLTE_MIN(cfo_min, cfo);
        cfo_max = SRSLTE_MAX(cfo_max, cfo);

        // Compute PSS/SSS strength
        if (sf_idx % (SRSLTE_NOF_SF_X_FRAME / 2) == 0) {
          float sss_strength       = 0.0f;
          float sss_strength_false = 0.0f;
          refsignal_dl_pss_sss_strength(q, buf, sf_idx, NULL, &sss_strength, &sss_strength_false);

          float rsrp_false = 0.0f;
          srslte_refsignal_dl_sync_measure_sf(q, buf, sf_idx + 1, &rsrp_false, NULL, NULL);

          sss_strength_avg += sss_strength;
          sss_strength_false_avg += sss_strength_false;
          rsrp_false_avg += rsrp_false;
        }

        // Increment counter
        sf_count++;
      }

      // Average measurements
      if (sf_count) {
        rsrp_lin /= sf_count;
        rssi_lin /= sf_count;
        cfo_acc /= sf_count;
        sss_strength_avg /= (2.0f * sf_count / SRSLTE_NOF_SF_X_FRAME);
        sss_strength_false_avg /= (2.0f * sf_count / SRSLTE_NOF_SF_X_FRAME);
        rsrp_false_avg /= (2.0f * sf_count / SRSLTE_NOF_SF_X_FRAME);
      }

      // RSRP conversion to dB
      float rsrp_dB_min   = srslte_convert_power_to_dBm(rsrp_lin_min);
      float rsrp_dB_max   = srslte_convert_power_to_dBm(rsrp_lin_max);
      float rsrp_dB       = srslte_convert_power_to_dBm(rsrp_lin);
      float rsrp_false_dB = srslte_convert_power_to_dBm(rsrp_false_avg);

      // Stage 3: Final false alarm decision
      uint32_t false_count = 0;
      if (sss_strength_avg < sss_strength_false_avg * REFSIGNAL_DL_SSS_FALSE_RATIO_SEVERE) {
        false_alarm = true;
      } else if (sss_strength_avg < sss_strength_false_avg * REFSIGNAL_DL_SSS_FALSE_RATIO_MILD) {
        false_count++;
      }

      if (cfo_max - cfo_min > REFSIGNAL_DL_CFO_MIN_MAX_SEVERE) {
        false_alarm = true;
      } else if (cfo_max - cfo_min > REFSIGNAL_DL_CFO_MIN_MAX_MILD) {
        false_count++;
      }

      if (rsrp_dB_max - rsrp_dB_min > REFSIGNAL_DL_RSRP_MIN_MAX_SEVERE) {
        false_alarm = true;
      } else if (rsrp_dB_max - rsrp_dB_min > REFSIGNAL_DL_RSRP_MIN_MAX_MILD) {
        false_count++;
      }

      if (rsrp_dB - rsrp_false_dB < REFSIGNAL_DL_RSRP_FALSE_RATIO_SEVERE) {
        false_alarm = true;
      } else if (rsrp_dB - rsrp_false_dB < REFSIGNAL_DL_RSRP_FALSE_RATIO_MILD) {
        false_count++;
      }

      // Allow only one check fail
      if (false_count > REFSIGNAL_DL_MAX_FAULT_CHECK) {
        false_alarm = true;
      }

      INFO("-- pci=%03d; rsrp_dB=(%+.1f|%+.1f|%+.1f); rsrp_max-min=%.1f; rsrp_false_ratio=%.1f; "
           "cfo=(%.1f|%.1f|%.1f); cfo_max-min=%.1f; sss_ratio=%f; false_count=%d;\n",
           q->refsignal.cell.id,
           rsrp_dB_min,
           rsrp_dB,
           rsrp_dB_max,
           rsrp_dB_max - rsrp_dB_min,
           rsrp_dB - rsrp_false_dB,
           cfo_min,
           cfo_acc,
           cfo_max,
           cfo_max - cfo_min,
           sss_strength_avg / sss_strength_false_avg,
           false_count);

      if (!false_alarm) {

        // Calculate in dBm
        q->rsrp_dBfs = rsrp_dB;

        // Calculate RSSI in dBm
        q->rssi_dBfs = srslte_convert_power_to_dBm(rssi_lin);

        // Calculate RSRQ
        q->rsrq_dB = srslte_convert_power_to_dB(q->refsignal.cell.nof_prb) + q->rsrp_dBfs - q->rssi_dBfs;

        q->found      = true;
        q->cfo_Hz     = cfo_acc;
        q->peak_index = peak_idx;
      } else {
        refsignal_set_results_not_found(q);
      }
    } else {
      refsignal_set_results_not_found(q);
    }
  }
}

void srslte_refsignal_dl_sync_measure_sf(srslte_refsignal_dl_sync_t* q,
                                         cf_t*                       buffer,
                                         uint32_t                    sf_idx,
                                         float*                      rsrp,
                                         float*                      rssi,
                                         float*                      cfo)
{
  float              rsrp_lin  = 0.0f;
  float              rssi_lin  = 0.0f;
  cf_t               corr[4]   = {};
  srslte_dl_sf_cfg_t dl_sf_cfg = {};
  dl_sf_cfg.tti                = sf_idx;

  if (q) {
    cf_t* sf_sequence = q->sequences[sf_idx % SRSLTE_NOF_SF_X_FRAME];

    uint32_t symbol_sz = q->ifft.cfg.symbol_sz;
    uint32_t nsymbols  = srslte_refsignal_cs_nof_symbols(&q->refsignal, &dl_sf_cfg, 0);
    uint32_t cp_len0   = SRSLTE_CP_LEN_NORM(0, symbol_sz);
    uint32_t cp_len1   = SRSLTE_CP_LEN_NORM(1, symbol_sz);

    for (uint32_t l = 0; l < nsymbols; l++) {
      // Calculate FFT window offset for reference signals symbols
      uint32_t symbidx = srslte_refsignal_cs_nsymbol(l, q->refsignal.cell.cp, 0);
      uint32_t offset  = cp_len0 + (symbol_sz + cp_len1) * symbidx;

      if (l >= nsymbols / 2) {
        offset += cp_len0 - cp_len1;
      }

      // Complex correlation
      corr[l] = srslte_vec_dot_prod_conj_ccc(&buffer[offset], &sf_sequence[offset], symbol_sz);

      // Calculate RSRP
      rsrp_lin += __real__(corr[l] * conjf(corr[l]));

      // Calculate RSSI
      if (rssi) {
        rssi_lin += srslte_vec_dot_prod_conj_ccc(&buffer[offset], &buffer[offset], symbol_sz);
      }
    }

    // Return measurements
    if (rsrp) {
      *rsrp = rsrp_lin * nsymbols;
    }

    if (rssi) {
      *rssi = (float)q->refsignal.cell.nof_prb * rssi_lin / (float)nsymbols * 7.41f;
    }

    if (cfo) {

      // Distances between symbols
      float distance_1 = (cp_len1 + symbol_sz) * 4.0f; // Number of samples between first and second symbol
      float distance_2 =
          (cp_len1 + symbol_sz) * 3.0f + (cp_len0 - cp_len1); // Number of samples between second and third symbol

      // Averaging weights, all of them must be 1.0f
      float low_w    = REFSIGNAL_DL_CFO_LOW_WEIGHT / 2.0f;    // Two of them
      float medium_w = REFSIGNAL_DL_CFO_MEDIUM_WEIGHT / 2.0f; // Two of them
      float high_w   = REFSIGNAL_DL_CFO_HIGH_WEIGHT;          // One of them

      // Initialise average
      *cfo = 0;

      // Low doppler (2 kHz)
      *cfo += cargf(corr[2] * conjf(corr[0])) / (2.0f * M_PI * 7.5f) * 15000.0f * low_w;
      *cfo += cargf(corr[3] * conjf(corr[1])) / (2.0f * M_PI * 7.5f) * 15000.0f * low_w;

      // Medium Doppler (3.5 kHz)
      *cfo += cargf(corr[1] * conjf(corr[0])) / (2.0f * M_PI * distance_1) * (15000.0f * symbol_sz) * medium_w;
      *cfo += cargf(corr[3] * conjf(corr[2])) / (2.0f * M_PI * distance_1) * (15000.0f * symbol_sz) * medium_w;

      // High doppler (4.66 kHz)
      *cfo += cargf(corr[2] * conjf(corr[1])) / (2.0f * M_PI * distance_2) * (15000.0f * symbol_sz) * high_w;
    }
  }
}
