/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

#include <string.h>

#include "srslte/phy/rf/rf.h"
#include "srslte/srslte.h"
#include "rf_dev.h"

int rf_get_available_devices(char **devnames, int max_strlen) {
  int i=0;
  while(available_devices[i]->name) {
    strncpy(devnames[i], available_devices[i]->name, max_strlen);
    i++;
  }
  return i;
}

double srslte_rf_set_rx_gain_th(srslte_rf_t *rf, double gain)
{
  if (gain > rf->cur_rx_gain + 2 || gain < rf->cur_rx_gain - 2){
    pthread_mutex_lock(&rf->mutex);
    rf->new_rx_gain = gain; 
    pthread_cond_signal(&rf->cond);
    pthread_mutex_unlock(&rf->mutex);
  }
  return rf->cur_rx_gain;
}

void srslte_rf_set_tx_rx_gain_offset(srslte_rf_t *rf, double offset) {
  rf->tx_rx_gain_offset = offset; 
}

/* This thread listens for set_rx_gain commands to the USRP */
static void* thread_gain_fcn(void *h) {
  srslte_rf_t* rf = (srslte_rf_t*) h;
  
  while(1) {
    pthread_mutex_lock(&rf->mutex);
    while(rf->cur_rx_gain == rf->new_rx_gain) 
    {
      pthread_cond_wait(&rf->cond, &rf->mutex);
    }
    if (rf->new_rx_gain != rf->cur_rx_gain) {
      srslte_rf_set_rx_gain(h, rf->new_rx_gain);
      rf->cur_rx_gain = srslte_rf_get_rx_gain(h);
      rf->new_rx_gain = rf->cur_rx_gain;
    }
    if (rf->tx_gain_same_rx) {
      printf("setting also tx\n");
      srslte_rf_set_tx_gain(h, rf->cur_rx_gain+rf->tx_rx_gain_offset);
    }
    pthread_mutex_unlock(&rf->mutex);
  }
  return NULL; 
}


/* Create auxiliary thread and mutexes for AGC */
int srslte_rf_start_gain_thread(srslte_rf_t *rf, bool tx_gain_same_rx) {
  rf->tx_gain_same_rx = tx_gain_same_rx;
  rf->tx_rx_gain_offset = 0.0;
  if (pthread_mutex_init(&rf->mutex, NULL)) {
    return -1;
  }
  if (pthread_cond_init(&rf->cond, NULL)) {
    return -1;
  }
  if (pthread_create(&rf->thread_gain, NULL, thread_gain_fcn, rf)) {
    perror("pthread_create");
    return -1;
  }
  return 0;
}

const char* srslte_rf_get_devname(srslte_rf_t *rf) {
  return ((rf_dev_t*) rf->dev)->name;
}

int srslte_rf_open_devname(srslte_rf_t *rf, char *devname, char *args, uint32_t nof_channels) {
  /* Try to open the device if name is provided */
  if (devname) {
    if (devname[0] != '\0') {
      int i=0;
      while(available_devices[i] != NULL) {
        if (!strcmp(available_devices[i]->name, devname)) {
          rf->dev = available_devices[i];
          return available_devices[i]->srslte_rf_open_multi(args, &rf->handler, nof_channels);
        }
        i++;
      }    
      printf("Device %s not found. Switching to auto mode\n", devname);
    }
  }
  
  /* If in auto mode or provided device not found, try to open in order of apperance in available_devices[] array */
  int i=0;
  while(available_devices[i] != NULL) {
    if (!available_devices[i]->srslte_rf_open_multi(args, &rf->handler, nof_channels)) {
      rf->dev = available_devices[i];
      return 0; 
    }
    i++;
  }
  fprintf(stderr, "No compatible RF frontend found\n");
  return -1; 
}

void srslte_rf_set_tx_cal(srslte_rf_t *rf, srslte_rf_cal_t *cal) {
  return ((rf_dev_t*) rf->dev)->srslte_rf_set_tx_cal(rf->handler, cal);  
}

