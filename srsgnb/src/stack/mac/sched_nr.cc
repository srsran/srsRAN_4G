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

#include "srsgnb/hdr/stack/mac/sched_nr.h"
#include "srsenb/hdr/stack/mac/common/mac_metrics.h"
#include "srsgnb/hdr/stack/mac/harq_softbuffer.h"
#include "srsgnb/hdr/stack/mac/sched_nr_bwp.h"
#include "srsgnb/hdr/stack/mac/sched_nr_worker.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/string_helpers.h"
#include "srsran/common/thread_pool.h"

namespace srsenb {

using namespace sched_nr_impl;

static int assert_ue_cfg_valid(uint16_t rnti, const sched_nr_interface::ue_cfg_t& uecfg);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Class that stores events that are not specific to a CC (e.g. SRs, removal of UEs, buffer state updates)
class sched_nr::event_manager
{
public:
  /// class used to accummulate all processed event messages of a single {slot,cc} and print them in a single log line
  struct logger {
    explicit logger(int cc_, srslog::basic_logger& logger_) :
      log_enabled(logger_.debug.enabled()), cc(cc_), sched_logger(logger_)
    {
    }
    logger(const logger&)            = delete;
    logger(logger&&)                 = delete;
    logger& operator=(const logger&) = delete;
    logger& operator=(logger&&)      = delete;
    ~logger()
    {
      if (log_enabled and event_fmtbuf.size() > 0) {
        if (cc < 0) {
          sched_logger.debug("SCHED: slot events: [%s]", srsran::to_c_str(event_fmtbuf));
        } else {
          sched_logger.debug("SCHED: slot events, cc=%d: [%s]", cc, srsran::to_c_str(event_fmtbuf));
        }
      }
    }

    template <typename... Args>
    void push(const char* fmt, Args&&... args)
    {
      if (log_enabled) {
        if (event_fmtbuf.size() > 0) {
          fmt::format_to(event_fmtbuf, ", ");
        }
        fmt::format_to(event_fmtbuf, fmt, std::forward<Args>(args)...);
      }
    }

  private:
    bool                  log_enabled;
    int                   cc;
    srslog::basic_logger& sched_logger;
    fmt::memory_buffer    event_fmtbuf;
  };

  explicit event_manager(sched_params_t& params) :
    sched_logger(srslog::fetch_basic_logger(params.sched_cfg.logger_name)), carriers(params.cells.size())
  {
  }

  /// Enqueue an event that does not map into a ue method (e.g. rem_user, add_user)
  void enqueue_event(const char* event_name, srsran::move_callback<void(logger&)> ev)
  {
    std::lock_guard<std::mutex> lock(event_mutex);
    next_slot_events.emplace_back(event_name, std::move(ev));
  }

  /// Enqueue an event that directly maps into a ue method (e.g. ul_sr_info, ul_bsr, etc.)
  /// Note: these events can be processed sequentially or in parallel, depending on whether the UE supports CA
  void enqueue_ue_event(const char* event_name, uint16_t rnti, srsran::move_callback<void(ue&, logger&)> callback)
  {
    srsran_assert(rnti != SRSRAN_INVALID_RNTI, "Invalid rnti=0x%x passed to common event manager", rnti);
    std::lock_guard<std::mutex> lock(event_mutex);
    next_slot_ue_events.emplace_back(rnti, event_name, std::move(callback));
  }

  /// Enqueue feedback directed at a given UE in a given cell (e.g. ACKs, CQI)
  void enqueue_ue_cc_feedback(const char*                                       event_name,
                              uint16_t                                          rnti,
                              uint32_t                                          cc,
                              srsran::move_callback<void(ue_carrier&, logger&)> callback)
  {
    srsran_assert(rnti != SRSRAN_INVALID_RNTI, "Invalid rnti=0x%x passed to event manager", rnti);
    srsran_assert(cc < carriers.size(), "Invalid cc=%d passed to event manager", cc);
    std::lock_guard<std::mutex> lock(carriers[cc].event_cc_mutex);
    carriers[cc].next_slot_ue_events.emplace_back(rnti, cc, event_name, std::move(callback));
  }

  /// Process all events that are not specific to a carrier or that are directed at CA-enabled UEs
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

