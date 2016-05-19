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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <math.h>
#include <string.h>

#include "srslte/utils/vector.h"
#include "srslte/fec/viterbi.h"
#include "parity.h"
#include "viterbi37.h"

#define DEB 0

#define TB_ITER 3

#define DEFAULT_GAIN 16

//#undef LV_HAVE_SSE

int decode37(void *o, uint8_t *symbols, uint8_t *data, uint32_t frame_length) {
  srslte_viterbi_t *q = o;

  uint32_t best_state;

  if (frame_length > q->framebits) {
    fprintf(stderr, "Initialized decoder for max frame length %d bits\n",
        q->framebits);
    return -1;
  }

  /* Initialize Viterbi decoder */
  init_viterbi37_port(q->ptr, q->tail_biting ? -1 : 0);

  /* Decode block */
  if (q->tail_biting) {
    for (int i=0;i<TB_ITER;i++) {
      memcpy(&q->tmp[i*3*frame_length], symbols, 3*frame_length*sizeof(uint8_t));      
    }
    update_viterbi37_blk_port(q->ptr, q->tmp, TB_ITER*frame_length, &best_state);
    chainback_viterbi37_port(q->ptr,  q->tmp, TB_ITER*frame_length, best_state);
    memcpy(data, &q->tmp[((int) (TB_ITER/2))*frame_length], frame_length*sizeof(uint8_t));
  } else {
    update_viterbi37_blk_port(q->ptr, symbols, frame_length + q->K - 1, NULL);
    chainback_viterbi37_port(q->ptr, data, frame_length, 0);
  }

  return q->framebits;
}


#ifdef LV_HAVE_SSE
int decode37_sse(void *o, uint8_t *symbols, uint8_t *data, uint32_t frame_length) {
  srslte_viterbi_t *q = o;

  uint32_t best_state;

  if (frame_length > q->framebits) {
    fprintf(stderr, "Initialized decoder for max frame length %d bits\n",
        q->framebits);
    return -1;
  }

  /* Initialize Viterbi decoder */
  init_viterbi37_sse(q->ptr, q->tail_biting?-1:0);

  /* Decode block */
  if (q->tail_biting) {
    for (int i=0;i<TB_ITER;i++) {
      memcpy(&q->tmp[i*3*frame_length], symbols, 3*frame_length*sizeof(uint8_t));      
    }
    update_viterbi37_blk_sse(q->ptr, q->tmp, TB_ITER*frame_length, &best_state);
    chainback_viterbi37_sse(q->ptr,  q->tmp, TB_ITER*frame_length, best_state);
    memcpy(data, &q->tmp[((int) (TB_ITER/2))*frame_length], frame_length*sizeof(uint8_t));
  } else {
    update_viterbi37_blk_sse(q->ptr, symbols, frame_length+q->K-1, &best_state);
    chainback_viterbi37_sse(q->ptr, data, frame_length, best_state);
  }
  
  return q->framebits;
}

void free37_sse(void *o) {
  srslte_viterbi_t *q = o;
  if (q->symbols_uc) {
    free(q->symbols_uc);
  }
  if (q->tmp) {
    free(q->tmp);
  }
  delete_viterbi37_sse(q->ptr);
}

#endif

void free37(void *o) {
  srslte_viterbi_t *q = o;
  if (q->symbols_uc) {
    free(q->symbols_uc);
  }
  if (q->tmp) {
    free(q->tmp);
  }
  delete_viterbi37_port(q->ptr);
}

int init37(srslte_viterbi_t *q, int poly[3], uint32_t framebits, bool tail_biting) {
  q->K = 7;
  q->R = 3;
  q->framebits = framebits;
  q->gain_quant_s = 4; 
  q->gain_quant = DEFAULT_GAIN; 
  q->tail_biting = tail_biting;
  q->decode = decode37;
  q->free = free37;
  q->decode_f = NULL;
  q->symbols_uc = srslte_vec_malloc(3 * (q->framebits + q->K - 1) * sizeof(uint8_t));
  if (!q->symbols_uc) {
    perror("malloc");
    return -1;
  }
  if (q->tail_biting) {
    q->tmp = srslte_vec_malloc(TB_ITER * 3 * (q->framebits + q->K - 1) * sizeof(uint8_t));
    bzero(q->tmp, 3 * (q->framebits + q->K - 1) * sizeof(uint8_t));
    if (!q->tmp) {
      perror("malloc");
      free37(q);
      return -1;
    }
  } else {
    q->tmp = NULL;
  }
  
  if ((q->ptr = create_viterbi37_port(poly, TB_ITER*framebits)) == NULL) {
    fprintf(stderr, "create_viterbi37 failed\n");
    free37(q);
    return -1;
  } else {
    return 0;
  }     
}

