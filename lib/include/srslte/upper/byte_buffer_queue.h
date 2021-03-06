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

#include "srslte/adt/circular_buffer.h"
#include "srslte/common/block_queue.h"
#include "srslte/common/common.h"
#include <pthread.h>

namespace srslte {

class byte_buffer_queue
{
public:
  byte_buffer_queue(int capacity = 128) : queue(capacity, push_callback(unread_bytes), pop_callback(unread_bytes)) {}

  void write(unique_byte_buffer_t msg) { queue.push_blocking(std::move(msg)); }

  srslte::error_type<unique_byte_buffer_t> try_write(unique_byte_buffer_t&& msg)
  {
    return queue.try_push(std::move(msg));
  }

  unique_byte_buffer_t read() { return queue.pop_blocking(); }

  bool try_read(unique_byte_buffer_t* msg) { return queue.try_pop(*msg); }

  void     resize(uint32_t capacity) { queue.set_size(capacity); }
  uint32_t size() { return (uint32_t)queue.size(); }

  uint32_t size_bytes() { return unread_bytes; }

  uint32_t size_tail_bytes()
  {
    uint32_t size_next = 0;
    queue.try_call_on_front([&size_next](const unique_byte_buffer_t& front_val) {
      if (front_val != nullptr) {
        size_next += front_val->N_bytes;
      }
    });
    return size_next;
  }

  // This is a hack to reset N_bytes counter when queue is corrupted (see line 89)
  void reset() { unread_bytes = 0; }

  bool is_empty() { return queue.empty(); }

  bool is_full() { return queue.full(); }

private:
  struct push_callback {
    explicit push_callback(uint32_t& unread_bytes_) : unread_bytes(&unread_bytes_) {}
    void      operator()(const unique_byte_buffer_t& msg) { *unread_bytes += msg->N_bytes; }
    uint32_t* unread_bytes;
  };
  struct pop_callback {
    explicit pop_callback(uint32_t& unread_bytes_) : unread_bytes(&unread_bytes_) {}
    void      operator()(const unique_byte_buffer_t& msg) { *unread_bytes -= std::min(msg->N_bytes, *unread_bytes); }
    uint32_t* unread_bytes;
  };

  dyn_block_queue<unique_byte_buffer_t, push_callback, pop_callback> queue;
  uint32_t                                                           unread_bytes = 0;
};

} // namespace srslte

#endif // SRSLTE_BYTE_BUFFERQUEUE_H
