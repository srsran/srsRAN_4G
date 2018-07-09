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

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef LV_HAVE_SSE

#include <immintrin.h>

#endif /* LV_HAVE_SSE */

#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/vector.h"

void srslte_bit_interleaver_init(srslte_bit_interleaver_t *q,
                                 uint16_t *interleaver,
                                 uint32_t nof_bits) {
  static const uint8_t mask[] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1 };

  bzero(q, sizeof(srslte_bit_interleaver_t));

  q->interleaver = srslte_vec_malloc(sizeof(uint16_t)*nof_bits);
  q->byte_idx = srslte_vec_malloc(sizeof(uint16_t)*nof_bits);
  q->bit_mask = srslte_vec_malloc(sizeof(uint8_t)*nof_bits);
  q->nof_bits = nof_bits;

  for (int i = 0; i < nof_bits; i++) {
    uint16_t i_px = interleaver[i];
    q->interleaver[i] = i_px;
    q->byte_idx[i] = (uint16_t) (interleaver[i] / 8);
    q->bit_mask[i] = (uint8_t) (mask[i_px%8]);
  }
}

void srslte_bit_interleaver_free(srslte_bit_interleaver_t *q) {
  if (q->interleaver) {
    free(q->interleaver);
  }

  if (q->byte_idx) {
    free(q->byte_idx);
  }

  if (q->bit_mask) {
    free(q->bit_mask);
  }

  bzero(q, sizeof(srslte_bit_interleaver_t));
}

void srslte_bit_interleaver_run(srslte_bit_interleaver_t *q, uint8_t *input, uint8_t *output, uint16_t w_offset) {
  static const uint8_t mask[] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1 };
  uint16_t *byte_idx = q->byte_idx;
  uint8_t *bit_mask = q->bit_mask;
  uint8_t *output_ptr = output;

  uint32_t st=0, w_offset_p=0;

  if (w_offset < 8 && w_offset > 0) {
    st=1;
    for (uint32_t j=0;j<8-w_offset;j++) {
      uint16_t i_p = q->interleaver[j];
      if (input[i_p/8] & mask[i_p%8]) {
        output[0] |= mask[j+w_offset];
      } else {
        output[0] &= ~(mask[j+w_offset]);
      }
    }
    w_offset_p=8-w_offset;
  }

  int i = st * 8;

  byte_idx += i - w_offset_p;
  bit_mask += i - w_offset_p;
  output_ptr += st;

#ifdef LV_HAVE_SSE
  for(; i < (int) q->nof_bits - 15; i += 16) {
    __m128i in128;
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0x7);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0x6);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0x5);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0x4);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0x3);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0x2);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0x1);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0x0);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0xF);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0xE);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0xD);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0xC);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0xB);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0xA);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0x9);
    in128 = _mm_insert_epi8(in128, input[*(byte_idx++)], 0x8);

    __m128i mask128 = _mm_loadu_si128((__m128i *) bit_mask);
    mask128 = _mm_shuffle_epi8(mask128, _mm_set_epi8(0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
                                                     0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7));

    __m128i cmp128 = _mm_cmpeq_epi8(_mm_and_si128(in128, mask128), mask128);
    *((uint16_t *) (output_ptr)) = (uint16_t) _mm_movemask_epi8(cmp128);

    bit_mask += 16;
    output_ptr += 2;
  }

