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

#ifndef SRSLTE_DYN_BITSET_H
#define SRSLTE_DYN_BITSET_H

#include "srslte/common/logmap.h"
#include <cstdint>
#include <inttypes.h>
#include <string>

namespace srslte {

constexpr uint32_t ceil_div(uint32_t x, uint32_t y)
{
  return (x > 0) ? (x - 1) / y + 1 : 0;
}

template <size_t N, bool reversed = false>
class bounded_bitset
{
  typedef uint64_t    word_t;
  static const size_t bits_per_word = 8 * sizeof(word_t);

public:
  constexpr bounded_bitset() : buffer(), cur_size(0) {}

  constexpr explicit bounded_bitset(size_t cur_size_) : buffer(), cur_size(cur_size_) {}

  constexpr size_t max_size() const noexcept { return N; }

  size_t size() const noexcept { return cur_size; }

  void resize(size_t new_size) noexcept
  {
    if (new_size > max_size()) {
      srslte::logmap::get("COMM")->error("ERROR: bitset resize out of bounds: %zd>=%zd\n", max_size(), new_size);
      return;
    } else if (new_size == cur_size) {
      return;
    }
    cur_size = new_size;
    sanitize_();
    for (size_t i = nof_words_(); i < max_nof_words_(); ++i) {
      get_word_(i) = static_cast<word_t>(0);
    }
  }

  void set(size_t pos, bool val) noexcept
  {
    if (pos >= size()) {
      srslte::logmap::get("COMM")->error("ERROR: bitset out of bounds: %zd>=%zd\n", pos, size());
      return;
    }
    if (val) {
      set_(pos);
    } else {
      reset_(pos);
    }
  }

  void set(size_t pos) noexcept
  {
    if (pos >= size()) {
      srslte::logmap::get("COMM")->error("ERROR: bitset out of bounds: %zd>=%zd\n", pos, size());
      return;
    }
    set_(pos);
  }

  void reset(size_t pos) noexcept
  {
    if (pos >= size()) {
      srslte::logmap::get("COMM")->error("ERROR: bitset out of bounds: %zd>=%zd\n", pos, size());
      return;
    }
    reset_(pos);
  }

  void reset() noexcept
  {
    for (size_t i = 0; i < nof_words_(); ++i) {
      buffer[i] = static_cast<word_t>(0);
    }
  }

  bool test(size_t pos) const noexcept
  {
    if (pos >= size()) {
      srslte::logmap::get("COMM")->error("ERROR: bitset out of bounds: %zd>=%zd\n", pos, size());
      return false;
    }
    return test_(pos);
  }

  bounded_bitset<N, reversed>& flip() noexcept
  {
    for (size_t i = 0; i < nof_words_(); ++i) {
      buffer[i] = ~buffer[i];
    }
    sanitize_();
    return *this;
  }

  bounded_bitset<N, reversed>& fill(size_t startpos, size_t endpos, bool value = true) noexcept
  {
    if (endpos > size() or startpos > endpos) {
      srslte::logmap::get("COMM")->error(
          "ERROR: bounds (%zd, %zd) are not valid for bitset of size: %zd\n", startpos, endpos, size());
      return *this;
    }
    // NOTE: can be optimized
    if (value) {
      for (size_t i = startpos; i < endpos; ++i) {
        set_(i);
      }
    } else {
      for (size_t i = startpos; i < endpos; ++i) {
        reset_(i);
      }
    }
    return *this;
  }

  bool all() const noexcept
  {
    const size_t nw = nof_words_();
    if (nw == 0) {
      return true;
    }
    word_t allset = ~static_cast<word_t>(0);
    for (size_t i = 0; i < nw - 1; i++) {
      if (buffer[i] != allset) {
        return false;
      }
    }
    return buffer[nw - 1] == (allset >> (nw * bits_per_word - size()));
  }

  bool any() const noexcept
  {
    for (size_t i = 0; i < nof_words_(); ++i) {
      if (buffer[i] != static_cast<word_t>(0)) {
        return true;
      }
    }
    return false;
  }

  bool any(size_t start, size_t stop) const noexcept
  {
    if (start > stop or stop > size()) {
      srslte::logmap::get("COMM")->error(
          "ERROR: bounds (%zd, %zd) are not valid for bitset of size: %zd\n", start, stop, size());
      return false;
    }
    // NOTE: can be optimized
    for (size_t i = start; i < stop; ++i) {
      if (test_(i)) {
        return true;
      }
    }
    return false;
  }

  bool none() const noexcept { return !any(); }

  size_t count() const noexcept
  {
    size_t result = 0;
    for (size_t i = 0; i < nof_words_(); i++) {
      //      result += __builtin_popcountl(buffer[i]);
      word_t w = buffer[i];
      for (; w; w >>= 1u) {
        result += (w & 1u);
      }
    }
    return result;
  }

