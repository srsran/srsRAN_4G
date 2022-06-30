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

#include "srsran/phy/phch/ra_ul_nr.h"
#include "ra_helper.h"
#include "srsran/phy/ch_estimation/dmrs_pucch.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/csi.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

typedef struct {
  srsran_sch_mapping_type_t mapping;
  uint32_t                  K2;
  uint32_t                  S;
  uint32_t                  L;
} ue_ra_time_resource_t;

static const ue_ra_time_resource_t ue_ul_default_A_lut[SRSRAN_MAX_NOF_TIME_RA] = {
    {srsran_sch_mapping_type_A, 0, 0, 14},
    {srsran_sch_mapping_type_A, 0, 0, 12},
    {srsran_sch_mapping_type_A, 0, 0, 10},
    {srsran_sch_mapping_type_B, 0, 2, 10},
    {srsran_sch_mapping_type_B, 0, 4, 10},
    {srsran_sch_mapping_type_B, 0, 4, 8},
    {srsran_sch_mapping_type_B, 0, 4, 6},
    {srsran_sch_mapping_type_A, 1, 0, 14},
    {srsran_sch_mapping_type_A, 1, 0, 12},
    {srsran_sch_mapping_type_A, 1, 0, 10},
    {srsran_sch_mapping_type_A, 2, 0, 14},
    {srsran_sch_mapping_type_A, 2, 0, 12},
    {srsran_sch_mapping_type_A, 2, 0, 10},
    {srsran_sch_mapping_type_B, 0, 8, 6},
    {srsran_sch_mapping_type_A, 3, 0, 14},
    {srsran_sch_mapping_type_A, 3, 0, 10}};

