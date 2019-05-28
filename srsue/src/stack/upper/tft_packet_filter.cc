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

namespace srsue {

tft_packet_filter_t::tft_packet_filter_t(const LIBLTE_MME_PACKET_FILTER_STRUCT* tft) :
  id(tft->id),
  eval_precedence(tft->eval_precedence),
  active_filters(0)
{
  int idx = 0;
  while (idx < tft->filter_size) {
    switch (tft->filter[idx] & 0x0F) {
      case IPV4_REMOTE_ADDR_TYPE:
        active_filters = IPV4_REMOTE_ADDR_FLAG;
        memcpy(&ipv4_remote_addr, &tft->filter[idx], 4);
        idx += 4;
        break;
      case IPV4_LOCAL_ADDR_TYPE:
        active_filters = IPV4_LOCAL_ADDR_FLAG;
        memcpy(&ipv4_local_addr, &tft->filter[idx], 4);
        idx += 4;
        break;
      case IPV6_REMOTE_ADDR_TYPE:
        active_filters = IPV6_REMOTE_ADDR_FLAG;
        memcpy(&ipv4_local_addr, &tft->filter[idx], 16);
        idx += 16;
        break;
      case IPV6_REMOTE_ADDR_LENGTH_TYPE:
        break;
      case IPV6_LOCAL_ADDR_LENGTH_TYPE:
        break;
      case PROTOCOL_ID_TYPE:
        break;
      case SINGLE_LOCAL_PORT_TYPE:
        active_filters = SINGLE_LOCAL_PORT_FLAG;
        memcpy(&single_local_port, &tft->filter[idx], 2);
        idx += 2;
        break;
      case LOCAL_PORT_RANGE_TYPE:
        break;
      case SINGLE_REMOTE_PORT_TYPE:
        break;
      case REMOTE_PORT_RANGE_TYPE:
        break;
      case SECURITY_PARAMETER_INDEX_TYPE:
        break;
      case TYPE_OF_SERVICE_TYPE:
        break;
      case FLOW_LABEL_TYPE:
        break;
      default:
        return;
    }
  }
}

tft_packet_filter_t::~tft_packet_filter_t() {}

} // namespace srsue
