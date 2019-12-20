/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSLTE_CHEST_SL_H
#define SRSLTE_CHEST_SL_H

#include <stdio.h>

#include "srslte/config.h"
#include "srslte/phy/ch_estimation/chest_common.h"
#include "srslte/phy/ch_estimation/refsignal_ul.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/resampling/interp.h"

#define SRSLTE_SL_MAX_DMRS_SYMB (4)
#define SRSLTE_SL_NOF_PRIME_NUMBERS (196)

typedef struct {

  uint32_t nof_prb;
  uint32_t sf_n_re;

  uint32_t M_sc_rs;
  int8_t   nof_dmrs_symbols;

  // Orthogonal Sequence (W) Transmission Mode 1, 2 and PSBCH
  int8_t w[SRSLTE_SL_MAX_DMRS_SYMB];

  // Cyclic Shift Values
  int8_t n_CS[SRSLTE_SL_MAX_DMRS_SYMB];

  // Reference Signal Cyclic Shift
  float alpha[SRSLTE_SL_MAX_DMRS_SYMB];

  // Group Hopping Flag
  uint32_t  f_gh;
  uint32_t* f_gh_pattern;
  uint32_t  f_ss;

  // Sequence Group Number
  uint32_t u[SRSLTE_SL_MAX_DMRS_SYMB];

  // Base Sequence Number - always 0 for sidelink
  uint32_t v;

  int32_t N_zc;

  int32_t q[SRSLTE_SL_MAX_DMRS_SYMB];

  float* r[SRSLTE_SL_MAX_DMRS_SYMB];

  cf_t* r_uv[SRSLTE_SL_MAX_DMRS_SYMB];

  cf_t* r_sequence[SRSLTE_SL_MAX_DMRS_SYMB];

  cf_t* dmrs_received[SRSLTE_SL_MAX_DMRS_SYMB];
  cf_t* pilot_estimates_1;
  cf_t* pilot_estimates_2;
  cf_t* ce;

  srslte_interp_linsrslte_vec_t lin_vec_sl;

} srslte_chest_sl_t;

SRSLTE_API int srslte_chest_sl_init_psbch_dmrs(srslte_chest_sl_t* q);

SRSLTE_API int srslte_chest_sl_gen_psbch_dmrs(srslte_chest_sl_t* q, srslte_sl_tm_t txMode, uint32_t N_sl_id);

SRSLTE_API int srslte_chest_sl_put_psbch_dmrs(srslte_chest_sl_t* q,
                                              cf_t*              sf_buffer,
                                              srslte_sl_tm_t     tx_mode,
                                              uint32_t           nof_prb,
                                              srslte_cp_t        cp);

SRSLTE_API void srslte_chest_sl_psbch_ls_estimate_equalize(srslte_chest_sl_t* q,
                                                           cf_t*              sf_buffer,
                                                           cf_t*              sf_buffer_rx,
                                                           uint32_t           nof_prb,
                                                           srslte_sl_tm_t     txMode,
                                                           srslte_cp_t        cp);

SRSLTE_API int srslte_chest_sl_get_psbch_dmrs(srslte_chest_sl_t* q,
                                              cf_t*              sf_buffer_rx,
                                              cf_t**             dmrs_received,
                                              srslte_sl_tm_t     tx_mode,
                                              uint32_t           nof_prb,
                                              srslte_cp_t        cp);

SRSLTE_API void srslte_chest_sl_free(srslte_chest_sl_t* q);

#endif