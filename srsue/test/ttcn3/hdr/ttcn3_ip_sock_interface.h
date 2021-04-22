/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSUE_TTCN3_IP_SOCK_INTERFACE_H
#define SRSUE_TTCN3_IP_SOCK_INTERFACE_H

#include "ttcn3_port_handler.h"

// The IP Socket interface to the IP_PTC
class ttcn3_ip_sock_interface : public ttcn3_port_handler
{
public:
  explicit ttcn3_ip_sock_interface(srslog::basic_logger& logger) : ttcn3_port_handler(logger) {}
  ~ttcn3_ip_sock_interface() = default;

  int init(std::string net_ip_, uint32_t net_port_)
  {
    net_ip      = net_ip_;
    net_port    = net_port_;
    initialized = true;
    logger.debug("Initialized.");
    return port_listen();
  }

private:
  ///< Main message handler
  int handle_message(const unique_byte_array_t& rx_buf, const uint32_t n)
  {
    logger.debug("Received %d B from remote.", n);

    Document document;
    if (document.Parse((char*)rx_buf->begin()).HasParseError() || document.IsObject() == false) {
      logger.error(rx_buf->begin(), n, "Error parsing incoming data.");
      return SRSRAN_ERROR;
    }

    // Pretty-print
    StringBuffer               buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);
    logger.info("Received %d bytes\n%s", n, (char*)buffer.GetString());

    // Get message
    if (document.HasMember("CTRL")) {
      logger.info("Received CTRL command.");
      handle_ctrl(document);
    } else {
      logger.error("Received unknown request.");
    }

    return SRSRAN_SUCCESS;
  }

  void handle_ctrl(Document& document)
  {
    // get CTRL
    const Value& ctrl = document["CTRL"];
    assert(ctrl.HasMember("ConnectionId"));

    const Value& conn_id = ctrl["ConnectionId"];
    assert(conn_id.HasMember("Protocol"));

    const Value& protocol = conn_id["Protocol"];
    assert(protocol.IsString());

    // Get IP version and addr
    std::string  ip_version;
    const Value& local = conn_id["Local"];
    assert(local.HasMember("IpAddr"));
    const Value& ipAddr = local["IpAddr"];
    if (ipAddr.HasMember("V4")) {
      ip_version = "V4";
    } else if (ipAddr.HasMember("V6")) {
      ip_version = "V6";
    }
    assert(ipAddr[ip_version.c_str()].HasMember("Addr"));
    const Value& addr = ipAddr[ip_version.c_str()]["Addr"];

    string resp = "";
    // TODO: Handle command
    if (ctrl.HasMember("Req")) {
      const Value& req = ctrl["Req"];

      // Don't send a ctrl cnf for UDP or ICMP Close
      if (!(req.HasMember("UDP") && req["UDP"].HasMember("Close") && req["UDP"]["Close"].GetBool()) &&
          !(req.HasMember("ICMP") && req["ICMP"].HasMember("Close") && req["ICMP"]["Close"].GetBool())) {
        resp = ttcn3_helpers::get_ctrl_cnf(protocol.GetString(), ip_version, addr.GetString());
      }
    } else {
      assert(false);
    }

    // Send response
    if (resp.length() > 0) {
      logger.info("Sending %s to tester (%zd B)", resp.c_str(), resp.length());
      send((const uint8_t*)resp.c_str(), resp.length());
    }
  }
};

#endif // SRSUE_TTCN3_IP_SOCK_INTERFACE_H
