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
 * \file ldpc_dec_c_avx2long.c
 * \brief Definition LDPC decoder inner functions working
 *    with 8-bit integer-valued LLRs (AVX2 version, large lifting size).
 *
 * Even if the inner representation is based on 8 bits, check-to-variable and
 * variable-to-check messages are actually represented with 7 bits, the
 * remaining bit is used to represent infinity.
 *
 * \author David Gregoratti
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <strings.h>

#include "../utils_avx2.h"
#include "ldpc_dec_all.h"
#include "srsran/phy/fec/ldpc/base_graph.h"
#include "srsran/phy/utils/vector.h"

#ifdef LV_HAVE_AVX2

#include <immintrin.h>

#include "ldpc_avx2_consts.h"

#define F2I 65535 /*!< \brief Used for float to int conversion---float f is stored as (int)(f*F2I). */

/*!
 * \brief Represents a node of the base factor graph.
 */
typedef union bg_node_t {
  int8_t  c[SRSRAN_AVX2_B_SIZE]; /*!< Each base node may contain up to \ref SRSRAN_AVX2_B_SIZE lifted nodes. */
  __m256i v;                     /*!< All the lifted nodes of the current base node as a 256-bit line. */
} bg_node_t;

/*!
 * \brief Maximum message magnitude.
 * Messages use a 7-bit quantization. Soft bits use the remaining bit to denote infinity.
 */
static const int8_t infinity7 = (1U << 6U) - 1;

/*!
 * \brief Inner registers for the LDPC decoder that works with 8-bit integer-valued LLRs.
 */
struct ldpc_regs_c_avx2long {
  __m256i scaling_fctr; /*!< \brief Scaling factor for the normalized min-sum decoding algorithm. */

  bg_node_t* soft_bits;            /*!< \brief A-posteriori log-likelihood ratios. */
  __m256i*   check_to_var;         /*!< \brief Check-to-variable messages. */
  __m256i*   var_to_check;         /*!< \brief Variable-to-check messages. */
  __m256i*   var_to_check_to_free; /*!< \brief the Variable-to-check messages with one extra _mm256 allocated space. */

  __m256i* rotated_v2c;           /*!< \brief To store a rotated version of the variable-to-check messages. */
  __m256i* this_c2v_epi8;         /*!< \brief Helper register for the current c2v node. */
  __m256i* this_c2v_epi8_to_free; /*!< \brief Helper register for the current c2v node with one extra __m256 allocated
                                     space. */
  __m256i* minp_v2c_epi8;         /*!< \brief Helper register for the minimum v2c message. */
  __m256i* mins_v2c_epi8;         /*!< \brief Helper register for the second minimum v2c message. */
  __m256i* prod_v2c_epi8;         /*!< \brief Helper register for the sign of the product of all v2c messages. */
  __m256i* min_ix_epi8;           /*!< \brief Helper register for the index of the minimum v2c message. */

  uint16_t ls;  /*!< \brief Lifting size. */
  uint8_t  hrr; /*!< \brief Number of variable nodes in the high-rate region (before lifting). */
  uint8_t  bgM; /*!< \brief Number of check nodes (before lifting). */
  uint8_t  bgN; /*!< \brief Number of variable nodes (before lifting). */

