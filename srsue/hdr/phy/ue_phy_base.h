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
 * File:        ue_phy_base.h
 * Description: Base class for all UE PHYs.
 *****************************************************************************/

#ifndef SRSUE_UE_PHY_BASE_H
#define SRSUE_UE_PHY_BASE_H

#include "srsran/interfaces/ue_phy_interfaces.h"
#include "srsue/hdr/phy/phy_metrics.h"

namespace srsue {

class ue_phy_base
{
public:
  ue_phy_base(){};
  virtual ~ue_phy_base(){};

  virtual std::string get_type() = 0;

  virtual void stop() = 0;

  virtual void wait_initialize() = 0;
  virtual bool is_initialized()  = 0;
  virtual void start_plot()      = 0;

  virtual void get_metrics(const srsran::srsran_rat_t& rat, phy_metrics_t* m) = 0;
};

} // namespace srsue

#endif // SRSUE_UE_PHY_BASE_H
