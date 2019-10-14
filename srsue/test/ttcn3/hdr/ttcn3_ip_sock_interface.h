/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSUE_TTCN3_IP_SOCK_INTERFACE_H
#define SRSUE_TTCN3_IP_SOCK_INTERFACE_H

// The IP Socket interface to the IP_PTC
class ttcn3_ip_sock_interface : public netsource_handler
{
public:
  ttcn3_ip_sock_interface() : netsource_handler("TTCN3_IP_SOCK_IF"){};
  ~ttcn3_ip_sock_interface(){};

  void init(srslte::log* log_, std::string net_ip_, uint32_t net_port_)
  {
    net_ip      = net_ip_;
    net_port    = net_port_;
    log         = log_;
    initialized = true;
    log->debug("Initialized.\n");
  }

private:
  void run_thread()
  {
    if (!initialized) {
      fprintf(stderr, "IP_SOCK interface not initialized. Exiting.\n");
      exit(-1);
    }

    // open TCP socket
    if (srslte_netsource_init(&net_source, net_ip.c_str(), net_port, SRSLTE_NETSOURCE_TCP)) {
      fprintf(stderr, "Error creating input TCP socket at port %d\n", net_port);
      exit(-1);
    }

    log->info("Listening on %s:%d for incoming connections ..\n", net_ip.c_str(), net_port);

    running = true;

    int n;
    while (run_enable) {
      log->debug("Reading from IP_SOCK port ..\n");
      n = srslte_netsource_read(&net_source, rx_buf->begin(), RX_BUF_SIZE);
      if (n > 0) {
        rx_buf->at(n) = '\0';

        Document document;
        if (document.Parse((char*)rx_buf->begin()).HasParseError()) {
          log->error_hex(rx_buf->begin(), n, "Error parsing incoming data.\n");
          break;
        }
        assert(document.IsObject());

        // Pretty-print
        StringBuffer               buffer;
        PrettyWriter<StringBuffer> writer(buffer);
        document.Accept(writer);
        log->info("Received %d bytes\n%s\n", n, (char*)buffer.GetString());

        // Get message
        if (document.HasMember("CTRL")) {
          log->info("Received CTRL command.\n");
          handle_ctrl(document);
        } else {
          log->error("Received unknown request.\n");
        }
      } else if (n == 0) {
        log->error("Receiving null from network\n");
      } else {
        log->error("Error receiving from network\n");
        // exit(-1);
      }
    }

    running = false;

    srslte_netsource_free(&net_source);
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

    // Todo: Handle command

    // Send response
    string resp = ttcn3_helpers::get_ctrl_cnf(protocol.GetString(), ip_version, addr.GetString());

    log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
    srslte_netsource_write(&net_source, (char*)resp.c_str(), resp.length());
  }
};

#endif // SRSUE_TTCN3_IP_SOCK_INTERFACE_H