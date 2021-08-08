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

#include "srsran/common/threads.h"
#include "srsran/srsran.h"

#include "srsenb/hdr/phy/lte/sf_worker.h"

#define Error(fmt, ...)                                                                                                \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.debug(fmt, ##__VA_ARGS__)

using namespace std;

// Enable this to log SI
//#define LOG_THIS(a) 1

// Enable this one to skip SI-RNTI
#define LOG_THIS(rnti) (rnti != 0xFFFF)

/* Define GUI-related things */
#ifdef ENABLE_GUI
#include "srsgui/srsgui.h"
#include <semaphore.h>

#include "srsran/srsran.h"

void       init_plots(srsenb::lte::sf_worker* worker);
pthread_t  plot_thread;
sem_t      plot_sem;
static int plot_worker_id = -1;
#else
#pragma message "Compiling without srsGUI support"
#endif
/*********************************************/

using namespace asn1::rrc;

//#define DEBUG_WRITE_FILE

namespace srsenb {
namespace lte {

#ifdef DEBUG_WRITE_FILE
FILE* f;
#endif

void sf_worker::init(phy_common* phy_)
{
  phy = phy_;

  // Initialise each component carrier workers
  for (uint32_t i = 0; i < phy->get_nof_carriers_lte(); i++) {
    // Create pointer
    auto q = new cc_worker(logger);

    // Initialise
    q->init(phy, i);

    // Create unique pointer
    cc_workers.push_back(std::unique_ptr<cc_worker>(q));
  }

  if (srsran_softbuffer_tx_init(&temp_mbsfn_softbuffer, phy->get_nof_prb(0))) {
    ERROR("Error initiating soft buffer");
    exit(-1);
  }

  srsran_softbuffer_tx_reset(&temp_mbsfn_softbuffer);

  Info("Worker %d configured cell %d PRB", get_id(), phy->get_nof_prb(0));

  initiated = true;
  running   = true;

#ifdef DEBUG_WRITE_FILE
  f = fopen("test.dat", "w");
#endif
}

cf_t* sf_worker::get_buffer_rx(uint32_t cc_idx, uint32_t antenna_idx)
{
  return cc_workers[cc_idx]->get_buffer_rx(antenna_idx);
}

void sf_worker::set_context(const srsran::phy_common_interface::worker_context_t& w_ctx)
{
  tti_rx    = w_ctx.sf_idx;
  tti_tx_dl = TTI_ADD(tti_rx, FDD_HARQ_DELAY_UL_MS);
  tti_tx_ul = TTI_RX_ACK(tti_rx);

  context.copy(w_ctx);

  for (auto& w : cc_workers) {
    w->set_tti(w_ctx.sf_idx);
  }
}

int sf_worker::add_rnti(uint16_t rnti, uint32_t cc_idx)
{
  int ret = SRSRAN_ERROR;

  if (cc_idx < cc_workers.size()) {
    cc_workers[cc_idx]->add_rnti(rnti);
    ret = SRSRAN_SUCCESS;
  }

  return ret;
}

void sf_worker::rem_rnti(uint16_t rnti)
{
  for (auto& w : cc_workers) {
    w->rem_rnti(rnti);
  }
}

uint32_t sf_worker::get_nof_rnti()
{
  return cc_workers[0]->get_nof_rnti();
}

void sf_worker::work_imp()
{
  std::lock_guard<std::mutex> lock(work_mutex);

  srsran_ul_sf_cfg_t ul_sf = {};
  srsran_dl_sf_cfg_t dl_sf = {};

  // Get Transmission buffers
  srsran::rf_buffer_t tx_buffer = {};
  tx_buffer.set_nof_samples(SRSRAN_SF_LEN_PRB(phy->get_nof_prb(0)));

  if (!running) {
    phy->worker_end(context, true, tx_buffer);
    return;
  }

  srsran_mbsfn_cfg_t mbsfn_cfg;
  srsran_sf_t        sf_type = phy->is_mbsfn_sf(&mbsfn_cfg, tti_tx_dl) ? SRSRAN_SF_MBSFN : SRSRAN_SF_NORM;

  // Uplink grants to receive this TTI
  stack_interface_phy_lte::ul_sched_list_t ul_grants = phy->get_ul_grants(tti_rx);
  // Uplink grants to transmit this tti and receive in the future
  stack_interface_phy_lte::ul_sched_list_t ul_grants_tx = phy->get_ul_grants(tti_tx_ul);

  // Downlink grants to transmit this TTI
  stack_interface_phy_lte::dl_sched_list_t dl_grants(phy->get_nof_carriers_lte());

  stack_interface_phy_lte* stack = phy->stack;

  logger.set_context(tti_rx);

  Debug("Worker %d running", get_id());

  // Configure UL subframe
  ul_sf.tti = tti_rx;

  // Set UL grant availability prior to any UL processing
  if (phy->ue_db.set_ul_grant_available(tti_rx, ul_grants) < SRSRAN_SUCCESS) {
    Info("Failed setting UL grants. Some grant's RNTI does not exist.");
  }

  // Process UL
  for (uint32_t cc = 0; cc < cc_workers.size(); cc++) {
    cc_workers[cc]->work_ul(ul_sf, ul_grants[cc]);
  }

  // Get DL scheduling for the TX TTI from MAC
  if (sf_type == SRSRAN_SF_NORM) {
    if (stack->get_dl_sched(tti_tx_dl, dl_grants) < 0) {
      Error("Getting DL scheduling from MAC");
      phy->worker_end(context, true, tx_buffer);
      return;
    }
  } else {
    dl_grants[0].cfi = mbsfn_cfg.non_mbsfn_region_length;
    if (stack->get_mch_sched(tti_tx_dl, mbsfn_cfg.is_mcch, dl_grants)) {
      Error("Getting MCH packets from MAC");
      phy->worker_end(context, true, tx_buffer);
      return;
    }
  }

  // Get UL scheduling for the TX TTI from MAC
  if (stack->get_ul_sched(tti_tx_ul, ul_grants_tx) < 0) {
    Error("Getting UL scheduling from MAC");
    phy->worker_end(context, true, tx_buffer);
    return;
  }

  // Configure DL subframe
  dl_sf.tti              = tti_tx_dl;
  dl_sf.sf_type          = sf_type;
  dl_sf.non_mbsfn_region = mbsfn_cfg.non_mbsfn_region_length;

  // Prepare for receive ACK for DL grants in t_tx_dl+4
  phy->ue_db.clear_tti_pending_ack(tti_tx_ul);

  // Process DL
  for (uint32_t cc = 0; cc < cc_workers.size(); cc++) {
    // Select CFI and make sure it is in the right range
    dl_sf.cfi = dl_grants[cc].cfi;
    dl_sf.cfi = SRSRAN_MAX(dl_sf.cfi, 1);
    dl_sf.cfi = SRSRAN_MIN(dl_sf.cfi, 3);

    cc_workers[cc]->work_dl(dl_sf, dl_grants[cc], ul_grants_tx[cc], &mbsfn_cfg);
  }

  // Save grants
  phy->set_ul_grants(tti_tx_ul, ul_grants_tx);

  // Set or combine RF ports
  for (uint32_t cc = 0; cc < phy->get_nof_carriers_lte(); cc++) {
    for (uint32_t ant = 0; ant < phy->get_nof_ports(0); ant++) {
      tx_buffer.set_combine(phy->get_rf_port(cc), ant, phy->get_nof_ports(0), cc_workers[cc]->get_buffer_tx(ant));
    }
  }

  Debug("Sending to radio");
  phy->worker_end(context, true, tx_buffer);

#ifdef DEBUG_WRITE_FILE
  fwrite(signal_buffer_tx, SRSRAN_SF_LEN_PRB(phy->cell.nof_prb) * sizeof(cf_t), 1, f);
#endif

#ifdef DEBUG_WRITE_FILE
  if (tti_tx_dl == 10) {
    fclose(f);
    exit(-1);
  }
#endif

  /* Tell the plotting thread to draw the plots */
#ifdef ENABLE_GUI
  if ((int)get_id() == plot_worker_id) {
    sem_post(&plot_sem);
  }
#endif
}

/************ METRICS interface ********************/
uint32_t sf_worker::get_metrics(std::vector<phy_metrics_t>& metrics)
{
  uint32_t                   cnt = 0;
  std::vector<phy_metrics_t> metrics_;
  for (uint32_t cc = 0; cc < phy->get_nof_carriers_lte(); cc++) {
    cnt = cc_workers[cc]->get_metrics(metrics_);
    metrics.resize(std::max(metrics_.size(), metrics.size()));
    for (uint32_t r = 0; r < cnt; r++) {
      phy_metrics_t* m  = &metrics[r];
      phy_metrics_t* m_ = &metrics_[r];
      m->dl.mcs         = SRSRAN_VEC_PMA(m->dl.mcs, m->dl.n_samples, m_->dl.mcs, m_->dl.n_samples);
      m->dl.n_samples += m_->dl.n_samples;
      m->ul.n          = SRSRAN_VEC_PMA(m->ul.n, m->ul.n_samples, m_->ul.n, m_->ul.n_samples);
      m->ul.pusch_sinr = SRSRAN_VEC_PMA(m->ul.pusch_sinr, m->ul.n_samples, m_->ul.pusch_sinr, m_->ul.n_samples);
      m->ul.pucch_sinr =
          SRSRAN_VEC_PMA(m->ul.pucch_sinr, m->ul.n_samples_pucch, m_->ul.pucch_sinr, m_->ul.n_samples_pucch);
      m->ul.mcs         = SRSRAN_VEC_PMA(m->ul.mcs, m->ul.n_samples, m_->ul.mcs, m_->ul.n_samples);
      m->ul.rssi        = SRSRAN_VEC_PMA(m->ul.rssi, m->ul.n_samples, m_->ul.rssi, m_->ul.n_samples);
      m->ul.turbo_iters = SRSRAN_VEC_PMA(m->ul.turbo_iters, m->ul.n_samples, m_->ul.turbo_iters, m_->ul.n_samples);
      m->ul.n_samples += m_->ul.n_samples;
      m->ul.n_samples_pucch += m_->ul.n_samples_pucch;
    }
  }
  return cnt;
}

void sf_worker::start_plot()
{
#ifdef ENABLE_GUI
  if (plot_worker_id == -1) {
    plot_worker_id = get_id();
    srsran::console("Starting plot for worker_id=%d\n", plot_worker_id);
    init_plots(this);
  } else {
    srsran::console("Trying to start a plot but already started by worker_id=%d\n", plot_worker_id);
  }
#else
  srsran::console("Trying to start a plot but plots are disabled (ENABLE_GUI constant in sf_worker.cc)\n");
#endif
}

uint32_t sf_worker::get_nof_carriers()
{
  return phy->get_nof_carriers_lte();
}
int sf_worker::get_carrier_pci(uint32_t cc_idx)
{
  return phy->get_cell(cc_idx).id;
}
int sf_worker::read_ce_abs(uint32_t cc_idx, float* ce_abs)
{
  return cc_workers[cc_idx]->read_ce_abs(ce_abs);
}

int sf_worker::read_ce_arg(uint32_t cc_idx, float* ce_arg)
{
  return cc_workers[cc_idx]->read_ce_arg(ce_arg);
}

int sf_worker::read_pusch_d(uint32_t cc_idx, cf_t* pdsch_d)
{
  return cc_workers[cc_idx]->read_pusch_d(pdsch_d);
}

int sf_worker::read_pucch_d(uint32_t cc_idx, cf_t* pdsch_d)
{
  return cc_workers[cc_idx]->read_pucch_d(pdsch_d);
}

sf_worker::~sf_worker()
{
  srsran_softbuffer_tx_free(&temp_mbsfn_softbuffer);
}

} // namespace lte
} // namespace srsenb

