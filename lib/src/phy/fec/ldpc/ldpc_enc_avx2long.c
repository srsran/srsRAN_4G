/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
 * \file ldpc_enc_avx2long.c
 * \brief Definition of the LDPC encoder inner functions (AVX2 version, large lifting size).
 * \author David Gregoratti and Jesus Gómez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include <stdint.h>

#include "../utils_avx2.h"
#include "ldpc_enc_all.h"
#include "srsran/phy/fec/ldpc/base_graph.h"
#include "srsran/phy/fec/ldpc/ldpc_encoder.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#ifdef LV_HAVE_AVX2

#include <immintrin.h>

#include "ldpc_avx2_consts.h"

/*!
 * \brief Represents a node of the base factor graph.
 */
typedef union bg_node_t {
  uint8_t c[SRSRAN_AVX2_B_SIZE]; /*!< Each base node may contain up to \ref SRSRAN_AVX2_B_SIZE lifted nodes. */
  __m256i v;                     /*!< All the lifted nodes of the current base node as a 256-bit line. */
} bg_node_t;

/*!
 * \brief Inner registers for the optimized LDPC encoder.
 */
struct ldpc_enc_avx2long {
  bg_node_t* codeword;           /*!< \brief Contains the entire codeword, before puncturing. */
  bg_node_t* codeword_to_free;   /*!< \brief Auxiliary pointer with a free memory of size SRSRAN_AVX2_B_SIZE previous to
                                    codeword */
  __m256i* aux;                  /*!< \brief Auxiliary register. */
  __m256i* rotated_node;         /*!< \brief To store rotated versions of the nodes. */
  __m256i* rotated_node_to_free; /*!< \brief Auxiliary pointer to store rotated versions of the nodes with extra free
                                    memory of size SRSRAN_AVX2_B_SIZE previous to rotated_node */
  uint8_t    n_subnodes;           /*!< \brief Number of subnodes. */
  uint16_t   node_size;            /*!< \brief Size of a node in bytes. */
};

/*!
 * Rotate the contents of a node towards the right by \b shift chars, that is the
 * \b shift * 8 most significant bits become the least significant ones.
 * \param[in]  in_256i    The node to rotate.
 * \param[out] out        The rotated node.
 * \param[in]  shift      The order of the rotation in number of chars.
 * \param[in]  ls         The size of the node (lifting size).
 * \param[in]  n_subnodes The number of subnodes in each node.
 * \return     The rotated node.
 */
static void rotate_node_right(const __m256i* in_256i, __m256i* out, uint16_t shift, uint16_t ls, int8_t n_subnodes);

void* create_ldpc_enc_avx2long(srsran_ldpc_encoder_t* q)
{
  struct ldpc_enc_avx2long* vp = NULL;

  if ((vp = malloc(sizeof(struct ldpc_enc_avx2long))) == NULL) {
    return NULL;
  }

  int left_out   = q->ls % SRSRAN_AVX2_B_SIZE;
  vp->n_subnodes = q->ls / SRSRAN_AVX2_B_SIZE + (left_out > 0);

  if ((vp->codeword_to_free = srsran_vec_malloc((q->bgN * vp->n_subnodes + 1) * sizeof(bg_node_t))) == NULL) {
    free(vp);
    return NULL;
  }
  vp->codeword = &vp->codeword_to_free[1];

  if ((vp->aux = srsran_vec_malloc(q->bgM * vp->n_subnodes * sizeof(__m256i))) == NULL) {
    free(vp->codeword_to_free);
    free(vp);
    return NULL;
  }

  if ((vp->rotated_node_to_free = srsran_vec_malloc((vp->n_subnodes + 2) * sizeof(__m256i))) == NULL) {
    free(vp->aux);
    free(vp->codeword_to_free);
    free(vp);
    return NULL;
  }
  vp->rotated_node = &vp->rotated_node_to_free[1];

  vp->node_size = SRSRAN_AVX2_B_SIZE * vp->n_subnodes;
  return vp;
}

void delete_ldpc_enc_avx2long(void* p)
{
  struct ldpc_enc_avx2long* vp = p;

  if (vp != NULL) {
    free(vp->rotated_node_to_free);
    free(vp->aux);
    free(vp->codeword_to_free);
    free(vp);
  }
}

