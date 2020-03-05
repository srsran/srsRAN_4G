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

#ifndef SRSLTE_CHEST_SL_H
#define SRSLTE_CHEST_SL_H

#include <stdio.h>

#include "srslte/phy/common/phy_common_sl.h"
#include "srslte/phy/resampling/interp.h"

#define SRSLTE_SL_N_RU_SEQ (30)
#define SRSLTE_SL_MAX_DMRS_SYMB (4)
#define SRSLTE_SL_DEFAULT_NOF_DMRS_CYCLIC_SHIFTS (1)
#define SRSLTE_SL_MAX_PSCCH_NOF_DMRS_CYCLIC_SHIFTS (4)

// Base Sequence Number - always 0 for sidelink: 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 5.5.1.4
#define SRSLTE_SL_BASE_SEQUENCE_NUMBER 0
#define SRSLTE_SL_MAX_DMRS_PERIOD_LENGTH 320

typedef struct SRSLTE_API {
  uint32_t prb_start_idx; // PRB start idx to map RE from RIV
  uint32_t nof_prb;       // PSSCH nof_prb, Length of continuous PRB to map RE (in the pool) from RIV
  uint32_t N_x_id;
  uint32_t sf_idx; // PSSCH sf_idx
  uint32_t cyclic_shift;
} srslte_chest_sl_cfg_t;

typedef struct SRSLTE_API {

  srslte_sl_channels_t           channel;
  srslte_cell_sl_t               cell;
  srslte_sl_comm_resource_pool_t sl_comm_resource_pool;
  srslte_chest_sl_cfg_t          chest_sl_cfg;

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
  uint32_t* f_gh_pattern;

  int32_t q[SRSLTE_SL_MAX_DMRS_SYMB];

  float* r[SRSLTE_SL_MAX_DMRS_SYMB];

  cf_t* r_uv[SRSLTE_SL_MAX_DMRS_SYMB];

  cf_t* r_sequence[SRSLTE_SL_MAX_DMRS_SYMB][SRSLTE_SL_MAX_PSCCH_NOF_DMRS_CYCLIC_SHIFTS];

  cf_t* r_sequence_rx[SRSLTE_SL_MAX_DMRS_SYMB];

  cf_t* ce;
  cf_t* ce_average;
  cf_t* noise_tmp;
  float noise_estimated;

  srslte_interp_linsrslte_vec_t lin_vec_sl;

  bool  sync_error_enable;
  bool  rsrp_enable;
  float sync_err;
  float rsrp_corr;

} srslte_chest_sl_t;

SRSLTE_API int srslte_chest_sl_init(srslte_chest_sl_t*             q,
                                    srslte_sl_channels_t           channel,
                                    srslte_cell_sl_t               cell,
                                    srslte_sl_comm_resource_pool_t sl_comm_resource_pool);

SRSLTE_API int srslte_chest_sl_set_cell(srslte_chest_sl_t* q, srslte_cell_sl_t cell);

SRSLTE_API int srslte_chest_sl_set_cfg(srslte_chest_sl_t* q, srslte_chest_sl_cfg_t chest_sl_cfg);

SRSLTE_API float srslte_chest_sl_get_sync_error(srslte_chest_sl_t* q);

SRSLTE_API float srslte_chest_sl_estimate_noise(srslte_chest_sl_t* q);

SRSLTE_API float srslte_chest_sl_get_rsrp(srslte_chest_sl_t* q);

SRSLTE_API int srslte_chest_sl_put_dmrs(srslte_chest_sl_t* q, cf_t* sf_buffer);

SRSLTE_API int srslte_chest_sl_get_dmrs(srslte_chest_sl_t* q, cf_t* sf_buffer, cf_t** dmrs_received);

SRSLTE_API void srslte_chest_sl_ls_estimate(srslte_chest_sl_t* q, cf_t* sf_buffer);

SRSLTE_API void srslte_chest_sl_ls_equalize(srslte_chest_sl_t* q, cf_t* sf_buffer, cf_t* equalized_sf_buffer);

SRSLTE_API void srslte_chest_sl_ls_estimate_equalize(srslte_chest_sl_t* q, cf_t* sf_buffer, cf_t* equalized_sf_buffer);

SRSLTE_API void srslte_chest_sl_free(srslte_chest_sl_t* q);

#endif