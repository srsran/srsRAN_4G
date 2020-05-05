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

#include "rf_zmq_imp.h"
#include "rf_helper.h"
#include "rf_zmq_imp_trx.h"
#include <math.h>
#include <srslte/phy/common/phy_common.h>
#include <srslte/phy/common/timestamp.h>
#include <srslte/phy/utils/vector.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <zmq.h>

typedef struct {
  // Common attributes
  char*            devname;
  srslte_rf_info_t info;
  uint32_t         nof_channels;

  // RF State
  uint32_t srate; // radio rate configured by upper layers
  uint32_t base_srate;
  uint32_t decim_factor; // decimation factor between base_srate used on transport on radio's rate
  double   rx_gain;
  uint32_t tx_freq_mhz[SRSLTE_MAX_CHANNELS];
  uint32_t rx_freq_mhz[SRSLTE_MAX_CHANNELS];
  bool     tx_off;
  char     id[RF_PARAM_LEN];

  // Server
  void*       context;
  rf_zmq_tx_t transmitter[SRSLTE_MAX_CHANNELS];
  rf_zmq_rx_t receiver[SRSLTE_MAX_CHANNELS];

  // Various sample buffers
  cf_t* buffer_decimation[SRSLTE_MAX_CHANNELS];
  cf_t* buffer_tx;

  // Rx timestamp
  uint64_t next_rx_ts;

  pthread_mutex_t tx_config_mutex;
  pthread_mutex_t rx_config_mutex;
  pthread_mutex_t decim_mutex;
} rf_zmq_handler_t;

void update_rates(rf_zmq_handler_t* handler, double srate);

/*
 * Static Atributes
 */
const char zmq_devname[4] = "zmq";

/*
 * Static methods
 */

void rf_zmq_info(char* id, const char* format, ...)
{
#if VERBOSE
  struct timeval t;
  gettimeofday(&t, NULL);
  va_list args;
  va_start(args, format);
  printf("[%s@%02ld.%06ld] ", id ? id : "zmq", t.tv_sec % 10, t.tv_usec);
  vprintf(format, args);
  va_end(args);
#else  /* VERBOSE */
  // Do nothing
#endif /* VERBOSE */
}

void rf_zmq_error(char* id, const char* format, ...)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}

