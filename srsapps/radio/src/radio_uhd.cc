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
  if (cuhd_send_timed(uhd, buffer, nof_samples, tx_time.full_secs, tx_time.frac_secs) > 0) {
    return true; 
  } else {
    return false; 
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
  //cuhd_set_tx_freq(uhd, freq);
  cuhd_set_tx_freq_offset(uhd, freq, 8e6);  
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
  cuhd_set_tx_srate(uhd, srate);
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

