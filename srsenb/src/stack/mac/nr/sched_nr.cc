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
#include "srsenb/hdr/stack/mac/nr/sched_nr_worker.h"
#include "srsran/common/thread_pool.h"

namespace srsenb {

using namespace sched_nr_impl;

static int assert_ue_cfg_valid(uint16_t rnti, const sched_nr_interface::ue_cfg_t& uecfg);

class ue_event_manager
{
  using callback_t    = srsran::move_callback<void()>;
  using callback_list = srsran::deque<callback_t>;

public:
  explicit ue_event_manager(ue_map_t& ue_db_) : ue_db(ue_db_) {}

  void push_event(srsran::move_callback<void()> event)
  {
    std::lock_guard<std::mutex> lock(common_mutex);
    common_events.push_back(std::move(event));
  }
  void push_cc_feedback(uint16_t rnti, uint32_t cc, srsran::move_callback<void(ue_carrier&)> event)
  {
    std::lock_guard<std::mutex> lock(common_mutex);
    feedback_list.emplace_back();
    feedback_list.back().rnti     = rnti;
    feedback_list.back().cc       = cc;
    feedback_list.back().callback = std::move(event);
  }
  void new_slot()
  {
    {
      std::lock_guard<std::mutex> lock(common_mutex);
      common_events.swap(common_events_tmp); // reuse memory
      feedback_list.swap(feedback_list_tmp);
    }
    while (not common_events_tmp.empty()) {
      common_events_tmp.front()();
      common_events_tmp.pop_front();
    }
    while (not feedback_list_tmp.empty()) {
      auto& e = feedback_list_tmp.front();
      if (ue_db.contains(e.rnti) and ue_db[e.rnti]->carriers[e.cc] != nullptr) {
        ue_db[e.rnti]->carriers[e.cc]->push_feedback(std::move(e.callback));
      }
      feedback_list_tmp.pop_front();
    }
  }

private:
  ue_map_t& ue_db;

  std::mutex    common_mutex;
  callback_list common_events;
  struct ue_feedback {
    uint16_t                                 rnti = SCHED_NR_INVALID_RNTI;
    uint32_t                                 cc   = SCHED_NR_MAX_CARRIERS;
    srsran::move_callback<void(ue_carrier&)> callback;
  };
  srsran::deque<ue_feedback> feedback_list;
  callback_list              common_events_tmp;
  srsran::deque<ue_feedback> feedback_list_tmp;
};

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

  dl_sched_t& add_dl_result(tti_point tti, uint32_t cc)
  {
    if (not has_dl_result(tti, cc)) {
      results[tti.to_uint()][cc].tti_dl = tti;
      results[tti.to_uint()][cc].dl_res = {};
    }
    return results[tti.to_uint()][cc].dl_res;
  }
  ul_sched_t& add_ul_result(tti_point tti, uint32_t cc)
  {
    if (not has_ul_result(tti, cc)) {
      results[tti.to_uint()][cc].tti_ul = tti;
      results[tti.to_uint()][cc].ul_res = {};
    }
    return results[tti.to_uint()][cc].ul_res;
  }

  bool has_dl_result(tti_point tti, uint32_t cc) const { return results[tti.to_uint()][cc].tti_dl == tti; }

  bool has_ul_result(tti_point tti, uint32_t cc) const { return results[tti.to_uint()][cc].tti_ul == tti; }

  dl_sched_t pop_dl_result(tti_point tti, uint32_t cc)
  {
    if (has_dl_result(tti, cc)) {
      results[tti.to_uint()][cc].tti_dl.reset();
      return results[tti.to_uint()][cc].dl_res;
    }
    return {};
  }

  ul_sched_t pop_ul_result(tti_point tti, uint32_t cc)
  {
    if (has_ul_result(tti, cc)) {
      results[tti.to_uint()][cc].tti_ul.reset();
      return results[tti.to_uint()][cc].ul_res;
    }
    return {};
  }

private:
  struct slot_result_t {
    tti_point  tti_dl;
    tti_point  tti_ul;
    dl_sched_t dl_res;
    ul_sched_t ul_res;
  };

