/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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


#include <stdint.h>
#include "srslte/srslte.h"
#include "srsapps/ue/mac/pcap.h"
#include "srsapps/ue/mac/mac_pcap.h"


namespace srslte {
namespace ue {
 
void mac_pcap::enable(bool en)
{
  enable_write = true; 
}
bool mac_pcap::open(const char* filename, uint32_t ue_id)
{
  pcap_file = MAC_LTE_PCAP_Open(filename);
  ue_id = ue_id; 
  enable_write = true; 
}
void mac_pcap::close()
{
  fprintf(stdout, "Saving PCAP file\n");
  MAC_LTE_PCAP_Close(pcap_file);
}

void mac_pcap::pack_and_write(uint8_t* pdu, uint32_t pdu_len_bytes, uint32_t reTX, bool crc_ok, uint32_t tti, 
                              uint16_t crnti, uint8_t direction, uint8_t rnti_type)
{
  if (enable_write) {
    if (pdu_len_bytes < max_pdu_len) {
      MAC_Context_Info_t  context =
      {
          FDD_RADIO, direction, rnti_type,
          crnti,       /* RNTI */
          ue_id,      /* UEId */
          reTX,        /* Retx */
          crc_ok,        /* CRC Stsatus (i.e. OK) */
          tti/10,        /* Sysframe number */
          tti%10        /* Subframe number */
      };
      if (pdu) {
        srslte_bit_unpack_vector(pdu, pdu_pcap_tmp, pdu_len_bytes*8);
        MAC_LTE_PCAP_WritePDU(pcap_file, &context, pdu_pcap_tmp, pdu_len_bytes);
      }
    } else {
      fprintf(stderr, "MAC PCAP: PDU len %d exceeds maximum allowed length (%d bytes)\n", pdu_len_bytes, max_pdu_len); 
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
void mac_pcap::write_dl_sirnti(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, 0, crc_ok, tti, SRSLTE_SIRNTI, DIRECTION_DOWNLINK, SI_RNTI);
}

  
}
}