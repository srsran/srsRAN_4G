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


#ifndef DFTPREC_
#define DFTPREC_

#include "liblte/config.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/dft.h"

typedef _Complex float cf_t;

/* DFT-based Transform Precoding object */
typedef struct LIBLTE_API {

  uint32_t max_prb;  
  dft_plan_t dft_plan[MAX_PRB];
  dft_plan_t idft_plan[MAX_PRB];
    
}dft_precoding_t;

LIBLTE_API int dft_precoding_init(dft_precoding_t *q, 
                                  uint32_t max_prb);

LIBLTE_API void dft_precoding_free(dft_precoding_t *q);

LIBLTE_API bool dft_precoding_valid_prb(uint32_t nof_prb); 

LIBLTE_API int dft_precoding(dft_precoding_t *q, 
                             cf_t *input, 
                             cf_t *output, 
                             uint32_t nof_prb, 
                             uint32_t nof_symbols);

LIBLTE_API int dft_predecoding(dft_precoding_t *q, 
                               cf_t *input, 
                               cf_t *output, 
                               uint32_t nof_prb, 
                               uint32_t nof_symbols);

#endif
