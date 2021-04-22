/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

namespace srsran {

class s1ap_pcap
{
public:
  s1ap_pcap()
  {
    enable_write = false;
    pcap_file    = NULL;
  }
  void enable();
  void open(const char* filename);
  void close();
  void write_s1ap(uint8_t* pdu, uint32_t pdu_len_bytes);

private:
  bool  enable_write;
  FILE* pcap_file;
};

} // namespace srsran

#endif // SRSRAN_NAS_PCAP_H
