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
#include "srsran/phy/phch/ra_dl_nr.h"
#include "ra_helper.h"
#include "srsran/phy/utils/debug.h"

// Validate S and L combination for TypeA time domain resource allocation
static bool check_time_ra_typeA(uint32_t* S, uint32_t* L)
{
  // Check values using Table 5.1.2.1-1
  if (*S > 3) {
    ERROR("S (%d) is out-of-range {0,1,2,3}", *S);
    return false;
  }

  if (*L < 3 || *L > 14) {
    ERROR("L (%d) is out-of-range {3,...,14}", *L);
    return false;
  }

  uint32_t sum = *S + *L;
  if (sum < 3) {
    ERROR("The sum of S (%d) and L (%d) is lower than 3", *S, *L);
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

bool srsran_ra_dl_nr_time_validate(srsran_sch_grant_nr_t* grant)
{
  if (grant->mapping == srsran_sch_mapping_type_A) {
    return check_time_ra_typeA(&grant->S, &grant->L);
  } else {
    return check_time_ra_typeB(&grant->S, &grant->L);
  }
}

int srsran_ra_dl_nr_time_default_A(uint32_t m, srsran_dmrs_sch_typeA_pos_t dmrs_typeA_pos, srsran_sch_grant_nr_t* grant)
{
  if (grant == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (m >= SRSRAN_MAX_NOF_TIME_RA) {
    ERROR("m (%d) is out-of-range", m);
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Select k0
  grant->k = 0;

  // Select PDSCH mapping
  static srsran_sch_mapping_type_t pdsch_mapping_lut[16] = {srsran_sch_mapping_type_A,
                                                            srsran_sch_mapping_type_A,
                                                            srsran_sch_mapping_type_A,
                                                            srsran_sch_mapping_type_A,
                                                            srsran_sch_mapping_type_A,
                                                            srsran_sch_mapping_type_B,
                                                            srsran_sch_mapping_type_B,
                                                            srsran_sch_mapping_type_B,
                                                            srsran_sch_mapping_type_B,
                                                            srsran_sch_mapping_type_B,
                                                            srsran_sch_mapping_type_B,
                                                            srsran_sch_mapping_type_A,
                                                            srsran_sch_mapping_type_A,
                                                            srsran_sch_mapping_type_A,
                                                            srsran_sch_mapping_type_B,
                                                            srsran_sch_mapping_type_B};
  grant->mapping                                         = pdsch_mapping_lut[m];

  static uint32_t S_pos2[SRSRAN_MAX_NOF_TIME_RA] = {2, 2, 2, 2, 2, 9, 4, 5, 5, 9, 12, 1, 1, 2, 4, 8};
  static uint32_t L_pos2[SRSRAN_MAX_NOF_TIME_RA] = {12, 10, 9, 7, 5, 4, 4, 7, 2, 2, 2, 13, 6, 4, 7, 4};
  static uint32_t S_pos3[SRSRAN_MAX_NOF_TIME_RA] = {3, 3, 3, 3, 3, 10, 6, 5, 5, 9, 12, 1, 1, 2, 4, 8};
  static uint32_t L_pos3[SRSRAN_MAX_NOF_TIME_RA] = {11, 9, 8, 6, 4, 4, 4, 7, 2, 2, 2, 13, 6, 4, 7, 4};

  // Select start symbol (S) and length (L)
  switch (dmrs_typeA_pos) {
    case srsran_dmrs_sch_typeA_pos_2:
      grant->S = S_pos2[m];
      grant->L = L_pos2[m];
      break;
    case srsran_dmrs_sch_typeA_pos_3:
      grant->S = S_pos3[m];
      grant->L = L_pos3[m];
      break;
    default:
      ERROR("Undefined case (%d)", dmrs_typeA_pos);
      return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

static void ra_dl_nr_time_hl(const srsran_sch_time_ra_t* hl_ra_cfg, srsran_sch_grant_nr_t* grant)
{
  // Compute S and L from SLIV from higher layers
  srsran_sliv_to_s_and_l(SRSRAN_NSYMB_PER_SLOT_NR, hl_ra_cfg->sliv, &grant->S, &grant->L);

  grant->k       = hl_ra_cfg->k;
  grant->mapping = hl_ra_cfg->mapping_type;
}

int srsran_ra_dl_nr_time(const srsran_sch_hl_cfg_nr_t*    cfg,
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

  // Determine which PDSCH Time domain RA configuration to apply (TS 38.214 Table 5.1.2.1.1-1)
  if (rnti_type == srsran_rnti_type_si && ss_type == srsran_search_space_type_common_0) {
    // Row 1
    // Note: Only Default A is supported, which corresponds SS/PBCH block and coreset mux pattern 1
    srsran_ra_dl_nr_time_default_A(m, cfg->typeA_pos, grant);
  } else if (rnti_type == srsran_rnti_type_si && ss_type == srsran_search_space_type_common_0A) {
    // Row 2
    ERROR("Row not implemented");
  } else if ((rnti_type == srsran_rnti_type_ra || rnti_type == srsran_rnti_type_tc) &&
             ss_type == srsran_search_space_type_common_1) {
    // Row 3
    if (cfg->nof_common_time_ra > 0) {
      ra_dl_nr_time_hl(&cfg->common_time_ra[m], grant);
    } else {
      // Note: Only Default A is supported, which corresponds SS/PBCH block and coreset mux pattern 1
      srsran_ra_dl_nr_time_default_A(m, cfg->typeA_pos, grant);
    }
  } else if (rnti_type == srsran_rnti_type_p && ss_type == srsran_search_space_type_common_2) {
    // Row 4
    ERROR("Row not implemented");
  } else if ((rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_mcs_c ||
              rnti_type == srsran_rnti_type_cs) &&
             SRSRAN_SEARCH_SPACE_IS_COMMON(ss_type) && coreset_id == 0) {
    // Row 5
    if (cfg->nof_common_time_ra > 0) {
      ra_dl_nr_time_hl(&cfg->common_time_ra[m], grant);
    } else {
      srsran_ra_dl_nr_time_default_A(m, cfg->typeA_pos, grant);
    }
  } else if ((rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_mcs_c ||
              rnti_type == srsran_rnti_type_cs) &&
             ((SRSRAN_SEARCH_SPACE_IS_COMMON(ss_type) && coreset_id != 0) || ss_type == srsran_search_space_type_ue)) {
    // Row 6
    if (cfg->nof_dedicated_time_ra > 0) {
      ra_dl_nr_time_hl(&cfg->dedicated_time_ra[m], grant);
    } else if (cfg->nof_common_time_ra > 0) {
      ra_dl_nr_time_hl(&cfg->common_time_ra[m], grant);
    } else {
      srsran_ra_dl_nr_time_default_A(m, cfg->typeA_pos, grant);
    }
  } else {
    ERROR("Unhandled case %s, ss_type=%s", srsran_rnti_type_str(rnti_type), srsran_ss_type_str(ss_type));
  }

  // Validate S and L parameters
  if (!srsran_ra_dl_nr_time_validate(grant)) {
    ERROR("Invalid Time RA S=%d; L=%d; m=%d", grant->S, grant->L, m);
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_ra_dl_nr_nof_front_load_symbols(const srsran_sch_hl_cfg_nr_t* cfg,
                                           const srsran_dci_dl_nr_t*     dci,
                                           srsran_dmrs_sch_len_t*        dmrs_duration)
{
  // Table 7.3.1.2.2-1: Antenna port(s) (1000 + DMRS port), dmrs-Type=1, maxLength=1
  // Table 7.3.1.2.2-3: Antenna port(s) (1000 + DMRS port), dmrs-Type=2, maxLength=1
  if (cfg->dmrs_max_length == srsran_dmrs_sch_len_1) {
    *dmrs_duration = srsran_dmrs_sch_len_1;
    return SRSRAN_SUCCESS;
  }

  // Table 7.3.1.2.2-2: Antenna port(s) (1000 + DMRS port), dmrs-Type=1, maxLength=2
  if (cfg->dmrs_type == srsran_dmrs_sch_type_1 && cfg->dmrs_max_length == srsran_dmrs_sch_len_2) {
    // Only one codeword supported!
    if (dci->ports < 12) {
      *dmrs_duration = srsran_dmrs_sch_len_1;
      return SRSRAN_SUCCESS;
    }

    if (dci->ports < 31) {
      *dmrs_duration = srsran_dmrs_sch_len_2;
      return SRSRAN_SUCCESS;
    }

    ERROR("reserved value for ports (%d)", dci->ports);
    return SRSRAN_ERROR;
  }

  // Table 7.3.1.2.2-4: Antenna port(s) (1000 + DMRS port), dmrs-Type=2, maxLength=2
  if (cfg->dmrs_type == srsran_dmrs_sch_type_2 && cfg->dmrs_max_length == srsran_dmrs_sch_len_2) {
    // Only one codeword supported!
    if (dci->ports < 3) {
      *dmrs_duration = srsran_dmrs_sch_len_1;
      return SRSRAN_SUCCESS;
    }

    if (dci->ports < 24) {
      *dmrs_duration = srsran_dmrs_sch_len_2;
      return SRSRAN_SUCCESS;
    }

    ERROR("reserved value for ports (%d)", dci->ports);
    return SRSRAN_ERROR;
  }

  ERROR("Unhandled case");
  return SRSRAN_ERROR;
}

int srsran_ra_dl_nr_nof_dmrs_cdm_groups_without_data(const srsran_sch_hl_cfg_nr_t* cfg,
                                                     const srsran_dci_dl_nr_t*     dci,
                                                     uint32_t                      L)
{
  // According to TS 38.214 5.1.6.2 DM-RS reception procedure
  switch (dci->ctx.format) {
    case srsran_dci_format_nr_1_0:
      // When receiving PDSCH scheduled by DCI format 1_0, the UE shall assume the number of DM-RS CDM groups
      // without data is 1 which corresponds to CDM group 0 for the case of PDSCH with allocation duration of 2 symbols,
      // and the UE shall assume that the number of DM-RS CDM groups without data is 2 which corresponds to CDM group
      // {0,1} for all other cases.
      if (L == 2) {
        return 1;
      } else {
        return 2;
      }
      return SRSRAN_SUCCESS;
    case srsran_dci_format_nr_1_1:
      // When receiving PDSCH scheduled by DCI format 1_1, the UE shall assume that the CDM groups indicated in the
      // configured index from Tables 7.3.1.2.2-1, 7.3.1.2.2-2, 7.3.1.2.2-3, 7.3.1.2.2-4 of [5, TS. 38.212] contain
      // potential co- scheduled downlink DM-RS and are not used for data transmission, where "1", "2" and "3" for the
      // number of DM-RS CDM group(s) in Tables 7.3.1.2.2-1, 7.3.1.2.2-2, 7.3.1.2.2-3, 7.3.1.2.2-4 of [5, TS. 38.212]
      // correspond to CDM group 0, {0,1}, {0,1,2}, respectively.

      // Table 7.3.1.2.2-1: Antenna port(s) (1000 + DMRS port), dmrs-Type=1, maxLength=1
      if (cfg->dmrs_type == srsran_dmrs_sch_type_1 && cfg->dmrs_max_length == srsran_dmrs_sch_len_1) {
        if (dci->ports < 3) {
          return 1;
        }
        if (dci->ports < 12) {
          return 2;
        }
        ERROR("Invalid ports=%d;", dci->ports);
        return SRSRAN_ERROR;
      }

      ERROR("Unhandled case (%d, %d)", cfg->dmrs_type, cfg->dmrs_max_length);
      return SRSRAN_ERROR;
    default:
      ERROR("Invalid DL DCI format %s", srsran_dci_format_nr_string(dci->ctx.format));
  }

  return SRSRAN_ERROR;
}

int srsran_ra_dl_nr_freq(const srsran_carrier_nr_t*    carrier,
                         const srsran_sch_hl_cfg_nr_t* cfg,
                         const srsran_dci_dl_nr_t*     dci_dl,
                         srsran_sch_grant_nr_t*        grant)
{
  if (carrier == NULL || cfg == NULL || grant == NULL || dci_dl == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // For a PDSCH scheduled with a DCI format 1_0 in any type of PDCCH common search space, regardless of which
  // bandwidth part is the active bandwidth part, RB numbering starts from the lowest RB of the CORESET in which the
  // DCI was received; otherwise RB numbering starts from the lowest RB in the determined downlink bandwidth part.
  uint32_t start_rb = 0;
  if (dci_dl->ctx.format == srsran_dci_format_nr_1_0 && SRSRAN_SEARCH_SPACE_IS_COMMON(dci_dl->ctx.ss_type)) {
    start_rb = dci_dl->ctx.coreset_start_rb;
  }

  // when DCI format 1_0 is decoded in any common search space in which case the size of CORESET 0 shall be used if
  // CORESET 0 is configured for the cell and the size of initial DL bandwidth part shall be used if CORESET 0 is not
  // configured for the cell.
  uint32_t type1_bwp_sz = carrier->nof_prb;
  if (SRSRAN_SEARCH_SPACE_IS_COMMON(dci_dl->ctx.ss_type) && dci_dl->coreset0_bw != 0) {
    type1_bwp_sz = dci_dl->coreset0_bw;
  }

  // The UE shall assume that when the scheduling grant is received with DCI format 1_0 , then downlink resource
  // allocation type 1 is used.
  if (dci_dl->ctx.format == srsran_dci_format_nr_1_0) {
    return ra_helper_freq_type1(type1_bwp_sz, start_rb, dci_dl->freq_domain_assigment, grant);
  }

  // If the scheduling DCI is configured to indicate the downlink resource allocation type as part of the Frequency
  // domain resource assignment field by setting a higher layer parameter resourceAllocation in pdsch-Config to
  // 'dynamicswitch', the UE shall use downlink resource allocation type 0 or type 1 as defined by this DCI field.
  if (cfg->alloc == srsran_resource_alloc_dynamic) {
    ERROR("Unsupported dynamic resource allocation");
    return SRSRAN_ERROR;
  }

  // Otherwise the UE shall use the downlink frequency resource allocation type as defined by the higher layer parameter
  // resourceAllocation.
  if (cfg->alloc == srsran_resource_alloc_type1) {
    return ra_helper_freq_type1(type1_bwp_sz, start_rb, dci_dl->freq_domain_assigment, grant);
  }

  if (cfg->alloc == srsran_resource_alloc_type0) {
    return ra_helper_freq_type0(carrier, cfg, dci_dl->freq_domain_assigment, grant);
  }

  ERROR("Unhandled case");
  return SRSRAN_ERROR;
}

uint32_t srsran_ra_nr_type1_riv(uint32_t N_prb, uint32_t start_rb, uint32_t length_rb)
{
  return srsran_sliv_from_s_and_l(N_prb, start_rb, length_rb);
}
