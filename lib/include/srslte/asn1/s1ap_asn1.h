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

/*******************************************************************************
 *
 *                     3GPP TS ASN1 S1AP v15.7.1 (2019-10)
 *
 ******************************************************************************/

#ifndef SRSASN1_S1AP_H
#define SRSASN1_S1AP_H

#include "asn1_utils.h"
#include <cstdio>
#include <stdarg.h>

namespace asn1 {
namespace s1ap {

/*******************************************************************************
 *                        Functions for external logging
 ******************************************************************************/

void log_invalid_access_choice_id(uint32_t val, uint32_t choice_id);

void assert_choice_type(uint32_t val, uint32_t choice_id);

void assert_choice_type(const std::string& access_type,
                        const std::string& current_type,
                        const std::string& choice_type);

const char* convert_enum_idx(const char* array[], uint32_t nof_types, uint32_t enum_val, const char* enum_type);

template <class ItemType>
ItemType map_enum_number(ItemType* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_S1AP_ID_HO_PREP 0
#define ASN1_S1AP_ID_HO_RES_ALLOC 1
#define ASN1_S1AP_ID_HO_NOTIF 2
#define ASN1_S1AP_ID_PATH_SWITCH_REQUEST 3
#define ASN1_S1AP_ID_HO_CANCEL 4
#define ASN1_S1AP_ID_ERAB_SETUP 5
#define ASN1_S1AP_ID_ERAB_MODIFY 6
#define ASN1_S1AP_ID_ERAB_RELEASE 7
#define ASN1_S1AP_ID_ERAB_RELEASE_IND 8
#define ASN1_S1AP_ID_INIT_CONTEXT_SETUP 9
#define ASN1_S1AP_ID_PAGING 10
#define ASN1_S1AP_ID_DL_NAS_TRANSPORT 11
#define ASN1_S1AP_ID_INIT_UE_MSG 12
#define ASN1_S1AP_ID_UL_NAS_TRANSPORT 13
#define ASN1_S1AP_ID_RESET 14
#define ASN1_S1AP_ID_ERROR_IND 15
#define ASN1_S1AP_ID_NAS_NON_DELIVERY_IND 16
#define ASN1_S1AP_ID_S1_SETUP 17
#define ASN1_S1AP_ID_UE_CONTEXT_RELEASE_REQUEST 18
#define ASN1_S1AP_ID_DL_S1CDMA2000TUNNELLING 19
#define ASN1_S1AP_ID_UL_S1CDMA2000TUNNELLING 20
#define ASN1_S1AP_ID_UE_CONTEXT_MOD 21
#define ASN1_S1AP_ID_UE_CAP_INFO_IND 22
#define ASN1_S1AP_ID_UE_CONTEXT_RELEASE 23
#define ASN1_S1AP_ID_ENB_STATUS_TRANSFER 24
#define ASN1_S1AP_ID_MME_STATUS_TRANSFER 25
#define ASN1_S1AP_ID_DEACTIV_TRACE 26
#define ASN1_S1AP_ID_TRACE_START 27
#define ASN1_S1AP_ID_TRACE_FAIL_IND 28
#define ASN1_S1AP_ID_ENB_CFG_UPD 29
#define ASN1_S1AP_ID_MME_CFG_UPD 30
#define ASN1_S1AP_ID_LOCATION_REPORT_CTRL 31
#define ASN1_S1AP_ID_LOCATION_REPORT_FAIL_IND 32
#define ASN1_S1AP_ID_LOCATION_REPORT 33
#define ASN1_S1AP_ID_OVERLOAD_START 34
#define ASN1_S1AP_ID_OVERLOAD_STOP 35
#define ASN1_S1AP_ID_WRITE_REPLACE_WARNING 36
#define ASN1_S1AP_ID_ENB_DIRECT_INFO_TRANSFER 37
#define ASN1_S1AP_ID_MME_DIRECT_INFO_TRANSFER 38
#define ASN1_S1AP_ID_PRIVATE_MSG 39
#define ASN1_S1AP_ID_ENB_CFG_TRANSFER 40
#define ASN1_S1AP_ID_MME_CFG_TRANSFER 41
#define ASN1_S1AP_ID_CELL_TRAFFIC_TRACE 42
#define ASN1_S1AP_ID_KILL 43
#define ASN1_S1AP_ID_DL_UEASSOCIATED_LP_PA_TRANSPORT 44
#define ASN1_S1AP_ID_UL_UEASSOCIATED_LP_PA_TRANSPORT 45
#define ASN1_S1AP_ID_DL_NON_UEASSOCIATED_LP_PA_TRANSPORT 46
#define ASN1_S1AP_ID_UL_NON_UEASSOCIATED_LP_PA_TRANSPORT 47
#define ASN1_S1AP_ID_UE_RADIO_CAP_MATCH 48
#define ASN1_S1AP_ID_PWS_RESTART_IND 49
#define ASN1_S1AP_ID_ERAB_MOD_IND 50
#define ASN1_S1AP_ID_PWS_FAIL_IND 51
#define ASN1_S1AP_ID_REROUTE_NAS_REQUEST 52
#define ASN1_S1AP_ID_UE_CONTEXT_MOD_IND 53
#define ASN1_S1AP_ID_CONN_ESTABLISHMENT_IND 54
#define ASN1_S1AP_ID_UE_CONTEXT_SUSPEND 55
#define ASN1_S1AP_ID_UE_CONTEXT_RESUME 56
#define ASN1_S1AP_ID_NAS_DELIVERY_IND 57
#define ASN1_S1AP_ID_RETRIEVE_UE_INFO 58
#define ASN1_S1AP_ID_UE_INFO_TRANSFER 59
#define ASN1_S1AP_ID_ENBCP_RELOCATION_IND 60
#define ASN1_S1AP_ID_MMECP_RELOCATION_IND 61
#define ASN1_S1AP_MAX_PRIVATE_IES 65535
#define ASN1_S1AP_MAX_PROTOCOL_EXTS 65535
#define ASN1_S1AP_MAX_PROTOCOL_IES 65535
#define ASN1_S1AP_MAXNOOF_CSGS 256
#define ASN1_S1AP_MAXNOOF_ERABS 256
#define ASN1_S1AP_MAXNOOF_TAIS 256
#define ASN1_S1AP_MAXNOOF_TACS 256
#define ASN1_S1AP_MAXNOOF_ERRORS 256
#define ASN1_S1AP_MAXNOOF_BPLMNS 6
#define ASN1_S1AP_MAXNOOF_PLMNS_PER_MME 32
#define ASN1_S1AP_MAXNOOF_EPLMNS 15
#define ASN1_S1AP_MAXNOOF_EPLMNS_PLUS_ONE 16
#define ASN1_S1AP_MAXNOOF_FORB_LACS 4096
#define ASN1_S1AP_MAXNOOF_FORB_TACS 4096
#define ASN1_S1AP_MAXNOOF_INDIVIDUAL_S1_CONNS_TO_RESET 256
#define ASN1_S1AP_MAXNOOF_CELLS 16
#define ASN1_S1AP_MAXNOOF_CELLSINENB 256
#define ASN1_S1AP_MAXNOOF_TA_IFOR_WARNING 65535
#define ASN1_S1AP_MAXNOOF_CELL_ID 65535
#define ASN1_S1AP_MAXNOOF_DCNS 32
#define ASN1_S1AP_MAXNOOF_EMERGENCY_AREA_ID 65535
#define ASN1_S1AP_MAXNOOF_CELLIN_TAI 65535
#define ASN1_S1AP_MAXNOOF_CELLIN_EAI 65535
#define ASN1_S1AP_MAXNOOFENBX2_TLAS 2
#define ASN1_S1AP_MAXNOOFENBX2_EXT_TLAS 16
#define ASN1_S1AP_MAXNOOFENBX2_GTPTLAS 16
#define ASN1_S1AP_MAXNOOF_RATS 8
#define ASN1_S1AP_MAXNOOF_GROUP_IDS 65535
#define ASN1_S1AP_MAXNOOF_MMECS 256
#define ASN1_S1AP_MAXNOOF_CELL_IDFOR_MDT 32
#define ASN1_S1AP_MAXNOOF_TAFOR_MDT 8
#define ASN1_S1AP_MAXNOOF_MDTPLMNS 16
#define ASN1_S1AP_MAXNOOF_CELLSFOR_RESTART 256
#define ASN1_S1AP_MAXNOOF_RESTART_TAIS 2048
#define ASN1_S1AP_MAXNOOF_RESTART_EMERGENCY_AREA_IDS 256
#define ASN1_S1AP_MAX_EARFCN 262143
#define ASN1_S1AP_MAXNOOF_MBSFN_AREA_MDT 8
#define ASN1_S1AP_MAXNOOF_RECOMMENDED_CELLS 16
#define ASN1_S1AP_MAXNOOF_RECOMMENDED_ENBS 16
#define ASN1_S1AP_ID_MME_UE_S1AP_ID 0
#define ASN1_S1AP_ID_HANDOV_TYPE 1
#define ASN1_S1AP_ID_CAUSE 2
#define ASN1_S1AP_ID_SOURCE_ID 3
#define ASN1_S1AP_ID_TARGET_ID 4
#define ASN1_S1AP_ID_ENB_UE_S1AP_ID 8
#define ASN1_S1AP_ID_ERAB_SUBJECTTO_DATA_FORWARDING_LIST 12
#define ASN1_S1AP_ID_ERAB_TO_RELEASE_LIST_HO_CMD 13
#define ASN1_S1AP_ID_ERAB_DATA_FORWARDING_ITEM 14
#define ASN1_S1AP_ID_ERAB_RELEASE_ITEM_BEARER_REL_COMP 15
#define ASN1_S1AP_ID_ERAB_TO_BE_SETUP_LIST_BEARER_SU_REQ 16
#define ASN1_S1AP_ID_ERAB_TO_BE_SETUP_ITEM_BEARER_SU_REQ 17
#define ASN1_S1AP_ID_ERAB_ADMITTED_LIST 18
#define ASN1_S1AP_ID_ERAB_FAILED_TO_SETUP_LIST_HO_REQ_ACK 19
#define ASN1_S1AP_ID_ERAB_ADMITTED_ITEM 20
#define ASN1_S1AP_ID_ERAB_FAILEDTO_SETUP_ITEM_HO_REQ_ACK 21
#define ASN1_S1AP_ID_ERAB_TO_BE_SWITCHED_DL_LIST 22
#define ASN1_S1AP_ID_ERAB_TO_BE_SWITCHED_DL_ITEM 23
#define ASN1_S1AP_ID_ERAB_TO_BE_SETUP_LIST_CTXT_SU_REQ 24
#define ASN1_S1AP_ID_TRACE_ACTIVATION 25
#define ASN1_S1AP_ID_NAS_PDU 26
#define ASN1_S1AP_ID_ERAB_TO_BE_SETUP_ITEM_HO_REQ 27
#define ASN1_S1AP_ID_ERAB_SETUP_LIST_BEARER_SU_RES 28
#define ASN1_S1AP_ID_ERAB_FAILED_TO_SETUP_LIST_BEARER_SU_RES 29
#define ASN1_S1AP_ID_ERAB_TO_BE_MODIFIED_LIST_BEARER_MOD_REQ 30
#define ASN1_S1AP_ID_ERAB_MODIFY_LIST_BEARER_MOD_RES 31
#define ASN1_S1AP_ID_ERAB_FAILED_TO_MODIFY_LIST 32
#define ASN1_S1AP_ID_ERAB_TO_BE_RELEASED_LIST 33
#define ASN1_S1AP_ID_ERAB_FAILED_TO_RELEASE_LIST 34
#define ASN1_S1AP_ID_ERAB_ITEM 35
#define ASN1_S1AP_ID_ERAB_TO_BE_MODIFIED_ITEM_BEARER_MOD_REQ 36
#define ASN1_S1AP_ID_ERAB_MODIFY_ITEM_BEARER_MOD_RES 37
#define ASN1_S1AP_ID_ERAB_RELEASE_ITEM 38
#define ASN1_S1AP_ID_ERAB_SETUP_ITEM_BEARER_SU_RES 39
#define ASN1_S1AP_ID_SECURITY_CONTEXT 40
#define ASN1_S1AP_ID_HO_RESTRICT_LIST 41
#define ASN1_S1AP_ID_UE_PAGING_ID 43
#define ASN1_S1AP_ID_PAGING_DRX 44
#define ASN1_S1AP_ID_TAI_LIST 46
#define ASN1_S1AP_ID_TAI_ITEM 47
#define ASN1_S1AP_ID_ERAB_FAILED_TO_SETUP_LIST_CTXT_SU_RES 48
#define ASN1_S1AP_ID_ERAB_RELEASE_ITEM_HO_CMD 49
#define ASN1_S1AP_ID_ERAB_SETUP_ITEM_CTXT_SU_RES 50
#define ASN1_S1AP_ID_ERAB_SETUP_LIST_CTXT_SU_RES 51
#define ASN1_S1AP_ID_ERAB_TO_BE_SETUP_ITEM_CTXT_SU_REQ 52
#define ASN1_S1AP_ID_ERAB_TO_BE_SETUP_LIST_HO_REQ 53
#define ASN1_S1AP_ID_GERA_NTO_LTEHO_INFO_RES 55
#define ASN1_S1AP_ID_UTRA_NTO_LTEHO_INFO_RES 57
#define ASN1_S1AP_ID_CRIT_DIAGNOSTICS 58
#define ASN1_S1AP_ID_GLOBAL_ENB_ID 59
#define ASN1_S1AP_ID_ENBNAME 60
#define ASN1_S1AP_ID_MM_ENAME 61
#define ASN1_S1AP_ID_SERVED_PLMNS 63
#define ASN1_S1AP_ID_SUPPORTED_TAS 64
#define ASN1_S1AP_ID_TIME_TO_WAIT 65
#define ASN1_S1AP_ID_U_EAGGREGATE_MAXIMUM_BITRATE 66
#define ASN1_S1AP_ID_TAI 67
#define ASN1_S1AP_ID_ERAB_RELEASE_LIST_BEARER_REL_COMP 69
#define ASN1_S1AP_ID_CDMA2000_PDU 70
#define ASN1_S1AP_ID_CDMA2000_RAT_TYPE 71
#define ASN1_S1AP_ID_CDMA2000_SECTOR_ID 72
#define ASN1_S1AP_ID_SECURITY_KEY 73
#define ASN1_S1AP_ID_UE_RADIO_CAP 74
#define ASN1_S1AP_ID_GUMMEI_ID 75
#define ASN1_S1AP_ID_ERAB_INFO_LIST_ITEM 78
#define ASN1_S1AP_ID_DIRECT_FORWARDING_PATH_AVAILABILITY 79
#define ASN1_S1AP_ID_UE_ID_IDX_VALUE 80
#define ASN1_S1AP_ID_CDMA2000_HO_STATUS 83
#define ASN1_S1AP_ID_CDMA2000_HO_REQUIRED_IND 84
#define ASN1_S1AP_ID_E_UTRAN_TRACE_ID 86
#define ASN1_S1AP_ID_RELATIVE_MME_CAPACITY 87
#define ASN1_S1AP_ID_SOURCE_MME_UE_S1AP_ID 88
#define ASN1_S1AP_ID_BEARERS_SUBJECT_TO_STATUS_TRANSFER_ITEM 89
#define ASN1_S1AP_ID_ENB_STATUS_TRANSFER_TRANSPARENT_CONTAINER 90
#define ASN1_S1AP_ID_UE_ASSOCIATED_LC_S1_CONN_ITEM 91
#define ASN1_S1AP_ID_RESET_TYPE 92
#define ASN1_S1AP_ID_UE_ASSOCIATED_LC_S1_CONN_LIST_RES_ACK 93
#define ASN1_S1AP_ID_ERAB_TO_BE_SWITCHED_UL_ITEM 94
#define ASN1_S1AP_ID_ERAB_TO_BE_SWITCHED_UL_LIST 95
#define ASN1_S1AP_ID_S_TMSI 96
#define ASN1_S1AP_ID_CDMA2000_ONE_XRAND 97
#define ASN1_S1AP_ID_REQUEST_TYPE 98
#define ASN1_S1AP_ID_UE_S1AP_IDS 99
#define ASN1_S1AP_ID_EUTRAN_CGI 100
#define ASN1_S1AP_ID_OVERLOAD_RESP 101
#define ASN1_S1AP_ID_CDMA2000_ONE_XSRVCC_INFO 102
#define ASN1_S1AP_ID_ERAB_FAILED_TO_BE_RELEASED_LIST 103
#define ASN1_S1AP_ID_SOURCE_TO_TARGET_TRANSPARENT_CONTAINER 104
#define ASN1_S1AP_ID_SERVED_GUMMEIS 105
#define ASN1_S1AP_ID_SUBSCRIBER_PROFILE_IDFOR_RFP 106
#define ASN1_S1AP_ID_UE_SECURITY_CAP 107
#define ASN1_S1AP_ID_CS_FALLBACK_IND 108
#define ASN1_S1AP_ID_CN_DOMAIN 109
#define ASN1_S1AP_ID_ERAB_RELEASED_LIST 110
#define ASN1_S1AP_ID_MSG_ID 111
#define ASN1_S1AP_ID_SERIAL_NUM 112
#define ASN1_S1AP_ID_WARNING_AREA_LIST 113
#define ASN1_S1AP_ID_REPEAT_PERIOD 114
#define ASN1_S1AP_ID_NUMOF_BROADCAST_REQUEST 115
#define ASN1_S1AP_ID_WARNING_TYPE 116
#define ASN1_S1AP_ID_WARNING_SECURITY_INFO 117
#define ASN1_S1AP_ID_DATA_CODING_SCHEME 118
#define ASN1_S1AP_ID_WARNING_MSG_CONTENTS 119
#define ASN1_S1AP_ID_BROADCAST_COMPLETED_AREA_LIST 120
#define ASN1_S1AP_ID_INTER_SYS_INFO_TRANSFER_TYPE_EDT 121
#define ASN1_S1AP_ID_INTER_SYS_INFO_TRANSFER_TYPE_MDT 122
#define ASN1_S1AP_ID_TARGET_TO_SOURCE_TRANSPARENT_CONTAINER 123
#define ASN1_S1AP_ID_SRVCC_OPERATION_POSSIBLE 124
#define ASN1_S1AP_ID_SRVCCHO_IND 125
#define ASN1_S1AP_ID_NAS_DL_COUNT 126
#define ASN1_S1AP_ID_CSG_ID 127
#define ASN1_S1AP_ID_CSG_ID_LIST 128
#define ASN1_S1AP_ID_SON_CFG_TRANSFER_ECT 129
#define ASN1_S1AP_ID_SON_CFG_TRANSFER_MCT 130
#define ASN1_S1AP_ID_TRACE_COLLECTION_ENTITY_IP_ADDRESS 131
#define ASN1_S1AP_ID_MS_CLASSMARK2 132
#define ASN1_S1AP_ID_MS_CLASSMARK3 133
#define ASN1_S1AP_ID_RRC_ESTABLISHMENT_CAUSE 134
#define ASN1_S1AP_ID_NAS_SECURITY_PARAMSFROM_E_UTRAN 135
#define ASN1_S1AP_ID_NAS_SECURITY_PARAMSTO_E_UTRAN 136
#define ASN1_S1AP_ID_DEFAULT_PAGING_DRX 137
#define ASN1_S1AP_ID_SOURCE_TO_TARGET_TRANSPARENT_CONTAINER_SECONDARY 138
#define ASN1_S1AP_ID_TARGET_TO_SOURCE_TRANSPARENT_CONTAINER_SECONDARY 139
#define ASN1_S1AP_ID_EUTRAN_ROUND_TRIP_DELAY_ESTIMATION_INFO 140
#define ASN1_S1AP_ID_BROADCAST_CANCELLED_AREA_LIST 141
#define ASN1_S1AP_ID_CONCURRENT_WARNING_MSG_IND 142
#define ASN1_S1AP_ID_DATA_FORWARDING_NOT_POSSIBLE 143
#define ASN1_S1AP_ID_EXTENDED_REPEAT_PERIOD 144
#define ASN1_S1AP_ID_CELL_ACCESS_MODE 145
#define ASN1_S1AP_ID_CSG_MEMBERSHIP_STATUS 146
#define ASN1_S1AP_ID_LP_PA_PDU 147
#define ASN1_S1AP_ID_ROUTING_ID 148
#define ASN1_S1AP_ID_TIME_SYNCHRONISATION_INFO 149
#define ASN1_S1AP_ID_PS_SERVICE_NOT_AVAILABLE 150
#define ASN1_S1AP_ID_PAGING_PRIO 151
#define ASN1_S1AP_ID_X2_TNL_CFG_INFO 152
#define ASN1_S1AP_ID_ENBX2_EXTENDED_TRANSPORT_LAYER_ADDRESSES 153
#define ASN1_S1AP_ID_GUMMEI_LIST 154
#define ASN1_S1AP_ID_GW_TRANSPORT_LAYER_ADDRESS 155
#define ASN1_S1AP_ID_CORRELATION_ID 156
#define ASN1_S1AP_ID_SOURCE_MME_GUMMEI 157
#define ASN1_S1AP_ID_MME_UE_S1AP_ID_MINUS2 158
#define ASN1_S1AP_ID_REGISTERED_LAI 159
#define ASN1_S1AP_ID_RELAY_NODE_IND 160
#define ASN1_S1AP_ID_TRAFFIC_LOAD_REDUCTION_IND 161
#define ASN1_S1AP_ID_MDT_CFG 162
#define ASN1_S1AP_ID_MME_RELAY_SUPPORT_IND 163
#define ASN1_S1AP_ID_GW_CONTEXT_RELEASE_IND 164
#define ASN1_S1AP_ID_MANAGEMENT_BASED_MDT_ALLOWED 165
#define ASN1_S1AP_ID_PRIVACY_IND 166
#define ASN1_S1AP_ID_TIME_UE_STAYED_IN_CELL_ENHANCED_GRANULARITY 167
#define ASN1_S1AP_ID_HO_CAUSE 168
#define ASN1_S1AP_ID_VOICE_SUPPORT_MATCH_IND 169
#define ASN1_S1AP_ID_GUMMEI_TYPE 170
#define ASN1_S1AP_ID_M3_CFG 171
#define ASN1_S1AP_ID_M4_CFG 172
#define ASN1_S1AP_ID_M5_CFG 173
#define ASN1_S1AP_ID_MDT_LOCATION_INFO 174
#define ASN1_S1AP_ID_MOB_INFO 175
#define ASN1_S1AP_ID_TUNNEL_INFO_FOR_BBF 176
#define ASN1_S1AP_ID_MANAGEMENT_BASED_MDTPLMN_LIST 177
#define ASN1_S1AP_ID_SIG_BASED_MDTPLMN_LIST 178
#define ASN1_S1AP_ID_ULCOUNT_VALUE_EXTENDED 179
#define ASN1_S1AP_ID_DLCOUNT_VALUE_EXTENDED 180
#define ASN1_S1AP_ID_RECEIVE_STATUS_OF_ULPDCPSDUS_EXTENDED 181
#define ASN1_S1AP_ID_ECGI_LIST_FOR_RESTART 182
#define ASN1_S1AP_ID_SIPTO_CORRELATION_ID 183
#define ASN1_S1AP_ID_SIPTO_L_GW_TRANSPORT_LAYER_ADDRESS 184
#define ASN1_S1AP_ID_TRANSPORT_INFO 185
#define ASN1_S1AP_ID_LHN_ID 186
#define ASN1_S1AP_ID_ADD_CS_FALLBACK_IND 187
#define ASN1_S1AP_ID_TAI_LIST_FOR_RESTART 188
#define ASN1_S1AP_ID_USER_LOCATION_INFO 189
#define ASN1_S1AP_ID_EMERGENCY_AREA_ID_LIST_FOR_RESTART 190
#define ASN1_S1AP_ID_KILL_ALL_WARNING_MSGS 191
#define ASN1_S1AP_ID_MASKED_IMEISV 192
#define ASN1_S1AP_ID_ENB_INDIRECT_X2_TRANSPORT_LAYER_ADDRESSES 193
#define ASN1_S1AP_ID_U_E_HISTORY_INFO_FROM_THE_UE 194
#define ASN1_S1AP_ID_PRO_SE_AUTHORIZED 195
#define ASN1_S1AP_ID_EXPECTED_UE_BEHAVIOUR 196
#define ASN1_S1AP_ID_LOGGED_MBSFNMDT 197
#define ASN1_S1AP_ID_UE_RADIO_CAP_FOR_PAGING 198
#define ASN1_S1AP_ID_ERAB_TO_BE_MODIFIED_LIST_BEARER_MOD_IND 199
#define ASN1_S1AP_ID_ERAB_TO_BE_MODIFIED_ITEM_BEARER_MOD_IND 200
#define ASN1_S1AP_ID_ERAB_NOT_TO_BE_MODIFIED_LIST_BEARER_MOD_IND 201
#define ASN1_S1AP_ID_ERAB_NOT_TO_BE_MODIFIED_ITEM_BEARER_MOD_IND 202
#define ASN1_S1AP_ID_ERAB_MODIFY_LIST_BEARER_MOD_CONF 203
#define ASN1_S1AP_ID_ERAB_MODIFY_ITEM_BEARER_MOD_CONF 204
#define ASN1_S1AP_ID_ERAB_FAILED_TO_MODIFY_LIST_BEARER_MOD_CONF 205
#define ASN1_S1AP_ID_SON_INFO_REPORT 206
#define ASN1_S1AP_ID_MUTING_AVAILABILITY_IND 207
#define ASN1_S1AP_ID_MUTING_PATTERN_INFO 208
#define ASN1_S1AP_ID_SYNCHRONISATION_INFO 209
#define ASN1_S1AP_ID_ERAB_TO_BE_RELEASED_LIST_BEARER_MOD_CONF 210
#define ASN1_S1AP_ID_ASSIST_DATA_FOR_PAGING 211
#define ASN1_S1AP_ID_CELL_ID_AND_CE_LEVEL_FOR_CE_CAPABLE_UES 212
#define ASN1_S1AP_ID_INFO_ON_RECOMMENDED_CELLS_AND_ENBS_FOR_PAGING 213
#define ASN1_S1AP_ID_RECOMMENDED_CELL_ITEM 214
#define ASN1_S1AP_ID_RECOMMENDED_ENB_ITEM 215
#define ASN1_S1AP_ID_PRO_SE_UETO_NETWORK_RELAYING 216
#define ASN1_S1AP_ID_ULCOUNT_VALUE_PDCP_SNLEN18 217
#define ASN1_S1AP_ID_DLCOUNT_VALUE_PDCP_SNLEN18 218
#define ASN1_S1AP_ID_RECEIVE_STATUS_OF_ULPDCPSDUS_PDCP_SNLEN18 219
#define ASN1_S1AP_ID_M6_CFG 220
#define ASN1_S1AP_ID_M7_CFG 221
#define ASN1_S1AP_ID_PW_SFAILED_ECGI_LIST 222
#define ASN1_S1AP_ID_MME_GROUP_ID 223
#define ASN1_S1AP_ID_ADD_GUTI 224
#define ASN1_S1AP_ID_S1_MSG 225
#define ASN1_S1AP_ID_CSG_MEMBERSHIP_INFO 226
#define ASN1_S1AP_ID_PAGING_E_DRX_INFO 227
#define ASN1_S1AP_ID_UE_RETENTION_INFO 228
#define ASN1_S1AP_ID_UE_USAGE_TYPE 230
#define ASN1_S1AP_ID_EXTENDED_UE_ID_IDX_VALUE 231
#define ASN1_S1AP_ID_RAT_TYPE 232
#define ASN1_S1AP_ID_BEARER_TYPE 233
#define ASN1_S1AP_ID_NB_IO_T_DEFAULT_PAGING_DRX 234
#define ASN1_S1AP_ID_ERAB_FAILED_TO_RESUME_LIST_RESUME_REQ 235
#define ASN1_S1AP_ID_ERAB_FAILED_TO_RESUME_ITEM_RESUME_REQ 236
#define ASN1_S1AP_ID_ERAB_FAILED_TO_RESUME_LIST_RESUME_RES 237
#define ASN1_S1AP_ID_ERAB_FAILED_TO_RESUME_ITEM_RESUME_RES 238
#define ASN1_S1AP_ID_NB_IO_T_PAGING_E_DRX_INFO 239
#define ASN1_S1AP_ID_V2XSERVICES_AUTHORIZED 240
#define ASN1_S1AP_ID_UEUSER_PLANE_CIO_TSUPPORT_IND 241
#define ASN1_S1AP_ID_CE_MODE_B_SUPPORT_IND 242
#define ASN1_S1AP_ID_SRVCC_OPERATION_NOT_POSSIBLE 243
#define ASN1_S1AP_ID_NB_IO_T_UE_ID_IDX_VALUE 244
#define ASN1_S1AP_ID_RRC_RESUME_CAUSE 245
#define ASN1_S1AP_ID_DCN_ID 246
#define ASN1_S1AP_ID_SERVED_DCNS 247
#define ASN1_S1AP_ID_UE_SIDELINK_AGGREGATE_MAXIMUM_BITRATE 248
#define ASN1_S1AP_ID_DLNASPDU_DELIVERY_ACK_REQUEST 249
#define ASN1_S1AP_ID_COVERAGE_LEVEL 250
#define ASN1_S1AP_ID_ENHANCED_COVERAGE_RESTRICTED 251
#define ASN1_S1AP_ID_UE_LEVEL_QOS_PARAMS 252
#define ASN1_S1AP_ID_DL_CP_SECURITY_INFO 253
#define ASN1_S1AP_ID_UL_CP_SECURITY_INFO 254
#define ASN1_S1AP_ID_CE_MODE_BRESTRICTED 271
#define ASN1_S1AP_ID_PENDING_DATA_IND 283
#define ASN1_S1AP_MAXNOOF_IRAT_REPORT_CELLS 128
#define ASN1_S1AP_MAXNOOFCANDIDATE_CELLS 16
#define ASN1_S1AP_MAXNOOF_CELLINENB 256

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
  SRSASN_CODE unpack(cbit_ref& bref);
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

// PrivateIE-Field{S1AP-PRIVATE-IES : IEsSetParam} ::= SEQUENCE{{S1AP-PRIVATE-IES}}
template <class ies_set_paramT_>
struct private_ie_field_s {
  private_ie_id_c                   id;
  crit_e                            crit;
  typename ies_set_paramT_::value_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PrivateIE-Container{S1AP-PRIVATE-IES : IEsSetParam} ::= SEQUENCE (SIZE (1..65535)) OF PrivateIE-Field
template <class ies_set_paramT_>
using private_ie_container_l = dyn_seq_of<private_ie_field_s<ies_set_paramT_>, 1, 65535, true>;

// ProtocolExtensionField{S1AP-PROTOCOL-EXTENSION : ExtensionSetParam} ::= SEQUENCE{{S1AP-PROTOCOL-EXTENSION}}
template <class ext_set_paramT_>
struct protocol_ext_field_s {
  uint32_t                        id = 0;
  crit_e                          crit;
  typename ext_set_paramT_::ext_c ext_value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint32_t& id_);
};

// ProtocolExtensionContainer{S1AP-PROTOCOL-EXTENSION : ExtensionSetParam} ::= SEQUENCE (SIZE (1..65535)) OF
// ProtocolExtensionField
template <class ext_set_paramT_>
using protocol_ext_container_l = dyn_seq_of<protocol_ext_field_s<ext_set_paramT_>, 1, 65535, true>;

// ProtocolIE-Field{S1AP-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE{{S1AP-PROTOCOL-IES}}
template <class ies_set_paramT_>
struct protocol_ie_field_s {
  uint32_t                          id = 0;
  crit_e                            crit;
  typename ies_set_paramT_::value_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint32_t& id_);
};

// ProtocolIE-Container{S1AP-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE (SIZE (0..65535)) OF ProtocolIE-Field
template <class ies_set_paramT_>
using protocol_ie_container_l = dyn_seq_of<protocol_ie_field_s<ies_set_paramT_>, 0, 65535, true>;

// ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE{{S1AP-PROTOCOL-IES}}
template <class ies_set_paramT_>
struct protocol_ie_single_container_s {
  uint32_t                          id = 0;
  crit_e                            crit;
  typename ies_set_paramT_::value_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint32_t& id_);
};

// ProtocolIE-FieldPair{S1AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{S1AP-PROTOCOL-IES-PAIR}}
template <class ies_set_paramT_>
struct protocol_ie_field_pair_s {
  uint32_t                                 id = 0;
  crit_e                                   first_crit;
  typename ies_set_paramT_::first_value_c  first_value;
  crit_e                                   second_crit;
  typename ies_set_paramT_::second_value_c second_value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint32_t& id_);
};

// ProtocolIE-ContainerPair{S1AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE (SIZE (0..65535)) OF ProtocolIE-FieldPair
template <class ies_set_paramT_>
using protocol_ie_container_pair_l = dyn_seq_of<protocol_ie_field_pair_s<ies_set_paramT_>, 0, 65535, true>;

// ActivatedCellsList-Item ::= SEQUENCE
struct activ_cells_list_item_s {
  bool                      ext = false;
  unbounded_octstring<true> cell_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ActivatedCellsList ::= SEQUENCE (SIZE (0..256)) OF ActivatedCellsList-Item
using activ_cells_list_l = dyn_array<activ_cells_list_item_s>;

struct s1ap_protocol_ext_empty_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};
// GUMMEI-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o gummei_ext_ies_o;

// Additional-GUTI-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o add_guti_ext_ies_o;

template <class extT_>
struct protocol_ext_container_item_s {
  uint32_t id = 0;
  crit_e   crit;
  extT_    ext;

  // sequence methods
  protocol_ext_container_item_s(uint32_t id_, crit_e crit_);
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct protocol_ext_container_empty_l {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};
typedef protocol_ext_container_empty_l gummei_ext_ies_container;

// GUMMEI ::= SEQUENCE
struct gummei_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plmn_id;
  fixed_octstring<2, true> mme_group_id;
  fixed_octstring<1, true> mme_code;
  gummei_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l add_guti_ext_ies_container;

// Additional-GUTI ::= SEQUENCE
struct add_guti_s {
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  gummei_s                   gummei;
  fixed_octstring<4, true>   m_tmsi;
  add_guti_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AllocationAndRetentionPriority-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o alloc_and_retention_prio_ext_ies_o;

// Pre-emptionCapability ::= ENUMERATED
struct pre_emption_cap_opts {
  enum options { shall_not_trigger_pre_emption, may_trigger_pre_emption, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<pre_emption_cap_opts> pre_emption_cap_e;

// Pre-emptionVulnerability ::= ENUMERATED
struct pre_emption_vulnerability_opts {
  enum options { not_pre_emptable, pre_emptable, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<pre_emption_vulnerability_opts> pre_emption_vulnerability_e;

typedef protocol_ext_container_empty_l alloc_and_retention_prio_ext_ies_container;

// AllocationAndRetentionPriority ::= SEQUENCE
struct alloc_and_retention_prio_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  uint8_t                                    prio_level      = 0;
  pre_emption_cap_e                          pre_emption_cap;
  pre_emption_vulnerability_e                pre_emption_vulnerability;
  alloc_and_retention_prio_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRAN-CGI-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o eutran_cgi_ext_ies_o;

// TAI-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o tai_ext_ies_o;

typedef protocol_ext_container_empty_l eutran_cgi_ext_ies_container;

// EUTRAN-CGI ::= SEQUENCE
struct eutran_cgi_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plm_nid;
  fixed_bitstring<28, false, true> cell_id;
  eutran_cgi_ext_ies_container     ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l tai_ext_ies_container;

// TAI ::= SEQUENCE
struct tai_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plm_nid;
  fixed_octstring<2, true> tac;
  tai_ext_ies_container    ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellBasedMDT-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o cell_based_mdt_ext_ies_o;

// CellIdListforMDT ::= SEQUENCE (SIZE (1..32)) OF EUTRAN-CGI
using cell_id_listfor_mdt_l = dyn_array<eutran_cgi_s>;

// TABasedMDT-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o ta_based_mdt_ext_ies_o;

// TAIBasedMDT-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o tai_based_mdt_ext_ies_o;

// TAIListforMDT ::= SEQUENCE (SIZE (1..8)) OF TAI
using tai_listfor_mdt_l = dyn_array<tai_s>;

// TAListforMDT ::= SEQUENCE (SIZE (1..8)) OF OCTET STRING (SIZE (2))
using ta_listfor_mdt_l = bounded_array<fixed_octstring<2, true>, 8>;

typedef protocol_ext_container_empty_l cell_based_mdt_ext_ies_container;

// CellBasedMDT ::= SEQUENCE
struct cell_based_mdt_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  cell_id_listfor_mdt_l            cell_id_listfor_mdt;
  cell_based_mdt_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l ta_based_mdt_ext_ies_container;

// TABasedMDT ::= SEQUENCE
struct ta_based_mdt_s {
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  ta_listfor_mdt_l               talistfor_mdt;
  ta_based_mdt_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l tai_based_mdt_ext_ies_container;

// TAIBasedMDT ::= SEQUENCE
struct tai_based_mdt_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  tai_listfor_mdt_l               tai_listfor_mdt;
  tai_based_mdt_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaScopeOfMDT ::= CHOICE
struct area_scope_of_mdt_c {
  struct types_opts {
    enum options { cell_based, tabased, plmn_wide, /*...*/ tai_based, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true, 1> types;

  // choice methods
  area_scope_of_mdt_c() = default;
  area_scope_of_mdt_c(const area_scope_of_mdt_c& other);
  area_scope_of_mdt_c& operator=(const area_scope_of_mdt_c& other);
  ~area_scope_of_mdt_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_based_mdt_s& cell_based()
  {
    assert_choice_type("cellBased", type_.to_string(), "AreaScopeOfMDT");
    return c.get<cell_based_mdt_s>();
  }
  ta_based_mdt_s& tabased()
  {
    assert_choice_type("tABased", type_.to_string(), "AreaScopeOfMDT");
    return c.get<ta_based_mdt_s>();
  }
  tai_based_mdt_s& tai_based()
  {
    assert_choice_type("tAIBased", type_.to_string(), "AreaScopeOfMDT");
    return c.get<tai_based_mdt_s>();
  }
  const cell_based_mdt_s& cell_based() const
  {
    assert_choice_type("cellBased", type_.to_string(), "AreaScopeOfMDT");
    return c.get<cell_based_mdt_s>();
  }
  const ta_based_mdt_s& tabased() const
  {
    assert_choice_type("tABased", type_.to_string(), "AreaScopeOfMDT");
    return c.get<ta_based_mdt_s>();
  }
  const tai_based_mdt_s& tai_based() const
  {
    assert_choice_type("tAIBased", type_.to_string(), "AreaScopeOfMDT");
    return c.get<tai_based_mdt_s>();
  }
  cell_based_mdt_s& set_cell_based()
  {
    set(types::cell_based);
    return c.get<cell_based_mdt_s>();
  }
  ta_based_mdt_s& set_tabased()
  {
    set(types::tabased);
    return c.get<ta_based_mdt_s>();
  }
  tai_based_mdt_s& set_tai_based()
  {
    set(types::tai_based);
    return c.get<tai_based_mdt_s>();
  }

private:
  types                                                              type_;
  choice_buffer_t<cell_based_mdt_s, ta_based_mdt_s, tai_based_mdt_s> c;

  void destroy_();
};

// CellIdentifierAndCELevelForCECapableUEs-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o cell_id_and_ce_level_for_ce_capable_ues_ext_ies_o;

typedef protocol_ext_container_empty_l cell_id_and_ce_level_for_ce_capable_ues_ext_ies_container;

// CellIdentifierAndCELevelForCECapableUEs ::= SEQUENCE
struct cell_id_and_ce_level_for_ce_capable_ues_s {
  bool                                                      ext             = false;
  bool                                                      ie_exts_present = false;
  eutran_cgi_s                                              global_cell_id;
  unbounded_octstring<true>                                 celevel;
  cell_id_and_ce_level_for_ce_capable_ues_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InformationForCECapableUEs-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o info_for_ce_capable_ues_ext_ies_o;

typedef protocol_ext_container_empty_l info_for_ce_capable_ues_ext_ies_container;

// AssistanceDataForCECapableUEs ::= SEQUENCE
struct assist_data_for_ce_capable_ues_s {
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  cell_id_and_ce_level_for_ce_capable_ues_s cell_id_and_ce_level_for_ce_capable_ues;
  info_for_ce_capable_ues_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RecommendedCellsForPagingItem-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o recommended_cells_for_paging_item_ext_ies_o;

typedef protocol_ext_container_empty_l recommended_cells_for_paging_item_ext_ies_container;

// RecommendedCellItem ::= SEQUENCE
struct recommended_cell_item_s {
  bool                                                ext                         = false;
  bool                                                time_stayed_in_cell_present = false;
  bool                                                ie_exts_present             = false;
  eutran_cgi_s                                        eutran_cgi;
  uint16_t                                            time_stayed_in_cell = 0;
  recommended_cells_for_paging_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RecommendedCellItemIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct recommended_cell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { recommended_cell_item, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::recommended_cell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    recommended_cell_item_s&       recommended_cell_item() { return c; }
    const recommended_cell_item_s& recommended_cell_item() const { return c; }

  private:
    recommended_cell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RecommendedCellList ::= SEQUENCE (SIZE (1..16)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES : IEsSetParam}
using recommended_cell_list_l = bounded_array<protocol_ie_single_container_s<recommended_cell_item_ies_o>, 16>;

// RecommendedCellsForPaging-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o recommended_cells_for_paging_ext_ies_o;

// AssistanceDataForRecommendedCells-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o assist_data_for_recommended_cells_ext_ies_o;

// NextPagingAreaScope ::= ENUMERATED
struct next_paging_area_scope_opts {
  enum options { same, changed, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<next_paging_area_scope_opts, true> next_paging_area_scope_e;

// PagingAttemptInformation-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o paging_attempt_info_ext_ies_o;

typedef protocol_ext_container_empty_l recommended_cells_for_paging_ext_ies_container;

// RecommendedCellsForPaging ::= SEQUENCE
struct recommended_cells_for_paging_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  recommended_cell_list_l                        recommended_cell_list;
  recommended_cells_for_paging_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssistanceDataForPaging-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o assist_data_for_paging_ext_ies_o;

typedef protocol_ext_container_empty_l assist_data_for_recommended_cells_ext_ies_container;

// AssistanceDataForRecommendedCells ::= SEQUENCE
struct assist_data_for_recommended_cells_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  recommended_cells_for_paging_s                      recommended_cells_for_paging;
  assist_data_for_recommended_cells_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l paging_attempt_info_ext_ies_container;

// PagingAttemptInformation ::= SEQUENCE
struct paging_attempt_info_s {
  bool                                  ext                            = false;
  bool                                  next_paging_area_scope_present = false;
  bool                                  ie_exts_present                = false;
  uint8_t                               paging_attempt_count           = 1;
  uint8_t                               intended_nof_paging_attempts   = 1;
  next_paging_area_scope_e              next_paging_area_scope;
  paging_attempt_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l assist_data_for_paging_ext_ies_container;

// AssistanceDataForPaging ::= SEQUENCE
struct assist_data_for_paging_s {
  bool                                     ext                                       = false;
  bool                                     assist_data_for_recommended_cells_present = false;
  bool                                     assist_data_for_ce_capable_ues_present    = false;
  bool                                     paging_attempt_info_present               = false;
  bool                                     ie_exts_present                           = false;
  assist_data_for_recommended_cells_s      assist_data_for_recommended_cells;
  assist_data_for_ce_capable_ues_s         assist_data_for_ce_capable_ues;
  paging_attempt_info_s                    paging_attempt_info;
  assist_data_for_paging_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BPLMNs ::= SEQUENCE (SIZE (1..6)) OF OCTET STRING (SIZE (3))
using bplmns_l = bounded_array<fixed_octstring<3, true>, 6>;

// COUNTValueExtended-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o count_value_extended_ext_ies_o;

// COUNTvaluePDCP-SNlength18-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o coun_tvalue_pdcp_snlen18_ext_ies_o;

typedef protocol_ext_container_empty_l count_value_extended_ext_ies_container;

// COUNTValueExtended ::= SEQUENCE
struct count_value_extended_s {
  bool                                   ext              = false;
  bool                                   ie_exts_present  = false;
  uint16_t                               pdcp_sn_extended = 0;
  uint32_t                               hfn_modified     = 0;
  count_value_extended_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// COUNTvalue-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o coun_tvalue_ext_ies_o;

typedef protocol_ext_container_empty_l coun_tvalue_pdcp_snlen18_ext_ies_container;

// COUNTvaluePDCP-SNlength18 ::= SEQUENCE
struct coun_tvalue_pdcp_snlen18_s {
  bool                                       ext                 = false;
  bool                                       ie_exts_present     = false;
  uint32_t                                   pdcp_snlen18        = 0;
  uint16_t                                   hfnfor_pdcp_snlen18 = 0;
  coun_tvalue_pdcp_snlen18_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Bearers-SubjectToStatusTransfer-ItemExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct bearers_subject_to_status_transfer_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        ulcount_value_extended,
        dlcount_value_extended,
        receive_status_of_ulpdcpsdus_extended,
        ulcount_value_pdcp_snlen18,
        dlcount_value_pdcp_snlen18,
        receive_status_of_ulpdcpsdus_pdcp_snlen18,
        nulltype
      } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    count_value_extended_s&                          ulcount_value_extended();
    count_value_extended_s&                          dlcount_value_extended();
    bounded_bitstring<1, 16384, false, true>&        receive_status_of_ulpdcpsdus_extended();
    coun_tvalue_pdcp_snlen18_s&                      ulcount_value_pdcp_snlen18();
    coun_tvalue_pdcp_snlen18_s&                      dlcount_value_pdcp_snlen18();
    bounded_bitstring<1, 131072, false, true>&       receive_status_of_ulpdcpsdus_pdcp_snlen18();
    const count_value_extended_s&                    ulcount_value_extended() const;
    const count_value_extended_s&                    dlcount_value_extended() const;
    const bounded_bitstring<1, 16384, false, true>&  receive_status_of_ulpdcpsdus_extended() const;
    const coun_tvalue_pdcp_snlen18_s&                ulcount_value_pdcp_snlen18() const;
    const coun_tvalue_pdcp_snlen18_s&                dlcount_value_pdcp_snlen18() const;
    const bounded_bitstring<1, 131072, false, true>& receive_status_of_ulpdcpsdus_pdcp_snlen18() const;

  private:
    types type_;
    choice_buffer_t<bounded_bitstring<1, 131072, false, true>,
                    bounded_bitstring<1, 16384, false, true>,
                    coun_tvalue_pdcp_snlen18_s,
                    count_value_extended_s>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

typedef protocol_ext_container_empty_l coun_tvalue_ext_ies_container;

// COUNTvalue ::= SEQUENCE
struct coun_tvalue_s {
  bool                          ext             = false;
  bool                          ie_exts_present = false;
  uint16_t                      pdcp_sn         = 0;
  uint32_t                      hfn             = 0;
  coun_tvalue_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct bearers_subject_to_status_transfer_item_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                                   ulcount_value_extended_present                    = false;
  bool                                                   dlcount_value_extended_present                    = false;
  bool                                                   receive_status_of_ulpdcpsdus_extended_present     = false;
  bool                                                   ulcount_value_pdcp_snlen18_present                = false;
  bool                                                   dlcount_value_pdcp_snlen18_present                = false;
  bool                                                   receive_status_of_ulpdcpsdus_pdcp_snlen18_present = false;
  ie_field_s<count_value_extended_s>                     ulcount_value_extended;
  ie_field_s<count_value_extended_s>                     dlcount_value_extended;
  ie_field_s<bounded_bitstring<1, 16384, false, true> >  receive_status_of_ulpdcpsdus_extended;
  ie_field_s<coun_tvalue_pdcp_snlen18_s>                 ulcount_value_pdcp_snlen18;
  ie_field_s<coun_tvalue_pdcp_snlen18_s>                 dlcount_value_pdcp_snlen18;
  ie_field_s<bounded_bitstring<1, 131072, false, true> > receive_status_of_ulpdcpsdus_pdcp_snlen18;

  // sequence methods
  bearers_subject_to_status_transfer_item_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Bearers-SubjectToStatusTransfer-Item ::= SEQUENCE
struct bearers_subject_to_status_transfer_item_s {
  bool                                                      ext                                 = false;
  bool                                                      receive_statusof_ulpdcpsdus_present = false;
  bool                                                      ie_exts_present                     = false;
  uint8_t                                                   erab_id                             = 0;
  coun_tvalue_s                                             ul_coun_tvalue;
  coun_tvalue_s                                             dl_coun_tvalue;
  fixed_bitstring<4096, false, true>                        receive_statusof_ulpdcpsdus;
  bearers_subject_to_status_transfer_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Bearers-SubjectToStatusTransfer-ItemIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct bearers_subject_to_status_transfer_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bearers_subject_to_status_transfer_item, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bearers_subject_to_status_transfer_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bearers_subject_to_status_transfer_item_s&       bearers_subject_to_status_transfer_item() { return c; }
    const bearers_subject_to_status_transfer_item_s& bearers_subject_to_status_transfer_item() const { return c; }

  private:
    bearers_subject_to_status_transfer_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Bearers-SubjectToStatusTransferList ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES :
// IEsSetParam}
using bearers_subject_to_status_transfer_list_l =
    dyn_array<protocol_ie_single_container_s<bearers_subject_to_status_transfer_item_ies_o> >;

// CancelledCellinEAI-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o cancelled_cellin_eai_item_ext_ies_o;

// CancelledCellinTAI-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o cancelled_cellin_tai_item_ext_ies_o;

typedef protocol_ext_container_empty_l cancelled_cellin_eai_item_ext_ies_container;

// CancelledCellinEAI-Item ::= SEQUENCE
struct cancelled_cellin_eai_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  eutran_cgi_s                                ecgi;
  uint32_t                                    nof_broadcasts = 0;
  cancelled_cellin_eai_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l cancelled_cellin_tai_item_ext_ies_container;

// CancelledCellinTAI-Item ::= SEQUENCE
struct cancelled_cellin_tai_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  eutran_cgi_s                                ecgi;
  uint32_t                                    nof_broadcasts = 0;
  cancelled_cellin_tai_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CancelledCellinEAI ::= SEQUENCE (SIZE (1..65535)) OF CancelledCellinEAI-Item
using cancelled_cellin_eai_l = dyn_array<cancelled_cellin_eai_item_s>;

// CancelledCellinTAI ::= SEQUENCE (SIZE (1..65535)) OF CancelledCellinTAI-Item
using cancelled_cellin_tai_l = dyn_array<cancelled_cellin_tai_item_s>;

// CellID-Cancelled-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o cell_id_cancelled_item_ext_ies_o;

// EmergencyAreaID-Cancelled-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o emergency_area_id_cancelled_item_ext_ies_o;

// TAI-Cancelled-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o tai_cancelled_item_ext_ies_o;

typedef protocol_ext_container_empty_l cell_id_cancelled_item_ext_ies_container;

// CellID-Cancelled-Item ::= SEQUENCE
struct cell_id_cancelled_item_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  eutran_cgi_s                             ecgi;
  uint32_t                                 nof_broadcasts = 0;
  cell_id_cancelled_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l emergency_area_id_cancelled_item_ext_ies_container;

// EmergencyAreaID-Cancelled-Item ::= SEQUENCE
struct emergency_area_id_cancelled_item_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  fixed_octstring<3, true>                           emergency_area_id;
  cancelled_cellin_eai_l                             cancelled_cellin_eai;
  emergency_area_id_cancelled_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l tai_cancelled_item_ext_ies_container;

// TAI-Cancelled-Item ::= SEQUENCE
struct tai_cancelled_item_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  tai_s                                tai;
  cancelled_cellin_tai_l               cancelled_cellin_tai;
  tai_cancelled_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellID-Cancelled ::= SEQUENCE (SIZE (1..65535)) OF CellID-Cancelled-Item
using cell_id_cancelled_l = dyn_array<cell_id_cancelled_item_s>;

// EmergencyAreaID-Cancelled ::= SEQUENCE (SIZE (1..65535)) OF EmergencyAreaID-Cancelled-Item
using emergency_area_id_cancelled_l = dyn_array<emergency_area_id_cancelled_item_s>;

// TAI-Cancelled ::= SEQUENCE (SIZE (1..65535)) OF TAI-Cancelled-Item
using tai_cancelled_l = dyn_array<tai_cancelled_item_s>;

// BroadcastCancelledAreaList ::= CHOICE
struct broadcast_cancelled_area_list_c {
  struct types_opts {
    enum options { cell_id_cancelled, tai_cancelled, emergency_area_id_cancelled, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  broadcast_cancelled_area_list_c() = default;
  broadcast_cancelled_area_list_c(const broadcast_cancelled_area_list_c& other);
  broadcast_cancelled_area_list_c& operator=(const broadcast_cancelled_area_list_c& other);
  ~broadcast_cancelled_area_list_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_id_cancelled_l& cell_id_cancelled()
  {
    assert_choice_type("cellID-Cancelled", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<cell_id_cancelled_l>();
  }
  tai_cancelled_l& tai_cancelled()
  {
    assert_choice_type("tAI-Cancelled", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<tai_cancelled_l>();
  }
  emergency_area_id_cancelled_l& emergency_area_id_cancelled()
  {
    assert_choice_type("emergencyAreaID-Cancelled", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<emergency_area_id_cancelled_l>();
  }
  const cell_id_cancelled_l& cell_id_cancelled() const
  {
    assert_choice_type("cellID-Cancelled", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<cell_id_cancelled_l>();
  }
  const tai_cancelled_l& tai_cancelled() const
  {
    assert_choice_type("tAI-Cancelled", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<tai_cancelled_l>();
  }
  const emergency_area_id_cancelled_l& emergency_area_id_cancelled() const
  {
    assert_choice_type("emergencyAreaID-Cancelled", type_.to_string(), "BroadcastCancelledAreaList");
    return c.get<emergency_area_id_cancelled_l>();
  }
  cell_id_cancelled_l& set_cell_id_cancelled()
  {
    set(types::cell_id_cancelled);
    return c.get<cell_id_cancelled_l>();
  }
  tai_cancelled_l& set_tai_cancelled()
  {
    set(types::tai_cancelled);
    return c.get<tai_cancelled_l>();
  }
  emergency_area_id_cancelled_l& set_emergency_area_id_cancelled()
  {
    set(types::emergency_area_id_cancelled);
    return c.get<emergency_area_id_cancelled_l>();
  }

private:
  types                                                                                type_;
  choice_buffer_t<cell_id_cancelled_l, emergency_area_id_cancelled_l, tai_cancelled_l> c;

  void destroy_();
};

// CompletedCellinEAI-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o completed_cellin_eai_item_ext_ies_o;

// CompletedCellinTAI-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o completed_cellin_tai_item_ext_ies_o;

typedef protocol_ext_container_empty_l completed_cellin_eai_item_ext_ies_container;

// CompletedCellinEAI-Item ::= SEQUENCE
struct completed_cellin_eai_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  eutran_cgi_s                                ecgi;
  completed_cellin_eai_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l completed_cellin_tai_item_ext_ies_container;

// CompletedCellinTAI-Item ::= SEQUENCE
struct completed_cellin_tai_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  eutran_cgi_s                                ecgi;
  completed_cellin_tai_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellID-Broadcast-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o cell_id_broadcast_item_ext_ies_o;

// CompletedCellinEAI ::= SEQUENCE (SIZE (1..65535)) OF CompletedCellinEAI-Item
using completed_cellin_eai_l = dyn_array<completed_cellin_eai_item_s>;

// CompletedCellinTAI ::= SEQUENCE (SIZE (1..65535)) OF CompletedCellinTAI-Item
using completed_cellin_tai_l = dyn_array<completed_cellin_tai_item_s>;

// EmergencyAreaID-Broadcast-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o emergency_area_id_broadcast_item_ext_ies_o;

// TAI-Broadcast-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o tai_broadcast_item_ext_ies_o;

typedef protocol_ext_container_empty_l cell_id_broadcast_item_ext_ies_container;

// CellID-Broadcast-Item ::= SEQUENCE
struct cell_id_broadcast_item_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  eutran_cgi_s                             ecgi;
  cell_id_broadcast_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l emergency_area_id_broadcast_item_ext_ies_container;

// EmergencyAreaID-Broadcast-Item ::= SEQUENCE
struct emergency_area_id_broadcast_item_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  fixed_octstring<3, true>                           emergency_area_id;
  completed_cellin_eai_l                             completed_cellin_eai;
  emergency_area_id_broadcast_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l tai_broadcast_item_ext_ies_container;

// TAI-Broadcast-Item ::= SEQUENCE
struct tai_broadcast_item_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  tai_s                                tai;
  completed_cellin_tai_l               completed_cellin_tai;
  tai_broadcast_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellID-Broadcast ::= SEQUENCE (SIZE (1..65535)) OF CellID-Broadcast-Item
using cell_id_broadcast_l = dyn_array<cell_id_broadcast_item_s>;

// EmergencyAreaID-Broadcast ::= SEQUENCE (SIZE (1..65535)) OF EmergencyAreaID-Broadcast-Item
using emergency_area_id_broadcast_l = dyn_array<emergency_area_id_broadcast_item_s>;

// TAI-Broadcast ::= SEQUENCE (SIZE (1..65535)) OF TAI-Broadcast-Item
using tai_broadcast_l = dyn_array<tai_broadcast_item_s>;

// BroadcastCompletedAreaList ::= CHOICE
struct broadcast_completed_area_list_c {
  struct types_opts {
    enum options { cell_id_broadcast, tai_broadcast, emergency_area_id_broadcast, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  broadcast_completed_area_list_c() = default;
  broadcast_completed_area_list_c(const broadcast_completed_area_list_c& other);
  broadcast_completed_area_list_c& operator=(const broadcast_completed_area_list_c& other);
  ~broadcast_completed_area_list_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_id_broadcast_l& cell_id_broadcast()
  {
    assert_choice_type("cellID-Broadcast", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<cell_id_broadcast_l>();
  }
  tai_broadcast_l& tai_broadcast()
  {
    assert_choice_type("tAI-Broadcast", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<tai_broadcast_l>();
  }
  emergency_area_id_broadcast_l& emergency_area_id_broadcast()
  {
    assert_choice_type("emergencyAreaID-Broadcast", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<emergency_area_id_broadcast_l>();
  }
  const cell_id_broadcast_l& cell_id_broadcast() const
  {
    assert_choice_type("cellID-Broadcast", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<cell_id_broadcast_l>();
  }
  const tai_broadcast_l& tai_broadcast() const
  {
    assert_choice_type("tAI-Broadcast", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<tai_broadcast_l>();
  }
  const emergency_area_id_broadcast_l& emergency_area_id_broadcast() const
  {
    assert_choice_type("emergencyAreaID-Broadcast", type_.to_string(), "BroadcastCompletedAreaList");
    return c.get<emergency_area_id_broadcast_l>();
  }
  cell_id_broadcast_l& set_cell_id_broadcast()
  {
    set(types::cell_id_broadcast);
    return c.get<cell_id_broadcast_l>();
  }
  tai_broadcast_l& set_tai_broadcast()
  {
    set(types::tai_broadcast);
    return c.get<tai_broadcast_l>();
  }
  emergency_area_id_broadcast_l& set_emergency_area_id_broadcast()
  {
    set(types::emergency_area_id_broadcast);
    return c.get<emergency_area_id_broadcast_l>();
  }

private:
  types                                                                                type_;
  choice_buffer_t<cell_id_broadcast_l, emergency_area_id_broadcast_l, tai_broadcast_l> c;

  void destroy_();
};

// CGI-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o cgi_ext_ies_o;

typedef protocol_ext_container_empty_l cgi_ext_ies_container;

// CGI ::= SEQUENCE
struct cgi_s {
  bool                     ext             = false;
  bool                     rac_present     = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plm_nid;
  fixed_octstring<2, true> lac;
  fixed_octstring<2, true> ci;
  fixed_octstring<1, true> rac;
  cgi_ext_ies_container    ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSG-IdList-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o csg_id_list_item_ext_ies_o;

typedef protocol_ext_container_empty_l csg_id_list_item_ext_ies_container;

// CSG-IdList-Item ::= SEQUENCE
struct csg_id_list_item_s {
  bool                               ext             = false;
  bool                               ie_exts_present = false;
  fixed_bitstring<27, false, true>   csg_id;
  csg_id_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSG-IdList ::= SEQUENCE (SIZE (1..256)) OF CSG-IdList-Item
using csg_id_list_l = dyn_array<csg_id_list_item_s>;

// CSGMembershipInfo-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o csg_membership_info_ext_ies_o;

// CSGMembershipStatus ::= ENUMERATED
struct csg_membership_status_opts {
  enum options { member, not_member, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<csg_membership_status_opts> csg_membership_status_e;

// CellAccessMode ::= ENUMERATED
struct cell_access_mode_opts {
  enum options { hybrid, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cell_access_mode_opts, true> cell_access_mode_e;

typedef protocol_ext_container_empty_l csg_membership_info_ext_ies_container;

// CSGMembershipInfo ::= SEQUENCE
struct csg_membership_info_s {
  bool                                  ext                      = false;
  bool                                  cell_access_mode_present = false;
  bool                                  plm_nid_present          = false;
  bool                                  ie_exts_present          = false;
  csg_membership_status_e               csg_membership_status;
  fixed_bitstring<27, false, true>      csg_id;
  cell_access_mode_e                    cell_access_mode;
  fixed_octstring<3, true>              plm_nid;
  csg_membership_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-Cell-ID ::= CHOICE
struct irat_cell_id_c {
  struct types_opts {
    enum options { eutran, utran, geran, /*...*/ ehrpd, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true, 1> types;

  // choice methods
  irat_cell_id_c() = default;
  irat_cell_id_c(const irat_cell_id_c& other);
  irat_cell_id_c& operator=(const irat_cell_id_c& other);
  ~irat_cell_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  unbounded_octstring<true>& eutran()
  {
    assert_choice_type("eUTRAN", type_.to_string(), "IRAT-Cell-ID");
    return c.get<unbounded_octstring<true> >();
  }
  unbounded_octstring<true>& utran()
  {
    assert_choice_type("uTRAN", type_.to_string(), "IRAT-Cell-ID");
    return c.get<unbounded_octstring<true> >();
  }
  unbounded_octstring<true>& geran()
  {
    assert_choice_type("gERAN", type_.to_string(), "IRAT-Cell-ID");
    return c.get<unbounded_octstring<true> >();
  }
  fixed_octstring<16, true>& ehrpd()
  {
    assert_choice_type("eHRPD", type_.to_string(), "IRAT-Cell-ID");
    return c.get<fixed_octstring<16, true> >();
  }
  const unbounded_octstring<true>& eutran() const
  {
    assert_choice_type("eUTRAN", type_.to_string(), "IRAT-Cell-ID");
    return c.get<unbounded_octstring<true> >();
  }
  const unbounded_octstring<true>& utran() const
  {
    assert_choice_type("uTRAN", type_.to_string(), "IRAT-Cell-ID");
    return c.get<unbounded_octstring<true> >();
  }
  const unbounded_octstring<true>& geran() const
  {
    assert_choice_type("gERAN", type_.to_string(), "IRAT-Cell-ID");
    return c.get<unbounded_octstring<true> >();
  }
  const fixed_octstring<16, true>& ehrpd() const
  {
    assert_choice_type("eHRPD", type_.to_string(), "IRAT-Cell-ID");
    return c.get<fixed_octstring<16, true> >();
  }
  unbounded_octstring<true>& set_eutran()
  {
    set(types::eutran);
    return c.get<unbounded_octstring<true> >();
  }
  unbounded_octstring<true>& set_utran()
  {
    set(types::utran);
    return c.get<unbounded_octstring<true> >();
  }
  unbounded_octstring<true>& set_geran()
  {
    set(types::geran);
    return c.get<unbounded_octstring<true> >();
  }
  fixed_octstring<16, true>& set_ehrpd()
  {
    set(types::ehrpd);
    return c.get<fixed_octstring<16, true> >();
  }

private:
  types                                                                  type_;
  choice_buffer_t<fixed_octstring<16, true>, unbounded_octstring<true> > c;

  void destroy_();
};

// CandidateCellList ::= SEQUENCE (SIZE (1..16)) OF IRAT-Cell-ID
using candidate_cell_list_l = dyn_array<irat_cell_id_c>;

// CandidatePCI ::= SEQUENCE
struct candidate_pci_s {
  bool                      ext = false;
  uint16_t                  pci = 0;
  unbounded_octstring<true> earfcn;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidatePCIList ::= SEQUENCE (SIZE (1..16)) OF CandidatePCI
using candidate_pci_list_l = dyn_array<candidate_pci_s>;

// CauseMisc ::= ENUMERATED
struct cause_misc_opts {
  enum options {
    ctrl_processing_overload,
    not_enough_user_plane_processing_res,
    hardware_fail,
    om_intervention,
    unspecified,
    unknown_plmn,
    // ...
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<cause_misc_opts, true> cause_misc_e;

// CauseNas ::= ENUMERATED
struct cause_nas_opts {
  enum options {
    normal_release,
    authentication_fail,
    detach,
    unspecified,
    /*...*/ csg_subscription_expiry,
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<cause_nas_opts, true, 1> cause_nas_e;

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
    tx2relocoverall_expiry,
    successful_ho,
    release_due_to_eutran_generated_reason,
    ho_cancelled,
    partial_ho,
    ho_fail_in_target_epc_enb_or_target_sys,
    ho_target_not_allowed,
    ts1relocoverall_expiry,
    ts1relocprep_expiry,
    cell_not_available,
    unknown_target_id,
    no_radio_res_available_in_target_cell,
    unknown_mme_ue_s1ap_id,
    unknown_enb_ue_s1ap_id,
    unknown_pair_ue_s1ap_id,
    ho_desirable_for_radio_reason,
    time_crit_ho,
    res_optim_ho,
    reduce_load_in_serving_cell,
    user_inactivity,
    radio_conn_with_ue_lost,
    load_balancing_tau_required,
    cs_fallback_triggered,
    ue_not_available_for_ps_service,
    radio_res_not_available,
    fail_in_radio_interface_proc,
    invalid_qos_combination,
    interrat_redirection,
    interaction_with_other_proc,
    unknown_erab_id,
    multiple_erab_id_instances,
    encryption_and_or_integrity_protection_algorithms_not_supported,
    s1_intra_sys_ho_triggered,
    s1_inter_sys_ho_triggered,
    x2_ho_triggered,
    // ...
    redirection_towards_minus1x_rtt,
    not_supported_qci_value,
    invalid_csg_id,
    release_due_to_pre_emption,
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<cause_radio_network_opts, true, 4> cause_radio_network_e;

// CauseTransport ::= ENUMERATED
struct cause_transport_opts {
  enum options { transport_res_unavailable, unspecified, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cause_transport_opts, true> cause_transport_e;

// Cause ::= CHOICE
struct cause_c {
  struct types_opts {
    enum options { radio_network, transport, nas, protocol, misc, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    std::string to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  cause_c() = default;
  cause_c(const cause_c& other);
  cause_c& operator=(const cause_c& other);
  ~cause_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
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

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// Cdma2000OneXSRVCCInfo-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o cdma2000_one_xsrvcc_info_ext_ies_o;

typedef protocol_ext_container_empty_l cdma2000_one_xsrvcc_info_ext_ies_container;

// Cdma2000OneXSRVCCInfo ::= SEQUENCE
struct cdma2000_one_xsrvcc_info_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  unbounded_octstring<true>                  cdma2000_one_xmeid;
  unbounded_octstring<true>                  cdma2000_one_xmsi;
  unbounded_octstring<true>                  cdma2000_one_xpilot;
  cdma2000_one_xsrvcc_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellsToActivateList-Item ::= SEQUENCE
struct cells_to_activ_list_item_s {
  bool                      ext = false;
  unbounded_octstring<true> cell_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellsToActivateList ::= SEQUENCE (SIZE (1..256)) OF CellsToActivateList-Item
using cells_to_activ_list_l = dyn_array<cells_to_activ_list_item_s>;

// CellActivationRequest ::= SEQUENCE
struct cell_activation_request_s {
  bool                  ext                             = false;
  bool                  minimum_activation_time_present = false;
  cells_to_activ_list_l cells_to_activ_list;
  uint8_t               minimum_activation_time = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellActivationResponse ::= SEQUENCE
struct cell_activation_resp_s {
  bool               ext = false;
  activ_cells_list_l activ_cells_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EHRPDCompositeAvailableCapacity ::= SEQUENCE
struct ehrpd_composite_available_capacity_s {
  bool    ext                               = false;
  uint8_t ehrpd_sector_capacity_class_value = 1;
  uint8_t ehrpd_capacity_value              = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EHRPDSectorLoadReportingResponse ::= SEQUENCE
struct ehrpd_sector_load_report_resp_s {
  bool                                 ext = false;
  ehrpd_composite_available_capacity_s dl_ehrpd_composite_available_capacity;
  ehrpd_composite_available_capacity_s ul_ehrpd_composite_available_capacity;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRANcellLoadReportingResponse ::= SEQUENCE
struct eutra_ncell_load_report_resp_s {
  bool                      ext = false;
  unbounded_octstring<true> composite_available_capacity_group;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellLoadReportingResponse ::= CHOICE
struct cell_load_report_resp_c {
  struct types_opts {
    enum options { eutran, utran, geran, /*...*/ ehrpd, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true, 1> types;

  // choice methods
  cell_load_report_resp_c() = default;
  cell_load_report_resp_c(const cell_load_report_resp_c& other);
  cell_load_report_resp_c& operator=(const cell_load_report_resp_c& other);
  ~cell_load_report_resp_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_ncell_load_report_resp_s& eutran()
  {
    assert_choice_type("eUTRAN", type_.to_string(), "CellLoadReportingResponse");
    return c.get<eutra_ncell_load_report_resp_s>();
  }
  unbounded_octstring<true>& utran()
  {
    assert_choice_type("uTRAN", type_.to_string(), "CellLoadReportingResponse");
    return c.get<unbounded_octstring<true> >();
  }
  unbounded_octstring<true>& geran()
  {
    assert_choice_type("gERAN", type_.to_string(), "CellLoadReportingResponse");
    return c.get<unbounded_octstring<true> >();
  }
  ehrpd_sector_load_report_resp_s& ehrpd()
  {
    assert_choice_type("eHRPD", type_.to_string(), "CellLoadReportingResponse");
    return c.get<ehrpd_sector_load_report_resp_s>();
  }
  const eutra_ncell_load_report_resp_s& eutran() const
  {
    assert_choice_type("eUTRAN", type_.to_string(), "CellLoadReportingResponse");
    return c.get<eutra_ncell_load_report_resp_s>();
  }
  const unbounded_octstring<true>& utran() const
  {
    assert_choice_type("uTRAN", type_.to_string(), "CellLoadReportingResponse");
    return c.get<unbounded_octstring<true> >();
  }
  const unbounded_octstring<true>& geran() const
  {
    assert_choice_type("gERAN", type_.to_string(), "CellLoadReportingResponse");
    return c.get<unbounded_octstring<true> >();
  }
  const ehrpd_sector_load_report_resp_s& ehrpd() const
  {
    assert_choice_type("eHRPD", type_.to_string(), "CellLoadReportingResponse");
    return c.get<ehrpd_sector_load_report_resp_s>();
  }
  eutra_ncell_load_report_resp_s& set_eutran()
  {
    set(types::eutran);
    return c.get<eutra_ncell_load_report_resp_s>();
  }
  unbounded_octstring<true>& set_utran()
  {
    set(types::utran);
    return c.get<unbounded_octstring<true> >();
  }
  unbounded_octstring<true>& set_geran()
  {
    set(types::geran);
    return c.get<unbounded_octstring<true> >();
  }
  ehrpd_sector_load_report_resp_s& set_ehrpd()
  {
    set(types::ehrpd);
    return c.get<ehrpd_sector_load_report_resp_s>();
  }

private:
  types                                                                                                        type_;
  choice_buffer_t<ehrpd_sector_load_report_resp_s, eutra_ncell_load_report_resp_s, unbounded_octstring<true> > c;

  void destroy_();
};

// NotifyFlag ::= ENUMERATED
struct notify_flag_opts {
  enum options { activ, deactiv, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<notify_flag_opts, true> notify_flag_e;

// NotificationCellList-Item ::= SEQUENCE
struct notif_cell_list_item_s {
  bool                      ext = false;
  unbounded_octstring<true> cell_id;
  notify_flag_e             notify_flag;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NotificationCellList ::= SEQUENCE (SIZE (1..256)) OF NotificationCellList-Item
using notif_cell_list_l = dyn_array<notif_cell_list_item_s>;

// CellStateIndication ::= SEQUENCE
struct cell_state_ind_s {
  bool              ext = false;
  notif_cell_list_l notif_cell_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PrivacyIndicator ::= ENUMERATED
struct privacy_ind_opts {
  enum options { immediate_mdt, logged_mdt, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<privacy_ind_opts, true> privacy_ind_e;

// CellTrafficTraceIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct cell_traffic_trace_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        e_utran_trace_id,
        eutran_cgi,
        trace_collection_entity_ip_address,
        privacy_ind,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                    mme_ue_s1ap_id();
    uint32_t&                                    enb_ue_s1ap_id();
    fixed_octstring<8, true>&                    e_utran_trace_id();
    eutran_cgi_s&                                eutran_cgi();
    bounded_bitstring<1, 160, true, true>&       trace_collection_entity_ip_address();
    privacy_ind_e&                               privacy_ind();
    const uint64_t&                              mme_ue_s1ap_id() const;
    const uint32_t&                              enb_ue_s1ap_id() const;
    const fixed_octstring<8, true>&              e_utran_trace_id() const;
    const eutran_cgi_s&                          eutran_cgi() const;
    const bounded_bitstring<1, 160, true, true>& trace_collection_entity_ip_address() const;
    const privacy_ind_e&                         privacy_ind() const;

  private:
    types                                                                                           type_;
    choice_buffer_t<bounded_bitstring<1, 160, true, true>, eutran_cgi_s, fixed_octstring<8, true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

template <class valueT_>
struct protocol_ie_container_item_s {
  uint32_t id = 0;
  crit_e   crit;
  valueT_  value;

  // sequence methods
  protocol_ie_container_item_s(uint32_t id_, crit_e crit_);
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct cell_traffic_trace_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       privacy_ind_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<fixed_octstring<8, true> >                      e_utran_trace_id;
  ie_field_s<eutran_cgi_s>                                   eutran_cgi;
  ie_field_s<bounded_bitstring<1, 160, true, true> >         trace_collection_entity_ip_address;
  ie_field_s<privacy_ind_e>                                  privacy_ind;

  // sequence methods
  cell_traffic_trace_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellTrafficTrace ::= SEQUENCE
struct cell_traffic_trace_s {
  bool                             ext = false;
  cell_traffic_trace_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cell-Size ::= ENUMERATED
struct cell_size_opts {
  enum options { verysmall, small, medium, large, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cell_size_opts, true> cell_size_e;

// CellType-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o cell_type_ext_ies_o;

typedef protocol_ext_container_empty_l cell_type_ext_ies_container;

// CellType ::= SEQUENCE
struct cell_type_s {
  bool                        ext             = false;
  bool                        ie_exts_present = false;
  cell_size_e                 cell_size;
  cell_type_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-CP-SecurityInformation-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o dl_cp_security_info_ext_ies_o;

// CE-ModeBRestricted ::= ENUMERATED
struct ce_mode_brestricted_opts {
  enum options { restricted, not_restricted, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ce_mode_brestricted_opts, true> ce_mode_brestricted_e;

typedef protocol_ext_container_empty_l dl_cp_security_info_ext_ies_container;

// DL-CP-SecurityInformation ::= SEQUENCE
struct dl_cp_security_info_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  fixed_bitstring<16, false, true>      dl_nas_mac;
  dl_cp_security_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EnhancedCoverageRestricted ::= ENUMERATED
struct enhanced_coverage_restricted_opts {
  enum options { restricted, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<enhanced_coverage_restricted_opts, true> enhanced_coverage_restricted_e;

// ConnectionEstablishmentIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct conn_establishment_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        ue_radio_cap,
        enhanced_coverage_restricted,
        dl_cp_security_info,
        ce_mode_brestricted,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                             mme_ue_s1ap_id();
    uint32_t&                             enb_ue_s1ap_id();
    unbounded_octstring<true>&            ue_radio_cap();
    enhanced_coverage_restricted_e&       enhanced_coverage_restricted();
    dl_cp_security_info_s&                dl_cp_security_info();
    ce_mode_brestricted_e&                ce_mode_brestricted();
    const uint64_t&                       mme_ue_s1ap_id() const;
    const uint32_t&                       enb_ue_s1ap_id() const;
    const unbounded_octstring<true>&      ue_radio_cap() const;
    const enhanced_coverage_restricted_e& enhanced_coverage_restricted() const;
    const dl_cp_security_info_s&          dl_cp_security_info() const;
    const ce_mode_brestricted_e&          ce_mode_brestricted() const;

  private:
    types                                                              type_;
    choice_buffer_t<dl_cp_security_info_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct conn_establishment_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       ue_radio_cap_present                 = false;
  bool                                                       enhanced_coverage_restricted_present = false;
  bool                                                       dl_cp_security_info_present          = false;
  bool                                                       ce_mode_brestricted_present          = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<unbounded_octstring<true> >                     ue_radio_cap;
  ie_field_s<enhanced_coverage_restricted_e>                 enhanced_coverage_restricted;
  ie_field_s<dl_cp_security_info_s>                          dl_cp_security_info;
  ie_field_s<ce_mode_brestricted_e>                          ce_mode_brestricted;

  // sequence methods
  conn_establishment_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConnectionEstablishmentIndication ::= SEQUENCE
struct conn_establishment_ind_s {
  bool                                 ext = false;
  conn_establishment_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CriticalityDiagnostics-IE-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o crit_diagnostics_ie_item_ext_ies_o;

// TypeOfError ::= ENUMERATED
struct type_of_error_opts {
  enum options { not_understood, missing, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<type_of_error_opts, true> type_of_error_e;

typedef protocol_ext_container_empty_l crit_diagnostics_ie_item_ext_ies_container;

// CriticalityDiagnostics-IE-Item ::= SEQUENCE
struct crit_diagnostics_ie_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  crit_e                                     iecrit;
  uint32_t                                   ie_id = 0;
  type_of_error_e                            type_of_error;
  crit_diagnostics_ie_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CriticalityDiagnostics-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o crit_diagnostics_ext_ies_o;

// CriticalityDiagnostics-IE-List ::= SEQUENCE (SIZE (1..256)) OF CriticalityDiagnostics-IE-Item
using crit_diagnostics_ie_list_l = dyn_array<crit_diagnostics_ie_item_s>;

// TriggeringMessage ::= ENUMERATED
struct trigger_msg_opts {
  enum options { init_msg, successful_outcome, unsuccessfull_outcome, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<trigger_msg_opts> trigger_msg_e;

typedef protocol_ext_container_empty_l crit_diagnostics_ext_ies_container;

// CriticalityDiagnostics ::= SEQUENCE
struct crit_diagnostics_s {
  bool                               ext                          = false;
  bool                               proc_code_present            = false;
  bool                               trigger_msg_present          = false;
  bool                               proc_crit_present            = false;
  bool                               ies_crit_diagnostics_present = false;
  bool                               ie_exts_present              = false;
  uint16_t                           proc_code                    = 0;
  trigger_msg_e                      trigger_msg;
  crit_e                             proc_crit;
  crit_diagnostics_ie_list_l         ies_crit_diagnostics;
  crit_diagnostics_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DeactivateTraceIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct deactiv_trace_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, e_utran_trace_id, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                       mme_ue_s1ap_id();
    uint32_t&                       enb_ue_s1ap_id();
    fixed_octstring<8, true>&       e_utran_trace_id();
    const uint64_t&                 mme_ue_s1ap_id() const;
    const uint32_t&                 enb_ue_s1ap_id() const;
    const fixed_octstring<8, true>& e_utran_trace_id() const;

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

struct deactiv_trace_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<fixed_octstring<8, true> >                      e_utran_trace_id;

  // sequence methods
  deactiv_trace_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DeactivateTrace ::= SEQUENCE
struct deactiv_trace_s {
  bool                        ext = false;
  deactiv_trace_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ForbiddenLACs ::= SEQUENCE (SIZE (1..4096)) OF OCTET STRING (SIZE (2))
using forbidden_lacs_l = dyn_array<fixed_octstring<2, true> >;

// ForbiddenLAs-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o forbidden_las_item_ext_ies_o;

// ForbiddenTACs ::= SEQUENCE (SIZE (1..4096)) OF OCTET STRING (SIZE (2))
using forbidden_tacs_l = dyn_array<fixed_octstring<2, true> >;

// ForbiddenTAs-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o forbidden_tas_item_ext_ies_o;

typedef protocol_ext_container_empty_l forbidden_las_item_ext_ies_container;

// ForbiddenLAs-Item ::= SEQUENCE
struct forbidden_las_item_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  fixed_octstring<3, true>             plmn_id;
  forbidden_lacs_l                     forbidden_lacs;
  forbidden_las_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l forbidden_tas_item_ext_ies_container;

// ForbiddenTAs-Item ::= SEQUENCE
struct forbidden_tas_item_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  fixed_octstring<3, true>             plmn_id;
  forbidden_tacs_l                     forbidden_tacs;
  forbidden_tas_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EPLMNs ::= SEQUENCE (SIZE (1..15)) OF OCTET STRING (SIZE (3))
using eplmns_l = bounded_array<fixed_octstring<3, true>, 15>;

// ForbiddenInterRATs ::= ENUMERATED
struct forbidden_inter_rats_opts {
  enum options { all, geran, utran, cdma2000, /*...*/ geranandutran, cdma2000andutran, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<forbidden_inter_rats_opts, true, 2> forbidden_inter_rats_e;

// ForbiddenLAs ::= SEQUENCE (SIZE (1..16)) OF ForbiddenLAs-Item
using forbidden_las_l = dyn_array<forbidden_las_item_s>;

// ForbiddenTAs ::= SEQUENCE (SIZE (1..16)) OF ForbiddenTAs-Item
using forbidden_tas_l = dyn_array<forbidden_tas_item_s>;

// HandoverRestrictionList-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o ho_restrict_list_ext_ies_o;

// DLNASPDUDeliveryAckRequest ::= ENUMERATED
struct dlnaspdu_delivery_ack_request_opts {
  enum options { requested, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<dlnaspdu_delivery_ack_request_opts, true> dlnaspdu_delivery_ack_request_e;

typedef protocol_ext_container_empty_l ho_restrict_list_ext_ies_container;

// HandoverRestrictionList ::= SEQUENCE
struct ho_restrict_list_s {
  bool                               ext                          = false;
  bool                               equivalent_plmns_present     = false;
  bool                               forbidden_tas_present        = false;
  bool                               forbidden_las_present        = false;
  bool                               forbidden_inter_rats_present = false;
  bool                               ie_exts_present              = false;
  fixed_octstring<3, true>           serving_plmn;
  eplmns_l                           equivalent_plmns;
  forbidden_tas_l                    forbidden_tas;
  forbidden_las_l                    forbidden_las;
  forbidden_inter_rats_e             forbidden_inter_rats;
  ho_restrict_list_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PendingDataIndication ::= ENUMERATED
struct pending_data_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<pending_data_ind_opts, true> pending_data_ind_e;

// SRVCCOperationPossible ::= ENUMERATED
struct srvcc_operation_possible_opts {
  enum options { possible, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<srvcc_operation_possible_opts, true> srvcc_operation_possible_e;

// DownlinkNASTransport-IEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct dl_nas_transport_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        nas_pdu,
        ho_restrict_list,
        subscriber_profile_idfor_rfp,
        srvcc_operation_possible,
        ue_radio_cap,
        dlnaspdu_delivery_ack_request,
        enhanced_coverage_restricted,
        ce_mode_brestricted,
        pending_data_ind,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                              mme_ue_s1ap_id();
    uint32_t&                              enb_ue_s1ap_id();
    unbounded_octstring<true>&             nas_pdu();
    ho_restrict_list_s&                    ho_restrict_list();
    uint16_t&                              subscriber_profile_idfor_rfp();
    srvcc_operation_possible_e&            srvcc_operation_possible();
    unbounded_octstring<true>&             ue_radio_cap();
    dlnaspdu_delivery_ack_request_e&       dlnaspdu_delivery_ack_request();
    enhanced_coverage_restricted_e&        enhanced_coverage_restricted();
    ce_mode_brestricted_e&                 ce_mode_brestricted();
    pending_data_ind_e&                    pending_data_ind();
    const uint64_t&                        mme_ue_s1ap_id() const;
    const uint32_t&                        enb_ue_s1ap_id() const;
    const unbounded_octstring<true>&       nas_pdu() const;
    const ho_restrict_list_s&              ho_restrict_list() const;
    const uint16_t&                        subscriber_profile_idfor_rfp() const;
    const srvcc_operation_possible_e&      srvcc_operation_possible() const;
    const unbounded_octstring<true>&       ue_radio_cap() const;
    const dlnaspdu_delivery_ack_request_e& dlnaspdu_delivery_ack_request() const;
    const enhanced_coverage_restricted_e&  enhanced_coverage_restricted() const;
    const ce_mode_brestricted_e&           ce_mode_brestricted() const;
    const pending_data_ind_e&              pending_data_ind() const;

  private:
    types                                                           type_;
    choice_buffer_t<ho_restrict_list_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct dl_nas_transport_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       ho_restrict_list_present              = false;
  bool                                                       subscriber_profile_idfor_rfp_present  = false;
  bool                                                       srvcc_operation_possible_present      = false;
  bool                                                       ue_radio_cap_present                  = false;
  bool                                                       dlnaspdu_delivery_ack_request_present = false;
  bool                                                       enhanced_coverage_restricted_present  = false;
  bool                                                       ce_mode_brestricted_present           = false;
  bool                                                       pending_data_ind_present              = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<unbounded_octstring<true> >                     nas_pdu;
  ie_field_s<ho_restrict_list_s>                             ho_restrict_list;
  ie_field_s<integer<uint16_t, 1, 256, false, true> >        subscriber_profile_idfor_rfp;
  ie_field_s<srvcc_operation_possible_e>                     srvcc_operation_possible;
  ie_field_s<unbounded_octstring<true> >                     ue_radio_cap;
  ie_field_s<dlnaspdu_delivery_ack_request_e>                dlnaspdu_delivery_ack_request;
  ie_field_s<enhanced_coverage_restricted_e>                 enhanced_coverage_restricted;
  ie_field_s<ce_mode_brestricted_e>                          ce_mode_brestricted;
  ie_field_s<pending_data_ind_e>                             pending_data_ind;

  // sequence methods
  dl_nas_transport_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkNASTransport ::= SEQUENCE
struct dl_nas_transport_s {
  bool                           ext = false;
  dl_nas_transport_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkNonUEAssociatedLPPaTransport-IEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct dl_non_ueassociated_lp_pa_transport_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { routing_id, lp_pa_pdu, nulltype } value;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                        routing_id();
    unbounded_octstring<true>&       lp_pa_pdu();
    const uint16_t&                  routing_id() const;
    const unbounded_octstring<true>& lp_pa_pdu() const;

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

struct dl_non_ueassociated_lp_pa_transport_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, false, true> > routing_id;
  ie_field_s<unbounded_octstring<true> >              lp_pa_pdu;

  // sequence methods
  dl_non_ueassociated_lp_pa_transport_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkNonUEAssociatedLPPaTransport ::= SEQUENCE
struct dl_non_ueassociated_lp_pa_transport_s {
  bool                                              ext = false;
  dl_non_ueassociated_lp_pa_transport_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABDataForwardingItem-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_data_forwarding_item_ext_ies_o;

typedef protocol_ext_container_empty_l erab_data_forwarding_item_ext_ies_container;

// E-RABDataForwardingItem ::= SEQUENCE
struct erab_data_forwarding_item_s {
  bool                                        ext                                = false;
  bool                                        dl_transport_layer_address_present = false;
  bool                                        dl_g_tp_teid_present               = false;
  bool                                        ul_transport_layer_address_present = false;
  bool                                        ul_gtp_teid_present                = false;
  bool                                        ie_exts_present                    = false;
  uint8_t                                     erab_id                            = 0;
  bounded_bitstring<1, 160, true, true>       dl_transport_layer_address;
  fixed_octstring<4, true>                    dl_g_tp_teid;
  bounded_bitstring<1, 160, true, true>       ul_transport_layer_address;
  fixed_octstring<4, true>                    ul_gtp_teid;
  erab_data_forwarding_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ProtocolIE-SingleContainer-item{S1AP-PROTOCOL-IES : IEsSetParam} ::= ProtocolIE-SingleContainer
template <class ies_set_paramT_>
using protocol_ie_single_container_item_s = protocol_ie_single_container_s<ies_set_paramT_>;

// E-RAB-IE-ContainerList{S1AP-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE (SIZE (1..256)) OF
// ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES : IEsSetParam}
template <class ies_set_paramT_>
using erab_ie_container_list_l = dyn_seq_of<protocol_ie_single_container_item_s<ies_set_paramT_>, 1, 256, true>;

// E-RABDataForwardingItemIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_data_forwarding_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_data_forwarding_item, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_data_forwarding_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_data_forwarding_item_s&       erab_data_forwarding_item() { return c; }
    const erab_data_forwarding_item_s& erab_data_forwarding_item() const { return c; }

  private:
    erab_data_forwarding_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cdma2000HOStatus ::= ENUMERATED
struct cdma2000_ho_status_opts {
  enum options { hosuccess, hofail, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cdma2000_ho_status_opts, true> cdma2000_ho_status_e;

// Cdma2000RATType ::= ENUMERATED
struct cdma2000_rat_type_opts {
  enum options { hrpd, onex_rtt, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<cdma2000_rat_type_opts, true> cdma2000_rat_type_e;

// DownlinkS1cdma2000tunnellingIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct dl_s1cdma2000tunnelling_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        erab_subjectto_data_forwarding_list,
        cdma2000_ho_status,
        cdma2000_rat_type,
        cdma2000_pdu,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                                        mme_ue_s1ap_id();
    uint32_t&                                                        enb_ue_s1ap_id();
    erab_ie_container_list_l<erab_data_forwarding_item_ies_o>&       erab_subjectto_data_forwarding_list();
    cdma2000_ho_status_e&                                            cdma2000_ho_status();
    cdma2000_rat_type_e&                                             cdma2000_rat_type();
    unbounded_octstring<true>&                                       cdma2000_pdu();
    const uint64_t&                                                  mme_ue_s1ap_id() const;
    const uint32_t&                                                  enb_ue_s1ap_id() const;
    const erab_ie_container_list_l<erab_data_forwarding_item_ies_o>& erab_subjectto_data_forwarding_list() const;
    const cdma2000_ho_status_e&                                      cdma2000_ho_status() const;
    const cdma2000_rat_type_e&                                       cdma2000_rat_type() const;
    const unbounded_octstring<true>&                                 cdma2000_pdu() const;

  private:
    types                                                                                                  type_;
    choice_buffer_t<erab_ie_container_list_l<erab_data_forwarding_item_ies_o>, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct dl_s1cdma2000tunnelling_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       erab_subjectto_data_forwarding_list_present = false;
  bool                                                       cdma2000_ho_status_present                  = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<erab_ie_container_list_l<erab_data_forwarding_item_ies_o> > erab_subjectto_data_forwarding_list;
  ie_field_s<cdma2000_ho_status_e>                                       cdma2000_ho_status;
  ie_field_s<cdma2000_rat_type_e>                                        cdma2000_rat_type;
  ie_field_s<unbounded_octstring<true> >                                 cdma2000_pdu;

  // sequence methods
  dl_s1cdma2000tunnelling_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkS1cdma2000tunnelling ::= SEQUENCE
struct dl_s1cdma2000tunnelling_s {
  bool                                  ext = false;
  dl_s1cdma2000tunnelling_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkUEAssociatedLPPaTransport-IEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct dl_ueassociated_lp_pa_transport_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, routing_id, lp_pa_pdu, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        mme_ue_s1ap_id();
    uint32_t&                        enb_ue_s1ap_id();
    uint16_t&                        routing_id();
    unbounded_octstring<true>&       lp_pa_pdu();
    const uint64_t&                  mme_ue_s1ap_id() const;
    const uint32_t&                  enb_ue_s1ap_id() const;
    const uint16_t&                  routing_id() const;
    const unbounded_octstring<true>& lp_pa_pdu() const;

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

struct dl_ueassociated_lp_pa_transport_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<integer<uint16_t, 0, 255, false, true> >        routing_id;
  ie_field_s<unbounded_octstring<true> >                     lp_pa_pdu;

  // sequence methods
  dl_ueassociated_lp_pa_transport_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkUEAssociatedLPPaTransport ::= SEQUENCE
struct dl_ueassociated_lp_pa_transport_s {
  bool                                          ext = false;
  dl_ueassociated_lp_pa_transport_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ProtocolIE-ContainerPair-item{S1AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= ProtocolIE-ContainerPair
template <class ies_set_paramT_>
using protocol_ie_container_pair_item_l = protocol_ie_container_pair_l<ies_set_paramT_>;

// E-RAB-IE-ContainerPairList{S1AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE (SIZE (1..256)) OF
// ProtocolIE-ContainerPair{S1AP-PROTOCOL-IES-PAIR : IEsSetParam}
template <class ies_set_paramT_>
using erab_ie_container_pair_list_l =
    dyn_seq_of<dyn_seq_of<protocol_ie_field_pair_s<ies_set_paramT_>, 0, 65535, true>, 1, 256>;

// E-RABAdmittedItem-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_admitted_item_ext_ies_o;

typedef protocol_ext_container_empty_l erab_admitted_item_ext_ies_container;

// E-RABAdmittedItem ::= SEQUENCE
struct erab_admitted_item_s {
  bool                                  ext                                = false;
  bool                                  dl_transport_layer_address_present = false;
  bool                                  dl_g_tp_teid_present               = false;
  bool                                  ul_transport_layer_address_present = false;
  bool                                  ul_gtp_teid_present                = false;
  bool                                  ie_exts_present                    = false;
  uint8_t                               erab_id                            = 0;
  bounded_bitstring<1, 160, true, true> transport_layer_address;
  fixed_octstring<4, true>              gtp_teid;
  bounded_bitstring<1, 160, true, true> dl_transport_layer_address;
  fixed_octstring<4, true>              dl_g_tp_teid;
  bounded_bitstring<1, 160, true, true> ul_transport_layer_address;
  fixed_octstring<4, true>              ul_gtp_teid;
  erab_admitted_item_ext_ies_container  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABAdmittedItemIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_admitted_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_admitted_item, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_admitted_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_admitted_item_s&       erab_admitted_item() { return c; }
    const erab_admitted_item_s& erab_admitted_item() const { return c; }

  private:
    erab_admitted_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABFailedToResumeItemResumeReq-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_failed_to_resume_item_resume_req_ext_ies_o;

typedef protocol_ext_container_empty_l erab_failed_to_resume_item_resume_req_ext_ies_container;

// E-RABFailedToResumeItemResumeReq ::= SEQUENCE
struct erab_failed_to_resume_item_resume_req_s {
  bool                                                    ext             = false;
  bool                                                    ie_exts_present = false;
  uint8_t                                                 erab_id         = 0;
  cause_c                                                 cause;
  erab_failed_to_resume_item_resume_req_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABFailedToResumeItemResumeReqIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_failed_to_resume_item_resume_req_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_failed_to_resume_item_resume_req, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_failed_to_resume_item_resume_req; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_failed_to_resume_item_resume_req_s&       erab_failed_to_resume_item_resume_req() { return c; }
    const erab_failed_to_resume_item_resume_req_s& erab_failed_to_resume_item_resume_req() const { return c; }

  private:
    erab_failed_to_resume_item_resume_req_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABFailedToResumeItemResumeRes-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_failed_to_resume_item_resume_res_ext_ies_o;

typedef protocol_ext_container_empty_l erab_failed_to_resume_item_resume_res_ext_ies_container;

// E-RABFailedToResumeItemResumeRes ::= SEQUENCE
struct erab_failed_to_resume_item_resume_res_s {
  bool                                                    ext             = false;
  bool                                                    ie_exts_present = false;
  uint8_t                                                 erab_id         = 0;
  cause_c                                                 cause;
  erab_failed_to_resume_item_resume_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABFailedToResumeItemResumeResIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_failed_to_resume_item_resume_res_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_failed_to_resume_item_resume_res, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_failed_to_resume_item_resume_res; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_failed_to_resume_item_resume_res_s&       erab_failed_to_resume_item_resume_res() { return c; }
    const erab_failed_to_resume_item_resume_res_s& erab_failed_to_resume_item_resume_res() const { return c; }

  private:
    erab_failed_to_resume_item_resume_res_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABFailedToSetupItemHOReqAckExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_failed_to_setup_item_ho_req_ack_ext_ies_o;

typedef protocol_ext_container_empty_l erab_failed_to_setup_item_ho_req_ack_ext_ies_container;

// E-RABFailedToSetupItemHOReqAck ::= SEQUENCE
struct erab_failed_to_setup_item_ho_req_ack_s {
  bool                                                   ext             = false;
  bool                                                   ie_exts_present = false;
  uint8_t                                                erab_id         = 0;
  cause_c                                                cause;
  erab_failed_to_setup_item_ho_req_ack_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABFailedtoSetupItemHOReqAckIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_failedto_setup_item_ho_req_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_failedto_setup_item_ho_req_ack, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_failedto_setup_item_ho_req_ack; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_failed_to_setup_item_ho_req_ack_s&       erab_failedto_setup_item_ho_req_ack() { return c; }
    const erab_failed_to_setup_item_ho_req_ack_s& erab_failedto_setup_item_ho_req_ack() const { return c; }

  private:
    erab_failed_to_setup_item_ho_req_ack_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DL-Forwarding ::= ENUMERATED
struct dl_forwarding_opts {
  enum options { dl_forwarding_proposed, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<dl_forwarding_opts, true> dl_forwarding_e;

// E-RABInformationListItem-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_info_list_item_ext_ies_o;

typedef protocol_ext_container_empty_l erab_info_list_item_ext_ies_container;

// E-RABInformationListItem ::= SEQUENCE
struct erab_info_list_item_s {
  bool                                  ext                   = false;
  bool                                  dl_forwarding_present = false;
  bool                                  ie_exts_present       = false;
  uint8_t                               erab_id               = 0;
  dl_forwarding_e                       dl_forwarding;
  erab_info_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABInformationListIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_info_list_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_info_list_item, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_info_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_info_list_item_s&       erab_info_list_item() { return c; }
    const erab_info_list_item_s& erab_info_list_item() const { return c; }

  private:
    erab_info_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABInformationList ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES : IEsSetParam}
using erab_info_list_l = dyn_array<protocol_ie_single_container_s<erab_info_list_ies_o> >;

// E-RABItem-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_item_ext_ies_o;

typedef protocol_ext_container_empty_l erab_item_ext_ies_container;

// E-RABItem ::= SEQUENCE
struct erab_item_s {
  bool                        ext             = false;
  bool                        ie_exts_present = false;
  uint8_t                     erab_id         = 0;
  cause_c                     cause;
  erab_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABItemIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_item, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_item_s&       erab_item() { return c; }
    const erab_item_s& erab_item() const { return c; }

  private:
    erab_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GBR-QosInformation-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o gbr_qos_info_ext_ies_o;

// E-RABQoSParameters-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_qos_params_ext_ies_o;

typedef protocol_ext_container_empty_l gbr_qos_info_ext_ies_container;

// GBR-QosInformation ::= SEQUENCE
struct gbr_qos_info_s {
  bool                           ext                        = false;
  bool                           ie_exts_present            = false;
  uint64_t                       erab_maximum_bitrate_dl    = 0;
  uint64_t                       erab_maximum_bitrate_ul    = 0;
  uint64_t                       erab_guaranteed_bitrate_dl = 0;
  uint64_t                       erab_guaranteed_bitrate_ul = 0;
  gbr_qos_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l erab_qos_params_ext_ies_container;

// E-RABLevelQoSParameters ::= SEQUENCE
struct erab_level_qos_params_s {
  bool                              ext                  = false;
  bool                              gbr_qos_info_present = false;
  bool                              ie_exts_present      = false;
  uint16_t                          qci                  = 0;
  alloc_and_retention_prio_s        alloc_retention_prio;
  gbr_qos_info_s                    gbr_qos_info;
  erab_qos_params_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABList ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES : IEsSetParam}
using erab_list_l = dyn_array<protocol_ie_single_container_s<erab_item_ies_o> >;

// E-RABModifyItemBearerModConfExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_modify_item_bearer_mod_conf_ext_ies_o;

typedef protocol_ext_container_empty_l erab_modify_item_bearer_mod_conf_ext_ies_container;

// E-RABModifyItemBearerModConf ::= SEQUENCE
struct erab_modify_item_bearer_mod_conf_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  uint8_t                                            erab_id         = 0;
  erab_modify_item_bearer_mod_conf_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABModifyItemBearerModConfIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_modify_item_bearer_mod_conf_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_modify_item_bearer_mod_conf, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_modify_item_bearer_mod_conf; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_modify_item_bearer_mod_conf_s&       erab_modify_item_bearer_mod_conf() { return c; }
    const erab_modify_item_bearer_mod_conf_s& erab_modify_item_bearer_mod_conf() const { return c; }

  private:
    erab_modify_item_bearer_mod_conf_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABModifyListBearerModConf ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES :
// IEsSetParam}
using erab_modify_list_bearer_mod_conf_l =
    dyn_array<protocol_ie_single_container_s<erab_modify_item_bearer_mod_conf_ies_o> >;

// E-RABModificationConfirmIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_mod_confirm_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        erab_modify_list_bearer_mod_conf,
        erab_failed_to_modify_list_bearer_mod_conf,
        erab_to_be_released_list_bearer_mod_conf,
        crit_diagnostics,
        csg_membership_status,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                 mme_ue_s1ap_id();
    uint32_t&                                 enb_ue_s1ap_id();
    erab_modify_list_bearer_mod_conf_l&       erab_modify_list_bearer_mod_conf();
    erab_list_l&                              erab_failed_to_modify_list_bearer_mod_conf();
    erab_list_l&                              erab_to_be_released_list_bearer_mod_conf();
    crit_diagnostics_s&                       crit_diagnostics();
    csg_membership_status_e&                  csg_membership_status();
    const uint64_t&                           mme_ue_s1ap_id() const;
    const uint32_t&                           enb_ue_s1ap_id() const;
    const erab_modify_list_bearer_mod_conf_l& erab_modify_list_bearer_mod_conf() const;
    const erab_list_l&                        erab_failed_to_modify_list_bearer_mod_conf() const;
    const erab_list_l&                        erab_to_be_released_list_bearer_mod_conf() const;
    const crit_diagnostics_s&                 crit_diagnostics() const;
    const csg_membership_status_e&            csg_membership_status() const;

  private:
    types                                                                                type_;
    choice_buffer_t<crit_diagnostics_s, erab_list_l, erab_modify_list_bearer_mod_conf_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct erab_mod_confirm_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       erab_modify_list_bearer_mod_conf_present           = false;
  bool                                                       erab_failed_to_modify_list_bearer_mod_conf_present = false;
  bool                                                       erab_to_be_released_list_bearer_mod_conf_present   = false;
  bool                                                       crit_diagnostics_present                           = false;
  bool                                                       csg_membership_status_present                      = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_modify_item_bearer_mod_conf_ies_o>, 1, 256, true> >
      erab_modify_list_bearer_mod_conf;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_item_ies_o>, 1, 256, true> >
      erab_failed_to_modify_list_bearer_mod_conf;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_item_ies_o>, 1, 256, true> >
                                      erab_to_be_released_list_bearer_mod_conf;
  ie_field_s<crit_diagnostics_s>      crit_diagnostics;
  ie_field_s<csg_membership_status_e> csg_membership_status;

  // sequence methods
  erab_mod_confirm_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABModificationConfirm ::= SEQUENCE
struct erab_mod_confirm_s {
  bool                           ext = false;
  erab_mod_confirm_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABNotToBeModifiedItemBearerModInd-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_not_to_be_modified_item_bearer_mod_ind_ext_ies_o;

// E-RABToBeModifiedItemBearerModInd-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_to_be_modified_item_bearer_mod_ind_ext_ies_o;

typedef protocol_ext_container_empty_l erab_not_to_be_modified_item_bearer_mod_ind_ext_ies_container;

// E-RABNotToBeModifiedItemBearerModInd ::= SEQUENCE
struct erab_not_to_be_modified_item_bearer_mod_ind_s {
  bool                                                          ext             = false;
  bool                                                          ie_exts_present = false;
  uint8_t                                                       erab_id         = 0;
  bounded_bitstring<1, 160, true, true>                         transport_layer_address;
  fixed_octstring<4, true>                                      dl_gtp_teid;
  erab_not_to_be_modified_item_bearer_mod_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l erab_to_be_modified_item_bearer_mod_ind_ext_ies_container;

// E-RABToBeModifiedItemBearerModInd ::= SEQUENCE
struct erab_to_be_modified_item_bearer_mod_ind_s {
  bool                                                      ext             = false;
  bool                                                      ie_exts_present = false;
  uint8_t                                                   erab_id         = 0;
  bounded_bitstring<1, 160, true, true>                     transport_layer_address;
  fixed_octstring<4, true>                                  dl_gtp_teid;
  erab_to_be_modified_item_bearer_mod_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABNotToBeModifiedItemBearerModIndIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_not_to_be_modified_item_bearer_mod_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_not_to_be_modified_item_bearer_mod_ind, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_not_to_be_modified_item_bearer_mod_ind; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_not_to_be_modified_item_bearer_mod_ind_s&       erab_not_to_be_modified_item_bearer_mod_ind() { return c; }
    const erab_not_to_be_modified_item_bearer_mod_ind_s& erab_not_to_be_modified_item_bearer_mod_ind() const
    {
      return c;
    }

  private:
    erab_not_to_be_modified_item_bearer_mod_ind_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABToBeModifiedItemBearerModIndIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_to_be_modified_item_bearer_mod_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_to_be_modified_item_bearer_mod_ind, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_to_be_modified_item_bearer_mod_ind; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_to_be_modified_item_bearer_mod_ind_s&       erab_to_be_modified_item_bearer_mod_ind() { return c; }
    const erab_to_be_modified_item_bearer_mod_ind_s& erab_to_be_modified_item_bearer_mod_ind() const { return c; }

  private:
    erab_to_be_modified_item_bearer_mod_ind_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Tunnel-Information-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o tunnel_info_ext_ies_o;

typedef protocol_ext_container_empty_l tunnel_info_ext_ies_container;

// TunnelInformation ::= SEQUENCE
struct tunnel_info_s {
  bool                                  ext                  = false;
  bool                                  udp_port_num_present = false;
  bool                                  ie_exts_present      = false;
  bounded_bitstring<1, 160, true, true> transport_layer_address;
  fixed_octstring<2, true>              udp_port_num;
  tunnel_info_ext_ies_container         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABModificationIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_mod_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        erab_to_be_modified_list_bearer_mod_ind,
        erab_not_to_be_modified_list_bearer_mod_ind,
        csg_membership_info,
        tunnel_info_for_bbf,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                                                mme_ue_s1ap_id();
    uint32_t&                                                                enb_ue_s1ap_id();
    erab_ie_container_list_l<erab_to_be_modified_item_bearer_mod_ind_ies_o>& erab_to_be_modified_list_bearer_mod_ind();
    erab_ie_container_list_l<erab_not_to_be_modified_item_bearer_mod_ind_ies_o>&
                           erab_not_to_be_modified_list_bearer_mod_ind();
    csg_membership_info_s& csg_membership_info();
    tunnel_info_s&         tunnel_info_for_bbf();
    const uint64_t&        mme_ue_s1ap_id() const;
    const uint32_t&        enb_ue_s1ap_id() const;
    const erab_ie_container_list_l<erab_to_be_modified_item_bearer_mod_ind_ies_o>&
    erab_to_be_modified_list_bearer_mod_ind() const;
    const erab_ie_container_list_l<erab_not_to_be_modified_item_bearer_mod_ind_ies_o>&
                                 erab_not_to_be_modified_list_bearer_mod_ind() const;
    const csg_membership_info_s& csg_membership_info() const;
    const tunnel_info_s&         tunnel_info_for_bbf() const;

  private:
    types type_;
    choice_buffer_t<csg_membership_info_s,
                    erab_ie_container_list_l<erab_not_to_be_modified_item_bearer_mod_ind_ies_o>,
                    erab_ie_container_list_l<erab_to_be_modified_item_bearer_mod_ind_ies_o>,
                    tunnel_info_s>
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

struct erab_mod_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool erab_not_to_be_modified_list_bearer_mod_ind_present = false;
  bool csg_membership_info_present                         = false;
  bool tunnel_info_for_bbf_present                         = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<erab_ie_container_list_l<erab_to_be_modified_item_bearer_mod_ind_ies_o> >
      erab_to_be_modified_list_bearer_mod_ind;
  ie_field_s<erab_ie_container_list_l<erab_not_to_be_modified_item_bearer_mod_ind_ies_o> >
                                    erab_not_to_be_modified_list_bearer_mod_ind;
  ie_field_s<csg_membership_info_s> csg_membership_info;
  ie_field_s<tunnel_info_s>         tunnel_info_for_bbf;

  // sequence methods
  erab_mod_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABModificationIndication ::= SEQUENCE
struct erab_mod_ind_s {
  bool                       ext = false;
  erab_mod_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABModifyItemBearerModResExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_modify_item_bearer_mod_res_ext_ies_o;

typedef protocol_ext_container_empty_l erab_modify_item_bearer_mod_res_ext_ies_container;

// E-RABModifyItemBearerModRes ::= SEQUENCE
struct erab_modify_item_bearer_mod_res_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint8_t                                           erab_id         = 0;
  erab_modify_item_bearer_mod_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABModifyItemBearerModResIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_modify_item_bearer_mod_res_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_modify_item_bearer_mod_res, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_modify_item_bearer_mod_res; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_modify_item_bearer_mod_res_s&       erab_modify_item_bearer_mod_res() { return c; }
    const erab_modify_item_bearer_mod_res_s& erab_modify_item_bearer_mod_res() const { return c; }

  private:
    erab_modify_item_bearer_mod_res_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABModifyListBearerModRes ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES :
// IEsSetParam}
using erab_modify_list_bearer_mod_res_l =
    dyn_array<protocol_ie_single_container_s<erab_modify_item_bearer_mod_res_ies_o> >;

// TransportInformation ::= SEQUENCE
struct transport_info_s {
  bool                                  ext = false;
  bounded_bitstring<1, 160, true, true> transport_layer_address;
  fixed_octstring<4, true>              ul_gtp_teid;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABToBeModifyItemBearerModReqExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct erab_to_be_modify_item_bearer_mod_req_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { transport_info, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::transport_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    transport_info_s&       transport_info() { return c; }
    const transport_info_s& transport_info() const { return c; }

  private:
    transport_info_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABToBeModifiedItemBearerModReq ::= SEQUENCE
struct erab_to_be_modified_item_bearer_mod_req_s {
  bool                                                                      ext             = false;
  bool                                                                      ie_exts_present = false;
  uint8_t                                                                   erab_id         = 0;
  erab_level_qos_params_s                                                   erab_level_qos_params;
  unbounded_octstring<true>                                                 nas_pdu;
  protocol_ext_container_l<erab_to_be_modify_item_bearer_mod_req_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABToBeModifiedItemBearerModReqIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_to_be_modified_item_bearer_mod_req_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_to_be_modified_item_bearer_mod_req, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_to_be_modified_item_bearer_mod_req; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_to_be_modified_item_bearer_mod_req_s&       erab_to_be_modified_item_bearer_mod_req() { return c; }
    const erab_to_be_modified_item_bearer_mod_req_s& erab_to_be_modified_item_bearer_mod_req() const { return c; }

  private:
    erab_to_be_modified_item_bearer_mod_req_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEAggregate-MaximumBitrates-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o ue_aggregate_maximum_bitrates_ext_ies_o;

// E-RABToBeModifiedListBearerModReq ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES :
// IEsSetParam}
using erab_to_be_modified_list_bearer_mod_req_l =
    dyn_array<protocol_ie_single_container_s<erab_to_be_modified_item_bearer_mod_req_ies_o> >;

typedef protocol_ext_container_empty_l ue_aggregate_maximum_bitrates_ext_ies_container;

// UEAggregateMaximumBitrate ::= SEQUENCE
struct ue_aggregate_maximum_bitrate_s {
  bool                                            ext                             = false;
  bool                                            ie_exts_present                 = false;
  uint64_t                                        ueaggregate_maximum_bit_rate_dl = 0;
  uint64_t                                        ueaggregate_maximum_bit_rate_ul = 0;
  ue_aggregate_maximum_bitrates_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABModifyRequestIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_modify_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        ueaggregate_maximum_bitrate,
        erab_to_be_modified_list_bearer_mod_req,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                        mme_ue_s1ap_id();
    uint32_t&                                        enb_ue_s1ap_id();
    ue_aggregate_maximum_bitrate_s&                  ueaggregate_maximum_bitrate();
    erab_to_be_modified_list_bearer_mod_req_l&       erab_to_be_modified_list_bearer_mod_req();
    const uint64_t&                                  mme_ue_s1ap_id() const;
    const uint32_t&                                  enb_ue_s1ap_id() const;
    const ue_aggregate_maximum_bitrate_s&            ueaggregate_maximum_bitrate() const;
    const erab_to_be_modified_list_bearer_mod_req_l& erab_to_be_modified_list_bearer_mod_req() const;

  private:
    types                                                                                      type_;
    choice_buffer_t<erab_to_be_modified_list_bearer_mod_req_l, ue_aggregate_maximum_bitrate_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct erab_modify_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       ueaggregate_maximum_bitrate_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<ue_aggregate_maximum_bitrate_s>                 ueaggregate_maximum_bitrate;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_to_be_modified_item_bearer_mod_req_ies_o>, 1, 256, true> >
      erab_to_be_modified_list_bearer_mod_req;

  // sequence methods
  erab_modify_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABModifyRequest ::= SEQUENCE
struct erab_modify_request_s {
  bool                              ext = false;
  erab_modify_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABModifyResponseIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_modify_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        erab_modify_list_bearer_mod_res,
        erab_failed_to_modify_list,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                mme_ue_s1ap_id();
    uint32_t&                                enb_ue_s1ap_id();
    erab_modify_list_bearer_mod_res_l&       erab_modify_list_bearer_mod_res();
    erab_list_l&                             erab_failed_to_modify_list();
    crit_diagnostics_s&                      crit_diagnostics();
    const uint64_t&                          mme_ue_s1ap_id() const;
    const uint32_t&                          enb_ue_s1ap_id() const;
    const erab_modify_list_bearer_mod_res_l& erab_modify_list_bearer_mod_res() const;
    const erab_list_l&                       erab_failed_to_modify_list() const;
    const crit_diagnostics_s&                crit_diagnostics() const;

  private:
    types                                                                               type_;
    choice_buffer_t<crit_diagnostics_s, erab_list_l, erab_modify_list_bearer_mod_res_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct erab_modify_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       erab_modify_list_bearer_mod_res_present = false;
  bool                                                       erab_failed_to_modify_list_present      = false;
  bool                                                       crit_diagnostics_present                = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_modify_item_bearer_mod_res_ies_o>, 1, 256, true> >
                                                                                         erab_modify_list_bearer_mod_res;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_item_ies_o>, 1, 256, true> > erab_failed_to_modify_list;
  ie_field_s<crit_diagnostics_s>                                                         crit_diagnostics;

  // sequence methods
  erab_modify_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABModifyResponse ::= SEQUENCE
struct erab_modify_resp_s {
  bool                           ext = false;
  erab_modify_resp_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABReleaseCommandIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_release_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        ueaggregate_maximum_bitrate,
        erab_to_be_released_list,
        nas_pdu,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                             mme_ue_s1ap_id();
    uint32_t&                             enb_ue_s1ap_id();
    ue_aggregate_maximum_bitrate_s&       ueaggregate_maximum_bitrate();
    erab_list_l&                          erab_to_be_released_list();
    unbounded_octstring<true>&            nas_pdu();
    const uint64_t&                       mme_ue_s1ap_id() const;
    const uint32_t&                       enb_ue_s1ap_id() const;
    const ue_aggregate_maximum_bitrate_s& ueaggregate_maximum_bitrate() const;
    const erab_list_l&                    erab_to_be_released_list() const;
    const unbounded_octstring<true>&      nas_pdu() const;

  private:
    types                                                                                    type_;
    choice_buffer_t<erab_list_l, ue_aggregate_maximum_bitrate_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct erab_release_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       ueaggregate_maximum_bitrate_present = false;
  bool                                                       nas_pdu_present                     = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<ue_aggregate_maximum_bitrate_s>                 ueaggregate_maximum_bitrate;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_item_ies_o>, 1, 256, true> > erab_to_be_released_list;
  ie_field_s<unbounded_octstring<true> >                                                 nas_pdu;

  // sequence methods
  erab_release_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABReleaseCommand ::= SEQUENCE
struct erab_release_cmd_s {
  bool                           ext = false;
  erab_release_cmd_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UserLocationInformation-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o user_location_info_ext_ies_o;

typedef protocol_ext_container_empty_l user_location_info_ext_ies_container;

// UserLocationInformation ::= SEQUENCE
struct user_location_info_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  eutran_cgi_s                         eutran_cgi;
  tai_s                                tai;
  user_location_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABReleaseIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_release_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, erab_released_list, user_location_info, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                   mme_ue_s1ap_id();
    uint32_t&                   enb_ue_s1ap_id();
    erab_list_l&                erab_released_list();
    user_location_info_s&       user_location_info();
    const uint64_t&             mme_ue_s1ap_id() const;
    const uint32_t&             enb_ue_s1ap_id() const;
    const erab_list_l&          erab_released_list() const;
    const user_location_info_s& user_location_info() const;

  private:
    types                                              type_;
    choice_buffer_t<erab_list_l, user_location_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct erab_release_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       user_location_info_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_item_ies_o>, 1, 256, true> > erab_released_list;
  ie_field_s<user_location_info_s>                                                       user_location_info;

  // sequence methods
  erab_release_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABReleaseIndication ::= SEQUENCE
struct erab_release_ind_s {
  bool                           ext = false;
  erab_release_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABReleaseItemBearerRelCompExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_release_item_bearer_rel_comp_ext_ies_o;

typedef protocol_ext_container_empty_l erab_release_item_bearer_rel_comp_ext_ies_container;

// E-RABReleaseItemBearerRelComp ::= SEQUENCE
struct erab_release_item_bearer_rel_comp_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  uint8_t                                             erab_id         = 0;
  erab_release_item_bearer_rel_comp_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABReleaseItemBearerRelCompIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_release_item_bearer_rel_comp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_release_item_bearer_rel_comp, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_release_item_bearer_rel_comp; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_release_item_bearer_rel_comp_s&       erab_release_item_bearer_rel_comp() { return c; }
    const erab_release_item_bearer_rel_comp_s& erab_release_item_bearer_rel_comp() const { return c; }

  private:
    erab_release_item_bearer_rel_comp_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABReleaseListBearerRelComp ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES :
// IEsSetParam}
using erab_release_list_bearer_rel_comp_l =
    dyn_array<protocol_ie_single_container_s<erab_release_item_bearer_rel_comp_ies_o> >;

// E-RABReleaseResponseIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_release_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        erab_release_list_bearer_rel_comp,
        erab_failed_to_release_list,
        crit_diagnostics,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                  mme_ue_s1ap_id();
    uint32_t&                                  enb_ue_s1ap_id();
    erab_release_list_bearer_rel_comp_l&       erab_release_list_bearer_rel_comp();
    erab_list_l&                               erab_failed_to_release_list();
    crit_diagnostics_s&                        crit_diagnostics();
    user_location_info_s&                      user_location_info();
    const uint64_t&                            mme_ue_s1ap_id() const;
    const uint32_t&                            enb_ue_s1ap_id() const;
    const erab_release_list_bearer_rel_comp_l& erab_release_list_bearer_rel_comp() const;
    const erab_list_l&                         erab_failed_to_release_list() const;
    const crit_diagnostics_s&                  crit_diagnostics() const;
    const user_location_info_s&                user_location_info() const;

  private:
    types                                                                                                       type_;
    choice_buffer_t<crit_diagnostics_s, erab_list_l, erab_release_list_bearer_rel_comp_l, user_location_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct erab_release_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       erab_release_list_bearer_rel_comp_present = false;
  bool                                                       erab_failed_to_release_list_present       = false;
  bool                                                       crit_diagnostics_present                  = false;
  bool                                                       user_location_info_present                = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_release_item_bearer_rel_comp_ies_o>, 1, 256, true> >
                                                                                         erab_release_list_bearer_rel_comp;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_item_ies_o>, 1, 256, true> > erab_failed_to_release_list;
  ie_field_s<crit_diagnostics_s>                                                         crit_diagnostics;
  ie_field_s<user_location_info_s>                                                       user_location_info;

  // sequence methods
  erab_release_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABReleaseResponse ::= SEQUENCE
struct erab_release_resp_s {
  bool                            ext = false;
  erab_release_resp_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABSetupItemBearerSUResExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_setup_item_bearer_su_res_ext_ies_o;

typedef protocol_ext_container_empty_l erab_setup_item_bearer_su_res_ext_ies_container;

// E-RABSetupItemBearerSURes ::= SEQUENCE
struct erab_setup_item_bearer_su_res_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  uint8_t                                         erab_id         = 0;
  bounded_bitstring<1, 160, true, true>           transport_layer_address;
  fixed_octstring<4, true>                        gtp_teid;
  erab_setup_item_bearer_su_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABSetupItemBearerSUResIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_setup_item_bearer_su_res_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_setup_item_bearer_su_res, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_setup_item_bearer_su_res; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_setup_item_bearer_su_res_s&       erab_setup_item_bearer_su_res() { return c; }
    const erab_setup_item_bearer_su_res_s& erab_setup_item_bearer_su_res() const { return c; }

  private:
    erab_setup_item_bearer_su_res_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABSetupItemCtxtSUResExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_setup_item_ctxt_su_res_ext_ies_o;

typedef protocol_ext_container_empty_l erab_setup_item_ctxt_su_res_ext_ies_container;

// E-RABSetupItemCtxtSURes ::= SEQUENCE
struct erab_setup_item_ctxt_su_res_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint8_t                                       erab_id         = 0;
  bounded_bitstring<1, 160, true, true>         transport_layer_address;
  fixed_octstring<4, true>                      gtp_teid;
  erab_setup_item_ctxt_su_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABSetupItemCtxtSUResIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_setup_item_ctxt_su_res_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_setup_item_ctxt_su_res, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_setup_item_ctxt_su_res; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_setup_item_ctxt_su_res_s&       erab_setup_item_ctxt_su_res() { return c; }
    const erab_setup_item_ctxt_su_res_s& erab_setup_item_ctxt_su_res() const { return c; }

  private:
    erab_setup_item_ctxt_su_res_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABSetupListBearerSURes ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES : IEsSetParam}
using erab_setup_list_bearer_su_res_l = dyn_array<protocol_ie_single_container_s<erab_setup_item_bearer_su_res_ies_o> >;

// E-RABSetupListCtxtSURes ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES : IEsSetParam}
using erab_setup_list_ctxt_su_res_l = dyn_array<protocol_ie_single_container_s<erab_setup_item_ctxt_su_res_ies_o> >;

// BearerType ::= ENUMERATED
struct bearer_type_opts {
  enum options { non_ip, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<bearer_type_opts, true> bearer_type_e;

// E-RABToBeSetupItemBearerSUReqExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct erab_to_be_setup_item_bearer_su_req_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { correlation_id, sipto_correlation_id, bearer_type, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_octstring<4, true>&       correlation_id();
    fixed_octstring<4, true>&       sipto_correlation_id();
    bearer_type_e&                  bearer_type();
    const fixed_octstring<4, true>& correlation_id() const;
    const fixed_octstring<4, true>& sipto_correlation_id() const;
    const bearer_type_e&            bearer_type() const;

  private:
    types                                      type_;
    choice_buffer_t<fixed_octstring<4, true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct erab_to_be_setup_item_bearer_su_req_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                  correlation_id_present       = false;
  bool                                  sipto_correlation_id_present = false;
  bool                                  bearer_type_present          = false;
  ie_field_s<fixed_octstring<4, true> > correlation_id;
  ie_field_s<fixed_octstring<4, true> > sipto_correlation_id;
  ie_field_s<bearer_type_e>             bearer_type;

  // sequence methods
  erab_to_be_setup_item_bearer_su_req_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABToBeSetupItemBearerSUReq ::= SEQUENCE
struct erab_to_be_setup_item_bearer_su_req_s {
  bool                                                  ext             = false;
  bool                                                  ie_exts_present = false;
  uint8_t                                               erab_id         = 0;
  erab_level_qos_params_s                               erab_level_qos_params;
  bounded_bitstring<1, 160, true, true>                 transport_layer_address;
  fixed_octstring<4, true>                              gtp_teid;
  unbounded_octstring<true>                             nas_pdu;
  erab_to_be_setup_item_bearer_su_req_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABToBeSetupItemBearerSUReqIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_to_be_setup_item_bearer_su_req_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_to_be_setup_item_bearer_su_req, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_to_be_setup_item_bearer_su_req; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_to_be_setup_item_bearer_su_req_s&       erab_to_be_setup_item_bearer_su_req() { return c; }
    const erab_to_be_setup_item_bearer_su_req_s& erab_to_be_setup_item_bearer_su_req() const { return c; }

  private:
    erab_to_be_setup_item_bearer_su_req_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABToBeSetupListBearerSUReq ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES :
// IEsSetParam}
using erab_to_be_setup_list_bearer_su_req_l =
    dyn_array<protocol_ie_single_container_s<erab_to_be_setup_item_bearer_su_req_ies_o> >;

// E-RABSetupRequestIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        ueaggregate_maximum_bitrate,
        erab_to_be_setup_list_bearer_su_req,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                    mme_ue_s1ap_id();
    uint32_t&                                    enb_ue_s1ap_id();
    ue_aggregate_maximum_bitrate_s&              ueaggregate_maximum_bitrate();
    erab_to_be_setup_list_bearer_su_req_l&       erab_to_be_setup_list_bearer_su_req();
    const uint64_t&                              mme_ue_s1ap_id() const;
    const uint32_t&                              enb_ue_s1ap_id() const;
    const ue_aggregate_maximum_bitrate_s&        ueaggregate_maximum_bitrate() const;
    const erab_to_be_setup_list_bearer_su_req_l& erab_to_be_setup_list_bearer_su_req() const;

  private:
    types                                                                                  type_;
    choice_buffer_t<erab_to_be_setup_list_bearer_su_req_l, ue_aggregate_maximum_bitrate_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct erab_setup_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       ueaggregate_maximum_bitrate_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<ue_aggregate_maximum_bitrate_s>                 ueaggregate_maximum_bitrate;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_to_be_setup_item_bearer_su_req_ies_o>, 1, 256, true> >
      erab_to_be_setup_list_bearer_su_req;

  // sequence methods
  erab_setup_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABSetupRequest ::= SEQUENCE
struct erab_setup_request_s {
  bool                             ext = false;
  erab_setup_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABSetupResponseIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        erab_setup_list_bearer_su_res,
        erab_failed_to_setup_list_bearer_su_res,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                              mme_ue_s1ap_id();
    uint32_t&                              enb_ue_s1ap_id();
    erab_setup_list_bearer_su_res_l&       erab_setup_list_bearer_su_res();
    erab_list_l&                           erab_failed_to_setup_list_bearer_su_res();
    crit_diagnostics_s&                    crit_diagnostics();
    const uint64_t&                        mme_ue_s1ap_id() const;
    const uint32_t&                        enb_ue_s1ap_id() const;
    const erab_setup_list_bearer_su_res_l& erab_setup_list_bearer_su_res() const;
    const erab_list_l&                     erab_failed_to_setup_list_bearer_su_res() const;
    const crit_diagnostics_s&              crit_diagnostics() const;

  private:
    types                                                                             type_;
    choice_buffer_t<crit_diagnostics_s, erab_list_l, erab_setup_list_bearer_su_res_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct erab_setup_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       erab_setup_list_bearer_su_res_present           = false;
  bool                                                       erab_failed_to_setup_list_bearer_su_res_present = false;
  bool                                                       crit_diagnostics_present                        = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_setup_item_bearer_su_res_ies_o>, 1, 256, true> >
      erab_setup_list_bearer_su_res;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_item_ies_o>, 1, 256, true> >
                                 erab_failed_to_setup_list_bearer_su_res;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  erab_setup_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABSetupResponse ::= SEQUENCE
struct erab_setup_resp_s {
  bool                          ext = false;
  erab_setup_resp_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABToBeSetupItemCtxtSUReqExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct erab_to_be_setup_item_ctxt_su_req_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { correlation_id, sipto_correlation_id, bearer_type, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_octstring<4, true>&       correlation_id();
    fixed_octstring<4, true>&       sipto_correlation_id();
    bearer_type_e&                  bearer_type();
    const fixed_octstring<4, true>& correlation_id() const;
    const fixed_octstring<4, true>& sipto_correlation_id() const;
    const bearer_type_e&            bearer_type() const;

  private:
    types                                      type_;
    choice_buffer_t<fixed_octstring<4, true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct erab_to_be_setup_item_ctxt_su_req_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                  correlation_id_present       = false;
  bool                                  sipto_correlation_id_present = false;
  bool                                  bearer_type_present          = false;
  ie_field_s<fixed_octstring<4, true> > correlation_id;
  ie_field_s<fixed_octstring<4, true> > sipto_correlation_id;
  ie_field_s<bearer_type_e>             bearer_type;

  // sequence methods
  erab_to_be_setup_item_ctxt_su_req_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABToBeSetupItemCtxtSUReq ::= SEQUENCE
struct erab_to_be_setup_item_ctxt_su_req_s {
  bool                                                ext             = false;
  bool                                                nas_pdu_present = false;
  bool                                                ie_exts_present = false;
  uint8_t                                             erab_id         = 0;
  erab_level_qos_params_s                             erab_level_qos_params;
  bounded_bitstring<1, 160, true, true>               transport_layer_address;
  fixed_octstring<4, true>                            gtp_teid;
  unbounded_octstring<true>                           nas_pdu;
  erab_to_be_setup_item_ctxt_su_req_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABToBeSetupItemCtxtSUReqIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_to_be_setup_item_ctxt_su_req_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_to_be_setup_item_ctxt_su_req, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_to_be_setup_item_ctxt_su_req; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_to_be_setup_item_ctxt_su_req_s&       erab_to_be_setup_item_ctxt_su_req() { return c; }
    const erab_to_be_setup_item_ctxt_su_req_s& erab_to_be_setup_item_ctxt_su_req() const { return c; }

  private:
    erab_to_be_setup_item_ctxt_su_req_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Data-Forwarding-Not-Possible ::= ENUMERATED
struct data_forwarding_not_possible_opts {
  enum options { data_forwarding_not_possible, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<data_forwarding_not_possible_opts, true> data_forwarding_not_possible_e;

// E-RABToBeSetupItemHOReq-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct erab_to_be_setup_item_ho_req_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { data_forwarding_not_possible, bearer_type, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    data_forwarding_not_possible_e&       data_forwarding_not_possible();
    bearer_type_e&                        bearer_type();
    const data_forwarding_not_possible_e& data_forwarding_not_possible() const;
    const bearer_type_e&                  bearer_type() const;

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct erab_to_be_setup_item_ho_req_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                       data_forwarding_not_possible_present = false;
  bool                                       bearer_type_present                  = false;
  ie_field_s<data_forwarding_not_possible_e> data_forwarding_not_possible;
  ie_field_s<bearer_type_e>                  bearer_type;

  // sequence methods
  erab_to_be_setup_item_ho_req_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABToBeSetupItemHOReq ::= SEQUENCE
struct erab_to_be_setup_item_ho_req_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  uint8_t                                        erab_id         = 0;
  bounded_bitstring<1, 160, true, true>          transport_layer_address;
  fixed_octstring<4, true>                       gtp_teid;
  erab_level_qos_params_s                        erab_level_qos_params;
  erab_to_be_setup_item_ho_req_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABToBeSetupItemHOReqIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_to_be_setup_item_ho_req_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_to_be_setup_item_ho_req, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_to_be_setup_item_ho_req; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_to_be_setup_item_ho_req_s&       erab_to_be_setup_item_ho_req() { return c; }
    const erab_to_be_setup_item_ho_req_s& erab_to_be_setup_item_ho_req() const { return c; }

  private:
    erab_to_be_setup_item_ho_req_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABToBeSetupListCtxtSUReq ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES :
// IEsSetParam}
using erab_to_be_setup_list_ctxt_su_req_l =
    dyn_array<protocol_ie_single_container_s<erab_to_be_setup_item_ctxt_su_req_ies_o> >;

// E-RABToBeSwitchedDLItem-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_to_be_switched_dl_item_ext_ies_o;

typedef protocol_ext_container_empty_l erab_to_be_switched_dl_item_ext_ies_container;

// E-RABToBeSwitchedDLItem ::= SEQUENCE
struct erab_to_be_switched_dl_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint8_t                                       erab_id         = 0;
  bounded_bitstring<1, 160, true, true>         transport_layer_address;
  fixed_octstring<4, true>                      gtp_teid;
  erab_to_be_switched_dl_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABToBeSwitchedDLItemIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_to_be_switched_dl_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_to_be_switched_dl_item, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_to_be_switched_dl_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_to_be_switched_dl_item_s&       erab_to_be_switched_dl_item() { return c; }
    const erab_to_be_switched_dl_item_s& erab_to_be_switched_dl_item() const { return c; }

  private:
    erab_to_be_switched_dl_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-RABToBeSwitchedULItem-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o erab_to_be_switched_ul_item_ext_ies_o;

typedef protocol_ext_container_empty_l erab_to_be_switched_ul_item_ext_ies_container;

// E-RABToBeSwitchedULItem ::= SEQUENCE
struct erab_to_be_switched_ul_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint8_t                                       erab_id         = 0;
  bounded_bitstring<1, 160, true, true>         transport_layer_address;
  fixed_octstring<4, true>                      gtp_teid;
  erab_to_be_switched_ul_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABToBeSwitchedULItemIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct erab_to_be_switched_ul_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { erab_to_be_switched_ul_item, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::erab_to_be_switched_ul_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    erab_to_be_switched_ul_item_s&       erab_to_be_switched_ul_item() { return c; }
    const erab_to_be_switched_ul_item_s& erab_to_be_switched_ul_item() const { return c; }

  private:
    erab_to_be_switched_ul_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ECGI-List ::= SEQUENCE (SIZE (1..256)) OF EUTRAN-CGI
using ecgi_list_l = dyn_array<eutran_cgi_s>;

// ECGIListForRestart ::= SEQUENCE (SIZE (1..256)) OF EUTRAN-CGI
using ecgi_list_for_restart_l = dyn_array<eutran_cgi_s>;

// EHRPDMultiSectorLoadReportingResponseItem ::= SEQUENCE
struct ehrpd_multi_sector_load_report_resp_item_s {
  bool                            ext = false;
  fixed_octstring<16, true>       ehrpd_sector_id;
  ehrpd_sector_load_report_resp_s ehrpd_sector_load_report_resp;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ENB-StatusTransfer-TransparentContainer-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o enb_status_transfer_transparent_container_ext_ies_o;

typedef protocol_ext_container_empty_l enb_status_transfer_transparent_container_ext_ies_container;

// ENB-StatusTransfer-TransparentContainer ::= SEQUENCE
struct enb_status_transfer_transparent_container_s {
  bool                                                        ext             = false;
  bool                                                        ie_exts_present = false;
  bearers_subject_to_status_transfer_list_l                   bearers_subject_to_status_transfer_list;
  enb_status_transfer_transparent_container_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// S-TMSI-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o s_tmsi_ext_ies_o;

// UL-CP-SecurityInformation-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o ul_cp_security_info_ext_ies_o;

typedef protocol_ext_container_empty_l s_tmsi_ext_ies_container;

// S-TMSI ::= SEQUENCE
struct s_tmsi_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<1, true> mmec;
  fixed_octstring<4, true> m_tmsi;
  s_tmsi_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l ul_cp_security_info_ext_ies_container;

// UL-CP-SecurityInformation ::= SEQUENCE
struct ul_cp_security_info_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  fixed_bitstring<16, false, true>      ul_nas_mac;
  fixed_bitstring<5, false, true>       ul_nas_count;
  ul_cp_security_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ENBCPRelocationIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct enbcp_relocation_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { enb_ue_s1ap_id, s_tmsi, eutran_cgi, tai, ul_cp_security_info, nulltype } value;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&                    enb_ue_s1ap_id();
    s_tmsi_s&                    s_tmsi();
    eutran_cgi_s&                eutran_cgi();
    tai_s&                       tai();
    ul_cp_security_info_s&       ul_cp_security_info();
    const uint32_t&              enb_ue_s1ap_id() const;
    const s_tmsi_s&              s_tmsi() const;
    const eutran_cgi_s&          eutran_cgi() const;
    const tai_s&                 tai() const;
    const ul_cp_security_info_s& ul_cp_security_info() const;

  private:
    types                                                                 type_;
    choice_buffer_t<eutran_cgi_s, s_tmsi_s, tai_s, ul_cp_security_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct enbcp_relocation_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> > enb_ue_s1ap_id;
  ie_field_s<s_tmsi_s>                                     s_tmsi;
  ie_field_s<eutran_cgi_s>                                 eutran_cgi;
  ie_field_s<tai_s>                                        tai;
  ie_field_s<ul_cp_security_info_s>                        ul_cp_security_info;

  // sequence methods
  enbcp_relocation_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ENBCPRelocationIndication ::= SEQUENCE
struct enbcp_relocation_ind_s {
  bool                               ext = false;
  enbcp_relocation_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ENBX2ExtTLA-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o enbx2_ext_tla_ext_ies_o;

// ENBX2GTPTLAs ::= SEQUENCE (SIZE (1..16)) OF BIT STRING (SIZE (1..160,...))
using enbx2_gtptlas_l = bounded_array<bounded_bitstring<1, 160, true, true>, 16>;

typedef protocol_ext_container_empty_l enbx2_ext_tla_ext_ies_container;

// ENBX2ExtTLA ::= SEQUENCE
struct enbx2_ext_tla_s {
  bool                                  ext               = false;
  bool                                  ipsec_tla_present = false;
  bool                                  gtptl_aa_present  = false;
  bool                                  ie_exts_present   = false;
  bounded_bitstring<1, 160, true, true> ipsec_tla;
  enbx2_gtptlas_l                       gtptl_aa;
  enbx2_ext_tla_ext_ies_container       ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MutingAvailabilityIndication ::= ENUMERATED
struct muting_availability_ind_opts {
  enum options { available, unavailable, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<muting_availability_ind_opts, true> muting_availability_ind_e;

// RLFReportInformation-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o rlf_report_info_ext_ies_o;

// ENBIndirectX2TransportLayerAddresses ::= SEQUENCE (SIZE (1..2)) OF BIT STRING (SIZE (1..160,...))
using enb_indirect_x2_transport_layer_addresses_l = bounded_array<bounded_bitstring<1, 160, true, true>, 2>;

// ENBX2ExtTLAs ::= SEQUENCE (SIZE (1..16)) OF ENBX2ExtTLA
using enbx2_ext_tlas_l = dyn_array<enbx2_ext_tla_s>;

// MutingPatternInformation-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o muting_pattern_info_ext_ies_o;

typedef protocol_ext_container_empty_l rlf_report_info_ext_ies_container;

// RLFReportInformation ::= SEQUENCE
struct rlf_report_info_s {
  bool                              ext                                                = false;
  bool                              ue_rlf_report_container_for_extended_bands_present = false;
  bool                              ie_exts_present                                    = false;
  unbounded_octstring<true>         ue_rlf_report_container;
  unbounded_octstring<true>         ue_rlf_report_container_for_extended_bands;
  rlf_report_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SynchronisationStatus ::= ENUMERATED
struct synchronisation_status_opts {
  enum options { sync, async, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<synchronisation_status_opts, true> synchronisation_status_e;

// TimeSynchronisationInfo-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct time_synchronisation_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { muting_availability_ind, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::muting_availability_ind; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    muting_availability_ind_e&       muting_availability_ind() { return c; }
    const muting_availability_ind_e& muting_availability_ind() const { return c; }

  private:
    muting_availability_ind_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ENBX2TLAs ::= SEQUENCE (SIZE (1..2)) OF BIT STRING (SIZE (1..160,...))
using enbx2_tlas_l = bounded_array<bounded_bitstring<1, 160, true, true>, 2>;

// ListeningSubframePattern-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o listening_sf_pattern_ext_ies_o;

typedef protocol_ext_container_empty_l muting_pattern_info_ext_ies_container;

// MutingPatternInformation ::= SEQUENCE
struct muting_pattern_info_s {
  struct muting_pattern_period_opts {
    enum options { ms0, ms1280, ms2560, ms5120, ms10240, /*...*/ nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<muting_pattern_period_opts, true> muting_pattern_period_e_;

  // member variables
  bool                                  ext                           = false;
  bool                                  muting_pattern_offset_present = false;
  bool                                  ie_exts_present               = false;
  muting_pattern_period_e_              muting_pattern_period;
  uint16_t                              muting_pattern_offset = 0;
  muting_pattern_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SONInformationReport ::= CHOICE
struct son_info_report_c {
  struct types_opts {
    enum options { rlf_report_info, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  types       type() const { return types::rlf_report_info; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  rlf_report_info_s&       rlf_report_info() { return c; }
  const rlf_report_info_s& rlf_report_info() const { return c; }

private:
  rlf_report_info_s c;
};

// TimeSynchronisationInfo ::= SEQUENCE
struct time_synchronisation_info_s {
  bool                                                          ext             = false;
  bool                                                          ie_exts_present = false;
  uint8_t                                                       stratum_level   = 0;
  synchronisation_status_e                                      synchronisation_status;
  protocol_ext_container_l<time_synchronisation_info_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// X2TNLConfigurationInfo-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct x2_tnl_cfg_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        enbx2_extended_transport_layer_addresses,
        enb_indirect_x2_transport_layer_addresses,
        nulltype
      } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    enbx2_ext_tlas_l&                                  enbx2_extended_transport_layer_addresses();
    enb_indirect_x2_transport_layer_addresses_l&       enb_indirect_x2_transport_layer_addresses();
    const enbx2_ext_tlas_l&                            enbx2_extended_transport_layer_addresses() const;
    const enb_indirect_x2_transport_layer_addresses_l& enb_indirect_x2_transport_layer_addresses() const;

  private:
    types                                                                          type_;
    choice_buffer_t<enb_indirect_x2_transport_layer_addresses_l, enbx2_ext_tlas_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ENB-ID ::= CHOICE
struct enb_id_c {
  struct types_opts {
    enum options { macro_enb_id, home_enb_id, /*...*/ short_macro_enb_id, long_macro_enb_id, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true, 2> types;

  // choice methods
  enb_id_c() = default;
  enb_id_c(const enb_id_c& other);
  enb_id_c& operator=(const enb_id_c& other);
  ~enb_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<20, false, true>& macro_enb_id()
  {
    assert_choice_type("macroENB-ID", type_.to_string(), "ENB-ID");
    return c.get<fixed_bitstring<20, false, true> >();
  }
  fixed_bitstring<28, false, true>& home_enb_id()
  {
    assert_choice_type("homeENB-ID", type_.to_string(), "ENB-ID");
    return c.get<fixed_bitstring<28, false, true> >();
  }
  fixed_bitstring<18, false, true>& short_macro_enb_id()
  {
    assert_choice_type("short-macroENB-ID", type_.to_string(), "ENB-ID");
    return c.get<fixed_bitstring<18, false, true> >();
  }
  fixed_bitstring<21, false, true>& long_macro_enb_id()
  {
    assert_choice_type("long-macroENB-ID", type_.to_string(), "ENB-ID");
    return c.get<fixed_bitstring<21, false, true> >();
  }
  const fixed_bitstring<20, false, true>& macro_enb_id() const
  {
    assert_choice_type("macroENB-ID", type_.to_string(), "ENB-ID");
    return c.get<fixed_bitstring<20, false, true> >();
  }
  const fixed_bitstring<28, false, true>& home_enb_id() const
  {
    assert_choice_type("homeENB-ID", type_.to_string(), "ENB-ID");
    return c.get<fixed_bitstring<28, false, true> >();
  }
  const fixed_bitstring<18, false, true>& short_macro_enb_id() const
  {
    assert_choice_type("short-macroENB-ID", type_.to_string(), "ENB-ID");
    return c.get<fixed_bitstring<18, false, true> >();
  }
  const fixed_bitstring<21, false, true>& long_macro_enb_id() const
  {
    assert_choice_type("long-macroENB-ID", type_.to_string(), "ENB-ID");
    return c.get<fixed_bitstring<21, false, true> >();
  }
  fixed_bitstring<20, false, true>& set_macro_enb_id()
  {
    set(types::macro_enb_id);
    return c.get<fixed_bitstring<20, false, true> >();
  }
  fixed_bitstring<28, false, true>& set_home_enb_id()
  {
    set(types::home_enb_id);
    return c.get<fixed_bitstring<28, false, true> >();
  }
  fixed_bitstring<18, false, true>& set_short_macro_enb_id()
  {
    set(types::short_macro_enb_id);
    return c.get<fixed_bitstring<18, false, true> >();
  }
  fixed_bitstring<21, false, true>& set_long_macro_enb_id()
  {
    set(types::long_macro_enb_id);
    return c.get<fixed_bitstring<21, false, true> >();
  }

private:
  types                                              type_;
  choice_buffer_t<fixed_bitstring<28, false, true> > c;

  void destroy_();
};

// GlobalENB-ID-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o global_enb_id_ext_ies_o;

typedef protocol_ext_container_empty_l listening_sf_pattern_ext_ies_container;

// ListeningSubframePattern ::= SEQUENCE
struct listening_sf_pattern_s {
  struct pattern_period_opts {
    enum options { ms1280, ms2560, ms5120, ms10240, /*...*/ nulltype } value;
    typedef uint16_t number_type;

    std::string to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<pattern_period_opts, true> pattern_period_e_;

  // member variables
  bool                                   ext             = false;
  bool                                   ie_exts_present = false;
  pattern_period_e_                      pattern_period;
  uint16_t                               pattern_offset = 0;
  listening_sf_pattern_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SONInformation-ExtensionIE ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct son_info_ext_ie_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { son_info_report, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::son_info_report; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    son_info_report_c&       son_info_report() { return c; }
    const son_info_report_c& son_info_report() const { return c; }

  private:
    son_info_report_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SONInformationReply-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct son_info_reply_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { time_synchronisation_info, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::time_synchronisation_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    time_synchronisation_info_s&       time_synchronisation_info() { return c; }
    const time_synchronisation_info_s& time_synchronisation_info() const { return c; }

  private:
    time_synchronisation_info_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SynchronisationInformation-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o synchronisation_info_ext_ies_o;

struct x2_tnl_cfg_info_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                                  enbx2_extended_transport_layer_addresses_present  = false;
  bool                                                  enb_indirect_x2_transport_layer_addresses_present = false;
  ie_field_s<dyn_seq_of<enbx2_ext_tla_s, 1, 16, true> > enbx2_extended_transport_layer_addresses;
  ie_field_s<dyn_seq_of<bounded_bitstring<1, 160, true, true>, 1, 2, true> > enb_indirect_x2_transport_layer_addresses;

  // sequence methods
  x2_tnl_cfg_info_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// X2TNLConfigurationInfo ::= SEQUENCE
struct x2_tnl_cfg_info_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  enbx2_tlas_l                      enbx2_transport_layer_addresses;
  x2_tnl_cfg_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l global_enb_id_ext_ies_container;

// Global-ENB-ID ::= SEQUENCE
struct global_enb_id_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  fixed_octstring<3, true>        plm_nid;
  enb_id_c                        enb_id;
  global_enb_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SONInformationReply ::= SEQUENCE
struct son_info_reply_s {
  bool                                               ext                     = false;
  bool                                               x2_tnl_cfg_info_present = false;
  bool                                               ie_exts_present         = false;
  x2_tnl_cfg_info_s                                  x2_tnl_cfg_info;
  protocol_ext_container_l<son_info_reply_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SONInformationRequest ::= ENUMERATED
struct son_info_request_opts {
  enum options { x2_tnl_cfg_info, /*...*/ time_synchronisation_info, activ_muting, deactiv_muting, nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<son_info_request_opts, true, 3> son_info_request_e;

// SourceeNB-ID-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o sourceenb_id_ext_ies_o;

typedef protocol_ext_container_empty_l synchronisation_info_ext_ies_container;

// SynchronisationInformation ::= SEQUENCE
struct synchronisation_info_s {
  bool                                   ext                          = false;
  bool                                   source_stratum_level_present = false;
  bool                                   listening_sf_pattern_present = false;
  bool                                   aggressore_cgi_list_present  = false;
  bool                                   ie_exts_present              = false;
  uint8_t                                source_stratum_level         = 0;
  listening_sf_pattern_s                 listening_sf_pattern;
  ecgi_list_l                            aggressore_cgi_list;
  synchronisation_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargeteNB-ID-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o targetenb_id_ext_ies_o;

// SONConfigurationTransfer-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct son_cfg_transfer_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { x2_tnl_cfg_info, synchronisation_info, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    x2_tnl_cfg_info_s&            x2_tnl_cfg_info();
    synchronisation_info_s&       synchronisation_info();
    const x2_tnl_cfg_info_s&      x2_tnl_cfg_info() const;
    const synchronisation_info_s& synchronisation_info() const;

  private:
    types                                                      type_;
    choice_buffer_t<synchronisation_info_s, x2_tnl_cfg_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SONInformation ::= CHOICE
struct son_info_c {
  struct types_opts {
    enum options { son_info_request, son_info_reply, /*...*/ son_info_ext, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true, 1> types;

  // choice methods
  son_info_c() = default;
  son_info_c(const son_info_c& other);
  son_info_c& operator=(const son_info_c& other);
  ~son_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
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
  protocol_ie_single_container_s<son_info_ext_ie_o>& son_info_ext()
  {
    assert_choice_type("sONInformation-Extension", type_.to_string(), "SONInformation");
    return c.get<protocol_ie_single_container_s<son_info_ext_ie_o> >();
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
  const protocol_ie_single_container_s<son_info_ext_ie_o>& son_info_ext() const
  {
    assert_choice_type("sONInformation-Extension", type_.to_string(), "SONInformation");
    return c.get<protocol_ie_single_container_s<son_info_ext_ie_o> >();
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
  protocol_ie_single_container_s<son_info_ext_ie_o>& set_son_info_ext()
  {
    set(types::son_info_ext);
    return c.get<protocol_ie_single_container_s<son_info_ext_ie_o> >();
  }

private:
  types                                                                                type_;
  choice_buffer_t<protocol_ie_single_container_s<son_info_ext_ie_o>, son_info_reply_s> c;

  void destroy_();
};

typedef protocol_ext_container_empty_l sourceenb_id_ext_ies_container;

// SourceeNB-ID ::= SEQUENCE
struct sourceenb_id_s {
  bool                           ie_exts_present = false;
  global_enb_id_s                global_enb_id;
  tai_s                          sel_tai;
  sourceenb_id_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l targetenb_id_ext_ies_container;

// TargeteNB-ID ::= SEQUENCE
struct targetenb_id_s {
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  global_enb_id_s                global_enb_id;
  tai_s                          sel_tai;
  targetenb_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct son_cfg_transfer_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                               x2_tnl_cfg_info_present      = false;
  bool                               synchronisation_info_present = false;
  ie_field_s<x2_tnl_cfg_info_s>      x2_tnl_cfg_info;
  ie_field_s<synchronisation_info_s> synchronisation_info;

  // sequence methods
  son_cfg_transfer_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SONConfigurationTransfer ::= SEQUENCE
struct son_cfg_transfer_s {
  bool                               ext             = false;
  bool                               ie_exts_present = false;
  targetenb_id_s                     targetenb_id;
  sourceenb_id_s                     sourceenb_id;
  son_info_c                         son_info;
  son_cfg_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ENBConfigurationTransferIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct enb_cfg_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { son_cfg_transfer_ect, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::son_cfg_transfer_ect; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    son_cfg_transfer_s&       son_cfg_transfer_ect() { return c; }
    const son_cfg_transfer_s& son_cfg_transfer_ect() const { return c; }

  private:
    son_cfg_transfer_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ENBConfigurationTransfer ::= SEQUENCE
struct enb_cfg_transfer_s {
  bool                                            ext = false;
  protocol_ie_container_l<enb_cfg_transfer_ies_o> protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAT-Type ::= ENUMERATED
struct rat_type_opts {
  enum options { nbiot, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<rat_type_opts, true> rat_type_e;

// SupportedTAs-Item-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct supported_tas_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { rat_type, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::rat_type; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rat_type_e&       rat_type() { return c; }
    const rat_type_e& rat_type() const { return c; }

  private:
    rat_type_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SupportedTAs-Item ::= SEQUENCE
struct supported_tas_item_s {
  bool                                                   ext             = false;
  bool                                                   ie_exts_present = false;
  fixed_octstring<2, true>                               tac;
  bplmns_l                                               broadcast_plmns;
  protocol_ext_container_l<supported_tas_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NB-IoT-DefaultPagingDRX ::= ENUMERATED
struct nb_io_t_default_paging_drx_opts {
  enum options { v128, v256, v512, v1024, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<nb_io_t_default_paging_drx_opts, true> nb_io_t_default_paging_drx_e;

// PagingDRX ::= ENUMERATED
struct paging_drx_opts {
  enum options { v32, v64, v128, v256, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<paging_drx_opts, true> paging_drx_e;

// SupportedTAs ::= SEQUENCE (SIZE (1..256)) OF SupportedTAs-Item
using supported_tas_l = dyn_array<supported_tas_item_s>;

// ENBConfigurationUpdateIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct enb_cfg_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        enbname,
        supported_tas,
        csg_id_list,
        default_paging_drx,
        nb_io_t_default_paging_drx,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    printable_string<1, 150, true, true>&       enbname();
    supported_tas_l&                            supported_tas();
    csg_id_list_l&                              csg_id_list();
    paging_drx_e&                               default_paging_drx();
    nb_io_t_default_paging_drx_e&               nb_io_t_default_paging_drx();
    const printable_string<1, 150, true, true>& enbname() const;
    const supported_tas_l&                      supported_tas() const;
    const csg_id_list_l&                        csg_id_list() const;
    const paging_drx_e&                         default_paging_drx() const;
    const nb_io_t_default_paging_drx_e&         nb_io_t_default_paging_drx() const;

  private:
    types                                                                                 type_;
    choice_buffer_t<csg_id_list_l, printable_string<1, 150, true, true>, supported_tas_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct enb_cfg_upd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                        enbname_present                    = false;
  bool                                                        supported_tas_present              = false;
  bool                                                        csg_id_list_present                = false;
  bool                                                        default_paging_drx_present         = false;
  bool                                                        nb_io_t_default_paging_drx_present = false;
  ie_field_s<printable_string<1, 150, true, true> >           enbname;
  ie_field_s<dyn_seq_of<supported_tas_item_s, 1, 256, true> > supported_tas;
  ie_field_s<dyn_seq_of<csg_id_list_item_s, 1, 256, true> >   csg_id_list;
  ie_field_s<paging_drx_e>                                    default_paging_drx;
  ie_field_s<nb_io_t_default_paging_drx_e>                    nb_io_t_default_paging_drx;

  // sequence methods
  enb_cfg_upd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ENBConfigurationUpdate ::= SEQUENCE
struct enb_cfg_upd_s {
  bool                      ext = false;
  enb_cfg_upd_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ENBConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct enb_cfg_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::crit_diagnostics; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
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

// ENBConfigurationUpdateAcknowledge ::= SEQUENCE
struct enb_cfg_upd_ack_s {
  bool                                           ext = false;
  protocol_ie_container_l<enb_cfg_upd_ack_ies_o> protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
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

// ENBConfigurationUpdateFailureIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct enb_cfg_upd_fail_ies_o {
  // Value ::= OPEN TYPE
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

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

struct enb_cfg_upd_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                           time_to_wait_present     = false;
  bool                           crit_diagnostics_present = false;
  ie_field_s<cause_c>            cause;
  ie_field_s<time_to_wait_e>     time_to_wait;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  enb_cfg_upd_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ENBConfigurationUpdateFailure ::= SEQUENCE
struct enb_cfg_upd_fail_s {
  bool                           ext = false;
  enb_cfg_upd_fail_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LAI-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o lai_ext_ies_o;

// GERAN-Cell-ID-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o geran_cell_id_ext_ies_o;

typedef protocol_ext_container_empty_l lai_ext_ies_container;

// LAI ::= SEQUENCE
struct lai_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plm_nid;
  fixed_octstring<2, true> lac;
  lai_ext_ies_container    ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargetRNC-ID-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o target_rnc_id_ext_ies_o;

typedef protocol_ext_container_empty_l geran_cell_id_ext_ies_container;

// GERAN-Cell-ID ::= SEQUENCE
struct geran_cell_id_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  lai_s                           lai;
  fixed_octstring<1, true>        rac;
  fixed_octstring<2, true>        ci;
  geran_cell_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l target_rnc_id_ext_ies_container;

// TargetRNC-ID ::= SEQUENCE
struct target_rnc_id_s {
  bool                            ext                     = false;
  bool                            rac_present             = false;
  bool                            extended_rnc_id_present = false;
  bool                            ie_exts_present         = false;
  lai_s                           lai;
  fixed_octstring<1, true>        rac;
  uint16_t                        rnc_id          = 0;
  uint32_t                        extended_rnc_id = 4096;
  target_rnc_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RIMRoutingAddress ::= CHOICE
struct rim_routing_address_c {
  struct types_opts {
    enum options { geran_cell_id, /*...*/ target_rnc_id, ehrpd_sector_id, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true, 2> types;

  // choice methods
  rim_routing_address_c() = default;
  rim_routing_address_c(const rim_routing_address_c& other);
  rim_routing_address_c& operator=(const rim_routing_address_c& other);
  ~rim_routing_address_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  geran_cell_id_s& geran_cell_id()
  {
    assert_choice_type("gERAN-Cell-ID", type_.to_string(), "RIMRoutingAddress");
    return c.get<geran_cell_id_s>();
  }
  target_rnc_id_s& target_rnc_id()
  {
    assert_choice_type("targetRNC-ID", type_.to_string(), "RIMRoutingAddress");
    return c.get<target_rnc_id_s>();
  }
  fixed_octstring<16, true>& ehrpd_sector_id()
  {
    assert_choice_type("eHRPD-Sector-ID", type_.to_string(), "RIMRoutingAddress");
    return c.get<fixed_octstring<16, true> >();
  }
  const geran_cell_id_s& geran_cell_id() const
  {
    assert_choice_type("gERAN-Cell-ID", type_.to_string(), "RIMRoutingAddress");
    return c.get<geran_cell_id_s>();
  }
  const target_rnc_id_s& target_rnc_id() const
  {
    assert_choice_type("targetRNC-ID", type_.to_string(), "RIMRoutingAddress");
    return c.get<target_rnc_id_s>();
  }
  const fixed_octstring<16, true>& ehrpd_sector_id() const
  {
    assert_choice_type("eHRPD-Sector-ID", type_.to_string(), "RIMRoutingAddress");
    return c.get<fixed_octstring<16, true> >();
  }
  geran_cell_id_s& set_geran_cell_id()
  {
    set(types::geran_cell_id);
    return c.get<geran_cell_id_s>();
  }
  target_rnc_id_s& set_target_rnc_id()
  {
    set(types::target_rnc_id);
    return c.get<target_rnc_id_s>();
  }
  fixed_octstring<16, true>& set_ehrpd_sector_id()
  {
    set(types::ehrpd_sector_id);
    return c.get<fixed_octstring<16, true> >();
  }

private:
  types                                                                        type_;
  choice_buffer_t<fixed_octstring<16, true>, geran_cell_id_s, target_rnc_id_s> c;

  void destroy_();
};

// RIMTransfer-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o rim_transfer_ext_ies_o;

typedef protocol_ext_container_empty_l rim_transfer_ext_ies_container;

// RIMTransfer ::= SEQUENCE
struct rim_transfer_s {
  bool                           ext                         = false;
  bool                           rim_routing_address_present = false;
  bool                           ie_exts_present             = false;
  unbounded_octstring<true>      rim_info;
  rim_routing_address_c          rim_routing_address;
  rim_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Inter-SystemInformationTransferType ::= CHOICE
struct inter_sys_info_transfer_type_c {
  struct types_opts {
    enum options { rim_transfer, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  types       type() const { return types::rim_transfer; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  rim_transfer_s&       rim_transfer() { return c; }
  const rim_transfer_s& rim_transfer() const { return c; }

private:
  rim_transfer_s c;
};

// ENBDirectInformationTransferIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct enb_direct_info_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { inter_sys_info_transfer_type_edt, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::inter_sys_info_transfer_type_edt; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    inter_sys_info_transfer_type_c&       inter_sys_info_transfer_type_edt() { return c; }
    const inter_sys_info_transfer_type_c& inter_sys_info_transfer_type_edt() const { return c; }

  private:
    inter_sys_info_transfer_type_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ENBDirectInformationTransfer ::= SEQUENCE
struct enb_direct_info_transfer_s {
  bool                                                    ext = false;
  protocol_ie_container_l<enb_direct_info_transfer_ies_o> protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ENBStatusTransferIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct enb_status_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, enb_status_transfer_transparent_container, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                          mme_ue_s1ap_id();
    uint32_t&                                          enb_ue_s1ap_id();
    enb_status_transfer_transparent_container_s&       enb_status_transfer_transparent_container();
    const uint64_t&                                    mme_ue_s1ap_id() const;
    const uint32_t&                                    enb_ue_s1ap_id() const;
    const enb_status_transfer_transparent_container_s& enb_status_transfer_transparent_container() const;

  private:
    types                                                        type_;
    choice_buffer_t<enb_status_transfer_transparent_container_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct enb_status_transfer_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<enb_status_transfer_transparent_container_s>    enb_status_transfer_transparent_container;

  // sequence methods
  enb_status_transfer_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ENBStatusTransfer ::= SEQUENCE
struct enb_status_transfer_s {
  bool                              ext = false;
  enb_status_transfer_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRANResponse ::= SEQUENCE
struct eutran_resp_s {
  bool                           ext = false;
  unbounded_octstring<true>      cell_id;
  eutra_ncell_load_report_resp_s eutra_ncell_load_report_resp;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EmergencyAreaIDList ::= SEQUENCE (SIZE (1..65535)) OF OCTET STRING (SIZE (3))
using emergency_area_id_list_l = dyn_array<fixed_octstring<3, true> >;

// EmergencyAreaIDListForRestart ::= SEQUENCE (SIZE (1..256)) OF OCTET STRING (SIZE (3))
using emergency_area_id_list_for_restart_l = dyn_array<fixed_octstring<3, true> >;

// ErrorIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct error_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, cause, crit_diagnostics, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 mme_ue_s1ap_id();
    uint32_t&                 enb_ue_s1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           mme_ue_s1ap_id() const;
    const uint32_t&           enb_ue_s1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

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

struct error_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       mme_ue_s1ap_id_present   = false;
  bool                                                       enb_ue_s1ap_id_present   = false;
  bool                                                       cause_present            = false;
  bool                                                       crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<cause_c>                                        cause;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;

  // sequence methods
  error_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ErrorIndication ::= SEQUENCE
struct error_ind_s {
  bool                    ext = false;
  error_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NumberOfMeasurementReportingLevels ::= ENUMERATED
struct nof_meas_report_levels_opts {
  enum options { rl2, rl3, rl4, rl5, rl10, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<nof_meas_report_levels_opts, true> nof_meas_report_levels_e;

// EventTriggeredCellLoadReportingRequest ::= SEQUENCE
struct event_triggered_cell_load_report_request_s {
  bool                     ext = false;
  nof_meas_report_levels_e nof_meas_report_levels;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OverloadFlag ::= ENUMERATED
struct overload_flag_opts {
  enum options { overload, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<overload_flag_opts, true> overload_flag_e;

// EventTriggeredCellLoadReportingResponse ::= SEQUENCE
struct event_triggered_cell_load_report_resp_s {
  bool                    ext                   = false;
  bool                    overload_flag_present = false;
  cell_load_report_resp_c cell_load_report_resp;
  overload_flag_e         overload_flag;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ExpectedUEActivityBehaviour-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o expected_ue_activity_behaviour_ext_ies_o;

// SourceOfUEActivityBehaviourInformation ::= ENUMERATED
struct source_of_ue_activity_behaviour_info_opts {
  enum options { subscription_info, statistics, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<source_of_ue_activity_behaviour_info_opts, true> source_of_ue_activity_behaviour_info_e;

typedef protocol_ext_container_empty_l expected_ue_activity_behaviour_ext_ies_container;

// ExpectedUEActivityBehaviour ::= SEQUENCE
struct expected_ue_activity_behaviour_s {
  bool                                             ext                                         = false;
  bool                                             expected_activity_period_present            = false;
  bool                                             expected_idle_period_present                = false;
  bool                                             sourceof_ue_activity_behaviour_info_present = false;
  bool                                             ie_exts_present                             = false;
  uint8_t                                          expected_activity_period                    = 1;
  uint8_t                                          expected_idle_period                        = 1;
  source_of_ue_activity_behaviour_info_e           sourceof_ue_activity_behaviour_info;
  expected_ue_activity_behaviour_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ExpectedHOInterval ::= ENUMERATED
struct expected_ho_interv_opts {
  enum options { sec15, sec30, sec60, sec90, sec120, sec180, long_time, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<expected_ho_interv_opts, true> expected_ho_interv_e;

// ExpectedUEBehaviour-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o expected_ue_behaviour_ext_ies_o;

typedef protocol_ext_container_empty_l expected_ue_behaviour_ext_ies_container;

// ExpectedUEBehaviour ::= SEQUENCE
struct expected_ue_behaviour_s {
  bool                                    ext                        = false;
  bool                                    expected_activity_present  = false;
  bool                                    expected_ho_interv_present = false;
  bool                                    ie_exts_present            = false;
  expected_ue_activity_behaviour_s        expected_activity;
  expected_ho_interv_e                    expected_ho_interv;
  expected_ue_behaviour_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TooEarlyInterRATHOReportReportFromEUTRAN ::= SEQUENCE
struct too_early_inter_ratho_report_report_from_eutran_s {
  bool                             ext              = false;
  bool                             mob_info_present = false;
  unbounded_octstring<true>        uerlf_report_container;
  fixed_bitstring<32, false, true> mob_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureEventReport ::= CHOICE
struct fail_event_report_c {
  struct types_opts {
    enum options { too_early_inter_ratho_report_from_eutran, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  types       type() const { return types::too_early_inter_ratho_report_from_eutran; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  too_early_inter_ratho_report_report_from_eutran_s&       too_early_inter_ratho_report_from_eutran() { return c; }
  const too_early_inter_ratho_report_report_from_eutran_s& too_early_inter_ratho_report_from_eutran() const
  {
    return c;
  }

private:
  too_early_inter_ratho_report_report_from_eutran_s c;
};

// GUMMEIList ::= SEQUENCE (SIZE (1..256)) OF GUMMEI
using gummei_list_l = dyn_array<gummei_s>;

// HoReportType ::= ENUMERATED
struct ho_report_type_opts {
  enum options { unnecessaryhotoanotherrat, /*...*/ earlyiratho, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ho_report_type_opts, true, 1> ho_report_type_e;

// HoType ::= ENUMERATED
struct ho_type_opts {
  enum options { ltetoutran, ltetogeran, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ho_type_opts, true> ho_type_e;

// HOReport ::= SEQUENCE
struct ho_report_s {
  bool                  ext = false;
  ho_type_e             ho_type;
  ho_report_type_e      ho_report_type;
  irat_cell_id_c        hosource_id;
  irat_cell_id_c        ho_target_id;
  candidate_cell_list_l candidate_cell_list;
  // ...
  copy_ptr<candidate_pci_list_l> candidate_pci_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCancelIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ho_cancel_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, cause, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&       mme_ue_s1ap_id();
    uint32_t&       enb_ue_s1ap_id();
    cause_c&        cause();
    const uint64_t& mme_ue_s1ap_id() const;
    const uint32_t& enb_ue_s1ap_id() const;
    const cause_c&  cause() const;

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

struct ho_cancel_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<cause_c>                                        cause;

  // sequence methods
  ho_cancel_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCancel ::= SEQUENCE
struct ho_cancel_s {
  bool                    ext = false;
  ho_cancel_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCancelAcknowledgeIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ho_cancel_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, crit_diagnostics, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 mme_ue_s1ap_id();
    uint32_t&                 enb_ue_s1ap_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           mme_ue_s1ap_id() const;
    const uint32_t&           enb_ue_s1ap_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

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

struct ho_cancel_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;

  // sequence methods
  ho_cancel_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCancelAcknowledge ::= SEQUENCE
struct ho_cancel_ack_s {
  bool                        ext = false;
  ho_cancel_ack_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverType ::= ENUMERATED
struct handov_type_opts {
  enum options { intralte, ltetoutran, ltetogeran, utrantolte, gerantolte, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<handov_type_opts, true> handov_type_e;

// HandoverCommandIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ho_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        handov_type,
        nas_security_paramsfrom_e_utran,
        erab_subjectto_data_forwarding_list,
        erab_to_release_list_ho_cmd,
        target_to_source_transparent_container,
        target_to_source_transparent_container_secondary,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                                        mme_ue_s1ap_id();
    uint32_t&                                                        enb_ue_s1ap_id();
    handov_type_e&                                                   handov_type();
    unbounded_octstring<true>&                                       nas_security_paramsfrom_e_utran();
    erab_ie_container_list_l<erab_data_forwarding_item_ies_o>&       erab_subjectto_data_forwarding_list();
    erab_list_l&                                                     erab_to_release_list_ho_cmd();
    unbounded_octstring<true>&                                       target_to_source_transparent_container();
    unbounded_octstring<true>&                                       target_to_source_transparent_container_secondary();
    crit_diagnostics_s&                                              crit_diagnostics();
    const uint64_t&                                                  mme_ue_s1ap_id() const;
    const uint32_t&                                                  enb_ue_s1ap_id() const;
    const handov_type_e&                                             handov_type() const;
    const unbounded_octstring<true>&                                 nas_security_paramsfrom_e_utran() const;
    const erab_ie_container_list_l<erab_data_forwarding_item_ies_o>& erab_subjectto_data_forwarding_list() const;
    const erab_list_l&                                               erab_to_release_list_ho_cmd() const;
    const unbounded_octstring<true>&                                 target_to_source_transparent_container() const;
    const unbounded_octstring<true>& target_to_source_transparent_container_secondary() const;
    const crit_diagnostics_s&        crit_diagnostics() const;

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    erab_ie_container_list_l<erab_data_forwarding_item_ies_o>,
                    erab_list_l,
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

struct ho_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool nas_security_paramsfrom_e_utran_present                  = false;
  bool erab_subjectto_data_forwarding_list_present              = false;
  bool erab_to_release_list_ho_cmd_present                      = false;
  bool target_to_source_transparent_container_secondary_present = false;
  bool crit_diagnostics_present                                 = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >             mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >               enb_ue_s1ap_id;
  ie_field_s<handov_type_e>                                              handov_type;
  ie_field_s<unbounded_octstring<true> >                                 nas_security_paramsfrom_e_utran;
  ie_field_s<erab_ie_container_list_l<erab_data_forwarding_item_ies_o> > erab_subjectto_data_forwarding_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_item_ies_o>, 1, 256, true> > erab_to_release_list_ho_cmd;
  ie_field_s<unbounded_octstring<true> > target_to_source_transparent_container;
  ie_field_s<unbounded_octstring<true> > target_to_source_transparent_container_secondary;
  ie_field_s<crit_diagnostics_s>         crit_diagnostics;

  // sequence methods
  ho_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCommand ::= SEQUENCE
struct ho_cmd_s {
  bool                 ext = false;
  ho_cmd_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverFailureIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ho_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, cause, crit_diagnostics, nulltype } value;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 mme_ue_s1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           mme_ue_s1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

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

struct ho_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<cause_c>                                        cause;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;

  // sequence methods
  ho_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverFailure ::= SEQUENCE
struct ho_fail_s {
  bool                  ext = false;
  ho_fail_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverNotifyIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ho_notify_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, eutran_cgi, tai, tunnel_info_for_bbf, lhn_id, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        mme_ue_s1ap_id();
    uint32_t&                        enb_ue_s1ap_id();
    eutran_cgi_s&                    eutran_cgi();
    tai_s&                           tai();
    tunnel_info_s&                   tunnel_info_for_bbf();
    unbounded_octstring<true>&       lhn_id();
    const uint64_t&                  mme_ue_s1ap_id() const;
    const uint32_t&                  enb_ue_s1ap_id() const;
    const eutran_cgi_s&              eutran_cgi() const;
    const tai_s&                     tai() const;
    const tunnel_info_s&             tunnel_info_for_bbf() const;
    const unbounded_octstring<true>& lhn_id() const;

  private:
    types                                                                           type_;
    choice_buffer_t<eutran_cgi_s, tai_s, tunnel_info_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ho_notify_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       tunnel_info_for_bbf_present = false;
  bool                                                       lhn_id_present              = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<eutran_cgi_s>                                   eutran_cgi;
  ie_field_s<tai_s>                                          tai;
  ie_field_s<tunnel_info_s>                                  tunnel_info_for_bbf;
  ie_field_s<unbounded_octstring<true> >                     lhn_id;

  // sequence methods
  ho_notify_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverNotify ::= SEQUENCE
struct ho_notify_s {
  bool                    ext = false;
  ho_notify_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationFailureIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ho_prep_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, cause, crit_diagnostics, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 mme_ue_s1ap_id();
    uint32_t&                 enb_ue_s1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           mme_ue_s1ap_id() const;
    const uint32_t&           enb_ue_s1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

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

struct ho_prep_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<cause_c>                                        cause;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;

  // sequence methods
  ho_prep_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationFailure ::= SEQUENCE
struct ho_prep_fail_s {
  bool                       ext = false;
  ho_prep_fail_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSFN-ResultToLogInfo-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o mbsfn_result_to_log_info_ext_ies_o;

typedef protocol_ext_container_empty_l mbsfn_result_to_log_info_ext_ies_container;

// MBSFN-ResultToLogInfo ::= SEQUENCE
struct mbsfn_result_to_log_info_s {
  bool                                       ext                   = false;
  bool                                       mbsfn_area_id_present = false;
  bool                                       ie_exts_present       = false;
  uint16_t                                   mbsfn_area_id         = 0;
  uint32_t                                   carrier_freq          = 0;
  mbsfn_result_to_log_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Links-to-log ::= ENUMERATED
struct links_to_log_opts {
  enum options { ul, dl, both_ul_and_dl, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<links_to_log_opts, true> links_to_log_e;

// LoggedMBSFNMDT-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o logged_mbsfnmdt_ext_ies_o;

// LoggingDuration ::= ENUMERATED
struct logging_dur_opts {
  enum options { m10, m20, m40, m60, m90, m120, nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<logging_dur_opts> logging_dur_e;

// LoggingInterval ::= ENUMERATED
struct logging_interv_opts {
  enum options { ms128, ms256, ms512, ms1024, ms2048, ms3072, ms4096, ms6144, nulltype } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<logging_interv_opts> logging_interv_e;

// M3Configuration-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o m3_cfg_ext_ies_o;

// M3period ::= ENUMERATED
struct m3period_opts {
  enum options { ms100, ms1000, ms10000, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<m3period_opts, true> m3period_e;

// M4Configuration-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o m4_cfg_ext_ies_o;

// M4period ::= ENUMERATED
struct m4period_opts {
  enum options { ms1024, ms2048, ms5120, ms10240, min1, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<m4period_opts, true> m4period_e;

// M5Configuration-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o m5_cfg_ext_ies_o;

// M5period ::= ENUMERATED
struct m5period_opts {
  enum options { ms1024, ms2048, ms5120, ms10240, min1, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<m5period_opts, true> m5period_e;

// M6Configuration-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o m6_cfg_ext_ies_o;

// M6delay-threshold ::= ENUMERATED
struct m6delay_thres_opts {
  enum options { ms30, ms40, ms50, ms60, ms70, ms80, ms90, ms100, ms150, ms300, ms500, ms750, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<m6delay_thres_opts, true> m6delay_thres_e;

// M6report-Interval ::= ENUMERATED
struct m6report_interv_opts {
  enum options { ms1024, ms2048, ms5120, ms10240, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<m6report_interv_opts, true> m6report_interv_e;

// M7Configuration-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o m7_cfg_ext_ies_o;

// MBSFN-ResultToLog ::= SEQUENCE (SIZE (1..8)) OF MBSFN-ResultToLogInfo
using mbsfn_result_to_log_l = dyn_array<mbsfn_result_to_log_info_s>;

typedef protocol_ext_container_empty_l logged_mbsfnmdt_ext_ies_container;

// LoggedMBSFNMDT ::= SEQUENCE
struct logged_mbsfnmdt_s {
  bool                              ext                         = false;
  bool                              mbsfn_result_to_log_present = false;
  bool                              ie_exts_present             = false;
  logging_interv_e                  logging_interv;
  logging_dur_e                     logging_dur;
  mbsfn_result_to_log_l             mbsfn_result_to_log;
  logged_mbsfnmdt_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// M1PeriodicReporting-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o m1_periodic_report_ext_ies_o;

// M1ThresholdEventA2-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o m1_thres_event_a2_ext_ies_o;

typedef protocol_ext_container_empty_l m3_cfg_ext_ies_container;

// M3Configuration ::= SEQUENCE
struct m3_cfg_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  m3period_e               m3period;
  m3_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l m4_cfg_ext_ies_container;

// M4Configuration ::= SEQUENCE
struct m4_cfg_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  m4period_e               m4period;
  links_to_log_e           m4_links_to_log;
  m4_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l m5_cfg_ext_ies_container;

// M5Configuration ::= SEQUENCE
struct m5_cfg_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  m5period_e               m5period;
  links_to_log_e           m5_links_to_log;
  m5_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l m6_cfg_ext_ies_container;

// M6Configuration ::= SEQUENCE
struct m6_cfg_s {
  bool                     ext                   = false;
  bool                     m6delay_thres_present = false;
  bool                     ie_exts_present       = false;
  m6report_interv_e        m6report_interv;
  m6delay_thres_e          m6delay_thres;
  links_to_log_e           m6_links_to_log;
  m6_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l m7_cfg_ext_ies_container;

// M7Configuration ::= SEQUENCE
struct m7_cfg_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  uint8_t                  m7period        = 1;
  links_to_log_e           m7_links_to_log;
  m7_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementThresholdA2 ::= CHOICE
struct meas_thres_a2_c {
  struct types_opts {
    enum options { thres_rsrp, thres_rsrq, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  meas_thres_a2_c() = default;
  meas_thres_a2_c(const meas_thres_a2_c& other);
  meas_thres_a2_c& operator=(const meas_thres_a2_c& other);
  ~meas_thres_a2_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& thres_rsrp()
  {
    assert_choice_type("threshold-RSRP", type_.to_string(), "MeasurementThresholdA2");
    return c.get<uint8_t>();
  }
  uint8_t& thres_rsrq()
  {
    assert_choice_type("threshold-RSRQ", type_.to_string(), "MeasurementThresholdA2");
    return c.get<uint8_t>();
  }
  const uint8_t& thres_rsrp() const
  {
    assert_choice_type("threshold-RSRP", type_.to_string(), "MeasurementThresholdA2");
    return c.get<uint8_t>();
  }
  const uint8_t& thres_rsrq() const
  {
    assert_choice_type("threshold-RSRQ", type_.to_string(), "MeasurementThresholdA2");
    return c.get<uint8_t>();
  }
  uint8_t& set_thres_rsrp()
  {
    set(types::thres_rsrp);
    return c.get<uint8_t>();
  }
  uint8_t& set_thres_rsrq()
  {
    set(types::thres_rsrq);
    return c.get<uint8_t>();
  }

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// ReportAmountMDT ::= ENUMERATED
struct report_amount_mdt_opts {
  enum options { r1, r2, r4, r8, r16, r32, r64, rinfinity, nulltype } value;
  typedef int8_t number_type;

  std::string to_string() const;
  int8_t      to_number() const;
};
typedef enumerated<report_amount_mdt_opts> report_amount_mdt_e;

// ReportIntervalMDT ::= ENUMERATED
struct report_interv_mdt_opts {
  enum options {
    ms120,
    ms240,
    ms480,
    ms640,
    ms1024,
    ms2048,
    ms5120,
    ms10240,
    min1,
    min6,
    min12,
    min30,
    min60,
    nulltype
  } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<report_interv_mdt_opts> report_interv_mdt_e;

// ImmediateMDT-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct immediate_mdt_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { m3_cfg, m4_cfg, m5_cfg, mdt_location_info, m6_cfg, m7_cfg, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    m3_cfg_s&                              m3_cfg();
    m4_cfg_s&                              m4_cfg();
    m5_cfg_s&                              m5_cfg();
    fixed_bitstring<8, false, true>&       mdt_location_info();
    m6_cfg_s&                              m6_cfg();
    m7_cfg_s&                              m7_cfg();
    const m3_cfg_s&                        m3_cfg() const;
    const m4_cfg_s&                        m4_cfg() const;
    const m5_cfg_s&                        m5_cfg() const;
    const fixed_bitstring<8, false, true>& mdt_location_info() const;
    const m6_cfg_s&                        m6_cfg() const;
    const m7_cfg_s&                        m7_cfg() const;

  private:
    types                                                                                              type_;
    choice_buffer_t<fixed_bitstring<8, false, true>, m3_cfg_s, m4_cfg_s, m5_cfg_s, m6_cfg_s, m7_cfg_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LoggedMDT-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o logged_mdt_ext_ies_o;

typedef protocol_ext_container_empty_l m1_periodic_report_ext_ies_container;

// M1PeriodicReporting ::= SEQUENCE
struct m1_periodic_report_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  report_interv_mdt_e                  report_interv;
  report_amount_mdt_e                  report_amount;
  m1_periodic_report_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// M1ReportingTrigger ::= ENUMERATED
struct m1_report_trigger_opts {
  enum options { periodic, a2eventtriggered, /*...*/ a2eventtriggered_periodic, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<m1_report_trigger_opts, true, 1> m1_report_trigger_e;

typedef protocol_ext_container_empty_l m1_thres_event_a2_ext_ies_container;

// M1ThresholdEventA2 ::= SEQUENCE
struct m1_thres_event_a2_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  meas_thres_a2_c                     meas_thres;
  m1_thres_event_a2_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MDTMode-ExtensionIE ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct mdt_mode_ext_ie_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { logged_mbsfnmdt, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::logged_mbsfnmdt; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    logged_mbsfnmdt_s&       logged_mbsfnmdt() { return c; }
    const logged_mbsfnmdt_s& logged_mbsfnmdt() const { return c; }

  private:
    logged_mbsfnmdt_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct immediate_mdt_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                         m3_cfg_present            = false;
  bool                                         m4_cfg_present            = false;
  bool                                         m5_cfg_present            = false;
  bool                                         mdt_location_info_present = false;
  bool                                         m6_cfg_present            = false;
  bool                                         m7_cfg_present            = false;
  ie_field_s<m3_cfg_s>                         m3_cfg;
  ie_field_s<m4_cfg_s>                         m4_cfg;
  ie_field_s<m5_cfg_s>                         m5_cfg;
  ie_field_s<fixed_bitstring<8, false, true> > mdt_location_info;
  ie_field_s<m6_cfg_s>                         m6_cfg;
  ie_field_s<m7_cfg_s>                         m7_cfg;

  // sequence methods
  immediate_mdt_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ImmediateMDT ::= SEQUENCE
struct immediate_mdt_s {
  bool                            ext                       = false;
  bool                            m1thresevent_a2_present   = false;
  bool                            m1periodic_report_present = false;
  bool                            ie_exts_present           = false;
  fixed_bitstring<8, false, true> meass_to_activ;
  m1_report_trigger_e             m1report_trigger;
  m1_thres_event_a2_s             m1thresevent_a2;
  m1_periodic_report_s            m1periodic_report;
  immediate_mdt_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l logged_mdt_ext_ies_container;

// LoggedMDT ::= SEQUENCE
struct logged_mdt_s {
  bool                         ext             = false;
  bool                         ie_exts_present = false;
  logging_interv_e             logging_interv;
  logging_dur_e                logging_dur;
  logged_mdt_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MDTPLMNList ::= SEQUENCE (SIZE (1..16)) OF OCTET STRING (SIZE (3))
using mdtplmn_list_l = bounded_array<fixed_octstring<3, true>, 16>;

// MDT-Activation ::= ENUMERATED
struct mdt_activation_opts {
  enum options {
    immediate_mdt_only,
    immediate_mdt_and_trace,
    logged_mdt_only,
    /*...*/ logged_mbsfn_mdt,
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<mdt_activation_opts, true, 1> mdt_activation_e;

// MDT-Configuration-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct mdt_cfg_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { sig_based_mdtplmn_list, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sig_based_mdtplmn_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mdtplmn_list_l&       sig_based_mdtplmn_list() { return c; }
    const mdtplmn_list_l& sig_based_mdtplmn_list() const { return c; }

  private:
    mdtplmn_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MDTMode ::= CHOICE
struct mdt_mode_c {
  struct types_opts {
    enum options { immediate_mdt, logged_mdt, /*...*/ mdt_mode_ext, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true, 1> types;

  // choice methods
  mdt_mode_c() = default;
  mdt_mode_c(const mdt_mode_c& other);
  mdt_mode_c& operator=(const mdt_mode_c& other);
  ~mdt_mode_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  immediate_mdt_s& immediate_mdt()
  {
    assert_choice_type("immediateMDT", type_.to_string(), "MDTMode");
    return c.get<immediate_mdt_s>();
  }
  logged_mdt_s& logged_mdt()
  {
    assert_choice_type("loggedMDT", type_.to_string(), "MDTMode");
    return c.get<logged_mdt_s>();
  }
  protocol_ie_single_container_s<mdt_mode_ext_ie_o>& mdt_mode_ext()
  {
    assert_choice_type("mDTMode-Extension", type_.to_string(), "MDTMode");
    return c.get<protocol_ie_single_container_s<mdt_mode_ext_ie_o> >();
  }
  const immediate_mdt_s& immediate_mdt() const
  {
    assert_choice_type("immediateMDT", type_.to_string(), "MDTMode");
    return c.get<immediate_mdt_s>();
  }
  const logged_mdt_s& logged_mdt() const
  {
    assert_choice_type("loggedMDT", type_.to_string(), "MDTMode");
    return c.get<logged_mdt_s>();
  }
  const protocol_ie_single_container_s<mdt_mode_ext_ie_o>& mdt_mode_ext() const
  {
    assert_choice_type("mDTMode-Extension", type_.to_string(), "MDTMode");
    return c.get<protocol_ie_single_container_s<mdt_mode_ext_ie_o> >();
  }
  immediate_mdt_s& set_immediate_mdt()
  {
    set(types::immediate_mdt);
    return c.get<immediate_mdt_s>();
  }
  logged_mdt_s& set_logged_mdt()
  {
    set(types::logged_mdt);
    return c.get<logged_mdt_s>();
  }
  protocol_ie_single_container_s<mdt_mode_ext_ie_o>& set_mdt_mode_ext()
  {
    set(types::mdt_mode_ext);
    return c.get<protocol_ie_single_container_s<mdt_mode_ext_ie_o> >();
  }

private:
  types                                                                                              type_;
  choice_buffer_t<immediate_mdt_s, logged_mdt_s, protocol_ie_single_container_s<mdt_mode_ext_ie_o> > c;

  void destroy_();
};

// MDT-Configuration ::= SEQUENCE
struct mdt_cfg_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  mdt_activation_e                            mdt_activation;
  area_scope_of_mdt_c                         area_scope_of_mdt;
  mdt_mode_c                                  mdt_mode;
  protocol_ext_container_l<mdt_cfg_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ProSeUEtoNetworkRelaying ::= ENUMERATED
struct pro_se_ueto_network_relaying_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<pro_se_ueto_network_relaying_opts, true> pro_se_ueto_network_relaying_e;

// EventType ::= ENUMERATED
struct event_type_opts {
  enum options { direct, change_of_serve_cell, stop_change_of_serve_cell, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<event_type_opts, true> event_type_e;

// PedestrianUE ::= ENUMERATED
struct pedestrian_ue_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<pedestrian_ue_opts, true> pedestrian_ue_e;

// ProSeAuthorized-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct pro_se_authorized_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { pro_se_ueto_network_relaying, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::pro_se_ueto_network_relaying; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pro_se_ueto_network_relaying_e&       pro_se_ueto_network_relaying() { return c; }
    const pro_se_ueto_network_relaying_e& pro_se_ueto_network_relaying() const { return c; }

  private:
    pro_se_ueto_network_relaying_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ProSeDirectCommunication ::= ENUMERATED
struct pro_se_direct_communication_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<pro_se_direct_communication_opts, true> pro_se_direct_communication_e;

// ProSeDirectDiscovery ::= ENUMERATED
struct pro_se_direct_discovery_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<pro_se_direct_discovery_opts, true> pro_se_direct_discovery_e;

// ReportArea ::= ENUMERATED
struct report_area_opts {
  enum options { ecgi, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<report_area_opts, true> report_area_e;

// RequestType-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o request_type_ext_ies_o;

// SecurityContext-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o security_context_ext_ies_o;

// TraceActivation-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct trace_activation_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { mdt_cfg, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::mdt_cfg; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mdt_cfg_s&       mdt_cfg() { return c; }
    const mdt_cfg_s& mdt_cfg() const { return c; }

  private:
    mdt_cfg_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

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

// UE-Sidelink-Aggregate-MaximumBitrates-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o ue_sidelink_aggregate_maximum_bitrates_ext_ies_o;

// UESecurityCapabilities-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o ue_security_cap_ext_ies_o;

// V2XServicesAuthorized-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o v2xservices_authorized_ext_ies_o;

// VehicleUE ::= ENUMERATED
struct vehicle_ue_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<vehicle_ue_opts, true> vehicle_ue_e;

// ManagementBasedMDTAllowed ::= ENUMERATED
struct management_based_mdt_allowed_opts {
  enum options { allowed, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<management_based_mdt_allowed_opts, true> management_based_mdt_allowed_e;

// ProSeAuthorized ::= SEQUENCE
struct pro_se_authorized_s {
  bool                                                  ext                                 = false;
  bool                                                  pro_se_direct_discovery_present     = false;
  bool                                                  pro_se_direct_communication_present = false;
  bool                                                  ie_exts_present                     = false;
  pro_se_direct_discovery_e                             pro_se_direct_discovery;
  pro_se_direct_communication_e                         pro_se_direct_communication;
  protocol_ext_container_l<pro_se_authorized_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l request_type_ext_ies_container;

// RequestType ::= SEQUENCE
struct request_type_s {
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  event_type_e                   event_type;
  report_area_e                  report_area;
  request_type_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l security_context_ext_ies_container;

// SecurityContext ::= SEQUENCE
struct security_context_s {
  bool                               ext                     = false;
  bool                               ie_exts_present         = false;
  uint8_t                            next_hop_chaining_count = 0;
  fixed_bitstring<256, false, true>  next_hop_param;
  security_context_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceActivation ::= SEQUENCE
struct trace_activation_s {
  bool                                                 ext             = false;
  bool                                                 ie_exts_present = false;
  fixed_octstring<8, true>                             e_utran_trace_id;
  fixed_bitstring<8, false, true>                      interfaces_to_trace;
  trace_depth_e                                        trace_depth;
  bounded_bitstring<1, 160, true, true>                trace_collection_entity_ip_address;
  protocol_ext_container_l<trace_activation_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l ue_security_cap_ext_ies_container;

// UESecurityCapabilities ::= SEQUENCE
struct ue_security_cap_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  fixed_bitstring<16, true, true>   encryption_algorithms;
  fixed_bitstring<16, true, true>   integrity_protection_algorithms;
  ue_security_cap_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l ue_sidelink_aggregate_maximum_bitrates_ext_ies_container;

// UESidelinkAggregateMaximumBitrate ::= SEQUENCE
struct ue_sidelink_aggregate_maximum_bitrate_s {
  bool                                                     ext                                   = false;
  bool                                                     ie_exts_present                       = false;
  uint64_t                                                 uesidelink_aggregate_maximum_bit_rate = 0;
  ue_sidelink_aggregate_maximum_bitrates_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEUserPlaneCIoTSupportIndicator ::= ENUMERATED
struct ueuser_plane_cio_tsupport_ind_opts {
  enum options { supported, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ueuser_plane_cio_tsupport_ind_opts, true> ueuser_plane_cio_tsupport_ind_e;

typedef protocol_ext_container_empty_l v2xservices_authorized_ext_ies_container;

// V2XServicesAuthorized ::= SEQUENCE
struct v2xservices_authorized_s {
  bool                                     ext                   = false;
  bool                                     vehicle_ue_present    = false;
  bool                                     pedestrian_ue_present = false;
  bool                                     ie_exts_present       = false;
  vehicle_ue_e                             vehicle_ue;
  pedestrian_ue_e                          pedestrian_ue;
  v2xservices_authorized_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverRequestIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ho_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        handov_type,
        cause,
        ueaggregate_maximum_bitrate,
        erab_to_be_setup_list_ho_req,
        source_to_target_transparent_container,
        ue_security_cap,
        ho_restrict_list,
        trace_activation,
        request_type,
        srvcc_operation_possible,
        security_context,
        nas_security_paramsto_e_utran,
        csg_id,
        csg_membership_status,
        gummei_id,
        mme_ue_s1ap_id_minus2,
        management_based_mdt_allowed,
        management_based_mdtplmn_list,
        masked_imeisv,
        expected_ue_behaviour,
        pro_se_authorized,
        ueuser_plane_cio_tsupport_ind,
        v2xservices_authorized,
        ue_sidelink_aggregate_maximum_bitrate,
        enhanced_coverage_restricted,
        ce_mode_brestricted,
        pending_data_ind,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                                           mme_ue_s1ap_id();
    handov_type_e&                                                      handov_type();
    cause_c&                                                            cause();
    ue_aggregate_maximum_bitrate_s&                                     ueaggregate_maximum_bitrate();
    erab_ie_container_list_l<erab_to_be_setup_item_ho_req_ies_o>&       erab_to_be_setup_list_ho_req();
    unbounded_octstring<true>&                                          source_to_target_transparent_container();
    ue_security_cap_s&                                                  ue_security_cap();
    ho_restrict_list_s&                                                 ho_restrict_list();
    trace_activation_s&                                                 trace_activation();
    request_type_s&                                                     request_type();
    srvcc_operation_possible_e&                                         srvcc_operation_possible();
    security_context_s&                                                 security_context();
    unbounded_octstring<true>&                                          nas_security_paramsto_e_utran();
    fixed_bitstring<27, false, true>&                                   csg_id();
    csg_membership_status_e&                                            csg_membership_status();
    gummei_s&                                                           gummei_id();
    uint64_t&                                                           mme_ue_s1ap_id_minus2();
    management_based_mdt_allowed_e&                                     management_based_mdt_allowed();
    mdtplmn_list_l&                                                     management_based_mdtplmn_list();
    fixed_bitstring<64, false, true>&                                   masked_imeisv();
    expected_ue_behaviour_s&                                            expected_ue_behaviour();
    pro_se_authorized_s&                                                pro_se_authorized();
    ueuser_plane_cio_tsupport_ind_e&                                    ueuser_plane_cio_tsupport_ind();
    v2xservices_authorized_s&                                           v2xservices_authorized();
    ue_sidelink_aggregate_maximum_bitrate_s&                            ue_sidelink_aggregate_maximum_bitrate();
    enhanced_coverage_restricted_e&                                     enhanced_coverage_restricted();
    ce_mode_brestricted_e&                                              ce_mode_brestricted();
    pending_data_ind_e&                                                 pending_data_ind();
    const uint64_t&                                                     mme_ue_s1ap_id() const;
    const handov_type_e&                                                handov_type() const;
    const cause_c&                                                      cause() const;
    const ue_aggregate_maximum_bitrate_s&                               ueaggregate_maximum_bitrate() const;
    const erab_ie_container_list_l<erab_to_be_setup_item_ho_req_ies_o>& erab_to_be_setup_list_ho_req() const;
    const unbounded_octstring<true>&                                    source_to_target_transparent_container() const;
    const ue_security_cap_s&                                            ue_security_cap() const;
    const ho_restrict_list_s&                                           ho_restrict_list() const;
    const trace_activation_s&                                           trace_activation() const;
    const request_type_s&                                               request_type() const;
    const srvcc_operation_possible_e&                                   srvcc_operation_possible() const;
    const security_context_s&                                           security_context() const;
    const unbounded_octstring<true>&                                    nas_security_paramsto_e_utran() const;
    const fixed_bitstring<27, false, true>&                             csg_id() const;
    const csg_membership_status_e&                                      csg_membership_status() const;
    const gummei_s&                                                     gummei_id() const;
    const uint64_t&                                                     mme_ue_s1ap_id_minus2() const;
    const management_based_mdt_allowed_e&                               management_based_mdt_allowed() const;
    const mdtplmn_list_l&                                               management_based_mdtplmn_list() const;
    const fixed_bitstring<64, false, true>&                             masked_imeisv() const;
    const expected_ue_behaviour_s&                                      expected_ue_behaviour() const;
    const pro_se_authorized_s&                                          pro_se_authorized() const;
    const ueuser_plane_cio_tsupport_ind_e&                              ueuser_plane_cio_tsupport_ind() const;
    const v2xservices_authorized_s&                                     v2xservices_authorized() const;
    const ue_sidelink_aggregate_maximum_bitrate_s&                      ue_sidelink_aggregate_maximum_bitrate() const;
    const enhanced_coverage_restricted_e&                               enhanced_coverage_restricted() const;
    const ce_mode_brestricted_e&                                        ce_mode_brestricted() const;
    const pending_data_ind_e&                                           pending_data_ind() const;

  private:
    types type_;
    choice_buffer_t<cause_c,
                    erab_ie_container_list_l<erab_to_be_setup_item_ho_req_ies_o>,
                    expected_ue_behaviour_s,
                    fixed_bitstring<64, false, true>,
                    gummei_s,
                    ho_restrict_list_s,
                    mdtplmn_list_l,
                    pro_se_authorized_s,
                    request_type_s,
                    security_context_s,
                    trace_activation_s,
                    ue_aggregate_maximum_bitrate_s,
                    ue_security_cap_s,
                    ue_sidelink_aggregate_maximum_bitrate_s,
                    unbounded_octstring<true>,
                    v2xservices_authorized_s>
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

struct ho_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       ho_restrict_list_present                      = false;
  bool                                                       trace_activation_present                      = false;
  bool                                                       request_type_present                          = false;
  bool                                                       srvcc_operation_possible_present              = false;
  bool                                                       nas_security_paramsto_e_utran_present         = false;
  bool                                                       csg_id_present                                = false;
  bool                                                       csg_membership_status_present                 = false;
  bool                                                       gummei_id_present                             = false;
  bool                                                       mme_ue_s1ap_id_minus2_present                 = false;
  bool                                                       management_based_mdt_allowed_present          = false;
  bool                                                       management_based_mdtplmn_list_present         = false;
  bool                                                       masked_imeisv_present                         = false;
  bool                                                       expected_ue_behaviour_present                 = false;
  bool                                                       pro_se_authorized_present                     = false;
  bool                                                       ueuser_plane_cio_tsupport_ind_present         = false;
  bool                                                       v2xservices_authorized_present                = false;
  bool                                                       ue_sidelink_aggregate_maximum_bitrate_present = false;
  bool                                                       enhanced_coverage_restricted_present          = false;
  bool                                                       ce_mode_brestricted_present                   = false;
  bool                                                       pending_data_ind_present                      = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<handov_type_e>                                  handov_type;
  ie_field_s<cause_c>                                        cause;
  ie_field_s<ue_aggregate_maximum_bitrate_s>                 ueaggregate_maximum_bitrate;
  ie_field_s<erab_ie_container_list_l<erab_to_be_setup_item_ho_req_ies_o> > erab_to_be_setup_list_ho_req;
  ie_field_s<unbounded_octstring<true> >                                    source_to_target_transparent_container;
  ie_field_s<ue_security_cap_s>                                             ue_security_cap;
  ie_field_s<ho_restrict_list_s>                                            ho_restrict_list;
  ie_field_s<trace_activation_s>                                            trace_activation;
  ie_field_s<request_type_s>                                                request_type;
  ie_field_s<srvcc_operation_possible_e>                                    srvcc_operation_possible;
  ie_field_s<security_context_s>                                            security_context;
  ie_field_s<unbounded_octstring<true> >                                    nas_security_paramsto_e_utran;
  ie_field_s<fixed_bitstring<27, false, true> >                             csg_id;
  ie_field_s<csg_membership_status_e>                                       csg_membership_status;
  ie_field_s<gummei_s>                                                      gummei_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >                mme_ue_s1ap_id_minus2;
  ie_field_s<management_based_mdt_allowed_e>                                management_based_mdt_allowed;
  ie_field_s<dyn_seq_of<fixed_octstring<3, true>, 1, 16, true> >            management_based_mdtplmn_list;
  ie_field_s<fixed_bitstring<64, false, true> >                             masked_imeisv;
  ie_field_s<expected_ue_behaviour_s>                                       expected_ue_behaviour;
  ie_field_s<pro_se_authorized_s>                                           pro_se_authorized;
  ie_field_s<ueuser_plane_cio_tsupport_ind_e>                               ueuser_plane_cio_tsupport_ind;
  ie_field_s<v2xservices_authorized_s>                                      v2xservices_authorized;
  ie_field_s<ue_sidelink_aggregate_maximum_bitrate_s>                       ue_sidelink_aggregate_maximum_bitrate;
  ie_field_s<enhanced_coverage_restricted_e>                                enhanced_coverage_restricted;
  ie_field_s<ce_mode_brestricted_e>                                         ce_mode_brestricted;
  ie_field_s<pending_data_ind_e>                                            pending_data_ind;

  // sequence methods
  ho_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverRequest ::= SEQUENCE
struct ho_request_s {
  bool                     ext = false;
  ho_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CE-mode-B-SupportIndicator ::= ENUMERATED
struct ce_mode_b_support_ind_opts {
  enum options { supported, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ce_mode_b_support_ind_opts, true> ce_mode_b_support_ind_e;

// HandoverRequestAcknowledgeIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ho_request_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        erab_admitted_list,
        erab_failed_to_setup_list_ho_req_ack,
        target_to_source_transparent_container,
        csg_id,
        crit_diagnostics,
        cell_access_mode,
        ce_mode_b_support_ind,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                                            mme_ue_s1ap_id();
    uint32_t&                                                            enb_ue_s1ap_id();
    erab_ie_container_list_l<erab_admitted_item_ies_o>&                  erab_admitted_list();
    erab_ie_container_list_l<erab_failedto_setup_item_ho_req_ack_ies_o>& erab_failed_to_setup_list_ho_req_ack();
    unbounded_octstring<true>&                                           target_to_source_transparent_container();
    fixed_bitstring<27, false, true>&                                    csg_id();
    crit_diagnostics_s&                                                  crit_diagnostics();
    cell_access_mode_e&                                                  cell_access_mode();
    ce_mode_b_support_ind_e&                                             ce_mode_b_support_ind();
    const uint64_t&                                                      mme_ue_s1ap_id() const;
    const uint32_t&                                                      enb_ue_s1ap_id() const;
    const erab_ie_container_list_l<erab_admitted_item_ies_o>&            erab_admitted_list() const;
    const erab_ie_container_list_l<erab_failedto_setup_item_ho_req_ack_ies_o>&
                                            erab_failed_to_setup_list_ho_req_ack() const;
    const unbounded_octstring<true>&        target_to_source_transparent_container() const;
    const fixed_bitstring<27, false, true>& csg_id() const;
    const crit_diagnostics_s&               crit_diagnostics() const;
    const cell_access_mode_e&               cell_access_mode() const;
    const ce_mode_b_support_ind_e&          ce_mode_b_support_ind() const;

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    erab_ie_container_list_l<erab_admitted_item_ies_o>,
                    erab_ie_container_list_l<erab_failedto_setup_item_ho_req_ack_ies_o>,
                    fixed_bitstring<27, false, true>,
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

struct ho_request_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                            erab_failed_to_setup_list_ho_req_ack_present = false;
  bool                                                            csg_id_present                               = false;
  bool                                                            crit_diagnostics_present                     = false;
  bool                                                            cell_access_mode_present                     = false;
  bool                                                            ce_mode_b_support_ind_present                = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >      mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >        enb_ue_s1ap_id;
  ie_field_s<erab_ie_container_list_l<erab_admitted_item_ies_o> > erab_admitted_list;
  ie_field_s<erab_ie_container_list_l<erab_failedto_setup_item_ho_req_ack_ies_o> > erab_failed_to_setup_list_ho_req_ack;
  ie_field_s<unbounded_octstring<true> >        target_to_source_transparent_container;
  ie_field_s<fixed_bitstring<27, false, true> > csg_id;
  ie_field_s<crit_diagnostics_s>                crit_diagnostics;
  ie_field_s<cell_access_mode_e>                cell_access_mode;
  ie_field_s<ce_mode_b_support_ind_e>           ce_mode_b_support_ind;

  // sequence methods
  ho_request_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverRequestAcknowledge ::= SEQUENCE
struct ho_request_ack_s {
  bool                         ext = false;
  ho_request_ack_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Direct-Forwarding-Path-Availability ::= ENUMERATED
struct direct_forwarding_path_availability_opts {
  enum options { direct_path_available, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<direct_forwarding_path_availability_opts, true> direct_forwarding_path_availability_e;

// PS-ServiceNotAvailable ::= ENUMERATED
struct ps_service_not_available_opts {
  enum options { ps_service_not_available, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ps_service_not_available_opts, true> ps_service_not_available_e;

// SRVCCHOIndication ::= ENUMERATED
struct srvccho_ind_opts {
  enum options { psand_cs, csonly, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<srvccho_ind_opts, true> srvccho_ind_e;

// TargetID ::= CHOICE
struct target_id_c {
  struct types_opts {
    enum options { targetenb_id, target_rnc_id, cgi, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  target_id_c() = default;
  target_id_c(const target_id_c& other);
  target_id_c& operator=(const target_id_c& other);
  ~target_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  targetenb_id_s& targetenb_id()
  {
    assert_choice_type("targeteNB-ID", type_.to_string(), "TargetID");
    return c.get<targetenb_id_s>();
  }
  target_rnc_id_s& target_rnc_id()
  {
    assert_choice_type("targetRNC-ID", type_.to_string(), "TargetID");
    return c.get<target_rnc_id_s>();
  }
  cgi_s& cgi()
  {
    assert_choice_type("cGI", type_.to_string(), "TargetID");
    return c.get<cgi_s>();
  }
  const targetenb_id_s& targetenb_id() const
  {
    assert_choice_type("targeteNB-ID", type_.to_string(), "TargetID");
    return c.get<targetenb_id_s>();
  }
  const target_rnc_id_s& target_rnc_id() const
  {
    assert_choice_type("targetRNC-ID", type_.to_string(), "TargetID");
    return c.get<target_rnc_id_s>();
  }
  const cgi_s& cgi() const
  {
    assert_choice_type("cGI", type_.to_string(), "TargetID");
    return c.get<cgi_s>();
  }
  targetenb_id_s& set_targetenb_id()
  {
    set(types::targetenb_id);
    return c.get<targetenb_id_s>();
  }
  target_rnc_id_s& set_target_rnc_id()
  {
    set(types::target_rnc_id);
    return c.get<target_rnc_id_s>();
  }
  cgi_s& set_cgi()
  {
    set(types::cgi);
    return c.get<cgi_s>();
  }

private:
  types                                                   type_;
  choice_buffer_t<cgi_s, target_rnc_id_s, targetenb_id_s> c;

  void destroy_();
};

// HandoverRequiredIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ho_required_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        handov_type,
        cause,
        target_id,
        direct_forwarding_path_availability,
        srvccho_ind,
        source_to_target_transparent_container,
        source_to_target_transparent_container_secondary,
        ms_classmark2,
        ms_classmark3,
        csg_id,
        cell_access_mode,
        ps_service_not_available,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                    mme_ue_s1ap_id();
    uint32_t&                                    enb_ue_s1ap_id();
    handov_type_e&                               handov_type();
    cause_c&                                     cause();
    target_id_c&                                 target_id();
    direct_forwarding_path_availability_e&       direct_forwarding_path_availability();
    srvccho_ind_e&                               srvccho_ind();
    unbounded_octstring<true>&                   source_to_target_transparent_container();
    unbounded_octstring<true>&                   source_to_target_transparent_container_secondary();
    unbounded_octstring<true>&                   ms_classmark2();
    unbounded_octstring<true>&                   ms_classmark3();
    fixed_bitstring<27, false, true>&            csg_id();
    cell_access_mode_e&                          cell_access_mode();
    ps_service_not_available_e&                  ps_service_not_available();
    const uint64_t&                              mme_ue_s1ap_id() const;
    const uint32_t&                              enb_ue_s1ap_id() const;
    const handov_type_e&                         handov_type() const;
    const cause_c&                               cause() const;
    const target_id_c&                           target_id() const;
    const direct_forwarding_path_availability_e& direct_forwarding_path_availability() const;
    const srvccho_ind_e&                         srvccho_ind() const;
    const unbounded_octstring<true>&             source_to_target_transparent_container() const;
    const unbounded_octstring<true>&             source_to_target_transparent_container_secondary() const;
    const unbounded_octstring<true>&             ms_classmark2() const;
    const unbounded_octstring<true>&             ms_classmark3() const;
    const fixed_bitstring<27, false, true>&      csg_id() const;
    const cell_access_mode_e&                    cell_access_mode() const;
    const ps_service_not_available_e&            ps_service_not_available() const;

  private:
    types                                                                                               type_;
    choice_buffer_t<cause_c, fixed_bitstring<27, false, true>, target_id_c, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ho_required_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool direct_forwarding_path_availability_present              = false;
  bool srvccho_ind_present                                      = false;
  bool source_to_target_transparent_container_secondary_present = false;
  bool ms_classmark2_present                                    = false;
  bool ms_classmark3_present                                    = false;
  bool csg_id_present                                           = false;
  bool cell_access_mode_present                                 = false;
  bool ps_service_not_available_present                         = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<handov_type_e>                                  handov_type;
  ie_field_s<cause_c>                                        cause;
  ie_field_s<target_id_c>                                    target_id;
  ie_field_s<direct_forwarding_path_availability_e>          direct_forwarding_path_availability;
  ie_field_s<srvccho_ind_e>                                  srvccho_ind;
  ie_field_s<unbounded_octstring<true> >                     source_to_target_transparent_container;
  ie_field_s<unbounded_octstring<true> >                     source_to_target_transparent_container_secondary;
  ie_field_s<unbounded_octstring<true> >                     ms_classmark2;
  ie_field_s<unbounded_octstring<true> >                     ms_classmark3;
  ie_field_s<fixed_bitstring<27, false, true> >              csg_id;
  ie_field_s<cell_access_mode_e>                             cell_access_mode;
  ie_field_s<ps_service_not_available_e>                     ps_service_not_available;

  // sequence methods
  ho_required_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverRequired ::= SEQUENCE
struct ho_required_s {
  bool                      ext = false;
  ho_required_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MMEPagingTarget ::= CHOICE
struct mme_paging_target_c {
  struct types_opts {
    enum options { global_enb_id, tai, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  mme_paging_target_c() = default;
  mme_paging_target_c(const mme_paging_target_c& other);
  mme_paging_target_c& operator=(const mme_paging_target_c& other);
  ~mme_paging_target_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  global_enb_id_s& global_enb_id()
  {
    assert_choice_type("global-ENB-ID", type_.to_string(), "MMEPagingTarget");
    return c.get<global_enb_id_s>();
  }
  tai_s& tai()
  {
    assert_choice_type("tAI", type_.to_string(), "MMEPagingTarget");
    return c.get<tai_s>();
  }
  const global_enb_id_s& global_enb_id() const
  {
    assert_choice_type("global-ENB-ID", type_.to_string(), "MMEPagingTarget");
    return c.get<global_enb_id_s>();
  }
  const tai_s& tai() const
  {
    assert_choice_type("tAI", type_.to_string(), "MMEPagingTarget");
    return c.get<tai_s>();
  }
  global_enb_id_s& set_global_enb_id()
  {
    set(types::global_enb_id);
    return c.get<global_enb_id_s>();
  }
  tai_s& set_tai()
  {
    set(types::tai);
    return c.get<tai_s>();
  }

private:
  types                                   type_;
  choice_buffer_t<global_enb_id_s, tai_s> c;

  void destroy_();
};

// RecommendedENBItem-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o recommended_enb_item_ext_ies_o;

typedef protocol_ext_container_empty_l recommended_enb_item_ext_ies_container;

// RecommendedENBItem ::= SEQUENCE
struct recommended_enb_item_s {
  bool                                   ext             = false;
  bool                                   ie_exts_present = false;
  mme_paging_target_c                    mme_paging_target;
  recommended_enb_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RecommendedENBItemIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct recommended_enb_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { recommended_enb_item, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::recommended_enb_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    recommended_enb_item_s&       recommended_enb_item() { return c; }
    const recommended_enb_item_s& recommended_enb_item() const { return c; }

  private:
    recommended_enb_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RecommendedENBList ::= SEQUENCE (SIZE (1..16)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES : IEsSetParam}
using recommended_enb_list_l = bounded_array<protocol_ie_single_container_s<recommended_enb_item_ies_o>, 16>;

// RecommendedENBsForPaging-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o recommended_enbs_for_paging_ext_ies_o;

// InformationOnRecommendedCellsAndENBsForPaging-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o info_on_recommended_cells_and_enbs_for_paging_ext_ies_o;

typedef protocol_ext_container_empty_l recommended_enbs_for_paging_ext_ies_container;

// RecommendedENBsForPaging ::= SEQUENCE
struct recommended_enbs_for_paging_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  recommended_enb_list_l                        recommended_enb_list;
  recommended_enbs_for_paging_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l info_on_recommended_cells_and_enbs_for_paging_ext_ies_container;

// InformationOnRecommendedCellsAndENBsForPaging ::= SEQUENCE
struct info_on_recommended_cells_and_enbs_for_paging_s {
  bool                                                            ext             = false;
  bool                                                            ie_exts_present = false;
  recommended_cells_for_paging_s                                  recommended_cells_for_paging;
  recommended_enbs_for_paging_s                                   recommend_enbs_for_paging;
  info_on_recommended_cells_and_enbs_for_paging_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialContextSetupFailureIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct init_context_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, cause, crit_diagnostics, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 mme_ue_s1ap_id();
    uint32_t&                 enb_ue_s1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           mme_ue_s1ap_id() const;
    const uint32_t&           enb_ue_s1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

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

struct init_context_setup_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<cause_c>                                        cause;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;

  // sequence methods
  init_context_setup_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialContextSetupFailure ::= SEQUENCE
struct init_context_setup_fail_s {
  bool                                  ext = false;
  init_context_setup_fail_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AdditionalCSFallbackIndicator ::= ENUMERATED
struct add_cs_fallback_ind_opts {
  enum options { no_restrict, restrict, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<add_cs_fallback_ind_opts, true> add_cs_fallback_ind_e;

// CSFallbackIndicator ::= ENUMERATED
struct cs_fallback_ind_opts {
  enum options { cs_fallback_required, /*...*/ cs_fallback_high_prio, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cs_fallback_ind_opts, true, 1> cs_fallback_ind_e;

// InitialContextSetupRequestIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct init_context_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        ueaggregate_maximum_bitrate,
        erab_to_be_setup_list_ctxt_su_req,
        ue_security_cap,
        security_key,
        trace_activation,
        ho_restrict_list,
        ue_radio_cap,
        subscriber_profile_idfor_rfp,
        cs_fallback_ind,
        srvcc_operation_possible,
        csg_membership_status,
        registered_lai,
        gummei_id,
        mme_ue_s1ap_id_minus2,
        management_based_mdt_allowed,
        management_based_mdtplmn_list,
        add_cs_fallback_ind,
        masked_imeisv,
        expected_ue_behaviour,
        pro_se_authorized,
        ueuser_plane_cio_tsupport_ind,
        v2xservices_authorized,
        ue_sidelink_aggregate_maximum_bitrate,
        enhanced_coverage_restricted,
        ce_mode_brestricted,
        pending_data_ind,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                      mme_ue_s1ap_id();
    uint32_t&                                      enb_ue_s1ap_id();
    ue_aggregate_maximum_bitrate_s&                ueaggregate_maximum_bitrate();
    erab_to_be_setup_list_ctxt_su_req_l&           erab_to_be_setup_list_ctxt_su_req();
    ue_security_cap_s&                             ue_security_cap();
    fixed_bitstring<256, false, true>&             security_key();
    trace_activation_s&                            trace_activation();
    ho_restrict_list_s&                            ho_restrict_list();
    unbounded_octstring<true>&                     ue_radio_cap();
    uint16_t&                                      subscriber_profile_idfor_rfp();
    cs_fallback_ind_e&                             cs_fallback_ind();
    srvcc_operation_possible_e&                    srvcc_operation_possible();
    csg_membership_status_e&                       csg_membership_status();
    lai_s&                                         registered_lai();
    gummei_s&                                      gummei_id();
    uint64_t&                                      mme_ue_s1ap_id_minus2();
    management_based_mdt_allowed_e&                management_based_mdt_allowed();
    mdtplmn_list_l&                                management_based_mdtplmn_list();
    add_cs_fallback_ind_e&                         add_cs_fallback_ind();
    fixed_bitstring<64, false, true>&              masked_imeisv();
    expected_ue_behaviour_s&                       expected_ue_behaviour();
    pro_se_authorized_s&                           pro_se_authorized();
    ueuser_plane_cio_tsupport_ind_e&               ueuser_plane_cio_tsupport_ind();
    v2xservices_authorized_s&                      v2xservices_authorized();
    ue_sidelink_aggregate_maximum_bitrate_s&       ue_sidelink_aggregate_maximum_bitrate();
    enhanced_coverage_restricted_e&                enhanced_coverage_restricted();
    ce_mode_brestricted_e&                         ce_mode_brestricted();
    pending_data_ind_e&                            pending_data_ind();
    const uint64_t&                                mme_ue_s1ap_id() const;
    const uint32_t&                                enb_ue_s1ap_id() const;
    const ue_aggregate_maximum_bitrate_s&          ueaggregate_maximum_bitrate() const;
    const erab_to_be_setup_list_ctxt_su_req_l&     erab_to_be_setup_list_ctxt_su_req() const;
    const ue_security_cap_s&                       ue_security_cap() const;
    const fixed_bitstring<256, false, true>&       security_key() const;
    const trace_activation_s&                      trace_activation() const;
    const ho_restrict_list_s&                      ho_restrict_list() const;
    const unbounded_octstring<true>&               ue_radio_cap() const;
    const uint16_t&                                subscriber_profile_idfor_rfp() const;
    const cs_fallback_ind_e&                       cs_fallback_ind() const;
    const srvcc_operation_possible_e&              srvcc_operation_possible() const;
    const csg_membership_status_e&                 csg_membership_status() const;
    const lai_s&                                   registered_lai() const;
    const gummei_s&                                gummei_id() const;
    const uint64_t&                                mme_ue_s1ap_id_minus2() const;
    const management_based_mdt_allowed_e&          management_based_mdt_allowed() const;
    const mdtplmn_list_l&                          management_based_mdtplmn_list() const;
    const add_cs_fallback_ind_e&                   add_cs_fallback_ind() const;
    const fixed_bitstring<64, false, true>&        masked_imeisv() const;
    const expected_ue_behaviour_s&                 expected_ue_behaviour() const;
    const pro_se_authorized_s&                     pro_se_authorized() const;
    const ueuser_plane_cio_tsupport_ind_e&         ueuser_plane_cio_tsupport_ind() const;
    const v2xservices_authorized_s&                v2xservices_authorized() const;
    const ue_sidelink_aggregate_maximum_bitrate_s& ue_sidelink_aggregate_maximum_bitrate() const;
    const enhanced_coverage_restricted_e&          enhanced_coverage_restricted() const;
    const ce_mode_brestricted_e&                   ce_mode_brestricted() const;
    const pending_data_ind_e&                      pending_data_ind() const;

  private:
    types type_;
    choice_buffer_t<erab_to_be_setup_list_ctxt_su_req_l,
                    expected_ue_behaviour_s,
                    fixed_bitstring<256, false, true>,
                    gummei_s,
                    ho_restrict_list_s,
                    lai_s,
                    mdtplmn_list_l,
                    pro_se_authorized_s,
                    trace_activation_s,
                    ue_aggregate_maximum_bitrate_s,
                    ue_security_cap_s,
                    ue_sidelink_aggregate_maximum_bitrate_s,
                    unbounded_octstring<true>,
                    v2xservices_authorized_s>
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

struct init_context_setup_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       trace_activation_present                      = false;
  bool                                                       ho_restrict_list_present                      = false;
  bool                                                       ue_radio_cap_present                          = false;
  bool                                                       subscriber_profile_idfor_rfp_present          = false;
  bool                                                       cs_fallback_ind_present                       = false;
  bool                                                       srvcc_operation_possible_present              = false;
  bool                                                       csg_membership_status_present                 = false;
  bool                                                       registered_lai_present                        = false;
  bool                                                       gummei_id_present                             = false;
  bool                                                       mme_ue_s1ap_id_minus2_present                 = false;
  bool                                                       management_based_mdt_allowed_present          = false;
  bool                                                       management_based_mdtplmn_list_present         = false;
  bool                                                       add_cs_fallback_ind_present                   = false;
  bool                                                       masked_imeisv_present                         = false;
  bool                                                       expected_ue_behaviour_present                 = false;
  bool                                                       pro_se_authorized_present                     = false;
  bool                                                       ueuser_plane_cio_tsupport_ind_present         = false;
  bool                                                       v2xservices_authorized_present                = false;
  bool                                                       ue_sidelink_aggregate_maximum_bitrate_present = false;
  bool                                                       enhanced_coverage_restricted_present          = false;
  bool                                                       ce_mode_brestricted_present                   = false;
  bool                                                       pending_data_ind_present                      = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<ue_aggregate_maximum_bitrate_s>                 ueaggregate_maximum_bitrate;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_to_be_setup_item_ctxt_su_req_ies_o>, 1, 256, true> >
                                                                 erab_to_be_setup_list_ctxt_su_req;
  ie_field_s<ue_security_cap_s>                                  ue_security_cap;
  ie_field_s<fixed_bitstring<256, false, true> >                 security_key;
  ie_field_s<trace_activation_s>                                 trace_activation;
  ie_field_s<ho_restrict_list_s>                                 ho_restrict_list;
  ie_field_s<unbounded_octstring<true> >                         ue_radio_cap;
  ie_field_s<integer<uint16_t, 1, 256, false, true> >            subscriber_profile_idfor_rfp;
  ie_field_s<cs_fallback_ind_e>                                  cs_fallback_ind;
  ie_field_s<srvcc_operation_possible_e>                         srvcc_operation_possible;
  ie_field_s<csg_membership_status_e>                            csg_membership_status;
  ie_field_s<lai_s>                                              registered_lai;
  ie_field_s<gummei_s>                                           gummei_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >     mme_ue_s1ap_id_minus2;
  ie_field_s<management_based_mdt_allowed_e>                     management_based_mdt_allowed;
  ie_field_s<dyn_seq_of<fixed_octstring<3, true>, 1, 16, true> > management_based_mdtplmn_list;
  ie_field_s<add_cs_fallback_ind_e>                              add_cs_fallback_ind;
  ie_field_s<fixed_bitstring<64, false, true> >                  masked_imeisv;
  ie_field_s<expected_ue_behaviour_s>                            expected_ue_behaviour;
  ie_field_s<pro_se_authorized_s>                                pro_se_authorized;
  ie_field_s<ueuser_plane_cio_tsupport_ind_e>                    ueuser_plane_cio_tsupport_ind;
  ie_field_s<v2xservices_authorized_s>                           v2xservices_authorized;
  ie_field_s<ue_sidelink_aggregate_maximum_bitrate_s>            ue_sidelink_aggregate_maximum_bitrate;
  ie_field_s<enhanced_coverage_restricted_e>                     enhanced_coverage_restricted;
  ie_field_s<ce_mode_brestricted_e>                              ce_mode_brestricted;
  ie_field_s<pending_data_ind_e>                                 pending_data_ind;

  // sequence methods
  init_context_setup_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialContextSetupRequest ::= SEQUENCE
struct init_context_setup_request_s {
  bool                                     ext = false;
  init_context_setup_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialContextSetupResponseIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct init_context_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        erab_setup_list_ctxt_su_res,
        erab_failed_to_setup_list_ctxt_su_res,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                            mme_ue_s1ap_id();
    uint32_t&                            enb_ue_s1ap_id();
    erab_setup_list_ctxt_su_res_l&       erab_setup_list_ctxt_su_res();
    erab_list_l&                         erab_failed_to_setup_list_ctxt_su_res();
    crit_diagnostics_s&                  crit_diagnostics();
    const uint64_t&                      mme_ue_s1ap_id() const;
    const uint32_t&                      enb_ue_s1ap_id() const;
    const erab_setup_list_ctxt_su_res_l& erab_setup_list_ctxt_su_res() const;
    const erab_list_l&                   erab_failed_to_setup_list_ctxt_su_res() const;
    const crit_diagnostics_s&            crit_diagnostics() const;

  private:
    types                                                                           type_;
    choice_buffer_t<crit_diagnostics_s, erab_list_l, erab_setup_list_ctxt_su_res_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct init_context_setup_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       erab_failed_to_setup_list_ctxt_su_res_present = false;
  bool                                                       crit_diagnostics_present                      = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_setup_item_ctxt_su_res_ies_o>, 1, 256, true> >
      erab_setup_list_ctxt_su_res;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_item_ies_o>, 1, 256, true> >
                                 erab_failed_to_setup_list_ctxt_su_res;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  init_context_setup_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialContextSetupResponse ::= SEQUENCE
struct init_context_setup_resp_s {
  bool                                  ext = false;
  init_context_setup_resp_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Coverage-Level ::= ENUMERATED
struct coverage_level_opts {
  enum options { extendedcoverage, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<coverage_level_opts, true> coverage_level_e;

// GUMMEIType ::= ENUMERATED
struct gummei_type_opts {
  enum options { native, mapped, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<gummei_type_opts, true> gummei_type_e;

// RRC-Establishment-Cause ::= ENUMERATED
struct rrc_establishment_cause_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    // ...
    delay_tolerant_access,
    mo_voice_call,
    mo_exception_data,
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<rrc_establishment_cause_opts, true, 3> rrc_establishment_cause_e;

// RelayNode-Indicator ::= ENUMERATED
struct relay_node_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<relay_node_ind_opts, true> relay_node_ind_e;

// InitialUEMessage-IEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct init_ue_msg_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        enb_ue_s1ap_id,
        nas_pdu,
        tai,
        eutran_cgi,
        rrc_establishment_cause,
        s_tmsi,
        csg_id,
        gummei_id,
        cell_access_mode,
        gw_transport_layer_address,
        relay_node_ind,
        gummei_type,
        tunnel_info_for_bbf,
        sipto_l_gw_transport_layer_address,
        lhn_id,
        mme_group_id,
        ue_usage_type,
        ce_mode_b_support_ind,
        dcn_id,
        coverage_level,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&                                    enb_ue_s1ap_id();
    unbounded_octstring<true>&                   nas_pdu();
    tai_s&                                       tai();
    eutran_cgi_s&                                eutran_cgi();
    rrc_establishment_cause_e&                   rrc_establishment_cause();
    s_tmsi_s&                                    s_tmsi();
    fixed_bitstring<27, false, true>&            csg_id();
    gummei_s&                                    gummei_id();
    cell_access_mode_e&                          cell_access_mode();
    bounded_bitstring<1, 160, true, true>&       gw_transport_layer_address();
    relay_node_ind_e&                            relay_node_ind();
    gummei_type_e&                               gummei_type();
    tunnel_info_s&                               tunnel_info_for_bbf();
    bounded_bitstring<1, 160, true, true>&       sipto_l_gw_transport_layer_address();
    unbounded_octstring<true>&                   lhn_id();
    fixed_octstring<2, true>&                    mme_group_id();
    uint16_t&                                    ue_usage_type();
    ce_mode_b_support_ind_e&                     ce_mode_b_support_ind();
    uint32_t&                                    dcn_id();
    coverage_level_e&                            coverage_level();
    const uint32_t&                              enb_ue_s1ap_id() const;
    const unbounded_octstring<true>&             nas_pdu() const;
    const tai_s&                                 tai() const;
    const eutran_cgi_s&                          eutran_cgi() const;
    const rrc_establishment_cause_e&             rrc_establishment_cause() const;
    const s_tmsi_s&                              s_tmsi() const;
    const fixed_bitstring<27, false, true>&      csg_id() const;
    const gummei_s&                              gummei_id() const;
    const cell_access_mode_e&                    cell_access_mode() const;
    const bounded_bitstring<1, 160, true, true>& gw_transport_layer_address() const;
    const relay_node_ind_e&                      relay_node_ind() const;
    const gummei_type_e&                         gummei_type() const;
    const tunnel_info_s&                         tunnel_info_for_bbf() const;
    const bounded_bitstring<1, 160, true, true>& sipto_l_gw_transport_layer_address() const;
    const unbounded_octstring<true>&             lhn_id() const;
    const fixed_octstring<2, true>&              mme_group_id() const;
    const uint16_t&                              ue_usage_type() const;
    const ce_mode_b_support_ind_e&               ce_mode_b_support_ind() const;
    const uint32_t&                              dcn_id() const;
    const coverage_level_e&                      coverage_level() const;

  private:
    types type_;
    choice_buffer_t<bounded_bitstring<1, 160, true, true>,
                    eutran_cgi_s,
                    fixed_bitstring<27, false, true>,
                    fixed_octstring<2, true>,
                    gummei_s,
                    s_tmsi_s,
                    tai_s,
                    tunnel_info_s,
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

struct init_ue_msg_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                     s_tmsi_present                             = false;
  bool                                                     csg_id_present                             = false;
  bool                                                     gummei_id_present                          = false;
  bool                                                     cell_access_mode_present                   = false;
  bool                                                     gw_transport_layer_address_present         = false;
  bool                                                     relay_node_ind_present                     = false;
  bool                                                     gummei_type_present                        = false;
  bool                                                     tunnel_info_for_bbf_present                = false;
  bool                                                     sipto_l_gw_transport_layer_address_present = false;
  bool                                                     lhn_id_present                             = false;
  bool                                                     mme_group_id_present                       = false;
  bool                                                     ue_usage_type_present                      = false;
  bool                                                     ce_mode_b_support_ind_present              = false;
  bool                                                     dcn_id_present                             = false;
  bool                                                     coverage_level_present                     = false;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> > enb_ue_s1ap_id;
  ie_field_s<unbounded_octstring<true> >                   nas_pdu;
  ie_field_s<tai_s>                                        tai;
  ie_field_s<eutran_cgi_s>                                 eutran_cgi;
  ie_field_s<rrc_establishment_cause_e>                    rrc_establishment_cause;
  ie_field_s<s_tmsi_s>                                     s_tmsi;
  ie_field_s<fixed_bitstring<27, false, true> >            csg_id;
  ie_field_s<gummei_s>                                     gummei_id;
  ie_field_s<cell_access_mode_e>                           cell_access_mode;
  ie_field_s<bounded_bitstring<1, 160, true, true> >       gw_transport_layer_address;
  ie_field_s<relay_node_ind_e>                             relay_node_ind;
  ie_field_s<gummei_type_e>                                gummei_type;
  ie_field_s<tunnel_info_s>                                tunnel_info_for_bbf;
  ie_field_s<bounded_bitstring<1, 160, true, true> >       sipto_l_gw_transport_layer_address;
  ie_field_s<unbounded_octstring<true> >                   lhn_id;
  ie_field_s<fixed_octstring<2, true> >                    mme_group_id;
  ie_field_s<integer<uint16_t, 0, 255, false, true> >      ue_usage_type;
  ie_field_s<ce_mode_b_support_ind_e>                      ce_mode_b_support_ind;
  ie_field_s<integer<uint32_t, 0, 65535, false, true> >    dcn_id;
  ie_field_s<coverage_level_e>                             coverage_level;

  // sequence methods
  init_ue_msg_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialUEMessage ::= SEQUENCE
struct init_ue_msg_s {
  bool                      ext = false;
  init_ue_msg_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-associatedLogicalS1-ConnectionItemExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o ue_associated_lc_s1_conn_item_ext_ies_o;

// TAIItemExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o tai_item_ext_ies_o;

typedef protocol_ext_container_empty_l ue_associated_lc_s1_conn_item_ext_ies_container;

// UE-associatedLogicalS1-ConnectionItem ::= SEQUENCE
struct ue_associated_lc_s1_conn_item_s {
  bool                                            ext                    = false;
  bool                                            mme_ue_s1ap_id_present = false;
  bool                                            enb_ue_s1ap_id_present = false;
  bool                                            ie_exts_present        = false;
  uint64_t                                        mme_ue_s1ap_id         = 0;
  uint32_t                                        enb_ue_s1ap_id         = 0;
  ue_associated_lc_s1_conn_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServedDCNsItem-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o served_dcns_item_ext_ies_o;

// ServedGUMMEIsItem-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o served_gummeis_item_ext_ies_o;

// ServedGroupIDs ::= SEQUENCE (SIZE (1..65535)) OF OCTET STRING (SIZE (2))
using served_group_ids_l = dyn_array<fixed_octstring<2, true> >;

// ServedMMECs ::= SEQUENCE (SIZE (1..256)) OF OCTET STRING (SIZE (1))
using served_mmecs_l = dyn_array<fixed_octstring<1, true> >;

// ServedPLMNs ::= SEQUENCE (SIZE (1..32)) OF OCTET STRING (SIZE (3))
using served_plmns_l = bounded_array<fixed_octstring<3, true>, 32>;

typedef protocol_ext_container_empty_l tai_item_ext_ies_container;

// TAIItem ::= SEQUENCE
struct tai_item_s {
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  tai_s                      tai;
  tai_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-S1AP-ID-pair-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o ue_s1ap_id_pair_ext_ies_o;

// UE-associatedLogicalS1-ConnectionItemRes ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_associated_lc_s1_conn_item_res_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_s1_conn_item, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_associated_lc_s1_conn_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_associated_lc_s1_conn_item_s&       ue_associated_lc_s1_conn_item() { return c; }
    const ue_associated_lc_s1_conn_item_s& ue_associated_lc_s1_conn_item() const { return c; }

  private:
    ue_associated_lc_s1_conn_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NB-IoT-Paging-eDRX-Cycle ::= ENUMERATED
struct nb_io_t_paging_e_drx_cycle_opts {
  enum options {
    hf2,
    hf4,
    hf6,
    hf8,
    hf10,
    hf12,
    hf14,
    hf16,
    hf32,
    hf64,
    hf128,
    hf256,
    hf512,
    hf1024,
    /*...*/ nulltype
  } value;
  typedef uint16_t number_type;

  std::string to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<nb_io_t_paging_e_drx_cycle_opts, true> nb_io_t_paging_e_drx_cycle_e;

// NB-IoT-Paging-eDRXInformation-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o nb_io_t_paging_e_drx_info_ext_ies_o;

// NB-IoT-PagingTimeWindow ::= ENUMERATED
struct nb_io_t_paging_time_win_opts {
  enum options { s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<nb_io_t_paging_time_win_opts, true> nb_io_t_paging_time_win_e;

// OverloadAction ::= ENUMERATED
struct overload_action_opts {
  enum options {
    reject_non_emergency_mo_dt,
    reject_rrc_cr_sig,
    permit_emergency_sessions_and_mobile_terminated_services_only,
    // ...
    permit_high_prio_sessions_and_mobile_terminated_services_only,
    reject_delay_tolerant_access,
    permit_high_prio_sessions_and_exception_report_and_mobile_terminated_services_only,
    not_accept_mo_data_or_delay_tolerant_access_from_cp_cio_t,
    nulltype
  } value;

  std::string to_string() const;
};
typedef enumerated<overload_action_opts, true, 4> overload_action_e;

// Paging-eDRX-Cycle ::= ENUMERATED
struct paging_e_drx_cycle_opts {
  enum options {
    hfhalf,
    hf1,
    hf2,
    hf4,
    hf6,
    hf8,
    hf10,
    hf12,
    hf14,
    hf16,
    hf32,
    hf64,
    hf128,
    hf256,
    /*...*/ nulltype
  } value;
  typedef float number_type;

  std::string to_string() const;
  float       to_number() const;
  std::string to_number_string() const;
};
typedef enumerated<paging_e_drx_cycle_opts, true> paging_e_drx_cycle_e;

// Paging-eDRXInformation-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o paging_e_drx_info_ext_ies_o;

// PagingTimeWindow ::= ENUMERATED
struct paging_time_win_opts {
  enum options { s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  std::string to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<paging_time_win_opts, true> paging_time_win_e;

// ResetAll ::= ENUMERATED
struct reset_all_opts {
  enum options { reset_all, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<reset_all_opts, true> reset_all_e;

typedef protocol_ext_container_empty_l served_dcns_item_ext_ies_container;

// ServedDCNsItem ::= SEQUENCE
struct served_dcns_item_s {
  bool                               ext                   = false;
  bool                               ie_exts_present       = false;
  uint32_t                           dcn_id                = 0;
  uint16_t                           relative_dcn_capacity = 0;
  served_dcns_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

typedef protocol_ext_container_empty_l served_gummeis_item_ext_ies_container;

// ServedGUMMEIsItem ::= SEQUENCE
struct served_gummeis_item_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  served_plmns_l                        served_plmns;
  served_group_ids_l                    served_group_ids;
  served_mmecs_l                        served_mmecs;
  served_gummeis_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAIItemIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct tai_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { tai_item, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::tai_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    tai_item_s&       tai_item() { return c; }
    const tai_item_s& tai_item() const { return c; }

  private:
    tai_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TAIListforWarning ::= SEQUENCE (SIZE (1..65535)) OF TAI
using tai_listfor_warning_l = dyn_array<tai_s>;

typedef protocol_ext_container_empty_l ue_s1ap_id_pair_ext_ies_container;

// UE-S1AP-ID-pair ::= SEQUENCE
struct ue_s1ap_id_pair_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  uint64_t                          mme_ue_s1ap_id  = 0;
  uint32_t                          enb_ue_s1ap_id  = 0;
  ue_s1ap_id_pair_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-associatedLogicalS1-ConnectionItemResAck ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_associated_lc_s1_conn_item_res_ack_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_s1_conn_item, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_associated_lc_s1_conn_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_associated_lc_s1_conn_item_s&       ue_associated_lc_s1_conn_item() { return c; }
    const ue_associated_lc_s1_conn_item_s& ue_associated_lc_s1_conn_item() const { return c; }

  private:
    ue_associated_lc_s1_conn_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-associatedLogicalS1-ConnectionListRes ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES
// : IEsSetParam}
using ue_associated_lc_s1_conn_list_res_l =
    dyn_array<protocol_ie_single_container_s<ue_associated_lc_s1_conn_item_res_o> >;

// CNDomain ::= ENUMERATED
struct cn_domain_opts {
  enum options { ps, cs, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cn_domain_opts> cn_domain_e;

// Cdma2000HORequiredIndication ::= ENUMERATED
struct cdma2000_ho_required_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cdma2000_ho_required_ind_opts, true> cdma2000_ho_required_ind_e;

// ConcurrentWarningMessageIndicator ::= ENUMERATED
struct concurrent_warning_msg_ind_opts {
  enum options { true_value, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<concurrent_warning_msg_ind_opts> concurrent_warning_msg_ind_e;

// GWContextReleaseIndication ::= ENUMERATED
struct gw_context_release_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<gw_context_release_ind_opts, true> gw_context_release_ind_e;

// KillAllWarningMessages ::= ENUMERATED
struct kill_all_warning_msgs_opts {
  enum options { true_value, nulltype } value;

  std::string to_string() const;
};
typedef enumerated<kill_all_warning_msgs_opts> kill_all_warning_msgs_e;

// MMERelaySupportIndicator ::= ENUMERATED
struct mme_relay_support_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<mme_relay_support_ind_opts, true> mme_relay_support_ind_e;

typedef protocol_ext_container_empty_l nb_io_t_paging_e_drx_info_ext_ies_container;

// NB-IoT-Paging-eDRXInformation ::= SEQUENCE
struct nb_io_t_paging_e_drx_info_s {
  bool                                        ext                             = false;
  bool                                        nb_io_t_paging_time_win_present = false;
  bool                                        ie_exts_present                 = false;
  nb_io_t_paging_e_drx_cycle_e                nb_io_t_paging_e_drx_cycle;
  nb_io_t_paging_time_win_e                   nb_io_t_paging_time_win;
  nb_io_t_paging_e_drx_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OverloadResponse ::= CHOICE
struct overload_resp_c {
  struct types_opts {
    enum options { overload_action, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  types       type() const { return types::overload_action; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  overload_action_e&       overload_action() { return c; }
  const overload_action_e& overload_action() const { return c; }

private:
  overload_action_e c;
};

// PWSfailedECGIList ::= SEQUENCE (SIZE (1..256)) OF EUTRAN-CGI
using pw_sfailed_ecgi_list_l = dyn_array<eutran_cgi_s>;

typedef protocol_ext_container_empty_l paging_e_drx_info_ext_ies_container;

// Paging-eDRXInformation ::= SEQUENCE
struct paging_e_drx_info_s {
  bool                                ext                     = false;
  bool                                paging_time_win_present = false;
  bool                                ie_exts_present         = false;
  paging_e_drx_cycle_e                paging_e_drx_cycle;
  paging_time_win_e                   paging_time_win;
  paging_e_drx_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// ResetType ::= CHOICE
struct reset_type_c {
  struct types_opts {
    enum options { s1_interface, part_of_s1_interface, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  reset_type_c() = default;
  reset_type_c(const reset_type_c& other);
  reset_type_c& operator=(const reset_type_c& other);
  ~reset_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  reset_all_e& s1_interface()
  {
    assert_choice_type("s1-Interface", type_.to_string(), "ResetType");
    return c.get<reset_all_e>();
  }
  ue_associated_lc_s1_conn_list_res_l& part_of_s1_interface()
  {
    assert_choice_type("partOfS1-Interface", type_.to_string(), "ResetType");
    return c.get<ue_associated_lc_s1_conn_list_res_l>();
  }
  const reset_all_e& s1_interface() const
  {
    assert_choice_type("s1-Interface", type_.to_string(), "ResetType");
    return c.get<reset_all_e>();
  }
  const ue_associated_lc_s1_conn_list_res_l& part_of_s1_interface() const
  {
    assert_choice_type("partOfS1-Interface", type_.to_string(), "ResetType");
    return c.get<ue_associated_lc_s1_conn_list_res_l>();
  }
  reset_all_e& set_s1_interface()
  {
    set(types::s1_interface);
    return c.get<reset_all_e>();
  }
  ue_associated_lc_s1_conn_list_res_l& set_part_of_s1_interface()
  {
    set(types::part_of_s1_interface);
    return c.get<ue_associated_lc_s1_conn_list_res_l>();
  }

private:
  types                                                type_;
  choice_buffer_t<ue_associated_lc_s1_conn_list_res_l> c;

  void destroy_();
};

// SRVCCOperationNotPossible ::= ENUMERATED
struct srvcc_operation_not_possible_opts {
  enum options { not_possible, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<srvcc_operation_not_possible_opts, true> srvcc_operation_not_possible_e;

// ServedDCNs ::= SEQUENCE (SIZE (0..32)) OF ServedDCNsItem
using served_dcns_l = dyn_array<served_dcns_item_s>;

// ServedGUMMEIs ::= SEQUENCE (SIZE (1..8)) OF ServedGUMMEIsItem
using served_gummeis_l = dyn_array<served_gummeis_item_s>;

// TAIList ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES : IEsSetParam}
using tai_list_l = dyn_array<protocol_ie_single_container_s<tai_item_ies_o> >;

// TAIListForRestart ::= SEQUENCE (SIZE (1..2048)) OF TAI
using tai_list_for_restart_l = dyn_array<tai_s>;

// UE-RetentionInformation ::= ENUMERATED
struct ue_retention_info_opts {
  enum options { ues_retained, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ue_retention_info_opts, true> ue_retention_info_e;

// UE-S1AP-IDs ::= CHOICE
struct ue_s1ap_ids_c {
  struct types_opts {
    enum options { ue_s1ap_id_pair, mme_ue_s1ap_id, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  ue_s1ap_ids_c() = default;
  ue_s1ap_ids_c(const ue_s1ap_ids_c& other);
  ue_s1ap_ids_c& operator=(const ue_s1ap_ids_c& other);
  ~ue_s1ap_ids_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ue_s1ap_id_pair_s& ue_s1ap_id_pair()
  {
    assert_choice_type("uE-S1AP-ID-pair", type_.to_string(), "UE-S1AP-IDs");
    return c.get<ue_s1ap_id_pair_s>();
  }
  uint64_t& mme_ue_s1ap_id()
  {
    assert_choice_type("mME-UE-S1AP-ID", type_.to_string(), "UE-S1AP-IDs");
    return c.get<uint64_t>();
  }
  const ue_s1ap_id_pair_s& ue_s1ap_id_pair() const
  {
    assert_choice_type("uE-S1AP-ID-pair", type_.to_string(), "UE-S1AP-IDs");
    return c.get<ue_s1ap_id_pair_s>();
  }
  const uint64_t& mme_ue_s1ap_id() const
  {
    assert_choice_type("mME-UE-S1AP-ID", type_.to_string(), "UE-S1AP-IDs");
    return c.get<uint64_t>();
  }
  ue_s1ap_id_pair_s& set_ue_s1ap_id_pair()
  {
    set(types::ue_s1ap_id_pair);
    return c.get<ue_s1ap_id_pair_s>();
  }
  uint64_t& set_mme_ue_s1ap_id()
  {
    set(types::mme_ue_s1ap_id);
    return c.get<uint64_t>();
  }

private:
  types                              type_;
  choice_buffer_t<ue_s1ap_id_pair_s> c;

  void destroy_();
};

// UE-associatedLogicalS1-ConnectionListResAck ::= SEQUENCE (SIZE (1..256)) OF
// ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES : IEsSetParam}
using ue_associated_lc_s1_conn_list_res_ack_l =
    dyn_array<protocol_ie_single_container_s<ue_associated_lc_s1_conn_item_res_ack_o> >;

// UEPagingID ::= CHOICE
struct ue_paging_id_c {
  struct types_opts {
    enum options { s_tmsi, imsi, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  ue_paging_id_c() = default;
  ue_paging_id_c(const ue_paging_id_c& other);
  ue_paging_id_c& operator=(const ue_paging_id_c& other);
  ~ue_paging_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  s_tmsi_s& s_tmsi()
  {
    assert_choice_type("s-TMSI", type_.to_string(), "UEPagingID");
    return c.get<s_tmsi_s>();
  }
  unbounded_octstring<true>& imsi()
  {
    assert_choice_type("iMSI", type_.to_string(), "UEPagingID");
    return c.get<unbounded_octstring<true> >();
  }
  const s_tmsi_s& s_tmsi() const
  {
    assert_choice_type("s-TMSI", type_.to_string(), "UEPagingID");
    return c.get<s_tmsi_s>();
  }
  const unbounded_octstring<true>& imsi() const
  {
    assert_choice_type("iMSI", type_.to_string(), "UEPagingID");
    return c.get<unbounded_octstring<true> >();
  }
  s_tmsi_s& set_s_tmsi()
  {
    set(types::s_tmsi);
    return c.get<s_tmsi_s>();
  }
  unbounded_octstring<true>& set_imsi()
  {
    set(types::imsi);
    return c.get<unbounded_octstring<true> >();
  }

private:
  types                                                 type_;
  choice_buffer_t<s_tmsi_s, unbounded_octstring<true> > c;

  void destroy_();
};

// VoiceSupportMatchIndicator ::= ENUMERATED
struct voice_support_match_ind_opts {
  enum options { supported, not_supported, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<voice_support_match_ind_opts, true> voice_support_match_ind_e;

// WarningAreaList ::= CHOICE
struct warning_area_list_c {
  struct types_opts {
    enum options { cell_id_list, tracking_area_listfor_warning, emergency_area_id_list, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  warning_area_list_c() = default;
  warning_area_list_c(const warning_area_list_c& other);
  warning_area_list_c& operator=(const warning_area_list_c& other);
  ~warning_area_list_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ecgi_list_l& cell_id_list()
  {
    assert_choice_type("cellIDList", type_.to_string(), "WarningAreaList");
    return c.get<ecgi_list_l>();
  }
  tai_listfor_warning_l& tracking_area_listfor_warning()
  {
    assert_choice_type("trackingAreaListforWarning", type_.to_string(), "WarningAreaList");
    return c.get<tai_listfor_warning_l>();
  }
  emergency_area_id_list_l& emergency_area_id_list()
  {
    assert_choice_type("emergencyAreaIDList", type_.to_string(), "WarningAreaList");
    return c.get<emergency_area_id_list_l>();
  }
  const ecgi_list_l& cell_id_list() const
  {
    assert_choice_type("cellIDList", type_.to_string(), "WarningAreaList");
    return c.get<ecgi_list_l>();
  }
  const tai_listfor_warning_l& tracking_area_listfor_warning() const
  {
    assert_choice_type("trackingAreaListforWarning", type_.to_string(), "WarningAreaList");
    return c.get<tai_listfor_warning_l>();
  }
  const emergency_area_id_list_l& emergency_area_id_list() const
  {
    assert_choice_type("emergencyAreaIDList", type_.to_string(), "WarningAreaList");
    return c.get<emergency_area_id_list_l>();
  }
  ecgi_list_l& set_cell_id_list()
  {
    set(types::cell_id_list);
    return c.get<ecgi_list_l>();
  }
  tai_listfor_warning_l& set_tracking_area_listfor_warning()
  {
    set(types::tracking_area_listfor_warning);
    return c.get<tai_listfor_warning_l>();
  }
  emergency_area_id_list_l& set_emergency_area_id_list()
  {
    set(types::emergency_area_id_list);
    return c.get<emergency_area_id_list_l>();
  }

private:
  types                                                                         type_;
  choice_buffer_t<ecgi_list_l, emergency_area_id_list_l, tai_listfor_warning_l> c;

  void destroy_();
};

// KillRequestIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct kill_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { msg_id, serial_num, warning_area_list, kill_all_warning_msgs, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>&       msg_id();
    fixed_bitstring<16, false, true>&       serial_num();
    warning_area_list_c&                    warning_area_list();
    kill_all_warning_msgs_e&                kill_all_warning_msgs();
    const fixed_bitstring<16, false, true>& msg_id() const;
    const fixed_bitstring<16, false, true>& serial_num() const;
    const warning_area_list_c&              warning_area_list() const;
    const kill_all_warning_msgs_e&          kill_all_warning_msgs() const;

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

// KillResponseIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct kill_resp_ies_o {
  // Value ::= OPEN TYPE
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>&       msg_id();
    fixed_bitstring<16, false, true>&       serial_num();
    broadcast_cancelled_area_list_c&        broadcast_cancelled_area_list();
    crit_diagnostics_s&                     crit_diagnostics();
    const fixed_bitstring<16, false, true>& msg_id() const;
    const fixed_bitstring<16, false, true>& serial_num() const;
    const broadcast_cancelled_area_list_c&  broadcast_cancelled_area_list() const;
    const crit_diagnostics_s&               crit_diagnostics() const;

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

// LocationReportIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct location_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, eutran_cgi, tai, request_type, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&             mme_ue_s1ap_id();
    uint32_t&             enb_ue_s1ap_id();
    eutran_cgi_s&         eutran_cgi();
    tai_s&                tai();
    request_type_s&       request_type();
    const uint64_t&       mme_ue_s1ap_id() const;
    const uint32_t&       enb_ue_s1ap_id() const;
    const eutran_cgi_s&   eutran_cgi() const;
    const tai_s&          tai() const;
    const request_type_s& request_type() const;

  private:
    types                                                type_;
    choice_buffer_t<eutran_cgi_s, request_type_s, tai_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LocationReportingControlIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct location_report_ctrl_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, request_type, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&             mme_ue_s1ap_id();
    uint32_t&             enb_ue_s1ap_id();
    request_type_s&       request_type();
    const uint64_t&       mme_ue_s1ap_id() const;
    const uint32_t&       enb_ue_s1ap_id() const;
    const request_type_s& request_type() const;

  private:
    types                           type_;
    choice_buffer_t<request_type_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LocationReportingFailureIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct location_report_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, cause, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&       mme_ue_s1ap_id();
    uint32_t&       enb_ue_s1ap_id();
    cause_c&        cause();
    const uint64_t& mme_ue_s1ap_id() const;
    const uint32_t& enb_ue_s1ap_id() const;
    const cause_c&  cause() const;

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

// MMECPRelocationIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct mmecp_relocation_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&       mme_ue_s1ap_id();
    uint32_t&       enb_ue_s1ap_id();
    const uint64_t& mme_ue_s1ap_id() const;
    const uint32_t& enb_ue_s1ap_id() const;

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

// MMEConfigurationTransferIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct mme_cfg_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { son_cfg_transfer_mct, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::son_cfg_transfer_mct; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    son_cfg_transfer_s&       son_cfg_transfer_mct() { return c; }
    const son_cfg_transfer_s& son_cfg_transfer_mct() const { return c; }

  private:
    son_cfg_transfer_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MMEConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct mme_cfg_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { crit_diagnostics, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::crit_diagnostics; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
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

// MMEConfigurationUpdateFailureIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct mme_cfg_upd_fail_ies_o {
  // Value ::= OPEN TYPE
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

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

// MMEConfigurationUpdateIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct mme_cfg_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mm_ename, served_gummeis, relative_mme_capacity, served_dcns, nulltype } value;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    printable_string<1, 150, true, true>&       mm_ename();
    served_gummeis_l&                           served_gummeis();
    uint16_t&                                   relative_mme_capacity();
    served_dcns_l&                              served_dcns();
    const printable_string<1, 150, true, true>& mm_ename() const;
    const served_gummeis_l&                     served_gummeis() const;
    const uint16_t&                             relative_mme_capacity() const;
    const served_dcns_l&                        served_dcns() const;

  private:
    types                                                                                  type_;
    choice_buffer_t<printable_string<1, 150, true, true>, served_dcns_l, served_gummeis_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MMEDirectInformationTransferIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct mme_direct_info_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { inter_sys_info_transfer_type_mdt, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::inter_sys_info_transfer_type_mdt; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    inter_sys_info_transfer_type_c&       inter_sys_info_transfer_type_mdt() { return c; }
    const inter_sys_info_transfer_type_c& inter_sys_info_transfer_type_mdt() const { return c; }

  private:
    inter_sys_info_transfer_type_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MMEStatusTransferIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct mme_status_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, enb_status_transfer_transparent_container, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                          mme_ue_s1ap_id();
    uint32_t&                                          enb_ue_s1ap_id();
    enb_status_transfer_transparent_container_s&       enb_status_transfer_transparent_container();
    const uint64_t&                                    mme_ue_s1ap_id() const;
    const uint32_t&                                    enb_ue_s1ap_id() const;
    const enb_status_transfer_transparent_container_s& enb_status_transfer_transparent_container() const;

  private:
    types                                                        type_;
    choice_buffer_t<enb_status_transfer_transparent_container_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NASDeliveryIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct nas_delivery_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&       mme_ue_s1ap_id();
    uint32_t&       enb_ue_s1ap_id();
    const uint64_t& mme_ue_s1ap_id() const;
    const uint32_t& enb_ue_s1ap_id() const;

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

// NASNonDeliveryIndication-IEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct nas_non_delivery_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, nas_pdu, cause, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        mme_ue_s1ap_id();
    uint32_t&                        enb_ue_s1ap_id();
    unbounded_octstring<true>&       nas_pdu();
    cause_c&                         cause();
    const uint64_t&                  mme_ue_s1ap_id() const;
    const uint32_t&                  enb_ue_s1ap_id() const;
    const unbounded_octstring<true>& nas_pdu() const;
    const cause_c&                   cause() const;

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

// OverloadStartIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct overload_start_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { overload_resp, gummei_list, traffic_load_reduction_ind, nulltype } value;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    overload_resp_c&       overload_resp();
    gummei_list_l&         gummei_list();
    uint8_t&               traffic_load_reduction_ind();
    const overload_resp_c& overload_resp() const;
    const gummei_list_l&   gummei_list() const;
    const uint8_t&         traffic_load_reduction_ind() const;

  private:
    types                                           type_;
    choice_buffer_t<gummei_list_l, overload_resp_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// OverloadStopIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct overload_stop_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gummei_list, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gummei_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gummei_list_l&       gummei_list() { return c; }
    const gummei_list_l& gummei_list() const { return c; }

  private:
    gummei_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSFailureIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct pws_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { pw_sfailed_ecgi_list, global_enb_id, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pw_sfailed_ecgi_list_l&       pw_sfailed_ecgi_list();
    global_enb_id_s&              global_enb_id();
    const pw_sfailed_ecgi_list_l& pw_sfailed_ecgi_list() const;
    const global_enb_id_s&        global_enb_id() const;

  private:
    types                                                    type_;
    choice_buffer_t<global_enb_id_s, pw_sfailed_ecgi_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSRestartIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct pws_restart_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ecgi_list_for_restart,
        global_enb_id,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ecgi_list_for_restart_l&                    ecgi_list_for_restart();
    global_enb_id_s&                            global_enb_id();
    tai_list_for_restart_l&                     tai_list_for_restart();
    emergency_area_id_list_for_restart_l&       emergency_area_id_list_for_restart();
    const ecgi_list_for_restart_l&              ecgi_list_for_restart() const;
    const global_enb_id_s&                      global_enb_id() const;
    const tai_list_for_restart_l&               tai_list_for_restart() const;
    const emergency_area_id_list_for_restart_l& emergency_area_id_list_for_restart() const;

  private:
    types type_;
    choice_buffer_t<ecgi_list_for_restart_l,
                    emergency_area_id_list_for_restart_l,
                    global_enb_id_s,
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

// PagingIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct paging_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ue_id_idx_value,
        ue_paging_id,
        paging_drx,
        cn_domain,
        tai_list,
        csg_id_list,
        paging_prio,
        ue_radio_cap_for_paging,
        assist_data_for_paging,
        paging_e_drx_info,
        extended_ue_id_idx_value,
        nb_io_t_paging_e_drx_info,
        nb_io_t_ue_id_idx_value,
        enhanced_coverage_restricted,
        ce_mode_brestricted,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<10, false, true>&       ue_id_idx_value();
    ue_paging_id_c&                         ue_paging_id();
    paging_drx_e&                           paging_drx();
    cn_domain_e&                            cn_domain();
    tai_list_l&                             tai_list();
    csg_id_list_l&                          csg_id_list();
    paging_prio_e&                          paging_prio();
    unbounded_octstring<true>&              ue_radio_cap_for_paging();
    assist_data_for_paging_s&               assist_data_for_paging();
    paging_e_drx_info_s&                    paging_e_drx_info();
    fixed_bitstring<14, false, true>&       extended_ue_id_idx_value();
    nb_io_t_paging_e_drx_info_s&            nb_io_t_paging_e_drx_info();
    fixed_bitstring<12, false, true>&       nb_io_t_ue_id_idx_value();
    enhanced_coverage_restricted_e&         enhanced_coverage_restricted();
    ce_mode_brestricted_e&                  ce_mode_brestricted();
    const fixed_bitstring<10, false, true>& ue_id_idx_value() const;
    const ue_paging_id_c&                   ue_paging_id() const;
    const paging_drx_e&                     paging_drx() const;
    const cn_domain_e&                      cn_domain() const;
    const tai_list_l&                       tai_list() const;
    const csg_id_list_l&                    csg_id_list() const;
    const paging_prio_e&                    paging_prio() const;
    const unbounded_octstring<true>&        ue_radio_cap_for_paging() const;
    const assist_data_for_paging_s&         assist_data_for_paging() const;
    const paging_e_drx_info_s&              paging_e_drx_info() const;
    const fixed_bitstring<14, false, true>& extended_ue_id_idx_value() const;
    const nb_io_t_paging_e_drx_info_s&      nb_io_t_paging_e_drx_info() const;
    const fixed_bitstring<12, false, true>& nb_io_t_ue_id_idx_value() const;
    const enhanced_coverage_restricted_e&   enhanced_coverage_restricted() const;
    const ce_mode_brestricted_e&            ce_mode_brestricted() const;

  private:
    types type_;
    choice_buffer_t<assist_data_for_paging_s,
                    csg_id_list_l,
                    fixed_bitstring<14, false, true>,
                    nb_io_t_paging_e_drx_info_s,
                    paging_e_drx_info_s,
                    tai_list_l,
                    ue_paging_id_c,
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

// PathSwitchRequestAcknowledgeIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct path_switch_request_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        ueaggregate_maximum_bitrate,
        erab_to_be_switched_ul_list,
        erab_to_be_released_list,
        security_context,
        crit_diagnostics,
        mme_ue_s1ap_id_minus2,
        csg_membership_status,
        pro_se_authorized,
        ueuser_plane_cio_tsupport_ind,
        v2xservices_authorized,
        ue_sidelink_aggregate_maximum_bitrate,
        enhanced_coverage_restricted,
        ce_mode_brestricted,
        pending_data_ind,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                                          mme_ue_s1ap_id();
    uint32_t&                                                          enb_ue_s1ap_id();
    ue_aggregate_maximum_bitrate_s&                                    ueaggregate_maximum_bitrate();
    erab_ie_container_list_l<erab_to_be_switched_ul_item_ies_o>&       erab_to_be_switched_ul_list();
    erab_list_l&                                                       erab_to_be_released_list();
    security_context_s&                                                security_context();
    crit_diagnostics_s&                                                crit_diagnostics();
    uint64_t&                                                          mme_ue_s1ap_id_minus2();
    csg_membership_status_e&                                           csg_membership_status();
    pro_se_authorized_s&                                               pro_se_authorized();
    ueuser_plane_cio_tsupport_ind_e&                                   ueuser_plane_cio_tsupport_ind();
    v2xservices_authorized_s&                                          v2xservices_authorized();
    ue_sidelink_aggregate_maximum_bitrate_s&                           ue_sidelink_aggregate_maximum_bitrate();
    enhanced_coverage_restricted_e&                                    enhanced_coverage_restricted();
    ce_mode_brestricted_e&                                             ce_mode_brestricted();
    pending_data_ind_e&                                                pending_data_ind();
    const uint64_t&                                                    mme_ue_s1ap_id() const;
    const uint32_t&                                                    enb_ue_s1ap_id() const;
    const ue_aggregate_maximum_bitrate_s&                              ueaggregate_maximum_bitrate() const;
    const erab_ie_container_list_l<erab_to_be_switched_ul_item_ies_o>& erab_to_be_switched_ul_list() const;
    const erab_list_l&                                                 erab_to_be_released_list() const;
    const security_context_s&                                          security_context() const;
    const crit_diagnostics_s&                                          crit_diagnostics() const;
    const uint64_t&                                                    mme_ue_s1ap_id_minus2() const;
    const csg_membership_status_e&                                     csg_membership_status() const;
    const pro_se_authorized_s&                                         pro_se_authorized() const;
    const ueuser_plane_cio_tsupport_ind_e&                             ueuser_plane_cio_tsupport_ind() const;
    const v2xservices_authorized_s&                                    v2xservices_authorized() const;
    const ue_sidelink_aggregate_maximum_bitrate_s&                     ue_sidelink_aggregate_maximum_bitrate() const;
    const enhanced_coverage_restricted_e&                              enhanced_coverage_restricted() const;
    const ce_mode_brestricted_e&                                       ce_mode_brestricted() const;
    const pending_data_ind_e&                                          pending_data_ind() const;

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    erab_ie_container_list_l<erab_to_be_switched_ul_item_ies_o>,
                    erab_list_l,
                    pro_se_authorized_s,
                    security_context_s,
                    ue_aggregate_maximum_bitrate_s,
                    ue_sidelink_aggregate_maximum_bitrate_s,
                    v2xservices_authorized_s>
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

// PathSwitchRequestFailureIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct path_switch_request_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, cause, crit_diagnostics, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 mme_ue_s1ap_id();
    uint32_t&                 enb_ue_s1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           mme_ue_s1ap_id() const;
    const uint32_t&           enb_ue_s1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

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

// PathSwitchRequestIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct path_switch_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        enb_ue_s1ap_id,
        erab_to_be_switched_dl_list,
        source_mme_ue_s1ap_id,
        eutran_cgi,
        tai,
        ue_security_cap,
        csg_id,
        cell_access_mode,
        source_mme_gummei,
        csg_membership_status,
        tunnel_info_for_bbf,
        lhn_id,
        rrc_resume_cause,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&                                                          enb_ue_s1ap_id();
    erab_ie_container_list_l<erab_to_be_switched_dl_item_ies_o>&       erab_to_be_switched_dl_list();
    uint64_t&                                                          source_mme_ue_s1ap_id();
    eutran_cgi_s&                                                      eutran_cgi();
    tai_s&                                                             tai();
    ue_security_cap_s&                                                 ue_security_cap();
    fixed_bitstring<27, false, true>&                                  csg_id();
    cell_access_mode_e&                                                cell_access_mode();
    gummei_s&                                                          source_mme_gummei();
    csg_membership_status_e&                                           csg_membership_status();
    tunnel_info_s&                                                     tunnel_info_for_bbf();
    unbounded_octstring<true>&                                         lhn_id();
    rrc_establishment_cause_e&                                         rrc_resume_cause();
    const uint32_t&                                                    enb_ue_s1ap_id() const;
    const erab_ie_container_list_l<erab_to_be_switched_dl_item_ies_o>& erab_to_be_switched_dl_list() const;
    const uint64_t&                                                    source_mme_ue_s1ap_id() const;
    const eutran_cgi_s&                                                eutran_cgi() const;
    const tai_s&                                                       tai() const;
    const ue_security_cap_s&                                           ue_security_cap() const;
    const fixed_bitstring<27, false, true>&                            csg_id() const;
    const cell_access_mode_e&                                          cell_access_mode() const;
    const gummei_s&                                                    source_mme_gummei() const;
    const csg_membership_status_e&                                     csg_membership_status() const;
    const tunnel_info_s&                                               tunnel_info_for_bbf() const;
    const unbounded_octstring<true>&                                   lhn_id() const;
    const rrc_establishment_cause_e&                                   rrc_resume_cause() const;

  private:
    types type_;
    choice_buffer_t<erab_ie_container_list_l<erab_to_be_switched_dl_item_ies_o>,
                    eutran_cgi_s,
                    fixed_bitstring<27, false, true>,
                    gummei_s,
                    tai_s,
                    tunnel_info_s,
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

struct s1ap_private_ies_empty_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
};
// PrivateMessageIEs ::= OBJECT SET OF S1AP-PRIVATE-IES
typedef s1ap_private_ies_empty_o private_msg_ies_o;

// RerouteNASRequest-IEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct reroute_nas_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { enb_ue_s1ap_id, mme_ue_s1ap_id, s1_msg, mme_group_id, add_guti, ue_usage_type, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&                        enb_ue_s1ap_id();
    uint64_t&                        mme_ue_s1ap_id();
    unbounded_octstring<true>&       s1_msg();
    fixed_octstring<2, true>&        mme_group_id();
    add_guti_s&                      add_guti();
    uint16_t&                        ue_usage_type();
    const uint32_t&                  enb_ue_s1ap_id() const;
    const uint64_t&                  mme_ue_s1ap_id() const;
    const unbounded_octstring<true>& s1_msg() const;
    const fixed_octstring<2, true>&  mme_group_id() const;
    const add_guti_s&                add_guti() const;
    const uint16_t&                  ue_usage_type() const;

  private:
    types                                                                             type_;
    choice_buffer_t<add_guti_s, fixed_octstring<2, true>, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResetAcknowledgeIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct reset_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_s1_conn_list_res_ack, crit_diagnostics, nulltype } value;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_associated_lc_s1_conn_list_res_ack_l&       ue_associated_lc_s1_conn_list_res_ack();
    crit_diagnostics_s&                            crit_diagnostics();
    const ue_associated_lc_s1_conn_list_res_ack_l& ue_associated_lc_s1_conn_list_res_ack() const;
    const crit_diagnostics_s&                      crit_diagnostics() const;

  private:
    types                                                                        type_;
    choice_buffer_t<crit_diagnostics_s, ue_associated_lc_s1_conn_list_res_ack_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResetIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct reset_ies_o {
  // Value ::= OPEN TYPE
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&            cause();
    reset_type_c&       reset_type();
    const cause_c&      cause() const;
    const reset_type_c& reset_type() const;

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

// RetrieveUEInformationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct retrieve_ue_info_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { s_tmsi, nulltype } value;

      std::string to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::s_tmsi; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    s_tmsi_s&       s_tmsi() { return c; }
    const s_tmsi_s& s_tmsi() const { return c; }

  private:
    s_tmsi_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// S1SetupFailureIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct s1_setup_fail_ies_o {
  // Value ::= OPEN TYPE
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

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

// S1SetupRequestIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct s1_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        global_enb_id,
        enbname,
        supported_tas,
        default_paging_drx,
        csg_id_list,
        ue_retention_info,
        nb_io_t_default_paging_drx,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    global_enb_id_s&                            global_enb_id();
    printable_string<1, 150, true, true>&       enbname();
    supported_tas_l&                            supported_tas();
    paging_drx_e&                               default_paging_drx();
    csg_id_list_l&                              csg_id_list();
    ue_retention_info_e&                        ue_retention_info();
    nb_io_t_default_paging_drx_e&               nb_io_t_default_paging_drx();
    const global_enb_id_s&                      global_enb_id() const;
    const printable_string<1, 150, true, true>& enbname() const;
    const supported_tas_l&                      supported_tas() const;
    const paging_drx_e&                         default_paging_drx() const;
    const csg_id_list_l&                        csg_id_list() const;
    const ue_retention_info_e&                  ue_retention_info() const;
    const nb_io_t_default_paging_drx_e&         nb_io_t_default_paging_drx() const;

  private:
    types                                                                                                  type_;
    choice_buffer_t<csg_id_list_l, global_enb_id_s, printable_string<1, 150, true, true>, supported_tas_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// S1SetupResponseIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct s1_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mm_ename,
        served_gummeis,
        relative_mme_capacity,
        mme_relay_support_ind,
        crit_diagnostics,
        ue_retention_info,
        served_dcns,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    printable_string<1, 150, true, true>&       mm_ename();
    served_gummeis_l&                           served_gummeis();
    uint16_t&                                   relative_mme_capacity();
    mme_relay_support_ind_e&                    mme_relay_support_ind();
    crit_diagnostics_s&                         crit_diagnostics();
    ue_retention_info_e&                        ue_retention_info();
    served_dcns_l&                              served_dcns();
    const printable_string<1, 150, true, true>& mm_ename() const;
    const served_gummeis_l&                     served_gummeis() const;
    const uint16_t&                             relative_mme_capacity() const;
    const mme_relay_support_ind_e&              mme_relay_support_ind() const;
    const crit_diagnostics_s&                   crit_diagnostics() const;
    const ue_retention_info_e&                  ue_retention_info() const;
    const served_dcns_l&                        served_dcns() const;

  private:
    types                                                                                                      type_;
    choice_buffer_t<crit_diagnostics_s, printable_string<1, 150, true, true>, served_dcns_l, served_gummeis_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TraceFailureIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct trace_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, e_utran_trace_id, cause, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                       mme_ue_s1ap_id();
    uint32_t&                       enb_ue_s1ap_id();
    fixed_octstring<8, true>&       e_utran_trace_id();
    cause_c&                        cause();
    const uint64_t&                 mme_ue_s1ap_id() const;
    const uint32_t&                 enb_ue_s1ap_id() const;
    const fixed_octstring<8, true>& e_utran_trace_id() const;
    const cause_c&                  cause() const;

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

// TraceStartIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct trace_start_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, trace_activation, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 mme_ue_s1ap_id();
    uint32_t&                 enb_ue_s1ap_id();
    trace_activation_s&       trace_activation();
    const uint64_t&           mme_ue_s1ap_id() const;
    const uint32_t&           enb_ue_s1ap_id() const;
    const trace_activation_s& trace_activation() const;

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

// UECapabilityInfoIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_cap_info_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, ue_radio_cap, ue_radio_cap_for_paging, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        mme_ue_s1ap_id();
    uint32_t&                        enb_ue_s1ap_id();
    unbounded_octstring<true>&       ue_radio_cap();
    unbounded_octstring<true>&       ue_radio_cap_for_paging();
    const uint64_t&                  mme_ue_s1ap_id() const;
    const uint32_t&                  enb_ue_s1ap_id() const;
    const unbounded_octstring<true>& ue_radio_cap() const;
    const unbounded_octstring<true>& ue_radio_cap_for_paging() const;

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

// UEContextModificationConfirmIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_mod_confirm_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, csg_membership_status, crit_diagnostics, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                      mme_ue_s1ap_id();
    uint32_t&                      enb_ue_s1ap_id();
    csg_membership_status_e&       csg_membership_status();
    crit_diagnostics_s&            crit_diagnostics();
    const uint64_t&                mme_ue_s1ap_id() const;
    const uint32_t&                enb_ue_s1ap_id() const;
    const csg_membership_status_e& csg_membership_status() const;
    const crit_diagnostics_s&      crit_diagnostics() const;

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

// UEContextModificationFailureIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_mod_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, cause, crit_diagnostics, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 mme_ue_s1ap_id();
    uint32_t&                 enb_ue_s1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           mme_ue_s1ap_id() const;
    const uint32_t&           enb_ue_s1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

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

// UEContextModificationIndicationIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_mod_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, csg_membership_info, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                    mme_ue_s1ap_id();
    uint32_t&                    enb_ue_s1ap_id();
    csg_membership_info_s&       csg_membership_info();
    const uint64_t&              mme_ue_s1ap_id() const;
    const uint32_t&              enb_ue_s1ap_id() const;
    const csg_membership_info_s& csg_membership_info() const;

  private:
    types                                  type_;
    choice_buffer_t<csg_membership_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextModificationRequestIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_mod_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        security_key,
        subscriber_profile_idfor_rfp,
        ueaggregate_maximum_bitrate,
        cs_fallback_ind,
        ue_security_cap,
        csg_membership_status,
        registered_lai,
        add_cs_fallback_ind,
        pro_se_authorized,
        srvcc_operation_possible,
        srvcc_operation_not_possible,
        v2xservices_authorized,
        ue_sidelink_aggregate_maximum_bitrate,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                      mme_ue_s1ap_id();
    uint32_t&                                      enb_ue_s1ap_id();
    fixed_bitstring<256, false, true>&             security_key();
    uint16_t&                                      subscriber_profile_idfor_rfp();
    ue_aggregate_maximum_bitrate_s&                ueaggregate_maximum_bitrate();
    cs_fallback_ind_e&                             cs_fallback_ind();
    ue_security_cap_s&                             ue_security_cap();
    csg_membership_status_e&                       csg_membership_status();
    lai_s&                                         registered_lai();
    add_cs_fallback_ind_e&                         add_cs_fallback_ind();
    pro_se_authorized_s&                           pro_se_authorized();
    srvcc_operation_possible_e&                    srvcc_operation_possible();
    srvcc_operation_not_possible_e&                srvcc_operation_not_possible();
    v2xservices_authorized_s&                      v2xservices_authorized();
    ue_sidelink_aggregate_maximum_bitrate_s&       ue_sidelink_aggregate_maximum_bitrate();
    const uint64_t&                                mme_ue_s1ap_id() const;
    const uint32_t&                                enb_ue_s1ap_id() const;
    const fixed_bitstring<256, false, true>&       security_key() const;
    const uint16_t&                                subscriber_profile_idfor_rfp() const;
    const ue_aggregate_maximum_bitrate_s&          ueaggregate_maximum_bitrate() const;
    const cs_fallback_ind_e&                       cs_fallback_ind() const;
    const ue_security_cap_s&                       ue_security_cap() const;
    const csg_membership_status_e&                 csg_membership_status() const;
    const lai_s&                                   registered_lai() const;
    const add_cs_fallback_ind_e&                   add_cs_fallback_ind() const;
    const pro_se_authorized_s&                     pro_se_authorized() const;
    const srvcc_operation_possible_e&              srvcc_operation_possible() const;
    const srvcc_operation_not_possible_e&          srvcc_operation_not_possible() const;
    const v2xservices_authorized_s&                v2xservices_authorized() const;
    const ue_sidelink_aggregate_maximum_bitrate_s& ue_sidelink_aggregate_maximum_bitrate() const;

  private:
    types type_;
    choice_buffer_t<fixed_bitstring<256, false, true>,
                    lai_s,
                    pro_se_authorized_s,
                    ue_aggregate_maximum_bitrate_s,
                    ue_security_cap_s,
                    ue_sidelink_aggregate_maximum_bitrate_s,
                    v2xservices_authorized_s>
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

// UEContextModificationResponseIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_mod_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, crit_diagnostics, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 mme_ue_s1ap_id();
    uint32_t&                 enb_ue_s1ap_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           mme_ue_s1ap_id() const;
    const uint32_t&           enb_ue_s1ap_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

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

// UEContextReleaseCommand-IEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_release_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_s1ap_ids, cause, nulltype } value;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_s1ap_ids_c&       ue_s1ap_ids();
    cause_c&             cause();
    const ue_s1ap_ids_c& ue_s1ap_ids() const;
    const cause_c&       cause() const;

  private:
    types                                   type_;
    choice_buffer_t<cause_c, ue_s1ap_ids_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextReleaseComplete-IEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_release_complete_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        crit_diagnostics,
        user_location_info,
        info_on_recommended_cells_and_enbs_for_paging,
        cell_id_and_ce_level_for_ce_capable_ues,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                              mme_ue_s1ap_id();
    uint32_t&                                              enb_ue_s1ap_id();
    crit_diagnostics_s&                                    crit_diagnostics();
    user_location_info_s&                                  user_location_info();
    info_on_recommended_cells_and_enbs_for_paging_s&       info_on_recommended_cells_and_enbs_for_paging();
    cell_id_and_ce_level_for_ce_capable_ues_s&             cell_id_and_ce_level_for_ce_capable_ues();
    const uint64_t&                                        mme_ue_s1ap_id() const;
    const uint32_t&                                        enb_ue_s1ap_id() const;
    const crit_diagnostics_s&                              crit_diagnostics() const;
    const user_location_info_s&                            user_location_info() const;
    const info_on_recommended_cells_and_enbs_for_paging_s& info_on_recommended_cells_and_enbs_for_paging() const;
    const cell_id_and_ce_level_for_ce_capable_ues_s&       cell_id_and_ce_level_for_ce_capable_ues() const;

  private:
    types type_;
    choice_buffer_t<cell_id_and_ce_level_for_ce_capable_ues_s,
                    crit_diagnostics_s,
                    info_on_recommended_cells_and_enbs_for_paging_s,
                    user_location_info_s>
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

// UEContextReleaseRequest-IEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_release_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, cause, gw_context_release_ind, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                       mme_ue_s1ap_id();
    uint32_t&                       enb_ue_s1ap_id();
    cause_c&                        cause();
    gw_context_release_ind_e&       gw_context_release_ind();
    const uint64_t&                 mme_ue_s1ap_id() const;
    const uint32_t&                 enb_ue_s1ap_id() const;
    const cause_c&                  cause() const;
    const gw_context_release_ind_e& gw_context_release_ind() const;

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

// UEContextResumeFailureIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_resume_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, cause, crit_diagnostics, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 mme_ue_s1ap_id();
    uint32_t&                 enb_ue_s1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           mme_ue_s1ap_id() const;
    const uint32_t&           enb_ue_s1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

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

// UEContextResumeRequestIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_resume_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        erab_failed_to_resume_list_resume_req,
        rrc_resume_cause,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                                              mme_ue_s1ap_id();
    uint32_t&                                                              enb_ue_s1ap_id();
    erab_ie_container_list_l<erab_failed_to_resume_item_resume_req_ies_o>& erab_failed_to_resume_list_resume_req();
    rrc_establishment_cause_e&                                             rrc_resume_cause();
    const uint64_t&                                                        mme_ue_s1ap_id() const;
    const uint32_t&                                                        enb_ue_s1ap_id() const;
    const erab_ie_container_list_l<erab_failed_to_resume_item_resume_req_ies_o>&
                                     erab_failed_to_resume_list_resume_req() const;
    const rrc_establishment_cause_e& rrc_resume_cause() const;

  private:
    types                                                                                   type_;
    choice_buffer_t<erab_ie_container_list_l<erab_failed_to_resume_item_resume_req_ies_o> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextResumeResponseIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_resume_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        erab_failed_to_resume_list_resume_res,
        crit_diagnostics,
        security_context,
        pending_data_ind,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                                              mme_ue_s1ap_id();
    uint32_t&                                                              enb_ue_s1ap_id();
    erab_ie_container_list_l<erab_failed_to_resume_item_resume_res_ies_o>& erab_failed_to_resume_list_resume_res();
    crit_diagnostics_s&                                                    crit_diagnostics();
    security_context_s&                                                    security_context();
    pending_data_ind_e&                                                    pending_data_ind();
    const uint64_t&                                                        mme_ue_s1ap_id() const;
    const uint32_t&                                                        enb_ue_s1ap_id() const;
    const erab_ie_container_list_l<erab_failed_to_resume_item_resume_res_ies_o>&
                              erab_failed_to_resume_list_resume_res() const;
    const crit_diagnostics_s& crit_diagnostics() const;
    const security_context_s& security_context() const;
    const pending_data_ind_e& pending_data_ind() const;

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    erab_ie_container_list_l<erab_failed_to_resume_item_resume_res_ies_o>,
                    security_context_s>
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

// UEContextSuspendRequestIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_suspend_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        info_on_recommended_cells_and_enbs_for_paging,
        cell_id_and_ce_level_for_ce_capable_ues,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                              mme_ue_s1ap_id();
    uint32_t&                                              enb_ue_s1ap_id();
    info_on_recommended_cells_and_enbs_for_paging_s&       info_on_recommended_cells_and_enbs_for_paging();
    cell_id_and_ce_level_for_ce_capable_ues_s&             cell_id_and_ce_level_for_ce_capable_ues();
    const uint64_t&                                        mme_ue_s1ap_id() const;
    const uint32_t&                                        enb_ue_s1ap_id() const;
    const info_on_recommended_cells_and_enbs_for_paging_s& info_on_recommended_cells_and_enbs_for_paging() const;
    const cell_id_and_ce_level_for_ce_capable_ues_s&       cell_id_and_ce_level_for_ce_capable_ues() const;

  private:
    types                                                                                                       type_;
    choice_buffer_t<cell_id_and_ce_level_for_ce_capable_ues_s, info_on_recommended_cells_and_enbs_for_paging_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextSuspendResponseIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_context_suspend_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, crit_diagnostics, security_context, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 mme_ue_s1ap_id();
    uint32_t&                 enb_ue_s1ap_id();
    crit_diagnostics_s&       crit_diagnostics();
    security_context_s&       security_context();
    const uint64_t&           mme_ue_s1ap_id() const;
    const uint32_t&           enb_ue_s1ap_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;
    const security_context_s& security_context() const;

  private:
    types                                                   type_;
    choice_buffer_t<crit_diagnostics_s, security_context_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEInformationTransferIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_info_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { s_tmsi, ue_level_qos_params, ue_radio_cap, pending_data_ind, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    s_tmsi_s&                        s_tmsi();
    erab_level_qos_params_s&         ue_level_qos_params();
    unbounded_octstring<true>&       ue_radio_cap();
    pending_data_ind_e&              pending_data_ind();
    const s_tmsi_s&                  s_tmsi() const;
    const erab_level_qos_params_s&   ue_level_qos_params() const;
    const unbounded_octstring<true>& ue_radio_cap() const;
    const pending_data_ind_e&        pending_data_ind() const;

  private:
    types                                                                          type_;
    choice_buffer_t<erab_level_qos_params_s, s_tmsi_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UERadioCapabilityMatchRequestIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_radio_cap_match_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, ue_radio_cap, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        mme_ue_s1ap_id();
    uint32_t&                        enb_ue_s1ap_id();
    unbounded_octstring<true>&       ue_radio_cap();
    const uint64_t&                  mme_ue_s1ap_id() const;
    const uint32_t&                  enb_ue_s1ap_id() const;
    const unbounded_octstring<true>& ue_radio_cap() const;

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

// UERadioCapabilityMatchResponseIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ue_radio_cap_match_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, voice_support_match_ind, crit_diagnostics, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        mme_ue_s1ap_id();
    uint32_t&                        enb_ue_s1ap_id();
    voice_support_match_ind_e&       voice_support_match_ind();
    crit_diagnostics_s&              crit_diagnostics();
    const uint64_t&                  mme_ue_s1ap_id() const;
    const uint32_t&                  enb_ue_s1ap_id() const;
    const voice_support_match_ind_e& voice_support_match_ind() const;
    const crit_diagnostics_s&        crit_diagnostics() const;

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

// UplinkNASTransport-IEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ul_nas_transport_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        nas_pdu,
        eutran_cgi,
        tai,
        gw_transport_layer_address,
        sipto_l_gw_transport_layer_address,
        lhn_id,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                    mme_ue_s1ap_id();
    uint32_t&                                    enb_ue_s1ap_id();
    unbounded_octstring<true>&                   nas_pdu();
    eutran_cgi_s&                                eutran_cgi();
    tai_s&                                       tai();
    bounded_bitstring<1, 160, true, true>&       gw_transport_layer_address();
    bounded_bitstring<1, 160, true, true>&       sipto_l_gw_transport_layer_address();
    unbounded_octstring<true>&                   lhn_id();
    const uint64_t&                              mme_ue_s1ap_id() const;
    const uint32_t&                              enb_ue_s1ap_id() const;
    const unbounded_octstring<true>&             nas_pdu() const;
    const eutran_cgi_s&                          eutran_cgi() const;
    const tai_s&                                 tai() const;
    const bounded_bitstring<1, 160, true, true>& gw_transport_layer_address() const;
    const bounded_bitstring<1, 160, true, true>& sipto_l_gw_transport_layer_address() const;
    const unbounded_octstring<true>&             lhn_id() const;

  private:
    types                                                                                                   type_;
    choice_buffer_t<bounded_bitstring<1, 160, true, true>, eutran_cgi_s, tai_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkNonUEAssociatedLPPaTransport-IEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ul_non_ueassociated_lp_pa_transport_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { routing_id, lp_pa_pdu, nulltype } value;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                        routing_id();
    unbounded_octstring<true>&       lp_pa_pdu();
    const uint16_t&                  routing_id() const;
    const unbounded_octstring<true>& lp_pa_pdu() const;

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

// UplinkS1cdma2000tunnellingIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ul_s1cdma2000tunnelling_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mme_ue_s1ap_id,
        enb_ue_s1ap_id,
        cdma2000_rat_type,
        cdma2000_sector_id,
        cdma2000_ho_required_ind,
        cdma2000_one_xsrvcc_info,
        cdma2000_one_xrand,
        cdma2000_pdu,
        eutran_round_trip_delay_estimation_info,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                         mme_ue_s1ap_id();
    uint32_t&                         enb_ue_s1ap_id();
    cdma2000_rat_type_e&              cdma2000_rat_type();
    unbounded_octstring<true>&        cdma2000_sector_id();
    cdma2000_ho_required_ind_e&       cdma2000_ho_required_ind();
    cdma2000_one_xsrvcc_info_s&       cdma2000_one_xsrvcc_info();
    unbounded_octstring<true>&        cdma2000_one_xrand();
    unbounded_octstring<true>&        cdma2000_pdu();
    uint16_t&                         eutran_round_trip_delay_estimation_info();
    const uint64_t&                   mme_ue_s1ap_id() const;
    const uint32_t&                   enb_ue_s1ap_id() const;
    const cdma2000_rat_type_e&        cdma2000_rat_type() const;
    const unbounded_octstring<true>&  cdma2000_sector_id() const;
    const cdma2000_ho_required_ind_e& cdma2000_ho_required_ind() const;
    const cdma2000_one_xsrvcc_info_s& cdma2000_one_xsrvcc_info() const;
    const unbounded_octstring<true>&  cdma2000_one_xrand() const;
    const unbounded_octstring<true>&  cdma2000_pdu() const;
    const uint16_t&                   eutran_round_trip_delay_estimation_info() const;

  private:
    types                                                                   type_;
    choice_buffer_t<cdma2000_one_xsrvcc_info_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkUEAssociatedLPPaTransport-IEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct ul_ueassociated_lp_pa_transport_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mme_ue_s1ap_id, enb_ue_s1ap_id, routing_id, lp_pa_pdu, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        mme_ue_s1ap_id();
    uint32_t&                        enb_ue_s1ap_id();
    uint16_t&                        routing_id();
    unbounded_octstring<true>&       lp_pa_pdu();
    const uint64_t&                  mme_ue_s1ap_id() const;
    const uint32_t&                  enb_ue_s1ap_id() const;
    const uint16_t&                  routing_id() const;
    const unbounded_octstring<true>& lp_pa_pdu() const;

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

// WriteReplaceWarningRequestIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct write_replace_warning_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        msg_id,
        serial_num,
        warning_area_list,
        repeat_period,
        extended_repeat_period,
        numof_broadcast_request,
        warning_type,
        warning_security_info,
        data_coding_scheme,
        warning_msg_contents,
        concurrent_warning_msg_ind,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>&       msg_id();
    fixed_bitstring<16, false, true>&       serial_num();
    warning_area_list_c&                    warning_area_list();
    uint16_t&                               repeat_period();
    uint32_t&                               extended_repeat_period();
    uint32_t&                               numof_broadcast_request();
    fixed_octstring<2, true>&               warning_type();
    fixed_octstring<50, true>&              warning_security_info();
    fixed_bitstring<8, false, true>&        data_coding_scheme();
    unbounded_octstring<true>&              warning_msg_contents();
    concurrent_warning_msg_ind_e&           concurrent_warning_msg_ind();
    const fixed_bitstring<16, false, true>& msg_id() const;
    const fixed_bitstring<16, false, true>& serial_num() const;
    const warning_area_list_c&              warning_area_list() const;
    const uint16_t&                         repeat_period() const;
    const uint32_t&                         extended_repeat_period() const;
    const uint32_t&                         numof_broadcast_request() const;
    const fixed_octstring<2, true>&         warning_type() const;
    const fixed_octstring<50, true>&        warning_security_info() const;
    const fixed_bitstring<8, false, true>&  data_coding_scheme() const;
    const unbounded_octstring<true>&        warning_msg_contents() const;
    const concurrent_warning_msg_ind_e&     concurrent_warning_msg_ind() const;

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

// WriteReplaceWarningResponseIEs ::= OBJECT SET OF S1AP-PROTOCOL-IES
struct write_replace_warning_resp_ies_o {
  // Value ::= OPEN TYPE
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>&       msg_id();
    fixed_bitstring<16, false, true>&       serial_num();
    broadcast_completed_area_list_c&        broadcast_completed_area_list();
    crit_diagnostics_s&                     crit_diagnostics();
    const fixed_bitstring<16, false, true>& msg_id() const;
    const fixed_bitstring<16, false, true>& serial_num() const;
    const broadcast_completed_area_list_c&  broadcast_completed_area_list() const;
    const crit_diagnostics_s&               crit_diagnostics() const;

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

struct kill_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                          warning_area_list_present     = false;
  bool                                          kill_all_warning_msgs_present = false;
  ie_field_s<fixed_bitstring<16, false, true> > msg_id;
  ie_field_s<fixed_bitstring<16, false, true> > serial_num;
  ie_field_s<warning_area_list_c>               warning_area_list;
  ie_field_s<kill_all_warning_msgs_e>           kill_all_warning_msgs;

  // sequence methods
  kill_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// KillRequest ::= SEQUENCE
struct kill_request_s {
  bool                       ext = false;
  kill_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct kill_resp_ies_container {
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
  kill_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// KillResponse ::= SEQUENCE
struct kill_resp_s {
  bool                    ext = false;
  kill_resp_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct location_report_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<eutran_cgi_s>                                   eutran_cgi;
  ie_field_s<tai_s>                                          tai;
  ie_field_s<request_type_s>                                 request_type;

  // sequence methods
  location_report_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationReport ::= SEQUENCE
struct location_report_s {
  bool                          ext = false;
  location_report_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct location_report_ctrl_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<request_type_s>                                 request_type;

  // sequence methods
  location_report_ctrl_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationReportingControl ::= SEQUENCE
struct location_report_ctrl_s {
  bool                               ext = false;
  location_report_ctrl_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct location_report_fail_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<cause_c>                                        cause;

  // sequence methods
  location_report_fail_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationReportingFailureIndication ::= SEQUENCE
struct location_report_fail_ind_s {
  bool                                   ext = false;
  location_report_fail_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct mmecp_relocation_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;

  // sequence methods
  mmecp_relocation_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MMECPRelocationIndication ::= SEQUENCE
struct mmecp_relocation_ind_s {
  bool                               ext = false;
  mmecp_relocation_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MMEConfigurationTransfer ::= SEQUENCE
struct mme_cfg_transfer_s {
  bool                                            ext = false;
  protocol_ie_container_l<mme_cfg_transfer_ies_o> protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct mme_cfg_upd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       mm_ename_present              = false;
  bool                                                       served_gummeis_present        = false;
  bool                                                       relative_mme_capacity_present = false;
  bool                                                       served_dcns_present           = false;
  ie_field_s<printable_string<1, 150, true, true> >          mm_ename;
  ie_field_s<dyn_seq_of<served_gummeis_item_s, 1, 8, true> > served_gummeis;
  ie_field_s<integer<uint16_t, 0, 255, false, true> >        relative_mme_capacity;
  ie_field_s<dyn_seq_of<served_dcns_item_s, 0, 32, true> >   served_dcns;

  // sequence methods
  mme_cfg_upd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MMEConfigurationUpdate ::= SEQUENCE
struct mme_cfg_upd_s {
  bool                      ext = false;
  mme_cfg_upd_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MMEConfigurationUpdateAcknowledge ::= SEQUENCE
struct mme_cfg_upd_ack_s {
  bool                                           ext = false;
  protocol_ie_container_l<mme_cfg_upd_ack_ies_o> protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct mme_cfg_upd_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                           time_to_wait_present     = false;
  bool                           crit_diagnostics_present = false;
  ie_field_s<cause_c>            cause;
  ie_field_s<time_to_wait_e>     time_to_wait;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  mme_cfg_upd_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MMEConfigurationUpdateFailure ::= SEQUENCE
struct mme_cfg_upd_fail_s {
  bool                           ext = false;
  mme_cfg_upd_fail_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MMEDirectInformationTransfer ::= SEQUENCE
struct mme_direct_info_transfer_s {
  bool                                                    ext = false;
  protocol_ie_container_l<mme_direct_info_transfer_ies_o> protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct mme_status_transfer_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<enb_status_transfer_transparent_container_s>    enb_status_transfer_transparent_container;

  // sequence methods
  mme_status_transfer_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MMEStatusTransfer ::= SEQUENCE
struct mme_status_transfer_s {
  bool                              ext = false;
  mme_status_transfer_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct nas_delivery_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;

  // sequence methods
  nas_delivery_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NASDeliveryIndication ::= SEQUENCE
struct nas_delivery_ind_s {
  bool                           ext = false;
  nas_delivery_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct nas_non_delivery_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<unbounded_octstring<true> >                     nas_pdu;
  ie_field_s<cause_c>                                        cause;

  // sequence methods
  nas_non_delivery_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NASNonDeliveryIndication ::= SEQUENCE
struct nas_non_delivery_ind_s {
  bool                               ext = false;
  nas_non_delivery_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct overload_start_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              gummei_list_present                = false;
  bool                                              traffic_load_reduction_ind_present = false;
  ie_field_s<overload_resp_c>                       overload_resp;
  ie_field_s<dyn_seq_of<gummei_s, 1, 256, true> >   gummei_list;
  ie_field_s<integer<uint8_t, 1, 99, false, true> > traffic_load_reduction_ind;

  // sequence methods
  overload_start_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OverloadStart ::= SEQUENCE
struct overload_start_s {
  bool                         ext = false;
  overload_start_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OverloadStop ::= SEQUENCE
struct overload_stop_s {
  bool                                         ext = false;
  protocol_ie_container_l<overload_stop_ies_o> protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pws_fail_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<dyn_seq_of<eutran_cgi_s, 1, 256, true> > pw_sfailed_ecgi_list;
  ie_field_s<global_enb_id_s>                         global_enb_id;

  // sequence methods
  pws_fail_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSFailureIndication ::= SEQUENCE
struct pws_fail_ind_s {
  bool                       ext = false;
  pws_fail_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pws_restart_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                            emergency_area_id_list_for_restart_present = false;
  ie_field_s<dyn_seq_of<eutran_cgi_s, 1, 256, true> >             ecgi_list_for_restart;
  ie_field_s<global_enb_id_s>                                     global_enb_id;
  ie_field_s<dyn_seq_of<tai_s, 1, 2048, true> >                   tai_list_for_restart;
  ie_field_s<dyn_seq_of<fixed_octstring<3, true>, 1, 256, true> > emergency_area_id_list_for_restart;

  // sequence methods
  pws_restart_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSRestartIndication ::= SEQUENCE
struct pws_restart_ind_s {
  bool                          ext = false;
  pws_restart_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct paging_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                          paging_drx_present                   = false;
  bool                                          csg_id_list_present                  = false;
  bool                                          paging_prio_present                  = false;
  bool                                          ue_radio_cap_for_paging_present      = false;
  bool                                          assist_data_for_paging_present       = false;
  bool                                          paging_e_drx_info_present            = false;
  bool                                          extended_ue_id_idx_value_present     = false;
  bool                                          nb_io_t_paging_e_drx_info_present    = false;
  bool                                          nb_io_t_ue_id_idx_value_present      = false;
  bool                                          enhanced_coverage_restricted_present = false;
  bool                                          ce_mode_brestricted_present          = false;
  ie_field_s<fixed_bitstring<10, false, true> > ue_id_idx_value;
  ie_field_s<ue_paging_id_c>                    ue_paging_id;
  ie_field_s<paging_drx_e>                      paging_drx;
  ie_field_s<cn_domain_e>                       cn_domain;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<tai_item_ies_o>, 1, 256, true> > tai_list;
  ie_field_s<dyn_seq_of<csg_id_list_item_s, 1, 256, true> >                             csg_id_list;
  ie_field_s<paging_prio_e>                                                             paging_prio;
  ie_field_s<unbounded_octstring<true> >                                                ue_radio_cap_for_paging;
  ie_field_s<assist_data_for_paging_s>                                                  assist_data_for_paging;
  ie_field_s<paging_e_drx_info_s>                                                       paging_e_drx_info;
  ie_field_s<fixed_bitstring<14, false, true> >                                         extended_ue_id_idx_value;
  ie_field_s<nb_io_t_paging_e_drx_info_s>                                               nb_io_t_paging_e_drx_info;
  ie_field_s<fixed_bitstring<12, false, true> >                                         nb_io_t_ue_id_idx_value;
  ie_field_s<enhanced_coverage_restricted_e>                                            enhanced_coverage_restricted;
  ie_field_s<ce_mode_brestricted_e>                                                     ce_mode_brestricted;

  // sequence methods
  paging_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Paging ::= SEQUENCE
struct paging_s {
  bool                 ext = false;
  paging_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct path_switch_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                                     csg_id_present                = false;
  bool                                                                     cell_access_mode_present      = false;
  bool                                                                     source_mme_gummei_present     = false;
  bool                                                                     csg_membership_status_present = false;
  bool                                                                     tunnel_info_for_bbf_present   = false;
  bool                                                                     lhn_id_present                = false;
  bool                                                                     rrc_resume_cause_present      = false;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >                 enb_ue_s1ap_id;
  ie_field_s<erab_ie_container_list_l<erab_to_be_switched_dl_item_ies_o> > erab_to_be_switched_dl_list;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >               source_mme_ue_s1ap_id;
  ie_field_s<eutran_cgi_s>                                                 eutran_cgi;
  ie_field_s<tai_s>                                                        tai;
  ie_field_s<ue_security_cap_s>                                            ue_security_cap;
  ie_field_s<fixed_bitstring<27, false, true> >                            csg_id;
  ie_field_s<cell_access_mode_e>                                           cell_access_mode;
  ie_field_s<gummei_s>                                                     source_mme_gummei;
  ie_field_s<csg_membership_status_e>                                      csg_membership_status;
  ie_field_s<tunnel_info_s>                                                tunnel_info_for_bbf;
  ie_field_s<unbounded_octstring<true> >                                   lhn_id;
  ie_field_s<rrc_establishment_cause_e>                                    rrc_resume_cause;

  // sequence methods
  path_switch_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequest ::= SEQUENCE
struct path_switch_request_s {
  bool                              ext = false;
  path_switch_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct path_switch_request_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       ueaggregate_maximum_bitrate_present           = false;
  bool                                                       erab_to_be_switched_ul_list_present           = false;
  bool                                                       erab_to_be_released_list_present              = false;
  bool                                                       crit_diagnostics_present                      = false;
  bool                                                       mme_ue_s1ap_id_minus2_present                 = false;
  bool                                                       csg_membership_status_present                 = false;
  bool                                                       pro_se_authorized_present                     = false;
  bool                                                       ueuser_plane_cio_tsupport_ind_present         = false;
  bool                                                       v2xservices_authorized_present                = false;
  bool                                                       ue_sidelink_aggregate_maximum_bitrate_present = false;
  bool                                                       enhanced_coverage_restricted_present          = false;
  bool                                                       ce_mode_brestricted_present                   = false;
  bool                                                       pending_data_ind_present                      = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<ue_aggregate_maximum_bitrate_s>                 ueaggregate_maximum_bitrate;
  ie_field_s<erab_ie_container_list_l<erab_to_be_switched_ul_item_ies_o> >               erab_to_be_switched_ul_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<erab_item_ies_o>, 1, 256, true> > erab_to_be_released_list;
  ie_field_s<security_context_s>                                                         security_context;
  ie_field_s<crit_diagnostics_s>                                                         crit_diagnostics;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >                             mme_ue_s1ap_id_minus2;
  ie_field_s<csg_membership_status_e>                                                    csg_membership_status;
  ie_field_s<pro_se_authorized_s>                                                        pro_se_authorized;
  ie_field_s<ueuser_plane_cio_tsupport_ind_e>                                            ueuser_plane_cio_tsupport_ind;
  ie_field_s<v2xservices_authorized_s>                                                   v2xservices_authorized;
  ie_field_s<ue_sidelink_aggregate_maximum_bitrate_s> ue_sidelink_aggregate_maximum_bitrate;
  ie_field_s<enhanced_coverage_restricted_e>          enhanced_coverage_restricted;
  ie_field_s<ce_mode_brestricted_e>                   ce_mode_brestricted;
  ie_field_s<pending_data_ind_e>                      pending_data_ind;

  // sequence methods
  path_switch_request_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestAcknowledge ::= SEQUENCE
struct path_switch_request_ack_s {
  bool                                  ext = false;
  path_switch_request_ack_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct path_switch_request_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<cause_c>                                        cause;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;

  // sequence methods
  path_switch_request_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestFailure ::= SEQUENCE
struct path_switch_request_fail_s {
  bool                                   ext = false;
  path_switch_request_fail_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
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
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct private_ie_container_empty_l {
  template <class valueT_>
  using ie_field_s = private_ie_container_item_s<valueT_>;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};
typedef private_ie_container_empty_l private_msg_ies_container;

// PrivateMessage ::= SEQUENCE
struct private_msg_s {
  bool                      ext = false;
  private_msg_ies_container private_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct reroute_nas_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       mme_ue_s1ap_id_present = false;
  bool                                                       add_guti_present       = false;
  bool                                                       ue_usage_type_present  = false;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<unbounded_octstring<true> >                     s1_msg;
  ie_field_s<fixed_octstring<2, true> >                      mme_group_id;
  ie_field_s<add_guti_s>                                     add_guti;
  ie_field_s<integer<uint16_t, 0, 255, false, true> >        ue_usage_type;

  // sequence methods
  reroute_nas_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RerouteNASRequest ::= SEQUENCE
struct reroute_nas_request_s {
  bool                              ext = false;
  reroute_nas_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct reset_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<cause_c>      cause;
  ie_field_s<reset_type_c> reset_type;

  // sequence methods
  reset_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Reset ::= SEQUENCE
struct reset_s {
  bool                ext = false;
  reset_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct reset_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool ue_associated_lc_s1_conn_list_res_ack_present = false;
  bool crit_diagnostics_present                      = false;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ue_associated_lc_s1_conn_item_res_ack_o>, 1, 256, true> >
                                 ue_associated_lc_s1_conn_list_res_ack;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  reset_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResetAcknowledge ::= SEQUENCE
struct reset_ack_s {
  bool                    ext = false;
  reset_ack_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RetrieveUEInformation ::= SEQUENCE
struct retrieve_ue_info_s {
  bool                                            ext = false;
  protocol_ie_container_l<retrieve_ue_info_ies_o> protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct s1_setup_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                           time_to_wait_present     = false;
  bool                           crit_diagnostics_present = false;
  ie_field_s<cause_c>            cause;
  ie_field_s<time_to_wait_e>     time_to_wait;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  s1_setup_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// S1SetupFailure ::= SEQUENCE
struct s1_setup_fail_s {
  bool                        ext = false;
  s1_setup_fail_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct s1_setup_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                        enbname_present                    = false;
  bool                                                        csg_id_list_present                = false;
  bool                                                        ue_retention_info_present          = false;
  bool                                                        nb_io_t_default_paging_drx_present = false;
  ie_field_s<global_enb_id_s>                                 global_enb_id;
  ie_field_s<printable_string<1, 150, true, true> >           enbname;
  ie_field_s<dyn_seq_of<supported_tas_item_s, 1, 256, true> > supported_tas;
  ie_field_s<paging_drx_e>                                    default_paging_drx;
  ie_field_s<dyn_seq_of<csg_id_list_item_s, 1, 256, true> >   csg_id_list;
  ie_field_s<ue_retention_info_e>                             ue_retention_info;
  ie_field_s<nb_io_t_default_paging_drx_e>                    nb_io_t_default_paging_drx;

  // sequence methods
  s1_setup_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// S1SetupRequest ::= SEQUENCE
struct s1_setup_request_s {
  bool                           ext = false;
  s1_setup_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct s1_setup_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       mm_ename_present              = false;
  bool                                                       mme_relay_support_ind_present = false;
  bool                                                       crit_diagnostics_present      = false;
  bool                                                       ue_retention_info_present     = false;
  bool                                                       served_dcns_present           = false;
  ie_field_s<printable_string<1, 150, true, true> >          mm_ename;
  ie_field_s<dyn_seq_of<served_gummeis_item_s, 1, 8, true> > served_gummeis;
  ie_field_s<integer<uint16_t, 0, 255, false, true> >        relative_mme_capacity;
  ie_field_s<mme_relay_support_ind_e>                        mme_relay_support_ind;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;
  ie_field_s<ue_retention_info_e>                            ue_retention_info;
  ie_field_s<dyn_seq_of<served_dcns_item_s, 0, 32, true> >   served_dcns;

  // sequence methods
  s1_setup_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// S1SetupResponse ::= SEQUENCE
struct s1_setup_resp_s {
  bool                        ext = false;
  s1_setup_resp_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct trace_fail_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<fixed_octstring<8, true> >                      e_utran_trace_id;
  ie_field_s<cause_c>                                        cause;

  // sequence methods
  trace_fail_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceFailureIndication ::= SEQUENCE
struct trace_fail_ind_s {
  bool                         ext = false;
  trace_fail_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct trace_start_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<trace_activation_s>                             trace_activation;

  // sequence methods
  trace_start_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceStart ::= SEQUENCE
struct trace_start_s {
  bool                      ext = false;
  trace_start_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_cap_info_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       ue_radio_cap_for_paging_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<unbounded_octstring<true> >                     ue_radio_cap;
  ie_field_s<unbounded_octstring<true> >                     ue_radio_cap_for_paging;

  // sequence methods
  ue_cap_info_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInfoIndication ::= SEQUENCE
struct ue_cap_info_ind_s {
  bool                          ext = false;
  ue_cap_info_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_mod_confirm_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       csg_membership_status_present = false;
  bool                                                       crit_diagnostics_present      = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<csg_membership_status_e>                        csg_membership_status;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;

  // sequence methods
  ue_context_mod_confirm_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationConfirm ::= SEQUENCE
struct ue_context_mod_confirm_s {
  bool                                 ext = false;
  ue_context_mod_confirm_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_mod_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<cause_c>                                        cause;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;

  // sequence methods
  ue_context_mod_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationFailure ::= SEQUENCE
struct ue_context_mod_fail_s {
  bool                              ext = false;
  ue_context_mod_fail_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_mod_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       csg_membership_info_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<csg_membership_info_s>                          csg_membership_info;

  // sequence methods
  ue_context_mod_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationIndication ::= SEQUENCE
struct ue_context_mod_ind_s {
  bool                             ext = false;
  ue_context_mod_ind_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_mod_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       security_key_present                          = false;
  bool                                                       subscriber_profile_idfor_rfp_present          = false;
  bool                                                       ueaggregate_maximum_bitrate_present           = false;
  bool                                                       cs_fallback_ind_present                       = false;
  bool                                                       ue_security_cap_present                       = false;
  bool                                                       csg_membership_status_present                 = false;
  bool                                                       registered_lai_present                        = false;
  bool                                                       add_cs_fallback_ind_present                   = false;
  bool                                                       pro_se_authorized_present                     = false;
  bool                                                       srvcc_operation_possible_present              = false;
  bool                                                       srvcc_operation_not_possible_present          = false;
  bool                                                       v2xservices_authorized_present                = false;
  bool                                                       ue_sidelink_aggregate_maximum_bitrate_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<fixed_bitstring<256, false, true> >             security_key;
  ie_field_s<integer<uint16_t, 1, 256, false, true> >        subscriber_profile_idfor_rfp;
  ie_field_s<ue_aggregate_maximum_bitrate_s>                 ueaggregate_maximum_bitrate;
  ie_field_s<cs_fallback_ind_e>                              cs_fallback_ind;
  ie_field_s<ue_security_cap_s>                              ue_security_cap;
  ie_field_s<csg_membership_status_e>                        csg_membership_status;
  ie_field_s<lai_s>                                          registered_lai;
  ie_field_s<add_cs_fallback_ind_e>                          add_cs_fallback_ind;
  ie_field_s<pro_se_authorized_s>                            pro_se_authorized;
  ie_field_s<srvcc_operation_possible_e>                     srvcc_operation_possible;
  ie_field_s<srvcc_operation_not_possible_e>                 srvcc_operation_not_possible;
  ie_field_s<v2xservices_authorized_s>                       v2xservices_authorized;
  ie_field_s<ue_sidelink_aggregate_maximum_bitrate_s>        ue_sidelink_aggregate_maximum_bitrate;

  // sequence methods
  ue_context_mod_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationRequest ::= SEQUENCE
struct ue_context_mod_request_s {
  bool                                 ext = false;
  ue_context_mod_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_mod_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;

  // sequence methods
  ue_context_mod_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationResponse ::= SEQUENCE
struct ue_context_mod_resp_s {
  bool                              ext = false;
  ue_context_mod_resp_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_release_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<ue_s1ap_ids_c> ue_s1ap_ids;
  ie_field_s<cause_c>       cause;

  // sequence methods
  ue_context_release_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseCommand ::= SEQUENCE
struct ue_context_release_cmd_s {
  bool                                 ext = false;
  ue_context_release_cmd_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_release_complete_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool crit_diagnostics_present                              = false;
  bool user_location_info_present                            = false;
  bool info_on_recommended_cells_and_enbs_for_paging_present = false;
  bool cell_id_and_ce_level_for_ce_capable_ues_present       = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >  mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >    enb_ue_s1ap_id;
  ie_field_s<crit_diagnostics_s>                              crit_diagnostics;
  ie_field_s<user_location_info_s>                            user_location_info;
  ie_field_s<info_on_recommended_cells_and_enbs_for_paging_s> info_on_recommended_cells_and_enbs_for_paging;
  ie_field_s<cell_id_and_ce_level_for_ce_capable_ues_s>       cell_id_and_ce_level_for_ce_capable_ues;

  // sequence methods
  ue_context_release_complete_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseComplete ::= SEQUENCE
struct ue_context_release_complete_s {
  bool                                      ext = false;
  ue_context_release_complete_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_release_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       gw_context_release_ind_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<cause_c>                                        cause;
  ie_field_s<gw_context_release_ind_e>                       gw_context_release_ind;

  // sequence methods
  ue_context_release_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseRequest ::= SEQUENCE
struct ue_context_release_request_s {
  bool                                     ext = false;
  ue_context_release_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_resume_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<cause_c>                                        cause;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;

  // sequence methods
  ue_context_resume_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextResumeFailure ::= SEQUENCE
struct ue_context_resume_fail_s {
  bool                                 ext = false;
  ue_context_resume_fail_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_resume_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       erab_failed_to_resume_list_resume_req_present = false;
  bool                                                       rrc_resume_cause_present                      = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<erab_ie_container_list_l<erab_failed_to_resume_item_resume_req_ies_o> >
                                        erab_failed_to_resume_list_resume_req;
  ie_field_s<rrc_establishment_cause_e> rrc_resume_cause;

  // sequence methods
  ue_context_resume_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextResumeRequest ::= SEQUENCE
struct ue_context_resume_request_s {
  bool                                    ext = false;
  ue_context_resume_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_resume_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       erab_failed_to_resume_list_resume_res_present = false;
  bool                                                       crit_diagnostics_present                      = false;
  bool                                                       security_context_present                      = false;
  bool                                                       pending_data_ind_present                      = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<erab_ie_container_list_l<erab_failed_to_resume_item_resume_res_ies_o> >
                                 erab_failed_to_resume_list_resume_res;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;
  ie_field_s<security_context_s> security_context;
  ie_field_s<pending_data_ind_e> pending_data_ind;

  // sequence methods
  ue_context_resume_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextResumeResponse ::= SEQUENCE
struct ue_context_resume_resp_s {
  bool                                 ext = false;
  ue_context_resume_resp_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_suspend_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool info_on_recommended_cells_and_enbs_for_paging_present = false;
  bool cell_id_and_ce_level_for_ce_capable_ues_present       = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> >  mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >    enb_ue_s1ap_id;
  ie_field_s<info_on_recommended_cells_and_enbs_for_paging_s> info_on_recommended_cells_and_enbs_for_paging;
  ie_field_s<cell_id_and_ce_level_for_ce_capable_ues_s>       cell_id_and_ce_level_for_ce_capable_ues;

  // sequence methods
  ue_context_suspend_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextSuspendRequest ::= SEQUENCE
struct ue_context_suspend_request_s {
  bool                                     ext = false;
  ue_context_suspend_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_context_suspend_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       crit_diagnostics_present = false;
  bool                                                       security_context_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;
  ie_field_s<security_context_s>                             security_context;

  // sequence methods
  ue_context_suspend_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextSuspendResponse ::= SEQUENCE
struct ue_context_suspend_resp_s {
  bool                                  ext = false;
  ue_context_suspend_resp_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_info_transfer_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                   ue_level_qos_params_present = false;
  bool                                   ue_radio_cap_present        = false;
  bool                                   pending_data_ind_present    = false;
  ie_field_s<s_tmsi_s>                   s_tmsi;
  ie_field_s<erab_level_qos_params_s>    ue_level_qos_params;
  ie_field_s<unbounded_octstring<true> > ue_radio_cap;
  ie_field_s<pending_data_ind_e>         pending_data_ind;

  // sequence methods
  ue_info_transfer_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationTransfer ::= SEQUENCE
struct ue_info_transfer_s {
  bool                           ext = false;
  ue_info_transfer_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_radio_cap_match_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       ue_radio_cap_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<unbounded_octstring<true> >                     ue_radio_cap;

  // sequence methods
  ue_radio_cap_match_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioCapabilityMatchRequest ::= SEQUENCE
struct ue_radio_cap_match_request_s {
  bool                                     ext = false;
  ue_radio_cap_match_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ue_radio_cap_match_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<voice_support_match_ind_e>                      voice_support_match_ind;
  ie_field_s<crit_diagnostics_s>                             crit_diagnostics;

  // sequence methods
  ue_radio_cap_match_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioCapabilityMatchResponse ::= SEQUENCE
struct ue_radio_cap_match_resp_s {
  bool                                  ext = false;
  ue_radio_cap_match_resp_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ul_nas_transport_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       gw_transport_layer_address_present         = false;
  bool                                                       sipto_l_gw_transport_layer_address_present = false;
  bool                                                       lhn_id_present                             = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<unbounded_octstring<true> >                     nas_pdu;
  ie_field_s<eutran_cgi_s>                                   eutran_cgi;
  ie_field_s<tai_s>                                          tai;
  ie_field_s<bounded_bitstring<1, 160, true, true> >         gw_transport_layer_address;
  ie_field_s<bounded_bitstring<1, 160, true, true> >         sipto_l_gw_transport_layer_address;
  ie_field_s<unbounded_octstring<true> >                     lhn_id;

  // sequence methods
  ul_nas_transport_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkNASTransport ::= SEQUENCE
struct ul_nas_transport_s {
  bool                           ext = false;
  ul_nas_transport_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ul_non_ueassociated_lp_pa_transport_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, false, true> > routing_id;
  ie_field_s<unbounded_octstring<true> >              lp_pa_pdu;

  // sequence methods
  ul_non_ueassociated_lp_pa_transport_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkNonUEAssociatedLPPaTransport ::= SEQUENCE
struct ul_non_ueassociated_lp_pa_transport_s {
  bool                                              ext = false;
  ul_non_ueassociated_lp_pa_transport_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ul_s1cdma2000tunnelling_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       cdma2000_ho_required_ind_present                = false;
  bool                                                       cdma2000_one_xsrvcc_info_present                = false;
  bool                                                       cdma2000_one_xrand_present                      = false;
  bool                                                       eutran_round_trip_delay_estimation_info_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<cdma2000_rat_type_e>                            cdma2000_rat_type;
  ie_field_s<unbounded_octstring<true> >                     cdma2000_sector_id;
  ie_field_s<cdma2000_ho_required_ind_e>                     cdma2000_ho_required_ind;
  ie_field_s<cdma2000_one_xsrvcc_info_s>                     cdma2000_one_xsrvcc_info;
  ie_field_s<unbounded_octstring<true> >                     cdma2000_one_xrand;
  ie_field_s<unbounded_octstring<true> >                     cdma2000_pdu;
  ie_field_s<integer<uint16_t, 0, 2047, false, true> >       eutran_round_trip_delay_estimation_info;

  // sequence methods
  ul_s1cdma2000tunnelling_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkS1cdma2000tunnelling ::= SEQUENCE
struct ul_s1cdma2000tunnelling_s {
  bool                                  ext = false;
  ul_s1cdma2000tunnelling_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ul_ueassociated_lp_pa_transport_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true> > mme_ue_s1ap_id;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true> >   enb_ue_s1ap_id;
  ie_field_s<integer<uint16_t, 0, 255, false, true> >        routing_id;
  ie_field_s<unbounded_octstring<true> >                     lp_pa_pdu;

  // sequence methods
  ul_ueassociated_lp_pa_transport_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkUEAssociatedLPPaTransport ::= SEQUENCE
struct ul_ueassociated_lp_pa_transport_s {
  bool                                          ext = false;
  ul_ueassociated_lp_pa_transport_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct write_replace_warning_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      warning_area_list_present          = false;
  bool                                                      extended_repeat_period_present     = false;
  bool                                                      warning_type_present               = false;
  bool                                                      warning_security_info_present      = false;
  bool                                                      data_coding_scheme_present         = false;
  bool                                                      warning_msg_contents_present       = false;
  bool                                                      concurrent_warning_msg_ind_present = false;
  ie_field_s<fixed_bitstring<16, false, true> >             msg_id;
  ie_field_s<fixed_bitstring<16, false, true> >             serial_num;
  ie_field_s<warning_area_list_c>                           warning_area_list;
  ie_field_s<integer<uint16_t, 0, 4095, false, true> >      repeat_period;
  ie_field_s<integer<uint32_t, 4096, 131071, false, true> > extended_repeat_period;
  ie_field_s<integer<uint32_t, 0, 65535, false, true> >     numof_broadcast_request;
  ie_field_s<fixed_octstring<2, true> >                     warning_type;
  ie_field_s<fixed_octstring<50, true> >                    warning_security_info;
  ie_field_s<fixed_bitstring<8, false, true> >              data_coding_scheme;
  ie_field_s<unbounded_octstring<true> >                    warning_msg_contents;
  ie_field_s<concurrent_warning_msg_ind_e>                  concurrent_warning_msg_ind;

  // sequence methods
  write_replace_warning_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WriteReplaceWarningRequest ::= SEQUENCE
struct write_replace_warning_request_s {
  bool                                        ext = false;
  write_replace_warning_request_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct write_replace_warning_resp_ies_container {
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
  write_replace_warning_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WriteReplaceWarningResponse ::= SEQUENCE
struct write_replace_warning_resp_s {
  bool                                     ext = false;
  write_replace_warning_resp_ies_container protocol_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// S1AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF S1AP-ELEMENTARY-PROCEDURE
struct s1ap_elem_procs_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        ho_required,
        ho_request,
        path_switch_request,
        erab_setup_request,
        erab_modify_request,
        erab_release_cmd,
        init_context_setup_request,
        ho_cancel,
        kill_request,
        reset,
        s1_setup_request,
        ue_context_mod_request,
        ue_context_release_cmd,
        enb_cfg_upd,
        mme_cfg_upd,
        write_replace_warning_request,
        ho_notify,
        erab_release_ind,
        paging,
        dl_nas_transport,
        init_ue_msg,
        ul_nas_transport,
        error_ind,
        nas_non_delivery_ind,
        ue_context_release_request,
        dl_s1cdma2000tunnelling,
        ul_s1cdma2000tunnelling,
        ue_cap_info_ind,
        enb_status_transfer,
        mme_status_transfer,
        deactiv_trace,
        trace_start,
        trace_fail_ind,
        cell_traffic_trace,
        location_report_ctrl,
        location_report_fail_ind,
        location_report,
        overload_start,
        overload_stop,
        enb_direct_info_transfer,
        mme_direct_info_transfer,
        enb_cfg_transfer,
        mme_cfg_transfer,
        private_msg,
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ho_required_s&                         ho_required();
    ho_request_s&                          ho_request();
    path_switch_request_s&                 path_switch_request();
    erab_setup_request_s&                  erab_setup_request();
    erab_modify_request_s&                 erab_modify_request();
    erab_release_cmd_s&                    erab_release_cmd();
    init_context_setup_request_s&          init_context_setup_request();
    ho_cancel_s&                           ho_cancel();
    kill_request_s&                        kill_request();
    reset_s&                               reset();
    s1_setup_request_s&                    s1_setup_request();
    ue_context_mod_request_s&              ue_context_mod_request();
    ue_context_release_cmd_s&              ue_context_release_cmd();
    enb_cfg_upd_s&                         enb_cfg_upd();
    mme_cfg_upd_s&                         mme_cfg_upd();
    write_replace_warning_request_s&       write_replace_warning_request();
    ho_notify_s&                           ho_notify();
    erab_release_ind_s&                    erab_release_ind();
    paging_s&                              paging();
    dl_nas_transport_s&                    dl_nas_transport();
    init_ue_msg_s&                         init_ue_msg();
    ul_nas_transport_s&                    ul_nas_transport();
    error_ind_s&                           error_ind();
    nas_non_delivery_ind_s&                nas_non_delivery_ind();
    ue_context_release_request_s&          ue_context_release_request();
    dl_s1cdma2000tunnelling_s&             dl_s1cdma2000tunnelling();
    ul_s1cdma2000tunnelling_s&             ul_s1cdma2000tunnelling();
    ue_cap_info_ind_s&                     ue_cap_info_ind();
    enb_status_transfer_s&                 enb_status_transfer();
    mme_status_transfer_s&                 mme_status_transfer();
    deactiv_trace_s&                       deactiv_trace();
    trace_start_s&                         trace_start();
    trace_fail_ind_s&                      trace_fail_ind();
    cell_traffic_trace_s&                  cell_traffic_trace();
    location_report_ctrl_s&                location_report_ctrl();
    location_report_fail_ind_s&            location_report_fail_ind();
    location_report_s&                     location_report();
    overload_start_s&                      overload_start();
    overload_stop_s&                       overload_stop();
    enb_direct_info_transfer_s&            enb_direct_info_transfer();
    mme_direct_info_transfer_s&            mme_direct_info_transfer();
    enb_cfg_transfer_s&                    enb_cfg_transfer();
    mme_cfg_transfer_s&                    mme_cfg_transfer();
    private_msg_s&                         private_msg();
    const ho_required_s&                   ho_required() const;
    const ho_request_s&                    ho_request() const;
    const path_switch_request_s&           path_switch_request() const;
    const erab_setup_request_s&            erab_setup_request() const;
    const erab_modify_request_s&           erab_modify_request() const;
    const erab_release_cmd_s&              erab_release_cmd() const;
    const init_context_setup_request_s&    init_context_setup_request() const;
    const ho_cancel_s&                     ho_cancel() const;
    const kill_request_s&                  kill_request() const;
    const reset_s&                         reset() const;
    const s1_setup_request_s&              s1_setup_request() const;
    const ue_context_mod_request_s&        ue_context_mod_request() const;
    const ue_context_release_cmd_s&        ue_context_release_cmd() const;
    const enb_cfg_upd_s&                   enb_cfg_upd() const;
    const mme_cfg_upd_s&                   mme_cfg_upd() const;
    const write_replace_warning_request_s& write_replace_warning_request() const;
    const ho_notify_s&                     ho_notify() const;
    const erab_release_ind_s&              erab_release_ind() const;
    const paging_s&                        paging() const;
    const dl_nas_transport_s&              dl_nas_transport() const;
    const init_ue_msg_s&                   init_ue_msg() const;
    const ul_nas_transport_s&              ul_nas_transport() const;
    const error_ind_s&                     error_ind() const;
    const nas_non_delivery_ind_s&          nas_non_delivery_ind() const;
    const ue_context_release_request_s&    ue_context_release_request() const;
    const dl_s1cdma2000tunnelling_s&       dl_s1cdma2000tunnelling() const;
    const ul_s1cdma2000tunnelling_s&       ul_s1cdma2000tunnelling() const;
    const ue_cap_info_ind_s&               ue_cap_info_ind() const;
    const enb_status_transfer_s&           enb_status_transfer() const;
    const mme_status_transfer_s&           mme_status_transfer() const;
    const deactiv_trace_s&                 deactiv_trace() const;
    const trace_start_s&                   trace_start() const;
    const trace_fail_ind_s&                trace_fail_ind() const;
    const cell_traffic_trace_s&            cell_traffic_trace() const;
    const location_report_ctrl_s&          location_report_ctrl() const;
    const location_report_fail_ind_s&      location_report_fail_ind() const;
    const location_report_s&               location_report() const;
    const overload_start_s&                overload_start() const;
    const overload_stop_s&                 overload_stop() const;
    const enb_direct_info_transfer_s&      enb_direct_info_transfer() const;
    const mme_direct_info_transfer_s&      mme_direct_info_transfer() const;
    const enb_cfg_transfer_s&              enb_cfg_transfer() const;
    const mme_cfg_transfer_s&              mme_cfg_transfer() const;
    const private_msg_s&                   private_msg() const;

  private:
    types type_;
    choice_buffer_t<cell_traffic_trace_s,
                    deactiv_trace_s,
                    dl_nas_transport_s,
                    dl_s1cdma2000tunnelling_s,
                    enb_cfg_transfer_s,
                    enb_cfg_upd_s,
                    enb_direct_info_transfer_s,
                    enb_status_transfer_s,
                    erab_modify_request_s,
                    erab_release_cmd_s,
                    erab_release_ind_s,
                    erab_setup_request_s,
                    error_ind_s,
                    ho_cancel_s,
                    ho_notify_s,
                    ho_request_s,
                    ho_required_s,
                    init_context_setup_request_s,
                    init_ue_msg_s,
                    kill_request_s,
                    location_report_ctrl_s,
                    location_report_fail_ind_s,
                    location_report_s,
                    mme_cfg_transfer_s,
                    mme_cfg_upd_s,
                    mme_direct_info_transfer_s,
                    mme_status_transfer_s,
                    nas_non_delivery_ind_s,
                    overload_start_s,
                    overload_stop_s,
                    paging_s,
                    path_switch_request_s,
                    private_msg_s,
                    reset_s,
                    s1_setup_request_s,
                    trace_fail_ind_s,
                    trace_start_s,
                    ue_cap_info_ind_s,
                    ue_context_mod_request_s,
                    ue_context_release_cmd_s,
                    ue_context_release_request_s,
                    ul_nas_transport_s,
                    ul_s1cdma2000tunnelling_s,
                    write_replace_warning_request_s>
        c;

    void destroy_();
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        ho_cmd,
        ho_request_ack,
        path_switch_request_ack,
        erab_setup_resp,
        erab_modify_resp,
        erab_release_resp,
        init_context_setup_resp,
        ho_cancel_ack,
        kill_resp,
        reset_ack,
        s1_setup_resp,
        ue_context_mod_resp,
        ue_context_release_complete,
        enb_cfg_upd_ack,
        mme_cfg_upd_ack,
        write_replace_warning_resp,
        nulltype
      } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ho_cmd_s&                            ho_cmd();
    ho_request_ack_s&                    ho_request_ack();
    path_switch_request_ack_s&           path_switch_request_ack();
    erab_setup_resp_s&                   erab_setup_resp();
    erab_modify_resp_s&                  erab_modify_resp();
    erab_release_resp_s&                 erab_release_resp();
    init_context_setup_resp_s&           init_context_setup_resp();
    ho_cancel_ack_s&                     ho_cancel_ack();
    kill_resp_s&                         kill_resp();
    reset_ack_s&                         reset_ack();
    s1_setup_resp_s&                     s1_setup_resp();
    ue_context_mod_resp_s&               ue_context_mod_resp();
    ue_context_release_complete_s&       ue_context_release_complete();
    enb_cfg_upd_ack_s&                   enb_cfg_upd_ack();
    mme_cfg_upd_ack_s&                   mme_cfg_upd_ack();
    write_replace_warning_resp_s&        write_replace_warning_resp();
    const ho_cmd_s&                      ho_cmd() const;
    const ho_request_ack_s&              ho_request_ack() const;
    const path_switch_request_ack_s&     path_switch_request_ack() const;
    const erab_setup_resp_s&             erab_setup_resp() const;
    const erab_modify_resp_s&            erab_modify_resp() const;
    const erab_release_resp_s&           erab_release_resp() const;
    const init_context_setup_resp_s&     init_context_setup_resp() const;
    const ho_cancel_ack_s&               ho_cancel_ack() const;
    const kill_resp_s&                   kill_resp() const;
    const reset_ack_s&                   reset_ack() const;
    const s1_setup_resp_s&               s1_setup_resp() const;
    const ue_context_mod_resp_s&         ue_context_mod_resp() const;
    const ue_context_release_complete_s& ue_context_release_complete() const;
    const enb_cfg_upd_ack_s&             enb_cfg_upd_ack() const;
    const mme_cfg_upd_ack_s&             mme_cfg_upd_ack() const;
    const write_replace_warning_resp_s&  write_replace_warning_resp() const;

  private:
    types type_;
    choice_buffer_t<enb_cfg_upd_ack_s,
                    erab_modify_resp_s,
                    erab_release_resp_s,
                    erab_setup_resp_s,
                    ho_cancel_ack_s,
                    ho_cmd_s,
                    ho_request_ack_s,
                    init_context_setup_resp_s,
                    kill_resp_s,
                    mme_cfg_upd_ack_s,
                    path_switch_request_ack_s,
                    reset_ack_s,
                    s1_setup_resp_s,
                    ue_context_mod_resp_s,
                    ue_context_release_complete_s,
                    write_replace_warning_resp_s>
        c;

    void destroy_();
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        ho_prep_fail,
        ho_fail,
        path_switch_request_fail,
        init_context_setup_fail,
        s1_setup_fail,
        ue_context_mod_fail,
        enb_cfg_upd_fail,
        mme_cfg_upd_fail,
        nulltype
      } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ho_prep_fail_s&                   ho_prep_fail();
    ho_fail_s&                        ho_fail();
    path_switch_request_fail_s&       path_switch_request_fail();
    init_context_setup_fail_s&        init_context_setup_fail();
    s1_setup_fail_s&                  s1_setup_fail();
    ue_context_mod_fail_s&            ue_context_mod_fail();
    enb_cfg_upd_fail_s&               enb_cfg_upd_fail();
    mme_cfg_upd_fail_s&               mme_cfg_upd_fail();
    const ho_prep_fail_s&             ho_prep_fail() const;
    const ho_fail_s&                  ho_fail() const;
    const path_switch_request_fail_s& path_switch_request_fail() const;
    const init_context_setup_fail_s&  init_context_setup_fail() const;
    const s1_setup_fail_s&            s1_setup_fail() const;
    const ue_context_mod_fail_s&      ue_context_mod_fail() const;
    const enb_cfg_upd_fail_s&         enb_cfg_upd_fail() const;
    const mme_cfg_upd_fail_s&         mme_cfg_upd_fail() const;

  private:
    types type_;
    choice_buffer_t<enb_cfg_upd_fail_s,
                    ho_fail_s,
                    ho_prep_fail_s,
                    init_context_setup_fail_s,
                    mme_cfg_upd_fail_s,
                    path_switch_request_fail_s,
                    s1_setup_fail_s,
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

// InitiatingMessage ::= SEQUENCE{{S1AP-ELEMENTARY-PROCEDURE}}
struct init_msg_s {
  uint16_t                      proc_code = 0;
  crit_e                        crit;
  s1ap_elem_procs_o::init_msg_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// LastVisitedEUTRANCellInformation-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct last_visited_eutran_cell_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { time_ue_stayed_in_cell_enhanced_granularity, ho_cause, nulltype } value;
      typedef uint8_t number_type;

      std::string to_string() const;
      uint8_t     to_number() const;
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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&       time_ue_stayed_in_cell_enhanced_granularity();
    cause_c&        ho_cause();
    const uint16_t& time_ue_stayed_in_cell_enhanced_granularity() const;
    const cause_c&  ho_cause() const;

  private:
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct last_visited_eutran_cell_info_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                                  time_ue_stayed_in_cell_enhanced_granularity_present = false;
  bool                                                  ho_cause_present                                    = false;
  ie_field_s<integer<uint16_t, 0, 40950, false, true> > time_ue_stayed_in_cell_enhanced_granularity;
  ie_field_s<cause_c>                                   ho_cause;

  // sequence methods
  last_visited_eutran_cell_info_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LastVisitedEUTRANCellInformation ::= SEQUENCE
struct last_visited_eutran_cell_info_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  eutran_cgi_s                                    global_cell_id;
  cell_type_s                                     cell_type;
  uint16_t                                        time_ue_stayed_in_cell = 0;
  last_visited_eutran_cell_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LastVisitedGERANCellInformation ::= CHOICE
struct last_visited_geran_cell_info_c {
  struct types_opts {
    enum options { undefined, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  types       type() const { return types::undefined; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LastVisitedCell-Item ::= CHOICE
struct last_visited_cell_item_c {
  struct types_opts {
    enum options { e_utran_cell, utran_cell, geran_cell, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  last_visited_cell_item_c() = default;
  last_visited_cell_item_c(const last_visited_cell_item_c& other);
  last_visited_cell_item_c& operator=(const last_visited_cell_item_c& other);
  ~last_visited_cell_item_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  last_visited_eutran_cell_info_s& e_utran_cell()
  {
    assert_choice_type("e-UTRAN-Cell", type_.to_string(), "LastVisitedCell-Item");
    return c.get<last_visited_eutran_cell_info_s>();
  }
  unbounded_octstring<true>& utran_cell()
  {
    assert_choice_type("uTRAN-Cell", type_.to_string(), "LastVisitedCell-Item");
    return c.get<unbounded_octstring<true> >();
  }
  last_visited_geran_cell_info_c& geran_cell()
  {
    assert_choice_type("gERAN-Cell", type_.to_string(), "LastVisitedCell-Item");
    return c.get<last_visited_geran_cell_info_c>();
  }
  const last_visited_eutran_cell_info_s& e_utran_cell() const
  {
    assert_choice_type("e-UTRAN-Cell", type_.to_string(), "LastVisitedCell-Item");
    return c.get<last_visited_eutran_cell_info_s>();
  }
  const unbounded_octstring<true>& utran_cell() const
  {
    assert_choice_type("uTRAN-Cell", type_.to_string(), "LastVisitedCell-Item");
    return c.get<unbounded_octstring<true> >();
  }
  const last_visited_geran_cell_info_c& geran_cell() const
  {
    assert_choice_type("gERAN-Cell", type_.to_string(), "LastVisitedCell-Item");
    return c.get<last_visited_geran_cell_info_c>();
  }
  last_visited_eutran_cell_info_s& set_e_utran_cell()
  {
    set(types::e_utran_cell);
    return c.get<last_visited_eutran_cell_info_s>();
  }
  unbounded_octstring<true>& set_utran_cell()
  {
    set(types::utran_cell);
    return c.get<unbounded_octstring<true> >();
  }
  last_visited_geran_cell_info_c& set_geran_cell()
  {
    set(types::geran_cell);
    return c.get<last_visited_geran_cell_info_c>();
  }

private:
  types                                                                                                        type_;
  choice_buffer_t<last_visited_eutran_cell_info_s, last_visited_geran_cell_info_c, unbounded_octstring<true> > c;

  void destroy_();
};

// RequestedCellList ::= SEQUENCE (SIZE (1..128)) OF IRAT-Cell-ID
using requested_cell_list_l = dyn_array<irat_cell_id_c>;

// MultiCellLoadReportingRequest ::= SEQUENCE
struct multi_cell_load_report_request_s {
  bool                  ext = false;
  requested_cell_list_l requested_cell_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiCellLoadReportingResponse-Item ::= CHOICE
struct multi_cell_load_report_resp_item_c {
  struct types_opts {
    enum options { eutran_resp, utran_resp, geran_resp, /*...*/ ehrpd, nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true, 1> types;

  // choice methods
  multi_cell_load_report_resp_item_c() = default;
  multi_cell_load_report_resp_item_c(const multi_cell_load_report_resp_item_c& other);
  multi_cell_load_report_resp_item_c& operator=(const multi_cell_load_report_resp_item_c& other);
  ~multi_cell_load_report_resp_item_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutran_resp_s& eutran_resp()
  {
    assert_choice_type("eUTRANResponse", type_.to_string(), "MultiCellLoadReportingResponse-Item");
    return c.get<eutran_resp_s>();
  }
  unbounded_octstring<true>& utran_resp()
  {
    assert_choice_type("uTRANResponse", type_.to_string(), "MultiCellLoadReportingResponse-Item");
    return c.get<unbounded_octstring<true> >();
  }
  unbounded_octstring<true>& geran_resp()
  {
    assert_choice_type("gERANResponse", type_.to_string(), "MultiCellLoadReportingResponse-Item");
    return c.get<unbounded_octstring<true> >();
  }
  ehrpd_multi_sector_load_report_resp_item_s& ehrpd()
  {
    assert_choice_type("eHRPD", type_.to_string(), "MultiCellLoadReportingResponse-Item");
    return c.get<ehrpd_multi_sector_load_report_resp_item_s>();
  }
  const eutran_resp_s& eutran_resp() const
  {
    assert_choice_type("eUTRANResponse", type_.to_string(), "MultiCellLoadReportingResponse-Item");
    return c.get<eutran_resp_s>();
  }
  const unbounded_octstring<true>& utran_resp() const
  {
    assert_choice_type("uTRANResponse", type_.to_string(), "MultiCellLoadReportingResponse-Item");
    return c.get<unbounded_octstring<true> >();
  }
  const unbounded_octstring<true>& geran_resp() const
  {
    assert_choice_type("gERANResponse", type_.to_string(), "MultiCellLoadReportingResponse-Item");
    return c.get<unbounded_octstring<true> >();
  }
  const ehrpd_multi_sector_load_report_resp_item_s& ehrpd() const
  {
    assert_choice_type("eHRPD", type_.to_string(), "MultiCellLoadReportingResponse-Item");
    return c.get<ehrpd_multi_sector_load_report_resp_item_s>();
  }
  eutran_resp_s& set_eutran_resp()
  {
    set(types::eutran_resp);
    return c.get<eutran_resp_s>();
  }
  unbounded_octstring<true>& set_utran_resp()
  {
    set(types::utran_resp);
    return c.get<unbounded_octstring<true> >();
  }
  unbounded_octstring<true>& set_geran_resp()
  {
    set(types::geran_resp);
    return c.get<unbounded_octstring<true> >();
  }
  ehrpd_multi_sector_load_report_resp_item_s& set_ehrpd()
  {
    set(types::ehrpd);
    return c.get<ehrpd_multi_sector_load_report_resp_item_s>();
  }

private:
  types                                                                                                  type_;
  choice_buffer_t<ehrpd_multi_sector_load_report_resp_item_s, eutran_resp_s, unbounded_octstring<true> > c;

  void destroy_();
};

// MultiCellLoadReportingResponse ::= SEQUENCE (SIZE (1..128)) OF MultiCellLoadReportingResponse-Item
using multi_cell_load_report_resp_l = dyn_array<multi_cell_load_report_resp_item_c>;

// ProtocolError-IE-ContainerList{S1AP-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE (SIZE (1..256)) OF
// ProtocolIE-SingleContainer{S1AP-PROTOCOL-IES : IEsSetParam}
template <class ies_set_paramT_>
using protocol_error_ie_container_list_l =
    dyn_seq_of<protocol_ie_single_container_item_s<ies_set_paramT_>, 1, 256, true>;

// ReportingCellList-Item ::= SEQUENCE
struct report_cell_list_item_s {
  bool           ext = false;
  irat_cell_id_c cell_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportingCellList ::= SEQUENCE (SIZE (1..128)) OF ReportingCellList-Item
using report_cell_list_l = dyn_array<report_cell_list_item_s>;

// SuccessfulOutcome ::= SEQUENCE{{S1AP-ELEMENTARY-PROCEDURE}}
struct successful_outcome_s {
  uint16_t                                proc_code = 0;
  crit_e                                  crit;
  s1ap_elem_procs_o::successful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// UnsuccessfulOutcome ::= SEQUENCE{{S1AP-ELEMENTARY-PROCEDURE}}
struct unsuccessful_outcome_s {
  uint16_t                                  proc_code = 0;
  crit_e                                    crit;
  s1ap_elem_procs_o::unsuccessful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// S1AP-PDU ::= CHOICE
struct s1ap_pdu_c {
  struct types_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, /*...*/ nulltype } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  s1ap_pdu_c() = default;
  s1ap_pdu_c(const s1ap_pdu_c& other);
  s1ap_pdu_c& operator=(const s1ap_pdu_c& other);
  ~s1ap_pdu_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  init_msg_s& init_msg()
  {
    assert_choice_type("initiatingMessage", type_.to_string(), "S1AP-PDU");
    return c.get<init_msg_s>();
  }
  successful_outcome_s& successful_outcome()
  {
    assert_choice_type("successfulOutcome", type_.to_string(), "S1AP-PDU");
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& unsuccessful_outcome()
  {
    assert_choice_type("unsuccessfulOutcome", type_.to_string(), "S1AP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  const init_msg_s& init_msg() const
  {
    assert_choice_type("initiatingMessage", type_.to_string(), "S1AP-PDU");
    return c.get<init_msg_s>();
  }
  const successful_outcome_s& successful_outcome() const
  {
    assert_choice_type("successfulOutcome", type_.to_string(), "S1AP-PDU");
    return c.get<successful_outcome_s>();
  }
  const unsuccessful_outcome_s& unsuccessful_outcome() const
  {
    assert_choice_type("unsuccessfulOutcome", type_.to_string(), "S1AP-PDU");
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

// CellActivationCause ::= ENUMERATED
struct cell_activation_cause_opts {
  enum options { application_container_syntax_error, inconsistent_report_cell_id, unspecified, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cell_activation_cause_opts, true> cell_activation_cause_e;

// CellLoadReportingCause ::= ENUMERATED
struct cell_load_report_cause_opts {
  enum options { application_container_syntax_error, inconsistent_report_cell_id, unspecified, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cell_load_report_cause_opts, true> cell_load_report_cause_e;

// CellStateIndicationCause ::= ENUMERATED
struct cell_state_ind_cause_opts {
  enum options { application_container_syntax_error, inconsistent_report_cell_id, unspecified, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<cell_state_ind_cause_opts, true> cell_state_ind_cause_e;

// FailureEventReportingCause ::= ENUMERATED
struct fail_event_report_cause_opts {
  enum options { application_container_syntax_error, inconsistent_report_cell_id, unspecified, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<fail_event_report_cause_opts, true> fail_event_report_cause_e;

// HOReportingCause ::= ENUMERATED
struct ho_report_cause_opts {
  enum options { application_container_syntax_error, inconsistent_report_cell_id, unspecified, /*...*/ nulltype } value;

  std::string to_string() const;
};
typedef enumerated<ho_report_cause_opts, true> ho_report_cause_e;

// SONtransferCause ::= CHOICE
struct so_ntransfer_cause_c {
  struct types_opts {
    enum options {
      cell_load_report,
      // ...
      multi_cell_load_report,
      event_triggered_cell_load_report,
      horeport,
      eutran_cell_activation,
      energy_savings_ind,
      fail_event_report,
      nulltype
    } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true, 6> types;

  // choice methods
  so_ntransfer_cause_c() = default;
  so_ntransfer_cause_c(const so_ntransfer_cause_c& other);
  so_ntransfer_cause_c& operator=(const so_ntransfer_cause_c& other);
  ~so_ntransfer_cause_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_load_report_cause_e& cell_load_report()
  {
    assert_choice_type("cellLoadReporting", type_.to_string(), "SONtransferCause");
    return c.get<cell_load_report_cause_e>();
  }
  cell_load_report_cause_e& multi_cell_load_report()
  {
    assert_choice_type("multiCellLoadReporting", type_.to_string(), "SONtransferCause");
    return c.get<cell_load_report_cause_e>();
  }
  cell_load_report_cause_e& event_triggered_cell_load_report()
  {
    assert_choice_type("eventTriggeredCellLoadReporting", type_.to_string(), "SONtransferCause");
    return c.get<cell_load_report_cause_e>();
  }
  ho_report_cause_e& horeport()
  {
    assert_choice_type("hOReporting", type_.to_string(), "SONtransferCause");
    return c.get<ho_report_cause_e>();
  }
  cell_activation_cause_e& eutran_cell_activation()
  {
    assert_choice_type("eutranCellActivation", type_.to_string(), "SONtransferCause");
    return c.get<cell_activation_cause_e>();
  }
  cell_state_ind_cause_e& energy_savings_ind()
  {
    assert_choice_type("energySavingsIndication", type_.to_string(), "SONtransferCause");
    return c.get<cell_state_ind_cause_e>();
  }
  fail_event_report_cause_e& fail_event_report()
  {
    assert_choice_type("failureEventReporting", type_.to_string(), "SONtransferCause");
    return c.get<fail_event_report_cause_e>();
  }
  const cell_load_report_cause_e& cell_load_report() const
  {
    assert_choice_type("cellLoadReporting", type_.to_string(), "SONtransferCause");
    return c.get<cell_load_report_cause_e>();
  }
  const cell_load_report_cause_e& multi_cell_load_report() const
  {
    assert_choice_type("multiCellLoadReporting", type_.to_string(), "SONtransferCause");
    return c.get<cell_load_report_cause_e>();
  }
  const cell_load_report_cause_e& event_triggered_cell_load_report() const
  {
    assert_choice_type("eventTriggeredCellLoadReporting", type_.to_string(), "SONtransferCause");
    return c.get<cell_load_report_cause_e>();
  }
  const ho_report_cause_e& horeport() const
  {
    assert_choice_type("hOReporting", type_.to_string(), "SONtransferCause");
    return c.get<ho_report_cause_e>();
  }
  const cell_activation_cause_e& eutran_cell_activation() const
  {
    assert_choice_type("eutranCellActivation", type_.to_string(), "SONtransferCause");
    return c.get<cell_activation_cause_e>();
  }
  const cell_state_ind_cause_e& energy_savings_ind() const
  {
    assert_choice_type("energySavingsIndication", type_.to_string(), "SONtransferCause");
    return c.get<cell_state_ind_cause_e>();
  }
  const fail_event_report_cause_e& fail_event_report() const
  {
    assert_choice_type("failureEventReporting", type_.to_string(), "SONtransferCause");
    return c.get<fail_event_report_cause_e>();
  }
  cell_load_report_cause_e& set_cell_load_report()
  {
    set(types::cell_load_report);
    return c.get<cell_load_report_cause_e>();
  }
  cell_load_report_cause_e& set_multi_cell_load_report()
  {
    set(types::multi_cell_load_report);
    return c.get<cell_load_report_cause_e>();
  }
  cell_load_report_cause_e& set_event_triggered_cell_load_report()
  {
    set(types::event_triggered_cell_load_report);
    return c.get<cell_load_report_cause_e>();
  }
  ho_report_cause_e& set_horeport()
  {
    set(types::horeport);
    return c.get<ho_report_cause_e>();
  }
  cell_activation_cause_e& set_eutran_cell_activation()
  {
    set(types::eutran_cell_activation);
    return c.get<cell_activation_cause_e>();
  }
  cell_state_ind_cause_e& set_energy_savings_ind()
  {
    set(types::energy_savings_ind);
    return c.get<cell_state_ind_cause_e>();
  }
  fail_event_report_cause_e& set_fail_event_report()
  {
    set(types::fail_event_report);
    return c.get<fail_event_report_cause_e>();
  }

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// SONtransferRequestContainer ::= CHOICE
struct so_ntransfer_request_container_c {
  struct types_opts {
    enum options {
      cell_load_report,
      // ...
      multi_cell_load_report,
      event_triggered_cell_load_report,
      horeport,
      eutran_cell_activation,
      energy_savings_ind,
      fail_event_report,
      nulltype
    } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true, 6> types;

  // choice methods
  so_ntransfer_request_container_c() = default;
  so_ntransfer_request_container_c(const so_ntransfer_request_container_c& other);
  so_ntransfer_request_container_c& operator=(const so_ntransfer_request_container_c& other);
  ~so_ntransfer_request_container_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  multi_cell_load_report_request_s& multi_cell_load_report()
  {
    assert_choice_type("multiCellLoadReporting", type_.to_string(), "SONtransferRequestContainer");
    return c.get<multi_cell_load_report_request_s>();
  }
  event_triggered_cell_load_report_request_s& event_triggered_cell_load_report()
  {
    assert_choice_type("eventTriggeredCellLoadReporting", type_.to_string(), "SONtransferRequestContainer");
    return c.get<event_triggered_cell_load_report_request_s>();
  }
  ho_report_s& horeport()
  {
    assert_choice_type("hOReporting", type_.to_string(), "SONtransferRequestContainer");
    return c.get<ho_report_s>();
  }
  cell_activation_request_s& eutran_cell_activation()
  {
    assert_choice_type("eutranCellActivation", type_.to_string(), "SONtransferRequestContainer");
    return c.get<cell_activation_request_s>();
  }
  cell_state_ind_s& energy_savings_ind()
  {
    assert_choice_type("energySavingsIndication", type_.to_string(), "SONtransferRequestContainer");
    return c.get<cell_state_ind_s>();
  }
  fail_event_report_c& fail_event_report()
  {
    assert_choice_type("failureEventReporting", type_.to_string(), "SONtransferRequestContainer");
    return c.get<fail_event_report_c>();
  }
  const multi_cell_load_report_request_s& multi_cell_load_report() const
  {
    assert_choice_type("multiCellLoadReporting", type_.to_string(), "SONtransferRequestContainer");
    return c.get<multi_cell_load_report_request_s>();
  }
  const event_triggered_cell_load_report_request_s& event_triggered_cell_load_report() const
  {
    assert_choice_type("eventTriggeredCellLoadReporting", type_.to_string(), "SONtransferRequestContainer");
    return c.get<event_triggered_cell_load_report_request_s>();
  }
  const ho_report_s& horeport() const
  {
    assert_choice_type("hOReporting", type_.to_string(), "SONtransferRequestContainer");
    return c.get<ho_report_s>();
  }
  const cell_activation_request_s& eutran_cell_activation() const
  {
    assert_choice_type("eutranCellActivation", type_.to_string(), "SONtransferRequestContainer");
    return c.get<cell_activation_request_s>();
  }
  const cell_state_ind_s& energy_savings_ind() const
  {
    assert_choice_type("energySavingsIndication", type_.to_string(), "SONtransferRequestContainer");
    return c.get<cell_state_ind_s>();
  }
  const fail_event_report_c& fail_event_report() const
  {
    assert_choice_type("failureEventReporting", type_.to_string(), "SONtransferRequestContainer");
    return c.get<fail_event_report_c>();
  }
  multi_cell_load_report_request_s& set_multi_cell_load_report()
  {
    set(types::multi_cell_load_report);
    return c.get<multi_cell_load_report_request_s>();
  }
  event_triggered_cell_load_report_request_s& set_event_triggered_cell_load_report()
  {
    set(types::event_triggered_cell_load_report);
    return c.get<event_triggered_cell_load_report_request_s>();
  }
  ho_report_s& set_horeport()
  {
    set(types::horeport);
    return c.get<ho_report_s>();
  }
  cell_activation_request_s& set_eutran_cell_activation()
  {
    set(types::eutran_cell_activation);
    return c.get<cell_activation_request_s>();
  }
  cell_state_ind_s& set_energy_savings_ind()
  {
    set(types::energy_savings_ind);
    return c.get<cell_state_ind_s>();
  }
  fail_event_report_c& set_fail_event_report()
  {
    set(types::fail_event_report);
    return c.get<fail_event_report_c>();
  }

private:
  types type_;
  choice_buffer_t<cell_activation_request_s,
                  cell_state_ind_s,
                  event_triggered_cell_load_report_request_s,
                  fail_event_report_c,
                  ho_report_s,
                  multi_cell_load_report_request_s>
      c;

  void destroy_();
};

// SONtransferResponseContainer ::= CHOICE
struct so_ntransfer_resp_container_c {
  struct types_opts {
    enum options {
      cell_load_report,
      // ...
      multi_cell_load_report,
      event_triggered_cell_load_report,
      horeport,
      eutran_cell_activation,
      energy_savings_ind,
      fail_event_report,
      nulltype
    } value;

    std::string to_string() const;
  };
  typedef enumerated<types_opts, true, 6> types;

  // choice methods
  so_ntransfer_resp_container_c() = default;
  so_ntransfer_resp_container_c(const so_ntransfer_resp_container_c& other);
  so_ntransfer_resp_container_c& operator=(const so_ntransfer_resp_container_c& other);
  ~so_ntransfer_resp_container_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_load_report_resp_c& cell_load_report()
  {
    assert_choice_type("cellLoadReporting", type_.to_string(), "SONtransferResponseContainer");
    return c.get<cell_load_report_resp_c>();
  }
  multi_cell_load_report_resp_l& multi_cell_load_report()
  {
    assert_choice_type("multiCellLoadReporting", type_.to_string(), "SONtransferResponseContainer");
    return c.get<multi_cell_load_report_resp_l>();
  }
  event_triggered_cell_load_report_resp_s& event_triggered_cell_load_report()
  {
    assert_choice_type("eventTriggeredCellLoadReporting", type_.to_string(), "SONtransferResponseContainer");
    return c.get<event_triggered_cell_load_report_resp_s>();
  }
  cell_activation_resp_s& eutran_cell_activation()
  {
    assert_choice_type("eutranCellActivation", type_.to_string(), "SONtransferResponseContainer");
    return c.get<cell_activation_resp_s>();
  }
  const cell_load_report_resp_c& cell_load_report() const
  {
    assert_choice_type("cellLoadReporting", type_.to_string(), "SONtransferResponseContainer");
    return c.get<cell_load_report_resp_c>();
  }
  const multi_cell_load_report_resp_l& multi_cell_load_report() const
  {
    assert_choice_type("multiCellLoadReporting", type_.to_string(), "SONtransferResponseContainer");
    return c.get<multi_cell_load_report_resp_l>();
  }
  const event_triggered_cell_load_report_resp_s& event_triggered_cell_load_report() const
  {
    assert_choice_type("eventTriggeredCellLoadReporting", type_.to_string(), "SONtransferResponseContainer");
    return c.get<event_triggered_cell_load_report_resp_s>();
  }
  const cell_activation_resp_s& eutran_cell_activation() const
  {
    assert_choice_type("eutranCellActivation", type_.to_string(), "SONtransferResponseContainer");
    return c.get<cell_activation_resp_s>();
  }
  cell_load_report_resp_c& set_cell_load_report()
  {
    set(types::cell_load_report);
    return c.get<cell_load_report_resp_c>();
  }
  multi_cell_load_report_resp_l& set_multi_cell_load_report()
  {
    set(types::multi_cell_load_report);
    return c.get<multi_cell_load_report_resp_l>();
  }
  event_triggered_cell_load_report_resp_s& set_event_triggered_cell_load_report()
  {
    set(types::event_triggered_cell_load_report);
    return c.get<event_triggered_cell_load_report_resp_s>();
  }
  cell_activation_resp_s& set_eutran_cell_activation()
  {
    set(types::eutran_cell_activation);
    return c.get<cell_activation_resp_s>();
  }

private:
  types type_;
  choice_buffer_t<cell_activation_resp_s,
                  cell_load_report_resp_c,
                  event_triggered_cell_load_report_resp_s,
                  multi_cell_load_report_resp_l>
      c;

  void destroy_();
};

// SourceeNB-ToTargeteNB-TransparentContainer-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
struct sourceenb_to_targetenb_transparent_container_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { mob_info, ue_history_info_from_the_ue, nulltype } value;

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
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<32, false, true>&       mob_info();
    unbounded_octstring<true>&              ue_history_info_from_the_ue();
    const fixed_bitstring<32, false, true>& mob_info() const;
    const unbounded_octstring<true>&        ue_history_info_from_the_ue() const;

  private:
    types                                                                         type_;
    choice_buffer_t<fixed_bitstring<32, false, true>, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-HistoryInformation ::= SEQUENCE (SIZE (1..16)) OF LastVisitedCell-Item
using ue_history_info_l = dyn_array<last_visited_cell_item_c>;

struct sourceenb_to_targetenb_transparent_container_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                          mob_info_present                    = false;
  bool                                          ue_history_info_from_the_ue_present = false;
  ie_field_s<fixed_bitstring<32, false, true> > mob_info;
  ie_field_s<unbounded_octstring<true> >        ue_history_info_from_the_ue;

  // sequence methods
  sourceenb_to_targetenb_transparent_container_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SourceeNB-ToTargeteNB-TransparentContainer ::= SEQUENCE
struct sourceenb_to_targetenb_transparent_container_s {
  bool                                                           ext                                  = false;
  bool                                                           erab_info_list_present               = false;
  bool                                                           subscriber_profile_idfor_rfp_present = false;
  bool                                                           ie_exts_present                      = false;
  unbounded_octstring<true>                                      rrc_container;
  erab_info_list_l                                               erab_info_list;
  eutran_cgi_s                                                   target_cell_id;
  uint16_t                                                       subscriber_profile_idfor_rfp = 1;
  ue_history_info_l                                              ue_history_info;
  sourceenb_to_targetenb_transparent_container_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargeteNB-ToSourceeNB-TransparentContainer-ExtIEs ::= OBJECT SET OF S1AP-PROTOCOL-EXTENSION
typedef s1ap_protocol_ext_empty_o targetenb_to_sourceenb_transparent_container_ext_ies_o;

typedef protocol_ext_container_empty_l targetenb_to_sourceenb_transparent_container_ext_ies_container;

// TargeteNB-ToSourceeNB-TransparentContainer ::= SEQUENCE
struct targetenb_to_sourceenb_transparent_container_s {
  bool                                                           ext             = false;
  bool                                                           ie_exts_present = false;
  unbounded_octstring<true>                                      rrc_container;
  targetenb_to_sourceenb_transparent_container_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace s1ap
} // namespace asn1

#endif // SRSASN1_S1AP_H
