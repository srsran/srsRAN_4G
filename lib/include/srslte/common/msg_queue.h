/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 *  File:         msg_queue.h
 *  Description:  Thread-safe bounded circular buffer of srsue_byte_buffer pointers.
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_MSG_QUEUE_H
#define SRSLTE_MSG_QUEUE_H

#include "srslte/common/common.h"
#include <pthread.h>

namespace srslte {

class msg_queue
{
public:
  msg_queue(uint32_t capacity_ = 128)
    :head(0)
    ,tail(0)
    ,unread(0)
    ,unread_bytes(0)
    ,capacity(capacity_)
  {
    buf = new byte_buffer_t*[capacity];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);
  }

  ~msg_queue()
  {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);
    delete [] buf;
  }

  void write(byte_buffer_t *msg)
  {
    pthread_mutex_lock(&mutex);
    while(is_full()) {
      pthread_cond_wait(&not_full, &mutex);
    }
    buf[head] = msg;
    head = (head+1)%capacity;
    unread++;
    unread_bytes += msg->N_bytes;
    
    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mutex);
  }

  void read(byte_buffer_t **msg)
  {
    pthread_mutex_lock(&mutex);
    while(is_empty()) {
      pthread_cond_wait(&not_empty, &mutex);
    }
    *msg = buf[tail];
    tail = (tail+1)%capacity;
    unread--;
    unread_bytes -= (*msg)->N_bytes;

    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&mutex);
  }

  bool try_read(byte_buffer_t **msg)
  {
    pthread_mutex_lock(&mutex);
    if(is_empty())
    {
      pthread_mutex_unlock(&mutex);
      return false;
    }else{
      *msg = buf[tail];
      tail = (tail+1)%capacity;
      unread--;
      unread_bytes -= (*msg)->N_bytes;
      pthread_cond_signal(&not_full);
      pthread_mutex_unlock(&mutex);
      return true;
    }
  }

  uint32_t size()
  {
    pthread_mutex_lock(&mutex);
    uint32_t r = unread;
    pthread_mutex_unlock(&mutex);
    return r; 
  }

  uint32_t size_bytes()
  {
    pthread_mutex_lock(&mutex);
    uint32_t r = unread_bytes;
    pthread_mutex_unlock(&mutex);
    return r; 
  }

  uint32_t size_tail_bytes()
  {
    pthread_mutex_lock(&mutex);
    uint32_t r = buf[tail]->N_bytes;
    pthread_mutex_unlock(&mutex);
    return r; 
  }

private:
  bool     is_empty() const { return unread == 0; }
  bool     is_full() const { return unread == capacity; }

  pthread_cond_t        not_empty;
  pthread_cond_t        not_full;
  pthread_mutex_t       mutex;
  byte_buffer_t **buf;
  uint32_t              capacity;
  uint32_t              unread;
  uint32_t              unread_bytes;
  uint32_t              head;
  uint32_t              tail;
};

} // namespace srslte


#endif // SRSLTE_MSG_QUEUE_H
