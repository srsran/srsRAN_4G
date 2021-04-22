/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/*!
 * \file ldpc_enc_avx512.c
 * \brief Definition of the LDPC encoder inner functions (AVX512 version, small (<64) lifting size).
 * \author Jesus Gomez
 * \date 2021
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include <stdint.h>

#include "../utils_avx512.h"
#include "ldpc_enc_all.h"
#include "srsran/phy/fec/ldpc/base_graph.h"
#include "srsran/phy/fec/ldpc/ldpc_encoder.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#ifdef LV_HAVE_AVX512

#include <immintrin.h>

#include "ldpc_avx512_consts.h"

/*!
 * \brief Represents a node of the base factor graph.
 */
typedef union bg_node_avx512_t {
  uint8_t c[SRSRAN_AVX512_B_SIZE]; /*!< Each base node may contain up to \ref SRSRAN_AVX512_B_SIZE lifted nodes. */
  __m512i v;                       /*!< All the lifted nodes of the current base node as a 512-bit line. */
} bg_node_avx512_t;

/*!
 * \brief Inner registers for the optimized LDPC encoder.
 */
struct ldpc_enc_avx512 {
  bg_node_avx512_t* codeword;         /*!< \brief Contains the entire codeword, before puncturing. */
  bg_node_avx512_t* codeword_to_free; /*!< \brief Auxiliary pointer with a free memory of size SRSRAN_AVX512_B_SIZE
                                      previous to codeword. */
  __m512i* aux;                       /*!< \brief Auxiliary register. */

  __m512i* rotated_node;         /*!< \brief To store rotated versions of the nodes. */
  __m512i* rotated_node_to_free; /*!< \brief Auxiliary pointer to store rotated versions of the nodes with extra free
                                   memory of size SRSRAN_AVX512_B_SIZE previous to rotated_node */
};

/*!
 * Rotate the contents of a node towards the right by \b shift chars, that is the
 * \b shift * 8 most significant bits become the least significant ones.
 * \param[in]  mem_addr    Address to the node to rotate.
 * \param[out] out     The rotated node.
 * \param[in]  shift  The order of the rotation in number of chars.
 * \param[in]  ls   The size of the node (lifting size).
 */
static void rotate_node_right(const uint8_t* mem_addr, __m512i* out, uint16_t this_shift2, uint16_t ls);

void* create_ldpc_enc_avx512(srsran_ldpc_encoder_t* q)
{
  struct ldpc_enc_avx512* vp = NULL;

  if ((vp = malloc(sizeof(struct ldpc_enc_avx512))) == NULL) {
    return NULL;
  }

  if ((vp->codeword_to_free = srsran_vec_malloc((q->bgN + 1) * sizeof(bg_node_avx512_t))) == NULL) {
    free(vp);
    return NULL;
  }
  vp->codeword = &vp->codeword_to_free[1];

  if ((vp->aux = srsran_vec_malloc(q->bgM * sizeof(__m512i))) == NULL) {
    free(vp->codeword_to_free);
    free(vp);
    return NULL;
  }

  if ((vp->rotated_node_to_free = srsran_vec_malloc((1 + 2) * sizeof(__m512i))) == NULL) {
    free(vp->aux);
    free(vp->codeword_to_free);
    free(vp);
    return NULL;
  }
  vp->rotated_node = &vp->rotated_node_to_free[1];

  return vp;
}

void delete_ldpc_enc_avx512(void* p)
{
  struct ldpc_enc_avx512* vp = p;

  if (vp != NULL) {
    free(vp->rotated_node_to_free);
    free(vp->aux);
    free(vp->codeword_to_free);
    free(vp);
  }
}

int load_avx512(void* p, const uint8_t* input, const uint8_t msg_len, const uint8_t cdwd_len, const uint16_t ls)
{
  struct ldpc_enc_avx512* vp = p;

  if (p == NULL) {
    return -1;
  }

  int i = 0;
  int k = 0;
  for (; i < msg_len; i++) {
    for (k = 0; k < ls; k++) {
      vp->codeword[i].c[k] = input[i * ls + k];
    }
    // This zero padding might be remove
    bzero(&(vp->codeword[i].c[k]), (SRSRAN_AVX512_B_SIZE - k) * sizeof(uint8_t));
  }

  bzero(vp->codeword + i, (cdwd_len - msg_len) * sizeof(__m512i));

  return 0;
}

int return_codeword_avx512(void* p, uint8_t* output, const uint8_t cdwd_len, const uint16_t ls)
{
  struct ldpc_enc_avx512* vp = p;

  if (p == NULL) {
    return -1;
  }

  int k = 0;
  for (int i = 0; i < cdwd_len - 2; i++) {
    for (k = 0; k < ls; k++) {
      output[i * ls + k] = vp->codeword[i + 2].c[k];
    }
  }
  return 0;
}

