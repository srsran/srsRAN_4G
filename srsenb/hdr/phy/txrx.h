/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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
#include "srsran/config.h"
#include "srsran/interfaces/enb_time_interface.h"
#include "srsran/phy/channel/channel.h"
#include "srsran/radio/radio.h"
#include <atomic>

namespace srsenb {

class txrx final : public srsran::thread
{
public:
  txrx(srslog::basic_logger& logger);
  bool init(enb_time_interface*          enb_,
            srsran::radio_interface_phy* radio_handler,
            lte::worker_pool*            lte_workers_,
            phy_common*                  worker_com,
            prach_worker_pool*           prach_,
            uint32_t                     prio);
  bool set_nr_workers(nr::worker_pool* nr_workers_);
  void stop();

private:
  void run_thread() override;

  enb_time_interface*          enb     = nullptr;
  srsran::radio_interface_phy* radio_h = nullptr;
  srslog::basic_logger&        logger;
  lte::worker_pool*            lte_workers = nullptr;
  nr::worker_pool*             nr_workers  = nullptr;
  prach_worker_pool*           prach       = nullptr;
  phy_common*                  worker_com  = nullptr;
  srsran::channel_ptr          ul_channel  = nullptr;

  // Main system TTI counter
  uint32_t tti = 0;

  std::atomic<bool> running;
};

} // namespace srsenb

#endif // SRSENB_TXRX_H
