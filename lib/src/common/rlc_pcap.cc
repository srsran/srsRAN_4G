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

#include "srslte/common/rlc_pcap.h"
#include "srslte/common/pcap.h"
#include "srslte/srslte.h"
#include <stdint.h>

namespace srslte {

void rlc_pcap::enable(bool en)
{
  enable_write = true;
}

void rlc_pcap::open(const char* filename, rlc_config_t config)
{
  fprintf(stdout, "Opening RLC PCAP with DLT=%d\n", UDP_DLT);
  pcap_file    = LTE_PCAP_Open(UDP_DLT, filename);
  enable_write = true;

  if (config.rlc_mode == rlc_mode_t::am) {
    mode      = RLC_AM_MODE;
    sn_length = AM_SN_LENGTH_10_BITS;
  } else if (config.rlc_mode == rlc_mode_t::um) {
    mode = RLC_UM_MODE;
    if (config.um.rx_sn_field_length == rlc_umd_sn_size_t::size5bits) {
      sn_length = UM_SN_LENGTH_5_BITS;
    } else {
      sn_length = UM_SN_LENGTH_10_BITS;
    }
  } else {
    mode = RLC_TM_MODE;
  }
}
void rlc_pcap::close()
{
  fprintf(stdout, "Saving RLC PCAP file\n");
  LTE_PCAP_Close(pcap_file);
}

void rlc_pcap::set_ue_id(uint16_t ue_id_)
{
  ue_id = ue_id_;
}

void rlc_pcap::pack_and_write(uint8_t* pdu,
                              uint32_t pdu_len_bytes,
                              uint8_t  mode_,
                              uint8_t  direction,
                              uint8_t  priority,
                              uint8_t  seqnumberlength,
                              uint16_t ueid,
                              uint16_t channel_type,
                              uint16_t channel_id)
{
  if (enable_write) {
    RLC_Context_Info_t context;
    context.rlcMode              = mode_;
    context.direction            = direction;
    context.priority             = priority;
    context.sequenceNumberLength = seqnumberlength;
    context.ueid                 = ueid;
    context.channelType          = channel_type;
    context.channelId            = channel_id;
    context.pduLength            = pdu_len_bytes;
    if (pdu) {
      LTE_PCAP_RLC_WritePDU(pcap_file, &context, pdu, pdu_len_bytes);
    }
  }
}

void rlc_pcap::write_dl_ccch(uint8_t* pdu, uint32_t pdu_len_bytes)
{
  uint8_t priority   = 0;
  uint8_t channel_id = CHANNEL_TYPE_DRB;
  pack_and_write(
      pdu, pdu_len_bytes, mode, DIRECTION_DOWNLINK, priority, sn_length, ue_id, CHANNEL_TYPE_CCCH, channel_id);
}

void rlc_pcap::write_ul_ccch(uint8_t* pdu, uint32_t pdu_len_bytes)
{
  uint8_t priority   = 0;
  uint8_t channel_id = CHANNEL_TYPE_DRB;
  pack_and_write(pdu, pdu_len_bytes, mode, DIRECTION_UPLINK, priority, sn_length, ue_id, CHANNEL_TYPE_CCCH, channel_id);
}

} // namespace srslte
