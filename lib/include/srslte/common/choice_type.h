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

#include <limits>
#include <memory>
#include <type_traits>

#ifndef SRSLTE_CHOICE_TYPE_H
#define SRSLTE_CHOICE_TYPE_H

namespace srslte {

namespace choice_details {

using size_idx_t = std::size_t;

static constexpr size_idx_t invalid_idx = std::numeric_limits<size_idx_t>::max();

class bad_choice_access : public std::runtime_error
{
public:
  explicit bad_choice_access(const std::string& what_arg) : runtime_error(what_arg) {}
  explicit bad_choice_access(const char* what_arg) : runtime_error(what_arg) {}
};

//! Get Index of a type in a list of types (reversed)
template <typename T, typename... Types>
struct type_indexer;

template <typename T, typename First, typename... Types>
struct type_indexer<T, First, Types...> {
  static constexpr size_idx_t index =
      std::is_same<T, First>::value ? sizeof...(Types) : type_indexer<T, Types...>::index;
};

template <typename T>
struct type_indexer<T> {
  static constexpr size_idx_t index = invalid_idx;
};

//! Get a type of an index in a list of types
template <std::size_t I, typename... Types>
struct type_get;

template <std::size_t I, typename First, typename... Types>
struct type_get<I, First, Types...> {
  using type = typename std::conditional<I == sizeof...(Types), First, typename type_get<I, Types...>::type>::type;
};

template <std::size_t I>
struct type_get<I> {
  using type = void;
};

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

//! Holds one of the Args types
template <std::size_t MaxSize, std::size_t MaxAlign>
struct choice_storage_t {
  static const std::size_t max_size  = MaxSize;
  static const std::size_t max_align = MaxAlign;
  using buffer_t                     = typename std::aligned_storage<max_size, max_align>::type;
  buffer_t buffer;

  void* get_buffer() { return &buffer; }

  template <typename T>
  T& get_unsafe()
  {
    return *reinterpret_cast<T*>(&buffer);
  }

  template <typename T>
  const T& get_unsafe() const
  {
    return *reinterpret_cast<const T*>(&buffer);
  }

  template <typename U>
  void destroy_unsafe()
  {
    get_unsafe<U>().~U();
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

/**
 * @brief visit pattern implementation
 * @tparam F functor
 * @tparam V tagged union type
 * @tparam Types remaining types to iterate
 */
template <typename F, typename V, typename... Types>
struct visit_impl;

template <typename F, typename V, typename First, typename... Types>
struct visit_impl<F, V, First, Types...> {
  static void apply(V& c, F&& f)
  {
    if (c.template is<First>()) {
      f(c.template get_unsafe<First>());
    } else {
      visit_impl<F, V, Types...>::apply(c, std::forward<F>(f));
    }
  }
  static void apply(const V& c, F&& f)
  {
    if (c.template is<First>()) {
      f(c.template get_unsafe<First>());
    } else {
      visit_impl<F, V, Types...>::apply(c, std::forward<F>(f));
    }
  }
};
template <typename F, typename V, typename T>
struct visit_impl<F, V, T> {
  static void apply(V& c, F&& f) { f(c.template get_unsafe<T>()); }
  static void apply(const V& c, F&& f) { f(c.template get_unsafe<T>()); }
};

template <typename... Args>
struct tagged_union_t;

template <typename Functor, typename First, typename... Args>
void visit(tagged_union_t<First, Args...>& u, Functor&& f)
{
  visit_impl<Functor, tagged_union_t<First, Args...>, First, Args...>::apply(u, std::forward<Functor>(f));
}
template <typename Functor, typename First, typename... Args>
void visit(const tagged_union_t<First, Args...>& u, Functor&& f)
{
  visit_impl<Functor, tagged_union_t<First, Args...>, First, Args...>::apply(u, std::forward<Functor>(f));
}

template <typename... Args>
struct tagged_union_t
  : private choice_storage_t<static_max<sizeof(Args)...>::value, static_max<alignof(Args)...>::value> {
  using base_t       = choice_storage_t<static_max<sizeof(Args)...>::value, static_max<alignof(Args)...>::value>;
  using buffer_t     = typename base_t::buffer_t;
  using this_type    = tagged_union_t<Args...>;
  using default_type = typename type_get<sizeof...(Args) - 1, Args...>::type;

  std::size_t type_id;

  using base_t::destroy_unsafe;
  using base_t::get_buffer;
  using base_t::get_unsafe;

  auto construct_unsafe() -> decltype(default_type(), void())
  {
    type_id = sizeof...(Args) - 1;
    new (get_buffer()) default_type();
  }

  template <typename U>
  void construct_unsafe(U&& u)
  {
    using U2 = typename std::decay<U>::type;
    static_assert(type_indexer<U2, Args...>::index != invalid_idx,
                  "The provided type to ctor is not part of the list of possible types");
    type_id = type_indexer<U2, Args...>::index;
    new (get_buffer()) U2(std::forward<U>(u));
  }

  void copy_unsafe(const this_type& other) { visit(other, CopyCtorVisitor<this_type>{this}); }

  void move_unsafe(this_type&& other) { visit(other, MoveCtorVisitor<this_type>{this}); }

  void dtor_unsafe() { visit(*this, choice_details::DtorUnsafeVisitor<base_t>{this}); }

  template <std::size_t I, typename T = typename type_get<sizeof...(Args) - I - 1, Args...>::type>
  T& get_unsafe()
  {
    return get_unsafe<T>();
  }

  template <typename T>
  bool is() const
  {
    return type_indexer<T, Args...>::index == type_id;
  }

  template <typename T>
  T& get()
  {
    if (is<T>()) {
      return get_unsafe<T>();
    }
    throw choice_details::bad_choice_access{"in get<T>"};
  }

  template <typename T>
  T* get_if()
  {
    return (is<T>()) ? &get_unsafe<T>() : nullptr;
  }

  template <typename T>
  constexpr static bool can_hold_type()
  {
    return type_indexer<T, Args...>::index != invalid_idx;
  }
};

} // namespace choice_details

template <typename... Args>
class choice_t : private choice_details::tagged_union_t<Args...>
{
  using base_t = choice_details::tagged_union_t<Args...>;
  template <typename T>
  using enable_if_can_hold =
      typename std::enable_if<base_t::template can_hold_type<typename std::decay<T>::type>()>::type;

public:
  using base_t::can_hold_type;
  using base_t::get;
  using base_t::get_if;
  using base_t::is;

  choice_t() noexcept { this_union().construct_unsafe(); }

  choice_t(const choice_t<Args...>& other) noexcept { base_t::copy_unsafe(other); }

  choice_t(choice_t<Args...>&& other) noexcept { base_t::move_unsafe(std::move(other)); }

  template <typename U, typename = enable_if_can_hold<U> >
  explicit choice_t(U&& u) noexcept
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

  template <typename U>
  void emplace(U&& u) noexcept
  {
    *this = std::forward<U>(u);
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
    base_t::move_unsafe(other);
    return *this;
  }

private:
  base_t&       this_union() { return *this; }
  const base_t& this_union() const { return *this; }
};

// template <typename Functor, typename First, typename... Args>
// void visit(choice_t<First, Args...>& u, Functor&& f)
//{
//  choice_details::visit_impl<Functor, decltype(u), First, Args...>::template apply(u, f);
//}

} // namespace srslte

#endif // SRSLTE_CHOICE_TYPE_H
