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
#ifndef SRSLTE_LIBLTE_M2AP_H
#define SRSLTE_LIBLTE_M2AP_H

/*******************************************************************************
 * Warnings/Todos
 *******************************************************************************/
// Extensions are not yet handled correctly

/*******************************************************************************
 * INCLUDES
 *******************************************************************************/
#include <srslte/asn1/liblte_common.h>

/*******************************************************************************
                              LOGGING
*******************************************************************************/
typedef void (*log_handler_t)(void* ctx, char* str);
void liblte_log_register_handler(void* ctx, log_handler_t handler);

/*******************************************************************************
 * MAX defines
 *******************************************************************************/
#define LIBLTE_M2AP_MAXPRIVATEIES 256
#define LIBLTE_M2AP_MAXNOOFMBSFNAREAS 8
#define LIBLTE_M2AP_MAXNOOFMBSFN_ALLOCATIONS 15
#define LIBLTE_M2AP_MAXNOOFPMCHSPERMBSFNAREA 256
#define LIBLTE_M2AP_MAXNOOFCELLS 256
#define LIBLTE_M2AP_MAXNOOFMBMSSERVICEAREASPERCELL 256
#define LIBLTE_M2AP_MAXNOOFSESSIONSPERPMCH 29
#define LIBLTE_M2AP_MAXNOOFERRORS 256
#define LIBLTE_M2AP_MAXNROFINDIVIDUALM2CONNECTIONSTORESET 256
#define LIBLTE_M2AP_MAXNOOFCOUNTINGSERVICE 16

/*******************************************************************************
 * Elementary Procedures
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_PROC_ID_SESSIONSTART                     = 0,
  LIBLTE_M2AP_PROC_ID_SESSIONSTOP                      = 1,
  LIBLTE_M2AP_PROC_ID_MBMSSCHEDULINGINFORMATION        = 2,
  LIBLTE_M2AP_PROC_ID_ERRORINDICATION                  = 3,
  LIBLTE_M2AP_PROC_ID_RESET                            = 4,
  LIBLTE_M2AP_PROC_ID_M2SETUP                          = 5,
  LIBLTE_M2AP_PROC_ID_ENBCONFIGURATIONUPDATE           = 6,
  LIBLTE_M2AP_PROC_ID_MCECONFIGURATIONUPDATE           = 7,
  LIBLTE_M2AP_PROC_ID_PRIVATEMESSAGE                   = 8,
  LIBLTE_M2AP_PROC_ID_SESSIONUPDATE                    = 9,
  LIBLTE_M2AP_PROC_ID_MBMSSERVICECOUNTING              = 10,
  LIBLTE_M2AP_PROC_ID_MBMSSERVICECOUNTINGRESULTSREPORT = 11,
  LIBLTE_M2AP_PROC_N_ITEMS,
} LIBLTE_M2AP_PROC_ENUM;
static const char liblte_m2ap_proc_text[LIBLTE_M2AP_PROC_N_ITEMS][64] = {"id-sessionStart",
                                                                         "id-sessionStop",
                                                                         "id-mbmsSchedulingInformation",
                                                                         "id-errorIndication",
                                                                         "id-reset",
                                                                         "id-m2Setup",
                                                                         "id-eNBConfigurationUpdate",
                                                                         "id-mCEConfigurationUpdate",
                                                                         "id-privateMessage",
                                                                         "id-sessionUpdate",
                                                                         "id-mbmsServiceCounting",
                                                                         "id-mbmsServiceCountingResultsReport"};

/*******************************************************************************
 * ProtocolIE Ids
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_IE_ID_MCE_MBMS_M2AP_ID                                      = 0,
  LIBLTE_M2AP_IE_ID_ENB_MBMS_M2AP_ID                                      = 1,
  LIBLTE_M2AP_IE_ID_TMGI                                                  = 2,
  LIBLTE_M2AP_IE_ID_MBMS_SESSION_ID                                       = 3,
  LIBLTE_M2AP_IE_ID_MBMS_SERVICE_AREA                                     = 6,
  LIBLTE_M2AP_IE_ID_TNL_INFORMATION                                       = 7,
  LIBLTE_M2AP_IE_ID_CRITICALITYDIAGNOSTICS                                = 8,
  LIBLTE_M2AP_IE_ID_CAUSE                                                 = 9,
  LIBLTE_M2AP_IE_ID_MBSFN_AREA_CONFIGURATION_LIST                         = 10,
  LIBLTE_M2AP_IE_ID_PMCH_CONFIGURATION_LIST                               = 11,
  LIBLTE_M2AP_IE_ID_PMCH_CONFIGURATION_ITEM                               = 12,
  LIBLTE_M2AP_IE_ID_GLOBALENB_ID                                          = 13,
  LIBLTE_M2AP_IE_ID_ENBNAME                                               = 14,
  LIBLTE_M2AP_IE_ID_ENB_MBMS_CONFIGURATION_DATA_LIST                      = 15,
  LIBLTE_M2AP_IE_ID_ENB_MBMS_CONFIGURATION_DATA_ITEM                      = 16,
  LIBLTE_M2AP_IE_ID_GLOBALMCE_ID                                          = 17,
  LIBLTE_M2AP_IE_ID_MCENAME                                               = 18,
  LIBLTE_M2AP_IE_ID_MCCHRELATEDBCCH_CONFIGPERMBSFNAREA                    = 19,
  LIBLTE_M2AP_IE_ID_MCCHRELATEDBCCH_CONFIGPERMBSFNAREA_ITEM               = 20,
  LIBLTE_M2AP_IE_ID_TIMETOWAIT                                            = 21,
  LIBLTE_M2AP_IE_ID_MBSFN_SUBFRAME_CONFIGURATION_LIST                     = 22,
  LIBLTE_M2AP_IE_ID_MBSFN_SUBFRAME_CONFIGURATION_ITEM                     = 23,
  LIBLTE_M2AP_IE_ID_COMMON_SUBFRAME_ALLOCATION_PERIOD                     = 24,
  LIBLTE_M2AP_IE_ID_MCCH_UPDATE_TIME                                      = 25,
  LIBLTE_M2AP_IE_ID_ENB_MBMS_CONFIGURATION_DATA_LIST_CONFIGUPDATE         = 26,
  LIBLTE_M2AP_IE_ID_ENB_MBMS_CONFIGURATION_DATA_CONFIGUPDATE_ITEM         = 27,
  LIBLTE_M2AP_IE_ID_MBMS_SERVICE_ASSOCIATEDLOGICALM2_CONNECTIONITEM       = 28,
  LIBLTE_M2AP_IE_ID_MBSFN_AREA_ID                                         = 29,
  LIBLTE_M2AP_IE_ID_RESETTYPE                                             = 30,
  LIBLTE_M2AP_IE_ID_MBMS_SERVICE_ASSOCIATEDLOGICALM2_CONNECTIONLISTRESACK = 31,
  LIBLTE_M2AP_IE_ID_MBMS_COUNTING_REQUEST_SESSION                         = 32,
  LIBLTE_M2AP_IE_ID_MBMS_COUNTING_REQUEST_SESSION_ITEM                    = 33,
  LIBLTE_M2AP_IE_ID_MBMS_COUNTING_RESULT_LIST                             = 34,
  LIBLTE_M2AP_IE_ID_MBMS_COUNTING_RESULT_ITEM                             = 35,
  LIBLTE_M2AP_IE_N_ITEMS
} LIBLTE_M2AP_IE_ENUM;
static const char liblte_m2ap_ie_text[LIBLTE_M2AP_IE_N_ITEMS][64] = {
    "id-MCE-MBMS-M2AP-ID",
    "id-ENB-MBMS-M2AP-ID",
    "id-TMGI",
    "id-MBMS-Session-ID",
    "id-MBMS-Service-Area",
    "id-TNL-Information",
    "id-CriticalityDiagnostics",
    "id-Cause",
    "id-MBSFN-Area-Configuration-List",
    "id-PMCH-Configuration-List",
    "id-PMCH-Configuration-Item",
    "id-GlobalENB-ID",
    "id-ENBname",
    "id-ENB-MBMS-Configuration-data-List",
    "id-ENB-MBMS-Configuration-data-Item",
    "id-GlobalMCE-ID",
    "id-MCEname",
    "id-MCCHrelatedBCCH-ConfigPerMBSFNArea",
    "id-MCCHrelatedBCCH-ConfigPerMBSFNArea-Item",
    "id-TimeToWait",
    "id-MBSFN-Subframe-Configuration-List",
    "id-MBSFN-Subframe-Configuration-Item",
    "id-Common-Subframe-Allocation-Period",
    "id-MCCH-Update-Time",
    "id-ENB-MBMS-Configuration-data-List-ConfigUpdate",
    "id-ENB-MBMS-Configuration-data-ConfigUpdate-Item",
    "id-MBMS-Service-associatedLogicalM2-ConnectionItem",
    "id-MBSFN-Area-ID",
    "id-ResetType",
    "id-MBMS-Service-associatedLogicalM2-ConnectionListResAck",
    "id-MBMS-Counting-Request-Session",
    "id-MBMS-Counting-Request-Session-Item",
    "id-MBMS-Counting-Result-List",
    "id-MBMS-Counting-Result-Item"};

/*******************************************************************************
 * ProtocolIE AllocatedSubFramesEnd INTEGER
 *******************************************************************************/
