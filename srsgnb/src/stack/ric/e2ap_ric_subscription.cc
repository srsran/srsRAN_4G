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

#include "srsgnb/hdr/stack/ric/e2ap_ric_subscription.h"

e2ap::ric_subscription::ric_subscription(e2ap* e2ap, ricsubscription_request_s ric_subscription_request) :
  parent(e2ap),
  initialized(false),
  ric_requestor_id(ric_subscription_request->ri_crequest_id->ric_requestor_id),
  ric_instance_id(ric_subscription_request->ri_crequest_id->ric_instance_id),
  ra_nfunction_id(ric_subscription_request->ra_nfunction_id->value),
  reporting_timer(parent->task_sched_ptr->get_unique_timer())
{
  RANfunction_description ran_func_desc;
  if (!parent->get_func_desc(ra_nfunction_id, ran_func_desc)) {
    parent->logger.debug("Cannot find RAN function with ID: %i\n", ra_nfunction_id);
    this->_send_subscription_failure();
    return;
  }

  sm_ptr = ran_func_desc.sm_ptr;
  if (sm_ptr == nullptr) {
    parent->logger.debug("No valid pointer to SM with RAN function id: %i\n", ra_nfunction_id);
    this->_send_subscription_failure();
    return;
  }

  RIC_event_trigger_definition_t event_trigger;
  if (sm_ptr->process_ric_event_trigger_definition(ric_subscription_request, event_trigger)) {
    if (event_trigger.type == RIC_event_trigger_definition_t::e2sm_event_trigger_type_t::E2SM_REPORT) {
      reporting_period = event_trigger.report_period;
      reporting_period = 3000; // TODO: to remove, keep it 3s for testing
    }
  }

  ri_cactions_to_be_setup_list_l& action_list =
      ric_subscription_request->ricsubscription_details->ric_action_to_be_setup_list;

  for (uint32_t i = 0; i < action_list.size(); i++) {
    ri_caction_to_be_setup_item_s action_item = action_list[i]->ri_caction_to_be_setup_item();

    E2AP_RIC_action_t candidate_action;
    candidate_action.ric_action_id   = action_item.ric_action_id;
    candidate_action.ric_action_type = action_item.ric_action_type;

    if (sm_ptr->process_ric_action_definition(action_item, candidate_action)) {
      parent->logger.debug("Admitted action %i (type: %i), mapped to SM local action ID: %i",
                           candidate_action.ric_action_id,
                           candidate_action.ric_action_type,
                           candidate_action.sm_local_ric_action_id);

      printf("Admitted action %i, mapped to  SM local action ID: %i\n",
             candidate_action.ric_action_id,
             candidate_action.sm_local_ric_action_id);

      admitted_actions.push_back(candidate_action);

      if (action_item.ric_subsequent_action_present) {
        parent->logger.debug("--Action %i (type: %i) contains subsequent action of type %i with wait time: %i",
                             action_item.ric_action_id,
                             action_item.ric_action_type,
                             action_item.ric_subsequent_action.ric_subsequent_action_type,
                             action_item.ric_subsequent_action.ric_time_to_wait);
      }
    } else {
      parent->logger.debug("Not admitted action %i (type: %i)", action_item.ric_action_id, action_item.ric_action_type);
      not_admitted_actions.push_back(action_item.ric_action_id);
    }
  }

  if (admitted_actions.size() == 0) {
    parent->logger.debug("No Action admitted -> remove subscription for RAN function id: %i", ra_nfunction_id);
    printf("No Action admitted -> remove subscription for RAN function id: %i\n", ra_nfunction_id);
    this->_send_subscription_failure();
    return;
  }

  initialized = true;
}

void e2ap::ric_subscription::start_subscription()
{
  this->_send_subscription_response();

  if (reporting_period) {
    printf("Start sending RIC indication msgs every %i ms\n", reporting_period);
    parent->logger.debug("Start sending RIC indication msgs every %i ms", reporting_period);
    reporting_timer.set(reporting_period, [this](uint32_t tid) { _send_ric_indication(); });
    reporting_timer.run();
  }
}

