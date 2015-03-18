/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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

#include "srslte/config.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/dft.h"

typedef _Complex float cf_t; /* this is only a shortcut */

/* This is common for both directions */
typedef struct SRSLTE_API{
  srslte_dft_plan_t fft_plan;
  uint32_t nof_symbols;
  uint32_t symbol_sz;
  uint32_t nof_guards;
  uint32_t nof_re;
  uint32_t slot_sz;
  srslte_cp_t cp;
  cf_t *tmp; // for removing zero padding
  
  bool freq_shift;
  cf_t *shift_buffer; 
}srslte_fft_t;

SRSLTE_API int srslte_fft_init(srslte_fft_t *q, 
                               srslte_cp_t cp_type, 
                               uint32_t nof_prb);

SRSLTE_API void srslte_fft_free(srslte_fft_t *q);

SRSLTE_API void srslte_fft_run_slot(srslte_fft_t *q, 
                                    cf_t *input, 
                                    cf_t *output);

SRSLTE_API void srslte_fft_run_sf(srslte_fft_t *q, 
                                  cf_t *input, 
                                  cf_t *output);

SRSLTE_API int lte_ifft_init(srslte_fft_t *q, 
                             srslte_cp_t cp_type, 
                             uint32_t nof_prb);

SRSLTE_API void lte_ifft_free(srslte_fft_t *q);

SRSLTE_API void lte_ifft_run_slot(srslte_fft_t *q, 
                                  cf_t *input, 
                                  cf_t *output);

SRSLTE_API void lte_ifft_run_sf(srslte_fft_t *q, 
                                cf_t *input, 
                                cf_t *output);

SRSLTE_API int srslte_fft_set_freq_shift(srslte_fft_t *q, 
                                         float freq_shift); 

SRSLTE_API void srslte_fft_set_normalize(srslte_fft_t *q, 
                                         bool normalize_enable); 

#endif
