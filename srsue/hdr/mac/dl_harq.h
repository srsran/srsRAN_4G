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

#ifndef DL_HARQ_H
#define DL_HARQ_H

#include "srslte/common/log.h"
#include "srslte/common/timers.h"
#include "mac/demux.h"
#include "mac/dl_sps.h"
#include "srslte/common/mac_pcap.h"

#include "srslte/interfaces/ue_interfaces.h"

/* Downlink HARQ entity as defined in 5.3.2 of 36.321 */


namespace srsue {
  
class dl_harq_entity
{
public:

  const static uint32_t NOF_HARQ_PROC = 8; 
  const static uint32_t HARQ_BCCH_PID = NOF_HARQ_PROC; 
  
  dl_harq_entity();
  bool init(srslte::log *log_h_, mac_interface_rrc::mac_cfg_t *mac_cfg, srslte::timers *timers_, demux *demux_unit);
  
  
  /***************** PHY->MAC interface for DL processes **************************/
  void new_grant_dl(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_dl_t *action);
  void tb_decoded(bool ack, srslte_rnti_type_t rnti_type, uint32_t harq_pid);
 
  
  void reset();
  void start_pcap(srslte::mac_pcap* pcap);
  int  get_current_tbs(uint32_t harq_pid);

  void set_si_window_start(int si_window_start);
  
  float get_average_retx(); 
  
private:  
  
  
  class dl_harq_process {
  public:
    dl_harq_process();
    bool init(uint32_t pid, dl_harq_entity *parent);
    void reset();
    bool is_sps(); 
    void new_grant_dl(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_dl_t *action);
    void tb_decoded(bool ack);   
    int get_current_tbs();
    
  private: 
    bool calc_is_new_transmission(mac_interface_phy::mac_grant_t grant); 
    
    bool            is_initiated; 
    dl_harq_entity *harq_entity; 
    srslte::log    *log_h;
    
    bool	    is_new_transmission; 
    
    uint32_t        pid;    
    uint8_t        *payload_buffer_ptr; 
    bool            ack;
    
    uint32_t 	    n_retx; 
    
    mac_interface_phy::mac_grant_t cur_grant;    
    srslte_softbuffer_rx_t         softbuffer; 
    
  };
  static bool      generate_ack_callback(void *arg);

  uint32_t         get_harq_sps_pid(uint32_t tti);
  
  dl_sps           dl_sps_assig;
  
  dl_harq_process  proc[NOF_HARQ_PROC+1];
  srslte::timers   *timers_db;
  mac_interface_rrc::mac_cfg_t *mac_cfg; 
  demux           *demux_unit; 
  srslte::log     *log_h;
  srslte::mac_pcap *pcap; 
  uint16_t         last_temporal_crnti;
  int 	           si_window_start;

  float 	   average_retx;   
  uint64_t         nof_pkts; 
};

} // namespace srsue

#endif // DL_HARQ_H
