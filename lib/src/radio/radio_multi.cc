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

#include <unistd.h>
#include "srslte/srslte.h"
#include "srslte/radio/radio_multi.h"

using namespace std;

#define RADIO_INFO(_fmt, ...) INFO("[radio_multi] " _fmt, ##__VA_ARGS__)
#define RADIO_DEBUG(_fmt, ...) DEBUG("[radio_multi] " _fmt, ##__VA_ARGS__)

namespace srslte {

radio_multi::radio_multi() {
  log_h     = NULL;
  initiated = false;
  radios.clear();
  rx_srate = 1.92e6;
  bzero(ts_rx, sizeof(ts_rx));
  bzero(temp_buffers, sizeof(temp_buffers));
  bzero(&mutex, sizeof(mutex));
  locked = false;
  nof_ports = 1;
}

bool radio_multi::init(
    log_filter* _log_h, char* args[SRSLTE_MAX_RADIOS], char* devname, uint32_t nof_radios, uint32_t nof_rf_ports)
{
  bool ret = true;

  log_h = _log_h;

  if (nof_radios == 0 || nof_radios > SRSLTE_MAX_RADIOS) {
    ERROR("Wrong number of radios (%d)\n", nof_radios);
    ret = false;
  }

  /* Create and initiate radios */
  for (size_t i = 0; i < nof_radios && ret; i++) {
    radio *r = new radio();
    if (!r->init(_log_h, args ? args[i] : NULL, devname, nof_rf_ports, nof_radios > 1)) {
      ERROR("Error initiating radio index %ld\n", i);
      ret = false;
    }
    radios.push_back(r);
  }

  for (size_t r = 0; r < nof_radios && ret; r++) {
    for (size_t i = 0; i < nof_rf_ports && ret; i++) {
      temp_buffers[r][i] = (cf_t *) srslte_vec_malloc(sizeof(cf_t) * TEMP_BUFFER_SIZE);
      if (!temp_buffers[r][i]) {
        ret = false;
      }
    }
  }

  pthread_mutex_init(&mutex, NULL);

  RADIO_INFO("Initiated\n");

  initiated = ret;
  nof_ports = nof_rf_ports;

  return ret;
}

void radio_multi::synch_issue() {
  RADIO_INFO("Issuing PPS synchronization\n");

  /* If more than one radio, synchronize radios */
  if (radios.size() > 1) {
    bool aligned;
    bool ret = true; /* TODO: currently ignored */

    do {
      /* Issue PPS synchronising and wait */
      for (size_t i = 0; i < radios.size(); i++) {
        radios[i]->synch_issue();
      }
      synch_wait();

      /* Issue some Rx for checking initial alignment and wait */
      for (size_t i = 0; i < radios.size(); i++) {
        /* Receive each radio */
        ret &= radios[i]->rx_now(temp_buffers[i], TEMP_BUFFER_SIZE, &ts_rx[i]);
      }
      synch_wait();

      /* Align radios */
      aligned = align_radio_ts();

      if (!aligned) {
        RADIO_INFO("Issuing PPS synchronization\n");
      }

      /* If it was not possible to align, keep trying */
    } while (!aligned);
  }
}

bool radio_multi::synch_wait() {

  bool ret = true;

  if (radios.size() > 1) {
    /* Wait for PPS synchronization */
    for (size_t i = 0; i < radios.size(); i++) {
      RADIO_DEBUG("Waiting for radio %ld to finish PPS synchronization\n", i);
      int err = radios[i]->synch_wait();
      if (err) {
        ERROR("Error receiving from radio %ld, it does not support PPS synchronization\n", i);
        ret &= false;
      }
    }
  }

  return ret;
}

void radio_multi::stop() {
  RADIO_INFO("Stopping...\n");
  pthread_mutex_destroy(&mutex);

  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->stop();
    delete radios[i];
  }

  for (size_t r = 0; r < SRSLTE_MAX_RADIOS; r++) {
    for (size_t i = 0; i < SRSLTE_MAX_PORTS; i++) {
      if (temp_buffers[r][i]) {
        free(temp_buffers[r][i]);
      }
    }
  }
}

