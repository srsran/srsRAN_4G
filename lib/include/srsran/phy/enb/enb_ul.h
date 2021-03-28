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

#ifndef SRSRAN_ENB_UL_H
#define SRSRAN_ENB_UL_H

#include <stdbool.h>

#include "srsran/phy/ch_estimation/chest_ul.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/phch/prach.h"
#include "srsran/phy/phch/pucch.h"
#include "srsran/phy/phch/pusch.h"
#include "srsran/phy/phch/pusch_cfg.h"
#include "srsran/phy/phch/ra.h"

#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#include "srsran/config.h"

typedef struct SRSRAN_API {
  srsran_cell_t cell;

  cf_t*                 sf_symbols;
  srsran_chest_ul_res_t chest_res;

  srsran_ofdm_t     fft;
  srsran_chest_ul_t chest;
  srsran_pusch_t    pusch;
  srsran_pucch_t    pucch;

} srsran_enb_ul_t;

/* This function shall be called just after the initial synchronization */
SRSRAN_API int srsran_enb_ul_init(srsran_enb_ul_t* q, cf_t* in_buffer, uint32_t max_prb);

SRSRAN_API void srsran_enb_ul_free(srsran_enb_ul_t* q);

SRSRAN_API int srsran_enb_ul_set_cell(srsran_enb_ul_t*                   q,
                                      srsran_cell_t                      cell,
                                      srsran_refsignal_dmrs_pusch_cfg_t* pusch_cfg,
                                      srsran_refsignal_srs_cfg_t*        srs_cfg);

SRSRAN_API void srsran_enb_ul_fft(srsran_enb_ul_t* q);

SRSRAN_API int srsran_enb_ul_get_pucch(srsran_enb_ul_t*    q,
                                       srsran_ul_sf_cfg_t* ul_sf,
                                       srsran_pucch_cfg_t* cfg,
                                       srsran_pucch_res_t* res);

SRSRAN_API int srsran_enb_ul_get_pusch(srsran_enb_ul_t*    q,
                                       srsran_ul_sf_cfg_t* ul_sf,
                                       srsran_pusch_cfg_t* cfg,
                                       srsran_pusch_res_t* res);

#endif // SRSRAN_ENB_UL_H