void srslte_rf_set_rx_cal(srslte_rf_t *rf, srslte_rf_cal_t *cal) {
  return ((rf_dev_t*) rf->dev)->srslte_rf_set_rx_cal(rf->handler, cal);  
}


const char* srslte_rf_name(srslte_rf_t *rf) {
  return ((rf_dev_t*) rf->dev)->srslte_rf_devname(rf->handler); 
}

bool srslte_rf_rx_wait_lo_locked(srslte_rf_t *rf)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_rx_wait_lo_locked(rf->handler);  
}

int srslte_rf_start_rx_stream(srslte_rf_t *rf, bool now)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_start_rx_stream(rf->handler, now);
}

int srslte_rf_stop_rx_stream(srslte_rf_t *rf)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_stop_rx_stream(rf->handler);  
}

void srslte_rf_flush_buffer(srslte_rf_t *rf)
{
  ((rf_dev_t*) rf->dev)->srslte_rf_flush_buffer(rf->handler);  
}

bool srslte_rf_has_rssi(srslte_rf_t *rf) 
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_has_rssi(rf->handler);  
}

float srslte_rf_get_rssi(srslte_rf_t *rf) 
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_get_rssi(rf->handler);  
}

void srslte_rf_suppress_stdout(srslte_rf_t *rf) 
{
  ((rf_dev_t*) rf->dev)->srslte_rf_suppress_stdout(rf->handler);  
}

void srslte_rf_register_error_handler(srslte_rf_t *rf, srslte_rf_error_handler_t error_handler)
{
  ((rf_dev_t*) rf->dev)->srslte_rf_register_error_handler(rf->handler, error_handler);  
}

int srslte_rf_open(srslte_rf_t *h, char *args) 
{
  return srslte_rf_open_devname(h, NULL, args, 1);
}

int srslte_rf_open_multi(srslte_rf_t *h, char *args, uint32_t nof_channels)
{
  return srslte_rf_open_devname(h, NULL, args, nof_channels);
}

int srslte_rf_close(srslte_rf_t *rf)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_close(rf->handler);  
}

void srslte_rf_set_master_clock_rate(srslte_rf_t *rf, double rate) 
{
  ((rf_dev_t*) rf->dev)->srslte_rf_set_master_clock_rate(rf->handler, rate);  
}

bool srslte_rf_is_master_clock_dynamic(srslte_rf_t *rf) 
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_is_master_clock_dynamic(rf->handler);  
}

double srslte_rf_set_rx_srate(srslte_rf_t *rf, double freq)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_set_rx_srate(rf->handler, freq);  
}

double srslte_rf_set_rx_gain(srslte_rf_t *rf, double gain)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_set_rx_gain(rf->handler, gain);  
}

double srslte_rf_get_rx_gain(srslte_rf_t *rf)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_get_rx_gain(rf->handler);  
}

double srslte_rf_get_tx_gain(srslte_rf_t *rf)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_get_tx_gain(rf->handler);  
}

srslte_rf_info_t *srslte_rf_get_info(srslte_rf_t *rf) {
  srslte_rf_info_t *ret = NULL;
  if (((rf_dev_t*) rf->dev)->srslte_rf_get_info) {
     ret = ((rf_dev_t*) rf->dev)->srslte_rf_get_info(rf->handler);
  }
  return ret;
}


double srslte_rf_set_rx_freq(srslte_rf_t *rf, double freq)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_set_rx_freq(rf->handler, freq);  
}


int srslte_rf_recv(srslte_rf_t *rf, void *data, uint32_t nsamples, bool blocking)
{
  return srslte_rf_recv_with_time(rf, data, nsamples, blocking, NULL, NULL);
}

int srslte_rf_recv_multi(srslte_rf_t *rf, void **data, uint32_t nsamples, bool blocking)
{
  return srslte_rf_recv_with_time_multi(rf, data, nsamples, blocking, NULL, NULL);
}

