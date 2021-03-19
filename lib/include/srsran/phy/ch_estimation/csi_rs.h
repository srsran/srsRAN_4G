/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_CSI_RS_H_
#define SRSRAN_CSI_RS_H_

#include "srsran/config.h"
#include "srsran/phy/common/phy_common_nr.h"
#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

#define SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW1 4
#define SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW2 12
#define SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW4 3
#define SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_OTHER 6
#define SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_MAX 12

typedef enum SRSRAN_API {
  srsran_csi_rs_resource_mapping_row_1 = 0,
  srsran_csi_rs_resource_mapping_row_2,
  srsran_csi_rs_resource_mapping_row_4,
  srsran_csi_rs_resource_mapping_row_other,
} srsran_csi_rs_resource_mapping_row_t;

typedef enum SRSRAN_API {
  srsran_csi_rs_resource_mapping_density_three = 0,
  srsran_csi_rs_resource_mapping_density_dot5_even,
  srsran_csi_rs_resource_mapping_density_dot5_odd,
  srsran_csi_rs_resource_mapping_density_one,
  srsran_csi_rs_resource_mapping_density_spare
} srsran_csi_rs_density_t;

typedef enum SRSRAN_API {
  srsran_csi_rs_cdm_nocdm = 0,
  srsran_csi_rs_cdm_fd_cdm2,
  srsran_csi_rs_cdm_cdm4_fd2_td2,
  srsran_csi_rs_cdm_cdm8_fd2_td4
} srsran_csi_rs_cdm_t;

/**
 * @brief Contains CSI-FrequencyOccupation flattened configuration
 */
typedef struct SRSRAN_API {
  uint32_t start_rb; ///< PRB where this CSI resource starts in relation to common resource block #0 (CRB#0) on the
  ///< common resource block grid. Only multiples of 4 are allowed (0, 4, ..., 274)

  uint32_t nof_rb; ///< Number of PRBs across which this CSI resource spans. Only multiples of 4 are allowed. The
  ///< smallest configurable number is the minimum of 24 and the width of the associated BWP. If the
  ///< configured value is larger than the width of the corresponding BWP, the UE shall assume that the
  ///< actual CSI-RS bandwidth is equal to the width of the BWP.
} srsran_csi_rs_freq_occupation_t;

/**
 * @brief Contains CSI-ResourcePeriodicityAndOffset flattened configuration
 */
typedef struct SRSRAN_API {
  uint32_t period; // 4,5,8,10,16,20,32,40,64,80,160,320,640
  uint32_t offset; // 0..period-1
} srsran_csi_rs_period_and_offset_t;

/**
 * @brief Contains CSI-RS-ResourceMapping flattened configuration
 */
typedef struct SRSRAN_API {
  srsran_csi_rs_resource_mapping_row_t row;
  bool                                 frequency_domain_alloc[SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_MAX];
  uint32_t                             ports;             // 1, 2, 4, 8, 12, 16, 24, 32
  uint32_t                             first_symbol_idx;  // 0..13
  uint32_t                             first_symbol_idx2; // 2..12 (set to 0 for disabled)
  srsran_csi_rs_cdm_t                  cdm;
  srsran_csi_rs_density_t              density;
  srsran_csi_rs_freq_occupation_t      freq_band;
} srsran_csi_rs_resource_mapping_t;

/**
 * @brief Contains NZP-CSI-RS-Resource flattened configuration
 */
typedef struct SRSRAN_API {
  srsran_csi_rs_resource_mapping_t resource_mapping;

  float power_control_offset;    // -8..15 dB
  float power_control_offset_ss; // -3, 0, 3, 6 dB

  uint32_t scrambling_id; // 0..1023

  srsran_csi_rs_period_and_offset_t periodicity;

} srsran_csi_rs_nzp_resource_t;

SRSRAN_API int srsran_csi_rs_nzp_put(const srsran_carrier_nr_t*          carrier,
                                     const srsran_slot_cfg_t*            slot_cfg,
                                     const srsran_csi_rs_nzp_resource_t* resource,
                                     cf_t*                               grid);

typedef struct SRSRAN_API {
  float    rsrp;
  float    rsrp_dB;
  float    epre;
  float    epre_dB;
  float    n0;
  float    n0_dB;
  float    snr_dB;
  uint32_t nof_re;
} srsran_csi_rs_measure_t;

SRSRAN_API int srsran_csi_rs_nzp_measure(const srsran_carrier_nr_t*          carrier,
                                         const srsran_slot_cfg_t*            slot_cfg,
                                         const srsran_csi_rs_nzp_resource_t* resource,
                                         const cf_t*                         grid,
                                         srsran_csi_rs_measure_t*            measure);

SRSRAN_API uint32_t srsran_csi_rs_measure_info(const srsran_csi_rs_measure_t* measure, char* str, uint32_t str_len);

#endif // SRSRAN_CSI_RS_H_
