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
 *  File:         rlc_tx_queue.h
 *  Description:  Queue used in RLC TM/UM/AM TX queues.
 *                Uses a blocking queue with bounded capacity to block higher layers
 *                when pushing Uplink traffic
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_MSG_QUEUE_H
#define SRSLTE_MSG_QUEUE_H

#include "srslte/common/block_queue.h"
#include "srslte/common/common.h"
#include <pthread.h>

namespace srslte {

class rlc_tx_queue : public block_queue<byte_buffer_t*>::call_mutexed_itf
{
public:
  rlc_tx_queue(int capacity = 128) : queue(capacity) {
    unread_bytes = 0;
    queue.set_mutexed_itf(this);
  }
  // increase/decrease unread_bytes inside push/pop mutexed operations
  void pushing(byte_buffer_t *msg) {
    unread_bytes += msg->N_bytes;
  }
  void popping(byte_buffer_t *msg) {
    if (unread_bytes > msg->N_bytes) {
      unread_bytes -= msg->N_bytes;
    } else {
      unread_bytes = 0;
    }
  }
  void write(byte_buffer_t *msg)
  {
    queue.push(msg);
  }

  bool try_write(byte_buffer_t *msg)
  {
    return queue.try_push(msg);
  }

  void read(byte_buffer_t **msg)
  {
    byte_buffer_t *m = queue.wait_pop();
    *msg = m;
  }

  bool try_read(byte_buffer_t **msg)
  {
    return queue.try_pop(msg);
  }

  void resize(uint32_t capacity)
  {
    queue.resize(capacity);
  }
  uint32_t size()
  {
    return (uint32_t) queue.size();
  }

  uint32_t size_bytes()
  {
    return unread_bytes;
  }

  uint32_t size_tail_bytes()
  {
    if (!queue.empty()) {
      byte_buffer_t *m = queue.front();
      if (m) {
        return m->N_bytes;
      }
    }
    return 0; 
  }

  // This is a hack to reset N_bytes counter when queue is corrupted (see line 89)
  void reset() {
    unread_bytes = 0;
  }

private:
  bool                        is_empty() { return queue.empty(); }

  block_queue<byte_buffer_t*> queue;
  uint32_t                    unread_bytes;
};

} // namespace srslte


#endif // SRSLTE_MSG_QUEUE_H
