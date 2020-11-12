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

#ifndef SRSLTE_SCHED_SIM_UE_H
#define SRSLTE_SCHED_SIM_UE_H

#include "sched_common_test_suite.h"
#include "srslte/interfaces/sched_interface.h"
#include <bitset>

namespace srsenb {

struct ue_harq_ctxt_t {
  bool              active    = false;
  bool              ndi       = false;
  uint32_t          pid       = 0;
  uint32_t          nof_txs   = 0;
  uint32_t          nof_retxs = 0;
  uint32_t          riv       = 0;
  srslte::tti_point last_tti_rx;
};
struct ue_cc_ctxt_t {
  std::array<ue_harq_ctxt_t, SRSLTE_FDD_NOF_HARQ> dl_harqs;
  std::array<ue_harq_ctxt_t, SRSLTE_FDD_NOF_HARQ> ul_harqs;
};
struct sim_ue_ctxt_t {
  uint16_t                                   rnti;
  srslte::tti_point                          prach_tti_rx;
  sched_interface::ue_cfg_t                  ue_cfg;
  std::vector<ue_cc_ctxt_t>                  cc_list;
  const sched_interface::ue_cfg_t::cc_cfg_t* get_cc_cfg(uint32_t enb_cc_idx) const;
  int                                        enb_to_ue_cc_idx(uint32_t enb_cc_idx) const;
};

struct sim_enb_ctxt_t {
  std::vector<sched_cell_params_t>  cell_params;
  std::map<uint16_t, sim_ue_ctxt_t> ue_db;
};
struct pucch_feedback {
  struct cc_data {
    uint32_t enb_cc_idx = 0;
    int      cqi        = -1;
    int      pid        = -1;
    bool     ack        = false;
  };
  std::vector<cc_data> cc_list;
};

class ue_sim
{
public:
  ue_sim() = default;

  int update(const sf_output_res_t& sf_out);

  bool enqueue_pending_acks(srslte::tti_point                tti_rx,
                            pucch_feedback&                  feedback_list,
                            std::bitset<SRSLTE_MAX_CARRIERS> ack_val);

  const sim_ue_ctxt_t& get_ctxt() const { return ctxt; }

private:
  void update_dl_harqs(const sf_output_res_t& sf_out);
  void update_ul_harqs(const sf_output_res_t& sf_out);

  sim_ue_ctxt_t ctxt;
};

} // namespace srsenb

#endif // SRSLTE_SCHED_SIM_UE_H
