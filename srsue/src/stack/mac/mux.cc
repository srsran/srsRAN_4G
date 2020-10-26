/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#define Error(fmt, ...) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) log_h->debug(fmt, ##__VA_ARGS__)

#include "srsue/hdr/stack/mac/mux.h"
#include "srsue/hdr/stack/mac/mac.h"

#include <algorithm>
#include <set>

namespace srsue {

mux::mux(srslte::log_ref log_) : pdu_msg(MAX_NOF_SUBHEADERS, log_), log_h(log_)
{
  msg3_flush();
}

void mux::init(rlc_interface_mac* rlc_, bsr_interface_mux* bsr_procedure_, phr_proc* phr_procedure_)
{
  rlc           = rlc_;
  bsr_procedure = bsr_procedure_;
  phr_procedure = phr_procedure_;
  reset();
}

void mux::reset()
{
  std::lock_guard<std::mutex> lock(mutex);

  for (auto& channel : logical_channels) {
    channel.Bj = 0;
  }
  msg3_pending     = false;
  pending_crnti_ce = 0;
}

void mux::step()
{
  std::lock_guard<std::mutex> lock(mutex);

  // update Bj according to 36.321 Sec 5.4.3.1
  for (auto& channel : logical_channels) {
    // Add PRB unless it's infinity
    if (channel.PBR >= 0) {
      channel.Bj += channel.PBR; // PBR is in kByte/s, conversion in Byte and ms not needed
    }
    channel.Bj = SRSLTE_MIN((uint32_t)channel.Bj, channel.bucket_size);
    Debug("Update Bj: lcid=%d, Bj=%d\n", channel.lcid, channel.Bj);
  }
}

bool mux::is_pending_any_sdu()
{
  for (auto& channel : logical_channels) {
    if (rlc->has_data_locked(channel.lcid)) {
      return true;
    }
  }
  return false;
}

bool mux::has_logical_channel(const uint32_t& lcid)
{
  for (auto& channel : logical_channels) {
    if (channel.lcid == lcid) {
      return true;
    }
  }
  return false;
}

bool priority_compare(const logical_channel_config_t& u1, const logical_channel_config_t& u2)
{
  return u1.priority <= u2.priority;
}

// This is called by RRC (stack thread) during bearer addition
void mux::setup_lcid(const logical_channel_config_t& config)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (has_logical_channel(config.lcid)) {
    // update settings
    for (auto& channel : logical_channels) {
      if (channel.lcid == config.lcid) {
        channel = config;
        break;
      }
    }
    // warn user if there is another LCID with same prio
    for (auto& channel : logical_channels) {
      if (channel.priority == config.priority && channel.lcid != config.lcid) {
        log_h->warning("LCID %d and %d have same priority.\n", channel.lcid, config.lcid);
      }
    }
  } else {
    // add new entry
    logical_channels.push_back(config);
  }

  // sort according to priority (increasing is lower priority)
  std::sort(logical_channels.begin(), logical_channels.end(), priority_compare);
}

// mutex should be hold by caller
void mux::print_logical_channel_state(const std::string& info)
{
  std::string logline = info;

  for (auto& channel : logical_channels) {
    logline += "\n";
    logline += "- lcid=";
    logline += std::to_string(channel.lcid);
    logline += ", lcg=";
    logline += std::to_string(channel.lcg);
    logline += ", prio=";
    logline += std::to_string(channel.priority);
    logline += ", Bj=";
    logline += std::to_string(channel.Bj);
    logline += ", PBR=";
    logline += std::to_string(channel.PBR);
    logline += ", BSD=";
    logline += std::to_string(channel.BSD);
    logline += ", buffer_len=";
    logline += std::to_string(channel.buffer_len);
    logline += ", sched_len=";
    logline += std::to_string(channel.sched_len);
  }
  log_h->debug("%s\n", logline.c_str());
}

srslte::ul_sch_lcid bsr_format_convert(bsr_proc::bsr_format_t format)
{
  switch (format) {
    case bsr_proc::LONG_BSR:
      return srslte::ul_sch_lcid::LONG_BSR;
    case bsr_proc::TRUNC_BSR:
      return srslte::ul_sch_lcid::TRUNC_BSR;
    case bsr_proc::SHORT_BSR:
    default:
      return srslte::ul_sch_lcid::SHORT_BSR;
  }
}

