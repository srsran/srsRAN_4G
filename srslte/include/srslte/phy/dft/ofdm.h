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


#ifndef LTEFFT_
#define LTEFFT_

/**********************************************************************************************
 *  File:         ofdm.h
 *
 *  Description:  OFDM modulation object.
 *                Used in generation of downlink OFDM signals.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6
 *********************************************************************************************/

#include <strings.h>
#include <stdlib.h>

#include "srslte/config.h"
#include "srslte/common/phy_common.h"
#include "srslte/dft/dft.h"

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
}srslte_ofdm_t;

SRSLTE_API int srslte_ofdm_init_(srslte_ofdm_t *q, 
                                 srslte_cp_t cp, 
                                 int symbol_sz, 
                                 int nof_prb, 
                                 srslte_dft_dir_t dir); 

SRSLTE_API int srslte_ofdm_rx_init(srslte_ofdm_t *q, 
                               srslte_cp_t cp_type, 
                               uint32_t nof_prb);

SRSLTE_API void srslte_ofdm_rx_free(srslte_ofdm_t *q);

SRSLTE_API void srslte_ofdm_rx_slot(srslte_ofdm_t *q, 
                                    cf_t *input, 
                                    cf_t *output);

SRSLTE_API void srslte_ofdm_rx_sf(srslte_ofdm_t *q, 
                                  cf_t *input, 
                                  cf_t *output);



SRSLTE_API int srslte_ofdm_tx_init(srslte_ofdm_t *q, 
                                    srslte_cp_t cp_type, 
                                    uint32_t nof_prb);

SRSLTE_API void srslte_ofdm_tx_free(srslte_ofdm_t *q);

SRSLTE_API void srslte_ofdm_tx_slot(srslte_ofdm_t *q, 
                                  cf_t *input, 
                                  cf_t *output);

SRSLTE_API void srslte_ofdm_tx_sf(srslte_ofdm_t *q, 
                                cf_t *input, 
                                cf_t *output);

SRSLTE_API int srslte_ofdm_set_freq_shift(srslte_ofdm_t *q, 
                                         float freq_shift); 

SRSLTE_API void srslte_ofdm_set_normalize(srslte_ofdm_t *q, 
                                         bool normalize_enable); 

#endif
