/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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

#include "srslte/srslte.h"
extern "C" {
#include "srslte/phy/rf/rf.h"
}
#include "srslte/common/trace.h"

#include "srslte/radio/radio.h"

#ifndef SRSLTE_RADIO_MULTI_H
#define SRSLTE_RADIO_MULTI_H


namespace srslte {
  
/* Interface to the RF frontend. 
  */
  class radio_multi : public radio
  {
    public: 
      
      bool init_multi(uint32_t nof_rx_antennas, char *args = NULL, char *devname = NULL);
      bool rx_now(cf_t *buffer[SRSLTE_MAX_PORTS], uint32_t nof_samples, srslte_timestamp_t *rxd_time);
  }; 
}

#endif // SRSLTE_RADIO_MULTI_H
