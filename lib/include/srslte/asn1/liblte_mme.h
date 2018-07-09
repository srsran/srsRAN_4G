/*******************************************************************************

    Copyright 2014-2015 Ben Wojtowicz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************

    File: liblte_mme.h

    Description: Contains all the definitions for the LTE Mobility Management
                 Entity library.

    Revision History
    ----------    -------------    --------------------------------------------
    06/15/2014    Ben Wojtowicz    Created file.
    08/03/2014    Ben Wojtowicz    Added more decoding/encoding.
    09/03/2014    Ben Wojtowicz    Added more decoding/encoding.
    11/01/2014    Ben Wojtowicz    Added more decoding/encoding.
    11/29/2014    Ben Wojtowicz    Added more decoding/encoding.
    12/16/2014    Ben Wojtowicz    Added more decoding/encoding.
    12/24/2014    Ben Wojtowicz    Cleaned up the Time Zone and Time IE.
    02/15/2015    Ben Wojtowicz    Added more decoding/encoding.

*******************************************************************************/

#ifndef SRSLTE_LIBLTE_MME_H
#define SRSLTE_LIBLTE_MME_H

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include "liblte_common.h"
#include <string>

/*******************************************************************************
                              DEFINES
*******************************************************************************/


/*******************************************************************************
                              TYPEDEFS
*******************************************************************************/


/*******************************************************************************
                              INFORMATION ELEMENT DECLARATIONS
*******************************************************************************/

/*********************************************************************
    IE Name: Additional Information

    Description: Provides additional information to upper layers in
                 relation to the generic NAS message transport
                 mechanism.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.0
*********************************************************************/
// Defines
#define LIBLTE_MME_ADDITIONAL_INFORMATION_MAX_N_OCTETS (LIBLTE_MAX_MSG_SIZE_BITS/2)
// Enums
// Structs
typedef struct{
    uint8  info[LIBLTE_MME_ADDITIONAL_INFORMATION_MAX_N_OCTETS];
    uint32 N_octets;
}LIBLTE_MME_ADDITIONAL_INFORMATION_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_additional_information_ie(LIBLTE_MME_ADDITIONAL_INFORMATION_STRUCT  *add_info,
                                                            uint8                                    **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_additional_information_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_MME_ADDITIONAL_INFORMATION_STRUCT  *add_info);

