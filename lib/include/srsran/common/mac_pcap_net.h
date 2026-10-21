// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_MAC_PCAP_NET_H
#define SRSRAN_MAC_PCAP_NET_H

#include "srsran/common/common.h"
#include "srsran/common/mac_pcap_base.h"
#include "srsran/common/network_utils.h"
#include "srsran/srsran.h"

namespace srsran {
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
  void write_pdu(srsran::mac_pcap_base::pcap_pdu_t& pdu);
  void write_mac_lte_pdu_to_net(srsran::mac_pcap_base::pcap_pdu_t& pdu);
  void write_mac_nr_pdu_to_net(srsran::mac_pcap_base::pcap_pdu_t& pdu);

  srsran::unique_socket socket;
  struct sockaddr_in    client_addr;
};
} // namespace srsran

#endif // SRSRAN_MAC_PCAP_NET_H
