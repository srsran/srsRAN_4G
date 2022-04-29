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

#include "srsran/phy/phch/harq_ack.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

// Implements TS 38.213 Table 9.1.3-1: Value of counter DAI in DCI format 1_0 and of counter DAI or total DAI DCI format
// 1_1
static uint32_t ue_dl_nr_V_DL_DAI(uint32_t dai)
{
  return dai + 1;
}

static int harq_ack_gen_ack_type2(const srsran_harq_ack_cfg_hl_t* cfg,
                                  const srsran_pdsch_ack_nr_t*    ack_info,
                                  srsran_uci_cfg_nr_t*            uci_cfg)
{
  bool harq_ack_spatial_bundling =
      ack_info->use_pusch ? cfg->harq_ack_spatial_bundling_pusch : cfg->harq_ack_spatial_bundling_pucch;
  uint32_t m = 0; // PDCCH with DCI format 1_0 or DCI format 1_1 monitoring occasion index: lower index corresponds to
  // earlier PDCCH with DCI format 1_0 or DCI format 1_1 monitoring occasion
  uint32_t j       = 0;
  uint32_t V_temp  = 0;
  uint32_t V_temp2 = 0;

  uint32_t N_DL_cells = ack_info->nof_cc; // number of serving cells configured by higher layers for the UE

  // Initialise ACK bits
  SRSRAN_MEM_ZERO(uci_cfg->ack.bits, srsran_harq_ack_bit_t, SRSRAN_HARQ_ACK_MAX_NOF_BITS);

  // The following code follows the exact pseudo-code provided in TS 38.213 9.1.3.1 Type-2 HARQ-ACK codebook ...
  while (m < SRSRAN_UCI_NR_MAX_M) {
    uint32_t c = 0; // serving cell index: lower indexes correspond to lower RRC indexes of corresponding cell
    while (c < N_DL_cells) {
      // Get ACK information of serving cell c for the PDCH monitoring occasion m
      const srsran_harq_ack_m_t* ack = &ack_info->cc[c].m[m];

      // Get DAI counter value
      uint32_t V_DL_CDAI = ue_dl_nr_V_DL_DAI(ack->resource.v_dai_dl);
      uint32_t V_DL_TDAI = ack->resource.dci_format_1_1 ? ue_dl_nr_V_DL_DAI(ack->resource.v_dai_dl) : UINT32_MAX;

      // Get ACK values
      srsran_harq_ack_bit_t ack_tb0 = {};
      ack_tb0.tb0                   = true;
      ack_tb0.cc_idx                = c;
      ack_tb0.m_idx                 = m;
      ack_tb0.pid                   = ack->resource.pid;
      srsran_harq_ack_bit_t ack_tb1 = {};
      ack_tb1.tb1                   = true;
      ack_tb1.cc_idx                = c;
      ack_tb1.m_idx                 = m;
      ack_tb1.pid                   = ack->resource.pid;

      // For a PDCCH monitoring occasion with DCI format 1_0 or DCI format 1_1 in the active DL BWP of a serving cell,
      // when a UE receives a PDSCH with one transport block and the value of maxNrofCodeWordsScheduledByDCI is 2, the
      // HARQ-ACK information is associated with the first transport block and the UE generates a NACK for the second
      // transport block if harq-ACK-SpatialBundlingPUCCH is not provided and generates HARQ-ACK information with
      // value of ACK for the second transport block if harq-ACK-SpatialBundlingPUCCH is provided.
      if (cfg->max_cw_sched_dci_is_2 && !ack->second_tb_present) {
        ack_tb1.tb1 = false;
      }

      // if PDCCH monitoring occasion m is before an active DL BWP change on serving cell c or an active UL
      // BWP change on the PCell and an active DL BWP change is not triggered by a DCI format 1_1 in PDCCH
      // monitoring occasion m
      if (ack->dl_bwp_changed || ack->ul_bwp_changed) {
        c = c + 1;
      } else {
        if (ack->present) {
          // Load ACK resource data into UCI info
          uci_cfg->pucch.resource_id = ack_info->cc[c].m[m].resource.pucch_resource_id;
          uci_cfg->pucch.n_cce_0     = ack_info->cc[c].m[m].resource.n_cce;
          uci_cfg->pucch.rnti        = ack_info->cc[c].m[m].resource.rnti;

          if (V_DL_CDAI <= V_temp) {
            j = j + 1;
          }

          V_temp = V_DL_CDAI;

          if (V_DL_TDAI == UINT32_MAX) {
            V_temp2 = V_DL_CDAI;
          } else {
            V_temp2 = V_DL_TDAI;
          }

          // if harq-ACK-SpatialBundlingPUCCH is not provided and m is a monitoring occasion for PDCCH with DCI format
          // 1_0 or DCI format 1_1 and the UE is configured by maxNrofCodeWordsScheduledByDCI with reception of two
          // transport blocks for at least one configured DL BWP of at least one serving cell,
          if (!harq_ack_spatial_bundling && cfg->max_cw_sched_dci_is_2) {
            uci_cfg->ack.bits[8 * j + 2 * (V_DL_CDAI - 1) + 0] = ack_tb0;
            uci_cfg->ack.bits[8 * j + 2 * (V_DL_CDAI - 1) + 1] = ack_tb1;
          }
          // elseif harq-ACK-SpatialBundlingPUCCH is provided to the UE and m is a monitoring occasion for
          // PDCCH with DCI format 1_1 and the UE is configured by maxNrofCodeWordsScheduledByDCI with
          // reception of two transport blocks in at least one configured DL BWP of a serving cell,
          else if (harq_ack_spatial_bundling && ack->resource.dci_format_1_1 && cfg->max_cw_sched_dci_is_2) {
            ack_tb0.tb1                                = true;
            uci_cfg->ack.bits[4 * j + (V_DL_CDAI - 1)] = ack_tb0;
          }
          // else
          else {
            uci_cfg->ack.bits[4 * j + (V_DL_CDAI - 1)] = ack_tb0;
          }
        }
        c = c + 1;
      }
    }
    m = m + 1;
  }
  if (V_temp2 < V_temp) {
    j = j + 1;
  }

  // if harq-ACK-SpatialBundlingPUCCH is not provided to the UE and the UE is configured by
  // maxNrofCodeWordsScheduledByDCI with reception of two transport blocks for at least one configured DL BWP of a
  // serving cell,
  if (!harq_ack_spatial_bundling && cfg->max_cw_sched_dci_is_2) {
    uci_cfg->ack.count = 2 * (4 * j + V_temp2);
  } else {
    uci_cfg->ack.count = 4 * j + V_temp2;
  }

  // Implement here SPS PDSCH reception
  // ...

  return SRSRAN_SUCCESS;
}

