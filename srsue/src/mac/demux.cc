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

#include "srsue/hdr/mac/mac.h"
#include "srsue/hdr/mac/demux.h"
#include "srslte/interfaces/ue_interfaces.h"

namespace srsue {
    
demux::demux() : mac_msg(20), pending_mac_msg(20), rlc(NULL)
{
}

void demux::init(phy_interface_mac_common* phy_h_, rlc_interface_mac *rlc_, srslte::log* log_h_, srslte::timers::timer* time_alignment_timer_)
{
  phy_h     = phy_h_; 
  log_h     = log_h_; 
  rlc       = rlc_;
  time_alignment_timer = time_alignment_timer_;
  pdus.init(this, log_h);
}

void demux::set_uecrid_callback(bool (*callback)(void*,uint64_t), void *arg) {
  uecrid_callback     = callback;
  uecrid_callback_arg = arg; 
}

bool demux::get_uecrid_successful() {
  return is_uecrid_successful;
}

void demux::deallocate(uint8_t* payload_buffer_ptr)
{
  if (payload_buffer_ptr != bcch_buffer) {
    pdus.deallocate(payload_buffer_ptr);
  }
}
uint8_t* demux::request_buffer_bcch(uint32_t len)
{
  if (len < MAX_BCCH_PDU_LEN) {
    return bcch_buffer;
  } else {
    return NULL;
  }
}

uint8_t* demux::request_buffer(uint32_t len)
{  
  return pdus.request(len);
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
  if (nof_bytes > 0) {
    // Unpack DLSCH MAC PDU 
    pending_mac_msg.init_rx(nof_bytes);
    pending_mac_msg.parse_packet(buff);
    
    // Look for Contention Resolution UE ID 
    is_uecrid_successful = false; 
    while(pending_mac_msg.next() && !is_uecrid_successful) {
      if (pending_mac_msg.get()->ce_type() == srslte::sch_subh::CON_RES_ID) {
        Debug("Found Contention Resolution ID CE\n");
        is_uecrid_successful = uecrid_callback(uecrid_callback_arg, pending_mac_msg.get()->get_con_res_id());
      }
    }
    
    pending_mac_msg.reset();
    
    Debug("Saved MAC PDU with Temporal C-RNTI in buffer\n");
    
    pdus.push(buff, nof_bytes);
  } else {
    Warning("Trying to push PDU with payload size zero\n");
  }
}

/* Demultiplexing of logical channels and dissassemble of MAC CE 
 * This function enqueues the packet and returns quicly because ACK 
 * deadline is important here. 
 */ 
void demux::push_pdu(uint8_t *buff, uint32_t nof_bytes, uint32_t tstamp) {
  return pdus.push(buff, nof_bytes, tstamp);
}

/* Demultiplexing of MAC PDU associated with SI-RNTI. The PDU passes through
* the MAC in transparent mode.
* Warning: In this case function sends the message to RLC now, since SI blocks do not
* require ACK feedback to be transmitted quickly.
*/
void demux::push_pdu_bcch(uint8_t *buff, uint32_t nof_bytes, uint32_t tstamp) {
  rlc->write_pdu_bcch_dlsch(buff, nof_bytes);
}

bool demux::process_pdus()
{
  return pdus.process_pdus();
}

void demux::process_pdu(uint8_t *mac_pdu, uint32_t nof_bytes, uint32_t tstamp)
{
  // Unpack DLSCH MAC PDU 
  mac_msg.init_rx(nof_bytes);
  mac_msg.parse_packet(mac_pdu);

  process_sch_pdu(&mac_msg);
  //srslte_vec_fprint_byte(stdout, mac_pdu, nof_bytes);
  Debug("MAC PDU processed\n");
}

void demux::process_sch_pdu(srslte::sch_pdu *pdu_msg)
{  
  while(pdu_msg->next()) {
    if (pdu_msg->get()->is_sdu()) {
      bool route_pdu = true; 
      if (pdu_msg->get()->get_sdu_lcid() == 0) {
        uint8_t *x = pdu_msg->get()->get_sdu_ptr();
        uint32_t sum = 0; 
        for (int i=0;i<pdu_msg->get()->get_payload_size();i++) {
          sum += x[i];
        }
        if (sum == 0) {
          route_pdu = false; 
          Warning("Received all zero PDU\n");
        }
      }
      // Route logical channel 
      if (route_pdu) {
        Info("Delivering PDU for lcid=%d, %d bytes\n", pdu_msg->get()->get_sdu_lcid(), pdu_msg->get()->get_payload_size());
        if (pdu_msg->get()->get_payload_size() < MAX_PDU_LEN) {
          rlc->write_pdu(pdu_msg->get()->get_sdu_lcid(), pdu_msg->get()->get_sdu_ptr(), pdu_msg->get()->get_payload_size());
        } else {
          char tmp[1024];
          srslte_vec_sprint_hex(tmp, sizeof(tmp), pdu_msg->get()->get_sdu_ptr(), 32);
          Error("PDU size %d exceeds maximum PDU buffer size, lcid=%d, hex=[%s]\n",
                pdu_msg->get()->get_payload_size(), pdu_msg->get()->get_sdu_lcid(), tmp);
        }
      }
    } else {
      // Process MAC Control Element
      if (!process_ce(pdu_msg->get())) {
        Warning("Received Subheader with invalid or unkonwn LCID\n");
      }
    }
  }      
}

bool demux::process_ce(srslte::sch_subh *subh) {
  switch(subh->ce_type()) {
    case srslte::sch_subh::CON_RES_ID:
      // Do nothing
      break;
    case srslte::sch_subh::TA_CMD:
      phy_h->set_timeadv(subh->get_ta_cmd());
      Info("Received TA=%d\n", subh->get_ta_cmd());
      
      // Start or restart timeAlignmentTimer
      time_alignment_timer->reset();
      time_alignment_timer->run();
      break;
    case srslte::sch_subh::PADDING:
      break;
    default:
      Error("MAC CE 0x%x not supported\n", subh->ce_type());
      break;
  }
  return true; 
}


}
