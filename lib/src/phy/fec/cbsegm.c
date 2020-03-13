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

#include <math.h>
#include <strings.h>

#include "srslte/phy/fec/cbsegm.h"
#include "srslte/phy/fec/turbodecoder_gen.h"
#include "srslte/phy/utils/debug.h"

const uint32_t tc_cb_sizes[SRSLTE_NOF_TC_CB_SIZES] = {
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
 * Calculate Codeblock Segmentation parameters as in Section 5.1.2 of 36.212
 *
 * @param[out] s Output of code block segmentation calculation
 * @param[in] tbs Input Transport Block Size in bits. CRC's will be added to this
 * @return Error code
 */
int srslte_cbsegm(srslte_cbsegm_t* s, uint32_t tbs)
{
  uint32_t Bp, B, idx1;
  int      ret;

  if (tbs == 0) {
    bzero(s, sizeof(srslte_cbsegm_t));
    ret = SRSLTE_SUCCESS;
  } else {
    B      = tbs + 24;
    s->tbs = tbs;

    /* Calculate CB sizes */
    if (B <= SRSLTE_TCOD_MAX_LEN_CB) {
      s->C = 1;
      Bp   = B;
    } else {
      s->C = (uint32_t)ceilf((float)B / (SRSLTE_TCOD_MAX_LEN_CB - 24));
      Bp   = B + 24 * s->C;
    }
    ret = srslte_cbsegm_cbindex((Bp - 1) / s->C + 1);
    if (ret != SRSLTE_ERROR) {
      idx1 = (uint32_t)ret;
      ret  = srslte_cbsegm_cbsize(idx1);
      if (ret != SRSLTE_ERROR) {
        s->K1     = (uint32_t)ret;
        s->K1_idx = idx1;
        if (idx1 > 0) {
          ret = srslte_cbsegm_cbsize(idx1 - 1);
        }
        if (ret != SRSLTE_ERROR) {
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
          s->F = s->C1 * s->K1 + s->C2 * s->K2 - Bp;
          INFO("CB Segmentation: TBS: %d, C=%d, C+=%d K+=%d, C-=%d, K-=%d, F=%d, Bp=%d\n",
               tbs,
               s->C,
               s->C1,
               s->K1,
               s->C2,
               s->K2,
               s->F,
               Bp);
          ret = SRSLTE_SUCCESS;
        }
      }
    }
  }
  return ret;
}

/*
 * Finds index of minimum K>=long_cb in Table 5.1.3-3 of 36.212
 *
 * @return I_TBS or error code
 */
int srslte_cbsegm_cbindex(uint32_t long_cb)
{
  int j = 0;
  while (j < SRSLTE_NOF_TC_CB_SIZES && tc_cb_sizes[j] < long_cb) {
    j++;
  }

  if (j == SRSLTE_NOF_TC_CB_SIZES) {
    return SRSLTE_ERROR;
  } else {
    return j;
  }
}

/*
 * Returns Turbo coder interleaver size for Table 5.1.3-3 (36.212) index
 *
 * @return Code block size in bits or error code
 */
int srslte_cbsegm_cbsize(uint32_t index)
{
  if (index < SRSLTE_NOF_TC_CB_SIZES) {
    return (int)tc_cb_sizes[index];
  } else {
    return SRSLTE_ERROR;
  }
}

/**
 * Check is code block size is valid for LTE Turbo Code
 *
 * @param[in] size Size of code block in bits
 * @return true if Code Block size is allowed
 */
bool srslte_cbsegm_cbsize_isvalid(uint32_t size)
{
  for (int i = 0; i < SRSLTE_NOF_TC_CB_SIZES; i++) {
    if (tc_cb_sizes[i] == size) {
      return true;
    }
  }
  return false;
}
