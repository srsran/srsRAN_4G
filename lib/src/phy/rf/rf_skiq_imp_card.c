/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "rf_skiq_imp_card.h"

static void* reader_thread(void* arg)
{
  rf_skiq_card_t* q = (rf_skiq_card_t*)arg;

  while (q->state != RF_SKIQ_PORT_STATE_STOP) {
    // Wait to leave idle state
    if (q->state == RF_SKIQ_PORT_STATE_IDLE) {
      SKIQ_RF_INFO("[Rx %d] IDLE\n", q->card);
      pthread_mutex_lock(&q->mutex);
      while (q->state == RF_SKIQ_PORT_STATE_IDLE) {
        pthread_cond_wait(&q->cvar, &q->mutex);
      }
      pthread_mutex_unlock(&q->mutex);
      SKIQ_RF_INFO("[Rx %d] %s\n", q->card, q->state == RF_SKIQ_PORT_STATE_STREAMING ? "STREAMING" : "STOP");
    }

    // Check exit condition
    if (q->state == RF_SKIQ_PORT_STATE_STOP) {
      break;
    }

    skiq_rx_hdl_t    curr_rx_hdl = 0;
    skiq_rx_block_t* p_rx_block  = NULL;
    uint32_t         len         = 0;
    skiq_rx_status_t rx_status   = skiq_receive(q->card, &curr_rx_hdl, &p_rx_block, &len);

    switch (rx_status) {
      case skiq_rx_status_success:
        if (curr_rx_hdl < q->nof_ports && p_rx_block != NULL && len > SKIQ_RX_HEADER_SIZE_IN_BYTES) {
          // Convert number of bytes into samples
          uint32_t nsamples = len / 4 - SKIQ_RX_HEADER_SIZE_IN_WORDS;

          // Give block to the port
          rf_skiq_rx_port_write(&q->rx_ports[curr_rx_hdl], p_rx_block, nsamples);
        } else {
          ERROR("Card %d received data with corrupted pointers\n", q->card);
        }
        break;
      case skiq_rx_status_no_data:
        // Do nothing
        break;
      case skiq_rx_status_error_generic:
        ERROR("Error: Generic error occurred during skiq_receive.\n");
        break;
      case skiq_rx_status_error_overrun:
        ERROR("Error: overrun error occurred during skiq_receive.\n");
        break;
      case skiq_rx_status_error_packet_malformed:
        ERROR("Error: packet malformed error occurred during skiq_receive.\n");
        break;
      case skiq_rx_status_error_card_not_active:
        ERROR("Error: inactive card error occurred during skiq_receive.\n");
        break;
      case skiq_rx_status_error_not_streaming:
        ERROR("Error: not streaming card error occurred during skiq_receive.\n");
        break;
      default:
        ERROR("Error: the impossible happened during skiq_receive.\n");
        break;
    }
  }

  SKIQ_RF_INFO("Exiting reader thread!\n");

  return NULL;
}

