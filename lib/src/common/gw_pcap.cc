/**
 * @file gw_pcap.cc
 * @author Zishuai CHENG (5786734+WingPig99@users.noreply.github.com)
 * @brief
 * @version 0.1
 * @date 2022-06-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "srsran/common/gw_pcap.h"
#include "srsran/common/pcap.h"
#include "srsran/srsran.h"

namespace srsran {
gw_pcap::gw_pcap() : enable_write(false) {}

gw_pcap::~gw_pcap()
{
  close();
}

void gw_pcap::enable()
{
  enable_write = true;
}

uint32_t gw_pcap::open(const char* filename)
{
  // open a file to write GW traffic
  pcap_filename.assign(filename, sizeof(filename));
  pcap_file = DLT_PCAP_Open(GW_DLT, filename);
  if (pcap_file == nullptr) {
    return SRSRAN_ERROR;
  }
  enable_write = true;
  return SRSRAN_SUCCESS;
}

void gw_pcap::close()
{
  enable_write = false;
  if (pcap_file != nullptr) {
    fprintf(stdout, "Saving GW PCAP file (DLT %d) to %s\n", GW_DLT, pcap_filename.c_str());
    DLT_PCAP_Close(pcap_file);
    pcap_file = nullptr;
  }
}

void gw_pcap::write_dl_pdu(uint32_t lcid, uint8_t* pdu, uint32_t pdu_len_bytes)
{
  if (enable_write) {
    GW_Context_Info_t context;
    LTE_PCAP_GW_Write_PDU(pcap_file, &context, pdu, pdu_len_bytes);
  }
}
void gw_pcap::write_dl_pdu_mch(uint32_t lcid, uint8_t* pdu, uint32_t pdu_len_bytes)
{
  if (enable_write) {
    GW_Context_Info_t context;
    LTE_PCAP_GW_Write_PDU(pcap_file, &context, pdu, pdu_len_bytes);
  }
}
void gw_pcap::write_ul_pdu(uint32_t lcid, uint8_t* pdu, uint32_t pdu_len_bytes)
{
  if (enable_write) {
    GW_Context_Info_t context;
    LTE_PCAP_GW_Write_PDU(pcap_file, &context, pdu, pdu_len_bytes);
  }
}
} // namespace srsran