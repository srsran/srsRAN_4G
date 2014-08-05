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


#include <strings.h>
#include <string.h>
#include <complex.h>
#include <math.h>

#include "liblte/phy/ch_estimation/chest.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

#define SLOT_SZ(q) (q->nof_symbols * q->symbol_sz)
#define SF_SZ(q) (2 * SLOT_SZ(q))

#define VOLK_INTERP

void chest_fprint(chest_t *q, FILE *stream, uint32_t nslot, uint32_t port_id) {
  chest_ref_fprint(q, stream, nslot, port_id);
  chest_recvsig_fprint(q, stream, nslot, port_id);
  chest_ce_fprint(q, stream, nslot, port_id);
}

/* Sets the number of ports to estimate. nof_ports must be smaler than nof_ports
 * used during the call to chest_init(). 
 */
int chest_set_nof_ports(chest_t *q, uint32_t nof_ports) {
  if (nof_ports < q->nof_ports) {
    q->nof_ports = nof_ports;
    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

void chest_ref_fprint(chest_t *q, FILE *stream, uint32_t nslot, uint32_t port_id) {
  int i;
  fprintf(stream, "refs%d=[",port_id);
  for (i=0;i<q->refsignal[port_id][nslot].nof_refs;i++) {
    fprintf(stream, "%3.3f%+3.3fi, ", __real__ q->refsignal[port_id][nslot].refs[i].simbol,
        __imag__ q->refsignal[port_id][nslot].refs[i].simbol);
  }
  fprintf(stream, "];\n");
}

void chest_recvsig_fprint(chest_t *q, FILE *stream, uint32_t nslot, uint32_t port_id) {
  int i;
  fprintf(stream, "recvsig%d=[",port_id);
  for (i=0;i<q->refsignal[port_id][nslot].nof_refs;i++) {
    fprintf(stream, "%3.3f%+3.3fi, ", __real__ q->refsignal[port_id][nslot].refs[i].recv_simbol,
        __imag__ q->refsignal[port_id][nslot].refs[i].recv_simbol);
  }
  fprintf(stream, "];\n");
}

void chest_ce_fprint(chest_t *q, FILE *stream, uint32_t nslot, uint32_t port_id) {
  int i;
  fprintf(stream, "mag%d=[",port_id);
  for (i=0;i<q->refsignal[port_id][nslot].nof_refs;i++) {
    fprintf(stream, "%3.3f, ", cabsf(q->refsignal[port_id][nslot].ch_est[i]));
  }
  fprintf(stream, "];\nphase%d=[",port_id);
  for (i=0;i<q->refsignal[port_id][nslot].nof_refs;i++) {
    fprintf(stream, "%3.3f, ", atan2f(__imag__ q->refsignal[port_id][nslot].ch_est[i],
        __real__ q->refsignal[port_id][nslot].ch_est[i]));
  }
  fprintf(stream, "];\n");
}

int chest_ce_ref(chest_t *q, cf_t *input, uint32_t nslot, uint32_t port_id, uint32_t nref) {
  int fidx, tidx;
  cf_t known_ref, channel_ref;
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q         != NULL            &&
      input     != NULL            &&
      nslot     <  NSLOTS_X_FRAME  &&
      port_id   <  q->nof_ports) 
  {
    if (nref < q->refsignal[port_id][nslot].nof_refs) {

      fidx = q->refsignal[port_id][nslot].refs[nref].freq_idx; // reference frequency index
      tidx = q->refsignal[port_id][nslot].refs[nref].time_idx; // reference time index

      known_ref = q->refsignal[port_id][nslot].refs[nref].simbol;
      channel_ref = input[tidx * q->nof_re + fidx];
      q->refsignal[port_id][nslot].refs[nref].recv_simbol = channel_ref;

      
      DEBUG("Reference %2d pos (%2d,%2d)=%3d %.2f dB %.2f/%.2f=%.2f\n", nref, tidx, fidx, tidx * q->nof_re + fidx,          
            10*log10f(cabsf(channel_ref/known_ref)),          
            cargf(channel_ref)/M_PI,cargf(known_ref)/M_PI,
            cargf(channel_ref/known_ref)/M_PI);
      
      
      /* FIXME: compare with threshold */
      if (channel_ref != 0) {
        q->refsignal[port_id][nslot].ch_est[nref] = channel_ref/known_ref;
      } else {
        q->refsignal[port_id][nslot].ch_est[nref] = 1e-6;
      }
      ret = LIBLTE_SUCCESS;
    }
  }
  return ret;
}

/* Computes channel estimates for each reference in a slot and port.
 * Saves the nof_prb * 12 * nof_symbols channel estimates in the array ce
 */
int chest_ce_slot_port(chest_t *q, cf_t *input, cf_t *ce, uint32_t nslot, uint32_t port_id) {
  uint32_t i, j;
  cf_t x[2], y[MAX_NSYMB];

  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q         != NULL            &&
      input     != NULL            &&
      nslot     <  NSLOTS_X_FRAME  &&
      port_id   <  q->nof_ports) 
  {
    if (q->refsignal[port_id][nslot].nsymbols <= 2) {
      refsignal_t *r = &q->refsignal[port_id][nslot];

      DEBUG("Estimating channel slot=%d port=%d using %d reference signals\n",
          nslot, port_id, r->nof_refs);

      for (i=0;i<r->nof_refs;i++) {
        chest_ce_ref(q, input, nslot, port_id, i);
      }

      /* interpolate the symbols with references
      * in the freq domain */
      for (i=0;i<r->nsymbols;i++) {
#ifdef VOLK_INTERP
        interp_run_offset(&q->interp_freq[port_id], 
                          &r->ch_est[i * r->nof_refs/2], &ce[r->symbols_ref[i] * q->nof_re], 
                          r->voffset, RE_X_RB/2-r->voffset);
#else
        interp_linear_offset(&r->ch_est[i * r->nof_refs/2],
            &ce[r->symbols_ref[i] * q->nof_re], RE_X_RB/2,
            r->nof_refs/2, r->voffset, RE_X_RB/2-r->voffset);
#endif
      }
      /* now interpolate in the time domain */
      for (i=0;i<q->nof_re; i++) {
        if (r->nsymbols > 1) {
          for (j=0;j<r->nsymbols;j++) {
            x[j] = ce[r->symbols_ref[j] * q->nof_re + i];
          }
#ifdef VOLK_INTERP
          interp_run_offset(&q->interp_time[port_id], x, y, 
                            r->symbols_ref[0], 3);
#else
          interp_linear_offset(x, y, r->symbols_ref[1]-r->symbols_ref[0],
              2, r->symbols_ref[0], 3);
#endif
        } else {
          for (j=0;j<MAX_NSYMB;j++) {
            y[j] = ce[r->symbols_ref[0] * q->nof_re + i];
          }
        }       
        for (j=0;j<q->nof_symbols;j++) {
          ce[j * q->nof_re + i] = y[j];
        }
      }
      ret = LIBLTE_SUCCESS;
    }
  }
  return ret;
}