  uint8_t n_subnodes; /*!< \brief Number of subnodes. */
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
static void inner_var_to_check_c_avx2long(const __m256i* x, const __m256i* y, __m256i* z, uint8_t clip, uint32_t len);

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

/*!
 * Scale packed 8-bit integers in \b a by the scaling factor \b sf / #F2I.
 * \param[in] a   Vector of packed 8-bit integers.
 * \param[in] sf  Scaling factor.
 * \return    Vector of packed 8-bit integers with the scaling result.
 */
static __m256i _mm256_scalei_epi8(__m256i a, __m256i sf);

void* create_ldpc_dec_c_avx2long(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr)
{
  struct ldpc_regs_c_avx2long* vp = NULL;

  uint8_t  bgK = bgN - bgM;
  uint16_t hrr = bgK + 4;

  if ((vp = SRSRAN_MEM_ALLOC(struct ldpc_regs_c_avx2long, 1)) == NULL) {
    return NULL;
  }
  SRSRAN_MEM_ZERO(vp, struct ldpc_regs_c_avx2long, 1);

  // compute number of subnodes
  int left_out   = ls % SRSRAN_AVX2_B_SIZE;
  int n_subnodes = ls / SRSRAN_AVX2_B_SIZE + (left_out > 0);

  if ((vp->soft_bits = SRSRAN_MEM_ALLOC(bg_node_t, bgN * n_subnodes)) == NULL) {
    delete_ldpc_dec_c_avx2long(vp);
    return NULL;
  }

  if ((vp->check_to_var = SRSRAN_MEM_ALLOC(__m256i, (hrr + 1) * bgM * n_subnodes)) == NULL) {
    delete_ldpc_dec_c_avx2long(vp);
    return NULL;
  }

  if ((vp->var_to_check_to_free = SRSRAN_MEM_ALLOC(__m256i, (hrr + 1) * n_subnodes + 2)) == NULL) {
    delete_ldpc_dec_c_avx2long(vp);
    return NULL;
  }
  vp->var_to_check = &vp->var_to_check_to_free[1];

  if ((vp->minp_v2c_epi8 = SRSRAN_MEM_ALLOC(__m256i, n_subnodes)) == NULL) {
    delete_ldpc_dec_c_avx2long(vp);
    return NULL;
  }

  if ((vp->mins_v2c_epi8 = SRSRAN_MEM_ALLOC(__m256i, n_subnodes)) == NULL) {
    delete_ldpc_dec_c_avx2long(vp);
    return NULL;
  }

  if ((vp->prod_v2c_epi8 = SRSRAN_MEM_ALLOC(__m256i, n_subnodes)) == NULL) {
    delete_ldpc_dec_c_avx2long(vp);
    return NULL;
  }

  if ((vp->min_ix_epi8 = SRSRAN_MEM_ALLOC(__m256i, n_subnodes)) == NULL) {
    delete_ldpc_dec_c_avx2long(vp);
    return NULL;
  }

  if ((vp->rotated_v2c = SRSRAN_MEM_ALLOC(__m256i, (hrr + 1) * n_subnodes)) == NULL) {
    delete_ldpc_dec_c_avx2long(vp);
    return NULL;
  }

  if ((vp->this_c2v_epi8_to_free = SRSRAN_MEM_ALLOC(__m256i, n_subnodes + 2)) == NULL) {
    delete_ldpc_dec_c_avx2long(vp);
    return NULL;
  }
  vp->this_c2v_epi8 =
      &vp->this_c2v_epi8_to_free[1]; //+1 to support reading negative position in this_c2v_epi8 at rotate_node_rigth

  vp->bgM = bgM;
  vp->bgN = bgN;
  vp->hrr = hrr;
  vp->ls  = ls;

  vp->n_subnodes = n_subnodes;

  // correction > 1/16 to compensate the scaling error (2^16-1)/2^16 incurred in _mm256_scalei_epi8
  vp->scaling_fctr = _mm256_set1_epi16((uint16_t)((scaling_fctr + 0.00001525879) * F2I));

  return vp;
}

void delete_ldpc_dec_c_avx2long(void* p)
{
  struct ldpc_regs_c_avx2long* vp = p;

  if (vp == NULL) {
    return;
  }
  if (vp->this_c2v_epi8_to_free) {
    free(vp->this_c2v_epi8_to_free);
  }
  if (vp->rotated_v2c != NULL) {
    free(vp->rotated_v2c);
  }
  if (vp->min_ix_epi8 != NULL) {
    free(vp->min_ix_epi8);
  }
  if (vp->prod_v2c_epi8 != NULL) {
    free(vp->prod_v2c_epi8);
  }
  if (vp->mins_v2c_epi8 != NULL) {
    free(vp->mins_v2c_epi8);
  }
  if (vp->minp_v2c_epi8 != NULL) {
    free(vp->minp_v2c_epi8);
  }
  if (vp->var_to_check_to_free != NULL) {
    free(vp->var_to_check_to_free);
  }
  if (vp->check_to_var != NULL) {
    free(vp->check_to_var);
  }
  if (vp->soft_bits != NULL) {
    free(vp->soft_bits);
  }
  free(vp);
}

int init_ldpc_dec_c_avx2long(void* p, const int8_t* llrs, uint16_t ls)
{
  struct ldpc_regs_c_avx2long* vp = p;
  int                          i  = 0;
  int                          j  = 0;
  int                          k  = 0;

  if (p == NULL) {
    return -1;
  }

  for (k = 0; k < vp->n_subnodes; k++) {
    vp->soft_bits[k].v                  = _mm256_set1_epi8(0);
    vp->soft_bits[vp->n_subnodes + k].v = _mm256_set1_epi8(0);
  }
  for (i = 2; i < vp->bgN; i++) {
    for (j = 0; j < vp->n_subnodes; j++) {
      for (k = 0; (k < SRSRAN_AVX2_B_SIZE) && (j * SRSRAN_AVX2_B_SIZE + k < ls); k++) {
        vp->soft_bits[i * vp->n_subnodes + j].c[k] = llrs[(i - 2) * ls + j * SRSRAN_AVX2_B_SIZE + k];
      }
    }
    srsran_vec_i8_zero(&(vp->soft_bits[i * vp->n_subnodes + j - 1].c[k]), SRSRAN_AVX2_B_SIZE - k);
  }

  SRSRAN_MEM_ZERO(vp->check_to_var, __m256i, (vp->hrr + 1) * vp->bgM * vp->n_subnodes);
  SRSRAN_MEM_ZERO(vp->var_to_check, __m256i, (vp->hrr + 1) * vp->n_subnodes);
  SRSRAN_MEM_ZERO(vp->this_c2v_epi8_to_free, __m256i, vp->n_subnodes + 2);
  SRSRAN_MEM_ZERO(vp->min_ix_epi8, __m256i, vp->n_subnodes);
  SRSRAN_MEM_ZERO(vp->var_to_check_to_free, __m256i, (vp->hrr + 1) * vp->n_subnodes + 2);
  return 0;
}

int update_ldpc_var_to_check_c_avx2long(void* p, int i_layer)
{
  struct ldpc_regs_c_avx2long* vp = p;

  if (p == NULL) {
    return -1;
  }

  __m256i* this_check_to_var = vp->check_to_var + i_layer * (vp->hrr + 1) * vp->n_subnodes;

  // Update the high-rate region.
  inner_var_to_check_c_avx2long(
      &(vp->soft_bits[0].v), this_check_to_var, vp->var_to_check, infinity7, vp->hrr * vp->n_subnodes);

  if (i_layer >= 4) {
    // Update the extension region.
    inner_var_to_check_c_avx2long(&(vp->soft_bits[0].v) + (vp->hrr + i_layer - 4) * vp->n_subnodes,
                                  this_check_to_var + vp->hrr * vp->n_subnodes,
                                  vp->var_to_check + vp->hrr * vp->n_subnodes,
                                  infinity7,
                                  vp->n_subnodes);
  }

  return 0;
}

int update_ldpc_check_to_var_c_avx2long(void*           p,
                                        int             i_layer,
                                        const uint16_t* this_pcm,
                                        const int8_t (*these_var_indices)[MAX_CNCT])
{
  struct ldpc_regs_c_avx2long* vp = p;

  if (p == NULL) {
    return -1;
  }

  int i = 0;
  int j = 0;

  uint16_t shift      = 0;
  int      i_v2c_base = 0;

  __m256i* this_rotated_v2c = NULL;

  __m256i this_abs_v2c_epi8;
#ifndef IMPROVED
  __m256i mask_sign_epi8;
#endif // IMPROVED
  __m256i mask_min_epi8;
  __m256i help_min_epi8;
  __m256i current_ix_epi8;

  for (j = 0; j < vp->n_subnodes; j++) {
    vp->minp_v2c_epi8[j] = _mm256_set1_epi8(INT8_MAX);
    vp->mins_v2c_epi8[j] = _mm256_set1_epi8(INT8_MAX);
    vp->prod_v2c_epi8[j] = _mm256_set1_epi8(0);
  }

  int8_t current_var_index = (*these_var_indices)[0];

  for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    shift      = this_pcm[current_var_index];
    i_v2c_base = (current_var_index <= vp->hrr) ? current_var_index : vp->hrr;
    i_v2c_base *= vp->n_subnodes;

    current_ix_epi8 = _mm256_set1_epi8((int8_t)i);

    this_rotated_v2c = vp->rotated_v2c + i * vp->n_subnodes;
    rotate_node_right(vp->var_to_check + i_v2c_base, this_rotated_v2c, shift, vp->ls, vp->n_subnodes);

    for (j = 0; j < vp->n_subnodes; j++) {
      // mask_sign is 1 if this_v2c_epi8 is strictly negative
#ifndef IMPROVED
      mask_sign_epi8       = _mm256_cmpgt_epi8(zero_epi8, this_rotated_v2c[j]);
      vp->prod_v2c_epi8[j] = _mm256_xor_si256(vp->prod_v2c_epi8[j], mask_sign_epi8);
#else
      vp->prod_v2c_epi8[j] = _mm256_xor_si256(vp->prod_v2c_epi8[j], this_rotated_v2c[j]);
#endif // IMPROVED

      this_abs_v2c_epi8 = _mm256_abs_epi8(this_rotated_v2c[j]);
      // mask_min is 1 if this_abs_v2c is strictly smaller tha minp_v2c
      mask_min_epi8        = _mm256_cmpgt_epi8(vp->minp_v2c_epi8[j], this_abs_v2c_epi8);
      help_min_epi8        = _mm256_blendv_epi8(this_abs_v2c_epi8, vp->minp_v2c_epi8[j], mask_min_epi8);
      vp->minp_v2c_epi8[j] = _mm256_blendv_epi8(vp->minp_v2c_epi8[j], this_abs_v2c_epi8, mask_min_epi8);
      vp->min_ix_epi8[j]   = _mm256_blendv_epi8(vp->min_ix_epi8[j], current_ix_epi8, mask_min_epi8);

      // mask_min is 1 if this_abs_v2c is strictly smaller tha mins_v2c
      mask_min_epi8        = _mm256_cmpgt_epi8(vp->mins_v2c_epi8[j], this_abs_v2c_epi8);
      vp->mins_v2c_epi8[j] = _mm256_blendv_epi8(vp->mins_v2c_epi8[j], help_min_epi8, mask_min_epi8);
    }

    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  __m256i* this_check_to_var = vp->check_to_var + i_layer * (vp->hrr + 1) * vp->n_subnodes;
  current_var_index          = (*these_var_indices)[0];

  __m256i mask_is_min_epi8;
  __m256i help_c2v_epi8;
  __m256i final_sign_epi8;

  for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    shift      = this_pcm[current_var_index];
    i_v2c_base = (current_var_index <= vp->hrr) ? current_var_index : vp->hrr;
    i_v2c_base *= vp->n_subnodes;

    this_rotated_v2c = vp->rotated_v2c + i * vp->n_subnodes;

    for (j = 0; j < vp->n_subnodes; j++) {
// mask_sign is 1 if this_v2c_epi8 is strictly negative
#ifndef IMPROVED
      final_sign_epi8 = _mm256_cmpgt_epi8(zero_epi8, this_rotated_v2c[j]);
      final_sign_epi8 = _mm256_xor_si256(final_sign_epi8, vp->prod_v2c_epi8[j]);
#endif // IMPROVED
      final_sign_epi8 = _mm256_xor_si256(this_rotated_v2c[j], vp->prod_v2c_epi8[j]);

      current_ix_epi8      = _mm256_set1_epi8((int8_t)i);
      mask_is_min_epi8     = _mm256_cmpeq_epi8(current_ix_epi8, vp->min_ix_epi8[j]);
      vp->this_c2v_epi8[j] = _mm256_blendv_epi8(vp->minp_v2c_epi8[j], vp->mins_v2c_epi8[j], mask_is_min_epi8);
      vp->this_c2v_epi8[j] = _mm256_scalei_epi8(vp->this_c2v_epi8[j], vp->scaling_fctr);
      help_c2v_epi8        = _mm256_sign_epi8(vp->this_c2v_epi8[j], final_sign_epi8);
      vp->this_c2v_epi8[j] = _mm256_blendv_epi8(vp->this_c2v_epi8[j], help_c2v_epi8, final_sign_epi8);
    }
    // rotating right LS - shift positions is the same as rotating left shift positions
    rotate_node_right(vp->this_c2v_epi8, this_check_to_var + i_v2c_base, vp->ls - shift, vp->ls, vp->n_subnodes);

    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  return 0;
}

int update_ldpc_soft_bits_c_avx2long(void* p, int i_layer, const int8_t (*these_var_indices)[MAX_CNCT])
{
  struct ldpc_regs_c_avx2long* vp = p;
  if (p == NULL) {
    return -1;
  }

  int j = 0;

  __m256i* this_check_to_var = vp->check_to_var + i_layer * (vp->hrr + 1) * vp->n_subnodes;

  int i_bit_tmp_base = 0;
  int i_bit_subnode  = 0;

  __m256i tmp_epi8;
  __m256i mask_epi8;

  int8_t current_var_index         = (*these_var_indices)[0];
  int    current_var_index_subnode = 0;

  for (int i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    current_var_index_subnode = current_var_index * vp->n_subnodes;
    for (j = 0; j < vp->n_subnodes; j++) {
      i_bit_tmp_base = (current_var_index <= vp->hrr) ? current_var_index : vp->hrr;
      i_bit_subnode  = i_bit_tmp_base * vp->n_subnodes + j;

      tmp_epi8 = _mm256_adds_epi8(this_check_to_var[i_bit_subnode], vp->var_to_check[i_bit_subnode]);

      mask_epi8 = _mm256_cmpgt_epi8(tmp_epi8, infty7_epi8);
      tmp_epi8  = _mm256_blendv_epi8(tmp_epi8, infty8_epi8, mask_epi8);

      mask_epi8 = _mm256_cmpgt_epi8(neg_infty7_epi8, tmp_epi8);

      vp->soft_bits[current_var_index_subnode + j].v = _mm256_blendv_epi8(tmp_epi8, neg_infty8_epi8, mask_epi8);
    }

    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  return 0;
}

int extract_ldpc_message_c_avx2long(void* p, uint8_t* message, uint16_t liftK)
{
  if (p == NULL) {
    return -1;
  }

  struct ldpc_regs_c_avx2long* vp = p;

  int j = 0;
  int k = 0;

  for (int i = 0; i < liftK / vp->ls; i++) {
    for (j = 0; j < vp->n_subnodes; j++) {
      for (k = 0; (k < SRSRAN_AVX2_B_SIZE) && (j * SRSRAN_AVX2_B_SIZE + k < vp->ls); k++) {
        message[i * vp->ls + j * SRSRAN_AVX2_B_SIZE + k] = (vp->soft_bits[i * vp->n_subnodes + j].c[k] < 0);
      }
    }
  }

  return 0;
}

static void
inner_var_to_check_c_avx2long(const __m256i* x, const __m256i* y, __m256i* z, const uint8_t clip, const uint32_t len)
{
  unsigned i = 0;

  __m256i x_epi8;
  __m256i y_epi8;
  __m256i z_epi8;
  __m256i mask_epi8;
  __m256i help_sub_epi8;
  __m256i clip_epi8     = _mm256_set1_epi8(clip);
  __m256i neg_clip_epi8 = _mm256_set1_epi8((char)(-clip));

  for (i = 0; i < len; i++) {
    x_epi8 = x[i];
    y_epi8 = y[i];

    help_sub_epi8 = _mm256_subs_epi8(x_epi8, y_epi8);
    mask_epi8     = _mm256_cmpgt_epi8(help_sub_epi8, clip_epi8);
    z_epi8        = _mm256_blendv_epi8(help_sub_epi8, clip_epi8, mask_epi8);

    mask_epi8 = _mm256_cmpgt_epi8(neg_clip_epi8, z_epi8);
    z_epi8    = _mm256_blendv_epi8(z_epi8, neg_clip_epi8, mask_epi8);

    mask_epi8 = _mm256_cmpgt_epi8(infty8_epi8, x_epi8);
    z_epi8    = _mm256_blendv_epi8(infty8_epi8, z_epi8, mask_epi8);

    mask_epi8 = _mm256_cmpgt_epi8(x_epi8, neg_infty8_epi8);
    z[i]      = _mm256_blendv_epi8(neg_infty8_epi8, z_epi8, mask_epi8);
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

static __m256i _mm256_scalei_epi8(__m256i a, __m256i sf)
{
  __m256i even_epi16 = _mm256_and_si256(a, mask_even_epi8);
  __m256i odd_epi16  = _mm256_srli_epi16(a, 8);

  __m256i p_even_epi16 = _mm256_mulhi_epu16(even_epi16, sf);
  __m256i p_odd_epi16  = _mm256_mulhi_epu16(odd_epi16, sf);

  p_odd_epi16 = _mm256_slli_epi16(p_odd_epi16, 8);

  return _mm256_xor_si256(p_even_epi16, p_odd_epi16);
}

#endif // LV_HAVE_AVX2
