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

#include "srslte/rf/rf.h"
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


int srslte_rf_open_devname(srslte_rf_t *rf, char *devname, char *args, bool agc_thread, bool tx_gain_same_rx) {
  /* Try to open the device if name is provided */
  if (devname) {
    int i=0;
    while(available_devices[i] != NULL) {
      if (!strcmp(available_devices[i]->name, devname)) {
        rf->dev = available_devices[i];
        return available_devices[i]->srslte_rf_open(args, &rf->handler, agc_thread, tx_gain_same_rx);
      }
      i++;
    }    
    printf("Device %s not found. Switching to auto mode\n", devname);
  }
  
  /* If in auto mode or provided device not found, try to open in order of apperance in available_devices[] array */
  int i=0;
  while(available_devices[i] != NULL) {
    if (!available_devices[i]->srslte_rf_open(args, &rf->handler, agc_thread, tx_gain_same_rx)) {
      rf->dev = available_devices[i];
      return 0; 
    }
    i++;
  }
  fprintf(stderr, "No compatible RF frontend found\n");
  return -1; 
}


bool srslte_rf_rx_wait_lo_locked(srslte_rf_t *rf)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_rx_wait_lo_locked(rf->handler);  
}

int srslte_rf_start_rx_stream(srslte_rf_t *rf)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_start_rx_stream(rf->handler);  
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

double srslte_rf_set_rx_gain_th(srslte_rf_t *rf, double gain)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_set_rx_gain_th(rf->handler, gain);  
}

void srslte_rf_set_tx_rx_gain_offset(srslte_rf_t *rf, double offset) 
{
  ((rf_dev_t*) rf->dev)->srslte_rf_set_tx_rx_gain_offset(rf->handler, offset);  
}

void srslte_rf_suppress_stdout(srslte_rf_t *rf) 
{
  ((rf_dev_t*) rf->dev)->srslte_rf_suppress_stdout(rf->handler);  
}

void srslte_rf_register_msg_handler(srslte_rf_t *rf, srslte_rf_msg_handler_t msg_handler)
{
  ((rf_dev_t*) rf->dev)->srslte_rf_register_msg_handler(rf->handler, msg_handler);  
}

int srslte_rf_open(srslte_rf_t *h, char *args) 
{
  return srslte_rf_open_devname(h, NULL, args, false, false);
}

int srslte_rf_open_th(srslte_rf_t *h, char *args, bool tx_gain_same_rx) 
{
  return srslte_rf_open_devname(h, NULL, args, true, tx_gain_same_rx);  
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

double srslte_rf_set_rx_freq(srslte_rf_t *rf, double freq)
{
  return ((rf_dev_t*) rf->dev)->srslte_rf_set_rx_freq(rf->handler, freq);  
}


int srslte_rf_recv(srslte_rf_t *rf, void *data, uint32_t nsamples, bool blocking)
{
  return srslte_rf_recv_with_time(rf, data, nsamples, blocking, NULL, NULL);
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
