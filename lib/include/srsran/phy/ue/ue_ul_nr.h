/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

/******************************************************************************
 *  @file ue_dl_nr.h
 *
 *  Description:  NR UE uplink physical layer procedures for data
 *
 *                This module is a frontend to all the uplink data channel processing modules.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_UE_UL_DATA_H
#define SRSRAN_UE_UL_DATA_H

#include "srsran/phy/ch_estimation/dmrs_sch.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/phch/phch_cfg_nr.h"
#include "srsran/phy/phch/pucch_cfg_nr.h"
#include "srsran/phy/phch/pucch_nr.h"
#include "srsran/phy/phch/pusch_nr.h"

typedef struct SRSRAN_API {
  srsran_pusch_nr_args_t pusch;
  srsran_pucch_nr_args_t pucch;
  uint32_t               nof_max_prb;
} srsran_ue_ul_nr_args_t;

typedef struct SRSRAN_API {
  uint32_t max_prb;

  srsran_carrier_nr_t carrier;

  srsran_ofdm_t ifft;

  cf_t*             sf_symbols[SRSRAN_MAX_PORTS];
  srsran_pusch_nr_t pusch;
  srsran_pucch_nr_t pucch;
  srsran_dmrs_sch_t dmrs;

  float freq_offset_hz;
} srsran_ue_ul_nr_t;

SRSRAN_API int srsran_ue_ul_nr_init(srsran_ue_ul_nr_t* q, cf_t* output, const srsran_ue_ul_nr_args_t* args);

SRSRAN_API int srsran_ue_ul_nr_set_carrier(srsran_ue_ul_nr_t* q, const srsran_carrier_nr_t* carrier);

SRSRAN_API void srsran_ue_ul_nr_set_freq_offset(srsran_ue_ul_nr_t* q, float freq_offset_hz);

SRSRAN_API int srsran_ue_ul_nr_encode_pusch(srsran_ue_ul_nr_t*            q,
                                            const srsran_slot_cfg_t*      slot_cfg,
                                            const srsran_sch_cfg_nr_t*    pusch_cfg,
                                            const srsran_pusch_data_nr_t* data);

SRSRAN_API int srsran_ue_ul_nr_encode_pucch(srsran_ue_ul_nr_t*                  q,
                                            const srsran_slot_cfg_t*            slot_cfg,
                                            const srsran_pucch_nr_common_cfg_t* cfg,
                                            const srsran_pucch_nr_resource_t*   resource,
                                            const srsran_uci_data_nr_t*         uci_data);

SRSRAN_API void srsran_ue_ul_nr_free(srsran_ue_ul_nr_t* q);

SRSRAN_API int srsran_ue_ul_nr_pusch_info(const srsran_ue_ul_nr_t*     q,
                                          const srsran_sch_cfg_nr_t*   cfg,
                                          const srsran_uci_value_nr_t* uci_value,
                                          char*                        str,
                                          uint32_t                     str_len);

SRSRAN_API int srsran_ue_ul_nr_pucch_info(const srsran_pucch_nr_resource_t* resource,
                                          const srsran_uci_data_nr_t*       uci_data,
                                          char*                             str,
                                          uint32_t                          str_len);

/**
 * @brief Decides whether the provided slot index within the radio frame is a SR transmission opportunity
 *
 * @remark Implemented according to TS 38.213 9.2.4 UE procedure for reporting SR
 *
 * @param sr_resources Provides the SR configuration from the upper layers
 * @param slot_idx Slot index in the radio frame
 * @param[out] sr_resource_id Optional SR resource index (or identifier)
 *
 * @return the number of SR opportunities if the provided slot index is a SR transmission opportunity, SRSRAN_ERROR code
 * if provided parameters are invalid
 */
SRSRAN_API int
srsran_ue_ul_nr_sr_send_slot(const srsran_pucch_nr_sr_resource_t sr_resources[SRSRAN_PUCCH_MAX_NOF_SR_RESOURCES],
                             uint32_t                            slot_idx,
                             uint32_t                            sr_resource_id[SRSRAN_PUCCH_MAX_NOF_SR_RESOURCES]);

#endif // SRSRAN_UE_UL_DATA_H
