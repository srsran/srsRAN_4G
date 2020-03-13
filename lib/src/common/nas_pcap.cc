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

#include "srslte/common/nas_pcap.h"
#include "srslte/common/pcap.h"
#include "srslte/srslte.h"
#include <stdint.h>

namespace srslte {

void nas_pcap::enable()
{
  enable_write = true;
}
void nas_pcap::open(const char* filename, uint32_t ue_id_)
{
  pcap_file    = LTE_PCAP_Open(NAS_LTE_DLT, filename);
  ue_id        = ue_id_;
  enable_write = true;
}
void nas_pcap::close()
{
  fprintf(stdout, "Saving NAS PCAP file (DLT=%d)\n", NAS_LTE_DLT);
  LTE_PCAP_Close(pcap_file);
}

void nas_pcap::write_nas(uint8_t* pdu, uint32_t pdu_len_bytes)
{
  if (enable_write) {
    NAS_Context_Info_t context;
    if (pdu) {
      LTE_PCAP_NAS_WritePDU(pcap_file, &context, pdu, pdu_len_bytes);
    }
  }
}

} // namespace srslte
