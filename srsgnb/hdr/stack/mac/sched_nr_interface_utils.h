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
