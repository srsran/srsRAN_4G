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

#include "srslte/config.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/utils/debug.h"
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
  uint32_t start_rb; // 0..274
  uint32_t nof_rb;   // 24..276
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

  int8_t power_control_offset;    // -8..15 dB
  int8_t power_control_offset_ss; // -3, 0, 3, 6 dB

  uint32_t scrambling_id; // 0..1023

  srslte_csi_rs_period_and_offset_t periodicity;

} srslte_csi_rs_nzp_resource_t;

#define CSI_RS_MAX_CDM_GROUP 16

static int csi_rs_location_f(const srslte_csi_rs_resource_mapping_t* resource, uint32_t i)
{
  uint32_t count           = 0;
  uint32_t nof_freq_domain = 0;
  switch (resource->row) {
    case srslte_csi_rs_resource_mapping_row_1:
      nof_freq_domain = SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW1;
      break;
    case srslte_csi_rs_resource_mapping_row_2:
      nof_freq_domain = SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW2;
      break;
    case srslte_csi_rs_resource_mapping_row_4:
      nof_freq_domain = SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW4;
      break;
    case srslte_csi_rs_resource_mapping_row_other:
      nof_freq_domain = SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_OTHER;
      break;
  }

  for (uint32_t j = 0; j < nof_freq_domain; j++) {
    if (resource->frequency_domain_alloc[j]) {
      count++;
    }

    if (count == i) {
      return i;
    }
  }

  return SRSLTE_ERROR;
}

static int csi_rs_location_get_k_list(const srslte_csi_rs_resource_mapping_t* resource,
                                      uint32_t                                k_list[CSI_RS_MAX_CDM_GROUP])
{
  int k0 = csi_rs_location_f(resource, 1);
  //  int k1 = csi_rs_location_f(resource, 2);
  //  int k2 = csi_rs_location_f(resource, 3);
  //  int k3 = csi_rs_location_f(resource, 4);

  if (k0 < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Row 1
  if (resource->row == srslte_csi_rs_resource_mapping_row_1 && resource->ports == 1 &&
      resource->density == srslte_csi_rs_resource_mapping_density_three && resource->cdm == srslte_csi_rs_cdm_nocdm) {
    k_list[0] = k0;
    k_list[1] = k0 + 4;
    k_list[3] = k0 + 8;
    return 3;
  }

  return SRSLTE_ERROR;
}

static int csi_rs_location_get_l_list(const srslte_csi_rs_resource_mapping_t* resource,
                                      uint32_t                                l_list[CSI_RS_MAX_CDM_GROUP])
{
  uint32_t l0 = resource->first_symbol_idx;

  if (l0 > 13) {
    return SRSLTE_ERROR;
  }

  //  uint32_t l1 = resource->first_symbol_idx2;
  //  if (l1 < 2 || l1 > 12) {
  //    return SRSLTE_ERROR;
  //  }

  // Row 1
  if (resource->row == srslte_csi_rs_resource_mapping_row_1 && resource->ports == 1 &&
      resource->density == srslte_csi_rs_resource_mapping_density_three && resource->cdm == srslte_csi_rs_cdm_nocdm) {
    l_list[0] = l0;
    return 1;
  }

  return SRSLTE_ERROR;
}

int srslte_csi_rs_put(const srslte_carrier_nr_t* carrier)
{
  return SRSLTE_SUCCESS;
}