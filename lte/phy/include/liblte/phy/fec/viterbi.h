/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
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
#include "liblte/config.h"

typedef enum {
  viterbi_27, viterbi_29, viterbi_37, viterbi_39
}viterbi_type_t;

typedef struct LIBLTE_API{
  void *ptr;
  int R;
  int K;
  unsigned int framebits;
  bool tail_biting;
  int poly[3];
  int (*decode) (void*, unsigned char*, char*, int);
  void (*free) (void*);
  unsigned char *tmp;
  unsigned char *symbols_uc;
}viterbi_t;

LIBLTE_API int viterbi_init(viterbi_t *q, viterbi_type_t type, int poly[3], int max_frame_length, bool tail_bitting);
LIBLTE_API void viterbi_free(viterbi_t *q);
LIBLTE_API int viterbi_decode_f(viterbi_t *q, float *symbols, char *data, int frame_length);
LIBLTE_API int viterbi_decode_uc(viterbi_t *q, unsigned char *symbols, char *data, int frame_length);


/* High-level API */
typedef struct LIBLTE_API{
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
  char *output;
  int out_len;
}viterbi_hl;

LIBLTE_API int viterbi_initialize(viterbi_hl* h);
LIBLTE_API int viterbi_work(viterbi_hl* hl);
LIBLTE_API int viterbi_stop(viterbi_hl* h);

#endif
