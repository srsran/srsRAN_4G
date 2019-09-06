/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include <srslte/srslte.h>
#include <assert.h>

// Useful macros
#define NSAMPLES2NBYTES(N) (sizeof(cf_t) * (N))
#define M_1_3 0.33333333333333333333f /* 1 / 3 */
#define M_1_4 0.25f /* 1 / 4 */
#define M_4_7 0.571428571f /* 4 / 7*/

// Local state function prototypes
static srslte_wiener_dl_state_t* srslte_wiener_dl_state_malloc(srslte_wiener_dl_t *q);
static void srslte_wiener_dl_state_free(srslte_wiener_dl_state_t *q);
static void srslte_wiener_dl_state_reset(srslte_wiener_dl_t *q, srslte_wiener_dl_state_t *state);

// Local run function prototypes
static void srslte_wiener_dl_run_symbol_1_8(srslte_wiener_dl_t *q,
                                            srslte_wiener_dl_state_t *state,
                                            cf_t *pilots,
                                            float snr_lin);
static void srslte_wiener_dl_run_symbol_2_9(srslte_wiener_dl_t *q,
                                            srslte_wiener_dl_state_t *state);
static void srslte_wiener_dl_run_symbol_5_12(srslte_wiener_dl_t *q,
                                             srslte_wiener_dl_state_t *state,
                                             cf_t *pilots,
                                             uint32_t shift);

// Local state related functions
static srslte_wiener_dl_state_t* srslte_wiener_dl_state_malloc(srslte_wiener_dl_t *q) {
  // Allocate Channel state
  srslte_wiener_dl_state_t* state = calloc(sizeof(srslte_wiener_dl_state_t), 1);

  // Check allocation
  if (!state) {
    perror("malloc");
  } else {
    int ret = SRSLTE_SUCCESS;

    // Allocate state variables
    for (int i = 0; i < SRSLTE_WIENER_DL_HLS_FIFO_SIZE && !ret; i++) {
      state->hls_fifo_1[i] = srslte_vec_malloc(NSAMPLES2NBYTES(q->max_ref));
      if (!state->hls_fifo_1[i]) {
        perror("malloc");
        ret = SRSLTE_ERROR;
      }

      if (!ret) {
        state->hls_fifo_2[i] = srslte_vec_malloc(NSAMPLES2NBYTES(q->max_ref));
        if (!state->hls_fifo_2[i]) {
          perror("malloc");
          ret = SRSLTE_ERROR;
        }
      }
    }

    for(uint32_t i = 0; i < SRSLTE_WIENER_DL_TFIFO_SIZE && !ret; i++) {
      state->tfifo[i] = srslte_vec_malloc(NSAMPLES2NBYTES(q->max_re));
      if (!state->tfifo[i]) {
        perror("malloc");
        ret = SRSLTE_ERROR;
      }
    }

    if (!ret) {
      state->xfifo = srslte_vec_malloc(NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE * SRSLTE_WIENER_DL_XFIFO_SIZE));
      if (!state->xfifo) {
        perror("malloc");
        ret = SRSLTE_ERROR;
      }
    }

    if (!ret) {
      state->cV = srslte_vec_malloc(NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE));
      if (!state->cV) {
        perror("malloc");
        ret = SRSLTE_ERROR;
      }
    }

    if (!ret) {
      state->timefifo = srslte_vec_malloc(NSAMPLES2NBYTES(SRSLTE_WIENER_DL_TIMEFIFO_SIZE));
      if (!state->timefifo) {
        perror("malloc");
        ret = SRSLTE_ERROR;
      }
    }

    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_CXFIFO_SIZE && !ret; i++) {
      state->cxfifo[i] = srslte_vec_malloc(NSAMPLES2NBYTES(SRSLTE_WIENER_DL_TFIFO_SIZE));
      if (!state->cxfifo[i]) {
        perror("malloc");
        ret = SRSLTE_ERROR;
      }
    }

    // Initialise the rest
    state->deltan = 0.0f;
    state->nfifosamps = 0;
    state->invtpilotoff = 0;
    state->sumlen = 0;
    state->skip = 0;
    state->cnt = 0;

    if (ret) {
      // Free all allocated memory
      srslte_wiener_dl_state_free(state);

      // Return NULL if error
      state = NULL;
    }
  }

  return state;
}

