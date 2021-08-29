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

#ifndef SRSRAN_SCHED_NR_CFG_H
#define SRSRAN_SCHED_NR_CFG_H

#include "sched_nr_interface.h"
#include "sched_nr_rb.h"

namespace srsenb {

const static size_t SCHED_NR_MAX_USERS     = 4;
const static size_t SCHED_NR_NOF_SUBFRAMES = 10;
const static size_t SCHED_NR_NOF_HARQS     = 16;
static const size_t MAX_NOF_AGGR_LEVELS    = 5;

namespace sched_nr_impl {

const static size_t MAX_GRANTS = sched_nr_interface::MAX_GRANTS;

using pucch_t      = mac_interface_phy_nr::pucch_t;
using pucch_list_t = srsran::bounded_vector<pucch_t, MAX_GRANTS>;
using pusch_t      = mac_interface_phy_nr::pusch_t;
using pusch_list_t = srsran::bounded_vector<pusch_t, MAX_GRANTS>;

using sched_cfg_t = sched_nr_interface::sched_cfg_t;
using cell_cfg_t  = sched_nr_interface::cell_cfg_t;
using bwp_cfg_t   = sched_nr_interface::bwp_cfg_t;

using pdcch_cce_pos_list = srsran::bounded_vector<uint32_t, SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR>;
using bwp_cce_pos_list   = std::array<std::array<pdcch_cce_pos_list, MAX_NOF_AGGR_LEVELS>, SRSRAN_NOF_SF_X_FRAME>;
void get_dci_locs(const srsran_coreset_t&      coreset,
                  const srsran_search_space_t& search_space,
                  uint16_t                     rnti,
                  bwp_cce_pos_list&            cce_locs);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct bwp_params {
  const uint32_t     bwp_id;
  const uint32_t     cc;
  const bwp_cfg_t&   cfg;
  const cell_cfg_t&  cell_cfg;
  const sched_cfg_t& sched_cfg;

  // derived params
  uint32_t P;
  uint32_t N_rbg;

  struct slot_cfg {
    bool is_dl;
    bool is_ul;
  };
  srsran::bounded_vector<slot_cfg, SRSRAN_NOF_SF_X_FRAME> slots;

  struct pusch_ra_time_cfg {
    uint32_t msg3_delay; ///< Includes K2 and delta. See TS 36.214 6.1.2.1.1-2/4/5
    uint32_t K;
    uint32_t S;
    uint32_t L;
  };
  std::vector<pusch_ra_time_cfg> pusch_ra_list;

  bwp_params(const cell_cfg_t& cell, const sched_cfg_t& sched_cfg_, uint32_t cc, uint32_t bwp_id);

  bwp_cce_pos_list rar_cce_list;
};

struct sched_cell_params {
  const uint32_t          cc;
  const cell_cfg_t        cell_cfg;
  const sched_cfg_t&      sched_cfg;
  std::vector<bwp_params> bwps;

  sched_cell_params(uint32_t cc_, const cell_cfg_t& cell, const sched_cfg_t& sched_cfg_);

  uint32_t nof_prb() const { return cell_cfg.carrier.nof_prb; }
};

struct sched_params {
  const sched_cfg_t              sched_cfg;
  std::vector<sched_cell_params> cells;

  explicit sched_params(const sched_cfg_t& sched_cfg_);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using prb_bitmap = srsran::bounded_bitset<SRSRAN_MAX_PRB_NR, true>;

using pdcchmask_t = srsran::bounded_bitset<SCHED_NR_MAX_NOF_RBGS, true>;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ue_cfg_t    = sched_nr_interface::ue_cfg_t;
using ue_cc_cfg_t = sched_nr_interface::ue_cc_cfg_t;

class bwp_ue_cfg
{
public:
  bwp_ue_cfg() = default;
  explicit bwp_ue_cfg(uint16_t rnti, const bwp_params& bwp_cfg, const ue_cfg_t& uecfg_);

  const ue_cfg_t*             ue_cfg() const { return cfg_; }
  const srsran::phy_cfg_nr_t& phy() const { return cfg_->phy_cfg; }
  const bwp_params&           active_bwp() const { return *bwp_cfg; }
  const bwp_cce_pos_list&     cce_pos_list(uint32_t search_id) const
  {
    return cce_positions_list[ss_id_to_cce_idx[search_id]];
  }

private:
  uint16_t          rnti    = SRSRAN_INVALID_RNTI;
  const ue_cfg_t*   cfg_    = nullptr;
  const bwp_params* bwp_cfg = nullptr;

  std::vector<bwp_cce_pos_list>                              cce_positions_list;
  std::array<uint32_t, SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE> ss_id_to_cce_idx;
};

class ue_cfg_extended : public ue_cfg_t
{
public:
  struct search_space_params {
    const srsran_search_space_t* cfg;
    bwp_cce_pos_list             cce_positions;
  };
  struct coreset_params {
    srsran_coreset_t*     cfg = nullptr;
    std::vector<uint32_t> ss_list;
  };
  struct bwp_params {
    std::array<srsran::optional<search_space_params>, SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE> ss_list;
    std::vector<coreset_params>                                                             coresets;
  };
  struct cc_params {
    srsran::bounded_vector<bwp_params, SCHED_NR_MAX_BWP_PER_CELL> bwps;
  };

  ue_cfg_extended() = default;
  explicit ue_cfg_extended(uint16_t rnti, const ue_cfg_t& uecfg);

  const bwp_cce_pos_list& get_dci_pos_list(uint32_t cc, uint32_t bwp_id, uint32_t search_space_id) const
  {
    return cc_params[cc].bwps[bwp_id].ss_list[search_space_id]->cce_positions;
  }

  uint16_t               rnti;
  std::vector<cc_params> cc_params;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct resource_guard {
public:
  resource_guard()                            = default;
  resource_guard(const resource_guard& other) = delete;
  resource_guard(resource_guard&& other)      = delete;
  resource_guard& operator=(const resource_guard& other) = delete;
  resource_guard& operator=(resource_guard&& other) = delete;
  bool            busy() const { return flag; }

  struct token {
    token() = default;
    token(resource_guard& parent) : flag(parent.busy() ? nullptr : &parent.flag)
    {
      if (flag != nullptr) {
        *flag = true;
      }
    }
    token(token&&) noexcept = default;
    token& operator=(token&&) noexcept = default;
    void   release() { flag.reset(); }
    bool   owns_token() const { return flag != nullptr; }
    bool   empty() const { return flag == nullptr; }

  private:
    struct release_deleter {
      void operator()(bool* ptr)
      {
        if (ptr != nullptr) {
          srsran_assert(*ptr == true, "resource token: detected inconsistency token state");
          *ptr = false;
        }
      }
    };
    std::unique_ptr<bool, release_deleter> flag;
  };

private:
  bool flag = false;
};

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_CFG_H
