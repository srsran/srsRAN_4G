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

#include "srslte/common/s1ap_pcap.h"
#include "srslte/common/pcap.h"
#include "srslte/srslte.h"
#include <stdint.h>

namespace srslte {

void s1ap_pcap::enable()
{
  enable_write = true;
}
void s1ap_pcap::open(const char* filename)
{
  pcap_file    = LTE_PCAP_Open(S1AP_LTE_DLT, filename);
  enable_write = true;
}
void s1ap_pcap::close()
{
  fprintf(stdout, "Saving S1AP PCAP file\n");
  LTE_PCAP_Close(pcap_file);
}

void s1ap_pcap::write_s1ap(uint8_t* pdu, uint32_t pdu_len_bytes)
{
  if (enable_write) {
    S1AP_Context_Info_t context;
    if (pdu) {
      LTE_PCAP_S1AP_WritePDU(pcap_file, &context, pdu, pdu_len_bytes);
    }
  }
}

} // namespace srslte
