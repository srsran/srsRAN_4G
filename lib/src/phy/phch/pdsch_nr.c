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
#include "srslte/phy/phch/pdsch_nr.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/phch/ra_nr.h"

int pdsch_nr_init_common(srslte_pdsch_nr_t* q)
{
  for (srslte_mod_t mod = SRSLTE_MOD_BPSK; mod < SRSLTE_MOD_NITEMS; mod++) {
    if (srslte_modem_table_lte(&q->modem_tables[mod], mod) < SRSLTE_SUCCESS) {
      ERROR("Error initialising modem table for %s\n", srslte_mod_string(mod));
      return SRSLTE_ERROR;
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_nr_init_tx(srslte_pdsch_nr_t* q)
{

  if (q == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (pdsch_nr_init_common(q) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_sch_nr_init_tx(&q->sch)) {
    ERROR("Initialising SCH\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_nr_init_rx(srslte_pdsch_nr_t* q)
{

  if (q == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (pdsch_nr_init_common(q) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  srslte_sch_nr_decoder_cfg_t decoder_cfg = {};
  decoder_cfg.disable_simd                = true;
  if (srslte_sch_nr_init_rx(&q->sch, &decoder_cfg)) {
    ERROR("Initialising SCH\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_nr_set_carrier(srslte_pdsch_nr_t*         q,
                                const srslte_carrier_nr_t* carrier,
                                const srslte_sch_cfg_t*    sch_cfg)
{
  // Set carrier
  q->carrier = *carrier;

  // Reallocate symbols if necessary
  if (q->max_layers < sch_cfg->max_mimo_layers || q->max_prb < carrier->nof_prb) {
    q->max_layers = sch_cfg->max_mimo_layers;
    q->max_prb    = carrier->nof_prb;

    // Free current allocations
    for (uint32_t i = 0; i < SRSLTE_MAX_LAYERS_NR; i++) {
      if (q->x[i] != NULL) {
        free(q->x[i]);
      }
    }

    // Allocate for new sizes
    for (uint32_t i = 0; i < q->max_layers; i++) {
      q->x[i] = srslte_vec_cf_malloc(SRSLTE_SLOT_LEN_RE_NR(q->max_prb));
      if (q->x[i] == NULL) {
        ERROR("Malloc");
        return SRSLTE_ERROR;
      }
    }
  }

  // Allocate code words according to table 7.3.1.3-1
  uint32_t max_cw = (q->max_layers > 5) ? 2 : 1;
  if (q->max_cw < max_cw) {
    q->max_cw = max_cw;

    for (uint32_t i = 0; i < max_cw; i++) {
      if (q->b[i] == NULL) {
        q->b[i] = srslte_vec_u8_malloc(SRSLTE_SLOT_MAX_LEN_RE_NR);
        if (q->b[i] == NULL) {
          ERROR("Malloc");
          return SRSLTE_ERROR;
        }
      }

      if (q->d[i] == NULL) {
        q->d[i] = srslte_vec_cf_malloc(SRSLTE_SLOT_MAX_LEN_RE_NR);
        if (q->d[i] == NULL) {
          ERROR("Malloc");
          return SRSLTE_ERROR;
        }
      }
    }
  }

  // Set carrier in SCH
  if (srslte_sch_nr_set_carrier(&q->sch, carrier) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

void srslte_pdsch_nr_free(srslte_pdsch_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  for (uint32_t cw = 0; cw < SRSLTE_MAX_CODEWORDS; cw++) {
    if (q->b[cw]) {
      free(q->b[cw]);
    }

    if (q->d[cw]) {
      free(q->d[cw]);
    }
  }

  srslte_sch_nr_free(&q->sch);

  for (uint32_t i = 0; i < SRSLTE_MAX_LAYERS_NR; i++) {
    if (q->x[i]) {
      free(q->x[i]);
    }
  }

  for (srslte_mod_t mod = SRSLTE_MOD_BPSK; mod < SRSLTE_MOD_NITEMS; mod++) {
    srslte_modem_table_free(&q->modem_tables[mod]);
  }
}

/**
 * @brief copies a number of countiguous Resource Elements
 * @param sf_symbols slot symbols in frequency domain
 * @param symbols resource elements
 * @param count number of resource elements to copy
 * @param put Direction, symbols are copied into sf_symbols if put is true, otherwise sf_symbols are copied into symbols
 */
static void srslte_pdsch_re_cp(cf_t* sf_symbols, cf_t* symbols, uint32_t count, bool put)
{
  if (put) {
    srslte_vec_cf_copy(sf_symbols, symbols, count);
  } else {
    srslte_vec_cf_copy(symbols, sf_symbols, count);
  }
}

static uint32_t srslte_pdsch_nr_cp_dmrs_type1(const srslte_pdsch_nr_t*       q,
                                              const srslte_pdsch_grant_nr_t* grant,
                                              cf_t*                          symbols,
                                              cf_t*                          sf_symbols,
                                              bool                           put)
{
  uint32_t count = 0;
  uint32_t delta = 0;

  for (uint32_t i = 0; i < q->carrier.nof_prb; i++) {
    if (grant->prb_idx[i]) {
      for (uint32_t j = 0; j < SRSLTE_NRE; j += 2) {
        if (put) {
          sf_symbols[i * SRSLTE_NRE + delta + j] = symbols[count++];
        } else {
          symbols[count++] = sf_symbols[i * SRSLTE_NRE + delta + j];
        }
      }
    }
  }

  return count;
}

static uint32_t srslte_pdsch_nr_cp_dmrs_type2(const srslte_pdsch_nr_t*       q,
                                              const srslte_pdsch_grant_nr_t* grant,
                                              cf_t*                          symbols,
                                              cf_t*                          sf_symbols,
                                              bool                           put)
{
  uint32_t count = 0;
  uint32_t delta = 0;

  for (uint32_t i = 0; i < q->carrier.nof_prb; i++) {
    if (grant->prb_idx[i]) {
      // Copy RE before first pilot pair
      if (delta > 0) {
        srslte_pdsch_re_cp(&sf_symbols[i * SRSLTE_NRE], &symbols[count], delta, put);
        count += delta;
      }

      // Copy RE between pilot pairs
      srslte_pdsch_re_cp(&sf_symbols[i * SRSLTE_NRE + delta + 2], &symbols[count], 4, put);
      count += 4;

      // Copy RE after second pilot
      srslte_pdsch_re_cp(&sf_symbols[(i + 1) * SRSLTE_NRE - 4 + delta], &symbols[count], 4 - delta, put);
      count += 4 - delta;
    }
  }

  return count;
}

static uint32_t srslte_pdsch_nr_cp_dmrs(const srslte_pdsch_nr_t*       q,
                                        const srslte_pdsch_cfg_nr_t*   cfg,
                                        const srslte_pdsch_grant_nr_t* grant,
                                        cf_t*                          symbols,
                                        cf_t*                          sf_symbols,
                                        bool                           put)
{
  uint32_t count = 0;

  const srslte_pdsch_dmrs_cfg_t* dmrs_cfg =
      grant->mapping == srslte_pdsch_mapping_type_A ? &cfg->dmrs_cfg_typeA : &cfg->dmrs_cfg_typeB;

  switch (dmrs_cfg->type) {
    case srslte_dmrs_pdsch_type_1:
      count = srslte_pdsch_nr_cp_dmrs_type1(q, grant, symbols, sf_symbols, put);
      break;
    case srslte_dmrs_pdsch_type_2:
      count = srslte_pdsch_nr_cp_dmrs_type2(q, grant, symbols, sf_symbols, put);
      break;
  }

  return count;
}

static uint32_t srslte_pdsch_nr_cp_clean(const srslte_pdsch_nr_t*       q,
                                         const srslte_pdsch_grant_nr_t* grant,
                                         cf_t*                          symbols,
                                         cf_t*                          sf_symbols,
                                         bool                           put)
{
  uint32_t count  = 0;
  uint32_t start  = 0; // Index of the start of continuous data
  uint32_t length = 0; // End of continuous RE

  for (uint32_t i = 0; i < q->carrier.nof_prb; i++) {
    if (grant->prb_idx[i]) {
      // If fist continuous block, save start
      if (length == 0) {
        start = i * SRSLTE_NRE;
      }
      length += SRSLTE_NRE;
    } else {
      // Consecutive block is finished
      if (put) {
        srslte_vec_cf_copy(&sf_symbols[start], &symbols[count], length);
      } else {
        srslte_vec_cf_copy(&symbols[count], &sf_symbols[start], length);
      }

      // Increase RE count
      count += length;

      // Reset consecutive block
      length = 0;
    }
  }

  // Copy last contiguous block
  if (length > 0) {
    if (put) {
      srslte_vec_cf_copy(&sf_symbols[start], &symbols[count], length);
    } else {
      srslte_vec_cf_copy(&symbols[count], &sf_symbols[start], length);
    }
    count += length;
  }

  return count;
}

static int srslte_pdsch_nr_cp(const srslte_pdsch_nr_t*       q,
                              const srslte_pdsch_cfg_nr_t*   cfg,
                              const srslte_pdsch_grant_nr_t* grant,
                              cf_t*                          symbols,
                              cf_t*                          sf_symbols,
                              bool                           put)
{
  uint32_t count                                     = 0;
  uint32_t dmrs_l_idx[SRSLTE_DMRS_PDSCH_MAX_SYMBOLS] = {};
  uint32_t dmrs_l_count                              = 0;

  // Get symbol indexes carrying DMRS
  int32_t nof_dmrs_symbols = srslte_dmrs_pdsch_get_symbols_idx(cfg, grant, dmrs_l_idx);
  if (nof_dmrs_symbols < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    printf("dmrs_l_idx=");
    srslte_vec_fprint_i(stdout, (int32_t*)dmrs_l_idx, nof_dmrs_symbols);
  }

  for (uint32_t l = grant->S; l < grant->S + grant->L; l++) {
    // Advance DMRS symbol counter until:
    // - the current DMRS symbol index is greater or equal than current symbol l
    // - no more DMRS symbols
    while (dmrs_l_idx[dmrs_l_count] < l && dmrs_l_count < nof_dmrs_symbols) {
      dmrs_l_count++;
    }

    if (l == dmrs_l_idx[dmrs_l_count]) {
      count += srslte_pdsch_nr_cp_dmrs(
          q, cfg, grant, &symbols[count], &sf_symbols[l * q->carrier.nof_prb * SRSLTE_NRE], put);
    } else {
      count +=
          srslte_pdsch_nr_cp_clean(q, grant, &symbols[count], &sf_symbols[l * q->carrier.nof_prb * SRSLTE_NRE], put);
    }
  }

  return count;
}

static int srslte_pdsch_nr_put(const srslte_pdsch_nr_t*       q,
                               const srslte_pdsch_cfg_nr_t*   cfg,
                               const srslte_pdsch_grant_nr_t* grant,
                               cf_t*                          symbols,
                               cf_t*                          sf_symbols)
{
  return srslte_pdsch_nr_cp(q, cfg, grant, symbols, sf_symbols, true);
}

static int srslte_pdsch_nr_get(const srslte_pdsch_nr_t*       q,
                               const srslte_pdsch_cfg_nr_t*   cfg,
                               const srslte_pdsch_grant_nr_t* grant,
                               cf_t*                          symbols,
                               cf_t*                          sf_symbols)
{
  return srslte_pdsch_nr_cp(q, cfg, grant, symbols, sf_symbols, false);
}

static inline int pdsch_nr_encode_codeword(srslte_pdsch_nr_t*           q,
                                           const srslte_pdsch_cfg_nr_t* cfg,
                                           const srslte_sch_tb_t*       tb,
                                           const uint8_t*               data,
                                           uint16_t                     rnti)
{
  // Early return if TB is not enabled
  if (!tb->enabled) {
    return SRSLTE_SUCCESS;
  }

  // Check codeword index
  if (tb->cw_idx >= q->max_cw) {
    ERROR("Unsupported codeword index %d\n", tb->cw_idx);
    return SRSLTE_ERROR;
  }

  // Check modulation
  if (tb->mod >= SRSLTE_MOD_NITEMS) {
    ERROR("Invalid modulation %s\n", srslte_mod_string(tb->mod));
    return SRSLTE_ERROR_OUT_OF_BOUNDS;
  }

  // Encode SCH
  if (srslte_dlsch_nr_encode(&q->sch, &cfg->sch_cfg, tb, data, q->b[tb->cw_idx]) < SRSLTE_SUCCESS) {
    ERROR("Error in DL-SCH encoding\n");
    return SRSLTE_ERROR;
  }

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    printf("b=");
    srslte_vec_fprint_b(stdout, q->b[tb->cw_idx], tb->nof_bits);
  }

  // 7.3.1.1 Scrambling
  uint32_t n_id = q->carrier.id;
  if (cfg->scrambling_id_present && SRSLTE_RNTI_ISUSER(rnti)) {
    n_id = cfg->scambling_id;
  }
  uint32_t cinit = ((uint32_t)rnti << 15U) + (tb->cw_idx << 14U) + n_id;
  srslte_sequence_apply_bit(q->b[tb->cw_idx], q->b[tb->cw_idx], tb->nof_bits, cinit);

  // 7.3.1.2 Modulation
  srslte_mod_modulate(&q->modem_tables[tb->mod], q->b[tb->cw_idx], q->d[tb->cw_idx], tb->nof_bits);

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    printf("d=");
    srslte_vec_fprint_c(stdout, q->d[tb->cw_idx], tb->nof_re);
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_nr_encode(srslte_pdsch_nr_t*             q,
                           const srslte_pdsch_cfg_nr_t*   cfg,
                           const srslte_pdsch_grant_nr_t* grant,
                           uint8_t*                       data[SRSLTE_MAX_TB],
                           cf_t*                          sf_symbols[SRSLTE_MAX_PORTS])
{
  uint32_t nof_cw = 0;

  // Check input pointers
  if (!q || !cfg || !grant || !data || !sf_symbols) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Check number of layers
  if (q->max_layers < grant->nof_layers) {
    ERROR("Error number of layers (%d) exceeds configured maximum (%d)\n", grant->nof_layers, q->max_layers);
    return SRSLTE_ERROR;
  }

  // 7.3.1.1 and 7.3.1.2
  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
    nof_cw += grant->tb[tb].enabled ? 1 : 0;

    if (pdsch_nr_encode_codeword(q, cfg, &grant->tb[tb], data[tb], grant->rnti) < SRSLTE_SUCCESS) {
      ERROR("Error encoding TB %d\n", tb);
      return SRSLTE_ERROR;
    }
  }

  // 7.3.1.3 Layer mapping
  cf_t** x = q->d;
  if (grant->nof_layers > 1) {
    x = q->x;
    srslte_layermap_nr(q->d, nof_cw, x, grant->nof_layers, grant->nof_layers);
  }

  // 7.3.1.4 Antenna port mapping
  // ... Not implemented

  // 7.3.1.5 Mapping to virtual resource blocks
  // ... Not implemented

  // 7.3.1.6 Mapping from virtual to physical resource blocks
  srslte_pdsch_nr_put(q, cfg, grant, x[0], sf_symbols[0]);

  return SRSLTE_SUCCESS;
}

static inline int pdsch_nr_decode_codeword(srslte_pdsch_nr_t*           q,
                                           const srslte_pdsch_cfg_nr_t* cfg,
                                           const srslte_sch_tb_t*       tb,
                                           srslte_pdsch_res_nr_t*       res,
                                           uint16_t                     rnti)
{
  // Early return if TB is not enabled
  if (!tb->enabled) {
    return SRSLTE_SUCCESS;
  }

  // Check codeword index
  if (tb->cw_idx >= q->max_cw) {
    ERROR("Unsupported codeword index %d\n", tb->cw_idx);
    return SRSLTE_ERROR;
  }

  // Check modulation
  if (tb->mod >= SRSLTE_MOD_NITEMS) {
    ERROR("Invalid modulation %s\n", srslte_mod_string(tb->mod));
    return SRSLTE_ERROR_OUT_OF_BOUNDS;
  }

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    printf("d=");
    srslte_vec_fprint_c(stdout, q->d[tb->cw_idx], tb->nof_re);
  }

  // Demodulation
  int8_t* llr = (int8_t*)q->b[tb->cw_idx];
  if (srslte_demod_soft_demodulate_b(tb->mod, q->d[tb->cw_idx], llr, tb->nof_re)) {
    return SRSLTE_ERROR;
  }
  // Change LLR sign
  for (uint32_t i = 0; i < tb->nof_bits; i++) {
    llr[i] = -llr[i];
  }

  // Descrambling
  uint32_t n_id = q->carrier.id;
  if (cfg->scrambling_id_present && SRSLTE_RNTI_ISUSER(rnti)) {
    n_id = cfg->scambling_id;
  }
  uint32_t cinit = ((uint32_t)rnti << 15U) + (tb->cw_idx << 14U) + n_id;
  srslte_sequence_apply_c(llr, llr, tb->nof_bits, cinit);

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    printf("b=");
    srslte_vec_fprint_b(stdout, q->b[tb->cw_idx], tb->nof_bits);
  }

  // Decode SCH
  if (srslte_dlsch_nr_decode(&q->sch, &cfg->sch_cfg, tb, llr, res->payload, &res->crc) < SRSLTE_SUCCESS) {
    ERROR("Error in DL-SCH encoding\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_nr_decode(srslte_pdsch_nr_t*             q,
                           const srslte_pdsch_cfg_nr_t*   cfg,
                           const srslte_pdsch_grant_nr_t* grant,
                           srslte_chest_dl_res_t*         channel,
                           cf_t*                          sf_symbols[SRSLTE_MAX_PORTS],
                           srslte_pdsch_res_nr_t          data[SRSLTE_MAX_TB])
{
  uint32_t nof_cw = 0;
  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
    nof_cw += grant->tb[tb].enabled ? 1 : 0;
  }

  uint32_t nof_re = srslte_ra_dl_nr_slot_nof_re(cfg, grant);

  // Demapping from virtual to physical resource blocks
  cf_t**   x          = (grant->nof_layers > 1) ? q->x : q->d;
  uint32_t nof_re_get = srslte_pdsch_nr_get(q, cfg, grant, x[0], sf_symbols[0]);
  if (nof_re_get != nof_re) {
    ERROR("Inconsistent number of RE (%d!=%d)\n", nof_re_get, nof_re);
    return SRSLTE_ERROR;
  }

  // Demapping to virtual resource blocks
  // ... Not implemented

  // Antenna port demapping
  // ... Not implemented

  // Layer demapping
  if (grant->nof_layers > 1) {
    srslte_layerdemap_nr(q->d, nof_cw, q->x, grant->nof_layers, nof_re);
  }

  // SCH decode
  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
    nof_cw += grant->tb[tb].enabled ? 1 : 0;

    if (pdsch_nr_decode_codeword(q, cfg, &grant->tb[tb], &data[tb], grant->rnti) < SRSLTE_SUCCESS) {
      ERROR("Error encoding TB %d\n", tb);
      return SRSLTE_ERROR;
    }
  }

  return SRSLTE_SUCCESS;
}
