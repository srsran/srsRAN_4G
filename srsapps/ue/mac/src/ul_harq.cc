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

#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/ue/mac/mac.h"
#include "srsapps/ue/mac/ul_harq.h"

namespace srslte {
  namespace ue {

  /***********************************************************
  * 
  * HARQ ENTITY
  * 
  *********************************************************/
    
bool ul_harq_entity::init(srslte_cell_t cell, mac_params *params_db_, log *log_h_, timers *timers_db_, mux *mux_unit_) {
  log_h     = log_h_; 
  mux_unit  = mux_unit_; 
  params_db = params_db_; 
  timers_db = timers_db_;
  for (uint32_t i=0;i<NOF_HARQ_PROC;i++) {
    if (!proc[i].init(cell, this)) {
      return false; 
    }
    proc[i].pid = i; 
  }
  return true; 
}
uint32_t ul_harq_entity::pidof(uint32_t tti) {
  return (uint32_t) tti%NOF_HARQ_PROC;  
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
// Called with no UL grant
void ul_harq_entity::run_tti(uint32_t tti, phy *phy_h) {
  run_tti(tti, NULL, phy_h);
}

// Implements Section 5.4.2.1
// Called with UL grant
void ul_harq_entity::run_tti(uint32_t tti, ul_sched_grant *grant, phy *phy_h)
{
  uint32_t tti_tx = (tti+4)%10240;
  uint32_t pid = pidof(tti_tx); 
  
  // Receive and route HARQ feedbacks
  int tti_harq = (int) tti - 4; 
  if (tti_harq < 0) {
    tti_harq += 10240; 
  }
  uint32_t pid_harq = pidof(tti_harq); 
  if (proc[pid_harq].has_grant() && proc[pid_harq].last_tx_tti() <= tti_harq) {
    proc[pid_harq].set_harq_feedback(phy_h->get_dl_buffer(tti)->decode_ack(proc[pid_harq].get_grant()));
  }

  if (grant) {
    if ((grant->is_temp_rnti() && grant->get_ndi() != proc[pid].get_ndi()) || 
        (grant->is_crnti() && !proc[pid].has_grant())                      ||
          grant->is_from_rar()) 
    {          
      // New transmission
      uint8_t* msg3_ptr = (uint8_t*) mux_unit->msg3_pop(grant->get_tbs());

      // Uplink grant in a RAR
      if (grant->is_from_rar()) {
        if (msg3_ptr) {
          proc[pid].generate_new_tx(tti_tx, msg3_ptr, true, grant, phy_h->get_ul_buffer(tti_tx));
          mux_unit->msg3_transmitted();
        } else {
          Warning("UL RAR grant available but no Msg3 on buffer\n");
        }
              
      // Normal UL grant
      } else {
        // Request a MAC PDU from the Multiplexing & Assemble Unit
        uint8_t* mac_pdu = mux_unit->pdu_pop(grant->get_tbs());
        if (mac_pdu) {            
          proc[pid].generate_new_tx(tti_tx, mac_pdu, false, grant, phy_h->get_ul_buffer(tti_tx));          
          mux_unit->pdu_release();
        } else {
          Warning("Uplink grant with MAC PDU available in Multiplex Unit\n");
        }
      }
    } else {
      // Adaptive Re-TX 
      proc[pid].generate_retx(tti_tx, grant, phy_h->get_ul_buffer(tti_tx));
    }        
  } else if (proc[pid].has_grant()) {
    // Non-Adaptive Re-Tx
    proc[pid].generate_retx(tti_tx, phy_h->get_ul_buffer(tti_tx));
  }
  
}



/***********************************************************
  * 
  * HARQ PROCESS
  * 
  *********************************************************/

static int rv_of_irv[4] = {0, 2, 3, 1}; 
static int irv_of_rv[4] = {0, 3, 1, 2}; 
    
ul_harq_entity::ul_harq_process::ul_harq_process() : cur_grant(0) {
  current_tx_nb = 0; 
  current_irv = 0; 
  is_initiated = false; 
  is_grant_configured = false; 
  tti_last_tx = 0; 
  bzero(&cur_grant, sizeof(ul_sched_grant));
}
void ul_harq_entity::ul_harq_process::reset() {
  current_tx_nb = 0; 
  current_irv = 0; 
  tti_last_tx = 0; 
  is_grant_configured = false; 
  bzero(&cur_grant, sizeof(ul_sched_grant));
  if (is_initiated) {
    srslte_softbuffer_tx_reset(&softbuffer);
  }
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

uint32_t ul_harq_entity::ul_harq_process::get_rv()
{
  return rv_of_irv[current_irv%4];
}

ul_sched_grant* ul_harq_entity::ul_harq_process::get_grant()
{
  return &cur_grant; 
}

void ul_harq_entity::ul_harq_process::set_harq_feedback(bool ack) {
  harq_feedback = ack; 
  // UL packet successfully delivered
  if (ack) {
    Info("UL PID %d: HARQ = ACK for UL transmission. Discarting TB.\n", pid);
    reset();
  } else {
    Info("UL PID %d: HARQ = NACK for UL transmission\n", pid);
  }
}

bool ul_harq_entity::ul_harq_process::init(srslte_cell_t cell, ul_harq_entity *parent) {
  if (srslte_softbuffer_tx_init(&softbuffer, cell)) {
    fprintf(stderr, "Error initiating soft buffer\n");
    return false; 
  } else {
    is_initiated = true; 
    harq_entity = parent; 
    log_h = harq_entity->log_h;
    return true; 
  }     
}

void ul_harq_entity::ul_harq_process::generate_retx(uint32_t tti_tx, ul_buffer* ul)
{
  generate_retx(tti_tx, NULL, ul);
}

// Retransmission with or w/o grant (Section 5.4.2.2)
void ul_harq_entity::ul_harq_process::generate_retx(uint32_t tti_tx, ul_sched_grant* grant, ul_buffer* ul)
{
  current_tx_nb++;    
  if (grant) {
    // HARQ entity requests an adaptive transmission
    memcpy(&cur_grant, grant, sizeof(ul_sched_grant));
    current_irv = irv_of_rv[grant->get_rv()%4];
    harq_feedback = false; 
    Info("UL PID %d: Adaptive retx=%d, RV=%d, TBS=%d, MCS=%d\n", pid, current_tx_nb, get_rv(), grant->get_tbs(), grant->get_mcs());
    generate_tx(tti_tx, NULL, ul);
  } else {
    Info("UL PID %d: Non-Adaptive retx=%d, RV=%d, TBS=%d, MCS=%d\n", pid, current_tx_nb, get_rv(), cur_grant.get_tbs(), cur_grant.get_mcs());
    // HARQ entity requests a non-adaptive transmission
    if (!harq_feedback) {
      generate_tx(tti_tx, NULL, ul);
    }
  }
  
  // On every Msg3 retransmission, restart mac-ContentionResolutionTimer as defined in Section 5.1.5
  if (is_msg3) {
    harq_entity->timers_db->get(mac::CONTENTION_TIMER)->reset();
  }
}

// New transmission (Section 5.4.2.2)
void ul_harq_entity::ul_harq_process::generate_new_tx(uint32_t tti_tx, uint8_t *pdu_payload, bool is_msg3_, ul_sched_grant* ul_grant, ul_buffer* ul)
{
  if (ul_grant && pdu_payload) {
    srslte_softbuffer_tx_reset(&softbuffer);
    memcpy(&cur_grant, ul_grant, sizeof(ul_sched_grant));        
    harq_feedback = false; 
    is_grant_configured = true; 
    current_tx_nb = 0; 
    current_irv = 0;         
    is_msg3 = is_msg3_;
    Info("UL PID %d: New TX%s, RV=%d, TBS=%d, MCS=%d, RNTI=%d\n", pid, is_msg3?" for Msg3":"", get_rv(), cur_grant.get_tbs(), 
         cur_grant.get_mcs(), cur_grant.get_rnti());
    generate_tx(tti_tx, pdu_payload, ul);
  }
}

// Transmission of pending frame (Section 5.4.2.2)
void ul_harq_entity::ul_harq_process::generate_tx(uint32_t tti_tx, uint8_t *pdu_payload, ul_buffer* ul)
{
  cur_grant.set_rv(get_rv());
  ul->set_current_tx_nb(current_tx_nb);
  ul->generate_data(&cur_grant, &softbuffer, pdu_payload);

  current_irv = (current_irv+1)%4;  
  tti_last_tx = tti_tx; 
  if (is_msg3) {
    if (current_tx_nb == harq_entity->params_db->get_param(mac_params::HARQ_MAXMSG3TX)) {
      Info("UL PID %d: Maximum number of ReTX for Msg3 reached (%d). Discarting TB.\n", pid, 
           harq_entity->params_db->get_param(mac_params::HARQ_MAXMSG3TX));
      reset();          
    }        
  } else {
    if (current_tx_nb == harq_entity->params_db->get_param(mac_params::HARQ_MAXTX)) {
      Info("UL PID %d: Maximum number of ReTX reached (%d). Discarting TB.\n", pid, 
           harq_entity->params_db->get_param(mac_params::HARQ_MAXTX));
      reset();
    }
  }

}

uint32_t ul_harq_entity::ul_harq_process::last_tx_tti()
{
  return tti_last_tx;
}


}
}