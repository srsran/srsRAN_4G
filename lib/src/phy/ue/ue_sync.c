/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#include "srslte/srslte.h"
#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "srslte/phy/ue/ue_sync.h"

#include "srslte/phy/io/filesource.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define MAX_TIME_OFFSET 128

#define TRACK_MAX_LOST 10
#define TRACK_FRAME_SIZE 32
#define FIND_NOF_AVG_FRAMES 4

#define PSS_OFFSET                                                                                                     \
  (q->sf_len / 2 + ((q->cell.frame_type == SRSLTE_FDD)                                                                 \
                        ? 0                                                                                            \
                        : ((SRSLTE_CP_NSYMB(q->cell.cp) - 3) * SRSLTE_SYMBOL_SZ(q->fft_size, q->cell.cp))))

#define DUMMY_BUFFER_NUM_SAMPLES (15 * 2048 / 2)
static cf_t  dummy_buffer0[DUMMY_BUFFER_NUM_SAMPLES];
static cf_t  dummy_buffer1[DUMMY_BUFFER_NUM_SAMPLES];
static cf_t* dummy_offset_buffer[SRSLTE_MAX_CHANNELS] = {dummy_buffer0, dummy_buffer1, dummy_buffer1, dummy_buffer1};

int srslte_ue_sync_init_file(srslte_ue_sync_t* q, uint32_t nof_prb, char* file_name, int offset_time, float offset_freq)
{
  return srslte_ue_sync_init_file_multi(q, nof_prb, file_name, offset_time, offset_freq, 1);
}

void srslte_ue_sync_file_wrap(srslte_ue_sync_t* q, bool enable)
{
  q->file_wrap_enable = enable;
}

int srslte_ue_sync_init_file_multi(srslte_ue_sync_t* q,
                                   uint32_t          nof_prb,
                                   char*             file_name,
                                   int               offset_time,
                                   float             offset_freq,
                                   uint32_t          nof_rx_ant)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && file_name != NULL && srslte_nofprb_isvalid(nof_prb)) {
    ret = SRSLTE_ERROR;
    bzero(q, sizeof(srslte_ue_sync_t));
    q->file_mode        = true;
    q->file_wrap_enable = true;
    q->sf_len           = SRSLTE_SF_LEN(srslte_symbol_sz(nof_prb));
    q->file_cfo         = -offset_freq;
    q->fft_size         = srslte_symbol_sz(nof_prb);
    q->nof_rx_antennas  = nof_rx_ant;

    q->cfo_correct_enable_find  = false;
    q->cfo_correct_enable_track = true;

    if (srslte_cfo_init(&q->file_cfo_correct, 2 * q->sf_len)) {
      ERROR("Error initiating CFO\n");
      goto clean_exit;
    }

    if (srslte_filesource_init(&q->file_source, file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
      ERROR("Error opening file %s\n", file_name);
      goto clean_exit;
    }

    INFO("Offseting input file by %d samples and %.1f kHz\n", offset_time, offset_freq / 1000);

    if (offset_time) {
      cf_t* file_offset_buffer = srslte_vec_cf_malloc(offset_time * nof_rx_ant);
      if (!file_offset_buffer) {
        perror("malloc");
        goto clean_exit;
      }
      srslte_filesource_read(&q->file_source, file_offset_buffer, offset_time * nof_rx_ant);
      free(file_offset_buffer);
    }

    srslte_ue_sync_cfo_reset(q, 0.0f);
    srslte_ue_sync_reset(q);

    ret = SRSLTE_SUCCESS;
  }
clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_ue_sync_free(q);
  }
  return ret;
}

void srslte_ue_sync_cfo_reset(srslte_ue_sync_t* q, float init_cfo_hz)
{
  q->cfo_is_copied     = false;
  q->cfo_current_value = init_cfo_hz / 15e3f;
  srslte_sync_cfo_reset(&q->strack, init_cfo_hz);
  srslte_sync_cfo_reset(&q->sfind, init_cfo_hz);
}

void srslte_ue_sync_reset(srslte_ue_sync_t* q)
{
  if (!q->file_mode && q->mode == SYNC_MODE_PSS) {
    srslte_sync_reset(&q->sfind);
    srslte_sync_reset(&q->strack);
  } else {
    q->sf_idx = 9;
  }
  q->pss_stable_timeout    = false;
  q->state                 = SF_FIND;
  q->frame_ok_cnt          = 0;
  q->frame_no_cnt          = 0;
  q->frame_total_cnt       = 0;
  q->mean_sample_offset    = 0.0;
  q->next_rf_sample_offset = 0;
  q->frame_find_cnt        = 0;
}

int srslte_ue_sync_start_agc(srslte_ue_sync_t* q,
                             SRSLTE_AGC_CALLBACK(set_gain_callback),
                             float min_gain_db,
                             float max_gain_db,
                             float init_gain_value_db)
{
  int n     = srslte_agc_init_uhd(&q->agc, SRSLTE_AGC_MODE_PEAK_AMPLITUDE, 0, set_gain_callback, q->stream);
  q->do_agc = n == 0 ? true : false;
  if (q->do_agc) {
    srslte_agc_set_gain_range(&q->agc, min_gain_db, max_gain_db);
    srslte_agc_set_gain(&q->agc, init_gain_value_db);
    srslte_ue_sync_set_agc_period(q, 4);
  }
  return n;
}

