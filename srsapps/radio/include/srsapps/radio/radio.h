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

#include <stdint.h>
#include "srslte/srslte.h"
#include "srslte/common/timestamp.h"

#ifndef RADIO_H
#define RADIO_H


namespace srslte {
  
/* Interface to the RF frontend. 
  */
  class SRSLTE_API radio
  {
    public: 
      virtual void get_time(srslte_timestamp_t *now) = 0; 
      virtual bool tx(void *buffer, uint32_t nof_samples, srslte_timestamp_t tx_time) = 0;
      virtual bool rx_now(void *buffer, uint32_t nof_samples, srslte_timestamp_t *rxd_time) = 0;
      virtual bool rx_at(void *buffer, uint32_t nof_samples, srslte_timestamp_t rx_time) = 0;

      virtual void set_tx_gain(float gain) = 0;
      virtual void set_rx_gain(float gain) = 0;
      virtual double set_rx_gain_th(float gain) = 0;

      virtual void set_tx_freq(float freq) = 0;
      virtual void set_rx_freq(float freq) = 0;

      virtual void set_tx_srate(float srate) = 0;
      virtual void set_rx_srate(float srate) = 0;

      virtual void start_rx() = 0;
      virtual void stop_rx() = 0;

      float get_tx_gain() { return cur_tx_gain; }
      float get_rx_gain() { return cur_rx_gain; }

      float get_tx_freq() { return cur_tx_freq; }
      float get_rx_freq() { return cur_rx_freq; }
      
      float get_tx_srate() { return cur_tx_srate; }
      float get_rx_srate() { return cur_rx_srate; }
      
    protected: 
      float cur_tx_gain; 
      float cur_rx_gain; 
      float cur_tx_freq; 
      float cur_rx_freq; 
      float cur_tx_srate; 
      float cur_rx_srate; 
  }; 
}

#endif
