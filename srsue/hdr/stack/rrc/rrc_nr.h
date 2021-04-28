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

#ifndef SRSUE_RRC_NR_H
#define SRSUE_RRC_NR_H

#include "srsran/asn1/rrc_nr.h"
#include "srsran/asn1/rrc_nr_utils.h"
#include "srsran/common/block_queue.h"
#include "srsran/common/common_nr.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/stack_procedure.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsue/hdr/stack/upper/gw.h"

namespace srsue {

class usim_interface_rrc_nr;
class pdcp_interface_rrc;
class rlc_interface_rrc;
class stack_interface_rrc;

// Expert arguments to create GW without proper RRC
struct core_less_args_t {
  std::string      ip_addr;
  srsue::gw_args_t gw_args;
  uint8_t          drb_lcid;
};

struct rrc_nr_args_t {
  core_less_args_t      coreless;
  uint32_t              sim_nr_meas_pci;
  bool                  pdcp_short_sn_support;
  std::string           supported_bands_nr_str;
  std::vector<uint32_t> supported_bands_nr;
  std::vector<uint32_t> supported_bands_eutra;
  std::string           log_level;
  uint32_t              log_hex_limit;
};

struct rrc_nr_metrics_t {};

class rrc_nr final : public rrc_interface_phy_nr,
                     public rrc_interface_pdcp,
                     public rrc_interface_rlc,
                     public rrc_interface_mac,
                     public rrc_nr_interface_rrc,
                     public srsran::timer_callback
{
public:
  rrc_nr(srsran::task_sched_handle task_sched_);
  ~rrc_nr();

  void init(phy_interface_rrc_nr*       phy_,
            mac_interface_rrc_nr*       mac_,
            rlc_interface_rrc*          rlc_,
            pdcp_interface_rrc*         pdcp_,
            gw_interface_rrc*           gw_,
            rrc_eutra_interface_rrc_nr* rrc_eutra_,
            usim_interface_rrc_nr*      usim_,
            srsran::timer_handler*      timers_,
            stack_interface_rrc*        stack_,
            const rrc_nr_args_t&        args_);

  void stop();
  void init_core_less();

  void get_metrics(rrc_nr_metrics_t& m);

  // Timeout callback interface
  void timer_expired(uint32_t timeout_id) final;
  void srsran_rrc_log(const char* str);

  enum direction_t { Rx = 0, Tx };
  template <class T>
  void log_rrc_message(const std::string&           source,
                       direction_t                  dir,
                       const srsran::byte_buffer_t* pdu,
                       const T&                     msg,
                       const std::string&           msg_type);
  template <class T>
  void log_rrc_message(const std::string&  source,
                       direction_t         dir,
                       asn1::dyn_octstring oct,
                       const T&            msg,
                       const std::string&  msg_type);
  // PHY interface
  void in_sync() final;
  void out_of_sync() final;

  // RLC interface
  void max_retx_attempted() final;

  // MAC interface
  void run_tti(uint32_t tti) final;
  void ra_completed() final;
  void ra_problem() final;
  void release_pucch_srs() final;

  // PDCP interface
  void write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu) final;
  void write_pdu_bcch_bch(srsran::unique_byte_buffer_t pdu) final;
  void write_pdu_bcch_dlsch(srsran::unique_byte_buffer_t pdu) final;
  void write_pdu_pcch(srsran::unique_byte_buffer_t pdu) final;
  void write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t pdu) final;

  // RRC (LTE) interface
  void get_eutra_nr_capabilities(srsran::byte_buffer_t* eutra_nr_caps);
  void get_nr_capabilities(srsran::byte_buffer_t* eutra_nr_caps);
  void phy_meas_stop();
  void phy_set_cells_to_meas(uint32_t carrier_freq_r15);
  bool rrc_reconfiguration(bool                endc_release_and_add_r15,
                           bool                nr_secondary_cell_group_cfg_r15_present,
                           asn1::dyn_octstring nr_secondary_cell_group_cfg_r15,
                           bool                sk_counter_r15_present,
                           uint32_t            sk_counter_r15,
                           bool                nr_radio_bearer_cfg1_r15_present,
                           asn1::dyn_octstring nr_radio_bearer_cfg1_r15);
  bool configure_sk_counter(uint16_t sk_counter);
  bool is_config_pending();
  // STACK interface
  void cell_search_completed(const rrc_interface_phy_lte::cell_search_ret_t& cs_ret, const phy_cell_t& found_cell);

