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

#include <assert.h>
#include <srslte/phy/ch_estimation/wiener_dl.h>
#include <srslte/phy/utils/mat.h>
#include <srslte/srslte.h>

// Useful macros
#define NSAMPLES2NBYTES(N) (sizeof(cf_t) * (N))
#define M_1_3 0.33333333333333333333f /* 1 / 3 */
#define M_1_4 0.25f                   /* 1 / 4 */
#define M_4_7 0.571428571f            /* 4 / 7 */
#define M_4_3 1.33333333333333333333f /* 4 / 3 */
#define M_5_3 1.66666666666666666666f /* 5 / 3 */
#define SRSLTE_WIENER_LOCAL

// Constants
const float hlsv_sum_norm[SRSLTE_WIENER_DL_MIN_RE] = {
    16.0f, 15.66f, 15.33f, 15.0f, 14.66f, 14.33f, 14.0f, 13.66f, 13.33f, 13.0f, 12.66f, 12.33f,
    12.0f, 11.66f, 11.33f, 11.0f, 10.66f, 10.33f, 10.0f, 9.66f,  9.33f,  9.0f,  8.66f,  8.33f,
    8.0f,  7.66f,  7.33f,  7.0f,  6.66f,  6.33f,  6.0f,  5.66f,  5.33f,  5.0f,  4.66f,  4.33f,
    4.0f,  3.66f,  3.33f,  3.0f,  2.66f,  2.33f,  2.0f,  1.66f,  1.33f,  1.0f,  0.66f,  0.33f};

// Local state function prototypes
SRSLTE_WIENER_LOCAL srslte_wiener_dl_state_t* srslte_wiener_dl_state_malloc(srslte_wiener_dl_t* q);
SRSLTE_WIENER_LOCAL void                      srslte_wiener_dl_state_free(srslte_wiener_dl_state_t* q);
SRSLTE_WIENER_LOCAL void srslte_wiener_dl_state_reset(srslte_wiener_dl_t* q, srslte_wiener_dl_state_t* state);

// Local run function prototypes
SRSLTE_WIENER_LOCAL void
                         srslte_wiener_dl_run_symbol_1_8(srslte_wiener_dl_t* q, srslte_wiener_dl_state_t* state, cf_t* pilots, float snr_lin);
SRSLTE_WIENER_LOCAL void srslte_wiener_dl_run_symbol_2_9(srslte_wiener_dl_t* q, srslte_wiener_dl_state_t* state);
SRSLTE_WIENER_LOCAL void srslte_wiener_dl_run_symbol_5_12(srslte_wiener_dl_t*       q,
                                                          srslte_wiener_dl_state_t* state,
                                                          cf_t*                     pilots,
                                                          uint32_t                  tx,
                                                          uint32_t                  rx,
                                                          uint32_t                  shift,
                                                          float                     snr_lin);

// Local state related functions
SRSLTE_WIENER_LOCAL srslte_wiener_dl_state_t* srslte_wiener_dl_state_malloc(srslte_wiener_dl_t* q)
{
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

    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_TFIFO_SIZE && !ret; i++) {
      state->tfifo[i] = srslte_vec_malloc(NSAMPLES2NBYTES(q->max_re));
      if (!state->tfifo[i]) {
        perror("malloc");
        ret = SRSLTE_ERROR;
      }
    }

    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_XFIFO_SIZE && !ret; i++) {
      state->xfifo[i] = srslte_vec_malloc(NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE));
      if (!state->xfifo[i]) {
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
      state->cxfifo[i] = srslte_vec_malloc(NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE));
      if (!state->cxfifo[i]) {
        perror("malloc");
        ret = SRSLTE_ERROR;
      }
    }

    // Initialise the rest
    state->deltan       = 0.0f;
    state->nfifosamps   = 0;
    state->invtpilotoff = 0;
    state->sumlen       = 0;
    state->skip         = 0;
    state->cnt          = 0;

    if (ret) {
      // Free all allocated memory
      srslte_wiener_dl_state_free(state);

      // Return NULL if error
      state = NULL;
    }
  }

  return state;
}

