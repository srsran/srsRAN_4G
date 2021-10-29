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
#include "srsenb/hdr/stack/mac/common/mac_metrics.h"
#include "srsenb/hdr/stack/mac/nr/harq_softbuffer.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_bwp.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_worker.h"
#include "srsran/common/string_helpers.h"
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

/// Class that stores events that are not specific to a CC (e.g. SRs, removal of UEs, buffer state updates)
class sched_nr::common_event_manager
{
  struct event_t {
    uint16_t                      rnti;
    const char*                   event_name;
    srsran::move_callback<void()> callback;
    event_t(uint16_t rnti_, const char* event_name_, srsran::move_callback<void()> c) :
      rnti(rnti_), event_name(event_name_), callback(std::move(c))
    {}
  };
  struct ue_event_t {
    uint16_t                                 rnti;
    srsran::move_callback<void(bool, void*)> callback;
    ue_event_t(uint16_t rnti_, srsran::move_callback<void(bool, void*)> c) : rnti(rnti_), callback(std::move(c)) {}
  };

public:
  explicit common_event_manager(srslog::basic_logger& logger_) : logger(logger_) {}

  void enqueue_event(const char* event_name, srsran::move_callback<void()> ev, uint16_t rnti = SRSRAN_INVALID_RNTI)
  {
    std::lock_guard<std::mutex> lock(event_mutex);
    next_slot_events.emplace_back(rnti, event_name, std::move(ev));
  }

  template <typename R, typename... FmtArgs>
  void enqueue_ue_event(uint16_t rnti, R (ue::*ue_action)(FmtArgs...), const char* fmt_str, FmtArgs... args)
  {
    srsran_assert(rnti != SRSRAN_INVALID_RNTI, "Invalid rnti=0x%x passed to common event manager", rnti);

    auto callback = [fmt_str, ue_action, args...](bool call_or_fmt, void* a) {
      if (call_or_fmt) {
        ue* u = static_cast<ue*>(a);
        (u->*ue_action)(args...);
      } else {
        fmt::memory_buffer* fmtbuf = static_cast<fmt::memory_buffer*>(a);
        fmt::format_to(*fmtbuf, fmt_str, args...);
      }
    };
    std::lock_guard<std::mutex> lock(event_mutex);
    next_slot_ue_events.emplace_back(rnti, std::move(callback));
  }

  /// Process events synchronized during slot_indication() that are directed at CA-enabled UEs
  /// Note: non-CA UEs are updated later in get_dl_sched, to leverage parallelism
  void process_common(ue_map_t& ues)
  {
    // Extract pending feedback events
    current_slot_ue_events.clear();
    current_slot_events.clear();
    {
      std::lock_guard<std::mutex> ev_lock(event_mutex);
      next_slot_ue_events.swap(current_slot_ue_events);
      next_slot_events.swap(current_slot_events);
    }
    log_slot_events();

    // non-UE specific events
    for (event_t& ev : current_slot_events) {
      ev.callback();
    }

    for (ue_event_t& ev : current_slot_ue_events) {
      auto ue_it = ues.find(ev.rnti);
      if (ue_it == ues.end()) {
        fmt::memory_buffer fmtbuf;
        ev.callback(false, &fmtbuf);
        logger.warning("SCHED: \"%s\" called for inexistent rnti=0x%x.", srsran::to_c_str(fmtbuf), ev.rnti);
        ev.rnti = SRSRAN_INVALID_RNTI;
      } else if (ue_it->second->has_ca()) {
        // events specific to existing UEs with CA
        ev.callback(true, ue_it->second.get());
        ev.rnti = SRSRAN_INVALID_RNTI;
      }
    }
  }

  /// Process events synchronized during slot_indication() that are directed at non CA-enabled UEs
  void process_carrier_events(ue_map_t& ues, uint32_t cc)
  {
    for (ue_event_t& ev : current_slot_ue_events) {
      if (ev.rnti == SRSRAN_INVALID_RNTI) {
        // events already processed
        continue;
      }
      auto ue_it = ues.find(ev.rnti);
      if (ue_it == ues.end()) {
        fmt::memory_buffer fmtbuf;
        ev.callback(false, &fmtbuf);
        logger.warning("SCHED: \"%s\" called for inexistent rnti=0x%x.", srsran::to_c_str(fmtbuf), ev.rnti);
        ev.rnti = SRSRAN_INVALID_RNTI;
      } else if (not ue_it->second->has_ca() and ue_it->second->carriers[cc] != nullptr) {
        ev.callback(true, ue_it->second.get());
        ev.rnti = SRSRAN_INVALID_RNTI;
      }
    }
  }

private:
  /// logs events to be processed in the current slot
  void log_slot_events()
  {
    if (not logger.debug.enabled()) {
      return;
    }
    fmt::memory_buffer common_fmtbuf;
    fmt::memory_buffer fmtbuf;
    const char*        prefix = "";
    for (event_t& ev : current_slot_events) {
      if (ev.rnti != SRSRAN_INVALID_RNTI) {
        fmt::format_to(common_fmtbuf, "{}{{0x{:x}: {}}}", prefix, ev.rnti, ev.event_name);
      } else {
        fmt::format_to(common_fmtbuf, "{}{{{}}}", prefix, ev.event_name);
      }
      prefix = ", ";
    }
    for (ue_event_t& ev : current_slot_ue_events) {
      fmt::format_to(fmtbuf, "{}{{0x{:x}: ", prefix, ev.rnti);
      ev.callback(false, &fmtbuf);
      fmt::format_to(fmtbuf, "}}");
      prefix = ", ";
    }
    logger.debug("SCHED: Pending slot events: [%s%s]", srsran::to_c_str(common_fmtbuf), srsran::to_c_str(fmtbuf));
  }

