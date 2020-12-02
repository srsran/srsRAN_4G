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

#ifndef SRSLTE_ACCUMULATORS_H
#define SRSLTE_ACCUMULATORS_H

#include <cassert>
#include <cstdint>
#include <limits>
#include <vector>

namespace srslte {

template <typename T>
struct rolling_average {
  void push(T sample)
  {
    avg_ += (sample - avg_) / (count_ + 1);
    ++count_;
  }
  T        value() const { return count_ == 0 ? 0 : avg_; }
  uint32_t count() const { return count_; }

private:
  T        avg_   = 0;
  uint32_t count_ = 0;
};

template <typename T>
struct exp_average_fast_start {
  exp_average_fast_start(T alpha_, uint32_t start_size = 100) : alpha(alpha_), start_count_size(start_size)
  {
    assert(start_size > 0);
  }
  void push(T sample)
  {
    if (count < start_count_size) {
      avg_ += (sample - avg_) / (count + 1);
      count++;
    } else {
      avg_ = (1 - alpha) * avg_ + alpha * sample;
    }
  }
  T value() const { return count == 0 ? 0 : avg_; }

private:
  T        avg_  = 0;
  uint32_t count = 0;
  uint32_t start_count_size;
  T        alpha;
};

namespace detail {

template <typename T>
struct sliding_window {
  sliding_window(uint32_t N, T val = 0) : window(N, val) {}
  void push(T sample)
  {
    window[next_idx++] = sample;
    if (next_idx >= window.size()) {
      next_idx -= window.size();
    }
  }
  size_t         size() const { return window.size(); }
  const T&       oldest() const { return window[(next_idx + size() - 1) % size()]; }
  T&             operator[](size_t i) { return window[i]; }
  const T&       operator[](size_t i) const { return window[i]; }
  std::vector<T> window;
  size_t         next_idx = 0;
};

} // namespace detail

template <typename T>
struct sliding_sum : private detail::sliding_window<T> {
  using base_t = detail::sliding_window<T>;
  using base_t::oldest;
  using base_t::push;
  using base_t::size;
  using base_t::sliding_window;

  T value() const
  {
    T ret = 0;
    for (size_t i = 0; i < size(); ++i) {
      ret += (*this)[i];
    }
    return ret;
  }
};

template <typename T>
struct sliding_average {
  sliding_average(uint32_t N) : window(N, 0) {}
  void push(T sample) { window.push(sample); }
  T    value() const { return window.value() / window.size(); }

private:
  sliding_sum<T> window;
};

template <typename T>
struct null_sliding_average {
  static constexpr T null_value = std::numeric_limits<T>::max();

  null_sliding_average(uint32_t N) : window(N, null_value) {}
  void push(T sample) { window.push(sample); }
  void push_hole() { window.push(null_value); }
  T    value() const
  {
    T        ret   = 0;
    uint32_t count = 0;
    for (size_t i = 0; i < window.size(); ++i) {
      if (window[i] != null_value) {
        ret += window[i];
        count++;
      }
    }
    return (count == 0) ? null_value : ret / count;
  }

private:
  detail::sliding_window<T> window;
};

} // namespace srslte

#endif // SRSLTE_ACCUMULATORS_H