void radio_multi::reset() {
  RADIO_INFO("Reseting...\n");
  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->reset();
  }
}

bool radio_multi::start_agc(bool tx_gain_same_rx) {
  bool ret = true;

  RADIO_INFO("Start AGC\n");

  for (size_t i = 0; i < radios.size(); i++) {
    ret &= radios[i]->start_agc(tx_gain_same_rx);
  }
  return ret;
}

void radio_multi::set_burst_preamble(double preamble_us) {
  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->set_burst_preamble(preamble_us);
  }
}

void radio_multi::set_tx_adv(int nsamples) {
  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->set_tx_adv(nsamples);
  }
}

void radio_multi::set_tx_adv_neg(bool tx_adv_is_neg) {
  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->set_tx_adv_neg(tx_adv_is_neg);
  }
}

bool radio_multi::is_continuous_tx() {
  bool ret = false;
  for (size_t i = 0; i < radios.size(); i++) {
    ret |= radios[i]->is_continuous_tx();
  }
  return ret;
}

void radio_multi::set_continuous_tx(bool enable) {
  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->set_continuous_tx(enable);
  }
}

bool radio_multi::tx_single(cf_t* buffer, uint32_t nof_samples, srslte_timestamp_t tx_time)
{
  bool ret = radios[0]->tx_single(buffer, nof_samples, tx_time);

  RADIO_DEBUG("Tx (Single) %d samples at %.9f\n", nof_samples, srslte_timestamp_real(&tx_time));

  return ret;
}

bool radio_multi::tx(cf_t *buffer[SRSLTE_MAX_RADIOS][SRSLTE_MAX_PORTS], uint32_t nof_samples, srslte_timestamp_t tx_time) {
  bool ret = true;
  RADIO_DEBUG("Tx %d samples at %.9f\n", nof_samples, srslte_timestamp_real(&tx_time));
  for (size_t i = 0; i < radios.size(); i++) {
    ret &= radios[i]->tx(buffer[i], nof_samples, tx_time);
  }

  return ret;
}

void radio_multi::tx_end() {
  RADIO_INFO("Tx end\n");

  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->tx_end();
  }
}

bool radio_multi::rx_now(cf_t *buffer[SRSLTE_MAX_RADIOS][SRSLTE_MAX_PORTS],
                         uint32_t nof_samples,
                         srslte_timestamp_t *rxd_time) {
  bool ret = true;

  if (locked) {
    /* If locked dont even try to get samples, but set input buffers to zeros */
    for (size_t i = 0; i < radios.size(); i++) {
      for (size_t p = 0; p < nof_ports; p++) {
        if (buffer[i][p]) {
          bzero(buffer[i][p], sizeof(cf_t) * nof_samples);
        }
      }
    }
    if (rxd_time) {
      rxd_time->full_secs = 0;
      rxd_time->frac_secs = 0;
    }
  } else if (initiated) {

    pthread_mutex_lock(&mutex);

    for (size_t i = 0; i < radios.size(); i++) {
      /* Check buffer pointer integrity */
      for (size_t p = 0; p < SRSLTE_MAX_PORTS; p++) {
        if (!buffer[i][p]) {
          buffer[i][p] = temp_buffers[i][p];
        }
      }

      /* Receive each radio */
      ret &= radios[i]->rx_now(buffer[i], nof_samples, &ts_rx[i]);
    }

    synch_wait();

    srslte_timestamp_copy(rxd_time, &ts_rx[0]);

    /* If more than 1 radio, align all radio timestamps. If alignment fails, issue PPS synchronization */
    if (radios.size() > 1) {
      if (!align_radio_ts()) {
        synch_issue();
      }
    }

    pthread_mutex_unlock(&mutex);

  } else {
    ret = false;
  }


  return ret;
}

void radio_multi::set_tx_gain(float gain, uint32_t radio_idx) {
  if (radio_idx < radios.size()) {
    RADIO_INFO("Set radio %d Tx gain to %.2f\n", radio_idx, gain);
    radios[radio_idx]->set_tx_gain(gain);
  } else if (radio_idx == UINT32_MAX) {
    RADIO_INFO("Set radio all %d Tx gain to %.2f\n", radio_idx, gain);
    for (size_t i = 0; i < radios.size(); i++) {
      radios[i]->set_tx_gain(gain);
    }
  }
}

