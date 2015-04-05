/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/srslte.h"
#include "srslte/ue_itf/dl_buffer.h"
#include "srslte/ue_itf/ul_buffer.h"
#include "srslte/ue_itf/prach.h"
#include "srslte/ue_itf/params.h"
#include "srslte/ue_itf/sched_grant.h"
#include "srslte/ue_itf/queue.h"

#ifndef UEPHY_H
#define UEPHY_H

#define SYNC_MODE_CV       0
#define SYNC_MODE_CALLBACK 1
#define SYNC_MODE          SYNC_MODE_CALLBACK

namespace srslte {
namespace ue {
  
typedef _Complex float cf_t; 

class SRSLTE_API phy
{
public:
    
#if SYNC_MODE==SYNC_MODE_CALLBACK
  typedef void (*ue_phy_callback_t) (void); 
  ue_phy_callback_t tti_clock_callback; 
  ue_phy_callback_t status_change;
  phy(ue_phy_callback_t tti_clock_callback, ue_phy_callback_t status_change);
#else
  phy();
#endif
  ~phy();
  
  void measure(); // TBD  
  void dl_bch();  
  void start_rxtx();
  void stop_rxtx();
  void send_prach(uint32_t preamble_idx);  
  
  bool status_is_idle();
  bool status_is_rxtx();
  bool status_is_bch_decoded(uint8_t payload[SRSLTE_BCH_PAYLOAD_LEN]);
  

  uint32_t                get_tti();
#if SYNC_MODE==SYNC_MODE_CV
  std::condition_variable tti_cv; 
  std::mutex              tti_mutex; 
#endif
  
  ul_buffer* get_ul_buffer(uint32_t tti);
  dl_buffer* get_dl_buffer(uint32_t tti);

  void main_radio_loop(); 
  
private:
  enum {
    IDLE, MEASURE, RX_BCH, MIB_DECODED, RXTX
  } phy_state; 
  
  srslte_cell_t cell; 
  uint8_t  bch_payload[SRSLTE_BCH_PAYLOAD_LEN];
  bool     is_sfn_synched = false; 
  bool     started        = false; 
  uint32_t current_tti; 
  
  srslte_ue_sync_t  ue_sync; 
  srslte_ue_mib_t   ue_mib;
  
  queue       *ul_buffer_queue;
  queue       *dl_buffer_queue; 
  prach        prach_buffer; 
  params       params_db; 
  
  pthread_t    radio_thread; 
  void        *radio_handler;
  static void *radio_thread_fnc(void *arg);
  void         run_rx_bch_state();
  bool         rx_bch(); 
  int          sync_sfn();
  void         run_rx_tx_state();
};

} 
}
#endif