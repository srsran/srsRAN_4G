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

/******************************************************************************
 *  @file ue_dl_nr.h
 *
 *  Description:  NR UE uplink physical layer procedures for data
 *
 *                This module is a frontend to all the uplink data channel processing modules.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_UE_UL_DATA_H
#define SRSLTE_UE_UL_DATA_H

#include "srslte/phy/ch_estimation/dmrs_sch.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/phch/phch_cfg_nr.h"
#include "srslte/phy/phch/pucch_cfg_nr.h"
#include "srslte/phy/phch/pucch_nr.h"
#include "srslte/phy/phch/pusch_nr.h"

typedef struct SRSLTE_API {
  srslte_pusch_nr_args_t pusch;
  srslte_pucch_nr_args_t pucch;
  uint32_t               nof_max_prb;
} srslte_ue_ul_nr_args_t;

typedef struct SRSLTE_API {
  uint32_t max_prb;

  srslte_carrier_nr_t carrier;

  srslte_ofdm_t ifft;

  cf_t*             sf_symbols[SRSLTE_MAX_PORTS];
  srslte_pusch_nr_t pusch;
  srslte_pucch_nr_t pucch;
  srslte_dmrs_sch_t dmrs;
} srslte_ue_ul_nr_t;

SRSLTE_API int srslte_ue_ul_nr_init(srslte_ue_ul_nr_t* q, cf_t* output, const srslte_ue_ul_nr_args_t* args);

SRSLTE_API int srslte_ue_ul_nr_set_carrier(srslte_ue_ul_nr_t* q, const srslte_carrier_nr_t* carrier);

SRSLTE_API int srslte_ue_ul_nr_encode_pusch(srslte_ue_ul_nr_t*            q,
                                            const srslte_slot_cfg_t*      slot_cfg,
                                            const srslte_sch_cfg_nr_t*    pusch_cfg,
                                            const srslte_pusch_data_nr_t* data);

SRSLTE_API int srslte_ue_ul_nr_encode_pucch(srslte_ue_ul_nr_t*                  q,
                                            const srslte_slot_cfg_t*            slot_cfg,
                                            const srslte_pucch_nr_common_cfg_t* cfg,
                                            const srslte_pucch_nr_resource_t*   resource,
                                            const srslte_uci_data_nr_t*         uci_data);

SRSLTE_API void srslte_ue_ul_nr_free(srslte_ue_ul_nr_t* q);

SRSLTE_API int
srslte_ue_ul_nr_pusch_info(const srslte_ue_ul_nr_t* q, const srslte_sch_cfg_nr_t* cfg, char* str, uint32_t str_len);

SRSLTE_API int srslte_ue_ul_nr_pucch_info(const srslte_pucch_nr_resource_t* resource,
                                          const srslte_uci_data_nr_t*       uci_data,
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
 * @return the number of SR opportunities if the provided slot index is a SR transmission opportunity, SRSLTE_ERROR code
 * if provided parameters are invalid
 */
SRSLTE_API int
srslte_ue_ul_nr_sr_send_slot(const srslte_pucch_nr_sr_resource_t sr_resources[SRSLTE_PUCCH_MAX_NOF_SR_RESOURCES],
                             uint32_t                            slot_idx,
                             uint32_t                            sr_resource_id[SRSLTE_PUCCH_MAX_NOF_SR_RESOURCES]);

#endif // SRSLTE_UE_UL_DATA_H
