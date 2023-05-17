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

#include "e2sm_kpm.h"
#include "srsran/asn1/e2ap.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/timers.h"
#include "srsran/interfaces/e2_metrics_interface.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/srsran.h"

#ifndef RIC_E2AP_H
#define RIC_E2AP_H

namespace srsenb {
using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;

typedef struct {
  uint16_t ric_id;
  uint16_t plmn_id;
} global_ric_id_t;

typedef struct {
  e2node_component_interface_type_e interface_type;
  std::string                       amf_name;
  std::string                       request_part;
  std::string                       response_part;
} e2_node_component_t;

typedef struct {
  uint32_t              ric_requestor_id;
  uint32_t              ric_instance_id;
  uint32_t              ra_nfunction_id;
  std::vector<uint32_t> admitted_actions;
  std::vector<uint32_t> not_admitted_actions;
} ric_subscription_reponse_t;

class e2_agent;

class e2ap
{
public:
  e2ap(srslog::basic_logger&         logger,
       e2_agent*                     _e2_agent,
       srsenb::e2_interface_metrics* _gnb_metrics,
       srsran::task_scheduler*       _task_sched_ptr);
  ~e2ap();

  e2_ap_pdu_c generate_setup_request();
  int         process_setup_response(e2setup_resp_s setup_response);
  int         process_setup_failure();
  int         process_subscription_request(ricsubscription_request_s ric_subscription_request);
  int         process_subscription_delete_request(ricsubscription_delete_request_s ricsubscription_delete_request);
  int         process_subscription_modification_request(uint32_t ric_subscription_modification_request);
  int         process_subscription_modification_confirm(uint32_t ric_subscription_modification_confirm);
  int         process_subscription_modification_refuse(uint32_t ric_subscription_modification_refuse);
  e2_ap_pdu_c generate_subscription_response(ric_subscription_reponse_t ric_subscription_reponse);
  e2_ap_pdu_c generate_subscription_failure(ric_subscription_reponse_t ric_subscription_reponse);
  e2_ap_pdu_c generate_subscription_delete_response(ric_subscription_reponse_t ric_subscription_reponse);
  e2_ap_pdu_c generate_subscription_delete_failure(ric_subscription_reponse_t ric_subscription_reponse);
  e2_ap_pdu_c generate_subscription_delete_required(ric_subscription_reponse_t ric_subscription_reponse);
  e2_ap_pdu_c generate_subscription_modification_response();
  e2_ap_pdu_c generate_subscription_modification_failure();
  e2_ap_pdu_c generate_subscription_modification_required();

  e2_ap_pdu_c generate_indication(ric_indication_t& ric_indication);
  e2_ap_pdu_c generate_reset_request();
  e2_ap_pdu_c generate_reset_response();
  int         process_reset_request(reset_request_s reset_request);
  int         process_reset_response(reset_resp_s reset_response);

  int process_e2_setup_failure(e2setup_fail_s e2setup_failure);
  int process_ric_service_update_failure(ricservice_upd_fail_s ric_service_update_failure);
  int process_e2_node_config_update_failure(e2node_cfg_upd_fail_s e2node_config_update_failure);
  int process_e2_removal_failure(e2_removal_fail_s e2_remove_failure);

  bool queue_send_e2ap_pdu(e2_ap_pdu_c e2ap_pdu);

  int  get_reset_id();
  bool get_func_desc(uint32_t ran_func_id, RANfunction_description& fdesc);
  bool send_setup_request() { return !e2_established && pending_e2_setup; }

  class ric_subscription;

private:
  srslog::basic_logger& logger;
  e2_agent*             _e2_agent;
  e2sm_kpm              e2sm_;
  bool                  e2_established = false;
  srsran::unique_timer  e2_procedure_timeout;
  bool                  pending_e2_setup              = true;
  bool                  pending_e2_node_config_update = false;
  bool                  pending_ric_service_update    = false;
  bool                  pending_e2_removal            = false;

  int                                         setup_procedure_transaction_id = 0;
  uint64_t                                    plmn_id                        = 0x05f510;
  uint64_t                                    gnb_id                         = 1;
  global_ric_id_t                             global_ric_id                  = {};
  std::map<uint32_t, RANfunction_description> ran_functions;
  srsenb::e2_interface_metrics*               gnb_metrics             = nullptr;
  srsran::task_scheduler*                     task_sched_ptr          = nullptr;
  bool                                        reset_response_received = false;
  int                                         reset_transaction_id    = 1;
  cause_c                                     reset_cause             = cause_c();
  int                                         reset_id                = 1;

  std::vector<std::unique_ptr<ric_subscription> > active_subscriptions;
};
} // namespace srsenb
#endif /* RIC_E2AP_H */
