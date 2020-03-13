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

#include "srslte/common/mac_nr_pcap.h"
#include "srslte/common/pcap.h"
#include <stdint.h>

namespace srslte {

mac_nr_pcap::mac_nr_pcap() {}

mac_nr_pcap::~mac_nr_pcap()
{
  if (pcap_file) {
    close();
  }
}

void mac_nr_pcap::enable(const bool& enable_)
{
  enable_write = enable_;
}
void mac_nr_pcap::open(const std::string& filename_, const uint16_t& ue_id_)
{
  fprintf(stdout, "Opening MAC-NR PCAP with DLT=%d\n", UDP_DLT);
  filename     = filename_;
  pcap_file    = LTE_PCAP_Open(UDP_DLT, filename.c_str());
  ue_id        = ue_id_;
  enable_write = true;
}
void mac_nr_pcap::close()
{
  enable_write = false;
  fprintf(stdout, "Saving MAC-NR PCAP to %s\n", filename.c_str());
  LTE_PCAP_Close(pcap_file);
  pcap_file = nullptr;
}

void mac_nr_pcap::set_ue_id(const uint16_t& ue_id_)
{
  ue_id = ue_id_;
}

void mac_nr_pcap::pack_and_write(uint8_t* pdu,
                                 uint32_t pdu_len_bytes,
                                 uint32_t tti,
                                 uint16_t crnti,
                                 uint8_t  harqid,
                                 uint8_t  direction,
                                 uint8_t  rnti_type)
{
  if (enable_write) {
    mac_nr_context_info_t context = {};
    context.radioType             = FDD_RADIO;
    context.direction             = direction;
    context.rntiType              = rnti_type;
    context.rnti                  = crnti;
    context.ueid                  = ue_id;
    context.harqid                = harqid;
    context.system_frame_number   = tti / 10;
    context.sub_frame_number      = tti % 10;

    if (pdu) {
      NR_PCAP_MAC_WritePDU(pcap_file, &context, pdu, pdu_len_bytes);
    }
  }
}

void mac_nr_pcap::write_dl_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, tti, rnti, harqid, DIRECTION_DOWNLINK, C_RNTI);
}

void mac_nr_pcap::write_ul_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, tti, rnti, harqid, DIRECTION_UPLINK, C_RNTI);
}

void mac_nr_pcap::write_dl_ra_rnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, tti, rnti, harqid, DIRECTION_DOWNLINK, RA_RNTI);
}

void mac_nr_pcap::write_dl_bch(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, tti, rnti, harqid, DIRECTION_DOWNLINK, NO_RNTI);
}

void mac_nr_pcap::write_dl_pch(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, tti, rnti, harqid, DIRECTION_DOWNLINK, P_RNTI);
}

void mac_nr_pcap::write_dl_si_rnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti)
{
  pack_and_write(pdu, pdu_len_bytes, tti, rnti, harqid, DIRECTION_DOWNLINK, SI_RNTI);
}

} // namespace srslte
