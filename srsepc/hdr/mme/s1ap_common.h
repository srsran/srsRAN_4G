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
#ifndef SRSEPC_S1AP_COMMON_H
#define SRSEPC_S1AP_COMMON_H

#include "srslte/common/security.h"
#include "srslte/asn1/gtpc_ies.h"
#include "srslte/asn1/liblte_s1ap.h"
#include "srslte/asn1/liblte_mme.h"
#include <netinet/sctp.h>

namespace srsepc{
  
static const uint8_t MAX_TA=255;  //Maximum TA supported
static const uint8_t MAX_BPLMN=6; //Maximum broadcasted PLMNs per TAC
static const uint8_t MAX_ERABS_PER_UE = 16;

// MME EMM states (3GPP 24.301 v10.0.0, section 5.1.3.4)
typedef enum {
  EMM_STATE_DEREGISTERED = 0,
  EMM_STATE_COMMON_PROCEDURE_INITIATED,
  EMM_STATE_REGISTERED,
  EMM_STATE_DEREGISTERED_INITIATED,
  EMM_STATE_N_ITEMS,
} emm_state_t;
static const char emm_state_text[EMM_STATE_N_ITEMS][100] = {"DEREGISTERED",
                                                            "COMMON PROCEDURE INITIATED",
                                                            "REGISTERED",
                                                            "DEREGISTERED INITIATED"};

// MME ECM states (3GPP 23.401 v10.0.0, section 4.6.3)
typedef enum {
  ECM_STATE_IDLE = 0,
  ECM_STATE_CONNECTED,
  ECM_STATE_N_ITEMS,
} ecm_state_t;
static const char ecm_state_text[ECM_STATE_N_ITEMS][100] = {"IDLE",
                                                            "CONNECTED"};

// MME ESM states (3GPP 23.401 v10.0.0, section 4.6.3)
typedef enum {
  ESM_BEARER_CONTEXT_INACTIVE = 0,
  ESM_BEARER_CONTEXT_ACTIVE_PENDING,
  ESM_BEARER_CONTEXT_ACTIVE,
  ESM_BEARER_CONTEXT_INACTIVE_PENDING,
  ESM_BEARER_CONTEXT_MODIFY_PENDING,
  ESM_BEARER_PROCEDURE_TRANSACTION_INACTIVE,
  ESM_BEARER_PROCEDURE_TRANSACTION_PENDING,
  ESM_STATE_N_ITEMS,
} esm_state_t;
static const char esm_state_text[ESM_STATE_N_ITEMS][100] = {"CONTEXT INACTIVE",
                                                            "CONTEXT ACTIVE PENDING",
                                                            "CONTEXT ACTIVE",
                                                            "CONTEXT_INACTIVE_PENDING",
                                                            "CONTEXT_MODIFY_PENDING",
                                                            "PROCEDURE_TRANSACTION_INACTIVE"
                                                            "PROCEDURE_TRANSACTION_PENDING"};

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
  std::string   dns_addr;
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
  uint8_t k_enb[32];
  LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT ue_network_cap;
  bool ms_network_cap_present;
  LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT ms_network_cap;
} eps_sec_ctx_t;

typedef struct{
    enum erab_state state;
    uint8_t erab_id;
    srslte::gtpc_f_teid_ie enb_fteid;
    srslte::gtpc_f_teid_ie sgw_s1u_fteid;
    srslte::gtpc_pdn_address_allocation_ie pdn_addr_alloc;
} erab_ctx_t;

typedef struct{
    uint64_t imsi;
    LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT guti;
    eps_sec_ctx_t security_ctxt;
    uint8_t procedure_transaction_id;
    emm_state_t state;
    uint32_t mme_ue_s1ap_id;
    uint8_t attach_type;
    struct in_addr ue_ip;
    srslte::gtpc_f_teid_ie sgw_ctrl_fteid;
} ue_emm_ctx_t;

typedef struct{
  uint64_t imsi;
  uint32_t enb_ue_s1ap_id;
  uint32_t mme_ue_s1ap_id;
  struct   sctp_sndrcvinfo enb_sri;
  ecm_state_t state;
  erab_ctx_t erabs_ctx[MAX_ERABS_PER_UE];
  bool eit;
} ue_ecm_ctx_t;


typedef struct{
  ue_emm_ctx_t emm_ctx;
  eps_sec_ctx_t sec_ctx;
  ue_ecm_ctx_t ecm_ctx;
  erab_ctx_t erabs_ctx[MAX_ERABS_PER_UE];
} ue_ctx_t;
}//namespace

#endif // SRSEPC_S1AP_COMMON_H
