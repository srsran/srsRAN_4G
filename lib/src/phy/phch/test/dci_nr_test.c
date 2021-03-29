/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/common/test_common.h"
#include "srsran/phy/phch/dci_nr.h"

static int test_52prb()
{
  // Default configuration with all options disabled
  srsran_dci_cfg_nr_t cfg = {};

  // Set bandwidths
  cfg.coreset0_bw       = 0;
  cfg.bwp_dl_initial_bw = 52;
  cfg.bwp_dl_active_bw  = 52;
  cfg.bwp_ul_initial_bw = 52;
  cfg.bwp_ul_active_bw  = 52;

  // Enable all monitoring
  cfg.monitor_common_0_0  = true;
  cfg.monitor_0_0_and_1_0 = true;
  cfg.monitor_0_1_and_1_1 = true;

  // Configure DCI
  srsran_dci_nr_t dci = {};
  TESTASSERT(srsran_dci_nr_set_cfg(&dci, &cfg) == SRSRAN_SUCCESS);

  // Check DCI sizes
  TESTASSERT(srsran_dci_nr_size(&dci, srsran_search_space_type_common_3, srsran_dci_format_nr_0_0) == 39);
  TESTASSERT(srsran_dci_nr_size(&dci, srsran_search_space_type_common_3, srsran_dci_format_nr_1_0) == 39);
  TESTASSERT(srsran_dci_nr_size(&dci, srsran_search_space_type_ue, srsran_dci_format_nr_0_0) == 39);
  TESTASSERT(srsran_dci_nr_size(&dci, srsran_search_space_type_ue, srsran_dci_format_nr_1_0) == 39);
  TESTASSERT(srsran_dci_nr_size(&dci, srsran_search_space_type_ue, srsran_dci_format_nr_0_1) == 28);
  TESTASSERT(srsran_dci_nr_size(&dci, srsran_search_space_type_ue, srsran_dci_format_nr_1_1) == 26);

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  TESTASSERT(test_52prb() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}