static void srslte_wiener_dl_state_reset(srslte_wiener_dl_t *q, srslte_wiener_dl_state_t *state) {
  if (q && state) {
    // Initialise memory
    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_HLS_FIFO_SIZE; i++) {
      bzero(state->hls_fifo_1[i], NSAMPLES2NBYTES(q->nof_ref));
      bzero(state->hls_fifo_2[i], NSAMPLES2NBYTES(q->nof_ref));
    }
    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_TFIFO_SIZE; i++) {
      bzero(state->tfifo[i], NSAMPLES2NBYTES(q->nof_re));
    }
    bzero(state->xfifo, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE * SRSLTE_WIENER_DL_XFIFO_SIZE));
    bzero(state->cV, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE));
    bzero(state->timefifo, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_TIMEFIFO_SIZE));

    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_CXFIFO_SIZE; i++) {
      bzero(state->cxfifo[i], NSAMPLES2NBYTES(SRSLTE_WIENER_DL_TFIFO_SIZE));
    }


    // Initialise counters and variables
    state->deltan = 0.0f;
    state->nfifosamps = 0;
    state->invtpilotoff = 0;
    state->sumlen = 0;
    state->skip = 0;
    state->cnt = 0;
  }
}


static void srslte_wiener_dl_state_free(srslte_wiener_dl_state_t *q) {

  if (q) {
    for (int i = 0; i < SRSLTE_WIENER_DL_HLS_FIFO_SIZE; i++) {
      if (q->hls_fifo_1[i]) {
        free(q->hls_fifo_1[i]);
      }
      if (q->hls_fifo_2[i]) {
        free(q->hls_fifo_2[i]);
      }
    }
    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_TFIFO_SIZE; i++) {
      if (q->tfifo[i]) {
        free(q->tfifo[i]);
      }
    }
    if (q->xfifo) {
      free(q->xfifo);
    }
    for(uint32_t i = 0; i < SRSLTE_WIENER_DL_CXFIFO_SIZE; i++) {
      if (q->cxfifo[i]) {
        free(q->cxfifo[i]);
      }
    }
    if (q->cV) {
      free(q->cV);
    }
    if (q->timefifo) {
      free(q->timefifo);
    }

    // Free state
    free(q);
  }
}


int srslte_wiener_dl_init(srslte_wiener_dl_t *q, uint32_t max_prb, uint32_t max_tx_ports, uint32_t max_rx_ant) {
  int ret = SRSLTE_SUCCESS;

  if (q && max_prb > SRSLTE_MAX_PRB && max_tx_ports > SRSLTE_MAX_PORTS && max_rx_ant > SRSLTE_MAX_PORTS) {
    // Bzero structure
    bzero(q, sizeof(srslte_wiener_dl_t));

    // Set maximum parameters
    q->max_prb = max_prb;
    q->max_ref = max_prb * 2;
    q->max_re = max_prb * SRSLTE_NRE;
    q->max_tx_ports = max_tx_ports;
    q->max_rx_ant= max_rx_ant;

    // Allocate state
    for(uint32_t tx = 0; tx < q->max_tx_ports && !ret; tx++) {
      for(uint32_t rx = 0; rx < q->max_tx_ports && !ret; rx++) {
        srslte_wiener_dl_state_t* state = srslte_wiener_dl_state_malloc(q);
        if (!state) {
          perror("srslte_wiener_dl_state_malloc");
          ret = SRSLTE_ERROR;
        } else {
          q->state[tx][rx] = state;
        }
      }
    }

    // Allocate temporal buffers, maximum SF size
    if (!ret) {
      q->tmp = srslte_vec_malloc(NSAMPLES2NBYTES(SRSLTE_SF_LEN_MAX));
      if (!q->tmp) {
        perror("malloc");
        ret = SRSLTE_ERROR;
      }
    }

    if (!ret) {
      q->random = srslte_random_init(0xdead);
      if (!q->random) {
        perror("srslte_random_init");
        ret = SRSLTE_ERROR;
      }
    }
  }

  return ret;
}

int srslte_wiener_dl_set_cell(srslte_wiener_dl_t *q, const srslte_cell_t *cell) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q && cell) {
    // No invalid inputs
    ret = SRSLTE_SUCCESS;

    // Set new values
    q->nof_prb = cell->nof_prb;
    q->nof_ref = cell->nof_prb * 2;
    q->nof_re = cell->nof_prb * SRSLTE_NRE;
    q->nof_tx_ports = cell->nof_ports;

    // Reset states
    srslte_wiener_dl_reset(q);
  }

  return ret;
}

