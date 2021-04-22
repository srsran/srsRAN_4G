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

#ifndef SRSLOG_DETAIL_SUPPORT_TMP_UTILS_H
#define SRSLOG_DETAIL_SUPPORT_TMP_UTILS_H

#include <cstddef>
#include <tuple>

namespace srslog {

namespace detail {

///
/// Implementation of the std::index_sequence C++14 library utility.
///

template <std::size_t...>
struct index_sequence {};

template <std::size_t N, std::size_t... Next>
struct index_sequence_helper : public index_sequence_helper<N - 1U, N - 1U, Next...> {};

template <std::size_t... Next>
struct index_sequence_helper<0U, Next...> {
  using type = index_sequence<Next...>;
};

template <std::size_t N>
using make_index_sequence = typename index_sequence_helper<N>::type;

///
/// Implementation of the std::get<T> C++14 library utility.
///

template <typename T, typename Tuple>
struct tuple_index;

template <typename T, typename... Ts>
struct tuple_index<T, std::tuple<T, Ts...> > {
  static constexpr std::size_t value = 0;
};

template <typename T, typename U, typename... Ts>
struct tuple_index<T, std::tuple<U, Ts...> > {
  static constexpr std::size_t value = 1 + tuple_index<T, std::tuple<Ts...> >::value;
};

template <typename T, typename... Ts>
constexpr std::size_t get_type_index_in_tuple()
{
  return tuple_index<T, std::tuple<Ts...> >::value;
}

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_SUPPORT_TMP_UTILS_H
