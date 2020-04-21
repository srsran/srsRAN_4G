/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_ENB_RRC_INTERFACE_TYPES_H
#define SRSLTE_ENB_RRC_INTERFACE_TYPES_H

#include <vector>

namespace srsenb {

// SCell configuration
struct scell_cfg_t {
  uint32_t cell_id;
  bool     cross_carrier_sched = false;
  uint32_t sched_cell_id;
  bool     ul_allowed;
};

// Cell to measure for Handover
struct meas_cell_cfg_t {
  uint32_t earfcn;
  uint16_t pci;
  uint32_t eci;
  float    q_offset;
};

// neigh measurement Cell info
struct rrc_meas_cfg_t {
  std::vector<meas_cell_cfg_t>               meas_cells;
  std::vector<asn1::rrc::report_cfg_eutra_s> meas_reports;
  asn1::rrc::quant_cfg_eutra_s               quant_cfg;
  // TODO: Add blacklist cells
  // TODO: Add multiple meas configs
};

// Cell/Sector configuration
struct cell_cfg_t {
  uint32_t                 rf_port;
  uint32_t                 cell_id;
  uint16_t                 tac;
  uint32_t                 pci;
  uint16_t                 root_seq_idx;
  uint32_t                 dl_earfcn;
  double                   dl_freq_hz;
  uint32_t                 ul_earfcn;
  double                   ul_freq_hz;
  uint32_t                 initial_dl_cqi;
  std::vector<scell_cfg_t> scell_list;
  rrc_meas_cfg_t           meas_cfg;
};

typedef std::vector<cell_cfg_t> cell_list_t;

} // namespace srsenb

#endif // SRSLTE_ENB_RRC_INTERFACE_TYPES_H
