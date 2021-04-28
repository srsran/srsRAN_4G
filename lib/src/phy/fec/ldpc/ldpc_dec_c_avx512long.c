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
 * \file ldpc_dec_c_avx512long.c
 * \brief Definition LDPC decoder inner functions working
 *    with 8-bit integer-valued LLRs (AVX512 version, large lifting size).
 *
 * Even if the inner representation is based on 8 bits, check-to-variable and
 * variable-to-check messages are actually represented with 7 bits, the
 * remaining bit is used to represent infinity.
 *
 * \author Jesus Gomez
 * \date 2021
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include <stdint.h>

#include <stdlib.h>
#include <strings.h>

#include "../utils_avx512.h"
#include "ldpc_dec_all.h"
#include "srsran/phy/fec/ldpc/base_graph.h"
#include "srsran/phy/utils/vector.h"

#ifdef LV_HAVE_AVX512

#include <immintrin.h>

#include "ldpc_avx512_consts.h"

#define F2I 65535 /*!< \brief Used for float to int conversion---float f is stored as (int)(f*F2I). */

/*!
 * \brief Maximum message magnitude.
 * Messages use a 7-bit quantization. Soft bits use the remaining bit to denote infinity.
 */
static const int8_t infinity7 = (1U << 6U) - 1;

/*!
 * \brief Represents a node of the base factor graph.
 */
typedef union bg_node_avx512_t {
  int8_t  c[SRSRAN_AVX512_B_SIZE]; /*!< Each base node may contain up to \ref SRSRAN_AVX512_B_SIZE lifted nodes. */
  __m512i v;                       /*!< All the lifted nodes of the current base node as a 512-bit line. */
} bg_node_avx512_t;

/*!
 * \brief Inner registers for the LDPC decoder that works with 8-bit integer-valued LLRs.
 */
struct ldpc_regs_c_avx512long {
  __m512i scaling_fctr; /*!< \brief Scaling factor for the normalized min-sum decoding algorithm. */

  bg_node_avx512_t* soft_bits;    /*!< \brief A-posteriori log-likelihood ratios. */
  __m512i*          check_to_var; /*!< \brief Check-to-variable messages. */
  __m512i*          var_to_check; /*!< \brief Variable-to-check messages. */
  __m512i* var_to_check_to_free;  /*!< \brief the Variable-to-check messages with one extra _mm512 allocated space. */

  __m512i* rotated_v2c;           /*!< \brief To store a rotated version of the variable-to-check messages. */
  __m512i* this_c2v_epi8;         /*!< \brief Helper register for the current c2v node. */
  __m512i* this_c2v_epi8_to_free; /*!< \brief Helper register for the current c2v node with one extra __m512 allocated
                                     space. */
  __m512i* minp_v2c_epi8;         /*!< \brief Helper register for the minimum v2c message. */
  __m512i* mins_v2c_epi8;         /*!< \brief Helper register for the second minimum v2c message. */
  __m512i* prod_v2c_epi8;         /*!< \brief Helper register for the sign of the product of all v2c messages. */
  __m512i* min_ix_epi8;           /*!< \brief Helper register for the index of the minimum v2c message. */

  uint16_t ls;         /*!< \brief Lifting size. */
  uint8_t  hrr;        /*!< \brief Number of variable nodes in the high-rate region (before lifting). */
  uint8_t  bgM;        /*!< \brief Number of check nodes (before lifting). */
  uint8_t  bgN;        /*!< \brief Number of variable nodes (before lifting). */
  uint16_t node_size;  /*!< \brief Size of the node in bytes >ls */
  uint16_t finalN;     /*!< \brief (bgN-2)*ls */
  uint8_t  n_subnodes; /*!< \brief Number of subnodes. */
};

/*!
 * Carries out the actual update of the variable-to-check messages. It basically
 * consists in \f$ z = x - y \f$ (as vectors). However, first it checks whether
 * \f$\lvert x[i] \rvert = 2^{7}-1 \f$ (our representation of infinity) to
 * ensure it is properly propagated. Also, the subtraction is saturated between
 * \f$- clip\f$ and \f$+ clip\f$.
 * \param[in] x     Minuend: array we subtract from (in practice, the soft bits).
 * \param[in] y     Subtrahend: array to be subtracted (in practice, the
 *                  check-to-variable messages).
 * \param[out] z    Resulting difference array(in practice, the updated
 *                  variable-to-check messages).
 * \param[in]  clip The saturation value.
 * \param[in]  len  The length of the vectors.
 */
static void inner_var_to_check_c_avx512long(const __m512i* x, const __m512i* y, __m512i* z, uint8_t clip, uint32_t len);

