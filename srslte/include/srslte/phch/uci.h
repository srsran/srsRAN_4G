/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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
 *  File:         uci.h
 *
 *  Description:  Uplink control information. Only 1-bit ACK for UCI on PUSCCH is supported
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.2.3, 5.2.4
 *****************************************************************************/

#ifndef UCI_
#define UCI_

#include "srslte/config.h"
#include "srslte/common/phy_common.h"
#include "srslte/phch/pusch_cfg.h"
#include "srslte/fec/crc.h"
#include "srslte/fec/viterbi.h"
#include "srslte/phch/cqi.h"

#define SRSLTE_UCI_MAX_CQI_LEN_PUSCH       512
#define SRSLTE_UCI_MAX_CQI_LEN_PUCCH       13
#define SRSLTE_UCI_CQI_CODED_PUCCH_B       20

typedef struct SRSLTE_API {
  srslte_crc_t crc;
  srslte_viterbi_t viterbi; 
  uint8_t tmp_cqi[SRSLTE_UCI_MAX_CQI_LEN_PUSCH];
  uint8_t encoded_cqi[3*SRSLTE_UCI_MAX_CQI_LEN_PUSCH];
  int16_t encoded_cqi_s[3*SRSLTE_UCI_MAX_CQI_LEN_PUSCH];
  uint8_t *cqi_table[11];
  int16_t *cqi_table_s[11];
} srslte_uci_cqi_pusch_t;

typedef struct SRSLTE_API {
  uint8_t  uci_cqi[SRSLTE_CQI_MAX_BITS];
  uint32_t uci_cqi_len;
  uint8_t  uci_ri;  // Only 1-bit supported for RI
  uint32_t uci_ri_len;
  uint8_t  uci_ack;   // 1st codeword bit for HARQ-ACK
  uint8_t  uci_ack_2; // 2st codeword bit for HARQ-ACK
  uint32_t uci_ack_len;
  bool scheduling_request; 
  bool channel_selection; 
  bool cqi_ack; 
} srslte_uci_data_t;

typedef enum {
  UCI_BIT_1 = 0, UCI_BIT_0, UCI_BIT_REPETITION, UCI_BIT_PLACEHOLDER
} srslte_uci_bit_type_t;

typedef struct {
  uint32_t position;
  srslte_uci_bit_type_t type;
} srslte_uci_bit_t;

SRSLTE_API int srslte_uci_cqi_init(srslte_uci_cqi_pusch_t *q);

SRSLTE_API void srslte_uci_cqi_free(srslte_uci_cqi_pusch_t *q);

SRSLTE_API int srslte_uci_encode_cqi_pusch(srslte_uci_cqi_pusch_t *q, 
                                           srslte_pusch_cfg_t *cfg,
                                           uint8_t *cqi_data, 
                                           uint32_t cqi_len, 
                                           float beta, 
                                           uint32_t Q_prime_ri, 
                                           uint8_t *q_bits);

SRSLTE_API int srslte_uci_decode_cqi_pusch(srslte_uci_cqi_pusch_t *q, 
                                           srslte_pusch_cfg_t *cfg,
                                           int16_t *q_bits, 
                                           float beta, 
                                           uint32_t Q_prime_ri, 
                                           uint32_t cqi_len, 
                                           uint8_t *cqi_data, 
                                           bool *cqi_ack); 

SRSLTE_API int srslte_uci_encode_cqi_pucch(uint8_t *cqi_data, 
                                           uint32_t cqi_len, 
                                           uint8_t b_bits[SRSLTE_UCI_CQI_CODED_PUCCH_B]);

SRSLTE_API int srslte_uci_encode_ack(srslte_pusch_cfg_t *cfg,
                                     uint8_t data, 
                                     uint32_t O_cqi, 
                                     float beta, 
                                     uint32_t H_prime_total, 
                                     srslte_uci_bit_t *ri_bits); 

SRSLTE_API int srslte_uci_decode_ack(srslte_pusch_cfg_t *cfg,
                                     int16_t *q_bits,
                                     uint8_t *c_seq, 
                                     float beta, 
                                     uint32_t H_prime_total, 
                                     uint32_t O_cqi,
                                     srslte_uci_bit_t *ack_bits,
                                     uint8_t *data); 

SRSLTE_API int srslte_uci_encode_ri(srslte_pusch_cfg_t *cfg,
                                    uint8_t data, 
                                    uint32_t O_cqi, 
                                    float beta, 
                                    uint32_t H_prime_total, 
                                    srslte_uci_bit_t *ri_bits); 

SRSLTE_API int srslte_uci_decode_ri(srslte_pusch_cfg_t *cfg,
                                    int16_t *q_bits, 
                                    uint8_t *c_seq, 
                                    float beta, 
                                    uint32_t H_prime_total, 
                                    uint32_t O_cqi, 
                                    srslte_uci_bit_t *ri_bits,
                                    uint8_t *data); 


#endif
