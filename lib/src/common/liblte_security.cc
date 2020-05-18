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
#include "srslte/common/zuc.h"
#include "srslte/common/s3g.h"
/*******************************************************************************
                              DEFINES
*******************************************************************************/

/*******************************************************************************
                              TYPEDEFS
*******************************************************************************/

typedef struct {
  uint8 rk[11][4][4];
} ROUND_KEY_STRUCT;

typedef struct {
  uint8 state[4][4];
} STATE_STRUCT;



/*******************************************************************************
                              GLOBAL VARIABLES
*******************************************************************************/



static const uint8_t S[256] = {
    99,  124, 119, 123, 242, 107, 111, 197, 48,  1,   103, 43,  254, 215, 171, 118, 202, 130, 201, 125, 250, 89,
    71,  240, 173, 212, 162, 175, 156, 164, 114, 192, 183, 253, 147, 38,  54,  63,  247, 204, 52,  165, 229, 241,
    113, 216, 49,  21,  4,   199, 35,  195, 24,  150, 5,   154, 7,   18,  128, 226, 235, 39,  178, 117, 9,   131,
    44,  26,  27,  110, 90,  160, 82,  59,  214, 179, 41,  227, 47,  132, 83,  209, 0,   237, 32,  252, 177, 91,
    106, 203, 190, 57,  74,  76,  88,  207, 208, 239, 170, 251, 67,  77,  51,  133, 69,  249, 2,   127, 80,  60,
    159, 168, 81,  163, 64,  143, 146, 157, 56,  245, 188, 182, 218, 33,  16,  255, 243, 210, 205, 12,  19,  236,
    95,  151, 68,  23,  196, 167, 126, 61,  100, 93,  25,  115, 96,  129, 79,  220, 34,  42,  144, 136, 70,  238,
    184, 20,  222, 94,  11,  219, 224, 50,  58,  10,  73,  6,   36,  92,  194, 211, 172, 98,  145, 149, 228, 121,
    231, 200, 55,  109, 141, 213, 78,  169, 108, 86,  244, 234, 101, 122, 174, 8,   186, 120, 37,  46,  28,  166,
    180, 198, 232, 221, 116, 31,  75,  189, 139, 138, 112, 62,  181, 102, 72,  3,   246, 14,  97,  53,  87,  185,
    134, 193, 29,  158, 225, 248, 152, 17,  105, 217, 142, 148, 155, 30,  135, 233, 206, 85,  40,  223, 140, 161,
    137, 13,  191, 230, 66,  104, 65,  153, 45,  15,  176, 84,  187, 22};

static const uint8 X_TIME[256] = {
    0,   2,   4,   6,   8,   10,  12,  14,  16,  18,  20,  22,  24,  26,  28,  30,  32,  34,  36,  38,  40,  42,
    44,  46,  48,  50,  52,  54,  56,  58,  60,  62,  64,  66,  68,  70,  72,  74,  76,  78,  80,  82,  84,  86,
    88,  90,  92,  94,  96,  98,  100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130,
    132, 134, 136, 138, 140, 142, 144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174,
    176, 178, 180, 182, 184, 186, 188, 190, 192, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218,
    220, 222, 224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 252, 254, 27,  25,  31,  29,
    19,  17,  23,  21,  11,  9,   15,  13,  3,   1,   7,   5,   59,  57,  63,  61,  51,  49,  55,  53,  43,  41,
    47,  45,  35,  33,  39,  37,  91,  89,  95,  93,  83,  81,  87,  85,  75,  73,  79,  77,  67,  65,  71,  69,
    123, 121, 127, 125, 115, 113, 119, 117, 107, 105, 111, 109, 99,  97,  103, 101, 155, 153, 159, 157, 147, 145,
    151, 149, 139, 137, 143, 141, 131, 129, 135, 133, 187, 185, 191, 189, 179, 177, 183, 181, 171, 169, 175, 173,
    163, 161, 167, 165, 219, 217, 223, 221, 211, 209, 215, 213, 203, 201, 207, 205, 195, 193, 199, 197, 251, 249,
    255, 253, 243, 241, 247, 245, 235, 233, 239, 237, 227, 225, 231, 229};

