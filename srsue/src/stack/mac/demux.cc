/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsue/hdr/stack/mac/demux.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/ue_phy_interfaces.h"

#define Error(fmt, ...) logger.error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) logger.warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) logger.info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) logger.debug(fmt, ##__VA_ARGS__)

namespace srsue {

demux::demux(srslog::basic_logger& logger) :
  logger(logger), mac_msg(20, logger), mch_mac_msg(20, logger), pending_mac_msg(20, logger), pdus(logger)
{}

void demux::init(phy_interface_mac_common*            phy_,
                 rlc_interface_mac*                   rlc_,
                 mac_interface_demux*                 mac_,
                 srsran::timer_handler::unique_timer* time_alignment_timer_)
{
  phy_h                = phy_;
  rlc                  = rlc_;
  mac                  = mac_;
  time_alignment_timer = time_alignment_timer_;
  pdus.init(this);
}

void demux::reset()
{
  // flush all buffered PDUs
  pdus.reset();
}

bool demux::get_uecrid_successful()
{
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
 * whether the PDU shall pass to upper layers or not, which depends on the
 * Contention Resolution result.
 *
 * Warning: this function does some processing here assuming ACK deadline is not an
 * issue here because Temp C-RNTI messages have small payloads
 */
void demux::push_pdu_temp_crnti(uint8_t* buff, uint32_t nof_bytes)
{
  if (nof_bytes > 0) {
    // Unpack DLSCH MAC PDU
    pending_mac_msg.init_rx(nof_bytes);
    pending_mac_msg.parse_packet(buff);

    // Look for Contention Resolution UE ID and TA commands
    is_uecrid_successful = false;
    while (pending_mac_msg.next()) {
      switch (pending_mac_msg.get()->dl_sch_ce_type()) {
        case srsran::dl_sch_lcid::CON_RES_ID:
          if (!is_uecrid_successful) {
            Debug("Found Contention Resolution ID CE");
            is_uecrid_successful = mac->contention_resolution_id_rcv(pending_mac_msg.get()->get_con_res_id());
          }
          break;
        case srsran::dl_sch_lcid::TA_CMD:
          parse_ta_cmd(pending_mac_msg.get(), 0);
          break;
        default:
          break;
      }
    }
    pending_mac_msg.reset();
    if (is_uecrid_successful) {
      Debug("Saved MAC PDU with Temporal C-RNTI in buffer");
      pdus.push(buff, nof_bytes, srsran::pdu_queue::DCH);
    } else {
      pdus.deallocate(buff);
    }
  } else {
    Warning("Trying to push PDU with payload size zero");
  }
}

/* Demultiplexing of logical channels and dissassemble of MAC CE
 * This function enqueues the packet and returns quickly because ACK
 * deadline is important here.
 */
void demux::push_pdu(uint8_t* buff, uint32_t nof_bytes, uint32_t tti)
{
  // Process Real-Time PDUs
  process_sch_pdu_rt(buff, nof_bytes, tti);

  return pdus.push(buff, nof_bytes, srsran::pdu_queue::DCH);
}

/* Demultiplexing of MAC PDU associated with SI-RNTI. The PDU passes through
 * the MAC in transparent mode.
 */
void demux::push_pdu_bcch(uint8_t* buff, uint32_t nof_bytes)
{
  pdus.push(buff, nof_bytes, srsran::pdu_queue::BCH);
}

void demux::push_pdu_mch(uint8_t* buff, uint32_t nof_bytes)
{
  uint8_t* mch_buffer_ptr = request_buffer(nof_bytes);
  memcpy(mch_buffer_ptr, buff, nof_bytes);
  pdus.push(mch_buffer_ptr, nof_bytes, srsran::pdu_queue::MCH);
  mch_buffer_ptr = NULL;
}

bool demux::process_pdus()
{
  return pdus.process_pdus();
}

void demux::process_pdu(uint8_t* mac_pdu, uint32_t nof_bytes, srsran::pdu_queue::channel_t channel, int ul_nof_prbs)
{
  Debug("Processing MAC PDU channel %d", channel);
  switch (channel) {
    case srsran::pdu_queue::DCH:
      // Unpack DLSCH MAC PDU
      mac_msg.init_rx(nof_bytes);
      mac_msg.parse_packet(mac_pdu);
      {
        fmt::memory_buffer buffer;
        mac_msg.to_string(buffer);
        Info("%s", srsran::to_c_str(buffer));
      }
      process_sch_pdu(&mac_msg);
      pdus.deallocate(mac_pdu);
      break;
    case srsran::pdu_queue::BCH:
      rlc->write_pdu_bcch_dlsch(mac_pdu, nof_bytes);
      break;
    case srsran::pdu_queue::MCH:
      mch_mac_msg.init_rx(nof_bytes);
      mch_mac_msg.parse_packet(mac_pdu);
      deallocate(mac_pdu);
      process_mch_pdu(&mch_mac_msg);
      // Process downlink MCH
      break;
  }
}

void demux::process_sch_pdu_rt(uint8_t* buff, uint32_t nof_bytes, uint32_t tti)
{
  srsran::sch_pdu mac_msg_rt(20, logger);

  mac_msg_rt.init_rx(nof_bytes);
  mac_msg_rt.parse_packet(buff);

  while (mac_msg_rt.next()) {
    if (mac_msg_rt.get()->is_sdu()) {
      // Ignore SDU
    } else {
      // Process MAC Control Element
      if (!process_ce(mac_msg_rt.get(), tti)) {
        Warning("Received Subheader with invalid or unknown LCID");
      }
    }
  }
}

void demux::process_sch_pdu(srsran::sch_pdu* pdu_msg)
{
  while (pdu_msg->next()) {
    if (pdu_msg->get()->is_sdu()) {
      bool route_pdu = true;
      if (pdu_msg->get()->get_sdu_lcid() == 0) {
        uint8_t* x   = pdu_msg->get()->get_sdu_ptr();
        uint32_t sum = 0;
        for (uint32_t i = 0; i < pdu_msg->get()->get_payload_size(); i++) {
          sum += x[i];
        }
        if (sum == 0) {
          route_pdu = false;
          Warning("Received all zero PDU");
        }
      }
      // Route logical channel
      if (route_pdu) {
        Debug(
            "Delivering PDU for lcid=%d, %d bytes", pdu_msg->get()->get_sdu_lcid(), pdu_msg->get()->get_payload_size());
        if (pdu_msg->get()->get_payload_size() < MAX_PDU_LEN) {
          rlc->write_pdu(
              pdu_msg->get()->get_sdu_lcid(), pdu_msg->get()->get_sdu_ptr(), pdu_msg->get()->get_payload_size());
        } else {
          char tmp[1024];
          srsran_vec_sprint_hex(tmp, sizeof(tmp), pdu_msg->get()->get_sdu_ptr(), 32);
          Error("PDU size %d exceeds maximum PDU buffer size, lcid=%d, hex=[%s]",
                pdu_msg->get()->get_payload_size(),
                pdu_msg->get()->get_sdu_lcid(),
                tmp);
        }
      }
    } else {
      // Ignore MAC Control Element
    }
  }
}
void demux::process_mch_pdu(srsran::mch_pdu* mch_msg)
{
  // disgarding headers that have already been processed
  while (mch_msg->next()) {
    if (srsran::mch_lcid::MCH_SCHED_INFO == mch_msg->get()->mch_ce_type()) {
      uint16_t stop;
      uint8_t  lcid;
      if (mch_msg->get()->get_next_mch_sched_info(&lcid, &stop)) {
        Info("MCH Sched Info: LCID: %d, Stop: %d, tti is %d ", lcid, stop, phy_h->get_current_tti());
      }
    }
    if (mch_msg->get()->is_sdu()) {
      uint32_t lcid = mch_msg->get()->get_sdu_lcid();

      if (lcid >= SRSRAN_N_MCH_LCIDS) {
        Error("Radio bearer id must be in [0:%d] - %d", SRSRAN_N_MCH_LCIDS, lcid);
        return;
      }
      Debug("Wrote MCH LCID=%d to RLC", lcid);
      if (1 == mch_lcids[lcid]) {
        rlc->write_pdu_mch(lcid, mch_msg->get()->get_sdu_ptr(), mch_msg->get()->get_payload_size());
      }
    }
  }
}

void demux::mch_start_rx(uint32_t lcid)
{
  if (lcid < 32) {
    Info("MCH Channel Setup: LCID=%d", lcid);
    mch_lcids[lcid] = 1;
  } else {
    Error("MCH Channel Setup: invalid LCID=%d", lcid);
  }
}

bool demux::process_ce(srsran::sch_subh* subh, uint32_t tti)
{
  switch (subh->dl_sch_ce_type()) {
    case srsran::dl_sch_lcid::CON_RES_ID:
      // Do nothing
      break;
    case srsran::dl_sch_lcid::TA_CMD:
      parse_ta_cmd(subh, tti);
      break;
    case srsran::dl_sch_lcid::SCELL_ACTIVATION: {
      uint32_t cmd = (uint32_t)subh->get_activation_deactivation_cmd();
      srsran::console("SCELL Activation / Deactivation CMD: %x\n", cmd);
      logger.info("SCELL Activation / Deactivation CMD: %x", cmd);
      phy_h->set_activation_deactivation_scell(cmd, tti);
      break;
    }
    case srsran::dl_sch_lcid::PADDING:
      break;
    default:
      Error("MAC CE 0x%x not supported", subh->lcid_value());
      break;
  }
  return true;
}

void demux::parse_ta_cmd(srsran::sch_subh* subh, uint32_t tti)
{
  phy_h->set_timeadv(tti, subh->get_ta_cmd());
  Info("Received TA=%d (%d/%d) ",
       subh->get_ta_cmd(),
       time_alignment_timer->time_elapsed(),
       time_alignment_timer->duration());
  // Start or restart timeAlignmentTimer only if set
  if (time_alignment_timer->is_set()) {
    time_alignment_timer->run();
  }
}

} // namespace srsue
