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
#include "srsapps/ue/phy/dl_sched_grant.h"

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
  for (uint32_t i=0;i<NOF_HARQ_PROC+1;i++) {
    proc[i].pid = i; 
  }  
  pending_ack_pid = -1; 
  pcap = NULL; 
}
bool dl_harq_entity::init(srslte_cell_t cell, uint32_t max_payload_len, log* log_h_, timers* timers_, demux *demux_unit_)
{
  timers_db  = timers_; 
  demux_unit = demux_unit_; 
  log_h = log_h_; 
  for (uint32_t i=0;i<NOF_HARQ_PROC+1;i++) {
    if (!proc[i].init(cell, max_payload_len, this)) {
      return false; 
    }
  }
  return true; 

}

void dl_harq_entity::start_pcap(mac_pcap* pcap_)
{
  pcap = pcap_; 
}

bool dl_harq_entity::is_sps(uint32_t pid)
{
  return false; 
}                                                            
void dl_harq_entity::set_harq_info(uint32_t pid, dl_sched_grant* grant)
{
  proc[pid%(NOF_HARQ_PROC+1)].set_harq_info(grant);
}

void dl_harq_entity::receive_data(uint32_t tti, uint32_t pid, dl_buffer* dl_buffer, phy* phy_h)
{
  proc[pid%(NOF_HARQ_PROC+1)].receive_data(tti, dl_buffer, phy_h);
}

void dl_harq_entity::reset()
{
  for (uint32_t i=0;i<NOF_HARQ_PROC+1;i++) {
    proc[i].reset();
  }
}

bool dl_harq_entity::is_ack_pending_resolution()
{
  return pending_ack_pid >= 0; 
}

void dl_harq_entity::send_pending_ack_contention_resolution()
{
  if (is_ack_pending_resolution()) {
    proc[pending_ack_pid].send_pending_ack_contention_resolution();
    pending_ack_pid = -1; 
  }
}

  /***********************************************************
  * 
  * HARQ PROCESS
  * 
  *********************************************************/
          
dl_harq_entity::dl_harq_process::dl_harq_process() : cur_grant(0),pending_ack_grant(0) {
  is_initiated = false; 
  ack = false; 
  bzero(&cur_grant, sizeof(srslte::ue::dl_sched_grant));
  payload = NULL; 
  max_payload_len = 0; 
}  
  
void dl_harq_entity::dl_harq_process::reset() {
  ack = false; 
  bzero(&cur_grant, sizeof(srslte::ue::dl_sched_grant));
  if (is_initiated) {
    srslte_softbuffer_rx_reset(&softbuffer);
  }
}

void dl_harq_entity::dl_harq_process::send_pending_ack_contention_resolution()
{
  if (pending_ul_buffer) {
    pending_ul_buffer->generate_ack(pending_ack, &pending_ack_grant);                      
  }
}