int recv_callback_multi_to_single(void* h, cf_t* x[SRSLTE_MAX_CHANNELS], uint32_t nsamples, srslte_timestamp_t* t)
{
  srslte_ue_sync_t* q = (srslte_ue_sync_t*)h;
  return q->recv_callback_single(q->stream_single, (void*)x[0], nsamples, t);
}

int srslte_ue_sync_init(srslte_ue_sync_t* q,
                        uint32_t          max_prb,
                        bool              search_cell,
                        int(recv_callback)(void*, void*, uint32_t, srslte_timestamp_t*),
                        void* stream_handler)
{
  int ret = srslte_ue_sync_init_multi(q, max_prb, search_cell, recv_callback_multi_to_single, 1, (void*)q);
  q->recv_callback_single = recv_callback;
  q->stream_single        = stream_handler;
  return ret;
}

int srslte_ue_sync_init_multi(srslte_ue_sync_t* q,
                              uint32_t          max_prb,
                              bool              search_cell,
                              int(recv_callback)(void*, cf_t* [SRSLTE_MAX_CHANNELS], uint32_t, srslte_timestamp_t*),
                              uint32_t nof_rx_antennas,
                              void*    stream_handler)

{
  return srslte_ue_sync_init_multi_decim(q, max_prb, search_cell, recv_callback, nof_rx_antennas, stream_handler, 1);
}

int srslte_ue_sync_init_multi_decim(
    srslte_ue_sync_t* q,
    uint32_t          max_prb,
    bool              search_cell,
    int(recv_callback)(void*, cf_t* [SRSLTE_MAX_CHANNELS], uint32_t, srslte_timestamp_t*),
    uint32_t nof_rx_antennas,
    void*    stream_handler,
    int      decimate)
{
  return srslte_ue_sync_init_multi_decim_mode(
      q, max_prb, search_cell, recv_callback, nof_rx_antennas, stream_handler, 1, SYNC_MODE_PSS);
}

int srslte_ue_sync_init_multi_decim_mode(
    srslte_ue_sync_t* q,
    uint32_t          max_prb,
    bool              search_cell,
    int(recv_callback)(void*, cf_t* [SRSLTE_MAX_CHANNELS], uint32_t, srslte_timestamp_t*),
    uint32_t              nof_rx_antennas,
    void*                 stream_handler,
    int                   decimate,
    srslte_ue_sync_mode_t mode)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && stream_handler != NULL && nof_rx_antennas <= SRSLTE_MAX_CHANNELS && recv_callback != NULL) {
    ret = SRSLTE_ERROR;
    // int decimate = q->decimate;
    bzero(q, sizeof(srslte_ue_sync_t));
    q->decimate                     = decimate;
    q->mode                         = mode;
    q->stream                       = stream_handler;
    q->recv_callback                = recv_callback;
    q->nof_rx_antennas              = nof_rx_antennas;
    q->fft_size                     = srslte_symbol_sz(max_prb);
    q->sf_len                       = SRSLTE_SF_LEN(q->fft_size);
    q->file_mode                    = false;
    q->agc_period                   = 0;
    q->sample_offset_correct_period = DEFAULT_SAMPLE_OFFSET_CORRECT_PERIOD;
    q->sfo_ema                      = DEFAULT_SFO_EMA_COEFF;

    q->max_prb = max_prb;

    q->cfo_ref_max     = DEFAULT_CFO_REF_MAX;
    q->cfo_ref_min     = DEFAULT_CFO_REF_MIN;
    q->cfo_pss_min     = DEFAULT_CFO_PSS_MIN;
    q->cfo_loop_bw_pss = DEFAULT_CFO_BW_PSS;
    q->cfo_loop_bw_ref = DEFAULT_CFO_BW_REF;

    q->cfo_correct_enable_find  = false;
    q->cfo_correct_enable_track = true;

    q->pss_stable_cnt     = 0;
    q->pss_stable_timeout = DEFAULT_PSS_STABLE_TIMEOUT;

    if (search_cell) {

      /* If the cell is unkown, we search PSS/SSS in 5 ms */
      q->nof_recv_sf = 5;

    } else {

      /* If the cell is known, we work on a 1ms basis */
      q->nof_recv_sf = 1;
    }

    q->frame_len = q->nof_recv_sf * q->sf_len;

    if (q->fft_size < 700 && q->decimate) {
      q->decimate = 1;
    }

    if (q->mode == SYNC_MODE_PSS) {
      if (srslte_sync_init_decim(&q->sfind, q->frame_len, q->frame_len, q->fft_size, q->decimate)) {
        ERROR("Error initiating sync find\n");
        goto clean_exit;
      }
      if (search_cell) {
        if (srslte_sync_init(&q->strack, q->frame_len, TRACK_FRAME_SIZE, q->fft_size)) {
          ERROR("Error initiating sync track\n");
          goto clean_exit;
        }
      } else {
        if (srslte_sync_init(&q->strack,
                             q->frame_len,
                             SRSLTE_MAX(TRACK_FRAME_SIZE, SRSLTE_CP_LEN_NORM(1, q->fft_size)),
                             q->fft_size)) {
          ERROR("Error initiating sync track\n");
          goto clean_exit;
        }
      }

      // Configure FIND and TRACK sync objects behaviour (this configuration is always the same)
      srslte_sync_set_cfo_i_enable(&q->sfind, false);
      srslte_sync_set_cfo_pss_enable(&q->sfind, true);
      srslte_sync_set_pss_filt_enable(&q->sfind, true);
      srslte_sync_set_sss_eq_enable(&q->sfind, false);

      // During track, we do CFO correction outside the sync object
      srslte_sync_set_cfo_i_enable(&q->strack, false);
      srslte_sync_set_cfo_pss_enable(&q->strack, true);
      srslte_sync_set_pss_filt_enable(&q->strack, true);
      srslte_sync_set_sss_eq_enable(&q->strack, false);

      // TODO: CP detection not working very well. Not supporting Extended CP right now
      srslte_sync_cp_en(&q->strack, false);
      srslte_sync_cp_en(&q->sfind, false);

      // Enable SSS on find and disable in track
      srslte_sync_sss_en(&q->sfind, true);
      srslte_sync_sss_en(&q->strack, false);
    }

    ret = SRSLTE_SUCCESS;
  }

clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_ue_sync_free(q);
  }
  return ret;
}

uint32_t srslte_ue_sync_sf_len(srslte_ue_sync_t* q)
{
  return q->frame_len;
}

void srslte_ue_sync_free(srslte_ue_sync_t* q)
{
  if (q->do_agc) {
    srslte_agc_free(&q->agc);
  }
  if (!q->file_mode && q->mode == SYNC_MODE_PSS) {
    srslte_sync_free(&q->sfind);
    srslte_sync_free(&q->strack);
  } else {
    srslte_filesource_free(&q->file_source);
  }
  bzero(q, sizeof(srslte_ue_sync_t));
}

int srslte_ue_sync_set_cell(srslte_ue_sync_t* q, srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_nofprb_isvalid(cell.nof_prb)) {
    if (cell.nof_prb > q->max_prb) {
      ERROR("Error in ue_sync_set_cell(): cell.nof_prb must be lower than initialized\n");
      return SRSLTE_ERROR;
    }

    q->cell     = cell;
    q->fft_size = srslte_symbol_sz(q->cell.nof_prb);
    q->sf_len   = SRSLTE_SF_LEN(q->fft_size);

    if (cell.id == 1000) {

      /* If the cell is unkown, we search PSS/SSS in 5 ms */
      q->nof_recv_sf = 5;
    } else {

      /* If the cell is known, we work on a 1ms basis */
      q->nof_recv_sf = 1;
    }

    q->frame_len = q->nof_recv_sf * q->sf_len;

    if (q->mode == SYNC_MODE_PSS) {
      // cell configuration for PSS-based sync
      if (q->fft_size < 700 && q->decimate) {
        q->decimate = 1;
      }

      if (srslte_sync_resize(&q->sfind, q->frame_len, q->frame_len, q->fft_size)) {
        ERROR("Error setting cell sync find\n");
        return SRSLTE_ERROR;
      }
      if (cell.id == 1000) {
        if (srslte_sync_resize(&q->strack, q->frame_len, TRACK_FRAME_SIZE, q->fft_size)) {
          ERROR("Error setting cell sync track\n");
          return SRSLTE_ERROR;
        }
      } else {
        if (srslte_sync_resize(&q->strack,
                               q->frame_len,
                               SRSLTE_MAX(TRACK_FRAME_SIZE, SRSLTE_CP_LEN_NORM(1, q->fft_size)),
                               q->fft_size)) {
          ERROR("Error setting cell sync track\n");
          return SRSLTE_ERROR;
        }
      }

      // When Cell ID is 1000, ue_sync receives nof_avg_find_frames frames in find state and does not go to tracking
      // state and is used to search a cell
      if (cell.id == 1000) {
        q->nof_avg_find_frames = FIND_NOF_AVG_FRAMES;

        srslte_sync_set_cfo_ema_alpha(&q->sfind, 0.8);
        srslte_sync_set_cfo_ema_alpha(&q->strack, 0.1);

        srslte_sync_set_em_alpha(&q->sfind, 1);

        srslte_sync_set_threshold(&q->sfind, 2.0);
        srslte_sync_set_threshold(&q->strack, 1.2);

        srslte_sync_set_cfo_ema_alpha(&q->sfind, 0.1);
        srslte_sync_set_cfo_ema_alpha(&q->strack, 0.1);

      } else {
        q->sfind.cp  = cell.cp;
        q->strack.cp = cell.cp;

        srslte_sync_set_frame_type(&q->sfind, cell.frame_type);
        srslte_sync_set_frame_type(&q->strack, cell.frame_type);

        srslte_sync_set_N_id_2(&q->sfind, cell.id % 3);
        srslte_sync_set_N_id_2(&q->strack, cell.id % 3);

        srslte_sync_set_N_id_1(&q->sfind, cell.id / 3);
        // track does not correlate SSS so no need to generate sequences

        srslte_sync_set_cfo_ema_alpha(&q->sfind, 0.1);
        srslte_sync_set_cfo_ema_alpha(&q->strack, DEFAULT_CFO_EMA_TRACK);

        /* In find phase and if the cell is known, do not average pss correlation
         * because we only capture 1 subframe and do not know where the peak is.
         */
        q->nof_avg_find_frames = 1;
        srslte_sync_set_em_alpha(&q->sfind, 1);
        srslte_sync_set_threshold(&q->sfind, 3.0);

        srslte_sync_set_em_alpha(&q->strack, 0.0);
        srslte_sync_set_threshold(&q->strack, 1.5);
      }

      // When cell is unknown, do CP CFO correction
      srslte_sync_set_cfo_cp_enable(&q->sfind, true, q->frame_len < 10000 ? 14 : 3);
      q->cfo_correct_enable_find = false;
    }

    srslte_ue_sync_reset(q);

    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

void srslte_ue_sync_set_nof_find_frames(srslte_ue_sync_t* q, uint32_t nof_frames)
{
  q->nof_avg_find_frames = nof_frames;
}

void srslte_ue_sync_set_frame_type(srslte_ue_sync_t* q, srslte_frame_type_t frame_type)
{
  srslte_sync_set_frame_type(&q->strack, frame_type);
  srslte_sync_set_frame_type(&q->sfind, frame_type);
}

srslte_frame_type_t srslte_ue_sync_get_frame_type(srslte_ue_sync_t* q)
{
  return q->sfind.frame_type;
}

void srslte_ue_sync_get_last_timestamp(srslte_ue_sync_t* q, srslte_timestamp_t* timestamp)
{
  *timestamp = q->last_timestamp;
}

void srslte_ue_sync_set_cfo_loop_bw(srslte_ue_sync_t* q,
                                    float             bw_pss,
                                    float             bw_ref,
                                    float             pss_tol,
                                    float             ref_tol,
                                    float             ref_max,
                                    uint32_t          pss_stable_conv_time)
{
  q->cfo_loop_bw_pss    = bw_pss;
  q->cfo_loop_bw_ref    = bw_ref;
  q->cfo_pss_min        = pss_tol;
  q->cfo_ref_min        = ref_tol;
  q->cfo_ref_max        = ref_max;
  q->pss_stable_timeout = pss_stable_conv_time;
}

void srslte_ue_sync_set_cfo_ema(srslte_ue_sync_t* q, float ema)
{
  srslte_sync_set_cfo_ema_alpha(&q->strack, ema);
}

void srslte_ue_sync_set_cfo_ref(srslte_ue_sync_t* q, float ref_cfo)
{
  // Accept REF-based CFO adjustments only after PSS CFO is stable
  if (q->pss_is_stable) {
    if (fabsf(ref_cfo) * 15000 > q->cfo_ref_min) {
      if (fabsf(ref_cfo) * 15000 > q->cfo_ref_max) {
        ref_cfo = (ref_cfo > 0 ? q->cfo_ref_max : -q->cfo_ref_max) / 15000;
      }
      q->cfo_current_value += ref_cfo * q->cfo_loop_bw_ref;
    }
  }
}

uint32_t srslte_ue_sync_get_sfn(srslte_ue_sync_t* q)
{
  return q->frame_number;
}

uint32_t srslte_ue_sync_get_sfidx(srslte_ue_sync_t* q)
{
  return q->sf_idx;
}

void srslte_ue_sync_set_cfo_i_enable(srslte_ue_sync_t* q, bool enable)
{
  printf("Warning: Setting integer CFO detection/correction. This is experimental!\n");
  srslte_sync_set_cfo_i_enable(&q->strack, enable);
  srslte_sync_set_cfo_i_enable(&q->sfind, enable);
}

float srslte_ue_sync_get_cfo(srslte_ue_sync_t* q)
{
  return 15000 * q->cfo_current_value;
}

void srslte_ue_sync_copy_cfo(srslte_ue_sync_t* q, srslte_ue_sync_t* src_obj)
{
  // Copy find object internal CFO averages
  srslte_sync_copy_cfo(&q->sfind, &src_obj->sfind);
  // Current CFO is tracking-phase CFO of previous object
  q->cfo_current_value = src_obj->cfo_current_value;
  q->cfo_is_copied     = true;
}

void srslte_ue_sync_set_cfo_tol(srslte_ue_sync_t* q, float cfo_tol)
{
  srslte_sync_set_cfo_tol(&q->strack, cfo_tol);
  srslte_sync_set_cfo_tol(&q->sfind, cfo_tol);
}

float srslte_ue_sync_get_sfo(srslte_ue_sync_t* q)
{
  return q->mean_sample_offset / 5e-3;
}

int srslte_ue_sync_get_last_sample_offset(srslte_ue_sync_t* q)
{
  return q->last_sample_offset;
}

void srslte_ue_sync_set_sfo_correct_period(srslte_ue_sync_t* q, uint32_t nof_subframes)
{
  q->sample_offset_correct_period = nof_subframes;
}

void srslte_ue_sync_set_sfo_ema(srslte_ue_sync_t* q, float ema_coefficient)
{
  q->sfo_ema = ema_coefficient;
}

void srslte_ue_sync_set_N_id_2(srslte_ue_sync_t* q, uint32_t N_id_2)
{
  if (!q->file_mode) {
    srslte_ue_sync_reset(q);
    srslte_sync_set_N_id_2(&q->strack, N_id_2);
    srslte_sync_set_N_id_2(&q->sfind, N_id_2);
  }
}

void srslte_ue_sync_set_agc_period(srslte_ue_sync_t* q, uint32_t period)
{
  q->agc_period = period;
}

static int find_peak_ok(srslte_ue_sync_t* q, cf_t* input_buffer[SRSLTE_MAX_CHANNELS])
{

  if (srslte_sync_sss_detected(&q->sfind)) {
    /* Get the subframe index (0 or 5) */
    q->sf_idx = (srslte_sync_get_sf_idx(&q->sfind) + q->nof_recv_sf) % 10;
  } else if (srslte_sync_sss_available(&q->sfind)) {
    INFO("Found peak at %d, SSS not detected\n", q->peak_idx);
    return 0;
  } else {
    INFO("Found peak at %d, No space for SSS. Realigning frame, reading %d samples\n", q->peak_idx, q->peak_idx);
    if (q->recv_callback(q->stream, input_buffer, q->peak_idx, &q->last_timestamp) < 0) {
      return SRSLTE_ERROR;
    }
    return 0;
  }

  q->frame_find_cnt++;
  DEBUG("Found peak %d at %d, value %.3f, Cell_id: %d CP: %s\n",
        q->frame_find_cnt,
        q->peak_idx,
        srslte_sync_get_peak_value(&q->sfind),
        q->cell.id,
        srslte_cp_string(q->cell.cp));

  if (q->frame_find_cnt >= q->nof_avg_find_frames || q->peak_idx < 2 * q->fft_size) {
    // Receive read_len samples until the start of the next subframe (different for FDD and TDD)
    uint32_t read_len = q->peak_idx + PSS_OFFSET;
    INFO("Realigning frame, reading %d samples\n", read_len);
    if (q->recv_callback(q->stream, input_buffer, read_len, &q->last_timestamp) < 0) {
      return SRSLTE_ERROR;
    }

    /* Reset variables */
    q->frame_ok_cnt       = 0;
    q->frame_no_cnt       = 0;
    q->frame_total_cnt    = 0;
    q->frame_find_cnt     = 0;
    q->mean_sample_offset = 0;

    /* Goto Tracking state if cell ID is known already */
    if (q->cell.id < 1000) {
      q->state = SF_TRACK;
    }

    /* Set CFO values. Since we correct before track, the initial track state is CFO=0 Hz */
    if (!q->cfo_is_copied) {
      q->cfo_current_value = srslte_sync_get_cfo(&q->sfind);
    }
    srslte_sync_cfo_reset(&q->strack, 0.0f);
  }

  if (q->cell.id < 1000) {
    return 0;
  } else {
    return 1;
  }
}

static int track_peak_ok(srslte_ue_sync_t* q, uint32_t track_idx)
{

  // Get sampling time offset
  q->last_sample_offset = ((int)track_idx - (int)q->strack.max_offset / 2 - (int)q->strack.fft_size);

  // Adjust sampling time every q->sample_offset_correct_period subframes
  uint32_t frame_idx = 0;
  if (q->sample_offset_correct_period) {
    frame_idx             = q->frame_ok_cnt % q->sample_offset_correct_period;
    q->mean_sample_offset = SRSLTE_VEC_EMA((float)q->last_sample_offset, q->mean_sample_offset, q->sfo_ema);
  } else {
    q->mean_sample_offset = q->last_sample_offset;
  }

  /* Adjust current CFO estimation with PSS
   * Since sync track has enabled only PSS-based correlation, get_cfo() returns that value only, already filtered.
   */
  DEBUG(
      "TRACK: cfo_current=%f, cfo_strack=%f\n", 15000 * q->cfo_current_value, 15000 * srslte_sync_get_cfo(&q->strack));
  if (15000 * fabsf(srslte_sync_get_cfo(&q->strack)) > q->cfo_pss_min) {
    q->cfo_current_value += srslte_sync_get_cfo(&q->strack) * q->cfo_loop_bw_pss;
    q->pss_stable_cnt = 0;
    q->pss_is_stable  = false;
  } else {
    if (!q->pss_is_stable) {
      q->pss_stable_cnt++;
      if (q->pss_stable_cnt >= q->pss_stable_timeout) {
        q->pss_is_stable = true;
      }
    }
  }

  // Compute cumulative moving average time offset */
  if (!frame_idx) {
    // Adjust RF sampling time based on the mean sampling offset
    q->next_rf_sample_offset = (int)round(q->mean_sample_offset);

    if (q->next_rf_sample_offset) {
      INFO("Time offset adjustment: %d samples (%.2f), mean SFO: %.2f Hz, ema=%f, length=%d\n",
           q->next_rf_sample_offset,
           q->mean_sample_offset,
           srslte_ue_sync_get_sfo(q),
           q->sfo_ema,
           q->sample_offset_correct_period);
    }
    q->mean_sample_offset = 0;
  }

  /* If the PSS peak is beyond the frame (we sample too slowly),
    discard the offseted samples to align next frame */
  if (q->next_rf_sample_offset > 0 && q->next_rf_sample_offset < MAX_TIME_OFFSET) {
    DEBUG("Positive time offset %d samples.\n", q->next_rf_sample_offset);
    if (q->recv_callback(q->stream, dummy_offset_buffer, (uint32_t)q->next_rf_sample_offset, NULL) < 0) {
      ERROR("Error receiving from USRP\n");
      return SRSLTE_ERROR;
    }
    q->next_rf_sample_offset = 0;
  }

  q->peak_idx = q->sf_len / 2 + q->last_sample_offset;
  q->frame_ok_cnt++;
  q->frame_no_cnt = 0;

  return 1;
}

static int track_peak_no(srslte_ue_sync_t* q)
{

  /* if we missed too many PSS go back to FIND and consider this frame unsynchronized */
  q->frame_no_cnt++;
  if (q->frame_no_cnt >= TRACK_MAX_LOST) {
    INFO("\n%d frames lost. Going back to FIND\n", (int)q->frame_no_cnt);
    q->state = SF_FIND;
    return 0;
  } else {
    INFO("Tracking peak not found. Peak %.3f, %d lost\n", srslte_sync_get_peak_value(&q->strack), (int)q->frame_no_cnt);
    return 1;
  }
}

static int receive_samples(srslte_ue_sync_t* q, cf_t* input_buffer[SRSLTE_MAX_CHANNELS], const uint32_t max_num_samples)
{
  ///< A negative time offset means there are samples in our buffer for the next subframe bc we are sampling too fast
  if (q->next_rf_sample_offset < 0) {
    q->next_rf_sample_offset = -q->next_rf_sample_offset;
  }

  ///< Make sure receive buffer is big enough
  if (q->frame_len - q->next_rf_sample_offset > max_num_samples) {
    fprintf(stderr, "Receive buffer too small (%d < %d)\n", max_num_samples, q->frame_len - q->next_rf_sample_offset);
    return SRSLTE_ERROR;
  }

  ///< Get N subframes from the USRP getting more samples and keeping the previous samples, if any
  cf_t* ptr[SRSLTE_MAX_CHANNELS] = {NULL};
  for (int i = 0; i < q->nof_rx_antennas; i++) {
    ptr[i] = &input_buffer[i][q->next_rf_sample_offset];
  }
  if (q->recv_callback(q->stream, ptr, q->frame_len - q->next_rf_sample_offset, &q->last_timestamp) < 0) {
    return SRSLTE_ERROR;
  }

  ///< reset time offset
  q->next_rf_sample_offset = 0;

  return SRSLTE_SUCCESS;
}

/* Returns 1 if the subframe is synchronized in time, 0 otherwise */
int srslte_ue_sync_zerocopy(srslte_ue_sync_t* q,
                            cf_t*             input_buffer[SRSLTE_MAX_CHANNELS],
                            const uint32_t    max_num_samples)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && input_buffer != NULL) {

    if (q->file_mode) {
      int n = srslte_filesource_read_multi(&q->file_source, (void**)input_buffer, q->sf_len, q->nof_rx_antennas);
      if (n < 0) {
        ERROR("Error reading input file\n");
        return SRSLTE_ERROR;
      }
      if (n == 0) {
        if (q->file_wrap_enable) {
          srslte_filesource_seek(&q->file_source, 0);
          q->sf_idx = 9;
          n = srslte_filesource_read_multi(&q->file_source, (void**)input_buffer, q->sf_len, q->nof_rx_antennas);
          if (n < 0) {
            ERROR("Error reading input file\n");
            return SRSLTE_ERROR;
          }
        } else {
          return SRSLTE_ERROR;
        }
      }
      if (q->cfo_correct_enable_track) {
        for (int i = 0; i < q->nof_rx_antennas; i++) {
          srslte_cfo_correct(&q->file_cfo_correct, input_buffer[i], input_buffer[i], q->file_cfo / 15000 / q->fft_size);
        }
      }
      q->sf_idx++;
      if (q->sf_idx == 10) {
        q->sf_idx = 0;
      }
      INFO("Reading %d samples. sf_idx = %d\n", q->sf_len, q->sf_idx);
      ret = 1;
    } else {

      if (receive_samples(q, input_buffer, max_num_samples)) {
        ERROR("Error receiving samples\n");
        return SRSLTE_ERROR;
      }

      switch (q->state) {
        case SF_FIND:
          // Correct CFO before PSS/SSS find using the sync object corrector (initialized for 1 ms)
          if (q->cfo_correct_enable_find) {
            for (int i = 0; i < q->nof_rx_antennas; i++) {
              if (input_buffer[i]) {
                srslte_cfo_correct(
                    &q->strack.cfo_corr_frame, input_buffer[i], input_buffer[i], -q->cfo_current_value / q->fft_size);
              }
            }
          }

          // Run mode-specific find operation
          if (q->mode == SYNC_MODE_PSS) {
            ret = srslte_ue_sync_run_find_pss_mode(q, input_buffer);
          } else if (q->mode == SYNC_MODE_GNSS) {
            ret = srslte_ue_sync_run_find_gnss_mode(q, input_buffer, max_num_samples);
          }

          if (q->do_agc) {
            srslte_agc_process(&q->agc, input_buffer[0], q->sf_len);
          }

          break;
        case SF_TRACK:

          ret = 1;

          // Increase subframe counter and system frame number
          q->sf_idx = (q->sf_idx + q->nof_recv_sf) % 10;
          if (q->sf_idx == 0) {
            q->frame_number = (q->frame_number + 1) % 1024;
          }

          // Correct CFO before PSS/SSS tracking using the sync object corrector (initialized for 1 ms)
          if (q->cfo_correct_enable_track) {
            for (int i = 0; i < q->nof_rx_antennas; i++) {
              if (input_buffer[i]) {
                srslte_cfo_correct(
                    &q->strack.cfo_corr_frame, input_buffer[i], input_buffer[i], -q->cfo_current_value / q->fft_size);
              }
            }
          }

          if (q->mode == SYNC_MODE_PSS) {
            srslte_ue_sync_run_track_pss_mode(q, input_buffer);
          } else {
            srslte_ue_sync_run_track_gnss_mode(q, input_buffer);
          }
          break;
        default:
          ERROR("Unknown sync state %d\n", q->state);
      }
    }
  }
  return ret;
}

