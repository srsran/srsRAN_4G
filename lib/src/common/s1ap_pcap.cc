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

#include "srsran/common/s1ap_pcap.h"
#include "srsran/common/pcap.h"
#include "srsran/srsran.h"
#include "srsran/support/emergency_handlers.h"
#include <stdint.h>

namespace srsran {

/// Try to flush the contents of the pcap class before the application is killed.
static void emergency_cleanup_handler(void* data)
{
  reinterpret_cast<s1ap_pcap*>(data)->close();
}

s1ap_pcap::s1ap_pcap()
{
  add_emergency_cleanup_handler(emergency_cleanup_handler, this);
}

void s1ap_pcap::enable()
{
  enable_write = true;
}
void s1ap_pcap::open(const char* filename_)
{
  filename     = filename_;
  pcap_file    = DLT_PCAP_Open(S1AP_LTE_DLT, filename.c_str());
  enable_write = true;
}
void s1ap_pcap::close()
{
  if (!enable_write) {
    return;
  }
  fprintf(stdout, "Saving S1AP PCAP file (DLT=%d) to %s\n", S1AP_LTE_DLT, filename.c_str());
  DLT_PCAP_Close(pcap_file);
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

} // namespace srsran
