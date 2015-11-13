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
#include "viterbi39.h"

#define DEB 0

int decode37(void *o, uint8_t *symbols, uint8_t *data, uint32_t frame_length) {
  srslte_viterbi_t *q = o;
  uint32_t i;

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
    memcpy(q->tmp, symbols, 3 * frame_length * sizeof(uint8_t));
    for (i = 0; i < 3 * (q->K - 1); i++) {
      q->tmp[i + 3 * frame_length] = q->tmp[i];
    }
  } else {
    q->tmp = symbols;
  }

  update_viterbi37_blk_port(q->ptr, q->tmp, frame_length + q->K - 1,
      q->tail_biting ? &best_state : NULL);

  /* Do Viterbi chainback */
  chainback_viterbi37_port(q->ptr, data, frame_length,
      q->tail_biting ? best_state : 0);

  return q->framebits;
}

int decode39(void *o, uint8_t *symbols, uint8_t *data, uint32_t frame_length) {
  srslte_viterbi_t *q = o;

  if (frame_length > q->framebits) {
    fprintf(stderr, "Initialized decoder for max frame length %d bits\n",
        q->framebits);
    return -1;
  }

  /* Initialize Viterbi decoder */
  init_viterbi39_port(q->ptr, 0);

  /* Decode block */
  update_viterbi39_blk_port(q->ptr, symbols, frame_length + q->K - 1);

  /* Do Viterbi chainback */
  chainback_viterbi39_port(q->ptr, data, frame_length, 0);

  return q->framebits;
}


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

void free39(void *o) {
  srslte_viterbi_t *q = o;
  if (q->symbols_uc) {
    free(q->symbols_uc);
  }
  delete_viterbi39_port(q->ptr);
}

int init37(srslte_viterbi_t *q, uint32_t poly[3], uint32_t framebits, bool tail_biting) {
  q->K = 7;
  q->R = 3;
  q->framebits = framebits;
  q->gain_quant = 32; 
  q->tail_biting = tail_biting;
  q->decode = decode37;
  q->free = free37;
  q->decode_f = NULL;
  q->symbols_uc = malloc(3 * (q->framebits + q->K - 1) * sizeof(uint8_t));
  if (!q->symbols_uc) {
    perror("malloc");
    return -1;
  }
  if (q->tail_biting) {
    q->tmp = malloc(3 * (q->framebits + q->K - 1) * sizeof(uint8_t));
    if (!q->tmp) {
      perror("malloc");
      free37(q);
      return -1;
    }
  } else {
    q->tmp = NULL;
  }

  if ((q->ptr = create_viterbi37_port(poly, framebits)) == NULL) {
    fprintf(stderr, "create_viterbi37 failed\n");
    free37(q);
    return -1;
  } else {
    return 0;
  }     
}

int init39(srslte_viterbi_t *q, uint32_t poly[3], uint32_t framebits, bool tail_biting) {
  q->K = 9;
  q->R = 3;
  q->framebits = framebits;
  q->tail_biting = tail_biting;
  q->gain_quant = 32; 
  q->decode = decode39;
  q->free = free39;
  q->decode_f = NULL;
  if (q->tail_biting) {
    fprintf(stderr,
        "Error: Tailbitting not supported in 1/3 K=9 decoder\n");
    return -1;
  }
  q->symbols_uc = malloc(3 * (q->framebits + q->K - 1) * sizeof(uint8_t));
  if (!q->symbols_uc) {
    perror("malloc");
    return -1;
  }
  if ((q->ptr = create_viterbi39_port(poly, framebits)) == NULL) {
    fprintf(stderr, "create_viterbi37 failed\n");
    free39(q);
    return -1;
  } else {
    return 0;
  }
}



void srslte_viterbi_set_gain_quant(srslte_viterbi_t *q, float gain_quant) {
  q->gain_quant = gain_quant;
}

int srslte_viterbi_init(srslte_viterbi_t *q, srslte_viterbi_type_t type, uint32_t poly[3],
    uint32_t max_frame_length, bool tail_bitting) {
  switch (type) {
  case SRSLTE_VITERBI_37:
    return init37(q, poly, max_frame_length, tail_bitting);
  case SRSLTE_VITERBI_39:
    return init39(q, poly, max_frame_length, tail_bitting);
  default:
    fprintf(stderr, "Decoder not implemented\n");
    return -1;
  }
}

void srslte_viterbi_free(srslte_viterbi_t *q) {
  if (q->free) {
    q->free(q);    
  }
  bzero(q, sizeof(srslte_viterbi_t));
}

/* symbols are real-valued */
int srslte_viterbi_decode_f(srslte_viterbi_t *q, float *symbols, uint8_t *data, uint32_t frame_length) {
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
    return q->decode(q, q->symbols_uc, data, frame_length);    
  } else {
    return q->decode_f(q, symbols, data, frame_length);
  }
  
  
}

int srslte_viterbi_decode_uc(srslte_viterbi_t *q, uint8_t *symbols, uint8_t *data,
    uint32_t frame_length) {
  return q->decode(q, symbols, data, frame_length);
}
