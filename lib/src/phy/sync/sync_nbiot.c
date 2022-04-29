/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
#include <strings.h>

#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/sync/cfo.h"
#include "srsran/phy/sync/sync_nbiot.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#define CFO_EMA_ALPHA 0.1
#define CP_EMA_ALPHA 0.1
#define DEFAULT_CFO_TOL 50.0 // Hz

/* We use the default LTE synch object internally for all the generic
 * functions like CFO correction, etc.
 *
 */
int srsran_sync_nbiot_init(srsran_sync_nbiot_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSRAN_ERROR;

    q->n_id_ncell            = SRSRAN_CELL_ID_UNKNOWN;
    q->mean_cfo              = 0;
    q->cfo_ema_alpha         = CFO_EMA_ALPHA;
    q->fft_size              = fft_size;
    q->frame_size            = frame_size;
    q->max_frame_size        = frame_size;
    q->max_offset            = max_offset;
    q->threshold             = 5.0;
    q->enable_cfo_estimation = true;

    if (srsran_cfo_init(&q->cfocorr, q->frame_size)) {
      fprintf(stderr, "Error initiating CFO\n");
      goto clean_exit;
    }

    // Set default CFO tolerance
    srsran_sync_nbiot_set_cfo_tol(q, DEFAULT_CFO_TOL);

    // initialize shift buffer for CFO estimation
    q->shift_buffer = srsran_vec_cf_malloc(SRSRAN_SF_LEN(q->fft_size));
    if (!q->shift_buffer) {
      perror("malloc");
      goto clean_exit;
    }
    srsran_cexptab_gen_sf(q->shift_buffer, -SRSRAN_NBIOT_FREQ_SHIFT_FACTOR, q->fft_size);

    // allocate memory for early CFO estimation
    q->cfo_output = srsran_vec_cf_malloc(SRSRAN_NOF_SF_X_FRAME * SRSRAN_SF_LEN(q->fft_size));
    if (!q->cfo_output) {
      perror("malloc");
      goto clean_exit;
    }

    // configure CP
    q->cp     = SRSRAN_CP_NORM;
    q->cp_len = SRSRAN_CP_LEN_NORM(1, q->fft_size);
    if (q->frame_size < q->fft_size) {
      q->nof_symbols = 1;
    } else {
      q->nof_symbols = q->frame_size / (q->fft_size + q->cp_len) - 1;
    }

    if (srsran_npss_synch_init(&q->npss, frame_size, fft_size)) {
      fprintf(stderr, "Error initializing NPSS object\n");
      return SRSRAN_ERROR;
    }

    if (srsran_nsss_synch_init(&q->nsss, SRSRAN_NSSS_NUM_SF_DETECT * SRSRAN_SF_LEN_PRB_NBIOT, fft_size)) {
      fprintf(stderr, "Error initializing NSSS object\n");
      exit(-1);
    }

    if (srsran_cp_synch_init(&q->cp_synch, fft_size)) {
      fprintf(stderr, "Error initiating CFO\n");
      goto clean_exit;
    }

    ret = SRSRAN_SUCCESS;
  }

clean_exit:
  if (ret == SRSRAN_ERROR) {
    srsran_sync_nbiot_free(q);
  }
  return ret;
}

void srsran_sync_nbiot_free(srsran_sync_nbiot_t* q)
{
  if (q) {
    srsran_npss_synch_free(&q->npss);
    srsran_nsss_synch_free(&q->nsss);
    srsran_cfo_free(&q->cfocorr);
    srsran_cp_synch_free(&q->cp_synch);
    if (q->shift_buffer) {
      free(q->shift_buffer);
    }
    if (q->cfo_output) {
      free(q->cfo_output);
    }
  }
}

