/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/******************************************************************************
 *  File:         pdsch_cfg_nr.h
 *
 *  Description:  Physical downlink shared channel configuration
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_PHCH_CFG_NR_H
#define SRSLTE_PHCH_CFG_NR_H

#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/phch/sch_cfg_nr.h"
#include "srslte/phy/phch/uci_cfg_nr.h"

/**
 * @brief PDSCH DMRS type
 */
typedef enum {
  srslte_dmrs_sch_type_1 = 0, // 1 pilot every 2 sub-carriers (default)
  srslte_dmrs_sch_type_2      // 2 consecutive pilots every 6 sub-carriers
} srslte_dmrs_sch_type_t;

/**
 * @brief PDSCH DMRS length in symbols
 */
typedef enum {
  srslte_dmrs_sch_len_1 = 0, // single, 1 symbol long (default)
  srslte_dmrs_sch_len_2      // double, 2 symbol long
} srslte_dmrs_sch_len_t;

/**
 * @brief Determines whether the first pilot goes into symbol index 2 or 3
 */
typedef enum {
  srslte_dmrs_sch_typeA_pos_2 = 0, // Start in slot symbol index 2 (default)
  srslte_dmrs_sch_typeA_pos_3      // Start in slot symbol index 3
} srslte_dmrs_sch_typeA_pos_t;

/**
 * @brief Determines additional symbols if possible to be added
 */
typedef enum {
  srslte_dmrs_sch_add_pos_2 = 0,
  srslte_dmrs_sch_add_pos_0,
  srslte_dmrs_sch_add_pos_1,
  srslte_dmrs_sch_add_pos_3
} srslte_dmrs_sch_add_pos_t;

/**
 * @brief Provides PDSCH DMRS configuration
 * @remark Parameters described in TS 38.331 V15.10.0
 */
typedef struct {
  /// Parameters provided by IE DMRS-DownlinkConfig
  srslte_dmrs_sch_type_t    type;
  srslte_dmrs_sch_add_pos_t additional_pos;
  srslte_dmrs_sch_len_t     length;
  bool                      scrambling_id0_present;
  uint32_t                  scrambling_id0;
  bool                      scrambling_id1_present;
  uint32_t                  scrambling_id1;

  /// Parameters provided by ServingCellConfigCommon
  srslte_dmrs_sch_typeA_pos_t typeA_pos;
  bool                        lte_CRS_to_match_around;

  /// Parameters provided by FeatureSetDownlink-v1540
  bool additional_DMRS_DL_Alt;

} srslte_dmrs_sch_cfg_t;

/**
 * @brief Common flatten PDSCH and PUSCH time domain allocation parameters
 * @remark Described in TS 38.331 V15.10.0 Section PDSCH-TimeDomainResourceAllocationList
 * @remark Described in TS 38.331 V15.10.0 Section PUSCH-TimeDomainResourceAllocationList
 */
typedef struct SRSLTE_API {
  /// For PDSCH Slot offset between DCI and its scheduled PDSCH
  /// For PUSCH parameter K2
  uint32_t k;

  /// SCH mapping type
  srslte_sch_mapping_type_t mapping_type;

  /// An index giving valid combinations of start symbol and length (jointly encoded) as start and length indicator
  /// (SLIV). The network configures the field so that the allocation does not cross the slot boundary
  uint32_t sliv;

} srslte_sch_time_ra_t;

/**
 * @brief PDSCH grant information provided by the Downlink Control Information (DCI)
 */
typedef struct SRSLTE_API {
  /// UE identifier
  uint16_t           rnti;
  srslte_rnti_type_t rnti_type;

  /// Time domain resources
  uint32_t                  k; // k0 for PDSCH, k2 for PUSCH
  uint32_t                  S;
  uint32_t                  L;
  srslte_sch_mapping_type_t mapping;

  /// Frequency domain resources
  bool     prb_idx[SRSLTE_MAX_PRB_NR];
  uint32_t nof_prb;

  /// Number of DMRS groups without data
  /// Described in TS 38.214 Section 5.1.6.2
  uint32_t nof_dmrs_cdm_groups_without_data;

  /// Linear DMRS power offset. Zero means unset and it is equivalent to one.
  /// For PUSCH, see TS 38.214 Table 6.2.2-1 for more information
  float beta_dmrs;

  /// Spatial resources
  uint32_t nof_layers;

  /// DMRS Scrambling sequence initialization (false: 0 or true: 1)
  bool n_scid;

  /// DCI information
  srslte_dci_format_nr_t     dci_format;
  srslte_search_space_type_t dci_search_space;

  /// Transport block
  uint32_t tb_scaling_field;
  /// ....

  srslte_sch_tb_t tb[SRSLTE_MAX_TB];
} srslte_sch_grant_nr_t;

/**
 * @brief flatten SCH configuration parameters provided by higher layers
 * @remark Described in TS 38.331 V15.10.0 Section PDSCH-Config
 * @remark Described in TS 38.331 V15.10.0 Section PUSCH-Config
 */
typedef struct SRSLTE_API {
  // Serving cell parameters
  uint32_t                    scs_cfg; // Subcarrier spacing configuration
  srslte_dmrs_sch_typeA_pos_t typeA_pos;

  bool     scrambling_id_present;
  uint32_t scambling_id; // Identifier used to initialize data scrambling (0-1023)

  struct {
    srslte_dmrs_sch_type_t    type;
    srslte_dmrs_sch_add_pos_t additional_pos;
    srslte_dmrs_sch_len_t     length;
    bool                      scrambling_id0_present;
    uint32_t                  scrambling_id0;
    bool                      scrambling_id1_present;
    uint32_t                  scrambling_id1;
    bool                      present;
  } dmrs_typeA;

  struct {
    srslte_dmrs_sch_type_t    type;
    srslte_dmrs_sch_add_pos_t additional_pos;
    srslte_dmrs_sch_len_t     length;
    bool                      scrambling_id0_present;
    uint32_t                  scrambling_id0;
    bool                      scrambling_id1_present;
    uint32_t                  scrambling_id1;
    bool                      present;
  } dmrs_typeB;

  srslte_sch_time_ra_t common_time_ra[SRSLTE_MAX_NOF_DL_ALLOCATION];
  uint32_t             nof_common_time_ra;

  srslte_sch_time_ra_t dedicated_time_ra[SRSLTE_MAX_NOF_DL_ALLOCATION];
  uint32_t             nof_dedicated_time_ra;

  bool rbg_size_cfg_1; ///< RBG size configuration (1 or 2)

  srslte_sch_cfg_t sch_cfg; ///< Common shared channel parameters
} srslte_sch_hl_cfg_nr_t;

/**
 * @brief Common NR-SCH (PDSCH and PUSCH for NR) configuration
 */
typedef struct SRSLTE_API {
  bool     scrambling_id_present;
  uint32_t scambling_id; // Identifier used to initialize data scrambling (0-1023)

  srslte_dmrs_sch_cfg_t dmrs;
  srslte_sch_grant_nr_t grant;

  srslte_sch_cfg_t sch_cfg; ///< Common shared channel parameters

  /// PUSCH only parameters
  srslte_uci_cfg_nr_t uci; ///< Uplink Control Information configuration
  bool                enable_transform_precoder;
  float               beta_harq_ack_offset;
  float               beta_csi_part1_offset;
  float               scaling;
} srslte_sch_cfg_nr_t;

#endif // SRSLTE_PHCH_CFG_NR_H
