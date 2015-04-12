  /**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/srslte.h"
#include "srslte/common/radio.h"
#include "srslte/cuhd/radio_uhd.h"


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
 cur_rx_freq = cuhd_set_rx_freq(uhd, freq);
}

void radio_uhd::set_rx_gain(float gain)
{
 cur_rx_gain = cuhd_set_rx_gain(uhd, gain);
}

void radio_uhd::set_rx_srate(float srate)
{
 cur_rx_srate = cuhd_set_rx_srate(uhd, srate);
}

void radio_uhd::set_tx_freq(float freq)
{
 cur_tx_freq = cuhd_set_tx_freq(uhd, freq);
}

void radio_uhd::set_tx_gain(float gain)
{
 cur_tx_gain = cuhd_set_tx_gain(uhd, gain);
}

void radio_uhd::set_tx_srate(float srate)
{
 cur_tx_srate = cuhd_set_tx_srate(uhd, srate);
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

