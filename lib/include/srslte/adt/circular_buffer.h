/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_CIRCULAR_BUFFER_H
#define SRSLTE_CIRCULAR_BUFFER_H

#include "srslte/adt/expected.h"

#include <array>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

namespace srslte {

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

template <typename Container>
class base_circular_buffer
{
  using T = typename Container::value_type;

public:
  using value_type      = T;
  using difference_type = ptrdiff_t;

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
  base_circular_buffer(Args&&... args) : buffer(std::forward<Args>(args)...)
  {}

  void push(T&& t)
  {
    assert(not full());
    size_t wpos  = (rpos + count) % max_size();
    buffer[wpos] = std::move(t);
    count++;
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

private:
  Container buffer;
  size_t    rpos  = 0;
  size_t    count = 0;
};

template <typename CircBuffer>
class base_block_queue
{
  using T = typename CircBuffer::value_type;

public:
  template <typename... Args>
  base_block_queue(Args&&... args) : circ_buffer(std::forward<Args>(args)...)
  {}
  ~base_block_queue() { stop(); }

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
  srslte::error_type<T> try_push(T&& t) { return push_(std::move(t), false); }
  bool                  push(const T& t) { return push_(t, true); }
  srslte::error_type<T> push(T&& t) { return push_(std::move(t), true); }
  bool                  try_pop(T& obj) { return pop_(obj, false); }
  T                     pop()
  {
    T obj{};
    pop_(obj, true);
    return obj;
  }
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
  size_t empty() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return circ_buffer.empty();
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

private:
  bool                    active      = true;
  uint8_t                 nof_waiting = 0;
  mutable std::mutex      mutex;
  std::condition_variable cvar_empty, cvar_full;
  CircBuffer              circ_buffer;

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
    circ_buffer.push(t);
    lock.unlock();
    cvar_empty.notify_one();
    return true;
  }
  srslte::error_type<T> push_(T&& t, bool block_mode)
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
    circ_buffer.push(t);
    lock.unlock();
    cvar_empty.notify_one();
    return {};
  }

  bool pop_(T& obj, bool block)
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
      while (circ_buffer.empty() and active) {
        cvar_empty.wait(lock);
      }
      nof_waiting--;
      if (not active) {
        return false;
      }
    }
    obj = std::move(circ_buffer.top());
    circ_buffer.pop();
    lock.unlock();
    cvar_full.notify_one();
    return true;
  }
};

} // namespace detail

template <typename T, size_t N>
class static_circular_buffer : public detail::base_circular_buffer<std::array<T, N> >
{};

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
    base_t::buffer.resize(size);
  }
};

template <typename T, size_t N>
class static_block_queue : public detail::base_block_queue<static_circular_buffer<T, N> >
{};

template <typename T>
class dyn_block_queue : public detail::base_block_queue<dyn_circular_buffer<T> >
{
  using base_t = detail::base_block_queue<dyn_circular_buffer<T> >;

public:
  dyn_block_queue() = default;
  explicit dyn_block_queue(size_t size) : base_t(size) {}
  void set_size(size_t size) { base_t::circ_buffer.set_size(size); }
};

} // namespace srslte

#endif // SRSLTE_CIRCULAR_BUFFER_H
