/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_REFSIGNAL_UL_
#define SRSLTE_REFSIGNAL_UL_

/* Object to manage Downlink reference signals for channel estimation.
 *
 */

#include "srslte/config.h"
#include "srslte/phch/pucch.h"
#include "srslte/common/phy_common.h"

#define SRSLTE_NOF_GROUPS_U    30
#define SRSLTE_NOF_SEQUENCES_U 2
#define SRSLTE_NOF_DELTA_SS    30
#define SRSLTE_NOF_CSHIFT      8

typedef _Complex float cf_t;

typedef struct SRSLTE_API {
  uint32_t cyclic_shift; 
  uint32_t cyclic_shift_for_drms;
  uint32_t delta_ss;  
  bool en_drms_2; 
  float beta_pusch;
  bool group_hopping_en; 
  bool srslte_sequence_hopping_en; 
}srslte_refsignal_drms_pusch_cfg_t;

typedef struct SRSLTE_API {
  float beta_pucch;
  uint32_t nof_prb;
}srslte_refsignal_srs_cfg_t;

/** Uplink DeModulation Reference Signal (DMRS) */
typedef struct SRSLTE_API {
  srslte_cell_t cell; 
  uint32_t n_cs_cell[SRSLTE_NSLOTS_X_FRAME][SRSLTE_SRSLTE_SRSLTE_CP_NORM_NSYMB]; 
  float *tmp_arg; 
  uint32_t n_prs_pusch[SRSLTE_NOF_DELTA_SS][SRSLTE_NSLOTS_X_FRAME]; // We precompute n_prs needed for cyclic shift alpha at srslte_refsignal_dl_init()
  uint32_t f_gh[SRSLTE_NSLOTS_X_FRAME];
  uint32_t u_pucch[SRSLTE_NSLOTS_X_FRAME];
  uint32_t v_pusch[SRSLTE_NSLOTS_X_FRAME][SRSLTE_NOF_DELTA_SS];
} srslte_refsignal_ul_t;


SRSLTE_API int srslte_refsignal_ul_init(srslte_refsignal_ul_t *q, 
                                        srslte_cell_t cell);

SRSLTE_API void srslte_refsignal_ul_free(srslte_refsignal_ul_t *q);

SRSLTE_API bool srslte_refsignal_drms_pusch_cfg_isvalid(srslte_refsignal_ul_t *q, 
                                                        srslte_refsignal_drms_pusch_cfg_t *cfg, 
                                                        uint32_t nof_prb); 

SRSLTE_API void srslte_refsignal_drms_pusch_put(srslte_refsignal_ul_t *q, 
                                                srslte_refsignal_drms_pusch_cfg_t *cfg, 
                                                cf_t *r_pusch, 
                                                uint32_t nof_prb, 
                                                uint32_t n_prb[2], 
                                                cf_t *sf_symbols); 

SRSLTE_API int srslte_refsignal_dmrs_pusch_gen(srslte_refsignal_ul_t *q, 
                                               srslte_refsignal_drms_pusch_cfg_t *cfg, 
                                               uint32_t nof_prb, 
                                               uint32_t sf_idx, 
                                               cf_t *r_pusch);

SRSLTE_API int srslte_refsignal_dmrs_pucch_gen(srslte_refsignal_ul_t *q, 
                                               pucch_cfg_t *cfg, 
                                               uint32_t sf_idx, 
                                               uint32_t n_rb, 
                                               cf_t *r_pucch) ;

SRSLTE_API void srslte_refsignal_srs_gen(srslte_refsignal_ul_t *q, 
                                         srslte_refsignal_srs_cfg_t *cfg, 
                                         uint32_t sf_idx, 
                                         cf_t *r_srs);

#endif
