/*
 * \section LICENSE
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
#ifndef S1AP_COMMON_H
#define S1AP_COMMON_H

#include "srslte/common/security.h"
#include "srslte/asn1/gtpc_ies.h"
#include "srslte/asn1/liblte_s1ap.h"
#include "srslte/asn1/liblte_mme.h"
#include <netinet/sctp.h>

namespace srsepc{
  
static const uint8_t MAX_TA=255;  //Maximum TA supported
static const uint8_t MAX_BPLMN=6; //Maximum broadcasted PLMNs per TAC
static const uint8_t MAX_ERABS_PER_UE = 16;

enum erab_state
{
  ERAB_DEACTIVATED,
  ERAB_CTX_REQUESTED,
  ERAB_CTX_SETUP,
  ERAB_ACTIVE
};

typedef struct{
  uint8_t       mme_code;
  uint16_t      mme_group;
  uint16_t      tac;        // 16-bit tac
  uint16_t      mcc;        // BCD-coded with 0xF filler
  uint16_t      mnc;        // BCD-coded with 0xF filler
  std::string   mme_bind_addr;
  std::string   mme_name;
  std::string   mme_apn;
} s1ap_args_t;

typedef struct{
  bool     enb_name_present;
  uint32_t enb_id;
  uint8_t  enb_name[150];
  uint16_t mcc, mnc;
  uint32_t plmn;
  uint8_t  nof_supported_ta;
  uint16_t tac[MAX_TA];
  uint8_t  nof_supported_bplmns[MAX_TA];
  uint16_t bplmns[MAX_TA][MAX_BPLMN];
  LIBLTE_S1AP_PAGINGDRX_ENUM drx;
  struct   sctp_sndrcvinfo sri;
} enb_ctx_t;

typedef struct{
  uint8_t  k_asme[32]; 
  uint8_t  xres[16]; //minimum 6, maximum 16
  uint32_t dl_nas_count;
  uint32_t ul_nas_count;
  srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo;
  srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo;
  uint8_t k_nas_enc[32];
  uint8_t k_nas_int[32];
} eps_security_ctx_t;

typedef struct{
    enum erab_state state;
    uint8_t erab_id;
    srslte::gtpc_f_teid_ie enb_fteid;
    srslte::gtpc_f_teid_ie sgw_ctrl_fteid;
} erab_ctx_t;

typedef struct{
  uint64_t imsi;
  uint32_t enb_ue_s1ap_id;
  uint32_t mme_ue_s1ap_id;
  uint16_t enb_id;
  struct   sctp_sndrcvinfo enb_sri;
  eps_security_ctx_t security_ctxt;
  erab_ctx_t erabs_ctx[MAX_ERABS_PER_UE];
  LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT ue_network_cap;
  bool ms_network_cap_present;
  LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT ms_network_cap;
  bool eit;
  uint8_t procedure_transaction_id;
} ue_ctx_t;
}//namespace
#endif
