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

#ifndef SRSLTE_MOVE_CALLBACK_H
#define SRSLTE_MOVE_CALLBACK_H

#include <cstddef>
#include <functional>
#include <type_traits>

namespace srslte {

constexpr size_t default_buffer_size = 32;

template <class Signature, size_t Capacity = default_buffer_size>
class move_callback;

namespace task_details {

template <typename R, typename... Args>
class oper_table_t
{
public:
  constexpr    oper_table_t()                        = default;
  virtual R    call(void* src, Args&&... args) const = 0;
  virtual void move(void* src, void* dest) const     = 0;
  virtual void dtor(void* src) const                 = 0;
  virtual bool is_in_small_buffer() const            = 0;
};

template <typename R, typename... Args>
class empty_table_t : public oper_table_t<R, Args...>
{
public:
  constexpr empty_table_t() = default;
  R         call(void* src, Args&&... args) const final { throw std::bad_function_call(); }
  void      move(void* src, void* dest) const final {}
  void      dtor(void* src) const final {}
  bool      is_in_small_buffer() const final { return true; }
};

template <typename FunT, typename R, typename... Args>
class smallbuffer_table_t : public oper_table_t<R, Args...>
{
public:
  constexpr smallbuffer_table_t() = default;
  R    call(void* src, Args&&... args) const final { return (*static_cast<FunT*>(src))(std::forward<Args>(args)...); }
  void move(void* src, void* dest) const final
  {
    ::new (dest) FunT{std::move(*static_cast<FunT*>(src))};
    static_cast<FunT*>(src)->~FunT();
  }
  void dtor(void* src) const final { static_cast<FunT*>(src)->~FunT(); }
  bool is_in_small_buffer() const final { return true; }
};

template <typename FunT, typename R, typename... Args>
class heap_table_t : public oper_table_t<R, Args...>
{
public:
  constexpr heap_table_t() = default;
  R    call(void* src, Args&&... args) const final { return (**static_cast<FunT**>(src))(std::forward<Args>(args)...); }
  void move(void* src, void* dest) const final
  {
    *static_cast<FunT**>(dest) = *static_cast<FunT**>(src);
    *static_cast<FunT**>(src)  = nullptr;
  }
  void dtor(void* src) const final { delete (*static_cast<FunT**>(src)); }
  bool is_in_small_buffer() const final { return false; }
};

//! Metafunction to check if object is move_callback<> type
template <class>
struct is_move_callback : std::false_type {};
template <class Sig, size_t Capacity>
struct is_move_callback<move_callback<Sig, Capacity> > : std::true_type {};

//! metafunctions to enable/disable functions based on whether the callback fits small buffer or not
template <typename T, size_t Cap, typename FunT = typename std::decay<T>::type>
using enable_if_small_capture =
    typename std::enable_if<sizeof(FunT) <= Cap and not is_move_callback<FunT>::value, bool>::type;
template <typename T, size_t Cap, typename FunT = typename std::decay<T>::type>
using enable_if_big_capture =
    typename std::enable_if < Cap<sizeof(FunT) and not is_move_callback<FunT>::value, bool>::type;

} // namespace task_details

template <class R, class... Args, size_t Capacity>
class move_callback<R(Args...), Capacity>
{
  static constexpr size_t capacity = Capacity >= sizeof(void*) ? Capacity : sizeof(void*);
  using storage_t                  = typename std::aligned_storage<capacity, alignof(std::max_align_t)>::type;
  using oper_table_t               = task_details::oper_table_t<R, Args...>;
  static constexpr task_details::empty_table_t<R, Args...> empty_table{};

public:
  move_callback() noexcept : oper_ptr(&empty_table) {}

  template <typename T, task_details::enable_if_small_capture<T, capacity> = true>
  move_callback(T&& function) noexcept
  {
    using FunT = typename std::decay<T>::type;
    static const task_details::smallbuffer_table_t<FunT, R, Args...> small_oper_table{};
    oper_ptr = &small_oper_table;
    ::new (&buffer) FunT{std::forward<T>(function)};
  }

  template <typename T, task_details::enable_if_big_capture<T, capacity> = true>
  move_callback(T&& function)
  {
    using FunT = typename std::decay<T>::type;
    static const task_details::heap_table_t<FunT, R, Args...> heap_oper_table{};
    oper_ptr = &heap_oper_table;
    ptr      = static_cast<void*>(new FunT{std::forward<T>(function)});
  }

  move_callback(move_callback&& other) noexcept : oper_ptr(other.oper_ptr)
  {
    other.oper_ptr = &empty_table;
    oper_ptr->move(&other.buffer, &buffer);
  }

  ~move_callback() { oper_ptr->dtor(&buffer); }

  move_callback& operator=(move_callback&& other) noexcept
  {
    oper_ptr->dtor(&buffer);
    oper_ptr       = other.oper_ptr;
    other.oper_ptr = &empty_table;
    oper_ptr->move(&other.buffer, &buffer);
    return *this;
  }

  R operator()(Args&&... args) const noexcept { return oper_ptr->call(&buffer, std::forward<Args>(args)...); }

  bool is_empty() const { return oper_ptr == empty_table; }
  bool is_in_small_buffer() const { return oper_ptr->is_in_small_buffer(); }

private:
  union {
    mutable storage_t buffer;
    void*             ptr;
  };
  const oper_table_t* oper_ptr;
};

template <typename R, typename... Args, size_t Capacity>
constexpr task_details::empty_table_t<R, Args...> move_callback<R(Args...), Capacity>::empty_table;

} // namespace srslte

#endif // SRSLTE_MOVE_CALLBACK_H
