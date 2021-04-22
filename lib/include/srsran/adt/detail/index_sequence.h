/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef CPP_TESTS_INDEX_SEQUENCE_H
#define CPP_TESTS_INDEX_SEQUENCE_H

#include <cstddef>

namespace srsran {

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

} // namespace srsran

#endif // CPP_TESTS_INDEX_SEQUENCE_H
