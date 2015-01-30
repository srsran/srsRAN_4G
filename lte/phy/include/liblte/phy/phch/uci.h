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


LIBLTE_API int uci_encode_cqi(uint8_t *data, 
                              uint8_t *e_bits, 
                              uint32_t tbs, 
                              uint32_t nb_e);

/* Encode UCI RI and HARQ ACK/NACK bits */
LIBLTE_API uint32_t uci_encode_ri_ack(uint8_t data, 
                                      float beta, 
                                      uint8_t q_bits[6], 
                                      harq_t *harq_process);

#endif