static int harq_ack_k1(const srsran_harq_ack_cfg_hl_t* cfg, const srsran_dci_dl_nr_t* dci_dl)
{
  // For DCI format 1_0, the PDSCH-to-HARQ_feedback timing indicator field values map to {1, 2, 3, 4, 5, 6, 7, 8}
  if (dci_dl->ctx.format == srsran_dci_format_nr_1_0) {
    return (int)dci_dl->harq_feedback + 1;
  }

  // For DCI format 1_1, if present, the PDSCH-to-HARQ_feedback timing indicator field values map to values for a set of
  // number of slots provided by dl-DataToUL-ACK as defined in Table 9.2.3-1.
  if (dci_dl->harq_feedback >= SRSRAN_MAX_NOF_DL_DATA_TO_UL || dci_dl->harq_feedback >= cfg->nof_dl_data_to_ul_ack) {
    ERROR("Out-of-range PDSCH-to-HARQ feedback index (%d, max %d)", dci_dl->harq_feedback, cfg->nof_dl_data_to_ul_ack);
    return SRSRAN_ERROR;
  }

  return (int)cfg->dl_data_to_ul_ack[dci_dl->harq_feedback];
}

int srsran_harq_ack_resource(const srsran_harq_ack_cfg_hl_t* cfg,
                             const srsran_dci_dl_nr_t*       dci_dl,
                             srsran_harq_ack_resource_t*     pdsch_ack_resource)
{
  if (cfg == NULL || dci_dl == NULL || pdsch_ack_resource == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Calculate Data to UL ACK timing k1
  int k1 = harq_ack_k1(cfg, dci_dl);
  if (k1 < SRSRAN_ERROR) {
    ERROR("Error calculating K1");
    return SRSRAN_ERROR;
  }

  // Fill PDSCH resource
  pdsch_ack_resource->dci_format_1_1    = (dci_dl->ctx.format == srsran_dci_format_nr_1_1);
  pdsch_ack_resource->k1                = k1;
  pdsch_ack_resource->v_dai_dl          = dci_dl->dai;
  pdsch_ack_resource->rnti              = dci_dl->ctx.rnti;
  pdsch_ack_resource->pucch_resource_id = dci_dl->pucch_resource;
  pdsch_ack_resource->n_cce             = dci_dl->ctx.location.ncce;
  pdsch_ack_resource->pid               = dci_dl->pid;

  return SRSRAN_SUCCESS;
}

int srsran_harq_ack_gen_uci_cfg(const srsran_harq_ack_cfg_hl_t* cfg,
                                const srsran_pdsch_ack_nr_t*    ack_info,
                                srsran_uci_cfg_nr_t*            uci_cfg)
{
  // Check inputs
  if (cfg == NULL || ack_info == NULL || uci_cfg == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // According TS 38.213 9.1.2 Type-1 HARQ-ACK codebook determination
  if (cfg->harq_ack_codebook == srsran_pdsch_harq_ack_codebook_semi_static) {
    // This clause applies if the UE is configured with pdsch-HARQ-ACK-Codebook = semi-static.
    ERROR("Type-1 HARQ-ACK codebook determination is NOT implemented");
    return SRSRAN_ERROR;
  }

  // According TS 38.213 9.1.3 Type-2 HARQ-ACK codebook determination
  if (cfg->harq_ack_codebook == srsran_pdsch_harq_ack_codebook_dynamic) {
    // This clause applies if the UE is configured with pdsch-HARQ-ACK-Codebook = dynamic.
    return harq_ack_gen_ack_type2(cfg, ack_info, uci_cfg);
  }

  ERROR("No HARQ-ACK codebook determination is NOT implemented");
  return SRSRAN_ERROR;
}

int srsran_harq_ack_pack(const srsran_harq_ack_cfg_hl_t* cfg,
                         const srsran_pdsch_ack_nr_t*    ack_info,
                         srsran_uci_data_nr_t*           uci_data)
{
  // Check inputs
  if (cfg == NULL || ack_info == NULL || uci_data == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Generate configuration
  if (srsran_harq_ack_gen_uci_cfg(cfg, ack_info, &uci_data->cfg) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Actual packing
  for (uint32_t i = 0; i < uci_data->cfg.ack.count; i++) {
    srsran_harq_ack_bit_t* ack_bit = &uci_data->cfg.ack.bits[i];

    // Skip bit if no TB is used
    if (!ack_bit->tb0 && !ack_bit->tb1) {
      uci_data->value.ack[i] = 0;
      continue;
    }

    // Only TB0
    if (ack_bit->tb0 && !ack_bit->tb1) {
      uci_data->value.ack[i] = ack_info->cc[ack_bit->cc_idx].m[ack_bit->m_idx].value[0];
      continue;
    }

    // Only TB1
    if (!ack_bit->tb0 && ack_bit->tb1) {
      uci_data->value.ack[i] = ack_info->cc[ack_bit->cc_idx].m[ack_bit->m_idx].value[1];
      continue;
    }

    // Both TB with bundling
    uci_data->value.ack[i] = ack_info->cc[ack_bit->cc_idx].m[ack_bit->m_idx].value[0];
    uci_data->value.ack[i] &= ack_info->cc[ack_bit->cc_idx].m[ack_bit->m_idx].value[1];
  }

  return SRSRAN_SUCCESS;
}

int srsran_harq_ack_unpack(const srsran_harq_ack_cfg_hl_t* cfg,
                           const srsran_uci_data_nr_t*     uci_data,
                           srsran_pdsch_ack_nr_t*          ack_info)
{
  // Check inputs
  if (cfg == NULL || ack_info == NULL || uci_data == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Actual packing
  for (uint32_t i = 0; i < uci_data->cfg.ack.count; i++) {
    const srsran_harq_ack_bit_t* ack_bit = &uci_data->cfg.ack.bits[i];

    // Extract TB0
    if (ack_bit->tb0) {
      ack_info->cc[ack_bit->cc_idx].m[ack_bit->m_idx].value[0] = uci_data->value.ack[i];
    }

    // Extract TB1
    if (ack_bit->tb1) {
      ack_info->cc[ack_bit->cc_idx].m[ack_bit->m_idx].value[1] = uci_data->value.ack[i];
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_harq_ack_insert_m(srsran_pdsch_ack_nr_t* ack_info, const srsran_harq_ack_m_t* m)
{
  // Check inputs
  if (ack_info == NULL || m == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Protect SCell index and extract information
  if (m->resource.scell_idx >= SRSRAN_MAX_CARRIERS) {
    ERROR("Serving cell index (%d) exceeds maximum", m->resource.scell_idx);
    return SRSRAN_ERROR;
  }
  srsran_harq_ack_cc_t* cc = &ack_info->cc[m->resource.scell_idx];

  if (cc->M >= SRSRAN_UCI_NR_MAX_M) {
    ERROR("Accumulated M HARQ feedback exceeds maximum (%d)", SRSRAN_UCI_NR_MAX_M);
    return SRSRAN_ERROR;
  }

  // Find insertion index
  uint32_t idx = cc->M; // Append at the end by default
  for (uint32_t i = 0; i < cc->M; i++) {
    if (cc->m[i].resource.k1 < m->resource.k1) {
      idx = i;
      break;
    }
  }

  // Increment count
  cc->M += 1;

  // Make space for insertion
  for (uint32_t i = cc->M - 1; i != idx; i--) {
    cc->m[i] = cc->m[i - 1];
  }

  // Actual insertion
  cc->m[idx] = *m;

  return SRSRAN_SUCCESS;
}

uint32_t srsran_harq_ack_info(const srsran_pdsch_ack_nr_t* ack_info, char* str, uint32_t str_len)
{
  uint32_t len = 0;

  if (ack_info == NULL || str == NULL) {
    return 0;
  }

  // Print base info
  len = srsran_print_check(
      str, str_len, len, "use_pusch=%c nof_cc=%d\n", ack_info->use_pusch ? 'y' : 'n', ack_info->nof_cc);

  // Iterate all carriers
  for (uint32_t cc = 0; cc < ack_info->nof_cc; cc++) {
    len = srsran_print_check(str, str_len, len, "  CC %d: M=%d\n", cc, ack_info->cc[cc].M);
    for (uint32_t m = 0; m < ack_info->cc[cc].M; m++) {
      if (ack_info->cc[cc].m[m].present) {
        len = srsran_print_check(str,
                                 str_len,
                                 len,
                                 "    m %d: k1=%d dai=%d ack=%d\n",
                                 m,
                                 ack_info->cc[cc].m[m].resource.k1,
                                 ack_info->cc[cc].m[m].resource.v_dai_dl,
                                 ack_info->cc[cc].m[m].value[0]);
      }
    }
  }

  return len;
}