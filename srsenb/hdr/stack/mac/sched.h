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

#ifndef SRSENB_SCHEDULER_H
#define SRSENB_SCHEDULER_H

#include "sched_grid.h"
#include "sched_interface.h"
#include "sched_ue.h"
#include "srsenb/hdr/common/common_enb.h"
#include <atomic>
#include <map>
#include <mutex>

namespace srsenb {

class rrc_interface_mac;

class sched : public sched_interface
{
public:
  /*************************************************************
   *
   * FAPI-like Interface
   *
   ************************************************************/

  sched();
  ~sched() override;

  void init(rrc_interface_mac* rrc, const sched_args_t& sched_cfg);
  int  cell_cfg(const std::vector<cell_cfg_t>& cell_cfg) override;
  int  reset() final;

  int  ue_cfg(uint16_t rnti, const ue_cfg_t& ue_cfg) final;
  int  ue_rem(uint16_t rnti) final;
  bool ue_exists(uint16_t rnti) final;

  void phy_config_enabled(uint16_t rnti, bool enabled);

  int bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, const mac_lc_ch_cfg_t& cfg) final;
  int bearer_ue_rem(uint16_t rnti, uint32_t lc_id) final;

  uint32_t get_ul_buffer(uint16_t rnti) final;
  uint32_t get_dl_buffer(uint16_t rnti) final;

  int dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t prio_tx_queue) final;
  int dl_mac_buffer_state(uint16_t rnti, uint32_t ce_code, uint32_t nof_cmds = 1) final;

  int dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack) final;
  int dl_rach_info(uint32_t enb_cc_idx, dl_sched_rar_info_t rar_info) final;
  int dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t ri_value) final;
  int dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t pmi_value) final;
  int dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi_value) final;
  int dl_sb_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t sb_idx, uint32_t cqi_value) final;
  int ul_crc_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, bool crc) final;
  int ul_sr_info(uint32_t tti, uint16_t rnti) override;
  int ul_bsr(uint16_t rnti, uint32_t lcg_id, uint32_t bsr) final;
  int ul_phr(uint16_t rnti, int phr, uint32_t ul_nof_prb) final;
  int ul_snr_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, float snr, uint32_t ul_ch_code) final;

  int dl_sched(uint32_t tti, uint32_t enb_cc_idx, dl_sched_res_t& sched_result) final;
  int ul_sched(uint32_t tti, uint32_t enb_cc_idx, ul_sched_res_t& sched_result) final;

  int set_pdcch_order(uint32_t enb_cc_idx, dl_sched_po_info_t pdcch_order_info) final;

  /* Custom functions
   */
  void                                 set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs) final;
  std::array<int, SRSRAN_MAX_CARRIERS> get_enb_ue_cc_map(uint16_t rnti) final;
  std::array<int, SRSRAN_MAX_CARRIERS> get_enb_ue_activ_cc_map(uint16_t rnti) final;
  int                                  ul_buffer_add(uint16_t rnti, uint32_t lcid, uint32_t bytes) final;
  int                                  metrics_read(uint16_t rnti, mac_ue_metrics_t& metrics);

  class carrier_sched;

protected:
  void new_tti(srsran::tti_point tti_rx);
  bool is_generated(srsran::tti_point, uint32_t enb_cc_idx) const;
  // Helper methods
  template <typename Func>
  int ue_db_access_locked(uint16_t rnti, Func&& f, const char* func_name = nullptr, bool log_fail = true);

  // args
  rrc_interface_mac*               rrc       = nullptr;
  sched_args_t                     sched_cfg = {};
  std::vector<sched_cell_params_t> sched_cell_params;

  rnti_map_t<std::unique_ptr<sched_ue> > ue_db;

  // independent schedulers for each carrier
  std::vector<std::unique_ptr<carrier_sched> > carrier_schedulers;

  // Storage of past scheduling results
  sched_result_ringbuffer sched_results;

  srsran::tti_point last_tti;
  std::mutex        sched_mutex;
  bool              configured;
};

} // namespace srsenb

#endif // SRSENB_SCHEDULER_H
