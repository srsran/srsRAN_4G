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

/******************************************************************************
 *  File:         pdsch_nr.h
 *
 *  Description:  Physical downlink shared channel for NR
 *
 *  Reference:    3GPP TS 38.211 V15.8.0 Sec. 7.3.1
 *****************************************************************************/

#ifndef srslte_pdsch_nr_H
#define srslte_pdsch_nr_H

#include "srslte/config.h"
#include "srslte/phy/phch/pdsch_cfg_nr.h"
#include "srslte/phy/ch_estimation/dmrs_pdsch.h"
#include "srslte/phy/phch/regs.h"
#include "srslte/phy/phch/sch.h"
#include "srslte/phy/scrambling/scrambling.h"

/**
 * @brief PDSCH NR object
 */
typedef struct SRSLTE_API {
  srslte_carrier_nr_t carrier;
} srslte_pdsch_nr_t;

/**
 *
 */
typedef struct {
  uint8_t* payload;
  bool     crc;
  float    avg_iterations_block;
  float    evm;
} srslte_pdsch_res_nr_t;

SRSLTE_API int srslte_pdsch_nr_init_ue(srslte_pdsch_nr_t* q);

SRSLTE_API int srslte_pdsch_nr_init_enb(srslte_pdsch_nr_t* q);

SRSLTE_API void srslte_pdsch_nr_free(srslte_pdsch_nr_t* q);

SRSLTE_API int srslte_pdsch_nr_set_carrier(srslte_pdsch_nr_t* q, srslte_cell_t cell);

/* These functions do not modify the state and run in real-time */
SRSLTE_API int srslte_pdsch_nr_encode(srslte_pdsch_nr_t*     q,
                                      uint32_t slot_idx,
                                      srslte_pdsch_cfg_nr_t* cfg,
                                      uint8_t*               data[SRSLTE_MAX_CODEWORDS],
                                      cf_t*                  sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API int srslte_pdsch_nr_decode(srslte_pdsch_nr_t*     q,
                                      uint32_t slot_idx,
                                      srslte_pdsch_cfg_nr_t* cfg,
                                      srslte_chest_dl_res_t* channel,
                                      cf_t*                  sf_symbols[SRSLTE_MAX_PORTS],
                                      srslte_pdsch_res_nr_t  data[SRSLTE_MAX_CODEWORDS]);

SRSLTE_API int srslte_pdsch_nr_select_pmi(srslte_pdsch_nr_t*     q,
                                          srslte_chest_dl_res_t* channel,
                                          uint32_t               nof_layers,
                                          uint32_t*              best_pmi,
                                          float                  sinr[SRSLTE_MAX_CODEBOOKS]);

SRSLTE_API int srslte_pdsch_nr_compute_cn(srslte_pdsch_nr_t* q, srslte_chest_dl_res_t* channel, float* cn);

SRSLTE_API uint32_t srslte_pdsch_nr_grant_rx_info(srslte_pdsch_grant_nr_t* grant,
                                                  srslte_pdsch_res_nr_t    res[SRSLTE_MAX_CODEWORDS],
                                                  char*                    str,
                                                  uint32_t                 str_len);

SRSLTE_API uint32_t srslte_pdsch_nr_rx_info(srslte_pdsch_cfg_nr_t* cfg,
                                            srslte_pdsch_res_nr_t  res[SRSLTE_MAX_CODEWORDS],
                                            char*                  str,
                                            uint32_t               str_len);

SRSLTE_API uint32_t srslte_pdsch_nr_tx_info(srslte_pdsch_cfg_nr_t* cfg, char* str, uint32_t str_len);

#endif // srslte_pdsch_nr_H
