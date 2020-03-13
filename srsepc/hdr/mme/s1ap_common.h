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

#ifndef SRSEPC_S1AP_COMMON_H
#define SRSEPC_S1AP_COMMON_H

#include "srslte/asn1/gtpc_ies.h"
#include "srslte/asn1/liblte_mme.h"
#include "srslte/asn1/s1ap_asn1.h"
#include "srslte/common/security.h"
#include <netinet/sctp.h>
#include <string.h>

namespace srsepc {

static const uint8_t MAX_TA    = 255; // Maximum TA supported
static const uint8_t MAX_BPLMN = 6;   // Maximum broadcasted PLMNs per TAC

typedef struct {
  uint8_t                             mme_code;
  uint16_t                            mme_group;
  uint16_t                            tac;          // 16-bit tac
  uint16_t                            mcc;          // BCD-coded with 0xF filler
  uint16_t                            mnc;          // BCD-coded with 0xF filler
  uint16_t                            paging_timer; // Paging timer in sec (T3413)
  std::string                         mme_bind_addr;
  std::string                         mme_name;
  std::string                         dns_addr;
  std::string                         mme_apn;
  bool                                pcap_enable;
  std::string                         pcap_filename;
  srslte::CIPHERING_ALGORITHM_ID_ENUM encryption_algo;
  srslte::INTEGRITY_ALGORITHM_ID_ENUM integrity_algo;
} s1ap_args_t;

typedef struct {
  bool                                                enb_name_present;
  uint32_t                                            enb_id;
  std::string                                         enb_name;
  uint16_t                                            mcc, mnc;
  uint32_t                                            plmn;
  uint8_t                                             nof_supported_ta;
  std::array<uint8_t, MAX_TA>                         tac;
  std::array<uint16_t, MAX_BPLMN>                     nof_supported_bplmns;
  std::array<std::array<uint16_t, MAX_BPLMN>, MAX_TA> bplmns;
  asn1::s1ap::paging_drx_opts                         drx;
  struct sctp_sndrcvinfo                              sri;
} enb_ctx_t;

} // namespace srsepc

#endif // SRSEPC_S1AP_COMMON_H
