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

#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srsran/phy/sync/pss.h"
#include "srsran/phy/utils/debug.h"

int srsran_pss_init_N_id_2(cf_t* pss_signal_freq, cf_t* pss_signal_time, uint32_t N_id_2, uint32_t fft_size, int cfo_i)
{
  srsran_dft_plan_t plan;
  cf_t              pss_signal_pad[2048];
  int               ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (srsran_N_id_2_isvalid(N_id_2) && fft_size <= 2048) {
    srsran_pss_generate(pss_signal_freq, N_id_2);

    srsran_vec_cf_zero(pss_signal_pad, fft_size);
    srsran_vec_cf_zero(pss_signal_time, fft_size);
    memcpy(&pss_signal_pad[(fft_size - SRSRAN_PSS_LEN) / 2 + cfo_i], pss_signal_freq, SRSRAN_PSS_LEN * sizeof(cf_t));

    /* Convert signal into the time domain */
    if (srsran_dft_plan(&plan, fft_size, SRSRAN_DFT_BACKWARD, SRSRAN_DFT_COMPLEX)) {
      return SRSRAN_ERROR;
    }

    srsran_dft_plan_set_mirror(&plan, true);
    srsran_dft_plan_set_dc(&plan, true);
    srsran_dft_plan_set_norm(&plan, true);
    srsran_dft_run_c(&plan, pss_signal_pad, pss_signal_time);

    srsran_vec_conj_cc(pss_signal_time, pss_signal_time, fft_size);
    srsran_vec_sc_prod_cfc(pss_signal_time, 1.0 / SRSRAN_PSS_LEN, pss_signal_time, fft_size);

    srsran_dft_plan_free(&plan);

    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

/* Initializes the PSS synchronization object with fft_size=128
 */
int srsran_pss_init(srsran_pss_t* q, uint32_t frame_size)
{
  return srsran_pss_init_fft(q, frame_size, 128);
}

int srsran_pss_init_fft(srsran_pss_t* q, uint32_t frame_size, uint32_t fft_size)
{
  return srsran_pss_init_fft_offset(q, frame_size, fft_size, 0);
}

int srsran_pss_init_fft_offset(srsran_pss_t* q, uint32_t frame_size, uint32_t fft_size, int offset)
{
  return srsran_pss_init_fft_offset_decim(q, frame_size, fft_size, offset, 1);
}

/* Initializes the PSS synchronization object.
 *
 * It correlates a signal of frame_size samples with the PSS sequence in the frequency
 * domain. The PSS sequence is transformed using fft_size samples.
 */
int srsran_pss_init_fft_offset_decim(srsran_pss_t* q,
                                     uint32_t      max_frame_size,
                                     uint32_t      max_fft_size,
                                     int           offset,
                                     int           decimate)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    ret = SRSRAN_ERROR;

    uint32_t N_id_2;
    uint32_t buffer_size;
    bzero(q, sizeof(srsran_pss_t));

    q->N_id_2    = 10;
    q->ema_alpha = 0.2;

    q->max_fft_size   = max_fft_size;
    q->max_frame_size = max_frame_size;

    q->decimate         = decimate;
    uint32_t fft_size   = max_fft_size / q->decimate;
    uint32_t frame_size = max_frame_size / q->decimate;

    q->fft_size   = fft_size;
    q->frame_size = frame_size;

    buffer_size = fft_size + frame_size + 1;

    q->filter_pss_enable = false;
    q->chest_on_filter   = false;

    if (q->decimate > 1) {
      int filter_order = 3;
      srsran_filt_decim_cc_init(&q->filter, q->decimate, filter_order);
      q->filter.filter_output     = srsran_vec_cf_malloc(buffer_size);
      q->filter.downsampled_input = srsran_vec_cf_malloc(buffer_size + filter_order);
      printf("decimation for the  PSS search is %d \n", q->decimate);
    }

    if (srsran_dft_plan(&q->dftp_input, fft_size, SRSRAN_DFT_FORWARD, SRSRAN_DFT_COMPLEX)) {
      ERROR("Error creating DFT plan ");
      goto clean_and_exit;
    }
    srsran_dft_plan_set_mirror(&q->dftp_input, true);
    srsran_dft_plan_set_dc(&q->dftp_input, true);
    srsran_dft_plan_set_norm(&q->dftp_input, false);

    if (srsran_dft_plan(&q->idftp_input, fft_size, SRSRAN_DFT_BACKWARD, SRSRAN_DFT_COMPLEX)) {
      ERROR("Error creating DFT plan ");
      goto clean_and_exit;
    }
    srsran_dft_plan_set_mirror(&q->idftp_input, true);
    srsran_dft_plan_set_dc(&q->idftp_input, true);
    srsran_dft_plan_set_norm(&q->idftp_input, false);

    srsran_vec_cf_zero(q->tmp_fft2, SRSRAN_SYMBOL_SZ_MAX);

    q->tmp_input = srsran_vec_cf_malloc(buffer_size + frame_size * (q->decimate - 1));
    if (!q->tmp_input) {
      ERROR("Error allocating memory");
      goto clean_and_exit;
    }

    srsran_vec_cf_zero(&q->tmp_input[q->frame_size], q->fft_size);

    q->conv_output = srsran_vec_cf_malloc(buffer_size);
    if (!q->conv_output) {
      ERROR("Error allocating memory");
      goto clean_and_exit;
    }
    srsran_vec_cf_zero(q->conv_output, buffer_size);
    q->conv_output_avg = srsran_vec_f_malloc(buffer_size);
    if (!q->conv_output_avg) {
      ERROR("Error allocating memory");
      goto clean_and_exit;
    }
    srsran_vec_f_zero(q->conv_output_avg, buffer_size);
#ifdef SRSRAN_PSS_ACCUMULATE_ABS
    q->conv_output_abs = srsran_vec_f_malloc(buffer_size);
    if (!q->conv_output_abs) {
      ERROR("Error allocating memory");
      goto clean_and_exit;
    }
    srsran_vec_f_zero(q->conv_output_abs, buffer_size);
#endif

    for (N_id_2 = 0; N_id_2 < 3; N_id_2++) {
      q->pss_signal_time[N_id_2] = srsran_vec_cf_malloc(buffer_size);
      if (!q->pss_signal_time[N_id_2]) {
        ERROR("Error allocating memory");
        goto clean_and_exit;
      }
      /* The PSS is translated into the time domain for each N_id_2  */
      if (srsran_pss_init_N_id_2(q->pss_signal_freq[N_id_2], q->pss_signal_time[N_id_2], N_id_2, fft_size, offset)) {
        ERROR("Error initiating PSS detector for N_id_2=%d fft_size=%d", N_id_2, fft_size);
        goto clean_and_exit;
      }
      srsran_vec_cf_zero(&q->pss_signal_time[N_id_2][q->fft_size], q->frame_size);
    }
#ifdef CONVOLUTION_FFT

    if (srsran_conv_fft_cc_init(&q->conv_fft, frame_size, fft_size)) {
      ERROR("Error initiating convolution FFT");
      goto clean_and_exit;
    }
    for (N_id_2 = 0; N_id_2 < 3; N_id_2++) {
      q->pss_signal_freq_full[N_id_2] = srsran_vec_cf_malloc(buffer_size);
      srsran_dft_run_c(&q->conv_fft.filter_plan, q->pss_signal_time[N_id_2], q->pss_signal_freq_full[N_id_2]);
    }

#endif

    srsran_pss_reset(q);

    ret = SRSRAN_SUCCESS;
  }

clean_and_exit:
  if (ret == SRSRAN_ERROR) {
    srsran_pss_free(q);
  }
  return ret;
}

