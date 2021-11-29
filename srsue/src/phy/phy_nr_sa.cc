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

phy_nr_sa::phy_nr_sa(const char* logname) :
  logger(srslog::fetch_basic_logger(logname)),
  sync(logger, workers),
  workers(logger, 4),
  common(logger),
  prach_buffer(logger)
{}

int phy_nr_sa::init(const phy_args_nr_t& args_, stack_interface_phy_nr* stack_, srsran::radio_interface_phy* radio_)
{
  args = args_;

  prach_buffer.init(SRSRAN_MAX_PRB);

  nr::sync_sa::args_t sync_args = {};
  if (not sync.init(sync_args, stack_, radio_)) {
    logger.error("Error initialising SYNC");
    return false;
  }

  return true;
}

phy_interface_rrc_nr::phy_nr_state_t phy_nr_sa::get_state() const
{
  {
    switch (sync.get_state()) {
      case nr::sync_sa::STATE_IDLE:
        break;
      case nr::sync_sa::STATE_CELL_SEARCH:
        return phy_interface_rrc_nr::PHY_NR_STATE_CELL_SEARCH;
      case nr::sync_sa::STATE_CELL_SELECT:
        return phy_interface_rrc_nr::PHY_NR_STATE_CELL_SELECT;
    }
  }
  return phy_interface_rrc_nr::PHY_NR_STATE_IDLE;
}

void phy_nr_sa::reset_nr()
{
  sync.go_idle();
}

bool phy_nr_sa::start_cell_search(const cell_search_args_t& req)
{
  // Prepare cell search configuration from the request
  nr::cell_search::cfg_t cfg = {};
  cfg.srate_hz               = 0; // args.srate_hz;
  cfg.center_freq_hz         = req.center_freq_hz;
  cfg.ssb_freq_hz            = req.ssb_freq_hz;
  cfg.ssb_scs                = req.ssb_scs;
  cfg.ssb_pattern            = req.ssb_pattern;
  cfg.duplex_mode            = req.duplex_mode;

  // Request cell search to lower synchronization instance
  return sync.start_cell_search(cfg);
}

} // namespace srsue