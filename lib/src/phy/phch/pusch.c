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
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/phy/ch_estimation/refsignal_ul.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/dft_precoding.h"
#include "srslte/phy/phch/pusch.h"
#include "srslte/phy/phch/pusch_cfg.h"
#include "srslte/phy/phch/uci.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define MAX_PUSCH_RE(cp) (2 * SRSLTE_CP_NSYMB(cp) * 12)

#define ACK_SNR_TH -1.0

/* Allocate/deallocate PUSCH RBs to the resource grid
 */
static int pusch_cp(srslte_pusch_t*       q,
                    srslte_pusch_grant_t* grant,
                    cf_t*                 input,
                    cf_t*                 output,
                    bool                  is_shortened,
                    bool                  advance_input)
{
  cf_t* in_ptr  = input;
  cf_t* out_ptr = output;

  uint32_t L_ref = 3;
  if (SRSLTE_CP_ISEXT(q->cell.cp)) {
    L_ref = 2;
  }
  for (uint32_t slot = 0; slot < 2; slot++) {
    uint32_t N_srs = 0;
    if (is_shortened && slot == 1) {
      N_srs = 1;
    }
    INFO("%s PUSCH %d PRB to index %d at slot %d\n",
         advance_input ? "Allocating" : "Getting",
         grant->L_prb,
         grant->n_prb_tilde[slot],
         slot);
    for (uint32_t l = 0; l < SRSLTE_CP_NSYMB(q->cell.cp) - N_srs; l++) {
      if (l != L_ref) {
        uint32_t idx = SRSLTE_RE_IDX(
            q->cell.nof_prb, l + slot * SRSLTE_CP_NSYMB(q->cell.cp), grant->n_prb_tilde[slot] * SRSLTE_NRE);
        if (advance_input) {
          out_ptr = &output[idx];
        } else {
          in_ptr = &input[idx];
        }
        memcpy(out_ptr, in_ptr, grant->L_prb * SRSLTE_NRE * sizeof(cf_t));
        if (advance_input) {
          in_ptr += grant->L_prb * SRSLTE_NRE;
        } else {
          out_ptr += grant->L_prb * SRSLTE_NRE;
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

static int pusch_put(srslte_pusch_t* q, srslte_pusch_grant_t* grant, cf_t* input, cf_t* output, bool is_shortened)
{
  return pusch_cp(q, grant, input, output, is_shortened, true);
}

static int pusch_get(srslte_pusch_t* q, srslte_pusch_grant_t* grant, cf_t* input, cf_t* output, bool is_shortened)
{
  return pusch_cp(q, grant, input, output, is_shortened, false);
}

/** Initializes the PDCCH transmitter and receiver */
static int pusch_init(srslte_pusch_t* q, uint32_t max_prb, bool is_ue)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {

    bzero(q, sizeof(srslte_pusch_t));
    ret       = SRSLTE_ERROR;
    q->max_re = max_prb * MAX_PUSCH_RE(SRSLTE_CP_NORM);

    INFO("Init PUSCH: %d PRBs\n", max_prb);

    for (srslte_mod_t i = 0; i < SRSLTE_MOD_NITEMS; i++) {
      if (srslte_modem_table_lte(&q->mod[i], i)) {
        goto clean;
      }
      srslte_modem_table_bytes(&q->mod[i]);
    }

    q->is_ue = is_ue;

    q->users = calloc(sizeof(srslte_pusch_user_t*), q->is_ue ? 1 : (1 + SRSLTE_SIRNTI));
    if (!q->users) {
      perror("malloc");
      goto clean;
    }

    if (srslte_sequence_init(&q->tmp_seq, q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM))) {
      goto clean;
    }

    srslte_sch_init(&q->ul_sch);

    if (srslte_dft_precoding_init(&q->dft_precoding, max_prb, is_ue)) {
      ERROR("Error initiating DFT transform precoding\n");
      goto clean;
    }

    // Allocate int16 for reception (LLRs). Buffer casted to uint8_t for transmission
    q->q = srslte_vec_i16_malloc(q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM));
    if (!q->q) {
      goto clean;
    }

    // Allocate int16 for reception (LLRs). Buffer casted to uint8_t for transmission
    q->g = srslte_vec_i16_malloc(q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM));
    if (!q->g) {
      goto clean;
    }
    q->d = srslte_vec_cf_malloc(q->max_re);
    if (!q->d) {
      goto clean;
    }

    // Allocate eNb specific buffers
    if (!q->is_ue) {
      q->ce = srslte_vec_cf_malloc(q->max_re);
      if (!q->ce) {
        goto clean;
      }

      q->evm_buffer = srslte_evm_buffer_alloc(6);
      if (!q->evm_buffer) {
        ERROR("Allocating EVM buffer\n");
        goto clean;
      }
    }
    q->z = srslte_vec_cf_malloc(q->max_re);
    if (!q->z) {
      goto clean;
    }

    ret = SRSLTE_SUCCESS;
  }
clean:
  if (ret == SRSLTE_ERROR) {
    srslte_pusch_free(q);
  }
  return ret;
}

int srslte_pusch_init_ue(srslte_pusch_t* q, uint32_t max_prb)
{
  return pusch_init(q, max_prb, true);
}

int srslte_pusch_init_enb(srslte_pusch_t* q, uint32_t max_prb)
{
  return pusch_init(q, max_prb, false);
}

void srslte_pusch_free(srslte_pusch_t* q)
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
    srslte_evm_free(q->evm_buffer);
  }
  srslte_dft_precoding_free(&q->dft_precoding);

  if (q->users) {
    if (q->is_ue) {
      srslte_pusch_free_rnti(q, 0);
    } else {
      for (int rnti = 0; rnti <= SRSLTE_SIRNTI; rnti++) {
        srslte_pusch_free_rnti(q, rnti);
      }
    }
    free(q->users);
  }

  srslte_sequence_free(&q->tmp_seq);

  for (i = 0; i < SRSLTE_MOD_NITEMS; i++) {
    srslte_modem_table_free(&q->mod[i]);
  }
  srslte_sch_free(&q->ul_sch);

  bzero(q, sizeof(srslte_pusch_t));
}

