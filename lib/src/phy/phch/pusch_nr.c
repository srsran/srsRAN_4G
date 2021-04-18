/**
 * Copyright 2013-2021 Software Radio Systems Limited
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
#include "srsran/phy/phch/pusch_nr.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/phch/csi.h"
#include "srsran/phy/phch/ra_nr.h"
#include "srsran/phy/phch/uci_cfg.h"

int pusch_nr_init_common(srsran_pusch_nr_t* q, const srsran_pusch_nr_args_t* args)
{
  for (srsran_mod_t mod = SRSRAN_MOD_BPSK; mod < SRSRAN_MOD_NITEMS; mod++) {
    if (srsran_modem_table_lte(&q->modem_tables[mod], mod) < SRSRAN_SUCCESS) {
      ERROR("Error initialising modem table for %s", srsran_mod_string(mod));
      return SRSRAN_ERROR;
    }
    if (args->measure_evm) {
      srsran_modem_table_bytes(&q->modem_tables[mod]);
    }
  }

  if (srsran_uci_nr_init(&q->uci, &args->uci) < SRSRAN_SUCCESS) {
    ERROR("Initialising UCI");
    return SRSRAN_ERROR;
  }

  q->g_ulsch = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
  q->g_ack   = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
  q->g_csi1  = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
  q->g_csi2  = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
  if (q->g_ack == NULL || q->g_csi1 == NULL || q->g_csi2 == NULL || q->g_ulsch == NULL) {
    ERROR("Malloc");
    return SRSRAN_ERROR;
  }

  q->pos_ulsch = srsran_vec_u32_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
  q->pos_ack   = srsran_vec_u32_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
  q->pos_csi1  = srsran_vec_u32_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
  q->pos_csi2  = srsran_vec_u32_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
  if (q->pos_ack == NULL || q->pos_csi1 == NULL || q->pos_csi2 == NULL || q->pos_ulsch == NULL) {
    ERROR("Malloc");
    return SRSRAN_ERROR;
  }

  q->meas_time_en = args->measure_time;

  return SRSRAN_SUCCESS;
}

int srsran_pusch_nr_init_ue(srsran_pusch_nr_t* q, const srsran_pusch_nr_args_t* args)
{
  if (q == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (pusch_nr_init_common(q, args) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_sch_nr_init_tx(&q->sch, &args->sch)) {
    ERROR("Initialising SCH");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_pusch_nr_init_gnb(srsran_pusch_nr_t* q, const srsran_pusch_nr_args_t* args)
{
  if (q == NULL || args == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (pusch_nr_init_common(q, args) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_sch_nr_init_rx(&q->sch, &args->sch) < SRSRAN_SUCCESS) {
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

  return SRSRAN_SUCCESS;
}

int srsran_pusch_nr_set_carrier(srsran_pusch_nr_t* q, const srsran_carrier_nr_t* carrier)
{
  // Set carrier
  q->carrier = *carrier;

  // Reallocate symbols if necessary
  if (q->max_layers < carrier->max_mimo_layers || q->max_prb < carrier->nof_prb) {
    q->max_layers = carrier->max_mimo_layers;
    q->max_prb    = carrier->nof_prb;

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

void srsran_pusch_nr_free(srsran_pusch_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  if (q->g_ulsch != NULL) {
    free(q->g_ulsch);
  }
  if (q->g_ack != NULL) {
    free(q->g_ack);
  }
  if (q->g_csi1 != NULL) {
    free(q->g_csi1);
  }
  if (q->g_csi2 != NULL) {
    free(q->g_csi2);
  }

  if (q->pos_ulsch != NULL) {
    free(q->pos_ulsch);
  }
  if (q->pos_ack != NULL) {
    free(q->pos_ack);
  }
  if (q->pos_csi1 != NULL) {
    free(q->pos_csi1);
  }
  if (q->pos_csi2 != NULL) {
    free(q->pos_csi2);
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
  srsran_uci_nr_free(&q->uci);

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

  SRSRAN_MEM_ZERO(q, srsran_pusch_nr_t, 1);
}

/**
 * @brief copies a number of countiguous Resource Elements
 * @param sf_symbols slot symbols in frequency domain
 * @param symbols resource elements
 * @param count number of resource elements to copy
 * @param put Direction, symbols are copied into sf_symbols if put is true, otherwise sf_symbols are copied into symbols
 */
static void srsran_pusch_re_cp(cf_t* sf_symbols, cf_t* symbols, uint32_t count, bool put)
{
  if (put) {
    srsran_vec_cf_copy(sf_symbols, symbols, count);
  } else {
    srsran_vec_cf_copy(symbols, sf_symbols, count);
  }
}

/*
 * As a RB is 12 RE wide, positions marked as 1 will be used for the 1st CDM group, and the same with group 2:
 *
 *  +---+---+---+---+---+---+---+---+---+---+---+---+
 *  | 1 | 1 | 2 | 2 | 1 | 1 | 2 | 2 | 1 | 1 | 2 | 2 |
 *  +---+---+---+---+---+---+---+---+---+---+---+---+
 *  -- k -->
 *
 * If the number of DMRS CDM groups without data is set to:
 * - 1, data is mapped in RE marked as 2
 * - Otherwise, no data is mapped in this symbol
 */