int rf_skiq_card_init(rf_skiq_card_t* q, uint8_t card, uint8_t nof_ports, const rf_skiq_port_opts_t* opts)
{
  q->card      = card;
  q->nof_ports = nof_ports;

  // Reprogram FPGA to reset all states
  if (skiq_prog_fpga_from_flash(card)) {
    ERROR("Error programming card %d from flash\n", q->card);
    return SRSRAN_ERROR;
  }

  // Read card parameters
  if (skiq_read_parameters(card, &q->param)) {
    ERROR("Reading card %d param", card);
    return SRSRAN_ERROR;
  }

  // Check number of rx channels
  if (q->param.rf_param.num_rx_channels < nof_ports) {
    ERROR("Card %d does not support %d Rx channels", card, nof_ports);
    return SRSRAN_ERROR;
  }

  // Check number of tx channels
  if (q->param.rf_param.num_tx_channels < nof_ports) {
    ERROR("Card %d does not support %d Tx channels", card, nof_ports);
    return SRSRAN_ERROR;
  }

  // set a modest rx timeout
  if (skiq_set_rx_transfer_timeout(card, 1000)) {
    ERROR("Setting Rx transfer timeout");
    return SRSRAN_ERROR;
  }

  // do not pack 12bit samples
  if (skiq_write_iq_pack_mode(card, false)) {
    ERROR("Setting Rx IQ pack mode");
    return SRSRAN_ERROR;
  }

  // set the control output bits to include the gain
  if (skiq_write_rfic_control_output_config(
          card, RFIC_CONTROL_OUTPUT_MODE_GAIN_CONTROL_RXA1, RFIC_CONTROL_OUTPUT_MODE_GAIN_BITS) != 0) {
    ERROR("Unable to configure card %d the RF IC control output (A1)", card);
    return SRSRAN_ERROR;
  }

  // set RX channel mode
  if (skiq_write_chan_mode(card, q->mode)) {
    ERROR("Setting card %d channel mode", card);
    return SRSRAN_ERROR;
  }

  // Select Rx streaming mode to low latency if the sampling rate is lower than 5MHz
  if (skiq_write_rx_stream_mode(q->card, opts->stream_mode)) {
    ERROR("Error setting Rx stream mode\n");
    return SRSRAN_ERROR;
  }

  // initialise tx/rx ports
  for (uint8_t i = 0; i < nof_ports; i++) {
    if (rf_skiq_tx_port_init(&q->tx_ports[i], card, (skiq_tx_hdl_t)i, opts)) {
      ERROR("Initiating card %d, Tx port %d", card, i);
      return SRSRAN_ERROR;
    }

    if (rf_skiq_rx_port_init(&q->rx_ports[i], card, (skiq_rx_hdl_t)i, opts)) {
      ERROR("Initiating card %d, Rx port %d", card, i);
      return SRSRAN_ERROR;
    }
  }

  if (pthread_mutex_init(&q->mutex, NULL)) {
    ERROR("Initiating mutex");
    return SRSRAN_ERROR;
  }

  if (pthread_cond_init(&q->cvar, NULL)) {
    ERROR("Initiating cvar");
    return SRSRAN_ERROR;
  }

  // Initialise thread parameters
  pthread_attr_t     attr;
  struct sched_param param;

  param.sched_priority = sched_get_priority_max(SCHED_FIFO);
  pthread_attr_init(&attr);
  if (pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
    ERROR("Error not enough privileges to set Scheduling priority\n");
  }

  if (pthread_attr_setschedpolicy(&attr, SCHED_FIFO)) {
    ERROR("Error not enough privileges to set Scheduling priority\n");
  }

  if (pthread_attr_setschedparam(&attr, &param)) {
    ERROR("Error not enough privileges to set Scheduling priority\n");
  }

  // Launch thread
  if (pthread_create(&q->thread, &attr, reader_thread, q)) {
    ERROR("Error creating reader thread with attributes (Did you miss sudo?). Trying without attributes.\n");

    // try to create thread without attributes
    pthread_attr_destroy(&attr);
    if (pthread_create(&q->thread, NULL, reader_thread, q)) {
      ERROR("Error creating reader thread, even without thread attributes. Exiting.\n");
      return SRSRAN_ERROR;
    }
  }

  // Rename thread
  char thread_name[32] = {};
  if (snprintf(thread_name, sizeof(thread_name), "SKIQ Rx %d", q->card) > 0) {
    pthread_setname_np(q->thread, thread_name);
  }

  return SRSRAN_SUCCESS;
}

void rf_skiq_card_set_error_handler(rf_skiq_card_t* q, srsran_rf_error_handler_t error_handler, void* arg)
{
  for (uint32_t i = 0; i < q->nof_ports; i++) {
    rf_skiq_tx_port_set_error_handler(&q->tx_ports[i], error_handler, arg);
    rf_skiq_rx_port_set_error_handler(&q->rx_ports[i], error_handler, arg);
  }
}