/* Computes channel estimates for each reference in a subframe and port id.
 */
int chest_ce_sf_port(chest_t *q, cf_t *input, cf_t *ce, uint32_t sf_idx, uint32_t port_id) {
  int n, slotsz, ret;
  slotsz = q->nof_symbols*q->nof_re;
  for (n=0;n<2;n++) {
    ret = chest_ce_slot_port(q, &input[n*slotsz], &ce[n*slotsz], 2*sf_idx+n, port_id);
    if (ret != LIBLTE_SUCCESS) {
      return ret;
    }
  }
  return LIBLTE_SUCCESS;
}

/* Computes channel estimates for each reference in a slot for all ports.
 */
int chest_ce_slot(chest_t *q, cf_t *input, cf_t **ce, uint32_t nslot) {
  int p, ret;
  for (p=0;p<q->nof_ports;p++) {
    ret = chest_ce_slot_port(q, input, ce[p], nslot, p);
    if (ret != LIBLTE_SUCCESS) {
      return ret;
    }
  }
  return LIBLTE_SUCCESS;
}

/* Computes channel estimates for each reference in a subframe for all ports.
 */
int chest_ce_sf(chest_t *q, cf_t *input, cf_t *ce[MAX_PORTS], uint32_t sf_idx) {
  int p, n, slotsz, ret;
  slotsz = q->nof_symbols*q->nof_re;
  for (p=0;p<q->nof_ports;p++) {
    for (n=0;n<2;n++) {
      ret = chest_ce_slot_port(q, &input[n*slotsz], &ce[p][n*slotsz], 2*sf_idx+n, p);
      if (ret != LIBLTE_SUCCESS) {
        return ret;
      }
    }
  }
  return LIBLTE_SUCCESS;
}

int chest_init(chest_t *q, uint32_t nof_re, uint32_t nof_symbols, uint32_t nof_ports) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q         != NULL &&
      nof_ports <=  MAX_PORTS)
  {
    bzero(q, sizeof(chest_t));

    q->nof_ports = nof_ports;
    q->nof_symbols = nof_symbols;
    q->nof_re = nof_re;
      
    INFO("Initializing channel estimator size %dx%d, nof_ports=%d\n",
        q->nof_symbols, q->nof_re, nof_ports);

    ret = LIBLTE_SUCCESS;
  }
  return ret;
}

