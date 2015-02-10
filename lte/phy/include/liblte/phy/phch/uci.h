/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#ifndef UCI_
#define UCI_

#include "liblte/config.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/phch/harq.h"
#include "liblte/phy/fec/crc.h"

#define MAX_CQI_LEN     512

typedef struct LIBLTE_API {
  crc_t crc;
  uint8_t tmp_cqi[MAX_CQI_LEN];
  uint8_t encoded_cqi[3*MAX_CQI_LEN];
} uci_cqi_t;

typedef struct LIBLTE_API {
  uint8_t *uci_cqi;
  uint32_t uci_cqi_len;
  float beta_cqi;
  uint8_t uci_ri;  // Only 1-bit supported for RI
  uint32_t uci_ri_len;
  float beta_ri;
  uint8_t uci_ack; // Only 1-bit supported for HARQ
  uint32_t uci_ack_len;
  float beta_ack;
} uci_data_t;

LIBLTE_API int uci_cqi_init(uci_cqi_t *q);

LIBLTE_API void uci_cqi_free(uci_cqi_t *q);

LIBLTE_API int uci_encode_cqi(uci_cqi_t *q, 
                              uint8_t *cqi_data, 
                              uint32_t cqi_len, 
                              float beta, 
                              uint32_t Q_prime_ri, 
                              harq_t *harq_process, 
                              uint8_t *q_bits);

/* Encode UCI RI and HARQ ACK/NACK bits */
LIBLTE_API int uci_encode_ri_ack(uint8_t data,
                                 uint32_t O_cqi,
                                 float beta, 
                                 harq_t *harq_process,
                                 uint8_t q_bits[6]); 

#endif