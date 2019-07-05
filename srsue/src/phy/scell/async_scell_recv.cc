/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srsue/hdr/phy/scell/async_scell_recv.h"
#include "srsue/hdr/phy/phy_common.h"
#include <srslte/interfaces/ue_interfaces.h>
#include <srslte/phy/ch_estimation/chest_dl.h>
#include <srslte/phy/common/phy_common.h>
#include <srslte/phy/ue/ue_sync.h>
#include <srslte/srslte.h>

#define LOG_PREABLE "[scell_recv] "

#define LOG_ALL_CONSOLE 0

#if LOG_ALL_CONSOLE
#define Error(fmt, ...) log_h->console(LOG_PREABLE fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->console(LOG_PREABLE fmt, ##__VA_ARGS__)
#define Info(fmt, ...) log_h->console(LOG_PREABLE fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) log_h->console(LOG_PREABLE fmt, ##__VA_ARGS__)
#else
#define Error(fmt, ...) log_h->error(LOG_PREABLE fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(LOG_PREABLE fmt, ##__VA_ARGS__)
#define Info(fmt, ...) log_h->info(LOG_PREABLE fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) log_h->debug(LOG_PREABLE fmt, ##__VA_ARGS__)
#endif

namespace srsue {
namespace scell {

async_scell_recv::async_scell_recv() : thread("ASYNC_SCELL_RECV")
{
  initiated        = false;
  buffer_write_idx = 0;
  buffer_read_idx  = 0;
  dl_freq          = -1;
  ul_freq          = -1;
  bzero(&cell, sizeof(srslte_cell_t));
  bzero(sf_buffer, sizeof(sf_buffer));
  running           = false;
  radio_idx         = 1;
  current_sflen     = 0;
  next_radio_offset = 0;
}

async_scell_recv::~async_scell_recv()
{
  if (initiated) {
    srslte_ue_sync_free(&ue_sync);
  }

  for (auto& b : sf_buffer) {
    if (b) {
      free(b);
    }
  }
}

static int radio_recv_callback(void* obj, cf_t* data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t* rx_time)
{
  return ((async_scell_recv*)obj)->radio_recv_fnc(data, nsamples, rx_time);
}

static double callback_set_rx_gain(void* h, double gain)
{
  return ((async_scell_recv*)h)->set_rx_gain(gain);
}

void async_scell_recv::init(srslte::radio_interface_phy* _radio_handler, phy_common* _worker_com, srslte::log* _log_h)
{
  // Get handlers
  radio_h    = _radio_handler;
  worker_com = _worker_com;
  log_h      = _log_h;

  // Calculate number of RF channels
  uint32_t nof_rf_channels = worker_com->args->nof_rf_channels * worker_com->args->nof_rx_ant;

  // Initialise buffers
  for (uint32_t s = 0; s < ASYNC_NOF_BUFFERS; s++) {
    buffers[s].init(nof_rf_channels);
  }

  for (uint32_t i = 0; i < nof_rf_channels; i++) {
    sf_buffer[i] = (cf_t*)srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_MAX * 5);
    if (!sf_buffer[i]) {
      fprintf(stderr, "Error allocating buffer\n");
      return;
    }
  }

  if (srslte_ue_sync_init_multi(&ue_sync, SRSLTE_MAX_PRB, false, radio_recv_callback, nof_rf_channels, this)) {
    fprintf(stderr, "SYNC:  Initiating ue_sync\n");
    return;
  }

  if (srslte_ue_mib_init(&ue_mib, sf_buffer, SRSLTE_MAX_PRB)) {
    fprintf(stderr, "Error initaiting UE MIB decoder\n");
    return;
  }

  if (pthread_cond_init(&cvar_buffer, nullptr)) {
    fprintf(stderr, "Initiating condition var\n");
    return;
  }

