/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <unistd.h>
#include <algorithm>
#include "srslte/srslte.h"
#include "srslte/common/log.h"
#include "srsue/hdr/phy/phch_worker.h"
#include "srsue/hdr/phy/phch_recv.h"

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug(fmt, ##__VA_ARGS__)

namespace srsue {

int radio_recv_callback(void *obj, cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *rx_time) {
  return ((phch_recv*) obj)->radio_recv_fnc(data, nsamples, rx_time);
}

double callback_set_rx_gain(void *h, double gain) {
  return ((phch_recv*) h)->set_rx_gain(gain);
}



phch_recv::phch_recv() {
  dl_freq = -1;
  ul_freq = -1;
  bzero(&cell, sizeof(srslte_cell_t));
  bzero(&metrics, sizeof(sync_metrics_t));
  running = false;
  worker_com = NULL;
}

void phch_recv::init(srslte::radio_multi *_radio_handler, mac_interface_phy *_mac, rrc_interface_phy *_rrc,
                     prach *_prach_buffer, srslte::thread_pool *_workers_pool,
                     phch_common *_worker_com, srslte::log *_log_h, srslte::log *_log_phy_lib_h, uint32_t nof_rx_antennas_, uint32_t prio,
                     int sync_cpu_affinity)
{
  radio_h = _radio_handler;
  log_h   = _log_h;
  log_phy_lib_h = _log_phy_lib_h;
  mac     = _mac;
  rrc     = _rrc;
  workers_pool    = _workers_pool;
  worker_com      = _worker_com;
  prach_buffer    = _prach_buffer;
  nof_rx_antennas = nof_rx_antennas_;

  for (uint32_t i = 0; i < nof_rx_antennas; i++) {
    sf_buffer[i] = (cf_t *) srslte_vec_malloc(sizeof(cf_t) * 3 * SRSLTE_SF_LEN_PRB(100));
  }

  if (srslte_ue_sync_init_multi(&ue_sync, SRSLTE_MAX_PRB, false, radio_recv_callback, nof_rx_antennas, this)) {
    Error("SYNC:  Initiating ue_sync\n");
    return;
  }

  nof_tx_mutex = MUTEX_X_WORKER * workers_pool->get_nof_workers();
  worker_com->set_nof_mutex(nof_tx_mutex);

  // Initialize cell searcher
  search_p.init(sf_buffer, log_h, nof_rx_antennas, this);

  // Initialize SFN synchronizer
  sfn_p.init(&ue_sync, sf_buffer, log_h);

  // Initialize measurement class for the primary cell 
  measure_p.init(sf_buffer, log_h, nof_rx_antennas);

  // Start intra-frequency measurement
  intra_freq_meas.init(worker_com, rrc, log_h);

  reset();
  running = true;
  // Start main thread
  if (sync_cpu_affinity < 0) {
    start(prio);
  } else {
    start_cpu(prio, sync_cpu_affinity);
  }
}

phch_recv::~phch_recv() {
  for (uint32_t i = 0; i < nof_rx_antennas; i++) {
    if (sf_buffer[i]) {
      free(sf_buffer[i]);
    }
  }
  srslte_ue_sync_free(&ue_sync);
}

void phch_recv::stop()
{
  intra_freq_meas.stop();
  running = false;
  wait_thread_finish();
}

void phch_recv::reset()
{
  in_sync_cnt = 0;
  out_of_sync_cnt = 0;
  tx_mutex_cnt = 0;
  phy_state = IDLE;
  time_adv_sec = 0;
  next_offset  = 0;
  cell_is_set = false;
  srate_mode = SRATE_NONE;
  current_earfcn = 0;
  sfn_p.reset();
  measure_p.reset();
  search_p.reset();

}

void phch_recv::radio_error()
{
  log_h->error("SYNC:  Receiving from radio.\n");
  reset();
  phy_state = CELL_SEARCH;
  // Need to find a method to effectively reset radio, reloading the driver does not work
  radio_h->reset();
}

void phch_recv::set_cfo(float cfo) {
  srslte_ue_sync_set_cfo_ref(&ue_sync, cfo);
}

void phch_recv::set_agc_enable(bool enable)
{
  do_agc = enable;
  if (do_agc) {
    if (running && radio_h) {
      srslte_ue_sync_start_agc(&ue_sync, callback_set_rx_gain, radio_h->get_rx_gain());
      search_p.set_agc_enable(true);
    } else {
      fprintf(stderr, "Error setting AGC: PHY not initiatec\n");
    }
  } else {
    fprintf(stderr, "Error stopping AGC: not implemented\n");
  }
}

void phch_recv::set_time_adv_sec(float time_adv_sec)
{
  this->time_adv_sec = time_adv_sec;
}

void phch_recv::set_ue_sync_opts(srslte_ue_sync_t *q, float cfo)
{
  if (worker_com->args->cfo_integer_enabled) {
    srslte_ue_sync_set_cfo_i_enable(q, true);
  }

  srslte_ue_sync_set_cfo_ema(q, worker_com->args->cfo_pss_ema);
  srslte_ue_sync_set_cfo_tol(q, worker_com->args->cfo_correct_tol_hz);
  srslte_ue_sync_set_cfo_loop_bw(q, worker_com->args->cfo_loop_bw_pss, worker_com->args->cfo_loop_bw_ref,
                                 worker_com->args->cfo_loop_pss_tol,
                                 worker_com->args->cfo_loop_ref_min,
                                 worker_com->args->cfo_loop_pss_tol,
                                 worker_com->args->cfo_loop_pss_conv);

  q->strack.pss.chest_on_filter = worker_com->args->sic_pss_enabled;

  // Disable CP based CFO estimation during find
  if (cfo != 0) {
    q->cfo_current_value = cfo/15000;
    q->cfo_is_copied = true;
    q->cfo_correct_enable_find = true;
    srslte_sync_set_cfo_cp_enable(&q->sfind, false, 0);
  }

  int time_correct_period = worker_com->args->time_correct_period;
  if (time_correct_period > 0) {
    srslte_ue_sync_set_sample_offset_correct_period(q, time_correct_period);
  }

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
  srslte_sync_set_sss_algorithm(&q->strack, (sss_alg_t) sss_alg);
  srslte_sync_set_sss_algorithm(&q->sfind, (sss_alg_t) sss_alg);
}

bool phch_recv::set_cell() {
  cell_is_set = false;

  // Set cell in all objects
  if (srslte_ue_sync_set_cell(&ue_sync, cell)) {
    Error("SYNC:  Setting cell: initiating ue_sync\n");
    return false;
  }
  measure_p.set_cell(cell);
  sfn_p.set_cell(cell);
  worker_com->set_cell(cell);
  intra_freq_meas.set_primay_cell(current_earfcn, cell);

  for (uint32_t i = 0; i < workers_pool->get_nof_workers(); i++) {
    if (!((phch_worker *) workers_pool->get_worker(i))->set_cell(cell)) {
      Error("SYNC:  Setting cell: initiating PHCH worker\n");
      return false;
    }
  }

  // Set options defined in expert section
  set_ue_sync_opts(&ue_sync, search_p.get_last_cfo());

  // Reset ue_sync and set CFO/gain from search procedure
  srslte_ue_sync_reset(&ue_sync);

  cell_is_set = true;

  return cell_is_set;
}

void phch_recv::set_earfcn(std::vector<uint32_t> earfcn) {
  this->earfcn = earfcn;
}

void phch_recv::force_freq(float dl_freq, float ul_freq) {
  this->dl_freq = dl_freq;
  this->ul_freq = ul_freq;
}

void phch_recv::reset_sync() {

  Info("SYNC:  Reset. Going to Cell Select\n");
  sfn_p.reset();
  search_p.reset();
  measure_p.reset();
  srslte_ue_sync_reset(&ue_sync);
  phy_state = CELL_SELECT;
}

void phch_recv::cell_search_inc()
{
  cur_earfcn_index++;
  if (cur_earfcn_index >= 0) {
    if (cur_earfcn_index >= (int) earfcn.size()) {
      Info("SYNC:  Cell Search finished. Going to IDLE\n");
      cur_earfcn_index = 0;
      phy_state = IDLE;
      rrc->earfcn_end();
    } else {
      Info("SYNC:  Cell Search idx %d/%zu\n", cur_earfcn_index, earfcn.size());
      if (current_earfcn != earfcn[cur_earfcn_index]) {
        current_earfcn = earfcn[cur_earfcn_index];
        set_frequency();
      }
      phy_state = CELL_SEARCH;
    }
  }
}

void phch_recv::cell_search_next(bool reset) {
  if (reset) {
    cur_earfcn_index = -1;
  }
  cell_search_inc();
}

void phch_recv::cell_search_start() {
  if (phy_state == CELL_CAMP) {
    Warning("SYNC:  Can't start cell search procedure while camping on cell\n");
  } else {
    if (earfcn.size() > 0) {
      Info("SYNC:  Starting Cell Search procedure in %zu EARFCNs...\n", earfcn.size());
      cell_search_next(true);
    } else {
      Info("SYNC:  Empty EARFCN list. Stopping cell search...\n");
      log_h->console("Empty EARFCN list. Stopping cell search...\n");
    }
  }
}

bool phch_recv::cell_handover(srslte_cell_t cell)
{
  if (!srslte_cell_isvalid(&cell)) {
    log_h->error("Received HO command to invalid cell. ID=%d, PRB=%d, ports=%d\n", cell.id, cell.nof_prb, cell.nof_ports);
    return false;
  }

  int cnt = 0;
  while(worker_com->is_any_pending_ack() && cnt < 10) {
    usleep(1000);
    cnt++;
    log_h->info("Cell HO: Waiting pending PHICH\n");
  }

  bool ret = false;
  this->cell = cell;
  Info("Cell HO: Stopping sync with current cell\n");
  phy_state = IDLE;
  cnt = 0;
  while(!is_in_idle && cnt<20) {
    usleep(1000);
    cnt++;
  }
  for(uint32_t i=0;i<workers_pool->get_nof_workers();i++) {
    ((phch_worker*) workers_pool->get_worker(i))->reset();
  }
  worker_com->reset();
  if (is_in_idle) {
    Info("Cell HO: Reconfiguring cell\n");
    if (set_cell()) {
      Info("Cell HO: Synchronizing with new cell\n");
      phy_state = CELL_SELECT;
      ret = true;
    } else {
      log_h->error("Cell HO: Configuring cell PCI=%d\n", cell.id);
      ret = false;
    }
  } else {
    log_h->error("Cell HO: Could not stop sync\n");
    ret = false;
  }
  return ret;
}

/* interface from higher layers to select a new cell */
void phch_recv::cell_select(uint32_t earfcn, srslte_cell_t cell)
{
  Info("SYNC:  Cell Reselect to EARFCN=%d, PCI=%d\n", earfcn, cell.id);
  new_earfcn = earfcn;
  new_cell = cell;
  phy_state = CELL_RESELECT;
}

/* Perform cell (re)-selection on IDLE or CAMP */
void phch_recv::cell_reselect()
{
  uint32_t earfcn = new_earfcn;
  srslte_cell_t cell = new_cell;

  reset_sync();

  // If we are already in the new cell, just resynchronize
  if (earfcn == current_earfcn && this->cell.id == cell.id) {
    log_h->info("Cell Select: Already in cell EARFCN=%d, PCI=%d\n", earfcn, cell.id);
    if (srate_mode != SRATE_CAMP) {
      set_sampling_rate();
      log_h->info("Cell Select: Setting Camping sampling rate\n");
    }
  } else {

    if (earfcn != current_earfcn) {
      if (set_frequency()) {
        log_h->error("Cell Select: Configuring cell in EARFCN=%d, PCI=%d\n", earfcn, cell.id);
      }
      current_earfcn = earfcn;
    }

    this->cell = cell;
    log_h->info("Cell Select: Configuring cell...\n");

    if (set_cell()) {
      log_h->info("Cell Select: Synchronizing on cell...\n");
    }
  }
}

bool phch_recv::set_frequency()
{
  double set_dl_freq = 0;
  double set_ul_freq = 0;

  if (this->dl_freq > 0 && this->ul_freq > 0) {
    set_dl_freq = this->dl_freq;
    set_ul_freq = this->ul_freq;
  } else {
    set_dl_freq = 1e6*srslte_band_fd(current_earfcn);
    set_ul_freq = 1e6*srslte_band_fu(srslte_band_ul_earfcn(current_earfcn));
  }
  if (set_dl_freq > 0 && set_ul_freq > 0) {
    log_h->info("SYNC:  Set DL EARFCN=%d, f_dl=%.1f MHz, f_ul=%.1f MHz\n",
                current_earfcn, set_dl_freq / 1e6, set_ul_freq / 1e6);

    log_h->console("Searching cell in DL EARFCN=%d, f_dl=%.1f MHz, f_ul=%.1f MHz\n",
                current_earfcn, set_dl_freq / 1e6, set_ul_freq / 1e6);

    radio_h->set_rx_freq(set_dl_freq);
    radio_h->set_tx_freq(set_ul_freq);
    ul_dl_factor = radio_h->get_tx_freq()/radio_h->get_rx_freq();

    srslte_ue_sync_reset(&ue_sync);

    return true;
  } else {
    log_h->error("SYNC:  Cell Search: Invalid EARFCN=%d\n", current_earfcn);
    return false;
  }
}

float phch_recv::get_cfo()
{
  float cfo = srslte_ue_sync_get_cfo(&ue_sync);

  float ret = cfo*ul_dl_factor;

  if (worker_com->args->cfo_is_doppler) {
    ret *= -1;
  }

  if (radio_h->get_freq_offset() != 0.0f) {
    /* Compensates the radio frequency offset applied equally to DL and UL */
    const float offset_hz = (float) radio_h->get_freq_offset() * (1.0f - ul_dl_factor);
    ret = cfo - offset_hz;
  }

  return ret/15000;
}

void phch_recv::set_sampling_rate()
{
  current_srate = (float) srslte_sampling_freq_hz(cell.nof_prb);
  current_sflen = SRSLTE_SF_LEN_PRB(cell.nof_prb);
  if (current_srate != -1) {
    Info("SYNC:  Setting sampling rate %.2f MHz\n", current_srate/1000000);

#if 0
    if (((int) current_srate / 1000) % 3072 == 0) {
      radio_h->set_master_clock_rate(30.72e6);
    } else {
      radio_h->set_master_clock_rate(23.04e6);
    }
#else
      if (current_srate < 10e6) {
        radio_h->set_master_clock_rate(4 * current_srate);
      } else {
        radio_h->set_master_clock_rate(current_srate);
      }
#endif

    srate_mode = SRATE_CAMP;
    radio_h->set_rx_srate(current_srate);
    radio_h->set_tx_srate(current_srate);
  } else {
    Error("Error setting sampling rate for cell with %d PRBs\n", cell.nof_prb);
  }
}

uint32_t phch_recv::get_current_tti() {
  return tti;
}

bool phch_recv::status_is_sync() {
  return phy_state == CELL_CAMP;
}

void phch_recv::get_current_cell(srslte_cell_t *cell_, uint32_t *earfcn) {
  if (cell_) {
    memcpy(cell_, &cell, sizeof(srslte_cell_t));
  }
  if (earfcn) {
    *earfcn = current_earfcn;
  }
}

int phch_recv::radio_recv_fnc(cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *rx_time)
{
  if (radio_h->rx_now(data, nsamples, rx_time)) {
    int offset = nsamples - current_sflen;
    if (abs(offset) < 10 && offset != 0) {
      next_offset = offset;
    } else if (nsamples < 10) {
      next_offset = nsamples;
    }

    log_h->debug("SYNC:  received %d samples from radio\n", nsamples);

    return nsamples;
  } else {
    return -1;
  }
}

double phch_recv::set_rx_gain(double gain) {
  return radio_h->set_rx_gain_th(gain);
}




/**
 * MAIN THREAD
 */

void phch_recv::run_thread()
{
  phch_worker *worker = NULL;
  phch_worker *last_worker = NULL;
  cf_t *buffer[SRSLTE_MAX_PORTS] = {NULL};
  uint32_t sf_idx = 0;
  phy_state  = IDLE;
  is_in_idle = true;

  cf_t *dummy_buffer[SRSLTE_MAX_PORTS];

  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    dummy_buffer[i] = (cf_t*) malloc(sizeof(cf_t)*SRSLTE_SF_LEN_PRB(100));
  }

  while (running)
  {
    if (phy_state != IDLE) {
      is_in_idle = false;
    }
    Debug("SYNC:  state=%d\n", phy_state);

    log_h->step(tti);
    log_phy_lib_h->step(tti);
    
    sf_idx = tti%10;

    prev_state = phy_state;

    switch (phy_state) {
      case CELL_SEARCH:
        switch(search_p.run(&cell))
        {
        case search::CELL_FOUND:
          if (!srslte_cell_isvalid(&cell)) {
            Error("SYNC:  Detected invalid cell. Going to IDLE\n");
            phy_state = IDLE;
            break;
          }
          if (set_cell()) {
            Info("SYNC:  Setting sampling rate and going to Cell Select\n");
            set_sampling_rate();
            phy_state = CELL_SELECT;
          }
          break;
        case search::CELL_NOT_FOUND:
          cell_search_inc();
          break;
        default:
          radio_error();
          break;
        }
        break;
      case CELL_RESELECT:
        cell_reselect();
        break;
      case CELL_SELECT:
        switch (sfn_p.run_subframe(&cell, &tti))
        {
          case sfn_sync::SFN_FOUND:
            if (prev_state != CELL_SEARCH) {
              log_h->info("Sync OK. Camping on cell PCI=%d...\n", cell.id);
              phy_state = CELL_CAMP;
              rrc->cell_camping(earfcn[cur_earfcn_index], cell);
            } else {
              log_h->info("Sync OK. Measuring PCI=%d...\n", cell.id);
              measure_p.reset();
              phy_state = CELL_MEASURE;
            }
            break;
          case sfn_sync::TIMEOUT:
            log_h->warning("SYNC:  Timeout while synchronizing SFN\n");
            rrc->out_of_sync();
            break;
          case sfn_sync::IDLE:
            break;
          default:
            radio_error();
            break;
        }
        break;
      case CELL_MEASURE:

        switch(measure_p.run_subframe_sync(&ue_sync, sf_idx))
        {
          case measure::MEASURE_OK:

            // Calibrate measure object since worker not yet calibrated
            if (worker_com->args->rssi_sensor_enabled) {
              measure_p.set_rx_gain_offset(measure_p.rssi() - radio_h->get_rssi() + 30);
            } else {
              measure_p.set_rx_gain_offset(worker_com->args->rx_gain_offset + radio_h->get_rx_gain());
            }

            log_h->info("SYNC:  Measured OK. Camping on cell PCI=%d...\n", cell.id);
            phy_state = CELL_CAMP;
            rrc->cell_camping(earfcn[cur_earfcn_index], cell, measure_p.rsrp());
            break;
          case measure::IDLE:
            break;
          default:
            radio_error();
            break;
        }
        break;
      case CELL_CAMP:

        worker = (phch_worker *) workers_pool->wait_worker(tti);
        if (worker) {
          for (uint32_t i = 0; i < SRSLTE_MAX_PORTS; i++) {
            buffer[i] = worker->get_buffer(i);
          }

          switch(srslte_ue_sync_zerocopy_multi(&ue_sync, buffer)) {
            case 1:

              if (last_worker) {
                Debug("SF: cfo_tot=%7.1f Hz, ref=%f Hz, pss=%f Hz, snr_sf=%.2f dB, rsrp=%.2f dB, noise=%.2f dB\n",
                        srslte_ue_sync_get_cfo(&ue_sync),
                     15000*last_worker->get_ref_cfo(),
                     15000*ue_sync.strack.cfo_pss_mean,
                     last_worker->get_snr(), last_worker->get_rsrp(), last_worker->get_noise());
              }

              last_worker = worker;

              Debug("SYNC:  Worker %d synchronized\n", worker->get_id());

              metrics.sfo = srslte_ue_sync_get_sfo(&ue_sync);
              metrics.cfo = srslte_ue_sync_get_cfo(&ue_sync);
              worker->set_cfo(get_cfo());
              worker_com->set_sync_metrics(metrics);

              /* Compute TX time: Any transmission happens in TTI+4 thus advance 4 ms the reception time */
              srslte_timestamp_t rx_time, tx_time, tx_time_prach;
              srslte_ue_sync_get_last_timestamp(&ue_sync, &rx_time);
              srslte_timestamp_copy(&tx_time, &rx_time);
              srslte_timestamp_add(&tx_time, 0, HARQ_DELAY_MS*1e-3 - time_adv_sec);
              worker->set_tx_time(tx_time, next_offset);
              next_offset = 0;

              Debug("SYNC:  Setting TTI=%d, tx_mutex=%d to worker %d\n", tti, tx_mutex_cnt, worker->get_id());
              worker->set_tti(tti, tx_mutex_cnt);
              tx_mutex_cnt = (tx_mutex_cnt+1) % nof_tx_mutex;

              // Reset Uplink TX buffer to avoid mixing packets in TX queue
              /*
              if (prach_buffer->is_pending()) {
                Info("SYNC:  PRACH pending: Reset UL\n");
                radio_h->tx_end();
              }*/

              // Check if we need to TX a PRACH
              if (prach_buffer->is_ready_to_send(tti)) {
                srslte_timestamp_copy(&tx_time_prach, &rx_time);
                srslte_timestamp_add(&tx_time_prach, 0, prach::tx_advance_sf * 1e-3);
                prach_buffer->send(radio_h, get_cfo(), worker_com->pathloss, tx_time_prach);
                radio_h->tx_end();
                worker_com->p0_preamble = prach_buffer->get_p0_preamble();
                worker_com->cur_radio_power = SRSLTE_MIN(SRSLTE_PC_MAX, worker_com->pathloss+worker_com->p0_preamble);
              }

              workers_pool->start_worker(worker);

              if ((tti%5) == 0 && worker_com->args->sic_pss_enabled) {
                srslte_pss_sic(&ue_sync.strack.pss, &buffer[0][SRSLTE_SF_LEN_PRB(cell.nof_prb)/2-ue_sync.strack.fft_size]);
              }
              if (srslte_cell_isvalid(&cell)) {
                intra_freq_meas.write(tti, buffer[0], SRSLTE_SF_LEN_PRB(cell.nof_prb));
              }
              break;
            case 0:
              Warning("SYNC:  Out-of-sync detected in PSS/SSS\n");
              out_of_sync();
              worker->release();
              worker_com->reset_ul();
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
      case IDLE:
        if (radio_h->is_init()) {
          uint32_t nsamples = 1920;
          if (current_srate > 0) {
            nsamples = current_srate/1000;
          }
          if (!radio_h->rx_now(dummy_buffer, nsamples, NULL)) {
            printf("SYNC:  Receiving from radio while in IDLE_RX\n");
          }
        } else {
          usleep(1000);
        }
        is_in_idle = true;
        break;
    }

    // Increase TTI counter and trigger MAC clock (lower priority)
    mac->tti_clock(tti);
    tti = (tti+1) % 10240;
  }

  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    if (dummy_buffer[i]) {
      free(dummy_buffer[i]);
    }
  }
}

void phch_recv::in_sync() {
  in_sync_cnt++;
  // Send RRC in-sync signal after 100 ms consecutive subframes
  if (in_sync_cnt == NOF_IN_SYNC_SF) {
    rrc->in_sync();
    in_sync_cnt = 0;
    out_of_sync_cnt = 0;
  }
}

// Out of sync called by worker or phch_recv every 1 or 5 ms
void phch_recv::out_of_sync() {
  // Send RRC out-of-sync signal after 200 ms consecutive subframes
  Info("Out-of-sync %d/%d\n", out_of_sync_cnt, NOF_OUT_OF_SYNC_SF);
  out_of_sync_cnt++;
  if (out_of_sync_cnt >= NOF_OUT_OF_SYNC_SF) {
    Info("Sending to RRC\n");
    rrc->out_of_sync();
    out_of_sync_cnt = 0;
    in_sync_cnt = 0;
  }
}












/*********
 * Cell search class
 */
phch_recv::search::~search()
{
  srslte_ue_mib_sync_free(&ue_mib_sync);
  srslte_ue_cellsearch_free(&cs);
}

void phch_recv::search::init(cf_t *buffer[SRSLTE_MAX_PORTS], srslte::log *log_h, uint32_t nof_rx_antennas, phch_recv *parent)
{
  this->log_h = log_h;
  this->p     = parent;

  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    this->buffer[i] = buffer[i];
  }

  if (srslte_ue_cellsearch_init_multi(&cs, 5, radio_recv_callback, nof_rx_antennas, parent)) {
    Error("SYNC:  Initiating UE cell search\n");
  }

  if (srslte_ue_mib_sync_init_multi(&ue_mib_sync, radio_recv_callback, nof_rx_antennas, parent)) {
    Error("SYNC:  Initiating UE MIB synchronization\n");
  }

  srslte_ue_cellsearch_set_nof_valid_frames(&cs, 2);

  // Set options defined in expert section
  p->set_ue_sync_opts(&cs.ue_sync, 0);

  force_N_id_2 = -1;
}

void phch_recv::search::set_N_id_2(int N_id_2) {
  force_N_id_2 = N_id_2;
}

void phch_recv::search::reset()
{
  srslte_ue_sync_reset(&ue_mib_sync.ue_sync);
}

float phch_recv::search::get_last_gain()
{
  return srslte_agc_get_gain(&ue_mib_sync.ue_sync.agc);
}

float phch_recv::search::get_last_cfo()
{
  return srslte_ue_sync_get_cfo(&ue_mib_sync.ue_sync);
}

void phch_recv::search::set_agc_enable(bool enable) {
  if (enable) {
    srslte_ue_sync_start_agc(&ue_mib_sync.ue_sync, callback_set_rx_gain, p->radio_h->get_rx_gain());
  } else {
    fprintf(stderr, "Error stop AGC not implemented\n");
  }
}

phch_recv::search::ret_code phch_recv::search::run(srslte_cell_t *cell)
{

  if (!cell) {
    return ERROR;
  }

  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];

  srslte_ue_cellsearch_result_t found_cells[3];

  bzero(cell, sizeof(srslte_cell_t));
  bzero(found_cells, 3 * sizeof(srslte_ue_cellsearch_result_t));

  if (p->srate_mode != SRATE_FIND) {
    p->srate_mode = SRATE_FIND;
    p->radio_h->set_rx_srate(1.92e6);
    Info("SYNC:  Setting Cell Search sampling rate\n");
  }

  /* Find a cell in the given N_id_2 or go through the 3 of them to find the strongest */
  uint32_t max_peak_cell = 0;
  int ret = SRSLTE_ERROR;

  Info("SYNC:  Searching for cell...\n");
  printf("."); fflush(stdout);

  if (force_N_id_2 >= 0 && force_N_id_2 < 3) {
    ret = srslte_ue_cellsearch_scan_N_id_2(&cs, force_N_id_2, &found_cells[force_N_id_2]);
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
  cell->id  = found_cells[max_peak_cell].cell_id;
  cell->cp  = found_cells[max_peak_cell].cp;
  float cfo = found_cells[max_peak_cell].cfo;

  printf("\n");
  Info("SYNC:  PSS/SSS detected: PCI=%d, CFO=%.1f KHz, CP=%s\n",
       cell->id, cfo/1000, srslte_cp_string(cell->cp));

  if (srslte_ue_mib_sync_set_cell(&ue_mib_sync, cell->id, cell->cp)) {
    Error("SYNC:  Setting UE MIB cell\n");
    return ERROR;
  }

  // Set options defined in expert section
  p->set_ue_sync_opts(&ue_mib_sync.ue_sync, cfo);

  srslte_ue_sync_reset(&ue_mib_sync.ue_sync);

  /* Find and decode MIB */
  int sfn_offset;
  ret = srslte_ue_mib_sync_decode(&ue_mib_sync,
                                  40,
                                  bch_payload, &cell->nof_ports, &sfn_offset);
  if (ret == 1) {
    srslte_pbch_mib_unpack(bch_payload, cell, NULL);

    fprintf(stdout, "Found Cell:  PCI=%d, PRB=%d, Ports=%d, CFO=%.1f KHz\n",
            cell->id, cell->nof_prb, cell->nof_ports, cfo/1000);

    Info("SYNC:  MIB Decoded: PCI=%d, PRB=%d, Ports=%d, CFO=%.1f KHz\n",
         cell->id, cell->nof_prb, cell->nof_ports, cfo/1000);

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

phch_recv::sfn_sync::~sfn_sync()
{
  srslte_ue_mib_free(&ue_mib);
}

void phch_recv::sfn_sync::init(srslte_ue_sync_t *ue_sync, cf_t *buffer[SRSLTE_MAX_PORTS], srslte::log *log_h, uint32_t timeout)
{
  this->log_h   = log_h;
  this->ue_sync = ue_sync;
  this->timeout = timeout;

  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    this->buffer[i] = buffer[i];
  }

  if (srslte_ue_mib_init(&ue_mib, this->buffer, SRSLTE_MAX_PRB)) {
    Error("SYNC:  Initiating UE MIB decoder\n");
  }
}

bool phch_recv::sfn_sync::set_cell(srslte_cell_t cell)
{
  if (srslte_ue_mib_set_cell(&ue_mib, cell)) {
    Error("SYNC:  Setting cell: initiating ue_mib\n");
    return false;
  }
  reset();
  return true;
}

void phch_recv::sfn_sync::reset()
{
  srslte_ue_mib_reset(&ue_mib);
  cnt = 0;
}

phch_recv::sfn_sync::ret_code phch_recv::sfn_sync::run_subframe(srslte_cell_t *cell, uint32_t *tti_cnt, bool sfidx_only)
{

  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];

  srslte_ue_sync_decode_sss_on_track(ue_sync, true);
  int ret = srslte_ue_sync_zerocopy_multi(ue_sync, buffer);
  if (ret < 0) {
    Error("SYNC:  Error calling ue_sync_get_buffer.\n");
    return ERROR;
  }

  if (ret == 1) {
    if (srslte_ue_sync_get_sfidx(ue_sync) == 0) {

      // Skip MIB decoding if we are only interested in subframe 0
      if (sfidx_only) {
        if (tti_cnt) {
          *tti_cnt = 0;
        }
        return SFX0_FOUND;
      }

      int sfn_offset = 0;
      Info("SYNC:  Trying to decode MIB... SNR=%.1f dB\n", 10*log10(srslte_chest_dl_get_snr(&ue_mib.chest)));

      int n = srslte_ue_mib_decode(&ue_mib, bch_payload, NULL, &sfn_offset);
      if (n < 0) {
        Error("SYNC:  Error decoding MIB while synchronising SFN");
        return ERROR;
      } else if (n == SRSLTE_UE_MIB_FOUND) {
        uint32_t sfn;
        srslte_pbch_mib_unpack(bch_payload, cell, &sfn);

        sfn = (sfn+sfn_offset)%1024;
        if (tti_cnt) {
          *tti_cnt = 10*sfn;
          Info("SYNC:  DONE, TTI=%d, sfn_offset=%d\n", *tti_cnt, sfn_offset);
        }

        srslte_ue_sync_decode_sss_on_track(ue_sync, true);
        reset();
        return SFN_FOUND;
      }
    }
  } else {
    Debug("SYNC:  PSS/SSS not found...\n");
  }

  cnt++;
  if (cnt >= timeout) {
    cnt = 0;
    return TIMEOUT;
  }

  return IDLE;
}






/*********
 * Measurement class 
 */
void phch_recv::measure::init(cf_t *buffer[SRSLTE_MAX_PORTS], srslte::log *log_h, uint32_t nof_rx_antennas, uint32_t nof_subframes)

{
  this->log_h         = log_h;
  this->nof_subframes = nof_subframes;
  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    this->buffer[i] = buffer[i]; 
  }

  if (srslte_ue_dl_init(&ue_dl, this->buffer, SRSLTE_MAX_PRB, nof_rx_antennas)) {
    Error("SYNC:  Initiating ue_dl_measure\n");
    return;
  }
  srslte_chest_dl_set_rsrp_neighbour(&ue_dl.chest, true);
  reset();
}