int srslte_ue_sync_run_find_pss_mode(srslte_ue_sync_t* q, cf_t* input_buffer[SRSLTE_MAX_CHANNELS])
{
  int ret = SRSLTE_ERROR;
  int n   = srslte_sync_find(&q->sfind, input_buffer[0], 0, &q->peak_idx);

  switch (n) {
    case SRSLTE_SYNC_ERROR:
      ERROR("Error finding correlation peak (%d)\n", ret);
      return ret;
    case SRSLTE_SYNC_FOUND:
      ret = find_peak_ok(q, input_buffer);
      break;
    case SRSLTE_SYNC_FOUND_NOSPACE:
      /* If a peak was found but there is not enough space for SSS/CP detection, discard a few samples */
      INFO("No space for SSS/CP detection. Realigning frame...\n");
      q->recv_callback(q->stream, dummy_offset_buffer, q->frame_len / 2, NULL);
      srslte_sync_reset(&q->sfind);
      ret = SRSLTE_SUCCESS;
      break;
    default:
      ret = SRSLTE_SUCCESS;
      break;
  }

  INFO("SYNC FIND: sf_idx=%d, ret=%d, peak_pos=%d, peak_value=%.2f, mean_cp_cfo=%.2f, mean_pss_cfo=%.2f, "
       "total_cfo_khz=%.1f\n",
       q->sf_idx,
       ret,
       q->peak_idx,
       q->sfind.peak_value,
       q->sfind.cfo_cp_mean,
       q->sfind.cfo_pss_mean,
       15 * srslte_sync_get_cfo(&q->sfind));

  return ret;
};

