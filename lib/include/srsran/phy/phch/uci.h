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
 *  File:         uci.h
 *
 *  Description:  Uplink control information. Only 1-bit ACK for UCI on PUSCCH is supported
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.2.3, 5.2.4
 *****************************************************************************/

#ifndef SRSRAN_UCI_H
#define SRSRAN_UCI_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/fec/convolutional/viterbi.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/phch/pusch_cfg.h"
#include "srsran/phy/phch/uci_cfg.h"

#define SRSRAN_UCI_MAX_CQI_LEN_PUSCH 512
#define SRSRAN_UCI_MAX_CQI_LEN_PUCCH 13
#define SRSRAN_UCI_CQI_CODED_PUCCH_B 20
#define SRSRAN_UCI_STR_MAX_CHAR 32

typedef struct SRSRAN_API {
  srsran_crc_t     crc;
  srsran_viterbi_t viterbi;
  uint8_t          tmp_cqi[SRSRAN_UCI_MAX_CQI_LEN_PUSCH];
  uint8_t          encoded_cqi[3 * SRSRAN_UCI_MAX_CQI_LEN_PUSCH];
  int16_t          encoded_cqi_s[3 * SRSRAN_UCI_MAX_CQI_LEN_PUSCH];
} srsran_uci_cqi_pusch_t;

typedef struct SRSRAN_API {
  uint8_t** cqi_table;
  int16_t** cqi_table_s;
} srsran_uci_cqi_pucch_t;

SRSRAN_API void srsran_uci_cqi_pucch_init(srsran_uci_cqi_pucch_t* q);

SRSRAN_API void srsran_uci_cqi_pucch_free(srsran_uci_cqi_pucch_t* q);

SRSRAN_API int
srsran_uci_encode_cqi_pucch(uint8_t* cqi_data, uint32_t cqi_len, uint8_t b_bits[SRSRAN_UCI_CQI_CODED_PUCCH_B]);

SRSRAN_API int srsran_uci_encode_cqi_pucch_from_table(srsran_uci_cqi_pucch_t* q,
                                                      uint8_t*                cqi_data,
                                                      uint32_t                cqi_len,
                                                      uint8_t                 b_bits[SRSRAN_UCI_CQI_CODED_PUCCH_B]);

SRSRAN_API int16_t srsran_uci_decode_cqi_pucch(srsran_uci_cqi_pucch_t* q,
                                               int16_t                 b_bits[SRSRAN_CQI_MAX_BITS], // aligned for simd
                                               uint8_t*                cqi_data,
                                               uint32_t                cqi_len);

SRSRAN_API int srsran_uci_cqi_init(srsran_uci_cqi_pusch_t* q);

SRSRAN_API void srsran_uci_cqi_free(srsran_uci_cqi_pusch_t* q);

SRSRAN_API int srsran_uci_encode_cqi_pusch(srsran_uci_cqi_pusch_t* q,
                                           srsran_pusch_cfg_t*     cfg,
                                           uint8_t*                cqi_data,
                                           uint32_t                cqi_len,
                                           float                   beta,
                                           uint32_t                Q_prime_ri,
                                           uint8_t*                q_bits);

SRSRAN_API int srsran_uci_decode_cqi_pusch(srsran_uci_cqi_pusch_t* q,
                                           srsran_pusch_cfg_t*     cfg,
                                           int16_t*                q_bits,
                                           float                   beta,
                                           uint32_t                Q_prime_ri,
                                           uint32_t                cqi_len,
                                           uint8_t*                cqi_data,
                                           bool*                   cqi_ack);

SRSRAN_API int srsran_uci_encode_ack(srsran_pusch_cfg_t* cfg,
                                     uint8_t             acks[2],
                                     uint32_t            nof_acks,
                                     uint32_t            O_cqi,
                                     float               beta,
                                     uint32_t            H_prime_total,
                                     srsran_uci_bit_t*   ri_bits);

SRSRAN_API int srsran_uci_encode_ack_ri(srsran_pusch_cfg_t* cfg,
                                        uint8_t*            data,
                                        uint32_t            O_ack,
                                        uint32_t            O_cqi,
                                        float               beta,
                                        uint32_t            H_prime_total,
                                        bool                input_is_ri,
                                        uint32_t            N_bundle,
                                        srsran_uci_bit_t*   ri_bits);

SRSRAN_API int srsran_uci_decode_ack_ri(srsran_pusch_cfg_t* cfg,
                                        int16_t*            q_bits,
                                        uint8_t*            c_seq,
                                        float               beta,
                                        uint32_t            H_prime_total,
                                        uint32_t            O_cqi,
                                        srsran_uci_bit_t*   ack_ri_bits,
                                        uint8_t*            data,
                                        bool*               valid,
                                        uint32_t            nof_bits,
                                        bool                is_ri);

/**
 * Calculates the maximum number of coded symbols used by CQI-UCI over PUSCH
 */
SRSRAN_API uint32_t srsran_qprime_cqi_ext(uint32_t L_prb, uint32_t nof_symbols, uint32_t tbs, float beta);

/**
 * Calculates the maximum number of coded symbols used by ACK/RI over PUSCH
 */
SRSRAN_API uint32_t
srsran_qprime_ack_ext(uint32_t L_prb, uint32_t nof_symbols, uint32_t tbs, uint32_t nof_ack, float beta);

/**
 * Calculates the number of acknowledgements carried by the Uplink Control Information (UCI) deduced from the number of
 * transport blocks indicated in the UCI's configuration.
 *
 * @param uci_cfg is the UCI configuration
 * @return the number of acknowledgements
 */
SRSRAN_API uint32_t srsran_uci_cfg_total_ack(const srsran_uci_cfg_t* uci_cfg);

SRSRAN_API void srsran_uci_data_reset(srsran_uci_data_t* uci_data);

SRSRAN_API int
srsran_uci_data_info(srsran_uci_cfg_t* uci_cfg, srsran_uci_value_t* uci_data, char* str, uint32_t maxlen);

#endif // SRSRAN_UCI_H
