/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
#include "srsran/phy/phch/pusch_nr.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/phch/csi.h"
#include "srsran/phy/phch/ra_nr.h"
#include "srsran/phy/phch/uci_cfg.h"

static int pusch_nr_alloc(srsran_pusch_nr_t* q, uint32_t max_mimo_layers, uint32_t max_prb)
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

  if (pusch_nr_alloc(q, args->max_layers, args->max_prb) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
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

  if (pusch_nr_alloc(q, carrier->max_mimo_layers, carrier->nof_prb) < SRSRAN_SUCCESS) {
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

static inline uint32_t pusch_nr_put_rb(cf_t* dst, cf_t* src, bool* rvd_mask)
{
  uint32_t count = 0;
  for (uint32_t i = 0; i < SRSRAN_NRE; i++) {
    if (!rvd_mask[i]) {
      dst[i] = src[count++];
    }
  }
  return count;
}

static inline uint32_t pusch_nr_get_rb(cf_t* dst, cf_t* src, bool* rvd_mask)
{
  uint32_t count = 0;
  for (uint32_t i = 0; i < SRSRAN_NRE; i++) {
    if (!rvd_mask[i]) {
      dst[count++] = src[i];
    }
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
        count += pusch_nr_put_rb(&sf_symbols[re_idx], &symbols[count], &rvd_mask[rb * SRSRAN_NRE]);
      } else {
        count += pusch_nr_get_rb(&symbols[count], &sf_symbols[re_idx], &rvd_mask[rb * SRSRAN_NRE]);
      }
    }
  }

  return count;
}

static int pusch_nr_put(const srsran_pusch_nr_t*     q,
                        const srsran_sch_cfg_nr_t*   cfg,
                        const srsran_sch_grant_nr_t* grant,
                        cf_t*                        symbols,
                        cf_t*                        sf_symbols)
{
  return srsran_pusch_nr_cp(q, cfg, grant, symbols, sf_symbols, true);
}

