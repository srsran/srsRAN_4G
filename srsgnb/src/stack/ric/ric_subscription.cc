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
  reporting_period = 1000;
  reporting_timer  = parent->task_sched.get_unique_timer();

  // TODO: process request to know what to report
  parent->e2ap_.process_subscription_request(ric_subscription_request);
}

void ric_client::ric_subscription::start_ric_indication_reporting()
{
  parent->logger.debug("Send RIC Subscription Response to RIC Requestor ID: %i\n", ric_requestor_id);
  ric_subscription_reponse_t ric_subscription_reponse;
  ric_subscription_reponse.ric_requestor_id = ric_requestor_id;
  ric_subscription_reponse.ric_instance_id  = ric_instance_id;
  ric_subscription_reponse.ra_nfunction_id  = ra_nfunction_id;

  ric_subscription_reponse.admitted_actions.push_back(0);
  ric_subscription_reponse.not_admitted_actions.push_back(10);

  e2_ap_pdu_c send_pdu = parent->e2ap_.generate_subscription_response(ric_subscription_reponse);
  parent->queue_send_e2ap_pdu(send_pdu);

  printf("Start sending RIC indication msgs every %i ms\n", reporting_period);
  parent->logger.debug("Start sending RIC indication msgs every %i ms", reporting_period);
  reporting_timer.set(reporting_period, [this](uint32_t tid) { send_ric_indication(); });
  reporting_timer.run();
}

void ric_client::ric_subscription::stop_ric_indication_reporting()
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