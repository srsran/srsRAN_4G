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
 *  File:         sch_nr.h
 *
 *  Description:  Common UL and DL shared channel encode/decode functions for NR.
 *
 *  Reference:    3GPP TS 38.212 V15.9.0
 *****************************************************************************/

#ifndef SRSLTE_SCH_NR_H
#define SRSLTE_SCH_NR_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/ldpc/ldpc_decoder.h"
#include "srslte/phy/fec/ldpc/ldpc_encoder.h"
#include "srslte/phy/fec/ldpc/ldpc_rm.h"
#include "srslte/phy/phch/pdsch_cfg_nr.h"
#include "srslte/phy/phch/ra_nr.h"

typedef struct SRSLTE_API {
  srslte_carrier_nr_t carrier;

  /// Temporal data buffers
  uint8_t* cb_in;

  /// CRC generators
  srslte_crc_t crc_tb_24;
  srslte_crc_t crc_tb_16;
  srslte_crc_t crc_cb;

  /// LDPC encoders
  srslte_ldpc_encoder_t* encoder_bg1[MAX_LIFTSIZE];
  srslte_ldpc_encoder_t* encoder_bg2[MAX_LIFTSIZE];

  /// LDPC Rate matcher
  srslte_ldpc_rm_t rm;
} srslte_sch_nr_encoder_t;

typedef struct SRSLTE_API {
  bool  disable_simd;
  bool  use_flooded;
  float scaling_factor;
} srslte_sch_nr_decoder_cfg_t;

typedef struct SRSLTE_API {
  /// CRC generators
  srslte_crc_t crc_tb_24;
  srslte_crc_t crc_tb_16;
  srslte_crc_t crc_cb;

  /// LDPC decoders
  srslte_ldpc_decoder_t* decoder_bg1[MAX_LIFTSIZE];
  srslte_ldpc_decoder_t* decoder_bg2[MAX_LIFTSIZE];

  /// LDPC Tx/Rx Rate matcher
  srslte_ldpc_rm_t rm;
} srslte_sch_nr_decoder_t;

/**
 * @brief Initialises shared channel encoder
 * @param q
 * @return
 */
SRSLTE_API int srslte_sch_nr_encoder_init(srslte_sch_nr_encoder_t* q);

SRSLTE_API int srslte_sch_nr_encoder_set_carrier(srslte_sch_nr_encoder_t* q, const srslte_carrier_nr_t* carrier);

SRSLTE_API int srslte_dlsch_nr_encode(srslte_sch_nr_encoder_t*     q,
                                      const srslte_pdsch_cfg_nr_t* cfg,
                                      const srslte_ra_tb_nr_t*     tb,
                                      const uint8_t*               data,
                                      uint8_t*                     e_bits);

SRSLTE_API void srslte_sch_nr_encoder_free(srslte_sch_nr_encoder_t* q);

/**
 * @brief Initialises shared channel decoder
 * @param q
 * @return
 */
SRSLTE_API int srslte_sch_nr_init_decoder(srslte_sch_nr_decoder_t* q, const srslte_sch_nr_decoder_cfg_t* cfg);

SRSLTE_API int srslte_dlsch_nr_decode(srslte_sch_nr_decoder_t*       q,
                                      const srslte_pdsch_cfg_nr_t*   cfg,
                                      const srslte_pdsch_grant_nr_t* grant,
                                      int8_t*                        e_bits,
                                      uint8_t*                       data);

SRSLTE_API void srslte_sch_nr_decoder_free(srslte_sch_nr_decoder_t* q);

#endif // SRSLTE_SCH_NR_H