  reset();
  running   = false;
  initiated = true;
}

void async_scell_recv::stop()
{
  running = false;
  wait_thread_finish();

  pthread_mutex_destroy(&mutex_buffer);
  pthread_mutex_destroy(&mutex_uesync);
  pthread_cond_destroy(&cvar_buffer);
}

void async_scell_recv::in_sync()
{
  in_sync_cnt++;
  // Send RRC in-sync signal after 100 ms consecutive subframes
  if (in_sync_cnt == NOF_IN_SYNC_SF) {
    in_sync_cnt     = 0;
    out_of_sync_cnt = 0;
  }
}
void async_scell_recv::out_of_sync()
{
  // Send RRC out-of-sync signal after 200 ms consecutive subframes
  Info("Out-of-sync %d/%d\n", out_of_sync_cnt, NOF_OUT_OF_SYNC_SF);
  out_of_sync_cnt++;
  if (out_of_sync_cnt == NOF_OUT_OF_SYNC_SF) {
    Info("Sending to RRC\n");
    out_of_sync_cnt = 0;
    in_sync_cnt     = 0;
  }
}

void async_scell_recv::set_cfo(float cfo)
{
  srslte_ue_sync_set_cfo_ref(&ue_sync, cfo);
}

float async_scell_recv::get_tx_cfo()
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

void async_scell_recv::set_agc_enable(bool enable)
{
  do_agc = enable;
  if (do_agc) {
    if (radio_h) {
      srslte_rf_info_t* rf_info = radio_h->get_info(radio_idx);
      srslte_ue_sync_start_agc(
          &ue_sync, callback_set_rx_gain, rf_info->min_rx_gain, rf_info->max_rx_gain, radio_h->get_rx_gain(radio_idx));
    } else {
      fprintf(stderr, "Error setting Secondary cell AGC: PHY not initiated\n");
    }
  } else {
    fprintf(stderr, "Error stopping AGC: not implemented\n");
  }
}

double async_scell_recv::set_rx_gain(double gain)
{
  return radio_h->set_rx_gain_th((float)gain);
}

int async_scell_recv::radio_recv_fnc(cf_t* data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t* rx_time)
{
  int ret = 0;

  if (running) {
    if (radio_h->rx_now(radio_idx, data, nsamples, rx_time)) {
      int offset = nsamples - current_sflen;
      if (abs(offset) < 10 && offset != 0) {
        next_radio_offset += offset;
      } else if (nsamples < 10) {
        next_radio_offset += nsamples;
      }

      log_h->debug("SYNC:  received %d samples from radio\n", nsamples);
      ret = nsamples;
    } else {
      ret = SRSLTE_ERROR;
    }
  }

  return ret;
}

void async_scell_recv::reset()
{
  in_sync_cnt     = 0;
  out_of_sync_cnt = 0;

  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
    current_earfcn[i] = UINT32_MAX;
  }
}

void async_scell_recv::radio_error()
{
  log_h->error("SYNC:  Receiving from radio.\n");
  // Need to find a method to effectively reset radio, reloading the driver does not work
  radio_h->reset();
}

void async_scell_recv::set_ue_sync_opts(srslte_ue_sync_t* q, float cfo)
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

  q->strack.pss.chest_on_filter = worker_com->args->sic_pss_enabled;

  // Disable CP based CFO estimation during find
  if (cfo != 0) {
    q->cfo_current_value       = cfo / 15000;
    q->cfo_is_copied           = true;
    q->cfo_correct_enable_find = true;
    srslte_sync_set_cfo_cp_enable(&q->sfind, false, 0);
  }

  // Set SFO ema and correct period
  srslte_ue_sync_set_sfo_correct_period(q, worker_com->args->sfo_correct_period);
  srslte_ue_sync_set_sfo_ema(q, worker_com->args->sfo_ema);

  srslte_sync_set_sss_algorithm(&q->strack, SSS_FULL);
  srslte_sync_set_sss_algorithm(&q->sfind, SSS_FULL);
}

