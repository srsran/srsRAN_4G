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
#include <srslte/srslte.h>
#include "srslte/srslte.h"
#include "srslte/common/log.h"
#include "phy/phch_worker.h"
#include "phy/phch_common.h"
#include "phy/phch_recv.h"

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

namespace srsue {

int radio_recv_wrapper_cs(void *obj, cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *rx_time) {
  phch_recv *h = (phch_recv*) obj;
  srslte::radio_multi *radio_h = h->radio_h;

  if (radio_h->rx_now(data, nsamples, rx_time)) {
    int offset = nsamples - h->current_sflen;
    if (abs(offset) < 10 && offset != 0) {
      h->next_offset = offset;
    } else if (nsamples < 10) {
      h->next_offset = nsamples;
    }
    return nsamples;
  } else {
    return -1;
  }
}

double callback_set_rx_gain(void *h, double gain) {
  srslte::radio_multi *radio_handler = (srslte::radio_multi *) h;
  return radio_handler->set_rx_gain_th(gain);
}


phch_recv::phch_recv() {
  bzero(&cell, sizeof(srslte_cell_t));
  running = false;
}

void phch_recv::  init(srslte::radio_multi *_radio_handler, mac_interface_phy *_mac, rrc_interface_phy *_rrc,
                     prach *_prach_buffer, srslte::thread_pool *_workers_pool,
                     phch_common *_worker_com, srslte::log *_log_h, uint32_t nof_rx_antennas_, uint32_t prio,
                     int sync_cpu_affinity) {
  radio_h = _radio_handler;
  log_h = _log_h;
  mac = _mac;
  rrc = _rrc;
  workers_pool = _workers_pool;
  worker_com = _worker_com;
  prach_buffer = _prach_buffer;
  nof_rx_antennas = nof_rx_antennas_;

  reset();

  for (uint32_t i = 0; i < nof_rx_antennas; i++) {
    sf_buffer[i] = (cf_t *) srslte_vec_malloc(sizeof(cf_t) * 3 * SRSLTE_SF_LEN_PRB(100));
  }


  if (srslte_ue_cellsearch_init_multi(&cs, 5, radio_recv_wrapper_cs, nof_rx_antennas,
                                      this)) {
    Error("SYNC:  Initiating UE cell search\n");
    return;
  }

  srslte_ue_cellsearch_set_nof_valid_frames(&cs, 2);

  // Set options defined in expert section
  set_ue_sync_opts(&cs.ue_sync);

  if (do_agc) {
    srslte_ue_sync_start_agc(&cs.ue_sync, callback_set_rx_gain, last_gain);
  }

  if (srslte_ue_dl_init(&ue_dl_measure, SRSLTE_MAX_PRB, nof_rx_antennas)) {
    Error("SYNC:  Initiating ue_dl_measure\n");
    return;
  }

  if (srslte_ue_mib_init(&ue_mib, SRSLTE_MAX_PRB)) {
    Error("SYNC:  Initiating UE MIB decoder\n");
    return;
  }

  if (srslte_ue_sync_init_multi(&ue_sync, SRSLTE_MAX_PRB, false, radio_recv_wrapper_cs, nof_rx_antennas, this)) {
    Error("SYNC:  Initiating ue_sync\n");
    return;
  }

  if (srslte_ue_mib_sync_init_multi(&ue_mib_sync, radio_recv_wrapper_cs, nof_rx_antennas, this)) {
    Error("SYNC:  Initiating UE MIB synchronization\n");
    return;
  }

  nof_tx_mutex = MUTEX_X_WORKER * workers_pool->get_nof_workers();
  worker_com->set_nof_mutex(nof_tx_mutex);
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
  srslte_ue_dl_free(&ue_dl_measure);
  srslte_ue_mib_free(&ue_mib);
  srslte_ue_mib_sync_free(&ue_mib_sync);
  srslte_ue_cellsearch_free(&cs);
}

void phch_recv::stop() {

  running = false;
  wait_thread_finish();
}

void phch_recv::reset() {
  tx_mutex_cnt = 0;
  running = true;
  phy_state = IDLE;
  time_adv_sec = 0;
  next_offset  = 0;
  cell_is_set = false;
  sync_sfn_cnt = 0;
  srate_mode = SRATE_NONE;
  cell_search_in_progress = false;
  current_earfcn = 0;
  radio_is_resetting = false;
}

void phch_recv::radio_error() {
  log_h->error("SYNC:  Receiving from radio.\n");
  phy_state = IDLE;
  radio_is_resetting=true;

  // Need to find a method to effectively reset radio, reloading the driver does not work
  //radio_h->reset();
  radio_h->stop();

  fprintf(stdout, "Error while receiving samples. Restart srsUE\n");
  exit(-1);

  reset();
  radio_is_resetting=false;
}

bool phch_recv::wait_radio_reset() {
  int cnt=0;
  while(cnt < 20 && radio_is_resetting) {
    sleep(1);
    cnt++;
  }
  return radio_is_resetting;
}

void phch_recv::set_agc_enable(bool enable) {
  do_agc = enable;
}

void phch_recv::set_time_adv_sec(float _time_adv_sec) {
  if (TX_MODE_CONTINUOUS && !radio_h->is_first_of_burst()) {
    int nsamples = ceil(current_srate*_time_adv_sec);
    next_offset = -nsamples;
  } else {
    time_adv_sec = _time_adv_sec;
  }
}

void phch_recv::set_ue_sync_opts(srslte_ue_sync_t *q) {
  if (worker_com->args->cfo_integer_enabled) {
    srslte_ue_sync_cfo_i_detec_en(q, true);
  }

  srslte_ue_sync_set_cfo_tol(q, worker_com->args->cfo_correct_tol_hz);

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
  if (srslte_ue_mib_set_cell(&ue_mib, cell)) {
    Error("SYNC:  Setting cell: initiating ue_mib\n");
    return false;
  }
  if (srslte_ue_sync_set_cell(&ue_sync, cell)) {
    Error("SYNC:  Setting cell: initiating ue_sync");
    return false;
  }

  // Set options defined in expert section
  set_ue_sync_opts(&ue_sync);

  if (srslte_ue_dl_set_cell(&ue_dl_measure, cell)) {
    Error("SYNC:  Setting cell: initiating ue_dl_measure\n");
    return false;
  }

  worker_com->set_cell(cell);

  for (uint32_t i = 0; i < workers_pool->get_nof_workers(); i++) {
    if (!((phch_worker *) workers_pool->get_worker(i))->set_cell(cell)) {
      Error("SYNC:  Setting cell: initiating PHCH worker\n");
      return false;
    }
  }
  if (do_agc) {
    srslte_ue_sync_start_agc(&ue_sync, callback_set_rx_gain, last_gain);
  }
  cell_is_set = true;

  return cell_is_set;
}

int phch_recv::cell_search(int force_N_id_2) {
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];

