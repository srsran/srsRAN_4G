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
 * \file ldpc_dec_c_avx2_flood.c
 * \brief Definition LDPC decoder inner functions working
 *    with 8-bit integer-valued LLRs (AVX2 version, flooded scheduling).
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
  int8_t*  c; /*!< Each base node may contain up to \ref SRSRAN_AVX2_B_SIZE lifted nodes. */
  __m256i* v; /*!< All the lifted nodes of the current base node as a 256-bit line. */
} bg_node_t;

/*!
 * \brief Maximum message magnitude.
 * Messages use a 7-bit quantization. Soft bits use the remaining bit to denote infinity.
 */
static const int8_t infinity7 = (1U << 6U) - 1;

/*!
 * \brief Inner registers for the LDPC decoder that works with 8-bit integer-valued LLRs.
 */
struct ldpc_regs_c_avx2_flood {
  __m256i scaling_fctr; /*!< \brief Scaling factor for the normalized min-sum decoding algorithm. */

  bg_node_t soft_bits;    /*!< \brief A-posteriori log-likelihood ratios. */
  __m256i*  llrs;         /*!< \brief A-priori log-likelihood ratios. */
  __m256i*  check_to_var; /*!< \brief Check-to-variable messages. */
  __m256i*  var_to_check; /*!< \brief Variable-to-check messages. */
  __m256i*  rotated_v2c;  /*!< \brief To store a rotated version of the variable-to-check messages. */

