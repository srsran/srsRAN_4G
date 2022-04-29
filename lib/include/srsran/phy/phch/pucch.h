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
 *  File:         pucch.h
 *
 *  Description:  Physical uplink control channel.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.4
 *****************************************************************************/

#ifndef SRSRAN_PUCCH_H
#define SRSRAN_PUCCH_H

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/chest_ul.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/common/sequence.h"
#include "srsran/phy/modem/mod.h"
#include "srsran/phy/phch/cqi.h"
#include "srsran/phy/phch/pucch_cfg.h"
#include "srsran/phy/phch/uci.h"

#define SRSRAN_PUCCH_N_SEQ SRSRAN_NRE
#define SRSRAN_PUCCH2_NOF_BITS SRSRAN_UCI_CQI_CODED_PUCCH_B
#define SRSRAN_PUCCH2_N_SF (5)
#define SRSRAN_PUCCH_1A_2A_NOF_ACK (1)
#define SRSRAN_PUCCH_1B_2B_NOF_ACK (2)
#define SRSRAN_PUCCH3_NOF_BITS (4 * SRSRAN_NRE)
#define SRSRAN_PUCCH_MAX_SYMBOLS (SRSRAN_PUCCH_N_SEQ * SRSRAN_PUCCH2_N_SF * SRSRAN_NOF_SLOTS_PER_SF)

// PUCCH Format 1B Channel selection
#define SRSRAN_PUCCH_CS_MAX_ACK 4
#define SRSRAN_PUCCH_CS_MAX_CARRIERS 2
#define SRSRAN_PUCCH_FORMAT3_MAX_CARRIERS 5

#define SRSRAN_PUCCH_DEFAULT_THRESHOLD_FORMAT1 (0.5f)
#define SRSRAN_PUCCH_DEFAULT_THRESHOLD_FORMAT1A (0.5f)
#define SRSRAN_PUCCH_DEFAULT_THRESHOLD_FORMAT2 (0.5f)
#define SRSRAN_PUCCH_DEFAULT_THRESHOLD_FORMAT3 (0.5f)
#define SRSRAN_PUCCH_DEFAULT_THRESHOLD_DMRS (0.4f)

/* PUCCH object */
typedef struct SRSRAN_API {
  srsran_cell_t        cell;
  srsran_modem_table_t mod;

  srsran_uci_cqi_pucch_t cqi;

  srsran_sequence_t seq_f2;
  bool              is_ue;

  int16_t  llr[SRSRAN_PUCCH3_NOF_BITS];
  uint8_t  bits_scram[SRSRAN_PUCCH_MAX_BITS];
  cf_t     d[SRSRAN_PUCCH_MAX_BITS / 2];
  uint32_t n_cs_cell[SRSRAN_NSLOTS_X_FRAME][SRSRAN_CP_NORM_NSYMB];
  uint32_t f_gh[SRSRAN_NSLOTS_X_FRAME];

  cf_t* z;
  cf_t* z_tmp;
  cf_t* ce;

} srsran_pucch_t;

typedef struct SRSRAN_API {
  srsran_uci_value_t uci_data;
  float              dmrs_correlation;
  float              snr_db;
  float              rssi_dbFs;
  float              ni_dbFs;
  float              correlation;
  bool               detected;

  // PUCCH Measurements
  bool  ta_valid;
  float ta_us;
} srsran_pucch_res_t;

SRSRAN_API int srsran_pucch_init_ue(srsran_pucch_t* q);

SRSRAN_API int srsran_pucch_init_enb(srsran_pucch_t* q);

SRSRAN_API void srsran_pucch_free(srsran_pucch_t* q);

/* These functions modify the state of the object and may take some time */
SRSRAN_API int srsran_pucch_set_cell(srsran_pucch_t* q, srsran_cell_t cell);

/* These functions do not modify the state and run in real-time */
SRSRAN_API void srsran_pucch_uci_gen_cfg(srsran_pucch_t* q, srsran_pucch_cfg_t* cfg, srsran_uci_data_t* uci_data);

SRSRAN_API int srsran_pucch_encode(srsran_pucch_t*     q,
                                   srsran_ul_sf_cfg_t* sf,
                                   srsran_pucch_cfg_t* cfg,
                                   srsran_uci_value_t* uci_data,
                                   cf_t*               sf_symbols);