void srslte_wiener_dl_reset(srslte_wiener_dl_t *q) {
  if (q) {
    // Reset states
    for (uint32_t tx = 0; tx < SRSLTE_MAX_PORTS; tx++) {
      for (uint32_t rx = 0; rx < SRSLTE_MAX_PORTS; rx++) {
        if (q->state[tx][rx]) {
          srslte_wiener_dl_state_reset(q, q->state[tx][rx]);
        }
      }
    }

    // Reset wiener
    bzero(q->wm1, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_REF * SRSLTE_WIENER_DL_MIN_RE));
    bzero(q->wm2, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_REF * SRSLTE_WIENER_DL_MIN_RE));
  }
}

static void circshift_dim1(cf_t **matrix, uint32_t ndim1, int32_t k) {
  // Wrap k
  k = (k + ndim1) % ndim1;

  // Run k times
  while(k--) {
    // Save first pointer
    cf_t *tmp_ptr = matrix[0];

    // Shift pointers one position
    for (int i = 0; i < ndim1 - 1; i++) {
      matrix[i] = matrix[i + 1];
    }

    // Save last pointer
    matrix[ndim1 - 1] = tmp_ptr;
  }
}

static void circshift_dim2(cf_t **matrix, uint32_t ndim1, uint32_t ndim2, int32_t k) {
  // Wrap k
  k = (k + ndim1) % ndim1;

  for(uint32_t dim1 = 0; dim1 < ndim1; dim1++) {
    // Run k times
    for(int i = 0; i < k; i++) {
      // Save first value
      cf_t tmp = matrix[dim1][0];

      // Shift one position
      for (int dim2 = 0; i < dim2 - 1; dim2++) {
        matrix[dim1][dim2] = matrix[dim1][dim2 + 1];
      }

      // Save last value
      matrix[ndim1][ndim2 - 1] = tmp;
    }
  }
}

static void matrix_acc_dim1_cc(cf_t **matrix, cf_t *res, uint32_t ndim1, uint32_t ndim2) {
  // Accumulate each column
  for (uint32_t dim2 = 0; dim2 < ndim2; dim2++) {
    cf_t acc = 0.0f;
    for (uint32_t dim1 = 0; dim1 < ndim1; dim1++) {
      acc += matrix[dim1][dim2];
    }
    res[dim2] = acc;
  }
}

static uint32_t matrix_acc_dim2_cc(cf_t **matrix, cf_t *res, uint32_t ndim1, uint32_t ndim2) {
  // Accumulate each row
  for (uint32_t dim1 = 0; dim1 < ndim1; dim1++) {
    res[dim1] = srslte_vec_acc_cc(matrix[dim1], ndim2);
  }
}

static uint32_t vec_find_first_smaller_than_cf(cf_t *x, float y, uint32_t n, uint32_t pos) {
  uint32_t ret = n;

  for(uint32_t i = pos; i < n && ret == n; i++) {
    if (cabsf(x[i]) > y) {
      ret = i;
    }
  }

  return ret;
}

static void estimate_wiener(srslte_wiener_dl_t *q, const cf_t wm[SRSLTE_WIENER_DL_MIN_RE][SRSLTE_WIENER_DL_MIN_REF], cf_t *ref, cf_t *h) {
  uint32_t r_offset = 0; // Resource Element indexing offset
  uint32_t p_offset = 0; // Pilot indexing offset

  // Estimate lower band
  for(uint32_t i = 0; i < SRSLTE_WIENER_DL_MIN_RE; i++) {
    h[r_offset + i] = srslte_vec_dot_prod_ccc(&ref[p_offset], wm[i], SRSLTE_WIENER_DL_MIN_REF);
  }

  // Estimate Upper band (it might overlap in 6PRB cells with the lower band)
  r_offset = q->nof_re - SRSLTE_WIENER_DL_MIN_RE;
  p_offset = q->nof_ref - SRSLTE_WIENER_DL_MIN_REF;
  for(uint32_t i = 0; i < SRSLTE_WIENER_DL_MIN_RE; i++) {
    h[r_offset + i] = srslte_vec_dot_prod_ccc(&ref[p_offset], wm[i], SRSLTE_WIENER_DL_MIN_REF);
  }

  // Estimate center Resource elements
  if (q->nof_re > 2 * SRSLTE_WIENER_DL_MIN_RE) {
    for (uint32_t prb = SRSLTE_WIENER_DL_MIN_PRB / 2; prb < q->nof_prb - SRSLTE_WIENER_DL_MIN_REF/2; prb += SRSLTE_WIENER_DL_MIN_PRB / 2) {
      uint32_t ref_idx = prb * 2 - SRSLTE_WIENER_DL_MIN_REF / 2;
      uint32_t re_idx = prb * SRSLTE_NRE;
      for (uint32_t i = SRSLTE_WIENER_DL_MIN_RE / 4; i < (3 * SRSLTE_WIENER_DL_MIN_RE) / 4; i++) {
        h[re_idx + i] = srslte_vec_dot_prod_ccc(&ref[ref_idx], wm[i], SRSLTE_WIENER_DL_MIN_REF);
      }
    }
  }
}

