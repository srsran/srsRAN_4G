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
#include "srsapps/common/log.h"

#include "srsapps/ue/mac/mac.h"
#include "srsapps/ue/mac/ul_harq.h"

namespace srslte {
  namespace ue {

  /***********************************************************
  * 
  * HARQ ENTITY
  * 
  *********************************************************/
    
ul_harq_entity::ul_harq_entity() {
  proc = new ul_harq_process[NOF_HARQ_PROC]; // BCCH process is separate      
}
ul_harq_entity::~ul_harq_entity() {
delete proc; 
}
bool ul_harq_entity::init(srslte_cell_t cell, uint32_t max_payload_len, log *log_h_, timers *timers_db_, mux *mux_unit_) {
  log_h     = log_h_; 
  mux_unit  = mux_unit_; 
  timers_db = timers_db_;
  
  for (uint32_t i=0;i<NOF_HARQ_PROC;i++) {
    if (!proc[i].init(cell, max_payload_len, this)) {
      return false; 
    }
  }
  return true; 
}
void ul_harq_entity::set_maxHARQ_Tx(uint32_t maxHARQ_Tx, uint32_t maxHARQ_Msg3Tx) {
  for (uint32_t i=0;i<NOF_HARQ_PROC;i++) {
    proc[i].set_maxHARQ_Tx(maxHARQ_Tx, maxHARQ_Msg3Tx);
  }
}
uint32_t ul_harq_entity::pidof(uint32_t tti) {
  return tti%NOF_HARQ_PROC;
}
void ul_harq_entity::reset() {
  for (uint32_t i=0;i<NOF_HARQ_PROC;i++) {
    proc[i].reset();
  }
}
void ul_harq_entity::reset_ndi() {
  for (uint32_t i=0;i<NOF_HARQ_PROC;i++) {
    proc[i].reset_ndi();
  }
}
bool ul_harq_entity::is_sps(uint32_t pid) {
  return false; 
}

void ul_harq_entity::run_tti(uint32_t tti, phy *phy_) {
  run_tti(tti, NULL, phy_);
}

// Implements Section 5.4.2.1
void ul_harq_entity::run_tti(uint32_t tti, ul_sched_grant *grant, phy *phy_h)
{
  uint32_t pid = pidof(tti); 
  last_retx_is_msg3 = false; 
  
  if (grant) {
    if ((grant->is_temp_rnti() && grant->get_ndi() != proc[pid].get_ndi()) || 
        (grant->is_crnti() && !proc[pid].has_grant())                      ||
          grant->is_from_rar()) 
    {          
      // New transmission
      uint8_t* msg3_ptr = (uint8_t*) mux_unit->msg3_pop(tti, grant->get_tbs());
      
      // Uplink grant in a RAR
      if (msg3_ptr && grant->is_from_rar()) {
        proc[pid].generate_new_tx(msg3_ptr, true, grant, phy_h->get_ul_buffer(tti+4));
        mux_unit->msg3_release();
              
      // Normal UL grant
      } else {
        // Request a MAC PDU from the Multiplexing & Assemble Unit
        uint8_t* mac_pdu = mux_unit->pdu_pop(tti, grant->get_tbs());
        if (mac_pdu) {            
          proc[pid].generate_new_tx(mac_pdu, false, grant, phy_h->get_ul_buffer(tti+4));          
          mux_unit->pdu_release();
        } else {
          Warning("Uplink grant with MAC PDU available in Multiplex Unit\n");
        }
      }
    } else {
      // Adaptive Re-TX 
      proc[pid].generate_retx(grant, phy_h->get_ul_buffer(tti+4));
    }        
  } else if (proc[pid].has_grant()) {
    // Non-Adaptive Re-Tx
    proc[pid].generate_retx(phy_h->get_ul_buffer(tti+4));
  }
  
  // Receive and route HARQ feedbacks
  for (uint32_t i=0;i<NOF_HARQ_PROC;i++) {
    if (proc[pid].has_grant()) {
      proc[pid].set_harq_feedback(phy_h->get_dl_buffer(tti)->decode_ack(proc[pid].get_grant()));
    }
  }
}

bool ul_harq_entity::is_last_retx_msg3()
{
  return last_retx_is_msg3; 
}



/***********************************************************
  * 
  * HARQ PROCESS
  * 
  *********************************************************/

static int rv_of_irv[4] = {0, 2, 3, 1}; 
static int irv_of_rv[4] = {0, 3, 1, 2}; 
    
ul_harq_entity::ul_harq_process::ul_harq_process() : cur_grant(0) {
  payload = NULL; 
  max_payload_len = 0; 
  current_tx_nb = 0; 
  current_irv = 0; 
  is_grant_configured = false; 
  bzero(&cur_grant, sizeof(ul_sched_grant));
}
void ul_harq_entity::ul_harq_process::reset() {
  current_tx_nb = 0; 
  current_irv = 0; 
  is_grant_configured = false; 
  bzero(&cur_grant, sizeof(ul_sched_grant));
  srslte_softbuffer_tx_reset(&softbuffer);
}
bool ul_harq_entity::ul_harq_process::has_grant() {
  return is_grant_configured; 
}
void ul_harq_entity::ul_harq_process::reset_ndi() {
  ndi = false; 
}
bool ul_harq_entity::ul_harq_process::get_ndi()
{
  return ndi; 
}

ul_sched_grant* ul_harq_entity::ul_harq_process::get_grant()
{
  return &cur_grant; 
}

void ul_harq_entity::ul_harq_process::set_harq_feedback(bool ack) {
  harq_feedback = ack; 
}
void ul_harq_entity::ul_harq_process::set_maxHARQ_Tx(uint32_t maxHARQ_Tx_, uint32_t maxHARQ_Msg3Tx_) {
  maxHARQ_Tx = maxHARQ_Tx_; 
  maxHARQ_Msg3Tx = maxHARQ_Msg3Tx_; 
}

bool ul_harq_entity::ul_harq_process::init(srslte_cell_t cell, uint32_t max_payload_len_, ul_harq_entity *parent) {
  max_payload_len = max_payload_len_; 
  if (srslte_softbuffer_tx_init(&softbuffer, cell)) {
    fprintf(stderr, "Error initiating soft buffer\n");
    return false; 
  } else {
    harq_entity = parent; 
    log_h = harq_entity->log_h;
    payload = (uint8_t*)  srslte_vec_malloc(sizeof(uint8_t) * max_payload_len);
    return payload?true:false;
  }     
}

// Retransmission with or w/o grant (Section 5.4.2.2)
void ul_harq_entity::ul_harq_process::generate_retx(ul_sched_grant* grant, ul_buffer* ul)
{
  current_tx_nb++;
  
  if (grant) {
    // HARQ entity requests an adaptive transmission
    memcpy(&cur_grant, grant, sizeof(grant));
    current_irv = irv_of_rv[grant->get_rv()%4];
    harq_feedback = false; 
    generate_tx(ul);
  } else {
    // HARQ entity requests a non-adaptive transmission
    if (!harq_feedback) {
      generate_tx(ul);
    }
  }
  
  // On every Msg3 retransmission, restart mac-ContentionResolutionTimer as defined in Section 5.1.5
  if (is_msg3) {
    harq_entity->timers_db->get(mac::CONTENTION_TIMER)->reset();
  }
}

// New transmission (Section 5.4.2.2)
void ul_harq_entity::ul_harq_process::generate_new_tx(uint8_t *pdu_payload, bool is_msg3_, ul_sched_grant* ul_grant, ul_buffer* ul)
{
  if (ul_grant && pdu_payload && ul_grant->get_tbs() < max_payload_len) {
    current_tx_nb = 0; 
    current_irv = 0;         
    // Store MAC PDU in the HARQ buffer
    srslte_bit_pack_vector(pdu_payload, payload, ul_grant->get_tbs());
    // Store the uplink grant
    memcpy(&cur_grant, ul_grant, sizeof(ul_grant));        
    harq_feedback = false; 
    generate_tx(ul);
    is_grant_configured = true; 
    is_msg3 = is_msg3_;
  }
}

void ul_harq_entity::ul_harq_process::generate_retx(ul_buffer* ul)
{
  generate_retx(NULL, ul);
}

  // Transmission of pending frame (Section 5.4.2.2)
void ul_harq_entity::ul_harq_process::generate_tx(ul_buffer* ul)
{
  cur_grant.set_rv(rv_of_irv[current_irv%4]);
  ul->set_current_tx_nb(current_tx_nb);
  ul->generate_data(&cur_grant, &softbuffer, payload);
  current_irv = (current_irv+1)%4;  
  if (is_msg3) {
    if (current_tx_nb == maxHARQ_Msg3Tx) {
      reset();          
    }        
  } else {
    if (current_tx_nb == maxHARQ_Tx) {
      reset();
    }
  }

}

}
}