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

#include <libbladeRF.h>
#include <string.h>
#include <unistd.h>

#include "rf_blade_imp.h"
#include "srslte/srslte.h"

#define UNUSED __attribute__((unused))
#define CONVERT_BUFFER_SIZE (240 * 1024)

typedef struct {
  struct bladerf*     dev;
  bladerf_sample_rate rx_rate;
  bladerf_sample_rate tx_rate;
  int16_t             rx_buffer[CONVERT_BUFFER_SIZE];
  int16_t             tx_buffer[CONVERT_BUFFER_SIZE];
  bool                rx_stream_enabled;
  bool                tx_stream_enabled;
  srslte_rf_info_t    info;
} rf_blade_handler_t;

static srslte_rf_error_handler_t blade_error_handler     = NULL;
static void*                     blade_error_handler_arg = NULL;

void rf_blade_suppress_stdout(UNUSED void* h)
{
  bladerf_log_set_verbosity(BLADERF_LOG_LEVEL_SILENT);
}

void rf_blade_register_error_handler(UNUSED void* ptr, srslte_rf_error_handler_t new_handler, void* arg)
{
  blade_error_handler     = new_handler;
  blade_error_handler_arg = arg;
}

const unsigned int num_buffers       = 256;
const unsigned int ms_buffer_size_rx = 1024;
const unsigned int buffer_size_tx    = 1024;
const unsigned int num_transfers     = 32;
const unsigned int timeout_ms        = 4000;

const char* rf_blade_devname(UNUSED void* h)
{
  return DEVNAME;
}

int rf_blade_start_tx_stream(void* h)
{
  int                 status;
  rf_blade_handler_t* handler = (rf_blade_handler_t*)h;

  status = bladerf_sync_config(handler->dev,
                               BLADERF_TX_X1,
                               BLADERF_FORMAT_SC16_Q11_META,
                               num_buffers,
                               buffer_size_tx,
                               num_transfers,
                               timeout_ms);
  if (status != 0) {
    ERROR("Failed to configure TX sync interface: %s\n", bladerf_strerror(status));
    return status;
  }
  status = bladerf_enable_module(handler->dev, BLADERF_TX_X1, true);
  if (status != 0) {
    ERROR("Failed to enable TX module: %s\n", bladerf_strerror(status));
    return status;
  }
  handler->tx_stream_enabled = true;
  return 0;
}

int rf_blade_start_rx_stream(void* h, UNUSED bool now)
{
  int                 status;
  rf_blade_handler_t* handler = (rf_blade_handler_t*)h;

  /* Configure the device's RX module for use with the sync interface.
   * SC16 Q11 samples *with* metadata are used. */
  uint32_t buffer_size_rx = ms_buffer_size_rx * (handler->rx_rate / 1000 / 1024);

  status = bladerf_sync_config(handler->dev,
                               BLADERF_RX_X1,
                               BLADERF_FORMAT_SC16_Q11_META,
                               num_buffers,
                               buffer_size_rx,
                               num_transfers,
                               timeout_ms);
  if (status != 0) {
    ERROR("Failed to configure RX sync interface: %s\n", bladerf_strerror(status));
    return status;
  }
  status = bladerf_sync_config(handler->dev,
                               BLADERF_TX_X1,
                               BLADERF_FORMAT_SC16_Q11_META,
                               num_buffers,
                               buffer_size_tx,
                               num_transfers,
                               timeout_ms);
  if (status != 0) {
    ERROR("Failed to configure TX sync interface: %s\n", bladerf_strerror(status));
    return status;
  }
  status = bladerf_enable_module(handler->dev, BLADERF_RX_X1, true);
  if (status != 0) {
    ERROR("Failed to enable RX module: %s\n", bladerf_strerror(status));
    return status;
  }
  status = bladerf_enable_module(handler->dev, BLADERF_TX_X1, true);
  if (status != 0) {
    ERROR("Failed to enable TX module: %s\n", bladerf_strerror(status));
    return status;
  }
  handler->rx_stream_enabled = true;
  return 0;
}

int rf_blade_stop_rx_stream(void* h)
{
  rf_blade_handler_t* handler = (rf_blade_handler_t*)h;
  int                 status  = bladerf_enable_module(handler->dev, BLADERF_RX_X1, false);
  if (status != 0) {
    ERROR("Failed to enable RX module: %s\n", bladerf_strerror(status));
    return status;
  }
  status = bladerf_enable_module(handler->dev, BLADERF_TX_X1, false);
  if (status != 0) {
    ERROR("Failed to enable TX module: %s\n", bladerf_strerror(status));
    return status;
  }
  handler->rx_stream_enabled = false;
  handler->tx_stream_enabled = false;
  return 0;
}

void rf_blade_flush_buffer(UNUSED void* h) {}

bool rf_blade_has_rssi(UNUSED void* h)
{
  return false;
}