#endif /* LV_HAVE_SSE */

  for(; i < (int) q->nof_bits - 7; i += 8) {
    uint8_t out0  = (input[*(byte_idx++)] & *(bit_mask++))?mask[0]:(uint8_t)0;
    uint8_t out1  = (input[*(byte_idx++)] & *(bit_mask++))?mask[1]:(uint8_t)0;
    uint8_t out2  = (input[*(byte_idx++)] & *(bit_mask++))?mask[2]:(uint8_t)0;
    uint8_t out3  = (input[*(byte_idx++)] & *(bit_mask++))?mask[3]:(uint8_t)0;
    uint8_t out4  = (input[*(byte_idx++)] & *(bit_mask++))?mask[4]:(uint8_t)0;
    uint8_t out5  = (input[*(byte_idx++)] & *(bit_mask++))?mask[5]:(uint8_t)0;
    uint8_t out6  = (input[*(byte_idx++)] & *(bit_mask++))?mask[6]:(uint8_t)0;
    uint8_t out7  = (input[*(byte_idx++)] & *(bit_mask++))?mask[7]:(uint8_t)0;

    *output_ptr = out0 | out1 | out2 | out3 | out4 | out5 | out6 | out7;
    output_ptr++;
  }

  for (uint32_t j=0;j<q->nof_bits%8;j++) {
    uint16_t i_p = q->interleaver[(q->nof_bits/8)*8+j-w_offset];
    if (input[i_p/8] & mask[i_p%8]) {
      output[q->nof_bits/8] |= mask[j];
    } else {
      output[q->nof_bits/8] &= ~(mask[j]);
    }
  }
  for (uint32_t j=0;j<w_offset;j++) {
    uint16_t i_p = q->interleaver[(q->nof_bits/8)*8+j-w_offset];
    if (input[i_p/8] & (1<<(7-i_p%8))) {
      output[q->nof_bits/8] |= mask[j];
    } else {
      output[q->nof_bits/8] &= ~(mask[j]);
    }
  }

#if 0
  /* THIS PIECE OF CODE IS FOR CHECKING SIMD BEHAVIOUR. DO NOT ENABLE. */
  uint8_t *output2 = malloc(q->nof_bits/8);
  for (i=st;i<q->nof_bits/8;i++) {

    uint16_t i_p0 = q->interleaver[i*8+0-w_offset_p];
    uint16_t i_p1 = q->interleaver[i*8+1-w_offset_p];
    uint16_t i_p2 = q->interleaver[i*8+2-w_offset_p];
    uint16_t i_p3 = q->interleaver[i*8+3-w_offset_p];
    uint16_t i_p4 = q->interleaver[i*8+4-w_offset_p];
    uint16_t i_p5 = q->interleaver[i*8+5-w_offset_p];
    uint16_t i_p6 = q->interleaver[i*8+6-w_offset_p];
    uint16_t i_p7 = q->interleaver[i*8+7-w_offset_p];

    uint8_t out0  = (input[i_p0/8] & mask[i_p0%8])?mask[0]:(uint8_t)0;
    uint8_t out1  = (input[i_p1/8] & mask[i_p1%8])?mask[1]:(uint8_t)0;
    uint8_t out2  = (input[i_p2/8] & mask[i_p2%8])?mask[2]:(uint8_t)0;
    uint8_t out3  = (input[i_p3/8] & mask[i_p3%8])?mask[3]:(uint8_t)0;
    uint8_t out4  = (input[i_p4/8] & mask[i_p4%8])?mask[4]:(uint8_t)0;
    uint8_t out5  = (input[i_p5/8] & mask[i_p5%8])?mask[5]:(uint8_t)0;
    uint8_t out6  = (input[i_p6/8] & mask[i_p6%8])?mask[6]:(uint8_t)0;
    uint8_t out7  = (input[i_p7/8] & mask[i_p7%8])?mask[7]:(uint8_t)0;

    output2[i] = out0 | out1 | out2 | out3 | out4 | out5 | out6 | out7;
  }

  for(i = st; i < q->nof_bits/8; i++) {
    if (true || output[i] != output2[i]) {
      printf("%05d/%05d %02X %02X\n", i, q->nof_bits/8, output[i], output2[i]);
    }
    //output[i] = output2[i];
  }
  free(output2);
#endif
}


void srslte_bit_interleave_i(uint8_t *input, uint8_t *output, uint32_t *interleaver, uint32_t nof_bits) {
  srslte_bit_interleave_i_w_offset(input, output, interleaver, nof_bits, 0);
}

void srslte_bit_interleave_i_w_offset(uint8_t *input, uint8_t *output, uint32_t *interleaver, uint32_t nof_bits, uint32_t w_offset) {
  uint32_t st=0, w_offset_p=0;
  static const uint8_t mask[] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1 };

  if (w_offset < 8 && w_offset > 0) {
    st=1;
    for (uint32_t j=0;j<8-w_offset;j++) {
      uint32_t i_p = interleaver[j];
      if (input[i_p/8] & mask[i_p%8]) {
        output[0] |= mask[j+w_offset];
      } else {
        output[0] &= ~(mask[j+w_offset]);
      }
    }
    w_offset_p=8-w_offset;
  }

