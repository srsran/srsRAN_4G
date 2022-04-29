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

#ifndef SRSUE_TTCN3_DRB_INTERFACE_H
#define SRSUE_TTCN3_DRB_INTERFACE_H

#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/mac/pdu.h"
#include "ttcn3_interfaces.h"
#include "ttcn3_port_handler.h"
#include <srsran/asn1/asn1_utils.h>
#include <srsran/interfaces/ue_interfaces.h>

using namespace srsran;

// The DRB interface
class ttcn3_drb_interface : public ttcn3_port_handler
{
public:
  explicit ttcn3_drb_interface(srslog::basic_logger& logger) : ttcn3_port_handler(logger) {}
  ~ttcn3_drb_interface() = default;

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

    // Check for user data
    assert(document.HasMember("U_Plane"));
    assert(document["U_Plane"].IsObject());

    // Handle Pdus
    const Value& uplane = document["U_Plane"];
    assert(uplane.HasMember("SubframeDataList"));
    assert(uplane["SubframeDataList"].IsArray());

    uint32_t lcid = document["Common"]["RoutingInfo"]["RadioBearerId"]["Drb"].GetInt() + 2;
    for (Value::ConstValueIterator itr = uplane["SubframeDataList"].Begin(); itr != uplane["SubframeDataList"].End();
         ++itr) {
      assert(itr->HasMember("PduSduList"));
      assert((*itr)["PduSduList"].IsObject());
      if ((*itr)["PduSduList"].HasMember("PdcpSdu")) {
        assert((*itr)["PduSduList"]["PdcpSdu"].IsArray());
        const Value& sdulist = (*itr)["PduSduList"]["PdcpSdu"];
        for (Value::ConstValueIterator sdu_itr = sdulist.Begin(); sdu_itr != sdulist.End(); ++sdu_itr) {
          assert(sdu_itr->IsString());
          string              sdustr = sdu_itr->GetString();
          asn1::dyn_octstring octstr(sdustr.size());
          octstr.from_string(sdustr);
          handle_sdu(document, lcid, octstr.data(), octstr.size(), ttcn3_helpers::get_follow_on_flag(document));
        }
      } else if ((*itr)["PduSduList"].HasMember("MacPdu")) {
        logger.warning("Not handling MacPdu type.");
      } else {
        logger.warning("Not handling this PduSdu type.");
      }
    }

    return SRSRAN_SUCCESS;
  }

  void handle_sdu(Document& document, const uint16_t lcid, const uint8_t* payload, const uint16_t len, bool follow_on)
  {
    logger.info(payload, len, "Received DRB PDU (lcid=%d)", lcid);

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

  ss_srb_interface* syssim = nullptr;
};

#endif // SRSUE_TTCN3_DRB_INTERFACE_H