int srsran_sync_nbiot_resize(srsran_sync_nbiot_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && frame_size <= 307200) {
    ret = SRSRAN_ERROR;

    if (frame_size > q->max_frame_size) {
      fprintf(stderr, "Error in srsran_sync_nbiot_resize(): frame_size must be lower than initialized\n");
      return ret;
    }
    q->mean_cfo             = 0;
    q->cfo_i                = 0;
    q->find_cfo_i           = false;
    q->find_cfo_i_initiated = false;
    q->cfo_ema_alpha        = CFO_EMA_ALPHA;
    q->fft_size             = fft_size;
    q->frame_size           = frame_size;
    q->max_offset           = max_offset;

    if (srsran_npss_synch_resize(&q->npss, max_offset, fft_size)) {
      fprintf(stderr, "Error resizing PSS object\n");
      return ret;
    }
    if (srsran_nsss_synch_resize(&q->nsss, fft_size)) {
      fprintf(stderr, "Error resizing SSS object\n");
      return ret;
    }

    if (srsran_cp_synch_resize(&q->cp_synch, fft_size)) {
      fprintf(stderr, "Error resizing CFO\n");
      return ret;
    }

    if (srsran_cfo_resize(&q->cfocorr, q->frame_size)) {
      fprintf(stderr, "Error resizing CFO\n");
      return ret;
    }

    // Update CFO tolerance
    srsran_sync_nbiot_set_cfo_tol(q, q->current_cfo_tol);

    DEBUG("NBIOT SYNC init with frame_size=%d, max_offset=%d and fft_size=%d", frame_size, max_offset, fft_size);

    ret = SRSRAN_SUCCESS;
  } else {
    fprintf(stderr, "Invalid parameters frame_size: %d, fft_size: %d\n", frame_size, fft_size);
  }

  return ret;
}

/** Finds the NPSS sequence around the position find_offset in the buffer input.
 * Returns 1 if the correlation peak exceeds the threshold set by srsran_sync_set_threshold()
 * or 0 otherwise. Returns a negative number on error.
 *
 * The maximum of the correlation peak is always stored in *peak_position
 */
srsran_sync_find_ret_t
srsran_sync_nbiot_find(srsran_sync_nbiot_t* q, cf_t* input, uint32_t find_offset, uint32_t* peak_position)
{
  srsran_sync_find_ret_t ret = SRSRAN_SYNC_NOFOUND;

  int peak_pos = 0;
  if (peak_position) {
    *peak_position = 0;
  }

  // Retrieve CFO from a set of candidates
  if (q->enable_cfo_cand_test) {
    q->mean_cfo     = q->cfo_cand[q->cfo_cand_idx] / 15000;
    q->cfo_cand_idx = (q->cfo_cand_idx + 1) % q->cfo_num_cand;
  }

  // correct CFO using current estimate, store result in seperate buffer for NPSS detection
  srsran_cfo_correct(&q->cfocorr, input, q->cfo_output, -q->mean_cfo / q->fft_size);

  peak_pos = srsran_npss_sync_find(&q->npss, &q->cfo_output[find_offset], &q->peak_value);
  if (peak_pos < 0) {
    fprintf(stderr, "Error calling finding NPSS sequence, peak pos: %d\n", peak_pos);
    return SRSRAN_ERROR;
  }

  if (peak_position) {
    *peak_position = (uint32_t)peak_pos;
  }

  /* If peak is over threshold return success */
  if (q->peak_value >= q->threshold) {
    ret = SRSRAN_SYNC_FOUND;
  }

  // estimate CFO after NPSS has been detected
  if (q->enable_cfo_estimation) {
    // check if there are enough samples left
    if (peak_pos + SRSRAN_NPSS_CFO_OFFSET + SRSRAN_NPSS_CFO_NUM_SAMPS + SRSRAN_NBIOT_FFT_SIZE < q->frame_size) {
      // shift input signal
      srsran_vec_prod_ccc(&q->shift_buffer[SRSRAN_SF_LEN(q->fft_size) / 2],
                          &input[peak_pos + SRSRAN_NPSS_CFO_OFFSET],
                          &input[peak_pos + SRSRAN_NPSS_CFO_OFFSET],
                          SRSRAN_NPSS_CFO_NUM_SAMPS);

      // use second slot of the NPSS for CFO estimation
      float cfo = cfo_estimate_nbiot(q, &input[peak_pos + SRSRAN_NPSS_CFO_OFFSET]);

      // compute exponential moving average CFO
      q->mean_cfo = SRSRAN_VEC_EMA(cfo, q->mean_cfo, q->cfo_ema_alpha);
      DEBUG("CFO=%.4f, mean=%.4f (%.2f Hz), ema=%.2f", cfo, q->mean_cfo, q->mean_cfo * 15000, q->cfo_ema_alpha);
    } else {
      DEBUG("Not enough samples for CFO estimation. Skipping.");
    }
  }

  DEBUG("sync_nbiot ret=%d find_offset=%d frame_len=%d, pos=%d peak=%.2f threshold=%.2f, CFO=%.3f kHz",
        ret,
        find_offset,
        q->frame_size,
        peak_pos,
        q->peak_value,
        q->threshold,
        15 * (q->mean_cfo));

  return ret;
}