void encode_ext_region_avx512(srsran_ldpc_encoder_t* q, uint8_t n_layers)
{
  struct ldpc_enc_avx512* vp = q->ptr;

  int m    = 0;
  int skip = 0;
  int k    = 0;

  uint16_t* this_shift = NULL;

  __m512i tmp_epi8;

  // Encode the extended region. In case of puncturing or IR-HARQ, we could focus on
  // specific check nodes instead of processing all of them from m = 4 to m = M - 1.
  for (m = 4; m < n_layers; m++) {
    skip = q->bgK + m;

    // the systematic part has already been computed
    vp->codeword[skip].v = vp->aux[m];

    // sum the contribution due to the high-rate region, with the proper circular shifts
    for (k = 0; k < 4; k++) {
      this_shift = q->pcm + q->bgK + k + m * q->bgN;
      if (*this_shift != NO_CNCT) {
        rotate_node_right(vp->codeword[q->bgK + k].c, &tmp_epi8, *this_shift, q->ls);
        vp->codeword[skip].v = _mm512_xor_si512(vp->codeword[skip].v, tmp_epi8);
      }
    }
  }
}

void preprocess_systematic_bits_avx512(srsran_ldpc_encoder_t* q)
{
  struct ldpc_enc_avx512* vp  = q->ptr;
  int                     N   = q->bgN;
  int                     K   = q->bgK;
  int                     M   = q->bgM;
  int                     ls  = q->ls;
  uint16_t*               pcm = q->pcm;

  int       k          = 0;
  int       m          = 0;
  uint16_t* this_shift = NULL;

  __m512i tmp_epi8;

  bzero(vp->aux, M * sizeof(__m512i));

  // split the input message into K chunks of ls bits each and, for all chunks
  for (k = 0; k < K; k++) {
    // for all check nodes
    // NB: if looking for performance you can do the following loop only over the high-rate
    // region of the PCM (m=0,1,2,3) and over the check nodes that result in a transmitted
    // coded bit after puncturing or IR-HARQ (see Deliverable D1 Section 3.4).
    for (m = 0; m < M; m++) {
      // entry of pcm corresponding to the current input chunk and the current check node
      this_shift = pcm + k + m * N;

      // xor array aux[m] with a circularly shifted version of the current input chunk, unless
      // the current check node and variable node are not connected.
      if (*this_shift != NO_CNCT) {
        rotate_node_right(vp->codeword[k].c, &tmp_epi8, *this_shift, ls);

        tmp_epi8   = _mm512_and_si512(tmp_epi8, _mm512_one_epi8);
        vp->aux[m] = _mm512_xor_si512(vp->aux[m], tmp_epi8);
      }
    }
  }
}

void encode_high_rate_case1_avx512(void* o)
{
  srsran_ldpc_encoder_t*  q  = o;
  struct ldpc_enc_avx512* vp = q->ptr;

  int ls = q->ls;

  int skip0 = q->bgK;
  int skip1 = q->bgK + 1;
  int skip2 = q->bgK + 2;
  int skip3 = q->bgK + 3;

  // first chunk of parity bits
  vp->codeword[skip0].v = _mm512_xor_si512(vp->aux[0], vp->aux[1]);
  vp->codeword[skip0].v = _mm512_xor_si512(vp->codeword[skip0].v, vp->aux[2]);
  vp->codeword[skip0].v = _mm512_xor_si512(vp->codeword[skip0].v, vp->aux[3]);

  __m512i tmp_epi8;
  rotate_node_right(vp->codeword[skip0].c, &tmp_epi8, 1, ls);

  // second chunk of parity bits
  vp->codeword[skip1].v = _mm512_xor_si512(vp->aux[0], tmp_epi8);
  // fourth chunk of parity bits
  vp->codeword[skip3].v = _mm512_xor_si512(vp->aux[3], tmp_epi8);
  // third chunk of parity bits
  vp->codeword[skip2].v = _mm512_xor_si512(vp->aux[2], vp->codeword[skip3].v);
}

