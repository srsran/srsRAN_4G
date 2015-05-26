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



#include "srsapps/radio/radio.h"
#include "srslte/srslte.h"
#include "srslte/cuhd/cuhd.h"

#ifndef RADIO_UHD_H
#define RADIO_UHD_H


namespace srslte {
  
/* Interface to the RF frontend. 
  */
  class radio_uhd : public radio
  {
    public: 
      bool init();
      bool init(char *args);
      bool init_agc();
      bool init_agc(char *args);

      void get_time(srslte_timestamp_t *now);
      bool tx(void *buffer, uint32_t nof_samples, srslte_timestamp_t tx_time);
      bool rx_now(void *buffer, uint32_t nof_samples, srslte_timestamp_t *rxd_time);
      bool rx_at(void *buffer, uint32_t nof_samples, srslte_timestamp_t rx_time);

      void set_tx_gain(float gain);
      void set_rx_gain(float gain);
      void set_tx_rx_gain_offset(float offset); 
      double set_rx_gain_th(float gain);

      void set_tx_freq(float freq);
      void set_rx_freq(float freq);

      void set_tx_srate(float srate);
      void set_rx_srate(float srate);

      void start_rx();
      void stop_rx();
      
    private:
      void *uhd; 
  }; 
}

#endif
