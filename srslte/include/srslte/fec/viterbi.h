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


#ifndef VITERBI_
#define VITERBI_

#include <stdbool.h>
#include "srslte/config.h"

typedef enum {
  viterbi_27, viterbi_29, viterbi_37, viterbi_39
}viterbi_type_t;

typedef struct SRSLTE_API{
  void *ptr;
  uint32_t R;
  uint32_t K;
  uint32_t framebits;
  bool tail_biting;
  float gain_quant; 
  uint32_t poly[3];
  int (*decode) (void*, uint8_t*, uint8_t*, uint32_t);
  int (*decode_f) (void*, float*, uint8_t*, uint32_t);
  void (*free) (void*);
  uint8_t *tmp;
  uint8_t *symbols_uc;
}viterbi_t;

SRSLTE_API int viterbi_init(viterbi_t *q, 
                            viterbi_type_t type, 
                            uint32_t poly[3], 
                            uint32_t max_frame_length, 
                            bool tail_bitting);

SRSLTE_API void viterbi_set_gain_quant(viterbi_t *q, 
                                       float gain_quant); 

SRSLTE_API void viterbi_free(viterbi_t *q);

SRSLTE_API int viterbi_decode_f(viterbi_t *q, 
                                float *symbols, 
                                uint8_t *data, 
                                uint32_t frame_length);

SRSLTE_API int viterbi_decode_uc(viterbi_t *q, 
                                 uint8_t *symbols, 
                                 uint8_t *data, 
                                 uint32_t frame_length);


/* High-level API */
typedef struct SRSLTE_API{
  viterbi_t obj;
  struct viterbi_init {
    int rate;
    int constraint_length;
    int tail_bitting;
    int generator_0;
    int generator_1;
    int generator_2;
    int frame_length;
  } init;
  float *input;
  int in_len;
  uint8_t *output;
  int out_len;
}viterbi_hl;

SRSLTE_API int viterbi_initialize(viterbi_hl* h);
SRSLTE_API int viterbi_work(viterbi_hl* hl);
SRSLTE_API int viterbi_stop(viterbi_hl* h);

#endif
