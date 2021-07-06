/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_SCHED_NR_CFG_H
#define SRSRAN_SCHED_NR_CFG_H

#include "sched_nr_interface.h"
#include "srsran/adt/bounded_bitset.h"

namespace srsenb {

const static size_t SCHED_NR_MAX_USERS     = 4;
const static size_t SCHED_NR_NOF_SUBFRAMES = 10;
const static size_t SCHED_NR_NOF_HARQS     = 16;
static const size_t MAX_NOF_AGGR_LEVELS    = 5;

namespace sched_nr_impl {

const static size_t MAX_GRANTS = sched_nr_interface::MAX_GRANTS;

using pucch_resource_grant = sched_nr_interface::pucch_resource_grant;
using pucch_grant          = sched_nr_interface::pucch_grant;
using pucch_list_t         = sched_nr_interface::pucch_list_t;

using sched_cfg_t = sched_nr_interface::sched_cfg_t;
using cell_cfg_t  = sched_nr_interface::cell_cfg_t;
using bwp_cfg_t   = sched_nr_interface::bwp_cfg_t;

struct sched_cell_params {
  const uint32_t     cc;
  const cell_cfg_t   cell_cfg;
  const sched_cfg_t& sched_cfg;

  sched_cell_params(uint32_t cc_, const cell_cfg_t& cell, const sched_cfg_t& sched_cfg_);
};

struct sched_params {
  const sched_cfg_t              sched_cfg;
  std::vector<sched_cell_params> cells;

  explicit sched_params(const sched_cfg_t& sched_cfg_);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using rbgmask_t = srsran::bounded_bitset<SCHED_NR_MAX_NOF_RBGS, true>;

using pdcchmask_t = srsran::bounded_bitset<SCHED_NR_MAX_NOF_RBGS, true>;

using pdcch_cce_pos_list = srsran::bounded_vector<uint32_t, SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR>;
using bwp_cce_pos_list   = std::array<std::array<pdcch_cce_pos_list, MAX_NOF_AGGR_LEVELS>, SRSRAN_NOF_SF_X_FRAME>;
void get_dci_locs(const srsran_coreset_t&      coreset,
                  const srsran_search_space_t& search_space,
                  uint16_t                     rnti,
                  bwp_cce_pos_list&            cce_locs);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ue_cfg_t    = sched_nr_interface::ue_cfg_t;
using ue_cc_cfg_t = sched_nr_interface::ue_cc_cfg_t;

class ue_cfg_extended : public ue_cfg_t
{
public:
  struct search_space_params {
    srsran_search_space_t* cfg = nullptr;
  };
  struct coreset_params {
    srsran_coreset_t*                 cfg = nullptr;
    std::vector<search_space_params*> ss_list;
    bwp_cce_pos_list                  cce_positions;
  };
  struct bwp_params {
    std::vector<search_space_params> search_spaces;
    std::vector<coreset_params>      coresets;
  };
  struct cc_params {
    srsran::bounded_vector<bwp_params, SCHED_NR_MAX_BWP_PER_CELL> bwps;
  };

  uint16_t               rnti;
  std::vector<cc_params> cc_params;

  ue_cfg_extended() = default;
  explicit ue_cfg_extended(uint16_t rnti, const ue_cfg_t& uecfg);
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