int srsran_ra_ul_nr_pusch_time_resource_default_A(uint32_t scs_cfg, uint32_t m, srsran_sch_grant_nr_t* grant)
{
  uint32_t j[4] = {1, 1, 2, 3};

  if (grant == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (scs_cfg >= 4) {
    ERROR("Invalid numerology (%d)", scs_cfg);
    return SRSRAN_ERROR;
  }

  if (m >= SRSRAN_MAX_NOF_TIME_RA) {
    ERROR("m (%d) is out-of-range", m);
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Select mapping
  grant->mapping = ue_ul_default_A_lut[m].mapping;
  grant->k       = ue_ul_default_A_lut[m].K2 + j[scs_cfg];
  grant->S       = ue_ul_default_A_lut[m].S;
  grant->L       = ue_ul_default_A_lut[m].L;

  return SRSRAN_SUCCESS;
}

static void ra_ul_nr_time_hl(const srsran_sch_time_ra_t* hl_ra_cfg, srsran_sch_grant_nr_t* grant)
{
  // Compute S and L from SLIV from higher layers
  srsran_sliv_to_s_and_l(SRSRAN_NSYMB_PER_SLOT_NR, hl_ra_cfg->sliv, &grant->S, &grant->L);

  grant->k       = hl_ra_cfg->k;
  grant->mapping = hl_ra_cfg->mapping_type;
}

// Validate S and L combination for TypeA time domain resource allocation
static bool check_time_ra_typeA(uint32_t* S, uint32_t* L)
{
  // Check values using Table 5.1.2.1-1
  if (*S != 0) {
    ERROR("S (%d) is out-of-range {0}", *S);
    return false;
  }

  if (*L < 4 || *L > 14) {
    ERROR("L (%d) is out-of-range {4,...,14}", *L);
    return false;
  }

  uint32_t sum = *S + *L;
  if (sum < 4) {
    ERROR("The sum of S (%d) and L (%d) is lower than 4", *S, *L);
    return false;
  }

  if (sum > 14) {
    ERROR("The sum of S (%d) and L (%d) is greater than 14", *S, *L);
    return false;
  }

  return true;
}

static bool check_time_ra_typeB(uint32_t* S, uint32_t* L)
{
  ERROR("Not implemented");
  return false;
}

bool srsran_ra_ul_nr_time_validate(srsran_sch_grant_nr_t* grant)
{
  if (grant->mapping == srsran_sch_mapping_type_A) {
    return check_time_ra_typeA(&grant->S, &grant->L);
  }

  return check_time_ra_typeB(&grant->S, &grant->L);
}

int srsran_ra_ul_nr_time(const srsran_sch_hl_cfg_nr_t*    cfg,
                         const srsran_rnti_type_t         rnti_type,
                         const srsran_search_space_type_t ss_type,
                         const uint32_t                   coreset_id,
                         const uint8_t                    m,
                         srsran_sch_grant_nr_t*           grant)
{
  if (cfg == NULL || grant == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (m >= SRSRAN_MAX_NOF_TIME_RA) {
    ERROR("m (%d) is out-of-range", m);
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Determine which PUSCH Time domain RA configuration to apply (TS 38.214 Table 6.1.2.1.1-1:)
  if (ss_type == srsran_search_space_type_rar) {
    // Row 1
    if (cfg->nof_common_time_ra == 0) {
      srsran_ra_ul_nr_pusch_time_resource_default_A(cfg->scs_cfg, m, grant);
    } else if (m < cfg->nof_common_time_ra) {
      ra_ul_nr_time_hl(&cfg->common_time_ra[m], grant);
    } else {
      ERROR("Time domain resource selection (m=%d) exceeds the maximum value (%d)",
            m,
            SRSRAN_MIN(cfg->nof_common_time_ra, SRSRAN_MAX_NOF_TIME_RA));
    }
  } else if ((rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_mcs_c ||
              rnti_type == srsran_rnti_type_tc || rnti_type == srsran_rnti_type_cs) &&
             SRSRAN_SEARCH_SPACE_IS_COMMON(ss_type) && coreset_id == 0) {
    // Row 2
    if (cfg->nof_common_time_ra == 0) {
      srsran_ra_ul_nr_pusch_time_resource_default_A(cfg->scs_cfg, m, grant);
    } else {
      ra_ul_nr_time_hl(&cfg->common_time_ra[m], grant);
    }
  } else if ((rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_mcs_c ||
              rnti_type == srsran_rnti_type_tc || rnti_type == srsran_rnti_type_cs ||
              rnti_type == srsran_rnti_type_sp_csi) &&
             ((SRSRAN_SEARCH_SPACE_IS_COMMON(ss_type) && coreset_id != 0) || ss_type == srsran_search_space_type_ue)) {
    // Row 3
    if (cfg->nof_dedicated_time_ra > 0) {
      ra_ul_nr_time_hl(&cfg->dedicated_time_ra[m], grant);
    } else if (cfg->nof_common_time_ra > 0) {
      ra_ul_nr_time_hl(&cfg->common_time_ra[m], grant);
    } else {
      srsran_ra_ul_nr_pusch_time_resource_default_A(cfg->scs_cfg, m, grant);
    }
  } else {
    ERROR("Unhandled case");
  }

  // Table 6.1.2.1.1-5 defines the additional subcarrier spacing specific slot delay value for the first transmission of
  // PUSCH scheduled by the RAR. When the UE transmits a PUSCH scheduled by RAR, the Δ value specific to the PUSCH
  // subcarrier spacing μ PUSCH is applied in addition to the K 2 value.
  if (ss_type == srsran_search_space_type_rar) {
    uint32_t delta[4] = {2, 3, 4, 6};
    if (cfg->scs_cfg >= 4) {
      ERROR("Invalid numerology");
      return SRSRAN_ERROR;
    }
    grant->k += delta[cfg->scs_cfg];
  }

  // Validate S and L parameters
  if (!srsran_ra_ul_nr_time_validate(grant)) {
    ERROR("Invalid Time RA S=%d; L=%d; m=%d", grant->S, grant->L, m);
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_ra_ul_nr_nof_front_load_symbols(const srsran_sch_hl_cfg_nr_t* cfg,
                                           const srsran_dci_ul_nr_t*     dci,
                                           srsran_dmrs_sch_len_t*        dmrs_duration)
{
  if (cfg == NULL || dci == NULL || dmrs_duration == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Table 7.3.1.1.2-6: Antenna port(s), transform precoder is enabled, dmrs-Type=1, maxLength=1
  // Table 7.3.1.1.2-8: Antenna port(s), transform precoder is disabled, dmrs-Type=1, maxLength=1, rank=1
  // Table 7.3.1.1.2-9: Antenna port(s), transform precoder is disabled, dmrs-Type=1, maxLength=1, rank=2
  // Table 7.3.1.1.2-10: Antenna port(s), transform precoder is disabled, dmrs-Type=1, maxLength=1, rank=3
  // Table 7.3.1.1.2-11: Antenna port(s), transform precoder is disabled, dmrs-Type=1, maxLength=1, rank=4
  // Table 7.3.1.1.2-16: Antenna port(s), transform precoder is disabled, dmrs-Type=2, maxLength=1, rank=1
  // Table 7.3.1.1.2-17: Antenna port(s), transform precoder is disabled, dmrs-Type=2, maxLength=1, rank=2
  // Table 7.3.1.1.2-18: Antenna port(s), transform precoder is disabled, dmrs-Type=2, maxLength=1, rank=3
  // Table 7.3.1.1.2-19: Antenna port(s), transform precoder is disabled, dmrs-Type=2, maxLength=1, rank=4
  if (cfg->dmrs_max_length == srsran_dmrs_sch_len_1) {
    *dmrs_duration = srsran_dmrs_sch_len_1;
    return SRSRAN_SUCCESS;
  }

  // Other tables are not implemented
  ERROR("Unhandled case");
  return SRSRAN_ERROR;
}

int srsran_ra_ul_nr_nof_dmrs_cdm_groups_without_data(const srsran_sch_hl_cfg_nr_t* cfg,
                                                     const srsran_dci_ul_nr_t*     dci,
                                                     uint32_t                      L)
{
  if (cfg == NULL || dci == NULL || L == 0) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
  uint32_t rank = 1; // No other supported

  // According to TS 38.214 V15.10.0 6.2.2 UE DM-RS transmission procedure:
  switch (dci->ctx.format) {
    case srsran_dci_format_nr_0_0:
    case srsran_dci_format_nr_rar:
      // For PUSCH scheduled by DCI format 0_0 or by activation DCI format 0_0 with CRC scrambled by CS-RNTI, the UE
      // shall assume the number of DM-RS CDM groups without data is 1 which corresponds to CDM group 0 for the case of
      // PUSCH with allocation duration of 2 or less OFDM symbols with transform precoding disabled, the UE shall assume
      // that the number of DM-RS CDM groups without data is 3 which corresponds to CDM group {0,1,2} for the case of
      // PUSCH scheduled by activation DCI format 0_0 and the dmrs-Type in cg-DMRS-Configuration equal to 'type2' and
      // the PUSCH allocation duration being more than 2 OFDM symbols, and the UE shall assume that the number of DM-RS
      // CDM groups without data is 2 which corresponds to CDM group {0,1} for all other cases.
      if (L <= 2 && !cfg->enable_transform_precoder) {
        return 1;
      } else if (L > 2 && cfg->dmrs_type == srsran_dmrs_sch_type_2 && dci->ctx.format == srsran_dci_format_nr_cg) {
        return 3;
      } else {
        return 2;
      }
      return SRSRAN_SUCCESS;
    case srsran_dci_format_nr_0_1:
      // For PUSCH scheduled by DCI format 0_1, by activation DCI format 0_1 with CRC scrambled by CS-RNTI, or
      // configured by configured grant Type 1 configuration, the UE shall assume the DM-RS CDM groups indicated in
      // Tables 7.3.1.1.2-6 to 7.3.1.1.2-23 of Clause 7.3.1.1 of [5, TS38.212] are not used for data transmission, where
      // "1", "2" and "3" for the number of DM-RS CDM group(s) correspond to CDM group 0, {0,1}, {0,1,2}, respectively.

      // Table 7.3.1.1.2-6: Antenna port(s), transform precoder is enabled, dmrs-Type=1, maxLength=1
      if (cfg->enable_transform_precoder && cfg->dmrs_type == srsran_dmrs_sch_type_1 &&
          cfg->dmrs_max_length == srsran_dmrs_sch_len_1 && rank == 1) {
        return 2;
      }

      // Table 7.3.1.1.2-8: Antenna port(s), transform precoder is disabled, dmrs-Type=1, maxLength=1, rank= 1
      if (!cfg->enable_transform_precoder && cfg->dmrs_type == srsran_dmrs_sch_type_1 &&
          cfg->dmrs_max_length == srsran_dmrs_sch_len_1 && rank == 1) {
        if (dci->ports < 2) {
          return 1;
        }

        if (dci->ports < 6) {
          return 2;
        }

        ERROR("Invalid ports (%d)", dci->ports);
        return SRSRAN_ERROR;
      }

      ERROR("Unhandled configuration");
      return SRSRAN_ERROR;
    default:
      ERROR("Invalid UL DCI format %s", srsran_dci_format_nr_string(dci->ctx.format));
  }

  return SRSRAN_ERROR;
}

#define RA_UL_PUCCH_CODE_RATE_N 8
#define RA_UL_PUCCH_CODE_RATE_RESERVED NAN

static const double ra_ul_pucch_code_rate_table[RA_UL_PUCCH_CODE_RATE_N] =
    {0.08, 0.15, 0.25, 0.35, 0.45, 0.60, 0.80, RA_UL_PUCCH_CODE_RATE_RESERVED};

// Implements TS 38.213 Table 9.2.5.2-1: Code rate r corresponding to value of maxCodeRate
static double ra_ul_nr_pucch_code_rate_r(const srsran_pucch_nr_resource_t* resource)
{
  if (resource->max_code_rate >= RA_UL_PUCCH_CODE_RATE_RESERVED) {
    ERROR("Invalid code rate");
    return RA_UL_PUCCH_CODE_RATE_RESERVED;
  }

  return ra_ul_pucch_code_rate_table[resource->max_code_rate];
}

// Calculate number of PRBs for PUCCH format 2, or PUCCH format 3, or PUCCH format 4, respectively
// static int ra_ul_nr_pucch_Mrb(const srsran_pucch_nr_resource_t* resource)
//{
//  switch (resource->format) {
//    case SRSRAN_PUCCH_NR_FORMAT_2:
//    case SRSRAN_PUCCH_NR_FORMAT_3:
//      return resource->nof_prb;
//    case SRSRAN_PUCCH_NR_FORMAT_4:
//      return SRSRAN_PUCCH_NR_FORMAT4_NPRB;
//    default:
//      ERROR("Invalid case");
//      break;
//  }
//  return SRSRAN_ERROR;
//}

// Calculate number of subcarriers per resource block for payload (No DMRS)
static int ra_ul_nr_pucch_nre(const srsran_pucch_nr_resource_t* resource)
{
  switch (resource->format) {
    case SRSRAN_PUCCH_NR_FORMAT_2:
      return SRSRAN_NRE - 4;
    case SRSRAN_PUCCH_NR_FORMAT_3:
      return SRSRAN_NRE;
    case SRSRAN_PUCCH_NR_FORMAT_4:
      return SRSRAN_NRE / resource->occ_lenth;
    default:
      ERROR("Invalid case");
      break;
  }
  return SRSRAN_ERROR;
}

// Calculate number of PUCCH symbols excluding the ones used exclusively for DMRS for formats 3 and 4
static int ra_ul_nr_pucch_nsymb(const srsran_pucch_nr_resource_t* resource)
{
  switch (resource->format) {
    case SRSRAN_PUCCH_NR_FORMAT_2:
      return resource->nof_symbols;
    case SRSRAN_PUCCH_NR_FORMAT_3:
    case SRSRAN_PUCCH_NR_FORMAT_4: {
      uint32_t idx[SRSRAN_DMRS_PUCCH_FORMAT_3_4_MAX_NSYMB] = {};

      // Get number of DMRS symbols for format 3 or 4
      int nsymb_dmrs = srsran_dmrs_pucch_format_3_4_get_symbol_idx(resource, idx);
      if (nsymb_dmrs < SRSRAN_SUCCESS) {
        return SRSRAN_ERROR;
      }

      return (int)resource->nof_symbols - nsymb_dmrs;
    }
    default:
      ERROR("Invalid case");
      break;
  }
  return SRSRAN_ERROR;
}

// Calculate number of PUCCH symbols excluding the ones used exclusively for DMRS for formats 3 and 4
static int ra_ul_nr_pucch_qm(const srsran_pucch_nr_resource_t* resource)
{
  switch (resource->format) {
    case SRSRAN_PUCCH_NR_FORMAT_2:
      return 2;
    case SRSRAN_PUCCH_NR_FORMAT_3:
    case SRSRAN_PUCCH_NR_FORMAT_4:
      return resource->enable_pi_bpsk ? 1 : 2;
    default:
      ERROR("Invalid case");
      break;
  }
  return SRSRAN_ERROR;
}

int srsran_ra_ul_nr_pucch_format_2_3_min_prb(const srsran_pucch_nr_resource_t* resource,
                                             const srsran_uci_cfg_nr_t*        uci_cfg)
{
  if (resource == NULL || uci_cfg == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Get maximum allowed code rate
  double r = ra_ul_nr_pucch_code_rate_r(resource);
  if (!isnormal(r)) {
    ERROR("Invalid coderate %f", r);
    return SRSRAN_ERROR;
  }

  // Get number of RE/PRB
  int nre = ra_ul_nr_pucch_nre(resource);
  if (nre < SRSRAN_SUCCESS) {
    ERROR("Getting nre");
    return SRSRAN_ERROR;
  }

  // Get number of symbols
  int nsymb = ra_ul_nr_pucch_nsymb(resource);
  if (nsymb < SRSRAN_SUCCESS) {
    ERROR("Getting nsymb");
    return SRSRAN_ERROR;
  }

  // Get modulation order
  int qm = ra_ul_nr_pucch_qm(resource);
  if (qm < SRSRAN_SUCCESS) {
    ERROR("Getting qm");
    return SRSRAN_ERROR;
  }

  // Calculate denominator
  double nof_bits_rb = r * nre * nsymb * qm;
  if (!isnormal(nof_bits_rb)) {
    return SRSRAN_ERROR;
  }

  // Compute total number of UCI bits
  uint32_t O_total = uci_cfg->ack.count + uci_cfg->o_sr + srsran_csi_part1_nof_bits(uci_cfg->csi, uci_cfg->nof_csi);

  // Add CRC bits if any
  O_total += srsran_uci_nr_crc_len(O_total);

  // Return the minimum
  return (int)ceil(O_total / nof_bits_rb);
}

int srsran_ra_ul_nr_freq(const srsran_carrier_nr_t*    carrier,
                         const srsran_sch_hl_cfg_nr_t* cfg,
                         const srsran_dci_ul_nr_t*     dci_ul,
                         srsran_sch_grant_nr_t*        grant)
{
  if (carrier == NULL || cfg == NULL || grant == NULL || dci_ul == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // TS 38.213 PUSCH scheduled by RAR UL grant
  if (dci_ul->ctx.format == srsran_dci_format_nr_rar) {
    return ra_helper_freq_type1(carrier->nof_prb, 0, dci_ul->freq_domain_assigment, grant);
  }

  // The UE shall assume that when the scheduling PDCCH is received with DCI format 0_0, then uplink resource
  // allocation type 1 is used.
  if (dci_ul->ctx.format == srsran_dci_format_nr_0_0) {
    return ra_helper_freq_type1(carrier->nof_prb, 0, dci_ul->freq_domain_assigment, grant);
  }

  // If the scheduling DCI is configured to indicate the uplink resource allocation type as part of the Frequency domain
  // resource assignment field by setting a higher layer parameter resourceAllocation in pusch-Config to 'dynamicSwitch'
  if (cfg->alloc == srsran_resource_alloc_dynamic) {
    ERROR("Unsupported dynamic resource allocation");
    return SRSRAN_ERROR;
  }

  // Otherwise the UE shall use the uplink frequency resource allocation type as defined by the higher layer parameter
  // resourceAllocation.
  if (cfg->alloc == srsran_resource_alloc_type1) {
    return ra_helper_freq_type1(carrier->nof_prb, 0, dci_ul->freq_domain_assigment, grant);
  }

  if (cfg->alloc == srsran_resource_alloc_type0) {
    return ra_helper_freq_type0(carrier, cfg, dci_ul->freq_domain_assigment, grant);
  }

  ERROR("Unhandled case");
  return SRSRAN_ERROR;
}

typedef struct {
  srsran_pucch_nr_format_t format;
  uint32_t                 start_symbol;
  uint32_t                 rb_offset;
  uint32_t                 N_cs;
} pucch_res_common_cfg_t;

// Table 9.2.1-1
#define PUCCH_RES_COMMON_CFG_IDX_MAX 15
static pucch_res_common_cfg_t pucch_res_common_cfg[PUCCH_RES_COMMON_CFG_IDX_MAX + 1] = {
    {SRSRAN_PUCCH_NR_FORMAT_0, 12, 0, 2},
    {SRSRAN_PUCCH_NR_FORMAT_0, 12, 0, 3},
    {SRSRAN_PUCCH_NR_FORMAT_0, 12, 3, 3},
    {SRSRAN_PUCCH_NR_FORMAT_1, 10, 0, 2},
    {SRSRAN_PUCCH_NR_FORMAT_1, 10, 0, 3},
    {SRSRAN_PUCCH_NR_FORMAT_1, 10, 2, 3},
    {SRSRAN_PUCCH_NR_FORMAT_1, 10, 4, 3},
    {SRSRAN_PUCCH_NR_FORMAT_1, 4, 0, 2},
    {SRSRAN_PUCCH_NR_FORMAT_1, 4, 0, 3},
    {SRSRAN_PUCCH_NR_FORMAT_1, 4, 2, 3},
    {SRSRAN_PUCCH_NR_FORMAT_1, 4, 4, 3},
    {SRSRAN_PUCCH_NR_FORMAT_1, 0, 0, 2},
    {SRSRAN_PUCCH_NR_FORMAT_1, 0, 0, 4},
    {SRSRAN_PUCCH_NR_FORMAT_1, 0, 2, 4},
    {SRSRAN_PUCCH_NR_FORMAT_1, 0, 4, 4},
    {SRSRAN_PUCCH_NR_FORMAT_1, 0, 0, 4}};

// Implements TS 38.213 Table 9.2.1-1: PUCCH resource sets before dedicated PUCCH resource configuration
static int ra_ul_nr_pucch_resource_default(uint32_t                    pucch_res_common_idx,
                                           uint32_t                    N_size_bwp,
                                           uint32_t                    r_pucch,
                                           srsran_pucch_nr_resource_t* resource)
{
  if (pucch_res_common_idx > PUCCH_RES_COMMON_CFG_IDX_MAX) {
    ERROR("Invalid pucch_res_common_idx value (%d)\n", pucch_res_common_idx);
    return SRSRAN_ERROR;
  }

  uint32_t cs_v_2[2] = {0, 6};
  uint32_t cs_v_3[3] = {0, 4, 8};
  uint32_t cs_v_4[4] = {0, 3, 6, 9};

  // Table 9.2.1-1
  resource->format           = pucch_res_common_cfg[pucch_res_common_idx].format;
  resource->start_symbol_idx = pucch_res_common_cfg[pucch_res_common_idx].start_symbol;
  resource->nof_symbols      = 14 - resource->start_symbol_idx;
  uint32_t rb_offset_bwp     = pucch_res_common_cfg[pucch_res_common_idx].rb_offset;
  uint32_t N_cs              = pucch_res_common_cfg[pucch_res_common_idx].N_cs;

  // Special case for cs_v_2 value
  if (pucch_res_common_idx == 0) {
    cs_v_2[1] = 3;
  }

  // Special case for rb_offset
  if (pucch_res_common_idx == 15) {
    rb_offset_bwp = N_size_bwp / 4;
  }

  uint32_t csv_idx = 0;
  if (r_pucch / 8 == 0) {
    resource->starting_prb   = rb_offset_bwp + SRSRAN_FLOOR(r_pucch, N_cs);
    resource->second_hop_prb = N_size_bwp - 1 - resource->starting_prb;
    csv_idx                  = r_pucch % N_cs;
  } else {
    resource->second_hop_prb = rb_offset_bwp + SRSRAN_FLOOR(r_pucch - 8, N_cs);
    resource->starting_prb   = N_size_bwp - 1 - resource->second_hop_prb;
    csv_idx                  = (r_pucch - 8) % N_cs;
  }

  switch (N_cs) {
    case 2:
      resource->initial_cyclic_shift = cs_v_2[csv_idx];
      break;
    case 3:
      resource->initial_cyclic_shift = cs_v_3[csv_idx];
      break;
    case 4:
      resource->initial_cyclic_shift = cs_v_4[csv_idx];
      break;
  }

  return SRSRAN_SUCCESS;
}

static int ra_ul_nr_pucch_resource_hl(const srsran_pucch_nr_hl_cfg_t* cfg,
                                      const srsran_uci_cfg_nr_t*      uci_cfg,
                                      uint32_t                        pucch_resource_id,
                                      srsran_pucch_nr_resource_t*     resource)
{
  uint32_t O_uci = srsran_uci_nr_total_bits(uci_cfg);
  uint32_t N2    = cfg->sets[1].max_payload_size > 0 ? cfg->sets[1].max_payload_size : SRSRAN_UCI_NR_MAX_NOF_BITS;
  uint32_t N3    = cfg->sets[2].max_payload_size > 0 ? cfg->sets[2].max_payload_size : SRSRAN_UCI_NR_MAX_NOF_BITS;

  // If the UE transmits O UCI UCI information bits, that include HARQ-ACK information bits, the UE determines a PUCCH
  // resource set to be...
  uint32_t resource_set_id = 3;
  if (uci_cfg->nof_csi == 0 && uci_cfg->ack.count <= 2) {
    // a first set of PUCCH resources with pucch-ResourceSetId = 0 if O_UCI ≤ 2 including 1 or 2 HARQ-ACK
    // information bits and a positive or negative SR on one SR transmission occasion if transmission of HARQ-ACK
    // information and SR occurs simultaneously, or
    resource_set_id = 0;
  } else if (O_uci <= N2 && cfg->sets[1].nof_resources > 0) {
    resource_set_id = 1;
  } else if (O_uci <= N3 && cfg->sets[2].nof_resources > 0) {
    resource_set_id = 2;
  } else if (cfg->sets[3].nof_resources == 0) {
    ERROR("Invalid PUCCH resource configuration, N2=%d, N3=%d, O_uci=%d", N2, N3, O_uci);
    return SRSRAN_ERROR;
  } else if (O_uci > SRSRAN_UCI_NR_MAX_NOF_BITS) {
    ERROR("The number of UCI bits (%d), exceeds the maximum (%d)", O_uci, SRSRAN_UCI_NR_MAX_NOF_BITS);
    return SRSRAN_ERROR;
  }

  // Select resource from the set
  if (pucch_resource_id >= SRSRAN_PUCCH_NR_MAX_NOF_RESOURCES_PER_SET ||
      pucch_resource_id >= cfg->sets[resource_set_id].nof_resources) {
    ERROR("The PUCCH resource identifier (%d) exceeds the number of configured resources (%d) for set identifier %d",
          pucch_resource_id,
          cfg->sets[resource_set_id].nof_resources,
          resource_set_id);
    return SRSRAN_ERROR;
  }
  *resource = cfg->sets[resource_set_id].resources[pucch_resource_id];

  return SRSRAN_SUCCESS;
}

int srsran_ra_ul_nr_pucch_resource(const srsran_pucch_nr_hl_cfg_t* pucch_cfg,
                                   const srsran_uci_cfg_nr_t*      uci_cfg,
                                   uint32_t                        N_bwp_sz,
                                   srsran_pucch_nr_resource_t*     resource)
{
  if (pucch_cfg == NULL || uci_cfg == NULL || resource == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Use SR PUCCH resource
  // - At least one positive SR
  // - No HARQ-ACK
  // - No CSI report
  if (uci_cfg->sr_positive_present > 0 && uci_cfg->ack.count == 0 && uci_cfg->nof_csi == 0) {
    uint32_t sr_resource_id = uci_cfg->pucch.sr_resource_id;
    if (sr_resource_id >= SRSRAN_PUCCH_MAX_NOF_SR_RESOURCES) {
      ERROR("SR resource ID (%d) exceeds the maximum ID (%d)", sr_resource_id, SRSRAN_PUCCH_MAX_NOF_SR_RESOURCES);
      return SRSRAN_ERROR;
    }

    if (!pucch_cfg->sr_resources[sr_resource_id].configured) {
      ERROR("SR resource ID (%d) is not configured", sr_resource_id);
      return SRSRAN_ERROR;
    }

    // Set PUCCH resource
    *resource = pucch_cfg->sr_resources[sr_resource_id].resource;

    // No more logic is required in this case
    return SRSRAN_SUCCESS;
  }

  // Use SR PUCCH resource
  // - At least one positive SR
  // - up to 2 HARQ-ACK
  // - No CSI report
  if (uci_cfg->sr_positive_present && uci_cfg->ack.count <= SRSRAN_PUCCH_NR_FORMAT1_MAX_NOF_BITS &&
      uci_cfg->nof_csi == 0) {
    uint32_t sr_resource_id = uci_cfg->pucch.sr_resource_id;
    if (sr_resource_id >= SRSRAN_PUCCH_MAX_NOF_SR_RESOURCES) {
      ERROR("SR resource ID (%d) exceeds the maximum ID (%d)", sr_resource_id, SRSRAN_PUCCH_MAX_NOF_SR_RESOURCES);
      return SRSRAN_ERROR;
    }

    if (!pucch_cfg->sr_resources[sr_resource_id].configured) {
      ERROR("SR resource ID (%d) is not configured", sr_resource_id);
      return SRSRAN_ERROR;
    }

    // Select PUCCH resource for SR
    srsran_pucch_nr_resource_t resource_sr = pucch_cfg->sr_resources[sr_resource_id].resource;

    // Select PUCCH resource for HARQ-ACK
    srsran_pucch_nr_resource_t resource_harq = {};
    if (ra_ul_nr_pucch_resource_hl(pucch_cfg, uci_cfg, uci_cfg->pucch.resource_id, &resource_harq) < SRSRAN_SUCCESS) {
      ERROR("Error selecting HARQ-ACK resource");
      return SRSRAN_ERROR;
    }

    // If a UE would transmit positive or negative SR in a resource using PUCCH format 0 and HARQ-ACK information bits
    // in a resource using PUCCH format 1 in a slot, the UE transmits only a PUCCH with the HARQ-ACK information bits
    // in the resource using PUCCH format 1.
    if (resource_sr.format == SRSRAN_PUCCH_NR_FORMAT_0 && resource_harq.format == SRSRAN_PUCCH_NR_FORMAT_1) {
      *resource = resource_harq;
      return SRSRAN_SUCCESS;
    }

    // If the UE would transmit positive SR in a first resource using PUCCH format 1 and at most two HARQ-ACK
    // information bits in a second resource using PUCCH format 1 in a slot, the UE transmits a PUCCH with HARQ-ACK
    // information bits in the first resource using PUCCH format 1 as described in Clause 9.2.3. If a UE would transmit
    // negative SR in a resource using PUCCH format 1 and at most two HARQ-ACK information bits in a resource using
    // PUCCH format 1 in a slot, the UE transmits a PUCCH in the resource using PUCCH format 1 for HARQ-ACK
    // information as described in Clause 9.2.3.
    if (resource_sr.format == SRSRAN_PUCCH_NR_FORMAT_1 && resource_harq.format == SRSRAN_PUCCH_NR_FORMAT_1) {
      *resource = resource_sr;
      return SRSRAN_SUCCESS;
    }

    // The impossible happened...
    ERROR("The impossible happened...");
    return SRSRAN_ERROR;
  }

  // Use format 2, 3 or 4 resource from higher layers
  // - Irrelevant SR opportunities
  // - More than 2 HARQ-ACK
  // - No CSI report
  if (uci_cfg->o_sr > 0 && uci_cfg->ack.count > SRSRAN_PUCCH_NR_FORMAT1_MAX_NOF_BITS && uci_cfg->nof_csi == 0) {
    return ra_ul_nr_pucch_resource_hl(pucch_cfg, uci_cfg, uci_cfg->pucch.resource_id, resource);
  }

  // Use format 2, 3 or 4 CSI report resource from higher layers
  // - Irrelevant SR opportunities
  // - No HARQ-ACK
  // - Single periodic CSI report
  if (uci_cfg->ack.count == 0 && uci_cfg->nof_csi == 1 && uci_cfg->csi[0].cfg.type == SRSRAN_CSI_REPORT_TYPE_PERIODIC) {
    *resource = uci_cfg->csi[0].cfg.periodic.resource;
    return SRSRAN_SUCCESS;
  }

  // If a UE does not have dedicated PUCCH resource configuration, provided by PUCCH-ResourceSet in PUCCH-Config,
  // a PUCCH resource set is provided by pucch-ResourceCommon through an index to a row of Table 9.2.1-1 for size
  // transmission of HARQ-ACK information on PUCCH in an initial UL BWP of N BWP PRBs.
  if (!pucch_cfg->enabled) {
    uint32_t N_cce   = SRSRAN_FLOOR(N_bwp_sz, 6);
    uint32_t r_pucch = ((2 * uci_cfg->pucch.n_cce_0) / N_cce) + 2 * uci_cfg->pucch.resource_id;
    return ra_ul_nr_pucch_resource_default(pucch_cfg->common.resource_common, N_bwp_sz, r_pucch, resource);
  }
  return ra_ul_nr_pucch_resource_hl(pucch_cfg, uci_cfg, uci_cfg->pucch.resource_id, resource);
}

uint32_t srsran_ra_ul_nr_nof_sr_bits(uint32_t K)
{
  if (K > 0) {
    return (uint32_t)ceilf(log2f((float)K + 1.0f));
  }
  return 0;
}