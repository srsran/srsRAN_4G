/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
 *  File:         pdcch.h
 *
 *  Description:  Physical downlink control channel.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.8
 *****************************************************************************/

#ifndef SRSRAN_PDCCH_H
#define SRSRAN_PDCCH_H

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/fec/convolutional/convcoder.h"
#include "srsran/phy/fec/convolutional/rm_conv.h"
#include "srsran/phy/fec/convolutional/viterbi.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/modem/mod.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/scrambling/scrambling.h"

typedef enum SRSRAN_API { SEARCH_UE, SEARCH_COMMON } srsran_pdcch_search_mode_t;

/* PDCCH object */
typedef struct SRSRAN_API {
  srsran_cell_t cell;
  uint32_t      nof_regs[3];
  uint32_t      nof_cce[3];
  uint32_t      max_bits;
  uint32_t      nof_rx_antennas;
  bool          is_ue;

  srsran_regs_t* regs;

  /* buffers */
  cf_t*    ce[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];
  cf_t*    symbols[SRSRAN_MAX_PORTS];
  cf_t*    x[SRSRAN_MAX_PORTS];
  cf_t*    d;
  uint8_t* e;
  float    rm_f[3 * (SRSRAN_DCI_MAX_BITS + 16)];
  float*   llr;

  /* tx & rx objects */
  srsran_modem_table_t mod;
  srsran_sequence_t    seq[SRSRAN_NOF_SF_X_FRAME];
  srsran_viterbi_t     decoder;
  srsran_crc_t         crc;

} srsran_pdcch_t;

SRSRAN_API int srsran_pdcch_init_ue(srsran_pdcch_t* q, uint32_t max_prb, uint32_t nof_rx_antennas);

SRSRAN_API int srsran_pdcch_init_enb(srsran_pdcch_t* q, uint32_t max_prb);

SRSRAN_API int srsran_pdcch_set_cell(srsran_pdcch_t* q, srsran_regs_t* regs, srsran_cell_t cell);

SRSRAN_API void srsran_pdcch_set_regs(srsran_pdcch_t* q, srsran_regs_t* regs);

SRSRAN_API void srsran_pdcch_free(srsran_pdcch_t* q);

SRSRAN_API float srsran_pdcch_coderate(uint32_t nof_bits, uint32_t l);

/* Encoding function */
SRSRAN_API int srsran_pdcch_encode(srsran_pdcch_t*     q,
                                   srsran_dl_sf_cfg_t* sf,
                                   srsran_dci_msg_t*   msg,
                                   cf_t*               sf_symbols[SRSRAN_MAX_PORTS]);

/* Decoding functions: Extract the LLRs and save them in the srsran_pdcch_t object */

SRSRAN_API int srsran_pdcch_extract_llr(srsran_pdcch_t*        q,
                                        srsran_dl_sf_cfg_t*    sf,
                                        srsran_chest_dl_res_t* channel,
                                        cf_t*                  sf_symbols[SRSRAN_MAX_PORTS]);

/* Decoding functions: Try to decode a DCI message after calling srsran_pdcch_extract_llr */
SRSRAN_API int
srsran_pdcch_decode_msg(srsran_pdcch_t* q, srsran_dl_sf_cfg_t* sf, srsran_dci_cfg_t* dci_cfg, srsran_dci_msg_t* msg);

/**
 * @brief Computes decoded DCI correlation. It encodes the given DCI message and compares it with the received LLRs
 * @param q PDCCH object
 * @param msg Previously decoded DCI message
 * @return The normalized correlation between the restored symbols and the received LLRs
 */
SRSRAN_API float srsran_pdcch_msg_corr(srsran_pdcch_t* q, srsran_dci_msg_t* msg);

SRSRAN_API int
srsran_pdcch_dci_decode(srsran_pdcch_t* q, float* e, uint8_t* data, uint32_t E, uint32_t nof_bits, uint16_t* crc);

SRSRAN_API int
srsran_pdcch_dci_encode(srsran_pdcch_t* q, uint8_t* data, uint8_t* e, uint32_t nof_bits, uint32_t E, uint16_t rnti);

SRSRAN_API void
srsran_pdcch_dci_encode_conv(srsran_pdcch_t* q, uint8_t* data, uint32_t nof_bits, uint8_t* coded_data, uint16_t rnti);

/* Function for generation of UE-specific search space DCI locations */
SRSRAN_API uint32_t srsran_pdcch_ue_locations(srsran_pdcch_t*        q,
                                              srsran_dl_sf_cfg_t*    sf,
                                              srsran_dci_location_t* locations,
                                              uint32_t               max_locations,
                                              uint16_t               rnti);

SRSRAN_API uint32_t srsran_pdcch_ue_locations_ncce(uint32_t               nof_cce,
                                                   srsran_dci_location_t* c,
                                                   uint32_t               max_candidates,
                                                   uint32_t               sf_idx,
                                                   uint16_t               rnti);

SRSRAN_API uint32_t srsran_pdcch_ue_locations_ncce_L(uint32_t               nof_cce,
                                                     srsran_dci_location_t* c,
                                                     uint32_t               max_candidates,
                                                     uint32_t               sf_idx,
                                                     uint16_t               rnti,
                                                     int                    L);

/* Function for generation of common search space DCI locations */
SRSRAN_API uint32_t srsran_pdcch_common_locations(srsran_pdcch_t*        q,
                                                  srsran_dci_location_t* locations,
                                                  uint32_t               max_locations,
                                                  uint32_t               cfi);

SRSRAN_API uint32_t srsran_pdcch_common_locations_ncce(uint32_t               nof_cce,
                                                       srsran_dci_location_t* c,
                                                       uint32_t               max_candidates);

#endif // SRSRAN_PDCCH_H
