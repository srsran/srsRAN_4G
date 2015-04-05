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

/**********************************************************************************************
 *  File:         rm_turbo.h
 *
 *  Description:  Rate matching for turbo coded transport channels.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.4.1
 *********************************************************************************************/

#ifndef RM_TURBO_
#define RM_TURBO_

#include "srslte/config.h"

#ifndef SRSLTE_RX_NULL
#define SRSLTE_RX_NULL 10000
#endif

#ifndef SRSLTE_TX_NULL
#define SRSLTE_TX_NULL 100
#endif

#include "srslte/config.h"


SRSLTE_API int srslte_rm_turbo_tx(uint8_t *w_buff,
                                  uint32_t buff_len, 
                                  uint8_t *input, 
                                  uint32_t in_len, 
                                  uint8_t *output,
                                  uint32_t out_len, 
                                  uint32_t rv_idx);

SRSLTE_API int srslte_rm_turbo_rx(float *w_buff,
                                  uint32_t buff_len, 
                                  float *input, 
                                  uint32_t in_len,
                                  float *output, 
                                  uint32_t out_len, 
                                  uint32_t rv_idx, 
                                  uint32_t nof_filler_bits);

/* High-level API */
typedef struct SRSLTE_API {
  
  struct srslte_rm_turbo_init {
    int direction;
  } init;
  void *input;  // input type may be uint8_t or float depending on hard
  int in_len;
  struct srslte_rm_turbo_ctrl_in {
    int E;
    int S;
    int rv_idx;
  } ctrl_in;
  void *output;
  int out_len;
} srslte_rm_turbo_hl;

SRSLTE_API int srslte_rm_turbo_initialize(srslte_rm_turbo_hl* h);
SRSLTE_API int srslte_rm_turbo_work(srslte_rm_turbo_hl* hl);
SRSLTE_API int srslte_rm_turbo_stop(srslte_rm_turbo_hl* hl);

#endif