#ifdef LV_HAVE_SSE
int init37_sse(srslte_viterbi_t *q, int poly[3], uint32_t framebits, bool tail_biting) {
  q->K = 7;
  q->R = 3;
  q->framebits = framebits;
  q->gain_quant_s = 4; 
  q->gain_quant = DEFAULT_GAIN; 
  q->tail_biting = tail_biting;
  q->decode = decode37_sse;
  q->free = free37_sse;
  q->decode_f = NULL;
  q->symbols_uc = srslte_vec_malloc(3 * (q->framebits + q->K - 1) * sizeof(uint8_t));
  if (!q->symbols_uc) {
    perror("malloc");
    return -1;
  }
  if (q->tail_biting) {
    q->tmp = srslte_vec_malloc(TB_ITER*3*(q->framebits + q->K - 1) * sizeof(uint8_t));
    if (!q->tmp) {
      perror("malloc");
      free37(q);
      return -1;
    }
  } else {
    q->tmp = NULL;
  }
  
  if ((q->ptr = create_viterbi37_sse(poly, TB_ITER*framebits)) == NULL) {
    fprintf(stderr, "create_viterbi37 failed\n");
    free37(q);
    return -1;
  } else {
    return 0;
  }     
}
#endif

void srslte_viterbi_set_gain_quant(srslte_viterbi_t *q, float gain_quant) {
  q->gain_quant = gain_quant;
}

void srslte_viterbi_set_gain_quant_s(srslte_viterbi_t *q, int16_t gain_quant) {
  q->gain_quant_s = gain_quant;
}

int srslte_viterbi_init(srslte_viterbi_t *q, srslte_viterbi_type_t type, int poly[3], uint32_t max_frame_length, bool tail_bitting) 
{
  switch (type) {
  case SRSLTE_VITERBI_37:
#ifdef LV_HAVE_SSE
    return init37_sse(q, poly, max_frame_length, tail_bitting);
#else
    return init37(q, poly, max_frame_length, tail_bitting);
#endif
  default:
    fprintf(stderr, "Decoder not implemented\n");
    return -1;
  }
}

#ifdef LV_HAVE_SSE
int srslte_viterbi_init_sse(srslte_viterbi_t *q, srslte_viterbi_type_t type, int poly[3], uint32_t max_frame_length, bool tail_bitting) 
{
  return init37_sse(q, poly, max_frame_length, tail_bitting);      
}
#endif

void srslte_viterbi_free(srslte_viterbi_t *q) {
  if (q->free) {
    q->free(q);    
  }
  bzero(q, sizeof(srslte_viterbi_t));
}

/* symbols are real-valued */
int srslte_viterbi_decode_f(srslte_viterbi_t *q, float *symbols, uint8_t *data, uint32_t frame_length) 
{
  uint32_t len;
  if (frame_length > q->framebits) {
    fprintf(stderr, "Initialized decoder for max frame length %d bits\n",
        q->framebits);
    return -1;
  }
  if (q->tail_biting) {
    len = 3 * frame_length;
  } else {
    len = 3 * (frame_length + q->K - 1);
  }
  if (!q->decode_f) {    
    srslte_vec_quant_fuc(symbols, q->symbols_uc, q->gain_quant, 127.5, 255, len);    
    return srslte_viterbi_decode_uc(q, q->symbols_uc, data, frame_length);    
  } else {
    return q->decode_f(q, symbols, data, frame_length);
  }  
}

/* symbols are int16 */
int srslte_viterbi_decode_s(srslte_viterbi_t *q, int16_t *symbols, uint8_t *data, uint32_t frame_length) 
{
  uint32_t len;
  if (frame_length > q->framebits) {
    fprintf(stderr, "Initialized decoder for max frame length %d bits\n",
        q->framebits);
    return -1;
  }
  if (q->tail_biting) {
    len = 3 * frame_length;
  } else {
    len = 3 * (frame_length + q->K - 1);
  }
  srslte_vec_quant_suc(symbols, q->symbols_uc, q->gain_quant_s, 127, 255, len);    
  return srslte_viterbi_decode_uc(q, q->symbols_uc, data, frame_length);    
}


int srslte_viterbi_decode_uc(srslte_viterbi_t *q, uint8_t *symbols, uint8_t *data, uint32_t frame_length) 
{
  return q->decode(q, symbols, data, frame_length);
}
