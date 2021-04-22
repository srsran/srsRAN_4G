/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/srsran.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srsran/phy/ch_estimation/refsignal_ul.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/dft_precoding.h"
#include "srsran/phy/phch/pusch.h"
#include "srsran/phy/phch/pusch_cfg.h"
#include "srsran/phy/phch/uci.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#define MAX_PUSCH_RE(cp) (2 * SRSRAN_CP_NSYMB(cp) * 12)

#define ACK_SNR_TH -1.0

/* Allocate/deallocate PUSCH RBs to the resource grid
 */
static int pusch_cp(srsran_pusch_t*       q,
                    srsran_pusch_grant_t* grant,
                    cf_t*                 input,
                    cf_t*                 output,
                    bool                  is_shortened,
                    bool                  advance_input)
{
  cf_t* in_ptr  = input;
  cf_t* out_ptr = output;

  uint32_t L_ref = 3;
  if (SRSRAN_CP_ISEXT(q->cell.cp)) {
    L_ref = 2;
  }
  for (uint32_t slot = 0; slot < 2; slot++) {
    uint32_t N_srs = 0;
    if (is_shortened && slot == 1) {
      N_srs = 1;
    }
    INFO("%s PUSCH %d PRB to index %d at slot %d",
         advance_input ? "Allocating" : "Getting",
         grant->L_prb,
         grant->n_prb_tilde[slot],
         slot);
    for (uint32_t l = 0; l < SRSRAN_CP_NSYMB(q->cell.cp) - N_srs; l++) {
      if (l != L_ref) {
        uint32_t idx = SRSRAN_RE_IDX(
            q->cell.nof_prb, l + slot * SRSRAN_CP_NSYMB(q->cell.cp), grant->n_prb_tilde[slot] * SRSRAN_NRE);
        if (advance_input) {
          out_ptr = &output[idx];
        } else {
          in_ptr = &input[idx];
        }
        memcpy(out_ptr, in_ptr, grant->L_prb * SRSRAN_NRE * sizeof(cf_t));
        if (advance_input) {
          in_ptr += grant->L_prb * SRSRAN_NRE;
        } else {
          out_ptr += grant->L_prb * SRSRAN_NRE;
        }
      }
    }
  }
  if (advance_input) {
    return in_ptr - input;
  } else {
    return out_ptr - output;
  }
}

static int pusch_put(srsran_pusch_t* q, srsran_pusch_grant_t* grant, cf_t* input, cf_t* output, bool is_shortened)
{
  return pusch_cp(q, grant, input, output, is_shortened, true);
}

static int pusch_get(srsran_pusch_t* q, srsran_pusch_grant_t* grant, cf_t* input, cf_t* output, bool is_shortened)
{
  return pusch_cp(q, grant, input, output, is_shortened, false);
}

/** Initializes the PDCCH transmitter and receiver */
static int pusch_init(srsran_pusch_t* q, uint32_t max_prb, bool is_ue)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    bzero(q, sizeof(srsran_pusch_t));
    ret       = SRSRAN_ERROR;
    q->max_re = max_prb * MAX_PUSCH_RE(SRSRAN_CP_NORM);

    INFO("Init PUSCH: %d PRBs", max_prb);

    for (srsran_mod_t i = 0; i < SRSRAN_MOD_NITEMS; i++) {
      if (srsran_modem_table_lte(&q->mod[i], i)) {
        goto clean;
      }
      srsran_modem_table_bytes(&q->mod[i]);
    }

    q->is_ue = is_ue;

    srsran_sch_init(&q->ul_sch);

    if (srsran_dft_precoding_init(&q->dft_precoding, max_prb, is_ue)) {
      ERROR("Error initiating DFT transform precoding");
      goto clean;
    }

    // Allocate int16 for reception (LLRs). Buffer casted to uint8_t for transmission
    q->q = srsran_vec_i16_malloc(q->max_re * srsran_mod_bits_x_symbol(SRSRAN_MOD_64QAM));
    if (!q->q) {
      goto clean;
    }

    // Allocate int16 for reception (LLRs). Buffer casted to uint8_t for transmission
    q->g = srsran_vec_i16_malloc(q->max_re * srsran_mod_bits_x_symbol(SRSRAN_MOD_64QAM));
    if (!q->g) {
      goto clean;
    }
    q->d = srsran_vec_cf_malloc(q->max_re);
    if (!q->d) {
      goto clean;
    }

    // Allocate eNb specific buffers
    if (!q->is_ue) {
      q->ce = srsran_vec_cf_malloc(q->max_re);
      if (!q->ce) {
        goto clean;
      }

      q->evm_buffer = srsran_evm_buffer_alloc(srsran_ra_tbs_from_idx(SRSRAN_RA_NOF_TBS_IDX - 1, 6));
      if (!q->evm_buffer) {
        ERROR("Allocating EVM buffer");
        goto clean;
      }
    }
    q->z = srsran_vec_cf_malloc(q->max_re);
    if (!q->z) {
      goto clean;
    }

    ret = SRSRAN_SUCCESS;
  }
