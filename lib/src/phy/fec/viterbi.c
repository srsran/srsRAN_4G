/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <string.h>

#include "parity.h"
#include "srslte/phy/fec/viterbi.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include "viterbi37.h"

#define DEB 0

#define TB_ITER 3

#define DEFAULT_GAIN 100

#define DEFAULT_GAIN_16 1000
#define VITERBI_16

#ifndef LV_HAVE_AVX2
#undef VITERBI_16
#endif

//#undef LV_HAVE_SSE

int decode37(void* o, uint8_t* symbols, uint8_t* data, uint32_t frame_length)
{
  srslte_viterbi_t* q = o;

  uint32_t best_state;

  if (frame_length > q->framebits) {
    fprintf(stderr, "Initialized decoder for max frame length %d bits\n", q->framebits);
    return -1;
  }

  /* Initialize Viterbi decoder */
  init_viterbi37_port(q->ptr, q->tail_biting ? -1 : 0);

  /* Decode block */
  if (q->tail_biting) {
    for (int i = 0; i < TB_ITER; i++) {
      memcpy(&q->tmp[i * 3 * frame_length], symbols, 3 * frame_length * sizeof(uint8_t));
    }
    update_viterbi37_blk_port(q->ptr, q->tmp, TB_ITER * frame_length, &best_state);
    chainback_viterbi37_port(q->ptr, q->tmp, TB_ITER * frame_length, best_state);
    memcpy(data, &q->tmp[((int)(TB_ITER / 2)) * frame_length], frame_length * sizeof(uint8_t));
  } else {
    update_viterbi37_blk_port(q->ptr, symbols, frame_length + q->K - 1, NULL);
    chainback_viterbi37_port(q->ptr, data, frame_length, 0);
  }

  return q->framebits;
}

#ifdef LV_HAVE_SSE
int decode37_sse(void* o, uint8_t* symbols, uint8_t* data, uint32_t frame_length)
{
  srslte_viterbi_t* q = o;

  uint32_t best_state;

  if (frame_length > q->framebits) {
    fprintf(stderr, "Initialized decoder for max frame length %d bits\n", q->framebits);
    return -1;
  }

  /* Initialize Viterbi decoder */
  init_viterbi37_sse(q->ptr, q->tail_biting ? -1 : 0);

  /* Decode block */
  if (q->tail_biting) {
    for (int i = 0; i < TB_ITER; i++) {
      memcpy(&q->tmp[i * 3 * frame_length], symbols, 3 * frame_length * sizeof(uint8_t));
    }
    update_viterbi37_blk_sse(q->ptr, q->tmp, TB_ITER * frame_length, &best_state);
    chainback_viterbi37_sse(q->ptr, q->tmp, TB_ITER * frame_length, best_state);
    memcpy(data, &q->tmp[((int)(TB_ITER / 2)) * frame_length], frame_length * sizeof(uint8_t));
  } else {
    update_viterbi37_blk_sse(q->ptr, symbols, frame_length + q->K - 1, NULL);
    chainback_viterbi37_sse(q->ptr, data, frame_length, 0);
  }

  return q->framebits;
}

void free37_sse(void* o)
{
  srslte_viterbi_t* q = o;
  if (q->symbols_uc) {
    free(q->symbols_uc);
  }
  if (q->symbols_us) {
    free(q->symbols_us);
  }
  if (q->tmp) {
    free(q->tmp);
  }
  delete_viterbi37_sse(q->ptr);
}

#endif

