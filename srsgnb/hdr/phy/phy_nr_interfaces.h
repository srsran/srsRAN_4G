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

#ifndef SRSRAN_PHY_NR_INTERFACES_H
#define SRSRAN_PHY_NR_INTERFACES_H

#include "srsran/srsran.h"
#include <vector>

namespace srsenb {

struct phy_cell_cfg_nr_t {
  srsran_carrier_nr_t   carrier;
  uint32_t              rf_port;
  uint32_t              cell_id;
  double                dl_freq_hz;
  double                ul_freq_hz;
  uint32_t              root_seq_idx;
  uint32_t              num_ra_preambles;
  float                 gain_db;
  srsran_pdcch_cfg_nr_t pdcch = {}; ///< Common CORESET and Search Space configuration
  srsran_pdsch_cfg_t    pdsch = {};
  srsran_prach_cfg_t    prach = {};
};

using phy_cell_cfg_list_nr_t = std::vector<phy_cell_cfg_nr_t>;

} // namespace srsenb

#endif // SRSRAN_PHY_NR_INTERFACES_H
