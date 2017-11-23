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

#ifndef GTPC_IES_H
#define GTPC_IES_H


#include "srslte/phy/io/netsource.h"

/****************************************************************
 *
 * GTP-C IE Types
 * Ref: TS 29.274 v10.14.0 Table 8.1-1
 *
 ****************************************************************/
enum gtpc_ie_type
{
//const uint8_t GTPC_IE_TYPE_RESERVED = 0;
  GTPC_IE_TYPE_IMSI = 1,
  GTPC_IE_TYPE_CAUSE = 2,
  GTPC_IE_TYPE_RECOVERY = 3,
//4 to 50 RESERVED_FOR_S101_INTERFACE
  GTPC_IE_TYPE_STN_SR = 51,
//52 to 70 RESERVED_FOR_SV_INTERFACE
  GTPC_IE_TYPE_APN = 71,
  GTPC_IE_TYPE_AMBR = 72,
  GTPC_IE_TYPE_EBI = 73,
  GTPC_IE_TYPE_IP_ADDRESS = 74,
  GTPC_IE_TYPE_MEI = 75,
  GTPC_IE_TYPE_MSISDN = 76,
  GTPC_IE_TYPE_INDICATION = 77,
  GTPC_IE_TYPE_PCO = 78,
  GTPC_IE_TYPE_PDN_ADDRESS_ALLOCATION = 79,
  GTPC_IE_TYPE_BEARER_QOS = 80,
  GTPC_IE_TYPE_FLOW_QOS = 81,
  GTPC_IE_TYPE_RAT_TYPE = 82,
  GTPC_IE_TYPE_SERVING_NETWORK = 83,
  GTPC_IE_TYPE_BEARER_TFT = 84,
  GTPC_IE_TYPE_TAD = 85,
  GTPC_IE_TYPE_ULI = 86,
  GTPC_IE_TYPE_F_TEID = 87,
  GTPC_IE_TYPE_TMSI = 88,
  GTPC_IE_TYPE_GLOBAL_CN_ID = 89,
  GTPC_IE_TYPE_S103_PDN_DATA_FORWARDING_INFO = 90,
  GTPC_IE_TYPE_S1_U_DATA_FORWARDING_INFO = 91,
  GTPC_IE_TYPE_DELAY_VALUE = 92,
  GTPC_IE_TYPE_BEARER_CONTEXT = 93,
  GTPC_IE_TYPE_CHARGING_ID = 94,
  GTPC_IE_TYPE_CHARGING_CHARACTERISTICS = 95,
  GTPC_IE_TYPE_TRACE_INFORMATION = 96,
  GTPC_IE_TYPE_BEARER_FLAGS = 97,
//98 Reserved
  GTPC_IE_TYPE_PDN_TYPE = 99,
  GTPC_IE_TYPE_PROCEDURE_TRANSACTION_ID = 100,
  GTPC_IE_TYPE_DRX_PARAMETER = 101,
//102 Reserved
  GTPC_IE_TYPE_MM_CONTEXT_GSM_KEY_AND_TRIPLETS = 103,
  GTPC_IE_TYPE_MM_CONTEXT_UMTS_KEY_USED_CIPHER_AND_QUINTUPLETS = 104,
  GTPC_IE_TYPE_MM_CONTEXT_GSM_KEY_USED_CIPHER_AND_QUINTUPLETS = 105,
  GTPC_IE_TYPE_MM_CONTEXT_UMTS_KEY_AND_QUINTUPLETS = 106,
  GTPC_IE_TYPE_MM_CONTEXT_EPS_SECURITY_CONTEXT_QUADRUPLETS_AND_QUINTUPLETS = 107,
  GTPC_IE_TYPE_MM_CONTEXT_UMTS_KEY_QUADRUPLETS_AND_QUINTUPLETS = 108,
  GTPC_IE_TYPE_PDN_CONNECTION = 109,
  GTPC_IE_TYPE_PDU_NUMBERS = 110,
  GTPC_IE_TYPE_P_TMSI = 111,
  GTPC_IE_TYPE_P_TMSI_SIGNATURE = 112,
  GTPC_IE_TYPE_HOP_COUNTER = 113,
  GTPC_IE_TYPE_UE_TIME_ZONE = 114,
  GTPC_IE_TYPE_TRACE_REFERENCE = 115,
  GTPC_IE_TYPE_COMPLETE_REQUEST_MESSAGE = 116,
  GTPC_IE_TYPE_GUTI = 117,
  GTPC_IE_TYPE_F_CONTAINER = 118,
  GTPC_IE_TYPE_F_CAUSE = 119,
  GTPC_IE_TYPE_SELECTED_PLMN_ID = 120,
  GTPC_IE_TYPE_TARGET_IDENTIFICATION = 121,
//122 Reserved
  GTPC_IE_TYPE_PACKET_FLOW_ID = 123,
  GTPC_IE_TYPE_RAB_CONTEXT = 124,
  GTPC_IE_TYPE_SOURCE_RNC_PDCP_CONTEXT_INFO = 125,
  GTPC_IE_TYPE_UDP_SOURCE_PORT_NUMBER = 126,
  GTPC_IE_TYPE_APN_RESTRICTION = 127,
  GTPC_IE_TYPE_SELECTION_MODE = 128,
  GTPC_IE_TYPE_SOURCE_IDENTIFICATION = 129,
//130 RESERVED
  GTPC_IE_TYPE_CHANGE_REPORTING_ACTION = 131,
  GTPC_IE_TYPE_FQ_CSID = 132,
  GTPC_IE_TYPE_CHANNEL_NEEDED = 133,
  GTPC_IE_TYPE_EMLPP_PRIORITY = 134,
  GTPC_IE_TYPE_NODE_TYPE = 135,
  GTPC_IE_TYPE_FQDN = 136,
  GTPC_IE_TYPE_TI = 137,
  GTPC_IE_TYPE_MBMS_SESSION_DURATION = 138,
  GTPC_IE_TYPE_MBMS_SERVICE_AREA = 139,
  GTPC_IE_TYPE_MBMS_SESSION_IDENTIFIER = 140,
  GTPC_IE_TYPE_MBMS_FLOW_IDENTIFIER = 141,
  GTPC_IE_TYPE_MBMS_IP_MULTICAST_DISTRIBUTION = 142,
  GTPC_IE_TYPE_MBMS_DISTRIBUTION_ACKNOWLEDGE = 143,
  GTPC_IE_TYPE_RFSP_INDEX = 144,
  GTPC_IE_TYPE_UCI = 145,
  GTPC_IE_TYPE_CSG_INFORMATION_REPORTING_ACTION = 146,
  GTPC_IE_TYPE_CSG_ID = 147,
  GTPC_IE_TYPE_CMI = 148,
  GTPC_IE_TYPE_SERVICE_INDICATOR = 149,
  GTPC_IE_TYPE_DETACH_TYPE = 150,
  GTPC_IE_TYPE_LDN = 151,
  GTPC_IE_TYPE_NODE_FEATURES = 152,
  GTPC_IE_TYPE_MBMS_TIME_TO_DATA_TRANSFER = 153,
  GTPC_IE_TYPE_THROTTLING =154,
  GTPC_IE_TYPE_ARP = 155,
  GTPC_IE_TYPE_EPC_TIMER = 156,
  GTPC_IE_TYPE_SIGNALLING_PRIORITY_INDICATION = 157,
  GTPC_IE_TYPE_TMGI = 158,
  GTPC_IE_TYPE_ADDITIONAL_MM_CONTEXT_FOR_SRVCC = 159,
  GTPC_IE_TYPE_ADDITIONAL_FLAGS_FOR_SRVCC = 160,
//161 RESERVED
  GTPC_IE_TYPE_MDT_CONFIGURATION = 162,
  GTPC_IE_TYPE_APCO = 163,
//164 RESERVED
  GTPC_IE_TYPE_CHANGE_TO_REPORT_FLAGS = 165,
//168 TO 254 SPARE. FOR FUTURE USE.
  GTPC_IE_TYPE_PRIVATE_EXTENSION = 255
};

