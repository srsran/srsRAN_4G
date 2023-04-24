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
 * \file ldpc_dec_c.c
 * \brief Definition of the LDPC decoder inner functions working
 *    with 8-bit integer-valued LLRs.
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

#include "ldpc_dec_all.h"
#include "srsran/phy/fec/ldpc/base_graph.h"
#include "srsran/phy/utils/vector.h"

#define F2I 100 /*!< \brief Used for float to int conversion---float f is stored as (int)(f*F2I). */

/*!
 * \brief Maximum message magnitude.
 * Messages use a 7-bit quantization. Soft bits use the remaining bit to denote infinity.
 */
static const int8_t infinity7 = (1U << 6U) - 1;

/*!
 * \brief Inner registers for the LDPC decoder that works with 8-bit integer-valued LLRs.
 */
struct ldpc_regs_c {
  int8_t* soft_bits;    /*!< \brief A-posteriori log-likelihood ratios. */
  int8_t* check_to_var; /*!< \brief Check-to-variable messages. */
  int8_t* var_to_check; /*!< \brief Variable-to-check messages. */
  int8_t (*min_v2c)[2]; /*!< \brief Helper register for computing check-to-variable messages. */
  int* min_v_index;     /*!< \brief Helper register for computing check-to-variable messages. */
  int* prod_v2c;        /*!< \brief Helper register for computing check-to-variable messages. */