  srslte_ue_cellsearch_result_t found_cells[3];

  bzero(&cell, sizeof(srslte_cell_t));
  bzero(found_cells, 3 * sizeof(srslte_ue_cellsearch_result_t));

  if (srate_mode != SRATE_FIND) {
    srate_mode = SRATE_FIND;
    radio_h->set_rx_srate(1.92e6);
  }
  start_rx();

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

  last_gain = srslte_agc_get_gain(&cs.ue_sync.agc);

  if (ret < 0) {
    Error("SYNC:  Error decoding MIB: Error searching PSS\n");
    return -1;
  } else if (ret == 0) {
    stop_rx();
    Info("SYNC:  Could not find any cell in this frequency\n");
    return 0;
  }
  // Save result
  cell.id = found_cells[max_peak_cell].cell_id;
  cell.cp = found_cells[max_peak_cell].cp;
  cellsearch_cfo = found_cells[max_peak_cell].cfo;

  printf("\n");
  Info("SYNC:  PSS/SSS detected: PCI=%d, CFO=%.1f KHz, CP=%s\n",
       cell.id, cellsearch_cfo/1000, srslte_cp_string(cell.cp));

  if (srslte_ue_mib_sync_set_cell(&ue_mib_sync, cell.id, cell.cp)) {
    Error("SYNC:  Setting UE MIB cell\n");
    return false;
  }

  // Set options defined in expert section
  set_ue_sync_opts(&ue_mib_sync.ue_sync);

