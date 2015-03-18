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


#ifndef DFTPREC_
#define DFTPREC_

#include "srslte/config.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/dft.h"

typedef _Complex float cf_t;

/* DFT-based Transform Precoding object */
typedef struct SRSLTE_API {

  uint32_t max_prb;  
  dft_plan_t dft_plan[MAX_PRB];
  dft_plan_t idft_plan[MAX_PRB];
    
}dft_precoding_t;

SRSLTE_API int dft_precoding_init(dft_precoding_t *q, 
                                  uint32_t max_prb);

SRSLTE_API void dft_precoding_free(dft_precoding_t *q);

SRSLTE_API bool dft_precoding_valid_prb(uint32_t nof_prb); 

SRSLTE_API int dft_precoding(dft_precoding_t *q, 
                             cf_t *input, 
                             cf_t *output, 
                             uint32_t nof_prb, 
                             uint32_t nof_symbols);

SRSLTE_API int dft_predecoding(dft_precoding_t *q, 
                               cf_t *input, 
                               cf_t *output, 
                               uint32_t nof_prb, 
                               uint32_t nof_symbols);

#endif