private:
  srsran::task_sched_handle task_sched;
  struct cmd_msg_t {
    enum { PDU, PCCH, PDU_MCH, RLF, PDU_BCCH_DLSCH, STOP } command;
    srsran::unique_byte_buffer_t pdu;
    uint16_t                     lcid;
  };

  srslog::basic_logger&          logger;
  bool                           running = false;
  srsran::block_queue<cmd_msg_t> cmd_q;

  // PHY config
  srsran::phy_cfg_nr_t phy_cfg = {};

  phy_interface_rrc_nr*       phy       = nullptr;
  mac_interface_rrc_nr*       mac       = nullptr;
  rlc_interface_rrc*          rlc       = nullptr;
  pdcp_interface_rrc*         pdcp      = nullptr;
  gw_interface_rrc*           gw        = nullptr;
  rrc_eutra_interface_rrc_nr* rrc_eutra = nullptr;
  usim_interface_rrc_nr*      usim      = nullptr;
  stack_interface_rrc*        stack     = nullptr;

  const uint32_t                      sim_measurement_timer_duration_ms = 250;
  uint32_t                            sim_measurement_carrier_freq_r15;
  srsran::timer_handler::unique_timer sim_measurement_timer;

  /// RRC states (3GPP 38.331 v15.5.1 Sec 4.2.1)
  enum rrc_nr_state_t {
    RRC_NR_STATE_IDLE = 0,
    RRC_NR_STATE_CONNECTED,
    RRC_NR_STATE_CONNECTED_INACTIVE,
    RRC_NR_STATE_N_ITEMS,
  };
  const static char* rrc_nr_state_text[RRC_NR_STATE_N_ITEMS];

  //  rrc_nr_state_t state = RRC_NR_STATE_IDLE;

  rrc_nr_args_t args = {};

  // RRC constants and timers
  srsran::timer_handler* timers = nullptr;

  const char* get_rb_name(uint32_t lcid) final;

  bool     add_lcid_drb(uint32_t lcid, uint32_t drb_id);
  uint32_t get_lcid_for_drbid(uint32_t rdid);

  std::map<uint32_t, srsran::nr_drb> lcid_drb; // Map of lcid to drb

  std::map<uint32_t, uint32_t> drb_eps_bearer_id; // Map of drb id to eps_bearer_id

  // temporary maps for building the pucch nr resources
  std::map<uint32_t, srsran_pucch_nr_resource_t>   res_list;
  std::map<uint32_t, bool>                         res_list_present;
  std::map<uint32_t, srsran_csi_rs_zp_resource_t>  csi_rs_zp_res;
  std::map<uint32_t, srsran_csi_rs_nzp_resource_t> csi_rs_nzp_res;

  bool apply_cell_group_cfg(const asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg);
  bool apply_radio_bearer_cfg(const asn1::rrc_nr::radio_bearer_cfg_s& radio_bearer_cfg);
  bool apply_rlc_add_mod(const asn1::rrc_nr::rlc_bearer_cfg_s& rlc_bearer_cfg);
  bool apply_mac_cell_group(const asn1::rrc_nr::mac_cell_group_cfg_s& mac_cell_group_cfg);
  bool apply_sp_cell_cfg(const asn1::rrc_nr::sp_cell_cfg_s& sp_cell_cfg);
  bool apply_phy_cell_group_cfg(const asn1::rrc_nr::phys_cell_group_cfg_s& phys_cell_group_cfg);
  bool apply_dl_common_cfg(const asn1::rrc_nr::dl_cfg_common_s& dl_cfg_common);
  bool apply_ul_common_cfg(const asn1::rrc_nr::ul_cfg_common_s& ul_cfg_common);
  bool apply_sp_cell_init_dl_pdcch(const asn1::rrc_nr::pdcch_cfg_s& pdcch_cfg);
  bool apply_sp_cell_init_dl_pdsch(const asn1::rrc_nr::pdsch_cfg_s& pdsch_cfg);
  bool apply_sp_cell_ded_ul_pucch(const asn1::rrc_nr::pucch_cfg_s& pucch_cfg);
  bool apply_sp_cell_ded_ul_pusch(const asn1::rrc_nr::pusch_cfg_s& pusch_cfg);
  bool apply_csi_meas_cfg(const asn1::rrc_nr::csi_meas_cfg_s& csi_meas_cfg);
  bool apply_res_csi_report_cfg(const asn1::rrc_nr::csi_report_cfg_s& csi_report_cfg);
  bool apply_drb_add_mod(const asn1::rrc_nr::drb_to_add_mod_s& drb_cfg);
  bool apply_drb_release(const uint8_t drb);
  bool apply_security_cfg(const asn1::rrc_nr::security_cfg_s& security_cfg);

  srsran::as_security_config_t sec_cfg;

  typedef enum { mcg_srb1, en_dc_srb3, nr } reconf_initiator_t;

  class connection_reconf_no_ho_proc
  {
  public:
    explicit connection_reconf_no_ho_proc(rrc_nr* parent_);
    srsran::proc_outcome_t init(const reconf_initiator_t  initiator_,
                                const bool                endc_release_and_add_r15,
                                const bool                nr_secondary_cell_group_cfg_r15_present,
                                const asn1::dyn_octstring nr_secondary_cell_group_cfg_r15,
                                const bool                sk_counter_r15_present,
                                const uint32_t            sk_counter_r15,
                                const bool                nr_radio_bearer_cfg1_r15_present,
                                const asn1::dyn_octstring nr_radio_bearer_cfg1_r15);
    srsran::proc_outcome_t step() { return srsran::proc_outcome_t::yield; }
    static const char*     name() { return "NR Connection Reconfiguration"; }
    srsran::proc_outcome_t react(const bool& config_complete);
    void                   then(const srsran::proc_state_t& result);

  private:
    // const
    rrc_nr*                        rrc_ptr;
    reconf_initiator_t             initiator;
    asn1::rrc_nr::rrc_recfg_s      rrc_recfg;
    asn1::rrc_nr::cell_group_cfg_s cell_group_cfg;
  };

  srsran::proc_t<connection_reconf_no_ho_proc> conn_recfg_proc;

  srsran::proc_manager_list_t callback_list;
};

} // namespace srsue

#endif // SRSUE_RRC_NR_H