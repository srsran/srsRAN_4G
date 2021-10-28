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

#ifndef SRSRAN_PHY_COMMON_NR_H
#define SRSRAN_PHY_COMMON_NR_H

#include "phy_common.h"
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines the number of symbols per slot. Defined by TS 38.211 v15.8.0 Table 4.3.2-1.
 */
#define SRSRAN_NSYMB_PER_SLOT_NR 14U

/**
 * @brief Defines the resource grid size in physical resource elements (frequency and time domain)
 */
#define SRSRAN_SLOT_LEN_RE_NR(nof_prb) (nof_prb * SRSRAN_NRE * SRSRAN_NSYMB_PER_SLOT_NR)

/**
 * @brief Minimum subframe length in samples for a given number of PRB
 */
#define SRSRAN_SF_LEN_PRB_NR(nof_prb) (srsran_min_symbol_sz_rb(nof_prb) * 15)

#define SRSRAN_SLOT_MAX_LEN_RE_NR (SRSRAN_SLOT_LEN_RE_NR(SRSRAN_MAX_PRB_NR))
#define SRSRAN_MAX_LAYERS_NR 8

/**
 * @brief Defines the maximum numerology supported. Defined by TS 38.211 v15.8.0 Table 4.3.2-1.
 */
#define SRSRAN_NR_MAX_NUMEROLOGY 4U

/**
 * @brief Defines the symbol duration, including cyclic prefix
 */
#define SRSRAN_SUBC_SPACING_NR(NUM) (15000U << (uint32_t)(NUM))

/**
 * @brief Defines the number of slots per SF. Defined by TS 38.211 v15.8.0 Table 4.3.2-1.
 */
#define SRSRAN_NSLOTS_PER_SF_NR(NUM) (1U << (NUM))

/**
 * @brief Defines the number of slots per frame. Defined by TS 38.211 v15.8.0 Table 4.3.2-1.
 */
#define SRSRAN_NSLOTS_PER_FRAME_NR(NUM) (SRSRAN_NSLOTS_PER_SF_NR(NUM) * SRSRAN_NOF_SF_X_FRAME)

/**
 * @brief Bounds slot index into the frame
 */
#define SRSRAN_SLOT_NR_MOD(NUM, N) ((N) % SRSRAN_NSLOTS_PER_FRAME_NR(NUM))

/**
 * @brief Maximum Carrier identification value. Defined by TS 38.331 v15.10.0 as PhysCellId from 0 to 1007.
 */
#define SRSRAN_MAX_ID_NR 1007

/**
 * @brief Maximum number of physical resource blocks (PRB) that a 5G NR can have. This is defined by TS 38.331 v15.10.0
 * as maxNrofPhysicalResourceBlocks
 */
#define SRSRAN_MAX_PRB_NR 275

/**
 * @brief Maximum start sub-carrier index for the carrier relative point
 */
#define SRSRAN_MAX_START_NR 2199

/**
 * @brief defines the maximum number of Aggregation levels: 1, 2, 4, 8 and 16
 */
#define SRSRAN_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR 5

/**
 * @brief defines the maximum number of RE
 */
#define SRSRAN_PDCCH_MAX_RE ((SRSRAN_NRE - 3U) * (1U << (SRSRAN_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR - 1U)) * 6U)

/**
 * @brief defines the maximum number of candidates for a given search-space and aggregation level according to TS 38.331
 * SearchSpace sequence
 */
#define SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR 8

/**
 * @brief defines the maximum number of monitored PDCCH candidates per slot and per serving cell according to TS 38.213
 * Table 10.1-2
 */
#define SRSRAN_MAX_NOF_CANDIDATES_SLOT_NR 44

/**
 * @brief defines the maximum number of resource elements per PRB
 * @remark Defined in TS 38.214 V15.10.0 5.1.3.2 Transport block size determination, point 1, second bullet
 */
#define SRSRAN_MAX_NRE_NR 156

/**
 * @brief defines the maximum number of resource elements in a PDSCH transmission
 * @remark deduced from in TS 36.214 V15.10.0 5.1.3.2 Transport block size determination
 */
#define SRSRAN_PDSCH_MAX_RE_NR (SRSRAN_MAX_NRE_NR * SRSRAN_MAX_PRB_NR)

