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

#include "srslte/common/common.h"
#include "srslte/srslte.h"
#include <vector>

#ifndef SRSLTE_SCHED_INTERFACE_H
#define SRSLTE_SCHED_INTERFACE_H

namespace srsenb {

class sched_interface
{
public:
  virtual ~sched_interface() = default;

  const static uint32_t max_cce = 128;
  const static uint32_t max_prb = 100;
  const static uint32_t max_rbg = 25;

  const static int MAX_SIB_PAYLOAD_LEN = 2048;
  const static int MAX_SIBS            = 16;
  const static int MAX_LC              = 6;
  const static int MAX_DATA_LIST       = 32;
  const static int MAX_RAR_LIST        = 8;
  const static int MAX_BC_LIST         = 8;
  const static int MAX_RLC_PDU_LIST    = 8;
  const static int MAX_PHICH_LIST      = 8;

  typedef struct {
    uint32_t len;
    uint32_t period_rf;
  } cell_cfg_sib_t;

  struct sched_args_t {
    int      pdsch_mcs            = -1;
    int      pdsch_max_mcs        = 28;
    int      pusch_mcs            = -1;
    int      pusch_max_mcs        = 28;
    uint32_t min_nof_ctrl_symbols = 1;
    uint32_t max_nof_ctrl_symbols = 3;
    int      max_aggr_level       = 3;
  };

  struct cell_cfg_t {

    // Main cell configuration (used to calculate DCI locations in scheduler)
    srslte_cell_t cell;

    /* SIB configuration */
    cell_cfg_sib_t sibs[MAX_SIBS];
    uint32_t       si_window_ms;

    /* pusch configuration */
    srslte_pusch_hopping_cfg_t pusch_hopping_cfg;

    /* prach configuration */
    uint32_t prach_config;
    uint32_t prach_nof_preambles;
    uint32_t prach_freq_offset;
    uint32_t prach_rar_window;
    uint32_t prach_contention_resolution_timer;

    uint32_t maxharq_msg3tx;
    uint32_t n1pucch_an;
    uint32_t delta_pucch_shift;

    // If non-negative, statically allocate N prbs at the edges of the uplink for PUCCH
    int nrb_pucch;

    uint32_t nrb_cqi;
    uint32_t ncs_an;
    uint32_t initial_dl_cqi;

    uint32_t srs_subframe_config;
    uint32_t srs_subframe_offset;
    uint32_t srs_bw_config;

    struct scell_cfg_t {
      uint32_t enb_cc_idx               = 0;
      bool     cross_carrier_scheduling = false;
      bool     ul_allowed               = false;
    };
    std::vector<scell_cfg_t> scell_list;
  };

  struct ue_bearer_cfg_t {
    int priority                              = 0;
    int bsd                                   = 0;
    int pbr                                   = 0;
    int group                                 = 0;
    enum { IDLE = 0, UL, DL, BOTH } direction = IDLE;
  };

  struct ant_info_ded_t {
    enum class tx_mode_t { tm1, tm2, tm3, tm4, tm5, tm6, tm7, tm8_v920, nulltype } tx_mode = tx_mode_t::tm1;
    enum class codebook_t {
      n2_tx_ant_tm3,
      n4_tx_ant_tm3,
      n2_tx_ant_tm4,
      n4_tx_ant_tm4,
      n2_tx_ant_tm5,
      n4_tx_ant_tm5,
      n2_tx_ant_tm6,
      n4_tx_ant_tm6,
      none
    } cookbook_subset_type = codebook_t::none;
    uint64_t codebook_subset_restrict;
    enum class ue_tx_ant_sel_t { release, closed_loop, open_loop } ue_tx_ant_sel = ue_tx_ant_sel_t::release;
  };

  struct ue_cfg_t {
    struct cc_cfg_t {
      bool     active     = false;
      uint32_t enb_cc_idx = 0; ///< eNB CC index
    };
    /* ue capabilities, etc */
    uint32_t                            maxharq_tx           = 5;
    bool                                continuous_pusch     = false;
    srslte_uci_offset_cfg_t             uci_offset           = {};
    srslte_pucch_cfg_t                  pucch_cfg            = {};
    uint32_t                            aperiodic_cqi_period = 0; // if 0 is periodic CQI
    srslte_dl_cfg_t                     dl_cfg;
    std::array<ue_bearer_cfg_t, MAX_LC> ue_bearers = {};
    std::vector<cc_cfg_t>               supported_cc_list; ///< list of UE supported CCs. First index for PCell
    ant_info_ded_t                      dl_ant_info;
    bool                                use_tbs_index_alt = false;
  };

  typedef struct {
    uint32_t lcid;
    uint32_t nbytes;
  } dl_sched_pdu_t;

  typedef struct {
    uint32_t lcid;
    uint32_t lcid_buffer_size;
    uint32_t stop;
    uint8_t* mtch_payload;
  } dl_mtch_sched_t;

  typedef struct {
    dl_sched_pdu_t  pdu[20];
    dl_mtch_sched_t mtch_sched[8];
    uint32_t        num_mtch_sched;
    uint8_t*        mcch_payload;
    uint32_t        current_sf_allocation_num;
  } dl_pdu_mch_t;

