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
 * \file ldpc_dec_c_avx512.c
 * \brief Definition LDPC decoder inner functions working
 *    with 8-bit integer-valued LLRs (AVX512 version, lifting size < 64).
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
  int8_t*  c; /*!< Each base node may contain up to \ref SRSRAN_AVX512_B_SIZE lifted nodes. */
  __m512i* v; /*!< All the lifted nodes of the current base node as a 512-bit line. */
} bg_node_avx512_t;

/*!
 * \brief Inner registers for the LDPC decoder that works with 8-bit integer-valued LLRs.
 */
struct ldpc_regs_c_avx512 {
  __m512i scaling_fctr; /*!< \brief Scaling factor for the normalized min-sum decoding algorithm. */

  bg_node_avx512_t soft_bits;    /*!< \brief A-posteriori log-likelihood ratios. */
  __m512i*         check_to_var; /*!< \brief Check-to-variable messages. */
  __m512i*         var_to_check; /*!< \brief Variable-to-check messages. */
  __m512i* var_to_check_to_free; /*!< \brief the Variable-to-check messages with one extra _mm512 allocated space. */
  __m512i* rotated_v2c;          /*!< \brief To store a rotated version of the variable-to-check messages. */

  __m512i* this_c2v_epi8;         /*!< \brief Helper register for the current c2v node. */
  __m512i* this_c2v_epi8_to_free; /*!< \brief Helper register for the current c2v node with one extra __m512 allocated
                                     space. */