  uint16_t liftN;        /*!< \brief Total number of variable nodes (after lifting). */
  uint16_t hrrN;         /*!< \brief Number of variable nodes in the high-rate region (after lifing). */
  uint8_t  bgM;          /*!< \brief Number of check nodes (before lifting). */
  uint16_t ls;           /*!< \brief Lifting size. */
  int      scaling_fctr; /*!< \brief Scaling factor for the normalized min-sum decoding algorithm. */
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
static void inner_var_to_check_c(const int8_t* x, const int8_t* y, int8_t* z, uint8_t clip, uint32_t len);

void* create_ldpc_dec_c(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr)
{
  struct ldpc_regs_c* vp = NULL;

  uint8_t  bgK   = bgN - bgM;
  uint16_t liftN = bgN * ls;
  uint16_t hrrN  = (bgK + 4) * ls;

  if ((vp = SRSRAN_MEM_ALLOC(struct ldpc_regs_c, 1)) == NULL) {
    return NULL;
  }

  if ((vp->soft_bits = srsran_vec_i8_malloc(liftN)) == NULL) {
    free(vp);
    return NULL;
  }

  if ((vp->check_to_var = srsran_vec_i8_malloc((hrrN + ls) * bgM)) == NULL) {
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  if ((vp->var_to_check = srsran_vec_i8_malloc((hrrN + ls))) == NULL) {
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  if ((vp->min_v2c = malloc(ls * sizeof(int8_t[2]))) == NULL) {
    free(vp->var_to_check);
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  if ((vp->min_v_index = srsran_vec_i32_malloc(ls)) == NULL) {
    free(vp->min_v2c);
    free(vp->var_to_check);
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  if ((vp->prod_v2c = srsran_vec_i32_malloc(ls)) == NULL) {
    free(vp->min_v_index);
    free(vp->min_v2c);
    free(vp->var_to_check);
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  vp->bgM   = bgM;
  vp->liftN = liftN;
  vp->hrrN  = hrrN;
  vp->ls    = ls;

  vp->scaling_fctr = (int)(scaling_fctr * F2I);

  return vp;
}

void delete_ldpc_dec_c(void* p)
{
  struct ldpc_regs_c* vp = p;

  if (vp != NULL) {
    free(vp->prod_v2c);
    free(vp->min_v_index);
    free(vp->min_v2c);
    free(vp->var_to_check);
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
  }
}

int init_ldpc_dec_c(void* p, const int8_t* llrs, uint16_t ls)
{
  struct ldpc_regs_c* vp   = p;
  int                 i    = 0;
  int                 skip = 2 * ls;

  if (p == NULL) {
    return -1;
  }

  srsran_vec_i8_zero(vp->soft_bits, skip);
  for (i = skip; i < vp->liftN; i++) {
    vp->soft_bits[i] = llrs[i - skip];
  }

  srsran_vec_i8_zero(vp->check_to_var, (vp->hrrN + vp->ls) * (uint32_t)vp->bgM);
  srsran_vec_i8_zero(vp->var_to_check, vp->hrrN + vp->ls);
  return 0;
}

int update_ldpc_var_to_check_c(void* p, int i_layer)
{
  struct ldpc_regs_c* vp = p;

  if (p == NULL) {
    return -1;
  }

  int8_t* this_check_to_var = vp->check_to_var + i_layer * (vp->hrrN + vp->ls);

  // Update the high-rate region.
  inner_var_to_check_c(vp->soft_bits, this_check_to_var, vp->var_to_check, infinity7, vp->hrrN);

  if (i_layer >= 4) {
    // Update the extension region.
    inner_var_to_check_c(vp->soft_bits + vp->hrrN + (i_layer - 4) * vp->ls,
                         this_check_to_var + vp->hrrN,
                         vp->var_to_check + vp->hrrN,
                         infinity7,
                         vp->ls);
  }

  return 0;
}

int update_ldpc_check_to_var_c(void*           p,
                               int             i_layer,
                               const uint16_t* this_pcm,
                               const int8_t (*these_var_indices)[MAX_CNCT])
{
  struct ldpc_regs_c* vp = p;

  if (p == NULL) {
    return -1;
  }

  int i = 0;
  int j = 0;

  for (i = 0; i < vp->ls; i++) {
    vp->prod_v2c[i] = 1;
    for (j = 0; j < 2; j++) {
      vp->min_v2c[i][j] = INT8_MAX;
    }
  }

  uint16_t shift      = 0;
  int      index      = 0;
  int8_t   this_v2c   = 0;
  int      is_min     = 0;
  int      i_v2c      = 0;
  int      i_v2c_base = 0;

  int8_t current_var_index = (*these_var_indices)[0];

  for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    shift      = this_pcm[current_var_index];
    i_v2c_base = current_var_index * vp->ls;
    i_v2c_base = (i_v2c_base <= vp->hrrN) ? i_v2c_base : vp->hrrN;
    for (j = 0; j < vp->ls; j++) {
      index    = (j + vp->ls - shift) % vp->ls;
      i_v2c    = i_v2c_base + j;
      this_v2c = abs(vp->var_to_check[i_v2c]);
      is_min   = this_v2c < vp->min_v2c[index][0];
      vp->min_v2c[index][1] =
          (this_v2c >= vp->min_v2c[index][1]) ? vp->min_v2c[index][1] : (is_min ? vp->min_v2c[index][0] : this_v2c);
      vp->min_v2c[index][0]  = is_min ? this_v2c : vp->min_v2c[index][0];
      vp->min_v_index[index] = is_min ? i_v2c : vp->min_v_index[index];

      vp->prod_v2c[index] *= (vp->var_to_check[i_v2c] >= 0) ? 1 : -1;
    }
    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  int8_t* this_check_to_var = vp->check_to_var + i_layer * (vp->hrrN + vp->ls);
  current_var_index         = (*these_var_indices)[0];

  for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    shift      = this_pcm[current_var_index];
    i_v2c_base = current_var_index * vp->ls;
    i_v2c_base = (i_v2c_base <= vp->hrrN) ? i_v2c_base : vp->hrrN;
    for (j = 0; j < vp->ls; j++) {
      index = (j + vp->ls - shift) % vp->ls;
      i_v2c = i_v2c_base + j;

      this_check_to_var[i_v2c] = (i_v2c != vp->min_v_index[index]) ? vp->min_v2c[index][0] : vp->min_v2c[index][1];
      this_check_to_var[i_v2c] = this_check_to_var[i_v2c] * vp->scaling_fctr / F2I;

      this_check_to_var[i_v2c] *= vp->prod_v2c[index] * ((vp->var_to_check[i_v2c] >= 0) ? 1 : -1);
    }
    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  return 0;
}

int update_ldpc_soft_bits_c(void* p, int i_layer, const int8_t (*these_var_indices)[MAX_CNCT])
{
  struct ldpc_regs_c* vp = p;
  if (p == NULL) {
    return -1;
  }

  int     i_bit             = 0;
  int     i_bit_tmp         = 0;
  int8_t* this_check_to_var = vp->check_to_var + i_layer * (vp->hrrN + vp->ls);
  int8_t* this_var_to_check = vp->var_to_check;

  long tmp = 0;

  int8_t current_var_index     = (*these_var_indices)[0];
  int    current_var_index_ext = 0;

  for (int i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    current_var_index_ext = current_var_index * vp->ls;
    for (int j = 0; j < vp->ls; j++) {
      i_bit     = current_var_index_ext + j;
      i_bit_tmp = (current_var_index_ext <= vp->hrrN) ? i_bit : vp->hrrN + j;
      tmp       = (long)this_check_to_var[i_bit_tmp] + this_var_to_check[i_bit_tmp];
      if (tmp > infinity7) {
        tmp = INT8_MAX;
      }
      if (tmp < -infinity7) {
        tmp = -INT8_MAX;
      }
      vp->soft_bits[i_bit] = (int8_t)tmp;
    }
    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  return 0;
}

int extract_ldpc_message_c(void* p, uint8_t* message, uint16_t liftK)
{
  if (p == NULL) {
    return -1;
  }

  struct ldpc_regs_c* vp = p;

  for (int i = 0; i < liftK; i++) {
    message[i] = (vp->soft_bits[i] < 0);
  }

  return 0;
}

void inner_var_to_check_c(const int8_t* x, const int8_t* y, int8_t* z, const uint8_t clip, const uint32_t len)
{
  unsigned i   = 0;
  long     tmp = 0;

  const long infinity8 = (1U << 7U) - 1; // Max positive value in 8-bit representation

  for (i = 0; i < len; i++) {
    if (x[i] >= infinity8) {
      z[i] = infinity8;
      continue;
    }
    if (x[i] <= -infinity8) {
      z[i] = -infinity8;
      continue;
    }
    tmp = (long)x[i] - y[i];
    if (tmp > clip) {
      tmp = clip;
    }
    if (tmp < -clip) {
      tmp = -clip;
    }
    z[i] = (int8_t)tmp;
  }
}
