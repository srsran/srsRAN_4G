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

#include "srsue/hdr/phy/sync.h"
#include "srsran/common/standard_streams.h"
#include "srsran/phy/channel/channel.h"
#include "srsran/srsran.h"
#include "srsue/hdr/phy/lte/sf_worker.h"

#include <algorithm>
#include <unistd.h>

#define Error(fmt, ...)                                                                                                \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  phy_logger.error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  phy_logger.warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  phy_logger.info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  phy_logger.debug(fmt, ##__VA_ARGS__)

namespace srsue {

static int
radio_recv_callback(void* obj, cf_t* data[SRSRAN_MAX_CHANNELS], uint32_t nsamples, srsran_timestamp_t* rx_time)
{
  srsran::rf_buffer_t x(data, nsamples);
  return ((sync*)obj)->radio_recv_fnc(x, rx_time);
}

static SRSRAN_AGC_CALLBACK(callback_set_rx_gain)
{
  ((sync*)h)->set_rx_gain(gain_db);
}

void sync::init(srsran::radio_interface_phy* _radio,
                stack_interface_phy_lte*     _stack,
                prach*                       _prach_buffer,
                lte::worker_pool*            _lte_workers_pool,
                nr::worker_pool*             _nr_workers_pool,
                phy_common*                  _worker_com,
                uint32_t                     prio,
                int                          sync_cpu_affinity)
{
  radio_h         = _radio;
  stack           = _stack;
  lte_worker_pool = _lte_workers_pool;
  nr_worker_pool  = _nr_workers_pool;
  worker_com      = _worker_com;
  prach_buffer    = _prach_buffer;

  nof_rf_channels =
      (worker_com->args->nof_lte_carriers + worker_com->args->nof_nr_carriers) * worker_com->args->nof_rx_ant;
  if (nof_rf_channels == 0 || nof_rf_channels > SRSRAN_MAX_CHANNELS) {
    Error("SYNC:  Invalid number of RF channels (%d)", nof_rf_channels);
    return;
  }

  if (srsran_ue_sync_init_multi(&ue_sync, SRSRAN_MAX_PRB, false, radio_recv_callback, nof_rf_channels, this) !=
      SRSRAN_SUCCESS) {
    Error("SYNC:  Initiating ue_sync");
    return;
  }
  srsran_ue_sync_cp_en(&ue_sync, worker_com->args->detect_cp);

  if (worker_com->args->dl_channel_args.enable) {
    channel_emulator =
        srsran::channel_ptr(new srsran::channel(worker_com->args->dl_channel_args, nof_rf_channels, phy_logger));
  }

  // Initialize cell searcher
  search_p.init(sf_buffer, nof_rf_channels, this, worker_com->args->force_N_id_2, worker_com->args->force_N_id_1);
  search_p.set_cp_en(worker_com->args->detect_cp);
  // Initialize SFN synchronizer, it uses only pcell buffer
  sfn_p.init(&ue_sync, worker_com->args, sf_buffer, sf_buffer.size());

  // Start intra-frequency measurement
  {
    std::lock_guard<std::mutex> lock(intra_freq_cfg_mutex);
    for (uint32_t i = 0; i < worker_com->args->nof_lte_carriers; i++) {
      scell::intra_measure_lte*         q    = new scell::intra_measure_lte(phy_logger, *this);
      scell::intra_measure_base::args_t args = {};
      args.len_ms                            = worker_com->args->intra_freq_meas_len_ms;
      args.period_ms                         = worker_com->args->intra_freq_meas_period_ms;
      args.rx_gain_offset_db                 = worker_com->args->rx_gain_offset;
      q->init(i, args);
      intra_freq_meas.push_back(std::unique_ptr<scell::intra_measure_lte>(q));
    }
  }

  // Allocate Secondary serving cell synchronization
  for (uint32_t i = 1; i < worker_com->args->nof_lte_carriers; i++) {
    // Give the logical channel
    scell_sync[i] = std::unique_ptr<scell::sync>(new scell::sync(this, i * worker_com->args->nof_rx_ant));
  }

  reset();
  running = true;

  // Enable AGC for primary cell receiver
  set_agc_enable(worker_com->args->agc_enable);

  // Start main thread
  if (sync_cpu_affinity < 0) {
    start(prio);
  } else {
    start_cpu(prio, sync_cpu_affinity);
  }
}

sync::~sync()
{
  srsran_ue_sync_free(&ue_sync);
}

void sync::stop()
{
  running = false;

  std::lock_guard<std::mutex> lock(intra_freq_cfg_mutex);
  for (auto& q : intra_freq_meas) {
    q->stop();
  }

  // Reset (stop Rx stream) as soon as possible to avoid base-band Rx buffer overflow
  radio_h->reset();

  // let the sync FSM finish before waiting for workers semaphores to avoid workers locking
  wait_thread_finish();
  worker_com->semaphore.wait_all();
}

void sync::reset()
{
  in_sync_cnt     = 0;
  out_of_sync_cnt = 0;
  current_earfcn  = -1;
  srate.reset();
  sfn_p.reset();
  search_p.reset();
}

/**
 * Higher layers API.
 *
 * These functions are called by higher layers (RRC) to control the Cell search and cell selection procedures.
 * They manipulate the SYNC state machine to switch states and perform different actions. In order to ensure mutual
 * exclusion any change of state variables such as cell configuration, MIB decoder, etc. must be done while the
 * SYNC thread is in IDLE.
 *
 * Functions will manipulate the SYNC state machine (sync_state class) to jump to states and wait for result then
 * return the result to the higher layers.
 *
 * Cell Search:
 *  It's the process of searching for cells in the bands or set of EARFCNs supported by the UE. Cell search is performed
 *  at 1.92 MHz sampling rate and involves PSS/SSS synchronization (PCI extraction) and MIB decoding for number of Ports
 * and PRB.
 *
 *
 * Cell Select:
 *  It's the process of moving the cell state from IDLE->CAMPING or from CAMPING->IDLE->CAMPING when RRC indicates to
 *  select a different cell.
 *
 *  If it is a new cell, the reconfiguration must take place while sync_state is on IDLE.
 *
 *  cell_search() and cell_select() functions can not be called concurrently. A mutex is used to prevent it from
 * happening.
 *
 */

/* A call to cell_search() finds the strongest cell at a given EARFCN or in the set of supported EARFCNs. When the first
 * cell is found, returns 1 and stores cell information and RSRP values in the pointers (if provided). If a cell is not
 * found in the current frequency it moves to the next one and the next call to cell_search() will look in the next
 * EARFCN in the set. If no cells are found in any frequency it returns 0. If error returns -1.
 *
 * The first part of the procedure (call to _init()) moves the PHY To IDLE, ensuring that no UL/DL/PRACH will happen
 *
 */
bool sync::cell_search_init()
{
  std::unique_lock<std::mutex> ul(rrc_mutex);

  if (rrc_proc_state != PROC_IDLE) {
    Error("Cell Search: Can't start procedure. SYNC already running a procedure (%d)", (uint32_t)rrc_proc_state);
    return false;
  }

  // Move state to IDLE
  phy_state.go_idle();

  // Stop all intra-frequency measurement before changing frequency
  meas_stop();

  rrc_proc_state = PROC_SEARCH_START;

  return true;
}

rrc_interface_phy_lte::cell_search_ret_t sync::cell_search_start(phy_cell_t* found_cell, int earfcn)
{
  std::unique_lock<std::mutex> ul(rrc_mutex);

  if (earfcn < 0) {
    Info("Cell Search: Start EARFCN index=%u/%zd", cellsearch_earfcn_index, worker_com->args->dl_earfcn_list.size());
  } else {
    Info("Cell Search: Start EARFCN=%d", earfcn);
  }

  rrc_interface_phy_lte::cell_search_ret_t ret = {};
  ret.found                                    = rrc_interface_phy_lte::cell_search_ret_t::ERROR;
  ret.last_freq                                = rrc_interface_phy_lte::cell_search_ret_t::NO_MORE_FREQS;

  if (rrc_proc_state != PROC_SEARCH_START) {
    Error("Cell Search: Can't run procedure. Must call cell_search_init() first (%d)", (uint32_t)rrc_proc_state);
    return ret;
  }

  rrc_proc_state = PROC_SEARCH_RUNNING;

  if (srate.set_find()) {
    radio_h->set_rx_srate(1.92e6);
    radio_h->set_tx_srate(1.92e6);
    Info("SYNC:  Setting Cell Search sampling rate");
  }

  if (earfcn < 0) {
    try {
      if (current_earfcn != (int)worker_com->args->dl_earfcn_list.at(cellsearch_earfcn_index)) {
        current_earfcn = (int)worker_com->args->dl_earfcn_list[cellsearch_earfcn_index];
      }
    } catch (const std::out_of_range& oor) {
      Error("Index %d is not a valid EARFCN element.", cellsearch_earfcn_index);
      return ret;
    }
  } else {
    current_earfcn = earfcn;
  }
  Info("Cell Search: changing frequency to EARFCN=%d", current_earfcn);
  set_frequency();

  // Move to CELL SEARCH and wait to finish
  Info("Cell Search: Setting Cell search state");
  phy_state.run_cell_search();

  // Check return state
  switch (cell_search_ret) {
    case search::CELL_FOUND:
      phy_logger.info("Cell Search: Found cell with PCI=%d with %d PRB", cell.get().id, cell.get().nof_prb);
      if (found_cell) {
        found_cell->earfcn = current_earfcn;
        found_cell->pci    = cell.get().id;
        found_cell->cfo_hz = search_p.get_last_cfo();
      }
      ret.found = rrc_interface_phy_lte::cell_search_ret_t::CELL_FOUND;
      break;
    case search::CELL_NOT_FOUND:
      Info("Cell Search: No cell found in this frequency");
      ret.found = rrc_interface_phy_lte::cell_search_ret_t::CELL_NOT_FOUND;
      break;
    default:
      Error("Cell Search: while receiving samples");
      radio_error();
      break;
  }

  cellsearch_earfcn_index++;
  if (cellsearch_earfcn_index >= worker_com->args->dl_earfcn_list.size() or earfcn < 0) {
    Info("Cell Search: No more frequencies in the current EARFCN set");
    cellsearch_earfcn_index = 0;
    ret.last_freq           = rrc_interface_phy_lte::cell_search_ret_t::NO_MORE_FREQS;
  } else {
    ret.last_freq = rrc_interface_phy_lte::cell_search_ret_t::MORE_FREQS;
  }

  rrc_proc_state = PROC_IDLE;

  return ret;
}

/* Cell select synchronizes to a new cell (e.g. during HO or during cell reselection on IDLE) or
 * re-synchronizes with the current cell if cell argument is NULL
 * The first phase of the procedure verifies the validity of the input parameters and switches the
 * PHY to IDLE. Once this function returns, the PHY will not process any DL/UL nor will PRACH
 *
 * See cell_select_start()
 */
bool sync::cell_select_init(phy_cell_t new_cell)
{
  std::unique_lock<std::mutex> ul(rrc_mutex);

  if (rrc_proc_state != PROC_IDLE) {
    Error("Cell Select: Can't start procedure. SYNC already running a procedure (%d)", (uint32_t)rrc_proc_state);
    return false;
  }

  // Move state to IDLE
  if (!srsran_cellid_isvalid(new_cell.pci)) {
    Error("Cell Select: Invalid cell_id=%d", new_cell.pci);
    return false;
  }

  Info("Cell Select: Going to IDLE");
  phy_state.go_idle();

  // Stop intra-frequency measurements if need to change frequency
  if ((int)new_cell.earfcn != current_earfcn) {
    meas_stop();
  }

  rrc_proc_state = PROC_SELECT_START;

  return true;
}

bool sync::cell_select_start(phy_cell_t new_cell)
{
  std::unique_lock<std::mutex> ul(rrc_mutex);

  bool ret = false;
  if (rrc_proc_state != PROC_SELECT_START) {
    Error("Cell Select: Can't run procedure. Must call cell_select_init() first (%d)", (uint32_t)rrc_proc_state);
    goto clean_exit;
  }

  rrc_proc_state = PROC_SELECT_RUNNING;

  // Reset SFN and cell search FSMs. They can safely be done while it is CAMPING or IDLE
  sfn_p.reset();
  search_p.reset();

  // Reconfigure cell if necessary
  cell.set_pci(new_cell.pci);
  if (not set_cell(new_cell.cfo_hz)) {
    Error("Cell Select: Reconfiguring cell");
    goto clean_exit;
  }

  /* Select new frequency if necessary */
  if ((int)new_cell.earfcn != current_earfcn) {
    current_earfcn = new_cell.earfcn;

    Info("Cell Select: Setting new frequency EARFCN=%d", new_cell.earfcn);
    if (!set_frequency()) {
      Error("Cell Select: Setting new frequency EARFCN=%d", new_cell.earfcn);
      goto clean_exit;
    }
  }

  // Reconfigure first intra-frequency measurement
  intra_freq_meas[0]->set_primary_cell(current_earfcn, cell.get());

  // Reconfigure secondary serving cell synchronization assuming the same BW than the primary
  // The secondary serving cell synchronization will not resize again when the SCell gets configured
  for (auto& e : scell_sync) {
    e.second->set_bw(cell.get().nof_prb);
  }

  // Change sampling rate if necessary
  set_sampling_rate();

  // SFN synchronization
  phy_state.run_sfn_sync();
  if (phy_state.is_camping()) {
    Info("Cell Select: SFN synchronized. CAMPING...");
    stack->in_sync();
    ret = true;
  } else {
    Info("Cell Select: Could not synchronize SFN");
  }

clean_exit:
  rrc_proc_state = PROC_IDLE;
  return ret;
}

bool sync::cell_is_camping()
{
  return phy_state.is_camping();
}

bool sync::wait_idle()
{
  // Wait for SYNC thread to transition to IDLE (max. 2000ms)
  if (not phy_state.wait_idle(TIMEOUT_TO_IDLE_MS)) {
    Error("SYNC: Failed transitioning to IDLE");
    return false;
  }

  // Reset UE sync. Attention: doing this reset when the FSM is NOT IDLE can cause PSS/SSS out-of-sync
  srsran_ue_sync_reset(&ue_sync);

  // Wait for workers to finish PHY processing
  worker_com->semaphore.wait_all();

  // As workers have finished, make sure the Tx burst is ended
  radio_h->tx_end();

  return true;
}

void sync::run_cell_search_state()
{
  srsran_cell_t tmp_cell = cell.get();
  cell_search_ret        = search_p.run(&tmp_cell, mib);
  if (cell_search_ret == search::CELL_FOUND) {
    cell.set(tmp_cell);
    stack->bch_decoded_ok(SYNC_CC_IDX, mib.data(), mib.size() / 8);
  }
  phy_state.state_exit();
}

void sync::run_sfn_sync_state()
{
  srsran_cell_t old_cell = cell.get();
  switch (sfn_p.run_subframe(&old_cell, &tti, mib)) {
    case sfn_sync::SFN_FOUND:
      if (!cell.equals(old_cell)) {
        srsran_cell_fprint(stdout, &old_cell, 0);
        phy_logger.error("Detected cell during SFN synchronization differs from configured cell. Cell reselection to "
                         "cells with different MIB is not supported");
        srsran::console("Detected cell during SFN synchronization differs from configured cell. Cell reselection "
                        "to cells with different MIB is not supported\n");
        phy_state.state_exit(false);
      }
      stack->in_sync();
      phy_state.state_exit();
      break;
    case sfn_sync::IDLE:
      break;
    default:
      phy_state.state_exit(false);
      break;
  }
}

void sync::run_camping_in_sync_state(lte::sf_worker*      lte_worker,
                                     nr::sf_worker*       nr_worker,
                                     srsran::rf_buffer_t& sync_buffer)
{
  // Update logging TTI
  phy_lib_logger.set_context(tti);
  phy_logger.set_context(tti);

  // Check tti is synched with ue_sync
  if (srsran_ue_sync_get_sfidx(&ue_sync) != tti % 10) {
    uint32_t sfn = tti / 10;
    tti          = (sfn * 10 + srsran_ue_sync_get_sfidx(&ue_sync)) % 10240;

    // Force SFN decode, just in case it is in the wrong frame
    force_camping_sfn_sync = true;
  }

  // Run secondary serving cell synchronization
  for (auto& e : scell_sync) {
    e.second->run(tti, sync_buffer.get(e.first, 0, worker_com->args->nof_rx_ant));
  }

  if (is_overflow) {
    force_camping_sfn_sync = true;
    is_overflow            = false;
    phy_logger.info("Detected overflow, trying to resync SFN");
  }

  // Force decode MIB if required
  if (force_camping_sfn_sync) {
    uint32_t           _tti      = 0;
    srsran_cell_t      temp_cell = cell.get();
    sfn_sync::ret_code ret       = sfn_p.decode_mib(&temp_cell, &_tti, &sync_buffer, mib);

    if (ret == sfn_sync::SFN_FOUND) {
      // Force tti
      tti = _tti;

      // Disable
      force_camping_sfn_sync = false;

      if (!cell.equals(temp_cell)) {
        phy_logger.error("Detected cell during SFN synchronization differs from configured cell. Cell "
                         "reselection to cells with different MIB is not supported");
        srsran::console("Detected cell during SFN synchronization differs from configured cell. Cell "
                        "reselection to cells with different MIB is not supported\n");
      } else {
        phy_logger.info("SFN resynchronized successfully");
      }
    } else {
      phy_logger.warning("SFN not yet synchronized, sending out-of-sync");
    }
  }

  Debug("SYNC:  Worker %d synchronized", lte_worker->get_id());

  // Collect and provide metrics from last successful sync
  metrics.sfo         = sfo;
  metrics.cfo         = cfo;
  metrics.ta_us       = worker_com->ta.get_usec();
  metrics.distance_km = worker_com->ta.get_km();
  metrics.speed_kmph  = worker_com->ta.get_speed_kmph(tti);
  for (uint32_t i = 0; i < worker_com->args->nof_lte_carriers; i++) {
    worker_com->set_sync_metrics(i, metrics);
  }

  // Check if we need to TX a PRACH
  if (prach_buffer->is_ready_to_send(tti, cell.get().id)) {
    prach_ptr = prach_buffer->generate(get_tx_cfo(), &prach_nof_sf, &prach_power);
    if (prach_ptr == nullptr) {
      Error("Generating PRACH");
    }
  }

  lte_worker->set_prach(prach_ptr ? &prach_ptr[prach_sf_cnt * SRSRAN_SF_LEN_PRB(cell.get().nof_prb)] : nullptr,
                        prach_power);

  // Execute Serving Cell state FSM
  worker_com->cell_state.run_tti(tti);

  // Set CFO for all Carriers
  for (uint32_t cc = 0; cc < worker_com->args->nof_lte_carriers; cc++) {
    lte_worker->set_cfo_nolock(cc, get_tx_cfo());
    worker_com->update_cfo_measurement(cc, cfo);
  }

  // Compute TX time: Any transmission happens in TTI+4 thus advance 4 ms the reception time
  last_rx_time.add(FDD_HARQ_DELAY_DL_MS * 1e-3);

  // Advance/reset prach subframe pointer
  if (prach_ptr) {
    prach_sf_cnt++;
    if (prach_sf_cnt == prach_nof_sf) {
      prach_sf_cnt = 0;
      prach_ptr    = nullptr;
    }
  }

  // Set NR worker context and start
  if (nr_worker != nullptr) {
    srsran::phy_common_interface::worker_context_t context;
    context.sf_idx     = tti;
    context.worker_ptr = nr_worker;
    context.last       = (lte_worker == nullptr); // Set last if standalone
    context.tx_time.copy(last_rx_time);

    nr_worker->set_context(context);

    // As UE sync compensates CFO externally based on LTE signal and the NR carrier may estimate the CFO from the LTE
    // signal. It is necessary setting an NR external CFO offset to compensate it.
    nr_worker_pool->set_ul_ext_cfo(srsran_ue_sync_get_cfo(&ue_sync));

    // NR worker needs to be launched first, phy_common::worker_end expects first the NR worker and the LTE worker.
    worker_com->semaphore.push(nr_worker);
    nr_worker_pool->start_worker(nr_worker);
  }

  // Set LTE worker context and start
  if (lte_worker != nullptr) {
    srsran::phy_common_interface::worker_context_t context;
    context.sf_idx     = tti;
    context.worker_ptr = lte_worker;
    context.last       = true;
    context.tx_time.copy(last_rx_time);

    lte_worker->set_context(context);

    // NR worker needs to be launched first, phy_common::worker_end expects first the NR worker and the LTE worker.
    worker_com->semaphore.push(lte_worker);
    lte_worker_pool->start_worker(lte_worker);
  }
}
void sync::run_camping_state()
{
  lte::sf_worker*     lte_worker  = lte_worker_pool->wait_worker(tti);
  srsran::rf_buffer_t sync_buffer = {};

  if (lte_worker == nullptr) {
    // wait_worker() only returns NULL if it's being closed. Quit now to avoid unnecessary loops here
    running = false;
    return;
  }

  nr::sf_worker* nr_worker = nullptr;
  if (worker_com->args->nof_nr_carriers > 0) {
    nr_worker = nr_worker_pool->wait_worker(tti);
    if (nr_worker == nullptr) {
      running = false;
      return;
    }
  }

  // Map carrier/antenna buffers to worker buffers
  uint32_t cc_idx = 0;
  for (uint32_t lte_cc_idx = 0; lte_cc_idx < worker_com->args->nof_lte_carriers; lte_cc_idx++, cc_idx++) {
    for (uint32_t i = 0; i < worker_com->args->nof_rx_ant; i++) {
      sync_buffer.set(cc_idx, i, worker_com->args->nof_rx_ant, lte_worker->get_buffer(lte_cc_idx, i));
    }
  }
  for (uint32_t nr_cc_idx = 0; nr_cc_idx < worker_com->args->nof_nr_carriers; nr_cc_idx++, cc_idx++) {
    for (uint32_t i = 0; i < worker_com->args->nof_rx_ant; i++) {
      sync_buffer.set(cc_idx, i, worker_com->args->nof_rx_ant, nr_worker->get_buffer(nr_cc_idx, i));
    }
  }

  // Apply CFO adjustment if available
  if (ref_cfo != 0.0) {
    srsran_ue_sync_set_cfo_ref(&ue_sync, ref_cfo);
    ref_cfo = 0.0; // reset until value changes again
  }

  // Primary Cell (PCell) Synchronization
  int sync_result = srsran_ue_sync_zerocopy(&ue_sync, sync_buffer.to_cf_t(), lte_worker->get_buffer_len());
  cfo             = srsran_ue_sync_get_cfo(&ue_sync);
  sfo             = srsran_ue_sync_get_sfo(&ue_sync);

  switch (sync_result) {
    case 1:
      run_camping_in_sync_state(lte_worker, nr_worker, sync_buffer);
      break;
    case 0:
      Warning("SYNC:  Out-of-sync detected in PSS/SSS");
      out_of_sync();
      lte_worker->release();

      // Force decoding MIB, for making sure that the TTI will be right
      if (!force_camping_sfn_sync) {
        force_camping_sfn_sync = true;
      }

      break;
    default:
      radio_error();
      lte_worker->release();
      break;
  }

  // Run stack
  Debug("run_stack_tti: from main");
  run_stack_tti();
}

void sync::run_idle_state()
{
  if (radio_h->is_init()) {
    uint32_t nsamples = 1920;
    if (srate.is_normal()) {
      nsamples = srate.get_srate() / 1000;
    }
    Debug("Discarding %d samples", nsamples);
    srsran_timestamp_t rx_time = {};
    dummy_buffer.set_nof_samples(nsamples);
    if (radio_recv_fnc(dummy_buffer, &rx_time) == SRSRAN_SUCCESS) {
      srsran::console("SYNC:  Receiving from radio while in IDLE_RX\n");
    }
    // If radio is in locked state returns immediately. In that case, do a 1 ms sleep
    if (rx_time.frac_secs == 0 && rx_time.full_secs == 0) {
      usleep(1000);
    }
    radio_h->tx_end();
  } else {
    Debug("Sleeping");
    usleep(1000);
  }
}

void sync::run_thread()
{
  while (running.load(std::memory_order_relaxed)) {
    phy_lib_logger.set_context(tti);

    Debug("SYNC:  state=%s, tti=%d", phy_state.to_string(), tti);

    // If not camping, clear SFN sync
    if (!phy_state.is_camping()) {
      force_camping_sfn_sync = false;
    }

    switch (phy_state.run_state()) {
      case sync_state::CELL_SEARCH:
        run_cell_search_state();
        break;
      case sync_state::SFN_SYNC:
        run_sfn_sync_state();
        break;
      case sync_state::CAMPING:
        run_camping_state();
        break;
      case sync_state::IDLE:
        run_idle_state();
        break;
    }

    // Increase TTI counter
    tti = TTI_ADD(tti, 1);
  }
}

/***************
 *
 * Utility functions called by the main thread or by functions called by other threads
 *
 */
void sync::radio_overflow()
{
  is_overflow = true;
}

void sync::radio_error()
{
  phy_logger.error("SYNC:  Receiving from radio.");
  // Need to find a method to effectively reset radio, reloading the driver does not work
  radio_h->reset();
}

void sync::in_sync()
{
  in_sync_cnt++;
  // Send RRC in-sync signal after 100 ms consecutive subframes
  if (in_sync_cnt == worker_com->args->nof_in_sync_events) {
    stack->in_sync();
    in_sync_cnt     = 0;
    out_of_sync_cnt = 0;
  }
}

// Out of sync called by worker or sync every 1 or 5 ms
void sync::out_of_sync()
{
  // Send RRC out-of-sync signal after NOF_OUT_OF_SYNC_SF consecutive subframes
  Info("Out-of-sync %d/%d", out_of_sync_cnt.load(std::memory_order_relaxed), worker_com->args->nof_out_of_sync_events);
  out_of_sync_cnt++;
  if (out_of_sync_cnt == worker_com->args->nof_out_of_sync_events) {
    Info("Sending to RRC");
    stack->out_of_sync();
    out_of_sync_cnt = 0;
    in_sync_cnt     = 0;
  }
}

void sync::set_cfo(float cfo_)
{
  ref_cfo = cfo_;
}

void sync::set_agc_enable(bool enable)
{
  // If not enabled, make sure it is not used and return
  if (not enable) {
    ue_sync.do_agc = false;
    return;
  }

  // Early return if the AGC is enabled
  if (ue_sync.do_agc) {
    return;
  }

  // PHY and radio must have been initialised
  if (not running or radio_h == nullptr) {
    ERROR("Error setting AGC: PHY not initiated");
    return;
  }

  // Get radio info and check it is valid
  srsran_rf_info_t* rf_info = radio_h->get_info();
  if (rf_info == nullptr) {
    Error("Error: Radio does not provide RF information");
    return;
  }

  // Enable AGC (unprotected call to ue_sync must not happen outside of thread calling recv)
  srsran_ue_sync_start_agc(
      &ue_sync, callback_set_rx_gain, rf_info->min_rx_gain, rf_info->max_rx_gain, radio_h->get_rx_gain());
  search_p.set_agc_enable(true);
}

float sync::get_tx_cfo()
{
  // Use CFO estimate from last successful sync
  float ret = cfo * ul_dl_factor;

  if (worker_com->args->cfo_is_doppler) {
    ret *= -1;
  } else {
    /* Compensates the radio frequency offset applied equally to DL and UL. Does not work in doppler mode */
    if (radio_h->get_freq_offset() != 0.0f) {
      const float offset_hz = (float)radio_h->get_freq_offset() * (1.0f - ul_dl_factor);
      ret                   = cfo - offset_hz;
    }
  }

  return ret / 15000;
}

void sync::set_ue_sync_opts(srsran_ue_sync_t* q, float cfo_)
{
  if (worker_com->args->cfo_integer_enabled) {
    srsran_ue_sync_set_cfo_i_enable(q, true);
  }

  srsran_ue_sync_set_cfo_ema(q, worker_com->args->cfo_pss_ema);
  srsran_ue_sync_set_cfo_tol(q, worker_com->args->cfo_correct_tol_hz);
  srsran_ue_sync_set_cfo_loop_bw(q,
                                 worker_com->args->cfo_loop_bw_pss,
                                 worker_com->args->cfo_loop_bw_ref,
                                 worker_com->args->cfo_loop_pss_tol,
                                 worker_com->args->cfo_loop_ref_min,
                                 worker_com->args->cfo_loop_pss_tol,
                                 worker_com->args->cfo_loop_pss_conv);

  // Disable CP based CFO estimation during find
  if (std::isnormal(cfo_)) {
    srsran_ue_sync_cfo_reset(q, cfo_);
    q->cfo_current_value       = cfo_ / 15000;
    q->cfo_is_copied           = true;
    q->cfo_correct_enable_find = true;
    srsran_sync_set_cfo_cp_enable(&q->sfind, false, 0);
  }

  // Set SFO ema and correct period
  srsran_ue_sync_set_sfo_correct_period(q, worker_com->args->sfo_correct_period);
  srsran_ue_sync_set_sfo_ema(q, worker_com->args->sfo_ema);

  sss_alg_t sss_alg = SSS_FULL;
  if (!worker_com->args->sss_algorithm.compare("diff")) {
    sss_alg = SSS_DIFF;
  } else if (!worker_com->args->sss_algorithm.compare("partial")) {
    sss_alg = SSS_PARTIAL_3;
  } else if (!worker_com->args->sss_algorithm.compare("full")) {
    sss_alg = SSS_FULL;
  } else {
    Warning("SYNC:  Invalid SSS algorithm %s. Using 'full'", worker_com->args->sss_algorithm.c_str());
  }
  srsran_sync_set_sss_algorithm(&q->strack, (sss_alg_t)sss_alg);
  srsran_sync_set_sss_algorithm(&q->sfind, (sss_alg_t)sss_alg);
}

bool sync::set_cell(float cfo_in)
{
  if (!cell.is_valid()) {
    Error("SYNC:  Setting cell: invalid cell (nof_prb=%d, pci=%d, ports=%d)",
          cell.get().nof_prb,
          cell.get().id,
          cell.get().nof_ports);
    return false;
  }

  // Set cell in all objects
  if (srsran_ue_sync_set_cell(&ue_sync, cell.get())) {
    Error("SYNC:  Setting cell: initiating ue_sync");
    return false;
  }
  sfn_p.set_cell(cell.get());
  worker_com->set_cell(cell.get());

  // Reset cell configuration
  for (uint32_t i = 0; i < worker_com->args->nof_phy_threads; i++) {
    (*lte_worker_pool)[i]->reset_cell_nolock(0);
  }

  bool success = true;
  for (uint32_t i = 0; i < worker_com->args->nof_phy_threads; i++) {
    lte::sf_worker* w = lte_worker_pool->wait_worker_id(i);
    if (w) {
      success &= w->set_cell_nolock(0, cell.get());
      w->release();
    }
  }
  if (!success) {
    Error("SYNC:  Setting cell: initiating PHCH worker");
    return false;
  }

  // Set options defined in expert section
  set_ue_sync_opts(&ue_sync, cfo_in);

  // Reset ue_sync and set CFO/gain from search procedure
  srsran_ue_sync_reset(&ue_sync);

  return true;
}

void sync::force_freq(float dl_freq_, float ul_freq_)
{
  dl_freq = dl_freq_;
  ul_freq = ul_freq_;
}

bool sync::set_frequency()
{
  double set_dl_freq = 0;
  double set_ul_freq = 0;

  if (dl_freq > 0 && ul_freq > 0) {
    set_dl_freq = dl_freq;
    set_ul_freq = ul_freq;
  } else {
    set_dl_freq = 1e6 * srsran_band_fd(current_earfcn);
    if (srsran_band_is_tdd(srsran_band_get_band(current_earfcn))) {
      set_ul_freq = set_dl_freq;
    } else {
      set_ul_freq = 1e6 * srsran_band_fu(worker_com->get_ul_earfcn(current_earfcn));
    }
  }
  if (set_dl_freq > 0 && set_ul_freq > 0) {
    phy_logger.info(
        "SYNC:  Set DL EARFCN=%d, f_dl=%.1f MHz, f_ul=%.1f MHz", current_earfcn, set_dl_freq / 1e6, set_ul_freq / 1e6);

    // Logical channel is 0
    radio_h->set_rx_freq(0, set_dl_freq);
    radio_h->set_tx_freq(0, set_ul_freq);

    ul_dl_factor = (float)(set_ul_freq / set_dl_freq);

    srsran_ue_sync_reset(&ue_sync);

    return true;
  } else {
    phy_logger.error("SYNC:  Cell Search: Invalid EARFCN=%d", current_earfcn);
    return false;
  }
}

void sync::set_sampling_rate()
{
  float new_srate = (float)srsran_sampling_freq_hz(cell.get().nof_prb);
  if (new_srate < 0.0) {
    Error("Invalid sampling rate for %d PRBs. keeping same.", cell.get().nof_prb);
    return;
  }

  srate.set_camp(new_srate);
  Info("SYNC:  Setting sampling rate %.2f MHz", new_srate / 1000000);
  radio_h->set_rx_srate(new_srate);
  radio_h->set_tx_srate(new_srate);
}

uint32_t sync::get_current_tti()
{
  return tti;
}

void sync::get_current_cell(srsran_cell_t* cell_, uint32_t* earfcn_)
{
  if (cell_) {
    *cell_ = cell.get();
  }
  if (earfcn_) {
    std::unique_lock<std::mutex> ul(rrc_mutex);
    *earfcn_ = current_earfcn;
  }
}

int sync::radio_recv_fnc(srsran::rf_buffer_t& data, srsran_timestamp_t* rx_time)
{
  // This function is designed for being called from the UE sync object which will pass a null rx_time in case
  // receive dummy samples. So, rf_timestamp points at dummy timestamp in case rx_time is not provided
  srsran::rf_timestamp_t  dummy_ts     = {};
  srsran::rf_timestamp_t& rf_timestamp = (rx_time == nullptr) ? dummy_ts : last_rx_time;

  // Receive
  if (not radio_h->rx_now(data, rf_timestamp)) {
    return SRSRAN_ERROR;
  }

  srsran_timestamp_t dummy_flat_ts = {};

  // Load flat timestamp
  if (rx_time == nullptr) {
    rx_time = &dummy_flat_ts;
  }
  *rx_time = rf_timestamp.get(0);

  // Save RF timestamp for the stack
  stack_tti_ts_new = rf_timestamp.get(0);

  // Run stack if the sync state is not in camping
  if (not phy_state.is_camping()) {
    Debug("run_stack_tti: from recv");
    run_stack_tti();
  }

  // Execute channel DL emulator
  if (channel_emulator) {
    channel_emulator->set_srate((uint32_t)srate.get_srate());
    channel_emulator->run(data.to_cf_t(), data.to_cf_t(), data.get_nof_samples(), *rx_time);
  }

  // Save signal for Intra-frequency measurement
  if (cell.is_valid()) {
    std::lock_guard<std::mutex> lock(intra_freq_cfg_mutex);
    for (uint32_t i = 0; (uint32_t)i < intra_freq_meas.size(); i++) {
      // Feed the exact number of base-band samples for avoiding an invalid buffer read
      intra_freq_meas[i]->run_tti(tti, data.get(i, 0, worker_com->args->nof_rx_ant), data.get_nof_samples());

      // Update RX gain
      intra_freq_meas[i]->set_rx_gain_offset(worker_com->get_rx_gain_offset());
    }
  }

  phy_logger.debug("SYNC:  received %d samples from radio", data.get_nof_samples());

  return data.get_nof_samples();
}

void sync::run_stack_tti()
{
  // check timestamp reset
  if (forced_rx_time_init || srsran_timestamp_iszero(&stack_tti_ts) ||
      srsran_timestamp_compare(&stack_tti_ts_new, &stack_tti_ts) < 0) {
    if (srsran_timestamp_compare(&stack_tti_ts_new, &stack_tti_ts) < 0) {
      phy_logger.warning("SYNC:  radio time seems to be going backwards (rx_time=%f, tti_ts=%f)",
                         srsran_timestamp_real(&stack_tti_ts_new),
                         srsran_timestamp_real(&stack_tti_ts));
      // time-stamp will be set to rx time below and run_tti() will be called with MIN_TTI_JUMP
    }

    // init tti_ts with last rx time
    phy_logger.debug("SYNC:  Setting initial TTI time to %f", srsran_timestamp_real(&stack_tti_ts_new));
    srsran_timestamp_copy(&stack_tti_ts, &stack_tti_ts_new);
    forced_rx_time_init = false;
  }

  // Advance stack in time
  if (srsran_timestamp_compare(&stack_tti_ts_new, &stack_tti_ts) >= 0) {
    srsran_timestamp_t temp = {};
    srsran_timestamp_copy(&temp, &stack_tti_ts_new);
    srsran_timestamp_sub(&temp, stack_tti_ts.full_secs, stack_tti_ts.frac_secs);
    int32_t tti_jump = static_cast<int32_t>(srsran_timestamp_uint64(&temp, 1e3));
    tti_jump         = SRSRAN_MAX(tti_jump, MIN_TTI_JUMP);
    if (tti_jump > MAX_TTI_JUMP) {
      phy_logger.warning("SYNC:  TTI jump of %d limited to %d", tti_jump, int(MAX_TTI_JUMP));
      tti_jump = SRSRAN_MIN(tti_jump, MAX_TTI_JUMP);
    }

    // Run stack
    Debug("run_stack_tti: calling stack tti=%d, tti_jump=%d", tti, tti_jump);
    stack->run_tti(tti, tti_jump);
    Debug("run_stack_tti: stack called");
  }

  // update timestamp
  srsran_timestamp_copy(&stack_tti_ts, &stack_tti_ts_new);
}

void sync::set_rx_gain(float gain)
{
  radio_h->set_rx_gain_th(gain);
}

/**********
 * PHY measurements
 *
 */

void sync::set_inter_frequency_measurement(uint32_t cc_idx, uint32_t earfcn_, srsran_cell_t cell_)
{
  std::lock_guard<std::mutex> lock(intra_freq_cfg_mutex);
  if (cc_idx < intra_freq_meas.size()) {
    intra_freq_meas[cc_idx]->set_primary_cell(earfcn_, cell_);
  }
}
void sync::set_cells_to_meas(uint32_t earfcn_, const std::set<uint32_t>& pci)
{
  std::lock_guard<std::mutex> lock(intra_freq_cfg_mutex);
  bool                        found = false;
  for (size_t i = 0; i < intra_freq_meas.size() and not found; i++) {
    if (earfcn_ == intra_freq_meas[i]->get_earfcn()) {
      intra_freq_meas[i]->set_cells_to_meas(pci);
      found = true;
    }
  }
  if (!found) {
    phy_logger.error("Neighbour cell measurement not supported in secondary carrier. EARFCN=%d", earfcn_);
  }
}

void sync::meas_stop()
{
  std::lock_guard<std::mutex> lock(intra_freq_cfg_mutex);
  for (auto& q : intra_freq_meas) {
    q->meas_stop();
  }
}

void sync::scell_sync_set(uint32_t cc_idx, const srsran_cell_t& _cell)
{
  // Ignore if out of range
  if (scell_sync.count(cc_idx) == 0) {
    return;
  }

  // Set secondary serving cell
  scell_sync.at(cc_idx)->set_cell(_cell);
}

void sync::scell_sync_stop()
{
  for (auto& e : scell_sync) {
    e.second->stop();
  }
}

void sync::cell_meas_reset(uint32_t cc_idx)
{
  worker_com->neighbour_cells_reset(cc_idx);
}

void sync::new_cell_meas(uint32_t cc_idx, const std::vector<phy_meas_t>& meas)
{
  // Pass measurements to phy_common for SINR estimation
  worker_com->set_neighbour_cells(cc_idx, meas);

  // Pass-through to the stack
  stack->new_cell_meas(meas);
}

} // namespace srsue
