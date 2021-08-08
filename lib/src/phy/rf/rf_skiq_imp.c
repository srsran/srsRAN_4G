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

#include <unistd.h>

#include <sidekiq_api.h>

#include "rf_helper.h"
#include "rf_skiq_imp.h"
#include "rf_skiq_imp_card.h"

/**
 * According the document Sidekiq API 4.13.0 @ref AD9361TimestampSlip:
 * Functions that will affect the timestamp:
 * • skiq_write_rx_LO_freq()
 * • skiq_write_rx_sample_rate_and_bandwidth()
 * • skiq_write_tx_LO_freq()
 * • skiq_run_tx_quadcal()
 * • skiq_write_rx_freq_tune_mode()
 * • skiq_write_tx_freq_tune_mode()
 * Functions that will be affected by the timestamp slip:
 * • skiq_read_last_1pps_timestamp()
 * • skiq_receive()
 * • skiq_transmit()
 * • skiq_read_curr_rx_timestamp()
 * • skiq_read_curr_tx_timestamp()
 *
 * The functions mentioned on the first group above can be divided in two groups. The first group are the ones that
 * require restart the tx/rx streams of cards:
 * • skiq_write_rx_sample_rate_and_bandwidth()
 *
 * The module assumes:
 * - Tx and Rx sampling rates are equal
 * - Tx/Rx shall be stopped during the configuration
 * - skiq_stop_rx_streaming_multi_immediate can be called while skiq_receive is being executed
 * - skiq_receive shall not be called while skiq_stop_rx_streaming_multi_immediate
 *
 * In order to update the sampling rate, the RF module shall:
 * - Stop all cards Rx streams
 * - Stop all cards Tx streams
 * - Update Tx/Rx sample rates
 * - Start Rx stream
 * - enable Tx stream on the next transmission
 *
 * The second group do not require restarting the tx/rx streams. Indeed, they only affect to a single card and there is
 * no interest on stalling the rest of cards stream. Because of this, the module shall suspend the affected card.
 * • skiq_write_rx_LO_freq()
 * • skiq_write_tx_LO_freq()
 *
 * The module assumes:
 * - The Tx/Rx LO frequency is changed for a single card
 * - The Tx/Rx is stalled only in selected card
 * - The rest of cards shall keep operating without stalling their streams
 *
 * In order to update the Tx/Rx LO frequencies, the RF module shall:
 * - Suspend the Tx/Rx streams of the card:
 *   - If receive port ring-buffer has samples, the module shall keep reading from ringbuffer;
 *   - Otherwise, the module shall not read from ring-buffer and write zeros in the receive buffer
 * - Set the Tx/Rx LO frequency
 * - Resume the reception
 *
 */

uint32_t rf_skiq_logging_level = SKIQ_LOG_INFO;

typedef struct {
  uint32_t nof_cards;
  uint32_t nof_ports;

  rf_skiq_card_t cards[SKIQ_MAX_NUM_CARDS];

  float cur_tx_gain;

  srsran_rf_info_t info;

  uint64_t next_tstamp;

  double current_srate_hz;
  cf_t   dummy_buffer[RF_SKIQ_DUMMY_BUFFER_SIZE];

  pthread_mutex_t mutex_rx; ///< Makes sure receive function and sampling rate setter are not running simultaneously

} rf_skiq_handler_t;

void rf_skiq_suppress_stdout(void* h)
{
  SKIQ_RF_INFO("Suppressing stdout... lowering logging level to warning\n");
  rf_skiq_logging_level = SKIQ_LOG_WARNING;
}

static bool rf_skiq_is_streaming(rf_skiq_handler_t* h)
{
  // If a single card is streaming, return true
  for (uint32_t i = 0; i < h->nof_cards; i++) {
    if (rf_skiq_card_is_streaming(&h->cards[i])) {
      return true;
    }
  }

  return false;
}

