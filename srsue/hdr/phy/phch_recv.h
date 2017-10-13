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

#ifndef UEPHYRECV_H
#define UEPHYRECV_H

#include "srslte/srslte.h"
#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/common/thread_pool.h"
#include "srslte/radio/radio_multi.h"
#include "phy/prach.h"
#include "phy/phch_worker.h"
#include "phy/phch_common.h"
#include "srslte/interfaces/ue_interfaces.h"

namespace srsue {
    
typedef _Complex float cf_t; 

class phch_recv : public thread
{
public:
  phch_recv();
  ~phch_recv();
  void init(srslte::radio_multi* radio_handler, mac_interface_phy *mac,rrc_interface_phy *rrc,
            prach *prach_buffer, srslte::thread_pool *_workers_pool,
            phch_common *_worker_com, srslte::log* _log_h, uint32_t nof_rx_antennas, uint32_t prio, int sync_cpu_affinity = -1);
  void stop();
  void set_agc_enable(bool enable);

  void    set_earfcn(std::vector<uint32_t> earfcn);
  void    force_freq(float dl_freq, float ul_freq);

  void    reset_sync();
  void    cell_search_start();
  void    cell_search_stop();
  void    cell_search_next(bool reset = false);
  bool    cell_select(uint32_t earfcn, srslte_cell_t cell);

  uint32_t get_current_tti();
  
  bool    status_is_sync();

  void    set_time_adv_sec(float time_adv_sec);
  void    get_current_cell(srslte_cell_t *cell);

  void    scell_enable(bool enable);

  const static int MUTEX_X_WORKER = 4;

  int radio_recv_fnc(cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *rx_time);
  int scell_recv_fnc(cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *rx_time);

private:

  std::vector<uint32_t> earfcn;

  void   reset();
  void   radio_error();
  bool   wait_radio_reset();
  void   set_ue_sync_opts(srslte_ue_sync_t *q);
  void   run_thread();

  void   set_sampling_rate();
  bool   set_frequency();
  bool   set_cell();

  void   cell_search_inc();
  void   resync_sfn(bool is_connected = false);
  bool   stop_sync();

  void   stop_rx();
  void   start_rx();
  bool   radio_is_rx;

  bool   radio_is_resetting;
  bool   running;

  // Class to run cell search
  class search {
  public:
    typedef enum {CELL_NOT_FOUND, CELL_FOUND, ERROR, TIMEOUT} ret_code;

    ~search();
    void     init(cf_t *buffer[SRSLTE_MAX_PORTS], srslte::log *log_h, uint32_t nof_rx_antennas, phch_recv *parent);
    void     reset();
    float    get_last_gain();
    float    get_last_cfo();
    void     set_N_id_2(int N_id_2);
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
    typedef enum {IDLE, SFN_FOUND, ERROR, TIMEOUT} ret_code;

    ~sfn_sync();
    void     init(srslte_ue_sync_t *ue_sync, cf_t *buffer[SRSLTE_MAX_PORTS], srslte::log *log_h, uint32_t timeout = SYNC_SFN_TIMEOUT);
    void     reset();
    bool     set_cell(srslte_cell_t cell);
    ret_code run_subframe(srslte_cell_t *cell, uint32_t *tti_cnt);

  private:
    srslte::log      *log_h;
    srslte_ue_sync_t *ue_sync;
    cf_t             *buffer[SRSLTE_MAX_PORTS];
    srslte_ue_mib_t   ue_mib;
    uint32_t          cnt;
    uint32_t          timeout;
    const static uint32_t SYNC_SFN_TIMEOUT = 200;
  };

  // Class to perform cell measurements
  class measure {
  public:
    typedef enum {IDLE, MEASURE_OK, ERROR} ret_code;

    ~measure();
    void      init(srslte_ue_sync_t *ue_sync, cf_t *buffer[SRSLTE_MAX_PORTS], srslte::log *log_h,
                   uint32_t nof_rx_antennas, uint32_t nof_subframes = RSRP_MEASURE_NOF_FRAMES);
    void      reset();
    void      set_cell(srslte_cell_t cell);
    ret_code  run_subframe(uint32_t sf_idx);
    float     rsrp();
    float     rsrq();
    float     snr();
    float     cfo();
  private:
    srslte::log      *log_h;
    srslte_ue_dl_t    ue_dl;
    srslte_ue_sync_t *ue_sync;
    cf_t              *buffer[SRSLTE_MAX_PORTS];
    uint32_t cnt;
    uint32_t nof_subframes;
    float mean_rsrp, mean_rsrq, mean_snr, mean_cfo;
    const static int RSRP_MEASURE_NOF_FRAMES = 5;
  };


  // Class to receive secondary cell
  class scell_recv : public thread {
  public:
    void init(phch_recv *parent, srslte::log *log_h, uint32_t nof_rx_antennas, uint32_t prio, int cpu_affinity = -1);
    void stop();
    void reset();
    int  recv(cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *rx_time);
    void write(cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *rx_time);
    bool is_enabled();
    void set_cell(srslte_cell_t scell);
  private:
    void run_thread();

    enum {
      IDLE = 0,
      SCELL_SELECT,
      SCELL_MEASURE,
      SCELL_CAMPING
    } scell_state;

    srslte::log        *log_h;
    phch_recv          *p;
    bool                running;
    srslte_ringbuffer_t ring_buffer[SRSLTE_MAX_PORTS];
    cf_t               *sf_buffer[SRSLTE_MAX_PORTS];
    srslte_ue_sync_t    ue_sync;
    srslte_cell_t       cell;
    uint32_t            nof_rx_antennas;
    uint32_t            current_sflen;

    measure    measure_p;
    sfn_sync   sfn_p;
    uint32_t   tti;
  };




  // Objects for internal use
  scell_recv            scell;
  measure               measure_p;
  search                search_p;
  sfn_sync              sfn_p;

  uint32_t              current_sflen;
  int                   next_offset;
  uint32_t              nof_rx_antennas;

  // Pointers to other classes
  mac_interface_phy    *mac;
  rrc_interface_phy    *rrc;
  srslte::log          *log_h;
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

  // State for primary cell
  enum {
    IDLE = 0,
    CELL_SEARCH,
    CELL_SELECT,
    CELL_RESELECT,
    CELL_MEASURE,
    CELL_CAMP
  } phy_state;

  bool is_in_idle;

  // Sampling rate mode (find is 1.96 MHz, camp is the full cell BW)
  enum {
    SRATE_NONE=0, SRATE_FIND, SRATE_CAMP
  } srate_mode;
  float         current_srate;

  // This is the primary cell
  srslte_cell_t cell;
  bool          cell_is_set;
  bool          started;
  float         time_adv_sec;
  uint32_t      tti;
  bool          do_agc;
  
  uint32_t      nof_tx_mutex;
  uint32_t      tx_mutex_cnt;

  float         ul_dl_factor;
  uint32_t      current_earfcn;
  int           cur_earfcn_index;
  bool          cell_search_in_progress;

  float         dl_freq;
  float         ul_freq;

};

} // namespace srsue

#endif // UEPHYRECV_H
