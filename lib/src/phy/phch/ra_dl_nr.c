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
#include "srslte/phy/phch/ra_dl_nr.h"
#include "srslte/phy/utils/debug.h"

static void compute_s_and_l(uint32_t N, uint32_t v, uint32_t* S, uint32_t* L)
{
  uint32_t low  = v % N;
  uint32_t high = v / N;
  if (high + 1 + low <= N) {
    *S = low;
    *L = high + 1;
  } else {
    *S = N - 1 - low;
    *L = N - high + 1;
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
  compute_s_and_l(SRSLTE_NSYMB_PER_SLOT_NR, hl_ra_cfg->sliv, &grant->S, &grant->L);

  grant->k0      = hl_ra_cfg->k0;
  grant->mapping = hl_ra_cfg->mapping_type;
}

int srslte_ra_dl_nr_time(const srslte_pdsch_cfg_nr_t*     cfg,
                         const srslte_rnti_type_t         rnti_type,
                         const srslte_search_space_type_t ss_type,
                         const uint32_t                   coreset_id,
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

  // Determine which PDSCH Time domain RA configuration to apply (TS 38.214 Table 5.1.2.1.1-1)
  if (rnti_type == srslte_rnti_type_si && ss_type == srslte_search_space_type_common_0) {
    // Row 1
    ERROR("Row not implemented");
  } else if (rnti_type == srslte_rnti_type_si && ss_type == srslte_search_space_type_common_0A) {
    // Row 2
    ERROR("Row not implemented");
  } else if ((rnti_type == srslte_rnti_type_ra || rnti_type == srslte_rnti_type_tc) &&
             ss_type == srslte_search_space_type_common_1) {
    // Row 3
    if (cfg->nof_common_pdsch_time_ra > 0) {
      srslte_ra_dl_nr_time_hl(&cfg->common_pdsch_time_ra[m], grant);
    } else {
      // Note: Only Default A is supported, which corresponds SS/PBCH block and coreset mux pattern 1
      srslte_ra_dl_nr_time_default_A(m, cfg->typeA_pos, grant);
    }
  } else if (rnti_type == srslte_rnti_type_p && ss_type == srslte_search_space_type_common_2) {
    // Row 4
    ERROR("Row not implemented");
  } else if ((rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_mcs_c ||
              rnti_type == srslte_rnti_type_cs) &&
             SRSLTE_SEARCH_SPACE_IS_COMMON(ss_type) && coreset_id == 0) {
    // Row 5
    if (cfg->nof_common_pdsch_time_ra > 0) {
      srslte_ra_dl_nr_time_hl(&cfg->common_pdsch_time_ra[m], grant);
    } else {
      srslte_ra_dl_nr_time_default_A(m, cfg->typeA_pos, grant);
    }
  } else if ((rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_mcs_c ||
              rnti_type == srslte_rnti_type_cs) &&
             ((SRSLTE_SEARCH_SPACE_IS_COMMON(ss_type) && coreset_id != 0) || ss_type == srslte_search_space_type_ue)) {
    // Row 6
    if (cfg->nof_pdsch_time_ra > 0) {
      srslte_ra_dl_nr_time_hl(&cfg->pdsch_time_ra[m], grant);
    } else if (cfg->nof_common_pdsch_time_ra > 0) {
      srslte_ra_dl_nr_time_hl(&cfg->common_pdsch_time_ra[m], grant);
    } else {
      srslte_ra_dl_nr_time_default_A(m, cfg->typeA_pos, grant);
    }
  } else {
    ERROR("Unhandled case");
  }

  // Validate S and L parameters
  if (!srslte_ra_dl_nr_time_validate(grant)) {
    ERROR("Invalid Time RA S=%d; L=%d; m=%d\n", grant->S, grant->L, m);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_ra_dl_nr_nof_dmrs_cdm_groups_without_data_format_1_0(const srslte_dmrs_sch_cfg_t* cfg,
                                                                srslte_sch_grant_nr_t*       grant)
{
  if (cfg == NULL || grant == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  /* According to TS 38.214 V15.10.0 5.1.6.1.3 CSI-RS for mobility:
   * When receiving PDSCH scheduled by DCI format 1_0, the UE shall assume the number of DM-RS CDM groups without data
   * is 1 which corresponds to CDM group 0 for the case of PDSCH with allocation duration of 2 symbols, and the UE
   * shall assume that the number of DM-RS CDM groups without data is 2 which corresponds to CDM group {0,1} for all
   * other cases.
   */
  if (cfg->length == srslte_dmrs_sch_len_2) {
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

static int ra_freq_type0(const srslte_carrier_nr_t*   carrier,
                         const srslte_pdsch_cfg_nr_t* cfg,
                         const srslte_dci_dl_nr_t*    dci_dl,
                         srslte_sch_grant_nr_t*       grant)
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

static int
ra_freq_type1(const srslte_carrier_nr_t* carrier, const srslte_dci_dl_nr_t* dci_dl, srslte_sch_grant_nr_t* grant)
{

  uint32_t riv        = dci_dl->freq_domain_assigment;
  uint32_t N_bwp_size = carrier->nof_prb;

  uint32_t start = 0;
  uint32_t len   = 0;
  compute_s_and_l(N_bwp_size, riv, &start, &len);

  if (start + len > N_bwp_size) {
    ERROR("RIV 0x%x for BWP size %d resulted in freq=%d:%d\n", riv, N_bwp_size, start, len);
    return SRSLTE_ERROR;
  }

  for (uint32_t i = 0; i < start; i++) {
    grant->prb_idx[i] = false;
  }

  for (uint32_t i = start; i < start + len; i++) {
    grant->prb_idx[i] = true;
  }

  for (uint32_t i = start + len; i < SRSLTE_MAX_PRB_NR; i++) {
    grant->prb_idx[i] = false;
  }
  grant->nof_prb = len;

  return SRSLTE_SUCCESS;
}

int srslte_ra_dl_nr_freq(const srslte_carrier_nr_t*   carrier,
                         const srslte_pdsch_cfg_nr_t* cfg,
                         const srslte_dci_dl_nr_t*    dci_dl,
                         srslte_sch_grant_nr_t*       grant)
{

  if (cfg == NULL || grant == NULL || dci_dl == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // RA scheme
  if (dci_dl->format == srslte_dci_format_nr_1_0) {
    // when the scheduling grant is received with DCI format 1_0 , then downlink resource allocation type 1 is used.
    return ra_freq_type1(carrier, dci_dl, grant);
  }

  ra_freq_type0(carrier, cfg, dci_dl, grant);
  ERROR("Only DCI Format 1_0 is supported\n");
  return SRSLTE_ERROR;
}
