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

#ifndef SRSUE_RRC_H
#define SRSUE_RRC_H

#include "rrc_cell.h"
#include "rrc_common.h"
#include "rrc_metrics.h"
#include "srslte/asn1/rrc_asn1.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/block_queue.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/logmap.h"
#include "srslte/common/security.h"
#include "srslte/common/stack_procedure.h"
#include "srslte/interfaces/ue_interfaces.h"

#include <map>
#include <math.h>
#include <queue>

#define SRSLTE_RRC_N_BANDS 43
typedef struct {
  std::string                             ue_category_str;
  uint32_t                                ue_category;
  int                                     ue_category_ul;
  int                                     ue_category_dl;
  uint32_t                                release;
  uint32_t                                feature_group;
  std::array<uint8_t, SRSLTE_RRC_N_BANDS> supported_bands;
  uint32_t                                nof_supported_bands;
  bool                                    support_ca;
  int                                     mbms_service_id;
  uint32_t                                mbms_service_port;
} rrc_args_t;

#define SRSLTE_UE_CATEGORY_DEFAULT "4"
#define SRSLTE_UE_CATEGORY_MIN 1
#define SRSLTE_UE_CATEGORY_MAX 21
#define SRSLTE_RELEASE_DEFAULT 8
#define SRSLTE_RELEASE_MIN 8
#define SRSLTE_RELEASE_MAX 15

using srslte::byte_buffer_t;

namespace srsue {

class phy_controller;

class rrc : public rrc_interface_nas,
            public rrc_interface_phy_lte,
            public rrc_interface_mac,
            public rrc_interface_pdcp,
            public rrc_interface_rlc,
            public srslte::timer_callback
{
public:
  rrc(stack_interface_rrc* stack_, srslte::task_sched_handle task_sched_);
  ~rrc();

  void init(phy_interface_rrc_lte* phy_,
            mac_interface_rrc*     mac_,
            rlc_interface_rrc*     rlc_,
            pdcp_interface_rrc*    pdcp_,
            nas_interface_rrc*     nas_,
            usim_interface_rrc*    usim_,
            gw_interface_rrc*      gw_,
            const rrc_args_t&      args_);

  void stop();

  void get_metrics(rrc_metrics_t& m);

  // Timeout callback interface
  void timer_expired(uint32_t timeout_id) final;
  void srslte_rrc_log(const char* str);

  typedef enum { Rx = 0, Tx } direction_t;
  template <class T>
  void log_rrc_message(const std::string            source,
                       const direction_t            dir,
                       const srslte::byte_buffer_t* pdu,
                       const T&                     msg,
                       const std::string&           msg_type);

  std::string print_mbms();
  bool        mbms_service_start(uint32_t serv, uint32_t port);

  // NAS interface
  void     write_sdu(srslte::unique_byte_buffer_t sdu);
  void     enable_capabilities();
  uint16_t get_mcc();
  uint16_t get_mnc();
  bool     plmn_search() final;
  void     plmn_select(srslte::plmn_id_t plmn_id);
  bool     connection_request(srslte::establishment_cause_t cause, srslte::unique_byte_buffer_t dedicated_info_nas);
  void     set_ue_identity(srslte::s_tmsi_t s_tmsi);
  void     paging_completed(bool outcome) final;

  // PHY interface
  void in_sync() final;
  void out_of_sync() final;
  void new_cell_meas(const std::vector<phy_meas_t>& meas);
  void cell_search_complete(cell_search_ret_t ret, phy_cell_t found_cell);
  void cell_select_complete(bool status);
  void set_config_complete(bool status);
  void set_scell_complete(bool status);

  // MAC interface
  void ra_completed() final;
  void release_pucch_srs();
  void run_tti();
  void ra_problem();

  // GW interface
  bool is_connected() final; // this is also NAS interface
  bool have_drb();

  // PDCP interface
  void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void write_pdu_bcch_bch(srslte::unique_byte_buffer_t pdu);
  void write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu);
  void write_pdu_pcch(srslte::unique_byte_buffer_t pdu);
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu);

  bool srbs_flushed(); //< Check if data on SRBs still needs to be sent

protected:
  // Moved to protected to be accessible by unit tests
  void set_serving_cell(phy_cell_t phy_cell, bool discard_serving);
  bool has_neighbour_cell(uint32_t earfcn, uint32_t pci) const;
  bool is_serving_cell(uint32_t earfcn, uint32_t pci) const;
  int  start_cell_select();

