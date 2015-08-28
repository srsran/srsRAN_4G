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

#include "srsapps/ue/phy/phy.h"

#include "srsapps/ue/mac/mac.h"
#include "srsapps/ue/mac/dl_harq.h"

namespace srslte {
  namespace ue {
    

  /***********************************************************
  * 
  * HARQ ENTITY
  * 
  *********************************************************/
        
dl_harq_entity::dl_harq_entity()
{
  pcap = NULL; 
}
bool dl_harq_entity::init(log* log_h_, mac_params *params_db_, timers* timers_, demux *demux_unit_)
{
  timers_db  = timers_; 
  demux_unit = demux_unit_; 
  params_db  = params_db_; 
  log_h = log_h_; 
  for (uint32_t i=0;i<NOF_HARQ_PROC+1;i++) {
    if (!proc[i].init(i, this)) {
      return false; 
    }
  }
  return true; 

}

void dl_harq_entity::start_pcap(mac_pcap* pcap_)
{
  pcap = pcap_; 
}

void dl_harq_entity::reset()
{
  for (uint32_t i=0;i<NOF_HARQ_PROC+1;i++) {
    proc[i].reset();
  }
  dl_sps_assig.clear();
}

uint32_t dl_harq_entity::get_harq_sps_pid(uint32_t tti) {
  /*
  uint32_t nof_proc = ((uint32_t) params_db->get_param(mac_interface_params::SPS_DL_NOF_PROC));
  return tti/params_db.get_param(mac_interface_params::SPS_DL_SCHED_INTERVAL)%nof_proc;
  */
}

void dl_harq_entity::new_grant_dl(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_dl_t* action)
{
  
  if (grant.rnti_type != SRSLTE_RNTI_SPS) {
    uint32_t harq_pid; 
    // Set BCCH PID for SI RNTI 
    if (grant.rnti_type == SRSLTE_RNTI_SI) {
      harq_pid = HARQ_BCCH_PID; 
    } else {
      harq_pid = grant.pid%NOF_HARQ_PROC; 
    }
    if (grant.rnti_type == SRSLTE_RNTI_TEMP && last_temporal_crnti != grant.rnti) {
      grant.ndi = true;
      Info("Set NDI=1 for Temp-RNTI DL grant\n");
      last_temporal_crnti = grant.rnti;
    }
    if (grant.rnti_type == SRSLTE_RNTI_USER && proc[harq_pid].is_sps()) {
      grant.ndi = true;
      Info("Set NDI=1 for C-RNTI DL grant\n");
    }
    proc[harq_pid].new_grant_dl(grant, action);
  } else {
    /* This is for SPS scheduling */
    uint32_t harq_pid = get_harq_sps_pid(grant.tti)%NOF_HARQ_PROC; 
    if (grant.ndi) {
      grant.ndi = false; 
      proc[harq_pid].new_grant_dl(grant, action);
    } else {
      if (grant.is_sps_release) {
        dl_sps_assig.clear();
        if (timers_db->get(mac::TIME_ALIGNMENT)->is_running()) {
          //phy_h->send_sps_ack();
          Warning("PHY Send SPS ACK not implemented\n");
        }
      } else {
        Error("SPS not implemented\n");
        //dl_sps_assig.reset(grant.tti, grant);
        //grant.ndi = true;
        //procs[harq_pid].save_grant();
      }
    }
  }
}

void dl_harq_entity::tb_decoded(bool ack, srslte_rnti_type_t rnti_type, uint32_t harq_pid)
{
  if (rnti_type == SRSLTE_RNTI_SI) {
    proc[NOF_HARQ_PROC].tb_decoded(ack);    
  } else {
    proc[harq_pid%NOF_HARQ_PROC].tb_decoded(ack);
  }
}

bool dl_harq_entity::generate_ack_callback(void *arg)
{
  demux *demux_unit = (demux*) arg;
  return demux_unit->get_uecrid_successful();
}