double rf_skiq_card_set_tx_gain_db(rf_skiq_card_t* q, uint32_t port_idx, double gain_db)
{
  double max_atten_dB = 0.25 * q->param.tx_param->atten_quarter_db_max;
  double min_atten_dB = 0.25 * q->param.tx_param->atten_quarter_db_min;

  // Calculate attenuation:
  // - 0dB attenuation -> Maximum gain;
  // - 0dB gain -> Maximum attenuation;
  double att_dB = max_atten_dB - gain_db;

  // Check gain range
  if (att_dB < min_atten_dB || att_dB > max_atten_dB) {
    ERROR("Error port %d:%d the selected gain (%.2f dB) is out of range (%.2f to %.2f dB).\n",
          q->card,
          port_idx,
          gain_db,
          min_atten_dB,
          max_atten_dB);
  }

  // Calculate attenuation index
  uint16_t att_index = (uint16_t)floor(att_dB * 4);

  // Bound index
  att_index = SRSRAN_MIN(SRSRAN_MAX(att_index, q->param.tx_param->atten_quarter_db_min),
                         q->param.tx_param->atten_quarter_db_max);

  // Calculate equivalent gain
  double actual_gain_dB = max_atten_dB - att_index * 0.25;

  // Set gain per port
  if (port_idx >= q->nof_ports) {
    for (uint8_t i = 0; i < q->nof_ports; i++) {
      if (skiq_write_tx_attenuation(q->card, (skiq_tx_hdl_t)i, att_index)) {
        ERROR("Error setting card %d:%d Tx attenuation\n", q->card, i);
      }
    }
  } else {
    if (skiq_write_tx_attenuation(q->card, (skiq_tx_hdl_t)port_idx, att_index)) {
      ERROR("Error setting card %d:%d Tx attenuation\n", q->card, port_idx);
    }
  }

  return actual_gain_dB;
}

double rf_skiq_card_set_rx_gain_db(rf_skiq_card_t* q, uint32_t port_idx, double gain_db)
{
  // From Sidekiq API doc:
  //
  // For Sidekiq mPCIe (skiq_mpcie), Sidekiq M.2 (skiq_m2), Sidekiq Stretch (skiq_m2_2280), Sidekiq Z2
  //(skiq_z2), and Matchstiq Z3u (skiq_z3u) each increment of the gain index value results in approxi-
  // mately 1 dB of gain, with approximately 76 dB of total gain available. For details on the gain table,
  // refer to p. 37 of AD9361 Reference Manual UG-570

  // Check gain range
  if (gain_db < q->param.rx_param->gain_index_min || gain_db > q->param.rx_param->gain_index_max) {
    ERROR("Error port %d:%d the selected gain (%.2f dB) is out of range (%d to %d dB).\n",
          q->card,
          port_idx,
          gain_db,
          q->param.rx_param->gain_index_min,
          q->param.rx_param->gain_index_max);
  }

  // Calculate attenuation index
  uint16_t gain_idx = (uint16_t)floor(gain_db);

  if (port_idx < q->nof_ports) {
    // Set single port gain
    skiq_write_rx_gain(q->card, (skiq_rx_hdl_t)port_idx, gain_idx);
  } else {
    // Set all gains
    for (int i = 0; i < q->nof_ports; i++) {
      skiq_write_rx_gain(q->card, (skiq_rx_hdl_t)i, gain_idx);
    }
  }

  // Update current rx_gain
  q->cur_rx_gain_db = gain_db;

  return gain_db;
}

int rf_skiq_card_update_timestamp(rf_skiq_card_t* q, bool use_1pps, uint64_t new_ts)
{
  if (use_1pps) {
    // Read 1pps source
    skiq_1pps_source_t pps_source = skiq_1pps_source_unavailable;
    if (skiq_read_1pps_source(q->card, &pps_source)) {
      ERROR("Error reading card %d 1PPS source\n", q->card);
      return SRSRAN_ERROR;
    }

    // Make sure the source is external
    if (pps_source != skiq_1pps_source_external) {
      ERROR("Error card %d is not configured with external 1PPS source\n", q->card);
      return SRSRAN_ERROR;
    }

    // Get last time a PPS was received
    uint64_t ts_sys_1pps = 0;
    if (skiq_read_last_1pps_timestamp(q->card, NULL, &ts_sys_1pps)) {
      ERROR("Reading card %d last 1PPS timestamp", q->card);
      return SRSRAN_ERROR;
    }

    // Read current system time
    uint64_t ts = 0;
    if (skiq_read_curr_sys_timestamp(q->card, &ts)) {
      ERROR("Reading card %d system timestamp", q->card);
      return SRSRAN_ERROR;
    }

    // Make sure a 1PPS was received less than 2 seconds ago
    if (ts - ts_sys_1pps > 2 * SKIQ_SYS_TIMESTAMP_FREQ) {
      ERROR("Error card %d last PPS was received %.1f seconds ago (%ld - %ld)\n",
            q->card,
            (double)(ts - ts_sys_1pps) / (double)SKIQ_SYS_TIMESTAMP_FREQ,
            ts,
            ts_sys_1pps);
      return SRSRAN_ERROR;
    }

    // Set a given time in the future, a 100th of a second (10ms)
    ts += SKIQ_SYS_TIMESTAMP_FREQ / 100;

    // Order that all timestamps are reseted when next 1PPS signal is received
    SKIQ_RF_INFO("  ... Resetting card %d system timestamp on next PPS\n", q->card);
    if (skiq_write_timestamp_update_on_1pps(q->card, ts, new_ts)) {
      ERROR("Error reseting card %d timestamp on 1 PPS", q->card);
      return SRSRAN_ERROR;
    }
  } else {
    // Simply, reset timestamp
    SKIQ_RF_INFO("  ... Resetting card %d system timestamp now\n", q->card);
    if (skiq_update_timestamps(q->card, new_ts)) {
      ERROR("Error resetting card %d timestamp", q->card);
      return SRSRAN_ERROR;
    }
  }
  return SRSRAN_SUCCESS;
}

