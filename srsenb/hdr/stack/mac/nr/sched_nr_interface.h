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
#include "srsran/adt/span.h"
#include "srsran/common/tti_point.h"
#include "srsran/interfaces/rrc_nr_interface_types.h"
#include "srsran/phy/phch/dci_nr.h"

namespace srsenb {

const static size_t   SCHED_NR_MAX_CARRIERS     = 4;
const static uint16_t SCHED_NR_INVALID_RNTI     = 0;
const static size_t   SCHED_NR_MAX_PDSCH_DATA   = 16;
const static size_t   SCHED_NR_MAX_NOF_RBGS     = 25;
const static size_t   SCHED_NR_MAX_UL_ALLOCS    = 16;
const static size_t   SCHED_NR_MAX_TB           = 1;
const static size_t   SCHED_NR_MAX_HARQ         = 16;
const static size_t   SCHED_NR_MAX_BWP_PER_CELL = 1;

class sched_nr_interface
{
public:
  using pdcch_bitmap = srsran::bounded_bitset<SCHED_NR_MAX_NOF_RBGS, true>;
  using rbg_bitmap   = srsran::bounded_bitset<SCHED_NR_MAX_NOF_RBGS, true>;

  ///// Configuration /////

  struct pdsch_td_res_alloc {
    uint8_t k0 = 0; // 0..32
    uint8_t k1 = 4; // 0..32
  };
  using pdsch_td_res_alloc_list = srsran::bounded_vector<pdsch_td_res_alloc, SCHED_NR_MAX_UL_ALLOCS>;
  struct pusch_td_res_alloc {
    uint8_t k2 = 4; // 0..32
  };
  using pusch_td_res_alloc_list = srsran::bounded_vector<pusch_td_res_alloc, SCHED_NR_MAX_UL_ALLOCS>;

  struct bwp_cfg_t {
    uint32_t start_rb = 0;
    uint32_t rb_width = 100;
  };

  struct cell_cfg_t {
    uint32_t                                                     nof_prb = 100;
    uint32_t                                                     nof_rbg = 25;
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

  ///// Sched Result /////

  struct pdsch_grant {
    srsran_dci_dl_nr_t dci;
    rbg_bitmap         bitmap;
  };
  using pdsch_list = srsran::bounded_vector<pdsch_grant, SCHED_NR_MAX_PDSCH_DATA>;

  struct dl_tti_request_t {
    tti_point  pdsch_tti;
    pdsch_list pdsch;
  };

  struct pusch_grant {
    srsran_dci_ul_nr_t dci;
    rbg_bitmap         bitmap;
  };
  using pusch_list = srsran::bounded_vector<pusch_grant, SCHED_NR_MAX_PDSCH_DATA>;

  struct ul_tti_request_t {
    tti_point                                                   pusch_tti;
    srsran::bounded_vector<pusch_grant, SCHED_NR_MAX_UL_ALLOCS> pusch;
  };

  struct tti_request_t {
    dl_tti_request_t dl_res;
    ul_tti_request_t ul_res;
  };

  virtual ~sched_nr_interface()                                                            = default;
  virtual int  cell_cfg(srsran::const_span<sched_nr_interface::cell_cfg_t> ue_cfg)         = 0;
  virtual void ue_cfg(uint16_t rnti, const ue_cfg_t& ue_cfg)                               = 0;
  virtual void slot_indication(tti_point tti_rx)                                           = 0;
  virtual int  generate_sched_result(tti_point tti_rx, uint32_t cc, tti_request_t& result) = 0;

  virtual void dl_ack_info(uint16_t rnti, uint32_t cc, uint32_t pid, uint32_t tb_idx, bool ack) = 0;
  virtual void ul_sr_info(tti_point, uint16_t rnti)                                             = 0;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_INTERFACE_H
