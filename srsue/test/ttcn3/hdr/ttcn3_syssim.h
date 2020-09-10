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

#ifndef SRSUE_TTCN3_SYSSIM_H
#define SRSUE_TTCN3_SYSSIM_H

#include "srslte/mac/pdu_queue.h"
#include "srslte/test/ue_test_interfaces.h"
#include "srslte/upper/pdcp.h"
#include "srslte/upper/rlc.h"
#include "swappable_log.h"
#include "ttcn3_common.h"
#include "ttcn3_drb_interface.h"
#include "ttcn3_ip_ctrl_interface.h"
#include "ttcn3_ip_sock_interface.h"
#include "ttcn3_srb_interface.h"
#include "ttcn3_sys_interface.h"
#include "ttcn3_ue.h"
#include "ttcn3_ut_interface.h"
#include <functional>

class ttcn3_syssim : public syssim_interface_phy,
                     public ss_ut_interface,
                     public ss_sys_interface,
                     public ss_srb_interface,
                     public rrc_interface_rlc,
                     public rlc_interface_pdcp,
                     public rrc_interface_pdcp,
                     public gw_interface_pdcp,
                     public srslte::pdu_queue::process_callback
{
public:
  ttcn3_syssim(swappable_log& logger_file_, srslte::logger& logger_stdout_, ttcn3_ue* ue_);

  ~ttcn3_syssim();

  int init(const all_args_t& args_);

  void set_forced_lcid(int lcid);

  int add_port_handler();

  ///< Function called by epoll timer handler when TTI timer expires
  void new_tti_indication(uint64_t res);

  void stop();

  void reset();

  // Called from UT before starting testcase
  void tc_start(const char* name);

  // Called from UT to terminate the testcase
  void tc_end();

  void power_off_ue();

  // Called from outside
  void switch_on_ue();

  void switch_off_ue();

  void enable_data();

  void disable_data();

  // Called from PHY but always from the SS main thread with lock being hold
  void prach_indication(uint32_t preamble_index_, const uint32_t& cell_id);

  // Called from PHY
  void sr_req(uint32_t tti_tx);

  // Called from PHY
  void tx_pdu(const uint8_t* payload, const int len, const uint32_t tx_tti);

  // Internal function called from main thread
  void send_rar(uint32_t preamble_index);

  // Internal function called from main thread
  void send_msg3_grant();

  // Internal function called from main thread
  void send_sr_ul_grant();

  // internal function called from tx_pdu (called from main thread)
  bool process_ce(srslte::sch_subh* subh);

  uint32_t get_pid(const uint32_t tti_);

  bool get_ndi_for_new_ul_tx(const uint32_t tti_);

  bool get_ndi_for_new_dl_tx(const uint32_t tti_);

  int run();

  uint32_t get_tti();

  void process_pdu(uint8_t* buff, uint32_t len, pdu_queue::channel_t channel);

  void set_cell_config(const ttcn3_helpers::timing_info_t timing, const cell_config_t cell);
  void set_cell_config_impl(const cell_config_t cell);

  // cell helper
  void set_cell_attenuation(const ttcn3_helpers::timing_info_t timing, const std::string cell_name, const float value);
  void set_cell_attenuation_impl(const std::string cell_name, const float value);

  void add_bcch_dlsch_pdu(const string cell_name, unique_byte_buffer_t pdu);

  void add_ccch_pdu(const ttcn3_helpers::timing_info_t timing, const std::string cell_name, unique_byte_buffer_t pdu);
  void add_ccch_pdu_impl(const std::string cell_name, unique_byte_buffer_t pdu);

  void add_dcch_pdu(const ttcn3_helpers::timing_info_t timing,
                    const std::string                  cell_name,
                    uint32_t                           lcid,
                    unique_byte_buffer_t               pdu,
                    bool                               follow_on_flag);
  void add_dcch_pdu_impl(const std::string cell_name, uint32_t lcid, unique_byte_buffer_t pdu, bool follow_on_flag);

  void add_pch_pdu(unique_byte_buffer_t pdu);

  void step_stack();

  void add_srb(const ttcn3_helpers::timing_info_t timing,
               const std::string                  cell_name,
               const uint32_t                     lcid,
               const pdcp_config_t                pdcp_config);
  void add_srb_impl(const std::string cell_name, const uint32_t lcid, const pdcp_config_t pdcp_config);

  void reestablish_bearer(const std::string cell_name, const uint32_t lcid);

  void del_srb(const ttcn3_helpers::timing_info_t timing, const std::string cell_name, const uint32_t lcid);
  void del_srb_impl(const std::string cell_name, const uint32_t lcid);

  void add_drb(const ttcn3_helpers::timing_info_t timing,
               const std::string                  cell_name,
               const uint32_t                     lcid,
               const pdcp_config_t                pdcp_config);
  void add_drb_impl(const std::string cell_name, const uint32_t lcid, const pdcp_config_t pdcp_config);
  void del_drb(const ttcn3_helpers::timing_info_t timing, const std::string cell_name, const uint32_t lcid);
  void del_drb_impl(const std::string cell_name, const uint32_t lcid);

  // RRC interface for PDCP, PDCP calls RRC to push RRC SDU
  void write_pdu(uint32_t lcid, unique_byte_buffer_t pdu);

  // Not supported right now
  void write_pdu_bcch_bch(unique_byte_buffer_t pdu);
  void write_pdu_bcch_dlsch(unique_byte_buffer_t pdu);
  void write_pdu_pcch(unique_byte_buffer_t pdu);
  void write_pdu_mch(uint32_t lcid, unique_byte_buffer_t pdu);
  void max_retx_attempted();

  std::string get_rb_name(uint32_t lcid);

  void write_sdu(uint32_t lcid, unique_byte_buffer_t sdu);

  void discard_sdu(uint32_t lcid, uint32_t sn);

  bool rb_is_um(uint32_t lcid);

  bool sdu_queue_is_full(uint32_t lcid);

  void set_as_security(const ttcn3_helpers::timing_info_t        timing,
                       const std::string                         cell_name,
                       std::array<uint8_t, 32>                   k_rrc_enc_,
                       std::array<uint8_t, 32>                   k_rrc_int_,
                       std::array<uint8_t, 32>                   k_up_enc_,
                       const srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                       const srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo_,
                       const ttcn3_helpers::pdcp_count_map_t     bearers_);

  void set_as_security_impl(const std::string                         cell_name,
                            std::array<uint8_t, 32>                   k_rrc_enc_,
                            std::array<uint8_t, 32>                   k_rrc_int_,
                            std::array<uint8_t, 32>                   k_up_enc_,
                            const srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                            const srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo_,
                            const ttcn3_helpers::pdcp_count_map_t     bearers);

  void release_as_security(const ttcn3_helpers::timing_info_t timing, const std::string cell_name);

  void release_as_security_impl(const std::string cell_name);

  void select_cell(srslte_cell_t phy_cell);

  ttcn3_helpers::pdcp_count_map_t get_pdcp_count(const std::string cell_name);

private:
  // SYS interface
  ttcn3_ut_interface      ut;
  ttcn3_sys_interface     sys;
  ttcn3_ip_sock_interface ip_sock;
  ttcn3_ip_ctrl_interface ip_ctrl;
  ttcn3_srb_interface     srb;
  ttcn3_drb_interface     drb;

  // Epoll
  int                                epoll_fd  = -1;
  int                                signal_fd = -1; ///< FD for signals
  std::map<uint32_t, epoll_handler*> event_handler;  ///< Lookup table for handler
  epoll_timer_handler                timer_handler;
  epoll_signal_handler               signal_handler;

  // Logging stuff
  srslte::logger&    logger_stdout;
  swappable_log&     logger_file;
  srslte::logger*    logger = nullptr;
  srslte::log_ref    log;
  srslte::log_filter ut_log;
  srslte::log_filter sys_log;
  srslte::log_filter ip_sock_log;
  srslte::log_filter ip_ctrl_log;
  srslte::log_filter srb_log;
  srslte::log_filter drb_log;
  srslte::log_ref    ss_mac_log{"SS-MAC"};
  srslte::log_ref    ss_rlc_log{"SS-RLC"};
  srslte::log_ref    ss_pdcp_log{"SS-PDCP"};

  all_args_t args = {};

  srslte::byte_buffer_pool* pool = nullptr;

  // Simulator vars
  ttcn3_ue* ue      = nullptr;
  bool      running = false;

  typedef enum { UE_SWITCH_ON = 0, UE_SWITCH_OFF, ENABLE_DATA, DISABLE_DATA } ss_events_t;
  block_queue<ss_events_t> event_queue;

  uint32_t run_id = 0;

  int32_t                 tti                  = 0;
  int32_t                 prach_tti            = -1;
  int32_t                 rar_tti              = -1;
  int32_t                 msg3_tti             = -1;
  int32_t                 sr_tti               = -1;
  uint32_t                prach_preamble_index = 0;
  uint16_t                dl_rnti              = 0;
  int                     force_lcid           = -1;
  srsue::stack_test_dummy stack;
  bool                    last_dl_ndi[SRSLTE_FDD_NOF_HARQ] = {};
  bool                    last_ul_ndi[SRSLTE_FDD_NOF_HARQ] = {};

  // For events/actions that need to be carried out in a specific TTI
  typedef std::queue<move_task_t>                task_queue_t;
  typedef std::map<const uint32_t, task_queue_t> tti_action_map_t;
  tti_action_map_t                              tti_actions;

  // Map between the cellId (name) used by 3GPP test suite and srsLTE cell struct
  class syssim_cell_t
  {
  public:
    syssim_cell_t(ttcn3_syssim* ss) :
      rlc(ss->ss_rlc_log->get_service_name().c_str()),
      pdcp(&ss->stack.task_sched, ss->ss_pdcp_log->get_service_name().c_str())
    {}

    cell_config_t                     config;
    std::vector<unique_byte_buffer_t> sibs;
    int                               sib_idx = 0; ///< Index of SIB scheduled for next transmission

    // Simulator objects
    srslte::rlc              rlc;
    srslte::pdcp             pdcp;
    std::map<uint32_t, bool> bearer_follow_on_map; ///< Indicates if for a given LCID the follow_on_flag is set or not

    // security config
    ttcn3_helpers::pdcp_count_map_t pending_bearer_config; ///< List of bearers with pending security configuration
    srslte::as_security_config_t    sec_cfg;
  };
  typedef std::unique_ptr<syssim_cell_t> unique_syssim_cell_t;
  std::vector<unique_syssim_cell_t>      cells;
  int32_t                                pcell_idx = -1;

  // Internal function
  void           update_cell_map();
  bool           syssim_has_cell(std::string cell_name);
  syssim_cell_t* get_cell(const std::string cell_name);
  bool           have_valid_pcell();

  srslte::pdu_queue pdus;
  srslte::sch_pdu   mac_msg_dl, mac_msg_ul;

  // buffer for DL transmissions
  srslte::byte_buffer_t rar_buffer;
  srslte::byte_buffer_t tx_payload_buffer; // Used to buffer final MAC PDU

  uint64_t conres_id = 0;

  std::vector<std::string> rb_id_vec =
      {"SRB0", "SRB1", "SRB2", "DRB1", "DRB2", "DRB3", "DRB4", "DRB5", "DRB6", "DRB7", "DRB8"};

  // port constants
  const std::string listen_address = "0.0.0.0";
  const uint32_t    UT_PORT        = 2222;
  const uint32_t    SYS_PORT       = 2223;
  const uint32_t    IPSOCK_PORT    = 2224;
  const uint32_t    IPCTRL_PORT    = 2225;
  const uint32_t    SRB_PORT       = 2226;
  const uint32_t    DRB_PORT       = 2227;
};

#endif // SRSUE_TTCN3_SYSSIM_H
