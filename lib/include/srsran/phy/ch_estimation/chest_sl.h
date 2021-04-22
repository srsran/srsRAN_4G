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

#ifndef SRSRAN_CHEST_SL_H
#define SRSRAN_CHEST_SL_H

#include <stdio.h>

#include "srsran/phy/common/phy_common_sl.h"
#include "srsran/phy/resampling/interp.h"

#define SRSRAN_SL_N_RU_SEQ (30)
#define SRSRAN_SL_MAX_DMRS_SYMB (4)
#define SRSRAN_SL_DEFAULT_NOF_DMRS_CYCLIC_SHIFTS (1)
#define SRSRAN_SL_MAX_PSCCH_NOF_DMRS_CYCLIC_SHIFTS (4)

// Base Sequence Number - always 0 for sidelink: 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 5.5.1.4
#define SRSRAN_SL_BASE_SEQUENCE_NUMBER 0
#define SRSRAN_SL_MAX_DMRS_PERIOD_LENGTH 320

typedef struct SRSRAN_API {
  uint32_t prb_start_idx; // PRB start idx to map RE from RIV
  uint32_t nof_prb;       // PSSCH nof_prb, Length of continuous PRB to map RE (in the pool) from RIV
  uint32_t N_x_id;
  uint32_t sf_idx; // PSSCH sf_idx
  uint32_t cyclic_shift;
} srsran_chest_sl_cfg_t;

typedef struct SRSRAN_API {
  srsran_sl_channels_t           channel;
  srsran_cell_sl_t               cell;
  srsran_sl_comm_resource_pool_t sl_comm_resource_pool;
  srsran_chest_sl_cfg_t          chest_sl_cfg;

  uint32_t sf_n_re;

  uint32_t M_sc_rs;
  int8_t   nof_dmrs_symbols;

  // Orthogonal Sequence (W) Transmission Mode 1, 2 and PSBCH
  int8_t w[SRSRAN_SL_MAX_DMRS_SYMB];

  // Cyclic Shift Values
  int8_t n_CS[SRSRAN_SL_MAX_DMRS_SYMB];

  // Reference Signal Cyclic Shift
  float alpha[SRSRAN_SL_MAX_DMRS_SYMB];

  // Group Hopping Flag
  uint32_t* f_gh_pattern;

  cf_t* r_sequence[SRSRAN_SL_MAX_DMRS_SYMB][SRSRAN_SL_MAX_PSCCH_NOF_DMRS_CYCLIC_SHIFTS];

  cf_t* r_sequence_rx[SRSRAN_SL_MAX_DMRS_SYMB];

  cf_t* ce;
  cf_t* ce_average;
  cf_t* noise_tmp;
  float noise_estimated;

  srsran_interp_linsrsran_vec_t lin_vec_sl;

  bool  sync_error_enable;
  bool  rsrp_enable;
  float sync_err;
  float rsrp_corr;

} srsran_chest_sl_t;

SRSRAN_API int srsran_chest_sl_init(srsran_chest_sl_t*             q,
                                    srsran_sl_channels_t           channel,
                                    srsran_cell_sl_t               cell,
                                    srsran_sl_comm_resource_pool_t sl_comm_resource_pool);

SRSRAN_API int srsran_chest_sl_set_cell(srsran_chest_sl_t* q, srsran_cell_sl_t cell);

SRSRAN_API int srsran_chest_sl_set_cfg(srsran_chest_sl_t* q, srsran_chest_sl_cfg_t chest_sl_cfg);

SRSRAN_API float srsran_chest_sl_get_sync_error(srsran_chest_sl_t* q);

SRSRAN_API float srsran_chest_sl_estimate_noise(srsran_chest_sl_t* q);

SRSRAN_API float srsran_chest_sl_get_rsrp(srsran_chest_sl_t* q);

SRSRAN_API int srsran_chest_sl_put_dmrs(srsran_chest_sl_t* q, cf_t* sf_buffer);

SRSRAN_API int srsran_chest_sl_get_dmrs(srsran_chest_sl_t* q, cf_t* sf_buffer, cf_t** dmrs_received);

SRSRAN_API void srsran_chest_sl_ls_estimate(srsran_chest_sl_t* q, cf_t* sf_buffer);

SRSRAN_API void srsran_chest_sl_ls_equalize(srsran_chest_sl_t* q, cf_t* sf_buffer, cf_t* equalized_sf_buffer);

SRSRAN_API void srsran_chest_sl_ls_estimate_equalize(srsran_chest_sl_t* q, cf_t* sf_buffer, cf_t* equalized_sf_buffer);

SRSRAN_API void srsran_chest_sl_free(srsran_chest_sl_t* q);

#endif