SRSRAN_API int srsran_pucch_decode(srsran_pucch_t*        q,
                                   srsran_ul_sf_cfg_t*    sf,
                                   srsran_pucch_cfg_t*    cfg,
                                   srsran_chest_ul_res_t* channel,
                                   cf_t*                  sf_symbols,
                                   srsran_pucch_res_t*    data);

/* Other utilities. These functions do not modify the state and run in real-time */
SRSRAN_API float srsran_pucch_alpha_format1(const uint32_t n_cs_cell[SRSRAN_NSLOTS_X_FRAME][SRSRAN_CP_NORM_NSYMB],
                                            const srsran_pucch_cfg_t* cfg,
                                            srsran_cp_t               cp,
                                            bool                      is_dmrs,
                                            uint32_t                  ns,
                                            uint32_t                  l,
                                            uint32_t*                 n_oc,
                                            uint32_t*                 n_prime_ns);

SRSRAN_API float srsran_pucch_alpha_format2(const uint32_t n_cs_cell[SRSRAN_NSLOTS_X_FRAME][SRSRAN_CP_NORM_NSYMB],
                                            const srsran_pucch_cfg_t* cfg,
                                            uint32_t                  ns,
                                            uint32_t                  l);

SRSRAN_API int srsran_pucch_format2ab_mod_bits(srsran_pucch_format_t format, uint8_t bits[2], cf_t* d_10);

SRSRAN_API uint32_t srsran_pucch_m(const srsran_pucch_cfg_t* cfg, srsran_cp_t cp);

SRSRAN_API uint32_t srsran_pucch_n_prb(const srsran_cell_t* cell, const srsran_pucch_cfg_t* cfg, uint32_t ns);

SRSRAN_API int srsran_pucch_n_cs_cell(srsran_cell_t cell,
                                      uint32_t      n_cs_cell[SRSRAN_NSLOTS_X_FRAME][SRSRAN_CP_NORM_NSYMB]);

/**
 * Checks PUCCH collision from cell and two PUCCH configurations. The provided configurations shall provide format and
 * n_pucch resource prior to this call.
 *
 * @param cell cell parameters
 * @param cfg1 First PUCCH configuration
 * @param cfg2 Second PUCCH configuration
 * @return SRSRAN_SUCCESS if no collision, SRSRAN_ERROR if collision and otherwise SRSRAN_INVALID_INPUTS
 */
SRSRAN_API int
srsran_pucch_collision(const srsran_cell_t* cell, const srsran_pucch_cfg_t* cfg1, const srsran_pucch_cfg_t* cfg2);

/**
 * Checks PUCCH format 1b with channel selection collision configuration from a cell.
 *
 * @param cell cell parameters
 * @param cfg PUCCH configuration
 * @return SRSRAN_SUCCESS if no collision, SRSRAN_ERROR if collision and otherwise SRSRAN_INVALID_INPUTS
 */
SRSRAN_API int srsran_pucch_cfg_assert(const srsran_cell_t* cell, const srsran_pucch_cfg_t* cfg);

SRSRAN_API char* srsran_pucch_format_text(srsran_pucch_format_t format);

SRSRAN_API char* srsran_pucch_format_text_short(srsran_pucch_format_t format);

/**
 * Returns the number of ACK bits supported by a given PUCCH format
 * @param format PUCCH format
 * @return Returns the number of bits supported by the format
 */
SRSRAN_API uint32_t srsran_pucch_nof_ack_format(srsran_pucch_format_t format);

SRSRAN_API void
srsran_pucch_tx_info(srsran_pucch_cfg_t* cfg, srsran_uci_value_t* uci_data, char* str, uint32_t str_len);

SRSRAN_API void
srsran_pucch_rx_info(srsran_pucch_cfg_t* cfg, srsran_pucch_res_t* pucch_res, char* str, uint32_t str_len);

SRSRAN_API bool srsran_pucch_cfg_isvalid(srsran_pucch_cfg_t* cfg, uint32_t nof_prb);

#endif // SRSRAN_PUCCH_H
