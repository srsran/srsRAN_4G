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
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <stdint.h>
#include <type_traits>

#if defined(__cpp_exceptions) && (1 == __cpp_exceptions)
#define THROW_BAD_FUNCTION_CALL(const char* cause) throw std::bad_function_call{};
#else
#define THROW_BAD_FUNCTION_CALL(cause)                                                                                 \
  fprintf(stderr, "ERROR: exception thrown due to bad function call (cause: %s)\n", cause);                            \
  std::abort()
#endif

namespace srslte {

// NOTE: gcc 4.8.5 is missing std::max_align_t. Need to create a struct
union max_alignment_t {
  char        c;
  float       f;
  uint32_t    i;
  uint64_t    i2;
  double      d;
  long double d2;
  uint32_t*   ptr;
};

//! Size of the buffer used by "move_callback<R(Args...)>" to store functors without calling "new"
constexpr size_t default_buffer_size = 32;

template <class Signature, size_t Capacity = default_buffer_size>
class move_callback;

namespace task_details {

//! Base vtable for move/call/destroy operations over the functor stored in "move_callback<R(Args...)"
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

//! specialization of move/call/destroy operations for when the "move_callback<R(Args...)>" is empty
template <typename R, typename... Args>
class empty_table_t : public oper_table_t<R, Args...>
{
public:
  constexpr empty_table_t() = default;
  R         call(void* src, Args&&... args) const final { THROW_BAD_FUNCTION_CALL("function ptr is empty"); }
  void      move(void* src, void* dest) const final {}
  void      dtor(void* src) const final {}
  bool      is_in_small_buffer() const final { return true; }
};

//! specialization of move/call/destroy operations for when the functor is stored in "move_callback<R(Args...)>" buffer
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

//! move/call/destroy operations for when the functor is stored outside of "move_callback<R(Args...)>" buffer
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

//! Metafunction to check if a type is an instantiation of move_callback<R(Args...)>
template <class>
struct is_move_callback : std::false_type {};
template <class Sig, size_t Capacity>
struct is_move_callback<move_callback<Sig, Capacity> > : std::true_type {};

//! metafunctions to enable different ctor implementations depending on whether the callback fits the small buffer
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
  static constexpr size_t capacity = Capacity >= sizeof(void*) ? Capacity : sizeof(void*); ///< size of buffer
  using storage_t                  = typename std::aligned_storage<capacity, alignof(max_alignment_t)>::type;
  using oper_table_t               = task_details::oper_table_t<R, Args...>;
  static constexpr task_details::empty_table_t<R, Args...> empty_table{};

public:
  move_callback() noexcept : oper_ptr(&empty_table) {}

  //! Called when T capture fits the move_callback buffer
  template <typename T, task_details::enable_if_small_capture<T, capacity> = true>
  move_callback(T&& function) noexcept
  {
    using FunT = typename std::decay<T>::type;
    static const task_details::smallbuffer_table_t<FunT, R, Args...> small_oper_table{};
    oper_ptr = &small_oper_table;
    ::new (&buffer) FunT{std::forward<T>(function)};
  }

  //! Called when T capture does not fit the move_callback buffer
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

//! Generic move task
using move_task_t = move_callback<void()>;

} // namespace srslte

#endif // SRSLTE_MOVE_CALLBACK_H
