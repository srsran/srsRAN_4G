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

typedef struct {
  uint32* lfsr;
  uint32* fsm;
} S3G_STATE;

/*******************************************************************************
                              GLOBAL VARIABLES
*******************************************************************************/

static const uint8 S[256] = {
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

/* S-box SQ */
static const uint8 SQ[256] = {
    0x25, 0x24, 0x73, 0x67, 0xD7, 0xAE, 0x5C, 0x30, 0xA4, 0xEE, 0x6E, 0xCB, 0x7D, 0xB5, 0x82, 0xDB, 0xE4, 0x8E, 0x48,
    0x49, 0x4F, 0x5D, 0x6A, 0x78, 0x70, 0x88, 0xE8, 0x5F, 0x5E, 0x84, 0x65, 0xE2, 0xD8, 0xE9, 0xCC, 0xED, 0x40, 0x2F,
    0x11, 0x28, 0x57, 0xD2, 0xAC, 0xE3, 0x4A, 0x15, 0x1B, 0xB9, 0xB2, 0x80, 0x85, 0xA6, 0x2E, 0x02, 0x47, 0x29, 0x07,
    0x4B, 0x0E, 0xC1, 0x51, 0xAA, 0x89, 0xD4, 0xCA, 0x01, 0x46, 0xB3, 0xEF, 0xDD, 0x44, 0x7B, 0xC2, 0x7F, 0xBE, 0xC3,
    0x9F, 0x20, 0x4C, 0x64, 0x83, 0xA2, 0x68, 0x42, 0x13, 0xB4, 0x41, 0xCD, 0xBA, 0xC6, 0xBB, 0x6D, 0x4D, 0x71, 0x21,
    0xF4, 0x8D, 0xB0, 0xE5, 0x93, 0xFE, 0x8F, 0xE6, 0xCF, 0x43, 0x45, 0x31, 0x22, 0x37, 0x36, 0x96, 0xFA, 0xBC, 0x0F,
    0x08, 0x52, 0x1D, 0x55, 0x1A, 0xC5, 0x4E, 0x23, 0x69, 0x7A, 0x92, 0xFF, 0x5B, 0x5A, 0xEB, 0x9A, 0x1C, 0xA9, 0xD1,
    0x7E, 0x0D, 0xFC, 0x50, 0x8A, 0xB6, 0x62, 0xF5, 0x0A, 0xF8, 0xDC, 0x03, 0x3C, 0x0C, 0x39, 0xF1, 0xB8, 0xF3, 0x3D,
    0xF2, 0xD5, 0x97, 0x66, 0x81, 0x32, 0xA0, 0x00, 0x06, 0xCE, 0xF6, 0xEA, 0xB7, 0x17, 0xF7, 0x8C, 0x79, 0xD6, 0xA7,
    0xBF, 0x8B, 0x3F, 0x1F, 0x53, 0x63, 0x75, 0x35, 0x2C, 0x60, 0xFD, 0x27, 0xD3, 0x94, 0xA5, 0x7C, 0xA1, 0x05, 0x58,
    0x2D, 0xBD, 0xD9, 0xC7, 0xAF, 0x6B, 0x54, 0x0B, 0xE0, 0x38, 0x04, 0xC8, 0x9D, 0xE7, 0x14, 0xB1, 0x87, 0x9C, 0xDF,
    0x6F, 0xF9, 0xDA, 0x2A, 0xC4, 0x59, 0x16, 0x74, 0x91, 0xAB, 0x26, 0x61, 0x76, 0x34, 0x2B, 0xAD, 0x99, 0xFB, 0x72,
    0xEC, 0x33, 0x12, 0xDE, 0x98, 0x3B, 0xC0, 0x9B, 0x3E, 0x18, 0x10, 0x3A, 0x56, 0xE1, 0x77, 0xC9, 0x1E, 0x9E, 0x95,
    0xA3, 0x90, 0x19, 0xA8, 0x6C, 0x09, 0xD0, 0xF0, 0x86};

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

/*********************************************************************
    Name: s3g_mul_x

    Description: Multiplication with reduction.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.1.1
*********************************************************************/
uint8 s3g_mul_x(uint8 v, uint8 c);

/*********************************************************************
    Name: s3g_mul_x_pow

    Description: Recursive multiplication with reduction.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.1.2
*********************************************************************/
uint8 s3g_mul_x_pow(uint8 v, uint8 i, uint8 c);

/*********************************************************************
    Name: s3g_mul_alpha

    Description: Multiplication with alpha.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.2
*********************************************************************/
uint32 s3g_mul_alpha(uint8 c);

/*********************************************************************
    Name: s3g_div_alpha

    Description: Division by alpha.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.3
*********************************************************************/
uint32 s3g_div_alpha(uint8 c);

/*********************************************************************
    Name: s3g_s1

    Description: S-Box S1.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.3.1
*********************************************************************/
uint32 s3g_s1(uint32 w);

/*********************************************************************
    Name: s3g_s2

    Description: S-Box S2.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.3.2
*********************************************************************/
uint32 s3g_s2(uint32 w);

/*********************************************************************
    Name: s3g_clock_lfsr

    Description: Clocking LFSR.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.4 and Section 3.4.5
*********************************************************************/
void s3g_clock_lfsr(S3G_STATE* state, uint32 f);

/*********************************************************************
    Name: s3g_clock_fsm

    Description: Clocking FSM.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.6
*********************************************************************/
uint32 s3g_clock_fsm(S3G_STATE* state);

/*********************************************************************
    Name: s3g_initialize

    Description: Initialization.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 4.1
*********************************************************************/
void s3g_initialize(S3G_STATE* state, uint32 k[4], uint32 iv[4]);

/*********************************************************************
    Name: s3g_deinitialize

    Description: Deinitialization.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
*********************************************************************/
void s3g_deinitialize(S3G_STATE* state);

/*********************************************************************
    Name: s3g_generate_keystream

    Description: Generation of Keystream.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 4.2
*********************************************************************/
void s3g_generate_keystream(S3G_STATE* state, uint32 n, uint32* ks);

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

u32 GET_WORD(u32* DATA, u32 i)
{
  u32 WORD, ti;
  ti = i % 32;
  if (ti == 0)
    WORD = DATA[i / 32];
  else
    WORD = (DATA[i / 32] << ti) | (DATA[i / 32 + 1] >> (32 - ti));
  return WORD;
}

u8 GET_BIT(uint8_t* DATA, u32 i)
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

/*********************************************************************
    Name: s3g_mul_x

    Description: Multiplication with reduction.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.1.1
*********************************************************************/
uint8 s3g_mul_x(uint8 v, uint8 c)
{
  if (v & 0x80)
    return ((v << 1) ^ c);
  else
    return (v << 1);
}

/*********************************************************************
    Name: s3g_mul_x_pow

    Description: Recursive multiplication with reduction.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.1.2
*********************************************************************/
uint8 s3g_mul_x_pow(uint8 v, uint8 i, uint8 c)
{
  if (i == 0)
    return v;
  else
    return s3g_mul_x(s3g_mul_x_pow(v, i - 1, c), c);
}

/*********************************************************************
    Name: s3g_mul_alpha

    Description: Multiplication with alpha.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.2
*********************************************************************/
uint32 s3g_mul_alpha(uint8 c)
{
  return ((((uint32)s3g_mul_x_pow(c, 23, 0xa9)) << 24) | (((uint32)s3g_mul_x_pow(c, 245, 0xa9)) << 16) |
          (((uint32)s3g_mul_x_pow(c, 48, 0xa9)) << 8) | (((uint32)s3g_mul_x_pow(c, 239, 0xa9))));
}

/*********************************************************************
    Name: s3g_div_alpha

    Description: Division by alpha.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.3
*********************************************************************/
uint32 s3g_div_alpha(uint8 c)
{
  return ((((uint32)s3g_mul_x_pow(c, 16, 0xa9)) << 24) | (((uint32)s3g_mul_x_pow(c, 39, 0xa9)) << 16) |
          (((uint32)s3g_mul_x_pow(c, 6, 0xa9)) << 8) | (((uint32)s3g_mul_x_pow(c, 64, 0xa9))));
}

/*********************************************************************
    Name: s3g_s1

    Description: S-Box S1.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.3.1
*********************************************************************/
uint32 s3g_s1(uint32 w)
{
  uint8 r0 = 0, r1 = 0, r2 = 0, r3 = 0;
  uint8 srw0 = S[(uint8)((w >> 24) & 0xff)];
  uint8 srw1 = S[(uint8)((w >> 16) & 0xff)];
  uint8 srw2 = S[(uint8)((w >> 8) & 0xff)];
  uint8 srw3 = S[(uint8)((w)&0xff)];

  r0 = ((s3g_mul_x(srw0, 0x1b)) ^ (srw1) ^ (srw2) ^ ((s3g_mul_x(srw3, 0x1b)) ^ srw3));

  r1 = (((s3g_mul_x(srw0, 0x1b)) ^ srw0) ^ (s3g_mul_x(srw1, 0x1b)) ^ (srw2) ^ (srw3));

  r2 = ((srw0) ^ ((s3g_mul_x(srw1, 0x1b)) ^ srw1) ^ (s3g_mul_x(srw2, 0x1b)) ^ (srw3));

  r3 = ((srw0) ^ (srw1) ^ ((s3g_mul_x(srw2, 0x1b)) ^ srw2) ^ (s3g_mul_x(srw3, 0x1b)));

  return ((((uint32)r0) << 24) | (((uint32)r1) << 16) | (((uint32)r2) << 8) | (((uint32)r3)));
}

/*********************************************************************
    Name: s3g_s2

    Description: S-Box S2.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.3.2
*********************************************************************/
uint32 s3g_s2(uint32 w)
{
  uint8 r0 = 0, r1 = 0, r2 = 0, r3 = 0;
  uint8 sqw0 = SQ[(uint8)((w >> 24) & 0xff)];
  uint8 sqw1 = SQ[(uint8)((w >> 16) & 0xff)];
  uint8 sqw2 = SQ[(uint8)((w >> 8) & 0xff)];
  uint8 sqw3 = SQ[(uint8)((w)&0xff)];

  r0 = ((s3g_mul_x(sqw0, 0x69)) ^ (sqw1) ^ (sqw2) ^ ((s3g_mul_x(sqw3, 0x69)) ^ sqw3));

  r1 = (((s3g_mul_x(sqw0, 0x69)) ^ sqw0) ^ (s3g_mul_x(sqw1, 0x69)) ^ (sqw2) ^ (sqw3));

  r2 = ((sqw0) ^ ((s3g_mul_x(sqw1, 0x69)) ^ sqw1) ^ (s3g_mul_x(sqw2, 0x69)) ^ (sqw3));

  r3 = ((sqw0) ^ (sqw1) ^ ((s3g_mul_x(sqw2, 0x69)) ^ sqw2) ^ (s3g_mul_x(sqw3, 0x69)));

  return ((((uint32)r0) << 24) | (((uint32)r1) << 16) | (((uint32)r2) << 8) | (((uint32)r3)));
}

/*********************************************************************
    Name: s3g_clock_lfsr

    Description: Clocking LFSR.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.4 and Section 3.4.5
*********************************************************************/
void s3g_clock_lfsr(S3G_STATE* state, uint32 f)
{
  uint32 v = (((state->lfsr[0] << 8) & 0xffffff00) ^ (s3g_mul_alpha((uint8)((state->lfsr[0] >> 24) & 0xff))) ^
              (state->lfsr[2]) ^ ((state->lfsr[11] >> 8) & 0x00ffffff) ^
              (s3g_div_alpha((uint8)((state->lfsr[11]) & 0xff))) ^ (f));
  uint8  i;

  for (i = 0; i < 15; i++) {
    state->lfsr[i] = state->lfsr[i + 1];
  }
  state->lfsr[15] = v;
}

/*********************************************************************
    Name: s3g_clock_fsm

    Description: Clocking FSM.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 3.4.6
*********************************************************************/
uint32 s3g_clock_fsm(S3G_STATE* state)
{
  uint32 f = ((state->lfsr[15] + state->fsm[0]) & 0xffffffff) ^ state->fsm[1];
  uint32 r = (state->fsm[1] + (state->fsm[2] ^ state->lfsr[5])) & 0xffffffff;

  state->fsm[2] = s3g_s2(state->fsm[1]);
  state->fsm[1] = s3g_s1(state->fsm[0]);
  state->fsm[0] = r;

  return f;
}

/*********************************************************************
    Name: s3g_initialize

    Description: Initialization.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 4.1
*********************************************************************/
void s3g_initialize(S3G_STATE* state, uint32 k[4], uint32 iv[4])
{
  uint8  i = 0;
  uint32 f = 0x0;

  state->lfsr = (uint32*)calloc(16, sizeof(uint32));
  state->fsm  = (uint32*)calloc(3, sizeof(uint32));

  state->lfsr[15] = k[3] ^ iv[0];
  state->lfsr[14] = k[2];
  state->lfsr[13] = k[1];
  state->lfsr[12] = k[0] ^ iv[1];

  state->lfsr[11] = k[3] ^ 0xffffffff;
  state->lfsr[10] = k[2] ^ 0xffffffff ^ iv[2];
  state->lfsr[9]  = k[1] ^ 0xffffffff ^ iv[3];
  state->lfsr[8]  = k[0] ^ 0xffffffff;
  state->lfsr[7]  = k[3];
  state->lfsr[6]  = k[2];
  state->lfsr[5]  = k[1];
  state->lfsr[4]  = k[0];
  state->lfsr[3]  = k[3] ^ 0xffffffff;
  state->lfsr[2]  = k[2] ^ 0xffffffff;
  state->lfsr[1]  = k[1] ^ 0xffffffff;
  state->lfsr[0]  = k[0] ^ 0xffffffff;

  state->fsm[0] = 0x0;
  state->fsm[1] = 0x0;
  state->fsm[2] = 0x0;
  for (i = 0; i < 32; i++) {
    f = s3g_clock_fsm(state);
    s3g_clock_lfsr(state, f);
  }
}

/*********************************************************************
    Name: s3g_deinitialize

    Description: Deinitialization.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
*********************************************************************/
void s3g_deinitialize(S3G_STATE* state)
{
  free(state->lfsr);
  free(state->fsm);
}

/*********************************************************************
    Name: s3g_generate_keystream

    Description: Generation of Keystream.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
                            Section 4.2
*********************************************************************/
void s3g_generate_keystream(S3G_STATE* state, uint32 n, uint32* ks)
{
  uint32 t = 0;
  uint32 f = 0x0;

  // Clock FSM once. Discard the output.
  s3g_clock_fsm(state);
  //  Clock LFSR in keystream mode once.
  s3g_clock_lfsr(state, 0x0);

  for (t = 0; t < n; t++) {
    f = s3g_clock_fsm(state);
    // Note that ks[t] corresponds to z_{t+1} in section 4.2
    ks[t] = f ^ state->lfsr[0];
    s3g_clock_lfsr(state, 0x0);
  }
}
