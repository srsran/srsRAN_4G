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




#include "srsapps/common/log.h"
#include "srsapps/ue/phy/phy.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/common/timers.h"
#include "srsapps/ue/mac/demux.h"
#include "srsapps/ue/mac/dl_sps.h"
#include "srsapps/ue/mac/mac_pcap.h"

#ifndef DLHARQ_H
#define DLHARQ_H

/* Downlink HARQ entity as defined in 5.3.2 of 36.321 */

namespace srslte {
namespace ue {
  
class dl_harq_entity
{
public:

  const static uint32_t NOF_HARQ_PROC = 8; 
  const static uint32_t HARQ_BCCH_PID = NOF_HARQ_PROC; 
  
  dl_harq_entity();
  bool init(log *log_h_, timers *timers_, demux *demux_unit);
  
  
  /***************** PHY->MAC interface for DL processes **************************/
  void new_grant_dl(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_dl_t *action);
  void tb_decoded_ok(uint32_t harq_pid);
 
  
  void reset();
  void start_pcap(mac_pcap* pcap);

private:  
  
  
  class dl_harq_process {
  public:
    dl_harq_process();
    bool init(uint32_t pid, dl_harq_entity *parent);
    void reset();
    bool is_sps(); 
    bool is_new_transmission(mac_interface_phy::mac_grant_t grant); 
    void new_grant_dl(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_dl_t *action);
    void tb_decoded_ok();   
    
  private: 
    
    bool            is_initiated; 
    dl_harq_entity *harq_entity; 
    log            *log_h; 
    
    uint32_t        pid;    
    uint8_t        *payload_buffer_ptr; 
    bool            ack;
    
    mac_interface_phy::mac_grant_t cur_grant;    
    srslte_softbuffer_rx_t         softbuffer; 
    
  };
  static bool      generate_ack_callback(void *arg);

  uint32_t         get_harq_sps_pid(uint32_t tti);
  
  dl_sps           dl_sps_assig;
  
  dl_harq_process  proc[NOF_HARQ_PROC+1];
  timers          *timers_db; 
  demux           *demux_unit; 
  log             *log_h; 
  mac_pcap        *pcap; 
  uint16_t         last_temporal_crnti;
};

} 
}
#endif