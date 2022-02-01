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
