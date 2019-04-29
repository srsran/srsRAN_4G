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
#include <math.h>
#include <signal.h>
#include <srslte/phy/common/phy_common.h>
#include <srslte/phy/common/timestamp.h>
#include <srslte/phy/rf/rf.h>
#include <srslte/phy/utils/ringbuffer.h>
#include <srslte/phy/utils/vector.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <zmq.h>

typedef struct {
  // Common attributes
  char*            devname;
  srslte_rf_info_t info;
  uint32_t         nof_channels;
  bool             running;

  // RF State
  double   srate; // radio rate configured by upper layers
  double   base_srate;
  uint32_t decim_factor; // decimation factor between base_srate used on transport on radio's rate
  double   rx_gain;
  double   tx_freq;
  double   rx_freq;
  bool     tx_used;

  // Server
  void* context;
  void* transmitter;
  void* receiver;

  char rx_port[PARAM_LEN];
  char tx_port[PARAM_LEN];
  char id[PARAM_LEN_SHORT];

  // Various sample buffers
  cf_t* buffer_decimation;
  cf_t* buffer_rx;
  cf_t* buffer_tx;

  // Rx and Tx timestamps
  uint64_t next_rx_ts;
  uint64_t next_tx_ts;

  // Ringbuffer
  srslte_ringbuffer_t rx_ringbuffer;

  pthread_t       thread;
  pthread_mutex_t mutex;
  pthread_mutex_t mutex_tx;
} rf_zmq_handler_t;

/* Definitions */
#define VERBOSE 0

#define NSAMPLES2NBYTES(X) (((uint32_t)(X)) * sizeof(cf_t))
#define NBYTES2NSAMPLES(X) ((X) / sizeof(cf_t))
#define BUFFER_SIZE (NSAMPLES2NBYTES(3072000)) // 10 subframes at 20 MHz
#define ZMQ_TIMEOUT_MS 1000
#define ZMQ_MAXTRIALS 3
#define ZMQ_TRX_MARGIN_MS 1

void update_rates(rf_zmq_handler_t* handler, double srate);

/*
 * Static Atributes
 */
const char zmq_devname[4] = "zmq";

/*
 * Static methods
 */

static inline void rf_zmq_info(rf_zmq_handler_t* handler, const char* format, ...)
{
#if VERBOSE
  struct timeval t;
  gettimeofday(&t, NULL);
  va_list args;
  va_start(args, format);
  printf("[%s@%02ld.%06ld] ", handler ? handler->id : "zmq", t.tv_sec % 10, t.tv_usec);
  vprintf(format, args);
  va_end(args);
#else  /* VERBOSE */
  // Do nothing
#endif /* VERBOSE */
}

static void rf_zmq_error(rf_zmq_handler_t* handler, const char* format, ...)
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

    pthread_mutex_lock(&handler->mutex);
    (*ts) += nsamples;
    pthread_mutex_unlock(&handler->mutex);

    srslte_timestamp_t _ts = {};
    srslte_timestamp_init_uint64(&_ts, *ts, handler->base_srate);
    rf_zmq_info(handler, "    -> next %s time after %d samples: %d + %.3f\n", dir, nsamples, _ts.full_secs,
                _ts.frac_secs);

    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

static inline int rf_zmq_handle_error(rf_zmq_handler_t* handler, const char* text)
{
  int ret = SRSLTE_SUCCESS;

  int err = zmq_errno();

  switch (err) {
    // handled errors
    case EFSM:
    case EAGAIN:
      rf_zmq_info(handler, "Warning %s: %s\n", text, zmq_strerror(err));
      break;
      // critical non-handled errors
    default:
      ret = SRSLTE_ERROR;
      rf_zmq_error(handler, "Error %s: %s\n", text, zmq_strerror(err));
  }

  return ret;
}

