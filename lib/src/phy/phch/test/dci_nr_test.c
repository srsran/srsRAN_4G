/**
 * Copyright 2013-2021 Software Radio Systems Limited
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