int srslte_rf_recv_with_time(srslte_rf_t *rf,
                    void *data,
                    uint32_t nsamples,
                    bool blocking,
                    time_t *secs,
                    double *frac_secs) 
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_recv_with_time(rf->handler, data, nsamples, blocking, secs, frac_secs);  
}

int srslte_rf_recv_with_time_multi(srslte_rf_t *rf,
                                    void **data,
                                    uint32_t nsamples,
                                    bool blocking,
                                    time_t *secs,
                                    double *frac_secs) 
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_recv_with_time_multi(rf->handler, data, nsamples, blocking, secs, frac_secs);  
}

double srslte_rf_set_tx_gain(srslte_rf_t *rf, double gain)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_set_tx_gain(rf->handler, gain);  
}

double srslte_rf_set_tx_srate(srslte_rf_t *rf, double freq)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_set_tx_srate(rf->handler, freq);  
}

double srslte_rf_set_tx_freq(srslte_rf_t *rf, double freq)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_set_tx_freq(rf->handler, freq);  
}

void srslte_rf_get_time(srslte_rf_t *rf, time_t *secs, double *frac_secs) 
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_get_time(rf->handler, secs, frac_secs);  
}

                   
int srslte_rf_send_timed3(srslte_rf_t *rf,
                     void *data,
                     int nsamples,
                     time_t secs,
                     double frac_secs,                      
                     bool has_time_spec,
                     bool blocking,
                     bool is_start_of_burst,
                     bool is_end_of_burst) 
{

  return ((rf_dev_t*) rf->dev)->srslte_rf_send_timed(rf->handler, data, nsamples, secs, frac_secs, 
                                 has_time_spec, blocking, is_start_of_burst, is_end_of_burst);  
}

int srslte_rf_send_timed_multi(srslte_rf_t *rf,
                               void *data[4],
                               int nsamples,
                               time_t secs,
                               double frac_secs,
                               bool blocking,
                               bool is_start_of_burst,
                               bool is_end_of_burst)
{

  return ((rf_dev_t*) rf->dev)->srslte_rf_send_timed_multi(rf->handler, data, nsamples, secs, frac_secs,
                                                           true, blocking, is_start_of_burst, is_end_of_burst);
}

int srslte_rf_send_multi(srslte_rf_t *rf,
                     void *data[4],
                     int nsamples,
                     bool blocking,
                     bool is_start_of_burst,
                     bool is_end_of_burst)
{

  return ((rf_dev_t*) rf->dev)->srslte_rf_send_timed_multi(rf->handler, data, nsamples, 0, 0,
                                                           false, blocking, is_start_of_burst, is_end_of_burst);
}

int srslte_rf_send(srslte_rf_t *rf, void *data, uint32_t nsamples, bool blocking)
{
  return srslte_rf_send2(rf, data, nsamples, blocking, true, true); 
}

int srslte_rf_send2(srslte_rf_t *rf, void *data, uint32_t nsamples, bool blocking, bool start_of_burst, bool end_of_burst)
{
  return srslte_rf_send_timed3(rf, data, nsamples, 0, 0, false, blocking, start_of_burst, end_of_burst);
}


int srslte_rf_send_timed(srslte_rf_t *rf,
                    void *data,
                    int nsamples,
                    time_t secs,
                    double frac_secs) 
{
  return srslte_rf_send_timed2(rf, data, nsamples, secs, frac_secs, true, true);
}

int srslte_rf_send_timed2(srslte_rf_t *rf,
                    void *data,
                    int nsamples,
                    time_t secs,
                    double frac_secs,
                    bool is_start_of_burst,
                    bool is_end_of_burst) 
{
  return srslte_rf_send_timed3(rf, data, nsamples, secs, frac_secs, true, true, is_start_of_burst, is_end_of_burst);
}
