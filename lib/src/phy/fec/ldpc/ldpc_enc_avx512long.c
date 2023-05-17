/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
 * \file ldpc_enc_avx512long.c
 * \brief Definition of the LDPC encoder inner functions (AVX512 version, large (>64) lifting size).
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
struct ldpc_enc_avx512long {
  bg_node_avx512_t* codeword;         /*!< \brief Contains the entire codeword, before puncturing. */
  bg_node_avx512_t* codeword_to_free; /*!< \brief Auxiliary pointer with a free memory of size SRSRAN_AVX512_B_SIZE
                                      previous to codeword. */
  __m512i* aux;                       /*!< \brief Auxiliary register. */
  __m512i* rotated_node;              /*!< \brief To store rotated versions of the nodes. */
  __m512i* rotated_node_to_free; /*!< \brief Auxiliary pointer to store rotated versions of the nodes with extra free
                                   memory of size SRSRAN_AVX512_B_SIZE previous to rotated_node */
  uint8_t  n_subnodes;           /*!< \brief Number of subnodes. */
  uint16_t node_size;            /*!> \brief Size of a node in bytes. */
};

/*!
 * Rotate the contents of a node towards the right by \b shift chars, that is the
 * \b shift * 8 most significant bits become the least significant ones.
 * \param[in]  mem_addr   The address to the node to rotate.
 * \param[out] out        The rotated node.
 * \param[in]  shift      The order of the rotation in number of chars.
 * \param[in]  ls         The size of the node (lifting size).
 */
static void rotate_node_right(const uint8_t* mem_addr, __m512i* out, uint16_t this_shift2, uint16_t ls);

void* create_ldpc_enc_avx512long(srsran_ldpc_encoder_t* q)
{
  struct ldpc_enc_avx512long* vp = NULL;

  if ((vp = malloc(sizeof(struct ldpc_enc_avx512long))) == NULL) {
    return NULL;
  }

  int left_out   = q->ls % SRSRAN_AVX512_B_SIZE;
  vp->n_subnodes = q->ls / SRSRAN_AVX512_B_SIZE + (left_out > 0);

  if ((vp->codeword_to_free = srsran_vec_malloc((q->bgN * vp->n_subnodes + 1) * sizeof(bg_node_avx512_t))) == NULL) {
    free(vp);
    return NULL;
  }
  vp->codeword = &vp->codeword_to_free[1];

  if ((vp->aux = srsran_vec_malloc(q->bgM * vp->n_subnodes * sizeof(__m512i))) == NULL) {
    free(vp->codeword_to_free);
    free(vp);
    return NULL;
  }

  if ((vp->rotated_node_to_free = srsran_vec_malloc((vp->n_subnodes + 2) * sizeof(__m512i))) == NULL) {
    free(vp->aux);
    free(vp->codeword_to_free);
    free(vp);
    return NULL;
  }
  vp->rotated_node = &vp->rotated_node_to_free[1];

  vp->node_size = SRSRAN_AVX512_B_SIZE * vp->n_subnodes;
  return vp;
}

void delete_ldpc_enc_avx512long(void* p)
{
  struct ldpc_enc_avx512long* vp = p;

  if (vp != NULL) {
    free(vp->rotated_node_to_free);
    free(vp->aux);
    free(vp->codeword_to_free);
    free(vp);
  }
}

int load_avx512long(void* p, const uint8_t* input, const uint8_t msg_len, const uint8_t cdwd_len, const uint16_t ls)
{
  struct ldpc_enc_avx512long* vp = p;

  if (p == NULL) {
    return -1;
  }

  int ini       = 0;
  int node_size = vp->node_size;
  for (int i = 0; i < msg_len * ls; i = i + ls) {
    for (int k = 0; k < ls; k++) {
      vp->codeword->c[ini + k] = input[i + k];
    }
    // this zero padding is not really necessary
    bzero(&(vp->codeword->c[ini + ls]), (node_size - ls) * sizeof(uint8_t));
    ini = ini + node_size;
  }

  bzero(vp->codeword + msg_len * vp->n_subnodes, (cdwd_len - msg_len) * vp->n_subnodes * sizeof(__m512i));

  return 0;
}

int return_codeword_avx512long(void* p, uint8_t* output, const uint8_t cdwd_len, const uint16_t ls)
{
  struct ldpc_enc_avx512long* vp = p;

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

void encode_ext_region_avx512long(srsran_ldpc_encoder_t* q, uint8_t n_layers)
{
  struct ldpc_enc_avx512long* vp   = q->ptr;
  int                         m    = 0;
  int                         skip = 0;
  int                         k    = 0;
  int                         j    = 0;

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
        rotate_node_right(vp->codeword[(q->bgK + k) * vp->n_subnodes].c, vp->rotated_node, *this_shift, q->ls);
        for (j = 0; j < vp->n_subnodes; j++) {
          vp->codeword[skip + j].v = _mm512_xor_si512(vp->codeword[skip + j].v, vp->rotated_node[j]);
        }
      }
    }
  }
}

