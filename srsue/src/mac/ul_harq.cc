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

#include "srslte/common/log.h"
#include "mac/mac.h"
#include "mac/ul_harq.h"


  namespace srsue {

  /***********************************************************
  * 
  * HARQ ENTITY
  * 
  *********************************************************/
    
bool ul_harq_entity::init(srslte::log *log_h_, 
                          mac_interface_rrc::ue_rnti_t *rntis_, 
                          mac_interface_rrc::mac_cfg_t *mac_cfg_, 
                          srslte::timers *timers_db_, 
                          mux *mux_unit_) {
  log_h     = log_h_; 
  mux_unit  = mux_unit_; 
  mac_cfg   = mac_cfg_; 
  rntis     = rntis_; 
  timers_db = timers_db_;
  for (uint32_t i=0;i<NOF_HARQ_PROC;i++) {
    if (!proc[i].init(i, this)) {
      return false; 
    }
  }
  return true; 
}
uint32_t ul_harq_entity::pidof(uint32_t tti) {
  return (uint32_t) tti%NOF_HARQ_PROC;  
}

void ul_harq_entity::start_pcap(srslte::mac_pcap* pcap_)
{
  pcap = pcap_; 
}

void ul_harq_entity::reset() {
  for (uint32_t i=0;i<NOF_HARQ_PROC;i++) {
    proc[i].reset();
  }
  ul_sps_assig.clear();
}
void ul_harq_entity::reset_ndi() {
  for (uint32_t i=0;i<NOF_HARQ_PROC;i++) {
    proc[i].reset_ndi();
  }
}

int ul_harq_entity::get_current_tbs(uint32_t tti) {
  int tti_harq = (int) tti-4;
  if (tti_harq < 0) {
    tti_harq += 10240; 
  }
  uint32_t pid_harq = pidof(tti_harq); 
  return proc[pid_harq].get_current_tbs();
}

void ul_harq_entity::set_ack(uint32_t tti, bool ack) {
  int tti_harq = (int) tti - 4; 
  if (tti_harq < 0) {
    tti_harq += 10240; 
  }
  uint32_t pid_harq = pidof(tti_harq); 
  if (proc[pid_harq].has_grant() && (proc[pid_harq].last_tx_tti() <= (uint32_t)tti_harq)) {
    proc[pid_harq].set_harq_feedback(ack);
  }
}

void ul_harq_entity::harq_recv(uint32_t tti, bool ack, mac_interface_phy::tb_action_ul_t* action)
{
  set_ack(tti, ack);
  run_tti(tti, NULL, action);
}

// Implements Section 5.4.1 
void ul_harq_entity::new_grant_ul(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_ul_t* action)
{
  if (grant.rnti_type == SRSLTE_RNTI_USER || 
      grant.rnti_type == SRSLTE_RNTI_TEMP ||
      grant.rnti_type == SRSLTE_RNTI_RAR)
  {
    if (grant.rnti_type == SRSLTE_RNTI_USER && proc[pidof(grant.tti)].is_sps()) {
      grant.ndi = true; 
    }
    run_tti(grant.tti, &grant, action);
  } else if (grant.rnti_type == SRSLTE_RNTI_SPS) {
    if (grant.ndi) {
      grant.ndi = proc[pidof(grant.tti)].get_ndi();
      run_tti(grant.tti, &grant, action);
    } else {
      Info("Not implemented\n");
    }
  }
}

void ul_harq_entity::new_grant_ul_ack(mac_interface_phy::mac_grant_t grant, bool ack, mac_interface_phy::tb_action_ul_t* action)
{
  set_ack(grant.tti, ack);
  new_grant_ul(grant, action);
}



// Implements Section 5.4.2.1
// Called with UL grant
void ul_harq_entity::run_tti(uint32_t tti, mac_interface_phy::mac_grant_t *grant, mac_interface_phy::tb_action_ul_t* action)
{
  uint32_t tti_tx = (tti+4)%10240;
  proc[pidof(tti_tx)].run_tti(tti_tx, grant, action);
}


float ul_harq_entity::get_average_retx()
{
  return average_retx; 
}


/***********************************************************
  * 
  * HARQ PROCESS
  * 
  *********************************************************/

static int rv_of_irv[4] = {0, 2, 3, 1}; 
static int irv_of_rv[4] = {0, 3, 1, 2}; 
    
ul_harq_entity::ul_harq_process::ul_harq_process() {
  current_tx_nb = 0; 
  current_irv = 0; 
  is_initiated = false; 
  is_grant_configured = false; 
  tti_last_tx = 0; 
  bzero(&cur_grant, sizeof(mac_interface_phy::mac_grant_t));
}

void ul_harq_entity::ul_harq_process::reset() {
  current_tx_nb = 0; 
  current_irv = 0; 
  tti_last_tx = 0; 
  is_grant_configured = false; 
  bzero(&cur_grant, sizeof(mac_interface_phy::mac_grant_t));
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

void ul_harq_entity::ul_harq_process::set_harq_feedback(bool ack) {
  harq_feedback = ack; 
  // UL packet successfully delivered
  if (ack) {
    Info("UL %d:  HARQ = ACK for UL transmission. Discarting TB.\n", pid);
    reset();
  } else {
    Info("UL %d:  HARQ = NACK for UL transmission\n", pid);
  }
}

bool ul_harq_entity::ul_harq_process::init(uint32_t pid_, ul_harq_entity* parent) {
  if (srslte_softbuffer_tx_init(&softbuffer, 110)) {
    fprintf(stderr, "Error initiating soft buffer\n");
    return false; 
  } else {
    is_initiated = true; 
    harq_entity = parent; 
    log_h = harq_entity->log_h;
    pid = pid_;
    payload_buffer = (uint8_t*) srslte_vec_malloc(payload_buffer_len*sizeof(uint8_t));
    if (!payload_buffer) {
      Error("Allocating memory\n");
      return false; 
    }
    pdu_ptr = payload_buffer;
    return true; 
  }     
}

void ul_harq_entity::ul_harq_process::run_tti(uint32_t tti_tx, mac_interface_phy::mac_grant_t* grant, mac_interface_phy::tb_action_ul_t* action)
{   
  
  
  uint32_t max_retx;
  if (is_msg3) {
    max_retx = harq_entity->mac_cfg->rach.max_harq_msg3_tx;
  } else {
    max_retx = liblte_rrc_max_harq_tx_num[harq_entity->mac_cfg->main.ulsch_cnfg.max_harq_tx];
  }

  
  // Receive and route HARQ feedbacks
  if (grant) {
    if ((!(grant->rnti_type == SRSLTE_RNTI_TEMP) && grant->ndi != get_ndi()) ||
        (grant->rnti_type == SRSLTE_RNTI_USER && !has_grant())             ||
         grant->is_from_rar) 
    {          
      // New transmission

      // Uplink grant in a RAR
      if (grant->is_from_rar) {
        Debug("Getting Msg3 buffer payload, grant size=%d bytes\n", grant->n_bytes);
        pdu_ptr  = harq_entity->mux_unit->msg3_get(payload_buffer, grant->n_bytes);
        if (pdu_ptr) {
          generate_new_tx(tti_tx, true, grant, action);
        } else {
          Warning("UL RAR grant available but no Msg3 on buffer\n");
        }
              
      // Normal UL grant
      } else {
        // Request a MAC PDU from the Multiplexing & Assemble Unit
        pdu_ptr = harq_entity->mux_unit->pdu_get(payload_buffer, grant->n_bytes, tti_tx, pid);
        if (pdu_ptr) {            
          generate_new_tx(tti_tx, false, grant, action);          
        } else {
          Warning("Uplink grant but no MAC PDU in Multiplex Unit buffer\n");
        }
      }
    } else {
      // Adaptive Re-TX 
      if (current_tx_nb >= max_retx) {
        Info("UL %d:  Maximum number of ReTX reached (%d). Discarting TB.\n", pid, max_retx);
        reset();          
        action->expect_ack = false;
      } else {
        generate_retx(tti_tx, grant, action);
      }
    }        
  } else if (has_grant()) {
    // Non-Adaptive Re-Tx
    if (current_tx_nb >= max_retx) {
      Info("UL %d:  Maximum number of ReTX reached (%d). Discarting TB.\n", pid, max_retx);
      reset();          
      action->expect_ack = false;
    } else {
      generate_retx(tti_tx, action);
    }
  }
  if (harq_entity->pcap && grant) {
    if (grant->is_from_rar) {
      grant->rnti = harq_entity->rntis->temp_rnti;
    }
    harq_entity->pcap->write_ul_crnti(pdu_ptr, grant->n_bytes, grant->rnti, get_nof_retx(), tti_tx);
  }

  

}

int ul_harq_entity::ul_harq_process::get_current_tbs()
{
  return cur_grant.n_bytes*8;
}

void ul_harq_entity::ul_harq_process::generate_retx(uint32_t tti_tx, mac_interface_phy::tb_action_ul_t *action)
{
  generate_retx(tti_tx, NULL, action);
}

// Retransmission with or w/o grant (Section 5.4.2.2)
void ul_harq_entity::ul_harq_process::generate_retx(uint32_t tti_tx, mac_interface_phy::mac_grant_t *grant, 
                                                    mac_interface_phy::tb_action_ul_t *action)
{
  if (grant) {
    // HARQ entity requests an adaptive transmission
    if (grant->rv) {
      current_irv = irv_of_rv[grant->rv%4];
    }
    memcpy(&cur_grant, grant, sizeof(mac_interface_phy::mac_grant_t));
    harq_feedback = false; 
    Info("UL %d:  Adaptive retx=%d, RV=%d, TBS=%d\n", 
         pid, current_tx_nb, get_rv(), grant->n_bytes);
    generate_tx(tti_tx, action);
  } else {
    Info("UL %d:  Non-Adaptive retx=%d, RV=%d, TBS=%d\n", 
         pid, current_tx_nb, get_rv(), cur_grant.n_bytes);
    // HARQ entity requests a non-adaptive transmission
    if (!harq_feedback) {
      generate_tx(tti_tx, action);
    }
  }
  
  // On every Msg3 retransmission, restart mac-ContentionResolutionTimer as defined in Section 5.1.5
  if (is_msg3) {
    harq_entity->timers_db->get(mac::CONTENTION_TIMER)->reset();
  }
  
  harq_entity->mux_unit->pusch_retx(tti_tx, pid);
}

// New transmission (Section 5.4.2.2)
void ul_harq_entity::ul_harq_process::generate_new_tx(uint32_t tti_tx, bool is_msg3_, 
                                                      mac_interface_phy::mac_grant_t *grant, 
                                                      mac_interface_phy::tb_action_ul_t *action)
{
  if (grant) {
    
  // Compute average number of retransmissions per packet considering previous packet
  harq_entity->average_retx = SRSLTE_VEC_CMA((float) current_tx_nb, harq_entity->average_retx, harq_entity->nof_pkts++); 

    
    memcpy(&cur_grant, grant, sizeof(mac_interface_phy::mac_grant_t));
    harq_feedback = false; 
    is_grant_configured = true; 
    current_tx_nb = 0; 
    current_irv = 0;         
    is_msg3 = is_msg3_;
    Info("UL %d:  New TX%s, RV=%d, TBS=%d, RNTI=%d\n", 
         pid, is_msg3?" for Msg3":"", get_rv(), cur_grant.n_bytes, cur_grant.rnti);
    generate_tx(tti_tx, action);
  }
}

// Transmission of pending frame (Section 5.4.2.2)
void ul_harq_entity::ul_harq_process::generate_tx(uint32_t tti_tx, mac_interface_phy::tb_action_ul_t *action)
{
  action->current_tx_nb = current_tx_nb;
  current_tx_nb++;    
  action->expect_ack = true;
  action->rnti = is_msg3?harq_entity->rntis->temp_rnti:cur_grant.rnti; 
  action->rv = cur_grant.rv>0?cur_grant.rv:get_rv();
  action->softbuffer = &softbuffer; 
  action->tx_enabled = true; 
  action->payload_ptr = pdu_ptr; 
  memcpy(&action->phy_grant, &cur_grant.phy_grant, sizeof(srslte_phy_grant_t));
  
  current_irv = (current_irv+1)%4;  
  tti_last_tx = tti_tx; 
}

bool ul_harq_entity::ul_harq_process::is_sps()
{
  return false; 
}

uint32_t ul_harq_entity::ul_harq_process::last_tx_tti()
{
  return tti_last_tx;
}

uint32_t ul_harq_entity::ul_harq_process::get_nof_retx()
{
  return current_tx_nb;
}

}
