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

#include "srsran/common/rlc_pcap.h"
#include "srsran/common/pcap.h"
#include "srsran/srsran.h"
#include <stdint.h>

namespace srsran {

void rlc_pcap::enable(bool en)
{
  enable_write = true;
}

void rlc_pcap::open(const char* filename, const rlc_config_t& config)
{
  fprintf(stdout, "Opening RLC PCAP with DLT=%d\n", UDP_DLT);
  pcap_file    = DLT_PCAP_Open(UDP_DLT, filename);
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
  DLT_PCAP_Close(pcap_file);
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

} // namespace srsran
