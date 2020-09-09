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

#ifndef SRSLTE_CIRCULAR_ARRAY_H
#define SRSLTE_CIRCULAR_ARRAY_H

#include <array>

/**
 *
 * @file circular_array.h
 *
 * @brief Helper class to safely access elements of a std::array
 *
 * Protects from out-of-bounds access by applying modulo of it's length
 * when using the [] operator for element access.
 *
 * This is useful for circular data structures, like TTIs or SNs.
 */

namespace srslte {

template <typename T, std::size_t N>
class circular_array
{
  std::array<T, N> data{};

public:
  T&       operator[](std::size_t pos) { return data[pos % N]; }
  const T& operator[](std::size_t pos) const { return data[pos % N]; }

  T*       begin() { return data.begin(); }
  const T* begin() const { return data.begin(); }

  T*       end() { return data.end(); }
  const T* end() const { return data.end(); }
};

} // namespace srslte

#endif // SRSLTE_CIRCULAR_ARRAY_H
