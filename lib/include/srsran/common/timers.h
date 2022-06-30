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

/******************************************************************************
 *  File:         timers.h
 *  Description:  Manually incremented timers. Call a callback function upon
 *                expiry.
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_TIMERS_H
#define SRSRAN_TIMERS_H

#include "srsran/adt/intrusive_list.h"
#include "srsran/adt/move_callback.h"
#include <algorithm>
#include <cstdint>
#include <deque>
#include <inttypes.h>
#include <limits>
#include <mutex>

namespace srsran {

class timer_callback
{
public:
  virtual void timer_expired(uint32_t timer_id) = 0;
};

/**
 * Class that manages stack timers. It allows creation of unique_timers with different ids. Each unique_timer duration,
 * and callback can be set via the set(...) method. A timer can be started/stopped via run()/stop() methods.
 * The timers access/alteration is thread-safe. Just beware non-atomic uses of its getters.
 * Internal Data structures:
 * - timer_list - std::deque that stores timer objects via push_back() to keep pointer/reference validity.
 *   The timer index in the timer_list matches the timer object id field.
 *   This deque will only grow in size. Erased timers are just tagged in the deque as empty, and can be reused for the
 *   creation of new timers. To avoid unnecessary runtime allocations, the user can set an initial capacity.
 * - free_list - intrusive forward linked list to keep track of the empty timers and speed up new timer creation.
 * - A large circular vector of size WHEEL_SIZE which works as a time wheel, storing and circularly indexing the
 *   currently running timers by their respective timeout value.
 *   For a number of running timers N, and uniform distribution of timeout values, the step_all() complexity
 *   should be O(N/WHEEL_SIZE). Thus, the performance should improve with a larger WHEEL_SIZE, at the expense of more
 *   used memory.
 */
class timer_handler
{
  using tic_diff_t                      = uint32_t;
  using tic_t                           = uint32_t;
  constexpr static uint32_t INVALID_ID  = std::numeric_limits<uint32_t>::max();
  constexpr static size_t   WHEEL_SHIFT = 16U;
  constexpr static size_t   WHEEL_SIZE  = 1U << WHEEL_SHIFT;
  constexpr static size_t   WHEEL_MASK  = WHEEL_SIZE - 1U;

  constexpr static uint64_t   STOPPED_FLAG       = 0U;
  constexpr static uint64_t   RUNNING_FLAG       = static_cast<uint64_t>(1U) << 63U;
  constexpr static uint64_t   EXPIRED_FLAG       = static_cast<uint64_t>(1U) << 62U;
  constexpr static tic_diff_t MAX_TIMER_DURATION = 0x3FFFFFFFU;

  static bool       decode_is_running(uint64_t value) { return (value & RUNNING_FLAG) != 0; }
  static bool       decode_is_expired(uint64_t value) { return (value & EXPIRED_FLAG) != 0; }
  static tic_diff_t decode_duration(uint64_t value) { return (value >> 32U) & MAX_TIMER_DURATION; }
  static tic_t      decode_timeout(uint64_t value) { return static_cast<uint32_t>(value & 0xFFFFFFFFU); }
  static uint64_t   encode_state(uint64_t mode_flag, uint32_t duration, uint32_t timeout)
  {
    return mode_flag + (static_cast<uint64_t>(duration) << 32U) + timeout;
  }

  struct timer_impl : public intrusive_double_linked_list_element<>, public intrusive_forward_list_element<> {
    // const
    const uint32_t id;
    timer_handler& parent;
    // writes protected by backend lock
    bool                                  allocated = false;
    std::atomic<uint64_t>                 state{0}; ///< read can be without lock, thus writes must be atomic
    srsran::move_callback<void(uint32_t)> callback;

    explicit timer_impl(timer_handler& parent_, uint32_t id_) : parent(parent_), id(id_) {}
    timer_impl(const timer_impl&) = delete;
    timer_impl(timer_impl&&)      = delete;
    timer_impl& operator=(const timer_impl&) = delete;
    timer_impl& operator=(timer_impl&&) = delete;

    // unprotected
    bool       is_running_() const { return decode_is_running(state.load(std::memory_order_relaxed)); }
    bool       is_expired_() const { return decode_is_expired(state.load(std::memory_order_relaxed)); }
    uint32_t   duration_() const { return decode_duration(state.load(std::memory_order_relaxed)); }
    bool       is_set_() const { return duration_() > 0; }
    tic_diff_t time_elapsed_() const
    {
      uint64_t state_snapshot = state.load(std::memory_order_relaxed);
      bool     running = decode_is_running(state_snapshot), expired = decode_is_expired(state_snapshot);
      uint32_t duration = decode_duration(state_snapshot), timeout = decode_timeout(state_snapshot);
      return running ? duration - (timeout - parent.cur_time) : (expired ? duration : 0);
    }