void dl_harq_entity::dl_harq_process::receive_data(uint32_t tti, srslte::ue::dl_buffer *dl_buffer, phy *phy_h)
{
  pending_ul_buffer = NULL; 
  
  if (payload) {
    if (cur_grant.get_tbs() <= max_payload_len) {
      
      // If data has not yet been successfully decoded
      if (ack == false) {
        
        // Combine the received data and attempt to decode it
        if (dl_buffer->decode_data(&cur_grant, &softbuffer, payload)) {
          ack = true; 
        } else {
          ack = false; 
        }

        Info("DL PID %d: TBS=%d, RV=%d, MCS=%d, crc=%s\n", pid, cur_grant.get_tbs(), cur_grant.get_rv(), cur_grant.get_mcs(), ack?"OK":"NOK");

        if (pid == HARQ_BCCH_PID) {
          if (ack) {
            Debug("Delivering PDU=%d bytes to Dissassemble and Demux unit (BCCH)\n", cur_grant.get_tbs()/8);
            harq_entity->demux_unit->push_pdu_bcch(payload, cur_grant.get_tbs());        
          }
          if (harq_entity->pcap) {
            harq_entity->pcap->write_dl_sirnti(payload, cur_grant.get_tbs()/8, ack, tti);
          }
        } else {
          if (ack) {
            if (cur_grant.is_temp_rnti()) {
              Debug("Delivering PDU=%d bytes to Dissassemble and Demux unit (Temporal C-RNTI)\n",
                    cur_grant.get_tbs()/8);
              harq_entity->demux_unit->push_pdu_temp_crnti(payload, cur_grant.get_tbs());
            } else {
              Debug("Delivering PDU=%d bytes to Dissassemble and Demux unit\n", cur_grant.get_tbs()/8);
              harq_entity->demux_unit->push_pdu(payload, cur_grant.get_tbs());
            }
          }
          if (harq_entity->pcap) {
            harq_entity->pcap->write_dl_crnti(payload, cur_grant.get_tbs()/8, cur_grant.get_rnti(), ack, tti);            
          }
        }
      } else {
        Warning("DL PID %d: Received duplicate TB. Discarting and retransmitting ACK\n", pid);
      }
      if (pid == HARQ_BCCH_PID || harq_entity->timers_db->get(mac::TIME_ALIGNMENT)->is_expired()) {
        // Do not generate ACK
        Debug("Not generating ACK\n");
      } else {
        if (cur_grant.is_temp_rnti()) {
          // Postpone ACK after contention resolution is resolved
          pending_ack       = ack; 
          pending_ul_buffer = phy_h->get_ul_buffer(tti+4);
          harq_entity->pending_ack_pid = pid; 
          memcpy(&pending_ack_grant, &cur_grant, sizeof(dl_sched_grant));
          Debug("ACK pending contention resolution\n");
        } else {
          Debug("Generating ACK\n");
          // Generate ACK
          srslte::ue::ul_buffer *ul_buffer = phy_h->get_ul_buffer(tti+4); 
          ul_buffer->generate_ack(ack, &cur_grant);                      
        }
      }
    } else {
      fprintf(stderr, "Error with DL grant. TBS (%d) exceeds payload buffer length (%d)\n", cur_grant.get_tbs(), max_payload_len);
    }          
  }
}
// Implement 5.3.2.2 
void dl_harq_entity::dl_harq_process::set_harq_info(srslte::ue::dl_sched_grant* new_grant)    {
  bool is_new_transmission = false; 

  bool is_new_tb = true; 
  if (srslte_tti_interval(new_grant->get_tti(), cur_grant.get_tti()) <= 8 && 
      new_grant->get_tbs() == cur_grant.get_tbs())
  {
    is_new_tb = false; 
  }
  
  if ((new_grant->get_ndi() != cur_grant.get_ndi() && !is_new_tb) || // NDI toggled for same TB
      is_new_tb                                                   || // is new TB
      (pid == HARQ_BCCH_PID && new_grant->get_rv() == 0))            // Broadcast PID and 1st TX (RV=0)
  {
    is_new_transmission = true; 
    Debug("Set HARQ Info for new transmission\n");
  } else {
    if (!is_new_tb) {
      Info("old_tbs=%d, new_tbs=%d, old_tti=%d new_tti=%d\n", cur_grant.get_tbs(), new_grant->get_tbs(), 
           cur_grant.get_tti(), new_grant->get_tti());
    }
    is_new_transmission = false; 
    Debug("Set HARQ Info for retransmission\n");
  }

  Info("DL PID %d: %s RV=%d, NDI=%d, LastNDI=%d, DCI %s\n", pid, is_new_transmission?"new TX":"reTX", new_grant->get_rv(), 
         new_grant->get_ndi(), cur_grant.get_ndi(), new_grant->get_dciformat_string());   
  
  if (is_new_transmission) {
    ack = false; 
    srslte_softbuffer_rx_reset(&softbuffer);
  }
  if (new_grant->get_tbs() <= max_payload_len) {
    memcpy(&cur_grant, new_grant, sizeof(srslte::ue::dl_sched_grant));        
  } else {
    Error("Error with DL grant. TBS (%d) exceeds payload buffer length (%d)\n", new_grant->get_tbs(), max_payload_len);
  }
}
bool dl_harq_entity::dl_harq_process::init(srslte_cell_t cell, uint32_t max_payload_len_, dl_harq_entity *parent) {
  max_payload_len = max_payload_len_; 
  if (srslte_softbuffer_rx_init(&softbuffer, cell)) {
    Error("Error initiating soft buffer\n");
    return false; 
  } else {
    is_initiated = true; 
    harq_entity = parent; 
    log_h = harq_entity->log_h; 
    payload = (uint8_t*)  srslte_vec_malloc(sizeof(uint8_t) * max_payload_len);
    return payload?true:false;
  }     
}

    
}
}