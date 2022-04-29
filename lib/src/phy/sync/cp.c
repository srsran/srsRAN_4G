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

#include "srsran/srsran.h"
#include <stdlib.h>

#include "srsran/phy/sync/cp.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

int srsran_cp_synch_init(srsran_cp_synch_t* q, uint32_t symbol_sz)
{
  q->symbol_sz     = symbol_sz;
  q->max_symbol_sz = symbol_sz;

  q->corr = srsran_vec_cf_malloc(q->symbol_sz);
  if (!q->corr) {
    perror("malloc");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

void srsran_cp_synch_free(srsran_cp_synch_t* q)
{
  if (q->corr) {
    free(q->corr);
  }
}

int srsran_cp_synch_resize(srsran_cp_synch_t* q, uint32_t symbol_sz)
{
  if (symbol_sz > q->max_symbol_sz) {
    ERROR("Error in cp_synch_resize(): symbol_sz must be lower than initialized");
    return SRSRAN_ERROR;
  }
  q->symbol_sz = symbol_sz;

  return SRSRAN_SUCCESS;
}

uint32_t
srsran_cp_synch(srsran_cp_synch_t* q, const cf_t* input, uint32_t max_offset, uint32_t nof_symbols, uint32_t cp_len)
{
  if (max_offset > q->symbol_sz) {
    max_offset = q->symbol_sz;
  }
  for (int i = 0; i < max_offset; i++) {
    q->corr[i]           = 0;
    const cf_t* inputPtr = input;
    for (int n = 0; n < nof_symbols; n++) {
      uint32_t cplen = (n % 7) ? cp_len : cp_len + 1;
      q->corr[i] += srsran_vec_dot_prod_conj_ccc(&inputPtr[i], &inputPtr[i + q->symbol_sz], cplen) / nof_symbols;
      inputPtr += q->symbol_sz + cplen;
    }
  }
  uint32_t max_idx = srsran_vec_max_abs_ci(q->corr, max_offset);
  return max_idx;
}

cf_t srsran_cp_synch_corr_output(srsran_cp_synch_t* q, uint32_t offset)
{
  if (offset < q->symbol_sz) {
    return q->corr[offset];
  } else {
    return 0;
  }
}
