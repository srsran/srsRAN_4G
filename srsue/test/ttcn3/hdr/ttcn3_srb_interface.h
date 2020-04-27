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

#ifndef SRSUE_TTCN3_SRB_INTERFACE_H
#define SRSUE_TTCN3_SRB_INTERFACE_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/mac/pdu.h"
#include "ttcn3_interfaces.h"
#include "ttcn3_port_handler.h"
#include <srslte/interfaces/ue_interfaces.h>

using namespace srslte;

// The SRB interface
class ttcn3_srb_interface : public ttcn3_port_handler
{
public:
  ttcn3_srb_interface() : pool(byte_buffer_pool::get_instance()) {}
  ~ttcn3_srb_interface() = default;

  int init(ss_srb_interface* syssim_, srslte::log* log_, std::string net_ip_, uint32_t net_port_)
  {
    syssim   = syssim_;
    log      = log_;
    net_ip   = net_ip_;
    net_port = net_port_;
    initialized = true;
    log->debug("Initialized.\n");
    return port_listen();
  }

  void tx(unique_byte_buffer_t pdu)
  {
    if (initialized) {
      log->info_hex(pdu->msg, pdu->N_bytes, "Sending %d B to Titan\n", pdu->N_bytes);
      send(pdu->msg, pdu->N_bytes);
    } else {
      log->error("Trying to transmit but port not connected.\n");
    }
  }

private:
  ///< Main message handler
  int handle_message(const unique_byte_array_t& rx_buf, const uint32_t n)
  {
    log->debug_hex(rx_buf->begin(), n, "Received %d B from remote.\n", n);

    // Chop incoming msg, first two bytes are length of the JSON
    // (see IPL4_EUTRA_SYSTEM_Definitions.ttcn
    uint16_t json_len = ((uint16_t)rx_buf->at(0) << 8) | rx_buf->at(1);

    // The data part after the JSON starts right here but handling
    // is done in the respective functions
    uint16_t rx_buf_offset = json_len + 2;

    Document document;
    if (document.Parse((char*)&rx_buf->at(2)).HasParseError() || document.IsObject() == false) {
      log->error_hex((uint8*)&rx_buf->at(2), json_len, "Error parsing incoming data.\n");
      return SRSLTE_ERROR;
    }

    // Pretty-print
    StringBuffer               buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);
    log->info("Received JSON with %d B\n%s\n", json_len, (char*)buffer.GetString());

    // check for common
    assert(document.HasMember("Common"));
    assert(document["Common"].IsObject());

    // Check for request type
    assert(document.HasMember("RrcPdu"));
    assert(document["RrcPdu"].IsObject());

    // Get request type
    const Value& rrcpdu = document["RrcPdu"];
    if (rrcpdu.HasMember("Ccch")) {
      rx_buf_offset += 2;
      handle_ccch_pdu(document, &rx_buf->at(rx_buf_offset), n - rx_buf_offset);
    } else if (rrcpdu.HasMember("Dcch")) {
      rx_buf_offset += 2;
      uint32_t lcid = document["Common"]["RoutingInfo"]["RadioBearerId"]["Srb"].GetInt();
      handle_dcch_pdu(document, lcid, &rx_buf->at(rx_buf_offset), n - rx_buf_offset, ttcn3_helpers::get_follow_on_flag(document));
    } else {
      log->error("Received unknown request.\n");
    }

    return SRSLTE_SUCCESS;
  }

  // Todo: move to SYSSIM
  void handle_ccch_pdu(Document& document, const uint8_t* payload, const uint16_t len)
  {
    log->info_hex(payload, len, "Received CCCH RRC PDU\n");

    // pack into byte buffer
    unique_byte_buffer_t pdu = pool_allocate_blocking;
    pdu->N_bytes             = len;
    memcpy(pdu->msg, payload, pdu->N_bytes);

    syssim->add_ccch_pdu(ttcn3_helpers::get_timing_info(document), std::move(pdu));

    // TODO: is there a better way to check for RRCConnectionReestablishment?
    if (ccch_is_rrc_reestablishment(document)) {
      syssim->reestablish_bearer(1);
    }
  }

  // Todo: move to SYSSIM
  void
  handle_dcch_pdu(Document& document, const uint16_t lcid, const uint8_t* payload, const uint16_t len, bool follow_on)
  {
    log->info_hex(payload, len, "Received DCCH RRC PDU (lcid=%d)\n", lcid);

    // pack into byte buffer
    unique_byte_buffer_t pdu = pool_allocate_blocking;
    pdu->N_bytes             = len;
    memcpy(pdu->msg, payload, pdu->N_bytes);

    syssim->add_dcch_pdu(ttcn3_helpers::get_timing_info(document), lcid, std::move(pdu), follow_on);
  }

  bool ccch_is_rrc_reestablishment(Document& document)
  {
    const Value& dcch = document["RrcPdu"]["Ccch"];
    if (dcch.HasMember("message_")) {
      if (dcch["message_"].HasMember("c1")) {
        if (dcch["message_"]["c1"].HasMember("rrcConnectionReestablishment")) {
          return true;
        }
      }
    }
    return false;
  }

  ss_srb_interface* syssim = nullptr;
  byte_buffer_pool* pool   = nullptr;

  // struct sctp_sndrcvinfo sri = {};
  // struct sockaddr_in client_addr;
};

#endif // SRSUE_TTCN3_SRB_INTERFACE_H