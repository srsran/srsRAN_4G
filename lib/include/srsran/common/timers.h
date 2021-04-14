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
 *  File:         timers.h
 *  Description:  Manually incremented timers. Call a callback function upon
 *                expiry.
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_TIMERS_H
#define SRSRAN_TIMERS_H

#include "srsran/adt/circular_array.h"
#include "srsran/adt/intrusive_list.h"
#include "srsran/adt/move_callback.h"
#include "srsran/phy/utils/debug.h"
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

class timer_handler
{
  using tic_diff_t                             = uint32_t;
  using tic_t                                  = uint32_t;
  constexpr static size_t   WHEEL_SIZE         = 1024;
  constexpr static tic_t    invalid_tic        = std::numeric_limits<tic_t>::max();
  constexpr static uint32_t MAX_TIMER_DURATION = std::numeric_limits<tic_diff_t>::max() / 4;

  struct timer_impl : public intrusive_double_linked_list_element<> {
    timer_handler& parent;
    const size_t   id;
    tic_diff_t     duration                                          = 0;
    tic_t          timeout                                           = 0;
    enum state_t : int8_t { empty, stopped, running, expired } state = empty;
    srsran::move_callback<void(uint32_t)> callback;

    explicit timer_impl(timer_handler& parent_, size_t id_) : parent(parent_), id(id_) {}

    bool       is_empty() const { return state == empty; }
    bool       is_running() const { return state == running; }
    bool       is_expired() const { return state == expired; }
    tic_diff_t time_left() const { return is_running() ? timeout - parent.cur_time : (is_expired() ? 0 : duration); }
    uint32_t   time_elapsed() const { return duration - time_left(); }

    bool set(uint32_t duration_)
    {
      if (duration_ > MAX_TIMER_DURATION) {
        ERROR("Error: timer durations above %u are not supported", MAX_TIMER_DURATION);
        return false;
      }
      duration = std::max(duration_, 1u);
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
      parent.stop_timer_(*this);
    }

    void clear() { parent.dealloc_timer(*this); }
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

    bool is_set() const { return is_valid() and handle->duration > 0; }

    bool is_running() const { return is_valid() and handle->is_running(); }

    bool is_expired() const { return is_valid() and handle->is_expired(); }

    tic_diff_t time_elapsed() const { return is_valid() ? handle->time_elapsed() : -1; }

    uint32_t id() const { return is_valid() ? handle->id : -1; }

    tic_diff_t duration() const { return is_valid() ? handle->duration : -1; }

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
        handle->clear();
        handle = nullptr;
      }
    }

  private:
    timer_impl* handle = nullptr;
  };

  explicit timer_handler(uint32_t capacity = 64)
  {
    // Pre-reserve timers
    while (timer_list.size() < capacity) {
      timer_list.emplace_back(*this, timer_list.size());
    }
  }

  void step_all()
  {
    std::unique_lock<std::mutex> lock(mutex);
    cur_time++;
    if (cur_time == WHEEL_SIZE) {
      // Promote timers from 2nd wheel to first if needed
      for (size_t i = 0; i < WHEEL_SIZE; ++i) {
        for (auto it = second_wheel[i].begin(); it != second_wheel[i].end();) {
          auto& timer = timer_list[it->id];
          timer.timeout -= WHEEL_SIZE;
          ++it;
          if (timer.timeout < WHEEL_SIZE) {
            second_wheel[i].pop(&timer);
            first_wheel[i].push_front(&timer);
          }
        }
      }
      cur_time = 0;
    }

    auto& wheel_list = first_wheel[cur_time % WHEEL_SIZE];
    while (not wheel_list.empty()) {
      // Remove timer from wheel
      timer_impl& timer = wheel_list.front();
      wheel_list.pop_front();

      // update timer state
      timer.state = timer_impl::expired;
      nof_timers_running_--;

      // Call callback
      if (not timer.callback.is_empty()) {
        // unlock mutex, it could be that the callback tries to run a timer too
        lock.unlock();

        timer.callback(timer.id);

        // Lock again to keep protecting the wheel
        lock.lock();
      }
    }
  }

  void stop_all()
  {
    std::lock_guard<std::mutex> lock(mutex);
    // does not call callback
    for (timer_impl& timer : timer_list) {
      stop_timer_(timer);
    }
  }

  unique_timer get_unique_timer() { return unique_timer(&alloc_timer()); }

  uint32_t nof_timers() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return nof_timers_;
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
      timer.clear();
    };
    timer.set(duration, std::move(c));
    timer.run();
  }

private:
  timer_impl& alloc_timer()
  {
    std::lock_guard<std::mutex> lock(mutex);
    nof_timers_++;
    if (nof_timers_ > timer_list.size()) {
      // Need to increase deque
      timer_list.emplace_back(*this, timer_list.size());
      timer_impl& ret = timer_list.back();
      ret.state       = timer_impl::stopped;
      return ret;
    }

    for (auto& timer : timer_list) {
      if (timer.is_empty()) {
        timer.state = timer_impl::stopped;
        return timer;
      }
    }
    srsran_terminate("Failed to allocate timer");
  }

  void dealloc_timer(timer_impl& timer)
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (timer.is_empty()) {
      // already deallocated
      return;
    }
    stop_timer_(timer);
    timer.state    = timer_impl::empty;
    timer.duration = 0;
    timer.timeout  = 0;
    timer.callback = srsran::move_callback<void(uint32_t)>();
    nof_timers_--;
    // leave id unchanged.
  }

  void start_run_(timer_impl& timer)
  {
    uint32_t timeout = cur_time + timer.duration;
    if (timer.is_running() and timer.timeout == timeout) {
      // If no change in timeout, no need to change wheel position
      return;
    }

    // Stop timer if it was running, removing it from wheel in the process
    stop_timer_(timer);

    // Insert timer in wheel
    if (timeout < WHEEL_SIZE) {
      first_wheel[timeout].push_front(&timer);
    } else {
      second_wheel[timeout % WHEEL_SIZE].push_front(&timer);
    }
    timer.timeout = timeout;
    timer.state   = timer_impl::running;
    nof_timers_running_++;
  }

  /// called when user manually stops timer (as an alternative to expiry)
  void stop_timer_(timer_impl& timer)
  {
    if (not timer.is_running()) {
      return;
    }

    // If already running, need to disconnect it from previous wheel
    if (timer.timeout < WHEEL_SIZE) {
      first_wheel[timer.timeout].pop(&timer);
    } else {
      second_wheel[timer.timeout % WHEEL_SIZE].pop(&timer);
    }

    timer.state = timer_impl::stopped;
    nof_timers_running_--;
  }

  uint32_t                                                                             cur_time            = 0;
  size_t                                                                               nof_timers_         = 0;
  size_t                                                                               nof_timers_running_ = 0;
  std::deque<timer_impl>                                                               timer_list;
  srsran::circular_array<srsran::intrusive_double_linked_list<timer_impl>, WHEEL_SIZE> first_wheel;
  srsran::circular_array<srsran::intrusive_double_linked_list<timer_impl>, WHEEL_SIZE> second_wheel;
  mutable std::mutex                                                                   mutex; // Protect priority queue
};

using unique_timer = timer_handler::unique_timer;

} // namespace srsran

#endif // SRSRAN_TIMERS_H
