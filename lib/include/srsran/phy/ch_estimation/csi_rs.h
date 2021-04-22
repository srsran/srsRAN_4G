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

#ifndef SRSRAN_CSI_RS_H_
#define SRSRAN_CSI_RS_H_

#include "csi_rs_cfg.h"
#include "srsran/phy/phch/phch_cfg_nr.h"
#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Number of frequency domain elements for Row 1
 */
#define SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW1 4

/**
 * @brief Number of frequency domain elements for Row 2
 */
#define SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW2 12

/**
 * @brief Number of frequency domain elements for Row 4
 */
#define SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW4 3

/**
 * @brief Number of frequency domain elements for other rows
 */
#define SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_OTHER 6

/**
 * @brief Measurement structure
 */
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

/**
 * @brief Calculates if the given periodicity implies a CSI-RS transmission in the given slot
 * @remark Described in TS 36.211 section 7.4.1.5.3 Mapping to physical resources
 * @param periodicity Periodicity configuration
 * @param slot_cfg Slot configuration
 * @return True if the periodicity configuration matches with the slot, false otherwise
 */
SRSRAN_API bool srsran_csi_rs_send(const srsran_csi_rs_period_and_offset_t* periodicity,
                                   const srsran_slot_cfg_t*                 slot_cfg);

/**
 * @brief Adds to a RE pattern list the RE used in a CSI-RS resource for all CDM grops. This is intended for generating
 * reserved RE pattern for PDSCH transmission.
 * @param carrier Provides carrier configuration
 * @param resource Provides a CSI-RS resource
 * @param nof_resources Provides the number of ZP-CSI-RS resources
 * @param l Symbol index in the slot
 * @param[out] rvd_mask Provides the reserved mask
 * @return SRSRAN_SUCCESS if the provided data is valid, and SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_csi_rs_append_resource_to_pattern(const srsran_carrier_nr_t*              carrier,
                                                        const srsran_csi_rs_resource_mapping_t* resource,
                                                        srsran_re_pattern_list_t*               re_pattern_list);

SRSRAN_API int srsran_csi_rs_nzp_put(const srsran_carrier_nr_t*          carrier,
                                     const srsran_slot_cfg_t*            slot_cfg,
                                     const srsran_csi_rs_nzp_resource_t* resource,
                                     cf_t*                               grid);

SRSRAN_API int srsran_csi_rs_nzp_measure(const srsran_carrier_nr_t*          carrier,
                                         const srsran_slot_cfg_t*            slot_cfg,
                                         const srsran_csi_rs_nzp_resource_t* resource,
                                         const cf_t*                         grid,
                                         srsran_csi_rs_measure_t*            measure);

SRSRAN_API uint32_t srsran_csi_rs_measure_info(const srsran_csi_rs_measure_t* measure, char* str, uint32_t str_len);

#endif // SRSRAN_CSI_RS_H_