  uint16_t ls;     /*!< \brief Lifting size. */
  uint8_t  hrr;    /*!< \brief Number of variable nodes in the high-rate region (before lifting). */
  uint8_t  bgM;    /*!< \brief Number of check nodes (before lifting). */
  uint8_t  bgN;    /*!< \brief Number of variable nodes (before lifting). */
  uint16_t finalN; /*!< \brief (bgN-2)*ls */
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
static void inner_var_to_check_c_avx512(const __m512i* x, const __m512i* y, __m512i* z, uint8_t clip, uint32_t len);

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

void* create_ldpc_dec_c_avx512(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr)
{
  struct ldpc_regs_c_avx512* vp = NULL;

  uint8_t  bgK = bgN - bgM;
  uint16_t hrr = bgK + 4;

  if ((vp = SRSRAN_MEM_ALLOC(struct ldpc_regs_c_avx512, 1)) == NULL) {
    return NULL;
  }
  SRSRAN_MEM_ZERO(vp, struct ldpc_regs_c_avx512, 1);

  if ((vp->soft_bits.v = SRSRAN_MEM_ALLOC(__m512i, bgN)) == NULL) {
    delete_ldpc_dec_c_avx512(vp);
    return NULL;
  }

  if ((vp->check_to_var = SRSRAN_MEM_ALLOC(__m512i, (hrr + 1) * bgM)) == NULL) {
    delete_ldpc_dec_c_avx512(vp);
    return NULL;
  }

  if ((vp->var_to_check_to_free = SRSRAN_MEM_ALLOC(__m512i, (hrr + 1) + 2)) == NULL) {
    delete_ldpc_dec_c_avx512(vp);
    return NULL;
  }
  vp->var_to_check = &vp->var_to_check_to_free[1];

  if ((vp->rotated_v2c = SRSRAN_MEM_ALLOC(__m512i, hrr + 1)) == NULL) {
    delete_ldpc_dec_c_avx512(vp);
    return NULL;
  }

  if ((vp->this_c2v_epi8_to_free = SRSRAN_MEM_ALLOC(__m512i, 1 + 2)) == NULL) {
    delete_ldpc_dec_c_avx512(vp);
    return NULL;
  }
  vp->this_c2v_epi8 =
      &vp->this_c2v_epi8_to_free[1]; //+1 to support reading negative position in this_c2v_epi8 at rotate_node_rigth

  vp->bgM = bgM;
  vp->bgN = bgN;
  vp->hrr = hrr;
  vp->ls  = ls;

  vp->finalN = (bgN - 2) * ls;
  // correction > 1/16 to compensate the scaling error (2^16-1)/2^16 incurred in _mm512_scalei_epi8
  vp->scaling_fctr = _mm512_set1_epi16((uint16_t)((scaling_fctr + 0.00001525879) * F2I));

  return vp;
}

void delete_ldpc_dec_c_avx512(void* p)
{
  struct ldpc_regs_c_avx512* vp = p;

  if (vp == NULL) {
    return;
  }
  if (vp->this_c2v_epi8_to_free) {
    free(vp->this_c2v_epi8_to_free);
  }
  if (vp->rotated_v2c) {
    free(vp->rotated_v2c);
  }
  if (vp->var_to_check_to_free) {
    free(vp->var_to_check_to_free);
  }
  if (vp->check_to_var) {
    free(vp->check_to_var);
  }
  if (vp->soft_bits.v) {
    free(vp->soft_bits.v);
  }
  free(vp);
}

int init_ldpc_dec_c_avx512(void* p, const int8_t* llrs, uint16_t ls)
{
  struct ldpc_regs_c_avx512* vp = p;

  if (p == NULL) {
    return -1;
  }

  int i = 0;
  int k = 0;

  // First 2 punctured bits
  int ini = SRSRAN_AVX512_B_SIZE + SRSRAN_AVX512_B_SIZE;
  srsran_vec_i8_zero(vp->soft_bits.c, ini);

  for (i = 0; i < vp->finalN; i = i + ls) {
    for (k = 0; k < ls; k++) {
      vp->soft_bits.c[ini + k] = llrs[i + k];
    }
    // this might be removed
    srsran_vec_i8_zero(&vp->soft_bits.c[ini + ls], SRSRAN_AVX512_B_SIZE - ls);
    ini = ini + SRSRAN_AVX512_B_SIZE;
  }

  SRSRAN_MEM_ZERO(vp->check_to_var, __m512i, (vp->hrr + 1) * vp->bgM);
  SRSRAN_MEM_ZERO(vp->var_to_check, __m512i, vp->hrr + 1);

  return 0;
}

int extract_ldpc_message_c_avx512(void* p, uint8_t* message, uint16_t liftK)
{
  if (p == NULL) {
    return -1;
  }
  struct ldpc_regs_c_avx512* vp = p;

  int ini = 0;
  for (int i = 0; i < liftK; i = i + vp->ls) {
    fec_avx512_hard_decision_c(&vp->soft_bits.c[ini], &message[i], vp->ls);
    ini = ini + SRSRAN_AVX512_B_SIZE;
  }

  return 0;
}

int update_ldpc_var_to_check_c_avx512(void* p, int i_layer)
{
  struct ldpc_regs_c_avx512* vp = p;

  if (p == NULL) {
    return -1;
  }

  __m512i* this_check_to_var = vp->check_to_var + i_layer * (vp->hrr + 1);

  // Update the high-rate region.
  inner_var_to_check_c_avx512(vp->soft_bits.v, this_check_to_var, vp->var_to_check, infinity7, vp->hrr);

  if (i_layer >= 4) {
    // Update the extension region.
    inner_var_to_check_c_avx512(
        vp->soft_bits.v + vp->hrr + i_layer - 4, this_check_to_var + vp->hrr, vp->var_to_check + vp->hrr, infinity7, 1);
  }

  return 0;
}

int update_ldpc_check_to_var_c_avx512(void*           p,
                                      int             i_layer,
                                      const uint16_t* this_pcm,
                                      const int8_t (*these_var_indices)[MAX_CNCT])
{
  struct ldpc_regs_c_avx512* vp = p;

  if (p == NULL) {
    return -1;
  }

  int i = 0;

  uint16_t shift      = 0;
  int      i_v2c_base = 0;

  __m512i* this_rotated_v2c = NULL;

  __m512i this_abs_v2c_epi8;

  __mmask64 mask_min_epi8;
  __m512i   help_min_epi8;
  __m512i   min_ix_epi8 = _mm512_setzero_si512();
  __m512i   current_ix_epi8;

  __m512i minp_v2c_epi8 = _mm512_set1_epi8(INT8_MAX);
  __m512i mins_v2c_epi8 = _mm512_set1_epi8(INT8_MAX);
  __m512i prod_v2c_epi8 = _mm512_setzero_si512();

  int8_t current_var_index = (*these_var_indices)[0];

  for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    shift      = this_pcm[current_var_index];
    i_v2c_base = (current_var_index <= vp->hrr) ? current_var_index : vp->hrr;

    current_ix_epi8 = _mm512_set1_epi8((int8_t)i);

    this_rotated_v2c = vp->rotated_v2c + i;

    rotate_node_right((uint8_t*)(vp->var_to_check + i_v2c_base), this_rotated_v2c, shift, vp->ls);

    prod_v2c_epi8 = _mm512_xor_si512(prod_v2c_epi8, *this_rotated_v2c);

    this_abs_v2c_epi8 = _mm512_abs_epi8(*this_rotated_v2c);
    // mask_min is 1 if this_abs_v2c is strictly smaller tha minp_v2c
    mask_min_epi8 = _mm512_cmpgt_epi8_mask(minp_v2c_epi8, this_abs_v2c_epi8);
    help_min_epi8 = _mm512_mask_blend_epi8(mask_min_epi8, this_abs_v2c_epi8, minp_v2c_epi8);
    minp_v2c_epi8 = _mm512_mask_blend_epi8(mask_min_epi8, minp_v2c_epi8, this_abs_v2c_epi8);
    min_ix_epi8   = _mm512_mask_blend_epi8(mask_min_epi8, min_ix_epi8, current_ix_epi8);

    // mask_min is 1 if this_abs_v2c is strictly smaller tha mins_v2c
    mask_min_epi8 = _mm512_cmpgt_epi8_mask(mins_v2c_epi8, this_abs_v2c_epi8);
    mins_v2c_epi8 = _mm512_mask_blend_epi8(mask_min_epi8, mins_v2c_epi8, help_min_epi8);

    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  __m512i* this_check_to_var = vp->check_to_var + i_layer * (vp->hrr + 1);

  current_var_index = (*these_var_indices)[0];

  __mmask64 mask_is_min_epi8;
  __m512i*  this_c2v_epi8 = vp->this_c2v_epi8;
  __m512i   final_sign_epi8;

  for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    shift      = this_pcm[current_var_index];
    i_v2c_base = (current_var_index <= vp->hrr) ? current_var_index : vp->hrr;

    this_rotated_v2c = vp->rotated_v2c + i;

    final_sign_epi8 = _mm512_xor_si512(*this_rotated_v2c, prod_v2c_epi8);

    current_ix_epi8  = _mm512_set1_epi8((int8_t)i);
    mask_is_min_epi8 = _mm512_cmpeq_epi8_mask(current_ix_epi8, min_ix_epi8);
    this_c2v_epi8[0] = _mm512_mask_blend_epi8(mask_is_min_epi8, minp_v2c_epi8, mins_v2c_epi8);
    this_c2v_epi8[0] = _mm512_scalei_epi8(this_c2v_epi8[0], vp->scaling_fctr);

    // does *not* do anything special for signs[i] == 0, just negative / non-negative
    __mmask64 negmask = _mm512_movepi8_mask(final_sign_epi8); // transform final_sing_epi8 into a mask

    this_c2v_epi8[0] = _mm512_mask_sub_epi8(this_c2v_epi8[0], negmask, _mm512_setzero_si512(), this_c2v_epi8[0]);

    // rotating right LS - shift positions is the same as rotating left shift positions
    rotate_node_right((uint8_t*)vp->this_c2v_epi8, this_check_to_var + i_v2c_base, (vp->ls - shift) % vp->ls, vp->ls);

    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  return 0;
}

int update_ldpc_soft_bits_c_avx512(void* p, int i_layer, const int8_t (*these_var_indices)[MAX_CNCT])
{
  struct ldpc_regs_c_avx512* vp = p;
  if (p == NULL) {
    return -1;
  }

  __m512i* this_check_to_var = vp->check_to_var + i_layer * (vp->hrr + 1);

  int i_bit_tmp_base = 0;

  __m512i   tmp_epi8;
  __mmask64 mask_epi8;

  int8_t current_var_index = (*these_var_indices)[0];

  for (int i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    i_bit_tmp_base = (current_var_index <= vp->hrr) ? current_var_index : vp->hrr;

    tmp_epi8 = _mm512_adds_epi8(this_check_to_var[i_bit_tmp_base], vp->var_to_check[i_bit_tmp_base]);

    mask_epi8 = _mm512_cmpgt_epi8_mask(tmp_epi8, _mm512_infty7_epi8);
    tmp_epi8  = _mm512_mask_blend_epi8(mask_epi8, tmp_epi8, _mm512_infty8_epi8);

    mask_epi8 = _mm512_cmpgt_epi8_mask(_mm512_neg_infty7_epi8, tmp_epi8);

    vp->soft_bits.v[current_var_index] = _mm512_mask_blend_epi8(mask_epi8, tmp_epi8, _mm512_neg_infty8_epi8);

    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  return 0;
}

static void
inner_var_to_check_c_avx512(const __m512i* x, const __m512i* y, __m512i* z, const uint8_t clip, const uint32_t len)
{
  unsigned i = 0;

  __m512i   x_epi8;
  __m512i   y_epi8;
  __m512i   z_epi8;
  __mmask64 mask_epi8;
  __m512i   help_sub_epi8;
  __m512i   clip_epi8     = _mm512_set1_epi8(clip);
  __m512i   neg_clip_epi8 = _mm512_set1_epi8((char)(-clip));

  // len = number of subnodes of size __m512
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
  const __m512i MZERO = _mm512_set1_epi8(0);

  uint16_t shift  = 0;
  uint16_t _shift = 0;
  uint64_t mask1  = 0;
  uint64_t mask2  = 0;

  if (this_shift == 0) {
    out[0] = _mm512_loadu_si512(mem_addr);
  } else { // if the last is broken, take _shift bits from the end and "shift" bits from the begin.

    _shift = ls - this_shift;
    shift  = SRSRAN_AVX512_B_SIZE - _shift;

    mask1 = (1ULL << _shift) - 1; // i.e. 000001111 _shift =4
    mask2 = (1ULL << shift) - 1;
    mask2 = mask2 << _shift; //    i.e. 000110000  shift = 2, _shift = 4

    out[0] = _mm512_mask_loadu_epi8(MZERO, mask1, mem_addr + this_shift);
    out[0] = _mm512_mask_loadu_epi8(out[0], mask2, mem_addr - _shift);
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

#endif // LV_HAVE_AVX2
