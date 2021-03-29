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

#ifndef SRSRAN_DYN_BITSET_H
#define SRSRAN_DYN_BITSET_H

#include "srsran/common/srsran_assert.h"
#include "srsran/srslog/bundled/fmt/format.h"
#include <cstdint>
#include <inttypes.h>
#include <string>

namespace srsran {

constexpr uint32_t ceil_div(uint32_t x, uint32_t y)
{
  return (x + y - 1) / y;
}

template <size_t N, bool reversed = false>
class bounded_bitset
{
  typedef uint64_t    word_t;
  static const size_t bits_per_word = 8 * sizeof(word_t);

public:
  constexpr bounded_bitset() = default;

  constexpr explicit bounded_bitset(size_t cur_size_) : cur_size(cur_size_) {}

  constexpr size_t max_size() const noexcept { return N; }

  size_t size() const noexcept { return cur_size; }

  void resize(size_t new_size)
  {
    srsran_assert(new_size <= max_size(), "ERROR: new size=%zd exceeds bitset capacity=%zd", new_size, max_size());
    if (new_size == cur_size) {
      return;
    }
    cur_size = new_size;
    sanitize_();
    for (size_t i = nof_words_(); i < max_nof_words_(); ++i) {
      buffer[i] = static_cast<word_t>(0);
    }
  }

  void set(size_t pos, bool val)
  {
    assert_within_bounds_(pos, true);
    if (val) {
      set_(pos);
    } else {
      reset_(pos);
    }
  }

  void set(size_t pos)
  {
    assert_within_bounds_(pos, true);
    set_(pos);
  }

  void reset(size_t pos)
  {
    assert_within_bounds_(pos, true);
    reset_(pos);
  }

  void reset() noexcept
  {
    for (size_t i = 0; i < nof_words_(); ++i) {
      buffer[i] = static_cast<word_t>(0);
    }
  }

  bool test(size_t pos) const
  {
    assert_within_bounds_(pos, true);
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

  bounded_bitset<N, reversed>& fill(size_t startpos, size_t endpos, bool value = true)
  {
    assert_within_bounds_(startpos, false);
    assert_within_bounds_(endpos, false);
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

  bool any(size_t start, size_t stop) const
  {
    assert_within_bounds_(start, false);
    assert_within_bounds_(stop, false);
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
      // Note: use an "int" for count triggers popcount optimization if SSE instructions are enabled.
      int c = 0;
      for (word_t w = buffer[i]; w > 0; c++) {
        w &= w - 1;
      }
      result += c;
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

  bounded_bitset<N, reversed>& operator|=(const bounded_bitset<N, reversed>& other)
  {
    srsran_assert(other.size() == size(),
                  "ERROR: operator|= called for bitsets of different sizes (%zd!=%zd)",
                  size(),
                  other.size());
    for (size_t i = 0; i < nof_words_(); ++i) {
      buffer[i] |= other.buffer[i];
    }
    return *this;
  }

  bounded_bitset<N, reversed>& operator&=(const bounded_bitset<N, reversed>& other)
  {
    srsran_assert(other.size() == size(),
                  "ERROR: operator&= called for bitsets of different sizes (%zd!=%zd)",
                  size(),
                  other.size());
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

  template <typename OutputIt>
  OutputIt to_string(OutputIt&& mem_buffer) const
  {
    if (size() == 0) {
      return mem_buffer;
    }

    std::string s;
    s.assign(size(), '0');
    if (not reversed) {
      for (size_t i = size(); i > 0; --i) {
        fmt::format_to(mem_buffer, "{}", test(i - 1) ? '1' : '0');
      }
    } else {
      for (size_t i = 0; i < size(); ++i) {
        fmt::format_to(mem_buffer, "{}", test(i) ? '1' : '0');
      }
    }
    return mem_buffer;
  }

  uint64_t to_uint64() const
  {
    srsran_assert(nof_words_() == 1, "ERROR: cannot convert bitset of size=%zd to uint64_t", size());
    return get_word_(0);
  }

  template <typename OutputIt>
  OutputIt to_hex(OutputIt&& mem_buffer) const noexcept
  {
    if (size() == 0) {
      return mem_buffer;
    }
    // first word may not print 16 hex digits
    int    i           = nof_words_() - 1;
    size_t rem_symbols = ceil_div((size() - (size() / bits_per_word) * bits_per_word), 4U);
    fmt::format_to(mem_buffer, "{:0>{}x}", buffer[i], rem_symbols);
    // remaining words will occupy 16 hex digits
    for (--i; i >= 0; --i) {
      fmt::format_to(mem_buffer, "{:0>16x}", buffer[i]);
    }
    return mem_buffer;
  }

private:
  word_t buffer[(N - 1) / bits_per_word + 1] = {0};
  size_t cur_size                            = 0;

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

  void assert_within_bounds_(size_t pos, bool strict) const
  {
    srsran_assert(pos < size() or (not strict and pos == size()),
                  "ERROR: index=%zd is out-of-bounds for bitset of size=%zd",
                  pos,
                  size());
  }

  static word_t maskbit(size_t pos) { return (static_cast<word_t>(1)) << (pos % bits_per_word); }

  static size_t max_nof_words_() { return (N - 1) / bits_per_word + 1; }
};

template <size_t N, bool reversed>
inline bounded_bitset<N, reversed> operator&(const bounded_bitset<N, reversed>& lhs,
                                             const bounded_bitset<N, reversed>& rhs) noexcept
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

} // namespace srsran

namespace fmt {
/// Custom formatter for bounded_bitset<N, reversed>
template <size_t N, bool reversed>
struct formatter<srsran::bounded_bitset<N, reversed> > {
  enum { hexadecimal, binary } mode = binary;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    auto it = ctx.begin();
    while (it != ctx.end() and *it != '}') {
      if (*it == 'x') {
        mode = hexadecimal;
      }
      ++it;
    }

    return it;
  }

  template <typename FormatContext>
  auto format(const srsran::bounded_bitset<N, reversed>& s, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (mode == hexadecimal) {
      return s.template to_hex(ctx.out());
    }
    return s.template to_string(ctx.out());
  }
};
} // namespace fmt

#endif // SRSRAN_DYN_BITSET_H
