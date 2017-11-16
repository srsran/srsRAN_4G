/* \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


/****************************************************************
 *
 * GTP-C IE Types
 * Ref: TS 29.274 v10.14.0 Table 8.1-1
 *
 ****************************************************************/

const uint8_t GTPC_IE_TYPE_RESERVED = 0;
const uint8_t GTPC_IE_TYPE_IMSI = 1;
const uint8_t GTPC_IE_TYPE_CAUSE = 2;
const uint8_t GTPC_IE_TYPE_RECOVERY = 3;
//4 to 50 RESERVED_FOR_S101_INTERFACE
const uint8_t GTPC_IE_TYPE_STN_SR = 51
//52 to 70 RESERVED_FOR_SV_INTERFACE
const uint8_t GTPC_IE_TYPE_APN = 71;
const uint8_t GTPC_IE_TYPE_AMBR = 72;
const uint8_t GTPC_IE_TYPE_EBI = 73;
const uint8_t GTPC_IE_TYPE_IP_ADDRESS = 74;
const uint8_t GTPC_IE_TYPE_MEI = 75;
const uint8_t GTPC_IE_TYPE_MSISDN = 76;
const uint8_t GTPC_IE_TYPE_INDICATION = 77;
const uint8_t GTPC_IE_TYPE_PCO = 78;
const uint8_t GTPC_IE_TYPE_PDN_ADDRESS_ALLOCATION = 79;
const uint8_t GTPC_IE_TYPE_BEARER_QOS = 80;
const uint8_t GTPC_IE_TYPE_FLOW_QOS = 81;
const uint8_t GTPC_IE_TYPE_RAT_TYPE = 82;
const uint8_t GTPC_IE_TYPE_SERVING_NETWORK = 83;
const uint8_t GTPC_IE_TYPE_BEARER_TFT = 84;
const uint8_t GTPC_IE_TYPE_TAD = 85;
const uint8_t GTPC_IE_TYPE_ULI = 86;
const uint8_t GTPC_IE_TYPE_F_TEID = 87;
const uint8_t GTPC_IE_TYPE_TMSI = 88;
const uint8_t GTPC_IE_TYPE_GLOBAL_CN_ID = 89;
const uint8_t GTPC_IE_TYPE_S103_PDN_DATA_FORWARDING_INFO = 90;
const uint8_t GTPC_IE_TYPE_S1_U_DATA_FORWARDING_INFO = 91;
const uint8_t GTPC_IE_TYPE_DELAY_VALUE = 92;
const uint8_t GTPC_IE_TYPE_BEARER_CONTEXT = 93;
const uint8_t GTPC_IE_TYPE_CHARGING_ID = 94;
const uint8_t GTPC_IE_TYPE_CHARGING_CHARACTERISTICS = 95;
const uint8_t GTPC_IE_TYPE_TRACE_INFORMATION = 96;
const uint8_t GTPC_IE_TYPE_BEARER_FLAGS = 97;
const uint8_t GTPC_IE_TYPE_RESERVED = 98;
const uint8_t GTPC_IE_TYPE_PDN_TYPE = 99;
const uint8_t GTPC_IE_TYPE_PROCEDURE_TRANSACTION_ID = 100;
const uint8_t GTPC_IE_TYPE_DRX_PARAMETER = 101;
const uint8_t GTPC_IE_TYPE_RESERVED = 102;
const uint8_t GTPC_IE_TYPE_MM_CONTEXT_GSM_KEY_AND_TRIPLETS = 103;
const uint8_t GTPC_IE_TYPE_MM_CONTEXT_UMTS_KEY_USED_CIPHER_AND_QUINTUPLETS = 104;
const uint8_t GTPC_IE_TYPE_MM_CONTEXT_GSM_KEY_USED_CIPHER_AND_QUINTUPLETS = 105;
const uint8_t GTPC_IE_TYPE_MM_CONTEXT_UMTS_KEY_AND_QUINTUPLETS = 106;
const uint8_t GTPC_IE_TYPE_MM_CONTEXT_EPS_SECURITY_CONTEXT_QUADRUPLETS_AND_QUINTUPLETS = 107;
const uint8_t GTPC_IE_TYPE_MM_CONTEXT_UMTS_KEY_QUADRUPLETS_AND_QUINTUPLETS = 108;
const uint8_t GTPC_IE_TYPE_PDN_CONNECTION = 109;
const uint8_t GTPC_IE_TYPE_PDU_NUMBERS = 110;
const uint8_t GTPC_IE_TYPE_P_TMSI = 111;
const uint8_t GTPC_IE_TYPE_P_TMSI_SIGNATURE = 112;
const uint8_t GTPC_IE_TYPE_HOP_COUNTER = 113;
const uint8_t GTPC_IE_TYPE_UE_TIME_ZONE = 114
const uint8_t GTPC_IE_TYPE_TRACE_REFERENCE = 115;
const uint8_t GTPC_IE_TYPE_COMPLETE_REQUEST_MESSAGE = 116;
const uint8_t GTPC_IE_TYPE_GUTI = 117;
const uint8_t GTPC_IE_TYPE_F_CONTAINER = 118;
const uint8_t GTPC_IE_TYPE_F_CAUSE = 119;
const uint8_t GTPC_IE_TYPE_SELECTED_PLMN_ID = 120;
const uint8_t GTPC_IE_TYPE_TARGET_IDENTIFICATION = 121;
const uint8_t GTPC_IE_TYPE_RESERVED = 122;
const uint8_t GTPC_IE_TYPE_PACKET_FLOW_ID = 123;
const uint8_t GTPC_IE_TYPE_RAB_CONTEXT = 124;
const uint8_t GTPC_IE_TYPE_SOURCE_RNC_PDCP_CONTEXT_INFO = 125;
const uint8_t GTPC_IE_TYPE_UDP_SOURCE_PORT_NUMBER = 126;
const uint8_t GTPC_IE_TYPE_APN_RESTRICTION = 127;
const uint8_t GTPC_IE_TYPE_SELECTION_MODE = 128;
const uint8_t GTPC_IE_TYPE_SOURCE_IDENTIFICATION = 129;
//130 RESERVED
const uint8_t GTPC_IE_TYPE_CHANGE_REPORTING_ACTION = 131;
const uint8_t GTPC_IE_TYPE_FQ_CSID = 132;
const uint8_t GTPC_IE_TYPE_CHANNEL_NEEDED = 133;
const uint8_t GTPC_IE_TYPE_EMLPP_PRIORITY = 134;
const uint8_t GTPC_IE_TYPE_NODE_TYPE = 135;
const uint8_t GTPC_IE_TYPE_FQDN = 136;
const uint8_t GTPC_IE_TYPE_TI = 137;
const uint8_t GTPC_IE_TYPE_MBMS_SESSION_DURATION = 138;
const uint8_t GTPC_IE_TYPE_MBMS_SERVICE_AREA = 139;
const uint8_t GTPC_IE_TYPE_MBMS_SESSION_IDENTIFIER = 140;
const uint8_t GTPC_IE_TYPE_MBMS_FLOW_IDENTIFIER = 141;
const uint8_t GTPC_IE_TYPE_MBMS_IP_MULTICAST_DISTRIBUTION = 142;
const uint8_t GTPC_IE_TYPE_MBMS_DISTRIBUTION_ACKNOWLEDGE = 143;
const uint8_t GTPC_IE_TYPE_RFSP_INDEX = 144;
const uint8_t GTPC_IE_TYPE_UCI = 145;
const uint8_t GTPC_IE_TYPE_CSG_INFORMATION_REPORTING_ACTION = 146;
const uint8_t GTPC_IE_TYPE_CSG_ID = 147;
const uint8_t GTPC_IE_TYPE_CMI = 148;
const uint8_t GTPC_IE_TYPE_SERVICE_INDICATOR = 149;
const uint8_t GTPC_IE_TYPE_DETACH_TYPE = 150;
const uint8_t GTPC_IE_TYPE_LDN = 151;
const uint8_t GTPC_IE_TYPE_NODE_FEATURES = 152;
const uint8_t GTPC_IE_TYPE_MBMS_TIME_TO_DATA_TRANSFER = 153;
const uint8_t GTPC_IE_TYPE_THROTTLING =154;
const uint8_t GTPC_IE_TYPE_ARP = 155;
const uint8_t GTPC_IE_TYPE_EPC_TIMER = 156;
const uint8_t GTPC_IE_TYPE_SIGNALLING_PRIORITY_INDICATION = 157;
const uint8_t GTPC_IE_TYPE_TMGI = 158;
const uint8_t GTPC_IE_TYPE_ADDITIONAL_MM_CONTEXT_FOR_SRVCC = 159;
const uint8_t GTPC_IE_TYPE_ADDITIONAL_FLAGS_FOR_SRVCC = 160;
//161 RESERVED 
const uint8_t GTPC_IE_TYPE_MDT_CONFIGURATION = 162;
const uint8_t GTPC_IE_TYPE_APCO = 163;
//164 RESERVED
const uint8_t GTPC_IE_TYPE_CHANGE_TO_REPORT_FLAGS = 165;
//168 TO 254 SPARE. FOR FUTURE USE.
const uint8_t GTPC_IE_TYPE_PRIVATE_EXTENSION = 255;