int srslte_ue_sync_run_track_pss_mode(srslte_ue_sync_t* q, cf_t* input_buffer[SRSLTE_MAX_CHANNELS])
{
  int      ret       = SRSLTE_ERROR;
  uint32_t track_idx = 0;
  /* Every SF idx 0 and 5, find peak around known position q->peak_idx */
  if ((q->sfind.frame_type == SRSLTE_FDD && (q->sf_idx == 0 || q->sf_idx == 5)) ||
      (q->sfind.frame_type == SRSLTE_TDD && (q->sf_idx == 1 || q->sf_idx == 6))) {
    // Process AGC every period
    if (q->do_agc && (q->agc_period == 0 || (q->agc_period && (q->frame_total_cnt % q->agc_period) == 0))) {
      srslte_agc_process(&q->agc, input_buffer[0], q->sf_len);
    }

    /* Track PSS around the expected PSS position
     * In tracking phase, the subframe carrying the PSS is always the last one of the frame
     */

    // Expected PSS position is different for FDD and TDD
    uint32_t pss_idx = q->frame_len - PSS_OFFSET - q->fft_size - q->strack.max_offset / 2;

    int n = srslte_sync_find(&q->strack, input_buffer[0], pss_idx, &track_idx);
    switch (n) {
      case SRSLTE_SYNC_ERROR:
        ERROR("Error tracking correlation peak\n");
        return SRSLTE_ERROR;
      case SRSLTE_SYNC_FOUND:
        ret = track_peak_ok(q, track_idx);
        break;
      case SRSLTE_SYNC_FOUND_NOSPACE:
        // It's very very unlikely that we fall here because this event should happen at FIND phase only
        ret      = 0;
        q->state = SF_FIND;
        INFO("Warning: No space for SSS/CP while in tracking phase\n");
        break;
      case SRSLTE_SYNC_NOFOUND:
        ret = track_peak_no(q);
        break;
    }

    if (ret == SRSLTE_ERROR) {
      ERROR("Error processing tracking peak\n");
      q->state = SF_FIND;
      return SRSLTE_SUCCESS;
    }

    q->frame_total_cnt++;

    INFO("SYNC TRACK: sf_idx=%d, ret=%d, peak_pos=%d, peak_value=%.2f, mean_cp_cfo=%.2f, mean_pss_cfo=%.2f, "
         "total_cfo_khz=%.1f\n",
         q->sf_idx,
         ret,
         track_idx,
         q->strack.peak_value,
         q->strack.cfo_cp_mean,
         q->strack.cfo_pss_mean,
         15 * srslte_sync_get_cfo(&q->strack));
  } else {
    INFO("SYNC TRACK: sf_idx=%d, ret=%d, next_state=%d\n", q->sf_idx, ret, q->state);
  }

  return 1; ///< 1 means subframe in sync
}

