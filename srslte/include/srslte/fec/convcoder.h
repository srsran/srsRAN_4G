/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */



#ifndef CONVCODER_
#define CONVCODER_

#include <stdbool.h>
#include "srslte/config.h"

typedef struct SRSLTE_API {
  uint32_t R;
  uint32_t K;
  uint32_t poly[3];
  bool tail_biting;
}convcoder_t;

SRSLTE_API int convcoder_encode(convcoder_t *q, uint8_t *input, uint8_t *output, uint32_t frame_length);


/* High-level API */
typedef struct SRSLTE_API {
  convcoder_t obj;
  struct convcoder_ctrl_in {
    int rate;
    int constraint_length;
    int tail_bitting;
    int generator_0;
    int generator_1;
    int generator_2;
    int frame_length;
  } ctrl_in;
  uint8_t *input;
  int in_len;
  uint8_t *output;
  int out_len;
}convcoder_hl;

SRSLTE_API int convcoder_initialize(convcoder_hl* h);
SRSLTE_API int convcoder_work(convcoder_hl* hl);
SRSLTE_API int convcoder_stop(convcoder_hl* h);

#endif
