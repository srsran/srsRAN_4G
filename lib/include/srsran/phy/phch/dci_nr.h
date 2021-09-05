/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_DCI_NR_H
#define SRSRAN_DCI_NR_H

#include "dci.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/phch/phch_cfg_nr.h"

/**
 * @brief Maximum number of NR DCI sizes the UE shall search for a given serving cell
 */
#define SRSRAN_DCI_NR_MAX_NOF_SIZES 4

/**
 * @brief DCI configuration given a serving cell
 */
typedef struct SRSRAN_API {
  /// Bandwidth parameters
  uint32_t coreset0_bw;       ///< CORESET0 DL bandwidth, set to 0 if not present
  uint32_t bwp_dl_initial_bw; ///< Initial DL BWP bandwidth
  uint32_t bwp_dl_active_bw;  ///< Active DL BWP bandwidth in PRB
  uint32_t bwp_ul_initial_bw; ///< Initial UL BWP bandwidth
  uint32_t bwp_ul_active_bw;  ///< Active UL BWP bandwidth in PRB

  /// Search space derived parameters
  bool monitor_common_0_0;  ///< Set to true if Format 0_0 is monitored in common SS
  bool monitor_0_0_and_1_0; ///< Set to true if Format 0_0 is monitored in UE-specific SS
  bool monitor_0_1_and_1_1; ///< Set to true if Formats 0_1 and 1_1 are monitored in UE-specific SS

  /// PUSCH configuration derived parameters
  bool enable_sul;     ///< Set to true if supplementary uplink is configured
  bool enable_hopping; ///< Set to true if frequency hopping is enabled

  /// Common Formats 0_1 and 1_1
  uint32_t                   carrier_indicator_size; ///< Defined in TS 38.213 clause 10.1
  srsran_harq_ack_codebook_t harq_ack_codebok;       ///< PDSCH HARQ-ACK codebook mode
  uint32_t                   nof_rb_groups;          ///< For allocation type 0, defined in TS 38.214 clause 6.1.2.2.1

  /// Format 0_1 specific configuration (for PUSCH only)
  uint32_t nof_ul_bwp;          ///< Number of UL BWPs excluding the initial UL BWP, mentioned in the TS as N_BWP_RRC
  uint32_t nof_ul_time_res;     ///< Number of dedicated PUSCH time domain resource assigment, set to 0 for default
  uint32_t nof_srs;             ///< Number of configured SRS resources
  uint32_t nof_ul_layers;       ///< Set to the maximum number of layers for PUSCH
  uint32_t pusch_nof_cbg;       ///< determined by maxCodeBlockGroupsPerTransportBlock for PUSCH
  uint32_t report_trigger_size; ///< determined by reportTriggerSize
  bool     enable_transform_precoding;      ///< Set to true if PUSCH transform precoding is enabled
  bool     dynamic_dual_harq_ack_codebook;  ///< Set to true if HARQ-ACK codebook is set to dynamic with 2 sub-codebooks
  bool     pusch_tx_config_non_codebook;    ///< Set to true if PUSCH txConfig is set to non-codebook
  bool     pusch_ptrs;                      ///< Set to true if PT-RS are enabled for PUSCH transmission
  bool     pusch_dynamic_betas;             ///< Set to true if beta offsets operation is not semi-static
  srsran_resource_alloc_t pusch_alloc_type; ///< PUSCH resource allocation type
  srsran_dmrs_sch_type_t  pusch_dmrs_type;  ///< PUSCH DMRS type
  srsran_dmrs_sch_len_t   pusch_dmrs_max_len; ///< PUSCH DMRS maximum length

  /// Format 1_1 specific configuration (for PDSCH only)
  uint32_t nof_dl_bwp;             ///< Number of DL BWPs excluding the initial UL BWP, mentioned in the TS as N_BWP_RRC
  uint32_t nof_dl_time_res;        ///< Number of dedicated PDSCH time domain resource assigment
  uint32_t nof_aperiodic_zp;       ///< Number of aperiodic ZP CSI-RS resource sets configured
  uint32_t pdsch_nof_cbg;          ///< determined by maxCodeBlockGroupsPerTransportBlock for PDSCH
  uint32_t nof_dl_to_ul_ack;       ///< Number of entries in the dl-DataToUL-ACK
  bool     pdsch_inter_prb_to_prb; ///< Set to true if interleaved VRB to PRB mapping is enabled
  bool     pdsch_rm_pattern1;      ///< Set to true if rateMatchPatternGroup1 is configured
  bool     pdsch_rm_pattern2;      ///< Set to true if rateMatchPatternGroup2 is configured
  bool     pdsch_2cw;              ///< Set to true if maxNrofCodeWordsScheduledByDCI is set to 2 in any BWP
  bool     multiple_scell;         ///< Set to true if configured with multiple serving cell
  bool     pdsch_tci;              ///< Set to true if tci-PresentInDCI is enabled
  bool     pdsch_cbg_flush;        ///< Set to true if codeBlockGroupFlushIndicator is true
  bool     pdsch_dynamic_bundling; ///< Set to true if prb-BundlingType is set to dynamicBundling
  srsran_resource_alloc_t pdsch_alloc_type;   ///< PDSCH resource allocation type, set to 0 for default
  srsran_dmrs_sch_type_t  pdsch_dmrs_type;    ///< PDSCH DMRS type
  srsran_dmrs_sch_len_t   pdsch_dmrs_max_len; ///< PDSCH DMRS maximum length
} srsran_dci_cfg_nr_t;

