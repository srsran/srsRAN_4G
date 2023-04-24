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

#ifndef SRSRAN_RRC_NR_CONFIG_H
#define SRSRAN_RRC_NR_CONFIG_H

#include <string>
#include <vector>

namespace srsue {

struct rrc_nr_args_t {
  uint32_t                    sim_nr_meas_pci;
  bool                        pdcp_short_sn_support;
  std::string                 supported_bands_nr_str;
  std::vector<uint32_t>       supported_bands_nr;
  std::vector<uint32_t>       supported_bands_eutra;
  uint32_t                    dl_nr_arfcn;
  uint32_t                    ssb_nr_arfcn;
  uint32_t                    nof_prb;
  srsran_subcarrier_spacing_t scs;
  srsran_subcarrier_spacing_t ssb_scs;
  std::string                 log_level;
  uint32_t                    log_hex_limit;
};

} // namespace srsue

#endif // SRSRAN_RRC_NR_CONFIG_H