  if (do_agc) {
    srslte_ue_sync_start_agc(&ue_mib_sync.ue_sync, callback_set_rx_gain, last_gain);
  }

  srslte_ue_sync_reset(&ue_mib_sync.ue_sync);
  srslte_ue_sync_set_cfo(&ue_mib_sync.ue_sync, cellsearch_cfo);

  /* Find and decode MIB */
  int sfn_offset;
  ret = srslte_ue_mib_sync_decode(&ue_mib_sync,
                                  40,
                                  bch_payload, &cell.nof_ports, &sfn_offset);
  stop_rx();
  last_gain = srslte_agc_get_gain(&ue_mib_sync.ue_sync.agc);
  cellsearch_cfo = srslte_ue_sync_get_cfo(&ue_mib_sync.ue_sync);

  srslte_ue_sync_reset(&ue_sync);
  srslte_ue_sync_set_cfo(&ue_sync, cellsearch_cfo);

  if (ret == 1) {
    srslte_pbch_mib_unpack(bch_payload, &cell, NULL);

    fprintf(stdout, "Found Cell:  PCI=%d, PRB=%d, Ports=%d, CFO=%.1f KHz\n",
         cell.id, cell.nof_prb, cell.nof_ports, cellsearch_cfo/1000);

    Info("SYNC:  MIB Decoded: PCI=%d, PRB=%d, Ports=%d, CFO=%.1f KHz\n",
         cell.id, cell.nof_prb, cell.nof_ports, cellsearch_cfo/1000);

    return true;
  } else if (ret == 0) {
    Warning("SYNC:  Found PSS but could not decode PBCH\n");
    return 0;
  } else {
    Error("SYNC:  Receiving MIB\n");
    return -1;
  }
}


int phch_recv::cell_sync_sfn(void) {

  int ret = SRSLTE_ERROR;
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];

  srslte_ue_sync_decode_sss_on_track(&ue_sync, true);
  ret = srslte_ue_sync_zerocopy_multi(&ue_sync, sf_buffer);
  if (ret < 0) {
    Error("SYNC:  Error calling ue_sync_get_buffer");
    return -1;
  }

  if (ret == 1) {
    if (srslte_ue_sync_get_sfidx(&ue_sync) == 0) {
      int sfn_offset = 0;
      Info("SYNC:  Trying to decode MIB... SNR=%.1f dB\n",
           10*log10(srslte_chest_dl_get_snr(&ue_mib.chest)));
      int n = srslte_ue_mib_decode(&ue_mib, sf_buffer[0], bch_payload, NULL, &sfn_offset);
      if (n < 0) {
        Error("SYNC:  Error decoding MIB while synchronising SFN");
        return -1;
      } else if (n == SRSLTE_UE_MIB_FOUND) {
        uint32_t sfn;
        srslte_pbch_mib_unpack(bch_payload, &cell, &sfn);

        sfn = (sfn+sfn_offset)%1024;
        tti = sfn * 10;

        srslte_ue_sync_decode_sss_on_track(&ue_sync, true);
        Info("SYNC:  DONE, TTI=%d, sfn_offset=%d\n", tti, sfn_offset);
        srslte_ue_mib_reset(&ue_mib);
        return 1;
      }
    }
  } else {
    Debug("SYNC:  PSS/SSS not found...\n");
  }
  return 0;
}

int phch_recv::cell_meas_rsrp() {

  uint32_t cfi = 0;

  tti = (tti+1) % 10240;
  log_h->step(tti);

  uint32_t sf_idx = tti%10;

  int sync_res = srslte_ue_sync_zerocopy_multi(&ue_sync, sf_buffer);
  if (sync_res == 1) {
    if (srslte_ue_dl_decode_fft_estimate(&ue_dl_measure, sf_buffer, sf_idx, &cfi)) {
      log_h->error("SYNC:  Measuring RSRP: Estimating channel\n");
      return -1;
    }
    float rsrp   = srslte_chest_dl_get_rsrp(&ue_dl_measure.chest);
    float snr    = srslte_chest_dl_get_snr(&ue_dl_measure.chest);
    measure_rsrp = SRSLTE_VEC_CMA(rsrp, measure_rsrp, measure_cnt);
    measure_cnt++;
    log_h->info("SYNC:  Measuring RSRP %d/%d, sf_idx=%d, RSRP=%.1f dBm, SNR=%.1f dB\n",
                measure_cnt, RSRP_MEASURE_NOF_FRAMES, sf_idx, 10 * log10(rsrp / 1000), 10*log10(snr));
    if (measure_cnt >= RSRP_MEASURE_NOF_FRAMES) {
      return 1;
    }
  } else {
    log_h->error("SYNC:  Measuring RSRP: Sync error\n");
    return -1;
  }

  return 0;
}

