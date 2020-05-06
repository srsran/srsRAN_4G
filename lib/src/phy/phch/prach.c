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

#include "srslte/srslte.h"
#include <math.h>
#include <string.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/prach.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#include "prach_tables.h"

float save_corr[4096];

// PRACH detection threshold is PRACH_DETECT_FACTOR*average
#define PRACH_DETECT_FACTOR 18

#define N_SEQS 64         // Number of prach sequences available
#define N_RB_SC 12        // Number of subcarriers per resource block
#define DELTA_F 15000     // Normal subcarrier spacing
#define DELTA_F_RA 1250   // PRACH subcarrier spacing
#define DELTA_F_RA_4 7500 // PRACH subcarrier spacing for format 4
#define PHI 7             // PRACH phi parameter
#define PHI_4 2           // PRACH phi parameter for format 4
#define MAX_ROOTS 838     // Max number of root sequences

#define PRACH_AMP 1.0

int srslte_prach_set_cell_(srslte_prach_t*      p,
                           uint32_t             N_ifft_ul,
                           uint32_t             config_idx,
                           uint32_t             root_seq_index,
                           bool                 high_speed_flag,
                           uint32_t             zero_corr_zone_config,
                           srslte_tdd_config_t* tdd_config,
                           uint32_t             num_ra_preambles);

uint32_t srslte_prach_get_preamble_format(uint32_t config_idx)
{
  return config_idx / 16;
}

srslte_prach_sfn_t srslte_prach_get_sfn(uint32_t config_idx)
{
  if ((config_idx % 16) < 3 || (config_idx % 16) == 15) {
    return SRSLTE_PRACH_SFN_EVEN;
  } else {
    return SRSLTE_PRACH_SFN_ANY;
  }
}

/* Returns true if current_tti is a valid opportunity for PRACH transmission and the is an allowed subframe,
 * or allowed_subframe == -1
 */
bool srslte_prach_tti_opportunity(srslte_prach_t* p, uint32_t current_tti, int allowed_subframe)
{
  uint32_t config_idx = p->config_idx;
  if (!p->tdd_config.configured) {
    return srslte_prach_tti_opportunity_config_fdd(config_idx, current_tti, allowed_subframe);
  } else {
    return srslte_prach_tti_opportunity_config_tdd(
        config_idx, p->tdd_config.sf_config, current_tti, &p->current_prach_idx);
  }
}

bool srslte_prach_tti_opportunity_config_fdd(uint32_t config_idx, uint32_t current_tti, int allowed_subframe)
{
  // Get SFN and sf_idx from the PRACH configuration index
  srslte_prach_sfn_t prach_sfn = srslte_prach_get_sfn(config_idx);

  // This is the only option which provides always an opportunity for PRACH transmission.
  if (config_idx == 14) {
    return true;
  }

  if ((prach_sfn == SRSLTE_PRACH_SFN_EVEN && ((current_tti / 10) % 2) == 0) || prach_sfn == SRSLTE_PRACH_SFN_ANY) {
    srslte_prach_sf_config_t sf_config;
    srslte_prach_sf_config(config_idx, &sf_config);
    for (int i = 0; i < sf_config.nof_sf; i++) {
      if (((current_tti % 10) == sf_config.sf[i] && allowed_subframe == -1) ||
          ((current_tti % 10) == sf_config.sf[i] && (current_tti % 10) == allowed_subframe)) {
        return true;
      }
    }
  }
  return false;
}

uint32_t srslte_prach_nof_f_idx_tdd(uint32_t config_idx, uint32_t tdd_ul_dl_config)
{
  if (config_idx < 64 && tdd_ul_dl_config < 7) {
    return prach_tdd_loc_table[config_idx][tdd_ul_dl_config].nof_elems;
  } else {
    ERROR("PRACH: Invalid parmeters config_idx=%d, tdd_ul_config=%d\n", config_idx, tdd_ul_dl_config);
    return 0;
  }
}

