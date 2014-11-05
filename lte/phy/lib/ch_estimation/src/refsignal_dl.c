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


#include <math.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <complex.h>

#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/ch_estimation/refsignal_dl.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"
#include "liblte/phy/common/sequence.h"

uint32_t refsignal_cs_v(uint32_t port_id, uint32_t ns, uint32_t symbol_id)
{
  uint32_t v = 0;
  switch (port_id) {
    case 0:
      if (symbol_id == 0) {
        v = 0;
      } else {
        v = 3;
      }
      break;
    case 1:
      if (symbol_id == 0) {
        v = 3;
      } else {
        v = 0;
      }
      break;
    case 2:
      v = 3 * (ns % 2);
      break;
    case 3:
      v = 3 + 3 * (ns % 2);
      break;
  }
  return v;
}

uint32_t refsignal_cs_nof_symbols(uint32_t port_id) 
{
    if (port_id < 2) {
      return 2; 
    } else {
      return 1;
    }
}

static uint32_t lp(uint32_t l, lte_cp_t cp) {
  if (l == 1) {
      return CP_NSYMB(cp) - 3;
  } else {
      return 0;
  }
}

/** Allocates and precomputes the Cell-Specific Reference (CSR) signal for 
 * the 20 slots in a subframe
 */
int refsignal_cs_generate(refsignal_cs_t * q, lte_cell_t cell)
{

  uint32_t c_init;
  uint32_t i, ns, l;
  uint32_t N_cp, mp;
  sequence_t seq;
  int ret = LIBLTE_ERROR_INVALID_INPUTS;

  if (q != NULL &&
      lte_cell_isvalid(&cell)) 
  {
    ret = LIBLTE_ERROR; 
    
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
    
    for (i=0;i<NSUBFRAMES_X_FRAME;i++) {
      q->pilots[i] = vec_malloc(sizeof(cf_t) * REFSIGNAL_MAX_NUM_SF(q->cell.nof_prb));
      if (!q->pilots[i]) {
        perror("malloc");
        goto free_and_exit;
      }
    }
    
    for (ns=0;ns<NSLOTS_X_FRAME;ns++) {
      for (l = 0; l < 2; l++) {
        /* Compute sequence init value */
        c_init = 1024 * (7 * (ns + 1) + lp(l,cell.cp) + 1) * (2 * cell.id + 1)
          + 2 * cell.id + N_cp;
        
        /* generate sequence for this symbol and slot */
        sequence_set_LTE_pr(&seq, c_init);
        
        /* Compute signal */
        for (i = 0; i < 2*q->cell.nof_prb; i++) {
          mp = i + MAX_PRB - cell.nof_prb;
          /* save signal */
          q->pilots[ns/2][REFSIGNAL_PILOT_IDX(i,l,ns,q->cell)] = 
                                                (1 - 2 * (float) seq.c[2 * mp]) / sqrt(2) +
                                   _Complex_I * (1 - 2 * (float) seq.c[2 * mp + 1]) / sqrt(2);
        }        
      }
    }
    sequence_free(&seq);
    ret = LIBLTE_SUCCESS;
  }
free_and_exit:
  if (ret == LIBLTE_ERROR) {
    sequence_free(&seq);
    refsignal_cs_free(q);
  }
  return ret;
}

/** Deallocates a refsignal_cs_t object allocated with refsignal_cs_init */
void refsignal_cs_free(refsignal_cs_t * q)
{
  int i; 
  
  for (i=0;i<NSUBFRAMES_X_FRAME;i++) {
    if (q->pilots[i]) {
      free(q->pilots[i]);
    }
  }  
  bzero(q, sizeof(refsignal_cs_t));
}


inline uint32_t refsignal_fidx(lte_cell_t cell, uint32_t ns, uint32_t l, uint32_t port_id, uint32_t m) {
  return 6*m + ((refsignal_cs_v(port_id, ns, lp(l,cell.cp)) + (cell.id % 6)) % 6);
}

inline uint32_t refsignal_nsymbol(lte_cell_t cell, uint32_t ns, uint32_t l) {
  return (ns%2)*CP_NSYMB(cell.cp)+lp(l,cell.cp);
}

/* Maps a reference signal initialized with refsignal_cs_init() into an array of subframe symbols */
int refsignal_cs_put_sf(lte_cell_t cell, uint32_t port_id, uint32_t sf_idx, 
                        cf_t *pilots, cf_t *sf_symbols)
{
  uint32_t i, l, ns;
  uint32_t fidx;

  if (lte_cell_isvalid(&cell)             &&
      lte_sfidx_isvalid(sf_idx)           &&
      lte_portid_isvalid(port_id)         &&
      pilots                      != NULL &&
      sf_symbols                  != NULL) 
  {
    
    for (ns=2*sf_idx;ns<2*(sf_idx+1);ns++) {
      for (l=0;l<refsignal_cs_nof_symbols(port_id);l++) {
        /* Compute offset frequency index */
        fidx = ((refsignal_cs_v(port_id, ns, lp(l,cell.cp)) + (cell.id % 6)) % 6); 
        for (i = 0; i < 2*cell.nof_prb; i++) {
          uint32_t nsymbol = refsignal_nsymbol(cell, ns, l);
          sf_symbols[SAMPLE_IDX(cell.nof_prb, nsymbol, fidx)] = pilots[REFSIGNAL_PILOT_IDX(i,l,ns,cell)];
          fidx += 6;       // 1 reference every 6 RE
          DEBUG("Putting %d to %d (fidx: %d, lp:%d)\n",REFSIGNAL_PILOT_IDX(i,l,ns,cell), SAMPLE_IDX(cell.nof_prb, nsymbol, fidx),
                 fidx, nsymbol);
        }    
      }
    }
    return LIBLTE_SUCCESS;      
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

/** Copies the RE containing references from an array of subframe symbols into the csr_signal[][]. 
 * csr_signal[0] is the signal for the first OFDM symbol containing references and csr_signal[1] is the
 * second OFDM symbol containing references (symbol 4 or 3 if port_id < 2)
 */
int refsignal_cs_get_sf(lte_cell_t cell, uint32_t port_id, uint32_t sf_idx, 
                     cf_t *sf_symbols, cf_t *pilots)
{
  uint32_t i, l, ns;
  uint32_t fidx;
  
  if (lte_cell_isvalid(&cell)             &&
      lte_sfidx_isvalid(sf_idx)           &&
      lte_portid_isvalid(port_id)         &&
      pilots                  != NULL && 
      sf_symbols                  != NULL) 
  {
        
    for (ns=2*sf_idx;ns<2*(sf_idx+1);ns++) {
      for (l=0;l<refsignal_cs_nof_symbols(port_id);l++) {
        /* Compute offset frequency index */
        fidx = ((refsignal_cs_v(port_id, ns, lp(l,cell.cp)) + (cell.id % 6)) % 6); 
        for (i = 0; i < 2*cell.nof_prb; i++) {
          uint32_t nsymbol = refsignal_nsymbol(cell, ns, l);
          pilots[REFSIGNAL_PILOT_IDX(i,l,ns,cell)] = sf_symbols[SAMPLE_IDX(cell.nof_prb, nsymbol, fidx)];
          fidx += 6;       // 1 reference every 6 RE
          DEBUG("Getting %d from %d (fidx: %d, lp:%d)\n",REFSIGNAL_PILOT_IDX(i,l,ns,cell), SAMPLE_IDX(cell.nof_prb, nsymbol, fidx),
                 fidx, nsymbol);
        }    
      }
    }
    return LIBLTE_SUCCESS;      
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }  
}


