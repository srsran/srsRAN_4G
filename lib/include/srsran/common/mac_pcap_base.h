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

#ifndef SRSRAN_MAC_PCAP_BASE_H
#define SRSRAN_MAC_PCAP_BASE_H

#include "srsran/adt/circular_buffer.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/pcap.h"
#include "srsran/common/threads.h"
#include "srsran/srslog/srslog.h"
#include <mutex>
#include <stdint.h>
#include <thread>

namespace srsran {
class mac_pcap_base : protected srsran::thread
{
public:
  mac_pcap_base();

  mac_pcap_base(const mac_pcap_base& other) = delete;
  mac_pcap_base& operator=(const mac_pcap_base& other) = delete;
  mac_pcap_base(mac_pcap_base&& other)                 = delete;
  mac_pcap_base& operator=(mac_pcap_base&& other) = delete;

  ~mac_pcap_base();
  void             enable(bool enable);
  virtual uint32_t close() = 0;

  void set_ue_id(uint16_t ue_id);

  // EUTRA
  void
  write_ul_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t crnti, uint32_t reTX, uint32_t tti, uint8_t cc_idx);
  void write_dl_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t crnti, bool crc_ok, uint32_t tti, uint8_t cc_idx);
  void write_dl_ranti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t ranti, bool crc_ok, uint32_t tti, uint8_t cc_idx);

  void write_ul_crnti(uint8_t* pdu,
                      uint32_t pdu_len_bytes,
                      uint16_t crnti,
                      uint16_t ue_id,
                      uint32_t reTX,
                      uint32_t tti,
                      uint8_t  cc_idx);

  void write_dl_crnti(uint8_t* pdu,
                      uint32_t pdu_len_bytes,
                      uint16_t crnti,
                      uint16_t ue_id,
                      bool     crc_ok,
                      uint32_t tti,
                      uint8_t  cc_idx);

  // SI and BCH only for DL
  void write_dl_sirnti(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti, uint8_t cc_idx);
  void write_dl_bch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti, uint8_t cc_idx);
  void write_dl_pch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti, uint8_t cc_idx);
  void write_dl_mch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti, uint8_t cc_idx);

  void write_ul_rrc_pdu(const uint8_t* input, const int32_t input_len);

  // Sidelink
  void write_sl_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint32_t reTX, uint32_t tti, uint8_t cc_idx);

  // NR
  void write_dl_crnti_nr(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t crnti, uint8_t harqid, uint32_t tti);
  void write_ul_crnti_nr(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti);
  void write_dl_ra_rnti_nr(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti);
  void write_dl_bch_nr(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti);
  void write_dl_pch_nr(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti);
  void write_dl_si_rnti_nr(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint8_t harqid, uint32_t tti);

  // NR for enb with different ue_id
  // clang-format off
  void write_dl_crnti_nr(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t crnti, uint16_t ue_id, uint8_t harqid, uint32_t tti);
  void write_ul_crnti_nr(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint16_t ue_id, uint8_t harqid, uint32_t tti);
  // clang-format on

protected:
  typedef struct {
    // Different PCAP context for both RATs
    srsran::srsran_rat_t  rat;
    MAC_Context_Info_t    context;
    mac_nr_context_info_t context_nr;
    unique_byte_buffer_t  pdu;
  } pcap_pdu_t;

  virtual void write_pdu(pcap_pdu_t& pdu) = 0;
  void         run_thread() final;

  std::mutex                              mutex;
  srslog::basic_logger&                   logger;
  std::atomic<bool>                       running = {false};
  static_blocking_queue<pcap_pdu_t, 1024> queue;
  uint16_t                                ue_id = 0;

private:
  void pack_and_queue(uint8_t* payload,
                      uint32_t payload_len,
                      uint16_t ue_id,
                      uint32_t reTX,
                      bool     crc_ok,
                      uint8_t  cc_idx,
                      uint32_t tti,
                      uint16_t crnti_,
                      uint8_t  direction,
                      uint8_t  rnti_type);
  void pack_and_queue_nr(uint8_t* payload,
                         uint32_t payload_len,
                         uint32_t tti,
                         uint16_t crnti,
                         uint16_t ue_id,
                         uint8_t  harqid,
                         uint8_t  direction,
                         uint8_t  rnti_type);
};

} // namespace srsran

#endif // SRSRAN_MAC_PCAP_BASE_H
