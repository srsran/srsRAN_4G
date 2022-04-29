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

#include "srsran/srsran.h"
#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "srsran/phy/ue/ue_sync.h"

#include "srsran/phy/io/filesource.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#define MAX_TIME_OFFSET 128

#define TRACK_MAX_LOST 10
#define TRACK_FRAME_SIZE 32
#define FIND_NOF_AVG_FRAMES 4

#define PSS_OFFSET                                                                                                     \
  (q->sf_len / 2 + ((q->cell.frame_type == SRSRAN_FDD)                                                                 \
                        ? 0                                                                                            \
                        : ((SRSRAN_CP_NSYMB(q->cell.cp) - 3) * SRSRAN_SYMBOL_SZ(q->fft_size, q->cell.cp))))

#define DUMMY_BUFFER_NUM_SAMPLES (15 * 2048 / 2)
static cf_t  dummy_buffer0[DUMMY_BUFFER_NUM_SAMPLES];
static cf_t  dummy_buffer1[DUMMY_BUFFER_NUM_SAMPLES];
static cf_t* dummy_offset_buffer[SRSRAN_MAX_CHANNELS] = {dummy_buffer0, dummy_buffer1, dummy_buffer1, dummy_buffer1};

int srsran_ue_sync_init_file(srsran_ue_sync_t* q, uint32_t nof_prb, char* file_name, int offset_time, float offset_freq)
{
  return srsran_ue_sync_init_file_multi(q, nof_prb, file_name, offset_time, offset_freq, 1);
}

void srsran_ue_sync_file_wrap(srsran_ue_sync_t* q, bool enable)
{
  q->file_wrap_enable = enable;
}

int srsran_ue_sync_init_file_multi(srsran_ue_sync_t* q,
                                   uint32_t          nof_prb,
                                   char*             file_name,
                                   int               offset_time,
                                   float             offset_freq,
                                   uint32_t          nof_rx_ant)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && file_name != NULL && srsran_nofprb_isvalid(nof_prb)) {
    ret = SRSRAN_ERROR;
    bzero(q, sizeof(srsran_ue_sync_t));
    q->file_mode        = true;
    q->file_wrap_enable = true;
    q->sf_len           = SRSRAN_SF_LEN(srsran_symbol_sz(nof_prb));
    q->file_cfo         = -offset_freq;
    q->fft_size         = srsran_symbol_sz(nof_prb);
    q->nof_rx_antennas  = nof_rx_ant;

    q->cfo_correct_enable_find  = false;
    q->cfo_correct_enable_track = true;

    if (srsran_cfo_init(&q->file_cfo_correct, 2 * q->sf_len)) {
      ERROR("Error initiating CFO");
      goto clean_exit;
    }

    if (srsran_filesource_init(&q->file_source, file_name, SRSRAN_COMPLEX_FLOAT_BIN)) {
      ERROR("Error opening file %s", file_name);
      goto clean_exit;
    }

    INFO("Offseting input file by %d samples and %.1f kHz", offset_time, offset_freq / 1000);

    if (offset_time) {
      cf_t* file_offset_buffer = srsran_vec_cf_malloc(offset_time * nof_rx_ant);
      if (!file_offset_buffer) {
        perror("malloc");
        goto clean_exit;
      }
      srsran_filesource_read(&q->file_source, file_offset_buffer, offset_time * nof_rx_ant);
      free(file_offset_buffer);
    }

    srsran_ue_sync_cfo_reset(q, 0.0f);
    srsran_ue_sync_reset(q);

    ret = SRSRAN_SUCCESS;
  }
clean_exit:
  if (ret == SRSRAN_ERROR) {
    srsran_ue_sync_free(q);
  }
  return ret;
}

void srsran_ue_sync_cfo_reset(srsran_ue_sync_t* q, float init_cfo_hz)
{
  q->cfo_is_copied     = false;
  q->cfo_current_value = init_cfo_hz / 15e3f;
  srsran_sync_cfo_reset(&q->strack, init_cfo_hz);
  srsran_sync_cfo_reset(&q->sfind, init_cfo_hz);
}