/* Initializes the PSS synchronization object.
 *
 * It correlates a signal of frame_size samples with the PSS sequence in the frequency
 * domain. The PSS sequence is transformed using fft_size samples.
 */
int srsran_pss_resize(srsran_pss_t* q, uint32_t frame_size, uint32_t fft_size, int offset)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    ret = SRSRAN_ERROR;

    if (fft_size > q->max_fft_size || frame_size > q->max_frame_size) {
      ERROR("Error in pss_config(): fft_size and frame_size must be lower than initialized");
      return SRSRAN_ERROR;
    }

    uint32_t N_id_2;
    uint32_t buffer_size;

    q->N_id_2    = 10;
    q->ema_alpha = 0.2;

    fft_size   = fft_size / q->decimate;
    frame_size = frame_size / q->decimate;

    q->fft_size   = fft_size;
    q->frame_size = frame_size;

    buffer_size = fft_size + frame_size + 1;

    if (srsran_dft_replan(&q->dftp_input, fft_size)) {
      ERROR("Error creating DFT plan ");
      return SRSRAN_ERROR;
    }

    if (srsran_dft_replan(&q->idftp_input, fft_size)) {
      ERROR("Error creating DFT plan ");
      return SRSRAN_ERROR;
    }

    srsran_vec_cf_zero(q->tmp_fft2, SRSRAN_SYMBOL_SZ_MAX);

    srsran_vec_cf_zero(&q->tmp_input[q->frame_size], q->fft_size);
    srsran_vec_cf_zero(q->conv_output, buffer_size);
    srsran_vec_f_zero(q->conv_output_avg, buffer_size);

