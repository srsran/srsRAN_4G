/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSENB_TXRX_H
#define SRSENB_TXRX_H

#include "phy_common.h"
#include "prach_worker.h"
#include "srsenb/hdr/phy/lte/worker_pool.h"
#include "srsenb/hdr/phy/nr/worker_pool.h"
#include "srslte/config.h"
#include "srslte/phy/channel/channel.h"
#include "srslte/radio/radio.h"

namespace srsenb {

class txrx final : public srslte::thread
{
public:
  txrx(srslog::basic_logger& logger);
  bool init(stack_interface_phy_lte*     stack_,
            srslte::radio_interface_phy* radio_handler,
            lte::worker_pool*            lte_workers_,
            nr::worker_pool*             nr_workers_,
            phy_common*                  worker_com,
            prach_worker_pool*           prach_,
            uint32_t                     prio);
  void stop();

private:
  void run_thread() override;

  stack_interface_phy_lte*     stack   = nullptr;
  srslte::radio_interface_phy* radio_h = nullptr;
  srslog::basic_logger&        logger;
  lte::worker_pool*            lte_workers = nullptr;
  nr::worker_pool*             nr_workers  = nullptr;
  prach_worker_pool*           prach       = nullptr;
  phy_common*                  worker_com  = nullptr;
  srslte::channel_ptr          ul_channel  = nullptr;

  // Main system TTI counter
  uint32_t tti = 0;

  uint32_t tx_worker_cnt = 0;
  uint32_t nof_workers   = 0;
  bool     running       = false;
};

} // namespace srsenb

#endif // SRSENB_TXRX_H
