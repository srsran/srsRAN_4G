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


#include "srsapps/ue/mac/mux.h"
#include "srsapps/ue/mac/mac.h"

namespace srslte {
namespace ue {

mux::mux() : pdu_msg(20)
{
  msg3_buff.init(1, MSG3_BUFF_SZ);

  pthread_mutex_init(&mutex, NULL);
  msg3_has_been_transmitted = false; 
  
  for (int i=0;i<NOF_UL_LCH;i++) {
   priority[i]        = i; 
   priority_sorted[i] = i; 
   PBR[i]             = -1; // -1 is infinite 
   BSD[i]             = 10;
   lchid_sorted[i]    = i; 
  }  
}

void mux::init(rlc_interface_mac *rlc_, log *log_h_, bsr_proc *bsr_procedure_)
{
  log_h      = log_h_;
  rlc        = rlc_;
  bsr_procedure = bsr_procedure_;
}

void mux::reset()
{
  for (int i=0;i<NOF_UL_LCH;i++) {
    Bj[i] = 0; 
  }
}

bool mux::is_pending_ccch_sdu()
{
  return is_pending_sdu(0);
}

bool mux::is_pending_any_sdu()
{
  for (int i=0;i<NOF_UL_LCH;i++) {
    if (rlc->get_buffer_state(i)) {
      return true; 
    }
  }
  return false; 
}

bool mux::is_pending_sdu(uint32_t lch_id) {
  return rlc->get_buffer_state(lch_id)>0;  
}

void mux::set_priority(uint32_t lch_id, uint32_t set_priority, int set_PBR, uint32_t set_BSD)
{
  pthread_mutex_lock(&mutex);
  if (lch_id < NOF_UL_LCH) {
    priority[lch_id] = set_priority;
    PBR[lch_id]      = set_PBR;
    BSD[lch_id]      = set_BSD; 
    
    // Insert priority in sorted idx array
    int new_index = 0; 
    while(set_priority > priority_sorted[new_index] && new_index < NOF_UL_LCH) {
      new_index++; 
    }
    int old_index = 0; 
    while(lch_id != lchid_sorted[old_index] && new_index < NOF_UL_LCH) {
      old_index++;
    }
    if (new_index ==  NOF_UL_LCH) {
      Error("Can't find LchID=%d in sorted list\n", lch_id);
      return;
    }
    // Replace goes in one direction or the other 
    int add=new_index>old_index?1:-1;
    for (int i=old_index;i!=new_index;i+=add) {
      priority_sorted[i] = priority_sorted[i+add];
      lchid_sorted[i]    = lchid_sorted[i+add];
    }
    priority_sorted[new_index] = set_priority;
    lchid_sorted[new_index]    = lch_id; 
  }
  pthread_mutex_unlock(&mutex);
  
}

sch_subh::cetype bsr_format_convert(bsr_proc::bsr_format_t format) {
  switch(format) {
    case bsr_proc::LONG_BSR: 
      return sch_subh::LONG_BSR;
    case bsr_proc::SHORT_BSR: 
      return sch_subh::SHORT_BSR;
    case bsr_proc::TRUNC_BSR: 
      return sch_subh::TRUNC_BSR;   
  }
}


int pkt_num = 0; 

// Multiplexing and logical channel priorization as defined in Section 5.4.3
bool mux::pdu_get(uint8_t *payload, uint32_t pdu_sz)
{

  pthread_mutex_lock(&mutex);
    
  // Update Bj
  for (int i=0;i<NOF_UL_LCH;i++) {    
    // Add PRB unless it's infinity 
    if (PBR[i] >= 0) {
      Bj[i] += PBR[i];
    }
    if (Bj[i] >= BSD[i]) {
      Bj[i] = BSD[i]; 
    }    
  }
  
// Logical Channel Procedure
   
  pdu_msg.init(pdu_sz, true);
  
  // MAC control element for C-RNTI or data from UL-CCCH
  bool is_first = true; 
  if (!allocate_sdu(0, &pdu_msg, &is_first)) {
    if (pending_crnti_ce) {
      if (pdu_msg.new_subh()) {http://moodycamel.com/blog/2014/detailed-design-of-a-lock-free-queue
        pdu_msg.next();
        if (!pdu_msg.get()->set_c_rnti(pending_crnti_ce)) {
          Warning("Pending C-RNTI CE could not be inserted in MAC PDU\n");
        }
      }
    }
  }
  pending_crnti_ce = 0; 
  
  uint32_t bsr_payload_sz = bsr_procedure->need_to_send_bsr_on_ul_grant(pdu_msg.rem_size());
  bsr_proc::bsr_t bsr; 
  
  // MAC control element for BSR, with exception of BSR included for padding;
  sch_subh *bsr_subh = NULL;
  if (bsr_payload_sz) {
    Debug("Including BSR CE size %d\n", bsr_payload_sz);
    if (pdu_msg.new_subh()) {
      pdu_msg.next();
      bsr_subh = pdu_msg.get();
      pdu_msg.update_space_ce(bsr_payload_sz);
    }
  }
  pkt_num++;
  // MAC control element for PHR
  if (pkt_num == 2) {
    if (pdu_msg.new_subh()) {
      pdu_msg.next();
      pdu_msg.get()->set_phd(46);
    }
  }

  // data from any Logical Channel, except data from UL-CCCH;  
  // first only those with positive Bj
  uint32_t sdu_sz   = 0; 
  for (int i=0;i<NOF_UL_LCH;i++) {
    uint32_t lcid = lchid_sorted[i];
    if (lcid != 0) {
      bool res = true; 
      while ((Bj[lcid] > 0 || PBR[lcid] < 0) && res) {
        res = allocate_sdu(lcid, &pdu_msg, Bj[lcid], &sdu_sz, &is_first);
        if (res && PBR[lcid] >= 0) {
          Bj[lcid] -= sdu_sz;         
        }
      }
    }
  }

  // If resources remain, allocate regardless of their Bj value
  for (int i=0;i<NOF_UL_LCH;i++) {
    while (allocate_sdu(lchid_sorted[i], &pdu_msg));   
  }

  bool send_bsr = bsr_procedure->generate_bsr_on_ul_grant(pdu_msg.rem_size(), &bsr);
  // Insert Padding BSR if not inserted Regular/Periodic BSR 
  if (!bsr_payload_sz && send_bsr) {
    if (pdu_msg.new_subh()) {
      pdu_msg.next();
      bsr_subh = pdu_msg.get();
    }    
  }

  // And set the BSR 
  if (bsr_subh) {
    bsr_subh->set_bsr(bsr.buff_size, bsr_format_convert(bsr.format), bsr_payload_sz?false:true);    
  }

  Debug("Assembled MAC PDU msg size %d/%d bytes\n", pdu_msg.size(), pdu_sz);

  pthread_mutex_unlock(&mutex);

  /* Generate MAC PDU and save to buffer */
  if (!pdu_msg.write_packet(payload, rlc)) {
    Error("Writing PDU message to packet\n");
    return false; 
  } else {      
    return true; 
  }
}

void mux::append_crnti_ce_next_tx(uint16_t crnti) {
  pending_crnti_ce = crnti; 
}


bool mux::allocate_sdu(uint32_t lcid, sch_pdu *pdu_msg) 
{
  return allocate_sdu(lcid, pdu_msg, -1, NULL, NULL);
}
bool mux::allocate_sdu(uint32_t lcid, sch_pdu *pdu_msg, bool *is_first) 
{
  return allocate_sdu(lcid, pdu_msg, -1, NULL, is_first);
}
bool mux::allocate_sdu(uint32_t lcid, sch_pdu *pdu_msg, int max_sdu_sz, uint32_t *sdu_sz, bool *is_first) 
{
  
  // Get n-th pending SDU pointer and length
  uint32_t sdu_len = rlc->get_buffer_state(lcid); 

  if (sdu_len > 0) { // there is pending SDU to allocate
    if (sdu_len > max_sdu_sz && max_sdu_sz >= 0) {
      sdu_len = max_sdu_sz;
    }
    if (sdu_len > pdu_msg->rem_size() - pdu_msg->size_plus_header_sdu(sdu_len)) {
      sdu_len = pdu_msg->rem_size() - pdu_msg->size_plus_header_sdu(sdu_len);
    }
    if (pdu_msg->new_subh()) { // there is space for a new subheader
      pdu_msg->next();
      if (pdu_msg->get()->set_sdu(lcid, sdu_len, is_first?*is_first:false)) { // new SDU could be added
        if (is_first) {
          *is_first = false;           
        }
        if (sdu_sz) {
          *sdu_sz = sdu_len; 
        }
        Info("Allocated SDU lcid=%d nbytes=%d\n", lcid, sdu_len);
        return true;               
      } else {
        Error("Could not add SDU rem_size=%d, sdu_len=%d\n", pdu_msg->rem_size(), sdu_len);
        pdu_msg->del_subh();
      }
    } 
  }
  return false; 
}

void mux::msg3_flush()
{
  msg3_buff.flush();
  msg3_has_been_transmitted = false; 
}

void mux::msg3_transmitted()
{
  msg3_has_been_transmitted = true; 
}

bool mux::msg3_is_transmitted()
{
  return msg3_has_been_transmitted; 
}


bool mux::pdu_move_to_msg3(uint32_t pdu_sz)
{
  uint8_t *msg3 = (uint8_t*) msg3_buff.request();
  if (msg3) {
    if (pdu_get(msg3, pdu_sz)) {
      msg3_buff.push(pdu_sz);
      return true;       
    } else {
      Error("Assembling PDU\n");
    }    
  } else {
    Error("Generating PDU: PDU pending in buffer for transmission\n");
  }  
  return false; 
}

/* Returns a pointer to the Msg3 buffer */
bool mux::msg3_get(uint8_t *payload, uint32_t pdu_sz)
{
  if (pdu_move_to_msg3(pdu_sz)) {
    uint8_t *msg3 = (uint8_t*) msg3_buff.pop();
    if (msg3) {
      memcpy(payload, msg3, sizeof(uint8_t)*pdu_sz);
      msg3_buff.release();
      return true; 
    } else {
      Error("Generating Msg3\n");
    }
  }
  return false; 
}

  
}
}