#ifdef LV_HAVE_SSE
  __m64 m64mask = _mm_setr_pi8((uint8_t) 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1);

  union {
    uint8_t v[8];
    __m64 m64;
  } a, b, c;

  union {
    __m128i m128;
    uint16_t u32[4];
    uint16_t u16[8];
    uint8_t u8[16];
    struct {
      __m64 reg_a;
      __m64 reg_b;
    } m64;
    struct {
      uint16_t i0, i1, i2, i3, i4, i5, i6, i7;
    } v16;
    struct {
      uint32_t i0, i1, i2, i3;
    } v32;
  } ipx1, ipx2, epx1, epx2;
  for (uint32_t i = st; i < nof_bits / 8; i++) {
    ipx1.m128 = _mm_loadu_si128((__m128i *) (interleaver + (i * 8 + 0) - w_offset_p));
    epx1.m128 = _mm_shuffle_epi8(ipx1.m128, _mm_set_epi8(0x00, 0x04, 0x08, 0x0C,
                                                         0x00, 0x04, 0x08, 0x0C,
                                                         0x00, 0x04, 0x08, 0x0C,
                                                         0x00, 0x04, 0x08, 0x0C));
    ipx2.m128 = _mm_loadu_si128((__m128i *) (interleaver + (i * 8 + 4) - w_offset_p));
    epx2.m128 = _mm_shuffle_epi8(ipx2.m128, _mm_set_epi8(0x00, 0x04, 0x08, 0x0C,
                                                         0x00, 0x04, 0x08, 0x0C,
                                                         0x00, 0x04, 0x08, 0x0C,
                                                         0x00, 0x04, 0x08, 0x0C));

    epx1.m128 = _mm_blendv_epi8(epx2.m128, epx1.m128, _mm_setr_epi8(+1, +1, +1, +1,
                                                                    -1, -1, -1, -1,
                                                                    +1, +1, +1, +1,
                                                                    -1, -1, -1, -1));

    b.m64 = _mm_and_si64(epx1.m64.reg_a, _mm_set1_pi8(0x7));
    b.m64 = _mm_shuffle_pi8(m64mask, b.m64);

    ipx1.m128 = _mm_srli_epi32(ipx1.m128, 3);
    ipx2.m128 = _mm_srli_epi32(ipx2.m128, 3);

    a.m64 = _mm_set_pi8(input[ipx1.v32.i0],
                        input[ipx1.v32.i1],
                        input[ipx1.v32.i2],
                        input[ipx1.v32.i3],
                        input[ipx2.v32.i0],
                        input[ipx2.v32.i1],
                        input[ipx2.v32.i2],
                        input[ipx2.v32.i3]);

    c.m64 = _mm_cmpeq_pi8(_mm_and_si64(a.m64, b.m64), b.m64);
    output[i] = (uint8_t) _mm_movemask_pi8(c.m64);
  }

#if 0 /* Disabled */
  /* THIS PIECE OF CODE IS FOR CHECKING SIMD BEHAVIOUR. DO NOT ENABLE. */
  uint8_t *output2 = malloc(nof_bits/8);
  for (uint32_t i=st;i<nof_bits/8;i++) {

    uint16_t i_p0 = interleaver[i*8+0-w_offset_p];
    uint16_t i_p1 = interleaver[i*8+1-w_offset_p];
    uint16_t i_p2 = interleaver[i*8+2-w_offset_p];
    uint16_t i_p3 = interleaver[i*8+3-w_offset_p];
    uint16_t i_p4 = interleaver[i*8+4-w_offset_p];
    uint16_t i_p5 = interleaver[i*8+5-w_offset_p];
    uint16_t i_p6 = interleaver[i*8+6-w_offset_p];
    uint16_t i_p7 = interleaver[i*8+7-w_offset_p];

    uint8_t out0  = (input[i_p0/8] & mask[i_p0%8])?mask[0]:(uint8_t)0;
    uint8_t out1  = (input[i_p1/8] & mask[i_p1%8])?mask[1]:(uint8_t)0;
    uint8_t out2  = (input[i_p2/8] & mask[i_p2%8])?mask[2]:(uint8_t)0;
    uint8_t out3  = (input[i_p3/8] & mask[i_p3%8])?mask[3]:(uint8_t)0;
    uint8_t out4  = (input[i_p4/8] & mask[i_p4%8])?mask[4]:(uint8_t)0;
    uint8_t out5  = (input[i_p5/8] & mask[i_p5%8])?mask[5]:(uint8_t)0;
    uint8_t out6  = (input[i_p6/8] & mask[i_p6%8])?mask[6]:(uint8_t)0;
    uint8_t out7  = (input[i_p7/8] & mask[i_p7%8])?mask[7]:(uint8_t)0;

    output2[i] = out0 | out1 | out2 | out3 | out4 | out5 | out6 | out7;
  }

  for(uint32_t i = st; i < nof_bits/8; i++) {
    if (output[i] != output2[i]) {
      printf("%05d/%05d %02X %02X\n", i, nof_bits/8, output[i], output2[i]);
    }
    //output[i] = output2[i];
  }
  free(output2);
