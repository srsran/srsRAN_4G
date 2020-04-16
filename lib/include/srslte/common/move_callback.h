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

//! Class used to type-erase the functor/lambda capture move/call/dtor operators
template <typename R, typename... Args>
struct oper_table_t {
  using call_oper_t = R (*)(void* src, Args&&... args);
  using move_oper_t = void (*)(void* src, void* dest);
  using dtor_oper_t = void (*)(void* src);

  //! Returns a operator table for when the move_function object is empty
  const static oper_table_t* get_empty() noexcept
  {
    const static oper_table_t t{true,
                                [](void* src, Args&&... args) -> R { throw std::bad_function_call(); },
                                [](void*, void*) {},
                                [](void*) {}};
    return &t;
  }

  //! Returns a operator table for when the move_function fits the small buffer
  template <typename Func>
  const static oper_table_t* get_small() noexcept
  {
    const static oper_table_t t{
        true,
        [](void* src, Args&&... args) -> R { return (*static_cast<Func*>(src))(std::forward<Args>(args)...); },
        [](void* src, void* dest) -> void {
          ::new (dest) Func{std::move(*static_cast<Func*>(src))};
          static_cast<Func*>(src)->~Func();
        },
        [](void* src) -> void { static_cast<Func*>(src)->~Func(); }};
    return &t;
  }

  //! Returns a operator table for when the move_function fits the big buffer
  template <typename Func>
  const static oper_table_t* get_big() noexcept
  {
    const static oper_table_t t{
        false,
        [](void* src, Args&&... args) -> R { return (**static_cast<Func**>(src))(std::forward<Args>(args)...); },
        [](void* src, void* dest) -> void {
          *static_cast<Func**>(dest) = *static_cast<Func**>(src);
          *static_cast<Func**>(src)  = nullptr;
        },
        [](void* src) -> void { delete (*static_cast<Func**>(src)); }};
    return &t;
  }

  oper_table_t(const oper_table_t&) = delete;
  oper_table_t(oper_table_t&&)      = delete;
  oper_table_t& operator=(const oper_table_t&) = delete;
  oper_table_t& operator=(oper_table_t&&) = delete;
  ~oper_table_t()                         = default;

  const bool        is_in_buffer;
  const call_oper_t call;
  const move_oper_t move;
  const dtor_oper_t dtor;

private:
  oper_table_t(bool is_in_buffer_, call_oper_t call_, move_oper_t move_, dtor_oper_t dtor_) :
    is_in_buffer(is_in_buffer_),
    call(call_),
    move(move_),
    dtor(dtor_)
  {}
};

template <class>
struct is_inplace_task : std::false_type {};
template <class Sig, size_t Capacity>
struct is_inplace_task<move_callback<Sig, Capacity> > : std::true_type {};

template <typename T, size_t Cap, typename FunT = typename std::decay<T>::type>
using enable_small_capture =
    typename std::enable_if<sizeof(FunT) <= Cap and not is_inplace_task<FunT>::value, bool>::type;
template <typename T, size_t Cap, typename FunT = typename std::decay<T>::type>
using enable_big_capture = typename std::enable_if < Cap<sizeof(FunT) and not is_inplace_task<FunT>::value, bool>::type;

} // namespace task_details

template <class R, class... Args, size_t Capacity>
class move_callback<R(Args...), Capacity>
{
  static constexpr size_t capacity = Capacity >= sizeof(void*) ? Capacity : sizeof(void*);
  using storage_t                  = typename std::aligned_storage<capacity, alignof(std::max_align_t)>::type;
  using oper_table_t               = task_details::oper_table_t<R, Args...>;

public:
  move_callback() noexcept { oper_ptr = oper_table_t::get_empty(); }

  template <typename T, task_details::enable_small_capture<T, capacity> = true>
  move_callback(T&& function) noexcept
  {
    using FunT = typename std::decay<T>::type;
    oper_ptr   = oper_table_t::template get_small<FunT>();
    ::new (&buffer) FunT{std::forward<T>(function)};
  }

  template <typename T, task_details::enable_big_capture<T, capacity> = true>
  move_callback(T&& function)
  {
    using FunT = typename std::decay<T>::type;
    oper_ptr   = oper_table_t::template get_big<FunT>();
    ptr        = static_cast<void*>(new FunT{std::forward<T>(function)});
  }

  move_callback(move_callback&& other) noexcept
  {
    oper_ptr       = other.oper_ptr;
    other.oper_ptr = oper_table_t::get_empty();
    oper_ptr->move(&other.buffer, &buffer);
  }

  ~move_callback() { oper_ptr->dtor(&buffer); }

  move_callback& operator=(move_callback&& other) noexcept
  {
    oper_ptr->dtor(&buffer);
    oper_ptr       = other.oper_ptr;
    other.oper_ptr = oper_table_t::get_empty();
    oper_ptr->move(&other.buffer, &buffer);
    return *this;
  }

  R operator()(Args&&... args) const noexcept { return oper_ptr->call(&buffer, std::forward<Args>(args)...); }

  bool is_empty() const { return oper_ptr == oper_table_t::get_empty(); }
  bool is_in_small_buffer() const { return oper_ptr->is_in_buffer; }

  void swap(move_callback& other) noexcept
  {
    if (this == &other)
      return;

    if (oper_ptr->is_in_buffer and other.oper_ptr->is_in_buffer) {
      storage_t tmp;
      oper_ptr->move(&buffer, &tmp);
      other.oper_ptr->move(&other.buffer, &buffer);
      oper_ptr->move(&tmp, &other.buffer);
    } else if (oper_ptr->is_in_buffer and not other.oper_ptr->is_in_buffer) {
      void* tmpptr = other.ptr;
      oper_ptr->move(&buffer, &other.buffer);
      ptr = tmpptr;
    } else if (not oper_ptr->is_in_buffer and other.oper_ptr->is_in_buffer) {
      void* tmpptr = ptr;
      other.oper_ptr->move(&other.buffer, &buffer);
      other.ptr = tmpptr;
    } else {
      std::swap(ptr, other.ptr);
    }
    std::swap(oper_ptr, other.oper_ptr);
  }

  friend void swap(move_callback& lhs, move_callback& rhs) noexcept { lhs.swap(rhs); }

private:
  union {
    mutable storage_t buffer;
    void*             ptr;
  };
  const oper_table_t* oper_ptr;
};

} // namespace srslte

#endif // SRSLTE_MOVE_CALLBACK_H
