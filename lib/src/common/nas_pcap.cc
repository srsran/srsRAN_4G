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

#include "srsran/common/nas_pcap.h"
#include "srsran/common/pcap.h"
#include "srsran/srsran.h"
#include "srsran/support/emergency_handlers.h"
#include <stdint.h>

namespace srsran {

/// Try to flush the contents of the pcap class before the application is killed.
static void emergency_cleanup_handler(void* data)
{
  reinterpret_cast<nas_pcap*>(data)->close();
}

nas_pcap::nas_pcap()
{
  add_emergency_cleanup_handler(emergency_cleanup_handler, this);
}

void nas_pcap::enable()
{
  enable_write = true;
}

uint32_t nas_pcap::open(std::string filename_, uint32_t ue_id_, srsran_rat_t rat_type)
{
  filename = filename_;
  if (rat_type == srsran_rat_t::nr) {
    pcap_file = DLT_PCAP_Open(NAS_5G_DLT, filename.c_str());
  } else {
    pcap_file = DLT_PCAP_Open(NAS_LTE_DLT, filename.c_str());
  }
  if (pcap_file == nullptr) {
    return SRSRAN_ERROR;
  }
  ue_id        = ue_id_;
  enable_write = true;
  return SRSRAN_SUCCESS;
}

void nas_pcap::close()
{
  fprintf(stdout, "Saving NAS PCAP file (DLT=%d) to %s \n", NAS_LTE_DLT, filename.c_str());
  DLT_PCAP_Close(pcap_file);
  pcap_file = nullptr;
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

} // namespace srsran