void srsran_ue_sync_reset(srsran_ue_sync_t* q)
{
  if (!q->file_mode && q->mode == SYNC_MODE_PSS) {
    srsran_sync_reset(&q->sfind);
    srsran_sync_reset(&q->strack);
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

int srsran_ue_sync_start_agc(srsran_ue_sync_t* q,
                             SRSRAN_AGC_CALLBACK(set_gain_callback),
                             float min_gain_db,
                             float max_gain_db,
                             float init_gain_value_db)
{
  int n     = srsran_agc_init_uhd(&q->agc, SRSRAN_AGC_MODE_PEAK_AMPLITUDE, 0, set_gain_callback, q->stream);
  q->do_agc = n == 0 ? true : false;
  if (q->do_agc) {
    srsran_agc_set_gain_range(&q->agc, min_gain_db, max_gain_db);
    srsran_agc_set_gain(&q->agc, init_gain_value_db);
    srsran_ue_sync_set_agc_period(q, 4);
  }
  return n;
}

int recv_callback_multi_to_single(void* h, cf_t* x[SRSRAN_MAX_CHANNELS], uint32_t nsamples, srsran_timestamp_t* t)
{
  srsran_ue_sync_t* q = (srsran_ue_sync_t*)h;
  return q->recv_callback_single(q->stream_single, (void*)x[0], nsamples, t);
}

int srsran_ue_sync_init(srsran_ue_sync_t* q,
                        uint32_t          max_prb,
                        bool              search_cell,
                        int(recv_callback)(void*, void*, uint32_t, srsran_timestamp_t*),
                        void* stream_handler)
{
  int ret = srsran_ue_sync_init_multi(q, max_prb, search_cell, recv_callback_multi_to_single, 1, (void*)q);
  q->recv_callback_single = recv_callback;
  q->stream_single        = stream_handler;
  return ret;
}

int srsran_ue_sync_init_multi(srsran_ue_sync_t* q,
                              uint32_t          max_prb,
                              bool              search_cell,
                              int(recv_callback)(void*, cf_t* [SRSRAN_MAX_CHANNELS], uint32_t, srsran_timestamp_t*),
                              uint32_t nof_rx_antennas,
                              void*    stream_handler)

{
  return srsran_ue_sync_init_multi_decim(q, max_prb, search_cell, recv_callback, nof_rx_antennas, stream_handler, 1);
}

int srsran_ue_sync_init_multi_decim(
    srsran_ue_sync_t* q,
    uint32_t          max_prb,
    bool              search_cell,
    int(recv_callback)(void*, cf_t* [SRSRAN_MAX_CHANNELS], uint32_t, srsran_timestamp_t*),
    uint32_t nof_rx_antennas,
    void*    stream_handler,
    int      decimate)
{
  return srsran_ue_sync_init_multi_decim_mode(
      q, max_prb, search_cell, recv_callback, nof_rx_antennas, stream_handler, 1, SYNC_MODE_PSS);
}

int srsran_ue_sync_init_multi_decim_mode(
    srsran_ue_sync_t* q,
    uint32_t          max_prb,
    bool              search_cell,
    int(recv_callback)(void*, cf_t* [SRSRAN_MAX_CHANNELS], uint32_t, srsran_timestamp_t*),
    uint32_t              nof_rx_antennas,
    void*                 stream_handler,
    int                   decimate,
    srsran_ue_sync_mode_t mode)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && stream_handler != NULL && nof_rx_antennas <= SRSRAN_MAX_CHANNELS && recv_callback != NULL) {
    ret = SRSRAN_ERROR;
    // int decimate = q->decimate;
    bzero(q, sizeof(srsran_ue_sync_t));
    q->decimate                     = decimate;
    q->mode                         = mode;
    q->stream                       = stream_handler;
    q->recv_callback                = recv_callback;
    q->nof_rx_antennas              = nof_rx_antennas;
    q->fft_size                     = srsran_symbol_sz(max_prb);
    q->sf_len                       = SRSRAN_SF_LEN(q->fft_size);
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
      if (srsran_sync_init_decim(&q->sfind, q->frame_len, q->frame_len, q->fft_size, q->decimate)) {
        ERROR("Error initiating sync find");
        goto clean_exit;
      }
      if (search_cell) {
        if (srsran_sync_init(&q->strack, q->frame_len, TRACK_FRAME_SIZE, q->fft_size)) {
          ERROR("Error initiating sync track");
          goto clean_exit;
        }
      } else {
        if (srsran_sync_init(&q->strack,
                             q->frame_len,
                             SRSRAN_MAX(TRACK_FRAME_SIZE, SRSRAN_CP_LEN_NORM(1, q->fft_size)),
                             q->fft_size)) {
          ERROR("Error initiating sync track");
          goto clean_exit;
        }
      }

      // Configure FIND and TRACK sync objects behaviour (this configuration is always the same)
      srsran_sync_set_cfo_i_enable(&q->sfind, false);
      srsran_sync_set_cfo_pss_enable(&q->sfind, true);
      srsran_sync_set_pss_filt_enable(&q->sfind, true);
      srsran_sync_set_sss_eq_enable(&q->sfind, false);

      // During track, we do CFO correction outside the sync object
      srsran_sync_set_cfo_i_enable(&q->strack, false);
      srsran_sync_set_cfo_pss_enable(&q->strack, true);
      srsran_sync_set_pss_filt_enable(&q->strack, true);
      srsran_sync_set_sss_eq_enable(&q->strack, false);

      // TODO: CP detection not working very well. Not supporting Extended CP right now
      srsran_sync_cp_en(&q->strack, false);
      srsran_sync_cp_en(&q->sfind, false);

      // Enable SSS on find and disable in track
      srsran_sync_sss_en(&q->sfind, true);
      srsran_sync_sss_en(&q->strack, false);
    }

    ret = SRSRAN_SUCCESS;
  }

