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

/******************************************************************************
 * File:        enb_phy_base.h
 * Description: Base class for all eNB PHYs.
 *****************************************************************************/

#ifndef SRSENB_PHY_BASE_H
#define SRSENB_PHY_BASE_H

#include "srsenb/hdr/phy/phy_metrics.h"
#include <vector>

namespace srsenb {

class enb_phy_base
{
public:
  enb_phy_base(){};
  virtual ~enb_phy_base(){};

  virtual std::string get_type() = 0;

  virtual void stop() = 0;

  virtual void start_plot() = 0;

  virtual void get_metrics(std::vector<phy_metrics_t>& m) = 0;

  virtual void cmd_cell_gain(uint32_t cell_idx, float gain_db) = 0;

  virtual void cmd_cell_measure() = 0;
};

} // namespace srsenb

#endif // SRSENB_PHY_BASE_H