SRSLTE_WIENER_LOCAL void srslte_wiener_dl_state_reset(srslte_wiener_dl_t* q, srslte_wiener_dl_state_t* state)
{
  if (q && state) {
    // Initialise memory
    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_HLS_FIFO_SIZE; i++) {
      bzero(state->hls_fifo_1[i], NSAMPLES2NBYTES(q->nof_ref));
      bzero(state->hls_fifo_2[i], NSAMPLES2NBYTES(q->nof_ref));
    }
    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_TFIFO_SIZE; i++) {
      bzero(state->tfifo[i], NSAMPLES2NBYTES(q->nof_re));
    }
    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_XFIFO_SIZE; i++) {
      bzero(state->xfifo[i], NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE));
    }
    bzero(state->cV, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE));
    bzero(state->timefifo, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_TIMEFIFO_SIZE));

    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_CXFIFO_SIZE; i++) {
      bzero(state->cxfifo[i], NSAMPLES2NBYTES(SRSLTE_WIENER_DL_TFIFO_SIZE));
    }

    // Initialise counters and variables
    state->deltan       = 0.0f;
    state->nfifosamps   = 0;
    state->invtpilotoff = 0;
    state->sumlen       = 0;
    state->skip         = 0;
    state->cnt          = 0;
  }
}

SRSLTE_WIENER_LOCAL void srslte_wiener_dl_state_free(srslte_wiener_dl_state_t* q)
{

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
    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_XFIFO_SIZE; i++) {
      if (q->xfifo[i]) {
        free(q->xfifo[i]);
      }
    }
    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_CXFIFO_SIZE; i++) {
      if (q->cxfifo[i]) {
        free(q->cxfifo[i]);
      }
    }
    if (q->timefifo) {
      free(q->timefifo);
    }

    // Free state
    free(q);
  }
}

int srslte_wiener_dl_init(srslte_wiener_dl_t* q, uint32_t max_prb, uint32_t max_tx_ports, uint32_t max_rx_ant)
{
  int ret = SRSLTE_SUCCESS;

  if (q && max_prb <= SRSLTE_MAX_PRB && max_tx_ports <= SRSLTE_MAX_PORTS && max_rx_ant <= SRSLTE_MAX_PORTS) {
    // Bzero structure
    bzero(q, sizeof(srslte_wiener_dl_t));

    // Set maximum parameters
    q->max_prb      = max_prb;
    q->max_ref      = max_prb * 2;
    q->max_re       = max_prb * SRSLTE_NRE;
    q->max_tx_ports = max_tx_ports;
    q->max_rx_ant   = max_rx_ant;

    // Allocate state
    for (uint32_t tx = 0; tx < q->max_tx_ports && !ret; tx++) {
      for (uint32_t rx = 0; rx < q->max_tx_ports && !ret; rx++) {
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

    // Create filter FFT/iFFT plans
    if (!ret) {
      ret = srslte_dft_plan_c(&q->fft, SRSLTE_WIENER_DL_MIN_RE, SRSLTE_DFT_FORWARD);
    }

    if (!ret) {
      ret = srslte_dft_plan_c(&q->ifft, SRSLTE_WIENER_DL_MIN_RE, SRSLTE_DFT_BACKWARD);
    }

    // Initialise interpolation filter
    if (!ret) {
      bzero(q->filter, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE));
      q->filter[0]  = 3.0f / SRSLTE_WIENER_DL_MIN_RE;
      q->filter[1]  = 2.0f / SRSLTE_WIENER_DL_MIN_RE;
      q->filter[2]  = 1.0f / SRSLTE_WIENER_DL_MIN_RE;
      q->filter[46] = 1.0f / SRSLTE_WIENER_DL_MIN_RE;
      q->filter[47] = 2.0f / SRSLTE_WIENER_DL_MIN_RE;
      srslte_dft_run_c(&q->fft, q->filter, q->filter);
    }

    // Initialise matrix inverter
    if (!ret) {
      q->matrix_inverter = calloc(sizeof(srslte_matrix_NxN_inv_t), 1);
      if (q->matrix_inverter) {
        ret = srslte_matrix_NxN_inv_init(q->matrix_inverter, SRSLTE_WIENER_DL_MIN_REF);
      } else {
        perror("calloc");
        ret = SRSLTE_ERROR;
      }
    }
  }

  return ret;
}

int srslte_wiener_dl_set_cell(srslte_wiener_dl_t* q, srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q) {
    // No invalid inputs
    ret = SRSLTE_SUCCESS;

    // Set new values
    q->nof_prb      = cell.nof_prb;
    q->nof_ref      = cell.nof_prb * 2;
    q->nof_re       = cell.nof_prb * SRSLTE_NRE;
    q->nof_tx_ports = cell.nof_ports;

    // Reset states
    srslte_wiener_dl_reset(q);
  }

  return ret;
}