clean_exit:
  if (ret == SRSRAN_ERROR) {
    srsran_ue_sync_free(q);
  }
  return ret;
}

uint32_t srsran_ue_sync_sf_len(srsran_ue_sync_t* q)
{
  return q->frame_len;
}

void srsran_ue_sync_free(srsran_ue_sync_t* q)
{
  if (q->do_agc) {
    srsran_agc_free(&q->agc);
  }
  if (!q->file_mode && q->mode == SYNC_MODE_PSS) {
    srsran_sync_free(&q->sfind);
    srsran_sync_free(&q->strack);
  } else {
    srsran_filesource_free(&q->file_source);
  }
  bzero(q, sizeof(srsran_ue_sync_t));
}

int srsran_ue_sync_set_cell(srsran_ue_sync_t* q, srsran_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && srsran_nofprb_isvalid(cell.nof_prb)) {
    if (cell.nof_prb > q->max_prb) {
      ERROR("Error in ue_sync_set_cell(): cell.nof_prb must be lower than initialized");
      return SRSRAN_ERROR;
    }

    q->cell     = cell;
    q->fft_size = srsran_symbol_sz(q->cell.nof_prb);
    q->sf_len   = SRSRAN_SF_LEN(q->fft_size);

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

      if (srsran_sync_resize(&q->sfind, q->frame_len, q->frame_len, q->fft_size)) {
        ERROR("Error setting cell sync find");
        return SRSRAN_ERROR;
      }
      if (cell.id == 1000) {
        if (srsran_sync_resize(&q->strack, q->frame_len, TRACK_FRAME_SIZE, q->fft_size)) {
          ERROR("Error setting cell sync track");
          return SRSRAN_ERROR;
        }
      } else {
        if (srsran_sync_resize(&q->strack,
                               q->frame_len,
                               SRSRAN_MAX(TRACK_FRAME_SIZE, SRSRAN_CP_LEN_NORM(1, q->fft_size)),
                               q->fft_size)) {
          ERROR("Error setting cell sync track");
          return SRSRAN_ERROR;
        }
      }

      // Set CP for find and track objects
      srsran_sync_set_cp(&q->sfind, cell.cp);
      srsran_sync_set_cp(&q->strack, cell.cp);

      // When Cell ID is 1000, ue_sync receives nof_avg_find_frames frames in find state and does not go to tracking
      // state and is used to search a cell
      if (cell.id == 1000) {
        q->nof_avg_find_frames = FIND_NOF_AVG_FRAMES;

        srsran_sync_set_cfo_ema_alpha(&q->sfind, 0.8);
        srsran_sync_set_cfo_ema_alpha(&q->strack, 0.1);

        srsran_sync_set_em_alpha(&q->sfind, 1);

        srsran_sync_set_threshold(&q->sfind, 2.0);
        srsran_sync_set_threshold(&q->strack, 1.2);

        srsran_sync_set_cfo_ema_alpha(&q->sfind, 0.1);
        srsran_sync_set_cfo_ema_alpha(&q->strack, 0.1);

      } else {
        srsran_sync_set_frame_type(&q->sfind, cell.frame_type);
        srsran_sync_set_frame_type(&q->strack, cell.frame_type);

        srsran_sync_set_N_id_2(&q->sfind, cell.id % 3);
        srsran_sync_set_N_id_2(&q->strack, cell.id % 3);

        srsran_sync_set_N_id_1(&q->sfind, cell.id / 3);
        // track does not correlate SSS so no need to generate sequences

        srsran_sync_set_cfo_ema_alpha(&q->sfind, 0.1);
        srsran_sync_set_cfo_ema_alpha(&q->strack, DEFAULT_CFO_EMA_TRACK);

        /* In find phase and if the cell is known, do not average pss correlation
         * because we only capture 1 subframe and do not know where the peak is.
         */
        q->nof_avg_find_frames = 1;
        srsran_sync_set_em_alpha(&q->sfind, 1);
        srsran_sync_set_threshold(&q->sfind, 3.0);

        srsran_sync_set_em_alpha(&q->strack, 0.0);
        srsran_sync_set_threshold(&q->strack, 1.5);
      }

      // When cell is unknown, do CP CFO correction
      srsran_sync_set_cfo_cp_enable(&q->sfind, true, q->frame_len < 10000 ? 14 : 3);
      q->cfo_correct_enable_find = false;
    }

    srsran_ue_sync_reset(q);

    ret = SRSRAN_SUCCESS;
  }

  return ret;
}

void srsran_ue_sync_set_nof_find_frames(srsran_ue_sync_t* q, uint32_t nof_frames)
{
  q->nof_avg_find_frames = nof_frames;
}

