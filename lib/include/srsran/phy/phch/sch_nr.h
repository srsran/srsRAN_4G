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
 *  File:         sch_nr.h
 *
 *  Description:  Common UL and DL shared channel encode/decode functions for NR.
 *
 *  Reference:    3GPP TS 38.212 V15.9.0
 *****************************************************************************/

#ifndef SRSRAN_SCH_NR_H
#define SRSRAN_SCH_NR_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/fec/ldpc/ldpc_decoder.h"
#include "srsran/phy/fec/ldpc/ldpc_encoder.h"
#include "srsran/phy/fec/ldpc/ldpc_rm.h"
#include "srsran/phy/phch/phch_cfg_nr.h"

/**
 * @brief Maximum number of codeblocks for a NR shared channel transmission. It assumes a rate of 1.0 for the maximum
 * amount of bits a resource grid can fit
 */
#define SRSRAN_SCH_NR_MAX_NOF_CB_LDPC                                                                                  \
  ((SRSRAN_SLOT_MAX_NOF_BITS_NR + (SRSRAN_LDPC_MAX_LEN_CB - 1)) / SRSRAN_LDPC_MAX_LEN_CB)

/**
 * @brief Groups NR-PUSCH data for reception
 */
typedef struct {
  uint8_t* payload;  ///< SCH payload
  bool     crc;      ///< CRC match
  float    avg_iter; ///< Average iterations
} srsran_sch_tb_res_nr_t;

typedef struct SRSRAN_API {
  srsran_carrier_nr_t carrier;

  /// Temporal data buffers
  uint8_t* temp_cb;

  /// CRC generators
  srsran_crc_t crc_tb_24;
  srsran_crc_t crc_tb_16;
  srsran_crc_t crc_cb;

  /// LDPC encoders
  srsran_ldpc_encoder_t* encoder_bg1[MAX_LIFTSIZE + 1];
  srsran_ldpc_encoder_t* encoder_bg2[MAX_LIFTSIZE + 1];

  /// LDPC decoders
  srsran_ldpc_decoder_t* decoder_bg1[MAX_LIFTSIZE + 1];
  srsran_ldpc_decoder_t* decoder_bg2[MAX_LIFTSIZE + 1];

  /// LDPC Rate matcher
  srsran_ldpc_rm_t tx_rm;
  srsran_ldpc_rm_t rx_rm;
} srsran_sch_nr_t;

/**
 * @brief SCH encoder and decoder initialization arguments
 */
typedef struct SRSRAN_API {
  bool     disable_simd;
  bool     decoder_use_flooded;
  float    decoder_scaling_factor;
  uint32_t max_nof_iter; ///< Maximum number of LDPC iterations
} srsran_sch_nr_args_t;

/**
 * @brief Common SCH configuration
 */
typedef struct {
  srsran_basegraph_t bg;   ///< @brief Base graph
  uint32_t           Qm;   ///< @brief Modulation order
  uint32_t           G;    ///< Number of available bits
  uint32_t           A;    ///< @brief Payload size, TBS
  uint32_t           L_tb; ///< @brief the number of the transport block parity bits (16 or 24 bits)
  uint32_t           L_cb; ///< @brief the number of the code block parity bits (0 or 24 bits)
  uint32_t           B;    ///< @brief the number of bits in the transport block including TB CRC
  uint32_t           Bp;   ///< @brief the number of bits in the transport block including CB and TB CRCs
  uint32_t           Kp;   ///< @brief Number of payload bits of the code block including CB CRC
  uint32_t           Kr;   ///< @brief Number of payload bits of the code block including CB CRC and filler bits
  uint32_t           F;    ///< @brief Number of filler bits
  uint32_t           Nref; ///< @brief N_ref parameter described in TS 38.212 V15.9.0 5.4.2.1
  uint32_t           Z;    ///< @brief LDPC lifting size
  uint32_t           Nl;   ///< @brief Number of transmission layers that the transport block is mapped onto
  bool               mask[SRSRAN_SCH_NR_MAX_NOF_CB_LDPC]; ///< Indicates what codeblocks shall be encoded/decoded
  uint32_t           C;                                   ///< Number of codeblocks
  uint32_t           Cp;                                  ///< Number of codeblocks that are actually transmitted
} srsran_sch_nr_tb_info_t;

