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

#ifndef SRSLTE_ID_MAP_H
#define SRSLTE_ID_MAP_H

#include "expected.h"
#include <array>
#include <cassert>

template <typename K, typename T, size_t N>
class static_circular_map
{
  static_assert(std::is_integral<K>::value, "Map key must be an integer");

  using obj_t         = std::pair<K, T>;
  using obj_storage_t = typename std::aligned_storage<sizeof(obj_t), alignof(obj_t)>::type;

public:
  bool has_key(K id)
  {
    size_t idx = id % N;
    return present[idx] and get_obj_(idx).first == id;
  }

  bool insert(K id, const T& obj)
  {
    size_t idx = id % N;
    if (present[idx]) {
      return false;
    }
    new (&buffer[idx]) obj_t(id, obj);
    present[idx] = true;
    count++;
    return true;
  }
  srslte::error_type<T> insert(K id, T&& obj)
  {
    size_t idx = id % N;
    if (present[idx]) {
      return srslte::error_type<T>(std::move(obj));
    }
    new (&buffer[idx]) obj_t(id, std::move(obj));
    present[idx] = true;
    count++;
    return {};
  }

  bool erase(K id)
  {
    if (not has_key(id)) {
      return false;
    }
    size_t idx = id % N;
    get_obj_(idx).~obj_t();
    present[idx] = false;
    --count;
    return true;
  }

  T& operator[](K id)
  {
    assert(has_key(id));
    return get_obj_(id % N).second;
  }
  const T& operator[](K id) const
  {
    assert(has_key(id));
    return get_obj_(id % N).second;
  }

  size_t size() const { return count; }
  bool   empty() const { return count == 0; }
  bool   full() const { return count == N; }
  size_t capacity() const { return N; }

  class iterator
  {
  public:
    iterator() = default;
    iterator(static_circular_map<K, T, N>* map, size_t idx_) : ptr(map), idx(idx_)
    {
      if (idx < ptr->capacity() and not ptr->present[idx]) {
        ++(*this);
      }
    }

    iterator& operator++()
    {
      while (++idx < ptr->capacity() and not ptr->present[idx]) {
      }
      return *this;
    }

    obj_t&       operator*() { return ptr->get_obj_(idx); }
    obj_t*       operator->() { return &ptr->get_obj_(idx); }
    const obj_t* operator*() const { return ptr->buffer[idx]; }
    const obj_t* operator->() const { return ptr->buffer[idx]; }

    bool operator==(const iterator& other) const { return ptr == other.ptr and idx == other.idx; }
    bool operator!=(const iterator& other) const { return not(*this == other); }

  private:
    static_circular_map<K, T, N>* ptr = nullptr;
    size_t                        idx = 0;
  };
  class const_iterator
  {
  public:
    const_iterator() = default;
    const_iterator(static_circular_map<K, T, N>* map, size_t idx_) : ptr(map), idx(idx_) {}

    const_iterator& operator++()
    {
      while (++idx < ptr->capacity() and not ptr->present[idx]) {
      }
      return *this;
    }

    const obj_t* operator*() const { return ptr->buffer[idx]; }
    const obj_t* operator->() const { return ptr->buffer[idx]; }

    bool operator==(const const_iterator& other) const { return ptr == other.ptr and idx == other.idx; }
    bool operator!=(const const_iterator& other) const { return not(*this == other); }

  private:
    const static_circular_map<K, T, N>* ptr = nullptr;
    size_t                              idx = 0;
  };

  iterator       begin() { return iterator(this, 0); }
  iterator       end() { return iterator(this, N); }
  const_iterator begin() const { return iterator(this, 0); }
  const_iterator end() const { return iterator(this, N); }

  iterator find(K id)
  {
    if (has_key(id)) {
      return iterator(this, id % N);
    }
    return end();
  }
  const_iterator find(K id) const
  {
    if (has_key(id)) {
      return iterator(this, id % N);
    }
    return end();
  }

private:
  obj_t&       get_obj_(size_t idx) { return reinterpret_cast<obj_t&>(buffer[idx]); }
  const obj_t& get_obj_(size_t idx) const { return reinterpret_cast<obj_t&>(buffer[idx]); }

  std::array<obj_storage_t, N> buffer;
  std::array<bool, N>          present = {false};
  size_t                       count   = 0;
};

#endif // SRSLTE_ID_MAP_H
