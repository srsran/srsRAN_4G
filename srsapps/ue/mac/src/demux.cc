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
    
demux::demux() : mac_msg(20),pending_mac_msg(20)
{
  contention_resolution_id = 0; 
  pending_temp_rnti = false; 
  has_pending_contention_resolution_id = false; 
  sdu_handler_ = NULL; 
}

void demux::init(phy* phy_h_, log* log_h_, mac_io* mac_io_h_, timers* timers_db_, bool pcap_, FILE *pcap_file_)
{
  phy_h     = phy_h_; 
  log_h     = log_h_; 
  mac_io_h  = mac_io_h_;  
  timers_db = timers_db_;
  pcap      = pcap_;
  pcap_file = pcap_file_;
}

void demux::add_sdu_handler(sdu_handler* handler)
{
  sdu_handler_ = handler; 
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


/* Demultiplexing of MAC PDU associated with SI-RNTI. The PDU passes through 
 * the MAC in transparent mode 
 */
void demux::push_pdu_bcch(uint8_t *mac_pdu, uint32_t nof_bits) 
{
  mac_io_h->get(mac_io::MAC_LCH_BCCH_DL)->send(mac_pdu, nof_bits);
  Debug("Pushed BCCH MAC PDU in transparent mode\n");
}

/* Demultiplexing of MAC PDU associated with a Temporal C-RNTI. The PDU will 
 * remain in buffer until demultiplex_pending_pdu() is called. 
 * This features is provided to enable the Random Access Procedure to decide 
 * wether the PDU shall pass to upper layers or not, which depends on the 
 * Contention Resolution result
 */
void demux::push_pdu_temp_crnti(uint8_t *mac_pdu, uint32_t nof_bits) 
{
  if (!pending_temp_rnti) {
    // Unpack DLSCH MAC PDU 
    pending_mac_msg.init(nof_bits/8);
    pending_mac_msg.parse_packet(mac_pdu, pcap_file);
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
  } else {
    Warning("Error pushing PDU with Temporal C-RNTI: Another PDU is still in pending\n");
  }
}

/* Demultiplexing of logical channels and dissassemble of MAC CE */ 
void demux::push_pdu(uint8_t *mac_pdu, uint32_t nof_bits)
{
  // Unpack DLSCH MAC PDU 
  mac_msg.init(nof_bits/8);
  mac_msg.parse_packet(mac_pdu, pcap_file);
  //mac_msg.fprint(stdout);
  process_pdu(&mac_msg);
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
    process_pdu(&pending_mac_msg);
    discard_pending_pdu();
  } else {
    Error("Error demultiplex pending PDU: No pending PDU\n");
  }
}



void demux::process_pdu(sch_pdu *pdu_msg)
{  
  while(pdu_msg->next()) {
    if (pdu_msg->get()->is_sdu()) {
      // Route logical channel 
      if (pdu_msg->get()->get_sdu_lcid() <= mac_io::MAC_LCH_DTCH2_DL) {
        qbuff *dest_lch = mac_io_h->get(pdu_msg->get()->get_sdu_lcid());
        if (dest_lch) {
          dest_lch->send(pdu_msg->get()->get_sdu_ptr(), pdu_msg->get()->get_sdu_nbytes()*8);
          Debug("Sent MAC SDU len=%d bytes to lchid=%d\n",
                pdu_msg->get()->get_sdu_nbytes(), pdu_msg->get()->get_sdu_lcid());
          if (sdu_handler_) {
            sdu_handler_->notify_new_sdu(pdu_msg->get()->get_sdu_lcid());
            Debug("Notified SDU handler len=%d bytes to lchid=%d\n",
                  pdu_msg->get()->get_sdu_nbytes(), pdu_msg->get()->get_sdu_lcid());
          }

        } else {
          Error("Getting destination channel LCID=%d\n", pdu_msg->get()->get_sdu_lcid());
        }
      } else {
        Warning("Received SDU for unsupported LCID=%d\n", pdu_msg->get()->get_sdu_lcid());
      }
    // Process MAC Control Element
    } else {
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
