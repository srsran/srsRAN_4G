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

#ifndef SRSLTE_STRING_HELPERS_H
#define SRSLTE_STRING_HELPERS_H

#include "srslte/srslog/bundled/fmt/format.h"
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

template <size_t N>
const char* to_c_str(fmt::basic_memory_buffer<char, N>& mem_buffer)
{
  fmt::format_to(mem_buffer, "{}", '\0');
  return mem_buffer.data();
}

} // namespace srslte

#endif // SRSLTE_STRING_HELPERS_H
