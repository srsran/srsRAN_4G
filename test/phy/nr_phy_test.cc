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

#include "srsenb/hdr/phy/nr/worker_pool.h"
#include "srsran/common/test_common.h"
#include "srsue/hdr/phy/nr/worker_pool.h"

class phy_common : public srsran::phy_common_interface
{
public:
  void
  worker_end(void* h, bool tx_enable, srsran::rf_buffer_t& buffer, srsran::rf_timestamp_t& tx_time, bool is_nr) override
  {}
};

class ue_dummy_stack : public srsue::stack_interface_phy_nr
{
public:
  void         in_sync() override {}
  void         out_of_sync() override {}
  void         run_tti(const uint32_t tti) override {}
  int          sf_indication(const uint32_t tti) override { return 0; }
  sched_rnti_t get_dl_sched_rnti_nr(const uint32_t tti) override { return sched_rnti_t(); }
  sched_rnti_t get_ul_sched_rnti_nr(const uint32_t tti) override { return sched_rnti_t(); }
  void         new_grant_dl(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_t* action) override {}
  void tb_decoded(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_result_t result) override {}
  void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action) override {}
  void prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id) override {}
  bool sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx) override { return false; }
};

class test_bench
{
private:
  srsenb::nr::worker_pool gnb_phy;
  phy_common              gnb_phy_com;
  srsue::nr::worker_pool  ue_phy;
  phy_common              ue_phy_com;
  ue_dummy_stack          ue_stack;
  bool                    initialised = false;

public:
  struct args_t {
    uint32_t nof_threads = 6;
    uint32_t nof_prb     = 52;

    bool parse(int argc, char** argv);
  };

  test_bench(const args_t& args) : ue_phy(args.nof_threads), gnb_phy(args.nof_threads)
  {
    // Prepare cell list
    srsenb::phy_cell_cfg_list_nr_t cell_list(1);
    cell_list[0].carrier.nof_prb = args.nof_prb;

    // Prepare gNb PHY arguments
    srsenb::phy_args_t gnb_phy_args = {};

    // Initialise gnb
    if (not gnb_phy.init(cell_list, gnb_phy_args, gnb_phy_com, srslog::get_default_sink(), 31)) {
      return;
    }

    // Prepare PHY
    srsue::phy_args_nr_t ue_phy_args = {};

    // Initialise UE PHY
    if (not ue_phy.init(ue_phy_args, ue_phy_com, &ue_stack, 31)) {
      return;
    }

    initialised = true;
  }

  ~test_bench()
  {
    gnb_phy.stop();
    ue_phy.stop();
  }

  bool is_initialised() const { return initialised; }
};

bool test_bench::args_t::parse(int argc, char** argv)
{
  return true;
}

int main(int argc, char** argv)
{
  test_bench::args_t args = {};

  // Parse arguments
  TESTASSERT(args.parse(argc, argv));

  // Create test bench
  test_bench tb(args);

  // Assert bench is initialised correctly
  TESTASSERT(tb.is_initialised());

  // If reached here, the test is successful
  return SRSRAN_SUCCESS;
}