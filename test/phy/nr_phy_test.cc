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

#include "dummy_gnb_stack.h"
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

class ue_dummy_stack : public srsue::stack_interface_phy_nr
{
private:
  uint16_t rnti  = 0;
  bool     valid = false;

  srsran::circular_array<dummy_tx_harq_proc, SRSRAN_MAX_HARQ_PROC_DL_NR> tx_harq_proc;
  srsran::circular_array<dummy_rx_harq_proc, SRSRAN_MAX_HARQ_PROC_DL_NR> rx_harq_proc;

public:
  struct args_t {
    uint16_t rnti = 0x1234;
  };
  ue_dummy_stack(const args_t& args) : rnti(args.rnti) { valid = true; }
  void         in_sync() override {}
  void         out_of_sync() override {}
  void         run_tti(const uint32_t tti) override {}
  int          sf_indication(const uint32_t tti) override { return 0; }
  sched_rnti_t get_dl_sched_rnti_nr(const uint32_t tti) override { return {rnti, srsran_rnti_type_c}; }
  sched_rnti_t get_ul_sched_rnti_nr(const uint32_t tti) override { return {rnti, srsran_rnti_type_c}; }
  void         new_grant_dl(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_t* action) override
  {
    action->tb.enabled    = true;
    action->tb.softbuffer = &rx_harq_proc[grant.pid].softbuffer;
  }
  void tb_decoded(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_result_t result) override {}
  void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action) override
  {
    if (action == nullptr) {
      return;
    }
    action->tb.enabled = true;
    action->tb.payload = &rx_harq_proc[grant.pid].data;
  }
  void prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id) override {}
  bool sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx) override { return false; }
  bool is_valid() const { return valid; }
};

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
  args.ue_args.log.phy_hex_limit = 0;
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
  float pdsch_rate = 0.0f;
  if (mac_metrics.tx_pkts != 0) {
    pdsch_rate = (float)mac_metrics.tx_brate / (float)mac_metrics.tx_pkts / 1000.0f;
  }

  srsran::console("PDSCH:\n");
  srsran::console("  Count: %d\n", mac_metrics.tx_pkts);
  srsran::console("   BLER: %f\n", pdsch_bler);
  srsran::console("   Rate: %f Mbps\n", pdsch_rate);

  // Assert metrics
  TESTASSERT(mac_metrics.tx_errors == 0);

  // If reached here, the test is successful
  return SRSRAN_SUCCESS;
}
