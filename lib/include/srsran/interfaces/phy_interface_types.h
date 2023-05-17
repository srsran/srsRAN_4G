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

#ifndef SRSRAN_PHY_INTERFACE_TYPES_H
#define SRSRAN_PHY_INTERFACE_TYPES_H

#include "srsran/common/common.h"
#include "srsran/srsran.h"

/// Common types defined by the PHY layer.

inline bool operator==(const srsran_tdd_config_t& a, const srsran_tdd_config_t& b)
{
  return (a.sf_config == b.sf_config && a.ss_config == b.ss_config && a.configured == b.configured);
}

inline bool operator!=(const srsran_tdd_config_t& a, const srsran_tdd_config_t& b)
{
  return !(a == b);
}

inline bool operator==(const srsran_prach_cfg_t& a, const srsran_prach_cfg_t& b)
{
  return (a.config_idx == b.config_idx && a.root_seq_idx == b.root_seq_idx && a.zero_corr_zone == b.zero_corr_zone &&
          a.freq_offset == b.freq_offset && a.num_ra_preambles == b.num_ra_preambles && a.hs_flag == b.hs_flag &&
          a.tdd_config == b.tdd_config && a.enable_successive_cancellation == b.enable_successive_cancellation &&
          a.enable_freq_domain_offset_calc == b.enable_freq_domain_offset_calc);
}

inline bool operator!=(const srsran_prach_cfg_t& a, const srsran_prach_cfg_t& b)
{
  return !(a == b);
}

namespace srsue {

struct phy_meas_nr_t {
  float    rsrp;
  float    rsrq;
  float    sinr;
  float    cfo_hz;
  uint32_t arfcn_nr;
  uint32_t pci_nr;
};

struct phy_meas_t {
  srsran::srsran_rat_t rat; ///< LTE or NR
  float                rsrp;
  float                rsrq;
  float                cfo_hz;
  uint32_t             earfcn;
  uint32_t             pci;
};

struct phy_cell_t {
  uint32_t pci;
  uint32_t earfcn;
  float    cfo_hz;
};

} // namespace srsue

#endif // SRSRAN_PHY_INTERFACE_TYPES_H
