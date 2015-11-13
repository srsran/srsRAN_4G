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

#include "srslte/fec/turbodecoder.h"
#include "srslte/fec/turbodecoder_gen.h"


#ifdef LV_HAVE_SSE
#include "srslte/fec/turbodecoder_sse.h"
#endif

#include "srslte/utils/vector.h"


int srslte_tdec_init(srslte_tdec_t * h, uint32_t max_long_cb) {
#ifdef LV_HAVE_SSE
  return srslte_tdec_sse_init(&h->tdec_sse, max_long_cb);
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
  srslte_tdec_sse_free(&h->tdec_sse);
#else
  if (h->input_conv) {
    free(h->input_conv);
  }
  srslte_tdec_gen_free(&h->tdec_gen);
#endif

}

int srslte_tdec_reset(srslte_tdec_t * h, uint32_t long_cb) {
#ifdef LV_HAVE_SSE
  return srslte_tdec_sse_reset(&h->tdec_sse, long_cb);
#else
  return srslte_tdec_gen_reset(&h->tdec_gen, long_cb);
#endif
}

void srslte_tdec_iteration(srslte_tdec_t * h, int16_t* input, uint32_t long_cb) {
#ifdef LV_HAVE_SSE
  srslte_tdec_sse_iteration(&h->tdec_sse, input, long_cb);
#else
  srslte_vec_convert_if(input, h->input_conv, 0.01, 3*long_cb+12);
  srslte_tdec_gen_iteration(&h->tdec_gen, h->input_conv, long_cb);
#endif
}

void srslte_tdec_decision(srslte_tdec_t * h, uint8_t *output, uint32_t long_cb) {
#ifdef LV_HAVE_SSE
  return srslte_tdec_sse_decision(&h->tdec_sse, output, long_cb);
#else
  return srslte_tdec_gen_decision(&h->tdec_gen, output, long_cb);
#endif

}

void srslte_tdec_decision_byte(srslte_tdec_t * h, uint8_t *output, uint32_t long_cb) {
#ifdef LV_HAVE_SSE
  return srslte_tdec_sse_decision_byte(&h->tdec_sse, output, long_cb);
#else
  return srslte_tdec_gen_decision_byte(&h->tdec_gen, output, long_cb);
#endif
  
}

int srslte_tdec_run_all(srslte_tdec_t * h, int16_t * input, uint8_t *output, uint32_t nof_iterations, uint32_t long_cb)
{
#ifdef LV_HAVE_SSE
  return srslte_tdec_sse_run_all(&h->tdec_sse, input, output, nof_iterations, long_cb);
#else
  srslte_vec_convert_if(input, h->input_conv, 0.01, 3*long_cb+12);
  return srslte_tdec_gen_run_all(&h->tdec_gen, h->input_conv, output, nof_iterations, long_cb);
#endif
}
