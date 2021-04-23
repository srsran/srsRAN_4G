/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/******************************************************************************
 *  File:         multiqueue.h
 *  Description:  General-purpose non-blocking multiqueue. It behaves as a list
 *                of bounded/unbounded queues.
 *****************************************************************************/

#ifndef SRSRAN_MULTIQUEUE_H
#define SRSRAN_MULTIQUEUE_H

#include "srsran/adt/circular_buffer.h"
#include "srsran/adt/move_callback.h"
#include <algorithm>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <vector>

namespace srsran {

#define MULTIQUEUE_DEFAULT_CAPACITY (8192) // Default per-queue capacity

// template <typename myobj>
// class multiqueue_handler
//{
//   class circular_buffer
//   {
//   public:
//     circular_buffer(uint32_t cap) : buffer(cap + 1) {}
//     circular_buffer(circular_buffer&& other) noexcept
//     {
//       active       = other.active;
//       other.active = false;
//       widx         = other.widx;
//       ridx         = other.ridx;
//       buffer       = std::move(other.buffer);
//     }
//
//     std::condition_variable cv_full;
//     bool                    active = true;
//
//     bool   empty() const { return widx == ridx; }
//     size_t size() const { return widx >= ridx ? widx - ridx : widx + (buffer.size() - ridx); }
//     bool   full() const { return (ridx > 0) ? widx == ridx - 1 : widx == buffer.size() - 1; }
//     size_t capacity() const { return buffer.size() - 1; }
//
//     template <typename T>
//     void push(T&& o) noexcept
//     {
//       buffer[widx++] = std::forward<T>(o);
//       if (widx >= buffer.size()) {
//         widx = 0;
//       }
//     }
//
//     void pop() noexcept
//     {
//       ridx++;
//       if (ridx >= buffer.size()) {
//         ridx = 0;
//       }
//     }
//
//     myobj&       front() noexcept { return buffer[ridx]; }
//     const myobj& front() const noexcept { return buffer[ridx]; }
//
//   private:
//     std::vector<myobj> buffer;
//     size_t             widx = 0, ridx = 0;
//   };
//
// public:
//   class queue_handle
//   {
//   public:
//     queue_handle() = default;
//     queue_handle(multiqueue_handler<myobj>* parent_, int id) : parent(parent_), queue_id(id) {}
//     template <typename FwdRef>
//     void push(FwdRef&& value)
//     {
//       parent->push(queue_id, std::forward<FwdRef>(value));
//     }
//     bool                   try_push(const myobj& value) { return parent->try_push(queue_id, value); }
//     std::pair<bool, myobj> try_push(myobj&& value) { return parent->try_push(queue_id, std::move(value)); }
//     size_t                 size() { return parent->size(queue_id); }
//
//   private:
//     multiqueue_handler<myobj>* parent   = nullptr;
//     int                        queue_id = -1;
//   };
//
//   explicit multiqueue_handler(uint32_t capacity_ = MULTIQUEUE_DEFAULT_CAPACITY) : capacity(capacity_) {}
//   ~multiqueue_handler() { reset(); }
//
//   void reset()
//   {
//     std::unique_lock<std::mutex> lock(mutex);
//     running = false;
//     while (nof_threads_waiting > 0) {
//       uint32_t size = queues.size();
//       cv_empty.notify_one();
//       for (uint32_t i = 0; i < size; ++i) {
//         queues[i].cv_full.notify_all();
//       }
//       // wait for all threads to unblock
//       cv_exit.wait(lock);
//     }
//     queues.clear();
//   }
//
//   /**
//    * Adds a new queue with fixed capacity
//    * @param capacity_ The capacity of the queue.
//    * @return The index of the newly created (or reused) queue within the vector of queues.
//    */
//   int add_queue(uint32_t capacity_)
//   {
//     uint32_t                    qidx = 0;
//     std::lock_guard<std::mutex> lock(mutex);
//     if (not running) {
//       return -1;
//     }
//     for (; qidx < queues.size() and queues[qidx].active; ++qidx)
//       ;
//
//     // check if there is a free queue of the required size
//     if (qidx == queues.size() || queues[qidx].capacity() != capacity_) {
//       // create new queue
//       queues.emplace_back(capacity_);
//       qidx = queues.size() - 1; // update qidx to the last element
//     } else {
//       queues[qidx].active = true;
//     }
//     return (int)qidx;
//   }
//
//   /**
//    * Add queue using the default capacity of the underlying multiqueue
//    * @return The queue index
//    */
//   int add_queue() { return add_queue(capacity); }
//
//   int nof_queues()
//   {
//     std::lock_guard<std::mutex> lock(mutex);
//     uint32_t                    count = 0;
//     for (uint32_t i = 0; i < queues.size(); ++i) {
//       count += queues[i].active ? 1 : 0;
//     }
//     return count;
//   }
//
//   template <typename FwdRef>
//   void push(int q_idx, FwdRef&& value)
//   {
//     {
//       std::unique_lock<std::mutex> lock(mutex);
//       while (is_queue_active_(q_idx) and queues[q_idx].full()) {
//         nof_threads_waiting++;
//         queues[q_idx].cv_full.wait(lock);
//         nof_threads_waiting--;
//       }
//       if (not is_queue_active_(q_idx)) {
//         cv_exit.notify_one();
//         return;
//       }
//       queues[q_idx].push(std::forward<FwdRef>(value));
//     }
//     cv_empty.notify_one();
//   }
//
//   bool try_push(int q_idx, const myobj& value)
//   {
//     {
//       std::lock_guard<std::mutex> lock(mutex);
//       if (not is_queue_active_(q_idx) or queues[q_idx].full()) {
//         return false;
//       }
//       queues[q_idx].push(value);
//     }
//     cv_empty.notify_one();
//     return true;
//   }
//
//   std::pair<bool, myobj> try_push(int q_idx, myobj&& value)
//   {
//     {
//       std::lock_guard<std::mutex> lck(mutex);
//       if (not is_queue_active_(q_idx) or queues[q_idx].full()) {
//         return {false, std::move(value)};
//       }
//       queues[q_idx].push(std::move(value));
//     }
//     cv_empty.notify_one();
//     return {true, std::move(value)};
//   }
//
//   int wait_pop(myobj* value)
//   {
//     std::unique_lock<std::mutex> lock(mutex);
//     while (running) {
//       if (round_robin_pop_(value)) {
//         if (nof_threads_waiting > 0) {
//           lock.unlock();
//           queues[spin_idx].cv_full.notify_one();
//         }
//         return spin_idx;
//       }
//       nof_threads_waiting++;
//       cv_empty.wait(lock);
//       nof_threads_waiting--;
//     }
//     cv_exit.notify_one();
//     return -1;
//   }
//
//   int try_pop(myobj* value)
//   {
//     std::unique_lock<std::mutex> lock(mutex);
//     if (running) {
//       if (round_robin_pop_(value)) {
//         if (nof_threads_waiting > 0) {
//           lock.unlock();
//           queues[spin_idx].cv_full.notify_one();
//         }
//         return spin_idx;
//       }
//       // didn't find any task
//       return -1;
//     }
//     cv_exit.notify_one();
//     return -1;
//   }
//
//   bool empty(int qidx)
//   {
//     std::lock_guard<std::mutex> lck(mutex);
//     return queues[qidx].empty();
//   }
//
//   size_t size(int qidx)
//   {
//     std::lock_guard<std::mutex> lck(mutex);
//     return queues[qidx].size();
//   }
//
//   size_t max_size(int qidx)
//   {
//     std::lock_guard<std::mutex> lck(mutex);
//     return queues[qidx].capacity();
//   }
//
//   const myobj& front(int qidx)
//   {
//     std::lock_guard<std::mutex> lck(mutex);
//     return queues[qidx].front();
//   }
//
//   void erase_queue(int qidx)
//   {
//     std::lock_guard<std::mutex> lck(mutex);
//     if (is_queue_active_(qidx)) {
//       queues[qidx].active = false;
//       while (not queues[qidx].empty()) {
//         queues[qidx].pop();
//       }
//     }
//   }
//
//   bool is_queue_active(int qidx)
//   {
//     std::lock_guard<std::mutex> lck(mutex);
//     return is_queue_active_(qidx);
//   }
//
//   queue_handle get_queue_handler() { return {this, add_queue()}; }
//   queue_handle get_queue_handler(uint32_t size) { return {this, add_queue(size)}; }
//
// private:
//   bool is_queue_active_(int qidx) const { return running and queues[qidx].active; }
//
//   bool round_robin_pop_(myobj* value)
//   {
//     // Round-robin for all queues
//     for (const circular_buffer& q : queues) {
//       spin_idx = (spin_idx + 1) % queues.size();
//       if (is_queue_active_(spin_idx) and not queues[spin_idx].empty()) {
//         if (value) {
//           *value = std::move(queues[spin_idx].front());
//         }
//         queues[spin_idx].pop();
//         return true;
//       }
//     }
//     return false;
//   }
//
//   std::mutex                   mutex;
//   std::condition_variable      cv_empty, cv_exit;
//   uint32_t                     spin_idx = 0;
//   bool                         running  = true;
//   std::vector<circular_buffer> queues;
//   uint32_t                     capacity            = 0;
//   uint32_t                     nof_threads_waiting = 0;
// };

/**
 * N-to-1 Message-Passing Broker that manages the creation, destruction of input ports, and popping of messages that
 * are pushed to these ports.
 * Each port provides a thread-safe push(...) / try_push(...) interface to enqueue messages
 * The class will pop from the several created ports in a round-robin fashion.
 * The popping() interface is not safe-thread. That means, that it is expected that only one thread will
 * be popping tasks.
 * @tparam myobj message type
 */
template <typename myobj>
class multiqueue_handler
{
  class input_port_impl
  {
  public:
    input_port_impl(uint32_t cap, multiqueue_handler<myobj>* parent_) : buffer(cap), parent(parent_) {}
    input_port_impl(input_port_impl&& other) noexcept
    {
      std::lock_guard<std::mutex> lock(other.q_mutex);
      active_       = other.active_;
      parent        = other.parent_;
      other.active_ = false;
      buffer        = std::move(other.buffer);
    }
    ~input_port_impl() { set_active_blocking(false); }

