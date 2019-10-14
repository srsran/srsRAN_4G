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

#ifndef SRSLTE_RF_ZMQ_IMP_TRX_H
#define SRSLTE_RF_ZMQ_IMP_TRX_H

#include <pthread.h>
#include <srslte/phy/utils/ringbuffer.h>
#include <stdbool.h>

/* Definitions */
#define VERBOSE (0)
#define NSAMPLES2NBYTES(X) (((uint32_t)(X)) * sizeof(cf_t))
#define NBYTES2NSAMPLES(X) ((X) / sizeof(cf_t))
#define ZMQ_MAX_BUFFER_SIZE (NSAMPLES2NBYTES(3072000)) // 10 subframes at 20 MHz
#define ZMQ_TIMEOUT_MS (1000)
#define ZMQ_BASERATE_DEFAULT_HZ (23040000)
#define ZMQ_ID_STRLEN 16

typedef struct {
  char            id[ZMQ_ID_STRLEN];
  void*           sock;
  uint64_t        nsamples;
  bool            running;
  pthread_mutex_t mutex;
  cf_t*           zeros;
} rf_zmq_tx_t;

typedef struct {
  char                id[ZMQ_ID_STRLEN];
  void*               sock;
  uint64_t            nsamples;
  bool                running;
  pthread_t           thread;
  pthread_mutex_t     mutex;
  srslte_ringbuffer_t ringbuffer;
  cf_t*               temp_buffer;
} rf_zmq_rx_t;

/*
 * Common functions
 */
SRSLTE_API void rf_zmq_info(char* id, const char* format, ...);

SRSLTE_API void rf_zmq_error(char* id, const char* format, ...);

SRSLTE_API int rf_zmq_handle_error(char* id, const char* text);

/*
 * Transmitter functions
 */
SRSLTE_API int rf_zmq_tx_open(rf_zmq_tx_t* q, const char* id, void* zmq_ctx, char* sock_args);

SRSLTE_API int rf_zmq_tx_align(rf_zmq_tx_t* q, uint64_t ts);

SRSLTE_API int rf_zmq_tx_baseband(rf_zmq_tx_t* q, cf_t* buffer, uint32_t nsamples);

SRSLTE_API void rf_zmq_tx_close(rf_zmq_tx_t* q);

/*
 * Receiver functions
 */
SRSLTE_API int rf_zmq_rx_open(rf_zmq_rx_t* q, char* id, void* zmq_ctx, char* sock_args);

SRSLTE_API int rf_zmq_rx_baseband(rf_zmq_rx_t* q, cf_t* buffer, uint32_t nsamples);

SRSLTE_API void rf_zmq_rx_close(rf_zmq_rx_t* q);

#endif // SRSLTE_RF_ZMQ_IMP_TRX_H
