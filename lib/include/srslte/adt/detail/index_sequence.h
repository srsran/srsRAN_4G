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

#ifndef CPP_TESTS_INDEX_SEQUENCE_H
#define CPP_TESTS_INDEX_SEQUENCE_H

namespace srslte {

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

} // namespace srslte

#endif // CPP_TESTS_INDEX_SEQUENCE_H