#ifdef SRSRAN_PSS_ACCUMULATE_ABS
    srsran_vec_f_zero(q->conv_output_abs, buffer_size);
#endif

    // Generate PSS sequences for this FFT size
    for (N_id_2 = 0; N_id_2 < 3; N_id_2++) {
      if (srsran_pss_init_N_id_2(q->pss_signal_freq[N_id_2], q->pss_signal_time[N_id_2], N_id_2, fft_size, offset)) {
        ERROR("Error initiating PSS detector for N_id_2=%d fft_size=%d", N_id_2, fft_size);
        return SRSRAN_ERROR;
      }
      srsran_vec_cf_zero(&q->pss_signal_time[N_id_2][q->fft_size], q->frame_size);
    }
#ifdef CONVOLUTION_FFT

    if (srsran_conv_fft_cc_replan(&q->conv_fft, frame_size, fft_size)) {
      ERROR("Error initiating convolution FFT");
      return SRSRAN_ERROR;
    }
    for (int i = 0; i < 3; i++) {
      srsran_dft_run_c(&q->conv_fft.filter_plan, q->pss_signal_time[i], q->pss_signal_freq_full[i]);
    }

#endif

    srsran_pss_reset(q);

    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

void srsran_pss_free(srsran_pss_t* q)
{
  uint32_t i;

  if (q) {
    for (i = 0; i < 3; i++) {
      if (q->pss_signal_time[i]) {
        free(q->pss_signal_time[i]);
      }
      if (q->pss_signal_freq_full[i]) {
        free(q->pss_signal_freq_full[i]);
      }
    }
#ifdef CONVOLUTION_FFT
    srsran_conv_fft_cc_free(&q->conv_fft);

#endif
    if (q->tmp_input) {
      free(q->tmp_input);
    }
    if (q->conv_output) {
      free(q->conv_output);
    }
    if (q->conv_output_abs) {
      free(q->conv_output_abs);
    }
    if (q->conv_output_avg) {
      free(q->conv_output_avg);
    }

    srsran_dft_plan_free(&q->dftp_input);
    srsran_dft_plan_free(&q->idftp_input);

    if (q->decimate > 1) {
      srsran_filt_decim_cc_free(&q->filter);
      free(q->filter.filter_output);
      free(q->filter.downsampled_input);
    }

    bzero(q, sizeof(srsran_pss_t));
  }
}

void srsran_pss_reset(srsran_pss_t* q)
{
  uint32_t buffer_size = q->fft_size + q->frame_size + 1;
  srsran_vec_f_zero(q->conv_output_avg, buffer_size);
}

/**
 * This function calculates the Zadoff-Chu sequence.
 * @param signal Output array.
 */
