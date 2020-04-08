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

#ifndef SRSUE_PACKET_FILTER_H
#define SRSUE_PACKET_FILTER_H

#include "srslte/asn1/liblte_mme.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/log.h"
#include "srslte/common/log_filter.h"
#include <mutex>

namespace srsue {

const uint16_t IPV4_REMOTE_ADDR_FLAG         = 1;
const uint16_t IPV4_LOCAL_ADDR_FLAG          = 1 << 1;
const uint16_t IPV6_REMOTE_ADDR_FLAG         = 1 << 2;
const uint16_t IPV6_REMOTE_ADDR_LENGTH_FLAG  = 1 << 3;
const uint16_t IPV6_LOCAL_ADDR_LENGTH_FLAG   = 1 << 4;
const uint16_t PROTOCOL_ID_FLAG              = 1 << 5;
const uint16_t SINGLE_LOCAL_PORT_FLAG        = 1 << 6;
const uint16_t LOCAL_PORT_RANGE_FLAG         = 1 << 7;
const uint16_t SINGLE_REMOTE_PORT_FLAG       = 1 << 8;
const uint16_t REMOTE_PORT_RANGE_FLAG        = 1 << 9;
const uint16_t SECURITY_PARAMETER_INDEX_FLAG = 1 << 10;
const uint16_t TYPE_OF_SERVICE_FLAG          = 1 << 11;
const uint16_t FLOW_LABEL_FLAG               = 1 << 12;

const uint8_t IPV4_REMOTE_ADDR_TYPE         = 0b00010000;
const uint8_t IPV4_LOCAL_ADDR_TYPE          = 0b00010001;
const uint8_t IPV6_REMOTE_ADDR_TYPE         = 0b00100000;
const uint8_t IPV6_REMOTE_ADDR_LENGTH_TYPE  = 0b00100001;
const uint8_t IPV6_LOCAL_ADDR_LENGTH_TYPE   = 0b00100011;
const uint8_t PROTOCOL_ID_TYPE              = 0b00110000;
const uint8_t SINGLE_LOCAL_PORT_TYPE        = 0b01000000;
const uint8_t LOCAL_PORT_RANGE_TYPE         = 0b01000001;
const uint8_t SINGLE_REMOTE_PORT_TYPE       = 0b01010000;
const uint8_t REMOTE_PORT_RANGE_TYPE        = 0b01010001;
const uint8_t SECURITY_PARAMETER_INDEX_TYPE = 0b01100000;
const uint8_t TYPE_OF_SERVICE_TYPE          = 0b01110000;
const uint8_t FLOW_LABEL_TYPE               = 0b10000000;

// Helper const
const uint8_t IPV4_ADDR_SIZE = 4;
const uint8_t IPV6_ADDR_SIZE = 16;
const uint8_t UDP_PROTOCOL   = 0x11;
const uint8_t TCP_PROTOCOL   = 0x06;

// TS 24.008 Table 10.5.162
class tft_packet_filter_t
{
public:
  tft_packet_filter_t(uint8_t                                eps_bearer_id_,
                      uint8_t                                lcid_,
                      const LIBLTE_MME_PACKET_FILTER_STRUCT& tft_,
                      srslte::log*                           log_);
  bool match(const srslte::unique_byte_buffer_t& pdu);

  uint8_t  eps_bearer_id;
  uint8_t  lcid;
  uint8_t  id;
  uint8_t  eval_precedence;
  uint16_t active_filters;
  uint32_t ipv4_remote_addr;
  uint32_t ipv4_local_addr;
  uint8_t  ipv6_remote_addr[16];
  uint8_t  ipv6_remote_addr_length;
  uint8_t  ipv6_local_addr_length;
  uint8_t  protocol_id;
  uint16_t single_local_port;
  uint16_t local_port_range[2];
  uint16_t single_remote_port;
  uint16_t remote_port_range[2];
  uint32_t security_parameter_index;
  uint8_t  type_of_service;
  uint8_t  type_of_service_mask;
  uint8_t  flow_label[3];

  srslte::log* log;

  bool match_ip(const srslte::unique_byte_buffer_t& pdu);
  bool match_protocol(const srslte::unique_byte_buffer_t& pdu);
  bool match_type_of_service(const srslte::unique_byte_buffer_t& pdu);
  bool match_flow_label(const srslte::unique_byte_buffer_t& pdu);
  bool match_port(const srslte::unique_byte_buffer_t& pdu);
};

/**
 * TFT PDU matcher class used by GW and TTCN3 DUT testloop handler
 */
class tft_pdu_matcher
{
public:
  tft_pdu_matcher(srslte::log_filter* log_) : log(log_){};
  ~tft_pdu_matcher(){};

  void    set_default_lcid(const uint8_t lcid);
  uint8_t check_tft_filter_match(const srslte::unique_byte_buffer_t& pdu);
  int     apply_traffic_flow_template(const uint8_t&                                 erab_id,
                                      const uint8_t&                                 lcid,
                                      const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft);

private:
  srslte::log_filter*                             log          = nullptr;
  uint8_t                                         default_lcid = 0;
  std::mutex                                      tft_mutex;
  typedef std::map<uint16_t, tft_packet_filter_t> tft_filter_map_t;
  tft_filter_map_t                                tft_filter_map;
};

} // namespace srsue

#endif // SRSUE_TFT_PACKET_FILTER_H