uint32_t srslte_prach_f_id_tdd(uint32_t config_idx, uint32_t tdd_ul_dl_config, uint32_t prach_idx)
{
  if (config_idx < 64 && tdd_ul_dl_config < 7) {
    return prach_tdd_loc_table[config_idx][tdd_ul_dl_config].elems[prach_idx].f;
  } else {
    ERROR("PRACH: Invalid parmeters config_idx=%d, tdd_ul_config=%d\n", config_idx, tdd_ul_dl_config);
    return 0;
  }
}

uint32_t srslte_prach_f_ra_tdd(uint32_t config_idx,
                               uint32_t tdd_ul_dl_config,
                               uint32_t current_tti,
                               uint32_t prach_idx,
                               uint32_t prach_offset,
                               uint32_t n_rb_ul)
{

  if (config_idx >= 64 || tdd_ul_dl_config >= 7) {
    ERROR("PRACH: Invalid parameters config_idx=%d, tdd_ul_config=%d\n", config_idx, tdd_ul_dl_config);
    return 0;
  }
  uint32_t f_ra = prach_tdd_loc_table[config_idx][tdd_ul_dl_config].elems[prach_idx].f;

  if (config_idx < 48) {
    if ((f_ra % 2) == 0) {
      return prach_offset + 6 * (f_ra / 2);
    } else {
      return n_rb_ul - 6 - prach_offset + 6 * (f_ra / 2);
    }
  } else {
    uint32_t N_sp;
    if (tdd_ul_dl_config >= 3 && tdd_ul_dl_config <= 5) {
      N_sp = 1;
    } else {
      N_sp = 2;
    }

    uint32_t t1 = prach_tdd_loc_table[config_idx][tdd_ul_dl_config].elems[prach_idx].t1;

    uint32_t sfn = current_tti / 10;

    if ((((sfn % 2) * (2 - N_sp) + t1) % 2) == 0) {
      return 6 * f_ra;
    } else {
      return n_rb_ul - 6 * (f_ra + 1);
    }
  }
}

bool srslte_prach_tti_opportunity_config_tdd(uint32_t  config_idx,
                                             uint32_t  tdd_ul_dl_config,
                                             uint32_t  current_tti,
                                             uint32_t* prach_idx)
{
  if (config_idx >= 64 || tdd_ul_dl_config >= 7) {
    ERROR("PRACH: Invalid parameters config_idx=%d, tdd_ul_config=%d\n", config_idx, tdd_ul_dl_config);
    return 0;
  }

  uint32_t nof_elems = prach_tdd_loc_table[config_idx][tdd_ul_dl_config].nof_elems;

  // Table 5.7.1-4 allocates in time then in frequency
  for (uint32_t i = 0; i < nof_elems; i++) {
    uint32_t t0 = prach_tdd_loc_table[config_idx][tdd_ul_dl_config].elems[i].t0;
    uint32_t t1 = prach_tdd_loc_table[config_idx][tdd_ul_dl_config].elems[i].t1;
    uint32_t t2 = prach_tdd_loc_table[config_idx][tdd_ul_dl_config].elems[i].t2;

    uint32_t sfn    = current_tti / 10;
    uint32_t sf_idx = current_tti % 10;

    if (((sfn % 2) && t0 == 2) || (!(sfn % 2) && t0 == 1) || (t0 == 0)) {
      if ((sf_idx < 5 && t1 == 0) || (sf_idx >= 5 && t1 == 1)) {
        if (config_idx < 48) { // format 0 to 3
          if ((sf_idx) % 5 == (t2 + 2)) {
            if (prach_idx) {
              *prach_idx = i;
            }
            return true;
          }
        } else {
          // Only UpTs subframes
          srslte_tdd_config_t c = {tdd_ul_dl_config, 0, true};
          if (srslte_sfidx_tdd_type(c, sf_idx) == SRSLTE_TDD_SF_S) {
            if (prach_idx) {
              *prach_idx = i;
            }
            return true;
          }
        }
      }
    }
  }
  return false;
}

