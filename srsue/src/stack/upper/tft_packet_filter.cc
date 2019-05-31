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

#include "srsue/hdr/stack/upper/tft_packet_filter.h"
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/udp.h>

namespace srsue {

tft_packet_filter_t::tft_packet_filter_t(const LIBLTE_MME_PACKET_FILTER_STRUCT& tft) :
  id(tft.id),
  eval_precedence(tft.eval_precedence),
  active_filters(0)
{
  int idx = 0;
  while (idx < tft.filter_size) {
    uint8_t filter_type = tft.filter[idx];  
    idx++;
    switch (filter_type) {
      // IPv4
      case IPV4_REMOTE_ADDR_TYPE:
        active_filters = IPV4_REMOTE_ADDR_FLAG;
        memcpy(&ipv4_remote_addr, &tft.filter[idx], IPV4_ADDR_SIZE);
        idx += IPV4_ADDR_SIZE;
        break;
      case IPV4_LOCAL_ADDR_TYPE:
        active_filters = IPV4_LOCAL_ADDR_FLAG;
        memcpy(&ipv4_local_addr, &tft.filter[idx], IPV4_ADDR_SIZE);
        idx += IPV4_ADDR_SIZE;
        break;
      //IPv6
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
        break;
      //Flow label
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

bool tft_packet_filter_t::match(const srslte::unique_byte_buffer_t& pdu)
{
  bool            match   = true;
  struct iphdr*   ip_pkt  = (struct iphdr*)pdu->msg;
  struct ipv6hdr* ip6_pkt = (struct ipv6hdr*)pdu->msg;

  // Match IP Header to active filters
  if (!match_ip(pdu)) {
    return false;
  }

  // Check Protocol ID/Next Header Field
  if (!match_protocol(pdu)) {
    return false;
  }

  // Check Ports/Port Range
  if (!match_port(pdu)) {
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
    if (active_filters & IPV4_REMOTE_ADDR_TYPE) {
      if (memcmp(&ipv4_remote_addr, &ip_pkt->daddr, IPV4_ADDR_SIZE) != 0) {
        return false;
      }
    }
    if (active_filters & IPV4_LOCAL_ADDR_TYPE) {
      if (memcmp(&ipv4_local_addr, &ip_pkt->saddr, IPV4_ADDR_SIZE) != 0) {
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

  if (active_filters & PROTOCOL_ID_TYPE) {
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
  }
  return true;
}

bool tft_packet_filter_t::match_type_of_service(const srslte::unique_byte_buffer_t& pdu)
{
  struct iphdr* ip_pkt = (struct iphdr*)pdu->msg;

  if (ip_pkt->version == 4 && (active_filters & TYPE_OF_SERVICE_FLAG)) {
    // Check match on IPv4 packet
    if (ip_pkt->tos != type_of_service) {
      return false;
    }
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
  struct udphdr* udp_pkt;

  // LOCAL_PORT_RANGE_FLAG
  // SINGLE_REMOTE_PORT_FLAG
  // REMOTE_PORT_RANGE_FLAG

  if (ip_pkt->version == 4) {
    switch (ip_pkt->protocol) {
      case UDP_PROTOCOL:
        printf("UDP protocol\n");
        udp_pkt = (struct udphdr*)&pdu->msg[ip_pkt->ihl * 4];
        printf("%d\n", ntohs(udp_pkt->source));
        printf("%d\n", ntohs(udp_pkt->dest));
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
        printf("TCP protocol\n");
        break;
      default:
        printf("Unhandled protocol\n");
        return false;
    }
  }
  return true;
}
} // namespace srsue
