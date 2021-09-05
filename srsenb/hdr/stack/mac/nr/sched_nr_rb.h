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

#ifndef SRSRAN_SCHED_NR_RB_H
#define SRSRAN_SCHED_NR_RB_H

#include "srsenb/hdr/stack/mac/nr/sched_nr_interface.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/phy/common/phy_common_nr.h"

namespace srsenb {
namespace sched_nr_impl {

using prb_bitmap = srsran::bounded_bitset<SRSRAN_MAX_PRB_NR, true>;
using rbg_bitmap = srsran::bounded_bitset<SCHED_NR_MAX_NOF_RBGS, true>;

/// TS 38.214, Table 6.1.2.2.1-1 - Nominal RBG size P
uint32_t get_P(uint32_t bwp_nof_prb, bool config_1_or_2);

/// TS 38.214 - total number of RBGs for a uplink bandwidth part of size "bwp_nof_prb" PRBs
uint32_t get_nof_rbgs(uint32_t bwp_nof_prb, uint32_t bwp_start, bool config1_or_2);

/// Struct to express a {min,...,max} range of PRBs
struct prb_interval : public srsran::interval<uint32_t> {
  using interval::interval;
};

struct prb_grant {
  prb_grant() = default;
  prb_grant(const prb_interval& other) noexcept : alloc_type_0(false), alloc(other) {}
  prb_grant(const rbg_bitmap& other) noexcept : alloc_type_0(true), alloc(other) {}
  prb_grant(const prb_grant& other) noexcept : alloc_type_0(other.alloc_type_0), alloc(other.alloc_type_0, other.alloc)
  {}
  prb_grant& operator=(const prb_grant& other) noexcept
  {
    if (this == &other) {
      return *this;
    }
    if (other.alloc_type_0) {
      *this = other.rbgs();
    } else {
      *this = other.prbs();
    }
    return *this;
  }
  prb_grant& operator=(const prb_interval& prbs)
  {
    if (alloc_type_0) {
      alloc_type_0 = false;
      alloc.rbgs.~rbg_bitmap();
      new (&alloc.interv) prb_interval(prbs);
    } else {
      alloc.interv = prbs;
    }
    return *this;
  }
  prb_grant& operator=(const rbg_bitmap& rbgs)
  {
    if (alloc_type_0) {
      alloc.rbgs = rbgs;
    } else {
      alloc_type_0 = true;
      alloc.interv.~prb_interval();
      new (&alloc.rbgs) rbg_bitmap(rbgs);
    }
    return *this;
  }
  ~prb_grant()
  {
    if (is_alloc_type0()) {
      alloc.rbgs.~rbg_bitmap();
    } else {
      alloc.interv.~prb_interval();
    }
  }

  bool              is_alloc_type0() const { return alloc_type_0; }
  bool              is_alloc_type1() const { return not is_alloc_type0(); }
  const rbg_bitmap& rbgs() const
  {
    srsran_assert(is_alloc_type0(), "Invalid access to rbgs() field of grant with alloc type 1");
    return alloc.rbgs;
  }
  const prb_interval& prbs() const
  {
    srsran_assert(is_alloc_type1(), "Invalid access to prbs() field of grant with alloc type 0");
    return alloc.interv;
  }
  rbg_bitmap& rbgs()
  {
    srsran_assert(is_alloc_type0(), "Invalid access to rbgs() field of grant with alloc type 1");
    return alloc.rbgs;
  }
  prb_interval& prbs()
  {
    srsran_assert(is_alloc_type1(), "Invalid access to prbs() field of grant with alloc type 0");
    return alloc.interv;
  }

private:
  bool alloc_type_0 = false;
  union alloc_t {
    rbg_bitmap   rbgs;
    prb_interval interv;

    alloc_t() : interv(0, 0) {}
    explicit alloc_t(const prb_interval& prbs) : interv(prbs) {}
    explicit alloc_t(const rbg_bitmap& rbgs_) : rbgs(rbgs_) {}
    alloc_t(bool type0, const alloc_t& other)
    {
      if (type0) {
        new (&rbgs) rbg_bitmap(other.rbgs);
      } else {
        new (&interv) prb_interval(other.interv);
      }
    }
  } alloc;
};

struct bwp_rb_bitmap {
public:
  bwp_rb_bitmap() = default;
  bwp_rb_bitmap(uint32_t bwp_nof_prbs, uint32_t bwp_prb_start_, bool config1_or_2);

