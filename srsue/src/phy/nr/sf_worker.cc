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

#include "srsue/hdr/phy/nr/sf_worker.h"
#include "srsran/common/standard_streams.h"

#ifdef ENABLE_GUI
#include "srsgui/srsgui.h"
#include <semaphore.h>

static void      init_plots(srsue::nr::sf_worker* worker);
static pthread_t plot_thread;
static sem_t     plot_sem;
static int       plot_worker_id = -1;
#else
#pragma message "Compiling without srsGUI support"
#endif

namespace srsue {
namespace nr {

sf_worker::sf_worker(srsran::phy_common_interface& common_,
                     state&                        phy_state_,
                     const srsran::phy_cfg_nr_t&   cfg,
                     srslog::basic_logger&         log) :
  phy_state(phy_state_), common(common_), logger(log), sf_len(SRSRAN_SF_LEN_PRB_NR(cfg.carrier.nof_prb))
{
  for (uint32_t i = 0; i < phy_state.args.nof_carriers; i++) {
    cc_worker* w = new cc_worker(i, log, phy_state, cfg);
    cc_workers.push_back(std::unique_ptr<cc_worker>(w));
  }
}

void sf_worker::update_cfg(uint32_t cc_idx, const srsran::phy_cfg_nr_t& new_cfg)
{
  if (cc_idx >= cc_workers.size()) {
    return;
  }

  cc_workers[cc_idx]->update_cfg(new_cfg);
}

cf_t* sf_worker::get_buffer(uint32_t cc_idx, uint32_t antenna_idx)
{
  if (cc_idx >= cc_workers.size()) {
    return nullptr;
  }

  return cc_workers.at(cc_idx)->get_rx_buffer(antenna_idx);
}

uint32_t sf_worker::get_buffer_len()
{
  return cc_workers.at(0)->get_buffer_len();
}

void sf_worker::set_context(const srsran::phy_common_interface::worker_context_t& w_ctx)
{
  logger.set_context(w_ctx.sf_idx);
  for (auto& w : cc_workers) {
    w->set_tti(w_ctx.sf_idx);
  }
  context.copy(w_ctx);
}

void sf_worker::work_imp()
{
  srsran::rf_buffer_t tx_buffer = {};

  // Perform DL processing
  for (auto& w : cc_workers) {
    w->work_dl();
  }

  // Align workers, wait for previous workers to finish DL processing before starting UL processing
  phy_state.dl_ul_semaphore.wait(this);
  phy_state.dl_ul_semaphore.release();

  // Check if PRACH is available
  if (prach_ptr != nullptr) {
    // PRACH is available, set buffer, transmit and return
    tx_buffer.set(phy_state.args.rf_channel_offset, prach_ptr);
    tx_buffer.set_nof_samples(sf_len);

    // Transmit NR PRACH
    common.worker_end(context, true, tx_buffer);

    // Reset PRACH pointer
    prach_ptr = nullptr;

    return;
  }

  // Perform UL processing
  for (auto& w : cc_workers) {
    w.get()->work_ul();
  }

  // Set Tx buffers
  for (uint32_t i = 0; i < (uint32_t)cc_workers.size(); i++) {
    tx_buffer.set(i + phy_state.args.rf_channel_offset, cc_workers[i]->get_tx_buffer(0));
  }
  tx_buffer.set_nof_samples(sf_len);

  // Always call worker_end before returning
  common.worker_end(context, true, tx_buffer);

  // Tell the plotting thread to draw the plots
#ifdef ENABLE_GUI
  if ((int)get_id() == plot_worker_id) {
    sem_post(&plot_sem);
  }
#endif
}

int sf_worker::read_pdsch_d(cf_t* pdsch_d)
{
  return cc_workers[0]->read_pdsch_d(pdsch_d);
}

void sf_worker::start_plot()
{
#ifdef ENABLE_GUI
  if (plot_worker_id == -1) {
    plot_worker_id = get_id();
    srsran::console("Starting NR plot for worker_id=%d\n", plot_worker_id);
    init_plots(this);
  } else {
    srsran::console("Trying to start a plot but already started by worker_id=%d\n", plot_worker_id);
  }
#else
  srsran::console("Trying to start a plot but plots are disabled (ENABLE_GUI constant in sf_worker.cc)\n");
#endif
}

void sf_worker::set_prach(cf_t* prach_ptr_, float prach_power_)
{
  prach_ptr   = prach_ptr_;
  prach_power = prach_power_;
}

} // namespace nr
} // namespace srsue

#ifdef ENABLE_GUI
extern plot_scatter_t pconst_nr;
extern bool           pconst_nr_ready;
#define SCATTER_PDSCH_PLOT_LEN 4000
static cf_t tmp_pconst_nr[SRSRAN_NSYMB_PER_SLOT_NR * SRSRAN_NRE * SRSRAN_MAX_PRB_NR] = {};
extern bool plot_quit;

static void* plot_thread_run(void* arg)
{
  auto worker         = (srsue::nr::sf_worker*)arg;
  int  pdsch_re_count = 0;
  while (!plot_quit) {
    sem_wait(&plot_sem);

    if (pdsch_re_count < SCATTER_PDSCH_PLOT_LEN) {
      int n = worker->read_pdsch_d(&tmp_pconst_nr[pdsch_re_count]);
      pdsch_re_count += n;
    } else {
      if (pdsch_re_count > 0 and pconst_nr_ready) {
        plot_scatter_setNewData(&pconst_nr, tmp_pconst_nr, pdsch_re_count);
      }
      pdsch_re_count = 0;
    }
  }
  return nullptr;
}

static void init_plots(srsue::nr::sf_worker* worker)
{
  if (sem_init(&plot_sem, 0, 0)) {
    perror("sem_init");
    exit(-1);
  }

  pthread_attr_t     attr;
  struct sched_param param = {};
  param.sched_priority     = 0;
  pthread_attr_init(&attr);
  pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
  pthread_attr_setschedparam(&attr, &param);
  if (pthread_create(&plot_thread, &attr, plot_thread_run, worker)) {
    perror("pthread_create");
    exit(-1);
  }
}
#endif
