/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include <stdlib.h>
#include <string.h>

#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/ringbuffer.h"
#include "srsran/phy/utils/vector.h"

int srsran_ringbuffer_init(srsran_ringbuffer_t* q, int capacity)
{
  q->buffer = srsran_vec_malloc(capacity);
  if (!q->buffer) {
    return SRSRAN_ERROR;
  }
  q->active   = true;
  q->capacity = capacity;
  pthread_mutex_init(&q->mutex, NULL);
  pthread_cond_init(&q->write_cvar, NULL);
  pthread_cond_init(&q->read_cvar, NULL);
  srsran_ringbuffer_reset(q);

  return SRSRAN_SUCCESS;
}

void srsran_ringbuffer_free(srsran_ringbuffer_t* q)
{
  if (q) {
    srsran_ringbuffer_stop(q);
    if (q->buffer) {
      free(q->buffer);
      q->buffer = NULL;
    }
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->write_cvar);
    pthread_cond_destroy(&q->read_cvar);
  }
}

void srsran_ringbuffer_reset(srsran_ringbuffer_t* q)
{
  // Check first if it is initiated
  if (q->capacity != 0) {
    pthread_mutex_lock(&q->mutex);
    q->count = 0;
    q->wpm   = 0;
    q->rpm   = 0;
    pthread_mutex_unlock(&q->mutex);
  }
}

int srsran_ringbuffer_resize(srsran_ringbuffer_t* q, int capacity)
{
  if (q->buffer) {
    free(q->buffer);
    q->buffer = NULL;
  }
  srsran_ringbuffer_reset(q);
  q->buffer = srsran_vec_malloc(capacity);
  if (!q->buffer) {
    return SRSRAN_ERROR;
  }
  q->active   = true;
  q->capacity = capacity;

  return SRSRAN_SUCCESS;
}

int srsran_ringbuffer_status(srsran_ringbuffer_t* q)
{
  int status = 0;
  pthread_mutex_lock(&q->mutex);
  status = q->count;
  pthread_mutex_unlock(&q->mutex);
  return status;
}

int srsran_ringbuffer_space(srsran_ringbuffer_t* q)
{
  return q->capacity - q->count;
}

int srsran_ringbuffer_write(srsran_ringbuffer_t* q, void* ptr, int nof_bytes)
{
  return srsran_ringbuffer_write_timed_block(q, ptr, nof_bytes, 0);
}

int srsran_ringbuffer_write_timed(srsran_ringbuffer_t* q, void* ptr, int nof_bytes, int32_t timeout_ms)
{
  return srsran_ringbuffer_write_timed_block(q, ptr, nof_bytes, timeout_ms);
}

int srsran_ringbuffer_write_block(srsran_ringbuffer_t* q, void* ptr, int nof_bytes)
{
  return srsran_ringbuffer_write_timed_block(q, ptr, nof_bytes, -1);
}