int srslte_pusch_set_cell(srslte_pusch_t* q, srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_cell_isvalid(&cell)) {

    // Resize EVM buffer, only for eNb
    if (!q->is_ue && q->evm_buffer) {
      srslte_evm_buffer_resize(q->evm_buffer, cell.nof_prb);
    }

    q->cell   = cell;
    q->max_re = cell.nof_prb * MAX_PUSCH_RE(cell.cp);
    ret       = SRSLTE_SUCCESS;
  }
  return ret;
}

/* Precalculate the PUSCH scramble sequences for a given RNTI. This function takes a while
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 * For the connection procedure, use srslte_pusch_encode() functions */
int srslte_pusch_set_rnti(srslte_pusch_t* q, uint16_t rnti)
{
  uint32_t i;

  uint32_t rnti_idx = q->is_ue ? 0 : rnti;

  if (!q->users[rnti_idx] || q->is_ue) {
    if (!q->users[rnti_idx]) {
      q->users[rnti_idx] = calloc(1, sizeof(srslte_pusch_user_t));
      if (!q->users[rnti_idx]) {
        perror("calloc");
        return -1;
      }
    }
    q->users[rnti_idx]->sequence_generated = false;
    for (i = 0; i < SRSLTE_NOF_SF_X_FRAME; i++) {
      if (srslte_sequence_pusch(&q->users[rnti_idx]->seq[i],
                                rnti,
                                2 * i,
                                q->cell.id,
                                q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM))) {
        ERROR("Error initializing PUSCH scrambling sequence\n");
        srslte_pusch_free_rnti(q, rnti);
        return SRSLTE_ERROR;
      }
    }
    q->ue_rnti                             = rnti;
    q->users[rnti_idx]->cell_id            = q->cell.id;
    q->users[rnti_idx]->sequence_generated = true;
  } else {
    ERROR("Error generating PUSCH sequence: rnti=0x%x already generated\n", rnti);
  }
  return SRSLTE_SUCCESS;
}

void srslte_pusch_free_rnti(srslte_pusch_t* q, uint16_t rnti)
{

  uint32_t rnti_idx = q->is_ue ? 0 : rnti;

  if (q->users[rnti_idx]) {
    for (int i = 0; i < SRSLTE_NOF_SF_X_FRAME; i++) {
      srslte_sequence_free(&q->users[rnti_idx]->seq[i]);
    }
    free(q->users[rnti_idx]);
    q->users[rnti_idx] = NULL;
    q->ue_rnti         = 0;
  }
}

