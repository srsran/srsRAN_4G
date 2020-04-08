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

#include "srsue/hdr/stack/upper/tft_packet_filter.h"
#include "srslte/upper/ipv6.h"
#include <linux/ip.h>
#include <linux/udp.h>

namespace srsue {

tft_packet_filter_t::tft_packet_filter_t(uint8_t                                eps_bearer_id_,
                                         uint8_t                                lcid_,
                                         const LIBLTE_MME_PACKET_FILTER_STRUCT& tft,
                                         srslte::log*                           log_) :
  eps_bearer_id(eps_bearer_id_),
  lcid(lcid_),
  id(tft.id),
  eval_precedence(tft.eval_precedence),
  active_filters(0),
  log(log_)
{
  int idx = 0;
  while (idx < tft.filter_size) {
    uint8_t filter_type = tft.filter[idx];
    idx++;
    switch (filter_type) {
      // IPv4
      case IPV4_LOCAL_ADDR_TYPE:
        active_filters = IPV4_LOCAL_ADDR_FLAG;
        memcpy(&ipv4_local_addr, &tft.filter[idx], IPV4_ADDR_SIZE);
        idx += IPV4_ADDR_SIZE;
        break;
      case IPV4_REMOTE_ADDR_TYPE:
        active_filters = IPV4_REMOTE_ADDR_FLAG;
        memcpy(&ipv4_remote_addr, &tft.filter[idx], IPV4_ADDR_SIZE);
        idx += IPV4_ADDR_SIZE;
        break;
      // IPv6
      case IPV6_REMOTE_ADDR_TYPE:
        break;
      case IPV6_REMOTE_ADDR_LENGTH_TYPE:
        break;
      case IPV6_LOCAL_ADDR_LENGTH_TYPE:
        break;
      // Ports
      case SINGLE_LOCAL_PORT_TYPE:
        active_filters = SINGLE_LOCAL_PORT_FLAG;
        memcpy(&single_local_port, &tft.filter[idx], 2);
        idx += 2;
        break;
      case SINGLE_REMOTE_PORT_TYPE:
        active_filters = SINGLE_REMOTE_PORT_FLAG;
        memcpy(&single_remote_port, &tft.filter[idx], 2);
        idx += 2;
        break;
      case LOCAL_PORT_RANGE_TYPE:
        break;
      case REMOTE_PORT_RANGE_TYPE:
        break;
      // Protocol/Next Header
      case PROTOCOL_ID_TYPE:
        break;
      // Type of service/Traffic class
      case TYPE_OF_SERVICE_TYPE:
        active_filters = TYPE_OF_SERVICE_FLAG;
        memcpy(&type_of_service, &tft.filter[idx], 1);
        idx += 1;
        memcpy(&type_of_service_mask, &tft.filter[idx], 1);
        idx += 1;
        break;
      // Flow label
      case FLOW_LABEL_TYPE:
        break;
      // IPsec security parameter
      case SECURITY_PARAMETER_INDEX_TYPE:
        break;
      default:
        return;
    }
  }
}

/*
 * Implements packet matching against the packet filter componenets as specified in TS 24.008, section 10.5.6.12.
 *
 * This function will only return true if all the active filter components match (logical AND).
 * It will return false as soon as any of the filter components does not match.
 *
 * Note: 'active_filters' is a bitmask; bits set to '1' represent active filter components.
 */
bool tft_packet_filter_t::match(const srslte::unique_byte_buffer_t& pdu)
{
  uint16_t ip_flags = IPV4_REMOTE_ADDR_FLAG | IPV4_LOCAL_ADDR_FLAG | IPV6_REMOTE_ADDR_FLAG |
                      IPV6_REMOTE_ADDR_LENGTH_FLAG | IPV6_LOCAL_ADDR_LENGTH_FLAG;
  uint16_t port_flags =
      SINGLE_LOCAL_PORT_FLAG | LOCAL_PORT_RANGE_FLAG | SINGLE_REMOTE_PORT_FLAG | REMOTE_PORT_RANGE_FLAG;

  // Check if there is any active filter
  if (active_filters == 0) {
    return false;
  }

  // Match IP Header to active filters
  if ((active_filters & ip_flags) != 0 && !match_ip(pdu)) {
    return false;
  }

  // Check Protocol ID/Next Header Field
  if ((active_filters & PROTOCOL_ID_FLAG) != 0 && !match_protocol(pdu)) {
    return false;
  }

  // Check Ports/Port Range
  if ((active_filters & port_flags) != 0 && !match_port(pdu)) {
    return false;
  }

  // Check Type of Service/Traffic class
  if ((active_filters & TYPE_OF_SERVICE_FLAG) != 0 && !match_type_of_service(pdu)) {
    return false;
  }

  return true;
}

bool tft_packet_filter_t::match_ip(const srslte::unique_byte_buffer_t& pdu)
{
  struct iphdr*   ip_pkt  = (struct iphdr*)pdu->msg;
  struct ipv6hdr* ip6_pkt = (struct ipv6hdr*)pdu->msg;

  if (ip_pkt->version == 4) {
    // Check match on IPv4 packet
    if (active_filters & IPV4_LOCAL_ADDR_FLAG) {
      if (memcmp(&ipv4_local_addr, &ip_pkt->saddr, IPV4_ADDR_SIZE) != 0) {
        return false;
      }
    }
    if (active_filters & IPV4_REMOTE_ADDR_FLAG) {
      if (memcmp(&ipv4_remote_addr, &ip_pkt->daddr, IPV4_ADDR_SIZE) != 0) {
        return false;
      }
    }
  } else if (ip_pkt->version == 6) {
    // Check match on IPv6 (TODO)
  } else {
    // Error
    return false;
  }
  return true;
}

bool tft_packet_filter_t::match_protocol(const srslte::unique_byte_buffer_t& pdu)
{
  struct iphdr*   ip_pkt  = (struct iphdr*)pdu->msg;
  struct ipv6hdr* ip6_pkt = (struct ipv6hdr*)pdu->msg;

  if (ip_pkt->version == 4) {
    // Check match on IPv4 packet
    if (ip_pkt->protocol != protocol_id) {
      return false;
    }
  } else if (ip_pkt->version == 6) {
    // Check match on IPv6 (TODO)
    if (ip6_pkt->nexthdr != protocol_id) {
      return false;
    }
  } else {
    // Error
    return false;
  }
  return true;
}

bool tft_packet_filter_t::match_type_of_service(const srslte::unique_byte_buffer_t& pdu)
{
  struct iphdr* ip_pkt = (struct iphdr*)pdu->msg;

  if (ip_pkt->version == 4) {
    // Check match on IPv4 packet
    if (ip_pkt->tos != type_of_service) {
      return false;
    }
  } else if (ip_pkt->version == 6) {
    // IPv6 traffic class not supported yet
    return false;
  }
  return true;
}

bool tft_packet_filter_t::match_flow_label(const srslte::unique_byte_buffer_t& pdu)
{
  struct ipv6hdr* ip6_pkt = (struct ipv6hdr*)pdu->msg;

  if (ip6_pkt->version == 6 && (active_filters & FLOW_LABEL_FLAG)) {
    // Check match on IPv4 packet
    if (memcmp(ip6_pkt->flow_lbl, flow_label, 3) != 0) {
      return false;
    }
  }
  return true;
}

bool tft_packet_filter_t::match_port(const srslte::unique_byte_buffer_t& pdu)
{
  struct iphdr*   ip_pkt  = (struct iphdr*)pdu->msg;
  struct ipv6hdr* ip6_pkt = (struct ipv6hdr*)pdu->msg;
  struct udphdr*  udp_pkt;

  if (ip_pkt->version == 4) {
    switch (ip_pkt->protocol) {
      case UDP_PROTOCOL:
        udp_pkt = (struct udphdr*)&pdu->msg[ip_pkt->ihl * 4];
        if (active_filters & SINGLE_LOCAL_PORT_FLAG) {
          if (udp_pkt->source != single_local_port) {
            return false;
          }
        }
        if (active_filters & SINGLE_REMOTE_PORT_FLAG) {
          if (udp_pkt->dest != single_remote_port) {
            return false;
          }
        }
        break;
      case TCP_PROTOCOL:
        return false;
      default:
        return false;
    }
  }
  return true;
}

uint8_t tft_pdu_matcher::check_tft_filter_match(const srslte::unique_byte_buffer_t& pdu)
{
  std::lock_guard<std::mutex> lock(tft_mutex);
  uint8_t                     lcid = default_lcid;
  for (std::pair<const uint16_t, tft_packet_filter_t>& filter_pair : tft_filter_map) {
    bool match = filter_pair.second.match(pdu);
    if (match) {
      lcid = filter_pair.second.lcid;
      log->debug("Found filter match -- EPS bearer Id %d, LCID %d\n", filter_pair.second.eps_bearer_id, lcid);
      break;
    }
  }
  return lcid;
}

int tft_pdu_matcher::apply_traffic_flow_template(const uint8_t&                                 erab_id,
                                                 const uint8_t&                                 lcid,
                                                 const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft)
{
  std::lock_guard<std::mutex> lock(tft_mutex);
  switch (tft->tft_op_code) {
    case LIBLTE_MME_TFT_OPERATION_CODE_CREATE_NEW_TFT:
      for (int i = 0; i < tft->packet_filter_list_size; i++) {
        log->info("New packet filter for TFT\n");
        tft_packet_filter_t filter(erab_id, lcid, tft->packet_filter_list[i], log);
        auto                it = tft_filter_map.insert(std::make_pair(filter.eval_precedence, filter));
        if (it.second == false) {
          log->error("Error inserting TFT Packet Filter\n");
          return SRSLTE_ERROR_CANT_START;
        }
      }
      break;
    default:
      log->error("Unhandled TFT OP code\n");
      return SRSLTE_ERROR_CANT_START;
  }
  return SRSLTE_SUCCESS;
}

void tft_pdu_matcher::set_default_lcid(const uint8_t lcid)
{
  default_lcid = lcid;
}

} // namespace srsue
