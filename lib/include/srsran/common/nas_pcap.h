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

#ifndef SRSRAN_NAS_PCAP_H
#define SRSRAN_NAS_PCAP_H

#include "srsran/common/common.h"
#include "srsran/common/pcap.h"
#include <string>

namespace srsran {

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
  uint32_t open(std::string filename_, uint32_t ue_id = 0, srsran_rat_t rat_type = srsran_rat_t::lte);
  void close();
  void write_nas(uint8_t* pdu, uint32_t pdu_len_bytes);

private:
  bool        enable_write;
  std::string filename;
  FILE*       pcap_file;
  uint32_t    ue_id;
  void        pack_and_write(uint8_t* pdu, uint32_t pdu_len_bytes);
};

} // namespace srsran

#endif // SRSRAN_NAS_PCAP_H
