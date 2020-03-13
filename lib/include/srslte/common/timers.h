/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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

#ifndef SRSLTE_TIMERS_H
#define SRSLTE_TIMERS_H

#include <algorithm>
#include <functional>
#include <limits>
#include <mutex>
#include <queue>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <vector>

#include "srslte/srslte.h"

namespace srslte {

class timer_callback
{
public:
  virtual void timer_expired(uint32_t timer_id) = 0;
};

class timer_handler
{
  constexpr static uint32_t MAX_TIMER_DURATION = std::numeric_limits<uint32_t>::max() / 4;
  constexpr static uint32_t MAX_TIMER_VALUE    = std::numeric_limits<uint32_t>::max() / 2;

  struct timer_impl {
    timer_handler*                parent;
    uint32_t                      duration = 0, timeout = 0;
    bool                          running = false;
    bool                          active  = false;
    std::function<void(uint32_t)> callback;

    explicit timer_impl(timer_handler* parent_) : parent(parent_) {}

    uint32_t id() const { return std::distance((const timer_handler::timer_impl*)&parent->timer_list[0], this); }

    bool is_running() const { return active and running and timeout > 0; }

    bool is_expired() const { return active and not running and timeout > 0 and timeout <= parent->cur_time; }

    uint32_t time_elapsed() const { return std::min(duration, parent->cur_time - (timeout - duration)); }

    bool set(uint32_t duration_)
    {
      if (duration_ > MAX_TIMER_DURATION) {
        ERROR("Error: timer durations above %u are not supported\n", MAX_TIMER_DURATION);
        return false;
      }
      if (not active) {
        ERROR("Error: setting inactive timer id=%d\n", id());
        return false;
      }
      duration = duration_;
      if (is_running()) {
        // if already running, just extends timer lifetime
        run();
      }
      return true;
    }

    bool set(uint32_t duration_, std::function<void(int)> callback_)
    {
      if (set(duration_)) {
        callback = std::move(callback_);
        return true;
      }
      return false;
    }

    void run()
    {
      std::unique_lock<std::mutex> lock(parent->mutex);
      if (not active) {
        ERROR("Error: calling run() for inactive timer id=%d\n", id());
        return;
      }
      timeout = parent->cur_time + duration;
      parent->running_timers.emplace(id(), timeout);
      running = true;
    }

    void stop()
    {
      running = false; // invalidates trigger
      if (not is_expired()) {
        timeout = 0; // if it has already expired, then do not alter is_expired() state
      }
    }

    void clear()
    {
      stop();
      duration = 0;
      active   = false;
      callback = std::function<void(uint32_t)>();
      // leave run_id unchanged. Since the timeout was changed, we shall not get spurious triggering
    }

    void trigger()
    {
      if (is_running()) {
        running = false;
        if (callback) {
          callback(id());
        }
      }
    }
  };

public:
  class unique_timer
  {
  public:
    unique_timer() : timer_id(std::numeric_limits<decltype(timer_id)>::max()) {}
    explicit unique_timer(timer_handler* parent_, uint32_t timer_id_) : parent(parent_), timer_id(timer_id_) {}

    unique_timer(const unique_timer&) = delete;

    unique_timer(unique_timer&& other) noexcept : parent(other.parent), timer_id(other.timer_id)
    {
      other.parent = nullptr;
    }

    ~unique_timer()
    {
      if (parent != nullptr) {
        // does not call callback
        impl()->clear();
      }
    }

    unique_timer& operator=(const unique_timer&) = delete;

    unique_timer& operator=(unique_timer&& other) noexcept
    {
      if (this != &other) {
        timer_id     = other.timer_id;
        parent       = other.parent;
        other.parent = nullptr;
      }
      return *this;
    }

    bool is_valid() const { return parent != nullptr; }

    void set(uint32_t duration_, const std::function<void(int)>& callback_) { impl()->set(duration_, callback_); }

    void set(uint32_t duration_) { impl()->set(duration_); }

    bool is_set() const { return (impl()->duration != 0); }

    bool is_running() const { return impl()->is_running(); }

    bool is_expired() const { return impl()->is_expired(); }

