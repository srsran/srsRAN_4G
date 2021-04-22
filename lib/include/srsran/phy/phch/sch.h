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
 *  File:         sch.h
 *
 *  Description:  Common UL and DL shared channel encode/decode functions.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10
 *****************************************************************************/

#ifndef SRSRAN_SCH_H
#define SRSRAN_SCH_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/fec/turbo/rm_turbo.h"
#include "srsran/phy/fec/turbo/turbocoder.h"
#include "srsran/phy/fec/turbo/turbodecoder.h"
#include "srsran/phy/phch/pdsch_cfg.h"
#include "srsran/phy/phch/pusch_cfg.h"
#include "srsran/phy/phch/uci.h"

#ifndef SRSRAN_RX_NULL
#define SRSRAN_RX_NULL 10000
#endif

#ifndef SRSRAN_TX_NULL
#define SRSRAN_TX_NULL 100
#endif

/* DL-SCH AND UL-SCH common functions */
typedef struct SRSRAN_API {

  uint32_t max_iterations;
  float    avg_iterations;

  bool llr_is_8bit;

  /* buffers */
  uint8_t*         cb_in;
  uint8_t*         parity_bits;
  void*            e;
  uint8_t*         temp_g_bits;
  uint32_t*        ul_interleaver;
  srsran_uci_bit_t ack_ri_bits[57600]; // 4*M_sc*Qm_max for RI and ACK

  srsran_tcod_t encoder;
  srsran_tdec_t decoder;
  srsran_crc_t  crc_tb;
  srsran_crc_t  crc_cb;

  srsran_uci_cqi_pusch_t uci_cqi;

} srsran_sch_t;

SRSRAN_API int srsran_sch_init(srsran_sch_t* q);

SRSRAN_API void srsran_sch_free(srsran_sch_t* q);

SRSRAN_API void srsran_sch_set_max_noi(srsran_sch_t* q, uint32_t max_iterations);

SRSRAN_API float srsran_sch_last_noi(srsran_sch_t* q);

SRSRAN_API int srsran_dlsch_encode(srsran_sch_t* q, srsran_pdsch_cfg_t* cfg, uint8_t* data, uint8_t* e_bits);

SRSRAN_API int srsran_dlsch_encode2(srsran_sch_t*       q,
                                    srsran_pdsch_cfg_t* cfg,
                                    uint8_t*            data,
                                    uint8_t*            e_bits,
                                    int                 codeword_idx,
                                    uint32_t            nof_layers);

SRSRAN_API int srsran_dlsch_decode(srsran_sch_t* q, srsran_pdsch_cfg_t* cfg, int16_t* e_bits, uint8_t* data);

SRSRAN_API int srsran_dlsch_decode2(srsran_sch_t*       q,
                                    srsran_pdsch_cfg_t* cfg,
                                    int16_t*            e_bits,
                                    uint8_t*            data,
                                    int                 codeword_idx,
                                    uint32_t            nof_layers);

SRSRAN_API int srsran_ulsch_encode(srsran_sch_t*       q,
                                   srsran_pusch_cfg_t* cfg,
                                   uint8_t*            data,
                                   srsran_uci_value_t* uci_data,
                                   uint8_t*            g_bits,
                                   uint8_t*            q_bits);

SRSRAN_API int srsran_ulsch_decode(srsran_sch_t*       q,
                                   srsran_pusch_cfg_t* cfg,
                                   int16_t*            q_bits,
                                   int16_t*            g_bits,
                                   uint8_t*            c_seq,
                                   uint8_t*            data,
                                   srsran_uci_value_t* uci_data);

SRSRAN_API float srsran_sch_beta_cqi(uint32_t I_cqi);

SRSRAN_API float srsran_sch_beta_ack(uint32_t I_harq);

SRSRAN_API uint32_t srsran_sch_find_Ioffset_ack(float beta);

SRSRAN_API uint32_t srsran_sch_find_Ioffset_cqi(float beta);

SRSRAN_API uint32_t srsran_sch_find_Ioffset_ri(float beta);

///< Sidelink uses PUSCH Interleaver in all channels
SRSRAN_API void srsran_sl_ulsch_interleave(uint8_t* g_bits,
                                           uint32_t Qm,
                                           uint32_t H_prime_total,
                                           uint32_t N_pusch_symbs,
                                           uint8_t* q_bits);

///< Sidelink uses PUSCH Deinterleaver in all channels
SRSRAN_API void srsran_sl_ulsch_deinterleave(int16_t*  q_bits,
                                             uint32_t  Qm,
                                             uint32_t  H_prime_total,
                                             uint32_t  N_pusch_symbs,
                                             int16_t*  g_bits,
                                             uint32_t* inteleaver_lut);

#endif // SRSRAN_SCH_H