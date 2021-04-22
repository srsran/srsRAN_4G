/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/common/mac_pcap_base.h"
#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include <stdint.h>

namespace srsran {

mac_pcap_base::mac_pcap_base() : logger(srslog::fetch_basic_logger("MAC")), thread("PCAP_WRITER_MAC") {}

mac_pcap_base::~mac_pcap_base() {}

void mac_pcap_base::enable(bool enable_)
{
  std::lock_guard<std::mutex> lock(mutex);
  running = enable_;
}

void mac_pcap_base::set_ue_id(uint16_t ue_id_)
{
  std::lock_guard<std::mutex> lock(mutex);
  ue_id = ue_id_;
}

void mac_pcap_base::run_thread()
{
  // blocking write until stopped
  while (running) {
    pcap_pdu_t pdu = queue.pop_blocking();
    {
      std::lock_guard<std::mutex> lock(mutex);
      write_pdu(pdu);
    }
  }

  // write remainder of queue
  pcap_pdu_t                  pdu = {};
  while (queue.try_pop(pdu)) {
    std::lock_guard<std::mutex> lock(mutex);
    write_pdu(pdu);
  }
}

// Function called from PHY worker context, locking not needed as PDU queue is thread-safe
void mac_pcap_base::pack_and_queue(uint8_t* payload,
                                   uint32_t payload_len,
                                   uint16_t ue_id,
                                   uint32_t reTX,
                                   bool     crc_ok,
                                   uint8_t  cc_idx,
                                   uint32_t tti,
                                   uint16_t crnti,
                                   uint8_t  direction,
                                   uint8_t  rnti_type)
{
  if (running && payload != nullptr) {
    pcap_pdu_t pdu             = {};
    pdu.rat                    = srsran::srsran_rat_t::lte;
    pdu.context.radioType      = FDD_RADIO;
    pdu.context.direction      = direction;
    pdu.context.rntiType       = rnti_type;
    pdu.context.rnti           = crnti;
    pdu.context.ueid           = ue_id;
    pdu.context.isRetx         = (uint8_t)reTX;
    pdu.context.crcStatusOK    = crc_ok;
    pdu.context.cc_idx         = cc_idx;
    pdu.context.sysFrameNumber = (uint16_t)(tti / 10);
    pdu.context.subFrameNumber = (uint16_t)(tti % 10);

    // try to allocate PDU buffer
    pdu.pdu = srsran::make_byte_buffer();
    if (pdu.pdu != nullptr && pdu.pdu->get_tailroom() >= payload_len) {
      // copy payload into PDU buffer
      memcpy(pdu.pdu->msg, payload, payload_len);
      pdu.pdu->N_bytes = payload_len;
      if (not queue.try_push(std::move(pdu))) {
        logger.warning("Dropping PDU (%d B) in PCAP. Write queue full.", payload_len);
      }
    } else {
      logger.warning("Dropping PDU in PCAP. No buffer available or not enough space (pdu_len=%d).", payload_len);
    }
  }
}

// Function called from PHY worker context, locking not needed as PDU queue is thread-safe
void mac_pcap_base::pack_and_queue_nr(uint8_t* payload,
                                      uint32_t payload_len,
                                      uint32_t tti,
                                      uint16_t crnti,
                                      uint16_t ue_id,
                                      uint8_t  harqid,
                                      uint8_t  direction,
                                      uint8_t  rnti_type)
{
  if (running && payload != nullptr) {
    pcap_pdu_t pdu                     = {};
    pdu.rat                            = srsran_rat_t::nr;
    pdu.context_nr.radioType           = FDD_RADIO;
    pdu.context_nr.direction           = direction;
    pdu.context_nr.rntiType            = rnti_type;
    pdu.context_nr.rnti                = crnti;
    pdu.context_nr.ueid                = ue_id;
    pdu.context_nr.harqid              = harqid;
    pdu.context_nr.system_frame_number = tti / 10;
    pdu.context_nr.sub_frame_number    = tti % 10;

    // try to allocate PDU buffer
    pdu.pdu = srsran::make_byte_buffer();
    if (pdu.pdu != nullptr && pdu.pdu->get_tailroom() >= payload_len) {
      // copy payload into PDU buffer
      memcpy(pdu.pdu->msg, payload, payload_len);
      pdu.pdu->N_bytes = payload_len;
      if (not queue.try_push(std::move(pdu))) {
        logger.warning("Dropping PDU (%d B) in NR PCAP. Write queue full.", payload_len);
      }
    } else {
      logger.warning("Dropping PDU in NR PCAP. No buffer available or not enough space (pdu_len=%d).", payload_len);
    }
  }
}

void mac_pcap_base::write_dl_crnti(uint8_t* pdu,
                                   uint32_t pdu_len_bytes,
                                   uint16_t rnti,
                                   bool     crc_ok,
                                   uint32_t tti,
                                   uint8_t  cc_idx)
{
  pack_and_queue(pdu, pdu_len_bytes, ue_id, 0, crc_ok, cc_idx, tti, rnti, DIRECTION_DOWNLINK, C_RNTI);
}
void mac_pcap_base::write_dl_ranti(uint8_t* pdu,
                                   uint32_t pdu_len_bytes,
                                   uint16_t rnti,
                                   bool     crc_ok,
                                   uint32_t tti,
                                   uint8_t  cc_idx)
{
  pack_and_queue(pdu, pdu_len_bytes, ue_id, 0, crc_ok, cc_idx, tti, rnti, DIRECTION_DOWNLINK, RA_RNTI);
}
void mac_pcap_base::write_ul_crnti(uint8_t* pdu,
                                   uint32_t pdu_len_bytes,
                                   uint16_t rnti,
                                   uint32_t reTX,
                                   uint32_t tti,
                                   uint8_t  cc_idx)
{
  pack_and_queue(pdu, pdu_len_bytes, ue_id, reTX, true, cc_idx, tti, rnti, DIRECTION_UPLINK, C_RNTI);
}

void mac_pcap_base::write_ul_crnti(uint8_t* pdu,
                                   uint32_t pdu_len_bytes,
                                   uint16_t rnti,
                                   uint16_t ue_id,
                                   uint32_t reTX,
                                   uint32_t tti,
                                   uint8_t  cc_idx)
{
  pack_and_queue(pdu, pdu_len_bytes, ue_id, reTX, true, cc_idx, tti, rnti, DIRECTION_UPLINK, C_RNTI);
}

void mac_pcap_base::write_dl_crnti(uint8_t* pdu,
                                   uint32_t pdu_len_bytes,
                                   uint16_t rnti,
                                   uint16_t ue_id,
                                   bool     crc_ok,
                                   uint32_t tti,
                                   uint8_t  cc_idx)
{
  pack_and_queue(pdu, pdu_len_bytes, ue_id, 0, crc_ok, cc_idx, tti, rnti, DIRECTION_DOWNLINK, C_RNTI);
}

void mac_pcap_base::write_sl_crnti(uint8_t* pdu,
                                   uint32_t pdu_len_bytes,
                                   uint16_t rnti,
                                   uint32_t reTX,
                                   uint32_t tti,
                                   uint8_t  cc_idx)
{
  pack_and_queue(pdu, pdu_len_bytes, ue_id, reTX, true, cc_idx, tti, rnti, DIRECTION_UPLINK, SL_RNTI);
}

void mac_pcap_base::write_dl_bch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti, uint8_t cc_idx)
{
  pack_and_queue(pdu, pdu_len_bytes, ue_id, 0, crc_ok, cc_idx, tti, 0, DIRECTION_DOWNLINK, NO_RNTI);
}
void mac_pcap_base::write_dl_pch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti, uint8_t cc_idx)
{
  pack_and_queue(pdu, pdu_len_bytes, ue_id, 0, crc_ok, cc_idx, tti, SRSRAN_PRNTI, DIRECTION_DOWNLINK, P_RNTI);
}
void mac_pcap_base::write_dl_mch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti, uint8_t cc_idx)
{
  pack_and_queue(pdu, pdu_len_bytes, ue_id, 0, crc_ok, cc_idx, tti, SRSRAN_MRNTI, DIRECTION_DOWNLINK, M_RNTI);
}
void mac_pcap_base::write_dl_sirnti(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti, uint8_t cc_idx)
{
  pack_and_queue(pdu, pdu_len_bytes, ue_id, 0, crc_ok, cc_idx, tti, SRSRAN_SIRNTI, DIRECTION_DOWNLINK, SI_RNTI);
}