  void reset()
  {
    prbs_.reset();
    rbgs_.reset();
  }

  template <typename T>
  void operator|=(const T& grant)
  {
    add(grant);
  }

  void add(const prb_interval& prbs)
  {
    prbs_.fill(prbs.start(), prbs.stop());
    add_prbs_to_rbgs(prbs);
  }
  void add(const prb_bitmap& grant)
  {
    prbs_ |= grant;
    add_prbs_to_rbgs(grant);
  }
  void add(const rbg_bitmap& grant)
  {
    rbgs_ |= grant;
    add_rbgs_to_prbs(grant);
  }
  void add(const prb_grant& grant)
  {
    if (grant.is_alloc_type0()) {
      add(grant.rbgs());
    } else {
      add(grant.prbs());
    }
  }
  bool collides(const prb_grant& grant) const
  {
    if (grant.is_alloc_type0()) {
      return (rbgs() & grant.rbgs()).any();
    }
    return prbs().any(grant.prbs().start(), grant.prbs().stop());
  }
  bool test(uint32_t prb_idx) { return prbs().test(prb_idx); }
  void set(uint32_t prb_idx)
  {
    prbs_.set(prb_idx);
    rbgs_.set(prb_to_rbg_idx(prb_idx));
  }

  const prb_bitmap& prbs() const { return prbs_; }
  const rbg_bitmap& rbgs() const { return rbgs_; }
  uint32_t          P() const { return P_; }
  uint32_t          nof_prbs() const { return prbs_.size(); }
  uint32_t          nof_rbgs() const { return rbgs_.size(); }

  uint32_t prb_to_rbg_idx(uint32_t prb_idx) const;

private:
  prb_bitmap prbs_;
  rbg_bitmap rbgs_;
  uint32_t   P_             = 0;
  uint32_t   Pnofbits       = 0;
  uint32_t   first_rbg_size = 0;

  void add_prbs_to_rbgs(const prb_bitmap& grant);
  void add_prbs_to_rbgs(const prb_interval& grant);
  void add_rbgs_to_prbs(const rbg_bitmap& grant);
};

inline prb_interval
find_next_empty_interval(const prb_bitmap& mask, size_t start_prb_idx = 0, size_t last_prb_idx = SRSRAN_MAX_PRB_NR)
{
  int rb_start = mask.find_lowest(start_prb_idx, std::min(mask.size(), last_prb_idx), false);
  if (rb_start != -1) {
    int rb_end = mask.find_lowest(rb_start + 1, std::min(mask.size(), last_prb_idx), true);
    return {(uint32_t)rb_start, (uint32_t)(rb_end < 0 ? mask.size() : rb_end)};
  }
  return {};
}

inline prb_interval find_empty_interval_of_length(const prb_bitmap& mask, size_t nof_prbs, uint32_t start_prb_idx = 0)
{
  prb_interval max_interv;
  do {
    prb_interval interv = find_next_empty_interval(mask, start_prb_idx, mask.size());
    if (interv.empty()) {
      break;
    }
    if (interv.length() >= nof_prbs) {
      max_interv.set(interv.start(), interv.start() + nof_prbs);
      break;
    }
    if (interv.length() > max_interv.length()) {
      max_interv = interv;
    }
    start_prb_idx = interv.stop() + 1;
  } while (start_prb_idx < mask.size());
  return max_interv;
}

} // namespace sched_nr_impl
} // namespace srsenb

namespace fmt {

template <>
struct formatter<srsenb::sched_nr_impl::prb_grant> : public formatter<srsenb::sched_nr_impl::rbg_bitmap> {
  template <typename FormatContext>
  auto format(const srsenb::sched_nr_impl::prb_grant& grant, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (grant.is_alloc_type1()) {
      return formatter<srsran::interval<uint32_t> >{}.format(grant.prbs(), ctx);
    }
    return formatter<srsenb::sched_nr_impl::rbg_bitmap>::format(grant.rbgs(), ctx);
  }
};

} // namespace fmt

#endif // SRSRAN_SCHED_NR_RB_H
