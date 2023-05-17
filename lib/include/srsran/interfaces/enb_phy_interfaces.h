/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsran/interfaces/rrc_interface_types.h"
#include "srsran/phy/common/phy_common.h"

#ifndef SRSRAN_ENB_PHY_INTERFACES_H
#define SRSRAN_ENB_PHY_INTERFACES_H

namespace srsenb {

/* Interface MAC -> PHY */
class phy_interface_mac_lte
{
public:
  /**
   * Removes an RNTI context from all the physical layer components, including secondary cells
   * @param rnti identifier of the user
   */
  virtual void rem_rnti(uint16_t rnti) = 0;

  /**
   *
   * @param stop
   */
  virtual void set_mch_period_stop(uint32_t stop) = 0;

  /**
   * Activates and/or deactivates Secondary Cells in the PHY for a given RNTI. Requires the RNTI of the given UE and a
   * vector with the activation/deactivation values. Use true for activation and false for deactivation. The index 0 is
   * reserved for PCell and will not be used.
   *
   * @param rnti identifier of the user
   * @param activation vector with the activate/deactivate.
   */
  virtual void set_activation_deactivation_scell(uint16_t                                     rnti,
                                                 const std::array<bool, SRSRAN_MAX_CARRIERS>& activation) = 0;
};

/* Interface RRC -> PHY */
class phy_interface_rrc_lte
{
public:
  srsran::phy_cfg_mbsfn_t mbsfn_cfg;

  virtual void configure_mbsfn(srsran::sib2_mbms_t* sib2, srsran::sib13_t* sib13, const srsran::mcch_msg_t& mcch) = 0;

  struct phy_rrc_cfg_t {
    bool              configured = false; ///< Indicates whether PHY shall consider configuring this cell/carrier
    uint32_t          enb_cc_idx = 0;     ///< eNb Cell index
    srsran::phy_cfg_t phy_cfg    = {};    ///< Dedicated physical layer configuration
  };

  typedef std::vector<phy_rrc_cfg_t> phy_rrc_cfg_list_t;

  /**
   * Sets the physical layer dedicated configuration for a given RNTI. The dedicated configuration list shall provide
   * all the required information configuration for the following cases:
   * - Add an RNTI straight from RRC
   * - Moving primary to another serving cell
   * - Add/Remove secondary serving cells
   *
   * Remind this call will partially reconfigure the primary serving cell, `complete_config``shall be called
   * in order to complete the configuration.
   *
   * @param rnti the given RNTI
   * @param phy_cfg_list Physical layer configuration for the indicated eNb cell
   */
  virtual void set_config(uint16_t rnti, const phy_rrc_cfg_list_t& phy_cfg_list) = 0;

  /**
   * Instructs the physical layer the configuration has been complete from upper layers for a given RNTI
   *
   * @param rnti the given UE identifier (RNTI)
   */
  virtual void complete_config(uint16_t rnti) = 0;
};

// Combined interface for stack (MAC and RRC) to access PHY
class phy_interface_stack_lte : public phy_interface_mac_lte, public phy_interface_rrc_lte
{};

} // namespace srsenb

#endif // SRSRAN_ENB_PHY_INTERFACES_H
