/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/common/security.h"
#include "srsran/common/liblte_security.h"
#include "srsran/common/s3g.h"
#include "srsran/config.h"

#ifdef HAVE_MBEDTLS
#include "mbedtls/md5.h"
#endif
#ifdef HAVE_POLARSSL
#include "polarssl/md5.h"
#endif

namespace srsran {

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

uint8_t security_generate_k_nr_rrc(uint8_t*                    k_gnb,
                                   CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                   INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                   uint8_t*                    k_rrc_enc,
                                   uint8_t*                    k_rrc_int)
{
  return liblte_security_generate_k_nr_rrc(k_gnb,
                                           (LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_ENUM)enc_alg_id,
                                           (LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_ENUM)int_alg_id,
                                           k_rrc_enc,
                                           k_rrc_int);
}

uint8_t security_generate_k_nr_up(uint8_t*                    k_gnb,
                                  CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                  INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                  uint8_t*                    k_up_enc,
                                  uint8_t*                    k_up_int)
{
  return liblte_security_generate_k_nr_up(k_gnb,
                                          (LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_ENUM)enc_alg_id,
                                          (LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_ENUM)int_alg_id,
                                          k_up_enc,
                                          k_up_int);
}

uint8_t security_generate_sk_gnb(uint8_t* k_enb, uint8_t* sk_gnb, uint16_t scg_count)
{
  return liblte_security_generate_sk_gnb(k_enb, sk_gnb, scg_count);
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
  return SRSRAN_SUCCESS;
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

int security_xor_f2345(uint8_t* k, uint8_t* rand, uint8_t* res, uint8_t* ck, uint8_t* ik, uint8_t* ak)
{
  uint8_t xdout[16];
  uint8_t cdout[8];
  // Use RAND and K to compute RES, CK, IK and AK
  for (uint32_t i = 0; i < 16; i++) {
    xdout[i] = k[i] ^ rand[i];
  }
  for (uint32_t i = 0; i < 16; i++) {
    res[i] = xdout[i];
    ck[i]  = xdout[(i + 1) % 16];
    ik[i]  = xdout[(i + 2) % 16];
  }
  for (uint32_t i = 0; i < 6; i++) {
    ak[i] = xdout[i + 3];
  }
  return SRSRAN_SUCCESS;
}

int security_xor_f1(uint8_t* k, uint8_t* rand, uint8_t* sqn, uint8_t* amf, uint8_t* mac_a)
{
  uint8_t xdout[16];
  uint8_t cdout[8];
  // Use RAND and K to compute RES, CK, IK and AK
  for (uint32_t i = 0; i < 16; i++) {
    xdout[i] = k[i] ^ rand[i];
  }
  // Generate cdout
  for (uint32_t i = 0; i < 6; i++) {
    cdout[i] = sqn[i];
  }
  for (uint32_t i = 0; i < 2; i++) {
    cdout[6 + i] = amf[i];
  }

  // Generate MAC
  for (uint32_t i = 0; i < 8; i++) {
    mac_a[i] = xdout[i] ^ cdout[i];
  }
  return SRSRAN_SUCCESS;
}

} // namespace srsran
