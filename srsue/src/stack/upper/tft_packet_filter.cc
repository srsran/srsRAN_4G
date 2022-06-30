/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsue/hdr/stack/upper/tft_packet_filter.h"
#include "srsran/upper/ipv6.h"

extern "C" {
#include "srsran/config.h"
}

#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>

namespace srsue {

tft_packet_filter_t::tft_packet_filter_t(uint8_t                                eps_bearer_id_,
                                         const LIBLTE_MME_PACKET_FILTER_STRUCT& tft,
                                         srslog::basic_logger&                  logger) :
  eps_bearer_id(eps_bearer_id_),
  id(tft.id),
  eval_precedence(tft.eval_precedence),
  active_filters(0),
  logger(logger)
{
  int      idx             = 0;
  uint32_t length_in_bytes = 0;
  uint32_t remaining_bits  = 0;
  while (idx < tft.filter_size) {
    uint8_t filter_type = tft.filter[idx];
    idx++;
    switch (filter_type) {
      // IPv4
      case IPV4_LOCAL_ADDR_TYPE:
        active_filters |= IPV4_LOCAL_ADDR_FLAG;
        memcpy(&ipv4_local_addr, &tft.filter[idx], IPV4_ADDR_SIZE);
        idx += IPV4_ADDR_SIZE;
        memcpy(&ipv4_local_addr_mask, &tft.filter[idx], IPV4_ADDR_SIZE);
        idx += IPV4_ADDR_SIZE;
        break;

      case IPV4_REMOTE_ADDR_TYPE:
        active_filters |= IPV4_REMOTE_ADDR_FLAG;
        memcpy(&ipv4_remote_addr, &tft.filter[idx], IPV4_ADDR_SIZE);
        idx += IPV4_ADDR_SIZE;
        memcpy(&ipv4_remote_addr_mask, &tft.filter[idx], IPV4_ADDR_SIZE);
        idx += IPV4_ADDR_SIZE;
        break;

      // IPv6
      case IPV6_REMOTE_ADDR_TYPE:
        active_filters |= IPV6_REMOTE_ADDR_FLAG;
        memcpy(&ipv6_remote_addr, &tft.filter[idx], IPV6_ADDR_SIZE);
        idx += IPV6_ADDR_SIZE;
        memcpy(&ipv6_remote_addr_mask, &tft.filter[idx], IPV6_ADDR_SIZE);
        idx += IPV6_ADDR_SIZE;
        ipv6_remote_addr_length = IPV6_ADDR_SIZE;
        break;

      case IPV6_REMOTE_ADDR_LENGTH_TYPE: // "IPv6 remote address/prefix length type"
        active_filters |= IPV6_REMOTE_ADDR_LENGTH_FLAG;
        memcpy(&ipv6_remote_addr, &tft.filter[idx], IPV6_ADDR_SIZE);
        idx += IPV6_ADDR_SIZE;
        ipv6_remote_addr_length = tft.filter[idx++];
        // convert address length to mask:
        length_in_bytes = ipv6_remote_addr_length / 8;
        remaining_bits  = ipv6_remote_addr_length % 8;
        for (uint i = 0; i < 16; i++)
          ipv6_remote_addr_mask[i] = 0;
        for (uint i = 0; i < length_in_bytes; i++)
          ipv6_remote_addr_mask[i] = 0xff;
        if (remaining_bits > 0)
          ipv6_remote_addr_mask[length_in_bytes] = 0xff - ((1 << (8 - remaining_bits)) - 1);
        break;

      case IPV6_LOCAL_ADDR_LENGTH_TYPE:
        active_filters |= IPV6_LOCAL_ADDR_LENGTH_FLAG;
        memcpy(&ipv6_local_addr, &tft.filter[idx], IPV6_ADDR_SIZE);
        idx += IPV6_ADDR_SIZE;
        ipv6_local_addr_length = tft.filter[idx++];
        // convert address length to mask:
        length_in_bytes = ipv6_local_addr_length / 8;
        remaining_bits  = ipv6_local_addr_length % 8;
        for (uint i = 0; i < 16; i++)
          ipv6_local_addr_mask[i] = 0;
        for (uint i = 0; i < length_in_bytes; i++)
          ipv6_local_addr_mask[i] = 0xff;
        if (remaining_bits > 0)
          ipv6_local_addr_mask[length_in_bytes] = 0xff - ((1 << (8 - remaining_bits)) - 1);
        break;

      // Ports
      case SINGLE_LOCAL_PORT_TYPE:
        active_filters |= SINGLE_LOCAL_PORT_FLAG;
        memcpy(&single_local_port, &tft.filter[idx], 2);
        idx += 2;
        break;

      case SINGLE_REMOTE_PORT_TYPE:
        active_filters |= SINGLE_REMOTE_PORT_FLAG;
        memcpy(&single_remote_port, &tft.filter[idx], 2);
        idx += 2;
        break;

      case LOCAL_PORT_RANGE_TYPE:
        active_filters |= LOCAL_PORT_RANGE_FLAG;
        memcpy(&local_port_range[0], &tft.filter[idx], 2);
        memcpy(&local_port_range[1], &tft.filter[idx + 2], 2);
        if (local_port_range[0] > local_port_range[1]) { // wrong order
          uint16_t t          = local_port_range[0];
          local_port_range[0] = local_port_range[1];
          local_port_range[1] = t;
        }
        idx += 4;
        break;

      case REMOTE_PORT_RANGE_TYPE:
        active_filters |= REMOTE_PORT_RANGE_FLAG;
        memcpy(&remote_port_range[0], &tft.filter[idx], 2);
        memcpy(&remote_port_range[1], &tft.filter[idx + 2], 2);
        if (remote_port_range[0] > remote_port_range[1]) { // wrong order
          uint16_t t           = remote_port_range[0];
          remote_port_range[0] = remote_port_range[1];
          remote_port_range[1] = t;
        }
        idx += 4;
        break;

      // Protocol/Next Header
      case PROTOCOL_ID_TYPE:
        active_filters |= PROTOCOL_ID_FLAG;
        protocol_id = tft.filter[idx++];
        break;

      // Type of service/Traffic class
      case TYPE_OF_SERVICE_TYPE:
        active_filters |= TYPE_OF_SERVICE_FLAG;
        type_of_service      = tft.filter[idx++];
        type_of_service_mask = tft.filter[idx++];
        break;

      // Flow label
      case FLOW_LABEL_TYPE:
        active_filters |= FLOW_LABEL_FLAG;
        memcpy(&flow_label, &tft.filter[idx], 3);
        idx += 3;
        break;

      // IPsec security parameter
      case SECURITY_PARAMETER_INDEX_TYPE:
        active_filters |= SECURITY_PARAMETER_INDEX_FLAG;
        memcpy(&security_parameter_index, &tft.filter[idx], 4);
        idx += 4;
        break;

      default:
        logger.error("ERROR: wrong type: 0x%02x", filter_type);
        return;
    }
  }
}

bool inline tft_packet_filter_t::filter_contains(uint16_t filtertype)
{
  return (active_filters & filtertype) != 0;
}

/*
 * Implements packet matching against the packet filter componenets as specified in TS 24.008, section 10.5.6.12.
 *
 * This function will only return true if all the active filter components match (logical AND).
 * It will return false as soon as any of the filter components does not match.
 *
 * Note: 'active_filters' is a bitmask; bits set to '1' represent active filter components.
 */
bool tft_packet_filter_t::match(const srsran::unique_byte_buffer_t& pdu)
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
  if (filter_contains(ip_flags) && !match_ip(pdu)) {
    return false;
  }

