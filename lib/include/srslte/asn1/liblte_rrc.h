/*******************************************************************************

    Copyright 2012-2014 Ben Wojtowicz
    Copyright 2014 Andrew Murphy (SIB13 unpack)

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

    File: liblte_rrc.h

    Description: Contains all the definitions for the LTE Radio Resource
                 Control Layer library.

    Revision History
    ----------    -------------    --------------------------------------------
    03/24/2012    Ben Wojtowicz    Created file.
    04/21/2012    Ben Wojtowicz    Added SIB1 parameters, IEs, and messages
    05/28/2012    Ben Wojtowicz    Added SIB1 pack functionality
    08/19/2012    Ben Wojtowicz    Added functionality to support SIB2, SIB3,
                                   SIB4, and SIB8 packing and unpacking
    10/06/2012    Ben Wojtowicz    Added more decoding/encoding.
    12/26/2012    Ben Wojtowicz    Added text versions of some enums.
    03/03/2013    Ben Wojtowicz    Added a test fill pattern, text and number
                                   mappings for all enums, carrier_freqs_geran,
                                   SIB5, SIB6, SIB7, and paging packing and
                                   unpacking.
    07/21/2013    Ben Wojtowicz    Using the common msg structure.
    03/26/2014    Ben Wojtowicz    Added support for RRC Connection Request,
                                   RRC Connection Reestablishment Request,
                                   and UL CCCH Messages.
    05/04/2014    Ben Wojtowicz    Added support for DL CCCH Messages.
    06/15/2014    Ben Wojtowicz    Added support for UL DCCH Messages.
    08/03/2014    Ben Wojtowicz    Added more decoding/encoding.
    09/19/2014    Andrew Murphy    Added SIB13 unpack.
    11/01/2014    Ben Wojtowicz    Added more decoding/encoding.
    11/09/2014    Ben Wojtowicz    Added SIB13 pack.
    11/29/2014    Ben Wojtowicz    Fixed a bug in RRC connection reestablishment
                                   UE identity.

*******************************************************************************/

#ifndef SRSLTE_LIBLTE_RRC_H
#define SRSLTE_LIBLTE_RRC_H

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include "liblte_common.h"

/*******************************************************************************
                              DEFINES
*******************************************************************************/


/*******************************************************************************
                              TYPEDEFS
*******************************************************************************/

static const uint8 liblte_rrc_test_fill[8] = {1,0,1,0,0,1,0,1};

typedef void (*log_handler_t)(void *ctx, char *str);

void liblte_rrc_log_register_handler(void *ctx, log_handler_t handler); 

/*******************************************************************************
                              INFORMATION ELEMENT DECLARATIONS
*******************************************************************************/