static srslte_sequence_t* get_user_sequence(srslte_pusch_t* q, uint16_t rnti, uint32_t sf_idx, uint32_t len)
{
  uint32_t rnti_idx = q->is_ue ? 0 : rnti;

  if (SRSLTE_RNTI_ISUSER(rnti)) {
    // The scrambling sequence is pregenerated for all RNTIs in the eNodeB but only for C-RNTI in the UE
    if (q->users[rnti_idx] && q->users[rnti_idx]->sequence_generated && q->users[rnti_idx]->cell_id == q->cell.id &&
        (!q->is_ue || q->ue_rnti == rnti)) {
      return &q->users[rnti_idx]->seq[sf_idx];
    } else {
      if (srslte_sequence_pusch(&q->tmp_seq, rnti, 2 * sf_idx, q->cell.id, len)) {
        ERROR("Error generating temporal scrambling sequence\n");
        return NULL;
      }
      return &q->tmp_seq;
    }
  } else {
    ERROR("Invalid RNTI=0x%x\n", rnti);
    return NULL;
  }
}

/** Converts the PUSCH data bits to symbols mapped to the slot ready for transmission
 */
int srslte_pusch_encode(srslte_pusch_t*      q,
                        srslte_ul_sf_cfg_t*  sf,
                        srslte_pusch_cfg_t*  cfg,
                        srslte_pusch_data_t* data,
                        cf_t*                sf_symbols)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && cfg != NULL) {

    /* Limit UL modulation if not supported by the UE or disabled by higher layers */
    if (!cfg->enable_64qam) {
      if (cfg->grant.tb.mod >= SRSLTE_MOD_64QAM) {
        cfg->grant.tb.mod      = SRSLTE_MOD_16QAM;
        cfg->grant.tb.nof_bits = cfg->grant.nof_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_16QAM);
      }
    }

    if (cfg->grant.nof_re > q->max_re) {
      ERROR("Error too many RE per subframe (%d). PUSCH configured for %d RE (%d PRB)\n",
            cfg->grant.nof_re,
            q->max_re,
            q->cell.nof_prb);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

    if (!srslte_dft_precoding_valid_prb(cfg->grant.L_prb)) {
      ERROR("Error encoding PUSCH: invalid L_prb=%d\n", cfg->grant.L_prb);
      return -1;
    }

    if (cfg->grant.tb.rv < 0 || cfg->grant.tb.rv > 3) {
      ERROR("Error encoding PUSCH: invalid rv=%d\n", cfg->grant.tb.rv);
      return -1;
    }

    if (cfg->grant.tb.tbs < 0) {
      ERROR("Error encoding PUSCH: invalid tbs=%d\n", cfg->grant.tb.tbs);
      return -1;
    }

    INFO("Encoding PUSCH SF: %d, Mod %s, RNTI: %d, TBS: %d, NofRE: %d, NofSymbols=%d, NofBitsE: %d, rv_idx: %d\n",
         sf->tti % 10,
         srslte_mod_string(cfg->grant.tb.mod),
         cfg->rnti,
         cfg->grant.tb.tbs,
         cfg->grant.nof_re,
         cfg->grant.nof_symb,
         cfg->grant.tb.nof_bits,
         cfg->grant.tb.rv);

    bzero(q->q, cfg->grant.tb.nof_bits);
    if ((ret = srslte_ulsch_encode(&q->ul_sch, cfg, data->ptr, &data->uci, q->g, q->q)) < 0) {
      ERROR("Error encoding TB\n");
      return SRSLTE_ERROR;
    }

    uint32_t nof_ri_ack_bits = (uint32_t)ret;

    // Generate scrambling sequence if not pre-generated
    srslte_sequence_t* seq = get_user_sequence(q, cfg->rnti, sf->tti % 10, cfg->grant.tb.nof_bits);
    if (!seq) {
      ERROR("Error getting user sequence for rnti=0x%x\n", cfg->rnti);
      return -1;
    }

    // Run scrambling
    srslte_scrambling_bytes(seq, (uint8_t*)q->q, cfg->grant.tb.nof_bits);

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
    srslte_mod_modulate_bytes(&q->mod[cfg->grant.tb.mod], (uint8_t*)q->q, q->d, cfg->grant.tb.nof_bits);

    // DFT precoding
    srslte_dft_precoding(&q->dft_precoding, q->d, q->z, cfg->grant.L_prb, cfg->grant.nof_symb);

    // Mapping to resource elements
    uint32_t n = pusch_put(q, &cfg->grant, q->z, sf_symbols, sf->shortened);
    if (n != cfg->grant.nof_re) {
      ERROR("Error trying to allocate %d symbols but %d were allocated (tti=%d, short=%d, L=%d)\n",
            cfg->grant.nof_re,
            n,
            sf->tti,
            sf->shortened,
            cfg->grant.L_prb);
      return SRSLTE_ERROR;
    }

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

/** Decodes the PUSCH from the received symbols
 */
int srslte_pusch_decode(srslte_pusch_t*        q,
                        srslte_ul_sf_cfg_t*    sf,
                        srslte_pusch_cfg_t*    cfg,
                        srslte_chest_ul_res_t* channel,
                        cf_t*                  sf_symbols,
                        srslte_pusch_res_t*    out)
{
  int      ret = SRSLTE_ERROR_INVALID_INPUTS;
  uint32_t n;

  if (q != NULL && sf_symbols != NULL && out != NULL && cfg != NULL) {

    struct timeval t[3];
    if (cfg->meas_time_en) {
      gettimeofday(&t[1], NULL);
    }

    /* Limit UL modulation if not supported by the UE or disabled by higher layers */
    if (!cfg->enable_64qam) {
      if (cfg->grant.tb.mod >= SRSLTE_MOD_64QAM) {
        cfg->grant.tb.mod      = SRSLTE_MOD_16QAM;
        cfg->grant.tb.nof_bits = cfg->grant.nof_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_16QAM);
      }
    }

    INFO("Decoding PUSCH SF: %d, Mod %s, NofBits: %d, NofRE: %d, NofSymbols=%d, NofBitsE: %d, rv_idx: %d\n",
         sf->tti % 10,
         srslte_mod_string(cfg->grant.tb.mod),
         cfg->grant.tb.tbs,
         cfg->grant.nof_re,
         cfg->grant.nof_symb,
         cfg->grant.tb.nof_bits,
         cfg->grant.tb.rv);

    /* extract symbols */
    n = pusch_get(q, &cfg->grant, sf_symbols, q->d, sf->shortened);
    if (n != cfg->grant.nof_re) {
      ERROR("Error expecting %d symbols but got %d\n", cfg->grant.nof_re, n);
      return SRSLTE_ERROR;
    }

    // Measure Energy per Resource Element
    if (cfg->meas_epre_en) {
      out->epre_dbfs = srslte_convert_power_to_dB(srslte_vec_avg_power_cf(q->d, n));
    } else {
      out->epre_dbfs = NAN;
    }

    /* extract channel estimates */
    n = pusch_get(q, &cfg->grant, channel->ce, q->ce, sf->shortened);
    if (n != cfg->grant.nof_re) {
      ERROR("Error expecting %d symbols but got %d\n", cfg->grant.nof_re, n);
      return SRSLTE_ERROR;
    }

    // Equalization
    srslte_predecoding_single(q->d, q->ce, q->z, NULL, cfg->grant.nof_re, 1.0f, channel->noise_estimate);

    // DFT predecoding
    srslte_dft_precoding(&q->dft_precoding, q->z, q->d, cfg->grant.L_prb, cfg->grant.nof_symb);

    // Soft demodulation
    if (q->llr_is_8bit) {
      srslte_demod_soft_demodulate_b(cfg->grant.tb.mod, q->d, q->q, cfg->grant.nof_re);
    } else {
      srslte_demod_soft_demodulate_s(cfg->grant.tb.mod, q->d, q->q, cfg->grant.nof_re);
    }

    if (cfg->meas_evm_en && q->evm_buffer) {
      if (q->llr_is_8bit) {
        out->evm = srslte_evm_run_b(q->evm_buffer, &q->mod[cfg->grant.tb.mod], q->d, q->q, cfg->grant.tb.nof_bits);
      } else {
        out->evm = srslte_evm_run_s(q->evm_buffer, &q->mod[cfg->grant.tb.mod], q->d, q->q, cfg->grant.tb.nof_bits);
      }
    } else {
      out->evm = NAN;
    }

    // Generate scrambling sequence if not pre-generated
    srslte_sequence_t* seq = get_user_sequence(q, cfg->rnti, sf->tti % 10, cfg->grant.tb.nof_bits);
    if (!seq) {
      ERROR("Error getting user sequence for rnti=0x%x\n", cfg->rnti);
      return -1;
    }

    // Descrambling
    if (q->llr_is_8bit) {
      srslte_scrambling_sb_offset(seq, q->q, 0, cfg->grant.tb.nof_bits);
    } else {
      srslte_scrambling_s_offset(seq, q->q, 0, cfg->grant.tb.nof_bits);
    }

    // Decode
    ret      = srslte_ulsch_decode(&q->ul_sch, cfg, q->q, q->g, seq->c, out->data, &out->uci);
    out->crc = (ret == 0);

    // Accept ACK only if SNR is above threshold
    out->uci.ack.valid        = channel->snr_db > ACK_SNR_TH;
    out->avg_iterations_block = q->ul_sch.avg_iterations;

    // Save O_cqi for power control
    cfg->last_O_cqi = srslte_cqi_size(&cfg->uci_cfg.cqi);
    ret             = SRSLTE_SUCCESS;

    if (cfg->meas_time_en) {
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      cfg->meas_time_value = t[0].tv_usec;
    }
  }

  return ret;
}