phch_recv::measure::~measure() {
  srslte_ue_dl_free(&ue_dl);
}
  
void phch_recv::measure::reset() {
  cnt       = 0; 
  mean_rsrp = 0;
  mean_rsrq = 0;
  mean_snr  = 0;
  mean_rssi = 0;
}

void phch_recv::measure::set_cell(srslte_cell_t cell) 
{
  current_prb = cell.nof_prb;
  if (srslte_ue_dl_set_cell(&ue_dl, cell)) {
    Error("SYNC:  Setting cell: initiating ue_dl_measure\n");
  }
  reset();
}

float phch_recv::measure::rssi() {
  return 10*log10(mean_rssi);
}

float phch_recv::measure::rsrp() {
  return 10*log10(mean_rsrp) + 30 - rx_gain_offset;
}

float phch_recv::measure::rsrq() {
  return 10*log10(mean_rsrq);
}

float phch_recv::measure::snr() {
  return 10*log10(mean_snr);
}

uint32_t phch_recv::measure::frame_st_idx() {
  return final_offset;
}

void phch_recv::measure::set_rx_gain_offset(float rx_gain_offset) {
  this->rx_gain_offset  = rx_gain_offset;
}

phch_recv::measure::ret_code phch_recv::measure::run_subframe_sync(srslte_ue_sync_t *ue_sync, uint32_t sf_idx)
{
  int sync_res = srslte_ue_sync_zerocopy_multi(ue_sync, buffer);
  if (sync_res == 1) {
    log_h->info("SYNC: CFO=%.1f KHz\n", srslte_ue_sync_get_cfo(ue_sync)/1000);
    return run_subframe(sf_idx);
  } else {
    log_h->error("SYNC:  Measuring RSRP: Sync error\n");
    return ERROR;
  }

  return IDLE;
}