bool async_scell_recv::set_scell_cell(uint32_t carrier_idx, srslte_cell_t* _cell, uint32_t dl_earfcn)
{
  bool ret           = true;
  bool reset_ue_sync = false;

  Info("Set cell:{nof_prb=%d; cp=%s; id=%d} dl_earfcn=%d\n",
       _cell->nof_prb,
       srslte_cp_string(_cell->cp),
       _cell->id,
       dl_earfcn);

  // Lock mutex
  pthread_mutex_lock(&mutex_uesync);

  // Get transceiver mapping
  carrier_map_t* m           = &worker_com->args->carrier_map[carrier_idx];
  uint32_t       channel_idx = m->channel_idx;
  radio_idx                  = m->radio_idx;

  // Set radio frequency if frequency changed
  if (current_earfcn[channel_idx] != dl_earfcn) {
    dl_freq = srslte_band_fd(dl_earfcn) * 1e6f;
    ul_freq = srslte_band_fu(srslte_band_ul_earfcn(dl_earfcn)) * 1e6f;
    for (uint32_t p = 0; p < worker_com->args->nof_rx_ant; p++) {
      radio_h->set_rx_freq(m->radio_idx, m->channel_idx + p, dl_freq);
      radio_h->set_tx_freq(m->radio_idx, m->channel_idx + p, ul_freq);
    }
    Info("Setting DL: %.1f MHz; UL %.1fMHz; Radio/Chan: %d/%d\n", dl_freq / 1e6, ul_freq / 1e6, radio_idx, channel_idx);
    ul_dl_factor                = ul_freq / dl_freq;
    current_earfcn[channel_idx] = dl_earfcn;
    reset_ue_sync               = true;
  }

  // Detect change in cell configuration
  if (memcmp(&cell, _cell, sizeof(srslte_cell_t)) != 0) {
    // Set sampling rate, if number of PRB changed
    if (cell.nof_prb != _cell->nof_prb) {
      double srate = srslte_sampling_freq_hz(_cell->nof_prb);
      radio_h->set_rx_srate(radio_idx, srate);
      radio_h->set_tx_srate(radio_idx, srate);
      current_sflen = (uint32_t)SRSLTE_SF_LEN_PRB(_cell->nof_prb);

      Info("Setting SRate to %.2f MHz\n", srate / 1e6);
    }

    // Copy cell
    cell          = *_cell;
    reset_ue_sync = true;

    // Set cell in ue sync
    if (srslte_ue_sync_set_cell(&ue_sync, cell)) {
      Error("SYNC:  Setting cell: initiating ue_sync\n");
      ret = false;
    }

    // Set cell in MIB decoder
    if (srslte_ue_mib_set_cell(&ue_mib, cell)) {
      fprintf(stderr, "Error setting cell in UE MIB decoder\n");
      ret = false;
    }

    srslte_ue_mib_reset(&ue_mib);
  }

  // Reset ue_sync and set CFO/gain from search procedure
  if (reset_ue_sync) {
    srslte_ue_sync_reset(&ue_sync);
  }

  // Reset thread state
  state = DECODE_MIB;

  // If not running start!
  if (!running) {
    // Start main thread
    start(1);
    running = true;
  }

  pthread_mutex_unlock(&mutex_uesync);

  return ret;
}

void async_scell_recv::state_decode_mib()
{
  int     sfn_offset = 0;
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];

  uint32_t sfidx = srslte_ue_sync_get_sfidx(&ue_sync);

  if (sfidx == 0) {
    // Run only for sub-frame index 0
    int n = srslte_ue_mib_decode(&ue_mib, bch_payload, nullptr, &sfn_offset);

    if (n < SRSLTE_SUCCESS) {
      // Error decoding MIB, log error
      Error("Error decoding UE MIB (%d)\n", n);
    } else if (n == SRSLTE_UE_MIB_FOUND) {
      // MIB Found
      uint32_t sfn = 0;
      srslte_pbch_mib_unpack(bch_payload, &cell, &sfn);

      Info("SCell MIB synchronised (SNR=%.2fdB)\n", ue_mib.chest_res.snr_db);

      // Set sub-frame index
      tti = ((sfn + sfn_offset) % 1024) * 10;

      // Change state, reset ring buffer and go to Synchronized but idle
      buffer_write_idx = 0;
      buffer_read_idx  = 0;
      state            = SYNCH_IDLE;
    } else {
      // MIB Not found
      // Do nothing. Keep going.
    }
  } else {
    // Do nothing. Keep going.
  }
}

