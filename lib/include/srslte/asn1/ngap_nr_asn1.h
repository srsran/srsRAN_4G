/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

/*******************************************************************************
 *
 *                    3GPP TS ASN1 NGAP NR v15.3.0 (2019-03)
 *
 ******************************************************************************/

#ifndef SRSASN1_NGAP_NR_H
#define SRSASN1_NGAP_NR_H

#include "asn1_utils.h"
#include <cstdio>
#include <stdarg.h>

namespace asn1 {
namespace ngap_nr {

/*******************************************************************************
 *                        Functions for external logging
 ******************************************************************************/

extern srslte::log* ngap_nr_log_ptr;

void ngap_nr_log_register_handler(srslte::log* ctx);

void ngap_nr_log_print(srslte::LOG_LEVEL_ENUM log_level, const char* format, ...);

void log_invalid_access_choice_id(uint32_t val, uint32_t choice_id);

void assert_choice_type(uint32_t val, uint32_t choice_id);

void assert_choice_type(const std::string& access_type,
                        const std::string& current_type,
                        const std::string& choice_type);

const char* convert_enum_idx(const char* array[], uint32_t nof_types, uint32_t enum_val, const char* enum_type);

template <class ItemType>
ItemType convert_enum_idx(ItemType* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type)
{
  if (enum_val >= nof_types) {
    if (enum_val == nof_types) {
      ngap_nr_log_print(LOG_LEVEL_ERROR, "The enum of type %s was not initialized.\n", enum_type);
    } else {
      ngap_nr_log_print(
          LOG_LEVEL_ERROR, "The provided enum value=%d of type %s cannot be converted.\n", enum_val, enum_type);
    }
    return 0;
  }
  return array[enum_val];
}

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_NGAP_NR_ID_AMF_CFG_UPD 0
#define ASN1_NGAP_NR_ID_AMF_STATUS_IND 1
#define ASN1_NGAP_NR_ID_CELL_TRAFFIC_TRACE 2
#define ASN1_NGAP_NR_ID_DEACTIV_TRACE 3
#define ASN1_NGAP_NR_ID_DL_NAS_TRANSPORT 4
#define ASN1_NGAP_NR_ID_DL_NON_UEASSOCIATED_NRP_PA_TRANSPORT 5
#define ASN1_NGAP_NR_ID_DL_RAN_CFG_TRANSFER 6
#define ASN1_NGAP_NR_ID_DL_RAN_STATUS_TRANSFER 7
#define ASN1_NGAP_NR_ID_DL_UEASSOCIATED_NRP_PA_TRANSPORT 8
#define ASN1_NGAP_NR_ID_ERROR_IND 9
#define ASN1_NGAP_NR_ID_HO_CANCEL 10
#define ASN1_NGAP_NR_ID_HO_NOTIF 11
#define ASN1_NGAP_NR_ID_HO_PREP 12
#define ASN1_NGAP_NR_ID_HO_RES_ALLOC 13
#define ASN1_NGAP_NR_ID_INIT_CONTEXT_SETUP 14
#define ASN1_NGAP_NR_ID_INIT_UE_MSG 15
#define ASN1_NGAP_NR_ID_LOCATION_REPORT_CTRL 16
#define ASN1_NGAP_NR_ID_LOCATION_REPORT_FAIL_IND 17
#define ASN1_NGAP_NR_ID_LOCATION_REPORT 18
#define ASN1_NGAP_NR_ID_NAS_NON_DELIVERY_IND 19
#define ASN1_NGAP_NR_ID_NG_RESET 20
#define ASN1_NGAP_NR_ID_NG_SETUP 21
#define ASN1_NGAP_NR_ID_OVERLOAD_START 22
#define ASN1_NGAP_NR_ID_OVERLOAD_STOP 23
#define ASN1_NGAP_NR_ID_PAGING 24
#define ASN1_NGAP_NR_ID_PATH_SWITCH_REQUEST 25
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_MODIFY 26
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_MODIFY_IND 27
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_RELEASE 28
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_SETUP 29
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_NOTIFY 30
#define ASN1_NGAP_NR_ID_PRIVATE_MSG 31
#define ASN1_NGAP_NR_ID_PWS_CANCEL 32
#define ASN1_NGAP_NR_ID_PWS_FAIL_IND 33
#define ASN1_NGAP_NR_ID_PWS_RESTART_IND 34
#define ASN1_NGAP_NR_ID_RAN_CFG_UPD 35
#define ASN1_NGAP_NR_ID_REROUTE_NAS_REQUEST 36
#define ASN1_NGAP_NR_ID_RRC_INACTIVE_TRANSITION_REPORT 37
#define ASN1_NGAP_NR_ID_TRACE_FAIL_IND 38
#define ASN1_NGAP_NR_ID_TRACE_START 39
#define ASN1_NGAP_NR_ID_UE_CONTEXT_MOD 40
#define ASN1_NGAP_NR_ID_UE_CONTEXT_RELEASE 41
#define ASN1_NGAP_NR_ID_UE_CONTEXT_RELEASE_REQUEST 42
#define ASN1_NGAP_NR_ID_UE_RADIO_CAP_CHECK 43
#define ASN1_NGAP_NR_ID_UE_RADIO_CAP_INFO_IND 44
#define ASN1_NGAP_NR_ID_UETNLA_BINDING_RELEASE 45
#define ASN1_NGAP_NR_ID_UL_NAS_TRANSPORT 46
#define ASN1_NGAP_NR_ID_UL_NON_UEASSOCIATED_NRP_PA_TRANSPORT 47
#define ASN1_NGAP_NR_ID_UL_RAN_CFG_TRANSFER 48
#define ASN1_NGAP_NR_ID_UL_RAN_STATUS_TRANSFER 49
#define ASN1_NGAP_NR_ID_UL_UEASSOCIATED_NRP_PA_TRANSPORT 50
#define ASN1_NGAP_NR_ID_WRITE_REPLACE_WARNING 51
#define ASN1_NGAP_NR_ID_SECONDARY_RAT_DATA_USAGE_REPORT 52
#define ASN1_NGAP_NR_MAX_PRIVATE_IES 65535
#define ASN1_NGAP_NR_MAX_PROTOCOL_EXTS 65535
#define ASN1_NGAP_NR_MAX_PROTOCOL_IES 65535
#define ASN1_NGAP_NR_MAXNOOF_ALLOWED_AREAS 16
#define ASN1_NGAP_NR_MAXNOOF_ALLOWED_S_NSSAIS 8
#define ASN1_NGAP_NR_MAXNOOF_BPLMNS 12
#define ASN1_NGAP_NR_MAXNOOF_CELL_IDFOR_WARNING 65535
#define ASN1_NGAP_NR_MAXNOOF_CELLIN_AO_I 256
#define ASN1_NGAP_NR_MAXNOOF_CELLIN_EAI 65535
#define ASN1_NGAP_NR_MAXNOOF_CELLIN_TAI 65535
#define ASN1_NGAP_NR_MAXNOOF_CELLSING_NB 16384
#define ASN1_NGAP_NR_MAXNOOF_CELLSINNGE_NB 256
#define ASN1_NGAP_NR_MAXNOOF_CELLSIN_UE_HISTORY_INFO 16
#define ASN1_NGAP_NR_MAXNOOF_CELLS_UE_MOVING_TRAJECTORY 16
#define ASN1_NGAP_NR_MAXNOOF_DRBS 32
#define ASN1_NGAP_NR_MAXNOOF_EMERGENCY_AREA_ID 65535
#define ASN1_NGAP_NR_MAXNOOF_EA_IFOR_RESTART 256
#define ASN1_NGAP_NR_MAXNOOF_EPLMNS 15
#define ASN1_NGAP_NR_MAXNOOF_EPLMNS_PLUS_ONE 16
#define ASN1_NGAP_NR_MAXNOOF_E_RABS 256
#define ASN1_NGAP_NR_MAXNOOF_ERRORS 256
#define ASN1_NGAP_NR_MAXNOOF_FORB_TACS 4096
#define ASN1_NGAP_NR_MAXNOOF_MULTI_CONNECT 4
#define ASN1_NGAP_NR_MAXNOOF_MULTI_CONNECT_MINUS_ONE 3
#define ASN1_NGAP_NR_MAXNOOF_NG_CONNS_TO_RESET 65536
#define ASN1_NGAP_NR_MAXNOOF_PDU_SESSIONS 256
#define ASN1_NGAP_NR_MAXNOOF_PLMNS 12
#define ASN1_NGAP_NR_MAXNOOF_QOS_FLOWS 64
#define ASN1_NGAP_NR_MAXNOOF_RAN_NODEIN_AO_I 64
#define ASN1_NGAP_NR_MAXNOOF_RECOMMENDED_CELLS 16
#define ASN1_NGAP_NR_MAXNOOF_RECOMMENDED_RAN_NODES 16
#define ASN1_NGAP_NR_MAXNOOF_AO_I 64
#define ASN1_NGAP_NR_MAXNOOF_SERVED_GUAMIS 256
#define ASN1_NGAP_NR_MAXNOOF_SLICE_ITEMS 1024
#define ASN1_NGAP_NR_MAXNOOF_TACS 256
#define ASN1_NGAP_NR_MAXNOOF_TA_IFOR_INACTIVE 16
#define ASN1_NGAP_NR_MAXNOOF_TA_IFOR_PAGING 16
#define ASN1_NGAP_NR_MAXNOOF_TA_IFOR_RESTART 2048
#define ASN1_NGAP_NR_MAXNOOF_TA_IFOR_WARNING 65535
#define ASN1_NGAP_NR_MAXNOOF_TA_IIN_AO_I 16
#define ASN1_NGAP_NR_MAXNOOF_TIME_PERIODS 2
#define ASN1_NGAP_NR_MAXNOOF_TNLASSOCS 32
#define ASN1_NGAP_NR_MAXNOOF_XN_EXT_TLAS 2
#define ASN1_NGAP_NR_MAXNOOF_XN_GTP_TLAS 16
#define ASN1_NGAP_NR_MAXNOOF_XN_TLAS 16
#define ASN1_NGAP_NR_ID_ALLOWED_NSSAI 0
#define ASN1_NGAP_NR_ID_AMF_NAME 1
#define ASN1_NGAP_NR_ID_AMF_OVERLOAD_RESP 2
#define ASN1_NGAP_NR_ID_AMF_SET_ID 3
#define ASN1_NGAP_NR_ID_AMF_TNLASSOC_FAILED_TO_SETUP_LIST 4
#define ASN1_NGAP_NR_ID_AMF_TNLASSOC_SETUP_LIST 5
#define ASN1_NGAP_NR_ID_AMF_TNLASSOC_TO_ADD_LIST 6
#define ASN1_NGAP_NR_ID_AMF_TNLASSOC_TO_REM_LIST 7
#define ASN1_NGAP_NR_ID_AMF_TNLASSOC_TO_UPD_LIST 8
#define ASN1_NGAP_NR_ID_AMF_TRAFFIC_LOAD_REDUCTION_IND 9
#define ASN1_NGAP_NR_ID_AMF_UE_NGAP_ID 10
#define ASN1_NGAP_NR_ID_ASSIST_DATA_FOR_PAGING 11
#define ASN1_NGAP_NR_ID_BROADCAST_CANCELLED_AREA_LIST 12
#define ASN1_NGAP_NR_ID_BROADCAST_COMPLETED_AREA_LIST 13
#define ASN1_NGAP_NR_ID_CANCEL_ALL_WARNING_MSGS 14
#define ASN1_NGAP_NR_ID_CAUSE 15
#define ASN1_NGAP_NR_ID_CELL_ID_LIST_FOR_RESTART 16
#define ASN1_NGAP_NR_ID_CONCURRENT_WARNING_MSG_IND 17
#define ASN1_NGAP_NR_ID_CORE_NETWORK_ASSIST_INFO 18
#define ASN1_NGAP_NR_ID_CRIT_DIAGNOSTICS 19
#define ASN1_NGAP_NR_ID_DATA_CODING_SCHEME 20
#define ASN1_NGAP_NR_ID_DEFAULT_PAGING_DRX 21
#define ASN1_NGAP_NR_ID_DIRECT_FORWARDING_PATH_AVAILABILITY 22
#define ASN1_NGAP_NR_ID_EMERGENCY_AREA_ID_LIST_FOR_RESTART 23
#define ASN1_NGAP_NR_ID_EMERGENCY_FALLBACK_IND 24
#define ASN1_NGAP_NR_ID_EUTRA_CGI 25
#define ASN1_NGAP_NR_ID_FIVE_G_S_TMSI 26
#define ASN1_NGAP_NR_ID_GLOBAL_RAN_NODE_ID 27
#define ASN1_NGAP_NR_ID_GUAMI 28
#define ASN1_NGAP_NR_ID_HO_TYPE 29
#define ASN1_NGAP_NR_ID_IMS_VOICE_SUPPORT_IND 30
#define ASN1_NGAP_NR_ID_IDX_TO_RFSP 31
#define ASN1_NGAP_NR_ID_INFO_ON_RECOMMENDED_CELLS_AND_RAN_NODES_FOR_PAGING 32
#define ASN1_NGAP_NR_ID_LOCATION_REPORT_REQUEST_TYPE 33
#define ASN1_NGAP_NR_ID_MASKED_IMEISV 34
#define ASN1_NGAP_NR_ID_MSG_ID 35
#define ASN1_NGAP_NR_ID_MOB_RESTRICT_LIST 36
#define ASN1_NGAP_NR_ID_NASC 37
#define ASN1_NGAP_NR_ID_NAS_PDU 38
#define ASN1_NGAP_NR_ID_NAS_SECURITY_PARAMS_FROM_NGRAN 39
#define ASN1_NGAP_NR_ID_NEW_AMF_UE_NGAP_ID 40
#define ASN1_NGAP_NR_ID_NEW_SECURITY_CONTEXT_IND 41
#define ASN1_NGAP_NR_ID_NGAP_MSG 42
#define ASN1_NGAP_NR_ID_NGRAN_CGI 43
#define ASN1_NGAP_NR_ID_NGRAN_TRACE_ID 44
#define ASN1_NGAP_NR_ID_NR_CGI 45
#define ASN1_NGAP_NR_ID_NRP_PA_PDU 46
#define ASN1_NGAP_NR_ID_NOF_BROADCASTS_REQUESTED 47
#define ASN1_NGAP_NR_ID_OLD_AMF 48
#define ASN1_NGAP_NR_ID_OVERLOAD_START_NSSAI_LIST 49
#define ASN1_NGAP_NR_ID_PAGING_DRX 50
#define ASN1_NGAP_NR_ID_PAGING_ORIGIN 51
#define ASN1_NGAP_NR_ID_PAGING_PRIO 52
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_ADMITTED_LIST 53
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_FAILED_TO_MODIFY_LIST_MOD_RES 54
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_FAILED_TO_SETUP_LIST_CXT_RES 55
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_FAILED_TO_SETUP_LIST_HO_ACK 56
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_FAILED_TO_SETUP_LIST_PS_REQ 57
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_FAILED_TO_SETUP_LIST_SU_RES 58
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_HO_LIST 59
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_LIST_CXT_REL_CPL 60
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_LIST_HO_RQD 61
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_MODIFY_LIST_MOD_CFM 62
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_MODIFY_LIST_MOD_IND 63
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_MODIFY_LIST_MOD_REQ 64
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_MODIFY_LIST_MOD_RES 65
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_NOTIFY_LIST 66
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_RELEASED_LIST_NOT 67
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_RELEASED_LIST_PS_ACK 68
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_RELEASED_LIST_PS_FAIL 69
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_RELEASED_LIST_REL_RES 70
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_SETUP_LIST_CXT_REQ 71
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_SETUP_LIST_CXT_RES 72
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_SETUP_LIST_HO_REQ 73
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_SETUP_LIST_SU_REQ 74
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_SETUP_LIST_SU_RES 75
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_TO_BE_SWITCHED_DL_LIST 76
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_SWITCHED_LIST 77
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_TO_RELEASE_LIST_HO_CMD 78
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_TO_RELEASE_LIST_REL_CMD 79
#define ASN1_NGAP_NR_ID_PLMN_SUPPORT_LIST 80
#define ASN1_NGAP_NR_ID_PWS_FAILED_CELL_ID_LIST 81
#define ASN1_NGAP_NR_ID_RAN_NODE_NAME 82
#define ASN1_NGAP_NR_ID_RAN_PAGING_PRIO 83
#define ASN1_NGAP_NR_ID_RAN_STATUS_TRANSFER_TRANSPARENT_CONTAINER 84
#define ASN1_NGAP_NR_ID_RAN_UE_NGAP_ID 85
#define ASN1_NGAP_NR_ID_RELATIVE_AMF_CAPACITY 86
#define ASN1_NGAP_NR_ID_REPEAT_PERIOD 87
#define ASN1_NGAP_NR_ID_RESET_TYPE 88
#define ASN1_NGAP_NR_ID_ROUTING_ID 89
#define ASN1_NGAP_NR_ID_RRCESTABLISHMENT_CAUSE 90
#define ASN1_NGAP_NR_ID_RRC_INACTIVE_TRANSITION_REPORT_REQUEST 91
#define ASN1_NGAP_NR_ID_RRC_STATE 92
#define ASN1_NGAP_NR_ID_SECURITY_CONTEXT 93
#define ASN1_NGAP_NR_ID_SECURITY_KEY 94
#define ASN1_NGAP_NR_ID_SERIAL_NUM 95
#define ASN1_NGAP_NR_ID_SERVED_GUAMI_LIST 96
#define ASN1_NGAP_NR_ID_SLICE_SUPPORT_LIST 97
#define ASN1_NGAP_NR_ID_SON_CFG_TRANSFER_DL 98
#define ASN1_NGAP_NR_ID_SON_CFG_TRANSFER_UL 99
#define ASN1_NGAP_NR_ID_SOURCE_AMF_UE_NGAP_ID 100
#define ASN1_NGAP_NR_ID_SOURCE_TO_TARGET_TRANSPARENT_CONTAINER 101
#define ASN1_NGAP_NR_ID_SUPPORTED_TA_LIST 102
#define ASN1_NGAP_NR_ID_TAI_LIST_FOR_PAGING 103
#define ASN1_NGAP_NR_ID_TAI_LIST_FOR_RESTART 104
#define ASN1_NGAP_NR_ID_TARGET_ID 105
#define ASN1_NGAP_NR_ID_TARGET_TO_SOURCE_TRANSPARENT_CONTAINER 106
#define ASN1_NGAP_NR_ID_TIME_TO_WAIT 107
#define ASN1_NGAP_NR_ID_TRACE_ACTIVATION 108
#define ASN1_NGAP_NR_ID_TRACE_COLLECTION_ENTITY_IP_ADDRESS 109
#define ASN1_NGAP_NR_ID_UE_AGGREGATE_MAXIMUM_BIT_RATE 110
#define ASN1_NGAP_NR_ID_UE_ASSOCIATED_LC_NG_CONN_LIST 111
#define ASN1_NGAP_NR_ID_UE_CONTEXT_REQUEST 112
#define ASN1_NGAP_NR_ID_UE_NGAP_IDS 114
#define ASN1_NGAP_NR_ID_UE_PAGING_ID 115
#define ASN1_NGAP_NR_ID_UE_PRESENCE_IN_AREA_OF_INTEREST_LIST 116
#define ASN1_NGAP_NR_ID_UE_RADIO_CAP 117
#define ASN1_NGAP_NR_ID_UE_RADIO_CAP_FOR_PAGING 118
#define ASN1_NGAP_NR_ID_UE_SECURITY_CAP 119
#define ASN1_NGAP_NR_ID_UNAVAILABLE_GUAMI_LIST 120
#define ASN1_NGAP_NR_ID_USER_LOCATION_INFO 121
#define ASN1_NGAP_NR_ID_WARNING_AREA_LIST 122
#define ASN1_NGAP_NR_ID_WARNING_MSG_CONTENTS 123
#define ASN1_NGAP_NR_ID_WARNING_SECURITY_INFO 124
#define ASN1_NGAP_NR_ID_WARNING_TYPE 125
#define ASN1_NGAP_NR_ID_ADD_UL_NGU_UP_TNL_INFO 126
#define ASN1_NGAP_NR_ID_DATA_FORWARDING_NOT_POSSIBLE 127
#define ASN1_NGAP_NR_ID_DL_NGU_UP_TNL_INFO 128
#define ASN1_NGAP_NR_ID_NETWORK_INSTANCE 129
#define ASN1_NGAP_NR_ID_PDU_SESSION_AGGREGATE_MAXIMUM_BIT_RATE 130
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_FAILED_TO_MODIFY_LIST_MOD_CFM 131
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_FAILED_TO_SETUP_LIST_CXT_FAIL 132
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_LIST_CXT_REL_REQ 133
#define ASN1_NGAP_NR_ID_PDU_SESSION_TYPE 134
#define ASN1_NGAP_NR_ID_QOS_FLOW_ADD_OR_MODIFY_REQUEST_LIST 135
#define ASN1_NGAP_NR_ID_QOS_FLOW_SETUP_REQUEST_LIST 136
#define ASN1_NGAP_NR_ID_QOS_FLOW_TO_RELEASE_LIST 137
#define ASN1_NGAP_NR_ID_SECURITY_IND 138
#define ASN1_NGAP_NR_ID_UL_NGU_UP_TNL_INFO 139
#define ASN1_NGAP_NR_ID_UL_NGU_UP_TNL_MODIFY_LIST 140
#define ASN1_NGAP_NR_ID_WARNING_AREA_COORDINATES 141
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_SECONDARY_RATUSAGE_LIST 142
#define ASN1_NGAP_NR_ID_HO_FLAG 143
#define ASN1_NGAP_NR_ID_SECONDARY_RATUSAGE_INFO 144
#define ASN1_NGAP_NR_ID_PDU_SESSION_RES_RELEASE_RESP_TRANSFER 145
#define ASN1_NGAP_NR_ID_REDIRECTION_VOICE_FALLBACK 146
#define ASN1_NGAP_NR_ID_UE_RETENTION_INFO 147
#define ASN1_NGAP_NR_ID_S_NSSAI 148
#define ASN1_NGAP_NR_ID_PS_CELL_INFO 149
#define ASN1_NGAP_NR_ID_LAST_EUTRAN_PLMN_ID 150
#define ASN1_NGAP_NR_ID_MAXIMUM_INTEGRITY_PROTECTED_DATA_RATE_DL 151
#define ASN1_NGAP_NR_ID_ADD_DL_FORWARDING_UPTNL_INFO 152
#define ASN1_NGAP_NR_ID_ADD_DLUPTNL_INFO_FOR_HO_LIST 153
#define ASN1_NGAP_NR_ID_ADD_NGU_UP_TNL_INFO 154
#define ASN1_NGAP_NR_ID_ADD_DL_QOS_FLOW_PER_TNL_INFO 155
#define ASN1_NGAP_NR_ID_SECURITY_RESULT 156
#define ASN1_NGAP_NR_ID_ENDC_SON_CFG_TRANSFER_DL 157
#define ASN1_NGAP_NR_ID_ENDC_SON_CFG_TRANSFER_UL 158

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// Criticality ::= ENUMERATED
struct crit_opts {
  enum options { reject, ignore, notify, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<crit_opts> crit_e;

// Presence ::= ENUMERATED
struct presence_opts {
  enum options { optional, conditional, mandatory, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<presence_opts> presence_e;

// ProtocolIE-Field{NGAP-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE{{NGAP-PROTOCOL-IES}}
template <class ies_set_param>
struct protocol_ie_field_s {
  uint32_t                        id = 0;
  crit_e                          crit;
  typename ies_set_param::value_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint32_t& id_);
};

struct ngap_protocol_ies_empty_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};
// CPTransportLayerInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o cp_transport_layer_info_ext_ies_o;

// ProtocolExtensionField{NGAP-PROTOCOL-EXTENSION : ExtensionSetParam} ::= SEQUENCE{{NGAP-PROTOCOL-EXTENSION}}
template <class ext_set_param>
struct protocol_ext_field_s {
  uint32_t                      id = 0;
  crit_e                        crit;
  typename ext_set_param::ext_c ext_value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint32_t& id_);
};

// ProtocolIE-SingleContainer ::= ProtocolIE-Field
template <class ies_set_param>
using protocol_ie_single_container_s = protocol_ie_field_s<ies_set_param>;

struct ngap_protocol_ext_empty_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};
// AMF-TNLAssociationSetupItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o amf_tnlassoc_setup_item_ext_ies_o;

// CPTransportLayerInformation ::= CHOICE
struct cp_transport_layer_info_c {
  struct types_opts {
    enum options { endpoint_ip_address, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cp_transport_layer_info_c() = default;
  cp_transport_layer_info_c(const cp_transport_layer_info_c& other);
  cp_transport_layer_info_c& operator=(const cp_transport_layer_info_c& other);
  ~cp_transport_layer_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bounded_bitstring<1, 160, true, true>& endpoint_ip_address()
  {
    assert_choice_type("endpointIPAddress", type_.to_string(), "CPTransportLayerInformation");
    return c.get<bounded_bitstring<1, 160, true, true> >();
  }
  protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "CPTransportLayerInformation");
    return c.get<protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o> >();
  }
  const bounded_bitstring<1, 160, true, true>& endpoint_ip_address() const
  {
    assert_choice_type("endpointIPAddress", type_.to_string(), "CPTransportLayerInformation");
    return c.get<bounded_bitstring<1, 160, true, true> >();
  }
  const protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "CPTransportLayerInformation");
    return c.get<protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o> >();
  }
  bounded_bitstring<1, 160, true, true>& set_endpoint_ip_address()
  {
    set(types::endpoint_ip_address);
    return c.get<bounded_bitstring<1, 160, true, true> >();
  }
  protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o> >();
  }

private:
  types type_;
  choice_buffer_t<bounded_bitstring<1, 160, true, true>,
                  protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o> >
      c;

  void destroy_();
};

// ProtocolExtensionContainer{NGAP-PROTOCOL-EXTENSION : ExtensionSetParam} ::= SEQUENCE (SIZE (1..65535)) OF
// ProtocolExtensionField
template <class ext_set_param>
using protocol_ext_container_l = dyn_array<protocol_ext_field_s<ext_set_param> >;

template <class extT_>
struct protocol_ext_container_item_s {
  uint32_t id = 0;
  crit_e   crit;
  extT_    ext;

  // sequence methods
  protocol_ext_container_item_s(uint32_t id_, crit_e crit_);
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct protocol_ext_container_empty_l {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};
// AMF-TNLAssociationSetupItem ::= SEQUENCE
struct amf_tnlassoc_setup_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  cp_transport_layer_info_c amf_tnlassoc_address;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMF-TNLAssociationSetupList ::= SEQUENCE (SIZE (1..32)) OF AMF-TNLAssociationSetupItem
using amf_tnlassoc_setup_list_l = dyn_array<amf_tnlassoc_setup_item_s>;

// AMF-TNLAssociationToAddItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o amf_tnlassoc_to_add_item_ext_ies_o;

// TNLAssociationUsage ::= ENUMERATED
struct tnlassoc_usage_opts {
  enum options { ue, non_ue, both, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<tnlassoc_usage_opts, true> tnlassoc_usage_e;

// AMF-TNLAssociationToAddItem ::= SEQUENCE
struct amf_tnlassoc_to_add_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext                    = false;
  bool                      tnlassoc_usage_present = false;
  bool                      ie_exts_present        = false;
  cp_transport_layer_info_c amf_tnlassoc_address;
  tnlassoc_usage_e          tnlassoc_usage;
  uint16_t                  tnl_address_weight_factor = 0;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMF-TNLAssociationToAddList ::= SEQUENCE (SIZE (1..32)) OF AMF-TNLAssociationToAddItem
using amf_tnlassoc_to_add_list_l = dyn_array<amf_tnlassoc_to_add_item_s>;

// AMF-TNLAssociationToRemoveItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o amf_tnlassoc_to_rem_item_ext_ies_o;

// AMF-TNLAssociationToRemoveItem ::= SEQUENCE
struct amf_tnlassoc_to_rem_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  cp_transport_layer_info_c amf_tnlassoc_address;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMF-TNLAssociationToRemoveList ::= SEQUENCE (SIZE (1..32)) OF AMF-TNLAssociationToRemoveItem
using amf_tnlassoc_to_rem_list_l = dyn_array<amf_tnlassoc_to_rem_item_s>;

// AMF-TNLAssociationToUpdateItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o amf_tnlassoc_to_upd_item_ext_ies_o;

// AMF-TNLAssociationToUpdateItem ::= SEQUENCE
struct amf_tnlassoc_to_upd_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext                               = false;
  bool                      tnlassoc_usage_present            = false;
  bool                      tnl_address_weight_factor_present = false;
  bool                      ie_exts_present                   = false;
  cp_transport_layer_info_c amf_tnlassoc_address;
  tnlassoc_usage_e          tnlassoc_usage;
  uint16_t                  tnl_address_weight_factor = 0;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMF-TNLAssociationToUpdateList ::= SEQUENCE (SIZE (1..32)) OF AMF-TNLAssociationToUpdateItem
using amf_tnlassoc_to_upd_list_l = dyn_array<amf_tnlassoc_to_upd_item_s>;

// S-NSSAI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o s_nssai_ext_ies_o;

// S-NSSAI ::= SEQUENCE
struct s_nssai_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext             = false;
  bool                     sd_present      = false;
  bool                     ie_exts_present = false;
  fixed_octstring<1, true> sst;
  fixed_octstring<3, true> sd;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SliceSupportItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o slice_support_item_ext_ies_o;

// GUAMI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o guami_ext_ies_o;

// SliceSupportItem ::= SEQUENCE
struct slice_support_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  s_nssai_s  s_nssai;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GUAMI ::= SEQUENCE
struct guami_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<8, false, true>  amf_region_id;
  fixed_bitstring<10, false, true> amf_set_id;
  fixed_bitstring<6, false, true>  amf_pointer;
  ie_exts_l_                       ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMNSupportItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o plmn_support_item_ext_ies_o;

// ServedGUAMIItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o served_guami_item_ext_ies_o;

// SliceSupportList ::= SEQUENCE (SIZE (1..1024)) OF SliceSupportItem
using slice_support_list_l = dyn_array<slice_support_item_s>;

// PLMNSupportItem ::= SEQUENCE
struct plmn_support_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plmn_id;
  slice_support_list_l     slice_support_list;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServedGUAMIItem ::= SEQUENCE
struct served_guami_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                 ext                     = false;
  bool                                 backup_amf_name_present = false;
  bool                                 ie_exts_present         = false;
  guami_s                              guami;
  printable_string<1, 150, true, true> backup_amf_name;
  ie_exts_l_                           ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMNSupportList ::= SEQUENCE (SIZE (1..12)) OF PLMNSupportItem
using plmn_support_list_l = dyn_array<plmn_support_item_s>;

// ServedGUAMIList ::= SEQUENCE (SIZE (1..256)) OF ServedGUAMIItem
using served_guami_list_l = dyn_array<served_guami_item_s>;

// AMFConfigurationUpdateIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct amf_cfg_upd_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_name,
        served_guami_list,
        relative_amf_capacity,
        plmn_support_list,
        amf_tnlassoc_to_add_list,
        amf_tnlassoc_to_rem_list,
        amf_tnlassoc_to_upd_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    printable_string<1, 150, true, true>& amf_name()
    {
      assert_choice_type("PrintableString", type_.to_string(), "Value");
      return c.get<printable_string<1, 150, true, true> >();
    }
    served_guami_list_l& served_guami_list()
    {
      assert_choice_type("ServedGUAMIList", type_.to_string(), "Value");
      return c.get<served_guami_list_l>();
    }
    uint16_t& relative_amf_capacity()
    {
      assert_choice_type("INTEGER (0..255)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    plmn_support_list_l& plmn_support_list()
    {
      assert_choice_type("PLMNSupportList", type_.to_string(), "Value");
      return c.get<plmn_support_list_l>();
    }
    amf_tnlassoc_to_add_list_l& amf_tnlassoc_to_add_list()
    {
      assert_choice_type("AMF-TNLAssociationToAddList", type_.to_string(), "Value");
      return c.get<amf_tnlassoc_to_add_list_l>();
    }
    amf_tnlassoc_to_rem_list_l& amf_tnlassoc_to_rem_list()
    {
      assert_choice_type("AMF-TNLAssociationToRemoveList", type_.to_string(), "Value");
      return c.get<amf_tnlassoc_to_rem_list_l>();
    }
    amf_tnlassoc_to_upd_list_l& amf_tnlassoc_to_upd_list()
    {
      assert_choice_type("AMF-TNLAssociationToUpdateList", type_.to_string(), "Value");
      return c.get<amf_tnlassoc_to_upd_list_l>();
    }
    const printable_string<1, 150, true, true>& amf_name() const
    {
      assert_choice_type("PrintableString", type_.to_string(), "Value");
      return c.get<printable_string<1, 150, true, true> >();
    }
    const served_guami_list_l& served_guami_list() const
    {
      assert_choice_type("ServedGUAMIList", type_.to_string(), "Value");
      return c.get<served_guami_list_l>();
    }
    const uint16_t& relative_amf_capacity() const
    {
      assert_choice_type("INTEGER (0..255)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    const plmn_support_list_l& plmn_support_list() const
    {
      assert_choice_type("PLMNSupportList", type_.to_string(), "Value");
      return c.get<plmn_support_list_l>();
    }
    const amf_tnlassoc_to_add_list_l& amf_tnlassoc_to_add_list() const
    {
      assert_choice_type("AMF-TNLAssociationToAddList", type_.to_string(), "Value");
      return c.get<amf_tnlassoc_to_add_list_l>();
    }
    const amf_tnlassoc_to_rem_list_l& amf_tnlassoc_to_rem_list() const
    {
      assert_choice_type("AMF-TNLAssociationToRemoveList", type_.to_string(), "Value");
      return c.get<amf_tnlassoc_to_rem_list_l>();
    }
    const amf_tnlassoc_to_upd_list_l& amf_tnlassoc_to_upd_list() const
    {
      assert_choice_type("AMF-TNLAssociationToUpdateList", type_.to_string(), "Value");
      return c.get<amf_tnlassoc_to_upd_list_l>();
    }
    printable_string<1, 150, true, true>& set_amf_name()
    {
      set(types::amf_name);
      return c.get<printable_string<1, 150, true, true> >();
    }
    served_guami_list_l& set_served_guami_list()
    {
      set(types::served_guami_list);
      return c.get<served_guami_list_l>();
    }
    uint16_t& set_relative_amf_capacity()
    {
      set(types::relative_amf_capacity);
      return c.get<uint16_t>();
    }
    plmn_support_list_l& set_plmn_support_list()
    {
      set(types::plmn_support_list);
      return c.get<plmn_support_list_l>();
    }
    amf_tnlassoc_to_add_list_l& set_amf_tnlassoc_to_add_list()
    {
      set(types::amf_tnlassoc_to_add_list);
      return c.get<amf_tnlassoc_to_add_list_l>();
    }
    amf_tnlassoc_to_rem_list_l& set_amf_tnlassoc_to_rem_list()
    {
      set(types::amf_tnlassoc_to_rem_list);
      return c.get<amf_tnlassoc_to_rem_list_l>();
    }
    amf_tnlassoc_to_upd_list_l& set_amf_tnlassoc_to_upd_list()
    {
      set(types::amf_tnlassoc_to_upd_list);
      return c.get<amf_tnlassoc_to_upd_list_l>();
    }

  private:
    types type_;
    choice_buffer_t<amf_tnlassoc_to_add_list_l,
                    amf_tnlassoc_to_rem_list_l,
                    amf_tnlassoc_to_upd_list_l,
                    plmn_support_list_l,
                    printable_string<1, 150, true, true>,
                    served_guami_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ProtocolIE-Container{NGAP-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE (SIZE (0..65535)) OF ProtocolIE-Field
template <class ies_set_param>
using protocol_ie_container_l = dyn_array<protocol_ie_field_s<ies_set_param> >;

template <class valueT_>
struct protocol_ie_container_item_s {
  uint32_t id = 0;
  crit_e   crit;
  valueT_  value;

  // sequence methods
  protocol_ie_container_item_s(uint32_t id_, crit_e crit_);
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMFConfigurationUpdate ::= SEQUENCE
struct amf_cfg_upd_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                             amf_name_present                 = false;
    bool                                                             served_guami_list_present        = false;
    bool                                                             relative_amf_capacity_present    = false;
    bool                                                             plmn_support_list_present        = false;
    bool                                                             amf_tnlassoc_to_add_list_present = false;
    bool                                                             amf_tnlassoc_to_rem_list_present = false;
    bool                                                             amf_tnlassoc_to_upd_list_present = false;
    ie_field_s<printable_string<1, 150, true, true> >                amf_name;
    ie_field_s<dyn_seq_of<served_guami_item_s, 1, 256, true> >       served_guami_list;
    ie_field_s<integer<uint16_t, 0, 255, false, true> >              relative_amf_capacity;
    ie_field_s<dyn_seq_of<plmn_support_item_s, 1, 12, true> >        plmn_support_list;
    ie_field_s<dyn_seq_of<amf_tnlassoc_to_add_item_s, 1, 32, true> > amf_tnlassoc_to_add_list;
    ie_field_s<dyn_seq_of<amf_tnlassoc_to_rem_item_s, 1, 32, true> > amf_tnlassoc_to_rem_list;
    ie_field_s<dyn_seq_of<amf_tnlassoc_to_upd_item_s, 1, 32, true> > amf_tnlassoc_to_upd_list;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cause-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o cause_ext_ies_o;

// CauseMisc ::= ENUMERATED
struct cause_misc_opts {
  enum options {
    ctrl_processing_overload,
    not_enough_user_plane_processing_res,
    hardware_fail,
    om_intervention,
    unknown_plmn,
    unspecified,
    // ...
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<cause_misc_opts, true> cause_misc_e;

// CauseNas ::= ENUMERATED
struct cause_nas_opts {
  enum options { normal_release, authentication_fail, deregister, unspecified, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cause_nas_opts, true> cause_nas_e;

// CauseProtocol ::= ENUMERATED
struct cause_protocol_opts {
  enum options {
    transfer_syntax_error,
    abstract_syntax_error_reject,
    abstract_syntax_error_ignore_and_notify,
    msg_not_compatible_with_receiver_state,
    semantic_error,
    abstract_syntax_error_falsely_constructed_msg,
    unspecified,
    // ...
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<cause_protocol_opts, true> cause_protocol_e;

// CauseRadioNetwork ::= ENUMERATED
struct cause_radio_network_opts {
  enum options {
    unspecified,
    txnrelocoverall_expiry,
    successful_ho,
    release_due_to_ngran_generated_reason,
    release_due_to_minus5gc_generated_reason,
    ho_cancelled,
    partial_ho,
    ho_fail_in_target_minus5_gc_ngran_node_or_target_sys,
    ho_target_not_allowed,
    tngrelocoverall_expiry,
    tngrelocprep_expiry,
    cell_not_available,
    unknown_target_id,
    no_radio_res_available_in_target_cell,
    unknown_local_ue_ngap_id,
    inconsistent_remote_ue_ngap_id,
    ho_desirable_for_radio_reason,
    time_crit_ho,
    res_optim_ho,
    reduce_load_in_serving_cell,
    user_inactivity,
    radio_conn_with_ue_lost,
    radio_res_not_available,
    invalid_qos_combination,
    fail_in_radio_interface_proc,
    interaction_with_other_proc,
    unknown_pdu_session_id,
    unkown_qos_flow_id,
    multiple_pdu_session_id_instances,
    multiple_qos_flow_id_instances,
    encryption_and_or_integrity_protection_algorithms_not_supported,
    ng_intra_sys_ho_triggered,
    ng_inter_sys_ho_triggered,
    xn_ho_triggered,
    not_supported_minus5_qi_value,
    ue_context_transfer,
    ims_voice_eps_fallback_or_rat_fallback_triggered,
    up_integrity_protection_not_possible,
    up_confidentiality_protection_not_possible,
    slice_not_supported,
    ue_in_rrc_inactive_state_not_reachable,
    redirection,
    res_not_available_for_the_slice,
    ue_max_integrity_protected_data_rate_reason,
    release_due_to_cn_detected_mob,
    // ...
    n26_interface_not_available,
    release_due_to_pre_emption,
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<cause_radio_network_opts, true, 2> cause_radio_network_e;

// CauseTransport ::= ENUMERATED
struct cause_transport_opts {
  enum options { transport_res_unavailable, unspecified, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cause_transport_opts, true> cause_transport_e;

// CriticalityDiagnostics-IE-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o crit_diagnostics_ie_item_ext_ies_o;

// TypeOfError ::= ENUMERATED
struct type_of_error_opts {
  enum options { not_understood, missing, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<type_of_error_opts, true> type_of_error_e;

// Cause ::= CHOICE
struct cause_c {
  struct types_opts {
    enum options { radio_network, transport, nas, protocol, misc, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cause_c() = default;
  cause_c(const cause_c& other);
  cause_c& operator=(const cause_c& other);
  ~cause_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cause_radio_network_e& radio_network()
  {
    assert_choice_type("radioNetwork", type_.to_string(), "Cause");
    return c.get<cause_radio_network_e>();
  }
  cause_transport_e& transport()
  {
    assert_choice_type("transport", type_.to_string(), "Cause");
    return c.get<cause_transport_e>();
  }
  cause_nas_e& nas()
  {
    assert_choice_type("nas", type_.to_string(), "Cause");
    return c.get<cause_nas_e>();
  }
  cause_protocol_e& protocol()
  {
    assert_choice_type("protocol", type_.to_string(), "Cause");
    return c.get<cause_protocol_e>();
  }
  cause_misc_e& misc()
  {
    assert_choice_type("misc", type_.to_string(), "Cause");
    return c.get<cause_misc_e>();
  }
  protocol_ie_single_container_s<cause_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "Cause");
    return c.get<protocol_ie_single_container_s<cause_ext_ies_o> >();
  }
  const cause_radio_network_e& radio_network() const
  {
    assert_choice_type("radioNetwork", type_.to_string(), "Cause");
    return c.get<cause_radio_network_e>();
  }
  const cause_transport_e& transport() const
  {
    assert_choice_type("transport", type_.to_string(), "Cause");
    return c.get<cause_transport_e>();
  }
  const cause_nas_e& nas() const
  {
    assert_choice_type("nas", type_.to_string(), "Cause");
    return c.get<cause_nas_e>();
  }
  const cause_protocol_e& protocol() const
  {
    assert_choice_type("protocol", type_.to_string(), "Cause");
    return c.get<cause_protocol_e>();
  }
  const cause_misc_e& misc() const
  {
    assert_choice_type("misc", type_.to_string(), "Cause");
    return c.get<cause_misc_e>();
  }
  const protocol_ie_single_container_s<cause_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "Cause");
    return c.get<protocol_ie_single_container_s<cause_ext_ies_o> >();
  }
  cause_radio_network_e& set_radio_network()
  {
    set(types::radio_network);
    return c.get<cause_radio_network_e>();
  }
  cause_transport_e& set_transport()
  {
    set(types::transport);
    return c.get<cause_transport_e>();
  }
  cause_nas_e& set_nas()
  {
    set(types::nas);
    return c.get<cause_nas_e>();
  }
  cause_protocol_e& set_protocol()
  {
    set(types::protocol);
    return c.get<cause_protocol_e>();
  }
  cause_misc_e& set_misc()
  {
    set(types::misc);
    return c.get<cause_misc_e>();
  }
  protocol_ie_single_container_s<cause_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<cause_ext_ies_o> >();
  }

private:
  types                                                             type_;
  choice_buffer_t<protocol_ie_single_container_s<cause_ext_ies_o> > c;

  void destroy_();
};

// CriticalityDiagnostics-IE-Item ::= SEQUENCE
struct crit_diagnostics_ie_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool            ext             = false;
  bool            ie_exts_present = false;
  crit_e          iecrit;
  uint32_t        ie_id = 0;
  type_of_error_e type_of_error;
  ie_exts_l_      ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TNLAssociationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o tnlassoc_item_ext_ies_o;

// CriticalityDiagnostics-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o crit_diagnostics_ext_ies_o;

// CriticalityDiagnostics-IE-List ::= SEQUENCE (SIZE (1..256)) OF CriticalityDiagnostics-IE-Item
using crit_diagnostics_ie_list_l = dyn_array<crit_diagnostics_ie_item_s>;

// TNLAssociationItem ::= SEQUENCE
struct tnlassoc_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  cp_transport_layer_info_c tnlassoc_address;
  cause_c                   cause;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TriggeringMessage ::= ENUMERATED
struct trigger_msg_opts {
  enum options { init_msg, successful_outcome, unsuccessfull_outcome, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<trigger_msg_opts> trigger_msg_e;

// CriticalityDiagnostics ::= SEQUENCE
struct crit_diagnostics_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                       ext                          = false;
  bool                       proc_code_present            = false;
  bool                       trigger_msg_present          = false;
  bool                       proc_crit_present            = false;
  bool                       ies_crit_diagnostics_present = false;
  bool                       ie_exts_present              = false;
  uint16_t                   proc_code                    = 0;
  trigger_msg_e              trigger_msg;
  crit_e                     proc_crit;
  crit_diagnostics_ie_list_l ies_crit_diagnostics;
  ie_exts_l_                 ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TNLAssociationList ::= SEQUENCE (SIZE (1..32)) OF TNLAssociationItem
using tnlassoc_list_l = dyn_array<tnlassoc_item_s>;

// AMFConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct amf_cfg_upd_ack_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_tnlassoc_setup_list, amf_tnlassoc_failed_to_setup_list, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    amf_tnlassoc_setup_list_l& amf_tnlassoc_setup_list()
    {
      assert_choice_type("AMF-TNLAssociationSetupList", type_.to_string(), "Value");
      return c.get<amf_tnlassoc_setup_list_l>();
    }
    tnlassoc_list_l& amf_tnlassoc_failed_to_setup_list()
    {
      assert_choice_type("TNLAssociationList", type_.to_string(), "Value");
      return c.get<tnlassoc_list_l>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const amf_tnlassoc_setup_list_l& amf_tnlassoc_setup_list() const
    {
      assert_choice_type("AMF-TNLAssociationSetupList", type_.to_string(), "Value");
      return c.get<amf_tnlassoc_setup_list_l>();
    }
    const tnlassoc_list_l& amf_tnlassoc_failed_to_setup_list() const
    {
      assert_choice_type("TNLAssociationList", type_.to_string(), "Value");
      return c.get<tnlassoc_list_l>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    amf_tnlassoc_setup_list_l& set_amf_tnlassoc_setup_list()
    {
      set(types::amf_tnlassoc_setup_list);
      return c.get<amf_tnlassoc_setup_list_l>();
    }
    tnlassoc_list_l& set_amf_tnlassoc_failed_to_setup_list()
    {
      set(types::amf_tnlassoc_failed_to_setup_list);
      return c.get<tnlassoc_list_l>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                                                           type_;
    choice_buffer_t<amf_tnlassoc_setup_list_l, crit_diagnostics_s, tnlassoc_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// AMFConfigurationUpdateAcknowledge ::= SEQUENCE
struct amf_cfg_upd_ack_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                            amf_tnlassoc_setup_list_present           = false;
    bool                                                            amf_tnlassoc_failed_to_setup_list_present = false;
    bool                                                            crit_diagnostics_present                  = false;
    ie_field_s<dyn_seq_of<amf_tnlassoc_setup_item_s, 1, 32, true> > amf_tnlassoc_setup_list;
    ie_field_s<dyn_seq_of<tnlassoc_item_s, 1, 32, true> >           amf_tnlassoc_failed_to_setup_list;
    ie_field_s<crit_diagnostics_s>                                  crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TimeToWait ::= ENUMERATED
struct time_to_wait_opts {
  enum options { v1s, v2s, v5s, v10s, v20s, v60s, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<time_to_wait_opts, true> time_to_wait_e;

// AMFConfigurationUpdateFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct amf_cfg_upd_fail_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, time_to_wait, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    time_to_wait_e& time_to_wait()
    {
      assert_choice_type("TimeToWait", type_.to_string(), "Value");
      return c.get<time_to_wait_e>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const time_to_wait_e& time_to_wait() const
    {
      assert_choice_type("TimeToWait", type_.to_string(), "Value");
      return c.get<time_to_wait_e>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }
    time_to_wait_e& set_time_to_wait()
    {
      set(types::time_to_wait);
      return c.get<time_to_wait_e>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// AMFConfigurationUpdateFailure ::= SEQUENCE
struct amf_cfg_upd_fail_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                           time_to_wait_present     = false;
    bool                           crit_diagnostics_present = false;
    ie_field_s<cause_c>            cause;
    ie_field_s<time_to_wait_e>     time_to_wait;
    ie_field_s<crit_diagnostics_s> crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o gnb_id_ext_ies_o;

// N3IWF-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o n3_iwf_id_ext_ies_o;

// NgENB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o ng_enb_id_ext_ies_o;

// GNB-ID ::= CHOICE
struct gnb_id_c {
  struct types_opts {
    enum options { gnb_id, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  gnb_id_c() = default;
  gnb_id_c(const gnb_id_c& other);
  gnb_id_c& operator=(const gnb_id_c& other);
  ~gnb_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bounded_bitstring<22, 32, false, true>& gnb_id()
  {
    assert_choice_type("gNB-ID", type_.to_string(), "GNB-ID");
    return c.get<bounded_bitstring<22, 32, false, true> >();
  }
  protocol_ie_single_container_s<gnb_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "GNB-ID");
    return c.get<protocol_ie_single_container_s<gnb_id_ext_ies_o> >();
  }
  const bounded_bitstring<22, 32, false, true>& gnb_id() const
  {
    assert_choice_type("gNB-ID", type_.to_string(), "GNB-ID");
    return c.get<bounded_bitstring<22, 32, false, true> >();
  }
  const protocol_ie_single_container_s<gnb_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "GNB-ID");
    return c.get<protocol_ie_single_container_s<gnb_id_ext_ies_o> >();
  }
  bounded_bitstring<22, 32, false, true>& set_gnb_id()
  {
    set(types::gnb_id);
    return c.get<bounded_bitstring<22, 32, false, true> >();
  }
  protocol_ie_single_container_s<gnb_id_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<gnb_id_ext_ies_o> >();
  }

private:
  types                                                                                                      type_;
  choice_buffer_t<bounded_bitstring<22, 32, false, true>, protocol_ie_single_container_s<gnb_id_ext_ies_o> > c;

  void destroy_();
};

// GlobalGNB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o global_gnb_id_ext_ies_o;

// GlobalN3IWF-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o global_n3_iwf_id_ext_ies_o;

// GlobalNgENB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o global_ng_enb_id_ext_ies_o;

// N3IWF-ID ::= CHOICE
struct n3_iwf_id_c {
  struct types_opts {
    enum options { n3_iwf_id, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  n3_iwf_id_c() = default;
  n3_iwf_id_c(const n3_iwf_id_c& other);
  n3_iwf_id_c& operator=(const n3_iwf_id_c& other);
  ~n3_iwf_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<16, false, true>& n3_iwf_id()
  {
    assert_choice_type("n3IWF-ID", type_.to_string(), "N3IWF-ID");
    return c.get<fixed_bitstring<16, false, true> >();
  }
  protocol_ie_single_container_s<n3_iwf_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "N3IWF-ID");
    return c.get<protocol_ie_single_container_s<n3_iwf_id_ext_ies_o> >();
  }
  const fixed_bitstring<16, false, true>& n3_iwf_id() const
  {
    assert_choice_type("n3IWF-ID", type_.to_string(), "N3IWF-ID");
    return c.get<fixed_bitstring<16, false, true> >();
  }
  const protocol_ie_single_container_s<n3_iwf_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "N3IWF-ID");
    return c.get<protocol_ie_single_container_s<n3_iwf_id_ext_ies_o> >();
  }
  fixed_bitstring<16, false, true>& set_n3_iwf_id()
  {
    set(types::n3_iwf_id);
    return c.get<fixed_bitstring<16, false, true> >();
  }
  protocol_ie_single_container_s<n3_iwf_id_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<n3_iwf_id_ext_ies_o> >();
  }

private:
  types                                                                                                   type_;
  choice_buffer_t<fixed_bitstring<16, false, true>, protocol_ie_single_container_s<n3_iwf_id_ext_ies_o> > c;

  void destroy_();
};

// NgENB-ID ::= CHOICE
struct ng_enb_id_c {
  struct types_opts {
    enum options { macro_ng_enb_id, short_macro_ng_enb_id, long_macro_ng_enb_id, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ng_enb_id_c() = default;
  ng_enb_id_c(const ng_enb_id_c& other);
  ng_enb_id_c& operator=(const ng_enb_id_c& other);
  ~ng_enb_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<20, false, true>& macro_ng_enb_id()
  {
    assert_choice_type("macroNgENB-ID", type_.to_string(), "NgENB-ID");
    return c.get<fixed_bitstring<20, false, true> >();
  }
  fixed_bitstring<18, false, true>& short_macro_ng_enb_id()
  {
    assert_choice_type("shortMacroNgENB-ID", type_.to_string(), "NgENB-ID");
    return c.get<fixed_bitstring<18, false, true> >();
  }
  fixed_bitstring<21, false, true>& long_macro_ng_enb_id()
  {
    assert_choice_type("longMacroNgENB-ID", type_.to_string(), "NgENB-ID");
    return c.get<fixed_bitstring<21, false, true> >();
  }
  protocol_ie_single_container_s<ng_enb_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "NgENB-ID");
    return c.get<protocol_ie_single_container_s<ng_enb_id_ext_ies_o> >();
  }
  const fixed_bitstring<20, false, true>& macro_ng_enb_id() const
  {
    assert_choice_type("macroNgENB-ID", type_.to_string(), "NgENB-ID");
    return c.get<fixed_bitstring<20, false, true> >();
  }
  const fixed_bitstring<18, false, true>& short_macro_ng_enb_id() const
  {
    assert_choice_type("shortMacroNgENB-ID", type_.to_string(), "NgENB-ID");
    return c.get<fixed_bitstring<18, false, true> >();
  }
  const fixed_bitstring<21, false, true>& long_macro_ng_enb_id() const
  {
    assert_choice_type("longMacroNgENB-ID", type_.to_string(), "NgENB-ID");
    return c.get<fixed_bitstring<21, false, true> >();
  }
  const protocol_ie_single_container_s<ng_enb_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "NgENB-ID");
    return c.get<protocol_ie_single_container_s<ng_enb_id_ext_ies_o> >();
  }
  fixed_bitstring<20, false, true>& set_macro_ng_enb_id()
  {
    set(types::macro_ng_enb_id);
    return c.get<fixed_bitstring<20, false, true> >();
  }
  fixed_bitstring<18, false, true>& set_short_macro_ng_enb_id()
  {
    set(types::short_macro_ng_enb_id);
    return c.get<fixed_bitstring<18, false, true> >();
  }
  fixed_bitstring<21, false, true>& set_long_macro_ng_enb_id()
  {
    set(types::long_macro_ng_enb_id);
    return c.get<fixed_bitstring<21, false, true> >();
  }
  protocol_ie_single_container_s<ng_enb_id_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<ng_enb_id_ext_ies_o> >();
  }

private:
  types                                                                                                   type_;
  choice_buffer_t<fixed_bitstring<21, false, true>, protocol_ie_single_container_s<ng_enb_id_ext_ies_o> > c;

  void destroy_();
};

// GlobalGNB-ID ::= SEQUENCE
struct global_gnb_id_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plmn_id;
  gnb_id_c                 gnb_id;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalN3IWF-ID ::= SEQUENCE
struct global_n3_iwf_id_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plmn_id;
  n3_iwf_id_c              n3_iwf_id;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalNgENB-ID ::= SEQUENCE
struct global_ng_enb_id_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plmn_id;
  ng_enb_id_c              ng_enb_id;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalRANNodeID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o global_ran_node_id_ext_ies_o;

// TAI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o tai_ext_ies_o;

// AMFPagingTarget-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o amf_paging_target_ext_ies_o;

// GlobalRANNodeID ::= CHOICE
struct global_ran_node_id_c {
  struct types_opts {
    enum options { global_gnb_id, global_ng_enb_id, global_n3_iwf_id, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  global_ran_node_id_c() = default;
  global_ran_node_id_c(const global_ran_node_id_c& other);
  global_ran_node_id_c& operator=(const global_ran_node_id_c& other);
  ~global_ran_node_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  global_gnb_id_s& global_gnb_id()
  {
    assert_choice_type("globalGNB-ID", type_.to_string(), "GlobalRANNodeID");
    return c.get<global_gnb_id_s>();
  }
  global_ng_enb_id_s& global_ng_enb_id()
  {
    assert_choice_type("globalNgENB-ID", type_.to_string(), "GlobalRANNodeID");
    return c.get<global_ng_enb_id_s>();
  }
  global_n3_iwf_id_s& global_n3_iwf_id()
  {
    assert_choice_type("globalN3IWF-ID", type_.to_string(), "GlobalRANNodeID");
    return c.get<global_n3_iwf_id_s>();
  }
  protocol_ie_single_container_s<global_ran_node_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "GlobalRANNodeID");
    return c.get<protocol_ie_single_container_s<global_ran_node_id_ext_ies_o> >();
  }
  const global_gnb_id_s& global_gnb_id() const
  {
    assert_choice_type("globalGNB-ID", type_.to_string(), "GlobalRANNodeID");
    return c.get<global_gnb_id_s>();
  }
  const global_ng_enb_id_s& global_ng_enb_id() const
  {
    assert_choice_type("globalNgENB-ID", type_.to_string(), "GlobalRANNodeID");
    return c.get<global_ng_enb_id_s>();
  }
  const global_n3_iwf_id_s& global_n3_iwf_id() const
  {
    assert_choice_type("globalN3IWF-ID", type_.to_string(), "GlobalRANNodeID");
    return c.get<global_n3_iwf_id_s>();
  }
  const protocol_ie_single_container_s<global_ran_node_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "GlobalRANNodeID");
    return c.get<protocol_ie_single_container_s<global_ran_node_id_ext_ies_o> >();
  }
  global_gnb_id_s& set_global_gnb_id()
  {
    set(types::global_gnb_id);
    return c.get<global_gnb_id_s>();
  }
  global_ng_enb_id_s& set_global_ng_enb_id()
  {
    set(types::global_ng_enb_id);
    return c.get<global_ng_enb_id_s>();
  }
  global_n3_iwf_id_s& set_global_n3_iwf_id()
  {
    set(types::global_n3_iwf_id);
    return c.get<global_n3_iwf_id_s>();
  }
  protocol_ie_single_container_s<global_ran_node_id_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<global_ran_node_id_ext_ies_o> >();
  }

private:
  types type_;
  choice_buffer_t<global_gnb_id_s,
                  global_n3_iwf_id_s,
                  global_ng_enb_id_s,
                  protocol_ie_single_container_s<global_ran_node_id_ext_ies_o> >
      c;

  void destroy_();
};

// TAI ::= SEQUENCE
struct tai_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plmn_id;
  fixed_octstring<3, true> tac;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMFPagingTarget ::= CHOICE
struct amf_paging_target_c {
  struct types_opts {
    enum options { global_ran_node_id, tai, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  amf_paging_target_c() = default;
  amf_paging_target_c(const amf_paging_target_c& other);
  amf_paging_target_c& operator=(const amf_paging_target_c& other);
  ~amf_paging_target_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  global_ran_node_id_c& global_ran_node_id()
  {
    assert_choice_type("globalRANNodeID", type_.to_string(), "AMFPagingTarget");
    return c.get<global_ran_node_id_c>();
  }
  tai_s& tai()
  {
    assert_choice_type("tAI", type_.to_string(), "AMFPagingTarget");
    return c.get<tai_s>();
  }
  protocol_ie_single_container_s<amf_paging_target_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "AMFPagingTarget");
    return c.get<protocol_ie_single_container_s<amf_paging_target_ext_ies_o> >();
  }
  const global_ran_node_id_c& global_ran_node_id() const
  {
    assert_choice_type("globalRANNodeID", type_.to_string(), "AMFPagingTarget");
    return c.get<global_ran_node_id_c>();
  }
  const tai_s& tai() const
  {
    assert_choice_type("tAI", type_.to_string(), "AMFPagingTarget");
    return c.get<tai_s>();
  }
  const protocol_ie_single_container_s<amf_paging_target_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "AMFPagingTarget");
    return c.get<protocol_ie_single_container_s<amf_paging_target_ext_ies_o> >();
  }
  global_ran_node_id_c& set_global_ran_node_id()
  {
    set(types::global_ran_node_id);
    return c.get<global_ran_node_id_c>();
  }
  tai_s& set_tai()
  {
    set(types::tai);
    return c.get<tai_s>();
  }
  protocol_ie_single_container_s<amf_paging_target_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<amf_paging_target_ext_ies_o> >();
  }

private:
  types                                                                                                     type_;
  choice_buffer_t<global_ran_node_id_c, protocol_ie_single_container_s<amf_paging_target_ext_ies_o>, tai_s> c;

  void destroy_();
};

// TimerApproachForGUAMIRemoval ::= ENUMERATED
struct timer_approach_for_guami_removal_opts {
  enum options { apply_timer, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<timer_approach_for_guami_removal_opts, true> timer_approach_for_guami_removal_e;

// UnavailableGUAMIItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o unavailable_guami_item_ext_ies_o;

// UnavailableGUAMIItem ::= SEQUENCE
struct unavailable_guami_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                 ext                                      = false;
  bool                                 timer_approach_for_guami_removal_present = false;
  bool                                 backup_amf_name_present                  = false;
  bool                                 ie_exts_present                          = false;
  guami_s                              guami;
  timer_approach_for_guami_removal_e   timer_approach_for_guami_removal;
  printable_string<1, 150, true, true> backup_amf_name;
  ie_exts_l_                           ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UnavailableGUAMIList ::= SEQUENCE (SIZE (1..256)) OF UnavailableGUAMIItem
using unavailable_guami_list_l = dyn_array<unavailable_guami_item_s>;

// AMFStatusIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct amf_status_ind_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { unavailable_guami_list, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::unavailable_guami_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unavailable_guami_list_l&       unavailable_guami_list() { return c; }
    const unavailable_guami_list_l& unavailable_guami_list() const { return c; }

  private:
    unavailable_guami_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// AMFStatusIndication ::= SEQUENCE
struct amf_status_ind_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<dyn_seq_of<unavailable_guami_item_s, 1, 256, true> > unavailable_guami_list;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataForwardingAccepted ::= ENUMERATED
struct data_forwarding_accepted_opts {
  enum options { data_forwarding_accepted, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<data_forwarding_accepted_opts, true> data_forwarding_accepted_e;

// GTPTunnel-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o gtp_tunnel_ext_ies_o;

// QosFlowItemWithDataForwarding-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_item_with_data_forwarding_ext_ies_o;

// GTPTunnel ::= SEQUENCE
struct gtp_tunnel_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  bounded_bitstring<1, 160, true, true> transport_layer_address;
  fixed_octstring<4, true>              gtp_teid;
  ie_exts_l_                            ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowItemWithDataForwarding ::= SEQUENCE
struct qos_flow_item_with_data_forwarding_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                       ext                              = false;
  bool                       data_forwarding_accepted_present = false;
  bool                       ie_exts_present                  = false;
  uint8_t                    qos_flow_id                      = 0;
  data_forwarding_accepted_e data_forwarding_accepted;
  ie_exts_l_                 ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UPTransportLayerInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o up_transport_layer_info_ext_ies_o;

// AdditionalDLUPTNLInformationForHOItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o add_dluptnl_info_for_ho_item_ext_ies_o;

// QosFlowListWithDataForwarding ::= SEQUENCE (SIZE (1..64)) OF QosFlowItemWithDataForwarding
using qos_flow_list_with_data_forwarding_l = dyn_array<qos_flow_item_with_data_forwarding_s>;

// UPTransportLayerInformation ::= CHOICE
struct up_transport_layer_info_c {
  struct types_opts {
    enum options { gtp_tunnel, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  up_transport_layer_info_c() = default;
  up_transport_layer_info_c(const up_transport_layer_info_c& other);
  up_transport_layer_info_c& operator=(const up_transport_layer_info_c& other);
  ~up_transport_layer_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  gtp_tunnel_s& gtp_tunnel()
  {
    assert_choice_type("gTPTunnel", type_.to_string(), "UPTransportLayerInformation");
    return c.get<gtp_tunnel_s>();
  }
  protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "UPTransportLayerInformation");
    return c.get<protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o> >();
  }
  const gtp_tunnel_s& gtp_tunnel() const
  {
    assert_choice_type("gTPTunnel", type_.to_string(), "UPTransportLayerInformation");
    return c.get<gtp_tunnel_s>();
  }
  const protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "UPTransportLayerInformation");
    return c.get<protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o> >();
  }
  gtp_tunnel_s& set_gtp_tunnel()
  {
    set(types::gtp_tunnel);
    return c.get<gtp_tunnel_s>();
  }
  protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o> >();
  }

private:
  types                                                                                             type_;
  choice_buffer_t<gtp_tunnel_s, protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o> > c;

  void destroy_();
};

// AdditionalDLUPTNLInformationForHOItem ::= SEQUENCE
struct add_dluptnl_info_for_ho_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                 ext                                  = false;
  bool                                 add_dl_forwarding_uptnl_info_present = false;
  bool                                 ie_exts_present                      = false;
  up_transport_layer_info_c            add_dl_ngu_up_tnl_info;
  qos_flow_list_with_data_forwarding_l add_qos_flow_setup_resp_list;
  up_transport_layer_info_c            add_dl_forwarding_uptnl_info;
  ie_exts_l_                           ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AdditionalDLUPTNLInformationForHOList ::= SEQUENCE (SIZE (1..3)) OF AdditionalDLUPTNLInformationForHOItem
using add_dluptnl_info_for_ho_list_l = dyn_array<add_dluptnl_info_for_ho_item_s>;

// AllocationAndRetentionPriority-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o alloc_and_retention_prio_ext_ies_o;

// Pre-emptionCapability ::= ENUMERATED
struct pre_emption_cap_opts {
  enum options { shall_not_trigger_pre_emption, may_trigger_pre_emption, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<pre_emption_cap_opts, true> pre_emption_cap_e;

// Pre-emptionVulnerability ::= ENUMERATED
struct pre_emption_vulnerability_opts {
  enum options { not_pre_emptable, pre_emptable, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<pre_emption_vulnerability_opts, true> pre_emption_vulnerability_e;

// AllocationAndRetentionPriority ::= SEQUENCE
struct alloc_and_retention_prio_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                        ext             = false;
  bool                        ie_exts_present = false;
  uint8_t                     prio_level_arp  = 1;
  pre_emption_cap_e           pre_emption_cap;
  pre_emption_vulnerability_e pre_emption_vulnerability;
  ie_exts_l_                  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AllowedNSSAI-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o allowed_nssai_item_ext_ies_o;

// AllowedNSSAI-Item ::= SEQUENCE
struct allowed_nssai_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  s_nssai_s  s_nssai;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AllowedNSSAI ::= SEQUENCE (SIZE (1..8)) OF AllowedNSSAI-Item
using allowed_nssai_l = dyn_array<allowed_nssai_item_s>;

// AllowedTACs ::= SEQUENCE (SIZE (1..16)) OF OCTET STRING (SIZE (3))
using allowed_tacs_l = bounded_array<fixed_octstring<3, true>, 16>;

// EUTRA-CGI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o eutra_cgi_ext_ies_o;

// NR-CGI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o nr_cgi_ext_ies_o;

// EUTRA-CGI ::= SEQUENCE
struct eutra_cgi_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<28, false, true> eutra_cell_id;
  ie_exts_l_                       ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGRAN-CGI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o ngran_cgi_ext_ies_o;

// NR-CGI ::= SEQUENCE
struct nr_cgi_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<36, false, true> nrcell_id;
  ie_exts_l_                       ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaOfInterestCellItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o area_of_interest_cell_item_ext_ies_o;

// AreaOfInterestRANNodeItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o area_of_interest_ran_node_item_ext_ies_o;

// AreaOfInterestTAIItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o area_of_interest_tai_item_ext_ies_o;

// NGRAN-CGI ::= CHOICE
struct ngran_cgi_c {
  struct types_opts {
    enum options { nr_cgi, eutra_cgi, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ngran_cgi_c() = default;
  ngran_cgi_c(const ngran_cgi_c& other);
  ngran_cgi_c& operator=(const ngran_cgi_c& other);
  ~ngran_cgi_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  nr_cgi_s& nr_cgi()
  {
    assert_choice_type("nR-CGI", type_.to_string(), "NGRAN-CGI");
    return c.get<nr_cgi_s>();
  }
  eutra_cgi_s& eutra_cgi()
  {
    assert_choice_type("eUTRA-CGI", type_.to_string(), "NGRAN-CGI");
    return c.get<eutra_cgi_s>();
  }
  protocol_ie_single_container_s<ngran_cgi_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "NGRAN-CGI");
    return c.get<protocol_ie_single_container_s<ngran_cgi_ext_ies_o> >();
  }
  const nr_cgi_s& nr_cgi() const
  {
    assert_choice_type("nR-CGI", type_.to_string(), "NGRAN-CGI");
    return c.get<nr_cgi_s>();
  }
  const eutra_cgi_s& eutra_cgi() const
  {
    assert_choice_type("eUTRA-CGI", type_.to_string(), "NGRAN-CGI");
    return c.get<eutra_cgi_s>();
  }
  const protocol_ie_single_container_s<ngran_cgi_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "NGRAN-CGI");
    return c.get<protocol_ie_single_container_s<ngran_cgi_ext_ies_o> >();
  }
  nr_cgi_s& set_nr_cgi()
  {
    set(types::nr_cgi);
    return c.get<nr_cgi_s>();
  }
  eutra_cgi_s& set_eutra_cgi()
  {
    set(types::eutra_cgi);
    return c.get<eutra_cgi_s>();
  }
  protocol_ie_single_container_s<ngran_cgi_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<ngran_cgi_ext_ies_o> >();
  }

private:
  types                                                                                        type_;
  choice_buffer_t<eutra_cgi_s, nr_cgi_s, protocol_ie_single_container_s<ngran_cgi_ext_ies_o> > c;

  void destroy_();
};

// AreaOfInterestCellItem ::= SEQUENCE
struct area_of_interest_cell_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool        ext             = false;
  bool        ie_exts_present = false;
  ngran_cgi_c ngran_cgi;
  ie_exts_l_  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaOfInterestRANNodeItem ::= SEQUENCE
struct area_of_interest_ran_node_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                 ext             = false;
  bool                 ie_exts_present = false;
  global_ran_node_id_c global_ran_node_id;
  ie_exts_l_           ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaOfInterestTAIItem ::= SEQUENCE
struct area_of_interest_tai_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  tai_s      tai;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaOfInterest-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o area_of_interest_ext_ies_o;

// AreaOfInterestCellList ::= SEQUENCE (SIZE (1..256)) OF AreaOfInterestCellItem
using area_of_interest_cell_list_l = dyn_array<area_of_interest_cell_item_s>;

// AreaOfInterestRANNodeList ::= SEQUENCE (SIZE (1..64)) OF AreaOfInterestRANNodeItem
using area_of_interest_ran_node_list_l = dyn_array<area_of_interest_ran_node_item_s>;

// AreaOfInterestTAIList ::= SEQUENCE (SIZE (1..16)) OF AreaOfInterestTAIItem
using area_of_interest_tai_list_l = dyn_array<area_of_interest_tai_item_s>;

// AreaOfInterest ::= SEQUENCE
struct area_of_interest_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                             ext                                    = false;
  bool                             area_of_interest_tai_list_present      = false;
  bool                             area_of_interest_cell_list_present     = false;
  bool                             area_of_interest_ran_node_list_present = false;
  bool                             ie_exts_present                        = false;
  area_of_interest_tai_list_l      area_of_interest_tai_list;
  area_of_interest_cell_list_l     area_of_interest_cell_list;
  area_of_interest_ran_node_list_l area_of_interest_ran_node_list;
  ie_exts_l_                       ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaOfInterestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o area_of_interest_item_ext_ies_o;

// AreaOfInterestItem ::= SEQUENCE
struct area_of_interest_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool               ext             = false;
  bool               ie_exts_present = false;
  area_of_interest_s area_of_interest;
  uint8_t            location_report_ref_id = 1;
  ie_exts_l_         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaOfInterestList ::= SEQUENCE (SIZE (1..64)) OF AreaOfInterestItem
using area_of_interest_list_l = dyn_array<area_of_interest_item_s>;

// RecommendedCellItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o recommended_cell_item_ext_ies_o;

// RecommendedCellItem ::= SEQUENCE
struct recommended_cell_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool        ext                         = false;
  bool        time_stayed_in_cell_present = false;
  bool        ie_exts_present             = false;
  ngran_cgi_c ngran_cgi;
  uint16_t    time_stayed_in_cell = 0;
  ie_exts_l_  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RecommendedCellList ::= SEQUENCE (SIZE (1..16)) OF RecommendedCellItem
using recommended_cell_list_l = dyn_array<recommended_cell_item_s>;

// RecommendedCellsForPaging-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o recommended_cells_for_paging_ext_ies_o;

// AssistanceDataForRecommendedCells-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o assist_data_for_recommended_cells_ext_ies_o;

// NextPagingAreaScope ::= ENUMERATED
struct next_paging_area_scope_opts {
  enum options { same, changed, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<next_paging_area_scope_opts, true> next_paging_area_scope_e;

// PagingAttemptInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o paging_attempt_info_ext_ies_o;

// RecommendedCellsForPaging ::= SEQUENCE
struct recommended_cells_for_paging_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                    ext             = false;
  bool                    ie_exts_present = false;
  recommended_cell_list_l recommended_cell_list;
  ie_exts_l_              ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssistanceDataForPaging-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o assist_data_for_paging_ext_ies_o;

// AssistanceDataForRecommendedCells ::= SEQUENCE
struct assist_data_for_recommended_cells_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  recommended_cells_for_paging_s recommended_cells_for_paging;
  ie_exts_l_                     ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingAttemptInformation ::= SEQUENCE
struct paging_attempt_info_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext                            = false;
  bool                     next_paging_area_scope_present = false;
  bool                     ie_exts_present                = false;
  uint8_t                  paging_attempt_count           = 1;
  uint8_t                  intended_nof_paging_attempts   = 1;
  next_paging_area_scope_e next_paging_area_scope;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssistanceDataForPaging ::= SEQUENCE
struct assist_data_for_paging_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                ext                                       = false;
  bool                                assist_data_for_recommended_cells_present = false;
  bool                                paging_attempt_info_present               = false;
  bool                                ie_exts_present                           = false;
  assist_data_for_recommended_cells_s assist_data_for_recommended_cells;
  paging_attempt_info_s               paging_attempt_info;
  ie_exts_l_                          ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssociatedQosFlowItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o associated_qos_flow_item_ext_ies_o;

// AssociatedQosFlowItem ::= SEQUENCE
struct associated_qos_flow_item_s {
  struct qos_flow_map_ind_opts {
    enum options { ul, dl, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<qos_flow_map_ind_opts, true> qos_flow_map_ind_e_;
  typedef protocol_ext_container_empty_l          ie_exts_l_;

  // member variables
  bool                ext                      = false;
  bool                qos_flow_map_ind_present = false;
  bool                ie_exts_present          = false;
  uint8_t             qos_flow_id              = 0;
  qos_flow_map_ind_e_ qos_flow_map_ind;
  ie_exts_l_          ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssociatedQosFlowList ::= SEQUENCE (SIZE (1..64)) OF AssociatedQosFlowItem
using associated_qos_flow_list_l = dyn_array<associated_qos_flow_item_s>;

// CancelledCellsInEAI-EUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o cancelled_cells_in_eai_eutra_item_ext_ies_o;

// CancelledCellsInEAI-NR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o cancelled_cells_in_eai_nr_item_ext_ies_o;

// CancelledCellsInTAI-EUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o cancelled_cells_in_tai_eutra_item_ext_ies_o;

// CancelledCellsInTAI-NR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o cancelled_cells_in_tai_nr_item_ext_ies_o;

// CancelledCellsInEAI-EUTRA-Item ::= SEQUENCE
struct cancelled_cells_in_eai_eutra_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool        ext             = false;
  bool        ie_exts_present = false;
  eutra_cgi_s eutra_cgi;
  uint32_t    nof_broadcasts = 0;
  ie_exts_l_  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CancelledCellsInEAI-NR-Item ::= SEQUENCE
struct cancelled_cells_in_eai_nr_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  nr_cgi_s   nr_cgi;
  uint32_t   nof_broadcasts = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CancelledCellsInTAI-EUTRA-Item ::= SEQUENCE
struct cancelled_cells_in_tai_eutra_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool        ext             = false;
  bool        ie_exts_present = false;
  eutra_cgi_s eutra_cgi;
  uint32_t    nof_broadcasts = 0;
  ie_exts_l_  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CancelledCellsInTAI-NR-Item ::= SEQUENCE
struct cancelled_cells_in_tai_nr_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  nr_cgi_s   nr_cgi;
  uint32_t   nof_broadcasts = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CancelledCellsInEAI-EUTRA ::= SEQUENCE (SIZE (1..65535)) OF CancelledCellsInEAI-EUTRA-Item
using cancelled_cells_in_eai_eutra_l = dyn_array<cancelled_cells_in_eai_eutra_item_s>;

// CancelledCellsInEAI-NR ::= SEQUENCE (SIZE (1..65535)) OF CancelledCellsInEAI-NR-Item
using cancelled_cells_in_eai_nr_l = dyn_array<cancelled_cells_in_eai_nr_item_s>;

// CancelledCellsInTAI-EUTRA ::= SEQUENCE (SIZE (1..65535)) OF CancelledCellsInTAI-EUTRA-Item
using cancelled_cells_in_tai_eutra_l = dyn_array<cancelled_cells_in_tai_eutra_item_s>;

// CancelledCellsInTAI-NR ::= SEQUENCE (SIZE (1..65535)) OF CancelledCellsInTAI-NR-Item
using cancelled_cells_in_tai_nr_l = dyn_array<cancelled_cells_in_tai_nr_item_s>;

// CellIDCancelledEUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o cell_id_cancelled_eutra_item_ext_ies_o;

// CellIDCancelledNR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o cell_id_cancelled_nr_item_ext_ies_o;

// EmergencyAreaIDCancelledEUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o emergency_area_id_cancelled_eutra_item_ext_ies_o;

// EmergencyAreaIDCancelledNR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o emergency_area_id_cancelled_nr_item_ext_ies_o;

// TAICancelledEUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o tai_cancelled_eutra_item_ext_ies_o;

// TAICancelledNR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o tai_cancelled_nr_item_ext_ies_o;

// CellIDCancelledEUTRA-Item ::= SEQUENCE
struct cell_id_cancelled_eutra_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool        ext             = false;
  bool        ie_exts_present = false;
  eutra_cgi_s eutra_cgi;
  uint32_t    nof_broadcasts = 0;
  ie_exts_l_  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellIDCancelledNR-Item ::= SEQUENCE
struct cell_id_cancelled_nr_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  nr_cgi_s   nr_cgi;
  uint32_t   nof_broadcasts = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EmergencyAreaIDCancelledEUTRA-Item ::= SEQUENCE
struct emergency_area_id_cancelled_eutra_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  fixed_octstring<3, true>       emergency_area_id;
  cancelled_cells_in_eai_eutra_l cancelled_cells_in_eai_eutra;
  ie_exts_l_                     ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EmergencyAreaIDCancelledNR-Item ::= SEQUENCE
struct emergency_area_id_cancelled_nr_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                        ext             = false;
  bool                        ie_exts_present = false;
  fixed_octstring<3, true>    emergency_area_id;
  cancelled_cells_in_eai_nr_l cancelled_cells_in_eai_nr;
  ie_exts_l_                  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAICancelledEUTRA-Item ::= SEQUENCE
struct tai_cancelled_eutra_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  tai_s                          tai;
  cancelled_cells_in_tai_eutra_l cancelled_cells_in_tai_eutra;
  ie_exts_l_                     ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAICancelledNR-Item ::= SEQUENCE
struct tai_cancelled_nr_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                        ext             = false;
  bool                        ie_exts_present = false;
  tai_s                       tai;
  cancelled_cells_in_tai_nr_l cancelled_cells_in_tai_nr;
  ie_exts_l_                  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastCancelledAreaList-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o broadcast_cancelled_area_list_ext_ies_o;

// CellIDCancelledEUTRA ::= SEQUENCE (SIZE (1..65535)) OF CellIDCancelledEUTRA-Item
using cell_id_cancelled_eutra_l = dyn_array<cell_id_cancelled_eutra_item_s>;

// CellIDCancelledNR ::= SEQUENCE (SIZE (1..65535)) OF CellIDCancelledNR-Item
using cell_id_cancelled_nr_l = dyn_array<cell_id_cancelled_nr_item_s>;

// EmergencyAreaIDCancelledEUTRA ::= SEQUENCE (SIZE (1..65535)) OF EmergencyAreaIDCancelledEUTRA-Item
using emergency_area_id_cancelled_eutra_l = dyn_array<emergency_area_id_cancelled_eutra_item_s>;

// EmergencyAreaIDCancelledNR ::= SEQUENCE (SIZE (1..65535)) OF EmergencyAreaIDCancelledNR-Item
using emergency_area_id_cancelled_nr_l = dyn_array<emergency_area_id_cancelled_nr_item_s>;

// TAICancelledEUTRA ::= SEQUENCE (SIZE (1..65535)) OF TAICancelledEUTRA-Item
using tai_cancelled_eutra_l = dyn_array<tai_cancelled_eutra_item_s>;

// TAICancelledNR ::= SEQUENCE (SIZE (1..65535)) OF TAICancelledNR-Item
using tai_cancelled_nr_l = dyn_array<tai_cancelled_nr_item_s>;

// BroadcastCancelledAreaList ::= CHOICE
struct broadcast_cancelled_area_list_c {
  struct types_opts {
    enum options {
      cell_id_cancelled_eutra,
      tai_cancelled_eutra,
      emergency_area_id_cancelled_eutra,
      cell_id_cancelled_nr,
      tai_cancelled_nr,
      emergency_area_id_cancelled_nr,
      choice_exts,
      nulltype
    } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  broadcast_cancelled_area_list_c() = default;
  broadcast_cancelled_area_list_c(const broadcast_cancelled_area_list_c& other);
  broadcast_cancelled_area_list_c& operator=(const broadcast_cancelled_area_list_c& other);
  ~broadcast_cancelled_area_list_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_id_cancelled_eutra_l& cell_id_cancelled_eutra()
  {
    assert_choice_type("cellIDCancelledEUTRA", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<cell_id_cancelled_eutra_l>();
  }
  tai_cancelled_eutra_l& tai_cancelled_eutra()
  {
    assert_choice_type("tAICancelledEUTRA", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<tai_cancelled_eutra_l>();
  }
  emergency_area_id_cancelled_eutra_l& emergency_area_id_cancelled_eutra()
  {
    assert_choice_type("emergencyAreaIDCancelledEUTRA", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<emergency_area_id_cancelled_eutra_l>();
  }
  cell_id_cancelled_nr_l& cell_id_cancelled_nr()
  {
    assert_choice_type("cellIDCancelledNR", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<cell_id_cancelled_nr_l>();
  }
  tai_cancelled_nr_l& tai_cancelled_nr()
  {
    assert_choice_type("tAICancelledNR", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<tai_cancelled_nr_l>();
  }
  emergency_area_id_cancelled_nr_l& emergency_area_id_cancelled_nr()
  {
    assert_choice_type("emergencyAreaIDCancelledNR", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<emergency_area_id_cancelled_nr_l>();
  }
  protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o> >();
  }
  const cell_id_cancelled_eutra_l& cell_id_cancelled_eutra() const
  {
    assert_choice_type("cellIDCancelledEUTRA", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<cell_id_cancelled_eutra_l>();
  }
  const tai_cancelled_eutra_l& tai_cancelled_eutra() const
  {
    assert_choice_type("tAICancelledEUTRA", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<tai_cancelled_eutra_l>();
  }
  const emergency_area_id_cancelled_eutra_l& emergency_area_id_cancelled_eutra() const
  {
    assert_choice_type("emergencyAreaIDCancelledEUTRA", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<emergency_area_id_cancelled_eutra_l>();
  }
  const cell_id_cancelled_nr_l& cell_id_cancelled_nr() const
  {
    assert_choice_type("cellIDCancelledNR", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<cell_id_cancelled_nr_l>();
  }
  const tai_cancelled_nr_l& tai_cancelled_nr() const
  {
    assert_choice_type("tAICancelledNR", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<tai_cancelled_nr_l>();
  }
  const emergency_area_id_cancelled_nr_l& emergency_area_id_cancelled_nr() const
  {
    assert_choice_type("emergencyAreaIDCancelledNR", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<emergency_area_id_cancelled_nr_l>();
  }
  const protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o> >();
  }
  cell_id_cancelled_eutra_l& set_cell_id_cancelled_eutra()
  {
    set(types::cell_id_cancelled_eutra);
    return c.get<cell_id_cancelled_eutra_l>();
  }
  tai_cancelled_eutra_l& set_tai_cancelled_eutra()
  {
    set(types::tai_cancelled_eutra);
    return c.get<tai_cancelled_eutra_l>();
  }
  emergency_area_id_cancelled_eutra_l& set_emergency_area_id_cancelled_eutra()
  {
    set(types::emergency_area_id_cancelled_eutra);
    return c.get<emergency_area_id_cancelled_eutra_l>();
  }
  cell_id_cancelled_nr_l& set_cell_id_cancelled_nr()
  {
    set(types::cell_id_cancelled_nr);
    return c.get<cell_id_cancelled_nr_l>();
  }
  tai_cancelled_nr_l& set_tai_cancelled_nr()
  {
    set(types::tai_cancelled_nr);
    return c.get<tai_cancelled_nr_l>();
  }
  emergency_area_id_cancelled_nr_l& set_emergency_area_id_cancelled_nr()
  {
    set(types::emergency_area_id_cancelled_nr);
    return c.get<emergency_area_id_cancelled_nr_l>();
  }
  protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o> >();
  }

private:
  types type_;
  choice_buffer_t<cell_id_cancelled_eutra_l,
                  cell_id_cancelled_nr_l,
                  emergency_area_id_cancelled_eutra_l,
                  emergency_area_id_cancelled_nr_l,
                  protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o>,
                  tai_cancelled_eutra_l,
                  tai_cancelled_nr_l>
      c;

  void destroy_();
};

// CompletedCellsInEAI-EUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o completed_cells_in_eai_eutra_item_ext_ies_o;

// CompletedCellsInEAI-NR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o completed_cells_in_eai_nr_item_ext_ies_o;

// CompletedCellsInTAI-EUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o completed_cells_in_tai_eutra_item_ext_ies_o;

// CompletedCellsInTAI-NR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o completed_cells_in_tai_nr_item_ext_ies_o;

// CompletedCellsInEAI-EUTRA-Item ::= SEQUENCE
struct completed_cells_in_eai_eutra_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool        ext             = false;
  bool        ie_exts_present = false;
  eutra_cgi_s eutra_cgi;
  ie_exts_l_  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CompletedCellsInEAI-NR-Item ::= SEQUENCE
struct completed_cells_in_eai_nr_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  nr_cgi_s   nr_cgi;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CompletedCellsInTAI-EUTRA-Item ::= SEQUENCE
struct completed_cells_in_tai_eutra_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool        ext             = false;
  bool        ie_exts_present = false;
  eutra_cgi_s eutra_cgi;
  ie_exts_l_  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CompletedCellsInTAI-NR-Item ::= SEQUENCE
struct completed_cells_in_tai_nr_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  nr_cgi_s   nr_cgi;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellIDBroadcastEUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o cell_id_broadcast_eutra_item_ext_ies_o;

// CellIDBroadcastNR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o cell_id_broadcast_nr_item_ext_ies_o;

// CompletedCellsInEAI-EUTRA ::= SEQUENCE (SIZE (1..65535)) OF CompletedCellsInEAI-EUTRA-Item
using completed_cells_in_eai_eutra_l = dyn_array<completed_cells_in_eai_eutra_item_s>;

// CompletedCellsInEAI-NR ::= SEQUENCE (SIZE (1..65535)) OF CompletedCellsInEAI-NR-Item
using completed_cells_in_eai_nr_l = dyn_array<completed_cells_in_eai_nr_item_s>;

// CompletedCellsInTAI-EUTRA ::= SEQUENCE (SIZE (1..65535)) OF CompletedCellsInTAI-EUTRA-Item
using completed_cells_in_tai_eutra_l = dyn_array<completed_cells_in_tai_eutra_item_s>;

// CompletedCellsInTAI-NR ::= SEQUENCE (SIZE (1..65535)) OF CompletedCellsInTAI-NR-Item
using completed_cells_in_tai_nr_l = dyn_array<completed_cells_in_tai_nr_item_s>;

// EmergencyAreaIDBroadcastEUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o emergency_area_id_broadcast_eutra_item_ext_ies_o;

// EmergencyAreaIDBroadcastNR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o emergency_area_id_broadcast_nr_item_ext_ies_o;

// TAIBroadcastEUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o tai_broadcast_eutra_item_ext_ies_o;

// TAIBroadcastNR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o tai_broadcast_nr_item_ext_ies_o;

// CellIDBroadcastEUTRA-Item ::= SEQUENCE
struct cell_id_broadcast_eutra_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool        ext             = false;
  bool        ie_exts_present = false;
  eutra_cgi_s eutra_cgi;
  ie_exts_l_  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellIDBroadcastNR-Item ::= SEQUENCE
struct cell_id_broadcast_nr_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  nr_cgi_s   nr_cgi;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EmergencyAreaIDBroadcastEUTRA-Item ::= SEQUENCE
struct emergency_area_id_broadcast_eutra_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  fixed_octstring<3, true>       emergency_area_id;
  completed_cells_in_eai_eutra_l completed_cells_in_eai_eutra;
  ie_exts_l_                     ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EmergencyAreaIDBroadcastNR-Item ::= SEQUENCE
struct emergency_area_id_broadcast_nr_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                        ext             = false;
  bool                        ie_exts_present = false;
  fixed_octstring<3, true>    emergency_area_id;
  completed_cells_in_eai_nr_l completed_cells_in_eai_nr;
  ie_exts_l_                  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAIBroadcastEUTRA-Item ::= SEQUENCE
struct tai_broadcast_eutra_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  tai_s                          tai;
  completed_cells_in_tai_eutra_l completed_cells_in_tai_eutra;
  ie_exts_l_                     ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAIBroadcastNR-Item ::= SEQUENCE
struct tai_broadcast_nr_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                        ext             = false;
  bool                        ie_exts_present = false;
  tai_s                       tai;
  completed_cells_in_tai_nr_l completed_cells_in_tai_nr;
  ie_exts_l_                  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastCompletedAreaList-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o broadcast_completed_area_list_ext_ies_o;

// CellIDBroadcastEUTRA ::= SEQUENCE (SIZE (1..65535)) OF CellIDBroadcastEUTRA-Item
using cell_id_broadcast_eutra_l = dyn_array<cell_id_broadcast_eutra_item_s>;

// CellIDBroadcastNR ::= SEQUENCE (SIZE (1..65535)) OF CellIDBroadcastNR-Item
using cell_id_broadcast_nr_l = dyn_array<cell_id_broadcast_nr_item_s>;

// EmergencyAreaIDBroadcastEUTRA ::= SEQUENCE (SIZE (1..65535)) OF EmergencyAreaIDBroadcastEUTRA-Item
using emergency_area_id_broadcast_eutra_l = dyn_array<emergency_area_id_broadcast_eutra_item_s>;

// EmergencyAreaIDBroadcastNR ::= SEQUENCE (SIZE (1..65535)) OF EmergencyAreaIDBroadcastNR-Item
using emergency_area_id_broadcast_nr_l = dyn_array<emergency_area_id_broadcast_nr_item_s>;

// TAIBroadcastEUTRA ::= SEQUENCE (SIZE (1..65535)) OF TAIBroadcastEUTRA-Item
using tai_broadcast_eutra_l = dyn_array<tai_broadcast_eutra_item_s>;

// TAIBroadcastNR ::= SEQUENCE (SIZE (1..65535)) OF TAIBroadcastNR-Item
using tai_broadcast_nr_l = dyn_array<tai_broadcast_nr_item_s>;

// BroadcastCompletedAreaList ::= CHOICE
struct broadcast_completed_area_list_c {
  struct types_opts {
    enum options {
      cell_id_broadcast_eutra,
      tai_broadcast_eutra,
      emergency_area_id_broadcast_eutra,
      cell_id_broadcast_nr,
      tai_broadcast_nr,
      emergency_area_id_broadcast_nr,
      choice_exts,
      nulltype
    } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  broadcast_completed_area_list_c() = default;
  broadcast_completed_area_list_c(const broadcast_completed_area_list_c& other);
  broadcast_completed_area_list_c& operator=(const broadcast_completed_area_list_c& other);
  ~broadcast_completed_area_list_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_id_broadcast_eutra_l& cell_id_broadcast_eutra()
  {
    assert_choice_type("cellIDBroadcastEUTRA", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<cell_id_broadcast_eutra_l>();
  }
  tai_broadcast_eutra_l& tai_broadcast_eutra()
  {
    assert_choice_type("tAIBroadcastEUTRA", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<tai_broadcast_eutra_l>();
  }
  emergency_area_id_broadcast_eutra_l& emergency_area_id_broadcast_eutra()
  {
    assert_choice_type("emergencyAreaIDBroadcastEUTRA", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<emergency_area_id_broadcast_eutra_l>();
  }
  cell_id_broadcast_nr_l& cell_id_broadcast_nr()
  {
    assert_choice_type("cellIDBroadcastNR", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<cell_id_broadcast_nr_l>();
  }
  tai_broadcast_nr_l& tai_broadcast_nr()
  {
    assert_choice_type("tAIBroadcastNR", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<tai_broadcast_nr_l>();
  }
  emergency_area_id_broadcast_nr_l& emergency_area_id_broadcast_nr()
  {
    assert_choice_type("emergencyAreaIDBroadcastNR", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<emergency_area_id_broadcast_nr_l>();
  }
  protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o> >();
  }
  const cell_id_broadcast_eutra_l& cell_id_broadcast_eutra() const
  {
    assert_choice_type("cellIDBroadcastEUTRA", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<cell_id_broadcast_eutra_l>();
  }
  const tai_broadcast_eutra_l& tai_broadcast_eutra() const
  {
    assert_choice_type("tAIBroadcastEUTRA", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<tai_broadcast_eutra_l>();
  }
  const emergency_area_id_broadcast_eutra_l& emergency_area_id_broadcast_eutra() const
  {
    assert_choice_type("emergencyAreaIDBroadcastEUTRA", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<emergency_area_id_broadcast_eutra_l>();
  }
  const cell_id_broadcast_nr_l& cell_id_broadcast_nr() const
  {
    assert_choice_type("cellIDBroadcastNR", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<cell_id_broadcast_nr_l>();
  }
  const tai_broadcast_nr_l& tai_broadcast_nr() const
  {
    assert_choice_type("tAIBroadcastNR", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<tai_broadcast_nr_l>();
  }
  const emergency_area_id_broadcast_nr_l& emergency_area_id_broadcast_nr() const
  {
    assert_choice_type("emergencyAreaIDBroadcastNR", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<emergency_area_id_broadcast_nr_l>();
  }
  const protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o> >();
  }
  cell_id_broadcast_eutra_l& set_cell_id_broadcast_eutra()
  {
    set(types::cell_id_broadcast_eutra);
    return c.get<cell_id_broadcast_eutra_l>();
  }
  tai_broadcast_eutra_l& set_tai_broadcast_eutra()
  {
    set(types::tai_broadcast_eutra);
    return c.get<tai_broadcast_eutra_l>();
  }
  emergency_area_id_broadcast_eutra_l& set_emergency_area_id_broadcast_eutra()
  {
    set(types::emergency_area_id_broadcast_eutra);
    return c.get<emergency_area_id_broadcast_eutra_l>();
  }
  cell_id_broadcast_nr_l& set_cell_id_broadcast_nr()
  {
    set(types::cell_id_broadcast_nr);
    return c.get<cell_id_broadcast_nr_l>();
  }
  tai_broadcast_nr_l& set_tai_broadcast_nr()
  {
    set(types::tai_broadcast_nr);
    return c.get<tai_broadcast_nr_l>();
  }
  emergency_area_id_broadcast_nr_l& set_emergency_area_id_broadcast_nr()
  {
    set(types::emergency_area_id_broadcast_nr);
    return c.get<emergency_area_id_broadcast_nr_l>();
  }
  protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o> >();
  }

private:
  types type_;
  choice_buffer_t<cell_id_broadcast_eutra_l,
                  cell_id_broadcast_nr_l,
                  emergency_area_id_broadcast_eutra_l,
                  emergency_area_id_broadcast_nr_l,
                  protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o>,
                  tai_broadcast_eutra_l,
                  tai_broadcast_nr_l>
      c;

  void destroy_();
};

// BroadcastPLMNItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o broadcast_plmn_item_ext_ies_o;

// BroadcastPLMNItem ::= SEQUENCE
struct broadcast_plmn_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plmn_id;
  slice_support_list_l     tai_slice_support_list;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastPLMNList ::= SEQUENCE (SIZE (1..12)) OF BroadcastPLMNItem
using broadcast_plmn_list_l = dyn_array<broadcast_plmn_item_s>;

// COUNTValueForPDCP-SN12-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o count_value_for_pdcp_sn12_ext_ies_o;

// COUNTValueForPDCP-SN12 ::= SEQUENCE
struct count_value_for_pdcp_sn12_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  uint16_t   pdcp_sn12       = 0;
  uint32_t   hfn_pdcp_sn12   = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// COUNTValueForPDCP-SN18-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o count_value_for_pdcp_sn18_ext_ies_o;

// COUNTValueForPDCP-SN18 ::= SEQUENCE
struct count_value_for_pdcp_sn18_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  uint32_t   pdcp_sn18       = 0;
  uint16_t   hfn_pdcp_sn18   = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellIDListForRestart-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o cell_id_list_for_restart_ext_ies_o;

// EUTRA-CGIList ::= SEQUENCE (SIZE (1..256)) OF EUTRA-CGI
using eutra_cgi_list_l = dyn_array<eutra_cgi_s>;

// NR-CGIList ::= SEQUENCE (SIZE (1..16384)) OF NR-CGI
using nr_cgi_list_l = dyn_array<nr_cgi_s>;

// CellIDListForRestart ::= CHOICE
struct cell_id_list_for_restart_c {
  struct types_opts {
    enum options { eutra_cgi_listfor_restart, nr_cgi_listfor_restart, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cell_id_list_for_restart_c() = default;
  cell_id_list_for_restart_c(const cell_id_list_for_restart_c& other);
  cell_id_list_for_restart_c& operator=(const cell_id_list_for_restart_c& other);
  ~cell_id_list_for_restart_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_cgi_list_l& eutra_cgi_listfor_restart()
  {
    assert_choice_type("eUTRA-CGIListforRestart", type_.to_string(), "CellIDListForRestart");
    return c.get<eutra_cgi_list_l>();
  }
  nr_cgi_list_l& nr_cgi_listfor_restart()
  {
    assert_choice_type("nR-CGIListforRestart", type_.to_string(), "CellIDListForRestart");
    return c.get<nr_cgi_list_l>();
  }
  protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "CellIDListForRestart");
    return c.get<protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o> >();
  }
  const eutra_cgi_list_l& eutra_cgi_listfor_restart() const
  {
    assert_choice_type("eUTRA-CGIListforRestart", type_.to_string(), "CellIDListForRestart");
    return c.get<eutra_cgi_list_l>();
  }
  const nr_cgi_list_l& nr_cgi_listfor_restart() const
  {
    assert_choice_type("nR-CGIListforRestart", type_.to_string(), "CellIDListForRestart");
    return c.get<nr_cgi_list_l>();
  }
  const protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "CellIDListForRestart");
    return c.get<protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o> >();
  }
  eutra_cgi_list_l& set_eutra_cgi_listfor_restart()
  {
    set(types::eutra_cgi_listfor_restart);
    return c.get<eutra_cgi_list_l>();
  }
  nr_cgi_list_l& set_nr_cgi_listfor_restart()
  {
    set(types::nr_cgi_listfor_restart);
    return c.get<nr_cgi_list_l>();
  }
  protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o> >();
  }

private:
  types type_;
  choice_buffer_t<eutra_cgi_list_l, nr_cgi_list_l, protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o> >
      c;

  void destroy_();
};

// CellTrafficTraceIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct cell_traffic_trace_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ngran_trace_id,
        ngran_cgi,
        trace_collection_entity_ip_address,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    fixed_octstring<8, true>& ngran_trace_id()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<fixed_octstring<8, true> >();
    }
    ngran_cgi_c& ngran_cgi()
    {
      assert_choice_type("NGRAN-CGI", type_.to_string(), "Value");
      return c.get<ngran_cgi_c>();
    }
    bounded_bitstring<1, 160, true, true>& trace_collection_entity_ip_address()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<bounded_bitstring<1, 160, true, true> >();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const fixed_octstring<8, true>& ngran_trace_id() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<fixed_octstring<8, true> >();
    }
    const ngran_cgi_c& ngran_cgi() const
    {
      assert_choice_type("NGRAN-CGI", type_.to_string(), "Value");
      return c.get<ngran_cgi_c>();
    }
    const bounded_bitstring<1, 160, true, true>& trace_collection_entity_ip_address() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<bounded_bitstring<1, 160, true, true> >();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    fixed_octstring<8, true>& set_ngran_trace_id()
    {
      set(types::ngran_trace_id);
      return c.get<fixed_octstring<8, true> >();
    }
    ngran_cgi_c& set_ngran_cgi()
    {
      set(types::ngran_cgi);
      return c.get<ngran_cgi_c>();
    }
    bounded_bitstring<1, 160, true, true>& set_trace_collection_entity_ip_address()
    {
      set(types::trace_collection_entity_ip_address);
      return c.get<bounded_bitstring<1, 160, true, true> >();
    }

  private:
    types                                                                                         type_;
    choice_buffer_t<bounded_bitstring<1, 160, true, true>, fixed_octstring<8, true>, ngran_cgi_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// CellTrafficTrace ::= SEQUENCE
struct cell_traffic_trace_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<fixed_octstring<8, true> >                         ngran_trace_id;
    ie_field_s<ngran_cgi_c>                                       ngran_cgi;
    ie_field_s<bounded_bitstring<1, 160, true, true> >            trace_collection_entity_ip_address;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellSize ::= ENUMERATED
struct cell_size_opts {
  enum options { verysmall, small, medium, large, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cell_size_opts, true> cell_size_e;

// CellType-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o cell_type_ext_ies_o;

// CellType ::= SEQUENCE
struct cell_type_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool        ext             = false;
  bool        ie_exts_present = false;
  cell_size_e cell_size;
  ie_exts_l_  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ExpectedUEMovingTrajectoryItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o expected_ue_moving_trajectory_item_ext_ies_o;

// ExpectedUEActivityBehaviour-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o expected_ue_activity_behaviour_ext_ies_o;

// ExpectedUEMovingTrajectoryItem ::= SEQUENCE
struct expected_ue_moving_trajectory_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool        ext                         = false;
  bool        time_stayed_in_cell_present = false;
  bool        ie_exts_present             = false;
  ngran_cgi_c ngran_cgi;
  uint16_t    time_stayed_in_cell = 0;
  ie_exts_l_  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SourceOfUEActivityBehaviourInformation ::= ENUMERATED
struct source_of_ue_activity_behaviour_info_opts {
  enum options { subscription_info, statistics, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<source_of_ue_activity_behaviour_info_opts, true> source_of_ue_activity_behaviour_info_e;

// TAIListForInactiveItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o tai_list_for_inactive_item_ext_ies_o;

// ExpectedHOInterval ::= ENUMERATED
struct expected_ho_interv_opts {
  enum options { sec15, sec30, sec60, sec90, sec120, sec180, long_time, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<expected_ho_interv_opts, true> expected_ho_interv_e;

// ExpectedUEActivityBehaviour ::= SEQUENCE
struct expected_ue_activity_behaviour_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                   ext                                          = false;
  bool                                   expected_activity_period_present             = false;
  bool                                   expected_idle_period_present                 = false;
  bool                                   source_of_ue_activity_behaviour_info_present = false;
  bool                                   ie_exts_present                              = false;
  uint8_t                                expected_activity_period                     = 1;
  uint8_t                                expected_idle_period                         = 1;
  source_of_ue_activity_behaviour_info_e source_of_ue_activity_behaviour_info;
  ie_exts_l_                             ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ExpectedUEBehaviour-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o expected_ue_behaviour_ext_ies_o;

// ExpectedUEMobility ::= ENUMERATED
struct expected_ue_mob_opts {
  enum options { stationary, mobile, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<expected_ue_mob_opts, true> expected_ue_mob_e;

// ExpectedUEMovingTrajectory ::= SEQUENCE (SIZE (1..16)) OF ExpectedUEMovingTrajectoryItem
using expected_ue_moving_trajectory_l = dyn_array<expected_ue_moving_trajectory_item_s>;

// TAIListForInactiveItem ::= SEQUENCE
struct tai_list_for_inactive_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  tai_s      tai;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEIdentityIndexValue-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o ue_id_idx_value_ext_ies_o;

// CoreNetworkAssistanceInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o core_network_assist_info_ext_ies_o;

// ExpectedUEBehaviour ::= SEQUENCE
struct expected_ue_behaviour_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                             ext                                    = false;
  bool                             expected_ue_activity_behaviour_present = false;
  bool                             expected_ho_interv_present             = false;
  bool                             expected_ue_mob_present                = false;
  bool                             expected_ue_moving_trajectory_present  = false;
  bool                             ie_exts_present                        = false;
  expected_ue_activity_behaviour_s expected_ue_activity_behaviour;
  expected_ho_interv_e             expected_ho_interv;
  expected_ue_mob_e                expected_ue_mob;
  expected_ue_moving_trajectory_l  expected_ue_moving_trajectory;
  ie_exts_l_                       ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MICOModeIndication ::= ENUMERATED
struct mico_mode_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<mico_mode_ind_opts, true> mico_mode_ind_e;

// PagingDRX ::= ENUMERATED
struct paging_drx_opts {
  enum options { v32, v64, v128, v256, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<paging_drx_opts, true> paging_drx_e;

// TAIListForInactive ::= SEQUENCE (SIZE (1..16)) OF TAIListForInactiveItem
using tai_list_for_inactive_l = dyn_array<tai_list_for_inactive_item_s>;

// UEIdentityIndexValue ::= CHOICE
struct ue_id_idx_value_c {
  struct types_opts {
    enum options { idx_len10, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ue_id_idx_value_c() = default;
  ue_id_idx_value_c(const ue_id_idx_value_c& other);
  ue_id_idx_value_c& operator=(const ue_id_idx_value_c& other);
  ~ue_id_idx_value_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<10, false, true>& idx_len10()
  {
    assert_choice_type("indexLength10", type_.to_string(), "UEIdentityIndexValue");
    return c.get<fixed_bitstring<10, false, true> >();
  }
  protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "UEIdentityIndexValue");
    return c.get<protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o> >();
  }
  const fixed_bitstring<10, false, true>& idx_len10() const
  {
    assert_choice_type("indexLength10", type_.to_string(), "UEIdentityIndexValue");
    return c.get<fixed_bitstring<10, false, true> >();
  }
  const protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "UEIdentityIndexValue");
    return c.get<protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o> >();
  }
  fixed_bitstring<10, false, true>& set_idx_len10()
  {
    set(types::idx_len10);
    return c.get<fixed_bitstring<10, false, true> >();
  }
  protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o> >();
  }

private:
  types                                                                                                         type_;
  choice_buffer_t<fixed_bitstring<10, false, true>, protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o> > c;

  void destroy_();
};

// CoreNetworkAssistanceInformation ::= SEQUENCE
struct core_network_assist_info_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                            ext                           = false;
  bool                            uespecific_drx_present        = false;
  bool                            mico_mode_ind_present         = false;
  bool                            expected_ue_behaviour_present = false;
  bool                            ie_exts_present               = false;
  ue_id_idx_value_c               ueid_idx_value;
  paging_drx_e                    uespecific_drx;
  fixed_bitstring<8, false, true> periodic_regist_upd_timer;
  mico_mode_ind_e                 mico_mode_ind;
  tai_list_for_inactive_l         tai_list_for_inactive;
  expected_ue_behaviour_s         expected_ue_behaviour;
  ie_exts_l_                      ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBStatusDL12-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o drb_status_dl12_ext_ies_o;

// DRBStatusDL18-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o drb_status_dl18_ext_ies_o;

// DRBStatusDL-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o drb_status_dl_ext_ies_o;

// DRBStatusDL12 ::= SEQUENCE
struct drb_status_dl12_s {
  typedef protocol_ext_container_empty_l ie_ext_l_;

  // member variables
  bool                        ext            = false;
  bool                        ie_ext_present = false;
  count_value_for_pdcp_sn12_s dl_count_value;
  ie_ext_l_                   ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBStatusDL18 ::= SEQUENCE
struct drb_status_dl18_s {
  typedef protocol_ext_container_empty_l ie_ext_l_;

  // member variables
  bool                        ext            = false;
  bool                        ie_ext_present = false;
  count_value_for_pdcp_sn18_s dl_count_value;
  ie_ext_l_                   ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBStatusDL ::= CHOICE
struct drb_status_dl_c {
  struct types_opts {
    enum options { drb_status_dl12, drb_status_dl18, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  drb_status_dl_c() = default;
  drb_status_dl_c(const drb_status_dl_c& other);
  drb_status_dl_c& operator=(const drb_status_dl_c& other);
  ~drb_status_dl_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  drb_status_dl12_s& drb_status_dl12()
  {
    assert_choice_type("dRBStatusDL12", type_.to_string(), "DRBStatusDL");
    return c.get<drb_status_dl12_s>();
  }
  drb_status_dl18_s& drb_status_dl18()
  {
    assert_choice_type("dRBStatusDL18", type_.to_string(), "DRBStatusDL");
    return c.get<drb_status_dl18_s>();
  }
  protocol_ie_single_container_s<drb_status_dl_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "DRBStatusDL");
    return c.get<protocol_ie_single_container_s<drb_status_dl_ext_ies_o> >();
  }
  const drb_status_dl12_s& drb_status_dl12() const
  {
    assert_choice_type("dRBStatusDL12", type_.to_string(), "DRBStatusDL");
    return c.get<drb_status_dl12_s>();
  }
  const drb_status_dl18_s& drb_status_dl18() const
  {
    assert_choice_type("dRBStatusDL18", type_.to_string(), "DRBStatusDL");
    return c.get<drb_status_dl18_s>();
  }
  const protocol_ie_single_container_s<drb_status_dl_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "DRBStatusDL");
    return c.get<protocol_ie_single_container_s<drb_status_dl_ext_ies_o> >();
  }
  drb_status_dl12_s& set_drb_status_dl12()
  {
    set(types::drb_status_dl12);
    return c.get<drb_status_dl12_s>();
  }
  drb_status_dl18_s& set_drb_status_dl18()
  {
    set(types::drb_status_dl18);
    return c.get<drb_status_dl18_s>();
  }
  protocol_ie_single_container_s<drb_status_dl_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<drb_status_dl_ext_ies_o> >();
  }

private:
  types                                                                                                           type_;
  choice_buffer_t<drb_status_dl12_s, drb_status_dl18_s, protocol_ie_single_container_s<drb_status_dl_ext_ies_o> > c;

  void destroy_();
};

// DRBStatusUL12-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o drb_status_ul12_ext_ies_o;

// DRBStatusUL18-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o drb_status_ul18_ext_ies_o;

// DRBStatusUL-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o drb_status_ul_ext_ies_o;

// DRBStatusUL12 ::= SEQUENCE
struct drb_status_ul12_s {
  typedef protocol_ext_container_empty_l ie_ext_l_;

  // member variables
  bool                                    ext                                    = false;
  bool                                    receive_status_of_ul_pdcp_sdus_present = false;
  bool                                    ie_ext_present                         = false;
  count_value_for_pdcp_sn12_s             ul_count_value;
  bounded_bitstring<1, 2048, false, true> receive_status_of_ul_pdcp_sdus;
  ie_ext_l_                               ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBStatusUL18 ::= SEQUENCE
struct drb_status_ul18_s {
  typedef protocol_ext_container_empty_l ie_ext_l_;

  // member variables
  bool                                      ext                                    = false;
  bool                                      receive_status_of_ul_pdcp_sdus_present = false;
  bool                                      ie_ext_present                         = false;
  count_value_for_pdcp_sn18_s               ul_count_value;
  bounded_bitstring<1, 131072, false, true> receive_status_of_ul_pdcp_sdus;
  ie_ext_l_                                 ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBStatusUL ::= CHOICE
struct drb_status_ul_c {
  struct types_opts {
    enum options { drb_status_ul12, drb_status_ul18, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  drb_status_ul_c() = default;
  drb_status_ul_c(const drb_status_ul_c& other);
  drb_status_ul_c& operator=(const drb_status_ul_c& other);
  ~drb_status_ul_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  drb_status_ul12_s& drb_status_ul12()
  {
    assert_choice_type("dRBStatusUL12", type_.to_string(), "DRBStatusUL");
    return c.get<drb_status_ul12_s>();
  }
  drb_status_ul18_s& drb_status_ul18()
  {
    assert_choice_type("dRBStatusUL18", type_.to_string(), "DRBStatusUL");
    return c.get<drb_status_ul18_s>();
  }
  protocol_ie_single_container_s<drb_status_ul_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "DRBStatusUL");
    return c.get<protocol_ie_single_container_s<drb_status_ul_ext_ies_o> >();
  }
  const drb_status_ul12_s& drb_status_ul12() const
  {
    assert_choice_type("dRBStatusUL12", type_.to_string(), "DRBStatusUL");
    return c.get<drb_status_ul12_s>();
  }
  const drb_status_ul18_s& drb_status_ul18() const
  {
    assert_choice_type("dRBStatusUL18", type_.to_string(), "DRBStatusUL");
    return c.get<drb_status_ul18_s>();
  }
  const protocol_ie_single_container_s<drb_status_ul_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "DRBStatusUL");
    return c.get<protocol_ie_single_container_s<drb_status_ul_ext_ies_o> >();
  }
  drb_status_ul12_s& set_drb_status_ul12()
  {
    set(types::drb_status_ul12);
    return c.get<drb_status_ul12_s>();
  }
  drb_status_ul18_s& set_drb_status_ul18()
  {
    set(types::drb_status_ul18);
    return c.get<drb_status_ul18_s>();
  }
  protocol_ie_single_container_s<drb_status_ul_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<drb_status_ul_ext_ies_o> >();
  }

private:
  types                                                                                                           type_;
  choice_buffer_t<drb_status_ul12_s, drb_status_ul18_s, protocol_ie_single_container_s<drb_status_ul_ext_ies_o> > c;

  void destroy_();
};

// DRBsSubjectToStatusTransferItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o drbs_subject_to_status_transfer_item_ext_ies_o;

// DRBsSubjectToStatusTransferItem ::= SEQUENCE
struct drbs_subject_to_status_transfer_item_s {
  typedef protocol_ext_container_empty_l ie_ext_l_;

  // member variables
  bool            ext            = false;
  bool            ie_ext_present = false;
  uint8_t         drb_id         = 1;
  drb_status_ul_c drb_status_ul;
  drb_status_dl_c drb_status_dl;
  ie_ext_l_       ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBsSubjectToStatusTransferList ::= SEQUENCE (SIZE (1..32)) OF DRBsSubjectToStatusTransferItem
using drbs_subject_to_status_transfer_list_l = dyn_array<drbs_subject_to_status_transfer_item_s>;

// DRBsToQosFlowsMappingItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o drbs_to_qos_flows_map_item_ext_ies_o;

// DRBsToQosFlowsMappingItem ::= SEQUENCE
struct drbs_to_qos_flows_map_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  uint8_t                    drb_id          = 1;
  associated_qos_flow_list_l associated_qos_flow_list;
  ie_exts_l_                 ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBsToQosFlowsMappingList ::= SEQUENCE (SIZE (1..32)) OF DRBsToQosFlowsMappingItem
using drbs_to_qos_flows_map_list_l = dyn_array<drbs_to_qos_flows_map_item_s>;

// DataForwardingResponseDRBItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o data_forwarding_resp_drb_item_ext_ies_o;

// DataForwardingResponseDRBItem ::= SEQUENCE
struct data_forwarding_resp_drb_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext                              = false;
  bool                      dlforwarding_up_tnl_info_present = false;
  bool                      ulforwarding_up_tnl_info_present = false;
  bool                      ie_exts_present                  = false;
  uint8_t                   drb_id                           = 1;
  up_transport_layer_info_c dlforwarding_up_tnl_info;
  up_transport_layer_info_c ulforwarding_up_tnl_info;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataForwardingResponseDRBList ::= SEQUENCE (SIZE (1..32)) OF DataForwardingResponseDRBItem
using data_forwarding_resp_drb_list_l = dyn_array<data_forwarding_resp_drb_item_s>;

// DeactivateTraceIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct deactiv_trace_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ngran_trace_id, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    fixed_octstring<8, true>& ngran_trace_id()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<fixed_octstring<8, true> >();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const fixed_octstring<8, true>& ngran_trace_id() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<fixed_octstring<8, true> >();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    fixed_octstring<8, true>& set_ngran_trace_id()
    {
      set(types::ngran_trace_id);
      return c.get<fixed_octstring<8, true> >();
    }

  private:
    types                                      type_;
    choice_buffer_t<fixed_octstring<8, true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DeactivateTrace ::= SEQUENCE
struct deactiv_trace_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<fixed_octstring<8, true> >                         ngran_trace_id;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ForbiddenAreaInformation-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o forbidden_area_info_item_ext_ies_o;

// ForbiddenTACs ::= SEQUENCE (SIZE (1..4096)) OF OCTET STRING (SIZE (3))
using forbidden_tacs_l = dyn_array<fixed_octstring<3, true> >;

// NotAllowedTACs ::= SEQUENCE (SIZE (1..16)) OF OCTET STRING (SIZE (3))
using not_allowed_tacs_l = bounded_array<fixed_octstring<3, true>, 16>;

// RATRestrictions-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o rat_restricts_item_ext_ies_o;

// ServiceAreaInformation-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o service_area_info_item_ext_ies_o;

// ForbiddenAreaInformation-Item ::= SEQUENCE
struct forbidden_area_info_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plmn_id;
  forbidden_tacs_l         forbidden_tacs;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RATRestrictions-Item ::= SEQUENCE
struct rat_restricts_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  fixed_octstring<3, true>       plmn_id;
  fixed_bitstring<8, true, true> rat_restrict_info;
  ie_exts_l_                     ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServiceAreaInformation-Item ::= SEQUENCE
struct service_area_info_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext                      = false;
  bool                     allowed_tacs_present     = false;
  bool                     not_allowed_tacs_present = false;
  bool                     ie_exts_present          = false;
  fixed_octstring<3, true> plmn_id;
  allowed_tacs_l           allowed_tacs;
  not_allowed_tacs_l       not_allowed_tacs;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EquivalentPLMNs ::= SEQUENCE (SIZE (1..15)) OF OCTET STRING (SIZE (3))
using equivalent_plmns_l = bounded_array<fixed_octstring<3, true>, 15>;

// ForbiddenAreaInformation ::= SEQUENCE (SIZE (1..16)) OF ForbiddenAreaInformation-Item
using forbidden_area_info_l = dyn_array<forbidden_area_info_item_s>;

// MobilityRestrictionList-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct mob_restrict_list_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { last_eutran_plmn_id, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::last_eutran_plmn_id; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_octstring<3, true>&       last_eutran_plmn_id() { return c; }
    const fixed_octstring<3, true>& last_eutran_plmn_id() const { return c; }

  private:
    fixed_octstring<3, true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RATRestrictions ::= SEQUENCE (SIZE (1..16)) OF RATRestrictions-Item
using rat_restricts_l = dyn_array<rat_restricts_item_s>;

// ServiceAreaInformation ::= SEQUENCE (SIZE (1..16)) OF ServiceAreaInformation-Item
using service_area_info_l = dyn_array<service_area_info_item_s>;

// UEAggregateMaximumBitRate-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o ue_aggregate_maximum_bit_rate_ext_ies_o;

// MobilityRestrictionList ::= SEQUENCE
struct mob_restrict_list_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                                  last_eutran_plmn_id_present = false;
    ie_field_s<fixed_octstring<3, true> > last_eutran_plmn_id;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                     ext                         = false;
  bool                     equivalent_plmns_present    = false;
  bool                     rat_restricts_present       = false;
  bool                     forbidden_area_info_present = false;
  bool                     service_area_info_present   = false;
  bool                     ie_exts_present             = false;
  fixed_octstring<3, true> serving_plmn;
  equivalent_plmns_l       equivalent_plmns;
  rat_restricts_l          rat_restricts;
  forbidden_area_info_l    forbidden_area_info;
  service_area_info_l      service_area_info;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAggregateMaximumBitRate ::= SEQUENCE
struct ue_aggregate_maximum_bit_rate_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext                             = false;
  bool       ie_exts_present                 = false;
  uint64_t   ueaggregate_maximum_bit_rate_dl = 0;
  uint64_t   ueaggregate_maximum_bit_rate_ul = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkNASTransport-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct dl_nas_transport_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        old_amf,
        ran_paging_prio,
        nas_pdu,
        mob_restrict_list,
        idx_to_rfsp,
        ue_aggregate_maximum_bit_rate,
        allowed_nssai,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    printable_string<1, 150, true, true>& old_amf()
    {
      assert_choice_type("PrintableString", type_.to_string(), "Value");
      return c.get<printable_string<1, 150, true, true> >();
    }
    uint16_t& ran_paging_prio()
    {
      assert_choice_type("INTEGER (1..256)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    unbounded_octstring<true>& nas_pdu()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    mob_restrict_list_s& mob_restrict_list()
    {
      assert_choice_type("MobilityRestrictionList", type_.to_string(), "Value");
      return c.get<mob_restrict_list_s>();
    }
    uint16_t& idx_to_rfsp()
    {
      assert_choice_type("INTEGER (1..256,...)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    ue_aggregate_maximum_bit_rate_s& ue_aggregate_maximum_bit_rate()
    {
      assert_choice_type("UEAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    allowed_nssai_l& allowed_nssai()
    {
      assert_choice_type("AllowedNSSAI", type_.to_string(), "Value");
      return c.get<allowed_nssai_l>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const printable_string<1, 150, true, true>& old_amf() const
    {
      assert_choice_type("PrintableString", type_.to_string(), "Value");
      return c.get<printable_string<1, 150, true, true> >();
    }
    const uint16_t& ran_paging_prio() const
    {
      assert_choice_type("INTEGER (1..256)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    const unbounded_octstring<true>& nas_pdu() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const mob_restrict_list_s& mob_restrict_list() const
    {
      assert_choice_type("MobilityRestrictionList", type_.to_string(), "Value");
      return c.get<mob_restrict_list_s>();
    }
    const uint16_t& idx_to_rfsp() const
    {
      assert_choice_type("INTEGER (1..256,...)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    const ue_aggregate_maximum_bit_rate_s& ue_aggregate_maximum_bit_rate() const
    {
      assert_choice_type("UEAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    const allowed_nssai_l& allowed_nssai() const
    {
      assert_choice_type("AllowedNSSAI", type_.to_string(), "Value");
      return c.get<allowed_nssai_l>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    printable_string<1, 150, true, true>& set_old_amf()
    {
      set(types::old_amf);
      return c.get<printable_string<1, 150, true, true> >();
    }
    uint16_t& set_ran_paging_prio()
    {
      set(types::ran_paging_prio);
      return c.get<uint16_t>();
    }
    unbounded_octstring<true>& set_nas_pdu()
    {
      set(types::nas_pdu);
      return c.get<unbounded_octstring<true> >();
    }
    mob_restrict_list_s& set_mob_restrict_list()
    {
      set(types::mob_restrict_list);
      return c.get<mob_restrict_list_s>();
    }
    uint16_t& set_idx_to_rfsp()
    {
      set(types::idx_to_rfsp);
      return c.get<uint16_t>();
    }
    ue_aggregate_maximum_bit_rate_s& set_ue_aggregate_maximum_bit_rate()
    {
      set(types::ue_aggregate_maximum_bit_rate);
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    allowed_nssai_l& set_allowed_nssai()
    {
      set(types::allowed_nssai);
      return c.get<allowed_nssai_l>();
    }

  private:
    types type_;
    choice_buffer_t<allowed_nssai_l,
                    mob_restrict_list_s,
                    printable_string<1, 150, true, true>,
                    ue_aggregate_maximum_bit_rate_s,
                    unbounded_octstring<true> >
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DownlinkNASTransport ::= SEQUENCE
struct dl_nas_transport_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          old_amf_present                       = false;
    bool                                                          ran_paging_prio_present               = false;
    bool                                                          mob_restrict_list_present             = false;
    bool                                                          idx_to_rfsp_present                   = false;
    bool                                                          ue_aggregate_maximum_bit_rate_present = false;
    bool                                                          allowed_nssai_present                 = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<printable_string<1, 150, true, true> >             old_amf;
    ie_field_s<integer<uint16_t, 1, 256, false, true> >           ran_paging_prio;
    ie_field_s<unbounded_octstring<true> >                        nas_pdu;
    ie_field_s<mob_restrict_list_s>                               mob_restrict_list;
    ie_field_s<integer<uint16_t, 1, 256, false, true> >           idx_to_rfsp;
    ie_field_s<ue_aggregate_maximum_bit_rate_s>                   ue_aggregate_maximum_bit_rate;
    ie_field_s<dyn_seq_of<allowed_nssai_item_s, 1, 8, true> >     allowed_nssai;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkNonUEAssociatedNRPPaTransportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct dl_non_ueassociated_nrp_pa_transport_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { routing_id, nrp_pa_pdu, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>& routing_id()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    unbounded_octstring<true>& nrp_pa_pdu()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const unbounded_octstring<true>& routing_id() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const unbounded_octstring<true>& nrp_pa_pdu() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    unbounded_octstring<true>& set_routing_id()
    {
      set(types::routing_id);
      return c.get<unbounded_octstring<true> >();
    }
    unbounded_octstring<true>& set_nrp_pa_pdu()
    {
      set(types::nrp_pa_pdu);
      return c.get<unbounded_octstring<true> >();
    }

  private:
    types                                       type_;
    choice_buffer_t<unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DownlinkNonUEAssociatedNRPPaTransport ::= SEQUENCE
struct dl_non_ueassociated_nrp_pa_transport_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<unbounded_octstring<true> > routing_id;
    ie_field_s<unbounded_octstring<true> > nrp_pa_pdu;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// XnExtTLA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o xn_ext_tla_item_ext_ies_o;

// XnGTP-TLAs ::= SEQUENCE (SIZE (1..16)) OF BIT STRING (SIZE (1..160,...))
using xn_gtp_tlas_l = bounded_array<bounded_bitstring<1, 160, true, true>, 16>;

// XnExtTLA-Item ::= SEQUENCE
struct xn_ext_tla_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                  ext               = false;
  bool                                  ipsec_tla_present = false;
  bool                                  gtp_tlas_present  = false;
  bool                                  ie_exts_present   = false;
  bounded_bitstring<1, 160, true, true> ipsec_tla;
  xn_gtp_tlas_l                         gtp_tlas;
  ie_exts_l_                            ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// XnExtTLAs ::= SEQUENCE (SIZE (1..2)) OF XnExtTLA-Item
using xn_ext_tlas_l = dyn_array<xn_ext_tla_item_s>;

// XnTLAs ::= SEQUENCE (SIZE (1..16)) OF BIT STRING (SIZE (1..160,...))
using xn_tlas_l = bounded_array<bounded_bitstring<1, 160, true, true>, 16>;

// XnTNLConfigurationInfo-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o xn_tnl_cfg_info_ext_ies_o;

// SONInformationReply-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o son_info_reply_ext_ies_o;

// XnTNLConfigurationInfo ::= SEQUENCE
struct xn_tnl_cfg_info_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool          ext                                           = false;
  bool          xn_extended_transport_layer_addresses_present = false;
  bool          ie_exts_present                               = false;
  xn_tlas_l     xn_transport_layer_addresses;
  xn_ext_tlas_l xn_extended_transport_layer_addresses;
  ie_exts_l_    ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SONInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o son_info_ext_ies_o;

// SONInformationReply ::= SEQUENCE
struct son_info_reply_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool              ext                     = false;
  bool              xn_tnl_cfg_info_present = false;
  bool              ie_exts_present         = false;
  xn_tnl_cfg_info_s xn_tnl_cfg_info;
  ie_exts_l_        ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SONInformationRequest ::= ENUMERATED
struct son_info_request_opts {
  enum options { xn_tnl_cfg_info, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<son_info_request_opts, true> son_info_request_e;

// SourceRANNodeID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o source_ran_node_id_ext_ies_o;

// TargetRANNodeID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o target_ran_node_id_ext_ies_o;

// SONConfigurationTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o son_cfg_transfer_ext_ies_o;

// SONInformation ::= CHOICE
struct son_info_c {
  struct types_opts {
    enum options { son_info_request, son_info_reply, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  son_info_c() = default;
  son_info_c(const son_info_c& other);
  son_info_c& operator=(const son_info_c& other);
  ~son_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  son_info_request_e& son_info_request()
  {
    assert_choice_type("sONInformationRequest", type_.to_string(), "SONInformation");
    return c.get<son_info_request_e>();
  }
  son_info_reply_s& son_info_reply()
  {
    assert_choice_type("sONInformationReply", type_.to_string(), "SONInformation");
    return c.get<son_info_reply_s>();
  }
  protocol_ie_single_container_s<son_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "SONInformation");
    return c.get<protocol_ie_single_container_s<son_info_ext_ies_o> >();
  }
  const son_info_request_e& son_info_request() const
  {
    assert_choice_type("sONInformationRequest", type_.to_string(), "SONInformation");
    return c.get<son_info_request_e>();
  }
  const son_info_reply_s& son_info_reply() const
  {
    assert_choice_type("sONInformationReply", type_.to_string(), "SONInformation");
    return c.get<son_info_reply_s>();
  }
  const protocol_ie_single_container_s<son_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "SONInformation");
    return c.get<protocol_ie_single_container_s<son_info_ext_ies_o> >();
  }
  son_info_request_e& set_son_info_request()
  {
    set(types::son_info_request);
    return c.get<son_info_request_e>();
  }
  son_info_reply_s& set_son_info_reply()
  {
    set(types::son_info_reply);
    return c.get<son_info_reply_s>();
  }
  protocol_ie_single_container_s<son_info_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<son_info_ext_ies_o> >();
  }

private:
  types                                                                                 type_;
  choice_buffer_t<protocol_ie_single_container_s<son_info_ext_ies_o>, son_info_reply_s> c;

  void destroy_();
};

// SourceRANNodeID ::= SEQUENCE
struct source_ran_node_id_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                 ext             = false;
  bool                 ie_exts_present = false;
  global_ran_node_id_c global_ran_node_id;
  tai_s                sel_tai;
  ie_exts_l_           ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargetRANNodeID ::= SEQUENCE
struct target_ran_node_id_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                 ext             = false;
  bool                 ie_exts_present = false;
  global_ran_node_id_c global_ran_node_id;
  tai_s                sel_tai;
  ie_exts_l_           ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SONConfigurationTransfer ::= SEQUENCE
struct son_cfg_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                 ext                     = false;
  bool                 xn_tnl_cfg_info_present = false;
  bool                 ie_exts_present         = false;
  target_ran_node_id_s target_ran_node_id;
  source_ran_node_id_s source_ran_node_id;
  son_info_c           son_info;
  xn_tnl_cfg_info_s    xn_tnl_cfg_info;
  ie_exts_l_           ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkRANConfigurationTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct dl_ran_cfg_transfer_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { son_cfg_transfer_dl, endc_son_cfg_transfer_dl, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    son_cfg_transfer_s& son_cfg_transfer_dl()
    {
      assert_choice_type("SONConfigurationTransfer", type_.to_string(), "Value");
      return c.get<son_cfg_transfer_s>();
    }
    unbounded_octstring<true>& endc_son_cfg_transfer_dl()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const son_cfg_transfer_s& son_cfg_transfer_dl() const
    {
      assert_choice_type("SONConfigurationTransfer", type_.to_string(), "Value");
      return c.get<son_cfg_transfer_s>();
    }
    const unbounded_octstring<true>& endc_son_cfg_transfer_dl() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    son_cfg_transfer_s& set_son_cfg_transfer_dl()
    {
      set(types::son_cfg_transfer_dl);
      return c.get<son_cfg_transfer_s>();
    }
    unbounded_octstring<true>& set_endc_son_cfg_transfer_dl()
    {
      set(types::endc_son_cfg_transfer_dl);
      return c.get<unbounded_octstring<true> >();
    }

  private:
    types                                                           type_;
    choice_buffer_t<son_cfg_transfer_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DownlinkRANConfigurationTransfer ::= SEQUENCE
struct dl_ran_cfg_transfer_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                   son_cfg_transfer_dl_present      = false;
    bool                                   endc_son_cfg_transfer_dl_present = false;
    ie_field_s<son_cfg_transfer_s>         son_cfg_transfer_dl;
    ie_field_s<unbounded_octstring<true> > endc_son_cfg_transfer_dl;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANStatusTransfer-TransparentContainer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o ran_status_transfer_transparent_container_ext_ies_o;

// RANStatusTransfer-TransparentContainer ::= SEQUENCE
struct ran_status_transfer_transparent_container_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                   ext             = false;
  bool                                   ie_exts_present = false;
  drbs_subject_to_status_transfer_list_l drbs_subject_to_status_transfer_list;
  ie_exts_l_                             ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkRANStatusTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct dl_ran_status_transfer_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ran_status_transfer_transparent_container, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    ran_status_transfer_transparent_container_s& ran_status_transfer_transparent_container()
    {
      assert_choice_type("RANStatusTransfer-TransparentContainer", type_.to_string(), "Value");
      return c.get<ran_status_transfer_transparent_container_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const ran_status_transfer_transparent_container_s& ran_status_transfer_transparent_container() const
    {
      assert_choice_type("RANStatusTransfer-TransparentContainer", type_.to_string(), "Value");
      return c.get<ran_status_transfer_transparent_container_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    ran_status_transfer_transparent_container_s& set_ran_status_transfer_transparent_container()
    {
      set(types::ran_status_transfer_transparent_container);
      return c.get<ran_status_transfer_transparent_container_s>();
    }

  private:
    types                                                        type_;
    choice_buffer_t<ran_status_transfer_transparent_container_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DownlinkRANStatusTransfer ::= SEQUENCE
struct dl_ran_status_transfer_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<ran_status_transfer_transparent_container_s>       ran_status_transfer_transparent_container;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkUEAssociatedNRPPaTransportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct dl_ueassociated_nrp_pa_transport_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, routing_id, nrp_pa_pdu, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& routing_id()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    unbounded_octstring<true>& nrp_pa_pdu()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const unbounded_octstring<true>& routing_id() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const unbounded_octstring<true>& nrp_pa_pdu() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& set_routing_id()
    {
      set(types::routing_id);
      return c.get<unbounded_octstring<true> >();
    }
    unbounded_octstring<true>& set_nrp_pa_pdu()
    {
      set(types::nrp_pa_pdu);
      return c.get<unbounded_octstring<true> >();
    }

  private:
    types                                       type_;
    choice_buffer_t<unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DownlinkUEAssociatedNRPPaTransport ::= SEQUENCE
struct dl_ueassociated_nrp_pa_transport_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<unbounded_octstring<true> >                        routing_id;
    ie_field_s<unbounded_octstring<true> >                        nrp_pa_pdu;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PacketErrorRate-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o packet_error_rate_ext_ies_o;

// DelayCritical ::= ENUMERATED
struct delay_crit_opts {
  enum options { delay_crit, non_delay_crit, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<delay_crit_opts, true> delay_crit_e;

// Dynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o dynamic5_qi_descriptor_ext_ies_o;

// PacketErrorRate ::= SEQUENCE
struct packet_error_rate_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  uint8_t    per_scalar      = 0;
  uint8_t    per_exponent    = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Dynamic5QIDescriptor ::= SEQUENCE
struct dynamic5_qi_descriptor_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                ext                               = false;
  bool                five_qi_present                   = false;
  bool                delay_crit_present                = false;
  bool                averaging_win_present             = false;
  bool                maximum_data_burst_volume_present = false;
  bool                ie_exts_present                   = false;
  uint8_t             prio_level_qos                    = 1;
  uint16_t            packet_delay_budget               = 0;
  packet_error_rate_s packet_error_rate;
  uint16_t            five_qi = 0;
  delay_crit_e        delay_crit;
  uint16_t            averaging_win             = 0;
  uint16_t            maximum_data_burst_volume = 0;
  ie_exts_l_          ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLForwarding ::= ENUMERATED
struct dl_forwarding_opts {
  enum options { dl_forwarding_proposed, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<dl_forwarding_opts, true> dl_forwarding_e;

// E-RABInformationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o e_rab_info_item_ext_ies_o;

// E-RABInformationItem ::= SEQUENCE
struct e_rab_info_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool            ext                  = false;
  bool            dlforwarding_present = false;
  bool            ie_exts_present      = false;
  uint8_t         e_rab_id             = 0;
  dl_forwarding_e dlforwarding;
  ie_exts_l_      ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABInformationList ::= SEQUENCE (SIZE (1..256)) OF E-RABInformationItem
using e_rab_info_list_l = dyn_array<e_rab_info_item_s>;

// EPS-TAI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o eps_tai_ext_ies_o;

// EPS-TAI ::= SEQUENCE
struct eps_tai_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plmn_id;
  fixed_octstring<2, true> eps_tac;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-CGIListForWarning ::= SEQUENCE (SIZE (1..65535)) OF EUTRA-CGI
using eutra_cgi_list_for_warning_l = dyn_array<eutra_cgi_s>;

// EmergencyAreaIDList ::= SEQUENCE (SIZE (1..65535)) OF OCTET STRING (SIZE (3))
using emergency_area_id_list_l = dyn_array<fixed_octstring<3, true> >;

// EmergencyAreaIDListForRestart ::= SEQUENCE (SIZE (1..256)) OF OCTET STRING (SIZE (3))
using emergency_area_id_list_for_restart_l = dyn_array<fixed_octstring<3, true> >;

// EmergencyFallbackIndicator-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o emergency_fallback_ind_ext_ies_o;

// EmergencyFallbackRequestIndicator ::= ENUMERATED
struct emergency_fallback_request_ind_opts {
  enum options { emergency_fallback_requested, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<emergency_fallback_request_ind_opts, true> emergency_fallback_request_ind_e;

// EmergencyServiceTargetCN ::= ENUMERATED
struct emergency_service_target_cn_opts {
  enum options { five_gc, epc, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<emergency_service_target_cn_opts, true> emergency_service_target_cn_e;

// EmergencyFallbackIndicator ::= SEQUENCE
struct emergency_fallback_ind_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                             ext                                 = false;
  bool                             emergency_service_target_cn_present = false;
  bool                             ie_exts_present                     = false;
  emergency_fallback_request_ind_e emergency_fallback_request_ind;
  emergency_service_target_cn_e    emergency_service_target_cn;
  ie_exts_l_                       ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ErrorIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct error_ind_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, cause, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ErrorIndication ::= SEQUENCE
struct error_ind_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          amf_ue_ngap_id_present   = false;
    bool                                                          ran_ue_ngap_id_present   = false;
    bool                                                          cause_present            = false;
    bool                                                          crit_diagnostics_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<cause_c>                                           cause;
    ie_field_s<crit_diagnostics_s>                                crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FiveG-S-TMSI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o five_g_s_tmsi_ext_ies_o;

// FiveG-S-TMSI ::= SEQUENCE
struct five_g_s_tmsi_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_bitstring<10, false, true> amf_set_id;
  fixed_bitstring<6, false, true>  amf_pointer;
  fixed_octstring<4, true>         five_g_tmsi;
  ie_exts_l_                       ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GBR-QosInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o gbr_qos_info_ext_ies_o;

// NotificationControl ::= ENUMERATED
struct notif_ctrl_opts {
  enum options { notif_requested, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<notif_ctrl_opts, true> notif_ctrl_e;

// GBR-QosInformation ::= SEQUENCE
struct gbr_qos_info_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool         ext                                 = false;
  bool         notif_ctrl_present                  = false;
  bool         maximum_packet_loss_rate_dl_present = false;
  bool         maximum_packet_loss_rate_ul_present = false;
  bool         ie_exts_present                     = false;
  uint64_t     maximum_flow_bit_rate_dl            = 0;
  uint64_t     maximum_flow_bit_rate_ul            = 0;
  uint64_t     guaranteed_flow_bit_rate_dl         = 0;
  uint64_t     guaranteed_flow_bit_rate_ul         = 0;
  notif_ctrl_e notif_ctrl;
  uint16_t     maximum_packet_loss_rate_dl = 0;
  uint16_t     maximum_packet_loss_rate_ul = 0;
  ie_exts_l_   ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCancelIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_cancel_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, cause, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }

  private:
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// HandoverCancel ::= SEQUENCE
struct ho_cancel_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<cause_c>                                           cause;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCancelAcknowledgeIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_cancel_ack_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// HandoverCancelAcknowledge ::= SEQUENCE
struct ho_cancel_ack_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          crit_diagnostics_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<crit_diagnostics_s>                                crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowPerTNLInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_per_tnl_info_ext_ies_o;

// QosFlowPerTNLInformation ::= SEQUENCE
struct qos_flow_per_tnl_info_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  up_transport_layer_info_c  uptransport_layer_info;
  associated_qos_flow_list_l associated_qos_flow_list;
  ie_exts_l_                 ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowPerTNLInformationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_per_tnl_info_item_ext_ies_o;

// QosFlowPerTNLInformationItem ::= SEQUENCE
struct qos_flow_per_tnl_info_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                    ext             = false;
  bool                    ie_exts_present = false;
  qos_flow_per_tnl_info_s qos_flow_per_tnl_info;
  ie_exts_l_              ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowToBeForwardedItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_to_be_forwarded_item_ext_ies_o;

// QosFlowPerTNLInformationList ::= SEQUENCE (SIZE (1..3)) OF QosFlowPerTNLInformationItem
using qos_flow_per_tnl_info_list_l = dyn_array<qos_flow_per_tnl_info_item_s>;

// QosFlowToBeForwardedItem ::= SEQUENCE
struct qos_flow_to_be_forwarded_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  uint8_t    qos_flow_id     = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCommandTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct ho_cmd_transfer_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { add_dl_forwarding_uptnl_info, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::add_dl_forwarding_uptnl_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_per_tnl_info_list_l&       add_dl_forwarding_uptnl_info() { return c; }
    const qos_flow_per_tnl_info_list_l& add_dl_forwarding_uptnl_info() const { return c; }

  private:
    qos_flow_per_tnl_info_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// HandoverPreparationUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o ho_prep_unsuccessful_transfer_ext_ies_o;

// QosFlowToBeForwardedList ::= SEQUENCE (SIZE (1..64)) OF QosFlowToBeForwardedItem
using qos_flow_to_be_forwarded_list_l = dyn_array<qos_flow_to_be_forwarded_item_s>;

// HandoverCommandTransfer ::= SEQUENCE
struct ho_cmd_transfer_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                                                              add_dl_forwarding_uptnl_info_present = false;
    ie_field_s<dyn_seq_of<qos_flow_per_tnl_info_item_s, 1, 3, true> > add_dl_forwarding_uptnl_info;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                            ext                                   = false;
  bool                            dlforwarding_up_tnl_info_present      = false;
  bool                            qos_flow_to_be_forwarded_list_present = false;
  bool                            data_forwarding_resp_drb_list_present = false;
  bool                            ie_exts_present                       = false;
  up_transport_layer_info_c       dlforwarding_up_tnl_info;
  qos_flow_to_be_forwarded_list_l qos_flow_to_be_forwarded_list;
  data_forwarding_resp_drb_list_l data_forwarding_resp_drb_list;
  ie_exts_l_                      ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationUnsuccessfulTransfer ::= SEQUENCE
struct ho_prep_unsuccessful_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  cause_c    cause;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceHandoverItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_ho_item_ext_ies_o;

// PDUSessionResourceToReleaseItemHOCmd-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_to_release_item_ho_cmd_ext_ies_o;

// PDUSessionResourceHandoverItem ::= SEQUENCE
struct pdu_session_res_ho_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> ho_cmd_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceToReleaseItemHOCmd ::= SEQUENCE
struct pdu_session_res_to_release_item_ho_cmd_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> ho_prep_unsuccessful_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverType ::= ENUMERATED
struct ho_type_opts {
  enum options { intra5gs, fivegs_to_eps, eps_to_minus5gs, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ho_type_opts, true> ho_type_e;

// PDUSessionResourceHandoverList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceHandoverItem
using pdu_session_res_ho_list_l = dyn_array<pdu_session_res_ho_item_s>;

// PDUSessionResourceToReleaseListHOCmd ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceToReleaseItemHOCmd
using pdu_session_res_to_release_list_ho_cmd_l = dyn_array<pdu_session_res_to_release_item_ho_cmd_s>;

// HandoverCommandIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_cmd_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ho_type,
        nas_security_params_from_ngran,
        pdu_session_res_ho_list,
        pdu_session_res_to_release_list_ho_cmd,
        target_to_source_transparent_container,
        crit_diagnostics,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    ho_type_e& ho_type()
    {
      assert_choice_type("HandoverType", type_.to_string(), "Value");
      return c.get<ho_type_e>();
    }
    unbounded_octstring<true>& nas_security_params_from_ngran()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    pdu_session_res_ho_list_l& pdu_session_res_ho_list()
    {
      assert_choice_type("PDUSessionResourceHandoverList", type_.to_string(), "Value");
      return c.get<pdu_session_res_ho_list_l>();
    }
    pdu_session_res_to_release_list_ho_cmd_l& pdu_session_res_to_release_list_ho_cmd()
    {
      assert_choice_type("PDUSessionResourceToReleaseListHOCmd", type_.to_string(), "Value");
      return c.get<pdu_session_res_to_release_list_ho_cmd_l>();
    }
    unbounded_octstring<true>& target_to_source_transparent_container()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const ho_type_e& ho_type() const
    {
      assert_choice_type("HandoverType", type_.to_string(), "Value");
      return c.get<ho_type_e>();
    }
    const unbounded_octstring<true>& nas_security_params_from_ngran() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const pdu_session_res_ho_list_l& pdu_session_res_ho_list() const
    {
      assert_choice_type("PDUSessionResourceHandoverList", type_.to_string(), "Value");
      return c.get<pdu_session_res_ho_list_l>();
    }
    const pdu_session_res_to_release_list_ho_cmd_l& pdu_session_res_to_release_list_ho_cmd() const
    {
      assert_choice_type("PDUSessionResourceToReleaseListHOCmd", type_.to_string(), "Value");
      return c.get<pdu_session_res_to_release_list_ho_cmd_l>();
    }
    const unbounded_octstring<true>& target_to_source_transparent_container() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    ho_type_e& set_ho_type()
    {
      set(types::ho_type);
      return c.get<ho_type_e>();
    }
    unbounded_octstring<true>& set_nas_security_params_from_ngran()
    {
      set(types::nas_security_params_from_ngran);
      return c.get<unbounded_octstring<true> >();
    }
    pdu_session_res_ho_list_l& set_pdu_session_res_ho_list()
    {
      set(types::pdu_session_res_ho_list);
      return c.get<pdu_session_res_ho_list_l>();
    }
    pdu_session_res_to_release_list_ho_cmd_l& set_pdu_session_res_to_release_list_ho_cmd()
    {
      set(types::pdu_session_res_to_release_list_ho_cmd);
      return c.get<pdu_session_res_to_release_list_ho_cmd_l>();
    }
    unbounded_octstring<true>& set_target_to_source_transparent_container()
    {
      set(types::target_to_source_transparent_container);
      return c.get<unbounded_octstring<true> >();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    pdu_session_res_ho_list_l,
                    pdu_session_res_to_release_list_ho_cmd_l,
                    unbounded_octstring<true> >
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// HandoverCommand ::= SEQUENCE
struct ho_cmd_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool nas_security_params_from_ngran_present         = false;
    bool pdu_session_res_to_release_list_ho_cmd_present = false;
    bool crit_diagnostics_present                       = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> >    amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >       ran_ue_ngap_id;
    ie_field_s<ho_type_e>                                            ho_type;
    ie_field_s<unbounded_octstring<true> >                           nas_security_params_from_ngran;
    ie_field_s<dyn_seq_of<pdu_session_res_ho_item_s, 1, 256, true> > pdu_session_res_ho_list;
    ie_field_s<dyn_seq_of<pdu_session_res_to_release_item_ho_cmd_s, 1, 256, true> >
                                           pdu_session_res_to_release_list_ho_cmd;
    ie_field_s<unbounded_octstring<true> > target_to_source_transparent_container;
    ie_field_s<crit_diagnostics_s>         crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_fail_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, cause, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// HandoverFailure ::= SEQUENCE
struct ho_fail_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          crit_diagnostics_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<cause_c>                                           cause;
    ie_field_s<crit_diagnostics_s>                                crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UserLocationInformationEUTRA-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o user_location_info_eutra_ext_ies_o;

// UserLocationInformationN3IWF-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o user_location_info_n3_iwf_ext_ies_o;

// UserLocationInformationNR-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o user_location_info_nr_ext_ies_o;

// UserLocationInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o user_location_info_ext_ies_o;

// UserLocationInformationEUTRA ::= SEQUENCE
struct user_location_info_eutra_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext                = false;
  bool                     time_stamp_present = false;
  bool                     ie_exts_present    = false;
  eutra_cgi_s              eutra_cgi;
  tai_s                    tai;
  fixed_octstring<4, true> time_stamp;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UserLocationInformationN3IWF ::= SEQUENCE
struct user_location_info_n3_iwf_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  bounded_bitstring<1, 160, true, true> ipaddress;
  fixed_octstring<2, true>              port_num;
  ie_exts_l_                            ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UserLocationInformationNR ::= SEQUENCE
struct user_location_info_nr_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext                = false;
  bool                     time_stamp_present = false;
  bool                     ie_exts_present    = false;
  nr_cgi_s                 nr_cgi;
  tai_s                    tai;
  fixed_octstring<4, true> time_stamp;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UserLocationInformation ::= CHOICE
struct user_location_info_c {
  struct types_opts {
    enum options {
      user_location_info_eutra,
      user_location_info_nr,
      user_location_info_n3_iwf,
      choice_exts,
      nulltype
    } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  user_location_info_c() = default;
  user_location_info_c(const user_location_info_c& other);
  user_location_info_c& operator=(const user_location_info_c& other);
  ~user_location_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  user_location_info_eutra_s& user_location_info_eutra()
  {
    assert_choice_type("userLocationInformationEUTRA", type_.to_string(), "UserLocationInformation");
    return c.get<user_location_info_eutra_s>();
  }
  user_location_info_nr_s& user_location_info_nr()
  {
    assert_choice_type("userLocationInformationNR", type_.to_string(), "UserLocationInformation");
    return c.get<user_location_info_nr_s>();
  }
  user_location_info_n3_iwf_s& user_location_info_n3_iwf()
  {
    assert_choice_type("userLocationInformationN3IWF", type_.to_string(), "UserLocationInformation");
    return c.get<user_location_info_n3_iwf_s>();
  }
  protocol_ie_single_container_s<user_location_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "UserLocationInformation");
    return c.get<protocol_ie_single_container_s<user_location_info_ext_ies_o> >();
  }
  const user_location_info_eutra_s& user_location_info_eutra() const
  {
    assert_choice_type("userLocationInformationEUTRA", type_.to_string(), "UserLocationInformation");
    return c.get<user_location_info_eutra_s>();
  }
  const user_location_info_nr_s& user_location_info_nr() const
  {
    assert_choice_type("userLocationInformationNR", type_.to_string(), "UserLocationInformation");
    return c.get<user_location_info_nr_s>();
  }
  const user_location_info_n3_iwf_s& user_location_info_n3_iwf() const
  {
    assert_choice_type("userLocationInformationN3IWF", type_.to_string(), "UserLocationInformation");
    return c.get<user_location_info_n3_iwf_s>();
  }
  const protocol_ie_single_container_s<user_location_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "UserLocationInformation");
    return c.get<protocol_ie_single_container_s<user_location_info_ext_ies_o> >();
  }
  user_location_info_eutra_s& set_user_location_info_eutra()
  {
    set(types::user_location_info_eutra);
    return c.get<user_location_info_eutra_s>();
  }
  user_location_info_nr_s& set_user_location_info_nr()
  {
    set(types::user_location_info_nr);
    return c.get<user_location_info_nr_s>();
  }
  user_location_info_n3_iwf_s& set_user_location_info_n3_iwf()
  {
    set(types::user_location_info_n3_iwf);
    return c.get<user_location_info_n3_iwf_s>();
  }
  protocol_ie_single_container_s<user_location_info_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<user_location_info_ext_ies_o> >();
  }

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<user_location_info_ext_ies_o>,
                  user_location_info_eutra_s,
                  user_location_info_n3_iwf_s,
                  user_location_info_nr_s>
      c;

  void destroy_();
};

// HandoverNotifyIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_notify_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, user_location_info, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    user_location_info_c& user_location_info()
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const user_location_info_c& user_location_info() const
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    user_location_info_c& set_user_location_info()
    {
      set(types::user_location_info);
      return c.get<user_location_info_c>();
    }

  private:
    types                                 type_;
    choice_buffer_t<user_location_info_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// HandoverNotify ::= SEQUENCE
struct ho_notify_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<user_location_info_c>                              user_location_info;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_prep_fail_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, cause, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// HandoverPreparationFailure ::= SEQUENCE
struct ho_prep_fail_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          crit_diagnostics_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<cause_c>                                           cause;
    ie_field_s<crit_diagnostics_s>                                crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NonDynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o non_dynamic5_qi_descriptor_ext_ies_o;

// NonDynamic5QIDescriptor ::= SEQUENCE
struct non_dynamic5_qi_descriptor_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext                               = false;
  bool       prio_level_qos_present            = false;
  bool       averaging_win_present             = false;
  bool       maximum_data_burst_volume_present = false;
  bool       ie_exts_present                   = false;
  uint16_t   five_qi                           = 0;
  uint8_t    prio_level_qos                    = 1;
  uint16_t   averaging_win                     = 0;
  uint16_t   maximum_data_burst_volume         = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosCharacteristics-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o qos_characteristics_ext_ies_o;

// AdditionalQosFlowInformation ::= ENUMERATED
struct add_qos_flow_info_opts {
  enum options { more_likely, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<add_qos_flow_info_opts, true> add_qos_flow_info_e;

// QosCharacteristics ::= CHOICE
struct qos_characteristics_c {
  struct types_opts {
    enum options { non_dynamic5_qi, dynamic5_qi, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  qos_characteristics_c() = default;
  qos_characteristics_c(const qos_characteristics_c& other);
  qos_characteristics_c& operator=(const qos_characteristics_c& other);
  ~qos_characteristics_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  non_dynamic5_qi_descriptor_s& non_dynamic5_qi()
  {
    assert_choice_type("nonDynamic5QI", type_.to_string(), "QosCharacteristics");
    return c.get<non_dynamic5_qi_descriptor_s>();
  }
  dynamic5_qi_descriptor_s& dynamic5_qi()
  {
    assert_choice_type("dynamic5QI", type_.to_string(), "QosCharacteristics");
    return c.get<dynamic5_qi_descriptor_s>();
  }
  protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "QosCharacteristics");
    return c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o> >();
  }
  const non_dynamic5_qi_descriptor_s& non_dynamic5_qi() const
  {
    assert_choice_type("nonDynamic5QI", type_.to_string(), "QosCharacteristics");
    return c.get<non_dynamic5_qi_descriptor_s>();
  }
  const dynamic5_qi_descriptor_s& dynamic5_qi() const
  {
    assert_choice_type("dynamic5QI", type_.to_string(), "QosCharacteristics");
    return c.get<dynamic5_qi_descriptor_s>();
  }
  const protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "QosCharacteristics");
    return c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o> >();
  }
  non_dynamic5_qi_descriptor_s& set_non_dynamic5_qi()
  {
    set(types::non_dynamic5_qi);
    return c.get<non_dynamic5_qi_descriptor_s>();
  }
  dynamic5_qi_descriptor_s& set_dynamic5_qi()
  {
    set(types::dynamic5_qi);
    return c.get<dynamic5_qi_descriptor_s>();
  }
  protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o> >();
  }

private:
  types type_;
  choice_buffer_t<dynamic5_qi_descriptor_s,
                  non_dynamic5_qi_descriptor_s,
                  protocol_ie_single_container_s<qos_characteristics_ext_ies_o> >
      c;

  void destroy_();
};

// QosFlowLevelQosParameters-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_level_qos_params_ext_ies_o;

// ReflectiveQosAttribute ::= ENUMERATED
struct reflective_qos_attribute_opts {
  enum options { subject_to, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<reflective_qos_attribute_opts, true> reflective_qos_attribute_e;

// MaximumIntegrityProtectedDataRate ::= ENUMERATED
struct maximum_integrity_protected_data_rate_opts {
  enum options { bitrate64kbs, maximum_ue_rate, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<maximum_integrity_protected_data_rate_opts, true> maximum_integrity_protected_data_rate_e;

// QosFlowLevelQosParameters ::= SEQUENCE
struct qos_flow_level_qos_params_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                       ext                              = false;
  bool                       gbr_qos_info_present             = false;
  bool                       reflective_qos_attribute_present = false;
  bool                       add_qos_flow_info_present        = false;
  bool                       ie_exts_present                  = false;
  qos_characteristics_c      qos_characteristics;
  alloc_and_retention_prio_s alloc_and_retention_prio;
  gbr_qos_info_s             gbr_qos_info;
  reflective_qos_attribute_e reflective_qos_attribute;
  add_qos_flow_info_e        add_qos_flow_info;
  ie_exts_l_                 ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowSetupRequestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_setup_request_item_ext_ies_o;

// UPTransportLayerInformationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o up_transport_layer_info_item_ext_ies_o;

// ConfidentialityProtectionIndication ::= ENUMERATED
struct confidentiality_protection_ind_opts {
  enum options { required, preferred, not_needed, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<confidentiality_protection_ind_opts, true> confidentiality_protection_ind_e;

// IntegrityProtectionIndication ::= ENUMERATED
struct integrity_protection_ind_opts {
  enum options { required, preferred, not_needed, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<integrity_protection_ind_opts, true> integrity_protection_ind_e;

// PDUSessionAggregateMaximumBitRate-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_aggregate_maximum_bit_rate_ext_ies_o;

// QosFlowSetupRequestItem ::= SEQUENCE
struct qos_flow_setup_request_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                        ext              = false;
  bool                        e_rab_id_present = false;
  bool                        ie_exts_present  = false;
  uint8_t                     qos_flow_id      = 0;
  qos_flow_level_qos_params_s qos_flow_level_qos_params;
  uint8_t                     e_rab_id = 0;
  ie_exts_l_                  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityIndication-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct security_ind_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { maximum_integrity_protected_data_rate_dl, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::maximum_integrity_protected_data_rate_dl; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    maximum_integrity_protected_data_rate_e&       maximum_integrity_protected_data_rate_dl() { return c; }
    const maximum_integrity_protected_data_rate_e& maximum_integrity_protected_data_rate_dl() const { return c; }

  private:
    maximum_integrity_protected_data_rate_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UPTransportLayerInformationItem ::= SEQUENCE
struct up_transport_layer_info_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  up_transport_layer_info_c ngu_up_tnl_info;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataForwardingNotPossible ::= ENUMERATED
struct data_forwarding_not_possible_opts {
  enum options { data_forwarding_not_possible, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<data_forwarding_not_possible_opts, true> data_forwarding_not_possible_e;

// PDUSessionAggregateMaximumBitRate ::= SEQUENCE
struct pdu_session_aggregate_maximum_bit_rate_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext                                       = false;
  bool       ie_exts_present                           = false;
  uint64_t   pdu_session_aggregate_maximum_bit_rate_dl = 0;
  uint64_t   pdu_session_aggregate_maximum_bit_rate_ul = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionType ::= ENUMERATED
struct pdu_session_type_opts {
  enum options { ipv4, ipv6, ipv4v6, ethernet, unstructured, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<pdu_session_type_opts, true> pdu_session_type_e;

// QosFlowSetupRequestList ::= SEQUENCE (SIZE (1..64)) OF QosFlowSetupRequestItem
using qos_flow_setup_request_list_l = dyn_array<qos_flow_setup_request_item_s>;

// SecurityIndication ::= SEQUENCE
struct security_ind_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                                                maximum_integrity_protected_data_rate_dl_present = false;
    ie_field_s<maximum_integrity_protected_data_rate_e> maximum_integrity_protected_data_rate_dl;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                                    ext                                              = false;
  bool                                    maximum_integrity_protected_data_rate_ul_present = false;
  bool                                    ie_exts_present                                  = false;
  integrity_protection_ind_e              integrity_protection_ind;
  confidentiality_protection_ind_e        confidentiality_protection_ind;
  maximum_integrity_protected_data_rate_e maximum_integrity_protected_data_rate_ul;
  ie_exts_l_                              ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UPTransportLayerInformationList ::= SEQUENCE (SIZE (1..3)) OF UPTransportLayerInformationItem
using up_transport_layer_info_list_l = dyn_array<up_transport_layer_info_item_s>;

// PDUSessionResourceSetupRequestTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_setup_request_transfer_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        pdu_session_aggregate_maximum_bit_rate,
        ul_ngu_up_tnl_info,
        add_ul_ngu_up_tnl_info,
        data_forwarding_not_possible,
        pdu_session_type,
        security_ind,
        network_instance,
        qos_flow_setup_request_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pdu_session_aggregate_maximum_bit_rate_s& pdu_session_aggregate_maximum_bit_rate()
    {
      assert_choice_type("PDUSessionAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<pdu_session_aggregate_maximum_bit_rate_s>();
    }
    up_transport_layer_info_c& ul_ngu_up_tnl_info()
    {
      assert_choice_type("UPTransportLayerInformation", type_.to_string(), "Value");
      return c.get<up_transport_layer_info_c>();
    }
    up_transport_layer_info_list_l& add_ul_ngu_up_tnl_info()
    {
      assert_choice_type("UPTransportLayerInformationList", type_.to_string(), "Value");
      return c.get<up_transport_layer_info_list_l>();
    }
    data_forwarding_not_possible_e& data_forwarding_not_possible()
    {
      assert_choice_type("DataForwardingNotPossible", type_.to_string(), "Value");
      return c.get<data_forwarding_not_possible_e>();
    }
    pdu_session_type_e& pdu_session_type()
    {
      assert_choice_type("PDUSessionType", type_.to_string(), "Value");
      return c.get<pdu_session_type_e>();
    }
    security_ind_s& security_ind()
    {
      assert_choice_type("SecurityIndication", type_.to_string(), "Value");
      return c.get<security_ind_s>();
    }
    uint16_t& network_instance()
    {
      assert_choice_type("INTEGER (1..256,...)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    qos_flow_setup_request_list_l& qos_flow_setup_request_list()
    {
      assert_choice_type("QosFlowSetupRequestList", type_.to_string(), "Value");
      return c.get<qos_flow_setup_request_list_l>();
    }
    const pdu_session_aggregate_maximum_bit_rate_s& pdu_session_aggregate_maximum_bit_rate() const
    {
      assert_choice_type("PDUSessionAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<pdu_session_aggregate_maximum_bit_rate_s>();
    }
    const up_transport_layer_info_c& ul_ngu_up_tnl_info() const
    {
      assert_choice_type("UPTransportLayerInformation", type_.to_string(), "Value");
      return c.get<up_transport_layer_info_c>();
    }
    const up_transport_layer_info_list_l& add_ul_ngu_up_tnl_info() const
    {
      assert_choice_type("UPTransportLayerInformationList", type_.to_string(), "Value");
      return c.get<up_transport_layer_info_list_l>();
    }
    const data_forwarding_not_possible_e& data_forwarding_not_possible() const
    {
      assert_choice_type("DataForwardingNotPossible", type_.to_string(), "Value");
      return c.get<data_forwarding_not_possible_e>();
    }
    const pdu_session_type_e& pdu_session_type() const
    {
      assert_choice_type("PDUSessionType", type_.to_string(), "Value");
      return c.get<pdu_session_type_e>();
    }
    const security_ind_s& security_ind() const
    {
      assert_choice_type("SecurityIndication", type_.to_string(), "Value");
      return c.get<security_ind_s>();
    }
    const uint16_t& network_instance() const
    {
      assert_choice_type("INTEGER (1..256,...)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    const qos_flow_setup_request_list_l& qos_flow_setup_request_list() const
    {
      assert_choice_type("QosFlowSetupRequestList", type_.to_string(), "Value");
      return c.get<qos_flow_setup_request_list_l>();
    }
    pdu_session_aggregate_maximum_bit_rate_s& set_pdu_session_aggregate_maximum_bit_rate()
    {
      set(types::pdu_session_aggregate_maximum_bit_rate);
      return c.get<pdu_session_aggregate_maximum_bit_rate_s>();
    }
    up_transport_layer_info_c& set_ul_ngu_up_tnl_info()
    {
      set(types::ul_ngu_up_tnl_info);
      return c.get<up_transport_layer_info_c>();
    }
    up_transport_layer_info_list_l& set_add_ul_ngu_up_tnl_info()
    {
      set(types::add_ul_ngu_up_tnl_info);
      return c.get<up_transport_layer_info_list_l>();
    }
    data_forwarding_not_possible_e& set_data_forwarding_not_possible()
    {
      set(types::data_forwarding_not_possible);
      return c.get<data_forwarding_not_possible_e>();
    }
    pdu_session_type_e& set_pdu_session_type()
    {
      set(types::pdu_session_type);
      return c.get<pdu_session_type_e>();
    }
    security_ind_s& set_security_ind()
    {
      set(types::security_ind);
      return c.get<security_ind_s>();
    }
    uint16_t& set_network_instance()
    {
      set(types::network_instance);
      return c.get<uint16_t>();
    }
    qos_flow_setup_request_list_l& set_qos_flow_setup_request_list()
    {
      set(types::qos_flow_setup_request_list);
      return c.get<qos_flow_setup_request_list_l>();
    }

  private:
    types type_;
    choice_buffer_t<pdu_session_aggregate_maximum_bit_rate_s,
                    qos_flow_setup_request_list_l,
                    security_ind_s,
                    up_transport_layer_info_c,
                    up_transport_layer_info_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceSetupItemHOReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_setup_item_ho_req_ext_ies_o;

// PDUSessionResourceSetupRequestTransfer ::= SEQUENCE
struct pdu_session_res_setup_request_transfer_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                 pdu_session_aggregate_maximum_bit_rate_present = false;
    bool                                                 add_ul_ngu_up_tnl_info_present                 = false;
    bool                                                 data_forwarding_not_possible_present           = false;
    bool                                                 security_ind_present                           = false;
    bool                                                 network_instance_present                       = false;
    ie_field_s<pdu_session_aggregate_maximum_bit_rate_s> pdu_session_aggregate_maximum_bit_rate;
    ie_field_s<up_transport_layer_info_c>                ul_ngu_up_tnl_info;
    ie_field_s<dyn_seq_of<up_transport_layer_info_item_s, 1, 3, true> > add_ul_ngu_up_tnl_info;
    ie_field_s<data_forwarding_not_possible_e>                          data_forwarding_not_possible;
    ie_field_s<pdu_session_type_e>                                      pdu_session_type;
    ie_field_s<security_ind_s>                                          security_ind;
    ie_field_s<integer<uint16_t, 1, 256, false, true> >                 network_instance;
    ie_field_s<dyn_seq_of<qos_flow_setup_request_item_s, 1, 64, true> > qos_flow_setup_request_list;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventType ::= ENUMERATED
struct event_type_opts {
  enum options {
    direct,
    change_of_serve_cell,
    ue_presence_in_area_of_interest,
    stop_change_of_serve_cell,
    stop_ue_presence_in_area_of_interest,
    cancel_location_report_for_the_ue,
    // ...
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<event_type_opts, true> event_type_e;

// LocationReportingRequestType-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o location_report_request_type_ext_ies_o;

// PDUSessionResourceSetupItemHOReq ::= SEQUENCE
struct pdu_session_res_setup_item_ho_req_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  s_nssai_s                 s_nssai;
  unbounded_octstring<true> ho_request_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportArea ::= ENUMERATED
struct report_area_opts {
  enum options { cell, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<report_area_opts, true> report_area_e;

// SecurityContext-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o security_context_ext_ies_o;

// TraceActivation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o trace_activation_ext_ies_o;

// TraceDepth ::= ENUMERATED
struct trace_depth_opts {
  enum options {
    minimum,
    medium,
    maximum,
    minimum_without_vendor_specific_ext,
    medium_without_vendor_specific_ext,
    maximum_without_vendor_specific_ext,
    // ...
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<trace_depth_opts, true> trace_depth_e;

// UESecurityCapabilities-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o ue_security_cap_ext_ies_o;

// LocationReportingRequestType ::= SEQUENCE
struct location_report_request_type_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                    ext                                            = false;
  bool                    area_of_interest_list_present                  = false;
  bool                    location_report_ref_id_to_be_cancelled_present = false;
  bool                    ie_exts_present                                = false;
  event_type_e            event_type;
  report_area_e           report_area;
  area_of_interest_list_l area_of_interest_list;
  uint8_t                 location_report_ref_id_to_be_cancelled = 1;
  ie_exts_l_              ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NewSecurityContextInd ::= ENUMERATED
struct new_security_context_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<new_security_context_ind_opts, true> new_security_context_ind_e;

// PDUSessionResourceSetupListHOReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSetupItemHOReq
using pdu_session_res_setup_list_ho_req_l = dyn_array<pdu_session_res_setup_item_ho_req_s>;

// RRCInactiveTransitionReportRequest ::= ENUMERATED
struct rrc_inactive_transition_report_request_opts {
  enum options {
    subsequent_state_transition_report,
    single_rrc_connected_state_report,
    cancel_report,
    /*...*/ nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<rrc_inactive_transition_report_request_opts, true> rrc_inactive_transition_report_request_e;

// RedirectionVoiceFallback ::= ENUMERATED
struct redirection_voice_fallback_opts {
  enum options { possible, not_possible, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<redirection_voice_fallback_opts, true> redirection_voice_fallback_e;

// SecurityContext ::= SEQUENCE
struct security_context_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                              ext                     = false;
  bool                              ie_exts_present         = false;
  uint8_t                           next_hop_chaining_count = 0;
  fixed_bitstring<256, false, true> next_hop_nh;
  ie_exts_l_                        ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceActivation ::= SEQUENCE
struct trace_activation_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  fixed_octstring<8, true>              ngran_trace_id;
  fixed_bitstring<8, false, true>       interfaces_to_trace;
  trace_depth_e                         trace_depth;
  bounded_bitstring<1, 160, true, true> trace_collection_entity_ip_address;
  ie_exts_l_                            ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UESecurityCapabilities ::= SEQUENCE
struct ue_security_cap_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  fixed_bitstring<16, true, true> nrencryption_algorithms;
  fixed_bitstring<16, true, true> nrintegrity_protection_algorithms;
  fixed_bitstring<16, true, true> eutr_aencryption_algorithms;
  fixed_bitstring<16, true, true> eutr_aintegrity_protection_algorithms;
  ie_exts_l_                      ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ho_type,
        cause,
        ue_aggregate_maximum_bit_rate,
        core_network_assist_info,
        ue_security_cap,
        security_context,
        new_security_context_ind,
        nasc,
        pdu_session_res_setup_list_ho_req,
        allowed_nssai,
        trace_activation,
        masked_imeisv,
        source_to_target_transparent_container,
        mob_restrict_list,
        location_report_request_type,
        rrc_inactive_transition_report_request,
        guami,
        redirection_voice_fallback,
        nulltype
      } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    ho_type_e& ho_type()
    {
      assert_choice_type("HandoverType", type_.to_string(), "Value");
      return c.get<ho_type_e>();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    ue_aggregate_maximum_bit_rate_s& ue_aggregate_maximum_bit_rate()
    {
      assert_choice_type("UEAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    core_network_assist_info_s& core_network_assist_info()
    {
      assert_choice_type("CoreNetworkAssistanceInformation", type_.to_string(), "Value");
      return c.get<core_network_assist_info_s>();
    }
    ue_security_cap_s& ue_security_cap()
    {
      assert_choice_type("UESecurityCapabilities", type_.to_string(), "Value");
      return c.get<ue_security_cap_s>();
    }
    security_context_s& security_context()
    {
      assert_choice_type("SecurityContext", type_.to_string(), "Value");
      return c.get<security_context_s>();
    }
    new_security_context_ind_e& new_security_context_ind()
    {
      assert_choice_type("NewSecurityContextInd", type_.to_string(), "Value");
      return c.get<new_security_context_ind_e>();
    }
    unbounded_octstring<true>& nasc()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    pdu_session_res_setup_list_ho_req_l& pdu_session_res_setup_list_ho_req()
    {
      assert_choice_type("PDUSessionResourceSetupListHOReq", type_.to_string(), "Value");
      return c.get<pdu_session_res_setup_list_ho_req_l>();
    }
    allowed_nssai_l& allowed_nssai()
    {
      assert_choice_type("AllowedNSSAI", type_.to_string(), "Value");
      return c.get<allowed_nssai_l>();
    }
    trace_activation_s& trace_activation()
    {
      assert_choice_type("TraceActivation", type_.to_string(), "Value");
      return c.get<trace_activation_s>();
    }
    fixed_bitstring<64, false, true>& masked_imeisv()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<64, false, true> >();
    }
    unbounded_octstring<true>& source_to_target_transparent_container()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    mob_restrict_list_s& mob_restrict_list()
    {
      assert_choice_type("MobilityRestrictionList", type_.to_string(), "Value");
      return c.get<mob_restrict_list_s>();
    }
    location_report_request_type_s& location_report_request_type()
    {
      assert_choice_type("LocationReportingRequestType", type_.to_string(), "Value");
      return c.get<location_report_request_type_s>();
    }
    rrc_inactive_transition_report_request_e& rrc_inactive_transition_report_request()
    {
      assert_choice_type("RRCInactiveTransitionReportRequest", type_.to_string(), "Value");
      return c.get<rrc_inactive_transition_report_request_e>();
    }
    guami_s& guami()
    {
      assert_choice_type("GUAMI", type_.to_string(), "Value");
      return c.get<guami_s>();
    }
    redirection_voice_fallback_e& redirection_voice_fallback()
    {
      assert_choice_type("RedirectionVoiceFallback", type_.to_string(), "Value");
      return c.get<redirection_voice_fallback_e>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const ho_type_e& ho_type() const
    {
      assert_choice_type("HandoverType", type_.to_string(), "Value");
      return c.get<ho_type_e>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const ue_aggregate_maximum_bit_rate_s& ue_aggregate_maximum_bit_rate() const
    {
      assert_choice_type("UEAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    const core_network_assist_info_s& core_network_assist_info() const
    {
      assert_choice_type("CoreNetworkAssistanceInformation", type_.to_string(), "Value");
      return c.get<core_network_assist_info_s>();
    }
    const ue_security_cap_s& ue_security_cap() const
    {
      assert_choice_type("UESecurityCapabilities", type_.to_string(), "Value");
      return c.get<ue_security_cap_s>();
    }
    const security_context_s& security_context() const
    {
      assert_choice_type("SecurityContext", type_.to_string(), "Value");
      return c.get<security_context_s>();
    }
    const new_security_context_ind_e& new_security_context_ind() const
    {
      assert_choice_type("NewSecurityContextInd", type_.to_string(), "Value");
      return c.get<new_security_context_ind_e>();
    }
    const unbounded_octstring<true>& nasc() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const pdu_session_res_setup_list_ho_req_l& pdu_session_res_setup_list_ho_req() const
    {
      assert_choice_type("PDUSessionResourceSetupListHOReq", type_.to_string(), "Value");
      return c.get<pdu_session_res_setup_list_ho_req_l>();
    }
    const allowed_nssai_l& allowed_nssai() const
    {
      assert_choice_type("AllowedNSSAI", type_.to_string(), "Value");
      return c.get<allowed_nssai_l>();
    }
    const trace_activation_s& trace_activation() const
    {
      assert_choice_type("TraceActivation", type_.to_string(), "Value");
      return c.get<trace_activation_s>();
    }
    const fixed_bitstring<64, false, true>& masked_imeisv() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<64, false, true> >();
    }
    const unbounded_octstring<true>& source_to_target_transparent_container() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const mob_restrict_list_s& mob_restrict_list() const
    {
      assert_choice_type("MobilityRestrictionList", type_.to_string(), "Value");
      return c.get<mob_restrict_list_s>();
    }
    const location_report_request_type_s& location_report_request_type() const
    {
      assert_choice_type("LocationReportingRequestType", type_.to_string(), "Value");
      return c.get<location_report_request_type_s>();
    }
    const rrc_inactive_transition_report_request_e& rrc_inactive_transition_report_request() const
    {
      assert_choice_type("RRCInactiveTransitionReportRequest", type_.to_string(), "Value");
      return c.get<rrc_inactive_transition_report_request_e>();
    }
    const guami_s& guami() const
    {
      assert_choice_type("GUAMI", type_.to_string(), "Value");
      return c.get<guami_s>();
    }
    const redirection_voice_fallback_e& redirection_voice_fallback() const
    {
      assert_choice_type("RedirectionVoiceFallback", type_.to_string(), "Value");
      return c.get<redirection_voice_fallback_e>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    ho_type_e& set_ho_type()
    {
      set(types::ho_type);
      return c.get<ho_type_e>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }
    ue_aggregate_maximum_bit_rate_s& set_ue_aggregate_maximum_bit_rate()
    {
      set(types::ue_aggregate_maximum_bit_rate);
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    core_network_assist_info_s& set_core_network_assist_info()
    {
      set(types::core_network_assist_info);
      return c.get<core_network_assist_info_s>();
    }
    ue_security_cap_s& set_ue_security_cap()
    {
      set(types::ue_security_cap);
      return c.get<ue_security_cap_s>();
    }
    security_context_s& set_security_context()
    {
      set(types::security_context);
      return c.get<security_context_s>();
    }
    new_security_context_ind_e& set_new_security_context_ind()
    {
      set(types::new_security_context_ind);
      return c.get<new_security_context_ind_e>();
    }
    unbounded_octstring<true>& set_nasc()
    {
      set(types::nasc);
      return c.get<unbounded_octstring<true> >();
    }
    pdu_session_res_setup_list_ho_req_l& set_pdu_session_res_setup_list_ho_req()
    {
      set(types::pdu_session_res_setup_list_ho_req);
      return c.get<pdu_session_res_setup_list_ho_req_l>();
    }
    allowed_nssai_l& set_allowed_nssai()
    {
      set(types::allowed_nssai);
      return c.get<allowed_nssai_l>();
    }
    trace_activation_s& set_trace_activation()
    {
      set(types::trace_activation);
      return c.get<trace_activation_s>();
    }
    fixed_bitstring<64, false, true>& set_masked_imeisv()
    {
      set(types::masked_imeisv);
      return c.get<fixed_bitstring<64, false, true> >();
    }
    unbounded_octstring<true>& set_source_to_target_transparent_container()
    {
      set(types::source_to_target_transparent_container);
      return c.get<unbounded_octstring<true> >();
    }
    mob_restrict_list_s& set_mob_restrict_list()
    {
      set(types::mob_restrict_list);
      return c.get<mob_restrict_list_s>();
    }
    location_report_request_type_s& set_location_report_request_type()
    {
      set(types::location_report_request_type);
      return c.get<location_report_request_type_s>();
    }
    rrc_inactive_transition_report_request_e& set_rrc_inactive_transition_report_request()
    {
      set(types::rrc_inactive_transition_report_request);
      return c.get<rrc_inactive_transition_report_request_e>();
    }
    guami_s& set_guami()
    {
      set(types::guami);
      return c.get<guami_s>();
    }
    redirection_voice_fallback_e& set_redirection_voice_fallback()
    {
      set(types::redirection_voice_fallback);
      return c.get<redirection_voice_fallback_e>();
    }

  private:
    types type_;
    choice_buffer_t<allowed_nssai_l,
                    cause_c,
                    core_network_assist_info_s,
                    fixed_bitstring<64, false, true>,
                    guami_s,
                    location_report_request_type_s,
                    mob_restrict_list_s,
                    pdu_session_res_setup_list_ho_req_l,
                    security_context_s,
                    trace_activation_s,
                    ue_aggregate_maximum_bit_rate_s,
                    ue_security_cap_s,
                    unbounded_octstring<true> >
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// HandoverRequest ::= SEQUENCE
struct ho_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool core_network_assist_info_present               = false;
    bool new_security_context_ind_present               = false;
    bool nasc_present                                   = false;
    bool trace_activation_present                       = false;
    bool masked_imeisv_present                          = false;
    bool mob_restrict_list_present                      = false;
    bool location_report_request_type_present           = false;
    bool rrc_inactive_transition_report_request_present = false;
    bool redirection_voice_fallback_present             = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> >              amf_ue_ngap_id;
    ie_field_s<ho_type_e>                                                      ho_type;
    ie_field_s<cause_c>                                                        cause;
    ie_field_s<ue_aggregate_maximum_bit_rate_s>                                ue_aggregate_maximum_bit_rate;
    ie_field_s<core_network_assist_info_s>                                     core_network_assist_info;
    ie_field_s<ue_security_cap_s>                                              ue_security_cap;
    ie_field_s<security_context_s>                                             security_context;
    ie_field_s<new_security_context_ind_e>                                     new_security_context_ind;
    ie_field_s<unbounded_octstring<true> >                                     nasc;
    ie_field_s<dyn_seq_of<pdu_session_res_setup_item_ho_req_s, 1, 256, true> > pdu_session_res_setup_list_ho_req;
    ie_field_s<dyn_seq_of<allowed_nssai_item_s, 1, 8, true> >                  allowed_nssai;
    ie_field_s<trace_activation_s>                                             trace_activation;
    ie_field_s<fixed_bitstring<64, false, true> >                              masked_imeisv;
    ie_field_s<unbounded_octstring<true> >                                     source_to_target_transparent_container;
    ie_field_s<mob_restrict_list_s>                                            mob_restrict_list;
    ie_field_s<location_report_request_type_s>                                 location_report_request_type;
    ie_field_s<rrc_inactive_transition_report_request_e>                       rrc_inactive_transition_report_request;
    ie_field_s<guami_s>                                                        guami;
    ie_field_s<redirection_voice_fallback_e>                                   redirection_voice_fallback;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowWithCauseItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_with_cause_item_ext_ies_o;

// ConfidentialityProtectionResult ::= ENUMERATED
struct confidentiality_protection_result_opts {
  enum options { performed, not_performed, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<confidentiality_protection_result_opts, true> confidentiality_protection_result_e;

// IntegrityProtectionResult ::= ENUMERATED
struct integrity_protection_result_opts {
  enum options { performed, not_performed, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<integrity_protection_result_opts, true> integrity_protection_result_e;

// QosFlowWithCauseItem ::= SEQUENCE
struct qos_flow_with_cause_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  uint8_t    qos_flow_id     = 0;
  cause_c    cause;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityResult-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o security_result_ext_ies_o;

// HandoverRequestAcknowledgeTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct ho_request_ack_transfer_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { add_dluptnl_info_for_ho_list, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::add_dluptnl_info_for_ho_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    add_dluptnl_info_for_ho_list_l&       add_dluptnl_info_for_ho_list() { return c; }
    const add_dluptnl_info_for_ho_list_l& add_dluptnl_info_for_ho_list() const { return c; }

  private:
    add_dluptnl_info_for_ho_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// HandoverResourceAllocationUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o ho_res_alloc_unsuccessful_transfer_ext_ies_o;

// QosFlowListWithCause ::= SEQUENCE (SIZE (1..64)) OF QosFlowWithCauseItem
using qos_flow_list_with_cause_l = dyn_array<qos_flow_with_cause_item_s>;

// SecurityResult ::= SEQUENCE
struct security_result_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  integrity_protection_result_e       integrity_protection_result;
  confidentiality_protection_result_e confidentiality_protection_result;
  ie_exts_l_                          ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverRequestAcknowledgeTransfer ::= SEQUENCE
struct ho_request_ack_transfer_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                                                                add_dluptnl_info_for_ho_list_present = false;
    ie_field_s<dyn_seq_of<add_dluptnl_info_for_ho_item_s, 1, 3, true> > add_dluptnl_info_for_ho_list;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                                 ext                                   = false;
  bool                                 dlforwarding_up_tnl_info_present      = false;
  bool                                 security_result_present               = false;
  bool                                 qos_flow_failed_to_setup_list_present = false;
  bool                                 data_forwarding_resp_drb_list_present = false;
  bool                                 ie_exts_present                       = false;
  up_transport_layer_info_c            dl_ngu_up_tnl_info;
  up_transport_layer_info_c            dlforwarding_up_tnl_info;
  security_result_s                    security_result;
  qos_flow_list_with_data_forwarding_l qos_flow_setup_resp_list;
  qos_flow_list_with_cause_l           qos_flow_failed_to_setup_list;
  data_forwarding_resp_drb_list_l      data_forwarding_resp_drb_list;
  ie_exts_l_                           ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverResourceAllocationUnsuccessfulTransfer ::= SEQUENCE
struct ho_res_alloc_unsuccessful_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool               ext                      = false;
  bool               crit_diagnostics_present = false;
  bool               ie_exts_present          = false;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;
  ie_exts_l_         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceAdmittedItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_admitted_item_ext_ies_o;

// PDUSessionResourceFailedToSetupItemHOAck-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_failed_to_setup_item_ho_ack_ext_ies_o;

// PDUSessionResourceAdmittedItem ::= SEQUENCE
struct pdu_session_res_admitted_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> ho_request_ack_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToSetupItemHOAck ::= SEQUENCE
struct pdu_session_res_failed_to_setup_item_ho_ack_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> ho_res_alloc_unsuccessful_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceAdmittedList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceAdmittedItem
using pdu_session_res_admitted_list_l = dyn_array<pdu_session_res_admitted_item_s>;

// PDUSessionResourceFailedToSetupListHOAck ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToSetupItemHOAck
using pdu_session_res_failed_to_setup_list_ho_ack_l = dyn_array<pdu_session_res_failed_to_setup_item_ho_ack_s>;

// HandoverRequestAcknowledgeIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_request_ack_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_admitted_list,
        pdu_session_res_failed_to_setup_list_ho_ack,
        target_to_source_transparent_container,
        crit_diagnostics,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    pdu_session_res_admitted_list_l& pdu_session_res_admitted_list()
    {
      assert_choice_type("PDUSessionResourceAdmittedList", type_.to_string(), "Value");
      return c.get<pdu_session_res_admitted_list_l>();
    }
    pdu_session_res_failed_to_setup_list_ho_ack_l& pdu_session_res_failed_to_setup_list_ho_ack()
    {
      assert_choice_type("PDUSessionResourceFailedToSetupListHOAck", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_setup_list_ho_ack_l>();
    }
    unbounded_octstring<true>& target_to_source_transparent_container()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const pdu_session_res_admitted_list_l& pdu_session_res_admitted_list() const
    {
      assert_choice_type("PDUSessionResourceAdmittedList", type_.to_string(), "Value");
      return c.get<pdu_session_res_admitted_list_l>();
    }
    const pdu_session_res_failed_to_setup_list_ho_ack_l& pdu_session_res_failed_to_setup_list_ho_ack() const
    {
      assert_choice_type("PDUSessionResourceFailedToSetupListHOAck", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_setup_list_ho_ack_l>();
    }
    const unbounded_octstring<true>& target_to_source_transparent_container() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    pdu_session_res_admitted_list_l& set_pdu_session_res_admitted_list()
    {
      set(types::pdu_session_res_admitted_list);
      return c.get<pdu_session_res_admitted_list_l>();
    }
    pdu_session_res_failed_to_setup_list_ho_ack_l& set_pdu_session_res_failed_to_setup_list_ho_ack()
    {
      set(types::pdu_session_res_failed_to_setup_list_ho_ack);
      return c.get<pdu_session_res_failed_to_setup_list_ho_ack_l>();
    }
    unbounded_octstring<true>& set_target_to_source_transparent_container()
    {
      set(types::target_to_source_transparent_container);
      return c.get<unbounded_octstring<true> >();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    pdu_session_res_admitted_list_l,
                    pdu_session_res_failed_to_setup_list_ho_ack_l,
                    unbounded_octstring<true> >
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// HandoverRequestAcknowledge ::= SEQUENCE
struct ho_request_ack_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool pdu_session_res_failed_to_setup_list_ho_ack_present = false;
    bool crit_diagnostics_present                            = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> >          amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >             ran_ue_ngap_id;
    ie_field_s<dyn_seq_of<pdu_session_res_admitted_item_s, 1, 256, true> > pdu_session_res_admitted_list;
    ie_field_s<dyn_seq_of<pdu_session_res_failed_to_setup_item_ho_ack_s, 1, 256, true> >
                                           pdu_session_res_failed_to_setup_list_ho_ack;
    ie_field_s<unbounded_octstring<true> > target_to_source_transparent_container;
    ie_field_s<crit_diagnostics_s>         crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DirectForwardingPathAvailability ::= ENUMERATED
struct direct_forwarding_path_availability_opts {
  enum options { direct_path_available, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<direct_forwarding_path_availability_opts, true> direct_forwarding_path_availability_e;

// HandoverRequiredTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o ho_required_transfer_ext_ies_o;

// HandoverRequiredTransfer ::= SEQUENCE
struct ho_required_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                  ext                                         = false;
  bool                                  direct_forwarding_path_availability_present = false;
  bool                                  ie_exts_present                             = false;
  direct_forwarding_path_availability_e direct_forwarding_path_availability;
  ie_exts_l_                            ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceItemHORqd-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_item_ho_rqd_ext_ies_o;

// TargeteNB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o targete_nb_id_ext_ies_o;

// PDUSessionResourceItemHORqd ::= SEQUENCE
struct pdu_session_res_item_ho_rqd_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> ho_required_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargetID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o target_id_ext_ies_o;

// TargeteNB-ID ::= SEQUENCE
struct targete_nb_id_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool               ext             = false;
  bool               ie_exts_present = false;
  global_ng_enb_id_s global_enb_id;
  eps_tai_s          sel_eps_tai;
  ie_exts_l_         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceListHORqd ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceItemHORqd
using pdu_session_res_list_ho_rqd_l = dyn_array<pdu_session_res_item_ho_rqd_s>;

// TargetID ::= CHOICE
struct target_id_c {
  struct types_opts {
    enum options { target_ran_node_id, targete_nb_id, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  target_id_c() = default;
  target_id_c(const target_id_c& other);
  target_id_c& operator=(const target_id_c& other);
  ~target_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  target_ran_node_id_s& target_ran_node_id()
  {
    assert_choice_type("targetRANNodeID", type_.to_string(), "TargetID");
    return c.get<target_ran_node_id_s>();
  }
  targete_nb_id_s& targete_nb_id()
  {
    assert_choice_type("targeteNB-ID", type_.to_string(), "TargetID");
    return c.get<targete_nb_id_s>();
  }
  protocol_ie_single_container_s<target_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "TargetID");
    return c.get<protocol_ie_single_container_s<target_id_ext_ies_o> >();
  }
  const target_ran_node_id_s& target_ran_node_id() const
  {
    assert_choice_type("targetRANNodeID", type_.to_string(), "TargetID");
    return c.get<target_ran_node_id_s>();
  }
  const targete_nb_id_s& targete_nb_id() const
  {
    assert_choice_type("targeteNB-ID", type_.to_string(), "TargetID");
    return c.get<targete_nb_id_s>();
  }
  const protocol_ie_single_container_s<target_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "TargetID");
    return c.get<protocol_ie_single_container_s<target_id_ext_ies_o> >();
  }
  target_ran_node_id_s& set_target_ran_node_id()
  {
    set(types::target_ran_node_id);
    return c.get<target_ran_node_id_s>();
  }
  targete_nb_id_s& set_targete_nb_id()
  {
    set(types::targete_nb_id);
    return c.get<targete_nb_id_s>();
  }
  protocol_ie_single_container_s<target_id_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<target_id_ext_ies_o> >();
  }

private:
  types                                                                                                       type_;
  choice_buffer_t<protocol_ie_single_container_s<target_id_ext_ies_o>, target_ran_node_id_s, targete_nb_id_s> c;

  void destroy_();
};

// HandoverRequiredIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_required_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ho_type,
        cause,
        target_id,
        direct_forwarding_path_availability,
        pdu_session_res_list_ho_rqd,
        source_to_target_transparent_container,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    ho_type_e& ho_type()
    {
      assert_choice_type("HandoverType", type_.to_string(), "Value");
      return c.get<ho_type_e>();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    target_id_c& target_id()
    {
      assert_choice_type("TargetID", type_.to_string(), "Value");
      return c.get<target_id_c>();
    }
    direct_forwarding_path_availability_e& direct_forwarding_path_availability()
    {
      assert_choice_type("DirectForwardingPathAvailability", type_.to_string(), "Value");
      return c.get<direct_forwarding_path_availability_e>();
    }
    pdu_session_res_list_ho_rqd_l& pdu_session_res_list_ho_rqd()
    {
      assert_choice_type("PDUSessionResourceListHORqd", type_.to_string(), "Value");
      return c.get<pdu_session_res_list_ho_rqd_l>();
    }
    unbounded_octstring<true>& source_to_target_transparent_container()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const ho_type_e& ho_type() const
    {
      assert_choice_type("HandoverType", type_.to_string(), "Value");
      return c.get<ho_type_e>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const target_id_c& target_id() const
    {
      assert_choice_type("TargetID", type_.to_string(), "Value");
      return c.get<target_id_c>();
    }
    const direct_forwarding_path_availability_e& direct_forwarding_path_availability() const
    {
      assert_choice_type("DirectForwardingPathAvailability", type_.to_string(), "Value");
      return c.get<direct_forwarding_path_availability_e>();
    }
    const pdu_session_res_list_ho_rqd_l& pdu_session_res_list_ho_rqd() const
    {
      assert_choice_type("PDUSessionResourceListHORqd", type_.to_string(), "Value");
      return c.get<pdu_session_res_list_ho_rqd_l>();
    }
    const unbounded_octstring<true>& source_to_target_transparent_container() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    ho_type_e& set_ho_type()
    {
      set(types::ho_type);
      return c.get<ho_type_e>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }
    target_id_c& set_target_id()
    {
      set(types::target_id);
      return c.get<target_id_c>();
    }
    direct_forwarding_path_availability_e& set_direct_forwarding_path_availability()
    {
      set(types::direct_forwarding_path_availability);
      return c.get<direct_forwarding_path_availability_e>();
    }
    pdu_session_res_list_ho_rqd_l& set_pdu_session_res_list_ho_rqd()
    {
      set(types::pdu_session_res_list_ho_rqd);
      return c.get<pdu_session_res_list_ho_rqd_l>();
    }
    unbounded_octstring<true>& set_source_to_target_transparent_container()
    {
      set(types::source_to_target_transparent_container);
      return c.get<unbounded_octstring<true> >();
    }

  private:
    types                                                                                            type_;
    choice_buffer_t<cause_c, pdu_session_res_list_ho_rqd_l, target_id_c, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// HandoverRequired ::= SEQUENCE
struct ho_required_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          direct_forwarding_path_availability_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<ho_type_e>                                         ho_type;
    ie_field_s<cause_c>                                           cause;
    ie_field_s<target_id_c>                                       target_id;
    ie_field_s<direct_forwarding_path_availability_e>             direct_forwarding_path_availability;
    ie_field_s<dyn_seq_of<pdu_session_res_item_ho_rqd_s, 1, 256, true> > pdu_session_res_list_ho_rqd;
    ie_field_s<unbounded_octstring<true> >                               source_to_target_transparent_container;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RecommendedRANNodeItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o recommended_ran_node_item_ext_ies_o;

// RecommendedRANNodeItem ::= SEQUENCE
struct recommended_ran_node_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                ext             = false;
  bool                ie_exts_present = false;
  amf_paging_target_c amf_paging_target;
  ie_exts_l_          ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RecommendedRANNodeList ::= SEQUENCE (SIZE (1..16)) OF RecommendedRANNodeItem
using recommended_ran_node_list_l = dyn_array<recommended_ran_node_item_s>;

// RecommendedRANNodesForPaging-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o recommended_ran_nodes_for_paging_ext_ies_o;

// InfoOnRecommendedCellsAndRANNodesForPaging-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o info_on_recommended_cells_and_ran_nodes_for_paging_ext_ies_o;

// RecommendedRANNodesForPaging ::= SEQUENCE
struct recommended_ran_nodes_for_paging_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                        ext             = false;
  bool                        ie_exts_present = false;
  recommended_ran_node_list_l recommended_ran_node_list;
  ie_exts_l_                  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InfoOnRecommendedCellsAndRANNodesForPaging ::= SEQUENCE
struct info_on_recommended_cells_and_ran_nodes_for_paging_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                               ext             = false;
  bool                               ie_exts_present = false;
  recommended_cells_for_paging_s     recommended_cells_for_paging;
  recommended_ran_nodes_for_paging_s recommend_ran_nodes_for_paging;
  ie_exts_l_                         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_setup_unsuccessful_transfer_ext_ies_o;

// PDUSessionResourceFailedToSetupItemCxtFail-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_failed_to_setup_item_cxt_fail_ext_ies_o;

// PDUSessionResourceSetupUnsuccessfulTransfer ::= SEQUENCE
struct pdu_session_res_setup_unsuccessful_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool               ext                      = false;
  bool               crit_diagnostics_present = false;
  bool               ie_exts_present          = false;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;
  ie_exts_l_         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToSetupItemCxtFail ::= SEQUENCE
struct pdu_session_res_failed_to_setup_item_cxt_fail_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_setup_unsuccessful_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToSetupListCxtFail ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToSetupItemCxtFail
using pdu_session_res_failed_to_setup_list_cxt_fail_l = dyn_array<pdu_session_res_failed_to_setup_item_cxt_fail_s>;

// InitialContextSetupFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct init_context_setup_fail_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_failed_to_setup_list_cxt_fail,
        cause,
        crit_diagnostics,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    pdu_session_res_failed_to_setup_list_cxt_fail_l& pdu_session_res_failed_to_setup_list_cxt_fail()
    {
      assert_choice_type("PDUSessionResourceFailedToSetupListCxtFail", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_setup_list_cxt_fail_l>();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const pdu_session_res_failed_to_setup_list_cxt_fail_l& pdu_session_res_failed_to_setup_list_cxt_fail() const
    {
      assert_choice_type("PDUSessionResourceFailedToSetupListCxtFail", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_setup_list_cxt_fail_l>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    pdu_session_res_failed_to_setup_list_cxt_fail_l& set_pdu_session_res_failed_to_setup_list_cxt_fail()
    {
      set(types::pdu_session_res_failed_to_setup_list_cxt_fail);
      return c.get<pdu_session_res_failed_to_setup_list_cxt_fail_l>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                                                                         type_;
    choice_buffer_t<cause_c, crit_diagnostics_s, pdu_session_res_failed_to_setup_list_cxt_fail_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// InitialContextSetupFailure ::= SEQUENCE
struct init_context_setup_fail_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool pdu_session_res_failed_to_setup_list_cxt_fail_present = false;
    bool crit_diagnostics_present                              = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<dyn_seq_of<pdu_session_res_failed_to_setup_item_cxt_fail_s, 1, 256, true> >
                                   pdu_session_res_failed_to_setup_list_cxt_fail;
    ie_field_s<cause_c>            cause;
    ie_field_s<crit_diagnostics_s> crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupItemCxtReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_setup_item_cxt_req_ext_ies_o;

// PDUSessionResourceSetupItemCxtReq ::= SEQUENCE
struct pdu_session_res_setup_item_cxt_req_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      nas_pdu_present = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> nas_pdu;
  s_nssai_s                 s_nssai;
  unbounded_octstring<true> pdu_session_res_setup_request_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioCapabilityForPaging-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o ue_radio_cap_for_paging_ext_ies_o;

// PDUSessionResourceSetupListCxtReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSetupItemCxtReq
using pdu_session_res_setup_list_cxt_req_l = dyn_array<pdu_session_res_setup_item_cxt_req_s>;

// UERadioCapabilityForPaging ::= SEQUENCE
struct ue_radio_cap_for_paging_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext                                     = false;
  bool                      ueradio_cap_for_paging_of_nr_present    = false;
  bool                      ueradio_cap_for_paging_of_eutra_present = false;
  bool                      ie_exts_present                         = false;
  unbounded_octstring<true> ueradio_cap_for_paging_of_nr;
  unbounded_octstring<true> ueradio_cap_for_paging_of_eutra;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialContextSetupRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct init_context_setup_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        old_amf,
        ue_aggregate_maximum_bit_rate,
        core_network_assist_info,
        guami,
        pdu_session_res_setup_list_cxt_req,
        allowed_nssai,
        ue_security_cap,
        security_key,
        trace_activation,
        mob_restrict_list,
        ue_radio_cap,
        idx_to_rfsp,
        masked_imeisv,
        nas_pdu,
        emergency_fallback_ind,
        rrc_inactive_transition_report_request,
        ue_radio_cap_for_paging,
        redirection_voice_fallback,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    printable_string<1, 150, true, true>& old_amf()
    {
      assert_choice_type("PrintableString", type_.to_string(), "Value");
      return c.get<printable_string<1, 150, true, true> >();
    }
    ue_aggregate_maximum_bit_rate_s& ue_aggregate_maximum_bit_rate()
    {
      assert_choice_type("UEAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    core_network_assist_info_s& core_network_assist_info()
    {
      assert_choice_type("CoreNetworkAssistanceInformation", type_.to_string(), "Value");
      return c.get<core_network_assist_info_s>();
    }
    guami_s& guami()
    {
      assert_choice_type("GUAMI", type_.to_string(), "Value");
      return c.get<guami_s>();
    }
    pdu_session_res_setup_list_cxt_req_l& pdu_session_res_setup_list_cxt_req()
    {
      assert_choice_type("PDUSessionResourceSetupListCxtReq", type_.to_string(), "Value");
      return c.get<pdu_session_res_setup_list_cxt_req_l>();
    }
    allowed_nssai_l& allowed_nssai()
    {
      assert_choice_type("AllowedNSSAI", type_.to_string(), "Value");
      return c.get<allowed_nssai_l>();
    }
    ue_security_cap_s& ue_security_cap()
    {
      assert_choice_type("UESecurityCapabilities", type_.to_string(), "Value");
      return c.get<ue_security_cap_s>();
    }
    fixed_bitstring<256, false, true>& security_key()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<256, false, true> >();
    }
    trace_activation_s& trace_activation()
    {
      assert_choice_type("TraceActivation", type_.to_string(), "Value");
      return c.get<trace_activation_s>();
    }
    mob_restrict_list_s& mob_restrict_list()
    {
      assert_choice_type("MobilityRestrictionList", type_.to_string(), "Value");
      return c.get<mob_restrict_list_s>();
    }
    unbounded_octstring<true>& ue_radio_cap()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    uint16_t& idx_to_rfsp()
    {
      assert_choice_type("INTEGER (1..256,...)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    fixed_bitstring<64, false, true>& masked_imeisv()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<64, false, true> >();
    }
    unbounded_octstring<true>& nas_pdu()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    emergency_fallback_ind_s& emergency_fallback_ind()
    {
      assert_choice_type("EmergencyFallbackIndicator", type_.to_string(), "Value");
      return c.get<emergency_fallback_ind_s>();
    }
    rrc_inactive_transition_report_request_e& rrc_inactive_transition_report_request()
    {
      assert_choice_type("RRCInactiveTransitionReportRequest", type_.to_string(), "Value");
      return c.get<rrc_inactive_transition_report_request_e>();
    }
    ue_radio_cap_for_paging_s& ue_radio_cap_for_paging()
    {
      assert_choice_type("UERadioCapabilityForPaging", type_.to_string(), "Value");
      return c.get<ue_radio_cap_for_paging_s>();
    }
    redirection_voice_fallback_e& redirection_voice_fallback()
    {
      assert_choice_type("RedirectionVoiceFallback", type_.to_string(), "Value");
      return c.get<redirection_voice_fallback_e>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const printable_string<1, 150, true, true>& old_amf() const
    {
      assert_choice_type("PrintableString", type_.to_string(), "Value");
      return c.get<printable_string<1, 150, true, true> >();
    }
    const ue_aggregate_maximum_bit_rate_s& ue_aggregate_maximum_bit_rate() const
    {
      assert_choice_type("UEAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    const core_network_assist_info_s& core_network_assist_info() const
    {
      assert_choice_type("CoreNetworkAssistanceInformation", type_.to_string(), "Value");
      return c.get<core_network_assist_info_s>();
    }
    const guami_s& guami() const
    {
      assert_choice_type("GUAMI", type_.to_string(), "Value");
      return c.get<guami_s>();
    }
    const pdu_session_res_setup_list_cxt_req_l& pdu_session_res_setup_list_cxt_req() const
    {
      assert_choice_type("PDUSessionResourceSetupListCxtReq", type_.to_string(), "Value");
      return c.get<pdu_session_res_setup_list_cxt_req_l>();
    }
    const allowed_nssai_l& allowed_nssai() const
    {
      assert_choice_type("AllowedNSSAI", type_.to_string(), "Value");
      return c.get<allowed_nssai_l>();
    }
    const ue_security_cap_s& ue_security_cap() const
    {
      assert_choice_type("UESecurityCapabilities", type_.to_string(), "Value");
      return c.get<ue_security_cap_s>();
    }
    const fixed_bitstring<256, false, true>& security_key() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<256, false, true> >();
    }
    const trace_activation_s& trace_activation() const
    {
      assert_choice_type("TraceActivation", type_.to_string(), "Value");
      return c.get<trace_activation_s>();
    }
    const mob_restrict_list_s& mob_restrict_list() const
    {
      assert_choice_type("MobilityRestrictionList", type_.to_string(), "Value");
      return c.get<mob_restrict_list_s>();
    }
    const unbounded_octstring<true>& ue_radio_cap() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const uint16_t& idx_to_rfsp() const
    {
      assert_choice_type("INTEGER (1..256,...)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    const fixed_bitstring<64, false, true>& masked_imeisv() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<64, false, true> >();
    }
    const unbounded_octstring<true>& nas_pdu() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const emergency_fallback_ind_s& emergency_fallback_ind() const
    {
      assert_choice_type("EmergencyFallbackIndicator", type_.to_string(), "Value");
      return c.get<emergency_fallback_ind_s>();
    }
    const rrc_inactive_transition_report_request_e& rrc_inactive_transition_report_request() const
    {
      assert_choice_type("RRCInactiveTransitionReportRequest", type_.to_string(), "Value");
      return c.get<rrc_inactive_transition_report_request_e>();
    }
    const ue_radio_cap_for_paging_s& ue_radio_cap_for_paging() const
    {
      assert_choice_type("UERadioCapabilityForPaging", type_.to_string(), "Value");
      return c.get<ue_radio_cap_for_paging_s>();
    }
    const redirection_voice_fallback_e& redirection_voice_fallback() const
    {
      assert_choice_type("RedirectionVoiceFallback", type_.to_string(), "Value");
      return c.get<redirection_voice_fallback_e>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    printable_string<1, 150, true, true>& set_old_amf()
    {
      set(types::old_amf);
      return c.get<printable_string<1, 150, true, true> >();
    }
    ue_aggregate_maximum_bit_rate_s& set_ue_aggregate_maximum_bit_rate()
    {
      set(types::ue_aggregate_maximum_bit_rate);
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    core_network_assist_info_s& set_core_network_assist_info()
    {
      set(types::core_network_assist_info);
      return c.get<core_network_assist_info_s>();
    }
    guami_s& set_guami()
    {
      set(types::guami);
      return c.get<guami_s>();
    }
    pdu_session_res_setup_list_cxt_req_l& set_pdu_session_res_setup_list_cxt_req()
    {
      set(types::pdu_session_res_setup_list_cxt_req);
      return c.get<pdu_session_res_setup_list_cxt_req_l>();
    }
    allowed_nssai_l& set_allowed_nssai()
    {
      set(types::allowed_nssai);
      return c.get<allowed_nssai_l>();
    }
    ue_security_cap_s& set_ue_security_cap()
    {
      set(types::ue_security_cap);
      return c.get<ue_security_cap_s>();
    }
    fixed_bitstring<256, false, true>& set_security_key()
    {
      set(types::security_key);
      return c.get<fixed_bitstring<256, false, true> >();
    }
    trace_activation_s& set_trace_activation()
    {
      set(types::trace_activation);
      return c.get<trace_activation_s>();
    }
    mob_restrict_list_s& set_mob_restrict_list()
    {
      set(types::mob_restrict_list);
      return c.get<mob_restrict_list_s>();
    }
    unbounded_octstring<true>& set_ue_radio_cap()
    {
      set(types::ue_radio_cap);
      return c.get<unbounded_octstring<true> >();
    }
    uint16_t& set_idx_to_rfsp()
    {
      set(types::idx_to_rfsp);
      return c.get<uint16_t>();
    }
    fixed_bitstring<64, false, true>& set_masked_imeisv()
    {
      set(types::masked_imeisv);
      return c.get<fixed_bitstring<64, false, true> >();
    }
    unbounded_octstring<true>& set_nas_pdu()
    {
      set(types::nas_pdu);
      return c.get<unbounded_octstring<true> >();
    }
    emergency_fallback_ind_s& set_emergency_fallback_ind()
    {
      set(types::emergency_fallback_ind);
      return c.get<emergency_fallback_ind_s>();
    }
    rrc_inactive_transition_report_request_e& set_rrc_inactive_transition_report_request()
    {
      set(types::rrc_inactive_transition_report_request);
      return c.get<rrc_inactive_transition_report_request_e>();
    }
    ue_radio_cap_for_paging_s& set_ue_radio_cap_for_paging()
    {
      set(types::ue_radio_cap_for_paging);
      return c.get<ue_radio_cap_for_paging_s>();
    }
    redirection_voice_fallback_e& set_redirection_voice_fallback()
    {
      set(types::redirection_voice_fallback);
      return c.get<redirection_voice_fallback_e>();
    }

  private:
    types type_;
    choice_buffer_t<allowed_nssai_l,
                    core_network_assist_info_s,
                    emergency_fallback_ind_s,
                    fixed_bitstring<256, false, true>,
                    guami_s,
                    mob_restrict_list_s,
                    pdu_session_res_setup_list_cxt_req_l,
                    printable_string<1, 150, true, true>,
                    trace_activation_s,
                    ue_aggregate_maximum_bit_rate_s,
                    ue_radio_cap_for_paging_s,
                    ue_security_cap_s,
                    unbounded_octstring<true> >
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// InitialContextSetupRequest ::= SEQUENCE
struct init_context_setup_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool old_amf_present                                = false;
    bool ue_aggregate_maximum_bit_rate_present          = false;
    bool core_network_assist_info_present               = false;
    bool pdu_session_res_setup_list_cxt_req_present     = false;
    bool trace_activation_present                       = false;
    bool mob_restrict_list_present                      = false;
    bool ue_radio_cap_present                           = false;
    bool idx_to_rfsp_present                            = false;
    bool masked_imeisv_present                          = false;
    bool nas_pdu_present                                = false;
    bool emergency_fallback_ind_present                 = false;
    bool rrc_inactive_transition_report_request_present = false;
    bool ue_radio_cap_for_paging_present                = false;
    bool redirection_voice_fallback_present             = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> >               amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >                  ran_ue_ngap_id;
    ie_field_s<printable_string<1, 150, true, true> >                           old_amf;
    ie_field_s<ue_aggregate_maximum_bit_rate_s>                                 ue_aggregate_maximum_bit_rate;
    ie_field_s<core_network_assist_info_s>                                      core_network_assist_info;
    ie_field_s<guami_s>                                                         guami;
    ie_field_s<dyn_seq_of<pdu_session_res_setup_item_cxt_req_s, 1, 256, true> > pdu_session_res_setup_list_cxt_req;
    ie_field_s<dyn_seq_of<allowed_nssai_item_s, 1, 8, true> >                   allowed_nssai;
    ie_field_s<ue_security_cap_s>                                               ue_security_cap;
    ie_field_s<fixed_bitstring<256, false, true> >                              security_key;
    ie_field_s<trace_activation_s>                                              trace_activation;
    ie_field_s<mob_restrict_list_s>                                             mob_restrict_list;
    ie_field_s<unbounded_octstring<true> >                                      ue_radio_cap;
    ie_field_s<integer<uint16_t, 1, 256, false, true> >                         idx_to_rfsp;
    ie_field_s<fixed_bitstring<64, false, true> >                               masked_imeisv;
    ie_field_s<unbounded_octstring<true> >                                      nas_pdu;
    ie_field_s<emergency_fallback_ind_s>                                        emergency_fallback_ind;
    ie_field_s<rrc_inactive_transition_report_request_e>                        rrc_inactive_transition_report_request;
    ie_field_s<ue_radio_cap_for_paging_s>                                       ue_radio_cap_for_paging;
    ie_field_s<redirection_voice_fallback_e>                                    redirection_voice_fallback;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupResponseTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_setup_resp_transfer_ext_ies_o;

// PDUSessionResourceFailedToSetupItemCxtRes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_failed_to_setup_item_cxt_res_ext_ies_o;

// PDUSessionResourceSetupItemCxtRes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_setup_item_cxt_res_ext_ies_o;

// PDUSessionResourceSetupResponseTransfer ::= SEQUENCE
struct pdu_session_res_setup_resp_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                         ext                                   = false;
  bool                         add_dl_qos_flow_per_tnl_info_present  = false;
  bool                         security_result_present               = false;
  bool                         qos_flow_failed_to_setup_list_present = false;
  bool                         ie_exts_present                       = false;
  qos_flow_per_tnl_info_s      dlqos_flow_per_tnl_info;
  qos_flow_per_tnl_info_list_l add_dl_qos_flow_per_tnl_info;
  security_result_s            security_result;
  qos_flow_list_with_cause_l   qos_flow_failed_to_setup_list;
  ie_exts_l_                   ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToSetupItemCxtRes ::= SEQUENCE
struct pdu_session_res_failed_to_setup_item_cxt_res_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_setup_unsuccessful_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupItemCxtRes ::= SEQUENCE
struct pdu_session_res_setup_item_cxt_res_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_setup_resp_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToSetupListCxtRes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToSetupItemCxtRes
using pdu_session_res_failed_to_setup_list_cxt_res_l = dyn_array<pdu_session_res_failed_to_setup_item_cxt_res_s>;

// PDUSessionResourceSetupListCxtRes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSetupItemCxtRes
using pdu_session_res_setup_list_cxt_res_l = dyn_array<pdu_session_res_setup_item_cxt_res_s>;

// InitialContextSetupResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct init_context_setup_resp_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_setup_list_cxt_res,
        pdu_session_res_failed_to_setup_list_cxt_res,
        crit_diagnostics,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    pdu_session_res_setup_list_cxt_res_l& pdu_session_res_setup_list_cxt_res()
    {
      assert_choice_type("PDUSessionResourceSetupListCxtRes", type_.to_string(), "Value");
      return c.get<pdu_session_res_setup_list_cxt_res_l>();
    }
    pdu_session_res_failed_to_setup_list_cxt_res_l& pdu_session_res_failed_to_setup_list_cxt_res()
    {
      assert_choice_type("PDUSessionResourceFailedToSetupListCxtRes", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_setup_list_cxt_res_l>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const pdu_session_res_setup_list_cxt_res_l& pdu_session_res_setup_list_cxt_res() const
    {
      assert_choice_type("PDUSessionResourceSetupListCxtRes", type_.to_string(), "Value");
      return c.get<pdu_session_res_setup_list_cxt_res_l>();
    }
    const pdu_session_res_failed_to_setup_list_cxt_res_l& pdu_session_res_failed_to_setup_list_cxt_res() const
    {
      assert_choice_type("PDUSessionResourceFailedToSetupListCxtRes", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_setup_list_cxt_res_l>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    pdu_session_res_setup_list_cxt_res_l& set_pdu_session_res_setup_list_cxt_res()
    {
      set(types::pdu_session_res_setup_list_cxt_res);
      return c.get<pdu_session_res_setup_list_cxt_res_l>();
    }
    pdu_session_res_failed_to_setup_list_cxt_res_l& set_pdu_session_res_failed_to_setup_list_cxt_res()
    {
      set(types::pdu_session_res_failed_to_setup_list_cxt_res);
      return c.get<pdu_session_res_failed_to_setup_list_cxt_res_l>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    pdu_session_res_failed_to_setup_list_cxt_res_l,
                    pdu_session_res_setup_list_cxt_res_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// InitialContextSetupResponse ::= SEQUENCE
struct init_context_setup_resp_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool pdu_session_res_setup_list_cxt_res_present           = false;
    bool pdu_session_res_failed_to_setup_list_cxt_res_present = false;
    bool crit_diagnostics_present                             = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> >               amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >                  ran_ue_ngap_id;
    ie_field_s<dyn_seq_of<pdu_session_res_setup_item_cxt_res_s, 1, 256, true> > pdu_session_res_setup_list_cxt_res;
    ie_field_s<dyn_seq_of<pdu_session_res_failed_to_setup_item_cxt_res_s, 1, 256, true> >
                                   pdu_session_res_failed_to_setup_list_cxt_res;
    ie_field_s<crit_diagnostics_s> crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCEstablishmentCause ::= ENUMERATED
struct rrcestablishment_cause_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    mo_voice_call,
    mo_video_call,
    mo_sms,
    mps_prio_access,
    mcs_prio_access,
    // ...
    not_available,
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<rrcestablishment_cause_opts, true, 1> rrcestablishment_cause_e;

// UEContextRequest ::= ENUMERATED
struct ue_context_request_opts {
  enum options { requested, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ue_context_request_opts, true> ue_context_request_e;

// InitialUEMessage-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct init_ue_msg_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ran_ue_ngap_id,
        nas_pdu,
        user_location_info,
        rrcestablishment_cause,
        five_g_s_tmsi,
        amf_set_id,
        ue_context_request,
        allowed_nssai,
        nulltype
      } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& nas_pdu()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    user_location_info_c& user_location_info()
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    rrcestablishment_cause_e& rrcestablishment_cause()
    {
      assert_choice_type("RRCEstablishmentCause", type_.to_string(), "Value");
      return c.get<rrcestablishment_cause_e>();
    }
    five_g_s_tmsi_s& five_g_s_tmsi()
    {
      assert_choice_type("FiveG-S-TMSI", type_.to_string(), "Value");
      return c.get<five_g_s_tmsi_s>();
    }
    fixed_bitstring<10, false, true>& amf_set_id()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<10, false, true> >();
    }
    ue_context_request_e& ue_context_request()
    {
      assert_choice_type("UEContextRequest", type_.to_string(), "Value");
      return c.get<ue_context_request_e>();
    }
    allowed_nssai_l& allowed_nssai()
    {
      assert_choice_type("AllowedNSSAI", type_.to_string(), "Value");
      return c.get<allowed_nssai_l>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const unbounded_octstring<true>& nas_pdu() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const user_location_info_c& user_location_info() const
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    const rrcestablishment_cause_e& rrcestablishment_cause() const
    {
      assert_choice_type("RRCEstablishmentCause", type_.to_string(), "Value");
      return c.get<rrcestablishment_cause_e>();
    }
    const five_g_s_tmsi_s& five_g_s_tmsi() const
    {
      assert_choice_type("FiveG-S-TMSI", type_.to_string(), "Value");
      return c.get<five_g_s_tmsi_s>();
    }
    const fixed_bitstring<10, false, true>& amf_set_id() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<10, false, true> >();
    }
    const ue_context_request_e& ue_context_request() const
    {
      assert_choice_type("UEContextRequest", type_.to_string(), "Value");
      return c.get<ue_context_request_e>();
    }
    const allowed_nssai_l& allowed_nssai() const
    {
      assert_choice_type("AllowedNSSAI", type_.to_string(), "Value");
      return c.get<allowed_nssai_l>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& set_nas_pdu()
    {
      set(types::nas_pdu);
      return c.get<unbounded_octstring<true> >();
    }
    user_location_info_c& set_user_location_info()
    {
      set(types::user_location_info);
      return c.get<user_location_info_c>();
    }
    rrcestablishment_cause_e& set_rrcestablishment_cause()
    {
      set(types::rrcestablishment_cause);
      return c.get<rrcestablishment_cause_e>();
    }
    five_g_s_tmsi_s& set_five_g_s_tmsi()
    {
      set(types::five_g_s_tmsi);
      return c.get<five_g_s_tmsi_s>();
    }
    fixed_bitstring<10, false, true>& set_amf_set_id()
    {
      set(types::amf_set_id);
      return c.get<fixed_bitstring<10, false, true> >();
    }
    ue_context_request_e& set_ue_context_request()
    {
      set(types::ue_context_request);
      return c.get<ue_context_request_e>();
    }
    allowed_nssai_l& set_allowed_nssai()
    {
      set(types::allowed_nssai);
      return c.get<allowed_nssai_l>();
    }

  private:
    types type_;
    choice_buffer_t<allowed_nssai_l,
                    five_g_s_tmsi_s,
                    fixed_bitstring<10, false, true>,
                    unbounded_octstring<true>,
                    user_location_info_c>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// InitialUEMessage ::= SEQUENCE
struct init_ue_msg_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                       five_g_s_tmsi_present      = false;
    bool                                                       amf_set_id_present         = false;
    bool                                                       ue_context_request_present = false;
    bool                                                       allowed_nssai_present      = false;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > ran_ue_ngap_id;
    ie_field_s<unbounded_octstring<true> >                     nas_pdu;
    ie_field_s<user_location_info_c>                           user_location_info;
    ie_field_s<rrcestablishment_cause_e>                       rrcestablishment_cause;
    ie_field_s<five_g_s_tmsi_s>                                five_g_s_tmsi;
    ie_field_s<fixed_bitstring<10, false, true> >              amf_set_id;
    ie_field_s<ue_context_request_e>                           ue_context_request;
    ie_field_s<dyn_seq_of<allowed_nssai_item_s, 1, 8, true> >  allowed_nssai;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VolumeTimedReport-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o volume_timed_report_item_ext_ies_o;

// VolumeTimedReport-Item ::= SEQUENCE
struct volume_timed_report_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<4, true> start_time_stamp;
  fixed_octstring<4, true> end_time_stamp;
  uint64_t                 usage_count_ul = 0;
  uint64_t                 usage_count_dl = 0;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoSFlowsUsageReport-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qo_sflows_usage_report_item_ext_ies_o;

// VolumeTimedReportList ::= SEQUENCE (SIZE (1..2)) OF VolumeTimedReport-Item
using volume_timed_report_list_l = dyn_array<volume_timed_report_item_s>;

// PDUSessionUsageReport-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_usage_report_ext_ies_o;

// QoSFlowsUsageReport-Item ::= SEQUENCE
struct qo_sflows_usage_report_item_s {
  struct rat_type_opts {
    enum options { nr, eutra, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<rat_type_opts, true> rat_type_e_;
  typedef protocol_ext_container_empty_l  ie_exts_l_;

  // member variables
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  uint8_t                    qos_flow_id     = 0;
  rat_type_e_                rat_type;
  volume_timed_report_list_l qo_sflows_timed_report_list;
  ie_exts_l_                 ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionUsageReport ::= SEQUENCE
struct pdu_session_usage_report_s {
  struct rat_type_opts {
    enum options { nr, eutra, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<rat_type_opts, true> rat_type_e_;
  typedef protocol_ext_container_empty_l  ie_exts_l_;

  // member variables
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  rat_type_e_                rat_type;
  volume_timed_report_list_l pdu_session_timed_report_list;
  ie_exts_l_                 ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoSFlowsUsageReportList ::= SEQUENCE (SIZE (1..64)) OF QoSFlowsUsageReport-Item
using qo_sflows_usage_report_list_l = dyn_array<qo_sflows_usage_report_item_s>;

// QosFlowAddOrModifyRequestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_add_or_modify_request_item_ext_ies_o;

// SecondaryRATUsageInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o secondary_ratusage_info_ext_ies_o;

// UL-NGU-UP-TNLModifyItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o ul_ngu_up_tnl_modify_item_ext_ies_o;

// UPTransportLayerInformationPairItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o up_transport_layer_info_pair_item_ext_ies_o;

// NotificationCause ::= ENUMERATED
struct notif_cause_opts {
  enum options { fulfilled, not_fulfilled, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<notif_cause_opts, true> notif_cause_e;

// QosFlowAcceptedItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_accepted_item_ext_ies_o;

// QosFlowAddOrModifyRequestItem ::= SEQUENCE
struct qos_flow_add_or_modify_request_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                        ext                               = false;
  bool                        qos_flow_level_qos_params_present = false;
  bool                        e_rab_id_present                  = false;
  bool                        ie_exts_present                   = false;
  uint8_t                     qos_flow_id                       = 0;
  qos_flow_level_qos_params_s qos_flow_level_qos_params;
  uint8_t                     e_rab_id = 0;
  ie_exts_l_                  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowAddOrModifyResponseItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_add_or_modify_resp_item_ext_ies_o;

// QosFlowModifyConfirmItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_modify_confirm_item_ext_ies_o;

// QosFlowNotifyItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_notify_item_ext_ies_o;

// SecondaryRATUsageInformation ::= SEQUENCE
struct secondary_ratusage_info_s {
  typedef protocol_ext_container_empty_l ie_ext_l_;

  // member variables
  bool                          ext                                 = false;
  bool                          pdu_session_usage_report_present    = false;
  bool                          qos_flows_usage_report_list_present = false;
  bool                          ie_ext_present                      = false;
  pdu_session_usage_report_s    pdu_session_usage_report;
  qo_sflows_usage_report_list_l qos_flows_usage_report_list;
  ie_ext_l_                     ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-NGU-UP-TNLModifyItem ::= SEQUENCE
struct ul_ngu_up_tnl_modify_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  up_transport_layer_info_c ul_ngu_up_tnl_info;
  up_transport_layer_info_c dl_ngu_up_tnl_info;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UPTransportLayerInformationPairItem ::= SEQUENCE
struct up_transport_layer_info_pair_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  up_transport_layer_info_c ul_ngu_up_tnl_info;
  up_transport_layer_info_c dl_ngu_up_tnl_info;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleaseResponseTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_release_resp_transfer_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { secondary_ratusage_info, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::secondary_ratusage_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    secondary_ratusage_info_s&       secondary_ratusage_info() { return c; }
    const secondary_ratusage_info_s& secondary_ratusage_info() const { return c; }

  private:
    secondary_ratusage_info_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// QosFlowAcceptedItem ::= SEQUENCE
struct qos_flow_accepted_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  uint8_t    qos_flow_id     = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowAddOrModifyRequestList ::= SEQUENCE (SIZE (1..64)) OF QosFlowAddOrModifyRequestItem
using qos_flow_add_or_modify_request_list_l = dyn_array<qos_flow_add_or_modify_request_item_s>;

// QosFlowAddOrModifyResponseItem ::= SEQUENCE
struct qos_flow_add_or_modify_resp_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  uint8_t    qos_flow_id     = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowModifyConfirmItem ::= SEQUENCE
struct qos_flow_modify_confirm_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  uint8_t    qos_flow_id     = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowNotifyItem ::= SEQUENCE
struct qos_flow_notify_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool          ext             = false;
  bool          ie_exts_present = false;
  uint8_t       qos_flow_id     = 0;
  notif_cause_e notif_cause;
  ie_exts_l_    ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SliceOverloadItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o slice_overload_item_ext_ies_o;

// UL-NGU-UP-TNLModifyList ::= SEQUENCE (SIZE (1..4)) OF UL-NGU-UP-TNLModifyItem
using ul_ngu_up_tnl_modify_list_l = dyn_array<ul_ngu_up_tnl_modify_item_s>;

// UPTransportLayerInformationPairList ::= SEQUENCE (SIZE (1..3)) OF UPTransportLayerInformationPairItem
using up_transport_layer_info_pair_list_l = dyn_array<up_transport_layer_info_pair_item_s>;

// UserPlaneSecurityInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o user_plane_security_info_ext_ies_o;

// DL-NGU-TNLInformationReused ::= ENUMERATED
struct dl_ngu_tnl_info_reused_opts {
  enum options { true_value, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<dl_ngu_tnl_info_reused_opts, true> dl_ngu_tnl_info_reused_e;

// OverloadAction ::= ENUMERATED
struct overload_action_opts {
  enum options {
    reject_non_emergency_mo_dt,
    reject_rrc_cr_sig,
    permit_emergency_sessions_and_mobile_terminated_services_only,
    permit_high_prio_sessions_and_mobile_terminated_services_only,
    // ...
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<overload_action_opts, true> overload_action_e;

// OverloadResponse-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o overload_resp_ext_ies_o;

// PDUSessionResourceModifyConfirmTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_modify_confirm_transfer_ext_ies_o;

// PDUSessionResourceModifyIndicationTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_modify_ind_transfer_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { secondary_ratusage_info, security_result, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    secondary_ratusage_info_s& secondary_ratusage_info()
    {
      assert_choice_type("SecondaryRATUsageInformation", type_.to_string(), "Extension");
      return c.get<secondary_ratusage_info_s>();
    }
    security_result_s& security_result()
    {
      assert_choice_type("SecurityResult", type_.to_string(), "Extension");
      return c.get<security_result_s>();
    }
    const secondary_ratusage_info_s& secondary_ratusage_info() const
    {
      assert_choice_type("SecondaryRATUsageInformation", type_.to_string(), "Extension");
      return c.get<secondary_ratusage_info_s>();
    }
    const security_result_s& security_result() const
    {
      assert_choice_type("SecurityResult", type_.to_string(), "Extension");
      return c.get<security_result_s>();
    }
    secondary_ratusage_info_s& set_secondary_ratusage_info()
    {
      set(types::secondary_ratusage_info);
      return c.get<secondary_ratusage_info_s>();
    }
    security_result_s& set_security_result()
    {
      set(types::security_result);
      return c.get<security_result_s>();
    }

  private:
    types                                                         type_;
    choice_buffer_t<secondary_ratusage_info_s, security_result_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceModifyIndicationUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_modify_ind_unsuccessful_transfer_ext_ies_o;

// PDUSessionResourceModifyRequestTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_modify_request_transfer_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        pdu_session_aggregate_maximum_bit_rate,
        ul_ngu_up_tnl_modify_list,
        network_instance,
        qos_flow_add_or_modify_request_list,
        qos_flow_to_release_list,
        add_ul_ngu_up_tnl_info,
        nulltype
      } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pdu_session_aggregate_maximum_bit_rate_s& pdu_session_aggregate_maximum_bit_rate()
    {
      assert_choice_type("PDUSessionAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<pdu_session_aggregate_maximum_bit_rate_s>();
    }
    ul_ngu_up_tnl_modify_list_l& ul_ngu_up_tnl_modify_list()
    {
      assert_choice_type("UL-NGU-UP-TNLModifyList", type_.to_string(), "Value");
      return c.get<ul_ngu_up_tnl_modify_list_l>();
    }
    uint16_t& network_instance()
    {
      assert_choice_type("INTEGER (1..256,...)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    qos_flow_add_or_modify_request_list_l& qos_flow_add_or_modify_request_list()
    {
      assert_choice_type("QosFlowAddOrModifyRequestList", type_.to_string(), "Value");
      return c.get<qos_flow_add_or_modify_request_list_l>();
    }
    qos_flow_list_with_cause_l& qos_flow_to_release_list()
    {
      assert_choice_type("QosFlowListWithCause", type_.to_string(), "Value");
      return c.get<qos_flow_list_with_cause_l>();
    }
    up_transport_layer_info_list_l& add_ul_ngu_up_tnl_info()
    {
      assert_choice_type("UPTransportLayerInformationList", type_.to_string(), "Value");
      return c.get<up_transport_layer_info_list_l>();
    }
    const pdu_session_aggregate_maximum_bit_rate_s& pdu_session_aggregate_maximum_bit_rate() const
    {
      assert_choice_type("PDUSessionAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<pdu_session_aggregate_maximum_bit_rate_s>();
    }
    const ul_ngu_up_tnl_modify_list_l& ul_ngu_up_tnl_modify_list() const
    {
      assert_choice_type("UL-NGU-UP-TNLModifyList", type_.to_string(), "Value");
      return c.get<ul_ngu_up_tnl_modify_list_l>();
    }
    const uint16_t& network_instance() const
    {
      assert_choice_type("INTEGER (1..256,...)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    const qos_flow_add_or_modify_request_list_l& qos_flow_add_or_modify_request_list() const
    {
      assert_choice_type("QosFlowAddOrModifyRequestList", type_.to_string(), "Value");
      return c.get<qos_flow_add_or_modify_request_list_l>();
    }
    const qos_flow_list_with_cause_l& qos_flow_to_release_list() const
    {
      assert_choice_type("QosFlowListWithCause", type_.to_string(), "Value");
      return c.get<qos_flow_list_with_cause_l>();
    }
    const up_transport_layer_info_list_l& add_ul_ngu_up_tnl_info() const
    {
      assert_choice_type("UPTransportLayerInformationList", type_.to_string(), "Value");
      return c.get<up_transport_layer_info_list_l>();
    }
    pdu_session_aggregate_maximum_bit_rate_s& set_pdu_session_aggregate_maximum_bit_rate()
    {
      set(types::pdu_session_aggregate_maximum_bit_rate);
      return c.get<pdu_session_aggregate_maximum_bit_rate_s>();
    }
    ul_ngu_up_tnl_modify_list_l& set_ul_ngu_up_tnl_modify_list()
    {
      set(types::ul_ngu_up_tnl_modify_list);
      return c.get<ul_ngu_up_tnl_modify_list_l>();
    }
    uint16_t& set_network_instance()
    {
      set(types::network_instance);
      return c.get<uint16_t>();
    }
    qos_flow_add_or_modify_request_list_l& set_qos_flow_add_or_modify_request_list()
    {
      set(types::qos_flow_add_or_modify_request_list);
      return c.get<qos_flow_add_or_modify_request_list_l>();
    }
    qos_flow_list_with_cause_l& set_qos_flow_to_release_list()
    {
      set(types::qos_flow_to_release_list);
      return c.get<qos_flow_list_with_cause_l>();
    }
    up_transport_layer_info_list_l& set_add_ul_ngu_up_tnl_info()
    {
      set(types::add_ul_ngu_up_tnl_info);
      return c.get<up_transport_layer_info_list_l>();
    }

  private:
    types type_;
    choice_buffer_t<pdu_session_aggregate_maximum_bit_rate_s,
                    qos_flow_add_or_modify_request_list_l,
                    qos_flow_list_with_cause_l,
                    ul_ngu_up_tnl_modify_list_l,
                    up_transport_layer_info_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceModifyResponseTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_modify_resp_transfer_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { add_ngu_up_tnl_info, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::add_ngu_up_tnl_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_transport_layer_info_pair_list_l&       add_ngu_up_tnl_info() { return c; }
    const up_transport_layer_info_pair_list_l& add_ngu_up_tnl_info() const { return c; }

  private:
    up_transport_layer_info_pair_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceModifyUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_modify_unsuccessful_transfer_ext_ies_o;

// PDUSessionResourceNotifyReleasedTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_notify_released_transfer_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { secondary_ratusage_info, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::secondary_ratusage_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    secondary_ratusage_info_s&       secondary_ratusage_info() { return c; }
    const secondary_ratusage_info_s& secondary_ratusage_info() const { return c; }

  private:
    secondary_ratusage_info_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceNotifyTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_notify_transfer_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { secondary_ratusage_info, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::secondary_ratusage_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    secondary_ratusage_info_s&       secondary_ratusage_info() { return c; }
    const secondary_ratusage_info_s& secondary_ratusage_info() const { return c; }

  private:
    secondary_ratusage_info_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceReleaseCommandTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_release_cmd_transfer_ext_ies_o;

// PDUSessionResourceReleaseResponseTransfer ::= SEQUENCE
struct pdu_session_res_release_resp_transfer_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                                  secondary_ratusage_info_present = false;
    ie_field_s<secondary_ratusage_info_s> secondary_ratusage_info;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestAcknowledgeTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct path_switch_request_ack_transfer_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { add_ngu_up_tnl_info, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::add_ngu_up_tnl_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_transport_layer_info_pair_list_l&       add_ngu_up_tnl_info() { return c; }
    const up_transport_layer_info_pair_list_l& add_ngu_up_tnl_info() const { return c; }

  private:
    up_transport_layer_info_pair_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PathSwitchRequestSetupFailedTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o path_switch_request_setup_failed_transfer_ext_ies_o;

// PathSwitchRequestTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct path_switch_request_transfer_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { add_dl_qos_flow_per_tnl_info, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::add_dl_qos_flow_per_tnl_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_per_tnl_info_list_l&       add_dl_qos_flow_per_tnl_info() { return c; }
    const qos_flow_per_tnl_info_list_l& add_dl_qos_flow_per_tnl_info() const { return c; }

  private:
    qos_flow_per_tnl_info_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PathSwitchRequestUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o path_switch_request_unsuccessful_transfer_ext_ies_o;

// QosFlowAcceptedList ::= SEQUENCE (SIZE (1..64)) OF QosFlowAcceptedItem
using qos_flow_accepted_list_l = dyn_array<qos_flow_accepted_item_s>;

// QosFlowAddOrModifyResponseList ::= SEQUENCE (SIZE (1..64)) OF QosFlowAddOrModifyResponseItem
using qos_flow_add_or_modify_resp_list_l = dyn_array<qos_flow_add_or_modify_resp_item_s>;

// QosFlowModifyConfirmList ::= SEQUENCE (SIZE (1..64)) OF QosFlowModifyConfirmItem
using qos_flow_modify_confirm_list_l = dyn_array<qos_flow_modify_confirm_item_s>;

// QosFlowNotifyList ::= SEQUENCE (SIZE (1..64)) OF QosFlowNotifyItem
using qos_flow_notify_list_l = dyn_array<qos_flow_notify_item_s>;

// SecondaryRATDataUsageReportTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o secondary_rat_data_usage_report_transfer_ext_ies_o;

// SliceOverloadItem ::= SEQUENCE
struct slice_overload_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  s_nssai_s  s_nssai;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-associatedLogicalNG-connectionItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o ue_associated_lc_ng_conn_item_ext_ies_o;

// UserPlaneSecurityInformation ::= SEQUENCE
struct user_plane_security_info_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool              ext             = false;
  bool              ie_exts_present = false;
  security_result_s security_result;
  security_ind_s    security_ind;
  ie_exts_l_        ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OverloadResponse ::= CHOICE
struct overload_resp_c {
  struct types_opts {
    enum options { overload_action, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  overload_resp_c() = default;
  overload_resp_c(const overload_resp_c& other);
  overload_resp_c& operator=(const overload_resp_c& other);
  ~overload_resp_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  overload_action_e& overload_action()
  {
    assert_choice_type("overloadAction", type_.to_string(), "OverloadResponse");
    return c.get<overload_action_e>();
  }
  protocol_ie_single_container_s<overload_resp_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "OverloadResponse");
    return c.get<protocol_ie_single_container_s<overload_resp_ext_ies_o> >();
  }
  const overload_action_e& overload_action() const
  {
    assert_choice_type("overloadAction", type_.to_string(), "OverloadResponse");
    return c.get<overload_action_e>();
  }
  const protocol_ie_single_container_s<overload_resp_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "OverloadResponse");
    return c.get<protocol_ie_single_container_s<overload_resp_ext_ies_o> >();
  }
  overload_action_e& set_overload_action()
  {
    set(types::overload_action);
    return c.get<overload_action_e>();
  }
  protocol_ie_single_container_s<overload_resp_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<overload_resp_ext_ies_o> >();
  }

private:
  types                                                                     type_;
  choice_buffer_t<protocol_ie_single_container_s<overload_resp_ext_ies_o> > c;

  void destroy_();
};

// OverloadStartNSSAIItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o overload_start_nssai_item_ext_ies_o;

// PDUSessionResourceFailedToModifyItemModCfm-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_failed_to_modify_item_mod_cfm_ext_ies_o;

// PDUSessionResourceFailedToModifyItemModRes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_failed_to_modify_item_mod_res_ext_ies_o;

// PDUSessionResourceFailedToSetupItemPSReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_failed_to_setup_item_ps_req_ext_ies_o;

// PDUSessionResourceFailedToSetupItemSURes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_failed_to_setup_item_su_res_ext_ies_o;

// PDUSessionResourceItemCxtRelCpl-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_item_cxt_rel_cpl_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { pdu_session_res_release_resp_transfer, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::pdu_session_res_release_resp_transfer; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>&       pdu_session_res_release_resp_transfer() { return c; }
    const unbounded_octstring<true>& pdu_session_res_release_resp_transfer() const { return c; }

  private:
    unbounded_octstring<true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceItemCxtRelReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_item_cxt_rel_req_ext_ies_o;

// PDUSessionResourceModifyConfirmTransfer ::= SEQUENCE
struct pdu_session_res_modify_confirm_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                                ext                                    = false;
  bool                                add_ng_uuptnl_info_present             = false;
  bool                                qos_flow_failed_to_modify_list_present = false;
  bool                                ie_exts_present                        = false;
  qos_flow_modify_confirm_list_l      qos_flow_modify_confirm_list;
  up_transport_layer_info_c           ulngu_up_tnl_info;
  up_transport_layer_info_pair_list_l add_ng_uuptnl_info;
  qos_flow_list_with_cause_l          qos_flow_failed_to_modify_list;
  ie_exts_l_                          ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyIndicationTransfer ::= SEQUENCE
struct pdu_session_res_modify_ind_transfer_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                                  secondary_ratusage_info_present = false;
    bool                                  security_result_present         = false;
    ie_field_s<secondary_ratusage_info_s> secondary_ratusage_info;
    ie_field_s<security_result_s>         security_result;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                         ext                                  = false;
  bool                         add_dl_qos_flow_per_tnl_info_present = false;
  bool                         ie_exts_present                      = false;
  qos_flow_per_tnl_info_s      dlqos_flow_per_tnl_info;
  qos_flow_per_tnl_info_list_l add_dl_qos_flow_per_tnl_info;
  ie_exts_l_                   ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyIndicationUnsuccessfulTransfer ::= SEQUENCE
struct pdu_session_res_modify_ind_unsuccessful_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  cause_c    cause;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyItemModCfm-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_modify_item_mod_cfm_ext_ies_o;

// PDUSessionResourceModifyItemModInd-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_modify_item_mod_ind_ext_ies_o;

// PDUSessionResourceModifyItemModReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_modify_item_mod_req_ext_ies_o {
  // Extension ::= CLASS OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { s_nssai, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::s_nssai; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    s_nssai_s&       s_nssai() { return c; }
    const s_nssai_s& s_nssai() const { return c; }

  private:
    s_nssai_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceModifyItemModRes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_modify_item_mod_res_ext_ies_o;

// PDUSessionResourceModifyRequestTransfer ::= SEQUENCE
struct pdu_session_res_modify_request_transfer_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                 pdu_session_aggregate_maximum_bit_rate_present = false;
    bool                                                 ul_ngu_up_tnl_modify_list_present              = false;
    bool                                                 network_instance_present                       = false;
    bool                                                 qos_flow_add_or_modify_request_list_present    = false;
    bool                                                 qos_flow_to_release_list_present               = false;
    bool                                                 add_ul_ngu_up_tnl_info_present                 = false;
    ie_field_s<pdu_session_aggregate_maximum_bit_rate_s> pdu_session_aggregate_maximum_bit_rate;
    ie_field_s<dyn_seq_of<ul_ngu_up_tnl_modify_item_s, 1, 4, true> >            ul_ngu_up_tnl_modify_list;
    ie_field_s<integer<uint16_t, 1, 256, false, true> >                         network_instance;
    ie_field_s<dyn_seq_of<qos_flow_add_or_modify_request_item_s, 1, 64, true> > qos_flow_add_or_modify_request_list;
    ie_field_s<dyn_seq_of<qos_flow_with_cause_item_s, 1, 64, true> >            qos_flow_to_release_list;
    ie_field_s<dyn_seq_of<up_transport_layer_info_item_s, 1, 3, true> >         add_ul_ngu_up_tnl_info;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyResponseTransfer ::= SEQUENCE
struct pdu_session_res_modify_resp_transfer_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                                                                     add_ngu_up_tnl_info_present = false;
    ie_field_s<dyn_seq_of<up_transport_layer_info_pair_item_s, 1, 3, true> > add_ngu_up_tnl_info;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                               ext                                           = false;
  bool                               dl_ngu_up_tnl_info_present                    = false;
  bool                               ul_ngu_up_tnl_info_present                    = false;
  bool                               qos_flow_add_or_modify_resp_list_present      = false;
  bool                               add_dl_qos_flow_per_tnl_info_present          = false;
  bool                               qos_flow_failed_to_add_or_modify_list_present = false;
  bool                               ie_exts_present                               = false;
  up_transport_layer_info_c          dl_ngu_up_tnl_info;
  up_transport_layer_info_c          ul_ngu_up_tnl_info;
  qos_flow_add_or_modify_resp_list_l qos_flow_add_or_modify_resp_list;
  qos_flow_per_tnl_info_list_l       add_dl_qos_flow_per_tnl_info;
  qos_flow_list_with_cause_l         qos_flow_failed_to_add_or_modify_list;
  ie_exts_l_                         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyUnsuccessfulTransfer ::= SEQUENCE
struct pdu_session_res_modify_unsuccessful_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool               ext                      = false;
  bool               crit_diagnostics_present = false;
  bool               ie_exts_present          = false;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;
  ie_exts_l_         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceNotifyItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_notify_item_ext_ies_o;

// PDUSessionResourceNotifyReleasedTransfer ::= SEQUENCE
struct pdu_session_res_notify_released_transfer_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                                  secondary_ratusage_info_present = false;
    ie_field_s<secondary_ratusage_info_s> secondary_ratusage_info;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  cause_c    cause;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceNotifyTransfer ::= SEQUENCE
struct pdu_session_res_notify_transfer_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                                  secondary_ratusage_info_present = false;
    ie_field_s<secondary_ratusage_info_s> secondary_ratusage_info;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                       ext                            = false;
  bool                       qos_flow_notify_list_present   = false;
  bool                       qos_flow_released_list_present = false;
  bool                       ie_exts_present                = false;
  qos_flow_notify_list_l     qos_flow_notify_list;
  qos_flow_list_with_cause_l qos_flow_released_list;
  ie_exts_l_                 ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleaseCommandTransfer ::= SEQUENCE
struct pdu_session_res_release_cmd_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  cause_c    cause;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleasedItemNot-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_released_item_not_ext_ies_o;

// PDUSessionResourceReleasedItemPSAck-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_released_item_ps_ack_ext_ies_o;

// PDUSessionResourceReleasedItemPSFail-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_released_item_ps_fail_ext_ies_o;

// PDUSessionResourceReleasedItemRelRes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_released_item_rel_res_ext_ies_o;

// PDUSessionResourceSecondaryRATUsageItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_secondary_ratusage_item_ext_ies_o;

// PDUSessionResourceSetupItemSUReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_setup_item_su_req_ext_ies_o;

// PDUSessionResourceSetupItemSURes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_setup_item_su_res_ext_ies_o;

// PDUSessionResourceSwitchedItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_switched_item_ext_ies_o;

// PDUSessionResourceToBeSwitchedDLItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_to_be_switched_dl_item_ext_ies_o;

// PDUSessionResourceToReleaseItemRelCmd-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_to_release_item_rel_cmd_ext_ies_o;

// PathSwitchRequestAcknowledgeTransfer ::= SEQUENCE
struct path_switch_request_ack_transfer_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                                                                     add_ngu_up_tnl_info_present = false;
    ie_field_s<dyn_seq_of<up_transport_layer_info_pair_item_s, 1, 3, true> > add_ngu_up_tnl_info;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                      ext                        = false;
  bool                      ul_ngu_up_tnl_info_present = false;
  bool                      security_ind_present       = false;
  bool                      ie_exts_present            = false;
  up_transport_layer_info_c ul_ngu_up_tnl_info;
  security_ind_s            security_ind;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestSetupFailedTransfer ::= SEQUENCE
struct path_switch_request_setup_failed_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  cause_c    cause;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestTransfer ::= SEQUENCE
struct path_switch_request_transfer_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                                                              add_dl_qos_flow_per_tnl_info_present = false;
    ie_field_s<dyn_seq_of<qos_flow_per_tnl_info_item_s, 1, 3, true> > add_dl_qos_flow_per_tnl_info;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                       ext                              = false;
  bool                       dl_ngu_tnl_info_reused_present   = false;
  bool                       user_plane_security_info_present = false;
  bool                       ie_exts_present                  = false;
  up_transport_layer_info_c  dl_ngu_up_tnl_info;
  dl_ngu_tnl_info_reused_e   dl_ngu_tnl_info_reused;
  user_plane_security_info_s user_plane_security_info;
  qos_flow_accepted_list_l   qos_flow_accepted_list;
  ie_exts_l_                 ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestUnsuccessfulTransfer ::= SEQUENCE
struct path_switch_request_unsuccessful_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  cause_c    cause;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PrivateIE-ID ::= CHOICE
struct private_ie_id_c {
  struct types_opts {
    enum options { local, global, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  private_ie_id_c() = default;
  private_ie_id_c(const private_ie_id_c& other);
  private_ie_id_c& operator=(const private_ie_id_c& other);
  ~private_ie_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint32_t& local()
  {
    assert_choice_type("local", type_.to_string(), "PrivateIE-ID");
    return c.get<uint32_t>();
  }
  const uint32_t& local() const
  {
    assert_choice_type("local", type_.to_string(), "PrivateIE-ID");
    return c.get<uint32_t>();
  }
  uint32_t& set_local()
  {
    set(types::local);
    return c.get<uint32_t>();
  }

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// SecondaryRATDataUsageReportTransfer ::= SEQUENCE
struct secondary_rat_data_usage_report_transfer_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext                             = false;
  bool                      secondary_ratusage_info_present = false;
  bool                      ie_exts_present                 = false;
  secondary_ratusage_info_s secondary_ratusage_info;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SliceOverloadList ::= SEQUENCE (SIZE (1..1024)) OF SliceOverloadItem
using slice_overload_list_l = dyn_array<slice_overload_item_s>;

// SupportedTAItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o supported_ta_item_ext_ies_o;

// TAIListForPagingItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o tai_list_for_paging_item_ext_ies_o;

// UE-NGAP-ID-pair-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o ue_ngap_id_pair_ext_ies_o;

// UE-associatedLogicalNG-connectionItem ::= SEQUENCE
struct ue_associated_lc_ng_conn_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext                    = false;
  bool       amf_ue_ngap_id_present = false;
  bool       ran_ue_ngap_id_present = false;
  bool       ie_exts_present        = false;
  uint64_t   amf_ue_ngap_id         = 0;
  uint64_t   ran_ue_ngap_id         = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEPresence ::= ENUMERATED
struct ue_presence_opts {
  enum options { in, out, unknown, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ue_presence_opts, true> ue_presence_e;

// UEPresenceInAreaOfInterestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o ue_presence_in_area_of_interest_item_ext_ies_o;

// NR-CGIListForWarning ::= SEQUENCE (SIZE (1..65535)) OF NR-CGI
using nr_cgi_list_for_warning_l = dyn_array<nr_cgi_s>;

// OverloadStartNSSAIItem ::= SEQUENCE
struct overload_start_nssai_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                  ext                                      = false;
  bool                  slice_overload_resp_present              = false;
  bool                  slice_traffic_load_reduction_ind_present = false;
  bool                  ie_exts_present                          = false;
  slice_overload_list_l slice_overload_list;
  overload_resp_c       slice_overload_resp;
  uint8_t               slice_traffic_load_reduction_ind = 1;
  ie_exts_l_            ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToModifyItemModCfm ::= SEQUENCE
struct pdu_session_res_failed_to_modify_item_mod_cfm_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_modify_ind_unsuccessful_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToModifyItemModRes ::= SEQUENCE
struct pdu_session_res_failed_to_modify_item_mod_res_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_modify_unsuccessful_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToSetupItemPSReq ::= SEQUENCE
struct pdu_session_res_failed_to_setup_item_ps_req_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> path_switch_request_setup_failed_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToSetupItemSURes ::= SEQUENCE
struct pdu_session_res_failed_to_setup_item_su_res_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_setup_unsuccessful_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceItemCxtRelCpl ::= SEQUENCE
struct pdu_session_res_item_cxt_rel_cpl_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                                   pdu_session_res_release_resp_transfer_present = false;
    ie_field_s<unbounded_octstring<true> > pdu_session_res_release_resp_transfer;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  uint16_t   pdu_session_id  = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceItemCxtRelReq ::= SEQUENCE
struct pdu_session_res_item_cxt_rel_req_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  uint16_t   pdu_session_id  = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyItemModCfm ::= SEQUENCE
struct pdu_session_res_modify_item_mod_cfm_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_modify_confirm_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyItemModInd ::= SEQUENCE
struct pdu_session_res_modify_item_mod_ind_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_modify_ind_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyItemModReq ::= SEQUENCE
struct pdu_session_res_modify_item_mod_req_s {
  struct ie_exts_l_ {
    template <class extT_>
    using ie_field_s = protocol_ext_container_item_s<extT_>;

    // member variables
    bool                  s_nssai_present = false;
    ie_field_s<s_nssai_s> s_nssai;

    // sequence methods
    ie_exts_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                      ext             = false;
  bool                      nas_pdu_present = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> nas_pdu;
  unbounded_octstring<true> pdu_session_res_modify_request_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyItemModRes ::= SEQUENCE
struct pdu_session_res_modify_item_mod_res_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_modify_resp_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceNotifyItem ::= SEQUENCE
struct pdu_session_res_notify_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_notify_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleasedItemNot ::= SEQUENCE
struct pdu_session_res_released_item_not_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_notify_released_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleasedItemPSAck ::= SEQUENCE
struct pdu_session_res_released_item_ps_ack_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> path_switch_request_unsuccessful_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleasedItemPSFail ::= SEQUENCE
struct pdu_session_res_released_item_ps_fail_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> path_switch_request_unsuccessful_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleasedItemRelRes ::= SEQUENCE
struct pdu_session_res_released_item_rel_res_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_release_resp_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSecondaryRATUsageItem ::= SEQUENCE
struct pdu_session_res_secondary_ratusage_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> secondary_rat_data_usage_report_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupItemSUReq ::= SEQUENCE
struct pdu_session_res_setup_item_su_req_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext                         = false;
  bool                      pdu_session_nas_pdu_present = false;
  bool                      ie_exts_present             = false;
  uint16_t                  pdu_session_id              = 0;
  unbounded_octstring<true> pdu_session_nas_pdu;
  s_nssai_s                 s_nssai;
  unbounded_octstring<true> pdu_session_res_setup_request_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupItemSURes ::= SEQUENCE
struct pdu_session_res_setup_item_su_res_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_setup_resp_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSwitchedItem ::= SEQUENCE
struct pdu_session_res_switched_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> path_switch_request_ack_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceToBeSwitchedDLItem ::= SEQUENCE
struct pdu_session_res_to_be_switched_dl_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> path_switch_request_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceToReleaseItemRelCmd ::= SEQUENCE
struct pdu_session_res_to_release_item_rel_cmd_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint16_t                  pdu_session_id  = 0;
  unbounded_octstring<true> pdu_session_res_release_cmd_transfer;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSFailedCellIDList-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o pws_failed_cell_id_list_ext_ies_o;

// ResetAll ::= ENUMERATED
struct reset_all_opts {
  enum options { reset_all, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<reset_all_opts, true> reset_all_e;

// ResetType-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o reset_type_ext_ies_o;

// SupportedTAItem ::= SEQUENCE
struct supported_ta_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> tac;
  broadcast_plmn_list_l    broadcast_plmn_list;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAIListForPagingItem ::= SEQUENCE
struct tai_list_for_paging_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  tai_s      tai;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAIListForWarning ::= SEQUENCE (SIZE (1..65535)) OF TAI
using tai_list_for_warning_l = dyn_array<tai_s>;

// UE-NGAP-ID-pair ::= SEQUENCE
struct ue_ngap_id_pair_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  uint64_t   amf_ue_ngap_id  = 0;
  uint64_t   ran_ue_ngap_id  = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NGAP-IDs-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o ue_ngap_ids_ext_ies_o;

// UE-associatedLogicalNG-connectionList ::= SEQUENCE (SIZE (1..65536)) OF UE-associatedLogicalNG-connectionItem
using ue_associated_lc_ng_conn_list_l = dyn_array<ue_associated_lc_ng_conn_item_s>;

// UEPagingIdentity-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o ue_paging_id_ext_ies_o;

// UEPresenceInAreaOfInterestItem ::= SEQUENCE
struct ue_presence_in_area_of_interest_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool          ext                    = false;
  bool          ie_exts_present        = false;
  uint8_t       location_report_ref_id = 1;
  ue_presence_e uepresence;
  ie_exts_l_    ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WarningAreaList-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o warning_area_list_ext_ies_o;

// CancelAllWarningMessages ::= ENUMERATED
struct cancel_all_warning_msgs_opts {
  enum options { true_value, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cancel_all_warning_msgs_opts, true> cancel_all_warning_msgs_e;

// ConcurrentWarningMessageInd ::= ENUMERATED
struct concurrent_warning_msg_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<concurrent_warning_msg_ind_opts, true> concurrent_warning_msg_ind_e;

// HandoverFlag ::= ENUMERATED
struct ho_flag_opts {
  enum options { ho_prep, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ho_flag_opts, true> ho_flag_e;

// IMSVoiceSupportIndicator ::= ENUMERATED
struct ims_voice_support_ind_opts {
  enum options { supported, not_supported, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ims_voice_support_ind_opts, true> ims_voice_support_ind_e;

// OverloadStartNSSAIList ::= SEQUENCE (SIZE (1..1024)) OF OverloadStartNSSAIItem
using overload_start_nssai_list_l = dyn_array<overload_start_nssai_item_s>;

// PDUSessionResourceFailedToModifyListModCfm ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToModifyItemModCfm
using pdu_session_res_failed_to_modify_list_mod_cfm_l = dyn_array<pdu_session_res_failed_to_modify_item_mod_cfm_s>;

// PDUSessionResourceFailedToModifyListModRes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToModifyItemModRes
using pdu_session_res_failed_to_modify_list_mod_res_l = dyn_array<pdu_session_res_failed_to_modify_item_mod_res_s>;

// PDUSessionResourceFailedToSetupListPSReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToSetupItemPSReq
using pdu_session_res_failed_to_setup_list_ps_req_l = dyn_array<pdu_session_res_failed_to_setup_item_ps_req_s>;

// PDUSessionResourceFailedToSetupListSURes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToSetupItemSURes
using pdu_session_res_failed_to_setup_list_su_res_l = dyn_array<pdu_session_res_failed_to_setup_item_su_res_s>;

// PDUSessionResourceListCxtRelCpl ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceItemCxtRelCpl
using pdu_session_res_list_cxt_rel_cpl_l = dyn_array<pdu_session_res_item_cxt_rel_cpl_s>;

// PDUSessionResourceListCxtRelReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceItemCxtRelReq
using pdu_session_res_list_cxt_rel_req_l = dyn_array<pdu_session_res_item_cxt_rel_req_s>;

// PDUSessionResourceModifyListModCfm ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceModifyItemModCfm
using pdu_session_res_modify_list_mod_cfm_l = dyn_array<pdu_session_res_modify_item_mod_cfm_s>;

// PDUSessionResourceModifyListModInd ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceModifyItemModInd
using pdu_session_res_modify_list_mod_ind_l = dyn_array<pdu_session_res_modify_item_mod_ind_s>;

// PDUSessionResourceModifyListModReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceModifyItemModReq
using pdu_session_res_modify_list_mod_req_l = dyn_array<pdu_session_res_modify_item_mod_req_s>;

// PDUSessionResourceModifyListModRes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceModifyItemModRes
using pdu_session_res_modify_list_mod_res_l = dyn_array<pdu_session_res_modify_item_mod_res_s>;

// PDUSessionResourceNotifyList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceNotifyItem
using pdu_session_res_notify_list_l = dyn_array<pdu_session_res_notify_item_s>;

// PDUSessionResourceReleasedListNot ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceReleasedItemNot
using pdu_session_res_released_list_not_l = dyn_array<pdu_session_res_released_item_not_s>;

// PDUSessionResourceReleasedListPSAck ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceReleasedItemPSAck
using pdu_session_res_released_list_ps_ack_l = dyn_array<pdu_session_res_released_item_ps_ack_s>;

// PDUSessionResourceReleasedListPSFail ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceReleasedItemPSFail
using pdu_session_res_released_list_ps_fail_l = dyn_array<pdu_session_res_released_item_ps_fail_s>;

// PDUSessionResourceReleasedListRelRes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceReleasedItemRelRes
using pdu_session_res_released_list_rel_res_l = dyn_array<pdu_session_res_released_item_rel_res_s>;

// PDUSessionResourceSecondaryRATUsageList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSecondaryRATUsageItem
using pdu_session_res_secondary_ratusage_list_l = dyn_array<pdu_session_res_secondary_ratusage_item_s>;

// PDUSessionResourceSetupListSUReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSetupItemSUReq
using pdu_session_res_setup_list_su_req_l = dyn_array<pdu_session_res_setup_item_su_req_s>;

// PDUSessionResourceSetupListSURes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSetupItemSURes
using pdu_session_res_setup_list_su_res_l = dyn_array<pdu_session_res_setup_item_su_res_s>;

// PDUSessionResourceSwitchedList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSwitchedItem
using pdu_session_res_switched_list_l = dyn_array<pdu_session_res_switched_item_s>;

// PDUSessionResourceToBeSwitchedDLList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceToBeSwitchedDLItem
using pdu_session_res_to_be_switched_dl_list_l = dyn_array<pdu_session_res_to_be_switched_dl_item_s>;

// PDUSessionResourceToReleaseListRelCmd ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceToReleaseItemRelCmd
using pdu_session_res_to_release_list_rel_cmd_l = dyn_array<pdu_session_res_to_release_item_rel_cmd_s>;

// PWSFailedCellIDList ::= CHOICE
struct pws_failed_cell_id_list_c {
  struct types_opts {
    enum options { eutra_cgi_pws_failed_list, nr_cgi_pws_failed_list, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  pws_failed_cell_id_list_c() = default;
  pws_failed_cell_id_list_c(const pws_failed_cell_id_list_c& other);
  pws_failed_cell_id_list_c& operator=(const pws_failed_cell_id_list_c& other);
  ~pws_failed_cell_id_list_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_cgi_list_l& eutra_cgi_pws_failed_list()
  {
    assert_choice_type("eUTRA-CGI-PWSFailedList", type_.to_string(), "PWSFailedCellIDList");
    return c.get<eutra_cgi_list_l>();
  }
  nr_cgi_list_l& nr_cgi_pws_failed_list()
  {
    assert_choice_type("nR-CGI-PWSFailedList", type_.to_string(), "PWSFailedCellIDList");
    return c.get<nr_cgi_list_l>();
  }
  protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "PWSFailedCellIDList");
    return c.get<protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o> >();
  }
  const eutra_cgi_list_l& eutra_cgi_pws_failed_list() const
  {
    assert_choice_type("eUTRA-CGI-PWSFailedList", type_.to_string(), "PWSFailedCellIDList");
    return c.get<eutra_cgi_list_l>();
  }
  const nr_cgi_list_l& nr_cgi_pws_failed_list() const
  {
    assert_choice_type("nR-CGI-PWSFailedList", type_.to_string(), "PWSFailedCellIDList");
    return c.get<nr_cgi_list_l>();
  }
  const protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "PWSFailedCellIDList");
    return c.get<protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o> >();
  }
  eutra_cgi_list_l& set_eutra_cgi_pws_failed_list()
  {
    set(types::eutra_cgi_pws_failed_list);
    return c.get<eutra_cgi_list_l>();
  }
  nr_cgi_list_l& set_nr_cgi_pws_failed_list()
  {
    set(types::nr_cgi_pws_failed_list);
    return c.get<nr_cgi_list_l>();
  }
  protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o> >();
  }

private:
  types type_;
  choice_buffer_t<eutra_cgi_list_l, nr_cgi_list_l, protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o> >
      c;

  void destroy_();
};

// PagingOrigin ::= ENUMERATED
struct paging_origin_opts {
  enum options { non_minus3gpp, /*...*/ nulltype } value;
  typedef int8_t number_type;

  std::string to_string() const;
  int8_t      to_number() const;
};
typedef enumerated<paging_origin_opts, true> paging_origin_e;

// PagingPriority ::= ENUMERATED
struct paging_prio_opts {
  enum options {
    priolevel1,
    priolevel2,
    priolevel3,
    priolevel4,
    priolevel5,
    priolevel6,
    priolevel7,
    priolevel8,
    // ...
    nulltype
  } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<paging_prio_opts, true> paging_prio_e;

// PrivateIE-Field{NGAP-PRIVATE-IES : IEsSetParam} ::= SEQUENCE{{NGAP-PRIVATE-IES}}
template <class ies_set_param>
struct private_ie_field_s {
  private_ie_id_c                 id;
  crit_e                          crit;
  typename ies_set_param::value_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCState ::= ENUMERATED
struct rrc_state_opts {
  enum options { inactive, connected, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<rrc_state_opts, true> rrc_state_e;

// ResetType ::= CHOICE
struct reset_type_c {
  struct types_opts {
    enum options { ng_interface, part_of_ng_interface, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  reset_type_c() = default;
  reset_type_c(const reset_type_c& other);
  reset_type_c& operator=(const reset_type_c& other);
  ~reset_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  reset_all_e& ng_interface()
  {
    assert_choice_type("nG-Interface", type_.to_string(), "ResetType");
    return c.get<reset_all_e>();
  }
  ue_associated_lc_ng_conn_list_l& part_of_ng_interface()
  {
    assert_choice_type("partOfNG-Interface", type_.to_string(), "ResetType");
    return c.get<ue_associated_lc_ng_conn_list_l>();
  }
  protocol_ie_single_container_s<reset_type_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "ResetType");
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o> >();
  }
  const reset_all_e& ng_interface() const
  {
    assert_choice_type("nG-Interface", type_.to_string(), "ResetType");
    return c.get<reset_all_e>();
  }
  const ue_associated_lc_ng_conn_list_l& part_of_ng_interface() const
  {
    assert_choice_type("partOfNG-Interface", type_.to_string(), "ResetType");
    return c.get<ue_associated_lc_ng_conn_list_l>();
  }
  const protocol_ie_single_container_s<reset_type_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "ResetType");
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o> >();
  }
  reset_all_e& set_ng_interface()
  {
    set(types::ng_interface);
    return c.get<reset_all_e>();
  }
  ue_associated_lc_ng_conn_list_l& set_part_of_ng_interface()
  {
    set(types::part_of_ng_interface);
    return c.get<ue_associated_lc_ng_conn_list_l>();
  }
  protocol_ie_single_container_s<reset_type_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o> >();
  }

private:
  types                                                                                                  type_;
  choice_buffer_t<protocol_ie_single_container_s<reset_type_ext_ies_o>, ue_associated_lc_ng_conn_list_l> c;

  void destroy_();
};

// SupportedTAList ::= SEQUENCE (SIZE (1..256)) OF SupportedTAItem
using supported_ta_list_l = dyn_array<supported_ta_item_s>;

// TAIListForPaging ::= SEQUENCE (SIZE (1..16)) OF TAIListForPagingItem
using tai_list_for_paging_l = dyn_array<tai_list_for_paging_item_s>;

// TAIListForRestart ::= SEQUENCE (SIZE (1..2048)) OF TAI
using tai_list_for_restart_l = dyn_array<tai_s>;

// UE-NGAP-IDs ::= CHOICE
struct ue_ngap_ids_c {
  struct types_opts {
    enum options { ue_ngap_id_pair, amf_ue_ngap_id, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ue_ngap_ids_c() = default;
  ue_ngap_ids_c(const ue_ngap_ids_c& other);
  ue_ngap_ids_c& operator=(const ue_ngap_ids_c& other);
  ~ue_ngap_ids_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ue_ngap_id_pair_s& ue_ngap_id_pair()
  {
    assert_choice_type("uE-NGAP-ID-pair", type_.to_string(), "UE-NGAP-IDs");
    return c.get<ue_ngap_id_pair_s>();
  }
  uint64_t& amf_ue_ngap_id()
  {
    assert_choice_type("aMF-UE-NGAP-ID", type_.to_string(), "UE-NGAP-IDs");
    return c.get<uint64_t>();
  }
  protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "UE-NGAP-IDs");
    return c.get<protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o> >();
  }
  const ue_ngap_id_pair_s& ue_ngap_id_pair() const
  {
    assert_choice_type("uE-NGAP-ID-pair", type_.to_string(), "UE-NGAP-IDs");
    return c.get<ue_ngap_id_pair_s>();
  }
  const uint64_t& amf_ue_ngap_id() const
  {
    assert_choice_type("aMF-UE-NGAP-ID", type_.to_string(), "UE-NGAP-IDs");
    return c.get<uint64_t>();
  }
  const protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "UE-NGAP-IDs");
    return c.get<protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o> >();
  }
  ue_ngap_id_pair_s& set_ue_ngap_id_pair()
  {
    set(types::ue_ngap_id_pair);
    return c.get<ue_ngap_id_pair_s>();
  }
  uint64_t& set_amf_ue_ngap_id()
  {
    set(types::amf_ue_ngap_id);
    return c.get<uint64_t>();
  }
  protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o> >();
  }

private:
  types                                                                                     type_;
  choice_buffer_t<protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o>, ue_ngap_id_pair_s> c;

  void destroy_();
};

// UEPagingIdentity ::= CHOICE
struct ue_paging_id_c {
  struct types_opts {
    enum options { five_g_s_tmsi, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ue_paging_id_c() = default;
  ue_paging_id_c(const ue_paging_id_c& other);
  ue_paging_id_c& operator=(const ue_paging_id_c& other);
  ~ue_paging_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  five_g_s_tmsi_s& five_g_s_tmsi()
  {
    assert_choice_type("fiveG-S-TMSI", type_.to_string(), "UEPagingIdentity");
    return c.get<five_g_s_tmsi_s>();
  }
  protocol_ie_single_container_s<ue_paging_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "UEPagingIdentity");
    return c.get<protocol_ie_single_container_s<ue_paging_id_ext_ies_o> >();
  }
  const five_g_s_tmsi_s& five_g_s_tmsi() const
  {
    assert_choice_type("fiveG-S-TMSI", type_.to_string(), "UEPagingIdentity");
    return c.get<five_g_s_tmsi_s>();
  }
  const protocol_ie_single_container_s<ue_paging_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "UEPagingIdentity");
    return c.get<protocol_ie_single_container_s<ue_paging_id_ext_ies_o> >();
  }
  five_g_s_tmsi_s& set_five_g_s_tmsi()
  {
    set(types::five_g_s_tmsi);
    return c.get<five_g_s_tmsi_s>();
  }
  protocol_ie_single_container_s<ue_paging_id_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<ue_paging_id_ext_ies_o> >();
  }

private:
  types                                                                                     type_;
  choice_buffer_t<five_g_s_tmsi_s, protocol_ie_single_container_s<ue_paging_id_ext_ies_o> > c;

  void destroy_();
};

// UEPresenceInAreaOfInterestList ::= SEQUENCE (SIZE (1..64)) OF UEPresenceInAreaOfInterestItem
using ue_presence_in_area_of_interest_list_l = dyn_array<ue_presence_in_area_of_interest_item_s>;

// UERetentionInformation ::= ENUMERATED
struct ue_retention_info_opts {
  enum options { ues_retained, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ue_retention_info_opts, true> ue_retention_info_e;

// WarningAreaList ::= CHOICE
struct warning_area_list_c {
  struct types_opts {
    enum options {
      eutra_cgi_list_for_warning,
      nr_cgi_list_for_warning,
      tai_list_for_warning,
      emergency_area_id_list,
      choice_exts,
      nulltype
    } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  warning_area_list_c() = default;
  warning_area_list_c(const warning_area_list_c& other);
  warning_area_list_c& operator=(const warning_area_list_c& other);
  ~warning_area_list_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_cgi_list_for_warning_l& eutra_cgi_list_for_warning()
  {
    assert_choice_type("eUTRA-CGIListForWarning", type_.to_string(), "WarningAreaList");
    return c.get<eutra_cgi_list_for_warning_l>();
  }
  nr_cgi_list_for_warning_l& nr_cgi_list_for_warning()
  {
    assert_choice_type("nR-CGIListForWarning", type_.to_string(), "WarningAreaList");
    return c.get<nr_cgi_list_for_warning_l>();
  }
  tai_list_for_warning_l& tai_list_for_warning()
  {
    assert_choice_type("tAIListForWarning", type_.to_string(), "WarningAreaList");
    return c.get<tai_list_for_warning_l>();
  }
  emergency_area_id_list_l& emergency_area_id_list()
  {
    assert_choice_type("emergencyAreaIDList", type_.to_string(), "WarningAreaList");
    return c.get<emergency_area_id_list_l>();
  }
  protocol_ie_single_container_s<warning_area_list_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "WarningAreaList");
    return c.get<protocol_ie_single_container_s<warning_area_list_ext_ies_o> >();
  }
  const eutra_cgi_list_for_warning_l& eutra_cgi_list_for_warning() const
  {
    assert_choice_type("eUTRA-CGIListForWarning", type_.to_string(), "WarningAreaList");
    return c.get<eutra_cgi_list_for_warning_l>();
  }
  const nr_cgi_list_for_warning_l& nr_cgi_list_for_warning() const
  {
    assert_choice_type("nR-CGIListForWarning", type_.to_string(), "WarningAreaList");
    return c.get<nr_cgi_list_for_warning_l>();
  }
  const tai_list_for_warning_l& tai_list_for_warning() const
  {
    assert_choice_type("tAIListForWarning", type_.to_string(), "WarningAreaList");
    return c.get<tai_list_for_warning_l>();
  }
  const emergency_area_id_list_l& emergency_area_id_list() const
  {
    assert_choice_type("emergencyAreaIDList", type_.to_string(), "WarningAreaList");
    return c.get<emergency_area_id_list_l>();
  }
  const protocol_ie_single_container_s<warning_area_list_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "WarningAreaList");
    return c.get<protocol_ie_single_container_s<warning_area_list_ext_ies_o> >();
  }
  eutra_cgi_list_for_warning_l& set_eutra_cgi_list_for_warning()
  {
    set(types::eutra_cgi_list_for_warning);
    return c.get<eutra_cgi_list_for_warning_l>();
  }
  nr_cgi_list_for_warning_l& set_nr_cgi_list_for_warning()
  {
    set(types::nr_cgi_list_for_warning);
    return c.get<nr_cgi_list_for_warning_l>();
  }
  tai_list_for_warning_l& set_tai_list_for_warning()
  {
    set(types::tai_list_for_warning);
    return c.get<tai_list_for_warning_l>();
  }
  emergency_area_id_list_l& set_emergency_area_id_list()
  {
    set(types::emergency_area_id_list);
    return c.get<emergency_area_id_list_l>();
  }
  protocol_ie_single_container_s<warning_area_list_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<warning_area_list_ext_ies_o> >();
  }

private:
  types type_;
  choice_buffer_t<emergency_area_id_list_l,
                  eutra_cgi_list_for_warning_l,
                  nr_cgi_list_for_warning_l,
                  protocol_ie_single_container_s<warning_area_list_ext_ies_o>,
                  tai_list_for_warning_l>
      c;

  void destroy_();
};

// LocationReportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct location_report_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        user_location_info,
        ue_presence_in_area_of_interest_list,
        location_report_request_type,
        ps_cell_info,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    user_location_info_c& user_location_info()
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    ue_presence_in_area_of_interest_list_l& ue_presence_in_area_of_interest_list()
    {
      assert_choice_type("UEPresenceInAreaOfInterestList", type_.to_string(), "Value");
      return c.get<ue_presence_in_area_of_interest_list_l>();
    }
    location_report_request_type_s& location_report_request_type()
    {
      assert_choice_type("LocationReportingRequestType", type_.to_string(), "Value");
      return c.get<location_report_request_type_s>();
    }
    ngran_cgi_c& ps_cell_info()
    {
      assert_choice_type("NGRAN-CGI", type_.to_string(), "Value");
      return c.get<ngran_cgi_c>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const user_location_info_c& user_location_info() const
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    const ue_presence_in_area_of_interest_list_l& ue_presence_in_area_of_interest_list() const
    {
      assert_choice_type("UEPresenceInAreaOfInterestList", type_.to_string(), "Value");
      return c.get<ue_presence_in_area_of_interest_list_l>();
    }
    const location_report_request_type_s& location_report_request_type() const
    {
      assert_choice_type("LocationReportingRequestType", type_.to_string(), "Value");
      return c.get<location_report_request_type_s>();
    }
    const ngran_cgi_c& ps_cell_info() const
    {
      assert_choice_type("NGRAN-CGI", type_.to_string(), "Value");
      return c.get<ngran_cgi_c>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    user_location_info_c& set_user_location_info()
    {
      set(types::user_location_info);
      return c.get<user_location_info_c>();
    }
    ue_presence_in_area_of_interest_list_l& set_ue_presence_in_area_of_interest_list()
    {
      set(types::ue_presence_in_area_of_interest_list);
      return c.get<ue_presence_in_area_of_interest_list_l>();
    }
    location_report_request_type_s& set_location_report_request_type()
    {
      set(types::location_report_request_type);
      return c.get<location_report_request_type_s>();
    }
    ngran_cgi_c& set_ps_cell_info()
    {
      set(types::ps_cell_info);
      return c.get<ngran_cgi_c>();
    }

  private:
    types type_;
    choice_buffer_t<location_report_request_type_s,
                    ngran_cgi_c,
                    ue_presence_in_area_of_interest_list_l,
                    user_location_info_c>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LocationReportingControlIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct location_report_ctrl_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, location_report_request_type, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    location_report_request_type_s& location_report_request_type()
    {
      assert_choice_type("LocationReportingRequestType", type_.to_string(), "Value");
      return c.get<location_report_request_type_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const location_report_request_type_s& location_report_request_type() const
    {
      assert_choice_type("LocationReportingRequestType", type_.to_string(), "Value");
      return c.get<location_report_request_type_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    location_report_request_type_s& set_location_report_request_type()
    {
      set(types::location_report_request_type);
      return c.get<location_report_request_type_s>();
    }

  private:
    types                                           type_;
    choice_buffer_t<location_report_request_type_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LocationReportingFailureIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct location_report_fail_ind_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, cause, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }

  private:
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NASNonDeliveryIndication-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct nas_non_delivery_ind_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, nas_pdu, cause, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& nas_pdu()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const unbounded_octstring<true>& nas_pdu() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& set_nas_pdu()
    {
      set(types::nas_pdu);
      return c.get<unbounded_octstring<true> >();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }

  private:
    types                                                type_;
    choice_buffer_t<cause_c, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NGResetAcknowledgeIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ng_reset_ack_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_ng_conn_list, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_associated_lc_ng_conn_list_l& ue_associated_lc_ng_conn_list()
    {
      assert_choice_type("UE-associatedLogicalNG-connectionList", type_.to_string(), "Value");
      return c.get<ue_associated_lc_ng_conn_list_l>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const ue_associated_lc_ng_conn_list_l& ue_associated_lc_ng_conn_list() const
    {
      assert_choice_type("UE-associatedLogicalNG-connectionList", type_.to_string(), "Value");
      return c.get<ue_associated_lc_ng_conn_list_l>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    ue_associated_lc_ng_conn_list_l& set_ue_associated_lc_ng_conn_list()
    {
      set(types::ue_associated_lc_ng_conn_list);
      return c.get<ue_associated_lc_ng_conn_list_l>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                                                type_;
    choice_buffer_t<crit_diagnostics_s, ue_associated_lc_ng_conn_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NGResetIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ng_reset_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, reset_type, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    reset_type_c& reset_type()
    {
      assert_choice_type("ResetType", type_.to_string(), "Value");
      return c.get<reset_type_c>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const reset_type_c& reset_type() const
    {
      assert_choice_type("ResetType", type_.to_string(), "Value");
      return c.get<reset_type_c>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }
    reset_type_c& set_reset_type()
    {
      set(types::reset_type);
      return c.get<reset_type_c>();
    }

  private:
    types                                  type_;
    choice_buffer_t<cause_c, reset_type_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NGSetupFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ng_setup_fail_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, time_to_wait, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    time_to_wait_e& time_to_wait()
    {
      assert_choice_type("TimeToWait", type_.to_string(), "Value");
      return c.get<time_to_wait_e>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const time_to_wait_e& time_to_wait() const
    {
      assert_choice_type("TimeToWait", type_.to_string(), "Value");
      return c.get<time_to_wait_e>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }
    time_to_wait_e& set_time_to_wait()
    {
      set(types::time_to_wait);
      return c.get<time_to_wait_e>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NGSetupRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ng_setup_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        global_ran_node_id,
        ran_node_name,
        supported_ta_list,
        default_paging_drx,
        ue_retention_info,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    global_ran_node_id_c& global_ran_node_id()
    {
      assert_choice_type("GlobalRANNodeID", type_.to_string(), "Value");
      return c.get<global_ran_node_id_c>();
    }
    printable_string<1, 150, true, true>& ran_node_name()
    {
      assert_choice_type("PrintableString", type_.to_string(), "Value");
      return c.get<printable_string<1, 150, true, true> >();
    }
    supported_ta_list_l& supported_ta_list()
    {
      assert_choice_type("SupportedTAList", type_.to_string(), "Value");
      return c.get<supported_ta_list_l>();
    }
    paging_drx_e& default_paging_drx()
    {
      assert_choice_type("PagingDRX", type_.to_string(), "Value");
      return c.get<paging_drx_e>();
    }
    ue_retention_info_e& ue_retention_info()
    {
      assert_choice_type("UERetentionInformation", type_.to_string(), "Value");
      return c.get<ue_retention_info_e>();
    }
    const global_ran_node_id_c& global_ran_node_id() const
    {
      assert_choice_type("GlobalRANNodeID", type_.to_string(), "Value");
      return c.get<global_ran_node_id_c>();
    }
    const printable_string<1, 150, true, true>& ran_node_name() const
    {
      assert_choice_type("PrintableString", type_.to_string(), "Value");
      return c.get<printable_string<1, 150, true, true> >();
    }
    const supported_ta_list_l& supported_ta_list() const
    {
      assert_choice_type("SupportedTAList", type_.to_string(), "Value");
      return c.get<supported_ta_list_l>();
    }
    const paging_drx_e& default_paging_drx() const
    {
      assert_choice_type("PagingDRX", type_.to_string(), "Value");
      return c.get<paging_drx_e>();
    }
    const ue_retention_info_e& ue_retention_info() const
    {
      assert_choice_type("UERetentionInformation", type_.to_string(), "Value");
      return c.get<ue_retention_info_e>();
    }
    global_ran_node_id_c& set_global_ran_node_id()
    {
      set(types::global_ran_node_id);
      return c.get<global_ran_node_id_c>();
    }
    printable_string<1, 150, true, true>& set_ran_node_name()
    {
      set(types::ran_node_name);
      return c.get<printable_string<1, 150, true, true> >();
    }
    supported_ta_list_l& set_supported_ta_list()
    {
      set(types::supported_ta_list);
      return c.get<supported_ta_list_l>();
    }
    paging_drx_e& set_default_paging_drx()
    {
      set(types::default_paging_drx);
      return c.get<paging_drx_e>();
    }
    ue_retention_info_e& set_ue_retention_info()
    {
      set(types::ue_retention_info);
      return c.get<ue_retention_info_e>();
    }

  private:
    types                                                                                            type_;
    choice_buffer_t<global_ran_node_id_c, printable_string<1, 150, true, true>, supported_ta_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NGSetupResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ng_setup_resp_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_name,
        served_guami_list,
        relative_amf_capacity,
        plmn_support_list,
        crit_diagnostics,
        ue_retention_info,
        nulltype
      } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    printable_string<1, 150, true, true>& amf_name()
    {
      assert_choice_type("PrintableString", type_.to_string(), "Value");
      return c.get<printable_string<1, 150, true, true> >();
    }
    served_guami_list_l& served_guami_list()
    {
      assert_choice_type("ServedGUAMIList", type_.to_string(), "Value");
      return c.get<served_guami_list_l>();
    }
    uint16_t& relative_amf_capacity()
    {
      assert_choice_type("INTEGER (0..255)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    plmn_support_list_l& plmn_support_list()
    {
      assert_choice_type("PLMNSupportList", type_.to_string(), "Value");
      return c.get<plmn_support_list_l>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    ue_retention_info_e& ue_retention_info()
    {
      assert_choice_type("UERetentionInformation", type_.to_string(), "Value");
      return c.get<ue_retention_info_e>();
    }
    const printable_string<1, 150, true, true>& amf_name() const
    {
      assert_choice_type("PrintableString", type_.to_string(), "Value");
      return c.get<printable_string<1, 150, true, true> >();
    }
    const served_guami_list_l& served_guami_list() const
    {
      assert_choice_type("ServedGUAMIList", type_.to_string(), "Value");
      return c.get<served_guami_list_l>();
    }
    const uint16_t& relative_amf_capacity() const
    {
      assert_choice_type("INTEGER (0..255)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    const plmn_support_list_l& plmn_support_list() const
    {
      assert_choice_type("PLMNSupportList", type_.to_string(), "Value");
      return c.get<plmn_support_list_l>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const ue_retention_info_e& ue_retention_info() const
    {
      assert_choice_type("UERetentionInformation", type_.to_string(), "Value");
      return c.get<ue_retention_info_e>();
    }
    printable_string<1, 150, true, true>& set_amf_name()
    {
      set(types::amf_name);
      return c.get<printable_string<1, 150, true, true> >();
    }
    served_guami_list_l& set_served_guami_list()
    {
      set(types::served_guami_list);
      return c.get<served_guami_list_l>();
    }
    uint16_t& set_relative_amf_capacity()
    {
      set(types::relative_amf_capacity);
      return c.get<uint16_t>();
    }
    plmn_support_list_l& set_plmn_support_list()
    {
      set(types::plmn_support_list);
      return c.get<plmn_support_list_l>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }
    ue_retention_info_e& set_ue_retention_info()
    {
      set(types::ue_retention_info);
      return c.get<ue_retention_info_e>();
    }

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s, plmn_support_list_l, printable_string<1, 150, true, true>, served_guami_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// OverloadStartIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct overload_start_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_overload_resp, amf_traffic_load_reduction_ind, overload_start_nssai_list, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    overload_resp_c& amf_overload_resp()
    {
      assert_choice_type("OverloadResponse", type_.to_string(), "Value");
      return c.get<overload_resp_c>();
    }
    uint8_t& amf_traffic_load_reduction_ind()
    {
      assert_choice_type("INTEGER (1..99)", type_.to_string(), "Value");
      return c.get<uint8_t>();
    }
    overload_start_nssai_list_l& overload_start_nssai_list()
    {
      assert_choice_type("OverloadStartNSSAIList", type_.to_string(), "Value");
      return c.get<overload_start_nssai_list_l>();
    }
    const overload_resp_c& amf_overload_resp() const
    {
      assert_choice_type("OverloadResponse", type_.to_string(), "Value");
      return c.get<overload_resp_c>();
    }
    const uint8_t& amf_traffic_load_reduction_ind() const
    {
      assert_choice_type("INTEGER (1..99)", type_.to_string(), "Value");
      return c.get<uint8_t>();
    }
    const overload_start_nssai_list_l& overload_start_nssai_list() const
    {
      assert_choice_type("OverloadStartNSSAIList", type_.to_string(), "Value");
      return c.get<overload_start_nssai_list_l>();
    }
    overload_resp_c& set_amf_overload_resp()
    {
      set(types::amf_overload_resp);
      return c.get<overload_resp_c>();
    }
    uint8_t& set_amf_traffic_load_reduction_ind()
    {
      set(types::amf_traffic_load_reduction_ind);
      return c.get<uint8_t>();
    }
    overload_start_nssai_list_l& set_overload_start_nssai_list()
    {
      set(types::overload_start_nssai_list);
      return c.get<overload_start_nssai_list_l>();
    }

  private:
    types                                                         type_;
    choice_buffer_t<overload_resp_c, overload_start_nssai_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// OverloadStopIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o overload_stop_ies_o;

// PDUSessionResourceModifyConfirmIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_modify_confirm_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_modify_list_mod_cfm,
        pdu_session_res_failed_to_modify_list_mod_cfm,
        crit_diagnostics,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    pdu_session_res_modify_list_mod_cfm_l& pdu_session_res_modify_list_mod_cfm()
    {
      assert_choice_type("PDUSessionResourceModifyListModCfm", type_.to_string(), "Value");
      return c.get<pdu_session_res_modify_list_mod_cfm_l>();
    }
    pdu_session_res_failed_to_modify_list_mod_cfm_l& pdu_session_res_failed_to_modify_list_mod_cfm()
    {
      assert_choice_type("PDUSessionResourceFailedToModifyListModCfm", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_modify_list_mod_cfm_l>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const pdu_session_res_modify_list_mod_cfm_l& pdu_session_res_modify_list_mod_cfm() const
    {
      assert_choice_type("PDUSessionResourceModifyListModCfm", type_.to_string(), "Value");
      return c.get<pdu_session_res_modify_list_mod_cfm_l>();
    }
    const pdu_session_res_failed_to_modify_list_mod_cfm_l& pdu_session_res_failed_to_modify_list_mod_cfm() const
    {
      assert_choice_type("PDUSessionResourceFailedToModifyListModCfm", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_modify_list_mod_cfm_l>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    pdu_session_res_modify_list_mod_cfm_l& set_pdu_session_res_modify_list_mod_cfm()
    {
      set(types::pdu_session_res_modify_list_mod_cfm);
      return c.get<pdu_session_res_modify_list_mod_cfm_l>();
    }
    pdu_session_res_failed_to_modify_list_mod_cfm_l& set_pdu_session_res_failed_to_modify_list_mod_cfm()
    {
      set(types::pdu_session_res_failed_to_modify_list_mod_cfm);
      return c.get<pdu_session_res_failed_to_modify_list_mod_cfm_l>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    pdu_session_res_failed_to_modify_list_mod_cfm_l,
                    pdu_session_res_modify_list_mod_cfm_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceModifyIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_modify_ind_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, pdu_session_res_modify_list_mod_ind, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    pdu_session_res_modify_list_mod_ind_l& pdu_session_res_modify_list_mod_ind()
    {
      assert_choice_type("PDUSessionResourceModifyListModInd", type_.to_string(), "Value");
      return c.get<pdu_session_res_modify_list_mod_ind_l>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const pdu_session_res_modify_list_mod_ind_l& pdu_session_res_modify_list_mod_ind() const
    {
      assert_choice_type("PDUSessionResourceModifyListModInd", type_.to_string(), "Value");
      return c.get<pdu_session_res_modify_list_mod_ind_l>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    pdu_session_res_modify_list_mod_ind_l& set_pdu_session_res_modify_list_mod_ind()
    {
      set(types::pdu_session_res_modify_list_mod_ind);
      return c.get<pdu_session_res_modify_list_mod_ind_l>();
    }

  private:
    types                                                  type_;
    choice_buffer_t<pdu_session_res_modify_list_mod_ind_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceModifyRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_modify_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ran_paging_prio,
        pdu_session_res_modify_list_mod_req,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint16_t& ran_paging_prio()
    {
      assert_choice_type("INTEGER (1..256)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    pdu_session_res_modify_list_mod_req_l& pdu_session_res_modify_list_mod_req()
    {
      assert_choice_type("PDUSessionResourceModifyListModReq", type_.to_string(), "Value");
      return c.get<pdu_session_res_modify_list_mod_req_l>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint16_t& ran_paging_prio() const
    {
      assert_choice_type("INTEGER (1..256)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    const pdu_session_res_modify_list_mod_req_l& pdu_session_res_modify_list_mod_req() const
    {
      assert_choice_type("PDUSessionResourceModifyListModReq", type_.to_string(), "Value");
      return c.get<pdu_session_res_modify_list_mod_req_l>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint16_t& set_ran_paging_prio()
    {
      set(types::ran_paging_prio);
      return c.get<uint16_t>();
    }
    pdu_session_res_modify_list_mod_req_l& set_pdu_session_res_modify_list_mod_req()
    {
      set(types::pdu_session_res_modify_list_mod_req);
      return c.get<pdu_session_res_modify_list_mod_req_l>();
    }

  private:
    types                                                  type_;
    choice_buffer_t<pdu_session_res_modify_list_mod_req_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceModifyResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_modify_resp_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_modify_list_mod_res,
        pdu_session_res_failed_to_modify_list_mod_res,
        user_location_info,
        crit_diagnostics,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    pdu_session_res_modify_list_mod_res_l& pdu_session_res_modify_list_mod_res()
    {
      assert_choice_type("PDUSessionResourceModifyListModRes", type_.to_string(), "Value");
      return c.get<pdu_session_res_modify_list_mod_res_l>();
    }
    pdu_session_res_failed_to_modify_list_mod_res_l& pdu_session_res_failed_to_modify_list_mod_res()
    {
      assert_choice_type("PDUSessionResourceFailedToModifyListModRes", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_modify_list_mod_res_l>();
    }
    user_location_info_c& user_location_info()
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const pdu_session_res_modify_list_mod_res_l& pdu_session_res_modify_list_mod_res() const
    {
      assert_choice_type("PDUSessionResourceModifyListModRes", type_.to_string(), "Value");
      return c.get<pdu_session_res_modify_list_mod_res_l>();
    }
    const pdu_session_res_failed_to_modify_list_mod_res_l& pdu_session_res_failed_to_modify_list_mod_res() const
    {
      assert_choice_type("PDUSessionResourceFailedToModifyListModRes", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_modify_list_mod_res_l>();
    }
    const user_location_info_c& user_location_info() const
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    pdu_session_res_modify_list_mod_res_l& set_pdu_session_res_modify_list_mod_res()
    {
      set(types::pdu_session_res_modify_list_mod_res);
      return c.get<pdu_session_res_modify_list_mod_res_l>();
    }
    pdu_session_res_failed_to_modify_list_mod_res_l& set_pdu_session_res_failed_to_modify_list_mod_res()
    {
      set(types::pdu_session_res_failed_to_modify_list_mod_res);
      return c.get<pdu_session_res_failed_to_modify_list_mod_res_l>();
    }
    user_location_info_c& set_user_location_info()
    {
      set(types::user_location_info);
      return c.get<user_location_info_c>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    pdu_session_res_failed_to_modify_list_mod_res_l,
                    pdu_session_res_modify_list_mod_res_l,
                    user_location_info_c>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceNotifyIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_notify_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_notify_list,
        pdu_session_res_released_list_not,
        user_location_info,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    pdu_session_res_notify_list_l& pdu_session_res_notify_list()
    {
      assert_choice_type("PDUSessionResourceNotifyList", type_.to_string(), "Value");
      return c.get<pdu_session_res_notify_list_l>();
    }
    pdu_session_res_released_list_not_l& pdu_session_res_released_list_not()
    {
      assert_choice_type("PDUSessionResourceReleasedListNot", type_.to_string(), "Value");
      return c.get<pdu_session_res_released_list_not_l>();
    }
    user_location_info_c& user_location_info()
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const pdu_session_res_notify_list_l& pdu_session_res_notify_list() const
    {
      assert_choice_type("PDUSessionResourceNotifyList", type_.to_string(), "Value");
      return c.get<pdu_session_res_notify_list_l>();
    }
    const pdu_session_res_released_list_not_l& pdu_session_res_released_list_not() const
    {
      assert_choice_type("PDUSessionResourceReleasedListNot", type_.to_string(), "Value");
      return c.get<pdu_session_res_released_list_not_l>();
    }
    const user_location_info_c& user_location_info() const
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    pdu_session_res_notify_list_l& set_pdu_session_res_notify_list()
    {
      set(types::pdu_session_res_notify_list);
      return c.get<pdu_session_res_notify_list_l>();
    }
    pdu_session_res_released_list_not_l& set_pdu_session_res_released_list_not()
    {
      set(types::pdu_session_res_released_list_not);
      return c.get<pdu_session_res_released_list_not_l>();
    }
    user_location_info_c& set_user_location_info()
    {
      set(types::user_location_info);
      return c.get<user_location_info_c>();
    }

  private:
    types                                                                                                     type_;
    choice_buffer_t<pdu_session_res_notify_list_l, pdu_session_res_released_list_not_l, user_location_info_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceReleaseCommandIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_release_cmd_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ran_paging_prio,
        nas_pdu,
        pdu_session_res_to_release_list_rel_cmd,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint16_t& ran_paging_prio()
    {
      assert_choice_type("INTEGER (1..256)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    unbounded_octstring<true>& nas_pdu()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    pdu_session_res_to_release_list_rel_cmd_l& pdu_session_res_to_release_list_rel_cmd()
    {
      assert_choice_type("PDUSessionResourceToReleaseListRelCmd", type_.to_string(), "Value");
      return c.get<pdu_session_res_to_release_list_rel_cmd_l>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint16_t& ran_paging_prio() const
    {
      assert_choice_type("INTEGER (1..256)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    const unbounded_octstring<true>& nas_pdu() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const pdu_session_res_to_release_list_rel_cmd_l& pdu_session_res_to_release_list_rel_cmd() const
    {
      assert_choice_type("PDUSessionResourceToReleaseListRelCmd", type_.to_string(), "Value");
      return c.get<pdu_session_res_to_release_list_rel_cmd_l>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint16_t& set_ran_paging_prio()
    {
      set(types::ran_paging_prio);
      return c.get<uint16_t>();
    }
    unbounded_octstring<true>& set_nas_pdu()
    {
      set(types::nas_pdu);
      return c.get<unbounded_octstring<true> >();
    }
    pdu_session_res_to_release_list_rel_cmd_l& set_pdu_session_res_to_release_list_rel_cmd()
    {
      set(types::pdu_session_res_to_release_list_rel_cmd);
      return c.get<pdu_session_res_to_release_list_rel_cmd_l>();
    }

  private:
    types                                                                                  type_;
    choice_buffer_t<pdu_session_res_to_release_list_rel_cmd_l, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceReleaseResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_release_resp_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_released_list_rel_res,
        user_location_info,
        crit_diagnostics,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    pdu_session_res_released_list_rel_res_l& pdu_session_res_released_list_rel_res()
    {
      assert_choice_type("PDUSessionResourceReleasedListRelRes", type_.to_string(), "Value");
      return c.get<pdu_session_res_released_list_rel_res_l>();
    }
    user_location_info_c& user_location_info()
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const pdu_session_res_released_list_rel_res_l& pdu_session_res_released_list_rel_res() const
    {
      assert_choice_type("PDUSessionResourceReleasedListRelRes", type_.to_string(), "Value");
      return c.get<pdu_session_res_released_list_rel_res_l>();
    }
    const user_location_info_c& user_location_info() const
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    pdu_session_res_released_list_rel_res_l& set_pdu_session_res_released_list_rel_res()
    {
      set(types::pdu_session_res_released_list_rel_res);
      return c.get<pdu_session_res_released_list_rel_res_l>();
    }
    user_location_info_c& set_user_location_info()
    {
      set(types::user_location_info);
      return c.get<user_location_info_c>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                                                                              type_;
    choice_buffer_t<crit_diagnostics_s, pdu_session_res_released_list_rel_res_l, user_location_info_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceSetupRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_setup_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ran_paging_prio,
        nas_pdu,
        pdu_session_res_setup_list_su_req,
        ue_aggregate_maximum_bit_rate,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint16_t& ran_paging_prio()
    {
      assert_choice_type("INTEGER (1..256)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    unbounded_octstring<true>& nas_pdu()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    pdu_session_res_setup_list_su_req_l& pdu_session_res_setup_list_su_req()
    {
      assert_choice_type("PDUSessionResourceSetupListSUReq", type_.to_string(), "Value");
      return c.get<pdu_session_res_setup_list_su_req_l>();
    }
    ue_aggregate_maximum_bit_rate_s& ue_aggregate_maximum_bit_rate()
    {
      assert_choice_type("UEAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint16_t& ran_paging_prio() const
    {
      assert_choice_type("INTEGER (1..256)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    const unbounded_octstring<true>& nas_pdu() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const pdu_session_res_setup_list_su_req_l& pdu_session_res_setup_list_su_req() const
    {
      assert_choice_type("PDUSessionResourceSetupListSUReq", type_.to_string(), "Value");
      return c.get<pdu_session_res_setup_list_su_req_l>();
    }
    const ue_aggregate_maximum_bit_rate_s& ue_aggregate_maximum_bit_rate() const
    {
      assert_choice_type("UEAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint16_t& set_ran_paging_prio()
    {
      set(types::ran_paging_prio);
      return c.get<uint16_t>();
    }
    unbounded_octstring<true>& set_nas_pdu()
    {
      set(types::nas_pdu);
      return c.get<unbounded_octstring<true> >();
    }
    pdu_session_res_setup_list_su_req_l& set_pdu_session_res_setup_list_su_req()
    {
      set(types::pdu_session_res_setup_list_su_req);
      return c.get<pdu_session_res_setup_list_su_req_l>();
    }
    ue_aggregate_maximum_bit_rate_s& set_ue_aggregate_maximum_bit_rate()
    {
      set(types::ue_aggregate_maximum_bit_rate);
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }

  private:
    types type_;
    choice_buffer_t<pdu_session_res_setup_list_su_req_l, ue_aggregate_maximum_bit_rate_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceSetupResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_setup_resp_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_setup_list_su_res,
        pdu_session_res_failed_to_setup_list_su_res,
        crit_diagnostics,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    pdu_session_res_setup_list_su_res_l& pdu_session_res_setup_list_su_res()
    {
      assert_choice_type("PDUSessionResourceSetupListSURes", type_.to_string(), "Value");
      return c.get<pdu_session_res_setup_list_su_res_l>();
    }
    pdu_session_res_failed_to_setup_list_su_res_l& pdu_session_res_failed_to_setup_list_su_res()
    {
      assert_choice_type("PDUSessionResourceFailedToSetupListSURes", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_setup_list_su_res_l>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const pdu_session_res_setup_list_su_res_l& pdu_session_res_setup_list_su_res() const
    {
      assert_choice_type("PDUSessionResourceSetupListSURes", type_.to_string(), "Value");
      return c.get<pdu_session_res_setup_list_su_res_l>();
    }
    const pdu_session_res_failed_to_setup_list_su_res_l& pdu_session_res_failed_to_setup_list_su_res() const
    {
      assert_choice_type("PDUSessionResourceFailedToSetupListSURes", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_setup_list_su_res_l>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    pdu_session_res_setup_list_su_res_l& set_pdu_session_res_setup_list_su_res()
    {
      set(types::pdu_session_res_setup_list_su_res);
      return c.get<pdu_session_res_setup_list_su_res_l>();
    }
    pdu_session_res_failed_to_setup_list_su_res_l& set_pdu_session_res_failed_to_setup_list_su_res()
    {
      set(types::pdu_session_res_failed_to_setup_list_su_res);
      return c.get<pdu_session_res_failed_to_setup_list_su_res_l>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    pdu_session_res_failed_to_setup_list_su_res_l,
                    pdu_session_res_setup_list_su_res_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSCancelRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pws_cancel_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { msg_id, serial_num, warning_area_list, cancel_all_warning_msgs, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>& msg_id()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    fixed_bitstring<16, false, true>& serial_num()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    warning_area_list_c& warning_area_list()
    {
      assert_choice_type("WarningAreaList", type_.to_string(), "Value");
      return c.get<warning_area_list_c>();
    }
    cancel_all_warning_msgs_e& cancel_all_warning_msgs()
    {
      assert_choice_type("CancelAllWarningMessages", type_.to_string(), "Value");
      return c.get<cancel_all_warning_msgs_e>();
    }
    const fixed_bitstring<16, false, true>& msg_id() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    const fixed_bitstring<16, false, true>& serial_num() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    const warning_area_list_c& warning_area_list() const
    {
      assert_choice_type("WarningAreaList", type_.to_string(), "Value");
      return c.get<warning_area_list_c>();
    }
    const cancel_all_warning_msgs_e& cancel_all_warning_msgs() const
    {
      assert_choice_type("CancelAllWarningMessages", type_.to_string(), "Value");
      return c.get<cancel_all_warning_msgs_e>();
    }
    fixed_bitstring<16, false, true>& set_msg_id()
    {
      set(types::msg_id);
      return c.get<fixed_bitstring<16, false, true> >();
    }
    fixed_bitstring<16, false, true>& set_serial_num()
    {
      set(types::serial_num);
      return c.get<fixed_bitstring<16, false, true> >();
    }
    warning_area_list_c& set_warning_area_list()
    {
      set(types::warning_area_list);
      return c.get<warning_area_list_c>();
    }
    cancel_all_warning_msgs_e& set_cancel_all_warning_msgs()
    {
      set(types::cancel_all_warning_msgs);
      return c.get<cancel_all_warning_msgs_e>();
    }

  private:
    types                                                                  type_;
    choice_buffer_t<fixed_bitstring<16, false, true>, warning_area_list_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSCancelResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pws_cancel_resp_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { msg_id, serial_num, broadcast_cancelled_area_list, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>& msg_id()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    fixed_bitstring<16, false, true>& serial_num()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    broadcast_cancelled_area_list_c& broadcast_cancelled_area_list()
    {
      assert_choice_type("BroadcastCancelledAreaList", type_.to_string(), "Value");
      return c.get<broadcast_cancelled_area_list_c>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const fixed_bitstring<16, false, true>& msg_id() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    const fixed_bitstring<16, false, true>& serial_num() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    const broadcast_cancelled_area_list_c& broadcast_cancelled_area_list() const
    {
      assert_choice_type("BroadcastCancelledAreaList", type_.to_string(), "Value");
      return c.get<broadcast_cancelled_area_list_c>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    fixed_bitstring<16, false, true>& set_msg_id()
    {
      set(types::msg_id);
      return c.get<fixed_bitstring<16, false, true> >();
    }
    fixed_bitstring<16, false, true>& set_serial_num()
    {
      set(types::serial_num);
      return c.get<fixed_bitstring<16, false, true> >();
    }
    broadcast_cancelled_area_list_c& set_broadcast_cancelled_area_list()
    {
      set(types::broadcast_cancelled_area_list);
      return c.get<broadcast_cancelled_area_list_c>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                                                                                   type_;
    choice_buffer_t<broadcast_cancelled_area_list_c, crit_diagnostics_s, fixed_bitstring<16, false, true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSFailureIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pws_fail_ind_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { pws_failed_cell_id_list, global_ran_node_id, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pws_failed_cell_id_list_c& pws_failed_cell_id_list()
    {
      assert_choice_type("PWSFailedCellIDList", type_.to_string(), "Value");
      return c.get<pws_failed_cell_id_list_c>();
    }
    global_ran_node_id_c& global_ran_node_id()
    {
      assert_choice_type("GlobalRANNodeID", type_.to_string(), "Value");
      return c.get<global_ran_node_id_c>();
    }
    const pws_failed_cell_id_list_c& pws_failed_cell_id_list() const
    {
      assert_choice_type("PWSFailedCellIDList", type_.to_string(), "Value");
      return c.get<pws_failed_cell_id_list_c>();
    }
    const global_ran_node_id_c& global_ran_node_id() const
    {
      assert_choice_type("GlobalRANNodeID", type_.to_string(), "Value");
      return c.get<global_ran_node_id_c>();
    }
    pws_failed_cell_id_list_c& set_pws_failed_cell_id_list()
    {
      set(types::pws_failed_cell_id_list);
      return c.get<pws_failed_cell_id_list_c>();
    }
    global_ran_node_id_c& set_global_ran_node_id()
    {
      set(types::global_ran_node_id);
      return c.get<global_ran_node_id_c>();
    }

  private:
    types                                                            type_;
    choice_buffer_t<global_ran_node_id_c, pws_failed_cell_id_list_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSRestartIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pws_restart_ind_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        cell_id_list_for_restart,
        global_ran_node_id,
        tai_list_for_restart,
        emergency_area_id_list_for_restart,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cell_id_list_for_restart_c& cell_id_list_for_restart()
    {
      assert_choice_type("CellIDListForRestart", type_.to_string(), "Value");
      return c.get<cell_id_list_for_restart_c>();
    }
    global_ran_node_id_c& global_ran_node_id()
    {
      assert_choice_type("GlobalRANNodeID", type_.to_string(), "Value");
      return c.get<global_ran_node_id_c>();
    }
    tai_list_for_restart_l& tai_list_for_restart()
    {
      assert_choice_type("TAIListForRestart", type_.to_string(), "Value");
      return c.get<tai_list_for_restart_l>();
    }
    emergency_area_id_list_for_restart_l& emergency_area_id_list_for_restart()
    {
      assert_choice_type("EmergencyAreaIDListForRestart", type_.to_string(), "Value");
      return c.get<emergency_area_id_list_for_restart_l>();
    }
    const cell_id_list_for_restart_c& cell_id_list_for_restart() const
    {
      assert_choice_type("CellIDListForRestart", type_.to_string(), "Value");
      return c.get<cell_id_list_for_restart_c>();
    }
    const global_ran_node_id_c& global_ran_node_id() const
    {
      assert_choice_type("GlobalRANNodeID", type_.to_string(), "Value");
      return c.get<global_ran_node_id_c>();
    }
    const tai_list_for_restart_l& tai_list_for_restart() const
    {
      assert_choice_type("TAIListForRestart", type_.to_string(), "Value");
      return c.get<tai_list_for_restart_l>();
    }
    const emergency_area_id_list_for_restart_l& emergency_area_id_list_for_restart() const
    {
      assert_choice_type("EmergencyAreaIDListForRestart", type_.to_string(), "Value");
      return c.get<emergency_area_id_list_for_restart_l>();
    }
    cell_id_list_for_restart_c& set_cell_id_list_for_restart()
    {
      set(types::cell_id_list_for_restart);
      return c.get<cell_id_list_for_restart_c>();
    }
    global_ran_node_id_c& set_global_ran_node_id()
    {
      set(types::global_ran_node_id);
      return c.get<global_ran_node_id_c>();
    }
    tai_list_for_restart_l& set_tai_list_for_restart()
    {
      set(types::tai_list_for_restart);
      return c.get<tai_list_for_restart_l>();
    }
    emergency_area_id_list_for_restart_l& set_emergency_area_id_list_for_restart()
    {
      set(types::emergency_area_id_list_for_restart);
      return c.get<emergency_area_id_list_for_restart_l>();
    }

  private:
    types type_;
    choice_buffer_t<cell_id_list_for_restart_c,
                    emergency_area_id_list_for_restart_l,
                    global_ran_node_id_c,
                    tai_list_for_restart_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PagingIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct paging_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ue_paging_id,
        paging_drx,
        tai_list_for_paging,
        paging_prio,
        ue_radio_cap_for_paging,
        paging_origin,
        assist_data_for_paging,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_paging_id_c& ue_paging_id()
    {
      assert_choice_type("UEPagingIdentity", type_.to_string(), "Value");
      return c.get<ue_paging_id_c>();
    }
    paging_drx_e& paging_drx()
    {
      assert_choice_type("PagingDRX", type_.to_string(), "Value");
      return c.get<paging_drx_e>();
    }
    tai_list_for_paging_l& tai_list_for_paging()
    {
      assert_choice_type("TAIListForPaging", type_.to_string(), "Value");
      return c.get<tai_list_for_paging_l>();
    }
    paging_prio_e& paging_prio()
    {
      assert_choice_type("PagingPriority", type_.to_string(), "Value");
      return c.get<paging_prio_e>();
    }
    ue_radio_cap_for_paging_s& ue_radio_cap_for_paging()
    {
      assert_choice_type("UERadioCapabilityForPaging", type_.to_string(), "Value");
      return c.get<ue_radio_cap_for_paging_s>();
    }
    paging_origin_e& paging_origin()
    {
      assert_choice_type("PagingOrigin", type_.to_string(), "Value");
      return c.get<paging_origin_e>();
    }
    assist_data_for_paging_s& assist_data_for_paging()
    {
      assert_choice_type("AssistanceDataForPaging", type_.to_string(), "Value");
      return c.get<assist_data_for_paging_s>();
    }
    const ue_paging_id_c& ue_paging_id() const
    {
      assert_choice_type("UEPagingIdentity", type_.to_string(), "Value");
      return c.get<ue_paging_id_c>();
    }
    const paging_drx_e& paging_drx() const
    {
      assert_choice_type("PagingDRX", type_.to_string(), "Value");
      return c.get<paging_drx_e>();
    }
    const tai_list_for_paging_l& tai_list_for_paging() const
    {
      assert_choice_type("TAIListForPaging", type_.to_string(), "Value");
      return c.get<tai_list_for_paging_l>();
    }
    const paging_prio_e& paging_prio() const
    {
      assert_choice_type("PagingPriority", type_.to_string(), "Value");
      return c.get<paging_prio_e>();
    }
    const ue_radio_cap_for_paging_s& ue_radio_cap_for_paging() const
    {
      assert_choice_type("UERadioCapabilityForPaging", type_.to_string(), "Value");
      return c.get<ue_radio_cap_for_paging_s>();
    }
    const paging_origin_e& paging_origin() const
    {
      assert_choice_type("PagingOrigin", type_.to_string(), "Value");
      return c.get<paging_origin_e>();
    }
    const assist_data_for_paging_s& assist_data_for_paging() const
    {
      assert_choice_type("AssistanceDataForPaging", type_.to_string(), "Value");
      return c.get<assist_data_for_paging_s>();
    }
    ue_paging_id_c& set_ue_paging_id()
    {
      set(types::ue_paging_id);
      return c.get<ue_paging_id_c>();
    }
    paging_drx_e& set_paging_drx()
    {
      set(types::paging_drx);
      return c.get<paging_drx_e>();
    }
    tai_list_for_paging_l& set_tai_list_for_paging()
    {
      set(types::tai_list_for_paging);
      return c.get<tai_list_for_paging_l>();
    }
    paging_prio_e& set_paging_prio()
    {
      set(types::paging_prio);
      return c.get<paging_prio_e>();
    }
    ue_radio_cap_for_paging_s& set_ue_radio_cap_for_paging()
    {
      set(types::ue_radio_cap_for_paging);
      return c.get<ue_radio_cap_for_paging_s>();
    }
    paging_origin_e& set_paging_origin()
    {
      set(types::paging_origin);
      return c.get<paging_origin_e>();
    }
    assist_data_for_paging_s& set_assist_data_for_paging()
    {
      set(types::assist_data_for_paging);
      return c.get<assist_data_for_paging_s>();
    }

  private:
    types                                                                                                       type_;
    choice_buffer_t<assist_data_for_paging_s, tai_list_for_paging_l, ue_paging_id_c, ue_radio_cap_for_paging_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PathSwitchRequestAcknowledgeIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct path_switch_request_ack_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ue_security_cap,
        security_context,
        new_security_context_ind,
        pdu_session_res_switched_list,
        pdu_session_res_released_list_ps_ack,
        allowed_nssai,
        core_network_assist_info,
        rrc_inactive_transition_report_request,
        crit_diagnostics,
        redirection_voice_fallback,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    ue_security_cap_s& ue_security_cap()
    {
      assert_choice_type("UESecurityCapabilities", type_.to_string(), "Value");
      return c.get<ue_security_cap_s>();
    }
    security_context_s& security_context()
    {
      assert_choice_type("SecurityContext", type_.to_string(), "Value");
      return c.get<security_context_s>();
    }
    new_security_context_ind_e& new_security_context_ind()
    {
      assert_choice_type("NewSecurityContextInd", type_.to_string(), "Value");
      return c.get<new_security_context_ind_e>();
    }
    pdu_session_res_switched_list_l& pdu_session_res_switched_list()
    {
      assert_choice_type("PDUSessionResourceSwitchedList", type_.to_string(), "Value");
      return c.get<pdu_session_res_switched_list_l>();
    }
    pdu_session_res_released_list_ps_ack_l& pdu_session_res_released_list_ps_ack()
    {
      assert_choice_type("PDUSessionResourceReleasedListPSAck", type_.to_string(), "Value");
      return c.get<pdu_session_res_released_list_ps_ack_l>();
    }
    allowed_nssai_l& allowed_nssai()
    {
      assert_choice_type("AllowedNSSAI", type_.to_string(), "Value");
      return c.get<allowed_nssai_l>();
    }
    core_network_assist_info_s& core_network_assist_info()
    {
      assert_choice_type("CoreNetworkAssistanceInformation", type_.to_string(), "Value");
      return c.get<core_network_assist_info_s>();
    }
    rrc_inactive_transition_report_request_e& rrc_inactive_transition_report_request()
    {
      assert_choice_type("RRCInactiveTransitionReportRequest", type_.to_string(), "Value");
      return c.get<rrc_inactive_transition_report_request_e>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    redirection_voice_fallback_e& redirection_voice_fallback()
    {
      assert_choice_type("RedirectionVoiceFallback", type_.to_string(), "Value");
      return c.get<redirection_voice_fallback_e>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const ue_security_cap_s& ue_security_cap() const
    {
      assert_choice_type("UESecurityCapabilities", type_.to_string(), "Value");
      return c.get<ue_security_cap_s>();
    }
    const security_context_s& security_context() const
    {
      assert_choice_type("SecurityContext", type_.to_string(), "Value");
      return c.get<security_context_s>();
    }
    const new_security_context_ind_e& new_security_context_ind() const
    {
      assert_choice_type("NewSecurityContextInd", type_.to_string(), "Value");
      return c.get<new_security_context_ind_e>();
    }
    const pdu_session_res_switched_list_l& pdu_session_res_switched_list() const
    {
      assert_choice_type("PDUSessionResourceSwitchedList", type_.to_string(), "Value");
      return c.get<pdu_session_res_switched_list_l>();
    }
    const pdu_session_res_released_list_ps_ack_l& pdu_session_res_released_list_ps_ack() const
    {
      assert_choice_type("PDUSessionResourceReleasedListPSAck", type_.to_string(), "Value");
      return c.get<pdu_session_res_released_list_ps_ack_l>();
    }
    const allowed_nssai_l& allowed_nssai() const
    {
      assert_choice_type("AllowedNSSAI", type_.to_string(), "Value");
      return c.get<allowed_nssai_l>();
    }
    const core_network_assist_info_s& core_network_assist_info() const
    {
      assert_choice_type("CoreNetworkAssistanceInformation", type_.to_string(), "Value");
      return c.get<core_network_assist_info_s>();
    }
    const rrc_inactive_transition_report_request_e& rrc_inactive_transition_report_request() const
    {
      assert_choice_type("RRCInactiveTransitionReportRequest", type_.to_string(), "Value");
      return c.get<rrc_inactive_transition_report_request_e>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const redirection_voice_fallback_e& redirection_voice_fallback() const
    {
      assert_choice_type("RedirectionVoiceFallback", type_.to_string(), "Value");
      return c.get<redirection_voice_fallback_e>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    ue_security_cap_s& set_ue_security_cap()
    {
      set(types::ue_security_cap);
      return c.get<ue_security_cap_s>();
    }
    security_context_s& set_security_context()
    {
      set(types::security_context);
      return c.get<security_context_s>();
    }
    new_security_context_ind_e& set_new_security_context_ind()
    {
      set(types::new_security_context_ind);
      return c.get<new_security_context_ind_e>();
    }
    pdu_session_res_switched_list_l& set_pdu_session_res_switched_list()
    {
      set(types::pdu_session_res_switched_list);
      return c.get<pdu_session_res_switched_list_l>();
    }
    pdu_session_res_released_list_ps_ack_l& set_pdu_session_res_released_list_ps_ack()
    {
      set(types::pdu_session_res_released_list_ps_ack);
      return c.get<pdu_session_res_released_list_ps_ack_l>();
    }
    allowed_nssai_l& set_allowed_nssai()
    {
      set(types::allowed_nssai);
      return c.get<allowed_nssai_l>();
    }
    core_network_assist_info_s& set_core_network_assist_info()
    {
      set(types::core_network_assist_info);
      return c.get<core_network_assist_info_s>();
    }
    rrc_inactive_transition_report_request_e& set_rrc_inactive_transition_report_request()
    {
      set(types::rrc_inactive_transition_report_request);
      return c.get<rrc_inactive_transition_report_request_e>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }
    redirection_voice_fallback_e& set_redirection_voice_fallback()
    {
      set(types::redirection_voice_fallback);
      return c.get<redirection_voice_fallback_e>();
    }

  private:
    types type_;
    choice_buffer_t<allowed_nssai_l,
                    core_network_assist_info_s,
                    crit_diagnostics_s,
                    pdu_session_res_released_list_ps_ack_l,
                    pdu_session_res_switched_list_l,
                    security_context_s,
                    ue_security_cap_s>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PathSwitchRequestFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct path_switch_request_fail_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_released_list_ps_fail,
        crit_diagnostics,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    pdu_session_res_released_list_ps_fail_l& pdu_session_res_released_list_ps_fail()
    {
      assert_choice_type("PDUSessionResourceReleasedListPSFail", type_.to_string(), "Value");
      return c.get<pdu_session_res_released_list_ps_fail_l>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const pdu_session_res_released_list_ps_fail_l& pdu_session_res_released_list_ps_fail() const
    {
      assert_choice_type("PDUSessionResourceReleasedListPSFail", type_.to_string(), "Value");
      return c.get<pdu_session_res_released_list_ps_fail_l>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    pdu_session_res_released_list_ps_fail_l& set_pdu_session_res_released_list_ps_fail()
    {
      set(types::pdu_session_res_released_list_ps_fail);
      return c.get<pdu_session_res_released_list_ps_fail_l>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                                                        type_;
    choice_buffer_t<crit_diagnostics_s, pdu_session_res_released_list_ps_fail_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PathSwitchRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct path_switch_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ran_ue_ngap_id,
        source_amf_ue_ngap_id,
        user_location_info,
        ue_security_cap,
        pdu_session_res_to_be_switched_dl_list,
        pdu_session_res_failed_to_setup_list_ps_req,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& source_amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    user_location_info_c& user_location_info()
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    ue_security_cap_s& ue_security_cap()
    {
      assert_choice_type("UESecurityCapabilities", type_.to_string(), "Value");
      return c.get<ue_security_cap_s>();
    }
    pdu_session_res_to_be_switched_dl_list_l& pdu_session_res_to_be_switched_dl_list()
    {
      assert_choice_type("PDUSessionResourceToBeSwitchedDLList", type_.to_string(), "Value");
      return c.get<pdu_session_res_to_be_switched_dl_list_l>();
    }
    pdu_session_res_failed_to_setup_list_ps_req_l& pdu_session_res_failed_to_setup_list_ps_req()
    {
      assert_choice_type("PDUSessionResourceFailedToSetupListPSReq", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_setup_list_ps_req_l>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& source_amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const user_location_info_c& user_location_info() const
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    const ue_security_cap_s& ue_security_cap() const
    {
      assert_choice_type("UESecurityCapabilities", type_.to_string(), "Value");
      return c.get<ue_security_cap_s>();
    }
    const pdu_session_res_to_be_switched_dl_list_l& pdu_session_res_to_be_switched_dl_list() const
    {
      assert_choice_type("PDUSessionResourceToBeSwitchedDLList", type_.to_string(), "Value");
      return c.get<pdu_session_res_to_be_switched_dl_list_l>();
    }
    const pdu_session_res_failed_to_setup_list_ps_req_l& pdu_session_res_failed_to_setup_list_ps_req() const
    {
      assert_choice_type("PDUSessionResourceFailedToSetupListPSReq", type_.to_string(), "Value");
      return c.get<pdu_session_res_failed_to_setup_list_ps_req_l>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_source_amf_ue_ngap_id()
    {
      set(types::source_amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    user_location_info_c& set_user_location_info()
    {
      set(types::user_location_info);
      return c.get<user_location_info_c>();
    }
    ue_security_cap_s& set_ue_security_cap()
    {
      set(types::ue_security_cap);
      return c.get<ue_security_cap_s>();
    }
    pdu_session_res_to_be_switched_dl_list_l& set_pdu_session_res_to_be_switched_dl_list()
    {
      set(types::pdu_session_res_to_be_switched_dl_list);
      return c.get<pdu_session_res_to_be_switched_dl_list_l>();
    }
    pdu_session_res_failed_to_setup_list_ps_req_l& set_pdu_session_res_failed_to_setup_list_ps_req()
    {
      set(types::pdu_session_res_failed_to_setup_list_ps_req);
      return c.get<pdu_session_res_failed_to_setup_list_ps_req_l>();
    }

  private:
    types type_;
    choice_buffer_t<pdu_session_res_failed_to_setup_list_ps_req_l,
                    pdu_session_res_to_be_switched_dl_list_l,
                    ue_security_cap_s,
                    user_location_info_c>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PrivateIE-Container{NGAP-PRIVATE-IES : IEsSetParam} ::= SEQUENCE (SIZE (1..65535)) OF PrivateIE-Field
template <class ies_set_param>
using private_ie_container_l = dyn_array<private_ie_field_s<ies_set_param> >;

struct ngap_private_ies_empty_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };
};
// PrivateMessageIEs ::= OBJECT SET OF NGAP-PRIVATE-IES
typedef ngap_private_ies_empty_o private_msg_ies_o;

// RANConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ran_cfg_upd_ack_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::crit_diagnostics; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    crit_diagnostics_s&       crit_diagnostics() { return c; }
    const crit_diagnostics_s& crit_diagnostics() const { return c; }

  private:
    crit_diagnostics_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RANConfigurationUpdateFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ran_cfg_upd_fail_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, time_to_wait, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    time_to_wait_e& time_to_wait()
    {
      assert_choice_type("TimeToWait", type_.to_string(), "Value");
      return c.get<time_to_wait_e>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const time_to_wait_e& time_to_wait() const
    {
      assert_choice_type("TimeToWait", type_.to_string(), "Value");
      return c.get<time_to_wait_e>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }
    time_to_wait_e& set_time_to_wait()
    {
      set(types::time_to_wait);
      return c.get<time_to_wait_e>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RANConfigurationUpdateIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ran_cfg_upd_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ran_node_name, supported_ta_list, default_paging_drx, global_ran_node_id, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    printable_string<1, 150, true, true>& ran_node_name()
    {
      assert_choice_type("PrintableString", type_.to_string(), "Value");
      return c.get<printable_string<1, 150, true, true> >();
    }
    supported_ta_list_l& supported_ta_list()
    {
      assert_choice_type("SupportedTAList", type_.to_string(), "Value");
      return c.get<supported_ta_list_l>();
    }
    paging_drx_e& default_paging_drx()
    {
      assert_choice_type("PagingDRX", type_.to_string(), "Value");
      return c.get<paging_drx_e>();
    }
    global_ran_node_id_c& global_ran_node_id()
    {
      assert_choice_type("GlobalRANNodeID", type_.to_string(), "Value");
      return c.get<global_ran_node_id_c>();
    }
    const printable_string<1, 150, true, true>& ran_node_name() const
    {
      assert_choice_type("PrintableString", type_.to_string(), "Value");
      return c.get<printable_string<1, 150, true, true> >();
    }
    const supported_ta_list_l& supported_ta_list() const
    {
      assert_choice_type("SupportedTAList", type_.to_string(), "Value");
      return c.get<supported_ta_list_l>();
    }
    const paging_drx_e& default_paging_drx() const
    {
      assert_choice_type("PagingDRX", type_.to_string(), "Value");
      return c.get<paging_drx_e>();
    }
    const global_ran_node_id_c& global_ran_node_id() const
    {
      assert_choice_type("GlobalRANNodeID", type_.to_string(), "Value");
      return c.get<global_ran_node_id_c>();
    }
    printable_string<1, 150, true, true>& set_ran_node_name()
    {
      set(types::ran_node_name);
      return c.get<printable_string<1, 150, true, true> >();
    }
    supported_ta_list_l& set_supported_ta_list()
    {
      set(types::supported_ta_list);
      return c.get<supported_ta_list_l>();
    }
    paging_drx_e& set_default_paging_drx()
    {
      set(types::default_paging_drx);
      return c.get<paging_drx_e>();
    }
    global_ran_node_id_c& set_global_ran_node_id()
    {
      set(types::global_ran_node_id);
      return c.get<global_ran_node_id_c>();
    }

  private:
    types                                                                                            type_;
    choice_buffer_t<global_ran_node_id_c, printable_string<1, 150, true, true>, supported_ta_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RRCInactiveTransitionReportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct rrc_inactive_transition_report_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, rrc_state, user_location_info, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    rrc_state_e& rrc_state()
    {
      assert_choice_type("RRCState", type_.to_string(), "Value");
      return c.get<rrc_state_e>();
    }
    user_location_info_c& user_location_info()
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const rrc_state_e& rrc_state() const
    {
      assert_choice_type("RRCState", type_.to_string(), "Value");
      return c.get<rrc_state_e>();
    }
    const user_location_info_c& user_location_info() const
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    rrc_state_e& set_rrc_state()
    {
      set(types::rrc_state);
      return c.get<rrc_state_e>();
    }
    user_location_info_c& set_user_location_info()
    {
      set(types::user_location_info);
      return c.get<user_location_info_c>();
    }

  private:
    types                                 type_;
    choice_buffer_t<user_location_info_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RerouteNASRequest-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct reroute_nas_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ran_ue_ngap_id, amf_ue_ngap_id, ngap_msg, amf_set_id, allowed_nssai, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& ngap_msg()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    fixed_bitstring<10, false, true>& amf_set_id()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<10, false, true> >();
    }
    allowed_nssai_l& allowed_nssai()
    {
      assert_choice_type("AllowedNSSAI", type_.to_string(), "Value");
      return c.get<allowed_nssai_l>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const unbounded_octstring<true>& ngap_msg() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const fixed_bitstring<10, false, true>& amf_set_id() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<10, false, true> >();
    }
    const allowed_nssai_l& allowed_nssai() const
    {
      assert_choice_type("AllowedNSSAI", type_.to_string(), "Value");
      return c.get<allowed_nssai_l>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& set_ngap_msg()
    {
      set(types::ngap_msg);
      return c.get<unbounded_octstring<true> >();
    }
    fixed_bitstring<10, false, true>& set_amf_set_id()
    {
      set(types::amf_set_id);
      return c.get<fixed_bitstring<10, false, true> >();
    }
    allowed_nssai_l& set_allowed_nssai()
    {
      set(types::allowed_nssai);
      return c.get<allowed_nssai_l>();
    }

  private:
    types                                                                                          type_;
    choice_buffer_t<allowed_nssai_l, fixed_bitstring<10, false, true>, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SecondaryRATDataUsageReportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct secondary_rat_data_usage_report_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, pdu_session_res_secondary_ratusage_list, ho_flag, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    pdu_session_res_secondary_ratusage_list_l& pdu_session_res_secondary_ratusage_list()
    {
      assert_choice_type("PDUSessionResourceSecondaryRATUsageList", type_.to_string(), "Value");
      return c.get<pdu_session_res_secondary_ratusage_list_l>();
    }
    ho_flag_e& ho_flag()
    {
      assert_choice_type("HandoverFlag", type_.to_string(), "Value");
      return c.get<ho_flag_e>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const pdu_session_res_secondary_ratusage_list_l& pdu_session_res_secondary_ratusage_list() const
    {
      assert_choice_type("PDUSessionResourceSecondaryRATUsageList", type_.to_string(), "Value");
      return c.get<pdu_session_res_secondary_ratusage_list_l>();
    }
    const ho_flag_e& ho_flag() const
    {
      assert_choice_type("HandoverFlag", type_.to_string(), "Value");
      return c.get<ho_flag_e>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    pdu_session_res_secondary_ratusage_list_l& set_pdu_session_res_secondary_ratusage_list()
    {
      set(types::pdu_session_res_secondary_ratusage_list);
      return c.get<pdu_session_res_secondary_ratusage_list_l>();
    }
    ho_flag_e& set_ho_flag()
    {
      set(types::ho_flag);
      return c.get<ho_flag_e>();
    }

  private:
    types                                                      type_;
    choice_buffer_t<pdu_session_res_secondary_ratusage_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TraceFailureIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct trace_fail_ind_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ngran_trace_id, cause, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    fixed_octstring<8, true>& ngran_trace_id()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<fixed_octstring<8, true> >();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const fixed_octstring<8, true>& ngran_trace_id() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<fixed_octstring<8, true> >();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    fixed_octstring<8, true>& set_ngran_trace_id()
    {
      set(types::ngran_trace_id);
      return c.get<fixed_octstring<8, true> >();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }

  private:
    types                                               type_;
    choice_buffer_t<cause_c, fixed_octstring<8, true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TraceStartIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct trace_start_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, trace_activation, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    trace_activation_s& trace_activation()
    {
      assert_choice_type("TraceActivation", type_.to_string(), "Value");
      return c.get<trace_activation_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const trace_activation_s& trace_activation() const
    {
      assert_choice_type("TraceActivation", type_.to_string(), "Value");
      return c.get<trace_activation_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    trace_activation_s& set_trace_activation()
    {
      set(types::trace_activation);
      return c.get<trace_activation_s>();
    }

  private:
    types                               type_;
    choice_buffer_t<trace_activation_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextModificationFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_mod_fail_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, cause, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextModificationRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_mod_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ran_paging_prio,
        security_key,
        idx_to_rfsp,
        ue_aggregate_maximum_bit_rate,
        ue_security_cap,
        core_network_assist_info,
        emergency_fallback_ind,
        new_amf_ue_ngap_id,
        rrc_inactive_transition_report_request,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint16_t& ran_paging_prio()
    {
      assert_choice_type("INTEGER (1..256)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    fixed_bitstring<256, false, true>& security_key()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<256, false, true> >();
    }
    uint16_t& idx_to_rfsp()
    {
      assert_choice_type("INTEGER (1..256,...)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    ue_aggregate_maximum_bit_rate_s& ue_aggregate_maximum_bit_rate()
    {
      assert_choice_type("UEAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    ue_security_cap_s& ue_security_cap()
    {
      assert_choice_type("UESecurityCapabilities", type_.to_string(), "Value");
      return c.get<ue_security_cap_s>();
    }
    core_network_assist_info_s& core_network_assist_info()
    {
      assert_choice_type("CoreNetworkAssistanceInformation", type_.to_string(), "Value");
      return c.get<core_network_assist_info_s>();
    }
    emergency_fallback_ind_s& emergency_fallback_ind()
    {
      assert_choice_type("EmergencyFallbackIndicator", type_.to_string(), "Value");
      return c.get<emergency_fallback_ind_s>();
    }
    uint64_t& new_amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    rrc_inactive_transition_report_request_e& rrc_inactive_transition_report_request()
    {
      assert_choice_type("RRCInactiveTransitionReportRequest", type_.to_string(), "Value");
      return c.get<rrc_inactive_transition_report_request_e>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint16_t& ran_paging_prio() const
    {
      assert_choice_type("INTEGER (1..256)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    const fixed_bitstring<256, false, true>& security_key() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<256, false, true> >();
    }
    const uint16_t& idx_to_rfsp() const
    {
      assert_choice_type("INTEGER (1..256,...)", type_.to_string(), "Value");
      return c.get<uint16_t>();
    }
    const ue_aggregate_maximum_bit_rate_s& ue_aggregate_maximum_bit_rate() const
    {
      assert_choice_type("UEAggregateMaximumBitRate", type_.to_string(), "Value");
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    const ue_security_cap_s& ue_security_cap() const
    {
      assert_choice_type("UESecurityCapabilities", type_.to_string(), "Value");
      return c.get<ue_security_cap_s>();
    }
    const core_network_assist_info_s& core_network_assist_info() const
    {
      assert_choice_type("CoreNetworkAssistanceInformation", type_.to_string(), "Value");
      return c.get<core_network_assist_info_s>();
    }
    const emergency_fallback_ind_s& emergency_fallback_ind() const
    {
      assert_choice_type("EmergencyFallbackIndicator", type_.to_string(), "Value");
      return c.get<emergency_fallback_ind_s>();
    }
    const uint64_t& new_amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const rrc_inactive_transition_report_request_e& rrc_inactive_transition_report_request() const
    {
      assert_choice_type("RRCInactiveTransitionReportRequest", type_.to_string(), "Value");
      return c.get<rrc_inactive_transition_report_request_e>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint16_t& set_ran_paging_prio()
    {
      set(types::ran_paging_prio);
      return c.get<uint16_t>();
    }
    fixed_bitstring<256, false, true>& set_security_key()
    {
      set(types::security_key);
      return c.get<fixed_bitstring<256, false, true> >();
    }
    uint16_t& set_idx_to_rfsp()
    {
      set(types::idx_to_rfsp);
      return c.get<uint16_t>();
    }
    ue_aggregate_maximum_bit_rate_s& set_ue_aggregate_maximum_bit_rate()
    {
      set(types::ue_aggregate_maximum_bit_rate);
      return c.get<ue_aggregate_maximum_bit_rate_s>();
    }
    ue_security_cap_s& set_ue_security_cap()
    {
      set(types::ue_security_cap);
      return c.get<ue_security_cap_s>();
    }
    core_network_assist_info_s& set_core_network_assist_info()
    {
      set(types::core_network_assist_info);
      return c.get<core_network_assist_info_s>();
    }
    emergency_fallback_ind_s& set_emergency_fallback_ind()
    {
      set(types::emergency_fallback_ind);
      return c.get<emergency_fallback_ind_s>();
    }
    uint64_t& set_new_amf_ue_ngap_id()
    {
      set(types::new_amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    rrc_inactive_transition_report_request_e& set_rrc_inactive_transition_report_request()
    {
      set(types::rrc_inactive_transition_report_request);
      return c.get<rrc_inactive_transition_report_request_e>();
    }

  private:
    types type_;
    choice_buffer_t<core_network_assist_info_s,
                    emergency_fallback_ind_s,
                    fixed_bitstring<256, false, true>,
                    ue_aggregate_maximum_bit_rate_s,
                    ue_security_cap_s>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextModificationResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_mod_resp_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, rrc_state, user_location_info, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    rrc_state_e& rrc_state()
    {
      assert_choice_type("RRCState", type_.to_string(), "Value");
      return c.get<rrc_state_e>();
    }
    user_location_info_c& user_location_info()
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const rrc_state_e& rrc_state() const
    {
      assert_choice_type("RRCState", type_.to_string(), "Value");
      return c.get<rrc_state_e>();
    }
    const user_location_info_c& user_location_info() const
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    rrc_state_e& set_rrc_state()
    {
      set(types::rrc_state);
      return c.get<rrc_state_e>();
    }
    user_location_info_c& set_user_location_info()
    {
      set(types::user_location_info);
      return c.get<user_location_info_c>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                                     type_;
    choice_buffer_t<crit_diagnostics_s, user_location_info_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextReleaseCommand-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_release_cmd_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_ngap_ids, cause, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_ngap_ids_c& ue_ngap_ids()
    {
      assert_choice_type("UE-NGAP-IDs", type_.to_string(), "Value");
      return c.get<ue_ngap_ids_c>();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const ue_ngap_ids_c& ue_ngap_ids() const
    {
      assert_choice_type("UE-NGAP-IDs", type_.to_string(), "Value");
      return c.get<ue_ngap_ids_c>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    ue_ngap_ids_c& set_ue_ngap_ids()
    {
      set(types::ue_ngap_ids);
      return c.get<ue_ngap_ids_c>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }

  private:
    types                                   type_;
    choice_buffer_t<cause_c, ue_ngap_ids_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextReleaseComplete-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_release_complete_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        user_location_info,
        info_on_recommended_cells_and_ran_nodes_for_paging,
        pdu_session_res_list_cxt_rel_cpl,
        crit_diagnostics,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    user_location_info_c& user_location_info()
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    info_on_recommended_cells_and_ran_nodes_for_paging_s& info_on_recommended_cells_and_ran_nodes_for_paging()
    {
      assert_choice_type("InfoOnRecommendedCellsAndRANNodesForPaging", type_.to_string(), "Value");
      return c.get<info_on_recommended_cells_and_ran_nodes_for_paging_s>();
    }
    pdu_session_res_list_cxt_rel_cpl_l& pdu_session_res_list_cxt_rel_cpl()
    {
      assert_choice_type("PDUSessionResourceListCxtRelCpl", type_.to_string(), "Value");
      return c.get<pdu_session_res_list_cxt_rel_cpl_l>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const user_location_info_c& user_location_info() const
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    const info_on_recommended_cells_and_ran_nodes_for_paging_s&
    info_on_recommended_cells_and_ran_nodes_for_paging() const
    {
      assert_choice_type("InfoOnRecommendedCellsAndRANNodesForPaging", type_.to_string(), "Value");
      return c.get<info_on_recommended_cells_and_ran_nodes_for_paging_s>();
    }
    const pdu_session_res_list_cxt_rel_cpl_l& pdu_session_res_list_cxt_rel_cpl() const
    {
      assert_choice_type("PDUSessionResourceListCxtRelCpl", type_.to_string(), "Value");
      return c.get<pdu_session_res_list_cxt_rel_cpl_l>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    user_location_info_c& set_user_location_info()
    {
      set(types::user_location_info);
      return c.get<user_location_info_c>();
    }
    info_on_recommended_cells_and_ran_nodes_for_paging_s& set_info_on_recommended_cells_and_ran_nodes_for_paging()
    {
      set(types::info_on_recommended_cells_and_ran_nodes_for_paging);
      return c.get<info_on_recommended_cells_and_ran_nodes_for_paging_s>();
    }
    pdu_session_res_list_cxt_rel_cpl_l& set_pdu_session_res_list_cxt_rel_cpl()
    {
      set(types::pdu_session_res_list_cxt_rel_cpl);
      return c.get<pdu_session_res_list_cxt_rel_cpl_l>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    info_on_recommended_cells_and_ran_nodes_for_paging_s,
                    pdu_session_res_list_cxt_rel_cpl_l,
                    user_location_info_c>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextReleaseRequest-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_release_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, pdu_session_res_list_cxt_rel_req, cause, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    pdu_session_res_list_cxt_rel_req_l& pdu_session_res_list_cxt_rel_req()
    {
      assert_choice_type("PDUSessionResourceListCxtRelReq", type_.to_string(), "Value");
      return c.get<pdu_session_res_list_cxt_rel_req_l>();
    }
    cause_c& cause()
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const pdu_session_res_list_cxt_rel_req_l& pdu_session_res_list_cxt_rel_req() const
    {
      assert_choice_type("PDUSessionResourceListCxtRelReq", type_.to_string(), "Value");
      return c.get<pdu_session_res_list_cxt_rel_req_l>();
    }
    const cause_c& cause() const
    {
      assert_choice_type("Cause", type_.to_string(), "Value");
      return c.get<cause_c>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    pdu_session_res_list_cxt_rel_req_l& set_pdu_session_res_list_cxt_rel_req()
    {
      set(types::pdu_session_res_list_cxt_rel_req);
      return c.get<pdu_session_res_list_cxt_rel_req_l>();
    }
    cause_c& set_cause()
    {
      set(types::cause);
      return c.get<cause_c>();
    }

  private:
    types                                                        type_;
    choice_buffer_t<cause_c, pdu_session_res_list_cxt_rel_req_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UERadioCapabilityCheckRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_radio_cap_check_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ue_radio_cap, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& ue_radio_cap()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const unbounded_octstring<true>& ue_radio_cap() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& set_ue_radio_cap()
    {
      set(types::ue_radio_cap);
      return c.get<unbounded_octstring<true> >();
    }

  private:
    types                                       type_;
    choice_buffer_t<unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UERadioCapabilityCheckResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_radio_cap_check_resp_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ims_voice_support_ind, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    ims_voice_support_ind_e& ims_voice_support_ind()
    {
      assert_choice_type("IMSVoiceSupportIndicator", type_.to_string(), "Value");
      return c.get<ims_voice_support_ind_e>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const ims_voice_support_ind_e& ims_voice_support_ind() const
    {
      assert_choice_type("IMSVoiceSupportIndicator", type_.to_string(), "Value");
      return c.get<ims_voice_support_ind_e>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    ims_voice_support_ind_e& set_ims_voice_support_ind()
    {
      set(types::ims_voice_support_ind);
      return c.get<ims_voice_support_ind_e>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UERadioCapabilityInfoIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_radio_cap_info_ind_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ue_radio_cap, ue_radio_cap_for_paging, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& ue_radio_cap()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    ue_radio_cap_for_paging_s& ue_radio_cap_for_paging()
    {
      assert_choice_type("UERadioCapabilityForPaging", type_.to_string(), "Value");
      return c.get<ue_radio_cap_for_paging_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const unbounded_octstring<true>& ue_radio_cap() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const ue_radio_cap_for_paging_s& ue_radio_cap_for_paging() const
    {
      assert_choice_type("UERadioCapabilityForPaging", type_.to_string(), "Value");
      return c.get<ue_radio_cap_for_paging_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& set_ue_radio_cap()
    {
      set(types::ue_radio_cap);
      return c.get<unbounded_octstring<true> >();
    }
    ue_radio_cap_for_paging_s& set_ue_radio_cap_for_paging()
    {
      set(types::ue_radio_cap_for_paging);
      return c.get<ue_radio_cap_for_paging_s>();
    }

  private:
    types                                                                  type_;
    choice_buffer_t<ue_radio_cap_for_paging_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UETNLABindingReleaseRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct uetnla_binding_release_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkNASTransport-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ul_nas_transport_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, nas_pdu, user_location_info, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& nas_pdu()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    user_location_info_c& user_location_info()
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const unbounded_octstring<true>& nas_pdu() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const user_location_info_c& user_location_info() const
    {
      assert_choice_type("UserLocationInformation", type_.to_string(), "Value");
      return c.get<user_location_info_c>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& set_nas_pdu()
    {
      set(types::nas_pdu);
      return c.get<unbounded_octstring<true> >();
    }
    user_location_info_c& set_user_location_info()
    {
      set(types::user_location_info);
      return c.get<user_location_info_c>();
    }

  private:
    types                                                            type_;
    choice_buffer_t<unbounded_octstring<true>, user_location_info_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkNonUEAssociatedNRPPaTransportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ul_non_ueassociated_nrp_pa_transport_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { routing_id, nrp_pa_pdu, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>& routing_id()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    unbounded_octstring<true>& nrp_pa_pdu()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const unbounded_octstring<true>& routing_id() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const unbounded_octstring<true>& nrp_pa_pdu() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    unbounded_octstring<true>& set_routing_id()
    {
      set(types::routing_id);
      return c.get<unbounded_octstring<true> >();
    }
    unbounded_octstring<true>& set_nrp_pa_pdu()
    {
      set(types::nrp_pa_pdu);
      return c.get<unbounded_octstring<true> >();
    }

  private:
    types                                       type_;
    choice_buffer_t<unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkRANConfigurationTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ul_ran_cfg_transfer_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { son_cfg_transfer_ul, endc_son_cfg_transfer_ul, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    son_cfg_transfer_s& son_cfg_transfer_ul()
    {
      assert_choice_type("SONConfigurationTransfer", type_.to_string(), "Value");
      return c.get<son_cfg_transfer_s>();
    }
    unbounded_octstring<true>& endc_son_cfg_transfer_ul()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const son_cfg_transfer_s& son_cfg_transfer_ul() const
    {
      assert_choice_type("SONConfigurationTransfer", type_.to_string(), "Value");
      return c.get<son_cfg_transfer_s>();
    }
    const unbounded_octstring<true>& endc_son_cfg_transfer_ul() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    son_cfg_transfer_s& set_son_cfg_transfer_ul()
    {
      set(types::son_cfg_transfer_ul);
      return c.get<son_cfg_transfer_s>();
    }
    unbounded_octstring<true>& set_endc_son_cfg_transfer_ul()
    {
      set(types::endc_son_cfg_transfer_ul);
      return c.get<unbounded_octstring<true> >();
    }

  private:
    types                                                           type_;
    choice_buffer_t<son_cfg_transfer_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkRANStatusTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ul_ran_status_transfer_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ran_status_transfer_transparent_container, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    ran_status_transfer_transparent_container_s& ran_status_transfer_transparent_container()
    {
      assert_choice_type("RANStatusTransfer-TransparentContainer", type_.to_string(), "Value");
      return c.get<ran_status_transfer_transparent_container_s>();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const ran_status_transfer_transparent_container_s& ran_status_transfer_transparent_container() const
    {
      assert_choice_type("RANStatusTransfer-TransparentContainer", type_.to_string(), "Value");
      return c.get<ran_status_transfer_transparent_container_s>();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    ran_status_transfer_transparent_container_s& set_ran_status_transfer_transparent_container()
    {
      set(types::ran_status_transfer_transparent_container);
      return c.get<ran_status_transfer_transparent_container_s>();
    }

  private:
    types                                                        type_;
    choice_buffer_t<ran_status_transfer_transparent_container_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkUEAssociatedNRPPaTransportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ul_ueassociated_nrp_pa_transport_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, routing_id, nrp_pa_pdu, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t& amf_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    uint64_t& ran_ue_ngap_id()
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& routing_id()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    unbounded_octstring<true>& nrp_pa_pdu()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const uint64_t& amf_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..1099511627775)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const uint64_t& ran_ue_ngap_id() const
    {
      assert_choice_type("INTEGER (0..4294967295)", type_.to_string(), "Value");
      return c.get<uint64_t>();
    }
    const unbounded_octstring<true>& routing_id() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const unbounded_octstring<true>& nrp_pa_pdu() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    uint64_t& set_amf_ue_ngap_id()
    {
      set(types::amf_ue_ngap_id);
      return c.get<uint64_t>();
    }
    uint64_t& set_ran_ue_ngap_id()
    {
      set(types::ran_ue_ngap_id);
      return c.get<uint64_t>();
    }
    unbounded_octstring<true>& set_routing_id()
    {
      set(types::routing_id);
      return c.get<unbounded_octstring<true> >();
    }
    unbounded_octstring<true>& set_nrp_pa_pdu()
    {
      set(types::nrp_pa_pdu);
      return c.get<unbounded_octstring<true> >();
    }

  private:
    types                                       type_;
    choice_buffer_t<unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// WriteReplaceWarningRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct write_replace_warning_request_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        msg_id,
        serial_num,
        warning_area_list,
        repeat_period,
        nof_broadcasts_requested,
        warning_type,
        warning_security_info,
        data_coding_scheme,
        warning_msg_contents,
        concurrent_warning_msg_ind,
        warning_area_coordinates,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>& msg_id()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    fixed_bitstring<16, false, true>& serial_num()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    warning_area_list_c& warning_area_list()
    {
      assert_choice_type("WarningAreaList", type_.to_string(), "Value");
      return c.get<warning_area_list_c>();
    }
    uint32_t& repeat_period()
    {
      assert_choice_type("INTEGER (0..131071)", type_.to_string(), "Value");
      return c.get<uint32_t>();
    }
    uint32_t& nof_broadcasts_requested()
    {
      assert_choice_type("INTEGER (0..65535)", type_.to_string(), "Value");
      return c.get<uint32_t>();
    }
    fixed_octstring<2, true>& warning_type()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<fixed_octstring<2, true> >();
    }
    fixed_octstring<50, true>& warning_security_info()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<fixed_octstring<50, true> >();
    }
    fixed_bitstring<8, false, true>& data_coding_scheme()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<8, false, true> >();
    }
    unbounded_octstring<true>& warning_msg_contents()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    concurrent_warning_msg_ind_e& concurrent_warning_msg_ind()
    {
      assert_choice_type("ConcurrentWarningMessageInd", type_.to_string(), "Value");
      return c.get<concurrent_warning_msg_ind_e>();
    }
    unbounded_octstring<true>& warning_area_coordinates()
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const fixed_bitstring<16, false, true>& msg_id() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    const fixed_bitstring<16, false, true>& serial_num() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    const warning_area_list_c& warning_area_list() const
    {
      assert_choice_type("WarningAreaList", type_.to_string(), "Value");
      return c.get<warning_area_list_c>();
    }
    const uint32_t& repeat_period() const
    {
      assert_choice_type("INTEGER (0..131071)", type_.to_string(), "Value");
      return c.get<uint32_t>();
    }
    const uint32_t& nof_broadcasts_requested() const
    {
      assert_choice_type("INTEGER (0..65535)", type_.to_string(), "Value");
      return c.get<uint32_t>();
    }
    const fixed_octstring<2, true>& warning_type() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<fixed_octstring<2, true> >();
    }
    const fixed_octstring<50, true>& warning_security_info() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<fixed_octstring<50, true> >();
    }
    const fixed_bitstring<8, false, true>& data_coding_scheme() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<8, false, true> >();
    }
    const unbounded_octstring<true>& warning_msg_contents() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    const concurrent_warning_msg_ind_e& concurrent_warning_msg_ind() const
    {
      assert_choice_type("ConcurrentWarningMessageInd", type_.to_string(), "Value");
      return c.get<concurrent_warning_msg_ind_e>();
    }
    const unbounded_octstring<true>& warning_area_coordinates() const
    {
      assert_choice_type("OCTET STRING", type_.to_string(), "Value");
      return c.get<unbounded_octstring<true> >();
    }
    fixed_bitstring<16, false, true>& set_msg_id()
    {
      set(types::msg_id);
      return c.get<fixed_bitstring<16, false, true> >();
    }
    fixed_bitstring<16, false, true>& set_serial_num()
    {
      set(types::serial_num);
      return c.get<fixed_bitstring<16, false, true> >();
    }
    warning_area_list_c& set_warning_area_list()
    {
      set(types::warning_area_list);
      return c.get<warning_area_list_c>();
    }
    uint32_t& set_repeat_period()
    {
      set(types::repeat_period);
      return c.get<uint32_t>();
    }
    uint32_t& set_nof_broadcasts_requested()
    {
      set(types::nof_broadcasts_requested);
      return c.get<uint32_t>();
    }
    fixed_octstring<2, true>& set_warning_type()
    {
      set(types::warning_type);
      return c.get<fixed_octstring<2, true> >();
    }
    fixed_octstring<50, true>& set_warning_security_info()
    {
      set(types::warning_security_info);
      return c.get<fixed_octstring<50, true> >();
    }
    fixed_bitstring<8, false, true>& set_data_coding_scheme()
    {
      set(types::data_coding_scheme);
      return c.get<fixed_bitstring<8, false, true> >();
    }
    unbounded_octstring<true>& set_warning_msg_contents()
    {
      set(types::warning_msg_contents);
      return c.get<unbounded_octstring<true> >();
    }
    concurrent_warning_msg_ind_e& set_concurrent_warning_msg_ind()
    {
      set(types::concurrent_warning_msg_ind);
      return c.get<concurrent_warning_msg_ind_e>();
    }
    unbounded_octstring<true>& set_warning_area_coordinates()
    {
      set(types::warning_area_coordinates);
      return c.get<unbounded_octstring<true> >();
    }

  private:
    types type_;
    choice_buffer_t<fixed_bitstring<16, false, true>,
                    fixed_octstring<2, true>,
                    fixed_octstring<50, true>,
                    unbounded_octstring<true>,
                    warning_area_list_c>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// WriteReplaceWarningResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct write_replace_warning_resp_ies_o {
  // Value ::= CLASS OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { msg_id, serial_num, broadcast_completed_area_list, crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>& msg_id()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    fixed_bitstring<16, false, true>& serial_num()
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    broadcast_completed_area_list_c& broadcast_completed_area_list()
    {
      assert_choice_type("BroadcastCompletedAreaList", type_.to_string(), "Value");
      return c.get<broadcast_completed_area_list_c>();
    }
    crit_diagnostics_s& crit_diagnostics()
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    const fixed_bitstring<16, false, true>& msg_id() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    const fixed_bitstring<16, false, true>& serial_num() const
    {
      assert_choice_type("BIT STRING", type_.to_string(), "Value");
      return c.get<fixed_bitstring<16, false, true> >();
    }
    const broadcast_completed_area_list_c& broadcast_completed_area_list() const
    {
      assert_choice_type("BroadcastCompletedAreaList", type_.to_string(), "Value");
      return c.get<broadcast_completed_area_list_c>();
    }
    const crit_diagnostics_s& crit_diagnostics() const
    {
      assert_choice_type("CriticalityDiagnostics", type_.to_string(), "Value");
      return c.get<crit_diagnostics_s>();
    }
    fixed_bitstring<16, false, true>& set_msg_id()
    {
      set(types::msg_id);
      return c.get<fixed_bitstring<16, false, true> >();
    }
    fixed_bitstring<16, false, true>& set_serial_num()
    {
      set(types::serial_num);
      return c.get<fixed_bitstring<16, false, true> >();
    }
    broadcast_completed_area_list_c& set_broadcast_completed_area_list()
    {
      set(types::broadcast_completed_area_list);
      return c.get<broadcast_completed_area_list_c>();
    }
    crit_diagnostics_s& set_crit_diagnostics()
    {
      set(types::crit_diagnostics);
      return c.get<crit_diagnostics_s>();
    }

  private:
    types                                                                                                   type_;
    choice_buffer_t<broadcast_completed_area_list_c, crit_diagnostics_s, fixed_bitstring<16, false, true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LocationReport ::= SEQUENCE
struct location_report_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          ue_presence_in_area_of_interest_list_present = false;
    bool                                                          ps_cell_info_present                         = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<user_location_info_c>                              user_location_info;
    ie_field_s<dyn_seq_of<ue_presence_in_area_of_interest_item_s, 1, 64, true> > ue_presence_in_area_of_interest_list;
    ie_field_s<location_report_request_type_s>                                   location_report_request_type;
    ie_field_s<ngran_cgi_c>                                                      ps_cell_info;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationReportingControl ::= SEQUENCE
struct location_report_ctrl_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<location_report_request_type_s>                    location_report_request_type;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationReportingFailureIndication ::= SEQUENCE
struct location_report_fail_ind_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<cause_c>                                           cause;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NASNonDeliveryIndication ::= SEQUENCE
struct nas_non_delivery_ind_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<unbounded_octstring<true> >                        nas_pdu;
    ie_field_s<cause_c>                                           cause;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGReset ::= SEQUENCE
struct ng_reset_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<cause_c>      cause;
    ie_field_s<reset_type_c> reset_type;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGResetAcknowledge ::= SEQUENCE
struct ng_reset_ack_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool ue_associated_lc_ng_conn_list_present = false;
    bool crit_diagnostics_present              = false;
    ie_field_s<dyn_seq_of<ue_associated_lc_ng_conn_item_s, 1, 65536, true> > ue_associated_lc_ng_conn_list;
    ie_field_s<crit_diagnostics_s>                                           crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGSetupFailure ::= SEQUENCE
struct ng_setup_fail_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                           time_to_wait_present     = false;
    bool                           crit_diagnostics_present = false;
    ie_field_s<cause_c>            cause;
    ie_field_s<time_to_wait_e>     time_to_wait;
    ie_field_s<crit_diagnostics_s> crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGSetupRequest ::= SEQUENCE
struct ng_setup_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                       ran_node_name_present     = false;
    bool                                                       ue_retention_info_present = false;
    ie_field_s<global_ran_node_id_c>                           global_ran_node_id;
    ie_field_s<printable_string<1, 150, true, true> >          ran_node_name;
    ie_field_s<dyn_seq_of<supported_ta_item_s, 1, 256, true> > supported_ta_list;
    ie_field_s<paging_drx_e>                                   default_paging_drx;
    ie_field_s<ue_retention_info_e>                            ue_retention_info;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGSetupResponse ::= SEQUENCE
struct ng_setup_resp_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                       crit_diagnostics_present  = false;
    bool                                                       ue_retention_info_present = false;
    ie_field_s<printable_string<1, 150, true, true> >          amf_name;
    ie_field_s<dyn_seq_of<served_guami_item_s, 1, 256, true> > served_guami_list;
    ie_field_s<integer<uint16_t, 0, 255, false, true> >        relative_amf_capacity;
    ie_field_s<dyn_seq_of<plmn_support_item_s, 1, 12, true> >  plmn_support_list;
    ie_field_s<crit_diagnostics_s>                             crit_diagnostics;
    ie_field_s<ue_retention_info_e>                            ue_retention_info;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OverloadStart ::= SEQUENCE
struct overload_start_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                                amf_overload_resp_present              = false;
    bool                                                                amf_traffic_load_reduction_ind_present = false;
    bool                                                                overload_start_nssai_list_present      = false;
    ie_field_s<overload_resp_c>                                         amf_overload_resp;
    ie_field_s<integer<uint8_t, 1, 99, false, true> >                   amf_traffic_load_reduction_ind;
    ie_field_s<dyn_seq_of<overload_start_nssai_item_s, 1, 1024, true> > overload_start_nssai_list;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct protocol_ie_container_empty_l {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};
// OverloadStop ::= SEQUENCE
struct overload_stop_s {
  typedef protocol_ie_container_empty_l protocol_ies_l_;

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyConfirm ::= SEQUENCE
struct pdu_session_res_modify_confirm_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool pdu_session_res_failed_to_modify_list_mod_cfm_present = false;
    bool crit_diagnostics_present                              = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> >                amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >                   ran_ue_ngap_id;
    ie_field_s<dyn_seq_of<pdu_session_res_modify_item_mod_cfm_s, 1, 256, true> > pdu_session_res_modify_list_mod_cfm;
    ie_field_s<dyn_seq_of<pdu_session_res_failed_to_modify_item_mod_cfm_s, 1, 256, true> >
                                   pdu_session_res_failed_to_modify_list_mod_cfm;
    ie_field_s<crit_diagnostics_s> crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyIndication ::= SEQUENCE
struct pdu_session_res_modify_ind_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> >                amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >                   ran_ue_ngap_id;
    ie_field_s<dyn_seq_of<pdu_session_res_modify_item_mod_ind_s, 1, 256, true> > pdu_session_res_modify_list_mod_ind;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyRequest ::= SEQUENCE
struct pdu_session_res_modify_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                                         ran_paging_prio_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> >                amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >                   ran_ue_ngap_id;
    ie_field_s<integer<uint16_t, 1, 256, false, true> >                          ran_paging_prio;
    ie_field_s<dyn_seq_of<pdu_session_res_modify_item_mod_req_s, 1, 256, true> > pdu_session_res_modify_list_mod_req;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyResponse ::= SEQUENCE
struct pdu_session_res_modify_resp_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool pdu_session_res_modify_list_mod_res_present           = false;
    bool pdu_session_res_failed_to_modify_list_mod_res_present = false;
    bool user_location_info_present                            = false;
    bool crit_diagnostics_present                              = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> >                amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >                   ran_ue_ngap_id;
    ie_field_s<dyn_seq_of<pdu_session_res_modify_item_mod_res_s, 1, 256, true> > pdu_session_res_modify_list_mod_res;
    ie_field_s<dyn_seq_of<pdu_session_res_failed_to_modify_item_mod_res_s, 1, 256, true> >
                                     pdu_session_res_failed_to_modify_list_mod_res;
    ie_field_s<user_location_info_c> user_location_info;
    ie_field_s<crit_diagnostics_s>   crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceNotify ::= SEQUENCE
struct pdu_session_res_notify_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          pdu_session_res_notify_list_present       = false;
    bool                                                          pdu_session_res_released_list_not_present = false;
    bool                                                          user_location_info_present                = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<dyn_seq_of<pdu_session_res_notify_item_s, 1, 256, true> >       pdu_session_res_notify_list;
    ie_field_s<dyn_seq_of<pdu_session_res_released_item_not_s, 1, 256, true> > pdu_session_res_released_list_not;
    ie_field_s<user_location_info_c>                                           user_location_info;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleaseCommand ::= SEQUENCE
struct pdu_session_res_release_cmd_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          ran_paging_prio_present = false;
    bool                                                          nas_pdu_present         = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<integer<uint16_t, 1, 256, false, true> >           ran_paging_prio;
    ie_field_s<unbounded_octstring<true> >                        nas_pdu;
    ie_field_s<dyn_seq_of<pdu_session_res_to_release_item_rel_cmd_s, 1, 256, true> >
        pdu_session_res_to_release_list_rel_cmd;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleaseResponse ::= SEQUENCE
struct pdu_session_res_release_resp_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          user_location_info_present = false;
    bool                                                          crit_diagnostics_present   = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<dyn_seq_of<pdu_session_res_released_item_rel_res_s, 1, 256, true> >
                                     pdu_session_res_released_list_rel_res;
    ie_field_s<user_location_info_c> user_location_info;
    ie_field_s<crit_diagnostics_s>   crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupRequest ::= SEQUENCE
struct pdu_session_res_setup_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          ran_paging_prio_present               = false;
    bool                                                          nas_pdu_present                       = false;
    bool                                                          ue_aggregate_maximum_bit_rate_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<integer<uint16_t, 1, 256, false, true> >           ran_paging_prio;
    ie_field_s<unbounded_octstring<true> >                        nas_pdu;
    ie_field_s<dyn_seq_of<pdu_session_res_setup_item_su_req_s, 1, 256, true> > pdu_session_res_setup_list_su_req;
    ie_field_s<ue_aggregate_maximum_bit_rate_s>                                ue_aggregate_maximum_bit_rate;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupResponse ::= SEQUENCE
struct pdu_session_res_setup_resp_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool pdu_session_res_setup_list_su_res_present           = false;
    bool pdu_session_res_failed_to_setup_list_su_res_present = false;
    bool crit_diagnostics_present                            = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> >              amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >                 ran_ue_ngap_id;
    ie_field_s<dyn_seq_of<pdu_session_res_setup_item_su_res_s, 1, 256, true> > pdu_session_res_setup_list_su_res;
    ie_field_s<dyn_seq_of<pdu_session_res_failed_to_setup_item_su_res_s, 1, 256, true> >
                                   pdu_session_res_failed_to_setup_list_su_res;
    ie_field_s<crit_diagnostics_s> crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSCancelRequest ::= SEQUENCE
struct pws_cancel_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                          warning_area_list_present       = false;
    bool                                          cancel_all_warning_msgs_present = false;
    ie_field_s<fixed_bitstring<16, false, true> > msg_id;
    ie_field_s<fixed_bitstring<16, false, true> > serial_num;
    ie_field_s<warning_area_list_c>               warning_area_list;
    ie_field_s<cancel_all_warning_msgs_e>         cancel_all_warning_msgs;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSCancelResponse ::= SEQUENCE
struct pws_cancel_resp_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                          broadcast_cancelled_area_list_present = false;
    bool                                          crit_diagnostics_present              = false;
    ie_field_s<fixed_bitstring<16, false, true> > msg_id;
    ie_field_s<fixed_bitstring<16, false, true> > serial_num;
    ie_field_s<broadcast_cancelled_area_list_c>   broadcast_cancelled_area_list;
    ie_field_s<crit_diagnostics_s>                crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSFailureIndication ::= SEQUENCE
struct pws_fail_ind_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<pws_failed_cell_id_list_c> pws_failed_cell_id_list;
    ie_field_s<global_ran_node_id_c>      global_ran_node_id;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSRestartIndication ::= SEQUENCE
struct pws_restart_ind_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                            emergency_area_id_list_for_restart_present = false;
    ie_field_s<cell_id_list_for_restart_c>                          cell_id_list_for_restart;
    ie_field_s<global_ran_node_id_c>                                global_ran_node_id;
    ie_field_s<dyn_seq_of<tai_s, 1, 2048, true> >                   tai_list_for_restart;
    ie_field_s<dyn_seq_of<fixed_octstring<3, true>, 1, 256, true> > emergency_area_id_list_for_restart;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Paging ::= SEQUENCE
struct paging_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                             paging_drx_present              = false;
    bool                                                             paging_prio_present             = false;
    bool                                                             ue_radio_cap_for_paging_present = false;
    bool                                                             paging_origin_present           = false;
    bool                                                             assist_data_for_paging_present  = false;
    ie_field_s<ue_paging_id_c>                                       ue_paging_id;
    ie_field_s<paging_drx_e>                                         paging_drx;
    ie_field_s<dyn_seq_of<tai_list_for_paging_item_s, 1, 16, true> > tai_list_for_paging;
    ie_field_s<paging_prio_e>                                        paging_prio;
    ie_field_s<ue_radio_cap_for_paging_s>                            ue_radio_cap_for_paging;
    ie_field_s<paging_origin_e>                                      paging_origin;
    ie_field_s<assist_data_for_paging_s>                             assist_data_for_paging;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequest ::= SEQUENCE
struct path_switch_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool pdu_session_res_failed_to_setup_list_ps_req_present = false;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > source_amf_ue_ngap_id;
    ie_field_s<user_location_info_c>                              user_location_info;
    ie_field_s<ue_security_cap_s>                                 ue_security_cap;
    ie_field_s<dyn_seq_of<pdu_session_res_to_be_switched_dl_item_s, 1, 256, true> >
        pdu_session_res_to_be_switched_dl_list;
    ie_field_s<dyn_seq_of<pdu_session_res_failed_to_setup_item_ps_req_s, 1, 256, true> >
        pdu_session_res_failed_to_setup_list_ps_req;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestAcknowledge ::= SEQUENCE
struct path_switch_request_ack_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool ue_security_cap_present                        = false;
    bool new_security_context_ind_present               = false;
    bool pdu_session_res_released_list_ps_ack_present   = false;
    bool core_network_assist_info_present               = false;
    bool rrc_inactive_transition_report_request_present = false;
    bool crit_diagnostics_present                       = false;
    bool redirection_voice_fallback_present             = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> >                 amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >                    ran_ue_ngap_id;
    ie_field_s<ue_security_cap_s>                                                 ue_security_cap;
    ie_field_s<security_context_s>                                                security_context;
    ie_field_s<new_security_context_ind_e>                                        new_security_context_ind;
    ie_field_s<dyn_seq_of<pdu_session_res_switched_item_s, 1, 256, true> >        pdu_session_res_switched_list;
    ie_field_s<dyn_seq_of<pdu_session_res_released_item_ps_ack_s, 1, 256, true> > pdu_session_res_released_list_ps_ack;
    ie_field_s<dyn_seq_of<allowed_nssai_item_s, 1, 8, true> >                     allowed_nssai;
    ie_field_s<core_network_assist_info_s>                                        core_network_assist_info;
    ie_field_s<rrc_inactive_transition_report_request_e> rrc_inactive_transition_report_request;
    ie_field_s<crit_diagnostics_s>                       crit_diagnostics;
    ie_field_s<redirection_voice_fallback_e>             redirection_voice_fallback;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestFailure ::= SEQUENCE
struct path_switch_request_fail_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          crit_diagnostics_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<dyn_seq_of<pdu_session_res_released_item_ps_fail_s, 1, 256, true> >
                                   pdu_session_res_released_list_ps_fail;
    ie_field_s<crit_diagnostics_s> crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

template <class valueT_>
struct private_ie_container_item_s {
  private_ie_id_c id;
  crit_e          crit;
  valueT_         value;

  // sequence methods
  private_ie_container_item_s(private_ie_id_c id_, crit_e crit_);
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct private_ie_container_empty_l {
  template <class valueT_>
  using ie_field_s = private_ie_container_item_s<valueT_>;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};
// PrivateMessage ::= SEQUENCE
struct private_msg_s {
  typedef private_ie_container_empty_l private_ies_l_;

  // member variables
  bool           ext = false;
  private_ies_l_ private_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANConfigurationUpdate ::= SEQUENCE
struct ran_cfg_upd_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                       ran_node_name_present      = false;
    bool                                                       supported_ta_list_present  = false;
    bool                                                       default_paging_drx_present = false;
    bool                                                       global_ran_node_id_present = false;
    ie_field_s<printable_string<1, 150, true, true> >          ran_node_name;
    ie_field_s<dyn_seq_of<supported_ta_item_s, 1, 256, true> > supported_ta_list;
    ie_field_s<paging_drx_e>                                   default_paging_drx;
    ie_field_s<global_ran_node_id_c>                           global_ran_node_id;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANConfigurationUpdateAcknowledge ::= SEQUENCE
struct ran_cfg_upd_ack_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                           crit_diagnostics_present = false;
    ie_field_s<crit_diagnostics_s> crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANConfigurationUpdateFailure ::= SEQUENCE
struct ran_cfg_upd_fail_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                           time_to_wait_present     = false;
    bool                           crit_diagnostics_present = false;
    ie_field_s<cause_c>            cause;
    ie_field_s<time_to_wait_e>     time_to_wait;
    ie_field_s<crit_diagnostics_s> crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCInactiveTransitionReport ::= SEQUENCE
struct rrc_inactive_transition_report_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<rrc_state_e>                                       rrc_state;
    ie_field_s<user_location_info_c>                              user_location_info;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RerouteNASRequest ::= SEQUENCE
struct reroute_nas_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          amf_ue_ngap_id_present = false;
    bool                                                          allowed_nssai_present  = false;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<unbounded_octstring<true> >                        ngap_msg;
    ie_field_s<fixed_bitstring<10, false, true> >                 amf_set_id;
    ie_field_s<dyn_seq_of<allowed_nssai_item_s, 1, 8, true> >     allowed_nssai;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecondaryRATDataUsageReport ::= SEQUENCE
struct secondary_rat_data_usage_report_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          ho_flag_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<dyn_seq_of<pdu_session_res_secondary_ratusage_item_s, 1, 256, true> >
                          pdu_session_res_secondary_ratusage_list;
    ie_field_s<ho_flag_e> ho_flag;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceFailureIndication ::= SEQUENCE
struct trace_fail_ind_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<fixed_octstring<8, true> >                         ngran_trace_id;
    ie_field_s<cause_c>                                           cause;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceStart ::= SEQUENCE
struct trace_start_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<trace_activation_s>                                trace_activation;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationFailure ::= SEQUENCE
struct ue_context_mod_fail_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          crit_diagnostics_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<cause_c>                                           cause;
    ie_field_s<crit_diagnostics_s>                                crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationRequest ::= SEQUENCE
struct ue_context_mod_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool ran_paging_prio_present                        = false;
    bool security_key_present                           = false;
    bool idx_to_rfsp_present                            = false;
    bool ue_aggregate_maximum_bit_rate_present          = false;
    bool ue_security_cap_present                        = false;
    bool core_network_assist_info_present               = false;
    bool emergency_fallback_ind_present                 = false;
    bool new_amf_ue_ngap_id_present                     = false;
    bool rrc_inactive_transition_report_request_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<integer<uint16_t, 1, 256, false, true> >           ran_paging_prio;
    ie_field_s<fixed_bitstring<256, false, true> >                security_key;
    ie_field_s<integer<uint16_t, 1, 256, false, true> >           idx_to_rfsp;
    ie_field_s<ue_aggregate_maximum_bit_rate_s>                   ue_aggregate_maximum_bit_rate;
    ie_field_s<ue_security_cap_s>                                 ue_security_cap;
    ie_field_s<core_network_assist_info_s>                        core_network_assist_info;
    ie_field_s<emergency_fallback_ind_s>                          emergency_fallback_ind;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > new_amf_ue_ngap_id;
    ie_field_s<rrc_inactive_transition_report_request_e>          rrc_inactive_transition_report_request;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationResponse ::= SEQUENCE
struct ue_context_mod_resp_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          rrc_state_present          = false;
    bool                                                          user_location_info_present = false;
    bool                                                          crit_diagnostics_present   = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<rrc_state_e>                                       rrc_state;
    ie_field_s<user_location_info_c>                              user_location_info;
    ie_field_s<crit_diagnostics_s>                                crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseCommand ::= SEQUENCE
struct ue_context_release_cmd_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<ue_ngap_ids_c> ue_ngap_ids;
    ie_field_s<cause_c>       cause;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseComplete ::= SEQUENCE
struct ue_context_release_complete_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool user_location_info_present                                 = false;
    bool info_on_recommended_cells_and_ran_nodes_for_paging_present = false;
    bool pdu_session_res_list_cxt_rel_cpl_present                   = false;
    bool crit_diagnostics_present                                   = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> >    amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >       ran_ue_ngap_id;
    ie_field_s<user_location_info_c>                                 user_location_info;
    ie_field_s<info_on_recommended_cells_and_ran_nodes_for_paging_s> info_on_recommended_cells_and_ran_nodes_for_paging;
    ie_field_s<dyn_seq_of<pdu_session_res_item_cxt_rel_cpl_s, 1, 256, true> > pdu_session_res_list_cxt_rel_cpl;
    ie_field_s<crit_diagnostics_s>                                            crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseRequest ::= SEQUENCE
struct ue_context_release_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          pdu_session_res_list_cxt_rel_req_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<dyn_seq_of<pdu_session_res_item_cxt_rel_req_s, 1, 256, true> > pdu_session_res_list_cxt_rel_req;
    ie_field_s<cause_c>                                                       cause;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioCapabilityCheckRequest ::= SEQUENCE
struct ue_radio_cap_check_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          ue_radio_cap_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<unbounded_octstring<true> >                        ue_radio_cap;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioCapabilityCheckResponse ::= SEQUENCE
struct ue_radio_cap_check_resp_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          crit_diagnostics_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<ims_voice_support_ind_e>                           ims_voice_support_ind;
    ie_field_s<crit_diagnostics_s>                                crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioCapabilityInfoIndication ::= SEQUENCE
struct ue_radio_cap_info_ind_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                          ue_radio_cap_for_paging_present = false;
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<unbounded_octstring<true> >                        ue_radio_cap;
    ie_field_s<ue_radio_cap_for_paging_s>                         ue_radio_cap_for_paging;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UETNLABindingReleaseRequest ::= SEQUENCE
struct uetnla_binding_release_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkNASTransport ::= SEQUENCE
struct ul_nas_transport_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<unbounded_octstring<true> >                        nas_pdu;
    ie_field_s<user_location_info_c>                              user_location_info;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkNonUEAssociatedNRPPaTransport ::= SEQUENCE
struct ul_non_ueassociated_nrp_pa_transport_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<unbounded_octstring<true> > routing_id;
    ie_field_s<unbounded_octstring<true> > nrp_pa_pdu;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkRANConfigurationTransfer ::= SEQUENCE
struct ul_ran_cfg_transfer_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                   son_cfg_transfer_ul_present      = false;
    bool                                   endc_son_cfg_transfer_ul_present = false;
    ie_field_s<son_cfg_transfer_s>         son_cfg_transfer_ul;
    ie_field_s<unbounded_octstring<true> > endc_son_cfg_transfer_ul;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkRANStatusTransfer ::= SEQUENCE
struct ul_ran_status_transfer_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<ran_status_transfer_transparent_container_s>       ran_status_transfer_transparent_container;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkUEAssociatedNRPPaTransport ::= SEQUENCE
struct ul_ueassociated_nrp_pa_transport_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    ie_field_s<integer<uint64_t, 0, 1099511627775, false, true> > amf_ue_ngap_id;
    ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >    ran_ue_ngap_id;
    ie_field_s<unbounded_octstring<true> >                        routing_id;
    ie_field_s<unbounded_octstring<true> >                        nrp_pa_pdu;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WriteReplaceWarningRequest ::= SEQUENCE
struct write_replace_warning_request_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                                   warning_area_list_present          = false;
    bool                                                   warning_type_present               = false;
    bool                                                   warning_security_info_present      = false;
    bool                                                   data_coding_scheme_present         = false;
    bool                                                   warning_msg_contents_present       = false;
    bool                                                   concurrent_warning_msg_ind_present = false;
    bool                                                   warning_area_coordinates_present   = false;
    ie_field_s<fixed_bitstring<16, false, true> >          msg_id;
    ie_field_s<fixed_bitstring<16, false, true> >          serial_num;
    ie_field_s<warning_area_list_c>                        warning_area_list;
    ie_field_s<integer<uint32_t, 0, 131071, false, true> > repeat_period;
    ie_field_s<integer<uint32_t, 0, 65535, false, true> >  nof_broadcasts_requested;
    ie_field_s<fixed_octstring<2, true> >                  warning_type;
    ie_field_s<fixed_octstring<50, true> >                 warning_security_info;
    ie_field_s<fixed_bitstring<8, false, true> >           data_coding_scheme;
    ie_field_s<unbounded_octstring<true> >                 warning_msg_contents;
    ie_field_s<concurrent_warning_msg_ind_e>               concurrent_warning_msg_ind;
    ie_field_s<unbounded_octstring<true> >                 warning_area_coordinates;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WriteReplaceWarningResponse ::= SEQUENCE
struct write_replace_warning_resp_s {
  struct protocol_ies_l_ {
    template <class valueT_>
    using ie_field_s = protocol_ie_container_item_s<valueT_>;

    // member variables
    bool                                          broadcast_completed_area_list_present = false;
    bool                                          crit_diagnostics_present              = false;
    ie_field_s<fixed_bitstring<16, false, true> > msg_id;
    ie_field_s<fixed_bitstring<16, false, true> > serial_num;
    ie_field_s<broadcast_completed_area_list_c>   broadcast_completed_area_list;
    ie_field_s<crit_diagnostics_s>                crit_diagnostics;

    // sequence methods
    protocol_ies_l_();
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool            ext = false;
  protocol_ies_l_ protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGAP-ELEMENTARY-PROCEDURES-CLASS-1 ::= OBJECT SET OF NGAP-ELEMENTARY-PROCEDURE
struct ngap_elem_procs_class_minus1_o {
  // InitiatingMessage ::= CLASS OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        amf_cfg_upd,
        ho_cancel,
        ho_required,
        ho_request,
        init_context_setup_request,
        ng_reset,
        ng_setup_request,
        path_switch_request,
        pdu_session_res_modify_request,
        pdu_session_res_modify_ind,
        pdu_session_res_release_cmd,
        pdu_session_res_setup_request,
        pws_cancel_request,
        ran_cfg_upd,
        ue_context_mod_request,
        ue_context_release_cmd,
        ue_radio_cap_check_request,
        write_replace_warning_request,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    init_msg_c() = default;
    init_msg_c(const init_msg_c& other);
    init_msg_c& operator=(const init_msg_c& other);
    ~init_msg_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    amf_cfg_upd_s& amf_cfg_upd()
    {
      assert_choice_type("AMFConfigurationUpdate", type_.to_string(), "InitiatingMessage");
      return c.get<amf_cfg_upd_s>();
    }
    ho_cancel_s& ho_cancel()
    {
      assert_choice_type("HandoverCancel", type_.to_string(), "InitiatingMessage");
      return c.get<ho_cancel_s>();
    }
    ho_required_s& ho_required()
    {
      assert_choice_type("HandoverRequired", type_.to_string(), "InitiatingMessage");
      return c.get<ho_required_s>();
    }
    ho_request_s& ho_request()
    {
      assert_choice_type("HandoverRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ho_request_s>();
    }
    init_context_setup_request_s& init_context_setup_request()
    {
      assert_choice_type("InitialContextSetupRequest", type_.to_string(), "InitiatingMessage");
      return c.get<init_context_setup_request_s>();
    }
    ng_reset_s& ng_reset()
    {
      assert_choice_type("NGReset", type_.to_string(), "InitiatingMessage");
      return c.get<ng_reset_s>();
    }
    ng_setup_request_s& ng_setup_request()
    {
      assert_choice_type("NGSetupRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ng_setup_request_s>();
    }
    path_switch_request_s& path_switch_request()
    {
      assert_choice_type("PathSwitchRequest", type_.to_string(), "InitiatingMessage");
      return c.get<path_switch_request_s>();
    }
    pdu_session_res_modify_request_s& pdu_session_res_modify_request()
    {
      assert_choice_type("PDUSessionResourceModifyRequest", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_modify_request_s>();
    }
    pdu_session_res_modify_ind_s& pdu_session_res_modify_ind()
    {
      assert_choice_type("PDUSessionResourceModifyIndication", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_modify_ind_s>();
    }
    pdu_session_res_release_cmd_s& pdu_session_res_release_cmd()
    {
      assert_choice_type("PDUSessionResourceReleaseCommand", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_release_cmd_s>();
    }
    pdu_session_res_setup_request_s& pdu_session_res_setup_request()
    {
      assert_choice_type("PDUSessionResourceSetupRequest", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_setup_request_s>();
    }
    pws_cancel_request_s& pws_cancel_request()
    {
      assert_choice_type("PWSCancelRequest", type_.to_string(), "InitiatingMessage");
      return c.get<pws_cancel_request_s>();
    }
    ran_cfg_upd_s& ran_cfg_upd()
    {
      assert_choice_type("RANConfigurationUpdate", type_.to_string(), "InitiatingMessage");
      return c.get<ran_cfg_upd_s>();
    }
    ue_context_mod_request_s& ue_context_mod_request()
    {
      assert_choice_type("UEContextModificationRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ue_context_mod_request_s>();
    }
    ue_context_release_cmd_s& ue_context_release_cmd()
    {
      assert_choice_type("UEContextReleaseCommand", type_.to_string(), "InitiatingMessage");
      return c.get<ue_context_release_cmd_s>();
    }
    ue_radio_cap_check_request_s& ue_radio_cap_check_request()
    {
      assert_choice_type("UERadioCapabilityCheckRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ue_radio_cap_check_request_s>();
    }
    write_replace_warning_request_s& write_replace_warning_request()
    {
      assert_choice_type("WriteReplaceWarningRequest", type_.to_string(), "InitiatingMessage");
      return c.get<write_replace_warning_request_s>();
    }
    const amf_cfg_upd_s& amf_cfg_upd() const
    {
      assert_choice_type("AMFConfigurationUpdate", type_.to_string(), "InitiatingMessage");
      return c.get<amf_cfg_upd_s>();
    }
    const ho_cancel_s& ho_cancel() const
    {
      assert_choice_type("HandoverCancel", type_.to_string(), "InitiatingMessage");
      return c.get<ho_cancel_s>();
    }
    const ho_required_s& ho_required() const
    {
      assert_choice_type("HandoverRequired", type_.to_string(), "InitiatingMessage");
      return c.get<ho_required_s>();
    }
    const ho_request_s& ho_request() const
    {
      assert_choice_type("HandoverRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ho_request_s>();
    }
    const init_context_setup_request_s& init_context_setup_request() const
    {
      assert_choice_type("InitialContextSetupRequest", type_.to_string(), "InitiatingMessage");
      return c.get<init_context_setup_request_s>();
    }
    const ng_reset_s& ng_reset() const
    {
      assert_choice_type("NGReset", type_.to_string(), "InitiatingMessage");
      return c.get<ng_reset_s>();
    }
    const ng_setup_request_s& ng_setup_request() const
    {
      assert_choice_type("NGSetupRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ng_setup_request_s>();
    }
    const path_switch_request_s& path_switch_request() const
    {
      assert_choice_type("PathSwitchRequest", type_.to_string(), "InitiatingMessage");
      return c.get<path_switch_request_s>();
    }
    const pdu_session_res_modify_request_s& pdu_session_res_modify_request() const
    {
      assert_choice_type("PDUSessionResourceModifyRequest", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_modify_request_s>();
    }
    const pdu_session_res_modify_ind_s& pdu_session_res_modify_ind() const
    {
      assert_choice_type("PDUSessionResourceModifyIndication", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_modify_ind_s>();
    }
    const pdu_session_res_release_cmd_s& pdu_session_res_release_cmd() const
    {
      assert_choice_type("PDUSessionResourceReleaseCommand", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_release_cmd_s>();
    }
    const pdu_session_res_setup_request_s& pdu_session_res_setup_request() const
    {
      assert_choice_type("PDUSessionResourceSetupRequest", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_setup_request_s>();
    }
    const pws_cancel_request_s& pws_cancel_request() const
    {
      assert_choice_type("PWSCancelRequest", type_.to_string(), "InitiatingMessage");
      return c.get<pws_cancel_request_s>();
    }
    const ran_cfg_upd_s& ran_cfg_upd() const
    {
      assert_choice_type("RANConfigurationUpdate", type_.to_string(), "InitiatingMessage");
      return c.get<ran_cfg_upd_s>();
    }
    const ue_context_mod_request_s& ue_context_mod_request() const
    {
      assert_choice_type("UEContextModificationRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ue_context_mod_request_s>();
    }
    const ue_context_release_cmd_s& ue_context_release_cmd() const
    {
      assert_choice_type("UEContextReleaseCommand", type_.to_string(), "InitiatingMessage");
      return c.get<ue_context_release_cmd_s>();
    }
    const ue_radio_cap_check_request_s& ue_radio_cap_check_request() const
    {
      assert_choice_type("UERadioCapabilityCheckRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ue_radio_cap_check_request_s>();
    }
    const write_replace_warning_request_s& write_replace_warning_request() const
    {
      assert_choice_type("WriteReplaceWarningRequest", type_.to_string(), "InitiatingMessage");
      return c.get<write_replace_warning_request_s>();
    }
    amf_cfg_upd_s& set_amf_cfg_upd()
    {
      set(types::amf_cfg_upd);
      return c.get<amf_cfg_upd_s>();
    }
    ho_cancel_s& set_ho_cancel()
    {
      set(types::ho_cancel);
      return c.get<ho_cancel_s>();
    }
    ho_required_s& set_ho_required()
    {
      set(types::ho_required);
      return c.get<ho_required_s>();
    }
    ho_request_s& set_ho_request()
    {
      set(types::ho_request);
      return c.get<ho_request_s>();
    }
    init_context_setup_request_s& set_init_context_setup_request()
    {
      set(types::init_context_setup_request);
      return c.get<init_context_setup_request_s>();
    }
    ng_reset_s& set_ng_reset()
    {
      set(types::ng_reset);
      return c.get<ng_reset_s>();
    }
    ng_setup_request_s& set_ng_setup_request()
    {
      set(types::ng_setup_request);
      return c.get<ng_setup_request_s>();
    }
    path_switch_request_s& set_path_switch_request()
    {
      set(types::path_switch_request);
      return c.get<path_switch_request_s>();
    }
    pdu_session_res_modify_request_s& set_pdu_session_res_modify_request()
    {
      set(types::pdu_session_res_modify_request);
      return c.get<pdu_session_res_modify_request_s>();
    }
    pdu_session_res_modify_ind_s& set_pdu_session_res_modify_ind()
    {
      set(types::pdu_session_res_modify_ind);
      return c.get<pdu_session_res_modify_ind_s>();
    }
    pdu_session_res_release_cmd_s& set_pdu_session_res_release_cmd()
    {
      set(types::pdu_session_res_release_cmd);
      return c.get<pdu_session_res_release_cmd_s>();
    }
    pdu_session_res_setup_request_s& set_pdu_session_res_setup_request()
    {
      set(types::pdu_session_res_setup_request);
      return c.get<pdu_session_res_setup_request_s>();
    }
    pws_cancel_request_s& set_pws_cancel_request()
    {
      set(types::pws_cancel_request);
      return c.get<pws_cancel_request_s>();
    }
    ran_cfg_upd_s& set_ran_cfg_upd()
    {
      set(types::ran_cfg_upd);
      return c.get<ran_cfg_upd_s>();
    }
    ue_context_mod_request_s& set_ue_context_mod_request()
    {
      set(types::ue_context_mod_request);
      return c.get<ue_context_mod_request_s>();
    }
    ue_context_release_cmd_s& set_ue_context_release_cmd()
    {
      set(types::ue_context_release_cmd);
      return c.get<ue_context_release_cmd_s>();
    }
    ue_radio_cap_check_request_s& set_ue_radio_cap_check_request()
    {
      set(types::ue_radio_cap_check_request);
      return c.get<ue_radio_cap_check_request_s>();
    }
    write_replace_warning_request_s& set_write_replace_warning_request()
    {
      set(types::write_replace_warning_request);
      return c.get<write_replace_warning_request_s>();
    }

  private:
    types type_;
    choice_buffer_t<amf_cfg_upd_s,
                    ho_cancel_s,
                    ho_request_s,
                    ho_required_s,
                    init_context_setup_request_s,
                    ng_reset_s,
                    ng_setup_request_s,
                    path_switch_request_s,
                    pdu_session_res_modify_ind_s,
                    pdu_session_res_modify_request_s,
                    pdu_session_res_release_cmd_s,
                    pdu_session_res_setup_request_s,
                    pws_cancel_request_s,
                    ran_cfg_upd_s,
                    ue_context_mod_request_s,
                    ue_context_release_cmd_s,
                    ue_radio_cap_check_request_s,
                    write_replace_warning_request_s>
        c;

    void destroy_();
  };
  // SuccessfulOutcome ::= CLASS OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        amf_cfg_upd,
        ho_cancel,
        ho_required,
        ho_request,
        init_context_setup_request,
        ng_reset,
        ng_setup_request,
        path_switch_request,
        pdu_session_res_modify_request,
        pdu_session_res_modify_ind,
        pdu_session_res_release_cmd,
        pdu_session_res_setup_request,
        pws_cancel_request,
        ran_cfg_upd,
        ue_context_mod_request,
        ue_context_release_cmd,
        ue_radio_cap_check_request,
        write_replace_warning_request,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    successful_outcome_c() = default;
    successful_outcome_c(const successful_outcome_c& other);
    successful_outcome_c& operator=(const successful_outcome_c& other);
    ~successful_outcome_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    amf_cfg_upd_ack_s& amf_cfg_upd()
    {
      assert_choice_type("AMFConfigurationUpdateAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<amf_cfg_upd_ack_s>();
    }
    ho_cancel_ack_s& ho_cancel()
    {
      assert_choice_type("HandoverCancelAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ho_cancel_ack_s>();
    }
    ho_cmd_s& ho_required()
    {
      assert_choice_type("HandoverCommand", type_.to_string(), "SuccessfulOutcome");
      return c.get<ho_cmd_s>();
    }
    ho_request_ack_s& ho_request()
    {
      assert_choice_type("HandoverRequestAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ho_request_ack_s>();
    }
    init_context_setup_resp_s& init_context_setup_request()
    {
      assert_choice_type("InitialContextSetupResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<init_context_setup_resp_s>();
    }
    ng_reset_ack_s& ng_reset()
    {
      assert_choice_type("NGResetAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ng_reset_ack_s>();
    }
    ng_setup_resp_s& ng_setup_request()
    {
      assert_choice_type("NGSetupResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<ng_setup_resp_s>();
    }
    path_switch_request_ack_s& path_switch_request()
    {
      assert_choice_type("PathSwitchRequestAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<path_switch_request_ack_s>();
    }
    pdu_session_res_modify_resp_s& pdu_session_res_modify_request()
    {
      assert_choice_type("PDUSessionResourceModifyResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_modify_resp_s>();
    }
    pdu_session_res_modify_confirm_s& pdu_session_res_modify_ind()
    {
      assert_choice_type("PDUSessionResourceModifyConfirm", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_modify_confirm_s>();
    }
    pdu_session_res_release_resp_s& pdu_session_res_release_cmd()
    {
      assert_choice_type("PDUSessionResourceReleaseResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_release_resp_s>();
    }
    pdu_session_res_setup_resp_s& pdu_session_res_setup_request()
    {
      assert_choice_type("PDUSessionResourceSetupResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_setup_resp_s>();
    }
    pws_cancel_resp_s& pws_cancel_request()
    {
      assert_choice_type("PWSCancelResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pws_cancel_resp_s>();
    }
    ran_cfg_upd_ack_s& ran_cfg_upd()
    {
      assert_choice_type("RANConfigurationUpdateAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ran_cfg_upd_ack_s>();
    }
    ue_context_mod_resp_s& ue_context_mod_request()
    {
      assert_choice_type("UEContextModificationResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<ue_context_mod_resp_s>();
    }
    ue_context_release_complete_s& ue_context_release_cmd()
    {
      assert_choice_type("UEContextReleaseComplete", type_.to_string(), "SuccessfulOutcome");
      return c.get<ue_context_release_complete_s>();
    }
    ue_radio_cap_check_resp_s& ue_radio_cap_check_request()
    {
      assert_choice_type("UERadioCapabilityCheckResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<ue_radio_cap_check_resp_s>();
    }
    write_replace_warning_resp_s& write_replace_warning_request()
    {
      assert_choice_type("WriteReplaceWarningResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<write_replace_warning_resp_s>();
    }
    const amf_cfg_upd_ack_s& amf_cfg_upd() const
    {
      assert_choice_type("AMFConfigurationUpdateAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<amf_cfg_upd_ack_s>();
    }
    const ho_cancel_ack_s& ho_cancel() const
    {
      assert_choice_type("HandoverCancelAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ho_cancel_ack_s>();
    }
    const ho_cmd_s& ho_required() const
    {
      assert_choice_type("HandoverCommand", type_.to_string(), "SuccessfulOutcome");
      return c.get<ho_cmd_s>();
    }
    const ho_request_ack_s& ho_request() const
    {
      assert_choice_type("HandoverRequestAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ho_request_ack_s>();
    }
    const init_context_setup_resp_s& init_context_setup_request() const
    {
      assert_choice_type("InitialContextSetupResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<init_context_setup_resp_s>();
    }
    const ng_reset_ack_s& ng_reset() const
    {
      assert_choice_type("NGResetAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ng_reset_ack_s>();
    }
    const ng_setup_resp_s& ng_setup_request() const
    {
      assert_choice_type("NGSetupResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<ng_setup_resp_s>();
    }
    const path_switch_request_ack_s& path_switch_request() const
    {
      assert_choice_type("PathSwitchRequestAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<path_switch_request_ack_s>();
    }
    const pdu_session_res_modify_resp_s& pdu_session_res_modify_request() const
    {
      assert_choice_type("PDUSessionResourceModifyResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_modify_resp_s>();
    }
    const pdu_session_res_modify_confirm_s& pdu_session_res_modify_ind() const
    {
      assert_choice_type("PDUSessionResourceModifyConfirm", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_modify_confirm_s>();
    }
    const pdu_session_res_release_resp_s& pdu_session_res_release_cmd() const
    {
      assert_choice_type("PDUSessionResourceReleaseResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_release_resp_s>();
    }
    const pdu_session_res_setup_resp_s& pdu_session_res_setup_request() const
    {
      assert_choice_type("PDUSessionResourceSetupResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_setup_resp_s>();
    }
    const pws_cancel_resp_s& pws_cancel_request() const
    {
      assert_choice_type("PWSCancelResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pws_cancel_resp_s>();
    }
    const ran_cfg_upd_ack_s& ran_cfg_upd() const
    {
      assert_choice_type("RANConfigurationUpdateAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ran_cfg_upd_ack_s>();
    }
    const ue_context_mod_resp_s& ue_context_mod_request() const
    {
      assert_choice_type("UEContextModificationResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<ue_context_mod_resp_s>();
    }
    const ue_context_release_complete_s& ue_context_release_cmd() const
    {
      assert_choice_type("UEContextReleaseComplete", type_.to_string(), "SuccessfulOutcome");
      return c.get<ue_context_release_complete_s>();
    }
    const ue_radio_cap_check_resp_s& ue_radio_cap_check_request() const
    {
      assert_choice_type("UERadioCapabilityCheckResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<ue_radio_cap_check_resp_s>();
    }
    const write_replace_warning_resp_s& write_replace_warning_request() const
    {
      assert_choice_type("WriteReplaceWarningResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<write_replace_warning_resp_s>();
    }
    amf_cfg_upd_ack_s& set_amf_cfg_upd()
    {
      set(types::amf_cfg_upd);
      return c.get<amf_cfg_upd_ack_s>();
    }
    ho_cancel_ack_s& set_ho_cancel()
    {
      set(types::ho_cancel);
      return c.get<ho_cancel_ack_s>();
    }
    ho_cmd_s& set_ho_required()
    {
      set(types::ho_required);
      return c.get<ho_cmd_s>();
    }
    ho_request_ack_s& set_ho_request()
    {
      set(types::ho_request);
      return c.get<ho_request_ack_s>();
    }
    init_context_setup_resp_s& set_init_context_setup_request()
    {
      set(types::init_context_setup_request);
      return c.get<init_context_setup_resp_s>();
    }
    ng_reset_ack_s& set_ng_reset()
    {
      set(types::ng_reset);
      return c.get<ng_reset_ack_s>();
    }
    ng_setup_resp_s& set_ng_setup_request()
    {
      set(types::ng_setup_request);
      return c.get<ng_setup_resp_s>();
    }
    path_switch_request_ack_s& set_path_switch_request()
    {
      set(types::path_switch_request);
      return c.get<path_switch_request_ack_s>();
    }
    pdu_session_res_modify_resp_s& set_pdu_session_res_modify_request()
    {
      set(types::pdu_session_res_modify_request);
      return c.get<pdu_session_res_modify_resp_s>();
    }
    pdu_session_res_modify_confirm_s& set_pdu_session_res_modify_ind()
    {
      set(types::pdu_session_res_modify_ind);
      return c.get<pdu_session_res_modify_confirm_s>();
    }
    pdu_session_res_release_resp_s& set_pdu_session_res_release_cmd()
    {
      set(types::pdu_session_res_release_cmd);
      return c.get<pdu_session_res_release_resp_s>();
    }
    pdu_session_res_setup_resp_s& set_pdu_session_res_setup_request()
    {
      set(types::pdu_session_res_setup_request);
      return c.get<pdu_session_res_setup_resp_s>();
    }
    pws_cancel_resp_s& set_pws_cancel_request()
    {
      set(types::pws_cancel_request);
      return c.get<pws_cancel_resp_s>();
    }
    ran_cfg_upd_ack_s& set_ran_cfg_upd()
    {
      set(types::ran_cfg_upd);
      return c.get<ran_cfg_upd_ack_s>();
    }
    ue_context_mod_resp_s& set_ue_context_mod_request()
    {
      set(types::ue_context_mod_request);
      return c.get<ue_context_mod_resp_s>();
    }
    ue_context_release_complete_s& set_ue_context_release_cmd()
    {
      set(types::ue_context_release_cmd);
      return c.get<ue_context_release_complete_s>();
    }
    ue_radio_cap_check_resp_s& set_ue_radio_cap_check_request()
    {
      set(types::ue_radio_cap_check_request);
      return c.get<ue_radio_cap_check_resp_s>();
    }
    write_replace_warning_resp_s& set_write_replace_warning_request()
    {
      set(types::write_replace_warning_request);
      return c.get<write_replace_warning_resp_s>();
    }

  private:
    types type_;
    choice_buffer_t<amf_cfg_upd_ack_s,
                    ho_cancel_ack_s,
                    ho_cmd_s,
                    ho_request_ack_s,
                    init_context_setup_resp_s,
                    ng_reset_ack_s,
                    ng_setup_resp_s,
                    path_switch_request_ack_s,
                    pdu_session_res_modify_confirm_s,
                    pdu_session_res_modify_resp_s,
                    pdu_session_res_release_resp_s,
                    pdu_session_res_setup_resp_s,
                    pws_cancel_resp_s,
                    ran_cfg_upd_ack_s,
                    ue_context_mod_resp_s,
                    ue_context_release_complete_s,
                    ue_radio_cap_check_resp_s,
                    write_replace_warning_resp_s>
        c;

    void destroy_();
  };
  // UnsuccessfulOutcome ::= CLASS OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        amf_cfg_upd,
        ho_cancel,
        ho_required,
        ho_request,
        init_context_setup_request,
        ng_reset,
        ng_setup_request,
        path_switch_request,
        pdu_session_res_modify_request,
        pdu_session_res_modify_ind,
        pdu_session_res_release_cmd,
        pdu_session_res_setup_request,
        pws_cancel_request,
        ran_cfg_upd,
        ue_context_mod_request,
        ue_context_release_cmd,
        ue_radio_cap_check_request,
        write_replace_warning_request,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    unsuccessful_outcome_c() = default;
    unsuccessful_outcome_c(const unsuccessful_outcome_c& other);
    unsuccessful_outcome_c& operator=(const unsuccessful_outcome_c& other);
    ~unsuccessful_outcome_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    amf_cfg_upd_fail_s& amf_cfg_upd()
    {
      assert_choice_type("AMFConfigurationUpdateFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<amf_cfg_upd_fail_s>();
    }
    ho_prep_fail_s& ho_required()
    {
      assert_choice_type("HandoverPreparationFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ho_prep_fail_s>();
    }
    ho_fail_s& ho_request()
    {
      assert_choice_type("HandoverFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ho_fail_s>();
    }
    init_context_setup_fail_s& init_context_setup_request()
    {
      assert_choice_type("InitialContextSetupFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<init_context_setup_fail_s>();
    }
    ng_setup_fail_s& ng_setup_request()
    {
      assert_choice_type("NGSetupFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ng_setup_fail_s>();
    }
    path_switch_request_fail_s& path_switch_request()
    {
      assert_choice_type("PathSwitchRequestFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<path_switch_request_fail_s>();
    }
    ran_cfg_upd_fail_s& ran_cfg_upd()
    {
      assert_choice_type("RANConfigurationUpdateFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ran_cfg_upd_fail_s>();
    }
    ue_context_mod_fail_s& ue_context_mod_request()
    {
      assert_choice_type("UEContextModificationFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ue_context_mod_fail_s>();
    }
    const amf_cfg_upd_fail_s& amf_cfg_upd() const
    {
      assert_choice_type("AMFConfigurationUpdateFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<amf_cfg_upd_fail_s>();
    }
    const ho_prep_fail_s& ho_required() const
    {
      assert_choice_type("HandoverPreparationFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ho_prep_fail_s>();
    }
    const ho_fail_s& ho_request() const
    {
      assert_choice_type("HandoverFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ho_fail_s>();
    }
    const init_context_setup_fail_s& init_context_setup_request() const
    {
      assert_choice_type("InitialContextSetupFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<init_context_setup_fail_s>();
    }
    const ng_setup_fail_s& ng_setup_request() const
    {
      assert_choice_type("NGSetupFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ng_setup_fail_s>();
    }
    const path_switch_request_fail_s& path_switch_request() const
    {
      assert_choice_type("PathSwitchRequestFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<path_switch_request_fail_s>();
    }
    const ran_cfg_upd_fail_s& ran_cfg_upd() const
    {
      assert_choice_type("RANConfigurationUpdateFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ran_cfg_upd_fail_s>();
    }
    const ue_context_mod_fail_s& ue_context_mod_request() const
    {
      assert_choice_type("UEContextModificationFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ue_context_mod_fail_s>();
    }
    amf_cfg_upd_fail_s& set_amf_cfg_upd()
    {
      set(types::amf_cfg_upd);
      return c.get<amf_cfg_upd_fail_s>();
    }
    ho_prep_fail_s& set_ho_required()
    {
      set(types::ho_required);
      return c.get<ho_prep_fail_s>();
    }
    ho_fail_s& set_ho_request()
    {
      set(types::ho_request);
      return c.get<ho_fail_s>();
    }
    init_context_setup_fail_s& set_init_context_setup_request()
    {
      set(types::init_context_setup_request);
      return c.get<init_context_setup_fail_s>();
    }
    ng_setup_fail_s& set_ng_setup_request()
    {
      set(types::ng_setup_request);
      return c.get<ng_setup_fail_s>();
    }
    path_switch_request_fail_s& set_path_switch_request()
    {
      set(types::path_switch_request);
      return c.get<path_switch_request_fail_s>();
    }
    ran_cfg_upd_fail_s& set_ran_cfg_upd()
    {
      set(types::ran_cfg_upd);
      return c.get<ran_cfg_upd_fail_s>();
    }
    ue_context_mod_fail_s& set_ue_context_mod_request()
    {
      set(types::ue_context_mod_request);
      return c.get<ue_context_mod_fail_s>();
    }

  private:
    types type_;
    choice_buffer_t<amf_cfg_upd_fail_s,
                    ho_fail_s,
                    ho_prep_fail_s,
                    init_context_setup_fail_s,
                    ng_setup_fail_s,
                    path_switch_request_fail_s,
                    ran_cfg_upd_fail_s,
                    ue_context_mod_fail_s>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// NGAP-ELEMENTARY-PROCEDURES-CLASS-2 ::= OBJECT SET OF NGAP-ELEMENTARY-PROCEDURE
struct ngap_elem_procs_class_minus2_o {
  // InitiatingMessage ::= CLASS OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        amf_status_ind,
        cell_traffic_trace,
        deactiv_trace,
        dl_nas_transport,
        dl_non_ueassociated_nrp_pa_transport,
        dl_ran_cfg_transfer,
        dl_ran_status_transfer,
        dl_ueassociated_nrp_pa_transport,
        error_ind,
        ho_notify,
        init_ue_msg,
        location_report,
        location_report_ctrl,
        location_report_fail_ind,
        nas_non_delivery_ind,
        overload_start,
        overload_stop,
        paging,
        pdu_session_res_notify,
        private_msg,
        pws_fail_ind,
        pws_restart_ind,
        reroute_nas_request,
        rrc_inactive_transition_report,
        secondary_rat_data_usage_report,
        trace_fail_ind,
        trace_start,
        ue_context_release_request,
        ue_radio_cap_info_ind,
        uetnla_binding_release_request,
        ul_nas_transport,
        ul_non_ueassociated_nrp_pa_transport,
        ul_ran_cfg_transfer,
        ul_ran_status_transfer,
        ul_ueassociated_nrp_pa_transport,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    init_msg_c() = default;
    init_msg_c(const init_msg_c& other);
    init_msg_c& operator=(const init_msg_c& other);
    ~init_msg_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    amf_status_ind_s& amf_status_ind()
    {
      assert_choice_type("AMFStatusIndication", type_.to_string(), "InitiatingMessage");
      return c.get<amf_status_ind_s>();
    }
    cell_traffic_trace_s& cell_traffic_trace()
    {
      assert_choice_type("CellTrafficTrace", type_.to_string(), "InitiatingMessage");
      return c.get<cell_traffic_trace_s>();
    }
    deactiv_trace_s& deactiv_trace()
    {
      assert_choice_type("DeactivateTrace", type_.to_string(), "InitiatingMessage");
      return c.get<deactiv_trace_s>();
    }
    dl_nas_transport_s& dl_nas_transport()
    {
      assert_choice_type("DownlinkNASTransport", type_.to_string(), "InitiatingMessage");
      return c.get<dl_nas_transport_s>();
    }
    dl_non_ueassociated_nrp_pa_transport_s& dl_non_ueassociated_nrp_pa_transport()
    {
      assert_choice_type("DownlinkNonUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<dl_non_ueassociated_nrp_pa_transport_s>();
    }
    dl_ran_cfg_transfer_s& dl_ran_cfg_transfer()
    {
      assert_choice_type("DownlinkRANConfigurationTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<dl_ran_cfg_transfer_s>();
    }
    dl_ran_status_transfer_s& dl_ran_status_transfer()
    {
      assert_choice_type("DownlinkRANStatusTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<dl_ran_status_transfer_s>();
    }
    dl_ueassociated_nrp_pa_transport_s& dl_ueassociated_nrp_pa_transport()
    {
      assert_choice_type("DownlinkUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<dl_ueassociated_nrp_pa_transport_s>();
    }
    error_ind_s& error_ind()
    {
      assert_choice_type("ErrorIndication", type_.to_string(), "InitiatingMessage");
      return c.get<error_ind_s>();
    }
    ho_notify_s& ho_notify()
    {
      assert_choice_type("HandoverNotify", type_.to_string(), "InitiatingMessage");
      return c.get<ho_notify_s>();
    }
    init_ue_msg_s& init_ue_msg()
    {
      assert_choice_type("InitialUEMessage", type_.to_string(), "InitiatingMessage");
      return c.get<init_ue_msg_s>();
    }
    location_report_s& location_report()
    {
      assert_choice_type("LocationReport", type_.to_string(), "InitiatingMessage");
      return c.get<location_report_s>();
    }
    location_report_ctrl_s& location_report_ctrl()
    {
      assert_choice_type("LocationReportingControl", type_.to_string(), "InitiatingMessage");
      return c.get<location_report_ctrl_s>();
    }
    location_report_fail_ind_s& location_report_fail_ind()
    {
      assert_choice_type("LocationReportingFailureIndication", type_.to_string(), "InitiatingMessage");
      return c.get<location_report_fail_ind_s>();
    }
    nas_non_delivery_ind_s& nas_non_delivery_ind()
    {
      assert_choice_type("NASNonDeliveryIndication", type_.to_string(), "InitiatingMessage");
      return c.get<nas_non_delivery_ind_s>();
    }
    overload_start_s& overload_start()
    {
      assert_choice_type("OverloadStart", type_.to_string(), "InitiatingMessage");
      return c.get<overload_start_s>();
    }
    overload_stop_s& overload_stop()
    {
      assert_choice_type("OverloadStop", type_.to_string(), "InitiatingMessage");
      return c.get<overload_stop_s>();
    }
    paging_s& paging()
    {
      assert_choice_type("Paging", type_.to_string(), "InitiatingMessage");
      return c.get<paging_s>();
    }
    pdu_session_res_notify_s& pdu_session_res_notify()
    {
      assert_choice_type("PDUSessionResourceNotify", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_notify_s>();
    }
    private_msg_s& private_msg()
    {
      assert_choice_type("PrivateMessage", type_.to_string(), "InitiatingMessage");
      return c.get<private_msg_s>();
    }
    pws_fail_ind_s& pws_fail_ind()
    {
      assert_choice_type("PWSFailureIndication", type_.to_string(), "InitiatingMessage");
      return c.get<pws_fail_ind_s>();
    }
    pws_restart_ind_s& pws_restart_ind()
    {
      assert_choice_type("PWSRestartIndication", type_.to_string(), "InitiatingMessage");
      return c.get<pws_restart_ind_s>();
    }
    reroute_nas_request_s& reroute_nas_request()
    {
      assert_choice_type("RerouteNASRequest", type_.to_string(), "InitiatingMessage");
      return c.get<reroute_nas_request_s>();
    }
    rrc_inactive_transition_report_s& rrc_inactive_transition_report()
    {
      assert_choice_type("RRCInactiveTransitionReport", type_.to_string(), "InitiatingMessage");
      return c.get<rrc_inactive_transition_report_s>();
    }
    secondary_rat_data_usage_report_s& secondary_rat_data_usage_report()
    {
      assert_choice_type("SecondaryRATDataUsageReport", type_.to_string(), "InitiatingMessage");
      return c.get<secondary_rat_data_usage_report_s>();
    }
    trace_fail_ind_s& trace_fail_ind()
    {
      assert_choice_type("TraceFailureIndication", type_.to_string(), "InitiatingMessage");
      return c.get<trace_fail_ind_s>();
    }
    trace_start_s& trace_start()
    {
      assert_choice_type("TraceStart", type_.to_string(), "InitiatingMessage");
      return c.get<trace_start_s>();
    }
    ue_context_release_request_s& ue_context_release_request()
    {
      assert_choice_type("UEContextReleaseRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ue_context_release_request_s>();
    }
    ue_radio_cap_info_ind_s& ue_radio_cap_info_ind()
    {
      assert_choice_type("UERadioCapabilityInfoIndication", type_.to_string(), "InitiatingMessage");
      return c.get<ue_radio_cap_info_ind_s>();
    }
    uetnla_binding_release_request_s& uetnla_binding_release_request()
    {
      assert_choice_type("UETNLABindingReleaseRequest", type_.to_string(), "InitiatingMessage");
      return c.get<uetnla_binding_release_request_s>();
    }
    ul_nas_transport_s& ul_nas_transport()
    {
      assert_choice_type("UplinkNASTransport", type_.to_string(), "InitiatingMessage");
      return c.get<ul_nas_transport_s>();
    }
    ul_non_ueassociated_nrp_pa_transport_s& ul_non_ueassociated_nrp_pa_transport()
    {
      assert_choice_type("UplinkNonUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<ul_non_ueassociated_nrp_pa_transport_s>();
    }
    ul_ran_cfg_transfer_s& ul_ran_cfg_transfer()
    {
      assert_choice_type("UplinkRANConfigurationTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<ul_ran_cfg_transfer_s>();
    }
    ul_ran_status_transfer_s& ul_ran_status_transfer()
    {
      assert_choice_type("UplinkRANStatusTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<ul_ran_status_transfer_s>();
    }
    ul_ueassociated_nrp_pa_transport_s& ul_ueassociated_nrp_pa_transport()
    {
      assert_choice_type("UplinkUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<ul_ueassociated_nrp_pa_transport_s>();
    }
    const amf_status_ind_s& amf_status_ind() const
    {
      assert_choice_type("AMFStatusIndication", type_.to_string(), "InitiatingMessage");
      return c.get<amf_status_ind_s>();
    }
    const cell_traffic_trace_s& cell_traffic_trace() const
    {
      assert_choice_type("CellTrafficTrace", type_.to_string(), "InitiatingMessage");
      return c.get<cell_traffic_trace_s>();
    }
    const deactiv_trace_s& deactiv_trace() const
    {
      assert_choice_type("DeactivateTrace", type_.to_string(), "InitiatingMessage");
      return c.get<deactiv_trace_s>();
    }
    const dl_nas_transport_s& dl_nas_transport() const
    {
      assert_choice_type("DownlinkNASTransport", type_.to_string(), "InitiatingMessage");
      return c.get<dl_nas_transport_s>();
    }
    const dl_non_ueassociated_nrp_pa_transport_s& dl_non_ueassociated_nrp_pa_transport() const
    {
      assert_choice_type("DownlinkNonUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<dl_non_ueassociated_nrp_pa_transport_s>();
    }
    const dl_ran_cfg_transfer_s& dl_ran_cfg_transfer() const
    {
      assert_choice_type("DownlinkRANConfigurationTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<dl_ran_cfg_transfer_s>();
    }
    const dl_ran_status_transfer_s& dl_ran_status_transfer() const
    {
      assert_choice_type("DownlinkRANStatusTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<dl_ran_status_transfer_s>();
    }
    const dl_ueassociated_nrp_pa_transport_s& dl_ueassociated_nrp_pa_transport() const
    {
      assert_choice_type("DownlinkUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<dl_ueassociated_nrp_pa_transport_s>();
    }
    const error_ind_s& error_ind() const
    {
      assert_choice_type("ErrorIndication", type_.to_string(), "InitiatingMessage");
      return c.get<error_ind_s>();
    }
    const ho_notify_s& ho_notify() const
    {
      assert_choice_type("HandoverNotify", type_.to_string(), "InitiatingMessage");
      return c.get<ho_notify_s>();
    }
    const init_ue_msg_s& init_ue_msg() const
    {
      assert_choice_type("InitialUEMessage", type_.to_string(), "InitiatingMessage");
      return c.get<init_ue_msg_s>();
    }
    const location_report_s& location_report() const
    {
      assert_choice_type("LocationReport", type_.to_string(), "InitiatingMessage");
      return c.get<location_report_s>();
    }
    const location_report_ctrl_s& location_report_ctrl() const
    {
      assert_choice_type("LocationReportingControl", type_.to_string(), "InitiatingMessage");
      return c.get<location_report_ctrl_s>();
    }
    const location_report_fail_ind_s& location_report_fail_ind() const
    {
      assert_choice_type("LocationReportingFailureIndication", type_.to_string(), "InitiatingMessage");
      return c.get<location_report_fail_ind_s>();
    }
    const nas_non_delivery_ind_s& nas_non_delivery_ind() const
    {
      assert_choice_type("NASNonDeliveryIndication", type_.to_string(), "InitiatingMessage");
      return c.get<nas_non_delivery_ind_s>();
    }
    const overload_start_s& overload_start() const
    {
      assert_choice_type("OverloadStart", type_.to_string(), "InitiatingMessage");
      return c.get<overload_start_s>();
    }
    const overload_stop_s& overload_stop() const
    {
      assert_choice_type("OverloadStop", type_.to_string(), "InitiatingMessage");
      return c.get<overload_stop_s>();
    }
    const paging_s& paging() const
    {
      assert_choice_type("Paging", type_.to_string(), "InitiatingMessage");
      return c.get<paging_s>();
    }
    const pdu_session_res_notify_s& pdu_session_res_notify() const
    {
      assert_choice_type("PDUSessionResourceNotify", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_notify_s>();
    }
    const private_msg_s& private_msg() const
    {
      assert_choice_type("PrivateMessage", type_.to_string(), "InitiatingMessage");
      return c.get<private_msg_s>();
    }
    const pws_fail_ind_s& pws_fail_ind() const
    {
      assert_choice_type("PWSFailureIndication", type_.to_string(), "InitiatingMessage");
      return c.get<pws_fail_ind_s>();
    }
    const pws_restart_ind_s& pws_restart_ind() const
    {
      assert_choice_type("PWSRestartIndication", type_.to_string(), "InitiatingMessage");
      return c.get<pws_restart_ind_s>();
    }
    const reroute_nas_request_s& reroute_nas_request() const
    {
      assert_choice_type("RerouteNASRequest", type_.to_string(), "InitiatingMessage");
      return c.get<reroute_nas_request_s>();
    }
    const rrc_inactive_transition_report_s& rrc_inactive_transition_report() const
    {
      assert_choice_type("RRCInactiveTransitionReport", type_.to_string(), "InitiatingMessage");
      return c.get<rrc_inactive_transition_report_s>();
    }
    const secondary_rat_data_usage_report_s& secondary_rat_data_usage_report() const
    {
      assert_choice_type("SecondaryRATDataUsageReport", type_.to_string(), "InitiatingMessage");
      return c.get<secondary_rat_data_usage_report_s>();
    }
    const trace_fail_ind_s& trace_fail_ind() const
    {
      assert_choice_type("TraceFailureIndication", type_.to_string(), "InitiatingMessage");
      return c.get<trace_fail_ind_s>();
    }
    const trace_start_s& trace_start() const
    {
      assert_choice_type("TraceStart", type_.to_string(), "InitiatingMessage");
      return c.get<trace_start_s>();
    }
    const ue_context_release_request_s& ue_context_release_request() const
    {
      assert_choice_type("UEContextReleaseRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ue_context_release_request_s>();
    }
    const ue_radio_cap_info_ind_s& ue_radio_cap_info_ind() const
    {
      assert_choice_type("UERadioCapabilityInfoIndication", type_.to_string(), "InitiatingMessage");
      return c.get<ue_radio_cap_info_ind_s>();
    }
    const uetnla_binding_release_request_s& uetnla_binding_release_request() const
    {
      assert_choice_type("UETNLABindingReleaseRequest", type_.to_string(), "InitiatingMessage");
      return c.get<uetnla_binding_release_request_s>();
    }
    const ul_nas_transport_s& ul_nas_transport() const
    {
      assert_choice_type("UplinkNASTransport", type_.to_string(), "InitiatingMessage");
      return c.get<ul_nas_transport_s>();
    }
    const ul_non_ueassociated_nrp_pa_transport_s& ul_non_ueassociated_nrp_pa_transport() const
    {
      assert_choice_type("UplinkNonUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<ul_non_ueassociated_nrp_pa_transport_s>();
    }
    const ul_ran_cfg_transfer_s& ul_ran_cfg_transfer() const
    {
      assert_choice_type("UplinkRANConfigurationTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<ul_ran_cfg_transfer_s>();
    }
    const ul_ran_status_transfer_s& ul_ran_status_transfer() const
    {
      assert_choice_type("UplinkRANStatusTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<ul_ran_status_transfer_s>();
    }
    const ul_ueassociated_nrp_pa_transport_s& ul_ueassociated_nrp_pa_transport() const
    {
      assert_choice_type("UplinkUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<ul_ueassociated_nrp_pa_transport_s>();
    }
    amf_status_ind_s& set_amf_status_ind()
    {
      set(types::amf_status_ind);
      return c.get<amf_status_ind_s>();
    }
    cell_traffic_trace_s& set_cell_traffic_trace()
    {
      set(types::cell_traffic_trace);
      return c.get<cell_traffic_trace_s>();
    }
    deactiv_trace_s& set_deactiv_trace()
    {
      set(types::deactiv_trace);
      return c.get<deactiv_trace_s>();
    }
    dl_nas_transport_s& set_dl_nas_transport()
    {
      set(types::dl_nas_transport);
      return c.get<dl_nas_transport_s>();
    }
    dl_non_ueassociated_nrp_pa_transport_s& set_dl_non_ueassociated_nrp_pa_transport()
    {
      set(types::dl_non_ueassociated_nrp_pa_transport);
      return c.get<dl_non_ueassociated_nrp_pa_transport_s>();
    }
    dl_ran_cfg_transfer_s& set_dl_ran_cfg_transfer()
    {
      set(types::dl_ran_cfg_transfer);
      return c.get<dl_ran_cfg_transfer_s>();
    }
    dl_ran_status_transfer_s& set_dl_ran_status_transfer()
    {
      set(types::dl_ran_status_transfer);
      return c.get<dl_ran_status_transfer_s>();
    }
    dl_ueassociated_nrp_pa_transport_s& set_dl_ueassociated_nrp_pa_transport()
    {
      set(types::dl_ueassociated_nrp_pa_transport);
      return c.get<dl_ueassociated_nrp_pa_transport_s>();
    }
    error_ind_s& set_error_ind()
    {
      set(types::error_ind);
      return c.get<error_ind_s>();
    }
    ho_notify_s& set_ho_notify()
    {
      set(types::ho_notify);
      return c.get<ho_notify_s>();
    }
    init_ue_msg_s& set_init_ue_msg()
    {
      set(types::init_ue_msg);
      return c.get<init_ue_msg_s>();
    }
    location_report_s& set_location_report()
    {
      set(types::location_report);
      return c.get<location_report_s>();
    }
    location_report_ctrl_s& set_location_report_ctrl()
    {
      set(types::location_report_ctrl);
      return c.get<location_report_ctrl_s>();
    }
    location_report_fail_ind_s& set_location_report_fail_ind()
    {
      set(types::location_report_fail_ind);
      return c.get<location_report_fail_ind_s>();
    }
    nas_non_delivery_ind_s& set_nas_non_delivery_ind()
    {
      set(types::nas_non_delivery_ind);
      return c.get<nas_non_delivery_ind_s>();
    }
    overload_start_s& set_overload_start()
    {
      set(types::overload_start);
      return c.get<overload_start_s>();
    }
    overload_stop_s& set_overload_stop()
    {
      set(types::overload_stop);
      return c.get<overload_stop_s>();
    }
    paging_s& set_paging()
    {
      set(types::paging);
      return c.get<paging_s>();
    }
    pdu_session_res_notify_s& set_pdu_session_res_notify()
    {
      set(types::pdu_session_res_notify);
      return c.get<pdu_session_res_notify_s>();
    }
    private_msg_s& set_private_msg()
    {
      set(types::private_msg);
      return c.get<private_msg_s>();
    }
    pws_fail_ind_s& set_pws_fail_ind()
    {
      set(types::pws_fail_ind);
      return c.get<pws_fail_ind_s>();
    }
    pws_restart_ind_s& set_pws_restart_ind()
    {
      set(types::pws_restart_ind);
      return c.get<pws_restart_ind_s>();
    }
    reroute_nas_request_s& set_reroute_nas_request()
    {
      set(types::reroute_nas_request);
      return c.get<reroute_nas_request_s>();
    }
    rrc_inactive_transition_report_s& set_rrc_inactive_transition_report()
    {
      set(types::rrc_inactive_transition_report);
      return c.get<rrc_inactive_transition_report_s>();
    }
    secondary_rat_data_usage_report_s& set_secondary_rat_data_usage_report()
    {
      set(types::secondary_rat_data_usage_report);
      return c.get<secondary_rat_data_usage_report_s>();
    }
    trace_fail_ind_s& set_trace_fail_ind()
    {
      set(types::trace_fail_ind);
      return c.get<trace_fail_ind_s>();
    }
    trace_start_s& set_trace_start()
    {
      set(types::trace_start);
      return c.get<trace_start_s>();
    }
    ue_context_release_request_s& set_ue_context_release_request()
    {
      set(types::ue_context_release_request);
      return c.get<ue_context_release_request_s>();
    }
    ue_radio_cap_info_ind_s& set_ue_radio_cap_info_ind()
    {
      set(types::ue_radio_cap_info_ind);
      return c.get<ue_radio_cap_info_ind_s>();
    }
    uetnla_binding_release_request_s& set_uetnla_binding_release_request()
    {
      set(types::uetnla_binding_release_request);
      return c.get<uetnla_binding_release_request_s>();
    }
    ul_nas_transport_s& set_ul_nas_transport()
    {
      set(types::ul_nas_transport);
      return c.get<ul_nas_transport_s>();
    }
    ul_non_ueassociated_nrp_pa_transport_s& set_ul_non_ueassociated_nrp_pa_transport()
    {
      set(types::ul_non_ueassociated_nrp_pa_transport);
      return c.get<ul_non_ueassociated_nrp_pa_transport_s>();
    }
    ul_ran_cfg_transfer_s& set_ul_ran_cfg_transfer()
    {
      set(types::ul_ran_cfg_transfer);
      return c.get<ul_ran_cfg_transfer_s>();
    }
    ul_ran_status_transfer_s& set_ul_ran_status_transfer()
    {
      set(types::ul_ran_status_transfer);
      return c.get<ul_ran_status_transfer_s>();
    }
    ul_ueassociated_nrp_pa_transport_s& set_ul_ueassociated_nrp_pa_transport()
    {
      set(types::ul_ueassociated_nrp_pa_transport);
      return c.get<ul_ueassociated_nrp_pa_transport_s>();
    }

  private:
    types type_;
    choice_buffer_t<amf_status_ind_s,
                    cell_traffic_trace_s,
                    deactiv_trace_s,
                    dl_nas_transport_s,
                    dl_non_ueassociated_nrp_pa_transport_s,
                    dl_ran_cfg_transfer_s,
                    dl_ran_status_transfer_s,
                    dl_ueassociated_nrp_pa_transport_s,
                    error_ind_s,
                    ho_notify_s,
                    init_ue_msg_s,
                    location_report_ctrl_s,
                    location_report_fail_ind_s,
                    location_report_s,
                    nas_non_delivery_ind_s,
                    overload_start_s,
                    overload_stop_s,
                    paging_s,
                    pdu_session_res_notify_s,
                    private_msg_s,
                    pws_fail_ind_s,
                    pws_restart_ind_s,
                    reroute_nas_request_s,
                    rrc_inactive_transition_report_s,
                    secondary_rat_data_usage_report_s,
                    trace_fail_ind_s,
                    trace_start_s,
                    ue_context_release_request_s,
                    ue_radio_cap_info_ind_s,
                    uetnla_binding_release_request_s,
                    ul_nas_transport_s,
                    ul_non_ueassociated_nrp_pa_transport_s,
                    ul_ran_cfg_transfer_s,
                    ul_ran_status_transfer_s,
                    ul_ueassociated_nrp_pa_transport_s>
        c;

    void destroy_();
  };
  // SuccessfulOutcome ::= CLASS OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        amf_status_ind,
        cell_traffic_trace,
        deactiv_trace,
        dl_nas_transport,
        dl_non_ueassociated_nrp_pa_transport,
        dl_ran_cfg_transfer,
        dl_ran_status_transfer,
        dl_ueassociated_nrp_pa_transport,
        error_ind,
        ho_notify,
        init_ue_msg,
        location_report,
        location_report_ctrl,
        location_report_fail_ind,
        nas_non_delivery_ind,
        overload_start,
        overload_stop,
        paging,
        pdu_session_res_notify,
        private_msg,
        pws_fail_ind,
        pws_restart_ind,
        reroute_nas_request,
        rrc_inactive_transition_report,
        secondary_rat_data_usage_report,
        trace_fail_ind,
        trace_start,
        ue_context_release_request,
        ue_radio_cap_info_ind,
        uetnla_binding_release_request,
        ul_nas_transport,
        ul_non_ueassociated_nrp_pa_transport,
        ul_ran_cfg_transfer,
        ul_ran_status_transfer,
        ul_ueassociated_nrp_pa_transport,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    successful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;

  private:
    types type_;
  };
  // UnsuccessfulOutcome ::= CLASS OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        amf_status_ind,
        cell_traffic_trace,
        deactiv_trace,
        dl_nas_transport,
        dl_non_ueassociated_nrp_pa_transport,
        dl_ran_cfg_transfer,
        dl_ran_status_transfer,
        dl_ueassociated_nrp_pa_transport,
        error_ind,
        ho_notify,
        init_ue_msg,
        location_report,
        location_report_ctrl,
        location_report_fail_ind,
        nas_non_delivery_ind,
        overload_start,
        overload_stop,
        paging,
        pdu_session_res_notify,
        private_msg,
        pws_fail_ind,
        pws_restart_ind,
        reroute_nas_request,
        rrc_inactive_transition_report,
        secondary_rat_data_usage_report,
        trace_fail_ind,
        trace_start,
        ue_context_release_request,
        ue_radio_cap_info_ind,
        uetnla_binding_release_request,
        ul_nas_transport,
        ul_non_ueassociated_nrp_pa_transport,
        ul_ran_cfg_transfer,
        ul_ran_status_transfer,
        ul_ueassociated_nrp_pa_transport,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    unsuccessful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;

  private:
    types type_;
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// NGAP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF NGAP-ELEMENTARY-PROCEDURE
struct ngap_elem_procs_o {
  // InitiatingMessage ::= CLASS OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        amf_cfg_upd,
        ho_cancel,
        ho_required,
        ho_request,
        init_context_setup_request,
        ng_reset,
        ng_setup_request,
        path_switch_request,
        pdu_session_res_modify_request,
        pdu_session_res_modify_ind,
        pdu_session_res_release_cmd,
        pdu_session_res_setup_request,
        pws_cancel_request,
        ran_cfg_upd,
        ue_context_mod_request,
        ue_context_release_cmd,
        ue_radio_cap_check_request,
        write_replace_warning_request,
        amf_status_ind,
        cell_traffic_trace,
        deactiv_trace,
        dl_nas_transport,
        dl_non_ueassociated_nrp_pa_transport,
        dl_ran_cfg_transfer,
        dl_ran_status_transfer,
        dl_ueassociated_nrp_pa_transport,
        error_ind,
        ho_notify,
        init_ue_msg,
        location_report,
        location_report_ctrl,
        location_report_fail_ind,
        nas_non_delivery_ind,
        overload_start,
        overload_stop,
        paging,
        pdu_session_res_notify,
        private_msg,
        pws_fail_ind,
        pws_restart_ind,
        reroute_nas_request,
        rrc_inactive_transition_report,
        secondary_rat_data_usage_report,
        trace_fail_ind,
        trace_start,
        ue_context_release_request,
        ue_radio_cap_info_ind,
        uetnla_binding_release_request,
        ul_nas_transport,
        ul_non_ueassociated_nrp_pa_transport,
        ul_ran_cfg_transfer,
        ul_ran_status_transfer,
        ul_ueassociated_nrp_pa_transport,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    init_msg_c() = default;
    init_msg_c(const init_msg_c& other);
    init_msg_c& operator=(const init_msg_c& other);
    ~init_msg_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    amf_cfg_upd_s& amf_cfg_upd()
    {
      assert_choice_type("AMFConfigurationUpdate", type_.to_string(), "InitiatingMessage");
      return c.get<amf_cfg_upd_s>();
    }
    ho_cancel_s& ho_cancel()
    {
      assert_choice_type("HandoverCancel", type_.to_string(), "InitiatingMessage");
      return c.get<ho_cancel_s>();
    }
    ho_required_s& ho_required()
    {
      assert_choice_type("HandoverRequired", type_.to_string(), "InitiatingMessage");
      return c.get<ho_required_s>();
    }
    ho_request_s& ho_request()
    {
      assert_choice_type("HandoverRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ho_request_s>();
    }
    init_context_setup_request_s& init_context_setup_request()
    {
      assert_choice_type("InitialContextSetupRequest", type_.to_string(), "InitiatingMessage");
      return c.get<init_context_setup_request_s>();
    }
    ng_reset_s& ng_reset()
    {
      assert_choice_type("NGReset", type_.to_string(), "InitiatingMessage");
      return c.get<ng_reset_s>();
    }
    ng_setup_request_s& ng_setup_request()
    {
      assert_choice_type("NGSetupRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ng_setup_request_s>();
    }
    path_switch_request_s& path_switch_request()
    {
      assert_choice_type("PathSwitchRequest", type_.to_string(), "InitiatingMessage");
      return c.get<path_switch_request_s>();
    }
    pdu_session_res_modify_request_s& pdu_session_res_modify_request()
    {
      assert_choice_type("PDUSessionResourceModifyRequest", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_modify_request_s>();
    }
    pdu_session_res_modify_ind_s& pdu_session_res_modify_ind()
    {
      assert_choice_type("PDUSessionResourceModifyIndication", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_modify_ind_s>();
    }
    pdu_session_res_release_cmd_s& pdu_session_res_release_cmd()
    {
      assert_choice_type("PDUSessionResourceReleaseCommand", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_release_cmd_s>();
    }
    pdu_session_res_setup_request_s& pdu_session_res_setup_request()
    {
      assert_choice_type("PDUSessionResourceSetupRequest", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_setup_request_s>();
    }
    pws_cancel_request_s& pws_cancel_request()
    {
      assert_choice_type("PWSCancelRequest", type_.to_string(), "InitiatingMessage");
      return c.get<pws_cancel_request_s>();
    }
    ran_cfg_upd_s& ran_cfg_upd()
    {
      assert_choice_type("RANConfigurationUpdate", type_.to_string(), "InitiatingMessage");
      return c.get<ran_cfg_upd_s>();
    }
    ue_context_mod_request_s& ue_context_mod_request()
    {
      assert_choice_type("UEContextModificationRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ue_context_mod_request_s>();
    }
    ue_context_release_cmd_s& ue_context_release_cmd()
    {
      assert_choice_type("UEContextReleaseCommand", type_.to_string(), "InitiatingMessage");
      return c.get<ue_context_release_cmd_s>();
    }
    ue_radio_cap_check_request_s& ue_radio_cap_check_request()
    {
      assert_choice_type("UERadioCapabilityCheckRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ue_radio_cap_check_request_s>();
    }
    write_replace_warning_request_s& write_replace_warning_request()
    {
      assert_choice_type("WriteReplaceWarningRequest", type_.to_string(), "InitiatingMessage");
      return c.get<write_replace_warning_request_s>();
    }
    amf_status_ind_s& amf_status_ind()
    {
      assert_choice_type("AMFStatusIndication", type_.to_string(), "InitiatingMessage");
      return c.get<amf_status_ind_s>();
    }
    cell_traffic_trace_s& cell_traffic_trace()
    {
      assert_choice_type("CellTrafficTrace", type_.to_string(), "InitiatingMessage");
      return c.get<cell_traffic_trace_s>();
    }
    deactiv_trace_s& deactiv_trace()
    {
      assert_choice_type("DeactivateTrace", type_.to_string(), "InitiatingMessage");
      return c.get<deactiv_trace_s>();
    }
    dl_nas_transport_s& dl_nas_transport()
    {
      assert_choice_type("DownlinkNASTransport", type_.to_string(), "InitiatingMessage");
      return c.get<dl_nas_transport_s>();
    }
    dl_non_ueassociated_nrp_pa_transport_s& dl_non_ueassociated_nrp_pa_transport()
    {
      assert_choice_type("DownlinkNonUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<dl_non_ueassociated_nrp_pa_transport_s>();
    }
    dl_ran_cfg_transfer_s& dl_ran_cfg_transfer()
    {
      assert_choice_type("DownlinkRANConfigurationTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<dl_ran_cfg_transfer_s>();
    }
    dl_ran_status_transfer_s& dl_ran_status_transfer()
    {
      assert_choice_type("DownlinkRANStatusTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<dl_ran_status_transfer_s>();
    }
    dl_ueassociated_nrp_pa_transport_s& dl_ueassociated_nrp_pa_transport()
    {
      assert_choice_type("DownlinkUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<dl_ueassociated_nrp_pa_transport_s>();
    }
    error_ind_s& error_ind()
    {
      assert_choice_type("ErrorIndication", type_.to_string(), "InitiatingMessage");
      return c.get<error_ind_s>();
    }
    ho_notify_s& ho_notify()
    {
      assert_choice_type("HandoverNotify", type_.to_string(), "InitiatingMessage");
      return c.get<ho_notify_s>();
    }
    init_ue_msg_s& init_ue_msg()
    {
      assert_choice_type("InitialUEMessage", type_.to_string(), "InitiatingMessage");
      return c.get<init_ue_msg_s>();
    }
    location_report_s& location_report()
    {
      assert_choice_type("LocationReport", type_.to_string(), "InitiatingMessage");
      return c.get<location_report_s>();
    }
    location_report_ctrl_s& location_report_ctrl()
    {
      assert_choice_type("LocationReportingControl", type_.to_string(), "InitiatingMessage");
      return c.get<location_report_ctrl_s>();
    }
    location_report_fail_ind_s& location_report_fail_ind()
    {
      assert_choice_type("LocationReportingFailureIndication", type_.to_string(), "InitiatingMessage");
      return c.get<location_report_fail_ind_s>();
    }
    nas_non_delivery_ind_s& nas_non_delivery_ind()
    {
      assert_choice_type("NASNonDeliveryIndication", type_.to_string(), "InitiatingMessage");
      return c.get<nas_non_delivery_ind_s>();
    }
    overload_start_s& overload_start()
    {
      assert_choice_type("OverloadStart", type_.to_string(), "InitiatingMessage");
      return c.get<overload_start_s>();
    }
    overload_stop_s& overload_stop()
    {
      assert_choice_type("OverloadStop", type_.to_string(), "InitiatingMessage");
      return c.get<overload_stop_s>();
    }
    paging_s& paging()
    {
      assert_choice_type("Paging", type_.to_string(), "InitiatingMessage");
      return c.get<paging_s>();
    }
    pdu_session_res_notify_s& pdu_session_res_notify()
    {
      assert_choice_type("PDUSessionResourceNotify", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_notify_s>();
    }
    private_msg_s& private_msg()
    {
      assert_choice_type("PrivateMessage", type_.to_string(), "InitiatingMessage");
      return c.get<private_msg_s>();
    }
    pws_fail_ind_s& pws_fail_ind()
    {
      assert_choice_type("PWSFailureIndication", type_.to_string(), "InitiatingMessage");
      return c.get<pws_fail_ind_s>();
    }
    pws_restart_ind_s& pws_restart_ind()
    {
      assert_choice_type("PWSRestartIndication", type_.to_string(), "InitiatingMessage");
      return c.get<pws_restart_ind_s>();
    }
    reroute_nas_request_s& reroute_nas_request()
    {
      assert_choice_type("RerouteNASRequest", type_.to_string(), "InitiatingMessage");
      return c.get<reroute_nas_request_s>();
    }
    rrc_inactive_transition_report_s& rrc_inactive_transition_report()
    {
      assert_choice_type("RRCInactiveTransitionReport", type_.to_string(), "InitiatingMessage");
      return c.get<rrc_inactive_transition_report_s>();
    }
    secondary_rat_data_usage_report_s& secondary_rat_data_usage_report()
    {
      assert_choice_type("SecondaryRATDataUsageReport", type_.to_string(), "InitiatingMessage");
      return c.get<secondary_rat_data_usage_report_s>();
    }
    trace_fail_ind_s& trace_fail_ind()
    {
      assert_choice_type("TraceFailureIndication", type_.to_string(), "InitiatingMessage");
      return c.get<trace_fail_ind_s>();
    }
    trace_start_s& trace_start()
    {
      assert_choice_type("TraceStart", type_.to_string(), "InitiatingMessage");
      return c.get<trace_start_s>();
    }
    ue_context_release_request_s& ue_context_release_request()
    {
      assert_choice_type("UEContextReleaseRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ue_context_release_request_s>();
    }
    ue_radio_cap_info_ind_s& ue_radio_cap_info_ind()
    {
      assert_choice_type("UERadioCapabilityInfoIndication", type_.to_string(), "InitiatingMessage");
      return c.get<ue_radio_cap_info_ind_s>();
    }
    uetnla_binding_release_request_s& uetnla_binding_release_request()
    {
      assert_choice_type("UETNLABindingReleaseRequest", type_.to_string(), "InitiatingMessage");
      return c.get<uetnla_binding_release_request_s>();
    }
    ul_nas_transport_s& ul_nas_transport()
    {
      assert_choice_type("UplinkNASTransport", type_.to_string(), "InitiatingMessage");
      return c.get<ul_nas_transport_s>();
    }
    ul_non_ueassociated_nrp_pa_transport_s& ul_non_ueassociated_nrp_pa_transport()
    {
      assert_choice_type("UplinkNonUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<ul_non_ueassociated_nrp_pa_transport_s>();
    }
    ul_ran_cfg_transfer_s& ul_ran_cfg_transfer()
    {
      assert_choice_type("UplinkRANConfigurationTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<ul_ran_cfg_transfer_s>();
    }
    ul_ran_status_transfer_s& ul_ran_status_transfer()
    {
      assert_choice_type("UplinkRANStatusTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<ul_ran_status_transfer_s>();
    }
    ul_ueassociated_nrp_pa_transport_s& ul_ueassociated_nrp_pa_transport()
    {
      assert_choice_type("UplinkUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<ul_ueassociated_nrp_pa_transport_s>();
    }
    const amf_cfg_upd_s& amf_cfg_upd() const
    {
      assert_choice_type("AMFConfigurationUpdate", type_.to_string(), "InitiatingMessage");
      return c.get<amf_cfg_upd_s>();
    }
    const ho_cancel_s& ho_cancel() const
    {
      assert_choice_type("HandoverCancel", type_.to_string(), "InitiatingMessage");
      return c.get<ho_cancel_s>();
    }
    const ho_required_s& ho_required() const
    {
      assert_choice_type("HandoverRequired", type_.to_string(), "InitiatingMessage");
      return c.get<ho_required_s>();
    }
    const ho_request_s& ho_request() const
    {
      assert_choice_type("HandoverRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ho_request_s>();
    }
    const init_context_setup_request_s& init_context_setup_request() const
    {
      assert_choice_type("InitialContextSetupRequest", type_.to_string(), "InitiatingMessage");
      return c.get<init_context_setup_request_s>();
    }
    const ng_reset_s& ng_reset() const
    {
      assert_choice_type("NGReset", type_.to_string(), "InitiatingMessage");
      return c.get<ng_reset_s>();
    }
    const ng_setup_request_s& ng_setup_request() const
    {
      assert_choice_type("NGSetupRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ng_setup_request_s>();
    }
    const path_switch_request_s& path_switch_request() const
    {
      assert_choice_type("PathSwitchRequest", type_.to_string(), "InitiatingMessage");
      return c.get<path_switch_request_s>();
    }
    const pdu_session_res_modify_request_s& pdu_session_res_modify_request() const
    {
      assert_choice_type("PDUSessionResourceModifyRequest", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_modify_request_s>();
    }
    const pdu_session_res_modify_ind_s& pdu_session_res_modify_ind() const
    {
      assert_choice_type("PDUSessionResourceModifyIndication", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_modify_ind_s>();
    }
    const pdu_session_res_release_cmd_s& pdu_session_res_release_cmd() const
    {
      assert_choice_type("PDUSessionResourceReleaseCommand", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_release_cmd_s>();
    }
    const pdu_session_res_setup_request_s& pdu_session_res_setup_request() const
    {
      assert_choice_type("PDUSessionResourceSetupRequest", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_setup_request_s>();
    }
    const pws_cancel_request_s& pws_cancel_request() const
    {
      assert_choice_type("PWSCancelRequest", type_.to_string(), "InitiatingMessage");
      return c.get<pws_cancel_request_s>();
    }
    const ran_cfg_upd_s& ran_cfg_upd() const
    {
      assert_choice_type("RANConfigurationUpdate", type_.to_string(), "InitiatingMessage");
      return c.get<ran_cfg_upd_s>();
    }
    const ue_context_mod_request_s& ue_context_mod_request() const
    {
      assert_choice_type("UEContextModificationRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ue_context_mod_request_s>();
    }
    const ue_context_release_cmd_s& ue_context_release_cmd() const
    {
      assert_choice_type("UEContextReleaseCommand", type_.to_string(), "InitiatingMessage");
      return c.get<ue_context_release_cmd_s>();
    }
    const ue_radio_cap_check_request_s& ue_radio_cap_check_request() const
    {
      assert_choice_type("UERadioCapabilityCheckRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ue_radio_cap_check_request_s>();
    }
    const write_replace_warning_request_s& write_replace_warning_request() const
    {
      assert_choice_type("WriteReplaceWarningRequest", type_.to_string(), "InitiatingMessage");
      return c.get<write_replace_warning_request_s>();
    }
    const amf_status_ind_s& amf_status_ind() const
    {
      assert_choice_type("AMFStatusIndication", type_.to_string(), "InitiatingMessage");
      return c.get<amf_status_ind_s>();
    }
    const cell_traffic_trace_s& cell_traffic_trace() const
    {
      assert_choice_type("CellTrafficTrace", type_.to_string(), "InitiatingMessage");
      return c.get<cell_traffic_trace_s>();
    }
    const deactiv_trace_s& deactiv_trace() const
    {
      assert_choice_type("DeactivateTrace", type_.to_string(), "InitiatingMessage");
      return c.get<deactiv_trace_s>();
    }
    const dl_nas_transport_s& dl_nas_transport() const
    {
      assert_choice_type("DownlinkNASTransport", type_.to_string(), "InitiatingMessage");
      return c.get<dl_nas_transport_s>();
    }
    const dl_non_ueassociated_nrp_pa_transport_s& dl_non_ueassociated_nrp_pa_transport() const
    {
      assert_choice_type("DownlinkNonUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<dl_non_ueassociated_nrp_pa_transport_s>();
    }
    const dl_ran_cfg_transfer_s& dl_ran_cfg_transfer() const
    {
      assert_choice_type("DownlinkRANConfigurationTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<dl_ran_cfg_transfer_s>();
    }
    const dl_ran_status_transfer_s& dl_ran_status_transfer() const
    {
      assert_choice_type("DownlinkRANStatusTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<dl_ran_status_transfer_s>();
    }
    const dl_ueassociated_nrp_pa_transport_s& dl_ueassociated_nrp_pa_transport() const
    {
      assert_choice_type("DownlinkUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<dl_ueassociated_nrp_pa_transport_s>();
    }
    const error_ind_s& error_ind() const
    {
      assert_choice_type("ErrorIndication", type_.to_string(), "InitiatingMessage");
      return c.get<error_ind_s>();
    }
    const ho_notify_s& ho_notify() const
    {
      assert_choice_type("HandoverNotify", type_.to_string(), "InitiatingMessage");
      return c.get<ho_notify_s>();
    }
    const init_ue_msg_s& init_ue_msg() const
    {
      assert_choice_type("InitialUEMessage", type_.to_string(), "InitiatingMessage");
      return c.get<init_ue_msg_s>();
    }
    const location_report_s& location_report() const
    {
      assert_choice_type("LocationReport", type_.to_string(), "InitiatingMessage");
      return c.get<location_report_s>();
    }
    const location_report_ctrl_s& location_report_ctrl() const
    {
      assert_choice_type("LocationReportingControl", type_.to_string(), "InitiatingMessage");
      return c.get<location_report_ctrl_s>();
    }
    const location_report_fail_ind_s& location_report_fail_ind() const
    {
      assert_choice_type("LocationReportingFailureIndication", type_.to_string(), "InitiatingMessage");
      return c.get<location_report_fail_ind_s>();
    }
    const nas_non_delivery_ind_s& nas_non_delivery_ind() const
    {
      assert_choice_type("NASNonDeliveryIndication", type_.to_string(), "InitiatingMessage");
      return c.get<nas_non_delivery_ind_s>();
    }
    const overload_start_s& overload_start() const
    {
      assert_choice_type("OverloadStart", type_.to_string(), "InitiatingMessage");
      return c.get<overload_start_s>();
    }
    const overload_stop_s& overload_stop() const
    {
      assert_choice_type("OverloadStop", type_.to_string(), "InitiatingMessage");
      return c.get<overload_stop_s>();
    }
    const paging_s& paging() const
    {
      assert_choice_type("Paging", type_.to_string(), "InitiatingMessage");
      return c.get<paging_s>();
    }
    const pdu_session_res_notify_s& pdu_session_res_notify() const
    {
      assert_choice_type("PDUSessionResourceNotify", type_.to_string(), "InitiatingMessage");
      return c.get<pdu_session_res_notify_s>();
    }
    const private_msg_s& private_msg() const
    {
      assert_choice_type("PrivateMessage", type_.to_string(), "InitiatingMessage");
      return c.get<private_msg_s>();
    }
    const pws_fail_ind_s& pws_fail_ind() const
    {
      assert_choice_type("PWSFailureIndication", type_.to_string(), "InitiatingMessage");
      return c.get<pws_fail_ind_s>();
    }
    const pws_restart_ind_s& pws_restart_ind() const
    {
      assert_choice_type("PWSRestartIndication", type_.to_string(), "InitiatingMessage");
      return c.get<pws_restart_ind_s>();
    }
    const reroute_nas_request_s& reroute_nas_request() const
    {
      assert_choice_type("RerouteNASRequest", type_.to_string(), "InitiatingMessage");
      return c.get<reroute_nas_request_s>();
    }
    const rrc_inactive_transition_report_s& rrc_inactive_transition_report() const
    {
      assert_choice_type("RRCInactiveTransitionReport", type_.to_string(), "InitiatingMessage");
      return c.get<rrc_inactive_transition_report_s>();
    }
    const secondary_rat_data_usage_report_s& secondary_rat_data_usage_report() const
    {
      assert_choice_type("SecondaryRATDataUsageReport", type_.to_string(), "InitiatingMessage");
      return c.get<secondary_rat_data_usage_report_s>();
    }
    const trace_fail_ind_s& trace_fail_ind() const
    {
      assert_choice_type("TraceFailureIndication", type_.to_string(), "InitiatingMessage");
      return c.get<trace_fail_ind_s>();
    }
    const trace_start_s& trace_start() const
    {
      assert_choice_type("TraceStart", type_.to_string(), "InitiatingMessage");
      return c.get<trace_start_s>();
    }
    const ue_context_release_request_s& ue_context_release_request() const
    {
      assert_choice_type("UEContextReleaseRequest", type_.to_string(), "InitiatingMessage");
      return c.get<ue_context_release_request_s>();
    }
    const ue_radio_cap_info_ind_s& ue_radio_cap_info_ind() const
    {
      assert_choice_type("UERadioCapabilityInfoIndication", type_.to_string(), "InitiatingMessage");
      return c.get<ue_radio_cap_info_ind_s>();
    }
    const uetnla_binding_release_request_s& uetnla_binding_release_request() const
    {
      assert_choice_type("UETNLABindingReleaseRequest", type_.to_string(), "InitiatingMessage");
      return c.get<uetnla_binding_release_request_s>();
    }
    const ul_nas_transport_s& ul_nas_transport() const
    {
      assert_choice_type("UplinkNASTransport", type_.to_string(), "InitiatingMessage");
      return c.get<ul_nas_transport_s>();
    }
    const ul_non_ueassociated_nrp_pa_transport_s& ul_non_ueassociated_nrp_pa_transport() const
    {
      assert_choice_type("UplinkNonUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<ul_non_ueassociated_nrp_pa_transport_s>();
    }
    const ul_ran_cfg_transfer_s& ul_ran_cfg_transfer() const
    {
      assert_choice_type("UplinkRANConfigurationTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<ul_ran_cfg_transfer_s>();
    }
    const ul_ran_status_transfer_s& ul_ran_status_transfer() const
    {
      assert_choice_type("UplinkRANStatusTransfer", type_.to_string(), "InitiatingMessage");
      return c.get<ul_ran_status_transfer_s>();
    }
    const ul_ueassociated_nrp_pa_transport_s& ul_ueassociated_nrp_pa_transport() const
    {
      assert_choice_type("UplinkUEAssociatedNRPPaTransport", type_.to_string(), "InitiatingMessage");
      return c.get<ul_ueassociated_nrp_pa_transport_s>();
    }
    amf_cfg_upd_s& set_amf_cfg_upd()
    {
      set(types::amf_cfg_upd);
      return c.get<amf_cfg_upd_s>();
    }
    ho_cancel_s& set_ho_cancel()
    {
      set(types::ho_cancel);
      return c.get<ho_cancel_s>();
    }
    ho_required_s& set_ho_required()
    {
      set(types::ho_required);
      return c.get<ho_required_s>();
    }
    ho_request_s& set_ho_request()
    {
      set(types::ho_request);
      return c.get<ho_request_s>();
    }
    init_context_setup_request_s& set_init_context_setup_request()
    {
      set(types::init_context_setup_request);
      return c.get<init_context_setup_request_s>();
    }
    ng_reset_s& set_ng_reset()
    {
      set(types::ng_reset);
      return c.get<ng_reset_s>();
    }
    ng_setup_request_s& set_ng_setup_request()
    {
      set(types::ng_setup_request);
      return c.get<ng_setup_request_s>();
    }
    path_switch_request_s& set_path_switch_request()
    {
      set(types::path_switch_request);
      return c.get<path_switch_request_s>();
    }
    pdu_session_res_modify_request_s& set_pdu_session_res_modify_request()
    {
      set(types::pdu_session_res_modify_request);
      return c.get<pdu_session_res_modify_request_s>();
    }
    pdu_session_res_modify_ind_s& set_pdu_session_res_modify_ind()
    {
      set(types::pdu_session_res_modify_ind);
      return c.get<pdu_session_res_modify_ind_s>();
    }
    pdu_session_res_release_cmd_s& set_pdu_session_res_release_cmd()
    {
      set(types::pdu_session_res_release_cmd);
      return c.get<pdu_session_res_release_cmd_s>();
    }
    pdu_session_res_setup_request_s& set_pdu_session_res_setup_request()
    {
      set(types::pdu_session_res_setup_request);
      return c.get<pdu_session_res_setup_request_s>();
    }
    pws_cancel_request_s& set_pws_cancel_request()
    {
      set(types::pws_cancel_request);
      return c.get<pws_cancel_request_s>();
    }
    ran_cfg_upd_s& set_ran_cfg_upd()
    {
      set(types::ran_cfg_upd);
      return c.get<ran_cfg_upd_s>();
    }
    ue_context_mod_request_s& set_ue_context_mod_request()
    {
      set(types::ue_context_mod_request);
      return c.get<ue_context_mod_request_s>();
    }
    ue_context_release_cmd_s& set_ue_context_release_cmd()
    {
      set(types::ue_context_release_cmd);
      return c.get<ue_context_release_cmd_s>();
    }
    ue_radio_cap_check_request_s& set_ue_radio_cap_check_request()
    {
      set(types::ue_radio_cap_check_request);
      return c.get<ue_radio_cap_check_request_s>();
    }
    write_replace_warning_request_s& set_write_replace_warning_request()
    {
      set(types::write_replace_warning_request);
      return c.get<write_replace_warning_request_s>();
    }
    amf_status_ind_s& set_amf_status_ind()
    {
      set(types::amf_status_ind);
      return c.get<amf_status_ind_s>();
    }
    cell_traffic_trace_s& set_cell_traffic_trace()
    {
      set(types::cell_traffic_trace);
      return c.get<cell_traffic_trace_s>();
    }
    deactiv_trace_s& set_deactiv_trace()
    {
      set(types::deactiv_trace);
      return c.get<deactiv_trace_s>();
    }
    dl_nas_transport_s& set_dl_nas_transport()
    {
      set(types::dl_nas_transport);
      return c.get<dl_nas_transport_s>();
    }
    dl_non_ueassociated_nrp_pa_transport_s& set_dl_non_ueassociated_nrp_pa_transport()
    {
      set(types::dl_non_ueassociated_nrp_pa_transport);
      return c.get<dl_non_ueassociated_nrp_pa_transport_s>();
    }
    dl_ran_cfg_transfer_s& set_dl_ran_cfg_transfer()
    {
      set(types::dl_ran_cfg_transfer);
      return c.get<dl_ran_cfg_transfer_s>();
    }
    dl_ran_status_transfer_s& set_dl_ran_status_transfer()
    {
      set(types::dl_ran_status_transfer);
      return c.get<dl_ran_status_transfer_s>();
    }
    dl_ueassociated_nrp_pa_transport_s& set_dl_ueassociated_nrp_pa_transport()
    {
      set(types::dl_ueassociated_nrp_pa_transport);
      return c.get<dl_ueassociated_nrp_pa_transport_s>();
    }
    error_ind_s& set_error_ind()
    {
      set(types::error_ind);
      return c.get<error_ind_s>();
    }
    ho_notify_s& set_ho_notify()
    {
      set(types::ho_notify);
      return c.get<ho_notify_s>();
    }
    init_ue_msg_s& set_init_ue_msg()
    {
      set(types::init_ue_msg);
      return c.get<init_ue_msg_s>();
    }
    location_report_s& set_location_report()
    {
      set(types::location_report);
      return c.get<location_report_s>();
    }
    location_report_ctrl_s& set_location_report_ctrl()
    {
      set(types::location_report_ctrl);
      return c.get<location_report_ctrl_s>();
    }
    location_report_fail_ind_s& set_location_report_fail_ind()
    {
      set(types::location_report_fail_ind);
      return c.get<location_report_fail_ind_s>();
    }
    nas_non_delivery_ind_s& set_nas_non_delivery_ind()
    {
      set(types::nas_non_delivery_ind);
      return c.get<nas_non_delivery_ind_s>();
    }
    overload_start_s& set_overload_start()
    {
      set(types::overload_start);
      return c.get<overload_start_s>();
    }
    overload_stop_s& set_overload_stop()
    {
      set(types::overload_stop);
      return c.get<overload_stop_s>();
    }
    paging_s& set_paging()
    {
      set(types::paging);
      return c.get<paging_s>();
    }
    pdu_session_res_notify_s& set_pdu_session_res_notify()
    {
      set(types::pdu_session_res_notify);
      return c.get<pdu_session_res_notify_s>();
    }
    private_msg_s& set_private_msg()
    {
      set(types::private_msg);
      return c.get<private_msg_s>();
    }
    pws_fail_ind_s& set_pws_fail_ind()
    {
      set(types::pws_fail_ind);
      return c.get<pws_fail_ind_s>();
    }
    pws_restart_ind_s& set_pws_restart_ind()
    {
      set(types::pws_restart_ind);
      return c.get<pws_restart_ind_s>();
    }
    reroute_nas_request_s& set_reroute_nas_request()
    {
      set(types::reroute_nas_request);
      return c.get<reroute_nas_request_s>();
    }
    rrc_inactive_transition_report_s& set_rrc_inactive_transition_report()
    {
      set(types::rrc_inactive_transition_report);
      return c.get<rrc_inactive_transition_report_s>();
    }
    secondary_rat_data_usage_report_s& set_secondary_rat_data_usage_report()
    {
      set(types::secondary_rat_data_usage_report);
      return c.get<secondary_rat_data_usage_report_s>();
    }
    trace_fail_ind_s& set_trace_fail_ind()
    {
      set(types::trace_fail_ind);
      return c.get<trace_fail_ind_s>();
    }
    trace_start_s& set_trace_start()
    {
      set(types::trace_start);
      return c.get<trace_start_s>();
    }
    ue_context_release_request_s& set_ue_context_release_request()
    {
      set(types::ue_context_release_request);
      return c.get<ue_context_release_request_s>();
    }
    ue_radio_cap_info_ind_s& set_ue_radio_cap_info_ind()
    {
      set(types::ue_radio_cap_info_ind);
      return c.get<ue_radio_cap_info_ind_s>();
    }
    uetnla_binding_release_request_s& set_uetnla_binding_release_request()
    {
      set(types::uetnla_binding_release_request);
      return c.get<uetnla_binding_release_request_s>();
    }
    ul_nas_transport_s& set_ul_nas_transport()
    {
      set(types::ul_nas_transport);
      return c.get<ul_nas_transport_s>();
    }
    ul_non_ueassociated_nrp_pa_transport_s& set_ul_non_ueassociated_nrp_pa_transport()
    {
      set(types::ul_non_ueassociated_nrp_pa_transport);
      return c.get<ul_non_ueassociated_nrp_pa_transport_s>();
    }
    ul_ran_cfg_transfer_s& set_ul_ran_cfg_transfer()
    {
      set(types::ul_ran_cfg_transfer);
      return c.get<ul_ran_cfg_transfer_s>();
    }
    ul_ran_status_transfer_s& set_ul_ran_status_transfer()
    {
      set(types::ul_ran_status_transfer);
      return c.get<ul_ran_status_transfer_s>();
    }
    ul_ueassociated_nrp_pa_transport_s& set_ul_ueassociated_nrp_pa_transport()
    {
      set(types::ul_ueassociated_nrp_pa_transport);
      return c.get<ul_ueassociated_nrp_pa_transport_s>();
    }

  private:
    types type_;
    choice_buffer_t<amf_cfg_upd_s,
                    amf_status_ind_s,
                    cell_traffic_trace_s,
                    deactiv_trace_s,
                    dl_nas_transport_s,
                    dl_non_ueassociated_nrp_pa_transport_s,
                    dl_ran_cfg_transfer_s,
                    dl_ran_status_transfer_s,
                    dl_ueassociated_nrp_pa_transport_s,
                    error_ind_s,
                    ho_cancel_s,
                    ho_notify_s,
                    ho_request_s,
                    ho_required_s,
                    init_context_setup_request_s,
                    init_ue_msg_s,
                    location_report_ctrl_s,
                    location_report_fail_ind_s,
                    location_report_s,
                    nas_non_delivery_ind_s,
                    ng_reset_s,
                    ng_setup_request_s,
                    overload_start_s,
                    overload_stop_s,
                    paging_s,
                    path_switch_request_s,
                    pdu_session_res_modify_ind_s,
                    pdu_session_res_modify_request_s,
                    pdu_session_res_notify_s,
                    pdu_session_res_release_cmd_s,
                    pdu_session_res_setup_request_s,
                    private_msg_s,
                    pws_cancel_request_s,
                    pws_fail_ind_s,
                    pws_restart_ind_s,
                    ran_cfg_upd_s,
                    reroute_nas_request_s,
                    rrc_inactive_transition_report_s,
                    secondary_rat_data_usage_report_s,
                    trace_fail_ind_s,
                    trace_start_s,
                    ue_context_mod_request_s,
                    ue_context_release_cmd_s,
                    ue_context_release_request_s,
                    ue_radio_cap_check_request_s,
                    ue_radio_cap_info_ind_s,
                    uetnla_binding_release_request_s,
                    ul_nas_transport_s,
                    ul_non_ueassociated_nrp_pa_transport_s,
                    ul_ran_cfg_transfer_s,
                    ul_ran_status_transfer_s,
                    ul_ueassociated_nrp_pa_transport_s,
                    write_replace_warning_request_s>
        c;

    void destroy_();
  };
  // SuccessfulOutcome ::= CLASS OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        amf_cfg_upd,
        ho_cancel,
        ho_required,
        ho_request,
        init_context_setup_request,
        ng_reset,
        ng_setup_request,
        path_switch_request,
        pdu_session_res_modify_request,
        pdu_session_res_modify_ind,
        pdu_session_res_release_cmd,
        pdu_session_res_setup_request,
        pws_cancel_request,
        ran_cfg_upd,
        ue_context_mod_request,
        ue_context_release_cmd,
        ue_radio_cap_check_request,
        write_replace_warning_request,
        amf_status_ind,
        cell_traffic_trace,
        deactiv_trace,
        dl_nas_transport,
        dl_non_ueassociated_nrp_pa_transport,
        dl_ran_cfg_transfer,
        dl_ran_status_transfer,
        dl_ueassociated_nrp_pa_transport,
        error_ind,
        ho_notify,
        init_ue_msg,
        location_report,
        location_report_ctrl,
        location_report_fail_ind,
        nas_non_delivery_ind,
        overload_start,
        overload_stop,
        paging,
        pdu_session_res_notify,
        private_msg,
        pws_fail_ind,
        pws_restart_ind,
        reroute_nas_request,
        rrc_inactive_transition_report,
        secondary_rat_data_usage_report,
        trace_fail_ind,
        trace_start,
        ue_context_release_request,
        ue_radio_cap_info_ind,
        uetnla_binding_release_request,
        ul_nas_transport,
        ul_non_ueassociated_nrp_pa_transport,
        ul_ran_cfg_transfer,
        ul_ran_status_transfer,
        ul_ueassociated_nrp_pa_transport,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    successful_outcome_c() = default;
    successful_outcome_c(const successful_outcome_c& other);
    successful_outcome_c& operator=(const successful_outcome_c& other);
    ~successful_outcome_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    amf_cfg_upd_ack_s& amf_cfg_upd()
    {
      assert_choice_type("AMFConfigurationUpdateAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<amf_cfg_upd_ack_s>();
    }
    ho_cancel_ack_s& ho_cancel()
    {
      assert_choice_type("HandoverCancelAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ho_cancel_ack_s>();
    }
    ho_cmd_s& ho_required()
    {
      assert_choice_type("HandoverCommand", type_.to_string(), "SuccessfulOutcome");
      return c.get<ho_cmd_s>();
    }
    ho_request_ack_s& ho_request()
    {
      assert_choice_type("HandoverRequestAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ho_request_ack_s>();
    }
    init_context_setup_resp_s& init_context_setup_request()
    {
      assert_choice_type("InitialContextSetupResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<init_context_setup_resp_s>();
    }
    ng_reset_ack_s& ng_reset()
    {
      assert_choice_type("NGResetAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ng_reset_ack_s>();
    }
    ng_setup_resp_s& ng_setup_request()
    {
      assert_choice_type("NGSetupResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<ng_setup_resp_s>();
    }
    path_switch_request_ack_s& path_switch_request()
    {
      assert_choice_type("PathSwitchRequestAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<path_switch_request_ack_s>();
    }
    pdu_session_res_modify_resp_s& pdu_session_res_modify_request()
    {
      assert_choice_type("PDUSessionResourceModifyResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_modify_resp_s>();
    }
    pdu_session_res_modify_confirm_s& pdu_session_res_modify_ind()
    {
      assert_choice_type("PDUSessionResourceModifyConfirm", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_modify_confirm_s>();
    }
    pdu_session_res_release_resp_s& pdu_session_res_release_cmd()
    {
      assert_choice_type("PDUSessionResourceReleaseResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_release_resp_s>();
    }
    pdu_session_res_setup_resp_s& pdu_session_res_setup_request()
    {
      assert_choice_type("PDUSessionResourceSetupResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_setup_resp_s>();
    }
    pws_cancel_resp_s& pws_cancel_request()
    {
      assert_choice_type("PWSCancelResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pws_cancel_resp_s>();
    }
    ran_cfg_upd_ack_s& ran_cfg_upd()
    {
      assert_choice_type("RANConfigurationUpdateAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ran_cfg_upd_ack_s>();
    }
    ue_context_mod_resp_s& ue_context_mod_request()
    {
      assert_choice_type("UEContextModificationResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<ue_context_mod_resp_s>();
    }
    ue_context_release_complete_s& ue_context_release_cmd()
    {
      assert_choice_type("UEContextReleaseComplete", type_.to_string(), "SuccessfulOutcome");
      return c.get<ue_context_release_complete_s>();
    }
    ue_radio_cap_check_resp_s& ue_radio_cap_check_request()
    {
      assert_choice_type("UERadioCapabilityCheckResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<ue_radio_cap_check_resp_s>();
    }
    write_replace_warning_resp_s& write_replace_warning_request()
    {
      assert_choice_type("WriteReplaceWarningResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<write_replace_warning_resp_s>();
    }
    const amf_cfg_upd_ack_s& amf_cfg_upd() const
    {
      assert_choice_type("AMFConfigurationUpdateAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<amf_cfg_upd_ack_s>();
    }
    const ho_cancel_ack_s& ho_cancel() const
    {
      assert_choice_type("HandoverCancelAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ho_cancel_ack_s>();
    }
    const ho_cmd_s& ho_required() const
    {
      assert_choice_type("HandoverCommand", type_.to_string(), "SuccessfulOutcome");
      return c.get<ho_cmd_s>();
    }
    const ho_request_ack_s& ho_request() const
    {
      assert_choice_type("HandoverRequestAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ho_request_ack_s>();
    }
    const init_context_setup_resp_s& init_context_setup_request() const
    {
      assert_choice_type("InitialContextSetupResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<init_context_setup_resp_s>();
    }
    const ng_reset_ack_s& ng_reset() const
    {
      assert_choice_type("NGResetAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ng_reset_ack_s>();
    }
    const ng_setup_resp_s& ng_setup_request() const
    {
      assert_choice_type("NGSetupResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<ng_setup_resp_s>();
    }
    const path_switch_request_ack_s& path_switch_request() const
    {
      assert_choice_type("PathSwitchRequestAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<path_switch_request_ack_s>();
    }
    const pdu_session_res_modify_resp_s& pdu_session_res_modify_request() const
    {
      assert_choice_type("PDUSessionResourceModifyResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_modify_resp_s>();
    }
    const pdu_session_res_modify_confirm_s& pdu_session_res_modify_ind() const
    {
      assert_choice_type("PDUSessionResourceModifyConfirm", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_modify_confirm_s>();
    }
    const pdu_session_res_release_resp_s& pdu_session_res_release_cmd() const
    {
      assert_choice_type("PDUSessionResourceReleaseResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_release_resp_s>();
    }
    const pdu_session_res_setup_resp_s& pdu_session_res_setup_request() const
    {
      assert_choice_type("PDUSessionResourceSetupResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pdu_session_res_setup_resp_s>();
    }
    const pws_cancel_resp_s& pws_cancel_request() const
    {
      assert_choice_type("PWSCancelResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<pws_cancel_resp_s>();
    }
    const ran_cfg_upd_ack_s& ran_cfg_upd() const
    {
      assert_choice_type("RANConfigurationUpdateAcknowledge", type_.to_string(), "SuccessfulOutcome");
      return c.get<ran_cfg_upd_ack_s>();
    }
    const ue_context_mod_resp_s& ue_context_mod_request() const
    {
      assert_choice_type("UEContextModificationResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<ue_context_mod_resp_s>();
    }
    const ue_context_release_complete_s& ue_context_release_cmd() const
    {
      assert_choice_type("UEContextReleaseComplete", type_.to_string(), "SuccessfulOutcome");
      return c.get<ue_context_release_complete_s>();
    }
    const ue_radio_cap_check_resp_s& ue_radio_cap_check_request() const
    {
      assert_choice_type("UERadioCapabilityCheckResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<ue_radio_cap_check_resp_s>();
    }
    const write_replace_warning_resp_s& write_replace_warning_request() const
    {
      assert_choice_type("WriteReplaceWarningResponse", type_.to_string(), "SuccessfulOutcome");
      return c.get<write_replace_warning_resp_s>();
    }
    amf_cfg_upd_ack_s& set_amf_cfg_upd()
    {
      set(types::amf_cfg_upd);
      return c.get<amf_cfg_upd_ack_s>();
    }
    ho_cancel_ack_s& set_ho_cancel()
    {
      set(types::ho_cancel);
      return c.get<ho_cancel_ack_s>();
    }
    ho_cmd_s& set_ho_required()
    {
      set(types::ho_required);
      return c.get<ho_cmd_s>();
    }
    ho_request_ack_s& set_ho_request()
    {
      set(types::ho_request);
      return c.get<ho_request_ack_s>();
    }
    init_context_setup_resp_s& set_init_context_setup_request()
    {
      set(types::init_context_setup_request);
      return c.get<init_context_setup_resp_s>();
    }
    ng_reset_ack_s& set_ng_reset()
    {
      set(types::ng_reset);
      return c.get<ng_reset_ack_s>();
    }
    ng_setup_resp_s& set_ng_setup_request()
    {
      set(types::ng_setup_request);
      return c.get<ng_setup_resp_s>();
    }
    path_switch_request_ack_s& set_path_switch_request()
    {
      set(types::path_switch_request);
      return c.get<path_switch_request_ack_s>();
    }
    pdu_session_res_modify_resp_s& set_pdu_session_res_modify_request()
    {
      set(types::pdu_session_res_modify_request);
      return c.get<pdu_session_res_modify_resp_s>();
    }
    pdu_session_res_modify_confirm_s& set_pdu_session_res_modify_ind()
    {
      set(types::pdu_session_res_modify_ind);
      return c.get<pdu_session_res_modify_confirm_s>();
    }
    pdu_session_res_release_resp_s& set_pdu_session_res_release_cmd()
    {
      set(types::pdu_session_res_release_cmd);
      return c.get<pdu_session_res_release_resp_s>();
    }
    pdu_session_res_setup_resp_s& set_pdu_session_res_setup_request()
    {
      set(types::pdu_session_res_setup_request);
      return c.get<pdu_session_res_setup_resp_s>();
    }
    pws_cancel_resp_s& set_pws_cancel_request()
    {
      set(types::pws_cancel_request);
      return c.get<pws_cancel_resp_s>();
    }
    ran_cfg_upd_ack_s& set_ran_cfg_upd()
    {
      set(types::ran_cfg_upd);
      return c.get<ran_cfg_upd_ack_s>();
    }
    ue_context_mod_resp_s& set_ue_context_mod_request()
    {
      set(types::ue_context_mod_request);
      return c.get<ue_context_mod_resp_s>();
    }
    ue_context_release_complete_s& set_ue_context_release_cmd()
    {
      set(types::ue_context_release_cmd);
      return c.get<ue_context_release_complete_s>();
    }
    ue_radio_cap_check_resp_s& set_ue_radio_cap_check_request()
    {
      set(types::ue_radio_cap_check_request);
      return c.get<ue_radio_cap_check_resp_s>();
    }
    write_replace_warning_resp_s& set_write_replace_warning_request()
    {
      set(types::write_replace_warning_request);
      return c.get<write_replace_warning_resp_s>();
    }

  private:
    types type_;
    choice_buffer_t<amf_cfg_upd_ack_s,
                    ho_cancel_ack_s,
                    ho_cmd_s,
                    ho_request_ack_s,
                    init_context_setup_resp_s,
                    ng_reset_ack_s,
                    ng_setup_resp_s,
                    path_switch_request_ack_s,
                    pdu_session_res_modify_confirm_s,
                    pdu_session_res_modify_resp_s,
                    pdu_session_res_release_resp_s,
                    pdu_session_res_setup_resp_s,
                    pws_cancel_resp_s,
                    ran_cfg_upd_ack_s,
                    ue_context_mod_resp_s,
                    ue_context_release_complete_s,
                    ue_radio_cap_check_resp_s,
                    write_replace_warning_resp_s>
        c;

    void destroy_();
  };
  // UnsuccessfulOutcome ::= CLASS OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        amf_cfg_upd,
        ho_cancel,
        ho_required,
        ho_request,
        init_context_setup_request,
        ng_reset,
        ng_setup_request,
        path_switch_request,
        pdu_session_res_modify_request,
        pdu_session_res_modify_ind,
        pdu_session_res_release_cmd,
        pdu_session_res_setup_request,
        pws_cancel_request,
        ran_cfg_upd,
        ue_context_mod_request,
        ue_context_release_cmd,
        ue_radio_cap_check_request,
        write_replace_warning_request,
        amf_status_ind,
        cell_traffic_trace,
        deactiv_trace,
        dl_nas_transport,
        dl_non_ueassociated_nrp_pa_transport,
        dl_ran_cfg_transfer,
        dl_ran_status_transfer,
        dl_ueassociated_nrp_pa_transport,
        error_ind,
        ho_notify,
        init_ue_msg,
        location_report,
        location_report_ctrl,
        location_report_fail_ind,
        nas_non_delivery_ind,
        overload_start,
        overload_stop,
        paging,
        pdu_session_res_notify,
        private_msg,
        pws_fail_ind,
        pws_restart_ind,
        reroute_nas_request,
        rrc_inactive_transition_report,
        secondary_rat_data_usage_report,
        trace_fail_ind,
        trace_start,
        ue_context_release_request,
        ue_radio_cap_info_ind,
        uetnla_binding_release_request,
        ul_nas_transport,
        ul_non_ueassociated_nrp_pa_transport,
        ul_ran_cfg_transfer,
        ul_ran_status_transfer,
        ul_ueassociated_nrp_pa_transport,
        nulltype
      } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    unsuccessful_outcome_c() = default;
    unsuccessful_outcome_c(const unsuccessful_outcome_c& other);
    unsuccessful_outcome_c& operator=(const unsuccessful_outcome_c& other);
    ~unsuccessful_outcome_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(bit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    amf_cfg_upd_fail_s& amf_cfg_upd()
    {
      assert_choice_type("AMFConfigurationUpdateFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<amf_cfg_upd_fail_s>();
    }
    ho_prep_fail_s& ho_required()
    {
      assert_choice_type("HandoverPreparationFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ho_prep_fail_s>();
    }
    ho_fail_s& ho_request()
    {
      assert_choice_type("HandoverFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ho_fail_s>();
    }
    init_context_setup_fail_s& init_context_setup_request()
    {
      assert_choice_type("InitialContextSetupFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<init_context_setup_fail_s>();
    }
    ng_setup_fail_s& ng_setup_request()
    {
      assert_choice_type("NGSetupFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ng_setup_fail_s>();
    }
    path_switch_request_fail_s& path_switch_request()
    {
      assert_choice_type("PathSwitchRequestFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<path_switch_request_fail_s>();
    }
    ran_cfg_upd_fail_s& ran_cfg_upd()
    {
      assert_choice_type("RANConfigurationUpdateFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ran_cfg_upd_fail_s>();
    }
    ue_context_mod_fail_s& ue_context_mod_request()
    {
      assert_choice_type("UEContextModificationFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ue_context_mod_fail_s>();
    }
    const amf_cfg_upd_fail_s& amf_cfg_upd() const
    {
      assert_choice_type("AMFConfigurationUpdateFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<amf_cfg_upd_fail_s>();
    }
    const ho_prep_fail_s& ho_required() const
    {
      assert_choice_type("HandoverPreparationFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ho_prep_fail_s>();
    }
    const ho_fail_s& ho_request() const
    {
      assert_choice_type("HandoverFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ho_fail_s>();
    }
    const init_context_setup_fail_s& init_context_setup_request() const
    {
      assert_choice_type("InitialContextSetupFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<init_context_setup_fail_s>();
    }
    const ng_setup_fail_s& ng_setup_request() const
    {
      assert_choice_type("NGSetupFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ng_setup_fail_s>();
    }
    const path_switch_request_fail_s& path_switch_request() const
    {
      assert_choice_type("PathSwitchRequestFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<path_switch_request_fail_s>();
    }
    const ran_cfg_upd_fail_s& ran_cfg_upd() const
    {
      assert_choice_type("RANConfigurationUpdateFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ran_cfg_upd_fail_s>();
    }
    const ue_context_mod_fail_s& ue_context_mod_request() const
    {
      assert_choice_type("UEContextModificationFailure", type_.to_string(), "UnsuccessfulOutcome");
      return c.get<ue_context_mod_fail_s>();
    }
    amf_cfg_upd_fail_s& set_amf_cfg_upd()
    {
      set(types::amf_cfg_upd);
      return c.get<amf_cfg_upd_fail_s>();
    }
    ho_prep_fail_s& set_ho_required()
    {
      set(types::ho_required);
      return c.get<ho_prep_fail_s>();
    }
    ho_fail_s& set_ho_request()
    {
      set(types::ho_request);
      return c.get<ho_fail_s>();
    }
    init_context_setup_fail_s& set_init_context_setup_request()
    {
      set(types::init_context_setup_request);
      return c.get<init_context_setup_fail_s>();
    }
    ng_setup_fail_s& set_ng_setup_request()
    {
      set(types::ng_setup_request);
      return c.get<ng_setup_fail_s>();
    }
    path_switch_request_fail_s& set_path_switch_request()
    {
      set(types::path_switch_request);
      return c.get<path_switch_request_fail_s>();
    }
    ran_cfg_upd_fail_s& set_ran_cfg_upd()
    {
      set(types::ran_cfg_upd);
      return c.get<ran_cfg_upd_fail_s>();
    }
    ue_context_mod_fail_s& set_ue_context_mod_request()
    {
      set(types::ue_context_mod_request);
      return c.get<ue_context_mod_fail_s>();
    }

  private:
    types type_;
    choice_buffer_t<amf_cfg_upd_fail_s,
                    ho_fail_s,
                    ho_prep_fail_s,
                    init_context_setup_fail_s,
                    ng_setup_fail_s,
                    path_switch_request_fail_s,
                    ran_cfg_upd_fail_s,
                    ue_context_mod_fail_s>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// InitiatingMessage ::= SEQUENCE{{NGAP-ELEMENTARY-PROCEDURE}}
struct init_msg_s {
  uint16_t                      proc_code = 0;
  crit_e                        crit;
  ngap_elem_procs_o::init_msg_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// LastVisitedNGRANCellInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o last_visited_ngran_cell_info_ext_ies_o;

// LastVisitedCellInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
typedef ngap_protocol_ies_empty_o last_visited_cell_info_ext_ies_o;

// LastVisitedNGRANCellInformation ::= SEQUENCE
struct last_visited_ngran_cell_info_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool        ext                                                 = false;
  bool        time_ue_stayed_in_cell_enhanced_granularity_present = false;
  bool        hocause_value_present                               = false;
  bool        ie_exts_present                                     = false;
  ngran_cgi_c global_cell_id;
  cell_type_s cell_type;
  uint16_t    time_ue_stayed_in_cell                      = 0;
  uint16_t    time_ue_stayed_in_cell_enhanced_granularity = 0;
  cause_c     hocause_value;
  ie_exts_l_  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LastVisitedCellInformation ::= CHOICE
struct last_visited_cell_info_c {
  struct types_opts {
    enum options { ngran_cell, eutran_cell, utran_cell, geran_cell, choice_exts, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  last_visited_cell_info_c() = default;
  last_visited_cell_info_c(const last_visited_cell_info_c& other);
  last_visited_cell_info_c& operator=(const last_visited_cell_info_c& other);
  ~last_visited_cell_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  last_visited_ngran_cell_info_s& ngran_cell()
  {
    assert_choice_type("nGRANCell", type_.to_string(), "LastVisitedCellInformation");
    return c.get<last_visited_ngran_cell_info_s>();
  }
  unbounded_octstring<true>& eutran_cell()
  {
    assert_choice_type("eUTRANCell", type_.to_string(), "LastVisitedCellInformation");
    return c.get<unbounded_octstring<true> >();
  }
  unbounded_octstring<true>& utran_cell()
  {
    assert_choice_type("uTRANCell", type_.to_string(), "LastVisitedCellInformation");
    return c.get<unbounded_octstring<true> >();
  }
  unbounded_octstring<true>& geran_cell()
  {
    assert_choice_type("gERANCell", type_.to_string(), "LastVisitedCellInformation");
    return c.get<unbounded_octstring<true> >();
  }
  protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "LastVisitedCellInformation");
    return c.get<protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o> >();
  }
  const last_visited_ngran_cell_info_s& ngran_cell() const
  {
    assert_choice_type("nGRANCell", type_.to_string(), "LastVisitedCellInformation");
    return c.get<last_visited_ngran_cell_info_s>();
  }
  const unbounded_octstring<true>& eutran_cell() const
  {
    assert_choice_type("eUTRANCell", type_.to_string(), "LastVisitedCellInformation");
    return c.get<unbounded_octstring<true> >();
  }
  const unbounded_octstring<true>& utran_cell() const
  {
    assert_choice_type("uTRANCell", type_.to_string(), "LastVisitedCellInformation");
    return c.get<unbounded_octstring<true> >();
  }
  const unbounded_octstring<true>& geran_cell() const
  {
    assert_choice_type("gERANCell", type_.to_string(), "LastVisitedCellInformation");
    return c.get<unbounded_octstring<true> >();
  }
  const protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type("choice-Extensions", type_.to_string(), "LastVisitedCellInformation");
    return c.get<protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o> >();
  }
  last_visited_ngran_cell_info_s& set_ngran_cell()
  {
    set(types::ngran_cell);
    return c.get<last_visited_ngran_cell_info_s>();
  }
  unbounded_octstring<true>& set_eutran_cell()
  {
    set(types::eutran_cell);
    return c.get<unbounded_octstring<true> >();
  }
  unbounded_octstring<true>& set_utran_cell()
  {
    set(types::utran_cell);
    return c.get<unbounded_octstring<true> >();
  }
  unbounded_octstring<true>& set_geran_cell()
  {
    set(types::geran_cell);
    return c.get<unbounded_octstring<true> >();
  }
  protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o>& set_choice_exts()
  {
    set(types::choice_exts);
    return c.get<protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o> >();
  }

private:
  types type_;
  choice_buffer_t<last_visited_ngran_cell_info_s,
                  protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o>,
                  unbounded_octstring<true> >
      c;

  void destroy_();
};

// LastVisitedCellItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o last_visited_cell_item_ext_ies_o;

// LastVisitedCellItem ::= SEQUENCE
struct last_visited_cell_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  last_visited_cell_info_c last_visited_cell_info;
  ie_exts_l_               ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SuccessfulOutcome ::= SEQUENCE{{NGAP-ELEMENTARY-PROCEDURE}}
struct successful_outcome_s {
  uint16_t                                proc_code = 0;
  crit_e                                  crit;
  ngap_elem_procs_o::successful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// UnsuccessfulOutcome ::= SEQUENCE{{NGAP-ELEMENTARY-PROCEDURE}}
struct unsuccessful_outcome_s {
  uint16_t                                  proc_code = 0;
  crit_e                                    crit;
  ngap_elem_procs_o::unsuccessful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// NGAP-PDU ::= CHOICE
struct ngap_pdu_c {
  struct types_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  ngap_pdu_c() = default;
  ngap_pdu_c(const ngap_pdu_c& other);
  ngap_pdu_c& operator=(const ngap_pdu_c& other);
  ~ngap_pdu_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  init_msg_s& init_msg()
  {
    assert_choice_type("initiatingMessage", type_.to_string(), "NGAP-PDU");
    return c.get<init_msg_s>();
  }
  successful_outcome_s& successful_outcome()
  {
    assert_choice_type("successfulOutcome", type_.to_string(), "NGAP-PDU");
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& unsuccessful_outcome()
  {
    assert_choice_type("unsuccessfulOutcome", type_.to_string(), "NGAP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  const init_msg_s& init_msg() const
  {
    assert_choice_type("initiatingMessage", type_.to_string(), "NGAP-PDU");
    return c.get<init_msg_s>();
  }
  const successful_outcome_s& successful_outcome() const
  {
    assert_choice_type("successfulOutcome", type_.to_string(), "NGAP-PDU");
    return c.get<successful_outcome_s>();
  }
  const unsuccessful_outcome_s& unsuccessful_outcome() const
  {
    assert_choice_type("unsuccessfulOutcome", type_.to_string(), "NGAP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  init_msg_s& set_init_msg()
  {
    set(types::init_msg);
    return c.get<init_msg_s>();
  }
  successful_outcome_s& set_successful_outcome()
  {
    set(types::successful_outcome);
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& set_unsuccessful_outcome()
  {
    set(types::unsuccessful_outcome);
    return c.get<unsuccessful_outcome_s>();
  }

private:
  types                                                                     type_;
  choice_buffer_t<init_msg_s, successful_outcome_s, unsuccessful_outcome_s> c;

  void destroy_();
};

// QosFlowInformationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_info_item_ext_ies_o;

// QosFlowInformationItem ::= SEQUENCE
struct qos_flow_info_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool            ext                  = false;
  bool            dlforwarding_present = false;
  bool            ie_exts_present      = false;
  uint8_t         qos_flow_id          = 0;
  dl_forwarding_e dlforwarding;
  ie_exts_l_      ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceInformationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o pdu_session_res_info_item_ext_ies_o;

// QosFlowInformationList ::= SEQUENCE (SIZE (1..64)) OF QosFlowInformationItem
using qos_flow_info_list_l = dyn_array<qos_flow_info_item_s>;

// PDUSessionResourceInformationItem ::= SEQUENCE
struct pdu_session_res_info_item_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                         ext                                = false;
  bool                         drbs_to_qos_flows_map_list_present = false;
  bool                         ie_exts_present                    = false;
  uint16_t                     pdu_session_id                     = 0;
  qos_flow_info_list_l         qos_flow_info_list;
  drbs_to_qos_flows_map_list_l drbs_to_qos_flows_map_list;
  ie_exts_l_                   ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceInformationList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceInformationItem
using pdu_session_res_info_list_l = dyn_array<pdu_session_res_info_item_s>;

// ProtocolIE-ContainerList{INTEGER : lowerBound, INTEGER : upperBound, NGAP-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE
// (SIZE (lowerBound..upperBound)) OF ProtocolIE-Field{NGAP-PROTOCOL-IES : IEsSetParam}
template <class ies_set_param>
using protocol_ie_container_list_l = dyn_array<protocol_ie_single_container_s<ies_set_param> >;

// ProtocolIE-FieldPair{NGAP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{NGAP-PROTOCOL-IES-PAIR}}
template <class ies_set_param>
struct protocol_ie_field_pair_s {
  uint32_t                              id = 0;
  crit_e                                first_crit;
  typename ies_set_param::first_value_c first_value;
  crit_e                                second_crit;
  typename ies_set_param::first_value_c second_value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint32_t& id_);
};

// ProtocolIE-ContainerPair{NGAP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE (SIZE (0..65535)) OF ProtocolIE-FieldPair
template <class ies_set_param>
using protocol_ie_container_pair_l = dyn_array<protocol_ie_field_pair_s<ies_set_param> >;

// ProtocolIE-ContainerPairList{INTEGER : lowerBound, INTEGER : upperBound, NGAP-PROTOCOL-IES-PAIR : IEsSetParam} ::=
// SEQUENCE (SIZE (lowerBound..upperBound)) OF ProtocolIE-ContainerPair
template <class ies_set_param>
using protocol_ie_container_pair_list_l = dyn_array<protocol_ie_container_pair_l<ies_set_param> >;

// QosFlowSetupResponseItemSURes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o qos_flow_setup_resp_item_su_res_ext_ies_o;

// QosFlowSetupResponseItemSURes ::= SEQUENCE
struct qos_flow_setup_resp_item_su_res_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool       ext             = false;
  bool       ie_exts_present = false;
  uint8_t    qos_flow_id     = 0;
  ie_exts_l_ ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowSetupResponseListSURes ::= SEQUENCE (SIZE (1..64)) OF QosFlowSetupResponseItemSURes
using qos_flow_setup_resp_list_su_res_l = dyn_array<qos_flow_setup_resp_item_su_res_s>;

// SourceNGRANNode-ToTargetNGRANNode-TransparentContainer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o source_ngran_node_to_target_ngran_node_transparent_container_ext_ies_o;

// UEHistoryInformation ::= SEQUENCE (SIZE (1..16)) OF LastVisitedCellItem
using ue_history_info_l = dyn_array<last_visited_cell_item_s>;

// SourceNGRANNode-ToTargetNGRANNode-TransparentContainer ::= SEQUENCE
struct source_ngran_node_to_target_ngran_node_transparent_container_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                        ext                               = false;
  bool                        pdu_session_res_info_list_present = false;
  bool                        e_rab_info_list_present           = false;
  bool                        idx_to_rfsp_present               = false;
  bool                        ie_exts_present                   = false;
  unbounded_octstring<true>   rrc_container;
  pdu_session_res_info_list_l pdu_session_res_info_list;
  e_rab_info_list_l           e_rab_info_list;
  ngran_cgi_c                 target_cell_id;
  uint16_t                    idx_to_rfsp = 1;
  ue_history_info_l           uehistory_info;
  ie_exts_l_                  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargetNGRANNode-ToSourceNGRANNode-TransparentContainer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
typedef ngap_protocol_ext_empty_o target_ngran_node_to_source_ngran_node_transparent_container_ext_ies_o;

// TargetNGRANNode-ToSourceNGRANNode-TransparentContainer ::= SEQUENCE
struct target_ngran_node_to_source_ngran_node_transparent_container_s {
  typedef protocol_ext_container_empty_l ie_exts_l_;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  unbounded_octstring<true> rrc_container;
  ie_exts_l_                ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace ngap_nr
} // namespace asn1

#endif // SRSASN1_NGAP_NR_H