void radio_multi::set_rx_gain(float gain, uint32_t radio_idx) {
  if (radio_idx < radios.size()) {
    RADIO_INFO("Set radio %d Rx gain to %.2f\n", radio_idx, gain);
    radios[radio_idx]->set_rx_gain(gain);
  } else if (radio_idx == UINT32_MAX) {
    RADIO_INFO("Set radio all %d Tx gain to %.2f\n", radio_idx, gain);
    for (size_t i = 0; i < radios.size(); i++) {
      radios[i]->set_rx_gain(gain);
    }
  }
}

void radio_multi::set_tx_rx_gain_offset(float offset) {
  RADIO_INFO("Set Tx/Rx gain offset to %.2f\n", offset);

  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->set_tx_rx_gain_offset(offset);
  }
}

double radio_multi::set_rx_gain_th(float gain) {
  RADIO_INFO("Set Rx gain Threshold to %.2f\n", gain);

  double ret = 0.0f;
  for (size_t i = 0; i < radios.size(); i++) {
    ret = radios[i]->set_rx_gain_th(gain);
  }
  return ret;
}

void radio_multi::set_freq_offset(double freq) {
  RADIO_INFO("Set frequency offset to %.3f Hz\n", freq);

  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->set_freq_offset(freq);
  }
}

void radio_multi::set_tx_freq(double freq, uint32_t radio_idx) {
  if (radio_idx < radios.size()) {
    RADIO_INFO("Set radio %d Tx frequency to %.3f MHz\n", radio_idx, freq * 1e-6);
    radios[radio_idx]->set_tx_freq(0, freq);
  } else if (radio_idx == UINT32_MAX) {
    RADIO_INFO("Set all radio Tx frequency to %.3f MHz\n", freq * 1e-6);
    for (size_t i = 0; i < radios.size(); i++) {
      radios[i]->set_tx_freq(0, freq);
    }
  }
}

void radio_multi::set_rx_freq(double freq, uint32_t radio_idx) {
  if (radio_idx < radios.size()) {
    RADIO_INFO("Set radio %d Rx frequency to %.3f MHz\n", radio_idx, freq * 1e-6);
    radios[radio_idx]->set_rx_freq(0, freq);
  } else if (radio_idx == UINT32_MAX) {
    RADIO_INFO("Set all radio Rx frequency to %.3f MHz\n", freq * 1e-6);
    for (size_t i = 0; i < radios.size(); i++) {
      radios[i]->set_rx_freq(0, freq);
    }
  }
}

double radio_multi::get_freq_offset() {
  return radios[0]->get_freq_offset();
}

double radio_multi::get_tx_freq(uint32_t radio_idx) {
  return radios[radio_idx]->get_tx_freq();
}

double radio_multi::get_rx_freq(uint32_t radio_idx) {
  return radios[radio_idx]->get_rx_freq();
}

void radio_multi::set_master_clock_rate(double rate) {
  locked = true;

  RADIO_INFO("Set master clock rate to %.3f MHz\n", rate * 1e-6);

  pthread_mutex_lock(&mutex);

  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->set_master_clock_rate(rate);
  }

  /* Issue synchronization */
  synch_issue();

  pthread_mutex_unlock(&mutex);
  locked = false;
}

void radio_multi::set_tx_srate(double srate) {
  RADIO_INFO("Set Tx srate to %.3f MHz\n", srate * 1e-6);

  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->set_tx_srate(srate);
  }

}

void radio_multi::set_rx_srate(double srate) {
  RADIO_INFO("Set Rx srate to %.3f MHz\n", srate * 1e-6);

  rx_srate = srate;
  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->set_rx_srate(srate);
  }
}

float radio_multi::get_tx_gain(uint32_t radio_idx) {
  float ret = NAN;

  if (radio_idx < radios.size()) {
    ret = radios[radio_idx]->get_tx_gain();
  }

  return ret;
}

