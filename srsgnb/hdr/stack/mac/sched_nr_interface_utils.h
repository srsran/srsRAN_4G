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

#ifndef SRSRAN_SCHED_NR_INTERFACE_HELPERS_H
#define SRSRAN_SCHED_NR_INTERFACE_HELPERS_H

#include "sched_nr_interface.h"
#include "srsran/adt/optional_array.h"

namespace srsenb {

// Helpers to handle PHY struct types

/// Get a range of active search spaces in a PDCCH configuration
inline srsran::split_optional_span<srsran_search_space_t> view_active_search_spaces(srsran_pdcch_cfg_nr_t& pdcch)
{
  return srsran::split_optional_span<srsran_search_space_t>{pdcch.search_space, pdcch.search_space_present};
}
inline srsran::split_optional_span<const srsran_search_space_t>
view_active_search_spaces(const srsran_pdcch_cfg_nr_t& pdcch)
{
  return srsran::split_optional_span<const srsran_search_space_t>{pdcch.search_space, pdcch.search_space_present};
}

srsran::phy_cfg_nr_t get_common_ue_phy_cfg(const sched_nr_interface::cell_cfg_t& cfg);

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_INTERFACE_HELPERS_H
