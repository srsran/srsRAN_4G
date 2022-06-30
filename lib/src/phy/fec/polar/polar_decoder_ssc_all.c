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
 * \file polar_decoder_ssc_all.c
 * \brief Definition of the SSC polar decoder functions common to all implementations
 *
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */
#include "polar_decoder_ssc_all.h"
#include "../utils_avx2.h"
#include "srsran/phy/utils/vector.h"

/*!
 * \brief Structure with pointers needed to obtain the node_type
 */
struct Tmp_node_type {
  uint8_t*  is_not_rate_0; /*!< \brief Pointers to a temporary buffer. */
  uint8_t*  is_rate_1;     /*!< \brief Pointers to a temporary buffer. */
  uint16_t* i_even;        /*!< \brief Pointers to a temporary buffer. */
  uint16_t* i_odd;         /*!< \brief Pointers to a temporary buffer. */
};

void* create_tmp_node_type(const uint8_t nMax)
{
  struct Tmp_node_type* tmp = NULL;
  // allocate memory to the polar decoder instance
  if ((tmp = malloc(sizeof(struct Tmp_node_type))) == NULL) {
    return NULL;
  }
  SRSRAN_MEM_ZERO(tmp, struct Tmp_node_type, 1);

  uint16_t max_code_size      = (1U << nMax);
  uint8_t  nMax_1             = nMax - 1;
  uint16_t max_code_half_size = (1U << nMax_1);

  tmp->is_not_rate_0 = srsran_vec_u8_malloc(2 * max_code_size);
  if (!tmp->is_not_rate_0) {
    free(tmp);
    perror("malloc");
    return NULL;
  }

  tmp->is_rate_1 = tmp->is_not_rate_0 + max_code_size;

  tmp->i_odd = srsran_vec_u16_malloc(max_code_half_size);
  if (!tmp->i_odd) {
    free(tmp->is_not_rate_0);
    free(tmp);
    perror("malloc");
    return NULL;
  }

  tmp->i_even = srsran_vec_u16_malloc(max_code_half_size);
  if (!tmp->i_even) {
    free(tmp->is_not_rate_0);
    free(tmp->i_odd);
    free(tmp);
    perror("malloc");
    return NULL;
  }

  return tmp;
}

void delete_tmp_node_type(void* p)
{
  struct Tmp_node_type* pp = p;
  if (p != NULL) {
    free(pp->i_even);
    free(pp->i_odd);
    free(pp->is_not_rate_0); // it also removes is_rate_1
    free(pp);
  }
}

int compute_node_type(void*           p,
                      uint8_t**       node_type,
                      const uint16_t* frozen_set,
                      const uint16_t  code_size_log,
                      const uint16_t  frozen_set_size)
{

  struct Tmp_node_type* tmp = p;

  if (p == NULL) {
    return -1;
  }

  uint8_t   s             = 0; // stage
  uint8_t*  is_not_rate_0 = tmp->is_not_rate_0;
  uint8_t*  is_rate_1     = tmp->is_rate_1;
  uint16_t* i_even        = tmp->i_even;
  uint16_t* i_odd         = tmp->i_odd;

  uint16_t code_size       = (1U << code_size_log);
  uint8_t  code_size_log_1 = code_size_log - 1;
  uint16_t code_half_size  = (1U << code_size_log_1);

  memset(i_even, 0, code_half_size);
  memset(i_odd, 0, code_half_size);
  for (uint16_t i = 0; i < code_half_size; i++) {
    i_even[i] = 2 * i;
    i_odd[i]  = 2 * i + 1;
  }

  // node_type = is_not_rate_0_node: 0 if rate 0, 1 if not rate 0.
  memset(is_not_rate_0, 1, code_size);
  memset(is_rate_1, 1, code_size);
  for (uint16_t i = 0; i < frozen_set_size; i++) {
    is_not_rate_0[frozen_set[i]] = 0;
    is_rate_1[frozen_set[i]]     = 0;
  }

  s = 0;
  for (uint16_t j = 0; j < code_size; j++) {
    node_type[s][j] = 3 * is_not_rate_0[j]; //  0 if rate-0; 2 if rate-r; 3 if rate 1
  }
  uint16_t code_stage_size = 0;
  uint16_t code_size_log_s = 0;
  for (s = 1; s < (uint8_t)(code_size_log + 1); s++) {
    code_size_log_s = code_size_log - s;
    code_stage_size = (1U << code_size_log_s);
    for (uint16_t j = 0; j < code_stage_size; j++) {
      is_not_rate_0[j] = is_not_rate_0[i_even[j]] | is_not_rate_0[i_odd[j]]; // bitor
      is_rate_1[j]     = is_rate_1[i_even[j]] & is_rate_1[i_odd[j]];         // bitand
      node_type[s][j]  = 2 * is_not_rate_0[j] + is_rate_1[j];                //  0 if rate-0; 2 if rate-r; 3 if rate 1
    }
  }

#ifdef debug
  for (s = 0; s < code_size_log + 1; s++) {
    printf("Node types (%d): ", s);
    code_stage_size = (1U << (code_size_log - s));
    for (uint16_t j = 0; j < code_stage_size; j++) {
      printf("%d ", node_type[s][j]);
    }
    printf("\n");
  }
#endif

  return 0;
}