// Use two OFDM symbols to estimate CFO
float cfo_estimate_nbiot(srsran_sync_nbiot_t* q, cf_t* input)
{
  uint32_t cp_offset = 0;
  cp_offset =
      srsran_cp_synch(&q->cp_synch, input, q->max_offset, SRSRAN_NPSS_CFO_NUM_SYMS, SRSRAN_CP_LEN_NORM(1, q->fft_size));
  cf_t  cp_corr_max = srsran_cp_synch_corr_output(&q->cp_synch, cp_offset);
  float cfo         = -cargf(cp_corr_max) / M_PI / 2;
  return cfo;
}

void srsran_sync_nbiot_set_threshold(srsran_sync_nbiot_t* q, float threshold)
{
  q->threshold = threshold;
}

void srsran_sync_nbiot_set_cfo_enable(srsran_sync_nbiot_t* q, bool enable)
{
  q->enable_cfo_estimation = enable;
}

void srsran_sync_nbiot_set_cfo_cand_test_enable(srsran_sync_nbiot_t* q, bool enable)
{
  q->enable_cfo_cand_test = enable;
}

int srsran_sync_nbiot_set_cfo_cand(srsran_sync_nbiot_t* q, const float* cand, const int num)
{
  if (num > MAX_NUM_CFO_CANDITATES) {
    printf("Too many candidates, maximum is %d.\n", MAX_NUM_CFO_CANDITATES);
    return SRSRAN_ERROR;
  }
  for (int i = 0; i < num; i++) {
    q->cfo_cand[i] = cand[i];
  }
  q->cfo_num_cand = num;
  return SRSRAN_SUCCESS;
}

void srsran_sync_nbiot_set_cfo_tol(srsran_sync_nbiot_t* q, float tol)
{
  srsran_cfo_set_tol(&q->cfocorr, tol / (15000.0 * q->fft_size));
}

void srsran_sync_nbiot_set_cfo_ema_alpha(srsran_sync_nbiot_t* q, float alpha)
{
  q->cfo_ema_alpha = alpha;
}

void srsran_sync_nbiot_set_npss_ema_alpha(srsran_sync_nbiot_t* q, float alpha)
{
  srsran_npss_synch_set_ema_alpha(&q->npss, alpha);
}

/** Determines the N_id_ncell using the samples in the buffer input.
 * The function expects two subframes of samples provided as input which
 * both contain subframe 9 of two consecutive frames. Either the first
 * or the seconds contain the NSSS sequence.
 *
 * Returns 1 if the correlation peak exceeds the threshold or 0 otherwise.
 * Returns a negative number on error.
 *
 */
int srsran_sync_nbiot_find_cell_id(srsran_sync_nbiot_t* q, cf_t* input)
{
  int      ret = SRSRAN_ERROR_INVALID_INPUTS;
  float    peak_value;
  uint32_t sfn_partial;

  if (q != NULL && input != NULL && q->frame_size == SRSRAN_SF_LEN_PRB_NBIOT) {
    ret = srsran_nsss_sync_find(&q->nsss, input, &peak_value, &q->n_id_ncell, &sfn_partial);
    printf("NSSS with peak=%f, cell-id: %d, partial SFN: %x\n", peak_value, q->n_id_ncell, sfn_partial);
  }
  return ret;
}

int srsran_sync_nbiot_get_cell_id(srsran_sync_nbiot_t* q)
{
  return q->n_id_ncell;
}

float srsran_sync_nbiot_get_cfo(srsran_sync_nbiot_t* q)
{
  return q->mean_cfo + q->cfo_i;
}

void srsran_sync_nbiot_set_cfo(srsran_sync_nbiot_t* q, float cfo)
{
  q->mean_cfo = cfo;
}

float srsran_sync_nbiot_get_peak_value(srsran_sync_nbiot_t* q)
{
  return q->peak_value;
}

void srsran_sync_nbiot_reset(srsran_sync_nbiot_t* q)
{
  srsran_npss_synch_reset(&q->npss);
}
