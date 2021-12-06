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

//////////////////////////////////// Search Space Helpers ////////////////////////////////////////////

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

//////////////////////////////////// CORESET Helpers ////////////////////////////////////////////

/// Get a range of active coresets in a PDCCH configuration
inline srsran::split_optional_span<srsran_coreset_t> view_active_coresets(srsran_pdcch_cfg_nr_t& pdcch)
{
  return srsran::split_optional_span<srsran_coreset_t>{pdcch.coreset, pdcch.coreset_present};
}
inline srsran::split_optional_span<const srsran_coreset_t> view_active_coresets(const srsran_pdcch_cfg_nr_t& pdcch)
{
  return srsran::split_optional_span<const srsran_coreset_t>{pdcch.coreset, pdcch.coreset_present};
}

/// Get number of CCEs available in CORESET for PDCCH
uint32_t coreset_nof_cces(const srsran_coreset_t& coreset);

//////////////////////////////////// Sched Output Helpers ////////////////////////////////////////////

inline bool operator==(srsran_dci_location_t lhs, srsran_dci_location_t rhs)
{
  return lhs.ncce == rhs.ncce and lhs.L == rhs.L;
}

//////////////////////////////////// UE configuration Helpers ////////////////////////////////////////////

srsran::phy_cfg_nr_t get_common_ue_phy_cfg(const sched_nr_interface::cell_cfg_t& cfg);

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_INTERFACE_HELPERS_H
