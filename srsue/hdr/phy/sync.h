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

#ifndef SRSUE_PHCH_RECV_H
#define SRSUE_PHCH_RECV_H

#include <condition_variable>
#include <map>
#include <mutex>
#include <pthread.h>

#include "phy_common.h"
#include "prach.h"
#include "scell/intra_measure.h"
#include "scell/scell_sync.h"
#include "search.h"
#include "sfn_sync.h"
#include "srsran/common/thread_pool.h"
#include "srsran/common/threads.h"
#include "srsran/common/tti_sync_cv.h"
#include "srsran/interfaces/radio_interfaces.h"
#include "srsran/phy/channel/channel.h"
#include "srsran/srsran.h"
#include "srsue/hdr/phy/lte/worker_pool.h"
#include "srsue/hdr/phy/nr/worker_pool.h"
#include "sync_state.h"

namespace srsue {

typedef _Complex float cf_t;

class sync : public srsran::thread,
             public rsrp_insync_itf,
             public search_callback,
             public scell::sync_callback,
             public scell::intra_measure::meas_itf
{
public:
  sync(srslog::basic_logger& phy_logger, srslog::basic_logger& phy_lib_logger) :
    thread("SYNC"),
    search_p(phy_logger),
    sfn_p(phy_logger),
    phy_logger(phy_logger),
    phy_lib_logger(phy_lib_logger),
    sf_buffer(sync_nof_rx_subframes),
    dummy_buffer(sync_nof_rx_subframes){};
  ~sync();

  void init(srsran::radio_interface_phy* radio_,
            stack_interface_phy_lte*     _stack,
            prach*                       prach_buffer,
            lte::worker_pool*            _lte_workers_pool,
            nr::worker_pool*             _nr_workers_pool,
            phy_common*                  _worker_com,
            uint32_t                     prio,
            int                          sync_cpu_affinity = -1);
  void stop();
  void radio_overflow();

  // RRC interface for controling the SYNC state
  bool                                     cell_search_init();
  rrc_interface_phy_lte::cell_search_ret_t cell_search_start(phy_cell_t* cell);
  bool                                     cell_select_init(phy_cell_t cell);
  bool                                     cell_select_start(phy_cell_t cell);
  bool                                     cell_is_camping();

  // RRC interface for controlling the neighbour cell measurement
  void set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci);
  void set_inter_frequency_measurement(uint32_t cc_idx, uint32_t earfcn_, srsran_cell_t cell_);
  void meas_stop();

  // from chest_feedback_itf
  void in_sync() final;
  void out_of_sync() final;
  void set_cfo(float cfo) final;

  void     get_current_cell(srsran_cell_t* cell, uint32_t* earfcn = nullptr);
  uint32_t get_current_tti();

  // From UE configuration
  void set_agc_enable(bool enable);
  void force_freq(float dl_freq, float ul_freq);

  // Other functions
  void set_rx_gain(float gain) override;
  int  radio_recv_fnc(srsran::rf_buffer_t&, srsran_timestamp_t* rx_time) override;

  srsran::radio_interface_phy* get_radio() override { return radio_h; }

  /**
   * Sets secondary serving cell for synchronization purposes
   * @param cc_idx component carrier index
   * @param _cell Cell information
   */
  void scell_sync_set(uint32_t cc_idx, const srsran_cell_t& _cell);

  /**
   * Stops all secondary serving cell synchronization
   */
  void scell_sync_stop();

  /**
   * Implements Secondary Serving cell feedback
   * @param ch Feedback channel
   * @param offset Number of samples to offset
   */
  void set_rx_channel_offset(uint32_t ch, int32_t offset) override { radio_h->set_channel_rx_offset(ch, offset); }

  // Interface from scell::intra_measure for providing neighbour cell measurements
  void cell_meas_reset(uint32_t cc_idx) override;
  void new_cell_meas(uint32_t cc_idx, const std::vector<phy_meas_t>& meas) override;

private:
  void reset();
  void radio_error();
  void set_ue_sync_opts(srsran_ue_sync_t* q, float cfo) override;

  /**
   * Search for a cell in the current frequency and go to IDLE.
   * The function search_p.run() will not return until the search finishes
   */
  void run_cell_search_state();

  /**
   * SFN synchronization using MIB. run_subframe() receives and processes 1 subframe
   * and returns
   */
  void run_sfn_sync_state();

  /**
   * Cell camping state. Calls the PHCH workers to process subframes and maintains cell synchronization
   */
  void run_camping_state();

  /**
   * Receives and discards received samples. Does not maintain synchronization
   */
  void run_idle_state();

