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


#ifndef DEMOD_SOFT_
#define DEMOD_SOFT_

#include <complex.h>
#include <stdint.h>

#include "srslte/config.h"
#include "modem_table.h"

enum alg { EXACT, APPROX };

typedef struct SRSLTE_API {
  float sigma;      // noise power
  enum alg alg_type;    // soft demapping algorithm (EXACT or APPROX)
  modem_table_t *table;  // symbol mapping table (see modem_table.h)
  uint32_t *zones; 
  float *dd; 
  uint32_t max_symbols;
}demod_soft_t;

SRSLTE_API int demod_soft_init(demod_soft_t *q, uint32_t max_symbols);
SRSLTE_API void demod_soft_free(demod_soft_t *q); 
SRSLTE_API void demod_soft_table_set(demod_soft_t *q, modem_table_t *table);
SRSLTE_API void demod_soft_alg_set(demod_soft_t *q, enum alg alg_type);
SRSLTE_API void demod_soft_sigma_set(demod_soft_t *q, float sigma);
SRSLTE_API int demod_soft_demodulate(demod_soft_t *q, const cf_t* symbols, float* llr, int nsymbols);


/* High-level API */
typedef struct SRSLTE_API {
  demod_soft_t obj;
  modem_table_t table;

  struct demod_soft_init{
    lte_mod_t std;    // symbol mapping standard (see modem_table.h)
  } init;

  const cf_t* input;
  int in_len;

  struct demod_soft_ctrl_in {
    float sigma;      // Estimated noise variance
    enum alg alg_type;    // soft demapping algorithm (EXACT or APPROX)
  }ctrl_in;

  float* output;
  int out_len;

}demod_soft_hl;

SRSLTE_API int demod_soft_initialize(demod_soft_hl* hl);
SRSLTE_API int demod_soft_work(demod_soft_hl* hl);
SRSLTE_API int demod_soft_stop(demod_soft_hl* hl);


#endif // DEMOD_SOFT_