// lb:0;ub:1535
typedef struct {
  uint16_t allocated_subframes_end;
} LIBLTE_M2AP_ALLOCATED_SUBFRAMES_END_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_allocatedsubframesend(LIBLTE_M2AP_ALLOCATED_SUBFRAMES_END_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_allocatedsubframesend(uint8_t**                                   ptr,
                                                           LIBLTE_M2AP_ALLOCATED_SUBFRAMES_END_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE CauseMisc CHOICE
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_CAUSE_MISC_CONTROL_PROCESSING_OVERLOAD,
  LIBLTE_M2AP_CAUSE_MISC_HARDWARE_FAILURE,
  LIBLTE_M2AP_CAUSE_MISC_OM_INTERVENTION,
  LIBLTE_M2AP_CAUSE_MISC_UNSPECIFIED,
  LIBLTE_M2AP_CAUSE_MISC_N_ITEMS
} LIBLTE_M2AP_CAUSE_CHOICE_MISC_ENUM;
static const char liblte_s1ap_cause_misc_text[LIBLTE_M2AP_CAUSE_MISC_N_ITEMS][80] = {
    "control-processing-overload",
    "hardware-failure",
    "om-intervention",
    "unspecified",
};

typedef struct {
  bool                               ext;
  LIBLTE_M2AP_CAUSE_CHOICE_MISC_ENUM cause;
} LIBLTE_M2AP_CAUSE_MISC_STRUCT;

/*******************************************************************************
 * ProtocolIE CauseNas CHOICE
 *******************************************************************************/
typedef enum { LIBLTE_M2AP_CAUSE_NAS_UNSPECIFIED, LIBLTE_M2AP_CAUSE_NAS_N_ITEMS } LIBLTE_M2AP_CAUSE_CHOICE_NAS_ENUM;
static const char liblte_s1ap_cause_nas_text[LIBLTE_M2AP_CAUSE_NAS_N_ITEMS][80] = {
    "unspecified",
};

typedef struct {
  bool                              ext;
  LIBLTE_M2AP_CAUSE_CHOICE_NAS_ENUM cause;
} LIBLTE_M2AP_CAUSE_NAS_STRUCT;

/*******************************************************************************
 * ProtocolIE CauseProtocol CHOICE
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_CAUSE_PROTOCOL_TRANSFER_SYNTAX_ERROR,
  LIBLTE_M2AP_CAUSE_PROTOCOL_ABSTRACT_SYNTAX_ERROR_REJECT,
  LIBLTE_M2AP_CAUSE_PROTOCOL_ABSTRACT_SYNTAX_ERROR_IGNORE_AND_NOTIFY,
  LIBLTE_M2AP_CAUSE_PROTOCOL_MESSAGE_NOT_COMPATIBLE_WITH_RECEIVER_STATE,
  LIBLTE_M2AP_CAUSE_PROTOCOL_SEMANTIC_ERROR,
  LIBLTE_M2AP_CAUSE_PROTOCOL_ABSTRACT_SYNTAX_ERROR_FALSELY_CONSTRUCTED_MESSAGE,
  LIBLTE_M2AP_CAUSE_PROTOCOL_UNSPECIFIED,
  LIBLTE_M2AP_CAUSE_PROTOCOL_N_ITEMS
} LIBLTE_M2AP_CAUSE_CHOICE_PROTOCOL_ENUM;
static const char liblte_m2ap_cause_protocol_text[LIBLTE_M2AP_CAUSE_PROTOCOL_N_ITEMS][80] = {
    "transfer-syntax-error",
    "abstract-syntax-error-reject",
    "abstract-syntax-error-ignore-and-notify",
    "message-not-compatible-with-receiver-state",
    "semantic-error",
    "abstract-syntax-error-falsely-constructed-message",
    "unspecified",
};

typedef struct {
  bool                                   ext;
  LIBLTE_M2AP_CAUSE_CHOICE_PROTOCOL_ENUM cause;
} LIBLTE_M2AP_CAUSE_PROTOCOL_STRUCT;

/*******************************************************************************
 * ProtocolIE CauseRadioNetwork CHOICE
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_CAUSE_RADIONETWORK_UNKNOWN_OR_ALREADY_ALLOCATED_MCE_MBMS_M2AP_ID,
  LIBLTE_M2AP_CAUSE_RADIONETWORK_UNKNOWN_OR_ALREADY_ALLOCATED_ENB_MBMS_M2AP_ID,
  LIBLTE_M2AP_CAUSE_RADIONETWORK_UNKNOWN_OR_INCONSISTENT_PAIR_OF_MBMS_M2AP_IDS,
  LIBLTE_M2AP_CAUSE_RADIONETWORK_RADIO_RESOURCES_NOT_AVAILABLE,
  LIBLTE_M2AP_CAUSE_RADIONETWORK_INTERACTION_WITH_OTHER_PROCEDURE,
  LIBLTE_M2AP_CAUSE_RADIONETWORK_UNSPECIFIED,
  LIBLTE_M2AP_CAUSE_RADIONETWORK_N_ITEMS
} LIBLTE_M2AP_CAUSE_CHOICE_RADIONETWORK_ENUM;
static const char liblte_m2ap_cause_radionetwork_text[LIBLTE_M2AP_CAUSE_RADIONETWORK_N_ITEMS][80] = {
    "unknown-or-already-allocated-MCE-MBMS-M2AP-ID",
    "unknown-or-already-allocated-eNB-MBMS-M2AP-ID",
    "unknown-or-inconsistent-pair-of-MBMS-M2AP-IDs",
    "radio-resources-not-available",
    "interaction-with-other-procedure",
    "unspecified",
};

typedef struct {
  bool                                       ext;
  LIBLTE_M2AP_CAUSE_CHOICE_RADIONETWORK_ENUM cause;
} LIBLTE_M2AP_CAUSE_RADIONETWORK_STRUCT;

/*******************************************************************************
 * ProtocolIE CauseTransport CHOICE
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_CAUSE_TRANSPORT_TRANSPORT_RESOURCE_UNAVAILABLE,
  LIBLTE_M2AP_CAUSE_TRANSPORT_UNSPECIFIED,
  LIBLTE_M2AP_CAUSE_TRANSPORT_N_ITEMS
} LIBLTE_M2AP_CAUSE_CHOICE_TRANSPORT_ENUM;
static const char liblte_m2ap_cause_transport_text[LIBLTE_M2AP_CAUSE_TRANSPORT_N_ITEMS][80] = {
    "transport-resource-unavailable",
    "unspecified",
};

typedef struct {
  bool                                    ext;
  LIBLTE_M2AP_CAUSE_CHOICE_TRANSPORT_ENUM cause;
} LIBLTE_M2AP_CAUSE_TRANSPORT_STRUCT;

/*******************************************************************************
 * ProtocolIE Cause CHOICE
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_CAUSE_CHOICE_RADIONETWORK,
  LIBLTE_M2AP_CAUSE_CHOICE_TRANSPORT,
  LIBLTE_M2AP_CAUSE_CHOICE_NAS,
  LIBLTE_M2AP_CAUSE_CHOICE_PROTOCOL,
  LIBLTE_M2AP_CAUSE_CHOICE_MISC,
  LIBLTE_M2AP_CAUSE_CHOICE_N_ITEMS,
} LIBLTE_M2AP_CAUSE_CHOICE_ENUM;
static const char liblte_m2ap_cause_choice_text[LIBLTE_M2AP_CAUSE_CHOICE_N_ITEMS][50] = {
    "radioNetwork",
    "transport",
    "nas",
    "protocol",
    "misc",
};

typedef union {
  LIBLTE_M2AP_CAUSE_RADIONETWORK_STRUCT radioNetwork;
  LIBLTE_M2AP_CAUSE_TRANSPORT_STRUCT    transport;
  LIBLTE_M2AP_CAUSE_NAS_STRUCT          nas;
  LIBLTE_M2AP_CAUSE_PROTOCOL_STRUCT     protocol;
  LIBLTE_M2AP_CAUSE_MISC_STRUCT         misc;
} LIBLTE_M2AP_CAUSE_CHOICE_UNION;

typedef struct {
  bool                           ext;
  LIBLTE_M2AP_CAUSE_CHOICE_UNION choice;
  LIBLTE_M2AP_CAUSE_CHOICE_ENUM  choice_type;
} LIBLTE_M2AP_CAUSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_cause(LIBLTE_M2AP_CAUSE_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_cause(uint8_t** ptr, LIBLTE_M2AP_CAUSE_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE Criticality ENUMERATED
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_CRITICALITY_REJECT,
  LIBLTE_M2AP_CRITICALITY_IGNORE,
  LIBLTE_M2AP_CRITICALITY_NOTIFY,
  LIBLTE_M2AP_CRITICALITY_N_ITEMS,
} LIBLTE_M2AP_CRITICALITY_ENUM;
static const char liblte_m2ap_criticality_text[LIBLTE_M2AP_CRITICALITY_N_ITEMS][80] = {
    "reject",
    "ignore",
    "notify",
};

LIBLTE_ERROR_ENUM liblte_m2ap_pack_criticality(LIBLTE_M2AP_CRITICALITY_ENUM* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_criticality(uint8_t** ptr, LIBLTE_M2AP_CRITICALITY_ENUM* ie);

/*******************************************************************************
 * ProtocolIE ProtocolIE_ID INTEGER
 *******************************************************************************/
typedef struct {
  uint16_t ProtocolIE_ID;
} LIBLTE_M2AP_PROTOCOLIE_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_protocolie_id(LIBLTE_M2AP_PROTOCOLIE_ID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_protocolie_id(uint8_t** ptr, LIBLTE_M2AP_PROTOCOLIE_ID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE ProtocolExtensionID INTEGER
 *******************************************************************************/
typedef struct {
  uint16_t ProtocolExtensionID;
} LIBLTE_M2AP_PROTOCOLEXTENSIONID_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_protocolextensionid(LIBLTE_M2AP_PROTOCOLEXTENSIONID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_protocolextensionid(uint8_t** ptr, LIBLTE_M2AP_PROTOCOLEXTENSIONID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE ProtocolExtensionField SEQUENCE
 ********************************************************************************/
typedef struct {
  LIBLTE_M2AP_PROTOCOLEXTENSIONID_STRUCT id;
  LIBLTE_M2AP_CRITICALITY_ENUM           criticality;
  LIBLTE_ASN1_OPEN_TYPE_STRUCT           extensionValue;
} LIBLTE_M2AP_PROTOCOLEXTENSIONFIELD_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_protocolextensionfield(LIBLTE_M2AP_PROTOCOLEXTENSIONFIELD_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_protocolextensionfield(uint8_t**                                  ptr,
                                                            LIBLTE_M2AP_PROTOCOLEXTENSIONFIELD_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE ProtocolIE_SingleContainer SEQUENCE
 *******************************************************************************/
typedef struct {
  LIBLTE_M2AP_PROTOCOLIE_ID_STRUCT id;
  LIBLTE_M2AP_CRITICALITY_ENUM     criticality;
  LIBLTE_ASN1_OPEN_TYPE_STRUCT     value;
} LIBLTE_M2AP_PROTOCOLIE_SINGLECONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_s1ap_pack_protocolie_singlecontainer(LIBLTE_M2AP_PROTOCOLIE_SINGLECONTAINER_STRUCT* ie,
                                                              uint8_t**                                      ptr);
LIBLTE_ERROR_ENUM liblte_s1ap_unpack_protocolie_singlecontainer(uint8_t**                                      ptr,
                                                                LIBLTE_M2AP_PROTOCOLIE_SINGLECONTAINER_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE ProtocolExtensionContainer DYNAMIC SEQUENCE OF
 *******************************************************************************/
// lb:1, ub:65535
typedef struct {
  uint32_t                                  len;
  LIBLTE_M2AP_PROTOCOLEXTENSIONFIELD_STRUCT buffer[32]; // WARNING: Artificial limit to reduce memory footprint
} LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_protocolextensioncontainer(LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT* ie,
                                                              uint8_t**                                      ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_protocolextensioncontainer(uint8_t**                                      ptr,
                                                                LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE PLMNidentity STATIC OCTET STRING
 *******************************************************************************/
#define LIBLTE_M2AP_PLMN_IDENTITY_OCTET_STRING_LEN 3
typedef struct {
  uint8_t buffer[LIBLTE_M2AP_PLMN_IDENTITY_OCTET_STRING_LEN];
} LIBLTE_M2AP_PLMN_IDENTITY_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_plmnidentity(LIBLTE_M2AP_PLMN_IDENTITY_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_plmnidentity(uint8_t** ptr, LIBLTE_M2AP_PLMN_IDENTITY_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE EUTRANCellIdentifier STATIC BIT STRING
 *******************************************************************************/
#define LIBLTE_M2AP_EUTRAN_CELL_IDENTIFIER_BIT_STRING_LEN 28
typedef struct {
  uint32_t eUTRANCellIdentifier;
} LIBLTE_M2AP_EUTRAN_CELL_IDENTIFIER_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_eutrancellidentifier(LIBLTE_M2AP_EUTRAN_CELL_IDENTIFIER_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_eutrancellidentifier(uint8_t** ptr, LIBLTE_M2AP_EUTRAN_CELL_IDENTIFIER_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE ECGI SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                          ext;
  LIBLTE_M2AP_PLMN_IDENTITY_STRUCT              pLMN_Identity;
  LIBLTE_M2AP_EUTRAN_CELL_IDENTIFIER_STRUCT     EUTRANCellIdentifier;
  bool                                          iE_Extensions_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT iE_Extensions;
} LIBLTE_M2AP_ECGI_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_ecgi(LIBLTE_M2AP_ECGI_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_ecgi(uint8_t** ptr, LIBLTE_M2AP_ECGI_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE CellReservationInfo ENUMERATED
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_CELL_RESERVATION_INFO_RESERVED_CELL,
  LIBLTE_M2AP_CELL_RESERVATION_INFO_NON_RESERVED_CELL,
} LIBLTE_M2AP_CELL_RESERVATION_INFO_ENUM;

typedef struct {
  bool                                   ext;
  LIBLTE_M2AP_CELL_RESERVATION_INFO_ENUM e;
} LIBLTE_M2AP_CELL_RESERVATION_INFO_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_cellreservationinfo(LIBLTE_M2AP_CELL_RESERVATION_INFO_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_cellreservationinfo(uint8_t** ptr, LIBLTE_M2AP_CELL_RESERVATION_INFO_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE CellInformation SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                          ext;
  LIBLTE_M2AP_ECGI_STRUCT                       eCGI;
  LIBLTE_M2AP_CELL_RESERVATION_INFO_STRUCT      cellReservationInfo;
  bool                                          ie_extensions_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT ie_extensions;
} LIBLTE_M2AP_CELL_INFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_cellinformation(LIBLTE_M2AP_CELL_INFORMATION_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_cellinformation(uint8_t** ptr, LIBLTE_M2AP_CELL_INFORMATION_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE CellInformationList SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                ext;
  uint16_t                            len;
  LIBLTE_M2AP_CELL_INFORMATION_STRUCT buffer[32]; // artificial limit to 32
} LIBLTE_M2AP_CELL_INFORMATION_LIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_cellinformationlist(LIBLTE_M2AP_CELL_INFORMATION_LIST_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_cellinformationlist(uint8_t** ptr, LIBLTE_M2AP_CELL_INFORMATION_LIST_STRUCT* ie);
/*******************************************************************************
 * ProtocolIE ProcedureCode INTEGER
 *******************************************************************************/
typedef struct {
  uint8_t ProcedureCode;
} LIBLTE_M2AP_PROCEDURECODE_STRUCT;

/*******************************************************************************
 * ProtocolIE TriggeringMessage ENUMERATED
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_TRIGGERINGMESSAGE_INITIATING_MESSAGE,
  LIBLTE_M2AP_TRIGGERINGMESSAGE_SUCCESSFUL_OUTCOME,
  LIBLTE_M2AP_TRIGGERINGMESSAGE_UNSUCCESSFULL_OUTCOME,
  LIBLTE_M2AP_TRIGGERINGMESSAGE_N_ITEMS,
} LIBLTE_M2AP_TRIGGERINGMESSAGE_ENUM;
static const char liblte_m2ap_triggeringmessage_text[LIBLTE_M2AP_TRIGGERINGMESSAGE_N_ITEMS][80] = {
    "initiating-message",
    "successful-outcome",
    "unsuccessfull-outcome",
};

LIBLTE_ERROR_ENUM liblte_m2ap_pack_triggeringmessage(LIBLTE_M2AP_TRIGGERINGMESSAGE_ENUM* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_triggeringmessage(uint8_t** ptr, LIBLTE_M2AP_TRIGGERINGMESSAGE_ENUM* ie);

/*******************************************************************************
 * ProtocolIE TypeOfError ENUMERATED
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_TYPEOFERROR_NOT_UNDERSTOOD,
  LIBLTE_M2AP_TYPEOFERROR_MISSING,
  LIBLTE_M2AP_TYPEOFERROR_N_ITEMS,
} LIBLTE_M2AP_TYPEOFERROR_ENUM;
static const char liblte_M2ap_typeoferror_text[LIBLTE_M2AP_TYPEOFERROR_N_ITEMS][80] = {
    "not-understood",
    "missing",
};

typedef struct {
  bool                         ext;
  LIBLTE_M2AP_TYPEOFERROR_ENUM e;
} LIBLTE_M2AP_TYPEOFERROR_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_typeoferror(LIBLTE_M2AP_TYPEOFERROR_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_typeoferror(uint8_t** ptr, LIBLTE_M2AP_TYPEOFERROR_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE CriticalityDiagnostics_IE_Item SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                          ext;
  LIBLTE_M2AP_CRITICALITY_ENUM                  iECriticality;
  LIBLTE_M2AP_PROTOCOLIE_ID_STRUCT              iE_ID;
  LIBLTE_M2AP_TYPEOFERROR_STRUCT                typeOfError;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT iE_Extensions;
  bool                                          iE_Extensions_present;
} LIBLTE_M2AP_CRITICALITYDIAGNOSTICS_IE_ITEM_STRUCT;

/*******************************************************************************
 * ProtocolIE CriticalityDiagnostics_IE_List DYNAMIC SEQUENCE OF
 *******************************************************************************/
// lb:1, ub:256
typedef struct {
  uint32_t                                          len;
  LIBLTE_M2AP_CRITICALITYDIAGNOSTICS_IE_ITEM_STRUCT buffer[32]; // WARNING: Artificial limit to reduce memory footprint
} LIBLTE_M2AP_CRITICALITYDIAGNOSTICS_IE_LIST_STRUCT;

/*******************************************************************************
 * ProtocolIE CriticalityDiagnostics SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                              ext;
  LIBLTE_M2AP_PROCEDURECODE_STRUCT                  procedureCode;
  bool                                              procedureCode_present;
  LIBLTE_M2AP_TRIGGERINGMESSAGE_ENUM                triggeringMessage;
  bool                                              triggeringMessage_present;
  LIBLTE_M2AP_CRITICALITY_ENUM                      procedureCriticality;
  bool                                              procedureCriticality_present;
  LIBLTE_M2AP_CRITICALITYDIAGNOSTICS_IE_LIST_STRUCT iEsCriticalityDiagnostics;
  bool                                              iEsCriticalityDiagnostics_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT     iE_Extensions;
  bool                                              iE_Extensions_present;
} LIBLTE_M2AP_CRITICALITYDIAGNOSTICS_STRUCT;

/*******************************************************************************
 * ProtocolIE ENB-ID STATIC BIT STRING
 *******************************************************************************/
#define LIBLTE_M2AP_ENBID_BIT_STRING_LEN 20
typedef struct {
  bool    ext;
  uint8_t buffer[20];
} LIBLTE_M2AP_ENB_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_enbid(LIBLTE_M2AP_ENB_ID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_enbid(uint8_t** ptr, LIBLTE_M2AP_ENB_ID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MCE-ID STATIC OCTET STRING
 *******************************************************************************/
#define LIBLTE_M2AP_MCEID_OCTET_STRING_LEN 2
typedef struct {
  bool    ext;
  uint8_t buffer[2];
} LIBLTE_M2AP_MCE_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mceid(LIBLTE_M2AP_MCE_ID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mceid(uint8_t** ptr, LIBLTE_M2AP_MCE_ID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MCE-Name PRINTABLE STRING
 *******************************************************************************/
typedef struct {
  bool     ext;
  uint32_t n_octets;
  uint8_t  buffer[150];
} LIBLTE_M2AP_MCE_NAME_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mcename(LIBLTE_M2AP_MCE_NAME_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mceid(uint8_t** ptr, LIBLTE_M2AP_MCE_NAME_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MCE-MBMS-M2AP-ID INTEGER
 *******************************************************************************/
// lb:0;ub:16777215
typedef struct {
  uint32_t mce_mbms_m2ap_id;
} LIBLTE_M2AP_MCE_MBMS_M2AP_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mcembmsm2apid(LIBLTE_M2AP_MCE_MBMS_M2AP_ID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mcembmsm2apid(uint8_t** ptr, LIBLTE_M2AP_MCE_MBMS_M2AP_ID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MBSFN-Synchronization-Area-Id INTEGER
 *******************************************************************************/
typedef struct {
  uint32_t mbsfn_synchronisation_area_id;
} LIBLTE_M2AP_MBSFN_SYNCHRONISATION_AREA_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbsfnsynchronisationareaid(LIBLTE_M2AP_MBSFN_SYNCHRONISATION_AREA_ID_STRUCT* ie,
                                                              uint8_t**                                         ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbsfnsynchronisationareaid(uint8_t**                                         ptr,
                                                                LIBLTE_M2AP_MBSFN_SYNCHRONISATION_AREA_ID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MBMS-Service-Area DYNAMIC OCTET STRING
 *******************************************************************************/
// lb:0, ub:512
typedef struct {
  uint32_t n_octets;
  uint8_t  buffer[512];
} LIBLTE_M2AP_MBMS_SERVICE_AREA_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmsservicearea(LIBLTE_M2AP_MBMS_SERVICE_AREA_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbmsservicearea(uint8_t** ptr, LIBLTE_M2AP_MBMS_SERVICE_AREA_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MBMS-Service-Area-Id-List SEQUENCE
 *******************************************************************************/
typedef struct {
  uint8_t                              len;
  LIBLTE_M2AP_MBMS_SERVICE_AREA_STRUCT buffer[32]; // Waring: Artificial limit to reduce memory footprint
} LIBLTE_M2AP_MBMS_SERVICE_AREA_ID_LIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmsserviceareaidlist(LIBLTE_M2AP_MBMS_SERVICE_AREA_ID_LIST_STRUCT* ie,
                                                         uint8_t**                                     ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbmsserviceareaidlist(uint8_t**                                     ptr,
                                                           LIBLTE_M2AP_MBMS_SERVICE_AREA_ID_LIST_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE ENB-MBMS-ConfigurationDataItem SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                             ext;
  LIBLTE_M2AP_ECGI_STRUCT                          eCGI;
  LIBLTE_M2AP_MBSFN_SYNCHRONISATION_AREA_ID_STRUCT mbsfnSynchronisationArea;
  LIBLTE_M2AP_MBMS_SERVICE_AREA_ID_LIST_STRUCT     mbmsServiceAreaList;
  bool                                             iE_Extensions_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT    iE_Extensions;
} LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_enbmbmsconfigurationdataitem(LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_ITEM_STRUCT* ie,
                                                                uint8_t** ptr);
LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_enbmbmsconfigurationdataitem(uint8_t** ptr, LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_ITEM_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE ENB-MBMS-ConfigurationDataConfigUpdateItem SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                                ext;
  LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_ITEM_STRUCT mBMSConfigData;
  LIBLTE_M2AP_ECGI_STRUCT                             eCGI;
} LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_CONFIGUPDATE_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_enbmbmsconfigurationdataconfigupdateitem(
    LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_CONFIGUPDATE_ITEM_STRUCT* ie,
    uint8_t**                                                         ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_enbmbmsconfigurationdataconfigupdateitem(
    uint8_t**                                                         ptr,
    LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_CONFIGUPDATE_ITEM_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE ENB-MBMS-M2AP-ID INTEGER
 *******************************************************************************/
// lb:0;ub:65535
typedef struct {
  uint16_t enb_mbms_m2ap_id;
} LIBLTE_M2AP_ENB_MBMS_M2AP_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_enbmbmsm2apid(LIBLTE_M2AP_ENB_MBMS_M2AP_ID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_enbmbmsm2apid(uint8_t** ptr, LIBLTE_M2AP_ENB_MBMS_M2AP_ID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE ENB-NAME PRINTABLE STRING
 *******************************************************************************/
typedef struct {
  bool     ext;
  uint32_t n_octets;
  uint8_t  buffer[150];
} LIBLTE_M2AP_ENBNAME_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_enbname(LIBLTE_M2AP_ENBNAME_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_enbname(uint8_t** ptr, LIBLTE_M2AP_ENBNAME_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE Global-ENB-Id SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                          ext;
  LIBLTE_M2AP_PLMN_IDENTITY_STRUCT              pLMNidentity;
  LIBLTE_M2AP_ENB_ID_STRUCT                     eNB_ID;
  bool                                          iE_Extensions_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT iE_Extensions;
} LIBLTE_M2AP_GLOBAL_ENB_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_globalenbid(LIBLTE_M2AP_GLOBAL_ENB_ID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_globalenbid(uint8_t** ptr, LIBLTE_M2AP_GLOBAL_ENB_ID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE Global-MCE-Id SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                          ext;
  LIBLTE_M2AP_PLMN_IDENTITY_STRUCT              pLMN_Identity;
  LIBLTE_M2AP_MCE_ID_STRUCT                     mCE_ID;
  bool                                          iE_Extensions_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT iE_extensions;
} LIBLTE_M2AP_GLOBAL_MCE_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_globalmceid(LIBLTE_M2AP_GLOBAL_MCE_ID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_globalmceid(uint8_t** ptr, LIBLTE_M2AP_GLOBAL_MCE_ID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE GTP-TEID STATIC OCTET STRING
 *******************************************************************************/
typedef struct {
  uint8_t buffer[4];
} LIBLTE_M2AP_GTP_TEID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_gtpteid(LIBLTE_M2AP_GTP_TEID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_gtpteid(uint8_t** ptr, LIBLTE_M2AP_GTP_TEID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE IPAddress DYNAMIC OCTET STRING
 *******************************************************************************/
typedef struct {
  uint8_t len;
  uint8_t buffer[16];
} LIBLTE_M2AP_IP_ADDRESS_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_ipaddress(LIBLTE_M2AP_IP_ADDRESS_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_ipaddress(uint8_t** ptr, LIBLTE_M2AP_IP_ADDRESS_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE LCID INTEGER
 *******************************************************************************/
typedef struct {
  uint8_t lcid;
} LIBLTE_M2AP_LCID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_lcid(LIBLTE_M2AP_LCID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_lcid(uint8_t** ptr, LIBLTE_M2AP_LCID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MBMS-Service-associatedLogicalM2-ConnectionItem SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                eNB_MBMS_M2AP_ID_present;
  LIBLTE_M2AP_ENB_MBMS_M2AP_ID_STRUCT eNB_MBMS_M2AP_ID;
  bool                                mCE_MBMS_M2AP_ID_present;
  LIBLTE_M2AP_ENB_MBMS_M2AP_ID_STRUCT mCE_MBMS_M2AP_ID;
} LIBLTE_M2AP_MBMS_SERVICE_ASSOCIATEDLOGICALM2_CONNECTIONITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmsserviceassociatedlogicalm2connectionitem(
    LIBLTE_M2AP_MBMS_SERVICE_ASSOCIATEDLOGICALM2_CONNECTIONITEM_STRUCT* ie,
    uint8_t**                                                           ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbmsserviceassociatedlogicalm2connectionitem(
    uint8_t**                                                           ptr,
    LIBLTE_M2AP_MBMS_SERVICE_ASSOCIATEDLOGICALM2_CONNECTIONITEM_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MBMS-Session-Id STATIC OCTET STRING
 *******************************************************************************/
typedef struct {
  uint8_t buffer[1];
} LIBLTE_M2AP_MBMS_SESSION_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmssessionid(LIBLTE_M2AP_MBMS_SESSION_ID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbmssessionid(uint8_t** ptr, LIBLTE_M2AP_MBMS_SESSION_ID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MBSFN-Area-ID INTEGER
 *******************************************************************************/
typedef struct {
  uint8_t mbsfn_area_id;
} LIBLTE_M2AP_MBSFN_AREA_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbsfnareaid(LIBLTE_M2AP_MBSFN_AREA_ID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbsfnareaid(uint8_t** ptr, LIBLTE_M2AP_MBSFN_AREA_ID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MBSFN-SynchronizationArea-ID INTEGER
 *******************************************************************************/
typedef struct {
  uint32_t mbsfn_synchronization_area_id;
} LIBLTE_M2AP_MBSFN_SYNCHRONIZATION_AREA_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbsfnsynchronizationareaid(LIBLTE_M2AP_MBSFN_SYNCHRONIZATION_AREA_ID_STRUCT* ie,
                                                              uint8_t**                                         ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbsfnsynchronizationareaid(uint8_t**                                         ptr,
                                                                LIBLTE_M2AP_MBSFN_SYNCHRONIZATION_AREA_ID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE RadioframeAllocationPeriod ENUMERATED
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N1,
  LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N2,
  LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N4,
  LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N8,
  LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N16,
  LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N32
} LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_ENUM;

typedef struct {
  LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_ENUM e;
} LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_radioframeallocationperiod(LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_STRUCT* ie,
                                                              uint8_t**                                        ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_radioframeallocationperiod(uint8_t**                                        ptr,
                                                                LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE RadioframeAllocationOffset INTEGER
 *******************************************************************************/
typedef struct {
  uint8_t radioframeAllocationOffset;
} LIBLTE_M2AP_RADIOFRAME_ALLOCATION_OFFSET_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_radioframeallocationoffset(LIBLTE_M2AP_RADIOFRAME_ALLOCATION_OFFSET_STRUCT* ie,
                                                              uint8_t**                                        ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_radioframeallocationoffset(uint8_t**                                        ptr,
                                                                LIBLTE_M2AP_RADIOFRAME_ALLOCATION_OFFSET_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE SubframeAllocation CHOICE
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_SUBFRAME_ALLOCATION_ONE_FRAME,
  LIBLTE_M2AP_SUBFRAME_ALLOCATION_FOUR_FRAMES
} LIBLTE_M2AP_SUBFRAME_ALLOCATION_ENUM;

typedef struct {
  LIBLTE_M2AP_SUBFRAME_ALLOCATION_ENUM choice_type;
  union {
    uint8_t oneFrame[6];    // BIT STRING
    uint8_t fourFrames[24]; // BIT STRING
  } choice;
} LIBLTE_M2AP_SUBFRAME_ALLOCATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_subframeallocation(LIBLTE_M2AP_SUBFRAME_ALLOCATION_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_subframeallocation(uint8_t** ptr, LIBLTE_M2AP_SUBFRAME_ALLOCATION_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MBSFN-Subframe-Configuration SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                            ext;
  LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_STRUCT radioFrameAllocationPeriod;
  LIBLTE_M2AP_RADIOFRAME_ALLOCATION_OFFSET_STRUCT radioFrameAllocationOffset;
  LIBLTE_M2AP_SUBFRAME_ALLOCATION_STRUCT          subframeAllocation;
  bool                                            iE_Extensions_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT   iE_Extensions;
} LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbsfnsubframeconfiguration(LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_STRUCT* ie,
                                                              uint8_t**                                        ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbsfnsubframeconfiguration(uint8_t**                                        ptr,
                                                                LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MBSFN-Subframe-Configuration-List SEQUENCE
 *******************************************************************************/
typedef struct {
  uint32_t                                        len;
  LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_STRUCT buffer[32];
} LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_LIST_STRUCT;

LIBLTE_ERROR_ENUM
liblte_m2ap_pack_mbsfnsubframeconfigurationlist(LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_LIST_STRUCT* ie,
                                                uint8_t**                                             ptr);
LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_mbsfnsubframeconfigurationlist(uint8_t**                                             ptr,
                                                  LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_LIST_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MCCH-Upadte-Time INTEGER
 *******************************************************************************/
// lb:0;ub:255
typedef struct {
  uint8_t mcchUpdateTime;
} LIBLTE_M2AP_MCCH_UPDATE_TIME_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mcchupdatetime(LIBLTE_M2AP_MCCH_UPDATE_TIME_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mcchupdatetime(uint8_t** ptr, LIBLTE_M2AP_MCCH_UPDATE_TIME_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE PDCCH-Length ENUM
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_PDCCH_LENGTH_S1,
  LIBLTE_M2AP_PDCCH_LENGTH_S2,
} LIBLTE_M2AP_PDCCH_LENGTH_ENUM;

typedef struct {
  bool                          ext;
  LIBLTE_M2AP_PDCCH_LENGTH_ENUM pdcchLength;
} LIBLTE_M2AP_PDCCH_LENGTH_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_pdcchlength(LIBLTE_M2AP_PDCCH_LENGTH_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_pdcchlength(uint8_t** ptr, LIBLTE_M2AP_PDCCH_LENGTH_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE Repetition Period ENUM
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_REPETITION_PERIOD_RF32,
  LIBLTE_M2AP_REPETITION_PERIOD_RF64,
  LIBLTE_M2AP_REPETITION_PERIOD_RF128,
  LIBLTE_M2AP_REPETITION_PERIOD_RF256,
} LIBLTE_M2AP_REPETITION_PERIOD_ENUM;

typedef struct {
  LIBLTE_M2AP_REPETITION_PERIOD_ENUM repetitionPeriod;
} LIBLTE_M2AP_REPETITION_PERIOD_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_repetiotionperiod(LIBLTE_M2AP_REPETITION_PERIOD_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_repetiotionperiod(uint8_t** ptr, LIBLTE_M2AP_REPETITION_PERIOD_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE OFFSET INTEGER
 *******************************************************************************/
typedef struct {
  uint8_t offset;
} LIBLTE_M2AP_OFFSET_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_offset(LIBLTE_M2AP_OFFSET_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_offset(uint8_t** ptr, LIBLTE_M2AP_OFFSET_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE Modification Period ENUM
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_MODIFICATION_PERIOD_RF512,
  LIBLTE_M2AP_MODIFICATION_PERIOD_RF1024,
} LIBLTE_M2AP_MODIFICATION_PERIOD_ENUM;

typedef struct {
  LIBLTE_M2AP_MODIFICATION_PERIOD_ENUM modificationPeriod;
} LIBLTE_M2AP_MODIFICATION_PERIOD_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_modificationperiod(LIBLTE_M2AP_MODIFICATION_PERIOD_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_modificationperiod(uint8_t** ptr, LIBLTE_M2AP_MODIFICATION_PERIOD_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE Subframe-Allocation-Info STATIC BIT STRING
 *******************************************************************************/
#define LIBLTE_M2AP_SUBFRAME_ALLOCATION_INFO_BIT_STRING_LEN 6
typedef struct {
  uint8_t buffer[6];
} LIBLTE_M2AP_SUBFRAME_ALLOCATION_INFO_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_subframeallocationinfo(LIBLTE_M2AP_SUBFRAME_ALLOCATION_INFO_STRUCT* ie,
                                                          uint8_t**                                    ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_subframeallocationinfo(uint8_t**                                    ptr,
                                                            LIBLTE_M2AP_SUBFRAME_ALLOCATION_INFO_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE Modulation-and-Coding-Scheme ENUM
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N2,
  LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N7,
  LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N13,
  LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N19
} LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_ENUM;

typedef struct {
  LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_ENUM mcs;
} LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_modulationandcodingscheme(LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_STRUCT* ie,
                                                             uint8_t**                                        ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_modulationandcodingscheme(uint8_t**                                        ptr,
                                                               LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MCCHrelatedBCCH-ConfigPerMBSFNArea-Item SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                            ext;
  LIBLTE_M2AP_MBSFN_AREA_ID_STRUCT                mbsfnArea;
  LIBLTE_M2AP_PDCCH_LENGTH_STRUCT                 pdcchLength;
  LIBLTE_M2AP_REPETITION_PERIOD_STRUCT            repetitionPeriod;
  LIBLTE_M2AP_OFFSET_STRUCT                       offset;
  LIBLTE_M2AP_MODIFICATION_PERIOD_STRUCT          modificationPeriod;
  LIBLTE_M2AP_SUBFRAME_ALLOCATION_INFO_STRUCT     subframeAllocationInfo;
  LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_STRUCT modulationAndCodingScheme;
  bool                                            cellInformationList_present;
  LIBLTE_M2AP_CELL_INFORMATION_LIST_STRUCT        cellInformationList;
  bool                                            iE_Extensions_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT   iE_Extensions;
} LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mcchrelatedbcchconfigpermbsfnareaitem(
    LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_ITEM_STRUCT* ie,
    uint8_t**                                                        ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mcchrelatedbcchconfigpermbsfnareaitem(
    uint8_t**                                                        ptr,
    LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_ITEM_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MCCHrelatedBCCH-ConfigPerMBSFNArea DYNAMIC SEQUENCE
 *******************************************************************************/
typedef struct {
  uint16_t                                                        len;
  LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_ITEM_STRUCT buffer[8];
} LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_STRUCT;

LIBLTE_ERROR_ENUM
liblte_m2ap_pack_mcchrelatedbcchconfigpermbsfnarea(LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_STRUCT* ie,
                                                   uint8_t**                                                   ptr);
LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_mcchrelatedbcchconfigpermbsfnarea(uint8_t**                                                   ptr,
                                                     LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_STRUCT* ie);
/*******************************************************************************
 * ProtocolIE MCH-Scheduling-Period ENUM
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF8,
  LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF16,
  LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF32,
  LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF64,
  LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF128,
  LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF256,
  LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF512,
  LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF1024
} LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_ENUM;

typedef struct {
  LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_ENUM e;
} LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mchschedulingperiod(LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mchschedulingperiod(uint8_t** ptr, LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE OFFSET INTEGER
 *******************************************************************************/
typedef struct {
  uint8_t dataMCS;
} LIBLTE_M2AP_DATA_MCS_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_datamcs(LIBLTE_M2AP_DATA_MCS_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_datamcs(uint8_t** ptr, LIBLTE_M2AP_DATA_MCS_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE Service Id STATIC OCTET STRING
 *******************************************************************************/
#define LIBLTE_M2AP_SERVICE_ID_OCTET_STRING_LEN 3
typedef struct {
  uint8_t buffer[3];
} LIBLTE_M2AP_SERVICE_ID_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_serviceid(LIBLTE_M2AP_SERVICE_ID_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_serviceid(uint8_t** ptr, LIBLTE_M2AP_SERVICE_ID_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE TMGI SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                          ext;
  LIBLTE_M2AP_PLMN_IDENTITY_STRUCT              pLMN_Identity;
  LIBLTE_M2AP_SERVICE_ID_STRUCT                 serviceID;
  bool                                          iE_Extensions_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT iE_Extensions;
} LIBLTE_M2AP_TMGI_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_tmgi(LIBLTE_M2AP_TMGI_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_tmgi(uint8_t** ptr, LIBLTE_M2AP_TMGI_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MBMSSessionListPerPMCH-Item SEQUENCE (SIZE(1..maxnoofSessionsPerPMCH)) OF SEQUENCE
 *******************************************************************************/
typedef struct {
  uint16_t len;
  struct {
    bool                    ext;
    LIBLTE_M2AP_TMGI_STRUCT Tmgi;
    LIBLTE_M2AP_LCID_STRUCT Lcid;
    bool                    iE_Extensions_present;
  } buffer[8];
} LIBLTE_M2AP_MBMS_SESSION_LIST_PER_PMCH_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmssessionlistperpmchitem(LIBLTE_M2AP_MBMS_SESSION_LIST_PER_PMCH_ITEM_STRUCT* ie,
                                                              uint8_t**                                           ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbmssessionlistperpmchitem(uint8_t**                                           ptr,
                                                                LIBLTE_M2AP_MBMS_SESSION_LIST_PER_PMCH_ITEM_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE PMCH-Configuration SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                          ext;
  LIBLTE_M2AP_ALLOCATED_SUBFRAMES_END_STRUCT    allocatedSubframesEnd;
  LIBLTE_M2AP_DATA_MCS_STRUCT                   dataMCS;
  LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_STRUCT      mchSchedulingPeriod;
  bool                                          iE_Extensions_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT iE_Extensions;
} LIBLTE_M2AP_PMCH_CONFIGURATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_pmchconfiguration(LIBLTE_M2AP_PMCH_CONFIGURATION_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_pmchconfiguration(uint8_t** ptr, LIBLTE_M2AP_PMCH_CONFIGURATION_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE PMCH-Configuration-Item SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                               ext;
  LIBLTE_M2AP_PMCH_CONFIGURATION_STRUCT              PMCHConfiguration;
  LIBLTE_M2AP_MBMS_SESSION_LIST_PER_PMCH_ITEM_STRUCT MBMSSessionListPerPMCHItem;
  bool                                               iE_Extensions_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT      iE_Extensions;
} LIBLTE_M2AP_PMCH_CONFIGURATION_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_pmchconfigurationitem(LIBLTE_M2AP_PMCH_CONFIGURATION_ITEM_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_pmchconfigurationitem(uint8_t**                                   ptr,
                                                           LIBLTE_M2AP_PMCH_CONFIGURATION_ITEM_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE PMCH-Configuration-List SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                          ext;
  uint16_t                                      len;
  LIBLTE_M2AP_PMCH_CONFIGURATION_ITEM_STRUCT    buffer[32];
  bool                                          iE_Extensions_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT iE_Extensions;
} LIBLTE_M2AP_PMCH_CONFIGURATION_LIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_pmchconfigurationlist(LIBLTE_M2AP_PMCH_CONFIGURATION_LIST_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_pmchconfigurationlist(uint8_t**                                   ptr,
                                                           LIBLTE_M2AP_PMCH_CONFIGURATION_LIST_STRUCT* ie);
/*******************************************************************************
 * ProtocolIE Common-Subframe-Scheduling-Period ENUM
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF4,
  LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF8,
  LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF16,
  LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF32,
  LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF64,
  LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF128,
  LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF256,
} LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_ENUM;

typedef struct {
  LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_ENUM e;
} LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_STRUCT;

LIBLTE_ERROR_ENUM
liblte_m2ap_pack_commonsubframeallocationperiod(LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_STRUCT* ie,
                                                uint8_t**                                             ptr);
LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_commonsubframeallocationperiod(uint8_t**                                             ptr,
                                                  LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE Time-to-Wait ENUM
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_TIME_TO_WAIT_V1S,
  LIBLTE_M2AP_TIME_TO_WAIT_V2S,
  LIBLTE_M2AP_TIME_TO_WAIT_V5S,
  LIBLTE_M2AP_TIME_TO_WAIT_V10S,
  LIBLTE_M2AP_TIME_TO_WAIT_V20S,
  LIBLTE_M2AP_TIME_TO_WAIT_V60S,
} LIBLTE_M2AP_TIME_TO_WAIT_ENUM;

typedef struct {
  LIBLTE_M2AP_TIME_TO_WAIT_ENUM e;
} LIBLTE_M2AP_TIME_TO_WAIT_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_timetowait(LIBLTE_M2AP_TIME_TO_WAIT_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_timetowait(uint8_t** ptr, LIBLTE_M2AP_TIME_TO_WAIT_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE TNL-Information SEQUENCE
 *******************************************************************************/
typedef struct {
  bool                                          ext;
  LIBLTE_M2AP_IP_ADDRESS_STRUCT                 iPMCAddress;
  LIBLTE_M2AP_IP_ADDRESS_STRUCT                 iPSourceAddress;
  LIBLTE_M2AP_GTP_TEID_STRUCT                   gtpTeid;
  bool                                          iE_Extensions_present;
  LIBLTE_M2AP_PROTOCOLEXTENSIONCONTAINER_STRUCT iE_Extensions;
} LIBLTE_M2AP_TNL_INFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_tnlinformation(LIBLTE_M2AP_TNL_INFORMATION_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_tnlinformation(uint8_t** ptr, LIBLTE_M2AP_TNL_INFORMATION_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE ENB-MBMS-Configuration-data-List SEQUENCE OF ProtocolIE-Single-Container
 *******************************************************************************/
// lb:1;ub:maxnofCells (256)
typedef struct {
  uint8_t                                             len;
  LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_ITEM_STRUCT buffer[32]; // Waring: Artificial limit to reduce memory footprint
} LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_LIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_enbmbmsconfigurationdatalist(LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_LIST_STRUCT* ie,
                                                                uint8_t** ptr);
LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_enbmbmsconfigurationdatalist(uint8_t** ptr, LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_LIST_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MBSFN-AreaConfiguration-Item ProtocolIE-Container
 *******************************************************************************/
typedef struct {
  LIBLTE_M2AP_PMCH_CONFIGURATION_LIST_STRUCT           PMCHConfigurationList;
  LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_LIST_STRUCT MBSFNSubframeConfigurationList;
  LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_STRUCT CommonSubframeAllocationPeriod;
  LIBLTE_M2AP_MBSFN_AREA_ID_STRUCT                     MBSFNAreaId;
} LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_ITEM_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbsfnareaconfigurationitem(LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_ITEM_STRUCT* ie,
                                                              uint8_t**                                         ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbsfnareaconfigurationitem(uint8_t**                                         ptr,
                                                                LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_ITEM_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE MBSFN-AreaConfiguration-List SEQUENCE OF ProtocolIE-Single-Container
 *******************************************************************************/
// lb:1;ub:maxnofCells (256)
typedef struct {
  uint8_t                                          len;
  LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_ITEM_STRUCT buffer[32]; // Waring: Artificial limit to reduce memory footprint
} LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_LIST_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbsfnareaconfigurationlist(LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_LIST_STRUCT* ie,
                                                              uint8_t**                                         ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbsfnareaconfigurationlist(uint8_t**                                         ptr,
                                                                LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_LIST_STRUCT* ie);

/*******************************************************************************
 * Protocol Message M2SetupRequest STRUCT
 *******************************************************************************/
typedef struct {
  bool                                                ext;
  LIBLTE_M2AP_GLOBAL_ENB_ID_STRUCT                    Global_ENB_ID;
  bool                                                eNBname_present;
  LIBLTE_M2AP_ENBNAME_STRUCT                          eNBname;
  LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_LIST_STRUCT configurationDataList;
} LIBLTE_M2AP_MESSAGE_M2SETUPREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_m2setuprequest(LIBLTE_M2AP_MESSAGE_M2SETUPREQUEST_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_m2setuprequest(uint8_t** ptr, LIBLTE_M2AP_MESSAGE_M2SETUPREQUEST_STRUCT* ie);

/*******************************************************************************
 * Protocol Message M2SetupResponse STRUCT
 *******************************************************************************/
typedef struct {
  bool                                                       ext;
  LIBLTE_M2AP_GLOBAL_MCE_ID_STRUCT                           Global_MCE_ID;
  bool                                                       MCEname_present;
  LIBLTE_M2AP_MCE_NAME_STRUCT                                MCEname;
  LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_STRUCT MCCHrelatedBCCHConfigPerMBSFNArea;
  bool                                                       criticalityDiagnosis_present;
  LIBLTE_M2AP_CRITICALITYDIAGNOSTICS_STRUCT                  criticalityDiagnosis;
} LIBLTE_M2AP_MESSAGE_M2SETUPRESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_m2setupresponse(LIBLTE_M2AP_MESSAGE_M2SETUPRESPONSE_STRUCT* ie, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_m2setupresponse(uint8_t** ptr, LIBLTE_M2AP_MESSAGE_M2SETUPRESPONSE_STRUCT* ie);

/*******************************************************************************
 * Protocol Message MBMSSessionStartRequest STRUCT
 *******************************************************************************/
typedef struct {
  bool                                 ext;
  LIBLTE_M2AP_MCE_MBMS_M2AP_ID_STRUCT  MceMbmsM2apId;
  LIBLTE_M2AP_TMGI_STRUCT              Tmgi;
  bool                                 MbmsSessionId_present;
  LIBLTE_M2AP_MBMS_SESSION_ID_STRUCT   MbmsSessionId;
  LIBLTE_M2AP_MBMS_SERVICE_AREA_STRUCT MbmsServiceArea;
  LIBLTE_M2AP_TNL_INFORMATION_STRUCT   TnlInformation;
} LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTREQUEST_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmssessionstartrequest(LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTREQUEST_STRUCT* ie,
                                                           uint8_t**                                           ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbmssessionstartrequest(uint8_t**                                           ptr,
                                                             LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTREQUEST_STRUCT* ie);

/*******************************************************************************
 * Protocol Message MBMSSessionStartResponse STRUCT
 *******************************************************************************/
typedef struct {
  bool                                      ext;
  LIBLTE_M2AP_MCE_MBMS_M2AP_ID_STRUCT       MceMbmsM2apId;
  LIBLTE_M2AP_ENB_MBMS_M2AP_ID_STRUCT       EnbMbmsM2apId;
  bool                                      CriticalityDiagnostics_present;
  LIBLTE_M2AP_CRITICALITYDIAGNOSTICS_STRUCT CriticalityDiagnostics;
} LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTRESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmssessionstartresponse(LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTRESPONSE_STRUCT* ie,
                                                            uint8_t**                                            ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_mbmssessionstartresponse(uint8_t**                                            ptr,
                                                              LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTRESPONSE_STRUCT* ie);

/*******************************************************************************
 * Protocol Message MBMSSchedulingInformation STRUCT
 *******************************************************************************/
typedef struct {
  bool                                             ext;
  LIBLTE_M2AP_MCCH_UPDATE_TIME_STRUCT              MCCHUpdateTime;
  LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_LIST_STRUCT MbsfnAreaConfigurationList;
} LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATION_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmsschedulinginformation(LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATION_STRUCT* ie,
                                                             uint8_t**                                             ptr);
LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_mbmsschedulinginformation(uint8_t** ptr, LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATION_STRUCT* ie);

/*******************************************************************************
 * Protocol Message MBMSSchedulingInformationResponse STRUCT
 *******************************************************************************/
typedef struct {
  bool                                      ext;
  bool                                      CriticalityDiagnostics_present;
  LIBLTE_M2AP_CRITICALITYDIAGNOSTICS_STRUCT CriticalityDiagnostics;
} LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATIONRESPONSE_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_mbmsschedulinginformation(LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATION_STRUCT* ie,
                                                             uint8_t**                                             ptr);
LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_mbmsschedulinginformation(uint8_t** ptr, LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATION_STRUCT* ie);

/*******************************************************************************
 * ProtocolIE-Field
 *******************************************************************************/
LIBLTE_ERROR_ENUM
liblte_m2ap_pack_protocolie_header(uint32_t len, uint32_t ie_id, LIBLTE_M2AP_CRITICALITY_ENUM crit, uint8_t** ptr);
LIBLTE_ERROR_ENUM
liblte_m2ap_unpack_protocolie_header(uint8_t** ptr, uint32_t* ie_id, LIBLTE_M2AP_CRITICALITY_ENUM* crit, uint32_t* len);

/*******************************************************************************
 * Procedure code criticality lookups
 *******************************************************************************/
LIBLTE_M2AP_CRITICALITY_ENUM liblte_m2ap_procedure_criticality(uint8_t procedureCode);

/*******************************************************************************
 * InitiatingMessage CHOICE
 ********************************************************************************/
typedef enum {
  LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_MBMSSESSIONSTARTREQUEST,
  LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_MBMSSESSIONSTOPREQUEST,
  LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_MBMSSESSION_UPDATEREQUEST,
  LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_MBMSSCHEDULINGINFORMATION,
  LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_RESET,
  LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_M2SETUPREQUEST,
  LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_ENBCONFIGURATIONUPDATE,
  LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_MCECONFIGURATIONUPDATE,
  LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_MBMSSERVICECOUNTINGREQUEST,
  LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_N_ITEMS,
} LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_ENUM;
static const char liblte_m2ap_initiatingmessage_choice_text[LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_N_ITEMS][50] = {
    "MbmsSessionStartRequest",
    "MbmsSessionStopRequest",
    "MbmsSessionUpdateRequest",
    "MbmsSchedulingInformation",
    "Reset",
    "M2SetupRequest",
    "EnbConfigurationUpdate",
    "MceConfigurationUpdate",
    "MbmsServiceCountingRequest"};

typedef union {
  LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTREQUEST_STRUCT MbmsSessionStartRequest;
  // LIBLTE_M2AP_MBMS_SESSION_STOP_REQUEST               MbmsSessionStopRequest;
  // LIBLTE_M2AP_MBMS_SESSION_UPDATE_REQUEST             MbmsSessionUpdateRequest;
  LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATION_STRUCT MbmsSchedulingInformation;
  // LIBLTE_M2AP_RESET                                   Reset;
  LIBLTE_M2AP_MESSAGE_M2SETUPREQUEST_STRUCT M2SetupRequest;
  // LIBLTE_M2AP_ENB_CONFIGURATION_UPDATE                EnbConfigurationUpdate;
  // LIBLTE_M2AP_MCE_CONFIGURATION_UPDATE                MceConfigurationUpdate;
  // LIBLTE_M2AP_MBMS_SERVICE_COUNTING_REQUEST           MbmsServiceCountingRequest
} LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_UNION;

typedef struct {
  uint8_t                                    procedureCode;
  LIBLTE_M2AP_CRITICALITY_ENUM               criticality;
  LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_UNION choice;
  LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_ENUM  choice_type;
} LIBLTE_M2AP_INITIATINGMESSAGE_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_initiatingmessage(LIBLTE_M2AP_INITIATINGMESSAGE_STRUCT* msg, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_initiatingmessage(uint8_t** ptr, LIBLTE_M2AP_INITIATINGMESSAGE_STRUCT* msg);

/*******************************************************************************
 * UnsuccessfulOutcome CHOICE
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_UNSUCCESSFULOUTCOME_CHOICE_MBMSSESSIONSTARTFAILURE,
  LIBLTE_M2AP_UNSUCCESSFULOUTCOME_CHOICE_MBMSSESSIONUPDATEFAILURE,
  LIBLTE_M2AP_UNSUCCESSFULOUTCOME_CHOICE_M2SETUPFAILURE,
  LIBLTE_M2AP_UNSUCCESSFULOUTCOME_CHOICE_ENBCONFIGURATIONUPDATEFAILURE,
  LIBLTE_M2AP_UNSUCCESSFULOUTCOME_CHOICE_MCECONFIGURATIONUPDATEFAILURE,
  LIBLTE_M2AP_UNSUCCESSFULOUTCOME_CHOICE_MBMSSERVICECOUNTINGFAILURE,
  LIBLTE_M2AP_UNSUCCESSFULOUTCOME_CHOICE_N_ITEMS,
} LIBLTE_M2AP_UNSUCCESSFULOUTCOME_CHOICE_ENUM;
static const char liblte_m2ap_unsuccessfuloutcome_choice_text[LIBLTE_M2AP_UNSUCCESSFULOUTCOME_CHOICE_N_ITEMS][50] = {
    "MbmsSessionStartFailure",
    "Mbmssessionupdatefailure",
    "M2SetupFailure",
    "EnbConfigurationUpdateFailure",
    "MceConfigurationUpdateFailure",
};

typedef union {
  // LIBLTE_S1AP_MESSAGE_S1SETUPFAILURE_STRUCT                           S1SetupFailure;
  // LIBLTE_S1AP_MESSAGE_HANDOVERFAILURE_STRUCT                          HandoverFailure;
  // LIBLTE_S1AP_MESSAGE_MMECONFIGURATIONUPDATEFAILURE_STRUCT            MMEConfigurationUpdateFailure;
} LIBLTE_M2AP_UNSUCCESSFULOUTCOME_CHOICE_UNION;

typedef struct {
  uint8_t                                      procedureCode;
  LIBLTE_M2AP_CRITICALITY_ENUM                 criticality;
  LIBLTE_M2AP_UNSUCCESSFULOUTCOME_CHOICE_UNION choice;
  LIBLTE_M2AP_UNSUCCESSFULOUTCOME_CHOICE_ENUM  choice_type;
} LIBLTE_M2AP_UNSUCCESSFULOUTCOME_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_unsuccessfuloutcome(LIBLTE_M2AP_UNSUCCESSFULOUTCOME_STRUCT* msg, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_unsuccessfuloutcome(uint8_t** ptr, LIBLTE_M2AP_UNSUCCESSFULOUTCOME_STRUCT* msg);

/*******************************************************************************
 * SuccessfulOutcome CHOICE
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_MBMSSESSIONSTARTRESPONSE,
  LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_MBMSSESSIONSTOPRESPONSE,
  LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_MBMSSESSIONUPDATERESPONSE,
  LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_MBMSSCHEDULINGINFORMATIONRESPONSE,
  LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_RESETACKNOWLEDGE,
  LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_M2SETUPRESPONSE,
  LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_ENBCONFIGURATIONUPDATEACKNOWLEDGE,
  LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_MCECONFIGURATIONUPDATEACKNOWLEDGE,
  LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_MBMSSERVICECOUNTINGRESPONSE,
  LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_N_ITEMS,
} LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_ENUM;
static const char liblte_m2ap_successfuloutcome_choice_text[LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_N_ITEMS][50] = {
    "MbmsSessionStartResponse",
    "MbmsSessionStopResponse",
    "MbmsSessionUpdateResponse",
    "MbmsSchedulingInformationResponse",
    "ResetAcknowledge",
    "M2SetupResponse",
    "EnbConfigurationUpdateAcknowledge",
    "MceConfigurationUpdateAcknowledge",
    "MbmsServiceCountingResponse",
};
typedef union {
  LIBLTE_M2AP_MESSAGE_M2SETUPRESPONSE_STRUCT                   M2SetupResponse;
  LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTRESPONSE_STRUCT          MbmsSessionStartResponse;
  LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATIONRESPONSE_STRUCT MbmsSchedulingInformationResponse;
} LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_UNION;

typedef struct {
  uint8_t                                    procedureCode;
  LIBLTE_M2AP_CRITICALITY_ENUM               criticality;
  LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_UNION choice;
  LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_ENUM  choice_type;
} LIBLTE_M2AP_SUCCESSFULOUTCOME_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_successfuloutcome(LIBLTE_M2AP_SUCCESSFULOUTCOME_STRUCT* msg, uint8_t** ptr);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_successfuloutcome(uint8_t** ptr, LIBLTE_M2AP_SUCCESSFULOUTCOME_STRUCT* msg);

/*******************************************************************************
 * M2AP_PDU CHOICE
 *******************************************************************************/
typedef enum {
  LIBLTE_M2AP_M2AP_PDU_CHOICE_INITIATINGMESSAGE,
  LIBLTE_M2AP_M2AP_PDU_CHOICE_SUCCESSFULOUTCOME,
  LIBLTE_M2AP_M2AP_PDU_CHOICE_UNSUCCESSFULOUTCOME,
  LIBLTE_M2AP_M2AP_PDU_CHOICE_N_ITEMS,
} LIBLTE_M2AP_M2AP_PDU_CHOICE_ENUM;
static const char liblte_m2ap_m2ap_pdu_choice_text[LIBLTE_M2AP_M2AP_PDU_CHOICE_N_ITEMS][50] = {
    "initiatingMessage",
    "successfulOutcome",
    "unsuccessfulOutcome",
};

typedef union {
  LIBLTE_M2AP_INITIATINGMESSAGE_STRUCT   initiatingMessage;
  LIBLTE_M2AP_SUCCESSFULOUTCOME_STRUCT   successfulOutcome;
  LIBLTE_M2AP_UNSUCCESSFULOUTCOME_STRUCT unsuccessfulOutcome;
} LIBLTE_M2AP_M2AP_PDU_CHOICE_UNION;

typedef struct {
  bool                              ext;
  LIBLTE_M2AP_M2AP_PDU_CHOICE_UNION choice;
  LIBLTE_M2AP_M2AP_PDU_CHOICE_ENUM  choice_type;
} LIBLTE_M2AP_M2AP_PDU_STRUCT;

LIBLTE_ERROR_ENUM liblte_m2ap_pack_m2ap_pdu(LIBLTE_M2AP_M2AP_PDU_STRUCT* m2ap_pdu, LIBLTE_BYTE_MSG_STRUCT* msg);
LIBLTE_ERROR_ENUM liblte_m2ap_unpack_m2ap_pdu(LIBLTE_BYTE_MSG_STRUCT* msg, LIBLTE_M2AP_M2AP_PDU_STRUCT* m2ap_pdu);

#endif // SRSLTE_LIBLTE_M2AP_H