bool rf_skiq_check_synch(rf_skiq_handler_t* h)
{
  // Get first card system timestamp
  int64_t ts0_sys = (int64_t)rf_skiq_card_read_sys_timestamp(&h->cards[0]);
  int64_t ts0_rf  = (int64_t)rf_skiq_card_read_rf_timestamp(&h->cards[0]);
  SKIQ_RF_INFO("  ... Card 0 TS(sys/rf)=%ld/%ld\n", ts0_sys, ts0_rf);

  bool pass = true;
  // Compare all the other card timestamps
  for (uint32_t i = 1; i < h->nof_cards; i++) {
    int64_t ts2_sys = (int64_t)rf_skiq_card_read_sys_timestamp(&h->cards[i]);
    int64_t ts2_rf  = (int64_t)rf_skiq_card_read_rf_timestamp(&h->cards[i]);

    // Use current sampling rate
    double srate = h->current_srate_hz;

    // If the current srate was not set (zero, nan or Inf), read it back from the first card
    if (!isnormal(srate)) {
      uint32_t srate_int = 0;
      if (skiq_read_rx_sample_rate(0, skiq_rx_hdl_A1, &srate_int, &srate)) {
        ERROR("Error reading sampling rate\n");
      }
    }

    // Make sure all cards system and RF timestamps are inside maximum allowed error window
    bool card_pass = labs(ts2_sys - ts0_sys) < SKIQ_CARD_SYNC_MAX_ERROR;
    card_pass      = card_pass && labs(ts2_rf - ts0_rf) < (int64_t)(srate / 2);

    // It is enough that a card does not pass to fail the check
    pass = pass && card_pass;

    SKIQ_RF_INFO("  ... Card %d TS(sys/rf)=(%ld/%ld) (%.4f/%.4f). %s\n",
                 i,
                 ts2_sys,
                 ts2_rf,
                 (double)labs(ts2_sys - ts0_sys) / (double)SKIQ_SYS_TIMESTAMP_FREQ,
                 (double)labs(ts2_rf - ts0_rf) / srate,
                 card_pass ? "Ok" : "KO");
  }

  return pass;
}

/**
 * Synchronizes single and multiple cards using the PPS signal. This function helper shall be used once at
 * initialization.
 *
 * @param h SKIQ driver handler
 * @return SRSRAN_SUCCESS if it is possible to synchronize boards, SRSRAN_ERROR otherwise
 */
static int rf_skiq_synch_cards(rf_skiq_handler_t* h)
{
  bool     do_1pps = h->nof_cards > 1; //< PPS is required when more than one card is used
  uint32_t trials  = 0;                //< Count PPS synchronization check trials

  // Try synchronizing timestamps of all cards up to 10 trials
  do {
    SKIQ_RF_INFO("Resetting system timestamp trial %d/%d\n", trials + 1, SKIQ_CARD_SYNCH_MAX_TRIALS);

    // Reset timestamp in next PPS
    for (int i = 0; i < h->nof_cards; i++) {
      // Sets the timestamps to the last Rx known time.
      if (rf_skiq_card_update_timestamp(
              &h->cards[i], do_1pps, h->cards->rx_ports->rb_tstamp_rem + h->current_srate_hz) != SRSRAN_SUCCESS) {
        return SRSRAN_ERROR;
      }
    }

    // It could be that one or more cards PPS reset was issued just after a PPS signal, so only if there is PPS
    // (multiple cards), verifies that all cards are synchronised on the same PPS
    if (!do_1pps) {
      return SRSRAN_SUCCESS;
    }

    // Wait for a second to pass
    SKIQ_RF_INFO("  ... Waiting PPS to pass ...\n");
    sleep(1);
    SKIQ_RF_INFO("  ... Checking:\n");

    // Successful synchronization across boards!
    if (rf_skiq_check_synch(h)) {
      return SRSRAN_SUCCESS;
    }

    // Increment trial count
    trials++;
  } while (trials < SKIQ_CARD_SYNCH_MAX_TRIALS);

  // All trials have been consumed without a Successful synchronization
  ERROR("Error card synchronization failed\n");
  return SRSRAN_ERROR;
}

void rf_skiq_register_error_handler(void* h_, srsran_rf_error_handler_t error_handler, void* arg)
{
  rf_skiq_handler_t* h = (rf_skiq_handler_t*)h_;

  SKIQ_RF_INFO("Registering error handler...\n");

  // Set error handler for each card
  for (uint32_t i = 0; i < h->nof_cards; i++) {
    rf_skiq_card_set_error_handler(&h->cards[i], error_handler, arg);
  }
}

const char* rf_skiq_devname(void* h)
{
  return "Sidekiq";
}

