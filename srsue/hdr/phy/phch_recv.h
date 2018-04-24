/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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

#include <map>
#include <pthread.h>

#include "srslte/srslte.h"
#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/common/thread_pool.h"
#include "srslte/common/tti_sync_cv.h"
#include "srslte/radio/radio_multi.h"
#include "prach.h"
#include "phch_worker.h"
#include "phch_common.h"
#include "srslte/interfaces/ue_interfaces.h"

namespace srsue {
    
typedef _Complex float cf_t;


class phch_recv : public thread, public chest_feedback_itf
{
public:
  phch_recv();
  ~phch_recv();

  void init(srslte::radio_multi* radio_handler, mac_interface_phy *mac,rrc_interface_phy *rrc,
            prach *prach_buffer, srslte::thread_pool *_workers_pool,
            phch_common *_worker_com, srslte::log* _log_h, srslte::log *_log_phy_lib_h, uint32_t nof_rx_antennas, uint32_t prio, int sync_cpu_affinity = -1);
  void stop();
  void radio_overflow();

  // RRC interface for controling the SYNC state
  phy_interface_rrc::cell_search_ret_t cell_search(phy_interface_rrc::phy_cell_t *cell);
  bool    cell_select(phy_interface_rrc::phy_cell_t *cell);
  bool    cell_is_camping();

  // RRC interface for controlling the neighbour cell measurement
  void    meas_reset();
  int     meas_start(uint32_t earfcn, int pci);
  int     meas_stop(uint32_t earfcn, int pci);

  // from chest_feedback_itf
  void    in_sync();
  void    out_of_sync();
  void    set_cfo(float cfo);

  void    set_time_adv_sec(float time_adv_sec);
  void    get_current_cell(srslte_cell_t *cell, uint32_t *earfcn = NULL);
  uint32_t get_current_tti();

  // From UE configuration
  void    set_agc_enable(bool enable);
  void    set_earfcn(std::vector<uint32_t> earfcn);
  void    force_freq(float dl_freq, float ul_freq);

  // Other functions
  const static int MUTEX_X_WORKER = 4;
  double set_rx_gain(double gain);
  int radio_recv_fnc(cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *rx_time);
  int scell_recv_fnc(cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *rx_time);

private:

  // Class to run cell search
  class search {
  public:
    typedef enum {CELL_NOT_FOUND, CELL_FOUND, ERROR, TIMEOUT} ret_code;

    ~search();
    void     init(cf_t *buffer[SRSLTE_MAX_PORTS], srslte::log *log_h, uint32_t nof_rx_antennas, phch_recv *parent);
    void     reset();
    float    get_last_cfo();
    void     set_agc_enable(bool enable);
    ret_code run(srslte_cell_t *cell);

  private:
    phch_recv              *p;
    srslte::log            *log_h;
    cf_t                   *buffer[SRSLTE_MAX_PORTS];
    srslte_ue_cellsearch_t  cs;
    srslte_ue_mib_sync_t    ue_mib_sync;
    int                     force_N_id_2;
  };

  // Class to synchronize system frame number
  class sfn_sync {
  public:
    typedef enum {IDLE, SFN_FOUND, SFX0_FOUND, SFN_NOFOUND, ERROR} ret_code;

    ~sfn_sync();
    void     init(srslte_ue_sync_t *ue_sync, cf_t *buffer[SRSLTE_MAX_PORTS], srslte::log *log_h, uint32_t nof_subframes = SFN_SYNC_NOF_SUBFRAMES);
    void     reset();
    bool     set_cell(srslte_cell_t cell);
    ret_code run_subframe(srslte_cell_t *cell, uint32_t *tti_cnt, bool sfidx_only = false);

  private:
    const static int SFN_SYNC_NOF_SUBFRAMES = 100;

    uint32_t          cnt;
    uint32_t          timeout;
    srslte::log      *log_h;
    srslte_ue_sync_t *ue_sync;
    cf_t             *buffer[SRSLTE_MAX_PORTS];
    srslte_ue_mib_t   ue_mib;
  };

  // Class to perform cell measurements
  class measure {

    // TODO: This class could early stop once the variance between the last N measurements is below 3GPP requirements

  public:
    typedef enum {IDLE, MEASURE_OK, ERROR} ret_code;

