/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/rrc/ue_meas_cfg.h"
#include "srsenb/hdr/stack/rrc/rrc_cell_cfg.h"
#include "srsran/asn1/obj_id_cmp_utils.h"
#include "srsran/asn1/rrc_utils.h"

using namespace asn1::rrc;

namespace srsenb {

/***********************************
 *          measObjToAddMod
 **********************************/

bool is_same_earfcn(const meas_obj_t& lhs, const meas_obj_t& rhs)
{
  int freq1 = srsran::get_carrier_freq(lhs);
  int freq2 = srsran::get_carrier_freq(rhs);
  return freq1 != -1 and freq1 == freq2;
}

meas_obj_t* find_meas_obj(meas_obj_to_add_mod_list_l& l, uint32_t earfcn)
{
  auto same_earfcn = [earfcn](const meas_obj_t& obj) { return (int)earfcn == srsran::get_carrier_freq(obj); };
  auto it          = std::find_if(l.begin(), l.end(), same_earfcn);
  return it == l.end() ? nullptr : &(*it);
}
const meas_obj_t* find_meas_obj(const meas_obj_to_add_mod_list_l& l, uint32_t earfcn)
{
  auto same_earfcn = [earfcn](const meas_obj_t& obj) { return (int)earfcn == srsran::get_carrier_freq(obj); };
  auto it          = std::find_if(l.begin(), l.end(), same_earfcn);
  return it == l.end() ? nullptr : &(*it);
}

std::pair<meas_obj_t*, cells_to_add_mod_s*> find_cell(meas_obj_to_add_mod_list_l& l, uint32_t earfcn, uint32_t pci)
{
  // find meas_obj with same earfcn
  meas_obj_t* obj = find_meas_obj(l, earfcn);
  if (obj == nullptr) {
    return std::make_pair(obj, (cells_to_add_mod_s*)nullptr);
  }
  // find cell with same id
  auto& cells   = obj->meas_obj.meas_obj_eutra().cells_to_add_mod_list;
  auto  cell_it = std::find_if(cells.begin(), cells.end(), [pci](const cells_to_add_mod_s& c) { return c.pci == pci; });
  if (cell_it == cells.end()) {
    cell_it = nullptr;
  }
  return std::make_pair(obj, cell_it);
}

/// Add EARFCN to the MeasObjToAddModList
std::pair<bool, meas_obj_t*> add_meas_obj(meas_obj_list& list, uint32_t dl_earfcn, uint32_t allowed_meas_bw)
{
  meas_obj_t* obj = find_meas_obj(list, dl_earfcn);
  if (obj != nullptr) {
    return {false, obj};
  }

  meas_obj_t new_obj;
  new_obj.meas_obj_id                = srsran::find_rrc_obj_id_gap(list);
  asn1::rrc::meas_obj_eutra_s& eutra = new_obj.meas_obj.set_meas_obj_eutra();
  eutra.carrier_freq                 = dl_earfcn;
  asn1::number_to_enum(eutra.allowed_meas_bw, allowed_meas_bw);
  eutra.neigh_cell_cfg.from_number(1); // No MBSFN subframes present in neighbors
  eutra.offset_freq_present = false;   // no offset
  obj                       = srsran::add_rrc_obj(list, new_obj);
  return {true, obj};
}

/// Add cell parsed in configuration file to the varMeasCfg
std::tuple<bool, meas_obj_t*, cells_to_add_mod_s*> add_cell_enb_cfg(meas_obj_list&         meas_obj_list,
                                                                    const meas_cell_cfg_t& cellcfg)
{
  bool inserted_flag = true;

  cells_to_add_mod_s new_cell;
  new_cell.cell_individual_offset = cellcfg.cell_individual_offset;
  new_cell.pci                    = cellcfg.pci;

  std::pair<meas_obj_t*, meas_cell_t*> ret = find_cell(meas_obj_list, cellcfg.earfcn, cellcfg.pci);

  if (ret.first != nullptr) {
    // there are cells with the same earfcn at least.
    if (ret.second != nullptr) {
      // the cell already existed.
      if (ret.second->cell_individual_offset != new_cell.cell_individual_offset) {
        // members of cell were updated
        new_cell.cell_idx = ret.second->cell_idx;
        *ret.second       = new_cell;
      } else {
        inserted_flag = false;
      }
    } else {
      auto& eutra_obj = ret.first->meas_obj.meas_obj_eutra();
      // pci not found. create new cell
      new_cell.cell_idx                       = srsran::find_rrc_obj_id_gap(eutra_obj.cells_to_add_mod_list);
      ret.second                              = srsran::add_rrc_obj(eutra_obj.cells_to_add_mod_list, new_cell);
      eutra_obj.cells_to_add_mod_list_present = true;
    }
  } else {
    // no measobj has been found with same earfcn, create a new one
    auto ret2 = add_meas_obj(meas_obj_list, cellcfg.earfcn, cellcfg.allowed_meas_bw);
    ret.first = ret2.second;

    new_cell.cell_idx                   = 1;
    auto& eutra                         = ret2.second->meas_obj.meas_obj_eutra();
    eutra.cells_to_add_mod_list_present = true;
    eutra.cells_to_add_mod_list.push_back(new_cell);
    ret.second = &ret.first->meas_obj.meas_obj_eutra().cells_to_add_mod_list.back();
  }

  return std::make_tuple(inserted_flag, ret.first, ret.second);
}

/**
 * TS 36.331 - Section 5.5.2.5
 * Description: Adds MeasObjtoAddMod to MeasCfg object
 */
meas_obj_to_add_mod_s* meascfg_add_meas_obj(meas_cfg_s* meas_cfg, const meas_obj_to_add_mod_s& meas_obj)
{
  meas_cfg->meas_obj_to_add_mod_list_present = true;

  // search for meas_obj by obj_id to ensure uniqueness (assume sorted)
  auto meas_obj_it = srsran::add_rrc_obj_id(meas_cfg->meas_obj_to_add_mod_list, meas_obj.meas_obj_id);
  // TODO: Assert dl_earfcn is the same

  auto& target_eutra               = meas_obj_it->meas_obj.set_meas_obj_eutra();
  auto& src_eutra                  = meas_obj.meas_obj.meas_obj_eutra();
  target_eutra.carrier_freq        = src_eutra.carrier_freq;
  target_eutra.offset_freq_present = src_eutra.offset_freq_present;
  target_eutra.offset_freq         = src_eutra.offset_freq;
  target_eutra.allowed_meas_bw     = src_eutra.allowed_meas_bw;
  target_eutra.presence_ant_port1  = src_eutra.presence_ant_port1;
  target_eutra.neigh_cell_cfg      = src_eutra.neigh_cell_cfg;
  // do not add cellsToAddModList, blackCells, whiteCells, etc. according to (5.5.2.5 1|1|1)

  return meas_obj_it;
}

/// adds all the cells that got updated to MeasCfg.
void compute_diff_cells(const meas_obj_eutra_s& src_it,
                        const meas_obj_eutra_s& target_it,
                        meas_obj_to_add_mod_s*  added_obj)
{
  meas_obj_eutra_s* eutra_obj = &added_obj->meas_obj.meas_obj_eutra();
  srsran::compute_cfg_diff(src_it.cells_to_add_mod_list,
                           target_it.cells_to_add_mod_list,
                           eutra_obj->cells_to_add_mod_list,
                           eutra_obj->cells_to_rem_list);
  eutra_obj->cells_to_add_mod_list_present = eutra_obj->cells_to_add_mod_list.size() > 0;
  eutra_obj->cells_to_rem_list_present     = eutra_obj->cells_to_rem_list.size() > 0;
}

/**
 * Section 5.5.2.4/5, Measurement Object removal and addition/modification
 * Description: compute diff between target_cfg and var_meas -> depending on diff, add/remove/update meas_obj in
 * meas_cfg
 */
void compute_diff_meas_objs(const meas_cfg_s& prev_cfg, const meas_cfg_s& target_cfg, meas_cfg_s& meas_cfg)
{
  auto rem_func = [&meas_cfg](const meas_obj_t* it) { meas_cfg.meas_obj_to_rem_list.push_back(it->meas_obj_id); };
  auto add_func = [&meas_cfg](const meas_obj_t* it) { meas_cfg.meas_obj_to_add_mod_list.push_back(*it); };
  auto mod_func = [&meas_cfg](const meas_obj_t* src_it, const meas_obj_t* target_it) {
    if (*src_it != *target_it) {
      meas_obj_t* added_obj = meascfg_add_meas_obj(&meas_cfg, *target_it);
      // Add cells if there were changes.
      compute_diff_cells(src_it->meas_obj.meas_obj_eutra(), target_it->meas_obj.meas_obj_eutra(), added_obj);
    }
  };
  srsran::compute_cfg_diff(
      prev_cfg.meas_obj_to_add_mod_list, target_cfg.meas_obj_to_add_mod_list, rem_func, add_func, mod_func);
  meas_cfg.meas_obj_to_add_mod_list_present = meas_cfg.meas_obj_to_add_mod_list.size() > 0;
  meas_cfg.meas_obj_to_rem_list_present     = meas_cfg.meas_obj_to_rem_list.size() > 0;
  // TODO: black cells and white cells
}

/***********************************
 *       reportToAddModList
 **********************************/

report_cfg_to_add_mod_s* add_report_cfg(report_cfg_list& list, const report_cfg_eutra_s& reportcfg)
{
  report_cfg_to_add_mod_s new_rep;
  new_rep.report_cfg_id                     = srsran::find_rrc_obj_id_gap(list);
  new_rep.report_cfg.set_report_cfg_eutra() = reportcfg;

  return srsran::add_rrc_obj(list, new_rep);
}

/**
 * TS 36.331 - Section 5.5.2.6/7 - Reporting configuration removal and addition/modification
 */
void compute_diff_report_cfgs(const meas_cfg_s& src_cfg, const meas_cfg_s& target_cfg, meas_cfg_s& meas_cfg)
{
  srsran::compute_cfg_diff(src_cfg.report_cfg_to_add_mod_list,
                           target_cfg.report_cfg_to_add_mod_list,
                           meas_cfg.report_cfg_to_add_mod_list,
                           meas_cfg.report_cfg_to_rem_list);
  meas_cfg.report_cfg_to_add_mod_list_present = meas_cfg.report_cfg_to_add_mod_list.size() > 0;
  meas_cfg.report_cfg_to_rem_list_present     = meas_cfg.report_cfg_to_rem_list.size() > 0;
}

meas_id_to_add_mod_s* add_measid_cfg(meas_id_to_add_mod_list_l& meas_id_list, uint8_t measobjid, uint8_t measrepid)
{
  meas_id_to_add_mod_s new_measid;
  new_measid.report_cfg_id = measrepid;
  new_measid.meas_obj_id   = measobjid;
  new_measid.meas_id       = srsran::find_rrc_obj_id_gap(meas_id_list);
  return srsran::add_rrc_obj(meas_id_list, new_measid);
}

void compute_diff_meas_ids(const meas_cfg_s& src_cfg, const meas_cfg_s& target_cfg, meas_cfg_s& meas_cfg)
{
  srsran::compute_cfg_diff(src_cfg.meas_id_to_add_mod_list,
                           target_cfg.meas_id_to_add_mod_list,
                           meas_cfg.meas_id_to_add_mod_list,
                           meas_cfg.meas_id_to_rem_list);
  meas_cfg.meas_id_to_add_mod_list_present = meas_cfg.meas_id_to_add_mod_list.size() > 0;
  meas_cfg.meas_id_to_rem_list_present     = meas_cfg.meas_id_to_rem_list.size() > 0;
}

meas_gap_cfg_c make_measgap(const meas_obj_list& measobjs, const ue_cell_ded& pcell)
{
  meas_gap_cfg_c meas_gap;
  if (measobjs.size() == 1) {
    // only add measGaps if PCell is not the only frequency to measure
    return meas_gap;
  }
  switch (pcell.cell_common->cell_cfg.meas_cfg.meas_gap_period) {
    case 40:
      meas_gap.set_setup().gap_offset.set_gp0() = pcell.meas_gap_offset;
      break;
    case 80:
      meas_gap.set_setup().gap_offset.set_gp1() = pcell.meas_gap_offset;
      break;
    case 0: // no meas gaps configured
      break;
    default:
      srslog::fetch_basic_logger("RRC").error("Error setting measurement gap.");
  }
  return meas_gap;
}

bool apply_meas_gap_updates(const meas_gap_cfg_c& src_gaps,
                            const meas_gap_cfg_c& target_gaps,
                            meas_gap_cfg_c&       diff_gaps)
{
  if (src_gaps.type() != target_gaps.type()) {
    if (target_gaps.type().value == setup_opts::setup) {
      diff_gaps = target_gaps;
      return true;
    } else if (src_gaps.type().value == setup_opts::setup) {
      diff_gaps.set(setup_opts::release);
      return true;
    }
  } else if (target_gaps.type().value == setup_opts::setup) {
    const auto& target_offset = target_gaps.setup().gap_offset;
    const auto& src_offset    = src_gaps.setup().gap_offset;
    if (target_offset.type().value != src_offset.type().value) {
      diff_gaps = target_gaps;
      return true;
    } else {
      switch (target_offset.type().value) {
        case meas_gap_cfg_c::setup_s_::gap_offset_c_::types_opts::gp0:
          if (target_offset.gp0() != src_offset.gp0()) {
            diff_gaps = target_gaps;
            return true;
          }
          break;
        case meas_gap_cfg_c::setup_s_::gap_offset_c_::types_opts::gp1:
          if (target_offset.gp1() != src_offset.gp1()) {
            diff_gaps = target_gaps;
            return true;
          }
          break;
        default:
          srslog::fetch_basic_logger("RRC").warning("MeasGap of type %s not supported",
                                                    target_offset.type().to_string());
      }
    }
  }
  return false;
}

/***********************************
 *          measConfig
 **********************************/

bool set_meascfg_presence_flags(meas_cfg_s& meascfg)
{
  meascfg.meas_obj_to_add_mod_list_present   = meascfg.meas_obj_to_add_mod_list.size() > 0;
  meascfg.meas_obj_to_rem_list_present       = meascfg.meas_obj_to_rem_list.size() > 0;
  meascfg.report_cfg_to_add_mod_list_present = meascfg.report_cfg_to_add_mod_list.size() > 0;
  meascfg.report_cfg_to_rem_list_present     = meascfg.report_cfg_to_rem_list.size() > 0;
  meascfg.meas_id_to_add_mod_list_present    = meascfg.meas_id_to_add_mod_list.size() > 0;
  meascfg.meas_id_to_rem_list_present        = meascfg.meas_id_to_rem_list.size() > 0;
  meascfg.quant_cfg_present                  = meascfg.quant_cfg.quant_cfg_eutra_present;
  meascfg.meas_gap_cfg_present               = meascfg.meas_gap_cfg.type().value != setup_opts::nulltype;

  return meascfg.meas_obj_to_add_mod_list_present or meascfg.meas_obj_to_rem_list_present or
         meascfg.report_cfg_to_add_mod_list_present or meascfg.report_cfg_to_rem_list_present or
         meascfg.meas_id_to_add_mod_list_present or meascfg.meas_id_to_rem_list_present or meascfg.quant_cfg_present or
         meascfg.meas_gap_cfg_present;
}

bool fill_meascfg_enb_cfg(meas_cfg_s& meascfg, const ue_cell_ded_list& ue_cell_list)
{
  const ue_cell_ded* pcell = ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  assert(pcell != nullptr);
  const enb_cell_common* pcell_cfg     = pcell->cell_common;
  const auto&            pcell_meascfg = pcell_cfg->cell_cfg.meas_cfg;

  // Add PCell+Scells to measObjToAddModList
  // NOTE: sort by EARFCN to avoid unnecessary reconfigurations of measObjToAddModList
  std::vector<const ue_cell_ded*> sorted_ue_cells(ue_cell_list.nof_cells());
  for (uint32_t ue_cc_idx = 0; ue_cc_idx < ue_cell_list.nof_cells(); ++ue_cc_idx) {
    sorted_ue_cells[ue_cc_idx] = ue_cell_list.get_ue_cc_idx(ue_cc_idx);
  }
  std::sort(sorted_ue_cells.begin(), sorted_ue_cells.end(), [](const ue_cell_ded* cc1, const ue_cell_ded* cc2) {
    return cc1->get_dl_earfcn() < cc2->get_dl_earfcn();
  });
  for (auto* cc : sorted_ue_cells) {
    add_meas_obj(
        meascfg.meas_obj_to_add_mod_list, cc->get_dl_earfcn(), cc->cell_common->cell_cfg.meas_cfg.allowed_meas_bw);
  }

  // Inserts all cells in meas_cell_list that are not PCell or SCells
  for (const meas_cell_cfg_t& meascell : pcell_meascfg.meas_cells) {
    if (ue_cell_list.find_cell(meascell.earfcn, meascell.pci) == nullptr) {
      add_cell_enb_cfg(meascfg.meas_obj_to_add_mod_list, meascell);
    }
  }

  // Insert report cfg of PCell
  for (const report_cfg_eutra_s& reportcfg : pcell_meascfg.meas_reports) {
    add_report_cfg(meascfg.report_cfg_to_add_mod_list, reportcfg);
  }

  // Insert quantity config
  meascfg.quant_cfg.quant_cfg_eutra_present = true;
  meascfg.quant_cfg.quant_cfg_eutra         = pcell_meascfg.quant_cfg;

  // Insert all measIds
  // TODO: add this to the parser. Now we combine all reports with all objects
  if (meascfg.report_cfg_to_add_mod_list.size() > 0) {
    for (const auto& measobj : meascfg.meas_obj_to_add_mod_list) {
      for (const auto& measrep : meascfg.report_cfg_to_add_mod_list) {
        add_measid_cfg(meascfg.meas_id_to_add_mod_list, measobj.meas_obj_id, measrep.report_cfg_id);
      }
    }
  }

  // Set MeasGap
  meascfg.meas_gap_cfg = make_measgap(meascfg.meas_obj_to_add_mod_list, *pcell);

  return set_meascfg_presence_flags(meascfg);
}

bool compute_diff_meascfg(const meas_cfg_s& current_meascfg, const meas_cfg_s& target_meascfg, meas_cfg_s& diff_meascfg)
{
  diff_meascfg = {};
  compute_diff_meas_objs(current_meascfg, target_meascfg, diff_meascfg);
  compute_diff_report_cfgs(current_meascfg, target_meascfg, diff_meascfg);
  compute_diff_meas_ids(current_meascfg, target_meascfg, diff_meascfg);
  if (target_meascfg.quant_cfg_present != current_meascfg.quant_cfg_present or
      (target_meascfg.quant_cfg_present and target_meascfg.quant_cfg != current_meascfg.quant_cfg)) {
    diff_meascfg.quant_cfg = target_meascfg.quant_cfg;
  }

  // Only update measGap if it was not set before or periodicity changed
  if (current_meascfg.meas_gap_cfg.type().value == setup_opts::setup) {
    if (target_meascfg.meas_gap_cfg.type().value != setup_opts::setup) {
      diff_meascfg.meas_gap_cfg.set(setup_opts::release);
    } else if (target_meascfg.meas_gap_cfg.setup().gap_offset.type() !=
               current_meascfg.meas_gap_cfg.setup().gap_offset.type()) {
      diff_meascfg.meas_gap_cfg = target_meascfg.meas_gap_cfg;
    }
  } else {
    diff_meascfg.meas_gap_cfg = target_meascfg.meas_gap_cfg;
  }

  return set_meascfg_presence_flags(diff_meascfg);
}

bool apply_meascfg_updates(meas_cfg_s&             meascfg,
                           meas_cfg_s&             current_meascfg,
                           const ue_cell_ded_list& ue_cell_list,
                           int                     prev_earfcn,
                           int                     prev_pci)
{
  meascfg = {};

  const ue_cell_ded* pcell         = ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  uint32_t           target_earfcn = pcell->get_dl_earfcn();

  if (static_cast<uint32_t>(prev_pci) == pcell->get_pci() and static_cast<uint32_t>(prev_earfcn) == target_earfcn) {
    // Shortcut: No PCell change -> no measConfig updates
    return false;
  }

  // Apply TS 36.331 5.5.6.1 - If Source and Target eNB EARFCNs do no match, update SourceMeasCfg.MeasIdList
  if ((uint32_t)prev_earfcn != target_earfcn) {
    meas_obj_t* found_target_obj = find_meas_obj(current_meascfg.meas_obj_to_add_mod_list, target_earfcn);
    meas_obj_t* found_src_obj    = prev_earfcn != 0 ? &current_meascfg.meas_obj_to_add_mod_list[0] : nullptr;
    if (found_target_obj != nullptr and found_src_obj != nullptr) {
      for (auto& mid : current_meascfg.meas_id_to_add_mod_list) {
        if (found_target_obj->meas_obj_id == mid.meas_obj_id) {
          mid.meas_obj_id = found_src_obj->meas_obj_id;
        } else if (found_src_obj->meas_obj_id == mid.meas_obj_id) {
          mid.meas_obj_id = found_target_obj->meas_obj_id;
        }
      }
    } else if (found_src_obj != nullptr) {
      for (auto it = current_meascfg.meas_id_to_add_mod_list.begin();
           it != current_meascfg.meas_id_to_add_mod_list.end();) {
        if (it->meas_obj_id == found_src_obj->meas_obj_id) {
          it = current_meascfg.meas_id_to_add_mod_list.erase(it);
        } else {
          ++it;
        }
      }
    }
  }

  // Generate final measConfig
  meas_cfg_s target_meascfg;
  fill_meascfg_enb_cfg(target_meascfg, ue_cell_list);

  // Set a MeasConfig in the RRC Connection Reconfiguration for HO.
  compute_diff_meas_objs(current_meascfg, target_meascfg, meascfg);
  compute_diff_report_cfgs(current_meascfg, target_meascfg, meascfg);
  compute_diff_meas_ids(current_meascfg, target_meascfg, meascfg);
  if (target_meascfg.quant_cfg_present != current_meascfg.quant_cfg_present or
      (target_meascfg.quant_cfg_present and target_meascfg.quant_cfg != current_meascfg.quant_cfg)) {
    meascfg.quant_cfg = target_meascfg.quant_cfg;
  }

  // Set measGaps if changed
  meascfg.meas_gap_cfg_present =
      apply_meas_gap_updates(current_meascfg.meas_gap_cfg, target_meascfg.meas_gap_cfg, meascfg.meas_gap_cfg);

  // Update current measconfig
  bool ret        = set_meascfg_presence_flags(meascfg);
  current_meascfg = target_meascfg;

  return ret;
}

} // namespace srsenb