/**
 * @brief defines the maximum number of bits that can be transmitted in a slot
 */
#define SRSRAN_SLOT_MAX_NOF_BITS_NR (SRSRAN_PDSCH_MAX_RE_NR * SRSRAN_MAX_QM)

/**
 * @brief Maximum number of PDSCH time domain resource allocations. This is defined by TS 38.331 v15.10.0
 * as maxNrofDL-Allocations
 */
#define SRSRAN_MAX_NOF_TIME_RA 16

/**
 * @brief Maximum dl-DataToUL-ACK value. This is defined by TS 38.331 v15.10.1 in PUCCH-Config
 */
#define SRSRAN_MAX_NOF_DL_DATA_TO_UL 8

/**
 * @brief Maximum number of HARQ processes in the DL, signaled through RRC (PDSCH-ServingCellConfig)
 */
#define SRSRAN_MAX_HARQ_PROC_DL_NR 16 // 3GPP TS 38.214 version 15.3.0 Sec. 5.1 or nrofHARQ-ProcessesForPDSCH

/**
 * @brief Default number of HARQ processes in the DL, if config is absent.
 */
#define SRSRAN_DEFAULT_HARQ_PROC_DL_NR 8

/**
 * @brief Maximum number of HARQ processes in the UL, signaled through RRC (ConfiguredGrantConfig)
 */
#define SRSRAN_MAX_HARQ_PROC_UL_NR 16 // 3GPP TS 38.214 version 15.3.0 Sec. 6.1

/**
 * @brief SSB bandwidth in subcarriers, described in TS 38.211 section 7.4.3.1 Time-frequency structure of an SS/PBCH
 * block
 */
#define SRSRAN_SSB_BW_SUBC 240

/**
 * @brief SSB duration in symbols, described in TS 38.211 section 7.4.3.1 Time-frequency structure of an SS/PBCH block
 */
#define SRSRAN_SSB_DURATION_NSYMB 4

/**
 * @brief Number of NR N_id_1 Physical Cell Identifier (PCI) as described in TS 38.211 section 7.4.2.1 Physical-layer
 * cell identities
 */
#define SRSRAN_NOF_NID_1_NR 336

/**
 * @brief Number of NR N_id_2 Physical Cell Identifier (PCI) as described in TS 38.211 section 7.4.2.1 Physical-layer
 * cell identities
 */
#define SRSRAN_NOF_NID_2_NR 3

/**
 * @brief Number of NR N_id Physical Cell Identifier (PCI) as described in TS 38.211 section 7.4.2.1 Physical-layer
 * cell identities
 */
#define SRSRAN_NOF_NID_NR (SRSRAN_NOF_NID_1_NR * SRSRAN_NOF_NID_2_NR)

/**
 * @brief Compute N_id_1 from the Physical Cell Identifier (PCI) as described in TS 38.211 section 7.4.2.1
 * Physical-layer cell identities
 */
#define SRSRAN_NID_1_NR(N_ID) ((N_ID) / SRSRAN_NOF_NID_2_NR)

/**
 * @brief Compute N_id_2 from the Physical Cell Identifier (PCI) as described in TS 38.211 section 7.4.2.1
 * Physical-layer cell identities
 */
#define SRSRAN_NID_2_NR(N_ID) ((N_ID) % SRSRAN_NOF_NID_2_NR)

/**
 * @brief Compute Physical Cell Identifier (PCI) N_id from N_id_1 and N_id_2
 */
#define SRSRAN_NID_NR(NID_1, NID_2) (SRSRAN_NOF_NID_2_NR * (NID_1) + (NID_2))

/**
 * @brief SSB number of resource elements, described in TS 38.211 section 7.4.3.1 Time-frequency structure of an SS/PBCH
 * block
 */
#define SRSRAN_SSB_NOF_RE (SRSRAN_SSB_BW_SUBC * SRSRAN_SSB_DURATION_NSYMB)

/**
 * @brief Symbol index with extended CP
 */
#define SRSRAN_EXT_CP_SYMBOL(SCS) (7U << (uint32_t)(SCS))

typedef enum SRSRAN_API {
  srsran_coreset_mapping_type_non_interleaved = 0,
  srsran_coreset_mapping_type_interleaved,
} srsran_coreset_mapping_type_t;