uint64_t rf_skiq_card_read_sys_timestamp(rf_skiq_card_t* q)
{
  uint64_t ts = 0UL;

  if (skiq_read_curr_sys_timestamp(q->card, &ts)) {
    ERROR("Reading card %d system timestamp", q->card);
  }

  return ts;
}

uint64_t rf_skiq_card_read_rf_timestamp(rf_skiq_card_t* q)
{
  uint64_t ts = 0UL;

  if (skiq_read_curr_rx_timestamp(q->card, skiq_rx_hdl_A1, &ts)) {
    ERROR("Reading card %d system timestamp", q->card);
  }

  return ts;
}

int rf_skiq_card_start_rx_streaming(rf_skiq_card_t* q, uint64_t timestamp)
{
  // Wrong state
  if (q->state == RF_SKIQ_PORT_STATE_STOP) {
    ERROR("Error starting Rx stream: wrong state (%d)\n", q->state);
    return SRSRAN_ERROR;
  }

  // Already enabled
  if (q->state == RF_SKIQ_PORT_STATE_STREAMING) {
    SKIQ_RF_INFO("Rx streams in card %d have already started\n", q->card);
    return SRSRAN_SUCCESS;
  }

  // Make a list with Rx handlers
  skiq_rx_hdl_t rx_hdl[RF_SKIQ_MAX_PORTS_CARD];
  for (uint8_t i = 0; i < RF_SKIQ_MAX_PORTS_CARD; i++) {
    rx_hdl[i] = (skiq_rx_hdl_t)i;
  }

  pthread_mutex_lock(&q->mutex);

  // Start all Rx in a row
  if (skiq_start_rx_streaming_multi_on_trigger(q->card, rx_hdl, q->nof_ports, skiq_trigger_src_synced, timestamp)) {
    ERROR("Failed to start card %d Rx streaming\n", q->card);
    pthread_mutex_unlock(&q->mutex);
    return SRSRAN_ERROR;
  }

  //  Update state and broadcast condition variable
  q->state = RF_SKIQ_PORT_STATE_STREAMING;
  pthread_cond_broadcast(&q->cvar);
  pthread_mutex_unlock(&q->mutex);

  SKIQ_RF_INFO("Rx streams in card %d have started\n", q->card);

  return SRSRAN_SUCCESS;
}

int rf_skiq_card_stop_rx_streaming(rf_skiq_card_t* q)
{
  if (q->state == RF_SKIQ_PORT_STATE_STOP) {
    ERROR("Error stopping Rx stream: wrong state (%d)\n", q->state);
    return SRSRAN_ERROR;
  }

  // Avoid stop streaming if it was not started
  if (q->state == RF_SKIQ_PORT_STATE_IDLE) {
    return SRSRAN_ERROR;
  }

  // Make a list with Tx/Rx handlers
  skiq_rx_hdl_t rx_hdl[RF_SKIQ_MAX_PORTS_CARD];
  for (uint8_t i = 0; i < RF_SKIQ_MAX_PORTS_CARD; i++) {
    rx_hdl[i] = (skiq_rx_hdl_t)i;
  }

  pthread_mutex_lock(&q->mutex);

  // Update state and broadcast condition variable first
  q->state = RF_SKIQ_PORT_STATE_IDLE;
  pthread_cond_broadcast(&q->cvar);

  // Stop all Rx in a row
  if (skiq_stop_rx_streaming_multi_immediate(q->card, rx_hdl, q->nof_ports)) {
    ERROR("Failed to stop card %d Rx streaming\n", q->card);
    pthread_mutex_unlock(&q->mutex);
    return SRSRAN_ERROR;
  }

  pthread_mutex_unlock(&q->mutex);

  SKIQ_RF_INFO("Rx streams in card %d have stopped\n", q->card);

  return SRSRAN_SUCCESS;
}

