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

#include "srsue/hdr/phy/nr/slot_sync.h"

namespace srsue {
namespace nr {

slot_sync::slot_sync(srslog::basic_logger& logger_) : logger(logger_), sfn_sync_buff(1) {}

slot_sync::~slot_sync()
{
  srsran_ue_sync_nr_free(&ue_sync_nr);
}

static int slot_sync_recv_callback(void* ptr, cf_t** buffer, uint32_t nsamples, srsran_timestamp_t* ts)
{
  if (ptr == nullptr) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
  slot_sync* sync = (slot_sync*)ptr;

  cf_t* buffer_ptr[SRSRAN_MAX_CHANNELS] = {};
  buffer_ptr[0]                         = buffer[0];

  srsran::rf_buffer_t rf_buffer(buffer_ptr, nsamples);

  return sync->recv_callback(rf_buffer, ts);
}

bool slot_sync::init(const args_t& args, stack_interface_phy_nr* stack_, srsran::radio_interface_phy* radio_)
{
  stack = stack_;
  radio = radio_;

  srsran_ue_sync_nr_args_t ue_sync_nr_args = {};
  ue_sync_nr_args.max_srate_hz             = args.max_srate_hz;
  ue_sync_nr_args.min_scs                  = args.ssb_min_scs;
  ue_sync_nr_args.nof_rx_channels          = args.nof_rx_channels;
  ue_sync_nr_args.disable_cfo              = args.disable_cfo;
  ue_sync_nr_args.pbch_dmrs_thr            = args.pbch_dmrs_thr;
  ue_sync_nr_args.cfo_alpha                = args.cfo_alpha;
  ue_sync_nr_args.recv_obj                 = this;
  ue_sync_nr_args.recv_callback            = slot_sync_recv_callback;

  if (srsran_ue_sync_nr_init(&ue_sync_nr, &ue_sync_nr_args) < SRSRAN_SUCCESS) {
    logger.error("Error initiating UE SYNC NR object");
    return false;
  }

  return true;
}

int slot_sync::set_sync_cfg(const srsran_ue_sync_nr_cfg_t& cfg)
{
  // Print the configuration, it is essential to make sure the UE synchronizes with the wight cell
  if (logger.info.enabled()) {
    std::array<char, 512> ssb_cfg_str = {};
    srsran_ssb_cfg_to_str(&cfg.ssb, ssb_cfg_str.data(), (uint32_t)ssb_cfg_str.size());
    logger.info("SYNC: Setting SSB configuration %s Tracking N_id=%d.", ssb_cfg_str.data(), cfg.N_id);
  }

  // Set the synchronization configuration
  if (srsran_ue_sync_nr_set_cfg(&ue_sync_nr, &cfg) < SRSRAN_SUCCESS) {
    logger.error("SYNC: failed to set cell configuration for N_id %d", cfg.N_id);
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int slot_sync::recv_callback(srsran::rf_buffer_t& data, srsran_timestamp_t* rx_time)
{
  // This function is designed for being called from the UE sync object which will pass a null rx_time in case
  // receive dummy samples. So, rf_timestamp points at dummy timestamp in case rx_time is not provided
  srsran::rf_timestamp_t  dummy_ts     = {};
  srsran::rf_timestamp_t& rf_timestamp = (rx_time == nullptr) ? dummy_ts : last_rx_time;

  // Receive
  if (not radio->rx_now(data, rf_timestamp)) {
    return SRSRAN_ERROR;
  }

  srsran_timestamp_t dummy_flat_ts = {};

  // Load flat timestamp
  if (rx_time == nullptr) {
    rx_time = &dummy_flat_ts;
  }
  *rx_time = rf_timestamp.get(0);

  // Save RF timestamp for the stack
  stack_tti_ts_new = rf_timestamp.get(0);

  // Run stack if the sync state is not in camping
  logger.debug("run_stack_tti: from recv");
  run_stack_tti();

  logger.debug("SYNC:  received %d samples from radio", data.get_nof_samples());

  return data.get_nof_samples();
}

bool slot_sync::run_sfn_sync()
{
  // Run UE SYNC process using the temporal SFN process buffer
  srsran_ue_sync_nr_outcome_t outcome = {};
  if (srsran_ue_sync_nr_zerocopy(&ue_sync_nr, sfn_sync_buff.to_cf_t(), &outcome) < SRSRAN_SUCCESS) {
    logger.error("SYNC: error in zerocopy");
    return false;
  }

  // If in sync, update slot index
  if (outcome.in_sync) {
    slot_cfg.idx = outcome.sfn * SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz) + outcome.sf_idx;
  }

  // Return true if the PHY in-sync
  return outcome.in_sync;
}

bool slot_sync::run_camping(srsran::rf_buffer_t& buffer, srsran::rf_timestamp_t& timestamp)
{
  // Run UE SYNC process using an external baseband buffer
  srsran_ue_sync_nr_outcome_t outcome = {};
  if (srsran_ue_sync_nr_zerocopy(&ue_sync_nr, buffer.to_cf_t(), &outcome) < SRSRAN_SUCCESS) {
    logger.error("SYNC: error in zerocopy");
    return false;
  }

  // If in sync, update slot index
  if (outcome.in_sync) {
    slot_cfg.idx = outcome.sfn * SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz) + outcome.sf_idx;
  }

  // Set RF timestamp
  *timestamp.get_ptr(0) = outcome.timestamp;

  // Return true if the PHY in-sync
  return outcome.in_sync;
}

void slot_sync::run_stack_tti()
{ // check timestamp reset
  if (forced_rx_time_init || srsran_timestamp_iszero(&stack_tti_ts) ||
      srsran_timestamp_compare(&stack_tti_ts_new, &stack_tti_ts) < 0) {
    if (srsran_timestamp_compare(&stack_tti_ts_new, &stack_tti_ts) < 0) {
      logger.warning("SYNC:  radio time seems to be going backwards (rx_time=%f, tti_ts=%f)",
                     srsran_timestamp_real(&stack_tti_ts_new),
                     srsran_timestamp_real(&stack_tti_ts));
      // time-stamp will be set to rx time below and run_tti() will be called with MIN_TTI_JUMP
    }

    // init tti_ts with last rx time
    logger.debug("SYNC:  Setting initial TTI time to %f", srsran_timestamp_real(&stack_tti_ts_new));
    srsran_timestamp_copy(&stack_tti_ts, &stack_tti_ts_new);
    forced_rx_time_init = false;
  }

  // Advance stack in time
  if (srsran_timestamp_compare(&stack_tti_ts_new, &stack_tti_ts) >= 0) {
    srsran_timestamp_t temp = {};
    srsran_timestamp_copy(&temp, &stack_tti_ts_new);
    srsran_timestamp_sub(&temp, stack_tti_ts.full_secs, stack_tti_ts.frac_secs);
    int32_t tti_jump = static_cast<int32_t>(srsran_timestamp_uint64(&temp, 1e3));
    tti_jump         = SRSRAN_MAX(tti_jump, MIN_TTI_JUMP);
    if (tti_jump > MAX_TTI_JUMP) {
      logger.warning("SYNC:  TTI jump of %d limited to %d", tti_jump, int(MAX_TTI_JUMP));
      tti_jump = SRSRAN_MIN(tti_jump, MAX_TTI_JUMP);
    }

    // Run stack
    logger.debug("run_stack_tti: calling stack tti=%d, tti_jump=%d", slot_cfg.idx, tti_jump);
    stack->run_tti(slot_cfg.idx, tti_jump);
    logger.debug("run_stack_tti: stack called");
  }

  // update timestamp
  srsran_timestamp_copy(&stack_tti_ts, &stack_tti_ts_new);
}

srsran_slot_cfg_t slot_sync::get_slot_cfg()
{
  return slot_cfg;
}

} // namespace nr
} // namespace srsue