  // Check Protocol ID/Next Header Field
  if (filter_contains(PROTOCOL_ID_FLAG) && !match_protocol(pdu)) {
    return false;
  }

  // Check Ports/Port Range
  if (filter_contains(port_flags) && !match_port(pdu)) {
    return false;
  }

  // Check Type of Service/Traffic class
  if (filter_contains(TYPE_OF_SERVICE_FLAG) && !match_type_of_service(pdu)) {
    return false;
  }

  return true;
}

bool tft_packet_filter_t::match_ip(const srsran::unique_byte_buffer_t& pdu)
{
  struct iphdr*   ip_pkt  = (struct iphdr*)pdu->msg;
  struct ipv6hdr* ip6_pkt = (struct ipv6hdr*)pdu->msg;
  // It is implied, that this is always an OUTGOING packet
  if (ip_pkt->version == 4) {
    // Check match on IPv4 packet
    if (filter_contains(IPV4_LOCAL_ADDR_FLAG)) {
      if ((ip_pkt->saddr & ipv4_local_addr_mask) != (ipv4_local_addr & ipv4_local_addr_mask)) {
        return false;
      }
    }

    if (filter_contains(IPV4_REMOTE_ADDR_FLAG)) {
      if ((ip_pkt->daddr & ipv4_remote_addr_mask) != (ipv4_remote_addr & ipv4_remote_addr_mask)) {
        return false;
      }
    }
  } else if (ip_pkt->version == 6) {
    // Check match on IPv6
    if (filter_contains(IPV6_REMOTE_ADDR_FLAG | IPV6_REMOTE_ADDR_LENGTH_FLAG)) {
      bool match = true;
      for (int i = 0; i < ipv6_remote_addr_length; i++) {
        match &= ((ipv6_remote_addr[i] ^ ip6_pkt->daddr.in6_u.u6_addr8[i]) & ipv6_remote_addr_mask[i]) == 0;
        if (!match) {
          return false;
        }
      }
      return true;
    }
  } else {
    // Error
    return false;
  }
  return true;
}

