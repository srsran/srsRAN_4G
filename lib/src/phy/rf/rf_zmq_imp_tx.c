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

#include "rf_zmq_imp_trx.h"
#include <inttypes.h>
#include <srslte/config.h>
#include <srslte/phy/utils/vector.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

int rf_zmq_tx_open(rf_zmq_tx_t* q, rf_zmq_opts_t opts, void* zmq_ctx, char* sock_args)
{
  int ret = SRSLTE_ERROR;

  if (q) {
    // Zero object
    bzero(q, sizeof(rf_zmq_tx_t));

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
    q->frequency_mhz = opts.frequency_mhz;

    rf_zmq_info(q->id, "Binding transmitter: %s\n", sock_args);

    ret = zmq_bind(q->sock, sock_args);
    if (ret) {
      fprintf(stderr, "Error: connecting transmitter socket: %s\n", zmq_strerror(zmq_errno()));
      goto clean_exit;
    }

#if ZMQ_TIMEOUT_MS
    int timeout = ZMQ_TIMEOUT_MS;
    if (zmq_setsockopt(q->sock, ZMQ_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
      fprintf(stderr, "Error: setting receive timeout on tx socket\n");
      goto clean_exit;
    }

    if (zmq_setsockopt(q->sock, ZMQ_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
      fprintf(stderr, "Error: setting receive timeout on tx socket\n");
      goto clean_exit;
    }

    timeout = 0;
    if (zmq_setsockopt(q->sock, ZMQ_LINGER, &timeout, sizeof(timeout)) == -1) {
      fprintf(stderr, "Error: setting linger timeout on tx socket\n");
      goto clean_exit;
    }
#endif

    if (pthread_mutex_init(&q->mutex, NULL)) {
      fprintf(stderr, "Error: creating mutex\n");
      goto clean_exit;
    }

    q->temp_buffer_convert = srslte_vec_malloc(ZMQ_MAX_BUFFER_SIZE);
    if (!q->temp_buffer_convert) {
      fprintf(stderr, "Error: allocating rx buffer\n");
      goto clean_exit;
    }

    q->zeros = srslte_vec_malloc(ZMQ_MAX_BUFFER_SIZE);
    if (!q->zeros) {
      fprintf(stderr, "Error: allocating zeros\n");
      goto clean_exit;
    }
    bzero(q->zeros, ZMQ_MAX_BUFFER_SIZE);

    q->running = true;

    ret = SRSLTE_SUCCESS;
  }

clean_exit:
  return ret;
}

static int _rf_zmq_tx_baseband(rf_zmq_tx_t* q, cf_t* buffer, uint32_t nsamples)
{
  int n = SRSLTE_ERROR;

  while (n < 0 && q->running) {
    // Receive Transmit request is socket type is REPLY
    if (q->socket_type == ZMQ_REP) {
      uint8_t dummy;
      n = zmq_recv(q->sock, &dummy, sizeof(dummy), 0);
      if (n < 0) {
        if (rf_zmq_handle_error(q->id, "tx request receive")) {
          n = SRSLTE_ERROR;
          goto clean_exit;
        }
      } else {
        // Tx request received successful
        rf_zmq_info(q->id, " - tx request received\n");
        rf_zmq_info(q->id, " - sending %d samples (%d B)\n", nsamples, NSAMPLES2NBYTES(nsamples));
      }
    } else {
      n = 1;
    }

    // convert samples if necessary
    void*    buf       = (buffer) ? buffer : q->zeros;
    uint32_t sample_sz = sizeof(cf_t);

    if (q->sample_format == ZMQ_TYPE_SC16) {
      buf       = q->temp_buffer_convert;
      sample_sz = 2 * sizeof(short);
      srslte_vec_convert_fi((float*)buffer, INT16_MAX, (short*)q->temp_buffer_convert, 2 * nsamples);
    }

    // Send base-band if request was received
    if (n > 0) {
      n = zmq_send(q->sock, buf, (size_t)sample_sz * nsamples, 0);
      if (n < 0) {
        if (rf_zmq_handle_error(q->id, "tx baseband send")) {
          n = SRSLTE_ERROR;
          goto clean_exit;
        }
      } else if (n != NSAMPLES2NBYTES(nsamples)) {
        rf_zmq_error(q->id,
                     "[zmq] Error: transmitter expected %d bytes and sent %d. %s.\n",
                     NSAMPLES2NBYTES(nsamples),
                     n,
                     strerror(zmq_errno()));
        n = SRSLTE_ERROR;
        goto clean_exit;
      }
    }

    // If failed to receive request or send base-band, keep trying
  }

  // Increment sample counter
  q->nsamples += nsamples;
  n = nsamples;

clean_exit:
  return n;
}

int rf_zmq_tx_align(rf_zmq_tx_t* q, uint64_t ts)
{
  pthread_mutex_lock(&q->mutex);

  int64_t nsamples = (int64_t)ts - (int64_t)q->nsamples;

  if (nsamples > 0) {
    rf_zmq_info(q->id, " - Detected Tx gap of %d samples.\n", nsamples);
    _rf_zmq_tx_baseband(q, q->zeros, (uint32_t)nsamples);
  }

  pthread_mutex_unlock(&q->mutex);

  return (int)nsamples;
}

int rf_zmq_tx_baseband(rf_zmq_tx_t* q, cf_t* buffer, uint32_t nsamples)
{
  int n;

  pthread_mutex_lock(&q->mutex);

  n = _rf_zmq_tx_baseband(q, buffer, nsamples);

  pthread_mutex_unlock(&q->mutex);

  return n;
}

int rf_zmq_tx_zeros(rf_zmq_tx_t* q, uint32_t nsamples)
{
  pthread_mutex_lock(&q->mutex);

  rf_zmq_info(q->id, " - Tx %d Zeros.\n", nsamples);
  _rf_zmq_tx_baseband(q, q->zeros, (uint32_t)nsamples);

  pthread_mutex_unlock(&q->mutex);

  return (int)nsamples;
}

bool rf_zmq_tx_match_freq(rf_zmq_tx_t* q, uint32_t freq_hz)
{
  bool ret = false;
  if (q) {
    ret = (q->frequency_mhz == 0 || q->frequency_mhz == freq_hz);
  }
  return ret;
}

void rf_zmq_tx_close(rf_zmq_tx_t* q)
{
  q->running = false;

  if (q->zeros) {
    free(q->zeros);
  }

  if (q->temp_buffer_convert) {
    free(q->temp_buffer_convert);
  }

  if (q->sock) {
    zmq_close(q->sock);
    q->sock = NULL;
  }
}