uint32_t srslte_pusch_grant_tx_info(srslte_pusch_grant_t* grant,
                                    srslte_uci_cfg_t*     uci_cfg,
                                    srslte_uci_value_t*   uci_data,
                                    char*                 str,
                                    uint32_t              str_len)
{

  uint32_t len = srslte_ra_ul_info(grant, str, str_len);

  if (uci_data) {
    len += srslte_uci_data_info(uci_cfg, uci_data, &str[len], str_len - len);
  }

  return len;
}

uint32_t srslte_pusch_tx_info(srslte_pusch_cfg_t* cfg, srslte_uci_value_t* uci_data, char* str, uint32_t str_len)
{

  uint32_t len = srslte_print_check(str, str_len, 0, "rnti=0x%x", cfg->rnti);

  len += srslte_pusch_grant_tx_info(&cfg->grant, &cfg->uci_cfg, uci_data, &str[len], str_len - len);

  if (cfg->meas_time_en) {
    len = srslte_print_check(str, str_len, len, ", t=%d us", cfg->meas_time_value);
  }
  return len;
}

uint32_t srslte_pusch_rx_info(srslte_pusch_cfg_t*    cfg,
                              srslte_pusch_res_t*    res,
                              srslte_chest_ul_res_t* chest_res,
                              char*                  str,
                              uint32_t               str_len)
{

  uint32_t len = srslte_print_check(str, str_len, 0, "rnti=0x%x", cfg->rnti);

  len += srslte_ra_ul_info(&cfg->grant, &str[len], str_len);

  len = srslte_print_check(
      str, str_len, len, ", crc=%s, avg_iter=%.1f", res->crc ? "OK" : "KO", res->avg_iterations_block);

  len += srslte_uci_data_info(&cfg->uci_cfg, &res->uci, &str[len], str_len - len);

  len = srslte_print_check(str, str_len, len, ", snr=%.1f dB", chest_res->snr_db);

  // Append Energy Per Resource Element
  if (cfg->meas_epre_en) {
    len = srslte_print_check(str, str_len, len, ", epre=%.1f dBfs", res->epre_dbfs);
  }

  // Append Time Aligment information if available
  if (cfg->meas_ta_en) {
    len = srslte_print_check(str, str_len, len, ", ta=%.1f us", chest_res->ta_us);
  }

  // Append EVM measurement if available
  if (cfg->meas_evm_en) {
    len = srslte_print_check(str, str_len, len, ", evm=%.1f %%", res->evm * 100);
  }

  if (cfg->meas_time_en) {
    len = srslte_print_check(str, str_len, len, ", t=%d us", cfg->meas_time_value);
  }
  return len;
}