typedef enum SRSRAN_API {
  srsran_coreset_bundle_size_n2 = 0,
  srsran_coreset_bundle_size_n3,
  srsran_coreset_bundle_size_n6,
} srsran_coreset_bundle_size_t;

typedef enum SRSRAN_API {
  srsran_coreset_precoder_granularity_contiguous = 0,
  srsran_coreset_precoder_granularity_reg_bundle
} srsran_coreset_precoder_granularity_t;

/**
 * @brief PDSCH mapping type
 * @remark Described in TS 38.331 V15.10.0 Section PDSCH-TimeDomainResourceAllocationList
 */
typedef enum SRSRAN_API { srsran_sch_mapping_type_A = 0, srsran_sch_mapping_type_B } srsran_sch_mapping_type_t;

/**
 * @brief Search Space (SS) type
 * @remark Described in TS 38.213 V15.10.0 Section 10.1 UE procedure for determining physical downlink control channel
 * assignment
 */
typedef enum SRSRAN_API {
  srsran_search_space_type_common_0 = 0, ///< configured by pdcch-ConfigSIB1 in MIB or by searchSpaceSIB1 in
                                         ///< PDCCH-ConfigCommon or by searchSpaceZero in PDCCH-ConfigCommon
  srsran_search_space_type_common_0A,    ///< configured by searchSpaceOtherSystemInformation in PDCCH-ConfigCommon
  srsran_search_space_type_common_1,     ///< configured by ra-SearchSpace in PDCCH-ConfigCommon
  srsran_search_space_type_common_2,     ///< configured by pagingSearchSpace in PDCCH-ConfigCommon
  srsran_search_space_type_common_3,     ///< configured by SearchSpace in PDCCH-Config with searchSpaceType = common
  srsran_search_space_type_ue,  ///< configured by SearchSpace in PDCCH-Config with searchSpaceType = ue-Specific
  srsran_search_space_type_rar, ///< Indicates that a grant was given by MAC RAR as described in TS 38.213 clause 8.2
  srsran_search_space_type_cg   ///< Indicates that a grant was given by Configured Grant from the upper layers
} srsran_search_space_type_t;

/**
 * @brief Helper macro to get if a search space type is common or not
 */
#define SRSRAN_SEARCH_SPACE_IS_COMMON(SS_TYPE) ((SS_TYPE) < srsran_search_space_type_ue)

/**
 * @brief RAR content length in bits (see TS 38.321 Sec 6.2.3)
 */
#define SRSRAN_RAR_UL_GRANT_NBITS (27)

/**
 * @brief Indicates the MCS table the UE shall use for PDSCH and/or PUSCH without transform precoding
 */
typedef enum SRSRAN_API {
  srsran_mcs_table_64qam = 0,
  srsran_mcs_table_256qam,
  srsran_mcs_table_qam64LowSE,
  srsran_mcs_table_N
} srsran_mcs_table_t;

/**
 * @brief RNTI types
 * @remark Usage described in TS 38.321 Table 7.1-2: RNTI usage.
 */
typedef enum SRSRAN_API {
  srsran_rnti_type_c = 0,
  srsran_rnti_type_p,      ///< @brief Paging and System Information change notification (PCH)
  srsran_rnti_type_si,     ///< @brief Broadcast of System Information (DL-SCH)
  srsran_rnti_type_ra,     ///< @brief Random Access Response (DL-SCH)
  srsran_rnti_type_tc,     ///< @brief Contention Resolution (when no valid C-RNTI is available) (DL-SCH)
  srsran_rnti_type_cs,     ///< @brief Configured scheduled unicast transmission (DL-SCH, UL-SCH)
  srsran_rnti_type_sp_csi, ///< @brief Activation of Semi-persistent CSI reporting on PUSCH
  srsran_rnti_type_mcs_c,  ///< @brief Dynamically scheduled unicast transmission (DL-SCH)
} srsran_rnti_type_t;

/**
 * @brief DCI formats
 * @remark Described in TS 38.212 V15.9.0 Section 7.3.1 DCI formats
 */