#ifdef LV_HAVE_AVX2
int decode37_avx2_16bit(void* o, uint16_t* symbols, uint8_t* data, uint32_t frame_length)
{
  srslte_viterbi_t* q = o;

  uint32_t best_state;

  if (frame_length > q->framebits) {
    fprintf(stderr, "Initialized decoder for max frame length %d bits\n", q->framebits);
    return -1;
  }

  /* Initialize Viterbi decoder */
  init_viterbi37_avx2_16bit(q->ptr, q->tail_biting ? -1 : 0);

  /* Decode block */
  if (q->tail_biting) {
    for (int i = 0; i < TB_ITER; i++) {
      memcpy(&q->tmp_s[i * 3 * frame_length], symbols, 3 * frame_length * sizeof(uint16_t));
    }
    update_viterbi37_blk_avx2_16bit(q->ptr, q->tmp_s, TB_ITER * frame_length, &best_state);
    chainback_viterbi37_avx2_16bit(q->ptr, q->tmp, TB_ITER * frame_length, best_state);
    memcpy(data, &q->tmp[((int)(TB_ITER / 2)) * frame_length], frame_length * sizeof(uint8_t));
  } else {
    update_viterbi37_blk_avx2_16bit(q->ptr, symbols, frame_length + q->K - 1, NULL);
    chainback_viterbi37_avx2_16bit(q->ptr, data, frame_length, 0);
  }

  return q->framebits;
}

void free37_avx2_16bit(void* o)
{
  srslte_viterbi_t* q = o;

  if (q->symbols_uc) {
    free(q->symbols_uc);
  }
  if (q->symbols_us) {
    free(q->symbols_us);
  }
  if (q->tmp) {
    free(q->tmp);
  }
  if (q->tmp_s) {
    free(q->tmp_s);
  }
  delete_viterbi37_avx2_16bit(q->ptr);
}

int decode37_avx2(void* o, uint8_t* symbols, uint8_t* data, uint32_t frame_length)
{
  srslte_viterbi_t* q = o;

  uint32_t best_state;

  if (frame_length > q->framebits) {
    fprintf(stderr, "Initialized decoder for max frame length %d bits\n", q->framebits);
    return -1;
  }
  /* Initialize Viterbi decoder */
  init_viterbi37_avx2(q->ptr, q->tail_biting ? -1 : 0);
  /* Decode block */
  if (q->tail_biting) {
    for (int i = 0; i < TB_ITER; i++) {
      memcpy(&q->tmp[i * 3 * frame_length], symbols, 3 * frame_length * sizeof(uint8_t));
    }
    update_viterbi37_blk_avx2(q->ptr, q->tmp, TB_ITER * frame_length, &best_state);
    chainback_viterbi37_avx2(q->ptr, q->tmp, TB_ITER * frame_length, best_state);
    memcpy(data, &q->tmp[((int)(TB_ITER / 2)) * frame_length], frame_length * sizeof(uint8_t));
  } else {
    update_viterbi37_blk_avx2(q->ptr, symbols, frame_length + q->K - 1, NULL);
    chainback_viterbi37_avx2(q->ptr, data, frame_length, 0);
  }

  return q->framebits;
}

void free37_avx2(void* o)
{
  srslte_viterbi_t* q = o;
  if (q->symbols_uc) {
    free(q->symbols_uc);
  }
  if (q->tmp) {
    free(q->tmp);
  }
  delete_viterbi37_avx2(q->ptr);
}

#endif

#ifdef HAVE_NEON
int decode37_neon(void* o, uint8_t* symbols, uint8_t* data, uint32_t frame_length)
{
  srslte_viterbi_t* q = o;

  uint32_t best_state;

  if (frame_length > q->framebits) {
    ERROR("Initialized decoder for max frame length %d bits\n", q->framebits);
    return -1;
  }

  /* Initialize Viterbi decoder */
  init_viterbi37_neon(q->ptr, q->tail_biting ? -1 : 0);

  /* Decode block */
  if (q->tail_biting) {
    for (int i = 0; i < TB_ITER; i++) {
      memcpy(&q->tmp[i * 3 * frame_length], symbols, 3 * frame_length * sizeof(uint8_t));
    }
    update_viterbi37_blk_neon(q->ptr, q->tmp, TB_ITER * frame_length, &best_state);
    chainback_viterbi37_neon(q->ptr, q->tmp, TB_ITER * frame_length, best_state);
    memcpy(data, &q->tmp[((int)(TB_ITER / 2)) * frame_length], frame_length * sizeof(uint8_t));
  } else {
    update_viterbi37_blk_neon(q->ptr, symbols, frame_length + q->K - 1, NULL);
    chainback_viterbi37_neon(q->ptr, data, frame_length, 0);
  }

  return q->framebits;
}