/*******************************************************************************
                              LOCAL FUNCTION PROTOTYPES
*******************************************************************************/

/*********************************************************************
    Name: compute_OPc

    Description: Computes OPc from OP and K.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
void compute_OPc(ROUND_KEY_STRUCT* rk, uint8* op, uint8* op_c);

/*********************************************************************
    Name: rijndael_key_schedule

    Description: Computes all Rijndael's internal subkeys from key.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
void rijndael_key_schedule(uint8* key, ROUND_KEY_STRUCT* rk);

/*********************************************************************
    Name: rijndael_encrypt

    Description: Computes output using input and round keys.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
void rijndael_encrypt(uint8* input, ROUND_KEY_STRUCT* rk, uint8* output);

/*********************************************************************
    Name: key_add

    Description: Round key addition function.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
void key_add(STATE_STRUCT* state, ROUND_KEY_STRUCT* rk, uint32 round);

/*********************************************************************
    Name: byte_sub

    Description: Byte substitution transformation.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
void byte_sub(STATE_STRUCT* state);

/*********************************************************************
    Name: shift_row

    Description: Row shift transformation.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
void shift_row(STATE_STRUCT* state);

/*********************************************************************
    Name: mix_column

    Description: Mix column transformation.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
// Defines
// Enums
// Structs
// Functions
void mix_column(STATE_STRUCT* state);

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

  if (k_asme != NULL && k_nas_enc != NULL && k_nas_int != NULL) {
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

/*********************************************************************
    Name: liblte_security_128_eia2

    Description: 128-bit integrity algorithm EIA2.

    Document Reference: 33.401 v10.0.0 Annex B.2.3
                        33.102 v10.0.0 Section 6.5.4
                        RFC4493
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_security_128_eia2(uint8* key,
                                           uint32 count,
                                           uint8  bearer,
                                           uint8  direction,
                                           uint8* msg,
                                           uint32 msg_len,
                                           uint8* mac)
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
LIBLTE_ERROR_ENUM liblte_security_128_eia2(uint8*                 key,
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

LIBLTE_ERROR_ENUM liblte_security_128_eia3(uint8* key,
                                           uint32 count,
                                           uint8  bearer,
                                           uint8  direction,
                                           uint8* msg,
                                           uint32 msg_len,
                                           uint8* mac)

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
  ROUND_KEY_STRUCT  round_keys;
  uint32            i;
  uint8             temp[16];
  uint8             in1[16];
  uint8             out1[16];
  uint8             rijndael_input[16];

  if (k != NULL && op_c != NULL && rand != NULL && sqn != NULL && amf != NULL && mac_a != NULL) {
    // Initialize the round keys
    rijndael_key_schedule(k, &round_keys);

    // Compute temp
    for (i = 0; i < 16; i++) {
      rijndael_input[i] = rand[i] ^ op_c[i];
    }
    rijndael_encrypt(rijndael_input, &round_keys, temp);

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
      rijndael_input[(i + 8) % 16] = in1[i] ^ op_c[i];
    }
    for (i = 0; i < 16; i++) {
      rijndael_input[i] ^= temp[i];
    }
    rijndael_encrypt(rijndael_input, &round_keys, out1);
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
  ROUND_KEY_STRUCT  round_keys;
  uint32            i;
  uint8             temp[16];
  uint8             in1[16];
  uint8             out1[16];
  uint8             rijndael_input[16];

  if (k != NULL && op_c != NULL && rand != NULL && sqn != NULL && amf != NULL && mac_s != NULL) {
    // Initialize the round keys
    rijndael_key_schedule(k, &round_keys);

    // Compute temp
    for (i = 0; i < 16; i++) {
      rijndael_input[i] = rand[i] ^ op_c[i];
    }
    rijndael_encrypt(rijndael_input, &round_keys, temp);

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
      rijndael_input[(i + 8) % 16] = in1[i] ^ op_c[i];
    }
    for (i = 0; i < 16; i++) {
      rijndael_input[i] ^= temp[i];
    }
    rijndael_encrypt(rijndael_input, &round_keys, out1);
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
  ROUND_KEY_STRUCT  round_keys;
  uint32            i;
  uint8             temp[16];
  uint8             out[16];
  uint8             rijndael_input[16];

  if (k != NULL && op_c != NULL && rand != NULL && res != NULL && ck != NULL && ik != NULL && ak != NULL) {
    // Initialize the round keys
    rijndael_key_schedule(k, &round_keys);

    // Compute temp
    for (i = 0; i < 16; i++) {
      rijndael_input[i] = rand[i] ^ op_c[i];
    }
    rijndael_encrypt(rijndael_input, &round_keys, temp);

    // Compute out for RES and AK
    for (i = 0; i < 16; i++) {
      rijndael_input[i] = temp[i] ^ op_c[i];
    }
    rijndael_input[15] ^= 1;
    rijndael_encrypt(rijndael_input, &round_keys, out);
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
      rijndael_input[(i + 12) % 16] = temp[i] ^ op_c[i];
    }
    rijndael_input[15] ^= 2;
    rijndael_encrypt(rijndael_input, &round_keys, out);
    for (i = 0; i < 16; i++) {
      out[i] ^= op_c[i];
    }

    // Return CK
    for (i = 0; i < 16; i++) {
      ck[i] = out[i];
    }

    // Compute out for IK
    for (i = 0; i < 16; i++) {
      rijndael_input[(i + 8) % 16] = temp[i] ^ op_c[i];
    }
    rijndael_input[15] ^= 4;
    rijndael_encrypt(rijndael_input, &round_keys, out);
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
  ROUND_KEY_STRUCT  round_keys;
  uint32            i;
  uint8             temp[16];
  uint8             out[16];
  uint8             rijndael_input[16];

  if (k != NULL && op_c != NULL && rand != NULL && ak != NULL) {
    // Initialize the round keys
    rijndael_key_schedule(k, &round_keys);

    // Compute temp
    for (i = 0; i < 16; i++) {
      rijndael_input[i] = rand[i] ^ op_c[i];
    }
    rijndael_encrypt(rijndael_input, &round_keys, temp);

    // Compute out
    for (i = 0; i < 16; i++) {
      rijndael_input[(i + 4) % 16] = temp[i] ^ op_c[i];
    }
    rijndael_input[15] ^= 8;
    rijndael_encrypt(rijndael_input, &round_keys, out);
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
  ROUND_KEY_STRUCT  round_keys;
  LIBLTE_ERROR_ENUM err = LIBLTE_ERROR_INVALID_INPUTS;

  if (k != NULL && op != NULL && op_c != NULL) {

    rijndael_key_schedule(k, &round_keys);
    rijndael_encrypt(op, &round_keys, op_c);
    for (i = 0; i < 16; i++) {
      op_c[i] ^= op[i];
    }
    err = LIBLTE_SUCCESS;
  }
  return err;
}

/*******************************************************************************
                              LOCAL FUNCTIONS
*******************************************************************************/

