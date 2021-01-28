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
  explicit ttcn3_ip_ctrl_interface(srslog::basic_logger& logger) : ttcn3_port_handler(logger) {}
  ~ttcn3_ip_ctrl_interface() = default;

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
      return SRSLTE_ERROR;
    }

    // Pretty-print
    StringBuffer               buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);
    logger.info("Received %d bytes\n%s", n, (char*)buffer.GetString());

    // Get message
    if (document.HasMember("RoutingInfo")) {
      logger.info("Received RoutingInfo");
      handle_routing_info(document);
    } else {
      logger.error("Received unknown request.");
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

    logger.info("Sending %s to tester (%zd B)", resp.c_str(), resp.length());
    send((const uint8_t*)resp.c_str(), resp.length());
  }
};

#endif // SRSUE_TTCN3_IP_CTRL_INTERFACE_H