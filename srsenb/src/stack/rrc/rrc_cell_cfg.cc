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

/*************************
 *    cell ctxt common
 ************************/

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

const cell_info_common* cell_ctxt_common_list::get_cell_id(uint32_t cell_id) const
{
  auto it = std::find_if(cell_list.begin(), cell_list.end(), [cell_id](const cell_info_common& c) {
    return c.cell_cfg.cell_id == cell_id;
  });
  return it == cell_list.end() ? nullptr : &(*it);
}

const cell_info_common* cell_ctxt_common_list::get_pci(uint32_t pci) const
{
  auto it = std::find_if(
      cell_list.begin(), cell_list.end(), [pci](const cell_info_common& c) { return c.cell_cfg.pci == pci; });
  return it == cell_list.end() ? nullptr : &(*it);
}

/*************************
 *  cell ctxt dedicated
 ************************/

cell_ctxt_dedicated_list::cell_ctxt_dedicated_list(const rrc_cfg_t&             cfg_,
                                                   pucch_res_common&            pucch_res_,
                                                   const cell_ctxt_common_list& enb_common_list) :
  cfg(cfg_),
  pucch_res(pucch_res_),
  common_list(enb_common_list)
{
  cell_ded_list.reserve(common_list.nof_cells());
}

cell_ctxt_dedicated_list::~cell_ctxt_dedicated_list()
{
  for (auto& c : cell_ded_list) {
    dealloc_cqi_resources(c.ue_cc_idx);
  }
}

cell_ctxt_dedicated* cell_ctxt_dedicated_list::get_enb_cc_idx(uint32_t enb_cc_idx)
{
  auto it = std::find_if(cell_ded_list.begin(), cell_ded_list.end(), [enb_cc_idx](const cell_ctxt_dedicated& c) {
    return c.cell_common.enb_cc_idx == enb_cc_idx;
  });
  return it == cell_ded_list.end() ? nullptr : &(*it);
}

size_t cell_ctxt_dedicated_list::nof_cell_with_cqi_res() const
{
  return std::count_if(
      cell_ded_list.begin(), cell_ded_list.end(), [](const cell_ctxt_dedicated& c) { return c.cqi_res_present; });
}

cell_ctxt_dedicated* cell_ctxt_dedicated_list::add_cell(uint32_t enb_cc_idx)
{
  const cell_info_common* cell_common = common_list.get_cc_idx(enb_cc_idx);
  if (cell_common == nullptr) {
    log_h->error("cell with enb_cc_idx=%d does not exist.\n", enb_cc_idx);
    return nullptr;
  }
  cell_ctxt_dedicated* ret = get_enb_cc_idx(enb_cc_idx);
  if (ret != nullptr) {
    log_h->error("UE already registered cell %d\n", enb_cc_idx);
    return nullptr;
  }

  cell_ded_list.emplace_back(cell_ded_list.size(), *cell_common);
  return &cell_ded_list.back();
}