int load_avx2long(void* p, const uint8_t* input, const uint8_t msg_len, const uint8_t cdwd_len, const uint16_t ls)
{
  struct ldpc_enc_avx2long* vp = p;

  if (p == NULL) {
    return -1;
  }

  int ini       = 0;
  int node_size = vp->node_size;
  for (int i = 0; i < msg_len * ls; i = i + ls) {
    for (int k = 0; k < ls; k++) {
      vp->codeword->c[ini + k] = input[i + k];
    }
    // this zero padding can be removed
    bzero(&(vp->codeword->c[ini + ls]), (node_size - ls) * sizeof(uint8_t));
    ini = ini + node_size;
  }

  SRSRAN_MEM_ZERO(vp->codeword + msg_len * vp->n_subnodes, bg_node_t, (cdwd_len - msg_len) * (uint32_t)vp->n_subnodes);

  return 0;
}

int return_codeword_avx2long(void* p, uint8_t* output, const uint8_t cdwd_len, const uint16_t ls)
{
  struct ldpc_enc_avx2long* vp = p;

  if (p == NULL) {
    return -1;
  }

  int ini = vp->node_size + vp->node_size;
  for (int i = 0; i < (cdwd_len - 2) * ls; i = i + ls) {
    for (int k = 0; k < ls; k++) {
      output[i + k] = vp->codeword->c[ini + k];
    }
    ini = ini + vp->node_size;
  }
  return 0;
}

void encode_ext_region_avx2long(srsran_ldpc_encoder_t* q, uint8_t n_layers)
{
  struct ldpc_enc_avx2long* vp = q->ptr;

  int m    = 0;
  int skip = 0;
  int k    = 0;
  int j    = 0;

  uint16_t* this_shift = NULL;

  // Encode the extended region. In case of puncturing or IR-HARQ, we could focus on
  // specific check nodes instead of processing all of them from m = 4 to m = M - 1.
  for (m = 4; m < n_layers; m++) {
    skip = (q->bgK + m) * vp->n_subnodes;

    // the systematic part has already been computed
    for (j = 0; j < vp->n_subnodes; j++) {
      vp->codeword[skip + j].v = vp->aux[m * vp->n_subnodes + j];
    }

    // sum the contribution due to the high-rate region, with the proper circular shifts
    for (k = 0; k < 4; k++) {
      this_shift = q->pcm + q->bgK + k + m * q->bgN;

      // xor array aux[m] with a circularly shifted version of the current input chunk, unless
      // the current check node and variable node are not connected.
      if (*this_shift != NO_CNCT) {
        rotate_node_right(
            &(vp->codeword[(q->bgK + k) * vp->n_subnodes].v), vp->rotated_node, *this_shift, q->ls, vp->n_subnodes);
        for (j = 0; j < vp->n_subnodes; j++) {
          vp->codeword[skip + j].v = _mm256_xor_si256(vp->codeword[skip + j].v, vp->rotated_node[j]);
        }
      }
    }
  }
}

void preprocess_systematic_bits_avx2long(srsran_ldpc_encoder_t* q)
{
  struct ldpc_enc_avx2long* vp = q->ptr;

  int       N   = q->bgN;
  int       K   = q->bgK;
  int       ls  = q->ls;
  uint32_t  M   = q->bgM;
  uint16_t* pcm = q->pcm;

  int       k          = 0;
  int       m          = 0;
  int       j          = 0;
  uint16_t* this_shift = NULL;

  __m256i tmp_epi8;

  SRSRAN_MEM_ZERO(vp->aux, __m256i, M * vp->n_subnodes);

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
        rotate_node_right(&(vp->codeword[k * vp->n_subnodes].v), vp->rotated_node, *this_shift, ls, vp->n_subnodes);
        for (j = 0; j < vp->n_subnodes; j++) {
          tmp_epi8                        = _mm256_and_si256(vp->rotated_node[j], one_epi8);
          vp->aux[m * vp->n_subnodes + j] = _mm256_xor_si256(vp->aux[m * vp->n_subnodes + j], tmp_epi8);
        }
      }
    }
  }
}

