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

#ifndef SRSUE_TTCN3_UT_INTERFACE_H
#define SRSUE_TTCN3_UT_INTERFACE_H

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "srslte/common/log.h"
#include "srslte/common/netsource_handler.h"
#include "ttcn3_interfaces.h"

using namespace rapidjson;

// The UpperTester interface
class ttcn3_ut_interface : public netsource_handler
{
public:
  ttcn3_ut_interface() : netsource_handler("TTCN3_UT_IF") {}
  ~ttcn3_ut_interface(){};

  void init(syssim_interface* syssim_, srslte::log* log_, std::string net_ip_, uint32_t net_port_)
  {
    syssim      = syssim_;
    log         = log_;
    net_ip      = net_ip_;
    net_port    = net_port_;
    initialized = true;
    log->debug("Initialized.\n");
  }

private:
  void run_thread()
  {
    if (!initialized) {
      fprintf(stderr, "UT interface not initialized. Exiting.\n");
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
      log->debug("Reading from UT port ..\n");

      n = srslte_netsource_read(&net_source, rx_buf->begin(), RX_BUF_SIZE);
      if (n > 0) {
        // Terminate
        rx_buf->at(n) = '\0';

        Document document;
        if (document.Parse((char*)rx_buf->begin()).HasParseError()) {
          log->error_hex(rx_buf->begin(), n, "Error parsing incoming data.\n");
          continue;
        }
        assert(document.IsObject());

        // Pretty-print
        StringBuffer               buffer;
        PrettyWriter<StringBuffer> writer(buffer);
        document.Accept(writer);
        log->info("Received %d bytes\n%s\n", n, (char*)buffer.GetString());

        // check for command
        assert(document.HasMember("Cmd"));
        assert(document["Cmd"].IsObject());

        // get Cmd
        const Value& a = document["Cmd"];

        if (a.HasMember("MMI")) {
          assert(a.HasMember("MMI"));

          // get MMI and make sure it has another Cmd nested
          const Value& mmi = a["MMI"];
          assert(mmi.HasMember("Cmd"));

          // get MMI cmd
          const Value& mmi_cmd = mmi["Cmd"];
          assert(mmi_cmd.IsString());

          // check for CnfRequired
          assert(document.HasMember("CnfRequired"));

          if (strcmp(mmi_cmd.GetString(), "POWER_OFF") == 0) {
            log->info("Received POWER_OFF command.\n");
            handle_power_off(document);
          } else if (strcmp(mmi_cmd.GetString(), "SWITCH_ON") == 0) {
            log->info("Received SWITCH_ON command.\n");
            syssim->switch_on_ue();
          } else if (strcmp(mmi_cmd.GetString(), "SWITCH_OFF") == 0) {
            log->info("Received SWITCH_OFF command.\n");
            syssim->switch_off_ue();
          } else {
            log->error("Received unknown command: %s\n", mmi_cmd.GetString());
          }
        } else if (a.HasMember("AT")) {
          handle_at_command(document);
        } else if (a.HasMember("TC_START")) {
          log->info("Received TC_START command.\n");
          const Value& cmd = a["TC_START"];
          assert(cmd.HasMember("Name"));
          const Value& tc_name = cmd["Name"];
          syssim->tc_start(tc_name.GetString());
        } else if (a.HasMember("TC_END")) {
          log->info("Received TC_END command.\n");
          syssim->tc_end();
        } else {
          log->error("Unknown command type.\n");
        }
      } else if (n == 0) {
        log->error("Connection closed on UT interface.\n");
      } else {
        log->error("Error receiving from network\n");
        exit(-1);
      }
    }
    running = false;

    srslte_netsource_free(&net_source);
  }

  void handle_power_off(Document& document)
  {
    syssim->power_off_ue();

    // Create response
    Document resp;
    resp.SetObject();
    resp.AddMember("Result", true, resp.GetAllocator());

    // Serialize and send to tester
    StringBuffer         buffer;
    Writer<StringBuffer> writer(buffer);
    resp.Accept(writer);

    log->info("Sending %s to tester (%zd B)\n", buffer.GetString(), buffer.GetSize());
    srslte_netsource_write(&net_source, (char*)buffer.GetString(), buffer.GetSize());
  }

  void handle_at_command(Document& document)
  {
    // We can assume the doc contains a AT CMD
    const Value& at = document["Cmd"]["AT"];

    // turn off data services
    if (std::string(at.GetString()) == "AT+CGATT=0<CR>") {
      log->info("Disabling data services\n");
      syssim->disable_data();
    } else if (std::string(at.GetString()) == "AT+CGATT=1<CR>") {
      log->info("Enabling data services\n");
      syssim->enable_data();
    } else {
      log->error("Not handling AT command %s\n", at.GetString());
    }
  }

  syssim_interface* syssim = nullptr;
};

#endif // SRSUE_TTCN3_UT_INTERFACE_H