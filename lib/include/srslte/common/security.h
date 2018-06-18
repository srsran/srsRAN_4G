/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_SECURITY_H
#define SRSLTE_SECURITY_H

/******************************************************************************
 * Common security header - wraps ciphering/integrity check algorithms.
 *****************************************************************************/


#include "srslte/common/common.h"


#define SECURITY_DIRECTION_UPLINK   0
#define SECURITY_DIRECTION_DOWNLINK 1

namespace srslte {

typedef enum{
    CIPHERING_ALGORITHM_ID_EEA0 = 0,
    CIPHERING_ALGORITHM_ID_128_EEA1,
    CIPHERING_ALGORITHM_ID_128_EEA2,
    CIPHERING_ALGORITHM_ID_N_ITEMS,
}CIPHERING_ALGORITHM_ID_ENUM;
static const char ciphering_algorithm_id_text[CIPHERING_ALGORITHM_ID_N_ITEMS][20] = {"EEA0",
                                                                                     "128-EEA1",
                                                                                     "128-EEA2"};
typedef enum{
    INTEGRITY_ALGORITHM_ID_EIA0 = 0,
    INTEGRITY_ALGORITHM_ID_128_EIA1,
    INTEGRITY_ALGORITHM_ID_128_EIA2,
    INTEGRITY_ALGORITHM_ID_N_ITEMS,
}INTEGRITY_ALGORITHM_ID_ENUM;
static const char integrity_algorithm_id_text[INTEGRITY_ALGORITHM_ID_N_ITEMS][20] = {"EIA0",
                                                                                     "128-EIA1",
                                                                                     "128-EIA2"};


/******************************************************************************
 * Key Generation
 *****************************************************************************/

uint8_t security_generate_k_asme( uint8_t  *ck,
                                  uint8_t  *ik,
                                  uint8_t  *ak,
                                  uint8_t  *sqn,
                                  uint16_t  mcc,
                                  uint16_t  mnc,
                                  uint8_t  *k_asme);

uint8_t security_generate_k_enb( uint8_t  *k_asme,
                                 uint32_t  nas_count,
                                 uint8_t  *k_enb);

uint8_t security_generate_k_enb_star( uint8_t  *k_enb,
                                      uint32_t  pci,
                                      uint32_t earfcn,
                                      uint8_t  *k_enb_star);

uint8_t security_generate_nh( uint8_t *k_asme,
                              uint8_t *sync,
                              uint8_t *nh);

uint8_t security_generate_k_nas( uint8_t                       *k_asme,
                                 CIPHERING_ALGORITHM_ID_ENUM    enc_alg_id,
                                 INTEGRITY_ALGORITHM_ID_ENUM    int_alg_id,
                                 uint8_t                       *k_nas_enc,
                                 uint8_t                       *k_nas_int);

uint8_t security_generate_k_rrc( uint8_t                       *k_enb,
                                 CIPHERING_ALGORITHM_ID_ENUM    enc_alg_id,
                                 INTEGRITY_ALGORITHM_ID_ENUM    int_alg_id,
                                 uint8_t                       *k_rrc_enc,
                                 uint8_t                       *k_rrc_int);

uint8_t security_generate_k_up( uint8_t                       *k_enb,
                                CIPHERING_ALGORITHM_ID_ENUM    enc_alg_id,
                                INTEGRITY_ALGORITHM_ID_ENUM    int_alg_id,
                                uint8_t                       *k_up_enc,
                                uint8_t                       *k_up_int);

/******************************************************************************
 * Integrity Protection
 *****************************************************************************/

uint8_t security_128_eia1( uint8_t  *key,
                           uint32_t  count,
                           uint32_t   bearer,
                           uint8_t   direction,
                           uint8_t  *msg,
                           uint32_t  msg_len,
                           uint8_t  *mac);

uint8_t security_128_eia2( uint8_t  *key,
                           uint32_t  count,
                           uint32_t   bearer,
                           uint8_t   direction,
                           uint8_t  *msg,
                           uint32_t  msg_len,
                           uint8_t  *mac);

uint8_t security_md5(const uint8_t *input,
                     size_t         len,
                     uint8_t       *output);


/******************************************************************************
 * Encryption / Decryption
 *****************************************************************************/

uint8_t security_128_eea1( uint8_t  *key,
                           uint32_t  count,
                           uint8_t   bearer,
                           uint8_t   direction,
                           uint8_t  *msg,
                           uint32_t  msg_len,
                           uint8_t  *msg_out);

uint8_t security_128_eea2(uint8_t  *key,
                           uint32_t  count,
                           uint8_t   bearer,
                           uint8_t   direction,
                           uint8_t  *msg,
                           uint32_t  msg_len,
                           uint8_t  *msg_out);

/******************************************************************************
 * Authentication
 *****************************************************************************/
uint8_t compute_opc( uint8_t *k,
                       uint8_t *op,
                       uint8_t *opc);

uint8_t security_milenage_f1( uint8_t *k,
                              uint8_t *op,
                              uint8_t *rand,
                              uint8_t *sqn,
                              uint8_t *amf,
                              uint8_t *mac_a);

uint8_t security_milenage_f1_star( uint8_t *k,
                                   uint8_t *op,
                                   uint8_t *rand,
                                   uint8_t *sqn,
                                   uint8_t *amf,
                                   uint8_t *mac_s);

uint8_t security_milenage_f2345( uint8_t *k,
                                 uint8_t *op,
                                 uint8_t *rand,
                                 uint8_t *res,
                                 uint8_t *ck,
                                 uint8_t *ik,
                                 uint8_t *ak);

uint8_t security_milenage_f5_star( uint8_t *k,
                                   uint8_t *op,
                                   uint8_t *rand,
                                   uint8_t *ak);


} // namespace srslte

#endif // SRSLTE_SECURITY_H
