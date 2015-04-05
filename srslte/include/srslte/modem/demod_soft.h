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

/******************************************************************************
 *  File:         demod_soft.h
 *
 *  Description:  Soft demodulator.
 *                Supports BPSK, QPSK, 16QAM and 64QAM.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 7.1
 *****************************************************************************/

#ifndef DEMOD_SOFT_
#define DEMOD_SOFT_

#include <complex.h>
#include <stdint.h>

#include "srslte/config.h"
#include "modem_table.h"

typedef enum SRSLTE_API { 
  SRSLTE_DEMOD_SOFT_ALG_EXACT, 
  SRSLTE_DEMOD_SOFT_ALG_APPROX   
} srslte_demod_soft_alg_t;

typedef struct SRSLTE_API {
  float sigma;      // noise power
  srslte_demod_soft_alg_t alg_type;    // soft demapping algorithm (SRSLTE_DEMOD_SOFT_ALG_EXACT or SRSLTE_DEMOD_SOFT_ALG_APPROX)
  srslte_modem_table_t *table;  // symbol mapping table (see modem_table.h)
  uint32_t *zones; 
  float *dd; 
  uint32_t max_symbols;
} srslte_demod_soft_t;

SRSLTE_API int srslte_demod_soft_init(srslte_demod_soft_t *q, 
                                      uint32_t max_symbols);

SRSLTE_API void srslte_demod_soft_free(srslte_demod_soft_t *q); 

SRSLTE_API void srslte_demod_soft_table_set(srslte_demod_soft_t *q, 
                                            srslte_modem_table_t *table);

SRSLTE_API void srslte_demod_soft_alg_set(srslte_demod_soft_t *q, 
                                          srslte_demod_soft_alg_t alg_type);

SRSLTE_API void srslte_demod_soft_sigma_set(srslte_demod_soft_t *q, 
                                            float sigma);

SRSLTE_API int srslte_demod_soft_demodulate(srslte_demod_soft_t *q, 
                                            const cf_t* symbols, 
                                            float* llr, 
                                            int nsymbols);


/* High-level API */
typedef struct SRSLTE_API {
  srslte_demod_soft_t obj;
  srslte_modem_table_t table;

  struct srslte_demod_soft_init{
    srslte_mod_t std;    // symbol mapping standard (see modem_table.h)
  } init;

  const cf_t* input;
  int in_len;

  struct srslte_demod_soft_ctrl_in {
    float sigma;      // Estimated noise variance
    srslte_demod_soft_alg_t alg_type;    // soft demapping algorithm (SRSLTE_DEMOD_SOFT_ALG_EXACT or SRSLTE_DEMOD_SOFT_ALG_APPROX)
  }ctrl_in;

  float* output;
  int out_len;

}srslte_demod_soft_hl;

SRSLTE_API int srslte_demod_soft_initialize(srslte_demod_soft_hl* hl);
SRSLTE_API int srslte_demod_soft_work(srslte_demod_soft_hl* hl);
SRSLTE_API int srslte_demod_soft_stop(srslte_demod_soft_hl* hl);


#endif // DEMOD_SOFT_