typedef enum SRSRAN_API {
  srsran_dci_format_nr_0_0 = 0, ///< @brief Scheduling of PUSCH in one cell
  srsran_dci_format_nr_0_1,     ///< @brief Scheduling of PUSCH in one cell
  srsran_dci_format_nr_1_0,     ///< @brief Scheduling of PDSCH in one cell
  srsran_dci_format_nr_1_1,     ///< @brief Scheduling of PDSCH in one cell
  srsran_dci_format_nr_2_0,     ///< @brief Notifying a group of UEs of the slot format
  srsran_dci_format_nr_2_1, ///< @brief Notifying a group of UEs of the PRB(s) and OFDM symbol(s) where UE may assume no
                            ///< transmission is intended for the UE
  srsran_dci_format_nr_2_2, ///< @brief Transmission of TPC commands for PUCCH and PUSCH
  srsran_dci_format_nr_2_3, ///< @brief Transmission of a group of TPC commands for SRS transmissions by one or more UEs
  srsran_dci_format_nr_rar, ///< @brief Scheduling a transmission of PUSCH from RAR
  srsran_dci_format_nr_cg,  ///< @brief Scheduling of PUSCH using a configured grant
  SRSRAN_DCI_FORMAT_NR_COUNT ///< @brief Number of DCI formats
} srsran_dci_format_nr_t;

/**
 * @brief Overhead configuration provided by higher layers
 * @remark Described in TS 38.331 V15.10.0 PDSCH-ServingCellConfig
 * @remark Described in TS 38.331 V15.10.0 PUSCH-ServingCellConfig
 */
typedef enum SRSRAN_API {
  srsran_xoverhead_0 = 0,
  srsran_xoverhead_6,
  srsran_xoverhead_12,
  srsran_xoverhead_18
} srsran_xoverhead_t;

/**
 * @brief PDSCH HARQ ACK codebook configuration
 * @remark Described in TS 38.331 V15.10.0 PhysicalCellGroupConfig
 */
typedef enum SRSRAN_API {
  srsran_pdsch_harq_ack_codebook_none = 0,
  srsran_pdsch_harq_ack_codebook_semi_static,
  srsran_pdsch_harq_ack_codebook_dynamic,
} srsran_harq_ack_codebook_t;

/**
 * @brief PDSCH/PUSCH Resource allocation configuration
 * @remark Described in TS 38.331 V15.10.0 PhysicalCellGroupConfig
 */
typedef enum SRSRAN_API {
  srsran_resource_alloc_type0 = 0,
  srsran_resource_alloc_type1,
  srsran_resource_alloc_dynamic,
} srsran_resource_alloc_t;

/**
 * @brief Subcarrier spacing 15 or 30 kHz <6GHz and 60 or 120 kHz >6GHz
 * @remark Described in TS 38.331 V15.10.0 subcarrier spacing
 */

typedef enum SRSRAN_API {
  srsran_subcarrier_spacing_15kHz = 0,
  srsran_subcarrier_spacing_30kHz,
  srsran_subcarrier_spacing_60kHz,
  srsran_subcarrier_spacing_120kHz,
  srsran_subcarrier_spacing_240kHz,
  srsran_subcarrier_spacing_invalid
} srsran_subcarrier_spacing_t;

typedef enum SRSRAN_API {
  SRSRAN_SSB_PATTERN_A = 0, // FR1, 15 kHz SCS
  SRSRAN_SSB_PATTERN_B,     // FR1, 30 kHz SCS
  SRSRAN_SSB_PATTERN_C,     // FR1, 30 kHz SCS
  SRSRAN_SSB_PATTERN_D,     // FR2, 120 kHz SCS
  SRSRAN_SSB_PATTERN_E,     // FR2, 240 kHz SCS
  SRSRAN_SSB_PATTERN_INVALID,
} srsran_ssb_patern_t;

typedef enum SRSRAN_API {
  SRSRAN_DUPLEX_MODE_FDD = 0, // Paired
  SRSRAN_DUPLEX_MODE_TDD,     // Unpaired
  SRSRAN_DUPLEX_MODE_SDL,     // Supplementary DownLink
  SRSRAN_DUPLEX_MODE_SUL,     // Supplementary UpLink
  SRSRAN_DUPLEX_MODE_INVALID
} srsran_duplex_mode_t;

/**
 * @brief Determines whether the first DMRS goes into symbol index 2 or 3
 */
typedef enum {
  srsran_dmrs_sch_typeA_pos_2 = 0, // Start in slot symbol index 2 (default)
  srsran_dmrs_sch_typeA_pos_3      // Start in slot symbol index 3
} srsran_dmrs_sch_typeA_pos_t;

