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

#ifndef RM_CONV_
#define RM_CONV_

#include "srslte/config.h"

#ifndef SRSLTE_RX_NULL
#define SRSLTE_RX_NULL 10000
#endif

#ifndef SRSLTE_TX_NULL
#define SRSLTE_TX_NULL 100
#endif
SRSLTE_API int srslte_rm_conv_tx(uint8_t *input, 
                                 uint32_t in_len, 
                                 uint8_t *output, 
                                 uint32_t out_len);

SRSLTE_API int srslte_rm_conv_rx(float *input, 
                                 uint32_t in_len, 
                                 float *output, 
                                 uint32_t out_len);

/* High-level API */
typedef struct
  SRSLTE_API {
    struct srslte_rm_conv_init {
      int direction;
    } init;
    void *input;      // input type may be uint8_t or float depending on hard
    int in_len;
    struct srslte_rm_conv_ctrl_in {
      int E;
      int S;
    } ctrl_in;
    void *output;
    int out_len;
  } srslte_rm_conv_hl;

  SRSLTE_API int srslte_rm_conv_initialize(srslte_rm_conv_hl* h);
  
  SRSLTE_API int srslte_rm_conv_work(srslte_rm_conv_hl* hl);
  
  SRSLTE_API int srslte_rm_conv_stop(srslte_rm_conv_hl* hl);

#endif