int init_node_type(const uint16_t* frozen_set, struct Params* param)
{

  uint8_t   s             = 0; // stage
  uint8_t*  is_not_rate_0 = NULL;
  uint8_t*  is_rate_1     = NULL;
  uint16_t* i_even        = NULL;
  uint16_t* i_odd         = NULL;

  uint16_t code_size      = param->code_stage_size[param->code_size_log];
  uint16_t code_half_size = param->code_stage_size[param->code_size_log - 1];

  is_not_rate_0 = srsran_vec_u8_malloc(2 * code_size);
  if (!is_not_rate_0) {
    perror("malloc");
    return -1;
  }
  is_rate_1 = is_not_rate_0 + code_size;

  i_odd = srsran_vec_u16_malloc(code_half_size);
  if (!i_odd) {
    free(is_not_rate_0);
    perror("malloc");
    return -1;
  }

  i_even = srsran_vec_u16_malloc(code_half_size);
  if (!i_even) {
    free(is_not_rate_0);
    free(i_odd);
    perror("malloc");
    return -1;
  }

  memset(i_even, 0, code_half_size);
  memset(i_odd, 0, code_half_size);
  for (uint16_t i = 0; i < code_half_size; i++) {
    i_even[i] = 2 * i;
    i_odd[i]  = 2 * i + 1;
  }

  // node_type = is_not_rate_0_node: 0 if rate 0, 1 if not rate 0.
  memset(is_not_rate_0, 1, code_size);
  memset(is_rate_1, 1, code_size);
  for (uint16_t i = 0; i < param->frozen_set_size; i++) {
    is_not_rate_0[frozen_set[i]] = 0;
    is_rate_1[frozen_set[i]]     = 0;
  }

  s = 0;
  for (uint16_t j = 0; j < code_size; j++) {
    param->node_type[s][j] = 3 * is_not_rate_0[j]; //  0 if rate-0; 2 if rate-r; 3 if rate 1
  }

  for (s = 1; s < param->code_size_log + 1; s++) {
    for (uint16_t j = 0; j < param->code_stage_size[param->code_size_log - s]; j++) {
      is_not_rate_0[j]       = is_not_rate_0[i_even[j]] | is_not_rate_0[i_odd[j]]; // bitor
      is_rate_1[j]           = is_rate_1[i_even[j]] & is_rate_1[i_odd[j]];         // bitand
      param->node_type[s][j] = 2 * is_not_rate_0[j] + is_rate_1[j]; //  0 if rate-0; 2 if rate-r; 3 if rate 1
    }
  }

  free(i_even);
  free(i_odd);
  free(is_not_rate_0);

  return 0;
}