void srslte_wiener_dl_reset(srslte_wiener_dl_t* q)
{
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

SRSLTE_WIENER_LOCAL void circshift_dim1(cf_t** matrix, uint32_t ndim1, int32_t k)
{
  // Check valid inputs
  if (matrix != NULL && ndim1 != 0 && k != 0) {
    // Wrap k
    k = (k + ndim1) % ndim1;

    // Run k times
    while (k--) {
      // Save first pointer
      cf_t* tmp_ptr = matrix[0];

      // Shift pointers one position
      for (int i = 0; i < ndim1 - 1; i++) {
        matrix[i] = matrix[i + 1];
      }

      // Save last pointer
      matrix[ndim1 - 1] = tmp_ptr;
    }
  } else {
    ERROR("unattended circshift_dim1!");
  }
}

SRSLTE_WIENER_LOCAL void circshift_dim2(cf_t** matrix, uint32_t ndim1, uint32_t ndim2, int32_t k)
{
  // Wrap k
  k = (k + ndim1) % ndim1;

  for (uint32_t dim1 = 0; dim1 < ndim1; dim1++) {
    // Run k times
    for (int i = 0; i < k; i++) {
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

SRSLTE_WIENER_LOCAL void matrix_acc_dim1_cc(cf_t** matrix, cf_t* res, uint32_t ndim1, uint32_t ndim2)
{
  int dim2 = 0;

#if SRSLTE_SIMD_CF_SIZE
  for (; dim2 < ndim2 - SRSLTE_SIMD_CF_SIZE / 2 + 1; dim2 += SRSLTE_SIMD_CF_SIZE / 2) {
    simd_f_t acc = srslte_simd_f_zero();
    for (uint32_t dim1 = 0; dim1 < ndim1; dim1++) {
      simd_f_t reg = srslte_simd_f_loadu((float*)&matrix[dim1][dim2]);
      acc          = srslte_simd_f_add(acc, reg);
    }
    srslte_simd_f_storeu((float*)&res[dim2], acc);
  }
#endif

  // Accumulate each column
  for (; dim2 < ndim2; dim2++) {
    cf_t acc = 0.0f;
    for (uint32_t dim1 = 0; dim1 < ndim1; dim1++) {
      acc += matrix[dim1][dim2];
    }
    res[dim2] = acc;
  }
}

/*static void matrix_acc_dim2_cc(cf_t** matrix, cf_t* res, uint32_t ndim1, uint32_t ndim2)
{
  // Accumulate each row
  for (uint32_t dim1 = 0; dim1 < ndim1; dim1++) {
    res[dim1] = srslte_vec_acc_cc(matrix[dim1], ndim2);
  }
}*/

SRSLTE_WIENER_LOCAL uint32_t vec_find_first_smaller_than_cf(cf_t* x, float y, uint32_t n, uint32_t pos)
{
  uint32_t ret = n;

  for (uint32_t i = pos; i < n && ret == n; i++) {
    if (cabsf(x[i]) > y) {
      ret = i;
    }
  }

  return ret;
}

SRSLTE_WIENER_LOCAL cf_t _srslte_vec_dot_prod_ccc_simd(const cf_t* x, const cf_t* y, const int len)
{
  int  i      = 0;
  cf_t result = 0;

#if SRSLTE_SIMD_CF_SIZE
  if (len >= SRSLTE_SIMD_CF_SIZE) {
    simd_cf_t avx_result = srslte_simd_cf_zero();
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t xVal = srslte_simd_cfi_load(&x[i]);
      simd_cf_t yVal = srslte_simd_cfi_load(&y[i]);

      avx_result = srslte_simd_cf_add(srslte_simd_cf_prod(xVal, yVal), avx_result);
    }

    __attribute__((aligned(64))) float simd_dotProdVector[SRSLTE_SIMD_CF_SIZE];
    simd_f_t                           acc_re = srslte_simd_cf_re(avx_result);
    simd_f_t                           acc_im = srslte_simd_cf_im(avx_result);

    simd_f_t acc = srslte_simd_f_hadd(acc_re, acc_im);
    for (int j = 2; j < SRSLTE_SIMD_F_SIZE; j *= 2) {
      acc = srslte_simd_f_hadd(acc, acc);
    }
    srslte_simd_f_store(simd_dotProdVector, acc);
    __real__ result = simd_dotProdVector[0];
    __imag__ result = simd_dotProdVector[1];
  }
#endif

  for (; i < len; i++) {
    result += (x[i] * y[i]);
  }

  return result;
}

static void estimate_wiener(srslte_wiener_dl_t* q,
                            const cf_t          wm[SRSLTE_WIENER_DL_MIN_RE][SRSLTE_WIENER_DL_MIN_REF],
                            cf_t*               ref,
                            cf_t*               h)
{
  uint32_t r_offset = 0; // Resource Element indexing offset
  uint32_t p_offset = 0; // Pilot indexing offset

  // Estimate lower band
  for (uint32_t i = 0; i < SRSLTE_WIENER_DL_MIN_RE; i++) {
    h[r_offset + i] = _srslte_vec_dot_prod_ccc_simd(&ref[p_offset], wm[i], SRSLTE_WIENER_DL_MIN_REF);
  }

  // Estimate Upper band (it might overlap in 6PRB cells with the lower band)
  r_offset = q->nof_re - SRSLTE_WIENER_DL_MIN_RE;
  p_offset = q->nof_ref - SRSLTE_WIENER_DL_MIN_REF;
  for (uint32_t i = 0; i < SRSLTE_WIENER_DL_MIN_RE; i++) {
    h[r_offset + i] = _srslte_vec_dot_prod_ccc_simd(&ref[p_offset], wm[i], SRSLTE_WIENER_DL_MIN_REF);
  }

  // Estimate center Resource elements
  if (q->nof_re > 2 * SRSLTE_WIENER_DL_MIN_RE) {
    for (uint32_t prb = SRSLTE_WIENER_DL_MIN_PRB / 2; prb < q->nof_prb - SRSLTE_WIENER_DL_MIN_REF / 2;
         prb += SRSLTE_WIENER_DL_MIN_PRB / 2) {
      uint32_t ref_idx = prb * 2 - SRSLTE_WIENER_DL_MIN_REF / 2;
      uint32_t re_idx  = prb * SRSLTE_NRE;
      for (uint32_t i = SRSLTE_WIENER_DL_MIN_RE / 4; i < (3 * SRSLTE_WIENER_DL_MIN_RE) / 4; i++) {
        h[re_idx + i] = _srslte_vec_dot_prod_ccc_simd(&ref[ref_idx], wm[i], SRSLTE_WIENER_DL_MIN_REF);
      }
    }
  }
}

SRSLTE_WIENER_LOCAL void
srslte_wiener_dl_run_symbol_1_8(srslte_wiener_dl_t* q, srslte_wiener_dl_state_t* state, cf_t* pilots, float snr_lin)
{

  // there are pilot symbols (even) in this OFDM period (first symbol of the slot)
  circshift_dim1(state->hls_fifo_2, SRSLTE_WIENER_DL_HLS_FIFO_SIZE, 1); // shift matrix rows down one position
  memcpy(state->hls_fifo_2[0], pilots, NSAMPLES2NBYTES(q->nof_ref));

  // Online training for pilot filtering
  circshift_dim2(&state->timefifo, 1, SRSLTE_WIENER_DL_TIMEFIFO_SIZE, 1); // shift columns right one position
  state->timefifo[0] = conjf(pilots[q->nof_ref / 2]);                     // train with center of subband frequency

  circshift_dim1(state->cxfifo, SRSLTE_WIENER_DL_CXFIFO_SIZE, 1); // shift rows down one position
  srslte_vec_sc_prod_ccc(state->timefifo, pilots[q->nof_ref / 2], state->cxfifo[0], SRSLTE_WIENER_DL_TIMEFIFO_SIZE);

  // Calculate auto-correlation and normalize
  matrix_acc_dim1_cc(state->cxfifo, q->tmp, SRSLTE_WIENER_DL_CXFIFO_SIZE, SRSLTE_WIENER_DL_TIMEFIFO_SIZE);
  srslte_vec_sc_prod_cfc(q->tmp, 1.0f / SRSLTE_WIENER_DL_CXFIFO_SIZE, q->tmp, SRSLTE_WIENER_DL_TIMEFIFO_SIZE);

  // Find index of half amplitude
  uint32_t halfcx = vec_find_first_smaller_than_cf(q->tmp, cabsf(q->tmp[1]) * 0.5f, SRSLTE_WIENER_DL_TFIFO_SIZE, 2);

  // Update internal states
  state->sumlen       = SRSLTE_MAX(1, floorf(halfcx / 8.0f * SRSLTE_MIN(2.0f, 1.0f + 1.0f / snr_lin)));
  state->skip         = SRSLTE_MAX(1, floorf(halfcx / 4.0f * SRSLTE_MIN(1, snr_lin / 16.0f)));
  state->deltan       = 0;
  state->invtpilotoff = M_1_3;
}

SRSLTE_WIENER_LOCAL void srslte_wiener_dl_run_symbol_2_9(srslte_wiener_dl_t* q, srslte_wiener_dl_state_t* state)
{

  // here we only shift and feed TD interpolation fifo
  circshift_dim1(state->tfifo, SRSLTE_WIENER_DL_TFIFO_SIZE, 1); // shift matrix columns right by one position

  // Average Reference Signals
  matrix_acc_dim1_cc(state->hls_fifo_2, q->tmp, SRSLTE_WIENER_DL_HLS_FIFO_SIZE, q->nof_ref); // Sum values
  srslte_vec_sc_prod_cfc(q->tmp, 1.0f / state->sumlen, q->tmp, q->nof_ref);                  // Sacle sum

  // Estimate channel based on the wiener matrix 2
  estimate_wiener(q, q->wm2, q->tmp, state->tfifo[0]);

  // Update internal states
  state->deltan       = 0.0f;
  state->invtpilotoff = M_1_3;
}

SRSLTE_WIENER_LOCAL void srslte_wiener_dl_run_symbol_5_12(srslte_wiener_dl_t*       q,
                                                          srslte_wiener_dl_state_t* state,
                                                          cf_t*                     pilots,
                                                          uint32_t                  tx,
                                                          uint32_t                  rx,
                                                          uint32_t                  shift,
                                                          float                     snr_lin)
{
  // there are pilot symbols (odd) in this OFDM period (fifth symbol of the slot)
  circshift_dim1(state->hls_fifo_1, SRSLTE_WIENER_DL_HLS_FIFO_SIZE, 1); // shift matrix rows down one position
  memcpy(state->hls_fifo_1[0], pilots, NSAMPLES2NBYTES(q->nof_ref));

  circshift_dim1(state->tfifo, SRSLTE_WIENER_DL_TFIFO_SIZE, 1); // shift matrix columns right by one position

  // Average Reference Signals
  matrix_acc_dim1_cc(state->hls_fifo_1, q->tmp, SRSLTE_WIENER_DL_HLS_FIFO_SIZE, q->nof_ref); // Sum values
  srslte_vec_sc_prod_cfc(q->tmp, 1.0f / state->sumlen, q->tmp, q->nof_ref);                  // Sacle sum

  // Estimate channel based on the wiener matrix 1
  estimate_wiener(q, q->wm1, q->tmp, state->tfifo[0]);

  // Update internal states
  state->deltan       = 0.0f;
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
    for (uint32_t i = pos2, k = pstart; i < SRSLTE_WIENER_DL_MIN_RE; i += 6, k++) {
      q->hlsv[i] = conjf(state->hls_fifo_2[1][k] + (state->hls_fifo_2[0][k] - state->hls_fifo_2[1][k]) * M_4_7);
    }
    for (uint32_t i = pos1, k = pstart; i < SRSLTE_WIENER_DL_MIN_RE; i += 6, k++) {
      q->hlsv[i] = conjf(state->hls_fifo_1[1][k]);
    }

    // Correlate Least Squares estimation
    bzero(q->hlsv_sum, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE)); // Zero correlation vector
    for (uint32_t i = 0; i < SRSLTE_WIENER_DL_MIN_REF * 2; i++) {
      uint32_t offset  = i * 3;
      uint32_t sum_len = SRSLTE_WIENER_DL_MIN_RE - offset;
      srslte_vec_sc_prod_ccc(&q->hlsv[offset], conjf(q->hlsv[offset]), q->tmp, sum_len);
      srslte_vec_sum_ccc(q->tmp, q->hlsv_sum, q->hlsv_sum, sum_len);
    }
    srslte_vec_prod_cfc(q->hlsv_sum, hlsv_sum_norm, q->hlsv_sum, SRSLTE_WIENER_DL_MIN_RE); // Normalize correlation

    // Put correlation in FIFO
    state->nfifosamps = SRSLTE_MIN(state->nfifosamps + 1, SRSLTE_WIENER_DL_XFIFO_SIZE);
    circshift_dim1(state->xfifo, state->nfifosamps, 1);
    memcpy(state->xfifo[0], q->hlsv_sum, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE));

    // Average samples in FIFO
    matrix_acc_dim1_cc(state->xfifo, state->cV, SRSLTE_WIENER_DL_XFIFO_SIZE, SRSLTE_WIENER_DL_MIN_RE);

    // Interpolate
    srslte_dft_run_c(&q->fft, state->cV, q->tmp);
    srslte_vec_prod_ccc(q->tmp, q->filter, q->tmp, SRSLTE_WIENER_DL_MIN_RE);
    srslte_dft_run_c(&q->ifft, q->tmp, state->cV);

    // Interpolate last edge
    state->cV[SRSLTE_WIENER_DL_MIN_RE - 2] =
        state->cV[SRSLTE_WIENER_DL_MIN_RE - 6] +
        (state->cV[SRSLTE_WIENER_DL_MIN_RE - 3] - state->cV[SRSLTE_WIENER_DL_MIN_RE - 6]) * M_4_3;
    state->cV[SRSLTE_WIENER_DL_MIN_RE - 1] =
        state->cV[SRSLTE_WIENER_DL_MIN_RE - 6] +
        (state->cV[SRSLTE_WIENER_DL_MIN_RE - 3] - state->cV[SRSLTE_WIENER_DL_MIN_RE - 6]) * M_5_3;

    if (tx == q->nof_tx_ports - 1 && rx == q->nof_rx_ant - 1) {
      // Average correlation vectors
      for (uint32_t i = 0; i < q->nof_tx_ports; i++) {
        for (uint32_t j = 0; j < q->nof_rx_ant; j++) {
          if (i == 0 && j == 0) {
            // Copy if first one
            memcpy(q->acV, q->state[i][j]->cV, NSAMPLES2NBYTES(SRSLTE_WIENER_DL_MIN_RE));
          } else {
            // Accumulate otherwise
            srslte_vec_sum_ccc(q->state[i][j]->cV, q->acV, q->acV, SRSLTE_WIENER_DL_MIN_RE);
          }
        }
      }

      // Apply averaging scale
      srslte_vec_sc_prod_cfc(q->acV, 1.0f / (q->nof_tx_ports + q->nof_rx_ant), q->acV, SRSLTE_WIENER_DL_MIN_RE);

      // Compute square wiener correlation matrix
      for (uint32_t i = 0; i < SRSLTE_WIENER_DL_MIN_REF; i++) {
        for (uint32_t k = i; k < SRSLTE_WIENER_DL_MIN_REF; k++) {
          q->RH.m[i][k] = q->acV[6 * (k - i)];
          q->RH.m[k][i] = conjf(q->RH.m[i][k]);
        }
      }

      // Add noise contribution to the square wiener
      float N = (__real__ q->acV[0] / SRSLTE_MIN(15, snr_lin * state->sumlen));
      for (uint32_t i = 0; i < SRSLTE_WIENER_DL_MIN_REF; i++) {
        q->RH.m[i][i] += N;
      }

      // Compute wiener correlation inverse matrix
      srslte_matrix_NxN_inv_run(q->matrix_inverter, q->RH.v, q->invRH.v);

      // Generate Rectangular Wiener
      for (uint32_t i = 0; i < SRSLTE_WIENER_DL_MIN_RE; i++) {
        for (uint32_t k = 0; k < SRSLTE_WIENER_DL_MIN_REF; k++) {
          int m1 = ((shift + 3) % 6) + 6 * k - i;
          int m2 = shift + 6 * k - i;

          if (m1 >= 0) {
            q->hH1[i][k] = q->acV[m1];
          } else {
            q->hH1[i][k] = conjf(q->acV[-m1]);
          }

          if (m2 >= 0) {
            q->hH2[i][k] = q->acV[m2];
          } else {
            q->hH2[i][k] = conjf(q->acV[-m2]);
          }
        }
      }

      // Compute Wiener matrices
      for (uint32_t dim1 = 0; dim1 < SRSLTE_WIENER_DL_MIN_RE; dim1++) {
        for (uint32_t dim2 = 0; dim2 < SRSLTE_WIENER_DL_MIN_REF; dim2++) {
          q->wm1[dim1][dim2] = srslte_vec_dot_prod_ccc(q->hH1[dim1], q->invRH.m[dim2], SRSLTE_WIENER_DL_MIN_REF);
          q->wm2[dim1][dim2] = srslte_vec_dot_prod_ccc(q->hH2[dim1], q->invRH.m[dim2], SRSLTE_WIENER_DL_MIN_REF);
        }
      }
    }
  }
}

