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

#ifndef SRSRAN_TTI_POINT_H
#define SRSRAN_TTI_POINT_H

#include "srsran/adt/interval.h"
#include "srsran/common/common.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/srsran_assert.h"
#include <cstdint>
#include <limits>

namespace srsran {

struct tti_point {
  constexpr tti_point() = default;
  explicit tti_point(uint32_t tti_val_) : tti_val(tti_val_ % 10240U)
  {
    if (tti_val_ > std::numeric_limits<uint32_t>::max() / 2) {
      // there was a overflow at tti initialization
      uint32_t diff = std::numeric_limits<uint32_t>::max() - tti_val_;
      srsran_expect(diff < 10240, "Invalid TTI point assigned");
      tti_val = 10240 - diff - 1;
    }
  }
  void     reset() { tti_val = std::numeric_limits<uint32_t>::max(); }
  uint32_t sf_idx() const { return tti_val % 10; }
  uint32_t sfn() const { return tti_val / 10; }

  // comparison operators
  bool operator==(const tti_point& other) const { return tti_val == other.tti_val; }
  bool operator!=(const tti_point& other) const { return tti_val != other.tti_val; }
  bool operator<(const tti_point& other) const
  {
    bool lower_val = tti_val < other.tti_val;
    bool wrap_flag = (lower_val ? other.tti_val - tti_val : tti_val - other.tti_val) > 10240 / 2;
    return (lower_val != wrap_flag);
  }
  bool operator<=(const tti_point& other) const { return *this == other or *this < other; }
  bool operator>(const tti_point& other) const { return not(*this <= other); }
  bool operator>=(const tti_point& other) const { return not(*this < other); }

  // add/subtract
  int operator-(const tti_point& other) const
  {
    int diff = static_cast<int>(tti_val) - static_cast<int>(other.tti_val);
    if (diff > 10240 / 2) {
      return diff - 10240;
    }
    if (diff < -10240 / 2) {
      return diff + 10240;
    }
    return diff;
  }
  tti_point& operator+=(uint32_t jump)
  {
    tti_val = (tti_val + jump) % 10240;
    return *this;
  }
  tti_point& operator-=(uint32_t jump)
  {
    tti_val -= jump - ((tti_val < jump) ? 10240 : 0);
    return *this;
  }
  tti_point& operator++() { return this->operator+=(1); }
  tti_point  operator++(int)
  {
    tti_point ret{tti_val};
    this->    operator+=(1);
    return ret;
  }
  bool     is_valid() const { return tti_val < 10240; }
  explicit operator uint32_t() const { return tti_val; }
  uint32_t to_uint() const { return tti_val; }
  bool     is_in_interval(tti_point l, tti_point u) const { return (*this >= l and *this <= u); }

private:
  uint32_t tti_val = std::numeric_limits<uint32_t>::max();
};
inline tti_point operator+(tti_point tti, uint32_t jump)
{
  return tti_point{(uint32_t)tti + jump};
}
inline tti_point operator+(uint32_t jump, tti_point tti)
{
  return tti_point{(uint32_t)tti + jump};
}
inline tti_point operator-(tti_point tti, uint32_t jump)
{
  return tti_point{(uint32_t)tti + 10240 - jump};
}
inline tti_point max(tti_point tti1, tti_point tti2)
{
  return tti1 > tti2 ? tti1 : tti2;
}
inline tti_point min(tti_point tti1, tti_point tti2)
{
  return tti1 < tti2 ? tti1 : tti2;
}

using tti_interval = srsran::interval<srsran::tti_point>;

} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::tti_point> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(srsran::tti_point tti, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", tti.to_uint());
  }
};
} // namespace fmt

namespace srsenb {

using tti_point    = srsran::tti_point;
using tti_interval = srsran::tti_interval;

inline tti_point to_tx_dl(tti_point t)
{
  return t + TX_ENB_DELAY;
}
inline tti_point to_tx_ul(tti_point t)
{
  return t + (TX_ENB_DELAY + FDD_HARQ_DELAY_DL_MS);
}
inline tti_point to_tx_dl_ack(tti_point t)
{
  return to_tx_ul(t);
}
inline tti_point to_tx_ul_ack(tti_point t)
{
  return to_tx_ul(t) + TX_ENB_DELAY;
}

} // namespace srsenb

#endif // SRSRAN_TTI_POINT_H
