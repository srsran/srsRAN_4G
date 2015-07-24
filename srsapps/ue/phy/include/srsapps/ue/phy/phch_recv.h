/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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




#include "srslte/srslte.h"
#include "srsapps/common/log.h"
#include "srsapps/common/threads.h"
#include "srsapps/common/thread_pool.h"
#include "srsapps/radio/radio.h"
#include "srsapps/ue/phy/prach.h"
#include "srsapps/ue/phy/phch_worker.h"
#include "srsapps/ue/phy/phch_common.h"

#ifndef UEPHYRECV_H
#define UEPHYRECV_H

namespace srslte {
namespace ue {
    
typedef _Complex float cf_t; 

class phch_recv : public thread
{
public:
  phch_recv();
  bool init(radio* radio_handler, mac_interface_phy *mac, prach *prach_buffer, thread_pool *_workers_pool, 
            phch_common *_worker_com, log* _log_h, bool do_agc = false, uint32_t prio = 1);
  void stop();
  
  uint32_t get_current_tti();
  
  void    sync_start(); 
  void    sync_stop();
  bool    status_is_sync();

  void    set_time_adv_sec(float time_adv_sec);
  void    get_current_cell(srslte_cell_t *cell);
  
private:
  void   run_thread();
  int    sync_sfn();
  
  bool   running; 
  
  radio             *radio_h;
  mac_interface_phy *mac;
  log               *log_h; 
  thread_pool       *workers_pool; 
  phch_common       *worker_com;
  prach             *prach_buffer; 
  
  srslte_ue_sync_t   ue_sync;
  srslte_ue_mib_t    ue_mib;

  enum {
    IDLE, CELL_SEARCH, SYNCING, SYNC_DONE
  } phy_state; 

  srslte_cell_t cell; 
  bool          cell_is_set;
  bool          is_sfn_synched; 
  bool          started; 
  float         time_adv_sec;
  bool          radio_is_streaming;
  uint32_t      tti; 
  bool          do_agc;
  
  float         last_gain;
  float         cellsearch_cfo;
  
  bool          cell_search(int force_N_id_2 = -1);
  bool          init_cell();
  void          free_cell();
};

}
}

#endif