static uint32_t srsran_pusch_nr_cp_dmrs_type1(const srsran_pusch_nr_t*     q,
                                              const srsran_sch_grant_nr_t* grant,
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
      for (uint32_t j = 0; j < SRSRAN_NRE; j += 2) {
        if (put) {
          sf_symbols[i * SRSRAN_NRE + delta + j + 1] = symbols[count++];
        } else {
          symbols[count++] = sf_symbols[i * SRSRAN_NRE + delta + j + 1];
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
static uint32_t srsran_pusch_nr_cp_dmrs_type2(const srsran_pusch_nr_t*     q,
                                              const srsran_sch_grant_nr_t* grant,
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
      srsran_pusch_re_cp(&sf_symbols[i * SRSRAN_NRE + re_offset], &symbols[count], re_count, put);
      count += re_count;

      // Copy RE after second pilot
      srsran_pusch_re_cp(&sf_symbols[(i + 1) * SRSRAN_NRE - re_count], &symbols[count], re_count, put);
      count += re_count;
    }
  }

  return count;
}

static uint32_t srsran_pusch_nr_cp_dmrs(const srsran_pusch_nr_t*     q,
                                        const srsran_sch_cfg_nr_t*   cfg,
                                        const srsran_sch_grant_nr_t* grant,
                                        cf_t*                        symbols,
                                        cf_t*                        sf_symbols,
                                        bool                         put)
{
  uint32_t count = 0;

  const srsran_dmrs_sch_cfg_t* dmrs_cfg = &cfg->dmrs;

  switch (dmrs_cfg->type) {
    case srsran_dmrs_sch_type_1:
      count = srsran_pusch_nr_cp_dmrs_type1(q, grant, symbols, sf_symbols, put);
      break;
    case srsran_dmrs_sch_type_2:
      count = srsran_pusch_nr_cp_dmrs_type2(q, grant, symbols, sf_symbols, put);
      break;
  }

  return count;
}

static uint32_t srsran_pusch_nr_cp_clean(const srsran_pusch_nr_t*     q,
                                         const srsran_sch_grant_nr_t* grant,
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
        start = i * SRSRAN_NRE;
      }
      length += SRSRAN_NRE;
    } else {
      // Consecutive block is finished
      if (put) {
        srsran_vec_cf_copy(&sf_symbols[start], &symbols[count], length);
      } else {
        srsran_vec_cf_copy(&symbols[count], &sf_symbols[start], length);
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
      srsran_vec_cf_copy(&sf_symbols[start], &symbols[count], length);
    } else {
      srsran_vec_cf_copy(&symbols[count], &sf_symbols[start], length);
    }
    count += length;
  }

  return count;
}

