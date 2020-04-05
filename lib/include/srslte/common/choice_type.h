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

#include "type_utils.h"

#ifndef SRSLTE_CHOICE_TYPE_H
#define SRSLTE_CHOICE_TYPE_H

namespace srslte {

//! Compute maximum at compile time
template <std::size_t arg1, std::size_t... others>
struct static_max;
template <std::size_t arg>
struct static_max<arg> {
  static const std::size_t value = arg;
};
template <std::size_t arg1, std::size_t arg2, std::size_t... others>
struct static_max<arg1, arg2, others...> {
  static const std::size_t value =
      arg1 >= arg2 ? static_max<arg1, others...>::value : static_max<arg2, others...>::value;
};

namespace choice_details {

//! Holds one of the Args types
template <std::size_t MaxSize, std::size_t MaxAlign>
struct choice_storage_t {
  static const std::size_t max_size  = MaxSize;
  static const std::size_t max_align = MaxAlign;
  using buffer_t                     = typename std::aligned_storage<max_size, max_align>::type;
  buffer_t buffer;

  void* get_buffer() { return &buffer; }

  template <typename T>
  T& get_unchecked()
  {
    return *reinterpret_cast<T*>(&buffer);
  }

  template <typename T>
  const T& get_unchecked() const
  {
    return *reinterpret_cast<const T*>(&buffer);
  }

  template <typename U>
  void destroy_unsafe()
  {
    get_unchecked<U>().~U();
  };
};

/*************************
 *  Tagged Union Helpers
 ************************/

template <typename C>
struct CopyCtorVisitor {
  explicit CopyCtorVisitor(C* c_) : c(c_) {}
  template <typename T>
  void operator()(const T& t)
  {
    c->construct_unsafe(t);
  }
  C* c;
};

template <typename C>
struct MoveCtorVisitor {
  explicit MoveCtorVisitor(C* c_) : c(c_) {}
  template <typename T>
  void operator()(T&& t)
  {
    c->construct_unsafe(std::move(t));
  }
  C* c;
};

template <typename C>
struct DtorUnsafeVisitor {
  explicit DtorUnsafeVisitor(C* c_) : c(c_) {}
  template <typename T>
  void operator()(T& t)
  {
    c->template destroy_unsafe<T>();
  }
  C* c;
};

template <typename... Args>
struct tagged_union_t;

template <typename... Args>
struct tagged_union_t
  : private choice_storage_t<static_max<sizeof(Args)...>::value, static_max<alignof(Args)...>::value> {
  using base_t    = choice_storage_t<static_max<sizeof(Args)...>::value, static_max<alignof(Args)...>::value>;
  using buffer_t  = typename base_t::buffer_t;
  using this_type = tagged_union_t<Args...>;

  std::size_t type_id;

  using base_t::destroy_unsafe;
  using base_t::get_buffer;
  using base_t::get_unchecked;

  template <typename U, typename... Args2>
  void construct_emplace_unsafe(Args2&&... args)
  {
    using U2 = typename std::decay<U>::type;
    static_assert(type_list_contains<U2, Args...>(),
                  "The provided type to ctor is not part of the list of possible types");
    type_id = get_type_index<U2, Args...>();
    new (get_buffer()) U2(std::forward<Args2>(args)...);
  }

  template <typename U>
  void construct_unsafe(U&& u)
  {
    using U2 = typename std::decay<U>::type;
    static_assert(type_list_contains<U2, Args...>(),
                  "The provided type to ctor is not part of the list of possible types");
    type_id = get_type_index<U2, Args...>();
    new (get_buffer()) U2(std::forward<U>(u));
  }

  void copy_unsafe(const this_type& other) { visit(CopyCtorVisitor<this_type>{this}, other); }

  void move_unsafe(this_type&& other) { visit(MoveCtorVisitor<this_type>{this}, other); }

  void dtor_unsafe() { visit(choice_details::DtorUnsafeVisitor<base_t>{this}, *this); }

  size_t get_type_idx() const { return type_id; }

  template <typename T>
  bool is() const
  {
    return get_type_index<T, Args...>() == type_id;
  }

  template <typename T>
  constexpr static bool can_hold_type()
  {
    return type_list_contains<T, Args...>();
  }
};

} // namespace choice_details

template <typename... Args>
class choice_t : private choice_details::tagged_union_t<Args...>
{
  using base_t = choice_details::tagged_union_t<Args...>;

public:
  using default_type = typename get_index_type<0, Args...>::type;
  //! Useful metafunction
  template <typename T>
  using enable_if_can_hold =
      typename std::enable_if<base_t::template can_hold_type<typename std::decay<T>::type>()>::type;
  template <typename T>
  using disable_if_can_hold =
      typename std::enable_if<not base_t::template can_hold_type<typename std::decay<T>::type>()>::type;

  using base_t::can_hold_type;
  using base_t::get_unchecked;
  using base_t::is;

  template <typename... Args2,
            typename = typename std::enable_if<std::is_constructible<default_type, Args2...>::value>::type>
  explicit choice_t(Args2&&... args) noexcept
  {
    base_t::template construct_emplace_unsafe<default_type>(std::forward<Args2>(args)...);
  }

  choice_t(const choice_t<Args...>& other) noexcept { base_t::copy_unsafe(other); }

  choice_t(choice_t<Args...>&& other) noexcept { base_t::move_unsafe(std::move(other)); }

  template <typename U, typename = enable_if_can_hold<U> >
  choice_t(U&& u) noexcept
  {
    base_t::construct_unsafe(std::forward<U>(u));
  }

  ~choice_t() { base_t::dtor_unsafe(); }

  template <typename U, typename = enable_if_can_hold<U> >
  choice_t& operator=(U&& u) noexcept
  {
    if (not base_t::template is<U>()) {
      base_t::dtor_unsafe();
    }
    base_t::construct_unsafe(std::forward<U>(u));
    return *this;
  }

  template <typename U, typename... Args2>
  void emplace(Args2&&... args) noexcept
  {
    base_t::dtor_unsafe();
    base_t::template construct_emplace_unsafe<U>(std::forward<Args2>(args)...);
  }

  choice_t& operator=(const choice_t& other) noexcept
  {
    if (this != &other) {
      base_t::dtor_unsafe();
      base_t::copy_unsafe(other);
    }
    return *this;
  }

  choice_t& operator=(choice_t&& other) noexcept
  {
    base_t::dtor_unsafe();
    base_t::move_unsafe(std::move(other));
    return *this;
  }

  bool holds_same_type(const choice_t& other) noexcept { return base_t::type_id == other.type_id; }
};

} // namespace srslte

#endif // SRSLTE_CHOICE_TYPE_H
