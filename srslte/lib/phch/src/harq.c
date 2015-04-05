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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "srslte/common/phy_common.h"
#include "srslte/phch/ra.h"
#include "srslte/phch/harq.h"
#include "srslte/fec/turbodecoder.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"


#define MAX_PDSCH_RE(cp) (2 * SRSLTE_CP_NSYMB(cp) * 12)

/* Calculate Codeblock Segmentation as in Section 5.1.2 of 36.212 */
int srslte_harq_codeblock_segmentation(srslte_harq_cbsegm_t *s, uint32_t tbs) {
  uint32_t Bp, B, idx1;
  int ret; 

  if (tbs == 0) {
    bzero(s, sizeof(srslte_harq_cbsegm_t));
    ret = SRSLTE_SUCCESS;
  } else {
    B = tbs + 24;

    /* Calculate CB sizes */
    if (B <= MAX_LONG_CB) {
      s->C = 1;
      Bp = B;
    } else {
      s->C = (uint32_t) ceilf((float) B / (MAX_LONG_CB - 24));
      Bp = B + 24 * s->C;
    }
    ret = srslte_find_cb_index((Bp-1) / s->C + 1);
    if (ret != SRSLTE_ERROR) {
      idx1 = (uint32_t) ret;
      ret = srslte_cb_size(idx1);
      if (ret != SRSLTE_ERROR) {
        s->K1 = (uint32_t) ret;
        if (idx1 > 0) {
          ret = srslte_cb_size(idx1 - 1);        
        }
        if (ret != SRSLTE_ERROR) {
          if (s->C == 1) {
            s->K2 = 0;
            s->C2 = 0;
            s->C1 = 1;
          } else {
            s->K2 = (uint32_t) ret;
            s->C2 = (s->C * s->K1 - Bp) / (s->K1 - s->K2);
            s->C1 = s->C - s->C2;
          }
          s->F = s->C1 * s->K1 + s->C2 * s->K2 - Bp;
          INFO("CB Segmentation: TBS: %d, C=%d, C+=%d K+=%d, C-=%d, K-=%d, F=%d, Bp=%d\n",
              tbs, s->C, s->C1, s->K1, s->C2, s->K2, s->F, Bp);         
        }
      }
    }  
    
  }
  return ret;
}

int srslte_harq_init(srslte_harq_t *q, srslte_cell_t cell) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  
  if (q != NULL) {    
    uint32_t i;
    bzero(q, sizeof(srslte_harq_t));
    
    memcpy(&q->cell, &cell, sizeof(srslte_cell_t));
    
    ret = srslte_ra_tbs_from_idx(26, cell.nof_prb);
    if (ret != SRSLTE_ERROR) {
      q->max_cb =  (uint32_t) ret / (MAX_LONG_CB - 24) + 1; 
      
      q->pdsch_w_buff_f = srslte_vec_malloc(sizeof(float*) * q->max_cb);
      if (!q->pdsch_w_buff_f) {
        perror("malloc");
        return SRSLTE_ERROR;
      }
      
      q->pdsch_w_buff_c = srslte_vec_malloc(sizeof(uint8_t*) * q->max_cb);
      if (!q->pdsch_w_buff_c) {
        perror("malloc");
        return SRSLTE_ERROR;
      }
      
      // FIXME: Use HARQ buffer limitation based on UE category
      q->w_buff_size = cell.nof_prb * MAX_PDSCH_RE(cell.cp) * 6 * 10;
      for (i=0;i<q->max_cb;i++) {
        q->pdsch_w_buff_f[i] = srslte_vec_malloc(sizeof(float) * q->w_buff_size);
        if (!q->pdsch_w_buff_f[i]) {
          perror("malloc");
          return SRSLTE_ERROR;
        }
        q->pdsch_w_buff_c[i] = srslte_vec_malloc(sizeof(uint8_t) * q->w_buff_size);
        if (!q->pdsch_w_buff_c[i]) {
          perror("malloc");
          return SRSLTE_ERROR;
        }
        bzero(q->pdsch_w_buff_c[i], sizeof(uint8_t) * q->w_buff_size);
      }      
      ret = SRSLTE_SUCCESS;
    }
  }
  return ret;
}

