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

freq_res_common_list::freq_res_common_list(const rrc_cfg_t& cfg_) : cfg(cfg_)
{
  for (auto& c : cfg.cell_list) {
    auto it = pucch_res_list.find(c.dl_earfcn);
    if (it == pucch_res_list.end()) {
      pucch_res_list[c.dl_earfcn] = {};
    }
  }
}

pucch_res_common* freq_res_common_list::get_earfcn(uint32_t earfcn)
{
  auto it = pucch_res_list.find(earfcn);
  return (it == pucch_res_list.end()) ? nullptr : &(it->second);
}

/*************************
 *    cell ctxt common
 ************************/

cell_info_common_list::cell_info_common_list(const rrc_cfg_t& cfg_) : cfg(cfg_)
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

const cell_info_common* cell_info_common_list::get_cell_id(uint32_t cell_id) const
{
  auto it = std::find_if(cell_list.begin(), cell_list.end(), [cell_id](const cell_info_common& c) {
    return c.cell_cfg.cell_id == cell_id;
  });
  return it == cell_list.end() ? nullptr : &(*it);
}

const cell_info_common* cell_info_common_list::get_pci(uint32_t pci) const
{
  auto it = std::find_if(
      cell_list.begin(), cell_list.end(), [pci](const cell_info_common& c) { return c.cell_cfg.pci == pci; });
  return it == cell_list.end() ? nullptr : &(*it);
}

/*************************
 *  cell ctxt dedicated
 ************************/

cell_ctxt_dedicated_list::cell_ctxt_dedicated_list(const rrc_cfg_t&             cfg_,
                                                   freq_res_common_list&        pucch_res_list_,
                                                   const cell_info_common_list& enb_common_list) :
  cfg(cfg_),
  pucch_res_list(pucch_res_list_),
  common_list(enb_common_list)
{
  cell_ded_list.reserve(common_list.nof_cells());
}

cell_ctxt_dedicated_list::~cell_ctxt_dedicated_list()
{
  for (auto& c : cell_ded_list) {
    dealloc_cqi_resources(c.ue_cc_idx);
  }
  dealloc_sr_resources();
  dealloc_pucch_cs_resources();
}

