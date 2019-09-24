/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

int srslte_refsignal_dl_sync_init(srslte_refsignal_dl_sync_t* q)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q) {
    // Initialise internals
    bzero(q, sizeof(srslte_refsignal_dl_sync_t));

    // Initialise Reference signals
    ret = srslte_refsignal_cs_init(&q->refsignal, SRSLTE_MAX_PRB);

    // Allocate time buffers
    for (int i = 0; i < SRSLTE_NOF_SF_X_FRAME; i++) {
      q->sequences[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_MAX);
      if (!q->sequences[i]) {
        perror("Allocating sequence\n");
      }
    }

    // Allocate Temporal OFDM buffer
    q->ifft_buffer_in = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_MAX);
    if (!q->ifft_buffer_in) {
      perror("Allocating ifft_buffer_in\n");
    }

    q->ifft_buffer_out = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_MAX);
    if (!q->ifft_buffer_out) {
      perror("Allocating ifft_buffer_out\n");
    }

    // Allocate correlation
    q->correlation = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_MAX * 2);
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
        bzero(q->ifft_buffer_in, sizeof(cf_t) * q->ifft.sf_sz);

        // Put Synchronization signals
        if (i == 0 || i == 5) {
          srslte_pss_put_slot(pss_signal, q->ifft_buffer_in, cell.nof_prb, cell.cp);
          srslte_sss_put_slot(i ? sss_signal5 : sss_signal0, q->ifft_buffer_in, cell.nof_prb, cell.cp);
        }

        // Increase correlation for 2 port eNb
        cell.nof_ports = 2;

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
  float    thr        = 5.5f;
  float    peak_value = 0.0f;
  int      peak_idx   = 0;
  float    rms_avg    = 0;
  uint32_t sf_len     = q->ifft.sf_sz;

  // Load correlation sequence and convert to frequency domain
  cf_t* ptr_filt = q->conv_fft_cc.filter_fft;
  memcpy(ptr_filt, q->sequences[0], sizeof(cf_t) * sf_len);
  bzero(&ptr_filt[sf_len], sizeof(cf_t) * sf_len);
  srslte_dft_run_c(&q->conv_fft_cc.filter_plan, ptr_filt, ptr_filt);

  // Limit correlate for a frame or less
  nsamples = SRSLTE_MIN(nsamples - sf_len, SRSLTE_NOF_SF_X_FRAME * sf_len);

  // Correlation
  for (int n = 0; n < nsamples; n += sf_len) {
    // Set input data, two subframes
    cf_t* ptr_in = &buffer[n];

    // Correlate
    srslte_corr_fft_cc_run_opt(&q->conv_fft_cc, ptr_in, ptr_filt, q->correlation);

    // Find maximum, calculate RMS and peak
    uint32_t imax = srslte_vec_max_abs_ci(q->correlation, sf_len);
    float    peak = cabsf(q->correlation[imax]);
    float    rms  = sqrtf(srslte_vec_avg_power_cf(q->correlation, sf_len));
    rms_avg += rms;

    // Found bigger peak
    if (peak > peak_value) {
      peak_value = peak;
      peak_idx   = imax + n;
    }
  }

  // Condition of peak detection
  rms_avg /= floorf((float)nsamples / sf_len);
  if (peak_value > rms_avg * thr) {
    ret = peak_idx;
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
    uint32_t sf_len   = q->ifft.sf_sz;
    uint32_t sf_count = 0;
    float    rsrp_lin = 0.0f;
    float    rssi_lin = 0.0f;
    float    cfo_acc  = 0.0f;

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
        rsrp_lin += rsrp;
        rssi_lin += rssi;
        cfo_acc += cfo;
        sf_count++;
      }

      // Average measurements
      if (sf_count) {
        rsrp_lin /= sf_count;
        rssi_lin /= sf_count;
        cfo_acc /= sf_count;
      }

      // Calculate in dBm
      q->rsrp_dBfs = 10.0f * log10f(rsrp_lin) + 30.0f;

      // Calculate RSSI in dBm
      q->rssi_dBfs = 10.0f * log10f(rssi_lin) + 30.0f;

      // Calculate RSRQ
      q->rsrq_dB = 10.0f * log10f(q->refsignal.cell.nof_prb) + q->rsrp_dBfs - q->rssi_dBfs;

      q->found      = true;
      q->cfo_Hz     = cfo_acc;
      q->peak_index = peak_idx;
    } else {
      q->found      = false;
      q->rsrp_dBfs  = NAN;
      q->rssi_dBfs  = NAN;
      q->rsrq_dB    = NAN;
      q->cfo_Hz     = NAN;
      q->peak_index = UINT32_MAX;
    }
  }
}

void srslte_refsignal_dl_sync_measure_sf(
    srslte_refsignal_dl_sync_t* q, cf_t* buffer, uint32_t sf_idx, float* rsrp, float* rssi, float* cfo)
{
  float              rsrp_lin  = 0.0f;
  float              rssi_lin  = 0.0f;
  cf_t               corr[4]   = {};
  srslte_dl_sf_cfg_t dl_sf_cfg = {};
  dl_sf_cfg.tti                = sf_idx;

  if (q) {
    cf_t* sf_sequence = q->sequences[sf_idx % SRSLTE_NOF_SF_X_FRAME];

    uint32_t symbol_sz = q->ifft.symbol_sz;
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
      rssi_lin += srslte_vec_dot_prod_conj_ccc(&buffer[offset], &buffer[offset], symbol_sz);
    }

    // Return measurements
    if (rsrp) {
      *rsrp = rsrp_lin * nsymbols;
    }

    if (rssi) {
      *rssi = (float)q->refsignal.cell.nof_prb * rssi_lin / (float)nsymbols * 7.41f;
    }

    if (cfo) {
      *cfo = 0;
      *cfo += cargf(corr[2] * conjf(corr[0])) / (2.0f * M_PI * 7.5f) * 15000.0f;
      *cfo += cargf(corr[3] * conjf(corr[1])) / (2.0f * M_PI * 7.5f) * 15000.0f;
      *cfo /= 2;
    }
  }
}