static int rf_zmq_tx(rf_zmq_handler_t* handler, uint8_t* buffer, uint32_t nbytes)
{
  int n, ntrials;
  pthread_mutex_lock(&handler->mutex_tx);

  // Receive Transmit request
  uint8_t dummy;

  for (ntrials = 0, n = -1; ntrials < ZMQ_MAXTRIALS && n < 0 && handler->running; ntrials++) {
    n = zmq_recv(handler->transmitter, &dummy, sizeof(dummy), 0);
    if (n < 0) {
      if (rf_zmq_handle_error(handler, "tx request receive")) {
        n = SRSLTE_ERROR;
        goto clean_exit;
      }
    } else {
      rf_zmq_info(handler, " - tx request received\n");
      rf_zmq_info(handler, " - sending %d samples (%d B)\n", NBYTES2NSAMPLES(nbytes), nbytes);
    }
  }

  // Send zeros
  for (ntrials = 0, n = -1; ntrials < ZMQ_MAXTRIALS && n < 0 && handler->running; ntrials++) {
    n = zmq_send(handler->transmitter, buffer, nbytes, 0);
    if (n < 0) {
      if (rf_zmq_handle_error(handler, "tx baseband send")) {
        n = SRSLTE_ERROR;
        goto clean_exit;
      }
    } else if (n != nbytes) {
      rf_zmq_error(handler, "[zmq] Error: transmitter expected %d bytes and sent %d. %s.\n", nbytes, n,
                   strerror(zmq_errno()));
      n = SRSLTE_ERROR;
      goto clean_exit;
    }
  }
  // update both tx timestamp and ringbuffer
  update_ts(handler, &handler->next_tx_ts, NBYTES2NSAMPLES(nbytes), "tx");

clean_exit:
  pthread_mutex_unlock(&handler->mutex_tx);

  return (n > 0) ? nbytes : SRSLTE_ERROR;
}

static int rf_zmq_tx_zeros(rf_zmq_handler_t* handler, int32_t nsamples)
{
  rf_zmq_info(handler, "Tx %d zero samples\n", nsamples);

  if (NSAMPLES2NBYTES(nsamples) > ZMQ_MAX_RX_BYTES) {
    // can't transmit zeros, buffer too small
    fprintf(stderr, "[zmq] Error: zero buffer too small (%ld) to transmit %ld samples\n", ZMQ_MAX_RX_BYTES,
            NSAMPLES2NBYTES(nsamples));
    return SRSLTE_ERROR;
  }

  bzero(handler->buffer_tx, NSAMPLES2NBYTES(nsamples));

  return rf_zmq_tx(handler, (uint8_t*)handler->buffer_tx, NSAMPLES2NBYTES(nsamples));
}