    uint32_t time_elapsed() const { return impl()->time_elapsed(); }

    void run() { impl()->run(); }

    void stop() { impl()->stop(); }

    void clear() { impl()->clear(); }

    void release()
    {
      impl()->clear();
      parent = nullptr;
    }

    uint32_t id() const { return timer_id; }

    uint32_t duration() const { return impl()->duration; }

  private:
    timer_impl* impl() { return &parent->timer_list[timer_id]; }

    const timer_impl* impl() const { return &parent->timer_list[timer_id]; }

    timer_handler* parent = nullptr;
    uint32_t       timer_id;
  };

  explicit timer_handler(uint32_t capacity = 64)
  {
    timer_list.reserve(capacity);
    // reserve a priority queue using a vector
    std::vector<timer_run> v;
    v.reserve(capacity);
    std::priority_queue<timer_run> q(std::less<timer_run>(), std::move(v));
    running_timers = std::move(q);
  }

  void step_all()
  {
    std::unique_lock<std::mutex> lock(mutex);
    cur_time++;
    while (not running_timers.empty()) {
      uint32_t    next_timeout = running_timers.top().timeout;
      timer_impl* ptr          = &timer_list[running_timers.top().timer_id];
      if (not ptr->is_running() or next_timeout != ptr->timeout) {
        // remove timers that were explicitly stopped, or re-run, to avoid unnecessary priority_queue growth
        running_timers.pop();
        continue;
      }
      if (cur_time < next_timeout) {
        break;
      }
      // if the timer_run and timer_impl timeouts do not match, it means that timer_impl::timeout was overwritten.
      // in such case, do not trigger
      uint32_t timeout = running_timers.top().timeout;
      running_timers.pop();

      if (ptr->timeout == timeout) {
        // unlock mutex, it could be that the callback tries to run a timer too
        lock.unlock();

        // Call callback
        ptr->trigger();

        // Lock again to keep protecting the queue
        lock.lock();
      }
    }
  }

  void stop_all()
  {
    // does not call callback
    while (not running_timers.empty()) {
      running_timers.pop();
    }
    for (auto& i : timer_list) {
      i.running = false;
    }
  }

  unique_timer get_unique_timer() { return unique_timer(this, alloc_timer()); }

  uint32_t get_cur_time() const { return cur_time; }

  uint32_t nof_timers() const
  {
    return std::count_if(timer_list.begin(), timer_list.end(), [](const timer_impl& t) { return t.active; });
  }

  uint32_t nof_running_timers() const
  {
    return std::count_if(timer_list.begin(), timer_list.end(), [](const timer_impl& t) { return t.is_running(); });
  }

  template <typename F>
  void defer_callback(uint32_t duration, const F& func)
  {
    uint32_t                      id = alloc_timer();
    std::function<void(uint32_t)> c  = [func, this, id](uint32_t tid) {
      func();
      // auto-deletes timer
      timer_list[id].clear();
    };
    timer_list[id].set(duration, std::move(c));
    timer_list[id].run();
  }

private:
  struct timer_run {
    uint32_t timer_id;
    uint32_t timeout;

    timer_run(uint32_t timer_id_, uint32_t timeout_) : timer_id(timer_id_), timeout(timeout_) {}

    bool operator<(const timer_run& other) const
    {
      // returns true, if other.timeout is lower than timeout, accounting for wrap around
      if (timeout > other.timeout) {
        return (timeout - other.timeout) < MAX_TIMER_VALUE / 2;
      }
      return (other.timeout - timeout) > MAX_TIMER_VALUE / 2;
    }
  };

  uint32_t alloc_timer()
  {
    uint32_t i = 0;
    for (; i < timer_list.size(); ++i) {
      if (not timer_list[i].active) {
        break;
      }
    }
    if (i == timer_list.size()) {
      timer_list.emplace_back(this);
    }
    timer_list[i].active = true;
    return i;
  }

  std::vector<timer_impl>        timer_list;
  std::priority_queue<timer_run> running_timers;
  uint32_t                       cur_time = 0;
  std::mutex                     mutex; // Protect priority queue
};

} // namespace srslte

#endif // SRSLTE_TIMERS_H