void async_scell_recv::state_write_buffer()
{
  if (tti % SRSLTE_NOF_SF_X_FRAME != srslte_ue_sync_get_sfidx(&ue_sync) || ue_sync.state != SF_TRACK) {
    // Real-time failure, go to decode MIB
    Info("Detected Real-Time failure; Going to search MIB (from WRITE)\n");
    state = DECODE_MIB;
  } else {
    // Normal operation, try to write buffer
    phch_scell_recv_buffer* buffer  = &buffers[buffer_write_idx];
    srslte_timestamp_t      rx_time = {};

    // Lock mutex
    pthread_mutex_lock(&mutex_buffer);

    // Copy last timestamp
    srslte_ue_sync_get_last_timestamp(&ue_sync, &rx_time);

    // Extract essential information
    buffer->set_sf(tti, &rx_time, next_radio_offset);
    next_radio_offset = 0;

    // Increment write index
    buffer_write_idx = (buffer_write_idx + 1) % ASYNC_NOF_BUFFERS;

    // Detect overflow
    if (buffer_write_idx == buffer_read_idx) {
      // Reset buffer and goto synchronized IDLE
      Info("Detected overflow; reseting ring buffer and going to IDLE...\n");
      buffer_write_idx = 0;
      buffer_read_idx  = 0;
      state            = SYNCH_IDLE;
    }

    // Unlock mutex and inform that data was received
    pthread_cond_broadcast(&cvar_buffer);
    pthread_mutex_unlock(&mutex_buffer);
  }
}

void async_scell_recv::state_synch_idle()
{
  if (tti % SRSLTE_NOF_SF_X_FRAME != srslte_ue_sync_get_sfidx(&ue_sync)) {
    // Real-time failure, go to decode MIB
    Debug("Detected Real-Time failure; Going to search MIB (from IDLE)\n");
    state = DECODE_MIB;
  } else {
    // Do nothing
  }
}

void async_scell_recv::run_thread()
{
  Info("Starting asynchronous scell reception...\n");
  while (running) {
    phch_scell_recv_buffer* buffer = &buffers[buffer_write_idx];

    // Lock ue_sync
    pthread_mutex_lock(&mutex_uesync);

    // Get RF base-band
    int ret = srslte_ue_sync_zerocopy(&ue_sync, (state == DECODE_MIB) ? sf_buffer : buffer->get_buffer_ptr());
    if (ret < 0) {
      fprintf(stderr, "Error calling srslte_ue_sync_work()\n");
    }

    // Unlock ue_sync
    pthread_mutex_unlock(&mutex_uesync);

    if (ret == 1) {
      // Synchronized
      switch (state) {
        case DECODE_MIB:
          state_decode_mib();
          break;
        case WRITE_BUFFER:
          state_write_buffer();
          break;
        case SYNCH_IDLE:
          state_synch_idle();
          break;
      }

      // Load metrics
      sync_metrics_t metrics = {};
      metrics.sfo            = srslte_ue_sync_get_sfo(&ue_sync);
      metrics.cfo            = srslte_ue_sync_get_cfo(&ue_sync);
      metrics.ta_us          = NAN;
      for (uint32_t i = 0; i < worker_com->args->nof_carriers; i++) {
        if (worker_com->args->carrier_map[i].radio_idx == radio_idx) {
          worker_com->set_sync_metrics(i, metrics);
        }
      }

      // Increment tti
      tti = (tti + 1) % 10240;
    } else if (ret == 0) {
      // Error in synchronization
      // Warning("SYNC:  Out-of-sync detected in PSS/SSS\n");
      // out_of_sync();
    }

    if (ret < 0) {
      // Radio error
      radio_error();
    }
  }
}