void rf_skiq_card_end_of_burst(rf_skiq_card_t* q)
{
  for (uint32_t i = 0; i < q->nof_ports; i++) {
    rf_skiq_tx_port_end_of_burst(&q->tx_ports[i]);
  }
}

int rf_skiq_card_set_srate_hz(rf_skiq_card_t* q, uint32_t srate_hz)
{
  for (uint8_t i = 0; i < q->nof_ports; i++) {
    // Set transmitter sampling rate
    if (skiq_write_tx_sample_rate_and_bandwidth(q->card, (skiq_tx_hdl_t)i, srate_hz, srate_hz)) {
      ERROR("Setting Tx sampling rate\n");
    }

    // Set receiver sampling rate
    if (skiq_write_rx_sample_rate_and_bandwidth(q->card, (skiq_rx_hdl_t)i, srate_hz, srate_hz)) {
      ERROR("Setting Rx sampling rate\n");
    }

    rf_skiq_rx_port_reset(&q->rx_ports[i]);
  }

  return SRSRAN_SUCCESS;
}

double rf_skiq_card_set_tx_freq_hz(rf_skiq_card_t* q, uint32_t port_idx, double freq_hz)
{
  q->suspend = true;
  rf_skiq_tx_port_set_lo(&q->tx_ports[port_idx], (uint64_t)freq_hz);
  q->suspend = false;

  return freq_hz;
}

double rf_skiq_card_set_rx_freq_hz(rf_skiq_card_t* q, uint32_t port_idx, double freq_hz)
{
  q->suspend = true;
  rf_skiq_rx_port_set_lo(&q->rx_ports[port_idx], (uint64_t)freq_hz);
  q->suspend = false;
  return freq_hz;
}

void rf_skiq_card_close(rf_skiq_card_t* q)
{
  SKIQ_RF_INFO("Closing card %d...\n", q->card);

  // Post stop state to reader thread
  q->state = RF_SKIQ_PORT_STATE_STOP;
  pthread_cond_broadcast(&q->cvar);

  // Wait for reader thread to finish
  pthread_join(q->thread, NULL);

  for (uint8_t i = 0; i < q->nof_ports; i++) {
    rf_skiq_rx_port_free(&q->rx_ports[i]);
    rf_skiq_tx_port_free(&q->tx_ports[i]);
  }

  pthread_cond_destroy(&q->cvar);
  pthread_mutex_destroy(&q->mutex);

  // Unlocks all cards
  if (skiq_disable_cards(&q->card, 1)) {
    ERROR("Unable to disable card %d\n", q->card);
  }
}

int rf_skiq_card_receive(rf_skiq_card_t* q, uint32_t port_idx, cf_t* dst, uint32_t nsamples, uint64_t* ts)
{
  // If suspended and samples are not available, then set all to zero
  if (q->suspend && rf_skiq_rx_port_available(&q->rx_ports[port_idx]) == 0) {
    srsran_vec_cf_zero(dst, nsamples);
    *ts = 0UL;
    return nsamples;
  }

  return rf_skiq_rx_port_read(&q->rx_ports[port_idx], dst, nsamples, ts);
}

uint64_t rf_skiq_card_get_rx_timestamp(rf_skiq_card_t* q, uint32_t port_idx)
{
  if (q->suspend || q->rx_ports[port_idx].rb_overflow) {
    return 0UL;
  }

  return rf_skiq_rx_port_get_timestamp(&q->rx_ports[port_idx]);
}

bool rf_skiq_card_is_streaming(rf_skiq_card_t* q)
{
  return q->state == RF_SKIQ_PORT_STATE_STREAMING && !q->suspend;
}

int rf_skiq_card_send(rf_skiq_card_t* q, uint32_t port_idx, const cf_t* data, uint32_t nsamples, uint64_t timestamp)
{
  // If suspended, do not bother the transmitter
  if (q->suspend) {
    return nsamples;
  }

  return rf_skiq_tx_port_send(&q->tx_ports[port_idx], data, nsamples, timestamp);
}
