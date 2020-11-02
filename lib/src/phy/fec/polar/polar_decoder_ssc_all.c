/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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
 * \author Jesus Gomez (CTTC)
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */
#include "polar_decoder_ssc_all.h"
#include "../utils_avx2.h"

int init_node_type(const uint16_t* frozen_set, struct Params* param)
{

  uint8_t   s             = 0; // stage
  uint8_t*  is_not_rate_0 = NULL;
  uint8_t*  is_rate_1     = NULL;
  uint16_t* i_even        = NULL;
  uint16_t* i_odd         = NULL;

  uint16_t code_size      = param->code_stage_size[param->code_size_log];
  uint16_t code_half_size = param->code_stage_size[param->code_size_log - 1];

  is_not_rate_0 = aligned_alloc(SRSLTE_AVX2_B_SIZE, 2 * code_size * sizeof(uint8_t));
  if (!is_not_rate_0) {
    perror("malloc");
    return -1;
  }
  is_rate_1 = is_not_rate_0 + code_size;

  i_odd = malloc(code_half_size * sizeof(uint16_t));
  if (!i_odd) {
    free(is_not_rate_0);
    perror("malloc");
    return -1;
  }

  i_even = malloc(code_half_size * sizeof(uint16_t));
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
