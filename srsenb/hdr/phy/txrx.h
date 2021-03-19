/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSENB_TXRX_H
#define SRSENB_TXRX_H

#include "phy_common.h"
#include "prach_worker.h"
#include "srsenb/hdr/phy/lte/worker_pool.h"
#include "srsenb/hdr/phy/nr/worker_pool.h"
#include "srsran/config.h"
#include "srsran/phy/channel/channel.h"
#include "srsran/radio/radio.h"

namespace srsenb {

class txrx final : public srsran::thread
{
public:
  txrx(srslog::basic_logger& logger);
  bool init(stack_interface_phy_lte*     stack_,
            srsran::radio_interface_phy* radio_handler,
            lte::worker_pool*            lte_workers_,
            nr::worker_pool*             nr_workers_,
            phy_common*                  worker_com,
            prach_worker_pool*           prach_,
            uint32_t                     prio);
  void stop();

private:
  void run_thread() override;

  stack_interface_phy_lte*     stack   = nullptr;
  srsran::radio_interface_phy* radio_h = nullptr;
  srslog::basic_logger&        logger;
  lte::worker_pool*            lte_workers = nullptr;
  nr::worker_pool*             nr_workers  = nullptr;
  prach_worker_pool*           prach       = nullptr;
  phy_common*                  worker_com  = nullptr;
  srsran::channel_ptr          ul_channel  = nullptr;

  // Main system TTI counter
  uint32_t tti = 0;

  uint32_t tx_worker_cnt = 0;
  uint32_t nof_workers   = 0;
  bool     running       = false;
};

} // namespace srsenb

#endif // SRSENB_TXRX_H