static int srsran_pusch_nr_cp(const srsran_pusch_nr_t*     q,
                              const srsran_sch_cfg_nr_t*   cfg,
                              const srsran_sch_grant_nr_t* grant,
                              cf_t*                        symbols,
                              cf_t*                        sf_symbols,
                              bool                         put)
{
  uint32_t count                                   = 0;
  uint32_t dmrs_l_idx[SRSRAN_DMRS_SCH_MAX_SYMBOLS] = {};
  uint32_t dmrs_l_count                            = 0;

  // Get symbol indexes carrying DMRS
  int32_t nof_dmrs_symbols = srsran_dmrs_sch_get_symbols_idx(&cfg->dmrs, grant, dmrs_l_idx);
  if (nof_dmrs_symbols < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (SRSRAN_DEBUG_ENABLED && srsran_verbose >= SRSRAN_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("dmrs_l_idx=");
    srsran_vec_fprint_i(stdout, (int32_t*)dmrs_l_idx, nof_dmrs_symbols);
  }

  for (uint32_t l = grant->S; l < grant->S + grant->L; l++) {
    // Advance DMRS symbol counter until:
    // - the current DMRS symbol index is greater or equal than current symbol l
    // - no more DMRS symbols
    while (dmrs_l_idx[dmrs_l_count] < l && dmrs_l_count < nof_dmrs_symbols) {
      dmrs_l_count++;
    }

    if (l == dmrs_l_idx[dmrs_l_count]) {
      count += srsran_pusch_nr_cp_dmrs(
          q, cfg, grant, &symbols[count], &sf_symbols[l * q->carrier.nof_prb * SRSRAN_NRE], put);
    } else {
      count +=
          srsran_pusch_nr_cp_clean(q, grant, &symbols[count], &sf_symbols[l * q->carrier.nof_prb * SRSRAN_NRE], put);
    }
  }

  return count;
}

static int srsran_pusch_nr_put(const srsran_pusch_nr_t*     q,
                               const srsran_sch_cfg_nr_t*   cfg,
                               const srsran_sch_grant_nr_t* grant,
                               cf_t*                        symbols,
                               cf_t*                        sf_symbols)
{
  return srsran_pusch_nr_cp(q, cfg, grant, symbols, sf_symbols, true);
}

static int srsran_pusch_nr_get(const srsran_pusch_nr_t*     q,
                               const srsran_sch_cfg_nr_t*   cfg,
                               const srsran_sch_grant_nr_t* grant,
                               cf_t*                        symbols,
                               cf_t*                        sf_symbols)
{
  return srsran_pusch_nr_cp(q, cfg, grant, symbols, sf_symbols, false);
}

static uint32_t
pusch_nr_cinit(const srsran_carrier_nr_t* carrier, const srsran_sch_cfg_nr_t* cfg, uint16_t rnti, uint32_t cw_idx)
{
  uint32_t n_id = carrier->id;
  if (cfg->scrambling_id_present && SRSRAN_RNTI_ISUSER(rnti)) {
    n_id = cfg->scambling_id;
  }
  uint32_t cinit = (((uint32_t)rnti) << 15U) + (cw_idx << 14U) + n_id;

  INFO("PUSCH: RNTI=%d (0x%x); nid=%d; cinit=%d (0x%x);", rnti, rnti, n_id, cinit, cinit);

  return cinit;
}

static inline int pusch_nr_fill_uci_cfg(srsran_pusch_nr_t* q, const srsran_sch_cfg_nr_t* cfg)
{
  if (cfg->grant.nof_prb == 0) {
    ERROR("Invalid number of PRB (%d)", cfg->grant.nof_prb);
    return SRSRAN_ERROR;
  }

  // Initially, copy all fields
  q->uci_cfg = cfg->uci;

  // Reset UCI PUSCH configuration
  SRSRAN_MEM_ZERO(&q->uci_cfg.pusch, srsran_uci_nr_pusch_cfg_t, 1);

  // Get DMRS symbol indexes
  uint32_t nof_dmrs_l                          = 0;
  uint32_t dmrs_l[SRSRAN_DMRS_SCH_MAX_SYMBOLS] = {};
  int      n                                   = srsran_dmrs_sch_get_symbols_idx(&cfg->dmrs, &cfg->grant, dmrs_l);
  if (n < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }
  nof_dmrs_l = (uint32_t)n;

  // Find OFDM symbol index of the first OFDM symbol after the first set of consecutive OFDM symbol(s) carrying DMRS
  // Starts at first OFDM symbol carrying DMRS
  for (uint32_t l = dmrs_l[0], dmrs_l_idx = 0; l < cfg->grant.S + cfg->grant.L; l++) {
    // Check if it is not carrying DMRS...
    if (l != dmrs_l[dmrs_l_idx]) {
      // Set value and stop iterating
      q->uci_cfg.pusch.l0 = l;
      break;
    }

    // Move to the next DMRS OFDM symbol index
    if (dmrs_l_idx < nof_dmrs_l) {
      dmrs_l_idx++;
    }
  }

  // Find OFDM symbol index of the first OFDM symbol that does not carry DMRS
  // Starts at first OFDM symbol of the PUSCH transmission
  for (uint32_t l = cfg->grant.S, dmrs_l_idx = 0; l < cfg->grant.S + cfg->grant.L; l++) {
    // Check if it is not carrying DMRS...
    if (l != dmrs_l[dmrs_l_idx]) {
      q->uci_cfg.pusch.l1 = l;
      break;
    }

    // Move to the next DMRS OFDM symbol index
    if (dmrs_l_idx < nof_dmrs_l) {
      dmrs_l_idx++;
    }
  }

  // Number of DMRS per PRB
  uint32_t n_sc_dmrs = SRSRAN_DMRS_SCH_SC(cfg->grant.nof_dmrs_cdm_groups_without_data, cfg->dmrs.type);

  // Set UCI RE number of candidates per OFDM symbol according to TS 38.312 6.3.2.4.2.1
  for (uint32_t l = 0, dmrs_l_idx = 0; l < SRSRAN_NSYMB_PER_SLOT_NR; l++) {
    // Skip if OFDM symbol is outside of the PUSCH transmission
    if (l < cfg->grant.S || l >= (cfg->grant.S + cfg->grant.L)) {
      q->uci_cfg.pusch.M_pusch_sc[l] = 0;
      q->uci_cfg.pusch.M_uci_sc[l]   = 0;
      continue;
    }

    // OFDM symbol carries DMRS
    if (l == dmrs_l[dmrs_l_idx]) {
      // Calculate PUSCH RE candidates
      q->uci_cfg.pusch.M_pusch_sc[l] = cfg->grant.nof_prb * (SRSRAN_NRE - n_sc_dmrs);

      // The Number of RE candidates for UCI are 0
      q->uci_cfg.pusch.M_uci_sc[l] = 0;

      // Advance DMRS symbol index
      dmrs_l_idx++;

      // Skip to next symbol
      continue;
    }

    // Number of RE for Phase Tracking Reference Signals (PT-RS)
    uint32_t M_ptrs_sc = 0; // Not implemented yet

    // Number of RE given by the grant
    q->uci_cfg.pusch.M_pusch_sc[l] = cfg->grant.nof_prb * SRSRAN_NRE;

    // Calculate the number of UCI candidates
    q->uci_cfg.pusch.M_uci_sc[l] = q->uci_cfg.pusch.M_pusch_sc[l] - M_ptrs_sc;
  }

  // Generate SCH Transport block information
  srsran_sch_nr_tb_info_t sch_tb_info = {};
  if (srsran_sch_nr_fill_tb_info(&q->carrier, &cfg->sch_cfg, &cfg->grant.tb[0], &sch_tb_info) < SRSRAN_SUCCESS) {
    ERROR("Generating TB info");
    return SRSRAN_ERROR;
  }

  // Calculate the sum of codeblock sizes
  for (uint32_t i = 0; i < sch_tb_info.C; i++) {
    // Accumulate codeblock size if mask is enabled
    q->uci_cfg.pusch.K_sum += (sch_tb_info.mask[i]) ? sch_tb_info.Kr : 0;
  }

  // Set other PUSCH parameters
  q->uci_cfg.pusch.modulation           = cfg->grant.tb[0].mod;
  q->uci_cfg.pusch.nof_layers           = cfg->grant.nof_layers;
  q->uci_cfg.pusch.R                    = (float)cfg->grant.tb[0].R;
  q->uci_cfg.pusch.alpha                = cfg->scaling;
  q->uci_cfg.pusch.beta_harq_ack_offset = cfg->beta_harq_ack_offset;
  q->uci_cfg.pusch.beta_csi1_offset     = cfg->beta_csi_part1_offset;
  q->uci_cfg.pusch.nof_re               = cfg->grant.tb[0].nof_re;

  return SRSRAN_SUCCESS;
}

// Implements TS 38.212 6.2.7 Data and control multiplexing (for NR-PUSCH)
static int pusch_nr_gen_mux_uci(srsran_pusch_nr_t* q, const srsran_uci_cfg_nr_t* cfg)
{
  // Decide whether UCI shall be multiplexed
  q->uci_mux = (q->G_ack > 0 || q->G_csi1 > 0 || q->G_csi2 > 0);

  // Check if UCI multiplexing is NOT required
  if (!q->uci_mux) {
    return SRSRAN_SUCCESS;
  }

  // Bit positions
  uint32_t* pos_ulsch = q->pos_ulsch; // coded bits for UL-SCH
  uint32_t* pos_ack   = q->pos_ack;   // coded bits for HARQ-ACK
  uint32_t* pos_csi1  = q->pos_csi1;  // coded bits for CSI part 1
  uint32_t* pos_csi2  = q->pos_csi2;  // coded bits for CSI part 2

  // Key OFDM symbol indexes
  uint32_t l1 =
      cfg->pusch.l0; // First OFDM symbol that does not carry DMRS of the PUSCH, after the first DMRS symbol(s)
  uint32_t l1_csi = cfg->pusch.l1; // OFDM symbol index of the first OFDM symbol that does not carry DMRS

  // Number of UCI bits
  uint32_t G_ack  = q->G_ack;
  uint32_t G_csi1 = q->G_csi1;
  uint32_t G_csi2 = q->G_csi2;

  // Other...
  uint32_t Nl = cfg->pusch.nof_layers;
  uint32_t Qm = srsran_mod_bits_x_symbol(cfg->pusch.modulation);

  // if the number of HARQ-ACK information bits to be transmitted on PUSCH is 0, 1 or 2 bits
  uint32_t G_ack_rvd = 0;
  if (cfg->o_ack <= 2) {
    // the number of reserved resource elements for potential HARQ-ACK transmission is calculated according to Clause
    // 6.3.2.4.2.1, by setting O_ACK = 2 ;
    G_ack_rvd = srsran_uci_nr_pusch_ack_nof_bits(&q->uci_cfg.pusch, 2);

    // Disable non reserved HARQ-ACK bits
    G_ack = 0;
  }

  // Counters
  uint32_t m_ack_count   = 0;
  uint32_t m_csi1_count  = 0;
  uint32_t m_csi2_count  = 0;
  uint32_t m_ulsch_count = 0;
  uint32_t m_all_count   = 0;

  for (uint32_t l = 0; l < SRSRAN_NSYMB_PER_SLOT_NR; l++) {
    // Skip if symbol has potential for data
    if (cfg->pusch.M_pusch_sc[l] == 0) {
      continue;
    }

    // Put UL-SCH only if this OFDM symbol has no potential for UCI
    if (cfg->pusch.M_uci_sc[l] == 0) {
      for (uint32_t i = 0; i < cfg->pusch.M_pusch_sc[l] * Qm * Nl; i++) {
        pos_ulsch[m_ulsch_count++] = m_all_count++;
      }
      continue;
    }

    uint32_t M_ulsch_sc = cfg->pusch.M_pusch_sc[l];
    uint32_t M_uci_sc   = cfg->pusch.M_uci_sc[l];
    uint32_t M_uci_rvd  = 0;

    // Compute HARQ-ACK bits multiplexing
    uint32_t ack_d          = 0;
    uint32_t ack_m_re_count = 0;
    if (l >= l1) {
      if (cfg->o_ack <= 2 && m_ack_count < G_ack_rvd) {
        ack_d          = 1;
        ack_m_re_count = M_ulsch_sc;
        if (G_ack_rvd - m_ack_count < M_uci_sc * Nl * Qm) {
          ack_d          = (M_uci_sc * Nl * Qm) / (G_ack_rvd - m_ack_count);
          ack_m_re_count = SRSRAN_CEIL(G_ack_rvd - m_ack_count, Nl * Qm);
        }
        M_uci_rvd = ack_m_re_count;
      } else if (m_ack_count < G_ack) {
        ack_d          = 1;
        ack_m_re_count = M_ulsch_sc;
        if (G_ack - m_ack_count < M_uci_sc * Nl * Qm) {
          ack_d          = (M_uci_sc * Nl * Qm) / (G_ack - m_ack_count);
          ack_m_re_count = SRSRAN_CEIL(G_ack - m_ack_count, Nl * Qm);
        }
        M_uci_sc -= ack_m_re_count;
      }
    }

    // Compute CSI part 1 bits multiplexing
    uint32_t csi1_d          = 0;
    uint32_t csi1_m_re_count = 0;
    if (l >= l1_csi && M_uci_sc > M_uci_rvd && m_csi1_count < G_csi1) {
      csi1_d          = 1;
      csi1_m_re_count = M_uci_sc - M_uci_rvd;
      if (G_csi1 - m_csi1_count < (M_uci_sc - M_uci_rvd) * Nl * Qm) {
        csi1_d          = ((M_uci_sc - M_uci_rvd) * Nl * Qm) / (G_csi1 - m_csi1_count);
        csi1_m_re_count = SRSRAN_CEIL(G_csi1 - m_csi1_count, Nl * Qm);
      }
      M_uci_sc -= csi1_m_re_count;
    }

    // Compute CSI part 2 bits multiplexing
    uint32_t csi2_d          = 0;
    uint32_t csi2_m_re_count = 0;
    if (l >= l1_csi && M_uci_sc > M_uci_rvd && m_csi2_count < G_csi2) {
      csi2_d          = 1;
      csi2_m_re_count = M_uci_sc - M_uci_rvd;
      if (G_csi2 - m_csi2_count < (M_uci_sc - M_uci_rvd) * Nl * Qm) {
        csi2_d          = ((M_uci_sc - M_uci_rvd) * Nl * Qm) / (G_csi2 - m_csi2_count);
        csi2_m_re_count = SRSRAN_CEIL(G_csi2 - m_csi2_count, Nl * Qm);
      }
      M_uci_sc -= csi2_m_re_count;
    }

    // Leave the rest for UL-SCH
    uint32_t ulsch_m_re_count = M_uci_sc;

    for (uint32_t i = 0, csi1_i = 0, csi2_i = 0; i < cfg->pusch.M_pusch_sc[l]; i++) {
      // Check if RE is reserved for ACK
      bool reserved = false;
      if (ack_m_re_count != 0 && i % ack_d == 0 && m_ack_count < G_ack_rvd) {
        reserved = true;
      }

      if (ack_m_re_count != 0 && i % ack_d == 0 && m_ack_count < G_ack) {
        for (uint32_t j = 0; j < Nl * Qm; j++) {
          pos_ack[m_ack_count++] = m_all_count + j;
        }
        ack_m_re_count--;
      } else if (!reserved && csi1_m_re_count != 0 && csi1_i % csi1_d == 0 && m_csi1_count < G_csi1) {
        for (uint32_t j = 0; j < Nl * Qm; j++) {
          pos_csi1[m_csi1_count++] = m_all_count + j;
        }
        csi1_m_re_count--;
        csi1_i++;
      } else if (!reserved && csi2_m_re_count != 0 && csi2_i % csi2_d == 0 && m_csi2_count < G_csi2) {
        for (uint32_t j = 0; j < Nl * Qm; j++) {
          pos_csi2[m_csi2_count++] = m_all_count + j;
        }
        csi2_m_re_count--;
        csi1_i++;
        csi2_i++;
      } else {
        for (uint32_t j = 0; j < Nl * Qm; j++) {
          pos_ulsch[m_ulsch_count++] = m_all_count + j;
        }
        ulsch_m_re_count--;
        if (!reserved) {
          csi1_i++;
          csi2_i++;
        }
      }

      // Set reserved bits only if there are ACK bits
      if (reserved) {
        if (cfg->o_ack > 0) {
          for (uint32_t j = 0; j < Nl * Qm; j++) {
            pos_ack[m_ack_count++] = m_all_count + j;
          }
        } else {
          m_ack_count += Nl * Qm;
        }
        ack_m_re_count--;
      }

      // Increment all bit counter
      m_all_count += Nl * Qm;
    }

    // Assert that all RE have been allocated
    if (ack_m_re_count != 0) {
      ERROR("ack_m_re_count=%d", ack_m_re_count);
    }
    if (csi1_m_re_count != 0) {
      ERROR("csi1_m_re_count=%d", csi1_m_re_count);
    }
    if (csi2_m_re_count != 0) {
      ERROR("csi2_m_re_count=%d", csi2_m_re_count);
    }
    if (ulsch_m_re_count != 0) {
      ERROR("ulsch_m_re_count=%d", ulsch_m_re_count);
    }
  }

  // Update UL-SCH number of encoded bits
  q->G_ulsch = m_ulsch_count;

  // Assert Number of bits
  if (G_ack_rvd != 0 && G_ack_rvd != m_ack_count && cfg->o_ack > 0) {
    ERROR("Not matched %d!=%d", G_ack_rvd, m_ack_count);
  }
  if (G_ack != 0 && G_ack != m_ack_count) {
    ERROR("Not matched %d!=%d", G_ack, m_ack_count);
  }
  q->G_csi1 = m_csi1_count;
  if (G_csi1 != 0 && G_csi1 != m_csi1_count) {
    ERROR("Not matched %d!=%d", G_csi1, m_csi1_count);
  }
  if (G_csi2 != 0 && G_csi2 != m_csi2_count) {
    ERROR("Not matched %d!=%d", G_csi2, m_csi2_count);
  }

  // Print debug information if configured for ity
  if (SRSRAN_DEBUG_ENABLED && srsran_verbose >= SRSRAN_VERBOSE_DEBUG && !handler_registered) {
    if (m_ulsch_count != 0) {
      DEBUG("UL-SCH bit positions:");
      srsran_vec_fprint_i(stdout, (int*)pos_ulsch, m_ulsch_count);
    }
    if (m_ack_count != 0 && cfg->o_ack > 0) {
      DEBUG("HARQ-ACK bit positions [%d]:", m_ack_count);
      srsran_vec_fprint_i(stdout, (int*)pos_ack, m_ack_count);
    }
    if (m_csi1_count != 0) {
      DEBUG("CSI part 1 bit positions [%d]:", m_csi1_count);
      srsran_vec_fprint_i(stdout, (int*)pos_csi1, m_csi1_count);
    }
    if (m_csi2_count != 0) {
      DEBUG("CSI part 2 bit positions [%d]:", m_csi2_count);
      srsran_vec_fprint_i(stdout, (int*)pos_csi2, m_csi2_count);
    }
  }

  return SRSRAN_SUCCESS;
}

static inline int pusch_nr_encode_codeword(srsran_pusch_nr_t*           q,
                                           const srsran_sch_cfg_nr_t*   cfg,
                                           const srsran_sch_tb_t*       tb,
                                           const uint8_t*               data,
                                           const srsran_uci_value_nr_t* uci,
                                           uint16_t                     rnti)
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

  // Encode HARQ-ACK bits
  int E_uci_ack = srsran_uci_nr_encode_pusch_ack(&q->uci, &q->uci_cfg, uci, q->g_ack);
  if (E_uci_ack < SRSRAN_SUCCESS) {
    ERROR("Error encoding HARQ-ACK bits");
    return SRSRAN_ERROR;
  }
  q->G_ack = (uint32_t)E_uci_ack;

  // Encode CSI part 1
  int E_uci_csi1 = srsran_uci_nr_encode_pusch_csi1(&q->uci, &q->uci_cfg, uci, q->g_csi1);
  if (E_uci_csi1 < SRSRAN_SUCCESS) {
    ERROR("Error encoding HARQ-ACK bits");
    return SRSRAN_ERROR;
  }
  q->G_csi1 = (uint32_t)E_uci_csi1;

  // Encode CSI part 2
  // ... Not implemented
  q->G_csi2 = 0;

  // Generate PUSCH UCI/UL-SCH multiplexing
  if (pusch_nr_gen_mux_uci(q, &q->uci_cfg) < SRSRAN_SUCCESS) {
    ERROR("Error generating PUSCH mux tables");
    return SRSRAN_ERROR;
  }

  // Encode SCH
  if (srsran_ulsch_nr_encode(&q->sch, &cfg->sch_cfg, tb, data, q->g_ulsch) < SRSRAN_SUCCESS) {
    ERROR("Error in SCH encoding");
    return SRSRAN_ERROR;
  }

  // Multiplex UL-SCH with UCI only if it is necessary
  uint8_t* b = q->g_ulsch;
  if (q->uci_mux) {
    // Change b location
    b = q->b[tb->cw_idx];

    // Multiplex UL-SCH
    for (uint32_t i = 0; i < q->G_ulsch; i++) {
      b[q->pos_ulsch[i]] = q->g_ulsch[i];
    }

    // Multiplex CSI part 1
    for (uint32_t i = 0; i < q->G_csi1; i++) {
      b[q->pos_csi1[i]] = q->g_csi1[i];
    }

    // Multiplex CSI part 2
    for (uint32_t i = 0; i < q->G_csi2; i++) {
      b[q->pos_csi2[i]] = q->g_csi2[i];
    }

    // Multiplex HARQ-ACK
    for (uint32_t i = 0; i < q->G_ack; i++) {
      b[q->pos_ack[i]] = q->g_ack[i];
    }
  }

  if (SRSRAN_DEBUG_ENABLED && srsran_verbose >= SRSRAN_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("b=");
    srsran_vec_fprint_b(stdout, b, tb->nof_bits);
  }

  // 7.3.1.1 Scrambling
  uint32_t cinit = pusch_nr_cinit(&q->carrier, cfg, rnti, tb->cw_idx);
  srsran_sequence_apply_bit(b, q->b[tb->cw_idx], tb->nof_bits, cinit);

  // Special Scrambling condition
  if (q->uci_cfg.o_ack <= 2) {
    for (uint32_t i = 0; i < q->G_ack; i++) {
      uint32_t idx = q->pos_ack[i];
      if (q->g_ack[i] == (uint8_t)UCI_BIT_REPETITION) {
        if (idx != 0) {
          q->b[tb->cw_idx][idx] = q->b[tb->cw_idx][idx - 1];
        }
      } else if (q->g_ack[i] == (uint8_t)UCI_BIT_PLACEHOLDER) {
        q->b[tb->cw_idx][idx] = 1;
      }
    }
  }

  // 7.3.1.2 Modulation
  srsran_mod_modulate(&q->modem_tables[tb->mod], q->b[tb->cw_idx], q->d[tb->cw_idx], tb->nof_bits);

  if (SRSRAN_DEBUG_ENABLED && srsran_verbose >= SRSRAN_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("d=");
    srsran_vec_fprint_c(stdout, q->d[tb->cw_idx], tb->nof_re);
  }

  return SRSRAN_SUCCESS;
}

