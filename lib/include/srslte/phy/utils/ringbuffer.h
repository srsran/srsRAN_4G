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

#ifndef SRSLTE_RINGBUFFER_H
#define SRSLTE_RINGBUFFER_H

#include "srslte/config.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t*        buffer;
  bool            active;
  int             capacity;
  int             count;
  int             wpm;
  int             rpm;
  pthread_mutex_t mutex;
  pthread_cond_t  write_cvar;
  pthread_cond_t  read_cvar;
} srslte_ringbuffer_t;

#ifdef __cplusplus
extern "C" {
#endif

SRSLTE_API int srslte_ringbuffer_init(srslte_ringbuffer_t* q, int capacity);

SRSLTE_API void srslte_ringbuffer_free(srslte_ringbuffer_t* q);

SRSLTE_API void srslte_ringbuffer_reset(srslte_ringbuffer_t* q);

SRSLTE_API int srslte_ringbuffer_status(srslte_ringbuffer_t* q);

SRSLTE_API int srslte_ringbuffer_space(srslte_ringbuffer_t* q);

SRSLTE_API int srslte_ringbuffer_resize(srslte_ringbuffer_t* q, int capacity);

// write to the buffer immediately, if there isnt enough space it will overflow
SRSLTE_API int srslte_ringbuffer_write(srslte_ringbuffer_t* q, void* ptr, int nof_bytes);

// block forever until there is enough space then write to buffer
SRSLTE_API int srslte_ringbuffer_write_block(srslte_ringbuffer_t* q, void* ptr, int nof_bytes);

// block for timeout_ms milliseconds, then either  write to buffer if there is space or return an error without writing
SRSLTE_API int srslte_ringbuffer_write_timed(srslte_ringbuffer_t* q, void* ptr, int nof_bytes, int32_t timeout_ms);

SRSLTE_API int
srslte_ringbuffer_write_timed_block(srslte_ringbuffer_t* q, void* ptr, int nof_bytes, int32_t timeout_ms);

// read from buffer, blocking until there is enough samples
SRSLTE_API int srslte_ringbuffer_read(srslte_ringbuffer_t* q, void* ptr, int nof_bytes);

// read from buffer, blocking for timeout_ms milliseconds until there is enough samples or return an error
SRSLTE_API int srslte_ringbuffer_read_timed(srslte_ringbuffer_t* q, void* p, int nof_bytes, int32_t timeout_ms);

SRSLTE_API int srslte_ringbuffer_read_timed_block(srslte_ringbuffer_t* q, void* p, int nof_bytes, int32_t timeout_ms);

// read samples from the buffer, convert them from uint16_t to cplx float and get the conjugate
SRSLTE_API int srslte_ringbuffer_read_convert_conj(srslte_ringbuffer_t* q, cf_t* dst_ptr, float norm, int nof_samples);

SRSLTE_API int srslte_ringbuffer_read_block(srslte_ringbuffer_t* q, void** p, int nof_bytes, int32_t timeout_ms);

SRSLTE_API void srslte_ringbuffer_stop(srslte_ringbuffer_t* q);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_RINGBUFFER_H
