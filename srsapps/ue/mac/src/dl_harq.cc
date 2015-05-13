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
  proc = new dl_harq_process[NOF_HARQ_PROC+1]; // BCCH process is separate
  for (uint32_t i=0;i<NOF_HARQ_PROC+1;i++) {
    proc[i].pid = i; 
  }  
  pending_ack_pid = -1; 
}
dl_harq_entity::~dl_harq_entity()
{
  delete proc; 
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
bool dl_harq_entity::is_sps(uint32_t pid)
{
  return false; 
}                                                            
void dl_harq_entity::set_harq_info(uint32_t tti, uint32_t pid, dl_sched_grant* grant)
{
  proc[pid%(NOF_HARQ_PROC+1)].set_harq_info(tti, grant);
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
  is_first_tx = true; 
  is_first_decoded = true; 
  is_initiated = false; 
  bzero(&cur_grant, sizeof(srslte::ue::dl_sched_grant));
  payload = NULL; 
  max_payload_len = 0; 
}  
  
void dl_harq_entity::dl_harq_process::reset() {
  is_first_tx = true; 
  is_first_decoded = true; 
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
  bool ack = false; 
  pending_ul_buffer = NULL; 
  
  if (payload) {
    if (cur_grant.get_tbs() <= max_payload_len) {
      Info("Decoding PDSCH data TBS=%d, RV=%d\n", cur_grant.get_tbs(), cur_grant.get_rv());
      if (dl_buffer->decode_data(&cur_grant, &softbuffer, payload)) {
        Info("Decoded OK\n");
        // RX OK
        Debug("Delivering PDU=%d bytes to Dissassemble and Demux unit\n", cur_grant.get_tbs()/8);
        if (pid == HARQ_BCCH_PID) {
          harq_entity->demux_unit->push_pdu_bcch(tti, payload, cur_grant.get_tbs()); 
          is_first_tx = true; 
        } else {
          if (is_first_decoded) {
            if (cur_grant.is_temp_rnti()) {
              harq_entity->demux_unit->push_pdu_temp_crnti(tti, payload, cur_grant.get_tbs());
            } else {
              harq_entity->demux_unit->push_pdu(tti, payload, cur_grant.get_tbs());
            }
            is_first_decoded = false; 
          }
          ack = true; 
        }            
      } else {
        Warning("Decoded Error\n");
        // RX NOK
        ack = false; 
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
void dl_harq_entity::dl_harq_process::set_harq_info(uint32_t tti, srslte::ue::dl_sched_grant* new_grant)    {
  bool is_new_transmission = false; 
  if (new_grant->get_ndi() && !cur_grant.get_ndi() || is_first_tx) {
    is_new_transmission = true; 
    is_first_decoded = true; 
    is_first_tx = false; 
    Debug("Set HARQ Info for new transmission\n");
  } else {
    is_new_transmission = false; 
    Debug("Set HARQ Info for retransmission\n");
  }
  if (is_new_transmission || cur_grant.get_tbs() != new_grant->get_tbs()) {
    Debug("Reset softbuffer RX\n");
    srslte_softbuffer_rx_reset(&softbuffer);
  }
  if (new_grant->get_tbs() <= max_payload_len) {
    memcpy(&cur_grant, new_grant, sizeof(srslte::ue::dl_sched_grant));        
  } else {
    fprintf(stderr, "Error with DL grant. TBS (%d) exceeds payload buffer length (%d)\n", new_grant->get_tbs(), max_payload_len);
  }
}
bool dl_harq_entity::dl_harq_process::init(srslte_cell_t cell, uint32_t max_payload_len_, dl_harq_entity *parent) {
  max_payload_len = max_payload_len_; 
  if (srslte_softbuffer_rx_init(&softbuffer, cell)) {
    fprintf(stderr, "Error initiating soft buffer\n");
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