void preprocess_systematic_bits_avx512long(srsran_ldpc_encoder_t* q)
{
  struct ldpc_enc_avx512long* vp = q->ptr;

  int       N   = q->bgN;
  int       K   = q->bgK;
  int       M   = q->bgM;
  int       ls  = q->ls;
  uint16_t* pcm = q->pcm;

  int       k          = 0;
  int       m          = 0;
  int       j          = 0;
  uint16_t* this_shift = NULL;

  __m512i tmp_epi8_avx512;

  bzero(vp->aux, M * vp->n_subnodes * sizeof(__m512i));

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
        rotate_node_right(vp->codeword[k * vp->n_subnodes].c, vp->rotated_node, *this_shift, ls);

        for (j = 0; j < vp->n_subnodes; j++) {
          tmp_epi8_avx512                 = _mm512_and_si512(vp->rotated_node[j], _mm512_one_epi8);
          vp->aux[m * vp->n_subnodes + j] = _mm512_xor_si512(vp->aux[m * vp->n_subnodes + j], tmp_epi8_avx512);
        }
      }
    } // m
  }   // k
}

void encode_high_rate_case1_avx512long(void* o)
{
  srsran_ldpc_encoder_t*      q  = o;
  struct ldpc_enc_avx512long* vp = q->ptr;

  int ls = q->ls;
  int j  = 0;

  int skip0 = q->bgK * vp->n_subnodes;
  int skip1 = (q->bgK + 1) * vp->n_subnodes;
  int skip2 = (q->bgK + 2) * vp->n_subnodes;
  int skip3 = (q->bgK + 3) * vp->n_subnodes;

  // first chunk of parity bits
  for (j = 0; j < vp->n_subnodes; j++) {
    vp->codeword[skip0 + j].v = _mm512_xor_si512(vp->aux[j], vp->aux[vp->n_subnodes + j]);
    vp->codeword[skip0 + j].v = _mm512_xor_si512(vp->codeword[skip0 + j].v, vp->aux[2 * vp->n_subnodes + j]);
    vp->codeword[skip0 + j].v = _mm512_xor_si512(vp->codeword[skip0 + j].v, vp->aux[3 * vp->n_subnodes + j]);
  }

  rotate_node_right(vp->codeword[skip0].c, vp->rotated_node, 1, ls);
  for (j = 0; j < vp->n_subnodes; j++) {
    // second chunk of parity bits
    vp->codeword[skip1 + j].v = _mm512_xor_si512(vp->aux[j], vp->rotated_node[j]);
    // fourth chunk of parity bits
    vp->codeword[skip3 + j].v = _mm512_xor_si512(vp->aux[3 * vp->n_subnodes + j], vp->rotated_node[j]);
    // third chunk of parity bits
    vp->codeword[skip2 + j].v = _mm512_xor_si512(vp->aux[2 * vp->n_subnodes + j], vp->codeword[skip3 + j].v);
  }
}

void encode_high_rate_case2_avx512long(void* o)
{
  srsran_ldpc_encoder_t*      q  = o;
  struct ldpc_enc_avx512long* vp = q->ptr;

  int ls = q->ls;
  int j  = 0;

  int skip0 = q->bgK * vp->n_subnodes;
  int skip1 = (q->bgK + 1) * vp->n_subnodes;
  int skip2 = (q->bgK + 2) * vp->n_subnodes;
  int skip3 = (q->bgK + 3) * vp->n_subnodes;

  // first chunk of parity bits
  for (j = 0; j < vp->n_subnodes; j++) {
    vp->rotated_node[j] = _mm512_xor_si512(vp->aux[j], vp->aux[vp->n_subnodes + j]);
    vp->rotated_node[j] = _mm512_xor_si512(vp->rotated_node[j], vp->aux[2 * vp->n_subnodes + j]);
    vp->rotated_node[j] = _mm512_xor_si512(vp->rotated_node[j], vp->aux[3 * vp->n_subnodes + j]);
  }
  rotate_node_right((uint8_t*)vp->rotated_node, &(vp->codeword[skip0].v), ls - 105 % ls, ls);

  for (j = 0; j < vp->n_subnodes; j++) {
    // second chunk of parity bits
    vp->codeword[skip1 + j].v = _mm512_xor_si512(vp->aux[j], vp->codeword[skip0 + j].v);
    // fourth chunk of parity bits
    vp->codeword[skip3 + j].v = _mm512_xor_si512(vp->aux[3 * vp->n_subnodes + j], vp->codeword[skip0 + j].v);
    // third chunk of parity bits
    vp->codeword[skip2 + j].v = _mm512_xor_si512(vp->aux[2 * vp->n_subnodes + j], vp->codeword[skip3 + j].v);
  }
}

