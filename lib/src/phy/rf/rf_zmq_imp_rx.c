/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

#include "rf_zmq_imp_trx.h"
#include <srslte/phy/utils/vector.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <inttypes.h>

static void* rf_zmq_async_rx_thread(void* h)
{
  rf_zmq_rx_t* q = (rf_zmq_rx_t*)h;

  while (q->sock && q->running) {
    int     nbytes = 0;
    int     n      = SRSLTE_ERROR;
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
                "[zmq] Error: receiver expected <= %ld bytes and received %d at channel %d.\n",
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
        n = srslte_ringbuffer_write_timed(&q->ringbuffer, q->temp_buffer, nbytes, ZMQ_TIMEOUT_MS);
      }

      // Check write
      if (nbytes == n) {
        rf_zmq_info(q->id,
                    "   - received %d baseband samples (%d B). %d samples available.\n",
                    NBYTES2NSAMPLES(n),
                    n,
                    NBYTES2NSAMPLES(srslte_ringbuffer_status(&q->ringbuffer)));
      }
    }
  }

  return NULL;
}

int rf_zmq_rx_open(rf_zmq_rx_t* q, rf_zmq_opts_t opts, void* zmq_ctx, char* sock_args)
{
  int ret = SRSLTE_ERROR;

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
    q->socket_type   = opts.socket_type;
    q->sample_format = opts.sample_format;

    if (opts.socket_type == ZMQ_SUB) {
      zmq_setsockopt(q->sock, ZMQ_SUBSCRIBE, "", 0);
    }

    rf_zmq_info(q->id, "Connecting receiver: %s\n", sock_args);

    ret = zmq_connect(q->sock, sock_args);
    if (ret) {
      fprintf(stderr, "Error: connecting receiver socket: %s\n", zmq_strerror(zmq_errno()));
      goto clean_exit;
    }

#if ZMQ_TIMEOUT_MS
    int timeout = ZMQ_TIMEOUT_MS;
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
#endif

    if (srslte_ringbuffer_init(&q->ringbuffer, ZMQ_MAX_BUFFER_SIZE)) {
      fprintf(stderr, "Error: initiating ringbuffer\n");
      goto clean_exit;
    }

    q->temp_buffer = srslte_vec_malloc(ZMQ_MAX_BUFFER_SIZE);
    if (!q->temp_buffer) {
      fprintf(stderr, "Error: allocating rx buffer\n");
      goto clean_exit;
    }

    q->temp_buffer_convert = srslte_vec_malloc(ZMQ_MAX_BUFFER_SIZE);
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

    ret        = SRSLTE_SUCCESS;
  }

clean_exit:
  return ret;
}

int rf_zmq_rx_baseband(rf_zmq_rx_t* q, cf_t* buffer, uint32_t nsamples)
{
  void *dst_buffer   = buffer;
  uint32_t sample_sz = sizeof(cf_t);
  if (q->sample_format != ZMQ_TYPE_FC32) {
    dst_buffer = q->temp_buffer_convert;
    sample_sz  = 2*sizeof(short);
  }

  int n = srslte_ringbuffer_read_timed(&q->ringbuffer, dst_buffer, sample_sz*nsamples, ZMQ_TIMEOUT_MS);
  if (n < 0) {
    return n;
  }

  if (q->sample_format == ZMQ_TYPE_SC16) {
    srslte_vec_convert_if(dst_buffer, INT16_MAX, (float*) buffer, 2*nsamples);
  }

  return n;
}

void rf_zmq_rx_close(rf_zmq_rx_t* q)
{
  rf_zmq_info(q->id, "Closing ...\n");
  q->running = false;

  if (q->thread) {
    pthread_join(q->thread, NULL);
    pthread_detach(q->thread);
  }

  srslte_ringbuffer_free(&q->ringbuffer);

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
}
