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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "liblte/phy/scrambling/scrambling.h"

void scrambling_f(sequence_t *s, float *data) {
  scrambling_f_offset(s, data, 0, s->len);
}

void scrambling_f_offset(sequence_t *s, float *data, int offset, int len) {
  int i;
  assert (len + offset <= s->len);

  for (i = 0; i < len; i++) {
    data[i] = data[i] * (1 - 2 * s->c[i + offset]);
  }
}

void scrambling_c(sequence_t *s, cf_t *data) {
  scrambling_c_offset(s, data, 0, s->len);
}

void scrambling_c_offset(sequence_t *s, cf_t *data, int offset, int len) {
  int i;
  assert (len + offset <= s->len);

  for (i = 0; i < len; i++) {
    data[i] = data[i] * (1 - 2 * s->c[i + offset]);
  }
}

void scrambling_b(sequence_t *s, char *data) {
  int i;

  for (i = 0; i < s->len; i++) {
    data[i] = (data[i] + s->c[i]) % 2;
  }
}

void scrambling_b_offset(sequence_t *s, char *data, int offset, int len) {
  int i;
  assert (len + offset <= s->len);
  for (i = 0; i < len; i++) {
    data[i] = (data[i] + s->c[i + offset]) % 2;
  }
}

/** High-level API */

int compute_sequences(scrambling_hl* h) {

  switch (h->init.channel) {
  case SCRAMBLING_PBCH:
    return sequence_pbch(&h->obj.seq[0],
        h->init.nof_symbols == CPNORM_NSYMB ? CPNORM : CPEXT, h->init.cell_id);
  case SCRAMBLING_PDSCH:
    for (int ns = 0; ns < NSUBFRAMES_X_FRAME; ns++) {
      sequence_pdsch(&h->obj.seq[ns], h->init.nrnti, 0, 2 * ns, h->init.cell_id,
          LTE_NSOFT_BITS);
    }
    return 0;
  case SCRAMBLING_PCFICH:
    for (int ns = 0; ns < NSUBFRAMES_X_FRAME; ns++) {
      sequence_pcfich(&h->obj.seq[ns], 2 * ns, h->init.cell_id);
    }
    return 0;
  case SCRAMBLING_PDCCH:
    for (int ns = 0; ns < NSUBFRAMES_X_FRAME; ns++) {
      sequence_pdcch(&h->obj.seq[ns], 2 * ns, h->init.cell_id, LTE_NSOFT_BITS);
    }
    return 0;
  case SCRAMBLING_PMCH:
  case SCRAMBLING_PUCCH:
    fprintf(stderr, "Not implemented\n");
    return -1;
  default:
    fprintf(stderr, "Invalid channel %d\n", h->init.channel);
    return -1;
  }
}

int scrambling_initialize(scrambling_hl* h) {

  bzero(&h->obj, sizeof(scrambling_t));

  return compute_sequences(h);
}

/** This function can be called in a subframe (1ms) basis for LTE */
int scrambling_work(scrambling_hl* hl) {
  int sf;
  if (hl->init.channel == SCRAMBLING_PBCH) {
    sf = 0;
  } else {
    sf = hl->ctrl_in.subframe;
  }
  sequence_t *seq = &hl->obj.seq[sf];

  if (hl->init.hard) {
    memcpy(hl->output, hl->input, sizeof(char) * hl->in_len);
    scrambling_b(seq, hl->output);
  } else {
    memcpy(hl->output, hl->input, sizeof(float) * hl->in_len);
    scrambling_f(seq, hl->output);
  }
  hl->out_len = hl->in_len;
  return 0;
}

int scrambling_stop(scrambling_hl* hl) {
  int i;
  for (i = 0; i < NSUBFRAMES_X_FRAME; i++) {
    sequence_free(&hl->obj.seq[i]);
  }
  return 0;
}