clean:
  if (ret == SRSRAN_ERROR) {
    srsran_pusch_free(q);
  }
  return ret;
}

int srsran_pusch_init_ue(srsran_pusch_t* q, uint32_t max_prb)
{
  return pusch_init(q, max_prb, true);
}

int srsran_pusch_init_enb(srsran_pusch_t* q, uint32_t max_prb)
{
  return pusch_init(q, max_prb, false);
}

void srsran_pusch_free(srsran_pusch_t* q)
{
  int i;

  if (q->q) {
    free(q->q);
  }
  if (q->d) {
    free(q->d);
  }
  if (q->g) {
    free(q->g);
  }
  if (q->ce) {
    free(q->ce);
  }
  if (q->z) {
    free(q->z);
  }
  if (q->evm_buffer) {
    srsran_evm_free(q->evm_buffer);
  }
  srsran_dft_precoding_free(&q->dft_precoding);

  for (i = 0; i < SRSRAN_MOD_NITEMS; i++) {
    srsran_modem_table_free(&q->mod[i]);
  }
  srsran_sch_free(&q->ul_sch);

  bzero(q, sizeof(srsran_pusch_t));
}

int srsran_pusch_set_cell(srsran_pusch_t* q, srsran_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && srsran_cell_isvalid(&cell)) {
    // Resize EVM buffer, only for eNb
    if (!q->is_ue && q->evm_buffer) {
      srsran_evm_buffer_resize(q->evm_buffer, srsran_ra_tbs_from_idx(SRSRAN_RA_NOF_TBS_IDX - 1, cell.nof_prb));
    }

    q->cell   = cell;
    q->max_re = cell.nof_prb * MAX_PUSCH_RE(cell.cp);
    ret       = SRSRAN_SUCCESS;
  }
  return ret;
}

int srsran_pusch_assert_grant(const srsran_pusch_grant_t* grant)
{
  // Check for valid number of PRB
  if (!srsran_dft_precoding_valid_prb(grant->L_prb)) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Check RV limits, -1 is for RAR, 0-3 normal HARQ
  if (grant->tb.rv < -1 || grant->tb.rv > 3) {
    return SRSRAN_ERROR_OUT_OF_BOUNDS;
  }

  // Check for positive TBS
  if (grant->tb.tbs < 0) {
    return SRSRAN_ERROR_OUT_OF_BOUNDS;
  }

  return SRSRAN_SUCCESS;
}

/** Converts the PUSCH data bits to symbols mapped to the slot ready for transmission
 */
