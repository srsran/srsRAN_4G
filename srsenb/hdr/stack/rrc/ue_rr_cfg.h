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

/*----------------------------------------------------------------------------------------------------------------------
 * \file
 * These .h/.cc files contain the functions responsible for setting the ASN1 fields
 * related to radio resource configuration from the eNB side (see TS 36.331 - 5.3.10)
 *
 * The following function naming convention is followed:
 * - suffix "_enb_cfg" - any function related to setting asn1 fields based on the configuration passed to the eNB
 *                       through parsed configuration files
 * - suffix "_setup" -   the asn1 fields are set using the respective "_enb_cfg" function as default, plus any other
 *                       parameter known at runtime (e.g. SR/CQI resources). Functions with this suffix are called
 *                       during the rrcConnectionSetup and rrcConnectionReestablishment proceduress
 * - suffix "_reconf" -  sets the asn1 fields that are known during the rrcConnectionReconfiguration. That includes
 *                       "_enb_cfg" + "_setup" fields + DRBs + SCells, etc. It compares the current UE RRC configuration
 *                       (accumulation of the RRC messages sent so far), with the next one that will result from
 *                       the RRC setup/reestablishment/reconfiguration procedure. Based on this comparison, it will
 *                       fill the output ASN1 struct with fields that are strictly necessary
 * - suffix "_diff" -    update the struct relative to the current UE RRC config with the fields present in
 *                       the Reconf/Setup/Reestablishment message that is about to be sent to the UE
 *
 *--------------------------------------------------------------------------------------------------------------------*/

#ifndef SRSENB_UE_RR_CFG_H
#define SRSENB_UE_RR_CFG_H

#include "srsran/asn1/rrc.h"
#include "srsran/interfaces/rrc_interface_types.h"

namespace srsenb {

struct rrc_cfg_t;
class ue_cell_ded_list;
class bearer_cfg_handler;
struct ue_var_cfg_t;

/// Fill RadioResourceConfigDedicated with data known at the RRCSetup/Reestablishment stage
int fill_rr_cfg_ded_setup(asn1::rrc::rr_cfg_ded_s& rr_cfg,
                          const rrc_cfg_t&         enb_cfg,
                          const ue_cell_ded_list&  ue_cell_list);

/// Apply Reconf updates and update current state
int apply_reconf_updates(asn1::rrc::rrc_conn_recfg_r8_ies_s&  recfg_r8,
                         ue_var_cfg_t&                        current_ue_cfg,
                         const rrc_cfg_t&                     enb_cfg,
                         const ue_cell_ded_list&              ue_cell_list,
                         bearer_cfg_handler&                  bearers,
                         const srsran::rrc_ue_capabilities_t& ue_caps,
                         bool                                 phy_cfg_updated);

/// Apply radioResourceConfigDedicated updates to the current UE RRC configuration
void apply_rr_cfg_ded_diff(asn1::rrc::rr_cfg_ded_s&       current_rr_cfg_ded,
                           const asn1::rrc::rr_cfg_ded_s& pending_rr_cfg_ded);

/// Apply Scell updates to the current UE RRC configuration
void apply_scells_to_add_diff(asn1::rrc::scell_to_add_mod_list_r10_l&   current_scells,
                              const asn1::rrc::rrc_conn_recfg_r8_ies_s& recfg_r8);

} // namespace srsenb

#endif // SRSENB_UE_RR_CFG_H
