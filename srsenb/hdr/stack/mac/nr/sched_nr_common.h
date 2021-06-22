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

#ifndef SRSRAN_SCHED_NR_COMMON_H
#define SRSRAN_SCHED_NR_COMMON_H

#include "sched_nr_interface.h"
#include "srsran/adt/bounded_bitset.h"

namespace srsenb {

const static size_t SCHED_NR_MAX_USERS     = 4;
const static size_t SCHED_NR_NOF_SUBFRAMES = 10;
const static size_t SCHED_NR_MAX_NOF_RBGS  = 25;
const static size_t SCHED_NR_NOF_HARQS     = 16;

namespace sched_nr_impl {

struct sched_cell_params {
  const uint32_t          cc;
  const sched_nr_cell_cfg cell_cfg;
  const sched_nr_cfg&     sched_cfg;

  sched_cell_params(uint32_t cc_, const sched_nr_cell_cfg& cell, const sched_nr_cfg& sched_cfg_) :
    cc(cc_), cell_cfg(cell), sched_cfg(sched_cfg_)
  {}
};

struct sched_params {
  const sched_nr_cfg             sched_cfg;
  std::vector<sched_cell_params> cells;

  explicit sched_params(const sched_nr_cfg& sched_cfg_) : sched_cfg(sched_cfg_) {}
};

using rbgmask_t = srsran::bounded_bitset<SCHED_NR_MAX_NOF_RBGS, true>;

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_COMMON_H