    size_t capacity() const { return buffer.max_size(); }
    size_t size() const
    {
      std::lock_guard<std::mutex> lock(q_mutex);
      return buffer.size();
    }
    bool active() const
    {
      std::lock_guard<std::mutex> lock(q_mutex);
      return active_;
    }

    void set_active(bool val)
    {
      std::unique_lock<std::mutex> lock(q_mutex);
      if (val == active_) {
        return;
      }
      active_ = val;

      if (not active_) {
        buffer.clear();
        lock.unlock();
        cv_full.notify_all();
      }
    }

    void set_active_blocking(bool val)
    {
      set_active(val);

      if (not val) {
        // wait for all the pushers to unlock
        std::unique_lock<std::mutex> lock(q_mutex);
        while (nof_waiting > 0) {
          cv_exit.wait(lock);
        }
      }
    }

    template <typename T>
    void push(T&& o) noexcept
    {
      push_(&o, true);
    }

    bool try_push(const myobj& o) { return push_(&o, false); }

    srsran::error_type<myobj> try_push(myobj&& o)
    {
      if (push_(&o, false)) {
        return {};
      }
      return {std::move(o)};
    }

    bool try_pop(myobj& obj)
    {
      std::unique_lock<std::mutex> lock(q_mutex);
      if (buffer.empty()) {
        return false;
      }
      obj = std::move(buffer.top());
      buffer.pop();
      if (nof_waiting > 0) {
        lock.unlock();
        cv_full.notify_one();
      }
      return true;
    }

