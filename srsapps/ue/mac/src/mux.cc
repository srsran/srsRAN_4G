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
  msg3_buff.init(mac::NOF_TTI_THREADS, MSG3_BUFF_SZ);
  pdu_buff.init(mac::NOF_TTI_THREADS, PDU_BUFF_SZ);
  bzero(nof_tx_pkts, sizeof(uint32_t) * mac_io::NOF_UL_LCH);
  pthread_mutex_init(&mutex, NULL);
  msg3_has_been_transmitted = false; 
  
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
   priority[i]        = i; 
   priority_sorted[i] = i; 
   PBR[i]             = -1; // -1 is infinite 
   BSD[i]             = 10;
   lchid_sorted[i]    = i; 
  }  
}

void mux::init(log *log_h_, mac_io *mac_io_h_, bsr_proc *bsr_procedure_)
{
  log_h      = log_h_;
  mac_io_h   = mac_io_h_;
  bsr_procedure = bsr_procedure_;
}

void mux::reset()
{
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    Bj[i] = 0; 
  }
}

bool mux::is_pending_ccch_sdu()
{
  return is_pending_sdu(0);
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

bool mux::is_pending_sdu(uint32_t lch_id) {
  lch_id += (uint32_t) mac_io::MAC_LCH_CCCH_UL;
  if (lch_id < mac_io::MAC_NOF_QUEUES) {
    return !mac_io_h->get(lch_id)->isempty();
  }
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
  pthread_mutex_unlock(&mutex);  
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
  // Acquire mutex. Will be released after a call to pdu_release
  pthread_mutex_lock(&mutex); 

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
int pkt_num=0;
bool mux::assemble_pdu(uint32_t pdu_sz_nbits) {

  uint8_t *buff = (uint8_t*) pdu_buff.request();
  if (!buff) {
    Error("Assembling PDU: Buffer is not available\n");
    return false; 
  }
  
  // Make sure pdu_sz is byte-aligned
  pdu_sz_nbits = 8*(pdu_sz_nbits/8);
  
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
  bool is_first = true; 
  if (!allocate_sdu(UL_IDX(mac_io::MAC_LCH_CCCH_UL), &pdu_msg, &is_first)) {
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
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    bool res = true; 
    while ((Bj[i] > 0 || PBR[i] < 0) && res) {
      res = allocate_sdu(lchid_sorted[i], &pdu_msg, &sdu_sz, &is_first);
      if (res && PBR[i] >= 0) {
        Bj[i] -= sdu_sz;         
      }
    }
  }

  // If resources remain, allocate regardless of their Bj value
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    while (allocate_sdu(lchid_sorted[i], &pdu_msg));   
  }

  /* Release all SDUs */
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    while(nof_tx_pkts[i] > 0) {
      mac_io_h->get(IO_IDX(i))->release();      
      nof_tx_pkts[i]--;
    }
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

  Debug("Assembled MAC PDU msg size %d/%d bytes\n", pdu_msg.size(), pdu_sz_nbits/8);
  //pdu_msg.fprint(stdout);

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
  return allocate_sdu(lcid, pdu_msg, NULL, NULL);
}
bool mux::allocate_sdu(uint32_t lcid, sch_pdu *pdu_msg, bool *is_first) 
{
  return allocate_sdu(lcid, pdu_msg, NULL, is_first);
}
bool mux::allocate_sdu(uint32_t lcid, sch_pdu *pdu_msg, uint32_t *sdu_sz, bool *is_first) 
{
  
  // Get n-th pending SDU pointer and length
  uint32_t buff_len = 0; 
  uint8_t *buff_ptr = (uint8_t*) mac_io_h->get(mac_io::MAC_LCH_CCCH_UL + lcid)->pop(&buff_len, nof_tx_pkts[lcid]);  

  uint32_t nbytes = (buff_len-1)/8 + 1; 

  if (buff_ptr && buff_len > 0) { // there is pending SDU to allocate
    if (sdu_sz) {
      *sdu_sz = buff_len; 
    }
    if (pdu_msg->new_subh()) { // there is space for a new subheader
      pdu_msg->next();
      if (pdu_msg->get()->set_sdu(lcid, buff_ptr, nbytes, is_first?*is_first:false)) { // new SDU could be added
        if (is_first) {
          *is_first = false;           
        }
        Info("Allocated SDU lcid=%d nbytes=%d\n", lcid, nbytes);
        // Increase number of pop'ed packets from queue
        nof_tx_pkts[lcid]++;      
        return true;               
      } else {
        if (pdu_msg->rem_size() > 10) {
          Info("Could not allocate SDU in current grant. SDU length: %d bytes. Grant space: %d bytes\n", nbytes, 
                pdu_msg->rem_size());
        }
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

/* Returns a pointer to the Msg3 buffer */
uint8_t* mux::msg3_pop(uint32_t TB_size)
{
  uint32_t len; 
  uint8_t *msg3 = (uint8_t*) msg3_buff.pop(&len);
  if (msg3) {
    if (len < TB_size) {
      // Pad with zeros without exceeding maximum buffer size 
      if (TB_size <= MSG3_BUFF_SZ) {
        bzero(&msg3[len], (TB_size-len)*sizeof(uint8_t));
      } else {
        Error("Requested TB size from Msg3 buffer exceeds buffer size (%d>%d)\n", TB_size, MSG3_BUFF_SZ);
        return NULL; 
      }
    }  
  }
  return msg3;
}

  
}
}
