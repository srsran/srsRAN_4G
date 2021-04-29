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

#include "srsran/phy/phch/pdsch_nr.h"
#include "srsran/phy/ch_estimation/csi_rs.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"

static int pdsch_nr_alloc(srsran_pdsch_nr_t* q, uint32_t max_mimo_layers, uint32_t max_prb)
{
  // Reallocate symbols if necessary
  if (q->max_layers < max_mimo_layers || q->max_prb < max_prb) {
    q->max_layers = max_mimo_layers;
    q->max_prb    = max_prb;

    // Free current allocations
    for (uint32_t i = 0; i < SRSRAN_MAX_LAYERS_NR; i++) {
      if (q->x[i] != NULL) {
        free(q->x[i]);
      }
    }

    // Allocate for new sizes
    for (uint32_t i = 0; i < q->max_layers; i++) {
      q->x[i] = srsran_vec_cf_malloc(SRSRAN_SLOT_LEN_RE_NR(q->max_prb));
      if (q->x[i] == NULL) {
        ERROR("Malloc");
        return SRSRAN_ERROR;
      }
    }
  }

  return SRSRAN_SUCCESS;
}

int pdsch_nr_init_common(srsran_pdsch_nr_t* q, const srsran_pdsch_nr_args_t* args)
{
  SRSRAN_MEM_ZERO(q, srsran_pdsch_nr_t, 1);

  for (srsran_mod_t mod = SRSRAN_MOD_BPSK; mod < SRSRAN_MOD_NITEMS; mod++) {
    if (srsran_modem_table_lte(&q->modem_tables[mod], mod) < SRSRAN_SUCCESS) {
      ERROR("Error initialising modem table for %s", srsran_mod_string(mod));
      return SRSRAN_ERROR;
    }
    if (args->measure_evm) {
      srsran_modem_table_bytes(&q->modem_tables[mod]);
    }
  }

  if (pdsch_nr_alloc(q, args->max_layers, args->max_prb) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_pdsch_nr_init_enb(srsran_pdsch_nr_t* q, const srsran_pdsch_nr_args_t* args)
{
  if (q == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (pdsch_nr_init_common(q, args) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_sch_nr_init_tx(&q->sch, &args->sch)) {
    ERROR("Initialising SCH");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_pdsch_nr_init_ue(srsran_pdsch_nr_t* q, const srsran_pdsch_nr_args_t* args)
{
  if (q == NULL || args == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (pdsch_nr_init_common(q, args) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_sch_nr_init_rx(&q->sch, &args->sch)) {
    ERROR("Initialising SCH");
    return SRSRAN_ERROR;
  }

  if (args->measure_evm) {
    q->evm_buffer = srsran_evm_buffer_alloc(8);
    if (q->evm_buffer == NULL) {
      ERROR("Initialising EVM");
      return SRSRAN_ERROR;
    }
  }

  q->meas_time_en = args->measure_time;

  return SRSRAN_SUCCESS;
}

int srsran_pdsch_nr_set_carrier(srsran_pdsch_nr_t* q, const srsran_carrier_nr_t* carrier)
{
  // Set carrier
  q->carrier = *carrier;

  if (pdsch_nr_alloc(q, carrier->max_mimo_layers, carrier->nof_prb) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Allocate code words according to table 7.3.1.3-1
  uint32_t max_cw = (q->max_layers > 5) ? 2 : 1;
  if (q->max_cw < max_cw) {
    q->max_cw = max_cw;

    for (uint32_t i = 0; i < max_cw; i++) {
      if (q->b[i] == NULL) {
        q->b[i] = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
        if (q->b[i] == NULL) {
          ERROR("Malloc");
          return SRSRAN_ERROR;
        }
      }

      if (q->d[i] == NULL) {
        q->d[i] = srsran_vec_cf_malloc(SRSRAN_SLOT_MAX_LEN_RE_NR);
        if (q->d[i] == NULL) {
          ERROR("Malloc");
          return SRSRAN_ERROR;
        }
      }
    }
  }

  // Set carrier in SCH
  if (srsran_sch_nr_set_carrier(&q->sch, carrier) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (q->evm_buffer != NULL) {
    srsran_evm_buffer_resize(q->evm_buffer, SRSRAN_SLOT_LEN_RE_NR(q->max_prb) * SRSRAN_MAX_QM);
  }

  return SRSRAN_SUCCESS;
}

void srsran_pdsch_nr_free(srsran_pdsch_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  for (uint32_t cw = 0; cw < SRSRAN_MAX_CODEWORDS; cw++) {
    if (q->b[cw]) {
      free(q->b[cw]);
    }

    if (q->d[cw]) {
      free(q->d[cw]);
    }
  }

  srsran_sch_nr_free(&q->sch);

  for (uint32_t i = 0; i < SRSRAN_MAX_LAYERS_NR; i++) {
    if (q->x[i]) {
      free(q->x[i]);
    }
  }

  for (srsran_mod_t mod = SRSRAN_MOD_BPSK; mod < SRSRAN_MOD_NITEMS; mod++) {
    srsran_modem_table_free(&q->modem_tables[mod]);
  }

  if (q->evm_buffer != NULL) {
    srsran_evm_free(q->evm_buffer);
  }

  SRSRAN_MEM_ZERO(q, srsran_pdsch_nr_t, 1);
}

static inline uint32_t pdsch_nr_put_rb(cf_t* dst, cf_t* src, bool* rvd_mask)
{
  uint32_t count = 0;
  for (uint32_t i = 0; i < SRSRAN_NRE; i++) {
    if (!rvd_mask[i]) {
      dst[i] = src[count++];
    }
  }
  return count;
}

static inline uint32_t pdsch_nr_get_rb(cf_t* dst, cf_t* src, bool* rvd_mask)
{
  uint32_t count = 0;
  for (uint32_t i = 0; i < SRSRAN_NRE; i++) {
    if (!rvd_mask[i]) {
      dst[count++] = src[i];
    }
  }
  return count;
}

static int srsran_pdsch_nr_cp(const srsran_pdsch_nr_t*     q,
                              const srsran_sch_cfg_nr_t*   cfg,
                              const srsran_sch_grant_nr_t* grant,
                              cf_t*                        symbols,
                              cf_t*                        sf_symbols,
                              bool                         put)
{
  uint32_t count = 0;

  for (uint32_t l = grant->S; l < grant->S + grant->L; l++) {
    // Initialise reserved RE mask to all false
    bool rvd_mask[SRSRAN_NRE * SRSRAN_MAX_PRB_NR] = {};

    // Reserve DMRS
    if (srsran_re_pattern_to_symbol_mask(&q->dmrs_re_pattern, l, rvd_mask) < SRSRAN_SUCCESS) {
      ERROR("Error generating DMRS reserved RE mask");
      return SRSRAN_ERROR;
    }

    // Reserve RE from configuration
    if (srsran_re_pattern_list_to_symbol_mask(&cfg->rvd_re, l, rvd_mask) < SRSRAN_SUCCESS) {
      ERROR("Error generating reserved RE mask");
      return SRSRAN_ERROR;
    }

    // Actual copy
    for (uint32_t rb = 0; rb < q->carrier.nof_prb; rb++) {
      // Skip PRB if not available in grant
      if (!grant->prb_idx[rb]) {
        continue;
      }

      // Calculate RE index at the begin of the symbol
      uint32_t re_idx = (q->carrier.nof_prb * l + rb) * SRSRAN_NRE;

      // Put or get
      if (put) {
        count += pdsch_nr_put_rb(&sf_symbols[re_idx], &symbols[count], &rvd_mask[rb * SRSRAN_NRE]);
      } else {
        count += pdsch_nr_get_rb(&symbols[count], &sf_symbols[re_idx], &rvd_mask[rb * SRSRAN_NRE]);
      }
    }
  }

  return count;
}

static int srsran_pdsch_nr_put(const srsran_pdsch_nr_t*     q,
                               const srsran_sch_cfg_nr_t*   cfg,
                               const srsran_sch_grant_nr_t* grant,
                               cf_t*                        symbols,
                               cf_t*                        sf_symbols)
{
  return srsran_pdsch_nr_cp(q, cfg, grant, symbols, sf_symbols, true);
}

static int srsran_pdsch_nr_get(const srsran_pdsch_nr_t*     q,
                               const srsran_sch_cfg_nr_t*   cfg,
                               const srsran_sch_grant_nr_t* grant,
                               cf_t*                        symbols,
                               cf_t*                        sf_symbols)
{
  return srsran_pdsch_nr_cp(q, cfg, grant, symbols, sf_symbols, false);
}

static uint32_t
pdsch_nr_cinit(const srsran_carrier_nr_t* carrier, const srsran_sch_cfg_nr_t* cfg, uint16_t rnti, uint32_t cw_idx)
{
  uint32_t n_id = carrier->pci;
  if (cfg->scrambling_id_present && SRSRAN_RNTI_ISUSER(rnti)) {
    n_id = cfg->scambling_id;
  }
  uint32_t cinit = (((uint32_t)rnti) << 15U) + (cw_idx << 14U) + n_id;

  INFO("PDSCH: RNTI=%d (0x%x); nid=%d; cinit=%d (0x%x);", rnti, rnti, n_id, cinit, cinit);

  return cinit;
}

static inline int pdsch_nr_encode_codeword(srsran_pdsch_nr_t*         q,
                                           const srsran_sch_cfg_nr_t* cfg,
                                           const srsran_sch_tb_t*     tb,
                                           const uint8_t*             data,
                                           uint16_t                   rnti)
{
  // Early return if TB is not enabled
  if (!tb->enabled) {
    return SRSRAN_SUCCESS;
  }

  // Check codeword index
  if (tb->cw_idx >= q->max_cw) {
    ERROR("Unsupported codeword index %d", tb->cw_idx);
    return SRSRAN_ERROR;
  }

  // Check modulation
  if (tb->mod >= SRSRAN_MOD_NITEMS) {
    ERROR("Invalid modulation %s", srsran_mod_string(tb->mod));
    return SRSRAN_ERROR_OUT_OF_BOUNDS;
  }

  // Encode SCH
  if (srsran_dlsch_nr_encode(&q->sch, &cfg->sch_cfg, tb, data, q->b[tb->cw_idx]) < SRSRAN_SUCCESS) {
    ERROR("Error in DL-SCH encoding");
    return SRSRAN_ERROR;
  }

  if (SRSRAN_DEBUG_ENABLED && srsran_verbose >= SRSRAN_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("b=");
    srsran_vec_fprint_b(stdout, q->b[tb->cw_idx], tb->nof_bits);
  }

  // 7.3.1.1 Scrambling
  uint32_t cinit = pdsch_nr_cinit(&q->carrier, cfg, rnti, tb->cw_idx);
  srsran_sequence_apply_bit(q->b[tb->cw_idx], q->b[tb->cw_idx], tb->nof_bits, cinit);

  // 7.3.1.2 Modulation
  srsran_mod_modulate(&q->modem_tables[tb->mod], q->b[tb->cw_idx], q->d[tb->cw_idx], tb->nof_bits);

  if (SRSRAN_DEBUG_ENABLED && srsran_verbose >= SRSRAN_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("d=");
    srsran_vec_fprint_c(stdout, q->d[tb->cw_idx], tb->nof_re);
  }

  return SRSRAN_SUCCESS;
}

int srsran_pdsch_nr_encode(srsran_pdsch_nr_t*           q,
                           const srsran_sch_cfg_nr_t*   cfg,
                           const srsran_sch_grant_nr_t* grant,
                           uint8_t*                     data[SRSRAN_MAX_TB],
                           cf_t*                        sf_symbols[SRSRAN_MAX_PORTS])
{
  // Check input pointers
  if (!q || !cfg || !grant || !data || !sf_symbols) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  struct timeval t[3];
  if (q->meas_time_en) {
    gettimeofday(&t[1], NULL);
  }

  // Check number of layers
  if (q->max_layers < grant->nof_layers) {
    ERROR("Error number of layers (%d) exceeds configured maximum (%d)", grant->nof_layers, q->max_layers);
    return SRSRAN_ERROR;
  }

  // Compute DMRS pattern
  if (srsran_dmrs_sch_rvd_re_pattern(&cfg->dmrs, grant, &q->dmrs_re_pattern) < SRSRAN_SUCCESS) {
    ERROR("Error computing DMRS pattern");
    return SRSRAN_ERROR;
  }

  // 7.3.1.1 and 7.3.1.2
  uint32_t nof_cw = 0;
  for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
    nof_cw += grant->tb[tb].enabled ? 1 : 0;

    if (pdsch_nr_encode_codeword(q, cfg, &grant->tb[tb], data[tb], grant->rnti) < SRSRAN_SUCCESS) {
      ERROR("Error encoding TB %d", tb);
      return SRSRAN_ERROR;
    }
  }

  // 7.3.1.3 Layer mapping
  cf_t** x = q->d;
  if (grant->nof_layers > 1) {
    x = q->x;
    srsran_layermap_nr(q->d, nof_cw, x, grant->nof_layers, grant->nof_layers);
  }

  // 7.3.1.4 Antenna port mapping
  // ... Not implemented

  // 7.3.1.5 Mapping to virtual resource blocks
  // ... Not implemented

  // 7.3.1.6 Mapping from virtual to physical resource blocks
  int n = srsran_pdsch_nr_put(q, cfg, grant, x[0], sf_symbols[0]);
  if (n < SRSRAN_SUCCESS) {
    ERROR("Putting NR PDSCH resources");
    return SRSRAN_ERROR;
  }

  if (n != grant->tb[0].nof_re) {
    ERROR("Unmatched number of RE (%d != %d)", n, grant->tb[0].nof_re);
    return SRSRAN_ERROR;
  }

  if (q->meas_time_en) {
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    q->meas_time_us = (uint32_t)t[0].tv_usec;
  }

  return SRSRAN_SUCCESS;
}

static inline int pdsch_nr_decode_codeword(srsran_pdsch_nr_t*         q,
                                           const srsran_sch_cfg_nr_t* cfg,
                                           const srsran_sch_tb_t*     tb,
                                           srsran_pdsch_res_nr_t*     res,
                                           uint16_t                   rnti)
{
  // Early return if TB is not enabled
  if (!tb->enabled) {
    return SRSRAN_SUCCESS;
  }

  // Check codeword index
  if (tb->cw_idx >= q->max_cw) {
    ERROR("Unsupported codeword index %d", tb->cw_idx);
    return SRSRAN_ERROR;
  }

  // Check modulation
  if (tb->mod >= SRSRAN_MOD_NITEMS) {
    ERROR("Invalid modulation %s", srsran_mod_string(tb->mod));
    return SRSRAN_ERROR_OUT_OF_BOUNDS;
  }

  if (SRSRAN_DEBUG_ENABLED && srsran_verbose >= SRSRAN_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("d=");
    srsran_vec_fprint_c(stdout, q->d[tb->cw_idx], tb->nof_re);
  }

  // Demodulation
  int8_t* llr = (int8_t*)q->b[tb->cw_idx];
  if (srsran_demod_soft_demodulate_b(tb->mod, q->d[tb->cw_idx], llr, tb->nof_re)) {
    return SRSRAN_ERROR;
  }

  // EVM
  if (q->evm_buffer != NULL) {
    res->evm[tb->cw_idx] =
        srsran_evm_run_b(q->evm_buffer, &q->modem_tables[tb->mod], q->d[tb->cw_idx], llr, tb->nof_bits);
  }

  // Change LLR sign and set to zero the LLR that are not used
  srsran_vec_neg_bb(llr, llr, tb->nof_bits);

  // Descrambling
  srsran_sequence_apply_c(llr, llr, tb->nof_bits, pdsch_nr_cinit(&q->carrier, cfg, rnti, tb->cw_idx));

  if (SRSRAN_DEBUG_ENABLED && srsran_verbose >= SRSRAN_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("b=");
    srsran_vec_fprint_b(stdout, q->b[tb->cw_idx], tb->nof_bits);
  }

  // Decode SCH
  if (srsran_dlsch_nr_decode(&q->sch, &cfg->sch_cfg, tb, llr, &res->tb[tb->cw_idx]) < SRSRAN_SUCCESS) {
    ERROR("Error in DL-SCH encoding");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_pdsch_nr_decode(srsran_pdsch_nr_t*           q,
                           const srsran_sch_cfg_nr_t*   cfg,
                           const srsran_sch_grant_nr_t* grant,
                           srsran_chest_dl_res_t*       channel,
                           cf_t*                        sf_symbols[SRSRAN_MAX_PORTS],
                           srsran_pdsch_res_nr_t*       data)
{
  // Check input pointers
  if (!q || !cfg || !grant || !data || !sf_symbols) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  struct timeval t[3];
  if (q->meas_time_en) {
    gettimeofday(&t[1], NULL);
  }

  // Compute DMRS pattern
  if (srsran_dmrs_sch_rvd_re_pattern(&cfg->dmrs, grant, &q->dmrs_re_pattern) < SRSRAN_SUCCESS) {
    ERROR("Error computing DMRS pattern");
    return SRSRAN_ERROR;
  }

  uint32_t nof_cw = 0;
  for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
    nof_cw += grant->tb[tb].enabled ? 1 : 0;
  }

  uint32_t nof_re = grant->tb[0].nof_re;

  if (channel->nof_re != nof_re) {
    ERROR("Inconsistent number of RE (%d!=%d)", channel->nof_re, nof_re);
    return SRSRAN_ERROR;
  }

  // Demapping from virtual to physical resource blocks
  uint32_t nof_re_get = srsran_pdsch_nr_get(q, cfg, grant, q->x[0], sf_symbols[0]);
  if (nof_re_get != nof_re) {
    ERROR("Inconsistent number of RE (%d!=%d)", nof_re_get, nof_re);
    return SRSRAN_ERROR;
  }

  if (SRSRAN_DEBUG_ENABLED && srsran_verbose >= SRSRAN_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("ce=");
    srsran_vec_fprint_c(stdout, channel->ce[0][0], nof_re);
    DEBUG("x=");
    srsran_vec_fprint_c(stdout, q->x[0], nof_re);
  }

  // Demapping to virtual resource blocks
  // ... Not implemented

  // Antenna port demapping
  // ... Not implemented
  srsran_predecoding_type(
      q->x, channel->ce, q->d, NULL, 1, 1, 1, 0, nof_re, SRSRAN_TXSCHEME_PORT0, 1.0f, channel->noise_estimate);

  // Layer demapping
  if (grant->nof_layers > 1) {
    srsran_layerdemap_nr(q->d, nof_cw, q->x, grant->nof_layers, nof_re);
  }

  // SCH decode
  for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
    if (pdsch_nr_decode_codeword(q, cfg, &grant->tb[tb], data, grant->rnti) < SRSRAN_SUCCESS) {
      ERROR("Error encoding TB %d", tb);
      return SRSRAN_ERROR;
    }
  }

  if (q->meas_time_en) {
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    q->meas_time_us = (uint32_t)t[0].tv_usec;
  }

  return SRSRAN_SUCCESS;
}

static uint32_t pdsch_nr_grant_info(const srsran_pdsch_nr_t*     q,
                                    const srsran_sch_cfg_nr_t*   cfg,
                                    const srsran_sch_grant_nr_t* grant,
                                    const srsran_pdsch_res_nr_t* res,
                                    char*                        str,
                                    uint32_t                     str_len)
{
  uint32_t len = 0;

  uint32_t first_prb = SRSRAN_MAX_PRB_NR;
  for (uint32_t i = 0; i < SRSRAN_MAX_PRB_NR && first_prb == SRSRAN_MAX_PRB_NR; i++) {
    if (grant->prb_idx[i]) {
      first_prb = i;
    }
  }

  // Append time-domain resource mapping
  len = srsran_print_check(
      str, str_len, len, "rnti=0x%x prb=%d:%d symb=%d:%d ", grant->rnti, first_prb, grant->nof_prb, grant->S, grant->L);

  // Append TB info
  for (uint32_t i = 0; i < SRSRAN_MAX_TB; i++) {
    len += srsran_sch_nr_tb_info(&grant->tb[i], &res->tb[i], &str[len], str_len - len);

    if (res != NULL) {
      if (grant->tb[i].enabled && !isnan(res->evm[i])) {
        len = srsran_print_check(str, str_len, len, "evm=%.2f ", res->evm[i]);
      }
    }
  }

  return len;
}

uint32_t srsran_pdsch_nr_rx_info(const srsran_pdsch_nr_t*     q,
                                 const srsran_sch_cfg_nr_t*   cfg,
                                 const srsran_sch_grant_nr_t* grant,
                                 const srsran_pdsch_res_nr_t* res,
                                 char*                        str,
                                 uint32_t                     str_len)
{
  uint32_t len = 0;

  len += pdsch_nr_grant_info(q, cfg, grant, res, &str[len], str_len - len);

  if (q->meas_time_en) {
    len = srsran_print_check(str, str_len, len, "t_us=%d ", q->meas_time_us);
  }

  return len;
}

uint32_t srsran_pdsch_nr_tx_info(const srsran_pdsch_nr_t*     q,
                                 const srsran_sch_cfg_nr_t*   cfg,
                                 const srsran_sch_grant_nr_t* grant,
                                 char*                        str,
                                 uint32_t                     str_len)
{
  return srsran_pdsch_nr_rx_info(q, cfg, grant, NULL, str, str_len);
}