static void* rf_zmq_async_rx_thread(void* h)
{
  rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;

  while (handler->receiver && handler->running) {
    int     n       = SRSLTE_ERROR;
    uint8_t dummy   = 0xFF;
    int     ntrials = 0;

    rf_zmq_info(handler, "-- ASYNC RX wait...\n");

    // Send request
    for (ntrials = 0; n < 0 && ntrials < ZMQ_MAXTRIALS && handler->running; ntrials++) {
      rf_zmq_info(handler, " - tx'ing rx request\n");
      n = zmq_send(handler->receiver, &dummy, sizeof(dummy), 0);
      if (n < 0) {
        if (rf_zmq_handle_error(handler, "synchronous rx request send")) {
          return NULL;
        }
      }
    }

    // Receive baseband
    for (n = (n < 0) ? 0 : -1; n < 0 && handler->running;) {
      n = zmq_recv(handler->receiver, handler->buffer_rx, BUFFER_SIZE, 0);
      if (n == -1) {
        if (rf_zmq_handle_error(handler, "asynchronous rx baseband receive")) {
          return NULL;
        }
      } else if (n > BUFFER_SIZE) {
        fprintf(stderr, "[zmq] Error: receiver expected <= %ld bytes and received %d at channel %d.\n", BUFFER_SIZE, n,
                0);
        return NULL;
      }
    }

    // Write received data in buffer
    if (n > 0) {
      if (srslte_ringbuffer_write(&handler->rx_ringbuffer, handler->buffer_rx, n) != n) {
        rf_zmq_error(handler, "[zmq] error writing asynchronous ring buffer...\n");
      }
      rf_zmq_info(handler, "   - received %d baseband samples (%d B). %d samples available.\n", NBYTES2NSAMPLES(n), n,
                  srslte_ringbuffer_status(&handler->rx_ringbuffer));
    }
  }

  return NULL;
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

char* rf_zmq_devname(void* h)
{
  return (char*)zmq_devname;
}

bool rf_zmq_rx_wait_lo_locked(void* h)
{
  // TODO: Return true if it is client and connected
  return true;
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

    if (nof_channels != 1) {
      printf("rf_zmq only supports single port at the moment.\n");
      return SRSLTE_ERROR;
    }

    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)malloc(sizeof(rf_zmq_handler_t));
    if (!handler) {
      perror("malloc");
      return SRSLTE_ERROR;
    }
    bzero(handler, sizeof(rf_zmq_handler_t));
    *h                        = handler;
    handler->base_srate       = 23.04e6; // Sample rate for 100 PRB cell
    handler->rx_gain          = 0.0;
    handler->info.max_rx_gain = +INFINITY;
    handler->info.min_rx_gain = -INFINITY;
    handler->info.max_tx_gain = +INFINITY;
    handler->info.min_tx_gain = -INFINITY;
    strcpy(handler->id, "zmq\0");

    pthread_mutex_init(&handler->mutex, NULL);
    pthread_mutex_init(&handler->mutex_tx, NULL);

    // parse args
    if (args) {
      // base_srate
      {
        const char config_arg[]          = "base_srate=";
        char       config_str[PARAM_LEN] = {0};
        char*      config_ptr            = strstr(args, config_arg);
        if (config_ptr) {
          copy_subdev_string(config_str, config_ptr + strlen(config_arg));
          printf("Using base rate=%s\n", config_str);
          handler->base_srate = strtod(config_str, NULL);
          remove_substring(args, config_arg);
          remove_substring(args, config_str);
        }
      }

      // rxport
      {
        const char config_arg[]          = "rx_port=";
        char       config_str[PARAM_LEN] = {0};
        char*      config_ptr            = strstr(args, config_arg);
        if (config_ptr) {
          copy_subdev_string(config_str, config_ptr + strlen(config_arg));
          printf("Using rx_port=%s\n", config_str);
          strncpy(handler->rx_port, config_str, PARAM_LEN);
          handler->rx_port[PARAM_LEN - 1] = 0;
          remove_substring(args, config_arg);
          remove_substring(args, config_str);
        }
      }

      // txport
      {
        const char config_arg[]          = "tx_port=";
        char       config_str[PARAM_LEN] = {0};
        char*      config_ptr            = strstr(args, config_arg);
        if (config_ptr) {
          copy_subdev_string(config_str, config_ptr + strlen(config_arg));
          printf("Using tx_port=%s\n", config_str);
          strncpy(handler->tx_port, config_str, PARAM_LEN);
          handler->tx_port[PARAM_LEN - 1] = 0;
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
    }

    update_rates(handler, 1.92e6);

    //  Create ZMQ context
    handler->context = zmq_ctx_new();
    if (!handler->context) {
      fprintf(stderr, "[zmq] Error: creating new context\n");
      goto clean_exit;
    }

    if (strlen(handler->tx_port) != 0) {
      // Initialise transmitter
      handler->transmitter = zmq_socket(handler->context, ZMQ_REP);
      if (!handler->transmitter) {
        fprintf(stderr, "[zmq] Error: creating transmitter socket\n");
        goto clean_exit;
      }

      rf_zmq_info(handler, "Binding transmitter: %s\n", handler->tx_port);

      ret = zmq_bind(handler->transmitter, handler->tx_port);
      if (ret) {
        fprintf(stderr, "Error: connecting transmitter socket: %s\n", zmq_strerror(zmq_errno()));
        goto clean_exit;
      }

#if ZMQ_TIMEOUT_MS
      // set recv timeout for transmitter
      int timeout = ZMQ_TIMEOUT_MS;
      if (zmq_setsockopt(handler->transmitter, ZMQ_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        fprintf(stderr, "Error: setting receive timeout on tx socket\n");
        goto clean_exit;
      }
      if (zmq_setsockopt(handler->transmitter, ZMQ_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
        fprintf(stderr, "Error: setting receive timeout on tx socket\n");
        goto clean_exit;
      }

      // set linger timeout for transmitter
      timeout = 0;
      if (zmq_setsockopt(handler->transmitter, ZMQ_LINGER, &timeout, sizeof(timeout)) == -1) {
        fprintf(stderr, "Error: setting linger timeout on tx socket\n");
      }
#endif

    } else {
      fprintf(stdout, "[zmq] %s Tx port not specified. Disabling transmitter.\n", handler->id);
    }

    // initialize receiver
    if (strlen(handler->rx_port) != 0) {
      handler->receiver = zmq_socket(handler->context, ZMQ_REQ);
      if (!handler->receiver) {
        fprintf(stderr, "[zmq] Error: creating receiver socket\n");
        goto clean_exit;
      }

      rf_zmq_info(handler, "Connecting receiver: %s\n", handler->rx_port);

      ret = zmq_connect(handler->receiver, handler->rx_port);
      if (ret) {
        fprintf(stderr, "Error: binding receiver socket: %s\n", zmq_strerror(zmq_errno()));
        goto clean_exit;
      }

#if ZMQ_TIMEOUT_MS
      // set recv timeout for receiver
      int timeout = ZMQ_TIMEOUT_MS;
      if (zmq_setsockopt(handler->receiver, ZMQ_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        fprintf(stderr, "Error: setting receive timeout on tx socket\n");
        goto clean_exit;
      }
      if (zmq_setsockopt(handler->receiver, ZMQ_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
        fprintf(stderr, "Error: setting receive timeout on tx socket\n");
        goto clean_exit;
      }

      timeout = 0;
      // set linger timeout for receiver
      if (zmq_setsockopt(handler->receiver, ZMQ_LINGER, &timeout, sizeof(timeout)) == -1) {
        fprintf(stderr, "Error: setting linger timeout on rx socket\n");
      }
#endif

      // init rx ringbuffer
      if (srslte_ringbuffer_init(&handler->rx_ringbuffer, BUFFER_SIZE)) {
        fprintf(stderr, "Error, initiating rx ringbuffer\n");
        goto clean_exit;
      }
    } else {
      fprintf(stdout, "[zmq] %s Rx port not specified. Disabling receiver.\n", handler->id);
    }

    if (handler->transmitter == NULL && handler->receiver == NULL) {
      fprintf(stderr, "[zmq] Error: Neither Tx port nor Rx port specified.\n");
      goto clean_exit;
    }

    // Create decimation and overflow buffer
    handler->buffer_decimation = srslte_vec_malloc(ZMQ_MAX_RX_BYTES);
    if (!handler->buffer_decimation) {
      fprintf(stderr, "Error: allocating decimation buffer\n");
      goto clean_exit;
    }

    handler->buffer_tx = srslte_vec_malloc(ZMQ_MAX_RX_BYTES);
    if (!handler->buffer_tx) {
      fprintf(stderr, "Error: allocating tx buffer\n");
      goto clean_exit;
    }

    handler->buffer_rx = srslte_vec_malloc(ZMQ_MAX_RX_BYTES);
    if (!handler->buffer_rx) {
      fprintf(stderr, "Error: allocating rx buffer\n");
      goto clean_exit;
    }

    handler->running = true;
    if (handler->receiver) {
      pthread_create(&handler->thread, NULL, rf_zmq_async_rx_thread, handler);
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

  rf_zmq_info(handler, "Closing %s ...\n", handler->id);

  handler->running = false;
  if (handler->thread) {
    pthread_join(handler->thread, NULL);
    pthread_detach(handler->thread);
  }

  if (handler->transmitter) {
    zmq_close(handler->transmitter);
    handler->transmitter = NULL;
  }
  if (handler->receiver) {
    zmq_close(handler->receiver);
    handler->receiver = NULL;
    srslte_ringbuffer_free(&handler->rx_ringbuffer);
  }

  if (handler->context) {
    zmq_ctx_destroy(handler->context);
  }

  if (handler->buffer_decimation) {
    free(handler->buffer_decimation);
  }

  if (handler->buffer_tx) {
    free(handler->buffer_tx);
  }

  if (handler->buffer_rx) {
    free(handler->buffer_rx);
  }

  pthread_mutex_destroy(&handler->mutex);
  pthread_mutex_destroy(&handler->mutex_tx);

  // Free all
  free(handler);

  return SRSLTE_SUCCESS;
}

void rf_zmq_set_master_clock_rate(void* h, double rate)
{
  // Do nothing
}

bool rf_zmq_is_master_clock_dynamic(void* h)
{
  return false;
}

void update_rates(rf_zmq_handler_t* handler, double srate)
{
  if (handler) {
    // Decimation must be full integer
    if (((uint64_t)handler->base_srate % (uint64_t)srate) == 0) {
      handler->srate        = srate;
      handler->decim_factor = handler->base_srate / handler->srate;
    } else {
      fprintf(stderr, "Error: couldn't update sample rate. %.2f is not divisible by %.2f\n", srate / 1e6,
              handler->base_srate / 1e6);
    }
    printf("Current sample rate is %.2f MHz with a base rate of %.2f MHz (x%d decimation)\n", handler->srate / 1e6,
           handler->base_srate / 1e6, handler->decim_factor);
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

int rf_zmq_recv_with_time_multi(
    void* h, void* data[4], uint32_t nsamples, bool blocking, time_t* secs, double* frac_secs)
{
  int ret = SRSLTE_ERROR;

  if (h) {
    rf_zmq_handler_t* handler = (rf_zmq_handler_t*)h;

    uint32_t nbytes            = NSAMPLES2NBYTES(nsamples * handler->decim_factor);
    uint32_t nsamples_baserate = nsamples * handler->decim_factor;
    uint32_t nbytes_baserate   = NSAMPLES2NBYTES(nsamples_baserate);

    rf_zmq_info(handler, "Rx %d samples (%d B)\n", nsamples, nbytes);

    // set timestamp for this reception
    if (secs != NULL && frac_secs != NULL) {
      srslte_timestamp_t ts = {};
      srslte_timestamp_init_uint64(&ts, handler->next_rx_ts, handler->base_srate);
      *secs      = ts.full_secs;
      *frac_secs = ts.frac_secs;
    }

    // return if receiver is turned off
    if (handler->receiver == NULL) {
      update_ts(handler, &handler->next_rx_ts, nsamples_baserate, "rx");
      return nsamples;
    }

    // Check available buffer size
    if (nbytes > ZMQ_MAX_RX_BYTES) {
      fprintf(stderr, "[zmq] Error: Trying to receive %d B but buffer is only %ld B at channel %d.\n", nbytes,
              ZMQ_MAX_RX_BYTES, 0);
      goto clean_exit;
    }

    // receive samples
    srslte_timestamp_t ts_tx = {}, ts_rx = {};
    srslte_timestamp_init_uint64(&ts_tx, handler->next_tx_ts, handler->base_srate);
    srslte_timestamp_init_uint64(&ts_rx, handler->next_rx_ts, handler->base_srate);
    rf_zmq_info(handler, " - next rx time: %d + %.3f\n", ts_rx.full_secs, ts_rx.frac_secs);
    rf_zmq_info(handler, " - next tx time: %d + %.3f\n", ts_tx.full_secs, ts_tx.frac_secs);

    // check for tx gap if we're also transmitting on this radio
    if (handler->transmitter) {
      uint32_t margin_nsamples =
          (uint32_t)(handler->tx_used ? (0) : (nsamples_baserate + ZMQ_TRX_MARGIN_MS * handler->base_srate / 1000.0));
      int num_tx_gap_samples_base_rate = (int)(handler->next_rx_ts - handler->next_tx_ts + margin_nsamples);
      if (num_tx_gap_samples_base_rate > 0) {
        rf_zmq_info(handler, " - tx_gap of %d samples\n", num_tx_gap_samples_base_rate);

        // Transmit zero samples
        rf_zmq_tx_zeros(handler, num_tx_gap_samples_base_rate);
      } else {
        rf_zmq_info(handler, " - no tx gap detected\n");
      }
    }

    // copy from rx buffer as many samples as requested into provided buffer
    cf_t* ptr = (handler->decim_factor != 1) ? handler->buffer_decimation : data[0];
    if (srslte_ringbuffer_read(&handler->rx_ringbuffer, ptr, nbytes_baserate) != nbytes) {
      fprintf(stderr, "Error: reading from rx ringbuffer.\n");
      goto clean_exit;
    }
    rf_zmq_info(handler, " - read %d samples. %d samples available\n", NBYTES2NSAMPLES(nbytes),
                NBYTES2NSAMPLES(srslte_ringbuffer_status(&handler->rx_ringbuffer)));

    // decimate if needed
    if (handler->decim_factor != 1) {
      cf_t* dst = data[0];

      int n;
      for (int i = n = 0; i < nsamples; i++) {
        // Averaging decimation
        cf_t avg = 0.0f;
        for (int j = 0; j < handler->decim_factor; j++, n++) {
          avg += ptr[n];
        }
        dst[i] = avg;
      }
      rf_zmq_info(handler, "  - re-adjust bytes due to %dx decimation %d --> %d samples)\n", handler->decim_factor,
                  nsamples_baserate, nsamples);
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

  return rf_zmq_send_timed_multi(h, _data, nsamples, secs, frac_secs, has_time_spec, blocking, is_start_of_burst,
                                 is_end_of_burst);
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

    if (nbytes_baseband > ZMQ_MAX_RX_BYTES) {
      fprintf(stderr, "Error: trying to transmit too many samples (%d > %ld).\n", nbytes, ZMQ_MAX_RX_BYTES);
      goto clean_exit;
    }

    rf_zmq_info(handler, "Tx %d samples (%d B)\n", nsamples, nbytes);

    // return if transmitter is switched off
    if (handler->tx_port == 0) {
      return SRSLTE_SUCCESS;
    }

    // check if this is a tx in the future
    if (has_time_spec) {
      rf_zmq_info(handler, "    - tx time: %d + %.3f\n", secs, frac_secs);

      srslte_timestamp_t ts = {};
      srslte_timestamp_init(&ts, secs, frac_secs);
      uint64_t tx_ts              = srslte_timestamp_uint64(&ts, handler->base_srate);
      int32_t  num_tx_gap_samples = (int32_t)((int64_t)tx_ts - (int64_t)handler->next_tx_ts);

      if (num_tx_gap_samples < 0) {
        fprintf(stderr, "[zmq] Error: tx time is %.3f ms in the past (%ld < %ld)\n",
                -1000.0 * num_tx_gap_samples / handler->base_srate, tx_ts, handler->next_tx_ts);
        goto clean_exit;
      } else if (num_tx_gap_samples > 0) {
        rf_zmq_info(handler, " - tx gap of %d baseband samples\n", num_tx_gap_samples);

        // send zero samples
        int n = rf_zmq_tx_zeros(handler, num_tx_gap_samples);
        if (n == -1) {
          goto clean_exit;
        }
      } else {
        rf_zmq_info(handler, " - no tx gap detected\n");
      }
    }

    cf_t* buf = (handler->decim_factor != 1) ? handler->buffer_tx : data[0];

    if (handler->decim_factor != 1) {
      rf_zmq_info(handler, "  - re-adjust bytes due to %dx interpolation %d --> %d samples)\n", handler->decim_factor,
                  nsamples, nsamples_baseband);

      int   n   = 0;
      cf_t* src = data[0];
      for (int i = 0; i < nsamples; i++) {
        // perform zero order hold
        for (int j = 0; j < handler->decim_factor; j++, n++) {
          buf[n] = src[i];
        }
      }

      if (nsamples_baseband != n) {
        fprintf(stderr, "Number of tx samples (%d) does not match with number of interpolated samples (%d)\n",
                nsamples_baseband, n);
        goto clean_exit;
      }
    }

    // send baseband samples
    int n = rf_zmq_tx(handler, (uint8_t*)buf, nbytes_baseband);
    if (n == SRSLTE_ERROR) {
      goto clean_exit;
    }
    handler->tx_used = true;
  }

  ret = SRSLTE_SUCCESS;

clean_exit:

  return ret;
}
