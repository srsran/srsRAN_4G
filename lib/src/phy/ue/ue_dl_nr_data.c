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
#include "srslte/phy/ue/ue_dl_nr_data.h"
#include "srslte/phy/utils/debug.h"

static int srslte_ue_dl_nr_pdsch_time_resource_hl_A(uint32_t sliv, uint32_t* S, uint32_t* L)
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

  // Check values using Table 5.1.2.1-1
  if (*S > 3) {
    ERROR("Invalid SLIV %d. S (%d) is out-of-range {0,1,2,3}\n", sliv, *S);
    return SRSLTE_ERROR;
  }

  if (*L < 3 || *L > 14) {
    ERROR("Invalid SLIV %d. L (%d) is out-of-range {3,...,14}\n", sliv, *L);
    return SRSLTE_ERROR;
  }

  uint32_t sum = *S + *L;
  if (sum < 3) {
    ERROR("Invalid SLIV %d. The sum of S (%d) and L (%d) is lower than 3\n", sliv, *S, *L);
    return SRSLTE_ERROR;
  }

  if (sum > 14) {
    ERROR("Invalid SLIV %d. The sum of S (%d) and L (%d) is greater than 14\n", sliv, *S, *L);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

static int srslte_ue_dl_nr_pdsch_time_resource_hl_B(uint32_t sliv, uint32_t* S, uint32_t* L)
{
  ERROR("Not implemented\n");
  return SRSLTE_ERROR;
}

int srslte_ue_dl_nr_pdsch_time_resource_hl(const srslte_pdsch_allocation_t* pdsch_alloc, srslte_pdsch_grant_nr_t* grant)
{

  if (pdsch_alloc == NULL || grant == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  grant->k0      = pdsch_alloc->k0;
  grant->mapping = pdsch_alloc->mapping_type;

  if (pdsch_alloc->mapping_type == srslte_pdsch_mapping_type_A) {
    return srslte_ue_dl_nr_pdsch_time_resource_hl_A(pdsch_alloc->sliv, &grant->S, &grant->L);
  }

  return srslte_ue_dl_nr_pdsch_time_resource_hl_B(pdsch_alloc->sliv, &grant->S, &grant->L);
}

int srslte_ue_dl_nr_pdsch_time_resource_default_A(uint32_t                      m,
                                                  srslte_dmrs_pdsch_typeA_pos_t dmrs_typeA_pos,
                                                  srslte_pdsch_grant_nr_t*      grant)
{
  if (grant == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (m >= 16) {
    ERROR("m (%d) is out-of-range\n", m);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Select k0
  grant->k0 = 0;

  // Select PDSCH mapping
  static srslte_pdsch_mapping_type_t pdsch_mapping_lut[16] = {srslte_pdsch_mapping_type_A,
                                                              srslte_pdsch_mapping_type_A,
                                                              srslte_pdsch_mapping_type_A,
                                                              srslte_pdsch_mapping_type_A,
                                                              srslte_pdsch_mapping_type_A,
                                                              srslte_pdsch_mapping_type_B,
                                                              srslte_pdsch_mapping_type_B,
                                                              srslte_pdsch_mapping_type_B,
                                                              srslte_pdsch_mapping_type_B,
                                                              srslte_pdsch_mapping_type_B,
                                                              srslte_pdsch_mapping_type_B,
                                                              srslte_pdsch_mapping_type_A,
                                                              srslte_pdsch_mapping_type_A,
                                                              srslte_pdsch_mapping_type_A,
                                                              srslte_pdsch_mapping_type_B,
                                                              srslte_pdsch_mapping_type_B};
  grant->mapping                                           = pdsch_mapping_lut[m];

  static uint32_t S_pos2[16] = {2, 2, 2, 2, 2, 9, 4, 5, 5, 9, 12, 1, 1, 2, 4, 8};
  static uint32_t L_pos2[16] = {12, 10, 9, 7, 5, 4, 4, 7, 2, 2, 2, 13, 6, 4, 7, 4};
  static uint32_t S_pos3[16] = {3, 3, 3, 3, 3, 10, 6, 5, 5, 9, 12, 1, 1, 2, 4, 8};
  static uint32_t L_pos3[16] = {11, 9, 8, 6, 4, 4, 4, 7, 2, 2, 2, 13, 6, 4, 7, 4};

  // Select start symbol (S) and length (L)
  switch (dmrs_typeA_pos) {

    case srslte_dmrs_pdsch_typeA_pos_2:
      grant->S = S_pos2[m];
      grant->L = L_pos2[m];
      break;
    case srslte_dmrs_pdsch_typeA_pos_3:
      grant->S = S_pos3[m];
      grant->L = L_pos3[m];
      break;
    default:
      ERROR("Undefined case (%d)\n", dmrs_typeA_pos);
      return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}