bool async_scell_recv::tti_align(uint32_t tti)
{
  bool ret = false;

  if (state == SYNCH_IDLE) {
    // Enable Writing in buffer
    Debug("Start writing in buffer\n");
    state = WRITE_BUFFER;
  } else if (state == DECODE_MIB) {
    // Debug("SCell not ready for reading\n");
    return false;
  }

  pthread_mutex_lock(&mutex_buffer);

  // Stage 1: Flush buffers if the tti is not available
  // While data is available and no tti match, discard
  while ((buffer_write_idx != buffer_read_idx) && (buffers[buffer_read_idx].get_tti() != tti)) {
    // Discard buffer
    Error("Expected TTI %d. Discarding tti %d.\n", tti, buffers[buffer_read_idx].get_tti());
    buffer_read_idx = (buffer_read_idx + 1) % ASYNC_NOF_BUFFERS;
  }

  if ((buffers[buffer_read_idx].get_tti() == tti)) {
    // tti match
    ret = true;
  }

  // Stage 2: If the tti is not found and the latest tti was -1; wait
  // Get time and set timeout time
  if (!ret) {
    bool timedout = false;

    while (!ret && !timedout && buffer_write_idx == buffer_read_idx && running) {
      struct timespec timeToWait = {};
      struct timeval  now        = {};

      gettimeofday(&now, nullptr);
      timeToWait.tv_sec  = now.tv_sec;
      timeToWait.tv_nsec = (now.tv_usec + 1000UL) * 1000UL;

      int rt = pthread_cond_timedwait(&cvar_buffer, &mutex_buffer, &timeToWait);
      switch (rt) {
        case ETIMEDOUT:
        case EPERM:
          // Consider all errors timed out, exit loop
          timedout = true;
          Error("Expected TTI %04d. timeout (%d).\n", tti, rt);
          tti_align_timeout_counter++;
          if (tti_align_timeout_counter > max_tti_align_timeout_counter) {
            Error("Maximum number of timeouts reached (%d). Going back to decode MIB.\n",
                  max_tti_align_timeout_counter);
            state = DECODE_MIB;
          }
          break;
        default:
          if ((buffers[buffer_read_idx].get_tti() == tti)) {
            // tti match
            ret = true;
          }
          break;
      }
    }
  }

  pthread_mutex_unlock(&mutex_buffer);

  return ret;
}

void async_scell_recv::read_sf(cf_t** dst, srslte_timestamp_t* timestamp, int* next_offset)
{
  pthread_mutex_lock(&mutex_buffer);

  // Block until data is filled
  while (buffer_write_idx == buffer_read_idx && running) {
    pthread_cond_wait(&cvar_buffer, &mutex_buffer);
  }

  // Exit condition detected
  if (!running) {
    pthread_mutex_unlock(&mutex_buffer);
    return;
  }

  // Get reading buffer
  phch_scell_recv_buffer* buffer = &buffers[buffer_read_idx];

  if (dst) {
    // Get data pointer
    cf_t** buff = buffer->get_buffer_ptr();

    uint32_t nof_rf_channels = worker_com->args->nof_rf_channels * worker_com->args->nof_rx_ant;

    // Copy data
    for (uint32_t i = 0; i < nof_rf_channels; i++) {
      if (dst[i]) {
        // Check pointer is allocated
        memcpy(dst[i], buff[i], sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
      }
    }
  }

  buffer->get_timestamp(timestamp);
  buffer->get_next_offset(next_offset);

  // Increment read index
  buffer_read_idx = (buffer_read_idx + 1) % ASYNC_NOF_BUFFERS;

  pthread_mutex_unlock(&mutex_buffer);
}

} // namespace scell
} // namespace srsue
