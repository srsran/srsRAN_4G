/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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

#include "srsenb/hdr/upper/rlc.h"
#include "srsenb/hdr/upper/common_enb.h"

namespace srsenb {
  
void rlc::init(pdcp_interface_rlc* pdcp_, rrc_interface_rlc* rrc_, mac_interface_rlc *mac_, 
               srslte::mac_interface_timers *mac_timers_, srslte::log* log_h_)
{
  pdcp       = pdcp_; 
  rrc        = rrc_, 
  log_h      = log_h_; 
  mac        = mac_; 
  mac_timers = mac_timers_; 

  pool       = srslte::byte_buffer_pool::get_instance();

  pthread_rwlock_init(&rwlock, NULL);
}

void rlc::stop()
{
  pthread_rwlock_wrlock(&rwlock);
  for(std::map<uint32_t, user_interface>::iterator iter=users.begin(); iter!=users.end(); ++iter) {
    clear_user(&iter->second);
  }
  users.clear();
  pthread_rwlock_unlock(&rwlock);
  pthread_rwlock_destroy(&rwlock);
}

void rlc::add_user(uint16_t rnti)
{
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti) == 0) {    
    srslte::rlc *obj = new srslte::rlc;     
    obj->init(&users[rnti], &users[rnti], &users[rnti], log_h, mac_timers, RB_ID_SRB0);
    users[rnti].rnti   = rnti; 
    users[rnti].pdcp   = pdcp; 
    users[rnti].rrc    = rrc; 
    users[rnti].rlc    = obj;
    users[rnti].parent = this; 
  }
  pthread_rwlock_unlock(&rwlock);
}

// Private unlocked deallocation of user
void rlc::clear_user(user_interface *ue)
{
  ue->rlc->stop();
  delete ue->rlc;
  ue->rlc = NULL;
}
void rlc::rem_user(uint16_t rnti)
{
  pthread_rwlock_wrlock(&rwlock);
  if (users.count(rnti)) {
    clear_user(&users[rnti]);
    users.erase(rnti);
  } else {
    log_h->error("Removing rnti=0x%x. Already removed\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
}

void rlc::clear_buffer(uint16_t rnti)
{
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti)) {
    users[rnti].rlc->empty_queue();
    for (int i=0;i<SRSLTE_N_RADIO_BEARERS;i++) {
      mac->rlc_buffer_state(rnti, i, 0, 0);      
    }
    log_h->info("Cleared buffer rnti=0x%x\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
}

void rlc::add_bearer(uint16_t rnti, uint32_t lcid)
{
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti)) {
    users[rnti].rlc->add_bearer(lcid);
  }
  pthread_rwlock_unlock(&rwlock);
}

void rlc::add_bearer(uint16_t rnti, uint32_t lcid, srslte::srslte_rlc_config_t cnfg)
{
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti)) {
    users[rnti].rlc->add_bearer(lcid, cnfg);
  }
  pthread_rwlock_unlock(&rwlock);
}

void rlc::add_bearer_mrb(uint16_t rnti, uint32_t lcid)
{
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti)) {
    users[rnti].rlc->add_bearer_mrb_enb(lcid);
  }
  pthread_rwlock_unlock(&rwlock);
}

void rlc::read_pdu_pcch(uint8_t* payload, uint32_t buffer_size)
{
  rrc->read_pdu_pcch(payload, buffer_size);
}

int rlc::read_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
{
  int ret;
  uint32_t tx_queue;

  pthread_rwlock_rdlock(&rwlock);
  if(users.count(rnti)) {
    if(rnti != SRSLTE_MRNTI) {
      ret = users[rnti].rlc->read_pdu(lcid, payload, nof_bytes);
      tx_queue = users[rnti].rlc->get_total_buffer_state(lcid);
    } else {
      ret = users[rnti].rlc->read_pdu_mch(lcid, payload, nof_bytes);
      tx_queue = users[rnti].rlc->get_total_mch_buffer_state(lcid);
    }
    // In the eNodeB, there is no polling for buffer state from the scheduler, thus
    // communicate buffer state every time a PDU is read
      
    uint32_t retx_queue = 0;
    log_h->debug("Buffer state PDCP: rnti=0x%x, lcid=%d, tx_queue=%d\n", rnti, lcid, tx_queue);
    mac->rlc_buffer_state(rnti, lcid, tx_queue, retx_queue);
  }else{
    ret = SRSLTE_ERROR;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

void rlc::write_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
{
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti)) {
    users[rnti].rlc->write_pdu(lcid, payload, nof_bytes);
    
    // In the eNodeB, there is no polling for buffer state from the scheduler, thus 
    // communicate buffer state every time a new PDU is written
    uint32_t tx_queue   = users[rnti].rlc->get_total_buffer_state(lcid);
    uint32_t retx_queue = 0; 
    log_h->debug("Buffer state PDCP: rnti=0x%x, lcid=%d, tx_queue=%d\n", rnti, lcid, tx_queue);
    mac->rlc_buffer_state(rnti, lcid, tx_queue, retx_queue);
  }
  pthread_rwlock_unlock(&rwlock);
}

void rlc::read_pdu_bcch_dlsch(uint32_t sib_index, uint8_t *payload)
{
  // RLC is transparent for BCCH
  rrc->read_pdu_bcch_dlsch(sib_index, payload);
}

void rlc::write_sdu(uint16_t rnti, uint32_t lcid, srslte::byte_buffer_t* sdu)
{
  
  uint32_t tx_queue;

  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti)) {
    if(rnti != SRSLTE_MRNTI){
      users[rnti].rlc->write_sdu_nb(lcid, sdu);
      tx_queue   = users[rnti].rlc->get_total_buffer_state(lcid);
    }else {
      users[rnti].rlc->write_sdu_mch(lcid, sdu);
      tx_queue   = users[rnti].rlc->get_total_mch_buffer_state(lcid);
    }
    // In the eNodeB, there is no polling for buffer state from the scheduler, thus 
    // communicate buffer state every time a new SDU is written
    
    uint32_t retx_queue = 0; 
    mac->rlc_buffer_state(rnti, lcid, tx_queue, retx_queue);
    log_h->info("Buffer state: rnti=0x%x, lcid=%d, tx_queue=%d\n", rnti, lcid, tx_queue);
  } else {
    pool->deallocate(sdu);
  }
  pthread_rwlock_unlock(&rwlock);
}

bool rlc::rb_is_um(uint16_t rnti, uint32_t lcid) {
  bool ret = false;
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti)) {
    ret = users[rnti].rlc->rb_is_um(lcid);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

void rlc::user_interface::max_retx_attempted()
{
  rrc->max_retx_attempted(rnti);
}

void rlc::user_interface::write_pdu(uint32_t lcid, srslte::byte_buffer_t* sdu)
{
  pdcp->write_pdu(rnti, lcid, sdu);
}

void rlc::user_interface::write_pdu_bcch_bch(srslte::byte_buffer_t* sdu)
{
  fprintf(stderr, "Error: Received BCCH from ue=%d\n", rnti);
}

void rlc::user_interface::write_pdu_bcch_dlsch(srslte::byte_buffer_t* sdu)
{
  fprintf(stderr, "Error: Received BCCH from ue=%d\n", rnti);
}

void rlc::user_interface::write_pdu_pcch(srslte::byte_buffer_t* sdu)
{
  fprintf(stderr, "Error: Received PCCH from ue=%d\n", rnti);
}

std::string rlc::user_interface::get_rb_name(uint32_t lcid)
{
  return std::string(rb_id_text[lcid]);
}

}
