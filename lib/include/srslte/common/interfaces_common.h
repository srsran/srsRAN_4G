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

#ifndef INTERFACE_COMMON_H
#define INTERFACE_COMMON_H

#include "srslte/common/timers.h"

namespace srslte {

class mac_interface_timers
{
public: 
  /* Timer services with ms resolution. 
   * timer_id must be lower than MAC_NOF_UPPER_TIMERS
   */
  virtual timers::timer* get(uint32_t timer_id) = 0;
  virtual uint32_t               get_unique_id() = 0;
};

class read_pdu_interface
{
public:
  virtual int read_pdu(uint32_t lcid, uint8_t *payload, uint32_t requested_bytes) = 0; 
};

}

#endif
