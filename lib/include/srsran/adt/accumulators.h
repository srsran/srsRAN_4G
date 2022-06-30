/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_ACCUMULATORS_H
#define SRSRAN_ACCUMULATORS_H

#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

namespace srsran {

template <typename T>
struct rolling_average {
  void push(T sample)
  {
    avg_ += (sample - avg_) / (count_ + 1);
    ++count_;
  }
  T        value() const { return count_ == 0 ? 0 : avg_; }
  uint32_t count() const { return count_; }
  void     reset()
  {
    avg_   = 0;
    count_ = 0;
  }

private:
  T        avg_   = 0;
  uint32_t count_ = 0;
};

template <typename T>
struct exp_average_fast_start {
  exp_average_fast_start(T alpha_val) : exp_average_fast_start(alpha_val, 1.0 / alpha_val) {}
  exp_average_fast_start(T alpha_val, uint32_t start_size) : alpha_(alpha_val), start_count_size(start_size)
  {
    assert(alpha_ < 1);
    assert(start_size > 0);
  }
  void push(T sample)
  {
    if (count < start_count_size) {
      avg_ += (sample - avg_) / (count + 1);
      count++;
    } else {
      avg_ = (1 - alpha_) * avg_ + alpha_ * sample;
    }
  }
  T    value() const { return count == 0 ? 0 : avg_; }
  T    alpha() const { return alpha_; }
  bool is_exp_average_mode() const { return count >= start_count_size; }

private:
  T        avg_  = 0;
  uint32_t count = 0;
  uint32_t start_count_size;
  T        alpha_;
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
  std::size_t    size() const { return window.size(); }
  const T&       oldest() const { return window[next_idx % size()]; }
  T&             operator[](std::size_t i) { return window[i]; }
  const T&       operator[](std::size_t i) const { return window[i]; }
  std::vector<T> window;
  std::size_t    next_idx = 0;
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
    for (std::size_t i = 0; i < size(); ++i) {
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
  null_sliding_average(uint32_t N) : window(N, null_value()) {}
  void push(T sample) { window.push(sample); }
  void push_hole() { window.push(null_value()); }
  T    value() const
  {
    T        ret   = 0;
    uint32_t count = 0;
    for (std::size_t i = 0; i < window.size(); ++i) {
      if (window[i] != null_value()) {
        ret += window[i];
        count++;
      }
    }
    return (count == 0) ? null_value() : ret / count;
  }
  static constexpr T null_value() { return std::numeric_limits<T>::max(); }

private:
  detail::sliding_window<T> window;
};

template <typename T>
struct exp_average_irreg_sampling {
  // an exp_average has the formula y_n = alpha*x + (1-alpha)*y_n-1 <=> y_n += alpha(x - y_n-1)
  // alpha can be thought as 1-exp^{-dt/T} where dt is the sample period and T is the time-constant of a LP filter
  // for variable dt, alpha[dt] = 1-exp^{-dt/T} = 1-(exp^{-1/T})^dt = 1 - (1-alpha[1])^dt
  exp_average_irreg_sampling(T alpha_, T init_val) : avg_(init_val)
  {
    assert(alpha_ < 1 and alpha_ > 0 and "Invalid alpha parameter.");
    coeff = 1 - alpha_;
  }
  void push(T sample, uint32_t sample_jump) { avg_ += (1 - pow(coeff, sample_jump)) * (sample - avg_); }
  T    value() const { return avg_; }

private:
  T avg_ = 0;
  T coeff;
};

} // namespace srsran

#endif // SRSRAN_ACCUMULATORS_H
