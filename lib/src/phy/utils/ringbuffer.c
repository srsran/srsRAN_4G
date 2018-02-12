
#include <string.h>
#include <stdlib.h>

#include "srslte/phy/utils/ringbuffer.h"
#include "srslte/phy/utils/vector.h"

int srslte_ringbuffer_init(srslte_ringbuffer_t *q, int capacity)
{
  q->buffer = srslte_vec_malloc(capacity);
  if (!q->buffer) {
    return -1; 
  }
  q->active     = true;
  q->capacity   = capacity;
  srslte_ringbuffer_reset(q);
  
  pthread_mutex_init(&q->mutex, NULL); 
  pthread_cond_init(&q->cvar, NULL);
  
  return 0; 
}

void srslte_ringbuffer_free(srslte_ringbuffer_t *q)
{
  if (q) {
    srslte_ringbuffer_stop(q);
    if (q->buffer) {
      free(q->buffer);
      q->buffer = NULL; 
    }
    pthread_mutex_destroy(&q->mutex); 
    pthread_cond_destroy(&q->cvar);
  }
}

void srslte_ringbuffer_reset(srslte_ringbuffer_t *q)
{
  pthread_mutex_lock(&q->mutex);
  q->count = 0;
  q->wpm   = 0;
  q->rpm   = 0;
  pthread_mutex_unlock(&q->mutex);
}

int srslte_ringbuffer_status(srslte_ringbuffer_t *q)
{
  return q->count;
}

int srslte_ringbuffer_write(srslte_ringbuffer_t *q, void *p, int nof_bytes)
{
  uint8_t *ptr = (uint8_t*) p;
  int w_bytes = nof_bytes;
  pthread_mutex_lock(&q->mutex);
  if (!q->active) {
    pthread_mutex_unlock(&q->mutex);
    return 0;
  }
  if (q->count + w_bytes > q->capacity) {
    w_bytes = q->capacity - q->count; 
    fprintf(stderr, "Buffer overrun: lost %d bytes\n", nof_bytes - w_bytes);
  }
  if (w_bytes > q->capacity - q->wpm) {
    int x = q->capacity - q->wpm; 
    memcpy(&q->buffer[q->wpm], ptr, x);    
    memcpy(q->buffer, &ptr[x], w_bytes - x);    
  } else {
    memcpy(&q->buffer[q->wpm], ptr, w_bytes);    
  }
  q->wpm += w_bytes; 
  if (q->wpm >= q->capacity) {
    q->wpm -= q->capacity; 
  }
  q->count += w_bytes; 
  pthread_cond_broadcast(&q->cvar);
  pthread_mutex_unlock(&q->mutex);
  return w_bytes; 
}

int srslte_ringbuffer_read(srslte_ringbuffer_t *q, void *p, int nof_bytes)
{
  uint8_t *ptr = (uint8_t*) p;
  pthread_mutex_lock(&q->mutex);
  while(q->count < nof_bytes && q->active) {
    pthread_cond_wait(&q->cvar, &q->mutex);
  }
  if (!q->active) {
    pthread_mutex_unlock(&q->mutex);
    return 0;
  }
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
  pthread_mutex_unlock(&q->mutex);
  return nof_bytes; 
}

void srslte_ringbuffer_stop(srslte_ringbuffer_t *q) {
  pthread_mutex_lock(&q->mutex);
  pthread_cond_broadcast(&q->cvar);
  pthread_mutex_unlock(&q->mutex);
}

