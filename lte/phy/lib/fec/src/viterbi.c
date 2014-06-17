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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "liblte/phy/utils/vector.h"
#include "liblte/phy/fec/viterbi.h"
#include "parity.h"
#include "viterbi37.h"
#include "viterbi39.h"

#define DEB 0

int decode37(void *o, unsigned char *symbols, char *data, int frame_length) {
  viterbi_t *q = o;
  int i;

  int best_state;

  if (frame_length > q->framebits) {
    fprintf(stderr, "Initialized decoder for max frame length %d bits\n",
        q->framebits);
    return -1;
  }

  /* Initialize Viterbi decoder */
  init_viterbi37_port(q->ptr, q->tail_biting ? -1 : 0);

  /* Decode block */
  if (q->tail_biting) {
    memcpy(q->tmp, symbols, 3 * frame_length * sizeof(char));
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

int decode39(void *o, unsigned char *symbols, char *data, int frame_length) {
  viterbi_t *q = o;

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
  viterbi_t *q = o;
  if (q->symbols_uc) {
    free(q->symbols_uc);
  }
  if (q->tmp) {
    free(q->tmp);
  }
  delete_viterbi37_port(q->ptr);
}

void free39(void *o) {
  viterbi_t *q = o;
  if (q->symbols_uc) {
    free(q->symbols_uc);
  }
  delete_viterbi39_port(q->ptr);
}

int init37(viterbi_t *q, int poly[3], int framebits, bool tail_biting) {
  q->K = 7;
  q->R = 3;
  q->framebits = framebits;
  q->tail_biting = tail_biting;
  q->decode = decode37;
  q->free = free37;
  q->symbols_uc = malloc(3 * (q->framebits + q->K - 1) * sizeof(char));
  if (!q->symbols_uc) {
    perror("malloc");
    return -1;
  }
  if (q->tail_biting) {
    q->tmp = malloc(3 * (q->framebits + q->K - 1) * sizeof(char));
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

int init39(viterbi_t *q, int poly[3], int framebits, bool tail_biting) {
  q->K = 9;
  q->R = 3;
  q->framebits = framebits;
  q->tail_biting = tail_biting;
  q->decode = decode39;
  q->free = free39;
  if (q->tail_biting) {
    fprintf(stderr,
        "Error: Tailbitting not supported in 1/3 K=9 decoder\n");
    return -1;
  }
  q->symbols_uc = malloc(3 * (q->framebits + q->K - 1) * sizeof(char));
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

int viterbi_init(viterbi_t *q, viterbi_type_t type, int poly[3],
    int max_frame_length, bool tail_bitting) {
  switch (type) {
  case viterbi_37:
    return init37(q, poly, max_frame_length, tail_bitting);
  case viterbi_39:
    return init39(q, poly, max_frame_length, tail_bitting);
  default:
    fprintf(stderr, "Decoder not implemented\n");
    return -1;
  }
}

void viterbi_free(viterbi_t *q) {
  q->free(q);
}

/* symbols are real-valued */
int viterbi_decode_f(viterbi_t *q, float *symbols, char *data, int frame_length) {
  int len;
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
  vec_quant_fuc(symbols, q->symbols_uc, 32, 127.5, 255, len);
  return q->decode(q, q->symbols_uc, data, frame_length);
}

int viterbi_decode_uc(viterbi_t *q, unsigned char *symbols, char *data,
    int frame_length) {
  return q->decode(q, symbols, data, frame_length);
}

int viterbi_initialize(viterbi_hl* h) {
  int poly[3];
  viterbi_type_t type;
  if (h->init.rate == 2) {
    if (h->init.constraint_length == 7) {
      type = viterbi_27;
    } else if (h->init.constraint_length == 9) {
      type = viterbi_29;
    } else {
      fprintf(stderr, "Unsupported decoder %d/%d\n", h->init.rate,
          h->init.constraint_length);
      return -1;
    }
  } else if (h->init.rate == 3) {
    if (h->init.constraint_length == 7) {
      type = viterbi_37;
    } else if (h->init.constraint_length == 9) {
      type = viterbi_39;
    } else {
      fprintf(stderr, "Unsupported decoder %d/%d\n", h->init.rate,
          h->init.constraint_length);
      return -1;
    }
  } else {
    fprintf(stderr, "Unsupported decoder %d/%d\n", h->init.rate,
        h->init.constraint_length);
    return -1;
  }
  poly[0] = h->init.generator_0;
  poly[1] = h->init.generator_1;
  poly[2] = h->init.generator_2;
  return viterbi_init(&h->obj, type, poly, h->init.frame_length,
      h->init.tail_bitting ? true : false);
}

int viterbi_work(viterbi_hl* hl) {
  if (hl->in_len != hl->init.frame_length) {
    fprintf(stderr, "Expected input length %d but got %d\n",
        hl->init.frame_length, hl->in_len);
    return -1;
  }
  return viterbi_decode_f(&hl->obj, hl->input, hl->output, hl->init.frame_length);
}

int viterbi_stop(viterbi_hl* h) {
  viterbi_free(&h->obj);
  return 0;
}
