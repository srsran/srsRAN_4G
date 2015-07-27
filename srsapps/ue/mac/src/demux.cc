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


#include "srsapps/ue/mac/mac.h"
#include "srsapps/ue/mac/demux.h"

namespace srslte {
namespace ue {
    
demux::demux() : mac_msg(20), pending_mac_msg(20)
{
  contention_resolution_id = 0; 
  pending_temp_rnti = false; 
  has_pending_contention_resolution_id = false; 
  for (int i=0;i<NOF_PDU_Q;i++) {
    pdu_q[i].init(8, MAX_PDU_LEN);
    used_q[i] = false; 
  }
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cvar, NULL);

}

void demux::init(phy_interface* phy_h_, rlc_interface_mac *rlc_, log* log_h_, timers* timers_db_)
{
  phy_h     = phy_h_; 
  log_h     = log_h_; 
  rlc       = rlc_;  
  timers_db = timers_db_;
}

bool demux::is_temp_crnti_pending()
{
  return pending_temp_rnti; 
}

bool demux::is_contention_resolution_id_pending() {
  return has_pending_contention_resolution_id; 
}

uint64_t demux::get_contention_resolution_id()
{
  uint64_t x = contention_resolution_id; 
  contention_resolution_id = 0; 
  has_pending_contention_resolution_id = false; 
  return x; 
}

bool demux::find_unused_queue(uint8_t *idx) {
  for (uint8_t i=0;i<NOF_PDU_Q;i++) {
    if (!used_q[i]) {
      if (idx) {
        *idx = i; 
      }
      return true; 
    }
  }
  return false; 
}

// Read packets from queues in round robin
bool demux::find_nonempty_queue(uint8_t *idx) {
  for (uint8_t i=0;i<NOF_PDU_Q;i++) {
    if (!pdu_q[(i+*idx)%NOF_PDU_Q].isempty()) {
      if (idx) {
        *idx = i; 
      }
      return true; 
    }
  }
  return false; 
}

uint8_t* demux::request_buffer(uint32_t len)
{
  if (len >= MAX_PDU_LEN - sizeof(buff_header_t)) {
    return NULL; 
  }
  pthread_mutex_lock(&mutex); 
  uint8_t idx;
  while(!find_unused_queue(&idx)) {
    pthread_cond_wait(&cvar, &mutex);  
  }
  if (idx > 0) {
    Debug("Using queue %d for MAC PDU\n", idx);
  }
  used_q[idx] = true; 
  uint8_t *buff = (uint8_t*) pdu_q[idx].request();
  buff_header_t *head = (buff_header_t*) buff;
  head->idx = idx;   
  pthread_mutex_unlock(&mutex);
  
  return &buff[sizeof(buff_header_t)]; 
}

void demux::push_buffer(uint8_t *buff, uint32_t nof_bytes) {
  buff_header_t *head = (buff_header_t*) (buff-sizeof(buff_header_t));
  if (head->idx < NOF_PDU_Q) {
    pthread_mutex_lock(&mutex);
    if (nof_bytes > 0) {
      if (!pdu_q[head->idx].push(nof_bytes)) {
        Warning("Full queue %d when pushing MAC PDU %d bytes\n", head->idx, nof_bytes);
      }
    } 
    used_q[head->idx] = false; 
    pthread_cond_signal(&cvar);
    pthread_mutex_unlock(&mutex);
  }
}

/* Demultiplexing of MAC PDU associated with SI-RNTI. The PDU passes through 
 * the MAC in transparent mode. 
 * Warning: this function sends the message to RLC now, since SI blocks do not 
 * require ACK feedback to be transmitted quickly. 
 */
void demux::release_pdu_bcch(uint8_t *buff, uint32_t nof_bytes) 
{
  Debug("Pushed BCCH MAC PDU in transparent mode\n");
  rlc->write_pdu_bcch_dlsch(buff, nof_bytes);
  push_buffer(buff, 0);
}

/* Demultiplexing of MAC PDU associated with a Temporal C-RNTI. The PDU will 
 * remain in buffer until demultiplex_pending_pdu() is called. 
 * This features is provided to enable the Random Access Procedure to decide 
 * wether the PDU shall pass to upper layers or not, which depends on the 
 * Contention Resolution result. 
 * 
 * Warning: this function does some processing here assuming ACK deadline is not an 
 * issue here because Temp C-RNTI messages have small payloads
 */
void demux::release_pdu_temp_crnti(uint8_t *buff, uint32_t nof_bytes) 
{
  if (!pending_temp_rnti) {
    // Unpack DLSCH MAC PDU 
    pending_mac_msg.init(nof_bytes);
    pending_mac_msg.parse_packet(buff);
    //pending_mac_msg.fprint(stdout);
    
    // Look for Contention Resolution UE ID 
    while(pending_mac_msg.next()) {
      if (pending_mac_msg.get()->ce_type() == sch_subh::CON_RES_ID) {
        contention_resolution_id = pending_mac_msg.get()->get_con_res_id();
        has_pending_contention_resolution_id = true; 
        Debug("Found Contention Resolution ID CE\n");
      }
    }
    pending_mac_msg.reset();
    pending_temp_rnti = true; 
    Debug("Saved MAC PDU with Temporal C-RNTI in buffer\n");
    push_buffer(buff, 0);
  } else {
    Warning("Error pushing PDU with Temporal C-RNTI: Another PDU is still in pending\n");
  }
}

/* Demultiplexing of logical channels and dissassemble of MAC CE 
 * This function enqueues the packet and returns quicly because ACK 
 * deadline is important here. 
 */ 
void demux::release_pdu(uint8_t *buff, uint32_t nof_bytes)
{
  push_buffer(buff, nof_bytes);
}

void demux::process_pdus()
{
  uint32_t len; 
  uint8_t idx; 
  while(find_nonempty_queue(&idx)) {
    uint8_t *mac_pdu = (uint8_t*) pdu_q[idx].pop(&len);
    if (mac_pdu) {
      process_pdu(mac_pdu, len);
      pdu_q[idx].release();
    }
    idx++;
  } 
}

void demux::process_pdu(uint8_t *mac_pdu, uint32_t nof_bytes)
{
  // Unpack DLSCH MAC PDU 
  mac_msg.init(nof_bytes);
  mac_msg.parse_packet(mac_pdu);
  mac_msg.fprint(stdout);
  process_sch_pdu(&mac_msg);
  Debug("Normal MAC PDU processed\n");
}

void demux::discard_pending_pdu()
{
  pending_temp_rnti = false; 
  pending_mac_msg.reset();  
}

void demux::demultiplex_pending_pdu()
{
  if (pending_temp_rnti) {
    process_sch_pdu(&pending_mac_msg);
    discard_pending_pdu();
  } else {
    Error("Error demultiplex pending PDU: No pending PDU\n");
  }
}

void demux::process_sch_pdu(sch_pdu *pdu_msg)
{  
  while(pdu_msg->next()) {
    if (pdu_msg->get()->is_sdu()) {
      // Route logical channel 
      rlc->write_pdu(pdu_msg->get()->get_sdu_lcid(), pdu_msg->get()->get_sdu_ptr(), pdu_msg->get()->get_sdu_nbytes());
    } else {
      // Process MAC Control Element
      if (!process_ce(pdu_msg->get())) {
        Warning("Received Subheader with invalid or unkonwn LCID\n");
      }
    }
  }      
}

bool demux::process_ce(sch_subh *subh) {
  switch(subh->ce_type()) {
    case sch_subh::CON_RES_ID:
      contention_resolution_id = subh->get_c_rnti();
      Debug("Saved Contention Resolution ID=%d\n", contention_resolution_id);
      break;
    case sch_subh::TA_CMD:
      phy_h->set_timeadv(subh->get_ta_cmd());
      
      // Start or restart timeAlignmentTimer
      timers_db->get(mac::TIME_ALIGNMENT)->reset();
      timers_db->get(mac::TIME_ALIGNMENT)->run();
      Debug("Set TimeAdvance Command %d\n", subh->get_ta_cmd());
      break;
    case sch_subh::PADDING:
      break;
    default:
      Error("MAC CE 0x%x not supported\n", subh->ce_type());
      break;
  }
  return true; 
}


}
}
