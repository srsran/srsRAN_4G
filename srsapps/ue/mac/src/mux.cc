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

#define IO_IDX(lch)     (lch + mac_io::MAC_LCH_CCCH_UL)
#define UL_IDX(lch)     (lch - mac_io::MAC_LCH_CCCH_UL)

mux::mux() : pdu_msg(20)
{
  msg3_buff.init(1, MSG3_BUFF_SZ);
  pdu_buff.init(1, PDU_BUFF_SZ);
  bzero(nof_tx_pkts, sizeof(uint32_t) * mac_io::NOF_UL_LCH);
  pthread_mutex_init(&mutex, NULL);
  msg3_has_been_transmitted = false; 
  
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
   priority[i]        = 1; 
   priority_sorted[i] = 1; 
   PBR[i]             = -1; // -1 is infinite 
   BSD[i]             = 10;
   lchid_sorted[i]      = i; 
  }  
}

void mux::init(log *log_h_, mac_io *mac_io_h_)
{
  log_h      = log_h_;
  mac_io_h   = mac_io_h_;
}

void mux::reset()
{
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    Bj[i] = 0; 
  }
}

bool mux::is_pending_any_sdu()
{
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    if (!mac_io_h->get(i)->isempty()) {
      return true; 
    }
  }
  return false; 
}

bool mux::is_pending_ccch_sdu()
{
  return !mac_io_h->get(mac_io::MAC_LCH_CCCH_UL)->isempty();
}

void mux::set_priority(uint32_t lch_id, uint32_t set_priority, int set_PBR, uint32_t set_BSD)
{
  pthread_mutex_lock(&mutex);
  if (lch_id < mac_io::NOF_UL_LCH) {
    priority[lch_id] = set_priority;
    PBR[lch_id]      = set_PBR;
    BSD[lch_id]      = set_BSD; 
    
    // Insert priority in sorted idx array
    int new_index = 0; 
    while(set_priority > priority_sorted[new_index] && new_index < mac_io::NOF_UL_LCH) {
      new_index++; 
    }
    int old_index = 0; 
    while(lch_id != lchid_sorted[old_index] && new_index < mac_io::NOF_UL_LCH) {
      old_index++;
    }
    if (new_index ==  mac_io::NOF_UL_LCH) {
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

void mux::pdu_release()
{
  pdu_buff.release();
}

bool mux::pdu_move_to_msg3(uint32_t pdu_sz)
{
  if (pdu_buff.isempty()) {
    if (assemble_pdu(pdu_sz)) {
      if (pdu_buff.pending_data() < MSG3_BUFF_SZ) {
        pdu_buff.move_to(&msg3_buff);        
        return true; 
      } else {
        pdu_buff.release();
        Error("Assembled PDU size exceeds Msg3 buffer size\n");
        return false; 
      }
    } else {
      Error("Assembling PDU\n");
      return false; 
    }    
  } else {
    Error("Generating PDU: PDU pending in buffer for transmission\n");
    return false; 
  }  
}

// Multiplexing and logical channel priorization as defined in Section 5.4.3
uint8_t* mux::pdu_pop(uint32_t pdu_sz)
{
  if (pdu_buff.isempty()) {
    if (assemble_pdu(pdu_sz)) {
      return (uint8_t*) pdu_buff.pop();
    } else {
      return NULL; 
    }    
  } else {
    Error("Generating PDU: PDU pending in buffer for transmission\n");
    return NULL; 
  }
}

void mux::append_crnti_ce_next_tx(uint16_t crnti) {
  pending_crnti_ce = crnti; 
}

bool mux::assemble_pdu(uint32_t pdu_sz_nbits) {

  uint8_t *buff = (uint8_t*) pdu_buff.request();
  if (!buff) {
    Error("Assembling PDU: Buffer is not available\n");
    return false; 
  }
  
  // Make sure pdu_sz is byte-aligned
  pdu_sz_nbits = 8*(pdu_sz_nbits/8);
  
  // Acquire mutex. Cannot change priorities, PBR or BSD after assemble finishes
  pthread_mutex_lock(&mutex); 
  
  // Update Bj
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {    
    // Add PRB unless it's infinity 
    if (PBR[i] >= 0) {
      Bj[i] += PBR[i];
    }
    if (Bj[i] >= BSD[i]) {
      Bj[i] = BSD[i]; 
    }    
  }
  
// Logical Channel Procedure
  
  uint32_t sdu_sz   = 0; 
 
  pdu_msg.init(pdu_sz_nbits/8, true);
  
  // MAC control element for C-RNTI or data from UL-CCCH
  if (!allocate_sdu(UL_IDX(mac_io::MAC_LCH_CCCH_UL), &pdu_msg)) {
    if (pending_crnti_ce) {
      if (pdu_msg.new_subh()) {
        pdu_msg.next();
        if (!pdu_msg.get()->set_c_rnti(pending_crnti_ce)) {
          Warning("Pending C-RNTI CE could not be inserted in MAC PDU\n");
        }
      }
    }
  }
  pending_crnti_ce = 0; 
  
  // MAC control element for BSR, with exception of BSR included for padding;
     // TODO
  // MAC control element for PHR
     // TODO
     
  // data from any Logical Channel, except data from UL-CCCH;  
  // first only those with positive Bj
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    bool res = true; 
    while ((Bj[i] > 0 || PBR[i] < 0) && res) {
      res = allocate_sdu(lchid_sorted[i], &pdu_msg, &sdu_sz);
      if (res && PBR[i] >= 0) {
        Bj[i] -= sdu_sz;         
      }
    }
  }
  
  // If resources remain, allocate regardless of their Bj value
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    while (allocate_sdu(lchid_sorted[i], &pdu_msg));   
  }
  
  // MAC control element for BSR included for padding.
     // TODO

  pthread_mutex_unlock(&mutex);

  /* Release all SDUs */
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    while(nof_tx_pkts[i] > 0) {
      mac_io_h->get(IO_IDX(i))->release();      
      nof_tx_pkts[i]--;
    }
  }

  Info("Assembled MAC PDU msg size %d bytes\n", pdu_msg.size());
  /* Generate MAC PDU and save to buffer */
  if (pdu_msg.write_packet(buff)) {
    pdu_buff.push(pdu_sz_nbits);
  } else {
    Error("Writing PDU message to packet\n");
    return false; 
  }  
  return true; 
}