void srsran_ue_sync_set_frame_type(srsran_ue_sync_t* q, srsran_frame_type_t frame_type)
{
  srsran_sync_set_frame_type(&q->strack, frame_type);
  srsran_sync_set_frame_type(&q->sfind, frame_type);
}

srsran_frame_type_t srsran_ue_sync_get_frame_type(srsran_ue_sync_t* q)
{
  return q->sfind.frame_type;
}

void srsran_ue_sync_get_last_timestamp(srsran_ue_sync_t* q, srsran_timestamp_t* timestamp)
{
  *timestamp = q->last_timestamp;
}

void srsran_ue_sync_set_cfo_loop_bw(srsran_ue_sync_t* q,
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

void srsran_ue_sync_set_cfo_ema(srsran_ue_sync_t* q, float ema)
{
  srsran_sync_set_cfo_ema_alpha(&q->strack, ema);
}

void srsran_ue_sync_set_cfo_ref(srsran_ue_sync_t* q, float ref_cfo)
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

uint32_t srsran_ue_sync_get_sfn(srsran_ue_sync_t* q)
{
  return q->frame_number;
}

uint32_t srsran_ue_sync_get_sfidx(srsran_ue_sync_t* q)
{
  return q->sf_idx;
}

void srsran_ue_sync_set_cfo_i_enable(srsran_ue_sync_t* q, bool enable)
{
  printf("Warning: Setting integer CFO detection/correction. This is experimental!\n");
  srsran_sync_set_cfo_i_enable(&q->strack, enable);
  srsran_sync_set_cfo_i_enable(&q->sfind, enable);
}

float srsran_ue_sync_get_cfo(srsran_ue_sync_t* q)
{
  return 15000 * q->cfo_current_value;
}

void srsran_ue_sync_cp_en(srsran_ue_sync_t* q, bool enabled)
{
  srsran_sync_cp_en(&q->strack, enabled);
  srsran_sync_cp_en(&q->sfind, enabled);
}

void srsran_ue_sync_copy_cfo(srsran_ue_sync_t* q, srsran_ue_sync_t* src_obj)
{
  // Copy find object internal CFO averages
  srsran_sync_copy_cfo(&q->sfind, &src_obj->sfind);
  // Current CFO is tracking-phase CFO of previous object
  q->cfo_current_value = src_obj->cfo_current_value;
  q->cfo_is_copied     = true;
}

void srsran_ue_sync_set_cfo_tol(srsran_ue_sync_t* q, float cfo_tol)
{
  srsran_sync_set_cfo_tol(&q->strack, cfo_tol);
  srsran_sync_set_cfo_tol(&q->sfind, cfo_tol);
}

float srsran_ue_sync_get_sfo(srsran_ue_sync_t* q)
{
  return q->mean_sample_offset / 5e-3;
}

int srsran_ue_sync_get_last_sample_offset(srsran_ue_sync_t* q)
{
  return q->last_sample_offset;
}

void srsran_ue_sync_set_sfo_correct_period(srsran_ue_sync_t* q, uint32_t nof_subframes)
{
  q->sample_offset_correct_period = nof_subframes;
}

void srsran_ue_sync_set_sfo_ema(srsran_ue_sync_t* q, float ema_coefficient)
{
  q->sfo_ema = ema_coefficient;
}

void srsran_ue_sync_set_N_id_2(srsran_ue_sync_t* q, uint32_t N_id_2)
{
  if (!q->file_mode) {
    srsran_ue_sync_reset(q);
    srsran_sync_set_N_id_2(&q->strack, N_id_2);
    srsran_sync_set_N_id_2(&q->sfind, N_id_2);
  }
}

void srsran_ue_sync_set_agc_period(srsran_ue_sync_t* q, uint32_t period)
{
  q->agc_period = period;
}

static int find_peak_ok(srsran_ue_sync_t* q, cf_t* input_buffer[SRSRAN_MAX_CHANNELS])
{
  if (srsran_sync_sss_detected(&q->sfind)) {
    /* Get the subframe index (0 or 5) */
    q->sf_idx = (srsran_sync_get_sf_idx(&q->sfind) + q->nof_recv_sf) % 10;
  } else if (srsran_sync_sss_available(&q->sfind)) {
    INFO("Found peak at %d, SSS not detected", q->peak_idx);
    return 0;
  } else {
    INFO("Found peak at %d, No space for SSS. Realigning frame, reading %d samples", q->peak_idx, q->peak_idx);
    if (q->recv_callback(q->stream, input_buffer, q->peak_idx, &q->last_timestamp) < 0) {
      return SRSRAN_ERROR;
    }
    return 0;
  }

  q->frame_find_cnt++;
  DEBUG("Found peak %d at %d, value %.3f, Cell_id: %d CP: %s",
        q->frame_find_cnt,
        q->peak_idx,
        srsran_sync_get_peak_value(&q->sfind),
        q->cell.id,
        srsran_cp_string(q->cell.cp));

  if (q->frame_find_cnt >= q->nof_avg_find_frames || q->peak_idx < 2 * q->fft_size) {
    // Receive read_len samples until the start of the next subframe (different for FDD and TDD)
    uint32_t read_len = q->peak_idx + PSS_OFFSET;
    INFO("Realigning frame, reading %d samples", read_len);
    if (q->recv_callback(q->stream, input_buffer, read_len, &q->last_timestamp) < 0) {
      return SRSRAN_ERROR;
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
      q->cfo_current_value = srsran_sync_get_cfo(&q->sfind);
    }
    srsran_sync_cfo_reset(&q->strack, 0.0f);
  }

  if (q->cell.id < 1000) {
    return 0;
  } else {
    return 1;
  }
}

static int track_peak_ok(srsran_ue_sync_t* q, uint32_t track_idx)
{
  // Get sampling time offset
  q->last_sample_offset = ((int)track_idx - (int)q->strack.max_offset / 2 - (int)q->strack.fft_size);

  // Adjust sampling time every q->sample_offset_correct_period subframes
  uint32_t frame_idx = 0;
  if (q->sample_offset_correct_period) {
    frame_idx             = q->frame_ok_cnt % q->sample_offset_correct_period;
    q->mean_sample_offset = SRSRAN_VEC_EMA((float)q->last_sample_offset, q->mean_sample_offset, q->sfo_ema);
  } else {
    q->mean_sample_offset = q->last_sample_offset;
  }

  /* Adjust current CFO estimation with PSS
   * Since sync track has enabled only PSS-based correlation, get_cfo() returns that value only, already filtered.
   */
  DEBUG("TRACK: cfo_current=%f, cfo_strack=%f", 15000 * q->cfo_current_value, 15000 * srsran_sync_get_cfo(&q->strack));
  if (15000 * fabsf(srsran_sync_get_cfo(&q->strack)) > q->cfo_pss_min) {
    q->cfo_current_value += srsran_sync_get_cfo(&q->strack) * q->cfo_loop_bw_pss;
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
      INFO("Time offset adjustment: %d samples (%.2f), mean SFO: %.2f Hz, ema=%f, length=%d",
           q->next_rf_sample_offset,
           q->mean_sample_offset,
           srsran_ue_sync_get_sfo(q),
           q->sfo_ema,
           q->sample_offset_correct_period);
    }
    q->mean_sample_offset = 0;
  }

  /* If the PSS peak is beyond the frame (we sample too slowly),
    discard the offseted samples to align next frame */
  if (q->next_rf_sample_offset > 0 && q->next_rf_sample_offset < MAX_TIME_OFFSET) {
    DEBUG("Positive time offset %d samples.", q->next_rf_sample_offset);
    if (q->recv_callback(q->stream, dummy_offset_buffer, (uint32_t)q->next_rf_sample_offset, NULL) < 0) {
      ERROR("Error receiving from USRP");
      return SRSRAN_ERROR;
    }
    q->next_rf_sample_offset = 0;
  }

  q->peak_idx = q->sf_len / 2 + q->last_sample_offset;
  q->frame_ok_cnt++;
  q->frame_no_cnt = 0;

  return 1;
}

