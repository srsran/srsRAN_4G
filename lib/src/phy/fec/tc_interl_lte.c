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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/fec/cbsegm.h"
#include "srslte/phy/fec/tc_interl.h"
#include "srslte/phy/fec/turbocoder.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

/************************************************
 *
 *  LTE TURBO CODE INTERLEAVER
 *
 ************************************************/

const uint32_t f1_list[SRSLTE_NOF_TC_CB_SIZES] = {
    3,   7,   19,  7,   7,   11,  5,   11,  7,   41,  103, 15,  9,   17,  9,   21,  101, 21,  57, 23,  13,
    27,  11,  27,  85,  29,  33,  15,  17,  33,  103, 19,  19,  37,  19,  21,  21,  115, 193, 21, 133, 81,
    45,  23,  243, 151, 155, 25,  51,  47,  91,  29,  29,  247, 29,  89,  91,  157, 55,  31,  17, 35,  227,
    65,  19,  37,  41,  39,  185, 43,  21,  155, 79,  139, 23,  217, 25,  17,  127, 25,  239, 17, 137, 215,
    29,  15,  147, 29,  59,  65,  55,  31,  17,  171, 67,  35,  19,  39,  19,  199, 21,  211, 21, 43,  149,
    45,  49,  71,  13,  17,  25,  183, 55,  127, 27,  29,  29,  57,  45,  31,  59,  185, 113, 31, 17,  171,
    209, 253, 367, 265, 181, 39,  27,  127, 143, 43,  29,  45,  157, 47,  13,  111, 443, 51,  51, 451, 257,
    57,  313, 271, 179, 331, 363, 375, 127, 31,  33,  43,  33,  477, 35,  233, 357, 337, 37,  71, 71,  37,
    39,  127, 39,  39,  31,  113, 41,  251, 43,  21,  43,  45,  45,  161, 89,  323, 47,  23,  47, 263};

const uint32_t f2_list[SRSLTE_NOF_TC_CB_SIZES] = {
    10,  12,  42,  16,  18,  20,  22,  24,  26,  84,  90,  32,  34,  108, 38,  120, 84,  44,  46,  48,  50,
    52,  36,  56,  58,  60,  62,  32,  198, 68,  210, 36,  74,  76,  78,  120, 82,  84,  86,  44,  90,  46,
    94,  48,  98,  40,  102, 52,  106, 72,  110, 168, 114, 58,  118, 180, 122, 62,  84,  64,  66,  68,  420,
    96,  74,  76,  234, 80,  82,  252, 86,  44,  120, 92,  94,  48,  98,  80,  102, 52,  106, 48,  110, 112,
    114, 58,  118, 60,  122, 124, 84,  64,  66,  204, 140, 72,  74,  76,  78,  240, 82,  252, 86,  88,  60,
    92,  846, 48,  28,  80,  102, 104, 954, 96,  110, 112, 114, 116, 354, 120, 610, 124, 420, 64,  66,  136,
    420, 216, 444, 456, 468, 80,  164, 504, 172, 88,  300, 92,  188, 96,  28,  240, 204, 104, 212, 192, 220,
    336, 228, 232, 236, 120, 244, 248, 168, 64,  130, 264, 134, 408, 138, 280, 142, 480, 146, 444, 120, 152,
    462, 234, 158, 80,  96,  902, 166, 336, 170, 86,  174, 176, 178, 120, 182, 184, 186, 94,  190, 480};

int srslte_tc_interl_LTE_gen(srslte_tc_interl_t* h, uint32_t long_cb)
{
  return srslte_tc_interl_LTE_gen_interl(h, long_cb, 1);
}

#define deinter(x, win) ((x % (long_cb / win)) * (win) + x / (long_cb / win))
#define inter(x, win) ((x % win) * (long_cb / win) + x / win)

int srslte_tc_interl_LTE_gen_interl(srslte_tc_interl_t* h, uint32_t long_cb, uint32_t interl_win)
{
  uint32_t cb_table_idx, f1, f2;
  uint64_t i, j;

  if (long_cb > h->max_long_cb) {
    ERROR("Interleaver initiated for max_long_cb=%d\n", h->max_long_cb);
    return -1;
  }

  cb_table_idx = srslte_cbsegm_cbindex(long_cb);
  if (cb_table_idx == -1) {
    ERROR("Can't find long_cb=%d in valid TC CB table\n", long_cb);
    return -1;
  }

  f1 = f1_list[cb_table_idx];
  f2 = f2_list[cb_table_idx];

  h->forward[0] = 0;
  h->reverse[0] = 0;
  for (i = 1; i < long_cb; i++) {
    j             = (f1 * i + f2 * i * i) % (long_cb);
    h->forward[i] = (uint32_t)j;
    h->reverse[j] = (uint32_t)i;
  }
  if (interl_win != 1) {
    uint16_t* f = srslte_vec_u16_malloc(long_cb);
    uint16_t* r = srslte_vec_u16_malloc(long_cb);
    memcpy(f, h->forward, long_cb * sizeof(uint16_t));
    memcpy(r, h->reverse, long_cb * sizeof(uint16_t));
    for (i = 0; i < long_cb; i++) {
      h->forward[i] = deinter(f[inter(i, interl_win)], interl_win);
      h->reverse[i] = deinter(r[inter(i, interl_win)], interl_win);
    }
    free(f);
    free(r);
  }

  return 0;
}
