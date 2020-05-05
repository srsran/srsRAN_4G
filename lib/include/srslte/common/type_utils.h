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

#ifndef SRSLTE_TYPE_UTILS_H
#define SRSLTE_TYPE_UTILS_H

#include <cstring>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>

namespace srslte {

#if defined(__cpp_exceptions) && (1 == __cpp_exceptions)
class bad_type_access : public std::runtime_error
{
public:
  explicit bad_type_access(const std::string& what_arg) : runtime_error(what_arg) {}
  explicit bad_type_access(const char* what_arg) : runtime_error(what_arg) {}
};

#define THROW_BAD_ACCESS(msg) throw bad_type_access{msg};
#else
#define THROW_BAD_ACCESS(msg)                                                                                          \
  fprintf(stderr, "ERROR: exception thrown at %s", msg);                                                               \
  std::abort()
#endif

/************************************
 *       get_type_name methods
 ***********************************/

//! Helper to print the name of a type for logging
/**
 * @brief Helper function that returns a type name string
 */
#if defined(__GNUC__) && !defined(__clang__)

template <typename T>
std::string get_type_name()
{
  static const char*       funcname = __PRETTY_FUNCTION__;
  static const std::string s        = []() {
    static const char* pos1 = strchr(funcname, '=') + 2;
    static const char* pos2 = strchr(pos1, ';');
    std::string        s2{pos1, pos2};
    size_t             colon_pos = s2.rfind(':');
    std::string        s3        = colon_pos == std::string::npos ? s2 : s2.substr(colon_pos + 1, s2.size());
    return s3.find('>') == std::string::npos ? s3 : s2;
  }();
  return s;
}

#elif defined(__clang__)
template <typename T>
std::string get_type_name()
{
  static const char*       funcname = __PRETTY_FUNCTION__;
  static const std::string s        = []() {
    static const char* pos1 = strchr(funcname, '=') + 2;
    static const char* pos2 = strchr(pos1, ']');
    std::string        s2{pos1, pos2};
    size_t             colon_pos = s2.rfind(':');
    std::string        s3        = colon_pos == std::string::npos ? s2 : s2.substr(colon_pos + 1, s2.size());
    return s3.find('>') == std::string::npos ? s3 : s2;
  }();
  return s;
}

#else
template <typename T>
std::string get_type_name()
{
  return "anonymous";
}
#endif

//! This version leverages argument type deduction for shorter syntax. (e.g. get_type_name(var))
template <typename T>
std::string get_type_name(const T& t)
{
  return get_type_name<T>();
}

/************************************
 *        type_list<Args...>
 ***********************************/

constexpr size_t invalid_type_index = std::numeric_limits<size_t>::max();

template <typename... Args>
struct type_list {};

template <typename... Args>
constexpr size_t type_list_size(type_list<Args...> t)
{
  return sizeof...(Args);
}

/************************************
 *   get_index_type/get_type_index
 ***********************************/

namespace type_utils {

//! Get Index of a type in a list of types (in reversed order)
template <typename T, typename... Types>
struct type_list_reverse_index;

template <typename T, typename First, typename... Types>
struct type_list_reverse_index<T, First, Types...> {
  static constexpr size_t index =
      std::is_same<T, First>::value ? sizeof...(Types) : type_list_reverse_index<T, Types...>::index;
};

template <typename T>
struct type_list_reverse_index<T> {
  static constexpr size_t index = invalid_type_index;
};

//! Get a type of an index in a list of types
template <std::size_t I, typename... Types>
struct get_type_reverse;

template <std::size_t I, typename First, typename... Types>
struct get_type_reverse<I, First, Types...> {
  using type =
      typename std::conditional<I == sizeof...(Types), First, typename get_type_reverse<I, Types...>::type>::type;
};

template <std::size_t I>
struct get_type_reverse<I> {
  using type = void;
};

} // namespace type_utils

//! Get index of T in Types...
template <typename T, typename... Types>
constexpr size_t get_type_index()
{
  using namespace type_utils;
  return (type_list_reverse_index<T, Types...>::index == invalid_type_index)
             ? invalid_type_index
             : sizeof...(Types) - type_list_reverse_index<T, Types...>::index - 1;
}

//! If T is in Types...
template <typename T, typename... Types>
constexpr bool type_list_contains()
{
  return get_type_index<T, Types...>() != invalid_type_index;
}

//! Get type of index I in Types...
template <size_t I, typename... Types>
struct get_index_type {
  using type = typename type_utils::get_type_reverse<sizeof...(Types) - I - 1, Types...>::type;
};

/**************************
 *     Filter utils
 *************************/

namespace type_utils {

//! pushes type to front of type_list
template <class...>
struct push_front;
template <class T, class... Types>
struct push_front<T, type_list<Types...> > {
  using type = type_list<T, Types...>;
};
template <>
struct push_front<> {
  using type = type_list<>;
};

//! Creates a type_list with the "Types..." for which Predicate<Type>::value is true
template <template <typename> class Predicate, class...>
struct filter;
template <template <typename> class Predicate, class First, class... Types>
struct filter<Predicate, First, Types...> {
  using type = typename std::conditional<Predicate<First>::value,
                                         typename push_front<First, typename filter<Predicate, Types...>::type>::type,
                                         typename filter<Predicate, Types...>::type>::type;
};
template <template <typename> class Predicate>
struct filter<Predicate> {
  using type = type_list<>;
};

} // namespace type_utils

/************************************
 *          static_visit
 ***********************************/

namespace type_utils {
template <typename F, typename... Types>
struct static_visit_impl;

template <typename F, typename First, typename... Types>
struct static_visit_impl<F, First, Types...> {
  static void apply(size_t idx, F&& f)
  {
    if (idx == sizeof...(Types)) {
      f.template operator()<First>();
    } else {
      static_visit_impl<F, Types...>::apply(idx, std::forward<F>(f));
    }
  }
};

template <typename F, typename First>
struct static_visit_impl<F, First> {
  static void apply(size_t idx, F&& f) { f.template operator()<First>(); }
};

/**
 * @brief visit pattern implementation
 * @tparam F functor
 * @tparam V tagged union type
 * @tparam Types remaining types to iterate
 */
template <typename F, typename TypeList, typename... Types>
struct visit_impl;

template <typename F, typename TypeList, typename First, typename... Types>
struct visit_impl<F, TypeList, First, Types...> {
  static void apply(TypeList& c, F&& f)
  {
    if (c.template is<First>()) {
      f(c.template get_unchecked<First>());
    } else {
      visit_impl<F, TypeList, Types...>::apply(c, std::forward<F>(f));
    }
  }
  static void apply(const TypeList& c, F&& f)
  {
    if (c.template is<First>()) {
      f(c.template get_unchecked<First>());
    } else {
      visit_impl<F, TypeList, Types...>::apply(c, std::forward<F>(f));
    }
  }
};
template <typename F, typename TypeList, typename First>
struct visit_impl<F, TypeList, First> {
  static void apply(TypeList& c, F&& f) { f(c.template get_unchecked<First>()); }
  static void apply(const TypeList& c, F&& f) { f(c.template get_unchecked<First>()); }
};

/**************************
 *    Invocable utils
 *************************/

template <class F, class... Args>
using invoke_result_t = typename std::result_of<F && (Args && ...)>::type;

} // namespace type_utils

//! srslte::get<Type> access methods
template <typename T, typename TypeContainer>
T* get_if(TypeContainer& c)
{
  return (c.template is<T>()) ? &c.template get_unchecked<T>() : nullptr;
}

template <typename T, typename TypeContainer>
const T* get_if(const TypeContainer& c)
{
  return (c.template is<T>()) ? &c.template get_unchecked<T>() : nullptr;
}

template <typename T, typename TypeContainer>
T& get(TypeContainer& c)
{
  if (c.template is<T>()) {
    return c.template get_unchecked<T>();
  }
  THROW_BAD_ACCESS("in get<T>");
}

template <typename T, typename TypeContainer>
const T& get(const TypeContainer& c)
{
  if (c.template is<T>()) {
    return c.template get_unchecked<T>();
  }
  THROW_BAD_ACCESS("in get<T>");
}

template <size_t I,
          template <typename...> class TypeContainer,
          typename... Args,
          typename T = typename get_index_type<I, Args...>::type>
T& get(TypeContainer<Args...>& c)
{
  return get<T>(c);
}

template <size_t I,
          template <typename...> class TypeContainer,
          typename... Args,
          typename T = typename get_index_type<I, Args...>::type>
const T& get(const TypeContainer<Args...>& c)
{
  return get<T>(c);
}

//! Function to static visit a template type that holds many variadic args
template <typename Visitor, typename... Args>
void static_visit(Visitor&& f, size_t current_idx)
{
  type_utils::static_visit_impl<Visitor, Args...>::apply(sizeof...(Args) - current_idx - 1, std::forward<Visitor>(f));
}
template <typename Visitor, template <typename...> class TypeSet, typename... Args>
void static_visit(Visitor&& f, const TypeSet<Args...>& tset)
{
  static_visit<Visitor, Args...>(std::forward<Visitor>(f), tset.get_type_idx());
}

//! Function to visit a template type that holds many variadic args
template <typename Visitor, template <typename...> class TypeSet, typename... Args>
void visit(Visitor&& f, TypeSet<Args...>& tset)
{
  using namespace type_utils;
  visit_impl<Visitor, TypeSet<Args...>, Args...>::apply(tset, std::forward<Visitor>(f));
}
template <typename Visitor, template <typename...> class TypeSet, typename... Args>
void visit(Visitor&& f, const TypeSet<Args...>& tset)
{
  using namespace type_utils;
  visit_impl<Visitor, TypeSet<Args...>, Args...>::apply(tset, std::forward<Visitor>(f));
}

} // namespace srslte

#endif // SRSLTE_TYPE_UTILS_H
