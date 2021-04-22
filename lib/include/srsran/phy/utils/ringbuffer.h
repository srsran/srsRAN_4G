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

#ifndef SRSRAN_RINGBUFFER_H
#define SRSRAN_RINGBUFFER_H

#include "srsran/config.h"
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
} srsran_ringbuffer_t;

#ifdef __cplusplus
extern "C" {
#endif

SRSRAN_API int srsran_ringbuffer_init(srsran_ringbuffer_t* q, int capacity);

SRSRAN_API void srsran_ringbuffer_free(srsran_ringbuffer_t* q);

SRSRAN_API void srsran_ringbuffer_reset(srsran_ringbuffer_t* q);

SRSRAN_API int srsran_ringbuffer_status(srsran_ringbuffer_t* q);

SRSRAN_API int srsran_ringbuffer_space(srsran_ringbuffer_t* q);

SRSRAN_API int srsran_ringbuffer_resize(srsran_ringbuffer_t* q, int capacity);

// write to the buffer immediately, if there isnt enough space it will overflow
SRSRAN_API int srsran_ringbuffer_write(srsran_ringbuffer_t* q, void* ptr, int nof_bytes);

// block forever until there is enough space then write to buffer
SRSRAN_API int srsran_ringbuffer_write_block(srsran_ringbuffer_t* q, void* ptr, int nof_bytes);

// block for timeout_ms milliseconds, then either  write to buffer if there is space or return an error without writing
SRSRAN_API int srsran_ringbuffer_write_timed(srsran_ringbuffer_t* q, void* ptr, int nof_bytes, int32_t timeout_ms);

SRSRAN_API int
srsran_ringbuffer_write_timed_block(srsran_ringbuffer_t* q, void* ptr, int nof_bytes, int32_t timeout_ms);

// read from buffer, blocking until there is enough samples
SRSRAN_API int srsran_ringbuffer_read(srsran_ringbuffer_t* q, void* ptr, int nof_bytes);

// read from buffer, blocking for timeout_ms milliseconds until there is enough samples or return an error
SRSRAN_API int srsran_ringbuffer_read_timed(srsran_ringbuffer_t* q, void* p, int nof_bytes, int32_t timeout_ms);

SRSRAN_API int srsran_ringbuffer_read_timed_block(srsran_ringbuffer_t* q, void* p, int nof_bytes, int32_t timeout_ms);

// read samples from the buffer, convert them from uint16_t to cplx float and get the conjugate
SRSRAN_API int srsran_ringbuffer_read_convert_conj(srsran_ringbuffer_t* q, cf_t* dst_ptr, float norm, int nof_samples);

SRSRAN_API int srsran_ringbuffer_read_block(srsran_ringbuffer_t* q, void** p, int nof_bytes, int32_t timeout_ms);

SRSRAN_API void srsran_ringbuffer_stop(srsran_ringbuffer_t* q);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_RINGBUFFER_H
