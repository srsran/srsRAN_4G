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
                       uint8_t*                            k_rrc_int)
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

//! convenience function overload to extract Id from MeasObj/MeasId/ReportCfg/Cells
constexpr uint8_t get_id(const cells_to_add_mod_s& obj)
{
  return obj.cell_idx;
}
constexpr uint8_t get_id(const meas_obj_to_add_mod_s& obj)
{
  return obj.meas_obj_id;
}
constexpr uint8_t get_id(const report_cfg_to_add_mod_s& obj)
{
  return obj.report_cfg_id;
}
constexpr uint8_t get_id(const meas_id_to_add_mod_s& obj)
{
  return obj.meas_id;
}

//! convenience function overload to print MeasObj/MeasId/etc. fields
std::string to_string(const cells_to_add_mod_s& obj)
{
  char buf[128];
  std::snprintf(
      buf, 128, "{cell_idx: %d, pci: %d, offset: %d}", obj.cell_idx, obj.pci, obj.cell_individual_offset.to_number());
  return {buf};
}

//! meas field comparison based on ID solely
template <typename T>
struct field_id_cmp {
  bool operator()(const T& lhs, const T& rhs) const { return get_id(lhs) < get_id(rhs); }
  template <typename IdType>
  bool operator()(const T& lhs, IdType id) const
  {
    return get_id(lhs) < id;
  }
  template <typename IdType>
  bool operator()(IdType id, const T& rhs) const
  {
    return id < get_id(rhs);
  }
};
using cell_id_cmp     = field_id_cmp<cells_to_add_mod_s>;
using meas_obj_id_cmp = field_id_cmp<meas_obj_to_add_mod_s>;
using rep_cfg_id_cmp  = field_id_cmp<report_cfg_to_add_mod_s>;
using meas_id_cmp     = field_id_cmp<meas_id_to_add_mod_s>;

template <typename Container, typename IdType>
typename Container::iterator binary_find(Container& c, IdType id)
{
  auto it = std::lower_bound(c.begin(), c.end(), id, field_id_cmp<decltype(*c.begin())>{});
  return (it == c.end() or get_id(*it) != id) ? c.end() : it;
}
template <typename Container, typename IdType>
typename Container::const_iterator binary_find(const Container& c, IdType id)
{
  auto it = std::lower_bound(c.begin(), c.end(), id, field_id_cmp<decltype(*c.begin())>{});
  return (it == c.end() or get_id(*it) != id) ? c.end() : it;
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
  auto& cells = obj->meas_obj.meas_obj_eutra().cells_to_add_mod_list;
  auto  it    = binary_find(cells, cell_id);
  if (it == cells.end()) {
    it = nullptr;
  }
  return std::make_pair(obj, it);
}

/**
 * Section 5.5.2.5
 * Description: Adds MeasObjtoAddMod to MeasCfg object
 */
meas_obj_to_add_mod_s* meascfg_add_meas_obj(meas_cfg_s* meas_cfg, const meas_obj_to_add_mod_s& meas_obj)
{
  meas_cfg->meas_obj_to_add_mod_list_present = true;
  meas_obj_to_add_mod_list_l& l              = meas_cfg->meas_obj_to_add_mod_list;

  // search for meas_obj by obj_id to ensure uniqueness (assume sorted)
  auto found_it = binary_find(l, meas_obj.meas_obj_id);
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
  // do not add cellsToAddModList, blacCells, whiteCells, etc. according to (5.5.2.5 1|1|1)

  return found_it;
}

/*
 * Algorithm to compare differences between Meas Fields. This is used for MeasObjs, Cells, ReportConfig, MeasId (and
 * more in the future)
 * Returns outcome==same_id, if two fields were spotted with same id, "id_removed" if an id was removed from the target
 * "id_added" if an id was added to the target, and complete when the iteration is over
 */
enum class diff_outcome_t { same_id, id_removed, id_added, complete };
template <typename Container>
struct compute_diff_generator {
  using const_iterator = typename Container::const_iterator;
  struct result_t {
    diff_outcome_t outcome;
    const_iterator src_it;
    const_iterator target_it;
  };

  compute_diff_generator(const Container& src, const Container& target) :
    src_it(src.begin()),
    src_end(src.end()),
    target_it(target.begin()),
    target_end(target.end())
  {}

