/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSUE_PHY_H
#define SRSUE_PHY_H

#include "phy_common.h"
#include "phy_metrics.h"
#include "prach.h"
#include "sf_worker.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/threads.h"
#include "srslte/common/trace.h"
#include "srslte/interfaces/phy_interface_types.h"
#include "srslte/interfaces/radio_interfaces.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/radio/radio.h"
#include "srslte/srslte.h"
#include "srsue/hdr/phy/ue_lte_phy_base.h"
#include "sync.h"

namespace srsue {

typedef _Complex float cf_t;

class phy_cmd_proc : public srslte::thread
{
public:
  phy_cmd_proc() : thread("PHY_CMD") { start(); }

  ~phy_cmd_proc() { stop(); }

  void add_cmd(std::function<void(void)> cmd) { cmd_queue.push(cmd); }

  void stop()
  {
    if (running) {
      add_cmd([this]() { running = false; });
      wait_thread_finish();
    }
  }

private:
  void run_thread()
  {
    std::function<void(void)> cmd;
    while (running) {
      cmd = cmd_queue.wait_pop();
      cmd();
    }
  }
  bool running = true;
  // Queue for commands
  srslte::block_queue<std::function<void(void)> > cmd_queue;
};

class phy final : public ue_lte_phy_base, public srslte::thread
{
public:
  explicit phy(srslte::logger* logger_) : logger(logger_), workers_pool(MAX_WORKERS), common(), thread("PHY"){};
  ~phy() final { stop(); }

  // Init defined in base class
  int init(const phy_args_t& args_) final;

  // Init for LTE PHYs
  int init(const phy_args_t& args_, stack_interface_phy_lte* stack_, srslte::radio_interface_phy* radio_) final;

  void stop() final;

  void wait_initialize() final;
  bool is_initiated();

  void get_metrics(phy_metrics_t* m) final;
  void srslte_phy_logger(phy_logger_level_t log_level, char* str);

  void enable_pregen_signals(bool enable) final;

  void radio_overflow() final;
  void radio_failure() final;

  /********** RRC INTERFACE ********************/

  bool cell_search() final;
  bool cell_select(phy_cell_t cell) final;

  // Sets the new PHY configuration for the given CC. The configuration is applied in the background. The notify()
  // function will be called when the reconfiguration is completed. Unless the PRACH configuration has changed, the
  // reconfiguration will not take more than 3 ms
  bool set_config(srslte::phy_cfg_t config, uint32_t cc_idx) final;

  // Adds or modifies the cell configuration for a given CC. If the EARFCN has changed w.r.t. the previous value, or if
  // the cell is new, this function might take a few hundred ms to complete, depending on the radio
  bool set_scell(srslte_cell_t cell_info, uint32_t cc_idx, uint32_t earfcn) final;

  // Applies a TDD configuration in the background. This function will take less than 3 ms to execute.
  void set_config_tdd(srslte_tdd_config_t& tdd_config) final;

  // Todo
  void set_config_mbsfn_sib2(srslte::mbsfn_sf_cfg_t* cfg_list, uint32_t nof_cfgs) final;
  void set_config_mbsfn_sib13(const srslte::sib13_t& sib13) final;
  void set_config_mbsfn_mcch(const srslte::mcch_msg_t& mcch) final;

  // This function applies the new configuration immediately
  void set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci) final;

  // This function applies the new configuration immediately
  void meas_stop() final;

  // also MAC interface
  bool cell_is_camping() final;

  /********** MAC INTERFACE ********************/
  // Precomputes sequences for the given RNTI. The computation is done in the background.
  void set_crnti(uint16_t rnti) final;

  /* Transmits PRACH in the next opportunity */
  void         prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm, float ta_base_sec) final;
  prach_info_t prach_get_info() final;

  /* Indicates the transmission of a SR signal in the next opportunity */
  void sr_send() final;
  int  sr_last_tx_tti() final;

  // Time advance commands
  void set_timeadv_rar(uint32_t ta_cmd) final;
  void set_timeadv(uint32_t ta_cmd) final;

  /* Activate / Disactivate SCell*/
  void set_activation_deactivation_scell(uint32_t ta_cmd) final;

  /* Sets RAR dci payload */
  void set_rar_grant(uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN], uint16_t rnti) final;

  /*Set MAC->PHY MCH period  stopping point*/
  void set_mch_period_stop(uint32_t stop) final;

  float get_phr() final;
  float get_pathloss_db() final;

  uint32_t get_current_tti() final;

  void start_plot() final;

  const static int MAX_WORKERS     = 4;
  const static int DEFAULT_WORKERS = 4;

  std::string get_type() final { return "lte_soft"; }

private:
  void run_thread() final;
  void configure_prach_params();
  void reset();

  std::mutex              config_mutex;
  std::condition_variable config_cond;
  bool                    is_configured = false;
  uint32_t                nof_workers   = 0;

  const static int SF_RECV_THREAD_PRIO = 0;
  const static int WORKERS_THREAD_PRIO = 2;

  srslte::radio_interface_phy*                      radio = nullptr;
  std::vector<std::unique_ptr<srslte::log_filter> > log_vec;
  srslte::logger*                                   logger = nullptr;

  srslte::log*                    log_h         = nullptr;
  srslte::log*                    log_phy_lib_h = nullptr;
  srsue::stack_interface_phy_lte* stack         = nullptr;

  srslte::thread_pool                      workers_pool;
  std::vector<std::unique_ptr<sf_worker> > workers;
  phy_common                               common;
  sync                                     sfsync;
  prach                                    prach_buffer;

  srslte_prach_cfg_t  prach_cfg  = {};
  srslte_tdd_config_t tdd_config = {};

  srslte::phy_cfg_t config = {};
  phy_args_t        args   = {};

  // Since cell_search/cell_select operations take a lot of time, we use another queue to process the other commands
  // in parallel and avoid accumulating in the queue
  phy_cmd_proc cmd_worker_cell, cmd_worker;

  // Tracks the current selected cell (last call to cell_select)
  srslte_cell_t selected_cell = {};

  static void set_default_args(phy_args_t& args);
  bool        check_args(const phy_args_t& args);
};

} // namespace srsue

#endif // SRSUE_PHY_H
