/**
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
#include "srslte/phy/phch/ra_dl_nr.h"
#include "srslte/phy/utils/debug.h"

static void sliv_to_s_and_l(uint32_t sliv, uint32_t* S, uint32_t* L)
{
  // S values can be 0 to 3
  uint32_t low = sliv % 14;
  if (low < 7) {
    *S = low;
    *L = sliv / 14 + 1;
  } else {
    *S = 14 - 1 - low;
    *L = 14 - sliv / 14 + 1;
  }
}

// Validate S and L combination for TypeA time domain resource allocation
static bool check_time_ra_typeA(uint32_t* S, uint32_t* L)
{
  // Check values using Table 5.1.2.1-1
  if (*S > 3) {
    ERROR("S (%d) is out-of-range {0,1,2,3}\n", *S);
    return false;
  }

  if (*L < 3 || *L > 14) {
    ERROR("L (%d) is out-of-range {3,...,14}\n", *L);
    return false;
  }

  uint32_t sum = *S + *L;
  if (sum < 3) {
    ERROR("The sum of S (%d) and L (%d) is lower than 3\n", *S, *L);
    return false;
  }

  if (sum > 14) {
    ERROR("The sum of S (%d) and L (%d) is greater than 14\n", *S, *L);
    return false;
  }

  return true;
}

static bool check_time_ra_typeB(uint32_t* S, uint32_t* L)
{
  ERROR("Not implemented\n");
  return false;
}

bool srslte_ra_dl_nr_time_validate(srslte_sch_grant_nr_t* grant)
{
  if (grant->mapping == srslte_sch_mapping_type_A) {
    return check_time_ra_typeA(&grant->S, &grant->L);
  } else {
    return check_time_ra_typeB(&grant->S, &grant->L);
  }
}

int srslte_ra_dl_nr_time_default_A(uint32_t m, srslte_dmrs_sch_typeA_pos_t dmrs_typeA_pos, srslte_sch_grant_nr_t* grant)
{
  if (grant == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (m >= SRSLTE_MAX_NOF_DL_ALLOCATION) {
    ERROR("m (%d) is out-of-range\n", m);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Select k0
  grant->k0 = 0;

  // Select PDSCH mapping
  static srslte_sch_mapping_type_t pdsch_mapping_lut[16] = {srslte_sch_mapping_type_A,
                                                            srslte_sch_mapping_type_A,
                                                            srslte_sch_mapping_type_A,
                                                            srslte_sch_mapping_type_A,
                                                            srslte_sch_mapping_type_A,
                                                            srslte_sch_mapping_type_B,
                                                            srslte_sch_mapping_type_B,
                                                            srslte_sch_mapping_type_B,
                                                            srslte_sch_mapping_type_B,
                                                            srslte_sch_mapping_type_B,
                                                            srslte_sch_mapping_type_B,
                                                            srslte_sch_mapping_type_A,
                                                            srslte_sch_mapping_type_A,
                                                            srslte_sch_mapping_type_A,
                                                            srslte_sch_mapping_type_B,
                                                            srslte_sch_mapping_type_B};
  grant->mapping                                         = pdsch_mapping_lut[m];

  static uint32_t S_pos2[SRSLTE_MAX_NOF_DL_ALLOCATION] = {2, 2, 2, 2, 2, 9, 4, 5, 5, 9, 12, 1, 1, 2, 4, 8};
  static uint32_t L_pos2[SRSLTE_MAX_NOF_DL_ALLOCATION] = {12, 10, 9, 7, 5, 4, 4, 7, 2, 2, 2, 13, 6, 4, 7, 4};
  static uint32_t S_pos3[SRSLTE_MAX_NOF_DL_ALLOCATION] = {3, 3, 3, 3, 3, 10, 6, 5, 5, 9, 12, 1, 1, 2, 4, 8};
  static uint32_t L_pos3[SRSLTE_MAX_NOF_DL_ALLOCATION] = {11, 9, 8, 6, 4, 4, 4, 7, 2, 2, 2, 13, 6, 4, 7, 4};

  // Select start symbol (S) and length (L)
  switch (dmrs_typeA_pos) {

    case srslte_dmrs_sch_typeA_pos_2:
      grant->S = S_pos2[m];
      grant->L = L_pos2[m];
      break;
    case srslte_dmrs_sch_typeA_pos_3:
      grant->S = S_pos3[m];
      grant->L = L_pos3[m];
      break;
    default:
      ERROR("Undefined case (%d)\n", dmrs_typeA_pos);
      return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

void srslte_ra_dl_nr_time_hl(const srslte_pdsch_time_ra_t* hl_ra_cfg, srslte_sch_grant_nr_t* grant)
{
  // Compute S and L from SLIV from higher layers
  sliv_to_s_and_l(hl_ra_cfg->sliv, &grant->S, &grant->L);
  grant->k0      = hl_ra_cfg->k0;
  grant->mapping = hl_ra_cfg->mapping_type;
}

int srslte_ra_dl_nr_time(const srslte_sch_cfg_nr_t*       cfg,
                         const srslte_rnti_type_t         rnti_type,
                         const srslte_search_space_type_t ss_type,
                         const uint8_t                    m,
                         srslte_sch_grant_nr_t*           grant)
{

  if (cfg == NULL || grant == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (m >= SRSLTE_MAX_NOF_DL_ALLOCATION) {
    ERROR("m (%d) is out-of-range\n", m);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Determine which PDSCH Time domain RA configuration to apply (Table 5.1.2.1.1-1)
  if (cfg->pdsch_time_is_default) {
    // Note: Only Default A is supported, which corresponds SS/PBCH block and coreset mux pattern 1
    srslte_ra_dl_nr_time_default_A(m, cfg->dmrs_typeA.typeA_pos, grant);
  } else {
    srslte_ra_dl_nr_time_hl(&cfg->pdsch_time_ra[m], grant);
  }

  // Validate S and L parameters
  if (!srslte_ra_dl_nr_time_validate(grant)) {
    ERROR("Invalid Time RA\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_ra_dl_nr_nof_dmrs_cdm_groups_without_data_format_1_0(const srslte_sch_cfg_nr_t* pdsch_cfg,
                                                                srslte_sch_grant_nr_t*     grant)
{
  if (pdsch_cfg == NULL || grant == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  const srslte_dmrs_sch_cfg_t* dmrs_cfg =
      grant->mapping == srslte_sch_mapping_type_A ? &pdsch_cfg->dmrs_typeA : &pdsch_cfg->dmrs_typeB;

  /* According to TS 38.214 V15.10.0 5.1.6.1.3 CSI-RS for mobility:
   * When receiving PDSCH scheduled by DCI format 1_0, the UE shall assume the number of DM-RS CDM groups without data
   * is 1 which corresponds to CDM group 0 for the case of PDSCH with allocation duration of 2 symbols, and the UE shall
   * assume that the number of DM-RS CDM groups without data is 2 which corresponds to CDM group {0,1} for all other
   * cases.
   */
  if (dmrs_cfg->length == srslte_dmrs_sch_len_2) {
    grant->nof_dmrs_cdm_groups_without_data = 1;
  } else {
    grant->nof_dmrs_cdm_groups_without_data = 2;
  }

  return SRSLTE_SUCCESS;
}