int srsran_ringbuffer_write_timed_block(srsran_ringbuffer_t* q, void* p, int nof_bytes, int32_t timeout_ms)
{
  int             ret     = SRSRAN_SUCCESS;
  uint8_t*        ptr     = (uint8_t*)p;
  int             w_bytes = nof_bytes;
  struct timespec towait;
  struct timeval  now;

  if (q == NULL || q->buffer == NULL) {
    ERROR("Invalid inputs");
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Get current time and update timeout
  if (timeout_ms > 0) {
    gettimeofday(&now, NULL);
    towait.tv_sec  = now.tv_sec + timeout_ms / 1000U;
    towait.tv_nsec = (now.tv_usec + 1000UL * (timeout_ms % 1000U)) * 1000UL;
  }
  pthread_mutex_lock(&q->mutex);

  // Wait to have enough space in the buffer
  while (q->count + w_bytes > q->capacity && q->active && ret == SRSRAN_SUCCESS) {
    if (timeout_ms > 0) {
      ret = pthread_cond_timedwait(&q->read_cvar, &q->mutex, &towait);
    } else if (timeout_ms < 0) {
      pthread_cond_wait(&q->read_cvar, &q->mutex);
    } else {
      w_bytes = q->capacity - q->count;
      ERROR("Buffer overrun: lost %d bytes", nof_bytes - w_bytes);
    }
  }
  if (ret == ETIMEDOUT) {
    ret = SRSRAN_ERROR_TIMEOUT;
  } else if (!q->active) {
    ret = SRSRAN_SUCCESS;
  } else if (ret == SRSRAN_SUCCESS) {
    if (ptr != NULL) {
      if (w_bytes > q->capacity - q->wpm) {
        int x = q->capacity - q->wpm;
        memcpy(&q->buffer[q->wpm], ptr, x);
        memcpy(q->buffer, &ptr[x], w_bytes - x);
      } else {
        memcpy(&q->buffer[q->wpm], ptr, w_bytes);
      }
    } else {
      if (w_bytes > q->capacity - q->wpm) {
        int x = q->capacity - q->wpm;
        memset(&q->buffer[q->wpm], 0, x);
        memset(q->buffer, 0, w_bytes - x);
      } else {
        memset(&q->buffer[q->wpm], 0, w_bytes);
      }
    }
    q->wpm += w_bytes;
    if (q->wpm >= q->capacity) {
      q->wpm -= q->capacity;
    }
    q->count += w_bytes;
    ret = w_bytes;
  } else {
    ret = SRSRAN_ERROR;
  }
  pthread_cond_broadcast(&q->write_cvar);
  pthread_mutex_unlock(&q->mutex);
  return ret;
}

int srsran_ringbuffer_read(srsran_ringbuffer_t* q, void* p, int nof_bytes)
{
  return srsran_ringbuffer_read_timed_block(q, p, nof_bytes, -1);
}

int srsran_ringbuffer_read_timed(srsran_ringbuffer_t* q, void* p, int nof_bytes, int32_t timeout_ms)
{
  return srsran_ringbuffer_read_timed_block(q, p, nof_bytes, timeout_ms);
}

int srsran_ringbuffer_read_timed_block(srsran_ringbuffer_t* q, void* p, int nof_bytes, int32_t timeout_ms)
{
  int             ret    = SRSRAN_SUCCESS;
  uint8_t*        ptr    = (uint8_t*)p;
  struct timespec towait = {};

  // Get current time and update timeout
  if (timeout_ms > 0) {
    struct timespec now = {};
    timespec_get(&now, TIME_UTC);

    // check nsec wrap-around
    towait.tv_sec = now.tv_sec + timeout_ms / 1000L;
    long nsec     = now.tv_nsec + ((timeout_ms % 1000U) * 1000UL);
    towait.tv_sec += nsec / 1000000000L;
    towait.tv_nsec = nsec % 1000000000L;
  }

  // Lock mutex
  pthread_mutex_lock(&q->mutex);

  // Wait for having enough samples
  while (q->count < nof_bytes && q->active && ret == SRSRAN_SUCCESS) {
    if (timeout_ms > 0) {
      ret = pthread_cond_timedwait(&q->write_cvar, &q->mutex, &towait);
    } else {
      pthread_cond_wait(&q->write_cvar, &q->mutex);
    }
  }

  if (ret == ETIMEDOUT) {
    ret = SRSRAN_ERROR_TIMEOUT;
  } else if (!q->active) {
    ret = SRSRAN_SUCCESS;
  } else if (ret == SRSRAN_SUCCESS) {
    if (nof_bytes + q->rpm > q->capacity) {
      int x = q->capacity - q->rpm;
      memcpy(ptr, &q->buffer[q->rpm], x);
      memcpy(&ptr[x], q->buffer, nof_bytes - x);
    } else {
      memcpy(ptr, &q->buffer[q->rpm], nof_bytes);
    }
    q->rpm += nof_bytes;
    if (q->rpm >= q->capacity) {
      q->rpm -= q->capacity;
    }
    q->count -= nof_bytes;
    ret = nof_bytes;
  } else if (ret == EINVAL) {
    fprintf(stderr, "Error: pthread_cond_timedwait() returned EINVAL, timeout value corrupted.\n");
    ret = SRSRAN_ERROR;
  } else {
    printf("ret=%d %s\n", ret, strerror(ret));
    ret = SRSRAN_ERROR;
  }

  // Unlock mutex
  pthread_cond_broadcast(&q->read_cvar);
  pthread_mutex_unlock(&q->mutex);

  return ret;
}

void srsran_ringbuffer_stop(srsran_ringbuffer_t* q)
{
  pthread_mutex_lock(&q->mutex);
  q->active = false;
  pthread_cond_broadcast(&q->write_cvar);
  pthread_cond_broadcast(&q->read_cvar);
  pthread_mutex_unlock(&q->mutex);
}

// Converts SC16 to cf_t
int srsran_ringbuffer_read_convert_conj(srsran_ringbuffer_t* q, cf_t* dst_ptr, float norm, int nof_samples)
{
  uint32_t nof_bytes = nof_samples * 4;

  pthread_mutex_lock(&q->mutex);
  while (q->count < nof_bytes && q->active) {
    pthread_cond_wait(&q->write_cvar, &q->mutex);
  }
  if (!q->active) {
    pthread_mutex_unlock(&q->mutex);
    return SRSRAN_ERROR;
  }

  int16_t* src = (int16_t*)&q->buffer[q->rpm];
  float*   dst = (float*)dst_ptr;

  if (nof_bytes + q->rpm > q->capacity) {
    int x = (q->capacity - q->rpm);
    srsran_vec_convert_if(src, norm, dst, x / 2);
    srsran_vec_convert_if((int16_t*)q->buffer, norm, &dst[x / 2], 2 * nof_samples - x / 2);
  } else {
    srsran_vec_convert_if(src, norm, dst, 2 * nof_samples);
  }
  srsran_vec_conj_cc(dst_ptr, dst_ptr, nof_samples);
  q->rpm += nof_bytes;
  if (q->rpm >= q->capacity) {
    q->rpm -= q->capacity;
  }
  q->count -= nof_bytes;
  pthread_cond_broadcast(&q->read_cvar);
  pthread_mutex_unlock(&q->mutex);
  return nof_samples;
}

/* For this function, the ring buffer capacity must be multiple of block size */
int srsran_ringbuffer_read_block(srsran_ringbuffer_t* q, void** p, int nof_bytes, int32_t timeout_ms)
{
  int             ret    = SRSRAN_SUCCESS;
  struct timespec towait = {};

  // Get current time and update timeout
  if (timeout_ms > 0) {
    struct timespec now = {};
    timespec_get(&now, TIME_UTC);

    // check nsec wrap-around
    towait.tv_sec = now.tv_sec + timeout_ms / 1000L;
    long nsec     = now.tv_nsec + ((timeout_ms % 1000U) * 1000UL);
    towait.tv_sec += nsec / 1000000000L;
    towait.tv_nsec = nsec % 1000000000L;
  }

  pthread_mutex_lock(&q->mutex);

  // Wait for having enough samples
  while (q->count < nof_bytes && q->active && ret == SRSRAN_SUCCESS) {
    if (timeout_ms > 0) {
      ret = pthread_cond_timedwait(&q->write_cvar, &q->mutex, &towait);
    } else {
      pthread_cond_wait(&q->write_cvar, &q->mutex);
    }
  }

  if (ret == ETIMEDOUT) {
    ret = SRSRAN_ERROR_TIMEOUT;
  } else if (!q->active) {
    ret = 0;
  } else if (ret == SRSRAN_SUCCESS) {
    *p = &q->buffer[q->rpm];

    q->count -= nof_bytes;
    q->rpm += nof_bytes;

    if (q->rpm >= q->capacity) {
      q->rpm -= q->capacity;
    }
    ret = nof_bytes;
  } else if (ret == EINVAL) {
    fprintf(stderr, "Error: pthread_cond_timedwait() returned EINVAL, timeout value corrupted.\n");
    ret = SRSRAN_ERROR;
  } else {
    printf("ret=%d %s\n", ret, strerror(ret));
    ret = SRSRAN_ERROR;
  }
  pthread_cond_broadcast(&q->read_cvar);
  pthread_mutex_unlock(&q->mutex);
  return ret;
}