private:
  typedef struct {
    enum { PCCH, RLF, RA_COMPLETE, STOP } command;
    srslte::unique_byte_buffer_t pdu;
    uint16_t                     lcid;
  } cmd_msg_t;

  bool                           running = false;
  srslte::block_queue<cmd_msg_t> cmd_q;

  void process_pcch(srslte::unique_byte_buffer_t pdu);

  stack_interface_rrc*      stack = nullptr;
  srslte::task_sched_handle task_sched;
  srslte::byte_buffer_pool* pool = nullptr;
  srslte::log_ref           rrc_log;
  phy_interface_rrc_lte*    phy  = nullptr;
  mac_interface_rrc*        mac  = nullptr;
  rlc_interface_rrc*        rlc  = nullptr;
  pdcp_interface_rrc*       pdcp = nullptr;
  nas_interface_rrc*        nas  = nullptr;
  usim_interface_rrc*       usim = nullptr;
  gw_interface_rrc*         gw   = nullptr;

  srslte::unique_byte_buffer_t dedicated_info_nas;

  void send_ul_ccch_msg(const asn1::rrc::ul_ccch_msg_s& msg);
  void send_ul_dcch_msg(uint32_t lcid, const asn1::rrc::ul_dcch_msg_s& msg);

  srslte::bit_buffer_t bit_buf;

  rrc_state_t      state = RRC_STATE_IDLE, last_state = RRC_STATE_IDLE;
  uint8_t          transaction_id = 0;
  srslte::s_tmsi_t ue_identity;
  bool             ue_identity_configured = false;

  bool drb_up = false;

  // PHY controller state machine
  std::unique_ptr<phy_controller> phy_ctrl;

  rrc_args_t args = {};

  uint32_t cell_clean_cnt = 0;

  srslte::phy_cfg_t previous_phy_cfg = {};
  srslte::mac_cfg_t current_mac_cfg, previous_mac_cfg = {};

  void                         generate_as_keys();
  srslte::as_security_config_t sec_cfg = {};

  std::map<uint32_t, asn1::rrc::srb_to_add_mod_s> srbs;
  std::map<uint32_t, asn1::rrc::drb_to_add_mod_s> drbs;

  // RRC constants and timers
  uint32_t                            n310_cnt = 0, N310 = 0;
  uint32_t                            n311_cnt = 0, N311 = 0;
  srslte::timer_handler::unique_timer t300, t301, t302, t310, t311, t304;

  // Radio bearers
  typedef enum {
    RB_ID_SRB0 = 0,
    RB_ID_SRB1,
    RB_ID_SRB2,
    RB_ID_DRB1,
    RB_ID_DRB2,
    RB_ID_DRB3,
    RB_ID_DRB4,
    RB_ID_DRB5,
    RB_ID_DRB6,
    RB_ID_DRB7,
    RB_ID_DRB8,
    RB_ID_MAX
  } rb_id_t;

  static const std::string rb_id_str[];

  std::string get_rb_name(uint32_t lcid)
  {
    if (lcid < RB_ID_MAX) {
      return rb_id_str[lcid];
    } else {
      return "INVALID_RB";
    }
  }

  // Measurements private subclass
  class rrc_meas;
  std::unique_ptr<rrc_meas> measurements;

  // List of strongest neighbour cell
  using unique_cell_t = std::unique_ptr<meas_cell>;
  meas_cell_list meas_cells;

  bool                     initiated                  = false;
  asn1::rrc::reest_cause_e m_reest_cause              = asn1::rrc::reest_cause_e::nulltype;
  uint16_t                 m_reest_rnti               = 0;
  uint16_t                 m_reest_source_pci         = 0;
  bool                     reestablishment_started    = false;
  bool                     reestablishment_successful = false;

  // Interface from rrc_meas
  void               send_srb1_msg(const asn1::rrc::ul_dcch_msg_s& msg);
  std::set<uint32_t> get_cells(const uint32_t earfcn);
  float              get_cell_rsrp(const uint32_t earfcn, const uint32_t pci);
  float              get_cell_rsrq(const uint32_t earfcn, const uint32_t pci);
  meas_cell*         get_serving_cell();

  void                                          process_cell_meas();
  void                                          process_new_cell_meas(const std::vector<phy_meas_t>& meas);
  srslte::block_queue<std::vector<phy_meas_t> > cell_meas_q;

  // Cell selection/reselection functions/variables
  typedef struct {
    float Qrxlevmin;
    float Qrxlevminoffset;
    float Qqualmin;
    float Qqualminoffset;
    float s_intrasearchP;
    float q_hyst;
    float threshservinglow;
  } cell_resel_cfg_t;

  cell_resel_cfg_t cell_resel_cfg = {};

  float get_srxlev(float Qrxlevmeas);
  float get_squal(float Qqualmeas);

  /********************
   *  RRC Procedures
   *******************/

  enum class cs_result_t { changed_cell, same_cell, no_cell };

  // RRC procedures (fwd declared)
  class cell_search_proc;
  class si_acquire_proc;
  class serving_cell_config_proc;
  class cell_selection_proc;
  class connection_request_proc;
  class connection_reconf_no_ho_proc;
  class plmn_search_proc;
  class process_pcch_proc;
  class go_idle_proc;
  class cell_reselection_proc;
  class connection_reest_proc;
  class ho_proc;
  srslte::proc_t<cell_search_proc, rrc_interface_phy_lte::cell_search_ret_t> cell_searcher;
  srslte::proc_t<si_acquire_proc>                                            si_acquirer;
  srslte::proc_t<serving_cell_config_proc>                                   serv_cell_cfg;
  srslte::proc_t<cell_selection_proc, cs_result_t>                           cell_selector;
  srslte::proc_t<go_idle_proc>                                               idle_setter;
  srslte::proc_t<process_pcch_proc>                                          pcch_processor;
  srslte::proc_t<connection_request_proc>                                    conn_req_proc;
  srslte::proc_t<plmn_search_proc>                                           plmn_searcher;
  srslte::proc_t<cell_reselection_proc>                                      cell_reselector;
  srslte::proc_t<connection_reest_proc>                                      connection_reest;
  srslte::proc_t<ho_proc>                                                    ho_handler;
  srslte::proc_t<connection_reconf_no_ho_proc>                               conn_recfg_proc;

  srslte::proc_manager_list_t callback_list;

  bool cell_selection_criteria(float rsrp, float rsrq = 0);
  void cell_reselection(float rsrp, float rsrq);

  std::vector<uint32_t> ue_required_sibs;
  srslte::plmn_id_t     selected_plmn_id = {};
  bool                  plmn_is_selected = false;

  bool security_is_activated = false;

  // RLC interface
  void max_retx_attempted();

  // Senders
  void send_con_request(srslte::establishment_cause_t cause);
  void send_con_restablish_request(asn1::rrc::reest_cause_e cause, uint16_t rnti, uint16_t pci, uint32_t cellid);
  void send_con_restablish_complete();
  void send_con_setup_complete(srslte::unique_byte_buffer_t nas_msg);
  void send_ul_info_transfer(srslte::unique_byte_buffer_t nas_msg);
  void send_security_mode_complete();
  void send_rrc_con_reconfig_complete();

  // Parsers
  void process_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_dl_ccch(srslte::unique_byte_buffer_t pdu);
  void parse_dl_dcch(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_dl_info_transfer(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu);
  void parse_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu);

  // Helpers
  void con_reconfig_failed();
  bool con_reconfig_ho(const asn1::rrc::rrc_conn_recfg_s& reconfig);
  void ho_failed();
  void start_go_idle();
  void rrc_connection_release(const std::string& cause);
  void radio_link_failure_push_cmd();
  void radio_link_failure_process();
  void leave_connected();
  void stop_timers();
  void start_con_restablishment(asn1::rrc::reest_cause_e cause);

  void log_rr_config_common();
  void log_phy_config_dedicated();
  void log_mac_config_dedicated();

  void apply_rr_config_common(asn1::rrc::rr_cfg_common_s* config, bool send_lower_layers);
  bool apply_rr_config_dedicated(const asn1::rrc::rr_cfg_ded_s* cnfg, bool is_handover = false);
  bool apply_rr_config_dedicated_on_ho_complete(const asn1::rrc::rr_cfg_ded_s& cnfg);
  void apply_scell_config(asn1::rrc::rrc_conn_recfg_r8_ies_s* reconfig_r8, bool enable_cqi);
  bool apply_scell_config_on_ho_complete(const asn1::rrc::rrc_conn_recfg_r8_ies_s& reconfig_r8);
  void apply_phy_config_dedicated(const asn1::rrc::phys_cfg_ded_s& phy_cnfg, bool is_handover);
  void apply_phy_scell_config(const asn1::rrc::scell_to_add_mod_r10_s& scell_config, bool enable_cqi);

  void apply_mac_config_dedicated_default();

  void handle_sib1();
  void handle_sib2();
  void handle_sib3();
  void handle_sib13();

  void     handle_con_setup(const asn1::rrc::rrc_conn_setup_s& setup);
  void     handle_con_reest(const asn1::rrc::rrc_conn_reest_s& setup);
  void     handle_rrc_con_reconfig(uint32_t lcid, const asn1::rrc::rrc_conn_recfg_s& reconfig);
  void     handle_ue_capability_enquiry(const asn1::rrc::ue_cap_enquiry_s& enquiry);
  void     add_srb(const asn1::rrc::srb_to_add_mod_s& srb_cnfg);
  void     add_drb(const asn1::rrc::drb_to_add_mod_s& drb_cnfg);
  void     release_drb(uint32_t drb_id);
  uint32_t get_lcid_for_eps_bearer(const uint32_t& eps_bearer_id);
  void     add_mrb(uint32_t lcid, uint32_t port);

  // Helpers for setting default values
  void set_phy_default_pucch_srs();
  void set_phy_default();
  void set_mac_default();
  void set_rrc_default();
};

} // namespace srsue

#endif // SRSUE_RRC_H
