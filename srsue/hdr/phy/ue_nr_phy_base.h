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
 * File:        ue_nr_phy_base.h
 * Description: Base class for UE NR PHYs.
 *****************************************************************************/

#ifndef SRSUE_UE_NR_PHY_BASE_H
#define SRSUE_UE_NR_PHY_BASE_H

#include "srsran/interfaces/radio_interfaces.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsue/hdr/phy/ue_phy_base.h"

namespace srsue {

class ue_nr_phy_base : public phy_interface_stack_nr
{
public:
  ue_nr_phy_base(){};
  virtual ~ue_nr_phy_base() {}

  virtual std::string get_type() = 0;

  virtual int init(const phy_args_nr_t& args_, stack_interface_phy_nr* stack_, srsran::radio_interface_phy* radio_) = 0;
  virtual void stop()                                                                                               = 0;

  virtual void set_earfcn(std::vector<uint32_t> earfcns) = 0;

  virtual void get_metrics(const srsran::srsran_rat_t& rat, phy_metrics_t* m) = 0;
};

} // namespace srsue

#endif // SRSUE_UE_NR_PHY_BASE_H