phch_recv::measure::ret_code phch_recv::measure::run_multiple_subframes(cf_t *input_buffer,
                                                                        int offset,
                                                                        uint32_t sf_idx,
                                                                        uint32_t max_sf)
{
  uint32_t sf_len = SRSLTE_SF_LEN_PRB(current_prb);

  ret_code ret = IDLE;

  offset = offset-sf_len/2;
  while (offset < 0 && sf_idx < max_sf) {
    offset += sf_len;
    sf_idx ++;
  }


  // Fine-tune offset using RS
#ifdef FINE_TUNE_OFFSET_WITH_RS
  float max_rsrp = -200;
  int best_test_offset = 0;
  int test_offset = 0;
  bool found_best = false;
  for (uint32_t n=0;n<5;n++) {

    test_offset = offset-2+n;
    if (test_offset >= 0) {

      cf_t *buf_m[SRSLTE_MAX_PORTS];
      buf_m[0] = &input_buffer[test_offset];

      uint32_t cfi;
      if (srslte_ue_dl_decode_fft_estimate_noguru(&ue_dl, buf_m, sf_idx, &cfi)) {
        Error("MEAS:  Measuring RSRP: Estimating channel\n");
        return ERROR;
      }

      float rsrp = srslte_chest_dl_get_rsrp(&ue_dl.chest);
      if (rsrp > max_rsrp) {
        max_rsrp = rsrp;
        best_test_offset = test_offset;
        found_best = true;
      }
    }
  }

  Debug("INTRA: fine-tuning offset: %d, found_best=%d, rem_sf=%d\n", offset, found_best, nof_sf);

  offset = found_best?best_test_offset:offset;
#endif

  if (offset >= 0 && offset < (int)(sf_len*max_sf)) {
    uint32_t nof_sf = (sf_len*max_sf - offset)/sf_len;


    final_offset = offset;

    for (uint32_t i=0;i<nof_sf;i++) {
      memcpy(buffer[0], &input_buffer[offset+i*sf_len], sizeof(cf_t)*sf_len);
      ret = run_subframe((sf_idx+i)%10);
      if (ret != IDLE) {
        return ret;
      }
    }
    if (ret != ERROR) {
      return MEASURE_OK;
    }
  } else {
    Info("INTRA: not running because offset=%d, sf_len*max_sf=%d*%d\n", offset, sf_len, max_sf);
  }
  return ret;
}

