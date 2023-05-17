/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "common/sched_config.h"
#include "srsran/adt/bounded_vector.h"
#include "srsran/common/common.h"
#include "srsran/srsran.h"
#include <vector>

#ifndef SRSRAN_SCHED_INTERFACE_H
#define SRSRAN_SCHED_INTERFACE_H

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
  const static int MAX_LC              = 11;
  const static int MAX_LC_GROUP        = 4;
  const static int MAX_DATA_LIST       = 32;
  const static int MAX_RAR_LIST        = 8;
  const static int MAX_BC_LIST         = 8;
  const static int MAX_PO_LIST         = 8;
  const static int MAX_RLC_PDU_LIST    = 8;
  const static int MAX_PHICH_LIST      = 8;

  typedef struct {
    uint32_t len;
    uint32_t period_rf;
  } cell_cfg_sib_t;

  struct sched_args_t {
    std::string sched_policy              = "time_pf";
    std::string sched_policy_args         = "2";
    int         pdsch_mcs                 = -1;
    int         pdsch_max_mcs             = 28;
    int         pusch_mcs                 = -1;
    int         pusch_max_mcs             = 28;
    uint32_t    min_nof_ctrl_symbols      = 1;
    uint32_t    max_nof_ctrl_symbols      = 3;
    int         min_aggr_level            = 0;
    int         max_aggr_level            = 3;
    bool        adaptive_aggr_level       = false;
    bool        pucch_mux_enabled         = false;
    int         pucch_harq_max_rb         = 0;
    float       target_bler               = 0.05;
    float       max_delta_dl_cqi          = 5;
    float       max_delta_ul_snr          = 5;
    float       adaptive_dl_mcs_step_size = 0.001;
    float       adaptive_ul_mcs_step_size = 0.001;
    uint32_t    min_tpc_tti_interval      = 1;
    float       ul_snr_avg_alpha          = 0.05;
    int         init_ul_snr_value         = 5;
    int         init_dl_cqi               = 5;
    float       max_sib_coderate          = 0.8;
    int         pdcch_cqi_offset          = 0;
  };

  struct cell_cfg_t {
    // Main cell configuration (used to calculate DCI locations in scheduler)
    srsran_cell_t cell;

    /* SIB configuration */
    cell_cfg_sib_t sibs[MAX_SIBS];
    uint32_t       si_window_ms;

    /* pucch configuration */
    float target_pucch_ul_sinr;

    /* pusch configuration */
    srsran_pusch_hopping_cfg_t pusch_hopping_cfg;
    float                      target_pusch_ul_sinr;
    int                        min_phr_thres;
    bool                       enable_phr_handling;
    bool                       enable_64qam;

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
      bool            active               = false;
      bool            ul_disabled          = false;
      uint32_t        enb_cc_idx           = 0; ///< eNB CC index
      srsran_dl_cfg_t dl_cfg               = {};
      uint32_t        aperiodic_cqi_period = 0; // if 0 is periodic CQI
    };
    /* ue capabilities, etc */
    uint32_t                            maxharq_tx       = 5;
    bool                                continuous_pusch = false;
    srsran_uci_offset_cfg_t             uci_offset       = {15, 12, 10};
    srsran_pucch_cfg_t                  pucch_cfg        = {};
    std::array<mac_lc_ch_cfg_t, MAX_LC> ue_bearers       = {};
    std::vector<cc_cfg_t>               supported_cc_list; ///< list of UE supported CCs. First index for PCell
    ant_info_ded_t                      dl_ant_info;
    bool                                use_tbs_index_alt = false;
    uint32_t                            measgap_period    = 0;
    uint32_t                            measgap_offset    = 0;
    enum class ul64qam_cap { undefined, disabled, enabled };
    ul64qam_cap support_ul64qam = ul64qam_cap::undefined;
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
    srsran_dci_dl_t dci;
    uint32_t        tbs[SRSRAN_MAX_TB];
    bool            mac_ce_ta;
    bool            mac_ce_rnti;
    uint32_t        nof_pdu_elems[SRSRAN_MAX_TB];
    dl_sched_pdu_t  pdu[SRSRAN_MAX_TB][MAX_RLC_PDU_LIST];
  };

  typedef struct {
    bool            needs_pdcch;
    uint32_t        current_tx_nb;
    uint32_t        tbs;
    srsran_dci_ul_t dci;
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
    srsran_dci_rar_grant_t grant;
  } dl_sched_rar_grant_t;

  struct dl_sched_rar_t {
    uint32_t                                                   tbs;
    srsran_dci_dl_t                                            dci;
    srsran::bounded_vector<dl_sched_rar_grant_t, MAX_RAR_LIST> msg3_grant;
  };

  typedef struct {
    srsran_dci_dl_t dci;
    enum bc_type { BCCH, PCCH } type;
    uint32_t index;
    uint32_t tbs;
  } dl_sched_bc_t;

  struct dl_sched_po_info_t {
    uint32_t preamble_idx;
    uint32_t prach_mask_idx;
    uint16_t crnti;
  };

  typedef struct {
    srsran_dci_dl_t dci;
    uint32_t        tbs;
    uint16_t        crnti;
    uint32_t        preamble_idx;
    uint32_t        prach_mask_idx;
  } dl_sched_po_t;

  struct dl_sched_res_t {
    uint32_t                                               cfi;
    srsran::bounded_vector<dl_sched_data_t, MAX_DATA_LIST> data;
    srsran::bounded_vector<dl_sched_rar_t, MAX_RAR_LIST>   rar;
    srsran::bounded_vector<dl_sched_bc_t, MAX_BC_LIST>     bc;
    srsran::bounded_vector<dl_sched_po_t, MAX_PO_LIST>     po;
  };

  typedef struct {
    uint16_t rnti;
    enum phich_elem { ACK, NACK } phich;
  } ul_sched_phich_t;

  struct ul_sched_res_t {
    srsran::bounded_vector<ul_sched_data_t, MAX_DATA_LIST>   pusch;
    srsran::bounded_vector<ul_sched_phich_t, MAX_PHICH_LIST> phich;
  };

  /******************* Scheduler Control ****************************/

  /* Provides cell configuration including SIB periodicity, etc. */
  virtual int cell_cfg(const std::vector<cell_cfg_t>& cell_cfg) = 0;
  virtual int reset()                                           = 0;

  /* Manages UE scheduling context */
  virtual int  ue_cfg(uint16_t rnti, const ue_cfg_t& cfg) = 0;
  virtual int  ue_rem(uint16_t rnti)                      = 0;
  virtual bool ue_exists(uint16_t rnti)                   = 0;

  /* Manages UE bearers and associated configuration */
  virtual int bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, const mac_lc_ch_cfg_t& cfg) = 0;
  virtual int bearer_ue_rem(uint16_t rnti, uint32_t lc_id)                             = 0;

  virtual uint32_t get_ul_buffer(uint16_t rnti) = 0;
  virtual uint32_t get_dl_buffer(uint16_t rnti) = 0;

  /******************* Scheduling Interface ***********************/

  /**
   * Update the current RLC buffer state for a given user and bearer.
   *
   * @param rnti user rnti
   * @param lc_id logical channel id for which the buffer update is concerned
   * @param tx_queue number of pending bytes for new DL RLC transmissions
   * @param prio_tx_queue number of pending bytes concerning RLC retransmissions and status PDUs
   * @return error code
   */
  virtual int dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t prio_tx_queue) = 0;

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
  virtual int dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack)        = 0;
  virtual int dl_rach_info(uint32_t enb_cc_idx, dl_sched_rar_info_t rar_info)                                 = 0;
  virtual int dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t ri_value)                 = 0;
  virtual int dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t pmi_value)               = 0;
  virtual int dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi_value)               = 0;
  virtual int dl_sb_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t sb_idx, uint32_t cqi) = 0;

  /* UL information */
  virtual int ul_crc_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, bool crc)                       = 0;
  virtual int ul_sr_info(uint32_t tti, uint16_t rnti)                                                       = 0;
  virtual int ul_bsr(uint16_t rnti, uint32_t lcg_id, uint32_t bsr)                                          = 0;
  virtual int ul_phr(uint16_t rnti, int phr, uint32_t ul_nof_prb)                                           = 0;
  virtual int ul_snr_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, float snr, uint32_t ul_ch_code) = 0;

  /* Run Scheduler for this tti */
  virtual int dl_sched(uint32_t tti, uint32_t enb_cc_idx, dl_sched_res_t& sched_result) = 0;
  virtual int ul_sched(uint32_t tti, uint32_t enb_cc_idx, ul_sched_res_t& sched_result) = 0;

  /* PDCCH order */
  virtual int set_pdcch_order(uint32_t enb_cc_idx, dl_sched_po_info_t pdcch_order_info) = 0;

  /* Custom */
  virtual void                                 set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs)        = 0;
  virtual std::array<int, SRSRAN_MAX_CARRIERS> get_enb_ue_cc_map(uint16_t rnti)                            = 0;
  virtual std::array<int, SRSRAN_MAX_CARRIERS> get_enb_ue_activ_cc_map(uint16_t rnti)                      = 0;
  virtual int                                  ul_buffer_add(uint16_t rnti, uint32_t lcid, uint32_t bytes) = 0;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_INTERFACE_H
