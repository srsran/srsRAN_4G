/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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
#include <srsran/phy/ch_estimation/wiener_dl.h>
#include <srsran/phy/utils/mat.h>
#include <srsran/srsran.h>

// Useful macros
#define NSAMPLES2NBYTES(N) (sizeof(cf_t) * (N))
#define M_1_3 0.33333333333333333333f /* 1 / 3 */
#define M_2_3 0.66666666666666666666f /* 2 / 3 */
#define M_1_4 0.25f                   /* 1 / 4 */
#define M_4_7 0.571428571f            /* 4 / 7 */
#define M_4_3 1.33333333333333333333f /* 4 / 3 */
#define M_5_3 1.66666666666666666666f /* 5 / 3 */
#define SRSRAN_WIENER_HALFREF_IDX (q->nof_ref / 2 - 1)

// Constants
const float hlsv_sum_norm[SRSRAN_WIENER_DL_MIN_RE] = {0.0625f,
                                                      0.0638297872326845f,
                                                      0.0652173913015123f,
                                                      0.0666666666622222f,
                                                      0.0681818181756198f,
                                                      0.0697674418523526f,
                                                      0.0714285714183674f,
                                                      0.0731707316948245f,
                                                      0.074999999985f,
                                                      0.0769230769053254f,
                                                      0.078947368400277f,
                                                      0.0810810810569759f,
                                                      0.0833333333055555f,
                                                      0.085714285682449f,
                                                      0.0882352940813149f,
                                                      0.0909090908677686f,
                                                      0.093749999953125f,
                                                      0.0967741934953174f,
                                                      0.09999999994f,
                                                      0.103448275794293f,
                                                      0.107142857066327f,
                                                      0.111111111024691f,
                                                      0.115384615286982f,
                                                      0.1199999998896f,
                                                      0.124999999875f,
                                                      0.130434782466919f,
                                                      0.136363636202479f,
                                                      0.142857142673469f,
                                                      0.14999999979f,
                                                      0.157894736601108f,
                                                      0.166666666388889f,
                                                      0.176470587913495f,
                                                      0.187499999625f,
                                                      0.19999999956f,
                                                      0.214285713765306f,
                                                      0.230769230147929f,
                                                      0.24999999925f,
                                                      0.272727271809917f,
                                                      0.29999999886f,
                                                      0.333333331888889f,
                                                      0.374999998125f,
                                                      0.428571426061225f,
                                                      0.4999999965f,
                                                      0.59999999484f,
                                                      0.74999999175f,
                                                      0.999999985f,
                                                      1.4999999655f,
                                                      2.99999985900001};

// Local state function prototypes
static srsran_wiener_dl_state_t* srsran_wiener_dl_state_malloc(srsran_wiener_dl_t* q);
static void                      srsran_wiener_dl_state_free(srsran_wiener_dl_state_t* q);
static void                      srsran_wiener_dl_state_reset(srsran_wiener_dl_t* q, srsran_wiener_dl_state_t* state);

// Local run function prototypes
static void
            srsran_wiener_dl_run_symbol_1_8(srsran_wiener_dl_t* q, srsran_wiener_dl_state_t* state, cf_t* pilots, float snr_lin);
static void srsran_wiener_dl_run_symbol_2_9(srsran_wiener_dl_t* q, srsran_wiener_dl_state_t* state);
static void srsran_wiener_dl_run_symbol_5_12(srsran_wiener_dl_t*       q,
                                             srsran_wiener_dl_state_t* state,
                                             cf_t*                     pilots,
                                             uint32_t                  tx,
                                             uint32_t                  rx,
                                             uint32_t                  shift,
                                             float                     snr_lin);

