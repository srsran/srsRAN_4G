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

#ifndef SRSRAN_CIRCULAR_BUFFER_H
#define SRSRAN_CIRCULAR_BUFFER_H

#include "srsran/adt/detail/type_storage.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/pool/pool_utils.h"
#include "srsran/support/srsran_assert.h"

#include <array>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

namespace srsran {

namespace detail {

template <typename T, size_t N>
size_t get_max_size(const std::array<T, N>& a)
{
  return a.max_size();
}

template <typename T>
size_t get_max_size(const std::vector<T>& a)
{
  return a.capacity();
}

/**
 * Base common class for definition of circular buffer data structures with the following features:
 * - no allocations while pushing/popping new elements. Just an internal index update
 * - it provides helper methods to add/remove objects
 * - it provides an iterator interface to iterate over added elements in the buffer
 * - not thread-safe
 * @tparam Container underlying container type used as buffer (e.g. std::array<T, N> or std::vector<T>)
 */
template <typename Container>
class base_circular_buffer
{
  using storage_t = typename Container::value_type;
  using T         = typename storage_t::value_type;

  template <typename DataType>
  class iterator_impl
  {
    using parent_type = typename std::conditional<std::is_same<DataType, T>::value,
                                                  base_circular_buffer<Container>,
                                                  const base_circular_buffer<Container> >::type;

  public:
    using value_type        = DataType;
    using reference         = DataType&;
    using pointer           = DataType*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iterator_impl(parent_type& parent_, size_t i) : parent(&parent_), idx(i) {}

    iterator_impl<DataType>& operator++()
    {
      idx = (idx + 1) % parent->max_size();
      return *this;
    }
    iterator_impl<DataType> operator++(int)
    {
      iterator_impl<DataType> tmp(*this);
      ++(*this);
      return tmp;
    }
    iterator_impl<DataType> operator+(difference_type n)
    {
      iterator_impl<DataType> tmp(*this);
      tmp += n;
      return tmp;
    }
    iterator_impl<DataType>& operator+=(difference_type n)
    {
      idx = (idx + n) % parent->max_size();
      return *this;
    }
    value_type*       operator->() { return &get(); }
    const value_type* operator->() const { return &get(); }
    value_type&       operator*() { return get(); }
    const value_type& operator*() const { return get(); }

    bool operator==(const iterator_impl<DataType>& it) const { return it.parent == parent and it.idx == idx; }
    bool operator!=(const iterator_impl<DataType>& it) const { return not(*this == it); }

  private:
    void assert_idx_within_bounds()
    {
      srsran_assert(idx + (idx >= parent->rpos ? 0 : parent->max_size()) < parent->rpos + parent->count,
                    "index=%zd is out-of-bounds [%zd, %zd)",
                    idx,
                    parent->rpos,
                    parent->count);
    }
    value_type& get()
    {
      assert_idx_within_bounds();
      return parent->buffer[idx].get();
    }
    const value_type& get() const
    {
      assert_idx_within_bounds();
      return parent->buffer[idx].get();
    }
    parent_type* parent;
    size_t       idx;
  };

public:
  using value_type      = T;
  using difference_type = typename Container::difference_type;
  using size_type       = std::size_t;

  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  base_circular_buffer() = default;
  ~base_circular_buffer() { clear(); }

  template <typename U>
  typename std::enable_if<std::is_constructible<T, U>::value>::type push(U&& t)
  {
    srsran_assert(not full(), "Circular buffer is full.");
    size_t wpos = (rpos + count) % max_size();
    buffer[wpos].emplace(std::forward<U>(t));
    count++;
  }

  bool try_push(T&& t)
  {
    if (full()) {
      return false;
    }
    push(std::move(t));
    return true;
  }

  bool try_push(const T& t)
  {
    if (full()) {
      return false;
    }
    push(t);
    return true;
  }
  void pop()
  {
    srsran_assert(not empty(), "Cannot call pop() in empty circular buffer");
    buffer[rpos].destroy();
    rpos = (rpos + 1) % max_size();
    count--;
  }
  T& top()
  {
    srsran_assert(not empty(), "Cannot call top() in empty circular buffer");
    return buffer[rpos].get();
  }
  const T& top() const
  {
    srsran_assert(not empty(), "Cannot call top() in empty circular buffer");
    return buffer[rpos].get();
  }
  void clear()
  {
    for (size_t i = 0; i < count; ++i) {
      buffer[(rpos + i) % max_size()].destroy();
    }
    count = 0;
  }

