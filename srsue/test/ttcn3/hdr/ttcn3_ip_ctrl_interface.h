/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSUE_TTCN3_IP_CTRL_INTERFACE_H
#define SRSUE_TTCN3_IP_CTRL_INTERFACE_H

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "srslte/phy/io/netsource.h"
#include "ttcn3_helpers.h"
#include "ttcn3_port_handler.h"

using namespace rapidjson;

// The IP CTRL interface to the IP_PTC
class ttcn3_ip_ctrl_interface : public ttcn3_port_handler
{
public:
  ttcn3_ip_ctrl_interface()  = default;
  ~ttcn3_ip_ctrl_interface() = default;

  int init(srslte::log* log_, std::string net_ip_, uint32_t net_port_)
  {
    net_ip      = net_ip_;
    net_port    = net_port_;
    log         = log_;
    initialized = true;
    log->debug("Initialized.\n");
    return port_listen();
  }

private:
  ///< Main message handler
  int handle_message(const unique_byte_array_t& rx_buf, const uint32_t n)
  {
    log->debug("Received %d B from remote.\n", n);

    Document document;
    if (document.Parse((char*)rx_buf->begin()).HasParseError() || document.IsObject() == false) {
      log->error_hex(rx_buf->begin(), n, "Error parsing incoming data.\n");
      return SRSLTE_ERROR;
    }

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

    return SRSLTE_SUCCESS;
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

      // TODO: actually do configuration
    }

    // What else to check?

    std::string resp = ttcn3_helpers::get_drbmux_common_ind_cnf();

    log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
    send((const uint8_t*)resp.c_str(), resp.length());
  }
};

#endif // SRSUE_TTCN3_IP_CTRL_INTERFACE_H