/**
 * @brief NR-DCI object. Stores DCI configuration and pre-calculated DCI sizes
 */
typedef struct SRSRAN_API {
  /// Configuration parameters
  srsran_dci_cfg_nr_t cfg;

  /// Formats 0_0 and 1_0 in common SS
  uint32_t dci_0_0_and_1_0_common_size; ///< DCI format 0_0 and 1_0 in common SS size
  uint32_t dci_0_0_common_trunc;        ///< DCI format 0_0 in common SS truncation
  uint32_t dci_0_0_common_padd;         ///< DCI format 0_0 in common SS padding

  /// Formats 0_0 and 1_0 in UE-specific SS
  uint32_t dci_0_0_and_1_0_ue_size; ///< DCI format 0_0 and 1_0 in UE-specific SS size
  uint32_t dci_0_0_ue_padd;         ///< DCI format 0_0 in ue SS padding
  uint32_t dci_1_0_ue_padd;         ///< DCI format 1_0 in ue SS padding

  /// Formats  0_1 and 1_0 (UE-specific SS only)
  uint32_t dci_0_1_size; ///< DCI format 0_1 size
  uint32_t dci_0_1_padd; ///< DCI format 0_1 padding
  uint32_t dci_1_1_size; ///< DCI format 0_1 size
  uint32_t dci_1_1_padd; ///< DCI format 1_1 padding

} srsran_dci_nr_t;

/**
 * @brief Describes the NR DCI search context
 */
typedef struct SRSRAN_API {
  srsran_dci_location_t      location;         ///< DCI location
  srsran_search_space_type_t ss_type;          ///< Search space type
  uint32_t                   coreset_id;       ///< CORESET identifier
  uint32_t                   coreset_start_rb; ///< CORESET lowest RB index in the resource grid
  srsran_rnti_type_t         rnti_type;        ///< RNTI type
  srsran_dci_format_nr_t     format;           ///< DCI format
  uint16_t                   rnti;             ///< UE temporal RNTI
} srsran_dci_ctx_t;

/**
 * @brief Describes any packed format NR DCI message
 */
typedef struct SRSRAN_API {
  srsran_dci_ctx_t ctx; ///< DCI context
  uint8_t          payload[50];
  uint32_t         nof_bits;
} srsran_dci_msg_nr_t;

/**
 * @brief Describes an unpacked DL NR DCI message
 */
