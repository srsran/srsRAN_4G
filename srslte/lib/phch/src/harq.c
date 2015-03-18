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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/phch/harq.h"
#include "srslte/phy/fec/turbodecoder.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/debug.h"


#define MAX_PDSCH_RE(cp) (2 * CP_NSYMB(cp) * 12)

/* Calculate Codeblock Segmentation as in Section 5.1.2 of 36.212 */
int codeblock_segmentation(struct cb_segm *s, uint32_t tbs) {
  uint32_t Bp, B, idx1;
  int ret; 

  if (tbs == 0) {
    bzero(s, sizeof(struct cb_segm));
    ret = LIBLTE_SUCCESS;
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
    ret = lte_find_cb_index((Bp-1) / s->C + 1);
    if (ret != LIBLTE_ERROR) {
      idx1 = (uint32_t) ret;
      ret = lte_cb_size(idx1);
      if (ret != LIBLTE_ERROR) {
        s->K1 = (uint32_t) ret;
        if (idx1 > 0) {
          ret = lte_cb_size(idx1 - 1);        
        }
        if (ret != LIBLTE_ERROR) {
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

int harq_init(harq_t *q, lte_cell_t cell) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q != NULL) {    
    uint32_t i;
    bzero(q, sizeof(harq_t));
    
    memcpy(&q->cell, &cell, sizeof(lte_cell_t));
    
    ret = ra_tbs_from_idx(26, cell.nof_prb);
    if (ret != LIBLTE_ERROR) {
      q->max_cb =  (uint32_t) ret / (MAX_LONG_CB - 24) + 1; 
      
      q->pdsch_w_buff_f = vec_malloc(sizeof(float*) * q->max_cb);
      if (!q->pdsch_w_buff_f) {
        perror("malloc");
        return LIBLTE_ERROR;
      }
      
      q->pdsch_w_buff_c = vec_malloc(sizeof(uint8_t*) * q->max_cb);
      if (!q->pdsch_w_buff_c) {
        perror("malloc");
        return LIBLTE_ERROR;
      }
      
      // FIXME: Use HARQ buffer limitation based on UE category
      q->w_buff_size = cell.nof_prb * MAX_PDSCH_RE(cell.cp) * 6 * 10;
      for (i=0;i<q->max_cb;i++) {
        q->pdsch_w_buff_f[i] = vec_malloc(sizeof(float) * q->w_buff_size);
        if (!q->pdsch_w_buff_f[i]) {
          perror("malloc");
          return LIBLTE_ERROR;
        }
        q->pdsch_w_buff_c[i] = vec_malloc(sizeof(uint8_t) * q->w_buff_size);
        if (!q->pdsch_w_buff_c[i]) {
          perror("malloc");
          return LIBLTE_ERROR;
        }
        bzero(q->pdsch_w_buff_c[i], sizeof(uint8_t) * q->w_buff_size);
      }      
      ret = LIBLTE_SUCCESS;
    }
  }
  return ret;
}

void harq_free(harq_t *q) {
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
    bzero(q, sizeof(harq_t));
  }
}

void harq_reset(harq_t *q) {
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
  bzero(&q->mcs, sizeof(ra_mcs_t));
  bzero(&q->cb_segm, sizeof(struct cb_segm));
  bzero(&q->dl_alloc, sizeof(ra_dl_alloc_t));
}

static int harq_setup_common(harq_t *q, ra_mcs_t mcs, uint32_t rv, uint32_t sf_idx) {
  if (mcs.tbs != q->mcs.tbs) {
    codeblock_segmentation(&q->cb_segm, mcs.tbs);          
    if (q->cb_segm.C > q->max_cb) {
      fprintf(stderr, "Codeblock segmentation returned more CBs (%d) than allocated (%d)\n", 
        q->cb_segm.C, q->max_cb);
      return LIBLTE_ERROR;
    }       
  }

  q->mcs = mcs;
  q->sf_idx = sf_idx;
  q->rv = rv;    
  return LIBLTE_SUCCESS;
}

int harq_setup_dl(harq_t *q, ra_mcs_t mcs, uint32_t rv, uint32_t sf_idx, ra_dl_alloc_t *dl_alloc) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q != NULL       && 
      rv         < 4  &&
      sf_idx     < 10)
  {
    ret = harq_setup_common(q, mcs, rv, sf_idx);
    if (ret) {
      return ret;
    }
    memcpy(&q->dl_alloc, dl_alloc, sizeof(ra_dl_alloc_t));

    // Number of symbols, RE and bits per subframe for DL
    q->nof_re = q->dl_alloc.re_sf[q->sf_idx];
    q->nof_symb = 2*CP_NSYMB(q->cell.cp)-q->dl_alloc.lstart;
    q->nof_bits = q->nof_re * lte_mod_bits_x_symbol(q->mcs.mod);
    q->nof_prb = q->dl_alloc.slot[0].nof_prb;

    ret = LIBLTE_SUCCESS;    
  }
  return ret;
}

int harq_setup_ul(harq_t *q, ra_mcs_t mcs, uint32_t rv, uint32_t sf_idx, ra_ul_alloc_t *ul_alloc) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q != NULL       && 
      rv         < 4  &&
      sf_idx     < 10)
  {
    ret = harq_setup_common(q, mcs, rv, sf_idx);
    if (ret) {
      return ret;
    }
    memcpy(&q->ul_alloc, ul_alloc, sizeof(ra_ul_alloc_t));

    // Number of symbols, RE and bits per subframe for UL
    q->nof_symb = 2*(CP_NSYMB(q->cell.cp)-1);
    q->nof_re = q->nof_symb*q->ul_alloc.L_prb*RE_X_RB;
    q->nof_bits = q->nof_re * lte_mod_bits_x_symbol(q->mcs.mod);
    q->nof_prb = q->ul_alloc.L_prb;

    ret = LIBLTE_SUCCESS;    
  }
  return ret;
}
