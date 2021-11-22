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

#include "srsenb/hdr/stack/rrc/rrc_config_common.h"
#include "srsgnb/hdr/phy/phy_nr_interfaces.h"
#include "srsran/asn1/rrc_nr.h"
#include "srsran/common/security.h"
#include "srsran/interfaces/gnb_rrc_nr_interfaces.h"

namespace srsenb {

// TODO: Make this common to NR and LTE
struct rrc_nr_cfg_sr_t {
  uint32_t period;
  //  asn1::rrc::sched_request_cfg_c::setup_s_::dsr_trans_max_e_ dsr_max;
  uint32_t nof_prb;
  uint32_t sf_mapping[80];
  uint32_t nof_subframes;
};

// Cell/Sector configuration for NR cells
struct rrc_cell_cfg_nr_t {
  phy_cell_cfg_nr_t    phy_cell; // already contains all PHY-related parameters (i.e. RF port, PCI, etc.)
  uint32_t             tac;      // Tracking area code
  uint32_t             dl_arfcn; // DL freq already included in phy_cell
  uint32_t             ul_arfcn; // UL freq also in phy_cell
  uint32_t             dl_absolute_freq_point_a; // derived from DL ARFCN
  uint32_t             ul_absolute_freq_point_a; // derived from UL ARFCN
  uint32_t             ssb_absolute_freq_point;  // derived from DL ARFCN
  uint32_t             band;
  uint32_t             coreset0_idx; // Table 13-{1,...15} row index
  srsran_duplex_mode_t duplex_mode;
  srsran_ssb_cfg_t     ssb_cfg;
};

typedef std::vector<rrc_cell_cfg_nr_t> rrc_cell_list_nr_t;

struct rrc_nr_cfg_t {
  rrc_nr_cfg_sr_t    sr_cfg;
  rrc_cfg_cqi_t      cqi_cfg;
  rrc_cell_list_nr_t cell_list;
  bool               is_standalone;

  std::array<srsran::CIPHERING_ALGORITHM_ID_NR_ENUM, srsran::CIPHERING_ALGORITHM_ID_NR_N_ITEMS> nea_preference_list;
  std::array<srsran::INTEGRITY_ALGORITHM_ID_NR_ENUM, srsran::INTEGRITY_ALGORITHM_ID_NR_N_ITEMS> nia_preference_list;

  std::string log_name = "RRC-NR";
  std::string log_level;
  uint32_t    log_hex_limit;
};

} // namespace srsenb

#endif // SRSRAN_RRC_NR_CONFIG_H