static int track_peak_no(srsran_ue_sync_t* q)
{
  /* if we missed too many PSS go back to FIND and consider this frame unsynchronized */
  q->frame_no_cnt++;
  if (q->frame_no_cnt >= TRACK_MAX_LOST) {
    INFO("%d frames lost. Going back to FIND", (int)q->frame_no_cnt);
    q->state = SF_FIND;
    return 0;
  } else {
    INFO("Tracking peak not found. Peak %.3f, %d lost", srsran_sync_get_peak_value(&q->strack), (int)q->frame_no_cnt);
    return 1;
  }
}

static int receive_samples(srsran_ue_sync_t* q, cf_t* input_buffer[SRSRAN_MAX_CHANNELS], const uint32_t max_num_samples)
{
  ///< A negative time offset means there are samples in our buffer for the next subframe bc we are sampling too fast
  if (q->next_rf_sample_offset < 0) {
    q->next_rf_sample_offset = -q->next_rf_sample_offset;
  }

  ///< Make sure receive buffer is big enough
  if (q->frame_len - q->next_rf_sample_offset > max_num_samples) {
    fprintf(stderr, "Receive buffer too small (%d < %d)\n", max_num_samples, q->frame_len - q->next_rf_sample_offset);
    return SRSRAN_ERROR;
  }

  ///< Get N subframes from the USRP getting more samples and keeping the previous samples, if any
  cf_t* ptr[SRSRAN_MAX_CHANNELS] = {NULL};
  for (int i = 0; i < q->nof_rx_antennas; i++) {
    ptr[i] = &input_buffer[i][q->next_rf_sample_offset];
  }
  if (q->recv_callback(q->stream, ptr, q->frame_len - q->next_rf_sample_offset, &q->last_timestamp) < 0) {
    return SRSRAN_ERROR;
  }

  ///< reset time offset
  q->next_rf_sample_offset = 0;

  return SRSRAN_SUCCESS;
}

