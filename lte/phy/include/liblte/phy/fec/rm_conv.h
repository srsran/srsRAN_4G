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

#ifndef RM_CONV_
#define RM_CONV_

#include "liblte/config.h"

#ifndef RX_NULL
#define RX_NULL 10000
#endif

#ifndef TX_NULL
#define TX_NULL 100
#endif
LIBLTE_API int rm_conv_tx(uint8_t *input, 
                          uint32_t in_len, 
                          uint8_t *output, 
                          uint32_t out_len);

LIBLTE_API int rm_conv_rx(float *input, 
                          uint32_t in_len, 
                          float *output, 
                          uint32_t out_len);

/* High-level API */
typedef struct
  LIBLTE_API {
    struct rm_conv_init {
      int direction;
    } init;
    void *input;      // input type may be uint8_t or float depending on hard
    int in_len;
    struct rm_conv_ctrl_in {
      int E;
      int S;
    } ctrl_in;
    void *output;
    int out_len;
  } rm_conv_hl;

  LIBLTE_API int rm_conv_initialize(rm_conv_hl* h);
  LIBLTE_API int rm_conv_work(rm_conv_hl* hl);
  LIBLTE_API int rm_conv_stop(rm_conv_hl* hl);

#endif
