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

SRSLTE_API int srslte_enb_ul_set_cell(srslte_enb_ul_t*                   q,
                                      srslte_cell_t                      cell,
                                      srslte_refsignal_dmrs_pusch_cfg_t* pusch_cfg,
                                      srslte_refsignal_srs_cfg_t*        srs_cfg);

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