void encode_high_rate_case2_avx512(void* o)
{
  srsran_ldpc_encoder_t*  q  = o;
  struct ldpc_enc_avx512* vp = q->ptr;

  int ls = q->ls;

  int skip0 = q->bgK;
  int skip1 = q->bgK + 1;
  int skip2 = q->bgK + 2;
  int skip3 = q->bgK + 3;

  // first chunk of parity bits

  __m512i* tmp_epi8 = vp->rotated_node;
  *tmp_epi8         = _mm512_xor_si512(vp->aux[0], vp->aux[1]);
  *tmp_epi8         = _mm512_xor_si512(*tmp_epi8, vp->aux[2]);
  *tmp_epi8         = _mm512_xor_si512(*tmp_epi8, vp->aux[3]);

  rotate_node_right((uint8_t*)tmp_epi8, &(vp->codeword[skip0].v), ls - 105 % ls, ls);

  // second chunk of parity bits
  vp->codeword[skip1].v = _mm512_xor_si512(vp->aux[0], vp->codeword[skip0].v);
  // fourth chunk of parity bits
  vp->codeword[skip3].v = _mm512_xor_si512(vp->aux[3], vp->codeword[skip0].v);
  // third chunk of parity bits
  vp->codeword[skip2].v = _mm512_xor_si512(vp->aux[2], vp->codeword[skip3].v);
}

void encode_high_rate_case3_avx512(void* o)
{
  srsran_ldpc_encoder_t*  q  = o;
  struct ldpc_enc_avx512* vp = q->ptr;

  int ls = q->ls;

  int skip0 = q->bgK;
  int skip1 = q->bgK + 1;
  int skip2 = q->bgK + 2;
  int skip3 = q->bgK + 3;

  // first chunk of parity bits
  __m512i* tmp_epi8 = vp->rotated_node;
  *tmp_epi8         = _mm512_xor_si512(vp->aux[0], vp->aux[1]);
  *tmp_epi8         = _mm512_xor_si512(*tmp_epi8, vp->aux[2]);
  *tmp_epi8         = _mm512_xor_si512(*tmp_epi8, vp->aux[3]);

  rotate_node_right((uint8_t*)tmp_epi8, &(vp->codeword[skip0].v), ls - 1, ls);

  // second chunk of parity bits
  vp->codeword[skip1].v = _mm512_xor_si512(vp->aux[0], vp->codeword[skip0].v);
  // third chunk of parity bits
  vp->codeword[skip2].v = _mm512_xor_si512(vp->aux[1], vp->codeword[skip1].v);
  // fourth chunk of parity bits
  vp->codeword[skip3].v = _mm512_xor_si512(vp->aux[3], vp->codeword[skip0].v);
}

void encode_high_rate_case4_avx512(void* o)
{
  srsran_ldpc_encoder_t*  q  = o;
  struct ldpc_enc_avx512* vp = q->ptr;

  int ls = q->ls;

  int skip0 = q->bgK;
  int skip1 = q->bgK + 1;
  int skip2 = q->bgK + 2;
  int skip3 = q->bgK + 3;

  // first chunk of parity bits
  vp->codeword[skip0].v = _mm512_xor_si512(vp->aux[0], vp->aux[1]);
  vp->codeword[skip0].v = _mm512_xor_si512(vp->codeword[skip0].v, vp->aux[2]);
  vp->codeword[skip0].v = _mm512_xor_si512(vp->codeword[skip0].v, vp->aux[3]);

  __m512i tmp_epi8;
  rotate_node_right(vp->codeword[skip0].c, &tmp_epi8, 1, ls);

  // second chunk of parity bits
  vp->codeword[skip1].v = _mm512_xor_si512(vp->aux[0], tmp_epi8);
  // third chunk of parity bits
  vp->codeword[skip2].v = _mm512_xor_si512(vp->aux[1], vp->codeword[skip1].v);
  // fourth chunk of parity bits
  vp->codeword[skip3].v = _mm512_xor_si512(vp->aux[3], tmp_epi8);
}

static void rotate_node_right(const uint8_t* mem_addr, __m512i* out, uint16_t this_shift2, uint16_t ls)
{
  const __m512i MZERO = _mm512_set1_epi8(0);

  uint16_t shift  = 0;
  uint16_t _shift = 0;
  uint64_t mask1  = 0;
  uint64_t mask2  = 0;

  if (this_shift2 == 0) {
    out[0] = _mm512_loadu_si512(mem_addr);
  } else { // if the last is broken, take _shift bits from the end and "shift" bits from the begin.

    _shift = ls - this_shift2;
    shift  = SRSRAN_AVX512_B_SIZE - _shift;

    mask1 = (1ULL << _shift) - 1; // i.e. 000001111 _shift =4
    mask2 = (1ULL << shift) - 1;
    mask2 = mask2 << _shift; //    i.e. 000110000  shift = 2, _shift = 4

    out[0] = _mm512_mask_loadu_epi8(MZERO, mask1, mem_addr + this_shift2);
    out[0] = _mm512_mask_loadu_epi8(out[0], mask2, mem_addr - _shift);
  }
}

#endif // LV_HAVE_AVX512