/***********************************************************
 *
 * PLOT TO VISUALIZE THE CHANNEL RESPONSEE
 *
 ***********************************************************/

#ifdef ENABLE_GUI
struct plot_cc_s {
  plot_real_t    pce, pce_arg;
  plot_scatter_t pconst;
  plot_scatter_t pconst2;
};
static std::map<uint32_t, plot_cc_s> plots;
#define SCATTER_PUSCH_BUFFER_LEN (20 * 6 * SRSRAN_SF_LEN_RE(SRSRAN_MAX_PRB, SRSRAN_CP_NORM))
static float tmp_plot[SCATTER_PUSCH_BUFFER_LEN];
static float tmp_plot_arg[SCATTER_PUSCH_BUFFER_LEN];
static cf_t  tmp_plot2[SRSRAN_SF_LEN_RE(SRSRAN_MAX_PRB, SRSRAN_CP_NORM)];
static cf_t  tmp_pucch_plot[SRSRAN_PUCCH_MAX_BITS / 2];

void* plot_thread_run(void* arg)
{
  auto worker = (srsenb::lte::sf_worker*)arg;

  for (uint32_t cc_idx = 0; cc_idx < worker->get_nof_carriers(); cc_idx++) {
    plot_cc_s& p = plots[cc_idx];

    char title[32] = {};
    snprintf(title, sizeof(title), "srsENB PCI %d", worker->get_carrier_pci(cc_idx));

    printf("Creating plot window '%s'...\n", title);

    sdrgui_init_title(title);

    plot_real_init(&p.pce);
    plot_real_setTitle(&p.pce, (char*)"Channel Response - Magnitude");
    plot_real_setLabels(&p.pce, (char*)"Index", (char*)"dB");
    plot_real_setYAxisScale(&p.pce, -40, 40);
    plot_real_addToWindowGrid(&p.pce, title, 0, 0);

    plot_real_init(&p.pce_arg);
    plot_real_setTitle(&p.pce_arg, (char*)"Channel Response - Argument");
    plot_real_setLabels(&p.pce_arg, (char*)"Angle", (char*)"deg");
    plot_real_setYAxisScale(&p.pce_arg, -180, 180);
    plot_real_addToWindowGrid(&p.pce_arg, title, 1, 0);

    plot_scatter_init(&p.pconst);
    plot_scatter_setTitle(&p.pconst, (char*)"PUSCH - Equalized Symbols");
    plot_scatter_setXAxisScale(&p.pconst, -4, 4);
    plot_scatter_setYAxisScale(&p.pconst, -4, 4);
    plot_scatter_addToWindowGrid(&p.pconst, title, 0, 1);

    plot_scatter_init(&p.pconst2);
    plot_scatter_setTitle(&p.pconst2, (char*)"PUCCH - Equalized Symbols");
    plot_scatter_setXAxisScale(&p.pconst2, -4, 4);
    plot_scatter_setYAxisScale(&p.pconst2, -4, 4);
    plot_scatter_addToWindowGrid(&p.pconst2, title, 1, 1);
  }

  int n, n_arg, n_pucch;
  while (true) {
    sem_wait(&plot_sem);

    for (auto& e : plots) {
      uint32_t   cc_idx = e.first;
      plot_cc_s& p      = e.second;

      n       = worker->read_pusch_d(cc_idx, tmp_plot2);
      n_pucch = worker->read_pucch_d(cc_idx, tmp_pucch_plot);
      plot_scatter_setNewData(&p.pconst, tmp_plot2, n);
      plot_scatter_setNewData(&p.pconst2, tmp_pucch_plot, n_pucch);

      n = worker->read_ce_abs(cc_idx, tmp_plot);
      plot_real_setNewData(&p.pce, tmp_plot, n);

      n_arg = worker->read_ce_arg(cc_idx, tmp_plot_arg);
      plot_real_setNewData(&p.pce_arg, tmp_plot_arg, n_arg);
    }
  }
  return nullptr;
}

void init_plots(srsenb::lte::sf_worker* worker)
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