float radio_multi::get_rx_gain(uint32_t radio_idx) {
  float ret = NAN;

  if (radio_idx < radios.size()) {
    ret = radios[radio_idx]->get_rx_gain();
  }

  return ret;
}

float radio_multi::get_max_tx_power() {
  return radios[0]->get_max_tx_power();
}

float radio_multi::set_tx_power(float power) {
  float ret = 0.0f;
  for (size_t i = 0; i < radios.size(); i++) {
    ret += radios[i]->set_tx_power(power);
  }
  return ret / radios.size();
}

float radio_multi::get_rssi() {
  return radios[0]->get_rssi();
}

bool radio_multi::has_rssi() {
  return radios[0]->has_rssi();
}

void radio_multi::set_tti(uint32_t tti) {
  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->set_tti(tti);
  }
}

bool radio_multi::is_init() {
  bool ret = initiated;
  for (size_t i = 0; i < radios.size() && ret; i++) {
    ret &= radios[i]->is_init();
  }

  return ret;
}

void radio_multi::register_error_handler(srslte_rf_error_handler_t h) {
  for (size_t i = 0; i < radios.size(); i++) {
    radios[i]->register_error_handler(h);
  }
}

/*
 * This method aligns radios from their time stamp
 * It returns true if the radios have been successfully aligned. Otherwise it returns false.
 */
bool radio_multi::align_radio_ts() {
  bool aligned = true;

  /* Find newest timestamp */
  size_t newer_idx = 0;
  srslte_timestamp_t ts_newer = {}, ts_diff = {};
  size_t nof_samples[SRSLTE_MAX_RADIOS] = {};
  size_t count_done = 0;

  /* Find highest timestamp */
  for (size_t i = 1; i < radios.size(); i++) {

    if (srslte_timestamp_compare(&ts_rx[i], &ts_rx[newer_idx]) == 1) {
      newer_idx = i;
    }

    srslte_timestamp_copy(&ts_newer, &ts_rx[newer_idx]);
  }

  /* Calculate number of samples that each radio needs to advance */
  for (size_t i = 0; i < radios.size() && aligned; i++) {

    /* Calculate delayed time */
    srslte_timestamp_copy(&ts_diff, &ts_newer);
    srslte_timestamp_sub(&ts_diff, ts_rx[i].full_secs, ts_rx[i].frac_secs);

    /* Compute number of samples */
    nof_samples[i] = (uint32_t) floor(srslte_timestamp_real(&ts_diff) * rx_srate);

    if (nof_samples[i] > MAX_NOF_ALIGN_SAMPLES) {
      RADIO_INFO("The number of samples (%ld) to align is too large (>%ld), returning false\n",
                 nof_samples[i],
                 MAX_NOF_ALIGN_SAMPLES);
      aligned = false;

    } else if (nof_samples[i]) {
      RADIO_INFO("Aligning %ld samples of radio %ld (@%.2f) [%.9f-%.9f=%.9f] %ld\n",
                 nof_samples[i],
                 i,
                 rx_srate * 1e-6,
                 srslte_timestamp_real(&ts_newer),
                 srslte_timestamp_real(&ts_rx[i]),
                 srslte_timestamp_real(&ts_diff),
                 newer_idx);
    }

  }

  /* Read samples */
  while (count_done < radios.size() && aligned) {
    count_done = 0;
    for (size_t i = 0; i < radios.size(); i++) {
      if (nof_samples[i]) {

        size_t n = SRSLTE_MIN(TEMP_BUFFER_SIZE, nof_samples[i]);
        radios[i]->rx_now(temp_buffers[i], n, NULL);
        nof_samples[i] -= n;
      } else {
        count_done++;
      }
    }

    /* Synchronize radios */
    if (!synch_wait()) {
      synch_issue();
    }

  }

  return aligned;
}

radio *radio_multi::get_radio_ptr(uint32_t idx) {
  radio *ret = NULL;

  if (idx < radios.size()) {
    ret = radios[idx];
  }

  return ret;
}

srslte_rf_info_t *radio_multi::get_info(uint32_t radio_idx) {
  srslte_rf_info_t *ret = NULL;

  if (radio_idx < radios.size()) {
    ret = radios[radio_idx]->get_info();
  }

  return ret;
}

}