void mac_pcap_base::write_dl_crnti_nr(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti)
{
  pack_and_queue_nr(pdu, pdu_len_bytes, tti, rnti, ue_id, harqid, DIRECTION_DOWNLINK, C_RNTI);
}

void mac_pcap_base::write_dl_crnti_nr(uint8_t* pdu,
                                      uint32_t pdu_len_bytes,
                                      uint16_t crnti,
                                      uint16_t ue_id,
                                      uint8_t  harqid,
                                      uint32_t tti)
{
  pack_and_queue_nr(pdu, pdu_len_bytes, tti, crnti, ue_id, harqid, DIRECTION_DOWNLINK, C_RNTI);
}

void mac_pcap_base::write_ul_crnti_nr(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti)
{
  pack_and_queue_nr(pdu, pdu_len_bytes, tti, rnti, ue_id, harqid, DIRECTION_UPLINK, C_RNTI);
}

void mac_pcap_base::write_ul_crnti_nr(uint8_t* pdu,
                                      uint32_t pdu_len_bytes,
                                      uint16_t rnti,
                                      uint16_t ue_id,
                                      uint8_t  harqid,
                                      uint32_t tti)
{
  pack_and_queue_nr(pdu, pdu_len_bytes, tti, rnti, ue_id, harqid, DIRECTION_UPLINK, C_RNTI);
}