  bool   full() const { return count == max_size(); }
  bool   empty() const { return count == 0; }
  size_t size() const { return count; }
  size_t max_size() const { return detail::get_max_size(buffer); }

  T& operator[](size_t i)
  {
    srsran_assert(i < count, "Out-of-bounds access to circular buffer (%zd >= %zd)", i, count);
    return buffer[(rpos + i) % max_size()].get();
  }
  const T& operator[](size_t i) const
  {
    srsran_assert(i < count, "Out-of-bounds access to circular buffer (%zd >= %zd)", i, count);
    return buffer[(rpos + i) % max_size()].get();
  }

  iterator       begin() { return iterator(*this, rpos); }
  const_iterator begin() const { return const_iterator(*this, rpos); }
  iterator       end() { return iterator(*this, (rpos + count) % max_size()); }
  const_iterator end() const { return const_iterator(*this, (rpos + count) % max_size()); }

  template <typename F>
  bool apply_first(const F& func)
  {
    for (auto it = begin(); it != end(); it++) {
      if (func(*it)) {
        return true;
      }
    }
    return false;
  }

protected:
  base_circular_buffer(size_t rpos_, size_t count_) : rpos(rpos_), count(count_) {}
  template <typename... BufferArgs>
  base_circular_buffer(size_t rpos_, size_t count_, BufferArgs&&... args) :
    rpos(rpos_), count(count_), buffer(std::forward<BufferArgs>(args)...)
  {}

  Container buffer;
  size_t    rpos  = 0;
  size_t    count = 0;
};

/**
 * Base common class for definition of blocking queue data structures with the following features:
 * - it stores pushed/popped samples in an internal circular buffer
 * - provides blocking and non-blocking push/pop APIs
 * - thread-safe
 * @tparam CircBuffer underlying circular buffer data type (e.g. static_circular_buffer<T, N> or dyn_circular_buffer<T>)
 * @tparam PushingFunc function void(const T&) called while pushing an element to the queue
 * @tparam PoppingFunc function void(const T&) called while popping an element from the queue
 */
template <typename CircBuffer, typename PushingFunc, typename PoppingFunc>
class base_blocking_queue
{
  using T = typename CircBuffer::value_type;

public:
  template <typename... Args>
  base_blocking_queue(PushingFunc push_func_, PoppingFunc pop_func_, Args&&... args) :
    circ_buffer(std::forward<Args>(args)...), push_func(push_func_), pop_func(pop_func_)
  {}
  base_blocking_queue(const base_blocking_queue&) = delete;
  base_blocking_queue(base_blocking_queue&&)      = delete;
  base_blocking_queue& operator=(const base_blocking_queue&) = delete;
  base_blocking_queue& operator=(base_blocking_queue&&) = delete;

  void stop()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (active) {
      active = false;
      if (nof_waiting > 0) {
        // Stop pending pushing/popping threads
        do {
          lock.unlock();
          cvar_empty.notify_all();
          cvar_full.notify_all();
          std::this_thread::yield();
          lock.lock();
        } while (nof_waiting > 0);
      }

      // Empty queue
      circ_buffer.clear();
    }
  }

  bool                  try_push(const T& t) { return push_(t, false); }
  srsran::error_type<T> try_push(T&& t) { return push_(std::move(t), false); }
  bool                  push_blocking(const T& t) { return push_(t, true); }
  srsran::error_type<T> push_blocking(T&& t) { return push_(std::move(t), true); }
  bool                  try_pop(T& obj) { return pop_(obj, false); }
  T                     pop_blocking(bool* success = nullptr)
  {
    T    obj{};
    bool ret = pop_(obj, true);
    if (success != nullptr) {
      *success = ret;
    }
    return obj;
  }
  bool pop_wait_until(T& obj, const std::chrono::steady_clock::time_point& until) { return pop_(obj, true, &until); }
  void clear()
  {
    T obj;
    while (pop_(obj, false)) {
    }
  }

  size_t size() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return circ_buffer.size();
  }
  bool empty() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return circ_buffer.empty();
  }
  bool full() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return circ_buffer.full();
  }
  size_t max_size() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return circ_buffer.max_size();
  }
  bool is_stopped() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return not active;
  }
  template <typename F>
  bool try_call_on_front(F&& f)
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (not circ_buffer.empty()) {
      f(circ_buffer.top());
      return true;
    }
    return false;
  }

  template <typename F>
  bool apply_first(const F& func)
  {
    std::lock_guard<std::mutex> lock(mutex);
    return circ_buffer.apply_first(func);
  }

  PushingFunc push_func;
  PoppingFunc pop_func;