void srslte_prach_sf_config(uint32_t config_idx, srslte_prach_sf_config_t* sf_config)
{
  memcpy(sf_config, &prach_sf_config[config_idx % 16], sizeof(srslte_prach_sf_config_t));
}

// For debug use only
void print(void* d, uint32_t size, uint32_t len, char* file_str)
{
  FILE* f;
  f = fopen(file_str, "wb");
  fwrite(d, size, len, f);
  fclose(f);
}

int srslte_prach_gen_seqs(srslte_prach_t* p)
{
  uint32_t u           = 0;
  uint32_t v           = 1;
  int      v_max       = 0;
  uint32_t p_          = 0;
  uint32_t d_u         = 0;
  uint32_t d_start     = 0;
  uint32_t N_shift     = 0;
  int      N_neg_shift = 0;
  uint32_t N_group     = 0;
  uint32_t C_v         = 0;
  cf_t     root[839];

  // Generate our 64 preamble sequences
  for (int i = 0; i < N_SEQS; i++) {

    if (v > v_max) {
      // Get a new root sequence
      if (4 == p->f) {
        u = prach_zc_roots_format4[(p->rsi + p->N_roots) % 138];
      } else {
        u = prach_zc_roots[(p->rsi + p->N_roots) % 838];
      }

      for (int j = 0; j < p->N_zc; j++) {
        double phase = -M_PI * u * j * (j + 1) / p->N_zc;
        root[j]      = cexp(phase * I);
      }
      p->root_seqs_idx[p->N_roots++] = i;

      // Determine v_max
      if (p->hs) {
        // High-speed cell
        for (p_ = 1; p_ <= p->N_zc; p_++) {
          if (((p_ * u) % p->N_zc) == 1)
            break;
        }
        if (p_ < p->N_zc / 2) {
          d_u = p_;
        } else {
          d_u = p->N_zc - p_;
        }
        if (d_u >= p->N_cs && d_u < p->N_zc / 3) {
          N_shift = d_u / p->N_cs;
          d_start = 2 * d_u + N_shift * p->N_cs;
          N_group = p->N_zc / d_start;
          if (p->N_zc > 2 * d_u + N_group * d_start) {
            N_neg_shift = (p->N_zc - 2 * d_u - N_group * d_start) / p->N_cs;
          } else {
            N_neg_shift = 0;
          }
        } else if (p->N_zc / 3 <= d_u && d_u <= (p->N_zc - p->N_cs) / 2) {
          N_shift = (p->N_zc - 2 * d_u) / p->N_cs;
          d_start = p->N_zc - 2 * d_u + N_shift * p->N_cs;
          N_group = d_u / d_start;
          if (d_u > N_group * d_start) {
            N_neg_shift = (d_u - N_group * d_start) / p->N_cs;
          } else {
            N_neg_shift = 0;
          }
          if (N_neg_shift > N_shift)
            N_neg_shift = N_shift;
        } else {
          N_shift = 0;
        }
        v_max = N_shift * N_group + N_neg_shift - 1;
        if (v_max < 0) {
          v_max = 0;
        }
      } else {
        // Normal cell
        if (0 == p->N_cs) {
          v_max = 0;
        } else {
          v_max = (p->N_zc / p->N_cs) - 1;
        }
      }

      v = 0;
    }

    // Shift root and add to set
    if (p->hs) {
      if (N_shift == 0) {
        C_v = 0;
      } else {
        C_v = d_start * floor(v / N_shift) + (v % N_shift) * p->N_cs;
      }
    } else {
      C_v = v * p->N_cs;
    }
    for (int j = 0; j < p->N_zc; j++) {
      p->seqs[i][j] = root[(j + C_v) % p->N_zc];
    }

    v++;
  }
  return 0;
}