/* RBG size for type0 scheduling as in table 5.1.2.2.1-1 of 36.214 */
uint32_t srslte_ra_dl_nr_type0_P(uint32_t bwp_size, bool config_is_1)
{
  if (bwp_size <= 36) {
    return config_is_1 ? 2 : 4;
  } else if (bwp_size <= 72) {
    return config_is_1 ? 4 : 8;
  } else if (bwp_size <= 144) {
    return config_is_1 ? 8 : 16;
  } else {
    return 16;
  }
}

static int ra_freq_type0(const srslte_carrier_nr_t* carrier,
                         const srslte_sch_cfg_nr_t* cfg,
                         const srslte_dci_dl_nr_t*  dci_dl,
                         srslte_sch_grant_nr_t*     grant)
{
  uint32_t P = srslte_ra_dl_nr_type0_P(carrier->nof_prb, cfg->rbg_size_cfg_1);

  uint32_t N_rbg      = (int)ceilf((float)(carrier->nof_prb + (carrier->start % P)) / P);
  uint32_t rbg_offset = 0;
  for (uint32_t i = 0; i < N_rbg; i++) {
    uint32_t rbg_size = P;
    if (i == 0) {
      rbg_size -= (carrier->start % P);
    } else if ((i == N_rbg - 1) && ((carrier->nof_prb + carrier->start) % P) > 0) {
      rbg_size = (carrier->nof_prb + carrier->start) % P;
    }
    if (dci_dl->freq_domain_assigment & (1 << (N_rbg - i - 1))) {
      for (uint32_t j = 0; j < rbg_size; j++) {
        if (rbg_offset + j < carrier->nof_prb) {
          grant->prb_idx[rbg_offset + j] = true;
          grant->nof_prb++;
        }
      }
    }
    rbg_offset += rbg_size;
  }
  return 0;
}

int srslte_ra_dl_nr_freq(const srslte_carrier_nr_t* carrier,
                         const srslte_sch_cfg_nr_t* cfg,
                         const srslte_dci_dl_nr_t*  dci_dl,
                         srslte_sch_grant_nr_t*     grant)
{

  if (cfg == NULL || grant == NULL || dci_dl == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // RA scheme
  if (dci_dl->format == srslte_dci_format_nr_1_0) {
    ra_freq_type0(carrier, cfg, dci_dl, grant);
  } else {
    ERROR("Only DCI Format 1_0 is supported\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}
