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
#include "srsenb/hdr/stack/rrc/rrc_cell_cfg.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/common.h"
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
      printf("Unsupported integrity algorithm.\n");
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

/** Finds a cell in this->objects based on cell_id and frequency
 *  return pair of (meas_obj,cell_obj). If no cell has frequency==earfcn, meas_obj=nullptr
 */
std::pair<meas_obj_to_add_mod_s*, cells_to_add_mod_s*>
find_cell(meas_obj_to_add_mod_list_l& l, uint32_t earfcn, uint8_t cell_id)
{
  // find meas_obj with same earfcn
  meas_obj_to_add_mod_s* obj = rrc_details::find_meas_obj(l, earfcn);
  if (obj == nullptr) {
    return std::make_pair(obj, (cells_to_add_mod_s*)nullptr);
  }
  // find cell with same id
  auto& cells   = obj->meas_obj.meas_obj_eutra().cells_to_add_mod_list;
  auto  cell_it = srslte::find_rrc_obj_id(cells, cell_id);
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

//! Add cell parsed in configuration file to the varMeasCfg
std::tuple<bool, var_meas_cfg_t::meas_obj_t*, var_meas_cfg_t::meas_cell_t*>
var_meas_cfg_t::add_cell_cfg(const meas_cell_cfg_t& cellcfg)
{
  using namespace rrc_details;
  bool inserted_flag = true;

  // TODO: cellcfg.eci is the ECI
  uint32_t         cell_id = rrc_details::eci_to_cellid(cellcfg.eci);
  q_offset_range_e offset;
  asn1::number_to_enum(offset, (int8_t)cellcfg.q_offset); // TODO: What's the difference

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
      // eci not found. create new cell
      ret.second = srslte::add_rrc_obj(ret.first->meas_obj.meas_obj_eutra().cells_to_add_mod_list, new_cell);
    }
  } else {
    // no measobj has been found with same earfcn, create a new one
    meas_obj_t new_obj;
    new_obj.meas_obj_id                = srslte::find_rrc_obj_id_gap(var_meas.meas_obj_list);
    asn1::rrc::meas_obj_eutra_s& eutra = new_obj.meas_obj.set_meas_obj_eutra();
    eutra.carrier_freq                 = cellcfg.earfcn;
    eutra.allowed_meas_bw.value        = asn1::rrc::allowed_meas_bw_e::mbw6; // TODO: What value to add here?
    eutra.neigh_cell_cfg.from_number(1);                                     // TODO: What value?
    eutra.offset_freq_present = true;
    // TODO: Assert that q_offset is in ms
    asn1::number_to_enum(eutra.offset_freq, cellcfg.q_offset);
    eutra.cells_to_add_mod_list_present = true;
    eutra.cells_to_add_mod_list.push_back(new_cell);
    ret.first  = srslte::add_rrc_obj(var_meas.meas_obj_list, new_obj);
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

/**
 * Convert MeasCfg asn1 struct to var_meas_cfg_t
 * @param meas_cfg
 * @return
 */
var_meas_cfg_t var_meas_cfg_t::make(const asn1::rrc::meas_cfg_s& meas_cfg)
{
  var_meas_cfg_t var;
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
  if (meas_cfg.report_cfg_to_rem_list_present or meas_cfg.meas_obj_to_rem_list_present or
      meas_cfg.meas_id_to_rem_list_present) {
    srslte::logmap::get("RRC")->warning("Remove lists not handled by the var_meas_cfg_t method\n");
  }
  return var;
}

/*************************************************************************************************
 *                                  mobility_cfg class
 ************************************************************************************************/

rrc::enb_mobility_handler::enb_mobility_handler(rrc* rrc_) : rrc_ptr(rrc_), cfg(&rrc_->cfg)
{
  cell_meas_cfg_list.resize(cfg->cell_list.size());

  /* Create Template Cell VarMeasCfg List */

  for (size_t i = 0; i < cfg->cell_list.size(); ++i) {
    std::unique_ptr<var_meas_cfg_t> var_meas{new var_meas_cfg_t{}};

    if (cfg->meas_cfg_present) {
      // inserts all neighbor cells
      for (const meas_cell_cfg_t& meascell : cfg->cell_list[i].meas_cfg.meas_cells) {
        var_meas->add_cell_cfg(meascell);
      }

      // insert same report cfg for all cells
      for (const report_cfg_eutra_s& reportcfg : cfg->cell_list[i].meas_cfg.meas_reports) {
        var_meas->add_report_cfg(reportcfg);
      }

      // insert all meas ids
      // TODO: add this to the parser
      if (var_meas->rep_cfgs().size() > 0) {
        for (const auto& measobj : var_meas->meas_objs()) {
          var_meas->add_measid_cfg(measobj.meas_obj_id, var_meas->rep_cfgs().begin()->report_cfg_id);
        }
      }

      // insert quantity config
      var_meas->add_quant_cfg(cfg->cell_list[i].meas_cfg.quant_cfg);
    }

    cell_meas_cfg_list[i].reset(var_meas.release());
  }
}

/*************************************************************************************************
 *                                  rrc_mobility class
 ************************************************************************************************/

rrc::ue::rrc_mobility::rrc_mobility(rrc::ue* outer_ue) :
  base_t(outer_ue->parent->rrc_log),
  rrc_ue(outer_ue),
  rrc_enb(outer_ue->parent),
  cfg(outer_ue->parent->enb_mobility_cfg.get()),
  pool(outer_ue->pool),
  rrc_log(outer_ue->parent->rrc_log),
  ue_var_meas(std::make_shared<var_meas_cfg_t>())
{}

//! Method to add Mobility Info to a RRC Connection Reconfiguration Message
bool rrc::ue::rrc_mobility::fill_conn_recfg_msg(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_recfg)
{
  // only reconfigure meas_cfg if no handover is occurring.
  // NOTE: We basically freeze ue_var_meas for the whole duration of the handover procedure
  if (is_ho_running()) {
    return false;
  }

  // Check if there has been any update in ue_var_meas
  cell_info_common*      pcell    = rrc_ue->get_ue_cc_cfg(UE_PCELL_CC_IDX);
  asn1::rrc::meas_cfg_s& meas_cfg = conn_recfg->meas_cfg;
  conn_recfg->meas_cfg_present    = update_ue_var_meas_cfg(*ue_var_meas, pcell->enb_cc_idx, &meas_cfg);
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
  auto measid_it = srslte::find_rrc_obj_id(ue_var_meas->meas_ids(), meas_res.meas_id);
  if (measid_it == ue_var_meas->meas_ids().end()) {
    Warning("The measurement ID %d provided by the UE does not exist.\n", meas_res.meas_id);
    return;
  }
  const meas_result_list_eutra_l& eutra_list = meas_res.meas_result_neigh_cells.meas_result_list_eutra();

  // Find respective ReportCfg and MeasObj
  ho_meas_report_ev meas_ev{};
  auto              obj_it = srslte::find_rrc_obj_id(ue_var_meas->meas_objs(), measid_it->meas_obj_id);
  meas_ev.meas_obj         = &(*obj_it);

  // iterate from strongest to weakest cell
  const cells_to_add_mod_list_l& cells         = obj_it->meas_obj.meas_obj_eutra().cells_to_add_mod_list;
  const cell_ctxt_dedicated*     pcell         = rrc_ue->cell_ded_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  const auto&                    meas_list_cfg = pcell->cell_common->cell_cfg.meas_cfg.meas_cells;
  const cells_to_add_mod_s*      cell_it       = nullptr;
  for (const meas_result_eutra_s& e : eutra_list) {
    uint16_t pci = e.pci;
    cell_it = std::find_if(cells.begin(), cells.end(), [pci](const cells_to_add_mod_s& c) { return c.pci == pci; });
    if (cell_it == cells.end()) {
      rrc_log->warning("The PCI=%d inside the MeasReport is not recognized.\n", pci);
      continue;
    }
    meas_ev.meas_cell  = cell_it;
    meas_ev.target_eci = std::find_if(meas_list_cfg.begin(), meas_list_cfg.end(), [pci](const meas_cell_cfg_t& c) {
                           return c.pci == pci;
                         })->eci;

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
  var_meas_cfg_t empty_meascfg{}, target_var_meas = *ue_var_meas;
  //  // however, reset the MeasObjToAdd Cells, so that the UE does not measure again the target eNB
  //  meas_obj_to_add_mod_s* obj = rrc_details::binary_find(target_var_meas.meas_objs(), measobj_id);
  //  obj->meas_obj.meas_obj_eutra().cells_to_add_mod_list.resize(0);
  empty_meascfg.compute_diff_meas_cfg(target_var_meas, &hoprep_r8.as_cfg.source_meas_cfg);
  // - fill source RR Config
  hoprep_r8.as_cfg.source_rr_cfg.sps_cfg_present = false; // TODO: CHECK
  hoprep_r8.as_cfg.source_rr_cfg.mac_main_cfg_present =
      rrc_ue->last_rrc_conn_recfg.crit_exts.c1().rrc_conn_recfg_r8().rr_cfg_ded.mac_main_cfg_present;
  hoprep_r8.as_cfg.source_rr_cfg.mac_main_cfg =
      rrc_ue->last_rrc_conn_recfg.crit_exts.c1().rrc_conn_recfg_r8().rr_cfg_ded.mac_main_cfg;
  hoprep_r8.as_cfg.source_rr_cfg.phys_cfg_ded_present =
      rrc_ue->last_rrc_conn_recfg.crit_exts.c1().rrc_conn_recfg_r8().rr_cfg_ded.phys_cfg_ded_present;
  hoprep_r8.as_cfg.source_rr_cfg.phys_cfg_ded =
      rrc_ue->last_rrc_conn_recfg.crit_exts.c1().rrc_conn_recfg_r8().rr_cfg_ded.phys_cfg_ded;
  // Add SRB2 to the message
  hoprep_r8.as_cfg.source_rr_cfg.srb_to_add_mod_list_present =
      rrc_ue->last_rrc_conn_recfg.crit_exts.c1().rrc_conn_recfg_r8().rr_cfg_ded.srb_to_add_mod_list_present;
  hoprep_r8.as_cfg.source_rr_cfg.srb_to_add_mod_list =
      rrc_ue->last_rrc_conn_recfg.crit_exts.c1().rrc_conn_recfg_r8().rr_cfg_ded.srb_to_add_mod_list;
  //  hoprep_r8.as_cfg.source_rr_cfg.srb_to_add_mod_list_present = true;
  //  asn1::rrc::srb_to_add_mod_list_l& srb_list                 = hoprep_r8.as_cfg.source_rr_cfg.srb_to_add_mod_list;
  //  srb_list.resize(1);
  //  srb_list[0].srb_id            = 2;
  //  srb_list[0].lc_ch_cfg_present = true;
  //  srb_list[0].lc_ch_cfg.set(asn1::rrc::srb_to_add_mod_s::lc_ch_cfg_c_::types::default_value);
  //  srb_list[0].rlc_cfg_present = true;
  //  srb_list[0].rlc_cfg.set_explicit_value();
  //  auto& am = srb_list[0].rlc_cfg.explicit_value().set_am(); // TODO: Which rlc cfg??? I took from a pcap for now
  //  am.ul_am_rlc.t_poll_retx             = asn1::rrc::t_poll_retx_e::ms60;
  //  am.ul_am_rlc.poll_pdu                = asn1::rrc::poll_pdu_e::p_infinity;
  //  am.ul_am_rlc.poll_byte.value         = asn1::rrc::poll_byte_e::kbinfinity;
  //  am.ul_am_rlc.max_retx_thres.value    = asn1::rrc::ul_am_rlc_s::max_retx_thres_e_::t32;
  //  am.dl_am_rlc.t_reordering.value      = asn1::rrc::t_reordering_e::ms45;
  //  am.dl_am_rlc.t_status_prohibit.value = asn1::rrc::t_status_prohibit_e::ms0;
  // Get DRB1 configuration
  hoprep_r8.as_cfg.source_rr_cfg.drb_to_add_mod_list_present =
      rrc_ue->last_rrc_conn_recfg.crit_exts.c1().rrc_conn_recfg_r8().rr_cfg_ded.drb_to_add_mod_list_present;
  hoprep_r8.as_cfg.source_rr_cfg.drb_to_add_mod_list =
      rrc_ue->last_rrc_conn_recfg.crit_exts.c1().rrc_conn_recfg_r8().rr_cfg_ded.drb_to_add_mod_list;
  //  hoprep_r8.as_cfg.source_rr_cfg.drb_to_add_mod_list_present = true;
  //  asn1::rrc::drb_to_add_mod_list_l& drb_list                 = hoprep_r8.as_cfg.source_rr_cfg.drb_to_add_mod_list;
  //  drb_list.resize(1);
  //  rrc_ue->get_drbid_config(&hoprep_r8.as_cfg.source_rr_cfg.drb_to_add_mod_list[0], 1);
  //  hoprep_r8.as_cfg.source_rr_cfg.drb_to_release_list_present = true;
  //  hoprep_r8.as_cfg.source_rr_cfg.drb_to_release_list.resize(1);
  //  hoprep_r8.as_cfg.source_rr_cfg.drb_to_release_list[0] = 1;
  hoprep_r8.as_cfg.source_security_algorithm_cfg = rrc_ue->ue_security_cfg.get_security_algorithm_cfg();
  hoprep_r8.as_cfg.source_ue_id.from_number(rrc_ue->rnti);
  asn1::number_to_enum(hoprep_r8.as_cfg.source_mib.dl_bw, rrc_enb->cfg.cell.nof_prb);
  hoprep_r8.as_cfg.source_mib.phich_cfg.phich_dur.value =
      (asn1::rrc::phich_cfg_s::phich_dur_e_::options)rrc_enb->cfg.cell.phich_length;
  hoprep_r8.as_cfg.source_mib.phich_cfg.phich_res.value =
      (asn1::rrc::phich_cfg_s::phich_res_e_::options)rrc_enb->cfg.cell.phich_resources;
  hoprep_r8.as_cfg.source_mib.sys_frame_num.from_number(0); // NOTE: The TS says this can go empty
  hoprep_r8.as_cfg.source_sib_type1 = rrc_enb->cfg.sib1;
  hoprep_r8.as_cfg.source_sib_type2 = rrc_ue->get_ue_cc_cfg(0)->sib2;
  asn1::number_to_enum(hoprep_r8.as_cfg.ant_info_common.ant_ports_count, rrc_enb->cfg.cell.nof_ports);
  hoprep_r8.as_cfg.source_dl_carrier_freq =
      rrc_enb->cfg.cell_list.at(0).dl_earfcn; // TODO: use actual DL EARFCN of source cell
  // - fill as_context
  hoprep_r8.as_context_present               = true;
  hoprep_r8.as_context.reest_info_present    = true;
  hoprep_r8.as_context.reest_info.source_pci = rrc_enb->cfg.cell_list.at(0).pci; // TODO: use actual PCI of source cell
  hoprep_r8.as_context.reest_info.target_cell_short_mac_i.from_number(
      rrc_details::compute_mac_i(rrc_ue->rnti,
                                 rrc_enb->cfg.sib1.cell_access_related_info.cell_id.to_number(),
                                 rrc_enb->cfg.cell_list.at(0).pci, // TODO: use actual PCI of source cell
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
  trigger(container);
}

bool rrc::ue::rrc_mobility::update_ue_var_meas_cfg(const asn1::rrc::meas_cfg_s& source_meas_cfg,
                                                   uint32_t                     target_enb_cc_idx,
                                                   asn1::rrc::meas_cfg_s*       diff_meas_cfg)
{
  // Generate equivalent VarMeasCfg
  var_meas_cfg_t source_var = var_meas_cfg_t::make(source_meas_cfg);

  // Compute difference measCfg and update UE VarMeasCfg
  return update_ue_var_meas_cfg(source_var, target_enb_cc_idx, diff_meas_cfg);
}

bool rrc::ue::rrc_mobility::update_ue_var_meas_cfg(const var_meas_cfg_t&  source_var_meas_cfg,
                                                   uint32_t               target_enb_cc_idx,
                                                   asn1::rrc::meas_cfg_s* diff_meas_cfg)
{
  // Fetch cell VarMeasCfg
  auto& target_var_ptr = rrc_enb->enb_mobility_cfg->cell_meas_cfg_list[target_enb_cc_idx];

  // Calculate difference between source and target VarMeasCfg
  bool meas_cfg_present = source_var_meas_cfg.compute_diff_meas_cfg(*target_var_ptr, diff_meas_cfg);

  // Update user varMeasCfg to target
  rrc_ue->mobility_handler->ue_var_meas = target_var_ptr;

  return meas_cfg_present;
}

void rrc::ue::rrc_mobility::fill_mobility_reconf_common(asn1::rrc::dl_dcch_msg_s& msg,
                                                        const cell_info_common&   target_cell)
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

  // Set security cfg
  recfg_r8.security_cfg_ho_present        = true;
  auto& intralte                          = recfg_r8.security_cfg_ho.handov_type.set_intra_lte();
  intralte.security_algorithm_cfg_present = false;
  intralte.key_change_ind                 = false;
  intralte.next_hop_chaining_count        = 0;

  recfg_r8.rr_cfg_ded_present              = true;
  recfg_r8.rr_cfg_ded.phys_cfg_ded_present = true;
  phys_cfg_ded_s& phy_cfg                  = recfg_r8.rr_cfg_ded.phys_cfg_ded;

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
    if (not rrc_enb->pdcp->get_bearer_status(rrc_ue->rnti, lcid, &b.pdcp_dl_sn, &b.dl_hfn, &b.pdcp_ul_sn, &b.ul_hfn)) {
      Error("PDCP bearer lcid=%d for rnti=0x%x was not found\n", lcid, rrc_ue->rnti);
      return false;
    }
    s1ap_bearers.push_back(b);
  }

  Info("PDCP Bearer list sent to S1AP to initiate the eNB Status Transfer\n");
  return rrc_enb->s1ap->send_enb_status_transfer_proc(rrc_ue->rnti, s1ap_bearers);
}

/*************************************
 *     rrc_mobility FSM methods
 *************************************/

bool rrc::ue::rrc_mobility::needs_s1_ho(idle_st& s, const ho_meas_report_ev& meas_result) const
{
  if (rrc_ue->get_state() != RRC_STATE_REGISTERED) {
    return false;
  }
  return rrc_details::eci_to_enbid(meas_result.target_eci) != rrc_enb->cfg.enb_id;
}

bool rrc::ue::rrc_mobility::needs_intraenb_ho(idle_st& s, const ho_meas_report_ev& meas_result) const
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

void rrc::ue::rrc_mobility::handle_s1_meas_report(idle_st& s, s1_source_ho_st& d, const ho_meas_report_ev& meas_report)
{
  Info("Starting S1 Handover of rnti=0x%x to 0x%x.\n", rrc_ue->rnti, meas_report.target_eci);
  d.report = meas_report;
}

/*************************************
 *   s1_source_ho subFSM methods
 *************************************/

void rrc::ue::rrc_mobility::s1_source_ho_st::wait_ho_req_ack_st::enter(s1_source_ho_st* f)
{
  bool success = f->parent_fsm()->start_ho_preparation(f->report.target_eci, f->report.meas_obj->meas_obj_id, false);
  if (not success) {
    f->trigger(srslte::failure_ev{});
  }
}

void rrc::ue::rrc_mobility::s1_source_ho_st::handle_ho_cmd(wait_ho_req_ack_st&                 s,
                                                           status_transfer_st&                 d,
                                                           const srslte::unique_byte_buffer_t& container)
{
  d.is_ho_cmd_sent = false;
  /* unpack RRC HOCmd struct and perform sanity checks */
  asn1::rrc::ho_cmd_s rrchocmd;
  {
    asn1::cbit_ref bref(container->msg, container->N_bytes);
    if (rrchocmd.unpack(bref) != asn1::SRSASN_SUCCESS) {
      log_h->warning("Unpacking of RRC HOCommand was unsuccessful\n");
      log_h->warning_hex(container->msg, container->N_bytes, "Received container:\n");
      return;
    }
  }
  if (rrchocmd.crit_exts.type().value != c1_or_crit_ext_opts::c1 or
      rrchocmd.crit_exts.c1().type().value != ho_cmd_s::crit_exts_c_::c1_c_::types_opts::ho_cmd_r8) {
    log_h->warning("Only handling r8 Handover Commands\n");
    return;
  }

  /* unpack DL-DCCH message containing the RRCRonnectionReconf (with MobilityInfo) to be sent to the UE */
  asn1::rrc::dl_dcch_msg_s dl_dcch_msg;
  {
    asn1::cbit_ref bref(&rrchocmd.crit_exts.c1().ho_cmd_r8().ho_cmd_msg[0],
                        rrchocmd.crit_exts.c1().ho_cmd_r8().ho_cmd_msg.size());
    if (dl_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS) {
      log_h->warning("Unpacking of RRC DL-DCCH message with HO Command was unsuccessful.\n");
      return;
    }
  }
  if (dl_dcch_msg.msg.type().value != dl_dcch_msg_type_c::types_opts::c1 or
      dl_dcch_msg.msg.c1().type().value != dl_dcch_msg_type_c::c1_c_::types_opts::rrc_conn_recfg) {
    log_h->warning("HandoverCommand is expected to contain an RRC Connection Reconf message inside\n");
    return;
  }
  asn1::rrc::rrc_conn_recfg_s& reconf = dl_dcch_msg.msg.c1().rrc_conn_recfg();
  if (not reconf.crit_exts.c1().rrc_conn_recfg_r8().mob_ctrl_info_present) {
    log_h->warning("HandoverCommand is expected to have mobility control subfield\n");
    return;
  }

  /* Send HO Command to UE */
  if (not parent_fsm()->rrc_ue->send_dl_dcch(&dl_dcch_msg)) {
    return;
  }

  d.is_ho_cmd_sent = true;
  log_h->info("HandoverCommand of rnti=0x%x handled successfully.\n", parent_fsm()->rrc_ue->rnti);
}

void rrc::ue::rrc_mobility::s1_source_ho_st::status_transfer_st::enter(s1_source_ho_st* f)
{
  if (not is_ho_cmd_sent) {
    f->trigger(srslte::failure_ev{});
  }
  // TODO: Do anything with MeasCfg info within the Msg (e.g. update ue_var_meas)?

  /* Start S1AP eNBStatusTransfer Procedure */
  if (not f->parent_fsm()->start_enb_status_transfer()) {
    f->trigger(srslte::failure_ev{});
  }
}

/*************************************************************************************************
 *                                  intraENB Handover sub-FSM
 ************************************************************************************************/

void rrc::ue::rrc_mobility::handle_intraenb_meas_report(idle_st&                 s,
                                                        intraenb_ho_st&          d,
                                                        const ho_meas_report_ev& meas_report)
{
  uint32_t cell_id   = rrc_details::eci_to_cellid(meas_report.target_eci);
  d.target_cell      = rrc_enb->cell_common_list->get_cell_id(cell_id);
  d.source_cell_ctxt = rrc_ue->cell_ded_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
  if (d.target_cell == nullptr) {
    rrc_log->error("The target cell_id=0x%x was not found in the list of eNB cells\n", cell_id);
    return;
  }

  Info("Starting intraeNB Handover of rnti=0x%x to 0x%x.\n", rrc_ue->rnti, meas_report.target_eci);
}

void rrc::ue::rrc_mobility::intraenb_ho_st::enter(rrc_mobility* f)
{
  if (target_cell == nullptr) {
    f->trigger(srslte::failure_ev{});
    return;
  }

  /* Allocate Resources in Target Cell */
  // NOTE: for intra-eNB Handover only CQI resources will change
  if (not f->rrc_ue->cell_ded_list.set_cells({target_cell->enb_cc_idx})) {
    f->trigger(srslte::failure_ev{});
    return;
  }

  /* Freeze all DRBs. SRBs are needed for sending the HO Cmd */
  for (const drb_to_add_mod_s& drb : f->rrc_ue->bearer_list.get_established_drbs()) {
    f->rrc_enb->pdcp->del_bearer(f->rrc_ue->rnti, drb.drb_id + 2);
    f->rrc_enb->mac->bearer_ue_rem(f->rrc_ue->rnti, drb.drb_id + 2);
  }

  /* Prepare RRC Reconf Message with mobility info */
  dl_dcch_msg_s dl_dcch_msg;
  f->fill_mobility_reconf_common(dl_dcch_msg, *target_cell);
  auto& recfg_r8 = dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();

  // Add MeasConfig of target cell
  auto prev_meas_var        = f->ue_var_meas;
  recfg_r8.meas_cfg_present = f->update_ue_var_meas_cfg(*f->ue_var_meas, target_cell->enb_cc_idx, &recfg_r8.meas_cfg);

  // Send DL-DCCH Message via current PCell
  if (not f->rrc_ue->send_dl_dcch(&dl_dcch_msg)) {
    f->trigger(srslte::failure_ev{});
    return;
  }
}

void rrc::ue::rrc_mobility::handle_crnti_ce(intraenb_ho_st& s, idle_st& d, const user_crnti_upd_ev& ev)
{
  rrc_log->info("UE performing handover updated its temp-crnti=0x%x to rnti=0x%x\n", ev.temp_crnti, ev.crnti);

  // Need to reset SNs of bearers.
  rrc_enb->pdcp->rem_user(rrc_ue->rnti);
  rrc_enb->pdcp->add_user(rrc_ue->rnti);
  rrc_enb->rlc->reestablish(rrc_ue->rnti);

  // Change PCell in MAC/Scheduler
  rrc_ue->current_sched_ue_cfg.supported_cc_list[0].active     = true;
  rrc_ue->current_sched_ue_cfg.supported_cc_list[0].enb_cc_idx = s.target_cell->enb_cc_idx;
  rrc_ue->apply_setup_phy_common(s.target_cell->sib2.rr_cfg_common);
  rrc_enb->mac->ue_set_crnti(ev.temp_crnti, ev.crnti, &rrc_ue->current_sched_ue_cfg);

  rrc_ue->ue_security_cfg.regenerate_keys_handover(s.target_cell->cell_cfg.pci, s.target_cell->cell_cfg.dl_earfcn);
  rrc_ue->bearer_list.reest_bearers();
  rrc_ue->bearer_list.apply_pdcp_bearer_updates(rrc_enb->pdcp, rrc_ue->ue_security_cfg);

  rrc_log->info("new rnti=0x%x PCell is %d\n", ev.crnti, s.target_cell->enb_cc_idx);
}

} // namespace srsenb
