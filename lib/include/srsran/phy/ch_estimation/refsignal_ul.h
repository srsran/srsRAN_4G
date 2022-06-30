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

/**********************************************************************************************
 *  File:         refsignal_ul.h
 *
 *  Description:  Object to manage uplink reference signals for channel estimation.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.5
 *********************************************************************************************/

#ifndef SRSRAN_REFSIGNAL_UL_H
#define SRSRAN_REFSIGNAL_UL_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/pucch_cfg.h"
#include "srsran/phy/phch/pusch_cfg.h"

#define SRSRAN_NOF_GROUPS_U 30
#define SRSRAN_NOF_SEQUENCES_U 2
#define SRSRAN_NOF_DELTA_SS 30
#define SRSRAN_NOF_CSHIFT 8

#define SRSRAN_REFSIGNAL_UL_L(ns_idx, cp) ((ns_idx + 1) * SRSRAN_CP_NSYMB(cp) - 4)

/* PUSCH DMRS common configuration (received in SIB2) */
typedef struct SRSRAN_API {
  uint32_t cyclic_shift;
  uint32_t delta_ss;
  bool     group_hopping_en;
  bool     sequence_hopping_en;
} srsran_refsignal_dmrs_pusch_cfg_t;

typedef struct SRSRAN_API {

  // Common Configuration
  uint32_t subframe_config;
  uint32_t bw_cfg;
  bool     simul_ack;

  // Dedicated configuration
  uint32_t B;
  uint32_t b_hop;
  uint32_t n_srs;
  uint32_t I_srs;
  uint32_t k_tc;
  uint32_t n_rrc;
  bool     dedicated_enabled;
  bool     common_enabled;
  bool     configured;
} srsran_refsignal_srs_cfg_t;

/** Uplink DeModulation Reference Signal (DMRS) */
typedef struct SRSRAN_API {
  srsran_cell_t cell;

  uint32_t n_cs_cell[SRSRAN_NSLOTS_X_FRAME][SRSRAN_CP_NORM_NSYMB];
  uint32_t n_prs_pusch[SRSRAN_NOF_DELTA_SS][SRSRAN_NSLOTS_X_FRAME]; // We precompute n_prs needed for cyclic shift alpha
                                                                    // at srsran_refsignal_dl_init()
  uint32_t f_gh[SRSRAN_NSLOTS_X_FRAME];
  uint32_t u_pucch[SRSRAN_NSLOTS_X_FRAME];
  uint32_t v_pusch[SRSRAN_NSLOTS_X_FRAME][SRSRAN_NOF_DELTA_SS];
} srsran_refsignal_ul_t;

typedef struct {
  uint32_t max_prb;
  cf_t** r[SRSRAN_NOF_CSHIFT][SRSRAN_NOF_SF_X_FRAME];
} srsran_refsignal_ul_dmrs_pregen_t;

typedef struct {
  cf_t* r[SRSRAN_NOF_SF_X_FRAME];
} srsran_refsignal_srs_pregen_t;

SRSRAN_API int srsran_refsignal_ul_set_cell(srsran_refsignal_ul_t* q, srsran_cell_t cell);

SRSRAN_API uint32_t srsran_refsignal_dmrs_N_rs(srsran_pucch_format_t format, srsran_cp_t cp);

SRSRAN_API uint32_t srsran_refsignal_dmrs_pucch_symbol(uint32_t m, srsran_pucch_format_t format, srsran_cp_t cp);

SRSRAN_API int srsran_refsignal_dmrs_pusch_pregen_init(srsran_refsignal_ul_dmrs_pregen_t* pregen, uint32_t max_prb);

SRSRAN_API int srsran_refsignal_dmrs_pusch_pregen(srsran_refsignal_ul_t*             q,
                                                  srsran_refsignal_ul_dmrs_pregen_t* pregen,
                                                  srsran_refsignal_dmrs_pusch_cfg_t* cfg);

SRSRAN_API void srsran_refsignal_dmrs_pusch_pregen_free(srsran_refsignal_ul_t*             q,
                                                        srsran_refsignal_ul_dmrs_pregen_t* pregen);

SRSRAN_API int srsran_refsignal_dmrs_pusch_pregen_put(srsran_refsignal_ul_t*             q,
                                                      srsran_ul_sf_cfg_t*                sf_cfg,
                                                      srsran_refsignal_ul_dmrs_pregen_t* pregen,
                                                      srsran_pusch_cfg_t*                pusch_cfg,
                                                      cf_t*                              sf_symbols);

