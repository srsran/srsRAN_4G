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
