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
  
  
/* The procedure for attaching to an eNodeB is the following: 
 * 
 * 1) Call init() to initialize the PHY (starts an internal thread)
 * 2) Call set_rx_freq() and set_rx_gain() to set Downlink frequency and receiver gain
 * 3) Call decode_mib() to receive and decode MIB from PBCH in the current frequency
 * 4) Call set_cell() to set the current eNodeB
 * 5) Call start_rxtx() to start continuous RX/TX stream
 * 6) Call set_tx_freq()/set_tx_gain() to set Uplink frequency and transmitter gain
 * 7) Call send_prach() to send the PRACH 
 * 
 */
  
typedef _Complex float cf_t; 

class SRSLTE_API phy
{
public:
    
  typedef void (*ue_phy_callback_tti_t) (uint32_t); 
  typedef void (*ue_phy_callback_status_t) (void); 
  ue_phy_callback_tti_t    tti_clock_callback; 
  ue_phy_callback_status_t status_change;
  bool init(ue_phy_callback_tti_t tti_clock_callback, ue_phy_callback_status_t status_change);
  void stop();

  // These functions can be called only if PHY is in IDLE (ie, not RX/TX)
  bool measure(); // TBD  
  bool decode_mib(uint32_t N_id_2, srslte_cell_t *cell, uint8_t payload[SRSLTE_BCH_PAYLOAD_LEN]);  
  bool decode_mib_best(srslte_cell_t *cell, uint8_t payload[SRSLTE_BCH_PAYLOAD_LEN]);  
  bool set_cell(srslte_cell_t cell); 
  
  // Sets the PHY in continuous RX/TX mode
  bool start_rxtx();
  bool stop_rxtx();

  // Indicate the PHY to send PRACH as soon as possible
  bool send_prach(uint32_t preamble_idx);  

  // Control USRP freq/gain
  void set_tx_gain(float gain);
  void set_rx_gain(float gain);
  void set_tx_freq(float freq);
  void set_rx_freq(float freq);
  
  // Get status 
  bool status_is_idle();
  bool status_is_rxtx();
  
  void set_param(params::param_t param, int64_t value); 

  uint32_t        get_current_tti(); 
  static uint32_t tti_to_SFN(uint32_t tti);
  static uint32_t tti_to_subf(uint32_t tti);

  #if SYNC_MODE==SYNC_MODE_CV
  std::condition_variable tti_cv; 
  std::mutex              tti_mutex; 
#endif
  
  ul_buffer* get_ul_buffer(uint32_t tti);
  dl_buffer* get_dl_buffer(uint32_t tti);

  void main_radio_loop(); 
  
private:
  enum {
    IDLE, RXTX
  } phy_state; 
  
  srslte_cell_t cell; 
  bool          cell_is_set;
  bool          is_sfn_synched = false; 
  bool          started        = false; 
  uint32_t      current_tti; 
  
  srslte_ue_sync_t  ue_sync; 
  srslte_ue_mib_t   ue_mib;
  
  queue       *ul_buffer_queue;
  queue       *dl_buffer_queue; 
  prach        prach_buffer; 
  params       params_db; 
  
  pthread_t    radio_thread; 
  void        *radio_handler;
  static void *radio_thread_fnc(void *arg);
  bool         decode_mib_N_id_2(int force_N_id_2, srslte_cell_t *cell, uint8_t payload[SRSLTE_BCH_PAYLOAD_LEN]);
  int          sync_sfn();
  void         run_rx_tx_state();
  bool         init_radio_handler(char *args);
  
};

} 
}
#endif