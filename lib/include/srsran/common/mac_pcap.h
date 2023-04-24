/**
 * Copyright 2013-2023 Software Radio Systems Limited
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