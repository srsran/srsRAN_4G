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

/******************************************************************************
 *  File:         pusch.h
 *
 *  Description:  Physical uplink shared channel.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.3
 *****************************************************************************/

#ifndef SRSRAN_PUSCH_H
#define SRSRAN_PUSCH_H

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/refsignal_ul.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/dft_precoding.h"
#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/modem/evm.h"
#include "srsran/phy/modem/mod.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/pusch_cfg.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/phch/sch.h"
#include "srsran/phy/scrambling/scrambling.h"

/* PUSCH object */
typedef struct SRSRAN_API {
  srsran_cell_t cell;

  bool     is_ue;
  uint16_t ue_rnti;
  uint32_t max_re;

  bool llr_is_8bit;

  srsran_dft_precoding_t dft_precoding;

  /* buffers */
  // void buffers are shared for tx and rx
  cf_t* ce;
  cf_t* z;
  cf_t* d;

  void* q;
  void* g;

  /* tx & rx objects */
  srsran_modem_table_t mod[SRSRAN_MOD_NITEMS];
  srsran_sch_t         ul_sch;

  // EVM buffer
  srsran_evm_buffer_t* evm_buffer;

} srsran_pusch_t;

typedef struct SRSRAN_API {
  uint8_t*           ptr;
  srsran_uci_value_t uci;
} srsran_pusch_data_t;

typedef struct SRSRAN_API {
  uint8_t*           data;
  srsran_uci_value_t uci;
  bool               crc;
  float              avg_iterations_block;
  float              evm;
  float              epre_dbfs;
} srsran_pusch_res_t;

SRSRAN_API int srsran_pusch_init_ue(srsran_pusch_t* q, uint32_t max_prb);

SRSRAN_API int srsran_pusch_init_enb(srsran_pusch_t* q, uint32_t max_prb);

SRSRAN_API void srsran_pusch_free(srsran_pusch_t* q);

/* These functions modify the state of the object and may take some time */
SRSRAN_API int srsran_pusch_set_cell(srsran_pusch_t* q, srsran_cell_t cell);

/**
 * Asserts PUSCH grant attributes are in range
 * @param grant Pointer to PUSCH grant
 * @return it returns SRSRAN_SUCCESS if the grant is correct, otherwise it returns a SRSRAN_ERROR code
 */
SRSRAN_API int srsran_pusch_assert_grant(const srsran_pusch_grant_t* grant);

/* These functions do not modify the state and run in real-time */
SRSRAN_API int srsran_pusch_encode(srsran_pusch_t*      q,
                                   srsran_ul_sf_cfg_t*  sf,
                                   srsran_pusch_cfg_t*  cfg,
                                   srsran_pusch_data_t* data,
                                   cf_t*                sf_symbols);

SRSRAN_API int srsran_pusch_decode(srsran_pusch_t*        q,
                                   srsran_ul_sf_cfg_t*    sf,
                                   srsran_pusch_cfg_t*    cfg,
                                   srsran_chest_ul_res_t* channel,
                                   cf_t*                  sf_symbols,
                                   srsran_pusch_res_t*    data);

SRSRAN_API uint32_t srsran_pusch_grant_tx_info(srsran_pusch_grant_t* grant,
                                               srsran_uci_cfg_t*     uci_cfg,
                                               srsran_uci_value_t*   uci_data,
                                               char*                 str,
                                               uint32_t              str_len);

SRSRAN_API uint32_t srsran_pusch_tx_info(srsran_pusch_cfg_t* cfg,
                                         srsran_uci_value_t* uci_data,
                                         char*               str,
                                         uint32_t            str_len);

SRSRAN_API uint32_t srsran_pusch_rx_info(srsran_pusch_cfg_t*    cfg,
                                         srsran_pusch_res_t*    res,
                                         srsran_chest_ul_res_t* chest_res,
                                         char*                  str,
                                         uint32_t               str_len);

#endif // SRSRAN_PUSCH_H
