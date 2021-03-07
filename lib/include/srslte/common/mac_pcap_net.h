/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSLTE_MAC_PCAP_NET_H
#define SRSLTE_MAC_PCAP_NET_H

#include "srslte/common/common.h"
#include "srslte/common/mac_pcap_base.h"
#include "srslte/common/network_utils.h"
#include "srslte/srslte.h"

namespace srslte {
class mac_pcap_net : public mac_pcap_base
{
public:
  mac_pcap_net();
  ~mac_pcap_net();
  uint32_t open(std::string client_ip_addr_,
                std::string bind_addr_str    = "0.0.0.0",
                uint16_t    client_udp_port_ = 5847,
                uint16_t    bind_udp_port_   = 5687,
                uint32_t    ue_id_           = 0);
  uint32_t close();

private:
  void write_pdu(srslte::mac_pcap_base::pcap_pdu_t& pdu);
  void write_mac_lte_pdu_to_net(srslte::mac_pcap_base::pcap_pdu_t& pdu);
  void write_mac_nr_pdu_to_net(srslte::mac_pcap_base::pcap_pdu_t& pdu);

  srslte::socket_handler_t socket;
  struct sockaddr_in       client_addr;
};
} // namespace srslte

#endif // SRSLTE_MAC_PCAP_NET_H