/*!
 * Rotate the contents of a node towards the right by \b shift chars, that is the
 * \b shift * 8 most significant bits become the least significant ones.
 * \param[in]  mem_addr   The node to rotate.
 * \param[out] out        The rotated node.
 * \param[in]  shift      The order of the rotation in number of chars.
 * \param[in]  ls         The size of the node (lifting size).
 * \param[in]  n_subnodes The number of subnodes in each node.
 */
static void rotate_node_right(const uint8_t* mem_addr, __m512i* out, uint16_t this_shift, uint16_t ls);

/*!
 * Scale packed 8-bit integers in \b a by the scaling factor \b sf / #F2I.
 * \param[in] a   Vector of packed 8-bit integers.
 * \param[in] sf  Scaling factor.
 * \return    Vector of packed 8-bit integers with the scaling result.
 */
static __m512i _mm512_scalei_epi8(__m512i a, __m512i sf);

void* create_ldpc_dec_c_avx512long(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr)
{
  struct ldpc_regs_c_avx512long* vp = NULL;

  uint8_t  bgK = bgN - bgM;
  uint16_t hrr = bgK + 4;

  if ((vp = srsran_vec_malloc(sizeof(struct ldpc_regs_c_avx512long))) == NULL) {
    return NULL;
  }

  // compute number of subnodes
  int left_out   = ls % SRSRAN_AVX512_B_SIZE;
  int n_subnodes = ls / SRSRAN_AVX512_B_SIZE + (left_out > 0);

  if ((vp->soft_bits = srsran_vec_malloc(bgN * n_subnodes * sizeof(bg_node_avx512_t))) == NULL) {
    free(vp);
    return NULL;
  }

  if ((vp->check_to_var = srsran_vec_malloc((hrr + 1) * bgM * n_subnodes * sizeof(__m512i))) == NULL) {
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  if ((vp->var_to_check_to_free = srsran_vec_malloc(((hrr + 1) * n_subnodes + 2) * sizeof(__m512i))) == NULL) {
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }
  vp->var_to_check = &vp->var_to_check_to_free[1];

  if ((vp->minp_v2c_epi8 = srsran_vec_malloc(n_subnodes * sizeof(__m512i))) == NULL) {
    free(vp->var_to_check_to_free);
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  if ((vp->mins_v2c_epi8 = srsran_vec_malloc(n_subnodes * sizeof(__m512i))) == NULL) {
    free(vp->minp_v2c_epi8);
    free(vp->var_to_check_to_free);
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  if ((vp->prod_v2c_epi8 = srsran_vec_malloc(n_subnodes * sizeof(__m512i))) == NULL) {
    free(vp->mins_v2c_epi8);
    free(vp->minp_v2c_epi8);
    free(vp->var_to_check_to_free);
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  if ((vp->min_ix_epi8 = srsran_vec_malloc(n_subnodes * sizeof(__m512i))) == NULL) {
    free(vp->prod_v2c_epi8);
    free(vp->mins_v2c_epi8);
    free(vp->minp_v2c_epi8);
    free(vp->var_to_check_to_free);
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  if ((vp->rotated_v2c = srsran_vec_malloc((hrr + 1) * n_subnodes * sizeof(__m512i))) == NULL) {
    free(vp->min_ix_epi8);
    free(vp->prod_v2c_epi8);
    free(vp->mins_v2c_epi8);
    free(vp->minp_v2c_epi8);
    free(vp->var_to_check_to_free);
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  if ((vp->this_c2v_epi8_to_free = srsran_vec_malloc((n_subnodes + 2) * sizeof(__m512i))) == NULL) {
    free(vp->rotated_v2c);
    free(vp->min_ix_epi8);
    free(vp->prod_v2c_epi8);
    free(vp->mins_v2c_epi8);
    free(vp->minp_v2c_epi8);
    free(vp->var_to_check_to_free);
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }
  vp->this_c2v_epi8 =
      &vp->this_c2v_epi8_to_free[1]; //+1 to support reading negative position in this_c2v_epi8 at rotate_node_rigth

  vp->bgM = bgM;
  vp->bgN = bgN;
  vp->hrr = hrr;
  vp->ls  = ls;

  vp->n_subnodes = n_subnodes;
  vp->node_size  = SRSRAN_AVX512_B_SIZE * vp->n_subnodes;
  vp->finalN     = (bgN - 2) * ls;
  // correction > 1/16 to compensate the scaling error (2^16-1)/2^16 incurred in _mm512_scalei_epi8
  vp->scaling_fctr = _mm512_set1_epi16((uint16_t)((scaling_fctr + 0.00001525879) * F2I));
  return vp;
}

void delete_ldpc_dec_c_avx512long(void* p)
{
  struct ldpc_regs_c_avx512long* vp = p;

  if (vp != NULL) {
    free(vp->this_c2v_epi8_to_free);
    free(vp->rotated_v2c);
    free(vp->min_ix_epi8);
    free(vp->prod_v2c_epi8);
    free(vp->mins_v2c_epi8);
    free(vp->minp_v2c_epi8);
    free(vp->var_to_check_to_free);
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
  }
}

int init_ldpc_dec_c_avx512long(void* p, const int8_t* llrs, uint16_t ls)
{
  struct ldpc_regs_c_avx512long* vp = p;

  int i = 0;
  int k = 0;

  if (p == NULL) {
    return -1;
  }
  // First 2 punctured bits
  int node_size = vp->node_size;

  int ini = node_size + node_size;
  bzero(vp->soft_bits->c, ini);

  for (i = 0; i < vp->finalN; i = i + ls) {
    for (k = 0; k < ls; k++) {
      vp->soft_bits->c[ini + k] = llrs[i + k];
    }
    // this zero padding might be removed
    bzero(&vp->soft_bits->c[ini + ls], (node_size - ls) * sizeof(int8_t));
    ini = ini + node_size;
  }

  bzero(vp->check_to_var, (vp->hrr + 1) * vp->bgM * vp->n_subnodes * sizeof(__m512i));
  bzero(vp->var_to_check, (vp->hrr + 1) * vp->n_subnodes * sizeof(__m512i));

  return 0;
}

int extract_ldpc_message_c_avx512long(void* p, uint8_t* message, uint16_t liftK)
{
  if (p == NULL) {
    return -1;
  }
  struct ldpc_regs_c_avx512long* vp = p;

  int ini = 0;
  for (int i = 0; i < liftK; i = i + vp->ls) {
    fec_avx512_hard_decision_c(&vp->soft_bits->c[ini], &message[i], vp->ls);
    ini = ini + vp->node_size;
  }

  return 0;
}

int update_ldpc_var_to_check_c_avx512long(void* p, int i_layer)
{
  struct ldpc_regs_c_avx512long* vp = p;

  if (p == NULL) {
    return -1;
  }

  __m512i* this_check_to_var = vp->check_to_var + i_layer * (vp->hrr + 1) * vp->n_subnodes;

  // Update the high-rate region.
  inner_var_to_check_c_avx512long(
      &(vp->soft_bits[0].v), this_check_to_var, vp->var_to_check, infinity7, vp->hrr * vp->n_subnodes);

  if (i_layer >= 4) {
    // Update the extension region.
    inner_var_to_check_c_avx512long(&(vp->soft_bits[0].v) + (vp->hrr + i_layer - 4) * vp->n_subnodes,
                                    this_check_to_var + vp->hrr * vp->n_subnodes,
                                    vp->var_to_check + vp->hrr * vp->n_subnodes,
                                    infinity7,
                                    vp->n_subnodes);
  }

  return 0;
}

int update_ldpc_check_to_var_c_avx512long(void*           p,
                                          int             i_layer,
                                          const uint16_t* this_pcm,
                                          const int8_t (*these_var_indices)[MAX_CNCT])
{
  struct ldpc_regs_c_avx512long* vp = p;

  if (p == NULL) {
    return -1;
  }

  int i = 0;
  int j = 0;

  uint16_t shift      = 0;
  int      i_v2c_base = 0;

  __m512i*  this_rotated_v2c = NULL;
  __m512i   this_abs_v2c_epi8;
  __mmask64 mask_min_epi8;
  __m512i   help_min_epi8;
  __m512i   current_ix_epi8;

  for (j = 0; j < vp->n_subnodes; j++) {
    vp->minp_v2c_epi8[j] = _mm512_set1_epi8(INT8_MAX);
    vp->mins_v2c_epi8[j] = _mm512_set1_epi8(INT8_MAX);
    vp->prod_v2c_epi8[j] = _mm512_set1_epi8(0);
  }

  int8_t current_var_index = (*these_var_indices)[0];

  for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    shift      = this_pcm[current_var_index];
    i_v2c_base = (current_var_index <= vp->hrr) ? current_var_index : vp->hrr;
    i_v2c_base *= vp->n_subnodes;

    current_ix_epi8 = _mm512_set1_epi8((int8_t)i);

    this_rotated_v2c = vp->rotated_v2c + i * vp->n_subnodes;

    rotate_node_right((uint8_t*)(vp->var_to_check + i_v2c_base), this_rotated_v2c, shift, vp->ls);

    for (j = 0; j < vp->n_subnodes; j++) {
      vp->prod_v2c_epi8[j] = _mm512_xor_si512(vp->prod_v2c_epi8[j], this_rotated_v2c[j]);

      this_abs_v2c_epi8 = _mm512_abs_epi8(this_rotated_v2c[j]);
      // mask_min is 1 if this_abs_v2c is strictly smaller tha minp_v2c
      mask_min_epi8        = _mm512_cmpgt_epi8_mask(vp->minp_v2c_epi8[j], this_abs_v2c_epi8);
      help_min_epi8        = _mm512_mask_blend_epi8(mask_min_epi8, this_abs_v2c_epi8, vp->minp_v2c_epi8[j]);
      vp->minp_v2c_epi8[j] = _mm512_mask_blend_epi8(mask_min_epi8, vp->minp_v2c_epi8[j], this_abs_v2c_epi8);
      vp->min_ix_epi8[j]   = _mm512_mask_blend_epi8(mask_min_epi8, vp->min_ix_epi8[j], current_ix_epi8);

      // mask_min is 1 if this_abs_v2c is strictly smaller tha mins_v2c
      mask_min_epi8        = _mm512_cmpgt_epi8_mask(vp->mins_v2c_epi8[j], this_abs_v2c_epi8);
      vp->mins_v2c_epi8[j] = _mm512_mask_blend_epi8(mask_min_epi8, vp->mins_v2c_epi8[j], help_min_epi8);
    }

    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  __m512i* this_check_to_var = vp->check_to_var + i_layer * (vp->hrr + 1) * vp->n_subnodes;

  current_var_index = (*these_var_indices)[0];

  __mmask64 mask_is_min_epi8;
  __m512i   final_sign_epi8;

  for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    shift      = this_pcm[current_var_index];
    i_v2c_base = (current_var_index <= vp->hrr) ? current_var_index : vp->hrr;
    i_v2c_base *= vp->n_subnodes;

    this_rotated_v2c = vp->rotated_v2c + i * vp->n_subnodes;

    for (j = 0; j < vp->n_subnodes; j++) {
      final_sign_epi8 = _mm512_xor_si512(this_rotated_v2c[j], vp->prod_v2c_epi8[j]);

      current_ix_epi8      = _mm512_set1_epi8((int8_t)i);
      mask_is_min_epi8     = _mm512_cmpeq_epi8_mask(current_ix_epi8, vp->min_ix_epi8[j]);
      vp->this_c2v_epi8[j] = _mm512_mask_blend_epi8(mask_is_min_epi8, vp->minp_v2c_epi8[j], vp->mins_v2c_epi8[j]);
      vp->this_c2v_epi8[j] = _mm512_scalei_epi8(vp->this_c2v_epi8[j], vp->scaling_fctr);

      // does *not* do anything special for signs[i] == 0, just negative / non-negative
      __mmask64 negmask = _mm512_movepi8_mask(final_sign_epi8); // transform final_sing_epi8 into a mask

      vp->this_c2v_epi8[j] =
          _mm512_mask_sub_epi8(vp->this_c2v_epi8[j], negmask, _mm512_setzero_si512(), vp->this_c2v_epi8[j]);
    }

    // rotating right LS - shift positions is the same as rotating left shift positions
    rotate_node_right((uint8_t*)vp->this_c2v_epi8, this_check_to_var + i_v2c_base, (vp->ls - shift) % vp->ls, vp->ls);

    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  return 0;
}

int update_ldpc_soft_bits_c_avx512long(void* p, int i_layer, const int8_t (*these_var_indices)[MAX_CNCT])
{
  struct ldpc_regs_c_avx512long* vp = p;
  if (p == NULL) {
    return -1;
  }

  int j = 0;

  __m512i* this_check_to_var = vp->check_to_var + i_layer * (vp->hrr + 1) * vp->n_subnodes;

  int i_bit_tmp_base = 0;
  int i_bit_subnode  = 0;

  __m512i   tmp_epi8;
  __mmask64 mask_epi8;

  int8_t current_var_index         = (*these_var_indices)[0];
  int    current_var_index_subnode = 0;

  for (int i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    current_var_index_subnode = current_var_index * vp->n_subnodes;
    for (j = 0; j < vp->n_subnodes; j++) {
      i_bit_tmp_base = (current_var_index <= vp->hrr) ? current_var_index : vp->hrr;
      i_bit_subnode  = i_bit_tmp_base * vp->n_subnodes + j;

      tmp_epi8 = _mm512_adds_epi8(this_check_to_var[i_bit_subnode], vp->var_to_check[i_bit_subnode]);

      mask_epi8 = _mm512_cmpgt_epi8_mask(tmp_epi8, _mm512_infty7_epi8);
      tmp_epi8  = _mm512_mask_blend_epi8(mask_epi8, tmp_epi8, _mm512_infty8_epi8);

      mask_epi8 = _mm512_cmpgt_epi8_mask(_mm512_neg_infty7_epi8, tmp_epi8);

      vp->soft_bits[current_var_index_subnode + j].v =
          _mm512_mask_blend_epi8(mask_epi8, tmp_epi8, _mm512_neg_infty8_epi8);
    }

    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  return 0;
}

static void
inner_var_to_check_c_avx512long(const __m512i* x, const __m512i* y, __m512i* z, const uint8_t clip, const uint32_t len)
{
  unsigned i = 0;

  __m512i   x_epi8;
  __m512i   y_epi8;
  __m512i   z_epi8;
  __mmask64 mask_epi8;
  __m512i   help_sub_epi8;
  __m512i   clip_epi8     = _mm512_set1_epi8(clip);
  __m512i   neg_clip_epi8 = _mm512_set1_epi8((char)(-clip));

  for (i = 0; i < len; i++) {
    x_epi8 = x[i];
    y_epi8 = y[i];

    help_sub_epi8 = _mm512_subs_epi8(x_epi8, y_epi8);                 // x-y
    mask_epi8     = _mm512_cmpgt_epi8_mask(help_sub_epi8, clip_epi8); // saturate to clip insteaof inifinty8
    z_epi8        = _mm512_mask_blend_epi8(mask_epi8, help_sub_epi8, clip_epi8);

    mask_epi8 = _mm512_cmpgt_epi8_mask(neg_clip_epi8, z_epi8);
    z_epi8    = _mm512_mask_blend_epi8(mask_epi8, z_epi8, neg_clip_epi8);

    mask_epi8 = _mm512_cmpgt_epi8_mask(_mm512_infty8_epi8, x_epi8);
    z_epi8    = _mm512_mask_blend_epi8(mask_epi8, _mm512_infty8_epi8, z_epi8);

    mask_epi8 = _mm512_cmpgt_epi8_mask(x_epi8, _mm512_neg_infty8_epi8);
    z[i]      = _mm512_mask_blend_epi8(mask_epi8, _mm512_neg_infty8_epi8, z_epi8);
  }
}

static void rotate_node_right(const uint8_t* mem_addr, __m512i* out, uint16_t this_shift, uint16_t ls)
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
  for (j = this_shift; j <= ls - SRSRAN_AVX512_B_SIZE; j = j + SRSRAN_AVX512_B_SIZE) {
    out[jj] = _mm512_loadu_si512(mem_addr + j);
    jj      = jj + 1;
  }

  // if the last is broken, take _shift bits from the end and "shift" bits from the begin.
  if (ls > j) {
    _shift = ls - j;
    shift  = SRSRAN_AVX512_B_SIZE - _shift;
    mask1  = (1ULL << _shift) - 1; // i.e. 000001111 _shift =4
    mask2  = (1ULL << shift) - 1;
    mask2  = mask2 << _shift; //    i.e. 000110000  shift = 2, _shift = 4

    out[jj] = _mm512_mask_loadu_epi8(MZERO, mask1, mem_addr + j);
    out[jj] = _mm512_mask_loadu_epi8(out[jj], mask2, mem_addr - _shift);

    jj = jj + 1;
  }

  // copy full avx512 registers from the start of mem_addr
  for (j = shift; j < this_shift; j = j + SRSRAN_AVX512_B_SIZE) {
    out[jj] = _mm512_loadu_si512(mem_addr + j); // the excess is filled with something arbitrary

    jj = jj + 1;
  }
}

static __m512i _mm512_scalei_epi8(__m512i a, __m512i sf)
{
  __m512i even_epi16 = _mm512_and_si512(a, _mm512_mask_even_epi8);
  __m512i odd_epi16  = _mm512_srli_epi16(a, 8);

  __m512i p_even_epi16 = _mm512_mulhi_epu16(even_epi16, sf);
  __m512i p_odd_epi16  = _mm512_mulhi_epu16(odd_epi16, sf);

  p_odd_epi16 = _mm512_slli_epi16(p_odd_epi16, 8);

  return _mm512_xor_si512(p_even_epi16, p_odd_epi16);
}

#endif // LV_HAVE_AVX512