cell_ctxt_dedicated* cell_ctxt_dedicated_list::get_enb_cc_idx(uint32_t enb_cc_idx)
{
  auto it = std::find_if(cell_ded_list.begin(), cell_ded_list.end(), [enb_cc_idx](const cell_ctxt_dedicated& c) {
    return c.cell_common.enb_cc_idx == enb_cc_idx;
  });
  return it == cell_ded_list.end() ? nullptr : &(*it);
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

  uint32_t ue_cc_idx = cell_ded_list.size();

  if (ue_cc_idx == UE_PCELL_CC_IDX) {
    // Fetch PUCCH resources if it's pcell
    pucch_res = pucch_res_list.get_earfcn(cell_common->cell_cfg.dl_earfcn);
  }

  cell_ded_list.emplace_back(cell_ded_list.size(), *cell_common);

  // Allocate CQI, SR, and PUCCH CS resources. If failure, do not add new cell
  if (ue_cc_idx == UE_PCELL_CC_IDX) {
    if (not alloc_sr_resources(cfg.sr_cfg.period)) {
      log_h->error("Failed to allocate SR resources for PCell\n");
      cell_ded_list.pop_back();
      return nullptr;
    }
    if (cfg.cell_list.size() == 2) {
      // Allocate resources for Format1b CS (will be optional PUCCH3/CS)
      if (not alloc_pucch_cs_resources()) {
        log_h->error("Error allocating PUCCH Format1b CS resource for SCell\n");
        cell_ded_list.pop_back();
        return nullptr;
      }
    }
  }
  if (not alloc_cqi_resources(ue_cc_idx, cfg.cqi_cfg.period)) {
    log_h->error("Failed to allocate CQIresources for cell enb_cc_idx=%d\n", enb_cc_idx);
    cell_ded_list.pop_back();
    return nullptr;
  }

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
  delta_pucch_shift             = SRSLTE_MAX(1, delta_pucch_shift);
  uint32_t max_users            = 12 * c / delta_pucch_shift;

  // Allocate all CQI resources for all carriers now
  // Find freq-time resources with least number of users
  int      i_min = 0, j_min = 0;
  uint32_t min_users = std::numeric_limits<uint32_t>::max();
  for (uint32_t i = 0; i < cfg.cqi_cfg.nof_prb; i++) {
    for (uint32_t j = 0; j < cfg.cqi_cfg.nof_subframes; j++) {
      if (pucch_res->cqi_sched.nof_users[i][j] < min_users) {
        i_min     = i;
        j_min     = j;
        min_users = pucch_res->cqi_sched.nof_users[i][j];
      }
    }
  }
  if (pucch_res->cqi_sched.nof_users[i_min][j_min] > max_users) {
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
  uint16_t n_pucch = i_min * max_users + pucch_res->cqi_sched.nof_users[i_min][j_min];
  if (pcell_pucch_cfg.ncs_an) {
    n_pucch += pcell_pucch_cfg.ncs_an;
  }

  cell->cqi_res_present   = true;
  cell->cqi_res.pmi_idx   = pmi_idx;
  cell->cqi_res.pucch_res = n_pucch;
  cell->cqi_res.prb_idx   = i_min;
  cell->cqi_res.sf_idx    = j_min;

  pucch_res->cqi_sched.nof_users[i_min][j_min]++;

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

  if (pucch_res->cqi_sched.nof_users[c->cqi_res.prb_idx][c->cqi_res.sf_idx] > 0) {
    pucch_res->cqi_sched.nof_users[c->cqi_res.prb_idx][c->cqi_res.sf_idx]--;
    log_h->info("Deallocated CQI resources for time-frequency slot (%d, %d)\n", c->cqi_res.prb_idx, c->cqi_res.sf_idx);
  } else {
    log_h->warning(
        "Removing CQI resources: no users in time-frequency slot (%d, %d)\n", c->cqi_res.prb_idx, c->cqi_res.sf_idx);
  }
  c->cqi_res_present = false;
  return true;
}

bool cell_ctxt_dedicated_list::alloc_sr_resources(uint32_t period)
{
  cell_ctxt_dedicated* cell = get_ue_cc_idx(UE_PCELL_CC_IDX);
  if (cell == nullptr) {
    log_h->error("The user cell pcell has not been allocated\n");
    return false;
  }
  if (cell->sr_res_present) {
    log_h->error("The user sr resources are already allocated\n");
    return false;
  }

  uint32_t c                 = SRSLTE_CP_ISNORM(cfg.cell.cp) ? 3 : 2;
  uint32_t delta_pucch_shift = cell->cell_common.sib2.rr_cfg_common.pucch_cfg_common.delta_pucch_shift.to_number();
  delta_pucch_shift          = SRSLTE_MAX(1, delta_pucch_shift); // prevent div by zero
  uint32_t max_users         = 12 * c / delta_pucch_shift;

  // Find freq-time resources with least number of users
  int      i_min = 0, j_min = 0;
  uint32_t min_users = std::numeric_limits<uint32_t>::max();
  for (uint32_t i = 0; i < cfg.sr_cfg.nof_prb; i++) {
    for (uint32_t j = 0; j < cfg.sr_cfg.nof_subframes; j++) {
      if (pucch_res->sr_sched.nof_users[i][j] < min_users) {
        i_min     = i;
        j_min     = j;
        min_users = pucch_res->sr_sched.nof_users[i][j];
      }
    }
  }

  if (pucch_res->sr_sched.nof_users[i_min][j_min] > max_users) {
    log_h->error("Not enough PUCCH resources to allocate Scheduling Request\n");
    return false;
  }

  // Compute I_sr
  if (period != 5 && period != 10 && period != 20 && period != 40 && period != 80) {
    log_h->error("Invalid SchedulingRequest period %d ms\n", period);
    return false;
  }
  if (cfg.sr_cfg.sf_mapping[j_min] < period) {
    sr_res.sr_I = period - 5 + cfg.sr_cfg.sf_mapping[j_min];
  } else {
    log_h->error("Allocating SR: invalid sf_idx=%d for period=%d\n", cfg.sr_cfg.sf_mapping[j_min], period);
    return false;
  }

  // Compute N_pucch_sr
  sr_res.sr_N_pucch = i_min * max_users + pucch_res->sr_sched.nof_users[i_min][j_min];
  if (cell->cell_common.sib2.rr_cfg_common.pucch_cfg_common.ncs_an) {
    sr_res.sr_N_pucch += cell->cell_common.sib2.rr_cfg_common.pucch_cfg_common.ncs_an;
  }

  // Allocate user
  pucch_res->sr_sched.nof_users[i_min][j_min]++;
  sr_res.sr_sched_prb_idx = i_min;
  sr_res.sr_sched_sf_idx  = j_min;
  sr_res_present          = true;

  return true;
}

bool cell_ctxt_dedicated_list::dealloc_sr_resources()
{
  if (sr_res_present) {
    if (pucch_res->sr_sched.nof_users[sr_res.sr_sched_prb_idx][sr_res.sr_sched_sf_idx] > 0) {
      pucch_res->sr_sched.nof_users[sr_res.sr_sched_prb_idx][sr_res.sr_sched_sf_idx]--;
    } else {
      log_h->warning("Removing SR resources: no users in time-frequency slot (%d, %d)\n",
                     sr_res.sr_sched_prb_idx,
                     sr_res.sr_sched_sf_idx);
    }
    log_h->info(
        "Deallocated SR resources for time-frequency slot (%d, %d)\n", sr_res.sr_sched_prb_idx, sr_res.sr_sched_sf_idx);
    sr_res_present = false;
    return true;
  }
  return false;
}

bool cell_ctxt_dedicated_list::alloc_pucch_cs_resources()
{
  cell_ctxt_dedicated* cell = get_ue_cc_idx(UE_PCELL_CC_IDX);
  if (cell == nullptr) {
    log_h->error("The user cell pcell has not been allocated\n");
    return false;
  }
  if (cell->sr_res_present) {
    log_h->error("The user sr resources are already allocated\n");
    return false;
  }

  const sib_type2_s& sib2      = cell->cell_common.sib2;
  const uint16_t     N_pucch_1 = sib2.rr_cfg_common.pucch_cfg_common.n1_pucch_an;
  const uint32_t     max_cce   = srslte_max_cce(cfg.cell.nof_prb);
  // Loop through all available resources
  for (uint32_t i = 0; i < pucch_res_common::N_PUCCH_MAX_RES; i++) {
    if (!pucch_res->n_pucch_cs_used[i] && !(i >= N_pucch_1 && i < N_pucch_1 + max_cce)) {
      // Allocate resource
      pucch_res->n_pucch_cs_used[i] = true;
      n_pucch_cs_idx                = i;
      n_pucch_cs_present            = true;
      log_h->info("Allocated N_pucch_cs=%d\n", n_pucch_cs_idx);
      return true;
    }
  }
  log_h->warning("Could not allocated N_pucch_cs\n");
  return false;
}

bool cell_ctxt_dedicated_list::dealloc_pucch_cs_resources()
{
  if (n_pucch_cs_present) {
    pucch_res->n_pucch_cs_used[n_pucch_cs_idx] = false;
    n_pucch_cs_present                         = false;
    log_h->info("Deallocated N_pucch_cs=%d\n", n_pucch_cs_idx);
    return true;
  }
  return false;
}

} // namespace srsenb