SRSRAN_API int srsran_refsignal_dmrs_pusch_gen(srsran_refsignal_ul_t*             q,
                                               srsran_refsignal_dmrs_pusch_cfg_t* cfg,
                                               uint32_t                           nof_prb,
                                               uint32_t                           sf_idx,
                                               uint32_t                           cyclic_shift_for_dmrs,
                                               cf_t*                              r_pusch);

SRSRAN_API void srsran_refsignal_dmrs_pusch_put(srsran_refsignal_ul_t* q,
                                                srsran_pusch_cfg_t*    pusch_cfg,
                                                cf_t*                  r_pusch,
                                                cf_t*                  sf_symbols);

SRSRAN_API void srsran_refsignal_dmrs_pusch_get(srsran_refsignal_ul_t* q,
                                                srsran_pusch_cfg_t*    pusch_cfg,
                                                cf_t*                  sf_symbols,
                                                cf_t*                  r_pusch);

SRSRAN_API int srsran_refsignal_dmrs_pucch_gen(srsran_refsignal_ul_t* q,
                                               srsran_ul_sf_cfg_t*    sf,
                                               srsran_pucch_cfg_t*    cfg,
                                               cf_t*                  r_pucch);

SRSRAN_API int
srsran_refsignal_dmrs_pucch_put(srsran_refsignal_ul_t* q, srsran_pucch_cfg_t* cfg, cf_t* r_pucch, cf_t* output);

SRSRAN_API int
srsran_refsignal_dmrs_pucch_get(srsran_refsignal_ul_t* q, srsran_pucch_cfg_t* cfg, cf_t* input, cf_t* r_pucch);

SRSRAN_API int srsran_refsignal_srs_pregen(srsran_refsignal_ul_t*             q,
                                           srsran_refsignal_srs_pregen_t*     pregen,
                                           srsran_refsignal_srs_cfg_t*        cfg,
                                           srsran_refsignal_dmrs_pusch_cfg_t* dmrs);

SRSRAN_API int srsran_refsignal_srs_pregen_put(srsran_refsignal_ul_t*         q,
                                               srsran_refsignal_srs_pregen_t* pregen,
                                               srsran_refsignal_srs_cfg_t*    cfg,
                                               uint32_t                       tti,
                                               cf_t*                          sf_symbols);

SRSRAN_API void srsran_refsignal_srs_pregen_free(srsran_refsignal_ul_t* q, srsran_refsignal_srs_pregen_t* pregen);

SRSRAN_API int srsran_refsignal_srs_gen(srsran_refsignal_ul_t*             q,
                                        srsran_refsignal_srs_cfg_t*        cfg,
                                        srsran_refsignal_dmrs_pusch_cfg_t* pusch_cfg,
                                        uint32_t                           sf_idx,
                                        cf_t*                              r_srs);

SRSRAN_API int srsran_refsignal_srs_put(srsran_refsignal_ul_t*      q,
                                        srsran_refsignal_srs_cfg_t* cfg,
                                        uint32_t                    tti,
                                        cf_t*                       r_srs,
                                        cf_t*                       sf_symbols);

SRSRAN_API int srsran_refsignal_srs_get(srsran_refsignal_ul_t*      q,
                                        srsran_refsignal_srs_cfg_t* cfg,
                                        uint32_t                    tti,
                                        cf_t*                       r_srs,
                                        cf_t*                       sf_symbols);

SRSRAN_API void srsran_refsignal_srs_pusch_shortened(srsran_refsignal_ul_t*      q,
                                                     srsran_ul_sf_cfg_t*         sf,
                                                     srsran_refsignal_srs_cfg_t* srs_cfg,
                                                     srsran_pusch_cfg_t*         pusch_cfg);

SRSRAN_API void srsran_refsignal_srs_pucch_shortened(srsran_refsignal_ul_t*      q,
                                                     srsran_ul_sf_cfg_t*         sf,
                                                     srsran_refsignal_srs_cfg_t* srs_cfg,
                                                     srsran_pucch_cfg_t*         pucch_cfg);

SRSRAN_API int srsran_refsignal_srs_send_cs(uint32_t subframe_config, uint32_t sf_idx);

SRSRAN_API int srsran_refsignal_srs_send_ue(uint32_t I_srs, uint32_t tti);

SRSRAN_API uint32_t srsran_refsignal_srs_rb_start_cs(uint32_t bw_cfg, uint32_t nof_prb);

SRSRAN_API uint32_t srsran_refsignal_srs_rb_L_cs(uint32_t bw_cfg, uint32_t nof_prb);

SRSRAN_API uint32_t srsran_refsignal_srs_M_sc(srsran_refsignal_ul_t* q, srsran_refsignal_srs_cfg_t* cfg);

#endif // SRSRAN_REFSIGNAL_UL_H