static int pusch_nr_get(const srsran_pusch_nr_t*     q,
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
  uint32_t n_id = carrier->pci;
  if (cfg->scrambling_id_present && SRSRAN_RNTI_ISUSER(rnti)) {
    n_id = cfg->scambling_id;
  }
  uint32_t cinit = (((uint32_t)rnti) << 15U) + (cw_idx << 14U) + n_id;

  INFO("PUSCH: RNTI=%d (0x%x); nid=%d; cinit=%d (0x%x);", rnti, rnti, n_id, cinit, cinit);

  return cinit;
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
  if (cfg->ack.count <= 2) {
    // the number of reserved resource elements for potential HARQ-ACK transmission is calculated according to Clause
    // 6.3.2.4.2.1, by setting O_ACK = 2 ;
    G_ack_rvd = srsran_uci_nr_pusch_ack_nof_bits(&cfg->pusch, 2);
  }

  // Counters
  uint32_t m_ack_count   = 0;
  uint32_t m_rvd_count   = 0;
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
    uint32_t rvd_d          = 0;
    uint32_t rvd_m_re_count = 0;
    if (l >= l1) {
      if (cfg->ack.count <= 2 && m_rvd_count < G_ack_rvd) {
        rvd_d          = 1;
        rvd_m_re_count = M_ulsch_sc;
        if (G_ack_rvd - m_rvd_count < M_uci_sc * Nl * Qm) {
          rvd_d          = (M_uci_sc * Nl * Qm) / (G_ack_rvd - m_rvd_count);
          rvd_m_re_count = SRSRAN_CEIL(G_ack_rvd - m_rvd_count, Nl * Qm);
        }
        M_uci_rvd = rvd_m_re_count;

        if (m_ack_count < G_ack) {
          ack_d          = 1;
          ack_m_re_count = M_uci_rvd;
          if (G_ack - m_ack_count < M_uci_rvd * Nl * Qm) {
            ack_d          = (M_uci_rvd * Nl * Qm) / (G_ack - m_ack_count);
            ack_m_re_count = SRSRAN_CEIL(G_ack - m_ack_count, Nl * Qm);
          }
        }
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

    for (uint32_t i = 0, csi1_i = 0, csi2_i = 0, rvd_i = 0; i < cfg->pusch.M_pusch_sc[l]; i++) {
      // Check if RE is reserved
      bool reserved = false;
      if (rvd_m_re_count != 0 && i % rvd_d == 0 && m_rvd_count < G_ack_rvd) {
        reserved = true;
      }

      if (G_ack_rvd == 0 && ack_m_re_count != 0 && i % ack_d == 0 && m_ack_count < G_ack) {
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
        if (ack_m_re_count != 0 && rvd_i % ack_d == 0 && m_ack_count < G_ack) {
          for (uint32_t j = 0; j < Nl * Qm; j++) {
            pos_ack[m_ack_count++] = m_all_count + j;
          }
          ack_m_re_count--;
        }
        m_rvd_count += Nl * Qm;
        rvd_m_re_count--;
        rvd_i++;
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
  if (G_ack_rvd != 0 && G_ack_rvd != m_rvd_count && cfg->ack.count <= 2) {
    ERROR("Not matched %d!=%d", G_ack_rvd, m_rvd_count);
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
  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
    if (m_ulsch_count != 0) {
      DEBUG("UL-SCH bit positions:");
      srsran_vec_fprint_i(stdout, (int*)pos_ulsch, m_ulsch_count);
    }
    if (m_ack_count != 0 && cfg->ack.count > 0) {
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
  int E_uci_ack = srsran_uci_nr_encode_pusch_ack(&q->uci, &cfg->uci, uci, q->g_ack);
  if (E_uci_ack < SRSRAN_SUCCESS) {
    ERROR("Error encoding HARQ-ACK bits");
    return SRSRAN_ERROR;
  }
  q->G_ack = (uint32_t)E_uci_ack;

  // Encode CSI part 1
  int E_uci_csi1 = srsran_uci_nr_encode_pusch_csi1(&q->uci, &cfg->uci, uci, q->g_csi1);
  if (E_uci_csi1 < SRSRAN_SUCCESS) {
    ERROR("Error encoding HARQ-ACK bits");
    return SRSRAN_ERROR;
  }
  q->G_csi1 = (uint32_t)E_uci_csi1;

  // Encode CSI part 2
  // ... Not implemented
  q->G_csi2 = 0;

  // Generate PUSCH UCI/UL-SCH multiplexing
  if (pusch_nr_gen_mux_uci(q, &cfg->uci) < SRSRAN_SUCCESS) {
    ERROR("Error generating PUSCH mux tables");
    return SRSRAN_ERROR;
  }

  // Encode SCH
  if (srsran_ulsch_nr_encode(&q->sch, &cfg->sch_cfg, tb, data, q->g_ulsch) < SRSRAN_SUCCESS) {
    ERROR("Error in SCH encoding");
    return SRSRAN_ERROR;
  }

  // Multiplex UL-SCH with UCI only if it is necessary
  uint32_t nof_bits = tb->nof_re * srsran_mod_bits_x_symbol(tb->mod);
  uint8_t* b        = q->g_ulsch;
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

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
    DEBUG("b=");
    srsran_vec_fprint_b(stdout, b, nof_bits);
  }

  // 7.3.1.1 Scrambling
  uint32_t cinit = pusch_nr_cinit(&q->carrier, cfg, rnti, tb->cw_idx);
  srsran_sequence_apply_bit(b, q->b[tb->cw_idx], nof_bits, cinit);

  // Special Scrambling condition
  if (cfg->uci.ack.count <= 2) {
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
  srsran_mod_modulate(&q->modem_tables[tb->mod], q->b[tb->cw_idx], q->d[tb->cw_idx], nof_bits);

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
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
    ERROR("Invalid inputs");
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

  // 6.3.1.1 and 6.3.1.2
  uint32_t nof_cw = 0;
  for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
    nof_cw += grant->tb[tb].enabled ? 1 : 0;

    if (pusch_nr_encode_codeword(q, cfg, &grant->tb[tb], data->payload[tb], &data[0].uci, grant->rnti) <
        SRSRAN_SUCCESS) {
      ERROR("Error encoding TB %d", tb);
      return SRSRAN_ERROR;
    }
  }

  // 6.3.1.3 Layer mapping
  cf_t** x = q->d;
  if (grant->nof_layers > 1) {
    x = q->x;
    srsran_layermap_nr(q->d, nof_cw, x, grant->nof_layers, grant->nof_layers);
  }

  // 6.3.1.4 Transform precoding
  // ... Not implemented

  // 6.3.1.5 Precoding
  // ... Not implemented

  // 6.3.1.6 Mapping to virtual resource blocks
  // ... Not implemented

  // 6.3.1.7 Mapping from virtual to physical resource blocks
  int n = pusch_nr_put(q, cfg, grant, x[0], sf_symbols[0]);
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

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
    DEBUG("d=");
    srsran_vec_fprint_c(stdout, q->d[tb->cw_idx], tb->nof_re);
  }

  // Total number of bits
  uint32_t nof_bits = tb->nof_re * srsran_mod_bits_x_symbol(tb->mod);

  // Calculate HARQ-ACK bits
  int n = srsran_uci_nr_pusch_ack_nof_bits(&cfg->uci.pusch, cfg->uci.ack.count);
  if (n < SRSRAN_SUCCESS) {
    ERROR("Calculating G_ack");
    return SRSRAN_ERROR;
  }
  q->G_ack = (uint32_t)n;

  // Calculate CSI part 1 bits
  n = srsran_uci_nr_pusch_csi1_nof_bits(&cfg->uci);
  if (n < SRSRAN_SUCCESS) {
    ERROR("Calculating G_csi1");
    return SRSRAN_ERROR;
  }
  q->G_csi1 = (uint32_t)n;

  // Calculate CSI part 2 bits
  // ... Not implemented
  q->G_csi2 = 0;

  // Generate PUSCH UCI/UL-SCH multiplexing
  if (pusch_nr_gen_mux_uci(q, &cfg->uci) < SRSRAN_SUCCESS) {
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
    res->evm[tb->cw_idx] = srsran_evm_run_b(q->evm_buffer, &q->modem_tables[tb->mod], q->d[tb->cw_idx], llr, nof_bits);
  }

  // Descrambling
  srsran_sequence_apply_c(llr, llr, nof_bits, pusch_nr_cinit(&q->carrier, cfg, rnti, tb->cw_idx));

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
    DEBUG("b=");
    srsran_vec_fprint_bs(stdout, llr, nof_bits);
  }

  // Demultiplex UCI only if necessary
  if (q->uci_mux) {
    // As it can be HARQ-ACK takes LLRs from ULSCH, demultiplex HARQ-ACK first
    int8_t* g_ack = (int8_t*)q->g_ack;
    for (uint32_t i = 0; i < q->G_ack; i++) {
      g_ack[i]           = llr[q->pos_ack[i]];
      llr[q->pos_ack[i]] = 0;
    }

    // Demultiplex UL-SCH, change sign
    int8_t* g_ulsch = (int8_t*)q->g_ulsch;
    for (uint32_t i = 0; i < q->G_ulsch; i++) {
      g_ulsch[i] = -llr[q->pos_ulsch[i]];
    }
    for (uint32_t i = q->G_ulsch; i < nof_bits; i++) {
      g_ulsch[i] = 0;
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
      if (srsran_uci_nr_decode_pusch_ack(&q->uci, &cfg->uci, g_ack, &res->uci)) {
        ERROR("Error in UCI decoding");
        return SRSRAN_ERROR;
      }
    }

    // Decode CSI part 1
    if (q->G_csi1) {
      if (srsran_uci_nr_decode_pusch_csi1(&q->uci, &cfg->uci, g_csi1, &res->uci)) {
        ERROR("Error in UCI decoding");
        return SRSRAN_ERROR;
      }
    }

    // Decode CSI part 2
    // ... Not implemented

    // Change LLR pointer
    llr = g_ulsch;
  } else {
    for (uint32_t i = 0; i < nof_bits; i++) {
      llr[i] *= -1;
    }
  }

  // Decode Ul-SCH
  if (nof_bits != 0) {
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
                           srsran_pusch_res_nr_t*       data)
{
  // Check input pointers
  if (!q || !cfg || !grant || !data || !sf_symbols || !channel) {
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

  uint32_t nof_cw = 0;
  for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
    nof_cw += grant->tb[tb].enabled ? 1 : 0;
  }

  int e = srsran_ra_dl_nr_slot_nof_re(cfg, grant);
  if (e < SRSRAN_SUCCESS) {
    ERROR("Getting number of RE");
    return SRSRAN_ERROR;
  }
  uint32_t nof_re = (uint32_t)e;

  if (channel->nof_re != nof_re) {
    ERROR("Inconsistent number of RE (%d!=%d)", channel->nof_re, nof_re);
    return SRSRAN_ERROR;
  }

  // Demapping from virtual to physical resource blocks
  uint32_t nof_re_get = pusch_nr_get(q, cfg, grant, q->x[0], sf_symbols[0]);
  if (nof_re_get != nof_re) {
    ERROR("Inconsistent number of RE (%d!=%d)", nof_re_get, nof_re);
    return SRSRAN_ERROR;
  }

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
    DEBUG("ce=");
    srsran_vec_fprint_c(stdout, channel->ce[0][0], nof_re);
    DEBUG("x=");
    srsran_vec_fprint_c(stdout, q->x[0], nof_re);
  }

  // Demapping to virtual resource blocks
  // ... Not implemented

  // Antenna port demapping
  // ... Not implemented
  srsran_predecoding_single(q->x[0], channel->ce[0][0], q->d[0], NULL, nof_re, 1.0f, channel->noise_estimate);

  // Layer demapping
  if (grant->nof_layers > 1) {
    srsran_layerdemap_nr(q->d, nof_cw, q->x, grant->nof_layers, nof_re);
  }

  // SCH decode
  for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
    if (pusch_nr_decode_codeword(q, cfg, &grant->tb[tb], data, grant->rnti) < SRSRAN_SUCCESS) {
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

static uint32_t pusch_nr_grant_info(const srsran_pusch_nr_t*     q,
                                    const srsran_sch_cfg_nr_t*   cfg,
                                    const srsran_sch_grant_nr_t* grant,
                                    const srsran_pusch_res_nr_t* res,
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

  // Append RNTI type and id
  len =
      srsran_print_check(str, str_len, len, "%s-rnti=0x%x ", srsran_rnti_type_str_short(grant->rnti_type), grant->rnti);

  // Append time-domain resource mapping
  len = srsran_print_check(str,
                           str_len,
                           len,
                           "prb=(%d,%d) symb=(%d,%d) ",
                           first_prb,
                           first_prb + grant->nof_prb - 1,
                           grant->S,
                           grant->S + grant->L - 1);

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

  len += pusch_nr_grant_info(q, cfg, grant, res, &str[len], str_len - len);

  if (res != NULL && srsran_uci_nr_total_bits(&cfg->uci) > 0) {
    srsran_uci_data_nr_t uci_data = {};
    uci_data.cfg                  = cfg->uci;
    uci_data.value                = res->uci;
    len += srsran_uci_nr_info(&uci_data, &str[len], str_len - len);

    len = srsran_print_check(str, str_len, len, "valid=%c ", res->uci.valid ? 'y' : 'n');
  }

  if (q->meas_time_en) {
    len = srsran_print_check(str, str_len, len, "t_us=%d ", q->meas_time_us);
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

  len += pusch_nr_grant_info(q, cfg, grant, NULL, &str[len], str_len - len);

  if (uci_value != NULL) {
    srsran_uci_data_nr_t uci_data = {};
    uci_data.cfg                  = cfg->uci;
    uci_data.value                = *uci_value;
    len += srsran_uci_nr_info(&uci_data, &str[len], str_len - len);
  }

  if (q->meas_time_en) {
    len = srsran_print_check(str, str_len, len, " t=%d us", q->meas_time_us);
  }

  return len;
}