  struct dl_sched_data_t {
    srslte_dci_dl_t dci;
    uint32_t        tbs[SRSLTE_MAX_TB];
    bool            mac_ce_ta;
    bool            mac_ce_rnti;
    uint32_t        nof_pdu_elems[SRSLTE_MAX_TB];
    dl_sched_pdu_t  pdu[SRSLTE_MAX_TB][MAX_RLC_PDU_LIST];
  };

  typedef struct {
    bool            needs_pdcch;
    uint32_t        current_tx_nb;
    uint32_t        tbs;
    srslte_dci_ul_t dci;
  } ul_sched_data_t;

  struct dl_sched_rar_info_t {
    uint32_t preamble_idx;
    uint32_t ta_cmd;
    uint16_t temp_crnti;
    uint32_t msg3_size;
    uint32_t prach_tti;
  };

  typedef struct {
    dl_sched_rar_info_t    data;
    srslte_dci_rar_grant_t grant;
  } dl_sched_rar_grant_t;

  typedef struct {
    uint32_t             tbs;
    srslte_dci_dl_t      dci;
    uint32_t             nof_grants;
    dl_sched_rar_grant_t msg3_grant[MAX_RAR_LIST];
  } dl_sched_rar_t;

  typedef struct {
    srslte_dci_dl_t dci;

    enum bc_type { BCCH, PCCH } type;

    uint32_t index;

    uint32_t tbs;

  } dl_sched_bc_t;

  typedef struct {
    uint32_t        cfi;
    uint32_t        nof_data_elems;
    uint32_t        nof_rar_elems;
    uint32_t        nof_bc_elems;
    dl_sched_data_t data[MAX_DATA_LIST];
    dl_sched_rar_t  rar[MAX_RAR_LIST];
    dl_sched_bc_t   bc[MAX_BC_LIST];
  } dl_sched_res_t;

  typedef struct {
    uint16_t rnti;
    enum phich_elem { ACK, NACK } phich;
  } ul_sched_phich_t;

  typedef struct {
    uint32_t         nof_dci_elems;
    uint32_t         nof_phich_elems;
    ul_sched_data_t  pusch[MAX_DATA_LIST];
    ul_sched_phich_t phich[MAX_PHICH_LIST];
  } ul_sched_res_t;

  /******************* Scheduler Control ****************************/

  /* Provides cell configuration including SIB periodicity, etc. */
  virtual int cell_cfg(const std::vector<cell_cfg_t>& cell_cfg) = 0;
  virtual int reset()                                           = 0;

  /* Manages UE scheduling context */
  virtual int  ue_cfg(uint16_t rnti, const ue_cfg_t& cfg) = 0;
  virtual int  ue_rem(uint16_t rnti)                      = 0;
  virtual bool ue_exists(uint16_t rnti)                   = 0;

  /* Manages UE bearers and associated configuration */
  virtual int bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, ue_bearer_cfg_t* cfg) = 0;
  virtual int bearer_ue_rem(uint16_t rnti, uint32_t lc_id)                       = 0;

  virtual uint32_t get_ul_buffer(uint16_t rnti) = 0;
  virtual uint32_t get_dl_buffer(uint16_t rnti) = 0;

  /******************* Scheduling Interface ***********************/

  /**
   * Update the current RLC buffer state for a given user and bearer.
   *
   * @param rnti user rnti
   * @param lc_id logical channel id for which the buffer update is concerned
   * @param tx_queue number of pending bytes for new DL RLC transmissions
   * @param retx_queue number of pending bytes concerning RLC retransmissions
   * @return error code
   */
  virtual int dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue) = 0;

  /**
   * Enqueue MAC CEs for DL transmission
   *
   * @param rnti user rnti
   * @param ce_code lcid of the MAC CE
   * @param nof_cmds how many repetitions of the same MAC CE should be scheduled
   * @return error code
   */
  virtual int dl_mac_buffer_state(uint16_t rnti, uint32_t ce_code, uint32_t nof_cmds) = 0;

  /* DL information */
  virtual int dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack) = 0;
  virtual int dl_rach_info(uint32_t enb_cc_idx, dl_sched_rar_info_t rar_info)                          = 0;
  virtual int dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t ri_value)          = 0;
  virtual int dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t pmi_value)        = 0;
  virtual int dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi_value)        = 0;

  /* UL information */
  virtual int ul_crc_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, bool crc)                          = 0;
  virtual int ul_sr_info(uint32_t tti, uint16_t rnti)                                                          = 0;
  virtual int ul_bsr(uint16_t rnti, uint32_t lcid, uint32_t bsr, bool set_value = true)                        = 0;
  virtual int ul_recv_len(uint16_t rnti, uint32_t lcid, uint32_t len)                                          = 0;
  virtual int ul_phr(uint16_t rnti, int phr)                                                                   = 0;
  virtual int ul_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi, uint32_t ul_ch_code) = 0;

  /* Run Scheduler for this tti */
  virtual int dl_sched(uint32_t tti, uint32_t enb_cc_idx, dl_sched_res_t& sched_result) = 0;
  virtual int ul_sched(uint32_t tti, uint32_t enb_cc_idx, ul_sched_res_t& sched_result) = 0;

  /* Custom */
  virtual void                                 set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs) = 0;
  virtual std::array<int, SRSLTE_MAX_CARRIERS> get_enb_ue_cc_map(uint16_t rnti)                     = 0;
};

} // namespace srsenb

#endif // SRSLTE_SCHED_INTERFACE_H
