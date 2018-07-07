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
#include "srslte/common/rlc_pcap.h"

namespace srslte {
 
void rlc_pcap::enable(bool en)
{
  enable_write = true; 
}
void rlc_pcap::open(const char* filename, uint32_t ue_id)
{
  fprintf(stdout, "Opening RLC PCAP with DLT=%d\n", RLC_LTE_DLT);
  pcap_file = LTE_PCAP_Open(RLC_LTE_DLT, filename);
  this->ue_id = ue_id;
  enable_write = true;
}
void rlc_pcap::close()
{
  fprintf(stdout, "Saving RLC PCAP file\n");
  LTE_PCAP_Close(pcap_file);
}

void rlc_pcap::set_ue_id(uint16_t ue_id) {
  this->ue_id = ue_id;
}

void rlc_pcap::pack_and_write(uint8_t* pdu, uint32_t pdu_len_bytes, uint8_t mode, uint8_t direction, uint8_t priority, uint8_t seqnumberlength, uint16_t ueid, uint16_t channel_type, uint16_t channel_id)
{
  if (enable_write) {
    RLC_Context_Info_t context;
    context.rlcMode = mode;
    context.direction = direction;
    context.priority = priority;
    context.sequenceNumberLength = seqnumberlength;
    context.ueid = ueid;
    context.channelType = channel_type;
    context.channelId = channel_id;
    context.pduLength = pdu_len_bytes;
    if (pdu) {
      LTE_PCAP_RLC_WritePDU(pcap_file, &context, pdu, pdu_len_bytes);
    }
  }
}

void rlc_pcap::write_dl_am_ccch(uint8_t* pdu, uint32_t pdu_len_bytes)
{
  uint8_t priority = 0;
  uint8_t seqnumberlength = 0; // normal length of 10bit
  uint8_t channel_id = 0;
  pack_and_write(pdu, pdu_len_bytes, RLC_AM_MODE, DIRECTION_DOWNLINK, priority, seqnumberlength, ue_id, CHANNEL_TYPE_CCCH, channel_id);
}

void rlc_pcap::write_ul_am_ccch(uint8_t* pdu, uint32_t pdu_len_bytes)
{
  uint8_t priority = 0;
  uint8_t seqnumberlength = 0; // normal length of 10bit
  uint8_t channel_id = 0;
  pack_and_write(pdu, pdu_len_bytes, RLC_AM_MODE, DIRECTION_UPLINK, priority, seqnumberlength, ue_id, CHANNEL_TYPE_CCCH, channel_id);
}

}
