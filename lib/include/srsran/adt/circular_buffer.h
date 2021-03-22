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

#ifndef SRSRAN_CIRCULAR_BUFFER_H
#define SRSRAN_CIRCULAR_BUFFER_H

#include "srsran/adt/expected.h"

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
  using T = typename Container::value_type;

public:
  using value_type      = T;
  using difference_type = typename Container::difference_type;

  struct iterator {
    iterator(base_circular_buffer<Container>& parent_, size_t i) : parent(&parent_), idx(i) {}
    iterator& operator++()
    {
      idx = (idx + 1) % parent->max_size();
      return *this;
    }
    iterator operator++(int)
    {
      iterator tmp(*this);
      ++(*this);
      return tmp;
    }
    iterator operator+(difference_type n)
    {
      iterator tmp(*this);
      tmp += n;
      return tmp;
    }
    iterator& operator+=(difference_type n)
    {
      idx = (idx + n) % parent->max_size();
      return *this;
    }
    value_type*       operator->() { return &parent->buffer[idx]; }
    const value_type* operator->() const { return &parent->buffer[idx]; }
    value_type&       operator*() { return parent->buffer[idx]; }
    const value_type& operator*() const { return parent->buffer[idx]; }
    bool              operator==(const iterator& it) const { return it.parent == parent and it.idx == idx; }
    bool              operator!=(const iterator& it) const { return not(*this == it); }

  private:
    base_circular_buffer<Container>* parent;
    size_t                           idx;
  };

  template <typename... Args>
  explicit base_circular_buffer(Args&&... args) : buffer(std::forward<Args>(args)...)
  {}

  bool try_push(T&& t)
  {
    if (full()) {
      return false;
    }
    push(std::move(t));
  }
  void push(T&& t)
  {
    assert(not full());
    size_t wpos  = (rpos + count) % max_size();
    buffer[wpos] = std::move(t);
    count++;
  }
  bool try_push(const T& t)
  {
    if (full()) {
      return false;
    }
    push(t);
  }
  void push(const T& t)
  {
    assert(not full());
    size_t wpos  = (rpos + count) % max_size();
    buffer[wpos] = t;
    count++;
  }
  void pop()
  {
    assert(not empty());
    rpos = (rpos + 1) % max_size();
    count--;
  }
  T& top()
  {
    assert(not empty());
    return buffer[rpos];
  }
  const T& top() const
  {
    assert(not empty());
    return buffer[rpos];
  }
  void clear() { count = 0; }

  bool   full() const { return count == max_size(); }
  bool   empty() const { return count == 0; }
  size_t size() const { return count; }
  size_t max_size() const { return detail::get_max_size(buffer); }

  iterator begin() { return iterator(*this, rpos); }
  iterator end() { return iterator(*this, (rpos + count) % max_size()); }

  template <typename = std::enable_if<std::is_same<Container, std::vector<T> >::value> >
  void set_size(size_t size)
  {
    buffer.resize(size);
  }

  template <typename F>
  T discard_if(const F& func)
  {
    for (auto it = buffer.begin(); it != buffer.end(); it++) {
      if (*it != nullptr && func(*it)) {
        T tmp = std::move(*it);
        *it   = nullptr;
        return tmp;
      }
    }
    return nullptr;
  }

private:
  Container buffer;
  size_t    rpos  = 0;
  size_t    count = 0;
};

struct noop_operator {
  template <typename T>
  void operator()(const T&)
  {
    // noop
  }
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

  void stop()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (active) {
      active = false;
      if (nof_waiting == 0) {
        return;
      }
      do {
        lock.unlock();
        cvar_empty.notify_all();
        cvar_full.notify_all();
        std::this_thread::yield();
        lock.lock();
      } while (nof_waiting > 0);
    }
  }

  bool                  try_push(const T& t) { return push_(t, false); }
  srsran::error_type<T> try_push(T&& t) { return push_(std::move(t), false); }
  bool                  push_blocking(const T& t) { return push_(t, true); }
  srsran::error_type<T> push_blocking(T&& t) { return push_(std::move(t), true); }
  bool                  try_pop(T& obj) { return pop_(obj, false); }
  T                     pop_blocking()
  {
    T obj{};
    pop_(obj, true);
    return obj;
  }
  bool pop_wait_until(T& obj, const std::chrono::system_clock::time_point& until) { return pop_(obj, true, &until); }
  void clear()
  {
    std::lock_guard<std::mutex> lock(mutex);
    T                           obj;
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
  bool discard_if(const F& func)
  {
    std::lock_guard<std::mutex> lock(mutex);
    T                           tmp = circ_buffer.discard_if(func);
    if (tmp == nullptr) {
      return false;
    }
    pop_func(tmp);
    return true;
  }

protected:
  bool                    active      = true;
  uint8_t                 nof_waiting = 0;
  mutable std::mutex      mutex;
  std::condition_variable cvar_empty, cvar_full;
  PushingFunc             push_func;
  PoppingFunc             pop_func;
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

  bool pop_(T& obj, bool block, const std::chrono::system_clock::time_point* until = nullptr)
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
class static_circular_buffer : public detail::base_circular_buffer<std::array<T, N> >
{};

/**
 * Circular buffer with buffer storage via a std::vector<T>.
 * - size can be defined at run-time.
 * - not thread-safe
 * @tparam T value type stored by buffer
 */
template <typename T>
class dyn_circular_buffer : public detail::base_circular_buffer<std::vector<T> >
{
  using base_t = detail::base_circular_buffer<std::vector<T> >;

public:
  dyn_circular_buffer() = default;
  explicit dyn_circular_buffer(size_t size) : base_t(size) {}

  void set_size(size_t size)
  {
    // Note: dynamic resizes not supported.
    assert(base_t::empty());
    base_t::set_size(size);
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

  template <typename F>
  bool discard_if(const F& func)
  {
    return base_t::discard_if(func);
  }
};

} // namespace srsran

#endif // SRSRAN_CIRCULAR_BUFFER_H
