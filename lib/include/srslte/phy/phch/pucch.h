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

/******************************************************************************
 *  File:         pucch.h
 *
 *  Description:  Physical uplink control channel.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.4
 *****************************************************************************/

#ifndef SRSLTE_PUCCH_H
#define SRSLTE_PUCCH_H

#include "srslte/config.h"
#include "srslte/phy/ch_estimation/chest_ul.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/common/sequence.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/phch/cqi.h"
#include "srslte/phy/phch/pucch_cfg.h"
#include "srslte/phy/phch/uci.h"

#define SRSLTE_PUCCH_N_SEQ 12
#define SRSLTE_PUCCH2_NOF_BITS SRSLTE_UCI_CQI_CODED_PUCCH_B
#define SRSLTE_PUCCH3_NOF_BITS (4 * SRSLTE_NRE)
#define SRSLTE_PUCCH_MAX_BITS SRSLTE_CQI_MAX_BITS
#define SRSLTE_PUCCH_MAX_SYMBOLS 128

typedef struct {
  srslte_sequence_t seq_f2[SRSLTE_NOF_SF_X_FRAME];
  uint32_t          cell_id;
  bool              sequence_generated;
} srslte_pucch_user_t;

/* PUCCH object */
typedef struct SRSLTE_API {
  srslte_cell_t        cell;
  srslte_modem_table_t mod;

  srslte_uci_cqi_pucch_t cqi;

  srslte_pucch_user_t** users;
  srslte_sequence_t     tmp_seq;
  uint16_t              ue_rnti;
  bool                  is_ue;

  uint8_t  bits_scram[SRSLTE_PUCCH_MAX_BITS];
  cf_t     d[SRSLTE_PUCCH_MAX_BITS / 2];
  uint32_t n_cs_cell[SRSLTE_NSLOTS_X_FRAME][SRSLTE_CP_NORM_NSYMB];
  uint32_t f_gh[SRSLTE_NSLOTS_X_FRAME];
  float    tmp_arg[SRSLTE_PUCCH_N_SEQ];

  cf_t *z;
  cf_t *z_tmp;
  cf_t *ce;

} srslte_pucch_t;

typedef struct SRSLTE_API {
  srslte_uci_value_t uci_data;
  float              correlation;
  bool               detected;
} srslte_pucch_res_t;

SRSLTE_API int srslte_pucch_init_ue(srslte_pucch_t *q);

SRSLTE_API int srslte_pucch_init_enb(srslte_pucch_t *q);

SRSLTE_API void srslte_pucch_free(srslte_pucch_t *q);

/* These functions modify the state of the object and may take some time */
SRSLTE_API int srslte_pucch_set_cell(srslte_pucch_t* q, srslte_cell_t cell);

SRSLTE_API int srslte_pucch_set_rnti(srslte_pucch_t* q, uint16_t rnti);

SRSLTE_API void srslte_pucch_free_rnti(srslte_pucch_t* q, uint16_t rnti);

/* These functions do not modify the state and run in real-time */
SRSLTE_API void srslte_pucch_uci_gen_cfg(srslte_pucch_t* q, srslte_pucch_cfg_t* cfg, srslte_uci_data_t* uci_data);

SRSLTE_API int srslte_pucch_encode(srslte_pucch_t* q,
                                   srslte_ul_sf_cfg_t* sf,
                                   srslte_pucch_cfg_t* cfg,
                                   srslte_uci_value_t* uci_data,
                                   cf_t* sf_symbols);

SRSLTE_API int srslte_pucch_decode(srslte_pucch_t*        q,
                                   srslte_ul_sf_cfg_t*    sf,
                                   srslte_pucch_cfg_t*    cfg,
                                   srslte_chest_ul_res_t* channel,
                                   cf_t*                  sf_symbols,
                                   srslte_pucch_res_t*    data);

/* Other utilities. These functions do not modify the state and run in real-time */
SRSLTE_API float srslte_pucch_alpha_format1(uint32_t            n_cs_cell[SRSLTE_NSLOTS_X_FRAME][SRSLTE_CP_NORM_NSYMB],
                                            srslte_pucch_cfg_t* cfg,
                                            srslte_cp_t         cp,
                                            bool                is_dmrs,
                                            uint32_t            ns,
                                            uint32_t            l,
                                            uint32_t*           n_oc,
                                            uint32_t*           n_prime_ns);

SRSLTE_API float srslte_pucch_alpha_format2(uint32_t            n_cs_cell[SRSLTE_NSLOTS_X_FRAME][SRSLTE_CP_NORM_NSYMB],
                                            srslte_pucch_cfg_t* cfg,
                                            uint32_t            ns,
                                            uint32_t            l);

SRSLTE_API int srslte_pucch_format2ab_mod_bits(srslte_pucch_format_t format, uint8_t bits[2], cf_t* d_10);

SRSLTE_API uint32_t srslte_pucch_m(srslte_pucch_cfg_t* cfg, srslte_cp_t cp);

SRSLTE_API uint32_t srslte_pucch_n_prb(srslte_cell_t* cell, srslte_pucch_cfg_t* cfg, uint32_t ns);

SRSLTE_API int srslte_pucch_n_cs_cell(srslte_cell_t cell,
                                      uint32_t      n_cs_cell[SRSLTE_NSLOTS_X_FRAME][SRSLTE_CP_NORM_NSYMB]);

SRSLTE_API char* srslte_pucch_format_text(srslte_pucch_format_t format);

SRSLTE_API char* srslte_pucch_format_text_short(srslte_pucch_format_t format);

SRSLTE_API void srslte_pucch_tx_info(srslte_pucch_cfg_t* cfg,
                                     srslte_uci_value_t* uci_data,
                                     char* str,
                                     uint32_t str_len);

SRSLTE_API void srslte_pucch_rx_info(srslte_pucch_cfg_t* cfg,
                                     srslte_uci_value_t* uci_data,
                                     char* str,
                                     uint32_t str_len);

SRSLTE_API bool srslte_pucch_cfg_isvalid(srslte_pucch_cfg_t* cfg, uint32_t nof_prb);

#endif // SRSLTE_PUCCH_H