static void srslte_wiener_dl_run_symbol_1_8(srslte_wiener_dl_t *q,
                                            srslte_wiener_dl_state_t *state,
                                            cf_t *pilots,
                                            float snr_lin) {

  // there are pilot symbols (even) in this OFDM period (first symbol of the slot)
  circshift_dim1(state->hls_fifo_2, SRSLTE_WIENER_DL_HLS_FIFO_SIZE, 1);   // shift matrix rows down one position
  memcpy(state->hls_fifo_2[0], pilots, NSAMPLES2NBYTES(q->nof_ref));

  // Online training for pilot filtering
  circshift_dim2(&state->timefifo, 1, SRSLTE_WIENER_DL_TIMEFIFO_SIZE, 1); // shift columns right one position
  state->timefifo[0] = conjf(pilots[q->nof_ref / 2]); // train with center of subband frequency

  circshift_dim1(state->cxfifo, SRSLTE_WIENER_DL_CXFIFO_SIZE, 1); // shift rows down one position
  srslte_vec_sc_prod_ccc(state->timefifo, pilots[q->nof_ref / 2], state->cxfifo[0], SRSLTE_WIENER_DL_TIMEFIFO_SIZE);

  // Calculate auto-correlation and normalize
  matrix_acc_dim1_cc(state->cxfifo, q->tmp, SRSLTE_WIENER_DL_CXFIFO_SIZE, SRSLTE_WIENER_DL_TIMEFIFO_SIZE);
  srslte_vec_sc_prod_cfc(q->tmp, 1.0f / SRSLTE_WIENER_DL_CXFIFO_SIZE, q->tmp, SRSLTE_WIENER_DL_TIMEFIFO_SIZE);

  // Find index of half amplitude
  uint32_t halfcx = vec_find_first_smaller_than_cf(q->tmp, cabsf(q->tmp[1]) * 0.5f, SRSLTE_WIENER_DL_TFIFO_SIZE, 2);

  // Update internal states
  state->sumlen = SRSLTE_MAX(1, floorf(halfcx / 8.0f * SRSLTE_MIN(2.0f, 1.0f + 1.0f / snr_lin)));
  state->skip = SRSLTE_MAX(1, floorf(halfcx / 4.0f * SRSLTE_MIN(1, snr_lin / 16.0f)));
  state->deltan = 0;
  state->invtpilotoff = M_1_3;
}

static void srslte_wiener_dl_run_symbol_2_9(srslte_wiener_dl_t *q, srslte_wiener_dl_state_t *state) {

  // here we only shift and feed TD interpolation fifo
  circshift_dim1(state->tfifo, SRSLTE_WIENER_DL_TFIFO_SIZE, 1); // shift matrix columns right by one position

  // Average Reference Signals
  matrix_acc_dim1_cc(state->hls_fifo_2, q->tmp, SRSLTE_WIENER_DL_HLS_FIFO_SIZE, q->nof_ref); // Sum values
  srslte_vec_sc_prod_cfc(q->tmp, 1.0f / state->sumlen, q->tmp, q->nof_ref); // Sacle sum

  // Estimate channel based on the wiener matrix 2
  estimate_wiener(q, q->wm2, q->tmp, state->tfifo[0]);

  // Update internal states
  state->deltan = 0.0f;
  state->invtpilotoff = M_1_3;
}