/**
 * @brief Base graph selection from a provided transport block size and target rate
 *
 * @remark Defined for DL-SCH in TS 38.212 V15.9.0 section 7.2.2 LDPC base graph selection
 * @remark Defined for UL-SCH in TS 38.212 V15.9.0 section 6.2.2 LDPC base graph selection
 *
 * @param tbs the payload size as described in Clause 7.2.1 for DL-SCH and 6.2.1 for UL-SCH.
 * @param R Target rate
 * @return it returns the selected base graph
 */
SRSRAN_API srsran_basegraph_t srsran_sch_nr_select_basegraph(uint32_t tbs, double R);

/**
 * @brief Calculates all the parameters required for performing TS 38.212 V15.9.0 5.4 General procedures for LDPC
 * @param sch_cfg Provides higher layers configuration
 * @param tb Provides transport block configuration
 * @param cfg SCH object
 * @return
 */
SRSRAN_API int srsran_sch_nr_fill_tb_info(const srsran_carrier_nr_t* carrier,
                                          const srsran_sch_cfg_t*    sch_cfg,
                                          const srsran_sch_tb_t*     tb,
                                          srsran_sch_nr_tb_info_t*   cfg);

/**
 * @brief Initialises an SCH object as transmitter
 * @param q Points ats the SCH object
 * @param args Provides static configuration arguments
 * @return SRSRAN_SUCCESS if the initialization is successful, SRSRAN_ERROR otherwise
 */
SRSRAN_API int srsran_sch_nr_init_tx(srsran_sch_nr_t* q, const srsran_sch_nr_args_t* args);

/**
 * @brief Initialises an SCH object as receiver
 * @param q Points ats the SCH object
 * @param args Provides static configuration arguments
 * @return SRSRAN_SUCCESS if the initialization is successful, SRSRAN_ERROR otherwise
 */
SRSRAN_API int srsran_sch_nr_init_rx(srsran_sch_nr_t* q, const srsran_sch_nr_args_t* args);

/**
 * @brief Sets SCH object carrier attribute
 * @param q Points ats the SCH object
 * @param carrier Provides the NR carrier object
 * @return SRSRAN_SUCCESS if the setting is successful, SRSRAN_ERROR otherwise
 */
SRSRAN_API int srsran_sch_nr_set_carrier(srsran_sch_nr_t* q, const srsran_carrier_nr_t* carrier);

/**
 * @brief Free allocated resources used by an SCH intance
 * @param q Points ats the SCH object
 */
SRSRAN_API void srsran_sch_nr_free(srsran_sch_nr_t* q);

SRSRAN_API int srsran_dlsch_nr_encode(srsran_sch_nr_t*        q,
                                      const srsran_sch_cfg_t* cfg,
                                      const srsran_sch_tb_t*  tb,
                                      const uint8_t*          data,
                                      uint8_t*                e_bits);

SRSRAN_API int srsran_dlsch_nr_decode(srsran_sch_nr_t*        q,
                                      const srsran_sch_cfg_t* sch_cfg,
                                      const srsran_sch_tb_t*  tb,
                                      int8_t*                 e_bits,
                                      srsran_sch_tb_res_nr_t* res);

SRSRAN_API int srsran_ulsch_nr_encode(srsran_sch_nr_t*        q,
                                      const srsran_sch_cfg_t* cfg,
                                      const srsran_sch_tb_t*  tb,
                                      const uint8_t*          data,
                                      uint8_t*                e_bits);

SRSRAN_API int srsran_ulsch_nr_decode(srsran_sch_nr_t*        q,
                                      const srsran_sch_cfg_t* sch_cfg,
                                      const srsran_sch_tb_t*  tb,
                                      int8_t*                 e_bits,
                                      srsran_sch_tb_res_nr_t* res);

SRSRAN_API int
srsran_sch_nr_tb_info(const srsran_sch_tb_t* tb, const srsran_sch_tb_res_nr_t* res, char* str, uint32_t str_len);

#endif // SRSRAN_SCH_NR_H