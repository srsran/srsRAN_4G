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

#ifndef SRSRAN_SCHED_NR_INTERFACE_H
#define SRSRAN_SCHED_NR_INTERFACE_H

#include "srsenb/hdr/stack/mac/common/sched_config.h"
#include "srsran/adt/bounded_vector.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/span.h"
#include "srsran/asn1/rrc_nr.h"
#include "srsran/common/common_nr.h"
#include "srsran/common/phy_cfg_nr.h"
#include "srsran/common/slot_point.h"
#include "srsran/interfaces/gnb_interfaces.h"
#include "srsran/phy/phch/dci_nr.h"

namespace srsenb {

const static size_t   SCHED_NR_MAX_CARRIERS     = 4;
const static uint16_t SCHED_NR_INVALID_RNTI     = 0;
const static size_t   SCHED_NR_MAX_NOF_RBGS     = 18;
const static size_t   SCHED_NR_MAX_TB           = 1;
const static size_t   SCHED_NR_MAX_HARQ         = SRSRAN_DEFAULT_HARQ_PROC_DL_NR;
const static size_t   SCHED_NR_MAX_BWP_PER_CELL = 2;
const static size_t   SCHED_NR_MAX_LCID         = srsran::MAX_NR_NOF_BEARERS;
const static size_t   SCHED_NR_MAX_LC_GROUP     = 7;

struct sched_nr_ue_cc_cfg_t {
  bool     active = false;
  uint32_t cc     = 0;
};

struct sched_nr_ue_lc_ch_cfg_t {
  uint32_t        lcid; // 1..32
  mac_lc_ch_cfg_t cfg;
};

struct sched_nr_ue_cfg_t {
  uint32_t                                                            maxharq_tx = 4;
  srsran::bounded_vector<sched_nr_ue_cc_cfg_t, SCHED_NR_MAX_CARRIERS> carriers;
  srsran::phy_cfg_nr_t                                                phy_cfg = {};
  asn1::copy_ptr<asn1::rrc_nr::mac_cell_group_cfg_s>                  mac_cell_group_cfg;
  asn1::copy_ptr<asn1::rrc_nr::phys_cell_group_cfg_s>                 phy_cell_group_cfg;
  asn1::copy_ptr<asn1::rrc_nr::sp_cell_cfg_s>                         sp_cell_cfg;
  std::vector<sched_nr_ue_lc_ch_cfg_t>                                lc_ch_to_add;
  std::vector<uint32_t>                                               lc_ch_to_rem;
};

struct sched_nr_bwp_cfg_t {
  uint32_t                 start_rb        = 0;
  uint32_t                 rb_width        = 100;
  srsran_pdcch_cfg_nr_t    pdcch           = {};
  srsran_sch_hl_cfg_nr_t   pdsch           = {};
  srsran_sch_hl_cfg_nr_t   pusch           = {};
  srsran_pucch_nr_hl_cfg_t pucch           = {};
  srsran_harq_ack_cfg_hl_t harq_ack        = {};
  uint32_t                 rar_window_size = 10; // See TS 38.331, ra-ResponseWindow: {1, 2, 4, 8, 10, 20, 40, 80}
  uint32_t                 numerology_idx  = 0;
};

struct sched_nr_cell_cfg_sib_t {
  uint32_t len;
  uint32_t period_rf;
  uint32_t si_window_slots;
};

struct sched_nr_cell_cfg_t {
  static const size_t MAX_SIBS   = 2;
  using ssb_positions_in_burst_t = asn1::rrc_nr::serving_cell_cfg_common_sib_s::ssb_positions_in_burst_s_;