/****************************************************************
 *
 * GTP-C IMSI IE
 * Ref: TS 29.274 v10.14.0 Figure 8.3-1
 *
 ****************************************************************/
/*
 * The IMSI should be kept as an uint64_t.
 * The responsibility to convert from uint64_t to BCD coded is on 
 * the pack_imsi_ie function
 */

/****************************************************************************
 *
 * GTP-C Cause IE
 * Ref: 3GPP TS 29.274 v10.14.0 Figure 8.4-1 and Table 8.4-1
 *
 ***************************************************************************/
enum gtpc_cause_value
{
  //Reserved
  LOCAL_DETACH = 2,
  COMPLETE_DETACH = 3,
  RAT_CHANGED_FROM_3GPP_TO_NON_3GPP = 4,
  ISR_DEACTIVATION = 5,
  ERROR_INDICATION_RECEIVED_FROM_RNC_ENODEB_S4_SGSN = 6,
  IMSI_DETACH_ONLY = 7,
  REACTIVATION_REQUESTED = 8,
  PDN_RECONNECTION_TO_THIS_APN_DISALLOWED = 9,
  ACCESS_CHANGED_FROM_NON_3GPP_TO_3GPP = 10,
  PDN_CONNECTION_INACTIVITY_TIMER_EXPIRES = 11
  //Spare. This value range shall be used by Cause values in an initial/request message.

};

