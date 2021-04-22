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

#ifndef SRSRAN_RRC_CELL_CFG_H
#define SRSRAN_RRC_CELL_CFG_H

#include "rrc_config.h"
#include "srsran/common/byte_buffer.h"
#include "srsran/srslog/srslog.h"

namespace srsenb {

/// Storage of cell-specific eNB config and derived params
struct enb_cell_common {
  uint32_t                                  enb_cc_idx = 0;
  asn1::rrc::mib_s                          mib;
  asn1::rrc::sib_type1_s                    sib1;
  asn1::rrc::sib_type2_s                    sib2;
  const cell_cfg_t&                         cell_cfg;
  std::vector<srsran::unique_byte_buffer_t> sib_buffer; ///< Packed SIBs for given CC
  std::vector<const enb_cell_common*>       scells;

  enb_cell_common(uint32_t idx_, const cell_cfg_t& cfg) : enb_cc_idx(idx_), cell_cfg(cfg) {}
};

class enb_cell_common_list
{
public:
  explicit enb_cell_common_list(const rrc_cfg_t& cfg_);

  enb_cell_common*       get_cc_idx(uint32_t enb_cc_idx) { return cell_list[enb_cc_idx].get(); }
  const enb_cell_common* get_cc_idx(uint32_t enb_cc_idx) const { return cell_list[enb_cc_idx].get(); }
  const enb_cell_common* get_cell_id(uint32_t cell_id) const;
  const enb_cell_common* get_pci(uint32_t pci) const;
  size_t                 nof_cells() const { return cell_list.size(); }

  // container interface
  using value_type     = std::unique_ptr<enb_cell_common>;
  using iterator       = std::vector<value_type>::iterator;
  using const_iterator = const std::vector<value_type>::const_iterator;
  iterator       begin() { return cell_list.begin(); }
  iterator       end() { return cell_list.end(); }
  const_iterator begin() const { return cell_list.begin(); }
  const_iterator end() const { return cell_list.end(); }
  size_t         size() const { return cell_list.size(); }

private:
  const rrc_cfg_t&                               cfg;
  std::vector<std::unique_ptr<enb_cell_common> > cell_list;
};

// Helper methods
std::vector<const enb_cell_common*> get_cfg_intraenb_scells(const enb_cell_common_list& list,
                                                            uint32_t                    pcell_enb_cc_idx);
std::vector<uint32_t>               get_measobj_earfcns(const enb_cell_common& pcell);

class cell_res_common
{
public:
  struct pucch_idx_sched_t {
    uint32_t nof_users[100][80];
  };

  const static uint32_t N_PUCCH_MAX_PRB = 4; // Maximum number of PRB to use for PUCCH ACK/NACK in CS mode
  const static uint32_t N_PUCCH_MAX_RES = 3 * SRSRAN_NRE * N_PUCCH_MAX_PRB;

  uint32_t                          next_measgap_offset = 0;
  pucch_idx_sched_t                 sr_sched            = {};
  pucch_idx_sched_t                 cqi_sched           = {};
  std::array<bool, N_PUCCH_MAX_RES> n_pucch_cs_used     = {};
};

/** Storage of CQI/SR/PUCCH CS resources across multiple frequencies and for multiple users */
class freq_res_common_list
{
public:
  explicit freq_res_common_list(const rrc_cfg_t& cfg_);

  cell_res_common* get_earfcn(uint32_t earfcn);

private:
  const rrc_cfg_t&                    cfg;
  std::map<uint32_t, cell_res_common> pucch_res_list;
};

/** Class used to store all the resources specific to a UE's cell */
struct ue_cell_ded {
  uint32_t               ue_cc_idx;
  const enb_cell_common* cell_common;
  bool                   cqi_res_present = false;
  struct cqi_res_t {
    uint32_t pmi_idx   = 0;
    uint32_t pucch_res = 0;
    uint32_t prb_idx   = 0;
    uint32_t sf_idx    = 0;
  } cqi_res;
  uint32_t meas_gap_period = 0;
  uint32_t meas_gap_offset = 0;

  explicit ue_cell_ded(uint32_t i_, const enb_cell_common& c_) : ue_cc_idx(i_), cell_common(&c_) {}

  // forbid copying to not break counting of pucch allocated resources
  ue_cell_ded(const ue_cell_ded&)     = delete;
  ue_cell_ded(ue_cell_ded&&) noexcept = default;
  ue_cell_ded& operator=(const ue_cell_ded&) = delete;
  ue_cell_ded& operator=(ue_cell_ded&&) noexcept = default;

  uint32_t get_dl_earfcn() const { return cell_common->cell_cfg.dl_earfcn; }
  uint32_t get_pci() const { return cell_common->cell_cfg.pci; }
};

/** Class used to handle the allocation of a UE's resources across its cells */
class ue_cell_ded_list
{
public:
  explicit ue_cell_ded_list(const rrc_cfg_t&            cfg_,
                            freq_res_common_list&       cell_res_list_,
                            const enb_cell_common_list& enb_common_list);
  ~ue_cell_ded_list();

  ue_cell_ded* add_cell(uint32_t enb_cc_idx);
  bool         rem_last_cell();
  bool         set_cells(const std::vector<uint32_t>& enb_cc_idxs);

  ue_cell_ded* get_ue_cc_idx(uint32_t ue_cc_idx) { return (ue_cc_idx < nof_cells()) ? &cell_list[ue_cc_idx] : nullptr; }
  const ue_cell_ded* get_ue_cc_idx(uint32_t ue_cc_idx) const
  {
    return (ue_cc_idx < nof_cells()) ? &cell_list[ue_cc_idx] : nullptr;
  }
  ue_cell_ded*       get_enb_cc_idx(uint32_t enb_cc_idx);
  const ue_cell_ded* find_cell(uint32_t earfcn, uint32_t pci) const;
  size_t             nof_cells() const { return cell_list.size(); }
  bool               is_allocated() const { return nof_cells() > 0; }

  // container interface
  using iterator       = std::vector<ue_cell_ded>::iterator;
  using const_iterator = std::vector<ue_cell_ded>::const_iterator;
  iterator       begin() { return cell_list.begin(); }
  iterator       end() { return cell_list.end(); }
  const_iterator begin() const { return cell_list.begin(); }
  const_iterator end() const { return cell_list.end(); }
  size_t         size() const { return cell_list.size(); }

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

  srslog::basic_logger&       logger;
  const rrc_cfg_t&            cfg;
  const enb_cell_common_list& common_list;
  freq_res_common_list&       cell_res_list;

  cell_res_common*         pucch_res = nullptr;
  std::vector<ue_cell_ded> cell_list;
  bool                     sr_res_present     = false;
  bool                     n_pucch_cs_present = false;
  sr_res_t                 sr_res             = {};
  uint16_t                 n_pucch_cs_idx     = 0;
};

} // namespace srsenb

#endif // SRSRAN_RRC_CELL_CFG_H
