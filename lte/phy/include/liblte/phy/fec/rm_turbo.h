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

#ifndef RM_TURBO_
#define RM_TURBO_

#include "liblte/config.h"

#ifndef RX_NULL
#define RX_NULL 10000
#endif

#ifndef TX_NULL
#define TX_NULL 100
#endif

#include "liblte/config.h"


LIBLTE_API int rm_turbo_tx(uint8_t *w_buff,
                           uint32_t buff_len, 
                           uint8_t *input, 
                           uint32_t in_len, 
                           uint8_t *output,
                           uint32_t out_len, 
                           uint32_t rv_idx);

LIBLTE_API int rm_turbo_rx(float *w_buff,
                           uint32_t buff_len, 
                           float *input, 
                           uint32_t in_len,
                           float *output, 
                           uint32_t out_len, 
                           uint32_t rv_idx);

/* High-level API */
typedef struct LIBLTE_API {
  
  struct rm_turbo_init {
    int direction;
  } init;
  void *input;  // input type may be uint8_t or float depending on hard
  int in_len;
  struct rm_turbo_ctrl_in {
    int E;
    int S;
    int rv_idx;
  } ctrl_in;
  void *output;
  int out_len;
} rm_turbo_hl;

LIBLTE_API int rm_turbo_initialize(rm_turbo_hl* h);
LIBLTE_API int rm_turbo_work(rm_turbo_hl* hl);
LIBLTE_API int rm_turbo_stop(rm_turbo_hl* hl);

#endif
