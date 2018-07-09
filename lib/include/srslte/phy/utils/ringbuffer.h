
#ifndef SRSLTE_RINGBUFFER_H
#define SRSLTE_RINGBUFFER_H

#include "srslte/config.h"
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint8_t *buffer;
  bool active;
  int capacity; 
  int count; 
  int wpm; 
  int rpm; 
  pthread_mutex_t mutex; 
  pthread_cond_t  cvar; 
} srslte_ringbuffer_t; 


SRSLTE_API int  srslte_ringbuffer_init(srslte_ringbuffer_t *q,
                                       int capacity);

SRSLTE_API void srslte_ringbuffer_free(srslte_ringbuffer_t *q);

SRSLTE_API void srslte_ringbuffer_reset(srslte_ringbuffer_t *q);

SRSLTE_API int  srslte_ringbuffer_status(srslte_ringbuffer_t *q);

SRSLTE_API int srslte_ringbuffer_space(srslte_ringbuffer_t *q);

SRSLTE_API int  srslte_ringbuffer_write(srslte_ringbuffer_t *q,
                                       void *ptr,
                                       int nof_bytes);

SRSLTE_API int  srslte_ringbuffer_read(srslte_ringbuffer_t *q,
                                       void *ptr,
                                       int nof_bytes);

SRSLTE_API void srslte_ringbuffer_stop(srslte_ringbuffer_t *q);

#endif // SRSLTE_RINGBUFFER_H


