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

#ifndef SRSRAN_SCHED_PHY_RESOURCE_H
#define SRSRAN_SCHED_PHY_RESOURCE_H

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/interval.h"
#include "srsran/support/srsran_assert.h"
extern "C" {
#include "srsran/phy/phch/ra.h"
}

// Description: This file defines the types associated with representing the allocation masks/intervals for RBGs, PRBs
//              and PDCCH CCEs, and provides some function helpers and algorithms to handle these types.

constexpr uint32_t MAX_NOF_RBGS = 25;
constexpr uint32_t MAX_NOF_PRBS = 100;
constexpr uint32_t MAX_NOF_CCES = 128;

namespace srsenb {

/// convert cell nof PRBs to nof RBGs
inline uint32_t cell_nof_prb_to_rbg(uint32_t nof_prbs)
{
  switch (nof_prbs) {
    case 6:
      return 6;
    case 15:
      return 8;
    case 25:
      return 13;
    case 50:
      return 17;
    case 75:
      return 19;
    case 100:
      return 25;
    default:
      srsran_assertion_failure("Provided nof PRBs not valid");
  }
  return 0;
}

/// convert cell nof RBGs to nof PRBs
inline uint32_t cell_nof_rbg_to_prb(uint32_t nof_rbgs)
{
  switch (nof_rbgs) {
    case 6:
      return 6;
    case 8:
      return 15;
    case 13:
      return 25;
    case 17:
      return 50;
    case 19:
      return 75;
    case 25:
      return 100;
    default:
      srsran_assertion_failure("Provided nof PRBs not valid");
  }
  return 0;
}

/// Bitmask used for CCE allocations
using pdcch_mask_t = srsran::bounded_bitset<MAX_NOF_CCES, true>;

/// Bitmask that stores the allocared DL RBGs
using rbgmask_t = srsran::bounded_bitset<MAX_NOF_RBGS, true>;

/// Bitmask that stores the allocated UL PRBs
using prbmask_t = srsran::bounded_bitset<MAX_NOF_PRBS, true>;

/// Struct to express a {min,...,max} range of RBGs
struct prb_interval;
struct rbg_interval : public srsran::interval<uint32_t> {
  using interval::interval;
  static rbg_interval find_first_interval(const rbgmask_t& mask);
  static rbg_interval prbs_to_rbgs(const prb_interval& prbs, uint32_t cell_nof_prb);
};

/// Struct to express a {min,...,max} range of PRBs
struct prb_interval : public srsran::interval<uint32_t> {
  using interval::interval;
  static prb_interval rbgs_to_prbs(const rbg_interval& rbgs, uint32_t cell_nof_prb)
  {
    uint32_t P = srsran_ra_type0_P(cell_nof_prb);
    return prb_interval{rbgs.start() * P, std::min(rbgs.stop() * P, cell_nof_prb)};
  }
  static prb_interval riv_to_prbs(uint32_t riv, uint32_t nof_prbs, int nof_vrbs = -1);
};

inline rbg_interval rbg_interval::prbs_to_rbgs(const prb_interval& prbs, uint32_t cell_nof_prb)
{
  uint32_t P = srsran_ra_type0_P(cell_nof_prb);
  return rbg_interval{prbs.start() / P, srsran::ceil_div(prbs.stop(), P)};
}

/*******************************************************
 *                helper functions
 *******************************************************/

/// If the RBG mask one bits are all contiguous
inline bool is_contiguous(const rbgmask_t& mask)
{
  return rbg_interval::find_first_interval(mask).length() == mask.count();
}

/// Count number of PRBs present in a DL RBG mask
inline uint32_t count_prb_per_tb(const rbgmask_t& bitmask)
{
  uint32_t Nprb    = cell_nof_rbg_to_prb(bitmask.size());
  uint32_t P       = srsran_ra_type0_P(Nprb);
  uint32_t nof_prb = P * bitmask.count();
  if (bitmask.test(bitmask.size() - 1)) {
    nof_prb -= bitmask.size() * P - Nprb;
  }
  return nof_prb;
}

/// Estimate of number of PRBs in DL grant given Nof RBGs
inline uint32_t count_prb_per_tb_approx(uint32_t nof_rbgs, uint32_t cell_nof_prb)
{
  uint32_t P = srsran_ra_type0_P(cell_nof_prb);
  return std::min(nof_rbgs * P, cell_nof_prb);
}

/**
 * Finds a contiguous interval of "zeroed"/available RBG resources
 * @param max_nof_rbgs maximum number of RBGs
 * @param current_mask bitmask of occupied RBGs, used to search for available RBGs
 * @return interval with found RBGs. If a valid interval wasn't found, interval.length() == 0
 */
rbg_interval find_empty_rbg_interval(uint32_t max_nof_rbgs, const rbgmask_t& current_mask);

/**
 * Finds a bitmask of "zeroed"/available RBG resources
 * @param max_nof_rbgs maximum number of RBGs
 * @param current_mask bitmask of occupied RBGs, used to search for available RBGs
 * @return bitmask of found RBGs. If a valid mask wasn't found, bitmask::size() == 0
 */
rbgmask_t find_available_rbgmask(uint32_t max_nof_rbgs, bool is_contiguous, const rbgmask_t& current_mask);

/**
 * Finds a range of L contiguous PRBs that are empty
 * @param L Max length of the requested UL PRBs
 * @param current_mask input PRB mask where to search for available PRBs
 * @return found interval of PRBs
 */
prb_interval find_contiguous_ul_prbs(uint32_t L, const prbmask_t& current_mask);

} // namespace srsenb

namespace fmt {

template <>
struct formatter<srsenb::rbg_interval> : public formatter<srsran::interval<uint32_t> > {};
template <>
struct formatter<srsenb::prb_interval> : public formatter<srsran::interval<uint32_t> > {};

} // namespace fmt

#endif // SRSRAN_SCHED_PHY_RESOURCE_H
