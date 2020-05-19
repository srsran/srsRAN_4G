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

#ifndef SRSLTE_STRING_HELPERS_H
#define SRSLTE_STRING_HELPERS_H

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace srslte {

/**
 * Removes all matching characters from a string
 * @param input input string
 * @param c character to match
 * @return It returns the resultant string without the matched characters
 */
static inline std::string string_remove_char(const std::string& input, char c)
{
  std::string ret = input;
  std::size_t p1  = ret.find(c);

  while (p1 != std::string::npos) {
    ret.erase(p1);
    p1 = ret.find(c);
  }

  return ret;
}

/**
 * Private casting overloaded functions
 */
namespace {
#define INTEGER_CAST(TYPE)                                                                                             \
  inline void string_cast(const std::string& str, TYPE& v) { v = (TYPE)std::strtol(str.c_str(), nullptr, 10); }

INTEGER_CAST(int32_t)
INTEGER_CAST(uint32_t)
INTEGER_CAST(int16_t)
INTEGER_CAST(uint16_t)
INTEGER_CAST(int8_t)
INTEGER_CAST(uint8_t)

#undef INTEGER_CAST

inline void string_cast(const std::string& str, float& f)
{
  f = std::strtof(str.c_str(), nullptr);
}
inline void string_cast(const std::string& str, double& d)
{
  d = std::strtod(str.c_str(), nullptr);
}

inline void string_cast(const std::string& str, std::string& str2)
{
  str2 = str;
}
} // namespace

/**
 * This function parses a string into any defined type
 * @tparam Target type
 * @param str input string
 * @return Parsed value of the given type
 */
template <class Type>
static inline Type string_cast(const std::string& str)
{
  Type ret;

  string_cast(str, ret);

  return ret;
}

/**
 * It splits a given string into multiple elements given a delimiter. The elements are casted to the specified type.
 * @tparam Insertable It is the list data-type. It needs to implement insert(iterator, element)
 * @param input It is the input string
 * @param delimiter Character used for indicating the end of the strings
 * @param list contains the parsed values
 */
template <class Insertable>
static inline void string_parse_list(const std::string& input, char delimiter, Insertable& list)
{
  std::stringstream ss(input);

  // Removes all possible elements of the list
  list.clear();

  while (ss.good()) {
    std::string substr;
    std::getline(ss, substr, delimiter);

    if (not substr.empty()) {
      list.insert(list.end(), string_cast<typename Insertable::value_type>(substr));
    }
  }
}

} // namespace srslte

#endif // SRSLTE_STRING_HELPERS_H