bool mux::allocate_sdu(uint32_t lcid, sch_pdu *pdu_msg) 
{
  return allocate_sdu(lcid, pdu_msg, NULL);
}

bool mux::allocate_sdu(uint32_t lcid, sch_pdu *pdu_msg, uint32_t *sdu_sz) 
{
  
  // Get n-th pending SDU pointer and length
  uint32_t buff_len; 
  uint8_t *buff_ptr = (uint8_t*) mac_io_h->get(mac_io::MAC_LCH_CCCH_UL + lcid)->pop(&buff_len, nof_tx_pkts[lcid]);  

  if (buff_ptr) { // there is pending SDU to allocate
    if (pdu_msg->new_subh()) { // there is space for a new subheader
      pdu_msg->next();
      if (pdu_msg->get()->set_sdu(lcid, buff_ptr, buff_len/8)) { // new SDU could be added
        // Increase number of pop'ed packets from queue
        nof_tx_pkts[lcid]++;      
        return true;               
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

/* Returns a pointer to the Msg3 buffer */
uint8_t* mux::msg3_pop(uint32_t TB_size)
{
  uint32_t len; 
  uint8_t *msg3 = (uint8_t*) msg3_buff.pop(&len);
  if (len < TB_size) {
    // Pad with zeros without exceeding maximum buffer size 
    if (TB_size <= MSG3_BUFF_SZ) {
      bzero(&msg3[len], (TB_size-len)*sizeof(uint8_t));
    } else {
      Error("Requested TB size from Msg3 buffer exceeds buffer size (%d>%d)\n", TB_size, MSG3_BUFF_SZ);
      return NULL; 
    }
  }  
  return msg3;
}

  
}
}