void encode_high_rate_case1_avx2long(void* o)
{
  srsran_ldpc_encoder_t*    q  = o;
  struct ldpc_enc_avx2long* vp = q->ptr;

  int ls = q->ls;
  int j  = 0;

  int skip0 = q->bgK * vp->n_subnodes;
  int skip1 = (q->bgK + 1) * vp->n_subnodes;
  int skip2 = (q->bgK + 2) * vp->n_subnodes;
  int skip3 = (q->bgK + 3) * vp->n_subnodes;

  // first chunk of parity bits
  for (j = 0; j < vp->n_subnodes; j++) {
    vp->codeword[skip0 + j].v = _mm256_xor_si256(vp->aux[j], vp->aux[vp->n_subnodes + j]);
    vp->codeword[skip0 + j].v = _mm256_xor_si256(vp->codeword[skip0 + j].v, vp->aux[2 * vp->n_subnodes + j]);
    vp->codeword[skip0 + j].v = _mm256_xor_si256(vp->codeword[skip0 + j].v, vp->aux[3 * vp->n_subnodes + j]);
  }

  rotate_node_right(&(vp->codeword[skip0].v), vp->rotated_node, 1, ls, vp->n_subnodes);
  for (j = 0; j < vp->n_subnodes; j++) {
    // second chunk of parity bits
    vp->codeword[skip1 + j].v = _mm256_xor_si256(vp->aux[j], vp->rotated_node[j]);
    // fourth chunk of parity bits
    vp->codeword[skip3 + j].v = _mm256_xor_si256(vp->aux[3 * vp->n_subnodes + j], vp->rotated_node[j]);
    // third chunk of parity bits
    vp->codeword[skip2 + j].v = _mm256_xor_si256(vp->aux[2 * vp->n_subnodes + j], vp->codeword[skip3 + j].v);
  }
}

void encode_high_rate_case2_avx2long(void* o)
{
  srsran_ldpc_encoder_t*    q  = o;
  struct ldpc_enc_avx2long* vp = q->ptr;

  int ls = q->ls;
  int j  = 0;

  int skip0 = q->bgK * vp->n_subnodes;
  int skip1 = (q->bgK + 1) * vp->n_subnodes;
  int skip2 = (q->bgK + 2) * vp->n_subnodes;
  int skip3 = (q->bgK + 3) * vp->n_subnodes;

  // first chunk of parity bits
  for (j = 0; j < vp->n_subnodes; j++) {
    vp->rotated_node[j] = _mm256_xor_si256(vp->aux[j], vp->aux[vp->n_subnodes + j]);
    vp->rotated_node[j] = _mm256_xor_si256(vp->rotated_node[j], vp->aux[2 * vp->n_subnodes + j]);
    vp->rotated_node[j] = _mm256_xor_si256(vp->rotated_node[j], vp->aux[3 * vp->n_subnodes + j]);
  }
  rotate_node_right(vp->rotated_node, &(vp->codeword[skip0].v), ls - 105 % ls, ls, vp->n_subnodes);

  for (j = 0; j < vp->n_subnodes; j++) {
    // second chunk of parity bits
    vp->codeword[skip1 + j].v = _mm256_xor_si256(vp->aux[j], vp->codeword[skip0 + j].v);
    // fourth chunk of parity bits
    vp->codeword[skip3 + j].v = _mm256_xor_si256(vp->aux[3 * vp->n_subnodes + j], vp->codeword[skip0 + j].v);
    // third chunk of parity bits
    vp->codeword[skip2 + j].v = _mm256_xor_si256(vp->aux[2 * vp->n_subnodes + j], vp->codeword[skip3 + j].v);
  }
}