  result_t next()
  {
    bool src_left    = src_it != src_end;
    bool target_left = target_it != target_end;
    if (not src_left and not target_left) {
      return {diff_outcome_t::complete, nullptr, nullptr};
    }
    if (not target_left or (src_left and get_id(*src_it) < get_id(*target_it))) {
      // an object has been removed from the target
      return {diff_outcome_t::id_removed, src_it++, target_it};
    }
    if (not src_left or (target_left and get_id(*src_it) > get_id(*target_it))) {
      // a new object has been added to target
      return {diff_outcome_t::id_added, src_it, target_it++};
    }
    // Same ID
    return {diff_outcome_t::same_id, src_it++, target_it++};
  }

private:
  const_iterator src_it, src_end, target_it, target_end;
};

//! Find a Gap in Ids in a list of Meas Fields
template <typename Container, typename IdType = decltype(get_id(*Container{}.begin()))>
IdType find_id_gap(const Container& c)
{
  auto prev_it = c.begin();
  if (prev_it != c.end() and get_id(*prev_it) == 1) {
    auto it = prev_it;
    for (++it; it != c.end(); prev_it = it, ++it) {
      if (get_id(*it) > get_id(*prev_it) + 1) {
        break;
      }
    }
  }
  return (prev_it == c.end()) ? 1 : get_id(*prev_it) + 1; // starts at 1.
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
      auto& cell_list = ret.first->meas_obj.meas_obj_eutra().cells_to_add_mod_list;
      cell_list.push_back(new_cell);
      std::sort(cell_list.begin(), cell_list.end(), rrc_details::cell_id_cmp{});
      // find cell in new position
      ret.second = std::lower_bound(cell_list.begin(), cell_list.end(), new_cell.cell_idx, rrc_details::cell_id_cmp{});
    }
  } else {
    // no measobj has been found with same earfcn, create a new one
    meas_obj_t new_obj;
    new_obj.meas_obj_id                = rrc_details::find_id_gap(var_meas.meas_obj_list);
    asn1::rrc::meas_obj_eutra_s& eutra = new_obj.meas_obj.set_meas_obj_eutra();
    eutra.carrier_freq                 = cellcfg.earfcn;
    eutra.allowed_meas_bw.value        = asn1::rrc::allowed_meas_bw_e::mbw6; // TODO: What value to add here?
    eutra.neigh_cell_cfg.from_number(1);                                     // TODO: What value?
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

  if (inserted_flag) {
    var_meas.meas_obj_list_present = true;
  }

  return std::make_tuple(inserted_flag, ret.first, ret.second);
}

report_cfg_to_add_mod_s* var_meas_cfg_t::add_report_cfg(const report_cfg_eutra_s& reportcfg)
{
  rrc_details::field_id_cmp<report_cfg_to_add_mod_s> cmp{};
  report_cfg_to_add_mod_s                            new_rep;
  new_rep.report_cfg_id                     = rrc_details::find_id_gap(var_meas.report_cfg_list);
  new_rep.report_cfg.set_report_cfg_eutra() = reportcfg;

  var_meas.report_cfg_list_present = true;
  var_meas.report_cfg_list.push_back(new_rep);
  std::sort(var_meas.report_cfg_list.begin(), var_meas.report_cfg_list.end(), cmp);
  return std::lower_bound(var_meas.report_cfg_list.begin(), var_meas.report_cfg_list.end(), new_rep.report_cfg_id, cmp);
}

