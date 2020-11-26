/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