/*********************************************************************
    IE Name: Device Properties

    Description: Indicates if the UE is configured for NAS signalling
                 low priority.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.0A
                        24.008 v10.2.0 Section 10.5.7.8
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_DEVICE_PROPERTIES_NOT_CONFIGURED_FOR_LOW_PRIORITY = 0,
    LIBLTE_MME_DEVICE_PROPERTIES_CONFIGURED_FOR_LOW_PRIORITY,
    LIBLTE_MME_DEVICE_PROPERTIES_N_ITEMS,
}LIBLTE_MME_DEVICE_PROPERTIES_ENUM;
static const char liblte_mme_device_properties_text[LIBLTE_MME_DEVICE_PROPERTIES_N_ITEMS][50] = {"Not configured for low priority",
                                                                                                 "Configured for low priority"};
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_device_properties_ie(LIBLTE_MME_DEVICE_PROPERTIES_ENUM   device_props,
                                                       uint8                               bit_offset,
                                                       uint8                             **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_device_properties_ie(uint8                             **ie_ptr,
                                                         uint8                               bit_offset,
                                                         LIBLTE_MME_DEVICE_PROPERTIES_ENUM  *device_props);

/*********************************************************************
    IE Name: EPS Bearer Context Status

    Description: Indicates the state of each EPS bearer context that
                 can be identified by an EPS bearer identity.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.1
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    bool ebi[16];
}LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_bearer_context_status_ie(LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_STRUCT  *ebcs,
                                                               uint8                                       **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_bearer_context_status_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_STRUCT  *ebcs);

/*********************************************************************
    IE Name: Location Area Identification

    Description: Provides an unambiguous identification of location
                 areas within the area covered by the 3GPP system.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.2
                        24.008 v10.2.0 Section 10.5.1.3
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint16 mcc;
    uint16 mnc;
    uint16 lac;
}LIBLTE_MME_LOCATION_AREA_ID_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_location_area_id_ie(LIBLTE_MME_LOCATION_AREA_ID_STRUCT  *lai,
                                                      uint8                              **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_location_area_id_ie(uint8                              **ie_ptr,
                                                        LIBLTE_MME_LOCATION_AREA_ID_STRUCT  *lai);

/*********************************************************************
    IE Name: Mobile Identity

    Description: Provides either the IMSI, TMSI/P-TMSI/M-TMSI, IMEI,
                 IMEISV, or TMGI, associated with the optional MBMS
                 session identity.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.3
                        24.008 v10.2.0 Section 10.5.1.4
*********************************************************************/
// Defines
#define LIBLTE_MME_MOBILE_ID_TYPE_IMSI   0x1
#define LIBLTE_MME_MOBILE_ID_TYPE_IMEI   0x2
#define LIBLTE_MME_MOBILE_ID_TYPE_IMEISV 0x3
#define LIBLTE_MME_MOBILE_ID_TYPE_TMSI   0x4
#define LIBLTE_MME_MOBILE_ID_TYPE_TMGI   0x5
// Enums
// Structs
typedef struct{
    uint8 type_of_id;
    uint8 imsi[15];
    uint8 imei[15];
    uint8 imeisv[16];
    uint32 tmsi;
}LIBLTE_MME_MOBILE_ID_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_mobile_id_ie(LIBLTE_MME_MOBILE_ID_STRUCT  *mobile_id,
                                               uint8                       **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_mobile_id_ie(uint8                       **ie_ptr,
                                                 LIBLTE_MME_MOBILE_ID_STRUCT  *mobile_id);

/*********************************************************************
    IE Name: Mobile Station Classmark 2

    Description: Provides the network with information concerning
                 aspects of both high and low priority of the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.4
                        24.008 v10.2.0 Section 10.5.1.6
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_REVISION_LEVEL_GSM_PHASE_1 = 0,
    LIBLTE_MME_REVISION_LEVEL_GSM_PHASE_2,
    LIBLTE_MME_REVISION_LEVEL_R99,
    LIBLTE_MME_REVISION_LEVEL_RESERVED,
    LIBLTE_MME_REVISION_LEVEL_N_ITEMS,
}LIBLTE_MME_REVISION_LEVEL_ENUM;
static const char liblte_mme_revision_level_text[LIBLTE_MME_REVISION_LEVEL_N_ITEMS][20] = {"GSM Phase 1",
                                                                                           "GSM Phase 2",
                                                                                           "R99",
                                                                                           "RESERVED"};
typedef enum{
    LIBLTE_MME_RF_POWER_CAPABILITY_CLASS_1 = 0,
    LIBLTE_MME_RF_POWER_CAPABILITY_CLASS_2,
    LIBLTE_MME_RF_POWER_CAPABILITY_CLASS_3,
    LIBLTE_MME_RF_POWER_CAPABILITY_CLASS_4,
    LIBLTE_MME_RF_POWER_CAPABILITY_CLASS_5,
    LIBLTE_MME_RF_POWER_CAPABILITY_N_ITEMS,
}LIBLTE_MME_RF_POWER_CAPABILITY_ENUM;
static const char liblte_mme_rf_power_capability_text[LIBLTE_MME_RF_POWER_CAPABILITY_N_ITEMS][20] = {"Class 1",
                                                                                                     "Class 2",
                                                                                                     "Class 3",
                                                                                                     "Class 4",
                                                                                                     "Class 5"};
typedef enum{
    LIBLTE_MME_SS_SCREEN_INDICATOR_0 = 0,
    LIBLTE_MME_SS_SCREEN_INDICATOR_1,
    LIBLTE_MME_SS_SCREEN_INDICATOR_2,
    LIBLTE_MME_SS_SCREEN_INDICATOR_3,
    LIBLTE_MME_SS_SCREEN_INDICATOR_N_ITEMS,
}LIBLTE_MME_SS_SCREEN_INDICATOR_ENUM;
static const char liblte_mme_ss_screen_indicator_text[LIBLTE_MME_SS_SCREEN_INDICATOR_N_ITEMS][100] = {"Default Phase 1",
                                                                                                      "Ellipsis Notation Phase 2",
                                                                                                      "RESERVED",
                                                                                                      "RESERVED"};
// Structs
typedef struct{
    LIBLTE_MME_REVISION_LEVEL_ENUM      rev_lev;
    LIBLTE_MME_RF_POWER_CAPABILITY_ENUM rf_power_cap;
    LIBLTE_MME_SS_SCREEN_INDICATOR_ENUM ss_screen_ind;
    bool                                es_ind;
    bool                                a5_1;
    bool                                ps_cap;
    bool                                sm_cap;
    bool                                vbs;
    bool                                vgcs;
    bool                                fc;
    bool                                cm3;
    bool                                lcsva_cap;
    bool                                ucs2;
    bool                                solsa;
    bool                                cmsp;
    bool                                a5_3;
    bool                                a5_2;
}LIBLTE_MME_MOBILE_STATION_CLASSMARK_2_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_mobile_station_classmark_2_ie(LIBLTE_MME_MOBILE_STATION_CLASSMARK_2_STRUCT  *ms_cm2,
                                                                uint8                                        **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_mobile_station_classmark_2_ie(uint8                                        **ie_ptr,
                                                                  LIBLTE_MME_MOBILE_STATION_CLASSMARK_2_STRUCT  *ms_cm2);

/*********************************************************************
    IE Name: Mobile Station Classmark 3

    Description: Provides the network with information concerning
                 aspects of the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.5
                        24.008 v10.2.0 Section 10.5.1.7
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    // FIXME
}LIBLTE_MME_MOBILE_STATION_CLASSMARK_3_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_mobile_station_classmark_3_ie(LIBLTE_MME_MOBILE_STATION_CLASSMARK_3_STRUCT  *ms_cm3,
                                                                uint8                                        **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_mobile_station_classmark_3_ie(uint8                                        **ie_ptr,
                                                                  LIBLTE_MME_MOBILE_STATION_CLASSMARK_3_STRUCT  *ms_cm3);

/*********************************************************************
    IE Name: NAS Security Parameters From E-UTRA

    Description: Provides the UE with information that enables the UE
                 to create a mapped UMTS security context.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.6
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_nas_security_parameters_from_eutra_ie(uint8   dl_nas_count,
                                                                        uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_nas_security_parameters_from_eutra_ie(uint8 **ie_ptr,
                                                                          uint8  *dl_nas_count);

/*********************************************************************
    IE Name: NAS Security Parameters To E-UTRA

    Description: Provides the UE with parameters that enables the UE
                 to create a mapped EPS security context and take
                 this context into use after inter-system handover to
                 S1 mode.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.7
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_EIA0 = 0,
    LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_128_EIA1,
    LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_128_EIA2,
    LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_EIA3,
    LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_EIA4,
    LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_EIA5,
    LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_EIA6,
    LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_EIA7,
    LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_N_ITEMS,
}LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_ENUM;
static const char liblte_mme_type_of_integrity_algorithm_text[LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_N_ITEMS][20] = {"EIA0",
                                                                                                                     "128-EIA1",
                                                                                                                     "128-EIA2",
                                                                                                                     "EIA3",
                                                                                                                     "EIA4",
                                                                                                                     "EIA5",
                                                                                                                     "EIA6",
                                                                                                                     "EIA7"};
typedef enum{
    LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_EEA0 = 0,
    LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_128_EEA1,
    LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_128_EEA2,
    LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_EEA3,
    LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_EEA4,
    LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_EEA5,
    LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_EEA6,
    LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_EEA7,
    LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_N_ITEMS,
}LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_ENUM;
static const char liblte_mme_type_of_ciphering_algorithm_text[LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_N_ITEMS][20] = {"EEA0",
                                                                                                                     "128-EEA1",
                                                                                                                     "128-EEA2",
                                                                                                                     "EEA3",
                                                                                                                     "EEA4",
                                                                                                                     "EEA5",
                                                                                                                     "EEA6",
                                                                                                                     "EEA7"};
typedef enum{
    LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_NATIVE = 0,
    LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_MAPPED,
    LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_N_ITEMS,
}LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_ENUM;
static const char liblte_mme_type_of_security_context_flag_text[LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_N_ITEMS][20] = {"Native",
                                                                                                                         "Mapped"};
// Structs
typedef struct{
    LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_ENUM   eea;
    LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_ENUM   eia;
    LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_ENUM tsc_flag;
    uint32                                        nonce_mme;
    uint8                                         nas_ksi;
}LIBLTE_MME_NAS_SECURITY_PARAMETERS_TO_EUTRA_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_nas_security_parameters_to_eutra_ie(LIBLTE_MME_NAS_SECURITY_PARAMETERS_TO_EUTRA_STRUCT  *sec_params,
                                                                      uint8                                              **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_nas_security_parameters_to_eutra_ie(uint8                                              **ie_ptr,
                                                                        LIBLTE_MME_NAS_SECURITY_PARAMETERS_TO_EUTRA_STRUCT  *sec_params);

/*********************************************************************
    IE Name: PLMN List

    Description: Provides a list of PLMN codes to the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.8
                        24.008 v10.2.0 Section 10.5.1.13
*********************************************************************/
// Defines
#define LIBLTE_MME_PLMN_LIST_MAX_SIZE 15
// Enums
// Structs
typedef struct{
    uint32 N_plmns;
    uint16 mcc[LIBLTE_MME_PLMN_LIST_MAX_SIZE];
    uint16 mnc[LIBLTE_MME_PLMN_LIST_MAX_SIZE];
}LIBLTE_MME_PLMN_LIST_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_plmn_list_ie(LIBLTE_MME_PLMN_LIST_STRUCT  *plmn_list,
                                               uint8                       **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_plmn_list_ie(uint8                       **ie_ptr,
                                                 LIBLTE_MME_PLMN_LIST_STRUCT  *plmn_list);

/*********************************************************************
    IE Name: Spare Half Octet

    Description: Used in the description of EMM and ESM messages when
                 an odd number of half octet type 1 information
                 elements are used.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.9
*********************************************************************/
// Defines
// Enums
// Structs
// Functions

/*********************************************************************
    IE Name: Supported Codec List

    Description: Provides the network with information about the
                 speech codecs supported by the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.2.10
                        24.008 v10.2.0 Section 10.5.4.32
*********************************************************************/
// Defines
#define LIBLTE_MME_MAX_N_SUPPORTED_CODECS (LIBLTE_MAX_MSG_SIZE_BITS/16)
// Enums
// Structs
typedef struct{
    uint8  sys_id;
    uint16 codec_bitmap;
}LIBLTE_MME_SUPPORTED_CODEC_STRUCT;
typedef struct{
    LIBLTE_MME_SUPPORTED_CODEC_STRUCT supported_codec[LIBLTE_MME_MAX_N_SUPPORTED_CODECS];
    uint32                            N_supported_codecs;
}LIBLTE_MME_SUPPORTED_CODEC_LIST_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_supported_codec_list_ie(LIBLTE_MME_SUPPORTED_CODEC_LIST_STRUCT  *supported_codec_list,
                                                          uint8                                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_supported_codec_list_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_MME_SUPPORTED_CODEC_LIST_STRUCT  *supported_codec_list);

/*********************************************************************
    IE Name: Additional Update Result

    Description: Provides additional information about the result of
                 a combined attached procedure or a combined tracking
                 area updating procedure.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.0A
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_NO_ADDITIONAL_INFO = 0,
    LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_CS_FALLBACK_NOT_PREFERRED,
    LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_SMS_ONLY,
    LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_RESERVED,
    LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_N_ITEMS,
}LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_ENUM;
static const char liblte_mme_additional_update_result_text[LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_N_ITEMS][100] = {"No Additional Information",
                                                                                                                "CS Fallback Not Preferred",
                                                                                                                "SMS Only",
                                                                                                                "RESERVED"};
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_additional_update_result_ie(LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_ENUM   result,
                                                              uint8                                      bit_offset,
                                                              uint8                                    **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_additional_update_result_ie(uint8                                    **ie_ptr,
                                                                uint8                                      bit_offset,
                                                                LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_ENUM  *result);

/*********************************************************************
    IE Name: Additional Update Type

    Description: Provides additional information about the type of
                 request for a combined attach or a combined tracking
                 area updating procedure.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.0B
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_NO_ADDITIONAL_INFO = 0,
    LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_SMS_ONLY,
    LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_N_ITEMS,
}LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_ENUM;
static const char liblte_mme_additional_update_type_text[LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_N_ITEMS][20] = {"No additional info",
                                                                                                           "SMS Only"};
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_additional_update_type_ie(LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_ENUM   aut,
                                                            uint8                                    bit_offset,
                                                            uint8                                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_additional_update_type_ie(uint8                                  **ie_ptr,
                                                              uint8                                    bit_offset,
                                                              LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_ENUM  *aut);

/*********************************************************************
    IE Name: Authentication Failure Parameter

    Description: Provides the network with the necessary information
                 to begin a re-authentication procedure in the case
                 of a 'Synch failure', following a UMTS or EPS
                 authentication challenge.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.1
                        24.008 v10.2.0 Section 10.5.3.2.2
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_failure_parameter_ie(uint8  *auth_fail_param,
                                                                      uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_failure_parameter_ie(uint8 **ie_ptr,
                                                                        uint8  *auth_fail_param);

/*********************************************************************
    IE Name: Authentication Parameter AUTN

    Description: Provides the UE with a means of authenticating the
                 network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.2
                        24.008 v10.2.0 Section 10.5.3.1.1
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_parameter_autn_ie(uint8  *autn,
                                                                   uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_parameter_autn_ie(uint8 **ie_ptr,
                                                                     uint8  *autn);

/*********************************************************************
    IE Name: Authentication Parameter RAND

    Description: Provides the UE with a non-predictable number to be
                 used to calculate the authentication signature SRES
                 and the ciphering key Kc (for a GSM authentication
                 challenge), or the response RES and both the
                 ciphering key CK and the integrity key IK (for a
                 UMTS authentication challenge).

    Document Reference: 24.301 v10.2.0 Section 9.9.3.3
                        24.008 v10.2.0 Section 10.5.3.1
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_parameter_rand_ie(uint8  *rand_val,
                                                                   uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_parameter_rand_ie(uint8 **ie_ptr,
                                                                     uint8  *rand_val);

/*********************************************************************
    IE Name: Authentication Response Parameter

    Description: Provides the network with the authentication
                 response calculated in the USIM.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_response_parameter_ie(uint8  *res,
                                                                       uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_response_parameter_ie(uint8 **ie_ptr,
                                                                         uint8  *res);

/*********************************************************************
    IE Name: Ciphering Key Sequence Number

    Description: Makes it possible for the network to identify the
                 ciphering key Kc which is stored in the UE without
                 invoking the authentication procedure.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.4A
                        24.008 v10.2.0 Section 10.5.1.2
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_ciphering_key_sequence_number_ie(uint8   key_seq,
                                                                   uint8   bit_offset,
                                                                   uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_ciphering_key_sequence_number_ie(uint8 **ie_ptr,
                                                                     uint8   bit_offset,
                                                                     uint8  *key_seq);

/*********************************************************************
    IE Name: CSFB Response

    Description: Indicates whether the UE accepts or rejects a paging
                 for CS fallback.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.5
*********************************************************************/
// Defines
#define LIBLTE_MME_CSFB_REJECTED_BY_THE_UE 0x0
#define LIBLTE_MME_CSFB_ACCEPTED_BY_THE_UE 0x1
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_csfb_response_ie(uint8   csfb_resp,
                                                   uint8   bit_offset,
                                                   uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_csfb_response_ie(uint8 **ie_ptr,
                                                     uint8   bit_offset,
                                                     uint8  *csfb_resp);

/*********************************************************************
    IE Name: Daylight Saving Time

    Description: Encodes the daylight saving time in steps of 1 hour.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.6
                        24.008 v10.2.0 Section 10.5.3.12
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_DAYLIGHT_SAVING_TIME_NO_ADJUSTMENT = 0,
    LIBLTE_MME_DAYLIGHT_SAVING_TIME_PLUS_ONE_HOUR,
    LIBLTE_MME_DAYLIGHT_SAVING_TIME_PLUS_TWO_HOURS,
    LIBLTE_MME_DAYLIGHT_SAVING_TIME_RESERVED,
    LIBLTE_MME_DAYLIGHT_SAVING_TIME_N_ITEMS,
}LIBLTE_MME_DAYLIGHT_SAVING_TIME_ENUM;
static const char liblte_mme_daylight_saving_time_text[LIBLTE_MME_DAYLIGHT_SAVING_TIME_N_ITEMS][20] = {"No Adjustment",
                                                                                                       "+1 Hour",
                                                                                                       "+2 Hours",
                                                                                                       "RESERVED"};
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_daylight_saving_time_ie(LIBLTE_MME_DAYLIGHT_SAVING_TIME_ENUM   dst,
                                                          uint8                                **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_daylight_saving_time_ie(uint8                                **ie_ptr,
                                                            LIBLTE_MME_DAYLIGHT_SAVING_TIME_ENUM  *dst);

/*********************************************************************
    IE Name: Detach Type

    Description: Indicates the type of detach.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.7
*********************************************************************/
// Defines
#define LIBLTE_MME_SO_FLAG_NORMAL_DETACH        0
#define LIBLTE_MME_SO_FLAG_SWITCH_OFF           1
#define LIBLTE_MME_TOD_UL_EPS_DETACH            0x1
#define LIBLTE_MME_TOD_UL_IMSI_DETACH           0x2
#define LIBLTE_MME_TOD_UL_COMBINED_DETACH       0x3
#define LIBLTE_MME_TOD_DL_REATTACH_REQUIRED     0x1
#define LIBLTE_MME_TOD_DL_REATTACH_NOT_REQUIRED 0x2
#define LIBLTE_MME_TOD_DL_IMSI_DETACH           0x3
// Enums
// Structs
typedef struct{
    uint8 switch_off;
    uint8 type_of_detach;
}LIBLTE_MME_DETACH_TYPE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_detach_type_ie(LIBLTE_MME_DETACH_TYPE_STRUCT  *detach_type,
                                                 uint8                           bit_offset,
                                                 uint8                         **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_detach_type_ie(uint8                         **ie_ptr,
                                                   uint8                           bit_offset,
                                                   LIBLTE_MME_DETACH_TYPE_STRUCT  *detach_type);

/*********************************************************************
    IE Name: DRX Parameter

    Description: Indicates whether the UE uses DRX mode or not.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.8
                        24.008 v10.2.0 Section 10.5.5.6
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_NON_DRX_TIMER_NO_NON_DRX_MODE = 0,
    LIBLTE_MME_NON_DRX_TIMER_MAX_1S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_MAX_2S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_MAX_4S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_MAX_8S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_MAX_16S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_MAX_32S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_MAX_64S_NON_DRX_MODE,
    LIBLTE_MME_NON_DRX_TIMER_N_ITEMS,
}LIBLTE_MME_NON_DRX_TIMER_ENUM;
static const char liblte_mme_non_drx_timer_text[LIBLTE_MME_NON_DRX_TIMER_N_ITEMS][100] = {"No Non-DRX Mode",
                                                                                          "Max 1s Non-DRX Mode",
                                                                                          "Max 2s Non-DRX Mode",
                                                                                          "Max 4s Non-DRX Mode",
                                                                                          "Max 8s Non-DRX Mode",
                                                                                          "Max 16s Non-DRX Mode",
                                                                                          "Max 32s Non-DRX Mode",
                                                                                          "Max 64s Non-DRX Mode"};
// Structs
typedef struct{
    LIBLTE_MME_NON_DRX_TIMER_ENUM non_drx_timer;
    uint8                         split_pg_cycle_code;
    uint8                         drx_cycle_len_coeff_and_value;
    bool                          split_on_ccch;
}LIBLTE_MME_DRX_PARAMETER_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_drx_parameter_ie(LIBLTE_MME_DRX_PARAMETER_STRUCT  *drx_param,
                                                   uint8                           **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_drx_parameter_ie(uint8                           **ie_ptr,
                                                     LIBLTE_MME_DRX_PARAMETER_STRUCT  *drx_param);

/*********************************************************************
    IE Name: EMM Cause

    Description: Indicates the reason why an EMM request from the UE
                 is rejected by the network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.9
*********************************************************************/
// Defines
#define LIBLTE_MME_EMM_CAUSE_IMSI_UNKNOWN_IN_HSS                                 0x02
#define LIBLTE_MME_EMM_CAUSE_ILLEGAL_UE                                          0x03
#define LIBLTE_MME_EMM_CAUSE_IMEI_NOT_ACCEPTED                                   0x05
#define LIBLTE_MME_EMM_CAUSE_ILLEGAL_ME                                          0x06
#define LIBLTE_MME_EMM_CAUSE_EPS_SERVICES_NOT_ALLOWED                            0x07
#define LIBLTE_MME_EMM_CAUSE_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED       0x08
#define LIBLTE_MME_EMM_CAUSE_UE_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK        0x09
#define LIBLTE_MME_EMM_CAUSE_IMPLICITLY_DETACHED                                 0x0A
#define LIBLTE_MME_EMM_CAUSE_PLMN_NOT_ALLOWED                                    0x0B
#define LIBLTE_MME_EMM_CAUSE_TRACKING_AREA_NOT_ALLOWED                           0x0C
#define LIBLTE_MME_EMM_CAUSE_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA           0x0D
#define LIBLTE_MME_EMM_CAUSE_EPS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN               0x0E
#define LIBLTE_MME_EMM_CAUSE_NO_SUITABLE_CELLS_IN_TRACKING_AREA                  0x0F
#define LIBLTE_MME_EMM_CAUSE_MSC_TEMPORARILY_NOT_REACHABLE                       0x10
#define LIBLTE_MME_EMM_CAUSE_NETWORK_FAILURE                                     0x11
#define LIBLTE_MME_EMM_CAUSE_CS_DOMAIN_NOT_AVAILABLE                             0x12
#define LIBLTE_MME_EMM_CAUSE_ESM_FAILURE                                         0x13
#define LIBLTE_MME_EMM_CAUSE_MAC_FAILURE                                         0x14
#define LIBLTE_MME_EMM_CAUSE_SYNCH_FAILURE                                       0x15
#define LIBLTE_MME_EMM_CAUSE_CONGESTION                                          0x16
#define LIBLTE_MME_EMM_CAUSE_UE_SECURITY_CAPABILITIES_MISMATCH                   0x17
#define LIBLTE_MME_EMM_CAUSE_SECURITY_MODE_REJECTED_UNSPECIFIED                  0x18
#define LIBLTE_MME_EMM_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG                         0x19
#define LIBLTE_MME_EMM_CAUSE_NON_EPS_AUTHENTICATION_UNACCEPTABLE                 0x1A
#define LIBLTE_MME_EMM_CAUSE_CS_SERVICE_TEMPORARILY_NOT_AVAILABLE                0x27
#define LIBLTE_MME_EMM_CAUSE_NO_EPS_BEARER_CONTEXT_ACTIVATED                     0x28
#define LIBLTE_MME_EMM_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE                      0x5F
#define LIBLTE_MME_EMM_CAUSE_INVALID_MANDATORY_INFORMATION                       0x60
#define LIBLTE_MME_EMM_CAUSE_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED        0x61
#define LIBLTE_MME_EMM_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE 0x62
#define LIBLTE_MME_EMM_CAUSE_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED 0x63
#define LIBLTE_MME_EMM_CAUSE_CONDITIONAL_IE_ERROR                                0x64
#define LIBLTE_MME_EMM_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE      0x65
#define LIBLTE_MME_EMM_CAUSE_PROTOCOL_ERROR_UNSPECIFIED                          0x6F
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_emm_cause_ie(uint8   emm_cause,
                                               uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_emm_cause_ie(uint8 **ie_ptr,
                                                 uint8  *emm_cause);

/*********************************************************************
    IE Name: EPS Attach Result

    Description: Specifies the result of an attach procedure.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.10
*********************************************************************/
// Defines
#define LIBLTE_MME_EPS_ATTACH_RESULT_EPS_ONLY                 0x1
#define LIBLTE_MME_EPS_ATTACH_RESULT_COMBINED_EPS_IMSI_ATTACH 0x2
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_attach_result_ie(uint8   result,
                                                       uint8   bit_offset,
                                                       uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_attach_result_ie(uint8 **ie_ptr,
                                                         uint8   bit_offset,
                                                         uint8  *result);

/*********************************************************************
    IE Name: EPS Attach Type

    Description: Indicates the type of the requested attach.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.11
*********************************************************************/
// Defines
#define LIBLTE_MME_EPS_ATTACH_TYPE_EPS_ATTACH               0x1
#define LIBLTE_MME_EPS_ATTACH_TYPE_COMBINED_EPS_IMSI_ATTACH 0x2
#define LIBLTE_MME_EPS_ATTACH_TYPE_EPS_EMERGENCY_ATTACH     0x6
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_attach_type_ie(uint8   attach_type,
                                                     uint8   bit_offset,
                                                     uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_attach_type_ie(uint8 **ie_ptr,
                                                       uint8   bit_offset,
                                                       uint8  *attach_type);

/*********************************************************************
    IE Name: EPS Mobile Identity

    Description: Provides either the IMSI, the GUTI, or the IMEI.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.12
*********************************************************************/
// Defines
#define LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI 0x1
#define LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI 0x6
#define LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMEI 0x3
// Enums
// Structs
typedef struct{
    uint32 m_tmsi;
    uint16 mcc;
    uint16 mnc;
    uint16 mme_group_id;
    uint8  mme_code;
}LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT;
typedef struct{
    LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT guti;
    uint8                                type_of_id;
    uint8                                imsi[15];
    uint8                                imei[15];
}LIBLTE_MME_EPS_MOBILE_ID_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_mobile_id_ie(LIBLTE_MME_EPS_MOBILE_ID_STRUCT  *eps_mobile_id,
                                                   uint8                           **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_mobile_id_ie(uint8                           **ie_ptr,
                                                     LIBLTE_MME_EPS_MOBILE_ID_STRUCT  *eps_mobile_id);

/*********************************************************************
    IE Name: EPS Network Feature Support

    Description: Indicates whether certain features are supported by
                 the network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.12A
*********************************************************************/
// Defines
#define LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_NOT_SUPPORTED 0
#define LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_SUPPORTED     1
// Enums
typedef enum{
    LIBLTE_MME_CS_LCS_NO_INFORMATION_AVAILABLE = 0,
    LIBLTE_MME_CS_LCS_NOT_SUPPORTED,
    LIBLTE_MME_CS_LCS_SUPPORTED,
    LIBLTE_MME_CS_LCS_RESERVED,
    LIBLTE_MME_CS_LCS_N_ITEMS,
}LIBLTE_MME_CS_LCS_ENUM;
static const char liblte_mme_cs_lcs_text[LIBLTE_MME_CS_LCS_N_ITEMS][100] = {"No Information Available",
                                                                            "Not Supported",
                                                                            "Supported",
                                                                            "RESERVED"};
// Structs
typedef struct{
    LIBLTE_MME_CS_LCS_ENUM cs_lcs;
    bool                   esrps;
    bool                   epc_lcs;
    bool                   emc_bs;
    bool                   ims_vops;
}LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_network_feature_support_ie(LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_STRUCT  *eps_nfs,
                                                                 uint8                                         **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_network_feature_support_ie(uint8                                         **ie_ptr,
                                                                   LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_STRUCT  *eps_nfs);

/*********************************************************************
    IE Name: EPS Update Result

    Description: Specifies the result of the associated updating
                 procedure.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.13
*********************************************************************/
// Defines
#define LIBLTE_MME_EPS_UPDATE_RESULT_TA_UPDATED                               0x0
#define LIBLTE_MME_EPS_UPDATE_RESULT_COMBINED_TA_LA_UPDATED                   0x1
#define LIBLTE_MME_EPS_UPDATE_RESULT_TA_UPDATED_AND_ISR_ACTIVATED             0x4
#define LIBLTE_MME_EPS_UPDATE_RESULT_COMBINED_TA_LA_UPDATED_AND_ISR_ACTIVATED 0x5
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_update_result_ie(uint8   eps_update_res,
                                                       uint8   bit_offset,
                                                       uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_update_result_ie(uint8 **ie_ptr,
                                                         uint8   bit_offset,
                                                         uint8  *eps_update_res);

/*********************************************************************
    IE Name: EPS Update Type

    Description: Specifies the area the updating procedure is
                 associated with.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.14
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_EPS_UPDATE_TYPE_TA_UPDATING = 0,
    LIBLTE_MME_EPS_UPDATE_TYPE_COMBINED_TA_LA_UPDATING,
    LIBLTE_MME_EPS_UPDATE_TYPE_COMBINED_TA_LA_UPDATING_WITH_IMSI_ATTACH,
    LIBLTE_MME_EPS_UPDATE_TYPE_PERIODIC_UPDATING,
    LIBLTE_MME_EPS_UPDATE_TYPE_N_ITEMS,
}LIBLTE_MME_EPS_UPDATE_TYPE_ENUM;
static const char liblte_mme_eps_update_type_text[LIBLTE_MME_EPS_UPDATE_TYPE_N_ITEMS][100] = {"TA Updating",
                                                                                              "Combined TA/LA Updating",
                                                                                              "Combined TA/LA Updating With IMSI Attach",
                                                                                              "Periodic Updating"};
// Structs
typedef struct{
    LIBLTE_MME_EPS_UPDATE_TYPE_ENUM type;
    bool                            active_flag;
}LIBLTE_MME_EPS_UPDATE_TYPE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_update_type_ie(LIBLTE_MME_EPS_UPDATE_TYPE_STRUCT  *eps_update_type,
                                                     uint8                               bit_offset,
                                                     uint8                             **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_update_type_ie(uint8                             **ie_ptr,
                                                       uint8                               bit_offset,
                                                       LIBLTE_MME_EPS_UPDATE_TYPE_STRUCT  *eps_update_type);

/*********************************************************************
    IE Name: ESM Message Container

    Description: Enables piggybacked transfer of a single ESM message
                 within an EMM message.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.15
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_esm_message_container_ie(LIBLTE_BYTE_MSG_STRUCT  *esm_msg,
                                                           uint8                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_message_container_ie(uint8                  **ie_ptr,
                                                             LIBLTE_BYTE_MSG_STRUCT  *esm_msg);

/*********************************************************************
    IE Name: GPRS Timer

    Description: Specifies GPRS specific timer values.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.16
                        24.008 v10.2.0 Section 10.5.7.3
*********************************************************************/
// Defines
#define LIBLTE_MME_GPRS_TIMER_UNIT_2_SECONDS 0x0
#define LIBLTE_MME_GPRS_TIMER_UNIT_1_MINUTE  0x1
#define LIBLTE_MME_GPRS_TIMER_UNIT_6_MINUTES 0x2
#define LIBLTE_MME_GPRS_TIMER_DEACTIVATED    0x7
// Enums
// Structs
typedef struct{
    uint8 unit;
    uint8 value;
}LIBLTE_MME_GPRS_TIMER_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_gprs_timer_ie(LIBLTE_MME_GPRS_TIMER_STRUCT  *timer,
                                                uint8                        **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_gprs_timer_ie(uint8                        **ie_ptr,
                                                  LIBLTE_MME_GPRS_TIMER_STRUCT  *timer);

/*********************************************************************
    IE Name: GPRS Timer 2

    Description: Specifies GPRS specific timer values.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.16A
                        24.008 v10.2.0 Section 10.5.7.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_gprs_timer_2_ie(uint8   value,
                                                  uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_gprs_timer_2_ie(uint8 **ie_ptr,
                                                    uint8  *value);

/*********************************************************************
    IE Name: GPRS Timer 3

    Description: Specifies GPRS specific timer values.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.16B
                        24.008 v10.2.0 Section 10.5.7.4A
*********************************************************************/
// Defines
#define LIBLTE_MME_GPRS_TIMER_3_UNIT_10_MINUTES 0x0
#define LIBLTE_MME_GPRS_TIMER_3_UNIT_1_HOUR     0x1
#define LIBLTE_MME_GPRS_TIMER_3_UNIT_10_HOURS   0x2
#define LIBLTE_MME_GPRS_TIMER_3_DEACTIVATED     0x7
// Enums
// Structs
typedef struct{
    uint8 unit;
    uint8 value;
}LIBLTE_MME_GPRS_TIMER_3_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_gprs_timer_3_ie(LIBLTE_MME_GPRS_TIMER_3_STRUCT  *timer,
                                                  uint8                          **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_gprs_timer_3_ie(uint8                          **ie_ptr,
                                                    LIBLTE_MME_GPRS_TIMER_3_STRUCT  *timer);

/*********************************************************************
    IE Name: Identity Type 2

    Description: Specifies which identity is requested.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.17
                        24.008 v10.2.0 Section 10.5.5.9
*********************************************************************/
// Defines
#define LIBLTE_MME_ID_TYPE_2_IMSI   0x1
#define LIBLTE_MME_ID_TYPE_2_IMEI   0x2
#define LIBLTE_MME_ID_TYPE_2_IMEISV 0x3
#define LIBLTE_MME_ID_TYPE_2_TMSI   0x4
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_identity_type_2_ie(uint8   id_type,
                                                     uint8   bit_offset,
                                                     uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_identity_type_2_ie(uint8 **ie_ptr,
                                                       uint8   bit_offset,
                                                       uint8  *id_type);

/*********************************************************************
    IE Name: IMEISV Request

    Description: Indicates that the IMEISV shall be included by the
                 UE in the authentication and ciphering response
                 message.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.18
                        24.008 v10.2.0 Section 10.5.5.10
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_IMEISV_NOT_REQUESTED = 0,
    LIBLTE_MME_IMEISV_REQUESTED,
    LIBLTE_MME_IMEISV_REQUEST_N_ITEMS,
}LIBLTE_MME_IMEISV_REQUEST_ENUM;
static const char liblte_mme_imeisv_request_text[LIBLTE_MME_IMEISV_REQUEST_N_ITEMS][20] = {"Not Requested",
                                                                                           "Requested"};
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_imeisv_request_ie(LIBLTE_MME_IMEISV_REQUEST_ENUM   imeisv_req,
                                                    uint8                            bit_offset,
                                                    uint8                          **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_imeisv_request_ie(uint8                          **ie_ptr,
                                                      uint8                            bit_offset,
                                                      LIBLTE_MME_IMEISV_REQUEST_ENUM  *imeisv_req);

/*********************************************************************
    IE Name: KSI And Sequence Number

    Description: Provides the network with the key set identifier
                 (KSI) value of the current EPS security context and
                 the 5 least significant bits of the NAS COUNT value
                 applicable for the message including this information
                 element.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.19
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 ksi;
    uint8 seq_num;
}LIBLTE_MME_KSI_AND_SEQUENCE_NUMBER_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_ksi_and_sequence_number_ie(LIBLTE_MME_KSI_AND_SEQUENCE_NUMBER_STRUCT  *ksi_and_seq_num,
                                                             uint8                                     **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_ksi_and_sequence_number_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_MME_KSI_AND_SEQUENCE_NUMBER_STRUCT  *ksi_and_seq_num);

/*********************************************************************
    IE Name: MS Network Capability

    Description: Provides the network with information concerning
                 aspects of the UE related to GPRS.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.20
                        24.008 v10.2.0 Section 10.5.5.12
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_SS_SCREENING_INDICATOR_PHASE_1 = 0,
    LIBLTE_MME_SS_SCREENING_INDICATOR_PHASE_2,
    LIBLTE_MME_SS_SCREENING_INDICATOR_RESERVED_1,
    LIBLTE_MME_SS_SCREENING_INDICATOR_RESERVED_2,
    LIBLTE_MME_SS_SCREENING_INDICATOR_N_ITEMS,
}LIBLTE_MME_SS_SCREENING_INDICATOR_ENUM;
static const char liblte_mme_ss_screening_indicator_text[LIBLTE_MME_SS_SCREENING_INDICATOR_N_ITEMS][20] = {"Phase 1",
                                                                                                           "Phase 2",
                                                                                                           "Reserved 1",
                                                                                                           "Reserved 2"};
// Structs
typedef struct{
    LIBLTE_MME_SS_SCREENING_INDICATOR_ENUM ss_screening;
    bool                                   gea[8];
    bool                                   sm_cap_ded;
    bool                                   sm_cap_gprs;
    bool                                   ucs2;
    bool                                   solsa;
    bool                                   revision;
    bool                                   pfc;
    bool                                   lcsva;
    bool                                   ho_g2u_via_iu;
    bool                                   ho_g2e_via_s1;
    bool                                   emm_comb;
    bool                                   isr;
    bool                                   srvcc;
    bool                                   epc;
    bool                                   nf;
}LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_ms_network_capability_ie(LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT  *ms_network_cap,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_ms_network_capability_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT  *ms_network_cap);

/*********************************************************************
    IE Name: NAS Key Set Identifier

    Description: Provides the NAS key set identifier that is allocated
                 by the network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.21
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_TYPE_OF_SECURITY_CONTEXT_FLAG_ENUM tsc_flag;
    uint8                                         nas_ksi;
}LIBLTE_MME_NAS_KEY_SET_ID_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_nas_key_set_id_ie(LIBLTE_MME_NAS_KEY_SET_ID_STRUCT  *nas_ksi,
                                                    uint8                              bit_offset,
                                                    uint8                            **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_nas_key_set_id_ie(uint8                            **ie_ptr,
                                                      uint8                              bit_offset,
                                                      LIBLTE_MME_NAS_KEY_SET_ID_STRUCT  *nas_ksi);

/*********************************************************************
    IE Name: NAS Message Container

    Description: Encapsulates the SMS messages transferred between
                 the UE and the network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.22
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_nas_message_container_ie(LIBLTE_BYTE_MSG_STRUCT  *nas_msg,
                                                           uint8                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_nas_message_container_ie(uint8                  **ie_ptr,
                                                             LIBLTE_BYTE_MSG_STRUCT  *nas_msg);

/*********************************************************************
    IE Name: NAS Security Algorithms

    Description: Indicates the algorithms to be used for ciphering
                 and integrity protection.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.23
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_TYPE_OF_CIPHERING_ALGORITHM_ENUM type_of_eea;
    LIBLTE_MME_TYPE_OF_INTEGRITY_ALGORITHM_ENUM type_of_eia;
}LIBLTE_MME_NAS_SECURITY_ALGORITHMS_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_nas_security_algorithms_ie(LIBLTE_MME_NAS_SECURITY_ALGORITHMS_STRUCT  *nas_sec_algs,
                                                             uint8                                     **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_nas_security_algorithms_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_MME_NAS_SECURITY_ALGORITHMS_STRUCT  *nas_sec_algs);

/*********************************************************************
    IE Name: Network Name

    Description: Passes a text string to the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.24
                        24.008 v10.2.0 Section 10.5.3.5A
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_ADD_CI_DONT_ADD = 0,
    LIBLTE_MME_ADD_CI_ADD,
    LIBLTE_MME_ADD_CI_N_ITEMS,
}LIBLTE_MME_ADD_CI_ENUM;
static const char liblte_mme_add_ci_text[LIBLTE_MME_ADD_CI_N_ITEMS][20] = {"Don't add",
                                                                           "Add"};
// Structs
typedef struct{
    char                   name[LIBLTE_STRING_LEN];
    LIBLTE_MME_ADD_CI_ENUM add_ci;
}LIBLTE_MME_NETWORK_NAME_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_network_name_ie(LIBLTE_MME_NETWORK_NAME_STRUCT  *net_name,
                                                  uint8                          **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_network_name_ie(uint8                          **ie_ptr,
                                                    LIBLTE_MME_NETWORK_NAME_STRUCT  *net_name);

/*********************************************************************
    IE Name: Nonce

    Description: Transfers a 32-bit nonce value to support deriving
                 a new mapped EPS security context.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.25
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_nonce_ie(uint32   nonce,
                                           uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_nonce_ie(uint8  **ie_ptr,
                                             uint32  *nonce);

/*********************************************************************
    IE Name: Paging Identity

    Description: Indicates the identity used for paging for non-EPS
                 services.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.25A
*********************************************************************/
// Defines
#define LIBLTE_MME_PAGING_IDENTITY_IMSI 0
#define LIBLTE_MME_PAGING_IDENTITY_TMSI 1
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_paging_identity_ie(uint8   paging_id,
                                                     uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_paging_identity_ie(uint8 **ie_ptr,
                                                       uint8  *paging_id);

/*********************************************************************
    IE Name: P-TMSI Signature

    Description: Identifies a GMM context of a UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.26
                        24.008 v10.2.0 Section 10.5.5.8
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_p_tmsi_signature_ie(uint32   p_tmsi_signature,
                                                      uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_p_tmsi_signature_ie(uint8  **ie_ptr,
                                                        uint32  *p_tmsi_signature);

/*********************************************************************
    IE Name: Service Type

    Description: Specifies the purpose of the service request
                 procedure.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.27
*********************************************************************/
// Defines
#define LIBLTE_MME_SERVICE_TYPE_MO_CSFB           0x0
#define LIBLTE_MME_SERVICE_TYPE_MT_CSFB           0x1
#define LIBLTE_MME_SERVICE_TYPE_MO_CSFB_EMERGENCY 0x2
#define LIBLTE_MME_SERVICE_TYPE_PACKET_SERVICES   0x8
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_service_type_ie(uint8   value,
                                                  uint8   bit_offset,
                                                  uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_service_type_ie(uint8 **ie_ptr,
                                                    uint8   bit_offset,
                                                    uint8  *value);

/*********************************************************************
    IE Name: Short MAC

    Description: Protects the integrity of a SERVICE REQUEST message.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.28
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_short_mac_ie(uint16   short_mac,
                                               uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_short_mac_ie(uint8  **ie_ptr,
                                                 uint16  *short_mac);

/*********************************************************************
    IE Name: Time Zone

    Description: Encodes the offset between universal time and local
                 time in steps of 15 minutes.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.29
                        24.008 v10.2.0 Section 10.5.3.8
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_time_zone_ie(uint8   tz,
                                               uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_time_zone_ie(uint8 **ie_ptr,
                                                 uint8  *tz);

/*********************************************************************
    IE Name: Time Zone And Time

    Description: Encodes the offset between universal time and local
                 time in steps of 15 minutes and encodes the universal
                 time at which the IE may have been sent by the
                 network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.30
                        24.008 v10.2.0 Section 10.5.3.9
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint16 year;
    uint8  month;
    uint8  day;
    uint8  hour;
    uint8  minute;
    uint8  second;
    uint8  tz;
}LIBLTE_MME_TIME_ZONE_AND_TIME_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_time_zone_and_time_ie(LIBLTE_MME_TIME_ZONE_AND_TIME_STRUCT  *ttz,
                                                        uint8                                **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_time_zone_and_time_ie(uint8                                **ie_ptr,
                                                          LIBLTE_MME_TIME_ZONE_AND_TIME_STRUCT  *ttz);

/*********************************************************************
    IE Name: TMSI Status

    Description: Indicates whether a valid TMSI is available in the
                 UE or not.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.31
                        24.008 v10.2.0 Section 10.5.5.4
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_TMSI_STATUS_NO_VALID_TMSI = 0,
    LIBLTE_MME_TMSI_STATUS_VALID_TMSI,
    LIBLTE_MME_TMSI_STATUS_N_ITEMS,
}LIBLTE_MME_TMSI_STATUS_ENUM;
static const char liblte_mme_tmsi_status_text[LIBLTE_MME_TMSI_STATUS_N_ITEMS][20] = {"No valid TMSI",
                                                                                     "Valid TMSI"};
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_tmsi_status_ie(LIBLTE_MME_TMSI_STATUS_ENUM   tmsi_status,
                                                 uint8                         bit_offset,
                                                 uint8                       **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_tmsi_status_ie(uint8                       **ie_ptr,
                                                   uint8                         bit_offset,
                                                   LIBLTE_MME_TMSI_STATUS_ENUM  *tmsi_status);

/*********************************************************************
    IE Name: Tracking Area Identity

    Description: Provides an unambiguous identification of tracking
                 areas within the area covered by the 3GPP system.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.32
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint16 mcc;
    uint16 mnc;
    uint16 tac;
}LIBLTE_MME_TRACKING_AREA_ID_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_tracking_area_id_ie(LIBLTE_MME_TRACKING_AREA_ID_STRUCT  *tai,
                                                      uint8                              **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_tracking_area_id_ie(uint8                              **ie_ptr,
                                                        LIBLTE_MME_TRACKING_AREA_ID_STRUCT  *tai);

/*********************************************************************
    IE Name: Tracking Area Identity List

    Description: Transfers a list of tracking areas from the network
                 to the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.33
*********************************************************************/
// Defines
#define LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_MAX_SIZE 16
// Enums
typedef enum{
    LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_TYPE_ONE_PLMN_NON_CONSECUTIVE_TACS = 0,
    LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_TYPE_ONE_PLMN_CONSECUTIVE_TACS,
    LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_TYPE_DIFFERENT_PLMNS,
    LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_TYPE_N_ITEMS,
}LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_TYPE_ENUM;
static const char liblte_mme_tracking_area_identity_list_type_text[LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_TYPE_N_ITEMS][100] = {"One PLMN, Non-Consecutive TACs",
                                                                                                                                "One PLMN, Consecutive TACs",
                                                                                                                                "Different PLMNs"};
// Structs
typedef struct{
    LIBLTE_MME_TRACKING_AREA_ID_STRUCT tai[LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_MAX_SIZE];
    uint32                             N_tais;
}LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_tracking_area_identity_list_ie(LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_STRUCT  *tai_list,
                                                                 uint8                                         **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_tracking_area_identity_list_ie(uint8                                         **ie_ptr,
                                                                   LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_STRUCT  *tai_list);

/*********************************************************************
    IE Name: UE Network Capability

    Description: Provides the network with information concerning
                 aspects of the UE related to EPS or interworking with
                 GPRS.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.34
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    bool eea[8];
    bool eia[8];
    bool uea[8];
    bool uea_present;
    bool ucs2;
    bool ucs2_present;
    bool uia[8];
    bool uia_present;
    bool lpp;
    bool lpp_present;
    bool lcs;
    bool lcs_present;
    bool onexsrvcc;
    bool onexsrvcc_present;
    bool nf;
    bool nf_present;
}LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_ue_network_capability_ie(LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT  *ue_network_cap,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_ue_network_capability_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT  *ue_network_cap);

/*********************************************************************
    IE Name: UE Radio Capability Update Needed

    Description: Indicates whether the MME shall delete the stored
                 UE radio capability information, if any.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.35
*********************************************************************/
// Defines
#define LIBLTE_MME_URC_UPDATE_NOT_NEEDED 0
#define LIBLTE_MME_URC_UPDATE_NEEDED     1
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_ue_radio_capability_update_needed_ie(uint8   urc_update,
                                                                       uint8   bit_offset,
                                                                       uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_ue_radio_capability_update_needed_ie(uint8 **ie_ptr,
                                                                         uint8   bit_offset,
                                                                         uint8  *urc_update);

/*********************************************************************
    IE Name: UE Security Capability

    Description: Indicates which security algorithms are supported by
                 the UE in S1 mode.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.36
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    bool eea[8];
    bool eia[8];
    bool uea[8];
    bool uea_present;
    bool uia[8];
    bool uia_present;
    bool gea[8];
    bool gea_present;
}LIBLTE_MME_UE_SECURITY_CAPABILITIES_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_ue_security_capabilities_ie(LIBLTE_MME_UE_SECURITY_CAPABILITIES_STRUCT  *ue_sec_cap,
                                                              uint8                                      **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_ue_security_capabilities_ie(uint8                                      **ie_ptr,
                                                                LIBLTE_MME_UE_SECURITY_CAPABILITIES_STRUCT  *ue_sec_cap);

/*********************************************************************
    IE Name: Emergency Number List

    Description: Encodes emergency number(s) for use within the
                 country (as indicated by MCC) where the IE is
                 received.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.37
                        24.008 v10.2.0 Section 10.5.3.13
*********************************************************************/
// Defines
#define LIBLTE_MME_EMERGENCY_NUMBER_LIST_MAX_SIZE  12
#define LIBLTE_MME_EMERGENCY_NUMBER_MAX_NUM_DIGITS 92
// Enums
typedef enum{
    LIBLTE_MME_EMERGENCY_SERVICE_CATEGORY_POLICE = 0,
    LIBLTE_MME_EMERGENCY_SERVICE_CATEGORY_AMBULANCE,
    LIBLTE_MME_EMERGENCY_SERVICE_CATEGORY_FIRE,
    LIBLTE_MME_EMERGENCY_SERVICE_CATEGORY_MARINE_GUARD,
    LIBLTE_MME_EMERGENCY_SERVICE_CATEGORY_MOUNTAIN_RESCUE,
    LIBLTE_MME_EMERGENCY_SERVICE_CATEGORY_MANUALLY_INITIATED_ECALL,
    LIBLTE_MME_EMERGENCY_SERVICE_CATEGORY_AUTOMATICALLY_INITIATED_ECALL,
    LIBLTE_MME_EMERGENCY_SERVICE_CATEGORY_N_ITEMS,
}LIBLTE_MME_EMERGENCY_SERVICE_CATEGORY_ENUM;
static const char liblte_mme_emergency_service_category_text[LIBLTE_MME_EMERGENCY_SERVICE_CATEGORY_N_ITEMS][100] = {"Police",
                                                                                                                    "Ambulance",
                                                                                                                    "Fire",
                                                                                                                    "Marine Guard",
                                                                                                                    "Mountain Rescue",
                                                                                                                    "Manually Initiated ECall",
                                                                                                                    "Automatically Initiated ECall"};
// Structs
typedef struct{
    LIBLTE_MME_EMERGENCY_SERVICE_CATEGORY_ENUM emerg_service_cat;
    uint32                                     N_emerg_num_digits;
    uint8                                      emerg_num[LIBLTE_MME_EMERGENCY_NUMBER_MAX_NUM_DIGITS];
}LIBLTE_MME_EMERGENCY_NUMBER_STRUCT;
typedef struct{
    LIBLTE_MME_EMERGENCY_NUMBER_STRUCT emerg_num[LIBLTE_MME_EMERGENCY_NUMBER_LIST_MAX_SIZE];
    uint32                             N_emerg_nums;
}LIBLTE_MME_EMERGENCY_NUMBER_LIST_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_emergency_number_list_ie(LIBLTE_MME_EMERGENCY_NUMBER_LIST_STRUCT  *emerg_num_list,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_emergency_number_list_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_MME_EMERGENCY_NUMBER_LIST_STRUCT  *emerg_num_list);

/*********************************************************************
    IE Name: CLI

    Description: Conveys information about the calling line for a
                 terminated call to a CS fallback capable UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.38
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: SS Code

    Description: Conveys information related to a network initiated
                 supplementary service request to a CS fallback
                 capable UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.39
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_ss_code_ie(uint8   code,
                                             uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_ss_code_ie(uint8 **ie_ptr,
                                               uint8  *code);

/*********************************************************************
    IE Name: LCS Indicator

    Description: Indicates that the origin of the message is due to a
                 LCS request and the type of this request to a CS
                 fallback capable UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.40
*********************************************************************/
// Defines
#define LIBLTE_MME_LCS_INDICATOR_MT_LR 0x01
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_lcs_indicator_ie(uint8   lcs_ind,
                                                   uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_lcs_indicator_ie(uint8 **ie_ptr,
                                                     uint8  *lcs_ind);

/*********************************************************************
    IE Name: LCS Client Identity

    Description: Conveys information related to the client of a LCS
                 request for a CS fallback capable UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.41
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: Generic Message Container Type

    Description: Specifies the type of message contained in the
                 generic message container IE.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.42
*********************************************************************/
// Defines
#define LIBLTE_MME_GENERIC_MESSAGE_CONTAINER_TYPE_LPP               0x01
#define LIBLTE_MME_GENERIC_MESSAGE_CONTAINER_TYPE_LOCATION_SERVICES 0x02
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_generic_message_container_type_ie(uint8   msg_cont_type,
                                                                    uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_generic_message_container_type_ie(uint8 **ie_ptr,
                                                                      uint8  *msg_cont_type);

/*********************************************************************
    IE Name: Generic Message Container

    Description: Encapsulates the application message transferred
                 between the UE and the network.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.43
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_generic_message_container_ie(LIBLTE_BYTE_MSG_STRUCT  *msg,
                                                               uint8                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_generic_message_container_ie(uint8                  **ie_ptr,
                                                                 LIBLTE_BYTE_MSG_STRUCT  *msg);

/*********************************************************************
    IE Name: Voice Domain Preference and UE's Usage Setting

    Description: Provides the network with the UE's usage setting and
                 the voice domain preference for the E-UTRAN.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.44
                        24.008 v10.2.0 Section 10.5.5.28
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_UE_USAGE_SETTING_VOICE_CENTRIC = 0,
    LIBLTE_MME_UE_USAGE_SETTING_DATA_CENTRIC,
    LIBLTE_MME_UE_USAGE_SETTING_N_ITEMS,
}LIBLTE_MME_UE_USAGE_SETTING_ENUM;
static const char liblte_mme_ue_usage_setting_text[LIBLTE_MME_UE_USAGE_SETTING_N_ITEMS][20] = {"Voice Centric",
                                                                                               "Data Centric"};
typedef enum{
    LIBLTE_MME_VOICE_DOMAIN_PREF_CS_ONLY = 0,
    LIBLTE_MME_VOICE_DOMAIN_PREF_PS_ONLY,
    LIBLTE_MME_VOICE_DOMAIN_PREF_CS_PREFFERED,
    LIBLTE_MME_VOICE_DOMAIN_PREF_PS_PREFFERED,
    LIBLTE_MME_VOICE_DOMAIN_PREF_N_ITEMS,
}LIBLTE_MME_VOICE_DOMAIN_PREF_ENUM;
static const char liblte_mme_voice_domain_pref_text[LIBLTE_MME_VOICE_DOMAIN_PREF_N_ITEMS][20] = {"CS Only",
                                                                                                 "PS Only",
                                                                                                 "CS Preffered",
                                                                                                 "PS Preffered"};
// Structs
typedef struct{
    LIBLTE_MME_UE_USAGE_SETTING_ENUM  ue_usage_setting;
    LIBLTE_MME_VOICE_DOMAIN_PREF_ENUM voice_domain_pref;
}LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_voice_domain_pref_and_ue_usage_setting_ie(LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_STRUCT  *voice_domain_pref_and_ue_usage_setting,
                                                                            uint8                                                    **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_voice_domain_pref_and_ue_usage_setting_ie(uint8                                                    **ie_ptr,
                                                                              LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_STRUCT  *voice_domain_pref_and_ue_usage_setting);

/*********************************************************************
    IE Name: GUTI Type

    Description: Indicates whether the GUTI included in the same
                 message in an information element of type EPS
                 mobility identity represents a native GUTI or a
                 mapped GUTI.

    Document Reference: 24.301 v10.2.0 Section 9.9.3.45
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_GUTI_TYPE_NATIVE = 0,
    LIBLTE_MME_GUTI_TYPE_MAPPED,
    LIBLTE_MME_GUTI_TYPE_N_ITEMS,
}LIBLTE_MME_GUTI_TYPE_ENUM;
static const char liblte_mme_guti_type_text[LIBLTE_MME_GUTI_TYPE_N_ITEMS][20] = {"Native",
                                                                                 "Mapped"};
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_guti_type_ie(LIBLTE_MME_GUTI_TYPE_ENUM   guti_type,
                                               uint8                       bit_offset,
                                               uint8                     **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_guti_type_ie(uint8                     **ie_ptr,
                                                 uint8                       bit_offset,
                                                 LIBLTE_MME_GUTI_TYPE_ENUM  *guti_type);

/*********************************************************************
    IE Name: Access Point Name

    Description: Identifies the packet data network to which the GPRS
                 user wishes to connect and notifies the access point
                 of the packet data network that wishes to connect to
                 the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.1
                        24.008 v10.2.0 Section 10.5.6.1
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    char apn[LIBLTE_STRING_LEN];
}LIBLTE_MME_ACCESS_POINT_NAME_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_access_point_name_ie(LIBLTE_MME_ACCESS_POINT_NAME_STRUCT  *apn,
                                                       uint8                               **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_access_point_name_ie(uint8                               **ie_ptr,
                                                         LIBLTE_MME_ACCESS_POINT_NAME_STRUCT  *apn);

/*********************************************************************
    IE Name: APN Aggregate Maximum Bit Rate

    Description: Indicates the initial subscribed APN-AMBR when the
                 UE establishes a PDN connection or indicates the new
                 APN-AMBR if it is changed by the network.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.2
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 apn_ambr_dl;
    uint8 apn_ambr_ul;
    uint8 apn_ambr_dl_ext;
    uint8 apn_ambr_ul_ext;
    uint8 apn_ambr_dl_ext2;
    uint8 apn_ambr_ul_ext2;
    bool  ext_present;
    bool  ext2_present;
}LIBLTE_MME_APN_AGGREGATE_MAXIMUM_BIT_RATE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_apn_aggregate_maximum_bit_rate_ie(LIBLTE_MME_APN_AGGREGATE_MAXIMUM_BIT_RATE_STRUCT  *apn_ambr,
                                                                    uint8                                            **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_apn_aggregate_maximum_bit_rate_ie(uint8                                            **ie_ptr,
                                                                      LIBLTE_MME_APN_AGGREGATE_MAXIMUM_BIT_RATE_STRUCT  *apn_ambr);

/*********************************************************************
    IE Name: Connectivity Type

    Description: Specifies the type of connectivity selected by the
                 network for the PDN connection.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.2A
                        24.008 v10.2.0 Section 10.5.6.19
*********************************************************************/
// Defines
#define LIBLTE_MME_CONNECTIVITY_TYPE_NOT_INDICATED 0x0
#define LIBLTE_MME_CONNECTIVITY_TYPE_LIPA_PDN      0x1
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_connectivity_type_ie(uint8   con_type,
                                                       uint8   bit_offset,
                                                       uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_connectivity_type_ie(uint8 **ie_ptr,
                                                         uint8   bit_offset,
                                                         uint8  *con_type);

/*********************************************************************
    IE Name: EPS Quality Of Service

    Description: Specifies the QoS parameters for an EPS bearer
                 context.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.3
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 qci;
    uint8 mbr_ul;
    uint8 mbr_dl;
    uint8 gbr_ul;
    uint8 gbr_dl;
    uint8 mbr_ul_ext;
    uint8 mbr_dl_ext;
    uint8 gbr_ul_ext;
    uint8 gbr_dl_ext;
    bool  br_present;
    bool  br_ext_present;
}LIBLTE_MME_EPS_QUALITY_OF_SERVICE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_eps_quality_of_service_ie(LIBLTE_MME_EPS_QUALITY_OF_SERVICE_STRUCT  *qos,
                                                            uint8                                    **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_eps_quality_of_service_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_MME_EPS_QUALITY_OF_SERVICE_STRUCT  *qos);

/*********************************************************************
    IE Name: ESM Cause

    Description: Indicates the reason why a session management request
                 is rejected.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.4
*********************************************************************/
// Defines
#define LIBLTE_MME_ESM_CAUSE_OPERATOR_DETERMINED_BARRING                                       0x08
#define LIBLTE_MME_ESM_CAUSE_INSUFFICIENT_RESOURCES                                            0x1A
#define LIBLTE_MME_ESM_CAUSE_UNKNOWN_OR_MISSING_APN                                            0x1B
#define LIBLTE_MME_ESM_CAUSE_UNKNOWN_PDN_TYPE                                                  0x1C
#define LIBLTE_MME_ESM_CAUSE_USER_AUTHENTICATION_FAILED                                        0x1D
#define LIBLTE_MME_ESM_CAUSE_REQUEST_REJECTED_BY_SERVING_OR_PDN_GW                             0x1E
#define LIBLTE_MME_ESM_CAUSE_REQUEST_REJECTED_UNSPECIFIED                                      0x1F
#define LIBLTE_MME_ESM_CAUSE_SERVICE_OPTION_NOT_SUPPORTED                                      0x20
#define LIBLTE_MME_ESM_CAUSE_REQUESTED_SERVICE_OPTION_NOT_SUBSCRIBED                           0x21
#define LIBLTE_MME_ESM_CAUSE_SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER                           0x22
#define LIBLTE_MME_ESM_CAUSE_PTI_ALREADY_IN_USE                                                0x23
#define LIBLTE_MME_ESM_CAUSE_REGULAR_DEACTIVATION                                              0x24
#define LIBLTE_MME_ESM_CAUSE_EPS_QOS_NOT_ACCEPTED                                              0x25
#define LIBLTE_MME_ESM_CAUSE_NETWORK_FAILURE                                                   0x26
#define LIBLTE_MME_ESM_CAUSE_REACTIVATION_REQUESTED                                            0x27
#define LIBLTE_MME_ESM_CAUSE_SEMANTIC_ERROR_IN_THE_TFT_OPERATION                               0x29
#define LIBLTE_MME_ESM_CAUSE_SYNTACTICAL_ERROR_IN_THE_TFT_OPERATION                            0x2A
#define LIBLTE_MME_ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY                                       0x2B
#define LIBLTE_MME_ESM_CAUSE_SEMANTIC_ERRORS_IN_PACKET_FILTERS                                 0x2C
#define LIBLTE_MME_ESM_CAUSE_SYNTACTICAL_ERRORS_IN_PACKET_FILTERS                              0x2D
#define LIBLTE_MME_ESM_CAUSE_UNUSED                                                            0x2E
#define LIBLTE_MME_ESM_CAUSE_PTI_MISMATCH                                                      0x2F
#define LIBLTE_MME_ESM_CAUSE_LAST_PDN_DISCONNECTION_NOT_ALLOWED                                0x31
#define LIBLTE_MME_ESM_CAUSE_PDN_TYPE_IPV4_ONLY_ALLOWED                                        0x32
#define LIBLTE_MME_ESM_CAUSE_PDN_TYPE_IPV6_ONLY_ALLOWED                                        0x33
#define LIBLTE_MME_ESM_CAUSE_SINGLE_ADDRESS_BEARERS_ONLY_ALLOWED                               0x34
#define LIBLTE_MME_ESM_CAUSE_ESM_INFORMATION_NOT_RECEIVED                                      0x35
#define LIBLTE_MME_ESM_CAUSE_PDN_CONNECTION_DOES_NOT_EXIST                                     0x36
#define LIBLTE_MME_ESM_CAUSE_MULTIPLE_PDN_CONNECTIONS_FOR_A_GIVEN_APN_NOT_ALLOWED              0x37
#define LIBLTE_MME_ESM_CAUSE_COLLISION_WITH_NETWORK_INITIATED_REQUEST                          0x38
#define LIBLTE_MME_ESM_CAUSE_UNSUPPORTED_QCI_VALUE                                             0x3B
#define LIBLTE_MME_ESM_CAUSE_BEARER_HANDLING_NOT_SUPPORTED                                     0x3C
#define LIBLTE_MME_ESM_CAUSE_INVALID_PTI_VALUE                                                 0x51
#define LIBLTE_MME_ESM_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE                                    0x5F
#define LIBLTE_MME_ESM_CAUSE_INVALID_MANDATORY_INFORMATION                                     0x60
#define LIBLTE_MME_ESM_CAUSE_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED                      0x61
#define LIBLTE_MME_ESM_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE               0x62
#define LIBLTE_MME_ESM_CAUSE_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED               0x63
#define LIBLTE_MME_ESM_CAUSE_CONDITIONAL_IE_ERROR                                              0x64
#define LIBLTE_MME_ESM_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE                    0x65
#define LIBLTE_MME_ESM_CAUSE_PROTOCOL_ERROR_UNSPECIFIED                                        0x6F
#define LIBLTE_MME_ESM_CAUSE_APN_RESTRICTION_VALUE_INCOMPATIBLE_WITH_ACTIVE_EPS_BEARER_CONTEXT 0x70
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_esm_cause_ie(uint8   cause,
                                               uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_cause_ie(uint8 **ie_ptr,
                                                 uint8  *cause);

/*********************************************************************
    IE Name: ESM Information Transfer Flag

    Description: Indicates whether ESM information, i.e. protocol
                 configuration options or APN or both, is to be
                 transferred security protected.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.5
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_NOT_REQUIRED = 0,
    LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_REQUIRED,
    LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_N_ITEMS,
}LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_ENUM;
static const char liblte_mme_esm_info_transfer_flag_text[LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_N_ITEMS][20] = {"Not Required",
                                                                                                           "Required"};
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_esm_info_transfer_flag_ie(LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_ENUM   esm_info_transfer_flag,
                                                            uint8                                    bit_offset,
                                                            uint8                                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_info_transfer_flag_ie(uint8                                  **ie_ptr,
                                                              uint8                                    bit_offset,
                                                              LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_ENUM  *esm_info_transfer_flag);

/*********************************************************************
    IE Name: Linked EPS Bearer Identity

    Description: Identifies the default bearer that is associated
                 with a dedicated EPS bearer or identifies the EPS
                 bearer (default or dedicated) with which one or more
                 packet filters specified in a traffic flow aggregate
                 are associated.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.6
*********************************************************************/
// Defines
#define LIBLTE_MME_LINKED_EPS_BEARER_IDENTITY_5  0x5
#define LIBLTE_MME_LINKED_EPS_BEARER_IDENTITY_6  0x6
#define LIBLTE_MME_LINKED_EPS_BEARER_IDENTITY_7  0x7
#define LIBLTE_MME_LINKED_EPS_BEARER_IDENTITY_8  0x8
#define LIBLTE_MME_LINKED_EPS_BEARER_IDENTITY_9  0x9
#define LIBLTE_MME_LINKED_EPS_BEARER_IDENTITY_10 0xA
#define LIBLTE_MME_LINKED_EPS_BEARER_IDENTITY_11 0xB
#define LIBLTE_MME_LINKED_EPS_BEARER_IDENTITY_12 0xC
#define LIBLTE_MME_LINKED_EPS_BEARER_IDENTITY_13 0xD
#define LIBLTE_MME_LINKED_EPS_BEARER_IDENTITY_14 0xE
#define LIBLTE_MME_LINKED_EPS_BEARER_IDENTITY_15 0xF
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_linked_eps_bearer_identity_ie(uint8   bearer_id,
                                                                uint8   bit_offset,
                                                                uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_linked_eps_bearer_identity_ie(uint8 **ie_ptr,
                                                                  uint8   bit_offset,
                                                                  uint8  *bearer_id);

/*********************************************************************
    IE Name: LLC Service Access Point Identifier

    Description: Identifies the service access point that is used for
                 the GPRS data transfer at LLC layer.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.7
                        24.008 v10.2.0 Section 10.5.6.9
*********************************************************************/
// Defines
#define LIBLTE_MME_LLC_SAPI_NOT_ASSIGNED 0x0
#define LIBLTE_MME_LLC_SAPI_3            0x3
#define LIBLTE_MME_LLC_SAPI_5            0x5
#define LIBLTE_MME_LLC_SAPI_9            0x9
#define LIBLTE_MME_LLC_SAPI_11           0xB
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_llc_service_access_point_identifier_ie(uint8   llc_sapi,
                                                                         uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_llc_service_access_point_identifier_ie(uint8 **ie_ptr,
                                                                           uint8  *llc_sapi);

/*********************************************************************
    IE Name: Notification Indicator

    Description: Informs the UE about an event which is relevant for
                 the upper layer using an EPS bearer context or
                 having requested a procedure transaction.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.7A
*********************************************************************/
// Defines
#define LIBLTE_MME_NOTIFICATION_INDICATOR_SRVCC_HO_CANCELLED_IMS_SESSION_REEST_REQ 0x01
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_notification_indicator_ie(uint8   notification_ind,
                                                            uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_notification_indicator_ie(uint8 **ie_ptr,
                                                              uint8  *notification_ind);

/*********************************************************************
    IE Name: Packet Flow Identifier

    Description: Indicates the packet flow identifier for a packet
                 flow context.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.8
                        24.008 v10.2.0 Section 10.5.6.11
*********************************************************************/
// Defines
#define LIBLTE_MME_PACKET_FLOW_ID_BEST_EFFORT 0x00
#define LIBLTE_MME_PACKET_FLOW_ID_SIGNALLING  0x01
#define LIBLTE_MME_PACKET_FLOW_ID_SMS         0x02
#define LIBLTE_MME_PACKET_FLOW_ID_TOM8        0x03
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_packet_flow_identifier_ie(uint8   packet_flow_id,
                                                            uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_packet_flow_identifier_ie(uint8 **ie_ptr,
                                                              uint8  *packet_flow_id);

/*********************************************************************
    IE Name: PDN Address

    Description: Assigns an IPv4 address to the UE associated with a
                 packet data network and provides the UE with an
                 interface identifier to be used to build the IPv6
                 link local address.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.9
*********************************************************************/
// Defines
#define LIBLTE_MME_PDN_TYPE_IPV4   0x1
#define LIBLTE_MME_PDN_TYPE_IPV6   0x2
#define LIBLTE_MME_PDN_TYPE_IPV4V6 0x3
// Enums
// Structs
typedef struct{
    uint8 pdn_type;
    uint8 addr[12];
}LIBLTE_MME_PDN_ADDRESS_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_pdn_address_ie(LIBLTE_MME_PDN_ADDRESS_STRUCT  *pdn_addr,
                                                 uint8                         **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_pdn_address_ie(uint8                         **ie_ptr,
                                                   LIBLTE_MME_PDN_ADDRESS_STRUCT  *pdn_addr);

/*********************************************************************
    IE Name: PDN Type

    Description: Indicates the IP version capability of the IP stack
                 associated with the UE.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.10
*********************************************************************/
// Defines
#define LIBLTE_MME_PDN_TYPE_IPV4   0x1
#define LIBLTE_MME_PDN_TYPE_IPV6   0x2
#define LIBLTE_MME_PDN_TYPE_IPV4V6 0x3
#define LIBLTE_MME_PDN_TYPE_UNUSED 0x4
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_pdn_type_ie(uint8   pdn_type,
                                              uint8   bit_offset,
                                              uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_pdn_type_ie(uint8 **ie_ptr,
                                                uint8   bit_offset,
                                                uint8  *pdn_type);

/*********************************************************************
    IE Name: Protocol Configuration Options

    Description: Transfers external network protocol options
                 associated with a PDP context activation and
                 transfers additional (protocol) data (e.g.
                 configuration parameters, error codes or messages/
                 events) associated with an external protocol or an
                 application.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.11
                        24.008 v10.2.0 Section 10.5.6.3
*********************************************************************/
// Defines
#define LIBLTE_MME_MAX_PROTOCOL_CONFIG_OPTIONS                                                       83
#define LIBLTE_MME_MAX_PROTOCOL_CONFIG_LEN                                                           248
#define LIBLTE_MME_CONFIGURATION_PROTOCOL_OPTIONS_LCP                                                0xC021
#define LIBLTE_MME_CONFIGURATION_PROTOCOL_OPTIONS_PAP                                                0xC023
#define LIBLTE_MME_CONFIGURATION_PROTOCOL_OPTIONS_CHAP                                               0xC223
#define LIBLTE_MME_CONFIGURATION_PROTOCOL_OPTIONS_IPCP                                               0x8021
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_P_CSCF_IPV6_ADDRESS_REQUEST                              0x0001
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_IM_CN_SUBSYSTEM_SIGNALLING_FLAG                          0x0002
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_DNS_SERVER_IPV6_ADDRESS_REQUEST                          0x0003
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_MS_SUPPORT_OF_NETWORK_REQUESTED_BEARER_CONTROL_INDICATOR 0x0005
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_DSMIPV6_HOME_AGENT_ADDRESS_REQUEST                       0x0007
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_DSMIPV6_HOME_NETWORK_PREFIX_REQUEST                      0x0008
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_DSMIPV6_IPV4_HOME_AGENT_ADDRESS_REQUEST                  0x0009
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_IP_ADDRESS_ALLOCATION_VIA_NAS_SIGNALLING                 0x000A
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_IPV4_ADDRESS_ALLOCATION_VIA_DHCPV4                       0x000B
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_P_CSCF_IPV4_ADDRESS_REQUEST                              0x000C
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_DNS_SERVER_IPV4_ADDRESS_REQUEST                          0x000D
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_MSISDN_REQUEST                                           0x000E
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_IFOM_SUPPORT_REQUEST                                     0x000F
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_UL_IPV4_LINK_MTU_REQUEST                                    0x0010
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_P_CSCF_IPV6_ADDRESS                                      0x0001
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_IM_CN_SUBSYSTEM_SIGNALLING_FLAG                          0x0002
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_DNS_SERVER_IPV6_ADDRESS                                  0x0003
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_POLICY_CONTROL_REJECTION_CODE                            0x0004
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_SELECTED_BEARER_CONTROL_MODE                             0x0005
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_DSMIPV6_HOME_AGENT_ADDRESS                               0x0007
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_DSMIPV6_HOME_NETWORK_PREFIX                              0x0008
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_DSMIPV6_IPV4_HOME_AGENT_ADDRESS                          0x0009
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_P_CSCF_IPV4_ADDRESS                                      0x000C
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_DNS_SERVER_IPV4_ADDRESS                                  0x000D
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_MSISDN                                                   0x000E
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_IFOM_SUPPORT                                             0x000F
#define LIBLTE_MME_ADDITIONAL_PARAMETERS_DL_IPV4_LINK_MTU                                            0x0010
// Enums
// Structs
typedef struct{
    uint16 id;
    uint8  len;
    uint8  contents[LIBLTE_MME_MAX_PROTOCOL_CONFIG_LEN];
}LIBLTE_MME_PROTOCOL_CONFIG_STRUCT;
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_STRUCT opt[LIBLTE_MME_MAX_PROTOCOL_CONFIG_OPTIONS];
    uint32                            N_opts;
}LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_protocol_config_options_ie(LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT  *protocol_cnfg_opts,
                                                             uint8                                     **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_protocol_config_options_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT  *protocol_cnfg_opts);

/*********************************************************************
    IE Name: Quality Of Service

    Description: Specifies the QoS parameters for a PDP context.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.12
                        24.008 v10.2.0 Section 10.5.6.5
*********************************************************************/
// Defines
#define LIBLTE_MME_QOS_DELAY_CLASS_UL_SUBSCRIBED                         0x0
#define LIBLTE_MME_QOS_DELAY_CLASS_DL_RESERVED                           0x0
#define LIBLTE_MME_QOS_DELAY_CLASS_1                                     0x1
#define LIBLTE_MME_QOS_DELAY_CLASS_2                                     0x2
#define LIBLTE_MME_QOS_DELAY_CLASS_3                                     0x3
#define LIBLTE_MME_QOS_DELAY_CLASS_4                                     0x4
#define LIBLTE_MME_QOS_DELAY_CLASS_RESERVED                              0x7
#define LIBLTE_MME_QOS_RELIABILITY_CLASS_UL_SUBSCRIBED                   0x0
#define LIBLTE_MME_QOS_RELIABILITY_CLASS_DL_RESERVED                     0x0
#define LIBLTE_MME_QOS_RELIABILITY_CLASS_UNUSED                          0x1
#define LIBLTE_MME_QOS_RELIABILITY_CLASS_UNACK_GTP_ACK_LLC_RLC_PROTECTED 0x2
#define LIBLTE_MME_QOS_RELIABILITY_CLASS_UNACK_GTP_LLC_ACK_RLC_PROTECTED 0x3
#define LIBLTE_MME_QOS_RELIABILITY_CLASS_UNACK_GTP_LLC_RLC_PROTECTED     0x4
#define LIBLTE_MME_QOS_RELIABILITY_CLASS_UNACK_GTP_LLC_RLC_UNPROTECTED   0x5
#define LIBLTE_MME_QOS_RELIABILITY_CLASS_RESERVED                        0x7
#define LIBLTE_MME_QOS_PEAK_THROUGHPUT_UL_SUBSCRIBED                     0x0
#define LIBLTE_MME_QOS_PEAK_THROUGHPUT_DL_RESERVED                       0x0
#define LIBLTE_MME_QOS_PEAK_THROUGHPUT_UP_TO_1000BPS                     0x1
#define LIBLTE_MME_QOS_PEAK_THROUGHPUT_UP_TO_2000BPS                     0x2
#define LIBLTE_MME_QOS_PEAK_THROUGHPUT_UP_TO_4000BPS                     0x3
#define LIBLTE_MME_QOS_PEAK_THROUGHPUT_UP_TO_8000BPS                     0x4
#define LIBLTE_MME_QOS_PEAK_THROUGHPUT_UP_TO_16000BPS                    0x5
#define LIBLTE_MME_QOS_PEAK_THROUGHPUT_UP_TO_32000BPS                    0x6
#define LIBLTE_MME_QOS_PEAK_THROUGHPUT_UP_TO_64000BPS                    0x7
#define LIBLTE_MME_QOS_PEAK_THROUGHPUT_UP_TO_128000BPS                   0x8
#define LIBLTE_MME_QOS_PEAK_THROUGHPUT_UP_TO_256000BPS                   0x9
#define LIBLTE_MME_QOS_PEAK_THROUGHPUT_RESERVED                          0xF
#define LIBLTE_MME_QOS_PRECEDENCE_CLASS_UL_SUBSCRIBED                    0x0
#define LIBLTE_MME_QOS_PRECEDENCE_CLASS_DL_RESERVED                      0x0
#define LIBLTE_MME_QOS_PRECEDENCE_CLASS_HIGH_PRIORITY                    0x1
#define LIBLTE_MME_QOS_PRECEDENCE_CLASS_NORMAL_PRIORITY                  0x2
#define LIBLTE_MME_QOS_PRECEDENCE_CLASS_LOW_PRIORITY                     0x3
#define LIBLTE_MME_QOS_PRECEDENCE_CLASS_RESERVED                         0x7
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_UL_SUBSCRIBED                     0x00
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_DL_RESERVED                       0x00
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_100BPH                            0x01
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_200BPH                            0x02
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_500BPH                            0x03
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_1000BPH                           0x04
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_2000BPH                           0x05
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_5000BPH                           0x06
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_10000BPH                          0x07
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_20000BPH                          0x08
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_50000BPH                          0x09
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_100000BPH                         0x0A
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_200000BPH                         0x0B
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_500000BPH                         0x0C
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_1000000BPH                        0x0D
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_2000000BPH                        0x0E
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_5000000BPH                        0x0F
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_10000000BPH                       0x10
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_20000000BPH                       0x11
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_50000000BPH                       0x12
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_RESERVED                          0x1E
#define LIBLTE_MME_QOS_MEAN_THROUGHPUT_BEST_EFFORT                       0x1F
#define LIBLTE_MME_QOS_TRAFFIC_CLASS_UL_SUBSCRIBED                       0x0
#define LIBLTE_MME_QOS_TRAFFIC_CLASS_DL_RESERVED                         0x0
#define LIBLTE_MME_QOS_TRAFFIC_CLASS_CONVERSATIONAL                      0x1
#define LIBLTE_MME_QOS_TRAFFIC_CLASS_STREAMING                           0x2
#define LIBLTE_MME_QOS_TRAFFIC_CLASS_INTERACTIVE                         0x3
#define LIBLTE_MME_QOS_TRAFFIC_CLASS_BACKGROUND                          0x4
#define LIBLTE_MME_QOS_TRAFFIC_CLASS_RESERVED                            0x7
#define LIBLTE_MME_QOS_DELIVERY_ORDER_UL_SUBSCRIBED                      0x0
#define LIBLTE_MME_QOS_DELIVERY_ORDER_DL_RESERVED                        0x0
#define LIBLTE_MME_QOS_DELIVERY_ORDER_WITH_DELIVERY_ORDER_YES            0x1
#define LIBLTE_MME_QOS_DELIVERY_ORDER_WITHOUT_DELIVERY_ORDER_NO          0x2
#define LIBLTE_MME_QOS_DELIVERY_ORDER_RESERVED                           0x3
#define LIBLTE_MME_QOS_DELIVERY_OF_ERRONEOUS_SDU_UL_SUBSCRIBED           0x0
#define LIBLTE_MME_QOS_DELIVERY_OF_ERRONEOUS_SDU_DL_RESERVED             0x0
#define LIBLTE_MME_QOS_DELIVERY_OF_ERRONEOUS_SDU_NO_DETECT               0x1
#define LIBLTE_MME_QOS_DELIVERY_OF_ERRONEOUS_SDU_DELIVERED               0x2
#define LIBLTE_MME_QOS_DELIVERY_OF_ERRONEOUS_SDU_NOT_DELIVERED           0x3
#define LIBLTE_MME_QOS_DELIVERY_OF_ERRONEOUS_SDU_RESERVED                0x7
#define LIBLTE_MME_QOS_MAX_SDU_SIZE_UL_SUBSCRIBED                        0x00
#define LIBLTE_MME_QOS_MAX_SDU_SIZE_DL_RESERVED                          0x00
#define LIBLTE_MME_QOS_MAX_SDU_SIZE_RESERVED                             0xFF
#define LIBLTE_MME_QOS_RESIDUAL_BER_UL_SUBSCRIBED                        0x0
#define LIBLTE_MME_QOS_RESIDUAL_BER_DL_RESERVED                          0x0
#define LIBLTE_MME_QOS_RESIDUAL_BER_5_E_NEG_2                            0x1
#define LIBLTE_MME_QOS_RESIDUAL_BER_1_E_NEG_2                            0x2
#define LIBLTE_MME_QOS_RESIDUAL_BER_5_E_NEG_3                            0x3
#define LIBLTE_MME_QOS_RESIDUAL_BER_4_E_NEG_3                            0x4
#define LIBLTE_MME_QOS_RESIDUAL_BER_1_E_NEG_3                            0x5
#define LIBLTE_MME_QOS_RESIDUAL_BER_1_E_NEG_4                            0x6
#define LIBLTE_MME_QOS_RESIDUAL_BER_1_E_NEG_5                            0x7
#define LIBLTE_MME_QOS_RESIDUAL_BER_1_E_NEG_6                            0x8
#define LIBLTE_MME_QOS_RESIDUAL_BER_6_E_NEG_8                            0x9
#define LIBLTE_MME_QOS_RESIDUAL_BER_RESERVED                             0xF
#define LIBLTE_MME_QOS_SDU_ERROR_RATIO_UL_SUBSCRIBED                     0x0
#define LIBLTE_MME_QOS_SDU_ERROR_RATIO_DL_RESERVED                       0x0
#define LIBLTE_MME_QOS_SDU_ERROR_RATIO_1_E_NEG_2                         0x1
#define LIBLTE_MME_QOS_SDU_ERROR_RATIO_7_E_NEG_3                         0x2
#define LIBLTE_MME_QOS_SDU_ERROR_RATIO_1_E_NEG_3                         0x3
#define LIBLTE_MME_QOS_SDU_ERROR_RATIO_1_E_NEG_4                         0x4
#define LIBLTE_MME_QOS_SDU_ERROR_RATIO_1_E_NEG_5                         0x5
#define LIBLTE_MME_QOS_SDU_ERROR_RATIO_1_E_NEG_6                         0x6
#define LIBLTE_MME_QOS_SDU_ERROR_RATIO_1_E_NEG_1                         0x7
#define LIBLTE_MME_QOS_SDU_ERROR_RATIO_RESERVED                          0xF
#define LIBLTE_MME_QOS_TRANSFER_DELAY_UL_SUBSCRIBED                      0x00
#define LIBLTE_MME_QOS_TRANSFER_DELAY_DL_RESERVED                        0x00
#define LIBLTE_MME_QOS_TRANSFER_DELAY_RESERVED                           0x3F
#define LIBLTE_MME_QOS_TRAFFIC_HANDLING_PRIORITY_UL_SUBSCRIBED           0x0
#define LIBLTE_MME_QOS_TRAFFIC_HANDLING_PRIORITY_DL_RESERVED             0x0
#define LIBLTE_MME_QOS_TRAFFIC_HANDLING_PRIORITY_LEVEL_1                 0x1
#define LIBLTE_MME_QOS_TRAFFIC_HANDLING_PRIORITY_LEVEL_2                 0x2
#define LIBLTE_MME_QOS_TRAFFIC_HANDLING_PRIORITY_LEVEL_3                 0x3
#define LIBLTE_MME_QOS_SIGNALLING_INDICATOR_NOT_OPTIMIZED_FOR_SIGNALLING 0x0
#define LIBLTE_MME_QOS_SIGNALLING_INDICATOR_OPTIMIZED_FOR_SIGNALLING     0x1
#define LIBLTE_MME_QOS_SOURCE_STATISTICS_DESCRIPTOR_UNKNOWN              0x0
#define LIBLTE_MME_QOS_SOURCE_STATISTICS_DESCRIPTOR_SPEECH               0x1
// Enums
// Structs
typedef struct{
    uint8 delay_class;
    uint8 reliability_class;
    uint8 peak_throughput;
    uint8 precedence_class;
    uint8 mean_throughput;
    uint8 traffic_class;
    uint8 delivery_order;
    uint8 delivery_of_erroneous_sdu;
    uint8 max_sdu_size;
    uint8 mbr_ul;
    uint8 mbr_dl;
    uint8 residual_ber;
    uint8 sdu_error_ratio;
    uint8 transfer_delay;
    uint8 traffic_handling_prio;
    uint8 gbr_ul;
    uint8 gbr_dl;
    uint8 signalling_ind;
    uint8 source_stats_descriptor;
    uint8 mbr_dl_ext;
    uint8 gbr_dl_ext;
    uint8 mbr_ul_ext;
    uint8 gbr_ul_ext;
    bool  dl_ext_present;
    bool  ul_ext_present;
}LIBLTE_MME_QUALITY_OF_SERVICE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_quality_of_service_ie(LIBLTE_MME_QUALITY_OF_SERVICE_STRUCT  *qos,
                                                        uint8                                **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_quality_of_service_ie(uint8                                **ie_ptr,
                                                          LIBLTE_MME_QUALITY_OF_SERVICE_STRUCT  *qos);

/*********************************************************************
    IE Name: Radio Priority

    Description: Specifies the priority level the UE shall use at the
                 lower layers for transmission of data related to a
                 PDP context or for mobile originated SMS
                 transmission.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.13
                        24.008 v10.2.0 Section 10.5.7.2
*********************************************************************/
// Defines
#define LIBLTE_MME_RADIO_PRIORITY_LEVEL_1 0x1
#define LIBLTE_MME_RADIO_PRIORITY_LEVEL_2 0x2
#define LIBLTE_MME_RADIO_PRIORITY_LEVEL_3 0x3
#define LIBLTE_MME_RADIO_PRIORITY_LEVEL_4 0x4
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_radio_priority_ie(uint8   radio_prio,
                                                    uint8   bit_offset,
                                                    uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_radio_priority_ie(uint8 **ie_ptr,
                                                      uint8   bit_offset,
                                                      uint8  *radio_prio);

/*********************************************************************
    IE Name: Request Type

    Description: Indicates whether the UE requests to establish a new
                 connectivity to a PDN or keep the connection(s) to
                 which it has connected via non-3GPP access.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.14
                        24.008 v10.2.0 Section 10.5.6.17
*********************************************************************/
// Defines
#define LIBLTE_MME_REQUEST_TYPE_INITIAL_REQUEST 0x1
#define LIBLTE_MME_REQUEST_TYPE_HANDOVER        0x2
#define LIBLTE_MME_REQUEST_TYPE_UNUSED          0x3
#define LIBLTE_MME_REQUEST_TYPE_EMERGENCY       0x4
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_request_type_ie(uint8   req_type,
                                                  uint8   bit_offset,
                                                  uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_request_type_ie(uint8 **ie_ptr,
                                                    uint8   bit_offset,
                                                    uint8  *req_type);

/*********************************************************************
    IE Name: Traffic Flow Aggregate Description

    Description: Specifies the aggregate of one or more packet filters
                 and their related parameters and operations.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.15
*********************************************************************/
// Defines
#define LIBLTE_MME_PACKET_FILTER_LIST_MAX_SIZE                                            15
#define LIBLTE_MME_PACKET_FILTER_MAX_SIZE                                                 20
#define LIBLTE_MME_PARAMETER_LIST_MAX_SIZE                                                15
#define LIBLTE_MME_PARAMETER_MAX_SIZE                                                     20
#define LIBLTE_MME_TFT_PACKET_FILTER_COMPONENT_TYPE_ID_IPV4_REMOTE_ADDRESS_TYPE           0x10
#define LIBLTE_MME_TFT_PACKET_FILTER_COMPONENT_TYPE_ID_IPV6_REMOTE_ADDRESS_TYPE           0x20
#define LIBLTE_MME_TFT_PACKET_FILTER_COMPONENT_TYPE_ID_PROTOCOL_ID_NEXT_HEADER_TYPE       0x30
#define LIBLTE_MME_TFT_PACKET_FILTER_COMPONENT_TYPE_ID_SINGLE_LOCAL_PORT_TYPE             0x40
#define LIBLTE_MME_TFT_PACKET_FILTER_COMPONENT_TYPE_ID_LOCAL_PORT_RANGE_TYPE              0x41
#define LIBLTE_MME_TFT_PACKET_FILTER_COMPONENT_TYPE_ID_SINGLE_REMOTE_PORT_TYPE            0x50
#define LIBLTE_MME_TFT_PACKET_FILTER_COMPONENT_TYPE_ID_REMOTE_PORT_RANGE_TYPE             0x51
#define LIBLTE_MME_TFT_PACKET_FILTER_COMPONENT_TYPE_ID_SECURITY_PARAMETER_INDEX_TYPE      0x60
#define LIBLTE_MME_TFT_PACKET_FILTER_COMPONENT_TYPE_ID_TYPE_OF_SERVICE_TRAFFIC_CLASS_TYPE 0x70
#define LIBLTE_MME_TFT_PACKET_FILTER_COMPONENT_TYPE_ID_FLOW_LABEL_TYPE                    0x80
// Enums
typedef enum{
    LIBLTE_MME_TFT_OPERATION_CODE_SPARE = 0,
    LIBLTE_MME_TFT_OPERATION_CODE_CREATE_NEW_TFT,
    LIBLTE_MME_TFT_OPERATION_CODE_DELETE_EXISTING_TFT,
    LIBLTE_MME_TFT_OPERATION_CODE_ADD_PACKET_FILTERS_TO_EXISTING_TFT,
    LIBLTE_MME_TFT_OPERATION_CODE_REPLACE_PACKET_FILTERS_IN_EXISTING_TFT,
    LIBLTE_MME_TFT_OPERATION_CODE_DELETE_PACKET_FILTERS_FROM_EXISTING_TFT,
    LIBLTE_MME_TFT_OPERATION_CODE_NO_TFT_OPERATION,
    LIBLTE_MME_TFT_OPERATION_CODE_RESERVED,
    LIBLTE_MME_TFT_OPERATION_CODE_N_ITEMS,
}LIBLTE_MME_TFT_OPERATION_CODE_ENUM;
static const char liblte_mme_tft_operation_code_text[LIBLTE_MME_TFT_OPERATION_CODE_N_ITEMS][100] = {"SPARE",
                                                                                                    "Create New TFT",
                                                                                                    "Delete Existing TFT",
                                                                                                    "Add Packet Filters to Existing TFT",
                                                                                                    "Replace Packet Filters in Existing TFT",
                                                                                                    "Delete Packet Filters from Existing TFT",
                                                                                                    "No TFT Operation",
                                                                                                    "RESERVED"};
typedef enum{
    LIBLTE_MME_TFT_PACKET_FILTER_DIRECTION_PRE_REL_7_TFT_FILTER = 0,
    LIBLTE_MME_TFT_PACKET_FILTER_DIRECTION_DOWNLINK_ONLY,
    LIBLTE_MME_TFT_PACKET_FILTER_DIRECTION_UPLINK_ONLY,
    LIBLTE_MME_TFT_PACKET_FILTER_DIRECTION_BIDIRECTIONAL,
    LIBLTE_MME_TFT_PACKET_FILTER_DIRECTION_N_ITEMS,
}LIBLTE_MME_TFT_PACKET_FILTER_DIRECTION_ENUM;
static const char liblte_mme_tft_packet_filter_direction_text[LIBLTE_MME_TFT_PACKET_FILTER_DIRECTION_N_ITEMS][100] = {"Pre Rel-7 TFT Filter",
                                                                                                                      "Downlink Only",
                                                                                                                      "Uplink Only",
                                                                                                                      "Bidirectional"};
// Structs
typedef struct{
    LIBLTE_MME_TFT_PACKET_FILTER_DIRECTION_ENUM dir;
    uint8                                       id;
    uint8                                       eval_precedence;
    uint8                                       filter[LIBLTE_MME_PACKET_FILTER_MAX_SIZE];
    uint8                                       filter_size;
}LIBLTE_MME_PACKET_FILTER_STRUCT;
typedef struct{
    uint8 id;
    uint8 parameter[LIBLTE_MME_PARAMETER_MAX_SIZE];
    uint8 parameter_size;
}LIBLTE_MME_PARAMETER_STRUCT;
typedef struct{
    LIBLTE_MME_PACKET_FILTER_STRUCT    packet_filter_list[LIBLTE_MME_PACKET_FILTER_LIST_MAX_SIZE];
    LIBLTE_MME_PARAMETER_STRUCT        parameter_list[LIBLTE_MME_PARAMETER_LIST_MAX_SIZE];
    LIBLTE_MME_TFT_OPERATION_CODE_ENUM tft_op_code;
    uint8                              packet_filter_list_size;
    uint8                              parameter_list_size;
}LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT;
typedef LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT LIBLTE_MME_TRAFFIC_FLOW_AGGREGATE_DESCRIPTION_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_traffic_flow_aggregate_description_ie(LIBLTE_MME_TRAFFIC_FLOW_AGGREGATE_DESCRIPTION_STRUCT  *tfad,
                                                                        uint8                                                **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_traffic_flow_aggregate_description_ie(uint8                                                **ie_ptr,
                                                                          LIBLTE_MME_TRAFFIC_FLOW_AGGREGATE_DESCRIPTION_STRUCT  *tfad);

/*********************************************************************
    IE Name: Traffic Flow Template

    Description: Specifies the TFT parameters and operations for a
                 PDP context.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.16
                        24.008 v10.2.0 Section 10.5.6.12
*********************************************************************/
// Defines
// Traffic Flow Template defines defined above
// Enums
// Traffic Flow Template enums defined above
// Structs
// Traffic Flow Template structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_traffic_flow_template_ie(LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT  *tft,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_traffic_flow_template_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT  *tft);

/*********************************************************************
    IE Name: Transaction Identifier

    Description: Represents the corresponding PDP context in A/Gb
                 mode or Iu mode which is mapped from the EPS bearer
                 context.

    Document Reference: 24.301 v10.2.0 Section 9.9.4.17
                        24.008 v10.2.0 Section 10.5.6.7
*********************************************************************/
// Defines
#define LIBLTE_MME_TI_FLAG_SENT_FROM_ORIGINATOR 0
#define LIBLTE_MME_TI_FLAG_SENT_TO_ORIGINATOR   1
#define LIBLTE_MME_TI_VALUE_IS_GIVEN_BY_TIE     0x7
// Enums
// Structs
typedef struct{
    uint8 ti_flag;
    uint8 tio;
    uint8 tie;
}LIBLTE_MME_TRANSACTION_IDENTIFIER_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_transaction_identifier_ie(LIBLTE_MME_TRANSACTION_IDENTIFIER_STRUCT  *trans_id,
                                                            uint8                                    **ie_ptr);
LIBLTE_ERROR_ENUM liblte_mme_unpack_transaction_identifier_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_MME_TRANSACTION_IDENTIFIER_STRUCT  *trans_id);

/*******************************************************************************
                              MESSAGE DECLARATIONS
*******************************************************************************/

/*********************************************************************
    Message Name: Message Header (Plain NAS Message)

    Description: Message header for plain NAS messages.

    Document Reference: 24.301 v10.2.0 Section 9.1
*********************************************************************/
// Defines
#define LIBLTE_MME_PD_EPS_SESSION_MANAGEMENT                                              0x2
#define LIBLTE_MME_PD_EPS_MOBILITY_MANAGEMENT                                             0x7
#define LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS                                            0x0
#define LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY                                            0x1
#define LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED                               0x2
#define LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_WITH_NEW_EPS_SECURITY_CONTEXT              0x3
#define LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED_WITH_NEW_EPS_SECURITY_CONTEXT 0x4
#define LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST                                      0xC
#define LIBLTE_MME_MSG_TYPE_ATTACH_REQUEST                                                0x41
#define LIBLTE_MME_MSG_TYPE_ATTACH_ACCEPT                                                 0x42
#define LIBLTE_MME_MSG_TYPE_ATTACH_COMPLETE                                               0x43
#define LIBLTE_MME_MSG_TYPE_ATTACH_REJECT                                                 0x44
#define LIBLTE_MME_MSG_TYPE_DETACH_REQUEST                                                0x45
#define LIBLTE_MME_MSG_TYPE_DETACH_ACCEPT                                                 0x46
#define LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST                                  0x48
#define LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_ACCEPT                                   0x49
#define LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_COMPLETE                                 0x4A
#define LIBLTE_MME_MSG_TYPE_TRACKING_AREA_UPDATE_REJECT                                   0x4B
#define LIBLTE_MME_MSG_TYPE_EXTENDED_SERVICE_REQUEST                                      0x4C
#define LIBLTE_MME_MSG_TYPE_SERVICE_REJECT                                                0x4E
#define LIBLTE_MME_MSG_TYPE_GUTI_REALLOCATION_COMMAND                                     0x50
#define LIBLTE_MME_MSG_TYPE_GUTI_REALLOCATION_COMPLETE                                    0x51
#define LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REQUEST                                        0x52
#define LIBLTE_MME_MSG_TYPE_AUTHENTICATION_RESPONSE                                       0x53
#define LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REJECT                                         0x54
#define LIBLTE_MME_MSG_TYPE_AUTHENTICATION_FAILURE                                        0x5C
#define LIBLTE_MME_MSG_TYPE_IDENTITY_REQUEST                                              0x55
#define LIBLTE_MME_MSG_TYPE_IDENTITY_RESPONSE                                             0x56
#define LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMMAND                                         0x5D
#define LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMPLETE                                        0x5E
#define LIBLTE_MME_MSG_TYPE_SECURITY_MODE_REJECT                                          0x5F
#define LIBLTE_MME_MSG_TYPE_EMM_STATUS                                                    0x60
#define LIBLTE_MME_MSG_TYPE_EMM_INFORMATION                                               0x61
#define LIBLTE_MME_MSG_TYPE_DOWNLINK_NAS_TRANSPORT                                        0x62
#define LIBLTE_MME_MSG_TYPE_UPLINK_NAS_TRANSPORT                                          0x63
#define LIBLTE_MME_MSG_TYPE_CS_SERVICE_NOTIFICATION                                       0x64
#define LIBLTE_MME_MSG_TYPE_DOWNLINK_GENERIC_NAS_TRANSPORT                                0x68
#define LIBLTE_MME_MSG_TYPE_UPLINK_GENERIC_NAS_TRANSPORT                                  0x69
#define LIBLTE_MME_MSG_TYPE_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST                   0xC1
#define LIBLTE_MME_MSG_TYPE_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT                    0xC2
#define LIBLTE_MME_MSG_TYPE_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT                    0xC3
#define LIBLTE_MME_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST                 0xC5
#define LIBLTE_MME_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT                  0xC6
#define LIBLTE_MME_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT                  0xC7
#define LIBLTE_MME_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_REQUEST                             0xC9
#define LIBLTE_MME_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_ACCEPT                              0xCA
#define LIBLTE_MME_MSG_TYPE_MODIFY_EPS_BEARER_CONTEXT_REJECT                              0xCB
#define LIBLTE_MME_MSG_TYPE_DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST                         0xCD
#define LIBLTE_MME_MSG_TYPE_DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT                          0xCE
#define LIBLTE_MME_MSG_TYPE_PDN_CONNECTIVITY_REQUEST                                      0xD0
#define LIBLTE_MME_MSG_TYPE_PDN_CONNECTIVITY_REJECT                                       0xD1
#define LIBLTE_MME_MSG_TYPE_PDN_DISCONNECT_REQUEST                                        0xD2
#define LIBLTE_MME_MSG_TYPE_PDN_DISCONNECT_REJECT                                         0xD3
#define LIBLTE_MME_MSG_TYPE_BEARER_RESOURCE_ALLOCATION_REQUEST                            0xD4
#define LIBLTE_MME_MSG_TYPE_BEARER_RESOURCE_ALLOCATION_REJECT                             0xD5
#define LIBLTE_MME_MSG_TYPE_BEARER_RESOURCE_MODIFICATION_REQUEST                          0xD6
#define LIBLTE_MME_MSG_TYPE_BEARER_RESOURCE_MODIFICATION_REJECT                           0xD7
#define LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_REQUEST                                       0xD9
#define LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_RESPONSE                                      0xDA
#define LIBLTE_MME_MSG_TYPE_NOTIFICATION                                                  0xDB
#define LIBLTE_MME_MSG_TYPE_ESM_STATUS                                                    0xE8
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_mme_parse_msg_sec_header(LIBLTE_BYTE_MSG_STRUCT *msg,
                                                  uint8                  *pd,
                                                  uint8                  *sec_hdr_type);
LIBLTE_ERROR_ENUM liblte_mme_parse_msg_header(LIBLTE_BYTE_MSG_STRUCT *msg,
                                              uint8                  *pd,
                                              uint8                  *msg_type);
LIBLTE_ERROR_ENUM liblte_mme_pack_security_protected_nas_msg(LIBLTE_BYTE_MSG_STRUCT *msg,
                                                             uint8                   sec_hdr_type,
                                                             uint32                  count,
                                                             LIBLTE_BYTE_MSG_STRUCT *sec_msg);

/*********************************************************************
    Message Name: Attach Accept

    Description: Sent by the network to the UE to indicate that the
                 corresponding attach request has been accepted.

    Document Reference: 24.301 v10.2.0 Section 8.2.1
*********************************************************************/
// Defines
#define LIBLTE_MME_GUTI_IEI                         0x50
#define LIBLTE_MME_LOCATION_AREA_IDENTIFICATION_IEI 0x13
#define LIBLTE_MME_MS_IDENTITY_IEI                  0x23
#define LIBLTE_MME_EMM_CAUSE_IEI                    0x53
#define LIBLTE_MME_T3402_VALUE_IEI                  0x17
#define LIBLTE_MME_T3423_VALUE_IEI                  0x59
#define LIBLTE_MME_EQUIVALENT_PLMNS_IEI             0x4A
#define LIBLTE_MME_EMERGENCY_NUMBER_LIST_IEI        0x34
#define LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_IEI  0x64
#define LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_IEI     0xF
#define LIBLTE_MME_T3412_EXTENDED_VALUE_IEI         0x5E
// Enums
// Structs
typedef struct{
    LIBLTE_MME_GPRS_TIMER_STRUCT                  t3412;
    LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_STRUCT tai_list;
    LIBLTE_BYTE_MSG_STRUCT                        esm_msg;
    LIBLTE_MME_EPS_MOBILE_ID_STRUCT               guti;
    LIBLTE_MME_LOCATION_AREA_ID_STRUCT            lai;
    LIBLTE_MME_MOBILE_ID_STRUCT                   ms_id;
    LIBLTE_MME_GPRS_TIMER_STRUCT                  t3402;
    LIBLTE_MME_GPRS_TIMER_STRUCT                  t3423;
    LIBLTE_MME_PLMN_LIST_STRUCT                   equivalent_plmns;
    LIBLTE_MME_EMERGENCY_NUMBER_LIST_STRUCT       emerg_num_list;
    LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_STRUCT eps_network_feature_support;
    LIBLTE_MME_GPRS_TIMER_3_STRUCT                t3412_ext;
    LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_ENUM      additional_update_result;
    uint8                                         eps_attach_result;
    uint8                                         emm_cause;
    bool                                          guti_present;
    bool                                          lai_present;
    bool                                          ms_id_present;
    bool                                          emm_cause_present;
    bool                                          t3402_present;
    bool                                          t3423_present;
    bool                                          equivalent_plmns_present;
    bool                                          emerg_num_list_present;
    bool                                          eps_network_feature_support_present;
    bool                                          additional_update_result_present;
    bool                                          t3412_ext_present;
}LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_attach_accept_msg(LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT *attach_accept,
                                                    uint8                                sec_hdr_type,
                                                    uint32                               count,
                                                    LIBLTE_BYTE_MSG_STRUCT              *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_attach_accept_msg(LIBLTE_BYTE_MSG_STRUCT              *msg,
                                                      LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT *attach_accept);

/*********************************************************************
    Message Name: Attach Complete

    Description: Sent by the UE to the network in response to an
                 ATTACH ACCEPT message.

    Document Reference: 24.301 v10.2.0 Section 8.2.2
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_BYTE_MSG_STRUCT esm_msg;
}LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_attach_complete_msg(LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT *attach_comp,
                                                      uint8                                  sec_hdr_type,
                                                      uint32                                 count,
                                                      LIBLTE_BYTE_MSG_STRUCT                *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_attach_complete_msg(LIBLTE_BYTE_MSG_STRUCT                *msg,
                                                        LIBLTE_MME_ATTACH_COMPLETE_MSG_STRUCT *attach_comp);

/*********************************************************************
    Message Name: Attach Reject

    Description: Sent by the network to the UE to indicate that the
                 corresponding attach request has been rejected.

    Document Reference: 24.301 v10.2.0 Section 8.2.3
*********************************************************************/
// Defines
#define LIBLTE_MME_ESM_MSG_CONTAINER_IEI 0x78
#define LIBLTE_MME_T3446_VALUE_IEI       0x5F
// Enums
// Structs
typedef struct{
    LIBLTE_BYTE_MSG_STRUCT esm_msg;
    uint8                  emm_cause;
    uint8                  t3446_value;
    bool                   esm_msg_present;
    bool                   t3446_value_present;
}LIBLTE_MME_ATTACH_REJECT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_attach_reject_msg(LIBLTE_MME_ATTACH_REJECT_MSG_STRUCT *attach_rej,
                                                    LIBLTE_BYTE_MSG_STRUCT              *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_attach_reject_msg(LIBLTE_BYTE_MSG_STRUCT              *msg,
                                                      LIBLTE_MME_ATTACH_REJECT_MSG_STRUCT *attach_rej);

/*********************************************************************
    Message Name: Attach Request

    Description: Sent by the UE to the network to perform an attach
                 procedure.

    Document Reference: 24.301 v10.2.0 Section 8.2.4
*********************************************************************/
// Defines
#define LIBLTE_MME_P_TMSI_SIGNATURE_IEI                       0x19
#define LIBLTE_MME_ADDITIONAL_GUTI_IEI                        0x50
#define LIBLTE_MME_LAST_VISITED_REGISTERED_TAI_IEI            0x52
#define LIBLTE_MME_DRX_PARAMETER_IEI                          0x5C
#define LIBLTE_MME_MS_NETWORK_CAPABILITY_IEI                  0x31
#define LIBLTE_MME_TMSI_STATUS_IEI                            0x9
#define LIBLTE_MME_MS_CLASSMARK_2_IEI                         0x11
#define LIBLTE_MME_MS_CLASSMARK_3_IEI                         0x20
#define LIBLTE_MME_SUPPORTED_CODEC_LIST_IEI                   0x40
#define LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_IEI                 0xF
#define LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_IEI 0x5D
#define LIBLTE_MME_ATTACH_REQUEST_DEVICE_PROPERTIES_IEI       0xD
#define LIBLTE_MME_GUTI_TYPE_IEI                              0xE
// Enums
// Structs
typedef struct{
    LIBLTE_MME_NAS_KEY_SET_ID_STRUCT                         nas_ksi;
    LIBLTE_MME_EPS_MOBILE_ID_STRUCT                          eps_mobile_id;
    LIBLTE_MME_UE_NETWORK_CAPABILITY_STRUCT                  ue_network_cap;
    LIBLTE_BYTE_MSG_STRUCT                                   esm_msg;
    LIBLTE_MME_EPS_MOBILE_ID_STRUCT                          additional_guti;
    LIBLTE_MME_TRACKING_AREA_ID_STRUCT                       last_visited_registered_tai;
    LIBLTE_MME_DRX_PARAMETER_STRUCT                          drx_param;
    LIBLTE_MME_MS_NETWORK_CAPABILITY_STRUCT                  ms_network_cap;
    LIBLTE_MME_LOCATION_AREA_ID_STRUCT                       old_lai;
    LIBLTE_MME_MOBILE_STATION_CLASSMARK_2_STRUCT             ms_cm2;
    LIBLTE_MME_MOBILE_STATION_CLASSMARK_3_STRUCT             ms_cm3;
    LIBLTE_MME_SUPPORTED_CODEC_LIST_STRUCT                   supported_codecs;
    LIBLTE_MME_VOICE_DOMAIN_PREF_AND_UE_USAGE_SETTING_STRUCT voice_domain_pref_and_ue_usage_setting;
    LIBLTE_MME_TMSI_STATUS_ENUM                              tmsi_status;
    LIBLTE_MME_ADDITIONAL_UPDATE_TYPE_ENUM                   additional_update_type;
    LIBLTE_MME_DEVICE_PROPERTIES_ENUM                        device_properties;
    LIBLTE_MME_GUTI_TYPE_ENUM                                old_guti_type;
    uint32                                                   old_p_tmsi_signature;
    uint8                                                    eps_attach_type;
    bool                                                     old_p_tmsi_signature_present;
    bool                                                     additional_guti_present;
    bool                                                     last_visited_registered_tai_present;
    bool                                                     drx_param_present;
    bool                                                     ms_network_cap_present;
    bool                                                     old_lai_present;
    bool                                                     tmsi_status_present;
    bool                                                     ms_cm2_present;
    bool                                                     ms_cm3_present;
    bool                                                     supported_codecs_present;
    bool                                                     additional_update_type_present;
    bool                                                     voice_domain_pref_and_ue_usage_setting_present;
    bool                                                     device_properties_present;
    bool                                                     old_guti_type_present;
}LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_attach_request_msg(LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req,
                                                     LIBLTE_BYTE_MSG_STRUCT               *msg);
LIBLTE_ERROR_ENUM liblte_mme_pack_attach_request_msg(LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req,
                                                     uint8                                 sec_hdr_type,
                                                     uint32                                count,
                                                     LIBLTE_BYTE_MSG_STRUCT               *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_attach_request_msg(LIBLTE_BYTE_MSG_STRUCT               *msg,
                                                       LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req);

/*********************************************************************
    Message Name: Authentication Failure

    Description: Sent by the UE to the network to indicate that
                 authentication of the network has failed.

    Document Reference: 24.301 v10.2.0 Section 8.2.5
*********************************************************************/
// Defines
#define LIBLTE_MME_AUTHENTICATION_FAILURE_PARAMETER_IEI 0x30
// Enums
// Structs
typedef struct{
    uint8 emm_cause;
    uint8 auth_fail_param[16];
    bool  auth_fail_param_present;
}LIBLTE_MME_AUTHENTICATION_FAILURE_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_failure_msg(LIBLTE_MME_AUTHENTICATION_FAILURE_MSG_STRUCT *auth_fail,
                                                             LIBLTE_BYTE_MSG_STRUCT                       *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_failure_msg(LIBLTE_BYTE_MSG_STRUCT                       *msg,
                                                               LIBLTE_MME_AUTHENTICATION_FAILURE_MSG_STRUCT *auth_fail);

/*********************************************************************
    Message Name: Authentication Reject

    Description: Sent by the network to the UE to indicate that the
                 authentication procedure has failed and that the UE
                 shall abort all activities.

    Document Reference: 24.301 v10.2.0 Section 8.2.6
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
}LIBLTE_MME_AUTHENTICATION_REJECT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_reject_msg(LIBLTE_MME_AUTHENTICATION_REJECT_MSG_STRUCT *auth_reject,
                                                            LIBLTE_BYTE_MSG_STRUCT                      *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_reject_msg(LIBLTE_BYTE_MSG_STRUCT                      *msg,
                                                              LIBLTE_MME_AUTHENTICATION_REJECT_MSG_STRUCT *auth_reject);

/*********************************************************************
    Message Name: Authentication Request

    Description: Sent by the network to the UE to initiate
                 authentication of the UE identity.

    Document Reference: 24.301 v10.2.0 Section 8.2.7
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_NAS_KEY_SET_ID_STRUCT nas_ksi;
    uint8                            autn[16];
    uint8                            rand[16];
}LIBLTE_MME_AUTHENTICATION_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_request_msg(LIBLTE_MME_AUTHENTICATION_REQUEST_MSG_STRUCT *auth_req,
                                                             LIBLTE_BYTE_MSG_STRUCT                       *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_request_msg(LIBLTE_BYTE_MSG_STRUCT                       *msg,
                                                               LIBLTE_MME_AUTHENTICATION_REQUEST_MSG_STRUCT *auth_req);

/*********************************************************************
    Message Name: Authentication Response

    Description: Sent by the UE to the network to deliver a calculated
                 authentication response to the network.

    Document Reference: 24.301 v10.2.0 Section 8.2.8
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 res[16];
    int res_len;
}LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_authentication_response_msg(LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT *auth_resp,
                                                              LIBLTE_BYTE_MSG_STRUCT                        *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_authentication_response_msg(LIBLTE_BYTE_MSG_STRUCT                        *msg,
                                                                LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT *auth_resp);

/*********************************************************************
    Message Name: CS Service Notification

    Description: Sent by the network when a paging request with CS
                 call indicator was received via SGs for a UE, and a
                 NAS signalling connection is already established for
                 the UE.

    Document Reference: 24.301 v10.2.0 Section 8.2.9
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    Message Name: Detach Accept

    Description: Sent by the network to indicate that the detach
                 procedure has been completed.

    Document Reference: 24.301 v10.2.0 Section 8.2.10
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
}LIBLTE_MME_DETACH_ACCEPT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_detach_accept_msg(LIBLTE_MME_DETACH_ACCEPT_MSG_STRUCT *detach_accept,
                                                    uint8                                sec_hdr_type,
                                                    uint32                               count,
                                                    LIBLTE_BYTE_MSG_STRUCT              *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_detach_accept_msg(LIBLTE_BYTE_MSG_STRUCT              *msg,
                                                      LIBLTE_MME_DETACH_ACCEPT_MSG_STRUCT *detach_accept);

/*********************************************************************
    Message Name: Detach Request

    Description: Sent by the UE to request the release of an EMM
                 context.

    Document Reference: 24.301 v10.2.0 Section 8.2.11
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_DETACH_TYPE_STRUCT    detach_type;
    LIBLTE_MME_NAS_KEY_SET_ID_STRUCT nas_ksi;
    LIBLTE_MME_EPS_MOBILE_ID_STRUCT  eps_mobile_id;
}LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_detach_request_msg(LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT *detach_req,
                                                     uint8                                 sec_hdr_type,
                                                     uint32                                count,
                                                     LIBLTE_BYTE_MSG_STRUCT               *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_detach_request_msg(LIBLTE_BYTE_MSG_STRUCT               *msg,
                                                       LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT *detach_req);

/*********************************************************************
    Message Name: Downlink NAS Transport

    Description: Sent by the network to the UE in order to carry an
                 SMS message in encapsulated format.

    Document Reference: 24.301 v10.2.0 Section 8.2.12
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_BYTE_MSG_STRUCT nas_msg;
}LIBLTE_MME_DOWNLINK_NAS_TRANSPORT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_downlink_nas_transport_msg(LIBLTE_MME_DOWNLINK_NAS_TRANSPORT_MSG_STRUCT *dl_nas_transport,
                                                             uint8                                         sec_hdr_type,
                                                             uint32                                        count,
                                                             LIBLTE_BYTE_MSG_STRUCT                       *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_downlink_nas_transport_msg(LIBLTE_BYTE_MSG_STRUCT                       *msg,
                                                               LIBLTE_MME_DOWNLINK_NAS_TRANSPORT_MSG_STRUCT *dl_nas_transport);

/*********************************************************************
    Message Name: EMM Information

    Description: Sent by the network at any time during EMM context is
                 established to send certain information to the UE.

    Document Reference: 24.301 v10.2.0 Section 8.2.13
*********************************************************************/
// Defines
#define LIBLTE_MME_FULL_NAME_FOR_NETWORK_IEI              0x43
#define LIBLTE_MME_SHORT_NAME_FOR_NETWORK_IEI             0x45
#define LIBLTE_MME_LOCAL_TIME_ZONE_IEI                    0x46
#define LIBLTE_MME_UNIVERSAL_TIME_AND_LOCAL_TIME_ZONE_IEI 0x47
#define LIBLTE_MME_NETWORK_DAYLIGHT_SAVING_TIME_IEI       0x49
// Enums
// Structs
typedef struct{
    LIBLTE_MME_NETWORK_NAME_STRUCT       full_net_name;
    LIBLTE_MME_NETWORK_NAME_STRUCT       short_net_name;
    LIBLTE_MME_TIME_ZONE_AND_TIME_STRUCT utc_and_local_time_zone;
    LIBLTE_MME_DAYLIGHT_SAVING_TIME_ENUM net_dst;
    uint8                                local_time_zone;
    bool                                 full_net_name_present;
    bool                                 short_net_name_present;
    bool                                 local_time_zone_present;
    bool                                 utc_and_local_time_zone_present;
    bool                                 net_dst_present;
}LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_emm_information_msg(LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT *emm_info,
                                                      uint8                                  sec_hdr_type,
                                                      uint32                                 count,
                                                      LIBLTE_BYTE_MSG_STRUCT                *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_emm_information_msg(LIBLTE_BYTE_MSG_STRUCT                *msg,
                                                        LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT *emm_info);

/*********************************************************************
    Message Name: EMM Status

    Description: Sent by the UE or by the network at any time to
                 report certain error conditions.

    Document Reference: 24.301 v10.2.0 Section 8.2.14
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 emm_cause;
}LIBLTE_MME_EMM_STATUS_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_emm_status_msg(LIBLTE_MME_EMM_STATUS_MSG_STRUCT *emm_status,
                                                 uint8                             sec_hdr_type,
                                                 uint32                            count,
                                                 LIBLTE_BYTE_MSG_STRUCT           *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_emm_status_msg(LIBLTE_BYTE_MSG_STRUCT           *msg,
                                                   LIBLTE_MME_EMM_STATUS_MSG_STRUCT *emm_status);

/*********************************************************************
    Message Name: Extended Service Request

    Description: Sent by the UE to the network to initiate a CS
                 fallback or 1xCS fallback call or respond to a mobile
                 terminated CS fallback or 1xCS fallback request from
                 the network or to request the establishment of a NAS
                 signalling connection and of the radio and S1 bearers
                 for packet services, if the UE needs to provide
                 additional information that cannot be provided via a
                 SERVICE REQUEST message.

    Document Reference: 24.301 v10.2.0 Section 8.2.15
*********************************************************************/
// Defines
#define LIBLTE_MME_CSFB_RESPONSE_IEI                              0xB
#define LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_IEI                  0x57
#define LIBLTE_MME_EXTENDED_SERVICE_REQUEST_DEVICE_PROPERTIES_IEI 0xD
// Enums
// Structs
typedef struct{
    LIBLTE_MME_NAS_KEY_SET_ID_STRUCT            nas_ksi;
    LIBLTE_MME_MOBILE_ID_STRUCT                 m_tmsi;
    LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_STRUCT eps_bearer_context_status;
    LIBLTE_MME_DEVICE_PROPERTIES_ENUM           device_props;
    uint8                                       service_type;
    uint8                                       csfb_resp;
    bool                                        csfb_resp_present;
    bool                                        eps_bearer_context_status_present;
    bool                                        device_props_present;
}LIBLTE_MME_EXTENDED_SERVICE_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_extended_service_request_msg(LIBLTE_MME_EXTENDED_SERVICE_REQUEST_MSG_STRUCT *ext_service_req,
                                                               uint8                                           sec_hdr_type,
                                                               uint32                                          count,
                                                               LIBLTE_BYTE_MSG_STRUCT                         *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_extended_service_request_msg(LIBLTE_BYTE_MSG_STRUCT                         *msg,
                                                                 LIBLTE_MME_EXTENDED_SERVICE_REQUEST_MSG_STRUCT *ext_service_req);

/*********************************************************************
    Message Name: GUTI Reallocation Command

    Description: Sent by the network to the UE to reallocate a GUTI
                 and optionally provide a new TAI list.

    Document Reference: 24.301 v10.2.0 Section 8.2.16
*********************************************************************/
// Defines
#define LIBLTE_MME_TAI_LIST_IEI 0x54
// Enums
// Structs
typedef struct{
    LIBLTE_MME_EPS_MOBILE_ID_STRUCT               guti;
    LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_STRUCT tai_list;
    bool                                          tai_list_present;
}LIBLTE_MME_GUTI_REALLOCATION_COMMAND_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_guti_reallocation_command_msg(LIBLTE_MME_GUTI_REALLOCATION_COMMAND_MSG_STRUCT *guti_realloc_cmd,
                                                                uint8                                            sec_hdr_type,
                                                                uint32                                           count,
                                                                LIBLTE_BYTE_MSG_STRUCT                          *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_guti_reallocation_command_msg(LIBLTE_BYTE_MSG_STRUCT                          *msg,
                                                                  LIBLTE_MME_GUTI_REALLOCATION_COMMAND_MSG_STRUCT *guti_realloc_cmd);

/*********************************************************************
    Message Name: GUTI Reallocation Complete

    Description: Sent by the UE to the network to indicate that
                 reallocation of a GUTI has taken place.

    Document Reference: 24.301 v10.2.0 Section 8.2.17
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
}LIBLTE_MME_GUTI_REALLOCATION_COMPLETE_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_guti_reallocation_complete_msg(LIBLTE_MME_GUTI_REALLOCATION_COMPLETE_MSG_STRUCT *guti_realloc_complete,
                                                                 uint8                                             sec_hdr_type,
                                                                 uint32                                            count,
                                                                 LIBLTE_BYTE_MSG_STRUCT                           *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_guti_reallocation_complete_msg(LIBLTE_BYTE_MSG_STRUCT                           *msg,
                                                                   LIBLTE_MME_GUTI_REALLOCATION_COMPLETE_MSG_STRUCT *guti_realloc_complete);

/*********************************************************************
    Message Name: Identity Request

    Description: Sent by the network to the UE to request the UE to
                 provide the specified identity.

    Document Reference: 24.301 v10.2.0 Section 8.2.18
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 id_type;
}LIBLTE_MME_ID_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_identity_request_msg(LIBLTE_MME_ID_REQUEST_MSG_STRUCT *id_req,
                                                       LIBLTE_BYTE_MSG_STRUCT           *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_identity_request_msg(LIBLTE_BYTE_MSG_STRUCT           *msg,
                                                         LIBLTE_MME_ID_REQUEST_MSG_STRUCT *id_req);

/*********************************************************************
    Message Name: Identity Response

    Description: Sent by the UE to the network in response to an
                 IDENTITY REQUEST message and provides the requested
                 identity.

    Document Reference: 24.301 v10.2.0 Section 8.2.19
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_MOBILE_ID_STRUCT mobile_id;
}LIBLTE_MME_ID_RESPONSE_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_identity_response_msg(LIBLTE_MME_ID_RESPONSE_MSG_STRUCT *id_resp,
                                                        LIBLTE_BYTE_MSG_STRUCT            *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_identity_response_msg(LIBLTE_BYTE_MSG_STRUCT            *msg,
                                                          LIBLTE_MME_ID_RESPONSE_MSG_STRUCT *id_resp);

/*********************************************************************
    Message Name: Security Mode Command

    Description: Sent by the network to the UE to establish NAS
                 signalling security.

    Document Reference: 24.301 v10.2.0 Section 8.2.20
*********************************************************************/
// Defines
#define LIBLTE_MME_IMEISV_REQUEST_IEI    0xC
#define LIBLTE_MME_REPLAYED_NONCE_UE_IEI 0x55
#define LIBLTE_MME_NONCE_MME_IEI         0x56
// Enums
// Structs
typedef struct{
    LIBLTE_MME_NAS_SECURITY_ALGORITHMS_STRUCT  selected_nas_sec_algs;
    LIBLTE_MME_NAS_KEY_SET_ID_STRUCT           nas_ksi;
    LIBLTE_MME_UE_SECURITY_CAPABILITIES_STRUCT ue_security_cap;
    LIBLTE_MME_IMEISV_REQUEST_ENUM             imeisv_req;
    uint32                                     nonce_ue;
    uint32                                     nonce_mme;
    bool                                       imeisv_req_present;
    bool                                       nonce_ue_present;
    bool                                       nonce_mme_present;
}LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_security_mode_command_msg(LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT *sec_mode_cmd,
                                                            uint8                                        sec_hdr_type,
                                                            uint32                                       count,
                                                            LIBLTE_BYTE_MSG_STRUCT                      *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_security_mode_command_msg(LIBLTE_BYTE_MSG_STRUCT                      *msg,
                                                              LIBLTE_MME_SECURITY_MODE_COMMAND_MSG_STRUCT *sec_mode_cmd);

/*********************************************************************
    Message Name: Security Mode Complete

    Description: Sent by the UE to the network in response to a
                 SECURITY MODE COMMAND message.

    Document Reference: 24.301 v10.2.0 Section 8.2.21
*********************************************************************/
// Defines
#define LIBLTE_MME_IMEISV_IEI 0x23
// Enums
// Structs
typedef struct{
    LIBLTE_MME_MOBILE_ID_STRUCT imeisv;
    bool                        imeisv_present;
}LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_security_mode_complete_msg(LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT *sec_mode_comp,
                                                             uint8                                         sec_hdr_type,
                                                             uint32                                        count,
                                                             LIBLTE_BYTE_MSG_STRUCT                       *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_security_mode_complete_msg(LIBLTE_BYTE_MSG_STRUCT                       *msg,
                                                               LIBLTE_MME_SECURITY_MODE_COMPLETE_MSG_STRUCT *sec_mode_comp);

/*********************************************************************
    Message Name: Security Mode Reject

    Description: Sent by the UE to the network to indicate that the
                 corresponding security mode command has been
                 rejected.

    Document Reference: 24.301 v10.2.0 Section 8.2.22
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 emm_cause;
}LIBLTE_MME_SECURITY_MODE_REJECT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_security_mode_reject_msg(LIBLTE_MME_SECURITY_MODE_REJECT_MSG_STRUCT *sec_mode_rej,
                                                           LIBLTE_BYTE_MSG_STRUCT                     *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_security_mode_reject_msg(LIBLTE_BYTE_MSG_STRUCT                     *msg,
                                                             LIBLTE_MME_SECURITY_MODE_REJECT_MSG_STRUCT *sec_mode_rej);

/*********************************************************************
    Message Name: Service Reject

    Description: Sent by the network to the UE in order to reject the
                 service request procedure.

    Document Reference: 24.301 v10.2.0 Section 8.2.24
*********************************************************************/
// Defines
#define LIBLTE_MME_T3442_VALUE_IEI 0x5B
// Enums
// Structs
typedef struct{
    LIBLTE_MME_GPRS_TIMER_STRUCT t3442;
    uint8                        emm_cause;
    uint8                        t3446;
    bool                         t3442_present;
    bool                         t3446_present;
}LIBLTE_MME_SERVICE_REJECT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_service_reject_msg(LIBLTE_MME_SERVICE_REJECT_MSG_STRUCT *service_rej,
                                                     uint8                                 sec_hdr_type,
                                                     uint32                                count,
                                                     LIBLTE_BYTE_MSG_STRUCT               *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_service_reject_msg(LIBLTE_BYTE_MSG_STRUCT               *msg,
                                                       LIBLTE_MME_SERVICE_REJECT_MSG_STRUCT *service_rej);

/*********************************************************************
    Message Name: Service Request

    Description: Sent by the UE to the network to request the
                 establishment of a NAS signalling connection and of
                 the radio and S1 bearers.

    Document Reference: 24.301 v10.2.0 Section 8.2.25
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_KSI_AND_SEQUENCE_NUMBER_STRUCT ksi_and_seq_num;
    uint16                                    short_mac;
}LIBLTE_MME_SERVICE_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_service_request_msg(LIBLTE_MME_SERVICE_REQUEST_MSG_STRUCT *service_req,
                                                      LIBLTE_BYTE_MSG_STRUCT                *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_service_request_msg(LIBLTE_BYTE_MSG_STRUCT                *msg,
                                                        LIBLTE_MME_SERVICE_REQUEST_MSG_STRUCT *service_req);

/*********************************************************************
    Message Name: Tracking Area Update Accept

    Description: Sent by the network to the UE to provide the UE with
                 EPS mobility management related data in response to
                 a tracking area update request message.

    Document Reference: 24.301 v10.2.0 Section 8.2.26
*********************************************************************/
// Defines
#define LIBLTE_MME_T3412_VALUE_IEI 0x5A
// Enums
// Structs
typedef struct{
    LIBLTE_MME_GPRS_TIMER_STRUCT                  t3412;
    LIBLTE_MME_EPS_MOBILE_ID_STRUCT               guti;
    LIBLTE_MME_TRACKING_AREA_IDENTITY_LIST_STRUCT tai_list;
    LIBLTE_MME_EPS_BEARER_CONTEXT_STATUS_STRUCT   eps_bearer_context_status;
    LIBLTE_MME_LOCATION_AREA_ID_STRUCT            lai;
    LIBLTE_MME_MOBILE_ID_STRUCT                   ms_id;
    LIBLTE_MME_GPRS_TIMER_STRUCT                  t3402;
    LIBLTE_MME_GPRS_TIMER_STRUCT                  t3423;
    LIBLTE_MME_PLMN_LIST_STRUCT                   equivalent_plmns;
    LIBLTE_MME_EMERGENCY_NUMBER_LIST_STRUCT       emerg_num_list;
    LIBLTE_MME_EPS_NETWORK_FEATURE_SUPPORT_STRUCT eps_network_feature_support;
    LIBLTE_MME_GPRS_TIMER_3_STRUCT                t3412_ext;
    LIBLTE_MME_ADDITIONAL_UPDATE_RESULT_ENUM      additional_update_result;
    uint8                                         eps_update_result;
    uint8                                         emm_cause;
    bool                                          t3412_present;
    bool                                          guti_present;
    bool                                          tai_list_present;
    bool                                          eps_bearer_context_status_present;
    bool                                          lai_present;
    bool                                          ms_id_present;
    bool                                          emm_cause_present;
    bool                                          t3402_present;
    bool                                          t3423_present;
    bool                                          equivalent_plmns_present;
    bool                                          emerg_num_list_present;
    bool                                          eps_network_feature_support_present;
    bool                                          additional_update_result_present;
    bool                                          t3412_ext_present;
}LIBLTE_MME_TRACKING_AREA_UPDATE_ACCEPT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_tracking_area_update_accept_msg(LIBLTE_MME_TRACKING_AREA_UPDATE_ACCEPT_MSG_STRUCT *ta_update_accept,
                                                                  uint8                                              sec_hdr_type,
                                                                  uint32                                             count,
                                                                  LIBLTE_BYTE_MSG_STRUCT                            *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_tracking_area_update_accept_msg(LIBLTE_BYTE_MSG_STRUCT                            *msg,
                                                                    LIBLTE_MME_TRACKING_AREA_UPDATE_ACCEPT_MSG_STRUCT *ta_update_accept);

/*********************************************************************
    Message Name: Tracking Area Update Complete

    Description: Sent by the UE to the network in response to a
                 tracking area update accept message if a GUTI has
                 been changed or a new TMSI has been assigned.

    Document Reference: 24.301 v10.2.0 Section 8.2.27
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
}LIBLTE_MME_TRACKING_AREA_UPDATE_COMPLETE_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_tracking_area_update_complete_msg(LIBLTE_MME_TRACKING_AREA_UPDATE_COMPLETE_MSG_STRUCT *ta_update_complete,
                                                                    uint8                                                sec_hdr_type,
                                                                    uint32                                               count,
                                                                    LIBLTE_BYTE_MSG_STRUCT                              *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_tracking_area_update_complete_msg(LIBLTE_BYTE_MSG_STRUCT                              *msg,
                                                                      LIBLTE_MME_TRACKING_AREA_UPDATE_COMPLETE_MSG_STRUCT *ta_update_complete);

/*********************************************************************
    Message Name: Tracking Area Update Reject

    Description: Sent by the network to the UE in order to reject the
                 tracking area updating procedure.

    Document Reference: 24.301 v10.2.0 Section 8.2.28
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 emm_cause;
    uint8 t3446;
    bool  t3446_present;
}LIBLTE_MME_TRACKING_AREA_UPDATE_REJECT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_tracking_area_update_reject_msg(LIBLTE_MME_TRACKING_AREA_UPDATE_REJECT_MSG_STRUCT *ta_update_rej,
                                                                  uint8                                              sec_hdr_type,
                                                                  uint32                                             count,
                                                                  LIBLTE_BYTE_MSG_STRUCT                            *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_tracking_area_update_reject_msg(LIBLTE_BYTE_MSG_STRUCT                            *msg,
                                                                    LIBLTE_MME_TRACKING_AREA_UPDATE_REJECT_MSG_STRUCT *ta_update_rej);

/*********************************************************************
    Message Name: Tracking Area Update Request

    Description: Sent by the UE to the network to initiate a tracking
                 area updating procedure.

    Document Reference: 24.301 v10.2.0 Section 8.2.29
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    Message Name: Uplink NAS Transport

    Description: Sent by the UE to the network in order to carry an
                 SMS message in encapsulated format.

    Document Reference: 24.301 v10.2.0 Section 8.2.30
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_BYTE_MSG_STRUCT nas_msg;
}LIBLTE_MME_UPLINK_NAS_TRANSPORT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_uplink_nas_transport_msg(LIBLTE_MME_UPLINK_NAS_TRANSPORT_MSG_STRUCT *ul_nas_transport,
                                                           uint8                                       sec_hdr_type,
                                                           uint32                                      count,
                                                           LIBLTE_BYTE_MSG_STRUCT                     *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_uplink_nas_transport_msg(LIBLTE_BYTE_MSG_STRUCT                     *msg,
                                                             LIBLTE_MME_UPLINK_NAS_TRANSPORT_MSG_STRUCT *ul_nas_transport);

/*********************************************************************
    Message Name: Downlink Generic NAS Transport

    Description: Sent by the network to the UE in order to carry an
                 application message in encapsulated format.

    Document Reference: 24.301 v10.2.0 Section 8.2.31
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_BYTE_MSG_STRUCT                   generic_msg_cont;
    LIBLTE_MME_ADDITIONAL_INFORMATION_STRUCT add_info;
    uint8                                    generic_msg_cont_type;
    bool                                     add_info_present;
}LIBLTE_MME_DOWNLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_downlink_generic_nas_transport_msg(LIBLTE_MME_DOWNLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT *dl_generic_nas_transport,
                                                                     uint8                                                 sec_hdr_type,
                                                                     uint32                                                count,
                                                                     LIBLTE_BYTE_MSG_STRUCT                               *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_downlink_generic_nas_transport_msg(LIBLTE_BYTE_MSG_STRUCT                               *msg,
                                                                       LIBLTE_MME_DOWNLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT *dl_generic_nas_transport);

/*********************************************************************
    Message Name: Uplink Generic NAS Transport

    Description: Sent by the UE to the network in order to carry an
                 application protocol message in encapsulated format.

    Document Reference: 24.301 v10.2.0 Section 8.2.32
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_BYTE_MSG_STRUCT                   generic_msg_cont;
    LIBLTE_MME_ADDITIONAL_INFORMATION_STRUCT add_info;
    uint8                                    generic_msg_cont_type;
    bool                                     add_info_present;
}LIBLTE_MME_UPLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_uplink_generic_nas_transport_msg(LIBLTE_MME_UPLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT *ul_generic_nas_transport,
                                                                   uint8                                               sec_hdr_type,
                                                                   uint32                                              count,
                                                                   LIBLTE_BYTE_MSG_STRUCT                             *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_uplink_generic_nas_transport_msg(LIBLTE_BYTE_MSG_STRUCT                             *msg,
                                                                     LIBLTE_MME_UPLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT *ul_generic_nas_transport);

/*********************************************************************
    Message Name: Activate Dedicated EPS Bearer Context Accept

    Description: Sent by the UE to the network to acknowledge
                 activation of a dedicated EPS bearer context
                 associated with the same PDN address(es) and APN as
                 an already active EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.1
*********************************************************************/
// Defines
#define LIBLTE_MME_PROTOCOL_CONFIGURATION_OPTIONS_IEI 0x27
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    bool                                      protocol_cnfg_opts_present;
}LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_activate_dedicated_eps_bearer_context_accept_msg(LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *act_ded_eps_bearer_context_accept,
                                                                                   LIBLTE_BYTE_MSG_STRUCT                                             *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_activate_dedicated_eps_bearer_context_accept_msg(LIBLTE_BYTE_MSG_STRUCT                                             *msg,
                                                                                     LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *act_ded_eps_bearer_context_accept);

/*********************************************************************
    Message Name: Activate Dedicated EPS Bearer Context Reject

    Description: Sent by the UE to the network to reject activation
                 of a dedicated EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.2
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    uint8                                     esm_cause;
    bool                                      protocol_cnfg_opts_present;
}LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_activate_dedicated_eps_bearer_context_reject_msg(LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT *act_ded_eps_bearer_context_rej,
                                                                                   LIBLTE_BYTE_MSG_STRUCT                                             *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_activate_dedicated_eps_bearer_context_reject_msg(LIBLTE_BYTE_MSG_STRUCT                                             *msg,
                                                                                     LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT *act_ded_eps_bearer_context_rej);

/*********************************************************************
    Message Name: Activate Dedicated EPS Bearer Context Request

    Description: Sent by the network to the UE to request activation
                 of a dedicated EPS bearer context associated with
                 the same PDN address(es) and APN as an already
                 active default EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.3
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_EPS_QUALITY_OF_SERVICE_STRUCT  eps_qos;
    LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT   tft;
    LIBLTE_MME_TRANSACTION_IDENTIFIER_STRUCT  transaction_id;
    LIBLTE_MME_QUALITY_OF_SERVICE_STRUCT      negotiated_qos;
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    uint8                                     linked_eps_bearer_id;
    uint8                                     llc_sapi;
    uint8                                     radio_prio;
    uint8                                     packet_flow_id;
    bool                                      transaction_id_present;
    bool                                      negotiated_qos_present;
    bool                                      llc_sapi_present;
    bool                                      radio_prio_present;
    bool                                      packet_flow_id_present;
    bool                                      protocol_cnfg_opts_present;
}LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_activate_dedicated_eps_bearer_context_request_msg(LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *act_ded_eps_bearer_context_req,
                                                                                    LIBLTE_BYTE_MSG_STRUCT                                              *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_activate_dedicated_eps_bearer_context_request_msg(LIBLTE_BYTE_MSG_STRUCT                                              *msg,
                                                                                      LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *act_ded_eps_bearer_context_req);

/*********************************************************************
    Message Name: Activate Default EPS Bearer Context Accept

    Description: Sent by the UE to the network to acknowledge
                 activation of a default EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.4
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    bool                                      protocol_cnfg_opts_present;
}LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_activate_default_eps_bearer_context_accept_msg(LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *act_def_eps_bearer_context_accept,
                                                                                 LIBLTE_BYTE_MSG_STRUCT                                           *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_activate_default_eps_bearer_context_accept_msg(LIBLTE_BYTE_MSG_STRUCT                                           *msg,
                                                                                   LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *act_def_eps_bearer_context_accept);

/*********************************************************************
    Message Name: Activate Default EPS Bearer Context Reject

    Description: Sent by the UE to the network to reject activation
                 of a default EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.5
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    uint8                                     esm_cause;
    bool                                      protocol_cnfg_opts_present;
}LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_activate_default_eps_bearer_context_reject_msg(LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT *act_def_eps_bearer_context_rej,
                                                                                 LIBLTE_BYTE_MSG_STRUCT                                           *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_activate_default_eps_bearer_context_reject_msg(LIBLTE_BYTE_MSG_STRUCT                                           *msg,
                                                                                   LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT *act_def_eps_bearer_context_rej);

/*********************************************************************
    Message Name: Activate Default EPS Bearer Context Request

    Description: Sent by the network to the UE to request activation
                 of a default EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.6
*********************************************************************/
// Defines
#define LIBLTE_MME_TRANSACTION_IDENTIFIER_IEI 0x5D
#define LIBLTE_MME_QUALITY_OF_SERVICE_IEI     0x30
#define LIBLTE_MME_LLC_SAPI_IEI               0x32
#define LIBLTE_MME_RADIO_PRIORITY_IEI         0x8
#define LIBLTE_MME_PACKET_FLOW_IDENTIFIER_IEI 0x34
#define LIBLTE_MME_APN_AMBR_IEI               0x5E
#define LIBLTE_MME_ESM_CAUSE_IEI              0x58
#define LIBLTE_MME_CONNECTIVITY_TYPE_IEI      0xB
// Enums
// Structs
typedef struct{
    LIBLTE_MME_EPS_QUALITY_OF_SERVICE_STRUCT         eps_qos;
    LIBLTE_MME_ACCESS_POINT_NAME_STRUCT              apn;
    LIBLTE_MME_PDN_ADDRESS_STRUCT                    pdn_addr;
    LIBLTE_MME_TRANSACTION_IDENTIFIER_STRUCT         transaction_id;
    LIBLTE_MME_QUALITY_OF_SERVICE_STRUCT             negotiated_qos;
    LIBLTE_MME_APN_AGGREGATE_MAXIMUM_BIT_RATE_STRUCT apn_ambr;
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT        protocol_cnfg_opts;
    uint8                                            eps_bearer_id;
    uint8                                            proc_transaction_id;
    uint8                                            llc_sapi;
    uint8                                            radio_prio;
    uint8                                            packet_flow_id;
    uint8                                            esm_cause;
    uint8                                            connectivity_type;
    bool                                             transaction_id_present;
    bool                                             negotiated_qos_present;
    bool                                             llc_sapi_present;
    bool                                             radio_prio_present;
    bool                                             packet_flow_id_present;
    bool                                             apn_ambr_present;
    bool                                             esm_cause_present;
    bool                                             protocol_cnfg_opts_present;
    bool                                             connectivity_type_present;
}LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_activate_default_eps_bearer_context_request_msg(LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *act_def_eps_bearer_context_req,
                                                                                  LIBLTE_BYTE_MSG_STRUCT                                            *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_activate_default_eps_bearer_context_request_msg(LIBLTE_BYTE_MSG_STRUCT                                            *msg,
                                                                                    LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *act_def_eps_bearer_context_req);

/*********************************************************************
    Message Name: Bearer Resource Allocation Reject

    Description: Sent by the network to the UE to reject the
                 allocation of a dedicated bearer resource.

    Document Reference: 24.301 v10.2.0 Section 8.3.7
*********************************************************************/
// Defines
#define LIBLTE_MME_T3496_VALUE_IEI 0x37
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    LIBLTE_MME_GPRS_TIMER_3_STRUCT            t3496;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    uint8                                     esm_cause;
    bool                                      protocol_cnfg_opts_present;
    bool                                      t3496_present;
}LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REJECT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_bearer_resource_allocation_reject_msg(LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REJECT_MSG_STRUCT *bearer_res_alloc_rej,
                                                                        LIBLTE_BYTE_MSG_STRUCT                                  *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_bearer_resource_allocation_reject_msg(LIBLTE_BYTE_MSG_STRUCT                                  *msg,
                                                                          LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REJECT_MSG_STRUCT *bearer_res_alloc_rej);

/*********************************************************************
    Message Name: Bearer Resource Allocation Request

    Description: Sent by the UE to the network to request the
                 allocation of a dedicated bearer resource.

    Document Reference: 24.301 v10.2.0 Section 8.3.8
*********************************************************************/
// Defines
#define LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REQUEST_DEVICE_PROPERTIES_IEI 0xC
// Enums
// Structs
typedef struct{
    LIBLTE_MME_TRAFFIC_FLOW_AGGREGATE_DESCRIPTION_STRUCT tfa;
    LIBLTE_MME_EPS_QUALITY_OF_SERVICE_STRUCT             req_tf_qos;
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT            protocol_cnfg_opts;
    LIBLTE_MME_DEVICE_PROPERTIES_ENUM                    device_properties;
    uint8                                                eps_bearer_id;
    uint8                                                proc_transaction_id;
    uint8                                                linked_eps_bearer_id;
    bool                                                 protocol_cnfg_opts_present;
    bool                                                 device_properties_present;
}LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_bearer_resource_allocation_request_msg(LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REQUEST_MSG_STRUCT *bearer_res_alloc_req,
                                                                         LIBLTE_BYTE_MSG_STRUCT                                   *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_bearer_resource_allocation_request_msg(LIBLTE_BYTE_MSG_STRUCT                                   *msg,
                                                                           LIBLTE_MME_BEARER_RESOURCE_ALLOCATION_REQUEST_MSG_STRUCT *bearer_res_alloc_req);

/*********************************************************************
    Message Name: Bearer Resource Modification Reject

    Description: Sent by the network to the UE to reject the
                 modification of a dedicated bearer resource.

    Document Reference: 24.301 v10.2.0 Section 8.3.9
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    LIBLTE_MME_GPRS_TIMER_3_STRUCT            t3496;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    uint8                                     esm_cause;
    bool                                      protocol_cnfg_opts_present;
    bool                                      t3496_present;
}LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REJECT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_bearer_resource_modification_reject_msg(LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REJECT_MSG_STRUCT *bearer_res_mod_rej,
                                                                          LIBLTE_BYTE_MSG_STRUCT                                    *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_bearer_resource_modification_reject_msg(LIBLTE_BYTE_MSG_STRUCT                                    *msg,
                                                                            LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REJECT_MSG_STRUCT *bearer_res_mod_rej);

/*********************************************************************
    Message Name: Bearer Resource Modification Request

    Description: Sent by the UE to the network to request the
                 modification of a dedicated bearer resource.

    Document Reference: 24.301 v10.2.0 Section 8.3.10
*********************************************************************/
// Defines
#define LIBLTE_MME_EPS_QUALITY_OF_SERVICE_IEI                                 0x5B
#define LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REQUEST_DEVICE_PROPERTIES_IEI 0xC
// Enums
// Structs
typedef struct{
    LIBLTE_MME_TRAFFIC_FLOW_AGGREGATE_DESCRIPTION_STRUCT tfa;
    LIBLTE_MME_EPS_QUALITY_OF_SERVICE_STRUCT             req_tf_qos;
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT            protocol_cnfg_opts;
    LIBLTE_MME_DEVICE_PROPERTIES_ENUM                    device_properties;
    uint8                                                eps_bearer_id;
    uint8                                                proc_transaction_id;
    uint8                                                eps_bearer_id_for_packet_filter;
    uint8                                                esm_cause;
    bool                                                 req_tf_qos_present;
    bool                                                 esm_cause_present;
    bool                                                 protocol_cnfg_opts_present;
    bool                                                 device_properties_present;
}LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_bearer_resource_modification_request_msg(LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REQUEST_MSG_STRUCT *bearer_res_mod_req,
                                                                           LIBLTE_BYTE_MSG_STRUCT                                     *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_bearer_resource_modification_request_msg(LIBLTE_BYTE_MSG_STRUCT                                     *msg,
                                                                             LIBLTE_MME_BEARER_RESOURCE_MODIFICATION_REQUEST_MSG_STRUCT *bearer_res_mod_req);

/*********************************************************************
    Message Name: Deactivate EPS Bearer Context Accept

    Description: Sent by the UE to acknowledge deactivation of the
                 EPS bearer context requested in the corresponding
                 deactivate EPS bearer context request message.

    Document Reference: 24.301 v10.2.0 Section 8.3.11
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    bool                                      protocol_cnfg_opts_present;
}LIBLTE_MME_DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_deactivate_eps_bearer_context_accept_msg(LIBLTE_MME_DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *deact_eps_bearer_context_accept,
                                                                           LIBLTE_BYTE_MSG_STRUCT                                     *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_deactivate_eps_bearer_context_accept_msg(LIBLTE_BYTE_MSG_STRUCT                                     *msg,
                                                                             LIBLTE_MME_DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *deact_eps_bearer_context_accept);

/*********************************************************************
    Message Name: Deactivate EPS Bearer Context Request

    Description: Sent by the network to request deactivation of an
                 EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.12
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    uint8                                     esm_cause;
    bool                                      protocol_cnfg_opts_present;
}LIBLTE_MME_DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_deactivate_eps_bearer_context_request_msg(LIBLTE_MME_DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *deact_eps_bearer_context_req,
                                                                            LIBLTE_BYTE_MSG_STRUCT                                      *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_deactivate_eps_bearer_context_request_msg(LIBLTE_BYTE_MSG_STRUCT                                      *msg,
                                                                              LIBLTE_MME_DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *deact_eps_bearer_context_req);

/*********************************************************************
    Message Name: ESM Information Request

    Description: Sent by the network to the UE to request the UE to
                 provide ESM information, i.e. protocol configuration
                 options or APN or both.

    Document Reference: 24.301 v10.2.0 Section 8.3.13
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 eps_bearer_id;
    uint8 proc_transaction_id;
}LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM srslte_mme_pack_esm_information_request_msg(LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT *esm_info_req,
                                                              uint8                                         sec_hdr_type,
                                                              uint32                                        count,
                                                              LIBLTE_BYTE_MSG_STRUCT                        *msg);
LIBLTE_ERROR_ENUM liblte_mme_pack_esm_information_request_msg(LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT *esm_info_req,
                                                              LIBLTE_BYTE_MSG_STRUCT                        *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_information_request_msg(LIBLTE_BYTE_MSG_STRUCT                        *msg,
                                                                LIBLTE_MME_ESM_INFORMATION_REQUEST_MSG_STRUCT *esm_info_req);

/*********************************************************************
    Message Name: ESM Information Response

    Description: Sent by the UE to the network in response to an ESM
                 INFORMATION REQUEST message and provides the
                 requested ESM information.

    Document Reference: 24.301 v10.2.0 Section 8.3.14
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_ACCESS_POINT_NAME_STRUCT       apn;
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    bool                                      apn_present;
    bool                                      protocol_cnfg_opts_present;
}LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_esm_information_response_msg(LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT *esm_info_resp,
                                                               uint8                                           sec_hdr_type,
                                                               uint32                                          count,
                                                               LIBLTE_BYTE_MSG_STRUCT                         *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_information_response_msg(LIBLTE_BYTE_MSG_STRUCT                         *msg,
                                                                 LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT *esm_info_resp);
LIBLTE_ERROR_ENUM srslte_mme_unpack_esm_information_response_msg(LIBLTE_BYTE_MSG_STRUCT                         *msg,
                                                                 LIBLTE_MME_ESM_INFORMATION_RESPONSE_MSG_STRUCT *esm_info_resp);

/*********************************************************************
    Message Name: ESM Status

    Description: Sent by the network or the UE to pass information on
                 the status of the indicated EPS bearer context and
                 report certain error conditions.

    Document Reference: 24.301 v10.2.0 Section 8.3.15
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 eps_bearer_id;
    uint8 proc_transaction_id;
    uint8 esm_cause;
}LIBLTE_MME_ESM_STATUS_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_esm_status_msg(LIBLTE_MME_ESM_STATUS_MSG_STRUCT *esm_status,
                                                 LIBLTE_BYTE_MSG_STRUCT           *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_esm_status_msg(LIBLTE_BYTE_MSG_STRUCT           *msg,
                                                   LIBLTE_MME_ESM_STATUS_MSG_STRUCT *esm_status);

/*********************************************************************
    Message Name: Modify EPS Bearer Context Accept

    Description: Sent by the UE to the network to acknowledge the
                 modification of an active EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.16
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    bool                                      protocol_cnfg_opts_present;
}LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_modify_eps_bearer_context_accept_msg(LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *mod_eps_bearer_context_accept,
                                                                       LIBLTE_BYTE_MSG_STRUCT                                 *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_modify_eps_bearer_context_accept_msg(LIBLTE_BYTE_MSG_STRUCT                                 *msg,
                                                                         LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_ACCEPT_MSG_STRUCT *mod_eps_bearer_context_accept);

/*********************************************************************
    Message Name: Modify EPS Bearer Context Reject

    Description: Sent by the UE or the network to reject a
                 modification of an active EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.17
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    uint8                                     esm_cause;
    bool                                      protocol_cnfg_opts_present;
}LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_modify_eps_bearer_context_reject_msg(LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT *mod_eps_bearer_context_rej,
                                                                       LIBLTE_BYTE_MSG_STRUCT                                 *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_modify_eps_bearer_context_reject_msg(LIBLTE_BYTE_MSG_STRUCT                                 *msg,
                                                                         LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_REJECT_MSG_STRUCT *mod_eps_bearer_context_rej);

/*********************************************************************
    Message Name: Modify EPS Bearer Context Request

    Description: Sent by the network to the UE to request modification
                 of an active EPS bearer context.

    Document Reference: 24.301 v10.2.0 Section 8.3.18
*********************************************************************/
// Defines
#define LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_IEI 0x36
#define LIBLTE_MME_QUALITY_OF_SERVICE_IEI    0x30
// Enums
// Structs
typedef struct{
    LIBLTE_MME_EPS_QUALITY_OF_SERVICE_STRUCT         new_eps_qos;
    LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT          tft;
    LIBLTE_MME_QUALITY_OF_SERVICE_STRUCT             new_qos;
    LIBLTE_MME_APN_AGGREGATE_MAXIMUM_BIT_RATE_STRUCT apn_ambr;
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT        protocol_cnfg_opts;
    uint8                                            eps_bearer_id;
    uint8                                            proc_transaction_id;
    uint8                                            negotiated_llc_sapi;
    uint8                                            radio_prio;
    uint8                                            packet_flow_id;
    bool                                             new_eps_qos_present;
    bool                                             tft_present;
    bool                                             new_qos_present;
    bool                                             negotiated_llc_sapi_present;
    bool                                             radio_prio_present;
    bool                                             packet_flow_id_present;
    bool                                             apn_ambr_present;
    bool                                             protocol_cnfg_opts_present;
}LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_modify_eps_bearer_context_request_msg(LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *mod_eps_bearer_context_req,
                                                                        LIBLTE_BYTE_MSG_STRUCT                                  *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_modify_eps_bearer_context_request_msg(LIBLTE_BYTE_MSG_STRUCT                                  *msg,
                                                                          LIBLTE_MME_MODIFY_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT *mod_eps_bearer_context_req);

/*********************************************************************
    Message Name: Notification

    Description: Sent by the network to inform the UE about events
                 which are relevant for the upper layer using an EPS
                 bearer context or having requested a procedure
                 transaction.

    Document Reference: 24.301 v10.2.0 Section 8.3.18A
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 eps_bearer_id;
    uint8 proc_transaction_id;
    uint8 notification_ind;
}LIBLTE_MME_NOTIFICATION_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_notification_msg(LIBLTE_MME_NOTIFICATION_MSG_STRUCT *notification,
                                                   LIBLTE_BYTE_MSG_STRUCT             *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_notification_msg(LIBLTE_BYTE_MSG_STRUCT             *msg,
                                                     LIBLTE_MME_NOTIFICATION_MSG_STRUCT *notification);

/*********************************************************************
    Message Name: PDN Connectivity Reject

    Description: Sent by the network to the UE to reject establishment
                 of a PDN connection.

    Document Reference: 24.301 v10.2.0 Section 8.3.19
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    LIBLTE_MME_GPRS_TIMER_3_STRUCT            t3496;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    uint8                                     esm_cause;
    bool                                      protocol_cnfg_opts_present;
    bool                                      t3496_present;
}LIBLTE_MME_PDN_CONNECTIVITY_REJECT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_pdn_connectivity_reject_msg(LIBLTE_MME_PDN_CONNECTIVITY_REJECT_MSG_STRUCT *pdn_con_rej,
                                                              LIBLTE_BYTE_MSG_STRUCT                        *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_pdn_connectivity_reject_msg(LIBLTE_BYTE_MSG_STRUCT                        *msg,
                                                                LIBLTE_MME_PDN_CONNECTIVITY_REJECT_MSG_STRUCT *pdn_con_rej);

/*********************************************************************
    Message Name: PDN Connectivity Request

    Description: Sent by the UE to the network to initiate
                 establishment of a PDN connection.

    Document Reference: 24.301 v10.2.0 Section 8.3.20
*********************************************************************/
// Defines
#define LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_IEI                     0xD
#define LIBLTE_MME_ACCESS_POINT_NAME_IEI                          0x28
#define LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_DEVICE_PROPERTIES_IEI 0xC
// Enums
// Structs
typedef struct{
    LIBLTE_MME_ACCESS_POINT_NAME_STRUCT       apn;
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    LIBLTE_MME_ESM_INFO_TRANSFER_FLAG_ENUM    esm_info_transfer_flag;
    LIBLTE_MME_DEVICE_PROPERTIES_ENUM         device_properties;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    uint8                                     pdn_type;
    uint8                                     request_type;
    bool                                      esm_info_transfer_flag_present;
    bool                                      apn_present;
    bool                                      protocol_cnfg_opts_present;
    bool                                      device_properties_present;
}LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_pdn_connectivity_request_msg(LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT *pdn_con_req,
                                                               LIBLTE_BYTE_MSG_STRUCT                         *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_pdn_connectivity_request_msg(LIBLTE_BYTE_MSG_STRUCT                         *msg,
                                                                 LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT *pdn_con_req);

/*********************************************************************
    Message Name: PDN Disconnect Reject

    Description: Sent by the network to the UE to reject release of a
                 PDN connection.

    Document Reference: 24.301 v10.2.0 Section 8.3.21
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    uint8                                     esm_cause;
    bool                                      protocol_cnfg_opts_present;
}LIBLTE_MME_PDN_DISCONNECT_REJECT_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_pdn_disconnect_reject_msg(LIBLTE_MME_PDN_DISCONNECT_REJECT_MSG_STRUCT *pdn_discon_rej,
                                                            LIBLTE_BYTE_MSG_STRUCT                      *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_pdn_disconnect_reject_msg(LIBLTE_BYTE_MSG_STRUCT                      *msg,
                                                              LIBLTE_MME_PDN_DISCONNECT_REJECT_MSG_STRUCT *pdn_discon_rej);

/*********************************************************************
    Message Name: PDN Disconnect Request

    Description: Sent by the UE to the network to initiate release of
                 a PDN connection.

    Document Reference: 24.301 v10.2.0 Section 8.3.22
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_MME_PROTOCOL_CONFIG_OPTIONS_STRUCT protocol_cnfg_opts;
    uint8                                     eps_bearer_id;
    uint8                                     proc_transaction_id;
    uint8                                     linked_eps_bearer_id;
    bool                                      protocol_cnfg_opts_present;
}LIBLTE_MME_PDN_DISCONNECT_REQUEST_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_mme_pack_pdn_disconnect_request_msg(LIBLTE_MME_PDN_DISCONNECT_REQUEST_MSG_STRUCT *pdn_discon_req,
                                                             LIBLTE_BYTE_MSG_STRUCT                       *msg);
LIBLTE_ERROR_ENUM liblte_mme_unpack_pdn_disconnect_request_msg(LIBLTE_BYTE_MSG_STRUCT                       *msg,
                                                               LIBLTE_MME_PDN_DISCONNECT_REQUEST_MSG_STRUCT *pdn_discon_req);

#endif // SRSLTE_LIBLTE_MME_H
