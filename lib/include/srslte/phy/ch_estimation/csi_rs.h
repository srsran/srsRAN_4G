/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSLTE_CSI_RS_H_
#define SRSLTE_CSI_RS_H_

#include "srslte/config.h"
#include "srslte/phy/common/phy_common_nr.h"
#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

#define SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW1 4
#define SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW2 12
#define SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW4 3
#define SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_OTHER 6
#define SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_MAX 12

typedef enum SRSLTE_API {
  srslte_csi_rs_resource_mapping_row_1 = 0,
  srslte_csi_rs_resource_mapping_row_2,
  srslte_csi_rs_resource_mapping_row_4,
  srslte_csi_rs_resource_mapping_row_other,
} srslte_csi_rs_resource_mapping_row_t;

typedef enum SRSLTE_API {
  srslte_csi_rs_resource_mapping_density_three = 0,
  srslte_csi_rs_resource_mapping_density_dot5_even,
  srslte_csi_rs_resource_mapping_density_dot5_odd,
  srslte_csi_rs_resource_mapping_density_one,
  srslte_csi_rs_resource_mapping_density_spare
} srslte_csi_rs_density_t;

typedef enum SRSLTE_API {
  srslte_csi_rs_cdm_nocdm = 0,
  srslte_csi_rs_cdm_fd_cdm2,
  srslte_csi_rs_cdm_cdm4_fd2_td2,
  srslte_csi_rs_cdm_cdm8_fd2_td4
} srslte_csi_rs_cdm_t;

/**
 * @brief Contains CSI-FrequencyOccupation flattened configuration
 */
typedef struct SRSLTE_API {
  uint32_t start_rb; ///< PRB where this CSI resource starts in relation to common resource block #0 (CRB#0) on the
  ///< common resource block grid. Only multiples of 4 are allowed (0, 4, ..., 274)

  uint32_t nof_rb; ///< Number of PRBs across which this CSI resource spans. Only multiples of 4 are allowed. The
  ///< smallest configurable number is the minimum of 24 and the width of the associated BWP. If the
  ///< configured value is larger than the width of the corresponding BWP, the UE shall assume that the
  ///< actual CSI-RS bandwidth is equal to the width of the BWP.
} srslte_csi_rs_freq_occupation_t;

/**
 * @brief Contains CSI-ResourcePeriodicityAndOffset flattened configuration
 */
typedef struct SRSLTE_API {
  uint32_t period; // 4,5,8,10,16,20,32,40,64,80,160,320,640
  uint32_t offset; // 0..period-1
} srslte_csi_rs_period_and_offset_t;

/**
 * @brief Contains CSI-RS-ResourceMapping flattened configuration
 */
typedef struct SRSLTE_API {
  srslte_csi_rs_resource_mapping_row_t row;
  bool                                 frequency_domain_alloc[SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_MAX];
  uint32_t                             ports;             // 1, 2, 4, 8, 12, 16, 24, 32
  uint32_t                             first_symbol_idx;  // 0..13
  uint32_t                             first_symbol_idx2; // 2..12 (set to 0 for disabled)
  srslte_csi_rs_cdm_t                  cdm;
  srslte_csi_rs_density_t              density;
  srslte_csi_rs_freq_occupation_t      freq_band;
} srslte_csi_rs_resource_mapping_t;

/**
 * @brief Contains NZP-CSI-RS-Resource flattened configuration
 */
typedef struct SRSLTE_API {
  srslte_csi_rs_resource_mapping_t resource_mapping;

  float power_control_offset;    // -8..15 dB
  float power_control_offset_ss; // -3, 0, 3, 6 dB

  uint32_t scrambling_id; // 0..1023

  srslte_csi_rs_period_and_offset_t periodicity;

} srslte_csi_rs_nzp_resource_t;

SRSLTE_API int srslte_csi_rs_nzp_put(const srslte_carrier_nr_t*          carrier,
                                     const srslte_slot_cfg_t*            slot_cfg,
                                     const srslte_csi_rs_nzp_resource_t* resource,
                                     cf_t*                               grid);

typedef struct SRSLTE_API {
  float    rsrp;
  float    rsrp_dB;
  float    epre;
  float    epre_dB;
  float    n0;
  float    n0_dB;
  float    snr_dB;
  uint32_t nof_re;
} srslte_csi_rs_measure_t;

SRSLTE_API int srslte_csi_rs_nzp_measure(const srslte_carrier_nr_t*          carrier,
                                         const srslte_slot_cfg_t*            slot_cfg,
                                         const srslte_csi_rs_nzp_resource_t* resource,
                                         const cf_t*                         grid,
                                         srslte_csi_rs_measure_t*            measure);

SRSLTE_API uint32_t srslte_csi_rs_measure_info(const srslte_csi_rs_measure_t* measure, char* str, uint32_t str_len);

#endif // SRSLTE_CSI_RS_H_