// Local state related functions
static srsran_wiener_dl_state_t* srsran_wiener_dl_state_malloc(srsran_wiener_dl_t* q)
{
  // Allocate Channel state
  srsran_wiener_dl_state_t* state = calloc(sizeof(srsran_wiener_dl_state_t), 1);

  // Check allocation
  if (!state) {
    perror("malloc");
  } else {
    int ret = SRSRAN_SUCCESS;

    // Allocate state variables
    for (int i = 0; i < SRSRAN_WIENER_DL_HLS_FIFO_SIZE && !ret; i++) {
      state->hls_fifo_1[i] = srsran_vec_malloc(NSAMPLES2NBYTES(q->max_ref));
      if (!state->hls_fifo_1[i]) {
        perror("malloc");
        ret = SRSRAN_ERROR;
      }

      if (!ret) {
        state->hls_fifo_2[i] = srsran_vec_malloc(NSAMPLES2NBYTES(q->max_ref));
        if (!state->hls_fifo_2[i]) {
          perror("malloc");
          ret = SRSRAN_ERROR;
        }
      }
    }

    for (uint32_t i = 0; i < SRSRAN_WIENER_DL_TFIFO_SIZE && !ret; i++) {
      state->tfifo[i] = srsran_vec_malloc(NSAMPLES2NBYTES(q->max_re));
      if (!state->tfifo[i]) {
        perror("malloc");
        ret = SRSRAN_ERROR;
      }
    }

    for (uint32_t i = 0; i < SRSRAN_WIENER_DL_XFIFO_SIZE && !ret; i++) {
      state->xfifo[i] = srsran_vec_malloc(NSAMPLES2NBYTES(SRSRAN_WIENER_DL_MIN_RE));
      if (!state->xfifo[i]) {
        perror("malloc");
        ret = SRSRAN_ERROR;
      }
    }

    if (!ret) {
      state->timefifo = srsran_vec_malloc(NSAMPLES2NBYTES(SRSRAN_WIENER_DL_TIMEFIFO_SIZE));
      if (!state->timefifo) {
        perror("malloc");
        ret = SRSRAN_ERROR;
      }
    }

    for (uint32_t i = 0; i < SRSRAN_WIENER_DL_CXFIFO_SIZE && !ret; i++) {
      state->cxfifo[i] = srsran_vec_malloc(NSAMPLES2NBYTES(SRSRAN_WIENER_DL_TIMEFIFO_SIZE));
      if (!state->cxfifo[i]) {
        perror("malloc");
        ret = SRSRAN_ERROR;
      }
    }

    // Initialise the rest
    state->deltan       = 0.0f;
    state->nfifosamps   = 0;
    state->invtpilotoff = 0;
    state->sumlen       = 1;
    state->skip         = 1;
    state->cnt          = 0;

    if (ret) {
      // Free all allocated memory
      srsran_wiener_dl_state_free(state);

      // Return NULL if error
      state = NULL;
    }
  }

  return state;
}

