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

#ifndef SRSRAN_ENB_RRC_INTERFACE_TYPES_H
#define SRSRAN_ENB_RRC_INTERFACE_TYPES_H

#include "srsran/asn1/rrc/meascfg.h"
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
  uint32_t                    earfcn;
  uint16_t                    pci;
  uint32_t                    eci;
  asn1::rrc::q_offset_range_e cell_individual_offset;
  uint32_t                    allowed_meas_bw;
  bool                        direct_forward_path_available;
  int                         tac;
};

// neigh measurement Cell info
struct rrc_meas_cfg_t {
  std::vector<meas_cell_cfg_t>               meas_cells;
  std::vector<asn1::rrc::report_cfg_eutra_s> meas_reports;
  asn1::rrc::quant_cfg_eutra_s               quant_cfg;
  uint32_t                                   meas_gap_period;
  std::vector<uint32_t>                      meas_gap_offset_subframe;
  uint32_t                                   allowed_meas_bw;
};

// Cell/Sector configuration
struct cell_cfg_t {
  uint32_t                            rf_port;
  uint32_t                            cell_id;
  uint16_t                            tac;
  uint32_t                            pci;
  double                              tx_gain;
  uint16_t                            root_seq_idx;
  uint32_t                            dl_earfcn;
  double                              dl_freq_hz;
  uint32_t                            ul_earfcn;
  double                              ul_freq_hz;
  int                                 target_pucch_sinr_db;
  int                                 target_pusch_sinr_db;
  bool                                enable_phr_handling;
  int                                 min_phr_thres;
  asn1::rrc::mob_ctrl_info_s::t304_e_ t304;
  std::vector<scell_cfg_t>            scell_list;
  rrc_meas_cfg_t                      meas_cfg;
  bool                                barred;
};

typedef std::vector<cell_cfg_t> cell_list_t;

} // namespace srsenb

#endif // SRSRAN_ENB_RRC_INTERFACE_TYPES_H