    logger evlogger(-1, sched_logger);

    // non-UE specific events
    for (event_t& ev : current_slot_events) {
      ev.callback(evlogger);
    }

    for (ue_event_t& ev : current_slot_ue_events) {
      auto ue_it = ues.find(ev.rnti);
      if (ue_it == ues.end()) {
        sched_logger.warning("SCHED: \"%s\" called for unknown rnti=0x%x.", ev.event_name, ev.rnti);
        ev.rnti = SRSRAN_INVALID_RNTI;
      } else if (ue_it->second->has_ca()) {
        // events specific to existing UEs with CA
        ev.callback(*ue_it->second, evlogger);
        ev.rnti = SRSRAN_INVALID_RNTI;
      }
    }
  }

  /// Process events synchronized during slot_indication() that are directed at non CA-enabled UEs
  void process_cc_events(ue_map_t& ues, uint32_t cc)
  {
    logger evlogger(cc, sched_logger);

    {
      carriers[cc].current_slot_ue_events.clear();
      std::lock_guard<std::mutex> lock(carriers[cc].event_cc_mutex);
      carriers[cc].current_slot_ue_events.swap(carriers[cc].next_slot_ue_events);
    }

    for (ue_event_t& ev : current_slot_ue_events) {
      if (ev.rnti == SRSRAN_INVALID_RNTI) {
        // events already processed
        continue;
      }
      auto ue_it = ues.find(ev.rnti);
      if (ue_it == ues.end()) {
        sched_logger.warning("SCHED: \"%s\" called for unknown rnti=0x%x.", ev.event_name, ev.rnti);
        ev.rnti = SRSRAN_INVALID_RNTI;
      } else if (not ue_it->second->has_ca() and ue_it->second->carriers[cc] != nullptr) {
        ev.callback(*ue_it->second, evlogger);
        ev.rnti = SRSRAN_INVALID_RNTI;
      }
    }

    for (ue_cc_event_t& ev : carriers[cc].current_slot_ue_events) {
      auto ue_it = ues.find(ev.rnti);
      if (ue_it != ues.end() and ue_it->second->carriers[cc] != nullptr) {
        ev.callback(*ue_it->second->carriers[cc], evlogger);
      } else {
        sched_logger.warning("SCHED: \"%s\" called for unknown rnti=0x%x,cc=%d.", ev.event_name, ev.rnti, ev.cc);
      }
    }
  }

private:
  struct event_t {
    const char*                          event_name;
    srsran::move_callback<void(logger&)> callback;
    event_t(const char* event_name_, srsran::move_callback<void(logger&)> c) :
      event_name(event_name_), callback(std::move(c))
    {
    }
  };
  struct ue_event_t {
    uint16_t                                  rnti;
    const char*                               event_name;
    srsran::move_callback<void(ue&, logger&)> callback;
    ue_event_t(uint16_t rnti_, const char* event_name_, srsran::move_callback<void(ue&, logger&)> c) :
      rnti(rnti_), event_name(event_name_), callback(std::move(c))
    {
    }
  };
  struct ue_cc_event_t {
    uint16_t                                          rnti;
    uint32_t                                          cc;
    const char*                                       event_name;
    srsran::move_callback<void(ue_carrier&, logger&)> callback;
    ue_cc_event_t(uint16_t                                          rnti_,
                  uint32_t                                          cc_,
                  const char*                                       event_name_,
                  srsran::move_callback<void(ue_carrier&, logger&)> c) :
      rnti(rnti_), cc(cc_), event_name(event_name_), callback(std::move(c))
    {
    }
  };

  srslog::basic_logger& sched_logger;