  /***********************************************************
  * 
  * HARQ PROCESS
  * 
  *********************************************************/
          
dl_harq_entity::dl_harq_process::dl_harq_process() {
  is_initiated = false; 
  ack = false; 
  bzero(&cur_grant, sizeof(mac_interface_phy::mac_grant_t));
}  
  
void dl_harq_entity::dl_harq_process::reset() {
  ack = false; 
  payload_buffer_ptr = NULL; 
  bzero(&cur_grant, sizeof(mac_interface_phy::mac_grant_t));
  if (is_initiated) {
    srslte_softbuffer_rx_reset(&softbuffer);
  }
}

bool dl_harq_entity::dl_harq_process::init(uint32_t pid_, dl_harq_entity *parent) {
  if (srslte_softbuffer_rx_init(&softbuffer, 100)) {
    Error("Error initiating soft buffer\n");
    return false; 
  } else {
    pid = pid_;
    is_initiated = true; 
    harq_entity = parent; 
    log_h = harq_entity->log_h; 
    return true;
  }     
}

bool dl_harq_entity::dl_harq_process::is_sps()
{
  return false; 
}                                                            

bool dl_harq_entity::dl_harq_process::is_new_transmission(mac_interface_phy::mac_grant_t grant) {
  bool is_new_transmission; 
  
  bool is_new_tb = true; 
  if (srslte_tti_interval(grant.tti, cur_grant.tti) <= 8 && grant.n_bytes == cur_grant.n_bytes) {
    is_new_tb = false; 
  }
  
  if ((grant.ndi != cur_grant.ndi && !is_new_tb) || // NDI toggled for same TB
      is_new_tb                                  || // is new TB
      (pid == HARQ_BCCH_PID && grant.rv == 0))      // Broadcast PID and 1st TX (RV=0)
  {
    is_new_transmission = true; 
    Debug("Set HARQ for new transmission\n");
  } else {
    is_new_transmission = false; 
    Debug("Set HARQ for retransmission\n");
  }

  Info("DL PID %d: %s RV=%d, NDI=%d, LastNDI=%d\n", pid, is_new_transmission?"new TX":"reTX", grant.rv, 
         grant.ndi, cur_grant.ndi);   
  
  return is_new_transmission;
}

void dl_harq_entity::dl_harq_process::new_grant_dl(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_dl_t* action)
{
  if (pid == HARQ_BCCH_PID) {
    // Compute RV
    uint32_t k; 
    if (grant.tti%10 == 5) { // This is SIB1, k is different
      k = (grant.tti/20)%4; 
    } else {      
      uint32_t nw = harq_entity->params_db->get_param(mac_interface_params::BCCH_SI_WINDOW_LEN);
      k = (grant.tti%nw)%4; 
    }
    grant.rv = ((uint32_t) ceilf((float)1.5*k))%4;
  }
  
  if (is_new_transmission(grant)) {
    ack = false; 
    srslte_softbuffer_rx_reset_tbs(&softbuffer, cur_grant.n_bytes*8);
  }
  
  // Save grant 
  memcpy(&cur_grant, &grant, sizeof(mac_interface_phy::mac_grant_t)); 
  
  // Fill action structure 
  bzero(action, sizeof(mac_interface_phy::tb_action_dl_t));
  action->default_ack = ack; 
  action->generate_ack = true; 
  action->decode_enabled = false; 
  
  // If data has not yet been successfully decoded
  if (ack == false) {
    
    // Instruct the PHY To combine the received data and attempt to decode it
    payload_buffer_ptr = harq_entity->demux_unit->request_buffer(cur_grant.n_bytes);
    action->payload_ptr = payload_buffer_ptr;
    if (!action->payload_ptr) {
      action->decode_enabled = false; 
      Error("Can't get a buffer for TBS=%d\n", cur_grant.n_bytes);
      return;       
    }    
    action->decode_enabled = true;     
    action->rv = cur_grant.rv; 
    action->rnti = cur_grant.rnti; 
    action->softbuffer = &softbuffer;     
    memcpy(&action->phy_grant, &cur_grant.phy_grant, sizeof(srslte_phy_grant_t));
    
  } else {
    Warning("DL PID %d: Received duplicate TB. Discarting and retransmitting ACK\n", pid);
  }
    
  if (pid == HARQ_BCCH_PID || harq_entity->timers_db->get(mac::TIME_ALIGNMENT)->is_expired()) {
    // Do not generate ACK
    Debug("Not generating ACK\n");
    action->generate_ack = false;    
  } else {
    if (cur_grant.rnti_type == SRSLTE_RNTI_TEMP && ack == false) {
      // Postpone ACK after contention resolution is resolved
      action->generate_ack_callback = harq_entity->generate_ack_callback; 
      action->generate_ack_callback_arg = harq_entity->demux_unit;
      Debug("ACK pending contention resolution\n");
    } else {
      Debug("Generating ACK\n");
    }
  }
}

void dl_harq_entity::dl_harq_process::tb_decoded(bool ack_)
{
  ack = ack_;
  if (ack == true) {
    if (pid == HARQ_BCCH_PID) {
      if (harq_entity->pcap) {
        harq_entity->pcap->write_dl_sirnti(payload_buffer_ptr, cur_grant.n_bytes, ack, cur_grant.tti);
      }
      if (ack) {
        Debug("Delivering PDU=%d bytes to Dissassemble and Demux unit (BCCH)\n", cur_grant.n_bytes);
        harq_entity->demux_unit->push_pdu_bcch(payload_buffer_ptr, cur_grant.n_bytes);
      }
    } else {
      
      if (harq_entity->pcap) {
        harq_entity->pcap->write_dl_crnti(payload_buffer_ptr, cur_grant.n_bytes, cur_grant.rnti, ack, cur_grant.tti);            
      }
      if (ack) {
        if (cur_grant.rnti_type == SRSLTE_RNTI_TEMP) {
          Debug("Delivering PDU=%d bytes to Dissassemble and Demux unit (Temporal C-RNTI)\n", cur_grant.n_bytes);
          harq_entity->demux_unit->push_pdu_temp_crnti(payload_buffer_ptr, cur_grant.n_bytes);
        } else {
          Debug("Delivering PDU=%d bytes to Dissassemble and Demux unit\n", cur_grant.n_bytes);
          harq_entity->demux_unit->push_pdu(payload_buffer_ptr, cur_grant.n_bytes);
        }
      }
    }
  } else {
    harq_entity->demux_unit->release_buffer(payload_buffer_ptr);
  }
  Info("DL PID %d: TBS=%d, RV=%d, ACK=%s\n", pid, cur_grant.n_bytes, cur_grant.rv, ack?"OK":"KO");
}


    
}
}