// Multiplexing and logical channel priorization as defined in Section 5.4.3
uint8_t* mux::pdu_get(srslte::byte_buffer_t* payload, uint32_t pdu_sz)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Logical Channel Procedure
  payload->clear();
  pdu_msg.init_tx(payload, pdu_sz, true);

  // MAC control element for C-RNTI or data from UL-CCCH
  if (!allocate_sdu(0, &pdu_msg, pdu_sz)) {
    if (pending_crnti_ce) {
      if (pdu_msg.new_subh()) {
        if (!pdu_msg.get()->set_c_rnti(pending_crnti_ce)) {
          Warning("Pending C-RNTI CE could not be inserted in MAC PDU\n");
        }
      }
    }
  } else {
    if (pending_crnti_ce) {
      Warning("Pending C-RNTI CE was not inserted because message was for CCCH\n");
    }
  }
  pending_crnti_ce = 0;

  // Calculate pending UL data per LCID and LCG as well as the total amount
  bsr_proc::bsr_t bsr                = {}; // pending data per LCG
  int             total_pending_data = 0;
  int             last_sdu_len       = 0;
  for (auto& channel : logical_channels) {
    channel.sched_len  = 0; // reset sched_len for LCID
    channel.buffer_len = rlc->get_buffer_state(channel.lcid);
    total_pending_data += channel.buffer_len + sch_pdu::size_header_sdu(channel.buffer_len);
    last_sdu_len = channel.buffer_len;
    bsr.buff_size[channel.lcg] += channel.buffer_len;
  }
  if (total_pending_data > 0) {
    // remove length byte(s) of last SDU
    total_pending_data -= sch_pdu::size_header_sdu(last_sdu_len);
    total_pending_data++; // the R/R/E/LCID subheader is still needed
  }

  bool regular_bsr = bsr_procedure->need_to_send_bsr_on_ul_grant(pdu_msg.rem_size(), total_pending_data, &bsr);
  int  bsr_idx     = -1; // subheader index of BSR (needed to update content later)

  // MAC control element for BSR, with exception of BSR included for padding;
  if (regular_bsr) {
    if (pdu_msg.new_subh()) {
      if (pdu_msg.get()->set_bsr(bsr.buff_size, bsr_format_convert(bsr.format)) == false) {
        pdu_msg.del_subh();
        regular_bsr = false; // make sure we don't update the BSR content later on
      } else {
        // BSR allocation was successful, store subheader index
        bsr_idx = pdu_msg.get_current_idx();
      }
    }
  }

  // MAC control element for PHR
  if (phr_procedure) {
    float phr_value;
    if (phr_procedure->generate_phr_on_ul_grant(&phr_value)) {
      if (pdu_msg.new_subh()) {
        if (pdu_msg.get()->set_phr(phr_value) == false) {
          pdu_msg.del_subh();
        }
      }
    }
  }

  int32_t sdu_space = pdu_msg.get_sdu_space(); // considers first SDU's header is only one byte

  // data from any Logical Channel, except data from UL-CCCH;
  // first only those with positive Bj
  uint32_t last_sdu_subheader_len = 0; // needed to keep track of added SDUs and actual required subheades
  for (auto& channel : logical_channels) {
    int max_sdu_sz = (channel.PBR < 0) ? -1 : channel.Bj; // this can be zero if no PBR has been allocated
    if (max_sdu_sz != 0) {
      if (sched_sdu(&channel, &sdu_space, max_sdu_sz)) {
        channel.Bj -= channel.sched_len;
        // account for (possible) subheader needed for next SDU
        last_sdu_subheader_len = SRSLTE_MIN((uint32_t)sdu_space, sch_pdu::size_header_sdu(channel.sched_len));
        sdu_space -= last_sdu_subheader_len;
      }
    }
  }
  if (last_sdu_subheader_len > 0) {
    // Unused last subheader, give it back ..
    sdu_space += last_sdu_subheader_len;
  }

  print_logical_channel_state("First round of allocation:");

  // If resources remain, allocate regardless of their Bj value
  for (auto& channel : logical_channels) {
    if (channel.lcid != 0) {
      // allocate subheader if this LCID has not been scheduled yet but there is data to send
      if (channel.sched_len == 0 && channel.buffer_len > 0) {
        sdu_space -= last_sdu_subheader_len;
      }
      sched_sdu(&channel, &sdu_space, -1);
    }
  }

  print_logical_channel_state("Second round of allocation:");

  for (auto& channel : logical_channels) {
    if (channel.sched_len != 0) {
      allocate_sdu(channel.lcid, &pdu_msg, channel.sched_len);

      // update BSR according to allocation
      bsr.buff_size[channel.lcg] -= channel.sched_len;
    }
  }

  if (regular_bsr && bsr_idx >= 0 && pdu_msg.get(bsr_idx) != nullptr) {
    // update BSR content
    pdu_msg.get(bsr_idx)->update_bsr(bsr.buff_size, bsr_format_convert(bsr.format));
  } else {
    // Insert Padding BSR if not inserted Regular/Periodic BSR
    if (bsr_procedure->generate_padding_bsr(pdu_msg.rem_size(), &bsr)) {
      if (pdu_msg.new_subh()) {
        if (pdu_msg.get()->set_bsr(bsr.buff_size, bsr_format_convert(bsr.format)) == false) {
          pdu_msg.del_subh();
        }
      }
    }
  }

  // Update buffers in BSR procedure after packing entire TTI
  bsr_procedure->update_bsr_tti_end(&bsr);

  // Generate MAC PDU and save to buffer
  uint8_t* ret = pdu_msg.write_packet(log_h);
  Info("%s\n", pdu_msg.to_string().c_str());
  Debug("Assembled MAC PDU msg size %d/%d bytes\n", pdu_msg.get_pdu_len() - pdu_msg.rem_size(), pdu_sz);

  return ret;
}

