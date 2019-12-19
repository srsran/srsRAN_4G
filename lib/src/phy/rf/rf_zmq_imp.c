/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
  double   tx_freq;
  double   rx_freq;
  bool     tx_used;

  // Server
  void*       context;
  rf_zmq_tx_t transmitter[SRSLTE_MAX_PORTS];
  rf_zmq_rx_t receiver[SRSLTE_MAX_PORTS];

  char rx_port[PARAM_LEN];
  char tx_port[PARAM_LEN];
  char id[PARAM_LEN_SHORT];

  // Various sample buffers
  cf_t* buffer_decimation[SRSLTE_MAX_PORTS];
  cf_t* buffer_tx;

  // Rx timestamp
  uint64_t next_rx_ts;

  pthread_t thread;
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

void rf_zmq_register_error_handler(void* h, srslte_rf_error_handler_t new_handler)
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
  if (h) {
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
    handler->info.max_rx_gain = +INFINITY;
    handler->info.min_rx_gain = -INFINITY;
    handler->info.max_tx_gain = +INFINITY;
    handler->info.min_tx_gain = -INFINITY;
    handler->nof_channels     = nof_channels;
    strcpy(handler->id, "zmq\0");

    rf_zmq_opts_t rx_opts = {};
    rf_zmq_opts_t tx_opts = {};
    rx_opts.socket_type   = ZMQ_REQ;
    tx_opts.socket_type   = ZMQ_REP;
    tx_opts.id            = handler->id;
    rx_opts.id            = handler->id;

    // parse args
    if (args && strlen(args)) {
      // base_srate
      {
        const char config_arg[]          = "base_srate=";
        char       config_str[PARAM_LEN] = {0};
        char*      config_ptr            = strstr(args, config_arg);
        if (config_ptr) {
          copy_subdev_string(config_str, config_ptr + strlen(config_arg));
          printf("Using base rate=%s\n", config_str);
          handler->base_srate = (uint32_t)strtod(config_str, NULL);
          remove_substring(args, config_arg);
          remove_substring(args, config_str);
        }
      }

      // id
      {
        const char config_arg[]                = "id=";
        char       config_str[PARAM_LEN_SHORT] = {0};
        char*      config_ptr                  = strstr(args, config_arg);
        if (config_ptr) {
          copy_subdev_string(config_str, config_ptr + strlen(config_arg));
          printf("Using ID=%s\n", config_str);
          strncpy(handler->id, config_str, PARAM_LEN_SHORT);
          handler->id[PARAM_LEN_SHORT - 1] = 0;
          remove_substring(args, config_arg);
          remove_substring(args, config_str);
        }
      }
      // rx_type
      {
        const char config_arg[]                = "rx_type=";
        char       config_str[PARAM_LEN_SHORT] = {0};
        char*      config_ptr                  = strstr(args, config_arg);
        if (config_ptr) {
          copy_subdev_string(config_str, config_ptr + strlen(config_arg));
          if (!strcmp(config_str, "sub")) {
            rx_opts.socket_type = ZMQ_SUB;
            printf("Using ZMQ_SUB for rx socket\n");
          } else {
            printf("Unsupported socket type %s. Using ZMQ_REQ for rx socket\n", config_str);
          }
          remove_substring(args, config_arg);
          remove_substring(args, config_str);
        }
      }
      // rx_format
      {
        const char config_arg[]                = "rx_format=";
        char       config_str[PARAM_LEN_SHORT] = {0};
        char*      config_ptr                  = strstr(args, config_arg);
        if (config_ptr) {
          copy_subdev_string(config_str, config_ptr + strlen(config_arg));
          rx_opts.sample_format = ZMQ_TYPE_FC32;
          if (!strcmp(config_str, "sc16")) {
            rx_opts.sample_format = ZMQ_TYPE_SC16;
            printf("Using sc16 format for rx socket\n");
          } else {
            printf("Unsupported sample format %s. Using fc32 for rx socket\n", config_str);
          }
          remove_substring(args, config_arg);
          remove_substring(args, config_str);
        }
      }
      // tx_type
      {
        const char config_arg[]                = "tx_type=";
        char       config_str[PARAM_LEN_SHORT] = {0};
        char*      config_ptr                  = strstr(args, config_arg);
        if (config_ptr) {
          copy_subdev_string(config_str, config_ptr + strlen(config_arg));
          if (!strcmp(config_str, "pub")) {
            tx_opts.socket_type = ZMQ_PUB;
            printf("Using ZMQ_PUB for tx socket\n");
          } else {
            printf("Unsupported socket type %s. Using ZMQ_REP for tx socket\n", config_str);
          }
          remove_substring(args, config_arg);
          remove_substring(args, config_str);
        }
      }
      // tx_format
      {
        const char config_arg[]                = "tx_format=";
        char       config_str[PARAM_LEN_SHORT] = {0};
        char*      config_ptr                  = strstr(args, config_arg);
        if (config_ptr) {
          copy_subdev_string(config_str, config_ptr + strlen(config_arg));
          tx_opts.sample_format = ZMQ_TYPE_FC32;
          if (!strcmp(config_str, "sc16")) {
            tx_opts.sample_format = ZMQ_TYPE_SC16;
            printf("Using sc16 format for tx socket\n");
          } else {
            printf("Unsupported sample format %s. Using fc32 for tx socket\n", config_str);
          }
          remove_substring(args, config_arg);
          remove_substring(args, config_str);
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
      // rxport
      {
        char config_arg[PARAM_LEN] = "rx_port=";
        char config_str[PARAM_LEN] = {0};

        if (i > 0) {
          snprintf(config_arg, PARAM_LEN, "rx_port%d=", i + 1);
        }

        char* config_ptr = strstr(args, config_arg);

        if (config_ptr) {
          copy_subdev_string(config_str, config_ptr + strlen(config_arg));
          printf("Channel %d. Using rx_port=%s\n", i, config_str);
          strncpy(handler->rx_port, config_str, PARAM_LEN);
          handler->rx_port[PARAM_LEN - 1] = 0;
          remove_substring(args, config_arg);
          remove_substring(args, config_str);
        }
      }

      // txport
      {
        char config_arg[PARAM_LEN] = "tx_port=";
        char config_str[PARAM_LEN] = {0};

        if (i > 0) {
          snprintf(config_arg, PARAM_LEN, "tx_port%d=", i + 1);
        }

        char* config_ptr = strstr(args, config_arg);

        if (config_ptr) {
          copy_subdev_string(config_str, config_ptr + strlen(config_arg));
          printf("Channel %d. Using tx_port=%s\n", i, config_str);
          strncpy(handler->tx_port, config_str, PARAM_LEN);
          handler->tx_port[PARAM_LEN - 1] = 0;
          remove_substring(args, config_arg);
          remove_substring(args, config_str);
        }
      }

      // initialize transmitter
      if (strlen(handler->tx_port) != 0) {
        if (rf_zmq_tx_open(&handler->transmitter[i], tx_opts, handler->context, handler->tx_port) != SRSLTE_SUCCESS) {
          fprintf(stderr, "[zmq] Error: opening transmitter\n");
          goto clean_exit;
        }
      } else {
        fprintf(stdout, "[zmq] %s Tx port not specified. Disabling transmitter.\n", handler->id);
      }

      // initialize receiver
      if (strlen(handler->rx_port) != 0) {
        if (rf_zmq_rx_open(&handler->receiver[i], rx_opts, handler->context, handler->rx_port) != SRSLTE_SUCCESS) {
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
  rf_zmq_stop_rx_stream(h);

  rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;

  rf_zmq_info(handler->id, "Closing ...\n");

  if (handler->thread) {
    pthread_join(handler->thread, NULL);
    pthread_detach(handler->thread);
  }

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

  // Free all
  free(handler);

  return SRSLTE_SUCCESS;
}

void update_rates(rf_zmq_handler_t* handler, double srate)
{
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
    handler->rx_freq          = freq;
    ret                       = freq;
  }
  return ret;
}

double rf_zmq_set_tx_freq(void* h, uint32_t ch, double freq)
{
  double ret = NAN;
  if (h) {
    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;
    handler->tx_freq          = freq;
    ret                       = freq;
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

    uint32_t nbytes            = NSAMPLES2NBYTES(nsamples * handler->decim_factor);
    uint32_t nsamples_baserate = nsamples * handler->decim_factor;

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
    bool    completed               = false;
    int32_t count[SRSLTE_MAX_PORTS] = {};
    while (!completed) {
      uint32_t completed_count = 0;

      // Iterate channels
      for (uint32_t i = 0; i < handler->nof_channels; i++) {
        cf_t* ptr = (handler->decim_factor != 1) ? handler->buffer_decimation[i] : data[i];

        // Completed condition
        if (count[i] < nsamples_baserate && handler->receiver[i].running) {
          // Keep receiving
          int32_t n = rf_zmq_rx_baseband(&handler->receiver[i], &ptr[count[i]], nsamples_baserate);
          if (n > 0) {
            // No error
            count[i] += n;
          } else if (n == SRSLTE_ERROR_TIMEOUT) {
            // Timeout, do nothing, keep going
          } else if (n > 0) {
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
    if (handler->decim_factor != 1) {
      for (int c = 0; c < handler->nof_channels; c++) {

        cf_t* dst = data[c];
        cf_t* ptr = (handler->decim_factor != 1) ? handler->buffer_decimation[c] : data[c];

        int n;
        for (uint32_t i = n = 0; i < nsamples; i++) {
          // Averaging decimation
          cf_t avg = 0.0f;
          for (int j = 0; j < handler->decim_factor; j++, n++) {
            avg += ptr[n];
          }
          dst[i] = avg;
        }
      }
      rf_zmq_info(handler->id,
                  "  - re-adjust bytes due to %dx decimation %d --> %d samples)\n",
                  handler->decim_factor,
                  nsamples_baserate,
                  nsamples);
    }

    // Set gain
    float scale = srslte_convert_dB_to_amplitude(handler->rx_gain);
    srslte_vec_sc_prod_cfc(data[0], scale, data[0], nsamples);

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
    rf_zmq_handler_t* handler           = (rf_zmq_handler_t*)h;
    uint32_t          nbytes            = NSAMPLES2NBYTES(nsamples);
    uint32_t          nsamples_baseband = nsamples * handler->decim_factor;
    uint32_t          nbytes_baseband   = NSAMPLES2NBYTES(nsamples_baseband);

    if (nbytes_baseband > ZMQ_MAX_BUFFER_SIZE) {
      fprintf(stderr, "Error: trying to transmit too many samples (%d > %zu).\n", nbytes, ZMQ_MAX_BUFFER_SIZE);
      goto clean_exit;
    }

    rf_zmq_info(handler->id, "Tx %d samples (%d B)\n", nsamples, nbytes);

    // return if transmitter is switched off
    if (strlen(handler->tx_port) == 0) {
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
      // Select buffer pointer depending on interpolation
      cf_t* buf = (handler->decim_factor != 1) ? handler->buffer_tx : data[i];

      // Interpolate if required
      if (handler->decim_factor != 1) {
        rf_zmq_info(handler->id,
                    "  - re-adjust bytes due to %dx interpolation %d --> %d samples)\n",
                    handler->decim_factor,
                    nsamples,
                    nsamples_baseband);

        int   n   = 0;
        cf_t* src = data[i];
        for (int k = 0; k < nsamples; k++) {
          // perform zero order hold
          for (int j = 0; j < handler->decim_factor; j++, n++) {
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
    }
    handler->tx_used = true;
  }

  ret = SRSLTE_SUCCESS;

clean_exit:

  return ret;
}