void mac_pcap_base::write_dl_ra_rnti_nr(uint8_t* pdu,
                                        uint32_t pdu_len_bytes,
                                        uint16_t rnti,
                                        uint8_t  harqid,
                                        uint32_t tti)
{
  pack_and_queue_nr(pdu, pdu_len_bytes, tti, rnti, ue_id, harqid, DIRECTION_DOWNLINK, RA_RNTI);
}

void mac_pcap_base::write_dl_bch_nr(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti)
{
  pack_and_queue_nr(pdu, pdu_len_bytes, tti, rnti, ue_id, harqid, DIRECTION_DOWNLINK, NO_RNTI);
}

void mac_pcap_base::write_dl_pch_nr(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti)
{
  pack_and_queue_nr(pdu, pdu_len_bytes, tti, rnti, ue_id, harqid, DIRECTION_DOWNLINK, P_RNTI);
}

void mac_pcap_base::write_dl_si_rnti_nr(uint8_t* pdu,
                                        uint32_t pdu_len_bytes,
                                        uint16_t rnti,
                                        uint8_t  harqid,
                                        uint32_t tti)
{
  pack_and_queue_nr(pdu, pdu_len_bytes, tti, rnti, ue_id, harqid, DIRECTION_DOWNLINK, SI_RNTI);
}

void mac_pcap_base::write_ul_rrc_pdu(const uint8_t* input, const int32_t input_len)
{
  uint8_t pdu[1024];
  bzero(pdu, sizeof(pdu));

  // Size is limited by PDU buffer and MAC subheader (format 1 < 128 B)
  if (input_len > 128 - 7) {
    logger.error("PDU too large.");
    return;
  }

  //  MAC PDU Header (Short BSR) (1:54) (Padding:remainder)  [3 subheaders]
  //  Sub-header (lcid=Short BSR)
  //      0... .... = SCH reserved bit: 0x0
  //      .0.. .... = Format2: Data length is < 32768 bytes
  //      ..1. .... = Extension: 0x1
  //      ...1 1101 = LCID: Short BSR (0x1d)
  //  Sub-header (lcid=1, length=54)
  //      0... .... = SCH reserved bit: 0x0
  //      .0.. .... = Format2: Data length is < 32768 bytes
  //      ..1. .... = Extension: 0x1
  //      ...0 0001 = LCID: 1 (0x01)
  //      0... .... = Format: Data length is < 128 bytes
  //      .011 0110 = Length: 54 (Will be dynamically updated)
  //  Sub-header (lcid=Padding, length is remainder)
  //      0... .... = SCH reserved bit: 0x0
  //      .0.. .... = Format2: Data length is < 32768 bytes
  //      ..0. .... = Extension: 0x0
  //      ...1 1111 = LCID: Padding (0x1f)
  uint8_t mac_hdr[] = {0x3D, 0x21, 0x36, 0x1F, 0x0C};

  // Update MAC length
  mac_hdr[2] = input_len + 7; // rlc_hdr (2) + pdcp_hdr (1) + MAC (4)

  //  AM Header  (P) sn=4
  //  1... .... = Frame type: Data PDU (0x1)
  //  .0.. .... = Re-segmentation Flag: AMD PDU (0x0)
  //  ..1. .... = Polling Bit: Status report is requested (0x1)
  //  ...0 0... = Framing Info: First byte begins a RLC SDU and last byte ends a RLC SDU (0x0)
  //  .... .0.. = Extension: Data field follows from the octet following the fixed part of the header (0x0)
  //  .... ..00 0000 0100 = Sequence Number: 4
  uint8_t rlc_hdr[] = {0xA0, 0x04};

  //  PDCP-LTE sn=3
  //  000. .... = Reserved: 0
  //  ...0 0011 = Seq Num: 3
  uint8_t pdcp_hdr[] = {0x03};

  uint8_t* pdu_ptr = pdu;

  memcpy(pdu_ptr, mac_hdr, sizeof(mac_hdr));
  pdu_ptr += sizeof(mac_hdr);
  memcpy(pdu_ptr, rlc_hdr, sizeof(rlc_hdr));
  pdu_ptr += sizeof(rlc_hdr);
  memcpy(pdu_ptr, pdcp_hdr, sizeof(pdcp_hdr));
  pdu_ptr += sizeof(pdcp_hdr);
  memcpy(pdu_ptr, input, input_len);
  pdu_ptr += input_len;

  // MAC
  uint8_t pad = 0x00;
  for (uint32_t i = 0; i < 4; i++) {
    memcpy(pdu_ptr, &pad, 1);
    pdu_ptr += 1;
  }

  // Pad
  memcpy(pdu_ptr, &pad, 1);
  pdu_ptr += 1;

  write_ul_crnti(pdu, pdu_ptr - pdu, 14931, true, 0, 0);
}
} // namespace srsran