  private:
    template <typename T>
    bool push_(T* o, bool blocking) noexcept
    {
      {
        std::unique_lock<std::mutex> lock(q_mutex);
        while (active_ and blocking and buffer.full()) {
          nof_waiting++;
          cv_full.wait(lock);
          nof_waiting--;
        }
        if (not active_) {
          lock.unlock();
          cv_exit.notify_one();
          return false;
        }
        buffer.push(std::forward<T>(*o));
      }
      parent->cv_empty.notify_one();
      return true;
    }

    multiqueue_handler<myobj>* parent = nullptr;

    mutable std::mutex                 q_mutex;
    srsran::dyn_circular_buffer<myobj> buffer;
    std::condition_variable            cv_full, cv_exit;
    bool                               active_     = true;
    int                                nof_waiting = 0;
  };

public:
  class queue_handle
  {
  public:
    queue_handle() = default;
    queue_handle(input_port_impl* impl_) : impl(impl_) {}
    template <typename FwdRef>
    void push(FwdRef&& value)
    {
      impl->push(std::forward<FwdRef>(value));
    }
    bool                      try_push(const myobj& value) { return impl->try_push(value); }
    srsran::error_type<myobj> try_push(myobj&& value) { return impl->try_push(std::move(value)); }
    void                      reset()
    {
      if (impl != nullptr) {
        impl->set_active_blocking(false);
        impl = nullptr;
      }
    }

