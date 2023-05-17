/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSUE_SYNC_NR_SA_H
#define SRSUE_SYNC_NR_SA_H

#include "cell_search.h"
#include "slot_sync.h"
#include "srsran/common/threads.h"
#include "srsran/interfaces/radio_interfaces.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsran/radio/rf_buffer.h"
#include "srsran/radio/rf_timestamp.h"
#include "srsran/srslog/logger.h"
#include "srsran/srsran.h"
#include "srsue/hdr/phy/sync_state.h"
#include "worker_pool.h"
#include <condition_variable>
#include <mutex>
#include <srsran/common/tti_sempahore.h>

namespace srsue {
namespace nr {

/**
 * @brief NR Standalone synchronization class
 */
class sync_sa : public srsran::thread, public srsran::phy_common_interface
{
public:
  struct args_t {
    double                      srate_hz        = 61.44e6;
    srsran_subcarrier_spacing_t ssb_min_scs     = srsran_subcarrier_spacing_15kHz;
    uint32_t                    nof_rx_channels = 1;
    bool                        disable_cfo     = false;
    float                       pbch_dmrs_thr   = 0.0f; ///< PBCH DMRS correlation detection threshold (0 means auto)
    float                       cfo_alpha       = 0.0f; ///< CFO averaging alpha (0 means auto)
    int                         thread_priority = 1;

    cell_search::args_t get_cell_search() const
    {
      cell_search::args_t ret = {};
      ret.max_srate_hz        = srate_hz;
      return ret;
    }

    slot_sync::args_t get_slot_sync() const
    {
      slot_sync::args_t ret = {};
      ret.max_srate_hz      = srate_hz;
      ret.nof_rx_channels   = nof_rx_channels;
      ret.ssb_min_scs       = ssb_min_scs;

      return ret;
    }
  };

  sync_sa(srslog::basic_logger& logger, worker_pool& workers_);
  ~sync_sa();

  bool                init(const args_t& args_, stack_interface_phy_nr* stack_, srsran::radio_interface_phy* radio_);
  bool                reset();
  void                stop();
  sync_state::state_t get_state();

  // The following methods control the SYNC state machine
  void                                       cell_go_idle();
  cell_search::ret_t                         cell_search_run(const cell_search::cfg_t& cfg);
  rrc_interface_phy_nr::cell_select_result_t cell_select_run(const phy_interface_rrc_nr::cell_select_args_t& req);

  void worker_end(const worker_context_t& w_ctx, const bool& tx_enable, srsran::rf_buffer_t& buffer) override;

  void add_ta_cmd_rar(uint32_t tti, uint32_t ta_cmd);
  void add_ta_cmd_new(uint32_t tti, uint32_t ta_cmd);
  void add_ta_offset(uint32_t ta_offset);

private:
  stack_interface_phy_nr*      stack = nullptr; ///< Stand-Alone RRC interface
  srsran::radio_interface_phy* radio = nullptr; ///< Radio object
  srslog::basic_logger&        logger;          ///< General PHY logger
  worker_pool&                 workers;

  // FSM that manages RRC commands for cell search/select/sync procedures
  std::mutex rrc_mutex;
  enum { PROC_IDLE = 0, PROC_SELECT_RUNNING, PROC_SEARCH_RUNNING } rrc_proc_state = PROC_IDLE;
  sync_state phy_state;

  std::atomic<bool>            running   = {false};
  cf_t*                        rx_buffer = nullptr;
  double                       srate_hz  = 0; ///< Sampling rate in Hz
  uint32_t                     slot_sz   = 0; ///< Subframe size (1-ms)
  uint32_t                     tti       = 0;
  srsran::tti_semaphore<void*> tti_semaphore;
  srsran::rf_timestamp_t       last_rx_time;
  std::atomic<bool>            is_pending_tx_end      = {false};
  uint32_t                     cell_search_nof_trials = 0;
  const static uint32_t        cell_search_max_trials = 100;
  uint32_t                     sfn_sync_nof_trials    = 0;
  const static uint32_t        sfn_sync_max_trials    = 100;

  cell_search::ret_t cs_ret;
  cell_search        searcher;
  slot_sync          slot_synchronizer;

  // Time Aligment Controller, internal thread safe
  ta_control ta;

  // FSM States
  bool wait_idle();
  void run_state_idle();
  void run_state_cell_search();
  void run_state_sfn_sync();
  void run_state_cell_camping();

  int  radio_recv_fnc(srsran::rf_buffer_t& data, srsran_timestamp_t* rx_time);
  void run_stack_tti();
  void run_thread() override;
};

} // namespace nr
} // namespace srsue

#endif // SRSUE_SYNC_NR_SA_H
