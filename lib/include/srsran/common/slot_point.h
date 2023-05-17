/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_SLOT_POINT_H
#define SRSRAN_SLOT_POINT_H

#include "srsran/adt/interval.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

class slot_point
{
  uint32_t numerology_ : 3;
  uint32_t count_ : 29;

  const static uint8_t  NOF_NUMEROLOGIES        = 5;
  const static uint16_t NOF_SFNS                = 1024;
  const static uint8_t  NOF_SUBFRAMES_PER_FRAME = 10;

  uint32_t nof_slots_per_hf() const { return nof_slots_per_frame() * NOF_SFNS; }

public:
  slot_point() : numerology_(NOF_NUMEROLOGIES), count_(0) {}
  slot_point(uint8_t numerology, uint32_t count) : numerology_(numerology), count_(count)
  {
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology idx=%d passed", (int)numerology);
    srsran_assert(count < nof_slots_per_hf(), "Invalid slot count=%d passed", (int)count);
  }
  slot_point(uint8_t numerology, uint16_t sfn_val, uint8_t slot) :
    numerology_(numerology), count_(slot + sfn_val * nof_slots_per_frame())
  {
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology idx=%d passed", (int)numerology);
    srsran_assert(sfn_val < NOF_SFNS, "Invalid SFN=%d provided", (int)sfn_val);
    srsran_assert(slot < nof_slots_per_frame(),
                  "Slot index=%d exceeds maximum number of slots=%d",
                  (int)slot,
                  (int)nof_slots_per_frame());
  }

  bool    valid() const { return numerology_ < NOF_NUMEROLOGIES; }
  uint8_t nof_slots_per_subframe() const { return 1U << numerology_; }
  uint8_t nof_slots_per_frame() const { return nof_slots_per_subframe() * NOF_SUBFRAMES_PER_FRAME; }

  uint16_t sfn() const { return count_ / nof_slots_per_frame(); }
  uint16_t subframe_idx() const { return slot_idx() / nof_slots_per_subframe(); }
  uint8_t  slot_idx() const { return count_ % nof_slots_per_frame(); }
  uint8_t  numerology_idx() const { return numerology_; }
  uint32_t to_uint() const { return count_; }
  explicit operator uint32_t() const { return count_; }

  void clear() { numerology_ = NOF_NUMEROLOGIES; }

  // operators
  bool operator==(const slot_point& other) const { return other.count_ == count_ and other.numerology_ == numerology_; }
  bool operator!=(const slot_point& other) const { return not(*this == other); }
  bool operator<(const slot_point& other) const
  {
    srsran_assert(numerology_idx() == other.numerology_idx(), "Comparing slots of different numerologies");
    int a = static_cast<int>(other.count_) - static_cast<int>(count_);
    if (a > 0) {
      return (a < (int)nof_slots_per_hf() / 2);
    }
    return (a < -(int)nof_slots_per_hf() / 2);
  }
  bool operator<=(const slot_point& other) const { return (*this == other) or (*this < other); }
  bool operator>=(const slot_point& other) const { return not(*this < other); }
  bool operator>(const slot_point& other) const { return (*this != other) and *this >= other; }

  int32_t operator-(const slot_point& other) const
  {
    int a = static_cast<int>(count_) - static_cast<int>(other.count_);
    if (a >= (int)nof_slots_per_hf() / 2) {
      return a - nof_slots_per_hf();
    }
    if (a < -(int)nof_slots_per_hf() / 2) {
      return a + nof_slots_per_hf();
    }
    return a;
  }
  slot_point& operator++()
  {
    count_++;
    if (count_ == nof_slots_per_hf()) {
      count_ = 0;
    }
    return *this;
  }
  slot_point operator++(int)
  {
    slot_point ret{*this};
    this->     operator++();
    return ret;
  }
  slot_point& operator+=(uint32_t jump)
  {
    count_ = (count_ + jump) % nof_slots_per_hf();
    return *this;
  }
  slot_point& operator-=(uint32_t jump)
  {
    int a = (static_cast<int>(count_) - static_cast<int>(jump)) % static_cast<int>(nof_slots_per_hf());
    if (a < 0) {
      a += nof_slots_per_hf();
    }
    count_ = a;
    return *this;
  }

  bool is_in_interval(slot_point begin, slot_point end) const { return (*this >= begin and *this < end); }
};
inline slot_point operator+(slot_point slot, uint32_t jump)
{
  slot += jump;
  return slot;
}
inline slot_point operator+(uint32_t jump, slot_point slot)
{
  slot += jump;
  return slot;
}
inline slot_point operator-(slot_point slot, uint32_t jump)
{
  slot -= jump;
  return slot;
}
inline slot_point max(slot_point s1, slot_point s2)
{
  return s1 > s2 ? s1 : s2;
}
inline slot_point min(slot_point s1, slot_point s2)
{
  return s1 < s2 ? s1 : s2;
}

using slot_interval = srsran::interval<slot_point>;

} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::slot_point> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(srsran::slot_point slot, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", slot.to_uint());
  }
};
} // namespace fmt

namespace srsenb {

using slot_point    = srsran::slot_point;
using slot_interval = srsran::slot_interval;

} // namespace srsenb

#endif // SRSRAN_SLOT_POINT_H
