/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSUE_NAS_H
#define SRSUE_NAS_H

#include "nas_base.h"
#include "srsran/asn1/liblte_mme.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/nas_pcap.h"
#include "srsran/common/security.h"
#include "srsran/common/stack_procedure.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/interfaces/ue_nas_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "srsue/hdr/stack/upper/nas_config.h"
#include "srsue/hdr/stack/upper/nas_emm_state.h"
#include "srsue/hdr/stack/upper/nas_metrics.h"

using srsran::byte_buffer_t;

namespace srsue {

class usim_interface_nas;
class gw_interface_nas;
class rrc_interface_nas;

/**
 * @brief This class implements the NAS layer of a EUTRA UE.
 *
 * The class is *NOT* thread-safe.
 *
 */

class nas : public nas_interface_rrc, public srsran::timer_callback, public nas_base
{
public:
  explicit nas(srslog::basic_logger& logger_, srsran::task_sched_handle task_sched_);
  virtual ~nas();
  int  init(usim_interface_nas* usim_, rrc_interface_nas* rrc_, gw_interface_nas* gw_, const nas_args_t& args_);
  void stop();
  void run_tti();

  // Metrics getter
  void get_metrics(nas_metrics_t* m);

  // RRC interface
  void     left_rrc_connected() override;
  bool     connection_request_completed(bool outcome) override;
  bool     paging(srsran::s_tmsi_t* ue_identity) override;
  void     set_barring(srsran::barring_t barring) override;
  void     write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu) override;
  uint32_t get_k_enb_count() override;
  bool     get_k_asme(uint8_t* k_asme_, uint32_t n) override;
  uint32_t get_ipv4_addr() override;
  bool     get_ipv6_addr(uint8_t* ipv6_addr) override;
  void     plmn_search_completed(const found_plmn_t found_plmns[MAX_FOUND_PLMNS], int nof_plmns) final;

  // Stack interface
  bool switch_on();
  bool switch_off();
  bool enable_data();
  bool disable_data();
  void start_service_request(srsran::establishment_cause_t cause_);

  // Stack+RRC interface
  bool is_registered() override;

  // timer callback
  void timer_expired(uint32_t timeout_id) override;

private:
  rrc_interface_nas*  rrc  = nullptr;
  usim_interface_nas* usim = nullptr;
  gw_interface_nas*   gw   = nullptr;

  bool running = false;

  nas_args_t  cfg   = {};
  emm_state_t state = {};

  srsran::barring_t current_barring = srsran::barring_t::none;

  srsran::plmn_id_t current_plmn;
  srsran::plmn_id_t home_plmn;

  std::vector<srsran::plmn_id_t> known_plmns;

  typedef enum { DEFAULT_EPS_BEARER = 0, DEDICATED_EPS_BEARER } eps_bearer_type_t;

  typedef struct {
    eps_bearer_type_t type;
    uint8_t           eps_bearer_id;
    uint8_t           linked_eps_bearer_id;
  } eps_bearer_t;

  typedef std::map<uint8_t, eps_bearer_t>  eps_bearer_map_t;
  typedef std::pair<uint8_t, eps_bearer_t> eps_bearer_map_pair_t;
  eps_bearer_map_t                         eps_bearer;

  bool    have_guti       = false;
  bool    have_ctxt       = false;
  bool    auth_request    = false;
  uint8_t current_sec_hdr = LIBLTE_MME_SECURITY_HDR_TYPE_PLAIN_NAS;

  const uint32_t max_attach_attempts    = 5; // Sec. 5.5.1.2.6
  uint32_t       attach_attempt_counter = 0;

  uint32_t ip_addr       = 0;
  uint8_t  ipv6_if_id[8] = {};

  uint8_t chap_id = 0;

  uint8_t transaction_id = 0;

  // timers
  srsran::task_sched_handle           task_sched;
  srsran::timer_handler::unique_timer t3402;          // started when attach attempt counter reached 5
  srsran::timer_handler::unique_timer t3410;          // started when attach request is sent, on expiry, start t3411
  srsran::timer_handler::unique_timer t3411;          // started when attach failed
  srsran::timer_handler::unique_timer t3421;          // started when detach request is sent
  srsran::timer_handler::unique_timer reattach_timer; // started to trigger delayed re-attach

  // Values according to TS 24.301 Sec 10.2
  const uint32_t t3402_duration_ms          = 12 * 60 * 1000; // 12m
  const uint32_t t3410_duration_ms          = 15 * 1000;      // 15s
  const uint32_t t3411_duration_ms          = 10 * 1000;      // 10s
  const uint32_t t3421_duration_ms          = 15 * 1000;      // 15s
  const uint32_t reattach_timer_duration_ms = 2 * 1000;       // 2s (arbitrarily chosen to delay re-attach)

  // TS 23.003 Sec. 6.2.2 IMEISV's last two octets are Software Version Number (SVN)
  // which identifies the software version number of the mobile equipment
  const uint8_t ue_svn_oct1 = 0x5;
  const uint8_t ue_svn_oct2 = 0x3;

  // Security
  bool eia_caps[8] = {};
  bool eea_caps[8] = {};

