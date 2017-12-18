/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "srslte/phy/fec/turbodecoder.h"
#include "srslte/phy/fec/turbodecoder_gen.h"


#ifdef LV_HAVE_SSE
#include "srslte/phy/fec/turbodecoder_simd.h"
#endif

#include "srslte/phy/utils/vector.h"


int srslte_tdec_init(srslte_tdec_t * h, uint32_t max_long_cb) {
#ifdef LV_HAVE_SSE
  return srslte_tdec_simd_init(&h->tdec_simd, SRSLTE_TDEC_MAX_NPAR, max_long_cb);
#else
  h->input_conv = srslte_vec_malloc(sizeof(float) * (3*max_long_cb+12));
  if (!h->input_conv) {
    perror("malloc");
    return -1;
  }
  return srslte_tdec_gen_init(&h->tdec_gen, max_long_cb);
#endif
}

void srslte_tdec_free(srslte_tdec_t * h) {
#ifdef LV_HAVE_SSE
  srslte_tdec_simd_free(&h->tdec_simd);  
#else
  if (h->input_conv) {
    free(h->input_conv);
  }
  srslte_tdec_gen_free(&h->tdec_gen);
#endif

}

int srslte_tdec_reset(srslte_tdec_t * h, uint32_t long_cb) {
#ifdef LV_HAVE_SSE
  return srslte_tdec_simd_reset(&h->tdec_simd, long_cb);
#else
  return srslte_tdec_gen_reset(&h->tdec_gen, long_cb);
#endif
}

int srslte_tdec_reset_cb(srslte_tdec_t * h, uint32_t cb_idx) {
#ifdef LV_HAVE_SSE
  return srslte_tdec_simd_reset_cb(&h->tdec_simd, cb_idx);      
#else
  return srslte_tdec_gen_reset(&h->tdec_gen, h->tdec_gen.current_cb_len);
#endif
}

int srslte_tdec_get_nof_iterations_cb(srslte_tdec_t * h, uint32_t cb_idx)
{
#ifdef LV_HAVE_SSE
  return srslte_tdec_simd_get_nof_iterations_cb(&h->tdec_simd, cb_idx);
#else
  return h->tdec_gen.n_iter;
#endif  
}

void srslte_tdec_iteration_par(srslte_tdec_t * h, int16_t* input[SRSLTE_TDEC_MAX_NPAR], uint32_t long_cb) {
#ifdef LV_HAVE_SSE
  srslte_tdec_simd_iteration(&h->tdec_simd, input, long_cb);      
#else
  srslte_vec_convert_if(input[0], 0.01, h->input_conv, 3*long_cb+12);
  srslte_tdec_gen_iteration(&h->tdec_gen, h->input_conv, long_cb);
#endif
}

void srslte_tdec_iteration(srslte_tdec_t * h, int16_t* input, uint32_t long_cb) {
  int16_t *input_par[SRSLTE_TDEC_MAX_NPAR];
  input_par[0] = input; 
  return srslte_tdec_iteration_par(h, input_par, long_cb);
}

void srslte_tdec_decision_par(srslte_tdec_t * h, uint8_t *output[SRSLTE_TDEC_MAX_NPAR], uint32_t long_cb) {
#ifdef LV_HAVE_SSE
  return srslte_tdec_simd_decision(&h->tdec_simd, output, long_cb);
#else
  return srslte_tdec_gen_decision(&h->tdec_gen, output[0], long_cb);
#endif
}

uint32_t srslte_tdec_get_nof_parallel(srslte_tdec_t *h) {
#ifdef LV_HAVE_AVX2
  return 2;
#else
  return 1;
#endif
}

void srslte_tdec_decision(srslte_tdec_t * h, uint8_t *output, uint32_t long_cb) {
  uint8_t *output_par[SRSLTE_TDEC_MAX_NPAR];
  output_par[0] = output; 
  srslte_tdec_decision_par(h, output_par, long_cb);
}

void srslte_tdec_decision_byte_par(srslte_tdec_t * h, uint8_t *output[SRSLTE_TDEC_MAX_NPAR], uint32_t long_cb) {
#ifdef LV_HAVE_SSE
  srslte_tdec_simd_decision_byte(&h->tdec_simd, output, long_cb);  
#else
  srslte_tdec_gen_decision_byte(&h->tdec_gen, output[0], long_cb);
#endif  
}

void srslte_tdec_decision_byte_par_cb(srslte_tdec_t * h, uint8_t *output, uint32_t cb_idx, uint32_t long_cb) {
#ifdef LV_HAVE_SSE
  srslte_tdec_simd_decision_byte_cb(&h->tdec_simd, output, cb_idx, long_cb);
#else
  srslte_tdec_gen_decision_byte(&h->tdec_gen, output, long_cb);
#endif  
}

void srslte_tdec_decision_byte(srslte_tdec_t * h, uint8_t *output, uint32_t long_cb) {
  uint8_t *output_par[SRSLTE_TDEC_MAX_NPAR];
  output_par[0] = output; 
  srslte_tdec_decision_byte_par(h, output_par, long_cb);
}

int srslte_tdec_run_all_par(srslte_tdec_t * h, int16_t * input[SRSLTE_TDEC_MAX_NPAR],
                            uint8_t *output[SRSLTE_TDEC_MAX_NPAR],
                            uint32_t nof_iterations, uint32_t long_cb) {
#ifdef LV_HAVE_SSE
  return srslte_tdec_simd_run_all(&h->tdec_simd, input, output, nof_iterations, long_cb);  
#else
  srslte_vec_convert_if(input[0], 0.01, h->input_conv, 3*long_cb+12);
  return srslte_tdec_gen_run_all(&h->tdec_gen, h->input_conv, output[0], nof_iterations, long_cb);
#endif
}

int srslte_tdec_run_all(srslte_tdec_t * h, int16_t * input, uint8_t *output, uint32_t nof_iterations, uint32_t long_cb)
{
  uint8_t *output_par[SRSLTE_TDEC_MAX_NPAR];
  output_par[0] = output;   
  int16_t *input_par[SRSLTE_TDEC_MAX_NPAR];
  input_par[0] = input; 
 
  return srslte_tdec_run_all_par(h, input_par, output_par, nof_iterations, long_cb);
}


