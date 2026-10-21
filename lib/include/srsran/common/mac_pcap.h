// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_MAC_PCAP_H
#define SRSRAN_MAC_PCAP_H

#include "srsran/common/common.h"
#include "srsran/common/mac_pcap_base.h"
#include "srsran/srsran.h"

namespace srsran {
class mac_pcap : public mac_pcap_base
{
public:
  mac_pcap();
  ~mac_pcap();
  uint32_t open(std::string filename, uint32_t ue_id = 0);
  uint32_t close();

private:
  void write_pdu(srsran::mac_pcap_base::pcap_pdu_t& pdu);

  FILE*       pcap_file = nullptr;
  uint32_t    dlt       = 0; // The DLT used for the PCAP file
  std::string filename;
};
} // namespace srsran

#endif // SRSRAN_MAC_PCAP_H