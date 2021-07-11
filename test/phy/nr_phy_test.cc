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

#include "dummy_gnb_stack.h"
#include "dummy_ue_stack.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/test_common.h"
#include "test_bench.h"

test_bench::args_t::args_t(int argc, char** argv)
{
  // Flag configuration as valid
  valid = true;

  // Load default reference configuration
  srsran::phy_cfg_nr_default_t::reference_cfg_t reference_cfg;
  phy_cfg = srsran::phy_cfg_nr_default_t(reference_cfg);

  cell_list.resize(1);
  cell_list[0].carrier = phy_cfg.carrier;
  cell_list[0].rf_port = 0;
  cell_list[0].cell_id = 0;
  cell_list[0].pdcch   = phy_cfg.pdcch;
}

int main(int argc, char** argv)
{
  srslog::init();

  // Parse test bench arguments
  test_bench::args_t args(argc, argv);
  args.gnb_args.log_id_preamble  = "GNB/";
  args.gnb_args.log_level        = "info";
  args.gnb_args.nof_phy_threads  = 1;
  args.ue_args.log.id_preamble   = " UE/";
  args.ue_args.log.phy_level     = "info";
  args.ue_args.log.phy_hex_limit = 1;
  args.ue_args.nof_phy_threads   = 1;

  // Parse arguments
  TESTASSERT(args.valid);

  // Create UE stack arguments
  ue_dummy_stack::args_t ue_stack_args = {};
  ue_stack_args.rnti                   = 0x1234;

  // Create UE stack
  ue_dummy_stack ue_stack(ue_stack_args);
  TESTASSERT(ue_stack.is_valid());

  // Create GNB stack arguments
  gnb_dummy_stack::args_t gnb_stack_args = {};
  gnb_stack_args.rnti                    = 0x1234;
  gnb_stack_args.mcs                     = 10;
  gnb_stack_args.phy_cfg                 = args.phy_cfg;
  gnb_stack_args.dl_start_rb             = 0;
  gnb_stack_args.dl_length_rb            = args.phy_cfg.carrier.nof_prb;
  gnb_stack_args.ul_start_rb             = 0;
  gnb_stack_args.ul_length_rb            = args.phy_cfg.carrier.nof_prb;

  // Create GNB stack
  gnb_dummy_stack gnb_stack(gnb_stack_args);
  TESTASSERT(gnb_stack.is_valid());

  // Create test bench
  test_bench tb(args, gnb_stack, ue_stack);

  // Assert bench is initialised correctly
  TESTASSERT(tb.is_initialised());

  // Run per TTI basis
  for (uint32_t i = 0; i < 1000; i++) {
    TESTASSERT(tb.run_tti());
  }

  // Stop test bench
  tb.stop();

  // Flush log
  srslog::flush();

  // Retrieve MAC metrics
  srsenb::mac_ue_metrics_t mac_metrics = gnb_stack.get_metrics();

  // Print metrics
  float pdsch_bler = 0.0f;
  if (mac_metrics.tx_pkts != 0) {
    pdsch_bler = (float)mac_metrics.tx_errors / (float)mac_metrics.tx_pkts;
  }
  float pusch_bler = 0.0f;
  if (mac_metrics.rx_pkts != 0) {
    pusch_bler = (float)mac_metrics.rx_errors / (float)mac_metrics.rx_pkts;
  }
  float pdsch_shed_rate = 0.0f;
  if (mac_metrics.tx_pkts != 0) {
    pdsch_shed_rate = (float)mac_metrics.tx_brate / (float)mac_metrics.tx_pkts / 1000.0f;
  }
  float pusch_shed_rate = 0.0f;
  if (mac_metrics.rx_pkts != 0) {
    pusch_shed_rate = (float)mac_metrics.rx_brate / (float)mac_metrics.rx_pkts / 1000.0f;
  }

  srsran::console("PDSCH:\n");
  srsran::console("       Count: %d\n", mac_metrics.tx_pkts);
  srsran::console("        BLER: %f\n", pdsch_bler);
  srsran::console("  Sched Rate: %f Mbps\n", pdsch_shed_rate);
  srsran::console("    Net Rate: %f Mbps\n", (1.0f - pdsch_bler) * pdsch_shed_rate);
  srsran::console("   Retx Rate: %f Mbps\n", pdsch_bler * pdsch_shed_rate);

  srsran::console("\n");
  srsran::console("PUSCH:\n");
  srsran::console("       Count: %d\n", mac_metrics.rx_pkts);
  srsran::console("        BLER: %f\n", pusch_bler);
  srsran::console("  Sched Rate: %f Mbps\n", pusch_shed_rate);
  srsran::console("    Net Rate: %f Mbps\n", (1.0f - pusch_bler) * pusch_shed_rate);
  srsran::console("   Retx Rate: %f Mbps\n", pusch_bler * pusch_shed_rate);

  // Assert metrics
  TESTASSERT(mac_metrics.tx_errors == 0);
  TESTASSERT(mac_metrics.rx_errors == 0);

  // If reached here, the test is successful
  return SRSRAN_SUCCESS;
}
