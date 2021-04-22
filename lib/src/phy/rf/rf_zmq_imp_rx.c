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

#include "rf_zmq_imp_trx.h"
#include <inttypes.h>
#include <srsran/phy/utils/vector.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

static void* rf_zmq_async_rx_thread(void* h)
{
  rf_zmq_rx_t* q = (rf_zmq_rx_t*)h;

  while (q->sock && q->running) {
    int     nbytes = 0;
    int     n      = SRSRAN_ERROR;
    uint8_t dummy  = 0xFF;

    rf_zmq_info(q->id, "-- ASYNC RX wait...\n");

    // Send request if socket type is REQUEST
    if (q->socket_type == ZMQ_REQ) {
      while (n < 0 && q->running) {
        rf_zmq_info(q->id, " - tx'ing rx request\n");
        n = zmq_send(q->sock, &dummy, sizeof(dummy), 0);
        if (n < 0) {
          if (rf_zmq_handle_error(q->id, "synchronous rx request send")) {
            return NULL;
          }
        }
      }
    } else {
      n = 0;
    }

    // Receive baseband
    for (n = (n < 0) ? 0 : -1; n < 0 && q->running;) {
      n = zmq_recv(q->sock, q->temp_buffer, ZMQ_MAX_BUFFER_SIZE, 0);
      if (n == -1) {
        if (rf_zmq_handle_error(q->id, "asynchronous rx baseband receive")) {
          return NULL;
        }

      } else if (n > ZMQ_MAX_BUFFER_SIZE) {
        fprintf(stderr,
                "[zmq] Error: receiver expected <= %zu bytes and received %d at channel %d.\n",
                ZMQ_MAX_BUFFER_SIZE,
                n,
                0);
        return NULL;
      } else {
        nbytes = n;
      }
    }

    // Write received data in buffer
    if (nbytes > 0) {
      n = -1;

      // Try to write in ring buffer
      while (n < 0 && q->running) {
        n = srsran_ringbuffer_write_timed(&q->ringbuffer, q->temp_buffer, nbytes, q->trx_timeout_ms);
        if (n == SRSRAN_ERROR_TIMEOUT && q->log_trx_timeout) {
          fprintf(stderr, "Error: timeout writing samples to ringbuffer after %dms\n", q->trx_timeout_ms);
        }
      }

      // Check write
      if (nbytes == n) {
        rf_zmq_info(q->id,
                    "   - received %d baseband samples (%d B). %d samples available.\n",
                    NBYTES2NSAMPLES(n),
                    n,
                    NBYTES2NSAMPLES(srsran_ringbuffer_status(&q->ringbuffer)));
      }
    }
  }

  return NULL;
}

