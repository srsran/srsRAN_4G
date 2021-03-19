/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

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