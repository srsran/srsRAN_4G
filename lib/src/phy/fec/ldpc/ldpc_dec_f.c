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
 * \file ldpc_dec_f.c
 * \brief Definition of the LDPC decoder inner functions working
 *    with float-valued LLRs.
 * \author David Gregoratti
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "ldpc_dec_all.h"
#include "math.h"
#include "srsran/phy/fec/ldpc/base_graph.h"

#include "srsran/phy/utils/vector.h"

/*!
 * \brief Inner registers for the LDPC decoder that works with real-valued LLRs.
 */
struct ldpc_regs {
  float* soft_bits;    /*!< \brief A-posteriori log-likelihood ratios. */
  float* check_to_var; /*!< \brief Check-to-variable messages. */
  float* var_to_check; /*!< \brief Variable-to-check messages. */
  float (*min_v2c)[2]; /*!< \brief Helper register for computing check-to-variable messages. */
  int* min_v_index;    /*!< \brief Helper register for computing check-to-variable messages. */
  int* prod_v2c;       /*!< \brief Helper register for computing check-to-variable messages. */

  uint16_t liftN;        /*!< \brief Total number of variable nodes (after lifting). */
  uint16_t hrrN;         /*!< \brief Number of variable nodes in the high-rate region (after lifing). */
  uint8_t  bgM;          /*!< \brief Number of check nodes (before lifting). */
  uint16_t ls;           /*!< \brief Lifting size. */
  float    scaling_fctr; /*!< Scaling factor for the normalized min-sum decoding algorithm. */
};