  std::mutex             event_mutex;
  std::deque<event_t>    next_slot_events, current_slot_events;
  std::deque<ue_event_t> next_slot_ue_events, current_slot_ue_events;
  struct cc_events {
    std::mutex                   event_cc_mutex;
    srsran::deque<ue_cc_event_t> next_slot_ue_events, current_slot_ue_events;
  };
  std::vector<cc_events> carriers;
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

int sched_nr::config(const sched_args_t& sched_cfg, srsran::const_span<sched_nr_cell_cfg_t> cell_list)
{
  cfg    = sched_params_t{sched_cfg};
  logger = &srslog::fetch_basic_logger(sched_cfg.logger_name);

  // Initiate UE memory pool
  ue_pool.reset(new srsran::circular_stack_pool<SRSENB_MAX_UES>(8, sizeof(ue), 4));

  // Initiate Common Sched Configuration
  cfg.cells.reserve(cell_list.size());
  for (uint32_t cc = 0; cc < cell_list.size(); ++cc) {
    cfg.cells.emplace_back(cc, cell_list[cc], cfg.sched_cfg);
  }

  pending_events.reset(new event_manager{cfg});

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
  pending_events->enqueue_event("ue_cfg", [this, rnti, uecfg](event_manager::logger& ev_logger) {
    if (ue_cfg_impl(rnti, uecfg) == SRSRAN_SUCCESS) {
      ev_logger.push("ue_cfg(0x{:x})", rnti);
    } else {
      logger->warning("Failed to create UE object for rnti=0x{:x}", rnti);
    }
  });
}

void sched_nr::ue_rem(uint16_t rnti)
{
  pending_events->enqueue_event("ue_rem", [this, rnti](event_manager::logger& ev_logger) {
    ue_db.erase(rnti);
    logger->info("SCHED: Removed user rnti=0x%x", rnti);
    ev_logger.push("ue_rem(0x{:x})", rnti);
  });
}

int sched_nr::add_ue_impl(uint16_t rnti, sched_nr_impl::unique_ue_ptr u)
{
  logger->info("SCHED: New user rnti=0x%x, cc=%d", rnti, cfg.cells[0].cc);
  return ue_db.insert(rnti, std::move(u)).has_value() ? SRSRAN_SUCCESS : SRSRAN_ERROR;
}

int sched_nr::ue_cfg_impl(uint16_t rnti, const ue_cfg_t& uecfg)
{
  if (not ue_db.contains(rnti)) {
    // create user object
    unique_ue_ptr u = srsran::make_pool_obj_with_fallback<ue>(*ue_pool, rnti, rnti, uecfg, cfg);
    return add_ue_impl(rnti, std::make_unique<ue>(rnti, uecfg, cfg));
  }
  ue_db[rnti]->set_cfg(uecfg);
  return SRSRAN_SUCCESS;
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
sched_nr::dl_res_t* sched_nr::get_dl_sched(slot_point pdsch_tti, uint32_t cc)
{
  srsran_assert(pdsch_tti == current_slot_tx, "Unexpected pdsch_tti slot received");

  // process non-cc specific feedback if pending (e.g. SRs, buffer state updates, UE config) for non-CA UEs
  pending_events->process_cc_events(ue_db, cc);

  // prepare non-CA UEs internal state for new slot
  for (auto& u : ue_db) {
    if (not u.second->has_ca() and u.second->carriers[cc] != nullptr) {
      u.second->new_slot(current_slot_tx);
    }
  }

  // Process pending CC-specific feedback, generate {slot_idx,cc} scheduling decision
  sched_nr::dl_res_t* ret = cc_workers[cc]->run_slot(pdsch_tti, ue_db);

  // decrement the number of active workers
  int rem_workers = worker_count.fetch_sub(1, std::memory_order_release) - 1;
  srsran_assert(rem_workers >= 0, "invalid number of calls to get_dl_sched(slot, cc)");
  if (rem_workers == 0) {
    // Last Worker to finish slot
    // TODO: Sync sched results with ue_db state
  }

  return ret;
}

/// Fetch {ul_slot,cc} UL scheduling decision
sched_nr::ul_res_t* sched_nr::get_ul_sched(slot_point slot_ul, uint32_t cc)
{
  return cc_workers[cc]->get_ul_sched(slot_ul);
}

void sched_nr::get_metrics(mac_metrics_t& metrics)
{
  metrics_handler->get_metrics(metrics);
}

int sched_nr::dl_rach_info(const rar_info_t& rar_info)
{
  // create user object outside of sched main thread
  unique_ue_ptr u =
      srsran::make_pool_obj_with_fallback<ue>(*ue_pool, rar_info.temp_crnti, rar_info.temp_crnti, rar_info.cc, cfg);

  // enqueue UE creation event + RACH handling
  auto add_ue = [this, rar_info, u = std::move(u)](event_manager::logger& ev_logger) mutable {
    uint16_t rnti = rar_info.temp_crnti;
    if (add_ue_impl(rnti, std::move(u)) == SRSRAN_SUCCESS) {
      ev_logger.push("dl_rach_info(temp c-rnti=0x{:x})", rar_info.temp_crnti);
      // RACH is handled only once the UE object is created and inserted in the ue_db
      uint32_t cc = rar_info.cc;
      cc_workers[cc]->dl_rach_info(rar_info);
    } else {
      logger->warning("Failed to create UE object with rnti=0x%x", rar_info.temp_crnti);
    }
  };
  pending_events->enqueue_event("dl_rach_info", std::move(add_ue));
  return SRSRAN_SUCCESS;
}

void sched_nr::dl_ack_info(uint16_t rnti, uint32_t cc, uint32_t pid, uint32_t tb_idx, bool ack)
{
  auto callback = [pid, tb_idx, ack](ue_carrier& ue_cc, event_manager::logger& ev_logger) {
    if (ue_cc.dl_ack_info(pid, tb_idx, ack) >= 0) {
      ev_logger.push("0x{:x}: dl_ack_info(pid={}, ack={})", ue_cc.rnti, pid, ack ? "OK" : "KO");
    }
  };
  pending_events->enqueue_ue_cc_feedback("dl_ack_info", rnti, cc, callback);
}

void sched_nr::ul_crc_info(uint16_t rnti, uint32_t cc, uint32_t pid, bool crc)
{
  auto callback = [pid, crc](ue_carrier& ue_cc, event_manager::logger& ev_logger) {
    if (ue_cc.ul_crc_info(pid, crc) >= 0) {
      ev_logger.push("0x{:x}: ul_crc_info(pid={}, crc={})", ue_cc.rnti, pid, crc ? "OK" : "KO");
    }
  };
  pending_events->enqueue_ue_cc_feedback("ul_crc_info", rnti, cc, callback);
}

void sched_nr::ul_sr_info(uint16_t rnti)
{
  pending_events->enqueue_ue_event("ul_sr_info", rnti, [](ue& u, event_manager::logger& evlogger) {
    u.ul_sr_info();
    evlogger.push("0x{:x}: ul_sr_info()", u.rnti);
  });
}

void sched_nr::ul_bsr(uint16_t rnti, uint32_t lcg_id, uint32_t bsr)
{
  pending_events->enqueue_ue_event("ul_bsr", rnti, [lcg_id, bsr](ue& u, event_manager::logger& evlogger) {
    u.ul_bsr(lcg_id, bsr);
    evlogger.push("0x{:x}: ul_bsr(lcg={}, bsr={})", u.rnti, lcg_id, bsr);
  });
}

void sched_nr::dl_mac_ce(uint16_t rnti, uint32_t ce_lcid)
{
  pending_events->enqueue_ue_event("dl_mac_ce", rnti, [ce_lcid](ue& u, event_manager::logger& event_logger) {
    // CE is added to list of pending CE
    u.add_dl_mac_ce(ce_lcid, 1);
    event_logger.push("0x{:x}: dl_mac_ce(lcid={})", u.rnti, ce_lcid);
  });
}

void sched_nr::dl_buffer_state(uint16_t rnti, uint32_t lcid, uint32_t newtx, uint32_t retx)
{
  pending_events->enqueue_ue_event(
      "dl_buffer_state", rnti, [lcid, newtx, retx](ue& u, event_manager::logger& event_logger) {
        u.rlc_buffer_state(lcid, newtx, retx);
        event_logger.push("0x{:x}: dl_buffer_state(lcid={}, bsr={},{})", u.rnti, lcid, newtx, retx);
      });
}

void sched_nr::dl_cqi_info(uint16_t rnti, uint32_t cc, uint32_t cqi_value)
{
  auto callback = [cqi_value](ue_carrier& ue_cc, event_manager::logger& ev_logger) {
    ue_cc.dl_cqi = cqi_value;
    ev_logger.push("0x{:x}: dl_cqi_info(cqi={})", ue_cc.rnti, ue_cc.dl_cqi);
  };
  pending_events->enqueue_ue_cc_feedback("dl_cqi_info", rnti, cc, callback);
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
