/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/phy/phch/pdsch_nr.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/mimo/layermap.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/phch/ra_nr.h"

int pdsch_nr_init_common(srslte_pdsch_nr_t* q, const srslte_pdsch_nr_args_t* args)
{
  for (srslte_mod_t mod = SRSLTE_MOD_BPSK; mod < SRSLTE_MOD_NITEMS; mod++) {
    if (srslte_modem_table_lte(&q->modem_tables[mod], mod) < SRSLTE_SUCCESS) {
      ERROR("Error initialising modem table for %s", srslte_mod_string(mod));
      return SRSLTE_ERROR;
    }
    if (args->measure_evm) {
      srslte_modem_table_bytes(&q->modem_tables[mod]);
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_nr_init_enb(srslte_pdsch_nr_t* q, const srslte_pdsch_nr_args_t* args)
{
  if (q == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (pdsch_nr_init_common(q, args) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_sch_nr_init_tx(&q->sch, &args->sch)) {
    ERROR("Initialising SCH");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_nr_init_ue(srslte_pdsch_nr_t* q, const srslte_pdsch_nr_args_t* args)
{
  if (q == NULL || args == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (pdsch_nr_init_common(q, args) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_sch_nr_init_rx(&q->sch, &args->sch)) {
    ERROR("Initialising SCH");
    return SRSLTE_ERROR;
  }

  if (args->measure_evm) {
    q->evm_buffer = srslte_evm_buffer_alloc(8);
    if (q->evm_buffer == NULL) {
      ERROR("Initialising EVM");
      return SRSLTE_ERROR;
    }
  }

  q->meas_time_en = args->measure_time;

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_nr_set_carrier(srslte_pdsch_nr_t* q, const srslte_carrier_nr_t* carrier)
{
  // Set carrier
  q->carrier = *carrier;

  // Reallocate symbols if necessary
  if (q->max_layers < carrier->max_mimo_layers || q->max_prb < carrier->nof_prb) {
    q->max_layers = carrier->max_mimo_layers;
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
        q->b[i] = srslte_vec_u8_malloc(SRSLTE_SLOT_MAX_NOF_BITS_NR);
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

  if (q->evm_buffer != NULL) {
    srslte_evm_buffer_resize(q->evm_buffer, SRSLTE_SLOT_LEN_RE_NR(q->max_prb) * SRSLTE_MAX_QM);
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

  if (q->evm_buffer != NULL) {
    srslte_evm_free(q->evm_buffer);
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

/*
 * As a RB is 12 RE wide, positions marked as 1 will be used for the 1st CDM group, and the same with group 2:
 *
 *  +---+---+---+---+---+---+---+---+---+---+---+---+
 *  | 1 | 2 | 1 | 2 | 1 | 2 | 1 | 2 | 1 | 2 | 1 | 2 |
 *  +---+---+---+---+---+---+---+---+---+---+---+---+
 *  -- k -->
 *
 * If the number of DMRS CDM groups without data is set to:
 * - 1, data is mapped in RE marked as 2
 * - Otherwise, no data is mapped in this symbol
 */
static uint32_t srslte_pdsch_nr_cp_dmrs_type1(const srslte_pdsch_nr_t*     q,
                                              const srslte_sch_grant_nr_t* grant,
                                              cf_t*                        symbols,
                                              cf_t*                        sf_symbols,
                                              bool                         put)
{
  uint32_t count = 0;
  uint32_t delta = 0;

  if (grant->nof_dmrs_cdm_groups_without_data != 1) {
    return count;
  }

  for (uint32_t i = 0; i < q->carrier.nof_prb; i++) {
    if (grant->prb_idx[i]) {
      for (uint32_t j = 0; j < SRSLTE_NRE; j += 2) {
        if (put) {
          sf_symbols[i * SRSLTE_NRE + delta + j + 1] = symbols[count++];
        } else {
          symbols[count++] = sf_symbols[i * SRSLTE_NRE + delta + j + 1];
        }
      }
    }
  }

  return count;
}

/*
 * As a RB is 12 RE wide, positions marked as 1 will be used for the 1st CDM group, and the same with groups 2 and 3:
 *
 *  +---+---+---+---+---+---+---+---+---+---+---+---+
 *  | 1 | 1 | 2 | 2 | 3 | 3 | 1 | 1 | 2 | 2 | 3 | 3 |
 *  +---+---+---+---+---+---+---+---+---+---+---+---+
 *  -- k -->
 *
 * If the number of DMRS CDM groups without data is set to:
 * - 1, data is mapped in RE marked as 2 and 3
 * - 2, data is mapped in RE marked as 3
 * - otherwise, no data is mapped in this symbol
 */
static uint32_t srslte_pdsch_nr_cp_dmrs_type2(const srslte_pdsch_nr_t*     q,
                                              const srslte_sch_grant_nr_t* grant,
                                              cf_t*                        symbols,
                                              cf_t*                        sf_symbols,
                                              bool                         put)
{
  uint32_t count = 0;

  if (grant->nof_dmrs_cdm_groups_without_data != 1 && grant->nof_dmrs_cdm_groups_without_data != 2) {
    return count;
  }

  uint32_t re_offset = (grant->nof_dmrs_cdm_groups_without_data == 1) ? 2 : 4;
  uint32_t re_count  = (grant->nof_dmrs_cdm_groups_without_data == 1) ? 4 : 2;

  for (uint32_t i = 0; i < q->carrier.nof_prb; i++) {
    if (grant->prb_idx[i]) {
      // Copy RE between pilot pairs
      srslte_pdsch_re_cp(&sf_symbols[i * SRSLTE_NRE + re_offset], &symbols[count], re_count, put);
      count += re_count;

      // Copy RE after second pilot
      srslte_pdsch_re_cp(&sf_symbols[(i + 1) * SRSLTE_NRE - re_count], &symbols[count], re_count, put);
      count += re_count;
    }
  }

  return count;
}

static uint32_t srslte_pdsch_nr_cp_dmrs(const srslte_pdsch_nr_t*     q,
                                        const srslte_sch_cfg_nr_t*   cfg,
                                        const srslte_sch_grant_nr_t* grant,
                                        cf_t*                        symbols,
                                        cf_t*                        sf_symbols,
                                        bool                         put)
{
  uint32_t count = 0;

  const srslte_dmrs_sch_cfg_t* dmrs_cfg = &cfg->dmrs;

  switch (dmrs_cfg->type) {
    case srslte_dmrs_sch_type_1:
      count = srslte_pdsch_nr_cp_dmrs_type1(q, grant, symbols, sf_symbols, put);
      break;
    case srslte_dmrs_sch_type_2:
      count = srslte_pdsch_nr_cp_dmrs_type2(q, grant, symbols, sf_symbols, put);
      break;
  }

  return count;
}

static uint32_t srslte_pdsch_nr_cp_clean(const srslte_pdsch_nr_t*     q,
                                         const srslte_sch_grant_nr_t* grant,
                                         cf_t*                        symbols,
                                         cf_t*                        sf_symbols,
                                         bool                         put)
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

static int srslte_pdsch_nr_cp(const srslte_pdsch_nr_t*     q,
                              const srslte_sch_cfg_nr_t*   cfg,
                              const srslte_sch_grant_nr_t* grant,
                              cf_t*                        symbols,
                              cf_t*                        sf_symbols,
                              bool                         put)
{
  uint32_t count                                   = 0;
  uint32_t dmrs_l_idx[SRSLTE_DMRS_SCH_MAX_SYMBOLS] = {};
  uint32_t dmrs_l_count                            = 0;

  // Get symbol indexes carrying DMRS
  int32_t nof_dmrs_symbols = srslte_dmrs_sch_get_symbols_idx(&cfg->dmrs, grant, dmrs_l_idx);
  if (nof_dmrs_symbols < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("dmrs_l_idx=");
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

static int srslte_pdsch_nr_put(const srslte_pdsch_nr_t*     q,
                               const srslte_sch_cfg_nr_t*   cfg,
                               const srslte_sch_grant_nr_t* grant,
                               cf_t*                        symbols,
                               cf_t*                        sf_symbols)
{
  return srslte_pdsch_nr_cp(q, cfg, grant, symbols, sf_symbols, true);
}

static int srslte_pdsch_nr_get(const srslte_pdsch_nr_t*     q,
                               const srslte_sch_cfg_nr_t*   cfg,
                               const srslte_sch_grant_nr_t* grant,
                               cf_t*                        symbols,
                               cf_t*                        sf_symbols)
{
  return srslte_pdsch_nr_cp(q, cfg, grant, symbols, sf_symbols, false);
}

static uint32_t
pdsch_nr_cinit(const srslte_carrier_nr_t* carrier, const srslte_sch_cfg_nr_t* cfg, uint16_t rnti, uint32_t cw_idx)
{
  uint32_t n_id = carrier->id;
  if (cfg->scrambling_id_present && SRSLTE_RNTI_ISUSER(rnti)) {
    n_id = cfg->scambling_id;
  }
  uint32_t cinit = (((uint32_t)rnti) << 15U) + (cw_idx << 14U) + n_id;

  INFO("PDSCH: RNTI=%d (0x%x); nid=%d; cinit=%d (0x%x);", rnti, rnti, n_id, cinit, cinit);

  return cinit;
}

static inline int pdsch_nr_encode_codeword(srslte_pdsch_nr_t*         q,
                                           const srslte_sch_cfg_nr_t* cfg,
                                           const srslte_sch_tb_t*     tb,
                                           const uint8_t*             data,
                                           uint16_t                   rnti)
{
  // Early return if TB is not enabled
  if (!tb->enabled) {
    return SRSLTE_SUCCESS;
  }

  // Check codeword index
  if (tb->cw_idx >= q->max_cw) {
    ERROR("Unsupported codeword index %d", tb->cw_idx);
    return SRSLTE_ERROR;
  }

  // Check modulation
  if (tb->mod >= SRSLTE_MOD_NITEMS) {
    ERROR("Invalid modulation %s", srslte_mod_string(tb->mod));
    return SRSLTE_ERROR_OUT_OF_BOUNDS;
  }

  // Encode SCH
  if (srslte_dlsch_nr_encode(&q->sch, &cfg->sch_cfg, tb, data, q->b[tb->cw_idx]) < SRSLTE_SUCCESS) {
    ERROR("Error in DL-SCH encoding");
    return SRSLTE_ERROR;
  }

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("b=");
    srslte_vec_fprint_b(stdout, q->b[tb->cw_idx], tb->nof_bits);
  }

  // 7.3.1.1 Scrambling
  uint32_t cinit = pdsch_nr_cinit(&q->carrier, cfg, rnti, tb->cw_idx);
  srslte_sequence_apply_bit(q->b[tb->cw_idx], q->b[tb->cw_idx], tb->nof_bits, cinit);

  // 7.3.1.2 Modulation
  srslte_mod_modulate(&q->modem_tables[tb->mod], q->b[tb->cw_idx], q->d[tb->cw_idx], tb->nof_bits);

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("d=");
    srslte_vec_fprint_c(stdout, q->d[tb->cw_idx], tb->nof_re);
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_nr_encode(srslte_pdsch_nr_t*           q,
                           const srslte_sch_cfg_nr_t*   cfg,
                           const srslte_sch_grant_nr_t* grant,
                           uint8_t*                     data[SRSLTE_MAX_TB],
                           cf_t*                        sf_symbols[SRSLTE_MAX_PORTS])
{
  // Check input pointers
  if (!q || !cfg || !grant || !data || !sf_symbols) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  struct timeval t[3];
  if (q->meas_time_en) {
    gettimeofday(&t[1], NULL);
  }

  // Check number of layers
  if (q->max_layers < grant->nof_layers) {
    ERROR("Error number of layers (%d) exceeds configured maximum (%d)", grant->nof_layers, q->max_layers);
    return SRSLTE_ERROR;
  }

  // 7.3.1.1 and 7.3.1.2
  uint32_t nof_cw = 0;
  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
    nof_cw += grant->tb[tb].enabled ? 1 : 0;

    if (pdsch_nr_encode_codeword(q, cfg, &grant->tb[tb], data[tb], grant->rnti) < SRSLTE_SUCCESS) {
      ERROR("Error encoding TB %d", tb);
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
  int n = srslte_pdsch_nr_put(q, cfg, grant, x[0], sf_symbols[0]);
  if (n < SRSLTE_SUCCESS) {
    ERROR("Putting NR PDSCH resources");
    return SRSLTE_ERROR;
  }

  if (n != grant->tb[0].nof_re) {
    ERROR("Unmatched number of RE (%d != %d)", n, grant->tb[0].nof_re);
    return SRSLTE_ERROR;
  }

  if (q->meas_time_en) {
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    q->meas_time_us = (uint32_t)t[0].tv_usec;
  }

  return SRSLTE_SUCCESS;
}

static inline int pdsch_nr_decode_codeword(srslte_pdsch_nr_t*         q,
                                           const srslte_sch_cfg_nr_t* cfg,
                                           const srslte_sch_tb_t*     tb,
                                           srslte_pdsch_res_nr_t*     res,
                                           uint16_t                   rnti)
{
  // Early return if TB is not enabled
  if (!tb->enabled) {
    return SRSLTE_SUCCESS;
  }

  // Check codeword index
  if (tb->cw_idx >= q->max_cw) {
    ERROR("Unsupported codeword index %d", tb->cw_idx);
    return SRSLTE_ERROR;
  }

  // Check modulation
  if (tb->mod >= SRSLTE_MOD_NITEMS) {
    ERROR("Invalid modulation %s", srslte_mod_string(tb->mod));
    return SRSLTE_ERROR_OUT_OF_BOUNDS;
  }

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("d=");
    srslte_vec_fprint_c(stdout, q->d[tb->cw_idx], tb->nof_re);
  }

  // Demodulation
  int8_t* llr = (int8_t*)q->b[tb->cw_idx];
  if (srslte_demod_soft_demodulate_b(tb->mod, q->d[tb->cw_idx], llr, tb->nof_re)) {
    return SRSLTE_ERROR;
  }

  // EVM
  if (q->evm_buffer != NULL) {
    res->evm = srslte_evm_run_b(q->evm_buffer, &q->modem_tables[tb->mod], q->d[tb->cw_idx], llr, tb->nof_bits);
  }

  // Change LLR sign
  for (uint32_t i = 0; i < tb->nof_bits; i++) {
    llr[i] = -llr[i];
  }

  // Descrambling
  srslte_sequence_apply_c(llr, llr, tb->nof_bits, pdsch_nr_cinit(&q->carrier, cfg, rnti, tb->cw_idx));

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("b=");
    srslte_vec_fprint_b(stdout, q->b[tb->cw_idx], tb->nof_bits);
  }

  // Decode SCH
  if (srslte_dlsch_nr_decode(&q->sch, &cfg->sch_cfg, tb, llr, res->payload, &res->crc) < SRSLTE_SUCCESS) {
    ERROR("Error in DL-SCH encoding");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_nr_decode(srslte_pdsch_nr_t*           q,
                           const srslte_sch_cfg_nr_t*   cfg,
                           const srslte_sch_grant_nr_t* grant,
                           srslte_chest_dl_res_t*       channel,
                           cf_t*                        sf_symbols[SRSLTE_MAX_PORTS],
                           srslte_pdsch_res_nr_t        data[SRSLTE_MAX_TB])
{
  // Check input pointers
  if (!q || !cfg || !grant || !data || !sf_symbols) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  struct timeval t[3];
  if (q->meas_time_en) {
    gettimeofday(&t[1], NULL);
  }

  uint32_t nof_cw = 0;
  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
    nof_cw += grant->tb[tb].enabled ? 1 : 0;
  }

  uint32_t nof_re = srslte_ra_dl_nr_slot_nof_re(cfg, grant);

  if (channel->nof_re != nof_re) {
    ERROR("Inconsistent number of RE (%d!=%d)", channel->nof_re, nof_re);
    return SRSLTE_ERROR;
  }

  // Demapping from virtual to physical resource blocks
  uint32_t nof_re_get = srslte_pdsch_nr_get(q, cfg, grant, q->x[0], sf_symbols[0]);
  if (nof_re_get != nof_re) {
    ERROR("Inconsistent number of RE (%d!=%d)", nof_re_get, nof_re);
    return SRSLTE_ERROR;
  }

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("ce=");
    srslte_vec_fprint_c(stdout, channel->ce[0][0], nof_re);
    DEBUG("x=");
    srslte_vec_fprint_c(stdout, q->x[0], nof_re);
  }

  // Demapping to virtual resource blocks
  // ... Not implemented

  // Antenna port demapping
  // ... Not implemented
  srslte_predecoding_type(
      q->x, channel->ce, q->d, NULL, 1, 1, 1, 0, nof_re, SRSLTE_TXSCHEME_PORT0, 1.0f, channel->noise_estimate);

  // Layer demapping
  if (grant->nof_layers > 1) {
    srslte_layerdemap_nr(q->d, nof_cw, q->x, grant->nof_layers, nof_re);
  }

  // SCH decode
  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
    nof_cw += grant->tb[tb].enabled ? 1 : 0;

    if (pdsch_nr_decode_codeword(q, cfg, &grant->tb[tb], &data[tb], grant->rnti) < SRSLTE_SUCCESS) {
      ERROR("Error encoding TB %d", tb);
      return SRSLTE_ERROR;
    }
  }

  if (q->meas_time_en) {
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    q->meas_time_us = (uint32_t)t[0].tv_usec;
  }

  return SRSLTE_SUCCESS;
}

static uint32_t srslte_pdsch_nr_grant_info(const srslte_sch_cfg_nr_t*   cfg,
                                           const srslte_sch_grant_nr_t* grant,
                                           char*                        str,
                                           uint32_t                     str_len)
{
  uint32_t len = 0;
  len          = srslte_print_check(str, str_len, len, "rnti=0x%x", grant->rnti);

  uint32_t first_prb = SRSLTE_MAX_PRB_NR;
  for (uint32_t i = 0; i < SRSLTE_MAX_PRB_NR && first_prb == SRSLTE_MAX_PRB_NR; i++) {
    if (grant->prb_idx[i]) {
      first_prb = i;
    }
  }

  // Append time-domain resource mapping
  len = srslte_print_check(str,
                           str_len,
                           len,
                           ",k0=%d,prb=%d:%d,symb=%d:%d,mapping=%s",
                           grant->k,
                           first_prb,
                           grant->nof_prb,
                           grant->S,
                           grant->L,
                           srslte_sch_mapping_type_to_str(grant->mapping));

  // Skip frequency domain resources...
  // ...

  // Append spatial resources
  len = srslte_print_check(str, str_len, len, ",Nl=%d", grant->nof_layers);

  // Append scrambling ID
  len = srslte_print_check(str, str_len, len, ",n_scid=%d,", grant->n_scid);

  // Append TB info
  for (uint32_t i = 0; i < SRSLTE_MAX_TB; i++) {
    len += srslte_sch_nr_tb_info(&grant->tb[i], &str[len], str_len - len);
  }

  return len;
}

uint32_t srslte_pdsch_nr_rx_info(const srslte_pdsch_nr_t*     q,
                                 const srslte_sch_cfg_nr_t*   cfg,
                                 const srslte_sch_grant_nr_t* grant,
                                 const srslte_pdsch_res_nr_t  res[SRSLTE_MAX_CODEWORDS],
                                 char*                        str,
                                 uint32_t                     str_len)
{
  uint32_t len = 0;

  len += srslte_pdsch_nr_grant_info(cfg, grant, &str[len], str_len - len);

  if (q->evm_buffer != NULL) {
    len = srslte_print_check(str, str_len, len, ",evm={", 0);
    for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      if (grant->tb[i].enabled && !isnan(res[i].evm)) {
        len = srslte_print_check(str, str_len, len, "%.2f", res[i].evm);
        if (i < SRSLTE_MAX_CODEWORDS - 1) {
          if (grant->tb[i + 1].enabled) {
            len = srslte_print_check(str, str_len, len, ",", 0);
          }
        }
      }
    }
    len = srslte_print_check(str, str_len, len, "}", 0);
  }

  if (res != NULL) {
    len = srslte_print_check(str, str_len, len, ",crc={", 0);
    for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      if (grant->tb[i].enabled) {
        len = srslte_print_check(str, str_len, len, "%s", res[i].crc ? "OK" : "KO");
        if (i < SRSLTE_MAX_CODEWORDS - 1) {
          if (grant->tb[i + 1].enabled) {
            len = srslte_print_check(str, str_len, len, ",", 0);
          }
        }
      }
    }
    len = srslte_print_check(str, str_len, len, "}", 0);
  }

  if (q->meas_time_en) {
    len = srslte_print_check(str, str_len, len, ", t=%d us", q->meas_time_us);
  }

  return len;
}

uint32_t srslte_pdsch_nr_tx_info(const srslte_pdsch_nr_t*     q,
                                 const srslte_sch_cfg_nr_t*   cfg,
                                 const srslte_sch_grant_nr_t* grant,
                                 char*                        str,
                                 uint32_t                     str_len)
{
  uint32_t len = 0;

  len += srslte_pdsch_nr_grant_info(cfg, grant, &str[len], str_len - len);

  if (q->meas_time_en) {
    len = srslte_print_check(str, str_len, len, ", t=%d us", q->meas_time_us);
  }

  return len;
}
