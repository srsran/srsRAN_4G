/*******************************************************************************

    Copyright 2014 Ben Wojtowicz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************

    File: liblte_security.cc

    Description: Contains all the implementations for the LTE security
                 algorithm library.

    Revision History
    ----------    -------------    --------------------------------------------
    08/03/2014    Ben Wojtowicz    Created file.
    09/03/2014    Ben Wojtowicz    Added key generation and EIA2 and fixed MCC
                                   and MNC packing.

*******************************************************************************/

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include "srslte/common/liblte_security.h"
#include "math.h"
#include "srslte/common/liblte_ssl.h"
#include "srslte/common/s3g.h"
#include "srslte/common/zuc.h"

/*******************************************************************************
                              LOCAL FUNCTION PROTOTYPES
*******************************************************************************/

/*********************************************************************
    Name: zero_tailing_bits

    Description: Fill tailing bits with zeros.

    Document Reference: -
*********************************************************************/
void zero_tailing_bits(uint8* data, uint32 length_bits);

/*******************************************************************************
                              FUNCTIONS
*******************************************************************************/

/*********************************************************************
    Name: liblte_security_generate_k_asme

    Description: Generate the security key Kasme.

    Document Reference: 33.401 v10.0.0 Annex A.2
*********************************************************************/
LIBLTE_ERROR_ENUM
liblte_security_generate_k_asme(uint8* ck, uint8* ik, uint8* ak, uint8* sqn, uint16 mcc, uint16 mnc, uint8* k_asme)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  uint32            i;
  uint8             s[14];
  uint8             key[32];

  if (ck != NULL && ik != NULL && ak != NULL && sqn != NULL && k_asme != NULL) {
    // Construct S
    s[0] = 0x10;                                   // FC
    s[1] = (mcc & 0x00F0) | ((mcc & 0x0F00) >> 8); // First byte of P0
    if ((mnc & 0xFF00) == 0xFF00) {
      // 2-digit MNC
      s[2] = 0xF0 | (mcc & 0x000F);                         // Second byte of P0
      s[3] = ((mnc & 0x000F) << 4) | ((mnc & 0x00F0) >> 4); // Third byte of P0
    } else {
      // 3-digit MNC
      s[2] = ((mnc & 0x000F) << 4) | (mcc & 0x000F);   // Second byte of P0
      s[3] = ((mnc & 0x00F0)) | ((mnc & 0x0F00) >> 8); // Third byte of P0
    }
    s[4] = 0x00; // First byte of L0
    s[5] = 0x03; // Second byte of L0
    for (i = 0; i < 6; i++) {
      s[6 + i] = sqn[i] ^ ak[i]; // P1
    }
    s[12] = 0x00; // First byte of L1
    s[13] = 0x06; // Second byte of L1

    // Construct Key
    for (i = 0; i < 16; i++) {
      key[i]      = ck[i];
      key[16 + i] = ik[i];
    }

    // Derive Kasme
    sha256(key, 32, s, 14, k_asme, 0);

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

/*********************************************************************
    Name: liblte_security_generate_k_enb

    Description: Generate the security key Kenb.

    Document Reference: 33.401 v10.0.0 Annex A.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_generate_k_enb(uint8* k_asme, uint32 nas_count, uint8* k_enb)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  uint8             s[7];

  if (k_asme != NULL && k_enb != NULL) {
    // Construct S
    s[0] = 0x11;                     // FC
    s[1] = (nas_count >> 24) & 0xFF; // First byte of P0
    s[2] = (nas_count >> 16) & 0xFF; // Second byte of P0
    s[3] = (nas_count >> 8) & 0xFF;  // Third byte of P0
    s[4] = nas_count & 0xFF;         // Fourth byte of P0
    s[5] = 0x00;                     // First byte of L0
    s[6] = 0x04;                     // Second byte of L0

    // Derive Kenb
    sha256(k_asme, 32, s, 7, k_enb, 0);

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

/*********************************************************************
    Name: liblte_security_generate_k_enb_star

    Description: Generate the security key Kenb*.

    Document Reference: 33.401 v10.0.0 Annex A.5
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_generate_k_enb_star(uint8* k_enb, uint32 pci, uint32_t earfcn, uint8* k_enb_star)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  uint8             s[9];

  if (k_enb_star != NULL && k_enb != NULL) {
    // Construct S
    s[0] = 0x13;                 // FC
    s[1] = (pci >> 8) & 0xFF;    // First byte of P0
    s[2] = pci & 0xFF;           // Second byte of P0
    s[3] = 0x00;                 // First byte of L0
    s[4] = 0x02;                 // Second byte of L0
    s[5] = (earfcn >> 8) & 0xFF; // First byte of P0
    s[6] = earfcn & 0xFF;        // Second byte of P0
    s[7] = 0x00;                 // First byte of L0
    s[8] = 0x02;                 // Second byte of L0

    // Derive Kenb
    sha256(k_enb, 32, s, 9, k_enb_star, 0);

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

LIBLTE_ERROR_ENUM liblte_security_generate_nh(uint8_t* k_asme, uint8_t* sync, uint8_t* nh)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  uint8             s[35];

  if (k_asme != NULL && sync != NULL && nh != NULL) {
    // Construct S
    s[0] = 0x12; // FC
    for (int i = 0; i < 32; i++) {
      s[1 + i] = sync[i];
    }
    s[33] = 0x00; // First byte of L0
    s[34] = 0x20, // Second byte of L0

        // Derive NH
        sha256(k_asme, 32, s, 35, nh, 0);

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

/*********************************************************************
    Name: liblte_security_generate_k_nas

    Description: Generate the NAS security keys KNASenc and KNASint.

    Document Reference: 33.401 v10.0.0 Annex A.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_generate_k_nas(uint8*                                      k_asme,
                                                 LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                                 LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                                 uint8*                                      k_nas_enc,
                                                 uint8*                                      k_nas_int)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  uint8             s[7];

  if (k_asme != NULL && k_nas_enc != NULL) { //{}
    if (enc_alg_id != LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_EEA0) {
      // Construct S for KNASenc
      s[0] = 0x15;       // FC
      s[1] = 0x01;       // P0
      s[2] = 0x00;       // First byte of L0
      s[3] = 0x01;       // Second byte of L0
      s[4] = enc_alg_id; // P1
      s[5] = 0x00;       // First byte of L1
      s[6] = 0x01;       // Second byte of L1

      // Derive KNASenc
      sha256(k_asme, 32, s, 7, k_nas_enc, 0);
    } else {
      memset(k_nas_enc, 0, 32);
    }
  }

  if (k_asme != NULL && k_nas_int != NULL) {
    // Construct S for KNASint
    s[0] = 0x15;       // FC
    s[1] = 0x02;       // P0
    s[2] = 0x00;       // First byte of L0
    s[3] = 0x01;       // Second byte of L0
    s[4] = int_alg_id; // P1
    s[5] = 0x00;       // First byte of L1
    s[6] = 0x01;       // Second byte of L1

    // Derive KNASint
    sha256(k_asme, 32, s, 7, k_nas_int, 0);

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

/*********************************************************************
    Name: liblte_security_generate_k_rrc

    Description: Generate the RRC security keys KRRCenc and KRRCint.

    Document Reference: 33.401 v10.0.0 Annex A.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_generate_k_rrc(uint8*                                      k_enb,
                                                 LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                                 LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                                 uint8*                                      k_rrc_enc,
                                                 uint8*                                      k_rrc_int)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  uint8             s[7];

  if (k_enb != NULL && k_rrc_enc != NULL && k_rrc_int != NULL) {
    // Construct S for KRRCenc
    s[0] = 0x15;       // FC
    s[1] = 0x03;       // P0
    s[2] = 0x00;       // First byte of L0
    s[3] = 0x01;       // Second byte of L0
    s[4] = enc_alg_id; // P1
    s[5] = 0x00;       // First byte of L1
    s[6] = 0x01;       // Second byte of L1

    // Derive KRRCenc
    sha256(k_enb, 32, s, 7, k_rrc_enc, 0);

    // Construct S for KRRCint
    s[0] = 0x15;       // FC
    s[1] = 0x04;       // P0
    s[2] = 0x00;       // First byte of L0
    s[3] = 0x01;       // Second byte of L0
    s[4] = int_alg_id; // P1
    s[5] = 0x00;       // First byte of L1
    s[6] = 0x01;       // Second byte of L1

    // Derive KRRCint
    sha256(k_enb, 32, s, 7, k_rrc_int, 0);

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

/*********************************************************************
    Name: liblte_security_generate_k_up

    Description: Generate the user plane security keys KUPenc and
                 KUPint.

    Document Reference: 33.401 v10.0.0 Annex A.2
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_generate_k_up(uint8*                                      k_enb,
                                                LIBLTE_SECURITY_CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                                LIBLTE_SECURITY_INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                                uint8*                                      k_up_enc,
                                                uint8*                                      k_up_int)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  uint8             s[7];

  if (k_enb != NULL && k_up_enc != NULL && k_up_int != NULL) {
    // Construct S for KUPenc
    s[0] = 0x15;       // FC
    s[1] = 0x05;       // P0
    s[2] = 0x00;       // First byte of L0
    s[3] = 0x01;       // Second byte of L0
    s[4] = enc_alg_id; // P1
    s[5] = 0x00;       // First byte of L1
    s[6] = 0x01;       // Second byte of L1

    // Derive KUPenc
    sha256(k_enb, 32, s, 7, k_up_enc, 0);

    // Construct S for KUPint
    s[0] = 0x15;       // FC
    s[1] = 0x06;       // P0
    s[2] = 0x00;       // First byte of L0
    s[3] = 0x01;       // Second byte of L0
    s[4] = int_alg_id; // P1
    s[5] = 0x00;       // First byte of L1
    s[6] = 0x01;       // Second byte of L1

    // Derive KUPint
    sha256(k_enb, 32, s, 7, k_up_int, 0);

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

LIBLTE_ERROR_ENUM liblte_security_128_eia1(const uint8* key,
                                           uint32       count,
                                           uint8        bearer,
                                           uint8        direction,
                                           uint8*       msg,
                                           uint32       msg_len,
                                           uint8*       mac)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (key != NULL && msg != NULL && mac != NULL) {
    uint32_t msg_len_bits = 0;
    uint32_t i            = 0;
    uint8_t* m_ptr;

    msg_len_bits = msg_len * 8;
    m_ptr        = s3g_f9(key, count, bearer << 27, direction, msg, msg_len_bits);
    for (i = 0; i < 4; i++) {
      mac[i] = m_ptr[i];
    }
    err = LIBLTE_SUCCESS;
  }
  return (err);
}

/*********************************************************************
    Name: liblte_security_128_eia2

    Description: 128-bit integrity algorithm EIA2.

    Document Reference: 33.401 v10.0.0 Annex B.2.3
                        33.102 v10.0.0 Section 6.5.4
                        RFC4493
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_128_eia2(const uint8* key,
                                           uint32       count,
                                           uint8        bearer,
                                           uint8        direction,
                                           uint8*       msg,
                                           uint32       msg_len,
                                           uint8*       mac)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  uint8             M[msg_len + 8 + 16];
  aes_context       ctx;
  uint32            i;
  uint32            j;
  uint32            n;
  uint32            pad_bits;
  uint8             const_zero[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint8             L[16];
  uint8             K1[16];
  uint8             K2[16];
  uint8             T[16];
  uint8             tmp[16];

  if (key != NULL && msg != NULL && mac != NULL) {
    // Subkey L generation
    aes_setkey_enc(&ctx, key, 128);
    aes_crypt_ecb(&ctx, AES_ENCRYPT, const_zero, L);

    // Subkey K1 generation
    for (i = 0; i < 15; i++) {
      K1[i] = (L[i] << 1) | ((L[i + 1] >> 7) & 0x01);
    }
    K1[15] = L[15] << 1;
    if (L[0] & 0x80) {
      K1[15] ^= 0x87;
    }

    // Subkey K2 generation
    for (i = 0; i < 15; i++) {
      K2[i] = (K1[i] << 1) | ((K1[i + 1] >> 7) & 0x01);
    }
    K2[15] = K1[15] << 1;
    if (K1[0] & 0x80) {
      K2[15] ^= 0x87;
    }

    // Construct M
    memset(M, 0, msg_len + 8 + 16);
    M[0] = (count >> 24) & 0xFF;
    M[1] = (count >> 16) & 0xFF;
    M[2] = (count >> 8) & 0xFF;
    M[3] = count & 0xFF;
    M[4] = (bearer << 3) | (direction << 2);
    for (i = 0; i < msg_len; i++) {
      M[8 + i] = msg[i];
    }

    // MAC generation
    n = (uint32)(ceilf((float)(msg_len + 8) / (float)(16)));
    for (i = 0; i < 16; i++) {
      T[i] = 0;
    }
    for (i = 0; i < n - 1; i++) {
      for (j = 0; j < 16; j++) {
        tmp[j] = T[j] ^ M[i * 16 + j];
      }
      aes_crypt_ecb(&ctx, AES_ENCRYPT, tmp, T);
    }
    pad_bits = ((msg_len * 8) + 64) % 128;
    if (pad_bits == 0) {
      for (j = 0; j < 16; j++) {
        tmp[j] = T[j] ^ K1[j] ^ M[i * 16 + j];
      }
      aes_crypt_ecb(&ctx, AES_ENCRYPT, tmp, T);
    } else {
      pad_bits = (128 - pad_bits) - 1;
      M[i * 16 + (15 - (pad_bits / 8))] |= 0x1 << (pad_bits % 8);
      for (j = 0; j < 16; j++) {
        tmp[j] = T[j] ^ K2[j] ^ M[i * 16 + j];
      }
      aes_crypt_ecb(&ctx, AES_ENCRYPT, tmp, T);
    }

    for (i = 0; i < 4; i++) {
      mac[i] = T[i];
    }

    err = LIBLTE_SUCCESS;
  }

  return (err);
}
LIBLTE_ERROR_ENUM liblte_security_128_eia2(const uint8*           key,
                                           uint32                 count,
                                           uint8                  bearer,
                                           uint8                  direction,
                                           LIBLTE_BIT_MSG_STRUCT* msg,
                                           uint8*                 mac)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  uint8             M[msg->N_bits * 8 + 8 + 16];
  aes_context       ctx;
  uint32            i;
  uint32            j;
  uint32            n;
  uint32            pad_bits;
  uint8             const_zero[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint8             L[16];
  uint8             K1[16];
  uint8             K2[16];
  uint8             T[16];
  uint8             tmp[16];

  if (key != NULL && msg != NULL && mac != NULL) {
    // Subkey L generation
    aes_setkey_enc(&ctx, key, 128);
    aes_crypt_ecb(&ctx, AES_ENCRYPT, const_zero, L);

    // Subkey K1 generation
    for (i = 0; i < 15; i++) {
      K1[i] = (L[i] << 1) | ((L[i + 1] >> 7) & 0x01);
    }
    K1[15] = L[15] << 1;
    if (L[0] & 0x80) {
      K1[15] ^= 0x87;
    }

    // Subkey K2 generation
    for (i = 0; i < 15; i++) {
      K2[i] = (K1[i] << 1) | ((K1[i + 1] >> 7) & 0x01);
    }
    K2[15] = K1[15] << 1;
    if (K1[0] & 0x80) {
      K2[15] ^= 0x87;
    }

    // Construct M
    memset(M, 0, msg->N_bits * 8 + 8 + 16);
    M[0] = (count >> 24) & 0xFF;
    M[1] = (count >> 16) & 0xFF;
    M[2] = (count >> 8) & 0xFF;
    M[3] = count & 0xFF;
    M[4] = (bearer << 3) | (direction << 2);
    for (i = 0; i < msg->N_bits / 8; i++) {
      M[8 + i] = 0;
      for (j = 0; j < 8; j++) {
        M[8 + i] |= msg->msg[i * 8 + j] << (7 - j);
      }
    }
    if ((msg->N_bits % 8) != 0) {
      M[8 + i] = 0;
      for (j = 0; j < msg->N_bits % 8; j++) {
        M[8 + i] |= msg->msg[i * 8 + j] << (7 - j);
      }
    }

    // MAC generation
    n = (uint32)(ceilf((float)(msg->N_bits + 64) / (float)(128)));
    for (i = 0; i < 16; i++) {
      T[i] = 0;
    }
    for (i = 0; i < n - 1; i++) {
      for (j = 0; j < 16; j++) {
        tmp[j] = T[j] ^ M[i * 16 + j];
      }
      aes_crypt_ecb(&ctx, AES_ENCRYPT, tmp, T);
    }
    pad_bits = (msg->N_bits + 64) % 128;
    if (pad_bits == 0) {
      for (j = 0; j < 16; j++) {
        tmp[j] = T[j] ^ K1[j] ^ M[i * 16 + j];
      }
      aes_crypt_ecb(&ctx, AES_ENCRYPT, tmp, T);
    } else {
      pad_bits = (128 - pad_bits) - 1;
      M[i * 16 + (15 - (pad_bits / 8))] |= 0x1 << (pad_bits % 8);
      for (j = 0; j < 16; j++) {
        tmp[j] = T[j] ^ K2[j] ^ M[i * 16 + j];
      }
      aes_crypt_ecb(&ctx, AES_ENCRYPT, tmp, T);
    }

    for (i = 0; i < 4; i++) {
      mac[i] = T[i];
    }

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

uint32_t GET_WORD(uint32_t* DATA, uint32_t i)
{
  uint32_t WORD, ti;
  ti = i % 32;
  if (ti == 0)
    WORD = DATA[i / 32];
  else
    WORD = (DATA[i / 32] << ti) | (DATA[i / 32 + 1] >> (32 - ti));
  return WORD;
}

uint8_t GET_BIT(uint8_t* DATA, uint32_t i)
{
  return (DATA[i / 8] & (1 << (7 - (i % 8)))) ? 1 : 0;
}

LIBLTE_ERROR_ENUM liblte_security_128_eia3(const uint8* key,
                                           uint32       count,
                                           uint8        bearer,
                                           uint8        direction,
                                           uint8*       msg,
                                           uint32       msg_len,
                                           uint8*       mac)

{
  LIBLTE_ERROR_ENUM err    = LIBLTE_ERROR_INVALID_INPUTS;
  uint8_t           iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  uint32* ks;
  uint32  msg_len_block_8, msg_len_block_32, m;

  if (key != NULL && msg != NULL && mac != NULL) {
    msg_len_block_8  = (msg_len + 7) / 8;
    msg_len_block_32 = (msg_len + 31) / 32;

    // Construct iv
    iv[0] = (count >> 24) & 0xFF;
    iv[1] = (count >> 16) & 0xFF;
    iv[2] = (count >> 8) & 0xFF;
    iv[3] = count & 0xFF;

    iv[4] = (bearer << 3) & 0xF8;
    iv[5] = iv[6] = iv[7] = 0;

    iv[8]  = ((count >> 24) & 0xFF) ^ ((direction & 1) << 7);
    iv[9]  = (count >> 16) & 0xFF;
    iv[10] = (count >> 8) & 0xFF;
    iv[11] = count & 0xFF;

    iv[12] = iv[4];
    iv[13] = iv[5];
    iv[14] = iv[6] ^ ((direction & 1) << 7);
    iv[15] = iv[7];

    zuc_state_t zuc_state;
    // Initialize keystream
    zuc_initialize(&zuc_state, key, iv);

    // Generate keystream
    int N = msg_len + 64;
    int L = (N + 31) / 32;

    ks = (uint32*)calloc(L, sizeof(uint32));

    zuc_generate_keystream(&zuc_state, L, ks);

    uint32_t T = 0;
    for (uint32_t i = 0; i < msg_len; i++) {
      if (GET_BIT(msg, i)) {
        T ^= GET_WORD(ks, i);
      }
    }

    T ^= GET_WORD(ks, msg_len);

    uint32_t mac_tmp = T ^ ks[L - 1];
    mac[0]           = (mac_tmp >> 24) & 0xFF;
    mac[1]           = (mac_tmp >> 16) & 0xFF;
    mac[2]           = (mac_tmp >> 8) & 0xFF;
    mac[3]           = mac_tmp & 0xFF;

    free(ks);
  }

  return (err);
}

/*********************************************************************
    Name: liblte_security_encryption_eea1

    Description: 128-bit encryption algorithm EEA1.

    Document Reference: 33.401 v13.1.0 Annex B.1.2
                        35.215 v13.0.0 References
                        Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D1 v2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_encryption_eea1(uint8* key,
                                                  uint32 count,
                                                  uint8  bearer,
                                                  uint8  direction,
                                                  uint8* msg,
                                                  uint32 msg_len,
                                                  uint8* out)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  S3G_STATE         state, *state_ptr;
  uint32            k[]  = {0, 0, 0, 0};
  uint32            iv[] = {0, 0, 0, 0};
  uint32*           ks;
  int32             i;
  uint32            msg_len_block_8, msg_len_block_32, m;

  if (key != NULL && msg != NULL && out != NULL) {
    state_ptr        = &state;
    msg_len_block_8  = (msg_len + 7) / 8;
    msg_len_block_32 = (msg_len + 31) / 32;

    // Transform key
    for (i = 3; i >= 0; i--) {
      k[i] = (key[4 * (3 - i) + 0] << 24) | (key[4 * (3 - i) + 1] << 16) | (key[4 * (3 - i) + 2] << 8) |
             (key[4 * (3 - i) + 3]);
    }

    // Construct iv
    iv[3] = count;
    iv[2] = ((bearer & 0x1F) << 27) | ((direction & 0x01) << 26);
    iv[1] = iv[3];
    iv[0] = iv[2];

    // Initialize keystream
    s3g_initialize(state_ptr, k, iv);

    // Generate keystream

    ks = (uint32*)calloc(msg_len_block_32, sizeof(uint32));
    s3g_generate_keystream(state_ptr, msg_len_block_32, ks);

    // Generate output except last block
    for (i = 0; i < (int32_t)msg_len_block_32 - 1; i++) {
      out[4 * i + 0] = msg[4 * i + 0] ^ ((ks[i] >> 24) & 0xFF);
      out[4 * i + 1] = msg[4 * i + 1] ^ ((ks[i] >> 16) & 0xFF);
      out[4 * i + 2] = msg[4 * i + 2] ^ ((ks[i] >> 8) & 0xFF);
      out[4 * i + 3] = msg[4 * i + 3] ^ ((ks[i] & 0xFF));
    }

    // Process last bytes
    for (i = (msg_len_block_32 - 1) * 4; i < (int32_t)msg_len_block_8; i++) {
      out[i] = msg[i] ^ ((ks[i / 4] >> ((3 - (i % 4)) * 8)) & 0xFF);
    }

    // Zero tailing bits
    zero_tailing_bits(out, msg_len);

    // Clean up
    free(ks);
    s3g_deinitialize(state_ptr);

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

/*********************************************************************
    Name: liblte_security_decryption_eea1

    Description: 128-bit decryption algorithm EEA1.

    Document Reference: 33.401 v13.1.0 Annex B.1.2
                        35.215 v13.0.0 References
                        Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D1 v2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_decryption_eea1(uint8* key,
                                                  uint32 count,
                                                  uint8  bearer,
                                                  uint8  direction,
                                                  uint8* ct,
                                                  uint32 ct_len,
                                                  uint8* out)
{
  return liblte_security_encryption_eea1(key, count, bearer, direction, ct, ct_len, out);
}

/*********************************************************************
    Name: liblte_security_encryption_eea2

    Description: 128-bit encryption algorithm EEA2.

    Document Reference: 33.401 v13.1.0 Annex B.1.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_encryption_eea2(uint8* key,
                                                  uint32 count,
                                                  uint8  bearer,
                                                  uint8  direction,
                                                  uint8* msg,
                                                  uint32 msg_len,
                                                  uint8* out)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  aes_context       ctx;
  unsigned char     stream_blk[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char     nonce_cnt[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int32             i;
  int               ret;
  size_t            nc_off = 0;

  if (key != NULL && msg != NULL && out != NULL) {
    ret = aes_setkey_enc(&ctx, key, 128);

    if (ret == 0) {
      // Construct nonce
      nonce_cnt[0] = (count >> 24) & 0xFF;
      nonce_cnt[1] = (count >> 16) & 0xFF;
      nonce_cnt[2] = (count >> 8) & 0xFF;
      nonce_cnt[3] = (count)&0xFF;
      nonce_cnt[4] = ((bearer & 0x1F) << 3) | ((direction & 0x01) << 2);

      // Encryption
      ret = aes_crypt_ctr(&ctx, (msg_len + 7) / 8, &nc_off, nonce_cnt, stream_blk, msg, out);
    }

    if (ret == 0) {
      // Zero tailing bits
      zero_tailing_bits(out, msg_len);
      err = LIBLTE_SUCCESS;
    }
  }

  return (err);
}

/*********************************************************************
    Name: liblte_security_decryption_eea2

    Description: 128-bit decryption algorithm EEA2.

    Document Reference: 33.401 v13.1.0 Annex B.1.3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_decryption_eea2(uint8* key,
                                                  uint32 count,
                                                  uint8  bearer,
                                                  uint8  direction,
                                                  uint8* ct,
                                                  uint32 ct_len,
                                                  uint8* out)
{
  return liblte_security_encryption_eea2(key, count, bearer, direction, ct, ct_len, out);
}

/*********************************************************************
    Name: liblte_security_encryption_eea1

    Description: 128-bit encryption algorithm EEA1.

    Document Reference: 33.401 v13.1.0 Annex B.1.2
                        35.215 v13.0.0 References
                        Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D1 v2.1
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_encryption_eea3(uint8* key,
                                                  uint32 count,
                                                  uint8  bearer,
                                                  uint8  direction,
                                                  uint8* msg,
                                                  uint32 msg_len,
                                                  uint8* out)
{
  LIBLTE_ERROR_ENUM err    = LIBLTE_ERROR_INVALID_INPUTS;
  uint8_t           iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  uint32* ks;
  int32   i;
  uint32  msg_len_block_8, msg_len_block_32, m;

  if (key != NULL && msg != NULL && out != NULL) {
    msg_len_block_8  = (msg_len + 7) / 8;
    msg_len_block_32 = (msg_len + 31) / 32;

    // Construct iv
    iv[0]  = (count >> 24) & 0xFF;
    iv[1]  = (count >> 16) & 0xFF;
    iv[2]  = (count >> 8) & 0xFF;
    iv[3]  = (count)&0xFF;
    iv[4]  = ((bearer & 0x1F) << 3) | ((direction & 0x01) << 2);
    iv[5]  = 0;
    iv[6]  = 0;
    iv[7]  = 0;
    iv[8]  = iv[0];
    iv[9]  = iv[1];
    iv[10] = iv[2];
    iv[11] = iv[3];
    iv[12] = iv[4];
    iv[13] = iv[5];
    iv[14] = iv[6];
    iv[15] = iv[7];

    zuc_state_t zuc_state;
    // Initialize keystream
    zuc_initialize(&zuc_state, key, iv);

    // Generate keystream

    ks = (uint32*)calloc(msg_len_block_32, sizeof(uint32));
    zuc_generate_keystream(&zuc_state, msg_len_block_32, ks);

    // Generate output except last block
    for (i = 0; i < (int32_t)msg_len_block_32 - 1; i++) {
      out[4 * i + 0] = msg[4 * i + 0] ^ ((ks[i] >> 24) & 0xFF);
      out[4 * i + 1] = msg[4 * i + 1] ^ ((ks[i] >> 16) & 0xFF);
      out[4 * i + 2] = msg[4 * i + 2] ^ ((ks[i] >> 8) & 0xFF);
      out[4 * i + 3] = msg[4 * i + 3] ^ ((ks[i] & 0xFF));
    }

    // Process last bytes
    for (i = (msg_len_block_32 - 1) * 4; i < (int32_t)msg_len_block_8; i++) {
      out[i] = msg[i] ^ ((ks[i / 4] >> ((3 - (i % 4)) * 8)) & 0xFF);
    }

    // Zero tailing bits
    zero_tailing_bits(out, msg_len);

    // Clean up
    free(ks);
    // zuc_deinitialize(state_ptr);

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

LIBLTE_ERROR_ENUM liblte_security_decryption_eea3(uint8* key,
                                                  uint32 count,
                                                  uint8  bearer,
                                                  uint8  direction,
                                                  uint8* msg,
                                                  uint32 msg_len,
                                                  uint8* out)
{
  return liblte_security_encryption_eea3(key, count, bearer, direction, msg, msg_len, out);
}

/*********************************************************************
    Name: liblte_security_milenage_f1

    Description: Milenage security function F1.  Computes network
                 authentication code MAC-A from key K, random
                 challenge RAND, sequence number SQN, and
                 authentication management field AMF.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_milenage_f1(uint8* k, uint8* op_c, uint8* rand, uint8* sqn, uint8* amf, uint8* mac_a)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  uint32            i;
  aes_context       ctx;
  uint8             temp[16];
  uint8             in1[16];
  uint8             out1[16];
  uint8             input[16];

  if (k != NULL && op_c != NULL && rand != NULL && sqn != NULL && amf != NULL && mac_a != NULL) {
    // Initialize the round keys
    aes_setkey_enc(&ctx, k, 128);

    // Compute temp
    for (i = 0; i < 16; i++) {
      input[i] = rand[i] ^ op_c[i];
    }
    aes_crypt_ecb(&ctx, AES_ENCRYPT, input, temp);

    // Construct in1
    for (i = 0; i < 6; i++) {
      in1[i]     = sqn[i];
      in1[i + 8] = sqn[i];
    }
    for (i = 0; i < 2; i++) {
      in1[i + 6]  = amf[i];
      in1[i + 14] = amf[i];
    }

    // Compute out1
    for (i = 0; i < 16; i++) {
      input[(i + 8) % 16] = in1[i] ^ op_c[i];
    }
    for (i = 0; i < 16; i++) {
      input[i] ^= temp[i];
    }
    aes_crypt_ecb(&ctx, AES_ENCRYPT, input, out1);
    for (i = 0; i < 16; i++) {
      out1[i] ^= op_c[i];
    }

    // Return MAC-A
    for (i = 0; i < 8; i++) {
      mac_a[i] = out1[i];
    }

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

/*********************************************************************
    Name: liblte_security_milenage_f1_star

    Description: Milenage security function F1*.  Computes resynch
                 authentication code MAC-S from key K, random
                 challenge RAND, sequence number SQN, and
                 authentication management field AMF.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
LIBLTE_ERROR_ENUM
liblte_security_milenage_f1_star(uint8* k, uint8* op_c, uint8* rand, uint8* sqn, uint8* amf, uint8* mac_s)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  aes_context       ctx;
  uint32            i;
  uint8             temp[16];
  uint8             in1[16];
  uint8             out1[16];
  uint8             input[16];

  if (k != NULL && op_c != NULL && rand != NULL && sqn != NULL && amf != NULL && mac_s != NULL) {
    // Initialize the round keys
    aes_setkey_enc(&ctx, k, 128);

    // Compute temp
    for (i = 0; i < 16; i++) {
      input[i] = rand[i] ^ op_c[i];
    }
    aes_crypt_ecb(&ctx, AES_ENCRYPT, input, temp);
    // Construct in1
    for (i = 0; i < 6; i++) {
      in1[i]     = sqn[i];
      in1[i + 8] = sqn[i];
    }
    for (i = 0; i < 2; i++) {
      in1[i + 6]  = amf[i];
      in1[i + 14] = amf[i];
    }

    // Compute out1
    for (i = 0; i < 16; i++) {
      input[(i + 8) % 16] = in1[i] ^ op_c[i];
    }
    for (i = 0; i < 16; i++) {
      input[i] ^= temp[i];
    }
    aes_crypt_ecb(&ctx, AES_ENCRYPT, input, out1);
    for (i = 0; i < 16; i++) {
      out1[i] ^= op_c[i];
    }

    // Return MAC-S
    for (i = 0; i < 8; i++) {
      mac_s[i] = out1[i + 8];
    }

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

/*********************************************************************
    Name: liblte_security_milenage_f2345

    Description: Milenage security functions F2, F3, F4, and F5.
                 Computes response RES, confidentiality key CK,
                 integrity key IK, and anonymity key AK from random
                 challenge RAND.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
LIBLTE_ERROR_ENUM
liblte_security_milenage_f2345(uint8* k, uint8* op_c, uint8* rand, uint8* res, uint8* ck, uint8* ik, uint8* ak)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  uint32            i;
  uint8             temp[16];
  uint8             out[16];
  uint8             input[16];
  aes_context       ctx;

  if (k != NULL && op_c != NULL && rand != NULL && res != NULL && ck != NULL && ik != NULL && ak != NULL) {
    // Initialize the round keys
    aes_setkey_enc(&ctx, k, 128);
    // Compute temp
    for (i = 0; i < 16; i++) {
      input[i] = rand[i] ^ op_c[i];
    }
    mbedtls_aes_crypt_ecb(&ctx, AES_ENCRYPT, input, temp);
    // Compute out for RES and AK
    for (i = 0; i < 16; i++) {
      input[i] = temp[i] ^ op_c[i];
    }
    input[15] ^= 1;
    mbedtls_aes_crypt_ecb(&ctx, AES_ENCRYPT, input, out);
    for (i = 0; i < 16; i++) {
      out[i] ^= op_c[i];
    }

    // Return RES
    for (i = 0; i < 8; i++) {
      res[i] = out[i + 8];
    }

    // Return AK
    for (i = 0; i < 6; i++) {
      ak[i] = out[i];
    }

    // Compute out for CK
    for (i = 0; i < 16; i++) {
      input[(i + 12) % 16] = temp[i] ^ op_c[i];
    }
    input[15] ^= 2;
    mbedtls_aes_crypt_ecb(&ctx, AES_ENCRYPT, input, out);
    for (i = 0; i < 16; i++) {
      out[i] ^= op_c[i];
    }

    // Return CK
    for (i = 0; i < 16; i++) {
      ck[i] = out[i];
    }

    // Compute out for IK
    for (i = 0; i < 16; i++) {
      input[(i + 8) % 16] = temp[i] ^ op_c[i];
    }
    input[15] ^= 4;
    mbedtls_aes_crypt_ecb(&ctx, AES_ENCRYPT, input, out);
    for (i = 0; i < 16; i++) {
      out[i] ^= op_c[i];
    }

    // Return IK
    for (i = 0; i < 16; i++) {
      ik[i] = out[i];
    }

    err = LIBLTE_SUCCESS;
  }

  return (err);
}

/*********************************************************************
    Name: liblte_security_milenage_f5_star

    Description: Milenage security function F5*.  Computes resynch
                 anonymity key AK from key K and random challenge
                 RAND.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_milenage_f5_star(uint8* k, uint8* op_c, uint8* rand, uint8* ak)
{
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;
  aes_context       ctx;
  uint32            i;
  uint8             temp[16];
  uint8             out[16];
  uint8             input[16];

  if (k != NULL && op_c != NULL && rand != NULL && ak != NULL) {
    // Initialize the round keys
    aes_setkey_enc(&ctx, k, 128);

    // Compute temp
    for (i = 0; i < 16; i++) {
      input[i] = rand[i] ^ op_c[i];
    }
    aes_crypt_ecb(&ctx, AES_ENCRYPT, input, temp);
    // Compute out
    for (i = 0; i < 16; i++) {
      input[(i + 4) % 16] = temp[i] ^ op_c[i];
    }
    input[15] ^= 8;
    aes_crypt_ecb(&ctx, AES_ENCRYPT, input, out);
    for (i = 0; i < 16; i++) {
      out[i] ^= op_c[i];
    }

    // Return AK
    for (i = 0; i < 6; i++) {
      ak[i] = out[i];
    }
    err = LIBLTE_SUCCESS;
  }
  return (err);
}

/*********************************************************************
    Name: liblte_compute_opc

    Description: Computes OPc from OP and K.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/

LIBLTE_ERROR_ENUM liblte_compute_opc(uint8* k, uint8* op, uint8* op_c)
{
  uint32            i;
  aes_context       ctx;
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (k != NULL && op != NULL && op_c != NULL) {
    aes_setkey_enc(&ctx, k, 128);
    aes_crypt_ecb(&ctx, AES_ENCRYPT, op, op_c);
    for (i = 0; i < 16; i++) {
      op_c[i] ^= op[i];
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*********************************************************************
    Name: zero_tailing_bits

    Description: Fill tailing bits with zeros.

    Document Reference: -
*********************************************************************/
void zero_tailing_bits(uint8* data, uint32 length_bits)
{
  uint8 bits = (8 - (length_bits & 0x07)) & 0x07;
  data[(length_bits + 7) / 8 - 1] &= (uint8)(0xFF << bits);
}
