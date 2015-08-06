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
  for (int i=0;i<NOF_PDU_Q;i++) {
    pdu_q[i].init(8, MAX_PDU_LEN);
    used_q[i] = false; 
  }
}

void demux::init(phy_interface* phy_h_, rlc_interface_mac *rlc_, log* log_h_, timers* timers_db_)
{
  phy_h     = phy_h_; 
  log_h     = log_h_; 
  rlc       = rlc_;  
  timers_db = timers_db_;
}

void demux::set_uecrid_callback(bool (*callback)(void*,uint64_t), void *arg) {
  uecrid_callback     = callback;
  uecrid_callback_arg = arg; 
}

bool demux::get_uecrid_successful() {
  return is_uecrid_successful;
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
  uint32_t start=0; 
  if (idx) {
    start = *idx; 
  }
  for (uint8_t i=0;i<NOF_PDU_Q;i++) {
    if (!pdu_q[(i+start+1)%NOF_PDU_Q].isempty()) {
      if (idx) {
        *idx = (i+start+1)%NOF_PDU_Q; 
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

  uint8_t idx=0;
  if(find_unused_queue(&idx)) {
    if (idx > 0) {
      printf("Using queue %d for MAC PDU\n", idx);
    }
    used_q[idx] = true; 
    uint8_t *buff = (uint8_t*) pdu_q[idx].request();
    buff_header_t *head = (buff_header_t*) buff;
    head->idx = idx;   
    return &buff[sizeof(buff_header_t)]; 
  } else {
    Error("All DL buffers are full. Packet will be lost\n");
    return NULL; 
  }
}

void demux::push_buffer(uint8_t *buff, uint32_t nof_bytes) {
  buff_header_t *head = (buff_header_t*) (buff-sizeof(buff_header_t));
  if (head->idx < NOF_PDU_Q) {
    if (nof_bytes > 0) {
      if (!pdu_q[head->idx].push(nof_bytes)) {
        Warning("Full queue %d when pushing MAC PDU %d bytes\n", head->idx, nof_bytes);
      }
    } 
    used_q[head->idx] = false; 
  }
}

/* Demultiplexing of MAC PDU associated with SI-RNTI. The PDU passes through 
 * the MAC in transparent mode. 
 * Warning: this function sends the message to RLC now, since SI blocks do not 
 * require ACK feedback to be transmitted quickly. 
 */
void demux::push_pdu_bcch(uint8_t *buff, uint32_t nof_bytes) 
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
void demux::push_pdu_temp_crnti(uint8_t *buff, uint32_t nof_bytes) 
{
  // Unpack DLSCH MAC PDU 
  pending_mac_msg.init_rx(buff, nof_bytes);
  
  // Look for Contention Resolution UE ID 
  is_uecrid_successful = false; 
  while(pending_mac_msg.next() && !is_uecrid_successful) {
    if (pending_mac_msg.get()->ce_type() == sch_subh::CON_RES_ID) {
      Debug("Found Contention Resolution ID CE\n");
      is_uecrid_successful = uecrid_callback(uecrid_callback_arg, pending_mac_msg.get()->get_con_res_id());
    }
  }
  
  pending_mac_msg.reset();
  Debug("Saved MAC PDU with Temporal C-RNTI in buffer\n");
  push_buffer(buff, nof_bytes);
}

/* Demultiplexing of logical channels and dissassemble of MAC CE 
 * This function enqueues the packet and returns quicly because ACK 
 * deadline is important here. 
 */ 
void demux::push_pdu(uint8_t *buff, uint32_t nof_bytes)
{
  push_buffer(buff, nof_bytes);
}

void demux::release_buffer(uint8_t* ptr)
{
  uint8_t *addr = ptr - sizeof(buff_header_t); 
  for (int i=0;i<NOF_PDU_Q;i++) {
    if (pdu_q[i].request() == addr) {
      used_q[i] = false; 
      break;
    }
  }
}

void demux::process_pdus()
{
  uint32_t len; 
  uint8_t idx=0; 
  while(find_nonempty_queue(&idx)) {
    uint8_t *mac_pdu = (uint8_t*) pdu_q[idx].pop(&len);
    if (mac_pdu) {
      process_pdu(&mac_pdu[sizeof(buff_header_t)], len);
    }
    pdu_q[idx].release();
    idx++;
  } 
}

void demux::process_pdu(uint8_t *mac_pdu, uint32_t nof_bytes)
{
  // Unpack DLSCH MAC PDU 
  mac_msg.init_rx(mac_pdu, nof_bytes);
  //mac_msg.fprint(stdout);
  process_sch_pdu(&mac_msg);
  Debug("MAC PDU processed\n");
}

void demux::process_sch_pdu(sch_pdu *pdu_msg)
{  
  while(pdu_msg->next()) {
    if (pdu_msg->get()->is_sdu()) {
      // Route logical channel 
      Info("Delivering PDU for lcid=%d, %d bytes\n", pdu_msg->get()->get_sdu_lcid(), pdu_msg->get()->get_sdu_nbytes());
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
      // Do nothing
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
