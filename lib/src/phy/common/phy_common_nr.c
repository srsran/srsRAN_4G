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

#include "srslte/phy/common/phy_common_nr.h"
#include <string.h>

uint32_t srslte_coreset_get_bw(const srslte_coreset_t* coreset)
{
  uint32_t prb_count = 0;

  // Iterate all the frequency domain resources bit-map...
  for (uint32_t i = 0; i < SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    // ... and count 6 PRB for every frequency domain resource that it is enabled
    if (coreset->freq_resources[i]) {
      prb_count += 6;
    }
  }

  // Return the total count of physical resource blocks
  return prb_count;
}

uint32_t srslte_coreset_get_sz(const srslte_coreset_t* coreset)
{
  // Returns the number of resource elements in time and frequency domains
  return srslte_coreset_get_bw(coreset) * SRSLTE_NRE * coreset->duration;
}

const char* srslte_sch_mapping_type_to_str(srslte_sch_mapping_type_t mapping_type)
{
  switch (mapping_type) {
    case srslte_sch_mapping_type_A:
      return "A";
    case srslte_sch_mapping_type_B:
      return "B";
    default:
      return "undefined";
  }
}

const char* srslte_mcs_table_to_str(srslte_mcs_table_t mcs_table)
{
  switch (mcs_table) {
    case srslte_mcs_table_64qam:
      return "64qam";
    case srslte_mcs_table_256qam:
      return "256qam";
    case srslte_mcs_table_qam64LowSE:
      return "qam64LowSE";
    default:
      return "undefined";
  }
}

srslte_mcs_table_t srslte_mcs_table_from_str(const char* str)
{
  if (strcmp(str, "64qam") == 0) {
    return srslte_mcs_table_64qam;
  }
  if (strcmp(str, "256qam") == 0) {
    return srslte_mcs_table_256qam;
  }
  if (strcmp(str, "qam64LowSE") == 0) {
    return srslte_mcs_table_qam64LowSE;
  }
  return srslte_mcs_table_N;
}

#define PHY_COMMON_NR_NOF_VALID_SYMB_SZ 10U

static const uint32_t phy_common_nr_valid_symbol_sz[PHY_COMMON_NR_NOF_VALID_SYMB_SZ] =
    {128, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096};

uint32_t srslte_min_symbol_sz_rb(uint32_t nof_prb)
{
  uint32_t nof_re = nof_prb * SRSLTE_NRE;

  if (nof_re == 0) {
    return 0;
  }

  for (uint32_t i = 0; i < PHY_COMMON_NR_NOF_VALID_SYMB_SZ; i++) {
    if (phy_common_nr_valid_symbol_sz[i] > nof_re) {
      return phy_common_nr_valid_symbol_sz[i];
    }
  }

  return 0;
}