meas_id_to_add_mod_s* var_meas_cfg_t::add_measid_cfg(uint8_t measobjid, uint8_t measrepid)
{
  // ensure MeasObjId and ReportCfgId already exist
  auto objit = std::lower_bound(
      var_meas.meas_obj_list.begin(), var_meas.meas_obj_list.end(), measobjid, rrc_details::meas_obj_id_cmp{});
  if (objit == var_meas.meas_obj_list.end() or objit->meas_obj_id != measobjid) {
    ERROR("Failed to add MeasId because MeasObjId=%d is not found.\n", measobjid);
    return nullptr;
  }
  auto repit = std::lower_bound(
      var_meas.report_cfg_list.begin(), var_meas.report_cfg_list.end(), measrepid, rrc_details::rep_cfg_id_cmp{});
  if (repit == var_meas.report_cfg_list.end() or repit->report_cfg_id != measrepid) {
    ERROR("Failed to add MeasId because ReportCfgId=%d is not found.\n", measrepid);
    return nullptr;
  }
  rrc_details::field_id_cmp<meas_id_to_add_mod_s> cmp{};
  meas_id_to_add_mod_s                            new_measid;
  new_measid.report_cfg_id = measrepid;
  new_measid.meas_obj_id   = measobjid;
  new_measid.meas_id       = rrc_details::find_id_gap(var_meas.meas_id_list);

  var_meas.meas_id_list_present = true;
  var_meas.meas_id_list.push_back(new_measid);
  std::sort(var_meas.meas_id_list.begin(), var_meas.meas_id_list.end(), cmp);
  return std::lower_bound(var_meas.meas_id_list.begin(), var_meas.meas_id_list.end(), new_measid.meas_id, cmp);
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
  rrc_details::compute_diff_generator<cells_to_add_mod_list_l> diffs{src_it.cells_to_add_mod_list,
                                                                     target_it.cells_to_add_mod_list};

  meas_obj_eutra_s* eutra_obj = &added_obj->meas_obj.meas_obj_eutra();
  while (true) {
    auto result = diffs.next();
    switch (result.outcome) {
      case rrc_details::diff_outcome_t::complete:
        return;
      case rrc_details::diff_outcome_t::id_removed:
        // case "entry with matching cellIndex exists in cellsToRemoveList
        Info("UE can now cease to measure activity of cell %s.\n", rrc_details::to_string(*result.target_it).c_str());
        eutra_obj->cells_to_rem_list_present = true;
        eutra_obj->cells_to_rem_list.push_back(result.src_it->cell_idx);
        break;
      case rrc_details::diff_outcome_t::id_added:
        // case "entry with matching cellIndex doesn't exist in cellsToAddModList"
        Info("UE has now to measure activity of %s.\n", rrc_details::to_string(*result.target_it).c_str());
        eutra_obj->cells_to_add_mod_list_present = true;
        eutra_obj->cells_to_add_mod_list.push_back(*result.target_it);
        break;
      case rrc_details::diff_outcome_t::same_id:
        if (not(*result.src_it == *result.target_it)) {
          // case "entry with matching cellIndex exists in cellsToAddModList"
          Info("UE has now to measure activity of %s with updated params.\n",
               rrc_details::to_string(*result.target_it).c_str());
          eutra_obj->cells_to_add_mod_list_present = true;
          eutra_obj->cells_to_add_mod_list.push_back(*result.target_it);
        }
        break;
    }
  }
}

/**
 * Section 5.5.2.4/5, Measurement Object removal and addition/modification
 * Description: compute diff between target_cfg and var_meas -> depending on diff, add/remove/update meas_obj in
 * meas_cfg
 */
void var_meas_cfg_t::compute_diff_meas_objs(const var_meas_cfg_t& target_cfg, meas_cfg_s* meas_cfg) const
{
  if (not target_cfg.var_meas.meas_obj_list_present) {
    return;
  }
  // TODO: black cells and white cells
  rrc_details::compute_diff_generator<meas_obj_to_add_mod_list_l> diffs{var_meas.meas_obj_list,
                                                                        target_cfg.var_meas.meas_obj_list};

  while (true) {
    auto result = diffs.next();
    switch (result.outcome) {
      case rrc_details::diff_outcome_t::complete:
        return;
      case rrc_details::diff_outcome_t::id_removed:
        // case "entry with matching cellIndex exists in cellsToRemoveList
        Info("UE can cease to measure activity in frequency earfcn=%d.\n",
             result.src_it->meas_obj.meas_obj_eutra().carrier_freq);
        meas_cfg->meas_obj_to_rem_list_present = true;
        meas_cfg->meas_obj_to_rem_list.push_back(result.src_it->meas_obj_id);
        break;
      case rrc_details::diff_outcome_t::id_added: {
        // case "entry with matching measObjectId doesn't exist in measObjToAddModList"
        Info("UE has now to measure activity of new frequency earfcn=%d.\n",
             result.target_it->meas_obj.meas_obj_eutra().carrier_freq);
        auto& target_eutra = result.target_it->meas_obj.meas_obj_eutra();
        auto& added_eutra  = rrc_details::meascfg_add_meas_obj(meas_cfg, *result.target_it)->meas_obj.meas_obj_eutra();
        // add all cells in measCfg
        for (const cells_to_add_mod_s& cell_it : target_eutra.cells_to_add_mod_list) {
          added_eutra.cells_to_add_mod_list_present = true;
          added_eutra.cells_to_add_mod_list.push_back(cell_it);
        }
      } break;
      case rrc_details::diff_outcome_t::same_id:
        // case "entry with matching measObjectId exists in measObjToAddModList"
        bool are_equal = *result.src_it == *result.target_it;
        if (not are_equal) {
          // if we found a difference in obj IDs
          meas_obj_to_add_mod_s* added_obj = rrc_details::meascfg_add_meas_obj(meas_cfg, *result.target_it);
          // Add cells if there were changes.
          compute_diff_cells(
              result.target_it->meas_obj.meas_obj_eutra(), result.src_it->meas_obj.meas_obj_eutra(), added_obj);
        }
        break;
    }
  }
}