typedef struct SRSRAN_API {
  srsran_dci_ctx_t ctx; ///< DCI search context

  // Common fields
  uint32_t freq_domain_assigment; ///< Frequency domain resource assignment
  uint32_t time_domain_assigment; ///< Time domain resource assignment
  uint32_t vrb_to_prb_mapping;    ///< VRB-to-PRB mapping
  uint32_t mcs;                   ///< Modulation and coding scheme
  uint32_t rv;                    ///< Redundancy version
  uint32_t reserved;              ///< Reserved bits

  // C-RNTI/TC-RNTI specific fields
  uint32_t ndi;            ///< New data indicator
  uint32_t pid;            ///< HARQ process number
  uint32_t dai;            ///< Downlink assignment index
  uint32_t tpc;            ///< TPC command for scheduled PUCCH
  uint32_t pucch_resource; ///< PUCCH resource indicator for HARQ feedback
                           ///< @note PUCCH resource is selected from PUCCH-ResourceSet if available, otherwise the UE
                           ///< shall pick a pucch-ResourceCommon from Table 9.2.1-1.
  uint32_t harq_feedback;  ///< PDSCH-to-HARQ_feedback timing indicator
                           ///< @note harq_feedback for format 1_0 indicates the delay between the PDSCH reception and
                           ///< the UL transmission timing
                           ///< @note harq_feedback for format 1_1 is index of the delay indicated in DL data to UL ACK
                           ///< dedicated configuration table

  // P-RNTI specific fields
  uint32_t smi;        ///< Short Messages Indicator
  uint32_t sm;         ///< Short Messages
  uint32_t tb_scaling; ///< TB scaling

  // SI-RNTI specific fields
  uint32_t sii; ///< System information indicator

  // Second TB (Optional)
  uint32_t mcs2; ///< Modulation and coding scheme for the second transport block
  uint32_t ndi2; ///< New data indicator for the second transport block
  uint32_t rv2;  ///< Redundancy version for the second transport block

  // Optional fields
  uint32_t cc_id;        ///< Carrier indicator
  uint32_t bwp_id;       ///< BWP indicator
  uint32_t rm_pattern1;  ///< Rate matching pattern 1 indicator
  uint32_t rm_pattern2;  ///< Rate matching pattern 2 indicator
  uint32_t zp_csi_rs_id; ///< ZP-CSI-RS trigger
  uint32_t ports;        ///< Antenna ports
  uint32_t tci;          ///< Transmission configuration indication
  uint32_t srs_request;  ///< SRS request
  uint32_t cbg_info;     ///< CBG transmission information (CBGTI)
  uint32_t cbg_flush;    ///< CBG flushing out information (CBGFI)
  uint32_t dmrs_id;      ///< DMRS sequence initialization

  // DL context from unpacking. Required for resource allocation
  uint32_t coreset0_bw; ///< CORESET0 size used for frequency resource allocation

} srsran_dci_dl_nr_t;

/**
 * @brief Describes an unpacked UL NR DCI message
 * @remark NR RAR UL DCI Described in TS 38.213 Table 8.2-1: Random Access Response Grant Content field size
 */
typedef struct SRSRAN_API {
  // Context information
  srsran_dci_ctx_t ctx; ///< Context information

  // Common fields
  uint32_t freq_domain_assigment; ///< Frequency domain resource assignment
  uint32_t time_domain_assigment; ///< Time domain resource assignment
  uint32_t freq_hopping_flag;     ///< Frequency hopping flag
  uint32_t mcs;                   ///< Modulation and coding scheme
  uint32_t rv;                    ///< Redundancy version
  uint32_t reserved;              ///< Reserved bits

  // C-RNTI or CS-RNTI or MCS-C-RNTI
  uint32_t ndi; ///< New data indicator
  uint32_t pid; ///< HARQ process number
  uint32_t tpc; ///< TPC command for scheduled PUCCH

  // Frequency hopping
  uint32_t frequency_offset; ///< frequency offset

  // Random Access Response Grant
  uint32_t csi_request; ///< CSI request

  // Optional fields
  uint32_t sul;         ///< Supplementary Uplink flag
  uint32_t cc_id;       ///< Carrier indicator
  uint32_t bwp_id;      ///< BWP indicator
  uint32_t dai1;        ///< First DAI
  uint32_t dai2;        ///< Second DAI
  uint32_t srs_id;      ///< SRS resource indicator
  uint32_t ports;       ///< Antenna ports
  uint32_t srs_request; ///< SRS request
  uint32_t cbg_info;    ///< CBG transmission information (CBGTI)
  uint32_t ptrs_id;     ///< PTRS-DMRS association
  uint32_t beta_id;     ///< beta_offset indicator
  uint32_t dmrs_id;     ///< DMRS sequence initialization
  uint32_t ulsch;       ///< UL-SCH indicator
} srsran_dci_ul_nr_t;

/**
 * @brief Set NR-DCI configuration for a given cell. This function will pre-compute the DCI sizes, padding, truncation
 * and so on from a given DCI configuration.
 * @remark Implemented according TS 38.212 section 7.3.1.0 DCI size alignment
 * @param[in,out] dci DCI object
 * @param[in] cfg NR-DCI configuration
 * @return SRSRAN_SUCCESS if the configuration is valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_dci_nr_set_cfg(srsran_dci_nr_t* dci, const srsran_dci_cfg_nr_t* cfg);

/**
 * @brief Calculates a DL NR-DCI size for a given SS type and format
 * @attention Only DCI 0_0 and 1_0 can be in common search-space
 * @param q NR DCI object
 * @param ss_type Search Space type
 * @param format NR DCI format
 * @return The number of bis for the DCI message if configured, 0 otherwise
 */