/**
 * @brief NR carrier parameters. It is a combination of fixed cell and bandwidth-part (BWP)
 */
typedef struct SRSRAN_API {
  uint32_t                    pci;
  double                      dl_center_frequency_hz; ///< Absolute baseband center frequency in Hz for DL grid
  double                      ul_center_frequency_hz; ///< Absolute baseband center frequency in Hz for UL grid
  double                      ssb_center_freq_hz;     ///< SS/PBCH Block center frequency in Hz. Set to 0 if not present
  uint32_t                    offset_to_carrier; ///< Offset between point A and the lowest subcarrier of the lowest RB
  srsran_subcarrier_spacing_t scs;
  uint32_t                    nof_prb; ///< @brief See TS 38.101-1 Table 5.3.2-1 for more details
  uint32_t                    start;
  uint32_t max_mimo_layers; ///< @brief DL: Indicates the maximum number of MIMO layers to be used for PDSCH in all BWPs
                            ///< of this serving cell. (see TS 38.212 [17], clause 5.4.2.1). UL: Indicates the maximum
                            ///< MIMO layer to be used for PUSCH in all BWPs of the normal UL of this serving cell (see
                            ///< TS 38.212 [17], clause 5.4.2.1)
} srsran_carrier_nr_t;

#define SRSRAN_DEFAULT_CARRIER_NR                                                                                      \
  {                                                                                                                    \
    .pci = 500, .dl_center_frequency_hz = 3.5e9, .ul_center_frequency_hz = 3.5e9, .ssb_center_freq_hz = 3.5e9,         \
    .offset_to_carrier = 0, .scs = srsran_subcarrier_spacing_15kHz, .nof_prb = 52, .start = 0, .max_mimo_layers = 1    \
  }

/**
 * @brief NR Slot parameters. It contains parameters that change in a slot basis.
 */
typedef struct SRSRAN_API {
  /// Slot index in the radio frame
  uint32_t idx;

  /// Left for future parameters
  /// ...
} srsran_slot_cfg_t;

/**
 * @brief Min number of OFDM symbols in a control resource set.
 */
#define SRSRAN_CORESET_DURATION_MIN 1

/**
 * @brief Max number of OFDM symbols in a control resource set. Specified in TS 38.331 V15.10.0 as maxCoReSetDuration
 */
#define SRSRAN_CORESET_DURATION_MAX 3

/**
 * @brief Number of possible CORESET frequency resources.
 */
#define SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE 45

/**
 * @brief Max value for shift index
 */
#define SRSRAN_CORESET_SHIFT_INDEX_MAX (SRSRAN_CORESET_NOF_PRB_MAX - 1)

/**
 * @brief CORESET parameters as defined in TS 38.331 V15.10.0 - ControlResourceSet
 */
typedef struct SRSRAN_API {
  uint32_t                              id;
  srsran_coreset_mapping_type_t         mapping_type;
  uint32_t                              duration;
  bool                                  freq_resources[SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE];
  bool                                  dmrs_scrambling_id_present;
  uint32_t                              dmrs_scrambling_id;
  srsran_coreset_precoder_granularity_t precoder_granularity;
  srsran_coreset_bundle_size_t          interleaver_size; ///< Referenced in TS 38.211 section 7.3.2.2 as R
  srsran_coreset_bundle_size_t          reg_bundle_size;  ///< Referenced in TS 38.211 section 7.3.2.2 as L
  uint32_t                              shift_index;
  uint32_t offset_rb; ///< Integer offset in resource blocks from the pointA (lowest subcarrier of resource grid) to the
                      ///< lowest resource block of the CORESET region (used by CORESET Zero only)

  /** Missing TCI parameters */
} srsran_coreset_t;

/**
 * @brief SearchSpace parameters as defined in TS 38.331 v15.10.0 SearchSpace sequence
 */
typedef struct SRSRAN_API {
  uint32_t                   id;
  uint32_t                   coreset_id;
  uint32_t                   duration; ///< SS duration length in slots
  srsran_search_space_type_t type;     ///< Sets the SS type, common (multiple types) or UE specific
  srsran_dci_format_nr_t     formats[SRSRAN_DCI_FORMAT_NR_COUNT]; ///< Specifies the DCI formats that shall be searched
  uint32_t                   nof_formats;
  uint32_t                   nof_candidates[SRSRAN_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR];
} srsran_search_space_t;

