/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 *
 */

#include "srsgnb/hdr/stack/ric/ric_subscription.h"

namespace srsenb {

ric_client::ric_subscription::ric_subscription(ric_client*               ric_client,
                                               ricsubscription_request_s ric_subscription_request) :
  parent(ric_client),
  ric_requestor_id(ric_subscription_request->ri_crequest_id->ric_requestor_id),
  ric_instance_id(ric_subscription_request->ri_crequest_id->ric_instance_id),
  ra_nfunction_id(ric_subscription_request->ra_nfunction_id->value)
{
  reporting_timer = parent->task_sched.get_unique_timer();

  parent->e2ap_.process_subscription_request(ric_subscription_request);

  RANfunction_description ran_func_desc;
  if (!parent->e2ap_.get_func_desc(ra_nfunction_id, ran_func_desc)) {
    return;
  }

  RIC_event_trigger_definition event_trigger;
  if (ran_func_desc.sm_ptr->process_ric_event_trigger_definition(ric_subscription_request, event_trigger)) {
    if (event_trigger.type == RIC_event_trigger_definition::e2sm_event_trigger_type_t::E2SM_REPORT) {
      reporting_period = event_trigger.report_period;
      reporting_period = 1000; // TODO: to remove, keep it 1s for testing
    }
  }

  ri_cactions_to_be_setup_list_l& action_list =
      ric_subscription_request->ricsubscription_details->ric_action_to_be_setup_list;

  for (uint32_t i = 0; i < action_list.size(); i++) {
    ri_caction_to_be_setup_item_s action_item = action_list[i]->ri_caction_to_be_setup_item();

    if (ran_func_desc.sm_ptr->process_ric_action_definition(action_item)) {
      parent->logger.debug("Admitted action %i (type: %i)\n", action_item.ric_action_id, action_item.ric_action_type);
      admitted_actions.push_back(action_item.ric_action_id);

      if (action_item.ric_subsequent_action_present) {
        parent->logger.debug("--Action %i (type: %i) contains subsequent action of type %i with wait time: %i\n",
                             action_item.ric_action_id,
                             action_item.ric_action_type,
                             action_item.ric_subsequent_action.ric_subsequent_action_type,
                             action_item.ric_subsequent_action.ric_time_to_wait);
      } else {
        parent->logger.debug(
            "Not admitted action %i (type: %i)\n", action_item.ric_action_id, action_item.ric_action_type);
        not_admitted_actions.push_back(action_item.ric_action_id);
      }
    }
  }
}

void ric_client::ric_subscription::start_subscription()
{
  parent->logger.debug("Send RIC Subscription Response to RIC Requestor ID: %i\n", ric_requestor_id);
  ric_subscription_reponse_t ric_subscription_reponse;
  ric_subscription_reponse.ric_requestor_id = ric_requestor_id;
  ric_subscription_reponse.ric_instance_id  = ric_instance_id;
  ric_subscription_reponse.ra_nfunction_id  = ra_nfunction_id;

  for (auto& action : admitted_actions) {
    ric_subscription_reponse.admitted_actions.push_back(action);
  }

  for (auto& action : not_admitted_actions) {
    ric_subscription_reponse.not_admitted_actions.push_back(action);
  }

  e2_ap_pdu_c send_pdu = parent->e2ap_.generate_subscription_response(ric_subscription_reponse);
  parent->queue_send_e2ap_pdu(send_pdu);

  if (reporting_period) {
    printf("Start sending RIC indication msgs every %i ms\n", reporting_period);
    parent->logger.debug("Start sending RIC indication msgs every %i ms", reporting_period);
    reporting_timer.set(reporting_period, [this](uint32_t tid) { send_ric_indication(); });
    reporting_timer.run();
  }
}

void ric_client::ric_subscription::delete_subscription()
{
  if (reporting_timer.is_running()) {
    parent->logger.debug("Stop sending RIC indication msgs");
    reporting_timer.stop();
  }

  parent->logger.debug("Send RIC Subscription Delete Response to RIC Requestor ID: %i\n", ric_requestor_id);
  e2_ap_pdu_c send_pdu =
      parent->e2ap_.generate_subscription_delete_response(ric_requestor_id, ric_instance_id, ra_nfunction_id);
  parent->queue_send_e2ap_pdu(send_pdu);
}

void ric_client::ric_subscription::send_ric_indication()
{
  printf("Sending RIC indication msg to RIC Requestor ID: %i\n", ric_requestor_id);
  ric_indication_t ric_indication;
  ric_indication.ric_requestor_id = ric_requestor_id;
  ric_indication.ric_instance_id  = ric_instance_id;
  ric_indication.ra_nfunction_id  = ra_nfunction_id;
  ric_indication.ri_caction_id    = ri_caction_id;
  ric_indication.indication_type  = ri_cind_type_opts::report;

  RANfunction_description ran_func_desc;
  e2sm*                   sm_ptr = nullptr;
  if (!parent->e2ap_.get_func_desc(ra_nfunction_id, ran_func_desc)) {
    return;
  }

  e2sm_kpm* sm_kpm_ptr = dynamic_cast<e2sm_kpm*>(ran_func_desc.sm_ptr);

  E2SM_KPM_RIC_ind_header ric_ind_header;
  ric_ind_header.collet_start_time = 0x12345;
  ric_indication.ri_cind_hdr       = srsran::make_byte_buffer();
  sm_kpm_ptr->generate_indication_header(ric_ind_header, ric_indication.ri_cind_hdr);

  E2SM_KPM_RIC_ind_message ric_ind_message;
  ric_ind_message.ind_msg_format = e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format1;

  ric_ind_message.meas_data.resize(1);
  ric_ind_message.meas_data[0].meas_record.resize(5);
  for (uint32_t i = 0; i < ric_ind_message.meas_data[0].meas_record.size(); i++) {
    ric_ind_message.meas_data[0].meas_record[i].set_integer() = i * 1000;
  }

  ric_ind_message.meas_info_list.resize(1);
  ric_ind_message.meas_info_list[0].meas_type.set_meas_name().from_string("RRU.PrbTotDl");
  ric_ind_message.meas_info_list[0].label_info_list.resize(1);
  ric_ind_message.meas_info_list[0].label_info_list[0].meas_label.no_label_present = true;
  ric_ind_message.meas_info_list[0].label_info_list[0].meas_label.no_label =
      asn1::e2sm_kpm::meas_label_s::no_label_opts::true_value;

  // ric_ind_message.granul_period = 12345; // not implemented by flexric and crashes it

  ric_indication.ri_cind_msg = srsran::make_byte_buffer();
  sm_kpm_ptr->generate_indication_message(ric_ind_message, ric_indication.ri_cind_msg);

  e2_ap_pdu_c send_pdu = parent->e2ap_.generate_indication(ric_indication);
  parent->queue_send_e2ap_pdu(send_pdu);

  // reschedule sending RIC indication
  reporting_timer.run();
}

} // namespace srsenb