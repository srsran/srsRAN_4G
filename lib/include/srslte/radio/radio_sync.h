/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

extern "C" {
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/common/timestamp.h"
#include "srslte/phy/rf/rf.h"
}

#ifndef SRSLTE_RADIO_SYNC_H
#define SRSLTE_RADIO_SYNC_H

namespace srslte {

/* Interface to the RF frontend.
 */
class radio_sync
{
private:
  void* thread_args;

public:
  radio_sync();
  bool init(srslte_rf_t* dev);
  void issue_sync();
  void issue_rx(cf_t*               data[SRSLTE_MAX_PORTS],
                uint32_t            nsamples,
                srslte_timestamp_t* timestamp,
                bool                start_streaming = false);
  int  wait();
  ~radio_sync();
};

} // namespace srslte

#endif // SRSLTE_RADIO_SYNC_H
