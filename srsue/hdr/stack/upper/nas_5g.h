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

#ifndef SRSUE_NAS_5G_H
#define SRSUE_NAS_5G_H

#include "nas_base.h"
#include "srsran/asn1/nas_5g_ies.h"
#include "srsran/asn1/nas_5g_msg.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/nas_pcap.h"
#include "srsran/common/security.h"
#include "srsran/common/stack_procedure.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/interfaces/ue_nas_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/interfaces/ue_usim_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "srsue/hdr/stack/upper/nas_5g_metrics.h"
#include "srsue/hdr/stack/upper/nas_5gmm_state.h"
#include "srsue/hdr/stack/upper/nas_config.h"

using srsran::byte_buffer_t;

#define MAX_PDU_SESSIONS 15
#define MAX_TRANS_ID 255

namespace srsue {

/**
 * @brief This class implements the NAS layer of a 5G UE.
 *
 * The class is *NOT* thread-safe.
 *
 */
class nas_5g : public nas_base, public nas_5g_interface_rrc_nr, public nas_5g_interface_procedures
{
public:
  explicit nas_5g(srslog::basic_logger& logger_, srsran::task_sched_handle task_sched_);
  virtual ~nas_5g();
  int  init(usim_interface_nas*      usim_,
            rrc_nr_interface_nas_5g* rrc_nr_,
            gw_interface_nas*        gw_,
            const nas_5g_args_t&     cfg_);
  void stop();
  void run_tti();

  // Stack+RRC interface
  bool is_registered();

  int write_pdu(srsran::unique_byte_buffer_t pdu);

  // timer callback
  void timer_expired(uint32_t timeout_id);

  // Stack interface
  int switch_on();
  int switch_off();
  int enable_data();
  int disable_data();
  int start_service_request();

  // Metrics getter
  void get_metrics(nas_5g_metrics_t& metrics);

private:
  rrc_nr_interface_nas_5g* rrc_nr = nullptr;
  usim_interface_nas*      usim   = nullptr;
  gw_interface_nas*        gw     = nullptr;

  bool running = false;

  bool                                             initial_sec_command = false;
  srsran::nas_5g::mobile_identity_5gs_t::guti_5g_s guti_5g;

  srsran::nas_5g::nas_5gs_msg initial_registration_request_stored;

  nas_5g_args_t cfg = {};
  mm5g_state_t  state;

  // Security
  bool ia5g_caps[8] = {};
  bool ea5g_caps[8] = {};

  // TS 23.003 Sec. 6.2.2 IMEISV's last two octets are Software Version Number (SVN)
  // which identifies the software version number of the mobile equipment
  const uint8_t ue_svn_oct1 = 0x5;
  const uint8_t ue_svn_oct2 = 0x3;

  // timers
  srsran::task_sched_handle           task_sched;
  srsran::timer_handler::unique_timer t3502; // started when registration failure and the attempt counter is equal to 5
  srsran::timer_handler::unique_timer t3510; // started when transmission of REGISTRATION REQUEST message. ON EXPIRY:
                                             // start T3511 or T3502 as specified in subclause 5.5.1.2.7
  srsran::timer_handler::unique_timer t3511; // started when registration failure due to lower layer failure
  srsran::timer_handler::unique_timer t3521; // started when detach request is sent
  srsran::timer_handler::unique_timer reregistration_timer; // started to trigger delayed re-attach

  // Values according to TS 24.501 Sec 10.2
  const uint32_t t3502_duration_ms                = 12 * 60 * 1000; // 12m
  const uint32_t t3510_duration_ms                = 15 * 1000;      // 15s
  const uint32_t t3511_duration_ms                = 10 * 1000;      // 10s
  const uint32_t t3521_duration_ms                = 15 * 1000;      // 15s
  const uint32_t reregistration_timer_duration_ms = 2 * 1000;       // 2s (arbitrarily chosen to delay re-attach)

  srsran::proc_manager_list_t callbacks;

  // Procedures
  // Forward declartion
  class registration_procedure;
  class pdu_session_establishment_procedure;

  srsran::proc_t<registration_procedure>              registration_proc;
  srsran::proc_t<pdu_session_establishment_procedure> pdu_session_establishment_proc;

