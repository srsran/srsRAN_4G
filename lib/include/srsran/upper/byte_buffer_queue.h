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

/*
 * @file byte_buffer_queue.h
 *
 * @brief Queue of unique pointers to byte buffers used in PDCP and RLC TX queues.
 *        Uses a blocking queue with bounded capacity to block higher layers
 *        when pushing uplink traffic
 */

#ifndef SRSRAN_BYTE_BUFFERQUEUE_H
#define SRSRAN_BYTE_BUFFERQUEUE_H

#include "srsran/adt/circular_buffer.h"
#include "srsran/common/block_queue.h"
#include "srsran/common/byte_buffer.h"
#include "srsran/common/common.h"
#include <functional>
#include <pthread.h>

namespace srsran {

class byte_buffer_queue
{
public:
  byte_buffer_queue(int capacity = 128) :
    queue(capacity, push_callback(unread_bytes, n_sdus), pop_callback(unread_bytes, n_sdus))
  {}

  void write(unique_byte_buffer_t msg) { queue.push_blocking(std::move(msg)); }

  srsran::error_type<unique_byte_buffer_t> try_write(unique_byte_buffer_t&& msg)
  {
    return queue.try_push(std::move(msg));
  }

  unique_byte_buffer_t read() { return queue.pop_blocking(); }

  bool try_read(unique_byte_buffer_t* msg) { return queue.try_pop(*msg); }

  void     resize(uint32_t capacity) { queue.set_size(capacity); }
  uint32_t size() { return (uint32_t)queue.size(); }
  uint32_t get_n_sdus() { return n_sdus; }

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

  template <typename F>
  bool apply_first(const F& func)
  {
    return queue.apply_first(func);
  }

private:
  struct push_callback {
    explicit push_callback(std::atomic<uint32_t>& unread_bytes_, std::atomic<uint32_t>& n_sdus_) :
      unread_bytes(unread_bytes_), n_sdus(n_sdus_)
    {}
    void operator()(const unique_byte_buffer_t& msg)
    {
      unread_bytes.fetch_add(msg->N_bytes, std::memory_order_relaxed);
      n_sdus.fetch_add(1, std::memory_order_relaxed);
    }
    std::atomic<uint32_t>& unread_bytes;
    std::atomic<uint32_t>& n_sdus;
  };
  struct pop_callback {
    explicit pop_callback(std::atomic<uint32_t>& unread_bytes_, std::atomic<uint32_t>& n_sdus_) :
      unread_bytes(unread_bytes_), n_sdus(n_sdus_)
    {}
    void operator()(const unique_byte_buffer_t& msg)
    {
      if (msg == nullptr) {
        return;
      }
      // non-atomic update of both state variables
      unread_bytes.fetch_sub(std::min(msg->N_bytes, unread_bytes.load(std::memory_order_relaxed)),
                             std::memory_order_relaxed);
      n_sdus.store(std::max(0, (int32_t)(n_sdus.load(std::memory_order_relaxed)) - 1), std::memory_order_relaxed);
    }
    std::atomic<uint32_t>& unread_bytes;
    std::atomic<uint32_t>& n_sdus;
  };

  std::atomic<uint32_t> unread_bytes = {0};
  std::atomic<uint32_t> n_sdus       = {0};

public:
  dyn_blocking_queue<unique_byte_buffer_t, push_callback, pop_callback> queue;
};

} // namespace srsran

#endif // SRSRAN_BYTE_BUFFERQUEUE_H
