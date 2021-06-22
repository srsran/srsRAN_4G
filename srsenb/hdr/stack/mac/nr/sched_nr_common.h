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

using rbgmask_t = srsran::bounded_bitset<SCHED_NR_MAX_NOF_RBGS, true>;

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_COMMON_H