void free37_neon(void* o)
{
  srslte_viterbi_t* q = o;
  if (q->symbols_uc) {
    free(q->symbols_uc);
  }
  if (q->tmp) {
    free(q->tmp);
  }
  delete_viterbi37_neon(q->ptr);
}

#endif

void free37(void* o)
{
  srslte_viterbi_t* q = o;
  if (q->symbols_uc) {
    free(q->symbols_uc);
  }
  if (q->tmp) {
    free(q->tmp);
  }
  delete_viterbi37_port(q->ptr);
}

int init37(srslte_viterbi_t* q, int poly[3], uint32_t framebits, bool tail_biting)
{
  q->K            = 7;
  q->R            = 3;
  q->framebits    = framebits;
  q->gain_quant_s = 4;
  q->gain_quant   = DEFAULT_GAIN;
  q->tail_biting  = tail_biting;
  q->decode       = decode37;
  q->free         = free37;
  q->decode_f     = NULL;
  q->symbols_uc   = srslte_vec_u8_malloc(3 * (q->framebits + q->K - 1));
  if (!q->symbols_uc) {
    perror("malloc");
    return -1;
  }
  if (q->tail_biting) {
    q->tmp = srslte_vec_u8_malloc(TB_ITER * 3 * (q->framebits + q->K - 1));
    bzero(q->tmp, 3 * (q->framebits + q->K - 1) * sizeof(uint8_t));
    if (!q->tmp) {
      perror("malloc");
      free37(q);
      return -1;
    }
  } else {
    q->tmp = NULL;
  }

  if ((q->ptr = create_viterbi37_port(poly, TB_ITER * framebits)) == NULL) {
    ERROR("create_viterbi37 failed\n");
    free37(q);
    return -1;
  } else {
    return 0;
  }
}

#ifdef LV_HAVE_SSE
int init37_sse(srslte_viterbi_t* q, int poly[3], uint32_t framebits, bool tail_biting)
{
  q->K            = 7;
  q->R            = 3;
  q->framebits    = framebits;
  q->gain_quant_s = 4;
  q->gain_quant   = DEFAULT_GAIN;
  q->tail_biting  = tail_biting;
  q->decode       = decode37_sse;
  q->free         = free37_sse;
  q->decode_f     = NULL;
  q->symbols_uc   = srslte_vec_u8_malloc(3 * (q->framebits + q->K - 1));
  if (!q->symbols_uc) {
    perror("malloc");
    return -1;
  }
#ifdef VITERBI_16
  q->symbols_us = srslte_vec_u16_malloc(3 * (q->framebits + q->K - 1));
  if (!q->symbols_us) {
    perror("malloc");
    return -1;
  }
#endif
  if (q->tail_biting) {
    q->tmp = srslte_vec_u8_malloc(TB_ITER * 3 * (q->framebits + q->K - 1));
    if (!q->tmp) {
      perror("malloc");
      free37(q);
      return -1;
    }
  } else {
    q->tmp = NULL;
  }

  if ((q->ptr = create_viterbi37_sse(poly, TB_ITER * framebits)) == NULL) {
    ERROR("create_viterbi37 failed\n");
    free37(q);
    return -1;
  } else {
    return 0;
  }
}
#endif

#ifdef HAVE_NEON
int init37_neon(srslte_viterbi_t* q, int poly[3], uint32_t framebits, bool tail_biting)
{
  q->K            = 7;
  q->R            = 3;
  q->framebits    = framebits;
  q->gain_quant_s = 4;
  q->gain_quant   = DEFAULT_GAIN;
  q->tail_biting  = tail_biting;
  q->decode       = decode37_neon;
  q->free         = free37_neon;
  q->decode_f     = NULL;
  q->symbols_uc   = srslte_vec_u8_malloc(3 * (q->framebits + q->K - 1));
  if (!q->symbols_uc) {
    perror("malloc");
    return -1;
  }
  if (q->tail_biting) {
    q->tmp = srslte_vec_u8_malloc(TB_ITER * 3 * (q->framebits + q->K - 1));
    if (!q->tmp) {
      perror("malloc");
      free37(q);
      return -1;
    }
  } else {
    q->tmp = NULL;
  }

  if ((q->ptr = create_viterbi37_neon(poly, TB_ITER * framebits)) == NULL) {
    ERROR("create_viterbi37 failed\n");
    free37(q);
    return -1;
  } else {
    return 0;
  }
}
#endif

