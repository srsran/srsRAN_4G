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

#ifndef SRSRAN_SCHED_NR_INTERFACE_H
#define SRSRAN_SCHED_NR_INTERFACE_H

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/bounded_vector.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/span.h"
#include "srsran/common/phy_cfg_nr.h"
#include "srsran/common/tti_point.h"
#include "srsran/interfaces/gnb_interfaces.h"
#include "srsran/phy/phch/dci_nr.h"

namespace srsenb {

const static size_t   SCHED_NR_MAX_CARRIERS     = 4;
const static uint16_t SCHED_NR_INVALID_RNTI     = 0;
const static size_t   SCHED_NR_MAX_NOF_RBGS     = 17;
const static size_t   SCHED_NR_MAX_TB           = 1;
const static size_t   SCHED_NR_MAX_HARQ         = 16;
const static size_t   SCHED_NR_MAX_BWP_PER_CELL = 2;

class sched_nr_interface
{
public:
  using pdcch_bitmap = srsran::bounded_bitset<SCHED_NR_MAX_NOF_RBGS, true>;
  using rbg_bitmap   = srsran::bounded_bitset<SCHED_NR_MAX_NOF_RBGS, true>;

  static const size_t MAX_GRANTS = mac_interface_phy_nr::MAX_GRANTS;

  ///// Configuration /////

  struct pdsch_td_res_alloc {
    uint8_t k0 = 0; // 0..32
    uint8_t k1 = 4; // 0..32
  };
  using pdsch_td_res_alloc_list = srsran::bounded_vector<pdsch_td_res_alloc, MAX_GRANTS>;
  struct pusch_td_res_alloc {
    uint8_t k2 = 4; // 0..32
  };
  using pusch_td_res_alloc_list = srsran::bounded_vector<pusch_td_res_alloc, MAX_GRANTS>;

  struct bwp_cfg_t {
    uint32_t               bwp_id   = 1;
    uint32_t               start_rb = 0;
    uint32_t               rb_width = 100;
    srsran_sch_hl_cfg_nr_t pdsch    = {};
    srsran_sch_hl_cfg_nr_t pusch    = {};

    std::array<srsran::optional<srsran_coreset_t>, SRSRAN_UE_DL_NR_MAX_NOF_CORESET> coresets;
  };

  struct cell_cfg_t {
    uint32_t                                                     rar_window_size;
    uint32_t                                                     nof_prb = 100;
    uint32_t                                                     nof_rbg = 25;
    srsran_tdd_config_nr_t                                       tdd     = {};
    srsran::bounded_vector<bwp_cfg_t, SCHED_NR_MAX_BWP_PER_CELL> bwps{1};
  };

  struct sched_cfg_t {
    uint32_t nof_concurrent_subframes = 1;
  };

  struct ue_cc_cfg_t {
    bool                    active = false;
    pdsch_td_res_alloc_list pdsch_res_list{1};
    pusch_td_res_alloc_list pusch_res_list{1};
  };

  struct ue_cfg_t {
    uint32_t                                                   maxharq_tx = 4;
    srsran::bounded_vector<ue_cc_cfg_t, SCHED_NR_MAX_CARRIERS> carriers;
    srsran::phy_cfg_nr_t                                       phy_cfg = {};
  };

  ////// RACH //////

  struct dl_sched_rar_info_t {
    uint32_t preamble_idx;
    uint32_t ta_cmd;
    uint16_t temp_crnti;
    uint32_t msg3_size;
    uint32_t prach_tti;
  };

  ///// Sched Result /////

  using dl_sched_t = mac_interface_phy_nr::dl_sched_t;
  using ul_sched_t = mac_interface_phy_nr::ul_sched_t;

  struct pucch_resource_grant {
    uint16_t rnti;
    uint32_t resource_set_id;
    uint32_t resource_id;
  };
  struct pucch_grant {
    pucch_resource_grant resource;
  };
  using pucch_list_t = srsran::bounded_vector<pucch_grant, MAX_GRANTS>;

  virtual ~sched_nr_interface()                                                    = default;
  virtual int  cell_cfg(srsran::const_span<sched_nr_interface::cell_cfg_t> ue_cfg) = 0;
  virtual void ue_cfg(uint16_t rnti, const ue_cfg_t& ue_cfg)                       = 0;
  virtual int  get_dl_sched(tti_point tti_rx, uint32_t cc, dl_sched_t& result)     = 0;
  virtual int  get_ul_sched(tti_point tti_rx, uint32_t cc, ul_sched_t& result)     = 0;

  virtual void dl_ack_info(uint16_t rnti, uint32_t cc, uint32_t pid, uint32_t tb_idx, bool ack) = 0;
  virtual void ul_sr_info(tti_point, uint16_t rnti)                                             = 0;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_INTERFACE_H