#endif /* Disabled */
#else /* LV_HAVE_SSE */
  for (uint32_t i=st;i<nof_bits/8;i++) {

    uint32_t i_p0 = interleaver[i*8+0-w_offset_p];
    uint32_t i_p1 = interleaver[i*8+1-w_offset_p];
    uint32_t i_p2 = interleaver[i*8+2-w_offset_p];
    uint32_t i_p3 = interleaver[i*8+3-w_offset_p];
    uint32_t i_p4 = interleaver[i*8+4-w_offset_p];
    uint32_t i_p5 = interleaver[i*8+5-w_offset_p];
    uint32_t i_p6 = interleaver[i*8+6-w_offset_p];
    uint32_t i_p7 = interleaver[i*8+7-w_offset_p];

    uint8_t out0  = (input[i_p0/8] & mask[i_p0%8])?mask[0]:0;
    uint8_t out1  = (input[i_p1/8] & mask[i_p1%8])?mask[1]:0;
    uint8_t out2  = (input[i_p2/8] & mask[i_p2%8])?mask[2]:0;
    uint8_t out3  = (input[i_p3/8] & mask[i_p3%8])?mask[3]:0;
    uint8_t out4  = (input[i_p4/8] & mask[i_p4%8])?mask[4]:0;
    uint8_t out5  = (input[i_p5/8] & mask[i_p5%8])?mask[5]:0;
    uint8_t out6  = (input[i_p6/8] & mask[i_p6%8])?mask[6]:0;
    uint8_t out7  = (input[i_p7/8] & mask[i_p7%8])?mask[7]:0;

    output[i] = out0 | out1 | out2 | out3 | out4 | out5 | out6 | out7;
  }
#endif /* LV_HAVE_SSE */

  for (uint32_t j=0;j<nof_bits%8;j++) {
    uint32_t i_p = interleaver[(nof_bits/8)*8+j-w_offset];
    if (input[i_p/8] & mask[i_p%8]) {
      output[nof_bits/8] |= mask[j];
    } else {
      output[nof_bits/8] &= ~(mask[j]);
    }
  }
  for (uint32_t j=0;j<w_offset;j++) {
    uint32_t i_p = interleaver[(nof_bits/8)*8+j-w_offset];
    if (input[i_p/8] & (1<<(7-i_p%8))) {
      output[nof_bits/8] |= mask[j];
    } else {
      output[nof_bits/8] &= ~(mask[j]);
    }
  }
}

void srslte_bit_interleave(uint8_t *input, uint8_t *output, uint16_t *interleaver, uint32_t nof_bits) {
  srslte_bit_interleave_w_offset(input, output, interleaver, nof_bits, 0);
}

void srslte_bit_interleave_w_offset(uint8_t *input, uint8_t *output, uint16_t *interleaver, uint32_t nof_bits, uint32_t w_offset) {
  uint32_t st=0, w_offset_p=0;
  static const uint8_t mask[] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1 };

  if (w_offset < 8 && w_offset > 0) {
    st=1;
    for (uint32_t j=0;j<8-w_offset;j++) {
      uint16_t i_p = interleaver[j];            
      if (input[i_p/8] & mask[i_p%8]) {
        output[0] |= mask[j+w_offset];
      } else {
        output[0] &= ~(mask[j+w_offset]);
      }
    }
    w_offset_p=8-w_offset;
  }
