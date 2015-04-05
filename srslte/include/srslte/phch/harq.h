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

/******************************************************************************
 *  File:         harq.h
 *
 *  Description:  Hybrid Automatic Repeat Request (HARQ)
 *
 *  Reference:
 *****************************************************************************/

#ifndef HARQ_
#define HARQ_

#include "srslte/config.h"
#include "srslte/common/phy_common.h"
#include "srslte/phch/ra.h"


 typedef struct SRSLTE_API {
  uint32_t F;
  uint32_t C;
  uint32_t K1;
  uint32_t K2;
  uint32_t C1;
  uint32_t C2;
} srslte_harq_cbsegm_t;

typedef struct SRSLTE_API {
  srslte_ra_mcs_t mcs;
  uint32_t rv;
  uint32_t sf_idx;
  srslte_ra_dl_alloc_t dl_alloc;
  srslte_ra_ul_alloc_t ul_alloc;
  srslte_cell_t cell;
  
  uint32_t nof_re;   // Number of RE per subframe 
  uint32_t nof_bits; // Number of bits per subframe
  uint32_t nof_symb; // Number of symbols per subframe 
  uint32_t nof_prb;  // Number of allocated PRB per subframe. 
  
  uint32_t max_cb;
  uint32_t w_buff_size;
  float **pdsch_w_buff_f;  
  uint8_t **pdsch_w_buff_c;  
  
  srslte_harq_cbsegm_t cb_segm;

} srslte_harq_t;

SRSLTE_API int srslte_harq_init(srslte_harq_t * q,
                                srslte_cell_t cell);

SRSLTE_API int srslte_harq_setup_dl(srslte_harq_t *p, 
                                    srslte_ra_mcs_t mcs,
                                    uint32_t rv,
                                    uint32_t sf_idx,
                                    srslte_ra_dl_alloc_t *prb_alloc);

SRSLTE_API int srslte_harq_setup_ul(srslte_harq_t *p, 
                                    srslte_ra_mcs_t mcs,
                                    uint32_t rv,
                                    uint32_t sf_idx,
                                    srslte_ra_ul_alloc_t *prb_alloc);

SRSLTE_API void srslte_harq_reset(srslte_harq_t *p); 

SRSLTE_API void srslte_harq_free(srslte_harq_t *p);

SRSLTE_API int srslte_harq_codeblock_segmentation(srslte_harq_cbsegm_t *s, 
                                                  uint32_t tbs); 


#endif
