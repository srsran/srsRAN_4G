/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSUE_PHCH_RECV_H
#define SRSUE_PHCH_RECV_H

#include <condition_variable>
#include <map>
#include <mutex>
#include <pthread.h>
#include <srslte/phy/channel/channel.h>

#include "phy_common.h"
#include "prach.h"
#include "sf_worker.h"
#include "srslte/common/log.h"
#include "srslte/common/thread_pool.h"
#include "srslte/common/threads.h"
#include "srslte/common/tti_sync_cv.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/srslte.h"
#include "srsue/hdr/phy/scell/async_scell_recv.h"

#include <srsue/hdr/phy/scell/intra_measure.h>

namespace srsue {

typedef _Complex float cf_t;

class sync : public thread, public chest_feedback_itf
{
public:
  sync() : thread("SYNC"){};
  ~sync();

  void init(srslte::radio_interface_phy* radio_,
            stack_interface_phy_lte*     _stack,
            prach*                       prach_buffer,
            srslte::thread_pool*         _workers_pool,
            phy_common*                  _worker_com,
            srslte::log*                 _log_h,
            srslte::log*                 _log_phy_lib_h,
            scell::async_recv_vector*    scell_sync_,
            uint32_t                     prio,
            int                          sync_cpu_affinity = -1);
  void stop();
  void radio_overflow();

  // RRC interface for controling the SYNC state
  phy_interface_rrc_lte::cell_search_ret_t cell_search(phy_interface_rrc_lte::phy_cell_t* cell);
  bool                                     cell_select(phy_interface_rrc_lte::phy_cell_t* cell);
  bool                                     cell_is_camping();

  // RRC interface for controlling the neighbour cell measurement
  void meas_reset();
  int  meas_start(uint32_t earfcn, int pci);
  int  meas_stop(uint32_t earfcn, int pci);

  // from chest_feedback_itf
  void in_sync() final;
  void out_of_sync() final;
  void set_cfo(float cfo) final;

  void     set_time_adv_sec(float time_adv_sec);
  void     get_current_cell(srslte_cell_t* cell, uint32_t* earfcn = nullptr);
  uint32_t get_current_tti();

  // From UE configuration
  void set_agc_enable(bool enable);
  void set_earfcn(std::vector<uint32_t> earfcn);
  void force_freq(float dl_freq, float ul_freq);

  // Other functions
  double set_rx_gain(double gain);
  int    radio_recv_fnc(cf_t* data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t* rx_time);

private:
  // Class to run cell search
  class search
  {
  public:
    typedef enum { CELL_NOT_FOUND, CELL_FOUND, ERROR, TIMEOUT } ret_code;

    ~search();
    void     init(cf_t* buffer[SRSLTE_MAX_PORTS], srslte::log* log_h, uint32_t nof_rx_antennas, sync* parent);
    void     reset();
    float    get_last_cfo();
    void     set_agc_enable(bool enable);
    ret_code run(srslte_cell_t* cell, std::array<uint8_t, SRSLTE_BCH_PAYLOAD_LEN>& bch_payload);

  private:
    sync*                  p                        = nullptr;
    srslte::log*           log_h                    = nullptr;
    cf_t*                  buffer[SRSLTE_MAX_PORTS] = {};
    srslte_ue_cellsearch_t cs                       = {};
    srslte_ue_mib_sync_t   ue_mib_sync              = {};
    int                    force_N_id_2             = 0;
  };

  // Class to synchronize system frame number
  class sfn_sync
  {
  public:
    typedef enum { IDLE, SFN_FOUND, SFX0_FOUND, SFN_NOFOUND, ERROR } ret_code;
    sfn_sync() = default;
    ~sfn_sync();
    void     init(srslte_ue_sync_t* ue_sync,
                  cf_t*             buffer[SRSLTE_MAX_PORTS],
                  srslte::log*      log_h,
                  uint32_t          nof_subframes = SFN_SYNC_NOF_SUBFRAMES);
    void     reset();
    bool     set_cell(srslte_cell_t cell);
    ret_code run_subframe(srslte_cell_t*                               cell,
                          uint32_t*                                    tti_cnt,
                          std::array<uint8_t, SRSLTE_BCH_PAYLOAD_LEN>& bch_payload,
                          bool                                         sfidx_only = false);
    ret_code decode_mib(srslte_cell_t*                               cell,
                        uint32_t*                                    tti_cnt,
                        cf_t*                                        ext_buffer[SRSLTE_MAX_PORTS],
                        std::array<uint8_t, SRSLTE_BCH_PAYLOAD_LEN>& bch_payload,
                        bool                                         sfidx_only = false);

  private:
    const static int SFN_SYNC_NOF_SUBFRAMES = 100;

    uint32_t          cnt                      = 0;
    uint32_t          timeout                  = 0;
    srslte::log*      log_h                    = nullptr;
    srslte_ue_sync_t* ue_sync                  = nullptr;
    cf_t*             buffer[SRSLTE_MAX_PORTS] = {};
    srslte_ue_mib_t   ue_mib                   = {};
  };

  /* TODO: Intra-freq measurements can be improved by capturing 200 ms length signal and run cell search +
   * measurements offline using sync object and finding multiple cells for each N_id_2
   */

  std::vector<uint32_t> earfcn;

  void  reset();
  void  radio_error();
  void  set_ue_sync_opts(srslte_ue_sync_t* q, float cfo);
  void  run_thread() final;
  float get_tx_cfo();

  void set_sampling_rate();
  bool set_frequency();
  bool set_cell();

  bool radio_is_overflow     = false;
  bool radio_overflow_return = false;
  bool running               = false;