/* Returns 1 if the subframe is synchronized in time, 0 otherwise */
int srsran_ue_sync_zerocopy(srsran_ue_sync_t* q,
                            cf_t*             input_buffer[SRSRAN_MAX_CHANNELS],
                            const uint32_t    max_num_samples)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && input_buffer != NULL) {
    if (q->file_mode) {
      int n = srsran_filesource_read_multi(&q->file_source, (void**)input_buffer, q->sf_len, q->nof_rx_antennas);
      if (n < 0) {
        ERROR("Error reading input file");
        return SRSRAN_ERROR;
      }
      if (n == 0) {
        if (q->file_wrap_enable) {
          srsran_filesource_seek(&q->file_source, 0);
          q->sf_idx = 9;
          n = srsran_filesource_read_multi(&q->file_source, (void**)input_buffer, q->sf_len, q->nof_rx_antennas);
          if (n < 0) {
            ERROR("Error reading input file");
            return SRSRAN_ERROR;
          }
        } else {
          return SRSRAN_ERROR;
        }
      }
      if (q->cfo_correct_enable_track) {
        for (int i = 0; i < q->nof_rx_antennas; i++) {
          srsran_cfo_correct(&q->file_cfo_correct, input_buffer[i], input_buffer[i], q->file_cfo / 15000 / q->fft_size);
        }
      }
      q->sf_idx++;
      if (q->sf_idx == 10) {
        q->sf_idx = 0;
      }
      INFO("Reading %d samples. sf_idx = %d", q->sf_len, q->sf_idx);
      ret = 1;
    } else {
      if (receive_samples(q, input_buffer, max_num_samples)) {
        ERROR("Error receiving samples");
        return SRSRAN_ERROR;
      }

      switch (q->state) {
        case SF_FIND:
          // Correct CFO before PSS/SSS find using the sync object corrector (initialized for 1 ms)
          if (q->cfo_correct_enable_find) {
            for (int i = 0; i < q->nof_rx_antennas; i++) {
              if (input_buffer[i]) {
                srsran_cfo_correct(
                    &q->strack.cfo_corr_frame, input_buffer[i], input_buffer[i], -q->cfo_current_value / q->fft_size);
              }
            }
          }

          // Run mode-specific find operation
          if (q->mode == SYNC_MODE_PSS) {
            ret = srsran_ue_sync_run_find_pss_mode(q, input_buffer);
          } else if (q->mode == SYNC_MODE_GNSS) {
            ret = srsran_ue_sync_run_find_gnss_mode(q, input_buffer, max_num_samples);
          }

          if (q->do_agc) {
            srsran_agc_process(&q->agc, input_buffer[0], q->sf_len);
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
                srsran_cfo_correct(
                    &q->strack.cfo_corr_frame, input_buffer[i], input_buffer[i], -q->cfo_current_value / q->fft_size);
              }
            }
          }

          if (q->mode == SYNC_MODE_PSS) {
            srsran_ue_sync_run_track_pss_mode(q, input_buffer);
          } else {
            srsran_ue_sync_run_track_gnss_mode(q, input_buffer);
          }
          break;
        default:
          ERROR("Unknown sync state %d", q->state);
      }
    }
  }
  return ret;
}

int srsran_ue_sync_run_find_pss_mode(srsran_ue_sync_t* q, cf_t* input_buffer[SRSRAN_MAX_CHANNELS])
{
  int ret = SRSRAN_ERROR;
  int n   = srsran_sync_find(&q->sfind, input_buffer[0], 0, &q->peak_idx);

  switch (n) {
    case SRSRAN_SYNC_ERROR:
      ERROR("Error finding correlation peak (%d)", ret);
      return ret;
    case SRSRAN_SYNC_FOUND:
      ret = find_peak_ok(q, input_buffer);
      break;
    case SRSRAN_SYNC_FOUND_NOSPACE:
      /* If a peak was found but there is not enough space for SSS/CP detection, discard a few samples */
      INFO("No space for SSS/CP detection. Realigning frame...");
      q->recv_callback(q->stream, dummy_offset_buffer, q->frame_len / 2, NULL);
      srsran_sync_reset(&q->sfind);
      ret = SRSRAN_SUCCESS;
      break;
    default:
      ret = SRSRAN_SUCCESS;
      break;
  }

  INFO("SYNC FIND: sf_idx=%d, ret=%d, peak_pos=%d, peak_value=%.2f, mean_cp_cfo=%.2f, mean_pss_cfo=%.2f, "
       "total_cfo_khz=%.1f",
       q->sf_idx,
       ret,
       q->peak_idx,
       q->sfind.peak_value,
       q->sfind.cfo_cp_mean,
       q->sfind.cfo_pss_mean,
       15 * srsran_sync_get_cfo(&q->sfind));

  return ret;
};