void phch_recv::resync_sfn(bool is_connected) {

  wait_radio_reset();

  stop_rx();
  start_rx();
  srslte_ue_mib_reset(&ue_mib);
  Info("SYNC:  Starting SFN synchronization\n");
  sync_sfn_cnt = 0;
  phy_state = is_connected?CELL_RESELECT:CELL_SELECT;
}

void phch_recv::set_earfcn(std::vector<uint32_t> earfcn) {
  this->earfcn = earfcn;
}

bool phch_recv::stop_sync() {

  wait_radio_reset();

  if (phy_state == IDLE && is_in_idle) {
    return true;
  } else {
    Info("SYNC:  Going to IDLE\n");
    phy_state = IDLE;
    int cnt = 0;
    while (!is_in_idle && cnt < 100) {
      usleep(10000);
      cnt++;
    }
    return is_in_idle;
  }
}

void phch_recv::reset_sync() {

  wait_radio_reset();

  Warning("SYNC:  Resetting sync, cell_search_in_progress=%s\n", cell_search_in_progress?"yes":"no");
  srslte_ue_sync_reset(&ue_mib_sync.ue_sync);
  srslte_ue_sync_reset(&ue_sync);
  resync_sfn(true);
}

void phch_recv::cell_search_inc()
{
  cur_earfcn_index++;
  if (cur_earfcn_index >= 0) {
    if (cur_earfcn_index >= (int) earfcn.size() - 1) {
      cur_earfcn_index = 0;
      rrc->earfcn_end();
    }
  }
  Info("SYNC:  Cell Search idx %d/%d\n", cur_earfcn_index, earfcn.size());
  if (current_earfcn != earfcn[cur_earfcn_index]) {
    current_earfcn = earfcn[cur_earfcn_index];
    set_frequency();
  }
}

void phch_recv::cell_search_next(bool reset) {
  if (cell_search_in_progress || reset) {
    cell_search_in_progress = false;
    if (!stop_sync()) {
      log_h->warning("SYNC:  Couldn't stop PHY\n");
    }
    if (reset) {
      cur_earfcn_index = -1;
    }
    cell_search_inc();
    phy_state = CELL_SEARCH;
    cell_search_in_progress = true;
  }
}

void phch_recv::cell_search_start() {
  if (earfcn.size() > 0) {
    Info("SYNC:  Starting Cell Search procedure in %d EARFCNs...\n", earfcn.size());
    cell_search_next(true);
  } else {
    Info("SYNC:  Empty EARFCN list. Stopping cell search...\n");
    log_h->console("Empty EARFCN list. Stopping cell search...\n");
  }
}

void phch_recv::cell_search_stop() {
  Info("SYNC:  Stopping Cell Search procedure...\n");
  if (!stop_sync()) {
    Error("SYNC:  Stopping cell search\n");
  }
  cell_search_in_progress = false;
}

bool phch_recv::cell_select(uint32_t earfcn, srslte_cell_t cell) {

  // Check if we are already camping in this cell
  if (earfcn == current_earfcn && this->cell.id == cell.id) {
    log_h->info("Cell Select: Already in cell EARFCN=%d\n", earfcn);
    cell_search_in_progress = false;
    if (srate_mode != SRATE_CAMP) {
      set_sampling_rate();
    }
    if (phy_state < CELL_SELECT) {
      resync_sfn();
    }
    return true;
  } else {

    cell_search_in_progress = false;

    if (!stop_sync()) {
      log_h->warning("Still not in idle\n");
    }

    current_earfcn = earfcn;

    printf("cell select called set frequency\n");

    if (set_frequency()) {
      this->cell = cell;
      log_h->info("Cell Select: Configuring cell...\n");

      if (set_cell()) {
        log_h->info("Cell Select: Synchronizing on cell...\n");

        resync_sfn();

        usleep(500000); // Time offset we set start_rx to start receveing samples
        return true;
      } else {
        log_h->error("Cell Select: Configuring cell in EARFCN=%d, PCI=%d\n", earfcn, cell.id);
      }
    }
    return false;
  }
}