bool cell_ctxt_dedicated_list::alloc_cqi_resources(uint32_t ue_cc_idx, uint32_t period)
{
  cell_ctxt_dedicated* cell = get_ue_cc_idx(ue_cc_idx);
  if (cell == nullptr) {
    log_h->error("The user cell ue_cc_idx=%d has not been allocated\n", ue_cc_idx);
    return false;
  }
  if (cell->cqi_res_present) {
    log_h->error("The user cqi resources for cell ue_cc_idx=%d are already allocated\n", ue_cc_idx);
    return false;
  }

  const auto& pcell_pucch_cfg   = get_ue_cc_idx(UE_PCELL_CC_IDX)->cell_common.sib2.rr_cfg_common.pucch_cfg_common;
  uint32_t    c                 = SRSLTE_CP_ISNORM(cfg.cell.cp) ? 3 : 2;
  uint32_t    delta_pucch_shift = pcell_pucch_cfg.delta_pucch_shift.to_number();
  uint32_t    max_users         = 12 * c / delta_pucch_shift;

  // Allocate all CQI resources for all carriers now
  // Find freq-time resources with least number of users
  int      i_min = 0, j_min = 0;
  uint32_t min_users = std::numeric_limits<uint32_t>::max();
  for (uint32_t i = 0; i < cfg.cqi_cfg.nof_prb; i++) {
    for (uint32_t j = 0; j < cfg.cqi_cfg.nof_subframes; j++) {
      if (pucch_res.cqi_sched.nof_users[i][j] < min_users) {
        i_min     = i;
        j_min     = j;
        min_users = pucch_res.cqi_sched.nof_users[i][j];
      }
    }
  }
  if (pucch_res.cqi_sched.nof_users[i_min][j_min] > max_users) {
    log_h->error("Not enough PUCCH resources to allocate Scheduling Request\n");
    return false;
  }

  uint16_t pmi_idx = 0;

  // Compute I_sr
  if (period != 2 && period != 5 && period != 10 && period != 20 && period != 40 && period != 80 && period != 160 &&
      period != 32 && period != 64 && period != 128) {
    log_h->error("Invalid CQI Report period %d ms\n", period);
    return false;
  }
  if (cfg.cqi_cfg.sf_mapping[j_min] < period) {
    if (period != 32 && period != 64 && period != 128) {
      if (period > 2) {
        pmi_idx = period - 3 + cfg.cqi_cfg.sf_mapping[j_min];
      } else {
        pmi_idx = cfg.cqi_cfg.sf_mapping[j_min];
      }
    } else {
      if (period == 32) {
        pmi_idx = 318 + cfg.cqi_cfg.sf_mapping[j_min];
      } else if (period == 64) {
        pmi_idx = 350 + cfg.cqi_cfg.sf_mapping[j_min];
      } else {
        pmi_idx = 414 + cfg.cqi_cfg.sf_mapping[j_min];
      }
    }
  } else {
    log_h->error("Allocating CQI: invalid sf_idx=%d for period=%d\n", cfg.cqi_cfg.sf_mapping[j_min], period);
    return false;
  }

  // Compute n_pucch_2
  uint16_t n_pucch = i_min * max_users + pucch_res.cqi_sched.nof_users[i_min][j_min];
  if (pcell_pucch_cfg.ncs_an) {
    n_pucch += pcell_pucch_cfg.ncs_an;
  }

  cell->cqi_res_present   = true;
  cell->cqi_res.pmi_idx   = pmi_idx;
  cell->cqi_res.pucch_res = n_pucch;
  cell->cqi_res.prb_idx   = i_min;
  cell->cqi_res.sf_idx    = j_min;

  pucch_res.cqi_sched.nof_users[i_min][j_min]++;

  log_h->info("Allocated CQI resources for ue_cc_idx=%d, time-frequency slot (%d, %d), n_pucch_2=%d, pmi_cfg_idx=%d\n",
              ue_cc_idx,
              i_min,
              j_min,
              n_pucch,
              pmi_idx);
  return true;
}

bool cell_ctxt_dedicated_list::dealloc_cqi_resources(uint32_t ue_cc_idx)
{
  cell_ctxt_dedicated* c = get_ue_cc_idx(ue_cc_idx);
  if (c == nullptr or not c->cqi_res_present) {
    return false;
  }

  if (pucch_res.cqi_sched.nof_users[c->cqi_res.prb_idx][c->cqi_res.sf_idx] > 0) {
    pucch_res.cqi_sched.nof_users[c->cqi_res.prb_idx][c->cqi_res.sf_idx]--;
    log_h->info("Deallocated CQI resources for time-frequency slot (%d, %d)\n", c->cqi_res.prb_idx, c->cqi_res.sf_idx);
  } else {
    log_h->warning(
        "Removing CQI resources: no users in time-frequency slot (%d, %d)\n", c->cqi_res.prb_idx, c->cqi_res.sf_idx);
  }
  c->cqi_res_present = false;
  return true;
}

} // namespace srsenb