#ifdef LV_HAVE_SSE
  __m64 m64mask = _mm_setr_pi8((uint8_t) 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1);
  __m128i m128mask = _mm_set1_epi64(m64mask);

  union {
    uint8_t v[8];
    __m64 m64;
  } a, b, c;

  union {
    __m128i m128;
    uint16_t u16[8];
    uint8_t u8[16];
    struct {
      __m64 reg_a;
      __m64 reg_b;
    } m64;
    struct {
      uint16_t i0, i1, i2, i3, i4, i5, i6, i7;
    } v;
  } ipx, epx, ipx2, epx2, b128, a128, c128;

  uint32_t i = st;
  for (; i < (nof_bits / 8 - 1); i += 2) {
    ipx.m128 = _mm_loadu_si128((__m128i *) (interleaver + (i * 8) - w_offset_p));
    epx.m128 = _mm_shuffle_epi8(ipx.m128, _mm_set_epi8(0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E,
                                                       0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E));
    ipx2.m128 = _mm_loadu_si128((__m128i *) (interleaver + ((i + 1) * 8) - w_offset_p));
    epx2.m128 = _mm_shuffle_epi8(ipx2.m128, _mm_set_epi8(0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E,
                                                         0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E));

    epx.m128 = _mm_blendv_epi8(epx.m128, epx2.m128, _mm_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0,
                                                                  (uint8_t) 0xFF, (uint8_t) 0xFF,
                                                                  (uint8_t) 0xFF, (uint8_t) 0xFF,
                                                                  (uint8_t) 0xFF, (uint8_t) 0xFF,
                                                                  (uint8_t) 0xFF, (uint8_t) 0xFF));

    b128.m128 = _mm_and_si128(epx.m128, _mm_set1_epi8(0x7));
    b128.m128 = _mm_shuffle_epi8(m128mask, b128.m128);

    ipx.m128 = _mm_srli_epi16(ipx.m128, 3);
    ipx2.m128 = _mm_srli_epi16(ipx2.m128, 3);

    a128.m128 = _mm_set_epi8(input[ipx2.v.i0],
                             input[ipx2.v.i1],
                             input[ipx2.v.i2],
                             input[ipx2.v.i3],
                             input[ipx2.v.i4],
                             input[ipx2.v.i5],
                             input[ipx2.v.i6],
                             input[ipx2.v.i7],
                             input[ipx.v.i0],
                             input[ipx.v.i1],
                             input[ipx.v.i2],
                             input[ipx.v.i3],
                             input[ipx.v.i4],
                             input[ipx.v.i5],
                             input[ipx.v.i6],
                             input[ipx.v.i7]);

    c128.m128 = _mm_cmpeq_epi8(_mm_and_si128(a128.m128, b128.m128), b128.m128);
    uint16_t o = (uint16_t) _mm_movemask_epi8(c128.m128);
    *((uint16_t *) (output + i)) = o;
  }

  for (; i < nof_bits / 8; i++) {
    ipx.m128 = _mm_loadu_si128((__m128i *) (interleaver + i * 8 - w_offset_p));
    epx.m128 = _mm_shuffle_epi8(ipx.m128, _mm_set_epi8(0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E,
                                                       0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E));
    b.m64 = _mm_and_si64(epx.m64.reg_a, _mm_set1_pi8(0x7));
    b.m64 = _mm_shuffle_pi8(m64mask, b.m64);

    ipx.m128 = _mm_srli_epi16(ipx.m128, 3);

    a.m64 = _mm_set_pi8(input[ipx.v.i0],
                        input[ipx.v.i1],
                        input[ipx.v.i2],
                        input[ipx.v.i3],
                        input[ipx.v.i4],
                        input[ipx.v.i5],
                        input[ipx.v.i6],
                        input[ipx.v.i7]);

    c.m64 = _mm_cmpeq_pi8(_mm_and_si64(a.m64, b.m64), b.m64);
    output[i] = (uint8_t) _mm_movemask_pi8(c.m64);
  }