protected:
  bool                    active      = true;
  uint8_t                 nof_waiting = 0;
  mutable std::mutex      mutex;
  std::condition_variable cvar_empty, cvar_full;
  CircBuffer              circ_buffer;

  ~base_blocking_queue() { stop(); }

  bool push_(const T& t, bool block_mode)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (not active) {
      return false;
    }
    if (circ_buffer.full()) {
      if (not block_mode) {
        return false;
      }
      nof_waiting++;
      while (circ_buffer.full() and active) {
        cvar_full.wait(lock);
      }
      nof_waiting--;
      if (not active) {
        return false;
      }
    }
    push_func(t);
    circ_buffer.push(t);
    lock.unlock();
    cvar_empty.notify_one();
    return true;
  }
  srsran::error_type<T> push_(T&& t, bool block_mode)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (not active) {
      return std::move(t);
    }
    if (circ_buffer.full()) {
      if (not block_mode) {
        return std::move(t);
      }
      nof_waiting++;
      while (circ_buffer.full() and active) {
        cvar_full.wait(lock);
      }
      nof_waiting--;
      if (not active) {
        return std::move(t);
      }
    }
    push_func(t);
    circ_buffer.push(std::move(t));
    lock.unlock();
    cvar_empty.notify_one();
    return {};
  }

  bool pop_(T& obj, bool block, const std::chrono::steady_clock::time_point* until = nullptr)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (not active) {
      return false;
    }
    if (circ_buffer.empty()) {
      if (not block) {
        return false;
      }
      nof_waiting++;
      if (until == nullptr) {
        cvar_empty.wait(lock, [this]() { return not circ_buffer.empty() or not active; });
      } else {
        cvar_empty.wait_until(lock, *until, [this]() { return not circ_buffer.empty() or not active; });
      }
      nof_waiting--;
      if (circ_buffer.empty()) {
        // either queue got deactivated or there was a timeout
        return false;
      }
    }
    obj = std::move(circ_buffer.top());
    pop_func(obj);
    circ_buffer.pop();
    lock.unlock();
    cvar_full.notify_one();
    return true;
  }
};

} // namespace detail

/**
 * Circular buffer with fixed, embedded buffer storage via a std::array<T, N>.
 * - Single allocation at object creation for std::array. Given that the buffer size is known at compile-time, the
 *   circular iteration over the buffer may be more optimized (e.g. when N is a power of 2, % operator can be avoided)
 * - not thread-safe
 * @tparam T value type stored by buffer
 * @tparam N size of the queue
 */
template <typename T, size_t N>
class static_circular_buffer : public detail::base_circular_buffer<std::array<detail::type_storage<T>, N> >
{
  using base_t = detail::base_circular_buffer<std::array<detail::type_storage<T>, N> >;

public:
  static_circular_buffer() = default;
  static_circular_buffer(const static_circular_buffer& other) : base_t(other.rpos, other.count)
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copy-constructible");
    std::uninitialized_copy(other.begin(), other.end(), base_t::begin());
  }
  static_circular_buffer(static_circular_buffer<T, N>&& other) noexcept : base_t(other.rpos, other.count)
  {
    static_assert(std::is_move_constructible<T>::value, "T must be move-constructible");
    for (size_t i = 0; i < other.count; ++i) {
      size_t idx = (other.rpos + i) % other.max_size();
      base_t::buffer[idx].move_ctor(std::move(other.buffer[idx]));
    }
    other.clear();
  }
  static_circular_buffer& operator=(const static_circular_buffer& other)
  {
    if (this == &other) {
      return *this;
    }
    base_t::clear();
    base_t::rpos  = other.rpos;
    base_t::count = other.count;
    for (size_t i = 0; i < other.count; ++i) {
      size_t idx = (other.rpos + i) % other.max_size();
      base_t::buffer[idx].copy_ctor(other.buffer[idx]);
    }
    return *this;
  }
  static_circular_buffer& operator=(static_circular_buffer&& other) noexcept
  {
    base_t::clear();
    base_t::rpos  = other.rpos;
    base_t::count = other.count;
    for (size_t i = 0; i < other.count; ++i) {
      size_t idx = (other.rpos + i) % other.max_size();
      base_t::buffer[idx].move_ctor(std::move(other.buffer[idx]));
    }
    other.clear();
    return *this;
  }
};