  // Airplane mode simulation
  typedef enum { DISABLED = 0, ENABLED } airplane_mode_state_t;
  airplane_mode_state_t               airplane_mode_state = {};
  srsran::timer_handler::unique_timer airplane_mode_sim_timer;

  // Security
  int  apply_security_config(srsran::unique_byte_buffer_t& pdu, uint8_t sec_hdr_type);
  void reset_security_context();
  void set_k_enb_count(uint32_t count);
  bool check_cap_replay(LIBLTE_MME_UE_SECURITY_CAPABILITIES_STRUCT* caps);

  // NAS Connection Initiation/Termination
  void start_attach_request(srsran::establishment_cause_t cause_);
  bool detach_request(const bool switch_off);

  // PLMN Selection Helpers
  void start_plmn_selection_proc();
  void select_plmn();

  // Parsers
  void parse_attach_accept(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void parse_attach_reject(uint32_t lcid, srsran::unique_byte_buffer_t pdu, const uint8_t sec_hdr_type);
  void parse_authentication_request(uint32_t lcid, srsran::unique_byte_buffer_t pdu, const uint8_t sec_hdr_type);
  void parse_authentication_reject(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void parse_identity_request(srsran::unique_byte_buffer_t pdu, const uint8_t sec_hdr_type);
  void parse_security_mode_command(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void parse_service_reject(uint32_t lcid, srsran::unique_byte_buffer_t pdu, const uint8_t sec_hdr_type);
  void parse_esm_information_request(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void parse_emm_information(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void parse_detach_request(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void parse_emm_status(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void parse_activate_dedicated_eps_bearer_context_request(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void parse_deactivate_eps_bearer_context_request(srsran::unique_byte_buffer_t pdu);
  void parse_activate_test_mode(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void parse_close_ue_test_loop(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void parse_modify_eps_bearer_context_request(srsran::unique_byte_buffer_t pdu);

  // Packet generators
  void gen_attach_request(srsran::unique_byte_buffer_t& msg);
  void gen_service_request(srsran::unique_byte_buffer_t& msg);

  // Senders
  void send_attach_complete(const uint8_t& transaction_id, const uint8_t& eps_bearer_id);
  void send_identity_response(uint8 id_type);
  void send_service_request();
  void send_esm_information_response(const uint8 proc_transaction_id);
  void send_authentication_response(const uint8_t* res, const size_t res_len);
  void send_authentication_failure(const uint8_t cause, const uint8_t* auth_fail_param);
  void gen_pdn_connectivity_request(LIBLTE_BYTE_MSG_STRUCT* msg);
  void send_security_mode_reject(uint8_t cause);
  void send_attach_request();
  void send_detach_request(bool switch_off);
  void send_detach_accept();
  void send_activate_dedicated_eps_bearer_context_accept(const uint8_t& proc_transaction_id,
                                                         const uint8_t& eps_bearer_id);
  void send_deactivate_eps_bearer_context_accept(const uint8_t& proc_transaction_id, const uint8_t& eps_bearer_id);
  void send_modify_eps_bearer_context_accept(const uint8_t& proc_transaction_id, const uint8_t& eps_bearer_id);
  void send_activate_test_mode_complete();
  void send_close_ue_test_loop_complete();

  // Airplane mode simulator helpers
  void start_airplane_mode_sim();
  void airplane_mode_sim_switch_off();
  void airplane_mode_sim_switch_on();

  // Misc helpers
  void clear_eps_bearer();

  // FSM Helpers
  void enter_state(emm_state_t state_);
  void enter_emm_null();
  void enter_emm_deregistered(emm_state_t::deregistered_substate_t substate);
  void enter_emm_deregistered_initiated();

  // security context persistence file
  bool read_ctxt_file(nas_sec_ctxt* ctxt_, nas_sec_base_ctxt* ctxt_base_);
  bool write_ctxt_file(nas_sec_ctxt ctxt_, nas_sec_base_ctxt ctxt_base_);

  // ctxt file helpers
  std::string hex_to_string(uint8_t* hex, int size);
  bool        string_to_hex(std::string hex_str, uint8_t* hex, uint32_t len);
  std::string emm_info_str(LIBLTE_MME_EMM_INFORMATION_MSG_STRUCT* info);

  template <class T>
  bool readvar(std::istream& file, const char* key, T* var)
  {
    std::string line;
    size_t      len = strlen(key);
    std::getline(file, line);
    if (line.substr(0, len).compare(key)) {
      return false;
    }
    *var = (T)strtol(line.substr(len).c_str(), NULL, 10);
    return true;
  }

  bool readvar(std::istream& file, const char* key, uint8_t* var, int varlen)
  {
    std::string line;
    size_t      len = strlen(key);
    std::getline(file, line);
    if (line.substr(0, len).compare(key)) {
      return false;
    }
    std::string tmp = line.substr(len);
    if (!string_to_hex(tmp, var, varlen)) {
      return false;
    }
    return true;
  }

  // NAS Idle procedures
  class plmn_search_proc; // PLMN selection proc (fwd declared)

  srsran::proc_manager_list_t      callbacks;
  srsran::proc_t<plmn_search_proc> plmn_searcher;
};

} // namespace srsue

#endif // SRSUE_NAS_H