/**
 * @brief TDD pattern configuration
 */
typedef struct SRSRAN_API {
  uint32_t period_ms;      ///< Period in milliseconds, set to 0 if not present
  uint32_t nof_dl_slots;   ///< Number of consecutive full DL slots at the beginning of each DL-UL pattern
  uint32_t nof_dl_symbols; ///< Number of consecutive DL symbols in the beginning of the slot following the last DL slot
  uint32_t nof_ul_slots;   ///< Number of consecutive full UL slots at the end of each DL-UL pattern
  uint32_t nof_ul_symbols; ///< Number of consecutive UL symbols in the end of the slot preceding the first full UL slot
} srsran_tdd_pattern_t;

/**
 * @brief TDD configuration as described in TS 38.331 v15.10.0 TDD-UL-DL-ConfigCommon
 */
typedef struct SRSRAN_API {
  srsran_tdd_pattern_t pattern1;
  srsran_tdd_pattern_t pattern2;
} srsran_tdd_config_nr_t;

/**
 * @brief Describes duplex configuration
 */
typedef struct SRSRAN_API {
  srsran_duplex_mode_t mode;
  union {
    srsran_tdd_config_nr_t tdd; ///< TDD configuration
    // ... add here other mode parameters
  };
} srsran_duplex_config_nr_t;

/**
 * @brief Describes a measurement based on NZP-CSI-RS or SSB-CSI
 * @note Used for tracking RSRP, SNR, CFO, SFO, and so on
 * @note srsran_csi_channel_measurements_t is used for CSI report generation
 */
typedef struct SRSRAN_API {
  float    rsrp;       ///< Linear scale RSRP
  float    rsrp_dB;    ///< Logarithm scale RSRP relative to full-scale
  float    epre;       ///< Linear scale EPRE
  float    epre_dB;    ///< Logarithm scale EPRE relative to full-scale
  float    n0;         ///< Linear noise level
  float    n0_dB;      ///< Logarithm scale noise level relative to full-scale
  float    snr_dB;     ///< Signal to noise ratio in decibels
  float    cfo_hz;     ///< Carrier frequency offset in Hz. Only set if more than 2 symbols are available in a TRS set
  float    cfo_hz_max; ///< Maximum CFO in Hz that can be measured. It is set to 0 if CFO cannot be estimated
  float    delay_us;   ///< Average measured delay in microseconds
  uint32_t nof_re;     ///< Number of available RE for the measurement, it can be used for weighting among different
  ///< measurements
} srsran_csi_trs_measurements_t;

/**
 * @brief Get the RNTI type name for NR
 * @param rnti_type RNTI type name
 * @return Constant string with the RNTI type name
 */
SRSRAN_API const char* srsran_rnti_type_str(srsran_rnti_type_t rnti_type);

/**
 * @brief Get the short RNTI type name for NR
 * @param rnti_type RNTI type name
 * @return Constant string with the short RNTI type name
 */
SRSRAN_API const char* srsran_rnti_type_str_short(srsran_rnti_type_t rnti_type);

/**
 * @brief Get the Search Space Type string for a given type
 * @param ss_type The given Search Space Type
 * @return The string describing the SS Type
 */
SRSRAN_API const char* srsran_ss_type_str(srsran_search_space_type_t ss_type);

/**
 * @brief Get the RNTI type name for NR
 * @param rnti_type RNTI type name
 * @return Constant string with the RNTI type name
 */
SRSRAN_API const char* srsran_dci_format_nr_string(srsran_dci_format_nr_t format);

/**
 * @brief Calculates the bandwidth of a given CORESET in physical resource blocks (PRB) . This function uses the
 * frequency domain resources bit-map for counting the number of PRB.
 *
 * @attention Prior to this function call, the frequency domain resources bit-map shall be zeroed beyond the
 * carrier.nof_prb / 6 index, otherwise the CORESET bandwidth might be greater than the carrier.
 *
 * @param coreset provides the given CORESET configuration
 * @return The number of PRB that the CORESET takes in frequency domain
 */
SRSRAN_API uint32_t srsran_coreset_get_bw(const srsran_coreset_t* coreset);

