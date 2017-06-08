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

#define Error(fmt, ...)   log_h->error_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#include "mac/mac.h"
#include "mac/dl_harq.h"


namespace srsue {
    

  /***********************************************************
  * 
  * HARQ ENTITY
  * 
  *********************************************************/
        
dl_harq_entity::dl_harq_entity()
{
  pcap = NULL; 
}

bool dl_harq_entity::init(srslte::log* log_h_, mac_interface_rrc::mac_cfg_t *mac_cfg_, srslte::timers* timers_, demux *demux_unit_)
{
  timers_db  = timers_; 
  demux_unit = demux_unit_; 
  mac_cfg    = mac_cfg_; 
  si_window_start = 0; 
  log_h = log_h_; 
  for (uint32_t i=0;i<NOF_HARQ_PROC+1;i++) {
    if (!proc[i].init(i, this)) {
      return false; 
    }
  }
  return true; 

}

void dl_harq_entity::start_pcap(srslte::mac_pcap* pcap_)
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
  return 0;
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

int dl_harq_entity::get_current_tbs(uint32_t harq_pid)
{
  return proc[harq_pid%NOF_HARQ_PROC].get_current_tbs();
}


bool dl_harq_entity::generate_ack_callback(void *arg)
{
  demux *demux_unit = (demux*) arg;
  return demux_unit->get_uecrid_successful();
}

void dl_harq_entity::set_si_window_start(int si_window_start_)
{
  si_window_start = si_window_start_;
}

float dl_harq_entity::get_average_retx()
{
  return average_retx; 
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
  if (srslte_softbuffer_rx_init(&softbuffer, 110)) {
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

bool dl_harq_entity::dl_harq_process::calc_is_new_transmission(mac_interface_phy::mac_grant_t grant) {
  
  bool is_new_tb = true; 
  if ((srslte_tti_interval(grant.tti, cur_grant.tti) <= 8 && (grant.n_bytes == cur_grant.n_bytes)) ||
      pid == HARQ_BCCH_PID) 
  {
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

  return is_new_transmission;
}

void dl_harq_entity::dl_harq_process::new_grant_dl(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_dl_t* action)
{
  // Compute RV for BCCH when not specified in PDCCH format
  if (pid == HARQ_BCCH_PID && grant.rv == -1) {
    uint32_t k; 
    if ((grant.tti/10)%2 == 0 && grant.tti%10 == 5) { // This is SIB1, k is different
      k = (grant.tti/20)%4; 
      grant.rv = ((uint32_t) ceilf((float)1.5*k))%4;
    } else if (grant.rv == -1) {      
      k = (grant.tti-harq_entity->si_window_start)%4; 
      grant.rv = ((uint32_t) ceilf((float)1.5*k))%4;
    }
  }
  calc_is_new_transmission(grant);
  if (is_new_transmission) {
    ack = false; 
    srslte_softbuffer_rx_reset_tbs(&softbuffer, cur_grant.n_bytes*8);
    n_retx = 0; 
  }
  
  // Save grant 
  grant.last_ndi = cur_grant.ndi; 
  grant.last_tti = cur_grant.tti; 
  memcpy(&cur_grant, &grant, sizeof(mac_interface_phy::mac_grant_t)); 
  
  // Fill action structure 
  bzero(action, sizeof(mac_interface_phy::tb_action_dl_t));
  action->default_ack = ack; 
  action->generate_ack = true; 
  action->decode_enabled = false; 
  
  // If data has not yet been successfully decoded
  if (ack == false) {
    
    // Instruct the PHY To combine the received data and attempt to decode it
    payload_buffer_ptr = harq_entity->demux_unit->request_buffer(pid, cur_grant.n_bytes);
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
    n_retx++; 
    
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

int dl_harq_entity::dl_harq_process::get_current_tbs()
{
  return cur_grant.n_bytes*8;
}

void dl_harq_entity::dl_harq_process::tb_decoded(bool ack_)
{
  ack = ack_;
  if (ack == true) {
    if (pid == HARQ_BCCH_PID) {
      if (harq_entity->pcap) {
        harq_entity->pcap->write_dl_sirnti(payload_buffer_ptr, cur_grant.n_bytes, ack, cur_grant.tti);
      }
      Debug("Delivering PDU=%d bytes to Dissassemble and Demux unit (BCCH)\n", cur_grant.n_bytes);
      harq_entity->demux_unit->push_pdu(pid, payload_buffer_ptr, cur_grant.n_bytes, cur_grant.tti);
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
          harq_entity->demux_unit->push_pdu(pid, payload_buffer_ptr, cur_grant.n_bytes, cur_grant.tti);

          // Compute average number of retransmissions per packet 
          harq_entity->average_retx = SRSLTE_VEC_CMA((float) n_retx, harq_entity->average_retx, harq_entity->nof_pkts++); 
        }
      }
    }
  } else {
    harq_entity->demux_unit->deallocate(payload_buffer_ptr);
  }
  
  Info("DL %d:  %s tbs=%d, rv=%d, ack=%s, ndi=%d (%d), tti=%d (%d)\n", 
       pid, is_new_transmission?"newTX":"reTX ", 
       cur_grant.n_bytes, cur_grant.rv, ack?"OK":"KO", 
       cur_grant.ndi, cur_grant.last_ndi, cur_grant.tti, cur_grant.last_tti);
  
  if (ack && pid == HARQ_BCCH_PID) {
    reset();
  }
}


    
}