bool phch_recv::set_frequency()
{
  double dl_freq = 1e6*srslte_band_fd(current_earfcn);
  double ul_freq = 1e6*srslte_band_fu(srslte_band_ul_earfcn(current_earfcn));
  if (dl_freq > 0 && ul_freq > 0) {
    log_h->info("SYNC:  Set DL EARFCN=%d, f_dl=%.1f MHz, f_ul=%.1f MHz\n",
                current_earfcn, dl_freq / 1e6, ul_freq / 1e6);

    log_h->console("Searching cell in DL EARFCN=%d, f_dl=%.1f MHz, f_ul=%.1f MHz\n",
                current_earfcn, dl_freq / 1e6, ul_freq / 1e6);

    radio_h->set_rx_freq(dl_freq);
    radio_h->set_tx_freq(ul_freq);
    ul_dl_factor = radio_h->get_tx_freq()/radio_h->get_rx_freq();

    srslte_ue_sync_reset(&ue_sync);

    return true;
  } else {
    log_h->error("SYNC:  Cell Search: Invalid EARFCN=%d\n", current_earfcn);
    return false;
  }
}

void phch_recv::set_sampling_rate()
{
  current_srate = (float) srslte_sampling_freq_hz(cell.nof_prb);
  current_sflen = SRSLTE_SF_LEN_PRB(cell.nof_prb);
  if (current_srate != -1) {
    Info("SYNC:  Setting sampling rate %.2f MHz\n", current_srate/1000000);

    if (30720 % ((int) current_srate / 1000) == 0) {
      radio_h->set_master_clock_rate(30.72e6);
    } else {
      radio_h->set_master_clock_rate(23.04e6);
    }
    srate_mode = SRATE_CAMP;
    radio_h->set_rx_srate(current_srate);
    radio_h->set_tx_srate(current_srate);
  } else {
    Error("Error setting sampling rate for cell with %d PRBs\n", cell.nof_prb);
  }
}

