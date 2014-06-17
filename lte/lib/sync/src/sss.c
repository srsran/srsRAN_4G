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
#include <stdlib.h>
#include <math.h>

#include "lte/sync/sss.h"
#include "lte/utils/dft.h"
#include "lte/utils/convolution.h"

void generate_sss_all_tables(struct sss_tables *tables, int N_id_2);
void convert_tables(struct fc_tables *fc_tables, struct sss_tables *in);
void generate_N_id_1_table(int table[30][30]);

int sss_synch_init(sss_synch_t *q) {
  bzero(q, sizeof(sss_synch_t));

  if (dft_plan(&q->dftp_input, SSS_DFT_LEN, COMPLEX_2_COMPLEX, FORWARD)) {
    return -1;
  }
  generate_N_id_1_table(q->N_id_1_table);
  q->dftp_input.options = DFT_MIRROR_POS | DFT_DC_OFFSET;
  return 0;
}

void sss_synch_free(sss_synch_t *q) {
  dft_plan_free(&q->dftp_input);
  bzero(q, sizeof(sss_synch_t));
}

/** Initializes the SSS sequences for the given N_id_2 */
int sss_synch_set_N_id_2(sss_synch_t *q, int N_id_2) {
  if (N_id_2 < 0 || N_id_2 > 2) {
    fprintf(stderr, "Invalid N_id_2 %d\n", N_id_2);
    return -1;
  }

  struct sss_tables sss_tables;
  generate_sss_all_tables(&sss_tables, N_id_2);
  convert_tables(&q->fc_tables, &sss_tables);

  return 0;
}

/** 36.211 10.3 section 6.11.2.2
 */
void sss_put_slot(float *sss, cf_t *slot, int nof_prb, lte_cp_t cp) {
  int i, k;

  k = (CP_NSYMB(cp) - 2) * nof_prb * RE_X_RB + nof_prb * RE_X_RB / 2 - 31;
  memset(&slot[k - 5], 0, 5 * sizeof(cf_t));
  for (i = 0; i < SSS_LEN; i++) {
    __real__ slot[k + i] = sss[i];
    __imag__ slot[k + i] = 0;
  }
  memset(&slot[k + SSS_LEN], 0, 5 * sizeof(cf_t));
}

/* In this function, input points to the beginning of the subframe. Saves result in subframe_idx and N_id_1
 * Return 1 if the sequence was found, 0 if the peak is not found, -1 if the subframe_sz or symbol_sz are
 * invalid or not configured.
 * Before calling this function, the correlation threshold and symbol size duration need to be set
 * using sss_synch_set_threshold() and sss_synch_set_symbol_sz().
 */
int sss_synch_frame(sss_synch_t *q, cf_t *input, int *subframe_idx, int *N_id_1) {
  int m0, m1;
  float m0_value, m1_value;

  if (q->subframe_sz <= 0 || q->symbol_sz <= 0) {
    return -1;
  }

  sss_synch_m0m1(q, &input[SSS_SYMBOL_ST(q->subframe_sz, q->symbol_sz)], &m0,
      &m0_value, &m1, &m1_value);

  if (m0_value > q->corr_peak_threshold
      && m1_value > q->corr_peak_threshold) {
    if (subframe_idx) {
      *subframe_idx = sss_synch_subframe(m0, m1);
    }
    if (N_id_1) {
      *N_id_1 = sss_synch_N_id_1(q, m0, m1);
    }
    return 1;
  } else {
    return 0;
  }
}

/** Used by sss_synch_frame() to compute the beginning of the SSS symbol
 * symbol_sz MUST INCLUDE THE CYCLIC PREFIX SIZE
 */
void sss_synch_set_symbol_sz(sss_synch_t *q, int symbol_sz) {
  q->symbol_sz = symbol_sz;
}

/** Used by sss_synch_frame() to compute the beginning of the SSS symbol */
void sss_synch_set_subframe_sz(sss_synch_t *q, int subframe_sz) {
  q->subframe_sz = subframe_sz;
}

/** Sets the SSS correlation peak detection threshold */
void sss_synch_set_threshold(sss_synch_t *q, float threshold) {
  q->corr_peak_threshold = threshold;
}

/** Returns the subframe index based on the m0 and m1 values */
int sss_synch_subframe(int m0, int m1) {
  if (m1 > m0) {
    return 0;
  } else {
    return 5;
  }
}

/** Returns the N_id_1 value based on the m0 and m1 values */
int sss_synch_N_id_1(sss_synch_t *q, int m0, int m1) {
  if (m0 < 0 || m0 > 29 || m1 < 0 || m1 > 29) {
    return -1;
  }
  if (m1 > m0) {
    return q->N_id_1_table[m0][m1 - 1];
  } else {
    return q->N_id_1_table[m1][m0 - 1];
  }
}

/** High-level API */

int sss_synch_initialize(sss_synch_hl* h) {

  if (sss_synch_init(&h->obj)) {
    return -1;
  }
  sss_synch_set_N_id_2(&h->obj, h->init.N_id_2);

  return 0;
}

int sss_synch_work(sss_synch_hl* hl) {

  if (hl->ctrl_in.correlation_threshold) {
    sss_synch_set_threshold(&hl->obj, hl->ctrl_in.correlation_threshold);
  }
  if (hl->ctrl_in.subframe_sz) {
    sss_synch_set_subframe_sz(&hl->obj, hl->ctrl_in.subframe_sz);
  }
  if (hl->ctrl_in.symbol_sz) {
    sss_synch_set_symbol_sz(&hl->obj, hl->ctrl_in.symbol_sz);
  }
  sss_synch_frame(&hl->obj, hl->input, &hl->ctrl_out.subframe_idx,
      &hl->ctrl_out.N_id_1);

  return 0;
}

int sss_synch_stop(sss_synch_hl* hl) {
  sss_synch_free(&hl->obj);
  return 0;
}