phch_recv::measure::ret_code phch_recv::measure::run_subframe(uint32_t sf_idx)
{
  uint32_t cfi = 0;
  if (srslte_ue_dl_decode_fft_estimate(&ue_dl, sf_idx, &cfi)) {
    log_h->error("SYNC:  Measuring RSRP: Estimating channel\n");
    return ERROR;
  }

  float rsrp   = srslte_chest_dl_get_rsrp_neighbour(&ue_dl.chest);
  float rsrq   = srslte_chest_dl_get_rsrq(&ue_dl.chest);
  float snr    = srslte_chest_dl_get_snr(&ue_dl.chest);
  float rssi   = srslte_vec_avg_power_cf(buffer[0], SRSLTE_SF_LEN_PRB(current_prb));

  if (cnt == 0) {
    mean_rsrp   = rsrp;
    mean_rsrq   = rsrq;
    mean_snr    = snr;
    mean_rssi   = rssi;
  } else {
    mean_rsrp   = SRSLTE_VEC_CMA(rsrp, mean_rsrp, cnt);
    mean_rsrq   = SRSLTE_VEC_CMA(rsrq, mean_rsrq, cnt);
    mean_snr    = SRSLTE_VEC_CMA(snr,  mean_snr,  cnt);
    mean_rssi   = SRSLTE_VEC_CMA(rssi, mean_rssi, cnt);
  }
  cnt++;

  log_h->debug("SYNC:  Measuring RSRP %d/%d, sf_idx=%d, RSRP=%.1f dBm, SNR=%.1f dB\n",
              cnt, nof_subframes, sf_idx, rsrp, snr);

  if (cnt >= nof_subframes) {
    return MEASURE_OK;
  } else {
    return IDLE;
  }
}