  srsran::circular_array<std::vector<slot_result_t>, TTIMOD_SZ> results;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sched_nr::sched_nr(const sched_cfg_t& sched_cfg) :
  cfg(sched_cfg), pending_events(new ue_event_manager(ue_db)), logger(srslog::fetch_basic_logger("MAC"))
{}

sched_nr::~sched_nr() {}

int sched_nr::cell_cfg(srsran::const_span<cell_cfg_t> cell_list)
{
  cfg.cells.reserve(cell_list.size());
  for (uint32_t cc = 0; cc < cell_list.size(); ++cc) {
    cfg.cells.emplace_back(cc, cell_list[cc], cfg.sched_cfg);
  }

  pending_results.reset(new sched_result_manager(cell_list.size()));
  sched_workers.reset(new sched_nr_impl::sched_worker_manager(ue_db, cfg));
  return SRSRAN_SUCCESS;
}

void sched_nr::ue_cfg(uint16_t rnti, const ue_cfg_t& uecfg)
{
  srsran_assert(assert_ue_cfg_valid(rnti, uecfg) == SRSRAN_SUCCESS, "Invalid UE configuration");
  pending_events->push_event([this, rnti, uecfg]() { ue_cfg_impl(rnti, uecfg); });
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
int sched_nr::generate_slot_result(tti_point pdcch_tti, uint32_t cc)
{
  tti_point tti_rx = pdcch_tti - TX_ENB_DELAY;

  // Lock carrier workers for provided tti_rx
  sched_workers->start_slot(tti_rx, [this]() {
    // In case it is first worker for the given slot
    // synchronize {tti,cc} state. e.g. reserve UE resources for {tti,cc} decision, process feedback
    pending_events->new_slot();
  });

  // unlocked, parallel region
  bool all_workers_finished = sched_workers->run_slot(tti_rx, cc);

  if (all_workers_finished) {
    // once all workers of the same subframe finished, synchronize sched outcome with ue_db
    sched_workers->release_slot(tti_rx);
  }

  // Copy results to intermediate buffer
  dl_sched_t& dl_res = pending_results->add_dl_result(pdcch_tti, cc);
  ul_sched_t& ul_res = pending_results->add_ul_result(pdcch_tti, cc);
  sched_workers->get_sched_result(pdcch_tti, cc, dl_res, ul_res);

  return SRSRAN_SUCCESS;
}

int sched_nr::get_dl_sched(tti_point tti_tx, uint32_t cc, dl_sched_t& result)
{
  if (not pending_results->has_dl_result(tti_tx, cc)) {
    generate_slot_result(tti_tx, cc);
  }

  result = pending_results->pop_dl_result(tti_tx, cc);
  return SRSRAN_SUCCESS;
}
int sched_nr::get_ul_sched(tti_point tti_rx, uint32_t cc, ul_sched_t& result)
{
  if (not pending_results->has_ul_result(tti_rx, cc)) {
    return SRSRAN_ERROR;
  }

  result = pending_results->pop_ul_result(tti_rx, cc);
  return SRSRAN_SUCCESS;
}

void sched_nr::dl_ack_info(uint16_t rnti, uint32_t cc, uint32_t pid, uint32_t tb_idx, bool ack)
{
  pending_events->push_cc_feedback(
      rnti, cc, [pid, tb_idx, ack](ue_carrier& ue_cc) { ue_cc.harq_ent.dl_ack_info(pid, tb_idx, ack); });
}

void sched_nr::ul_sr_info(tti_point tti_rx, uint16_t rnti)
{
  pending_events->push_event([this, rnti, tti_rx]() {
    if (ue_db.contains(rnti)) {
      ue_db[rnti]->ul_sr_info(tti_rx);
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