void mux::append_crnti_ce_next_tx(uint16_t crnti)
{
  pending_crnti_ce = crnti;
}

bool mux::sched_sdu(logical_channel_config_t* ch, int* sdu_space, int max_sdu_sz)
{
  if (sdu_space != nullptr && *sdu_space > 0) {
    // Get n-th pending SDU pointer and length
    int sched_len = ch->buffer_len;
    if (sched_len > 0) { // there is pending SDU to allocate
      if (sched_len > max_sdu_sz && max_sdu_sz >= 0) {
        sched_len = max_sdu_sz;
      }
      if (sched_len > *sdu_space) {
        sched_len = *sdu_space;
      }

      *sdu_space -= sched_len; // UL-SCH subheader size is accounted for in the calling function

      Debug("SDU:   scheduled lcid=%d, rlc_buffer=%d, allocated=%d\n", ch->lcid, ch->buffer_len, sched_len);

      ch->buffer_len -= sched_len;
      ch->sched_len += sched_len;
      return true;
    }
  }
  return false;
}

bool mux::allocate_sdu(uint32_t lcid, srslte::sch_pdu* pdu_msg, int max_sdu_sz)
{
  bool sdu_added    = false;
  int  sdu_space    = max_sdu_sz;
  int  buffer_state = rlc->get_buffer_state(lcid);

  while (buffer_state > 0 && sdu_space > 0) { // there is pending SDU to allocate
    int requested_sdu_len = SRSLTE_MIN(buffer_state, sdu_space);

    if (pdu_msg->new_subh()) { // there is space for a new subheader
      int sdu_len = pdu_msg->get()->set_sdu(lcid, requested_sdu_len, rlc);
      if (sdu_len > 0) { // new SDU could be added
        Debug("SDU:   allocated lcid=%d, buffer_state=%d, request_sdu_len=%d, allocated=%d/%d, max_sdu_sz=%d, "
              "remaining=%d\n",
              lcid,
              buffer_state,
              requested_sdu_len,
              sdu_len,
              sdu_space,
              max_sdu_sz,
              pdu_msg->rem_size());
        sdu_space -= sdu_len;
        sdu_added = true;

        buffer_state = rlc->get_buffer_state(lcid);
      } else {
        Debug("Couldn't allocate new SDU (buffer_state=%d, requested_sdu_len=%d, sdu_len=%d, sdu_space=%d, "
              "remaining=%d, get_sdu_space=%d)\n",
              buffer_state,
              requested_sdu_len,
              sdu_len,
              sdu_space,
              pdu_msg->rem_size(),
              pdu_msg->get_sdu_space());
        pdu_msg->del_subh();
        // prevent endless loop
        break;
      }
    } else {
      Debug("Couldn't add new MAC subheader (buffer_state=%d, requested_sdu_len=%d, sdu_space=%d, remaining=%d)\n",
            buffer_state,
            requested_sdu_len,
            sdu_space,
            pdu_msg->rem_size());
      // prevent endless loop
      break;
    }
  }
  return sdu_added;
}

void mux::msg3_flush()
{
  if (log_h) {
    Debug("Msg3 buffer flushed\n");
  }
  msg3_buff.clear();
  msg3_has_been_transmitted = false;
  msg3_pending              = false;
}

bool mux::msg3_is_transmitted()
{
  return msg3_has_been_transmitted;
}

void mux::msg3_prepare()
{
  msg3_has_been_transmitted = false;
  msg3_pending              = true;
}

bool mux::msg3_is_pending()
{
  return msg3_pending;
}

bool mux::msg3_is_empty()
{
  return msg3_buff.N_bytes == 0;
}

/* Returns a pointer to the Msg3 buffer */
uint8_t* mux::msg3_get(srslte::byte_buffer_t* payload, uint32_t pdu_sz)
{
  if (pdu_sz < msg3_buff.get_tailroom()) {
    if (msg3_is_empty()) {
      if (!pdu_get(&msg3_buff, pdu_sz)) {
        Error("Moving PDU from Mux unit to Msg3 buffer\n");
        return NULL;
      }
      msg3_pending = false;
    }
    *payload                  = msg3_buff;
    msg3_has_been_transmitted = true;
    return payload->msg;
  } else {
    Error("Msg3 size exceeds buffer\n");
    return nullptr;
  }
}

} // namespace srsue
