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

#include "srslte/phy/phch/ra_ul_nr.h"
#include "ra_helper.h"
#include "srslte/phy/ch_estimation/dmrs_pucch.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

typedef struct {
  srslte_sch_mapping_type_t mapping;
  uint32_t                  K2;
  uint32_t                  S;
  uint32_t                  L;
} ue_ra_time_resource_t;

static const ue_ra_time_resource_t ue_ul_default_A_lut[16] = {{srslte_sch_mapping_type_A, 0, 0, 14},
                                                              {srslte_sch_mapping_type_A, 0, 0, 12},
                                                              {srslte_sch_mapping_type_A, 0, 0, 10},
                                                              {srslte_sch_mapping_type_B, 0, 2, 10},
                                                              {srslte_sch_mapping_type_B, 0, 4, 10},
                                                              {srslte_sch_mapping_type_B, 0, 4, 8},
                                                              {srslte_sch_mapping_type_B, 0, 4, 6},
                                                              {srslte_sch_mapping_type_A, 1, 0, 14},
                                                              {srslte_sch_mapping_type_A, 1, 0, 12},
                                                              {srslte_sch_mapping_type_A, 1, 0, 10},
                                                              {srslte_sch_mapping_type_A, 2, 0, 14},
                                                              {srslte_sch_mapping_type_A, 2, 0, 12},
                                                              {srslte_sch_mapping_type_A, 2, 0, 10},
                                                              {srslte_sch_mapping_type_B, 0, 8, 6},
                                                              {srslte_sch_mapping_type_A, 3, 0, 14},
                                                              {srslte_sch_mapping_type_A, 3, 0, 10}};