  // Network information
  srsran::nas_5g::network_name_t full_network_name;

  // Message sender
  int send_registration_request();
  int send_authentication_response(const uint8_t res[16]);
  int send_security_mode_reject(const srsran::nas_5g::cause_5gmm_t::cause_5gmm_type_::options cause);
  int send_authentication_failure(const srsran::nas_5g::cause_5gmm_t::cause_5gmm_type_::options cause,
                                  const uint8_t                                                 res_star[16]);
  int send_security_mode_complete(const srsran::nas_5g::security_mode_command_t& security_mode_command);
  int send_registration_complete();
  int send_pdu_session_establishment_request(uint32_t                 transaction_identity,
                                             uint16_t                 pdu_session_id,
                                             const pdu_session_cfg_t& pdu_session);
  int send_deregistration_request_ue_originating(bool switch_off);
  int send_identity_response(srsran::nas_5g::identity_type_5gs_t::identity_types_::options requested_identity_type);
  int send_configuration_update_complete();

  // Helper functions
  void fill_security_caps(srsran::nas_5g::ue_security_capability_t& sec_caps);
  int  apply_security_config(srsran::unique_byte_buffer_t& pdu, uint8_t sec_hdr_type);
  bool check_replayed_ue_security_capabilities(srsran::nas_5g::ue_security_capability_t& caps);

  // message handler
  int handle_registration_accept(srsran::nas_5g::registration_accept_t& registration_accept);
  int handle_registration_reject(srsran::nas_5g::registration_reject_t& registration_reject);
  int handle_authentication_request(srsran::nas_5g::authentication_request_t& authentication_request);
  int handle_authentication_reject(srsran::nas_5g::authentication_reject_t& authentication_reject);
  int handle_identity_request(srsran::nas_5g::identity_request_t& identity_request);
  int handle_service_accept(srsran::nas_5g::service_accept_t& service_accept);
  int handle_service_reject(srsran::nas_5g::service_reject_t& service_reject);
  int handle_security_mode_command(srsran::nas_5g::security_mode_command_t& security_mode_command,
                                   srsran::unique_byte_buffer_t             pdu);
  int handle_deregistration_accept_ue_terminated(
      srsran::nas_5g::deregistration_accept_ue_terminated_t& deregistration_accept_ue_terminated);
  int handle_deregistration_accept_ue_originating(
      srsran::nas_5g::deregistration_accept_ue_originating_t& deregistration_accept_ue_originating);
  int handle_deregistration_request_ue_terminated(
      srsran::nas_5g::deregistration_request_ue_terminated_t& deregistration_request_ue_terminated);
  int handle_configuration_update_command(srsran::nas_5g::configuration_update_command_t& configuration_update_command);
  int handle_dl_nas_transport(srsran::nas_5g::dl_nas_transport_t& dl_nas_transport);

  // message handler container
  int handle_n1_sm_information(std::vector<uint8_t> payload_container_contents);

  // Transaction ID management
  std::array<bool, MAX_TRANS_ID> pdu_trans_ids;
  uint32_t                       allocate_next_proc_trans_id();
  void                           release_proc_trans_id(uint32_t proc_id);

  int trigger_pdu_session_est();

  // PDU Session Management
  int  add_pdu_session(uint16_t pdu_session_id, uint16_t pdu_session_type, srsran::nas_5g::pdu_address_t pdu_address);
  int  init_pdu_sessions(std::vector<pdu_session_cfg_t> pdu_session_cfgs);
  int  configure_pdu_session(uint16_t pdu_session_id);
  bool unestablished_pdu_sessions();
  int  get_unestablished_pdu_session(uint16_t& pdu_session_id, pdu_session_cfg_t& pdu_session_cfg);
  int  reset_pdu_sessions();
  uint32_t num_of_est_pdu_sessions();

  struct pdu_session_t {
    bool              configured;
    bool              established;
    uint16_t          pdu_session_id;
    pdu_session_cfg_t pdu_session_cfg;
  };

  std::array<pdu_session_t, MAX_PDU_SESSIONS> pdu_sessions;
};
} // namespace srsue
#endif