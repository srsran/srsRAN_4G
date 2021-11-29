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

#ifndef SRSUE_PHY_NR_SA_H
#define SRSUE_PHY_NR_SA_H

#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsue/hdr/phy/nr/sync_sa.h"
#include "srsue/hdr/phy/ue_nr_phy_base.h"

namespace srsue {

/**
 * @brief NR Standalone PHY
 */
class phy_nr_sa final : public ue_nr_phy_base
{
public:
  phy_nr_sa(const char* logname);

  int init(const phy_args_nr_t& args_, stack_interface_phy_nr* stack_, srsran::radio_interface_phy* radio_) final;

  std::string get_type() final { return "nr_soft"; }

  int set_rar_grant(uint32_t                                       rar_slot_idx,
                    std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS> packed_ul_grant,
                    uint16_t                                       rnti,
                    srsran_rnti_type_t                             rnti_type) final
  {
    return -1;
  }

  void send_prach(const uint32_t prach_occasion,
                  const int      preamble_index,
                  const float    preamble_received_target_power,
                  const float    ta_base_sec) final
  {}
  void set_earfcn(std::vector<uint32_t> earfcns) final{};
  bool has_valid_sr_resource(uint32_t sr_id) final { return false; }
  void clear_pending_grants() final {}
  bool set_config(const srsran::phy_cfg_nr_t& cfg) final { return false; }

  phy_nr_state_t get_state() const final;
  void           reset_nr() final;
  bool           start_cell_search(const cell_search_args_t& req) final;
  bool           start_cell_select(const cell_search_args_t& req) final { return false; }

  void stop() final { sync.stop(); }

  void get_metrics(const srsran::srsran_rat_t& rat, phy_metrics_t* m) final{};

private:
  srslog::basic_logger& logger;

  nr::worker_pool       workers;
  phy_common            common;
  prach                 prach_buffer;
  nr::sync_sa           sync;

  srsran::phy_cfg_nr_t config_nr = {};
  phy_args_nr_t        args      = {};

  srsran::radio_interface_phy* radio = nullptr;

  std::mutex              config_mutex;
  std::condition_variable config_cond;
  std::atomic<bool>       is_configured = {false};

  const static int SF_RECV_THREAD_PRIO = 0;
  const static int WORKERS_THREAD_PRIO = 2;
};

} // namespace srsue
#endif // SRSUE_PHY_NR_SA_H
