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

#ifndef SRSRAN_CSI_CFG_H
#define SRSRAN_CSI_CFG_H

#include "pucch_cfg_nr.h"
#include "srsran/config.h"
#include <stdint.h>

/**
 * @brief Maximum number of CSI report configurations defined in TS 38.331 maxNrofCSI-ReportConfigurations
 */
#define SRSRAN_CSI_MAX_NOF_REPORT 48

/**
 * @brief Maximum number of CSI-RS resources defined in TS 38.331 maxNrofCSI-ResourceConfigurations
 */
#define SRSRAN_CSI_MAX_NOF_RESOURCES 112

/**
 * @brief CSI report types defined in TS 38.331 CSI-ReportConfig
 */
typedef enum SRSRAN_API {
  SRSRAN_CSI_REPORT_TYPE_NONE = 0,
  SRSRAN_CSI_REPORT_TYPE_PERIODIC,
  SRSRAN_CSI_REPORT_TYPE_SEMI_PERSISTENT_ON_PUCCH,
  SRSRAN_CSI_REPORT_TYPE_SEMI_PERSISTENT_ON_PUSCH,
  SRSRAN_CSI_REPORT_TYPE_APERIODIC,
} srsran_csi_report_type_t;

/**
 * @brief CSI report quantities defined in TS 38.331 CSI-ReportConfig
 */
typedef enum SRSRAN_API {
  SRSRAN_CSI_REPORT_QUANTITY_NONE = 0,
  SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI,
  SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_I1,
  SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_I1_CQI,
  SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_CQI,
  SRSRAN_CSI_REPORT_QUANTITY_CRI_RSRP,
  SRSRAN_CSI_REPORT_QUANTITY_SSB_INDEX_RSRP,
  SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_LI_PMI_CQI
} srsran_csi_report_quantity_t;

/**
 * @brief CSI report frequency configuration defined in TS 38.331 CSI-ReportConfig
 */
typedef enum SRSRAN_API {
  SRSRAN_CSI_REPORT_FREQ_WIDEBAND = 0,
  SRSRAN_CSI_REPORT_FREQ_SUBBAND
} srsran_csi_report_freq_t;

/**
 * @brief CQI table selection
 */
typedef enum SRSRAN_API {
  SRSRAN_CSI_CQI_TABLE_1 = 0,
  SRSRAN_CSI_CQI_TABLE_2,
  SRSRAN_CSI_CQI_TABLE_3,
} srsran_csi_cqi_table_t;

/**
 * @brief CSI periodic report configuration from upper layers
 * @remark Described in TS 38.331 CSI-ReportConfig
 */
typedef struct SRSRAN_API {
  uint32_t                   period;   ///< Period in slots
  uint32_t                   offset;   ///< Offset from beginning of the period in slots
  srsran_pucch_nr_resource_t resource; ///< PUCCH resource to use for reporting
} srsran_csi_periodic_report_cfg_t;

/**
 * @brief CSI report configuration from higher layers
 */
typedef struct SRSRAN_API {
  uint32_t                 channel_meas_id;     ///< Channel measurement resource identifier
  uint32_t                 interf_meas_id;      ///< Interference measurement resource identifier
  bool                     interf_meas_present; ///< Indicates if interference measurement identifier is present
  srsran_csi_report_type_t type;                ///< CSI report type (none, periodic, semiPersistentOnPUCCH, ...)
  union {
    void*                            none;     ///< Reserved, no configured
    srsran_csi_periodic_report_cfg_t periodic; ///< Used for periodic reporting
    // ... add here other types
  };
  srsran_csi_report_quantity_t quantity;  ///< Report quantity
  srsran_csi_cqi_table_t       cqi_table; ///< CQI table selection
  srsran_csi_report_freq_t     freq_cfg;  ///< Determine whether it is wideband or subband
} srsran_csi_hl_report_cfg_t;

/**
 * @brief General CSI configuration provided by higher layers
 */
typedef struct SRSRAN_API {
  srsran_csi_hl_report_cfg_t reports[SRSRAN_CSI_MAX_NOF_REPORT]; ///< CSI report configuration
  // ... add here physical CSI measurement sets
} srsran_csi_hl_cfg_t;

/**
 * @brief Generic measurement structure
 */
typedef struct SRSRAN_API {
  uint32_t cri;               ///< CSI-RS Resource Indicator
  float    wideband_rsrp_dBm; ///< Measured NZP-CSI-RS RSRP (Ignore for IM-CSI-RS)
  float    wideband_epre_dBm; ///< Measured EPRE
  float    wideband_snr_db;   ///< SNR calculated from NZP-CSI-RS RSRP and EPRE (Ignore for IM-CSI-RS)

  // Resource set context
  uint32_t nof_ports; ///< Number of antenna ports
  uint32_t K_csi_rs;  ///< Number of CSI-RS in the corresponding resource set
} srsran_csi_measurements_t;

/**
 * @brief CSI report configuration
 */
typedef struct SRSRAN_API {
  srsran_csi_report_type_t     type;           ///< CSI report type (none, periodic, semiPersistentOnPUCCH, ...)
  srsran_csi_report_quantity_t quantity;       ///< Report quantity
  srsran_pucch_nr_resource_t   pucch_resource; ///< PUCCH resource to use for periodic reporting
  srsran_csi_report_freq_t     freq_cfg;       ///< Determine whether it is wideband or subband

  // Resource set context
  uint32_t nof_ports; ///< Number of antenna ports
  uint32_t K_csi_rs;  ///< Number of CSI-RS in the corresponding resource set
  bool     has_part2; ///< Set to true if the report has part 2
} srsran_csi_report_cfg_t;

/**
 * @brief Wideband CSI report values
 */
typedef struct SRSRAN_API {
  uint32_t ri;
  uint32_t pmi;
  uint32_t cqi;
} srsran_csi_report_wideband_cri_ri_pmi_cqi_t;

/**
 * @brief Unified CSI report values
 */
typedef struct SRSRAN_API {
  uint32_t cri; ///< CSI-RS Resource Indicator
  union {
    void*                                       none;
    srsran_csi_report_wideband_cri_ri_pmi_cqi_t wideband_cri_ri_pmi_cqi;
  };
  bool valid; ///< Used by receiver only
} srsran_csi_report_value_t;

/**
 * @brief Complete report configuration and value
 */
typedef struct SRSRAN_API {
  srsran_csi_report_cfg_t   cfg[SRSRAN_CSI_MAX_NOF_REPORT];   ///< Configuration ready for encoding
  srsran_csi_report_value_t value[SRSRAN_CSI_MAX_NOF_REPORT]; ///< Quantified values
  uint32_t                  nof_reports;                      ///< Total number of reports to transmit
} srsran_csi_reports_t;

#endif // SRSRAN_CSI_CFG_H