/**
 * @brief Calculates the number of Physical Resource Elements (time and frequency domain) that a given CORESET uses.
 * This function uses the frequency domain resources bit-map and duration to compute the number of symbols.
 *
 * @attention Prior to this function call, the frequency domain resources bit-map shall be zeroed beyond the
 * carrier.nof_prb / 6 index, otherwise the CORESET bandwidth might be greater than the carrier.
 *
 * @param coreset provides the given CORESET configuration
 * @return The number of resource elements that compose the coreset
 */
SRSRAN_API uint32_t srsran_coreset_get_sz(const srsran_coreset_t* coreset);

/**
 * @brief Calculates the starting resource block index in the resource grid
 *
 * @remark Intended to be used for common search space as specifies the lat clause in TS 38.214 section 5.1.2.2 Resource
 * allocation in frequency domain
 *
 * @param coreset provides the given CORESET configuration
 * @return The index of the lowest resource block in the resource grid used by the given CORESET if the CORESET
 * configuration is valid; Otherwise, 0.
 */
SRSRAN_API uint32_t srsran_coreset_start_rb(const srsran_coreset_t* coreset);

/**
 * @brief Get the NR PDSCH mapping type in string
 * @param mapping_type Mapping type
 * @return Constant string with PDSCH mapping type
 */
SRSRAN_API const char* srsran_sch_mapping_type_to_str(srsran_sch_mapping_type_t mapping_type);

/**
 * @brief Get the MCS table string
 * @param mcs_table MCS table value
 * @return Constant string with the MCS table name
 */
SRSRAN_API const char* srsran_mcs_table_to_str(srsran_mcs_table_t mcs_table);

/**
 * @brief Get the MCS table value from a string
 * @param str Points to a given string
 * @return The MCS table value
 */
SRSRAN_API srsran_mcs_table_t srsran_mcs_table_from_str(const char* str);

/**
 * @brief Computes the minimum valid symbol size for a given amount of PRB
 * @attention The valid FFT sizes are radix 2 and radix 3 between 128 to 4096 points.
 * @param nof_prb Number of PRB
 * @return The minimum valid FFT size if the number of PRB is in range, 0 otherwise
 */
SRSRAN_API uint32_t srsran_min_symbol_sz_rb(uint32_t nof_prb);

/**
 * @brief Computes the minimum valid symbol size for a given amount of PRB
 * @attention The valid FFT sizes are radix 2 and radix 3 between 128 to 4096 points.
 * @param nof_prb Number of PRB
 * @return The minimum valid FFT size if the number of PRB is in range, 0 otherwise
 */
SRSRAN_API int srsran_symbol_sz_from_srate(double srate_hz, srsran_subcarrier_spacing_t scs);

/**
 * @brief Computes the time in seconds between the beginning of the slot and the given symbol
 * @remark All symbol size reference and values are taken from TS 38.211 section 5.3 OFDM baseband signal generation
 * @param l Given symbol index
 * @param scs Subcarrier spacing
 * @return Returns the symbol time offset in seconds
 */
SRSRAN_API float srsran_symbol_offset_s(uint32_t l, srsran_subcarrier_spacing_t scs);

/**
 * @brief Computes the time in seconds between two symbols in a slot
 * @note l0 is expected to be smaller than l1
 * @remark All symbol size reference and values are taken from TS 38.211 section 5.3 OFDM baseband signal generation
 * @param l0 First symbol index within the slot
 * @param l1 Second symbol index within the slot
 * @param scs Subcarrier spacing
 * @return Returns the time in seconds between the two symbols if the condition above is satisfied, 0 seconds otherwise
 */
SRSRAN_API float srsran_symbol_distance_s(uint32_t l0, uint32_t l1, srsran_subcarrier_spacing_t scs);

/**
 * @brief Decides whether a given slot is configured as Downlink
 * @param cfg Provides the carrier duplex configuration
 * @param numerology Provides BWP numerology
 * @param slot_idx Slot index in the frame for the given numerology
 * @return true if the provided slot index is configured for Downlink
 */
SRSRAN_API bool srsran_duplex_nr_is_dl(const srsran_duplex_config_nr_t* cfg, uint32_t numerology, uint32_t slot_idx);