int srslte_ue_sync_run_find_gnss_mode(srslte_ue_sync_t* q,
                                      cf_t*             input_buffer[SRSLTE_MAX_CHANNELS],
                                      const uint32_t    max_num_samples)
{
  INFO("Calibration samples received start at %ld + %f\n", q->last_timestamp.full_secs, q->last_timestamp.frac_secs);

  // round to nearest second
  srslte_timestamp_t ts_next_rx;
  srslte_timestamp_copy(&ts_next_rx, &q->last_timestamp);
  ts_next_rx.full_secs++;
  ts_next_rx.frac_secs = 0.0;

  INFO("Next desired recv at %ld + %f\n", ts_next_rx.full_secs, ts_next_rx.frac_secs);

  // get difference in time between second rx and now
  srslte_timestamp_sub(&ts_next_rx, q->last_timestamp.full_secs, q->last_timestamp.frac_secs);
  srslte_timestamp_sub(&ts_next_rx, 0, 0.001); ///< account for samples that have already been rx'ed

  uint64_t align_len = srslte_timestamp_uint64(&ts_next_rx, q->sf_len * 1000);

  DEBUG("Difference between first recv is %ld + %f, realigning %" PRIu64 " samples\n",
        ts_next_rx.full_secs,
        ts_next_rx.frac_secs,
        align_len);

  // receive align_len samples into dummy_buffer, make sure to not exceed buffer len
  uint32_t sample_count = 0;
  while (sample_count < align_len) {
    uint32_t actual_rx_len = SRSLTE_MIN(align_len, DUMMY_BUFFER_NUM_SAMPLES);
    actual_rx_len          = SRSLTE_MIN(align_len - sample_count, actual_rx_len);
    q->recv_callback(q->stream, dummy_offset_buffer, actual_rx_len, &q->last_timestamp);
    sample_count += actual_rx_len;
  }

  DEBUG("Received %d samples during alignment\n", sample_count);

  // do one normal receive, the first time-aligned subframe
  if (receive_samples(q, input_buffer, max_num_samples)) {
    ERROR("Error receiving samples\n");
    return SRSLTE_ERROR;
  }

  INFO("First aligned samples received start at %ld + %f\n", q->last_timestamp.full_secs, q->last_timestamp.frac_secs);

  // switch to track state, from here on, samples should be ms aligned
  q->state = SF_TRACK;

  // calculate system timing
  if (srslte_ue_sync_set_tti_from_timestamp(q, &q->last_timestamp)) {
    ERROR("Error deriving timing from received samples\n");
    return SRSLTE_ERROR;
  }

  INFO("SYNC FIND: sfn=%d, sf_idx=%d next_state=%d\n", q->frame_number, q->sf_idx, q->state);

  return 1; ///< 1 means subframe in sync
}