  srslog::basic_logger& logger;

  std::mutex                event_mutex;
  srsran::deque<ue_event_t> next_slot_ue_events, current_slot_ue_events;
  srsran::deque<event_t>    next_slot_events, current_slot_events;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class sched_nr::ue_metrics_manager
{
public:
  explicit ue_metrics_manager(ue_map_t& ues_) : ues(ues_) {}

  void stop()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (not stopped) {
      stopped = true;
      // requests during sched::stop may not be fulfilled by sched main thread
      save_metrics_nolock();
    }
  }

  /// Blocking call that waits for the metrics to be filled
  void get_metrics(mac_metrics_t& requested_metrics)
  {
    std::unique_lock<std::mutex> lock(mutex);
    pending_metrics = &requested_metrics;
    if (not stopped) {
      cvar.wait(lock, [this]() { return pending_metrics == nullptr; });
    } else {
      save_metrics_nolock();
    }
  }

  /// called from within the scheduler main thread to save metrics
  void save_metrics()
  {
    {
      std::unique_lock<std::mutex> lock(mutex);
      save_metrics_nolock();
    }
    cvar.notify_one();
  }

private:
  void save_metrics_nolock()
  {
    if (pending_metrics == nullptr) {
      return;
    }
    for (mac_ue_metrics_t& ue_metric : pending_metrics->ues) {
      if (ues.contains(ue_metric.rnti) and ues[ue_metric.rnti]->carriers[0] != nullptr) {
        auto& ue_cc         = *ues[ue_metric.rnti]->carriers[0];
        ue_metric.tx_brate  = ue_cc.metrics.tx_brate;
        ue_metric.tx_errors = ue_cc.metrics.tx_errors;
        ue_metric.tx_pkts   = ue_cc.metrics.tx_pkts;
        ue_cc.metrics       = {};
      }
    }
    pending_metrics = nullptr;
  }

  ue_map_t& ues;

