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
#include <assert.h>
#include <math.h>

#include "lte/ch_estimation/chest.h"
#include "lte/resampling/interp.h"
#include "lte/utils/vector.h"
#include "lte/utils/debug.h"

#define SLOT_SZ(q) (q->nof_symbols * q->symbol_sz)
#define SF_SZ(q) (2 * SLOT_SZ(q))

void chest_fprint(chest_t *q, FILE *stream, int nslot, int port_id) {
  chest_ref_fprint(q, stream, nslot, port_id);
  chest_recvsig_fprint(q, stream, nslot, port_id);
  chest_ce_fprint(q, stream, nslot, port_id);
}

void chest_ref_fprint(chest_t *q, FILE *stream, int nslot, int port_id) {
  int i;
  fprintf(stream, "refs%d=[",port_id);
  for (i=0;i<q->refsignal[port_id][nslot].nof_refs;i++) {
    fprintf(stream, "%3.3f%+3.3fi, ", __real__ q->refsignal[port_id][nslot].refs[i].simbol,
        __imag__ q->refsignal[port_id][nslot].refs[i].simbol);
  }
  fprintf(stream, "];\n");
}

void chest_recvsig_fprint(chest_t *q, FILE *stream, int nslot, int port_id) {
  int i;
  fprintf(stream, "recvsig%d=[",port_id);
  for (i=0;i<q->refsignal[port_id][nslot].nof_refs;i++) {
    fprintf(stream, "%3.3f%+3.3fi, ", __real__ q->refsignal[port_id][nslot].refs[i].recv_simbol,
        __imag__ q->refsignal[port_id][nslot].refs[i].recv_simbol);
  }
  fprintf(stream, "];\n");
}

