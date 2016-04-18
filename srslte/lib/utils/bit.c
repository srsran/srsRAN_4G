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

#include "srslte/utils/bit.h"

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