///< The track function in GNSS mode only needs to increment the system frame number
int srslte_ue_sync_run_track_gnss_mode(srslte_ue_sync_t* q, cf_t* input_buffer[SRSLTE_MAX_CHANNELS])
{
  INFO("TRACK samples received at %ld + %.4f\n", q->last_timestamp.full_secs, q->last_timestamp.frac_secs);

  // make sure the fractional receive time is ms-aligned
  uint32_t rx_full_ms  = floor(q->last_timestamp.frac_secs * 1e3);
  double   rx_frac_ms  = q->last_timestamp.frac_secs - (rx_full_ms / 1e3);
  int32_t  offset_samp = round(rx_frac_ms / (1.0 / (q->sf_len * 1000)));
  INFO("rx_full_ms=%d, rx_frac_ms=%f, offset_samp=%d\n", rx_full_ms, rx_frac_ms, offset_samp);
  if (offset_samp != q->sf_len) {
    q->next_rf_sample_offset = offset_samp;
  }

  if (q->next_rf_sample_offset) {
    INFO("Time offset adjustment: %d samples\n", q->next_rf_sample_offset);
  }

  // update SF index
  q->sf_idx = ((int)round(q->last_timestamp.frac_secs * 1e3)) % SRSLTE_NOF_SF_X_FRAME;

  INFO("SYNC TRACK: sfn=%d, sf_idx=%d, next_state=%d\n", q->frame_number, q->sf_idx, q->state);

  return 1; ///< 1 means subframe in sync
}