    size_t size() { return impl->size(); }
    size_t capacity() { return impl->capacity(); }
    bool   active() const { return impl != nullptr and impl->active(); }
    bool   empty() const { return impl->size() == 0; }

    bool operator==(const queue_handle& other) const { return impl == other.impl; }
    bool operator!=(const queue_handle& other) const { return impl != other.impl; }

  private:
    input_port_impl* impl = nullptr;
  };

  explicit multiqueue_handler(uint32_t default_capacity_ = MULTIQUEUE_DEFAULT_CAPACITY) : capacity(default_capacity_) {}
  ~multiqueue_handler() { reset(); }

  void reset()
  {
    std::unique_lock<std::mutex> lock(mutex);
    running = false;
    for (auto& q : queues) {
      // signal deactivation to pushing threads in a non-blocking way
      q.set_active(false);
    }
    while (wait_pop_state) {
      cv_empty.notify_one();
      cv_exit.wait(lock);
    }
    for (auto& q : queues) {
      // ensure that all queues are completed with the deactivation before clearing the memory
      q.set_active_blocking(false);
    }
    queues.clear();
  }

  /**
   * Adds a new queue with fixed capacity
   * @param capacity_ The capacity of the queue.
   * @return The index of the newly created (or reused) queue within the vector of queues.
   */
  queue_handle add_queue(uint32_t capacity_)
  {
    uint32_t                    qidx = 0;
    std::lock_guard<std::mutex> lock(mutex);
    if (not running) {
      return queue_handle();
    }
    for (; qidx < queues.size() and (queues[qidx].active() or (queues[qidx].capacity() != capacity_)); ++qidx)
      ;

    // check if there is a free queue of the required size
    if (qidx == queues.size()) {
      // create new queue
      queues.emplace_back(capacity_, this);
      qidx = queues.size() - 1; // update qidx to the last element
    } else {
      queues[qidx].set_active(true);
    }
    return queue_handle(&queues[qidx]);
  }

  /**
   * Add queue using the default capacity of the underlying multiqueue
   * @return The queue index
   */
  queue_handle add_queue() { return add_queue(capacity); }

  uint32_t nof_queues() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    uint32_t                    count = 0;
    for (uint32_t i = 0; i < queues.size(); ++i) {
      count += queues[i].active() ? 1 : 0;
    }
    return count;
  }

  bool wait_pop(myobj* value)
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (running) {
      if (round_robin_pop_(value)) {
        return true;
      }
      wait_pop_state = true;
      cv_empty.wait(lock);
      wait_pop_state = false;
    }
    if (not running) {
      cv_exit.notify_one();
    }
    return false;
  }

  bool try_pop(myobj* value)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (running and round_robin_pop_(value)) {
      return true;
    }
    return false;
  }

private:
  bool round_robin_pop_(myobj* value)
  {
    // Round-robin for all queues
    auto     it    = queues.begin() + spin_idx;
    uint32_t count = 0;
    for (; count < queues.size(); ++count, ++it) {
      if (it == queues.end()) {
        it = queues.begin(); // wrap-around
      }
      if (it->try_pop(*value)) {
        spin_idx = (spin_idx + count + 1) % queues.size();
        return true;
      }
    }
    return false;
  }

  mutable std::mutex          mutex;
  std::condition_variable     cv_empty, cv_exit;
  uint32_t                    spin_idx = 0;
  bool                        running = true, wait_pop_state = false;
  std::deque<input_port_impl> queues;
  uint32_t                    capacity = 0;
};

template <typename T>
using queue_handle = typename multiqueue_handler<T>::queue_handle;

//! Specialization for tasks
using task_multiqueue   = multiqueue_handler<move_task_t>;
using task_queue_handle = task_multiqueue::queue_handle;

} // namespace srsran

#endif // SRSRAN_MULTIQUEUE_H