  uint16_t ls;  /*!< \brief Lifting size. */
  uint8_t  hrr; /*!< \brief Number of variable nodes in the high-rate region (before lifting). */
  uint8_t  bgM; /*!< \brief Number of check nodes (before lifting). */
  uint8_t  bgN; /*!< \brief Number of variable nodes (before lifting). */
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
static void inner_var_to_check_c_avx2(const __m256i* x, const __m256i* y, __m256i* z, uint8_t clip, uint32_t len);

/*!
 * Rotate the content of an __m256i vector (first input) towards the left by
 * the number of chars specified by the second input (i.e., the \b imm * 8 least
 * significant bits become the \b imm * 8 most significant bits).
 * \param[in]  a    Vector to circularly shift.
 * \param[in]  imm  The shift order in chars.
 * \return          The shifted vector.
 */
static __m256i _mm256_rotatelli_si256(__m256i a, int imm);

/*!
 * Rotate the content of an __m256i vector (first input) towards the right by
 * the number of chars specified by the second input (i.e., the \b imm * 8 most
 * significant bits become the \b imm * 8 least significant bits).
 * \param[in]  a    Vector to circularly shift.
 * \param[in]  imm  The shift order in chars.
 * \return          The shifted vector.
 */
static __m256i _mm256_rotaterli_si256(__m256i a, int imm);

/*!
 * Rotate the contents of a node towards the left by \b imm chars, that is the
 * \b imm * 8 most significant bits become the least significant ones.
 * \param[in]  a    The node to rotate.
 * \param[in]  imm  The order of the rotation in number of chars.
 * \param[in]  ls   The size of the node (lifting size).
 * \return     The rotated node.
 */
static __m256i rotate_node_left(__m256i a, int imm, uint16_t ls);

/*!
 * Rotate the contents of a node towards the right by \b imm chars, that is the
 * \b imm * 8 most significant bits become the least significant ones.
 * \param[in]  a    The node to rotate.
 * \param[in]  imm  The order of the rotation in number of chars.
 * \param[in]  ls   The size of the node (lifting size).
 * \return     The rotated node.
 */
static __m256i rotate_node_right(__m256i a, int imm, uint16_t ls);

/*!
 * Scale packed 8-bit integers in \b a by the scaling factor \b sf / #F2I.
 * \param[in] a   Vector of packed 8-bit integers.
 * \param[in] sf  Scaling factor.
 * \return    Vector of packed 8-bit integers with the scaling result.
 */
static __m256i _mm256_scalei_epi8(__m256i a, __m256i sf);

void* create_ldpc_dec_c_avx2_flood(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr)
{
  struct ldpc_regs_c_avx2_flood* vp = NULL;

  uint8_t  bgK = bgN - bgM;
  uint16_t hrr = bgK + 4;

  if ((vp = SRSRAN_MEM_ALLOC(struct ldpc_regs_c_avx2_flood, 1)) == NULL) {
    return NULL;
  }
  SRSRAN_MEM_ZERO(vp, struct ldpc_regs_c_avx2_flood, 1);

  if ((vp->llrs = SRSRAN_MEM_ALLOC(__m256i, bgN)) == NULL) {
    delete_ldpc_dec_c_avx2_flood(vp);
    return NULL;
  }

  if ((vp->soft_bits.v = SRSRAN_MEM_ALLOC(__m256i, bgN)) == NULL) {
    delete_ldpc_dec_c_avx2_flood(vp);
    return NULL;
  }

  uint32_t sz = (uint32_t)(hrr + 1) * (uint32_t)bgM;
  if ((vp->check_to_var = SRSRAN_MEM_ALLOC(__m256i, sz)) == NULL) {
    delete_ldpc_dec_c_avx2_flood(vp);
    return NULL;
  }

  if ((vp->var_to_check = SRSRAN_MEM_ALLOC(__m256i, sz)) == NULL) {
    delete_ldpc_dec_c_avx2_flood(vp);
    return NULL;
  }

  if ((vp->rotated_v2c = SRSRAN_MEM_ALLOC(__m256i, hrr + 1)) == NULL) {
    delete_ldpc_dec_c_avx2_flood(vp);
    return NULL;
  }

  vp->bgM = bgM;
  vp->bgN = bgN;
  vp->hrr = hrr;
  vp->ls  = ls;

  // correction > 1/16 to compensate the scaling error (2^16-1)/2^16 incurred in _mm256_scalei_epi8
  vp->scaling_fctr = _mm256_set1_epi16((uint16_t)((scaling_fctr + 0.00001525879) * F2I));

  return vp;
}

void delete_ldpc_dec_c_avx2_flood(void* p)
{
  struct ldpc_regs_c_avx2_flood* vp = p;

  if (vp == NULL) {
    return;
  }
  if (vp->rotated_v2c) {
    free(vp->rotated_v2c);
  }
  if (vp->var_to_check) {
    free(vp->var_to_check);
  }
  if (vp->check_to_var) {
    free(vp->check_to_var);
  }
  if (vp->soft_bits.v) {
    free(vp->soft_bits.v);
  }
  if (vp->llrs) {
    free(vp->llrs);
  }
  free(vp);
}

int init_ldpc_dec_c_avx2_flood(void* p, const int8_t* llrs, uint16_t ls)
{
  struct ldpc_regs_c_avx2_flood* vp = p;
  int                            i  = 0;
  int                            j  = 0;

  if (p == NULL) {
    return -1;
  }

  // the first 2 x LS bits of the codeword are not sent
  vp->soft_bits.v[0] = _mm256_set1_epi8(0);
  vp->soft_bits.v[1] = _mm256_set1_epi8(0);
  vp->llrs[0]        = _mm256_set1_epi8(0);
  vp->llrs[1]        = _mm256_set1_epi8(0);
  for (i = 2; i < vp->bgN; i++) {
    for (j = 0; j < ls; j++) {
      vp->soft_bits.c[i * SRSRAN_AVX2_B_SIZE + j] = llrs[(i - 2) * ls + j];
    }
    srsran_vec_i8_zero(&(vp->soft_bits.c[i * SRSRAN_AVX2_B_SIZE + ls]), SRSRAN_AVX2_B_SIZE - ls);
    vp->llrs[i] = vp->soft_bits.v[i];
  }

  SRSRAN_MEM_ZERO(vp->check_to_var, __m256i, (vp->hrr + 1) * (uint32_t)vp->bgM);
  SRSRAN_MEM_ZERO(vp->var_to_check, __m256i, (vp->hrr + 1) * (uint32_t)vp->bgM);
  return 0;
}

int update_ldpc_var_to_check_c_avx2_flood(void* p, int i_layer)
{
  struct ldpc_regs_c_avx2_flood* vp = p;

  if (p == NULL) {
    return -1;
  }

  __m256i* this_check_to_var = vp->check_to_var + i_layer * (vp->hrr + 1);
  __m256i* this_var_to_check = vp->var_to_check + i_layer * (vp->hrr + 1);

  // Update the high-rate region.
  inner_var_to_check_c_avx2(&(vp->soft_bits.v[0]), this_check_to_var, this_var_to_check, infinity7, vp->hrr);

  if (i_layer >= 4) {
    // Update the extension region.
    inner_var_to_check_c_avx2(&(vp->soft_bits.v[0]) + vp->hrr + i_layer - 4,
                              this_check_to_var + vp->hrr,
                              this_var_to_check + vp->hrr,
                              infinity7,
                              1);
  }

  return 0;
}

int update_ldpc_check_to_var_c_avx2_flood(void*           p,
                                          int             i_layer,
                                          const uint16_t* this_pcm,
                                          const int8_t (*these_var_indices)[MAX_CNCT])
{
  struct ldpc_regs_c_avx2_flood* vp = p;

  if (p == NULL) {
    return -1;
  }

  int i = 0;

  uint16_t shift      = 0;
  int      i_v2c_base = 0;

  __m256i* this_rotated_v2c = NULL;

  __m256i this_abs_v2c_epi8;
  __m256i minp_v2c_epi8 = _mm256_set1_epi8(INT8_MAX);
  __m256i mins_v2c_epi8 = _mm256_set1_epi8(INT8_MAX);
  __m256i prod_v2c_epi8 = _mm256_setzero_si256();
  __m256i mask_sign_epi8;
  __m256i mask_min_epi8;
  __m256i help_min_epi8;
  __m256i min_ix_epi8 = _mm256_setzero_si256();
  __m256i current_ix_epi8;

  int8_t current_var_index = (*these_var_indices)[0];

  __m256i* this_var_to_check = vp->var_to_check + i_layer * (vp->hrr + 1);

  for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    shift      = this_pcm[current_var_index];
    i_v2c_base = (current_var_index <= vp->hrr) ? current_var_index : vp->hrr;

    current_ix_epi8 = _mm256_set1_epi8((int8_t)i);

    this_rotated_v2c  = vp->rotated_v2c + i;
    *this_rotated_v2c = rotate_node_right(this_var_to_check[i_v2c_base], shift, vp->ls);
    // mask_sign is 1 if this_rotated_v2c is strictly negative
    mask_sign_epi8 = _mm256_cmpgt_epi8(zero_epi8, *this_rotated_v2c);
    prod_v2c_epi8  = _mm256_xor_si256(prod_v2c_epi8, mask_sign_epi8);

    this_abs_v2c_epi8 = _mm256_abs_epi8(*this_rotated_v2c);
    // mask_min is 1 if this_abs_v2c is strictly smaller tha minp_v2c
    mask_min_epi8 = _mm256_cmpgt_epi8(minp_v2c_epi8, this_abs_v2c_epi8);
    help_min_epi8 = _mm256_blendv_epi8(this_abs_v2c_epi8, minp_v2c_epi8, mask_min_epi8);
    minp_v2c_epi8 = _mm256_blendv_epi8(minp_v2c_epi8, this_abs_v2c_epi8, mask_min_epi8);
    min_ix_epi8   = _mm256_blendv_epi8(min_ix_epi8, current_ix_epi8, mask_min_epi8);

    // mask_min is 1 if this_abs_v2c is strictly smaller tha mins_v2c
    mask_min_epi8 = _mm256_cmpgt_epi8(mins_v2c_epi8, this_abs_v2c_epi8);
    mins_v2c_epi8 = _mm256_blendv_epi8(mins_v2c_epi8, help_min_epi8, mask_min_epi8);

    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  __m256i* this_check_to_var = vp->check_to_var + i_layer * (vp->hrr + 1);
  current_var_index          = (*these_var_indices)[0];

  __m256i mask_is_min_epi8;
  __m256i this_c2v_epi8;
  __m256i help_c2v_epi8;
  __m256i final_sign_epi8;

  for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    shift      = this_pcm[current_var_index];
    i_v2c_base = (current_var_index <= vp->hrr) ? current_var_index : vp->hrr;

    this_rotated_v2c = vp->rotated_v2c + i;
    // mask_sign is 1 if this_rotated_v2c is strictly negative
    final_sign_epi8 = _mm256_cmpgt_epi8(zero_epi8, *this_rotated_v2c);
    final_sign_epi8 = _mm256_xor_si256(final_sign_epi8, prod_v2c_epi8);

    current_ix_epi8  = _mm256_set1_epi8((int8_t)i);
    mask_is_min_epi8 = _mm256_cmpeq_epi8(current_ix_epi8, min_ix_epi8);
    this_c2v_epi8    = _mm256_blendv_epi8(minp_v2c_epi8, mins_v2c_epi8, mask_is_min_epi8);
    this_c2v_epi8    = _mm256_scalei_epi8(this_c2v_epi8, vp->scaling_fctr);
    help_c2v_epi8    = _mm256_sign_epi8(this_c2v_epi8, final_sign_epi8);
    this_c2v_epi8    = _mm256_blendv_epi8(this_c2v_epi8, help_c2v_epi8, final_sign_epi8);

    this_check_to_var[i_v2c_base] = rotate_node_left(this_c2v_epi8, shift, vp->ls);

    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  return 0;
}

int update_ldpc_soft_bits_c_avx2_flood(void* p, const int8_t (*these_var_indices)[MAX_CNCT])
{
  struct ldpc_regs_c_avx2_flood* vp = p;
  if (p == NULL) {
    return -1;
  }

  __m256i* this_check_to_var = NULL;

  int    i                 = 0;
  int    i_layer           = 0;
  int    i_bit_tmp_base    = 0;
  int8_t current_var_index = 0;

  __m256i tmp_epi8;
  __m256i mask_epi8;

  for (i = 0; i < vp->bgN; i++) {
    vp->soft_bits.v[i] = vp->llrs[i];
  }

  for (i_layer = 0; i_layer < vp->bgM; i_layer++) {
    current_var_index = these_var_indices[i_layer][0];

    this_check_to_var = vp->check_to_var + i_layer * (vp->hrr + 1);
    for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
      i_bit_tmp_base = (current_var_index <= vp->hrr) ? current_var_index : vp->hrr;

      tmp_epi8 = _mm256_adds_epi8(this_check_to_var[i_bit_tmp_base], vp->soft_bits.v[current_var_index]);

      // tmp = (tmp > infty7) : infty8 ? tmp
      mask_epi8 = _mm256_cmpgt_epi8(tmp_epi8, infty7_epi8);
      tmp_epi8  = _mm256_blendv_epi8(tmp_epi8, infty8_epi8, mask_epi8);

      // tmp = (tmp < -infty7) : -infty8 ? tmp
      mask_epi8                          = _mm256_cmpgt_epi8(neg_infty7_epi8, tmp_epi8);
      vp->soft_bits.v[current_var_index] = _mm256_blendv_epi8(tmp_epi8, neg_infty8_epi8, mask_epi8);

      current_var_index = these_var_indices[i_layer][i + 1];
    }
  }

