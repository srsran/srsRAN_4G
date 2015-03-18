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


#include <math.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <complex.h>

#include "srslte/common/phy_common.h"
#include "srslte/ch_estimation/refsignal_dl.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"
#include "srslte/common/sequence.h"

uint32_t refsignal_cs_v(uint32_t port_id, uint32_t ref_symbol_idx)
{
  uint32_t v = 0;
  switch (port_id) {
    case 0:
      if (!(ref_symbol_idx % 2)) {
        v = 0;
      } else {
        v = 3;
      }
      break;
    case 1:
      if (!(ref_symbol_idx % 2)) {
        v = 3;
      } else {
        v = 0;
      }
      break;
    case 2:
      if (ref_symbol_idx < 2) {
        v = 0;
      } else {
        v = 3;
      }
      break;
    case 3:
      if (ref_symbol_idx < 2) {
        v = 3;
      } else {
        v = 6;
      }
      break;
  }
  return v;
}

uint32_t refsignal_cs_nof_symbols(uint32_t port_id) 
{
  if (port_id < 2) {
    return 4; 
  } else {
    return 2;
  }
}

inline uint32_t refsignal_cs_fidx(lte_cell_t cell, uint32_t l, uint32_t port_id, uint32_t m) {
  return 6*m + ((refsignal_cs_v(port_id, l) + (cell.id % 6)) % 6);
}

inline uint32_t refsignal_cs_nsymbol(uint32_t l, lte_cp_t cp, uint32_t port_id) {
  if (port_id < 2) {
    if (l % 2) {
        return (l/2+1)*CP_NSYMB(cp) - 3;
    } else {
        return (l/2)*CP_NSYMB(cp);
    }    
  } else {
    return 1+l*CP_NSYMB(cp);
  }
}


/** Allocates and precomputes the Cell-Specific Reference (CSR) signal for 
 * the 20 slots in a subframe
 */
int refsignal_cs_init(refsignal_cs_t * q, lte_cell_t cell)
{

  uint32_t c_init;
  uint32_t i, ns, l, p;
  uint32_t N_cp, mp;
  sequence_t seq;
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL &&
      lte_cell_isvalid(&cell)) 
  {
    ret = SRSLTE_ERROR; 
    
    bzero(q, sizeof(refsignal_cs_t));
    bzero(&seq, sizeof(sequence_t));
    if (sequence_init(&seq, 2 * 2 * MAX_PRB)) {
      goto free_and_exit;
    }
    
    if (CP_ISNORM(cell.cp)) {
      N_cp = 1;
    } else {
      N_cp = 0;
    }
    
    q->cell = cell; 
    
    for (p=0;p<2;p++) {
      for (i=0;i<NSUBFRAMES_X_FRAME;i++) {
        q->pilots[p][i] = vec_malloc(sizeof(cf_t) * REFSIGNAL_NUM_SF(q->cell.nof_prb, 2*p));
        if (!q->pilots[p][i]) {
          perror("malloc");
          goto free_and_exit;
        }
      }      
    }
    
    for (ns=0;ns<NSLOTS_X_FRAME;ns++) {
      for (p=0;p<2;p++) {
        uint32_t nsymbols = refsignal_cs_nof_symbols(2*p)/2;
        for (l = 0; l < nsymbols; l++) {
          /* Compute sequence init value */
          uint32_t lp = refsignal_cs_nsymbol(l, cell.cp, 2*p);
          c_init = 1024 * (7 * (ns + 1) + lp + 1) * (2 * cell.id + 1)
            + 2 * cell.id + N_cp;
          
          /* generate sequence for this symbol and slot */
          sequence_set_LTE_pr(&seq, c_init);
          
          /* Compute signal */
          for (i = 0; i < 2*q->cell.nof_prb; i++) {
            mp = i + MAX_PRB - cell.nof_prb;
            /* save signal */
            q->pilots[p][ns/2][REFSIGNAL_PILOT_IDX(i,(ns%2)*nsymbols+l,q->cell)] = 
                                                  (1 - 2 * (float) seq.c[2 * mp]) / sqrt(2) +
                                    _Complex_I * (1 - 2 * (float) seq.c[2 * mp + 1]) / sqrt(2);
          }        
        }
        
      }
    }
    sequence_free(&seq);
    ret = SRSLTE_SUCCESS;
  }
free_and_exit:
  if (ret == SRSLTE_ERROR) {
    sequence_free(&seq);
    refsignal_cs_free(q);
  }
  return ret;
}

/** Deallocates a refsignal_cs_t object allocated with refsignal_cs_init */
void refsignal_cs_free(refsignal_cs_t * q)
{
  int i, p; 
  
  for (p=0;p<2;p++) {
    for (i=0;i<NSUBFRAMES_X_FRAME;i++) {
      if (q->pilots[p][i]) {
        free(q->pilots[p][i]);
      }
    }      
  }
  bzero(q, sizeof(refsignal_cs_t));
}


/* Maps a reference signal initialized with refsignal_cs_init() into an array of subframe symbols */
int refsignal_cs_put_sf(lte_cell_t cell, uint32_t port_id, cf_t *pilots, cf_t *sf_symbols)
{
  uint32_t i, l;
  uint32_t fidx;

  if (lte_cell_isvalid(&cell)             &&
      lte_portid_isvalid(port_id)         &&
      pilots                      != NULL &&
      sf_symbols                  != NULL) 
  {
    
    for (l=0;l<refsignal_cs_nof_symbols(port_id);l++) {
      uint32_t nsymbol = refsignal_cs_nsymbol(l, cell.cp, port_id);
      /* Compute offset frequency index */
      fidx = ((refsignal_cs_v(port_id, l) + (cell.id % 6)) % 6); 
      for (i = 0; i < 2*cell.nof_prb; i++) {
        sf_symbols[RE_IDX(cell.nof_prb, nsymbol, fidx)] = pilots[REFSIGNAL_PILOT_IDX(i,l,cell)];
        fidx += RE_X_RB/2;       // 1 reference every 6 RE        
      }    
    }
    return SRSLTE_SUCCESS;      
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

/** Copies the RE containing references from an array of subframe symbols into the csr_signal[][]. 
 * csr_signal[0] is the signal for the first OFDM symbol containing references and csr_signal[1] is the
 * second OFDM symbol containing references (symbol 4 or 3 if port_id < 2)
 */
int refsignal_cs_get_sf(lte_cell_t cell, uint32_t port_id, cf_t *sf_symbols, cf_t *pilots)
{
  uint32_t i, l;
  uint32_t fidx;
  
  if (lte_cell_isvalid(&cell)             &&
      lte_portid_isvalid(port_id)         &&
      pilots                  != NULL && 
      sf_symbols                  != NULL) 
  {       
    for (l=0;l<refsignal_cs_nof_symbols(port_id);l++) {
      uint32_t nsymbol = refsignal_cs_nsymbol(l, cell.cp, port_id);
      /* Compute offset frequency index */
      fidx = ((refsignal_cs_v(port_id, l) + (cell.id % 6)) % 6); 
      for (i = 0; i < 2*cell.nof_prb; i++) {
        pilots[REFSIGNAL_PILOT_IDX(i,l,cell)] = sf_symbols[RE_IDX(cell.nof_prb, nsymbol, fidx)];
        fidx += RE_X_RB/2;       // 2 references per PRB
      }
    }
    return SRSLTE_SUCCESS;      
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }  
}