int srsran_pss_generate(cf_t* signal, uint32_t N_id_2)
{
  int         i;
  float       arg;
  const float root_value[] = {25.0, 29.0, 34.0};
  int         root_idx;

  int sign = -1;

  if (N_id_2 > 2) {
    ERROR("Invalid N_id_2 %d", N_id_2);
    return -1;
  }

  root_idx = N_id_2;

  for (i = 0; i < SRSRAN_PSS_LEN / 2; i++) {
    arg                = (float)sign * M_PI * root_value[root_idx] * ((float)i * ((float)i + 1.0)) / 63.0;
    __real__ signal[i] = cosf(arg);
    __imag__ signal[i] = sinf(arg);
  }
  for (i = SRSRAN_PSS_LEN / 2; i < SRSRAN_PSS_LEN; i++) {
    arg                = (float)sign * M_PI * root_value[root_idx] * (((float)i + 2.0) * ((float)i + 1.0)) / 63.0;
    __real__ signal[i] = cosf(arg);
    __imag__ signal[i] = sinf(arg);
  }
  return 0;
}

/** 36.211 10.3 section 6.11.1.2
 */
void srsran_pss_put_slot(cf_t* pss_signal, cf_t* slot, uint32_t nof_prb, srsran_cp_t cp)
{
  int k;
  k = (SRSRAN_CP_NSYMB(cp) - 1) * nof_prb * SRSRAN_NRE + nof_prb * SRSRAN_NRE / 2 - 31;
  memset(&slot[k - 5], 0, 5 * sizeof(cf_t));
  memcpy(&slot[k], pss_signal, SRSRAN_PSS_LEN * sizeof(cf_t));
  memset(&slot[k + SRSRAN_PSS_LEN], 0, 5 * sizeof(cf_t));
}

void srsran_pss_get_slot(cf_t* slot, cf_t* pss_signal, uint32_t nof_prb, srsran_cp_t cp)
{
  int k;
  k = (SRSRAN_CP_NSYMB(cp) - 1) * nof_prb * SRSRAN_NRE + nof_prb * SRSRAN_NRE / 2 - 31;
  memcpy(pss_signal, &slot[k], SRSRAN_PSS_LEN * sizeof(cf_t));
}

/** Sets the current N_id_2 value. Returns -1 on ERROR(0 otherwise
 */
int srsran_pss_set_N_id_2(srsran_pss_t* q, uint32_t N_id_2)
{
  if (!srsran_N_id_2_isvalid((N_id_2))) {
    ERROR("Invalid N_id_2 %d", N_id_2);
    return -1;
  } else {
    q->N_id_2 = N_id_2;
    return 0;
  }
}

/* Sets the weight factor alpha for the exponential moving average of the PSS correlation output
 */
void srsran_pss_set_ema_alpha(srsran_pss_t* q, float alpha)
{
  q->ema_alpha = alpha;
}

float compute_peak_sidelobe(srsran_pss_t* q, uint32_t corr_peak_pos, uint32_t conv_output_len)
{
  // Find end of peak lobe to the right
  int pl_ub = corr_peak_pos + 1;
  while (q->conv_output_avg[pl_ub + 1] <= q->conv_output_avg[pl_ub] && pl_ub < conv_output_len) {
    pl_ub++;
  }
  // Find end of peak lobe to the left
  int pl_lb;
  if (corr_peak_pos > 2) {
    pl_lb = corr_peak_pos - 1;
    while (q->conv_output_avg[pl_lb - 1] <= q->conv_output_avg[pl_lb] && pl_lb > 1) {
      pl_lb--;
    }
  } else {
    pl_lb = 0;
  }

  int sl_distance_right = conv_output_len - 1 - pl_ub;
  if (sl_distance_right < 0) {
    sl_distance_right = 0;
  }
  int sl_distance_left = pl_lb;

  int   sl_right        = pl_ub + srsran_vec_max_fi(&q->conv_output_avg[pl_ub], sl_distance_right);
  int   sl_left         = srsran_vec_max_fi(q->conv_output_avg, sl_distance_left);
  float side_lobe_value = SRSRAN_MAX(q->conv_output_avg[sl_right], q->conv_output_avg[sl_left]);

  return q->conv_output_avg[corr_peak_pos] / side_lobe_value;
}

