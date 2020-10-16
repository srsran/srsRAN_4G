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

#ifndef SRSLTE_RRC_CELL_CFG_H
#define SRSLTE_RRC_CELL_CFG_H

#include "rrc_config.h"
#include "srslte/common/logmap.h"

namespace srsenb {

class pucch_res_common
{
public:
  struct pucch_idx_sched_t {
    uint32_t nof_users[100][80];
  };

  const static uint32_t             N_PUCCH_MAX_PRB = 4; // Maximum number of PRB to use for PUCCH ACK/NACK in CS mode
  const static uint32_t             N_PUCCH_MAX_RES = 3 * SRSLTE_NRE * N_PUCCH_MAX_PRB;
  pucch_idx_sched_t                 sr_sched        = {};
  pucch_idx_sched_t                 cqi_sched       = {};
  std::array<bool, N_PUCCH_MAX_RES> n_pucch_cs_used = {};
};

/** Storage of CQI/SR/PUCCH CS resources across multiple frequencies and for multiple users */
class freq_res_common_list
{
public:
  explicit freq_res_common_list(const rrc_cfg_t& cfg_);

  pucch_res_common* get_earfcn(uint32_t earfcn);

private:
  const rrc_cfg_t&                     cfg;
  std::map<uint32_t, pucch_res_common> pucch_res_list;
};

/** Storage of cell-specific eNB config and derived params */
struct cell_info_common {
  uint32_t                                  enb_cc_idx = 0;
  asn1::rrc::mib_s                          mib;
  asn1::rrc::sib_type1_s                    sib1;
  asn1::rrc::sib_type2_s                    sib2;
  const cell_cfg_t&                         cell_cfg;
  std::vector<srslte::unique_byte_buffer_t> sib_buffer; ///< Packed SIBs for given CC
  std::vector<const cell_info_common*>      scells;

  cell_info_common(uint32_t idx_, const cell_cfg_t& cfg) : enb_cc_idx(idx_), cell_cfg(cfg) {}
};

class cell_info_common_list
{
public:
  explicit cell_info_common_list(const rrc_cfg_t& cfg_);

  cell_info_common*       get_cc_idx(uint32_t enb_cc_idx) { return cell_list[enb_cc_idx].get(); }
  const cell_info_common* get_cc_idx(uint32_t enb_cc_idx) const { return cell_list[enb_cc_idx].get(); }
  const cell_info_common* get_cell_id(uint32_t cell_id) const;
  const cell_info_common* get_pci(uint32_t pci) const;
  size_t                  nof_cells() const { return cell_list.size(); }

private:
  const rrc_cfg_t&                                cfg;
  std::vector<std::unique_ptr<cell_info_common> > cell_list;
};

// Helper methods
std::vector<const cell_info_common*> get_cfg_intraenb_scells(const cell_info_common_list& list,
                                                             uint32_t                     pcell_enb_cc_idx);
std::vector<uint32_t>                get_measobj_earfcns(const cell_info_common& pcell);

/** Class used to store all the resources specific to a UE's cell */
struct cell_ctxt_dedicated {
  uint32_t                ue_cc_idx;
  const cell_info_common* cell_common;
  bool                    cqi_res_present = false;
  struct cqi_res_t {
    uint32_t pmi_idx   = 0;
    uint32_t pucch_res = 0;
    uint32_t prb_idx   = 0;
    uint32_t sf_idx    = 0;
  } cqi_res;

  explicit cell_ctxt_dedicated(uint32_t i_, const cell_info_common& c_) : ue_cc_idx(i_), cell_common(&c_) {}

  // forbid copying to not break counting of pucch allocated resources
  cell_ctxt_dedicated(const cell_ctxt_dedicated&)     = delete;
  cell_ctxt_dedicated(cell_ctxt_dedicated&&) noexcept = default;
  cell_ctxt_dedicated& operator=(const cell_ctxt_dedicated&) = delete;
  cell_ctxt_dedicated& operator=(cell_ctxt_dedicated&&) noexcept = default;

  uint32_t get_dl_earfcn() const { return cell_common->cell_cfg.dl_earfcn; }
};

/** Class used to handle the allocation of a UE's resources across its cells */
class cell_ctxt_dedicated_list
{
public:
  explicit cell_ctxt_dedicated_list(const rrc_cfg_t&             cfg_,
                                    freq_res_common_list&        pucch_res_list_,
                                    const cell_info_common_list& enb_common_list);
  ~cell_ctxt_dedicated_list();

  cell_ctxt_dedicated* add_cell(uint32_t enb_cc_idx);
  bool                 rem_last_cell();
  bool                 set_cells(const std::vector<uint32_t>& enb_cc_idxs);

  cell_ctxt_dedicated* get_ue_cc_idx(uint32_t ue_cc_idx)
  {
    return (ue_cc_idx < nof_cells()) ? &cell_ded_list[ue_cc_idx] : nullptr;
  }
  const cell_ctxt_dedicated* get_ue_cc_idx(uint32_t ue_cc_idx) const
  {
    return (ue_cc_idx < nof_cells()) ? &cell_ded_list[ue_cc_idx] : nullptr;
  }
  cell_ctxt_dedicated* get_enb_cc_idx(uint32_t enb_cc_idx);
  size_t               nof_cells() const { return cell_ded_list.size(); }
  bool                 is_allocated() const { return nof_cells() > 0; }

  using iterator = std::vector<cell_ctxt_dedicated>::iterator;
  iterator begin() { return cell_ded_list.begin(); }
  iterator end() { return cell_ded_list.end(); }

  struct sr_res_t {
    int      sr_sched_sf_idx  = 0;
    int      sr_sched_prb_idx = 0;
    uint32_t sr_N_pucch       = 0;
    uint32_t sr_I             = 0;
  };

  const sr_res_t* get_sr_res() const { return sr_res_present ? &sr_res : nullptr; }
  const uint16_t* get_n_pucch_cs() const { return n_pucch_cs_present ? &n_pucch_cs_idx : nullptr; }
  bool            is_pucch_cs_allocated() const { return n_pucch_cs_present; }

private:
  bool alloc_cell_resources(uint32_t ue_cc_idx);
  bool alloc_cqi_resources(uint32_t ue_cc_idx, uint32_t period);
  bool dealloc_cqi_resources(uint32_t ue_cc_idx);
  bool alloc_sr_resources(uint32_t period);
  bool dealloc_sr_resources();
  bool alloc_pucch_cs_resources();
  bool dealloc_pucch_cs_resources();

  srslte::log_ref              log_h{"RRC"};
  const rrc_cfg_t&             cfg;
  const cell_info_common_list& common_list;
  freq_res_common_list&        pucch_res_list;

  pucch_res_common*                pucch_res = nullptr;
  std::vector<cell_ctxt_dedicated> cell_ded_list;
  bool                             sr_res_present     = false;
  bool                             n_pucch_cs_present = false;
  sr_res_t                         sr_res             = {};
  uint16_t                         n_pucch_cs_idx     = 0;
};

} // namespace srsenb

#endif // SRSLTE_RRC_CELL_CFG_H
