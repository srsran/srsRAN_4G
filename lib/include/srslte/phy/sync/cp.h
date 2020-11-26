/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_CP_H
#define SRSLTE_CP_H

#include <complex.h>
#include <stdint.h>

#include "srslte/config.h"

typedef struct {
  cf_t*    corr;
  uint32_t symbol_sz;
  uint32_t max_symbol_sz;
} srslte_cp_synch_t;

SRSLTE_API int srslte_cp_synch_init(srslte_cp_synch_t* q, uint32_t symbol_sz);

SRSLTE_API void srslte_cp_synch_free(srslte_cp_synch_t* q);

SRSLTE_API int srslte_cp_synch_resize(srslte_cp_synch_t* q, uint32_t symbol_sz);

SRSLTE_API uint32_t
           srslte_cp_synch(srslte_cp_synch_t* q, const cf_t* input, uint32_t max_offset, uint32_t nof_symbols, uint32_t cp_len);

SRSLTE_API cf_t srslte_cp_synch_corr_output(srslte_cp_synch_t* q, uint32_t offset);

#endif // SRSLTE_CP_H