/**
 * Circular buffer with buffer storage via a std::vector<T>.
 * - size can be defined at run-time.
 * - not thread-safe
 * @tparam T value type stored by buffer
 */
template <typename T>
class dyn_circular_buffer : public detail::base_circular_buffer<std::vector<detail::type_storage<T> > >
{
  using base_t = detail::base_circular_buffer<std::vector<detail::type_storage<T> > >;

public:
  dyn_circular_buffer() = default;
  explicit dyn_circular_buffer(size_t max_size) : base_t(0, 0, max_size) {}
  dyn_circular_buffer(dyn_circular_buffer&& other) noexcept : base_t(other.rpos, other.count, std::move(other.buffer))
  {
    other.count = 0;
    other.rpos  = 0;
  }
  dyn_circular_buffer(const dyn_circular_buffer& other) : base_t(other.rpos, other.count, other.max_size())
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copy-constructible");
    for (size_t i = 0; i < other.count; ++i) {
      size_t idx = (other.rpos + i) % other.max_size();
      base_t::buffer[idx].copy_ctor(other.buffer[idx]);
    }
  }
  dyn_circular_buffer& operator=(dyn_circular_buffer other) noexcept
  {
    swap(other);
    other.clear();
    return *this;
  }

  void swap(dyn_circular_buffer& other) noexcept
  {
    std::swap(base_t::rpos, other.rpos);
    std::swap(base_t::count, other.count);
    std::swap(base_t::buffer, other.buffer);
  }

  void set_size(size_t sz)
  {
    srsran_assert(base_t::empty() or sz == base_t::size(),
                  "Dynamic resizes not supported when circular buffer is not empty");
    base_t::buffer.resize(sz);
  }
};

/**
 * Blocking queue with fixed, embedded buffer storage via a std::array<T, N>.
 * - Blocking push/pop API via push_blocking(...) and pop_blocking(...) methods
 * - Non-blocking push/pop API via try_push(...) and try_pop(...) methods
 * - Only one initial allocation for the std::array<T, N>
 * - thread-safe
 * @tparam T value type stored by buffer
 * @tparam N size of queue
 * @tparam PushingCallback function void(const T&) called while pushing an element to the queue
 * @tparam PoppingCallback function void(const T&) called while popping an element from the queue
 */
template <typename T,
          size_t N,
          typename PushingCallback = detail::noop_operator,
          typename PoppingCallback = detail::noop_operator>
class static_blocking_queue
  : public detail::base_blocking_queue<static_circular_buffer<T, N>, PushingCallback, PoppingCallback>
{
  using base_t = detail::base_blocking_queue<static_circular_buffer<T, N>, PushingCallback, PoppingCallback>;

public:
  explicit static_blocking_queue(PushingCallback push_callback = {}, PoppingCallback pop_callback = {}) :
    base_t(push_callback, pop_callback)
  {}
};

/**
 * Blocking queue with buffer storage represented via a std::vector<T>. Features:
 * - Blocking push/pop API via push_blocking(...) and pop_blocking(...) methods
 * - Non-blocking push/pop API via try_push(...) and try_pop(...) methods
 * - Size can be defined at runtime.
 * - thread-safe
 * @tparam T value type stored by buffer
 * @tparam PushingCallback function void(const T&) called while pushing an element to the queue
 * @tparam PoppingCallback function void(const T&) called while popping an element from the queue
 */
template <typename T,
          typename PushingCallback = detail::noop_operator,
          typename PoppingCallback = detail::noop_operator>
class dyn_blocking_queue : public detail::base_blocking_queue<dyn_circular_buffer<T>, PushingCallback, PoppingCallback>
{
  using base_t = detail::base_blocking_queue<dyn_circular_buffer<T>, PushingCallback, PoppingCallback>;

public:
  dyn_blocking_queue() = default;
  explicit dyn_blocking_queue(size_t size, PushingCallback push_callback = {}, PoppingCallback pop_callback = {}) :
    base_t(push_callback, pop_callback, size)
  {}
  void set_size(size_t size) { base_t::circ_buffer.set_size(size); }
};

} // namespace srsran

#endif // SRSRAN_CIRCULAR_BUFFER_H