/** Performs time-domain PSS correlation.
 * Returns the index of the PSS correlation peak in a subframe.
 * The frame starts at corr_peak_pos-subframe_size/2.
 * The value of the correlation is stored in corr_peak_value.
 *
 * Input buffer must be subframe_size long.
 */
int srsran_pss_find_pss(srsran_pss_t* q, const cf_t* input, float* corr_peak_value)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && input != NULL) {
    uint32_t corr_peak_pos;
    uint32_t conv_output_len;

    if (!srsran_N_id_2_isvalid(q->N_id_2)) {
      ERROR("Error finding PSS peak, Must set N_id_2 first");
      return SRSRAN_ERROR;
    }

    /* Correlate input with PSS sequence
     *
     * We do not reverse time-domain PSS signal because it's conjugate is symmetric.
     * The conjugate operation on pss_signal_time has been done in srsran_pss_init_N_id_2
     * This is why we can use FFT-based convolution
     */
    if (q->frame_size >= q->fft_size) {
#ifdef CONVOLUTION_FFT
      memcpy(q->tmp_input, input, (q->frame_size * q->decimate) * sizeof(cf_t));
      if (q->decimate > 1) {
        srsran_filt_decim_cc_execute(&(q->filter),
                                     q->tmp_input,
                                     q->filter.downsampled_input,
                                     q->filter.filter_output,
                                     (q->frame_size * q->decimate));
        conv_output_len = srsran_conv_fft_cc_run_opt(
            &q->conv_fft, q->filter.filter_output, q->pss_signal_freq_full[q->N_id_2], q->conv_output);
      } else {
        conv_output_len =
            srsran_conv_fft_cc_run_opt(&q->conv_fft, q->tmp_input, q->pss_signal_freq_full[q->N_id_2], q->conv_output);
      }

#else
      conv_output_len =
          srsran_conv_cc(input, q->pss_signal_time[q->N_id_2], q->conv_output, q->frame_size, q->fft_size);
#endif
    } else {
      for (int i = 0; i < q->frame_size; i++) {
        q->conv_output[i] = srsran_vec_dot_prod_ccc(q->pss_signal_time[q->N_id_2], &input[i], q->fft_size);
      }
      conv_output_len = q->frame_size;
    }

    // Compute modulus square
    srsran_vec_abs_square_cf(q->conv_output, q->conv_output_abs, conv_output_len - 1);

    // If enabled, average the absolute value from previous calls
    if (q->ema_alpha < 1.0 && q->ema_alpha > 0.0) {
      srsran_vec_sc_prod_fff(q->conv_output_abs, q->ema_alpha, q->conv_output_abs, conv_output_len - 1);
      srsran_vec_sc_prod_fff(q->conv_output_avg, 1 - q->ema_alpha, q->conv_output_avg, conv_output_len - 1);

      srsran_vec_sum_fff(q->conv_output_abs, q->conv_output_avg, q->conv_output_avg, conv_output_len - 1);
    } else {
      memcpy(q->conv_output_avg, q->conv_output_abs, sizeof(float) * (conv_output_len - 1));
    }

    /* Find maximum of the absolute value of the correlation */
    corr_peak_pos = srsran_vec_max_fi(q->conv_output_avg, conv_output_len - 1);

    // save absolute value
    q->peak_value = q->conv_output_avg[corr_peak_pos];

#ifdef SRSRAN_PSS_RETURN_PSR
    if (corr_peak_value) {
      *corr_peak_value = compute_peak_sidelobe(q, corr_peak_pos, conv_output_len);
    }
#else
    if (corr_peak_value) {
      *corr_peak_value = q->conv_output_avg[corr_peak_pos];
    }
#endif

    if (q->decimate > 1) {
      int decimation_correction = (q->filter.num_taps - 2);
      corr_peak_pos             = corr_peak_pos - decimation_correction;
      corr_peak_pos             = corr_peak_pos * q->decimate;
    }

    if (q->frame_size >= q->fft_size) {
      ret = (int)corr_peak_pos;
    } else {
      ret = (int)corr_peak_pos + q->fft_size;
    }
  }
  return ret;
}