/**
 * Section 5.5.2.6/7 - Reporting configuration removal and addition/modification
 */
void var_meas_cfg_t::compute_diff_report_cfgs(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const
{
  if (not target_cfg.var_meas.report_cfg_list_present) {
    return;
  }
  rrc_details::compute_diff_generator<report_cfg_to_add_mod_list_l> diffs{var_meas.report_cfg_list,
                                                                          target_cfg.var_meas.report_cfg_list};

  while (true) {
    auto result = diffs.next();
    switch (result.outcome) {
      case rrc_details::diff_outcome_t::complete:
        return;
      case rrc_details::diff_outcome_t::id_removed:
        meas_cfg->report_cfg_to_rem_list_present = true;
        meas_cfg->report_cfg_to_rem_list.push_back(result.src_it->report_cfg_id);
        break;
      case rrc_details::diff_outcome_t::id_added:
        meas_cfg->report_cfg_to_add_mod_list_present = true;
        meas_cfg->report_cfg_to_add_mod_list.push_back(*result.target_it);
        break;
      case rrc_details::diff_outcome_t::same_id:
        if (not(*result.src_it == *result.target_it)) {
          meas_cfg->report_cfg_to_add_mod_list_present = true;
          meas_cfg->report_cfg_to_add_mod_list.push_back(*result.target_it);
        }
        break;
    }
  }
}

void var_meas_cfg_t::compute_diff_meas_ids(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const
{
  if (not target_cfg.var_meas.meas_id_list_present) {
    return;
  }
  rrc_details::compute_diff_generator<meas_id_to_add_mod_list_l> diffs{var_meas.meas_id_list,
                                                                       target_cfg.var_meas.meas_id_list};

  while (true) {
    auto result = diffs.next();
    switch (result.outcome) {
      case rrc_details::diff_outcome_t::complete:
        return;
      case rrc_details::diff_outcome_t::id_removed:
        meas_cfg->meas_id_to_rem_list_present = true;
        meas_cfg->meas_id_to_rem_list.push_back(result.src_it->meas_id);
        break;
      case rrc_details::diff_outcome_t::id_added:
        meas_cfg->meas_id_to_add_mod_list_present = true;
        meas_cfg->meas_id_to_add_mod_list.push_back(*result.target_it);
        break;
      case rrc_details::diff_outcome_t::same_id:
        if (not(*result.src_it == *result.target_it)) {
          meas_cfg->meas_id_to_add_mod_list_present = true;
          meas_cfg->meas_id_to_add_mod_list.push_back(*result.target_it);
        }
        break;
    }
  }
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
  rrc_ue(outer_ue),
  rrc_enb(outer_ue->parent),
  cfg(outer_ue->parent->enb_mobility_cfg.get()),
  pool(outer_ue->pool),
  rrc_log(outer_ue->parent->rrc_log),
  source_ho_proc(this),
  ue_var_meas(std::make_shared<var_meas_cfg_t>())
{}

//! Method to add Mobility Info to a RRC Connection Reconfiguration Message
bool rrc::ue::rrc_mobility::fill_conn_recfg_msg(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_recfg)
{
  // only reconfigure meas_cfg if no handover is occurring.
  // NOTE: We basically freeze ue_var_meas for the whole duration of the handover procedure
  if (source_ho_proc.is_busy()) {
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
  const meas_results_s& meas_res = msg.crit_exts.c1().meas_report_r8().meas_results;

  const meas_id_to_add_mod_list_l& l         = ue_var_meas->meas_ids();
  auto                             measid_it = rrc_details::binary_find(l, meas_res.meas_id);
  if (measid_it == l.end()) {
    Warning("The measurement ID %d provided by the UE does not exist.\n", meas_res.meas_id);
    return;
  }
  if (not meas_res.meas_result_neigh_cells_present) {
    Info("Received a MeasReport, but the UE did not detect any cell.\n");
    return;
  }
  if (meas_res.meas_result_neigh_cells.type().value !=
      meas_results_s::meas_result_neigh_cells_c_::types::meas_result_list_eutra) {
    Error("MeasReports regarding non-EUTRA are not supported!\n");
    return;
  }

  const meas_obj_to_add_mod_list_l&   objs       = ue_var_meas->meas_objs();
  const report_cfg_to_add_mod_list_l& reps       = ue_var_meas->rep_cfgs();
  auto                                obj_it     = rrc_details::binary_find(objs, measid_it->meas_obj_id);
  auto                                rep_it     = rrc_details::binary_find(reps, measid_it->report_cfg_id);
  const meas_result_list_eutra_l&     eutra_list = meas_res.meas_result_neigh_cells.meas_result_list_eutra();

  // iterate from strongest to weakest cell
  // NOTE: From now we just look at the strongest.
  if (eutra_list.size() > 0) {
    uint32_t i = 0;

    uint16_t                       pci   = eutra_list[i].pci;
    const cells_to_add_mod_list_l& cells = obj_it->meas_obj.meas_obj_eutra().cells_to_add_mod_list;

    const cells_to_add_mod_s* cell_it =
        std::find_if(cells.begin(), cells.end(), [pci](const cells_to_add_mod_s& c) { return c.pci == pci; });
    if (cell_it == cells.end()) {
      rrc_log->error("The PCI=%d inside the MeasReport is not recognized.\n", pci);
      return;
    }

    // eNB found the respective cell. eNB takes "HO Decision"
    // TODO: check what to do here to take the decision.
    // NOTE: for now just accept anything.

    // NOTE: Handover disabled
    // Target cell to handover to was selected.
    //    auto&    L = rrc_enb->cfg.cell_list[rrc_ue->get_ue_cc_cfg(UE_PCELL_CC_IDX)->enb_cc_idx].meas_cfg.meas_cells;
    //    uint32_t target_eci = std::find_if(L.begin(), L.end(), [pci](meas_cell_cfg_t& c) { return c.pci == pci;
    //    })->eci; if (not source_ho_proc.launch(*measid_it, *obj_it, *rep_it, *cell_it, eutra_list[i], target_eci)) {
    //      Error("Failed to start HO procedure, as it is already on-going\n");
    //      return;
    //    }
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
  hoprep_r8.as_cfg.source_security_algorithm_cfg = rrc_ue->last_security_mode_cmd;
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
                                 rrc_ue->sec_cfg.integ_algo,
                                 rrc_ue->sec_cfg.k_rrc_int.data()));

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

void rrc::ue::rrc_mobility::handle_ho_preparation_complete(bool is_success, srslte::unique_byte_buffer_t container)
{
  source_ho_proc.trigger(sourceenb_ho_proc_t::ho_prep_result{is_success, std::move(container)});
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

/**
 * TS 36.413, Section 8.4.6 - eNB Status Transfer
 * Description: Send "eNBStatusTransfer" message from source eNB to MME
 *              - Pass bearers' DL/UL HFN and PDCP SN to be put inside a transparent container
 */
bool rrc::ue::rrc_mobility::start_enb_status_transfer()
{
  std::vector<s1ap_interface_rrc::bearer_status_info> bearer_list;
  bearer_list.reserve(rrc_ue->erabs.size());

  for (const auto& erab_pair : rrc_ue->erabs) {
    s1ap_interface_rrc::bearer_status_info b    = {};
    uint8_t                                lcid = erab_pair.second.id - 2u;
    b.erab_id                                   = erab_pair.second.id;
    if (not rrc_enb->pdcp->get_bearer_status(rrc_ue->rnti, lcid, &b.pdcp_dl_sn, &b.dl_hfn, &b.pdcp_ul_sn, &b.ul_hfn)) {
      Error("PDCP bearer lcid=%d for rnti=0x%x was not found\n", lcid, rrc_ue->rnti);
      return false;
    }
    bearer_list.push_back(b);
  }

  Info("PDCP Bearer list sent to S1AP to initiate the eNB Status Transfer\n");
  return rrc_enb->s1ap->send_enb_status_transfer_proc(rrc_ue->rnti, bearer_list);
}

/*************************************************************************************************
 *                                  sourceenb_ho_proc_t class
 ************************************************************************************************/

rrc::ue::rrc_mobility::sourceenb_ho_proc_t::sourceenb_ho_proc_t(rrc_mobility* ue_mobility_) : parent(ue_mobility_) {}

srslte::proc_outcome_t rrc::ue::rrc_mobility::sourceenb_ho_proc_t::init(const meas_id_to_add_mod_s&    measid_,
                                                                        const meas_obj_to_add_mod_s&   measobj_,
                                                                        const report_cfg_to_add_mod_s& repcfg_,
                                                                        const cells_to_add_mod_s&      cell_,
                                                                        const meas_result_eutra_s&     meas_res_,
                                                                        uint32_t                       target_eci_)
{
  measid     = &measid_;
  measobj    = &measobj_;
  repcfg     = &repcfg_;
  cell       = &cell_;
  meas_res   = meas_res_;
  target_eci = target_eci_;

  // TODO: Check X2 is available first. If fail, go for S1.
  // NOTE: For now only S1-HO is supported. X2 also not available for fwd direct path
  ho_interface              = ho_interface_t::S1;
  fwd_direct_path_available = false;

  state = state_t::ho_preparation;
  procInfo("Started Handover of rnti=0x%x to %s.\n", parent->rrc_ue->rnti, rrc_details::to_string(*cell).c_str());
  if (not parent->start_ho_preparation(target_eci, measobj->meas_obj_id, fwd_direct_path_available)) {
    procError("Failed to send HO Required to MME.\n");
    return srslte::proc_outcome_t::error;
  }
  return srslte::proc_outcome_t::yield;
}

srslte::proc_outcome_t rrc::ue::rrc_mobility::sourceenb_ho_proc_t::react(ho_prep_result e)
{
  if (not e.is_success) {
    procError("Failure during handover preparation.\n");
    return srslte::proc_outcome_t::error;
  }

  /* unpack RRC HOCmd struct and perform sanity checks */
  asn1::rrc::ho_cmd_s rrchocmd;
  {
    asn1::cbit_ref bref(e.rrc_container->msg, e.rrc_container->N_bytes);
    if (rrchocmd.unpack(bref) != asn1::SRSASN_SUCCESS) {
      procError("Unpacking of RRC HOCommand was unsuccessful\n");
      parent->rrc_log->error_hex(e.rrc_container->msg, e.rrc_container->N_bytes, "Received container:\n");
      return srslte::proc_outcome_t::error;
    }
  }
  if (rrchocmd.crit_exts.type().value != c1_or_crit_ext_opts::c1 or
      rrchocmd.crit_exts.c1().type().value != ho_cmd_s::crit_exts_c_::c1_c_::types_opts::ho_cmd_r8) {
    procError("Only handling r8 Handover Commands\n");
  }

  /* unpack DL-DCCH message containing the RRCRonnectionReconf (with MobilityInfo) to be sent to the UE */
  asn1::rrc::dl_dcch_msg_s dl_dcch_msg;
  {
    asn1::cbit_ref bref(&rrchocmd.crit_exts.c1().ho_cmd_r8().ho_cmd_msg[0],
                        rrchocmd.crit_exts.c1().ho_cmd_r8().ho_cmd_msg.size());
    if (dl_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS) {
      procError("Unpacking of RRC DL-DCCH message with HO Command was unsuccessful.\n");
      return srslte::proc_outcome_t::error;
    }
  }
  if (dl_dcch_msg.msg.type().value != dl_dcch_msg_type_c::types_opts::c1 or
      dl_dcch_msg.msg.c1().type().value != dl_dcch_msg_type_c::c1_c_::types_opts::rrc_conn_recfg) {
    procError("HandoverCommand is expected to contain an RRC Connection Reconf message inside\n");
    return srslte::proc_outcome_t::error;
  }
  asn1::rrc::rrc_conn_recfg_s& reconf = dl_dcch_msg.msg.c1().rrc_conn_recfg();
  if (not reconf.crit_exts.c1().rrc_conn_recfg_r8().mob_ctrl_info_present) {
    procWarning("HandoverCommand is expected to have mobility control subfield\n");
    return srslte::proc_outcome_t::error;
  }

  // TODO: Do anything with MeasCfg info within the Msg (e.g. update ue_var_meas)?

  /* Send HO Command to UE */
  parent->rrc_ue->send_dl_dcch(&dl_dcch_msg);
  procInfo("HandoverCommand of rnti=0x%x handled successfully.\n", parent->rrc_ue->rnti);
  state = state_t::ho_execution;

  /* Start S1AP eNBStatusTransfer Procedure */
  if (not parent->start_enb_status_transfer()) {
    return srslte::proc_outcome_t::error;
  }

  return srslte::proc_outcome_t::success;
}

} // namespace srsenb
