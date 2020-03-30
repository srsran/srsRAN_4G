/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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
#include "srslte/common/log.h"
#include "srslte/phy/channel/channel.h"
#include "srslte/srslte.h"
#include "srsue/hdr/phy/sf_worker.h"
#include <algorithm>
#include <unistd.h>

#define Error(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->debug(fmt, ##__VA_ARGS__)

namespace srsue {

static int
radio_recv_callback(void* obj, cf_t* data[SRSLTE_MAX_CHANNELS], uint32_t nsamples, srslte_timestamp_t* rx_time)
{
  srslte::rf_buffer_t x(data);
  return ((sync*)obj)->radio_recv_fnc(x, nsamples, rx_time);
}

static SRSLTE_AGC_CALLBACK(callback_set_rx_gain)
{
  ((sync*)h)->set_rx_gain(gain_db);
}

void sync::init(srslte::radio_interface_phy* _radio,
                stack_interface_phy_lte*     _stack,
                prach*                       _prach_buffer,
                srslte::thread_pool*         _workers_pool,
                phy_common*                  _worker_com,
                srslte::log*                 _log_h,
                srslte::log*                 _log_phy_lib_h,
                uint32_t                     prio,
                int                          sync_cpu_affinity)
{
  radio_h       = _radio;
  log_h         = _log_h;
  log_phy_lib_h = _log_phy_lib_h;
  stack         = _stack;
  workers_pool  = _workers_pool;
  worker_com    = _worker_com;
  prach_buffer  = _prach_buffer;

  nof_rf_channels = worker_com->args->nof_carriers * worker_com->args->nof_rx_ant;
  if (nof_rf_channels == 0 || nof_rf_channels > SRSLTE_MAX_CHANNELS) {
    Error("SYNC:  Invalid number of RF channels (%d)\n", nof_rf_channels);
    return;
  }

  if (srslte_ue_sync_init_multi(&ue_sync, SRSLTE_MAX_PRB, false, radio_recv_callback, nof_rf_channels, this)) {
    Error("SYNC:  Initiating ue_sync\n");
    return;
  }

  if (worker_com->args->dl_channel_args.enable) {
    channel_emulator = srslte::channel_ptr(new srslte::channel(worker_com->args->dl_channel_args, nof_rf_channels));
  }

  nof_workers = workers_pool->get_nof_workers();
  worker_com->set_nof_workers(nof_workers);

  // Initialize cell searcher
  search_p.init(sf_buffer, log_h, nof_rf_channels, this);

  // Initialize SFN synchronizer, it uses only pcell buffer
  sfn_p.init(&ue_sync, worker_com->args, sf_buffer, sf_buffer.size(), log_h);

  // Start intra-frequency measurement
  for (uint32_t i = 0; i < worker_com->args->nof_carriers; i++) {
    auto q = new scell::intra_measure;
    q->init(worker_com, stack, log_h);
    intra_freq_meas.push_back(std::unique_ptr<scell::intra_measure>(q));
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
  srslte_ue_sync_free(&ue_sync);
}

void sync::stop()
{
  worker_com->semaphore.wait_all();
  for (auto& q : intra_freq_meas) {
    q->stop();
  }
  running = false;
  wait_thread_finish();
}

void sync::reset()
{
  in_sync_cnt           = 0;
  out_of_sync_cnt       = 0;
  current_earfcn        = -1;
  srate_mode            = SRATE_NONE;
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

/* A call to cell_search() finds the strongest cell in the set of supported EARFCNs. When the first cell is found,
 * returns 1 and stores cell information and RSRP values in the pointers (if provided). If a cell is not found in the
 * current frequency it moves to the next one and the next call to cell_search() will look in the next EARFCN in the
 * set. If no cells are found in any frequency it returns 0. If error returns -1.
 */

phy_interface_rrc_lte::cell_search_ret_t sync::cell_search(phy_interface_rrc_lte::phy_cell_t* found_cell)
{
  std::unique_lock<std::mutex> ul(rrc_mutex);

  phy_interface_rrc_lte::cell_search_ret_t ret = {};
  ret.found     = phy_interface_rrc_lte::cell_search_ret_t::ERROR;
  ret.last_freq = phy_interface_rrc_lte::cell_search_ret_t::NO_MORE_FREQS;

  // Move state to IDLE
  Info("Cell Search: Start EARFCN index=%u/%zd\n", cellsearch_earfcn_index, worker_com->args->dl_earfcn_list.size());
  phy_state.go_idle();
  worker_com->reset();

  // Stop all intra-frequency measurement before changing frequency
  meas_stop();

  if (srate_mode != SRATE_FIND) {
    srate_mode = SRATE_FIND;
    radio_h->set_rx_srate(1.92e6);
    radio_h->set_tx_srate(1.92e6);
    Info("SYNC:  Setting Cell Search sampling rate\n");
  }

  try {
    if (current_earfcn != (int)worker_com->args->dl_earfcn_list.at(cellsearch_earfcn_index)) {
      current_earfcn = (int)worker_com->args->dl_earfcn_list[cellsearch_earfcn_index];
      Info("Cell Search: changing frequency to EARFCN=%d\n", current_earfcn);
      set_frequency();
    }
  } catch (const std::out_of_range& oor) {
    Error("Index %d is not a valid EARFCN element.\n", cellsearch_earfcn_index);
    return ret;
  }

  // Move to CELL SEARCH and wait to finish
  Info("Cell Search: Setting Cell search state\n");
  phy_state.run_cell_search();

  // Check return state
  switch (cell_search_ret) {
    case search::CELL_FOUND:
      log_h->info("Cell Search: Found cell with PCI=%d with %d PRB\n", cell.id, cell.nof_prb);
      if (found_cell) {
        found_cell->earfcn = current_earfcn;
        found_cell->pci    = cell.id;
        found_cell->cfo_hz = search_p.get_last_cfo();
      }
      ret.found = phy_interface_rrc_lte::cell_search_ret_t::CELL_FOUND;
      break;
    case search::CELL_NOT_FOUND:
      Info("Cell Search: No cell found in this frequency\n");
      ret.found = phy_interface_rrc_lte::cell_search_ret_t::CELL_NOT_FOUND;
      break;
    default:
      Error("Cell Search: while receiving samples\n");
      radio_error();
      break;
  }

  cellsearch_earfcn_index++;
  if (cellsearch_earfcn_index >= worker_com->args->dl_earfcn_list.size()) {
    Info("Cell Search: No more frequencies in the current EARFCN set\n");
    cellsearch_earfcn_index = 0;
    ret.last_freq           = phy_interface_rrc_lte::cell_search_ret_t::NO_MORE_FREQS;
  } else {
    ret.last_freq = phy_interface_rrc_lte::cell_search_ret_t::MORE_FREQS;
  }

  return ret;
}

/* Cell select synchronizes to a new cell (e.g. during HO or during cell reselection on IDLE) or
 * re-synchronizes with the current cell if cell argument is NULL
 */
bool sync::cell_select(const phy_interface_rrc_lte::phy_cell_t* new_cell)
{
  std::unique_lock<std::mutex> ul(rrc_mutex);

  bool ret = false;
  int  cnt = 0;

  // Move state to IDLE
  if (new_cell == nullptr) {
    Info("Cell Select: Starting cell resynchronization\n");
  } else {
    if (!srslte_cellid_isvalid(new_cell->pci)) {
      log_h->error("Cell Select: Invalid cell_id=%d\n", new_cell->pci);
      return ret;
    }
    Info("Cell Select: Starting cell selection for PCI=%d, EARFCN=%d\n", new_cell->pci, new_cell->earfcn);
  }

  // Wait for any pending PHICH
  while (worker_com->is_any_ul_pending_ack() && cnt < 10) {
    usleep(1000);
    cnt++;
    Info("Cell Select: waiting pending PHICH (cnt=%d)\n", cnt);
  }

  Info("Cell Select: Going to IDLE\n");
  phy_state.go_idle();

  worker_com->reset();
  sfn_p.reset();
  search_p.reset();
  srslte_ue_sync_reset(&ue_sync);

  /* Reconfigure cell if necessary */
  if (new_cell != nullptr) {
    cell.id = new_cell->pci;
    if (not set_cell(new_cell->cfo_hz)) {
      Error("Cell Select: Reconfiguring cell\n");
      return ret;
    }

    /* Select new frequency if necessary */
    if ((int)new_cell->earfcn != current_earfcn) {
      current_earfcn = new_cell->earfcn;

      // Stop all intra-frequency measurement before changing frequency
      meas_stop();

      Info("Cell Select: Setting new frequency EARFCN=%d\n", new_cell->earfcn);
      if (!set_frequency()) {
        Error("Cell Select: Setting new frequency EARFCN=%d\n", new_cell->earfcn);
        return ret;
      }
    }

    // Reconfigure first intra-frequency measurement
    intra_freq_meas[0]->set_primary_cell(current_earfcn, cell);
  }

  /* Change sampling rate if necessary */
  if (srate_mode != SRATE_CAMP) {
    log_h->info("Cell Select: Setting CAMPING sampling rate\n");
    set_sampling_rate();
  }

  /* SFN synchronization */
  phy_state.run_sfn_sync();
  if (phy_state.is_camping()) {
    Info("Cell Select: SFN synchronized. CAMPING...\n");
    stack->in_sync();
    ret = true;
  } else {
    Info("Cell Select: Could not synchronize SFN\n");
  }

  return ret;
}

bool sync::cell_is_camping()
{
  return phy_state.is_camping();
}

/**
 * MAIN THREAD
 *
 * The main thread process the SYNC state machine. Every state except IDLE must have exclusive access to
 * all variables. If any change of cell configuration must be done, the thread must be in IDLE.
 *
 * On each state except campling, 1 function is called and the thread jumps to the next state based on the output.
 *
 * It has 3 states: Cell search, SFN synchronization, initial measurement and camping.
 * - CELL_SEARCH:   Initial Cell id and MIB acquisition. Uses 1.92 MHz sampling rate
 * - CELL_SYNC:     Full sampling rate, uses MIB to obtain SFN. When SFN is obtained, moves to CELL_CAMP
 * - CELL_CAMP:     Cell camping state. Calls the PHCH workers to process subframes and maintains cell synchronization.
 * - IDLE:          Receives and discards received samples. Does not maintain synchronization.
 *
 */

void sync::run_thread()
{
  sf_worker*    worker    = nullptr;
  srslte_cell_t temp_cell = {};

  bool force_camping_sfn_sync = false;

  srslte::rf_buffer_t dummy_buffer(sync_nof_rx_subframes);
  srslte::rf_buffer_t sync_buffer = {};

  uint32_t prach_nof_sf = 0;
  uint32_t prach_sf_cnt = 0;
  cf_t*    prach_ptr    = NULL;
  float    prach_power  = 0;

  while (running) {
    Debug("SYNC:  state=%s, tti=%d\n", phy_state.to_string(), tti);

    // If not camping, clear SFN sync
    if (!phy_state.is_camping()) {
      force_camping_sfn_sync = false;
    }

    if (log_phy_lib_h) {
      log_phy_lib_h->step(tti);
    }

    switch (phy_state.run_state()) {
      case sync_state::CELL_SEARCH:
        /* Search for a cell in the current frequency and go to IDLE.
         * The function search_p.run() will not return until the search finishes
         */
        cell_search_ret = search_p.run(&cell, mib);
        if (cell_search_ret == search::CELL_FOUND) {
          stack->bch_decoded_ok(SYNC_CC_IDX, mib.data(), mib.size() / 8);
        }
        phy_state.state_exit();
        break;
      case sync_state::SFN_SYNC:

        /* SFN synchronization using MIB. run_subframe() receives and processes 1 subframe
         * and returns
         */
        temp_cell = cell;
        switch (sfn_p.run_subframe(&temp_cell, &tti, mib)) {
          case sfn_sync::SFN_FOUND:
            if (memcmp(&cell, &temp_cell, sizeof(srslte_cell_t))) {
              srslte_cell_fprint(stdout, &cell, 0);
              srslte_cell_fprint(stdout, &temp_cell, 0);
              log_h->error("Detected cell during SFN synchronization differs from configured cell. Cell reselection to "
                           "cells with different MIB is not supported\n");
              log_h->console("Detected cell during SFN synchronization differs from configured cell. Cell reselection "
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
        break;
      case sync_state::CAMPING:

        worker = (sf_worker*)workers_pool->wait_worker(tti);
        if (worker) {
          // Map carrier/antenna buffers to worker buffers
          for (uint32_t c = 0; c < worker_com->args->nof_carriers; c++) {
            for (uint32_t i = 0; i < worker_com->args->nof_rx_ant; i++) {
              sync_buffer.set(c, i, worker_com->args->nof_rx_ant, worker->get_buffer(c, i));
            }
          }

          // Primary Cell (PCell) Synchronization
          switch (srslte_ue_sync_zerocopy(&ue_sync, sync_buffer.to_cf_t(), worker->get_buffer_len())) {
            case 1:

              // Check tti is synched with ue_sync
              if (srslte_ue_sync_get_sfidx(&ue_sync) != tti % 10) {
                uint32_t sfn = tti / 10;
                tti          = (sfn * 10 + srslte_ue_sync_get_sfidx(&ue_sync)) % 10240;

                // Force SFN decode, just in case it is in the wrong frame
                force_camping_sfn_sync = true;
              }

              if (is_overflow) {
                force_camping_sfn_sync = true;
                is_overflow            = false;
                log_h->info("Detected overflow, trying to resync SFN\n");
              }

              // Force decode MIB if required
              if (force_camping_sfn_sync) {
                uint32_t _tti                = 0;
                temp_cell                    = cell;
                sync::sfn_sync::ret_code ret = sfn_p.decode_mib(&temp_cell, &_tti, &sync_buffer, mib);
                if (ret == sfn_sync::SFN_FOUND) {
                  // Force tti
                  tti = _tti;

                  // Disable
                  force_camping_sfn_sync = false;

                  if (memcmp(&cell, &temp_cell, sizeof(srslte_cell_t))) {
                    log_h->error("Detected cell during SFN synchronization differs from configured cell. Cell "
                                 "reselection to cells with different MIB is not supported\n");
                    log_h->console("Detected cell during SFN synchronization differs from configured cell. Cell "
                                   "reselection to cells with different MIB is not supported\n");
                  } else {
                    log_h->info("SFN resynchronized successfully\n");
                  }
                } else {
                  log_h->warning("SFN not yet synchronized, sending out-of-sync\n");
                }
              }

              Debug("SYNC:  Worker %d synchronized\n", worker->get_id());

              metrics.sfo   = srslte_ue_sync_get_sfo(&ue_sync);
              metrics.cfo   = srslte_ue_sync_get_cfo(&ue_sync);
              metrics.ta_us = worker_com->ta.get_usec();
              for (uint32_t i = 0; i < worker_com->args->nof_carriers; i++) {
                worker_com->set_sync_metrics(i, metrics);
              }

              // Check if we need to TX a PRACH
              if (prach_buffer->is_ready_to_send(tti)) {
                prach_ptr = prach_buffer->generate(get_tx_cfo(), &prach_nof_sf, &prach_power);
                if (!prach_ptr) {
                  Error("Generating PRACH\n");
                }
              }

              /* Compute TX time: Any transmission happens in TTI+4 thus advance 4 ms the reception time */
              srslte_timestamp_t rx_time, tx_time;
              srslte_ue_sync_get_last_timestamp(&ue_sync, &rx_time);
              srslte_timestamp_copy(&tx_time, &rx_time);
              srslte_timestamp_add(&tx_time, 0, FDD_HARQ_DELAY_DL_MS * 1e-3);

              worker->set_prach(prach_ptr ? &prach_ptr[prach_sf_cnt * SRSLTE_SF_LEN_PRB(cell.nof_prb)] : nullptr,
                                prach_power);

              // Set CFO for all Carriers
              for (uint32_t cc = 0; cc < worker_com->args->nof_carriers; cc++) {
                worker->set_cfo(cc, get_tx_cfo());
                worker_com->avg_cfo_hz[cc] = srslte_ue_sync_get_cfo(&ue_sync);
              }

              worker->set_tti(tti);
              worker->set_tx_time(tx_time);

              // Advance/reset prach subframe pointer
              if (prach_ptr) {
                prach_sf_cnt++;
                if (prach_sf_cnt == prach_nof_sf) {
                  prach_sf_cnt = 0;
                  prach_ptr    = nullptr;
                }
              }

              // Start worker
              worker_com->semaphore.push(worker);
              workers_pool->start_worker(worker);

              break;
            case 0:
              Warning("SYNC:  Out-of-sync detected in PSS/SSS\n");
              out_of_sync();
              worker->release();

              // Force decoding MIB, for making sure that the TTI will be right
              if (!force_camping_sfn_sync) {
                force_camping_sfn_sync = true;
              }

              break;
            default:
              radio_error();
              break;
          }
        } else {
          // wait_worker() only returns NULL if it's being closed. Quit now to avoid unnecessary loops here
          running = false;
        }
        break;
      case sync_state::IDLE:
        if (radio_h->is_init()) {
          uint32_t nsamples = 1920;
          if (current_srate > 0) {
            nsamples = current_srate / 1000;
          }
          Debug("Discarding %d samples\n", nsamples);
          srslte_timestamp_t rx_time = {};
          if (!radio_recv_fnc(dummy_buffer, nsamples, &rx_time)) {
            log_h->console("SYNC:  Receiving from radio while in IDLE_RX\n");
          }
          // If radio is in locked state returns immediately. In that case, do a 1 ms sleep
          if (rx_time.frac_secs == 0 && rx_time.full_secs == 0) {
            usleep(1000);
          }
          radio_h->tx_end();
        } else {
          Debug("Sleeping\n");
          usleep(1000);
        }
        break;
    }

    // Increase TTI counter
    tti = (tti + 1) % 10240;
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
  log_h->error("SYNC:  Receiving from radio.\n");
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
  Info("Out-of-sync %d/%d\n", out_of_sync_cnt, worker_com->args->nof_out_of_sync_events);
  out_of_sync_cnt++;
  if (out_of_sync_cnt == worker_com->args->nof_out_of_sync_events) {
    Info("Sending to RRC\n");
    stack->out_of_sync();
    out_of_sync_cnt = 0;
    in_sync_cnt     = 0;
  }
}

void sync::set_cfo(float cfo)
{
  srslte_ue_sync_set_cfo_ref(&ue_sync, cfo);
}

void sync::set_agc_enable(bool enable)
{
  if (enable) {
    if (running && radio_h) {
      srslte_rf_info_t* rf_info = radio_h->get_info();
      if (rf_info) {
        srslte_ue_sync_start_agc(
            &ue_sync, callback_set_rx_gain, rf_info->min_rx_gain, rf_info->max_rx_gain, radio_h->get_rx_gain());
        search_p.set_agc_enable(true);
      } else {
        Error("Error: Radio does not provide RF information\n");
      }
    } else {
      ERROR("Error setting AGC: PHY not initiated\n");
    }
  } else {
    ERROR("Error stopping AGC: not implemented\n");
  }
}

float sync::get_tx_cfo()
{
  float cfo = srslte_ue_sync_get_cfo(&ue_sync);

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

void sync::set_ue_sync_opts(srslte_ue_sync_t* q, float cfo)
{
  if (worker_com->args->cfo_integer_enabled) {
    srslte_ue_sync_set_cfo_i_enable(q, true);
  }

  srslte_ue_sync_set_cfo_ema(q, worker_com->args->cfo_pss_ema);
  srslte_ue_sync_set_cfo_tol(q, worker_com->args->cfo_correct_tol_hz);
  srslte_ue_sync_set_cfo_loop_bw(q,
                                 worker_com->args->cfo_loop_bw_pss,
                                 worker_com->args->cfo_loop_bw_ref,
                                 worker_com->args->cfo_loop_pss_tol,
                                 worker_com->args->cfo_loop_ref_min,
                                 worker_com->args->cfo_loop_pss_tol,
                                 worker_com->args->cfo_loop_pss_conv);

  // Disable CP based CFO estimation during find
  if (std::isnormal(cfo)) {
    srslte_ue_sync_cfo_reset(q, cfo);
    q->cfo_current_value       = cfo / 15000;
    q->cfo_is_copied           = true;
    q->cfo_correct_enable_find = true;
    srslte_sync_set_cfo_cp_enable(&q->sfind, false, 0);
  }

  // Set SFO ema and correct period
  srslte_ue_sync_set_sfo_correct_period(q, worker_com->args->sfo_correct_period);
  srslte_ue_sync_set_sfo_ema(q, worker_com->args->sfo_ema);

  sss_alg_t sss_alg = SSS_FULL;
  if (!worker_com->args->sss_algorithm.compare("diff")) {
    sss_alg = SSS_DIFF;
  } else if (!worker_com->args->sss_algorithm.compare("partial")) {
    sss_alg = SSS_PARTIAL_3;
  } else if (!worker_com->args->sss_algorithm.compare("full")) {
    sss_alg = SSS_FULL;
  } else {
    Warning("SYNC:  Invalid SSS algorithm %s. Using 'full'\n", worker_com->args->sss_algorithm.c_str());
  }
  srslte_sync_set_sss_algorithm(&q->strack, (sss_alg_t)sss_alg);
  srslte_sync_set_sss_algorithm(&q->sfind, (sss_alg_t)sss_alg);
}

bool sync::set_cell(float cfo)
{
  if (!phy_state.is_idle()) {
    Warning("Can not change Cell while not in IDLE\n");
    return false;
  }

  if (!srslte_cell_isvalid(&cell)) {
    Error("SYNC:  Setting cell: invalid cell (nof_prb=%d, pci=%d, ports=%d)\n", cell.nof_prb, cell.id, cell.nof_ports);
    return false;
  }

  // Set cell in all objects
  if (srslte_ue_sync_set_cell(&ue_sync, cell)) {
    Error("SYNC:  Setting cell: initiating ue_sync\n");
    return false;
  }
  sfn_p.set_cell(cell);
  worker_com->set_cell(cell);

  bool success = true;
  for (uint32_t i = 0; i < workers_pool->get_nof_workers(); i++) {
    sf_worker* w = (sf_worker*)workers_pool->wait_worker_id(i);
    success &= w->set_cell(0, cell);
    w->release();
  }
  if (!success) {
    Error("SYNC:  Setting cell: initiating PHCH worker\n");
    return false;
  }

  // Set options defined in expert section
  set_ue_sync_opts(&ue_sync, cfo);

  // Reset ue_sync and set CFO/gain from search procedure
  srslte_ue_sync_reset(&ue_sync);

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
    set_dl_freq = 1e6 * srslte_band_fd(current_earfcn);
    if (srslte_band_is_tdd(srslte_band_get_band(current_earfcn))) {
      set_ul_freq = set_dl_freq;
    } else {
      set_ul_freq = 1e6 * srslte_band_fu(worker_com->get_ul_earfcn(current_earfcn));
    }
  }
  if (set_dl_freq > 0 && set_ul_freq > 0) {
    log_h->info("SYNC:  Set DL EARFCN=%d, f_dl=%.1f MHz, f_ul=%.1f MHz\n",
                current_earfcn,
                set_dl_freq / 1e6,
                set_ul_freq / 1e6);

    // Logical channel is 0
    radio_h->set_rx_freq(0, set_dl_freq);
    radio_h->set_tx_freq(0, set_ul_freq);

    ul_dl_factor = (float)(set_ul_freq / set_dl_freq);

    srslte_ue_sync_reset(&ue_sync);

    return true;
  } else {
    log_h->error("SYNC:  Cell Search: Invalid EARFCN=%d\n", current_earfcn);
    return false;
  }
}

void sync::set_sampling_rate()
{
  float new_srate = (float)srslte_sampling_freq_hz(cell.nof_prb);
  if (new_srate < 0.0) {
    Error("Invalid sampling rate for %d PRBs. keeping same.\n", cell.nof_prb);
    return;
  }

  current_sflen = (uint32_t)SRSLTE_SF_LEN_PRB(cell.nof_prb);
  if (current_srate != new_srate || srate_mode != SRATE_CAMP) {
    current_srate = new_srate;
    Info("SYNC:  Setting sampling rate %.2f MHz\n", current_srate / 1000000);

    srate_mode = SRATE_CAMP;
    radio_h->set_rx_srate(current_srate);
    radio_h->set_tx_srate(current_srate);
  } else {
    Error("Error setting sampling rate for cell with %d PRBs\n", cell.nof_prb);
  }
}

uint32_t sync::get_current_tti()
{
  return tti;
}

void sync::get_current_cell(srslte_cell_t* cell_, uint32_t* earfcn_)
{
  if (cell_) {
    *cell_ = cell;
  }
  if (earfcn_) {
    *earfcn_ = current_earfcn;
  }
}

int sync::radio_recv_fnc(srslte::rf_buffer_t& data, uint32_t nsamples, srslte_timestamp_t* rx_time)
{
  srslte_timestamp_t ts = {};

  // Use local timestamp if timestamp is not provided
  if (!rx_time) {
    rx_time = &ts;
  }

  // Receive
  if (radio_h->rx_now(data, nsamples, rx_time)) {
    // check timestamp reset
    if (forced_rx_time_init || srslte_timestamp_iszero(&tti_ts) || srslte_timestamp_compare(rx_time, &tti_ts) < 0) {
      if (srslte_timestamp_compare(rx_time, &tti_ts) < 0) {
        log_h->warning("SYNC:  radio time seems to be going backwards (rx_time=%f, tti_ts=%f)\n",
                       srslte_timestamp_real(rx_time),
                       srslte_timestamp_real(&tti_ts));
        // time-stamp will be set to rx time below and run_tti() will be called with MIN_TTI_JUMP
      }

      // init tti_ts with last rx time
      log_h->debug("SYNC:  Setting initial TTI time to %f\n", srslte_timestamp_real(rx_time));
      srslte_timestamp_copy(&tti_ts, rx_time);
      forced_rx_time_init = false;
    }

    // Advance stack in time
    if (srslte_timestamp_compare(rx_time, &tti_ts) >= 0) {
      srslte_timestamp_t temp = {};
      srslte_timestamp_copy(&temp, rx_time);
      srslte_timestamp_sub(&temp, tti_ts.full_secs, tti_ts.frac_secs);
      int32_t tti_jump = static_cast<int32_t>(srslte_timestamp_uint64(&temp, 1e3));
      tti_jump         = SRSLTE_MAX(tti_jump, MIN_TTI_JUMP);
      if (tti_jump > MAX_TTI_JUMP) {
        log_h->warning("SYNC:  TTI jump of %d limited to %d\n", tti_jump, MAX_TTI_JUMP);
        tti_jump = SRSLTE_MIN(tti_jump, MAX_TTI_JUMP);
      }

      // Run stack
      stack->run_tti(tti, tti_jump);
    }

    // update timestamp
    srslte_timestamp_copy(&tti_ts, rx_time);

    if (channel_emulator && rx_time) {
      channel_emulator->set_srate((uint32_t)current_srate);
      channel_emulator->run(data.to_cf_t(), data.to_cf_t(), nsamples, *rx_time);
    }

    // Save signal for Intra-frequency measurement
    if (srslte_cell_isvalid(&cell)) {
      for (uint32_t i = 0; (uint32_t)i < intra_freq_meas.size(); i++) {
        intra_freq_meas[i]->write(tti, data.get(i, 0, worker_com->args->nof_rx_ant), SRSLTE_SF_LEN_PRB(cell.nof_prb));

        // Update RX gain
        intra_freq_meas[i]->set_rx_gain_offset(worker_com->rx_gain_offset);
      }
    }

    log_h->debug("SYNC:  received %d samples from radio\n", nsamples);

    return nsamples;
  } else {
    return -1;
  }
}

void sync::set_rx_gain(float gain)
{
  radio_h->set_rx_gain_th(gain);
}

/*********
 * Cell search class
 */
sync::search::~search()
{
  srslte_ue_mib_sync_free(&ue_mib_sync);
  srslte_ue_cellsearch_free(&cs);
}

void sync::search::init(srslte::rf_buffer_t& buffer_, srslte::log* log_h_, uint32_t nof_rx_channels, sync* parent)
{
  log_h = log_h_;
  p     = parent;

  buffer = buffer_;

  if (srslte_ue_cellsearch_init_multi(&cs, 8, radio_recv_callback, nof_rx_channels, parent)) {
    Error("SYNC:  Initiating UE cell search\n");
  }
  srslte_ue_cellsearch_set_nof_valid_frames(&cs, 4);

  if (srslte_ue_mib_sync_init_multi(&ue_mib_sync, radio_recv_callback, nof_rx_channels, parent)) {
    Error("SYNC:  Initiating UE MIB synchronization\n");
  }

  // Set options defined in expert section
  p->set_ue_sync_opts(&cs.ue_sync, 0);

  force_N_id_2 = -1;
}

void sync::search::reset()
{
  srslte_ue_sync_reset(&ue_mib_sync.ue_sync);
}

float sync::search::get_last_cfo()
{
  return srslte_ue_sync_get_cfo(&ue_mib_sync.ue_sync);
}

void sync::search::set_agc_enable(bool enable)
{
  if (enable) {
    srslte_rf_info_t* rf_info = p->radio_h->get_info();
    srslte_ue_sync_start_agc(&ue_mib_sync.ue_sync,
                             callback_set_rx_gain,
                             rf_info->min_rx_gain,
                             rf_info->max_rx_gain,
                             p->radio_h->get_rx_gain());
  } else {
    ERROR("Error stop AGC not implemented\n");
  }
}

sync::search::ret_code sync::search::run(srslte_cell_t* cell_, std::array<uint8_t, SRSLTE_BCH_PAYLOAD_LEN>& bch_payload)
{
  srslte_cell_t new_cell = {};

  srslte_ue_cellsearch_result_t found_cells[3];

  bzero(found_cells, 3 * sizeof(srslte_ue_cellsearch_result_t));

  /* Find a cell in the given N_id_2 or go through the 3 of them to find the strongest */
  uint32_t max_peak_cell = 0;
  int      ret           = SRSLTE_ERROR;

  Info("SYNC:  Searching for cell...\n");
  log_h->console(".");

  if (force_N_id_2 >= 0 && force_N_id_2 < 3) {
    ret           = srslte_ue_cellsearch_scan_N_id_2(&cs, force_N_id_2, &found_cells[force_N_id_2]);
    max_peak_cell = force_N_id_2;
  } else {
    ret = srslte_ue_cellsearch_scan(&cs, found_cells, &max_peak_cell);
  }

  if (ret < 0) {
    Error("SYNC:  Error decoding MIB: Error searching PSS\n");
    return ERROR;
  } else if (ret == 0) {
    Info("SYNC:  Could not find any cell in this frequency\n");
    return CELL_NOT_FOUND;
  }
  // Save result
  new_cell.id         = found_cells[max_peak_cell].cell_id;
  new_cell.cp         = found_cells[max_peak_cell].cp;
  new_cell.frame_type = found_cells[max_peak_cell].frame_type;
  float cfo           = found_cells[max_peak_cell].cfo;

  log_h->console("\n");
  Info("SYNC:  PSS/SSS detected: Mode=%s, PCI=%d, CFO=%.1f KHz, CP=%s\n",
       new_cell.frame_type ? "TDD" : "FDD",
       new_cell.id,
       cfo / 1000,
       srslte_cp_string(new_cell.cp));

  if (srslte_ue_mib_sync_set_cell(&ue_mib_sync, new_cell)) {
    Error("SYNC:  Setting UE MIB cell\n");
    return ERROR;
  }

  // Set options defined in expert section
  p->set_ue_sync_opts(&ue_mib_sync.ue_sync, cfo);

  srslte_ue_sync_reset(&ue_mib_sync.ue_sync);

  /* Find and decode MIB */
  int sfn_offset;
  ret = srslte_ue_mib_sync_decode(&ue_mib_sync, 40, bch_payload.data(), &new_cell.nof_ports, &sfn_offset);
  if (ret == 1) {
    srslte_pbch_mib_unpack(bch_payload.data(), &new_cell, NULL);
    // pack MIB and store inplace for PCAP dump
    std::array<uint8_t, SRSLTE_BCH_PAYLOAD_LEN / 8> mib_packed;
    srslte_bit_pack_vector(bch_payload.data(), mib_packed.data(), SRSLTE_BCH_PAYLOAD_LEN);
    std::copy(std::begin(mib_packed), std::end(mib_packed), std::begin(bch_payload));

    fprintf(stdout,
            "Found Cell:  Mode=%s, PCI=%d, PRB=%d, Ports=%d, CFO=%.1f KHz\n",
            new_cell.frame_type ? "TDD" : "FDD",
            new_cell.id,
            new_cell.nof_prb,
            new_cell.nof_ports,
            cfo / 1000);

    Info("SYNC:  MIB Decoded: Mode=%s, PCI=%d, PRB=%d, Ports=%d, CFO=%.1f KHz\n",
         new_cell.frame_type ? "TDD" : "FDD",
         new_cell.id,
         new_cell.nof_prb,
         new_cell.nof_ports,
         cfo / 1000);

    if (!srslte_cell_isvalid(&new_cell)) {
      Error("SYNC:  Detected invalid cell.\n");
      return CELL_NOT_FOUND;
    }

    // Save cell pointer
    if (cell_) {
      *cell_ = new_cell;
    }

    return CELL_FOUND;
  } else if (ret == 0) {
    Warning("SYNC:  Found PSS but could not decode PBCH\n");
    return CELL_NOT_FOUND;
  } else {
    Error("SYNC:  Receiving MIB\n");
    return ERROR;
  }
}

/*********
 * SFN synchronizer class
 */

sync::sfn_sync::~sfn_sync()
{
  srslte_ue_mib_free(&ue_mib);
}

void sync::sfn_sync::init(srslte_ue_sync_t*    ue_sync_,
                          const phy_args_t*    phy_args_,
                          srslte::rf_buffer_t& buffer,
                          uint32_t             buffer_max_samples_,
                          srslte::log*         log_h_,
                          uint32_t             nof_subframes)
{
  log_h   = log_h_;
  ue_sync = ue_sync_;
  phy_args = phy_args_;
  timeout = nof_subframes;

  mib_buffer         = buffer;
  buffer_max_samples = buffer_max_samples_;

  // MIB decoder uses a single receiver antenna in logical channel 0
  if (srslte_ue_mib_init(&ue_mib, buffer.get(0), SRSLTE_MAX_PRB)) {
    Error("SYNC:  Initiating UE MIB decoder\n");
  }
}

bool sync::sfn_sync::set_cell(srslte_cell_t cell_)
{
  if (srslte_ue_mib_set_cell(&ue_mib, cell_)) {
    Error("SYNC:  Setting cell: initiating ue_mib\n");
    return false;
  }
  reset();
  return true;
}

void sync::sfn_sync::reset()
{
  cnt = 0;
  srslte_ue_mib_reset(&ue_mib);
}

sync::sfn_sync::ret_code sync::sfn_sync::run_subframe(srslte_cell_t*                               cell_,
                                                      uint32_t*                                    tti_cnt,
                                                      std::array<uint8_t, SRSLTE_BCH_PAYLOAD_LEN>& bch_payload,
                                                      bool                                         sfidx_only)
{
  int ret = srslte_ue_sync_zerocopy(ue_sync, mib_buffer.to_cf_t(), buffer_max_samples);
  if (ret < 0) {
    Error("SYNC:  Error calling ue_sync_get_buffer.\n");
    return ERROR;
  }

  if (ret == 1) {
    sync::sfn_sync::ret_code ret2 = decode_mib(cell_, tti_cnt, nullptr, bch_payload, sfidx_only);
    if (ret2 != SFN_NOFOUND) {
      return ret2;
    }
  } else {
    Info("SYNC:  Waiting for PSS while trying to decode MIB (%d/%d)\n", cnt, timeout);
  }

  cnt++;
  if (cnt >= timeout) {
    cnt = 0;
    return SFN_NOFOUND;
  }

  return IDLE;
}

sync::sfn_sync::ret_code sync::sfn_sync::decode_mib(srslte_cell_t*                               cell_,
                                                    uint32_t*                                    tti_cnt,
                                                    srslte::rf_buffer_t*                         ext_buffer,
                                                    std::array<uint8_t, SRSLTE_BCH_PAYLOAD_LEN>& bch_payload,
                                                    bool                                         sfidx_only)
{
  // If external buffer provided not equal to internal buffer, copy samples from channel/port 0
  if (ext_buffer != nullptr) {
    memcpy(mib_buffer.get(0), ext_buffer->get(0), sizeof(cf_t) * ue_sync->sf_len);
  }

  if (srslte_ue_sync_get_sfidx(ue_sync) == 0) {

    // Skip MIB decoding if we are only interested in subframe 0
    if (sfidx_only) {
      if (tti_cnt) {
        *tti_cnt = 0;
      }
      return SFX0_FOUND;
    }

    int sfn_offset = 0;
    int n          = srslte_ue_mib_decode(&ue_mib, bch_payload.data(), NULL, &sfn_offset);
    switch (n) {
      default:
        Error("SYNC:  Error decoding MIB while synchronising SFN");
        return ERROR;
      case SRSLTE_UE_MIB_FOUND:
        uint32_t sfn;
        srslte_pbch_mib_unpack(bch_payload.data(), cell_, &sfn);

        sfn = (sfn + sfn_offset) % 1024;
        if (tti_cnt) {
          *tti_cnt = 10 * sfn;

          // Check if SNR is below the minimum threshold
          if (ue_mib.chest_res.snr_db < phy_args->in_sync_snr_db_th) {
            Info("SYNC:  MIB decoded, SNR is too low (%+.1f < %+.1f)\n",
                 ue_mib.chest_res.snr_db,
                 phy_args->in_sync_snr_db_th);
            return SFN_NOFOUND;
          }

          Info("SYNC:  DONE, SNR=%.1f dB, TTI=%d, sfn_offset=%d\n", ue_mib.chest_res.snr_db, *tti_cnt, sfn_offset);
        }

        reset();
        return SFN_FOUND;
      case SRSLTE_UE_MIB_NOTFOUND:
        Info("SYNC:  Found PSS but could not decode MIB. SNR=%.1f dB (%d/%d)\n", ue_mib.chest_res.snr_db, cnt, timeout);
        return SFN_NOFOUND;
    }
  }

  return IDLE;
}

/**********
 * PHY measurements
 *
 */

void sync::set_inter_frequency_measurement(uint32_t cc_idx, uint32_t earfcn_, srslte_cell_t cell_)
{
  if (cc_idx < intra_freq_meas.size()) {
    intra_freq_meas[cc_idx]->set_primary_cell(earfcn_, cell_);
  }
}
void sync::set_cells_to_meas(uint32_t earfcn_, const std::set<uint32_t>& pci)
{
  bool found = false;
  for (size_t i = 0; i < intra_freq_meas.size() and not found; i++) {
    if (earfcn_ == intra_freq_meas[i]->get_earfcn()) {
      intra_freq_meas[i]->set_cells_to_meas(pci);
      found = true;
    }
  }
  if (!found) {
    log_h->error("Neighbour cell measurement not supported in secondary carrier. EARFCN=%d\n", earfcn_);
  }
}

void sync::meas_stop()
{
  for (auto& q : intra_freq_meas) {
    q->meas_stop();
  }
}

} // namespace srsue