#if 0
  /* THIS PIECE OF CODE IS FOR CHECKING SIMD BEHAVIOUR. DO NOT ENABLE. */
  uint8_t *output2 = malloc(nof_bits/8);
  for (i=st;i<nof_bits/8;i++) {

    uint16_t i_p0 = interleaver[i*8+0-w_offset_p];
    uint16_t i_p1 = interleaver[i*8+1-w_offset_p];
    uint16_t i_p2 = interleaver[i*8+2-w_offset_p];
    uint16_t i_p3 = interleaver[i*8+3-w_offset_p];
    uint16_t i_p4 = interleaver[i*8+4-w_offset_p];
    uint16_t i_p5 = interleaver[i*8+5-w_offset_p];
    uint16_t i_p6 = interleaver[i*8+6-w_offset_p];
    uint16_t i_p7 = interleaver[i*8+7-w_offset_p];

    uint8_t out0  = (input[i_p0/8] & mask[i_p0%8])?mask[0]:(uint8_t)0;
    uint8_t out1  = (input[i_p1/8] & mask[i_p1%8])?mask[1]:(uint8_t)0;
    uint8_t out2  = (input[i_p2/8] & mask[i_p2%8])?mask[2]:(uint8_t)0;
    uint8_t out3  = (input[i_p3/8] & mask[i_p3%8])?mask[3]:(uint8_t)0;
    uint8_t out4  = (input[i_p4/8] & mask[i_p4%8])?mask[4]:(uint8_t)0;
    uint8_t out5  = (input[i_p5/8] & mask[i_p5%8])?mask[5]:(uint8_t)0;
    uint8_t out6  = (input[i_p6/8] & mask[i_p6%8])?mask[6]:(uint8_t)0;
    uint8_t out7  = (input[i_p7/8] & mask[i_p7%8])?mask[7]:(uint8_t)0;

    output2[i] = out0 | out1 | out2 | out3 | out4 | out5 | out6 | out7;
  }

  for(i = st; i < nof_bits/8; i++) {
    if (output[i] != output2[i]) {
      printf("%05d/%05d %02X %02X\n", i, nof_bits/8, output[i], output2[i]);
    }
    //output[i] = output2[i];
  }
  free(output2);
#endif

#else /* LV_HAVE_SSE */
  for (uint32_t i=st;i<nof_bits/8;i++) {
    
    uint16_t i_p0 = interleaver[i*8+0-w_offset_p];
    uint16_t i_p1 = interleaver[i*8+1-w_offset_p];      
    uint16_t i_p2 = interleaver[i*8+2-w_offset_p];      
    uint16_t i_p3 = interleaver[i*8+3-w_offset_p];      
    uint16_t i_p4 = interleaver[i*8+4-w_offset_p];      
    uint16_t i_p5 = interleaver[i*8+5-w_offset_p];      
    uint16_t i_p6 = interleaver[i*8+6-w_offset_p];      
    uint16_t i_p7 = interleaver[i*8+7-w_offset_p];      
    
    uint8_t out0  = (input[i_p0/8] & mask[i_p0%8])?mask[0]:0;
    uint8_t out1  = (input[i_p1/8] & mask[i_p1%8])?mask[1]:0;
    uint8_t out2  = (input[i_p2/8] & mask[i_p2%8])?mask[2]:0;
    uint8_t out3  = (input[i_p3/8] & mask[i_p3%8])?mask[3]:0;
    uint8_t out4  = (input[i_p4/8] & mask[i_p4%8])?mask[4]:0;
    uint8_t out5  = (input[i_p5/8] & mask[i_p5%8])?mask[5]:0;
    uint8_t out6  = (input[i_p6/8] & mask[i_p6%8])?mask[6]:0;
    uint8_t out7  = (input[i_p7/8] & mask[i_p7%8])?mask[7]:0;
    
    output[i] = out0 | out1 | out2 | out3 | out4 | out5 | out6 | out7; 
  }
#endif /* LV_HAVE_SSE */
  for (uint32_t j=0;j<nof_bits%8;j++) {
    uint16_t i_p = interleaver[(nof_bits/8)*8+j-w_offset];          
    if (input[i_p/8] & mask[i_p%8]) {
      output[nof_bits/8] |= mask[j];
    } else {
      output[nof_bits/8] &= ~(mask[j]);
    }
  }
  for (uint32_t j=0;j<w_offset;j++) {
    uint16_t i_p = interleaver[(nof_bits/8)*8+j-w_offset];          
    if (input[i_p/8] & (1<<(7-i_p%8))) {
      output[nof_bits/8] |= mask[j];
    } else {
      output[nof_bits/8] &= ~(mask[j]);
    }
  }
}

