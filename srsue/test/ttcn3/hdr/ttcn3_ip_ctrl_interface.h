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

#ifndef SRSUE_TTCN3_IP_CTRL_INTERFACE_H
#define SRSUE_TTCN3_IP_CTRL_INTERFACE_H

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "srslte/common/netsource_handler.h"
#include "srslte/phy/io/netsource.h"
#include "ttcn3_helpers.h"

using namespace rapidjson;

// The IP CTRL interface to the IP_PTC
class ttcn3_ip_ctrl_interface : public netsource_handler
{
public:
  ttcn3_ip_ctrl_interface() : netsource_handler("TTCN3_IP_CTRL_IF"){};
  ~ttcn3_ip_ctrl_interface(){};

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
    // open TCP socket
    if (srslte_netsource_init(&net_source, net_ip.c_str(), net_port, SRSLTE_NETSOURCE_TCP)) {
      fprintf(stderr, "Error creating input TCP socket at port %d\n", net_port);
      exit(-1);
    }

    log->info("Listening on %s:%d for incoming connections ..\n", net_ip.c_str(), net_port);

    running = true;

    int n;
    while (run_enable) {
      log->debug("Reading from IP_CTRL port ..\n");
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
        if (document.HasMember("RoutingInfo")) {
          log->info("Received RoutingInfo\n");
          handle_routing_info(document);
        } else {
          log->error("Received unknown request.\n");
        }
      } else if (n == 0) {
        log->error("Receiving null from network\n");
      } else {
        log->error("Error receiving from network\n");
      }
    }

    running = false;

    srslte_netsource_free(&net_source);
  }

  void handle_routing_info(Document& document)
  {
    // get CTRL
    const Value& routes = document["RoutingInfo"];
    assert(routes.IsArray());

    // iterate over all routes
    for (Value::ConstValueIterator itr = routes.Begin(); itr != routes.End(); ++itr) {
      assert(itr->HasMember("IpInfo"));

      // printf("Configuring attenuation of %s to %ddB\n", id.GetString(), att["Value"].GetInt());

      // FIXME: actually do configuration
    }

    // What else to check?

    std::string resp = ttcn3_helpers::get_drbmux_common_ind_cnf();

    log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
    srslte_netsource_write(&net_source, (char*)resp.c_str(), resp.length());
  }
};

#endif // SRSUE_TTCN3_IP_CTRL_INTERFACE_H