int srslte_ra_ul_nr_pdsch_time_resource_default_A(uint32_t scs_cfg, uint32_t m, srslte_sch_grant_nr_t* grant)
{
  uint32_t j[4] = {1, 1, 2, 3};

  if (grant == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (scs_cfg > 4) {
    ERROR("Invalid numerology (%d)", scs_cfg);
    return SRSLTE_ERROR;
  }

  if (m >= 16) {
    ERROR("m (%d) is out-of-range", m);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Select mapping
  grant->mapping = ue_ul_default_A_lut[m].mapping;
  grant->k       = ue_ul_default_A_lut[m].K2 + j[scs_cfg];
  grant->S       = ue_ul_default_A_lut[m].S;
  grant->L       = ue_ul_default_A_lut[m].L;

  return SRSLTE_SUCCESS;
}

static void ra_ul_nr_time_hl(const srslte_sch_time_ra_t* hl_ra_cfg, srslte_sch_grant_nr_t* grant)
{
  // Compute S and L from SLIV from higher layers
  ra_helper_compute_s_and_l(SRSLTE_NSYMB_PER_SLOT_NR, hl_ra_cfg->sliv, &grant->S, &grant->L);

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

bool srslte_ra_ul_nr_time_validate(srslte_sch_grant_nr_t* grant)
{
  if (grant->mapping == srslte_sch_mapping_type_A) {
    return check_time_ra_typeA(&grant->S, &grant->L);
  }

  return check_time_ra_typeB(&grant->S, &grant->L);
}

int srslte_ra_ul_nr_time(const srslte_sch_hl_cfg_nr_t*    cfg,
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
    ERROR("m (%d) is out-of-range", m);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Determine which PUSCH Time domain RA configuration to apply (TS 38.214 Table 6.1.2.1.1-1:)
  if (rnti_type == srslte_rnti_type_ra) {
    // Row 1
    if (cfg->nof_common_time_ra == 0) {
      srslte_ra_ul_nr_pdsch_time_resource_default_A(cfg->scs_cfg, m, grant);
    } else if (m < SRSLTE_MAX_NOF_DL_ALLOCATION) {
      ra_ul_nr_time_hl(&cfg->common_time_ra[m], grant);
    }
  } else if ((rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_mcs_c ||
              rnti_type == srslte_rnti_type_tc || rnti_type == srslte_rnti_type_cs) &&
             SRSLTE_SEARCH_SPACE_IS_COMMON(ss_type) && coreset_id == 0) {
    // Row 2
    if (cfg->nof_common_time_ra == 0) {
      srslte_ra_ul_nr_pdsch_time_resource_default_A(cfg->scs_cfg, m, grant);
    } else if (m < SRSLTE_MAX_NOF_DL_ALLOCATION) {
      ra_ul_nr_time_hl(&cfg->common_time_ra[m], grant);
    }
  } else if ((rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_mcs_c ||
              rnti_type == srslte_rnti_type_tc || rnti_type == srslte_rnti_type_cs ||
              rnti_type == srslte_rnti_type_sp_csi) &&
             ((SRSLTE_SEARCH_SPACE_IS_COMMON(ss_type) && coreset_id != 0) || ss_type == srslte_search_space_type_ue)) {
    // Row 3
    if (cfg->nof_dedicated_time_ra > 0) {
      ra_ul_nr_time_hl(&cfg->dedicated_time_ra[m], grant);
    } else if (cfg->nof_common_time_ra > 0) {
      ra_ul_nr_time_hl(&cfg->common_time_ra[m], grant);
    } else {
      srslte_ra_ul_nr_pdsch_time_resource_default_A(cfg->scs_cfg, m, grant);
    }
  } else {
    ERROR("Unhandled case");
  }

  // Table 6.1.2.1.1-5 defines the additional subcarrier spacing specific slot delay value for the first transmission of
  // PUSCH scheduled by the RAR. When the UE transmits a PUSCH scheduled by RAR, the Δ value specific to the PUSCH
  // subcarrier spacing μ PUSCH is applied in addition to the K 2 value.
  if (rnti_type == srslte_rnti_type_ra) {
    uint32_t delta[4] = {2, 3, 4, 6};
    if (cfg->scs_cfg >= 4) {
      ERROR("Invalid numerology");
      return SRSLTE_ERROR;
    }
    grant->k += delta[cfg->scs_cfg];
  }

  // Validate S and L parameters
  if (!srslte_ra_ul_nr_time_validate(grant)) {
    ERROR("Invalid Time RA S=%d; L=%d; m=%d", grant->S, grant->L, m);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_ra_ul_nr_nof_dmrs_cdm_groups_without_data_format_0_0(const srslte_sch_cfg_nr_t* cfg,
                                                                srslte_sch_grant_nr_t*     grant)
{
  if (cfg == NULL || grant == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  /* According to TS 38.214 V15.10.0 6.2.2 UE DM-RS transmission procedure:
   * For PUSCH scheduled by DCI format 0_0 or by activation DCI format 0_0 with CRC scrambled by CS-RNTI, the UE
   * shall assume the number of DM-RS CDM groups without data is 1 which corresponds to CDM group 0 for the case of
   * PUSCH with allocation duration of 2 or less OFDM symbols with transform precoding disabled, the UE shall assume
   * that the number of DM-RS CDM groups without data is 3 which corresponds to CDM group {0,1,2} for the case of PUSCH
   * scheduled by activation DCI format 0_0 and the dmrs-Type in cg-DMRS-Configuration equal to 'type2' and the PUSCH
   * allocation duration being more than 2 OFDM symbols, and the UE shall assume that the number of DM-RS CDM groups
   * without data is 2 which corresponds to CDM group {0,1} for all other cases.
   */
  if (grant->L <= 2 && !cfg->enable_transform_precoder) {
    grant->nof_dmrs_cdm_groups_without_data = 1;
    //  } else if (grant->L > 2 && cfg->dmrs_cg.type == srslte_dmrs_sch_type_2){
    //    grant->nof_dmrs_cdm_groups_without_data = 3;
  } else {
    grant->nof_dmrs_cdm_groups_without_data = 2;
  }

  return SRSLTE_SUCCESS;
}

int srslte_ra_ul_nr_dmrs_power_offset(srslte_sch_grant_nr_t* grant)
{
  if (grant == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  float ratio_dB[3] = {0, -3, -4.77};

  if (grant->nof_dmrs_cdm_groups_without_data < 1 || grant->nof_dmrs_cdm_groups_without_data > 3) {
    ERROR("Invalid number of DMRS CDM groups without data (%d)", grant->nof_dmrs_cdm_groups_without_data);
    return SRSLTE_ERROR;
  }

  grant->beta_dmrs = srslte_convert_dB_to_amplitude(-ratio_dB[grant->nof_dmrs_cdm_groups_without_data - 1]);

  return SRSLTE_SUCCESS;
}

#define RA_UL_PUCCH_CODE_RATE_N 8
#define RA_UL_PUCCH_CODE_RATE_RESERVED NAN

static const double ra_ul_pucch_code_rate_table[RA_UL_PUCCH_CODE_RATE_N] =
    {0.08, 0.15, 0.25, 0.35, 0.45, 0.60, 0.80, RA_UL_PUCCH_CODE_RATE_RESERVED};

// Implements TS 38.213 Table 9.2.5.2-1: Code rate r corresponding to value of maxCodeRate
static double ra_ul_nr_pucch_code_rate_r(const srslte_pucch_nr_resource_t* resource)
{
  if (resource->max_code_rate >= RA_UL_PUCCH_CODE_RATE_RESERVED) {
    ERROR("Invalid code rate");
    return RA_UL_PUCCH_CODE_RATE_RESERVED;
  }

  return ra_ul_pucch_code_rate_table[resource->max_code_rate];
}

// Calculate number of PRBs for PUCCH format 2, or PUCCH format 3, or PUCCH format 4, respectively
// static int ra_ul_nr_pucch_Mrb(const srslte_pucch_nr_resource_t* resource)
//{
//  switch (resource->format) {
//    case SRSLTE_PUCCH_NR_FORMAT_2:
//    case SRSLTE_PUCCH_NR_FORMAT_3:
//      return resource->nof_prb;
//    case SRSLTE_PUCCH_NR_FORMAT_4:
//      return SRSLTE_PUCCH_NR_FORMAT4_NPRB;
//    default:
//      ERROR("Invalid case");
//      break;
//  }
//  return SRSLTE_ERROR;
//}

// Calculate number of subcarriers per resource block for payload (No DMRS)
static int ra_ul_nr_pucch_nre(const srslte_pucch_nr_resource_t* resource)
{
  switch (resource->format) {
    case SRSLTE_PUCCH_NR_FORMAT_2:
      return SRSLTE_NRE - 4;
    case SRSLTE_PUCCH_NR_FORMAT_3:
      return SRSLTE_NRE;
    case SRSLTE_PUCCH_NR_FORMAT_4:
      return SRSLTE_NRE / resource->occ_lenth;
    default:
      ERROR("Invalid case");
      break;
  }
  return SRSLTE_ERROR;
}

// Calculate number of PUCCH symbols excluding the ones used exclusively for DMRS for formats 3 and 4
static int ra_ul_nr_pucch_nsymb(const srslte_pucch_nr_resource_t* resource)
{
  switch (resource->format) {
    case SRSLTE_PUCCH_NR_FORMAT_2:
      return resource->nof_symbols;
    case SRSLTE_PUCCH_NR_FORMAT_3:
    case SRSLTE_PUCCH_NR_FORMAT_4: {
      uint32_t idx[SRSLTE_DMRS_PUCCH_FORMAT_3_4_MAX_NSYMB] = {};

      // Get number of DMRS symbols for format 3 or 4
      int nsymb_dmrs = srslte_dmrs_pucch_format_3_4_get_symbol_idx(resource, idx);
      if (nsymb_dmrs < SRSLTE_SUCCESS) {
        return SRSLTE_ERROR;
      }

      return (int)resource->nof_symbols - nsymb_dmrs;
    }
    default:
      ERROR("Invalid case");
      break;
  }
  return SRSLTE_ERROR;
}

// Calculate number of PUCCH symbols excluding the ones used exclusively for DMRS for formats 3 and 4
static int ra_ul_nr_pucch_qm(const srslte_pucch_nr_resource_t* resource)
{
  switch (resource->format) {
    case SRSLTE_PUCCH_NR_FORMAT_2:
      return 2;
    case SRSLTE_PUCCH_NR_FORMAT_3:
    case SRSLTE_PUCCH_NR_FORMAT_4:
      return resource->enable_pi_bpsk ? 1 : 2;
    default:
      ERROR("Invalid case");
      break;
  }
  return SRSLTE_ERROR;
}

int srslte_ra_ul_nr_pucch_format_2_3_min_prb(const srslte_pucch_nr_resource_t* resource,
                                             const srslte_uci_cfg_nr_t*        uci_cfg)
{
  if (resource == NULL || uci_cfg == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Get maximum allowed code rate
  double r = ra_ul_nr_pucch_code_rate_r(resource);
  if (!isnormal(r)) {
    ERROR("Invalid coderate %f", r);
    return SRSLTE_ERROR;
  }

  // Get number of RE/PRB
  int nre = ra_ul_nr_pucch_nre(resource);
  if (nre < SRSLTE_SUCCESS) {
    ERROR("Getting nre");
    return SRSLTE_ERROR;
  }

  // Get number of symbols
  int nsymb = ra_ul_nr_pucch_nsymb(resource);
  if (nsymb < SRSLTE_SUCCESS) {
    ERROR("Getting nsymb");
    return SRSLTE_ERROR;
  }

  // Get modulation order
  int qm = ra_ul_nr_pucch_qm(resource);
  if (qm < SRSLTE_SUCCESS) {
    ERROR("Getting qm");
    return SRSLTE_ERROR;
  }

  // Calculate denominator
  double nof_bits_rb = r * nre * nsymb * qm;
  if (!isnormal(nof_bits_rb)) {
    return SRSLTE_ERROR;
  }

  // Compute total number of UCI bits
  uint32_t O_total = uci_cfg->o_ack + uci_cfg->o_sr + uci_cfg->o_csi1 + uci_cfg->o_csi2;

  // Add CRC bits if any
  O_total += srslte_uci_nr_crc_len(O_total);

  // Return the minimum
  return (int)ceil(O_total / nof_bits_rb);
}

int srslte_ra_ul_nr_freq(const srslte_carrier_nr_t*    carrier,
                         const srslte_sch_hl_cfg_nr_t* cfg,
                         const srslte_dci_ul_nr_t*     dci_ul,
                         srslte_sch_grant_nr_t*        grant)
{
  if (cfg == NULL || grant == NULL || dci_ul == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // RA scheme
  if (dci_ul->format == srslte_dci_format_nr_0_0) {
    // when the scheduling grant is received with DCI format 1_0 , then downlink resource allocation type 1 is used.
    return ra_helper_freq_type1(carrier->nof_prb, dci_ul->freq_domain_assigment, grant);
  }

  ERROR("Only DCI Format 0_0 is supported");
  return SRSLTE_ERROR;
}

// Implements TS 38.213 Table 9.2.1-1: PUCCH resource sets before dedicated PUCCH resource configuration
static int ra_ul_nr_pucch_resource_default(uint32_t r_pucch, srslte_pucch_nr_resource_t* resource)
{
  ERROR("Not implemented");
  return SRSLTE_ERROR;
}

static int ra_ul_nr_pucch_resource_hl(const srslte_pucch_nr_hl_cfg_t* cfg,
                                      uint32_t                        O_uci,
                                      uint32_t                        pucch_resource_id,
                                      srslte_pucch_nr_resource_t*     resource)
{
  uint32_t N2 = cfg->sets[1].max_payload_size > 0 ? cfg->sets[1].max_payload_size : SRSLTE_UCI_NR_MAX_NOF_BITS;
  uint32_t N3 = cfg->sets[2].max_payload_size > 0 ? cfg->sets[2].max_payload_size : SRSLTE_UCI_NR_MAX_NOF_BITS;

  // If the UE transmits O UCI UCI information bits, that include HARQ-ACK information bits, the UE determines a PUCCH
  // resource set to be...
  uint32_t resource_set_id = 3;
  if (O_uci <= 2 && cfg->sets[0].nof_resources > 0) {
    resource_set_id = 0;
  } else if (O_uci <= N2 && cfg->sets[1].nof_resources > 0) {
    resource_set_id = 1;
  } else if (O_uci <= N3 && cfg->sets[2].nof_resources > 0) {
    resource_set_id = 2;
  } else if (cfg->sets[3].nof_resources == 0) {
    ERROR("Invalid PUCCH resource configuration, N3=%d, O_uci=%d", N3, O_uci);
    return SRSLTE_ERROR;
  } else if (O_uci > SRSLTE_UCI_NR_MAX_NOF_BITS) {
    ERROR("The number of UCI bits (%d), exceeds the maximum (%d)", O_uci, SRSLTE_UCI_NR_MAX_NOF_BITS);
    return SRSLTE_ERROR;
  }

  // Select resource from the set
  if (pucch_resource_id >= SRSLTE_PUCCH_NR_MAX_NOF_RESOURCES_PER_SET ||
      pucch_resource_id >= cfg->sets[resource_set_id].nof_resources) {
    ERROR("The PUCCH resource identifier (%d) exceeds the number of configured resources (%d) for set identifier %d",
          pucch_resource_id,
          cfg->sets[resource_set_id].nof_resources,
          resource_set_id);
    return SRSLTE_ERROR;
  }
  *resource = cfg->sets[resource_set_id].resources[pucch_resource_id];

  return SRSLTE_SUCCESS;
}

int srslte_ra_ul_nr_pucch_resource(const srslte_pucch_nr_hl_cfg_t* pucch_cfg,
                                   const srslte_uci_cfg_nr_t*      uci_cfg,
                                   srslte_pucch_nr_resource_t*     resource)
{
  if (pucch_cfg == NULL || uci_cfg == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  uint32_t O_uci = srslte_uci_nr_total_bits(uci_cfg);

  // If a UE does not have dedicated PUCCH resource configuration, provided by PUCCH-ResourceSet in PUCCH-Config,
  // a PUCCH resource set is provided by pucch-ResourceCommon through an index to a row of Table 9.2.1-1 for size
  // transmission of HARQ-ACK information on PUCCH in an initial UL BWP of N BWP PRBs.
  if (!pucch_cfg->enabled) {
    uint32_t r_pucch = (2 * uci_cfg->n_cce_0) + 2 * uci_cfg->pucch_resource_id;
    return ra_ul_nr_pucch_resource_default(r_pucch, resource);
  }
  return ra_ul_nr_pucch_resource_hl(pucch_cfg, O_uci, uci_cfg->pucch_resource_id, resource);
}