void chest_ce_fprint(chest_t *q, FILE *stream, int nslot, int port_id) {
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

void chest_ce_ref(chest_t *q, cf_t *input, int nslot, int port_id, int nref) {
  int fidx, tidx;
  cf_t known_ref, channel_ref;

  fidx = q->refsignal[port_id][nslot].refs[nref].freq_idx; // reference frequency index
  tidx = q->refsignal[port_id][nslot].refs[nref].time_idx; // reference time index
  known_ref = q->refsignal[port_id][nslot].refs[nref].simbol;
  channel_ref = input[SAMPLE_IDX(q->nof_prb, tidx, fidx)];
  q->refsignal[port_id][nslot].refs[nref].recv_simbol = channel_ref;

  DEBUG("Reference %2d pos (%2d,%2d)=%3d %.2f dB %.2f/%.2f=%.2f\n", nref, tidx, fidx, SAMPLE_IDX(q->nof_prb, tidx, fidx),
      10*log10f(cabsf(channel_ref/known_ref)),
      cargf(channel_ref)/M_PI,cargf(known_ref)/M_PI,cargf(channel_ref/known_ref)/M_PI);

  /* FIXME: compare with threshold */
  if (channel_ref != 0) {
    q->refsignal[port_id][nslot].ch_est[nref] = channel_ref/known_ref;
  } else {
    q->refsignal[port_id][nslot].ch_est[nref] = 0;
  }
}

/* Computes channel estimates for each reference in a slot and port.
 * Saves the nof_prb * 12 * nof_symbols channel estimates in the array ce
 */
void chest_ce_slot_port(chest_t *q, cf_t *input, cf_t *ce, int nslot, int port_id) {
  int i, j;
  cf_t x[2], y[MAX_NSYMB];

  assert(nslot >= 0 && nslot < NSLOTS_X_FRAME);
  assert(port_id >= 0 && port_id < q->nof_ports);
  assert(q->refsignal[port_id][nslot].nsymbols <= 2);

  refsignal_t *r = &q->refsignal[port_id][nslot];

  INFO("Estimating channel slot=%d port=%d using %d reference signals\n",
      nslot, port_id, r->nof_refs);

  for (i=0;i<r->nof_refs;i++) {
    chest_ce_ref(q, input, nslot, port_id, i);
  }

  /* interpolate the symbols with references
   * in the freq domain */
  for (i=0;i<r->nsymbols;i++) {
    interp_linear_offset(&r->ch_est[i * r->nof_refs/2],
        &ce[r->symbols_ref[i] * q->nof_prb * RE_X_RB], RE_X_RB/2,
        r->nof_refs/2, r->voffset, RE_X_RB/2-r->voffset);

  }
  /* now interpolate in the time domain */
  for (i=0;i<q->nof_prb * RE_X_RB; i++) {
    if (r->nsymbols > 1) {
      for (j=0;j<r->nsymbols;j++) {
        x[j] = ce[r->symbols_ref[j] * q->nof_prb * RE_X_RB + i];
      }
      interp_linear_offset(x, y, r->symbols_ref[1]-r->symbols_ref[0],
          2, r->symbols_ref[0], 3);
    } else {
      for (j=0;j<MAX_NSYMB;j++) {
        y[j] = ce[r->symbols_ref[0] * q->nof_prb * RE_X_RB + i];
      }
    }
    for (j=0;j<q->nof_symbols;j++) {
      ce[j * q->nof_prb * RE_X_RB + i] = y[j];
    }
  }
}

/* Computes channel estimates for each reference in a slot.
 * Saves the result for the p-th port to the pointer ce[p]
 */
void chest_ce_slot(chest_t *q, cf_t *input, cf_t **ce, int nslot) {
  int p;
  for (p=0;p<q->nof_ports;p++) {
    chest_ce_slot_port(q, input, ce[p], nslot, p);
  }
}

int chest_init(chest_t *q, chest_interp_t interp, lte_cp_t cp, int nof_prb, int nof_ports) {

  if (nof_ports > MAX_PORTS) {
    fprintf(stderr, "Error: Maximum ports %d\n", MAX_PORTS);
    return -1;
  }
  bzero(q, sizeof(chest_t));

  q->nof_ports = nof_ports;
  q->nof_symbols = CP_NSYMB(cp);
  q->cp = cp;
  q->nof_prb = nof_prb;

  switch(interp) {
  case LINEAR:
    q->interp = interp_linear_offset;
  }

  INFO("Initializing channel estimator size %dx%d, nof_ports=%d\n",
      q->nof_symbols, nof_prb, nof_ports);

  return 0;
}

int chest_ref_LTEDL_slot_port(chest_t *q, int port, int nslot, int cell_id) {
  if (port < 0 || port > q->nof_ports) {
    return -1;
  }
  if (nslot < 0 || nslot > NSLOTS_X_FRAME) {
    return -1;
  }

  if (refsignal_init_LTEDL(&q->refsignal[port][nslot], port, nslot, cell_id, q->cp, q->nof_prb)) {
    fprintf(stderr, "Error initiating CRS port=%d, slot=%d\n", port, nslot);
    return -1;
  }

  return 0;
}

int chest_ref_LTEDL_slot(chest_t *q, int nslot, int cell_id) {
  int p;
  for (p=0;p<q->nof_ports;p++) {
    if (chest_ref_LTEDL_slot_port(q, p, nslot, cell_id)) {
      return -1;
    }
  }
  return 0;
}

int chest_ref_LTEDL(chest_t *q, int cell_id) {
  int n;
  for (n=0;n<NSLOTS_X_FRAME;n++) {
    if (chest_ref_LTEDL_slot(q, n, cell_id)) {
      return -1;
    }
  }
  return 0;
}

void chest_free(chest_t *q) {
  int p, n;
  for (p=0;p<q->nof_ports;p++) {
    for (n=0;n<NSLOTS_X_FRAME;n++) {
      refsignal_free(&q->refsignal[p][n]);
    }
  }
  bzero(q, sizeof(chest_t));
}

/* Fills l[2] with the symbols in the slot nslot that contain references.
 * returns the number of symbols with references (in the slot)
 */
int chest_ref_symbols(chest_t *q, int port_id, int nslot, int l[2]) {
  if (nslot < 0 || nslot > NSLOTS_X_FRAME) {
    return -1;
  }
  memcpy(l, q->refsignal[port_id][nslot].symbols_ref, sizeof(int) * q->refsignal[port_id][nslot].nsymbols);
  return q->refsignal[port_id][nslot].nsymbols;
}


/** High-level API
*/
int chest_initialize(chest_hl* h) {

  if (!h->init.nof_symbols) {
    h->init.nof_symbols = CPNORM_NSYMB; // Normal CP
  }
  if (!h->init.nof_prb) {
    h->init.nof_prb = 6;
  }

  if (chest_init(&h->obj, LINEAR, (h->init.nof_symbols==CPNORM_NSYMB)?CPNORM:CPEXT,
      h->init.nof_prb, h->init.nof_ports)) {
    fprintf(stderr, "Error initializing equalizer\n");
    return -1;
  }
  if (h->init.cell_id != -1) {
    if (chest_ref_LTEDL(&h->obj, h->init.cell_id)) {
      fprintf(stderr, "Error initializing reference signal\n");
      return -1;
    }
  }

  return 0;
}

/** This function must be called in an slot basis (0.5ms) for LTE */
int chest_work(chest_hl* hl) {
  int i;
  chest_t *q = &hl->obj;

  if (hl->init.cell_id != hl->ctrl_in.cell_id) {
    if (chest_ref_LTEDL(q, hl->init.cell_id)) {
      fprintf(stderr, "Error initializing reference signal\n");
      return -1;
    }
  }

  for (i=0;i<hl->init.nof_ports;i++) {
    chest_ce_slot_port(q, hl->input, hl->output[i], 1, 0);
    hl->out_len[i] = hl->in_len;
  }
  return 0;
}

int chest_stop(chest_hl* hl) {
  chest_free(&hl->obj);
  return 0;
}