int rf_zmq_rx_open(rf_zmq_rx_t* q, rf_zmq_opts_t opts, void* zmq_ctx, char* sock_args)
{
  int ret = SRSRAN_ERROR;

  if (q) {
    // Zero object
    bzero(q, sizeof(rf_zmq_rx_t));

    // Copy id
    strncpy(q->id, opts.id, ZMQ_ID_STRLEN - 1);
    q->id[ZMQ_ID_STRLEN - 1] = '\0';

    // Create socket
    q->sock = zmq_socket(zmq_ctx, opts.socket_type);
    if (!q->sock) {
      fprintf(stderr, "[zmq] Error: creating transmitter socket\n");
      goto clean_exit;
    }
    q->socket_type        = opts.socket_type;
    q->sample_format      = opts.sample_format;
    q->frequency_mhz      = opts.frequency_mhz;
    q->fail_on_disconnect = opts.fail_on_disconnect;
    q->sample_offset      = opts.sample_offset;
    q->trx_timeout_ms     = opts.trx_timeout_ms;
    q->log_trx_timeout    = opts.log_trx_timeout;

    if (opts.socket_type == ZMQ_SUB) {
      zmq_setsockopt(q->sock, ZMQ_SUBSCRIBE, "", 0);
    }

#if ZMQ_MONITOR
    // Monitor all events (monitoring only works over inproc://)
    ret = zmq_socket_monitor(q->sock, "inproc://monitor-client", ZMQ_EVENT_ALL);
    if (ret == -1) {
      fprintf(stderr, "Error: creating socket monitor: %s\n", zmq_strerror(zmq_errno()));
      goto clean_exit;
    }

    // create socket socket for monitoring and connect monitor
    q->socket_monitor = zmq_socket(zmq_ctx, ZMQ_PAIR);
    ret               = zmq_connect(q->socket_monitor, "inproc://monitor-client");
    if (ret) {
      fprintf(stderr, "Error: connecting monitor socket: %s\n", zmq_strerror(zmq_errno()));
      goto clean_exit;
    }
#endif // ZMQ_MONITOR

    rf_zmq_info(q->id, "Connecting receiver: %s\n", sock_args);

    ret = zmq_connect(q->sock, sock_args);
    if (ret) {
      fprintf(stderr, "Error: connecting receiver socket: %s\n", zmq_strerror(zmq_errno()));
      goto clean_exit;
    }

    if (opts.trx_timeout_ms) {
      int timeout = opts.trx_timeout_ms;
      if (zmq_setsockopt(q->sock, ZMQ_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        fprintf(stderr, "Error: setting receive timeout on rx socket\n");
        goto clean_exit;
      }

      if (zmq_setsockopt(q->sock, ZMQ_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
        fprintf(stderr, "Error: setting receive timeout on rx socket\n");
        goto clean_exit;
      }

      timeout = 0;
      if (zmq_setsockopt(q->sock, ZMQ_LINGER, &timeout, sizeof(timeout)) == -1) {
        fprintf(stderr, "Error: setting linger timeout on rx socket\n");
        goto clean_exit;
      }
    }

    if (srsran_ringbuffer_init(&q->ringbuffer, ZMQ_MAX_BUFFER_SIZE)) {
      fprintf(stderr, "Error: initiating ringbuffer\n");
      goto clean_exit;
    }

    q->temp_buffer = srsran_vec_malloc(ZMQ_MAX_BUFFER_SIZE);
    if (!q->temp_buffer) {
      fprintf(stderr, "Error: allocating rx buffer\n");
      goto clean_exit;
    }

    q->temp_buffer_convert = srsran_vec_malloc(ZMQ_MAX_BUFFER_SIZE);
    if (!q->temp_buffer_convert) {
      fprintf(stderr, "Error: allocating rx buffer\n");
      goto clean_exit;
    }

    if (pthread_mutex_init(&q->mutex, NULL)) {
      fprintf(stderr, "Error: creating mutex\n");
      goto clean_exit;
    }

    q->running = true;
    if (pthread_create(&q->thread, NULL, rf_zmq_async_rx_thread, q)) {
      fprintf(stderr, "Error: creating thread\n");
      goto clean_exit;
    }

    ret = SRSRAN_SUCCESS;
  }

clean_exit:
  return ret;
}

int rf_zmq_rx_baseband(rf_zmq_rx_t* q, cf_t* buffer, uint32_t nsamples)
{
  void*    dst_buffer = buffer;
  uint32_t sample_sz  = sizeof(cf_t);
  if (q->sample_format != ZMQ_TYPE_FC32) {
    dst_buffer = q->temp_buffer_convert;
    sample_sz  = 2 * sizeof(short);
  }

  // If the read needs to be delayed
  while (q->sample_offset > 0) {
    uint32_t n_offset = SRSRAN_MIN(q->sample_offset, NBYTES2NSAMPLES(ZMQ_MAX_BUFFER_SIZE));
    srsran_vec_zero(q->temp_buffer, n_offset);
    int n = srsran_ringbuffer_write(&q->ringbuffer, q->temp_buffer, (int)(n_offset * sample_sz));
    if (n < SRSRAN_SUCCESS) {
      return n;
    }
    q->sample_offset -= n_offset;
  }

  // If the read needs to be advanced
  while (q->sample_offset < 0) {
    uint32_t n_offset = SRSRAN_MIN(-q->sample_offset, NBYTES2NSAMPLES(ZMQ_MAX_BUFFER_SIZE));
    int      n =
        srsran_ringbuffer_read_timed(&q->ringbuffer, q->temp_buffer, (int)(n_offset * sample_sz), q->trx_timeout_ms);
    if (n < SRSRAN_SUCCESS) {
      return n;
    }
    q->sample_offset += n_offset;
  }

  int n = srsran_ringbuffer_read_timed(&q->ringbuffer, dst_buffer, sample_sz * nsamples, q->trx_timeout_ms);
  if (n < 0) {
    return n;
  }

  if (q->sample_format == ZMQ_TYPE_SC16) {
    srsran_vec_convert_if(dst_buffer, INT16_MAX, (float*)buffer, 2 * nsamples);
  }

  return n;
}

bool rf_zmq_rx_match_freq(rf_zmq_rx_t* q, uint32_t freq_hz)
{
  bool ret = false;
  if (q) {
    ret = (q->frequency_mhz == 0 || q->frequency_mhz == freq_hz);
  }
  return ret;
}

void rf_zmq_rx_close(rf_zmq_rx_t* q)
{
  rf_zmq_info(q->id, "Closing ...\n");
  q->running = false;

  if (q->thread) {
    pthread_join(q->thread, NULL);
    pthread_detach(q->thread);
  }

  srsran_ringbuffer_free(&q->ringbuffer);

  if (q->temp_buffer) {
    free(q->temp_buffer);
  }

  if (q->temp_buffer_convert) {
    free(q->temp_buffer_convert);
  }

  if (q->sock) {
    zmq_close(q->sock);
    q->sock = NULL;
  }

#if ZMQ_MONITOR
  if (q->socket_monitor) {
    zmq_close(q->socket_monitor);
    q->socket_monitor = NULL;
  }
#endif // ZMQ_MONITOR
}
