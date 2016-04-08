/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

#include <stdlib.h>

#include "srslte/sync/cp.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"

int srslte_cp_synch_init(srslte_cp_synch_t *q, uint32_t symbol_sz) 
{
  q->symbol_sz = symbol_sz;
  
  q->corr = srslte_vec_malloc(sizeof(cf_t) * q->symbol_sz);
  if (!q->corr) {
    perror("malloc");
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

void srslte_cp_synch_free(srslte_cp_synch_t *q)
{
  if (q->corr) {
    free(q->corr);
  }
}

uint32_t srslte_cp_synch(srslte_cp_synch_t *q, cf_t *input, uint32_t max_offset, uint32_t nof_symbols, uint32_t cp_len)
{  
  if (max_offset > q->symbol_sz) {    
    max_offset = q->symbol_sz; 
  }
  for (int i=0;i<max_offset;i++) {
    q->corr[i] = 0;
    cf_t *inputPtr = input; 
    for (int n=0;n<nof_symbols;n++) {
      uint32_t cplen = (n%7)?cp_len:cp_len+1;
      q->corr[i] += srslte_vec_dot_prod_conj_ccc(&inputPtr[i], &inputPtr[i+q->symbol_sz], cplen)/nof_symbols;
      inputPtr += q->symbol_sz+cplen;        
    }    
  }
  uint32_t max_idx = srslte_vec_max_abs_ci(q->corr, max_offset);
  return max_idx; 
}

cf_t srslte_cp_synch_corr_output(srslte_cp_synch_t *q, uint32_t offset) 
{
  if (offset < q->symbol_sz) {
    return q->corr[offset];
  } else {
    return 0;
  }
}

