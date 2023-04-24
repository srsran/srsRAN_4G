/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSUE_TTCN3_SRB_INTERFACE_H
#define SRSUE_TTCN3_SRB_INTERFACE_H

#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/mac/pdu.h"
#include "ttcn3_interfaces.h"
#include "ttcn3_port_handler.h"
#include <srsran/interfaces/ue_interfaces.h>

using namespace srsran;

// The SRB interface
class ttcn3_srb_interface : public ttcn3_port_handler
{
public:
  explicit ttcn3_srb_interface(srslog::basic_logger& logger) :
    ttcn3_port_handler(logger), pool(byte_buffer_pool::get_instance())
  {}
  ~ttcn3_srb_interface() = default;

  int init(ss_srb_interface* syssim_, std::string net_ip_, uint32_t net_port_)
  {
    syssim      = syssim_;
    net_ip      = net_ip_;
    net_port    = net_port_;
    initialized = true;
    logger.debug("Initialized.");
    return port_listen();
  }

  void tx(const uint8_t* buffer, uint32_t len)
  {
    if (initialized) {
      logger.info(buffer, len, "Sending %d B to Titan", len);
      send(buffer, len);
    } else {
      logger.error("Trying to transmit but port not connected.");
    }
  }

private:
  ///< Main message handler
  int handle_message(const unique_byte_array_t& rx_buf, const uint32_t n)
  {
    logger.debug(rx_buf->begin(), n, "Received %d B from remote.", n);

    // Chop incoming msg, first two bytes are length of the JSON
    // (see IPL4_EUTRA_SYSTEM_Definitions.ttcn
    uint16_t json_len = ((uint16_t)rx_buf->at(0) << 8) | rx_buf->at(1);

    // The data part after the JSON starts right here but handling
    // is done in the respective functions
    uint16_t rx_buf_offset = json_len + 2;

    Document document;
    if (document.Parse((char*)&rx_buf->at(2)).HasParseError() || document.IsObject() == false) {
      logger.error((uint8_t*)&rx_buf->at(2), json_len, "Error parsing incoming data.");
      return SRSRAN_ERROR;
    }

    // Pretty-print
    StringBuffer               buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);
    logger.info("Received JSON with %d B\n%s", json_len, (char*)buffer.GetString());

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
      handle_dcch_pdu(
          document, lcid, &rx_buf->at(rx_buf_offset), n - rx_buf_offset, ttcn3_helpers::get_follow_on_flag(document));
    } else {
      logger.error("Received unknown request.");
    }

    return SRSRAN_SUCCESS;
  }

  // Todo: move to SYSSIM
  void handle_ccch_pdu(Document& document, const uint8_t* payload, const uint16_t len)
  {
    logger.info(payload, len, "Received CCCH RRC PDU");

    // pack into byte buffer
    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (pdu == nullptr) {
      logger.error("Couldn't allocate buffer in %s().", __FUNCTION__);
      return;
    }
    pdu->N_bytes             = len;
    memcpy(pdu->msg, payload, pdu->N_bytes);

    syssim->add_ccch_pdu(
        ttcn3_helpers::get_timing_info(document), ttcn3_helpers::get_cell_name(document), std::move(pdu));

    // TODO: is there a better way to check for RRCConnectionReestablishment?
    if (ccch_is_rrc_reestablishment(document)) {
      syssim->reestablish_bearer(ttcn3_helpers::get_cell_name(document), 1);
    }
  }

  // Todo: move to SYSSIM
  void
  handle_dcch_pdu(Document& document, const uint16_t lcid, const uint8_t* payload, const uint16_t len, bool follow_on)
  {
    logger.info(payload, len, "Received DCCH RRC PDU (lcid=%d)", lcid);

    // pack into byte buffer
    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (pdu == nullptr) {
      logger.error("Couldn't allocate buffer in %s().", __FUNCTION__);
      return;
    }
    pdu->N_bytes             = len;
    memcpy(pdu->msg, payload, pdu->N_bytes);

    syssim->add_dcch_pdu(ttcn3_helpers::get_timing_info(document),
                         ttcn3_helpers::get_cell_name(document),
                         lcid,
                         std::move(pdu),
                         follow_on);
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
};

#endif // SRSUE_TTCN3_SRB_INTERFACE_H