int srslte_prach_set_cfg(srslte_prach_t* p, srslte_prach_cfg_t* cfg, uint32_t nof_prb)
{
  return srslte_prach_set_cell_(p,
                                srslte_symbol_sz(nof_prb),
                                cfg->config_idx,
                                cfg->root_seq_idx,
                                cfg->hs_flag,
                                cfg->zero_corr_zone,
                                &cfg->tdd_config,
                                cfg->num_ra_preambles);
}

int srslte_prach_init(srslte_prach_t* p, uint32_t max_N_ifft_ul)
{
  int ret = SRSLTE_ERROR;
  if (p != NULL && max_N_ifft_ul < 2049) {
    bzero(p, sizeof(srslte_prach_t));

    p->max_N_ifft_ul = max_N_ifft_ul;

    // Set up containers
    p->prach_bins = srslte_vec_cf_malloc(MAX_N_zc);
    p->corr_spec  = srslte_vec_cf_malloc(MAX_N_zc);
    p->corr       = srslte_vec_f_malloc(MAX_N_zc);

    // Set up ZC FFTS
    if (srslte_dft_plan(&p->zc_fft, MAX_N_zc, SRSLTE_DFT_FORWARD, SRSLTE_DFT_COMPLEX)) {
      return SRSLTE_ERROR;
    }
    srslte_dft_plan_set_mirror(&p->zc_fft, false);
    srslte_dft_plan_set_norm(&p->zc_fft, true);

    if (srslte_dft_plan(&p->zc_ifft, MAX_N_zc, SRSLTE_DFT_BACKWARD, SRSLTE_DFT_COMPLEX)) {
      return SRSLTE_ERROR;
    }
    srslte_dft_plan_set_mirror(&p->zc_ifft, false);
    srslte_dft_plan_set_norm(&p->zc_ifft, false);

    uint32_t fft_size_alloc = max_N_ifft_ul * DELTA_F / DELTA_F_RA;

    p->ifft_in  = srslte_vec_cf_malloc(fft_size_alloc);
    p->ifft_out = srslte_vec_cf_malloc(fft_size_alloc);
    if (srslte_dft_plan(&p->ifft, fft_size_alloc, SRSLTE_DFT_BACKWARD, SRSLTE_DFT_COMPLEX)) {
      ERROR("Error creating DFT plan\n");
      return -1;
    }
    srslte_dft_plan_set_mirror(&p->ifft, true);
    srslte_dft_plan_set_norm(&p->ifft, true);

    if (srslte_dft_plan(&p->fft, fft_size_alloc, SRSLTE_DFT_FORWARD, SRSLTE_DFT_COMPLEX)) {
      ERROR("Error creating DFT plan\n");
      return -1;
    }

    p->signal_fft = srslte_vec_cf_malloc(fft_size_alloc);
    if (!p->signal_fft) {
      ERROR("Error allocating memory\n");
      return -1;
    }

    srslte_dft_plan_set_mirror(&p->fft, true);
    srslte_dft_plan_set_norm(&p->fft, false);

    ret = SRSLTE_SUCCESS;
  } else {
    ERROR("Invalid parameters\n");
  }

  return ret;
}