  // Objects for internal use
  search               search_p;
  sfn_sync             sfn_p;
  scell::intra_measure intra_freq_meas;

  uint32_t current_sflen                        = 0;
  int      next_offset                          = 0;  // Sample offset triggered by Time aligment commands
  int      next_radio_offset[SRSLTE_MAX_RADIOS] = {}; // Sample offset triggered by SFO compensation

  // Pointers to other classes
  stack_interface_phy_lte*     stack            = nullptr;
  srslte::log*                 log_h            = nullptr;
  srslte::log*                 log_phy_lib_h    = nullptr;
  srslte::thread_pool*         workers_pool     = nullptr;
  srslte::radio_interface_phy* radio_h          = nullptr;
  phy_common*                  worker_com       = nullptr;
  prach*                       prach_buffer     = nullptr;
  scell::async_recv_vector*    scell_sync       = nullptr;
  srslte::channel_ptr          channel_emulator = nullptr;

  // Object for synchronization of the primary cell
  srslte_ue_sync_t ue_sync = {};

  // Buffer for primary and secondary cell samples
  cf_t* sf_buffer[SRSLTE_MAX_RADIOS][SRSLTE_MAX_PORTS] = {};

  // Sync metrics
  sync_metrics_t metrics = {};

  // in-sync / out-of-sync counters
  uint32_t out_of_sync_cnt = 0;
  uint32_t in_sync_cnt     = 0;

  const static uint32_t NOF_OUT_OF_SYNC_SF = 20;
  const static uint32_t NOF_IN_SYNC_SF     = 10;

  // State machine for SYNC thread
  class sync_state
  {
  public:
    typedef enum {
      IDLE = 0,
      CELL_SEARCH,
      SFN_SYNC,
      CAMPING,
    } state_t;

    /* Run_state is called by the main thread at the start of each loop. It updates the state
     * and returns the current state
     */
    state_t run_state()
    {
      std::lock_guard<std::mutex> lock(inside);
      cur_state = next_state;
      if (state_setting) {
        state_setting = false;
        state_running = true;
      }
      cvar.notify_all();
      return cur_state;
    }

    // Called by the main thread at the end of each state to indicate it has finished.
    void state_exit(bool exit_ok = true)
    {
      std::lock_guard<std::mutex> lock(inside);
      if (cur_state == SFN_SYNC && exit_ok == true) {
        next_state = CAMPING;
      } else {
        next_state = IDLE;
      }
      state_running = false;
      cvar.notify_all();
    }
    void force_sfn_sync()
    {
      std::lock_guard<std::mutex> lock(inside);
      next_state = SFN_SYNC;
    }

    /* Functions to be called from outside the STM thread to instruct the STM to switch state.
     * The functions change the state and wait until it has changed it.
     *
     * These functions are mutexed and only 1 can be called at a time
     */
    void go_idle()
    {
      std::lock_guard<std::mutex> lock(outside);
      go_state(IDLE);
    }
    void run_cell_search()
    {
      std::lock_guard<std::mutex> lock(outside);
      go_state(CELL_SEARCH);
      wait_state_run();
      wait_state_next();
    }
    void run_sfn_sync()
    {
      std::lock_guard<std::mutex> lock(outside);
      go_state(SFN_SYNC);
      wait_state_run();
      wait_state_next();
    }

    /* Helpers below this */
    bool is_idle() { return cur_state == IDLE; }
    bool is_camping() { return cur_state == CAMPING; }

    const char* to_string()
    {
      switch (cur_state) {
        case IDLE:
          return "IDLE";
        case CELL_SEARCH:
          return "SEARCH";
        case SFN_SYNC:
          return "SYNC";
        case CAMPING:
          return "CAMPING";
        default:
          return "UNKNOWN";
      }
    }

    sync_state() = default;

  private:
    void go_state(state_t s)
    {
      std::unique_lock<std::mutex> ul(inside);
      next_state    = s;
      state_setting = true;
      while (state_setting) {
        cvar.wait(ul);
      }
    }

    /* Waits until there is a call to set_state() and then run_state(). Returns when run_state() returns */
    void wait_state_run()
    {
      std::unique_lock<std::mutex> ul(inside);
      while (state_running) {
        cvar.wait(ul);
      }
    }
    void wait_state_next()
    {
      std::unique_lock<std::mutex> ul(inside);
      while (cur_state != next_state) {
        cvar.wait(ul);
      }
    }

    bool                    state_running = false;
    bool                    state_setting = false;
    state_t                 cur_state     = IDLE;
    state_t                 next_state    = IDLE;
    std::mutex              inside;
    std::mutex              outside;
    std::condition_variable cvar;
  };

  std::mutex rrc_mutex;

  sync_state phy_state;

  search::ret_code cell_search_ret = search::CELL_NOT_FOUND;

  // Sampling rate mode (find is 1.96 MHz, camp is the full cell BW)
  enum { SRATE_NONE = 0, SRATE_FIND, SRATE_CAMP } srate_mode = SRATE_NONE;
  float current_srate                                        = 0;

  // This is the primary cell
  srslte_cell_t cell              = {};
  bool          started           = false;
  float         time_adv_sec      = 0;
  float         next_time_adv_sec = 0;
  uint32_t      tti               = 0;
  std::array<uint8_t, SRSLTE_BCH_PAYLOAD_LEN> mib;

  uint32_t tx_worker_cnt = 0;
  uint32_t nof_workers   = 0;

  float    ul_dl_factor            = NAN;
  int      current_earfcn          = 0;
  uint32_t cellsearch_earfcn_index = 0;

  float dl_freq = -1;
  float ul_freq = -1;
};

} // namespace srsue

#endif // SRSUE_PHCH_RECV_H
