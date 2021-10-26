/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/mac/nr/sched_nr.h"
#include "srsenb/hdr/stack/mac/common/mac_metrics.h"
#include "srsenb/hdr/stack/mac/nr/harq_softbuffer.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_cell.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_worker.h"
#include "srsran/common/thread_pool.h"

namespace srsenb {

using namespace sched_nr_impl;

static int assert_ue_cfg_valid(uint16_t rnti, const sched_nr_interface::ue_cfg_t& uecfg);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ul_sched_result_buffer
{
public:
  explicit ul_sched_result_buffer(uint32_t nof_cc_)
  {
    for (auto& v : results) {
      v.resize(nof_cc_);
    }
  }

  ul_sched_t& add_ul_result(slot_point tti, uint32_t cc)
  {
    if (not has_ul_result(tti, cc)) {
      results[tti.to_uint()][cc].slot_ul = tti;
      results[tti.to_uint()][cc].ul_res  = {};
    }
    return results[tti.to_uint()][cc].ul_res;
  }

  bool has_ul_result(slot_point tti, uint32_t cc) const { return results[tti.to_uint()][cc].slot_ul == tti; }

  ul_sched_t pop_ul_result(slot_point tti, uint32_t cc)
  {
    if (has_ul_result(tti, cc)) {
      results[tti.to_uint()][cc].slot_ul.clear();
      return results[tti.to_uint()][cc].ul_res;
    }
    return {};
  }

private:
  struct slot_result_t {
    slot_point slot_ul;
    ul_sched_t ul_res;
  };

  srsran::circular_array<std::vector<slot_result_t>, TTIMOD_SZ> results;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sched_nr::sched_nr() : logger(&srslog::fetch_basic_logger("MAC-NR")) {}

sched_nr::~sched_nr() {}

int sched_nr::config(const sched_args_t& sched_cfg, srsran::const_span<cell_cfg_t> cell_list)
{
  cfg    = sched_params{sched_cfg};
  logger = &srslog::fetch_basic_logger(sched_cfg.logger_name);

  // Initiate Common Sched Configuration
  cfg.cells.reserve(cell_list.size());
  for (uint32_t cc = 0; cc < cell_list.size(); ++cc) {
    cfg.cells.emplace_back(cc, cell_list[cc], cfg.sched_cfg);
  }

  // Initiate cell-specific schedulers
  cells.reserve(cell_list.size());
  for (uint32_t cc = 0; cc < cell_list.size(); ++cc) {
    cells.emplace_back(new serv_cell_manager{cfg.cells[cc]});
  }

  pending_results.reset(new ul_sched_result_buffer(cell_list.size()));
  sched_workers.reset(new sched_nr_impl::sched_worker_manager(ue_db, cfg, cells));

  return SRSRAN_SUCCESS;
}

void sched_nr::ue_cfg(uint16_t rnti, const ue_cfg_t& uecfg)
{
  srsran_assert(assert_ue_cfg_valid(rnti, uecfg) == SRSRAN_SUCCESS, "Invalid UE configuration");
  sched_workers->enqueue_event(rnti, [this, rnti, uecfg]() { ue_cfg_impl(rnti, uecfg); });
}

void sched_nr::ue_rem(uint16_t rnti)
{
  sched_workers->enqueue_event(rnti, [this, rnti]() {
    auto ue_it = ue_db.find(rnti);
    if (ue_it == ue_db.end()) {
      logger->warning("SCHED: ue_rem(rnti) called for inexistent rnti=0x%x", rnti);
      return;
    }
    ue_db.erase(rnti);
  });
}

bool sched_nr::ue_exists(uint16_t rnti)
{
  return ue_db.contains(rnti);
}

void sched_nr::ue_cfg_impl(uint16_t rnti, const ue_cfg_t& uecfg)
{
  if (not ue_db.contains(rnti)) {
    auto ret = ue_db.insert(rnti, std::unique_ptr<ue>(new ue{rnti, uecfg, cfg}));
    if (ret.has_value()) {
      logger->info("SCHED: New user rnti=0x%x, cc=%d", rnti, cfg.cells[0].cc);
    } else {
      logger->error("SCHED: Failed to create new user rnti=0x%x", rnti);
    }
  } else {
    ue_db[rnti]->set_cfg(uecfg);
  }
}

/// Generate {pdcch_slot,cc} scheduling decision
int sched_nr::run_slot(slot_point slot_dl, uint32_t cc, dl_res_t& result)
{
  // Copy UL results to intermediate buffer
  ul_res_t& ul_res = pending_results->add_ul_result(slot_dl, cc);

  // Generate {slot_idx,cc} result
  sched_workers->run_slot(slot_dl, cc, result, ul_res);

  return SRSRAN_SUCCESS;
}

/// Fetch {ul_slot,cc} UL scheduling decision
int sched_nr::get_ul_sched(slot_point slot_ul, uint32_t cc, ul_res_t& result)
{
  if (not pending_results->has_ul_result(slot_ul, cc)) {
    // sched result hasn't been generated
    result.pucch.clear();
    result.pusch.clear();
    return SRSRAN_SUCCESS;
  }

  result = pending_results->pop_ul_result(slot_ul, cc);
  return SRSRAN_SUCCESS;
}

void sched_nr::get_metrics(mac_metrics_t& metrics)
{
  sched_workers->get_metrics(metrics);
}

int sched_nr::dl_rach_info(uint32_t cc, const rar_info_t& rar_info)
{
  sched_workers->enqueue_cc_event(cc, [this, cc, rar_info]() { cells[cc]->bwps[0].ra.dl_rach_info(rar_info); });
  return SRSRAN_SUCCESS;
}

void sched_nr::dl_ack_info(uint16_t rnti, uint32_t cc, uint32_t pid, uint32_t tb_idx, bool ack)
{
  sched_workers->enqueue_cc_feedback(rnti, cc, [this, pid, tb_idx, ack](ue_carrier& ue_cc) {
    int tbs = ue_cc.harq_ent.dl_ack_info(pid, tb_idx, ack);
    if (tbs >= 0) {
      std::lock_guard<std::mutex> lock(ue_cc.metrics_mutex);
      if (ack) {
        ue_cc.metrics.tx_brate += tbs;
      } else {
        ue_cc.metrics.tx_errors++;
      }
      ue_cc.metrics.tx_pkts++;
    } else {
      logger->warning("SCHED: rnti=0x%x, received DL HARQ-ACK for empty pid=%d", ue_cc.rnti, pid);
    }
  });
}

void sched_nr::ul_crc_info(uint16_t rnti, uint32_t cc, uint32_t pid, bool crc)
{
  sched_workers->enqueue_cc_feedback(rnti, cc, [this, pid, crc](ue_carrier& ue_cc) {
    if (ue_cc.harq_ent.ul_crc_info(pid, crc) < 0) {
      logger->warning("SCHED: rnti=0x%x, received CRC for empty pid=%d", ue_cc.rnti, pid);
    }
  });
}

void sched_nr::ul_sr_info(uint16_t rnti)
{
  sched_workers->enqueue_event(rnti, [this, rnti]() {
    if (ue_db.contains(rnti)) {
      ue_db[rnti]->ul_sr_info();
    } else {
      logger->warning("Received SR for inexistent rnti=0x%x", rnti);
    }
  });
}

void sched_nr::ul_bsr(uint16_t rnti, uint32_t lcg_id, uint32_t bsr)
{
  sched_workers->enqueue_event(rnti, [this, rnti, lcg_id, bsr]() {
    if (ue_db.contains(rnti)) {
      ue_db[rnti]->ul_bsr(lcg_id, bsr);
    } else {
      logger->warning("Received BSR=%d for inexistent rnti=0x%x", bsr, rnti);
    }
  });
}

void sched_nr::dl_buffer_state(uint16_t rnti, uint32_t lcid, uint32_t newtx, uint32_t retx)
{
  sched_workers->enqueue_event(rnti, [this, rnti, lcid, newtx, retx]() {
    if (ue_db.contains(rnti)) {
      ue_db[rnti]->rlc_buffer_state(lcid, newtx, retx);
    } else {
      logger->warning("Received DL buffer state=%d/%d for inexistent rnti=0x%x", newtx, retx, rnti);
    }
  });
}

#define VERIFY_INPUT(cond, msg, ...)                                                                                   \
  do {                                                                                                                 \
    if (not(cond)) {                                                                                                   \
      srslog::fetch_basic_logger("MAC").warning(msg, ##__VA_ARGS__);                                                   \
      return SRSRAN_ERROR;                                                                                             \
    }                                                                                                                  \
  } while (0)

int assert_ue_cfg_valid(uint16_t rnti, const sched_nr_interface::ue_cfg_t& uecfg)
{
  VERIFY_INPUT(std::count(&uecfg.phy_cfg.pdcch.coreset_present[0],
                          &uecfg.phy_cfg.pdcch.coreset_present[SRSRAN_UE_DL_NR_MAX_NOF_CORESET],
                          true) > 0,
               "Provided rnti=0x%x configuration does not contain any coreset",
               rnti);
  return SRSRAN_SUCCESS;
}

} // namespace srsenb