void e2ap::ric_subscription::_send_subscription_response()
{
  parent->logger.debug("Send RIC Subscription Response to RIC Requestor ID: %i\n", ric_requestor_id);
  ric_subscription_reponse_t ric_subscription_reponse;
  ric_subscription_reponse.ric_requestor_id = ric_requestor_id;
  ric_subscription_reponse.ric_instance_id  = ric_instance_id;
  ric_subscription_reponse.ra_nfunction_id  = ra_nfunction_id;

  for (auto& action : admitted_actions) {
    ric_subscription_reponse.admitted_actions.push_back(action.ric_action_id);
  }

  for (auto& action : not_admitted_actions) {
    ric_subscription_reponse.not_admitted_actions.push_back(action);
  }

  e2_ap_pdu_c send_pdu = parent->generate_subscription_response(ric_subscription_reponse);
  parent->queue_send_e2ap_pdu(send_pdu);
}

void e2ap::ric_subscription::_send_subscription_failure()
{
  parent->logger.debug("Send RIC Subscription Failure Response to RIC Requestor ID: %i\n", ric_requestor_id);
  ric_subscription_reponse_t ric_subscription_reponse;
  ric_subscription_reponse.ric_requestor_id = ric_requestor_id;
  ric_subscription_reponse.ric_instance_id  = ric_instance_id;
  ric_subscription_reponse.ra_nfunction_id  = ra_nfunction_id;

  e2_ap_pdu_c send_pdu = parent->generate_subscription_failure(ric_subscription_reponse);
  parent->queue_send_e2ap_pdu(send_pdu);
}

void e2ap::ric_subscription::delete_subscription()
{
  if (reporting_timer.is_running()) {
    parent->logger.debug("Stop sending RIC indication msgs");
    reporting_timer.stop();
  }

  ric_subscription_reponse_t ric_subscription_reponse;
  ric_subscription_reponse.ric_requestor_id = ric_requestor_id;
  ric_subscription_reponse.ric_instance_id  = ric_instance_id;
  ric_subscription_reponse.ra_nfunction_id  = ra_nfunction_id;

  // remove registered actions from SM
  if (sm_ptr) {
    for (auto& action : admitted_actions) {
      sm_ptr->remove_ric_action_definition(action);
    }
  } else {
    e2_ap_pdu_c send_pdu = parent->generate_subscription_delete_failure(ric_subscription_reponse);
    parent->queue_send_e2ap_pdu(send_pdu);
    return;
  }

  parent->logger.debug("Send RIC Subscription Delete Response to RIC Requestor ID: %i\n", ric_requestor_id);

  e2_ap_pdu_c send_pdu = parent->generate_subscription_delete_response(ric_subscription_reponse);
  parent->queue_send_e2ap_pdu(send_pdu);
}

bool e2ap::ric_subscription::process_subscription_modification_request(uint32_t ric_subscription_modification_request)
{
  // TODO: implement, currently not supported in ans1
  return false;
}

bool e2ap::ric_subscription::process_subscription_modification_confirm(uint32_t ric_subscription_modification_confirm)
{
  // TODO: implement, currently not supported in ans1
  return false;
}

bool e2ap::ric_subscription::process_subscription_modification_refuse(uint32_t ric_subscription_modification_refuse)
{
  // TODO: implement, currently not supported in ans1
  return false;
}

uint32_t e2ap::ric_subscription::_generate_ric_indication_sn()
{
  uint32_t sn = _ric_indication_sn_gen;
  _ric_indication_sn_gen++;
  if (_ric_indication_sn_gen > 65535) {
    _ric_indication_sn_gen = 0;
  }
  return sn;
};

void e2ap::ric_subscription::_send_ric_indication()
{
  if (sm_ptr == nullptr) {
    parent->logger.error("SM pointer not set in subscription: %i\n", ric_requestor_id);
    return;
  }

  for (auto& action : admitted_actions) {
    printf("Sending RIC indication msg to RIC Requestor ID: %i\n", ric_requestor_id);
    ric_indication_t ric_indication;
    ric_indication.ric_requestor_id = ric_requestor_id;
    ric_indication.ric_instance_id  = ric_instance_id;
    ric_indication.ra_nfunction_id  = ra_nfunction_id;
    ric_indication.ri_caction_id    = action.ric_action_id;
    ric_indication.ri_indication_sn_present = true;
    ric_indication.ri_indication_sn         = _generate_ric_indication_sn();
    if (sm_ptr->generate_ric_indication_content(action, ric_indication)) {
      e2_ap_pdu_c send_pdu = parent->generate_indication(ric_indication);
      parent->queue_send_e2ap_pdu(send_pdu);
    }
  }

  // reschedule sending RIC indication
  if (reporting_period) {
    reporting_timer.run();
  }
}