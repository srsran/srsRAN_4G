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

#ifndef SRSLTE_OBSERVER_H
#define SRSLTE_OBSERVER_H

#include <deque>
#include <functional>
#include <limits>
#include <vector>

namespace srslte {

using observer_id                = std::size_t;
const size_t invalid_observer_id = std::numeric_limits<observer_id>::max();

template <typename... Args>
class base_observable
{
public:
  using callback_t = std::function<void(Args...)>;

  //! Subscribe Observer that is a callback
  template <typename Callable>
  typename std::enable_if<std::is_convertible<Callable, callback_t>::value, observer_id>::type
  subscribe(Callable&& callable)
  {
    return subscribe_common(callable);
  }

  //! Subscribe Observer type with method Observer::trigger(Args...)
  template <typename Observer>
  typename std::enable_if<not std::is_convertible<Observer, callback_t>::value, observer_id>::type
  subscribe(Observer& observer)
  {
    return subscribe_common([&observer](Args... args) { observer.trigger(std::forward<Args>(args)...); });
  }

  //! Subscribe Observer type with custom trigger method
  template <typename Observer>
  observer_id subscribe(Observer& observer, void (Observer::*trigger_method)(Args...))
  {
    return subscribe_common(
        [&observer, trigger_method](Args... args) { (observer.*trigger_method)(std::forward<Args>(args)...); });
  }

  //! Unsubscribe Observer
  bool unsubscribe(observer_id id)
  {
    if (id < observers.size() and static_cast<bool>(observers[id])) {
      observers[id] = nullptr;
      return true;
    }
    return false;
  }

  size_t nof_observers() const
  {
    size_t count = 0;
    for (auto& slot : observers) {
      count += static_cast<bool>(slot) ? 1 : 0;
    }
    return count;
  }

  //! Signal result to observers
  void dispatch(Args... args)
  {
    for (auto& obs_callback : observers) {
      if (obs_callback) {
        obs_callback(std::forward<Args>(args)...);
      }
    }
  }

protected:
  using observer_list_t = std::deque<callback_t>;

  ~base_observable() = default;

  template <typename Callable>
  observer_id subscribe_common(Callable&& callable)
  {
    size_t id = 0;
    for (auto& slot : observers) {
      if (not static_cast<bool>(slot)) {
        // empty slot found
        slot = std::forward<Callable>(callable);
        return id;
      }
      id++;
    }
    // append to end of list
    observers.emplace_back(std::forward<Callable>(callable));
    return observers.size() - 1;
  }

  observer_list_t observers;
};

template <typename... Args>
class observable : public base_observable<Args...>
{};

//! Special case of observable for event types
template <typename Event>
class event_dispatcher : public base_observable<const Event&>
{};

//! Event Subject that enqueues events and only signals observers when ::process() is called
template <typename Event>
class event_queue : public base_observable<const Event&>
{
  using base_t = base_observable<const Event&>;

public:
  template <typename... Args>
  void enqueue(Args&&... args)
  {
    pending_events.emplace_back(std::forward<Args>(args)...);
  }

  void process()
  {
    for (auto& ev : pending_events) {
      base_t::dispatch(ev);
    }
    pending_events.clear();
  }

private:
  // forbid direct dispatches
  using base_t::dispatch;

  std::vector<Event> pending_events;
};

//! RAII class to automatically unsubscribe an observer from an Event
template <typename Event>
class unique_observer_id
{
  using subject_t = base_observable<const Event&>;

public:
  unique_observer_id(subject_t& parent_, observer_id id_) : parent(&parent_), id(id_) {}
  template <typename T>
  unique_observer_id(subject_t& parent_, T&& callable) : parent(&parent_)
  {
    id = parent->subscribe(std::forward<T>(callable));
  }
  template <typename Observer>
  unique_observer_id(subject_t& parent_, Observer& observer, void (Observer::*trigger_method)(const Event&)) :
    parent(&parent_)
  {
    id = parent->subscribe(observer, trigger_method);
  }
  unique_observer_id(unique_observer_id&& other) noexcept : parent(other.parent), id(other.id)
  {
    other.parent = nullptr;
  }
  unique_observer_id(const unique_observer_id& other) = delete;

  unique_observer_id& operator=(unique_observer_id&& other) noexcept
  {
    parent   = other.parent;
    id       = other.id;
    other.id = invalid_observer_id;
    return *this;
  }
  unique_observer_id& operator=(const unique_observer_id& other) = delete;
  ~unique_observer_id()
  {
    if (id != invalid_observer_id) {
      parent->unsubscribe(id);
    }
  }

  observer_id get_id() const { return id; }
  bool        is_valid() const { return id != invalid_observer_id; }
  observer_id release()
  {
    observer_id ret = id;
    id              = invalid_observer_id;
    return ret;
  }

private:
  subject_t*  parent;
  observer_id id;
};

} // namespace srslte

#endif // SRSLTE_OBSERVER_H