void chest_free(chest_t *q) {
  int p, n;
  for (p=0;p<q->nof_ports;p++) {
    for (n=0;n<NSLOTS_X_FRAME;n++) {
      refsignal_free(&q->refsignal[p][n]);
    }
  }
#ifdef VOLK_INTERP
  for (p=0;p<MAX_PORTS;p++) {
    interp_free(&q->interp_freq[p]);
    interp_free(&q->interp_time[p]);    
  }
#endif
  bzero(q, sizeof(chest_t));
}

/* Fills l[2] with the symbols in the slot nslot that contain references.
 * returns the number of symbols with references (in the slot)
 */
int chest_ref_get_symbols(chest_t *q, uint32_t port_id, uint32_t nslot, uint32_t l[2]) {
  
  if (q         != NULL          && 
      port_id   <  MAX_PORTS     &&
      nslot     <  NSLOTS_X_FRAME)
  {
    memcpy(l, q->refsignal[port_id][nslot].symbols_ref, sizeof(uint32_t) * q->refsignal[port_id][nslot].nsymbols);
    return q->refsignal[port_id][nslot].nsymbols;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}





/*********************************************************************
 * 
 * Downlink Channel estimator
 * 
 *********************************************************************/ 
int chest_init_LTEDL(chest_t *q, lte_cell_t cell) {
  int ret; 
  ret = chest_init(q, cell.nof_prb * RE_X_RB, CP_NSYMB(cell.cp), cell.nof_ports);
  if (ret != LIBLTE_SUCCESS) {
    return ret;
  } else {
    return chest_ref_set_LTEDL(q, cell);    
  }
}

int chest_ref_set_LTEDL_slot_port(chest_t *q, uint32_t nslot, uint32_t port_id, lte_cell_t cell) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q         != NULL         && 
      port_id   < MAX_PORTS     &&
      nslot     < NSLOTS_X_FRAME)
  {
    ret = refsignal_init_LTEDL(&q->refsignal[port_id][nslot], port_id, nslot, cell);
    
#ifdef VOLK_INTERP
    if (ret == LIBLTE_SUCCESS) {
      if (nslot == 0) {
        ret = interp_init(&q->interp_freq[port_id], LINEAR, q->refsignal[port_id][nslot].nof_refs/2, RE_X_RB/2);
        if (ret == LIBLTE_SUCCESS) {
          ret = interp_init(&q->interp_time[port_id], LINEAR, 2, 
                    q->refsignal[port_id][nslot].symbols_ref[1] - q->refsignal[port_id][nslot].symbols_ref[0]);
        }
      }
    }
#endif
  }
  return ret;
}

int chest_ref_set_LTEDL_slot(chest_t *q, uint32_t nslot, lte_cell_t cell) {
  int p, ret;
  for (p=0;p<q->nof_ports;p++) {
    ret = chest_ref_set_LTEDL_slot_port(q, nslot, p, cell);
    if (ret != LIBLTE_SUCCESS) {
      return ret;
    }
  }
  return LIBLTE_SUCCESS;
}

int chest_ref_set_LTEDL(chest_t *q, lte_cell_t cell) {
  int n, ret;
  for (n=0;n<NSLOTS_X_FRAME;n++) {
    ret = chest_ref_set_LTEDL_slot(q, n, cell);
    if (ret != LIBLTE_SUCCESS) {
      return ret;
    }
  }
  return LIBLTE_SUCCESS;
}




/*********************************************************************
 * 
 * TODO: Uplink Channel estimator
 * 
 * 
 *********************************************************************/ 



















/** High-level API
*/
int chest_initialize(chest_hl* h) {

  lte_cell_t cell;

  if (!h->init.nof_symbols) {
    h->init.nof_symbols = CPNORM_NSYMB; // Normal CP
  }
  if (!h->init.nof_prb) {
    h->init.nof_prb = 6;
  }

  cell.id = h->init.cell_id; 
  cell.nof_ports = h->init.nof_ports;
  cell.nof_prb = h->init.nof_prb;
  cell.cp = h->init.nof_symbols == CPNORM_NSYMB ? CPNORM : CPEXT;
  
  if (chest_init_LTEDL(&h->obj, cell)) {
    fprintf(stderr, "Error initializing equalizer\n");
    return -1;
  }

  return 0;
}

/** This function must be called in an subframe basis (1ms) for LTE */
int chest_work(chest_hl* hl) {
  chest_t *q = &hl->obj;
  chest_ce_sf(q, hl->input, hl->output, hl->ctrl_in.sf_idx);
  return 0;
}

int chest_stop(chest_hl* hl) {
  chest_free(&hl->obj);
  return 0;
}


