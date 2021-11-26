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
