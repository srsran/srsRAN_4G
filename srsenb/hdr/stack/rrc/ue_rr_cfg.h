/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSENB_UE_RR_CFG_H
#define SRSENB_UE_RR_CFG_H

#include "srslte/asn1/rrc_asn1.h"
#include "srslte/common/logmap.h"
#include "srslte/interfaces/rrc_interface_types.h"

namespace srsenb {

struct rrc_cfg_t;
class cell_ctxt_dedicated_list;
class bearer_cfg_handler;

/// Fill RadioResourceConfigDedicated with RRCSetup/Reestablishment data
void fill_rr_cfg_ded_setup(asn1::rrc::rr_cfg_ded_s&        rr_cfg,
                           const rrc_cfg_t&                enb_cfg,
                           const cell_ctxt_dedicated_list& ue_cell_list);

/// Fill RadioResourceConfigDedicated with RRCReconf data
void fill_rr_cfg_ded_reconf(asn1::rrc::rr_cfg_ded_s&             rr_cfg,
                            const asn1::rrc::rr_cfg_ded_s&       current_rr_cfg,
                            const rrc_cfg_t&                     enb_cfg,
                            const cell_ctxt_dedicated_list&      ue_cell_list,
                            const bearer_cfg_handler&            bearers,
                            const srslte::rrc_ue_capabilities_t& ue_caps,
                            bool                                 phy_cfg_updated);

void apply_rr_cfg_ded_diff(asn1::rrc::rr_cfg_ded_s&       current_rr_cfg_ded,
                           const asn1::rrc::rr_cfg_ded_s& pending_rr_cfg_ded);

void fill_scells_reconf(asn1::rrc::rrc_conn_recfg_r8_ies_s&           recfg_r8,
                        const asn1::rrc::scell_to_add_mod_list_r10_l& current_scells,
                        const rrc_cfg_t&                              enb_cfg,
                        const cell_ctxt_dedicated_list&               ue_cell_list,
                        const srslte::rrc_ue_capabilities_t&          ue_caps);

void apply_reconf_diff(asn1::rrc::rr_cfg_ded_s&                  current_rr_cfg_ded,
                       asn1::rrc::scell_to_add_mod_list_r10_l&   current_scells,
                       const asn1::rrc::rrc_conn_recfg_r8_ies_s& recfg_r8);

} // namespace srsenb

#endif // SRSENB_UE_RR_CFG_H
