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
class observer;
template <typename T>
struct is_observer : public std::false_type {};
template <typename... Args2>
struct is_observer<observer<Args2...> > : public std::true_type {};

//! Type-erasure of Observer
template <typename... Args>
class observer
{
public:
  using callback_t = std::function<void(Args...)>;
  template <typename Callable>
  using enable_if_callback_t =
      typename std::enable_if<std::is_convertible<Callable, callback_t>::value and
                              not is_observer<typename std::decay<Callable>::type>::value>::type;

  observer() = default;

  //! Subscribe Observer that is a callback
  template <typename Callable, typename Check = enable_if_callback_t<Callable> >
  observer(Callable&& callable) : callback(std::forward<Callable>(callable))
  {}

  template <typename Observer,
            typename TCheck =
                typename std::enable_if<not std::is_convertible<Observer, callback_t>::value, observer_id>::type>
  observer(Observer& observer) : callback([&observer](Args... args) { observer.trigger(std::forward<Args>(args)...); })
  {}

  template <typename Observer>
  observer(Observer& observer, void (Observer::*trigger_method)(Args...)) :
    callback([&observer, trigger_method](Args... args) { (observer.*trigger_method)(std::forward<Args>(args)...); })
  {}

  void operator()(Args... args)
  {
    if (callback) {
      callback(std::forward<Args>(args)...);
    }
  }

  explicit operator bool() const { return static_cast<bool>(callback); }

  void reset() { callback = nullptr; }

private:
  callback_t callback;
};

template <typename... Args>
class base_observable
{
public:
  using this_observer_t = observer<Args...>;

  //! Subscribe Observer that is a callback
  template <typename... Args2>
  observer_id subscribe(Args2&&... args)
  {
    size_t id = 0;
    for (auto& slot : observers) {
      if (not static_cast<bool>(slot)) {
        // empty slot found
        slot = this_observer_t{std::forward<Args2>(args)...};
        return id;
      }
      id++;
    }
    // append to end of list
    observers.emplace_back(std::forward<Args2>(args)...);
    return observers.size() - 1;
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

  void unsubscribe_all() { observers.clear(); }

  //! Signal result to observers
  void dispatch(Args... args)
  {
    for (auto& obs_callback : observers) {
      obs_callback(std::forward<Args>(args)...);
    }
  }

protected:
  using observer_list_t = std::deque<this_observer_t>;

  ~base_observable() = default;

  observer_list_t observers;
};

template <typename... Args>
class observable : public base_observable<Args...>
{};

template <typename Event>
using event_observer = observer<const Event&>;

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
