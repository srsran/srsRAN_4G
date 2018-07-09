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

#ifndef SRSLTE_CP_H
#define SRSLTE_CP_H

#include <stdint.h>
#include <complex.h>

#include "srslte/config.h"

typedef struct {
  cf_t *corr;
  uint32_t symbol_sz;
  uint32_t max_symbol_sz;
} srslte_cp_synch_t;

SRSLTE_API int srslte_cp_synch_init(srslte_cp_synch_t *q, 
                                    uint32_t symbol_sz);

SRSLTE_API void srslte_cp_synch_free(srslte_cp_synch_t *q);

SRSLTE_API int srslte_cp_synch_resize(srslte_cp_synch_t *q,
                                      uint32_t symbol_sz);

SRSLTE_API uint32_t srslte_cp_synch(srslte_cp_synch_t *q, 
                                    const cf_t *input,
                                    uint32_t max_offset, 
                                    uint32_t nof_symbols, 
                                    uint32_t cp_len);

SRSLTE_API cf_t srslte_cp_synch_corr_output(srslte_cp_synch_t *q, 
                                            uint32_t offset);

#endif // SRSLTE_CP_H