int srsran_pusch_encode(srsran_pusch_t*      q,
                        srsran_ul_sf_cfg_t*  sf,
                        srsran_pusch_cfg_t*  cfg,
                        srsran_pusch_data_t* data,
                        cf_t*                sf_symbols)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && cfg != NULL) {
    /* Limit UL modulation if not supported by the UE or disabled by higher layers */
    if (!cfg->enable_64qam) {
      if (cfg->grant.tb.mod >= SRSRAN_MOD_64QAM) {
        cfg->grant.tb.mod      = SRSRAN_MOD_16QAM;
        cfg->grant.tb.nof_bits = cfg->grant.nof_re * srsran_mod_bits_x_symbol(SRSRAN_MOD_16QAM);
      }
    }

    if (cfg->grant.nof_re > q->max_re) {
      ERROR("Error too many RE per subframe (%d). PUSCH configured for %d RE (%d PRB)",
            cfg->grant.nof_re,
            q->max_re,
            q->cell.nof_prb);
      return SRSRAN_ERROR_INVALID_INPUTS;
    }

    int err = srsran_pusch_assert_grant(&cfg->grant);
    if (err != SRSRAN_SUCCESS) {
      return err;
    }

    INFO("Encoding PUSCH SF: %d, Mod %s, RNTI: %d, TBS: %d, NofRE: %d, NofSymbols=%d, NofBitsE: %d, rv_idx: %d",
         sf->tti % 10,
         srsran_mod_string(cfg->grant.tb.mod),
         cfg->rnti,
         cfg->grant.tb.tbs,
         cfg->grant.nof_re,
         cfg->grant.nof_symb,
         cfg->grant.tb.nof_bits,
         cfg->grant.tb.rv);

    bzero(q->q, cfg->grant.tb.nof_bits);
    if ((ret = srsran_ulsch_encode(&q->ul_sch, cfg, data->ptr, &data->uci, q->g, q->q)) < 0) {
      ERROR("Error encoding TB");
      return SRSRAN_ERROR;
    }

    uint32_t nof_ri_ack_bits = (uint32_t)ret;

    // Run scrambling
    srsran_sequence_pusch_apply_pack((uint8_t*)q->q,
                                     (uint8_t*)q->q,
                                     cfg->rnti,
                                     2 * (sf->tti % SRSRAN_NOF_SF_X_FRAME),
                                     q->cell.id,
                                     cfg->grant.tb.nof_bits);

    // Correct UCI placeholder/repetition bits
    uint8_t* d = q->q;
    for (int i = 0; i < nof_ri_ack_bits; i++) {
      if (q->ul_sch.ack_ri_bits[i].type == UCI_BIT_PLACEHOLDER) {
        d[q->ul_sch.ack_ri_bits[i].position / 8] |= (1 << (7 - q->ul_sch.ack_ri_bits[i].position % 8));
      } else if (q->ul_sch.ack_ri_bits[i].type == UCI_BIT_REPETITION) {
        if (q->ul_sch.ack_ri_bits[i].position > 1) {
          uint32_t p   = q->ul_sch.ack_ri_bits[i].position;
          uint8_t  bit = d[(p - 1) / 8] & (1 << (7 - (p - 1) % 8));
          if (bit) {
            d[p / 8] |= 1 << (7 - p % 8);
          } else {
            d[p / 8] &= ~(1 << (7 - p % 8));
          }
        }
      }
    }

    // Bit mapping
    srsran_mod_modulate_bytes(&q->mod[cfg->grant.tb.mod], (uint8_t*)q->q, q->d, cfg->grant.tb.nof_bits);

    // DFT precoding
    srsran_dft_precoding(&q->dft_precoding, q->d, q->z, cfg->grant.L_prb, cfg->grant.nof_symb);

    // Mapping to resource elements
    uint32_t n = pusch_put(q, &cfg->grant, q->z, sf_symbols, sf->shortened);
    if (n != cfg->grant.nof_re) {
      ERROR("Error trying to allocate %d symbols but %d were allocated (tti=%d, short=%d, L=%d)",
            cfg->grant.nof_re,
            n,
            sf->tti,
            sf->shortened,
            cfg->grant.L_prb);
      return SRSRAN_ERROR;
    }

    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

/** Decodes the PUSCH from the received symbols
 */
int srsran_pusch_decode(srsran_pusch_t*        q,
                        srsran_ul_sf_cfg_t*    sf,
                        srsran_pusch_cfg_t*    cfg,
                        srsran_chest_ul_res_t* channel,
                        cf_t*                  sf_symbols,
                        srsran_pusch_res_t*    out)
{
  int      ret = SRSRAN_ERROR_INVALID_INPUTS;
  uint32_t n;

  if (q != NULL && sf_symbols != NULL && out != NULL && cfg != NULL) {
    struct timeval t[3];
    if (cfg->meas_time_en) {
      gettimeofday(&t[1], NULL);
    }

    /* Limit UL modulation if not supported by the UE or disabled by higher layers */
    if (!cfg->enable_64qam) {
      if (cfg->grant.tb.mod >= SRSRAN_MOD_64QAM) {
        cfg->grant.tb.mod      = SRSRAN_MOD_16QAM;
        cfg->grant.tb.nof_bits = cfg->grant.nof_re * srsran_mod_bits_x_symbol(SRSRAN_MOD_16QAM);
      }
    }

    INFO("Decoding PUSCH SF: %d, Mod %s, NofBits: %d, NofRE: %d, NofSymbols=%d, NofBitsE: %d, rv_idx: %d",
         sf->tti % 10,
         srsran_mod_string(cfg->grant.tb.mod),
         cfg->grant.tb.tbs,
         cfg->grant.nof_re,
         cfg->grant.nof_symb,
         cfg->grant.tb.nof_bits,
         cfg->grant.tb.rv);

    /* extract symbols */
    n = pusch_get(q, &cfg->grant, sf_symbols, q->d, sf->shortened);
    if (n != cfg->grant.nof_re) {
      ERROR("Error expecting %d symbols but got %d", cfg->grant.nof_re, n);
      return SRSRAN_ERROR;
    }

    // Measure Energy per Resource Element
    if (cfg->meas_epre_en) {
      out->epre_dbfs = srsran_convert_power_to_dB(srsran_vec_avg_power_cf(q->d, n));
    } else {
      out->epre_dbfs = NAN;
    }

    /* extract channel estimates */
    n = pusch_get(q, &cfg->grant, channel->ce, q->ce, sf->shortened);
    if (n != cfg->grant.nof_re) {
      ERROR("Error expecting %d symbols but got %d", cfg->grant.nof_re, n);
      return SRSRAN_ERROR;
    }

    // Equalization
    srsran_predecoding_single(q->d, q->ce, q->z, NULL, cfg->grant.nof_re, 1.0f, channel->noise_estimate);

    // DFT predecoding
    srsran_dft_precoding(&q->dft_precoding, q->z, q->d, cfg->grant.L_prb, cfg->grant.nof_symb);

    // Soft demodulation
    if (q->llr_is_8bit) {
      srsran_demod_soft_demodulate_b(cfg->grant.tb.mod, q->d, q->q, cfg->grant.nof_re);
    } else {
      srsran_demod_soft_demodulate_s(cfg->grant.tb.mod, q->d, q->q, cfg->grant.nof_re);
    }

    if (cfg->meas_evm_en && q->evm_buffer) {
      if (q->llr_is_8bit) {
        out->evm = srsran_evm_run_b(q->evm_buffer, &q->mod[cfg->grant.tb.mod], q->d, q->q, cfg->grant.tb.nof_bits);
      } else {
        out->evm = srsran_evm_run_s(q->evm_buffer, &q->mod[cfg->grant.tb.mod], q->d, q->q, cfg->grant.tb.nof_bits);
      }
    } else {
      out->evm = NAN;
    }

    // Descrambling
    if (q->llr_is_8bit) {
      srsran_sequence_pusch_apply_c(
          q->q, q->q, cfg->rnti, 2 * (sf->tti % SRSRAN_NOF_SF_X_FRAME), q->cell.id, cfg->grant.tb.nof_bits);
    } else {
      srsran_sequence_pusch_apply_s(
          q->q, q->q, cfg->rnti, 2 * (sf->tti % SRSRAN_NOF_SF_X_FRAME), q->cell.id, cfg->grant.tb.nof_bits);
    }

    // Generate packed sequence for UCI decoder
    uint8_t* c = (uint8_t*)q->z; // Reuse Z
    srsran_sequence_pusch_gen_unpack(
        c, cfg->rnti, 2 * (sf->tti % SRSRAN_NOF_SF_X_FRAME), q->cell.id, cfg->grant.tb.nof_bits);

    // Set max number of iterations
    srsran_sch_set_max_noi(&q->ul_sch, cfg->max_nof_iterations);

    // Decode
    ret      = srsran_ulsch_decode(&q->ul_sch, cfg, q->q, q->g, c, out->data, &out->uci);
    out->crc = (ret == 0);

    // Save number of iterations
    out->avg_iterations_block = q->ul_sch.avg_iterations;

    // Save O_cqi for power control
    cfg->last_O_cqi = srsran_cqi_size(&cfg->uci_cfg.cqi);
    ret             = SRSRAN_SUCCESS;

    if (cfg->meas_time_en) {
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      cfg->meas_time_value = t[0].tv_usec;
    }
  }

  return ret;
}

uint32_t srsran_pusch_grant_tx_info(srsran_pusch_grant_t* grant,
                                    srsran_uci_cfg_t*     uci_cfg,
                                    srsran_uci_value_t*   uci_data,
                                    char*                 str,
                                    uint32_t              str_len)
{
  uint32_t len = srsran_ra_ul_info(grant, str, str_len);

  if (uci_data) {
    len += srsran_uci_data_info(uci_cfg, uci_data, &str[len], str_len - len);
  }

  return len;
}

uint32_t srsran_pusch_tx_info(srsran_pusch_cfg_t* cfg, srsran_uci_value_t* uci_data, char* str, uint32_t str_len)
{
  uint32_t len = srsran_print_check(str, str_len, 0, "rnti=0x%x", cfg->rnti);

  len += srsran_pusch_grant_tx_info(&cfg->grant, &cfg->uci_cfg, uci_data, &str[len], str_len - len);

  if (cfg->meas_time_en) {
    len = srsran_print_check(str, str_len, len, ", t=%d us", cfg->meas_time_value);
  }
  return len;
}

uint32_t srsran_pusch_rx_info(srsran_pusch_cfg_t*    cfg,
                              srsran_pusch_res_t*    res,
                              srsran_chest_ul_res_t* chest_res,
                              char*                  str,
                              uint32_t               str_len)
{
  uint32_t len = srsran_print_check(str, str_len, 0, "rnti=0x%x", cfg->rnti);

  len += srsran_ra_ul_info(&cfg->grant, &str[len], str_len);

  len = srsran_print_check(
      str, str_len, len, ", crc=%s, avg_iter=%.1f", res->crc ? "OK" : "KO", res->avg_iterations_block);

  len += srsran_uci_data_info(&cfg->uci_cfg, &res->uci, &str[len], str_len - len);

  len = srsran_print_check(str, str_len, len, ", snr=%.1f dB", chest_res->snr_db);

  // Append Energy Per Resource Element
  if (cfg->meas_epre_en) {
    len = srsran_print_check(str, str_len, len, ", epre=%.1f dBfs", res->epre_dbfs);
  }

  // Append Time Aligment information if available
  if (cfg->meas_ta_en) {
    len = srsran_print_check(str, str_len, len, ", ta=%.1f us", chest_res->ta_us);
  }

  // Append CFO information if available
  if (!isnan(chest_res->cfo_hz)) {
    len = srsran_print_check(str, str_len, len, ", cfo=%.1f hz", chest_res->cfo_hz);
  }

  // Append EVM measurement if available
  if (cfg->meas_evm_en) {
    len = srsran_print_check(str, str_len, len, ", evm=%.1f %%", res->evm * 100);
  }

  if (cfg->meas_time_en) {
    len = srsran_print_check(str, str_len, len, ", t=%d us", cfg->meas_time_value);
  }
  return len;
}
