/**
 * Copyright 2013-2021 Software Radio Systems Limited
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
#include <limits>
#include <mutex>

namespace srsran {

class timer_callback
{
public:
  virtual void timer_expired(uint32_t timer_id) = 0;
};

/**
 * Class that manages stack timers. It allows creation of unique_timers, with different ids. Each unique_timer duration,
 * and callback can be set via the set(...) method. A timer can be started/stopped via run()/stop() methods.
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
  using tic_diff_t                              = uint32_t;
  using tic_t                                   = uint32_t;
  constexpr static uint32_t   INVALID_ID        = std::numeric_limits<uint32_t>::max();
  constexpr static tic_diff_t INVALID_TIME_DIFF = std::numeric_limits<tic_diff_t>::max();
  constexpr static size_t     WHEEL_SHIFT       = 16U;
  constexpr static size_t     WHEEL_SIZE        = 1U << WHEEL_SHIFT;
  constexpr static size_t     WHEEL_MASK        = WHEEL_SIZE - 1U;

  struct timer_impl : public intrusive_double_linked_list_element<>, public intrusive_forward_list_element<> {
    timer_handler& parent;
    const uint32_t id;
    tic_diff_t     duration                                          = INVALID_TIME_DIFF;
    tic_t          timeout                                           = 0;
    enum state_t : int8_t { empty, stopped, running, expired } state = empty;
    srsran::move_callback<void(uint32_t)> callback;

    explicit timer_impl(timer_handler& parent_, uint32_t id_) : parent(parent_), id(id_) {}
    timer_impl(const timer_impl&) = delete;
    timer_impl(timer_impl&&)      = delete;
    timer_impl& operator=(const timer_impl&) = delete;
    timer_impl& operator=(timer_impl&&) = delete;

    bool       is_empty() const { return state == empty; }
    bool       is_running() const { return state == running; }
    bool       is_expired() const { return state == expired; }
    tic_diff_t time_left() const { return is_running() ? timeout - parent.cur_time : (is_expired() ? 0 : duration); }
    uint32_t   time_elapsed() const { return duration - time_left(); }

    bool set(uint32_t duration_)
    {
      duration = std::max(duration_, 1U); // the next step will be one place ahead of current one
      if (is_running()) {
        // if already running, just extends timer lifetime
        run();
      } else {
        state   = stopped;
        timeout = 0;
      }
      return true;
    }

    bool set(uint32_t duration_, srsran::move_callback<void(uint32_t)> callback_)
    {
      if (set(duration_)) {
        callback = std::move(callback_);
        return true;
      }
      return false;
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

    void deallocate() { parent.dealloc_timer(*this); }
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

    bool is_set() const { return is_valid() and handle->duration != INVALID_TIME_DIFF; }

    bool is_running() const { return is_valid() and handle->is_running(); }

    bool is_expired() const { return is_valid() and handle->is_expired(); }

    tic_diff_t time_elapsed() const { return is_valid() ? handle->time_elapsed() : INVALID_TIME_DIFF; }

    uint32_t id() const { return is_valid() ? handle->id : INVALID_ID; }

    tic_diff_t duration() const { return is_valid() ? handle->duration : INVALID_TIME_DIFF; }

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
    cur_time++;
    auto& wheel_list = time_wheel[cur_time & WHEEL_MASK];

    for (auto it = wheel_list.begin(); it != wheel_list.end();) {
      timer_impl& timer = timer_list[it->id];
      ++it;
      if (timer.timeout == cur_time) {
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
      srsran_assert(t->is_empty(), "Invalid timer id=%d state", t->id);
      free_list.pop_front();
      nof_free_timers--;
    } else {
      // Need to increase deque
      timer_list.emplace_back(*this, timer_list.size());
      t = &timer_list.back();
    }
    t->state = timer_impl::stopped;
    return *t;
  }

  void dealloc_timer(timer_impl& timer)
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (timer.is_empty()) {
      // already deallocated
      return;
    }
    stop_timer_(timer, false);
    timer.state    = timer_impl::empty;
    timer.duration = INVALID_TIME_DIFF;
    timer.timeout  = 0;
    timer.callback = srsran::move_callback<void(uint32_t)>();
    free_list.push_front(&timer);
    nof_free_timers++;
    // leave id unchanged.
  }

  void start_run_(timer_impl& timer)
  {
    uint32_t timeout       = cur_time + timer.duration;
    size_t   new_wheel_pos = timeout & WHEEL_MASK;
    if (timer.is_running() and (timer.timeout & WHEEL_MASK) == new_wheel_pos) {
      // If no change in timer wheel position. Just update absolute timeout
      timer.timeout = timeout;
      return;
    }

    // Stop timer if it was running, removing it from wheel in the process
    stop_timer_(timer, false);

    // Insert timer in wheel
    time_wheel[new_wheel_pos].push_front(&timer);
    timer.timeout = timeout;
    timer.state   = timer_impl::running;
    nof_timers_running_++;
  }

  /// called when user manually stops timer (as an alternative to expiry)
  void stop_timer_(timer_impl& timer, bool expiry)
  {
    if (not timer.is_running()) {
      return;
    }

    // If already running, need to disconnect it from previous wheel
    time_wheel[timer.timeout & WHEEL_MASK].pop(&timer);

    timer.state = expiry ? timer_impl::expired : timer_impl::stopped;
    nof_timers_running_--;
  }

  tic_t  cur_time            = 0;
  size_t nof_timers_running_ = 0, nof_free_timers = 0;
  // using a deque to maintain reference validity on emplace_back. Also, this deque will only grow.
  std::deque<timer_impl>                                         timer_list;
  srsran::intrusive_forward_list<timer_impl>                     free_list;
  std::vector<srsran::intrusive_double_linked_list<timer_impl> > time_wheel;
  mutable std::mutex                                             mutex; // Protect priority queue
};

using unique_timer = timer_handler::unique_timer;

} // namespace srsran

#endif // SRSRAN_TIMERS_H