int srsran_pusch_nr_encode(srsran_pusch_nr_t*            q,
                           const srsran_sch_cfg_nr_t*    cfg,
                           const srsran_sch_grant_nr_t*  grant,
                           const srsran_pusch_data_nr_t* data,
                           cf_t*                         sf_symbols[SRSRAN_MAX_PORTS])
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

  // Fill UCI configuration for PUSCH configuration
  if (pusch_nr_fill_uci_cfg(q, cfg) < SRSRAN_SUCCESS) {
    ERROR("Error filling UCI configuration for PUSCH");
    return SRSRAN_ERROR;
  }

  // 7.3.1.1 and 7.3.1.2
  uint32_t nof_cw = 0;
  for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
    nof_cw += grant->tb[tb].enabled ? 1 : 0;

    if (pusch_nr_encode_codeword(q, cfg, &grant->tb[tb], data->payload[tb], &data[0].uci, grant->rnti) <
        SRSRAN_SUCCESS) {
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
  int n = srsran_pusch_nr_put(q, cfg, grant, x[0], sf_symbols[0]);
  if (n < SRSRAN_SUCCESS) {
    ERROR("Putting NR PUSCH resources");
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

static inline int pusch_nr_decode_codeword(srsran_pusch_nr_t*         q,
                                           const srsran_sch_cfg_nr_t* cfg,
                                           const srsran_sch_tb_t*     tb,
                                           srsran_pusch_res_nr_t*     res,
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

  // Calculate HARQ-ACK bits
  int n = srsran_uci_nr_pusch_ack_nof_bits(&q->uci_cfg.pusch, q->uci_cfg.o_ack);
  if (n < SRSRAN_SUCCESS) {
    ERROR("Calculating G_ack");
    return SRSRAN_ERROR;
  }
  q->G_ack = (uint32_t)n;

  // Calculate CSI part 1 bits
  n = srsran_uci_nr_pusch_csi1_nof_bits(&q->uci_cfg);
  if (n < SRSRAN_SUCCESS) {
    ERROR("Calculating G_csi1");
    return SRSRAN_ERROR;
  }
  q->G_csi1 = (uint32_t)n;

  // Calculate CSI part 2 bits
  // ... Not implemented
  q->G_csi2 = 0;

  // Generate PUSCH UCI/UL-SCH multiplexing
  if (pusch_nr_gen_mux_uci(q, &q->uci_cfg) < SRSRAN_SUCCESS) {
    ERROR("Error generating PUSCH mux tables");
    return SRSRAN_ERROR;
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

  // Descrambling
  srsran_sequence_apply_c(llr, llr, tb->nof_bits, pusch_nr_cinit(&q->carrier, cfg, rnti, tb->cw_idx));

  if (SRSRAN_DEBUG_ENABLED && srsran_verbose >= SRSRAN_VERBOSE_DEBUG && !handler_registered) {
    DEBUG("b=");
    srsran_vec_fprint_bs(stdout, llr, tb->nof_bits);
  }

  // Demultiplex UCI only if necessary
  if (q->uci_mux) {
    // Demultiplex UL-SCH, change sign
    int8_t* g_ulsch = (int8_t*)q->g_ulsch;
    for (uint32_t i = 0; i < q->G_ulsch; i++) {
      g_ulsch[i] = -llr[q->pos_ulsch[i]];
    }
    for (uint32_t i = q->G_ulsch; i < tb->nof_bits; i++) {
      g_ulsch[i] = 0;
    }

    // Demultiplex HARQ-ACK
    int8_t* g_ack = (int8_t*)q->g_ack;
    for (uint32_t i = 0; i < q->G_ack; i++) {
      g_ack[i] = llr[q->pos_ack[i]];
    }

    // Demultiplex CSI part 1
    int8_t* g_csi1 = (int8_t*)q->g_csi1;
    for (uint32_t i = 0; i < q->G_csi1; i++) {
      g_csi1[i] = llr[q->pos_csi1[i]];
    }

    // Demultiplex CSI part 2
    int8_t* g_csi2 = (int8_t*)q->g_csi2;
    for (uint32_t i = 0; i < q->G_csi2; i++) {
      g_csi2[i] = llr[q->pos_csi2[i]];
    }

    // Decode HARQ-ACK
    if (q->G_ack) {
      if (srsran_uci_nr_decode_pusch_ack(&q->uci, &q->uci_cfg, g_ack, &res->uci)) {
        ERROR("Error in UCI decoding");
        return SRSRAN_ERROR;
      }
    }

    // Decode CSI part 1
    if (q->G_csi1) {
      if (srsran_uci_nr_decode_pusch_csi1(&q->uci, &q->uci_cfg, g_csi1, &res->uci)) {
        ERROR("Error in UCI decoding");
        return SRSRAN_ERROR;
      }
    }

    // Decode CSI part 2
    // ... Not implemented

    // Change LLR pointer
    llr = g_ulsch;
  } else {
    for (uint32_t i = 0; i < tb->nof_bits; i++) {
      llr[i] *= -1;
    }
  }

  // Decode Ul-SCH
  if (tb->nof_bits != 0) {
    if (srsran_ulsch_nr_decode(&q->sch, &cfg->sch_cfg, tb, llr, &res->tb[tb->cw_idx]) < SRSRAN_SUCCESS) {
      ERROR("Error in SCH decoding");
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_pusch_nr_decode(srsran_pusch_nr_t*           q,
                           const srsran_sch_cfg_nr_t*   cfg,
                           const srsran_sch_grant_nr_t* grant,
                           srsran_chest_dl_res_t*       channel,
                           cf_t*                        sf_symbols[SRSRAN_MAX_PORTS],
                           srsran_pusch_res_nr_t        data[SRSRAN_MAX_TB])
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

  // Fill UCI configuration for PUSCH configuration
  if (pusch_nr_fill_uci_cfg(q, cfg) < SRSRAN_SUCCESS) {
    ERROR("Error filling UCI configuration for PUSCH");
    return SRSRAN_ERROR;
  }

  uint32_t nof_cw = 0;
  for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
    nof_cw += grant->tb[tb].enabled ? 1 : 0;
  }

  uint32_t nof_re = srsran_ra_dl_nr_slot_nof_re(cfg, grant);

  if (channel->nof_re != nof_re) {
    ERROR("Inconsistent number of RE (%d!=%d)", channel->nof_re, nof_re);
    return SRSRAN_ERROR;
  }

  // Demapping from virtual to physical resource blocks
  uint32_t nof_re_get = srsran_pusch_nr_get(q, cfg, grant, q->x[0], sf_symbols[0]);
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
    nof_cw += grant->tb[tb].enabled ? 1 : 0;

    if (pusch_nr_decode_codeword(q, cfg, &grant->tb[tb], &data[tb], grant->rnti) < SRSRAN_SUCCESS) {
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

static uint32_t srsran_pusch_nr_grant_info(const srsran_sch_cfg_nr_t*   cfg,
                                           const srsran_sch_grant_nr_t* grant,
                                           const srsran_pusch_res_nr_t* res,
                                           char*                        str,
                                           uint32_t                     str_len)
{
  uint32_t len = 0;
  len          = srsran_print_check(str, str_len, len, "rnti=0x%x", grant->rnti);

  uint32_t first_prb = SRSRAN_MAX_PRB_NR;
  for (uint32_t i = 0; i < SRSRAN_MAX_PRB_NR && first_prb == SRSRAN_MAX_PRB_NR; i++) {
    if (grant->prb_idx[i]) {
      first_prb = i;
    }
  }

  // Append time-domain resource mapping
  len = srsran_print_check(str,
                           str_len,
                           len,
                           ",k2=%d,prb=%d:%d,S=%d,L=%d,mapping=%s",
                           grant->k,
                           first_prb,
                           grant->nof_prb,
                           grant->S,
                           grant->L,
                           srsran_sch_mapping_type_to_str(grant->mapping));

  // Skip frequency domain resources...
  // ...

  // Append spatial resources
  len = srsran_print_check(str, str_len, len, ",Nl=%d", grant->nof_layers);

  // Append scrambling ID
  len = srsran_print_check(str, str_len, len, ",n_scid=%d,", grant->n_scid);

  // Append TB info
  for (uint32_t i = 0; i < SRSRAN_MAX_TB; i++) {
    len += srsran_sch_nr_tb_info(&grant->tb[i], &res->tb[i], &str[len], str_len - len);
  }

  return len;
}

uint32_t srsran_pusch_nr_rx_info(const srsran_pusch_nr_t*     q,
                                 const srsran_sch_cfg_nr_t*   cfg,
                                 const srsran_sch_grant_nr_t* grant,
                                 const srsran_pusch_res_nr_t* res,
                                 char*                        str,
                                 uint32_t                     str_len)
{
  uint32_t len = 0;

  if (q == NULL || cfg == NULL || grant == NULL || str == NULL || str_len == 0) {
    return 0;
  }

  len += srsran_pusch_nr_grant_info(cfg, grant, res, &str[len], str_len - len);

  if (q->evm_buffer != NULL) {
    len = srsran_print_check(str, str_len, len, ",evm={", 0);
    for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
      if (grant->tb[i].enabled && !isnan(res->evm[i])) {
        len = srsran_print_check(str, str_len, len, "%.2f", res[i].evm);
        if (i < SRSRAN_MAX_CODEWORDS - 1) {
          if (grant->tb[i + 1].enabled) {
            len = srsran_print_check(str, str_len, len, ",", 0);
          }
        }
      }
    }
    len = srsran_print_check(str, str_len, len, "}", 0);
  }

  if (res != NULL) {
    srsran_uci_data_nr_t uci_data = {};
    uci_data.cfg                  = cfg->uci;
    uci_data.value                = res[0].uci;
    len += srsran_uci_nr_info(&uci_data, &str[len], str_len - len);

    len = srsran_print_check(str, str_len, len, ",crc={", 0);
    for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
      if (grant->tb[i].enabled) {
        len = srsran_print_check(str, str_len, len, "%s", res->tb[i].crc ? "OK" : "KO");
        if (i < SRSRAN_MAX_CODEWORDS - 1) {
          if (grant->tb[i + 1].enabled) {
            len = srsran_print_check(str, str_len, len, ",", 0);
          }
        }
      }
    }
    len = srsran_print_check(str, str_len, len, "}", 0);
  }

  if (q->meas_time_en) {
    len = srsran_print_check(str, str_len, len, ", t=%d us", q->meas_time_us);
  }

  return len;
}

uint32_t srsran_pusch_nr_tx_info(const srsran_pusch_nr_t*     q,
                                 const srsran_sch_cfg_nr_t*   cfg,
                                 const srsran_sch_grant_nr_t* grant,
                                 const srsran_uci_value_nr_t* uci_value,
                                 char*                        str,
                                 uint32_t                     str_len)
{
  uint32_t len = 0;

  if (q == NULL || cfg == NULL || grant == NULL || str == NULL || str_len == 0) {
    return 0;
  }

  len += srsran_pusch_nr_grant_info(cfg, grant, NULL, &str[len], str_len - len);

  if (uci_value != NULL) {
    srsran_uci_data_nr_t uci_data = {};
    uci_data.cfg                  = cfg->uci;
    uci_data.value                = *uci_value;
    len += srsran_uci_nr_info(&uci_data, &str[len], str_len - len);
  }

  if (q->meas_time_en) {
    len = srsran_print_check(str, str_len, len, ", t=%d us", q->meas_time_us);
  }

  return len;
}
