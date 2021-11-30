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

#include "phy_common.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsue/hdr/phy/nr/sync_sa.h"
#include "srsue/hdr/phy/ue_phy_base.h"

namespace srsue {

/**
 * @brief NR Standalone PHY
 */
class phy_nr_sa final : public ue_phy_base, public phy_interface_stack_nr
{
public:
  phy_nr_sa(const char* logname);

  int  init(const phy_args_nr_t& args_, stack_interface_phy_nr* stack_, srsran::radio_interface_phy* radio_);
  void wait_initialize() final;
  bool is_initialized() final;
  void stop() final;
  void reset_nr() final;

  void start_plot() final {}

  std::string get_type() final { return "nr_soft"; }

  int  set_rar_grant(uint32_t                                       rar_slot_idx,
                     std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS> packed_ul_grant,
                     uint16_t                                       rnti,
                     srsran_rnti_type_t                             rnti_type) final;
  void send_prach(const uint32_t prach_occasion,
                  const int      preamble_index,
                  const float    preamble_received_target_power,
                  const float    ta_base_sec) final;
  void set_earfcn(std::vector<uint32_t> earfcns);
  bool has_valid_sr_resource(uint32_t sr_id) final;
  void clear_pending_grants() final;
  bool set_config(const srsran::phy_cfg_nr_t& cfg) final;

  phy_nr_state_t get_state() final;
  bool           start_cell_search(const cell_search_args_t& req) final;
  bool           start_cell_select(const cell_select_args_t& req) final;

  void get_metrics(const srsran::srsran_rat_t& rat, phy_metrics_t* m) final{};
  void srsran_phy_logger(phy_logger_level_t log_level, char* str);

private:
  srslog::basic_logger& logger;
  srslog::basic_logger& logger_phy_lib;

  nr::worker_pool       workers;
  phy_common            common;
  nr::sync_sa           sync;

  srsran::phy_cfg_nr_t config_nr     = {};
  phy_args_nr_t        args          = {};
  srsran_carrier_nr_t  selected_cell = {};

  srsran::radio_interface_phy* radio = nullptr;
  stack_interface_phy_nr*      stack = nullptr;

  std::atomic<bool>       is_configured = {false};

  // Since cell_search/cell_select operations take a lot of time, we use another queue to process the other commands
  // in parallel and avoid accumulating in the queue
  phy_cmd_proc cmd_worker_cell, cmd_worker;

  // Run initialization functions in the background
  void        init_background();
  std::thread init_thread;

  const static int SF_RECV_THREAD_PRIO = 0;
  const static int WORKERS_THREAD_PRIO = 2;
  const static int MAX_WORKERS         = 4;
  const static int DEFAULT_WORKERS     = 4;

  static void set_default_args(phy_args_nr_t& args);
  bool        check_args(const phy_args_nr_t& args);
};

} // namespace srsue
#endif // SRSUE_PHY_NR_SA_H
