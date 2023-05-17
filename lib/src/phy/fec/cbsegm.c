/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsran/phy/fec/cbsegm.h"
#include "srsran/phy/fec/ldpc/base_graph.h"
#include "srsran/phy/fec/turbo/turbodecoder_gen.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"
#include <strings.h>

/**
 * TS 36.212 V8.8.0 Table 5.1.3-3: Turbo code internal interleaver parameters
 */
const uint32_t tc_cb_sizes[SRSRAN_NOF_TC_CB_SIZES] = {
    40,   48,   56,   64,   72,   80,   88,   96,   104,  112,  120,  128,  136,  144,  152,  160,  168,  176,  184,
    192,  200,  208,  216,  224,  232,  240,  248,  256,  264,  272,  280,  288,  296,  304,  312,  320,  328,  336,
    344,  352,  360,  368,  376,  384,  392,  400,  408,  416,  424,  432,  440,  448,  456,  464,  472,  480,  488,
    496,  504,  512,  528,  544,  560,  576,  592,  608,  624,  640,  656,  672,  688,  704,  720,  736,  752,  768,
    784,  800,  816,  832,  848,  864,  880,  896,  912,  928,  944,  960,  976,  992,  1008, 1024, 1056, 1088, 1120,
    1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728,
    1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016, 2048, 2112, 2176, 2240, 2304, 2368, 2432, 2496, 2560, 2624,
    2688, 2752, 2816, 2880, 2944, 3008, 3072, 3136, 3200, 3264, 3328, 3392, 3456, 3520, 3584, 3648, 3712, 3776, 3840,
    3904, 3968, 4032, 4096, 4160, 4224, 4288, 4352, 4416, 4480, 4544, 4608, 4672, 4736, 4800, 4864, 4928, 4992, 5056,
    5120, 5184, 5248, 5312, 5376, 5440, 5504, 5568, 5632, 5696, 5760, 5824, 5888, 5952, 6016, 6080, 6144};

/**
 * @brief Calculates the number of code blocks and the total size
 * @param[in] B Transport block size including TB CRC
 * @param[in] Z Maximum transport block size
 * @param[out] C Number of code blocks
 * @param[out] B_prime Code block size
 */
static void cbsegm_cb_size(uint32_t B, uint32_t Z, uint32_t* C, uint32_t* B_prime)
{
  if (B <= Z) {
    *C       = 1;
    *B_prime = B;
  } else {
    *C       = SRSRAN_CEIL(B, (Z - 24U));
    *B_prime = B + 24U * (*C);
  }
}

int srsran_cbsegm(srsran_cbsegm_t* s, uint32_t tbs)
{
  uint32_t Bp, B, idx1;
  int      ret;

  if (tbs == 0) {
    bzero(s, sizeof(srsran_cbsegm_t));
    ret = SRSRAN_SUCCESS;
  } else {
    B      = tbs + 24;
    s->tbs = tbs;

    // Calculate CB sizes
    cbsegm_cb_size(B, SRSRAN_TCOD_MAX_LEN_CB, &s->C, &Bp);

    ret = srsran_cbsegm_cbindex((Bp - 1) / s->C + 1);
    if (ret != SRSRAN_ERROR) {
      idx1 = (uint32_t)ret;
      ret  = srsran_cbsegm_cbsize(idx1);
      if (ret != SRSRAN_ERROR) {
        s->K1     = (uint32_t)ret;
        s->K1_idx = idx1;
        if (idx1 > 0) {
          ret = srsran_cbsegm_cbsize(idx1 - 1);
        }
        if (ret != SRSRAN_ERROR) {
          if (s->C == 1) {
            s->K2     = 0;
            s->K2_idx = 0;
            s->C2     = 0;
            s->C1     = 1;
          } else {
            s->K2     = (uint32_t)ret;
            s->K2_idx = idx1 - 1;
            s->C2     = (s->C * s->K1 - Bp) / (s->K1 - s->K2);
            s->C1     = s->C - s->C2;
          }
          s->L_tb = 24; // 24 bit CRC always
          s->L_cb = 24; // 24 bit CRC always
          s->F    = s->C1 * s->K1 + s->C2 * s->K2 - Bp;
          INFO("CB Segmentation: TBS: %d, C=%d, C+=%d K+=%d, C-=%d, K-=%d, F=%d, Bp=%d",
               tbs,
               s->C,
               s->C1,
               s->K1,
               s->C2,
               s->K2,
               s->F,
               Bp);
          ret = SRSRAN_SUCCESS;
        }
      }
    }
  }
  return ret;
}

int srsran_cbsegm_cbindex(uint32_t long_cb)
{
  int j = 0;
  while (j < SRSRAN_NOF_TC_CB_SIZES && tc_cb_sizes[j] < long_cb) {
    j++;
  }

  if (j == SRSRAN_NOF_TC_CB_SIZES) {
    return SRSRAN_ERROR;
  } else {
    return j;
  }
}

