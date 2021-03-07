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

#ifndef SRSLTE_CSI_CFG_H
#define SRSLTE_CSI_CFG_H

#include "pucch_cfg_nr.h"
#include "srslte/config.h"
#include <stdint.h>

/**
 * @brief Maximum number of CSI report configurations defined in TS 38.331 maxNrofCSI-ReportConfigurations
 */
#define SRSLTE_CSI_MAX_NOF_REPORT 48

/**
 * @brief Maximum number of CSI-RS resources defined in TS 38.331 maxNrofCSI-ResourceConfigurations
 */
#define SRSLTE_CSI_MAX_NOF_RESOURCES 112
/**
 * @brief CSI report types defined in TS 38.331 CSI-ReportConfig
 */
typedef enum SRSLTE_API {
  SRSLTE_CSI_REPORT_TYPE_NONE = 0,
  SRSLTE_CSI_REPORT_TYPE_PERIODIC,
  SRSLTE_CSI_REPORT_TYPE_SEMI_PERSISTENT_ON_PUCCH,
  SRSLTE_CSI_REPORT_TYPE_SEMI_PERSISTENT_ON_PUSCH,
  SRSLTE_CSI_REPORT_TYPE_APERIODIC,
} srslte_csi_report_type_t;

/**
 * @brief CSI report quantities defined in TS 38.331 CSI-ReportConfig
 */
typedef enum SRSLTE_API {
  SRSLTE_CSI_REPORT_QUANTITY_NONE = 0,
  SRSLTE_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI,
  SRSLTE_CSI_REPORT_QUANTITY_CRI_RI_I1,
  SRSLTE_CSI_REPORT_QUANTITY_CRI_RI_I1_CQI,
  SRSLTE_CSI_REPORT_QUANTITY_CRI_RI_CQI,
  SRSLTE_CSI_REPORT_QUANTITY_CRI_RSRP,
  SRSLTE_CSI_REPORT_QUANTITY_SSB_INDEX_RSRP,
  SRSLTE_CSI_REPORT_QUANTITY_CRI_RI_LI_PMI_CQI
} srslte_csi_report_quantity_t;

/**
 * @brief CSI report frequency configuration defined in TS 38.331 CSI-ReportConfig
 */
typedef enum SRSLTE_API {
  SRSLTE_CSI_REPORT_FREQ_WIDEBAND = 0,
  SRSLTE_CSI_REPORT_FREQ_SUBBAND
} srslte_csi_report_freq_t;

/**
 * @brief CQI table selection
 */
typedef enum SRSLTE_API {
  SRSLTE_CSI_CQI_TABLE_1 = 0,
  SRSLTE_CSI_CQI_TABLE_2,
  SRSLTE_CSI_CQI_TABLE_3,
} srslte_csi_cqi_table_t;

/**
 * @brief CSI periodic report configuration from upper layers
 * @remark Described in TS 38.331 CSI-ReportConfig
 */
typedef struct SRSLTE_API {
  uint32_t                   period;   ///< Period in slots
  uint32_t                   offset;   ///< Offset from beginning of the period in slots
  srslte_pucch_nr_resource_t resource; ///< PUCCH resource to use for reporting
} srslte_csi_periodic_report_cfg_t;

/**
 * @brief CSI report configuration from higher layers
 */
typedef struct SRSLTE_API {
  uint32_t                 channel_meas_id;     ///< Channel measurement resource identifier
  uint32_t                 interf_meas_id;      ///< Interference measurement resource identifier
  bool                     interf_meas_present; ///< Indicates if interference measurement identifier is present
  srslte_csi_report_type_t type;                ///< CSI report type (none, periodic, semiPersistentOnPUCCH, ...)
  union {
    void*                            none;     ///< Reserved, no configured
    srslte_csi_periodic_report_cfg_t periodic; ///< Used for periodic reporting
    // ... add here other types
  };
  srslte_csi_report_quantity_t quantity;  ///< Report quantity
  srslte_csi_cqi_table_t       cqi_table; ///< CQI table selection
  srslte_csi_report_freq_t     freq_cfg;  ///< Determine whether it is wideband or subband
} srslte_csi_hl_report_cfg_t;

/**
 * @brief General CSI configuration provided by higher layers
 */
typedef struct SRSLTE_API {
  srslte_csi_hl_report_cfg_t reports[SRSLTE_CSI_MAX_NOF_REPORT]; ///< CSI report configuration
  // ... add here physical CSI measurement sets
} srslte_csi_hl_cfg_t;

/**
 * @brief Generic measurement structure
 */
typedef struct SRSLTE_API {
  uint32_t cri;               ///< CSI-RS Resource Indicator
  float    wideband_rsrp_dBm; ///< Measured NZP-CSI-RS RSRP (Ignore for IM-CSI-RS)
  float    wideband_epre_dBm; ///< Measured EPRE
  float    wideband_snr_db;   ///< SNR calculated from NZP-CSI-RS RSRP and EPRE (Ignore for IM-CSI-RS)

  // Resource set context
  uint32_t nof_ports; ///< Number of antenna ports
  uint32_t K_csi_rs;  ///< Number of CSI-RS in the corresponding resource set
} srslte_csi_measurements_t;

/**
 * @brief CSI report configuration
 */
typedef struct SRSLTE_API {
  srslte_csi_report_type_t     type;           ///< CSI report type (none, periodic, semiPersistentOnPUCCH, ...)
  srslte_csi_report_quantity_t quantity;       ///< Report quantity
  srslte_pucch_nr_resource_t   pucch_resource; ///< PUCCH resource to use for periodic reporting
  srslte_csi_report_freq_t     freq_cfg;       ///< Determine whether it is wideband or subband

  // Resource set context
  uint32_t nof_ports; ///< Number of antenna ports
  uint32_t K_csi_rs;  ///< Number of CSI-RS in the corresponding resource set
} srslte_csi_report_cfg_t;

/**
 * @brief Wideband CSI report values
 */
typedef struct SRSLTE_API {
  uint32_t ri;
  uint32_t pmi;
  uint32_t cqi;
} srslte_csi_report_wideband_cri_ri_pmi_cqi_t;

/**
 * @brief Unified CSI report values
 */
typedef struct SRSLTE_API {
  uint32_t cri; ///< CSI-RS Resource Indicator
  union {
    void*                                       none;
    srslte_csi_report_wideband_cri_ri_pmi_cqi_t wideband_cri_ri_pmi_cqi;
  };
  bool valid; ///< Used by receiver only
} srslte_csi_report_value_t;

/**
 * @brief Complete report configuration and value
 */
typedef struct SRSLTE_API {
  srslte_csi_report_cfg_t   cfg[SRSLTE_CSI_MAX_NOF_REPORT];   ///< Configuration ready for encoding
  srslte_csi_report_value_t value[SRSLTE_CSI_MAX_NOF_REPORT]; ///< Quantified values
  uint32_t                  nof_reports;                      ///< Total number of reports to transmit
} srslte_csi_reports_t;

#endif // SRSLTE_CSI_CFG_H
