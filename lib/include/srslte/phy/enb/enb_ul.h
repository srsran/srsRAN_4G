/*
 * Copyright 2013-2020 Software Radio Systems Limited
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
 *  File:         enb_ul.h
 *
 *  Description:  ENB uplink object.
 *
 *                This module is a frontend to all the uplink data and control
 *                channel processing modules for the ENB receiver side.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_ENB_UL_H
#define SRSLTE_ENB_UL_H

#include <stdbool.h>

#include "srslte/phy/ch_estimation/chest_ul.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/phch/prach.h"
#include "srslte/phy/phch/pucch.h"
#include "srslte/phy/phch/pusch.h"
#include "srslte/phy/phch/pusch_cfg.h"
#include "srslte/phy/phch/ra.h"

#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#include "srslte/config.h"

typedef struct SRSLTE_API {
  srslte_cell_t cell;

  cf_t*                 sf_symbols;
  srslte_chest_ul_res_t chest_res;

  srslte_ofdm_t     fft;
  srslte_chest_ul_t chest;
  srslte_pusch_t    pusch;
  srslte_pucch_t    pucch;

} srslte_enb_ul_t;

/* This function shall be called just after the initial synchronization */
SRSLTE_API int srslte_enb_ul_init(srslte_enb_ul_t* q, cf_t* in_buffer, uint32_t max_prb);

SRSLTE_API void srslte_enb_ul_free(srslte_enb_ul_t* q);

SRSLTE_API int
srslte_enb_ul_set_cell(srslte_enb_ul_t* q, srslte_cell_t cell, srslte_refsignal_dmrs_pusch_cfg_t* pusch_cfg);

SRSLTE_API int srslte_enb_ul_add_rnti(srslte_enb_ul_t* q, uint16_t rnti);

SRSLTE_API void srslte_enb_ul_rem_rnti(srslte_enb_ul_t* q, uint16_t rnti);

SRSLTE_API void srslte_enb_ul_fft(srslte_enb_ul_t* q);

SRSLTE_API int srslte_enb_ul_get_pucch(srslte_enb_ul_t*    q,
                                       srslte_ul_sf_cfg_t* ul_sf,
                                       srslte_pucch_cfg_t* cfg,
                                       srslte_pucch_res_t* res);

SRSLTE_API int srslte_enb_ul_get_pusch(srslte_enb_ul_t*    q,
                                       srslte_ul_sf_cfg_t* ul_sf,
                                       srslte_pusch_cfg_t* cfg,
                                       srslte_pusch_res_t* res);

#endif // SRSLTE_ENB_UL_H