int srslte_prach_set_cell_(srslte_prach_t*      p,
                           uint32_t             N_ifft_ul,
                           uint32_t             config_idx,
                           uint32_t             root_seq_index,
                           bool                 high_speed_flag,
                           uint32_t             zero_corr_zone_config,
                           srslte_tdd_config_t* tdd_config,
                           uint32_t             num_ra_preambles)
{
  int ret = SRSLTE_ERROR;
  if (p != NULL && N_ifft_ul < 2049 && config_idx < 64 && root_seq_index < MAX_ROOTS) {
    if (N_ifft_ul > p->max_N_ifft_ul) {
      ERROR("PRACH: Error in set_cell(): N_ifft_ul must be lower or equal max_N_ifft_ul in init()\n");
      return -1;
    }

    uint32_t preamble_format = srslte_prach_get_preamble_format(config_idx);
    p->config_idx            = config_idx;
    p->f                     = preamble_format;
    p->rsi                   = root_seq_index;
    p->hs                    = high_speed_flag;
    p->zczc                  = zero_corr_zone_config;
    p->detect_factor         = PRACH_DETECT_FACTOR;
    p->num_ra_preambles      = num_ra_preambles;
    if (tdd_config) {
      p->tdd_config = *tdd_config;
    }

    // Determine N_zc and N_cs
    if (4 == preamble_format) {
      if (p->zczc < 7) {
        p->N_zc = 139;
        p->N_cs = prach_Ncs_format4[p->zczc];
      } else {
        ERROR("Invalid zeroCorrelationZoneConfig=%d for format4\n", p->zczc);
        return SRSLTE_ERROR;
      }
    } else {
      p->N_zc = MAX_N_zc;
      if (p->hs) {
        if (p->zczc < 15) {
          p->N_cs = prach_Ncs_restricted[p->zczc];
        } else {
          ERROR("Invalid zeroCorrelationZoneConfig=%d for restricted set\n", p->zczc);
          return SRSLTE_ERROR;
        }
      } else {
        if (p->zczc < 16) {
          p->N_cs = prach_Ncs_unrestricted[p->zczc];
        } else {
          ERROR("Invalid zeroCorrelationZoneConfig=%d\n", p->zczc);
          return SRSLTE_ERROR;
        }
      }
    }

    // Set up ZC FFTS
    if (p->N_zc != MAX_N_zc) {
      if (srslte_dft_replan(&p->zc_fft, p->N_zc)) {
        return SRSLTE_ERROR;
      }
      if (srslte_dft_replan(&p->zc_ifft, p->N_zc)) {
        return SRSLTE_ERROR;
      }
    }

    // Generate our 64 sequences
    p->N_roots = 0;
    srslte_prach_gen_seqs(p);
    // Ensure num_ra_preambles is valid, if not assign default value
    if (p->num_ra_preambles < 4 || p->num_ra_preambles > p->N_roots) {
      p->num_ra_preambles = p->N_roots;
    }
    // Generate sequence FFTs
    for (int i = 0; i < N_SEQS; i++) {
      srslte_dft_run(&p->zc_fft, p->seqs[i], p->dft_seqs[i]);
    }

    // Create our FFT objects and buffers
    p->N_ifft_ul = N_ifft_ul;
    if (4 == preamble_format) {
      p->N_ifft_prach = p->N_ifft_ul * DELTA_F / DELTA_F_RA_4;
    } else {
      p->N_ifft_prach = p->N_ifft_ul * DELTA_F / DELTA_F_RA;
    }

    /* The deadzone specifies the number of samples at the end of the correlation window
     * that will be considered as belonging to the next preamble
     */
    p->deadzone = 0;
    /*
    if(p->N_cs != 0) {
      float samp_rate=15000*p->N_ifft_ul;
      p->deadzone = (uint32_t) ceil((float) samp_rate/((float) p->N_zc*subcarrier_spacing));
    }*/

    if (srslte_dft_replan(&p->ifft, p->N_ifft_prach)) {
      ERROR("Error creating DFT plan\n");
      return -1;
    }
    if (srslte_dft_replan(&p->fft, p->N_ifft_prach)) {
      ERROR("Error creating DFT plan\n");
      return -1;
    }

    p->N_seq = prach_Tseq[p->f] * p->N_ifft_ul / 2048;
    p->N_cp  = prach_Tcp[p->f] * p->N_ifft_ul / 2048;
    p->T_seq = prach_Tseq[p->f] * SRSLTE_LTE_TS;
    p->T_tot = (prach_Tseq[p->f] + prach_Tcp[p->f]) * SRSLTE_LTE_TS;

    ret = SRSLTE_SUCCESS;
  } else {
    ERROR("Invalid parameters\n");
  }

  return ret;
}

