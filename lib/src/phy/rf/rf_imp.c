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

#include <string.h>

#include "rf_dev.h"
#include "srsran/phy/rf/rf.h"
#include "srsran/srsran.h"

int rf_get_available_devices(char** devnames, int max_strlen)
{
  int i = 0;
  while (available_devices[i]->name) {
    strncpy(devnames[i], available_devices[i]->name, max_strlen);
    i++;
  }
  return i;
}

int srsran_rf_set_rx_gain_th(srsran_rf_t* rf, double gain)
{
  if (gain > rf->cur_rx_gain + 2 || gain < rf->cur_rx_gain - 2) {
    pthread_mutex_lock(&rf->mutex);
    rf->new_rx_gain = gain;
    pthread_cond_signal(&rf->cond);
    pthread_mutex_unlock(&rf->mutex);
  }
  return SRSRAN_SUCCESS;
}

void srsran_rf_set_tx_rx_gain_offset(srsran_rf_t* rf, double offset)
{
  rf->tx_rx_gain_offset = offset;
}

/* This thread listens for set_rx_gain commands to the USRP */
static void* thread_gain_fcn(void* h)
{
  srsran_rf_t* rf = (srsran_rf_t*)h;

  while (rf->thread_gain_run) {
    pthread_mutex_lock(&rf->mutex);
    while (rf->cur_rx_gain == rf->new_rx_gain && rf->thread_gain_run) {
      pthread_cond_wait(&rf->cond, &rf->mutex);
    }
    if (rf->new_rx_gain != rf->cur_rx_gain) {
      srsran_rf_set_rx_gain(h, rf->new_rx_gain);
      rf->cur_rx_gain = srsran_rf_get_rx_gain(h);
      rf->new_rx_gain = rf->cur_rx_gain;
    }
    if (rf->tx_gain_same_rx) {
      srsran_rf_set_tx_gain(h, rf->cur_rx_gain + rf->tx_rx_gain_offset);
    }
    pthread_mutex_unlock(&rf->mutex);
  }
  return NULL;
}

/* Create auxiliary thread and mutexes for AGC */
int srsran_rf_start_gain_thread(srsran_rf_t* rf, bool tx_gain_same_rx)
{
  rf->tx_gain_same_rx   = tx_gain_same_rx;
  rf->tx_rx_gain_offset = 0.0;
  if (pthread_mutex_init(&rf->mutex, NULL)) {
    return -1;
  }
  if (pthread_cond_init(&rf->cond, NULL)) {
    return -1;
  }
  rf->thread_gain_run = true;
  if (pthread_create(&rf->thread_gain, NULL, thread_gain_fcn, rf)) {
    perror("pthread_create");
    rf->thread_gain_run = false;
    return -1;
  }
  return 0;
}

const char* srsran_rf_get_devname(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->name;
}

int srsran_rf_open_devname(srsran_rf_t* rf, const char* devname, char* args, uint32_t nof_channels)
{
  rf->thread_gain_run = false;
  /* Try to open the device if name is provided */
  if (devname) {
    if (devname[0] != '\0') {
      int i = 0;
      while (available_devices[i] != NULL) {
        if (!strcasecmp(available_devices[i]->name, devname)) {
          rf->dev = available_devices[i];
          return available_devices[i]->srsran_rf_open_multi(args, &rf->handler, nof_channels);
        }
        i++;
      }
      printf("Device %s not found. Switching to auto mode\n", devname);
    }
  }

  /* If in auto mode or provided device not found, try to open in order of apperance in available_devices[] array */
  int i = 0;
  while (available_devices[i] != NULL) {
    if (!available_devices[i]->srsran_rf_open_multi(args, &rf->handler, nof_channels)) {
      rf->dev = available_devices[i];
      return 0;
    }
    i++;
  }
  ERROR("No compatible RF frontend found");
  return -1;
}

const char* srsran_rf_name(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_devname(rf->handler);
}