static void srslte_wiener_dl_run_symbol_5_12(srslte_wiener_dl_t *q,
                                             srslte_wiener_dl_state_t *state,
                                             cf_t *pilots,
                                             uint32_t shift) {
  // there are pilot symbols (odd) in this OFDM period (fifth symbol of the slot)
  circshift_dim1(state->hls_fifo_1, SRSLTE_WIENER_DL_HLS_FIFO_SIZE, 1); // shift matrix rows down one position
  memcpy(state->hls_fifo_1[0], pilots, NSAMPLES2NBYTES(q->nof_ref));

  circshift_dim1(state->tfifo, SRSLTE_WIENER_DL_TFIFO_SIZE, 1); // shift matrix columns right by one position

  // Average Reference Signals
  matrix_acc_dim1_cc(state->hls_fifo_1, q->tmp, SRSLTE_WIENER_DL_HLS_FIFO_SIZE, q->nof_ref); // Sum values
  srslte_vec_sc_prod_cfc(q->tmp, 1.0f / state->sumlen, q->tmp, q->nof_ref); // Sacle sum

  // Estimate channel based on the wiener matrix 1
  estimate_wiener(q, q->wm1, q->tmp, state->tfifo[0]);

  // Update internal states
  state->deltan = 0.0f;
  state->invtpilotoff = M_1_4;
  state->cnt++;

  // Online training of Wiener matrices (random sub-bands)
  if (state->cnt == state->skip) {
    state->cnt = 0; // Reset counter
    uint32_t pos1, pos2, nsbb, pstart;

    pos1 = (shift < 3) ? 0 : 3;
    pos2 = (pos1 + 3) % 6;

    // Choose randomly a pair of PRB and calculate the start reference signal
    nsbb = srslte_random_uniform_int_dist(q->random, 0, q->nof_prb / 2);
    if (nsbb == 0) {
      pstart = 0;
    } else if (nsbb >= (q->nof_prb / 2) - 1) {
      pstart = q->nof_ref - SRSLTE_WIENER_DL_MIN_REF;
    } else {
      pstart = (SRSLTE_WIENER_DL_MIN_REF / 2) * nsbb - 1;
    }

    bzero(q->hlsv, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE));
    bzero(q->hlsv_sum, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE));
    for(uint32_t i = pos2, k = pstart; i < SRSLTE_WIENER_DL_MIN_RE; i += 6, k++) {
      q->hlsv[i] = conjf(state->hls_fifo_2[1][k]  +  (state->hls_fifo_2[0][k] - state->hls_fifo_2[1][k]) * M_4_7);
    }
    for(uint32_t i = pos1, k = pstart; i < SRSLTE_WIENER_DL_MIN_RE; i += 6, k++) {
      q->hlsv[i] = conjf(state->hls_fifo_1[1][k]);
    }

    for(uint32_t i = 0; i < SRSLTE_WIENER_DL_MIN_REF * 2; i++) {
      srslte_vec_sc_prod_ccc(q->hlsv, conjf(q->hlsv[0]), q->tmp, SRSLTE_WIENER_DL_MIN_RE);
      srslte_vec_sum_ccc(q->tmp, q->hlsv_sum, q->hlsv_sum, SRSLTE_WIENER_DL_MIN_RE);

    }
  }
}

int srslte_wiener_dl_run(srslte_wiener_dl_t *q, uint32_t tx, uint32_t rx, uint32_t m, uint32_t shift, cf_t *pilots, cf_t *estimated, float snr_lin) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q) {
    // m is based on 0, increase one;
    m++;

    // Process symbol
    switch (m) {
      case 1:
      case 8:
        srslte_wiener_dl_run_symbol_1_8(q, q->state[tx][rx], pilots, snr_lin);
        break;
      case 2:
      case 9:
        srslte_wiener_dl_run_symbol_2_9(q, q->state[tx][rx]);
        break;
      case 5:
      case 12:
        srslte_wiener_dl_run_symbol_5_12(q, q->state[tx][rx], pilots, snr_lin);
        break;
      default:
        perror("unhandled switch-case");
    }

    // Estimate

    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

void srslte_wiener_dl_free(srslte_wiener_dl_t *q) {
  if (q) {
    for(uint32_t tx = 0; tx < SRSLTE_MAX_PORTS; tx++) {
      for(uint32_t rx = 0; rx < SRSLTE_MAX_PORTS; rx++) {
        if (q->state[tx][rx]) {
          srslte_wiener_dl_state_free(q->state[tx][rx]);
          q->state[tx][rx] = NULL;
        }
      }
    }

    if (q->tmp) {
      free(q->tmp);
    }

    if (q->random) {
      srslte_random_free(q->random);
    }

  }
}