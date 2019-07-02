/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSENB_SCHEDULER_H
#define SRSENB_SCHEDULER_H

#include "scheduler_grid.h"
#include "scheduler_harq.h"
#include "scheduler_ue.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/sched_interface.h"
#include <map>
#include <mutex>
#include <pthread.h>

namespace srsenb {


/* Caution: User addition (ue_cfg) and removal (ue_rem) are not thread-safe
 * Rest of operations are thread-safe
 *
 * The subclass sched_ue is thread-safe so that access to shared variables like buffer states
 * from scheduler thread and other threads is protected for each individual user.
 */

class sched : public sched_interface
{
public:
  // handle for DL metric
  class dl_tti_sched_t
  {
  public:
    virtual alloc_outcome_t  alloc_dl_user(sched_ue* user, const rbgmask_t& user_mask, uint32_t pid) = 0;
    virtual const rbgmask_t& get_dl_mask() const                                                     = 0;
    virtual uint32_t         get_tti_tx_dl() const                                                   = 0;
    virtual uint32_t         get_nof_ctrl_symbols() const                                            = 0;
    virtual bool             is_dl_alloc(sched_ue* user) const                                       = 0;
  };

  // handle for UL metric
  class ul_tti_sched_t
  {
  public:
    virtual alloc_outcome_t  alloc_ul_user(sched_ue* user, ul_harq_proc::ul_alloc_t alloc) = 0;
    virtual const prbmask_t& get_ul_mask() const                                           = 0;
    virtual uint32_t         get_tti_tx_ul() const                                         = 0;
    virtual bool             is_ul_alloc(sched_ue* user) const                             = 0;
  };

  /*************************************************************
   * 
   * Scheduling metric interface definition
   * 
   ************************************************************/

  class metric_dl
  {
  public: 

    /* Virtual methods for user metric calculation */
    virtual void set_log(srslte::log* log_)                                                  = 0;
    virtual void sched_users(std::map<uint16_t, sched_ue>& ue_db, dl_tti_sched_t* tti_sched) = 0;
  };

  
  class metric_ul
  {
  public: 
    /* Virtual methods for user metric calculation */
    virtual void set_log(srslte::log* log_)                                                  = 0;
    virtual void sched_users(std::map<uint16_t, sched_ue>& ue_db, ul_tti_sched_t* tti_sched) = 0;
  };

  /*************************************************************
   * 
   * FAPI-like Interface 
   * 
   ************************************************************/
  
  sched(); 
  ~sched();

  void init(rrc_interface_mac *rrc, srslte::log *log);
  void set_metric(metric_dl *dl_metric, metric_ul *ul_metric);
  int cell_cfg(cell_cfg_t *cell_cfg); 
  void set_sched_cfg(sched_args_t *sched_cfg);
  int reset();

  int ue_cfg(uint16_t rnti, ue_cfg_t *ue_cfg);
  int ue_rem(uint16_t rnti);
  bool ue_exists(uint16_t rnti); 
  
  void phy_config_enabled(uint16_t rnti, bool enabled); 
  
  int bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, ue_bearer_cfg_t *cfg); 
  int bearer_ue_rem(uint16_t rnti, uint32_t lc_id); 

  uint32_t get_ul_buffer(uint16_t rnti); 
  uint32_t get_dl_buffer(uint16_t rnti); 

  int dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue);
  int dl_mac_buffer_state(uint16_t rnti, uint32_t ce_code);

  int dl_ant_info(uint16_t rnti, asn1::rrc::phys_cfg_ded_s::ant_info_c_* dedicated);
  int dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t tb_idx, bool ack);
  int dl_rach_info(uint32_t tti, uint32_t ra_id, uint16_t rnti, uint32_t estimated_size); 
  int dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t ri_value);
  int dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t pmi_value);
  int dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi_value); 
  
  int ul_crc_info(uint32_t tti, uint16_t rnti, bool crc);
  int ul_sr_info(uint32_t tti, uint16_t rnti); 
  int ul_bsr(uint16_t rnti, uint32_t lcid, uint32_t bsr, bool set_value = true);
  int ul_recv_len(uint16_t rnti, uint32_t lcid, uint32_t len);
  int ul_phr(uint16_t rnti, int phr); 
  int ul_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi, uint32_t ul_ch_code);

  int dl_sched(uint32_t tti, dl_sched_res_t* sched_result) final;
  int ul_sched(uint32_t tti, ul_sched_res_t* sched_result) final;

  /* Custom functions
   */
  void set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs) final;
  void tpc_inc(uint16_t rnti); 
  void tpc_dec(uint16_t rnti);

  // Static Methods
  static uint32_t get_rvidx(uint32_t retx_idx) {
    const static int rv_idx[4] = {0, 2, 3, 1}; 
    return rv_idx[retx_idx%4]; 
  }
  static void generate_cce_location(
      srslte_regs_t* regs, sched_ue::sched_dci_cce_t* location, uint32_t cfi, uint32_t sf_idx = 0, uint16_t rnti = 0);
  static uint32_t aggr_level(uint32_t aggr_idx) { return 1u << aggr_idx; }

