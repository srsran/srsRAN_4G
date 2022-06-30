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

#ifndef SRSRAN_S1AP_PCAP_H
#define SRSRAN_S1AP_PCAP_H

#include "srsran/common/pcap.h"
#include <string>

namespace srsran {

class s1ap_pcap
{
public:
  s1ap_pcap();
  ~s1ap_pcap();
  s1ap_pcap(const s1ap_pcap& other) = delete;
  s1ap_pcap& operator=(const s1ap_pcap& other) = delete;
  s1ap_pcap(s1ap_pcap&& other)                 = delete;
  s1ap_pcap& operator=(s1ap_pcap&& other) = delete;

  void enable();
  void open(const char* filename_);
  void close();
  void write_s1ap(uint8_t* pdu, uint32_t pdu_len_bytes);

private:
  bool        enable_write = false;
  std::string filename;
  FILE*       pcap_file            = nullptr;
  int         emergency_handler_id = -1;
};

} // namespace srsran

#endif // SRSRAN_NAS_PCAP_H
