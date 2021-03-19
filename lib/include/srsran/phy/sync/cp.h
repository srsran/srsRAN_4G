/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_CP_H
#define SRSRAN_CP_H

#include <complex.h>
#include <stdint.h>

#include "srsran/config.h"

typedef struct {
  cf_t*    corr;
  uint32_t symbol_sz;
  uint32_t max_symbol_sz;
} srsran_cp_synch_t;

SRSRAN_API int srsran_cp_synch_init(srsran_cp_synch_t* q, uint32_t symbol_sz);

SRSRAN_API void srsran_cp_synch_free(srsran_cp_synch_t* q);

SRSRAN_API int srsran_cp_synch_resize(srsran_cp_synch_t* q, uint32_t symbol_sz);

SRSRAN_API uint32_t
srsran_cp_synch(srsran_cp_synch_t* q, const cf_t* input, uint32_t max_offset, uint32_t nof_symbols, uint32_t cp_len);

SRSRAN_API cf_t srsran_cp_synch_corr_output(srsran_cp_synch_t* q, uint32_t offset);

#endif // SRSRAN_CP_H
