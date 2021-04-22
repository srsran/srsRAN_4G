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

#include <stdio.h>
#include <stdlib.h>

#include "srsran/common/liblte_security.h"
#include "srsran/common/test_common.h"
/*
 * Prototypes
 */

int32 arrcmp(uint8_t const* const a, uint8_t const* const b, uint32 len)
{
  uint32 i = 0;

  for (i = 0; i < len; i++) {
    if (a[i] != b[i]) {
      return a[i] - b[i];
    }
  }
  return 0;
}

void arrprint(uint8_t const* const a, uint32 len)
{
  uint32 i = 0;

  for (i = 0; i < len; i++) {
    printf("0x%02x ", a[i]);
    if ((i % 16 == 0) && i)
      printf("\n");
  }
  printf("\n");
  return;
}

/*
 * Tests
 *
 * Document Reference: 35.208 e00
 */

/*
 * Functions
 */

int test_set_2()
{
  LIBLTE_ERROR_ENUM err_lte = LIBLTE_ERROR_INVALID_INPUTS;
  int32             err_cmp = 0;

  uint8_t k[]    = {0x46, 0x5b, 0x5c, 0xe8, 0xb1, 0x99, 0xb4, 0x9f, 0xaa, 0x5f, 0x0a, 0x2e, 0xe2, 0x38, 0xa6, 0xbc};
  uint8_t rand[] = {0x23, 0x55, 0x3c, 0xbe, 0x96, 0x37, 0xa8, 0x9d, 0x21, 0x8a, 0xe6, 0x4d, 0xae, 0x47, 0xbf, 0x35};
  uint8_t sqn[]  = {0xff, 0x9b, 0xb4, 0xd0, 0xb6, 0x07};
  uint8_t amf[]  = {0xb9, 0xb9};
  uint8_t op[]   = {0xcd, 0xc2, 0x02, 0xd5, 0x12, 0x3e, 0x20, 0xf6, 0x2b, 0x6d, 0x67, 0x6a, 0xc7, 0x2c, 0xb3, 0x18};
  // f1

  uint8_t opc_o[16];
  err_lte = liblte_compute_opc(k, op, opc_o);
  TESTASSERT(err_lte == LIBLTE_SUCCESS);

  arrprint(opc_o, sizeof(opc_o));

  uint8_t opc_a[] = {0xcd, 0x63, 0xcb, 0x71, 0x95, 0x4a, 0x9f, 0x4e, 0x48, 0xa5, 0x99, 0x4e, 0x37, 0xa0, 0x2b, 0xaf};
  err_cmp         = arrcmp(opc_o, opc_a, sizeof(opc_o));
  TESTASSERT(err_cmp == 0);

  uint8_t mac_o[8];
  err_lte = liblte_security_milenage_f1(k, opc_o, rand, sqn, amf, mac_o);
  TESTASSERT(err_lte == LIBLTE_SUCCESS);

  arrprint(mac_o, sizeof(mac_o));

  uint8_t mac_a[] = {0x4a, 0x9f, 0xfa, 0xc3, 0x54, 0xdf, 0xaf, 0xb3};

  // compare mac a
  err_cmp = arrcmp(mac_o, mac_a, sizeof(mac_a));
  TESTASSERT(err_cmp == 0);

  // f1 star

  uint8_t mac_so[8];
  err_lte = liblte_security_milenage_f1_star(k, opc_o, rand, sqn, amf, mac_so);

  TESTASSERT(err_lte == LIBLTE_SUCCESS);

  uint8_t mac_s[] = {0x01, 0xcf, 0xaf, 0x9e, 0xc4, 0xe8, 0x71, 0xe9};

  arrprint(mac_so, sizeof(mac_so));

  err_cmp = arrcmp(mac_so, mac_s, sizeof(mac_s));
  TESTASSERT(err_cmp == 0);

  // f2345
  uint8_t res_o[8];
  uint8_t ck_o[16];
  uint8_t ik_o[16];
  uint8_t ak_o[6];

  err_lte = liblte_security_milenage_f2345(k, opc_o, rand, res_o, ck_o, ik_o, ak_o);

  TESTASSERT(err_lte == LIBLTE_SUCCESS);

  uint8_t res[] = {0xa5, 0x42, 0x11, 0xd5, 0xe3, 0xba, 0x50, 0xbf};
  uint8_t ck[]  = {0xb4, 0x0b, 0xa9, 0xa3, 0xc5, 0x8b, 0x2a, 0x05, 0xbb, 0xf0, 0xd9, 0x87, 0xb2, 0x1b, 0xf8, 0xcb};
  uint8_t ik[]  = {0xf7, 0x69, 0xbc, 0xd7, 0x51, 0x04, 0x46, 0x04, 0x12, 0x76, 0x72, 0x71, 0x1c, 0x6d, 0x34, 0x41};
  uint8_t ak[]  = {0xaa, 0x68, 0x9c, 0x64, 0x83, 0x70};

  // RESPONSE
  arrprint(res_o, sizeof(res_o));

  err_cmp = arrcmp(res_o, res, sizeof(res));
  TESTASSERT(err_cmp == 0);

  // CK
  arrprint(ck_o, sizeof(ck_o));

  err_cmp = arrcmp(ck_o, ck, sizeof(ck));
  TESTASSERT(err_cmp == 0);

  // IK
  arrprint(ik_o, sizeof(ik_o));
  err_cmp = arrcmp(ik_o, ik, sizeof(ik));
  TESTASSERT(err_cmp == 0);

  // AK
  arrprint(ak_o, sizeof(ak_o));
  err_cmp = arrcmp(ak_o, ak, sizeof(ak));
  TESTASSERT(err_cmp == 0);

  // f star
  uint8_t ak_star_o[6];

  err_lte = liblte_security_milenage_f5_star(k, opc_o, rand, ak_star_o);
  TESTASSERT(err_lte == LIBLTE_SUCCESS);

  arrprint(ak_star_o, sizeof(ak_star_o));
  uint8_t ak_star[] = {0x45, 0x1e, 0x8b, 0xec, 0xa4, 0x3b};
  err_cmp           = arrcmp(ak_star_o, ak_star, sizeof(ak_star));
  TESTASSERT(err_cmp == 0);
  return SRSRAN_SUCCESS;
  ;
}

