/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/******************************************************************************
 * File:        interfaces.h
 * Description: Abstract base class interfaces provided by layers
 *              to other layers.
 *****************************************************************************/

#ifndef SRSLTE_UE_INTERFACES_H
#define SRSLTE_UE_INTERFACES_H

#include "ue_mac_interfaces.h"
#include "ue_rrc_interfaces.h"

namespace srsue {

// STACK interface for RRC
class stack_interface_rrc
{
public:
  virtual srslte::tti_point get_current_tti() = 0;
};

// Combined interface for PHY to access stack (MAC and RRC)
class stack_interface_phy_lte : public mac_interface_phy_lte, public rrc_interface_phy_lte
{
public:
  /* Indicate new TTI */
  virtual void run_tti(const uint32_t tti, const uint32_t tti_jump) = 0;
};

} // namespace srsue

#endif // SRSLTE_UE_INTERFACES_H
