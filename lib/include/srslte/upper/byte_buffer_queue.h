/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/*
 * @file byte_buffer_queue.h
 *
 * @brief Queue of unique pointers to byte buffers used in PDCP and RLC TX queues.
 *        Uses a blocking queue with bounded capacity to block higher layers
 *        when pushing uplink traffic
 */

#ifndef SRSLTE_BYTE_BUFFERQUEUE_H
#define SRSLTE_BYTE_BUFFERQUEUE_H

#include "srslte/common/block_queue.h"
#include "srslte/common/common.h"
#include <pthread.h>

namespace srslte {

class byte_buffer_queue : public block_queue<unique_byte_buffer_t>::call_mutexed_itf
{
public:
  byte_buffer_queue(int capacity = 128) : queue(capacity) { queue.set_mutexed_itf(this); }
  // increase/decrease unread_bytes inside push/pop mutexed operations
  void pushing(const unique_byte_buffer_t& msg) final { unread_bytes += msg->N_bytes; }
  void popping(const unique_byte_buffer_t& msg) final
  {
    if (unread_bytes > msg->N_bytes) {
      unread_bytes -= msg->N_bytes;
    } else {
      unread_bytes = 0;
    }
  }
  void write(unique_byte_buffer_t msg) { queue.push(std::move(msg)); }

  srslte::error_type<unique_byte_buffer_t> try_write(unique_byte_buffer_t&& msg)
  {
    return queue.try_push(std::move(msg));
  }

  unique_byte_buffer_t read() { return queue.wait_pop(); }

  bool try_read(unique_byte_buffer_t* msg) { return queue.try_pop(msg); }

  void     resize(uint32_t capacity) { queue.resize(capacity); }
  uint32_t size() { return (uint32_t)queue.size(); }

  uint32_t size_bytes() { return unread_bytes; }

  uint32_t size_tail_bytes()
  {
    if (!queue.empty()) {
      const unique_byte_buffer_t& m = queue.front();
      if (m.get()) {
        return m->N_bytes;
      }
    }
    return 0;
  }

  // This is a hack to reset N_bytes counter when queue is corrupted (see line 89)
  void reset() { unread_bytes = 0; }

  bool is_empty() { return queue.empty(); }

  bool is_full() { return queue.full(); }

private:
  block_queue<unique_byte_buffer_t> queue;
  uint32_t                          unread_bytes = 0;
};

} // namespace srslte

#endif // SRSLTE_BYTE_BUFFERQUEUE_H
