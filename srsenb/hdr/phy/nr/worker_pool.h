/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSENB_NR_WORKER_POOL_H
#define SRSENB_NR_WORKER_POOL_H

#include "slot_worker.h"
#include "srsenb/hdr/phy/phy_interfaces.h"
#include "srsenb/hdr/phy/prach_worker.h"
#include "srsran/common/thread_pool.h"
#include "srsran/common/tti_sempahore.h"
#include "srsran/interfaces/enb_mac_interfaces.h"
#include "srsran/interfaces/gnb_interfaces.h"

namespace srsenb {
namespace nr {

class worker_pool final : private slot_worker::sync_interface
{
private:
  srsran::tti_semaphore<slot_worker*> slot_sync; ///< Slot synchronization semaphore
  void                                wait(slot_worker* w) override { slot_sync.wait(w); }
  void                                release() override { slot_sync.release(); }

  class prach_stack_adaptor_t : public stack_interface_phy_lte
  {
  private:
    stack_interface_phy_nr& stack;

  public:
    prach_stack_adaptor_t(stack_interface_phy_nr& stack_) : stack(stack_)
    {
      // Do nothing
    }

    int  sr_detected(uint32_t tti, uint16_t rnti) override { return 0; }
    void rach_detected(uint32_t tti, uint32_t primary_cc_idx, uint32_t preamble_idx, uint32_t time_adv) override
    {
      stack_interface_phy_nr::rach_info_t rach_info = {};
      rach_info.slot_index                          = tti;
      rach_info.preamble                            = preamble_idx;
      rach_info.time_adv                            = time_adv;

      stack.rach_detected(rach_info);
    }
    int ri_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t ri_value) override { return 0; }
    int pmi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t pmi_value) override { return 0; }
    int cqi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t cqi_value) override { return 0; }
    int sb_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t sb_idx, uint32_t cqi_value) override
    {
      return 0;
    }
    int snr_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, float snr_db, ul_channel_t ch) override { return 0; }
    int ta_info(uint32_t tti, uint16_t rnti, float ta_us) override { return 0; }
    int ack_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t tb_idx, bool ack) override { return 0; }
    int crc_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t nof_bytes, bool crc_res) override { return 0; }
    int push_pdu(uint32_t tti_rx,
                 uint16_t rnti,
                 uint32_t enb_cc_idx,
                 uint32_t nof_bytes,
                 bool     crc_res,
                 uint32_t ul_nof_prbs) override
    {
      return 0;
    }
    int  get_dl_sched(uint32_t tti, dl_sched_list_t& dl_sched_res) override { return 0; }
    int  get_mch_sched(uint32_t tti, bool is_mcch, dl_sched_list_t& dl_sched_res) override { return 0; }
    int  get_ul_sched(uint32_t tti, ul_sched_list_t& ul_sched_res) override { return 0; }
    void set_sched_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs) override {}
  };

  srsran::phy_common_interface&              common;
  stack_interface_phy_nr&                    stack;
  srslog::sink&                              log_sink;
  srsran::thread_pool                        pool;
  std::vector<std::unique_ptr<slot_worker> > workers;
  prach_worker_pool                          prach;
  uint32_t                                   current_tti = 0; ///< Current TTI, read and write from same thread
  srslog::basic_logger&                      logger;
  prach_stack_adaptor_t                      prach_stack_adaptor;
  uint32_t                                   nof_prach_workers = 0;
  double                                     srate_hz          = 0.0; ///< Current sampling rate in Hz

public:
  struct args_t {
    double                 srate_hz          = 0.0;
    uint32_t               nof_phy_threads   = 3;
    uint32_t               nof_prach_workers = 0;
    uint32_t               prio              = 52;
    uint32_t               pusch_max_its     = 10;
    float                  pusch_min_snr_dB  = -10;
    srsran::phy_log_args_t log               = {};
  };
  slot_worker* operator[](std::size_t pos) { return workers.at(pos).get(); }

  worker_pool(srsran::phy_common_interface& common,
              stack_interface_phy_nr&       stack,
              srslog::sink&                 log_sink,
              uint32_t                      max_workers);
  bool         init(const args_t& args, const phy_cell_cfg_list_nr_t& cell_list);
  slot_worker* wait_worker(uint32_t tti);
  slot_worker* wait_worker_id(uint32_t id);
  void         start_worker(slot_worker* w);
  void         stop();
  int          set_common_cfg(const phy_interface_rrc_nr::common_cfg_t& common_cfg);
};

} // namespace nr
} // namespace srsenb

#endif // SRSENB_NR_WORKER_POOL_H
