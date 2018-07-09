/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include <stdint.h>
#include "srslte/srslte.h"
#include "srslte/common/pcap.h"
#include "srslte/common/mac_pcap.h"



namespace srslte {
 
void mac_pcap::enable(bool en)
{
  enable_write = true; 
}
void mac_pcap::open(const char* filename, uint32_t ue_id)
{
  pcap_file = LTE_PCAP_Open(MAC_LTE_DLT, filename);
  this->ue_id = ue_id;
  enable_write = true;
}
void mac_pcap::close()
{
  fprintf(stdout, "Saving MAC PCAP file\n");
  LTE_PCAP_Close(pcap_file);
}

void mac_pcap::set_ue_id(uint16_t ue_id) {
  this->ue_id = ue_id;
}

void mac_pcap::pack_and_write(uint8_t* pdu, uint32_t pdu_len_bytes, uint32_t reTX, bool crc_ok, uint32_t tti, 
                              uint16_t crnti, uint8_t direction, uint8_t rnti_type)
{
  if (enable_write) {
    MAC_Context_Info_t  context =
    {
        FDD_RADIO, direction, rnti_type,
        crnti,       /* RNTI */
        (uint16_t)ue_id,      /* UEId */
        (uint8_t)reTX,        /* Retx */
        crc_ok,        /* CRC Stsatus (i.e. OK) */
        (uint16_t)(tti/10),        /* Sysframe number */
        (uint16_t)(tti%10)        /* Subframe number */
    };
    if (pdu) {
      LTE_PCAP_MAC_WritePDU(pcap_file, &context, pdu, pdu_len_bytes);
    }
  }
}

void mac_pcap::write_dl_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, bool crc_ok, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, 0, crc_ok, tti, rnti, DIRECTION_DOWNLINK, C_RNTI);
}
void mac_pcap::write_dl_ranti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, bool crc_ok, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, 0, crc_ok, tti, rnti, DIRECTION_DOWNLINK, RA_RNTI);
}
void mac_pcap::write_ul_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint32_t reTX, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, reTX, true, tti, rnti, DIRECTION_UPLINK, C_RNTI);
}
void mac_pcap::write_dl_bch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, 0, crc_ok, tti, 0, DIRECTION_DOWNLINK, NO_RNTI);
}
void mac_pcap::write_dl_pch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, 0, crc_ok, tti, SRSLTE_PRNTI, DIRECTION_DOWNLINK, P_RNTI);
}
void mac_pcap::write_dl_mch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, 0, crc_ok, tti, SRSLTE_MRNTI, DIRECTION_DOWNLINK, M_RNTI);
}
void mac_pcap::write_dl_sirnti(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, 0, crc_ok, tti, SRSLTE_SIRNTI, DIRECTION_DOWNLINK, SI_RNTI);
}

  
}