/**********
 * Secondary cell receiver
 */

void phch_recv::scell_recv::init(srslte::log *log_h, bool sic_pss_enabled, uint32_t max_sf_window)
{
  this->log_h           = log_h;
  this->sic_pss_enabled = sic_pss_enabled;

  // and a separate ue_sync instance

  uint32_t max_fft_sz  = srslte_symbol_sz(100);
  uint32_t max_sf_size = SRSLTE_SF_LEN(max_fft_sz);

  sf_buffer[0]        = (cf_t*) srslte_vec_malloc(sizeof(cf_t)*max_sf_size);

  measure_p.init(sf_buffer, log_h, 1, max_sf_window);

  //do this different we don't need all this search window.
  if(srslte_sync_init(&sync_find, max_sf_window*max_sf_size, 5*max_sf_size, max_fft_sz)) {
    fprintf(stderr, "Error initiating sync_find\n");
    return;
  }
  srslte_sync_set_sss_algorithm(&sync_find, SSS_FULL);
  srslte_sync_cp_en(&sync_find, false);
  srslte_sync_set_cfo_pss_enable(&sync_find, true);
  srslte_sync_set_threshold(&sync_find, 1.7);
  srslte_sync_set_em_alpha(&sync_find, 0.3);

  // Configure FIND object behaviour (this configuration is always the same)
  srslte_sync_set_cfo_ema_alpha(&sync_find,    1.0);
  srslte_sync_set_cfo_i_enable(&sync_find,     false);
  srslte_sync_set_cfo_pss_enable(&sync_find,   true);
  srslte_sync_set_pss_filt_enable(&sync_find,  true);
  srslte_sync_set_sss_eq_enable(&sync_find,    true);

  sync_find.pss.chest_on_filter = true;
  sync_find.sss_channel_equalize = false;

  reset();
}