void encode_high_rate_case3_avx2long(void* o)
{
  srsran_ldpc_encoder_t*    q  = o;
  struct ldpc_enc_avx2long* vp = q->ptr;

  int ls = q->ls;
  int j  = 0;

  int skip0 = q->bgK * vp->n_subnodes;
  int skip1 = (q->bgK + 1) * vp->n_subnodes;
  int skip2 = (q->bgK + 2) * vp->n_subnodes;
  int skip3 = (q->bgK + 3) * vp->n_subnodes;

  // first chunk of parity bits
  for (j = 0; j < vp->n_subnodes; j++) {
    vp->rotated_node[j] = _mm256_xor_si256(vp->aux[j], vp->aux[vp->n_subnodes + j]);
    vp->rotated_node[j] = _mm256_xor_si256(vp->rotated_node[j], vp->aux[2 * vp->n_subnodes + j]);
    vp->rotated_node[j] = _mm256_xor_si256(vp->rotated_node[j], vp->aux[3 * vp->n_subnodes + j]);
  }
  rotate_node_right(vp->rotated_node, &(vp->codeword[skip0].v), ls - 1, ls, vp->n_subnodes);

  for (j = 0; j < vp->n_subnodes; j++) {
    // second chunk of parity bits
    vp->codeword[skip1 + j].v = _mm256_xor_si256(vp->aux[j], vp->codeword[skip0 + j].v);
    // third chunk of parity bits
    vp->codeword[skip2 + j].v = _mm256_xor_si256(vp->aux[vp->n_subnodes + j], vp->codeword[skip1 + j].v);
    // fourth chunk of parity bits
    vp->codeword[skip3 + j].v = _mm256_xor_si256(vp->aux[3 * vp->n_subnodes + j], vp->codeword[skip0 + j].v);
  }
}

void encode_high_rate_case4_avx2long(void* o)
{
  srsran_ldpc_encoder_t*    q  = o;
  struct ldpc_enc_avx2long* vp = q->ptr;

  int ls = q->ls;
  int j  = 0;

  int skip0 = q->bgK * vp->n_subnodes;
  int skip1 = (q->bgK + 1) * vp->n_subnodes;
  int skip2 = (q->bgK + 2) * vp->n_subnodes;
  int skip3 = (q->bgK + 3) * vp->n_subnodes;

  // first chunk of parity bits
  for (j = 0; j < vp->n_subnodes; j++) {
    vp->codeword[skip0 + j].v = _mm256_xor_si256(vp->aux[j], vp->aux[vp->n_subnodes + j]);
    vp->codeword[skip0 + j].v = _mm256_xor_si256(vp->codeword[skip0 + j].v, vp->aux[2 * vp->n_subnodes + j]);
    vp->codeword[skip0 + j].v = _mm256_xor_si256(vp->codeword[skip0 + j].v, vp->aux[3 * vp->n_subnodes + j]);
  }

  rotate_node_right(&(vp->codeword[skip0].v), vp->rotated_node, 1, ls, vp->n_subnodes);
  for (j = 0; j < vp->n_subnodes; j++) {
    // second chunk of parity bits
    vp->codeword[skip1 + j].v = _mm256_xor_si256(vp->aux[j], vp->rotated_node[j]);
    // third chunk of parity bits
    vp->codeword[skip2 + j].v = _mm256_xor_si256(vp->aux[vp->n_subnodes + j], vp->codeword[skip1 + j].v);
    // fourth chunk of parity bits
    vp->codeword[skip3 + j].v = _mm256_xor_si256(vp->aux[3 * vp->n_subnodes + j], vp->rotated_node[j]);
  }
}

static void rotate_node_right(const __m256i* in_256i, __m256i* out, uint16_t shift, uint16_t ls, int8_t n_subnodes)
{
  const int8_t* in = (const int8_t*)in_256i;

  int16_t n_type1 = (ls - shift) / SRSRAN_AVX2_B_SIZE - (ls == SRSRAN_AVX2_B_SIZE);
  int16_t n_type2 = n_subnodes - n_type1 - 1 - (ls == SRSRAN_AVX2_B_SIZE);
  int16_t gap     = (ls - shift) % SRSRAN_AVX2_B_SIZE;

  int16_t i = 0;
  for (; i < n_type1; i++) {
    out[i] = _mm256_loadu_si256((const __m256i*)(in + shift + i * SRSRAN_AVX2_B_SIZE));
  }

  __m256i tmp1 = _mm256_loadu_si256((const __m256i*)(in + shift + i * SRSRAN_AVX2_B_SIZE));
  __m256i tmp2 = _mm256_loadu_si256((const __m256i*)(in - gap));

  out[i] = _mm256_blendv_epi8(tmp1, tmp2, mask_most_epi8[gap]);

  for (i = 1; i <= n_type2; i++) {
    out[n_type1 + i] = _mm256_loadu_si256((const __m256i*)(in - gap + i * SRSRAN_AVX2_B_SIZE));
  }
}

#endif // LV_HAVE_AVX2