struct gtpc_cause_ie
{
  enum gtpc_cause_value cause_value;
  bool pce;
  bool bce;
  bool cs;
  enum gtpc_ie_type offending_ie_type;
  uint16_t length_of_offending_ie;
  uint8_t offending_ie_instance;
};

/****************************************************************************
 *
 * GTP-C Recovery IE
 * Ref: 3GPP TS 29.274 v10.14.0 Figure 8.5-1
 *
 ***************************************************************************/
/*
 * The Recovery (Restart Counter) IE should be kept as an uint8_t.
 */

/****************************************************************************
 *
 * GTP-C Access Point Name IE
 * Ref: 3GPP TS 29.274 v10.14.0 Figure 8.6-1
 *
 ***************************************************************************/
/*
 * APN IE should be kept as an null terminated string.
 * This string will be kept in a char[MAX_APN_LENGTH] buffer.
 */
#define MAX_APN_LENGTH 1024

/****************************************************************************
 *
 * GTP-C Aggregate Maximum bit-rate IE
 * Ref: 3GPP TS 29.274 v10.14.0 Table 8.7-1
 *
 ***************************************************************************/
struct gtpc_ambr_ie
{
  uint32_t apn_ambr_uplink;
  uint32_t apn_ambr_downlink;
};

/****************************************************************************
 *
 * GTP-C EPS Bearer ID address IE
 * Ref: 3GPP TS 29.274 v10.14.0 Figure 8.8-1
 *
 ***************************************************************************/
/*
 * The EPS Bearer ID (EBI) IE should be kept as an uint8_t.
 */

/****************************************************************************
 *
 * GTP-C IP address IE
 * Ref: 3GPP TS 29.274 v10.14.0 Figure 8.9-1
 *
 ***************************************************************************/
/*
 * IP addresse IEs should the sockaddr_storage struct, which can hold IPv4
 * and IPv6 addresses.
 */

//TODO
//TODO IEs between 8.10 and 8.17 missing
//TODO

/****************************************************************************
 *
 * GTP-C RAT Type IE
 * Ref: 3GPP TS 29.274 v10.14.0 Figure 8.17-1
 *
 ***************************************************************************/