  std::mutex              mutex;
  std::condition_variable cvar;
  mac_metrics_t*          pending_metrics = nullptr;
  bool                    stopped         = false;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sched_nr::sched_nr() : logger(&srslog::fetch_basic_logger("MAC-NR")), metrics_handler(new ue_metrics_manager{ue_db}) {}

sched_nr::~sched_nr()
{
  stop();
}

void sched_nr::stop()
{
  metrics_handler->stop();
}

int sched_nr::config(const sched_args_t& sched_cfg, srsran::const_span<cell_cfg_t> cell_list)
{
  cfg    = sched_params{sched_cfg};
  logger = &srslog::fetch_basic_logger(sched_cfg.logger_name);

  // Initiate Common Sched Configuration
  cfg.cells.reserve(cell_list.size());
  for (uint32_t cc = 0; cc < cell_list.size(); ++cc) {
    cfg.cells.emplace_back(cc, cell_list[cc], cfg.sched_cfg);
  }

  pending_results.reset(new ul_sched_result_buffer(cell_list.size()));

  pending_events.reset(new common_event_manager{*logger});

  // Initiate cell-specific schedulers
  cc_workers.resize(cfg.cells.size());
  for (uint32_t cc = 0; cc < cfg.cells.size(); ++cc) {
    cc_workers[cc].reset(new slot_cc_worker{cfg.cells[cc]});
  }

  return SRSRAN_SUCCESS;
}

void sched_nr::ue_cfg(uint16_t rnti, const ue_cfg_t& uecfg)
{
  srsran_assert(assert_ue_cfg_valid(rnti, uecfg) == SRSRAN_SUCCESS, "Invalid UE configuration");
  pending_events->enqueue_event(
      "ue_cfg", [this, rnti, uecfg]() { ue_cfg_impl(rnti, uecfg); }, rnti);
}

void sched_nr::ue_rem(uint16_t rnti)
{
  pending_events->enqueue_event(
      "ue_rem", [this, rnti]() { ue_db.erase(rnti); }, rnti);
}

void sched_nr::ue_cfg_impl(uint16_t rnti, const ue_cfg_t& uecfg)
{
  if (not ue_db.contains(rnti)) {
    logger->info("SCHED: New user rnti=0x%x, cc=%d", rnti, cfg.cells[0].cc);
    auto ret = ue_db.insert(rnti, std::unique_ptr<ue>(new ue{rnti, uecfg, cfg}));
    if (not ret.has_value()) {
      logger->error("SCHED: Failed to create new user rnti=0x%x", rnti);
    }
  } else {
    ue_db[rnti]->set_cfg(uecfg);
  }
}

// NOTE: there is no parallelism in these operations
void sched_nr::slot_indication(slot_point slot_tx)
{
  srsran_assert(worker_count.load(std::memory_order_relaxed) == 0,
                "Call of sched slot_indication when previous TTI has not been completed");
  // mark the start of slot.
  current_slot_tx = slot_tx;
  worker_count.store(static_cast<int>(cfg.cells.size()), std::memory_order_relaxed);

  // process non-cc specific feedback if pending (e.g. SRs, buffer state updates, UE config) for CA-enabled UEs
  // Note: non-CA UEs are updated later in get_dl_sched, to leverage parallelism
  pending_events->process_common(ue_db);

  // prepare CA-enabled UEs internal state for new slot
  // Note: non-CA UEs are updated later in get_dl_sched, to leverage parallelism
  for (auto& u : ue_db) {
    if (u.second->has_ca()) {
      u.second->new_slot(slot_tx);
    }
  }

  // If UE metrics were externally requested, store the current UE state
  metrics_handler->save_metrics();
}

/// Generate {pdcch_slot,cc} scheduling decision
int sched_nr::get_dl_sched(slot_point pdsch_tti, uint32_t cc, dl_res_t& result)
{
  srsran_assert(pdsch_tti == current_slot_tx, "Unexpected pdsch_tti slot received");

  // Copy UL results to intermediate buffer
  ul_res_t& ul_res = pending_results->add_ul_result(pdsch_tti, cc);

  // process non-cc specific feedback if pending (e.g. SRs, buffer state updates, UE config) for non-CA UEs
  pending_events->process_carrier_events(ue_db, cc);

  // prepare non-CA UEs internal state for new slot
  for (auto& u : ue_db) {
    if (not u.second->has_ca() and u.second->carriers[cc] != nullptr) {
      u.second->new_slot(current_slot_tx);
    }
  }

  // Process pending CC-specific feedback, generate {slot_idx,cc} scheduling decision
  cc_workers[cc]->run_slot(pdsch_tti, ue_db, result, ul_res);

  // decrement the number of active workers
  int rem_workers = worker_count.fetch_sub(1, std::memory_order_release) - 1;
  srsran_assert(rem_workers >= 0, "invalid number of calls to get_dl_sched(slot, cc)");
  if (rem_workers == 0) {
    // Last Worker to finish slot
    // TODO: Sync sched results with ue_db state
  }

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
  metrics_handler->get_metrics(metrics);
}

int sched_nr::dl_rach_info(uint32_t cc, const rar_info_t& rar_info)
{
  cc_workers[cc]->pending_feedback.enqueue_common_event(
      [this, cc, rar_info]() { cc_workers[cc]->bwps[0].ra.dl_rach_info(rar_info); });
  return SRSRAN_SUCCESS;
}

void sched_nr::dl_ack_info(uint16_t rnti, uint32_t cc, uint32_t pid, uint32_t tb_idx, bool ack)
{
  cc_workers[cc]->pending_feedback.enqueue_ue_feedback(rnti, [this, pid, tb_idx, ack](ue_carrier& ue_cc) {
    int tbs = ue_cc.harq_ent.dl_ack_info(pid, tb_idx, ack);
    if (tbs >= 0) {
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
  cc_workers[cc]->pending_feedback.enqueue_ue_feedback(rnti, [this, pid, crc](ue_carrier& ue_cc) {
    if (ue_cc.harq_ent.ul_crc_info(pid, crc) < 0) {
      logger->warning("SCHED: rnti=0x%x, received CRC for empty pid=%d", ue_cc.rnti, pid);
    }
  });
}

void sched_nr::ul_sr_info(uint16_t rnti)
{
  pending_events->enqueue_ue_event(rnti, &ue::ul_sr_info, "ul_sr_info");
}

void sched_nr::ul_bsr(uint16_t rnti, uint32_t lcg_id, uint32_t bsr)
{
  pending_events->enqueue_ue_event(rnti, &ue::ul_bsr, "ul_bsr(lcg_id={}, bsr={})", lcg_id, bsr);
}

void sched_nr::dl_buffer_state(uint16_t rnti, uint32_t lcid, uint32_t newtx, uint32_t retx)
{
  pending_events->enqueue_ue_event(
      rnti, &ue::rlc_buffer_state, "dl_buffer_state(lcid={}, newtx={}, retx={})", lcid, newtx, retx);
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
