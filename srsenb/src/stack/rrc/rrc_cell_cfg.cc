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

#include "srsenb/hdr/stack/rrc/rrc_cell_cfg.h"

using namespace asn1::rrc;

namespace srsenb {

cell_ctxt_common_list::cell_ctxt_common_list(const rrc_cfg_t& cfg_) : cfg(cfg_)
{
  cell_list.reserve(cfg.cell_list.size());

  // Store the SIB cfg of each carrier
  for (size_t ccidx = 0; ccidx < cfg.cell_list.size(); ++ccidx) {
    cell_list.emplace_back(ccidx, cfg.cell_list[ccidx]);
    auto& cell_ctxt = cell_list.back();

    // Set Cell MIB
    asn1::number_to_enum(cell_ctxt.mib.dl_bw, cfg.cell.nof_prb);
    cell_ctxt.mib.phich_cfg.phich_res.value = (phich_cfg_s::phich_res_opts::options)cfg.cell.phich_resources;
    cell_ctxt.mib.phich_cfg.phich_dur.value = (phich_cfg_s::phich_dur_opts::options)cfg.cell.phich_length;

    // Set Cell SIB1
    cell_ctxt.sib1 = cfg.sib1;
    // Update cellId
    sib_type1_s::cell_access_related_info_s_* cell_access = &cell_ctxt.sib1.cell_access_related_info;
    cell_access->cell_id.from_number((cfg.enb_id << 8u) + cell_ctxt.cell_cfg.cell_id);
    cell_access->tac.from_number(cell_ctxt.cell_cfg.tac);
    // Update DL EARFCN
    cell_ctxt.sib1.freq_band_ind = (uint8_t)srslte_band_get_band(cell_ctxt.cell_cfg.dl_earfcn);

    // Set Cell SIB2
    // update PRACH root seq index for this cell
    cell_ctxt.sib2                                      = cfg.sibs[1].sib2();
    cell_ctxt.sib2.rr_cfg_common.prach_cfg.root_seq_idx = cell_ctxt.cell_cfg.root_seq_idx;
    // update carrier freq
    if (cell_ctxt.sib2.freq_info.ul_carrier_freq_present) {
      cell_ctxt.sib2.freq_info.ul_carrier_freq = cell_ctxt.cell_cfg.ul_earfcn;
    }
  }
}

cell_ctxt_common* cell_ctxt_common_list::get_cell_id(uint32_t cell_id)
{
  auto it = std::find_if(cell_list.begin(), cell_list.end(), [cell_id](const cell_ctxt_common& c) {
    return c.cell_cfg.cell_id == cell_id;
  });
  return it == cell_list.end() ? nullptr : &(*it);
}

cell_ctxt_common* cell_ctxt_common_list::get_pci(uint32_t pci)
{
  auto it = std::find_if(
      cell_list.begin(), cell_list.end(), [pci](const cell_ctxt_common& c) { return c.cell_cfg.pci == pci; });
  return it == cell_list.end() ? nullptr : &(*it);
}

} // namespace srsenb