  bool operator==(const bounded_bitset<N, reversed>& other) const noexcept
  {
    if (size() != other.size()) {
      return false;
    }
    for (uint32_t i = 0; i < nof_words_(); ++i) {
      if (buffer[i] != other.buffer[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const bounded_bitset<N, reversed>& other) const noexcept { return not(*this == other); }

  bounded_bitset<N, reversed>& operator|=(const bounded_bitset<N, reversed>& other) noexcept
  {
    if (other.size() != size()) {
      srslte::logmap::get("COMM")->error(
          "ERROR: operator|= called for bitsets of different sizes (%zd!=%zd)\n", size(), other.size());
      return *this;
    }
    for (size_t i = 0; i < nof_words_(); ++i) {
      buffer[i] |= other.buffer[i];
    }
    return *this;
  }

  bounded_bitset<N, reversed>& operator&=(const bounded_bitset<N, reversed>& other) noexcept
  {
    if (other.size() != size()) {
      srslte::logmap::get("COMM")->error(
          "ERROR: operator&= called for bitsets of different sizes (%zd!=%zd)\n", size(), other.size());
      return *this;
    }
    for (size_t i = 0; i < nof_words_(); ++i) {
      buffer[i] &= other.buffer[i];
    }
    return *this;
  }

  bounded_bitset<N, reversed> operator~() const noexcept
  {
    bounded_bitset<N, reversed> ret(*this);
    ret.flip();
    return ret;
  }

  std::string to_string() const noexcept
  {
    std::string s;
    s.assign(size(), '0');
    if (not reversed) {
      for (size_t i = size(); i > 0; --i) {
        if (test(i - 1)) {
          s[size() - i] = '1';
        }
      }
    } else {
      for (size_t i = 0; i < size(); ++i) {
        if (test(i)) {
          s[i] = '1';
        }
      }
    }
    return s;
  }

  uint64_t to_uint64() const noexcept
  {
    if (nof_words_() > 1) {
      srslte::logmap::get("COMM")->error("ERROR: cannot convert bitset of size %zd bits to uint64_t\n", size());
      return 0;
    }
    return get_word_(0);
  }

  std::string to_hex() const noexcept
  {
    size_t nof_digits = (size() - 1) / 4 + 1;
    char   cstr[ceil_div(ceil_div(N, bits_per_word) * bits_per_word, 4) + 1];
    size_t count = 0;

    for (int i = nof_words_() - 1; i >= 0; --i) {
      count += sprintf(&cstr[count], "%016" PRIx64, buffer[i]);
    }

    size_t skip = nof_words_() * bits_per_word / 4 - nof_digits;
    //    printf("bitstring: %s\n", to_string().c_str());
    return std::string(&cstr[skip], &cstr[nof_digits + skip + 1]);
  }

private:
  word_t buffer[(N - 1) / bits_per_word + 1];
  size_t cur_size;

  void sanitize_()
  {
    size_t n      = size() % bits_per_word;
    size_t nwords = nof_words_();
    if (n != 0 and nwords > 0) {
      buffer[nwords - 1] &= ~((~static_cast<word_t>(0)) << n);
    }
  }

  bool test_(size_t pos) const noexcept
  {
    pos = reversed ? size() - 1 - pos : pos;
    return ((get_word_(pos) & maskbit(pos)) != static_cast<word_t>(0));
  }

  void set_(size_t pos) noexcept
  {
    pos = reversed ? size() - 1 - pos : pos;
    get_word_(pos) |= maskbit(pos);
  }

  void reset_(size_t pos) noexcept
  {
    pos = reversed ? size() - 1 - pos : pos;
    get_word_(pos) &= ~(maskbit(pos));
  }

  size_t nof_words_() const noexcept { return size() > 0 ? (size() - 1) / bits_per_word + 1 : 0; }

  word_t& get_word_(size_t pos) noexcept { return buffer[pos / bits_per_word]; }

  const word_t& get_word_(size_t pos) const { return buffer[pos / bits_per_word]; }

  size_t word_idx_(size_t pos) const { return pos / bits_per_word; }

  static word_t maskbit(size_t pos) { return (static_cast<word_t>(1)) << (pos % bits_per_word); }

  static size_t max_nof_words_() { return (N - 1) / bits_per_word + 1; }
};

template <size_t N, bool reversed>
inline bounded_bitset<N, reversed> operator&(const bounded_bitset<N, reversed>& lhs,
                                             const bounded_bitset<N, reversed>& rhs)noexcept
{
  bounded_bitset<N, reversed> res(lhs);
  res &= rhs;
  return res;
}

template <size_t N, bool reversed>
inline bounded_bitset<N, reversed> operator|(const bounded_bitset<N, reversed>& lhs,
                                             const bounded_bitset<N, reversed>& rhs) noexcept
{
  bounded_bitset<N, reversed> res(lhs);
  res |= rhs;
  return res;
}

template <size_t N, bool reversed>
inline bounded_bitset<N, reversed> fliplr(const bounded_bitset<N, reversed>& other) noexcept
{
  bounded_bitset<N, reversed> ret(other.size());
  for (uint32_t i = 0; i < ret.size(); ++i) {
    if (other.test(i)) {
      ret.set(ret.size() - 1 - i);
    }
  }
  return ret;
}

} // namespace srslte

#endif // SRSLTE_DYN_BITSET_H
