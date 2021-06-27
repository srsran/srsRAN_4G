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

#include "srsran/phy/ue/ue_sync_nr.h"
#include "srsran/phy/utils/vector.h"

#define UE_SYNC_NR_DEFAULT_CFO_ALPHA 0.1

int srsran_ue_sync_nr_init(srsran_ue_sync_nr_t* q, const srsran_ue_sync_nr_args_t* args)
{
  // Check inputs
  if (q == NULL || args == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Copy arguments
  q->recv_obj        = args->recv_obj;
  q->recv_callback   = args->recv_callback;
  q->nof_rx_channels = args->nof_rx_channels == 0 ? 1 : args->nof_rx_channels;
  q->disable_cfo     = args->disable_cfo;
  q->cfo_alpha       = isnormal(args->cfo_alpha) ? args->cfo_alpha : UE_SYNC_NR_DEFAULT_CFO_ALPHA;

  // Initialise SSB
  srsran_ssb_args_t ssb_args = {};
  ssb_args.max_srate_hz      = args->max_srate_hz;
  ssb_args.min_scs           = args->min_scs;
  ssb_args.enable_search     = true;
  ssb_args.enable_decode     = true;
  ssb_args.pbch_dmrs_thr     = args->pbch_dmrs_thr;
  if (srsran_ssb_init(&q->ssb, &ssb_args) < SRSRAN_SUCCESS) {
    ERROR("Error SSB init");
    return SRSRAN_ERROR;
  }

  // Allocate temporal buffer pointers
  q->tmp_buffer = SRSRAN_MEM_ALLOC(cf_t*, q->nof_rx_channels);
  if (q->tmp_buffer == NULL) {
    ERROR("Error alloc");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void srsran_ue_sync_nr_free(srsran_ue_sync_nr_t* q)
{
  // Check inputs
  if (q == NULL) {
    return;
  }

  srsran_ssb_free(&q->ssb);

  if (q->tmp_buffer) {
    free(q->tmp_buffer);
  }

  SRSRAN_MEM_ZERO(q, srsran_ue_sync_nr_t, 1);
}

int srsran_ue_sync_nr_set_cfg(srsran_ue_sync_nr_t* q, const srsran_ue_sync_nr_cfg_t* cfg)
{
  // Check inputs
  if (q == NULL || cfg == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Copy parameters
  q->N_id     = cfg->N_id;
  q->srate_hz = cfg->ssb.srate_hz;

  // Calculate new subframe size
  q->sf_sz = (uint32_t)round(1e-3 * q->srate_hz);

  // Configure SSB
  if (srsran_ssb_set_cfg(&q->ssb, &cfg->ssb) < SRSRAN_SUCCESS) {
    ERROR("Error configuring SSB");
    return SRSRAN_ERROR;
  }

  // Transition to find
  q->state = SRSRAN_UE_SYNC_NR_STATE_FIND;

  return SRSRAN_SUCCESS;
}

static void ue_sync_nr_reset_feedback(srsran_ue_sync_nr_t* q)
{
  SRSRAN_MEM_ZERO(&q->feedback, srsran_csi_trs_measurements_t, 1);
}

static void ue_sync_nr_apply_feedback(srsran_ue_sync_nr_t* q)
{
  // Skip any update if there is no feedback available
  if (q->feedback.nof_re == 0) {
    return;
  }

  // Update number of samples
  q->avg_delay_us          = q->feedback.delay_us;
  q->next_rf_sample_offset = (uint32_t)round((double)q->avg_delay_us * (q->srate_hz * 1e-6));

  // Integrate CFO
  if (q->disable_cfo) {
    q->cfo_hz = SRSRAN_VEC_SAFE_EMA(q->feedback.cfo_hz, q->cfo_hz, q->cfo_alpha);
  } else {
    q->cfo_hz += q->feedback.cfo_hz * q->cfo_alpha;
  }

  // Reset feedback
  ue_sync_nr_reset_feedback(q);
}

static int ue_sync_nr_run_find(srsran_ue_sync_nr_t* q, cf_t* buffer)
{
  srsran_csi_trs_measurements_t measurements = {};
  srsran_pbch_msg_nr_t          pbch_msg     = {};

  // Find SSB, measure PSS/SSS and decode PBCH
  if (srsran_ssb_find(&q->ssb, buffer, q->N_id, &measurements, &pbch_msg) < SRSRAN_SUCCESS) {
    ERROR("Error finding SSB");
    return SRSRAN_ERROR;
  }

  // If the PBCH message was NOT decoded, early return
  if (!pbch_msg.crc) {
    return SRSRAN_SUCCESS;
  }

  // Reset feedback to prevent any previous erroneous measurement
  ue_sync_nr_reset_feedback(q);

  // Set feedback measurement
  srsran_combine_csi_trs_measurements(&q->feedback, &measurements, &q->feedback);

  // Apply feedback
  ue_sync_nr_apply_feedback(q);

  // Setup context
  q->ssb_idx = pbch_msg.ssb_idx;
  q->sf_idx  = srsran_ssb_candidate_sf_idx(&q->ssb, pbch_msg.ssb_idx, pbch_msg.hrf);
  q->sfn     = pbch_msg.sfn_4lsb;

  // Transition to track only if the measured delay is below 2.4 microseconds
  if (measurements.delay_us < 2.4f) {
    q->state = SRSRAN_UE_SYNC_NR_STATE_TRACK;
  }

  return SRSRAN_SUCCESS;
}

static int ue_sync_nr_run_track(srsran_ue_sync_nr_t* q, cf_t* buffer)
{
  srsran_csi_trs_measurements_t measurements = {};
  srsran_pbch_msg_nr_t          pbch_msg     = {};
  uint32_t                      half_frame   = q->sf_idx / (SRSRAN_NOF_SF_X_FRAME / 2);

  // Check if the SSB selected candidate index shall be received in this subframe
  bool is_ssb_opportunity = (q->sf_idx == srsran_ssb_candidate_sf_idx(&q->ssb, q->ssb_idx, half_frame > 0));

  // If
  if (is_ssb_opportunity) {
    // Measure PSS/SSS and decode PBCH
    if (srsran_ssb_track(&q->ssb, buffer, q->N_id, q->ssb_idx, half_frame, &measurements, &pbch_msg) < SRSRAN_SUCCESS) {
      ERROR("Error finding SSB");
      return SRSRAN_ERROR;
    }

    // If the PBCH message was NOT decoded, transition to track
    if (!pbch_msg.crc) {
      q->state = SRSRAN_UE_SYNC_NR_STATE_FIND;
      return SRSRAN_SUCCESS;
    }

    // Otherwise feedback measurements and apply
    srsran_combine_csi_trs_measurements(&q->feedback, &measurements, &q->feedback);
  }

  // Apply accumulated feedback
  ue_sync_nr_apply_feedback(q);

  return SRSRAN_SUCCESS;
}

static int ue_sync_nr_recv(srsran_ue_sync_nr_t* q, cf_t** buffer, srsran_timestamp_t* timestamp)
{
  // Verify callback and srate are valid
  if (q->recv_callback == NULL && !isnormal(q->srate_hz)) {
    return SRSRAN_ERROR;
  }

  uint32_t buffer_offset = 0;
  uint32_t nof_samples   = q->sf_sz;

  if (q->next_rf_sample_offset > 0) {
    // Discard a number of samples from RF
    if (q->recv_callback(q->recv_obj, buffer, (uint32_t)q->next_rf_sample_offset, timestamp) < SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  } else {
    // Adjust receive buffer
    buffer_offset = (uint32_t)(-q->next_rf_sample_offset);
    nof_samples   = (uint32_t)(q->sf_sz + q->next_rf_sample_offset);
  }
  q->next_rf_sample_offset = 0;

  // Select buffer offsets
  for (uint32_t chan = 0; chan < q->nof_rx_channels; chan++) {
    // Set buffer to NULL if not present
    if (buffer[chan] == NULL) {
      q->tmp_buffer[chan] = NULL;
      continue;
    }

    // Initialise first offset samples to zero
    if (buffer_offset > 0) {
      srsran_vec_cf_zero(buffer[chan], buffer_offset);
    }

    // Set to sample index
    q->tmp_buffer[chan] = &buffer[chan][buffer_offset];
  }

  // Receive
  if (q->recv_callback(q->recv_obj, q->tmp_buffer, nof_samples, timestamp) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Compensate CFO
  for (uint32_t chan = 0; chan < q->nof_rx_channels; chan++) {
    if (buffer[chan] != 0 && !q->disable_cfo) {
      srsran_vec_apply_cfo(buffer[chan], q->cfo_hz / q->srate_hz, buffer[chan], (int)q->sf_sz);
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_ue_sync_nr_zerocopy(srsran_ue_sync_nr_t* q, cf_t** buffer, srsran_ue_sync_nr_outcome_t* outcome)
{
  // Check inputs
  if (q == NULL || buffer == NULL || outcome == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Verify callback is valid
  if (q->recv_callback == NULL) {
    return SRSRAN_ERROR;
  }

  // Receive
  if (ue_sync_nr_recv(q, buffer, &outcome->timestamp) < SRSRAN_SUCCESS) {
    ERROR("Error receiving baseband");
    return SRSRAN_ERROR;
  }

  // Run FSM
  switch (q->state) {
    case SRSRAN_UE_SYNC_NR_STATE_IDLE:
      // Do nothing
      break;
    case SRSRAN_UE_SYNC_NR_STATE_FIND:
      if (ue_sync_nr_run_find(q, buffer[0]) < SRSRAN_SUCCESS) {
        ERROR("Error running find");
        return SRSRAN_ERROR;
      }
      break;
    case SRSRAN_UE_SYNC_NR_STATE_TRACK:
      if (ue_sync_nr_run_track(q, buffer[0]) < SRSRAN_SUCCESS) {
        ERROR("Error running track");
        return SRSRAN_ERROR;
      }
      break;
  }

  // Increment subframe counter
  q->sf_idx++;

  // Increment SFN
  if (q->sf_idx >= SRSRAN_NOF_SF_X_FRAME) {
    q->sfn    = (q->sfn + 1) % 1024;
    q->sf_idx = 0;
  }

  // Fill outcome
  outcome->in_sync  = (q->state == SRSRAN_UE_SYNC_NR_STATE_TRACK);
  outcome->sf_idx   = q->sf_idx;
  outcome->sfn      = q->sfn;
  outcome->cfo_hz   = q->cfo_hz;
  outcome->delay_us = q->avg_delay_us;

  return SRSRAN_SUCCESS;
}

int srsran_ue_sync_nr_feedback(srsran_ue_sync_nr_t* q, const srsran_csi_trs_measurements_t* measurements)
{
  if (q == NULL || measurements == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Accumulate feedback proportional to the number of elements provided by the measurement
  srsran_combine_csi_trs_measurements(&q->feedback, measurements, &q->feedback);

  return SRSRAN_SUCCESS;
}