  uint32_t                                               nof_layers;
  uint32_t                                               pci;
  uint32_t                                               ssb_offset;
  uint32_t                                               dl_cell_nof_prb;
  uint32_t                                               ul_cell_nof_prb;
  asn1::rrc_nr::dl_cfg_common_sib_s                      dl_cfg_common;
  asn1::rrc_nr::ul_cfg_common_sib_s                      ul_cfg_common;
  srsran::optional<asn1::rrc_nr::tdd_ul_dl_cfg_common_s> tdd_ul_dl_cfg_common;
  ssb_positions_in_burst_t                               ssb_positions_in_burst;
  uint32_t                                               ssb_periodicity_ms = 0;
  asn1::rrc_nr::mib_s::dmrs_type_a_position_e_           dmrs_type_a_position;
  asn1::rrc_nr::subcarrier_spacing_e                     ssb_scs;
  asn1::rrc_nr::pdcch_cfg_sib1_s                         pdcch_cfg_sib1;
  int                                                    ss_pbch_block_power = 0;
  // Extras
  std::vector<sched_nr_bwp_cfg_t>      bwps{1}; // idx0 for BWP-common
  std::vector<sched_nr_cell_cfg_sib_t> sibs;
  double                               dl_center_frequency_hz;
  double                               ul_center_frequency_hz;
  double                               ssb_center_freq_hz;
};

class sched_nr_interface
{
public:
  static const size_t MAX_GRANTS  = mac_interface_phy_nr::MAX_GRANTS;
  static const size_t MAX_SUBPDUS = 8;

  ///// Configuration /////
  struct sched_args_t {
    bool        pdsch_enabled      = true;
    bool        pusch_enabled      = true;
    bool        auto_refill_buffer = false;
    int         fixed_dl_mcs       = 28;
    int         fixed_ul_mcs       = 28;
    std::string logger_name        = "MAC-NR";
  };

  using ue_cc_cfg_t = sched_nr_ue_cc_cfg_t;
  using ue_cfg_t    = sched_nr_ue_cfg_t;

  ////// RA signalling //////

  struct rar_info_t {
    uint32_t   msg3_size = 7;
    uint32_t   cc;
    uint16_t   temp_crnti;
    slot_point prach_slot;
    uint32_t   ofdm_symbol_idx;
    uint32_t   freq_idx;
    uint32_t   preamble_idx;
    uint32_t   ta_cmd;
  };
  struct msg3_grant_t {
    rar_info_t         data;
    srsran_dci_ul_nr_t msg3_dci = {};
  };
  struct rar_t {
    srsran::bounded_vector<msg3_grant_t, MAX_GRANTS> grants;
  };

  ////// DL data signalling //////

  struct dl_pdu_t {
    srsran::bounded_vector<uint32_t, MAX_SUBPDUS> subpdus;
  };

  ///// Sched Result /////

  using dl_sched_t = mac_interface_phy_nr::dl_sched_t;
  using ul_res_t   = mac_interface_phy_nr::ul_sched_t;

  using sched_sib_list_t    = srsran::bounded_vector<uint32_t, MAX_GRANTS>; /// list of SI indexes
  using sched_rar_list_t    = srsran::bounded_vector<rar_t, MAX_GRANTS>;
  using sched_dl_pdu_list_t = srsran::bounded_vector<dl_pdu_t, MAX_GRANTS>;
  struct dl_res_t {
    dl_sched_t          phy;
    sched_dl_pdu_list_t data;
    sched_rar_list_t    rar;
    sched_sib_list_t    sib_idxs;
  };

  virtual ~sched_nr_interface()                                                                      = default;
  virtual int  config(const sched_args_t& sched_cfg, srsran::const_span<sched_nr_cell_cfg_t> ue_cfg) = 0;
  virtual void ue_cfg(uint16_t rnti, const ue_cfg_t& ue_cfg)                                         = 0;
  virtual void ue_rem(uint16_t rnti)                                                                 = 0;

  virtual void      slot_indication(slot_point slot_tx)           = 0;
  virtual dl_res_t* get_dl_sched(slot_point slot_rx, uint32_t cc) = 0;
  virtual ul_res_t* get_ul_sched(slot_point slot_rx, uint32_t cc) = 0;

  virtual void dl_ack_info(uint16_t rnti, uint32_t cc, uint32_t pid, uint32_t tb_idx, bool ack) = 0;
  virtual void ul_crc_info(uint16_t rnti, uint32_t cc, uint32_t pid, bool crc)                  = 0;
  virtual void ul_sr_info(uint16_t rnti)                                                        = 0;
  virtual void ul_bsr(uint16_t rnti, uint32_t lcg_id, uint32_t bsr)                             = 0;

  /**
   * Enqueue MAC CEs for DL transmission
   *
   * @param rnti user rnti
   * @param ce_lcid lcid of the MAC CE
   * @return error code
   */
  virtual void dl_mac_ce(uint16_t rnti, uint32_t ce_lcid) = 0;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_INTERFACE_H
