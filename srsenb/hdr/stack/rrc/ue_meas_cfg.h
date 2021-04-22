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

#ifndef SRSRAN_UE_MEAS_CFG_H
#define SRSRAN_UE_MEAS_CFG_H

#include "srsran/asn1/rrc/meascfg.h"

namespace srsenb {

// fwd declarations
struct rrc_cfg_t;
struct meas_cell_cfg_t;
class ue_cell_ded_list;

using meas_obj_t      = asn1::rrc::meas_obj_to_add_mod_s;
using meas_obj_list   = asn1::rrc::meas_obj_to_add_mod_list_l;
using meas_cell_t     = asn1::rrc::cells_to_add_mod_s;
using report_cfg_t    = asn1::rrc::report_cfg_to_add_mod_s;
using report_cfg_list = asn1::rrc::report_cfg_to_add_mod_list_l;
using meas_id_t       = asn1::rrc::meas_id_to_add_mod_s;
using meas_id_list    = asn1::rrc::meas_id_to_add_mod_list_l;

bool is_same_earfcn(const meas_obj_t& lhs, const meas_obj_t& rhs);

/// Find MeasObj with same earfcn. Assumes ordered list
meas_obj_t*       find_meas_obj(meas_obj_list& l, uint32_t earfcn);
const meas_obj_t* find_meas_obj(const meas_obj_list& l, uint32_t earfcn);

/// Add EARFCN to the MeasObjToAddModList
std::pair<bool, meas_obj_t*> add_meas_obj(meas_obj_list& list, uint32_t dl_earfcn);

std::tuple<bool, meas_obj_t*, meas_cell_t*> add_cell_enb_cfg(meas_obj_list&         meas_obj_list,
                                                             const meas_cell_cfg_t& cellcfg);

report_cfg_t* add_report_cfg(report_cfg_list& list, const asn1::rrc::report_cfg_eutra_s& reportcfg);

meas_id_t* add_measid_cfg(meas_id_list& meas_id_list, uint8_t measobjid, uint8_t measrepid);

/**
 * Finds a cell in meas_obj_list based on pci and earfcn
 * @return pair of (meas_obj,cell_obj). If no cell has frequency==earfcn, meas_obj=nullptr
 */
std::pair<meas_obj_t*, meas_cell_t*> find_cell(meas_obj_list& l, uint32_t earfcn, uint32_t pci);

bool compute_diff_meascfg(const asn1::rrc::meas_cfg_s& current_meascfg,
                          const asn1::rrc::meas_cfg_s& target_meascfg,
                          asn1::rrc::meas_cfg_s&       diff_meascfg);

bool fill_meascfg_enb_cfg(asn1::rrc::meas_cfg_s& meascfg, const ue_cell_ded_list& ue_cell_list);

/**
 * Computes diff measConfig based on the previous measConfig and the UE current cells
 * @param diff_meascfg resulting configuration to be added to the RRCConnectionReconf message
 * @param prev_meascfg measConfig sent in the last RRCConnectionReconfiguration to the UE
 * @param ue_cell_list contains the final list of cells configured in the UE after the Reconf procedure
 * @param prev_earfcn  DL earfcn of Pcell in the last RRCReconf message
 * @param prev_pci     pci of PCell in the last RRCReconf message
 * @return true if updates exist that have been registered in diff_meascfg
 */
bool apply_meascfg_updates(asn1::rrc::meas_cfg_s&          diff_meascfg,
                           asn1::rrc::meas_cfg_s&          prev_meascfg,
                           const ue_cell_ded_list& ue_cell_list,
                           int                             prev_earfcn = -1,
                           int                             prev_pci    = -1);

} // namespace srsenb

#endif // SRSRAN_UE_MEAS_CFG_H