int test_set_ksg()
{
  LIBLTE_ERROR_ENUM err_lte = LIBLTE_ERROR_INVALID_INPUTS;
  int32             err_cmp = 0;

  uint8_t  k_enb[] = {0xfe, 0x7d, 0xee, 0x80, 0x8d, 0x7f, 0x3b, 0x88, 0x2a, 0x08, 0x2c, 0xbd, 0xc8, 0x39, 0x0d, 0x12,
                     0x9e, 0x5d, 0x28, 0xaf, 0x0e, 0x83, 0x22, 0xeb, 0x57, 0x3a, 0xda, 0x36, 0xf2, 0x1a, 0x5a, 0x89};
  uint8_t  sk_gnb_o[32];
  uint16_t scg_counter = 0;
  err_lte              = liblte_security_generate_sk_gnb(k_enb, sk_gnb_o, scg_counter);
  TESTASSERT(err_lte == LIBLTE_SUCCESS);
  arrprint(sk_gnb_o, sizeof(sk_gnb_o));
  uint8_t sk_gnb[] = {0x45, 0xcb, 0xc3, 0xf8, 0xa8, 0x11, 0x93, 0xfd, 0x5c, 0x52, 0x29, 0x30, 0x0d, 0x59, 0xed, 0xf8,
                      0x12, 0xe9, 0x98, 0xa1, 0x15, 0xec, 0x4e, 0x0c, 0xe9, 0x03, 0xba, 0x89, 0x36, 0x7e, 0x26, 0x28};
  err_cmp          = arrcmp(sk_gnb_o, sk_gnb, sizeof(sk_gnb));
  TESTASSERT(err_cmp == 0);
  return SRSRAN_SUCCESS;
}

int test_set_nr_rrc_up()
{
  LIBLTE_ERROR_ENUM err_lte = LIBLTE_ERROR_INVALID_INPUTS;
  int32             err_cmp = 0;

  uint8_t sk_gnb[] = {0x45, 0xcb, 0xc3, 0xf8, 0xa8, 0x11, 0x93, 0xfd, 0x5c, 0x52, 0x29, 0x30, 0x0d, 0x59, 0xed, 0xf8,
                      0x12, 0xe9, 0x98, 0xa1, 0x15, 0xec, 0x4e, 0x0c, 0xe9, 0x03, 0xba, 0x89, 0x36, 0x7e, 0x26, 0x28};

  uint8_t sk_gnb_o[32];
  uint8_t k_rrc_enc_o[32];
  uint8_t k_rrc_int_o[32];

  err_lte = liblte_security_generate_k_nr_rrc(sk_gnb,
                                              LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_128_EEA2,
                                              LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_EIA0,
                                              k_rrc_enc_o,
                                              k_rrc_int_o);

  TESTASSERT(err_lte == LIBLTE_SUCCESS);
  printf("RRC ENC output:\n");
  arrprint(&k_rrc_enc_o[0], sizeof(k_rrc_enc_o));
  uint8_t k_rrc_enc[] = {0x52, 0xa9, 0x95, 0xdf, 0xf8, 0x9b, 0xc2, 0x94, 0xbd, 0x89, 0xff,
                         0xb1, 0x37, 0xa2, 0x9f, 0x24, 0x66, 0xa0, 0x9e, 0x99, 0x23, 0x86,
                         0xc8, 0xd1, 0xdf, 0x78, 0x92, 0x96, 0x4c, 0x6f, 0xb5, 0x22};

  err_cmp = arrcmp(k_rrc_enc_o, k_rrc_enc, sizeof(k_rrc_enc_o));

  TESTASSERT(err_cmp == 0);

  uint8_t k_up_enc_o[32];
  uint8_t k_up_int_o[32];

  err_lte = liblte_security_generate_k_nr_up(sk_gnb,
                                             LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_128_EEA2,
                                             LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_EIA0,
                                             k_up_enc_o,
                                             k_up_int_o);

  uint8_t k_up_enc[] = {0x7c, 0xe2, 0x06, 0x70, 0xbb, 0xbc, 0xc5, 0x90, 0x40, 0x87, 0xc0, 0xd4, 0x26, 0x53, 0xc5, 0x40,
                        0x15, 0x20, 0x52, 0xd3, 0xdf, 0xbc, 0x3f, 0x05, 0x86, 0x9b, 0x7f, 0x92, 0x00, 0x95, 0xbe, 0x68};
  printf("UP ENC output:\n");
  arrprint(&k_up_enc_o[0], sizeof(k_up_enc_o));

  err_cmp = arrcmp(k_up_enc_o, k_up_enc, sizeof(k_up_enc_o));
  TESTASSERT(err_cmp == 0);
  return SRSRAN_SUCCESS;
}

/*
  Own test sets
*/

int main(int argc, char* argv[])
{

  TESTASSERT(test_set_2() == SRSRAN_SUCCESS);
  TESTASSERT(test_set_ksg() == SRSRAN_SUCCESS);
  TESTASSERT(test_set_nr_rrc_up() == SRSRAN_SUCCESS);
  return SRSRAN_SUCCESS;
}
