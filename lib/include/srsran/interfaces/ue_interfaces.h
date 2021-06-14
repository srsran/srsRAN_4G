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
 * File:        interfaces.h
 * Description: Abstract base class interfaces provided by layers
 *              to other layers.
 *****************************************************************************/

#ifndef SRSRAN_UE_INTERFACES_H
#define SRSRAN_UE_INTERFACES_H

#include "ue_mac_interfaces.h"
#include "ue_rrc_interfaces.h"

namespace srsue {

// STACK interface for RRC
class stack_interface_rrc
{
public:
  virtual srsran::tti_point get_current_tti()                                                              = 0;
  virtual void              add_eps_bearer(uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid) = 0;
  virtual void              remove_eps_bearer(uint8_t eps_bearer_id)                                       = 0;
};

// Combined interface for PHY to access stack (MAC and RRC)
class stack_interface_phy_lte : public mac_interface_phy_lte, public rrc_interface_phy_lte
{
public:
  /* Indicate new TTI */
  virtual void run_tti(const uint32_t tti, const uint32_t tti_jump) = 0;
};

} // namespace srsue

#endif // SRSRAN_UE_INTERFACES_H