int srsran_rf_start_rx_stream(srsran_rf_t* rf, bool now)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_start_rx_stream(rf->handler, now);
}

int srsran_rf_stop_rx_stream(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_stop_rx_stream(rf->handler);
}

void srsran_rf_flush_buffer(srsran_rf_t* rf)
{
  ((rf_dev_t*)rf->dev)->srsran_rf_flush_buffer(rf->handler);
}

bool srsran_rf_has_rssi(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_has_rssi(rf->handler);
}

float srsran_rf_get_rssi(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_get_rssi(rf->handler);
}

void srsran_rf_suppress_stdout(srsran_rf_t* rf)
{
  ((rf_dev_t*)rf->dev)->srsran_rf_suppress_stdout(rf->handler);
}

void srsran_rf_register_error_handler(srsran_rf_t* rf, srsran_rf_error_handler_t error_handler, void* arg)
{
  ((rf_dev_t*)rf->dev)->srsran_rf_register_error_handler(rf->handler, error_handler, arg);
}

int srsran_rf_open(srsran_rf_t* h, char* args)
{
  return srsran_rf_open_devname(h, NULL, args, 1);
}

int srsran_rf_open_multi(srsran_rf_t* h, char* args, uint32_t nof_channels)
{
  return srsran_rf_open_devname(h, NULL, args, nof_channels);
}

int srsran_rf_close(srsran_rf_t* rf)
{
  // Stop gain thread
  if (rf->thread_gain_run) {
    rf->thread_gain_run = false;
    pthread_cond_signal(&rf->cond);
    pthread_join(rf->thread_gain, NULL);
  }

  return ((rf_dev_t*)rf->dev)->srsran_rf_close(rf->handler);
}

double srsran_rf_set_rx_srate(srsran_rf_t* rf, double freq)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_rx_srate(rf->handler, freq);
}

int srsran_rf_set_rx_gain(srsran_rf_t* rf, double gain)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_rx_gain(rf->handler, gain);
}

int srsran_rf_set_rx_gain_ch(srsran_rf_t* rf, uint32_t ch, double gain)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_rx_gain_ch(rf->handler, ch, gain);
}

double srsran_rf_get_rx_gain(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_get_rx_gain(rf->handler);
}

double srsran_rf_get_tx_gain(srsran_rf_t* rf)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_get_tx_gain(rf->handler);
}

srsran_rf_info_t* srsran_rf_get_info(srsran_rf_t* rf)
{
  srsran_rf_info_t* ret = NULL;
  if (((rf_dev_t*)rf->dev)->srsran_rf_get_info) {
    ret = ((rf_dev_t*)rf->dev)->srsran_rf_get_info(rf->handler);
  }
  return ret;
}

double srsran_rf_set_rx_freq(srsran_rf_t* rf, uint32_t ch, double freq)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_rx_freq(rf->handler, ch, freq);
}

int srsran_rf_recv(srsran_rf_t* rf, void* data, uint32_t nsamples, bool blocking)
{
  return srsran_rf_recv_with_time(rf, data, nsamples, blocking, NULL, NULL);
}

int srsran_rf_recv_multi(srsran_rf_t* rf, void** data, uint32_t nsamples, bool blocking)
{
  return srsran_rf_recv_with_time_multi(rf, data, nsamples, blocking, NULL, NULL);
}

int srsran_rf_recv_with_time(srsran_rf_t* rf,
                             void*        data,
                             uint32_t     nsamples,
                             bool         blocking,
                             time_t*      secs,
                             double*      frac_secs)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_recv_with_time(rf->handler, data, nsamples, blocking, secs, frac_secs);
}

int srsran_rf_recv_with_time_multi(srsran_rf_t* rf,
                                   void**       data,
                                   uint32_t     nsamples,
                                   bool         blocking,
                                   time_t*      secs,
                                   double*      frac_secs)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_recv_with_time_multi(rf->handler, data, nsamples, blocking, secs, frac_secs);
}