bool tft_packet_filter_t::match_protocol(const srsran::unique_byte_buffer_t& pdu)
{
  struct iphdr*   ip_pkt  = (struct iphdr*)pdu->msg;
  struct ipv6hdr* ip6_pkt = (struct ipv6hdr*)pdu->msg;

  if (ip_pkt->version == 4) {
    // Check match on IPv4 packet
    if (ip_pkt->protocol != protocol_id) {
      return false;
    }
  } else if (ip_pkt->version == 6) {
    // Check match on IPv6 packet
    if (ip6_pkt->nexthdr != protocol_id) {
      return false;
    }
  } else {
    // Error
    return false;
  }
  return true;
}

bool tft_packet_filter_t::match_type_of_service(const srsran::unique_byte_buffer_t& pdu)
{
  struct iphdr* ip_pkt = (struct iphdr*)pdu->msg;

  if (ip_pkt->version == 4) {
    // Check match on IPv4 packet
    if ((ip_pkt->tos ^ type_of_service) & type_of_service_mask) {
      return false;
    }
  } else if (ip_pkt->version == 6) {
    // IPv6 traffic class not supported yet
    return false;
  }
  return true;
}

bool tft_packet_filter_t::match_flow_label(const srsran::unique_byte_buffer_t& pdu)
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

bool tft_packet_filter_t::match_port(const srsran::unique_byte_buffer_t& pdu)
{
  struct iphdr*   ip_pkt  = (struct iphdr*)pdu->msg;
  struct ipv6hdr* ip6_pkt = (struct ipv6hdr*)pdu->msg;
  struct udphdr*  udp_pkt;
  struct tcphdr*  tcp_pkt;

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
        tcp_pkt = (struct tcphdr*)&pdu->msg[ip_pkt->ihl * 4];
        if (active_filters & SINGLE_LOCAL_PORT_FLAG) {
          if (tcp_pkt->source != single_local_port) {
            return false;
          }
        }
        if (active_filters & SINGLE_REMOTE_PORT_FLAG) {
          if (tcp_pkt->dest != single_remote_port) {
            return false;
          }
        }
        break;
      default:
        return false;
    }
  } else if (ip_pkt->version == 6) {
    switch (ip6_pkt->nexthdr) {
      case UDP_PROTOCOL:
        udp_pkt = (struct udphdr*)&pdu->msg[sizeof(ipv6hdr)];
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
        tcp_pkt = (struct tcphdr*)&pdu->msg[sizeof(ipv6hdr)];
        if (active_filters & SINGLE_LOCAL_PORT_FLAG) {
          if (tcp_pkt->source != single_local_port) {
            return false;
          }
        }
        if (active_filters & SINGLE_REMOTE_PORT_FLAG) {
          if (tcp_pkt->dest != single_remote_port) {
            return false;
          }
        }
        break;
      default:
        return false;
    }
  }
  return true;
}