  /**
   * MAIN THREAD
   *
   * The main thread process the SYNC state machine. Every state except IDLE must have exclusive access to
   * all variables. If any change of cell configuration must be done, the thread must be in IDLE.
   *
   * On each state except campling, 1 function is called and the thread jumps to the next state based on the output.
   *
   * It has 3 states: Cell search, SFN synchronization, initial measurement and camping.
   * - CELL_SEARCH:   Initial Cell id and MIB acquisition. Uses 1.92 MHz sampling rate
   * - CELL_SYNC:     Full sampling rate, uses MIB to obtain SFN. When SFN is obtained, moves to CELL_CAMP
   * - CELL_CAMP:     Cell camping state. Calls the PHCH workers to process subframes and maintains cell
   * synchronization.
   * - IDLE:          Receives and discards received samples. Does not maintain synchronization.
   *
   */
  void run_thread() final;

  /**
   * Helper method, executed when the UE is camping and in-sync
   * @param lte_worker Selected LTE worker for the current TTI
   * @param nr_worker Selected NR worker for the current TTI
   * @param sync_buffer Sub-frame buffer for the current TTI
   */
  void
  run_camping_in_sync_state(lte::sf_worker* lte_worker, nr::sf_worker* nr_worker, srsran::rf_buffer_t& sync_buffer);

  /**
   * Helper method, executed in a TTI basis for signaling to the stack a new TTI execution
   *
   * The PHY shall not call run_stack_tti when the PHY has reserved a worker.
   *
   * Since the sync thread has reserved a worker in camping state, the PHY shall not call the stack in this state.
   * Otherwise, there is a risk that the stack tries to reserve the same worker for configuration.
   */
  void run_stack_tti();

  float get_tx_cfo();

  void set_sampling_rate();
  bool set_frequency();
  bool set_cell(float cfo);

  bool running     = false;
  bool is_overflow = false;

  srsran::rf_timestamp_t last_rx_time;
  bool                   forced_rx_time_init = true; // Rx time sync after first receive from radio

  // Objects for internal use
  search                                              search_p;
  sfn_sync                                            sfn_p;
  std::vector<std::unique_ptr<scell::intra_measure> > intra_freq_meas;

  // Pointers to other classes
  stack_interface_phy_lte*     stack = nullptr;
  srslog::basic_logger&        phy_logger;
  srslog::basic_logger&        phy_lib_logger;
  lte::worker_pool*            lte_worker_pool  = nullptr;
  nr::worker_pool*             nr_worker_pool   = nullptr;
  srsran::radio_interface_phy* radio_h          = nullptr;
  phy_common*                  worker_com       = nullptr;
  prach*                       prach_buffer     = nullptr;
  srsran::channel_ptr          channel_emulator = nullptr;

  // PRACH state
  uint32_t prach_nof_sf = 0;
  uint32_t prach_sf_cnt = 0;
  cf_t*    prach_ptr    = nullptr;
  float    prach_power  = 0;

  // Object for synchronization of the primary cell
  srsran_ue_sync_t ue_sync = {};

  // Object for synchronization secondary serving cells
  std::map<uint32_t, std::unique_ptr<scell::sync> > scell_sync;

  // Buffer for primary and secondary cell samples
  const static uint32_t sync_nof_rx_subframes = 5;
  srsran::rf_buffer_t   sf_buffer             = {};
  srsran::rf_buffer_t   dummy_buffer;

  // Sync metrics
  sync_metrics_t metrics = {};

  // in-sync / out-of-sync counters
  uint32_t out_of_sync_cnt = 0;
  uint32_t in_sync_cnt     = 0;

  std::mutex rrc_mutex;
  enum {
    PROC_IDLE = 0,
    PROC_SELECT_START,
    PROC_SELECT_RUNNING,
    PROC_SEARCH_START,
    PROC_SEARCH_RUNNING
  } rrc_proc_state = PROC_IDLE;

  sync_state phy_state;

  search::ret_code cell_search_ret = search::CELL_NOT_FOUND;

  // Sampling rate mode (find is 1.96 MHz, camp is the full cell BW)
  enum { SRATE_NONE = 0, SRATE_FIND, SRATE_CAMP } srate_mode = SRATE_NONE;
  float current_srate                                        = 0;

  // This is the primary cell
  srsran_cell_t                               cell                   = {};
  bool                                        force_camping_sfn_sync = false;
  uint32_t                                    tti                    = 0;
  srsran_timestamp_t                          stack_tti_ts_new       = {};
  srsran_timestamp_t                          stack_tti_ts           = {};
  std::array<uint8_t, SRSRAN_BCH_PAYLOAD_LEN> mib                    = {};

  uint32_t nof_rf_channels         = 0;
  float    ul_dl_factor            = NAN;
  int      current_earfcn          = 0;
  uint32_t cellsearch_earfcn_index = 0;

  float dl_freq = -1;
  float ul_freq = -1;

  const static int MIN_TTI_JUMP = 1;    // Time gap reported to stack after receiving subframe
  const static int MAX_TTI_JUMP = 1000; // Maximum time gap tolerance in RF stream metadata

  const uint8_t SYNC_CC_IDX = 0; ///< From the sync POV, the CC idx is always the first
};

} // namespace srsue

#endif // SRSUE_PHCH_RECV_H