    ~measure();
    void      init(cf_t *buffer[SRSLTE_MAX_PORTS], srslte::log *log_h,
                   uint32_t nof_rx_antennas, uint32_t nof_subframes = RSRP_MEASURE_NOF_FRAMES);
    void      reset();
    void      set_cell(srslte_cell_t cell);
    ret_code  run_subframe(uint32_t sf_idx);
    ret_code  run_subframe_sync(srslte_ue_sync_t *ue_sync, uint32_t sf_idx);
    ret_code  run_multiple_subframes(cf_t *buffer, uint32_t offset, uint32_t sf_idx, uint32_t nof_sf);
    float     rssi();
    float     rsrp();
    float     rsrq();
    float     snr();
    uint32_t  frame_st_idx();
    void      set_rx_gain_offset(float rx_gain_offset);
  private:
    srslte::log      *log_h;
    srslte_ue_dl_t    ue_dl;
    cf_t              *buffer[SRSLTE_MAX_PORTS];
    uint32_t cnt;
    uint32_t nof_subframes;
    uint32_t current_prb;
    float rx_gain_offset;
    float mean_rsrp, mean_rsrq, mean_snr, mean_rssi;
    uint32_t final_offset;
    const static int RSRP_MEASURE_NOF_FRAMES = 5;
  };

  // Class to receive secondary cell
  class scell_recv {
  public:
    const static int MAX_CELLS = 8;
    typedef struct {
      uint32_t pci;
      float    rsrp;
      float    rsrq;
      uint32_t offset;
    } cell_info_t;
    void init(srslte::log *log_h, bool sic_pss_enabled, uint32_t max_sf_window);
    void deinit();
    void reset();
    int find_cells(cf_t *input_buffer, float rx_gain_offset, srslte_cell_t current_cell, uint32_t nof_sf, cell_info_t found_cells[MAX_CELLS]);
  private:

    cf_t               *sf_buffer[SRSLTE_MAX_PORTS];
    srslte::log        *log_h;
    srslte_sync_t       sync_find;

    bool       sic_pss_enabled;
    uint32_t   current_fft_sz;
    measure    measure_p;
  };

  /* TODO: Intra-freq measurements can be improved by capturing 200 ms length signal and run cell search +
   * measurements offline using sync object and finding multiple cells for each N_id_2
   */

  // Class to perform intra-frequency measurements
  class intra_measure : public thread {
  public:
    ~intra_measure();
    void init(phch_common *common, rrc_interface_phy *rrc, srslte::log *log_h);
    void stop();
    void add_cell(int pci);
    void rem_cell(int pci);
    void set_primay_cell(uint32_t earfcn, srslte_cell_t cell);
    void clear_cells();
    int  get_offset(uint32_t pci);
    void write(uint32_t tti, cf_t *data, uint32_t nsamples);
  private:
    void run_thread();
    const static int INTRA_FREQ_MEAS_PRIO      = DEFAULT_PRIORITY + 5;

    scell_recv         scell;
    rrc_interface_phy  *rrc;
    srslte::log        *log_h;
    phch_common        *common;
    uint32_t           current_earfcn;
    uint32_t           current_sflen;
    srslte_cell_t      primary_cell;
    std::vector<int> active_pci;

    srslte::tti_sync_cv tti_sync;

    cf_t               *search_buffer;

    scell_recv::cell_info_t info[scell_recv::MAX_CELLS];

    bool                running;
    bool                receive_enabled;
    bool                receiving;
    uint32_t            measure_tti;
    uint32_t            receive_cnt;
    srslte_ringbuffer_t ring_buffer;
  };

  // 36.133 9.1.2.1 for band 7
  const static float ABSOLUTE_RSRP_THRESHOLD_DBM = -125;

  std::vector<uint32_t> earfcn;

  void   reset();
  void   radio_error();
  void   set_ue_sync_opts(srslte_ue_sync_t *q, float cfo);
  void   run_thread();
  float  get_tx_cfo();

  void   set_sampling_rate();
  bool   set_frequency();
  bool   set_cell();

  uint32_t new_earfcn;
  srslte_cell_t new_cell;

  bool   radio_is_overflow;
  bool   radio_overflow_return;
  bool   running;

  // Objects for internal use
  search                search_p;
  sfn_sync              sfn_p;
  intra_measure         intra_freq_meas;

  uint32_t              current_sflen;
  int                   next_offset;
  uint32_t              nof_rx_antennas;