enum gtpc_rat_type
{
  UTRAN = 1,
  GERAN,
  WLAN,
  GAN,
  HSPA_EVOLUTION,
  EUTRAN,
  Virtual
};

//TODO
//TODO IEs between 8.17 and 8.22 missing
//TODO

/****************************************************************************
 *
 * GTP-C Fully Qualified Tunnel End-point Identifier (F-TEID) IE
 * Ref: 3GPP TS 29.274 v10.14.0 Figure 8.22-1
 *
 ***************************************************************************/
enum gtpc_interface_type
{
  S1_U_ENODEB_GTP_U_INTERFACE,
  S1_U_SGW_GTP_U_INTERFACE,
  S12_RNC_GTP_U_INTERFACE,
  S12_SGW_GTP_U_INTERFACE,
  S5_S8_SGW_GTP_U_INTERFACE,
  S5_S8_PGW_GTP_U_INTERFACE,
  S5_S8_SGW_GTP_C_INTERFACE,
  S5_S8_PGW_GTP_C_INTERFACE,
  S5_S8_SGW_PMIPV6_INTERFACE, //(the 32 bit GRE key is encoded in 32 bit TEID field and since alternate CoA is not used the control plane and user plane addresses are the same for PMIPv6)
  S5_S8_PGW_PMIPV6_INTERFACE, //(the 32 bit GRE key is encoded in 32 bit TEID field and the control plane and user plane addresses are the same for PMIPv6)
  S11_MME_GTP_C_INTERFACE,
  S11_S4_SGW_GTP_C_INTERFACE,
  S10_MME_GTP_C_INTERFACE,
  S3_MME_GTP_C_INTERFACE,
  S3_SGSN_GTP_C_INTERFACE,
  S4_SGSN_GTP_U_INTERFACE,
  S4_SGW_GTP_U_INTERFACE,
  S4_SGSN_GTP_C_INTERFACE,
  S16_SGSN_GTP_C_INTERFACE,
  ENODEB_GTP_U_INTERFACE_FOR_DL_DATA_FORWARDING,
  ENODEB_GTP_U_INTERFACE_FOR_UL_DATA_FORWARDING,
  RNC_GTP_U_INTERFACE_FOR_DATA_FORWARDING,
  SGSN_GTP_U_INTERFACE_FOR_DATA_FORWARDING,
  SGW_GTP_U_INTERFACE_FOR_DL_DATA_FORWARDING,
  SM_MBMS_GW_GTP_C_INTERFACE,
  SN_MBMS_GW_GTP_C_INTERFACE,
  SM_MME_GTP_C_INTERFACE,
  SN_SGSN_GTP_C_INTERFACE,
  SGW_GTP_U_INTERFACE_FOR_UL_DATA_FORWARDING,
  SN_SGSN_GTP_U_INTERFACE,
  S2B_EPDG_GTP_C_INTERFACE,
  S2B_U_EPDG_GTP_U_INTERFACE,
  S2B_PGW_GTP_C_INTERFACE,
  S2B_U_PGW_GTP_U_INTERFACE
};

struct gtpc_f_teid_ie
{
  bool v4_present;
  bool v6_present;
  enum gtpc_interface_type interface_type;
  uint32_t teid;
  in_addr_t  ipv4;
  struct in6_addr ipv6; //FIXME
};

//TODO
//TODO IEs between 8.22 and 8.28 missing
//TODO

/****************************************************************************
 *
 * GTP-C Bearer Context IE
 * Ref: 3GPP TS 29.274 v10.14.0 Table 8.28-1
 *
 ***************************************************************************/
//The usage of this grouped IE is specific to the GTP-C message being sent.
//As such, each GTP-C message will define it's bearer context structures
//locally, according to the rules of  TS 29.274 v10.14.0 Section 7.

#endif //GTPC_IES_H