float rf_blade_get_rssi(UNUSED void* h)
{
  return 0;
}

int rf_blade_open_multi(char* args, void** h, UNUSED uint32_t nof_channels)
{
  return rf_blade_open(args, h);
}

int rf_blade_open(char* args, void** h)
{
  const struct bladerf_range* range_tx = NULL;
  const struct bladerf_range* range_rx = NULL;
  *h                                   = NULL;

  rf_blade_handler_t* handler = (rf_blade_handler_t*)malloc(sizeof(rf_blade_handler_t));
  if (!handler) {
    perror("malloc");
    return -1;
  }
  *h = handler;

  printf("Opening bladeRF...\n");
  int status = bladerf_open(&handler->dev, args);
  if (status) {
    ERROR("Unable to open device: %s\n", bladerf_strerror(status));
    goto clean_exit;
  }

  status = bladerf_set_gain_mode(handler->dev, BLADERF_RX_X1, BLADERF_GAIN_MGC);
  if (status) {
    ERROR("Unable to open device: %s\n", bladerf_strerror(status));
    goto clean_exit;
  }

  // bladerf_log_set_verbosity(BLADERF_LOG_LEVEL_VERBOSE);

  /* Get Gain ranges and set Rx to maximum */
  status = bladerf_get_gain_range(handler->dev, BLADERF_RX_X1, &range_rx);
  if ((status != 0) || (range_rx == NULL)) {
    ERROR("Failed to get RX gain range: %s\n", bladerf_strerror(status));
    goto clean_exit;
  }

  status = bladerf_get_gain_range(handler->dev, BLADERF_TX_X1, &range_tx);
  if ((status != 0) || (range_tx == NULL)) {
    ERROR("Failed to get TX gain range: %s\n", bladerf_strerror(status));
    goto clean_exit;
  }

  status = bladerf_set_gain(handler->dev, BLADERF_RX_X1, (bladerf_gain)range_rx->max);
  if (status != 0) {
    ERROR("Failed to set RX LNA gain: %s\n", bladerf_strerror(status));
    goto clean_exit;
  }
  handler->rx_stream_enabled = false;
  handler->tx_stream_enabled = false;

  /* Set default sampling rates */
  rf_blade_set_tx_srate(handler, 1.92e6);
  rf_blade_set_rx_srate(handler, 1.92e6);

  /* Set info structure */
  handler->info.min_tx_gain = range_tx->min;
  handler->info.max_tx_gain = range_tx->max;
  handler->info.min_rx_gain = range_rx->min;
  handler->info.max_rx_gain = range_rx->max;

  return SRSLTE_SUCCESS;

clean_exit:
  free(handler);
  return status;
}

int rf_blade_close(void* h)
{
  rf_blade_handler_t* handler = (rf_blade_handler_t*)h;
  bladerf_close(handler->dev);
  return 0;
}

double rf_blade_set_rx_srate(void* h, double freq)
{
  uint32_t            bw;
  rf_blade_handler_t* handler = (rf_blade_handler_t*)h;
  int                 status  = bladerf_set_sample_rate(handler->dev, BLADERF_RX_X1, (uint32_t)freq, &handler->rx_rate);
  if (status != 0) {
    ERROR("Failed to set samplerate = %u: %s\n", (uint32_t)freq, bladerf_strerror(status));
    return -1;
  }
  if (handler->rx_rate < 2000000) {
    status = bladerf_set_bandwidth(handler->dev, BLADERF_RX_X1, handler->rx_rate, &bw);
    if (status != 0) {
      ERROR("Failed to set bandwidth = %u: %s\n", handler->rx_rate, bladerf_strerror(status));
      return -1;
    }
  } else {
    status = bladerf_set_bandwidth(handler->dev, BLADERF_RX_X1, (bladerf_bandwidth)(handler->rx_rate * 0.8), &bw);
    if (status != 0) {
      ERROR("Failed to set bandwidth = %u: %s\n", handler->rx_rate, bladerf_strerror(status));
      return -1;
    }
  }
  printf("Set RX sampling rate %.2f Mhz, filter BW: %.2f Mhz\n", (float)handler->rx_rate / 1e6, (float)bw / 1e6);
  return (double)handler->rx_rate;
}

double rf_blade_set_tx_srate(void* h, double freq)
{
  uint32_t            bw;
  rf_blade_handler_t* handler = (rf_blade_handler_t*)h;
  int                 status  = bladerf_set_sample_rate(handler->dev, BLADERF_TX_X1, (uint32_t)freq, &handler->tx_rate);
  if (status != 0) {
    ERROR("Failed to set samplerate = %u: %s\n", (uint32_t)freq, bladerf_strerror(status));
    return -1;
  }
  status = bladerf_set_bandwidth(handler->dev, BLADERF_TX_X1, handler->tx_rate, &bw);
  if (status != 0) {
    ERROR("Failed to set bandwidth = %u: %s\n", handler->tx_rate, bladerf_strerror(status));
    return -1;
  }
  return (double)handler->tx_rate;
}

