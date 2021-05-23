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
 * @param resource Provides any CSI-RS resource mapping
 * @param nof_resources Provides the number of ZP-CSI-RS resources
 * @param l Symbol index in the slot
 * @param[out] rvd_mask Provides the reserved mask
 * @return SRSRAN_SUCCESS if the provided data is valid, and SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_csi_rs_append_resource_to_pattern(const srsran_carrier_nr_t*              carrier,
                                                        const srsran_csi_rs_resource_mapping_t* resource,
                                                        srsran_re_pattern_list_t*               re_pattern_list);

/**
 * @brief Puts in the provided resource grid NZP-CSI-RS signals given by a NZP-CSI-RS resource
 *
 * @note it does not check if the provided slot matches with the periodicity of the provided NZP-CSI-RS resource
 *
 * @param carrier Provides carrier configuration
 * @param slot_cfg Provides current slot configuration
 * @param resource Provides a NZP-CSI-RS resource
 * @param[out] grid Resource grid
 * @return SRSRAN_SUCCESS if the arguments and the resource are valid. SRSRAN_ERROR code otherwise.
 */
SRSRAN_API int srsran_csi_rs_nzp_put_resource(const srsran_carrier_nr_t*          carrier,
                                              const srsran_slot_cfg_t*            slot_cfg,
                                              const srsran_csi_rs_nzp_resource_t* resource,
                                              cf_t*                               grid);
/**
 * @brief Puts in the provided resource grid NZP-CSI-RS signals given by a NZP-CSI-RS resource set if their periodicity
 * configuration matches with the provided slot
 *
 * @param carrier Provides carrier configuration
 * @param slot_cfg Provides current slot configuration
 * @param set Provides a NZP-CSI-RS resource set
 * @param[out] grid Resource grid
 * @return The number of NZP-CSI-RS resources that have been scheduled for this slot if the arguments and the resource
 * are valid. SRSRAN_ERROR code otherwise.
 */
SRSRAN_API int srsran_csi_rs_nzp_put_set(const srsran_carrier_nr_t*     carrier,
                                         const srsran_slot_cfg_t*       slot_cfg,
                                         const srsran_csi_rs_nzp_set_t* set,
                                         cf_t*                          grid);

SRSRAN_API int srsran_csi_rs_nzp_measure(const srsran_carrier_nr_t*          carrier,
                                         const srsran_slot_cfg_t*            slot_cfg,
                                         const srsran_csi_rs_nzp_resource_t* resource,
                                         const cf_t*                         grid,
                                         srsran_csi_trs_measurements_t*      measure);

/**
 * @brief Performs measurements of NZP-CSI-RS resource set flagged as TRS
 *
 * @attention It expects:
 * - The NZP-CSI-RS resource set shall be flagged as TRS; and
 * - at least a pair of active NZP-CSR-RS per measurement opportunity with their first transmission symbol in ascending
 * order.
 *
 * @note It performs the following wideband measurements:
 * - RSRP (linear and dB),
 * - EPRE (linear and dB),
 * - Noise (linear and dB),
 * - SNR (dB),
 * - average delay (microseconds), and
 * - CFO (Hz)
 *
 * @note It is intended for fine tracking of synchronization error (average delay) and carrier frequency error
 *
 * @param carrier Provides carrier configuration
 * @param slot_cfg Provides current slot
 * @param set Provides NZP-CSI-RS resource
 * @param grid Resource grid
 * @param measure Provides measurement
 * @return The number of NZP-CSI-RS resources scheduled for this TTI if the configuration is right, SRSRAN_ERROR code if
 * the configuration is invalid
 */
SRSRAN_API int srsran_csi_rs_nzp_measure_trs(const srsran_carrier_nr_t*     carrier,
                                             const srsran_slot_cfg_t*       slot_cfg,
                                             const srsran_csi_rs_nzp_set_t* set,
                                             const cf_t*                    grid,
                                             srsran_csi_trs_measurements_t* measure);

SRSRAN_API uint32_t srsran_csi_rs_measure_info(const srsran_csi_trs_measurements_t* measure,
                                               char*                                str,
                                               uint32_t                             str_len);

/**
 * @brief Performs channel measurements of NZP-CSI-RS resource set for CSI reports
 *
 * @note It performs the following wideband measurements:
 * - RSRP (dB),
 * - EPRE (dB),
 * - SNR (dB),
 *
 * @note It is intended for generating CSI wideband measurements that are used for generating CSI reporting
 *
 * @param carrier Provides carrier configuration
 * @param slot_cfg Provides current slot
 * @param set Provides NZP-CSI-RS resource
 * @param grid Resource grid
 * @param measure Provides CSI measurement
 * @return The number of NZP-CSI-RS resources scheduled for this slot if the configuration is right, SRSRAN_ERROR code
 * if the configuration is invalid
 */
SRSRAN_API int srsran_csi_rs_nzp_measure_channel(const srsran_carrier_nr_t*         carrier,
                                                 const srsran_slot_cfg_t*           slot_cfg,
                                                 const srsran_csi_rs_nzp_set_t*     set,
                                                 const cf_t*                        grid,
                                                 srsran_csi_channel_measurements_t* measure);

/**
 * @brief Performs measurements of ZP-CSI-RS resource set for CSI reports
 *
 * @note It performs the following wideband measurememnts:
 * - EPRE (dB)
 *
 * @note It is intended for measuring interference
 *
 * @param carrier Provides carrier configuration
 * @param slot_cfg Provides current slot
 * @param set Provides ZP-CSI-RS resource
 * @param grid Resource grid
 * @param measure Provides CSI measurement
 * @return The number of ZP-CSI-RS resources scheduled for this slot if the configuration is right, SRSRAN_ERROR code if
 * the configuration is invalid
 */
SRSRAN_API int srsran_csi_rs_zp_measure_channel(const srsran_carrier_nr_t*         carrier,
                                                const srsran_slot_cfg_t*           slot_cfg,
                                                const srsran_csi_rs_zp_set_t*      set,
                                                const cf_t*                        grid,
                                                srsran_csi_channel_measurements_t* measure);

#endif // SRSRAN_CSI_RS_H_
