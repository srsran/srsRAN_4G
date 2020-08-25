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
 *  File:         pusch.h
 *
 *  Description:  Physical uplink shared channel.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.3
 *****************************************************************************/

#ifndef SRSLTE_PUSCH_H
#define SRSLTE_PUSCH_H

#include "srslte/config.h"
#include "srslte/phy/ch_estimation/refsignal_ul.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/dft_precoding.h"
#include "srslte/phy/mimo/layermap.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/modem/evm.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/pusch_cfg.h"
#include "srslte/phy/phch/regs.h"
#include "srslte/phy/phch/sch.h"
#include "srslte/phy/scrambling/scrambling.h"

typedef struct {
  srslte_sequence_t seq[SRSLTE_NOF_SF_X_FRAME];
  uint32_t          cell_id;
  bool              sequence_generated;
} srslte_pusch_user_t;

/* PUSCH object */
typedef struct SRSLTE_API {
  srslte_cell_t cell;

  bool     is_ue;
  uint16_t ue_rnti;
  uint32_t max_re;

  bool llr_is_8bit;

  srslte_dft_precoding_t dft_precoding;

  /* buffers */
  // void buffers are shared for tx and rx
  cf_t* ce;
  cf_t* z;
  cf_t* d;

  void* q;
  void* g;

  /* tx & rx objects */
  srslte_modem_table_t mod[SRSLTE_MOD_NITEMS];
  srslte_sch_t         ul_sch;

  // This is to generate the scrambling seq for multiple CRNTIs
  srslte_pusch_user_t** users;
  srslte_sequence_t     tmp_seq;

  // EVM buffer
  srslte_evm_buffer_t* evm_buffer;

} srslte_pusch_t;

typedef struct SRSLTE_API {
  uint8_t*           ptr;
  srslte_uci_value_t uci;
} srslte_pusch_data_t;

typedef struct SRSLTE_API {
  uint8_t*           data;
  srslte_uci_value_t uci;
  bool               crc;
  float              avg_iterations_block;
  float              evm;
  float              epre_dbfs;
} srslte_pusch_res_t;

SRSLTE_API int srslte_pusch_init_ue(srslte_pusch_t* q, uint32_t max_prb);

SRSLTE_API int srslte_pusch_init_enb(srslte_pusch_t* q, uint32_t max_prb);

SRSLTE_API void srslte_pusch_free(srslte_pusch_t* q);

/* These functions modify the state of the object and may take some time */
SRSLTE_API int srslte_pusch_set_cell(srslte_pusch_t* q, srslte_cell_t cell);

SRSLTE_API int srslte_pusch_set_rnti(srslte_pusch_t* q, uint16_t rnti);

SRSLTE_API void srslte_pusch_free_rnti(srslte_pusch_t* q, uint16_t rnti);

/**
 * Asserts PUSCH grant attributes are in range
 * @param grant Pointer to PUSCH grant
 * @return it returns SRSLTE_SUCCESS if the grant is correct, otherwise it returns a SRSLTE_ERROR code
 */
SRSLTE_API int srslte_pusch_assert_grant(const srslte_pusch_grant_t* grant);

/* These functions do not modify the state and run in real-time */
SRSLTE_API int srslte_pusch_encode(srslte_pusch_t*      q,
                                   srslte_ul_sf_cfg_t*  sf,
                                   srslte_pusch_cfg_t*  cfg,
                                   srslte_pusch_data_t* data,
                                   cf_t*                sf_symbols);

SRSLTE_API int srslte_pusch_decode(srslte_pusch_t*        q,
                                   srslte_ul_sf_cfg_t*    sf,
                                   srslte_pusch_cfg_t*    cfg,
                                   srslte_chest_ul_res_t* channel,
                                   cf_t*                  sf_symbols,
                                   srslte_pusch_res_t*    data);

SRSLTE_API uint32_t srslte_pusch_grant_tx_info(srslte_pusch_grant_t* grant,
                                               srslte_uci_cfg_t*     uci_cfg,
                                               srslte_uci_value_t*   uci_data,
                                               char*                 str,
                                               uint32_t              str_len);

SRSLTE_API uint32_t srslte_pusch_tx_info(srslte_pusch_cfg_t* cfg,
                                         srslte_uci_value_t* uci_data,
                                         char*               str,
                                         uint32_t            str_len);

SRSLTE_API uint32_t srslte_pusch_rx_info(srslte_pusch_cfg_t*    cfg,
                                         srslte_pusch_res_t*    res,
                                         srslte_chest_ul_res_t* chest_res,
                                         char*                  str,
                                         uint32_t               str_len);

#endif // SRSLTE_PUSCH_H