int srsran_cbsegm_cbsize(uint32_t index)
{
  if (index < SRSRAN_NOF_TC_CB_SIZES) {
    return (int)tc_cb_sizes[index];
  } else {
    return SRSRAN_ERROR;
  }
}

bool srsran_cbsegm_cbsize_isvalid(uint32_t size)
{
  for (int i = 0; i < SRSRAN_NOF_TC_CB_SIZES; i++) {
    if (tc_cb_sizes[i] == size) {
      return true;
    }
  }
  return false;
}

/**
 * @brief Selects a lifting size Z_c that satisfies K_b · Z_c >= Kp
 * @param[in] Kp actual code block size
 * @param[in] K_b Some constant
 * @param[out] Z_c Lifting size
 * @param[out] i_ls Lifting size group index
 */
static int cbsegm_ldpc_select_ls(uint32_t Kp, uint32_t K_b, uint32_t* Z_c, uint8_t* i_ls)
{
  // Early return if the minimum required lift size is too high
  if (SRSRAN_CEIL(Kp, K_b) > MAX_LIFTSIZE) {
    return SRSRAN_ERROR;
  }

  // Iterate from the minimum required lift size to the maximum value
  for (uint16_t Z = SRSRAN_CEIL(Kp, K_b); Z <= MAX_LIFTSIZE; Z++) {
    // Get index for a selected lifting size
    uint8_t i = get_ls_index(Z);

    // If the lifting index is valid, save outputs and return
    if (i != VOID_LIFTSIZE) {
      if (i_ls) {
        *i_ls = i;
      }

      if (Z_c) {
        *Z_c = Z;
      }

      return SRSRAN_SUCCESS;
    }

    // Otherwise continue...
  }

  return SRSRAN_ERROR;
}

/**
 * @brief Calculate the transport block (TB) CRC length for LDPC based shared
 *
 * @remark Implemented according to TS 38.212 V15.9.0 7.2.1 Transport block CRC attachment
 *
 * @param tbs Transport block size
 * @return The TB CRC length L
 */
static uint32_t srsran_cbsegm_ldpc_L(uint32_t tbs)
{
  if (tbs <= 3824) {
    return 16;
  }

  return 24;
}

static int srsran_cbsegm_ldpc(srsran_cbsegm_t* s, srsran_basegraph_t bg, uint32_t tbs)
{
  // Check input pointer
  if (s == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Early return if no TBS is provided
  if (tbs == 0) {
    bzero(s, sizeof(srsran_cbsegm_t));
    return SRSRAN_SUCCESS;
  }

  // Calculate TB CRC length
  uint32_t L = srsran_cbsegm_ldpc_L(tbs);

  // Select maximum code block size
  uint32_t K_cb = (bg == BG1) ? SRSRAN_LDPC_BG1_MAX_LEN_CB : SRSRAN_LDPC_BG2_MAX_LEN_CB;

  // Calculate CB sizes
  uint32_t B  = tbs + L;
  uint32_t C  = 0;
  uint32_t Bp = 0;
  cbsegm_cb_size(B, K_cb, &C, &Bp);

  // Calculate the code block size
  uint32_t Kp  = Bp / C;
  uint32_t K_b = 22;
  if (bg == BG2) {
    if (B > 640) {
      K_b = 10;
    } else if (B > 560) {
      K_b = 9;
    } else if (B > 192) {
      K_b = 8;
    } else {
      K_b = 6;
    }
  }

  // Select lifting size
  uint8_t  i_ls = 0;
  uint32_t Z_c  = 0;
  int      ret  = cbsegm_ldpc_select_ls(Kp, K_b, &Z_c, &i_ls);
  if (ret < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }
  uint32_t K = Z_c * ((bg == BG1) ? 22U : 10U);

  // Save segmentation
  s->tbs    = tbs;
  s->L_tb   = L;
  s->L_cb   = C > 1 ? 24 : 0;
  s->C      = C;
  s->F      = K * C;
  s->C1     = C;
  s->K1     = K;
  s->K1_idx = i_ls;
  s->Z      = Z_c;

  // Only one CB size is used
  s->C2     = 0;
  s->K2     = 0;
  s->K2_idx = 0;

  INFO("LDPC CB Segmentation: TBS: %d, C=%d, K=%d, F=%d, Bp=%d", tbs, s->C, s->K1, s->F, Bp);

  return SRSRAN_SUCCESS;
}

int srsran_cbsegm_ldpc_bg1(srsran_cbsegm_t* s, uint32_t tbs)
{
  return srsran_cbsegm_ldpc(s, BG1, tbs);
}

int srsran_cbsegm_ldpc_bg2(srsran_cbsegm_t* s, uint32_t tbs)
{
  return srsran_cbsegm_ldpc(s, BG2, tbs);
}