void tft_pdu_matcher::reset()
{
  tft_filter_map.clear();
}

/**
 * Checks whether the provided PDU matches any configured TFT.
 * If it finds a match, it updates the eps_bearer_id parameter.
 * @param pdu           Reference to the PDU to check.
 * @param eps_bearer_id Reference to variable to store EPS bearer ID.
 * @return SRSRAN_SUCCESS if a reference could be found, SRSRAN_ERROR otherwise.
 */
int tft_pdu_matcher::check_tft_filter_match(const srsran::unique_byte_buffer_t& pdu, uint8_t& eps_bearer_id)
{
  std::lock_guard<std::mutex> lock(tft_mutex);
  for (std::pair<const uint16_t, tft_packet_filter_t>& filter_pair : tft_filter_map) {
    bool match = filter_pair.second.match(pdu);
    if (match) {
      eps_bearer_id = filter_pair.second.eps_bearer_id;
      logger.debug("Found filter match -- EPS bearer Id %d", filter_pair.second.eps_bearer_id);
      return SRSRAN_SUCCESS;
    }
  }
  return SRSRAN_ERROR;
}

/**
 * @brief Deletes all registered TFT for a given EPS bearer ID
 *
 * @param eps_bearer_id The EPS bearer ID
 */
void tft_pdu_matcher::delete_tft_for_eps_bearer(const uint8_t eps_bearer_id)
{
  std::lock_guard<std::mutex> lock(tft_mutex);
  auto                        old_filter = std::find_if(
      tft_filter_map.begin(), tft_filter_map.end(), [&](const std::pair<uint16_t, tft_packet_filter_t>& filter) {
        return filter.second.eps_bearer_id == eps_bearer_id;
      });
  if (old_filter != tft_filter_map.end()) {
    logger.debug("Deleting TFT for EPS bearer %d", eps_bearer_id);
    tft_filter_map.erase(old_filter);
  }
}

int tft_pdu_matcher::apply_traffic_flow_template(const uint8_t&                                 eps_bearer_id,
                                                 const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft)
{
  std::lock_guard<std::mutex> lock(tft_mutex);
  switch (tft->tft_op_code) {
    case LIBLTE_MME_TFT_OPERATION_CODE_CREATE_NEW_TFT:
      for (int i = 0; i < tft->packet_filter_list_size; i++) {
        logger.info("New TFT for eps_bearer_id=%d, eval_precedence=%d",
                    eps_bearer_id,
                    tft->packet_filter_list[i].eval_precedence);
        tft_packet_filter_t filter(eps_bearer_id, tft->packet_filter_list[i], logger);
        auto                it = tft_filter_map.insert(std::make_pair(filter.eval_precedence, filter));
        if (it.second == false) {
          logger.error("Error inserting TFT Packet Filter");
          return SRSRAN_ERROR_CANT_START;
        }
      }
      break;
    case LIBLTE_MME_TFT_OPERATION_CODE_REPLACE_PACKET_FILTERS_IN_EXISTING_TFT:
      for (int i = 0; i < tft->packet_filter_list_size; i++) {
        // erase old filter if it exists
        auto old_filter = std::find_if(
            tft_filter_map.begin(), tft_filter_map.end(), [&](const std::pair<uint16_t, tft_packet_filter_t>& filter) {
              return filter.second.id == tft->packet_filter_list[i].id;
            });
        if (old_filter == tft_filter_map.end()) {
          logger.error("Error couldn't find TFT with id %d", tft->packet_filter_list[i].id);
          return SRSRAN_ERROR_CANT_START;
        }

        // release old filter
        tft_filter_map.erase(old_filter);

        // Add new filter
        tft_packet_filter_t new_filter(eps_bearer_id, tft->packet_filter_list[i], logger);
        auto                it = tft_filter_map.insert(std::make_pair(new_filter.eval_precedence, new_filter));
        if (it.second == false) {
          logger.error("Error inserting TFT Packet Filter");
          return SRSRAN_ERROR_CANT_START;
        }
      }
      break;
    default:
      logger.error("Unhandled TFT OP code");
      return SRSRAN_ERROR_CANT_START;
  }
  return SRSRAN_SUCCESS;
}

} // namespace srsue
