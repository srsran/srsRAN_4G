/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSENB_SCHEDULER_H
#define SRSENB_SCHEDULER_H

#include "scheduler_grid.h"
#include "scheduler_harq.h"
#include "scheduler_ue.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/sched_interface.h"
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>

namespace srsenb {

namespace sched_utils {

inline bool is_in_tti_interval(uint32_t tti, uint32_t tti1, uint32_t tti2)
{
  tti %= 10240;
  tti1 %= 10240;
  tti2 %= 10240;
  if (tti1 <= tti2) {
    return tti >= tti1 and tti <= tti2;
  }
  return tti >= tti1 or tti <= tti2;
}

} // namespace sched_utils

//! structs to bundle together all the sched arguments, and share them with all the sched sub-components
class sched_cell_params_t
{
  struct regs_deleter {
    void operator()(srslte_regs_t* p)
    {
      if (p != nullptr) {
        srslte_regs_free(p);
        delete p;
      }
    }
  };

public:
  bool set_cfg(uint32_t                             enb_cc_idx_,
               const sched_interface::cell_cfg_t&   cfg_,
               const sched_interface::sched_args_t& sched_args);
  // convenience getters
  uint32_t prb_to_rbg(uint32_t nof_prbs) const { return (nof_prbs + (P - 1)) / P; }
  uint32_t nof_prb() const { return cfg.cell.nof_prb; }

  uint32_t                                       enb_cc_idx = 0;
  sched_interface::cell_cfg_t                    cfg        = {};
  const sched_interface::sched_args_t*           sched_cfg  = nullptr;
  std::unique_ptr<srslte_regs_t, regs_deleter>   regs;
  std::array<sched_dci_cce_t, 3>                 common_locations = {};
  std::array<std::array<sched_dci_cce_t, 10>, 3> rar_locations    = {};
  std::array<uint32_t, 3>                        nof_cce_table    = {}; ///< map cfix -> nof cces in PDCCH
  uint32_t                                       P                = 0;
  uint32_t                                       nof_rbgs         = 0;
};

/* Caution: User addition (ue_cfg) and removal (ue_rem) are not thread-safe
 * Rest of operations are thread-safe
 *
 * The subclass sched_ue is thread-safe so that access to shared variables like buffer states
 * from scheduler thread and other threads is protected for each individual user.
 */

class sched : public sched_interface
{
public:
  /*************************************************************
   *
   * Scheduling metric interface definition
   *
   ************************************************************/

  class metric_dl
  {
  public:
    /* Virtual methods for user metric calculation */
    virtual void set_params(const sched_cell_params_t& cell_params_)                          = 0;
    virtual void sched_users(std::map<uint16_t, sched_ue>& ue_db, dl_sf_sched_itf* tti_sched) = 0;
  };

  class metric_ul
  {
  public:
    /* Virtual methods for user metric calculation */
    virtual void set_params(const sched_cell_params_t& cell_params_)                          = 0;
    virtual void sched_users(std::map<uint16_t, sched_ue>& ue_db, ul_sf_sched_itf* tti_sched) = 0;
  };

  /*************************************************************
   *
   * FAPI-like Interface
   *
   ************************************************************/

  sched();
  ~sched();

  void init(rrc_interface_mac* rrc);
  int  cell_cfg(const std::vector<cell_cfg_t>& cell_cfg) override;
  void set_sched_cfg(sched_args_t* sched_cfg);
  int  reset() final;

  int  ue_cfg(uint16_t rnti, const ue_cfg_t& ue_cfg) final;
  int  ue_rem(uint16_t rnti) final;
  bool ue_exists(uint16_t rnti) final;
  void ue_needs_ta_cmd(uint16_t rnti, uint32_t nof_ta_cmd);

  void phy_config_enabled(uint16_t rnti, bool enabled);

  int bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, ue_bearer_cfg_t* cfg) final;
  int bearer_ue_rem(uint16_t rnti, uint32_t lc_id) final;

  uint32_t get_ul_buffer(uint16_t rnti) final;
  uint32_t get_dl_buffer(uint16_t rnti) final;

  int dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue) final;
  int dl_mac_buffer_state(uint16_t rnti, uint32_t ce_code) final;

  int dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack) final;
  int dl_rach_info(uint32_t enb_cc_idx, dl_sched_rar_info_t rar_info) final;
  int dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t ri_value) final;
  int dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t pmi_value) final;
  int dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi_value) final;
  int ul_crc_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, bool crc) final;
  int ul_sr_info(uint32_t tti, uint16_t rnti) override;
  int ul_bsr(uint16_t rnti, uint32_t lcid, uint32_t bsr, bool set_value = true) final;
  int ul_recv_len(uint16_t rnti, uint32_t lcid, uint32_t len) final;
  int ul_phr(uint16_t rnti, int phr) final;
  int ul_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi, uint32_t ul_ch_code) final;

  int dl_sched(uint32_t tti, uint32_t enb_cc_idx, dl_sched_res_t& sched_result) final;
  int ul_sched(uint32_t tti, uint32_t enb_cc_idx, ul_sched_res_t& sched_result) final;

  /* Custom functions
   */
  void            set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs) final;
  void            tpc_inc(uint16_t rnti);
  void            tpc_dec(uint16_t rnti);
  const ue_cfg_t* get_ue_cfg(uint16_t rnti) final;

  // Static Methods
  static uint32_t get_rvidx(uint32_t retx_idx)
  {
    const static int rv_idx[4] = {0, 2, 3, 1};
    return rv_idx[retx_idx % 4];
  }
  static void     generate_cce_location(srslte_regs_t*   regs,
                                        sched_dci_cce_t* location,
                                        uint32_t         cfi,
                                        uint32_t         sf_idx = 0,
                                        uint16_t         rnti   = 0);
  static uint32_t aggr_level(uint32_t aggr_idx) { return 1u << aggr_idx; }

  class carrier_sched;

protected:
  srslte::log_ref                  log_h;
  rrc_interface_mac*               rrc       = nullptr;
  sched_args_t                     sched_cfg = {};
  std::vector<sched_cell_params_t> sched_cell_params;

  pthread_rwlock_t rwlock;

  // Helper methods
  template <typename Func>
  int ue_db_access(uint16_t rnti, Func);

  std::map<uint16_t, sched_ue> ue_db;

  // independent schedulers for each carrier
  std::vector<std::unique_ptr<carrier_sched> > carrier_schedulers;

  std::array<uint32_t, 10> pdsch_re    = {};
  uint32_t                 current_tti = 0;

  bool configured = false;
};

} // namespace srsenb

#endif // SRSENB_SCHEDULER_H
