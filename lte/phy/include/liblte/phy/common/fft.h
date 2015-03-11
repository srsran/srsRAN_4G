/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#ifndef LTEFFT_
#define LTEFFT_


#include <strings.h>
#include <stdlib.h>

#include "liblte/config.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/dft.h"

typedef _Complex float cf_t; /* this is only a shortcut */

/* This is common for both directions */
typedef struct LIBLTE_API{
  dft_plan_t fft_plan;
  uint32_t nof_symbols;
  uint32_t symbol_sz;
  uint32_t nof_guards;
  uint32_t nof_re;
  uint32_t slot_sz;
  lte_cp_t cp;
  cf_t *tmp; // for removing zero padding
  
  bool freq_shift;
  cf_t *shift_buffer; 
}lte_fft_t;

LIBLTE_API int lte_fft_init(lte_fft_t *q, 
                            lte_cp_t cp_type, 
                            uint32_t nof_prb);

LIBLTE_API void lte_fft_free(lte_fft_t *q);

LIBLTE_API void lte_fft_run_slot(lte_fft_t *q, 
                                 cf_t *input, 
                                 cf_t *output);

LIBLTE_API void lte_fft_run_sf(lte_fft_t *q, 
                               cf_t *input, 
                               cf_t *output);

LIBLTE_API int lte_ifft_init(lte_fft_t *q, 
                             lte_cp_t cp_type, 
                             uint32_t nof_prb);

LIBLTE_API void lte_ifft_free(lte_fft_t *q);

LIBLTE_API void lte_ifft_run_slot(lte_fft_t *q, 
                                  cf_t *input, 
                                  cf_t *output);

LIBLTE_API void lte_ifft_run_sf(lte_fft_t *q, 
                                cf_t *input, 
                                cf_t *output);

LIBLTE_API int lte_fft_set_freq_shift(lte_fft_t *q, 
                                      float freq_shift); 

LIBLTE_API void lte_fft_set_normalize(lte_fft_t *q, 
                                      bool normalize_enable); 

#endif