  return 0;
}

int extract_ldpc_message_c_avx2_flood(void* p, uint8_t* message, uint16_t liftK)
{
  if (p == NULL) {
    return -1;
  }

  struct ldpc_regs_c_avx2_flood* vp = p;

  int j = 0;

  for (int i = 0; i < liftK / vp->ls; i++) {
    for (j = 0; j < vp->ls; j++) {
      message[i * vp->ls + j] = (vp->soft_bits.c[i * SRSRAN_AVX2_B_SIZE + j] < 0);
    }
  }

  return 0;
}

static void
inner_var_to_check_c_avx2(const __m256i* x, const __m256i* y, __m256i* z, const uint8_t clip, const uint32_t len)
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

    // z = (x-y > clip) ? clip : x-y
    help_sub_epi8 = _mm256_subs_epi8(x_epi8, y_epi8);
    mask_epi8     = _mm256_cmpgt_epi8(help_sub_epi8, clip_epi8);
    z_epi8        = _mm256_blendv_epi8(help_sub_epi8, clip_epi8, mask_epi8);

    // z = (z < -clip) ? -clip : z
    mask_epi8 = _mm256_cmpgt_epi8(neg_clip_epi8, z_epi8);
    z_epi8    = _mm256_blendv_epi8(z_epi8, neg_clip_epi8, mask_epi8);

    // ensure that x = +/- infinity => z = +/- infinity
    // z = (x < infinity) ? z : infinity
    mask_epi8 = _mm256_cmpgt_epi8(infty8_epi8, x_epi8);
    z_epi8    = _mm256_blendv_epi8(infty8_epi8, z_epi8, mask_epi8);

    // z = (x > - infinity) ? z : - infinity
    mask_epi8 = _mm256_cmpgt_epi8(x_epi8, neg_infty8_epi8);
    z[i]      = _mm256_blendv_epi8(neg_infty8_epi8, z_epi8, mask_epi8);
  }
}

