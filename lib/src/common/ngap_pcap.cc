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

#include "srsran/common/ngap_pcap.h"
#include "srsran/common/pcap.h"
#include "srsran/srsran.h"
#include "srsran/support/emergency_handlers.h"
#include <stdint.h>

namespace srsran {

/// Try to flush the contents of the pcap class before the application is killed.
static void emergency_cleanup_handler(void* data)
{
  reinterpret_cast<ngap_pcap*>(data)->close();
}

ngap_pcap::ngap_pcap()
{
  add_emergency_cleanup_handler(emergency_cleanup_handler, this);
}

void ngap_pcap::enable()
{
  enable_write = true;
}
void ngap_pcap::open(const char* filename_)
{
  filename     = filename_;
  pcap_file    = DLT_PCAP_Open(NGAP_5G_DLT, filename.c_str());
  enable_write = true;
}
void ngap_pcap::close()
{
  if (!enable_write) {
    return;
  }
  fprintf(stdout, "Saving NGAP PCAP file (DLT=%d) to %s\n", NGAP_5G_DLT, filename.c_str());
  DLT_PCAP_Close(pcap_file);
}

void ngap_pcap::write_ngap(uint8_t* pdu, uint32_t pdu_len_bytes)
{
  if (enable_write) {
    NGAP_Context_Info_t context;
    if (pdu) {
      LTE_PCAP_NGAP_WritePDU(pcap_file, &context, pdu, pdu_len_bytes);
    }
  }
}

} // namespace srsran