void phch_recv::run_thread() {
  phch_worker *worker = NULL;
  cf_t *buffer[SRSLTE_MAX_PORTS];
  phy_state  = IDLE;
  is_in_idle = true;

  while (running) {
    if (phy_state != IDLE) {
      is_in_idle = false;
      Debug("SYNC:  state=%d\n", phy_state);
    }
    switch (phy_state) {
      case CELL_SEARCH:
        if (cell_search_in_progress) {
          switch(cell_search()) {
          case 1:
            if (!srslte_cell_isvalid(&cell)) {
              Error("SYNC:  Detected invalid cell\n");
              phy_state = IDLE;
              break;
            }
            if (set_cell()) {
              set_sampling_rate();
              resync_sfn();
            }
            break;
          case 0:
            if (cell_search_in_progress) {
              cell_search_inc();
            }
            break;
          default:
            radio_error();
            break;
          }
        }
        break;
      case CELL_RESELECT:
      case CELL_SELECT:

        srslte_ue_sync_decode_sss_on_track(&ue_sync, true);

        switch (cell_sync_sfn()) {
          case 1:
            srslte_ue_sync_set_agc_period(&ue_sync, 20);
            if (!cell_search_in_progress) {
              phy_state = CELL_CAMP;
              log_h->info("Sync OK. Camping on cell PCI=%d...\n", cell.id);
            } else {
              measure_cnt  = 0;
              measure_rsrp = 0;
              phy_state    = CELL_MEASURE;
            }
            break;
          case 0:
            break;
          default:
            radio_error();
            break;
        }
        sync_sfn_cnt++;
        if (sync_sfn_cnt >= SYNC_SFN_TIMEOUT) {
          sync_sfn_cnt = 0;
          log_h->warning("SYNC:  Timeout while synchronizing SFN\n");
          if (phy_state == CELL_SELECT) {
            phy_state = CELL_SEARCH;
          } else {
            phy_state = IDLE;
          }
        }
        break;
      case CELL_MEASURE:
        switch(cell_meas_rsrp()) {
          case 1:
            log_h->info("SYNC:  Measured OK. Camping on cell PCI=%d...\n", cell.id);
            phy_state = CELL_CAMP;
            rrc->cell_found(earfcn[cur_earfcn_index], cell, 10*log10(measure_rsrp/1000));
            break;
          case 0:
            break;
          default:
            radio_error();
            break;
        }
        break;
      case CELL_CAMP:
        tti = (tti+1) % 10240;
        worker = (phch_worker *) workers_pool->wait_worker(tti);
        if (worker) {
          for (uint32_t i = 0; i < nof_rx_antennas; i++) {
            buffer[i] = worker->get_buffer(i);
          }

          switch(srslte_ue_sync_zerocopy_multi(&ue_sync, buffer)) {
            case 1:

              log_h->step(tti);

              Debug("SYNC:  Worker %d synchronized\n", worker->get_id());

              metrics.sfo = srslte_ue_sync_get_sfo(&ue_sync);
              metrics.cfo = srslte_ue_sync_get_cfo(&ue_sync);
              worker->set_cfo(ul_dl_factor * metrics.cfo / 15000);
              worker_com->set_sync_metrics(metrics);

              worker->set_sample_offset(srslte_ue_sync_get_sfo(&ue_sync)/1000);

              /* Compute TX time: Any transmission happens in TTI4 thus advance 4 ms the reception time */
              srslte_timestamp_t rx_time, tx_time, tx_time_prach;
              srslte_ue_sync_get_last_timestamp(&ue_sync, &rx_time);
              srslte_timestamp_copy(&tx_time, &rx_time);
              srslte_timestamp_add(&tx_time, 0, 4e-3 - time_adv_sec);
              worker->set_tx_time(tx_time, next_offset);
              next_offset = 0;

              Debug("SYNC:  Setting TTI=%d, tx_mutex=%d to worker %d\n", tti, tx_mutex_cnt, worker->get_id());
              worker->set_tti(tti, tx_mutex_cnt);
              tx_mutex_cnt = (tx_mutex_cnt+1) % nof_tx_mutex;

              // Check if we need to TX a PRACH
              if (prach_buffer->is_ready_to_send(tti)) {
                srslte_timestamp_copy(&tx_time_prach, &rx_time);
                srslte_timestamp_add(&tx_time_prach, 0, prach::tx_advance_sf * 1e-3);
                prach_buffer->send(radio_h, ul_dl_factor * metrics.cfo / 15000, worker_com->pathloss, tx_time_prach);
                radio_h->tx_end();
                worker_com->p0_preamble = prach_buffer->get_p0_preamble();
                worker_com->cur_radio_power = SRSLTE_MIN(SRSLTE_PC_MAX, worker_com->pathloss+worker_com->p0_preamble);
              }
              workers_pool->start_worker(worker);
              break;
            case 0:
              log_h->error("SYNC:  Sync error. Sending out-of-sync to RRC\n");
              // Notify RRC of out-of-sync frame
              rrc->out_of_sync();
              worker->release();
              worker_com->reset_ul();
              mac->tti_clock(tti);
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
        if (!is_in_idle) {
          stop_rx();
        }
        is_in_idle = true;
        usleep(1000);
        // Keep running MAC timer from system clock
        tti = (tti+1) % 10240;
        mac->tti_clock(tti);
        break;
    }
  }
}

void phch_recv::stop_rx() {
  if (radio_is_rx) {
    Info("SYNC:  Stopping RX streaming\n");
    radio_h->stop_rx();
  }
  radio_is_rx = false;
}

void phch_recv::start_rx() {
  if (!radio_is_rx) {
    Info("SYNC:  Starting RX streaming\n");
    radio_h->start_rx();
  }
  radio_is_rx = true;
}

uint32_t phch_recv::get_current_tti() {
  return tti;
}

bool phch_recv::status_is_sync() {
  return phy_state == CELL_CAMP;
}

void phch_recv::get_current_cell(srslte_cell_t *cell_) {
  if (cell_) {
    memcpy(cell_, &cell, sizeof(srslte_cell_t));
  }
}
}