int srsran_ue_sync_run_track_pss_mode(srsran_ue_sync_t* q, cf_t* input_buffer[SRSRAN_MAX_CHANNELS])
{
  int      ret       = SRSRAN_ERROR;
  uint32_t track_idx = 0;
  /* Every SF idx 0 and 5, find peak around known position q->peak_idx */
  if ((q->sfind.frame_type == SRSRAN_FDD && (q->sf_idx == 0 || q->sf_idx == 5)) ||
      (q->sfind.frame_type == SRSRAN_TDD && (q->sf_idx == 1 || q->sf_idx == 6))) {
    // Process AGC every period
    if (q->do_agc && (q->agc_period == 0 || (q->agc_period && (q->frame_total_cnt % q->agc_period) == 0))) {
      srsran_agc_process(&q->agc, input_buffer[0], q->sf_len);
    }

    /* Track PSS around the expected PSS position
     * In tracking phase, the subframe carrying the PSS is always the last one of the frame
     */

    // Expected PSS position is different for FDD and TDD
    uint32_t pss_idx = q->frame_len - PSS_OFFSET - q->fft_size - q->strack.max_offset / 2;

    int n = srsran_sync_find(&q->strack, input_buffer[0], pss_idx, &track_idx);
    switch (n) {
      case SRSRAN_SYNC_ERROR:
        ERROR("Error tracking correlation peak");
        return SRSRAN_ERROR;
      case SRSRAN_SYNC_FOUND:
        ret = track_peak_ok(q, track_idx);
        break;
      case SRSRAN_SYNC_FOUND_NOSPACE:
        // It's very very unlikely that we fall here because this event should happen at FIND phase only
        ret      = 0;
        q->state = SF_FIND;
        INFO("Warning: No space for SSS/CP while in tracking phase");
        break;
      case SRSRAN_SYNC_NOFOUND:
        ret = track_peak_no(q);
        break;
    }

    if (ret == SRSRAN_ERROR) {
      ERROR("Error processing tracking peak");
      q->state = SF_FIND;
      return SRSRAN_SUCCESS;
    }

    q->frame_total_cnt++;

    INFO("SYNC TRACK: sf_idx=%d, ret=%d, peak_pos=%d, peak_value=%.2f, mean_cp_cfo=%.2f, mean_pss_cfo=%.2f, "
         "total_cfo_khz=%.1f",
         q->sf_idx,
         ret,
         track_idx,
         q->strack.peak_value,
         q->strack.cfo_cp_mean,
         q->strack.cfo_pss_mean,
         15 * srsran_sync_get_cfo(&q->strack));
  } else {
    INFO("SYNC TRACK: sf_idx=%d, ret=%d, next_state=%d", q->sf_idx, ret, q->state);
  }

  return 1; ///< 1 means subframe in sync
}

int srsran_ue_sync_run_find_gnss_mode(srsran_ue_sync_t* q,
                                      cf_t*             input_buffer[SRSRAN_MAX_CHANNELS],
                                      const uint32_t    max_num_samples)
{
  INFO("Calibration samples received start at %ld + %f", q->last_timestamp.full_secs, q->last_timestamp.frac_secs);

  // round to nearest second
  srsran_timestamp_t ts_next_rx, ts_next_rx_tmp, ts_tmp;
  srsran_timestamp_copy(&ts_next_rx, &q->last_timestamp);
  ts_next_rx.full_secs++;
  ts_next_rx.frac_secs = 0.0;

  srsran_timestamp_copy(&ts_next_rx_tmp, &ts_next_rx);
  INFO("Next desired recv at %ld + %f\n", ts_next_rx_tmp.full_secs, ts_next_rx_tmp.frac_secs);

  // get difference in time between second rx and now
  srsran_timestamp_sub(&ts_next_rx_tmp, q->last_timestamp.full_secs, q->last_timestamp.frac_secs);
  srsran_timestamp_sub(&ts_next_rx_tmp, 0, 0.001); ///< account for samples that have already been rx'ed

  uint64_t align_len = srsran_timestamp_uint64(&ts_next_rx_tmp, q->sf_len * 1000);

  DEBUG("Difference between first recv is %ld + %f, realigning %" PRIu64 " samples\n",
        ts_next_rx_tmp.full_secs,
        ts_next_rx_tmp.frac_secs,
        align_len);

  // receive align_len samples into dummy_buffer, make sure to not exceed buffer len
  uint32_t sample_count = 0;
  while (align_len > q->sf_len) {
    uint32_t actual_rx_len = SRSRAN_MIN(align_len, q->sf_len);
    q->recv_callback(q->stream, dummy_offset_buffer, actual_rx_len, &q->last_timestamp);

    srsran_timestamp_copy(&ts_tmp, &ts_next_rx);
    srsran_timestamp_sub(&ts_tmp, q->last_timestamp.full_secs, q->last_timestamp.frac_secs);
    srsran_timestamp_sub(&ts_tmp, 0, 0.001); ///< account for samples that have already been rx'ed
    align_len = srsran_timestamp_uint64(&ts_tmp, q->sf_len * 1000);

    if (align_len > (uint64_t)q->sf_len * 1000) {
      ts_next_rx.full_secs++;
      ts_next_rx.frac_secs = 0.0;
      srsran_timestamp_copy(&ts_tmp, &ts_next_rx);
      srsran_timestamp_sub(&ts_tmp, q->last_timestamp.full_secs, q->last_timestamp.frac_secs);
      srsran_timestamp_sub(&ts_tmp, 0, 0.001); ///< account for samples that have already been rx'ed
      align_len = srsran_timestamp_uint64(&ts_tmp, q->sf_len * 1000);
    }
  }

  DEBUG("Received %d samples during alignment", sample_count);

  // do one normal receive, the first time-aligned subframe
  if (receive_samples(q, input_buffer, max_num_samples)) {
    ERROR("Error receiving samples");
    return SRSRAN_ERROR;
  }

  INFO("First aligned samples received start at %ld + %f", q->last_timestamp.full_secs, q->last_timestamp.frac_secs);

  // switch to track state, from here on, samples should be ms aligned
  q->state = SF_TRACK;

  // calculate system timing
  if (srsran_ue_sync_set_tti_from_timestamp(q, &q->last_timestamp)) {
    ERROR("Error deriving timing from received samples");
    return SRSRAN_ERROR;
  }

  INFO("SYNC FIND: sfn=%d, sf_idx=%d next_state=%d", q->frame_number, q->sf_idx, q->state);

  return 1; ///< 1 means subframe in sync
}