/* bitarray copy function taken from 
 * http://stackoverflow.com/questions/3534535/whats-a-time-efficient-algorithm-to-copy-unaligned-bit-arrays
 */


#define PREPARE_FIRST_COPY()                                      \
    do {                                                          \
    if (src_len >= (CHAR_BIT - dst_offset_modulo)) {              \
        *dst     &= reverse_mask[dst_offset_modulo];              \
        src_len -= CHAR_BIT - dst_offset_modulo;                  \
    } else {                                                      \
        *dst     &= reverse_mask[dst_offset_modulo]               \
              | reverse_mask_xor[dst_offset_modulo + src_len];    \
         c       &= reverse_mask[dst_offset_modulo + src_len];    \
        src_len = 0;                                              \
    } } while (0)


static void
bitarray_copy(const unsigned char *src_org, int src_offset, int src_len,
                    unsigned char *dst_org, int dst_offset)
{
    static const unsigned char reverse_mask[] =
        { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };
    static const unsigned char reverse_mask_xor[] =
        { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00 };

    if (src_len) {
        const unsigned char *src;
              unsigned char *dst;
        int                  src_offset_modulo,
                             dst_offset_modulo;

        src = src_org + (src_offset / CHAR_BIT);
        dst = dst_org + (dst_offset / CHAR_BIT);

        src_offset_modulo = src_offset % CHAR_BIT;
        dst_offset_modulo = dst_offset % CHAR_BIT;

        if (src_offset_modulo == dst_offset_modulo) {
            int              byte_len;
            int              src_len_modulo;
            if (src_offset_modulo) {
                unsigned char   c;

                c = reverse_mask_xor[dst_offset_modulo]     & *src++;

                PREPARE_FIRST_COPY();
                *dst++ |= c;
            }

            byte_len = src_len / CHAR_BIT;
            src_len_modulo = src_len % CHAR_BIT;

            if (byte_len) {
                memcpy(dst, src, byte_len);
                src += byte_len;
                dst += byte_len;
            }
            if (src_len_modulo) {
                *dst     &= reverse_mask_xor[src_len_modulo];
                *dst |= reverse_mask[src_len_modulo]     & *src;
            }
        } else {
            int             bit_diff_ls,
                            bit_diff_rs;
            int             byte_len;
            int             src_len_modulo;
            unsigned char   c;
            /*
             * Begin: Line things up on destination. 
             */
            if (src_offset_modulo > dst_offset_modulo) {
                bit_diff_ls = src_offset_modulo - dst_offset_modulo;
                bit_diff_rs = CHAR_BIT - bit_diff_ls;

                c = *src++ << bit_diff_ls;
                c |= *src >> bit_diff_rs;
                c     &= reverse_mask_xor[dst_offset_modulo];
            } else {
                bit_diff_rs = dst_offset_modulo - src_offset_modulo;
                bit_diff_ls = CHAR_BIT - bit_diff_rs;

                c = *src >> bit_diff_rs     &
                    reverse_mask_xor[dst_offset_modulo];
            }
            PREPARE_FIRST_COPY();
            *dst++ |= c;

            /*
             * Middle: copy with only shifting the source. 
             */
            byte_len = src_len / CHAR_BIT;

            while (--byte_len >= 0) {
                c = *src++ << bit_diff_ls;
                c |= *src >> bit_diff_rs;
                *dst++ = c;
            }

            /*
             * End: copy the remaing bits; 
             */
            src_len_modulo = src_len % CHAR_BIT;
            if (src_len_modulo) {
                c = *src++ << bit_diff_ls;
                c |= *src >> bit_diff_rs;
                c     &= reverse_mask[src_len_modulo];

                *dst     &= reverse_mask_xor[src_len_modulo];
                *dst |= c;
            }
        }
    }
}