void encode_high_rate_case3_avx512long(void* o)
{
  srsran_ldpc_encoder_t*      q  = o;
  struct ldpc_enc_avx512long* vp = q->ptr;

  int ls = q->ls;
  int j  = 0;

  int skip0 = q->bgK * vp->n_subnodes;
  int skip1 = (q->bgK + 1) * vp->n_subnodes;
  int skip2 = (q->bgK + 2) * vp->n_subnodes;
  int skip3 = (q->bgK + 3) * vp->n_subnodes;

  // first chunk of parity bits
  for (j = 0; j < vp->n_subnodes; j++) {
    vp->rotated_node[j] = _mm512_xor_si512(vp->aux[j], vp->aux[vp->n_subnodes + j]);
    vp->rotated_node[j] = _mm512_xor_si512(vp->rotated_node[j], vp->aux[2 * vp->n_subnodes + j]);
    vp->rotated_node[j] = _mm512_xor_si512(vp->rotated_node[j], vp->aux[3 * vp->n_subnodes + j]);
  }
  rotate_node_right((uint8_t*)vp->rotated_node, &(vp->codeword[skip0].v), ls - 1, ls);

  for (j = 0; j < vp->n_subnodes; j++) {
    // second chunk of parity bits
    vp->codeword[skip1 + j].v = _mm512_xor_si512(vp->aux[j], vp->codeword[skip0 + j].v);
    // third chunk of parity bits
    vp->codeword[skip2 + j].v = _mm512_xor_si512(vp->aux[vp->n_subnodes + j], vp->codeword[skip1 + j].v);
    // fourth chunk of parity bits
    vp->codeword[skip3 + j].v = _mm512_xor_si512(vp->aux[3 * vp->n_subnodes + j], vp->codeword[skip0 + j].v);
  }
}

void encode_high_rate_case4_avx512long(void* o)
{
  srsran_ldpc_encoder_t*      q  = o;
  struct ldpc_enc_avx512long* vp = q->ptr;

  int ls = q->ls;
  int j  = 0;

  int skip0 = q->bgK * vp->n_subnodes;
  int skip1 = (q->bgK + 1) * vp->n_subnodes;
  int skip2 = (q->bgK + 2) * vp->n_subnodes;
  int skip3 = (q->bgK + 3) * vp->n_subnodes;

  // first chunk of parity bits
  for (j = 0; j < vp->n_subnodes; j++) {
    vp->codeword[skip0 + j].v = _mm512_xor_si512(vp->aux[j], vp->aux[vp->n_subnodes + j]);
    vp->codeword[skip0 + j].v = _mm512_xor_si512(vp->codeword[skip0 + j].v, vp->aux[2 * vp->n_subnodes + j]);
    vp->codeword[skip0 + j].v = _mm512_xor_si512(vp->codeword[skip0 + j].v, vp->aux[3 * vp->n_subnodes + j]);
  }

  rotate_node_right(vp->codeword[skip0].c, vp->rotated_node, 1, ls);

  for (j = 0; j < vp->n_subnodes; j++) {
    // second chunk of parity bits
    vp->codeword[skip1 + j].v = _mm512_xor_si512(vp->aux[j], vp->rotated_node[j]);
    // third chunk of parity bits
    vp->codeword[skip2 + j].v = _mm512_xor_si512(vp->aux[vp->n_subnodes + j], vp->codeword[skip1 + j].v);
    // fourth chunk of parity bits
    vp->codeword[skip3 + j].v = _mm512_xor_si512(vp->aux[3 * vp->n_subnodes + j], vp->rotated_node[j]);
  }
}

static void rotate_node_right(const uint8_t* mem_addr, __m512i* out, uint16_t this_shift2, uint16_t ls)
{
  uint16_t shift  = 0;
  uint16_t _shift = 0;
  uint64_t mask1  = 0;
  uint64_t mask2  = 0;

  const __m512i MZERO = _mm512_set1_epi8(0);

  // the part in the middle - we simply copy.
  int j  = 0;
  int jj = 0;

  // copy full avx512 registers from this_shift_2
  for (j = this_shift2; j <= ls - SRSRAN_AVX512_B_SIZE; j = j + SRSRAN_AVX512_B_SIZE) {
    out[jj] = _mm512_loadu_si512(mem_addr + j);
    jj      = jj + 1;
  }

  // if the last is broken, take _shift bits from the end and "shift" bits from the begin.
  if (ls > j) {
    _shift = ls - j;
    shift  = SRSRAN_AVX512_B_SIZE - _shift;

    mask1 = (1ULL << _shift) - 1; // i.e. 000001111 _shift =4
    mask2 = (1ULL << shift) - 1;
    mask2 = mask2 << _shift; // i.e. 000110000  shift = 2, _shift = 4

    out[jj] = _mm512_mask_loadu_epi8(MZERO, mask1, mem_addr + j);
    out[jj] = _mm512_mask_loadu_epi8(out[jj], mask2, mem_addr - _shift);

    jj = jj + 1;
  }

  // copy full avx512 registers from the start of mem_addr, fill with zeros after out is filled with ls
  for (j = shift; j < this_shift2; j = j + SRSRAN_AVX512_B_SIZE) {
    out[jj] = _mm512_loadu_si512(mem_addr + j); // the exes is filled with something arbitrary

    jj = jj + 1;
  }
}

#endif // LV_HAVE_AVX512
