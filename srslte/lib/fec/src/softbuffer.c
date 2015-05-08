/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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
#include "srslte/fec/turbodecoder.h"
#include "srslte/fec/rm_turbo.h"
#include "srslte/fec/softbuffer.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"

#define MAX_PDSCH_RE(cp) (2 * SRSLTE_CP_NSYMB(cp) * 12)

int srslte_softbuffer_rx_init(srslte_softbuffer_rx_t *q, srslte_cell_t cell) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  
  if (q != NULL) {    
    ret = SRSLTE_ERROR; 
    
    bzero(q, sizeof(srslte_softbuffer_rx_t));
    
    ret = srslte_ra_tbs_from_idx(26, cell.nof_prb);
    if (ret != SRSLTE_ERROR) {
      q->max_cb =  (uint32_t) ret / (SRSLTE_TCOD_MAX_LEN_CB - 24) + 1; 
      
      q->buffer_f = srslte_vec_malloc(sizeof(float*) * q->max_cb);
      if (!q->buffer_f) {
        perror("malloc");
        return SRSLTE_ERROR;
      }
      
      // FIXME: Use HARQ buffer limitation based on UE category
      q->buff_size = cell.nof_prb * MAX_PDSCH_RE(cell.cp) * 6 * 10;
      for (uint32_t i=0;i<q->max_cb;i++) {
        q->buffer_f[i] = srslte_vec_malloc(sizeof(float) * q->buff_size);
        if (!q->buffer_f[i]) {
          perror("malloc");
          return SRSLTE_ERROR;
        }
      }      
      srslte_softbuffer_rx_reset(q);
      ret = SRSLTE_SUCCESS;
    }
  }
  return ret;
}

void srslte_softbuffer_rx_free(srslte_softbuffer_rx_t *q) {
  if (q) {
    if (q->buffer_f) {
      for (uint32_t i=0;i<q->max_cb;i++) {
        if (q->buffer_f[i]) {
          free(q->buffer_f[i]);
        }
      }
      free(q->buffer_f);
    }
    bzero(q, sizeof(srslte_softbuffer_rx_t));
  }
}

void srslte_softbuffer_rx_reset(srslte_softbuffer_rx_t *q) {
  if (q->buffer_f) {
    for (uint32_t i=0;i<q->max_cb;i++) {
      if (q->buffer_f[i]) {
        for (uint32_t j=0;j<q->buff_size;j++) {
          q->buffer_f[i][j] = SRSLTE_RX_NULL;
        }
      }
    }
  }
}



int srslte_softbuffer_tx_init(srslte_softbuffer_tx_t *q, srslte_cell_t cell) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  
  if (q != NULL) {    
    ret = SRSLTE_ERROR; 
    
    bzero(q, sizeof(srslte_softbuffer_tx_t));
    
    ret = srslte_ra_tbs_from_idx(26, cell.nof_prb);
    if (ret != SRSLTE_ERROR) {
      q->max_cb =  (uint32_t) ret / (SRSLTE_TCOD_MAX_LEN_CB - 24) + 1; 
      
      q->buffer_b = srslte_vec_malloc(sizeof(uint8_t*) * q->max_cb);
      if (!q->buffer_b) {
        perror("malloc");
        return SRSLTE_ERROR;
      }
     
      // FIXME: Use HARQ buffer limitation based on UE category
      q->buff_size = cell.nof_prb * MAX_PDSCH_RE(cell.cp) * 6 * 10;
      for (uint32_t i=0;i<q->max_cb;i++) {
        q->buffer_b[i] = srslte_vec_malloc(sizeof(float) * q->buff_size);
        if (!q->buffer_b[i]) {
          perror("malloc");
          return SRSLTE_ERROR;
        }
      }      
      srslte_softbuffer_tx_reset(q);
      ret = SRSLTE_SUCCESS;
    }
  }
  return ret;
}

void srslte_softbuffer_tx_free(srslte_softbuffer_tx_t *q) {
  if (q) {
    if (q->buffer_b) {
      for (uint32_t i=0;i<q->max_cb;i++) {
        if (q->buffer_b[i]) {
          free(q->buffer_b[i]);
        }
      }
      free(q->buffer_b);
    }
    bzero(q, sizeof(srslte_softbuffer_tx_t));
  }
}

void srslte_softbuffer_tx_reset(srslte_softbuffer_tx_t *q) {
  int i; 
  if (q->buffer_b) {
    for (i=0;i<q->max_cb;i++) {
      if (q->buffer_b[i]) {
        bzero(q->buffer_b[i], sizeof(uint8_t) * q->buff_size);
      }
    }
  }
}