static void srsran_wiener_dl_state_reset(srsran_wiener_dl_t* q, srsran_wiener_dl_state_t* state)
{
  if (q && state) {
    // Initialise memory
    for (uint32_t i = 0; i < SRSRAN_WIENER_DL_HLS_FIFO_SIZE; i++) {
      bzero(state->hls_fifo_1[i], NSAMPLES2NBYTES(q->nof_ref));
      bzero(state->hls_fifo_2[i], NSAMPLES2NBYTES(q->nof_ref));
    }
    for (uint32_t i = 0; i < SRSRAN_WIENER_DL_TFIFO_SIZE; i++) {
      bzero(state->tfifo[i], NSAMPLES2NBYTES(q->nof_re));
    }
    for (uint32_t i = 0; i < SRSRAN_WIENER_DL_XFIFO_SIZE; i++) {
      bzero(state->xfifo[i], NSAMPLES2NBYTES(SRSRAN_WIENER_DL_MIN_RE));
    }
    bzero(state->cV, NSAMPLES2NBYTES(SRSRAN_WIENER_DL_MIN_RE));
    bzero(state->timefifo, NSAMPLES2NBYTES(SRSRAN_WIENER_DL_TIMEFIFO_SIZE));

    for (uint32_t i = 0; i < SRSRAN_WIENER_DL_CXFIFO_SIZE; i++) {
      bzero(state->cxfifo[i], NSAMPLES2NBYTES(SRSRAN_WIENER_DL_TIMEFIFO_SIZE));
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

static void srsran_wiener_dl_state_free(srsran_wiener_dl_state_t* q)
{

  if (q) {
    for (int i = 0; i < SRSRAN_WIENER_DL_HLS_FIFO_SIZE; i++) {
      if (q->hls_fifo_1[i]) {
        free(q->hls_fifo_1[i]);
      }
      if (q->hls_fifo_2[i]) {
        free(q->hls_fifo_2[i]);
      }
    }
    for (uint32_t i = 0; i < SRSRAN_WIENER_DL_TFIFO_SIZE; i++) {
      if (q->tfifo[i]) {
        free(q->tfifo[i]);
      }
    }
    for (uint32_t i = 0; i < SRSRAN_WIENER_DL_XFIFO_SIZE; i++) {
      if (q->xfifo[i]) {
        free(q->xfifo[i]);
      }
    }
    for (uint32_t i = 0; i < SRSRAN_WIENER_DL_CXFIFO_SIZE; i++) {
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

int srsran_wiener_dl_init(srsran_wiener_dl_t* q, uint32_t max_prb, uint32_t max_tx_ports, uint32_t max_rx_ant)
{
  int ret = SRSRAN_SUCCESS;

  if (q && max_prb <= SRSRAN_MAX_PRB && max_tx_ports <= SRSRAN_MAX_PORTS && max_rx_ant <= SRSRAN_MAX_PORTS) {
    // Bzero structure
    bzero(q, sizeof(srsran_wiener_dl_t));

    // Set maximum parameters
    q->max_prb      = max_prb;
    q->max_ref      = max_prb * 2;
    q->max_re       = max_prb * SRSRAN_NRE;
    q->max_tx_ports = max_tx_ports;
    q->max_rx_ant   = max_rx_ant;

    // Allocate state
    for (uint32_t tx = 0; tx < q->max_tx_ports && !ret; tx++) {
      for (uint32_t rx = 0; rx < q->max_rx_ant && !ret; rx++) {
        srsran_wiener_dl_state_t* state = srsran_wiener_dl_state_malloc(q);
        if (!state) {
          perror("srsran_wiener_dl_state_malloc");
          ret = SRSRAN_ERROR;
        } else {
          q->state[tx][rx] = state;
        }
      }
    }

    // Allocate temporal buffers, maximum SF size
    if (!ret) {
      q->tmp = srsran_vec_malloc(NSAMPLES2NBYTES(SRSRAN_SF_LEN_MAX));
      if (!q->tmp) {
        perror("malloc");
        ret = SRSRAN_ERROR;
      }
    }

    if (!ret) {
      q->random = srsran_random_init(0xdead);
      if (!q->random) {
        perror("srsran_random_init");
        ret = SRSRAN_ERROR;
      }
    }

    // Create filter FFT/iFFT plans
    if (!ret) {
      ret = srsran_dft_plan_c(&q->fft, SRSRAN_WIENER_DL_MIN_RE, SRSRAN_DFT_FORWARD);
    }

    if (!ret) {
      ret = srsran_dft_plan_c(&q->ifft, SRSRAN_WIENER_DL_MIN_RE, SRSRAN_DFT_BACKWARD);
    }

    // Initialise interpolation filter
    if (!ret) {
      bzero(q->filter, NSAMPLES2NBYTES(SRSRAN_WIENER_DL_MIN_RE));
      q->filter[0]                           = 1.0f / SRSRAN_WIENER_DL_MIN_RE;
      q->filter[1]                           = M_2_3 / SRSRAN_WIENER_DL_MIN_RE;
      q->filter[2]                           = M_1_3 / SRSRAN_WIENER_DL_MIN_RE;
      q->filter[SRSRAN_WIENER_DL_MIN_RE - 2] = M_1_3 / SRSRAN_WIENER_DL_MIN_RE;
      q->filter[SRSRAN_WIENER_DL_MIN_RE - 1] = M_2_3 / SRSRAN_WIENER_DL_MIN_RE;
      srsran_dft_run_c(&q->fft, q->filter, q->filter);
    }

    // Initialise matrix inverter
    if (!ret) {
      q->matrix_inverter = calloc(sizeof(srsran_matrix_NxN_inv_t), 1);
      if (q->matrix_inverter) {
        ret = srsran_matrix_NxN_inv_init(q->matrix_inverter, SRSRAN_WIENER_DL_MIN_REF);
      } else {
        perror("calloc");
        ret = SRSRAN_ERROR;
      }
    }
  }

  return ret;
}

int srsran_wiener_dl_set_cell(srsran_wiener_dl_t* q, srsran_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q) {
    // No invalid inputs
    ret = SRSRAN_SUCCESS;

    // Set new values
    q->nof_prb      = cell.nof_prb;
    q->nof_ref      = cell.nof_prb * 2;
    q->nof_re       = cell.nof_prb * SRSRAN_NRE;
    q->nof_tx_ports = cell.nof_ports;
    q->nof_rx_ant   = q->max_rx_ant;
    q->ready        = false;
    q->wm_computed  = false;

    // Reset states
    srsran_wiener_dl_reset(q);
  }

  return ret;
}

void srsran_wiener_dl_reset(srsran_wiener_dl_t* q)
{
  if (q) {
    // Reset states
    for (uint32_t tx = 0; tx < SRSRAN_MAX_PORTS; tx++) {
      for (uint32_t rx = 0; rx < SRSRAN_MAX_PORTS; rx++) {
        if (q->state[tx][rx]) {
          srsran_wiener_dl_state_reset(q, q->state[tx][rx]);
        }
      }
    }

    // Reset wiener
    bzero(q->wm1, NSAMPLES2NBYTES(SRSRAN_WIENER_DL_MIN_REF * SRSRAN_WIENER_DL_MIN_RE));
    bzero(q->wm2, NSAMPLES2NBYTES(SRSRAN_WIENER_DL_MIN_REF * SRSRAN_WIENER_DL_MIN_RE));
  }
}

static void circshift_dim1(cf_t** matrix, uint32_t ndim1, int32_t k)
{
  // Check valid inputs
  if (matrix != NULL && ndim1 != 0 && k != 0) {
    // Wrap k
    k = (k + ndim1) % ndim1;

    // Run k times
    while (k--) {
      // Save last pointer
      cf_t* tmp_ptr = matrix[ndim1 - 1];

      // Shift pointers one position
      for (int i = ndim1 - 1; i > 0; i--) {
        matrix[i] = matrix[i - 1];
      }

      // Save last pointer
      matrix[0] = tmp_ptr;
    }
  } else {
    ERROR("unattended circshift_dim1!");
  }
}

static void circshift_dim2(cf_t** matrix, uint32_t ndim1, uint32_t ndim2, int32_t k)
{
  // Wrap k
  k = (k + ndim2) % ndim2;

  for (uint32_t dim1 = 0; dim1 < ndim1; dim1++) {
    // Run k times
    for (int i = 0; i < k; i++) {
      // Save last value
      cf_t tmp = matrix[dim1][ndim2 - 1];

      // Shift one position
      for (int dim2 = ndim2 - 1; dim2 > 0; dim2--) {
        matrix[dim1][dim2] = matrix[dim1][dim2 - 1];
      }

      // Save last value
      matrix[dim1][0] = tmp;
    }
  }
}

static void matrix_acc_dim1_cc(cf_t** matrix, cf_t* res, uint32_t ndim1, uint32_t ndim2)
{
  int dim2 = 0;

#if SRSRAN_SIMD_CF_SIZE
  for (; dim2 < ndim2 - SRSRAN_SIMD_CF_SIZE / 2 + 1; dim2 += SRSRAN_SIMD_CF_SIZE / 2) {
    simd_f_t acc = srsran_simd_f_zero();
    for (uint32_t dim1 = 0; dim1 < ndim1; dim1++) {
      simd_f_t reg = srsran_simd_f_loadu((float*)&matrix[dim1][dim2]);
      acc          = srsran_simd_f_add(acc, reg);
    }
    srsran_simd_f_storeu((float*)&res[dim2], acc);
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
    res[dim1] = srsran_vec_acc_cc(matrix[dim1], ndim2);
  }
}*/

static inline uint32_t vec_find_first_smaller_than_cf(cf_t* x, float y, uint32_t n, uint32_t pos)
{
  uint32_t ret = n;

  for (uint32_t i = pos; i < n && ret == n; i++) {
    if (cabsf(x[i]) <= y) {
      ret = i - pos + 1;
    }
  }

  return ret;
}

static inline cf_t _srsran_vec_dot_prod_ccc_simd(const cf_t* x, const cf_t* y, const int len)
{
  int  i      = 0;
  cf_t result = 0;

#if SRSRAN_SIMD_CF_SIZE
  if (len >= SRSRAN_SIMD_CF_SIZE) {
    simd_cf_t avx_result = srsran_simd_cf_zero();
    for (; i < len - SRSRAN_SIMD_CF_SIZE + 1; i += SRSRAN_SIMD_CF_SIZE) {
      simd_cf_t xVal = srsran_simd_cfi_loadu(&x[i]);
      simd_cf_t yVal = srsran_simd_cfi_loadu(&y[i]);

      avx_result = srsran_simd_cf_add(srsran_simd_cf_prod(xVal, yVal), avx_result);
    }

    __attribute__((aligned(64))) float simd_dotProdVector[SRSRAN_SIMD_CF_SIZE];
    simd_f_t                           acc_re = srsran_simd_cf_re(avx_result);
    simd_f_t                           acc_im = srsran_simd_cf_im(avx_result);

    simd_f_t acc = srsran_simd_f_hadd(acc_re, acc_im);
    for (int j = 2; j < SRSRAN_SIMD_F_SIZE; j *= 2) {
      acc = srsran_simd_f_hadd(acc, acc);
    }
    srsran_simd_f_store(simd_dotProdVector, acc);
    __real__ result = simd_dotProdVector[0];
    __imag__ result = simd_dotProdVector[1];
  }
#endif

  for (; i < len; i++) {
    result += (x[i] * y[i]);
  }

  return result;
}

static void estimate_wiener(srsran_wiener_dl_t* q,
                            const cf_t          wm[SRSRAN_WIENER_DL_MIN_RE][SRSRAN_WIENER_DL_MIN_REF],
                            cf_t*               ref,
                            cf_t*               h)
{
  uint32_t r_offset = 0; // Resource Element indexing offset
  uint32_t p_offset = 0; // Pilot indexing offset

  // Estimate lower band
  for (uint32_t i = 0; i < SRSRAN_WIENER_DL_MIN_RE; i++) {
    h[r_offset + i] = _srsran_vec_dot_prod_ccc_simd(&ref[p_offset], wm[i], SRSRAN_WIENER_DL_MIN_REF);
  }

  // Estimate Upper band (it might overlap in 6PRB cells with the lower band)
  r_offset = q->nof_re - SRSRAN_WIENER_DL_MIN_RE;
  p_offset = q->nof_ref - SRSRAN_WIENER_DL_MIN_REF;
  for (uint32_t i = 0; i < SRSRAN_WIENER_DL_MIN_RE; i++) {
    h[r_offset + i] = _srsran_vec_dot_prod_ccc_simd(&ref[p_offset], wm[i], SRSRAN_WIENER_DL_MIN_REF);
  }

  // Estimate center Resource elements
  if (q->nof_re > 2 * SRSRAN_WIENER_DL_MIN_RE) {
    for (uint32_t prb = 2; prb < q->nof_prb - 2; prb += 2) {
      p_offset = (prb - 1) * 2;
      r_offset = prb * SRSRAN_NRE;
      for (uint32_t i = 0; i < SRSRAN_NRE * 2; i++) {
        h[r_offset + i] = _srsran_vec_dot_prod_ccc_simd(&ref[p_offset], wm[i + SRSRAN_NRE], SRSRAN_WIENER_DL_MIN_REF);
      }
    }
  }
}

inline static cf_t _cmul(cf_t a, cf_t b)
{
  cf_t ret = 0;

  __real__ ret = __real__ a * __real__ b - __imag__ a * __imag__ b;
  __imag__ ret = __real__ a * __imag__ b + __imag__ a * __real__ b;

  return ret;
}

static void
srsran_wiener_dl_run_symbol_1_8(srsran_wiener_dl_t* q, srsran_wiener_dl_state_t* state, cf_t* pilots, float snr_lin)
{

  // there are pilot symbols (even) in this OFDM period (first symbol of the slot)
  circshift_dim1(state->hls_fifo_2, SRSRAN_WIENER_DL_HLS_FIFO_SIZE, 1); // shift matrix rows down one position
  memcpy(state->hls_fifo_2[0], pilots, NSAMPLES2NBYTES(q->nof_ref));

  // Online training for pilot filtering
  circshift_dim2(&state->timefifo, 1, SRSRAN_WIENER_DL_TIMEFIFO_SIZE, 1); // shift columns right one position
  state->timefifo[0] = conjf(pilots[SRSRAN_WIENER_HALFREF_IDX]);          // train with center of subband frequency

  circshift_dim1(state->cxfifo, SRSRAN_WIENER_DL_CXFIFO_SIZE, 1); // shift rows down one position
  srsran_vec_sc_prod_ccc(
      state->timefifo, pilots[SRSRAN_WIENER_HALFREF_IDX], state->cxfifo[0], SRSRAN_WIENER_DL_TIMEFIFO_SIZE);

  // Calculate auto-correlation and normalize
  matrix_acc_dim1_cc(state->cxfifo, q->tmp, SRSRAN_WIENER_DL_CXFIFO_SIZE, SRSRAN_WIENER_DL_TIMEFIFO_SIZE);
  srsran_vec_sc_prod_cfc(q->tmp, 1.0f / SRSRAN_WIENER_DL_CXFIFO_SIZE, q->tmp, SRSRAN_WIENER_DL_TIMEFIFO_SIZE);

  // Find index of half amplitude
  uint32_t halfcx = vec_find_first_smaller_than_cf(q->tmp, cabsf(q->tmp[1]) * 0.5f, SRSRAN_WIENER_DL_TIMEFIFO_SIZE, 2);

  // Update internal states
  state->sumlen       = SRSRAN_MAX(1, floorf(halfcx / 8.0f * SRSRAN_MIN(2.0f, 1.0f + 1.0f / snr_lin)));
  state->skip         = SRSRAN_MAX(1, floorf(halfcx / 4.0f * SRSRAN_MIN(1, snr_lin / 16.0f)));
}

static void srsran_wiener_dl_run_symbol_2_9(srsran_wiener_dl_t* q, srsran_wiener_dl_state_t* state)
{

  // here we only shift and feed TD interpolation fifo
  circshift_dim1(state->tfifo, SRSRAN_WIENER_DL_TFIFO_SIZE, 1); // shift matrix columns right by one position

  // Average Reference Signals
  matrix_acc_dim1_cc(state->hls_fifo_2, q->tmp, state->sumlen, q->nof_ref); // Sum values
  srsran_vec_sc_prod_cfc(q->tmp, 1.0f / state->sumlen, q->tmp, q->nof_ref); // Scale sum

  // Estimate channel based on the wiener matrix 2
  estimate_wiener(q, q->wm2, q->tmp, state->tfifo[0]);

  // Update internal states
  state->deltan       = 0.0f;
  state->invtpilotoff = M_1_3;
}

static void srsran_wiener_dl_run_symbol_5_12(srsran_wiener_dl_t*       q,
                                             srsran_wiener_dl_state_t* state,
                                             cf_t*                     pilots,
                                             uint32_t                  tx,
                                             uint32_t                  rx,
                                             uint32_t                  shift,
                                             float                     snr_lin)
{
  // there are pilot symbols (odd) in this OFDM period (fifth symbol of the slot)
  circshift_dim1(state->hls_fifo_1, SRSRAN_WIENER_DL_HLS_FIFO_SIZE, 1); // shift matrix rows down one position
  memcpy(state->hls_fifo_1[0], pilots, NSAMPLES2NBYTES(q->nof_ref));

  circshift_dim1(state->tfifo, SRSRAN_WIENER_DL_TFIFO_SIZE, 1); // shift matrix columns right by one position

  // Average Reference Signals
  matrix_acc_dim1_cc(state->hls_fifo_1, q->tmp, state->sumlen, q->nof_ref); // Sum values
  srsran_vec_sc_prod_cfc(q->tmp, 1.0f / state->sumlen, q->tmp, q->nof_ref); // Scale sum

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

    pos2 = (shift < 3) ? 0 : 3;
    pos1 = (pos2 + 3) % 6;

    // Choose randomly a pair of PRB and calculate the start reference signal
    nsbb = srsran_random_uniform_int_dist(q->random, 0, q->nof_prb / 2);
    if (nsbb == 0) {
      pstart = 0;
    } else if (nsbb >= (q->nof_prb / 2) - 1) {
      pstart = q->nof_ref - SRSRAN_WIENER_DL_MIN_REF;
    } else {
      pstart = (SRSRAN_WIENER_DL_MIN_REF / 2) * nsbb - 2;
    }

    bzero(q->hlsv, NSAMPLES2NBYTES(SRSRAN_WIENER_DL_MIN_RE));
    for (uint32_t i = pos2, k = pstart; i < SRSRAN_WIENER_DL_MIN_RE; i += 6, k++) {
      q->hlsv[i] = conjf(state->hls_fifo_2[1][k] + (state->hls_fifo_2[0][k] - state->hls_fifo_2[1][k]) * M_4_7);
    }
    for (uint32_t i = pos1, k = pstart; i < SRSRAN_WIENER_DL_MIN_RE; i += 6, k++) {
      q->hlsv[i] = conjf(state->hls_fifo_1[1][k]);
    }

    // Correlate Least Squares estimation
    bzero(q->hlsv_sum, NSAMPLES2NBYTES(SRSRAN_WIENER_DL_MIN_RE)); // Zero correlation vector
    for (uint32_t i = 0; i < SRSRAN_WIENER_DL_MIN_REF * 2; i++) {
      uint32_t offset  = i * 3;
      uint32_t sum_len = SRSRAN_WIENER_DL_MIN_RE - offset;
      srsran_vec_sc_prod_ccc(&q->hlsv[offset], conjf(q->hlsv[offset]), q->tmp, sum_len);
      srsran_vec_sum_ccc(q->tmp, q->hlsv_sum, q->hlsv_sum, sum_len);
    }
    srsran_vec_prod_cfc(q->hlsv_sum, hlsv_sum_norm, q->hlsv_sum, SRSRAN_WIENER_DL_MIN_RE); // Normalize correlation

    // Put correlation in FIFO
    state->nfifosamps = SRSRAN_MIN(state->nfifosamps + 1, SRSRAN_WIENER_DL_XFIFO_SIZE);
    circshift_dim1(state->xfifo, state->nfifosamps, 1);
    memcpy(state->xfifo[0], q->hlsv_sum, NSAMPLES2NBYTES(SRSRAN_WIENER_DL_MIN_RE));

    // Average samples in FIFO
    matrix_acc_dim1_cc(state->xfifo, state->cV, SRSRAN_WIENER_DL_XFIFO_SIZE, SRSRAN_WIENER_DL_MIN_RE);
    if (state->nfifosamps) {
      srsran_vec_sc_prod_cfc(state->cV, 1.0f / state->nfifosamps, state->cV, SRSRAN_WIENER_DL_MIN_RE);
    }

    // Interpolate
    srsran_dft_run_c(&q->fft, state->cV, q->tmp);
    srsran_vec_prod_ccc(q->tmp, q->filter, q->tmp, SRSRAN_WIENER_DL_MIN_RE);
    srsran_dft_run_c(&q->ifft, q->tmp, state->cV);

    // Interpolate last edge
    state->cV[SRSRAN_WIENER_DL_MIN_RE - 2] =
        state->cV[SRSRAN_WIENER_DL_MIN_RE - 6] +
        (state->cV[SRSRAN_WIENER_DL_MIN_RE - 3] - state->cV[SRSRAN_WIENER_DL_MIN_RE - 6]) * M_4_3;
    state->cV[SRSRAN_WIENER_DL_MIN_RE - 1] =
        state->cV[SRSRAN_WIENER_DL_MIN_RE - 6] +
        (state->cV[SRSRAN_WIENER_DL_MIN_RE - 3] - state->cV[SRSRAN_WIENER_DL_MIN_RE - 6]) * M_5_3;

    if (tx == q->nof_tx_ports - 1 && rx == q->nof_rx_ant - 1) {
      // Average correlation vectors
      for (uint32_t i = 0; i < q->nof_tx_ports; i++) {
        for (uint32_t j = 0; j < q->nof_rx_ant; j++) {
          if (i == 0 && j == 0) {
            // Copy if first one
            memcpy(q->acV, q->state[i][j]->cV, NSAMPLES2NBYTES(SRSRAN_WIENER_DL_MIN_RE));

          } else {
            // Accumulate otherwise
            srsran_vec_sum_ccc(q->state[i][j]->cV, q->acV, q->acV, SRSRAN_WIENER_DL_MIN_RE);
          }
        }
      }

      // Apply averaging scale
      srsran_vec_sc_prod_cfc(q->acV, 1.0f / (q->nof_tx_ports * q->nof_rx_ant), q->acV, SRSRAN_WIENER_DL_MIN_RE);

      // Compute square wiener correlation matrix
      for (uint32_t i = 0; i < SRSRAN_WIENER_DL_MIN_REF; i++) {
        for (uint32_t k = i; k < SRSRAN_WIENER_DL_MIN_REF; k++) {
          q->RH.m[i][k] = q->acV[6 * (k - i)];
          q->RH.m[k][i] = conjf(q->RH.m[i][k]);
        }
      }

      // Add noise contribution to the square wiener
      float N = 0.0f;

      if (isnormal(__real__ q->acV[0]) && isnormal(snr_lin) && state->sumlen > 0) {
        N = (__real__ q->acV[0] / SRSRAN_MIN(15, snr_lin * state->sumlen));
      }

      for (uint32_t i = 0; i < SRSRAN_WIENER_DL_MIN_REF; i++) {
        q->RH.m[i][i] += N;
      }

      // Compute wiener correlation inverse matrix
      srsran_matrix_NxN_inv_run(q->matrix_inverter, q->RH.v, q->invRH.v);

      // Generate Rectangular Wiener
      for (uint32_t i = 0; i < SRSRAN_WIENER_DL_MIN_RE; i++) {
        for (uint32_t k = 0; k < SRSRAN_WIENER_DL_MIN_REF; k++) {
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
      for (uint32_t dim1 = 0; dim1 < SRSRAN_WIENER_DL_MIN_RE; dim1++) {
        for (uint32_t dim2 = 0; dim2 < SRSRAN_WIENER_DL_MIN_REF; dim2++) {
          q->wm1[dim1][dim2] = 0;
          q->wm2[dim1][dim2] = 0;
          for (int i = 0; i < SRSRAN_WIENER_DL_MIN_REF; i++) {
            q->wm1[dim1][dim2] += _cmul(q->hH1[dim1][i], q->invRH.m[i][dim2]);
            q->wm2[dim1][dim2] += _cmul(q->hH2[dim1][i], q->invRH.m[i][dim2]);
          }
        }
      }
      q->wm_computed = true;
    }
  }
}

int srsran_wiener_dl_run(srsran_wiener_dl_t* q,
                         uint32_t            tx,
                         uint32_t            rx,
                         uint32_t            m,
                         uint32_t            shift,
                         cf_t*               pilots,
                         cf_t*               estimated,
                         float               snr_lin)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q) {
    // m is based on 0, increase one;
    m++;

    // Get estimator state
    srsran_wiener_dl_state_t* state = q->state[tx][rx];

    // Process symbol
    switch (m) {
      case 1:
        q->ready = q->wm_computed;
      case 8:
        srsran_wiener_dl_run_symbol_1_8(q, state, pilots, snr_lin);
        break;
      case 2:
      case 9:
        srsran_wiener_dl_run_symbol_2_9(q, state);
        break;
      case 5:
      case 12:
        srsran_wiener_dl_run_symbol_5_12(q, state, pilots, tx, rx, shift, snr_lin);
        break;
      default:
          /* Do nothing */;
    }

    // Estimate
    srsran_vec_sub_ccc(state->tfifo[0], state->tfifo[1], q->tmp, q->nof_re);
    srsran_vec_sc_prod_cfc(q->tmp, state->deltan * state->invtpilotoff, q->tmp, q->nof_re);
    srsran_vec_sum_ccc(state->tfifo[1], q->tmp, estimated, q->nof_re);
    state->deltan += 1.0f;

    ret = SRSRAN_SUCCESS;
  }

  return ret;
}

void srsran_wiener_dl_free(srsran_wiener_dl_t* q)
{
  if (q) {
    for (uint32_t tx = 0; tx < SRSRAN_MAX_PORTS; tx++) {
      for (uint32_t rx = 0; rx < SRSRAN_MAX_PORTS; rx++) {
        if (q->state[tx][rx]) {
          srsran_wiener_dl_state_free(q->state[tx][rx]);
          q->state[tx][rx] = NULL;
        }
      }
    }

    if (q->tmp) {
      free(q->tmp);
    }

    if (q->random) {
      srsran_random_free(q->random);
    }

    srsran_dft_plan_free(&q->fft);
    srsran_dft_plan_free(&q->ifft);

    if (q->matrix_inverter) {
      srsran_matrix_NxN_inv_free(q->matrix_inverter);
      free(q->matrix_inverter);
    }
  }
}
