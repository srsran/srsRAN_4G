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
#include "srsran/common/thread_pool.h"

namespace srsenb {

using sched_nr_impl::bwp_worker;
using sched_nr_impl::sched_worker_manager;
using sched_nr_impl::ue;
using sched_nr_impl::ue_carrier;
using sched_nr_impl::ue_map_t;

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
  void new_tti()
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

sched_nr::sched_nr(const sched_nr_cfg& cfg_) :
  cfg(cfg_), pending_events(new ue_event_manager(ue_db)), sched_workers(ue_db, cfg)
{}

sched_nr::~sched_nr() {}

void sched_nr::ue_cfg(uint16_t rnti, const sched_nr_ue_cfg& uecfg)
{
  pending_events->push_event([this, rnti, uecfg]() { ue_cfg_impl(rnti, uecfg); });
}

void sched_nr::ue_cfg_impl(uint16_t rnti, const sched_nr_ue_cfg& uecfg)
{
  if (not ue_db.contains(rnti)) {
    ue_db.insert(rnti, std::unique_ptr<ue>(new ue{rnti, uecfg}));
  } else {
    ue_db[rnti]->set_cfg(uecfg);
  }
}

void sched_nr::new_tti(tti_point tti_rx)
{
  // Lock subframe workers to provided tti_rx
  sched_workers.reserve_workers(tti_rx, sched_results[tti_rx.sf_idx()]);

  {
    // synchronize {tti,cc} state. e.g. reserve UE resources for {tti,cc} decision, process feedback
    std::lock_guard<std::mutex> lock(ue_db_mutex);
    // Process pending events
    pending_events->new_tti();

    sched_workers.start_tti(tti_rx);
  }
}

int sched_nr::generate_sched_result(tti_point tti_rx, uint32_t cc, sched_nr_res_t& result)
{
  // Generate {tti,cc} scheduling decision
  run_tti(tti_rx, cc);

  // copy scheduling decision result
  result = sched_results[tti_rx.sf_idx()][cc];

  return SRSRAN_SUCCESS;
}

void sched_nr::run_tti(tti_point tti_rx, uint32_t cc)
{
  // unlocked, parallel region
  bool all_workers_finished = sched_workers.run_tti(tti_rx, cc);

  if (all_workers_finished) {
    // once all workers of the same subframe finished, synchronize sched outcome with ue_db
    std::lock_guard<std::mutex> lock(ue_db_mutex);
    sched_workers.end_tti(tti_rx);
  }
}

void sched_nr::dl_ack_info(tti_point tti_rx, uint16_t rnti, uint32_t cc, uint32_t tb_idx, bool ack)
{
  pending_events->push_cc_feedback(
      rnti, cc, [tti_rx, tb_idx, ack](ue_carrier& ue_cc) { ue_cc.harq_ent.dl_ack_info(tti_rx, tb_idx, ack); });
}

void sched_nr::ul_sr_info(tti_point tti_rx, uint16_t rnti)
{
  pending_events->push_event([this, rnti, tti_rx]() {
    if (ue_db.contains(rnti)) {
      ue_db[rnti]->ul_sr_info(tti_rx);
    }
  });
}

} // namespace srsenb