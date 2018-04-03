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

/**********************************************************************************************
 *  File:         dft_precoding.h
 *
 *  Description:  DFT-based transform precoding object.
 *                Used in generation of uplink SCFDMA signals.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.3.3
 *********************************************************************************************/

#ifndef SRSLTE_DFT_PRECODING_H
#define SRSLTE_DFT_PRECODING_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/dft.h"

/* DFT-based Transform Precoding object */
typedef struct SRSLTE_API {

  uint32_t max_prb;  
  srslte_dft_plan_t dft_plan[SRSLTE_MAX_PRB+1];

}srslte_dft_precoding_t;

SRSLTE_API int srslte_dft_precoding_init(srslte_dft_precoding_t *q,
                                         uint32_t max_prb,
                                         bool is_tx);

SRSLTE_API int srslte_dft_precoding_init_tx(srslte_dft_precoding_t *q,
                                            uint32_t max_prb);

SRSLTE_API int srslte_dft_precoding_init_rx(srslte_dft_precoding_t *q,
                                            uint32_t max_prb);

SRSLTE_API void srslte_dft_precoding_free(srslte_dft_precoding_t *q);

SRSLTE_API bool srslte_dft_precoding_valid_prb(uint32_t nof_prb); 

SRSLTE_API int srslte_dft_precoding(srslte_dft_precoding_t *q, 
                                    cf_t *input, 
                                    cf_t *output, 
                                    uint32_t nof_prb, 
                                    uint32_t nof_symbols);

#endif // SRSLTE_DFT_PRECODING_H
