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
#include "srsue/hdr/phy/phy_nr_sa.h"

namespace srsue {

phy_nr_sa::phy_nr_sa(stack_interface_phy_sa_nr& stack_, srsran::radio_interface_phy& radio_) :
  logger(srslog::fetch_basic_logger("PHY-NR")),
  sync(stack_, radio_, workers),
  workers(4)
{}

bool phy_nr_sa::init(const args_t& args)
{
  nr::sync_sa::args_t sync_args = {};
  sync_args.srate_hz            = args.srate_hz;
  if (not sync.init(sync_args)) {
    logger.error("Error initialising SYNC");
    return false;
  }

  return true;
}

} // namespace srsue