/* Computes frequency-domain channel estimation of the PSS symbol
 * input signal is in the time-domain.
 * ce is the returned frequency-domain channel estimates.
 */
int srsran_pss_chest(srsran_pss_t* q, const cf_t* input, cf_t ce[SRSRAN_PSS_LEN])
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && input != NULL) {
    if (!srsran_N_id_2_isvalid(q->N_id_2)) {
      ERROR("Error finding PSS peak, Must set N_id_2 first");
      return SRSRAN_ERROR;
    }

    /* Transform to frequency-domain */
    srsran_dft_run_c(&q->dftp_input, input, q->tmp_fft);

    /* Compute channel estimate taking the PSS sequence as reference */
    srsran_vec_prod_conj_ccc(
        &q->tmp_fft[(q->fft_size - SRSRAN_PSS_LEN) / 2], q->pss_signal_freq[q->N_id_2], ce, SRSRAN_PSS_LEN);

    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

/* input points to beginning of last OFDM symbol of slot 0 of subframe 0 or 5
 * It must be called after calling srsran_pss_cfo_compute() with filter enabled
 */
void srsran_pss_sic(srsran_pss_t* q, cf_t* input)
{
  if (q->chest_on_filter) {
    srsran_vec_cf_zero(q->tmp_fft, q->fft_size);

    // Pass transmitted PSS sequence through the channel
    srsran_vec_prod_ccc(
        q->pss_signal_freq[q->N_id_2], q->tmp_ce, &q->tmp_fft[(q->fft_size - SRSRAN_PSS_LEN) / 2], SRSRAN_PSS_LEN);

    // Get time-domain version of the received PSS
    srsran_dft_run_c(&q->idftp_input, q->tmp_fft, q->tmp_fft2);

    // Substract received PSS from this N_id_2 from the input signal
    srsran_vec_sc_prod_cfc(q->tmp_fft2, 1.0 / q->fft_size, q->tmp_fft2, q->fft_size);
    srsran_vec_sub_ccc(input, q->tmp_fft2, input, q->fft_size);

  } else {
    ERROR("Error calling srsran_pss_sic(): need to enable channel estimation on filtering");
  }
}

// Frequency-domain filtering of the central 64 sub-carriers
void srsran_pss_filter(srsran_pss_t* q, const cf_t* input, cf_t* output)
{
  srsran_dft_run_c(&q->dftp_input, input, q->tmp_fft);

  memcpy(&q->tmp_fft2[q->fft_size / 2 - SRSRAN_PSS_LEN / 2],
         &q->tmp_fft[q->fft_size / 2 - SRSRAN_PSS_LEN / 2],
         sizeof(cf_t) * SRSRAN_PSS_LEN);

  if (q->chest_on_filter) {
    srsran_vec_prod_conj_ccc(
        &q->tmp_fft[(q->fft_size - SRSRAN_PSS_LEN) / 2], q->pss_signal_freq[q->N_id_2], q->tmp_ce, SRSRAN_PSS_LEN);
  }

  srsran_dft_run_c(&q->idftp_input, q->tmp_fft2, output);
}

/* Returns the CFO estimation given a PSS received sequence
 *
 * Source: An Efﬁcient CFO Estimation Algorithm for the Downlink of 3GPP-LTE
 *       Feng Wang and Yu Zhu
 */
float srsran_pss_cfo_compute(srsran_pss_t* q, const cf_t* pss_recv)
{
  cf_t y0, y1;

  const cf_t* pss_ptr = pss_recv;

  if (q->filter_pss_enable) {
    srsran_pss_filter(q, pss_recv, q->tmp_fft);
    pss_ptr = (const cf_t*)q->tmp_fft;
  }

  y0 = srsran_vec_dot_prod_ccc(q->pss_signal_time[q->N_id_2], pss_ptr, q->fft_size / 2);
  y1 = srsran_vec_dot_prod_ccc(
      &q->pss_signal_time[q->N_id_2][q->fft_size / 2], &pss_ptr[q->fft_size / 2], q->fft_size / 2);
  return cargf(conjf(y0) * y1) / M_PI;
}
