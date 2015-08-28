  /**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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

#include "srslte/srslte.h"
#include "srsapps/radio/radio_uhd.h"

namespace srslte {

bool radio_uhd::init()
{
  return init((char*) "");
}

bool radio_uhd::init(char *args)
{
  printf("Opening UHD device...\n");
  if (cuhd_open(args, &uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    return false;
  }
  bzero(zeros, burst_settle_max_samples*sizeof(cf_t));
  return true;    
}

bool radio_uhd::init_agc()
{
  return init_agc((char*) "");
}

void radio_uhd::set_tx_rx_gain_offset(float offset) {
  cuhd_set_tx_rx_gain_offset(uhd, offset);  
}

bool radio_uhd::init_agc(char *args)
{
  printf("Opening UHD device with threaded RX Gain control ...\n");
  if (cuhd_open_th(args, &uhd, true)) {
    fprintf(stderr, "Error opening uhd\n");
    return false;
  }
  cuhd_set_rx_gain(uhd, 40);
  cuhd_set_tx_gain(uhd, 40);

  burst_settle_samples = 0; 
  burst_settle_time_rounded = 0; 
  is_start_of_burst = true; 

  return true;    
}
bool radio_uhd::rx_at(void* buffer, uint32_t nof_samples, srslte_timestamp_t rx_time)
{
  fprintf(stderr, "Not implemented\n");
  return false; 
}

bool radio_uhd::rx_now(void* buffer, uint32_t nof_samples, srslte_timestamp_t* rxd_time)
{
  if (cuhd_recv_with_time(uhd, buffer, nof_samples, true, &rxd_time->full_secs, &rxd_time->frac_secs) > 0) {
    return true; 
  } else {
    return false; 
  }
}

void radio_uhd::get_time(srslte_timestamp_t *now) {
  cuhd_get_time(uhd, &now->full_secs, &now->frac_secs);  
}

bool radio_uhd::tx(void* buffer, uint32_t nof_samples, srslte_timestamp_t tx_time)
{
  if (is_start_of_burst) {
    
    if (burst_settle_samples != 0) {
      srslte_timestamp_t tx_time_pad; 
      srslte_timestamp_copy(&tx_time_pad, &tx_time);
      srslte_timestamp_sub(&tx_time_pad, 0, burst_settle_time_rounded); 
      save_trace(1, &tx_time_pad);
      cuhd_send_timed2(uhd, zeros, burst_settle_samples, tx_time_pad.full_secs, tx_time_pad.frac_secs, true, false);
    }        
    is_start_of_burst = false; 
  }
  
  // Save possible end of burst time 
  srslte_timestamp_copy(&end_of_burst_time, &tx_time);
  srslte_timestamp_add(&end_of_burst_time, 0, (double) nof_samples/cur_tx_srate); 
  
  save_trace(0, &tx_time);
  if (cuhd_send_timed2(uhd, buffer, nof_samples, tx_time.full_secs, tx_time.frac_secs, false, false) > 0) {
    return true; 
  } else {
    return false; 
  }
}

bool radio_uhd::tx_end()
{
  save_trace(2, &end_of_burst_time);
  cuhd_send_timed2(uhd, zeros, 0, end_of_burst_time.full_secs, end_of_burst_time.frac_secs, false, true);
  is_start_of_burst = true; 
}

void radio_uhd::start_trace() {
  trace_enabled = true; 
}

void radio_uhd::set_tti(uint32_t tti_) {
  tti = tti_; 
}

void radio_uhd::write_trace(std::string filename)
{
  tr_local_time.writeToBinary(filename + ".local");
  tr_is_eob.writeToBinary(filename + ".eob");
  tr_usrp_time.writeToBinary(filename + ".usrp");
  tr_tx_time.writeToBinary(filename + ".tx");
}

void radio_uhd::save_trace(uint32_t is_eob, srslte_timestamp_t *tx_time) {
  if (trace_enabled) {
    tr_local_time.push_cur_time_us(tti);
    srslte_timestamp_t usrp_time; 
    cuhd_get_time(uhd, &usrp_time.full_secs, &usrp_time.frac_secs);
    tr_usrp_time.push(tti, srslte_timestamp_uint32(&usrp_time));
    tr_tx_time.push(tti, srslte_timestamp_uint32(tx_time));
    tr_is_eob.push(tti, is_eob);
  }
}

void radio_uhd::set_rx_freq(float freq)
{
  cuhd_set_rx_freq(uhd, freq);
}

void radio_uhd::set_rx_gain(float gain)
{
  cuhd_set_rx_gain(uhd, gain);
}

double radio_uhd::set_rx_gain_th(float gain)
{
  return cuhd_set_rx_gain_th(uhd, gain);
}

void radio_uhd::set_rx_srate(float srate)
{
  cuhd_set_rx_srate(uhd, srate);
}

void radio_uhd::set_tx_freq(float freq)
{
  cuhd_set_tx_freq_offset(uhd, freq, lo_offset);  
}

void radio_uhd::set_tx_gain(float gain)
{
  cuhd_set_tx_gain(uhd, gain);
}

float radio_uhd::get_tx_gain()
{
  return cuhd_get_tx_gain(uhd);
}

float radio_uhd::get_rx_gain()
{
  return cuhd_get_rx_gain(uhd);
}

void radio_uhd::set_tx_srate(float srate)
{
  cur_tx_srate = cuhd_set_tx_srate(uhd, srate);
  burst_settle_samples = (uint32_t) (cur_tx_srate * burst_settle_time);
  if (burst_settle_samples > burst_settle_max_samples) {
    burst_settle_samples = burst_settle_max_samples;
    fprintf(stderr, "Error setting TX srate %.1f MHz. Maximum frequency for zero prepadding is 30.72 MHz\n", srate*1e-6);
  }
  burst_settle_time_rounded = (double) burst_settle_samples/cur_tx_srate;
}

void radio_uhd::start_rx()
{
  cuhd_start_rx_stream(uhd);
}

void radio_uhd::stop_rx()
{
  cuhd_stop_rx_stream(uhd);
}

  
}

