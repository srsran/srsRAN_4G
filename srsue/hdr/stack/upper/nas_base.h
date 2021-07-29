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

#ifndef SRSUE_NAS_BASE_H
#define SRSUE_NAS_BASE_H

#include "srsran/asn1/liblte_mme.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/nas_pcap.h"
#include "srsran/common/security.h"
#include "srsran/common/string_helpers.h"
#include "srsran/config.h"

using srsran::byte_buffer_t;

namespace srsue {

class nas_base
{
public:
  nas_base(srslog::basic_logger& logger_, uint32_t mac_offset, uint32_t seq_offset_, uint32_t bearer_id_);
  // PCAP
  void start_pcap(srsran::nas_pcap* pcap_) { pcap = pcap_; }

protected:
  srslog::basic_logger& logger;
  // PCAP
  srsran::nas_pcap* pcap = nullptr;

  // Security context
  struct nas_sec_ctxt {
    uint8_t                              ksi;
    uint8_t                              k_asme[32];
    uint32_t                             tx_count;
    uint32_t                             rx_count;
    uint32_t                             k_enb_count;
    srsran::CIPHERING_ALGORITHM_ID_ENUM  cipher_algo;
    srsran::INTEGRITY_ALGORITHM_ID_ENUM  integ_algo;
    LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT guti;
  };

  nas_sec_ctxt ctxt          = {};
  uint8_t      k_nas_enc[32] = {};
  uint8_t      k_nas_int[32] = {};

  int parse_security_algorithm_list(std::string algorithm_string, bool* algorithm_caps);

  // Security
  void
       integrity_generate(uint8_t* key_128, uint32_t count, uint8_t direction, uint8_t* msg, uint32_t msg_len, uint8_t* mac);
  bool integrity_check(srsran::byte_buffer_t* pdu);
  void cipher_encrypt(srsran::byte_buffer_t* pdu);
  void cipher_decrypt(srsran::byte_buffer_t* pdu);

  uint32_t mac_offset = 0;
  uint32_t seq_offset = 0;
  uint32_t bearer_id  = 0;
};

} // namespace srsue
#endif
