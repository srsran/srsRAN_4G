/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
 *  File:         ra.h
 *
 *  Description:  Implements Resource allocation Procedures common in for DL and UL
 *
 *  Reference:    3GPP TS 36.213 version 10.0.1 Release 10
 *****************************************************************************/

#ifndef SRSRAN_RA_H
#define SRSRAN_RA_H

#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"

/**************************************************
 * Common structures used for Resource Allocation
 **************************************************/

typedef struct SRSRAN_API {
  srsran_mod_t mod;
  int          tbs;
  int          rv;
  uint32_t     nof_bits;
  uint32_t     cw_idx;
  bool         enabled;

  // this is for debugging and metrics purposes
  uint32_t mcs_idx;
} srsran_ra_tb_t;

typedef enum SRSRAN_API {
  SRSRAN_RA_ALLOC_TYPE0 = 0,
  SRSRAN_RA_ALLOC_TYPE1 = 1,
  SRSRAN_RA_ALLOC_TYPE2 = 2
} srsran_ra_type_t;

typedef struct SRSRAN_API {
  uint32_t rbg_bitmask;
} srsran_ra_type0_t;

typedef struct SRSRAN_API {
  uint32_t vrb_bitmask;
  uint32_t rbg_subset;
  bool     shift;
} srsran_ra_type1_t;

typedef struct SRSRAN_API {
  uint32_t riv; // if L_crb==0, DCI message packer will take this value directly
  enum { SRSRAN_RA_TYPE2_NPRB1A_2 = 0, SRSRAN_RA_TYPE2_NPRB1A_3 = 1 } n_prb1a;
  enum { SRSRAN_RA_TYPE2_NG1 = 0, SRSRAN_RA_TYPE2_NG2 = 1 } n_gap;
  enum { SRSRAN_RA_TYPE2_LOC = 0, SRSRAN_RA_TYPE2_DIST = 1 } mode;
} srsran_ra_type2_t;

#define SRSRAN_RA_NOF_TBS_IDX 34

SRSRAN_API uint32_t srsran_ra_type0_P(uint32_t nof_prb);

SRSRAN_API uint32_t srsran_ra_type2_n_vrb_dl(uint32_t nof_prb, bool ngap_is_1);

SRSRAN_API uint32_t srsran_ra_type2_n_rb_step(uint32_t nof_prb);

SRSRAN_API uint32_t srsran_ra_type2_ngap(uint32_t nof_prb, bool ngap_is_1);

SRSRAN_API uint32_t srsran_ra_type1_N_rb(uint32_t nof_prb);

SRSRAN_API uint32_t srsran_ra_type2_to_riv(uint32_t L_crb, uint32_t RB_start, uint32_t nof_prb);

SRSRAN_API void
srsran_ra_type2_from_riv(uint32_t riv, uint32_t* L_crb, uint32_t* RB_start, uint32_t nof_prb, uint32_t nof_vrb);

SRSRAN_API int srsran_ra_tbs_idx_from_mcs(uint32_t mcs, bool use_tbs_index_alt, bool is_ul);

SRSRAN_API srsran_mod_t srsran_ra_dl_mod_from_mcs(uint32_t mcs, bool use_tbs_index_alt);

SRSRAN_API srsran_mod_t srsran_ra_ul_mod_from_mcs(uint32_t mcs);

SRSRAN_API int srsran_ra_mcs_from_tbs_idx(uint32_t tbs_idx, bool use_tbs_index_alt, bool is_ul);

SRSRAN_API int srsran_ra_tbs_from_idx(uint32_t tbs_idx, uint32_t n_prb);

SRSRAN_API int srsran_ra_tbs_to_table_idx(uint32_t tbs, uint32_t n_prb, uint32_t max_tbs_idx);

#endif // SRSRAN_RA_H