/** Calculate TTI for UEs that are synced using GNSS time reference (TS 36.331 Sec. 5.10.14)
 *
 * @param q Pointer to current object
 * @param rx_timestamp Pointer to receive timestamp
 * @return SRSLTE_SUCCESS on success
 */
int srslte_ue_sync_set_tti_from_timestamp(srslte_ue_sync_t* q, srslte_timestamp_t* rx_timestamp)
{
  // calculate time_t of Rx time
  time_t t_cur = rx_timestamp->full_secs;
  DEBUG("t_cur=%ld\n", t_cur);

  // time_t of reference UTC time on 1. Jan 1900 at 0:00
  // If we put this date in https://www.epochconverter.com it returns a negative number
  time_t t_ref = {0};
#if 0
  struct tm t = {0};
  t.tm_year = 1900; // year-1900
  t.tm_mday = 1;          // first of January
  // t.tm_isdst = 0;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
  t_ref = mktime(&t);
#endif

  DEBUG("t_ref=%ld\n", t_ref);

  static const uint32_t MSECS_PER_SEC = 1000;

  DEBUG("diff=%f\n", difftime(t_cur, t_ref));

  double time_diff_secs = difftime(t_cur, t_ref);

  if (time_diff_secs < 0) {
    fprintf(stderr, "Time diff between Rx timestamp and reference UTC is negative. Is the timestamp correct?\n");
    return SRSLTE_ERROR;
  }

  DEBUG("time diff in s %f\n", time_diff_secs);

  // convert to ms and add fractional part
  double time_diff_msecs = time_diff_secs * MSECS_PER_SEC + rx_timestamp->frac_secs;
  DEBUG("time diff in ms %f\n", time_diff_msecs);

  // calculate SFN and SF index according to TS 36.331 Sec. 5.10.14
  q->frame_number = ((uint32_t)floor(0.1 * (time_diff_msecs - q->sfn_offset))) % 1024;
  q->sf_idx       = ((uint32_t)floor(time_diff_msecs - q->sfn_offset)) % 10;

  return SRSLTE_SUCCESS;
}
