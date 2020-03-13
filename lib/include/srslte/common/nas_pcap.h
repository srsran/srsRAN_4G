/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#ifndef SRSLTE_NAS_PCAP_H
#define SRSLTE_NAS_PCAP_H

#include "srslte/common/pcap.h"

namespace srslte {

class nas_pcap
{
public:
  nas_pcap()
  {
    enable_write = false;
    ue_id        = 0;
    pcap_file    = NULL;
  }
  void enable();
  void open(const char* filename, uint32_t ue_id = 0);
  void close();
  void write_nas(uint8_t* pdu, uint32_t pdu_len_bytes);

private:
  bool     enable_write;
  FILE*    pcap_file;
  uint32_t ue_id;
  void     pack_and_write(uint8_t* pdu, uint32_t pdu_len_bytes);
};

} // namespace srslte

#endif // SRSLTE_NAS_PCAP_H
