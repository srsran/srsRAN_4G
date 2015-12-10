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

#include <libbladeRF.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "srslte/srslte.h"
#include "rf_blade_imp.h"
#include "srslte/rf/rf.h"

typedef struct {
} rf_blade_handler_t;


void rf_blade_suppress_stdout(void *h) {
}

void rf_blade_register_msg_handler(void *notused, rf_msg_handler_t new_handler)
{
}


bool rf_blade_rx_wait_lo_locked(void *h)
{
}

int rf_blade_start_rx_stream(void *h)
{
}

int rf_blade_stop_rx_stream(void *h)
{
}

void rf_blade_flush_buffer(void *h)
{
}

bool rf_blade_has_rssi(void *h) {
}

float rf_blade_get_rssi(void *h) {
}

double rf_blade_set_rx_gain_th(void *h, double gain)
{
}

void rf_blade_set_tx_rx_gain_offset(void *h, double offset) {
}

/* This thread listens for set_rx_gain commands to the USRP */
static void* thread_gain_fcn(void *h) {
}

int rf_blade_open(char *args, void **h, bool create_thread_gain, bool tx_gain_same_rx)
{
}


int rf_blade_close(void *h)
{
}

void rf_blade_set_master_clock_rate(void *h, double rate) {
}

bool rf_blade_is_master_clock_dynamic(void *h) {
}

double rf_blade_set_rx_srate(void *h, double freq)
{
}

double rf_blade_set_tx_srate(void *h, double freq)
{
}

double rf_blade_set_rx_gain(void *h, double gain)
{
}

double rf_blade_set_tx_gain(void *h, double gain)
{
}

double rf_blade_get_rx_gain(void *h)
{
}

double rf_blade_get_tx_gain(void *h)
{
}

double rf_blade_set_rx_freq(void *h, double freq)
{
}

double rf_blade_set_tx_freq(void *h, double freq)
{
}


void rf_blade_get_time(void *h, time_t *secs, double *frac_secs) {
}

int rf_blade_recv_with_time(void *h,
                    void *data,
                    uint32_t nsamples,
                    bool blocking,
                    time_t *secs,
                    double *frac_secs) 
{
}
                   
int rf_blade_send_timed(void *h,
                     void *data,
                     int nsamples,
                     time_t secs,
                     double frac_secs,                      
                     bool has_time_spec,
                     bool blocking,
                     bool is_start_of_burst,
                     bool is_end_of_burst) 
{
}