    void set(uint32_t duration_)
    {
      srsran_assert(duration_ <= MAX_TIMER_DURATION,
                    "Invalid timer duration=%" PRIu32 ">%" PRIu32,
                    duration_,
                    MAX_TIMER_DURATION);
      std::lock_guard<std::mutex> lock(parent.mutex);
      set_(duration_);
    }

    void set(uint32_t duration_, srsran::move_callback<void(uint32_t)> callback_)
    {
      srsran_assert(duration_ <= MAX_TIMER_DURATION,
                    "Invalid timer duration=%" PRIu32 ">%" PRIu32,
                    duration_,
                    MAX_TIMER_DURATION);
      std::lock_guard<std::mutex> lock(parent.mutex);
      set_(duration_);
      callback = std::move(callback_);
    }

    void run()
    {
      std::lock_guard<std::mutex> lock(parent.mutex);
      parent.start_run_(*this);
    }

    void stop()
    {
      std::lock_guard<std::mutex> lock(parent.mutex);
      // does not call callback
      parent.stop_timer_(*this, false);
    }

    void deallocate()
    {
      std::lock_guard<std::mutex> lock(parent.mutex);
      parent.dealloc_timer_(*this);
    }

  private:
    void set_(uint32_t duration_)
    {
      duration_ = std::max(duration_, 1U); // the next step will be one place ahead of current one
      // called in locked context
      uint64_t old_state = state.load(std::memory_order_relaxed);
      if (decode_is_running(old_state)) {
        // if already running, just extends timer lifetime
        parent.start_run_(*this, duration_);
      } else {
        state.store(encode_state(STOPPED_FLAG, duration_, 0), std::memory_order_relaxed);
      }
    }
  };

public:
  class unique_timer
  {
  public:
    unique_timer() = default;
    explicit unique_timer(timer_impl* handle_) : handle(handle_) {}
    unique_timer(const unique_timer&) = delete;
    unique_timer(unique_timer&& other) noexcept : handle(other.handle) { other.handle = nullptr; }
    ~unique_timer() { release(); }
    unique_timer& operator=(const unique_timer&) = delete;
    unique_timer& operator                       =(unique_timer&& other) noexcept
    {
      if (this != &other) {
        handle       = other.handle;
        other.handle = nullptr;
      }
      return *this;
    }

    bool is_valid() const { return handle != nullptr; }

    void set(uint32_t duration_, move_callback<void(uint32_t)> callback_)
    {
      srsran_assert(is_valid(), "Trying to setup empty timer handle");
      handle->set(duration_, std::move(callback_));
    }
    void set(uint32_t duration_)
    {
      srsran_assert(is_valid(), "Trying to setup empty timer handle");
      handle->set(duration_);
    }

    uint32_t   id() const { return is_valid() ? handle->id : INVALID_ID; }
    bool       is_set() const { return is_valid() and handle->is_set_(); }
    bool       is_running() const { return is_valid() and handle->is_running_(); }
    bool       is_expired() const { return is_valid() and handle->is_expired_(); }
    tic_diff_t time_elapsed() const { return is_valid() ? handle->time_elapsed_() : 0; }
    tic_diff_t duration() const { return is_valid() ? handle->duration_() : 0; }

    void run()
    {
      srsran_assert(is_valid(), "Starting invalid timer");
      handle->run();
    }

    void stop()
    {
      if (is_valid()) {
        handle->stop();
      }
    }

    void release()
    {
      if (is_valid()) {
        handle->deallocate();
        handle = nullptr;
      }
    }

  private:
    timer_impl* handle = nullptr;
  };

  explicit timer_handler(uint32_t capacity = 64)
  {
    time_wheel.resize(WHEEL_SIZE);
    // Pre-reserve timers
    while (timer_list.size() < capacity) {
      timer_list.emplace_back(*this, timer_list.size());
    }
    // push to free list in reverse order to keep ascending ids
    for (auto it = timer_list.rbegin(); it != timer_list.rend(); ++it) {
      free_list.push_front(&(*it));
    }
    nof_free_timers = timer_list.size();
  }

  void step_all()
  {
    std::unique_lock<std::mutex> lock(mutex);
    uint32_t                     cur_time_local = cur_time.load(std::memory_order_relaxed) + 1;
    auto&                        wheel_list     = time_wheel[cur_time_local & WHEEL_MASK];

    for (auto it = wheel_list.begin(); it != wheel_list.end();) {
      timer_impl& timer = timer_list[it->id];
      ++it;
      if (decode_timeout(timer.state.load(std::memory_order_relaxed)) == cur_time_local) {
        // stop timer (callback has to see the timer has already expired)
        stop_timer_(timer, true);

        // Call callback if configured
        if (not timer.callback.is_empty()) {
          // unlock mutex. It can happen that the callback tries to run a timer too
          lock.unlock();

          timer.callback(timer.id);

          // Lock again to keep protecting the wheel
          lock.lock();
        }
      }
    }

    cur_time.fetch_add(1, std::memory_order_relaxed);
  }

