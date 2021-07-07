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

#ifndef SRSRAN_DUMMY_UE_STACK_H
#define SRSRAN_DUMMY_UE_STACK_H

#include <srsran/interfaces/ue_nr_interfaces.h>

class ue_dummy_stack : public srsue::stack_interface_phy_nr
{
private:
  srsran_random_t random_gen = srsran_random_init(0x4567);
  uint16_t        rnti       = 0;
  bool            valid      = false;
  uint32_t        sr_period  = 0;
  uint32_t        sr_count   = 0;

  srsran::circular_array<dummy_tx_harq_proc, SRSRAN_MAX_HARQ_PROC_DL_NR> tx_harq_proc;
  srsran::circular_array<dummy_rx_harq_proc, SRSRAN_MAX_HARQ_PROC_DL_NR> rx_harq_proc;

public:
  struct args_t {
    uint16_t rnti      = 0x1234; ///< C-RNTI for PUSCH and PDSCH transmissions
    uint32_t sr_period = 0;      ///< Indicates positive SR period in number of opportunities. Set to 0 to disable.
  };
  ue_dummy_stack(const args_t& args) : rnti(args.rnti), sr_period(args.sr_period) { valid = true; }
  ~ue_dummy_stack() { srsran_random_free(random_gen); }
  void         in_sync() override {}
  void         out_of_sync() override {}
  void         run_tti(const uint32_t tti) override {}
  int          sf_indication(const uint32_t tti) override { return 0; }
  sched_rnti_t get_dl_sched_rnti_nr(const uint32_t tti) override { return {rnti, srsran_rnti_type_c}; }
  sched_rnti_t get_ul_sched_rnti_nr(const uint32_t tti) override { return {rnti, srsran_rnti_type_c}; }
  void         new_grant_dl(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_t* action) override
  {
    action->tb.enabled    = true;
    action->tb.softbuffer = &rx_harq_proc[grant.pid].get_softbuffer(grant.ndi);
  }
  void tb_decoded(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_result_t result) override {}
  void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action) override
  {
    if (action == nullptr) {
      return;
    }
    action->tb.enabled    = true;
    action->tb.payload    = &tx_harq_proc[grant.pid].get_tb(grant.tbs);
    action->tb.softbuffer = &tx_harq_proc[grant.pid].get_softbuffer(grant.ndi);
    srsran_random_byte_vector(random_gen, action->tb.payload->msg, grant.tbs / 8);
  }
  void prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id) override {}
  bool sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx) override
  {
    if (sr_period == 0) {
      return false;
    }

    bool ret = (sr_count % sr_period == 0);

    sr_count++;

    return ret;
  }
  bool is_valid() const { return valid; }
};

#endif // SRSRAN_DUMMY_UE_STACK_H