int srslte_prach_gen(srslte_prach_t* p, uint32_t seq_index, uint32_t freq_offset, cf_t* signal)
{
  int ret = SRSLTE_ERROR;
  if (p != NULL && seq_index < N_SEQS && signal != NULL) {
    // Calculate parameters
    uint32_t N_rb_ul = srslte_nof_prb(p->N_ifft_ul);
    uint32_t k_0     = freq_offset * N_RB_SC - N_rb_ul * N_RB_SC / 2 + p->N_ifft_ul / 2;
    uint32_t K       = DELTA_F / DELTA_F_RA;
    uint32_t begin   = PHI + (K * k_0) + (K / 2);

    if (6 + freq_offset > N_rb_ul) {
      ERROR("Error no space for PRACH: frequency offset=%d, N_rb_ul=%d\n", freq_offset, N_rb_ul);
      return ret;
    }

    DEBUG("N_zc: %d, N_cp: %d, N_seq: %d, N_ifft_prach=%d begin: %d\n",
          p->N_zc,
          p->N_cp,
          p->N_seq,
          p->N_ifft_prach,
          begin);

    // Map dft-precoded sequence to ifft bins
    memset(p->ifft_in, 0, begin * sizeof(cf_t));
    memcpy(&p->ifft_in[begin], p->dft_seqs[seq_index], p->N_zc * sizeof(cf_t));
    memset(&p->ifft_in[begin + p->N_zc], 0, (p->N_ifft_prach - begin - p->N_zc) * sizeof(cf_t));

    srslte_dft_run(&p->ifft, p->ifft_in, p->ifft_out);

    // Copy CP into buffer
    memcpy(signal, &p->ifft_out[p->N_ifft_prach - p->N_cp], p->N_cp * sizeof(cf_t));

    // Copy preamble sequence into buffer
    for (int i = 0; i < p->N_seq; i++) {
      signal[p->N_cp + i] = p->ifft_out[i % p->N_ifft_prach];
    }

    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

void srslte_prach_set_detect_factor(srslte_prach_t* p, float ratio)
{
  p->detect_factor = ratio;
}

int srslte_prach_detect(srslte_prach_t* p,
                        uint32_t        freq_offset,
                        cf_t*           signal,
                        uint32_t        sig_len,
                        uint32_t*       indices,
                        uint32_t*       n_indices)
{
  return srslte_prach_detect_offset(p, freq_offset, signal, sig_len, indices, NULL, NULL, n_indices);
}

int srslte_prach_detect_offset(srslte_prach_t* p,
                               uint32_t        freq_offset,
                               cf_t*           signal,
                               uint32_t        sig_len,
                               uint32_t*       indices,
                               float*          t_offsets,
                               float*          peak_to_avg,
                               uint32_t*       n_indices)
{
  int ret = SRSLTE_ERROR;
  if (p != NULL && signal != NULL && sig_len > 0 && indices != NULL) {

    if (sig_len < p->N_ifft_prach) {
      ERROR("srslte_prach_detect: Signal length is %d and should be %d\n", sig_len, p->N_ifft_prach);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

    // FFT incoming signal
    srslte_dft_run(&p->fft, signal, p->signal_fft);

    *n_indices = 0;

    // Extract bins of interest
    uint32_t N_rb_ul = srslte_nof_prb(p->N_ifft_ul);
    uint32_t k_0     = freq_offset * N_RB_SC - N_rb_ul * N_RB_SC / 2 + p->N_ifft_ul / 2;
    uint32_t K       = DELTA_F / DELTA_F_RA;
    uint32_t begin   = PHI + (K * k_0) + (K / 2);

    memcpy(p->prach_bins, &p->signal_fft[begin], p->N_zc * sizeof(cf_t));

    for (int i = 0; i < p->num_ra_preambles; i++) {
      cf_t* root_spec = p->dft_seqs[p->root_seqs_idx[i]];

      srslte_vec_prod_conj_ccc(p->prach_bins, root_spec, p->corr_spec, p->N_zc);

      srslte_dft_run(&p->zc_ifft, p->corr_spec, p->corr_spec);

      srslte_vec_abs_square_cf(p->corr_spec, p->corr, p->N_zc);

      float corr_ave = srslte_vec_acc_ff(p->corr, p->N_zc) / p->N_zc;

      uint32_t winsize = 0;
      if (p->N_cs != 0) {
        winsize = p->N_cs;
      } else {
        winsize = p->N_zc;
      }
      uint32_t n_wins = p->N_zc / winsize;

      float max_peak = 0;
      for (int j = 0; j < n_wins; j++) {
        uint32_t start = (p->N_zc - (j * p->N_cs)) % p->N_zc;
        uint32_t end   = start + winsize;
        if (end > p->deadzone) {
          end -= p->deadzone;
        }
        start += p->deadzone;
        p->peak_values[j] = 0;
        for (int k = start; k < end; k++) {
          if (p->corr[k] > p->peak_values[j]) {
            p->peak_values[j]  = p->corr[k];
            p->peak_offsets[j] = k - start;
            if (p->peak_values[j] > max_peak) {
              max_peak = p->peak_values[j];
            }
          }
        }
      }
      if (max_peak > p->detect_factor * corr_ave) {
        for (int j = 0; j < n_wins; j++) {
          if (p->peak_values[j] > p->detect_factor * corr_ave) {
            // printf("saving prach correlation\n");
            // memcpy(save_corr, p->corr, p->N_zc*sizeof(float));
            if (indices) {
              indices[*n_indices] = (i * n_wins) + j;
            }
            if (peak_to_avg) {
              peak_to_avg[*n_indices] = p->peak_values[j] / corr_ave;
            }
            if (t_offsets) {
              float corr = 1.8;
              if (p->peak_offsets[j] > 30) {
                corr = 1.9;
              }
              if (p->peak_offsets[j] > 250) {
                corr = 1.91;
              }

              t_offsets[*n_indices] = corr * p->peak_offsets[j] / (DELTA_F_RA * p->N_zc);
            }
            (*n_indices)++;
          }
        }
      }
    }

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

int srslte_prach_free(srslte_prach_t* p)
{
  free(p->prach_bins);
  free(p->corr_spec);
  free(p->corr);
  srslte_dft_plan_free(&p->ifft);
  free(p->ifft_in);
  free(p->ifft_out);
  srslte_dft_plan_free(&p->fft);
  srslte_dft_plan_free(&p->zc_fft);
  srslte_dft_plan_free(&p->zc_ifft);

  if (p->signal_fft) {
    free(p->signal_fft);
  }

  bzero(p, sizeof(srslte_prach_t));

  return 0;
}

int srslte_prach_print_seqs(srslte_prach_t* p)
{
  for (int i = 0; i < N_SEQS; i++) {
    FILE* f;
    char  str[32];
    sprintf(str, "prach_seq_%d.bin", i);
    f = fopen(str, "wb");
    fwrite(p->seqs[i], sizeof(cf_t), p->N_zc, f);
    fclose(f);
  }
  for (int i = 0; i < N_SEQS; i++) {
    FILE* f;
    char  str[32];
    sprintf(str, "prach_dft_seq_%d.bin", i);
    f = fopen(str, "wb");
    fwrite(p->dft_seqs[i], sizeof(cf_t), p->N_zc, f);
    fclose(f);
  }
  for (int i = 0; i < p->N_roots; i++) {
    FILE* f;
    char  str[32];
    sprintf(str, "prach_root_seq_%d.bin", i);
    f = fopen(str, "wb");
    fwrite(p->seqs[p->root_seqs_idx[i]], sizeof(cf_t), p->N_zc, f);
    fclose(f);
  }
  return 0;
}
