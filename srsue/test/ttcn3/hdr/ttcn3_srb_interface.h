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

#ifndef SRSUE_TTCN3_SRB_INTERFACE_H
#define SRSUE_TTCN3_SRB_INTERFACE_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/pdu.h"
#include "ttcn3_interfaces.h"
#include <srslte/interfaces/ue_interfaces.h>

using namespace srslte;

// The SRB interface
class ttcn3_srb_interface : public netsource_handler
{
public:
  ttcn3_srb_interface() : syssim(nullptr), pool(byte_buffer_pool::get_instance()), netsource_handler("TTCN3_SRB_IF"){};
  ~ttcn3_srb_interface(){};

  void init(syssim_interface* syssim_, srslte::log* log_, std::string net_ip_, uint32_t net_port_)
  {
    syssim   = syssim_;
    log      = log_;
    net_ip   = net_ip_;
    net_port = net_port_;

    initialized = true;
    log->debug("Initialized.\n");
  }

  void tx(unique_byte_buffer_t pdu)
  {
    if (running) {
      log->info_hex(pdu->msg, pdu->N_bytes, "Sending %d B to Titan\n", pdu->N_bytes);
      srslte_netsource_write(&net_source, (void*)pdu->msg, pdu->N_bytes);
    } else {
      log->error("Trying to transmit but port not connected.\n");
    }
  }

private:
  void run_thread()
  {
    if (!initialized) {
      fprintf(stderr, "SRB interface not initialized. Exiting.\n");
      exit(-1);
    }

    // open TCP socket
    if (srslte_netsource_init(&net_source, net_ip.c_str(), net_port, SRSLTE_NETSOURCE_TCP)) {
      fprintf(stderr, "Error creating input TCP socket at port %d\n", net_port);
      exit(-1);
    }

    running = true;

    int n;
    while (run_enable) {
      log->debug("Reading from SRB port ..\n");
      n = srslte_netsource_read(&net_source, rx_buf->begin(), RX_BUF_SIZE);
      if (n > 0) {
        rx_buf->at(n) = '\0';

        log->debug_hex(rx_buf->begin(), n, "Received %d B from remote.\n", n);

        // Chop incoming msg, first two bytes are length of the JSON
        // (see IPL4_EUTRA_SYSTEM_Definitions.ttcn
        uint16_t json_len = ((uint16_t)rx_buf->at(0) << 8) | rx_buf->at(1);

        // Copy JSON from received buffer and null-terminate
        char json[json_len + 1];
        memcpy(json, &rx_buf->at(2), json_len);
        json[json_len] = '\0';

        // The data part after the JSON starts right here but handling
        // is done in the respective functions
        uint16_t rx_buf_offset = json_len + 2;

        Document document;
        if (document.Parse(json).HasParseError()) {
          log->error_hex((uint8*)json, json_len, "Error parsing incoming data.\n");
          break;
        }
        assert(document.IsObject());

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
          uint32_t lcid = 1;
          handle_dcch_pdu(document, lcid, &rx_buf->at(rx_buf_offset), n - rx_buf_offset);
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

  // Todo: move to SYSSIM
  void handle_ccch_pdu(Document& document, const uint8_t* payload, const uint16_t len)
  {
    log->info_hex(payload, len, "Received CCCH RRC PDU\n");

    // pack into byte buffer
    unique_byte_buffer_t pdu = pool_allocate_blocking;
    pdu->N_bytes             = len;
    memcpy(pdu->msg, payload, pdu->N_bytes);

    syssim->add_ccch_pdu(std::move(pdu));
  }

  // Todo: move to SYSSIM
  void handle_dcch_pdu(Document& document, const uint16_t lcid, const uint8_t* payload, const uint16_t len)
  {
    log->info_hex(payload, len, "Received DCCH RRC PDU\n");

    // pack into byte buffer
    unique_byte_buffer_t pdu = pool_allocate_blocking;
    pdu->N_bytes             = len;
    memcpy(pdu->msg, payload, pdu->N_bytes);

    syssim->add_dcch_pdu(lcid, std::move(pdu));
  }

  syssim_interface* syssim = nullptr;
  byte_buffer_pool* pool   = nullptr;
};

#endif // SRSUE_TTCN3_SRB_INTERFACE_H