void* create_ldpc_dec_f(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr)
{
  struct ldpc_regs* vp = NULL;

  uint8_t  bgK   = bgN - bgM;
  uint16_t liftN = bgN * ls;
  uint16_t hrrN  = (bgK + 4) * ls;

  if ((vp = malloc(sizeof(struct ldpc_regs))) == NULL) {
    return NULL;
  }

  if ((vp->soft_bits = srsran_vec_f_malloc(liftN)) == NULL) {
    free(vp);
    return NULL;
  }

  if ((vp->check_to_var = srsran_vec_f_malloc((hrrN + ls) * bgM)) == NULL) {
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  if ((vp->var_to_check = srsran_vec_f_malloc((hrrN + ls))) == NULL) {
    free(vp->check_to_var);
    free(vp->soft_bits);
    free(vp);
    return NULL;
  }

  if ((vp->min_v2c = malloc(ls * sizeof(float[2]))) == NULL) {
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

  vp->bgM          = bgM;
  vp->liftN        = liftN;
  vp->hrrN         = hrrN;
  vp->ls           = ls;
  vp->scaling_fctr = scaling_fctr;

  return vp;
}

void delete_ldpc_dec_f(void* p)
{
  struct ldpc_regs* vp = p;

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

int init_ldpc_dec_f(void* p, const float* llrs, uint16_t ls)
{
  struct ldpc_regs* vp   = p;
  int               i    = 0;
  int               skip = 2 * ls;

  if (p == NULL) {
    return -1;
  }

  bzero(vp->soft_bits, skip * sizeof(float));
  for (i = skip; i < vp->liftN; i++) {
    vp->soft_bits[i] = llrs[i - skip];
  }

  srsran_vec_f_zero(vp->check_to_var, (vp->hrrN + vp->ls) * (uint32_t)vp->bgM);
  srsran_vec_f_zero(vp->var_to_check, vp->hrrN + vp->ls);
  return 0;
}

int update_ldpc_var_to_check_f(void* p, int i_layer)
{
  struct ldpc_regs* vp = p;

  if (p == NULL) {
    return -1;
  }

  float* this_check_to_var = vp->check_to_var + i_layer * (vp->hrrN + vp->ls);

  // Update the high-rate region.
  srsran_vec_sub_fff(vp->soft_bits, this_check_to_var, vp->var_to_check, vp->hrrN);

  if (i_layer >= 4) {
    // Update the extension region.
    srsran_vec_sub_fff(vp->soft_bits + vp->hrrN + (i_layer - 4) * vp->ls,
                       this_check_to_var + vp->hrrN,
                       vp->var_to_check + vp->hrrN,
                       vp->ls);
  }

  return 0;
}

int update_ldpc_check_to_var_f(void*           p,
                               int             i_layer,
                               const uint16_t* this_pcm,
                               const int8_t (*these_var_indices)[MAX_CNCT])
{
  struct ldpc_regs* vp = p;

  if (p == NULL) {
    return -1;
  }

  int i = 0;
  int j = 0;

  for (i = 0; i < vp->ls; i++) {
    vp->prod_v2c[i] = 1;
    for (j = 0; j < 2; j++) {
      vp->min_v2c[i][j] = INFINITY;
    }
  }

  uint16_t shift      = 0;
  int      index      = 0;
  float    this_v2c   = NAN;
  int      is_min     = 0;
  int      i_v2c_base = 0;
  int      i_v2c      = 0;

  int8_t current_var_index = (*these_var_indices)[0];

  for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    shift      = this_pcm[current_var_index];
    i_v2c_base = current_var_index * vp->ls;
    i_v2c_base = (i_v2c_base <= vp->hrrN) ? i_v2c_base : vp->hrrN;
    for (j = 0; j < vp->ls; j++) {
      index    = (j + vp->ls - shift) % vp->ls;
      i_v2c    = i_v2c_base + j;
      this_v2c = fabsf(vp->var_to_check[i_v2c]);
      is_min   = this_v2c < vp->min_v2c[index][0];
      vp->min_v2c[index][1] =
          (this_v2c >= vp->min_v2c[index][1]) ? vp->min_v2c[index][1] : (is_min ? vp->min_v2c[index][0] : this_v2c);
      vp->min_v2c[index][0]  = is_min ? this_v2c : vp->min_v2c[index][0];
      vp->min_v_index[index] = is_min ? i_v2c : vp->min_v_index[index];

      vp->prod_v2c[index] *= (vp->var_to_check[i_v2c] >= 0) ? 1 : -1;
    }

    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  float* this_check_to_var = vp->check_to_var + i_layer * (vp->hrrN + vp->ls);
  current_var_index        = (*these_var_indices)[0];

  for (i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    shift      = this_pcm[current_var_index];
    i_v2c_base = current_var_index * vp->ls;
    i_v2c_base = (i_v2c_base <= vp->hrrN) ? i_v2c_base : vp->hrrN;
    for (j = 0; j < vp->ls; j++) {
      index = (j + vp->ls - shift) % vp->ls;
      i_v2c = i_v2c_base + j;

      this_check_to_var[i_v2c] = (i_v2c != vp->min_v_index[index]) ? vp->min_v2c[index][0] : vp->min_v2c[index][1];
      this_check_to_var[i_v2c] *= vp->scaling_fctr;

      this_check_to_var[i_v2c] *= (float)vp->prod_v2c[index] * ((vp->var_to_check[i_v2c] >= 0) ? 1.F : -1.F);
    }
    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  return 0;
}

int update_ldpc_soft_bits_f(void* p, int i_layer, const int8_t (*these_var_indices)[MAX_CNCT])
{
  struct ldpc_regs* vp = p;
  if (p == NULL) {
    return -1;
  }

  int    i_bit             = 0;
  int    i_bit_tmp         = 0;
  float* this_check_to_var = vp->check_to_var + i_layer * (vp->hrrN + vp->ls);
  float* this_var_to_check = vp->var_to_check;

  int8_t current_var_index     = (*these_var_indices)[0];
  int    current_var_index_ext = 0;

  for (int i = 0; (current_var_index != -1) && (i < MAX_CNCT); i++) {
    current_var_index_ext = current_var_index * vp->ls;
    for (int j = 0; j < vp->ls; j++) {
      i_bit     = current_var_index_ext + j;
      i_bit_tmp = (current_var_index_ext <= vp->hrrN) ? i_bit : vp->hrrN + j;

      vp->soft_bits[i_bit] = this_check_to_var[i_bit_tmp] + this_var_to_check[i_bit_tmp];
    }
    current_var_index = (*these_var_indices)[(i + 1) % MAX_CNCT];
  }

  return 0;
}

int extract_ldpc_message_f(void* p, uint8_t* message, uint16_t liftK)
{
  if (p == NULL) {
    return -1;
  }

  struct ldpc_regs* vp = p;

  for (int i = 0; i < liftK; i++) {
    message[i] = (vp->soft_bits[i] < 0);
  }

  return 0;
}