void phch_recv::scell_recv::reset()
{
  current_fft_sz = 0;
  measure_p.reset();
}

void phch_recv::scell_recv::deinit()
{
  srslte_sync_free(&sync_find);
  free(sf_buffer[0]);
}

int phch_recv::scell_recv::find_cells(cf_t *input_buffer, float rx_gain_offset, srslte_cell_t cell, uint32_t nof_sf, cell_info_t cells[MAX_CELLS])
{
  uint32_t fft_sz  = srslte_symbol_sz(cell.nof_prb);
  uint32_t sf_len  = SRSLTE_SF_LEN(fft_sz);

  if (fft_sz != current_fft_sz) {
    if (srslte_sync_resize(&sync_find, nof_sf*sf_len, 5*sf_len, fft_sz)) {
      fprintf(stderr, "Error resizing sync nof_sf=%d, sf_len=%d, fft_sz=%d\n", nof_sf, sf_len, fft_sz);
      return SRSLTE_ERROR;
    }
    current_fft_sz = fft_sz;
  }

  int nof_cells = 0;
  uint32_t peak_idx = 0;
  uint32_t sf_idx   = 0;
  int      cell_id  = 0;

  srslte_cell_t found_cell;
  memcpy(&found_cell, &cell, sizeof(srslte_cell_t));

  measure_p.set_rx_gain_offset(rx_gain_offset);

  for (uint32_t n_id_2=0;n_id_2<3;n_id_2++) {

    found_cell.id = 10000;

    if (n_id_2 != (cell.id%3) || sic_pss_enabled) {
      srslte_sync_set_N_id_2(&sync_find, n_id_2);

      srslte_sync_find_ret_t sync_res;

      do {
        srslte_sync_reset(&sync_find);
        srslte_sync_cfo_reset(&sync_find);

        sync_res = SRSLTE_SYNC_NOFOUND;
        cell_id          = 0;
        float max_peak   = -1;
        uint32_t max_sf5 = 0;
        uint32_t max_sf_idx = 0;

        for (uint32_t sf5_cnt=0;sf5_cnt<nof_sf/5;sf5_cnt++) {
          sync_res = srslte_sync_find(&sync_find, input_buffer, sf5_cnt*5*sf_len, &peak_idx);
          Debug("INTRA: n_id_2=%d, cnt=%d/%d, sync_res=%d, sf_idx=%d, peak_idx=%d, peak_value=%f\n",
                 n_id_2, sf5_cnt, nof_sf/5, sync_res, srslte_sync_get_sf_idx(&sync_find), peak_idx, sync_find.peak_value);

          if (sync_find.peak_value > max_peak && sync_res == SRSLTE_SYNC_FOUND) {
            max_sf5    = sf5_cnt;
            max_sf_idx = srslte_sync_get_sf_idx(&sync_find);
            cell_id    = srslte_sync_get_cell_id(&sync_find);
          }
        }

        switch(sync_res) {
          case SRSLTE_SYNC_ERROR:
            return SRSLTE_ERROR;
            fprintf(stderr, "Error finding correlation peak\n");
            return SRSLTE_ERROR;
          case SRSLTE_SYNC_FOUND:

            sf_idx = (10-max_sf_idx - 5*(max_sf5%2))%10;

            if (cell_id >= 0) {
              // We found the same cell as before, look another N_id_2
              if ((uint32_t) cell_id == found_cell.id || (uint32_t) cell_id == cell.id) {
                Info("n_id_2=%d, PCI=%d, found_cell.id=%d, cell.id=%d\n", n_id_2, cell_id, found_cell.id, cell.id);
                sync_res = SRSLTE_SYNC_NOFOUND;
              } else {
                // We found a new cell ID
                found_cell.id = cell_id;
                found_cell.nof_ports = 1;  // Use port 0 only for measurement
                measure_p.set_cell(found_cell);

                switch(measure_p.run_multiple_subframes(input_buffer, peak_idx, sf_idx, nof_sf))
                {
                  case measure::MEASURE_OK:
                    // Consider a cell to be detectable 8.1.2.2.1.1 from 36.133. Currently only using first condition
                    if (measure_p.rsrp() > ABSOLUTE_RSRP_THRESHOLD_DBM) {

                      cells[nof_cells].pci = found_cell.id;
                      cells[nof_cells].rsrp = measure_p.rsrp();
                      cells[nof_cells].rsrq = measure_p.rsrq();
                      cells[nof_cells].offset = measure_p.frame_st_idx();

                      Info(
                          "INTRA: Found neighbour cell %d: PCI=%03d, RSRP=%5.1f dBm, peak_idx=%5d, peak_value=%3.2f, sf=%d, nof_sf=%d, n_id_2=%d, CFO=%6.1f Hz\n",
                          nof_cells, cell_id, measure_p.rsrp(), measure_p.frame_st_idx(), sync_find.peak_value,
                          sf_idx, nof_sf, n_id_2, 15000 * srslte_sync_get_cfo(&sync_find));

                      nof_cells++;

                      /*
                      if (sic_pss_enabled) {
                        srslte_pss_sic(&sync_find.pss, &input_buffer[sf5_cnt * 5 * sf_len + sf_len / 2 - fft_sz]);
                      }*/
                    }
                    break;
                  default:
                    Info("INTRA: Not enough samples to measure PCI=%d\n", cell_id);
                    break;
                  case measure::ERROR:
                    Error("Measuring neighbour cell\n");
                    return SRSLTE_ERROR;
                }
              }
            } else {
              sync_res = SRSLTE_SYNC_NOFOUND;
            }
            break;
          case SRSLTE_SYNC_FOUND_NOSPACE:
            /* If a peak was found but there is not enough space for SSS/CP detection, discard a few samples */
            break;
          default:
            break;
        }
      } while (sync_res == SRSLTE_SYNC_FOUND && sic_pss_enabled && nof_cells < MAX_CELLS);
    }
  }
  return nof_cells;
}