/**
 * Copy bits from src to dst, with offsets and length in bits
 *
 * @param[out] dst Output array
 * @param[in] src Input array
 * @param dst_offset Output array write offset in bits
 * @param src_offset Input array read offset in bits
 * @param nof_bits Number of bits to copy
 */
void srslte_bit_copy(uint8_t *dst, uint32_t dst_offset, uint8_t *src, uint32_t src_offset, uint32_t nof_bits)
{
  static const uint8_t mask_dst[] =
        { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };
  if ((dst_offset%8) == 0 && (src_offset%8) == 0) {
    // copy rest of words
    memcpy(&dst[dst_offset/8], &src[src_offset/8], nof_bits/8);
    // copy last word
    if (nof_bits%8) {
      dst[dst_offset/8+nof_bits/8] = src[src_offset/8+nof_bits/8] & mask_dst[nof_bits%8];
    }
  } else {
    bitarray_copy(src, src_offset, nof_bits, dst, dst_offset);
  }
}

void srslte_bit_unpack_vector(uint8_t *packed, uint8_t *unpacked, int nof_bits)
{
  uint32_t i, nbytes;
  nbytes = nof_bits/8;
  for (i=0;i<nbytes;i++) {
    srslte_bit_unpack(packed[i], &unpacked, 8);
  }
  if (nof_bits%8) {
    srslte_bit_unpack(packed[i]>>(8-nof_bits%8), &unpacked, nof_bits%8);
  }
}

void srslte_bit_unpack_l(uint64_t value, uint8_t **bits, int nof_bits)
{
  int i;

  for(i=0; i<nof_bits; i++) {
      (*bits)[i] = (value >> (nof_bits-i-1)) & 0x1;
  }
  *bits += nof_bits;
}

/**
 * Unpacks nof_bits from LSBs of value in MSB order to *bits. Advances pointer past unpacked bits.
 *
 * @param[in] value nof_bits lowest order bits will be unpacked in MSB order
 * @param[in] nof_bits Number of bits to unpack
 * @param[out] bits Points to buffer pointer. The buffer pointer will be advanced by nof_bits
 */
void srslte_bit_unpack(uint32_t value, uint8_t **bits, int nof_bits)
{
    int i;

    for(i=0; i<nof_bits; i++) {
        (*bits)[i] = (value >> (nof_bits-i-1)) & 0x1;
    }
    *bits += nof_bits;
}

void srslte_bit_pack_vector(uint8_t *unpacked, uint8_t *packed, int nof_bits)
{
  uint32_t i, nbytes;
  nbytes = nof_bits/8;
  for (i=0;i<nbytes;i++) {
    packed[i] = srslte_bit_pack(&unpacked, 8);
  }
  if (nof_bits%8) {
    packed[i] = srslte_bit_pack(&unpacked, nof_bits%8);
    packed[i] <<= 8-(nof_bits%8);
  }
}

uint32_t srslte_bit_pack(uint8_t **bits, int nof_bits)
{
    int i;
    uint32_t value=0;

    for(i=0; i<nof_bits; i++) {
      value |= (uint32_t) (*bits)[i] << (nof_bits-i-1);
    }
    *bits += nof_bits;
    return value;
}

uint64_t srslte_bit_pack_l(uint8_t **bits, int nof_bits)
{
    int i;
    uint64_t value=0;

    for(i=0; i<nof_bits; i++) {
      value |= (uint64_t) (*bits)[i] << (nof_bits-i-1);
    }
    *bits += nof_bits;
    return value;
}

void srslte_bit_fprint(FILE *stream, uint8_t *bits, int nof_bits) {
  int i;

  fprintf(stream,"[");
  for (i=0;i<nof_bits-1;i++) {
    fprintf(stream,"%d,",bits[i]);
  }
  fprintf(stream,"%d]\n",bits[i]);
}

uint32_t srslte_bit_diff(uint8_t *x, uint8_t *y, int nbits) {
  uint32_t errors=0;
  for (int i=0;i<nbits;i++) {
    if (x[i] != y[i]) {
      errors++;
    }
  }
  return errors;
}

// Counts the number of ones in a word. K&R book exercise 2.9
uint32_t srslte_bit_count(uint32_t n) {
  int c;
  for (c = 0; n; c++)
    n &= n - 1;
  return c;
}