  // Pointers to other classes
  mac_interface_phy    *mac;
  rrc_interface_phy    *rrc;
  srslte::log          *log_h;
  srslte::log          *log_phy_lib_h;
  srslte::thread_pool  *workers_pool;
  srslte::radio_multi  *radio_h;
  phch_common          *worker_com;
  prach                *prach_buffer;

  // Object for synchronization of the primary cell
  srslte_ue_sync_t      ue_sync;

  // Buffer for primary cell samples
  cf_t                 *sf_buffer[SRSLTE_MAX_PORTS];

  // Sync metrics
  sync_metrics_t        metrics;

  // in-sync / out-of-sync counters
  uint32_t out_of_sync_cnt;
  uint32_t in_sync_cnt;

  const static uint32_t NOF_OUT_OF_SYNC_SF = 200;
  const static uint32_t NOF_IN_SYNC_SF     = 100;

  // State machine for SYNC thread
  class sync_state {
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
    state_t run_state() {
      pthread_mutex_lock(&inside);
      cur_state = next_state;
      pthread_cond_broadcast(&cvar);
      pthread_mutex_unlock(&inside);
      return cur_state;
    }

    // Called by the main thread at the end of each state to indicate it has finished.
    void state_exit(bool exit_ok = true) {
      pthread_mutex_lock(&inside);
      if (cur_state == SFN_SYNC && exit_ok == true) {
        next_state = CAMPING;
      } else {
        next_state = IDLE;
      }
      pthread_mutex_unlock(&inside);
    }
    void force_sfn_sync() {
      pthread_mutex_lock(&inside);
      next_state = SFN_SYNC;
      pthread_mutex_unlock(&inside);
    }

    /* Functions to be called from outside the STM thread to instruct the STM to switch state.
     * The functions change the state and wait until it has changed it.
     *
     * These functions are mutexed and only 1 can be called at a time
     */
    void go_idle() {
      pthread_mutex_lock(&outside);
      go_state(IDLE);
      pthread_mutex_unlock(&outside);
    }
    void run_cell_search() {
      pthread_mutex_lock(&outside);
      go_state(CELL_SEARCH);
      wait_state_change(CELL_SEARCH);
      pthread_mutex_unlock(&outside);
    }
    void run_sfn_sync() {
      pthread_mutex_lock(&outside);
      go_state(SFN_SYNC);
      wait_state_change(SFN_SYNC);
      pthread_mutex_unlock(&outside);
    }


    /* Helpers below this */
    bool is_idle() {
      return cur_state == IDLE;
    }
    bool is_camping() {
      return cur_state == CAMPING;
    }

    const char *to_string() {
      switch(cur_state) {
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

    sync_state() {
      pthread_mutex_init(&inside, NULL);
      pthread_mutex_init(&outside, NULL);
      pthread_cond_init(&cvar, NULL);
      cur_state = IDLE;
      next_state = IDLE;
    }
   private:

    void go_state(state_t s) {
      pthread_mutex_lock(&inside);
      next_state = s;
      while(cur_state != s) {
        pthread_cond_wait(&cvar, &inside);
      }
      pthread_mutex_unlock(&inside);
    }

    /* Waits until there is a call to set_state() and then run_state(). Returns when run_state() returns */
    void wait_state_change(state_t prev_state) {
      pthread_mutex_lock(&inside);
      while(cur_state == prev_state) {
        pthread_cond_wait(&cvar, &inside);
      }
      pthread_mutex_unlock(&inside);
    }

    state_t cur_state, next_state;
    pthread_mutex_t inside, outside;
    pthread_cond_t  cvar;

  };

  pthread_mutex_t rrc_mutex;

  sync_state phy_state;

  search::ret_code   cell_search_ret;

  // Sampling rate mode (find is 1.96 MHz, camp is the full cell BW)
  enum {
    SRATE_NONE=0, SRATE_FIND, SRATE_CAMP
  } srate_mode;
  float         current_srate;

  // This is the primary cell
  srslte_cell_t cell;
  bool          started;
  float         time_adv_sec, next_time_adv_sec;
  uint32_t      tti;
  bool          do_agc;
  
  uint32_t      nof_tx_mutex;
  uint32_t      tx_mutex_cnt;

  float         ul_dl_factor;
  int           current_earfcn;
  uint32_t      cellsearch_earfcn_index;

  float         dl_freq;
  float         ul_freq;

};

} // namespace srsue

#endif // SRSUE_PHCH_RECV_H