/*********************************************************************
    IE Name: MBSFN Notification Config

    Description: Specifies the MBMS notification related configuration
                 parameters, that are applicable for all MBSFN areas

    Document Reference: 36.331 v10.0.0 Section 6.3.7
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_NOTIFICATION_REPETITION_COEFF_R9_N2 = 0,
    LIBLTE_RRC_NOTIFICATION_REPETITION_COEFF_R9_N4,
    LIBLTE_RRC_NOTIFICATION_REPETITION_COEFF_R9_N_ITEMS,
}LIBLTE_RRC_NOTIFICATION_REPETITION_COEFF_R9_ENUM;
static const char liblte_rrc_notification_repetition_coeff_r9_text[LIBLTE_RRC_NOTIFICATION_REPETITION_COEFF_R9_N_ITEMS][20] = {"2", "4"};
static const uint8 liblte_rrc_notification_repetition_coeff_r9_num[LIBLTE_RRC_NOTIFICATION_REPETITION_COEFF_R9_N_ITEMS] = {2, 4};
// Structs
typedef struct{
    LIBLTE_RRC_NOTIFICATION_REPETITION_COEFF_R9_ENUM repetition_coeff;
    uint8                                            offset;
    uint8                                            sf_index;
}LIBLTE_RRC_MBSFN_NOTIFICATION_CONFIG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_mbsfn_notification_config_ie(LIBLTE_RRC_MBSFN_NOTIFICATION_CONFIG_STRUCT  *mbsfn_notification_cnfg,
                                                               uint8                                       **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mbsfn_notification_config_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_MBSFN_NOTIFICATION_CONFIG_STRUCT  *mbsfn_notification_cnfg);

/*********************************************************************
    IE Name: MBSFN Area Info List

    Description: Contains the information required to acquire the MBMS
                 control information associated with one or more MBSFN
                 areas

    Document Reference: 36.331 v10.0.0 Section 6.3.7
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_MBSFN_AREAS 8
// Enums
typedef enum{
    LIBLTE_RRC_NON_MBSFN_REGION_LENGTH_S1 = 0,
    LIBLTE_RRC_NON_MBSFN_REGION_LENGTH_S2,
    LIBLTE_RRC_NON_MBSFN_REGION_LENGTH_N_ITEMS,
}LIBLTE_RRC_NON_MBSFN_REGION_LENGTH_ENUM;
static const char liblte_rrc_non_mbsfn_region_length_text[LIBLTE_RRC_NON_MBSFN_REGION_LENGTH_N_ITEMS][20] = {"1", "2"};
static const uint8 liblte_rrc_non_mbsfn_region_length_num[LIBLTE_RRC_NON_MBSFN_REGION_LENGTH_N_ITEMS] = {1, 2};
typedef enum{
    LIBLTE_RRC_MCCH_REPETITION_PERIOD_RF32 = 0,
    LIBLTE_RRC_MCCH_REPETITION_PERIOD_RF64,
    LIBLTE_RRC_MCCH_REPETITION_PERIOD_RF128,
    LIBLTE_RRC_MCCH_REPETITION_PERIOD_RF256,
    LIBLTE_RRC_MCCH_REPETITION_PERIOD_N_ITEMS,
}LIBLTE_RRC_MCCH_REPETITION_PERIOD_ENUM;
static const char liblte_rrc_mcch_repetition_period_r9_text[LIBLTE_RRC_MCCH_REPETITION_PERIOD_N_ITEMS][20] = {"32", "64", "128", "256"};
static const uint16 liblte_rrc_mcch_repetition_period_r9_num[LIBLTE_RRC_MCCH_REPETITION_PERIOD_N_ITEMS] = {32, 64, 128, 256};
typedef enum{
    LIBLTE_RRC_MCCH_MODIFICATION_PERIOD_512 = 0,
    LIBLTE_RRC_MCCH_MODIFICATION_PERIOD_1024,
    LIBLTE_RRC_MCCH_MODIFICATION_PERIOD_N_ITEMS,
}LIBLTE_RRC_MCCH_MODIFICATION_PERIOD_ENUM;
static const char liblte_rrc_mcch_modification_period_r9_text[LIBLTE_RRC_MCCH_MODIFICATION_PERIOD_N_ITEMS][20] = {"512", "1024"};
static const uint16 liblte_rrc_mcch_modification_period_r9_num[LIBLTE_RRC_MCCH_MODIFICATION_PERIOD_N_ITEMS] = {512, 1024};
typedef enum{
    LIBLTE_RRC_MCCH_SIGNALLING_MCS_N2 = 0,
    LIBLTE_RRC_MCCH_SIGNALLING_MCS_N7,
    LIBLTE_RRC_MCCH_SIGNALLING_MCS_N13,
    LIBLTE_RRC_MCCH_SIGNALLING_MCS_N19,
    LIBLTE_RRC_MCCH_SIGNALLING_MCS_N_ITEMS,
}LIBLTE_RRC_MCCH_SIGNALLING_MCS_ENUM;
static const char liblte_rrc_mcch_signalling_mcs_r9_text[LIBLTE_RRC_MCCH_SIGNALLING_MCS_N_ITEMS][20] = {"2", "7", "13", "19"};
static const uint8 liblte_rrc_mcch_signalling_mcs_r9_num[LIBLTE_RRC_MCCH_SIGNALLING_MCS_N_ITEMS] = {2, 7, 13, 19};
// Structs
typedef struct{
    LIBLTE_RRC_NON_MBSFN_REGION_LENGTH_ENUM  non_mbsfn_region_length;
    LIBLTE_RRC_MCCH_REPETITION_PERIOD_ENUM   mcch_repetition_period_r9;
    LIBLTE_RRC_MCCH_MODIFICATION_PERIOD_ENUM mcch_modification_period_r9;
    LIBLTE_RRC_MCCH_SIGNALLING_MCS_ENUM      signalling_mcs_r9;
    uint8                                    mbsfn_area_id_r9;
    uint8                                    notification_indicator_r9;
    uint8                                    mcch_offset_r9;
    uint8                                    sf_alloc_info_r9;
}LIBLTE_RRC_MBSFN_AREA_INFO_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_mbsfn_area_info_ie(LIBLTE_RRC_MBSFN_AREA_INFO_STRUCT  *mbsfn_area_info,
                                                     uint8                             **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mbsfn_area_info_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_MBSFN_AREA_INFO_STRUCT  *mbsfn_area_info);

/*********************************************************************
    IE Name: MBSFN Subframe Config

    Description: Defines subframes that are reserved for MBSFN in
                 downlink

    Document Reference: 36.331 v10.0.0 Section 6.3.7
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_MBSFN_ALLOCATIONS 8
// Enums
typedef enum{
    LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N1 = 0,
    LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N2,
    LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N4,
    LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N8,
    LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N16,
    LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N32,
    LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N_ITEMS,
}LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_ENUM;
static const char liblte_rrc_radio_frame_allocation_period_text[LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N_ITEMS][20] = { "1",  "2",  "4",  "8",
                                                                                                                         "16", "32"};
static const uint8 liblte_rrc_radio_frame_allocation_period_num[LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N_ITEMS] = {1, 2, 4, 8, 16, 32};
typedef enum{
    LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_ONE = 0,
    LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_FOUR,
    LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_N_ITEMS,
}LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_ENUM;
static const char liblte_rrc_subframe_allocation_num_frames_text[LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_N_ITEMS][20] = {"1", "4"};
static const uint8 liblte_rrc_subframe_allocation_num_frames_num[LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_N_ITEMS] = {1, 4};
// Structs
typedef struct{
    LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_ENUM  radio_fr_alloc_period;
    LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_ENUM subfr_alloc_num_frames;
    uint32                                         subfr_alloc;
    uint8                                          radio_fr_alloc_offset;
}LIBLTE_RRC_MBSFN_SUBFRAME_CONFIG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_mbsfn_subframe_config_ie(LIBLTE_RRC_MBSFN_SUBFRAME_CONFIG_STRUCT  *mbsfn_subfr_cnfg,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mbsfn_subframe_config_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_MBSFN_SUBFRAME_CONFIG_STRUCT  *mbsfn_subfr_cnfg);

/*********************************************************************
    IE Name: PMCH Info List

    Description: Specifies configuration of all PMCHs of an MBSFN area

    Document Reference: 36.331 v10.0.0 Section 6.3.7
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: C-RNTI

    Description: Identifies a UE having a RRC connection within a cell

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_c_rnti_ie(uint16   rnti,
                                            uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_c_rnti_ie(uint8  **ie_ptr,
                                              uint16  *rnti);

/*********************************************************************
    IE Name: Dedicated Info CDMA2000

    Description: Transfers UE specific CDMA2000 information between
                 the network and the UE

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_dedicated_info_cdma2000_ie(LIBLTE_BYTE_MSG_STRUCT  *ded_info_cdma2000,
                                                             uint8                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_dedicated_info_cdma2000_ie(uint8                  **ie_ptr,
                                                               LIBLTE_BYTE_MSG_STRUCT  *ded_info_cdma2000);

/*********************************************************************
    IE Name: Dedicated Info NAS

    Description: Transfers UE specific NAS layer information between
                 the network and the UE

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_dedicated_info_nas_ie(LIBLTE_BYTE_MSG_STRUCT  *ded_info_nas,
                                                        uint8                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_dedicated_info_nas_ie(uint8                  **ie_ptr,
                                                          LIBLTE_BYTE_MSG_STRUCT  *ded_info_nas);

/*********************************************************************
    IE Name: Filter Coefficient

    Description: Specifies the measurement filtering coefficient

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_FILTER_COEFFICIENT_FC0 = 0,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC1,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC2,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC3,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC4,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC5,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC6,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC7,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC8,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC9,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC11,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC13,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC15,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC17,
    LIBLTE_RRC_FILTER_COEFFICIENT_FC19,
    LIBLTE_RRC_FILTER_COEFFICIENT_SPARE1,
    LIBLTE_RRC_FILTER_COEFFICIENT_N_ITEMS,
}LIBLTE_RRC_FILTER_COEFFICIENT_ENUM;
static const char liblte_rrc_filter_coefficient_text[LIBLTE_RRC_FILTER_COEFFICIENT_N_ITEMS][20] = {    "0",     "1",     "2",     "3",
                                                                                                       "4",     "5",     "6",     "7",
                                                                                                       "8",     "9",    "11",    "13",
                                                                                                      "15",    "17",    "19", "SPARE"};
static const int8 liblte_rrc_filter_coefficient_num[LIBLTE_RRC_FILTER_COEFFICIENT_N_ITEMS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 13, 15, 17, 19, -1};
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_filter_coefficient_ie(LIBLTE_RRC_FILTER_COEFFICIENT_ENUM   filter_coeff,
                                                        uint8                              **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_filter_coefficient_ie(uint8                              **ie_ptr,
                                                          LIBLTE_RRC_FILTER_COEFFICIENT_ENUM  *filter_coeff);

/*********************************************************************
    IE Name: MMEC

    Description: Identifies an MME within the scope of an MME group
                 within a PLMN

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_mmec_ie(uint8   mmec,
                                          uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mmec_ie(uint8 **ie_ptr,
                                            uint8  *mmec);

/*********************************************************************
    IE Name: Neigh Cell Config

    Description: Provides the information related to MBSFN and TDD
                 UL/DL configuration of neighbor cells

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_neigh_cell_config_ie(uint8   neigh_cell_config,
                                                       uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_neigh_cell_config_ie(uint8 **ie_ptr,
                                                         uint8  *neigh_cell_config);

/*********************************************************************
    IE Name: Other Config

    Description: Contains configuration related to other configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_REPORT_PROXIMITY_INDICATION_EUTRA_R9_ENABLED = 0,
    LIBLTE_RRC_REPORT_PROXIMITY_INDICATION_EUTRA_R9_N_ITEMS,
}LIBLTE_RRC_REPORT_PROXIMITY_INDICATION_EUTRA_R9_ENUM;
static const char liblte_rrc_report_proximity_indication_eutra_r9_text[LIBLTE_RRC_REPORT_PROXIMITY_INDICATION_EUTRA_R9_N_ITEMS][20] = {"Enabled"};
typedef enum{
    LIBLTE_RRC_REPORT_PROXIMITY_INDICATION_UTRA_R9_ENABLED = 0,
    LIBLTE_RRC_REPORT_PROXIMITY_INDICATION_UTRA_R9_N_ITEMS,
}LIBLTE_RRC_REPORT_PROXIMITY_INDICATION_UTRA_R9_ENUM;
static const char liblte_rrc_report_proximity_indication_utra_r9_text[LIBLTE_RRC_REPORT_PROXIMITY_INDICATION_UTRA_R9_N_ITEMS][20] = {"Enabled"};
// Structs
typedef struct{
    LIBLTE_RRC_REPORT_PROXIMITY_INDICATION_EUTRA_R9_ENUM report_proximity_ind_eutra;
    LIBLTE_RRC_REPORT_PROXIMITY_INDICATION_UTRA_R9_ENUM  report_proximity_ind_utra;
    bool                                                 report_proximity_ind_eutra_present;
    bool                                                 report_proximity_ind_utra_present;
}LIBLTE_RRC_REPORT_PROXIMITY_CONFIG_R9_STRUCT;
typedef struct{
    LIBLTE_RRC_REPORT_PROXIMITY_CONFIG_R9_STRUCT report_proximity_cnfg;
    bool                                         report_proximity_cnfg_present;
}LIBLTE_RRC_OTHER_CONFIG_R9_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_other_config_ie(LIBLTE_RRC_OTHER_CONFIG_R9_STRUCT  *other_cnfg,
                                                  uint8                             **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_other_config_ie(uint8                             **ie_ptr,
                                                    LIBLTE_RRC_OTHER_CONFIG_R9_STRUCT  *other_cnfg);

/*********************************************************************
    IE Name: RAND CDMA2000 (1xRTT)

    Description: Contains a random value, generated by the eNB, to be
                 passed to the CDMA2000 upper layers

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rand_cdma2000_1xrtt_ie(uint32   rand,
                                                         uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rand_cdma2000_1xrtt_ie(uint8  **ie_ptr,
                                                           uint32  *rand);

/*********************************************************************
    IE Name: RAT Type

    Description: Indicates the radio access technology (RAT),
                 including E-UTRA, of the requested/transferred UE
                 capabilities

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_RAT_TYPE_EUTRA = 0,
    LIBLTE_RRC_RAT_TYPE_UTRA,
    LIBLTE_RRC_RAT_TYPE_GERAN_CS,
    LIBLTE_RRC_RAT_TYPE_GERAN_PS,
    LIBLTE_RRC_RAT_TYPE_CDMA2000_1XRTT,
    LIBLTE_RRC_RAT_TYPE_SPARE_3,
    LIBLTE_RRC_RAT_TYPE_SPARE_2,
    LIBLTE_RRC_RAT_TYPE_SPARE_1,
    LIBLTE_RRC_RAT_TYPE_N_ITEMS,
}LIBLTE_RRC_RAT_TYPE_ENUM;
static const char liblte_rrc_rat_type_text[LIBLTE_RRC_RAT_TYPE_N_ITEMS][20] = {         "EUTRA",            "UTRA",      "GERAN_CS",      "GERAN_PS",
                                                                               "CDMA2000_1XRTT",           "SPARE",         "SPARE",         "SPARE"};
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rat_type_ie(LIBLTE_RRC_RAT_TYPE_ENUM   rat_type,
                                              uint8                    **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rat_type_ie(uint8                    **ie_ptr,
                                                LIBLTE_RRC_RAT_TYPE_ENUM  *rat_type);

/*********************************************************************
    IE Name: RRC Transaction Identifier

    Description: Identifies an RRC procedure along with the message
                 type

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_transaction_identifier_ie(uint8   rrc_transaction_id,
                                                                uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_transaction_identifier_ie(uint8 **ie_ptr,
                                                                  uint8  *rrc_transaction_id);

/*********************************************************************
    IE Name: S-TMSI

    Description: Contains an S-Temporary Mobile Subscriber Identity,
                 a temporary UE identity provided by the EPC which
                 uniquely identifies the UE within the tracking area

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint32 m_tmsi;
    uint8  mmec;
}LIBLTE_RRC_S_TMSI_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_s_tmsi_ie(LIBLTE_RRC_S_TMSI_STRUCT  *s_tmsi,
                                            uint8                    **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_s_tmsi_ie(uint8                    **ie_ptr,
                                              LIBLTE_RRC_S_TMSI_STRUCT  *s_tmsi);

/*********************************************************************
    IE Name: UE Capability RAT Container List

    Description: Contains list of containers, one for each RAT for
                 which UE capabilities are transferred

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: UE EUTRA Capability

    Description: Conveys the E-UTRA UE Radio Access Capability
                 Parameters

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAXBANDS 64

// Enums
typedef enum{
    LIBLTE_RRC_ACCESS_STRATUM_RELEASE_REL8 = 0,
    LIBLTE_RRC_ACCESS_STRATUM_RELEASE_REL9,
    LIBLTE_RRC_ACCESS_STRATUM_RELEASE_SPARE6,
    LIBLTE_RRC_ACCESS_STRATUM_RELEASE_SPARE5,
    LIBLTE_RRC_ACCESS_STRATUM_RELEASE_SPARE4,
    LIBLTE_RRC_ACCESS_STRATUM_RELEASE_SPARE3,
    LIBLTE_RRC_ACCESS_STRATUM_RELEASE_SPARE2,
    LIBLTE_RRC_ACCESS_STRATUM_RELEASE_SPARE1,
    LIBLTE_RRC_ACCESS_STRATUM_RELEASE_N_ITEMS
}LIBLTE_RRC_ACCESS_STRATUM_RELEASE_ENUM;
static const char liblte_rrc_access_stratum_release_text[LIBLTE_RRC_ACCESS_STRATUM_RELEASE_N_ITEMS][20] = { "rel8",  "rel9",  "spare6",  "spare5",
                                                                        "spare4", "spare3", "spare2", "spare1"};

typedef enum{
    LIBLTE_RRC_ROHC_PROFILES_0x0001,
    LIBLTE_RRC_ROHC_PROFILES_0x0002,
    LIBLTE_RRC_ROHC_PROFILES_0x0003,
    LIBLTE_RRC_ROHC_PROFILES_0x0004,
    LIBLTE_RRC_ROHC_PROFILES_0x0006,
    LIBLTE_RRC_ROHC_PROFILES_0x0101,
    LIBLTE_RRC_ROHC_PROFILES_0x0102,
    LIBLTE_RRC_ROHC_PROFILES_0x0103,
    LIBLTE_RRC_ROHC_PROFILES_0x0104,
    LIBLTE_RRC_ROHC_PROFILES_NITEMS
}LIBLTE_RRC_ROHC_PROFILES_ENUM;
static const char liblte_rrc_rohc_profiles_text[LIBLTE_RRC_ROHC_PROFILES_NITEMS][20] = {"profile0x0001",
                                                                                        "profile0x0002",
                                                                                        "profile0x0003",
                                                                                        "profile0x0004",
                                                                                        "profile0x0006",
                                                                                        "profile0x0101",
                                                                                        "profile0x0102",
                                                                                        "profile0x0103",
                                                                                        "profile0x0104"};

typedef enum{
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS2 = 0,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS4,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS8,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS12,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS16,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS24,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS32,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS48,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS64,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS128,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS256,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS512,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS1024,
    LIBLTE_RRC_MAX_ROHC_CTXTS_CS16384,
    LIBLTE_RRC_MAX_ROHC_CTXTS_SPARE1,
    LIBLTE_RRC_MAX_ROHC_CTXTS_SPARE2,
    LIBLTE_RRC_MAX_ROHC_CTXTS_N_ITEMS
}LIBLTE_RRC_MAX_ROHC_CTXTS_ENUM;
static const char liblte_rrc_max_rohc_ctxts_text[LIBLTE_RRC_MAX_ROHC_CTXTS_N_ITEMS][20] = { "cs2", "cs4", "cs8", "cs12", "cs16", "cs24", "cs32",
                                                                                            "cs48", "cs64", "cs128", "cs256", "cs512", "cs1024",
                                                                                            "cs16384", "spare2", "spare1"};

typedef enum{
    LIBLTE_RRC_BAND_1 = 0,
    LIBLTE_RRC_BAND_2,
    LIBLTE_RRC_BAND_3,
    LIBLTE_RRC_BAND_4,
    LIBLTE_RRC_BAND_5,
    LIBLTE_RRC_BAND_6,
    LIBLTE_RRC_BAND_7,
    LIBLTE_RRC_BAND_8,
    LIBLTE_RRC_BAND_9,
    LIBLTE_RRC_BAND_10,
    LIBLTE_RRC_BAND_11,
    LIBLTE_RRC_BAND_12,
    LIBLTE_RRC_BAND_13,
    LIBLTE_RRC_BAND_14,
    LIBLTE_RRC_BAND_17,
    LIBLTE_RRC_BAND_18,
    LIBLTE_RRC_BAND_19,
    LIBLTE_RRC_BAND_20,
    LIBLTE_RRC_BAND_21,
    LIBLTE_RRC_BAND_22,
    LIBLTE_RRC_BAND_23,
    LIBLTE_RRC_BAND_24,
    LIBLTE_RRC_BAND_25,
    LIBLTE_RRC_BAND_33,
    LIBLTE_RRC_BAND_34,
    LIBLTE_RRC_BAND_35,
    LIBLTE_RRC_BAND_36,
    LIBLTE_RRC_BAND_37,
    LIBLTE_RRC_BAND_38,
    LIBLTE_RRC_BAND_39,
    LIBLTE_RRC_BAND_40,
    LIBLTE_RRC_BAND_41,
    LIBLTE_RRC_BAND_42,
    LIBLTE_RRC_BAND_43,
    LIBLTE_RRC_BAND_N_ITEMS,
}LIBLTE_RRC_BAND_ENUM;
static const char liblte_rrc_band_text[LIBLTE_RRC_BAND_N_ITEMS][20] = { "1",  "2",  "3",  "4",
                                                                        "5",  "6",  "7",  "8",
                                                                        "9", "10", "11", "12",
                                                                       "13", "14", "17", "18",
                                                                       "19", "20", "21", "22",
                                                                       "23", "24", "25", "33",
                                                                       "34", "35", "36", "37",
                                                                       "38", "39", "40", "41",
                                                                       "42", "43"};
static const uint8 liblte_rrc_band_num[LIBLTE_RRC_BAND_N_ITEMS] = { 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 17, 18, 19,
                                                                   20, 21, 22, 23, 24, 25, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43};

// Structs
typedef struct{
    bool                            supported_rohc_profiles[9];
    LIBLTE_RRC_MAX_ROHC_CTXTS_ENUM  max_rohc_ctxts;
    bool                            max_rohc_ctxts_present;
}LIBLTE_RRC_PDCP_PARAMS_STRUCT;

LIBLTE_ERROR_ENUM liblte_rrc_pack_pdcp_params_ie(LIBLTE_RRC_PDCP_PARAMS_STRUCT  *pdcp_params,
                                                 uint8                     **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pdcp_params_ie(uint8                            **ie_ptr,
                                                   LIBLTE_RRC_PDCP_PARAMS_STRUCT  *pdcp_params);

typedef struct{
    bool tx_antenna_selection_supported;
    bool specific_ref_sigs_supported;
}LIBLTE_RRC_PHY_LAYER_PARAMS_STRUCT;

LIBLTE_ERROR_ENUM liblte_rrc_pack_phy_layer_params_ie(LIBLTE_RRC_PHY_LAYER_PARAMS_STRUCT  *params,
                                                      uint8                              **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phy_layer_params_ie(uint8                              **ie_ptr,
                                                        LIBLTE_RRC_PHY_LAYER_PARAMS_STRUCT  *params);

typedef struct{
    uint8 band_eutra;
    bool  half_duplex;
}LIBLTE_RRC_SUPPORTED_BAND_EUTRA_STRUCT;

typedef struct{
    LIBLTE_RRC_SUPPORTED_BAND_EUTRA_STRUCT supported_band_eutra[LIBLTE_RRC_MAXBANDS];
    uint32                                 N_supported_band_eutras;
}LIBLTE_RRC_RF_PARAMS_STRUCT;

LIBLTE_ERROR_ENUM liblte_rrc_pack_rf_params_ie(LIBLTE_RRC_RF_PARAMS_STRUCT  *params,
                                               uint8                       **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rf_params_ie(uint8                       **ie_ptr,
                                                 LIBLTE_RRC_RF_PARAMS_STRUCT  *params);

typedef struct{
    bool   inter_freq_need_for_gaps[LIBLTE_RRC_MAXBANDS];
    uint32 N_inter_freq_need_for_gaps;
}LIBLTE_RRC_BAND_INFO_EUTRA_STRUCT;

LIBLTE_ERROR_ENUM liblte_rrc_pack_band_info_eutra_ie(LIBLTE_RRC_BAND_INFO_EUTRA_STRUCT  *info,
                                                     uint8                             **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_band_info_eutra_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_BAND_INFO_EUTRA_STRUCT  *info);

typedef struct{
    LIBLTE_RRC_BAND_INFO_EUTRA_STRUCT   band_list_eutra[LIBLTE_RRC_MAXBANDS];
    uint32                              N_band_list_eutra;
}LIBLTE_RRC_MEAS_PARAMS_STRUCT;

LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_params_ie(LIBLTE_RRC_MEAS_PARAMS_STRUCT  *params,
                                                 uint8                         **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_params_ie(uint8                         **ie_ptr,
                                                   LIBLTE_RRC_MEAS_PARAMS_STRUCT  *params);

typedef struct{
    // WARNING: hardcoding these options to not present
    bool    utra_fdd_present;
    bool    utra_tdd128_present;
    bool    utra_tdd384_present;
    bool    utra_tdd768_present;
    bool    geran_present;
    bool    cdma2000_hrpd_present;
    bool    cdma2000_1xrtt_present;
}LIBLTE_RRC_INTER_RAT_PARAMS_STRUCT;

LIBLTE_ERROR_ENUM liblte_rrc_pack_inter_rat_params_ie(LIBLTE_RRC_INTER_RAT_PARAMS_STRUCT  *params,
                                                      uint8                              **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_inter_rat_params_ie(uint8                              **ie_ptr,
                                                        LIBLTE_RRC_INTER_RAT_PARAMS_STRUCT  *params);

typedef struct{
    uint8                               access_stratum_release;
    uint8                               ue_category;
    LIBLTE_RRC_PDCP_PARAMS_STRUCT       pdcp_params;
    LIBLTE_RRC_PHY_LAYER_PARAMS_STRUCT  phy_params;
    LIBLTE_RRC_RF_PARAMS_STRUCT         rf_params;
    LIBLTE_RRC_MEAS_PARAMS_STRUCT       meas_params;
    uint32                              feature_group_indicator;
    LIBLTE_RRC_INTER_RAT_PARAMS_STRUCT  inter_rat_params;
    bool                                feature_group_indicator_present;
}LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_ue_eutra_capability_ie(LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT  *ue_eutra_capability,
                                                         LIBLTE_BIT_MSG_STRUCT                  *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ue_eutra_capability_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT  *ue_eutra_capability);


/*********************************************************************
    IE Name: UE Timers and Constants

    Description: Contains timers and constants used by the UE in
                 either RRC_CONNECTED or RRC_IDLE

    Document Reference: 36.331 v10.0.0 Section 6.3.6
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_T300_MS100 = 0,
    LIBLTE_RRC_T300_MS200,
    LIBLTE_RRC_T300_MS300,
    LIBLTE_RRC_T300_MS400,
    LIBLTE_RRC_T300_MS600,
    LIBLTE_RRC_T300_MS1000,
    LIBLTE_RRC_T300_MS1500,
    LIBLTE_RRC_T300_MS2000,
    LIBLTE_RRC_T300_N_ITEMS,
}LIBLTE_RRC_T300_ENUM;
static const char liblte_rrc_t300_text[LIBLTE_RRC_T300_N_ITEMS][20] = { "100",  "200",  "300",  "400",
                                                                        "600", "1000", "1500", "2000"};
static const uint16 liblte_rrc_t300_num[LIBLTE_RRC_T300_N_ITEMS] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
typedef enum{
    LIBLTE_RRC_T301_MS100 = 0,
    LIBLTE_RRC_T301_MS200,
    LIBLTE_RRC_T301_MS300,
    LIBLTE_RRC_T301_MS400,
    LIBLTE_RRC_T301_MS600,
    LIBLTE_RRC_T301_MS1000,
    LIBLTE_RRC_T301_MS1500,
    LIBLTE_RRC_T301_MS2000,
    LIBLTE_RRC_T301_N_ITEMS,
}LIBLTE_RRC_T301_ENUM;
static const char liblte_rrc_t301_text[LIBLTE_RRC_T301_N_ITEMS][20] = { "100",  "200",  "300",  "400",
                                                                        "600", "1000", "1500", "2000"};
static const uint16 liblte_rrc_t301_num[LIBLTE_RRC_T301_N_ITEMS] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
typedef enum{
    LIBLTE_RRC_T310_MS0 = 0,
    LIBLTE_RRC_T310_MS50,
    LIBLTE_RRC_T310_MS100,
    LIBLTE_RRC_T310_MS200,
    LIBLTE_RRC_T310_MS500,
    LIBLTE_RRC_T310_MS1000,
    LIBLTE_RRC_T310_MS2000,
    LIBLTE_RRC_T310_N_ITEMS,
}LIBLTE_RRC_T310_ENUM;
static const char liblte_rrc_t310_text[LIBLTE_RRC_T310_N_ITEMS][20] = {   "0",   "50",  "100",  "200",
                                                                        "500", "1000", "2000"};
static const uint16 liblte_rrc_t310_num[LIBLTE_RRC_T310_N_ITEMS] = {0, 50, 100, 200, 500, 1000, 2000};
typedef enum{
    LIBLTE_RRC_N310_N1 = 0,
    LIBLTE_RRC_N310_N2,
    LIBLTE_RRC_N310_N3,
    LIBLTE_RRC_N310_N4,
    LIBLTE_RRC_N310_N6,
    LIBLTE_RRC_N310_N8,
    LIBLTE_RRC_N310_N10,
    LIBLTE_RRC_N310_N20,
    LIBLTE_RRC_N310_N_ITEMS,
}LIBLTE_RRC_N310_ENUM;
static const char liblte_rrc_n310_text[LIBLTE_RRC_N310_N_ITEMS][20] = { "1",  "2",  "3",  "4",
                                                                        "6",  "8", "10", "20"};
static const uint8 liblte_rrc_n310_num[LIBLTE_RRC_N310_N_ITEMS] = {1, 2, 3, 4, 6, 8, 10, 20};
typedef enum{
    LIBLTE_RRC_T311_MS1000 = 0,
    LIBLTE_RRC_T311_MS3000,
    LIBLTE_RRC_T311_MS5000,
    LIBLTE_RRC_T311_MS10000,
    LIBLTE_RRC_T311_MS15000,
    LIBLTE_RRC_T311_MS20000,
    LIBLTE_RRC_T311_MS30000,
    LIBLTE_RRC_T311_N_ITEMS,
}LIBLTE_RRC_T311_ENUM;
static const char liblte_rrc_t311_text[LIBLTE_RRC_T311_N_ITEMS][20] = { "1000",  "3000",  "5000", "10000",
                                                                       "15000", "20000", "30000"};
static const uint16 liblte_rrc_t311_num[LIBLTE_RRC_T311_N_ITEMS] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
typedef enum{
    LIBLTE_RRC_N311_N1 = 0,
    LIBLTE_RRC_N311_N2,
    LIBLTE_RRC_N311_N3,
    LIBLTE_RRC_N311_N4,
    LIBLTE_RRC_N311_N5,
    LIBLTE_RRC_N311_N6,
    LIBLTE_RRC_N311_N8,
    LIBLTE_RRC_N311_N10,
    LIBLTE_RRC_N311_N_ITEMS,
}LIBLTE_RRC_N311_ENUM;
static const char liblte_rrc_n311_text[LIBLTE_RRC_N311_N_ITEMS][20] = { "1",  "2",  "3",  "4",
                                                                        "5",  "6",  "8", "10"};
static const uint8 liblte_rrc_n311_num[LIBLTE_RRC_N311_N_ITEMS] = {1, 2, 3, 4, 5, 6, 8, 10};
// Structs
typedef struct{
    LIBLTE_RRC_T300_ENUM t300;
    LIBLTE_RRC_T301_ENUM t301;
    LIBLTE_RRC_T310_ENUM t310;
    LIBLTE_RRC_N310_ENUM n310;
    LIBLTE_RRC_T311_ENUM t311;
    LIBLTE_RRC_N311_ENUM n311;
}LIBLTE_RRC_UE_TIMERS_AND_CONSTANTS_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_ue_timers_and_constants_ie(LIBLTE_RRC_UE_TIMERS_AND_CONSTANTS_STRUCT  *ue_timers_and_constants,
                                                             uint8                                     **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ue_timers_and_constants_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_RRC_UE_TIMERS_AND_CONSTANTS_STRUCT  *ue_timers_and_constants);

/*********************************************************************
    IE Name: Allowed Meas Bandwidth

    Description: Indicates the maximum allowed measurement bandwidth
                 on a carrier frequency as defined by the parameter
                 Transmission Bandwidth Configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_MBW6 = 0,
    LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_MBW15,
    LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_MBW25,
    LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_MBW50,
    LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_MBW75,
    LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_MBW100,
    LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_N_ITEMS,
}LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_ENUM;
static const char liblte_rrc_allowed_meas_bandwidth_text[LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_N_ITEMS][20] = {"1.4",   "3",   "5",  "10",
                                                                                                            "15",  "20"};
static const double liblte_rrc_allowed_meas_bandwidth_num[LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_N_ITEMS] = {1.4, 3, 5, 10, 15, 20};
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_allowed_meas_bandwidth_ie(LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_ENUM   allowed_meas_bw,
                                                            uint8                                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_allowed_meas_bandwidth_ie(uint8                                  **ie_ptr,
                                                              LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_ENUM  *allowed_meas_bw);

/*********************************************************************
    IE Name: Hysteresis

    Description: Used within the entry and leave condition of an
                 event triggered reporting condition

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_hysteresis_ie(uint8   hysteresis,
                                                uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_hysteresis_ie(uint8 **ie_ptr,
                                                  uint8  *hysteresis);

/*********************************************************************
    IE Name: Location Info

    Description: Transfers location information available at the UE to
                 correlate measurements and UE position information

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: Meas Config

    Description: Specifies measurements to be performed by the UE,
                 and covers intra-frequency, inter-frequency and
                 inter-RAT mobility as well as configuration of
                 measurement gaps

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_EXPLICIT_LIST_OF_ARFCNS 31
#define LIBLTE_RRC_MAX_CELL_MEAS               32
#define LIBLTE_RRC_MAX_OBJECT_ID               32
#define LIBLTE_RRC_MAX_REPORT_CONFIG_ID        32
#define LIBLTE_RRC_MAX_MEAS_ID                 32
// Enums
typedef enum{
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC0 = 0,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC1,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC2,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC3,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC4,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC5,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC6,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC7,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC8,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC9,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC10,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC11,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC12,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC13,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC14,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC15,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC16,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_BC17,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE14,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE13,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE12,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE11,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE10,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE9,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE8,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE7,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE6,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE5,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE4,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE3,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE2,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_SPARE1,
    LIBLTE_RRC_BAND_CLASS_CDMA2000_N_ITEMS,
}LIBLTE_RRC_BAND_CLASS_CDMA2000_ENUM;
static const char liblte_rrc_band_class_cdma2000_text[LIBLTE_RRC_BAND_CLASS_CDMA2000_N_ITEMS][20] = {    "0",     "1",     "2",     "3",
                                                                                                         "4",     "5",     "6",     "7",
                                                                                                         "8",     "9",    "10",    "11",
                                                                                                        "12",    "13",    "14",    "15",
                                                                                                        "16",    "17", "SPARE", "SPARE",
                                                                                                     "SPARE", "SPARE", "SPARE", "SPARE",
                                                                                                     "SPARE", "SPARE", "SPARE", "SPARE",
                                                                                                     "SPARE", "SPARE", "SPARE", "SPARE"};
static const int8 liblte_rrc_band_class_cdma2000_num[LIBLTE_RRC_BAND_CLASS_CDMA2000_N_ITEMS] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
                                                                                                16, 17, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
typedef enum{
    LIBLTE_RRC_BAND_INDICATOR_GERAN_DCS1800 = 0,
    LIBLTE_RRC_BAND_INDICATOR_GERAN_PCS1900,
    LIBLTE_RRC_BAND_INDICATOR_GERAN_N_ITEMS,
}LIBLTE_RRC_BAND_INDICATOR_GERAN_ENUM;
static const char liblte_rrc_band_indicator_geran_text[LIBLTE_RRC_BAND_INDICATOR_GERAN_N_ITEMS][20] = {"DCS1800", "PCS1900"};
typedef enum{
    LIBLTE_RRC_FOLLOWING_ARFCNS_EXPLICIT_LIST = 0,
    LIBLTE_RRC_FOLLOWING_ARFCNS_EQUALLY_SPACED,
    LIBLTE_RRC_FOLLOWING_ARFCNS_VARIABLE_BIT_MAP,
    LIBLTE_RRC_FOLLOWING_ARFCNS_N_ITEMS,
}LIBLTE_RRC_FOLLOWING_ARFCNS_ENUM;
static const char liblte_rrc_following_arfcns_text[LIBLTE_RRC_FOLLOWING_ARFCNS_N_ITEMS][20] = {"Explicit List", "Equally Spaced", "Variable Bit Map"};
typedef enum{
    LIBLTE_RRC_CDMA2000_TYPE_1XRTT = 0,
    LIBLTE_RRC_CDMA2000_TYPE_HRPD,
    LIBLTE_RRC_CDMA2000_TYPE_N_ITEMS,
}LIBLTE_RRC_CDMA2000_TYPE_ENUM;
static const char liblte_rrc_cdma2000_type_text[LIBLTE_RRC_CDMA2000_TYPE_N_ITEMS][20] = {"1xrtt", "hrpd"};
typedef enum{
    LIBLTE_RRC_T_EVALUATION_S30 = 0,
    LIBLTE_RRC_T_EVALUATION_S60,
    LIBLTE_RRC_T_EVALUATION_S120,
    LIBLTE_RRC_T_EVALUATION_S180,
    LIBLTE_RRC_T_EVALUATION_S240,
    LIBLTE_RRC_T_EVALUATION_SPARE3,
    LIBLTE_RRC_T_EVALUATION_SPARE2,
    LIBLTE_RRC_T_EVALUATION_SPARE1,
    LIBLTE_RRC_T_EVALUATION_N_ITEMS,
}LIBLTE_RRC_T_EVALUATION_ENUM;
static const char liblte_rrc_t_evaluation_text[LIBLTE_RRC_T_EVALUATION_N_ITEMS][20] = {   "30",    "60",   "120",   "180",
                                                                                         "240", "SPARE", "SPARE", "SPARE"};
static const int16 liblte_rrc_t_evaluation_num[LIBLTE_RRC_T_EVALUATION_N_ITEMS] = {30, 60, 120, 180, 240, -1, -1, -1};
typedef enum{
    LIBLTE_RRC_T_HYST_NORMAL_S30 = 0,
    LIBLTE_RRC_T_HYST_NORMAL_S60,
    LIBLTE_RRC_T_HYST_NORMAL_S120,
    LIBLTE_RRC_T_HYST_NORMAL_S180,
    LIBLTE_RRC_T_HYST_NORMAL_S240,
    LIBLTE_RRC_T_HYST_NORMAL_SPARE3,
    LIBLTE_RRC_T_HYST_NORMAL_SPARE2,
    LIBLTE_RRC_T_HYST_NORMAL_SPARE1,
    LIBLTE_RRC_T_HYST_NORMAL_N_ITEMS,
}LIBLTE_RRC_T_HYST_NORMAL_ENUM;
static const char liblte_rrc_t_hyst_normal_text[LIBLTE_RRC_T_HYST_NORMAL_N_ITEMS][20] = {   "30",    "60",   "120",   "180",
                                                                                           "240", "SPARE", "SPARE", "SPARE"};
static const int16 liblte_rrc_t_hyst_normal_num[LIBLTE_RRC_T_HYST_NORMAL_N_ITEMS] = {30, 60, 120, 180, 240, -1, -1, -1};
typedef enum{
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N4 = 0,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N8,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N12,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N16,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N24,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N32,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N48,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N64,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N84,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N96,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N128,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N168,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N252,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N504,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_SPARE2,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_SPARE1,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N1,
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_N_ITEMS,
}LIBLTE_RRC_PHYS_CELL_ID_RANGE_ENUM;
static const char liblte_rrc_phys_cell_id_range_text[LIBLTE_RRC_PHYS_CELL_ID_RANGE_N_ITEMS][20] = {    "4",     "8",    "12",    "16",
                                                                                                      "24",    "32",    "48",    "64",
                                                                                                      "84",    "96",   "128",   "168",
                                                                                                     "252",   "504", "SPARE", "SPARE",
                                                                                                       "1"};
static const int16 liblte_rrc_phys_cell_id_range_num[LIBLTE_RRC_PHYS_CELL_ID_RANGE_N_ITEMS] = {4, 8, 12, 16, 24, 32, 48, 64, 84, 96, 128, 168, 252, 504, -1, -1, 1};
typedef enum{
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N24 = 0,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N22,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N20,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N18,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N16,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N14,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N12,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N10,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N8,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N6,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N5,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N4,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N3,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N2,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_N1,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_0,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_1,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_2,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_3,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_4,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_5,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_6,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_8,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_10,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_12,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_14,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_16,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_18,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_20,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_22,
    LIBLTE_RRC_Q_OFFSET_RANGE_DB_24,
    LIBLTE_RRC_Q_OFFSET_RANGE_N_ITEMS,
}LIBLTE_RRC_Q_OFFSET_RANGE_ENUM;
static const char liblte_rrc_q_offset_range_text[LIBLTE_RRC_Q_OFFSET_RANGE_N_ITEMS][20] = {"-24", "-22", "-20", "-18",
                                                                                           "-16", "-14", "-12", "-10",
                                                                                            "-8",  "-6",  "-5",  "-4",
                                                                                            "-3",  "-2",  "-1",   "0",
                                                                                             "1",   "2",   "3",   "4",
                                                                                             "5",   "6",   "8",  "10",
                                                                                            "12",  "14",  "16",  "18",
                                                                                            "20",  "22",  "24"};
static const int8 liblte_rrc_q_offset_range_num[LIBLTE_RRC_Q_OFFSET_RANGE_N_ITEMS] = {-24, -22, -20, -18, -16, -14, -12, -10,  -8,  -6,  -5,  -4,  -3,  -2,  -1,   0,
                                                                                        1,   2,   3,   4,   5,   6,   8,  10,  12,  14,  16,  18,  20,  22,  24};
typedef enum{
    LIBLTE_RRC_SSSF_MEDIUM_0DOT25 = 0,
    LIBLTE_RRC_SSSF_MEDIUM_0DOT5,
    LIBLTE_RRC_SSSF_MEDIUM_0DOT75,
    LIBLTE_RRC_SSSF_MEDIUM_1DOT0,
    LIBLTE_RRC_SSSF_MEDIUM_N_ITEMS,
}LIBLTE_RRC_SSSF_MEDIUM_ENUM;
static const char liblte_rrc_sssf_medium_text[LIBLTE_RRC_SSSF_MEDIUM_N_ITEMS][20] = {"0.25", "0.5", "0.75", "1.0"};
static const double liblte_rrc_sssf_medium_num[LIBLTE_RRC_SSSF_MEDIUM_N_ITEMS] = {0.25, 0.5, 0.75, 1.0};
typedef enum{
    LIBLTE_RRC_SSSF_HIGH_0DOT25 = 0,
    LIBLTE_RRC_SSSF_HIGH_0DOT5,
    LIBLTE_RRC_SSSF_HIGH_0DOT75,
    LIBLTE_RRC_SSSF_HIGH_1DOT0,
    LIBLTE_RRC_SSSF_HIGH_N_ITEMS,
}LIBLTE_RRC_SSSF_HIGH_ENUM;
static const char liblte_rrc_sssf_high_text[LIBLTE_RRC_SSSF_HIGH_N_ITEMS][20] = {"0.25", "0.5", "0.75", "1.0"};
static const double liblte_rrc_sssf_high_num[LIBLTE_RRC_SSSF_HIGH_N_ITEMS] = {0.25, 0.5, 0.75, 1.0};
typedef enum{
    LIBLTE_RRC_GAP_OFFSET_TYPE_GP0 = 0,
    LIBLTE_RRC_GAP_OFFSET_TYPE_GP1,
    LIBLTE_RRC_GAP_OFFSET_TYPE_N_ITEMS,
}LIBLTE_RRC_GAP_OFFSET_TYPE_ENUM;
static const char liblte_rrc_gap_offset_type_text[LIBLTE_RRC_GAP_OFFSET_TYPE_N_ITEMS][20] = {"GP0",
                                                                                             "GP1"};
typedef enum{
    LIBLTE_RRC_UTRA_SYSTEM_TYPE_FDD = 0,
    LIBLTE_RRC_UTRA_SYSTEM_TYPE_TDD,
    LIBLTE_RRC_UTRA_SYSTEM_TYPE_N_ITEMS,
}LIBLTE_RRC_UTRA_SYSTEM_TYPE_ENUM;
static const char liblte_rrc_utra_system_type_text[LIBLTE_RRC_UTRA_SYSTEM_TYPE_N_ITEMS][20] = {"FDD",
                                                                                               "TDD"};
typedef enum{
    LIBLTE_RRC_MEAS_OBJECT_TYPE_EUTRA = 0,
    LIBLTE_RRC_MEAS_OBJECT_TYPE_UTRA,
    LIBLTE_RRC_MEAS_OBJECT_TYPE_GERAN,
    LIBLTE_RRC_MEAS_OBJECT_TYPE_CDMA2000,
    LIBLTE_RRC_MEAS_OBJECT_TYPE_N_ITEMS,
}LIBLTE_RRC_MEAS_OBJECT_TYPE_ENUM;
static const char liblte_rrc_meas_object_type_text[LIBLTE_RRC_MEAS_OBJECT_TYPE_N_ITEMS][20] = {"EUTRA",
                                                                                               "UTRA",
                                                                                               "GERAN",
                                                                                               "CDMA2000"};
typedef enum{
    LIBLTE_RRC_MEAS_QUANTITY_UTRA_FDD_CPICH_RSCP = 0,
    LIBLTE_RRC_MEAS_QUANTITY_UTRA_FDD_CPICH_ECNO,
    LIBLTE_RRC_MEAS_QUANTITY_UTRA_FDD_N_ITEMS,
}LIBLTE_RRC_MEAS_QUANTITY_UTRA_FDD_ENUM;
static const char liblte_rrc_meas_quantity_utra_fdd_text[LIBLTE_RRC_MEAS_QUANTITY_UTRA_FDD_N_ITEMS][20] = {"CPICH RSCP",
                                                                                                           "CPICH Ec/No"};
typedef enum{
    LIBLTE_RRC_MEAS_QUANTITY_UTRA_TDD_PCCPCH_RSCP = 0,
    LIBLTE_RRC_MEAS_QUANTITY_UTRA_TDD_N_ITEMS,
}LIBLTE_RRC_MEAS_QUANTITY_UTRA_TDD_ENUM;
static const char liblte_rrc_meas_quantity_utra_tdd_text[LIBLTE_RRC_MEAS_QUANTITY_UTRA_TDD_N_ITEMS][20] = {"PCCPCH RSCP"};
typedef enum{
    LIBLTE_RRC_MEAS_QUANTITY_GERAN_RSSI = 0,
    LIBLTE_RRC_MEAS_QUANTITY_GERAN_N_ITEMS,
}LIBLTE_RRC_MEAS_QUANTITY_GERAN_ENUM;
static const char liblte_rrc_meas_quantity_geran_text[LIBLTE_RRC_MEAS_QUANTITY_GERAN_N_ITEMS][20] = {"RSSI"};
typedef enum{
    LIBLTE_RRC_MEAS_QUANTITY_CDMA2000_PILOT_STRENGTH = 0,
    LIBLTE_RRC_MEAS_QUANTITY_CDMA2000_PILOT_PN_PHASE_AND_STRENGTH,
    LIBLTE_RRC_MEAS_QUANTITY_CDMA2000_N_ITEMS,
}LIBLTE_RRC_MEAS_QUANTITY_CDMA2000_ENUM;
static const char liblte_rrc_meas_quantity_cdma2000_text[LIBLTE_RRC_MEAS_QUANTITY_CDMA2000_N_ITEMS][100] = {"Pilot Strength",
                                                                                                            "Pilot PN Phase and Strength"};
typedef enum{
    LIBLTE_RRC_REPORT_INTERVAL_MS120 = 0,
    LIBLTE_RRC_REPORT_INTERVAL_MS240,
    LIBLTE_RRC_REPORT_INTERVAL_MS480,
    LIBLTE_RRC_REPORT_INTERVAL_MS640,
    LIBLTE_RRC_REPORT_INTERVAL_MS1024,
    LIBLTE_RRC_REPORT_INTERVAL_MS2048,
    LIBLTE_RRC_REPORT_INTERVAL_MS5120,
    LIBLTE_RRC_REPORT_INTERVAL_MS10240,
    LIBLTE_RRC_REPORT_INTERVAL_MIN1,
    LIBLTE_RRC_REPORT_INTERVAL_MIN6,
    LIBLTE_RRC_REPORT_INTERVAL_MIN12,
    LIBLTE_RRC_REPORT_INTERVAL_MIN30,
    LIBLTE_RRC_REPORT_INTERVAL_MIN60,
    LIBLTE_RRC_REPORT_INTERVAL_SPARE3,
    LIBLTE_RRC_REPORT_INTERVAL_SPARE2,
    LIBLTE_RRC_REPORT_INTERVAL_SPARE1,
    LIBLTE_RRC_REPORT_INTERVAL_N_ITEMS,
}LIBLTE_RRC_REPORT_INTERVAL_ENUM;
static const char liblte_rrc_report_interval_text[LIBLTE_RRC_REPORT_INTERVAL_N_ITEMS][20] = {    "120",     "240",     "480",     "640",
                                                                                                "1024",    "2048",    "5120",   "10240",
                                                                                               "60000",  "360000",  "720000", "1800000",
                                                                                             "3600000",   "SPARE",   "SPARE",   "SPARE"};
static const int32 liblte_rrc_report_interval_num[LIBLTE_RRC_REPORT_INTERVAL_N_ITEMS] = {120, 240, 480, 640, 1024, 2048, 5120, 10240, 60000, 360000, 720000, 1800000, 3600000, -1, -1, -1};
typedef enum{
    LIBLTE_RRC_TIME_TO_TRIGGER_MS0 = 0,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS40,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS64,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS80,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS100,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS128,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS160,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS256,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS320,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS480,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS512,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS640,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS1024,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS1280,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS2560,
    LIBLTE_RRC_TIME_TO_TRIGGER_MS5120,
    LIBLTE_RRC_TIME_TO_TRIGGER_N_ITEMS,
}LIBLTE_RRC_TIME_TO_TRIGGER_ENUM;
static const char liblte_rrc_time_to_trigger_text[LIBLTE_RRC_TIME_TO_TRIGGER_N_ITEMS][20] = {   "0",   "40",   "64",   "80",
                                                                                              "100",  "128",  "160",  "256",
                                                                                              "320",  "480",  "512",  "640",
                                                                                             "1024", "1280", "2560", "5120"};
static const uint16 liblte_rrc_time_to_trigger_num[LIBLTE_RRC_TIME_TO_TRIGGER_N_ITEMS] = {0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280, 2560, 5120};
typedef enum{
    LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRP = 0,
    LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_RSRQ,
    LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_N_ITEMS,
}LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_ENUM;
static const char liblte_rrc_threshold_eutra_type_text[LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_N_ITEMS][20] = {"RSRP",
                                                                                                       "RSRQ"};
typedef enum{
    LIBLTE_RRC_EVENT_ID_EUTRA_A1 = 0,
    LIBLTE_RRC_EVENT_ID_EUTRA_A2,
    LIBLTE_RRC_EVENT_ID_EUTRA_A3,
    LIBLTE_RRC_EVENT_ID_EUTRA_A4,
    LIBLTE_RRC_EVENT_ID_EUTRA_A5,
    LIBLTE_RRC_EVENT_ID_EUTRA_A6,
    LIBLTE_RRC_EVENT_ID_EUTRA_N_ITEMS,
}LIBLTE_RRC_EVENT_ID_EUTRA_ENUM;
static const char liblte_rrc_event_id_eutra_text[LIBLTE_RRC_EVENT_ID_EUTRA_N_ITEMS][20] = {"A1", "A2", "A3",
                                                                                           "A4", "A5", "A6"};
typedef enum{
    LIBLTE_RRC_PURPOSE_EUTRA_REPORT_STRONGEST_CELL = 0,
    LIBLTE_RRC_PURPOSE_EUTRA_REPORT_CGI,
    LIBLTE_RRC_PURPOSE_EUTRA_N_ITEMS,
}LIBLTE_RRC_PURPOSE_EUTRA_ENUM;
static const char liblte_rrc_purpose_eutra_text[LIBLTE_RRC_PURPOSE_EUTRA_N_ITEMS][100] = {"Report Strongest Cell",
                                                                                          "Report CGI"};
typedef enum{
    LIBLTE_RRC_TRIGGER_TYPE_EUTRA_EVENT = 0,
    LIBLTE_RRC_TRIGGER_TYPE_EUTRA_PERIODICAL,
    LIBLTE_RRC_TRIGGER_TYPE_EUTRA_N_ITEMS,
}LIBLTE_RRC_TRIGGER_TYPE_EUTRA_ENUM;
static const char liblte_rrc_trigger_type_eutra_text[LIBLTE_RRC_TRIGGER_TYPE_EUTRA_N_ITEMS][20] = {"Event",
                                                                                                   "Periodical"};
typedef enum{
    LIBLTE_RRC_TRIGGER_QUANTITY_RSRP = 0,
    LIBLTE_RRC_TRIGGER_QUANTITY_RSRQ,
    LIBLTE_RRC_TRIGGER_QUANTITY_N_ITEMS,
}LIBLTE_RRC_TRIGGER_QUANTITY_ENUM;
static const char liblte_rrc_trigger_quantity_text[LIBLTE_RRC_TRIGGER_QUANTITY_N_ITEMS][20] = {"RSRP",
                                                                                               "RSRQ"};
typedef enum{
    LIBLTE_RRC_REPORT_QUANTITY_SAME_AS_TRIGGER_QUANTITY = 0,
    LIBLTE_RRC_REPORT_QUANTITY_BOTH,
    LIBLTE_RRC_REPORT_QUANTITY_N_ITEMS,
}LIBLTE_RRC_REPORT_QUANTITY_ENUM;
static const char liblte_rrc_report_quantity_text[LIBLTE_RRC_REPORT_QUANTITY_N_ITEMS][100] = {"Same As Trigger Quantity",
                                                                                              "Both"};
typedef enum{
    LIBLTE_RRC_REPORT_AMOUNT_R1 = 0,
    LIBLTE_RRC_REPORT_AMOUNT_R2,
    LIBLTE_RRC_REPORT_AMOUNT_R4,
    LIBLTE_RRC_REPORT_AMOUNT_R8,
    LIBLTE_RRC_REPORT_AMOUNT_R16,
    LIBLTE_RRC_REPORT_AMOUNT_R32,
    LIBLTE_RRC_REPORT_AMOUNT_R64,
    LIBLTE_RRC_REPORT_AMOUNT_INFINITY,
    LIBLTE_RRC_REPORT_AMOUNT_N_ITEMS,
}LIBLTE_RRC_REPORT_AMOUNT_ENUM;
static const char liblte_rrc_report_amount_text[LIBLTE_RRC_REPORT_AMOUNT_N_ITEMS][20] = {      "r1",       "r2",       "r4",       "r8",
                                                                                              "r16",      "r32",      "r64", "INFINITY"};

static const int8 liblte_rrc_report_amount_num[LIBLTE_RRC_REPORT_AMOUNT_N_ITEMS] = {1, 2, 4, 8, 16, 32, 64, -1};

typedef enum{
    LIBLTE_RRC_THRESHOLD_UTRA_TYPE_RSCP = 0,
    LIBLTE_RRC_THRESHOLD_UTRA_TYPE_ECNO,
    LIBLTE_RRC_THRESHOLD_UTRA_TYPE_N_ITEMS,
}LIBLTE_RRC_THRESHOLD_UTRA_TYPE_ENUM;
static const char liblte_rrc_threshold_utra_type_text[LIBLTE_RRC_THRESHOLD_UTRA_TYPE_N_ITEMS][20] = {"RSCP",
                                                                                                     "Ec/No"};
typedef enum{
    LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_UTRA = 0,
    LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_GERAN,
    LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_CDMA2000,
    LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_N_ITEMS,
}LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_ENUM;
static const char liblte_rrc_threshold_inter_rat_type_text[LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_N_ITEMS][20] = {"UTRA",
                                                                                                               "GERAN",
                                                                                                               "CDMA2000"};
typedef enum{
    LIBLTE_RRC_EVENT_ID_INTER_RAT_B1 = 0,
    LIBLTE_RRC_EVENT_ID_INTER_RAT_B2,
    LIBLTE_RRC_EVENT_ID_INTER_RAT_N_ITEMS,
}LIBLTE_RRC_EVENT_ID_INTER_RAT_ENUM;
static const char liblte_rrc_event_id_inter_rat_text[LIBLTE_RRC_EVENT_ID_INTER_RAT_N_ITEMS][20] = {"B1",
                                                                                                   "B2"};
typedef enum{
    LIBLTE_RRC_PURPOSE_INTER_RAT_REPORT_STRONGEST_CELLS = 0,
    LIBLTE_RRC_PURPOSE_INTER_RAT_REPORT_STRONGEST_CELLS_FOR_SON,
    LIBLTE_RRC_PURPOSE_INTER_RAT_REPORT_CGI,
    LIBLTE_RRC_PURPOSE_INTER_RAT_N_ITEMS,
}LIBLTE_RRC_PURPOSE_INTER_RAT_ENUM;
static const char liblte_rrc_purpose_inter_rat_text[LIBLTE_RRC_PURPOSE_INTER_RAT_N_ITEMS][100] = {"Report Strongest Cells",
                                                                                                  "Report Strongest Cells for SON",
                                                                                                  "Report CGI"};
typedef enum{
    LIBLTE_RRC_TRIGGER_TYPE_INTER_RAT_EVENT = 0,
    LIBLTE_RRC_TRIGGER_TYPE_INTER_RAT_PERIODICAL,
    LIBLTE_RRC_TRIGGER_TYPE_INTER_RAT_N_ITEMS,
}LIBLTE_RRC_TRIGGER_TYPE_INTER_RAT_ENUM;
static const char liblte_rrc_trigger_type_inter_rat_text[LIBLTE_RRC_TRIGGER_TYPE_INTER_RAT_N_ITEMS][20] = {"Event",
                                                                                                           "Periodical"};
typedef enum{
    LIBLTE_RRC_REPORT_CONFIG_TYPE_EUTRA = 0,
    LIBLTE_RRC_REPORT_CONFIG_TYPE_INTER_RAT,
    LIBLTE_RRC_REPORT_CONFIG_TYPE_N_ITEMS,
}LIBLTE_RRC_REPORT_CONFIG_TYPE_ENUM;
static const char liblte_rrc_report_config_type_text[LIBLTE_RRC_REPORT_CONFIG_TYPE_N_ITEMS][20] = {"EUTRA",
                                                                                                   "Inter RAT"};
// Structs
typedef struct{
    LIBLTE_RRC_BAND_CLASS_CDMA2000_ENUM bandclass;
    uint16                              arfcn;
}LIBLTE_RRC_CARRIER_FREQ_CDMA2000_STRUCT;
typedef struct{
    uint8 arfcn_spacing;
    uint8 number_of_arfcns;
}LIBLTE_RRC_EQUALLY_SPACED_ARFCNS_STRUCT;
typedef struct{
    LIBLTE_RRC_EQUALLY_SPACED_ARFCNS_STRUCT equally_spaced_arfcns;
    LIBLTE_RRC_BAND_INDICATOR_GERAN_ENUM    band_indicator;
    LIBLTE_RRC_FOLLOWING_ARFCNS_ENUM        following_arfcns;
    uint16                                  starting_arfcn;
    uint16                                  explicit_list_of_arfcns[LIBLTE_RRC_MAX_EXPLICIT_LIST_OF_ARFCNS];
    uint16                                  variable_bit_map_of_arfcns;
    uint8                                   explicit_list_of_arfcns_size;
}LIBLTE_RRC_CARRIER_FREQS_GERAN_STRUCT;
typedef struct{
    uint32 N_cell_idx;
    uint8  cell_idx[LIBLTE_RRC_MAX_CELL_MEAS];
}LIBLTE_RRC_CELL_INDEX_LIST_STRUCT;
typedef struct{
    LIBLTE_RRC_T_EVALUATION_ENUM  t_eval;
    LIBLTE_RRC_T_HYST_NORMAL_ENUM t_hyst_normal;
    uint8                         n_cell_change_medium;
    uint8                         n_cell_change_high;
}LIBLTE_RRC_MOBILITY_STATE_PARAMETERS_STRUCT;
typedef struct{
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_ENUM range;
    uint16                             start;
}LIBLTE_RRC_PHYS_CELL_ID_RANGE_STRUCT;
typedef struct{
    uint8 ncc;
    uint8 bcc;
}LIBLTE_RRC_PHYS_CELL_ID_GERAN_STRUCT;
typedef struct{
    uint8 pre_reg_zone_id;
    uint8 secondary_pre_reg_zone_id_list[2];
    uint8 secondary_pre_reg_zone_id_list_size;
    bool  pre_reg_allowed;
    bool  pre_reg_zone_id_present;
}LIBLTE_RRC_PRE_REGISTRATION_INFO_HRPD_STRUCT;
typedef struct{
    LIBLTE_RRC_SSSF_MEDIUM_ENUM sf_medium;
    LIBLTE_RRC_SSSF_HIGH_ENUM   sf_high;
}LIBLTE_RRC_SPEED_STATE_SCALE_FACTORS_STRUCT;
typedef struct{
    LIBLTE_RRC_GAP_OFFSET_TYPE_ENUM gap_offset_type;
    uint8                           gap_offset;
    bool                            setup_present;
}LIBLTE_RRC_MEAS_GAP_CONFIG_STRUCT;
typedef struct{
    uint8 meas_id;
    uint8 meas_obj_id;
    uint8 rep_cnfg_id;
}LIBLTE_RRC_MEAS_ID_TO_ADD_MOD_STRUCT;
typedef struct{
    LIBLTE_RRC_MEAS_ID_TO_ADD_MOD_STRUCT meas_id_list[LIBLTE_RRC_MAX_MEAS_ID];
    uint32                               N_meas_id;
}LIBLTE_RRC_MEAS_ID_TO_ADD_MOD_LIST_STRUCT;
typedef struct{
    uint16 pci;
    uint8  cell_idx;
}LIBLTE_RRC_CELLS_TO_ADD_MOD_CDMA2000_STRUCT;
typedef struct{
    LIBLTE_RRC_CARRIER_FREQ_CDMA2000_STRUCT     carrier_freq;
    LIBLTE_RRC_CELL_INDEX_LIST_STRUCT           cells_to_remove_list;
    LIBLTE_RRC_CELLS_TO_ADD_MOD_CDMA2000_STRUCT cells_to_add_mod_list[LIBLTE_RRC_MAX_CELL_MEAS];
    LIBLTE_RRC_CDMA2000_TYPE_ENUM               cdma2000_type;
    uint32                                      N_cells_to_add_mod;
    uint16                                      cell_for_which_to_rep_cgi;
    uint8                                       search_win_size;
    int8                                        offset_freq;
    bool                                        search_win_size_present;
    bool                                        cells_to_remove_list_present;
    bool                                        cell_for_which_to_rep_cgi_present;
}LIBLTE_RRC_MEAS_OBJECT_CDMA2000_STRUCT;
typedef struct{
    LIBLTE_RRC_Q_OFFSET_RANGE_ENUM cell_offset;
    uint16                         pci;
    uint8                          cell_idx;
}LIBLTE_RRC_CELLS_TO_ADD_MOD_STRUCT;
typedef struct{
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_STRUCT pci_range;
    uint8                                cell_idx;
}LIBLTE_RRC_BLACK_CELLS_TO_ADD_MOD_STRUCT;
typedef struct{
    LIBLTE_RRC_CELL_INDEX_LIST_STRUCT        cells_to_remove_list;
    LIBLTE_RRC_CELLS_TO_ADD_MOD_STRUCT       cells_to_add_mod_list[LIBLTE_RRC_MAX_CELL_MEAS];
    LIBLTE_RRC_CELL_INDEX_LIST_STRUCT        black_cells_to_remove_list;
    LIBLTE_RRC_BLACK_CELLS_TO_ADD_MOD_STRUCT black_cells_to_add_mod_list[LIBLTE_RRC_MAX_CELL_MEAS];
    LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_ENUM   allowed_meas_bw;
    LIBLTE_RRC_Q_OFFSET_RANGE_ENUM           offset_freq;
    uint32                                   N_cells_to_add_mod;
    uint32                                   N_black_cells_to_add_mod;
    uint16                                   carrier_freq;
    uint16                                   cell_for_which_to_rep_cgi;
    uint8                                    neigh_cell_cnfg;
    bool                                     offset_freq_not_default;
    bool                                     presence_ant_port_1;
    bool                                     cells_to_remove_list_present;
    bool                                     black_cells_to_remove_list_present;
    bool                                     cell_for_which_to_rep_cgi_present;
}LIBLTE_RRC_MEAS_OBJECT_EUTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_CARRIER_FREQS_GERAN_STRUCT carrier_freqs;
    LIBLTE_RRC_PHYS_CELL_ID_GERAN_STRUCT  cell_for_which_to_rep_cgi;
    int8                                  offset_freq;
    uint8                                 ncc_permitted;
    bool                                  cell_for_which_to_rep_cgi_present;
}LIBLTE_RRC_MEAS_OBJECT_GERAN_STRUCT;
typedef struct{
    uint16 pci;
    uint8  cell_idx;
}LIBLTE_RRC_CELLS_TO_ADD_MOD_LIST_UTRA_FDD_STRUCT;
typedef struct{
    uint8 cell_idx;
    uint8 pci;
}LIBLTE_RRC_CELLS_TO_ADD_MOD_LIST_UTRA_TDD_STRUCT;
typedef struct{
    LIBLTE_RRC_CELLS_TO_ADD_MOD_LIST_UTRA_FDD_STRUCT cells_fdd[LIBLTE_RRC_MAX_CELL_MEAS];
    LIBLTE_RRC_CELLS_TO_ADD_MOD_LIST_UTRA_TDD_STRUCT cells_tdd[LIBLTE_RRC_MAX_CELL_MEAS];
    LIBLTE_RRC_UTRA_SYSTEM_TYPE_ENUM                 type;
    uint32                                           N_cells;
}LIBLTE_RRC_CELLS_TO_ADD_MOD_LIST_UTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_UTRA_SYSTEM_TYPE_ENUM type;
    uint16                           pci_fdd;
    uint8                            pci_tdd;
}LIBLTE_RRC_CELLS_FOR_WHICH_TO_REPORT_CGI_STRUCT;
typedef struct{
    LIBLTE_RRC_CELL_INDEX_LIST_STRUCT               cells_to_remove_list;
    LIBLTE_RRC_CELLS_TO_ADD_MOD_LIST_UTRA_STRUCT    cells_to_add_mod_list;
    LIBLTE_RRC_CELLS_FOR_WHICH_TO_REPORT_CGI_STRUCT cells_for_which_to_rep_cgi;
    uint16                                          carrier_freq;
    int8                                            offset_freq;
    bool                                            cells_to_remove_list_present;
    bool                                            cells_to_add_mod_list_present;
    bool                                            cells_for_which_to_rep_cgi_present;
}LIBLTE_RRC_MEAS_OBJECT_UTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_MEAS_OBJECT_EUTRA_STRUCT    meas_obj_eutra;
    LIBLTE_RRC_MEAS_OBJECT_UTRA_STRUCT     meas_obj_utra;
    LIBLTE_RRC_MEAS_OBJECT_GERAN_STRUCT    meas_obj_geran;
    LIBLTE_RRC_MEAS_OBJECT_CDMA2000_STRUCT meas_obj_cdma2000;
    LIBLTE_RRC_MEAS_OBJECT_TYPE_ENUM       meas_obj_type;
    uint8                                  meas_obj_id;
}LIBLTE_RRC_MEAS_OBJECT_TO_ADD_MOD_STRUCT;
typedef struct{
    LIBLTE_RRC_MEAS_OBJECT_TO_ADD_MOD_STRUCT meas_obj_list[LIBLTE_RRC_MAX_OBJECT_ID];
    uint32                                   N_meas_obj;
}LIBLTE_RRC_MEAS_OBJECT_TO_ADD_MOD_LIST_STRUCT;
typedef struct{
    LIBLTE_RRC_FILTER_COEFFICIENT_ENUM fc_rsrp;
    LIBLTE_RRC_FILTER_COEFFICIENT_ENUM fc_rsrq;
    bool                               fc_rsrp_not_default;
    bool                               fc_rsrq_not_default;
}LIBLTE_RRC_QUANTITY_CONFIG_EUTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_MEAS_QUANTITY_UTRA_FDD_ENUM mq_fdd;
    LIBLTE_RRC_MEAS_QUANTITY_UTRA_TDD_ENUM mq_tdd;
    LIBLTE_RRC_FILTER_COEFFICIENT_ENUM     fc;
    bool                                   fc_not_default;
}LIBLTE_RRC_QUANTITY_CONFIG_UTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_MEAS_QUANTITY_GERAN_ENUM mq;
    LIBLTE_RRC_FILTER_COEFFICIENT_ENUM  fc;
    bool                                fc_not_default;
}LIBLTE_RRC_QUANTITY_CONFIG_GERAN_STRUCT;
typedef struct{
    LIBLTE_RRC_MEAS_QUANTITY_CDMA2000_ENUM mq;
}LIBLTE_RRC_QUANTITY_CONFIG_CDMA2000_STRUCT;
typedef struct{
    LIBLTE_RRC_QUANTITY_CONFIG_EUTRA_STRUCT    qc_eutra;
    LIBLTE_RRC_QUANTITY_CONFIG_UTRA_STRUCT     qc_utra;
    LIBLTE_RRC_QUANTITY_CONFIG_GERAN_STRUCT    qc_geran;
    LIBLTE_RRC_QUANTITY_CONFIG_CDMA2000_STRUCT qc_cdma2000;
    bool                                       qc_eutra_present;
    bool                                       qc_utra_present;
    bool                                       qc_geran_present;
    bool                                       qc_cdma2000_present;
}LIBLTE_RRC_QUANTITY_CONFIG_STRUCT;
typedef struct{
    LIBLTE_RRC_THRESHOLD_EUTRA_TYPE_ENUM type;
    uint8                                range;
}LIBLTE_RRC_THRESHOLD_EUTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_THRESHOLD_EUTRA_STRUCT eutra;
}LIBLTE_RRC_EVENT_A1_STRUCT;
typedef struct{
    LIBLTE_RRC_THRESHOLD_EUTRA_STRUCT eutra;
}LIBLTE_RRC_EVENT_A2_STRUCT;
typedef struct{
    int8 offset;
    bool report_on_leave;
}LIBLTE_RRC_EVENT_A3_STRUCT;
typedef struct{
    LIBLTE_RRC_THRESHOLD_EUTRA_STRUCT eutra;
}LIBLTE_RRC_EVENT_A4_STRUCT;
typedef struct{
    LIBLTE_RRC_THRESHOLD_EUTRA_STRUCT eutra1;
    LIBLTE_RRC_THRESHOLD_EUTRA_STRUCT eutra2;
}LIBLTE_RRC_EVENT_A5_STRUCT;
typedef struct{
    int8 offset;
    bool report_on_leave;
}LIBLTE_RRC_EVENT_A6_STRUCT;
typedef struct{
    LIBLTE_RRC_EVENT_A1_STRUCT      event_a1;
    LIBLTE_RRC_EVENT_A2_STRUCT      event_a2;
    LIBLTE_RRC_EVENT_A3_STRUCT      event_a3;
    LIBLTE_RRC_EVENT_A4_STRUCT      event_a4;
    LIBLTE_RRC_EVENT_A5_STRUCT      event_a5;
    LIBLTE_RRC_EVENT_A6_STRUCT      event_a6;
    LIBLTE_RRC_EVENT_ID_EUTRA_ENUM  event_id;
    LIBLTE_RRC_TIME_TO_TRIGGER_ENUM time_to_trigger;
    uint8                           hysteresis;
}LIBLTE_RRC_EVENT_EUTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_PURPOSE_EUTRA_ENUM purpose;
}LIBLTE_RRC_PERIODICAL_EUTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_EVENT_EUTRA_STRUCT      event;
    LIBLTE_RRC_PERIODICAL_EUTRA_STRUCT periodical;
    LIBLTE_RRC_TRIGGER_TYPE_EUTRA_ENUM trigger_type;
    LIBLTE_RRC_TRIGGER_QUANTITY_ENUM   trigger_quantity;
    LIBLTE_RRC_REPORT_QUANTITY_ENUM    report_quantity;
    LIBLTE_RRC_REPORT_INTERVAL_ENUM    report_interval;
    LIBLTE_RRC_REPORT_AMOUNT_ENUM      report_amount;
    uint8                              max_report_cells;
}LIBLTE_RRC_REPORT_CONFIG_EUTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_THRESHOLD_UTRA_TYPE_ENUM type;
    int8                                value;
}LIBLTE_RRC_THRESHOLD_UTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_THRESHOLD_UTRA_STRUCT         utra;
    LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_ENUM type;
    uint8                                    geran;
    uint8                                    cdma2000;
}LIBLTE_RRC_EVENT_B1_STRUCT;
typedef struct{
    LIBLTE_RRC_THRESHOLD_UTRA_STRUCT         utra;
    LIBLTE_RRC_THRESHOLD_EUTRA_STRUCT        eutra;
    LIBLTE_RRC_THRESHOLD_INTER_RAT_TYPE_ENUM type2;
    uint8                                    geran;
    uint8                                    cdma2000;
}LIBLTE_RRC_EVENT_B2_STRUCT;
typedef struct{
    LIBLTE_RRC_EVENT_B1_STRUCT         event_b1;
    LIBLTE_RRC_EVENT_B2_STRUCT         event_b2;
    LIBLTE_RRC_EVENT_ID_INTER_RAT_ENUM event_id;
    LIBLTE_RRC_TIME_TO_TRIGGER_ENUM    time_to_trigger;
    uint8                              hysteresis;
}LIBLTE_RRC_EVENT_INTER_RAT_STRUCT;
typedef struct{
    LIBLTE_RRC_PURPOSE_INTER_RAT_ENUM purpose;
}LIBLTE_RRC_PERIODICAL_INTER_RAT_STRUCT;
typedef struct{
    LIBLTE_RRC_EVENT_INTER_RAT_STRUCT      event;
    LIBLTE_RRC_PERIODICAL_INTER_RAT_STRUCT periodical;
    LIBLTE_RRC_TRIGGER_TYPE_INTER_RAT_ENUM trigger_type;
    LIBLTE_RRC_REPORT_INTERVAL_ENUM        report_interval;
    LIBLTE_RRC_REPORT_AMOUNT_ENUM          report_amount;
    uint8                                  max_report_cells;
}LIBLTE_RRC_REPORT_CONFIG_INTER_RAT_STRUCT;
typedef struct{
    LIBLTE_RRC_REPORT_CONFIG_EUTRA_STRUCT     rep_cnfg_eutra;
    LIBLTE_RRC_REPORT_CONFIG_INTER_RAT_STRUCT rep_cnfg_inter_rat;
    LIBLTE_RRC_REPORT_CONFIG_TYPE_ENUM        rep_cnfg_type;
    uint8                                     rep_cnfg_id;
}LIBLTE_RRC_REPORT_CONFIG_TO_ADD_MOD_STRUCT;
typedef struct{
    LIBLTE_RRC_REPORT_CONFIG_TO_ADD_MOD_STRUCT rep_cnfg_list[LIBLTE_RRC_MAX_REPORT_CONFIG_ID];
    uint32                                     N_rep_cnfg;
}LIBLTE_RRC_REPORT_CONFIG_TO_ADD_MOD_LIST_STRUCT;
typedef struct{
    LIBLTE_RRC_MOBILITY_STATE_PARAMETERS_STRUCT mob_state_params;
    LIBLTE_RRC_SPEED_STATE_SCALE_FACTORS_STRUCT time_to_trig_sf;
}LIBLTE_RRC_SPEED_STATE_PARAMS_STRUCT;
typedef struct{
    LIBLTE_RRC_MEAS_OBJECT_TO_ADD_MOD_LIST_STRUCT   meas_obj_to_add_mod_list;
    LIBLTE_RRC_REPORT_CONFIG_TO_ADD_MOD_LIST_STRUCT rep_cnfg_to_add_mod_list;
    LIBLTE_RRC_MEAS_ID_TO_ADD_MOD_LIST_STRUCT       meas_id_to_add_mod_list;
    LIBLTE_RRC_QUANTITY_CONFIG_STRUCT               quantity_cnfg;
    LIBLTE_RRC_MEAS_GAP_CONFIG_STRUCT               meas_gap_cnfg;
    LIBLTE_RRC_PRE_REGISTRATION_INFO_HRPD_STRUCT    pre_reg_info_hrpd;
    LIBLTE_RRC_SPEED_STATE_PARAMS_STRUCT            speed_state_params;
    uint32                                          N_meas_obj_to_remove;
    uint32                                          N_rep_cnfg_to_remove;
    uint32                                          N_meas_id_to_remove;
    uint8                                           meas_obj_to_remove_list[LIBLTE_RRC_MAX_OBJECT_ID];
    uint8                                           rep_cnfg_to_remove_list[LIBLTE_RRC_MAX_REPORT_CONFIG_ID];
    uint8                                           meas_id_to_remove_list[LIBLTE_RRC_MAX_MEAS_ID];
    uint8                                           s_meas;
    bool                                            meas_obj_to_add_mod_list_present;
    bool                                            rep_cnfg_to_add_mod_list_present;
    bool                                            meas_id_to_add_mod_list_present;
    bool                                            quantity_cnfg_present;
    bool                                            meas_gap_cnfg_present;
    bool                                            s_meas_present;
    bool                                            pre_reg_info_hrpd_present;
    bool                                            speed_state_params_present;
}LIBLTE_RRC_MEAS_CONFIG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_config_ie(LIBLTE_RRC_MEAS_CONFIG_STRUCT  *meas_cnfg,
                                                 uint8                         **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_config_ie(uint8                         **ie_ptr,
                                                   LIBLTE_RRC_MEAS_CONFIG_STRUCT  *meas_cnfg);

/*********************************************************************
    IE Name: Meas Gap Config

    Description: Specifies the measurement gap configuration and
                 controls setup/release of measurement gaps

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Meas Gap Config enum defined above
// Structs
// Meas Gap Config struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_gap_config_ie(LIBLTE_RRC_MEAS_GAP_CONFIG_STRUCT  *meas_gap_cnfg,
                                                     uint8                             **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_gap_config_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_MEAS_GAP_CONFIG_STRUCT  *meas_gap_cnfg);

/*********************************************************************
    IE Name: Meas ID

    Description: Identifies a measurement configuration, i.e. linking
                 of a measurement object and a reporting configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_id_ie(uint8   meas_id,
                                             uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_id_ie(uint8 **ie_ptr,
                                               uint8  *meas_id);

/*********************************************************************
    IE Name: Meas Id To Add Mod List

    Description: Concerns a list of measurement identities to add or
                 modify, with for each entry the meas ID, the
                 associated meas object ID and the associated report
                 config ID

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Structs
// Meas ID To Add Mod List structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_id_to_add_mod_list_ie(LIBLTE_RRC_MEAS_ID_TO_ADD_MOD_LIST_STRUCT  *list,
                                                             uint8                                     **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_id_to_add_mod_list_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_RRC_MEAS_ID_TO_ADD_MOD_LIST_STRUCT  *list);

/*********************************************************************
    IE Name: Meas Object CDMA2000

    Description: Specifies information applicable for inter-RAT
                 CDMA2000 neighboring cells

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Structs
// Meas Object CDMA2000 structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_object_cdma2000_ie(LIBLTE_RRC_MEAS_OBJECT_CDMA2000_STRUCT  *meas_obj_cdma2000,
                                                          uint8                                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_object_cdma2000_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_RRC_MEAS_OBJECT_CDMA2000_STRUCT  *meas_obj_cdma2000);

/*********************************************************************
    IE Name: Meas Object EUTRA

    Description: Specifies information applicable for intra-frequency
                 or inter-frequency E-UTRA cells

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Structs
// Meas Object EUTRA structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_object_eutra_ie(LIBLTE_RRC_MEAS_OBJECT_EUTRA_STRUCT  *meas_obj_eutra,
                                                       uint8                               **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_object_eutra_ie(uint8                               **ie_ptr,
                                                         LIBLTE_RRC_MEAS_OBJECT_EUTRA_STRUCT  *meas_obj_eutra);

/*********************************************************************
    IE Name: Meas Object GERAN

    Description: Specifies information applicable for inter-RAT
                 GERAN neighboring frequencies

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Structs
// Meas Object GERAN struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_object_geran_ie(LIBLTE_RRC_MEAS_OBJECT_GERAN_STRUCT  *meas_obj_geran,
                                                       uint8                               **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_object_geran_ie(uint8                               **ie_ptr,
                                                         LIBLTE_RRC_MEAS_OBJECT_GERAN_STRUCT  *meas_obj_geran);

/*********************************************************************
    IE Name: Meas Object ID

    Description: Identifies a measurement object configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_object_id_ie(uint8   meas_object_id,
                                                    uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_object_id_ie(uint8 **ie_ptr,
                                                      uint8  *meas_object_id);

/*********************************************************************
    IE Name: Meas Object To Add Mod List

    Description: Concerns a list of measurement objects to add or
                 modify

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Meas Object To Add Mod List enum defined above
// Structs
// Meas Object To Add Mod List structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_object_to_add_mod_list_ie(LIBLTE_RRC_MEAS_OBJECT_TO_ADD_MOD_LIST_STRUCT  *list,
                                                                 uint8                                         **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_object_to_add_mod_list_ie(uint8                                         **ie_ptr,
                                                                   LIBLTE_RRC_MEAS_OBJECT_TO_ADD_MOD_LIST_STRUCT  *list);

/*********************************************************************
    IE Name: Meas Object UTRA

    Description: Specifies information applicable for inter-RAT UTRA
                 neighboring cells

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Meas Object UTRA define defined above
// Enums
// Meas Object UTRA enum defined above
// Structs
// Meas Object UTRA structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_object_utra_ie(LIBLTE_RRC_MEAS_OBJECT_UTRA_STRUCT  *meas_obj_utra,
                                                      uint8                              **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_object_utra_ie(uint8                              **ie_ptr,
                                                        LIBLTE_RRC_MEAS_OBJECT_UTRA_STRUCT  *meas_obj_utra);

/*********************************************************************
    IE Name: Meas Results

    Description: Covers measured results for intra-frequency,
                 inter-frequency and inter-RAT mobility

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: Quantity Config

    Description: Specifies the measurement quantities and layer 3
                 filtering coefficients for E-UTRA and inter-RAT
                 measurements

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Quantity Config enums defined above
// Structs
// Quantity Config structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_quantity_config_ie(LIBLTE_RRC_QUANTITY_CONFIG_STRUCT  *qc,
                                                     uint8                             **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_quantity_config_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_QUANTITY_CONFIG_STRUCT  *qc);

/*********************************************************************
    IE Name: Report Config EUTRA

    Description: Specifies criteria for triggering of an E-UTRA
                 measurement reporting event

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Report Config EUTRA enums defined above
// Structs
// Report Config EUTRA structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_report_config_eutra_ie(LIBLTE_RRC_REPORT_CONFIG_EUTRA_STRUCT  *rep_cnfg_eutra,
                                                         uint8                                 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_report_config_eutra_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_REPORT_CONFIG_EUTRA_STRUCT  *rep_cnfg_eutra);

/*********************************************************************
    IE Name: Report Config ID

    Description: Identifies a measurement reporting configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_report_config_id_ie(uint8   report_cnfg_id,
                                                      uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_report_config_id_ie(uint8 **ie_ptr,
                                                        uint8  *report_cnfg_id);

/*********************************************************************
    IE Name: Report Config Inter RAT

    Description: Specifies criteria for triggering of an inter-RAT
                 measurement reporting event

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Report Config Inter RAT enums defined above
// Structs
// Report Config Inter RAT structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_report_config_inter_rat_ie(LIBLTE_RRC_REPORT_CONFIG_INTER_RAT_STRUCT  *rep_cnfg_inter_rat,
                                                             uint8                                     **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_report_config_inter_rat_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_RRC_REPORT_CONFIG_INTER_RAT_STRUCT  *rep_cnfg_inter_rat);

/*********************************************************************
    IE Name: Report Config To Add Mod List

    Description: Concerns a list of reporting configurations to add
                 or modify

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Report Config To Add Mod List enum defined above
// Structs
// Report Config To Add Mod List structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_report_config_to_add_mod_list_ie(LIBLTE_RRC_REPORT_CONFIG_TO_ADD_MOD_LIST_STRUCT  *list,
                                                                   uint8                                           **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_report_config_to_add_mod_list_ie(uint8                                           **ie_ptr,
                                                                     LIBLTE_RRC_REPORT_CONFIG_TO_ADD_MOD_LIST_STRUCT  *list);

/*********************************************************************
    IE Name: Report Interval

    Description: Indicates the interval between periodic reports

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Report Interval enum defined above
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_report_interval_ie(LIBLTE_RRC_REPORT_INTERVAL_ENUM   report_int,
                                                     uint8                           **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_report_interval_ie(uint8                           **ie_ptr,
                                                       LIBLTE_RRC_REPORT_INTERVAL_ENUM  *report_int);

/*********************************************************************
    IE Name: RSRP Range

    Description: Specifies the value range used in RSRP measurements
                 and thresholds

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rsrp_range_ie(uint8   rsrp_range,
                                                uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rsrp_range_ie(uint8 **ie_ptr,
                                                  uint8  *rsrp_range);

/*********************************************************************
    IE Name: RSRQ Range

    Description: Specifies the value range used in RSRQ measurements
                 and thresholds

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rsrq_range_ie(uint8   rsrq_range,
                                                uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rsrq_range_ie(uint8 **ie_ptr,
                                                  uint8  *rsrq_range);

/*********************************************************************
    IE Name: Time To Trigger

    Description: Specifies the value range used for the time to
                 trigger parameter, which concerns the time during
                 which specific criteria for the event needs to be
                 met in order to trigger a measurement report

    Document Reference: 36.331 v10.0.0 Section 6.3.5
*********************************************************************/
// Defines
// Enums
// Time To Trigger enum defined above
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_time_to_trigger_ie(LIBLTE_RRC_TIME_TO_TRIGGER_ENUM   time_to_trigger,
                                                     uint8                           **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_time_to_trigger_ie(uint8                           **ie_ptr,
                                                       LIBLTE_RRC_TIME_TO_TRIGGER_ENUM  *time_to_trigger);

/*********************************************************************
    IE Name: Additional Spectrum Emission

    Description: FIXME

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_additional_spectrum_emission_ie(uint8   add_spect_em,
                                                                  uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_additional_spectrum_emission_ie(uint8 **ie_ptr,
                                                                    uint8  *add_spect_em);

/*********************************************************************
    IE Name: ARFCN value CDMA2000

    Description: Indicates the CDMA2000 carrier frequency within
                 a CDMA2000 band

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_arfcn_value_cdma2000_ie(uint16   arfcn,
                                                          uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_arfcn_value_cdma2000_ie(uint8  **ie_ptr,
                                                            uint16  *arfcn);

/*********************************************************************
    IE Name: ARFCN value EUTRA

    Description: Indicates the ARFCN applicable for a downlink,
                 uplink, or bi-directional (TDD) E-UTRA carrier
                 frequency

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_arfcn_value_eutra_ie(uint16   arfcn,
                                                       uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_arfcn_value_eutra_ie(uint8  **ie_ptr,
                                                         uint16  *arfcn);

/*********************************************************************
    IE Name: ARFCN value GERAN

    Description: Specifies the ARFCN value applicable for a GERAN
                 BCCH carrier frequency

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_arfcn_value_geran_ie(uint16   arfcn,
                                                       uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_arfcn_value_geran_ie(uint8  **ie_ptr,
                                                         uint16  *arfcn);

/*********************************************************************
    IE Name: ARFCN value UTRA

    Description: Indicates the ARFCN applicable for a downlink (Nd,
                 FDD) or bi-directional (Nt, TDD) UTRA carrier
                 frequency

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_arfcn_value_utra_ie(uint16   arfcn,
                                                      uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_arfcn_value_utra_ie(uint8  **ie_ptr,
                                                        uint16  *arfcn);

/*********************************************************************
    IE Name: Band Class CDMA2000

    Description: Defines the CDMA2000 band in which the CDMA2000
                 carrier frequency can be found

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Band Class CDMA2000 enum defined above
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_band_class_cdma2000_ie(LIBLTE_RRC_BAND_CLASS_CDMA2000_ENUM   bc_cdma2000,
                                                         uint8                               **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_band_class_cdma2000_ie(uint8                               **ie_ptr,
                                                           LIBLTE_RRC_BAND_CLASS_CDMA2000_ENUM  *bc_cdma2000);

/*********************************************************************
    IE Name: Band Indicator GERAN

    Description: Indicates how to interpret an associated GERAN
                 carrier ARFCN

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Band Indicator GERAN enum defined above
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_band_indicator_geran_ie(LIBLTE_RRC_BAND_INDICATOR_GERAN_ENUM   bi_geran,
                                                          uint8                                **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_band_indicator_geran_ie(uint8                                **ie_ptr,
                                                            LIBLTE_RRC_BAND_INDICATOR_GERAN_ENUM  *bi_geran);

/*********************************************************************
    IE Name: Carrier Freq CDMA2000

    Description: Provides the CDMA2000 carrier information

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Carrier Freq CDMA2000 struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_carrier_freq_cdma2000_ie(LIBLTE_RRC_CARRIER_FREQ_CDMA2000_STRUCT  *carrier_freq,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_carrier_freq_cdma2000_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_CARRIER_FREQ_CDMA2000_STRUCT  *carrier_freq);

/*********************************************************************
    IE Name: Carrier Freq GERAN

    Description: Provides an unambiguous carrier frequency description
                 of a GERAN cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_BAND_INDICATOR_GERAN_ENUM band_indicator;
    uint16                               arfcn;
}LIBLTE_RRC_CARRIER_FREQ_GERAN_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_carrier_freq_geran_ie(LIBLTE_RRC_CARRIER_FREQ_GERAN_STRUCT  *carrier_freq,
                                                        uint8                                **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_carrier_freq_geran_ie(uint8                                **ie_ptr,
                                                          LIBLTE_RRC_CARRIER_FREQ_GERAN_STRUCT  *carrier_freq);

/*********************************************************************
    IE Name: Carrier Freqs GERAN

    Description: Provides one or more GERAN ARFCN values, which
                 represent a list of GERAN BCCH carrier frequencies

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Carrier Freqs GERAN define defined above
// Enums
// Carrier Freqs GERAN enum defined above
// Structs
// Carrier Freqs GERAN structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_carrier_freqs_geran_ie(LIBLTE_RRC_CARRIER_FREQS_GERAN_STRUCT  *carrier_freqs,
                                                         uint8                                 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_carrier_freqs_geran_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_CARRIER_FREQS_GERAN_STRUCT  *carrier_freqs);

/*********************************************************************
    IE Name: CDMA2000 Type

    Description: Describes the type of CDMA2000 network

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// CDMA2000 Type enum defined above
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_cdma2000_type_ie(LIBLTE_RRC_CDMA2000_TYPE_ENUM   cdma2000_type,
                                                   uint8                         **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cdma2000_type_ie(uint8                         **ie_ptr,
                                                     LIBLTE_RRC_CDMA2000_TYPE_ENUM  *cdma2000_type);

/*********************************************************************
    IE Name: Cell Identity

    Description: Unambiguously identifies a cell within a PLMN

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_identity_ie(uint32   cell_id,
                                                   uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_identity_ie(uint8  **ie_ptr,
                                                     uint32  *cell_id);

/*********************************************************************
    IE Name: Cell Index List

    Description: Concerns a list of cell indecies, which may be used
                 for different purposes

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Cell Index List struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_index_list_ie(LIBLTE_RRC_CELL_INDEX_LIST_STRUCT  *cell_idx_list,
                                                     uint8                             **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_index_list_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_CELL_INDEX_LIST_STRUCT  *cell_idx_list);

/*********************************************************************
    IE Name: Cell Reselection Priority

    Description: Contains the absolute priority of the concerned
                 carrier frequency/set of frequencies (GERAN)/
                 bandclass (CDMA2000), as used by the cell
                 reselection procedure

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_reselection_priority_ie(uint8   cell_resel_prio,
                                                               uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_reselection_priority_ie(uint8 **ie_ptr,
                                                                 uint8  *cell_resel_prio);

/*********************************************************************
    IE Name: CSFB Registration Param 1xRTT

    Description: Indicates whether or not the UE shall perform a
                 CDMA2000 1xRTT pre-registration if the UE does not
                 have a valid/current pre-registration

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_POWER_DOWN_REG_R9_TRUE = 0,
    LIBLTE_RRC_POWER_DOWN_REG_R9_N_ITEMS,
}LIBLTE_RRC_POWER_DOWN_REG_R9_ENUM;
static const char liblte_rrc_power_down_reg_r9_text[LIBLTE_RRC_POWER_DOWN_REG_R9_N_ITEMS][20] = {"TRUE"};
// Structs
typedef struct{
    uint16 sid;
    uint16 nid;
    uint16 reg_zone;
    uint8  reg_period;
    uint8  total_zone;
    uint8  zone_timer;
    bool   multiple_sid;
    bool   multiple_nid;
    bool   home_reg;
    bool   foreign_sid_reg;
    bool   foreign_nid_reg;
    bool   param_reg;
    bool   power_up_reg;
}LIBLTE_RRC_CSFB_REGISTRATION_PARAM_1XRTT_STRUCT;
typedef struct{
    LIBLTE_RRC_POWER_DOWN_REG_R9_ENUM power_down_reg;
}LIBLTE_RRC_CSFB_REGISTRATION_PARAM_1XRTT_V920_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_csfb_registration_param_1xrtt_ie(LIBLTE_RRC_CSFB_REGISTRATION_PARAM_1XRTT_STRUCT  *csfb_reg_param,
                                                                   uint8                                           **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_csfb_registration_param_1xrtt_ie(uint8                                           **ie_ptr,
                                                                     LIBLTE_RRC_CSFB_REGISTRATION_PARAM_1XRTT_STRUCT  *csfb_reg_param);
LIBLTE_ERROR_ENUM liblte_rrc_pack_csfb_registration_param_1xrtt_v920_ie(LIBLTE_RRC_CSFB_REGISTRATION_PARAM_1XRTT_V920_STRUCT  *csfb_reg_param,
                                                                        uint8                                                **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_csfb_registration_param_1xrtt_v920_ie(uint8                                                **ie_ptr,
                                                                          LIBLTE_RRC_CSFB_REGISTRATION_PARAM_1XRTT_V920_STRUCT  *csfb_reg_param);

/*********************************************************************
    IE Name: Cell Global ID EUTRA

    Description: Specifies the Evolved Cell Global Identifier (ECGI),
                 the globally unique identity of a cell in E-UTRA

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint16 mcc;
    uint16 mnc;
}LIBLTE_RRC_PLMN_IDENTITY_STRUCT;
typedef struct{
    LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id;
    uint32                          cell_id;
}LIBLTE_RRC_CELL_GLOBAL_ID_EUTRA_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_global_id_eutra_ie(LIBLTE_RRC_CELL_GLOBAL_ID_EUTRA_STRUCT  *cell_global_id,
                                                          uint8                                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_global_id_eutra_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_RRC_CELL_GLOBAL_ID_EUTRA_STRUCT  *cell_global_id);

/*********************************************************************
    IE Name: Cell Global ID UTRA

    Description: Specifies the global UTRAN Cell Identifier, the
                 globally unique identity of a cell in UTRA

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id;
    uint32                          cell_id;
}LIBLTE_RRC_CELL_GLOBAL_ID_UTRA_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_global_id_utra_ie(LIBLTE_RRC_CELL_GLOBAL_ID_UTRA_STRUCT  *cell_global_id,
                                                         uint8                                 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_global_id_utra_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_CELL_GLOBAL_ID_UTRA_STRUCT  *cell_global_id);

/*********************************************************************
    IE Name: Cell Global ID GERAN

    Description: Specifies the Cell Global Identity (CGI), the
                 globally unique identity of a cell in GERAN

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id;
    uint16                          lac;
    uint16                          cell_id;
}LIBLTE_RRC_CELL_GLOBAL_ID_GERAN_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_global_id_geran_ie(LIBLTE_RRC_CELL_GLOBAL_ID_GERAN_STRUCT  *cell_global_id,
                                                          uint8                                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_global_id_geran_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_RRC_CELL_GLOBAL_ID_GERAN_STRUCT  *cell_global_id);

/*********************************************************************
    IE Name: Cell Global ID CDMA2000

    Description: Specifies the Cell Global Identity (CGI), the
                 globally unique identity of a cell in CDMA2000

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint64 onexrtt;
    uint32 hrpd[4];
}LIBLTE_RRC_CELL_GLOBAL_ID_CDMA2000_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_cell_global_id_cdma2000_ie(LIBLTE_RRC_CELL_GLOBAL_ID_CDMA2000_STRUCT  *cell_global_id,
                                                             uint8                                     **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cell_global_id_cdma2000_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_RRC_CELL_GLOBAL_ID_CDMA2000_STRUCT  *cell_global_id);

/*********************************************************************
    IE Name: CSG Identity

    Description: Identifies a Closed Subscriber Group

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
#define LIBLTE_RRC_CSG_IDENTITY_NOT_PRESENT 0xFFFFFFFF
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_csg_identity_ie(uint32   csg_id,
                                                  uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_csg_identity_ie(uint8  **ie_ptr,
                                                    uint32  *csg_id);

/*********************************************************************
    IE Name: Mobility Control Info

    Description: Includes parameters relevant for network controlled
                 mobility to/within E-UTRA

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_ANTENNA_PORTS_COUNT_AN1 = 0,
    LIBLTE_RRC_ANTENNA_PORTS_COUNT_AN2,
    LIBLTE_RRC_ANTENNA_PORTS_COUNT_AN4,
    LIBLTE_RRC_ANTENNA_PORTS_COUNT_SPARE1,
    LIBLTE_RRC_ANTENNA_PORTS_COUNT_N_ITEMS,
}LIBLTE_RRC_ANTENNA_PORTS_COUNT_ENUM;
static const char liblte_rrc_antenna_ports_count_text[LIBLTE_RRC_ANTENNA_PORTS_COUNT_N_ITEMS][20] = {"an1", "an2", "an4", "SPARE"};
typedef enum{
    LIBLTE_RRC_PHICH_DURATION_NORMAL = 0,
    LIBLTE_RRC_PHICH_DURATION_EXTENDED,
    LIBLTE_RRC_PHICH_DURATION_N_ITEMS,
}LIBLTE_RRC_PHICH_DURATION_ENUM;
static const char liblte_rrc_phich_duration_text[LIBLTE_RRC_PHICH_DURATION_N_ITEMS][20] = {"Normal", "Extended"};
typedef enum{
    LIBLTE_RRC_PHICH_RESOURCE_1_6 = 0,
    LIBLTE_RRC_PHICH_RESOURCE_1_2,
    LIBLTE_RRC_PHICH_RESOURCE_1,
    LIBLTE_RRC_PHICH_RESOURCE_2,
    LIBLTE_RRC_PHICH_RESOURCE_N_ITEMS,
}LIBLTE_RRC_PHICH_RESOURCE_ENUM;
static const char liblte_rrc_phich_resource_text[LIBLTE_RRC_PHICH_RESOURCE_N_ITEMS][20] = {"1/6", "1/2", "1", "2"};
static const double liblte_rrc_phich_resource_num[LIBLTE_RRC_PHICH_RESOURCE_N_ITEMS] = {0.16666667, 0.5, 1, 2};
typedef enum{
    LIBLTE_RRC_DELTA_PUCCH_SHIFT_DS1 = 0,
    LIBLTE_RRC_DELTA_PUCCH_SHIFT_DS2,
    LIBLTE_RRC_DELTA_PUCCH_SHIFT_DS3,
    LIBLTE_RRC_DELTA_PUCCH_SHIFT_N_ITEMS,
}LIBLTE_RRC_DELTA_PUCCH_SHIFT_ENUM;
static const char liblte_rrc_delta_pucch_shift_text[LIBLTE_RRC_DELTA_PUCCH_SHIFT_N_ITEMS][20] = {"1", "2", "3"};
static const uint8 liblte_rrc_delta_pucch_shift_num[LIBLTE_RRC_DELTA_PUCCH_SHIFT_N_ITEMS] = {1, 2, 3};
typedef enum{
    LIBLTE_RRC_HOPPING_MODE_INTER_SUBFRAME = 0,
    LIBLTE_RRC_HOPPING_MODE_INTRA_AND_INTER_SUBFRAME,
    LIBLTE_RRC_HOOPPING_MODE_N_ITEMS,
}LIBLTE_RRC_HOPPING_MODE_ENUM;
static const char liblte_rrc_hopping_mode_text[LIBLTE_RRC_HOOPPING_MODE_N_ITEMS][20] = {"inter-subframe","intra-subframe"};
typedef enum{
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N4 = 0,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N8,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N12,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N16,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N20,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N24,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N28,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N32,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N36,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N40,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N44,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N48,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N52,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N56,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N60,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N64,
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N_ITEMS,
}LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_ENUM;
static const char liblte_rrc_number_of_ra_preambles_text[LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N_ITEMS][20] = { "4",  "8", "12", "16",
                                                                                                           "20", "24", "28", "32",
                                                                                                           "36", "40", "44", "48",
                                                                                                           "52", "56", "60", "64"};
static const uint8 liblte_rrc_number_of_ra_preambles_num[LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N_ITEMS] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64};
typedef enum{
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N4 = 0,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N8,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N12,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N16,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N20,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N24,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N28,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N32,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N36,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N40,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N44,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N48,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N52,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N56,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N60,
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N_ITEMS,
}LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_ENUM;
static const char liblte_rrc_size_of_ra_preambles_group_a_text[LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N_ITEMS][20] = { "4",  "8", "12", "16",
                                                                                                                       "20", "24", "28", "32",
                                                                                                                       "36", "40", "44", "48",
                                                                                                                       "52", "56", "60"};
static const uint8 liblte_rrc_size_of_ra_preambles_group_a_num[LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N_ITEMS] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60};
typedef enum{
    LIBLTE_RRC_MESSAGE_SIZE_GROUP_A_B56 = 0,
    LIBLTE_RRC_MESSAGE_SIZE_GROUP_A_B144,
    LIBLTE_RRC_MESSAGE_SIZE_GROUP_A_B208,
    LIBLTE_RRC_MESSAGE_SIZE_GROUP_A_B256,
    LIBLTE_RRC_MESSAGE_SIZE_GROUP_A_N_ITEMS,
}LIBLTE_RRC_MESSAGE_SIZE_GROUP_A_ENUM;
static const char liblte_rrc_message_size_group_a_text[LIBLTE_RRC_MESSAGE_SIZE_GROUP_A_N_ITEMS][20] = {"56", "144", "208", "256"};
static const uint16 liblte_rrc_message_size_group_a_num[LIBLTE_RRC_MESSAGE_SIZE_GROUP_A_N_ITEMS] = {56, 144, 208, 256};
typedef enum{
    LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_MINUS_INFINITY = 0,
    LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_DB0,
    LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_DB5,
    LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_DB8,
    LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_DB10,
    LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_DB12,
    LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_DB15,
    LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_DB18,
    LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_N_ITEMS,
}LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_ENUM;
static const char liblte_rrc_message_power_offset_group_b_text[LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_N_ITEMS][20] = {"-INFINITY",         "0",         "5",         "8",
                                                                                                                              "10",        "12",        "15",        "18"};
static const int liblte_rrc_message_power_offset_group_b_num[LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_N_ITEMS] = {-1,         0,         5,         8,
                                                                                                                              10,        12,        15,        18};
typedef enum{
    LIBLTE_RRC_POWER_RAMPING_STEP_DB0 = 0,
    LIBLTE_RRC_POWER_RAMPING_STEP_DB2,
    LIBLTE_RRC_POWER_RAMPING_STEP_DB4,
    LIBLTE_RRC_POWER_RAMPING_STEP_DB6,
    LIBLTE_RRC_POWER_RAMPING_STEP_N_ITEMS,
}LIBLTE_RRC_POWER_RAMPING_STEP_ENUM;
static const char liblte_rrc_power_ramping_step_text[LIBLTE_RRC_POWER_RAMPING_STEP_N_ITEMS][20] = {"0", "2", "4", "6"};
static const uint8 liblte_rrc_power_ramping_step_num[LIBLTE_RRC_POWER_RAMPING_STEP_N_ITEMS] = {0, 2, 4, 6};
typedef enum{
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N120 = 0,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N118,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N116,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N114,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N112,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N110,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N108,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N106,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N104,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N102,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N100,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N98,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N96,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N94,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N92,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N90,
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_N_ITEMS,
}LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_ENUM;
static const char liblte_rrc_preamble_initial_received_target_power_text[LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_N_ITEMS][20] = {"-120", "-118", "-116", "-114",
                                                                                                                                           "-112", "-110", "-108", "-106",
                                                                                                                                           "-104", "-102", "-100",  "-98",
                                                                                                                                            "-96",  "-94",  "-92",  "-90"};
static const int8 liblte_rrc_preamble_initial_received_target_power_num[LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_N_ITEMS] = {-120, -118, -116, -114, -112, -110, -108, -106,
                                                                                                                                      -104, -102, -100,  -98,  -96,  -94,  -92,  -90};
typedef enum{
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_N3 = 0,
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_N4,
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_N5,
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_N6,
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_N7,
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_N8,
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_N10,
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_N20,
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_N50,
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_N100,
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_N200,
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_N_ITEMS,
}LIBLTE_RRC_PREAMBLE_TRANS_MAX_ENUM;
static const char liblte_rrc_preamble_trans_max_text[LIBLTE_RRC_PREAMBLE_TRANS_MAX_N_ITEMS][20] = {  "3",   "4",   "5",   "6",
                                                                                                     "7",   "8",  "10",  "20",
                                                                                                    "50", "100", "200"};
static const uint8 liblte_rrc_preamble_trans_max_num[LIBLTE_RRC_PREAMBLE_TRANS_MAX_N_ITEMS] = {3, 4, 5, 6, 7, 8, 10, 20, 50, 100, 200};
typedef enum{
    LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_SF2 = 0,
    LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_SF3,
    LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_SF4,
    LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_SF5,
    LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_SF6,
    LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_SF7,
    LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_SF8,
    LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_SF10,
    LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_N_ITEMS,
}LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_ENUM;
static const char liblte_rrc_ra_response_window_size_text[LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_N_ITEMS][20] = { "2",  "3",  "4",  "5",
                                                                                                              "6",  "7",  "8", "10"};
static const uint8 liblte_rrc_ra_response_window_size_num[LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_N_ITEMS] = {2, 3, 4, 5, 6, 7, 8, 10};
typedef enum{
    LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_SF8 = 0,
    LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_SF16,
    LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_SF24,
    LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_SF32,
    LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_SF40,
    LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_SF48,
    LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_SF56,
    LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_SF64,
    LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_N_ITEMS,
}LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_ENUM;
static const char liblte_rrc_mac_contention_resolution_timer_text[LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_N_ITEMS][20] = { "8", "16", "24", "32",
                                                                                                                             "40", "48", "56", "64"};
static const uint8 liblte_rrc_mac_contention_resolution_timer_num[LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_N_ITEMS] = {8, 16, 24, 32, 40, 48, 56, 64};
typedef enum{
    LIBLTE_RRC_UL_CP_LENGTH_1 = 0,
    LIBLTE_RRC_UL_CP_LENGTH_2,
    LIBLTE_RRC_UL_CP_LENGTH_N_ITEMS,
}LIBLTE_RRC_UL_CP_LENGTH_ENUM;
static const char liblte_rrc_ul_cp_length_text[LIBLTE_RRC_UL_CP_LENGTH_N_ITEMS][20] = {"Normal", "Extended"};
typedef enum{
    LIBLTE_RRC_SRS_BW_CONFIG_0 = 0,
    LIBLTE_RRC_SRS_BW_CONFIG_1,
    LIBLTE_RRC_SRS_BW_CONFIG_2,
    LIBLTE_RRC_SRS_BW_CONFIG_3,
    LIBLTE_RRC_SRS_BW_CONFIG_4,
    LIBLTE_RRC_SRS_BW_CONFIG_5,
    LIBLTE_RRC_SRS_BW_CONFIG_6,
    LIBLTE_RRC_SRS_BW_CONFIG_7,
    LIBLTE_RRC_SRS_BW_CONFIG_N_ITEMS,
}LIBLTE_RRC_SRS_BW_CONFIG_ENUM;
static const char liblte_rrc_srs_bw_config_text[LIBLTE_RRC_SRS_BW_CONFIG_N_ITEMS][20] = {"0", "1", "2", "3",
                                                                                         "4", "5", "6", "7"};
static const uint8 liblte_rrc_srs_bw_config_num[LIBLTE_RRC_SRS_BW_CONFIG_N_ITEMS] = {0, 1, 2, 3, 4, 5, 6, 7};
typedef enum{
    LIBLTE_RRC_SRS_SUBFR_CONFIG_0 = 0,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_1,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_2,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_3,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_4,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_5,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_6,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_7,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_8,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_9,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_10,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_11,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_12,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_13,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_14,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_15,
    LIBLTE_RRC_SRS_SUBFR_CONFIG_N_ITEMS,
}LIBLTE_RRC_SRS_SUBFR_CONFIG_ENUM;
static const char liblte_rrc_srs_subfr_config_text[LIBLTE_RRC_SRS_SUBFR_CONFIG_N_ITEMS][20] = { "0",  "1",  "2",  "3",
                                                                                                "4",  "5",  "6",  "7",
                                                                                                "8",  "9", "10", "11",
                                                                                               "12", "13", "14", "15"};
static const uint8 liblte_rrc_srs_subfr_config_num[LIBLTE_RRC_SRS_SUBFR_CONFIG_N_ITEMS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
typedef enum{
    LIBLTE_RRC_SUBFRAME_ASSIGNMENT_0 = 0,
    LIBLTE_RRC_SUBFRAME_ASSIGNMENT_1,
    LIBLTE_RRC_SUBFRAME_ASSIGNMENT_2,
    LIBLTE_RRC_SUBFRAME_ASSIGNMENT_3,
    LIBLTE_RRC_SUBFRAME_ASSIGNMENT_4,
    LIBLTE_RRC_SUBFRAME_ASSIGNMENT_5,
    LIBLTE_RRC_SUBFRAME_ASSIGNMENT_6,
    LIBLTE_RRC_SUBFRAME_ASSIGNMENT_N_ITEMS,
}LIBLTE_RRC_SUBFRAME_ASSIGNMENT_ENUM;
static const char liblte_rrc_subframe_assignment_text[LIBLTE_RRC_SUBFRAME_ASSIGNMENT_N_ITEMS][20] = {"0", "1", "2", "3",
                                                                                                     "4", "5", "6"};
static const uint8 liblte_rrc_subframe_assignment_num[LIBLTE_RRC_SUBFRAME_ASSIGNMENT_N_ITEMS] = {0, 1, 2, 3, 4, 5, 6};
typedef enum{
    LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_0 = 0,
    LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_1,
    LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_2,
    LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_3,
    LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_4,
    LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_5,
    LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_6,
    LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_7,
    LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_8,
    LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_N_ITEMS,
}LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_ENUM;
static const char liblte_rrc_special_subframe_patterns_text[LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_N_ITEMS][20] = {"0", "1", "2", "3",
                                                                                                                 "4", "5", "6", "7",
                                                                                                                 "8"};
static const uint8 liblte_rrc_special_subframe_patterns_num[LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_N_ITEMS] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
typedef enum{
    LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_0 = 0,
    LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_04,
    LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_05,
    LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_06,
    LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_07,
    LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_08,
    LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_09,
    LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_1,
    LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_N_ITEMS,
}LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_ENUM;
static const char liblte_rrc_ul_power_control_alpha_text[LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_N_ITEMS][20] = {"0.0", "0.4", "0.5", "0.6",
                                                                                                           "0.7", "0.8", "0.9", "1.0"};
static const double liblte_rrc_ul_power_control_alpha_num[LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_N_ITEMS] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
typedef enum{
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_NEG_2 = 0,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_0,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_2,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_N_ITEMS,
}LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_ENUM;
static const char liblte_rrc_delta_f_pucch_format_1_text[LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_N_ITEMS][20] = {"-2", "0", "2"};
static const int8 liblte_rrc_delta_f_pucch_format_1_num[LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_N_ITEMS] = {-2, 0, 2};
typedef enum{
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_1 = 0,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_3,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_5,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_N_ITEMS,
}LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_ENUM;
static const char liblte_rrc_delta_f_pucch_format_1b_text[LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_N_ITEMS][20] = {"1", "3", "5"};
static const uint8 liblte_rrc_delta_f_pucch_format_1b_num[LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_N_ITEMS] = {1, 3, 5};
typedef enum{
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_NEG_2 = 0,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_0,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_1,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_2,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_N_ITEMS,
}LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_ENUM;
static const char liblte_rrc_delta_f_pucch_format_2_text[LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_N_ITEMS][20] = {"-2", "0", "1", "2"};
static const int8 liblte_rrc_delta_f_pucch_format_2_num[LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_N_ITEMS] = {-2, 0, 1, 2};
typedef enum{
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_NEG_2 = 0,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_0,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_2,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_N_ITEMS,
}LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_ENUM;
static const char liblte_rrc_delta_f_pucch_format_2a_text[LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_N_ITEMS][20] = {"-2", "0", "2"};
static const int8 liblte_rrc_delta_f_pucch_format_2a_num[LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_N_ITEMS] = {-2, 0, 2};
typedef enum{
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_NEG_2 = 0,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_0,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_2,
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_N_ITEMS,
}LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_ENUM;
static const char liblte_rrc_delta_f_pucch_format_2b_text[LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_N_ITEMS][20] = {"-2", "0", "2"};
static const int8 liblte_rrc_delta_f_pucch_format_2b_num[LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_N_ITEMS] = {-2, 0, 2};
typedef enum{
    LIBLTE_RRC_BANDWIDTH_N6 = 0,
    LIBLTE_RRC_BANDWIDTH_N15,
    LIBLTE_RRC_BANDWIDTH_N25,
    LIBLTE_RRC_BANDWIDTH_N50,
    LIBLTE_RRC_BANDWIDTH_N75,
    LIBLTE_RRC_BANDWIDTH_N100,
    LIBLTE_RRC_BANDWIDTH_SPARE10,
    LIBLTE_RRC_BANDWIDTH_SPARE9,
    LIBLTE_RRC_BANDWIDTH_SPARE8,
    LIBLTE_RRC_BANDWIDTH_SPARE7,
    LIBLTE_RRC_BANDWIDTH_SPARE6,
    LIBLTE_RRC_BANDWIDTH_SPARE5,
    LIBLTE_RRC_BANDWIDTH_SPARE4,
    LIBLTE_RRC_BANDWIDTH_SPARE3,
    LIBLTE_RRC_BANDWIDTH_SPARE2,
    LIBLTE_RRC_BANDWIDTH_SPARE1,
    LIBLTE_RRC_BANDWIDTH_N_ITEMS,
}LIBLTE_RRC_BANDWIDTH_ENUM;
static const char liblte_rrc_bandwidth_text[LIBLTE_RRC_BANDWIDTH_N_ITEMS][20] = {  "1.4",     "3",     "5",    "10",
                                                                                    "15",    "20", "SPARE", "SPARE",
                                                                                 "SPARE", "SPARE", "SPARE", "SPARE",
                                                                                 "SPARE", "SPARE", "SPARE", "SPARE"};
typedef enum{
    LIBLTE_RRC_T304_MS50 = 0,
    LIBLTE_RRC_T304_MS100,
    LIBLTE_RRC_T304_MS150,
    LIBLTE_RRC_T304_MS200,
    LIBLTE_RRC_T304_MS500,
    LIBLTE_RRC_T304_MS1000,
    LIBLTE_RRC_T304_MS2000,
    LIBLTE_RRC_T304_SPARE,
    LIBLTE_RRC_T304_N_ITEMS,
}LIBLTE_RRC_T304_ENUM;
static const char liblte_rrc_t304_text[LIBLTE_RRC_T304_N_ITEMS][20] = {   "50",   "100",   "150",   "200",
                                                                         "500",  "1000",  "2000", "SPARE"};
static const int32 liblte_rrc_t304_num[LIBLTE_RRC_T304_N_ITEMS] = {50, 100, 150, 200, 500, 1000, 2000, -1};
// Structs
typedef struct{
    uint8 p_b;
    int8  rs_power;
}LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT;
typedef struct{
    LIBLTE_RRC_PHICH_DURATION_ENUM dur;
    LIBLTE_RRC_PHICH_RESOURCE_ENUM res;
}LIBLTE_RRC_PHICH_CONFIG_STRUCT;
typedef struct{
    uint8 prach_config_index;
    uint8 zero_correlation_zone_config;
    uint8 prach_freq_offset;
    bool  high_speed_flag;
}LIBLTE_RRC_PRACH_CONFIG_INFO_STRUCT;
typedef struct{
    LIBLTE_RRC_PRACH_CONFIG_INFO_STRUCT prach_cnfg_info;
    uint16                              root_sequence_index;
}LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT;
typedef struct{
    LIBLTE_RRC_PRACH_CONFIG_INFO_STRUCT prach_cnfg_info;
    uint16                              root_sequence_index;
    bool                                prach_cnfg_info_present;
}LIBLTE_RRC_PRACH_CONFIG_STRUCT;
typedef struct{
    LIBLTE_RRC_DELTA_PUCCH_SHIFT_ENUM delta_pucch_shift;
    uint16                            n1_pucch_an;
    uint8                             n_rb_cqi;
    uint8                             n_cs_an;
}LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT;
typedef struct{
    uint8 group_assignment_pusch;
    uint8 cyclic_shift;
    bool  group_hopping_enabled;
    bool  sequence_hopping_enabled;
}LIBLTE_RRC_UL_RS_PUSCH_STRUCT;
typedef struct{
    LIBLTE_RRC_UL_RS_PUSCH_STRUCT ul_rs;
    LIBLTE_RRC_HOPPING_MODE_ENUM  hopping_mode;
    uint8                         n_sb;
    uint8                         pusch_hopping_offset;
    bool                          enable_64_qam;
}LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT;
typedef struct{
    LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_ENUM size_of_ra;
    LIBLTE_RRC_MESSAGE_SIZE_GROUP_A_ENUM         msg_size;
    LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_ENUM msg_pwr_offset_group_b;
    bool                                         present;
}LIBLTE_RRC_PREAMBLES_GROUP_A_STRUCT;
typedef struct{
    LIBLTE_RRC_PREAMBLES_GROUP_A_STRUCT                    preambles_group_a_cnfg;
    LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_ENUM                 num_ra_preambles;
    LIBLTE_RRC_POWER_RAMPING_STEP_ENUM                     pwr_ramping_step;
    LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_ENUM preamble_init_rx_target_pwr;
    LIBLTE_RRC_PREAMBLE_TRANS_MAX_ENUM                     preamble_trans_max;
    LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_ENUM                ra_resp_win_size;
    LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_ENUM        mac_con_res_timer;
    uint8                                                  max_harq_msg3_tx;
}LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT;
typedef struct{
    uint8 preamble_index;
    uint8 prach_mask_index;
}LIBLTE_RRC_RACH_CONFIG_DEDICATED_STRUCT;
typedef struct{
    LIBLTE_RRC_SRS_BW_CONFIG_ENUM    bw_cnfg;
    LIBLTE_RRC_SRS_SUBFR_CONFIG_ENUM subfr_cnfg;
    bool                             ack_nack_simul_tx;
    bool                             max_up_pts;
    bool                             max_up_pts_present;
    bool                             present;
}LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT;
typedef struct{
    LIBLTE_RRC_SUBFRAME_ASSIGNMENT_ENUM       sf_assignment;
    LIBLTE_RRC_SPECIAL_SUBFRAME_PATTERNS_ENUM special_sf_patterns;
}LIBLTE_RRC_TDD_CONFIG_STRUCT;
typedef struct{
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_ENUM  format_1;
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_ENUM format_1b;
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_ENUM  format_2;
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_ENUM format_2a;
    LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_ENUM format_2b;
}LIBLTE_RRC_DELTA_FLIST_PUCCH_STRUCT;
typedef struct{
    LIBLTE_RRC_DELTA_FLIST_PUCCH_STRUCT    delta_flist_pucch;
    LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_ENUM alpha;
    int8                                   p0_nominal_pusch;
    int8                                   p0_nominal_pucch;
    int8                                   delta_preamble_msg3;
}LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT;
typedef struct{
    LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT      rach_cnfg;
    LIBLTE_RRC_PRACH_CONFIG_STRUCT            prach_cnfg;
    LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT     pdsch_cnfg;
    LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT     pusch_cnfg;
    LIBLTE_RRC_PHICH_CONFIG_STRUCT            phich_cnfg;
    LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT     pucch_cnfg;
    LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT    srs_ul_cnfg;
    LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT ul_pwr_ctrl;
    LIBLTE_RRC_TDD_CONFIG_STRUCT              tdd_cnfg;
    LIBLTE_RRC_ANTENNA_PORTS_COUNT_ENUM       ant_info;
    LIBLTE_RRC_UL_CP_LENGTH_ENUM              ul_cp_length;
    int8                                      p_max;
    bool                                      rach_cnfg_present;
    bool                                      pdsch_cnfg_present;
    bool                                      phich_cnfg_present;
    bool                                      pucch_cnfg_present;
    bool                                      srs_ul_cnfg_present;
    bool                                      ul_pwr_ctrl_present;
    bool                                      ant_info_present;
    bool                                      p_max_present;
    bool                                      tdd_cnfg_present;
}LIBLTE_RRC_RR_CONFIG_COMMON_STRUCT;
typedef struct{
    uint16 dl_carrier_freq;
    uint16 ul_carrier_freq;
    bool   ul_carrier_freq_present;
}LIBLTE_RRC_CARRIER_FREQ_EUTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_BANDWIDTH_ENUM dl_bw;
    LIBLTE_RRC_BANDWIDTH_ENUM ul_bw;
    bool                      ul_bw_present;
}LIBLTE_RRC_CARRIER_BANDWIDTH_EUTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_CARRIER_FREQ_EUTRA_STRUCT      carrier_freq_eutra;
    LIBLTE_RRC_CARRIER_BANDWIDTH_EUTRA_STRUCT carrier_bw_eutra;
    LIBLTE_RRC_RR_CONFIG_COMMON_STRUCT        rr_cnfg_common;
    LIBLTE_RRC_RACH_CONFIG_DEDICATED_STRUCT   rach_cnfg_ded;
    LIBLTE_RRC_T304_ENUM                      t304;
    uint16                                    target_pci;
    uint16                                    new_ue_id;
    uint8                                     add_spect_em;
    bool                                      carrier_freq_eutra_present;
    bool                                      carrier_bw_eutra_present;
    bool                                      add_spect_em_present;
    bool                                      rach_cnfg_ded_present;
}LIBLTE_RRC_MOBILITY_CONTROL_INFO_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_mobility_control_info_ie(LIBLTE_RRC_MOBILITY_CONTROL_INFO_STRUCT  *mob_ctrl_info,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mobility_control_info_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_MOBILITY_CONTROL_INFO_STRUCT  *mob_ctrl_info);

/*********************************************************************
    IE Name: Mobility Parameters CDMA2000 (1xRTT)

    Description: Contains the parameters provided to the UE for
                 handover and (enhanced) CSFB to 1xRTT support

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: Mobility State Parameters

    Description: Contains parameters to determine UE mobility state

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Mobility State Parameters enums defined above
// Structs
// Mobility State Parameters struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_mobility_state_parameters_ie(LIBLTE_RRC_MOBILITY_STATE_PARAMETERS_STRUCT  *mobility_state_params,
                                                               uint8                                       **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mobility_state_parameters_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_MOBILITY_STATE_PARAMETERS_STRUCT  *mobility_state_params);

/*********************************************************************
    IE Name: Phys Cell ID

    Description: Indicates the physical layer identity of the cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_phys_cell_id_ie(uint16   phys_cell_id,
                                                  uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phys_cell_id_ie(uint8  **ie_ptr,
                                                    uint16  *phys_cell_id);

/*********************************************************************
    IE Name: Phys Cell ID Range

    Description: Encodes either a single or a range of physical cell
                 identities

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Phys Cell ID Range enum defined above
// Structs
// Phys Cell ID Range struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_phys_cell_id_range_ie(LIBLTE_RRC_PHYS_CELL_ID_RANGE_STRUCT  *phys_cell_id_range,
                                                        uint8                                **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phys_cell_id_range_ie(uint8                                **ie_ptr,
                                                          LIBLTE_RRC_PHYS_CELL_ID_RANGE_STRUCT  *phys_cell_id_range);

/*********************************************************************
    IE Name: Phys Cell ID Range UTRA FDD List

    Description: Encodes one or more of Phys Cell ID Range UTRA FDD

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: Phys Cell ID CDMA2000

    Description: Identifies the PN offset that represents the
                 "Physical cell identity" in CDMA2000

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_PN_OFFSET 511
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_phys_cell_id_cdma2000_ie(uint16   phys_cell_id,
                                                           uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phys_cell_id_cdma2000_ie(uint8  **ie_ptr,
                                                             uint16  *phys_cell_id);

/*********************************************************************
    IE Name: Phys Cell ID GERAN

    Description: Contains the Base Station Identity Code (BSIC)

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Phys Cell ID GERAN struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_phys_cell_id_geran_ie(LIBLTE_RRC_PHYS_CELL_ID_GERAN_STRUCT  *phys_cell_id,
                                                        uint8                                **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phys_cell_id_geran_ie(uint8                                **ie_ptr,
                                                          LIBLTE_RRC_PHYS_CELL_ID_GERAN_STRUCT  *phys_cell_id);

/*********************************************************************
    IE Name: Phys Cell ID UTRA FDD

    Description: Indicates the physical layer identity of the cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_phys_cell_id_utra_fdd_ie(uint16   phys_cell_id,
                                                           uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phys_cell_id_utra_fdd_ie(uint8  **ie_ptr,
                                                             uint16  *phys_cell_id);

/*********************************************************************
    IE Name: Phys Cell ID UTRA TDD

    Description: Indicates the physical layer identity of the cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_phys_cell_id_utra_tdd_ie(uint8   phys_cell_id,
                                                           uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phys_cell_id_utra_tdd_ie(uint8 **ie_ptr,
                                                             uint8  *phys_cell_id);

/*********************************************************************
    IE Name: PLMN Identity

    Description: Identifies a Public Land Mobile Network

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
#define LIBLTE_RRC_MCC_NOT_PRESENT 0xFFFF
// Enums
// Structs
// PLMN Identity struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_plmn_identity_ie(LIBLTE_RRC_PLMN_IDENTITY_STRUCT  *plmn_id,
                                                   uint8                           **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_plmn_identity_ie(uint8                           **ie_ptr,
                                                     LIBLTE_RRC_PLMN_IDENTITY_STRUCT  *plmn_id);

/*********************************************************************
    IE Name: Pre Registration Info HRPD

    Description: FIXME

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Pre Registration Info HRPD struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_pre_registration_info_hrpd_ie(LIBLTE_RRC_PRE_REGISTRATION_INFO_HRPD_STRUCT  *pre_reg_info_hrpd,
                                                                uint8                                        **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pre_registration_info_hrpd_ie(uint8                                        **ie_ptr,
                                                                  LIBLTE_RRC_PRE_REGISTRATION_INFO_HRPD_STRUCT  *pre_reg_info_hrpd);

/*********************************************************************
    IE Name: Q Qual Min

    Description: Indicates for cell selection/re-selection the
                 required minimum received RSRQ level in the (E-UTRA)
                 cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_q_qual_min_ie(int8    q_qual_min,
                                                uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_q_qual_min_ie(uint8 **ie_ptr,
                                                  int8   *q_qual_min);

/*********************************************************************
    IE Name: Q Rx Lev Min

    Description: Indicates the required minimum received RSRP level in
                 the (E-UTRA) cell for cell selection/re-selection

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_q_rx_lev_min_ie(int16   q_rx_lev_min,
                                                  uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_q_rx_lev_min_ie(uint8 **ie_ptr,
                                                    int16  *q_rx_lev_min);

/*********************************************************************
    IE Name: Q Offset Range

    Description: Indicates a cell or frequency specific offset to be
                 applied when evaluating candidates for cell
                 reselection or when evaluating triggering conditions
                 for measurement reporting

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Q Offset Range enum defined above
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_q_offset_range_ie(LIBLTE_RRC_Q_OFFSET_RANGE_ENUM   q_offset_range,
                                                    uint8                          **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_q_offset_range_ie(uint8                          **ie_ptr,
                                                      LIBLTE_RRC_Q_OFFSET_RANGE_ENUM  *q_offset_range);

/*********************************************************************
    IE Name: Q Offset Range Inter RAT

    Description: Indicates a frequency specific offset to be applied
                 when evaluating triggering conditions for
                 measurement reporting

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_q_offset_range_inter_rat_ie(int8    q_offset_range_inter_rat,
                                                              uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_q_offset_range_inter_rat_ie(uint8 **ie_ptr,
                                                                int8   *q_offset_range_inter_rat);

/*********************************************************************
    IE Name: Reselection Threshold

    Description: Indicates an RX level threshold for cell reselection

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_reselection_threshold_ie(uint8   resel_thresh,
                                                           uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_reselection_threshold_ie(uint8 **ie_ptr,
                                                             uint8  *resel_thresh);

/*********************************************************************
    IE Name: Reselection Threshold Q

    Description: Indicates a quality level threshold for cell
                 reselection

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_reselection_threshold_q_ie(uint8   resel_thresh_q,
                                                             uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_reselection_threshold_q_ie(uint8 **ie_ptr,
                                                               uint8  *resel_thresh_q);

/*********************************************************************
    IE Name: S Cell Index

    Description: Contains a short identity, used to identify an
                 SCell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_s_cell_index_ie(uint8   s_cell_idx,
                                                  uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_s_cell_index_ie(uint8 **ie_ptr,
                                                    uint8  *s_cell_idx);

/*********************************************************************
    IE Name: Serv Cell Index

    Description: Contains a short identity, used to identify a
                 serving cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_serv_cell_index_ie(uint8   serv_cell_idx,
                                                     uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_serv_cell_index_ie(uint8 **ie_ptr,
                                                       uint8  *serv_cell_idx);

/*********************************************************************
    IE Name: Speed State Scale Factors

    Description: Contains factors, to be applied when the UE is in
                 medium or high speed state, used for scaling a
                 mobility control related parameter

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Speed State Scale Factors enums defined above
// Structs
// Speed State Scale Factors struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_speed_state_scale_factors_ie(LIBLTE_RRC_SPEED_STATE_SCALE_FACTORS_STRUCT  *speed_state_scale_factors,
                                                               uint8                                       **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_speed_state_scale_factors_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_SPEED_STATE_SCALE_FACTORS_STRUCT  *speed_state_scale_factors);

/*********************************************************************
    IE Name: System Info List GERAN

    Description: Contains system information of a GERAN cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: System Time Info CDMA2000

    Description: Informs the UE about the absolute time in the current
                 cell

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint64 system_time;
    bool   system_time_async;
    bool   cdma_eutra_sync;
}LIBLTE_RRC_SYSTEM_TIME_INFO_CDMA2000_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_system_time_info_cdma2000_ie(LIBLTE_RRC_SYSTEM_TIME_INFO_CDMA2000_STRUCT  *sys_time_info_cdma2000,
                                                               uint8                                       **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_system_time_info_cdma2000_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_SYSTEM_TIME_INFO_CDMA2000_STRUCT  *sys_time_info_cdma2000);

/*********************************************************************
    IE Name: Tracking Area Code

    Description: Identifies a tracking area within the scope of a
                 PLMN

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_tracking_area_code_ie(uint16   tac,
                                                        uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_tracking_area_code_ie(uint8  **ie_ptr,
                                                          uint16  *tac);

/*********************************************************************
    IE Name: T Reselection

    Description: Contains the timer T_reselection_rat for E-UTRA,
                 UTRA, GERAN, or CDMA2000

    Document Reference: 36.331 v10.0.0 Section 6.3.4
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_t_reselection_ie(uint8   t_resel,
                                                   uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_t_reselection_ie(uint8 **ie_ptr,
                                                     uint8  *t_resel);

/*********************************************************************
    IE Name: Next Hop Chaining Count

    Description: Updates the Kenb key and corresponds to parameter
                 NCC

    Document Reference: 36.331 v10.0.0 Section 6.3.3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_next_hop_chaining_count_ie(uint8   next_hop_chaining_count,
                                                             uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_next_hop_chaining_count_ie(uint8 **ie_ptr,
                                                               uint8  *next_hop_chaining_count);

/*********************************************************************
    IE Name: Security Algorithm Config

    Description: Configures AS integrity protection algorithm (SRBs)
                 and AS ciphering algorithm (SRBs and DRBs)

    Document Reference: 36.331 v10.0.0 Section 6.3.3
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_CIPHERING_ALGORITHM_EEA0 = 0,
    LIBLTE_RRC_CIPHERING_ALGORITHM_EEA1,
    LIBLTE_RRC_CIPHERING_ALGORITHM_EEA2,
    LIBLTE_RRC_CIPHERING_ALGORITHM_SPARE5,
    LIBLTE_RRC_CIPHERING_ALGORITHM_SPARE4,
    LIBLTE_RRC_CIPHERING_ALGORITHM_SPARE3,
    LIBLTE_RRC_CIPHERING_ALGORITHM_SPARE2,
    LIBLTE_RRC_CIPHERING_ALGORITHM_SPARE1,
    LIBLTE_RRC_CIPHERING_ALGORITHM_N_ITEMS,
}LIBLTE_RRC_CIPHERING_ALGORITHM_ENUM;
static const char liblte_rrc_ciphering_algorithm_text[LIBLTE_RRC_CIPHERING_ALGORITHM_N_ITEMS][20] = { "EEA0",  "EEA1",  "EEA2", "SPARE",
                                                                                                     "SPARE", "SPARE", "SPARE", "SPARE"};
typedef enum{
    LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_EIA0_V920 = 0,
    LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_EIA1,
    LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_EIA2,
    LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_SPARE5,
    LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_SPARE4,
    LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_SPARE3,
    LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_SPARE2,
    LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_SPARE1,
    LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_N_ITEMS,
}LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_ENUM;
static const char liblte_rrc_integrity_prot_algorithm_text[LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_N_ITEMS][20] = { "EIA0",  "EIA1",  "EIA2", "SPARE",
                                                                                                               "SPARE", "SPARE", "SPARE", "SPARE"};
// Structs
typedef struct{
    LIBLTE_RRC_CIPHERING_ALGORITHM_ENUM      cipher_alg;
    LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_ENUM int_alg;
}LIBLTE_RRC_SECURITY_ALGORITHM_CONFIG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_security_algorithm_config_ie(LIBLTE_RRC_SECURITY_ALGORITHM_CONFIG_STRUCT  *sec_alg_cnfg,
                                                               uint8                                       **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_security_algorithm_config_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_SECURITY_ALGORITHM_CONFIG_STRUCT  *sec_alg_cnfg);

/*********************************************************************
    IE Name: Short MAC I

    Description: Identifies and verifies the UE at RRC connection
                 re-establishment

    Document Reference: 36.331 v10.0.0 Section 6.3.3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_short_mac_i_ie(uint16   short_mac_i,
                                                 uint8  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_short_mac_i_ie(uint8  **ie_ptr,
                                                   uint16  *short_mac_i);

/*********************************************************************
    IE Name: Antenna Info

    Description: Specifies the common and the UE specific antenna
                 configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Antenna Ports Count enum defined above
typedef enum{
    LIBLTE_RRC_TRANSMISSION_MODE_1 = 0,
    LIBLTE_RRC_TRANSMISSION_MODE_2,
    LIBLTE_RRC_TRANSMISSION_MODE_3,
    LIBLTE_RRC_TRANSMISSION_MODE_4,
    LIBLTE_RRC_TRANSMISSION_MODE_5,
    LIBLTE_RRC_TRANSMISSION_MODE_6,
    LIBLTE_RRC_TRANSMISSION_MODE_7,
    LIBLTE_RRC_TRANSMISSION_MODE_8,
    LIBLTE_RRC_TRANSMISSION_MODE_N_ITEMS,
}LIBLTE_RRC_TRANSMISSION_MODE_ENUM;
static const char liblte_rrc_transmission_mode_text[LIBLTE_RRC_TRANSMISSION_MODE_N_ITEMS][20] = {"1", "2", "3", "4",
                                                                                                 "5", "6", "7", "8"};
static const uint8 liblte_rrc_transmission_mode_num[LIBLTE_RRC_TRANSMISSION_MODE_N_ITEMS] = {1, 2, 3, 4, 5, 6, 7, 8};
typedef enum{
    LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM3 = 0,
    LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N4_TM3,
    LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM4,
    LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N4_TM4,
    LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM5,
    LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N4_TM5,
    LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM6,
    LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N4_TM6,
    LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N_ITEMS,
}LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_CHOICE_ENUM;
static const char liblte_rrc_codebook_subset_restriction_choice_text[LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N_ITEMS][20] = {"n2_tm3", "n4_tm3", "n2_tm4", "n4_tm4",
                                                                                                                            "n2_tm5", "n4_tm5", "n2_tm6", "n4_tm6"};
typedef enum{
    LIBLTE_RRC_UE_TX_ANTENNA_SELECTION_CLOSED_LOOP = 0,
    LIBLTE_RRC_UE_TX_ANTENNA_SELECTION_OPEN_LOOP,
    LIBLTE_RRC_UE_TX_ANTENNA_SELECTION_N_ITEMS,
}LIBLTE_RRC_UE_TX_ANTENNA_SELECTION_ENUM;
static const char liblte_rrc_ue_tx_antenna_selection_text[LIBLTE_RRC_UE_TX_ANTENNA_SELECTION_N_ITEMS][20] = {"closed_loop", "open_loop"};
// Structs
typedef struct{
    LIBLTE_RRC_TRANSMISSION_MODE_ENUM                  tx_mode;
    LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_CHOICE_ENUM codebook_subset_restriction_choice;
    LIBLTE_RRC_UE_TX_ANTENNA_SELECTION_ENUM            ue_tx_antenna_selection_setup;
    uint64                                             codebook_subset_restriction;
    bool                                               codebook_subset_restriction_present;
    bool                                               ue_tx_antenna_selection_setup_present;
}LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_antenna_info_common_ie(LIBLTE_RRC_ANTENNA_PORTS_COUNT_ENUM   antenna_ports_cnt,
                                                         uint8                               **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_antenna_info_common_ie(uint8                               **ie_ptr,
                                                           LIBLTE_RRC_ANTENNA_PORTS_COUNT_ENUM  *antenna_ports_cnt);
LIBLTE_ERROR_ENUM liblte_rrc_pack_antenna_info_dedicated_ie(LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT  *antenna_info,
                                                            uint8                                    **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_antenna_info_dedicated_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT  *antenna_info);

/*********************************************************************
    IE Name: CQI Report Config

    Description: Specifies the CQI reporting configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM12 = 0,
    LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM20,
    LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM22,
    LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM30,
    LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM31,
    LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_SPARE3,
    LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_SPARE2,
    LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_SPARE1,
    LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_N_ITEMS,
}LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_ENUM;
static const char liblte_rrc_cqi_report_mode_aperiodic_text[LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_N_ITEMS][20] = { "rm12",  "rm20",  "rm22",  "rm30",
                                                                                                                  "rm31", "SPARE", "SPARE", "SPARE"};
typedef enum{
    LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_WIDEBAND_CQI = 0,
    LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND_CQI,
    LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_N_ITEMS,
}LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_ENUM;
static const char liblte_rrc_cqi_format_indicator_periodic_text[LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_N_ITEMS][20] = {"wideband_cqi", "subband_cqi"};
// Structs
typedef struct{
    LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_ENUM format_ind_periodic;
    uint32                                        pucch_resource_idx;
    uint32                                        pmi_cnfg_idx;
    uint32                                        ri_cnfg_idx;
    uint32                                        format_ind_periodic_subband_k;
    bool                                          ri_cnfg_idx_present;
    bool                                          simult_ack_nack_and_cqi;
}LIBLTE_RRC_CQI_REPORT_PERIODIC_STRUCT;
typedef struct{
    LIBLTE_RRC_CQI_REPORT_PERIODIC_STRUCT     report_periodic;
    LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_ENUM report_mode_aperiodic;
    int32                                     nom_pdsch_rs_epre_offset;
    bool                                      report_mode_aperiodic_present;
    bool                                      report_periodic_present;
    bool                                      report_periodic_setup_present;
}LIBLTE_RRC_CQI_REPORT_CONFIG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_cqi_report_config_ie(LIBLTE_RRC_CQI_REPORT_CONFIG_STRUCT  *cqi_report_cnfg,
                                                       uint8                               **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cqi_report_config_ie(uint8                               **ie_ptr,
                                                         LIBLTE_RRC_CQI_REPORT_CONFIG_STRUCT  *cqi_report_cnfg);

/*********************************************************************
    IE Name: Cross Carrier Scheduling Config

    Description: Specifies the configuration when the cross carrier
                 scheduling is used in a cell

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: CSI RS Config

    Description: Specifies the CSI (Channel State Information)
                 reference signal configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: DRB Identity

    Description: Identifies a DRB used by a UE

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_drb_identity_ie(uint8   drb_id,
                                                  uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_drb_identity_ie(uint8 **ie_ptr,
                                                    uint8  *drb_id);

/*********************************************************************
    IE Name: Logical Channel Config

    Description: Configures the logical channel parameters

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_KBPS_0 = 0,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_KBPS_8,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_KBPS_16,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_KBPS_32,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_KBPS_64,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_KBPS_128,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_KBPS_256,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_INFINITY,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_SPARE8,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_SPARE7,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_SPARE6,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_SPARE5,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_SPARE4,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_SPARE3,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_SPARE2,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_SPARE1,
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_N_ITEMS,
}LIBLTE_RRC_PRIORITIZED_BIT_RATE_ENUM;
static const char liblte_rrc_prioritized_bit_rate_text[LIBLTE_RRC_PRIORITIZED_BIT_RATE_N_ITEMS][20] = {       "0",        "8",       "16",       "32",
                                                                                                             "64",      "128",      "256", "INFINITY",
                                                                                                          "SPARE",    "SPARE",    "SPARE",    "SPARE",
                                                                                                          "SPARE",    "SPARE",    "SPARE",    "SPARE"};
static const int liblte_rrc_prioritized_bit_rate_num[LIBLTE_RRC_PRIORITIZED_BIT_RATE_N_ITEMS] = {         0 ,         8 ,        16 ,        32 ,
                                                                                                         64 ,       128 ,       256 ,        -1 ,
                                                                                                         -1 ,        -1 ,        -1 ,        -1 ,
                                                                                                         -1 ,        -1 ,        -1 ,        -1 };
typedef enum{
    LIBLTE_RRC_BUCKET_SIZE_DURATION_MS50 = 0,
    LIBLTE_RRC_BUCKET_SIZE_DURATION_MS100,
    LIBLTE_RRC_BUCKET_SIZE_DURATION_MS150,
    LIBLTE_RRC_BUCKET_SIZE_DURATION_MS300,
    LIBLTE_RRC_BUCKET_SIZE_DURATION_MS500,
    LIBLTE_RRC_BUCKET_SIZE_DURATION_MS1000,
    LIBLTE_RRC_BUCKET_SIZE_DURATION_SPARE2,
    LIBLTE_RRC_BUCKET_SIZE_DURATION_SPARE1,
    LIBLTE_RRC_BUCKET_SIZE_DURATION_N_ITEMS,
}LIBLTE_RRC_BUCKET_SIZE_DURATION_ENUM;
static const char liblte_rrc_bucket_size_duration_text[LIBLTE_RRC_BUCKET_SIZE_DURATION_N_ITEMS][20] = {   "50",   "100",   "150",   "300",
                                                                                                         "500",  "1000", "SPARE", "SPARE"};
static const int16 liblte_rrc_bucket_size_duration_num[LIBLTE_RRC_BUCKET_SIZE_DURATION_N_ITEMS] = {50, 100, 150, 300, 500, 1000, -1, -1};
typedef enum{
    LIBLTE_RRC_LOGICAL_CHANNEL_SR_MASK_R9_SETUP = 0,
    LIBLTE_RRC_LOGICAL_CHANNEL_SR_MASK_R9_N_ITEMS,
}LIBLTE_RRC_LOGICAL_CHANNEL_SR_MASK_R9_ENUM;
static const char liblte_rrc_logical_channel_sr_mask_r9_text[LIBLTE_RRC_LOGICAL_CHANNEL_SR_MASK_R9_N_ITEMS][20] = {"SETUP"};
// Structs
typedef struct{
    LIBLTE_RRC_PRIORITIZED_BIT_RATE_ENUM prioritized_bit_rate;
    LIBLTE_RRC_BUCKET_SIZE_DURATION_ENUM bucket_size_duration;
    uint8                                priority;
    uint8                                log_chan_group;
    bool                                 log_chan_group_present;
}LIBLTE_RRC_UL_SPECIFIC_PARAMETERS_STRUCT;
typedef struct{
    LIBLTE_RRC_UL_SPECIFIC_PARAMETERS_STRUCT   ul_specific_params;
    LIBLTE_RRC_LOGICAL_CHANNEL_SR_MASK_R9_ENUM log_chan_sr_mask;
    bool                                       ul_specific_params_present;
    bool                                       log_chan_sr_mask_present;
}LIBLTE_RRC_LOGICAL_CHANNEL_CONFIG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_logical_channel_config_ie(LIBLTE_RRC_LOGICAL_CHANNEL_CONFIG_STRUCT  *log_chan_cnfg,
                                                            uint8                                    **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_logical_channel_config_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_RRC_LOGICAL_CHANNEL_CONFIG_STRUCT  *log_chan_cnfg);

/*********************************************************************
    IE Name: MAC Main Config

    Description: Specifies the MAC main configuration for signalling
                 and data radio bearers

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_MAX_HARQ_TX_N1 = 0,
    LIBLTE_RRC_MAX_HARQ_TX_N2,
    LIBLTE_RRC_MAX_HARQ_TX_N3,
    LIBLTE_RRC_MAX_HARQ_TX_N4,
    LIBLTE_RRC_MAX_HARQ_TX_N5,
    LIBLTE_RRC_MAX_HARQ_TX_N6,
    LIBLTE_RRC_MAX_HARQ_TX_N7,
    LIBLTE_RRC_MAX_HARQ_TX_N8,
    LIBLTE_RRC_MAX_HARQ_TX_N10,
    LIBLTE_RRC_MAX_HARQ_TX_N12,
    LIBLTE_RRC_MAX_HARQ_TX_N16,
    LIBLTE_RRC_MAX_HARQ_TX_N20,
    LIBLTE_RRC_MAX_HARQ_TX_N24,
    LIBLTE_RRC_MAX_HARQ_TX_N28,
    LIBLTE_RRC_MAX_HARQ_TX_SPARE2,
    LIBLTE_RRC_MAX_HARQ_TX_SPARE1,
    LIBLTE_RRC_MAX_HARQ_TX_N_ITEMS,
}LIBLTE_RRC_MAX_HARQ_TX_ENUM;
static const char liblte_rrc_max_harq_tx_text[LIBLTE_RRC_MAX_HARQ_TX_N_ITEMS][20] = {    "1",     "2",     "3",     "4",
                                                                                         "5",     "6",     "7",     "8",
                                                                                        "10",    "12",    "16",    "20",
                                                                                        "24",    "28", "SPARE", "SPARE"};
static const int8 liblte_rrc_max_harq_tx_num[LIBLTE_RRC_MAX_HARQ_TX_N_ITEMS] = {1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 16, 20, 24, 28, -1, -1};
typedef enum{
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF5 = 0,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF10,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF16,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF20,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF32,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF40,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF64,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF80,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF128,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF160,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF320,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF640,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF1280,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SF2560,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_INFINITY,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_SPARE,
    LIBLTE_RRC_PERIODIC_BSR_TIMER_N_ITEMS,
}LIBLTE_RRC_PERIODIC_BSR_TIMER_ENUM;
static const char liblte_rrc_periodic_bsr_timer_text[LIBLTE_RRC_PERIODIC_BSR_TIMER_N_ITEMS][20] = {     "sf5",     "sf10",     "sf16",     "sf20",
                                                                                                       "sf32",     "sf40",     "sf64",     "sf80",
                                                                                                      "sf128",    "sf160",    "sf320",    "sf640",
                                                                                                     "sf1280",   "sf2560", "INFINITY",    "SPARE"};
static const int32 liblte_rrc_periodic_bsr_timer_num[LIBLTE_RRC_PERIODIC_BSR_TIMER_N_ITEMS] = { 5, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640,
                                                                                                     1280, 2560, -1, -1};
typedef enum{
    LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_SF320 = 0,
    LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_SF640,
    LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_SF1280,
    LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_SF2560,
    LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_SF5120,
    LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_SF10240,
    LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_SPARE2,
    LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_SPARE1,
    LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_N_ITEMS,
}LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_ENUM;
static const char liblte_rrc_retransmission_bsr_timer_text[LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_N_ITEMS][20] = {  "sf320",   "sf640",  "sf1280",  "sf2560",
                                                                                                                "sf5120", "sf10240",   "SPARE",   "SPARE"};
static const int32 liblte_rrc_retransmission_bsr_timer_num[LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_N_ITEMS] = {  320,   640,  1280,  2560, 5120, 10240, -1, -1};
typedef enum{
    LIBLTE_RRC_ON_DURATION_TIMER_PSF1 = 0,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF2,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF3,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF4,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF5,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF6,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF8,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF10,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF20,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF30,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF40,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF50,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF60,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF80,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF100,
    LIBLTE_RRC_ON_DURATION_TIMER_PSF200,
    LIBLTE_RRC_ON_DURATION_TIMER_N_ITEMS,
}LIBLTE_RRC_ON_DURATION_TIMER_ENUM;
static const char liblte_rrc_on_duration_timer_text[LIBLTE_RRC_ON_DURATION_TIMER_N_ITEMS][20] = {  "psf1",   "psf2",   "psf3",   "psf4",
                                                                                                   "psf5",   "psf6",   "psf8",  "psf10",
                                                                                                  "psf20",  "psf30",  "psf40",  "psf50",
                                                                                                  "psf60",  "psf80", "psf100", "psf200"};
typedef enum{
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF1 = 0,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF2,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF3,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF4,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF5,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF6,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF8,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF10,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF20,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF30,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF40,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF50,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF60,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF80,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF100,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF200,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF300,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF500,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF750,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF1280,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF1920,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_PSF2560,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_SPARE10,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_SPARE9,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_SPARE8,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_SPARE7,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_SPARE6,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_SPARE5,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_SPARE4,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_SPARE3,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_SPARE2,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_SPARE1,
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_N_ITEMS,
}LIBLTE_RRC_DRX_INACTIVITY_TIMER_ENUM;
static const char liblte_rrc_drx_inactivity_timer_text[LIBLTE_RRC_DRX_INACTIVITY_TIMER_N_ITEMS][20] = {   "psf1",    "psf2",    "psf3",    "psf4",
                                                                                                          "psf5",    "psf6",    "psf8",   "psf10",
                                                                                                         "psf20",   "psf30",   "psf40",   "psf50",
                                                                                                         "psf60",   "psf80",  "psf100",  "psf200",
                                                                                                        "psf300",  "psf500",  "psf750", "psf1280",
                                                                                                       "psf1920", "psf2560",   "SPARE",   "SPARE",
                                                                                                         "SPARE",   "SPARE",   "SPARE",   "SPARE",
                                                                                                         "SPARE",   "SPARE",   "SPARE",   "SPARE"};
typedef enum{
    LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_PSF1 = 0,
    LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_PSF2,
    LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_PSF4,
    LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_PSF6,
    LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_PSF8,
    LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_PSF16,
    LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_PSF24,
    LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_PSF33,
    LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_N_ITEMS,
}LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_ENUM;
static const char liblte_rrc_drx_retransmission_timer_text[LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_N_ITEMS][20] = { "psf1",  "psf2",  "psf4",  "psf6",
                                                                                                                "psf8", "psf16", "psf24", "psf33"};
typedef enum{
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF10 = 0,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF20,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF32,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF40,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF64,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF80,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF128,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF160,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF256,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF320,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF512,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF640,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF1024,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF1280,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF2048,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_SF2560,
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_N_ITEMS,
}LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_CHOICE_ENUM;
static const char liblte_rrc_long_drx_cycle_start_offset_choice_text[LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_N_ITEMS][20] = {  "sf10",   "sf20",   "sf32",   "sf40",
                                                                                                                              "sf64",   "sf80",  "sf128",  "sf160",
                                                                                                                             "sf256",  "sf320",  "sf512",  "sf640",
                                                                                                                            "sf1024", "sf1280", "sf2048", "sf2560"};
typedef enum{
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF2 = 0,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF5,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF8,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF10,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF16,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF20,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF32,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF40,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF64,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF80,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF128,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF160,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF256,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF320,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF512,
    LIBLTE_RRC_SHORT_DRX_CYCLE_SF640,
    LIBLTE_RRC_SHORT_DRX_CYCLE_N_ITEMS,
}LIBLTE_RRC_SHORT_DRX_CYCLE_ENUM;
static const char liblte_rrc_short_drx_cycle_text[LIBLTE_RRC_SHORT_DRX_CYCLE_N_ITEMS][20] = {  "sf2",   "sf5",   "sf8",  "sf10",
                                                                                              "sf16",  "sf20",  "sf32",  "sf40",
                                                                                              "sf64",  "sf80", "sf128", "sf160",
                                                                                             "sf256", "sf320", "sf512", "sf640"};
typedef enum{
    LIBLTE_RRC_TIME_ALIGNMENT_TIMER_SF500 = 0,
    LIBLTE_RRC_TIME_ALIGNMENT_TIMER_SF750,
    LIBLTE_RRC_TIME_ALIGNMENT_TIMER_SF1280,
    LIBLTE_RRC_TIME_ALIGNMENT_TIMER_SF1920,
    LIBLTE_RRC_TIME_ALIGNMENT_TIMER_SF2560,
    LIBLTE_RRC_TIME_ALIGNMENT_TIMER_SF5120,
    LIBLTE_RRC_TIME_ALIGNMENT_TIMER_SF10240,
    LIBLTE_RRC_TIME_ALIGNMENT_TIMER_INFINITY,
    LIBLTE_RRC_TIME_ALIGNMENT_TIMER_N_ITEMS,
}LIBLTE_RRC_TIME_ALIGNMENT_TIMER_ENUM;
static const char liblte_rrc_time_alignment_timer_text[LIBLTE_RRC_TIME_ALIGNMENT_TIMER_N_ITEMS][20] = {   "sf500",    "sf750",   "sf1280",   "sf1920",
                                                                                                         "sf2560",   "sf5120",  "sf10240", "INFINITY"};
static const int liblte_rrc_time_alignment_timer_num[LIBLTE_RRC_TIME_ALIGNMENT_TIMER_N_ITEMS] = {   500,    750,   1280,   1920, 2560,   5120,  10240, -1};
typedef enum{
    LIBLTE_RRC_PERIODIC_PHR_TIMER_SF10 = 0,
    LIBLTE_RRC_PERIODIC_PHR_TIMER_SF20,
    LIBLTE_RRC_PERIODIC_PHR_TIMER_SF50,
    LIBLTE_RRC_PERIODIC_PHR_TIMER_SF100,
    LIBLTE_RRC_PERIODIC_PHR_TIMER_SF200,
    LIBLTE_RRC_PERIODIC_PHR_TIMER_SF500,
    LIBLTE_RRC_PERIODIC_PHR_TIMER_SF1000,
    LIBLTE_RRC_PERIODIC_PHR_TIMER_INFINITY,
    LIBLTE_RRC_PERIODIC_PHR_TIMER_N_ITEMS,
}LIBLTE_RRC_PERIODIC_PHR_TIMER_ENUM;
static const char liblte_rrc_periodic_phr_timer_text[LIBLTE_RRC_PERIODIC_PHR_TIMER_N_ITEMS][20] = {    "sf10",     "sf20",     "sf50",    "sf100",
                                                                                                      "sf200",    "sf500",   "sf1000", "INFINITY"};
static int liblte_rrc_periodic_phr_timer_num[LIBLTE_RRC_PERIODIC_PHR_TIMER_N_ITEMS] = {10, 20, 50, 100, 200, 500, 1000, -1};
                                                                                                      
typedef enum{
    LIBLTE_RRC_PROHIBIT_PHR_TIMER_SF0 = 0,
    LIBLTE_RRC_PROHIBIT_PHR_TIMER_SF10,
    LIBLTE_RRC_PROHIBIT_PHR_TIMER_SF20,
    LIBLTE_RRC_PROHIBIT_PHR_TIMER_SF50,
    LIBLTE_RRC_PROHIBIT_PHR_TIMER_SF100,
    LIBLTE_RRC_PROHIBIT_PHR_TIMER_SF200,
    LIBLTE_RRC_PROHIBIT_PHR_TIMER_SF500,
    LIBLTE_RRC_PROHIBIT_PHR_TIMER_SF1000,
    LIBLTE_RRC_PROHIBIT_PHR_TIMER_N_ITEMS,
}LIBLTE_RRC_PROHIBIT_PHR_TIMER_ENUM;
static const char liblte_rrc_prohibit_phr_timer_text[LIBLTE_RRC_PROHIBIT_PHR_TIMER_N_ITEMS][20] = {   "sf0",   "sf10",   "sf20",   "sf50",
                                                                                                    "sf100",  "sf200",  "sf500", "sf1000"};
                                                                                                    
static int liblte_rrc_prohibit_phr_timer_num[LIBLTE_RRC_PROHIBIT_PHR_TIMER_N_ITEMS] = {0, 10, 20, 50, 100, 200, 500, 1000};

typedef enum{
    LIBLTE_RRC_DL_PATHLOSS_CHANGE_DB1 = 0,
    LIBLTE_RRC_DL_PATHLOSS_CHANGE_DB3,
    LIBLTE_RRC_DL_PATHLOSS_CHANGE_DB6,
    LIBLTE_RRC_DL_PATHLOSS_CHANGE_INFINITY,
    LIBLTE_RRC_DL_PATHLOSS_CHANGE_N_ITEMS,
}LIBLTE_RRC_DL_PATHLOSS_CHANGE_ENUM;
static const char liblte_rrc_dl_pathloss_change_text[LIBLTE_RRC_DL_PATHLOSS_CHANGE_N_ITEMS][20] = {"1dB", "3dB", "6dB", "INFINITY"};

static int liblte_rrc_dl_pathloss_change_num[LIBLTE_RRC_DL_PATHLOSS_CHANGE_N_ITEMS] = {1, 3, 6, -1};

// Structs
typedef struct{
    LIBLTE_RRC_MAX_HARQ_TX_ENUM              max_harq_tx;
    LIBLTE_RRC_PERIODIC_BSR_TIMER_ENUM       periodic_bsr_timer;
    LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_ENUM retx_bsr_timer;
    bool                                     tti_bundling;
    bool                                     max_harq_tx_present;
    bool                                     periodic_bsr_timer_present;
}LIBLTE_RRC_ULSCH_CONFIG_STRUCT;
typedef struct{
    LIBLTE_RRC_ON_DURATION_TIMER_ENUM                  on_duration_timer;
    LIBLTE_RRC_DRX_INACTIVITY_TIMER_ENUM               drx_inactivity_timer;
    LIBLTE_RRC_DRX_RETRANSMISSION_TIMER_ENUM           drx_retx_timer;
    LIBLTE_RRC_LONG_DRX_CYCLE_START_OFFSET_CHOICE_ENUM long_drx_cycle_start_offset_choice;
    LIBLTE_RRC_SHORT_DRX_CYCLE_ENUM                    short_drx_cycle;
    uint32                                             long_drx_cycle_start_offset;
    uint32                                             short_drx_cycle_timer;
    bool                                               setup_present;
    bool                                               short_drx_present;
}LIBLTE_RRC_DRX_CONFIG_STRUCT;
typedef struct{
    LIBLTE_RRC_PERIODIC_PHR_TIMER_ENUM periodic_phr_timer;
    LIBLTE_RRC_PROHIBIT_PHR_TIMER_ENUM prohibit_phr_timer;
    LIBLTE_RRC_DL_PATHLOSS_CHANGE_ENUM dl_pathloss_change;
    bool                               setup_present;
}LIBLTE_RRC_PHR_CONFIG_STRUCT;
typedef struct{
    LIBLTE_RRC_ULSCH_CONFIG_STRUCT       ulsch_cnfg;
    LIBLTE_RRC_DRX_CONFIG_STRUCT         drx_cnfg;
    LIBLTE_RRC_PHR_CONFIG_STRUCT         phr_cnfg;
    LIBLTE_RRC_TIME_ALIGNMENT_TIMER_ENUM time_alignment_timer;
    bool                                 ulsch_cnfg_present;
    bool                                 drx_cnfg_present;
    bool                                 phr_cnfg_present;
}LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_mac_main_config_ie(LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT  *mac_main_cnfg,
                                                     uint8                             **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mac_main_config_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT  *mac_main_cnfg);

/*********************************************************************
    IE Name: PDCP Config

    Description: Sets the configurable PDCP parameters for data
                 radio bearers

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_DISCARD_TIMER_MS50 = 0,
    LIBLTE_RRC_DISCARD_TIMER_MS100,
    LIBLTE_RRC_DISCARD_TIMER_MS150,
    LIBLTE_RRC_DISCARD_TIMER_MS300,
    LIBLTE_RRC_DISCARD_TIMER_MS500,
    LIBLTE_RRC_DISCARD_TIMER_MS750,
    LIBLTE_RRC_DISCARD_TIMER_MS1500,
    LIBLTE_RRC_DISCARD_TIMER_INFINITY,
    LIBLTE_RRC_DISCARD_TIMER_N_ITEMS,
}LIBLTE_RRC_DISCARD_TIMER_ENUM;
static const char liblte_rrc_discard_timer_text[LIBLTE_RRC_DISCARD_TIMER_N_ITEMS][20] = {    "ms50",    "ms100",    "ms150",    "ms300",
                                                                                            "ms500",    "ms750",   "ms1500", "INFINITY"};
static const int32 liblte_rrc_discard_timer_num[LIBLTE_RRC_DISCARD_TIMER_N_ITEMS] = { 50, 100, 150, 300, 500, 750, 1500, -1};                                                                                            
typedef enum{
    LIBLTE_RRC_PDCP_SN_SIZE_7_BITS = 0,
    LIBLTE_RRC_PDCP_SN_SIZE_12_BITS,
    LIBLTE_RRC_PDCP_SN_SIZE_N_ITEMS,
}LIBLTE_RRC_PDCP_SN_SIZE_ENUM;
static const char liblte_rrc_pdcp_sn_size_text[LIBLTE_RRC_PDCP_SN_SIZE_N_ITEMS][20] = {"7-bits", "12-bits"};

static const int8 liblte_rrc_pdcp_sn_size_num[LIBLTE_RRC_PDCP_SN_SIZE_N_ITEMS] = {7, 12};

// Structs
typedef struct{
    LIBLTE_RRC_DISCARD_TIMER_ENUM discard_timer;
    LIBLTE_RRC_PDCP_SN_SIZE_ENUM  rlc_um_pdcp_sn_size;
    uint32                        hdr_compression_max_cid;
    bool                          hdr_compression_rohc;
    bool                          hdr_compression_profile_0001;
    bool                          hdr_compression_profile_0002;
    bool                          hdr_compression_profile_0003;
    bool                          hdr_compression_profile_0004;
    bool                          hdr_compression_profile_0006;
    bool                          hdr_compression_profile_0101;
    bool                          hdr_compression_profile_0102;
    bool                          hdr_compression_profile_0103;
    bool                          hdr_compression_profile_0104;
    bool                          discard_timer_present;
    bool                          rlc_am_status_report_required_present;
    bool                          rlc_am_status_report_required;
    bool                          rlc_um_pdcp_sn_size_present;
}LIBLTE_RRC_PDCP_CONFIG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_pdcp_config_ie(LIBLTE_RRC_PDCP_CONFIG_STRUCT  *pdcp_cnfg,
                                                 uint8                         **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pdcp_config_ie(uint8                         **ie_ptr,
                                                   LIBLTE_RRC_PDCP_CONFIG_STRUCT  *pdcp_cnfg);

/*********************************************************************
    IE Name: PDSCH Config

    Description: Specifies the common and the UE specific PDSCH
                 configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_PDSCH_CONFIG_P_A_DB_N6 = 0,
    LIBLTE_RRC_PDSCH_CONFIG_P_A_DB_N4_DOT_77,
    LIBLTE_RRC_PDSCH_CONFIG_P_A_DB_N3,
    LIBLTE_RRC_PDSCH_CONFIG_P_A_DB_N1_DOT_77,
    LIBLTE_RRC_PDSCH_CONFIG_P_A_DB_0,
    LIBLTE_RRC_PDSCH_CONFIG_P_A_DB_1,
    LIBLTE_RRC_PDSCH_CONFIG_P_A_DB_2,
    LIBLTE_RRC_PDSCH_CONFIG_P_A_DB_3,
    LIBLTE_RRC_PDSCH_CONFIG_P_A_N_ITEMS,
}LIBLTE_RRC_PDSCH_CONFIG_P_A_ENUM;
static const char liblte_rrc_pdsch_config_p_a_text[LIBLTE_RRC_PDSCH_CONFIG_P_A_N_ITEMS][20] = {   "-6", "-4.77",    "-3", "-1.77",
                                                                                                   "0",     "1",     "2",     "3"};
static const float liblte_rrc_pdsch_config_p_a_num[LIBLTE_RRC_PDSCH_CONFIG_P_A_N_ITEMS] = {-6, -4.77f, -3, -1.77f, 0, 1, 2, 3};
// Structs
// PDSCH Config Common struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_pdsch_config_common_ie(LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT  *pdsch_config,
                                                         uint8                                 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pdsch_config_common_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT  *pdsch_config);
LIBLTE_ERROR_ENUM liblte_rrc_pack_pdsch_config_dedicated_ie(LIBLTE_RRC_PDSCH_CONFIG_P_A_ENUM   p_a,
                                                            uint8                            **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pdsch_config_dedicated_ie(uint8                            **ie_ptr,
                                                              LIBLTE_RRC_PDSCH_CONFIG_P_A_ENUM  *p_a);

/*********************************************************************
    IE Name: PHICH Config

    Description: Specifies the PHICH configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// PHICH Config enums defined above
// Structs
// PHICH Config struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_phich_config_ie(LIBLTE_RRC_PHICH_CONFIG_STRUCT  *phich_config,
                                                  uint8                          **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_phich_config_ie(uint8                          **ie_ptr,
                                                    LIBLTE_RRC_PHICH_CONFIG_STRUCT  *phich_config);

/*********************************************************************
    IE Name: Physical Config Dedicated

    Description: Specifies the UE specific physical channel
                 configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_ACK_NACK_REPETITION_FACTOR_N2 = 0,
    LIBLTE_RRC_ACK_NACK_REPETITION_FACTOR_N4,
    LIBLTE_RRC_ACK_NACK_REPETITION_FACTOR_N6,
    LIBLTE_RRC_ACK_NACK_REPETITION_FACTOR_SPARE1,
    LIBLTE_RRC_ACK_NACK_REPETITION_FACTOR_N_ITEMS,
}LIBLTE_RRC_ACK_NACK_REPETITION_FACTOR_ENUM;
static const char liblte_rrc_ack_nack_repetition_factor_text[LIBLTE_RRC_ACK_NACK_REPETITION_FACTOR_N_ITEMS][20] = {"n2", "n4", "n6", "SPARE"};
typedef enum{
    LIBLTE_RRC_TDD_ACK_NACK_FEEDBACK_MODE_BUNDLING = 0,
    LIBLTE_RRC_TDD_ACK_NACK_FEEDBACK_MODE_MULTIPLEXING,
    LIBLTE_RRC_TDD_ACK_NACK_FEEDBACK_MODE_N_ITEMS,
}LIBLTE_RRC_TDD_ACK_NACK_FEEDBACK_MODE_ENUM;
static const char liblte_rrc_tdd_ack_nack_feedback_mode_text[LIBLTE_RRC_TDD_ACK_NACK_FEEDBACK_MODE_N_ITEMS][20] = {"bundling", "multiplexing"};
typedef enum{
    LIBLTE_RRC_DSR_TRANS_MAX_N4 = 0,
    LIBLTE_RRC_DSR_TRANS_MAX_N8,
    LIBLTE_RRC_DSR_TRANS_MAX_N16,
    LIBLTE_RRC_DSR_TRANS_MAX_N32,
    LIBLTE_RRC_DSR_TRANS_MAX_N64,
    LIBLTE_RRC_DSR_TRANS_MAX_SPARE3,
    LIBLTE_RRC_DSR_TRANS_MAX_SPARE2,
    LIBLTE_RRC_DSR_TRANS_MAX_SPARE1,
    LIBLTE_RRC_DSR_TRANS_MAX_N_ITEMS,
}LIBLTE_RRC_DSR_TRANS_MAX_ENUM;
static const char liblte_rrc_dsr_trans_max_text[LIBLTE_RRC_DSR_TRANS_MAX_N_ITEMS][20] = {   "n4",    "n8",   "n16",   "n32",
                                                                                           "n64", "SPARE", "SPARE", "SPARE"};
static const int32 liblte_rrc_dsr_trans_max_num[LIBLTE_RRC_DSR_TRANS_MAX_N_ITEMS] =  {4, 8, 16, 32, 64, -1, -1, -1};

typedef enum{
    LIBLTE_RRC_DELTA_MCS_ENABLED_EN0 = 0,
    LIBLTE_RRC_DELTA_MCS_ENABLED_EN1,
    LIBLTE_RRC_DELTA_MCS_ENABLED_N_ITEMS,
}LIBLTE_RRC_DELTA_MCS_ENABLED_ENUM;
static const char liblte_rrc_delta_mcs_enabled_text[LIBLTE_RRC_DELTA_MCS_ENABLED_N_ITEMS][20] = {"en0", "en1"};
typedef enum{
    LIBLTE_RRC_TPC_INDEX_FORMAT_3 = 0,
    LIBLTE_RRC_TPC_INDEX_FORMAT_3A,
    LIBLTE_RRC_TPC_INDEX_N_ITEMS,
}LIBLTE_RRC_TPC_INDEX_ENUM;
static const char liblte_rrc_tpc_index_text[LIBLTE_RRC_TPC_INDEX_N_ITEMS][20] = {"format_3", "format_3a"};
typedef enum{
    LIBLTE_RRC_SRS_BANDWIDTH_BW0 = 0,
    LIBLTE_RRC_SRS_BANDWIDTH_BW1,
    LIBLTE_RRC_SRS_BANDWIDTH_BW2,
    LIBLTE_RRC_SRS_BANDWIDTH_BW3,
    LIBLTE_RRC_SRS_BANDWIDTH_N_ITEMS,
}LIBLTE_RRC_SRS_BANDWIDTH_ENUM;
static const char liblte_rrc_srs_bandwidth_text[LIBLTE_RRC_SRS_BANDWIDTH_N_ITEMS][20] = {"bw0", "bw1", "bw2", "bw3"};
typedef enum{
    LIBLTE_RRC_SRS_HOPPING_BANDWIDTH_HBW0 = 0,
    LIBLTE_RRC_SRS_HOPPING_BANDWIDTH_HBW1,
    LIBLTE_RRC_SRS_HOPPING_BANDWIDTH_HBW2,
    LIBLTE_RRC_SRS_HOPPING_BANDWIDTH_HBW3,
    LIBLTE_RRC_SRS_HOPPING_BANDWIDTH_N_ITEMS,
}LIBLTE_RRC_SRS_HOPPING_BANDWIDTH_ENUM;
static const char liblte_rrc_srs_hopping_bandwidth_text[LIBLTE_RRC_SRS_HOPPING_BANDWIDTH_N_ITEMS][20] = {"hbw0", "hbw1", "hbw2", "hbw3"};
typedef enum{
    LIBLTE_RRC_CYCLIC_SHIFT_CS0 = 0,
    LIBLTE_RRC_CYCLIC_SHIFT_CS1,
    LIBLTE_RRC_CYCLIC_SHIFT_CS2,
    LIBLTE_RRC_CYCLIC_SHIFT_CS3,
    LIBLTE_RRC_CYCLIC_SHIFT_CS4,
    LIBLTE_RRC_CYCLIC_SHIFT_CS5,
    LIBLTE_RRC_CYCLIC_SHIFT_CS6,
    LIBLTE_RRC_CYCLIC_SHIFT_CS7,
    LIBLTE_RRC_CYCLIC_SHIFT_N_ITEMS,
}LIBLTE_RRC_CYCLIC_SHIFT_ENUM;
static const char liblte_rrc_cyclic_shift_text[LIBLTE_RRC_CYCLIC_SHIFT_N_ITEMS][20] = {"cs0", "cs1", "cs2", "cs3",
                                                                                       "cs4", "cs5", "cs6", "cs7"};
// Structs
typedef struct{
    LIBLTE_RRC_ACK_NACK_REPETITION_FACTOR_ENUM ack_nack_repetition_factor;
    LIBLTE_RRC_TDD_ACK_NACK_FEEDBACK_MODE_ENUM tdd_ack_nack_feedback_mode;
    uint32                                     ack_nack_repetition_n1_pucch_an;
    bool                                       tdd_ack_nack_feedback_mode_present;
    bool                                       ack_nack_repetition_setup_present;
}LIBLTE_RRC_PUCCH_CONFIG_DEDICATED_STRUCT;
typedef struct{
    uint8 beta_offset_ack_idx;
    uint8 beta_offset_ri_idx;
    uint8 beta_offset_cqi_idx;
}LIBLTE_RRC_PUSCH_CONFIG_DEDICATED_STRUCT;
typedef struct{
    LIBLTE_RRC_DELTA_MCS_ENABLED_ENUM  delta_mcs_en;
    LIBLTE_RRC_FILTER_COEFFICIENT_ENUM filter_coeff;
    uint32                             p_srs_offset;
    int32                              p0_ue_pusch;
    int32                              p0_ue_pucch;
    bool                               accumulation_en;
    bool                               filter_coeff_present;
}LIBLTE_RRC_UL_POWER_CONTROL_DEDICATED_STRUCT;
typedef struct{
    LIBLTE_RRC_TPC_INDEX_ENUM tpc_idx_choice;
    uint32                    tpc_rnti;
    uint32                    tpc_idx;
    bool                      setup_present;
}LIBLTE_RRC_TPC_PDCCH_CONFIG_STRUCT;
typedef struct{
    LIBLTE_RRC_SRS_BANDWIDTH_ENUM         srs_bandwidth;
    LIBLTE_RRC_SRS_HOPPING_BANDWIDTH_ENUM srs_hopping_bandwidth;
    LIBLTE_RRC_CYCLIC_SHIFT_ENUM          cyclic_shift;
    uint32                                freq_domain_pos;
    uint32                                srs_cnfg_idx;
    uint32                                tx_comb;
    bool                                  setup_present;
    bool                                  duration;
}LIBLTE_RRC_SRS_UL_CONFIG_DEDICATED_STRUCT;
typedef struct{
    LIBLTE_RRC_DSR_TRANS_MAX_ENUM dsr_trans_max;
    uint32                        sr_pucch_resource_idx;
    uint32                        sr_cnfg_idx;
    bool                          setup_present;
}LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT;
typedef struct{
    LIBLTE_RRC_PUCCH_CONFIG_DEDICATED_STRUCT     pucch_cnfg_ded;
    LIBLTE_RRC_PUSCH_CONFIG_DEDICATED_STRUCT     pusch_cnfg_ded;
    LIBLTE_RRC_UL_POWER_CONTROL_DEDICATED_STRUCT ul_pwr_ctrl_ded;
    LIBLTE_RRC_TPC_PDCCH_CONFIG_STRUCT           tpc_pdcch_cnfg_pucch;
    LIBLTE_RRC_TPC_PDCCH_CONFIG_STRUCT           tpc_pdcch_cnfg_pusch;
    LIBLTE_RRC_CQI_REPORT_CONFIG_STRUCT          cqi_report_cnfg;
    LIBLTE_RRC_SRS_UL_CONFIG_DEDICATED_STRUCT    srs_ul_cnfg_ded;
    LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT     antenna_info_explicit_value;
    LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT  sched_request_cnfg;
    LIBLTE_RRC_PDSCH_CONFIG_P_A_ENUM             pdsch_cnfg_ded;
    bool                                         pdsch_cnfg_ded_present;
    bool                                         pucch_cnfg_ded_present;
    bool                                         pusch_cnfg_ded_present;
    bool                                         ul_pwr_ctrl_ded_present;
    bool                                         tpc_pdcch_cnfg_pucch_present;
    bool                                         tpc_pdcch_cnfg_pusch_present;
    bool                                         cqi_report_cnfg_present;
    bool                                         srs_ul_cnfg_ded_present;
    bool                                         antenna_info_present;
    bool                                         antenna_info_default_value;
    bool                                         sched_request_cnfg_present;
}LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_physical_config_dedicated_ie(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT  *phy_cnfg_ded,
                                                               uint8                                       **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_physical_config_dedicated_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT  *phy_cnfg_ded);

/*********************************************************************
    IE Name: P Max

    Description: Limits the UE's uplink transmission power on a
                 carrier frequency and is used to calculate the
                 parameter P Compensation

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_p_max_ie(int8    p_max,
                                           uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_p_max_ie(uint8 **ie_ptr,
                                             int8   *p_max);

/*********************************************************************
    IE Name: PRACH Config

    Description: Specifies the PRACH configuration in the system
                 information and in the mobility control information

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// PRACH Config structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_prach_config_sib_ie(LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT  *prach_cnfg,
                                                      uint8                              **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_prach_config_sib_ie(uint8                              **ie_ptr,
                                                        LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT  *prach_cnfg);
LIBLTE_ERROR_ENUM liblte_rrc_pack_prach_config_ie(LIBLTE_RRC_PRACH_CONFIG_STRUCT  *prach_cnfg,
                                                  uint8                          **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_prach_config_ie(uint8                          **ie_ptr,
                                                    LIBLTE_RRC_PRACH_CONFIG_STRUCT  *prach_cnfg);
LIBLTE_ERROR_ENUM liblte_rrc_pack_prach_config_scell_r10_ie(uint8   prach_cnfg_idx,
                                                            uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_prach_config_scell_r10_ie(uint8 **ie_ptr,
                                                              uint8  *prach_cnfg_idx);

/*********************************************************************
    IE Name: Presence Antenna Port 1

    Description: Indicates whether all the neighboring cells use
                 antenna port 1

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_presence_antenna_port_1_ie(bool    presence_ant_port_1,
                                                             uint8 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_presence_antenna_port_1_ie(uint8 **ie_ptr,
                                                               bool   *presence_ant_port_1);

/*********************************************************************
    IE Name: PUCCH Config

    Description: Specifies the common and the UE specific PUCCH
                 configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// PUCCH Config enum defined above
// Structs
// PUCCH Config structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_pucch_config_common_ie(LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT  *pucch_cnfg,
                                                         uint8                                 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pucch_config_common_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT  *pucch_cnfg);
LIBLTE_ERROR_ENUM liblte_rrc_pack_pucch_config_dedicated_ie(LIBLTE_RRC_PUCCH_CONFIG_DEDICATED_STRUCT  *pucch_cnfg,
                                                            uint8                                    **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pucch_config_dedicated_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_RRC_PUCCH_CONFIG_DEDICATED_STRUCT  *pucch_cnfg);

/*********************************************************************
    IE Name: PUSCH Config

    Description: Specifies the common and the UE specific PUSCH
                 configuration and the reference signal configuration
                 for PUSCH and PUCCH

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// PUSCH Config enum defined above
// Structs
// PUSCH Config structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_pusch_config_common_ie(LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT  *pusch_cnfg,
                                                         uint8                                 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pusch_config_common_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT  *pusch_cnfg);
LIBLTE_ERROR_ENUM liblte_rrc_pack_pusch_config_dedicated_ie(LIBLTE_RRC_PUSCH_CONFIG_DEDICATED_STRUCT  *pusch_cnfg,
                                                            uint8                                    **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pusch_config_dedicated_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_RRC_PUSCH_CONFIG_DEDICATED_STRUCT  *pusch_cnfg);

/*********************************************************************
    IE Name: RACH Config Common

    Description: Specifies the generic random access parameters

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// RACH Config Common enums defined above
// Structs
// RACH Config Common structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rach_config_common_ie(LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT  *rach_cnfg,
                                                        uint8                                **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rach_config_common_ie(uint8                                **ie_ptr,
                                                          LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT  *rach_cnfg);

/*********************************************************************
    IE Name: RACH Config Dedicated

    Description: Specifies the dedicated random access parameters

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// RACH Config Dedicated struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rach_config_dedicated_ie(LIBLTE_RRC_RACH_CONFIG_DEDICATED_STRUCT  *rach_cnfg,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rach_config_dedicated_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_RACH_CONFIG_DEDICATED_STRUCT  *rach_cnfg);

/*********************************************************************
    IE Name: Radio Resource Config Common

    Description: Specifies the common radio resource configurations
                 in the system information and in the mobility control
                 information, including random access parameters
                 and static physical layer parameters

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_N2 = 0,
    LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_N4,
    LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_N8,
    LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_N16,
    LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_N_ITEMS,
}LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_ENUM;
static const char liblte_rrc_modification_period_coeff_text[LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_N_ITEMS][20] = {"2", "4", "8", "16"};
static const uint8 liblte_rrc_modification_period_coeff_num[LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_N_ITEMS] = {2, 4, 8, 16};
typedef enum{
    LIBLTE_RRC_DEFAULT_PAGING_CYCLE_RF32 = 0,
    LIBLTE_RRC_DEFAULT_PAGING_CYCLE_RF64,
    LIBLTE_RRC_DEFAULT_PAGING_CYCLE_RF128,
    LIBLTE_RRC_DEFAULT_PAGING_CYCLE_RF256,
    LIBLTE_RRC_DEFAULT_PAGING_CYCLE_N_ITEMS,
}LIBLTE_RRC_DEFAULT_PAGING_CYCLE_ENUM;
static const char liblte_rrc_default_paging_cycle_text[LIBLTE_RRC_DEFAULT_PAGING_CYCLE_N_ITEMS][20] = {"32", "64", "128", "256"};
static const uint16 liblte_rrc_default_paging_cycle_num[LIBLTE_RRC_DEFAULT_PAGING_CYCLE_N_ITEMS] = {32, 64, 128, 256};
typedef enum{
    LIBLTE_RRC_NB_FOUR_T = 0,
    LIBLTE_RRC_NB_TWO_T,
    LIBLTE_RRC_NB_ONE_T,
    LIBLTE_RRC_NB_HALF_T,
    LIBLTE_RRC_NB_QUARTER_T,
    LIBLTE_RRC_NB_ONE_EIGHTH_T,
    LIBLTE_RRC_NB_ONE_SIXTEENTH_T,
    LIBLTE_RRC_NB_ONE_THIRTY_SECOND_T,
    LIBLTE_RRC_NB_N_ITEMS,
}LIBLTE_RRC_NB_ENUM;
static const char liblte_rrc_nb_text[LIBLTE_RRC_NB_N_ITEMS][20] = {   "4",    "2",    "1",  "1/2",
                                                                    "1/4",  "1/8", "1/16", "1/32"};
static const double liblte_rrc_nb_num[LIBLTE_RRC_NB_N_ITEMS] = {4.0, 2.0, 1.0, 0.5, 0.25, 0.125, 0.0625, 0.03125};
// Structs
typedef struct{
    LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_ENUM modification_period_coeff;
}LIBLTE_RRC_BCCH_CONFIG_STRUCT;
typedef struct{
    LIBLTE_RRC_DEFAULT_PAGING_CYCLE_ENUM default_paging_cycle;
    LIBLTE_RRC_NB_ENUM                   nB;
}LIBLTE_RRC_PCCH_CONFIG_STRUCT;
typedef struct{
    LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT      rach_cnfg;
    LIBLTE_RRC_BCCH_CONFIG_STRUCT             bcch_cnfg;
    LIBLTE_RRC_PCCH_CONFIG_STRUCT             pcch_cnfg;
    LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT        prach_cnfg;
    LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT     pdsch_cnfg;
    LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT     pusch_cnfg;
    LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT     pucch_cnfg;
    LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT    srs_ul_cnfg;
    LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT ul_pwr_ctrl;
    LIBLTE_RRC_UL_CP_LENGTH_ENUM              ul_cp_length;
}LIBLTE_RRC_RR_CONFIG_COMMON_SIB_STRUCT;
// RR Config Common struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rr_config_common_sib_ie(LIBLTE_RRC_RR_CONFIG_COMMON_SIB_STRUCT  *rr_cnfg,
                                                          uint8                                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rr_config_common_sib_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_RRC_RR_CONFIG_COMMON_SIB_STRUCT  *rr_cnfg);
LIBLTE_ERROR_ENUM liblte_rrc_pack_rr_config_common_ie(LIBLTE_RRC_RR_CONFIG_COMMON_STRUCT  *rr_cnfg,
                                                      uint8                              **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rr_config_common_ie(uint8                              **ie_ptr,
                                                        LIBLTE_RRC_RR_CONFIG_COMMON_STRUCT  *rr_cnfg);

/*********************************************************************
    IE Name: Radio Resource Config Dedicated

    Description: Sets up/Modifies/Releases RBs, modifies the MAC
                 main configuration, modifies the SPS configuration
                 and modifies dedicated physical configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_DRB 11
// Enums
typedef enum{
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS5 = 0,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS10,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS15,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS20,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS25,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS30,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS35,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS40,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS45,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS50,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS55,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS60,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS65,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS70,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS75,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS80,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS85,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS90,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS95,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS100,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS105,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS110,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS115,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS120,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS125,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS130,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS135,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS140,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS145,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS150,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS155,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS160,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS165,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS170,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS175,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS180,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS185,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS190,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS195,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS200,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS205,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS210,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS215,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS220,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS225,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS230,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS235,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS240,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS245,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS250,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS300,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS350,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS400,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS450,
    LIBLTE_RRC_T_POLL_RETRANSMIT_MS500,
    LIBLTE_RRC_T_POLL_RETRANSMIT_SPARE9,
    LIBLTE_RRC_T_POLL_RETRANSMIT_SPARE8,
    LIBLTE_RRC_T_POLL_RETRANSMIT_SPARE7,
    LIBLTE_RRC_T_POLL_RETRANSMIT_SPARE6,
    LIBLTE_RRC_T_POLL_RETRANSMIT_SPARE5,
    LIBLTE_RRC_T_POLL_RETRANSMIT_SPARE4,
    LIBLTE_RRC_T_POLL_RETRANSMIT_SPARE3,
    LIBLTE_RRC_T_POLL_RETRANSMIT_SPARE2,
    LIBLTE_RRC_T_POLL_RETRANSMIT_SPARE1,
    LIBLTE_RRC_T_POLL_RETRANSMIT_N_ITEMS,
}LIBLTE_RRC_T_POLL_RETRANSMIT_ENUM;
static const char liblte_rrc_t_poll_retransmit_text[LIBLTE_RRC_T_POLL_RETRANSMIT_N_ITEMS][20] = {  "5ms",  "10ms",  "15ms",  "20ms",
                                                                                                  "25ms",  "30ms",  "35ms",  "40ms",
                                                                                                  "45ms",  "50ms",  "55ms",  "60ms",
                                                                                                  "65ms",  "70ms",  "75ms",  "80ms",
                                                                                                  "85ms",  "90ms",  "95ms", "100ms",
                                                                                                 "105ms", "110ms", "115ms", "120ms",
                                                                                                 "125ms", "130ms", "135ms", "140ms",
                                                                                                 "145ms", "150ms", "155ms", "160ms",
                                                                                                 "165ms", "170ms", "175ms", "180ms",
                                                                                                 "185ms", "190ms", "195ms", "200ms",
                                                                                                 "205ms", "210ms", "215ms", "220ms",
                                                                                                 "225ms", "230ms", "235ms", "240ms",
                                                                                                 "245ms", "250ms", "300ms", "350ms",
                                                                                                 "400ms", "450ms", "500ms", "SPARE",
                                                                                                 "SPARE", "SPARE", "SPARE", "SPARE",
                                                                                                 "SPARE", "SPARE", "SPARE", "SPARE"};
static const int32 liblte_rrc_t_poll_retransmit_num[LIBLTE_RRC_T_POLL_RETRANSMIT_N_ITEMS] = {    5,  10,  15,  20,
                                                                                                25,  30,  35,  40,
                                                                                                45,  50,  55,  60,
                                                                                                65,  70,  75,  80,
                                                                                                85,  90,  95, 100,
                                                                                               105, 110, 115, 120,
                                                                                               125, 130, 135, 140,
                                                                                               145, 150, 155, 160,
                                                                                               165, 170, 175, 180,
                                                                                               185, 190, 195, 200,
                                                                                               205, 210, 215, 220,
                                                                                               225, 230, 235, 240,
                                                                                               245, 250, 300, 350,
                                                                                               400, 450, 500, -1,
                                                                                               -1, -1, -1, -1,
                                                                                               -1, -1, -1, -1};
typedef enum{
    LIBLTE_RRC_POLL_PDU_P4 = 0,
    LIBLTE_RRC_POLL_PDU_P8,
    LIBLTE_RRC_POLL_PDU_P16,
    LIBLTE_RRC_POLL_PDU_P32,
    LIBLTE_RRC_POLL_PDU_P64,
    LIBLTE_RRC_POLL_PDU_P128,
    LIBLTE_RRC_POLL_PDU_P256,
    LIBLTE_RRC_POLL_PDU_INFINITY,
    LIBLTE_RRC_POLL_PDU_N_ITEMS,
}LIBLTE_RRC_POLL_PDU_ENUM;
static const char liblte_rrc_poll_pdu_text[LIBLTE_RRC_POLL_PDU_N_ITEMS][20] = {      "p4",       "p8",      "p16",      "p32",
                                                                                    "p64",     "p128",     "p256", "INFINITY"};
static const int32 liblte_rrc_poll_pdu_num[LIBLTE_RRC_POLL_PDU_N_ITEMS]     = {      4,       8,      16,      32,
                                                                                    64,     128,     256,      -1};
typedef enum{
    LIBLTE_RRC_POLL_BYTE_KB25 = 0,
    LIBLTE_RRC_POLL_BYTE_KB50,
    LIBLTE_RRC_POLL_BYTE_KB75,
    LIBLTE_RRC_POLL_BYTE_KB100,
    LIBLTE_RRC_POLL_BYTE_KB125,
    LIBLTE_RRC_POLL_BYTE_KB250,
    LIBLTE_RRC_POLL_BYTE_KB375,
    LIBLTE_RRC_POLL_BYTE_KB500,
    LIBLTE_RRC_POLL_BYTE_KB750,
    LIBLTE_RRC_POLL_BYTE_KB1000,
    LIBLTE_RRC_POLL_BYTE_KB1250,
    LIBLTE_RRC_POLL_BYTE_KB1500,
    LIBLTE_RRC_POLL_BYTE_KB2000,
    LIBLTE_RRC_POLL_BYTE_KB3000,
    LIBLTE_RRC_POLL_BYTE_INFINITY,
    LIBLTE_RRC_POLL_BYTE_SPARE1,
    LIBLTE_RRC_POLL_BYTE_N_ITEMS,
}LIBLTE_RRC_POLL_BYTE_ENUM;
static const char liblte_rrc_poll_byte_text[LIBLTE_RRC_POLL_BYTE_N_ITEMS][20] = {    "25kB",     "50kB",     "75kB",    "100kB",
                                                                                    "125kB",    "250kB",    "375kB",    "500kB",
                                                                                    "750kB",   "1000kB",   "1250kB",   "1500kB",
                                                                                   "2000kB",   "3000kB", "INFINITY",    "SPARE"};
static const int32 liblte_rrc_poll_byte_num[LIBLTE_RRC_POLL_BYTE_N_ITEMS]     = {    25,     50,     75,    100,
                                                                                    125,    250,    375,    500,
                                                                                    750,   1000,   1250,   1500,
                                                                                   2000,   3000,     -1,     -1};
typedef enum{
    LIBLTE_RRC_MAX_RETX_THRESHOLD_T1 = 0,
    LIBLTE_RRC_MAX_RETX_THRESHOLD_T2,
    LIBLTE_RRC_MAX_RETX_THRESHOLD_T3,
    LIBLTE_RRC_MAX_RETX_THRESHOLD_T4,
    LIBLTE_RRC_MAX_RETX_THRESHOLD_T6,
    LIBLTE_RRC_MAX_RETX_THRESHOLD_T8,
    LIBLTE_RRC_MAX_RETX_THRESHOLD_T16,
    LIBLTE_RRC_MAX_RETX_THRESHOLD_T32,
    LIBLTE_RRC_MAX_RETX_THRESHOLD_N_ITEMS,
}LIBLTE_RRC_MAX_RETX_THRESHOLD_ENUM;
static const char liblte_rrc_max_retx_threshold_text[LIBLTE_RRC_MAX_RETX_THRESHOLD_N_ITEMS][20] = { "t1",  "t2",  "t3",  "t4",
                                                                                                    "t6",  "t8", "t16", "t32"};
static const uint32_t liblte_rrc_max_retx_threshold_num[LIBLTE_RRC_MAX_RETX_THRESHOLD_N_ITEMS]     = { 1,  2,  3,  4,
                                                                                                    6,  8, 16, 32};
typedef enum{
    LIBLTE_RRC_T_REORDERING_MS0 = 0,
    LIBLTE_RRC_T_REORDERING_MS5,
    LIBLTE_RRC_T_REORDERING_MS10,
    LIBLTE_RRC_T_REORDERING_MS15,
    LIBLTE_RRC_T_REORDERING_MS20,
    LIBLTE_RRC_T_REORDERING_MS25,
    LIBLTE_RRC_T_REORDERING_MS30,
    LIBLTE_RRC_T_REORDERING_MS35,
    LIBLTE_RRC_T_REORDERING_MS40,
    LIBLTE_RRC_T_REORDERING_MS45,
    LIBLTE_RRC_T_REORDERING_MS50,
    LIBLTE_RRC_T_REORDERING_MS55,
    LIBLTE_RRC_T_REORDERING_MS60,
    LIBLTE_RRC_T_REORDERING_MS65,
    LIBLTE_RRC_T_REORDERING_MS70,
    LIBLTE_RRC_T_REORDERING_MS75,
    LIBLTE_RRC_T_REORDERING_MS80,
    LIBLTE_RRC_T_REORDERING_MS85,
    LIBLTE_RRC_T_REORDERING_MS90,
    LIBLTE_RRC_T_REORDERING_MS95,
    LIBLTE_RRC_T_REORDERING_MS100,
    LIBLTE_RRC_T_REORDERING_MS110,
    LIBLTE_RRC_T_REORDERING_MS120,
    LIBLTE_RRC_T_REORDERING_MS130,
    LIBLTE_RRC_T_REORDERING_MS140,
    LIBLTE_RRC_T_REORDERING_MS150,
    LIBLTE_RRC_T_REORDERING_MS160,
    LIBLTE_RRC_T_REORDERING_MS170,
    LIBLTE_RRC_T_REORDERING_MS180,
    LIBLTE_RRC_T_REORDERING_MS190,
    LIBLTE_RRC_T_REORDERING_MS200,
    LIBLTE_RRC_T_REORDERING_SPARE1,
    LIBLTE_RRC_T_REORDERING_N_ITEMS,
}LIBLTE_RRC_T_REORDERING_ENUM;
static const char liblte_rrc_t_reordering_text[LIBLTE_RRC_T_REORDERING_N_ITEMS][20] = {  "ms0",   "ms5",  "ms10",  "ms15",
                                                                                        "ms20",  "ms25",  "ms30",  "ms35",
                                                                                        "ms40",  "ms45",  "ms50",  "ms55",
                                                                                        "ms60",  "ms65",  "ms70",  "ms75",
                                                                                        "ms80",  "ms85",  "ms90",  "ms95",
                                                                                       "ms100", "ms110", "ms120", "ms130",
                                                                                       "ms140", "ms150", "ms160", "ms170",
                                                                                       "ms180", "ms190", "ms200", "SPARE"};
static const int32 liblte_rrc_t_reordering_num[LIBLTE_RRC_T_REORDERING_N_ITEMS]     = {  0,   5,  10,  15,
                                                                                        20,  25,  30,  35,
                                                                                        40,  45,  50,  55,
                                                                                        60,  65,  70,  75,
                                                                                        80,  85,  90,  95,
                                                                                       100, 110, 120, 130,
                                                                                       140, 150, 160, 170,
                                                                                       180, 190, 200,  -1};
typedef enum{
    LIBLTE_RRC_RLC_MODE_AM = 0,
    LIBLTE_RRC_RLC_MODE_UM_BI,
    LIBLTE_RRC_RLC_MODE_UM_UNI_UL,
    LIBLTE_RRC_RLC_MODE_UM_UNI_DL,
    LIBLTE_RRC_RLC_MODE_N_ITEMS,
}LIBLTE_RRC_RLC_MODE_ENUM;
static const char liblte_rrc_rlc_mode_text[LIBLTE_RRC_RLC_MODE_N_ITEMS][20] = {"AM",
                                                                               "UM BI",
                                                                               "UM UNI UL",
                                                                               "UM UNI DL"};
typedef enum{
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS0 = 0,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS5,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS10,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS15,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS20,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS25,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS30,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS35,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS40,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS45,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS50,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS55,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS60,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS65,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS70,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS75,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS80,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS85,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS90,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS95,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS100,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS105,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS110,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS115,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS120,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS125,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS130,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS135,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS140,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS145,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS150,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS155,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS160,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS165,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS170,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS175,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS180,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS185,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS190,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS195,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS200,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS205,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS210,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS215,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS220,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS225,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS230,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS235,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS240,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS245,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS250,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS300,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS350,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS400,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS450,
    LIBLTE_RRC_T_STATUS_PROHIBIT_MS500,
    LIBLTE_RRC_T_STATUS_PROHIBIT_SPARE8,
    LIBLTE_RRC_T_STATUS_PROHIBIT_SPARE7,
    LIBLTE_RRC_T_STATUS_PROHIBIT_SPARE6,
    LIBLTE_RRC_T_STATUS_PROHIBIT_SPARE5,
    LIBLTE_RRC_T_STATUS_PROHIBIT_SPARE4,
    LIBLTE_RRC_T_STATUS_PROHIBIT_SPARE3,
    LIBLTE_RRC_T_STATUS_PROHIBIT_SPARE2,
    LIBLTE_RRC_T_STATUS_PROHIBIT_SPARE1,
    LIBLTE_RRC_T_STATUS_PROHIBIT_N_ITEMS,
}LIBLTE_RRC_T_STATUS_PROHIBIT_ENUM;
static const char liblte_rrc_t_status_prohibit_text[LIBLTE_RRC_T_STATUS_PROHIBIT_N_ITEMS][20] = {  "ms0",   "ms5",  "ms10",  "ms15",
                                                                                                  "ms20",  "ms25",  "ms30",  "ms35",
                                                                                                  "ms40",  "ms45",  "ms50",  "ms55",
                                                                                                  "ms60",  "ms65",  "ms70",  "ms75",
                                                                                                  "ms80",  "ms85",  "ms90",  "ms95",
                                                                                                 "ms100", "ms105", "ms110", "ms115",
                                                                                                 "ms120", "ms125", "ms130", "ms135",
                                                                                                 "ms140", "ms145", "ms150", "ms155",
                                                                                                 "ms160", "ms165", "ms170", "ms175",
                                                                                                 "ms180", "ms185", "ms190", "ms195",
                                                                                                 "ms200", "ms205", "ms210", "ms215",
                                                                                                 "ms220", "ms225", "ms230", "ms235",
                                                                                                 "ms240", "ms245", "ms250", "ms300",
                                                                                                 "ms350", "ms400", "ms450", "ms500",
                                                                                                 "SPARE", "SPARE", "SPARE", "SPARE",
                                                                                                 "SPARE", "SPARE", "SPARE", "SPARE"};
static const int32 liblte_rrc_t_status_prohibit_num[LIBLTE_RRC_T_STATUS_PROHIBIT_N_ITEMS]     = {  0,   5,  10,  15,
                                                                                                  20,  25,  30,  35,
                                                                                                  40,  45,  50,  55,
                                                                                                  60,  65,  70,  75,
                                                                                                  80,  85,  90,  95,
                                                                                                 100, 105, 110, 115,
                                                                                                 120, 125, 130, 135,
                                                                                                 140, 145, 150, 155,
                                                                                                 160, 165, 170, 175,
                                                                                                 180, 185, 190, 195,
                                                                                                 200, 205, 210, 215,
                                                                                                 220, 225, 230, 235,
                                                                                                 240, 245, 250, 300,
                                                                                                 350, 400, 450, 500,
                                                                                                 -1,   -1,  -1,  -1,
                                                                                                 -1,   -1,  -1,  -1};
typedef enum{
    LIBLTE_RRC_SN_FIELD_LENGTH_SIZE5 = 0,
    LIBLTE_RRC_SN_FIELD_LENGTH_SIZE10,
    LIBLTE_RRC_SN_FIELD_LENGTH_N_ITEMS,
}LIBLTE_RRC_SN_FIELD_LENGTH_ENUM;
static const char liblte_rrc_sn_field_length_text[LIBLTE_RRC_SN_FIELD_LENGTH_N_ITEMS][20] = {"size5", "size10"};
static const uint8 liblte_rrc_sn_field_length_num[LIBLTE_RRC_SN_FIELD_LENGTH_N_ITEMS] = {5, 10};
typedef enum{
    LIBLTE_RRC_SPS_INTERVAL_DL_SF10 = 0,
    LIBLTE_RRC_SPS_INTERVAL_DL_SF20,
    LIBLTE_RRC_SPS_INTERVAL_DL_SF32,
    LIBLTE_RRC_SPS_INTERVAL_DL_SF40,
    LIBLTE_RRC_SPS_INTERVAL_DL_SF64,
    LIBLTE_RRC_SPS_INTERVAL_DL_SF80,
    LIBLTE_RRC_SPS_INTERVAL_DL_SF128,
    LIBLTE_RRC_SPS_INTERVAL_DL_SF160,
    LIBLTE_RRC_SPS_INTERVAL_DL_SF320,
    LIBLTE_RRC_SPS_INTERVAL_DL_SF640,
    LIBLTE_RRC_SPS_INTERVAL_DL_SPARE6,
    LIBLTE_RRC_SPS_INTERVAL_DL_SPARE5,
    LIBLTE_RRC_SPS_INTERVAL_DL_SPARE4,
    LIBLTE_RRC_SPS_INTERVAL_DL_SPARE3,
    LIBLTE_RRC_SPS_INTERVAL_DL_SPARE2,
    LIBLTE_RRC_SPS_INTERVAL_DL_SPARE1,
    LIBLTE_RRC_SPS_INTERVAL_DL_N_ITEMS,
}LIBLTE_RRC_SPS_INTERVAL_DL_ENUM;
static const char liblte_rrc_sps_interval_dl_text[LIBLTE_RRC_SPS_INTERVAL_DL_N_ITEMS][20] = { "sf10",  "sf20",  "sf32",  "sf40",
                                                                                              "sf64",  "sf80", "sf128", "sf160",
                                                                                             "sf320", "sf640", "SPARE", "SPARE",
                                                                                             "SPARE", "SPARE", "SPARE", "SPARE"};
typedef enum{
    LIBLTE_RRC_SPS_INTERVAL_UL_SF10 = 0,
    LIBLTE_RRC_SPS_INTERVAL_UL_SF20,
    LIBLTE_RRC_SPS_INTERVAL_UL_SF32,
    LIBLTE_RRC_SPS_INTERVAL_UL_SF40,
    LIBLTE_RRC_SPS_INTERVAL_UL_SF64,
    LIBLTE_RRC_SPS_INTERVAL_UL_SF80,
    LIBLTE_RRC_SPS_INTERVAL_UL_SF128,
    LIBLTE_RRC_SPS_INTERVAL_UL_SF160,
    LIBLTE_RRC_SPS_INTERVAL_UL_SF320,
    LIBLTE_RRC_SPS_INTERVAL_UL_SF640,
    LIBLTE_RRC_SPS_INTERVAL_UL_SPARE6,
    LIBLTE_RRC_SPS_INTERVAL_UL_SPARE5,
    LIBLTE_RRC_SPS_INTERVAL_UL_SPARE4,
    LIBLTE_RRC_SPS_INTERVAL_UL_SPARE3,
    LIBLTE_RRC_SPS_INTERVAL_UL_SPARE2,
    LIBLTE_RRC_SPS_INTERVAL_UL_SPARE1,
    LIBLTE_RRC_SPS_INTERVAL_UL_N_ITEMS,
}LIBLTE_RRC_SPS_INTERVAL_UL_ENUM;
static const char liblte_rrc_sps_interval_ul_text[LIBLTE_RRC_SPS_INTERVAL_UL_N_ITEMS][20] = { "sf10",  "sf20",  "sf32",  "sf40",
                                                                                              "sf64",  "sf80", "sf128", "sf160",
                                                                                             "sf320", "sf640", "SPARE", "SPARE",
                                                                                             "SPARE", "SPARE", "SPARE", "SPARE"};
typedef enum{
    LIBLTE_RRC_IMPLICIT_RELEASE_AFTER_E2 = 0,
    LIBLTE_RRC_IMPLICIT_RELEASE_AFTER_E3,
    LIBLTE_RRC_IMPLICIT_RELEASE_AFTER_E4,
    LIBLTE_RRC_IMPLICIT_RELEASE_AFTER_E8,
    LIBLTE_RRC_IMPLICIT_RELEASE_AFTER_N_ITEMS,
}LIBLTE_RRC_IMPLICIT_RELEASE_AFTER_ENUM;
static const char liblte_rrc_implicit_release_after_text[LIBLTE_RRC_IMPLICIT_RELEASE_AFTER_N_ITEMS][20] = {"e2", "e3", "e4", "e8"};
typedef enum{
    LIBLTE_RRC_TWO_INTERVALS_CONFIG_TRUE = 0,
    LIBLTE_RRC_TWO_INTERVALS_CONFIG_N_ITEMS,
}LIBLTE_RRC_TWO_INTERVALS_CONFIG_ENUM;
static const char liblte_rrc_two_intervals_config_text[LIBLTE_RRC_TWO_INTERVALS_CONFIG_N_ITEMS][20] = {"TRUE"};
// Structs
typedef struct{
    LIBLTE_RRC_T_POLL_RETRANSMIT_ENUM  t_poll_retx;
    LIBLTE_RRC_POLL_PDU_ENUM           poll_pdu;
    LIBLTE_RRC_POLL_BYTE_ENUM          poll_byte;
    LIBLTE_RRC_MAX_RETX_THRESHOLD_ENUM max_retx_thresh;
}LIBLTE_RRC_UL_AM_RLC_STRUCT;
typedef struct{
    LIBLTE_RRC_T_REORDERING_ENUM      t_reordering;
    LIBLTE_RRC_T_STATUS_PROHIBIT_ENUM t_status_prohibit;
}LIBLTE_RRC_DL_AM_RLC_STRUCT;
typedef struct{
    LIBLTE_RRC_SN_FIELD_LENGTH_ENUM sn_field_len;
}LIBLTE_RRC_UL_UM_RLC_STRUCT;
typedef struct{
    LIBLTE_RRC_SN_FIELD_LENGTH_ENUM sn_field_len;
    LIBLTE_RRC_T_REORDERING_ENUM    t_reordering;
}LIBLTE_RRC_DL_UM_RLC_STRUCT;
typedef struct{
    LIBLTE_RRC_UL_AM_RLC_STRUCT ul_am_rlc;
    LIBLTE_RRC_DL_AM_RLC_STRUCT dl_am_rlc;
    LIBLTE_RRC_UL_UM_RLC_STRUCT ul_um_bi_rlc;
    LIBLTE_RRC_DL_UM_RLC_STRUCT dl_um_bi_rlc;
    LIBLTE_RRC_UL_UM_RLC_STRUCT ul_um_uni_rlc;
    LIBLTE_RRC_DL_UM_RLC_STRUCT dl_um_uni_rlc;
    LIBLTE_RRC_RLC_MODE_ENUM    rlc_mode;
}LIBLTE_RRC_RLC_CONFIG_STRUCT;
typedef struct{
    LIBLTE_RRC_RLC_CONFIG_STRUCT             rlc_explicit_cnfg;
    LIBLTE_RRC_LOGICAL_CHANNEL_CONFIG_STRUCT lc_explicit_cnfg;
    uint32                                   srb_id;
    bool                                     rlc_cnfg_present;
    bool                                     rlc_default_cnfg_present;
    bool                                     lc_cnfg_present;
    bool                                     lc_default_cnfg_present;
}LIBLTE_RRC_SRB_TO_ADD_MOD_STRUCT;
typedef struct{
    LIBLTE_RRC_PDCP_CONFIG_STRUCT            pdcp_cnfg;
    LIBLTE_RRC_RLC_CONFIG_STRUCT             rlc_cnfg;
    LIBLTE_RRC_LOGICAL_CHANNEL_CONFIG_STRUCT lc_cnfg;
    uint32                                   eps_bearer_id;
    uint32                                   lc_id;
    uint8                                    drb_id;
    bool                                     eps_bearer_id_present;
    bool                                     pdcp_cnfg_present;
    bool                                     rlc_cnfg_present;
    bool                                     lc_id_present;
    bool                                     lc_cnfg_present;
}LIBLTE_RRC_DRB_TO_ADD_MOD_STRUCT;
typedef struct{
    LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT explicit_value;
    bool                              default_value;
}LIBLTE_RRC_MAC_MAIN_CONFIG_CHOICE_STRUCT;
typedef struct{
    LIBLTE_RRC_SPS_INTERVAL_DL_ENUM sps_interval_dl;
    uint32                          n1_pucch_an_persistent_list[4];
    uint32                          n1_pucch_an_persistent_list_size;
    uint8                           N_sps_processes;
    bool                            setup_present;
}LIBLTE_RRC_SPS_CONFIG_DL_STRUCT;
typedef struct{
    LIBLTE_RRC_SPS_INTERVAL_UL_ENUM        sps_interval_ul;
    LIBLTE_RRC_IMPLICIT_RELEASE_AFTER_ENUM implicit_release_after;
    LIBLTE_RRC_TWO_INTERVALS_CONFIG_ENUM   two_intervals_cnfg;
    int32                                  p0_nominal_pusch;
    int32                                  p0_ue_pusch;
    bool                                   setup_present;
    bool                                   p0_persistent_present;
    bool                                   two_intervals_cnfg_present;
}LIBLTE_RRC_SPS_CONFIG_UL_STRUCT;
typedef struct{
    LIBLTE_RRC_SPS_CONFIG_DL_STRUCT sps_cnfg_dl;
    LIBLTE_RRC_SPS_CONFIG_UL_STRUCT sps_cnfg_ul;
    uint16                          sps_c_rnti;
    bool                            sps_c_rnti_present;
    bool                            sps_cnfg_dl_present;
    bool                            sps_cnfg_ul_present;
}LIBLTE_RRC_SPS_CONFIG_STRUCT;
typedef struct{
    LIBLTE_RRC_T301_ENUM t301;
    LIBLTE_RRC_T310_ENUM t310;
    LIBLTE_RRC_N310_ENUM n310;
    LIBLTE_RRC_T311_ENUM t311;
    LIBLTE_RRC_N311_ENUM n311;
}LIBLTE_RRC_RLF_TIMERS_AND_CONSTANTS_STRUCT;
typedef struct{
    LIBLTE_RRC_SRB_TO_ADD_MOD_STRUCT            srb_to_add_mod_list[2];
    LIBLTE_RRC_DRB_TO_ADD_MOD_STRUCT            drb_to_add_mod_list[LIBLTE_RRC_MAX_DRB];
    LIBLTE_RRC_MAC_MAIN_CONFIG_CHOICE_STRUCT    mac_main_cnfg;
    LIBLTE_RRC_SPS_CONFIG_STRUCT                sps_cnfg;
    LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT phy_cnfg_ded;
    LIBLTE_RRC_RLF_TIMERS_AND_CONSTANTS_STRUCT  rlf_timers_and_constants;
    uint32                                      srb_to_add_mod_list_size;
    uint32                                      drb_to_add_mod_list_size;
    uint32                                      drb_to_release_list_size;
    uint8                                       drb_to_release_list[LIBLTE_RRC_MAX_DRB];
    bool                                        mac_main_cnfg_present;
    bool                                        sps_cnfg_present;
    bool                                        phy_cnfg_ded_present;
    bool                                        rlf_timers_and_constants_present;
}LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rr_config_dedicated_ie(LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT  *rr_cnfg,
                                                         uint8                                 **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rr_config_dedicated_ie(uint8                                 **ie_ptr,
                                                           LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT  *rr_cnfg);

/*********************************************************************
    IE Name: RLC Config

    Description: Specifies the RLC configuration of SRBs and DRBs

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// RLC Config struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rlc_config_ie(LIBLTE_RRC_RLC_CONFIG_STRUCT  *rlc_cnfg,
                                                uint8                        **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rlc_config_ie(uint8                        **ie_ptr,
                                                  LIBLTE_RRC_RLC_CONFIG_STRUCT  *rlc_cnfg);

/*********************************************************************
    IE Name: RLF Timers and Constants

    Description: Contains UE specific timers and constants applicable
                 for UEs in RRC_CONNECTED

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// RLF Timers and Constants struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rlf_timers_and_constants_ie(LIBLTE_RRC_RLF_TIMERS_AND_CONSTANTS_STRUCT  *rlf_timers_and_constants,
                                                              uint8                                      **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rlf_timers_and_constants_ie(uint8                                      **ie_ptr,
                                                                LIBLTE_RRC_RLF_TIMERS_AND_CONSTANTS_STRUCT  *rlf_timers_and_constants);

/*********************************************************************
    IE Name: RN Subframe Config

    Description: Specifies the subframe configuration for an RN

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: Scheduling Request Config

    Description: Specifies the scheduling request related parameters

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// Scheduling Request Config struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_scheduling_request_config_ie(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT  *sched_request_cnfg,
                                                               uint8                                       **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_scheduling_request_config_ie(uint8                                       **ie_ptr,
                                                                 LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT  *sched_request_cnfg);

/*********************************************************************
    IE Name: Sounding RS UL Config

    Description: Specifies the uplink Sounding RS configuration for
                 periodic and aperiodic sounding

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Sounding RS UL Config enums defined above
// Structs
// Sounding RS UL Config struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_srs_ul_config_common_ie(LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT  *srs_ul_cnfg,
                                                          uint8                                  **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_srs_ul_config_common_ie(uint8                                  **ie_ptr,
                                                            LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT  *srs_ul_cnfg);
LIBLTE_ERROR_ENUM liblte_rrc_pack_srs_ul_config_dedicated_ie(LIBLTE_RRC_SRS_UL_CONFIG_DEDICATED_STRUCT  *srs_ul_cnfg,
                                                             uint8                                     **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_srs_ul_config_dedicated_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_RRC_SRS_UL_CONFIG_DEDICATED_STRUCT  *srs_ul_cnfg);

/*********************************************************************
    IE Name: SPS Config

    Description: Specifies the semi-persistent scheduling
                 configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// SPS Config struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_sps_config_ie(LIBLTE_RRC_SPS_CONFIG_STRUCT  *sps_cnfg,
                                                uint8                        **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sps_config_ie(uint8                        **ie_ptr,
                                                  LIBLTE_RRC_SPS_CONFIG_STRUCT  *sps_cnfg);

/*********************************************************************
    IE Name: TDD Config

    Description: Specifies the TDD specific physical channel
                 configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// TDD Config enums defined above
// Structs
// TDD Config struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_tdd_config_ie(LIBLTE_RRC_TDD_CONFIG_STRUCT  *tdd_cnfg,
                                                uint8                        **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_tdd_config_ie(uint8                        **ie_ptr,
                                                  LIBLTE_RRC_TDD_CONFIG_STRUCT  *tdd_cnfg);

/*********************************************************************
    IE Name: Time Alignment Timer

    Description: Controls how long the UE is considered uplink time
                 aligned

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Time Alignment Timer enum defined above
// Structs
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_time_alignment_timer_ie(LIBLTE_RRC_TIME_ALIGNMENT_TIMER_ENUM   time_alignment_timer,
                                                          uint8                                **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_time_alignment_timer_ie(uint8                                **ie_ptr,
                                                            LIBLTE_RRC_TIME_ALIGNMENT_TIMER_ENUM  *time_alignment_timer);

/*********************************************************************
    IE Name: TPC PDCCH Config

    Description: Specifies the RNTIs and indecies for PUCCH and PUSCH
                 power control

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Structs
// TPC PDCCH Config struct defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_tpc_pdcch_config_ie(LIBLTE_RRC_TPC_PDCCH_CONFIG_STRUCT  *tpc_pdcch_cnfg,
                                                      uint8                              **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_tpc_pdcch_config_ie(uint8                              **ie_ptr,
                                                        LIBLTE_RRC_TPC_PDCCH_CONFIG_STRUCT  *tpc_pdcch_cnfg);

/*********************************************************************
    IE Name: UL Antenna Info

    Description: Specifies the UL antenna configuration

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_TM1 = 0,
    LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_TM2,
    LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_SPARE6,
    LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_SPARE5,
    LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_SPARE4,
    LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_SPARE3,
    LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_SPARE2,
    LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_SPARE1,
    LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_N_ITEMS,
}LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_ENUM;
static const char liblte_rrc_ul_transmission_mode_r10_text[LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_N_ITEMS][20] = {  "TM1",   "TM2", "SPARE", "SPARE",
                                                                                                               "SPARE", "SPARE", "SPARE", "SPARE"};
// Structs
typedef struct{
    LIBLTE_RRC_UL_TRANSMISSION_MODE_R10_ENUM ul_tx_mode;
    bool                                     four_ant_port_activated;
}LIBLTE_RRC_UL_ANTENNA_INFO_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_antenna_info_ie(LIBLTE_RRC_UL_ANTENNA_INFO_STRUCT  *ul_ant_info,
                                                     uint8                             **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_antenna_info_ie(uint8                             **ie_ptr,
                                                       LIBLTE_RRC_UL_ANTENNA_INFO_STRUCT  *ul_ant_info);

/*********************************************************************
    IE Name: Uplink Power Control

    Description: Specifies the parameters for uplink power control in
                 the system information and in the dedicated
                 signalling

    Document Reference: 36.331 v10.0.0 Section 6.3.2
*********************************************************************/
// Defines
// Enums
// Uplink Power Control enums defined above
// Structs
// Uplink Power Control structs defined above
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_power_control_common_ie(LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT  *ul_pwr_ctrl,
                                                             uint8                                     **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_power_control_common_ie(uint8                                     **ie_ptr,
                                                               LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT  *ul_pwr_ctrl);
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_power_control_dedicated_ie(LIBLTE_RRC_UL_POWER_CONTROL_DEDICATED_STRUCT  *ul_pwr_ctrl,
                                                                uint8                                        **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_power_control_dedicated_ie(uint8                                        **ie_ptr,
                                                                  LIBLTE_RRC_UL_POWER_CONTROL_DEDICATED_STRUCT  *ul_pwr_ctrl);

/*********************************************************************
    IE Name: System Information Block Type 2

    Description: Contains radio resource configuration that is common
                 for all UEs

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_AC_BARRING_FACTOR_P00 = 0,
    LIBLTE_RRC_AC_BARRING_FACTOR_P05,
    LIBLTE_RRC_AC_BARRING_FACTOR_P10,
    LIBLTE_RRC_AC_BARRING_FACTOR_P15,
    LIBLTE_RRC_AC_BARRING_FACTOR_P20,
    LIBLTE_RRC_AC_BARRING_FACTOR_P25,
    LIBLTE_RRC_AC_BARRING_FACTOR_P30,
    LIBLTE_RRC_AC_BARRING_FACTOR_P40,
    LIBLTE_RRC_AC_BARRING_FACTOR_P50,
    LIBLTE_RRC_AC_BARRING_FACTOR_P60,
    LIBLTE_RRC_AC_BARRING_FACTOR_P70,
    LIBLTE_RRC_AC_BARRING_FACTOR_P75,
    LIBLTE_RRC_AC_BARRING_FACTOR_P80,
    LIBLTE_RRC_AC_BARRING_FACTOR_P85,
    LIBLTE_RRC_AC_BARRING_FACTOR_P90,
    LIBLTE_RRC_AC_BARRING_FACTOR_P95,
    LIBLTE_RRC_AC_BARRING_FACTOR_N_ITEMS,
}LIBLTE_RRC_AC_BARRING_FACTOR_ENUM;
static const char liblte_rrc_ac_barring_factor_text[LIBLTE_RRC_AC_BARRING_FACTOR_N_ITEMS][20] = {"0.00", "0.05", "0.10", "0.15",
                                                                                                 "0.20", "0.25", "0.30", "0.40",
                                                                                                 "0.50", "0.60", "0.70", "0.75",
                                                                                                 "0.80", "0.85", "0.90", "0.95"};
static const double liblte_rrc_ac_barring_factor_num[LIBLTE_RRC_AC_BARRING_FACTOR_N_ITEMS] = {0.00, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.40,
                                                                                              0.50, 0.60, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95};
typedef enum{
    LIBLTE_RRC_AC_BARRING_TIME_S4 = 0,
    LIBLTE_RRC_AC_BARRING_TIME_S8,
    LIBLTE_RRC_AC_BARRING_TIME_S16,
    LIBLTE_RRC_AC_BARRING_TIME_S32,
    LIBLTE_RRC_AC_BARRING_TIME_S64,
    LIBLTE_RRC_AC_BARRING_TIME_S128,
    LIBLTE_RRC_AC_BARRING_TIME_S256,
    LIBLTE_RRC_AC_BARRING_TIME_S512,
    LIBLTE_RRC_AC_BARRING_TIME_N_ITEMS,
}LIBLTE_RRC_AC_BARRING_TIME_ENUM;
static const char liblte_rrc_ac_barring_time_text[LIBLTE_RRC_AC_BARRING_TIME_N_ITEMS][20] = {  "4",   "8",  "16",  "32",
                                                                                              "64", "128", "256", "512"};
static const uint16 liblte_rrc_ac_barring_time_num[LIBLTE_RRC_AC_BARRING_TIME_N_ITEMS] = {4, 8, 16, 32, 64, 128, 256, 512};
typedef enum{
    LIBLTE_RRC_UL_BW_N6 = 0,
    LIBLTE_RRC_UL_BW_N15,
    LIBLTE_RRC_UL_BW_N25,
    LIBLTE_RRC_UL_BW_N50,
    LIBLTE_RRC_UL_BW_N75,
    LIBLTE_RRC_UL_BW_N100,
    LIBLTE_RRC_UL_BW_N_ITEMS,
}LIBLTE_RRC_UL_BW_ENUM;
static const char liblte_rrc_ul_bw_text[LIBLTE_RRC_UL_BW_N_ITEMS][20] = {"1.4",   "3",   "5",  "10",
                                                                          "15",  "20"};
static const double liblte_rrc_ul_bw_num[LIBLTE_RRC_UL_BW_N_ITEMS] = {1.4, 3, 5, 10, 15, 20};
// Structs
typedef struct{
    LIBLTE_RRC_AC_BARRING_FACTOR_ENUM factor;
    LIBLTE_RRC_AC_BARRING_TIME_ENUM   time;
    uint8                             for_special_ac;
    bool                              enabled;
}LIBLTE_RRC_AC_BARRING_CONFIG_STRUCT;
typedef struct{
    uint16 value;
    bool   present;
}LIBLTE_RRC_ARFCN_VALUE_EUTRA_STRUCT;
typedef struct{
    LIBLTE_RRC_UL_BW_ENUM bw;
    bool                  present;
}LIBLTE_RRC_UL_BW_STRUCT;
typedef struct{
    LIBLTE_RRC_AC_BARRING_CONFIG_STRUCT       ac_barring_for_mo_signalling;
    LIBLTE_RRC_AC_BARRING_CONFIG_STRUCT       ac_barring_for_mo_data;
    LIBLTE_RRC_RR_CONFIG_COMMON_SIB_STRUCT    rr_config_common_sib;
    LIBLTE_RRC_UE_TIMERS_AND_CONSTANTS_STRUCT ue_timers_and_constants;
    LIBLTE_RRC_ARFCN_VALUE_EUTRA_STRUCT       arfcn_value_eutra;
    LIBLTE_RRC_UL_BW_STRUCT                   ul_bw;
    LIBLTE_RRC_MBSFN_SUBFRAME_CONFIG_STRUCT   mbsfn_subfr_cnfg[LIBLTE_RRC_MAX_MBSFN_ALLOCATIONS];
    LIBLTE_RRC_TIME_ALIGNMENT_TIMER_ENUM      time_alignment_timer;
    uint32                                    mbsfn_subfr_cnfg_list_size;
    uint8                                     additional_spectrum_emission;
    bool                                      ac_barring_for_emergency;
    bool                                      ac_barring_info_present;
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_2_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT  *sib2,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_2_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT  *sib2);

/*********************************************************************
    IE Name: System Information Block Type 3

    Description: Contains cell reselection information common for
                 intra-frequency, inter-frequency, and/or inter-RAT
                 cell re-selection as well as intra-frequency cell
                 re-selection information other than neighboring
                 cell related

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_Q_HYST_DB_0 = 0,
    LIBLTE_RRC_Q_HYST_DB_1,
    LIBLTE_RRC_Q_HYST_DB_2,
    LIBLTE_RRC_Q_HYST_DB_3,
    LIBLTE_RRC_Q_HYST_DB_4,
    LIBLTE_RRC_Q_HYST_DB_5,
    LIBLTE_RRC_Q_HYST_DB_6,
    LIBLTE_RRC_Q_HYST_DB_8,
    LIBLTE_RRC_Q_HYST_DB_10,
    LIBLTE_RRC_Q_HYST_DB_12,
    LIBLTE_RRC_Q_HYST_DB_14,
    LIBLTE_RRC_Q_HYST_DB_16,
    LIBLTE_RRC_Q_HYST_DB_18,
    LIBLTE_RRC_Q_HYST_DB_20,
    LIBLTE_RRC_Q_HYST_DB_22,
    LIBLTE_RRC_Q_HYST_DB_24,
    LIBLTE_RRC_Q_HYST_N_ITEMS,
}LIBLTE_RRC_Q_HYST_ENUM;
static const char liblte_rrc_q_hyst_text[LIBLTE_RRC_Q_HYST_N_ITEMS][20] = { "0",  "1",  "2",  "3",
                                                                            "4",  "5",  "6",  "8",
                                                                           "10", "12", "14", "16",
                                                                           "18", "20", "22", "24"};
static const uint8 liblte_rrc_q_hyst_num[LIBLTE_RRC_Q_HYST_N_ITEMS] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
typedef enum{
    LIBLTE_RRC_SF_MEDIUM_DB_N6 = 0,
    LIBLTE_RRC_SF_MEDIUM_DB_N4,
    LIBLTE_RRC_SF_MEDIUM_DB_N2,
    LIBLTE_RRC_SF_MEDIUM_DB_0,
    LIBLTE_RRC_SF_MEDIUM_N_ITEMS,
}LIBLTE_RRC_SF_MEDIUM_ENUM;
static const char liblte_rrc_sf_medium_text[LIBLTE_RRC_SF_MEDIUM_N_ITEMS][20] = {"-6", "-4", "-2", "0"};
static const int8 liblte_rrc_sf_medium_num[LIBLTE_RRC_SF_MEDIUM_N_ITEMS] = {-6, -4, -2, 0};
typedef enum{
    LIBLTE_RRC_SF_HIGH_DB_N6 = 0,
    LIBLTE_RRC_SF_HIGH_DB_N4,
    LIBLTE_RRC_SF_HIGH_DB_N2,
    LIBLTE_RRC_SF_HIGH_DB_0,
    LIBLTE_RRC_SF_HIGH_N_ITEMS,
}LIBLTE_RRC_SF_HIGH_ENUM;
static const char liblte_rrc_sf_high_text[LIBLTE_RRC_SF_HIGH_N_ITEMS][20] = {"-6", "-4", "-2", "0"};
static const int8 liblte_rrc_sf_high_num[LIBLTE_RRC_SF_HIGH_N_ITEMS] = {-6, -4, -2, 0};
// Structs
typedef struct{
    LIBLTE_RRC_SF_MEDIUM_ENUM medium;
    LIBLTE_RRC_SF_HIGH_ENUM   high;
}LIBLTE_RRC_Q_HYST_SF_STRUCT;
typedef struct{
    LIBLTE_RRC_MOBILITY_STATE_PARAMETERS_STRUCT mobility_state_params;
    LIBLTE_RRC_Q_HYST_SF_STRUCT                 q_hyst_sf;
    bool                                        present;
}LIBLTE_RRC_SPEED_STATE_RESELECTION_PARS_STRUCT;
typedef struct{
    LIBLTE_RRC_SPEED_STATE_RESELECTION_PARS_STRUCT speed_state_resel_params;
    LIBLTE_RRC_SPEED_STATE_SCALE_FACTORS_STRUCT    t_resel_eutra_sf;
    LIBLTE_RRC_Q_HYST_ENUM                         q_hyst;
    LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_ENUM         allowed_meas_bw;
    int16                                          q_rx_lev_min;
    uint8                                          s_non_intra_search;
    uint8                                          thresh_serving_low;
    uint8                                          cell_resel_prio;
    uint8                                          s_intra_search;
    uint8                                          neigh_cell_cnfg;
    uint8                                          t_resel_eutra;
    int8                                           p_max;
    bool                                           s_non_intra_search_present;
    bool                                           presence_ant_port_1;
    bool                                           p_max_present;
    bool                                           s_intra_search_present;
    bool                                           allowed_meas_bw_present;
    bool                                           t_resel_eutra_sf_present;
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_3_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT  *sib3,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_3_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT  *sib3);

/*********************************************************************
    IE Name: System Information Block Type 4

    Description: Contains the neighboring cell related information
                 relevant only for intra-frequency cell reselection

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_CELL_INTRA 16
#define LIBLTE_RRC_MAX_CELL_BLACK 16
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_Q_OFFSET_RANGE_ENUM q_offset_range;
    uint16                         phys_cell_id;
}LIBLTE_RRC_INTRA_FREQ_NEIGH_CELL_INFO_STRUCT;
typedef struct{
    LIBLTE_RRC_INTRA_FREQ_NEIGH_CELL_INFO_STRUCT intra_freq_neigh_cell_list[LIBLTE_RRC_MAX_CELL_INTRA];
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_STRUCT         intra_freq_black_cell_list[LIBLTE_RRC_MAX_CELL_BLACK];
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_STRUCT         csg_phys_cell_id_range;
    uint32                                       intra_freq_neigh_cell_list_size;
    uint32                                       intra_freq_black_cell_list_size;
    bool                                         csg_phys_cell_id_range_present;
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_4_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT  *sib4,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_4_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT  *sib4);

/*********************************************************************
    IE Name: System Information Block Type 5

    Description: Contains information relevant only for
                 inter-frequency cell reselection, i.e. information
                 about other E-UTRA frequencies and inter-frequency
                 neighboring cells relevant for cell reselection

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_FREQ       8
#define LIBLTE_RRC_MAX_CELL_INTER 16
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_Q_OFFSET_RANGE_ENUM q_offset_cell;
    uint16                         phys_cell_id;
}LIBLTE_RRC_INTER_FREQ_NEIGH_CELL_STRUCT;
typedef struct{
    LIBLTE_RRC_SPEED_STATE_SCALE_FACTORS_STRUCT t_resel_eutra_sf;
    LIBLTE_RRC_INTER_FREQ_NEIGH_CELL_STRUCT     inter_freq_neigh_cell_list[LIBLTE_RRC_MAX_CELL_INTER];
    LIBLTE_RRC_PHYS_CELL_ID_RANGE_STRUCT        inter_freq_black_cell_list[LIBLTE_RRC_MAX_CELL_BLACK];
    LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_ENUM      allowed_meas_bw;
    LIBLTE_RRC_Q_OFFSET_RANGE_ENUM              q_offset_freq;
    uint16                                      dl_carrier_freq;
    int16                                       q_rx_lev_min;
    uint8                                       t_resel_eutra;
    uint8                                       threshx_high;
    uint8                                       threshx_low;
    uint8                                       cell_resel_prio;
    uint8                                       neigh_cell_cnfg;
    uint8                                       inter_freq_neigh_cell_list_size;
    uint8                                       inter_freq_black_cell_list_size;
    int8                                        p_max;
    bool                                        presence_ant_port_1;
    bool                                        p_max_present;
    bool                                        t_resel_eutra_sf_present;
    bool                                        cell_resel_prio_present;
}LIBLTE_RRC_INTER_FREQ_CARRIER_FREQ_INFO_STRUCT;
typedef struct{
    LIBLTE_RRC_INTER_FREQ_CARRIER_FREQ_INFO_STRUCT inter_freq_carrier_freq_list[LIBLTE_RRC_MAX_FREQ];
    uint32                                         inter_freq_carrier_freq_list_size;
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_5_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_5_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_5_STRUCT  *sib5,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_5_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_5_STRUCT  *sib5);

/*********************************************************************
    IE Name: System Information Block Type 6

    Description: Contains information relevant only for inter-RAT
                 cell reselection, i.e. information about UTRA
                 frequencies and UTRA neighboring cells relevant for
                 cell reselection

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_UTRA_FDD_CARRIER 16
#define LIBLTE_RRC_MAX_UTRA_TDD_CARRIER 16
// Enums
// Structs
typedef struct{
    uint16 carrier_freq;
    uint8  cell_resel_prio;
    uint8  threshx_high;
    uint8  threshx_low;
    int8   q_rx_lev_min;
    int8   p_max_utra;
    int8   q_qual_min;
    bool   cell_resel_prio_present;
}LIBLTE_RRC_CARRIER_FREQ_UTRA_FDD_STRUCT;
typedef struct{
    uint16 carrier_freq;
    uint8  cell_resel_prio;
    uint8  threshx_high;
    uint8  threshx_low;
    int8   q_rx_lev_min;
    int8   p_max_utra;
    bool   cell_resel_prio_present;
}LIBLTE_RRC_CARRIER_FREQ_UTRA_TDD_STRUCT;
typedef struct{
    LIBLTE_RRC_CARRIER_FREQ_UTRA_FDD_STRUCT     carrier_freq_list_utra_fdd[LIBLTE_RRC_MAX_UTRA_FDD_CARRIER];
    LIBLTE_RRC_CARRIER_FREQ_UTRA_TDD_STRUCT     carrier_freq_list_utra_tdd[LIBLTE_RRC_MAX_UTRA_TDD_CARRIER];
    LIBLTE_RRC_SPEED_STATE_SCALE_FACTORS_STRUCT t_resel_utra_sf;
    uint8                                       t_resel_utra;
    uint8                                       carrier_freq_list_utra_fdd_size;
    uint8                                       carrier_freq_list_utra_tdd_size;
    bool                                        t_resel_utra_sf_present;
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_6_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_6_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_6_STRUCT  *sib6,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_6_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_6_STRUCT  *sib6);

/*********************************************************************
    IE Name: System Information Block Type 7

    Description: Contains information relevant only for inter-RAT
                 cell reselection, i.e. information about GERAN
                 frequencies relevant for cell reselection

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_GNFG 16
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_CARRIER_FREQS_GERAN_STRUCT carrier_freqs;
    uint8                                 cell_resel_prio;
    uint8                                 ncc_permitted;
    uint8                                 p_max_geran;
    uint8                                 threshx_high;
    uint8                                 threshx_low;
    int8                                  q_rx_lev_min;
    bool                                  cell_resel_prio_present;
    bool                                  p_max_geran_present;
}LIBLTE_RRC_CARRIER_FREQS_INFO_LIST_GERAN_STRUCT;
typedef struct{
    LIBLTE_RRC_SPEED_STATE_SCALE_FACTORS_STRUCT     t_resel_geran_sf;
    LIBLTE_RRC_CARRIER_FREQS_INFO_LIST_GERAN_STRUCT carrier_freqs_info_list[LIBLTE_RRC_MAX_GNFG];
    uint8                                           t_resel_geran;
    uint8                                           carrier_freqs_info_list_size;
    bool                                            t_resel_geran_sf_present;
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_7_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_7_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_7_STRUCT  *sib7,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_7_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_7_STRUCT  *sib7);

/*********************************************************************
    IE Name: System Information Block Type 8

    Description: Contains information relevant only for inter-RAT
                 cell re-selection i.e. information about CDMA2000
                 frequencies and CDMA2000 neighboring cells relevant
                 for cell re-selection

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_CDMA_BAND_CLASS 32
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_BAND_CLASS_CDMA2000_ENUM band_class;
    uint8                               cell_resel_prio;
    uint8                               thresh_x_high;
    uint8                               thresh_x_low;
    bool                                cell_resel_prio_present;
}LIBLTE_RRC_BAND_CLASS_INFO_CDMA2000_STRUCT;
typedef struct{
    uint16 arfcn;
    uint16 phys_cell_id_list[16];
    uint8  phys_cell_id_list_size;
}LIBLTE_RRC_NEIGH_CELLS_PER_BAND_CLASS_CDMA2000_STRUCT;
typedef struct{
    LIBLTE_RRC_BAND_CLASS_CDMA2000_ENUM                   band_class;
    LIBLTE_RRC_NEIGH_CELLS_PER_BAND_CLASS_CDMA2000_STRUCT neigh_cells_per_freq_list[16];
    uint8                                                 neigh_cells_per_freq_list_size;
}LIBLTE_RRC_NEIGH_CELL_CDMA2000_STRUCT;
typedef struct{
    LIBLTE_RRC_BAND_CLASS_INFO_CDMA2000_STRUCT  band_class_list[LIBLTE_RRC_MAX_CDMA_BAND_CLASS];
    LIBLTE_RRC_NEIGH_CELL_CDMA2000_STRUCT       neigh_cell_list[16];
    LIBLTE_RRC_SPEED_STATE_SCALE_FACTORS_STRUCT t_resel_cdma2000_sf;
    uint8                                       band_class_list_size;
    uint8                                       neigh_cell_list_size;
    uint8                                       t_resel_cdma2000;
    bool                                        t_resel_cdma2000_sf_present;
}LIBLTE_RRC_CELL_RESELECTION_PARAMS_CDMA2000_STRUCT;
typedef struct{
    LIBLTE_RRC_SYSTEM_TIME_INFO_CDMA2000_STRUCT        sys_time_info_cdma2000;
    LIBLTE_RRC_PRE_REGISTRATION_INFO_HRPD_STRUCT       pre_reg_info_hrpd;
    LIBLTE_RRC_CELL_RESELECTION_PARAMS_CDMA2000_STRUCT cell_resel_params_hrpd;
    LIBLTE_RRC_CSFB_REGISTRATION_PARAM_1XRTT_STRUCT    csfb_reg_param_1xrtt;
    LIBLTE_RRC_CELL_RESELECTION_PARAMS_CDMA2000_STRUCT cell_resel_params_1xrtt;
    uint64                                             long_code_state_1xrtt;
    uint8                                              search_win_size;
    bool                                               sys_time_info_present;
    bool                                               search_win_size_present;
    bool                                               params_hrpd_present;
    bool                                               cell_resel_params_hrpd_present;
    bool                                               params_1xrtt_present;
    bool                                               csfb_reg_param_1xrtt_present;
    bool                                               long_code_state_1xrtt_present;
    bool                                               cell_resel_params_1xrtt_present;
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_8_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_8_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_8_STRUCT  *sib8,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_8_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_8_STRUCT  *sib8);

/*********************************************************************
    IE Name: System Information Block Type 9

    Description: Contains a home eNB name (HNB name)

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// Defines
// Enums
// Structs
// lb:1, ub:48
typedef struct{
  uint32 hnb_name_size;
  uint8  hnb_name[48];
  bool   hnb_name_present;
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9_STRUCT;

// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_9_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9_STRUCT  *sib9,
                                                           uint8                                   **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_9_ie(uint8                                   **ie_ptr,
                                                             LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9_STRUCT  *sib9);

/*********************************************************************
    IE Name: System Information Block Type 10

    Description: Contains an ETWS primary notification

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: System Information Block Type 11

    Description: Contains an ETWS secondary notification

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: System Information Block Type 12

    Description: Contains a CMAS notification

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    IE Name: System Information Block Type 13

    Description: Contains the information required to acquire the
                 MBMS control information associated with one or more
                 MBSFN areas

    Document Reference: 36.331 v10.0.0 Section 6.3.1
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_MBSFN_AREA_INFO_STRUCT           mbsfn_area_info_list_r9[LIBLTE_RRC_MAX_MBSFN_AREAS];
    LIBLTE_RRC_MBSFN_NOTIFICATION_CONFIG_STRUCT mbms_notification_config;
    uint8                                       mbsfn_area_info_list_r9_size;
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_13_ie(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT  *sib13,
                                                            uint8                                    **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_13_ie(uint8                                    **ie_ptr,
                                                              LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT  *sib13);

/*******************************************************************************
                              MESSAGE DECLARATIONS
*******************************************************************************/

/*********************************************************************
    Message Name: UL Information Transfer

    Description: Used for the uplink transfer dedicated NAS
                 information

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_NAS = 0,
    LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_CDMA2000_1XRTT,
    LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_CDMA2000_HRPD,
    LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_N_ITEMS,
}LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_ENUM;
static const char liblte_rrc_ul_information_transfer_type_text[LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_N_ITEMS][20] = {"NAS",
                                                                                                                       "CDMA2000-1XRTT",
                                                                                                                       "CDMA2000-HRPD"};
// Structs
typedef struct{
    LIBLTE_SIMPLE_BYTE_MSG_STRUCT                dedicated_info;
    LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_ENUM dedicated_info_type;
}LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_information_transfer_msg(LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT *ul_info_transfer,
                                                              LIBLTE_BIT_MSG_STRUCT                     *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_information_transfer_msg(LIBLTE_BIT_MSG_STRUCT                     *msg,
                                                                LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT *ul_info_transfer);

/*********************************************************************
    Message Name: UL Handover Preparation Transfer (CDMA2000)

    Description: Used for the uplink transfer of handover related
                 CDMA2000 information when requested by the higher
                 layers

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    // FIXME
}LIBLTE_RRC_UL_HANDOVER_PREPARATION_TRANSFER_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_handover_preparation_transfer_msg(LIBLTE_RRC_UL_HANDOVER_PREPARATION_TRANSFER_STRUCT *ul_handover_prep_transfer,
                                                                       LIBLTE_BIT_MSG_STRUCT                              *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_handover_preparation_transfer_msg(LIBLTE_BIT_MSG_STRUCT                              *msg,
                                                                         LIBLTE_RRC_UL_HANDOVER_PREPARATION_TRANSFER_STRUCT *ul_handover_prep_transfer);

/*********************************************************************
    Message Name: UE Information Response

    Description: Used by the UE to transfer the information requested
                 by the E-UTRAN

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    // FIXME
}LIBLTE_RRC_UE_INFORMATION_RESPONSE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_ue_information_response_msg(LIBLTE_RRC_UE_INFORMATION_RESPONSE_STRUCT *ue_info_resp,
                                                              LIBLTE_BIT_MSG_STRUCT                     *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ue_information_response_msg(LIBLTE_BIT_MSG_STRUCT                     *msg,
                                                                LIBLTE_RRC_UE_INFORMATION_RESPONSE_STRUCT *ue_info_resp);

/*********************************************************************
    Message Name: UE Information Request

    Description: Used by E-UTRAN to retrieve information from the UE

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 rrc_transaction_id;
    bool  rach_report_req;
    bool  rlf_report_req;
}LIBLTE_RRC_UE_INFORMATION_REQUEST_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_ue_information_request_msg(LIBLTE_RRC_UE_INFORMATION_REQUEST_STRUCT *ue_info_req,
                                                             LIBLTE_BIT_MSG_STRUCT                    *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ue_information_request_msg(LIBLTE_BIT_MSG_STRUCT                    *msg,
                                                               LIBLTE_RRC_UE_INFORMATION_REQUEST_STRUCT *ue_info_req);

/*********************************************************************
    Message Name: UE Capability Information

    Description: Used to transfer UE radio access capabilities
                 requested by the E-UTRAN

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_RAT_CAPABILITIES 8
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_RAT_TYPE_ENUM              rat_type;
    LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT eutra_capability;
}LIBLTE_RRC_UE_CAPABILITY_RAT_CONTAINER_STRUCT;

typedef struct{
    uint8                                           rrc_transaction_id;
    LIBLTE_RRC_UE_CAPABILITY_RAT_CONTAINER_STRUCT   ue_capability_rat[LIBLTE_RRC_MAX_RAT_CAPABILITIES];
    uint32                                          N_ue_caps;
}LIBLTE_RRC_UE_CAPABILITY_INFORMATION_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_ue_capability_information_msg(LIBLTE_RRC_UE_CAPABILITY_INFORMATION_STRUCT *ue_capability_info,
                                                                LIBLTE_BIT_MSG_STRUCT                       *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ue_capability_information_msg(LIBLTE_BIT_MSG_STRUCT                       *msg,
                                                                  LIBLTE_RRC_UE_CAPABILITY_INFORMATION_STRUCT *ue_capability_info);

/*********************************************************************
    Message Name: UE Capability Enquiry

    Description: Used to request the transfer of UE radio access
                 capabilities for E-UTRA as well as for other RATs

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8                     rrc_transaction_id;
    LIBLTE_RRC_RAT_TYPE_ENUM  ue_capability_request[LIBLTE_RRC_MAX_RAT_CAPABILITIES];
    uint32                    N_ue_cap_reqs;

}LIBLTE_RRC_UE_CAPABILITY_ENQUIRY_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_ue_capability_enquiry_msg(LIBLTE_RRC_UE_CAPABILITY_ENQUIRY_STRUCT *ue_cap_enquiry,
                                                            LIBLTE_BIT_MSG_STRUCT                   *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ue_capability_enquiry_msg(LIBLTE_BIT_MSG_STRUCT                   *msg,
                                                              LIBLTE_RRC_UE_CAPABILITY_ENQUIRY_STRUCT *ue_cap_enquiry);

/*********************************************************************
    Message Name: System Information Block Type 1

    Description: Contains information relevant when evaluating if a
                 UE is allowed to access a cell and defines the
                 scheduling of other system information

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_N_PLMN_IDENTITIES 6
#define LIBLTE_RRC_MAX_SIB               32
#define LIBLTE_RRC_MAX_SI_MESSAGE        32
// Enums
typedef enum{
    LIBLTE_RRC_CELL_BARRED = 0,
    LIBLTE_RRC_CELL_NOT_BARRED,
    LIBLTE_RRC_CELL_BARRED_N_ITEMS,
}LIBLTE_RRC_CELL_BARRED_ENUM;
static const char liblte_rrc_cell_barred_text[LIBLTE_RRC_CELL_BARRED_N_ITEMS][20] = {"Barred", "Not Barred"};
typedef enum{
    LIBLTE_RRC_INTRA_FREQ_RESELECTION_ALLOWED = 0,
    LIBLTE_RRC_INTRA_FREQ_RESELECTION_NOT_ALLOWED,
    LIBLTE_RRC_INTRA_FREQ_RESELECTION_N_ITEMS,
}LIBLTE_RRC_INTRA_FREQ_RESELECTION_ENUM;
static const char liblte_rrc_intra_freq_reselection_text[LIBLTE_RRC_INTRA_FREQ_RESELECTION_N_ITEMS][20] = {"Allowed", "Not Allowed"};
typedef enum{
    LIBLTE_RRC_SI_WINDOW_LENGTH_MS1 = 0,
    LIBLTE_RRC_SI_WINDOW_LENGTH_MS2,
    LIBLTE_RRC_SI_WINDOW_LENGTH_MS5,
    LIBLTE_RRC_SI_WINDOW_LENGTH_MS10,
    LIBLTE_RRC_SI_WINDOW_LENGTH_MS15,
    LIBLTE_RRC_SI_WINDOW_LENGTH_MS20,
    LIBLTE_RRC_SI_WINDOW_LENGTH_MS40,
    LIBLTE_RRC_SI_WINDOW_LENGTH_N_ITEMS,
}LIBLTE_RRC_SI_WINDOW_LENGTH_ENUM;
static const char liblte_rrc_si_window_length_text[LIBLTE_RRC_SI_WINDOW_LENGTH_N_ITEMS][20] = { "1",  "2",  "5", "10",
                                                                                               "15", "20", "40"};
static const uint8 liblte_rrc_si_window_length_num[LIBLTE_RRC_SI_WINDOW_LENGTH_N_ITEMS] = {1, 2, 5, 10, 15, 20, 40};
typedef enum{
    LIBLTE_RRC_RESV_FOR_OPER = 0,
    LIBLTE_RRC_NOT_RESV_FOR_OPER,
    LIBLTE_RRC_RESV_FOR_OPER_N_ITEMS,
}LIBLTE_RRC_RESV_FOR_OPER_ENUM;
static const char liblte_rrc_resv_for_oper_text[LIBLTE_RRC_RESV_FOR_OPER_N_ITEMS][20] = {"Reserved", "Not Reserved"};
typedef enum{
    LIBLTE_RRC_SI_PERIODICITY_RF8 = 0,
    LIBLTE_RRC_SI_PERIODICITY_RF16,
    LIBLTE_RRC_SI_PERIODICITY_RF32,
    LIBLTE_RRC_SI_PERIODICITY_RF64,
    LIBLTE_RRC_SI_PERIODICITY_RF128,
    LIBLTE_RRC_SI_PERIODICITY_RF256,
    LIBLTE_RRC_SI_PERIODICITY_RF512,
    LIBLTE_RRC_SI_PERIODICITY_N_ITEMS,
}LIBLTE_RRC_SI_PERIODICITY_ENUM;
static const char liblte_rrc_si_periodicity_text[LIBLTE_RRC_SI_PERIODICITY_N_ITEMS][20] = {  "8",  "16",  "32",  "64",
                                                                                           "128", "256", "512"};
static const uint16 liblte_rrc_si_periodicity_num[LIBLTE_RRC_SI_PERIODICITY_N_ITEMS] = {8, 16, 32, 64, 128, 256, 512};
typedef enum{
    LIBLTE_RRC_SIB_TYPE_3 = 0,
    LIBLTE_RRC_SIB_TYPE_4,
    LIBLTE_RRC_SIB_TYPE_5,
    LIBLTE_RRC_SIB_TYPE_6,
    LIBLTE_RRC_SIB_TYPE_7,
    LIBLTE_RRC_SIB_TYPE_8,
    LIBLTE_RRC_SIB_TYPE_9,
    LIBLTE_RRC_SIB_TYPE_10,
    LIBLTE_RRC_SIB_TYPE_11,
    LIBLTE_RRC_SIB_TYPE_12_v920,
    LIBLTE_RRC_SIB_TYPE_13_v920,
    LIBLTE_RRC_SIB_TYPE_SPARE_5,
    LIBLTE_RRC_SIB_TYPE_SPARE_4,
    LIBLTE_RRC_SIB_TYPE_SPARE_3,
    LIBLTE_RRC_SIB_TYPE_SPARE_2,
    LIBLTE_RRC_SIB_TYPE_SPARE_1,
    LIBLTE_RRC_SIB_TYPE_N_ITEMS,
}LIBLTE_RRC_SIB_TYPE_ENUM;
static const char liblte_rrc_sib_type_text[LIBLTE_RRC_SIB_TYPE_N_ITEMS][20] = {    "3",     "4",     "5",     "6",
                                                                                   "7",     "8",     "9",    "10",
                                                                                  "11",    "12",    "13", "SPARE",
                                                                               "SPARE", "SPARE", "SPARE", "SPARE"};
static const uint8 liblte_rrc_sib_type_num[LIBLTE_RRC_SIB_TYPE_N_ITEMS] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0, 0, 0, 0, 0};
// Structs
typedef struct{
    LIBLTE_RRC_PLMN_IDENTITY_STRUCT id;
    LIBLTE_RRC_RESV_FOR_OPER_ENUM   resv_for_oper;
}LIBLTE_RRC_PLMN_IDENTITY_LIST_STRUCT;
typedef struct{
    LIBLTE_RRC_SIB_TYPE_ENUM sib_type;
}LIBLTE_RRC_SIB_MAPPING_INFO_STRUCT;
typedef struct{
    LIBLTE_RRC_SIB_MAPPING_INFO_STRUCT sib_mapping_info[LIBLTE_RRC_MAX_SIB];
    LIBLTE_RRC_SI_PERIODICITY_ENUM     si_periodicity;
    uint32                             N_sib_mapping_info;
}LIBLTE_RRC_SCHEDULING_INFO_STRUCT;
typedef struct{
    LIBLTE_RRC_PLMN_IDENTITY_LIST_STRUCT      plmn_id[LIBLTE_RRC_MAX_N_PLMN_IDENTITIES];
    LIBLTE_RRC_SCHEDULING_INFO_STRUCT         sched_info[LIBLTE_RRC_MAX_SI_MESSAGE];
    LIBLTE_RRC_TDD_CONFIG_STRUCT              tdd_cnfg;
    LIBLTE_RRC_CELL_BARRED_ENUM               cell_barred;
    LIBLTE_RRC_INTRA_FREQ_RESELECTION_ENUM    intra_freq_reselection;
    LIBLTE_RRC_SI_WINDOW_LENGTH_ENUM          si_window_length;
    uint32                                    cell_id;
    uint32                                    csg_id;
    uint32                                    N_plmn_ids;
    uint32                                    N_sched_info;
    uint16                                    tracking_area_code;
    int16                                     q_rx_lev_min;
    uint8                                     csg_indication;
    uint8                                     q_rx_lev_min_offset;
    uint8                                     freq_band_indicator;
    uint8                                     system_info_value_tag;
    int8                                      p_max;
    bool                                      tdd;
    bool                                      p_max_present;
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_block_type_1_msg(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *sib1,
                                                            LIBLTE_BIT_MSG_STRUCT                   *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_block_type_1_msg(LIBLTE_BIT_MSG_STRUCT                   *msg,
                                                              LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *sib1,
                                                              uint32                                  *N_bits_used);

/*********************************************************************
    Message Name: System Information

    Description: Conveys one or more System Information Blocks

    Document Reference: 36.331 v10.0.0 Section 6.2.2
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2 = 0,
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3,
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4,
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_5,
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_6,
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_7,
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_8,
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9,
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_10,
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_11,
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_12,
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13,
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1, // Intentionally not first
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_N_ITEMS,
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_ENUM;
static const char liblte_rrc_sys_info_block_type_text[LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_N_ITEMS][20] = { "2",  "3",  "4",  "5",
                                                                                                      "6",  "7",  "8",  "9",
                                                                                                     "10", "11", "12", "13",
                                                                                                      "1"};
static const uint8 liblte_rrc_sys_info_block_type_num[LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_N_ITEMS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 1};
// Structs
typedef union{
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT  sib1;
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT  sib2;
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT  sib3;
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT  sib4;
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_5_STRUCT  sib5;
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_6_STRUCT  sib6;
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_7_STRUCT  sib7;
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_8_STRUCT  sib8;
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9_STRUCT  sib9;
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT sib13;
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_UNION;
typedef struct{
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_UNION sib;
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_ENUM  sib_type;
}LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_STRUCT;
typedef struct{
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_STRUCT sibs[LIBLTE_RRC_MAX_SIB];
    uint32                                N_sibs;
}LIBLTE_RRC_SYS_INFO_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_sys_info_msg(LIBLTE_RRC_SYS_INFO_MSG_STRUCT *sibs,
                                               LIBLTE_BIT_MSG_STRUCT          *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_sys_info_msg(LIBLTE_BIT_MSG_STRUCT          *msg,
                                                 LIBLTE_RRC_SYS_INFO_MSG_STRUCT *sibs);

/*********************************************************************
    Message Name: Security Mode Failure

    Description: Used to indicate an unsuccessful completion of a
                 security mode command

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 rrc_transaction_id;
}LIBLTE_RRC_SECURITY_MODE_FAILURE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_security_mode_failure_msg(LIBLTE_RRC_SECURITY_MODE_FAILURE_STRUCT *security_mode_failure,
                                                            LIBLTE_BIT_MSG_STRUCT                   *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_security_mode_failure_msg(LIBLTE_BIT_MSG_STRUCT                   *msg,
                                                              LIBLTE_RRC_SECURITY_MODE_FAILURE_STRUCT *security_mode_failure);

/*********************************************************************
    Message Name: Security Mode Complete

    Description: Used to confirm the successful completion of a
                 security mode command

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 rrc_transaction_id;
}LIBLTE_RRC_SECURITY_MODE_COMPLETE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_security_mode_complete_msg(LIBLTE_RRC_SECURITY_MODE_COMPLETE_STRUCT *security_mode_complete,
                                                             LIBLTE_BIT_MSG_STRUCT                    *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_security_mode_complete_msg(LIBLTE_BIT_MSG_STRUCT                    *msg,
                                                               LIBLTE_RRC_SECURITY_MODE_COMPLETE_STRUCT *security_mode_complete);

/*********************************************************************
    Message Name: Security Mode Command

    Description: Used to command the activation of AS security

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_SECURITY_ALGORITHM_CONFIG_STRUCT sec_algs;
    uint8                                       rrc_transaction_id;
}LIBLTE_RRC_SECURITY_MODE_COMMAND_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_security_mode_command_msg(LIBLTE_RRC_SECURITY_MODE_COMMAND_STRUCT *security_mode_cmd,
                                                            LIBLTE_BIT_MSG_STRUCT                   *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_security_mode_command_msg(LIBLTE_BIT_MSG_STRUCT                   *msg,
                                                              LIBLTE_RRC_SECURITY_MODE_COMMAND_STRUCT *security_mode_cmd);

/*********************************************************************
    Message Name: RRC Connection Setup Complete

    Description: Used to confirm the successful completion of an RRC
                 connection

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id;
    uint16                          mmegi;
    uint8                           mmec;
    bool                            plmn_id_present;
}LIBLTE_RRC_REGISTERED_MME_STRUCT;
typedef struct{
    LIBLTE_RRC_REGISTERED_MME_STRUCT registered_mme;
    LIBLTE_SIMPLE_BYTE_MSG_STRUCT    dedicated_info_nas;
    uint8                            rrc_transaction_id;
    uint8                            selected_plmn_id;
    bool                             registered_mme_present;
}LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_setup_complete_msg(LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT *con_setup_complete,
                                                                    LIBLTE_BIT_MSG_STRUCT                       *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_setup_complete_msg(LIBLTE_BIT_MSG_STRUCT                       *msg,
                                                                      LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT *con_setup_complete);

/*********************************************************************
    Message Name: RRC Connection Setup

    Description: Used to establish SRB1

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT rr_cnfg;
    uint8                                 rrc_transaction_id;
}LIBLTE_RRC_CONNECTION_SETUP_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_setup_msg(LIBLTE_RRC_CONNECTION_SETUP_STRUCT *con_setup,
                                                           LIBLTE_BIT_MSG_STRUCT              *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_setup_msg(LIBLTE_BIT_MSG_STRUCT              *msg,
                                                             LIBLTE_RRC_CONNECTION_SETUP_STRUCT *con_setup);

/*********************************************************************
    Message Name: RRC Connection Request

    Description: Used to request the establishment of an RRC
                 connection

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_CON_REQ_UE_ID_TYPE_S_TMSI = 0,
    LIBLTE_RRC_CON_REQ_UE_ID_TYPE_RANDOM_VALUE,
    LIBLTE_RRC_CON_REQ_UE_ID_TYPE_N_ITEMS,
}LIBLTE_RRC_CON_REQ_UE_ID_TYPE_ENUM;
static const char liblte_rrc_con_req_ue_id_type_text[LIBLTE_RRC_CON_REQ_UE_ID_TYPE_N_ITEMS][20] = {"S-TMSI",
                                                                                                   "Random Value"};
typedef enum{
    LIBLTE_RRC_CON_REQ_EST_CAUSE_EMERGENCY = 0,
    LIBLTE_RRC_CON_REQ_EST_CAUSE_HIGH_PRIO_ACCESS,
    LIBLTE_RRC_CON_REQ_EST_CAUSE_MT_ACCESS,
    LIBLTE_RRC_CON_REQ_EST_CAUSE_MO_SIGNALLING,
    LIBLTE_RRC_CON_REQ_EST_CAUSE_MO_DATA,
    LIBLTE_RRC_CON_REQ_EST_CAUSE_SPARE3,
    LIBLTE_RRC_CON_REQ_EST_CAUSE_SPARE2,
    LIBLTE_RRC_CON_REQ_EST_CAUSE_SPARE1,
    LIBLTE_RRC_CON_REQ_EST_CAUSE_N_ITEMS,
}LIBLTE_RRC_CON_REQ_EST_CAUSE_ENUM;
static const char liblte_rrc_con_req_est_cause_text[LIBLTE_RRC_CON_REQ_EST_CAUSE_N_ITEMS][100] = {"Emergency",
                                                                                                  "High Priority Access",
                                                                                                  "MT Access",
                                                                                                  "MO Signalling",
                                                                                                  "MO Data",
                                                                                                  "SPARE",
                                                                                                  "SPARE",
                                                                                                  "SPARE"};
// Structs
typedef union{
    LIBLTE_RRC_S_TMSI_STRUCT s_tmsi;
    uint64                   random;
}LIBLTE_RRC_CON_REQ_UE_ID_UNION;
typedef struct{
    LIBLTE_RRC_CON_REQ_UE_ID_UNION     ue_id;
    LIBLTE_RRC_CON_REQ_UE_ID_TYPE_ENUM ue_id_type;
    LIBLTE_RRC_CON_REQ_EST_CAUSE_ENUM  cause;
}LIBLTE_RRC_CONNECTION_REQUEST_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_request_msg(LIBLTE_RRC_CONNECTION_REQUEST_STRUCT *con_req,
                                                             LIBLTE_BIT_MSG_STRUCT                *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_request_msg(LIBLTE_BIT_MSG_STRUCT                *msg,
                                                               LIBLTE_RRC_CONNECTION_REQUEST_STRUCT *con_req);

/*********************************************************************
    Message Name: RRC Connection Release

    Description: Used to command the release of an RRC connection

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_RELEASE_CAUSE_LOAD_BALANCING_TAU_REQUIRED = 0,
    LIBLTE_RRC_RELEASE_CAUSE_OTHER,
    LIBLTE_RRC_RELEASE_CAUSE_CS_FALLBACK_HIGH_PRIORITY,
    LIBLTE_RRC_RELEASE_CAUSE_SPARE1,
    LIBLTE_RRC_RELEASE_CAUSE_N_ITEMS,
}LIBLTE_RRC_RELEASE_CAUSE_ENUM;
static const char liblte_rrc_release_cause_text[LIBLTE_RRC_RELEASE_CAUSE_N_ITEMS][100] = {"Load Balancing TAU Required",
                                                                                          "Other",
                                                                                          "CS Fallback High Priority",
                                                                                          "SPARE"};
// Structs
typedef struct{
    LIBLTE_RRC_RELEASE_CAUSE_ENUM release_cause;
    uint8                         rrc_transaction_id;
}LIBLTE_RRC_CONNECTION_RELEASE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_release_msg(LIBLTE_RRC_CONNECTION_RELEASE_STRUCT *con_release,
                                                             LIBLTE_BIT_MSG_STRUCT                *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_release_msg(LIBLTE_BIT_MSG_STRUCT                *msg,
                                                               LIBLTE_RRC_CONNECTION_RELEASE_STRUCT *con_release);

/*********************************************************************
    Message Name: RRC Connection Reject

    Description: Used to reject the RRC connection establishment

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 wait_time;
}LIBLTE_RRC_CONNECTION_REJECT_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reject_msg(LIBLTE_RRC_CONNECTION_REJECT_STRUCT *con_rej,
                                                            LIBLTE_BIT_MSG_STRUCT               *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reject_msg(LIBLTE_BIT_MSG_STRUCT               *msg,
                                                              LIBLTE_RRC_CONNECTION_REJECT_STRUCT *con_rej);

/*********************************************************************
    Message Name: RRC Connection Reestablishment Request

    Description: Used to request the reestablishment of an RRC
                 connection

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_CON_REEST_REQ_CAUSE_RECONFIG_FAILURE = 0,
    LIBLTE_RRC_CON_REEST_REQ_CAUSE_HANDOVER_FAILURE,
    LIBLTE_RRC_CON_REEST_REQ_CAUSE_OTHER_FAILURE,
    LIBLTE_RRC_CON_REEST_REQ_CAUSE_SPARE1,
    LIBLTE_RRC_CON_REEST_REQ_CAUSE_N_ITEMS,
}LIBLTE_RRC_CON_REEST_REQ_CAUSE_ENUM;
static const char liblte_rrc_con_reest_req_cause_text[LIBLTE_RRC_CON_REEST_REQ_CAUSE_N_ITEMS][100] = {"Reconfiguration Failure",
                                                                                                      "Handover Failure",
                                                                                                      "Other Failure",
                                                                                                      "SPARE"};
// Structs
typedef struct{
    uint16 c_rnti;
    uint16 phys_cell_id;
    uint16 short_mac_i;
}LIBLTE_RRC_CON_REEST_REQ_UE_ID_STRUCT;
typedef struct{
    LIBLTE_RRC_CON_REEST_REQ_UE_ID_STRUCT ue_id;
    LIBLTE_RRC_CON_REEST_REQ_CAUSE_ENUM   cause;
}LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REQUEST_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reestablishment_request_msg(LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REQUEST_STRUCT *con_reest_req,
                                                                             LIBLTE_BIT_MSG_STRUCT                                *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reestablishment_request_msg(LIBLTE_BIT_MSG_STRUCT                                *msg,
                                                                               LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REQUEST_STRUCT *con_reest_req);

/*********************************************************************
    Message Name: RRC Connection Reestablishment Reject

    Description: Used to indicate the rejection of an RRC connection
                 reestablishment request

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
}LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REJECT_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reestablishment_reject_msg(LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REJECT_STRUCT *con_reest_rej,
                                                                            LIBLTE_BIT_MSG_STRUCT                               *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reestablishment_reject_msg(LIBLTE_BIT_MSG_STRUCT                               *msg,
                                                                              LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REJECT_STRUCT *con_reest_rej);

/*********************************************************************
    Message Name: RRC Connection Reestablishment Complete

    Description: Used to confirm the successful completion of an RRC
                 connection reestablishment

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 rrc_transaction_id;
}LIBLTE_RRC_CONNECTION_REESTABLISHMENT_COMPLETE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reestablishment_complete_msg(LIBLTE_RRC_CONNECTION_REESTABLISHMENT_COMPLETE_STRUCT *con_reest_complete,
                                                                              LIBLTE_BIT_MSG_STRUCT                                 *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reestablishment_complete_msg(LIBLTE_BIT_MSG_STRUCT                                 *msg,
                                                                                LIBLTE_RRC_CONNECTION_REESTABLISHMENT_COMPLETE_STRUCT *con_reest_complete);

/*********************************************************************
    Message Name: RRC Connection Reestablishment

    Description: Used to resolve contention and to re-establish SRB1

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT rr_cnfg;
    uint8                                 rrc_transaction_id;
    uint8                                 next_hop_chaining_count;
}LIBLTE_RRC_CONNECTION_REESTABLISHMENT_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reestablishment_msg(LIBLTE_RRC_CONNECTION_REESTABLISHMENT_STRUCT *con_reest,
                                                                     LIBLTE_BIT_MSG_STRUCT                        *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reestablishment_msg(LIBLTE_BIT_MSG_STRUCT                        *msg,
                                                                       LIBLTE_RRC_CONNECTION_REESTABLISHMENT_STRUCT *con_reest);

/*********************************************************************
    Message Name: RRC Connection Reconfiguration Complete

    Description: Used to confirm the successful completion of an RRC
                 connection reconfiguration

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 rrc_transaction_id;
}LIBLTE_RRC_CONNECTION_RECONFIGURATION_COMPLETE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reconfiguration_complete_msg(LIBLTE_RRC_CONNECTION_RECONFIGURATION_COMPLETE_STRUCT *con_reconfig_complete,
                                                                              LIBLTE_BIT_MSG_STRUCT                                 *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reconfiguration_complete_msg(LIBLTE_BIT_MSG_STRUCT                                 *msg,
                                                                                LIBLTE_RRC_CONNECTION_RECONFIGURATION_COMPLETE_STRUCT *con_reconfig_complete);

/*********************************************************************
    Message Name: RRC Connection Reconfiguration

    Description: Modifies an RRC connection

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_HANDOVER_TYPE_INTRA_LTE = 0,
    LIBLTE_RRC_HANDOVER_TYPE_INTER_RAT,
    LIBLTE_RRC_HANDOVER_TYPE_N_ITEMS,
}LIBLTE_RRC_HANDOVER_TYPE_ENUM;
static const char liblte_rrc_handover_type_text[LIBLTE_RRC_HANDOVER_TYPE_N_ITEMS][20] = {"Intra LTE",
                                                                                         "Inter RAT"};
// Structs
typedef struct{
    LIBLTE_RRC_SECURITY_ALGORITHM_CONFIG_STRUCT sec_alg_cnfg;
    uint8                                       next_hop_chaining_count;
    bool                                        key_change_ind;
    bool                                        sec_alg_cnfg_present;
}LIBLTE_RRC_INTRA_LTE_HANDOVER_STRUCT;
typedef struct{
    LIBLTE_RRC_SECURITY_ALGORITHM_CONFIG_STRUCT sec_alg_cnfg;
    uint8                                       nas_sec_param_to_eutra[6];
}LIBLTE_RRC_INTER_RAT_HANDOVER_STRUCT;
typedef struct{
    LIBLTE_RRC_INTRA_LTE_HANDOVER_STRUCT intra_lte;
    LIBLTE_RRC_INTER_RAT_HANDOVER_STRUCT inter_rat;
    LIBLTE_RRC_HANDOVER_TYPE_ENUM        ho_type;
}LIBLTE_RRC_SECURITY_CONFIG_HO_STRUCT;
typedef struct{
    LIBLTE_RRC_MEAS_CONFIG_STRUCT           meas_cnfg;
    LIBLTE_RRC_MOBILITY_CONTROL_INFO_STRUCT mob_ctrl_info;
    LIBLTE_SIMPLE_BYTE_MSG_STRUCT           ded_info_nas_list[LIBLTE_RRC_MAX_DRB];
    LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT   rr_cnfg_ded;
    LIBLTE_RRC_SECURITY_CONFIG_HO_STRUCT    sec_cnfg_ho;
    uint32                                  N_ded_info_nas;
    uint8                                   rrc_transaction_id;
    bool                                    meas_cnfg_present;
    bool                                    mob_ctrl_info_present;
    bool                                    rr_cnfg_ded_present;
    bool                                    sec_cnfg_ho_present;
}LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rrc_connection_reconfiguration_msg(LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT *con_reconfig,
                                                                     LIBLTE_BIT_MSG_STRUCT                        *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rrc_connection_reconfiguration_msg(LIBLTE_BIT_MSG_STRUCT                        *msg,
                                                                       LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT *con_reconfig);

/*********************************************************************
    Message Name: RN Reconfiguration Complete

    Description: Used to confirm the successful completion of an RN
                 reconfiguration

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    uint8 rrc_transaction_id;
}LIBLTE_RRC_RN_RECONFIGURATION_COMPLETE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rn_reconfiguration_complete_msg(LIBLTE_RRC_RN_RECONFIGURATION_COMPLETE_STRUCT *rn_reconfig_complete,
                                                                  LIBLTE_BIT_MSG_STRUCT                         *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rn_reconfiguration_complete_msg(LIBLTE_BIT_MSG_STRUCT                         *msg,
                                                                    LIBLTE_RRC_RN_RECONFIGURATION_COMPLETE_STRUCT *rn_reconfig_complete);

/*********************************************************************
    Message Name: RN Reconfiguration

    Description: Modifies the RRC connection between the RN and the
                 E-UTRAN

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    // FIXME
}LIBLTE_RRC_RN_RECONFIGURATION_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_rn_reconfiguration_msg(LIBLTE_RRC_RN_RECONFIGURATION_STRUCT *rn_reconfig,
                                                         LIBLTE_BIT_MSG_STRUCT                *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_rn_reconfiguration_msg(LIBLTE_BIT_MSG_STRUCT                *msg,
                                                           LIBLTE_RRC_RN_RECONFIGURATION_STRUCT *rn_reconfig);

/*********************************************************************
    Message Name: Proximity Indication

    Description: Used to indicate that the UE is entering or leaving
                 the proximity of one or more cells whose CSG IDs are
                 in the UEs CSG whitelist

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_PROXIMITY_INDICATION_TYPE_ENTERING = 0,
    LIBLTE_RRC_PROXIMITY_INDICATION_TYPE_LEAVING,
    LIBLTE_RRC_PROXIMITY_INDICATION_TYPE_N_ITEMS,
}LIBLTE_RRC_PROXIMITY_INDICATION_TYPE_ENUM;
static const char liblte_rrc_proximity_indication_type_text[LIBLTE_RRC_PROXIMITY_INDICATION_TYPE_N_ITEMS][20] = {"Entering", "Leaving"};
typedef enum{
    LIBLTE_RRC_PROXIMITY_INDICATION_CARRIER_FREQ_TYPE_EUTRA = 0,
    LIBLTE_RRC_PROXIMITY_INDICATION_CARRIER_FREQ_TYPE_UTRA,
    LIBLTE_RRC_PROXIMITY_INDICATION_CARRIER_FREQ_TYPE_N_ITEMS,
}LIBLTE_RRC_PROXIMITY_INDICATION_CARRIER_FREQ_TYPE_ENUM;
static const char liblte_rrc_proximity_indication_carrier_freq_type_text[LIBLTE_RRC_PROXIMITY_INDICATION_CARRIER_FREQ_TYPE_N_ITEMS][20] = {"EUTRA", "UTRA"};
// Structs
typedef struct{
    LIBLTE_RRC_PROXIMITY_INDICATION_TYPE_ENUM              type;
    LIBLTE_RRC_PROXIMITY_INDICATION_CARRIER_FREQ_TYPE_ENUM carrier_freq_type;
    uint16                                                 carrier_freq;
}LIBLTE_RRC_PROXIMITY_INDICATION_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_proximity_indication_msg(LIBLTE_RRC_PROXIMITY_INDICATION_STRUCT *proximity_ind,
                                                           LIBLTE_BIT_MSG_STRUCT                  *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_proximity_indication_msg(LIBLTE_BIT_MSG_STRUCT                  *msg,
                                                             LIBLTE_RRC_PROXIMITY_INDICATION_STRUCT *proximity_ind);

/*********************************************************************
    Message Name: Paging

    Description: Used for the notification of one or more UEs

    Document Reference: 36.331 v10.0.0 Section 6.2.2
*********************************************************************/
// Defines
#define LIBLTE_RRC_MAX_PAGE_REC 16
// Enums
typedef enum{
    LIBLTE_RRC_PAGING_UE_IDENTITY_TYPE_S_TMSI = 0,
    LIBLTE_RRC_PAGING_UE_IDENTITY_TYPE_IMSI,
    LIBLTE_RRC_PAGING_UE_IDENTITY_TYPE_N_ITEMS,
}LIBLTE_RRC_PAGING_UE_IDENTITY_TYPE_ENUM;
static const char liblte_rrc_paging_ue_identity_type_text[LIBLTE_RRC_PAGING_UE_IDENTITY_TYPE_N_ITEMS][20] = {"S-TMSI", "IMSI"};
typedef enum{
    LIBLTE_RRC_CN_DOMAIN_PS = 0,
    LIBLTE_RRC_CN_DOMAIN_CS,
    LIBLTE_RRC_CN_DOMAIN_N_ITEMS,
}LIBLTE_RRC_CN_DOMAIN_ENUM;
static const char liblte_rrc_cn_domain_text[LIBLTE_RRC_CN_DOMAIN_N_ITEMS][20] = {"PS", "CS"};
typedef enum{
    LIBLTE_RRC_CMAS_INDICATION_R9_TRUE = 0,
    LIBLTE_RRC_CMAS_INDICATION_R9_N_ITEMS,
}LIBLTE_RRC_CMAS_INDICATION_R9_ENUM;
static const char liblte_rrc_cmas_indication_r9_text[LIBLTE_RRC_CMAS_INDICATION_R9_N_ITEMS][20] = {"TRUE"};
typedef enum{
    LIBLTE_RRC_SYSTEM_INFO_MODIFICATION_TRUE = 0,
    LIBLTE_RRC_SYSTEM_INFO_MODIFICATION_N_ITEMS,
}LIBLTE_RRC_SYSTEM_INFO_MODIFICATION_ENUM;
static const char liblte_rrc_system_info_modification_text[LIBLTE_RRC_SYSTEM_INFO_MODIFICATION_N_ITEMS][20] = {"TRUE"};
typedef enum{
    LIBLTE_RRC_ETWS_INDICATION_TRUE = 0,
    LIBLTE_RRC_ETWS_INDICATION_N_ITEMS,
}LIBLTE_RRC_ETWS_INDICATION_ENUM;
static const char liblte_rrc_etws_indication_text[LIBLTE_RRC_ETWS_INDICATION_N_ITEMS][20] = {"TRUE"};
// Structs
typedef struct{
    LIBLTE_RRC_S_TMSI_STRUCT                s_tmsi;
    LIBLTE_RRC_PAGING_UE_IDENTITY_TYPE_ENUM ue_identity_type;
    uint32                                  imsi_size;
    uint8                                   imsi[21];
}LIBLTE_RRC_PAGING_UE_IDENTITY_STRUCT;
typedef struct{
    LIBLTE_RRC_PAGING_UE_IDENTITY_STRUCT ue_identity;
    LIBLTE_RRC_CN_DOMAIN_ENUM            cn_domain;
}LIBLTE_RRC_PAGING_RECORD_STRUCT;
typedef struct{
    LIBLTE_RRC_CMAS_INDICATION_R9_ENUM cmas_ind_r9;
    bool                               cmas_ind_present;
    bool                               non_crit_ext_present;
}LIBLTE_RRC_PAGING_V920_IES_STRUCT;
typedef struct{
    LIBLTE_RRC_PAGING_V920_IES_STRUCT non_crit_ext;
    uint8                             late_non_crit_ext;
    bool                              late_non_crit_ext_present;
    bool                              non_crit_ext_present;
}LIBLTE_RRC_PAGING_V890_IES_STRUCT;
typedef struct{
    LIBLTE_RRC_PAGING_RECORD_STRUCT          paging_record_list[LIBLTE_RRC_MAX_PAGE_REC];
    LIBLTE_RRC_PAGING_V890_IES_STRUCT        non_crit_ext;
    LIBLTE_RRC_SYSTEM_INFO_MODIFICATION_ENUM system_info_modification;
    LIBLTE_RRC_ETWS_INDICATION_ENUM          etws_indication;
    uint32                                   paging_record_list_size;
    bool                                     system_info_modification_present;
    bool                                     etws_indication_present;
    bool                                     non_crit_ext_present;
}LIBLTE_RRC_PAGING_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_paging_msg(LIBLTE_RRC_PAGING_STRUCT *page,
                                             LIBLTE_BIT_MSG_STRUCT    *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_paging_msg(LIBLTE_BIT_MSG_STRUCT    *msg,
                                               LIBLTE_RRC_PAGING_STRUCT *page);

/*********************************************************************
    Message Name: Mobility From EUTRA Command

    Description: Used to command handover or a cell change from E-UTRA
                 to another RAT, or enhanced CS fallback to CDMA2000
                 1xRTT

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    // FIXME
}LIBLTE_RRC_MOBILITY_FROM_EUTRA_COMMAND_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_mobility_from_eutra_command_msg(LIBLTE_RRC_MOBILITY_FROM_EUTRA_COMMAND_STRUCT *mobility_from_eutra_cmd,
                                                                  LIBLTE_BIT_MSG_STRUCT                         *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_mobility_from_eutra_command_msg(LIBLTE_BIT_MSG_STRUCT                         *msg,
                                                                    LIBLTE_RRC_MOBILITY_FROM_EUTRA_COMMAND_STRUCT *mobility_from_eutra_cmd);

/*********************************************************************
    Message Name: Measurement Report

    Description: Used for the indication of measurement results

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
typedef struct{
    LIBLTE_RRC_CELL_GLOBAL_ID_EUTRA_STRUCT  cell_global_id;
    uint16                                  tracking_area_code;
    LIBLTE_RRC_PLMN_IDENTITY_STRUCT         plmn_identity_list[5];
    uint32                                  n_plmn_identity_list;
    bool                                    have_plmn_identity_list;
}LIBLTE_RRC_CGI_INFO_STRUCT;
LIBLTE_ERROR_ENUM liblte_rrc_pack_cgi_info_ie(LIBLTE_RRC_CGI_INFO_STRUCT  *cgi_info,
                                              uint8                      **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_cgi_info_ie(uint8                      **ie_ptr,
                                                LIBLTE_RRC_CGI_INFO_STRUCT  *cgi_info);

typedef struct{
    uint8 rsrp_result;
    bool  have_rsrp;
    uint8 rsrq_result;
    bool  have_rsrq;
}LIBLTE_RRC_MEAS_RESULT_STRUCT;
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_result_ie(LIBLTE_RRC_MEAS_RESULT_STRUCT  *meas_result,
                                                 uint8                         **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_result_ie(uint8                         **ie_ptr,
                                                   LIBLTE_RRC_MEAS_RESULT_STRUCT  *meas_result);

typedef struct{
    uint16                                  phys_cell_id;
    LIBLTE_RRC_CGI_INFO_STRUCT              cgi_info;
    bool                                    have_cgi_info;
    LIBLTE_RRC_MEAS_RESULT_STRUCT           meas_result;
}LIBLTE_RRC_MEAS_RESULT_EUTRA_STRUCT;
LIBLTE_ERROR_ENUM liblte_rrc_pack_meas_result_eutra_ie(LIBLTE_RRC_MEAS_RESULT_EUTRA_STRUCT  *meas_result_eutra,
                                                       uint8                               **ie_ptr);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_meas_result_eutra_ie(uint8                               **ie_ptr,
                                                         LIBLTE_RRC_MEAS_RESULT_EUTRA_STRUCT  *meas_result_eutra);

typedef struct{
  //FIXME
}LIBLTE_RRC_MEAS_RESULT_UTRA_STRUCT;
typedef struct{
  //FIXME
}LIBLTE_RRC_MEAS_RESULT_GERAN_STRUCT;
typedef struct{
  //FIXME
}LIBLTE_RRC_MEAS_RESULT_CDMA2000_STRUCT;



typedef struct{
    LIBLTE_RRC_MEAS_RESULT_EUTRA_STRUCT     result_eutra_list[8];
    uint8                                   n_result;
}LIBLTE_RRC_MEAS_RESULT_LIST_EUTRA_STRUCT;

typedef struct{
    LIBLTE_RRC_MEAS_RESULT_UTRA_STRUCT     result_utra_list[8];
    uint8                                   n_result;
}LIBLTE_RRC_MEAS_RESULT_LIST_UTRA_STRUCT;

typedef struct{
    LIBLTE_RRC_MEAS_RESULT_GERAN_STRUCT     result_geran_list[8];
    uint8                                   n_result;
}LIBLTE_RRC_MEAS_RESULT_LIST_GERAN_STRUCT;


typedef struct{
    bool pre_registration_status_HRPD;
    LIBLTE_RRC_MEAS_RESULT_CDMA2000_STRUCT cdma2000[8];
}LIBLTE_RRC_MEAS_RESULTS_CDMA2000_STRUCT;

typedef enum{
    LIBLTE_RRC_MEAS_RESULT_LIST_EUTRA = 0,
    LIBLTE_RRC_MEAS_RESULT_LIST_UTRA,
    LIBLTE_RRC_MEAS_RESULT_LIST_GERAN,
    LIBLTE_RRC_MEAS_RESULTS_CDMA2000,
    LIBLTE_RRC_MEAS_RESULT_N_ITEMS,
}LIBLTE_RRC_MEAS_RESULT_NEIGH_CELLS_ENUM;
static const char liblte_rrc_meas_reult_neigh_cells_text[LIBLTE_RRC_MEAS_RESULT_N_ITEMS][32] = { "measResultListEUTRA",  "measResultListUTRA",  "measResultListGERAN",  "measResultsCDMA2000"};


typedef union{
    LIBLTE_RRC_MEAS_RESULT_LIST_EUTRA_STRUCT     eutra;
    LIBLTE_RRC_MEAS_RESULT_LIST_UTRA_STRUCT      utra;
    LIBLTE_RRC_MEAS_RESULT_LIST_GERAN_STRUCT     geran;
    LIBLTE_RRC_MEAS_RESULTS_CDMA2000_STRUCT cdma2000;
}LIBLTE_RRC_MEAS_RESULT_NEIGH_CELLS_UNION;

//TODO: pack/unpack for the result lists


// Structs
typedef struct{
    uint8   meas_id;
    uint8   pcell_rsrp_result;
    uint8   pcell_rsrq_result;

    LIBLTE_RRC_MEAS_RESULT_NEIGH_CELLS_UNION  meas_result_neigh_cells;
    LIBLTE_RRC_MEAS_RESULT_NEIGH_CELLS_ENUM   meas_result_neigh_cells_choice;
    bool                                      have_meas_result_neigh_cells;
}LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_measurement_report_msg(LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT *meas_report,
                                                         LIBLTE_BIT_MSG_STRUCT                *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_measurement_report_msg(LIBLTE_BIT_MSG_STRUCT                *msg,
                                                           LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT *meas_report);

/*********************************************************************
    Message Name: MBSFN Area Configuration

    Description: Contains the MBMS control information applicable for
                 an MBSFN area

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    Message Name: Master Information Block

    Description: Includes the system information transmitted on BCH

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Inlined with BCCH BCH Message

/*********************************************************************
    Message Name: Logged Measurements Configuration

    Description: Used by E-UTRAN to configure the UE to perform
                 logging of measurement results while in RRC_IDLE

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    // FIXME
}LIBLTE_RRC_LOGGED_MEASUREMENTS_CONFIGURATION_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_logged_measurements_configuration_msg(LIBLTE_RRC_LOGGED_MEASUREMENTS_CONFIGURATION_STRUCT *logged_measurements_config,
                                                                        LIBLTE_BIT_MSG_STRUCT                               *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_logged_measurements_configuration_msg(LIBLTE_BIT_MSG_STRUCT                               *msg,
                                                                          LIBLTE_RRC_LOGGED_MEASUREMENTS_CONFIGURATION_STRUCT *logged_measurements_config);

/*********************************************************************
    Message Name: Handover From EUTRA Preparation Request (CDMA2000)

    Description: Used to trigger the handover preparation procedure
                 with a CDMA2000 RAT

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    // FIXME
}LIBLTE_RRC_HANDOVER_FROM_EUTRA_PREPARATION_REQUEST_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_handover_from_eutra_preparation_request_msg(LIBLTE_RRC_HANDOVER_FROM_EUTRA_PREPARATION_REQUEST_STRUCT *handover_from_eutra_prep_req,
                                                                              LIBLTE_BIT_MSG_STRUCT                                     *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_handover_from_eutra_preparation_request_msg(LIBLTE_BIT_MSG_STRUCT                                     *msg,
                                                                                LIBLTE_RRC_HANDOVER_FROM_EUTRA_PREPARATION_REQUEST_STRUCT *handover_from_eutra_prep_req);

/*********************************************************************
    Message Name: DL Information Transfer

    Description: Used for the downlink transfer of dedicated NAS
                 information

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_DL_INFORMATION_TRANSFER_TYPE_NAS = 0,
    LIBLTE_RRC_DL_INFORMATION_TRANSFER_TYPE_CDMA2000_1XRTT,
    LIBLTE_RRC_DL_INFORMATION_TRANSFER_TYPE_CDMA2000_HRPD,
    LIBLTE_RRC_DL_INFORMATION_TRANSFER_TYPE_N_ITEMS,
}LIBLTE_RRC_DL_INFORMATION_TRANSFER_TYPE_ENUM;
static const char liblte_rrc_dl_information_transfer_type_text[LIBLTE_RRC_DL_INFORMATION_TRANSFER_TYPE_N_ITEMS][20] = {"NAS",
                                                                                                                       "CDMA2000-1XRTT",
                                                                                                                       "CDMA2000-HRPD"};
// Structs
typedef struct{
    LIBLTE_SIMPLE_BYTE_MSG_STRUCT                dedicated_info;
    LIBLTE_RRC_DL_INFORMATION_TRANSFER_TYPE_ENUM dedicated_info_type;
    uint8                                        rrc_transaction_id;
}LIBLTE_RRC_DL_INFORMATION_TRANSFER_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_dl_information_transfer_msg(LIBLTE_RRC_DL_INFORMATION_TRANSFER_STRUCT *dl_info_transfer,
                                                              LIBLTE_BIT_MSG_STRUCT                     *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_dl_information_transfer_msg(LIBLTE_BIT_MSG_STRUCT                     *msg,
                                                                LIBLTE_RRC_DL_INFORMATION_TRANSFER_STRUCT *dl_info_transfer);

/*********************************************************************
    Message Name: CSFB Parameters Response CDMA2000

    Description: Used to provide the CDMA2000 1xRTT parameters to the
                 UE so the UE can register with the CDMA2000 1xRTT
                 network to support CSFB to CDMA2000 1xRTT

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    // FIXME
}LIBLTE_RRC_CSFB_PARAMETERS_RESPONSE_CDMA2000_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_csfb_parameters_response_cdma2000_msg(LIBLTE_RRC_CSFB_PARAMETERS_RESPONSE_CDMA2000_STRUCT *csfb_params_resp_cdma2000,
                                                                        LIBLTE_BIT_MSG_STRUCT                               *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_csfb_parameters_response_cdma2000_msg(LIBLTE_BIT_MSG_STRUCT                               *msg,
                                                                          LIBLTE_RRC_CSFB_PARAMETERS_RESPONSE_CDMA2000_STRUCT *csfb_params_resp_cdma2000);

/*********************************************************************
    Message Name: CSFB Parameters Request CDMA2000

    Description: Used by the UE to obtain the CDMA2000 1xRTT
                 parameters from the network

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
}LIBLTE_RRC_CSFB_PARAMETERS_REQUEST_CDMA2000_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_csfb_parameters_request_cdma2000_msg(LIBLTE_RRC_CSFB_PARAMETERS_REQUEST_CDMA2000_STRUCT *csfb_params_req_cdma2000,
                                                                       LIBLTE_BIT_MSG_STRUCT                              *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_csfb_parameters_request_cdma2000_msg(LIBLTE_BIT_MSG_STRUCT                              *msg,
                                                                         LIBLTE_RRC_CSFB_PARAMETERS_REQUEST_CDMA2000_STRUCT *csfb_params_req_cdma2000);

/*********************************************************************
    Message Name: Counter Check Response

    Description: Used by the UE to respond to a Counter Check message

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    // FIXME
}LIBLTE_RRC_COUNTER_CHECK_RESPONSE_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_counter_check_response_msg(LIBLTE_RRC_COUNTER_CHECK_RESPONSE_STRUCT *counter_check_resp,
                                                             LIBLTE_BIT_MSG_STRUCT                    *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_counter_check_response_msg(LIBLTE_BIT_MSG_STRUCT                    *msg,
                                                               LIBLTE_RRC_COUNTER_CHECK_RESPONSE_STRUCT *counter_check_resp);

/*********************************************************************
    Message Name: Counter Check

    Description: Used by the E-UTRAN to indicate the current COUNT MSB
                 values associated to each DRB and to request the UE
                 to compare these to its COUNT MSB values and to
                 report the comparison results to E-UTRAN

    Document Reference: 36.331 v10.0.0 Section 6.2.2 
*********************************************************************/
// Defines
// Enums
// Structs
typedef struct{
    // FIXME
}LIBLTE_RRC_COUNTER_CHECK_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_counter_check_msg(LIBLTE_RRC_COUNTER_CHECK_STRUCT *counter_check,
                                                    LIBLTE_BIT_MSG_STRUCT           *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_counter_check_msg(LIBLTE_BIT_MSG_STRUCT           *msg,
                                                      LIBLTE_RRC_COUNTER_CHECK_STRUCT *counter_check);

/*********************************************************************
    Message Name: BCCH BCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the E-UTRAN to the UE via BCH on the BCCH
                 logical channel

    Document Reference: 36.331 v10.0.0 Sections 6.2.1 and 6.2.2
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_DL_BANDWIDTH_6 = 0,
    LIBLTE_RRC_DL_BANDWIDTH_15,
    LIBLTE_RRC_DL_BANDWIDTH_25,
    LIBLTE_RRC_DL_BANDWIDTH_50,
    LIBLTE_RRC_DL_BANDWIDTH_75,
    LIBLTE_RRC_DL_BANDWIDTH_100,
    LIBLTE_RRC_DL_BANDWIDTH_N_ITEMS,
}LIBLTE_RRC_DL_BANDWIDTH_ENUM;
static const char liblte_rrc_dl_bandwidth_text[LIBLTE_RRC_DL_BANDWIDTH_N_ITEMS][20] = {"1.4",   "3",   "5",  "10",
                                                                                        "15",  "20"};
static const double liblte_rrc_dl_bandwidth_num[LIBLTE_RRC_DL_BANDWIDTH_N_ITEMS] = {1.4, 3, 5, 10, 15, 20};
// Structs
typedef struct{
    LIBLTE_RRC_PHICH_CONFIG_STRUCT phich_config;
    LIBLTE_RRC_DL_BANDWIDTH_ENUM   dl_bw;
    uint8                          sfn_div_4;
}LIBLTE_RRC_MIB_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_bcch_bch_msg(LIBLTE_RRC_MIB_STRUCT *mib,
                                               LIBLTE_BIT_MSG_STRUCT *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_bcch_bch_msg(LIBLTE_BIT_MSG_STRUCT *msg,
                                                 LIBLTE_RRC_MIB_STRUCT *mib);

/*********************************************************************
    Message Name: BCCH DLSCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the E-UTRAN to the UE via DLSCH on the BCCH
                 logical channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
// Defines
// Enums
// Structs
typedef LIBLTE_RRC_SYS_INFO_MSG_STRUCT LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_bcch_dlsch_msg(LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT *bcch_dlsch_msg,
                                                 LIBLTE_BIT_MSG_STRUCT            *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_bcch_dlsch_msg(LIBLTE_BIT_MSG_STRUCT            *msg,
                                                   LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT *bcch_dlsch_msg);

/*********************************************************************
    Message Name: MCCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the E-UTRAN to the UE on the MCCH logical
                 channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
// FIXME

/*********************************************************************
    Message Name: PCCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the E-UTRAN to the UE on the PCCH logical
                 channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
// Defines
// Enums
// Structs
typedef LIBLTE_RRC_PAGING_STRUCT LIBLTE_RRC_PCCH_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_pcch_msg(LIBLTE_RRC_PCCH_MSG_STRUCT *pcch_msg,
                                           LIBLTE_BIT_MSG_STRUCT      *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_pcch_msg(LIBLTE_BIT_MSG_STRUCT      *msg,
                                             LIBLTE_RRC_PCCH_MSG_STRUCT *pcch_msg);

/*********************************************************************
    Message Name: DL CCCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the E-UTRAN to the UE on the CCCH logical
                 channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REEST = 0,
    LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REEST_REJ,
    LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REJ,
    LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_SETUP,
    LIBLTE_RRC_DL_CCCH_MSG_TYPE_N_ITEMS,
}LIBLTE_RRC_DL_CCCH_MSG_TYPE_ENUM;
static const char liblte_rrc_dl_ccch_msg_type_text[LIBLTE_RRC_DL_CCCH_MSG_TYPE_N_ITEMS][100] = {"RRC Connection Reestablishment",
                                                                                                "RRC Connection Reestablishment Reject",
                                                                                                "RRC Connection Reject",
                                                                                                "RRC Connection Setup"};
// Structs
typedef union{
    LIBLTE_RRC_CONNECTION_REESTABLISHMENT_STRUCT        rrc_con_reest;
    LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REJECT_STRUCT rrc_con_reest_rej;
    LIBLTE_RRC_CONNECTION_REJECT_STRUCT                 rrc_con_rej;
    LIBLTE_RRC_CONNECTION_SETUP_STRUCT                  rrc_con_setup;
}LIBLTE_RRC_DL_CCCH_MSG_TYPE_UNION;
typedef struct{
    LIBLTE_RRC_DL_CCCH_MSG_TYPE_UNION msg;
    LIBLTE_RRC_DL_CCCH_MSG_TYPE_ENUM  msg_type;
}LIBLTE_RRC_DL_CCCH_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_dl_ccch_msg(LIBLTE_RRC_DL_CCCH_MSG_STRUCT *dl_ccch_msg,
                                              LIBLTE_BIT_MSG_STRUCT         *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_dl_ccch_msg(LIBLTE_BIT_MSG_STRUCT         *msg,
                                                LIBLTE_RRC_DL_CCCH_MSG_STRUCT *dl_ccch_msg);

/*********************************************************************
    Message Name: DL DCCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the E-UTRAN to the UE on the DCCH logical
                 channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_CSFB_PARAMS_RESP_CDMA2000 = 0,
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_DL_INFO_TRANSFER,
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_HANDOVER_FROM_EUTRA_PREP_REQ,
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_MOBILITY_FROM_EUTRA_COMMAND,
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RECONFIG,
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RELEASE,
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_SECURITY_MODE_COMMAND,
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_UE_CAPABILITY_ENQUIRY,
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_COUNTER_CHECK,
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_UE_INFO_REQ,
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_LOGGED_MEASUREMENTS_CONFIG,
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_RN_RECONFIG,
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_N_ITEMS,
}LIBLTE_RRC_DL_DCCH_MSG_TYPE_ENUM;
static const char liblte_rrc_dl_dcch_msg_type_text[LIBLTE_RRC_DL_DCCH_MSG_TYPE_N_ITEMS][100] = {"CSFB Parameters Response CDMA2000",
                                                                                                "DL Information Transfer",
                                                                                                "Handover From EUTRA Preparation Request",
                                                                                                "Mobility From EUTRA Command",
                                                                                                "RRC Connection Reconfiguration",
                                                                                                "RRC Connection Release",
                                                                                                "Security Mode Command",
                                                                                                "UE Capability Enquiry",
                                                                                                "Counter Check",
                                                                                                "UE Information Request",
                                                                                                "Logged Measurements Configuration",
                                                                                                "RN Reconfiguration"};
// Structs
typedef union{
    LIBLTE_RRC_CSFB_PARAMETERS_RESPONSE_CDMA2000_STRUCT       csfb_params_resp_cdma2000;
    LIBLTE_RRC_DL_INFORMATION_TRANSFER_STRUCT                 dl_info_transfer;
    LIBLTE_RRC_HANDOVER_FROM_EUTRA_PREPARATION_REQUEST_STRUCT handover_from_eutra_prep_req;
    LIBLTE_RRC_MOBILITY_FROM_EUTRA_COMMAND_STRUCT             mobility_from_eutra_cmd;
    LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT              rrc_con_reconfig;
    LIBLTE_RRC_CONNECTION_RELEASE_STRUCT                      rrc_con_release;
    LIBLTE_RRC_SECURITY_MODE_COMMAND_STRUCT                   security_mode_cmd;
    LIBLTE_RRC_UE_CAPABILITY_ENQUIRY_STRUCT                   ue_cap_enquiry;
    LIBLTE_RRC_COUNTER_CHECK_STRUCT                           counter_check;
    LIBLTE_RRC_UE_INFORMATION_REQUEST_STRUCT                  ue_info_req;
    LIBLTE_RRC_LOGGED_MEASUREMENTS_CONFIGURATION_STRUCT       logged_measurements_config;
    LIBLTE_RRC_RN_RECONFIGURATION_STRUCT                      rn_reconfig;
}LIBLTE_RRC_DL_DCCH_MSG_TYPE_UNION;
typedef struct{
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_UNION msg;
    LIBLTE_RRC_DL_DCCH_MSG_TYPE_ENUM  msg_type;
}LIBLTE_RRC_DL_DCCH_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_dl_dcch_msg(LIBLTE_RRC_DL_DCCH_MSG_STRUCT *dl_dcch_msg,
                                              LIBLTE_BIT_MSG_STRUCT         *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_dl_dcch_msg(LIBLTE_BIT_MSG_STRUCT         *msg,
                                                LIBLTE_RRC_DL_DCCH_MSG_STRUCT *dl_dcch_msg);

/*********************************************************************
    Message Name: UL CCCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the UE to the E-UTRAN on the CCCH logical
                 channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REEST_REQ = 0,
    LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REQ,
    LIBLTE_RRC_UL_CCCH_MSG_TYPE_N_ITEMS,
}LIBLTE_RRC_UL_CCCH_MSG_TYPE_ENUM;
static const char liblte_rrc_ul_ccch_msg_type_text[LIBLTE_RRC_UL_CCCH_MSG_TYPE_N_ITEMS][100] = {"RRC Connection Reestablishment Request",
                                                                                                "RRC Connection Request"};
// Structs
typedef union{
    LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REQUEST_STRUCT rrc_con_reest_req;
    LIBLTE_RRC_CONNECTION_REQUEST_STRUCT                 rrc_con_req;
}LIBLTE_RRC_UL_CCCH_MSG_TYPE_UNION;
typedef struct{
    LIBLTE_RRC_UL_CCCH_MSG_TYPE_UNION msg;
    LIBLTE_RRC_UL_CCCH_MSG_TYPE_ENUM  msg_type;
}LIBLTE_RRC_UL_CCCH_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_ccch_msg(LIBLTE_RRC_UL_CCCH_MSG_STRUCT *ul_ccch_msg,
                                              LIBLTE_BIT_MSG_STRUCT         *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_ccch_msg(LIBLTE_BIT_MSG_STRUCT         *msg,
                                                LIBLTE_RRC_UL_CCCH_MSG_STRUCT *ul_ccch_msg);

/*********************************************************************
    Message Name: UL DCCH Message

    Description: Contains the set of RRC messages that may be sent
                 from the UE to the E-UTRAN on the DCCH logical
                 channel

    Document Reference: 36.331 v10.0.0 Section 6.2.1
*********************************************************************/
// Defines
// Enums
typedef enum{
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_CSFB_PARAMS_REQ_CDMA2000 = 0,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_MEASUREMENT_REPORT,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_RECONFIG_COMPLETE,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_REEST_COMPLETE,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_COMPLETE,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_FAILURE,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_UE_CAPABILITY_INFO,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_UL_HANDOVER_PREP_TRANSFER,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_COUNTER_CHECK_RESP,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_UE_INFO_RESP,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_PROXIMITY_IND,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_RN_RECONFIG_COMPLETE,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_SPARE2,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_SPARE1,
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_N_ITEMS,
}LIBLTE_RRC_UL_DCCH_MSG_TYPE_ENUM;
static const char liblte_rrc_ul_dcch_msg_type_text[LIBLTE_RRC_UL_DCCH_MSG_TYPE_N_ITEMS][100] = {"CSFB Parameters Request CDMA2000",
                                                                                                "Measurement Report",
                                                                                                "RRC Connection Reconfiguration Complete",
                                                                                                "RRC Connection Reestablishment Complete",
                                                                                                "RRC Connection Setup Complete",
                                                                                                "Security Mode Complete",
                                                                                                "Security Mode Failure",
                                                                                                "UE Capability Information",
                                                                                                "UL Handover Preparation Transfer",
                                                                                                "UL Information Transfer",
                                                                                                "Counter Check Response",
                                                                                                "UE Information Response",
                                                                                                "Proximity Indication",
                                                                                                "RN Reconfiguration Complete",
                                                                                                "SPARE",
                                                                                                "SPARE"};
// Structs
typedef union{
    LIBLTE_RRC_CSFB_PARAMETERS_REQUEST_CDMA2000_STRUCT    csfb_params_req_cdma2000;
    LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT                  measurement_report;
    LIBLTE_RRC_CONNECTION_RECONFIGURATION_COMPLETE_STRUCT rrc_con_reconfig_complete;
    LIBLTE_RRC_CONNECTION_REESTABLISHMENT_COMPLETE_STRUCT rrc_con_reest_complete;
    LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT           rrc_con_setup_complete;
    LIBLTE_RRC_SECURITY_MODE_COMPLETE_STRUCT              security_mode_complete;
    LIBLTE_RRC_SECURITY_MODE_FAILURE_STRUCT               security_mode_failure;
    LIBLTE_RRC_UE_CAPABILITY_INFORMATION_STRUCT           ue_capability_info;
    LIBLTE_RRC_UL_HANDOVER_PREPARATION_TRANSFER_STRUCT    ul_handover_prep_transfer;
    LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT             ul_info_transfer;
    LIBLTE_RRC_COUNTER_CHECK_RESPONSE_STRUCT              counter_check_resp;
    LIBLTE_RRC_UE_INFORMATION_RESPONSE_STRUCT             ue_info_resp;
    LIBLTE_RRC_PROXIMITY_INDICATION_STRUCT                proximity_ind;
    LIBLTE_RRC_RN_RECONFIGURATION_COMPLETE_STRUCT         rn_reconfig_complete;
}LIBLTE_RRC_UL_DCCH_MSG_TYPE_UNION;
typedef struct{
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_UNION msg;
    LIBLTE_RRC_UL_DCCH_MSG_TYPE_ENUM  msg_type;
}LIBLTE_RRC_UL_DCCH_MSG_STRUCT;
// Functions
LIBLTE_ERROR_ENUM liblte_rrc_pack_ul_dcch_msg(LIBLTE_RRC_UL_DCCH_MSG_STRUCT *ul_dcch_msg,
                                              LIBLTE_BIT_MSG_STRUCT         *msg);
LIBLTE_ERROR_ENUM liblte_rrc_unpack_ul_dcch_msg(LIBLTE_BIT_MSG_STRUCT         *msg,
                                                LIBLTE_RRC_UL_DCCH_MSG_STRUCT *ul_dcch_msg);

#endif // SRSLTE_LIBLTE_RRC_H
