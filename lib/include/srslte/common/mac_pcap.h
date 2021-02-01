/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_MAC_PCAP_H
#define SRSLTE_MAC_PCAP_H

#include "srslte/common/block_queue.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/logmap.h"
#include "srslte/common/pcap.h"
#include "srslte/common/threads.h"
#include <mutex>
#include <stdint.h>
#include <thread>
namespace srslte {
class mac_pcap : srslte::thread
{
public:
  mac_pcap();
  ~mac_pcap();
  void     enable(bool en);
  uint32_t open(const char* filename, uint32_t ue_id = 0);
  uint32_t close();

  void set_ue_id(uint16_t ue_id);

  void
       write_ul_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t crnti, uint32_t reTX, uint32_t tti, uint8_t cc_idx);
  void write_dl_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t crnti, bool crc_ok, uint32_t tti, uint8_t cc_idx);
  void write_dl_ranti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t ranti, bool crc_ok, uint32_t tti, uint8_t cc_idx);

  // SI and BCH only for DL
  void write_dl_sirnti(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti, uint8_t cc_idx);
  void write_dl_bch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti, uint8_t cc_idx);
  void write_dl_pch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti, uint8_t cc_idx);
  void write_dl_mch(uint8_t* pdu, uint32_t pdu_len_bytes, bool crc_ok, uint32_t tti, uint8_t cc_idx);

  void write_ul_rrc_pdu(const uint8_t* input, const int32_t input_len);

  // Sidelink
  void write_sl_crnti(uint8_t* pdu, uint32_t pdu_len_bytes, uint16_t rnti, uint32_t reTX, uint32_t tti, uint8_t cc_idx);

private:
  srslte::byte_buffer_pool* pool = nullptr;
  srslog::basic_logger&     logger;
  bool                      running   = false;
  FILE*                     pcap_file = nullptr;
  uint32_t                  ue_id     = 0;
  void                      pack_and_queue(uint8_t* pdu,
                                           uint32_t pdu_len_bytes,
                                           uint32_t reTX,
                                           bool     crc_ok,
                                           uint8_t  cc_idx,
                                           uint32_t tti,
                                           uint16_t crnti_,
                                           uint8_t  direction,
                                           uint8_t  rnti_type);

  typedef struct {
    MAC_Context_Info_t   context;
    unique_byte_buffer_t pdu;
  } pcap_pdu_t;
  block_queue<pcap_pdu_t> queue;
  std::mutex              mutex;

  void write_pdu(pcap_pdu_t& pdu);
  void run_thread() final;
};

} // namespace srslte

#endif // SRSLTE_MAC_PCAP_H