/**
 * @brief Decides whether a given slot is configured as Uplink
 * @param cfg Provides the carrier duplex configuration
 * @param numerology Provides BWP numerology
 * @param slot_idx Slot index in the frame for the given numerology
 * @return true if the provided slot index is configured for Uplink
 */
SRSRAN_API bool srsran_duplex_nr_is_ul(const srsran_duplex_config_nr_t* cfg, uint32_t numerology, uint32_t slot_idx);

SRSRAN_API int srsran_carrier_to_cell(const srsran_carrier_nr_t* carrier, srsran_cell_t* cell);

/**
 * @brief Writes detailed Channel State Information measurement into a string
 * @param meas Provides the measurement
 * @param str Provides string
 * @param str_len Maximum string length
 * @return The number of writen characters
 */
SRSRAN_API uint32_t srsran_csi_meas_info(const srsran_csi_trs_measurements_t* meas, char* str, uint32_t str_len);

/**
 * @brief Writes short Channel State Information measurement into a string
 * @param meas Provides the measurement
 * @param str Provides string
 * @param str_len Maximum string length
 * @return The number of writen characters
 */
SRSRAN_API uint32_t srsran_csi_meas_info_short(const srsran_csi_trs_measurements_t* meas, char* str, uint32_t str_len);

/**
 * @brief Converts a given string into a subcarrier spacing
 * @param str Provides the string
 * @return A valid subcarrier if the string is valid, srsran_subcarrier_spacing_invalid otherwise
 */
SRSRAN_API srsran_subcarrier_spacing_t srsran_subcarrier_spacing_from_str(const char* str);

/**
 * @brief Converts a given subcarrier spacing to string
 * @param scs Subcarrier spacing
 * @return A constant string pointer
 */
SRSRAN_API const char* srsran_subcarrier_spacing_to_str(srsran_subcarrier_spacing_t scs);

/**
 * @brief Combine Channel State Information from Tracking Reference Signals (CSI-TRS)
 * @param[in] a CSI-RS measurement
 * @param[in] b CSI-RS measurement
 * @param[out] dst Destination of the combined
 */
SRSRAN_API void srsran_combine_csi_trs_measurements(const srsran_csi_trs_measurements_t* a,
                                                    const srsran_csi_trs_measurements_t* b,
                                                    srsran_csi_trs_measurements_t*       dst);

/**
 * @brief Setup CORESET Zero from a configuration index
 * @remark Defined by TS 38.213 tables 13-1, 13-2, 13-3, 13-4, 13-5, 13-6,  13-7,  13-8,  13-9,  13-10
 * @param n_cell_id Physical Cell identifier
 * @param ssb_pointA_freq_offset_Hz Integer frequency offset in Hz between the SS/PBCH block center and pointA
 * @param ssb_scs SS/PBCH block subcarrier spacing
 * @param pdcch_scs PDCCH subcarrier spacing
 * @param idx CORESET Zero configuration index
 * @param[out] coreset Points to the resultant CORESET
 * @return SRSRAN_SUCCESS if the given inputs lead to a valid CORESET configuration, otherise SRSRAN_ERROR code
 */
SRSRAN_API int srsran_coreset_zero(uint32_t                    n_cell_id,
                                   uint32_t                    ssb_pointA_freq_offset_Hz,
                                   srsran_subcarrier_spacing_t ssb_scs,
                                   srsran_subcarrier_spacing_t pdcch_scs,
                                   uint32_t                    idx,
                                   srsran_coreset_t*           coreset);

/**
 * @brief Convert SSB pattern to string
 * @param pattern
 * @return a string describing the SSB pattern
 */
SRSRAN_API const char* srsran_ssb_pattern_to_str(srsran_ssb_patern_t pattern);

/**
 * @brief Convert string to SSB pattern
 * @param str String to convert
 * @return The pattern, SRSRAN_SSB_PATTERN_INVALID if string is invalid
 */
SRSRAN_API srsran_ssb_patern_t srsran_ssb_pattern_fom_str(const char* str);

/**
 * @brief Compares if two NR carrier structures are equal
 * @param a First carrier to compare
 * @param b Second carrier to compare
 * @return True if all the carrier structure fields are equal, otherwise false
 */
SRSRAN_API bool srsran_carrier_nr_equal(const srsran_carrier_nr_t* a, const srsran_carrier_nr_t* b);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_PHY_COMMON_NR_H