static inline int update_ts(void* h, uint64_t* ts, int nsamples, const char* dir)
{
  int ret = SRSLTE_ERROR;

  if (h && nsamples > 0) {
    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;

    (*ts) += nsamples;

    srslte_timestamp_t _ts = {};
    srslte_timestamp_init_uint64(&_ts, *ts, handler->base_srate);
    rf_zmq_info(
        handler->id, "    -> next %s time after %d samples: %d + %.3f\n", dir, nsamples, _ts.full_secs, _ts.frac_secs);

    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

int rf_zmq_handle_error(char* id, const char* text)
{
  int ret = SRSLTE_SUCCESS;

  int err = zmq_errno();

  switch (err) {
    // handled errors
    case EFSM:
    case EAGAIN:
      rf_zmq_info(id, "Warning %s: %s\n", text, zmq_strerror(err));
      break;

    // critical non-handled errors
    default:
      ret = SRSLTE_ERROR;
      rf_zmq_error(id, "Error %s: %s\n", text, zmq_strerror(err));
  }

  return ret;
}

/*
 * Public methods
 */

void rf_zmq_suppress_stdout(void* h)
{
  // do nothing
}

void rf_zmq_register_error_handler(void* h, srslte_rf_error_handler_t new_handler, void* arg)
{
  // do nothing
}

const char* rf_zmq_devname(void* h)
{
  return zmq_devname;
}

int rf_zmq_start_rx_stream(void* h, bool now)
{
  return SRSLTE_SUCCESS;
}

int rf_zmq_stop_rx_stream(void* h)
{
  return 0;
}

void rf_zmq_flush_buffer(void* h)
{
  printf("%s\n", __FUNCTION__);
}

bool rf_zmq_has_rssi(void* h)
{
  return false;
}

float rf_zmq_get_rssi(void* h)
{
  return 0.0;
}

int rf_zmq_open(char* args, void** h)
{
  return rf_zmq_open_multi(args, h, 1);
}

int rf_zmq_open_multi(char* args, void** h, uint32_t nof_channels)
{
  int ret = SRSLTE_ERROR;
  if (h && nof_channels < SRSLTE_MAX_CHANNELS) {
    *h = NULL;

    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)malloc(sizeof(rf_zmq_handler_t));
    if (!handler) {
      perror("malloc");
      return SRSLTE_ERROR;
    }
    bzero(handler, sizeof(rf_zmq_handler_t));
    *h                        = handler;
    handler->base_srate       = ZMQ_BASERATE_DEFAULT_HZ; // Sample rate for 100 PRB cell
    handler->rx_gain          = 0.0;
    handler->info.max_rx_gain = ZMQ_MAX_GAIN_DB;
    handler->info.min_rx_gain = ZMQ_MIN_GAIN_DB;
    handler->info.max_tx_gain = ZMQ_MAX_GAIN_DB;
    handler->info.min_tx_gain = ZMQ_MIN_GAIN_DB;
    handler->nof_channels     = nof_channels;
    strcpy(handler->id, "zmq\0");

    rf_zmq_opts_t rx_opts = {};
    rf_zmq_opts_t tx_opts = {};
    rx_opts.socket_type   = ZMQ_REQ;
    tx_opts.socket_type   = ZMQ_REP;
    tx_opts.id            = handler->id;
    rx_opts.id            = handler->id;

    if (pthread_mutex_init(&handler->tx_config_mutex, NULL)) {
      perror("Mutex init");
    }
    if (pthread_mutex_init(&handler->rx_config_mutex, NULL)) {
      perror("Mutex init");
    }
    if (pthread_mutex_init(&handler->decim_mutex, NULL)) {
      perror("Mutex init");
    }

    // parse args
    if (args && strlen(args)) {
      // base_srate
      parse_uint32(args, "base_srate", -1, &handler->base_srate);

      // id
      parse_string(args, "id", -1, handler->id);

      // rx_type
      char tmp[RF_PARAM_LEN] = {0};
      if (parse_string(args, "rx_type", -1, tmp) == SRSLTE_SUCCESS) {
        if (!strcmp(tmp, "sub")) {
          rx_opts.socket_type = ZMQ_SUB;
        } else {
          printf("Unsupported socket type %s\n", tmp);
          goto clean_exit;
        }
      }

      // rx_format
      rx_opts.sample_format = ZMQ_TYPE_FC32;
      if (parse_string(args, "rx_format", -1, tmp) == SRSLTE_SUCCESS) {
        if (!strcmp(tmp, "sc16")) {
          rx_opts.sample_format = ZMQ_TYPE_SC16;
        } else {
          printf("Unsupported sample format %s\n", tmp);
          goto clean_exit;
        }
      }

      // tx_type
      if (parse_string(args, "tx_type", -1, tmp) == SRSLTE_SUCCESS) {
        if (!strcmp(tmp, "pub")) {
          tx_opts.socket_type = ZMQ_PUB;
        } else {
          printf("Unsupported socket type %s\n", tmp);
          goto clean_exit;
        }
      }

      // tx_format
      tx_opts.sample_format = ZMQ_TYPE_FC32;
      if (parse_string(args, "tx_format", -1, tmp) == SRSLTE_SUCCESS) {
        if (!strcmp(tmp, "sc16")) {
          tx_opts.sample_format = ZMQ_TYPE_SC16;
        } else {
          printf("Unsupported sample format %s\n", tmp);
          goto clean_exit;
        }
      }
    } else {
      fprintf(stderr, "[zmq] Error: RF device args are required for ZMQ no-RF module\n");
      goto clean_exit;
    }

    update_rates(handler, 1.92e6);

    //  Create ZMQ context
    handler->context = zmq_ctx_new();
    if (!handler->context) {
      fprintf(stderr, "[zmq] Error: creating new context\n");
      goto clean_exit;
    }

    for (int i = 0; i < handler->nof_channels; i++) {
      char rx_port[RF_PARAM_LEN] = {};
      char tx_port[RF_PARAM_LEN] = {};

      // rx_port
      parse_string(args, "rx_port", i, rx_port);

      // rx_freq
      double rx_freq = 0.0f;
      parse_double(args, "rx_freq", i, &rx_freq);
      rx_opts.frequency_mhz = (uint32_t)(rx_freq / 1e6);

      // tx_port
      parse_string(args, "tx_port", i, tx_port);

      // tx_freq
      double tx_freq = 0.0f;
      parse_double(args, "tx_freq", i, &tx_freq);
      tx_opts.frequency_mhz = (uint32_t)(tx_freq / 1e6);

      // fail_on_disconnect
      char tmp[RF_PARAM_LEN] = {};
      parse_string(args, "fail_on_disconnect", i, tmp);
      if (strncmp(tmp, "true", RF_PARAM_LEN) == 0 || strncmp(tmp, "yes", RF_PARAM_LEN) == 0) {
        rx_opts.fail_on_disconnect = true;
      }

      // initialize transmitter
      if (strlen(tx_port) != 0) {
        if (rf_zmq_tx_open(&handler->transmitter[i], tx_opts, handler->context, tx_port) != SRSLTE_SUCCESS) {
          fprintf(stderr, "[zmq] Error: opening transmitter\n");
          goto clean_exit;
        }
      } else {
        fprintf(stdout, "[zmq] %s Tx port not specified. Disabling transmitter.\n", handler->id);
        handler->tx_off = true;
      }

      // initialize receiver
      if (strlen(rx_port) != 0) {
        if (rf_zmq_rx_open(&handler->receiver[i], rx_opts, handler->context, rx_port) != SRSLTE_SUCCESS) {
          fprintf(stderr, "[zmq] Error: opening receiver\n");
          goto clean_exit;
        }
      } else {
        fprintf(stdout, "[zmq] %s Rx port not specified. Disabling receiver.\n", handler->id);
      }

      if (!handler->transmitter[i].running && !handler->receiver[i].running) {
        fprintf(stderr, "[zmq] Error: Neither Tx port nor Rx port specified.\n");
        goto clean_exit;
      }
    }

    // Create decimation and overflow buffer
    for (uint32_t i = 0; i < handler->nof_channels; i++) {
      handler->buffer_decimation[i] = srslte_vec_malloc(ZMQ_MAX_BUFFER_SIZE);
      if (!handler->buffer_decimation[i]) {
        fprintf(stderr, "Error: allocating decimation buffer\n");
        goto clean_exit;
      }
    }

    handler->buffer_tx = srslte_vec_malloc(ZMQ_MAX_BUFFER_SIZE);
    if (!handler->buffer_tx) {
      fprintf(stderr, "Error: allocating tx buffer\n");
      goto clean_exit;
    }

    ret = SRSLTE_SUCCESS;

  clean_exit:
    if (ret) {
      rf_zmq_close(handler);
    }
  }
  return ret;
}

int rf_zmq_close(void* h)
{
  rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;

  rf_zmq_info(handler->id, "Closing ...\n");

  for (int i = 0; i < handler->nof_channels; i++) {
    rf_zmq_tx_close(&handler->transmitter[i]);
    rf_zmq_rx_close(&handler->receiver[i]);
  }

  if (handler->context) {
    zmq_ctx_destroy(handler->context);
  }

  for (uint32_t i = 0; i < handler->nof_channels; i++) {
    if (handler->buffer_decimation[i]) {
      free(handler->buffer_decimation[i]);
    }
  }

  if (handler->buffer_tx) {
    free(handler->buffer_tx);
  }

  pthread_mutex_destroy(&handler->tx_config_mutex);
  pthread_mutex_destroy(&handler->rx_config_mutex);
  pthread_mutex_destroy(&handler->decim_mutex);

  // Free all
  free(handler);

  return SRSLTE_SUCCESS;
}

void update_rates(rf_zmq_handler_t* handler, double srate)
{
  pthread_mutex_lock(&handler->decim_mutex);
  if (handler) {
    // Decimation must be full integer
    if (((uint64_t)handler->base_srate % (uint64_t)srate) == 0) {
      handler->srate        = (uint32_t)srate;
      handler->decim_factor = handler->base_srate / handler->srate;
    } else {
      fprintf(stderr,
              "Error: couldn't update sample rate. %.2f is not divisible by %.2f\n",
              srate / 1e6,
              handler->base_srate / 1e6);
    }
    printf("Current sample rate is %.2f MHz with a base rate of %.2f MHz (x%d decimation)\n",
           handler->srate / 1e6,
           handler->base_srate / 1e6,
           handler->decim_factor);
  }
  pthread_mutex_unlock(&handler->decim_mutex);
}

double rf_zmq_set_rx_srate(void* h, double srate)
{
  double ret = 0.0;
  if (h) {
    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;
    update_rates(handler, srate);
    ret = handler->srate;
  }
  return ret;
}

double rf_zmq_set_tx_srate(void* h, double srate)
{
  double ret = 0.0;
  if (h) {
    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;
    update_rates(handler, srate);
    ret = srate;
  }
  return ret;
}

double rf_zmq_set_rx_gain(void* h, double gain)
{
  double ret = 0.0;
  if (h) {
    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;
    handler->rx_gain          = gain;
    ret                       = gain;
  }
  return ret;
}

double rf_zmq_set_tx_gain(void* h, double gain)
{
  return 0.0;
}

double rf_zmq_get_rx_gain(void* h)
{
  double ret = 0.0;
  if (h) {
    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;
    ret                       = handler->rx_gain;
  }
  return ret;
}

double rf_zmq_get_tx_gain(void* h)
{
  return 0.0;
}

srslte_rf_info_t* rf_zmq_get_info(void* h)
{
  srslte_rf_info_t* info = NULL;
  if (h) {
    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;
    info                      = &handler->info;
  }
  return info;
}

double rf_zmq_set_rx_freq(void* h, uint32_t ch, double freq)
{
  double ret = NAN;
  if (h) {
    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;
    pthread_mutex_lock(&handler->rx_config_mutex);
    if (ch < handler->nof_channels && isnormal(freq) && freq > 0.0) {
      handler->rx_freq_mhz[ch] = (uint32_t)(freq / 1e6);
      ret                      = freq;
    }
    pthread_mutex_unlock(&handler->rx_config_mutex);
  }
  return ret;
}

double rf_zmq_set_tx_freq(void* h, uint32_t ch, double freq)
{
  double ret = NAN;
  if (h) {
    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;
    pthread_mutex_lock(&handler->tx_config_mutex);
    if (ch < handler->nof_channels && isnormal(freq) && freq > 0.0) {
      handler->tx_freq_mhz[ch] = (uint32_t)(freq / 1e6);
      ret                      = freq;
    }
    pthread_mutex_unlock(&handler->tx_config_mutex);
  }
  return ret;
}

void rf_zmq_get_time(void* h, time_t* secs, double* frac_secs)
{
  if (h) {
    if (secs) {
      *secs = 0;
    }

    if (frac_secs) {
      *frac_secs = 0;
    }
  }
}

#if ZMQ_MONITOR
static int rf_zmq_rx_get_monitor_event(void* monitor, int* value, char** address)
{
  // First frame in message contains event number and value
  zmq_msg_t msg;
  zmq_msg_init(&msg);
  if (zmq_msg_recv(&msg, monitor, 0) == -1) {
    printf("zmq_msg_recv failed!\n");
    return -1; // Interruped, presumably
  }

  if (zmq_msg_more(&msg)) {
    printf("more to read\n");
  }

  uint8_t* data  = (uint8_t*)zmq_msg_data(&msg);
  uint16_t event = *(uint16_t*)(data);
  if (value) {
    *value = *(uint32_t*)(data + 2);
  }

  // Second frame in message contains event address
  zmq_msg_init(&msg);
  if (zmq_msg_recv(&msg, monitor, 0) == -1) {
    return -1; // Interruped, presumably
  }
  if (zmq_msg_more(&msg)) {
    printf("error in msg_more \n");
  }

  if (address) {
    uint8_t* data = (uint8_t*)zmq_msg_data(&msg);
    size_t   size = zmq_msg_size(&msg);
    *address      = (char*)malloc(size + 1);
    memcpy(*address, data, size);
    *address[size] = 0;
  }
  return event;
}
#endif // ZMQ_MONITOR

int rf_zmq_recv_with_time(void* h, void* data, uint32_t nsamples, bool blocking, time_t* secs, double* frac_secs)
{
  return rf_zmq_recv_with_time_multi(h, &data, nsamples, blocking, secs, frac_secs);
}

int rf_zmq_recv_with_time_multi(void*    h,
                                void*    data[4],
                                uint32_t nsamples,
                                bool     blocking,
                                time_t*  secs,
                                double*  frac_secs)
{
  int ret = SRSLTE_ERROR;

  if (h) {
    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;

    // Map ports to data buffers according to the selected frequencies
    pthread_mutex_lock(&handler->rx_config_mutex);
    cf_t* buffers[SRSLTE_MAX_CHANNELS] = {}; // Buffer pointers, NULL if unmatched
    for (uint32_t i = 0; i < handler->nof_channels; i++) {
      bool mapped = false;

      // Find first matching frequency
      for (uint32_t j = 0; j < handler->nof_channels && !mapped; j++) {
        // Traverse all channels, break if mapped
        if (buffers[j] == NULL && rf_zmq_rx_match_freq(&handler->receiver[j], handler->rx_freq_mhz[i])) {
          // Available buffer and matched frequency with receiver
          buffers[j] = (cf_t*)data[i];
          mapped     = true;
        }
      }

      // If no matching frequency found; set data to zeros
      if (!mapped && data[i]) {
        memset(data[i], 0, sizeof(cf_t) * nsamples);
      }
    }
    pthread_mutex_unlock(&handler->rx_config_mutex);

    // Protect the access to decim_factor since is a shared variable
    pthread_mutex_lock(&handler->decim_mutex);
    uint32_t decim_factor = handler->decim_factor;
    pthread_mutex_unlock(&handler->decim_mutex);

    uint32_t nbytes            = NSAMPLES2NBYTES(nsamples * decim_factor);
    uint32_t nsamples_baserate = nsamples * decim_factor;

    rf_zmq_info(handler->id, "Rx %d samples (%d B)\n", nsamples, nbytes);

    // set timestamp for this reception
    if (secs != NULL && frac_secs != NULL) {
      srslte_timestamp_t ts = {};
      srslte_timestamp_init_uint64(&ts, handler->next_rx_ts, handler->base_srate);
      *secs      = ts.full_secs;
      *frac_secs = ts.frac_secs;
    }

    // return if receiver is turned off
    if (!handler->receiver[0].running) {
      update_ts(handler, &handler->next_rx_ts, nsamples_baserate, "rx");
      return nsamples;
    }

    // Check available buffer size
    if (nbytes > ZMQ_MAX_BUFFER_SIZE) {
      fprintf(stderr,
              "[zmq] Error: Trying to receive %d B but buffer is only %zu B at channel %d.\n",
              nbytes,
              ZMQ_MAX_BUFFER_SIZE,
              0);
      goto clean_exit;
    }

    // receive samples
    srslte_timestamp_t ts_tx = {}, ts_rx = {};
    srslte_timestamp_init_uint64(&ts_tx, handler->transmitter[0].nsamples, handler->base_srate);
    srslte_timestamp_init_uint64(&ts_rx, handler->next_rx_ts, handler->base_srate);
    rf_zmq_info(handler->id, " - next rx time: %d + %.3f\n", ts_rx.full_secs, ts_rx.frac_secs);
    rf_zmq_info(handler->id, " - next tx time: %d + %.3f\n", ts_tx.full_secs, ts_tx.frac_secs);

    // Leave time for the Tx to transmit
    usleep((1000000 * nsamples) / handler->base_srate);

    // check for tx gap if we're also transmitting on this radio
    for (int i = 0; i < handler->nof_channels; i++) {
      if (handler->transmitter[i].running) {
        rf_zmq_tx_align(&handler->transmitter[i], handler->next_rx_ts + nsamples_baserate);
      }
    }

    // copy from rx buffer as many samples as requested into provided buffer
    bool    completed                  = false;
    int32_t count[SRSLTE_MAX_CHANNELS] = {};
    while (!completed) {
      uint32_t completed_count = 0;

      // Iterate channels
      for (uint32_t i = 0; i < handler->nof_channels; i++) {
        cf_t* ptr = (decim_factor != 1 || buffers[i] == NULL) ? handler->buffer_decimation[i] : buffers[i];

        // Completed condition
        if (count[i] < nsamples_baserate && handler->receiver[i].running) {
          // Keep receiving
          int32_t n = rf_zmq_rx_baseband(&handler->receiver[i], &ptr[count[i]], nsamples_baserate);
#if ZMQ_MONITOR
          // handle socket events
          int event = rf_zmq_rx_get_monitor_event(handler->receiver[i].socket_monitor, NULL, NULL);
          if (event != -1) {
            printf("event=0x%X\n", event);
            switch (event) {
              case ZMQ_EVENT_CONNECTED:
                handler->receiver[i].tx_connected = true;
                break;
              case ZMQ_EVENT_CLOSED:
                handler->receiver[i].tx_connected = false;
                break;
              default:
                break;
            }
          }
#endif // ZMQ_MONITOR
          if (n > SRSLTE_SUCCESS) {
            // No error
            count[i] += n;
          } else if (n == SRSLTE_ERROR_TIMEOUT) {
            // Other end disconnected, either keep going, or fail
            if (handler->receiver[i].fail_on_disconnect) {
              goto clean_exit;
            }
          } else if (n < SRSLTE_SUCCESS) {
            // Other error, exit
            fprintf(stderr, "Error: receiving data.\n");
            goto clean_exit;
          }
        } else {
          // Completed, count it
          completed_count++;
        }
      }

      // Check if all channels are completed
      completed = (completed_count == handler->nof_channels);
    }
    rf_zmq_info(handler->id,
                " - read %d samples. %d samples available\n",
                NBYTES2NSAMPLES(nbytes),
                NBYTES2NSAMPLES(srslte_ringbuffer_status(&handler->receiver[0].ringbuffer)));

    // decimate if needed
    if (decim_factor != 1) {
      for (uint32_t c = 0; c < handler->nof_channels; c++) {
        // skip if buffer is not available
        if (buffers[c]) {
          cf_t* dst = buffers[c];
          cf_t* ptr = handler->buffer_decimation[c];

          for (uint32_t i = 0, n = 0; i < nsamples; i++) {
            // Averaging decimation
            cf_t avg = 0.0f;
            for (int j = 0; j < decim_factor; j++, n++) {
              avg += ptr[n];
            }
            dst[i] = avg;
          }

          rf_zmq_info(handler->id,
                      "  - re-adjust bytes due to %dx decimation %d --> %d samples)\n",
                      decim_factor,
                      nsamples_baserate,
                      nsamples);
        }
      }
    }

    // Set gain
    float scale = srslte_convert_dB_to_amplitude(handler->rx_gain);
    for (uint32_t c = 0; c < handler->nof_channels; c++) {
      if (buffers[c]) {
        srslte_vec_sc_prod_cfc(buffers[c], scale, buffers[c], nsamples);
      }
    }

    // update rx time
    update_ts(handler, &handler->next_rx_ts, nsamples_baserate, "rx");
  }

  ret = nsamples;

clean_exit:

  return ret;
}

int rf_zmq_send_timed(void*  h,
                      void*  data,
                      int    nsamples,
                      time_t secs,
                      double frac_secs,
                      bool   has_time_spec,
                      bool   blocking,
                      bool   is_start_of_burst,
                      bool   is_end_of_burst)
{
  void* _data[4] = {data, NULL, NULL, NULL};

  return rf_zmq_send_timed_multi(
      h, _data, nsamples, secs, frac_secs, has_time_spec, blocking, is_start_of_burst, is_end_of_burst);
}

// TODO: Implement Tx upsampling
int rf_zmq_send_timed_multi(void*  h,
                            void*  data[4],
                            int    nsamples,
                            time_t secs,
                            double frac_secs,
                            bool   has_time_spec,
                            bool   blocking,
                            bool   is_start_of_burst,
                            bool   is_end_of_burst)
{
  int ret = SRSLTE_ERROR;

  if (h && data && nsamples > 0) {
    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;

    // Map ports to data buffers according to the selected frequencies
    pthread_mutex_lock(&handler->tx_config_mutex);
    cf_t* buffers[SRSLTE_MAX_CHANNELS] = {}; // Buffer pointers, NULL if unmatched
    for (uint32_t i = 0; i < handler->nof_channels; i++) {
      bool mapped = false;

      // Find first matching frequency
      for (uint32_t j = 0; j < handler->nof_channels && !mapped; j++) {
        // Traverse all channels, break if mapped
        if (buffers[j] == NULL && rf_zmq_tx_match_freq(&handler->transmitter[j], handler->tx_freq_mhz[i])) {
          // Available buffer and matched frequency with receiver
          buffers[j] = (cf_t*)data[i];
          mapped     = true;
        }
      }
    }
    pthread_mutex_unlock(&handler->tx_config_mutex);

    // Protect the access to decim_factor since is a shared variable
    pthread_mutex_lock(&handler->decim_mutex);
    uint32_t decim_factor = handler->decim_factor;
    pthread_mutex_unlock(&handler->decim_mutex);

    uint32_t nbytes            = NSAMPLES2NBYTES(nsamples);
    uint32_t nsamples_baseband = nsamples * decim_factor;
    uint32_t nbytes_baseband   = NSAMPLES2NBYTES(nsamples_baseband);
    if (nbytes_baseband > ZMQ_MAX_BUFFER_SIZE) {
      fprintf(stderr, "Error: trying to transmit too many samples (%d > %zu).\n", nbytes, ZMQ_MAX_BUFFER_SIZE);
      goto clean_exit;
    }

    rf_zmq_info(handler->id, "Tx %d samples (%d B)\n", nsamples, nbytes);

    // return if transmitter is switched off
    if (handler->tx_off) {
      return SRSLTE_SUCCESS;
    }

    // check if this is a tx in the future
    if (has_time_spec) {
      rf_zmq_info(handler->id, "    - tx time: %d + %.3f\n", secs, frac_secs);

      srslte_timestamp_t ts = {};
      srslte_timestamp_init(&ts, secs, frac_secs);
      uint64_t tx_ts              = srslte_timestamp_uint64(&ts, handler->base_srate);
      int      num_tx_gap_samples = 0;

      for (int i = 0; i < handler->nof_channels; i++) {
        if (handler->transmitter[i].running) {
          num_tx_gap_samples = rf_zmq_tx_align(&handler->transmitter[i], tx_ts);
        }
      }

      if (num_tx_gap_samples < 0) {
        fprintf(stderr,
                "[zmq] Error: tx time is %.3f ms in the past (%" PRIu64 " < %" PRIu64 ")\n",
                -1000.0 * num_tx_gap_samples / handler->base_srate,
                tx_ts,
                handler->transmitter[0].nsamples);
        goto clean_exit;
      }
    }

    // Send base-band samples
    for (int i = 0; i < handler->nof_channels; i++) {
      if (buffers[i] != NULL) {
        // Select buffer pointer depending on interpolation
        cf_t* buf = (decim_factor != 1) ? handler->buffer_tx : buffers[i];

        // Interpolate if required
        if (decim_factor != 1) {
          rf_zmq_info(handler->id,
                      "  - re-adjust bytes due to %dx interpolation %d --> %d samples)\n",
                      decim_factor,
                      nsamples,
                      nsamples_baseband);

          int   n   = 0;
          cf_t* src = data[i];
          for (int k = 0; k < nsamples; k++) {
            // perform zero order hold
            for (int j = 0; j < decim_factor; j++, n++) {
              buf[n] = src[k];
            }
          }

          if (nsamples_baseband != n) {
            fprintf(stderr,
                    "Number of tx samples (%d) does not match with number of interpolated samples (%d)\n",
                    nsamples_baseband,
                    n);
            goto clean_exit;
          }
        }

        int n = rf_zmq_tx_baseband(&handler->transmitter[i], buf, nsamples_baseband);
        if (n == SRSLTE_ERROR) {
          goto clean_exit;
        }
      } else {
        int n = rf_zmq_tx_zeros(&handler->transmitter[i], nsamples_baseband);
        if (n == SRSLTE_ERROR) {
          goto clean_exit;
        }
      }
    }
  }

  ret = SRSLTE_SUCCESS;

clean_exit:

  return ret;
}
