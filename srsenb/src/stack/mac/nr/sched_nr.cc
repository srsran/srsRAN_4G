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

#include "srsenb/hdr/stack/mac/nr/sched_nr.h"
#include "srsenb/hdr/stack/mac/nr/harq_softbuffer.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_cell.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_worker.h"
#include "srsran/common/thread_pool.h"

namespace srsenb {

using namespace sched_nr_impl;

static int assert_ue_cfg_valid(uint16_t rnti, const sched_nr_interface::ue_cfg_t& uecfg);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class sched_result_manager
{
public:
  explicit sched_result_manager(uint32_t nof_cc_)
  {
    for (auto& v : results) {
      v.resize(nof_cc_);
    }
  }

  dl_sched_t& add_dl_result(slot_point tti, uint32_t cc)
  {
    if (not has_dl_result(tti, cc)) {
      results[tti.to_uint()][cc].slot_dl = tti;
      results[tti.to_uint()][cc].dl_res  = {};
    }
    return results[tti.to_uint()][cc].dl_res;
  }
  ul_sched_t& add_ul_result(slot_point tti, uint32_t cc)
  {
    if (not has_ul_result(tti, cc)) {
      results[tti.to_uint()][cc].slot_ul = tti;
      results[tti.to_uint()][cc].ul_res  = {};
    }
    return results[tti.to_uint()][cc].ul_res;
  }

  bool has_dl_result(slot_point tti, uint32_t cc) const { return results[tti.to_uint()][cc].slot_dl == tti; }

  bool has_ul_result(slot_point tti, uint32_t cc) const { return results[tti.to_uint()][cc].slot_ul == tti; }

  dl_sched_t pop_dl_result(slot_point tti, uint32_t cc)
  {
    if (has_dl_result(tti, cc)) {
      results[tti.to_uint()][cc].slot_dl.clear();
      return results[tti.to_uint()][cc].dl_res;
    }
    return {};
  }

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
    slot_point slot_dl;
    slot_point slot_ul;
    dl_sched_t dl_res;
    ul_sched_t ul_res;
  };

  srsran::circular_array<std::vector<slot_result_t>, TTIMOD_SZ> results;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sched_nr::sched_nr(const sched_cfg_t& sched_cfg) : cfg(sched_cfg), logger(srslog::fetch_basic_logger("MAC")) {}

sched_nr::~sched_nr() {}

int sched_nr::cell_cfg(srsran::const_span<cell_cfg_t> cell_list)
{
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

  pending_results.reset(new sched_result_manager(cell_list.size()));
  sched_workers.reset(new sched_nr_impl::sched_worker_manager(ue_db, cfg, cells));

  return SRSRAN_SUCCESS;
}

void sched_nr::ue_cfg(uint16_t rnti, const ue_cfg_t& uecfg)
{
  srsran_assert(assert_ue_cfg_valid(rnti, uecfg) == SRSRAN_SUCCESS, "Invalid UE configuration");
  sched_workers->enqueue_event(rnti, [this, rnti, uecfg]() { ue_cfg_impl(rnti, uecfg); });
}

void sched_nr::ue_cfg_impl(uint16_t rnti, const ue_cfg_t& uecfg)
{
  if (not ue_db.contains(rnti)) {
    ue_db.insert(rnti, std::unique_ptr<ue>(new ue{rnti, uecfg, cfg}));
  } else {
    ue_db[rnti]->set_cfg(uecfg);
  }
}

/// Generate {tti,cc} scheduling decision
int sched_nr::generate_slot_result(slot_point pdcch_tti, uint32_t cc)
{
  // Copy results to intermediate buffer
  dl_sched_t& dl_res = pending_results->add_dl_result(pdcch_tti, cc);
  ul_sched_t& ul_res = pending_results->add_ul_result(pdcch_tti, cc);

  // Generate {slot_idx,cc} result
  sched_workers->run_slot(pdcch_tti, cc, dl_res, ul_res);

  return SRSRAN_SUCCESS;
}

int sched_nr::get_dl_sched(slot_point slot_tx, uint32_t cc, dl_sched_t& result)
{
  if (not pending_results->has_dl_result(slot_tx, cc)) {
    generate_slot_result(slot_tx, cc);
  }

  result = pending_results->pop_dl_result(slot_tx, cc);
  return SRSRAN_SUCCESS;
}
int sched_nr::get_ul_sched(slot_point pusch_tti, uint32_t cc, ul_sched_t& result)
{
  if (not pending_results->has_ul_result(pusch_tti, cc)) {
    // sched result hasn't been generated
    return SRSRAN_SUCCESS;
  }

  result = pending_results->pop_ul_result(pusch_tti, cc);
  return SRSRAN_SUCCESS;
}

void sched_nr::dl_ack_info(uint16_t rnti, uint32_t cc, uint32_t pid, uint32_t tb_idx, bool ack)
{
  sched_workers->enqueue_cc_feedback(
      rnti, cc, [pid, tb_idx, ack](ue_carrier& ue_cc) { ue_cc.harq_ent.dl_ack_info(pid, tb_idx, ack); });
}

void sched_nr::ul_crc_info(uint16_t rnti, uint32_t cc, uint32_t pid, bool crc)
{
  sched_workers->enqueue_cc_feedback(rnti, cc, [pid, crc](ue_carrier& ue_cc) { ue_cc.harq_ent.ul_crc_info(pid, crc); });
}

void sched_nr::ul_sr_info(slot_point slot_rx, uint16_t rnti)
{
  sched_workers->enqueue_event(rnti, [this, rnti, slot_rx]() { ue_db[rnti]->ul_sr_info(slot_rx); });
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