static __m256i _mm256_rotatelli_si256(__m256i a, int imm)
{
  __m256i rotated_block_a[4];

  // rotate left a as if made of 64-bit blocks: rotated_block_a[i] contains the
  // rotation by i units
  rotated_block_a[0] = a;                                // blocks 0 - 1 - 2 - 3
  rotated_block_a[1] = _mm256_permute4x64_epi64(a, 147); // 3 - 0 - 1 - 2
  rotated_block_a[2] = _mm256_permute4x64_epi64(a, 78);  // 2 - 3 - 0 - 1
  rotated_block_a[3] = _mm256_permute4x64_epi64(a, 57);  // 1 - 2 - 3 - 0

  // rotation index we are interested in
  int step1 = imm / 8;
  // small-step rotation
  int left = imm % 8;
  // next block, for carry-over
  int step2 = (step1 + 1) % 4;

  // shift right each block
  __m256i reg1 = _mm256_slli_epi64(rotated_block_a[step1], left * 8);
  // carry-over from the next block
  __m256i reg2 = _mm256_srli_epi64(rotated_block_a[step2], (8 - left) * 8);

  return _mm256_xor_si256(reg1, reg2);
}

static __m256i _mm256_rotaterli_si256(__m256i a, int imm)
{
  __m256i rotated_block_a[4];

  // rotate right a as if made of 64-bit blocks: rotated_block_a[i] contains the
  // rotation by i units
  rotated_block_a[0] = a;                                // blocks 0 - 1 - 2 - 3
  rotated_block_a[1] = _mm256_permute4x64_epi64(a, 57);  // 1 - 2 - 3 - 0
  rotated_block_a[2] = _mm256_permute4x64_epi64(a, 78);  // 2 - 3 - 0 - 1
  rotated_block_a[3] = _mm256_permute4x64_epi64(a, 147); // 3 - 0 - 1 - 2

  // rotation index we are interested in
  int step1 = imm / 8;
  // small-step rotation
  int left = imm % 8;
  // next block, for carry-over
  int step2 = (step1 + 1) % 4;

  // shift right each block
  __m256i reg1 = _mm256_srli_epi64(rotated_block_a[step1], left * 8);
  // carry-over from the next block
  __m256i reg2 = _mm256_slli_epi64(rotated_block_a[step2], (8 - left) * 8);

  return _mm256_xor_si256(reg1, reg2);
}

static __m256i rotate_node_left(__m256i a, int imm, uint16_t ls)
{
  if (imm == 0) {
    return a;
  }
  __m256i step1 = _mm256_rotatelli_si256(a, imm);
  if (ls == SRSRAN_AVX2_B_SIZE) {
    return step1;
  }

  __m256i step2 = _mm256_rotaterli_si256(a, ls - imm);

  step1 = _mm256_and_si256(step1, mask_most_epi8[imm]);
  step2 = _mm256_and_si256(step2, mask_least_epi8[imm]);

  step1 = _mm256_xor_si256(step1, step2);

  return step1;
  ;
}

static __m256i rotate_node_right(__m256i a, int imm, uint16_t ls)
{
  if (imm == 0) {
    return a;
  }
  __m256i step1 = _mm256_rotaterli_si256(a, imm);
  if (ls == SRSRAN_AVX2_B_SIZE) {
    return step1;
  }

  __m256i step2 = _mm256_rotatelli_si256(a, ls - imm);

  step1 = _mm256_and_si256(step1, mask_least_epi8[ls - imm]);
  step2 = _mm256_and_si256(step2, mask_most_epi8[ls - imm]);

  step1 = _mm256_xor_si256(step1, step2);

  return step1;
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
