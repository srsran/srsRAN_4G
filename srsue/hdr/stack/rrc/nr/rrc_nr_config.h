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

#ifndef SRSRAN_RRC_NR_CONFIG_H
#define SRSRAN_RRC_NR_CONFIG_H

#include <string>
#include <vector>

namespace srsue {

// Expert arguments to create GW without proper RRC
struct core_less_args_t {
  std::string ip_addr;
  uint8_t     drb_lcid;
};

struct rrc_nr_args_t {
  core_less_args_t      coreless;
  uint32_t              sim_nr_meas_pci;
  bool                  pdcp_short_sn_support;
  std::string           supported_bands_nr_str;
  std::vector<uint32_t> supported_bands_nr;
  std::vector<uint32_t> supported_bands_eutra;
  std::string           log_level;
  uint32_t              log_hex_limit;
};

} // namespace srsue

#endif // SRSRAN_RRC_NR_CONFIG_H
