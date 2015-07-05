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
#include "srsapps/common/tti_sync.h"
#include "srsapps/ue/phy/dl_buffer.h"
#include "srsapps/ue/phy/ul_buffer.h"
#include "srsapps/ue/phy/prach.h"
#include "srsapps/ue/phy/phy_params.h"
#include "srsapps/ue/phy/sched_grant.h"
#include "srsapps/common/queue.h"
#include "srsapps/radio/radio.h"
#include "srsapps/common/trace.h"

#ifndef UEPHY_H
#define UEPHY_H

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

class phy
{
public:
  phy();
  void set();
  bool init(radio *radio_handler, tti_sync *ttisync, log *log_h);
  bool init_agc(radio *radio_handler, tti_sync *ttisync, log *log_h);
  void stop();

  // These functions can be called only if PHY is in IDLE (ie, not RX/TX)
  bool measure(); // TBD  
  bool decode_mib(uint32_t N_id_2, srslte_cell_t *cell, uint8_t payload[SRSLTE_BCH_PAYLOAD_LEN]);  
  bool decode_mib_best(srslte_cell_t *cell, uint8_t payload[SRSLTE_BCH_PAYLOAD_LEN]);  
  bool set_cell(srslte_cell_t cell); 
  
  // Sets the PHY in continuous RX/TX mode
  bool start_rxtx();
  bool stop_rxtx();

  float get_agc_gain();
  
  void set_crnti(uint16_t rnti);
  void pregen_signals();
  
  // Indicate the PHY to send PRACH as soon as possible
  bool init_prach();
  bool send_prach(uint32_t preamble_idx);  
  bool send_prach(uint32_t preamble_idx, int allowed_subframe);  
  bool send_prach(uint32_t preamble_idx, int allowed_subframe, int target_power_dbm);  

  // Indicate the PHY to send SR as soon as possible or not
  void send_sr(bool enable);
  int  sr_last_tx_tti(); 
  
  // Returns TTI when PRACH was transmitted. -1 if not yet transmitted
  int get_prach_transmitted_tti();

  // Get handler to the radio
  radio* get_radio(); 
  
  // Time advance commands
  void set_timeadv_rar(uint32_t ta_cmd);
  void set_timeadv(uint32_t ta_cmd);
  
  // Convert Msg3 UL grant to ul_sched_grant 
  void rar_ul_grant(srslte_dci_rar_grant_t *rar, ul_sched_grant *grant); 
  
  // Get status 
  bool status_is_idle();
  bool status_is_rxtx();
  
  void set_param(phy_params::phy_param_t param, int64_t value); 
  int64_t get_param(phy_params::phy_param_t param);

  uint32_t        get_current_tti(); 
  static uint32_t tti_to_SFN(uint32_t tti);
  static uint32_t tti_to_subf(uint32_t tti);

  ul_buffer* get_ul_buffer(uint32_t tti);
  dl_buffer* get_dl_buffer(uint32_t tti);

  void start_trace();
  void write_trace(std::string filename); 
  
  bool sr_is_ready_to_send(uint32_t tti);
  bool cqi_is_ready_to_send(uint32_t tti);

  void main_radio_loop(); 
  
private:
  enum {
    IDLE, RXTX
  } phy_state; 
  
  static const int NOF_ULDL_QUEUES = 10; 
  
  tti_sync      *ttisync; 
  radio         *radio_handler;
  log           *log_h; 
  
  srslte_cell_t cell; 
  bool          cell_is_set;
  bool          is_sfn_synched; 
  volatile bool          started; 
  
  srslte_ue_sync_t  ue_sync; 
  srslte_ue_mib_t   ue_mib;
  
  queue       *ul_buffer_queue;
  queue       *dl_buffer_queue; 
  prach        prach_buffer; 
  phy_params   params_db; 
  
  pthread_t    phy_thread; 
  float        time_adv_sec;
  uint32_t     n_ta;
  bool         radio_is_streaming;
  srslte_timestamp_t last_rx_time; 
  float        cellsearch_cfo;
  bool         do_agc;
  double       last_gain;
  
  bool         sr_enabled;
  
  bool         init_(radio *radio_handler, tti_sync *ttisync, log *log_h, bool do_agc);
  static void *phy_thread_fnc(void *arg);
  bool         decode_mib_N_id_2(int force_N_id_2, srslte_cell_t *cell, uint8_t payload[SRSLTE_BCH_PAYLOAD_LEN]);
  int          sync_sfn();
  void         run_rx_tx_state();
  ul_buffer*   get_ul_buffer_adv(uint32_t tti);
  float        old_gain; 
  uint32_t     sr_tx_tti;

  bool         is_first_of_burst; 
  
  trace<uint32_t> tr_start_time;
  trace<uint32_t> tr_end_time;
  bool tr_enabled; 
  void tr_log_start();
  void tr_log_end();    
};

} 
}
#endif