#ifdef LV_HAVE_AVX2
int init37_avx2(srslte_viterbi_t* q, int poly[3], uint32_t framebits, bool tail_biting)
{
  q->K            = 7;
  q->R            = 3;
  q->framebits    = framebits;
  q->gain_quant_s = 4;
  q->gain_quant   = DEFAULT_GAIN;
  q->tail_biting  = tail_biting;
  q->decode       = decode37_avx2;
  q->free         = free37_avx2;
  q->decode_f     = NULL;
  q->symbols_uc   = srslte_vec_u8_malloc(3 * (q->framebits + q->K - 1));
  if (!q->symbols_uc) {
    perror("malloc");
    return -1;
  }
  if (q->tail_biting) {
    q->tmp = srslte_vec_u8_malloc(TB_ITER * 3 * (q->framebits + q->K - 1));
    if (!q->tmp) {
      perror("malloc");
      free37(q);
      return -1;
    }
  } else {
    q->tmp = NULL;
  }

  if ((q->ptr = create_viterbi37_avx2(poly, TB_ITER * framebits)) == NULL) {
    ERROR("create_viterbi37 failed\n");
    free37(q);
    return -1;
  } else {
    return 0;
  }
}

int init37_avx2_16bit(srslte_viterbi_t* q, int poly[3], uint32_t framebits, bool tail_biting)
{
  q->K            = 7;
  q->R            = 3;
  q->framebits    = framebits;
  q->gain_quant_s = 4;
  q->gain_quant   = DEFAULT_GAIN_16;
  q->tail_biting  = tail_biting;
  q->decode_s     = decode37_avx2_16bit;
  q->free         = free37_avx2_16bit;
  q->decode_f     = NULL;
  q->symbols_uc   = srslte_vec_u8_malloc(3 * (q->framebits + q->K - 1));
  q->symbols_us   = srslte_vec_u16_malloc(3 * (q->framebits + q->K - 1));
  if (!q->symbols_uc || !q->symbols_us) {
    perror("malloc");
    return -1;
  }
  if (q->tail_biting) {
    q->tmp   = srslte_vec_u8_malloc(TB_ITER * 3 * (q->framebits + q->K - 1));
    q->tmp_s = srslte_vec_u16_malloc(TB_ITER * 3 * (q->framebits + q->K - 1));
    if (!q->tmp) {
      perror("malloc");
      free37(q);
      return -1;
    }
  } else {
    q->tmp = NULL;
  }
  // printf("pt0\n");
  if ((q->ptr = create_viterbi37_avx2_16bit(poly, TB_ITER * framebits)) == NULL) {
    ERROR("create_viterbi37 failed\n");
    free37(q);
    return -1;
  } else {
    return 0;
  }
}

#endif

void srslte_viterbi_set_gain_quant(srslte_viterbi_t* q, float gain_quant)
{
  q->gain_quant = gain_quant;
}

void srslte_viterbi_set_gain_quant_s(srslte_viterbi_t* q, int16_t gain_quant)
{
  q->gain_quant_s = gain_quant;
}

int srslte_viterbi_init(srslte_viterbi_t*     q,
                        srslte_viterbi_type_t type,
                        int                   poly[3],
                        uint32_t              max_frame_length,
                        bool                  tail_bitting)
{
  bzero(q, sizeof(srslte_viterbi_t));
  switch (type) {
    case SRSLTE_VITERBI_37:
#ifdef LV_HAVE_SSE

#ifdef LV_HAVE_AVX2
#ifdef VITERBI_16
      return init37_avx2_16bit(q, poly, max_frame_length, tail_bitting);
#else
      return init37_avx2(q, poly, max_frame_length, tail_bitting);
#endif
#else
      return init37_sse(q, poly, max_frame_length, tail_bitting);
#endif
#else
#ifdef HAVE_NEON
      return init37_neon(q, poly, max_frame_length, tail_bitting);
#else
      return init37(q, poly, max_frame_length, tail_bitting);
#endif
#endif
    default:
      ERROR("Decoder not implemented\n");
      return -1;
  }
}