int srslte_wiener_dl_run(srslte_wiener_dl_t* q,
                         uint32_t            tx,
                         uint32_t            rx,
                         uint32_t            m,
                         uint32_t            shift,
                         cf_t*               pilots,
                         cf_t*               estimated,
                         float               snr_lin)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q) {
    // m is based on 0, increase one;
    m++;

    // Get estimator state
    srslte_wiener_dl_state_t* state = q->state[tx][rx];

    // Process symbol
    switch (m) {
      case 1:
      case 8:
        srslte_wiener_dl_run_symbol_1_8(q, state, pilots, snr_lin);
        break;
      case 2:
      case 9:
        srslte_wiener_dl_run_symbol_2_9(q, state);
        break;
      case 5:
      case 12:
        srslte_wiener_dl_run_symbol_5_12(q, state, pilots, tx, rx, shift, snr_lin);
        break;
      default:
          /* Do nothing */;
    }

    // Estimate
    srslte_vec_sub_ccc(state->tfifo[0], state->tfifo[1], q->tmp, q->nof_re);
    srslte_vec_sc_prod_cfc(q->tmp, state->deltan * state->invtpilotoff, q->tmp, q->nof_re);
    srslte_vec_sum_ccc(state->tfifo[1], q->tmp, estimated, q->nof_re);
    state->deltan += 1.0f;

    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

void srslte_wiener_dl_free(srslte_wiener_dl_t* q)
{
  if (q) {
    for (uint32_t tx = 0; tx < SRSLTE_MAX_PORTS; tx++) {
      for (uint32_t rx = 0; rx < SRSLTE_MAX_PORTS; rx++) {
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

    srslte_dft_plan_free(&q->fft);
    srslte_dft_plan_free(&q->ifft);

    if (q->matrix_inverter) {
      srslte_matrix_NxN_inv_free(q->matrix_inverter);
      free(q->matrix_inverter);
    }
  }
}
