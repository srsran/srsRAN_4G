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

  void    reset_sync();
  void    cell_search_start();
  void    cell_search_stop();
  void    cell_search_next(bool reset = false);
  bool    cell_select(uint32_t earfcn, srslte_cell_t cell);

  uint32_t get_current_tti();
  
  bool    status_is_sync();

  void    set_time_adv_sec(float time_adv_sec);
  void    get_current_cell(srslte_cell_t *cell);
  
  const static int MUTEX_X_WORKER = 4;

  // public variables needed by callback function
  uint32_t              current_sflen;
  srslte::radio_multi  *radio_h;
  int                   next_offset;


private:

  std::vector<uint32_t> earfcn;

  void   reset();
  void   radio_error();
  bool   wait_radio_reset();
  void   set_ue_sync_opts(srslte_ue_sync_t *q); 
  void   run_thread();

  void   set_sampling_rate();
  bool   set_frequency();
  void   resync_sfn(bool is_connected = false);
  bool   stop_sync();

  void   cell_search_inc();

  bool   init_cell();
  void   free_cell();

  void   stop_rx();
  void   start_rx();
  bool   radio_is_rx;

  bool   radio_is_resetting;

  bool   running; 
  
  mac_interface_phy    *mac;
  rrc_interface_phy    *rrc;
  srslte::log          *log_h;
  srslte::thread_pool  *workers_pool;
  phch_common          *worker_com;
  prach                *prach_buffer;

  // Structures for Cell Camp
  srslte_ue_sync_t    ue_sync;
  srslte_ue_mib_t     ue_mib;

  // Structures for Cell Search
  srslte_ue_cellsearch_t cs;
  srslte_ue_mib_sync_t   ue_mib_sync;

  uint32_t      nof_rx_antennas;

  cf_t *sf_buffer[SRSLTE_MAX_PORTS];

  // Sync metrics
  sync_metrics_t metrics;

  enum {
    IDLE = 0,
    CELL_SEARCH,
    CELL_SELECT,
    CELL_RESELECT,
    CELL_MEASURE,
    CELL_CAMP
  } phy_state;

  bool is_in_idle;

  enum {
    SRATE_NONE=0, SRATE_FIND, SRATE_CAMP
  } srate_mode;
  float         current_srate;

  srslte_cell_t cell;
  bool          cell_is_set;
  bool          is_sfn_synched; 
  bool          started; 
  float         time_adv_sec;
  uint32_t      tti;
  bool          do_agc;
  
  float         last_gain;
  float         cellsearch_cfo;
  uint32_t      nof_tx_mutex;
  uint32_t      tx_mutex_cnt;

  uint32_t      current_earfcn;

  uint32_t      sync_sfn_cnt;
  const static uint32_t SYNC_SFN_TIMEOUT = 200;
  float ul_dl_factor;
  int cur_earfcn_index;
  bool cell_search_in_progress;
  uint32_t measure_cnt;
  float    measure_rsrp;
  srslte_ue_dl_t ue_dl_measure;

  const static int RSRP_MEASURE_NOF_FRAMES = 5;

  int    cell_sync_sfn();
  int    cell_meas_rsrp();
  int    cell_search(int force_N_id_2 = -1);
  bool   set_cell();
};

} // namespace srsue

#endif // UEPHYRECV_H