  void stop_all()
  {
    std::lock_guard<std::mutex> lock(mutex);
    // does not call callback
    for (timer_impl& timer : timer_list) {
      stop_timer_(timer, false);
    }
  }

  unique_timer get_unique_timer() { return unique_timer(&alloc_timer()); }

  uint32_t nof_timers() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return timer_list.size() - nof_free_timers;
  }

  uint32_t nof_running_timers() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return nof_timers_running_;
  }

  constexpr static uint32_t max_timer_duration() { return MAX_TIMER_DURATION; }

  template <typename F>
  void defer_callback(uint32_t duration, const F& func)
  {
    timer_impl&                           timer = alloc_timer();
    srsran::move_callback<void(uint32_t)> c     = [func, &timer](uint32_t tid) {
      func();
      // auto-deletes timer
      timer.deallocate();
    };
    timer.set(duration, std::move(c));
    timer.run();
  }

  // useful for testing
  static size_t get_wheel_size() { return WHEEL_SIZE; }

private:
  timer_impl& alloc_timer()
  {
    std::lock_guard<std::mutex> lock(mutex);
    timer_impl*                 t;
    if (not free_list.empty()) {
      t = &free_list.front();
      srsran_assert(not t->allocated, "Invalid timer id=%d state", t->id);
      free_list.pop_front();
      nof_free_timers--;
    } else {
      // Need to increase deque
      timer_list.emplace_back(*this, timer_list.size());
      t = &timer_list.back();
    }
    t->allocated = true;
    return *t;
  }

  void dealloc_timer_(timer_impl& timer)
  {
    if (not timer.allocated) {
      // already deallocated
      return;
    }
    stop_timer_(timer, false);
    timer.allocated = false;
    timer.state.store(encode_state(STOPPED_FLAG, 0, 0), std::memory_order_relaxed);
    timer.callback = srsran::move_callback<void(uint32_t)>();
    free_list.push_front(&timer);
    nof_free_timers++;
    // leave id unchanged.
  }

  void start_run_(timer_impl& timer, uint32_t duration_ = 0)
  {
    uint64_t timer_old_state = timer.state.load(std::memory_order_relaxed);
    duration_                = duration_ == 0 ? decode_duration(timer_old_state) : duration_;
    uint32_t new_timeout     = cur_time.load(std::memory_order_relaxed) + duration_;
    size_t   new_wheel_pos   = new_timeout & WHEEL_MASK;

    uint32_t old_timeout = decode_timeout(timer_old_state);
    bool     was_running = decode_is_running(timer_old_state);
    if (was_running and (old_timeout & WHEEL_MASK) == new_wheel_pos) {
      // If no change in timer wheel position. Just update absolute timeout
      timer.state.store(encode_state(RUNNING_FLAG, duration_, new_timeout), std::memory_order_relaxed);
      return;
    }

    // Stop timer if it was running, removing it from wheel in the process
    if (was_running) {
      time_wheel[old_timeout & WHEEL_MASK].pop(&timer);
      nof_timers_running_--;
    }

    // Insert timer in wheel
    time_wheel[new_wheel_pos].push_front(&timer);
    timer.state.store(encode_state(RUNNING_FLAG, duration_, new_timeout), std::memory_order_relaxed);
    nof_timers_running_++;
  }

  /// called when user manually stops timer (as an alternative to expiry)
  void stop_timer_(timer_impl& timer, bool expiry)
  {
    uint64_t timer_old_state = timer.state.load(std::memory_order_relaxed);
    if (not decode_is_running(timer_old_state)) {
      return;
    }

    // If already running, need to disconnect it from previous wheel
    uint32_t old_timeout = decode_timeout(timer_old_state);
    time_wheel[old_timeout & WHEEL_MASK].pop(&timer);
    uint64_t new_state =
        encode_state(expiry ? EXPIRED_FLAG : STOPPED_FLAG, decode_duration(timer_old_state), old_timeout);
    timer.state.store(new_state, std::memory_order_relaxed);
    nof_timers_running_--;
  }

  std::atomic<tic_t> cur_time{0};
  size_t             nof_timers_running_ = 0, nof_free_timers = 0;
  // using a deque to maintain reference validity on emplace_back. Also, this deque will only grow.
  std::deque<timer_impl>                                         timer_list;
  srsran::intrusive_forward_list<timer_impl>                     free_list;
  std::vector<srsran::intrusive_double_linked_list<timer_impl> > time_wheel;
  mutable std::mutex                                             mutex; // Protect priority queue
};

using unique_timer = timer_handler::unique_timer;

} // namespace srsran

#endif // SRSRAN_TIMERS_H