/*********************************************************************
    Name: rijndael_key_schedule

    Description: Computes all Rijndael's internal subkeys from key.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
void rijndael_key_schedule(uint8* key, ROUND_KEY_STRUCT* rk)
{
  uint32 i;
  uint32 j;
  uint8  round_const;

  // Set first round key to key
  for (i = 0; i < 16; i++) {
    rk->rk[0][i & 0x03][i >> 2] = key[i];
  }

  round_const = 1;

  // Compute the remaining round keys
  for (i = 1; i < 11; i++) {
    rk->rk[i][0][0] = S[rk->rk[i - 1][1][3]] ^ rk->rk[i - 1][0][0] ^ round_const;
    rk->rk[i][1][0] = S[rk->rk[i - 1][2][3]] ^ rk->rk[i - 1][1][0];
    rk->rk[i][2][0] = S[rk->rk[i - 1][3][3]] ^ rk->rk[i - 1][2][0];
    rk->rk[i][3][0] = S[rk->rk[i - 1][0][3]] ^ rk->rk[i - 1][3][0];

    for (j = 0; j < 4; j++) {
      rk->rk[i][j][1] = rk->rk[i - 1][j][1] ^ rk->rk[i][j][0];
      rk->rk[i][j][2] = rk->rk[i - 1][j][2] ^ rk->rk[i][j][1];
      rk->rk[i][j][3] = rk->rk[i - 1][j][3] ^ rk->rk[i][j][2];
    }

    round_const = X_TIME[round_const];
  }
}

/*********************************************************************
    Name: rijndael_encrypt

    Description: Computes output using input and round keys.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
void rijndael_encrypt(uint8* input, ROUND_KEY_STRUCT* rk, uint8* output)
{
  STATE_STRUCT state;
  uint32       i;
  uint32       r;

  // Initialize and perform round 0
  for (i = 0; i < 16; i++) {
    state.state[i & 0x03][i >> 2] = input[i];
  }
  key_add(&state, rk, 0);

  // Perform rounds 1 through 9
  for (r = 1; r <= 9; r++) {
    byte_sub(&state);
    shift_row(&state);
    mix_column(&state);
    key_add(&state, rk, r);
  }

  // Perform round 10
  byte_sub(&state);
  shift_row(&state);
  key_add(&state, rk, r);

  // Return output
  for (i = 0; i < 16; i++) {
    output[i] = state.state[i & 0x03][i >> 2];
  }
}

/*********************************************************************
    Name: key_add

    Description: Round key addition function.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
void key_add(STATE_STRUCT* state, ROUND_KEY_STRUCT* rk, uint32 round)
{
  uint32 i;
  uint32 j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      state->state[i][j] ^= rk->rk[round][i][j];
    }
  }
}

/*********************************************************************
    Name: byte_sub

    Description: Byte substitution transformation.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
void byte_sub(STATE_STRUCT* state)
{
  uint32 i;
  uint32 j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      state->state[i][j] = S[state->state[i][j]];
    }
  }
}

/*********************************************************************
    Name: shift_row

    Description: Row shift transformation.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
void shift_row(STATE_STRUCT* state)
{
  uint8 temp;

  // Left rotate row 1 by 1
  temp               = state->state[1][0];
  state->state[1][0] = state->state[1][1];
  state->state[1][1] = state->state[1][2];
  state->state[1][2] = state->state[1][3];
  state->state[1][3] = temp;

  // Left rotate row 2 by 2
  temp               = state->state[2][0];
  state->state[2][0] = state->state[2][2];
  state->state[2][2] = temp;
  temp               = state->state[2][1];
  state->state[2][1] = state->state[2][3];
  state->state[2][3] = temp;

  // Left rotate row 3 by 3
  temp               = state->state[3][0];
  state->state[3][0] = state->state[3][3];
  state->state[3][3] = state->state[3][2];
  state->state[3][2] = state->state[3][1];
  state->state[3][1] = temp;
}

/*********************************************************************
    Name: mix_column

    Description: Mix column transformation.

    Document Reference: 35.206 v10.0.0 Annex 3
*********************************************************************/
void mix_column(STATE_STRUCT* state)
{
  uint32 i;
  uint8  temp;
  uint8  tmp0;
  uint8  tmp;

  for (i = 0; i < 4; i++) {
    temp = state->state[0][i] ^ state->state[1][i] ^ state->state[2][i] ^ state->state[3][i];
    tmp0 = state->state[0][i];

    tmp = X_TIME[state->state[0][i] ^ state->state[1][i]];
    state->state[0][i] ^= temp ^ tmp;

    tmp = X_TIME[state->state[1][i] ^ state->state[2][i]];
    state->state[1][i] ^= temp ^ tmp;

    tmp = X_TIME[state->state[2][i] ^ state->state[3][i]];
    state->state[2][i] ^= temp ^ tmp;

    tmp = X_TIME[state->state[3][i] ^ tmp0];
    state->state[3][i] ^= temp ^ tmp;
  }
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