int rf_blade_set_rx_gain(void* h, double gain)
{
  int                 status;
  rf_blade_handler_t* handler = (rf_blade_handler_t*)h;
  status                      = bladerf_set_gain(handler->dev, BLADERF_RX_X1, (bladerf_gain)gain);
  if (status != 0) {
    ERROR("Failed to set RX gain: %s\n", bladerf_strerror(status));
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

int rf_blade_set_rx_gain_ch(void* h, uint32_t ch, double gain)
{
  return rf_blade_set_rx_gain(h, gain);
}

int rf_blade_set_tx_gain(void* h, double gain)
{
  int                 status;
  rf_blade_handler_t* handler = (rf_blade_handler_t*)h;
  status                      = bladerf_set_gain(handler->dev, BLADERF_TX_X1, (bladerf_gain)gain);
  if (status != 0) {
    ERROR("Failed to set TX gain: %s\n", bladerf_strerror(status));
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

int rf_blade_set_tx_gain_ch(void* h, uint32_t ch, double gain)
{
  return rf_blade_set_tx_gain(h, gain);
}

double rf_blade_get_rx_gain(void* h)
{
  int                 status;
  bladerf_gain        gain    = 0;
  rf_blade_handler_t* handler = (rf_blade_handler_t*)h;
  status                      = bladerf_get_gain(handler->dev, BLADERF_RX_X1, &gain);
  if (status != 0) {
    ERROR("Failed to get RX gain: %s\n", bladerf_strerror(status));
    return -1;
  }
  return gain;
}

double rf_blade_get_tx_gain(void* h)
{
  int                 status;
  bladerf_gain        gain    = 0;
  rf_blade_handler_t* handler = (rf_blade_handler_t*)h;
  status                      = bladerf_get_gain(handler->dev, BLADERF_TX_X1, &gain);
  if (status != 0) {
    ERROR("Failed to get TX gain: %s\n", bladerf_strerror(status));
    return -1;
  }
  return gain;
}

srslte_rf_info_t* rf_blade_get_info(void* h)
{

  srslte_rf_info_t* info = NULL;

  if (h) {
    rf_blade_handler_t* handler = (rf_blade_handler_t*)h;

    info = &handler->info;
  }
  return info;
}

double rf_blade_set_rx_freq(void* h, UNUSED uint32_t ch, double freq)
{
  rf_blade_handler_t* handler = (rf_blade_handler_t*)h;
  bladerf_frequency   f_int   = (uint32_t)round(freq);
  int                 status  = bladerf_set_frequency(handler->dev, BLADERF_RX_X1, f_int);
  if (status != 0) {
    ERROR("Failed to set samplerate = %u: %s\n", (uint32_t)freq, bladerf_strerror(status));
    return -1;
  }
  f_int = 0;
  bladerf_get_frequency(handler->dev, BLADERF_RX_X1, &f_int);
  printf("set RX frequency to %lu\n", f_int);

  return freq;
}

double rf_blade_set_tx_freq(void* h, UNUSED uint32_t ch, double freq)
{
  rf_blade_handler_t* handler = (rf_blade_handler_t*)h;
  bladerf_frequency   f_int   = (uint32_t)round(freq);
  int                 status  = bladerf_set_frequency(handler->dev, BLADERF_TX_X1, f_int);
  if (status != 0) {
    ERROR("Failed to set samplerate = %u: %s\n", (uint32_t)freq, bladerf_strerror(status));
    return -1;
  }

  f_int = 0;
  bladerf_get_frequency(handler->dev, BLADERF_TX_X1, &f_int);
  printf("set TX frequency to %lu\n", f_int);
  return freq;
}

static void timestamp_to_secs(uint32_t rate, uint64_t timestamp, time_t* secs, double* frac_secs)
{
  double totalsecs = (double)timestamp / rate;
  time_t secs_i    = (time_t)totalsecs;
  if (secs) {
    *secs = secs_i;
  }
  if (frac_secs) {
    *frac_secs = totalsecs - secs_i;
  }
}

void rf_blade_get_time(void* h, time_t* secs, double* frac_secs)
{
  rf_blade_handler_t*     handler = (rf_blade_handler_t*)h;
  struct bladerf_metadata meta;

  int status = bladerf_get_timestamp(handler->dev, BLADERF_RX, &meta.timestamp);
  if (status != 0) {
    ERROR("Failed to get current RX timestamp: %s\n", bladerf_strerror(status));
  }
  timestamp_to_secs(handler->rx_rate, meta.timestamp, secs, frac_secs);
}

int rf_blade_recv_with_time_multi(void*    h,
                                  void**   data,
                                  uint32_t nsamples,
                                  bool     blocking,
                                  time_t*  secs,
                                  double*  frac_secs)
{
  return rf_blade_recv_with_time(h, *data, nsamples, blocking, secs, frac_secs);
}

int rf_blade_recv_with_time(void*       h,
                            void*       data,
                            uint32_t    nsamples,
                            UNUSED bool blocking,
                            time_t*     secs,
                            double*     frac_secs)
{
  rf_blade_handler_t*     handler = (rf_blade_handler_t*)h;
  struct bladerf_metadata meta;
  int                     status;

  memset(&meta, 0, sizeof(meta));
  meta.flags = BLADERF_META_FLAG_RX_NOW;

  if (2 * nsamples > CONVERT_BUFFER_SIZE) {
    ERROR("RX failed: nsamples exceeds buffer size (%d>%d)\n", nsamples, CONVERT_BUFFER_SIZE);
    return -1;
  }
  status = bladerf_sync_rx(handler->dev, handler->rx_buffer, nsamples, &meta, 2000);
  if (status) {
    ERROR("RX failed: %s; nsamples=%d;\n", bladerf_strerror(status), nsamples);
    return -1;
  } else if (meta.status & BLADERF_META_STATUS_OVERRUN) {
    if (blade_error_handler) {
      srslte_rf_error_t error;
      error.opt  = meta.actual_count;
      error.type = SRSLTE_RF_ERROR_OVERFLOW;
      blade_error_handler(blade_error_handler_arg, error);
    } else {
      /*ERROR("Overrun detected in scheduled RX. "
            "%u valid samples were read.\n\n", meta.actual_count);*/
    }
  }

  timestamp_to_secs(handler->rx_rate, meta.timestamp, secs, frac_secs);
  srslte_vec_convert_if(handler->rx_buffer, 2048, data, 2 * nsamples);

  return nsamples;
}

int rf_blade_send_timed_multi(void*  h,
                              void*  data[4],
                              int    nsamples,
                              time_t secs,
                              double frac_secs,
                              bool   has_time_spec,
                              bool   blocking,
                              bool   is_start_of_burst,
                              bool   is_end_of_burst)
{
  return rf_blade_send_timed(
      h, data[0], nsamples, secs, frac_secs, has_time_spec, blocking, is_start_of_burst, is_end_of_burst);
}

int rf_blade_send_timed(void*       h,
                        void*       data,
                        int         nsamples,
                        time_t      secs,
                        double      frac_secs,
                        bool        has_time_spec,
                        UNUSED bool blocking,
                        bool        is_start_of_burst,
                        bool        is_end_of_burst)
{
  rf_blade_handler_t*     handler = (rf_blade_handler_t*)h;
  struct bladerf_metadata meta;
  int                     status;

  if (!handler->tx_stream_enabled) {
    rf_blade_start_tx_stream(h);
  }

  if (2 * nsamples > CONVERT_BUFFER_SIZE) {
    ERROR("TX failed: nsamples exceeds buffer size (%d>%d)\n", nsamples, CONVERT_BUFFER_SIZE);
    return -1;
  }

  srslte_vec_convert_fi(data, 2048, handler->tx_buffer, 2 * nsamples);

  memset(&meta, 0, sizeof(meta));
  if (is_start_of_burst) {
    if (has_time_spec) {
      // Convert time to ticks
      srslte_timestamp_t ts = {.full_secs = secs, .frac_secs = frac_secs};
      meta.timestamp        = srslte_timestamp_uint64(&ts, handler->tx_rate);
    } else {
      meta.flags |= BLADERF_META_FLAG_TX_NOW;
    }
    meta.flags |= BLADERF_META_FLAG_TX_BURST_START;
  }
  if (is_end_of_burst) {
    meta.flags |= BLADERF_META_FLAG_TX_BURST_END;
  }
  srslte_rf_error_t error;
  bzero(&error, sizeof(srslte_rf_error_t));

  status = bladerf_sync_tx(handler->dev, handler->tx_buffer, nsamples, &meta, 2000);
  if (status == BLADERF_ERR_TIME_PAST) {
    if (blade_error_handler) {
      error.type = SRSLTE_RF_ERROR_LATE;
      blade_error_handler(blade_error_handler_arg, error);
    } else {
      ERROR("TX failed: %s\n", bladerf_strerror(status));
    }
  } else if (status) {
    ERROR("TX failed: %s\n", bladerf_strerror(status));
    return status;
  } else if (meta.status == BLADERF_META_STATUS_UNDERRUN) {
    if (blade_error_handler) {
      error.type = SRSLTE_RF_ERROR_UNDERFLOW;
      blade_error_handler(blade_error_handler_arg, error);
    } else {
      ERROR("TX warning: underflow detected.\n");
    }
  }

  return nsamples;
}