SRSRAN_API uint32_t srsran_dci_nr_size(const srsran_dci_nr_t*     q,
                                       srsran_search_space_type_t ss_type,
                                       srsran_dci_format_nr_t     format);

/**
 * @brief Indicates whether the provided DCI message format bit indicator belongs to the a Dl DCI format (1_0 or 1_1)
 * according according to the RNTI type. If invalid, the DCI message is likely to be an UL DCI
 * @param dci Provides DCI format message
 * @return true if the DCI message is for DL, false otherwise
 */
SRSRAN_API bool srsran_dci_nr_valid_direction(const srsran_dci_msg_nr_t* dci);

/**
 * @brief Packs a DL NR DCI into a DCI message
 * @param q NR DCI object with precomputed DCI parameters
 * @param dci DL NR DCI to pack (serialize)
 * @param[out] msg Resultant packed DCI message
 * @return SRSRAN_SUCCESS if provided arguments are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_dci_nr_dl_pack(const srsran_dci_nr_t* q, const srsran_dci_dl_nr_t* dci, srsran_dci_msg_nr_t* msg);

/**
 * @brief Unpacks an NR DCI message into a DL NR DCI
 * @param q NR DCI object with precomputed DCI parameters
 * @param msg  DCI message to unpack (deserialize)
 * @param[out] dci Resultant unpacked DL DCI
 * @return SRSRAN_SUCCESS if provided arguments are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_dci_nr_dl_unpack(const srsran_dci_nr_t* q, srsran_dci_msg_nr_t* msg, srsran_dci_dl_nr_t* dci);

/**
 * @brief Packs an UL NR DCI into a DCI message
 * @param q NR DCI object with precomputed DCI parameters (not required for RAR type, set to NULL)
 * @param dci UL NR DCI to pack (serialize)
 * @param[out] msg resultant DCI message
 * @return SRSRAN_SUCCESS if provided arguments are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_dci_nr_ul_pack(const srsran_dci_nr_t* q, const srsran_dci_ul_nr_t* dci, srsran_dci_msg_nr_t* msg);

/**
 * @brief Unpacks an NR DCI message into an UL NR DCI
 * @param q NR DCI object with precomputed DCI parameters (not required for RAR type, set to NULL)
 * @param msg  DCI message to unpack (deserialize)
 * @param[out] dci Resultant unpacked UL DCI
 * @return SRSRAN_SUCCESS if provided arguments are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_dci_nr_ul_unpack(const srsran_dci_nr_t* q, srsran_dci_msg_nr_t* msg, srsran_dci_ul_nr_t* dci);

/**
 * @brief Stringifies an NR DCI conxtext
 * @param ctx Provides NR DCI context
 * @param[out] str Destination string
 * @param str_len Destination string length
 * @return The number of written characters
 */
SRSRAN_API uint32_t srsran_dci_ctx_to_str(const srsran_dci_ctx_t* ctx, char* str, uint32_t str_len);

/**
 * @brief Stringifies a DL NR DCI structure
 * @param q NR DCI object with precomputed DCI parameters
 * @param dci DL NR SCI structure to stringify
 * @param[out] str Destination string
 * @param str_len Destination string length
 * @return The number of written characters
 */
SRSRAN_API uint32_t srsran_dci_dl_nr_to_str(const srsran_dci_nr_t*    q,
                                            const srsran_dci_dl_nr_t* dci,
                                            char*                     str,
                                            uint32_t                  str_len);

/**
 * @brief Stringifies an UL NR DCI structure
 * @param q NR DCI object with precomputed DCI parameters
 * @param dci UL NR SCI structure to stringify
 * @param[out] str Destination string
 * @param str_len Destination string length
 * @return The number of written characters
 */
SRSRAN_API uint32_t srsran_dci_ul_nr_to_str(const srsran_dci_nr_t*    q,
                                            const srsran_dci_ul_nr_t* dci,
                                            char*                     str,
                                            uint32_t                  str_len);

#endif // SRSRAN_DCI_NR_H