///< The track function in GNSS mode only needs to increment the system frame number
int srsran_ue_sync_run_track_gnss_mode(srsran_ue_sync_t* q, cf_t* input_buffer[SRSRAN_MAX_CHANNELS])
{
  INFO("TRACK samples received at %ld + %.4f", q->last_timestamp.full_secs, q->last_timestamp.frac_secs);

  // make sure the fractional receive time is ms-aligned
  uint32_t rx_full_ms  = floor(q->last_timestamp.frac_secs * 1e3);
  double   rx_frac_ms  = q->last_timestamp.frac_secs - (rx_full_ms / 1e3);
  int32_t  offset_samp = round(rx_frac_ms / (1.0 / (q->sf_len * 1000)));
  INFO("rx_full_ms=%d, rx_frac_ms=%f, offset_samp=%d", rx_full_ms, rx_frac_ms, offset_samp);
  if (offset_samp != q->sf_len) {
    q->next_rf_sample_offset = offset_samp;
  }

  if (q->next_rf_sample_offset) {
    INFO("Time offset adjustment: %d samples", q->next_rf_sample_offset);
  }

  // update SF index
  q->sf_idx = ((int)round(q->last_timestamp.frac_secs * 1e3)) % SRSRAN_NOF_SF_X_FRAME;

  INFO("SYNC TRACK: sfn=%d, sf_idx=%d, next_state=%d", q->frame_number, q->sf_idx, q->state);

  return 1; ///< 1 means subframe in sync
}

/** Calculate TTI for UEs that are synced using GNSS time reference (TS 36.331 Sec. 5.10.14)
 *
 * @param q Pointer to current object
 * @param rx_timestamp Pointer to receive timestamp
 * @return SRSRAN_SUCCESS on success
 */
int srsran_ue_sync_set_tti_from_timestamp(srsran_ue_sync_t* q, srsran_timestamp_t* rx_timestamp)
{
  // calculate time_t of Rx time
  time_t t_cur = rx_timestamp->full_secs;
  DEBUG("t_cur=%ld", t_cur);

  // 3GPP Reference UTC time is 1. Jan 1900 at 0:00
  // If we put this date in https://www.epochconverter.com it returns a negative number (-2208988800)
  // as epoch time starts at 1. Jan 1970 at 0:00
  uint64_t epoch_offset_3gpp = 2208988800;

  static const uint32_t MSECS_PER_SEC = 1000;

  uint64_t time_3gpp_secs = t_cur + epoch_offset_3gpp;

  // convert to ms and add fractional part
  uint64_t time_3gpp_msecs = (time_3gpp_secs + rx_timestamp->frac_secs) * MSECS_PER_SEC;
  DEBUG("rx time with 3gpp base in ms %" PRIu64 "\n", time_3gpp_msecs);

  // calculate SFN and SF index according to TS 36.331 Sec. 5.10.14
  q->frame_number = (uint32_t)(((uint64_t)floor(0.1 * (time_3gpp_msecs - q->sfn_offset))) % 1024);
  q->sf_idx       = (uint32_t)(((uint64_t)floor(time_3gpp_msecs - q->sfn_offset)) % 10);

  return SRSRAN_SUCCESS;
}