/**********
 * PHY measurements
 *
 */

void phch_recv::meas_reset() {
  // Stop all measurements
  intra_freq_meas.clear_cells();
  if (worker_com) {
    worker_com->pcell_meas_enabled = false;
  }
}

int phch_recv::meas_start(uint32_t earfcn, int pci) {
  if (earfcn == current_earfcn) {
    worker_com->pcell_meas_enabled = true;
    if (pci != (int) cell.id) {
      intra_freq_meas.add_cell(pci);
    }
    return 0;
  } else {
    Warning("INTRA: Inter-frequency measurements not supported (current EARFCN=%d, requested measurement for %d)\n",
            current_earfcn, earfcn);
    return -1;
  }
}

int phch_recv::meas_stop(uint32_t earfcn, int pci) {
  if (earfcn == current_earfcn) {
    intra_freq_meas.rem_cell(pci);
    return 0;
  } else {
    Warning("INTRA: Inter-frequency measurements not supported (current EARFCN=%d, requested stop measurement for %d)\n",
            current_earfcn, earfcn);
  }
  return -1;
}

void phch_recv::intra_measure::init(phch_common *common, rrc_interface_phy *rrc, srslte::log *log_h) {
  this->rrc    = rrc;
  this->log_h  = log_h;
  this->common = common;
  receive_enabled = false;

  // Start scell
  scell.init(log_h, common->args->sic_pss_enabled, common->args->intra_freq_meas_len_ms);

  search_buffer = (cf_t*) srslte_vec_malloc(common->args->intra_freq_meas_len_ms*SRSLTE_SF_LEN_PRB(SRSLTE_MAX_PRB)*sizeof(cf_t));

  if (srslte_ringbuffer_init(&ring_buffer, sizeof(cf_t)*common->args->intra_freq_meas_len_ms*2*SRSLTE_SF_LEN_PRB(SRSLTE_MAX_PRB))) {
    return;
  }

  running = true;
  start(INTRA_FREQ_MEAS_PRIO);
}

