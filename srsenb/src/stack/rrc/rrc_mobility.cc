/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/rrc/rrc_mobility.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/common.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <functional>

namespace srsenb {

#define Info(fmt, ...) rrc_log->info("Mobility: " fmt, ##__VA_ARGS__)

using namespace asn1::rrc;

namespace rrc_details {

//! extract cell id from ECI
uint32_t eci_to_cellid(uint32_t eci)
{
  return eci & 0xFFu;
}

//! cell comparison based on content
bool cells_are_equal(const cells_to_add_mod_s& lhs, const cells_to_add_mod_s& rhs)
{
  return lhs.cell_idx == rhs.cell_idx and lhs.pci == rhs.pci and
         lhs.cell_individual_offset == rhs.cell_individual_offset;
}

//! meas field comparison based on ID solely
template <typename T, typename IdType, IdType T::*field>
struct field_id_cmp {
  bool operator()(const T& lhs, const T& rhs) const { return lhs.*field < rhs.*field; }
  bool operator()(const T& lhs, IdType id) const { return lhs.*field < id; }
};
using cell_id_cmp     = field_id_cmp<cells_to_add_mod_s, uint8_t, &cells_to_add_mod_s::cell_idx>;
using meas_obj_id_cmp = field_id_cmp<meas_obj_to_add_mod_s, uint8_t, &meas_obj_to_add_mod_s::meas_obj_id>;

//! Find MeasObj with same earfcn
meas_obj_to_add_mod_s* find_meas_obj(meas_obj_to_add_mod_list_l& l, uint32_t earfcn)
{
  auto same_earfcn = [earfcn](const meas_obj_to_add_mod_s& obj) {
    return obj.meas_obj.type().value == meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra and
           obj.meas_obj.meas_obj_eutra().carrier_freq == earfcn;
  };
  auto it = std::find_if(l.begin(), l.end(), same_earfcn);
  if (it == l.end()) {
    return nullptr;
  }
  return it;
}

/** Finds a cell in this->objects based on cell_id and frequency
 *  return pair of (meas_obj,cell_obj). If no cell has frequency==earfcn, meas_obj=nullptr
 */
std::pair<meas_obj_to_add_mod_s*, cells_to_add_mod_s*>
find_cell(meas_obj_to_add_mod_list_l& l, uint32_t earfcn, uint8_t cell_id)
{
  // find meas_obj with same earfcn
  meas_obj_to_add_mod_s* obj = rrc_details::find_meas_obj(l, earfcn);
  if (obj == nullptr) {
    return {nullptr, nullptr};
  }
  // find cell with same id
  auto& cells = obj->meas_obj.meas_obj_eutra().cells_to_add_mod_list;
  auto  it    = std::lower_bound(cells.begin(), cells.end(), cell_id, rrc_details::cell_id_cmp{});
  if (it == cells.end() or it->cell_idx != cell_id) {
    return {obj, nullptr};
  }
  return {obj, it};
}

//! Adds Cell to MeasCfg MeasObjToAddMod field
cells_to_add_mod_s* meascfg_add_cell(meas_obj_eutra_s& eutra_obj, const cells_to_add_mod_s& celltoadd)
{
  // create new cell_id in the provided eutra_obj.
  // if the cell_id already exists, just update the fields.

  // find the cell.
  auto& l        = eutra_obj.cells_to_add_mod_list;
  auto  found_it = std::lower_bound(l.begin(), l.end(), celltoadd.cell_idx, rrc_details::cell_id_cmp{});
  if (found_it == l.end()) {
    eutra_obj.cells_to_add_mod_list.push_back({});
    found_it = &eutra_obj.cells_to_add_mod_list.back();
  }

  *found_it = celltoadd;
  //  cell_obj->cell_idx = (uint8_t)(cell_id % 3); // 0-3 // FIXME: What?
  //  printf("The added cell has idx=%d, pci=%d\n", cell_obj->cell_idx, cell_obj->pci);

  return found_it;
}

/**
 * Adds MeasObjtoAddMod to MeasCfg object
 */
meas_obj_to_add_mod_s*
meascfg_add_meas_obj(meas_cfg_s* meas_cfg, const meas_obj_to_add_mod_s& meas_obj, bool add_cells_flag)
{
  meas_cfg->meas_obj_to_add_mod_list_present = true;
  meas_obj_to_add_mod_list_l& l              = meas_cfg->meas_obj_to_add_mod_list;

  // search for meas_obj by obj_id to ensure uniqueness (assume sorted)
  auto found_it = std::lower_bound(l.begin(), l.end(), meas_obj.meas_obj_id, meas_obj_id_cmp{});
  // TODO: Assert dl_earfcn is the same
  if (found_it == l.end()) {
    l.push_back({});
    found_it              = &l.back();
    found_it->meas_obj_id = meas_obj.meas_obj_id;
  }

  auto& target_eutra               = found_it->meas_obj.set_meas_obj_eutra();
  auto& src_eutra                  = meas_obj.meas_obj.meas_obj_eutra();
  target_eutra.carrier_freq        = src_eutra.carrier_freq;
  target_eutra.offset_freq_present = src_eutra.offset_freq_present;
  target_eutra.offset_freq         = src_eutra.offset_freq;
  target_eutra.allowed_meas_bw     = src_eutra.allowed_meas_bw;
  target_eutra.presence_ant_port1  = src_eutra.presence_ant_port1;
  target_eutra.neigh_cell_cfg      = src_eutra.neigh_cell_cfg;

  if (add_cells_flag) {
    for (const cells_to_add_mod_s& cell_it : src_eutra.cells_to_add_mod_list) {
      rrc_details::meascfg_add_cell(target_eutra, cell_it);
    }
  }

  return found_it;
}

//! Find difference between MeasObjs
bool meas_objs_are_equal(const meas_obj_to_add_mod_s& lhs, const meas_obj_to_add_mod_s& rhs)
{
  if (lhs.meas_obj_id != rhs.meas_obj_id or lhs.meas_obj.type() != lhs.meas_obj.type()) {
    return false;
  }
  auto &lhs_eutra = lhs.meas_obj.meas_obj_eutra(), &rhs_eutra = rhs.meas_obj.meas_obj_eutra();
  if (lhs_eutra.ext or rhs_eutra.ext) {
    printf("[%d] extension of measObjToAddMod not supported\n", __LINE__);
    return false;
  }

  if (lhs_eutra.offset_freq_present != rhs_eutra.offset_freq_present or
      (lhs_eutra.offset_freq_present and lhs_eutra.offset_freq != rhs_eutra.offset_freq)) {
    return false;
  }

  if (lhs_eutra.carrier_freq != rhs_eutra.carrier_freq or not(lhs_eutra.neigh_cell_cfg == rhs_eutra.neigh_cell_cfg) or
      lhs_eutra.presence_ant_port1 != rhs_eutra.presence_ant_port1 or
      lhs_eutra.allowed_meas_bw != rhs_eutra.allowed_meas_bw) {
    return false;
  }

  if (lhs_eutra.cells_to_add_mod_list.size() != rhs_eutra.cells_to_add_mod_list.size()) {
    return false;
  }

  return std::equal(lhs_eutra.cells_to_add_mod_list.begin(),
                    lhs_eutra.cells_to_add_mod_list.end(),
                    rhs_eutra.cells_to_add_mod_list.begin(),
                    cells_are_equal);
}

} // namespace rrc_details

/*************************************************************************************************
 *                                  var_meas_cfg_t class
 ************************************************************************************************/

//! Add cell parsed in configuration file to the varMeasCfg
std::tuple<bool, var_meas_cfg_t::meas_obj_t*, var_meas_cfg_t::meas_cell_t*>
var_meas_cfg_t::add_cell_cfg(const meas_cell_cfg_t& cellcfg)
{
  using namespace rrc_details;
  bool inserted_flag = true;

  // FIXME: cellcfg.cell_id is the ECI
  uint32_t         cell_id = rrc_details::eci_to_cellid(cellcfg.cell_id);
  q_offset_range_e offset;
  asn1::number_to_enum(offset, (int8_t)cellcfg.q_offset); // FIXME: What's the difference

  std::pair<meas_obj_t*, meas_cell_t*> ret = rrc_details::find_cell(var_meas.meas_obj_list, cellcfg.earfcn, cell_id);
  cells_to_add_mod_s                   new_cell;
  new_cell.cell_idx               = cell_id;
  new_cell.cell_individual_offset = offset;
  new_cell.pci                    = cellcfg.pci;

  if (ret.first != nullptr) {
    // there are cells with the same earfcn at least.
    if (ret.second != nullptr) {
      // the cell already existed.
      if (ret.second->pci != cellcfg.pci or ret.second->cell_individual_offset != offset) {
        // members of cell were updated
        *ret.second = new_cell;
      } else {
        inserted_flag = false;
      }
    } else {
      // cell_id not found. create new cell
      auto& cell_list = ret.first->meas_obj.meas_obj_eutra().cells_to_add_mod_list;
      cell_list.push_back(new_cell);
      std::sort(cell_list.begin(), cell_list.end(), rrc_details::cell_id_cmp{});
      // find cell in new position
      ret.second = std::lower_bound(cell_list.begin(), cell_list.end(), new_cell.cell_idx, rrc_details::cell_id_cmp{});
    }
  } else {
    // no measobj has been found with same earfcn, create a new one
    meas_obj_t new_obj;
    new_obj.meas_obj_id                = get_new_obj_id();
    asn1::rrc::meas_obj_eutra_s& eutra = new_obj.meas_obj.set_meas_obj_eutra();
    eutra.carrier_freq                 = cellcfg.earfcn;
    eutra.allowed_meas_bw.value        = asn1::rrc::allowed_meas_bw_e::mbw6; // FIXME: What value to add here?
    eutra.neigh_cell_cfg.from_number(1);                                     // FIXME: What value?
    eutra.offset_freq_present = true;
    // TODO: Assert that q_offset is in ms
    asn1::number_to_enum(eutra.offset_freq, cellcfg.q_offset);
    eutra.cells_to_add_mod_list_present = true;
    eutra.cells_to_add_mod_list.push_back(new_cell);
    var_meas.meas_obj_list.push_back(new_obj);
    std::sort(var_meas.meas_obj_list.begin(), var_meas.meas_obj_list.end(), rrc_details::meas_obj_id_cmp{});
    // get measObj in new position
    ret.first  = std::lower_bound(var_meas.meas_obj_list.begin(),
                                 var_meas.meas_obj_list.end(),
                                 new_obj.meas_obj_id,
                                 rrc_details::meas_obj_id_cmp{});
    ret.second = &ret.first->meas_obj.meas_obj_eutra().cells_to_add_mod_list.back();
  }

  return {inserted_flag, ret.first, ret.second};
}

//! Find first gap in meas_obj_id and return it
uint32_t var_meas_cfg_t::get_new_obj_id()
{
  meas_obj_t* prev_it = var_meas.meas_obj_list.begin();
  if (prev_it != var_meas.meas_obj_list.end() and prev_it->meas_obj_id == 1) {
    meas_obj_t* it = prev_it;
    for (++it; it != var_meas.meas_obj_list.end(); prev_it = it, ++it) {
      if (it->meas_obj_id > prev_it->meas_obj_id + 1) {
        break;
      }
    }
  }
  return (prev_it == var_meas.meas_obj_list.end()) ? 1 : prev_it->meas_obj_id + 1; // starts at 1.
}

void var_meas_cfg_t::compute_diff_meas_cfg(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg)
{
  // TODO: Create a flag to disable changing the "this" members (useful for transparent container)
  // Set a MeasConfig in the RRC Connection Reconfiguration for HO.
  compute_diff_meas_objs(target_cfg, meas_cfg);
  //  deltaconfig_meas_reports(target_cfg, meas_cfg);
  //  deltaconfig_meas_ids(target_cfg, meas_cfg);
  //  deltaconfig_meas_quantity_config(target_cfg, meas_cfg);
  meas_cfg->meas_gap_cfg_present         = false; // NOTE: we do not support inter-freq. HO
  meas_cfg->s_measure_present            = false; // NOTE: We do not support SCells
  meas_cfg->pre_regist_info_hrpd_present = false; // NOTE: not supported
  meas_cfg->speed_state_pars_present     = false; // NOTE: not supported
}

//! adds all the cells that got updated to MeasCfg.
void var_meas_cfg_t::compute_diff_cells(const meas_obj_eutra_s& target_it,
                                        meas_obj_eutra_s&       src_it,
                                        meas_obj_to_add_mod_s*  added_obj)
{
  cells_to_add_mod_s*       src_cell    = src_it.cells_to_add_mod_list.begin();
  const cells_to_add_mod_s* target_cell = target_it.cells_to_add_mod_list.begin();
  bool                      src_left    = src_cell != src_it.cells_to_add_mod_list.end();
  bool                      target_left = target_cell != target_it.cells_to_add_mod_list.end();
  while (src_left or target_left) {
    if (not target_left or (src_left and src_cell->cell_idx < target_cell->cell_idx)) {
      // a cell was removed from the eNB
      // TODO: add cell to remove list
      ++src_cell;
    } else if (not src_left or (target_left and src_cell->cell_idx > target_cell->cell_idx)) {
      // a cell was added to the eNB
      Info("UE has now to measure activity of (earfcn,cell_id)=(%d,%d).\n", target_cell->cell_idx, target_cell->pci);
      rrc_details::meascfg_add_cell(added_obj->meas_obj.meas_obj_eutra(), *target_cell);
    } else {
      // check if cells are the same. if not, update.
      if (not rrc_details::cells_are_equal(*src_cell, *target_cell)) {
        Info("UE has now to measure activity of (earfcn,cell_id)=(%d,%d) with updated params.\n",
             target_cell->cell_idx,
             target_cell->pci);
        rrc_details::meascfg_add_cell(added_obj->meas_obj.meas_obj_eutra(), *target_cell);
        *src_cell = *target_cell;
      }
      ++src_cell;
      ++target_cell;
    }
    src_left    = src_cell != src_it.cells_to_add_mod_list.end();
    target_left = target_cell != target_it.cells_to_add_mod_list.end();
  }
}

//! compute diff between target_cfg and var_meas -> depending on diff, add/remove/update meas_obj in meas_cfg
void var_meas_cfg_t::compute_diff_meas_objs(const var_meas_cfg_t& target_cfg, meas_cfg_s* meas_cfg)
{
  // TODO: black cells and white cells

  meas_obj_t *      ue_it = var_meas.meas_obj_list.begin(), *ue_end = var_meas.meas_obj_list.end();
  const meas_obj_t *target_it  = target_cfg.var_meas.meas_obj_list.begin(),
                   *target_end = target_cfg.var_meas.meas_obj_list.end();

  bool ues_left  = ue_it != ue_end;
  bool enbs_left = target_it != target_end;
  while (ues_left or enbs_left) {
    if (not enbs_left or (ues_left and ue_it->meas_obj_id < target_it->meas_obj_id)) {
      // an object has been removed from the target_var_meas
      Info("UE can cease to measure activity in frequency earfcn=%d.\n", ue_it->meas_obj.meas_obj_eutra().carrier_freq);
      // TODO: add to remove list
      ++ue_it;
    } else if (!ues_left or (enbs_left and ue_it->meas_obj_id > target_it->meas_obj_id)) {
      // a new object has been added to enb_var_meas
      Info("HO: UE has now to measure activity of new frequency earfcn=%d.\n",
           target_it->meas_obj.meas_obj_eutra().carrier_freq);
      rrc_details::meascfg_add_meas_obj(meas_cfg, *target_it, true);
      ++target_it;
    } else {
      bool are_equal = rrc_details::meas_objs_are_equal(*ue_it, *target_it);
      if (not are_equal) {
        // if we found a difference in obj IDs
        meas_obj_to_add_mod_s* added_obj = rrc_details::meascfg_add_meas_obj(meas_cfg, *target_it, false);
        // Add cells/meas_obj if there were changes.
        compute_diff_cells(target_it->meas_obj.meas_obj_eutra(), ue_it->meas_obj.meas_obj_eutra(), added_obj);
      }
      ++ue_it;
      ++target_it;
    }
    ues_left  = ue_it != ue_end;
    enbs_left = target_it != target_end;
  }
}

/*************************************************************************************************
 *                                  mobility_cfg class
 ************************************************************************************************/

rrc::mobility_cfg::mobility_cfg(rrc* outer_rrc) : rrc_enb(outer_rrc), current_meas_cfg(outer_rrc->rrc_log)
{
  // inserts all neighbor cells
  if (rrc_enb->cfg.meas_cfg_present) {
    for (meas_cell_cfg_t& meascell : rrc_enb->cfg.meas_cfg.meas_cells) {
      current_meas_cfg.add_cell_cfg(meascell);
    }
  }
}

/*************************************************************************************************
 *                                  rrc_mobility class
 ************************************************************************************************/

rrc::ue::rrc_mobility::rrc_mobility(rrc::ue* outer_ue) :
  rrc_ue(outer_ue),
  rrc_enb(outer_ue->parent),
  cfg(outer_ue->parent->enb_mobility_cfg.get()),
  pool(outer_ue->pool),
  rrc_log(outer_ue->parent->rrc_log),
  ue_var_meas(outer_ue->parent->rrc_log)
{
}

bool rrc::ue::rrc_mobility::fill_conn_recfg_msg(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_recfg)
{
  // only reconfigure meas_cfg if no handover is occurring
  if (mobility_proc.is_busy()) {
    return false;
  }

  asn1::rrc::meas_cfg_s* meas_cfg = &conn_recfg->meas_cfg;
  ue_var_meas.compute_diff_meas_cfg(cfg->current_meas_cfg, meas_cfg);

  // if there is at least one difference, we tag a new measurement report in conn_reconf.
  bool diff = meas_cfg->meas_obj_to_add_mod_list_present;
  diff |= meas_cfg->meas_obj_to_rem_list_present;
  diff |= meas_cfg->report_cfg_to_add_mod_list_present;
  diff |= meas_cfg->report_cfg_to_rem_list_present;
  diff |= meas_cfg->meas_id_to_add_mod_list_present;
  diff |= meas_cfg->meas_id_to_rem_list_present;
  diff |= meas_cfg->quant_cfg_present;
  diff |= meas_cfg->meas_gap_cfg_present;
  diff |= meas_cfg->s_measure_present;
  diff |= meas_cfg->pre_regist_info_hrpd_present;
  diff |= meas_cfg->speed_state_pars_present;
  if (diff) {
    conn_recfg->meas_cfg_present = true;
    return true;
  }
  return false;
}

} // namespace srsenb
