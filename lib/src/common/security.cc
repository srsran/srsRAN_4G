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

#include "srslte/common/security.h"
#include "srslte/common/liblte_security.h"
#include "srslte/common/s3g.h"

#ifdef HAVE_MBEDTLS
#include "mbedtls/md5.h"
#endif
#ifdef HAVE_POLARSSL
#include "polarssl/md5.h"
#endif

namespace srslte {

/******************************************************************************
 * Key Generation
 *****************************************************************************/

uint8_t security_generate_k_asme(uint8_t* ck,
                                 uint8_t* ik,
                                 uint8_t* ak,
                                 uint8_t* sqn,
                                 uint16_t mcc,
                                 uint16_t mnc,
                                 uint8_t* k_asme)
{
  return liblte_security_generate_k_asme(ck, ik, ak, sqn, mcc, mnc, k_asme);
}

uint8_t security_generate_k_enb(uint8_t* k_asme, uint32_t nas_count, uint8_t* k_enb)
{
  return liblte_security_generate_k_enb(k_asme, nas_count, k_enb);
}

uint8_t security_generate_k_enb_star(uint8_t* k_enb, uint32_t pci, uint32_t earfcn, uint8_t* k_enb_star)
{
  return liblte_security_generate_k_enb_star(k_enb, pci, earfcn, k_enb_star);
}

uint8_t security_generate_nh(uint8_t* k_asme, uint8_t* sync, uint8_t* nh)
{
  return liblte_security_generate_nh(k_asme, sync, nh);
}

uint8_t security_generate_k_nas(uint8_t*                    k_asme,
                                CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                uint8_t*                    k_nas_enc,
                                uint8_t*                    k_nas_int)
{
  return liblte_security_generate_k_nas(k_asme,
                                        (LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_ENUM)enc_alg_id,
                                        (LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_ENUM)int_alg_id,
                                        k_nas_enc,
                                        k_nas_int);
}

uint8_t security_generate_k_rrc(uint8_t*                    k_enb,
                                CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                uint8_t*                    k_rrc_enc,
                                uint8_t*                    k_rrc_int)
{
  return liblte_security_generate_k_rrc(k_enb,
                                        (LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_ENUM)enc_alg_id,
                                        (LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_ENUM)int_alg_id,
                                        k_rrc_enc,
                                        k_rrc_int);
}

uint8_t security_generate_k_up(uint8_t*                    k_enb,
                               CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                               INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                               uint8_t*                    k_up_enc,
                               uint8_t*                    k_up_int)
{
  return liblte_security_generate_k_up(k_enb,
                                       (LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_ENUM)enc_alg_id,
                                       (LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_ENUM)int_alg_id,
                                       k_up_enc,
                                       k_up_int);
}

/******************************************************************************
 * Integrity Protection
 *****************************************************************************/

uint8_t security_128_eia1(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac)
{
  return liblte_security_128_eia1(key, count, bearer, direction, msg, msg_len, mac);
}

uint8_t security_128_eia2(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac)
{
  return liblte_security_128_eia2(key, count, bearer, direction, msg, msg_len, mac);
}

uint8_t security_128_eia3(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac)
{
  return liblte_security_128_eia3(key, count, bearer, direction, msg, msg_len * 8, mac);
}

uint8_t security_md5(const uint8_t* input, size_t len, uint8_t* output)
{
  memset(output, 0x00, 16);
#ifdef HAVE_MBEDTLS
  mbedtls_md5(input, len, output);
#endif // HAVE_MBEDTLS
#ifdef HAVE_POLARSSL
  md5(input, len, output);
#endif
  return SRSLTE_SUCCESS;
}

/******************************************************************************
 * Encryption / Decryption
 *****************************************************************************/

uint8_t security_128_eea1(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out)
{

  return liblte_security_encryption_eea1(key, count, bearer, direction, msg, msg_len * 8, msg_out);
}

uint8_t security_128_eea2(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out)
{

  return liblte_security_encryption_eea2(key, count, bearer, direction, msg, msg_len * 8, msg_out);
}

uint8_t security_128_eea3(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out)
{

  return liblte_security_encryption_eea3(key, count, bearer, direction, msg, msg_len * 8, msg_out);
}

/******************************************************************************
 * Authentication
 *****************************************************************************/
uint8_t compute_opc(uint8_t* k, uint8_t* op, uint8_t* opc)
{
  return liblte_compute_opc(k, op, opc);
}

uint8_t security_milenage_f1(uint8_t* k, uint8_t* op, uint8_t* rand, uint8_t* sqn, uint8_t* amf, uint8_t* mac_a)
{
  return liblte_security_milenage_f1(k, op, rand, sqn, amf, mac_a);
}

uint8_t security_milenage_f1_star(uint8_t* k, uint8_t* op, uint8_t* rand, uint8_t* sqn, uint8_t* amf, uint8_t* mac_s)
{
  return liblte_security_milenage_f1_star(k, op, rand, sqn, amf, mac_s);
}

uint8_t
security_milenage_f2345(uint8_t* k, uint8_t* op, uint8_t* rand, uint8_t* res, uint8_t* ck, uint8_t* ik, uint8_t* ak)
{
  return liblte_security_milenage_f2345(k, op, rand, res, ck, ik, ak);
}

uint8_t security_milenage_f5_star(uint8_t* k, uint8_t* op, uint8_t* rand, uint8_t* ak)
{
  return liblte_security_milenage_f5_star(k, op, rand, ak);
}

} // namespace srslte