void srslte_harq_free(srslte_harq_t *q) {
  if (q) {
    uint32_t i;
    if (q->pdsch_w_buff_f) {
      for (i=0;i<q->max_cb;i++) {
        if (q->pdsch_w_buff_f[i]) {
          free(q->pdsch_w_buff_f[i]);
        }
      }
      free(q->pdsch_w_buff_f);
    }
    if (q->pdsch_w_buff_c) {
      for (i=0;i<q->max_cb;i++) {
        if (q->pdsch_w_buff_c[i]) {
          free(q->pdsch_w_buff_c[i]);
        }
      }
      free(q->pdsch_w_buff_c);
    }
    bzero(q, sizeof(srslte_harq_t));
  }
}

void srslte_harq_reset(srslte_harq_t *q) {
  int i; 
  if (q->pdsch_w_buff_f) {
    for (i=0;i<q->max_cb;i++) {
      if (q->pdsch_w_buff_f[i]) {
        bzero(q->pdsch_w_buff_f[i], sizeof(float) * q->w_buff_size);
      }
    }
  }
  if (q->pdsch_w_buff_c) {
    for (i=0;i<q->max_cb;i++) {
      if (q->pdsch_w_buff_c[i]) {
        bzero(q->pdsch_w_buff_c[i], sizeof(uint8_t) * q->w_buff_size);
      }
    }
  }
  bzero(&q->mcs, sizeof(srslte_ra_mcs_t));
  bzero(&q->cb_segm, sizeof(srslte_harq_cbsegm_t));
  bzero(&q->dl_alloc, sizeof(srslte_ra_dl_alloc_t));
}

static int harq_setup_common(srslte_harq_t *q, srslte_ra_mcs_t mcs, uint32_t rv, uint32_t sf_idx) {
  if (mcs.tbs != q->mcs.tbs) {
    srslte_harq_codeblock_segmentation(&q->cb_segm, mcs.tbs);          
    if (q->cb_segm.C > q->max_cb) {
      fprintf(stderr, "Codeblock segmentation returned more CBs (%d) than allocated (%d)\n", 
        q->cb_segm.C, q->max_cb);
      return SRSLTE_ERROR;
    }       
  }

  q->mcs = mcs;
  q->sf_idx = sf_idx;
  q->rv = rv;    
  return SRSLTE_SUCCESS;
}

int srslte_harq_setup_dl(srslte_harq_t *q, srslte_ra_mcs_t mcs, uint32_t rv, uint32_t sf_idx, srslte_ra_dl_alloc_t *dl_alloc) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  
  if (q != NULL       && 
      rv         < 4  &&
      sf_idx     < 10)
  {
    ret = harq_setup_common(q, mcs, rv, sf_idx);
    if (ret) {
      return ret;
    }
    memcpy(&q->dl_alloc, dl_alloc, sizeof(srslte_ra_dl_alloc_t));

    // Number of symbols, RE and bits per subframe for DL
    q->nof_re = q->dl_alloc.re_sf[q->sf_idx];
    q->nof_symb = 2*SRSLTE_CP_NSYMB(q->cell.cp)-q->dl_alloc.lstart;
    q->nof_bits = q->nof_re * srslte_mod_bits_x_symbol(q->mcs.mod);
    q->nof_prb = q->dl_alloc.slot[0].nof_prb;

    ret = SRSLTE_SUCCESS;    
  }
  return ret;
}

int srslte_harq_setup_ul(srslte_harq_t *q, srslte_ra_mcs_t mcs, uint32_t rv, uint32_t sf_idx, srslte_ra_ul_alloc_t *ul_alloc) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  
  if (q != NULL       && 
      rv         < 4  &&
      sf_idx     < 10)
  {
    ret = harq_setup_common(q, mcs, rv, sf_idx);
    if (ret) {
      return ret;
    }
    memcpy(&q->ul_alloc, ul_alloc, sizeof(srslte_ra_ul_alloc_t));

    // Number of symbols, RE and bits per subframe for UL
    q->nof_symb = 2*(SRSLTE_CP_NSYMB(q->cell.cp)-1);
    q->nof_re = q->nof_symb*q->ul_alloc.L_prb*SRSLTE_NRE;
    q->nof_bits = q->nof_re * srslte_mod_bits_x_symbol(q->mcs.mod);
    q->nof_prb = q->ul_alloc.L_prb;

    ret = SRSLTE_SUCCESS;    
  }
  return ret;
}