int srsran_rf_set_tx_gain(srsran_rf_t* rf, double gain)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_tx_gain(rf->handler, gain);
}

int srsran_rf_set_tx_gain_ch(srsran_rf_t* rf, uint32_t ch, double gain)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_tx_gain_ch(rf->handler, ch, gain);
}

double srsran_rf_set_tx_srate(srsran_rf_t* rf, double freq)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_tx_srate(rf->handler, freq);
}

double srsran_rf_set_tx_freq(srsran_rf_t* rf, uint32_t ch, double freq)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_set_tx_freq(rf->handler, ch, freq);
}

void srsran_rf_get_time(srsran_rf_t* rf, time_t* secs, double* frac_secs)
{
  return ((rf_dev_t*)rf->dev)->srsran_rf_get_time(rf->handler, secs, frac_secs);
}

int srsran_rf_sync(srsran_rf_t* rf)
{
  int ret = SRSRAN_ERROR;

  if (((rf_dev_t*)rf->dev)->srsran_rf_sync_pps) {
    ((rf_dev_t*)rf->dev)->srsran_rf_sync_pps(rf->handler);

    ret = SRSRAN_SUCCESS;
  }

  return ret;
}

int srsran_rf_send_timed3(srsran_rf_t* rf,
                          void*        data,
                          int          nsamples,
                          time_t       secs,
                          double       frac_secs,
                          bool         has_time_spec,
                          bool         blocking,
                          bool         is_start_of_burst,
                          bool         is_end_of_burst)
{
  return ((rf_dev_t*)rf->dev)
      ->srsran_rf_send_timed(
          rf->handler, data, nsamples, secs, frac_secs, has_time_spec, blocking, is_start_of_burst, is_end_of_burst);
}

int srsran_rf_send_timed_multi(srsran_rf_t* rf,
                               void**       data,
                               int          nsamples,
                               time_t       secs,
                               double       frac_secs,
                               bool         blocking,
                               bool         is_start_of_burst,
                               bool         is_end_of_burst)
{
  return ((rf_dev_t*)rf->dev)
      ->srsran_rf_send_timed_multi(
          rf->handler, data, nsamples, secs, frac_secs, true, blocking, is_start_of_burst, is_end_of_burst);
}

int srsran_rf_send_multi(srsran_rf_t* rf,
                         void**       data,
                         int          nsamples,
                         bool         blocking,
                         bool         is_start_of_burst,
                         bool         is_end_of_burst)
{
  return ((rf_dev_t*)rf->dev)
      ->srsran_rf_send_timed_multi(
          rf->handler, data, nsamples, 0, 0, false, blocking, is_start_of_burst, is_end_of_burst);
}

int srsran_rf_send(srsran_rf_t* rf, void* data, uint32_t nsamples, bool blocking)
{
  return srsran_rf_send2(rf, data, nsamples, blocking, true, true);
}

int srsran_rf_send2(srsran_rf_t* rf,
                    void*        data,
                    uint32_t     nsamples,
                    bool         blocking,
                    bool         start_of_burst,
                    bool         end_of_burst)
{
  return srsran_rf_send_timed3(rf, data, nsamples, 0, 0, false, blocking, start_of_burst, end_of_burst);
}

int srsran_rf_send_timed(srsran_rf_t* rf, void* data, int nsamples, time_t secs, double frac_secs)
{
  return srsran_rf_send_timed2(rf, data, nsamples, secs, frac_secs, true, true);
}

int srsran_rf_send_timed2(srsran_rf_t* rf,
                          void*        data,
                          int          nsamples,
                          time_t       secs,
                          double       frac_secs,
                          bool         is_start_of_burst,
                          bool         is_end_of_burst)
{
  return srsran_rf_send_timed3(rf, data, nsamples, secs, frac_secs, true, true, is_start_of_burst, is_end_of_burst);
}
