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