void phch_recv::intra_measure::stop() {
  running = false;
  srslte_ringbuffer_stop(&ring_buffer);
  tti_sync.increase();
  wait_thread_finish();
}

phch_recv::intra_measure::~intra_measure() {
  srslte_ringbuffer_free(&ring_buffer);
  scell.deinit();
  free(search_buffer);
}

void phch_recv::intra_measure::set_primay_cell(uint32_t earfcn, srslte_cell_t cell) {
  this->current_earfcn = earfcn;
  current_sflen = SRSLTE_SF_LEN_PRB(cell.nof_prb);
  memcpy(&this->primary_cell, &cell, sizeof(srslte_cell_t));
}

void phch_recv::intra_measure::clear_cells() {
  active_pci.clear();
  receive_enabled = false;
  receiving = false;
  receive_cnt = 0;
  srslte_ringbuffer_reset(&ring_buffer);
}

void phch_recv::intra_measure::add_cell(int pci) {
  if (std::find(active_pci.begin(), active_pci.end(), pci) == active_pci.end()) {
    active_pci.push_back(pci);
    receive_enabled = true;
    Info("INTRA: Starting intra-frequency measurement for pci=%d\n", pci);
  } else {
    Debug("INTRA: Requested to start already existing intra-frequency measurement for PCI=%d\n", pci);
  }
}

int phch_recv::intra_measure::get_offset(uint32_t pci) {
  for (int i=0;i<scell_recv::MAX_CELLS;i++) {
    if (info[i].pci == pci) {
      return info[i].offset;
    }
  }
  return -1;
}

void phch_recv::intra_measure::rem_cell(int pci) {
  std::vector<int>::iterator newEnd = std::remove(active_pci.begin(), active_pci.end(), pci);

  if (newEnd != active_pci.end()) {
    active_pci.erase(newEnd, active_pci.end());
    if (active_pci.size() == 0) {
      receive_enabled = false;
    }
    Info("INTRA: Stopping intra-frequency measurement for pci=%d. Number of cells: %zu\n", pci, active_pci.size());
  } else {
    Warning("INTRA: Requested to stop non-existing intra-frequency measurement for PCI=%d\n", pci);
  }
}

void phch_recv::intra_measure::write(uint32_t tti, cf_t *data, uint32_t nsamples) {
  if (receive_enabled) {
    if ((tti%common->args->intra_freq_meas_period_ms) == 0) {
      receiving   = true;
      receive_cnt = 0;
      measure_tti = tti;
      srslte_ringbuffer_reset(&ring_buffer);
    }
    if (receiving == true) {
      if (srslte_ringbuffer_write(&ring_buffer, data, nsamples*sizeof(cf_t)) < (int) (nsamples*sizeof(cf_t))) {
        Warning("Error writting to ringbuffer\n");
        receiving = false;
      } else {
        receive_cnt++;
        if (receive_cnt == common->args->intra_freq_meas_len_ms) {
          tti_sync.increase();
          receiving = false; 
        }
      }
    }
  }
}

void phch_recv::intra_measure::run_thread()
{
  while(running) {
    if (running) {
      tti_sync.wait();
    }

    if (running) {

      // Read data from buffer and find cells in it
      srslte_ringbuffer_read(&ring_buffer, search_buffer, common->args->intra_freq_meas_len_ms*current_sflen*sizeof(cf_t));
      int found_cells = scell.find_cells(search_buffer, common->rx_gain_offset, primary_cell, common->args->intra_freq_meas_len_ms, info);
      receiving = false;

      for (int i=0;i<found_cells;i++) {
        rrc->new_phy_meas(info[i].rsrp, info[i].rsrq, measure_tti, current_earfcn, info[i].pci);
      }
      // Look for other cells not found automatically
    }
  }
}

}