protected:
  metric_dl *dl_metric;
  metric_ul *ul_metric; 
  srslte::log *log_h; 
  rrc_interface_mac *rrc;

  pthread_rwlock_t rwlock;
  std::mutex       sched_mutex;

  cell_cfg_t cfg;
  sched_args_t sched_cfg; 

  // This is for computing DCI locations
  srslte_regs_t regs;

  typedef struct {
    int buf_rar; 
    uint16_t rnti; 
    uint32_t ra_id; 
    uint32_t rar_tti;    
  } sched_rar_t; 
  
  typedef struct {
    bool is_in_window;
    uint32_t window_start;
    uint32_t n_tx;
  } sched_sib_t;

  class tti_sched_t : public dl_tti_sched_t, public ul_tti_sched_t
  {
  public:
    struct ctrl_alloc_t {
      size_t      dci_idx;
      rbg_range_t rbg_range;
      uint16_t     rnti;
      uint32_t     req_bytes;
      alloc_type_t alloc_type;
    };
    struct rar_alloc_t : public ctrl_alloc_t {
      dl_sched_rar_t rar_grant;
      rar_alloc_t() = default;
      explicit rar_alloc_t(const ctrl_alloc_t& c) : ctrl_alloc_t(c) {}
    };
    struct bc_alloc_t : public ctrl_alloc_t {
      uint32_t       rv = 0;
      uint32_t       sib_idx = 0;
      bc_alloc_t() = default;
      explicit bc_alloc_t(const ctrl_alloc_t& c) : ctrl_alloc_t(c) {}
    };
    struct dl_alloc_t {
      size_t           dci_idx;
      sched_ue*        user_ptr;
      rbgmask_t        user_mask;
      uint32_t         pid;
    };
    struct ul_alloc_t {
      enum type_t { NEWTX, NOADAPT_RETX, ADAPT_RETX, MSG3 };
      size_t           dci_idx;
      type_t           type;
      sched_ue*        user_ptr;
      ul_harq_proc::ul_alloc_t alloc;
      uint32_t                 mcs = 0;
      bool             is_retx() const { return type == NOADAPT_RETX or type == ADAPT_RETX; }
      bool             is_msg3() const { return type == MSG3; }
      bool             needs_pdcch() const { return type == NEWTX or type == ADAPT_RETX; }
    };
    typedef std::pair<alloc_outcome_t, const rar_alloc_t*> rar_code_t;
    typedef std::pair<alloc_outcome_t, const ctrl_alloc_t> ctrl_code_t;

    // TTI scheduler result
    pdcch_mask_t                    pdcch_mask;
    sched_interface::dl_sched_res_t dl_sched_result;
    sched_interface::ul_sched_res_t ul_sched_result;

    void            init(sched* parent_);
    void            new_tti(uint32_t tti_rx_, uint32_t start_cfi);
    alloc_outcome_t alloc_bc(uint32_t aggr_lvl, uint32_t sib_idx, uint32_t sib_ntx);
    alloc_outcome_t alloc_paging(uint32_t aggr_lvl, uint32_t paging_payload);
    rar_code_t      alloc_rar(uint32_t aggr_lvl, const dl_sched_rar_t& rar_grant, uint32_t rar_tti, uint32_t buf_rar);
    void            generate_dcis();
    // dl_tti_sched itf
    alloc_outcome_t  alloc_dl_user(sched_ue* user, const rbgmask_t& user_mask, uint32_t pid) final;
    uint32_t         get_tti_tx_dl() const final { return tti_alloc.get_tti_tx_dl(); }
    uint32_t         get_nof_ctrl_symbols() const final;
    const rbgmask_t& get_dl_mask() const final { return tti_alloc.get_dl_mask(); }
    // ul_tti_sched itf
    alloc_outcome_t  alloc_ul_user(sched_ue* user, ul_harq_proc::ul_alloc_t alloc) final;
    alloc_outcome_t  alloc_ul_msg3(sched_ue* user, ul_harq_proc::ul_alloc_t alloc, uint32_t mcs);
    const prbmask_t& get_ul_mask() const final { return tti_alloc.get_ul_mask(); }
    uint32_t         get_tti_tx_ul() const final { return tti_alloc.get_tti_tx_ul(); }

    // getters
    const pdcch_mask_t&            get_pdcch_mask() const { return pdcch_mask; }
    rbgmask_t&                     get_dl_mask() { return tti_alloc.get_dl_mask(); }
    prbmask_t&                     get_ul_mask() { return tti_alloc.get_ul_mask(); }
    const std::vector<ul_alloc_t>& get_ul_allocs() const { return ul_data_allocs; }
    uint32_t                       get_cfi() const { return tti_alloc.get_cfi(); }
    uint32_t                       get_tti_rx() const { return tti_alloc.get_tti_rx(); }
    uint32_t                       get_sfn() const { return tti_alloc.get_sfn(); }
    uint32_t                       get_sf_idx() const { return tti_alloc.get_sf_idx(); }

  private:
    bool            is_dl_alloc(sched_ue* user) const final;
    bool            is_ul_alloc(sched_ue* user) const final;
    ctrl_code_t     alloc_dl_ctrl(uint32_t aggr_lvl, uint32_t tbs_bytes, uint16_t rnti);
    alloc_outcome_t alloc_ul(sched_ue*                       user,
                             ul_harq_proc::ul_alloc_t        alloc,
                             tti_sched_t::ul_alloc_t::type_t alloc_type,
                             uint32_t                        msg3 = 0);
    int             generate_format1a(
                    uint32_t rb_start, uint32_t l_crb, uint32_t tbs, uint32_t rv, uint16_t rnti, srslte_dci_dl_t* dci);
    void set_bc_sched_result(const pdcch_grid_t::alloc_result_t& dci_result);
    void set_rar_sched_result(const pdcch_grid_t::alloc_result_t& dci_result);
    void set_dl_data_sched_result(const pdcch_grid_t::alloc_result_t& dci_result);
    void set_ul_sched_result(const pdcch_grid_t::alloc_result_t& dci_result);

    // consts
    sched*          parent = NULL;
    srslte::log*    log_h  = NULL;
    uint32_t        P;
    cell_cfg_sib_t* sibs_cfg = NULL;

    // internal state
    tti_grid_t               tti_alloc;
    std::vector<rar_alloc_t> rar_allocs;
    std::vector<bc_alloc_t>  bc_allocs;
    std::vector<dl_alloc_t>  data_allocs;
    std::vector<ul_alloc_t>  ul_data_allocs;
  };

  const static uint32_t nof_sched_ttis = 10;
  tti_sched_t           tti_scheds[nof_sched_ttis];
  tti_sched_t*          get_tti_sched(uint32_t tti_rx) { return &tti_scheds[tti_rx % nof_sched_ttis]; }
  std::vector<uint8_t>  tti_dl_mask;

  tti_sched_t* new_tti(uint32_t tti_rx);
  void         generate_phich(tti_sched_t* tti_sched);
  int          generate_dl_sched(tti_sched_t* tti_sched);
  int          generate_ul_sched(tti_sched_t* tti_sched);
  void         dl_sched_bc(tti_sched_t* tti_sched);
  void         dl_sched_rar(tti_sched_t* tti_sched);
  void         dl_sched_data(tti_sched_t* tti_sched);
  void         ul_sched_msg3(tti_sched_t* tti_sched);

  std::map<uint16_t, sched_ue>   ue_db;
  sched_sib_t pending_sibs[MAX_SIBS];

  typedef struct {
    bool enabled; 
    uint16_t rnti; 
    uint32_t L; 
    uint32_t n_prb; 
    uint32_t mcs; 
  } pending_msg3_t; 
 
  const static int SCHED_MAX_PENDING_RAR = 8; 
  sched_rar_t pending_rar[SCHED_MAX_PENDING_RAR];
  pending_msg3_t pending_msg3[10]; 
    
  // Allowed DCI locations for SIB and RAR per CFI
  sched_ue::sched_dci_cce_t common_locations[3];
  sched_ue::sched_dci_cce_t rar_locations[3][10];

  // derived from args
  uint32_t  P;
  uint32_t  si_n_rbg;
  uint32_t  rar_n_rbg;
  uint32_t  nof_rbg;
  prbmask_t prach_mask;
  prbmask_t pucch_mask;

  uint32_t bc_aggr_level; 
  uint32_t rar_aggr_level; 
  
  uint32_t pdsch_re[10];
  uint32_t current_tti;

  bool configured;

};

}

#endif // SRSENB_SCHEDULER_H