int rf_skiq_start_rx_stream(void* h_, bool now)
{
  rf_skiq_handler_t* h = (rf_skiq_handler_t*)h_;

  rf_skiq_synch_cards(h);

  // Get current system timestamp, assume all cards are synchronized
  uint64_t ts = rf_skiq_card_read_sys_timestamp(&h->cards[0]);

  // Advance a 10th of a second (100ms)
  ts += SKIQ_SYS_TIMESTAMP_FREQ / 10;

  // Start streams for each card at the indicated timestamp...
  for (uint32_t i = 0; i < h->nof_cards; i++) {
    if (rf_skiq_card_start_rx_streaming(&h->cards[i], ts)) {
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

int rf_skiq_stop_rx_stream(void* h_)
{
  rf_skiq_handler_t* h = (rf_skiq_handler_t*)h_;

  for (int i = 0; i < h->nof_cards; i++) {
    rf_skiq_card_stop_rx_streaming(&h->cards[i]);
  }

  return SRSRAN_SUCCESS;
}

static int rf_skiq_send_end_of_burst(void* h_)
{
  rf_skiq_handler_t* h = (rf_skiq_handler_t*)h_;

  for (int i = 0; i < h->nof_cards; i++) {
    rf_skiq_card_end_of_burst(&h->cards[i]);
  }

  return SRSRAN_SUCCESS;
}

void rf_skiq_flush_buffer(void* h)
{
  SKIQ_RF_INFO("Flushing buffers...\n");

  int   n;
  void* data[SKIQ_MAX_CHANNELS] = {};
  do {
    n = rf_skiq_recv_with_time_multi(h, data, 1024, 0, NULL, NULL);
  } while (n > 0);
}

bool rf_skiq_has_rssi(void* h)
{
  return false;
}

float rf_skiq_get_rssi(void* h)
{
  return 0.0f;
}

int rf_skiq_open(char* args, void** h)
{
  return rf_skiq_open_multi(args, h, 1);
}

void rf_skiq_log_msg(int32_t priority, const char* message)
{
  if (priority <= rf_skiq_logging_level) {
    printf("%s", message);
  }
}

int rf_skiq_open_multi(char* args, void** h_, uint32_t nof_channels)
{
  // Check number of antennas bounds
  if (nof_channels < SKIQ_MIN_CHANNELS || nof_channels > SKIQ_MAX_CHANNELS) {
    ERROR("Number of channels (%d) not supported (%d-%d)\n", nof_channels, SKIQ_MIN_CHANNELS, SKIQ_MAX_CHANNELS);
    return SRSRAN_ERROR_OUT_OF_BOUNDS;
  }

  rf_skiq_handler_t* h = (rf_skiq_handler_t*)calloc(1, sizeof(rf_skiq_handler_t));
  if (!h) {
    return SRSRAN_ERROR;
  }
  *h_ = h;

  // Parse main parameters
  parse_uint32(args, "nof_cards", 0, &h->nof_cards);
  parse_uint32(args, "nof_ports", 0, &h->nof_ports);

  char log_level[RF_PARAM_LEN] = "info";
  parse_string(args, "log_level", 0, log_level);
  if (strcmp(log_level, "info") == 0) {
    rf_skiq_logging_level = SKIQ_LOG_INFO;
  } else if (strcmp(log_level, "debug") == 0) {
    rf_skiq_logging_level = SKIQ_LOG_DEBUG;
  } else if (strcmp(log_level, "warn") == 0) {
    rf_skiq_logging_level = SKIQ_LOG_WARNING;
  } else if (strcmp(log_level, "error") == 0) {
    rf_skiq_logging_level = SKIQ_LOG_ERROR;
  } else {
    ERROR("Error log_level %s is undefined. Options: debug, info, warn and error\n", log_level);
    return SRSRAN_ERROR;
  }

  // Register Logger
  skiq_register_logging(&rf_skiq_log_msg);

  // Get available cards
  uint8_t nof_available_cards                 = 0;
  uint8_t available_cards[SKIQ_MAX_NUM_CARDS] = {};
  if (skiq_get_cards(skiq_xport_type_auto, &nof_available_cards, available_cards)) {
    ERROR("Getting available cards\n");
    return SRSRAN_ERROR;
  }

  //
  if (h->nof_cards == 0 && h->nof_ports == 0) {
    if (nof_channels <= (uint32_t)nof_available_cards) {
      // One channel per card
      h->nof_cards = nof_channels;
      h->nof_ports = 1;
    } else if (nof_channels <= RF_SKIQ_MAX_PORTS_CARD) {
      // One channel per port
      h->nof_cards = 1;
      h->nof_ports = nof_channels;
    } else if (nof_channels % RF_SKIQ_MAX_PORTS_CARD == 0) {
      // use all ports
      h->nof_cards = nof_channels / RF_SKIQ_MAX_PORTS_CARD;
      h->nof_ports = RF_SKIQ_MAX_PORTS_CARD;
    } else if (nof_channels % nof_available_cards == 0) {
      // use all cards
      h->nof_cards = nof_available_cards;
      h->nof_ports = nof_channels / nof_available_cards;
    } else {
      ERROR("Error deducing the number of cards and ports");
    }
  } else if (h->nof_ports == 0 && nof_channels % h->nof_cards == 0) {
    h->nof_ports = nof_channels / h->nof_cards;
  } else if (h->nof_cards == 0 && nof_channels % h->nof_ports == 0) {
    h->nof_cards = nof_channels / h->nof_ports;
  }

  if (h->nof_cards == 0 || h->nof_cards > nof_available_cards) {
    ERROR("Error invalid number of cards %d, available %d\n", h->nof_cards, nof_available_cards);
    return SRSRAN_ERROR_OUT_OF_BOUNDS;
  }

  if (h->nof_ports == 0 || h->nof_ports > RF_SKIQ_MAX_PORTS_CARD) {
    ERROR("Error invalid number of cards %d, available %d\n", h->nof_cards, nof_available_cards);
    return SRSRAN_ERROR_OUT_OF_BOUNDS;
  }

  // Create default port options
  rf_skiq_port_opts_t port_opts = {};
  port_opts.tx_rb_size          = 2048;
  port_opts.rx_rb_size          = 2048;
  port_opts.chan_mode           = (h->nof_ports > 1) ? skiq_chan_mode_dual : skiq_chan_mode_single;
  port_opts.stream_mode         = skiq_rx_stream_mode_balanced;

  // Parse other options
  parse_uint32(args, "tx_rb_size", 0, &port_opts.tx_rb_size);
  parse_uint32(args, "rx_rb_size", 0, &port_opts.rx_rb_size);
  parse_string(args, "mode", 0, port_opts.stream_mode_str);

  if (strlen(port_opts.stream_mode_str) > 0) {
    if (strcmp(port_opts.stream_mode_str, "low_latency") == 0) {
      port_opts.stream_mode = skiq_rx_stream_mode_low_latency;
    } else if (strcmp(port_opts.stream_mode_str, "balanced") == 0) {
      port_opts.stream_mode = skiq_rx_stream_mode_balanced;
    } else if (strcmp(port_opts.stream_mode_str, "high_tput") == 0) {
      port_opts.stream_mode = skiq_rx_stream_mode_high_tput;
    } else {
      ERROR("Invalid mode: %s; Valid modes are: low_latency, balanced, high_tput\n", port_opts.stream_mode_str);
      return SRSRAN_ERROR;
    }
  }

  SKIQ_RF_INFO("Opening %d SKIQ cards with %d ports...\n", h->nof_cards, h->nof_ports);

  if (pthread_mutex_init(&h->mutex_rx, NULL)) {
    ERROR("Error initialising mutex\n");
    return SRSRAN_ERROR;
  }

  // Initialise driver
  if (skiq_init(skiq_xport_type_auto, skiq_xport_init_level_full, available_cards, h->nof_cards)) {
    ERROR("Unable to initialise libsidekiq driver\n");
    return SRSRAN_ERROR;
  }

  // Initialise each card
  for (uint32_t i = 0; i < h->nof_cards; i++) {
    if (rf_skiq_card_init(&h->cards[i], available_cards[i], h->nof_ports, &port_opts)) {
      return SRSRAN_ERROR;
    }
  }

  // Parse default frequencies
  for (uint32_t i = 0, ch = 0; i < h->nof_cards; i++) {
    for (uint32_t j = 0; j < h->nof_ports; j++, ch++) {
      double tx_freq = 0.0;
      parse_double(args, "tx_freq", ch, &tx_freq);

      if (isnormal(tx_freq)) {
        rf_skiq_set_tx_freq(h, ch, tx_freq);
      }
      double rx_freq = 0.0;
      parse_double(args, "rx_freq", ch, &rx_freq);

      if (isnormal(rx_freq)) {
        rf_skiq_set_rx_freq(h, ch, rx_freq);
      }
    }
  }

  // Set a default gain
  rf_skiq_set_rx_gain(h, SKIQ_RX_GAIN_DEFAULT_dB);

  // Parse default sample rate
  double srate_hz = 0.0;
  parse_double(args, "srate", 0, &srate_hz);
  srate_hz = isnormal(srate_hz) ? srate_hz : SKIQ_DEFAULT_SAMPLING_RATE_HZ;

  // Set a default sampling rate, default can be too low
  rf_skiq_set_tx_srate(h, srate_hz);
  rf_skiq_set_rx_srate(h, srate_hz);

  return SRSRAN_SUCCESS;
}

int rf_skiq_close(void* h_)
{
  rf_skiq_handler_t* h = (rf_skiq_handler_t*)h_;

  SKIQ_RF_INFO("Closing...\n");

  // Ensure Tx/Rx streaming is stopped
  rf_skiq_send_end_of_burst(h);
  rf_skiq_stop_rx_stream(h);

  // Free all open cards
  for (int i = 0; i < h->nof_cards; i++) {
    rf_skiq_card_close(&h->cards[i]);
  }

  // Close sidekiq SDK
  skiq_exit();

  pthread_mutex_destroy(&h->mutex_rx);

  // Deallocate object memory
  if (h != NULL) {
    free(h);
  }

  return SRSRAN_SUCCESS;
}

static double rf_skiq_set_srate_hz(rf_skiq_handler_t* h, double srate_hz)
{
  // If the sampling rate is not modified dont bother
  if (h->current_srate_hz == srate_hz) {
    return srate_hz;
  }
  SKIQ_RF_INFO("Setting sampling rate to %.2f MHz ...\n", srate_hz / 1e6);

  // Save streaming state
  bool is_streaming = rf_skiq_is_streaming(h);

  // Stop streaming
  SKIQ_RF_INFO("  ... Stop Tx/Rx streaming\n");
  rf_skiq_send_end_of_burst(h);
  rf_skiq_stop_rx_stream(h);

  // Set sampling
  SKIQ_RF_INFO("  ... Setting sampling rates to %.2f MHz\n", srate_hz / 1e6);
  pthread_mutex_lock(&h->mutex_rx);
  for (uint32_t i = 0; i < h->nof_cards; i++) {
    rf_skiq_card_set_srate_hz(&h->cards[i], (uint32_t)srate_hz);
  }
  pthread_mutex_unlock(&h->mutex_rx);

  // Start streaming if it was started
  if (is_streaming) {
    SKIQ_RF_INFO("  ... Start Rx streaming\n");
    rf_skiq_start_rx_stream(h, true);
  }

  // Update current sampling rate
  h->current_srate_hz = srate_hz;
  SKIQ_RF_INFO("  ... Done!\n");

  return srate_hz;
}

double rf_skiq_set_rx_srate(void* h, double sample_rate)
{
  return rf_skiq_set_srate_hz((rf_skiq_handler_t*)h, sample_rate);
}

double rf_skiq_set_tx_srate(void* h, double sample_rate)
{
  return rf_skiq_set_srate_hz((rf_skiq_handler_t*)h, sample_rate);
}

int rf_skiq_set_rx_gain(void* h_, double rx_gain)
{
  rf_skiq_handler_t* h = (rf_skiq_handler_t*)h_;

  for (uint32_t i = 0; i < h->nof_cards; i++) {
    rf_skiq_card_set_rx_gain_db(&h->cards[i], h->nof_ports, rx_gain);
  }

  return SRSRAN_SUCCESS;
}

int rf_skiq_set_tx_gain(void* h_, double tx_gain)
{
  rf_skiq_handler_t* h = (rf_skiq_handler_t*)h_;

  for (uint32_t i = 0; i < h->nof_cards; i++) {
    h->cur_tx_gain = rf_skiq_card_set_tx_gain_db(&h->cards[i], h->nof_ports, tx_gain);
  }

  return SRSRAN_SUCCESS;
}

int rf_skiq_set_tx_gain_ch(void* h_, uint32_t ch, double tx_gain)
{
  rf_skiq_handler_t* h        = (rf_skiq_handler_t*)h_;
  uint32_t           card_idx = ch / h->nof_ports;
  uint32_t           port_idx = ch % h->nof_ports;

  if (card_idx >= h->nof_cards || port_idx >= h->nof_ports) {
    return SRSRAN_ERROR_OUT_OF_BOUNDS;
  }

  tx_gain = rf_skiq_card_set_tx_gain_db(&h->cards[card_idx], port_idx, tx_gain);

  if (ch == 0) {
    h->cur_tx_gain = tx_gain;
  }

  return SRSRAN_SUCCESS;
}

int rf_skiq_set_rx_gain_ch(void* h_, uint32_t ch, double rx_gain)
{
  rf_skiq_handler_t* h        = (rf_skiq_handler_t*)h_;
  uint32_t           card_idx = ch / h->nof_ports;
  uint32_t           port_idx = ch % h->nof_ports;

  if (card_idx >= h->nof_cards || port_idx >= h->nof_ports) {
    return SRSRAN_ERROR_OUT_OF_BOUNDS;
  }

  rx_gain = rf_skiq_card_set_rx_gain_db(&h->cards[card_idx], port_idx, rx_gain);

  if (ch == 0) {
    h->cur_tx_gain = rx_gain;
  }

  return SRSRAN_SUCCESS;
}

double rf_skiq_get_rx_gain(void* h_)
{
  rf_skiq_handler_t* h = (rf_skiq_handler_t*)h_;

  return h->cards[0].cur_rx_gain_db;
}

double rf_skiq_get_tx_gain(void* h_)
{
  rf_skiq_handler_t* h = (rf_skiq_handler_t*)h_;
  return h->cur_tx_gain;
}

srsran_rf_info_t* rf_skiq_get_info(void* h_)
{
  srsran_rf_info_t*  ret = NULL;
  rf_skiq_handler_t* h   = (rf_skiq_handler_t*)h_;

  if (h != NULL) {
    ret = &h->info;

    rf_skiq_card_update_gain_table(&h->cards[0]);

    ret->min_tx_gain = 0.25 * (double)h->cards[0].param.tx_param->atten_quarter_db_max;
    ret->max_tx_gain = 0.25 * (double)h->cards[0].param.tx_param->atten_quarter_db_min;
    ret->min_rx_gain = h->cards[0].rx_gain_table_db[h->cards[0].param.rx_param[0].gain_index_min];
    ret->max_rx_gain = h->cards[0].rx_gain_table_db[h->cards[0].param.rx_param[0].gain_index_max];
  }

  return ret;
}

double rf_skiq_set_rx_freq(void* h_, uint32_t ch, double freq)
{
  rf_skiq_handler_t* h = (rf_skiq_handler_t*)h_;

  uint32_t card_idx = ch / h->nof_ports;
  uint32_t port_idx = ch % h->nof_ports;

#pragma message "TODO: The Rx stream needs to stop, RF timestamp shall be aligned with other cards and start again"

  if (card_idx < h->nof_cards && port_idx < h->nof_ports) {
    return rf_skiq_card_set_rx_freq_hz(&h->cards[card_idx], port_idx, freq);
  }

  return freq;
}

double rf_skiq_set_tx_freq(void* h_, uint32_t ch, double freq)
{
  rf_skiq_handler_t* h = (rf_skiq_handler_t*)h_;

  uint32_t card_idx = ch / h->nof_ports;
  uint32_t port_idx = ch % h->nof_ports;

#pragma message "TODO: The Rx stream needs to stop, RF timestamp shall be aligned with other cards and start again"

  if (card_idx < h->nof_cards && port_idx < h->nof_ports) {
    return rf_skiq_card_set_tx_freq_hz(&h->cards[card_idx], port_idx, freq);
  }

  return freq;
}

void tstamp_to_time(rf_skiq_handler_t* h, uint64_t tstamp, time_t* secs, double* frac_secs)
{
  uint64_t srate_hz = (uint64_t)h->current_srate_hz;

  if (srate_hz == 0) {
    ERROR("Warning: Sampling rate has not been set yet.\n");
    srate_hz = UINT64_MAX;
  }

  if (secs) {
    *secs = (time_t)tstamp / srate_hz;
  }
  if (frac_secs) {
    uint64_t rem = tstamp % srate_hz;
    *frac_secs   = (double)rem / h->current_srate_hz;
  }
}

uint64_t time_to_tstamp(rf_skiq_handler_t* h, time_t secs, double frac_secs)
{
  return secs * h->current_srate_hz + frac_secs * h->current_srate_hz;
}

void rf_skiq_get_time(void* h_, time_t* secs, double* frac_secs)
{
  rf_skiq_handler_t* h      = (rf_skiq_handler_t*)h_;
  uint64_t           tstamp = rf_skiq_card_read_rf_timestamp(&h->cards[0]);
  tstamp_to_time(h, tstamp, secs, frac_secs);
}

static int
rf_skiq_discard_rx_samples(rf_skiq_handler_t* h, uint32_t card, uint32_t port, uint32_t nsamples, uint64_t* ts_start)
{
  *ts_start = 0;
  while (nsamples > 0) {
    uint64_t ts = 0;

    // Receive in dummy buffer
    int32_t n = rf_skiq_card_receive(
        &h->cards[card], port, h->dummy_buffer, SRSRAN_MIN(nsamples, RF_SKIQ_DUMMY_BUFFER_SIZE), &ts);

    // Check for error
    if (n < 0) {
      ERROR("An error occurred discarding %d Rx samples for channel %d:%d\n", nsamples, card, port);
      return n;
    }

    // Save first timestamp
    if (*ts_start == 0) {
      *ts_start = ts;
    }

    // Decrement pending samples
    nsamples -= n;
  }

  return nsamples;
}

static int rf_skiq_synch_rx_ports(rf_skiq_handler_t* h)
{
  int64_t tstamp_min = INT64_MAX;
  int64_t tstamp_max = 0;

  // no  need to synchronize
  if (h->nof_cards * h->nof_ports < 2) {
    return SRSRAN_SUCCESS;
  }

  // Find minimum and maximum next timestamps
  for (uint32_t card = 0; card < h->nof_cards; card++) {
    // Iterate for all ports
    for (uint32_t port = 0; port < h->nof_ports; port++) {
      int64_t ts = (int64_t)rf_skiq_card_get_rx_timestamp(&h->cards[card], port);

      // If the card is not streaming or suspended will return TS 0; so skip
      if (ts == 0UL) {
        continue;
      }

      // Is minimum?
      tstamp_min = SRSRAN_MIN(tstamp_min, ts);

      // Is maximum?
      tstamp_max = SRSRAN_MAX(tstamp_max, ts);
    }
  }

  // Check if any synchronization is required
  if (tstamp_max == tstamp_min) {
    return SRSRAN_SUCCESS;
  }

  // Align all channels to the maximum timestamp
  for (uint32_t card = 0; card < h->nof_cards; card++) {
    // Iterate for all ports
    for (uint32_t port = 0; port < h->nof_ports; port++) {
      uint64_t ts = rf_skiq_card_get_rx_timestamp(&h->cards[card], port);

      // If the card is not streaming or suspended will return TS 0; so skip
      if (ts == 0UL) {
        continue;
      }

      // Calculate number of samples
      int nsamples = (int)(tstamp_max - (int64_t)ts);

      // Skip port if negative or zero (possible if stream is enabled during this time)
      if (nsamples <= 0) {
        continue;
      }

      // Too many samples, sign of some extreme error
      if (nsamples > SKIQ_PORT_SYNC_MAX_GAP) {
        ERROR("too many samples to align (%d) for channel %d:%d\n", nsamples, card, port);
        return SRSRAN_ERROR;
      }

      ts = 0;
      if (rf_skiq_discard_rx_samples(h, card, port, nsamples, &ts) < SRSRAN_SUCCESS) {
        ERROR("Error occurred during Rx streams alignment.");
        return SRSRAN_ERROR;
      }
    }
  }

  return SRSRAN_SUCCESS;
}

int rf_skiq_recv_with_time(void* h, void* data, uint32_t nsamples, bool blocking, time_t* secs, double* frac_secs)
{
  return rf_skiq_recv_with_time_multi(h, &data, nsamples, blocking, secs, frac_secs);
}

int rf_skiq_recv_with_time_multi(void*    h_,
                                 void**   data_,
                                 uint32_t nsamples,
                                 bool     blocking,
                                 time_t*  secs,
                                 double*  frac_secs)
{
  rf_skiq_handler_t* h        = (rf_skiq_handler_t*)h_;
  uint64_t           ts_start = 0;
  cf_t**             data     = (cf_t**)data_;

  pthread_mutex_lock(&h->mutex_rx);

  // Perform channel synchronization
  rf_skiq_synch_rx_ports(h);

  bool     completed                = false;
  uint32_t count[SKIQ_MAX_CHANNELS] = {};

  while (!completed) {
    // Completion true by default
    completed = true;

    // Iterate over cards
    for (uint32_t card = 0, chan = 0; card < h->nof_cards; card++) {
      // Iterate over ports
      for (uint32_t port = 0; port < h->nof_ports; port++, chan++) {
        // Calculate number of pending samples
        uint32_t pending = nsamples - count[chan];

        if (pending > 0) {
          uint64_t ts = 0;
          int      n  = 0;

          // If data is not provided...
          if (data[chan] == NULL) {
            // ... discard up to RF_SKIQ_DUMMY_BUFFER_SIZE samples
            n = rf_skiq_card_receive(
                &h->cards[card], port, h->dummy_buffer, SRSRAN_MIN(pending, RF_SKIQ_DUMMY_BUFFER_SIZE), &ts);
          } else {
            // ... read base-band
            n = rf_skiq_card_receive(&h->cards[card], port, &data[chan][count[chan]], pending, &ts);
          }

          // If error is detected, return it
          if (n < SRSRAN_SUCCESS) {
            pthread_mutex_unlock(&h->mutex_rx);
            return n;
          }

          // Save first valid timestamp
          if (ts_start == 0) {
            ts_start = ts;
          }

          // Increment count for the channel
          count[chan] += n;

          // Lower completed flag if at least a channel has not reach the target
          if (count[chan] < nsamples) {
            completed = false;
          }
        }
      }
    }
  }

  pthread_mutex_unlock(&h->mutex_rx);

  // Convert u64 to srsran timestamp
  tstamp_to_time(h, ts_start, secs, frac_secs);

  // No error, return number of received samples
  return nsamples;
}

int rf_skiq_send_timed(void*  h,
                       void*  data,
                       int    nsamples,
                       time_t secs,
                       double frac_secs,
                       bool   has_time_spec,
                       bool   blocking,
                       bool   is_start_of_burst,
                       bool   is_end_of_burst)
{
  void* _data[SRSRAN_MAX_PORTS] = {};
  _data[0]                      = data;

  return rf_skiq_send_timed_multi(
      h, _data, nsamples, secs, frac_secs, has_time_spec, blocking, is_start_of_burst, is_end_of_burst);
}

int rf_skiq_send_timed_multi(void*  h_,
                             void** data_,
                             int    nsamples,
                             time_t secs,
                             double frac_secs,
                             bool   has_time_spec,
                             bool   blocking,
                             bool   is_start_of_burst,
                             bool   is_end_of_burst)
{
  rf_skiq_handler_t* h    = (rf_skiq_handler_t*)h_;
  cf_t**             data = (cf_t**)data_;

  // Force timestamp only if start of burst
  if (is_start_of_burst) {
    if (has_time_spec) {
      h->next_tstamp = time_to_tstamp(h, secs, frac_secs);
      SKIQ_RF_DEBUG("[Tx SOB] ts=%ld\n", h->next_tstamp);
    } else {
      h->next_tstamp = rf_skiq_card_read_rf_timestamp(&h->cards[0]);
      h->next_tstamp += (uint64_t)round(h->current_srate_hz / 10); // increment a 10th of a second
    }
  }

  uint32_t rpm                      = 0;
  bool     completed                = false;
  uint32_t count[SKIQ_MAX_CHANNELS] = {};

  while (!completed) {
    // Completion true by default
    completed = true;

    // Iterate all cards...
    for (uint32_t card = 0, chan = 0; card < h->nof_cards; card++) {
      // Iterate all ports...
      for (uint32_t port = 0; port < h->nof_ports; port++, chan++) {
        // Calculate number of pending samples
        uint32_t pending = nsamples - count[chan];

        if (pending > 0) {
          uint64_t ts  = h->next_tstamp + count[chan];
          cf_t*    ptr = (data[chan] == NULL) ? NULL : &data[chan][count[chan]];
          int      n   = rf_skiq_card_send(&h->cards[card], port, ptr, pending, ts);
          if (n > SRSRAN_SUCCESS) {
            count[chan] += n;
          }

          if (count[chan] < nsamples) {
            completed = false;
          }
        }
      }
    }
  }

  // Increment timestamp
  h->next_tstamp += nsamples;

  if (is_end_of_burst) {
    rf_skiq_send_end_of_burst(h);
  }

  return (int)rpm;
}
