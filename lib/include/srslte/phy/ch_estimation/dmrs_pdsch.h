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

#ifndef SRSLTE_DMRS_PDSCH_H
#define SRSLTE_DMRS_PDSCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "srslte/phy/common/phy_common.h"
#include "srslte/srslte.h"
#include <stdint.h>

#define SRSLTE_DMRS_PDSCH_MAX_SYMBOLS 4
#define SRSLTE_DMRS_PDSCH_TYPEA_SINGLE_DURATION_MIN 3
#define SRSLTE_DMRS_PDSCH_TYPEA_DOUBLE_DURATION_MIN 4

typedef enum {
  srslte_dmrs_pdsch_type_1 = 0, // 1 pilot every 2 sub-carriers (default)
  srslte_dmrs_pdsch_type_2      // 2 consecutive pilots every 6 sub-carriers
} srslte_dmrs_pdsch_type_t;

typedef enum {
  srslte_dmrs_pdsch_len_1 = 0, // single, 1 symbol long (default)
  srslte_dmrs_pdsch_len_2      // double, 2 symbol long
} srslte_dmrs_pdsch_len_t;

/**
 * Determines whether the first pilot goes into symbol index 2 or 3
 */
typedef enum {
  srslte_dmrs_pdsch_typeA_pos_2 = 0, // Start in slot symbol index 2 (default)
  srslte_dmrs_pdsch_typeA_pos_3      // Start in slot symbol index 3
} srslte_dmrs_pdsch_typeA_pos_t;

/**
 * Determines additional symbols if possible to be added
 */
typedef enum {
  srslte_dmrs_pdsch_add_pos_2 = 0,
  srslte_dmrs_pdsch_add_pos_0,
  srslte_dmrs_pdsch_add_pos_1,
  srslte_dmrs_pdsch_add_pos_3
} srslte_dmrs_pdsch_add_pos_t;

typedef enum {
  srslte_dmrs_pdsch_mapping_type_A = 0,
  srslte_dmrs_pdsch_mapping_type_B
} srslte_dmrs_pdsch_mapping_type_t;

typedef struct {
  srslte_dmrs_pdsch_type_t         type;
  srslte_dmrs_pdsch_typeA_pos_t    typeA_pos;
  srslte_dmrs_pdsch_add_pos_t      additional_pos;
  srslte_dmrs_pdsch_len_t          length;
  srslte_dmrs_pdsch_mapping_type_t mapping_type;
  uint32_t                         duration;

  bool lte_CRS_to_match_around;
  bool additional_DMRS_DL_Alt;

  uint32_t n_id;
  uint32_t n_scid;
  uint32_t nof_prb;
  float    beta;
  uint32_t reference_point;
} srslte_dmrs_pdsch_cfg_t;

typedef struct {
  srslte_dmrs_pdsch_cfg_t cfg;

  uint32_t symbols_idx[SRSLTE_DMRS_PDSCH_MAX_SYMBOLS];
  uint32_t nof_symbols;

  uint32_t sc_idx[SRSLTE_NRE];
  uint32_t nof_sc;

} srslte_dmrs_pdsch_t;

SRSLTE_API int srslte_dmrs_pdsch_cfg_to_str(const srslte_dmrs_pdsch_cfg_t* cfg, char* msg, uint32_t max_len);

SRSLTE_API int srslte_dmrs_pdsch_init(srslte_dmrs_pdsch_t* q, const srslte_dmrs_pdsch_cfg_t* cfg);

SRSLTE_API int srslte_dmrs_pdsch_put_sf(srslte_dmrs_pdsch_t* q, const srslte_dl_sf_cfg_t* sf, cf_t* sf_symbols);

SRSLTE_API int srslte_dmrs_pdsch_get_sf(srslte_dmrs_pdsch_t*      q,
                                        const srslte_dl_sf_cfg_t* sf,
                                        const cf_t*               sf_symbols,
                                        cf_t*                     lest_square_estimates);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_DMRS_PDSCH_H