#ifdef LV_HAVE_SSE
int srslte_viterbi_init_sse(srslte_viterbi_t*     q,
                            srslte_viterbi_type_t type,
                            int                   poly[3],
                            uint32_t              max_frame_length,
                            bool                  tail_bitting)
{
  return init37_sse(q, poly, max_frame_length, tail_bitting);
}
#endif

#ifdef LV_HAVE_AVX2
int srslte_viterbi_init_avx2(srslte_viterbi_t*     q,
                             srslte_viterbi_type_t type,
                             int                   poly[3],
                             uint32_t              max_frame_length,
                             bool                  tail_bitting)
{
  return init37_avx2(q, poly, max_frame_length, tail_bitting);
}
#endif

void srslte_viterbi_free(srslte_viterbi_t* q)
{
  if (q->free) {
    q->free(q);
  }
  bzero(q, sizeof(srslte_viterbi_t));
}

/* symbols are real-valued */
int srslte_viterbi_decode_f(srslte_viterbi_t* q, float* symbols, uint8_t* data, uint32_t frame_length)
{
  uint32_t len;
  if (frame_length > q->framebits) {
    ERROR("Initialized decoder for max frame length %d bits\n", q->framebits);
    return -1;
  }
  if (q->tail_biting) {
    len = 3 * frame_length;
  } else {
    len = 3 * (frame_length + q->K - 1);
  }
  if (!q->decode_f) {
    float max = 1e-9;
    for (int i = 0; i < len; i++) {
      if (fabs(symbols[i]) > max) {
        max = fabs(symbols[i]);
      }
    }
#ifdef VITERBI_16
    srslte_vec_quant_fus(symbols, q->symbols_us, q->gain_quant / max, 32767.5, 65535, len);
    return srslte_viterbi_decode_us(q, q->symbols_us, data, frame_length);
#else
    srslte_vec_quant_fuc(symbols, q->symbols_uc, q->gain_quant / max, 127.5, 255, len);
    return srslte_viterbi_decode_uc(q, q->symbols_uc, data, frame_length);
#endif
  } else {
    return q->decode_f(q, symbols, data, frame_length);
  }
}

/* symbols are int16 */
int srslte_viterbi_decode_s(srslte_viterbi_t* q, int16_t* symbols, uint8_t* data, uint32_t frame_length)
{
  uint32_t len;
  if (frame_length > q->framebits) {
    ERROR("Initialized decoder for max frame length %d bits\n", q->framebits);
    return -1;
  }
  if (q->tail_biting) {
    len = 3 * frame_length;
  } else {
    len = 3 * (frame_length + q->K - 1);
  }

  int16_t max = -INT16_MAX;
  for (int i = 0; i < len; i++) {
    if (abs(symbols[i]) > max) {
      max = abs(symbols[i]);
    }
  }
#ifdef VITERBI_16
  srslte_vec_quant_sus(symbols, q->symbols_us, 1, (float)INT16_MAX, UINT16_MAX, len);
  return srslte_viterbi_decode_us(q, q->symbols_us, data, frame_length);
#else
  srslte_vec_quant_suc(symbols, q->symbols_uc, (float)q->gain_quant / max, 127, 255, len);
  return srslte_viterbi_decode_uc(q, q->symbols_uc, data, frame_length);
#endif
}

int srslte_viterbi_decode_us(srslte_viterbi_t* q, uint16_t* symbols, uint8_t* data, uint32_t frame_length)
{
  int ret = SRSLTE_ERROR;

  if (q && q->decode_s) {
    ret = q->decode_s(q, symbols, data, frame_length);
  }

  return ret;
}

int srslte_viterbi_decode_uc(srslte_viterbi_t* q, uint8_t* symbols, uint8_t* data, uint32_t frame_length)
{
  int ret = SRSLTE_ERROR;

  if (q && q->decode) {
    ret = q->decode(q, symbols, data, frame_length);
  }

  return ret;
}
