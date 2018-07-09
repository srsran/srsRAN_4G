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

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

#include "srsue/hdr/mac/mux.h"
#include "srsue/hdr/mac/mac.h"

#include <set>
#include <algorithm>

namespace srsue {

mux::mux(uint8_t nof_harq_proc_) : pdu_msg(MAX_NOF_SUBHEADERS), pid_has_bsr(nof_harq_proc_), nof_harq_proc(nof_harq_proc_)
{
  pthread_mutex_init(&mutex, NULL);
  
  pending_crnti_ce = 0;

  log_h = NULL; 
  rlc   = NULL; 
  bsr_procedure = NULL; 
  phr_procedure = NULL;
  msg3_buff_start_pdu = NULL;

  msg3_flush();
}

void mux::init(rlc_interface_mac *rlc_, srslte::log *log_h_, bsr_interface_mux *bsr_procedure_, phr_proc *phr_procedure_)
{
  log_h      = log_h_;
  rlc        = rlc_;
  bsr_procedure = bsr_procedure_;
  phr_procedure = phr_procedure_;
  reset();
}

void mux::reset()
{
  for (uint32_t i=0;i<lch.size();i++) {
    lch[i].Bj = 0;
  }
  msg3_pending = false;
  pending_crnti_ce = 0;
}

bool mux::is_pending_any_sdu()
{
  for (uint32_t i=0;i<lch.size();i++) {
    if (rlc->get_buffer_state(lch[i].id)) {
      return true; 
    }
  }
  return false; 
}

bool mux::is_pending_sdu(uint32_t lch_id) {
  return rlc->get_buffer_state(lch_id)>0;  
}

int mux::find_lchid(uint32_t lcid) 
{
  for (uint32_t i=0;i<lch.size();i++) {
    if(lch[i].id == lcid) {
      return i;
    }
  }
  return -1; 
}

bool sortPriority(lchid_t u1, lchid_t u2) {
  return u1.priority < u2.priority; 
}

void mux::clear_lch(uint32_t lch_id)
{
  int pos = find_lchid(lch_id);
  if (pos >= 0) {
    lch.erase(lch.begin()+pos);
  } else {
    Error("Deleting logical channel id %d. Does not exist\n", lch_id);
  }
}

void mux::set_priority(uint32_t lch_id, uint32_t new_priority, int set_PBR, uint32_t set_BSD)
{
  int pos = find_lchid(lch_id);
    
  // Create new channel if it does not exist
  if (pos < 0) {
    lchid_t ch; 
    ch.id       = lch_id; 
    ch.priority = new_priority; 
    ch.BSD      = set_BSD; 
    ch.PBR      = set_PBR; 
    ch.Bj       = 0; 
    lch.push_back(ch);
  } else {
    lch[pos].priority = new_priority; 
    lch[pos].PBR      = set_PBR; 
    lch[pos].BSD      = set_BSD;     
  }
  
  // sort according to priority (increasing is lower priority)
  std::sort(lch.begin(), lch.end(), sortPriority); 
}

srslte::sch_subh::cetype bsr_format_convert(bsr_proc::bsr_format_t format) {
  switch(format) {
    case bsr_proc::LONG_BSR: 
      return srslte::sch_subh::LONG_BSR;
    case bsr_proc::TRUNC_BSR:
      return srslte::sch_subh::TRUNC_BSR;
    case bsr_proc::SHORT_BSR:
    default:
      return srslte::sch_subh::SHORT_BSR;
  }
}

void mux::pusch_retx(uint32_t tx_tti, uint32_t pid)
{
  if (pid_has_bsr[pid%nof_harq_proc]) {
    bsr_procedure->set_tx_tti(tx_tti);
  }
}

// Multiplexing and logical channel priorization as defined in Section 5.4.3
uint8_t* mux::pdu_get(uint8_t *payload, uint32_t pdu_sz, uint32_t tx_tti, uint32_t pid)
{
  pthread_mutex_lock(&mutex);
    
  // Update Bj
  for (uint32_t i=0;i<lch.size();i++) {
    // Add PRB unless it's infinity 
    if (lch[i].PBR >= 0) {
      lch[i].Bj += lch[i].PBR;
    }
    if (lch[i].Bj >= (int)lch[i].BSD) {
      lch[i].Bj = lch[i].BSD*lch[i].PBR; 
    }    
  }
  
  // Logical Channel Procedure
  bool is_rar = false;

  pdu_msg.init_tx(payload, pdu_sz, true);

  // MAC control element for C-RNTI or data from UL-CCCH
  if (!allocate_sdu(0, &pdu_msg, -1)) {
    if (pending_crnti_ce) {
      is_rar = true;
      if (pdu_msg.new_subh()) {
        if (!pdu_msg.get()->set_c_rnti(pending_crnti_ce)) {
          Warning("Pending C-RNTI CE could not be inserted in MAC PDU\n");
        }
      }
    }
  } else {
    is_rar = true;
  }
  pending_crnti_ce = 0; 
  
  bsr_proc::bsr_t bsr; 
  bool regular_bsr = bsr_procedure->need_to_send_bsr_on_ul_grant(pdu_msg.rem_size(), &bsr);
  bool bsr_is_inserted = false;
  
  // MAC control element for BSR, with exception of BSR included for padding;
  if (regular_bsr) {
    if (pdu_msg.new_subh()) {
      pdu_msg.get()->set_bsr(bsr.buff_size, bsr_format_convert(bsr.format));    
      bsr_is_inserted  = true; 
    }
  }

  // MAC control element for PHR
  if (phr_procedure) {
    float phr_value;
    if (phr_procedure->generate_phr_on_ul_grant(&phr_value)) {
      if (pdu_msg.new_subh()) {
        pdu_msg.get()->set_phr(phr_value);
      }
    }
  }

  if (!is_rar) {
    // Update buffer states for all logical channels
    int sdu_space = pdu_msg.get_sdu_space();
    for (uint32_t i=0;i<lch.size();i++) {
      lch[i].buffer_len = rlc->get_buffer_state(lch[i].id);
      lch[i].sched_len  = 0;
    }

    // data from any Logical Channel, except data from UL-CCCH;
    // first only those with positive Bj
    for (uint32_t i=0;i<lch.size();i++) {
      if (lch[i].id != 0) {
        if (sched_sdu(&lch[i], &sdu_space, (lch[i].PBR<0)?-1:lch[i].Bj) && lch[i].PBR >= 0) {
          lch[i].Bj -= lch[i].sched_len;
        }
      }
    }

    // If resources remain, allocate regardless of their Bj value
    for (uint32_t i=0;i<lch.size();i++) {
      if (lch[i].id != 0) {
        sched_sdu(&lch[i], &sdu_space, -1);
      }
    }

    // Maximize the grant utilization
    if (lch.size() > 0) {
      for (int i=(int)lch.size()-1;i>=0;i--) {
        if (lch[i].sched_len > 0) {
          lch[i].sched_len = -1;
          break;
        }
      }
    }
    for (uint32_t i=0;i<lch.size();i++) {
      if (lch[i].sched_len != 0) {
        allocate_sdu(lch[i].id, &pdu_msg, lch[i].sched_len);
      }
    }
  }

  if (!regular_bsr) {
    // Insert Padding BSR if not inserted Regular/Periodic BSR 
    if (bsr_procedure->generate_padding_bsr(pdu_msg.rem_size(), &bsr)) {
      if (pdu_msg.new_subh()) {
        pdu_msg.get()->set_bsr(bsr.buff_size, bsr_format_convert(bsr.format));
        bsr_is_inserted  = true; 
      }    
    }
  }
  
  log_h->debug("Assembled MAC PDU msg size %d/%d bytes\n", pdu_msg.get_pdu_len()-pdu_msg.rem_size(), pdu_sz);

  /* Generate MAC PDU and save to buffer */
  uint8_t *ret = pdu_msg.write_packet(log_h);   

  pid_has_bsr[pid%nof_harq_proc] = bsr_is_inserted;
  if (bsr_is_inserted) {
    bsr_procedure->set_tx_tti(tx_tti);
  }
  
  pthread_mutex_unlock(&mutex);


  return ret; 
}

void mux::append_crnti_ce_next_tx(uint16_t crnti) {
  pending_crnti_ce = crnti; 
}

bool mux::sched_sdu(lchid_t *ch, int *sdu_space, int max_sdu_sz) 
{
 
  if (*sdu_space > 0) {
    // Get n-th pending SDU pointer and length
    int sched_len = ch->buffer_len;     
    if (sched_len > 0) { // there is pending SDU to allocate
      if (sched_len > max_sdu_sz && max_sdu_sz >= 0) {
        sched_len = max_sdu_sz;
      }
      if (sched_len > *sdu_space) {
        sched_len = *sdu_space;
      }        

      log_h->debug("SDU:   scheduled lcid=%d, rlc_buffer=%d, allocated=%d/%d\n",
                   ch->id, ch->buffer_len, sched_len, sdu_space?*sdu_space:0);
      
      *sdu_space     -= sched_len; 
      ch->buffer_len -= sched_len; 
      ch->sched_len  += sched_len; 
      return true; 
    }
  }
  return false; 
}

bool mux::allocate_sdu(uint32_t lcid, srslte::sch_pdu* pdu_msg, int max_sdu_sz) 
{
 
  // Get n-th pending SDU pointer and length
  int sdu_len = rlc->get_buffer_state(lcid); 
  
  if (sdu_len > 0) { // there is pending SDU to allocate
    int buffer_state = sdu_len; 
    if (sdu_len > max_sdu_sz && max_sdu_sz >= 0) {
      sdu_len = max_sdu_sz;
    }
    int sdu_space = pdu_msg->get_sdu_space();
    if (sdu_len > sdu_space || max_sdu_sz < 0) {
      sdu_len = sdu_space;
    }
    if (sdu_len > MIN_RLC_SDU_LEN) {
      if (pdu_msg->new_subh()) { // there is space for a new subheader
        sdu_len = pdu_msg->get()->set_sdu(lcid, sdu_len, rlc);
        if (sdu_len > 0) { // new SDU could be added
          Debug("SDU:   allocated lcid=%d, rlc_buffer=%d, allocated=%d/%d, max_sdu_sz=%d, remaining=%d\n",
                 lcid, buffer_state, sdu_len, sdu_space, max_sdu_sz, pdu_msg->rem_size());
          return true;               
        } else {
          Warning("SDU:   rlc_buffer=%d, allocated=%d/%d, remaining=%d\n", 
               buffer_state, sdu_len, sdu_space, pdu_msg->rem_size());
          pdu_msg->del_subh();
        }
      } 
    }
  }
  return false; 
}

void mux::msg3_flush()
{
  if (log_h) {
    Debug("Msg3 buffer flushed\n");
  }
  msg3_has_been_transmitted = false;
  msg3_pending = false;
  bzero(msg3_buff, sizeof(MSG3_BUFF_SZ));
  msg3_buff_start_pdu = NULL;
}

bool mux::msg3_is_transmitted()
{
  return msg3_has_been_transmitted; 
}

void mux::msg3_prepare() {
  msg3_pending = true;
}

bool mux::msg3_is_pending() {
  return msg3_pending;
}

/* Returns a pointer to the Msg3 buffer */
uint8_t* mux::msg3_get(uint8_t *payload, uint32_t pdu_sz)
{
  if (pdu_sz < MSG3_BUFF_SZ - 32) {
    if (!msg3_buff_start_pdu) {
      msg3_buff_start_pdu = pdu_get(msg3_buff, pdu_sz, 0, 0);
      if (!msg3_buff_start_pdu) {
        Error("Moving PDU from Mux unit to Msg3 buffer\n");
        return NULL;
      }
      msg3_has_been_transmitted = true;
      msg3_pending = false;
    }
  } else {
    Error("Msg3 size (%d) is longer than internal msg3_buff size=%d, (see mux.h)\n", pdu_sz, MSG3_BUFF_SZ-32);
    return NULL;
  }
  memcpy(payload, msg3_buff_start_pdu, sizeof(uint8_t)*pdu_sz);
  msg3_has_been_transmitted = true;
  return payload;
}

  
}
