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

#include "srsenb/hdr/stack/rrc/rrc_mobility.h"
#include "srsenb/hdr/stack/rrc/mac_controller.h"
#include "srsenb/hdr/stack/rrc/rrc_cell_cfg.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/common.h"
#include "srslte/common/int_helpers.h"
#include "srslte/rrc/rrc_cfg_utils.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <functional>

namespace srsenb {

#define Info(fmt, ...) rrc_log->info("Mobility: " fmt, ##__VA_ARGS__)
#define Error(fmt, ...) rrc_log->error("Mobility: " fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) rrc_log->warning("Mobility: " fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) rrc_log->debug("Mobility: " fmt, ##__VA_ARGS__)

#define procInfo(fmt, ...) parent->rrc_log->info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procWarning(fmt, ...) parent->rrc_log->warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procError(fmt, ...) parent->rrc_log->error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

using namespace asn1::rrc;

/*************************************************************************************************
 *         Convenience Functions to handle ASN1 MeasObjs/MeasId/ReportCfg/Cells/etc.
 ************************************************************************************************/

namespace rrc_details {

//! extract cell id from ECI
uint32_t eci_to_cellid(uint32_t eci)
{
  return eci & 0xFFu;
}
//! extract enb id from ECI
uint32_t eci_to_enbid(uint32_t eci)
{
  return (eci - eci_to_cellid(eci)) >> 8u;
}
uint16_t compute_mac_i(uint16_t                            crnti,
                       uint32_t                            cellid,
                       uint16_t                            pci,
                       srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo,
                       const uint8_t*                      k_rrc_int)
{
  // Compute shortMAC-I
  uint8_t varShortMAC_packed[16] = {};
  uint8_t mac_key[4]             = {};

  // ASN.1 encode VarShortMAC-Input
  asn1::rrc::var_short_mac_input_s var_short_mac;
  var_short_mac.cell_id.from_number(cellid);
  var_short_mac.pci = pci;
  var_short_mac.c_rnti.from_number(crnti);

  asn1::bit_ref bref(varShortMAC_packed, sizeof(varShortMAC_packed));
  if (var_short_mac.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) { // already zeroed, so no need to align
    printf("Error packing varShortMAC\n");
  }
  uint32_t N_bytes = bref.distance_bytes();

  printf("Encoded varShortMAC: cellId=0x%x, PCI=%d, rnti=0x%x (%d bytes)\n", cellid, pci, crnti, N_bytes);

  // Compute MAC-I
  switch (integ_algo) {
    case srslte::INTEGRITY_ALGORITHM_ID_128_EIA1:
      srslte::security_128_eia1(&k_rrc_int[16],
                                0xffffffff, // 32-bit all to ones
                                0x1f,       // 5-bit all to ones
                                1,          // 1-bit to one
                                varShortMAC_packed,
                                N_bytes,
                                mac_key);
      break;
    case srslte::INTEGRITY_ALGORITHM_ID_128_EIA2:
      srslte::security_128_eia2(&k_rrc_int[16],
                                0xffffffff, // 32-bit all to ones
                                0x1f,       // 5-bit all to ones
                                1,          // 1-bit to one
                                varShortMAC_packed,
                                N_bytes,
                                mac_key);
      break;
    default:
      printf("Unsupported integrity algorithm %d.\n", integ_algo);
  }

  uint16_t short_mac_i = (((uint16_t)mac_key[2] << 8u) | (uint16_t)mac_key[3]);
  return short_mac_i;
}

//! convenience function overload to print MeasObj/MeasId/etc. fields
std::string to_string(const cells_to_add_mod_s& obj)
{
  char buf[128];
  std::snprintf(
      buf, 128, "{cell_idx: %d, pci: %d, offset: %d}", obj.cell_idx, obj.pci, obj.cell_individual_offset.to_number());
  return {buf};
}

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

/** Finds a cell in this->objects based on pci and earfcn
 *  return pair of (meas_obj,cell_obj). If no cell has frequency==earfcn, meas_obj=nullptr
 */
std::pair<meas_obj_to_add_mod_s*, cells_to_add_mod_s*>
find_cell(meas_obj_to_add_mod_list_l& l, uint32_t earfcn, uint32_t pci)
{
  // find meas_obj with same earfcn
  meas_obj_to_add_mod_s* obj = rrc_details::find_meas_obj(l, earfcn);
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

/**
 * Section 5.5.2.5
 * Description: Adds MeasObjtoAddMod to MeasCfg object
 */
meas_obj_to_add_mod_s* meascfg_add_meas_obj(meas_cfg_s* meas_cfg, const meas_obj_to_add_mod_s& meas_obj)
{
  meas_cfg->meas_obj_to_add_mod_list_present = true;

  // search for meas_obj by obj_id to ensure uniqueness (assume sorted)
  auto meas_obj_it = srslte::add_rrc_obj_id(meas_cfg->meas_obj_to_add_mod_list, meas_obj.meas_obj_id);
  // TODO: Assert dl_earfcn is the same

  auto& target_eutra               = meas_obj_it->meas_obj.set_meas_obj_eutra();
  auto& src_eutra                  = meas_obj.meas_obj.meas_obj_eutra();
  target_eutra.carrier_freq        = src_eutra.carrier_freq;
  target_eutra.offset_freq_present = src_eutra.offset_freq_present;
  target_eutra.offset_freq         = src_eutra.offset_freq;
  target_eutra.allowed_meas_bw     = src_eutra.allowed_meas_bw;
  target_eutra.presence_ant_port1  = src_eutra.presence_ant_port1;
  target_eutra.neigh_cell_cfg      = src_eutra.neigh_cell_cfg;
  // do not add cellsToAddModList, blacCells, whiteCells, etc. according to (5.5.2.5 1|1|1)

  return meas_obj_it;
}

} // namespace rrc_details

/*************************************************************************************************
 *                                  var_meas_cfg_t class
 ************************************************************************************************/

var_meas_cfg_t::var_meas_cfg_t() : rrc_log(srslte::logmap::get("RRC")) {}

//! Add EARFCN to the varMeasCfg
std::pair<bool, var_meas_cfg_t::meas_obj_t*> var_meas_cfg_t::add_meas_obj(uint32_t dl_earfcn)
{
  auto* obj = rrc_details::find_meas_obj(var_meas.meas_obj_list, dl_earfcn);
  if (obj != nullptr) {
    return {false, obj};
  }

  meas_obj_t new_obj;
  new_obj.meas_obj_id                = srslte::find_rrc_obj_id_gap(var_meas.meas_obj_list);
  asn1::rrc::meas_obj_eutra_s& eutra = new_obj.meas_obj.set_meas_obj_eutra();
  eutra.carrier_freq                 = dl_earfcn;
  eutra.allowed_meas_bw.value        = asn1::rrc::allowed_meas_bw_e::mbw6; // TODO: What value to add here?
  eutra.neigh_cell_cfg.from_number(1);                                     // No MBSFN subframes present in neighbors
  eutra.offset_freq_present      = false;                                  // no offset
  obj                            = srslte::add_rrc_obj(var_meas.meas_obj_list, new_obj);
  var_meas.meas_obj_list_present = true;
  return {true, obj};
}

//! Add cell parsed in configuration file to the varMeasCfg
std::tuple<bool, var_meas_cfg_t::meas_obj_t*, var_meas_cfg_t::meas_cell_t*>
var_meas_cfg_t::add_cell_cfg(const meas_cell_cfg_t& cellcfg)
{
  using namespace rrc_details;
  bool inserted_flag = true;

  q_offset_range_e offset;
  asn1::number_to_enum(offset, (int8_t)cellcfg.q_offset);

  std::pair<meas_obj_t*, meas_cell_t*> ret =
      rrc_details::find_cell(var_meas.meas_obj_list, cellcfg.earfcn, cellcfg.pci);
  cells_to_add_mod_s new_cell;
  new_cell.cell_individual_offset = offset;
  new_cell.pci                    = cellcfg.pci;

  if (ret.first != nullptr) {
    // there are cells with the same earfcn at least.
    if (ret.second != nullptr) {
      // the cell already existed.
      if (ret.second->cell_individual_offset != offset) {
        // members of cell were updated
        new_cell.cell_idx = ret.second->cell_idx;
        *ret.second       = new_cell;
      } else {
        inserted_flag = false;
      }
    } else {
      auto& eutra_obj = ret.first->meas_obj.meas_obj_eutra();
      // pci not found. create new cell
      new_cell.cell_idx                       = srslte::find_rrc_obj_id_gap(eutra_obj.cells_to_add_mod_list);
      ret.second                              = srslte::add_rrc_obj(eutra_obj.cells_to_add_mod_list, new_cell);
      eutra_obj.cells_to_add_mod_list_present = true;
    }
  } else {
    // no measobj has been found with same earfcn, create a new one
    auto ret2 = add_meas_obj(cellcfg.earfcn);
    ret.first = ret2.second;

    new_cell.cell_idx                   = 1;
    auto& eutra                         = ret2.second->meas_obj.meas_obj_eutra();
    eutra.cells_to_add_mod_list_present = true;
    eutra.cells_to_add_mod_list.push_back(new_cell);
    ret.second = &ret.first->meas_obj.meas_obj_eutra().cells_to_add_mod_list.back();
  }

  if (inserted_flag) {
    var_meas.meas_obj_list_present = true;
  }

  return std::make_tuple(inserted_flag, ret.first, ret.second);
}

report_cfg_to_add_mod_s* var_meas_cfg_t::add_report_cfg(const report_cfg_eutra_s& reportcfg)
{
  report_cfg_to_add_mod_s new_rep;
  new_rep.report_cfg_id                     = srslte::find_rrc_obj_id_gap(var_meas.report_cfg_list);
  new_rep.report_cfg.set_report_cfg_eutra() = reportcfg;

  var_meas.report_cfg_list_present = true;
  return srslte::add_rrc_obj(var_meas.report_cfg_list, new_rep);
}

meas_id_to_add_mod_s* var_meas_cfg_t::add_measid_cfg(uint8_t measobjid, uint8_t measrepid)
{
  // ensure MeasObjId and ReportCfgId already exist
  auto objit = srslte::find_rrc_obj_id(var_meas.meas_obj_list, measobjid);
  if (objit == var_meas.meas_obj_list.end()) {
    ERROR("Failed to add MeasId because MeasObjId=%d is not found.\n", measobjid);
    return nullptr;
  }
  auto repit = srslte::find_rrc_obj_id(var_meas.report_cfg_list, measrepid);
  if (repit == var_meas.report_cfg_list.end()) {
    ERROR("Failed to add MeasId because ReportCfgId=%d is not found.\n", measrepid);
    return nullptr;
  }
  meas_id_to_add_mod_s new_measid;
  new_measid.report_cfg_id = measrepid;
  new_measid.meas_obj_id   = measobjid;
  new_measid.meas_id       = srslte::find_rrc_obj_id_gap(var_meas.meas_id_list);

  var_meas.meas_id_list_present = true;
  return srslte::add_rrc_obj(var_meas.meas_id_list, new_measid);
}

asn1::rrc::quant_cfg_s* var_meas_cfg_t::add_quant_cfg(const asn1::rrc::quant_cfg_eutra_s& quantcfg)
{
  var_meas.quant_cfg_present                 = true;
  var_meas.quant_cfg.quant_cfg_eutra_present = true;
  var_meas.quant_cfg.quant_cfg_eutra         = quantcfg;
  return &var_meas.quant_cfg;
}

bool var_meas_cfg_t::compute_diff_meas_cfg(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const
{
  *meas_cfg = {};

  // Shortcut in case this is the same as target
  if (this == &target_cfg) {
    return false;
  }

  // Set a MeasConfig in the RRC Connection Reconfiguration for HO.
  compute_diff_meas_objs(target_cfg, meas_cfg);
  compute_diff_report_cfgs(target_cfg, meas_cfg);
  compute_diff_meas_ids(target_cfg, meas_cfg);
  compute_diff_quant_cfg(target_cfg, meas_cfg);
  meas_cfg->meas_gap_cfg_present         = false; // NOTE: we do not support inter-freq. HO
  meas_cfg->s_measure_present            = false; // NOTE: We do not support SCells
  meas_cfg->pre_regist_info_hrpd_present = false; // NOTE: not supported
  meas_cfg->speed_state_pars_present     = false; // NOTE: not supported

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
  return diff;
}

//! adds all the cells that got updated to MeasCfg.
void var_meas_cfg_t::compute_diff_cells(const meas_obj_eutra_s& target_it,
                                        const meas_obj_eutra_s& src_it,
                                        meas_obj_to_add_mod_s*  added_obj) const
{
  meas_obj_eutra_s* eutra_obj = &added_obj->meas_obj.meas_obj_eutra();
  srslte::compute_cfg_diff(src_it.cells_to_add_mod_list,
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
void var_meas_cfg_t::compute_diff_meas_objs(const var_meas_cfg_t& target_cfg, meas_cfg_s* meas_cfg) const
{
  auto rem_func = [meas_cfg](const meas_obj_to_add_mod_s* it) {
    meas_cfg->meas_obj_to_rem_list.push_back(it->meas_obj_id);
  };
  auto add_func = [meas_cfg](const meas_obj_to_add_mod_s* it) { meas_cfg->meas_obj_to_add_mod_list.push_back(*it); };
  auto mod_func = [this, meas_cfg](const meas_obj_to_add_mod_s* src_it, const meas_obj_to_add_mod_s* target_it) {
    if (not(*src_it == *target_it)) {
      meas_obj_to_add_mod_s* added_obj = rrc_details::meascfg_add_meas_obj(meas_cfg, *target_it);
      // Add cells if there were changes.
      compute_diff_cells(target_it->meas_obj.meas_obj_eutra(), src_it->meas_obj.meas_obj_eutra(), added_obj);
    }
  };
  srslte::compute_cfg_diff(var_meas.meas_obj_list, target_cfg.var_meas.meas_obj_list, rem_func, add_func, mod_func);
  meas_cfg->meas_obj_to_add_mod_list_present = meas_cfg->meas_obj_to_add_mod_list.size() > 0;
  meas_cfg->meas_obj_to_rem_list_present     = meas_cfg->meas_obj_to_rem_list.size() > 0;
  // TODO: black cells and white cells
}

/**
 * Section 5.5.2.6/7 - Reporting configuration removal and addition/modification
 */
void var_meas_cfg_t::compute_diff_report_cfgs(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const
{
  srslte::compute_cfg_diff(var_meas.report_cfg_list,
                           target_cfg.var_meas.report_cfg_list,
                           meas_cfg->report_cfg_to_add_mod_list,
                           meas_cfg->report_cfg_to_rem_list);
  meas_cfg->report_cfg_to_add_mod_list_present = meas_cfg->report_cfg_to_add_mod_list.size() > 0;
  meas_cfg->report_cfg_to_rem_list_present     = meas_cfg->report_cfg_to_rem_list.size() > 0;
}

void var_meas_cfg_t::compute_diff_meas_ids(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const
{
  srslte::compute_cfg_diff(var_meas.meas_id_list,
                           target_cfg.var_meas.meas_id_list,
                           meas_cfg->meas_id_to_add_mod_list,
                           meas_cfg->meas_id_to_rem_list);
  meas_cfg->meas_id_to_add_mod_list_present = meas_cfg->meas_id_to_add_mod_list.size() > 0;
  meas_cfg->meas_id_to_rem_list_present     = meas_cfg->meas_id_to_rem_list.size() > 0;
}

void var_meas_cfg_t::compute_diff_quant_cfg(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg_msg) const
{
  if (target_cfg.var_meas.quant_cfg_present and
      (not var_meas.quant_cfg_present or not(target_cfg.var_meas.quant_cfg == var_meas.quant_cfg))) {
    meas_cfg_msg->quant_cfg_present = true;
    meas_cfg_msg->quant_cfg         = target_cfg.var_meas.quant_cfg;
  }
}

std::string var_meas_cfg_t::to_string() const
{
  asn1::json_writer js;
  var_meas.to_json(js);
  return js.to_string();
}

/**
 * Convert MeasCfg asn1 struct to var_meas_cfg_t
 * @param meas_cfg
 * @return
 */
var_meas_cfg_t var_meas_cfg_t::make(const asn1::rrc::meas_cfg_s& meas_cfg)
{
  var_meas_cfg_t var{};
  if (meas_cfg.meas_id_to_add_mod_list_present) {
    var.var_meas.meas_id_list_present = true;
    var.var_meas.meas_id_list         = meas_cfg.meas_id_to_add_mod_list;
  }
  if (meas_cfg.meas_obj_to_add_mod_list_present) {
    var.var_meas.meas_obj_list_present = true;
    var.var_meas.meas_obj_list         = meas_cfg.meas_obj_to_add_mod_list;
  }
  if (meas_cfg.report_cfg_to_add_mod_list_present) {
    var.var_meas.report_cfg_list_present = true;
    var.var_meas.report_cfg_list         = meas_cfg.report_cfg_to_add_mod_list;
  }
  if (meas_cfg.quant_cfg_present) {
    var.var_meas.quant_cfg_present = true;
    var.var_meas.quant_cfg         = meas_cfg.quant_cfg;
  }
  if (meas_cfg.s_measure_present) {
    var.var_meas.s_measure_present = true;
    var.var_meas.s_measure         = meas_cfg.s_measure;
  }
  if (meas_cfg.speed_state_pars_present) {
    var.var_meas.speed_state_pars_present = true;
    var.var_meas.speed_state_pars.set(meas_cfg.speed_state_pars.type().value);
    if (var.var_meas.speed_state_pars.type().value == setup_opts::setup) {
      var.var_meas.speed_state_pars.setup().mob_state_params   = meas_cfg.speed_state_pars.setup().mob_state_params;
      var.var_meas.speed_state_pars.setup().time_to_trigger_sf = meas_cfg.speed_state_pars.setup().time_to_trigger_sf;
    }
  }
  if (meas_cfg.report_cfg_to_rem_list_present or meas_cfg.meas_obj_to_rem_list_present or
      meas_cfg.meas_id_to_rem_list_present) {
    srslte::logmap::get("RRC")->warning("Remove lists not handled by the var_meas_cfg_t method\n");
  }
  return var;
}

var_meas_cfg_t var_meas_cfg_t::make(const rrc_cfg_t& cfg, const cell_info_common& pcell)
{
  // The measConfig contains measObjs for the PCell DL-EARFCN and activated UE carriers
  auto active_earfcns = get_measobj_earfcns(pcell);

  var_meas_cfg_t var_meas;
  if (not cfg.meas_cfg_present) {
    return var_meas;
  }

  // Add PCell+Scells as MeasObjs
  for (uint32_t earfcn : active_earfcns) {
    var_meas.add_meas_obj(earfcn);
  }

  for (const auto& cell_cfg : cfg.cell_list) {
    // inserts all neighbor cells and pcell, if q_offset > 0
    for (const meas_cell_cfg_t& meascell : cell_cfg.meas_cfg.meas_cells) {
      if (meascell.pci != pcell.cell_cfg.pci or meascell.earfcn != pcell.cell_cfg.dl_earfcn or meascell.q_offset > 0) {
        var_meas.add_cell_cfg(meascell);
      }
    }
    // insert same report cfg for all cells
    for (const report_cfg_eutra_s& reportcfg : cell_cfg.meas_cfg.meas_reports) {
      var_meas.add_report_cfg(reportcfg);
    }
    // insert quantity config
    var_meas.add_quant_cfg(cell_cfg.meas_cfg.quant_cfg);
  }

  // insert all meas ids
  // TODO: add this to the parser
  if (var_meas.rep_cfgs().size() > 0) {
    for (const auto& measobj : var_meas.meas_objs()) {
      var_meas.add_measid_cfg(measobj.meas_obj_id, var_meas.rep_cfgs().begin()->report_cfg_id);
    }
  }

  return var_meas;
}

/*************************************************************************************************
 *                                  mobility_cfg class
 ************************************************************************************************/

/**
 * Description: Handover Request Handling
 *             - Allocation of RNTI
 *             - Apply HandoverPreparation container to created UE state
 *             - Apply target cell config to UE state
 *             - Preparation of HandoverCommand that goes inside the transparent container of HandoverRequestAck
 *             - Response from TeNB on whether it was able to allocate resources for user doing handover
 * @return rnti of created ue
 */
uint16_t rrc::start_ho_ue_resource_alloc(const asn1::s1ap::ho_request_s&                                   msg,
                                         const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s& container)
{
  // TODO: Decision Making on whether the same QoS of the source eNB can be provided by target eNB

  /* Evaluate if cell exists */
  uint32_t                target_eci  = container.target_cell_id.cell_id.to_number();
  const cell_info_common* target_cell = cell_common_list->get_cell_id(rrc_details::eci_to_cellid(target_eci));
  if (target_cell == nullptr) {
    rrc_log->error("The S1-handover target cell_id=0x%x does not exist\n", rrc_details::eci_to_cellid(target_eci));
    return SRSLTE_INVALID_RNTI;
  }

  /* Create new User */

  // Allocate C-RNTI in MAC
  sched_interface::ue_cfg_t ue_cfg = {};
  ue_cfg.supported_cc_list.resize(1);
  ue_cfg.supported_cc_list[0].active     = true;
  ue_cfg.supported_cc_list[0].enb_cc_idx = target_cell->enb_cc_idx;
  ue_cfg.ue_bearers[0].direction         = sched_interface::ue_bearer_cfg_t::BOTH;
  ue_cfg.supported_cc_list[0].dl_cfg.tm  = SRSLTE_TM1;
  uint16_t rnti                          = mac->reserve_new_crnti(ue_cfg);
  if (rnti == SRSLTE_INVALID_RNTI) {
    rrc_log->error("Failed to allocate C-RNTI resources\n");
    return SRSLTE_INVALID_RNTI;
  }

  // Register new user in RRC
  add_user(rnti, ue_cfg);
  auto it     = users.find(rnti);
  ue*  ue_ptr = it->second.get();
  // Reset activity timer (Response is not expected)
  ue_ptr->set_activity_timeout(ue::UE_INACTIVITY_TIMEOUT);

  //  /* Setup e-RABs & DRBs / establish an UL/DL S1 bearer to the S-GW */
  //  if (not setup_ue_erabs(rnti, msg)) {
  //    rrc_ptr->rrc_log->error("Failed to setup e-RABs for rnti=0x%x\n", );
  //  }

  // TODO: KeNB derivations

  if (not ue_ptr->mobility_handler->start_s1_tenb_ho(msg, container)) {
    rem_user_thread(rnti);
    return SRSLTE_INVALID_RNTI;
  }
  return rnti;
}

/*************************************************************************************************
 *                                  rrc_mobility class
 ************************************************************************************************/

rrc::ue::rrc_mobility::rrc_mobility(rrc::ue* outer_ue) :
  base_t(outer_ue->parent->rrc_log),
  rrc_ue(outer_ue),
  rrc_enb(outer_ue->parent),
  pool(outer_ue->pool),
  rrc_log(outer_ue->parent->rrc_log)
{}

//! Method to add Mobility Info to a RRC Connection Reconfiguration Message
bool rrc::ue::rrc_mobility::fill_conn_recfg_no_ho_cmd(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_recfg)
{
  // only reconfigure meas_cfg if no handover is occurring.
  // NOTE: We basically freeze ue_var_meas for the whole duration of the handover procedure
  if (is_ho_running()) {
    return false;
  }

  // Check if there has been any update in ue_var_meas based on UE current cell list
  cell_ctxt_dedicated* pcell      = rrc_ue->cell_ded_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  uint32_t             src_earfcn = pcell->get_dl_earfcn();
  conn_recfg->meas_cfg_present    = update_ue_var_meas_cfg(src_earfcn, *pcell->cell_common, &conn_recfg->meas_cfg);
  return conn_recfg->meas_cfg_present;
}

//! Method called whenever the eNB receives a MeasReport from the UE. In normal situations, an HO procedure is started
void rrc::ue::rrc_mobility::handle_ue_meas_report(const meas_report_s& msg)
{
  if (not is_in_state<idle_st>()) {
    Info("Received a MeasReport while UE is performing Handover. Ignoring...\n");
    return;
  }
  // Check if meas_id is valid
  const meas_results_s& meas_res = msg.crit_exts.c1().meas_report_r8().meas_results;
  if (not meas_res.meas_result_neigh_cells_present) {
    Info("Received a MeasReport, but the UE did not detect any cell.\n");
    return;
  }
  if (meas_res.meas_result_neigh_cells.type().value !=
      meas_results_s::meas_result_neigh_cells_c_::types::meas_result_list_eutra) {
    Error("MeasReports regarding non-EUTRA are not supported!\n");
    return;
  }
  auto measid_it = srslte::find_rrc_obj_id(ue_var_meas.meas_ids(), meas_res.meas_id);
  if (measid_it == ue_var_meas.meas_ids().end()) {
    Warning("The measurement ID %d provided by the UE does not exist.\n", meas_res.meas_id);
    return;
  }
  const meas_result_list_eutra_l& eutra_report_list = meas_res.meas_result_neigh_cells.meas_result_list_eutra();

  // Find respective ReportCfg and MeasObj
  ho_meas_report_ev meas_ev{};
  auto              obj_it = srslte::find_rrc_obj_id(ue_var_meas.meas_objs(), measid_it->meas_obj_id);
  meas_ev.meas_obj         = &(*obj_it);

  // iterate from strongest to weakest cell
  const cell_ctxt_dedicated* pcell         = rrc_ue->cell_ded_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  const auto&                meas_list_cfg = pcell->cell_common->cell_cfg.meas_cfg.meas_cells;
  for (const meas_result_eutra_s& e : eutra_report_list) {
    auto                    same_pci = [&e](const meas_cell_cfg_t& c) { return c.pci == e.pci; };
    auto                    meas_it  = std::find_if(meas_list_cfg.begin(), meas_list_cfg.end(), same_pci);
    const cell_info_common* c        = rrc_enb->cell_common_list->get_pci(e.pci);
    if (meas_it != meas_list_cfg.end()) {
      meas_ev.target_eci = meas_it->eci;
    } else if (c != nullptr) {
      meas_ev.target_eci = (rrc_enb->cfg.enb_id << 8u) + c->cell_cfg.cell_id;
    } else {
      rrc_log->warning("The PCI=%d inside the MeasReport is not recognized.\n", e.pci);
      continue;
    }

    // eNB found the respective cell. eNB takes "HO Decision"
    // NOTE: From now we just choose the strongest.
    if (trigger(meas_ev)) {
      break;
    }
  }
}

/**
 * Description: Send "HO Required" message from source eNB to MME
 *              - 1st Message of the handover preparation phase
 *              - The RRC stores info regarding the source eNB configuration in a HO Preparation Info struct
 *              - This struct goes in a transparent container to the S1AP
 */
bool rrc::ue::rrc_mobility::start_ho_preparation(uint32_t target_eci,
                                                 uint8_t  measobj_id,
                                                 bool     fwd_direct_path_available)
{
  if (fwd_direct_path_available) {
    Error("Direct tunnels not supported supported\n");
    return false;
  }

  srslte::plmn_id_t target_plmn =
      srslte::make_plmn_id_t(rrc_enb->cfg.sib1.cell_access_related_info.plmn_id_list[0].plmn_id);
  const cell_ctxt_dedicated* src_cell_ded = rrc_ue->cell_ded_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  const cell_info_common*    src_cell_cfg = src_cell_ded->cell_common;

  /*** Fill HO Preparation Info ***/
  asn1::rrc::ho_prep_info_s         hoprep;
  asn1::rrc::ho_prep_info_r8_ies_s& hoprep_r8 = hoprep.crit_exts.set_c1().set_ho_prep_info_r8();
  if (not rrc_ue->eutra_capabilities_unpacked) {
    // TODO: temporary. Made up something to please target eNB. (there must be at least one capability in this packet)
    hoprep_r8.ue_radio_access_cap_info.resize(1);
    hoprep_r8.ue_radio_access_cap_info[0].rat_type = asn1::rrc::rat_type_e::eutra;
    asn1::rrc::ue_eutra_cap_s capitem;
    capitem.access_stratum_release                            = asn1::rrc::access_stratum_release_e::rel8;
    capitem.ue_category                                       = 4;
    capitem.pdcp_params.max_num_rohc_context_sessions_present = true;
    capitem.pdcp_params.max_num_rohc_context_sessions = asn1::rrc::pdcp_params_s::max_num_rohc_context_sessions_e_::cs2;
    bzero(&capitem.pdcp_params.supported_rohc_profiles,
          sizeof(asn1::rrc::rohc_profile_support_list_r15_s)); // TODO: why is it r15?
    capitem.phy_layer_params.ue_specific_ref_sigs_supported = false;
    capitem.phy_layer_params.ue_tx_ant_sel_supported        = false;
    capitem.rf_params.supported_band_list_eutra.resize(1);
    capitem.rf_params.supported_band_list_eutra[0].band_eutra  = 7;
    capitem.rf_params.supported_band_list_eutra[0].half_duplex = false;
    capitem.meas_params.band_list_eutra.resize(1);
    capitem.meas_params.band_list_eutra[0].inter_rat_band_list_present = false;
    capitem.meas_params.band_list_eutra[0].inter_freq_band_list.resize(1);
    capitem.meas_params.band_list_eutra[0].inter_freq_band_list[0].inter_freq_need_for_gaps = false;
    capitem.feature_group_inds_present                                                      = true;
    capitem.feature_group_inds.from_number(0xe6041000); // 0x5d0ffc80); // 0xe6041c00;
    {
      uint8_t       buffer[128];
      asn1::bit_ref bref(&buffer[0], sizeof(buffer));
      if (capitem.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
        rrc_log->error("Failed to pack UE EUTRA Capability\n");
      }
      hoprep_r8.ue_radio_access_cap_info[0].ue_cap_rat_container.resize((uint32_t)bref.distance_bytes());
      memcpy(&hoprep_r8.ue_radio_access_cap_info[0].ue_cap_rat_container[0], &buffer[0], bref.distance_bytes());
    }
    Debug("UE RA Category: %d\n", capitem.ue_category);
  } else {
    hoprep_r8.ue_radio_access_cap_info.resize(1);
    hoprep_r8.ue_radio_access_cap_info[0].rat_type = asn1::rrc::rat_type_e::eutra;

    srslte::unique_byte_buffer_t buffer = srslte::allocate_unique_buffer(*pool);
    asn1::bit_ref                bref(buffer->msg, buffer->get_tailroom());
    if (rrc_ue->eutra_capabilities.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
      rrc_log->error("Failed to pack UE EUTRA Capability\n");
      return false;
    }
    hoprep_r8.ue_radio_access_cap_info[0].ue_cap_rat_container.resize(bref.distance_bytes());
    memcpy(&hoprep_r8.ue_radio_access_cap_info[0].ue_cap_rat_container[0], buffer->msg, bref.distance_bytes());
  }
  /*** fill AS-Config ***/
  hoprep_r8.as_cfg_present = true;
  // NOTE: set source_meas_cnfg equal to the UE's current var_meas_cfg
  var_meas_cfg_t empty_meascfg{}, &target_var_meas = ue_var_meas;
  //  // however, reset the MeasObjToAdd Cells, so that the UE does not measure again the target eNB
  //  meas_obj_to_add_mod_s* obj = rrc_details::binary_find(target_var_meas.meas_objs(), measobj_id);
  //  obj->meas_obj.meas_obj_eutra().cells_to_add_mod_list.resize(0);
  empty_meascfg.compute_diff_meas_cfg(target_var_meas, &hoprep_r8.as_cfg.source_meas_cfg);
  // - fill source RR Config
  rrc_ue->fill_rrc_setup_rr_config_dedicated(&hoprep_r8.as_cfg.source_rr_cfg);
  // Add already established SRBs to the message
  hoprep_r8.as_cfg.source_rr_cfg.srb_to_add_mod_list_present = true;
  hoprep_r8.as_cfg.source_rr_cfg.srb_to_add_mod_list         = rrc_ue->bearer_list.get_established_srbs();
  // Get DRBs configuration
  hoprep_r8.as_cfg.source_rr_cfg.drb_to_add_mod_list = rrc_ue->bearer_list.get_established_drbs();
  hoprep_r8.as_cfg.source_rr_cfg.drb_to_add_mod_list_present =
      hoprep_r8.as_cfg.source_rr_cfg.drb_to_add_mod_list.size() > 0;
  // Get security cfg
  hoprep_r8.as_cfg.source_security_algorithm_cfg = rrc_ue->ue_security_cfg.get_security_algorithm_cfg();
  hoprep_r8.as_cfg.source_ue_id.from_number(rrc_ue->rnti);
  asn1::number_to_enum(hoprep_r8.as_cfg.source_mib.dl_bw, rrc_enb->cfg.cell.nof_prb);
  hoprep_r8.as_cfg.source_mib.phich_cfg.phich_dur.value =
      (asn1::rrc::phich_cfg_s::phich_dur_e_::options)rrc_enb->cfg.cell.phich_length;
  hoprep_r8.as_cfg.source_mib.phich_cfg.phich_res.value =
      (asn1::rrc::phich_cfg_s::phich_res_e_::options)rrc_enb->cfg.cell.phich_resources;
  hoprep_r8.as_cfg.source_mib.sys_frame_num.from_number(0); // NOTE: The TS says this can go empty
  hoprep_r8.as_cfg.source_sib_type1 = src_cell_cfg->sib1;
  hoprep_r8.as_cfg.source_sib_type2 = src_cell_cfg->sib2;
  asn1::number_to_enum(hoprep_r8.as_cfg.ant_info_common.ant_ports_count, rrc_enb->cfg.cell.nof_ports);
  hoprep_r8.as_cfg.source_dl_carrier_freq = src_cell_cfg->cell_cfg.dl_earfcn;
  // - fill as_context
  hoprep_r8.as_context_present               = true;
  hoprep_r8.as_context.reest_info_present    = true;
  hoprep_r8.as_context.reest_info.source_pci = src_cell_cfg->cell_cfg.pci;
  hoprep_r8.as_context.reest_info.target_cell_short_mac_i.from_number(
      rrc_details::compute_mac_i(rrc_ue->rnti,
                                 src_cell_cfg->sib1.cell_access_related_info.cell_id.to_number(),
                                 src_cell_cfg->cell_cfg.pci,
                                 rrc_ue->ue_security_cfg.get_as_sec_cfg().integ_algo,
                                 rrc_ue->ue_security_cfg.get_as_sec_cfg().k_rrc_int.data()));

  /*** pack HO Preparation Info into an RRC container buffer ***/
  srslte::unique_byte_buffer_t buffer = srslte::allocate_unique_buffer(*pool);
  asn1::bit_ref                bref(buffer->msg, buffer->get_tailroom());
  if (hoprep.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
    Error("Failed to pack HO preparation msg\n");
    return false;
  }
  buffer->N_bytes = bref.distance_bytes();

  bool success = rrc_enb->s1ap->send_ho_required(rrc_ue->rnti, target_eci, target_plmn, std::move(buffer));
  Info("sent s1ap msg with HO Required\n");
  return success;
}

/**
 * Description: Handover Preparation Complete (with success or failure)
 *             - MME --> SeNB
 *             - Response from MME on whether the HandoverRequired command is valid and the TeNB was able to allocate
 *               space for the UE
 * @param is_success flag to whether an HandoverCommand or HandoverReject was received
 * @param container RRC container with HandoverCommand to send to UE
 */
void rrc::ue::rrc_mobility::handle_ho_preparation_complete(bool is_success, srslte::unique_byte_buffer_t container)
{
  if (not is_success) {
    log_h->info("Received S1AP HandoverFailure. Aborting Handover...\n");
    trigger(srslte::failure_ev{});
    return;
  }
  /* unpack RRC HOCmd struct and perform sanity checks */
  asn1::rrc::ho_cmd_s rrchocmd;
  {
    asn1::cbit_ref bref(container->msg, container->N_bytes);
    if (rrchocmd.unpack(bref) != asn1::SRSASN_SUCCESS) {
      get_log()->warning("Unpacking of RRC HOCommand was unsuccessful\n");
      get_log()->warning_hex(container->msg, container->N_bytes, "Received container:\n");
      trigger(ho_cancel_ev{});
      return;
    }
  }
  if (rrchocmd.crit_exts.type().value != c1_or_crit_ext_opts::c1 or
      rrchocmd.crit_exts.c1().type().value != ho_cmd_s::crit_exts_c_::c1_c_::types_opts::ho_cmd_r8) {
    get_log()->warning("Only handling r8 Handover Commands\n");
    trigger(ho_cancel_ev{});
    return;
  }

  trigger(rrchocmd.crit_exts.c1().ho_cmd_r8());
}

bool rrc::ue::rrc_mobility::start_s1_tenb_ho(
    const asn1::s1ap::ho_request_s&                                   msg,
    const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s& container)
{
  trigger(ho_req_rx_ev{&msg, &container});
  return is_in_state<s1_target_ho_st>();
}

bool rrc::ue::rrc_mobility::update_ue_var_meas_cfg(uint32_t                src_earfcn,
                                                   const cell_info_common& target_pcell,
                                                   asn1::rrc::meas_cfg_s*  diff_meas_cfg)
{
  // Make UE Target VarMeasCfg based on active cells and parsed Config files
  var_meas_cfg_t target_var_meas = var_meas_cfg_t::make(rrc_enb->cfg, target_pcell);
  uint32_t       target_earfcn   = target_pcell.cell_cfg.dl_earfcn;

  // Apply TS 36.331 5.5.6.1 - If Source and Target eNB EARFCNs do no match, update SourceVarMeasCfg.MeasIdList
  if (target_earfcn != src_earfcn) {
    auto&                  meas_objs        = ue_var_meas.meas_objs();
    meas_obj_to_add_mod_s* found_target_obj = rrc_details::find_meas_obj(meas_objs, target_earfcn);
    meas_obj_to_add_mod_s* found_src_obj    = rrc_details::find_meas_obj(meas_objs, src_earfcn);
    if (found_target_obj != nullptr and found_src_obj != nullptr) {
      for (auto& mid : ue_var_meas.meas_ids()) {
        if (found_target_obj->meas_obj_id == mid.meas_obj_id) {
          mid.meas_obj_id = found_src_obj->meas_obj_id;
        } else if (found_src_obj->meas_obj_id == mid.meas_obj_id) {
          mid.meas_obj_id = found_target_obj->meas_obj_id;
        }
      }
    } else if (found_src_obj != nullptr) {
      for (auto it = ue_var_meas.meas_ids().begin(); it != ue_var_meas.meas_ids().end();) {
        if (it->meas_obj_id == found_src_obj->meas_obj_id) {
          auto rit = it++;
          ue_var_meas.meas_ids().erase(rit);
        } else {
          ++it;
        }
      }
    }
  }

  // Calculate difference between source and target VarMeasCfg
  bool meas_cfg_present = ue_var_meas.compute_diff_meas_cfg(target_var_meas, diff_meas_cfg);

  // Update user varMeasCfg to target
  ue_var_meas = target_var_meas;
  rrc_log->debug_long("New rnti=0x%x varMeasConfig: %s", rrc_ue->rnti, ue_var_meas.to_string().c_str());

  return meas_cfg_present;
}

/**
 * @brief Fills RRCConnectionReconfigurationMessage with Handover Command fields that are common to
 *        all types of handover (e.g. S1, intra-enb, X2), namely:
 *        - mobilityControlInformation
 *        - SecurityConfigHandover
 *        - RadioReconfiguration.PhyConfig
 *          - Scheduling Request setup
 *          - CQI report cfg
 *          - AntennaConfig
 * @param msg
 * @param target_cell
 */
void rrc::ue::rrc_mobility::fill_mobility_reconf_common(asn1::rrc::dl_dcch_msg_s& msg,
                                                        const cell_info_common&   target_cell,
                                                        uint32_t                  src_dl_earfcn)
{
  auto& recfg              = msg.msg.set_c1().set_rrc_conn_recfg();
  recfg.rrc_transaction_id = rrc_ue->transaction_id;
  rrc_ue->transaction_id   = (rrc_ue->transaction_id + 1) % 4;
  auto& recfg_r8           = recfg.crit_exts.set_c1().set_rrc_conn_recfg_r8();

  // Pack MobilityControlInfo message with params of target Cell
  recfg_r8.mob_ctrl_info_present = true;
  auto& mob_info                 = recfg_r8.mob_ctrl_info;
  mob_info.target_pci            = target_cell.cell_cfg.pci;
  mob_info.t304.value            = mob_ctrl_info_s::t304_opts::ms2000; // TODO: make it reconfigurable
  mob_info.new_ue_id.from_number(rrc_ue->rnti);
  mob_info.rr_cfg_common.pusch_cfg_common       = target_cell.sib2.rr_cfg_common.pusch_cfg_common;
  mob_info.rr_cfg_common.prach_cfg.root_seq_idx = target_cell.sib2.rr_cfg_common.prach_cfg.root_seq_idx;
  mob_info.rr_cfg_common.ul_cp_len              = target_cell.sib2.rr_cfg_common.ul_cp_len;
  mob_info.rr_cfg_common.p_max_present          = true;
  mob_info.rr_cfg_common.p_max                  = rrc_enb->cfg.sib1.p_max;
  mob_info.carrier_freq_present                 = false; // same frequency handover for now
  asn1::number_to_enum(mob_info.carrier_bw.dl_bw, target_cell.mib.dl_bw.to_number());
  if (target_cell.cell_cfg.dl_earfcn != src_dl_earfcn) {
    mob_info.carrier_freq_present         = true;
    mob_info.carrier_freq.dl_carrier_freq = target_cell.cell_cfg.dl_earfcn;
  }

  // Set security cfg
  recfg_r8.security_cfg_ho_present        = true;
  auto& intralte                          = recfg_r8.security_cfg_ho.handov_type.set_intra_lte();
  intralte.security_algorithm_cfg_present = false;
  intralte.key_change_ind                 = false;
  intralte.next_hop_chaining_count        = rrc_ue->ue_security_cfg.get_ncc();

  recfg_r8.rr_cfg_ded_present              = true;
  recfg_r8.rr_cfg_ded.phys_cfg_ded_present = true;
  phys_cfg_ded_s& phy_cfg                  = recfg_r8.rr_cfg_ded.phys_cfg_ded;

  phy_cfg.pusch_cfg_ded_present = true;
  phy_cfg.pusch_cfg_ded         = rrc_enb->cfg.pusch_cfg;

  // Set SR in new CC
  phy_cfg.sched_request_cfg_present = true;
  auto& sr_setup                    = phy_cfg.sched_request_cfg.set_setup();
  sr_setup.dsr_trans_max            = rrc_enb->cfg.sr_cfg.dsr_max;
  // TODO: For intra-freq handover, SR resources do not get updated. Update for inter-freq case
  sr_setup.sr_cfg_idx       = rrc_ue->cell_ded_list.get_sr_res()->sr_I;
  sr_setup.sr_pucch_res_idx = rrc_ue->cell_ded_list.get_sr_res()->sr_N_pucch;

  // Set CQI in new CC
  phy_cfg.cqi_report_cfg_present = true;
  if (rrc_enb->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC) {
    phy_cfg.cqi_report_cfg.cqi_report_mode_aperiodic_present = true;
    phy_cfg.cqi_report_cfg.cqi_report_mode_aperiodic         = cqi_report_mode_aperiodic_e::rm30;
  } else {
    phy_cfg.cqi_report_cfg.cqi_report_periodic_present = true;
    phy_cfg.cqi_report_cfg.cqi_report_periodic.set_setup();
    phy_cfg.cqi_report_cfg.cqi_report_periodic.setup().cqi_format_ind_periodic.set(
        cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::wideband_cqi);
    phy_cfg.cqi_report_cfg.cqi_report_periodic.setup().simul_ack_nack_and_cqi = rrc_enb->cfg.cqi_cfg.simultaneousAckCQI;
    rrc_ue->get_cqi(&phy_cfg.cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx,
                    &phy_cfg.cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx,
                    UE_PCELL_CC_IDX);
  }

  // Antenna info - start at TM1
  recfg_r8.rr_cfg_ded.phys_cfg_ded.ant_info_present = true;
  auto& ant_info                                    = recfg_r8.rr_cfg_ded.phys_cfg_ded.ant_info.set_explicit_value();
  ant_info.tx_mode.value                            = ant_info_ded_s::tx_mode_e_::tm1;
  ant_info.ue_tx_ant_sel.set(setup_e::release);

  // 256-QAM
  if (rrc_ue->ue_capabilities.support_dl_256qam) {
    phy_cfg.ext = true;
    phy_cfg.cqi_report_cfg_pcell_v1250.set_present(true);
    phy_cfg.cqi_report_cfg_pcell_v1250->alt_cqi_table_r12_present = true;
    phy_cfg.cqi_report_cfg_pcell_v1250->alt_cqi_table_r12.value =
        cqi_report_cfg_v1250_s::alt_cqi_table_r12_opts::all_sfs;
  }

  rrc_ue->apply_setup_phy_common(target_cell.sib2.rr_cfg_common, false);
  rrc_ue->apply_reconf_phy_config(recfg_r8, false);

  // Add MeasConfig of target cell
  recfg_r8.meas_cfg_present = update_ue_var_meas_cfg(src_dl_earfcn, target_cell, &recfg_r8.meas_cfg);

  // Add SCells
  if (rrc_ue->fill_scell_to_addmod_list(&recfg_r8) != SRSLTE_SUCCESS) {
    rrc_log->warning("Could not create configuration for Scell\n");
    return;
  }
}

/**
 * TS 36.413, Section 8.4.6 - eNB Status Transfer
 * Description: Send "eNBStatusTransfer" message from source eNB to MME
 *              - Pass bearers' DL/UL HFN and PDCP SN to be put inside a transparent container
 */
bool rrc::ue::rrc_mobility::start_enb_status_transfer()
{
  std::vector<s1ap_interface_rrc::bearer_status_info> s1ap_bearers;
  s1ap_bearers.reserve(rrc_ue->bearer_list.get_erabs().size());

  for (const auto& erab_pair : rrc_ue->bearer_list.get_erabs()) {
    s1ap_interface_rrc::bearer_status_info b    = {};
    uint8_t                                lcid = erab_pair.second.id - 2u;
    b.erab_id                                   = erab_pair.second.id;
    srslte::pdcp_lte_state_t pdcp_state         = {};
    if (not rrc_enb->pdcp->get_bearer_state(rrc_ue->rnti, lcid, &pdcp_state)) {
      Error("PDCP bearer lcid=%d for rnti=0x%x was not found\n", lcid, rrc_ue->rnti);
      return false;
    }
    b.dl_hfn     = pdcp_state.tx_hfn;
    b.pdcp_dl_sn = pdcp_state.next_pdcp_tx_sn;
    b.ul_hfn     = pdcp_state.rx_hfn;
    b.pdcp_ul_sn = pdcp_state.next_pdcp_rx_sn;
    s1ap_bearers.push_back(b);
  }

  Info("PDCP Bearer list sent to S1AP to initiate the eNB Status Transfer\n");
  return rrc_enb->s1ap->send_enb_status_transfer_proc(rrc_ue->rnti, s1ap_bearers);
}

/*************************************
 *     rrc_mobility FSM methods
 *************************************/

bool rrc::ue::rrc_mobility::needs_s1_ho(idle_st& s, const ho_meas_report_ev& meas_result)
{
  if (rrc_ue->get_state() != RRC_STATE_REGISTERED) {
    return false;
  }
  return rrc_details::eci_to_enbid(meas_result.target_eci) != rrc_enb->cfg.enb_id;
}

bool rrc::ue::rrc_mobility::needs_intraenb_ho(idle_st& s, const ho_meas_report_ev& meas_result)
{
  if (rrc_ue->get_state() != RRC_STATE_REGISTERED) {
    return false;
  }
  if (rrc_details::eci_to_enbid(meas_result.target_eci) != rrc_enb->cfg.enb_id) {
    return false;
  }
  uint32_t cell_id = rrc_details::eci_to_cellid(meas_result.target_eci);
  return rrc_ue->get_ue_cc_cfg(UE_PCELL_CC_IDX)->cell_cfg.cell_id != cell_id;
}

/*************************************
 *   s1_source_ho subFSM methods
 *************************************/

void rrc::ue::rrc_mobility::s1_source_ho_st::wait_ho_req_ack_st::enter(s1_source_ho_st* f, const ho_meas_report_ev& ev)
{
  srslte::console("Starting S1 Handover of rnti=0x%x to cellid=0x%x.\n", f->parent_fsm()->rrc_ue->rnti, ev.target_eci);
  f->get_log()->info(
      "Starting S1 Handover of rnti=0x%x to cellid=0x%x.\n", f->parent_fsm()->rrc_ue->rnti, ev.target_eci);
  f->report = ev;

  bool success = f->parent_fsm()->start_ho_preparation(f->report.target_eci, f->report.meas_obj->meas_obj_id, false);
  if (not success) {
    f->trigger(srslte::failure_ev{});
  }
}

void rrc::ue::rrc_mobility::s1_source_ho_st::send_ho_cmd(wait_ho_req_ack_st& s, const ho_cmd_r8_ies_s& ho_cmd)
{
  /* unpack DL-DCCH message containing the RRCRonnectionReconf (with MobilityInfo) to be sent to the UE */
  asn1::rrc::dl_dcch_msg_s dl_dcch_msg;
  {
    asn1::cbit_ref bref(&ho_cmd.ho_cmd_msg[0], ho_cmd.ho_cmd_msg.size());
    if (dl_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS) {
      get_log()->warning("Unpacking of RRC DL-DCCH message with HO Command was unsuccessful.\n");
      trigger(ho_cancel_ev{});
      return;
    }
  }
  if (dl_dcch_msg.msg.type().value != dl_dcch_msg_type_c::types_opts::c1 or
      dl_dcch_msg.msg.c1().type().value != dl_dcch_msg_type_c::c1_c_::types_opts::rrc_conn_recfg) {
    get_log()->warning("HandoverCommand is expected to contain an RRC Connection Reconf message inside\n");
    trigger(ho_cancel_ev{});
    return;
  }
  asn1::rrc::rrc_conn_recfg_s& reconf = dl_dcch_msg.msg.c1().rrc_conn_recfg();
  if (not reconf.crit_exts.c1().rrc_conn_recfg_r8().mob_ctrl_info_present) {
    get_log()->warning("HandoverCommand is expected to have mobility control subfield\n");
    trigger(ho_cancel_ev{});
    return;
  }

  // Disable DRBs
  parent_fsm()->rrc_ue->mac_ctrl->set_drb_activation(false);
  parent_fsm()->rrc_ue->mac_ctrl->update_mac(mac_controller::proc_stage_t::other);

  /* Send HO Command to UE */
  if (not parent_fsm()->rrc_ue->send_dl_dcch(&dl_dcch_msg)) {
    trigger(ho_cancel_ev{});
    return;
  }
}

//! Called in Source ENB during S1-Handover when there was a Reestablishment Request
void rrc::ue::rrc_mobility::s1_source_ho_st::handle_ho_cancel(const ho_cancel_ev& ev)
{
  parent_fsm()->rrc_enb->s1ap->send_ho_cancel(parent_fsm()->rrc_ue->rnti);
}

void rrc::ue::rrc_mobility::s1_source_ho_st::status_transfer_st::enter(s1_source_ho_st* f)
{
  f->get_log()->info("HandoverCommand of rnti=0x%x handled successfully.\n", f->parent_fsm()->rrc_ue->rnti);

  // TODO: Do anything with MeasCfg info within the Msg (e.g. update ue_var_meas)?

  /* Start S1AP eNBStatusTransfer Procedure */
  if (not f->parent_fsm()->start_enb_status_transfer()) {
    f->trigger(srslte::failure_ev{});
  }
}

/*************************************
 *   s1_target_ho state methods
 *************************************/

void rrc::ue::rrc_mobility::handle_ho_req(idle_st& s, const ho_req_rx_ev& ho_req)
{
  const auto& rrc_container = ho_req.transparent_container->rrc_container;

  /* TS 36.331 10.2.2. - Decode HandoverPreparationInformation */
  asn1::cbit_ref            bref{rrc_container.data(), rrc_container.size()};
  asn1::rrc::ho_prep_info_s hoprep;
  if (hoprep.unpack(bref) != asn1::SRSASN_SUCCESS) {
    rrc_enb->rrc_log->error("Failed to decode HandoverPreparationinformation in S1AP SourceENBToTargetENBContainer\n");
    trigger(srslte::failure_ev{});
    return;
  }
  if (hoprep.crit_exts.type().value != c1_or_crit_ext_opts::c1 or
      hoprep.crit_exts.c1().type().value != ho_prep_info_s::crit_exts_c_::c1_c_::types_opts::ho_prep_info_r8) {
    rrc_enb->rrc_log->error("Only release 8 supported\n");
    trigger(srslte::failure_ev{});
    return;
  }
  rrc_enb->log_rrc_message("HandoverPreparation", direction_t::fromS1AP, rrc_container, hoprep, "HandoverPreparation");

  /* Setup UE current state in TeNB based on HandoverPreparation message */
  const ho_prep_info_r8_ies_s& hoprep_r8 = hoprep.crit_exts.c1().ho_prep_info_r8();
  if (not apply_ho_prep_cfg(hoprep_r8, *ho_req.ho_req_msg)) {
    return;
  }

  /* Prepare Handover Request Acknowledgment - Handover Command */
  dl_dcch_msg_s              dl_dcch_msg;
  const cell_ctxt_dedicated* target_cell = rrc_ue->cell_ded_list.get_ue_cc_idx(UE_PCELL_CC_IDX);

  // Fill fields common to all types of handover (e.g. new CQI/SR configuration, mobControlInfo)
  fill_mobility_reconf_common(dl_dcch_msg, *target_cell->cell_common, hoprep_r8.as_cfg.source_dl_carrier_freq);
  rrc_conn_recfg_r8_ies_s& recfg_r8 = dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();

  // Apply new Security Config based on HandoverRequest
  // See TS 33.401, Sec. 7.2.8.4.3
  recfg_r8.security_cfg_ho_present = true;
  recfg_r8.security_cfg_ho.handov_type.set(security_cfg_ho_s::handov_type_c_::types_opts::intra_lte);
  recfg_r8.security_cfg_ho.handov_type.intra_lte().security_algorithm_cfg_present = true;
  recfg_r8.security_cfg_ho.handov_type.intra_lte().security_algorithm_cfg =
      rrc_ue->ue_security_cfg.get_security_algorithm_cfg();
  recfg_r8.security_cfg_ho.handov_type.intra_lte().key_change_ind = false;
  recfg_r8.security_cfg_ho.handov_type.intra_lte().next_hop_chaining_count =
      ho_req.ho_req_msg->protocol_ies.security_context.value.next_hop_chaining_count;

  /* Prepare Handover Command to be sent via S1AP */
  srslte::unique_byte_buffer_t ho_cmd_pdu = srslte::allocate_unique_buffer(*pool);
  asn1::bit_ref                bref2{ho_cmd_pdu->msg, ho_cmd_pdu->get_tailroom()};
  if (dl_dcch_msg.pack(bref2) != asn1::SRSASN_SUCCESS) {
    rrc_log->error("Failed to pack HandoverCommand\n");
    trigger(srslte::failure_ev{});
    return;
  }
  ho_cmd_pdu->N_bytes = bref2.distance_bytes();
  rrc_enb->log_rrc_message("RRC container", direction_t::toS1AP, ho_cmd_pdu.get(), dl_dcch_msg, "HandoverCommand");

  asn1::rrc::ho_cmd_s         ho_cmd;
  asn1::rrc::ho_cmd_r8_ies_s& ho_cmd_r8 = ho_cmd.crit_exts.set_c1().set_ho_cmd_r8();
  ho_cmd_r8.ho_cmd_msg.resize(bref2.distance_bytes());
  memcpy(ho_cmd_r8.ho_cmd_msg.data(), ho_cmd_pdu->msg, bref2.distance_bytes());
  bref2 = {ho_cmd_pdu->msg, ho_cmd_pdu->get_tailroom()};
  if (ho_cmd.pack(bref2) != asn1::SRSASN_SUCCESS) {
    rrc_log->error("Failed to pack HandoverCommand\n");
    trigger(srslte::failure_ev{});
    return;
  }
  ho_cmd_pdu->N_bytes = bref2.distance_bytes();

  /* Configure remaining layers based on pending changes */
  // Update RLC + PDCP SRBs (no DRBs until MME Status Transfer)
  rrc_ue->apply_pdcp_srb_updates();
  rrc_ue->apply_rlc_rb_updates();
  // Update MAC
  rrc_ue->mac_ctrl->handle_target_enb_ho_cmd(recfg_r8);
  // Apply PHY updates
  rrc_ue->apply_reconf_phy_config(recfg_r8, true);

  /* send S1AP HandoverRequestAcknowledge */
  std::vector<asn1::fixed_octstring<4, true> > admitted_erabs;
  for (auto& erab : rrc_ue->bearer_list.get_erabs()) {
    admitted_erabs.emplace_back();
    srslte::uint32_to_uint8(erab.second.teid_in, admitted_erabs.back().data());
  }
  if (not rrc_enb->s1ap->send_ho_req_ack(*ho_req.ho_req_msg, rrc_ue->rnti, std::move(ho_cmd_pdu), admitted_erabs)) {
    trigger(srslte::failure_ev{});
    return;
  }
}

bool rrc::ue::rrc_mobility::apply_ho_prep_cfg(const ho_prep_info_r8_ies_s&    ho_prep,
                                              const asn1::s1ap::ho_request_s& ho_req_msg)
{
  const cell_ctxt_dedicated*     target_cell     = rrc_ue->cell_ded_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  const cell_cfg_t&              target_cell_cfg = target_cell->cell_common->cell_cfg;
  const asn1::rrc::rr_cfg_ded_s& src_rr_cfg      = ho_prep.as_cfg.source_rr_cfg;

  // Establish SRBs
  if (src_rr_cfg.srb_to_add_mod_list_present) {
    for (auto& srb : src_rr_cfg.srb_to_add_mod_list) {
      rrc_ue->bearer_list.add_srb(srb.srb_id);
    }
  }

  // Establish ERABs/DRBs
  for (const auto& erab_item : ho_req_msg.protocol_ies.erab_to_be_setup_list_ho_req.value) {
    auto& erab = erab_item.value.erab_to_be_setup_item_ho_req();
    if (erab.ext) {
      get_log()->warning("Not handling E-RABToBeSetupList extensions\n");
    }
    if (erab.transport_layer_address.length() > 32) {
      get_log()->error("IPv6 addresses not currently supported\n");
      trigger(srslte::failure_ev{});
      return false;
    }

    if (not erab.ie_exts_present or not erab.ie_exts.data_forwarding_not_possible_present or
        erab.ie_exts.data_forwarding_not_possible.ext.value !=
            asn1::s1ap::data_forwarding_not_possible_opts::data_forwarding_not_possible) {
      get_log()->warning("Data Forwarding of E-RABs not supported\n");
    }

    uint32_t teid_out;
    srslte::uint8_to_uint32(erab.gtp_teid.data(), &teid_out);
    rrc_ue->bearer_list.add_erab(
        erab.erab_id, erab.erab_level_qos_params, erab.transport_layer_address, teid_out, nullptr);
    rrc_ue->bearer_list.add_gtpu_bearer(rrc_enb->gtpu, erab.erab_id);
  }

  // Regenerate AS Keys
  // See TS 33.401, Sec. 7.2.8.4.3
  rrc_ue->ue_security_cfg.set_security_capabilities(ho_req_msg.protocol_ies.ue_security_cap.value);
  rrc_ue->ue_security_cfg.set_security_key(ho_req_msg.protocol_ies.security_context.value.next_hop_param);
  rrc_ue->ue_security_cfg.set_ncc(ho_req_msg.protocol_ies.security_context.value.next_hop_chaining_count);
  rrc_ue->ue_security_cfg.regenerate_keys_handover(target_cell_cfg.pci, target_cell_cfg.dl_earfcn);

  // Save UE Capabilities
  for (const auto& cap : ho_prep.ue_radio_access_cap_info) {
    if (cap.rat_type.value == rat_type_opts::eutra) {
      asn1::cbit_ref bref(cap.ue_cap_rat_container.data(), cap.ue_cap_rat_container.size());
      if (rrc_ue->eutra_capabilities.unpack(bref) != asn1::SRSASN_SUCCESS) {
        rrc_log->warning("Failed to unpack UE EUTRA Capability\n");
        continue;
      }
      if (rrc_log->get_level() == srslte::LOG_LEVEL_DEBUG) {
        asn1::json_writer js{};
        rrc_ue->eutra_capabilities.to_json(js);
        rrc_log->debug_long("New rnti=0x%x EUTRA capabilities: %s\n", rrc_ue->rnti, js.to_string().c_str());
      }
      rrc_ue->ue_capabilities             = srslte::make_rrc_ue_capabilities(rrc_ue->eutra_capabilities);
      rrc_ue->eutra_capabilities_unpacked = true;
    }
  }

  // Save source UE MAC configuration as a base
  rrc_ue->mac_ctrl->handle_ho_prep(ho_prep);

  // Save measConfig
  ue_var_meas = var_meas_cfg_t::make(ho_prep.as_cfg.source_meas_cfg);
  rrc_log->debug_long("New rnti=0x%x varMeasConfig: %s", rrc_ue->rnti, ue_var_meas.to_string().c_str());

  return true;
}

void rrc::ue::rrc_mobility::handle_recfg_complete(wait_recfg_comp& s, const recfg_complete_ev& ev)
{
  cell_ctxt_dedicated* target_cell = rrc_ue->cell_ded_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  rrc_log->info("User rnti=0x%x successfully handovered to cell_id=0x%x\n",
                rrc_ue->rnti,
                target_cell->cell_common->cell_cfg.cell_id);
  uint64_t target_eci = (rrc_enb->cfg.enb_id << 8u) + target_cell->cell_common->cell_cfg.cell_id;

  rrc_enb->s1ap->send_ho_notify(rrc_ue->rnti, target_eci);
}

void rrc::ue::rrc_mobility::handle_status_transfer(s1_target_ho_st& s, const status_transfer_ev& erabs)
{
  // Establish DRBs
  rrc_ue->apply_pdcp_drb_updates();

  // Set DRBs SNs
  for (const auto& erab : erabs) {
    const auto& erab_item = erab.value.bearers_subject_to_status_transfer_item();
    auto        erab_it   = rrc_ue->bearer_list.get_erabs().find(erab_item.erab_id);
    if (erab_it == rrc_ue->bearer_list.get_erabs().end()) {
      rrc_log->warning("The E-RAB Id=%d is not recognized\n", erab_item.erab_id);
      continue;
    }
    const auto& drbs  = rrc_ue->bearer_list.get_pending_addmod_drbs();
    uint8_t     drbid = erab_item.erab_id - 4;
    auto        drb_it =
        std::find_if(drbs.begin(), drbs.end(), [drbid](const drb_to_add_mod_s& drb) { return drb.drb_id == drbid; });
    if (drb_it == drbs.end()) {
      rrc_log->warning("The DRB id=%d does not exist\n", erab_item.erab_id - 4);
    }

    srslte::pdcp_lte_state_t drb_state{};
    drb_state.tx_hfn                    = erab_item.dl_coun_tvalue.hfn;
    drb_state.next_pdcp_tx_sn           = erab_item.dl_coun_tvalue.pdcp_sn;
    drb_state.rx_hfn                    = erab_item.ul_coun_tvalue.hfn;
    drb_state.next_pdcp_rx_sn           = erab_item.ul_coun_tvalue.pdcp_sn;
    drb_state.last_submitted_pdcp_rx_sn = erab_item.ul_coun_tvalue.pdcp_sn;
    rrc_log->info("Setting lcid=%d PDCP state to {Tx SN: %d, Rx SN: %d}\n",
                  drb_it->lc_ch_id,
                  drb_state.next_pdcp_tx_sn,
                  drb_state.next_pdcp_rx_sn);
    rrc_enb->pdcp->set_bearer_state(rrc_ue->rnti, drb_it->lc_ch_id, drb_state);
  }

  // Check if there is any pending Reconfiguration Complete. If there is, self-trigger
  if (pending_recfg_complete.crit_exts.type().value != rrc_conn_recfg_complete_s::crit_exts_c_::types_opts::nulltype) {
    trigger(pending_recfg_complete);
    pending_recfg_complete.crit_exts.set(rrc_conn_recfg_complete_s::crit_exts_c_::types_opts::nulltype);
  }
}

void rrc::ue::rrc_mobility::defer_recfg_complete(s1_target_ho_st& s, const recfg_complete_ev& ev)
{
  pending_recfg_complete = ev;
}

/*************************************************************************************************
 *                                  intraENB Handover sub-FSM
 ************************************************************************************************/

void rrc::ue::rrc_mobility::intraenb_ho_st::enter(rrc_mobility* f, const ho_meas_report_ev& meas_report)
{
  uint32_t cell_id = rrc_details::eci_to_cellid(meas_report.target_eci);
  target_cell      = f->rrc_enb->cell_common_list->get_cell_id(cell_id);
  source_cell      = f->rrc_ue->cell_ded_list.get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common;
  if (target_cell == nullptr) {
    f->log_h->error("The target cell_id=0x%x was not found in the list of eNB cells\n", cell_id);
    f->trigger(srslte::failure_ev{});
    return;
  }

  f->log_h->info("Starting intraeNB Handover of rnti=0x%x to 0x%x.\n", f->rrc_ue->rnti, meas_report.target_eci);

  if (target_cell == nullptr) {
    f->trigger(srslte::failure_ev{});
    return;
  }
  last_temp_crnti = SRSLTE_INVALID_RNTI;

  /* Allocate Resources in Target Cell */
  if (not f->rrc_ue->cell_ded_list.set_cells({target_cell->enb_cc_idx})) {
    f->trigger(srslte::failure_ev{});
    return;
  }

  /* Prepare RRC Reconf Message with mobility info */
  dl_dcch_msg_s dl_dcch_msg;
  f->fill_mobility_reconf_common(dl_dcch_msg, *target_cell, source_cell->cell_cfg.dl_earfcn);

  // Apply changes to the MAC scheduler
  f->rrc_ue->mac_ctrl->handle_intraenb_ho_cmd(dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8());

  // Send DL-DCCH Message via current PCell
  if (not f->rrc_ue->send_dl_dcch(&dl_dcch_msg)) {
    f->trigger(srslte::failure_ev{});
    return;
  }
}

void rrc::ue::rrc_mobility::handle_crnti_ce(intraenb_ho_st& s, const user_crnti_upd_ev& ev)
{
  rrc_log->info("UE performing handover updated its temp-crnti=0x%x to rnti=0x%x\n", ev.temp_crnti, ev.crnti);
  bool is_first_crnti_ce = s.last_temp_crnti == SRSLTE_INVALID_RNTI;
  s.last_temp_crnti      = ev.temp_crnti;

  if (is_first_crnti_ce) {
    // Need to reset SNs of bearers.
    rrc_enb->pdcp->reestablish(rrc_ue->rnti);
    rrc_enb->rlc->reestablish(rrc_ue->rnti);

    // Change PCell in MAC/Scheduler
    rrc_ue->mac_ctrl->handle_crnti_ce(ev.temp_crnti);

    // finally apply new phy changes
    rrc_enb->phy->set_config(rrc_ue->rnti, rrc_ue->phy_rrc_dedicated_list);

    rrc_ue->ue_security_cfg.regenerate_keys_handover(s.target_cell->cell_cfg.pci, s.target_cell->cell_cfg.dl_earfcn);
    rrc_ue->bearer_list.reest_bearers();
    rrc_ue->apply_pdcp_srb_updates();
    rrc_ue->apply_pdcp_drb_updates();
  } else {
    rrc_log->info("Received duplicate C-RNTI CE during rnti=0x%x handover.\n", rrc_ue->rnti);
  }
}

void rrc::ue::rrc_mobility::handle_recfg_complete(intraenb_ho_st& s, const recfg_complete_ev& ev)
{
  rrc_log->info(
      "User rnti=0x%x successfully handovered to cell_id=0x%x\n", rrc_ue->rnti, s.target_cell->cell_cfg.cell_id);
}

} // namespace srsenb
