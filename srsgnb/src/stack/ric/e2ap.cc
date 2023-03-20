
#include "srsgnb/hdr/stack/ric/e2ap.h"
#include "stdint.h"

e2ap::e2ap(srslog::basic_logger& logger, srsenb::e2_interface_metrics* _gnb_metrics) : logger(logger), e2sm_(logger)
{
  gnb_metrics = _gnb_metrics;
}

e2_ap_pdu_c e2ap::generate_setup_request()
{
  e2_ap_pdu_c pdu;
  init_msg_s& initmsg = pdu.set_init_msg();
  initmsg.load_info_obj(ASN1_E2AP_ID_E2SETUP);

  e2setup_request_s& setup = initmsg.value.e2setup_request();

  setup->transaction_id.crit        = asn1::crit_opts::reject;
  setup->transaction_id.value.value = setup_procedure_transaction_id;
  setup->global_e2node_id.crit      = asn1::crit_opts::reject;
  auto& gnb_                        = setup->global_e2node_id.value.set_gnb();

  gnb_.global_g_nb_id.plmn_id.from_number(plmn_id);
  gnb_.global_g_nb_id.gnb_id.gnb_id().from_number(gnb_id, 28); // TODO: to keep flexric happy, provide them feedback

  setup->ra_nfunctions_added.crit = asn1::crit_opts::reject;
  auto& list                      = setup->ra_nfunctions_added.value;

  setup->ra_nfunctions_added.id = ASN1_E2AP_ID_RA_NFUNCTIONS_ADDED;
  asn1::protocol_ie_single_container_s<ra_nfunction_item_ies_o> item;
  item.load_info_obj(ASN1_E2AP_ID_RA_NFUNCTION_ITEM);
  item.value().ra_nfunction_item().ran_function_id       = 147;
  item.value().ra_nfunction_item().ran_function_revision = 0;

  // pack E2SM-KPM-PDU into ran function definition
  // add function to map
  RANfunction_description add_func;
  add_func.function_desc                                          = "KPM monitor";
  add_func.function_shortname                                     = "ORAN-E2SM-KPM";
  add_func.function_e2_sm_oid                                     = "1.3.6.1.4.1.53148.1.2.2.2";
  add_func.function_instance                                      = 0;
  ran_functions[item.value().ra_nfunction_item().ran_function_id] = add_func;

  auto&                        ran_func_def = item.value().ra_nfunction_item().ran_function_definition;
  srsran::unique_byte_buffer_t buf          = srsran::make_byte_buffer();
  e2sm_.generate_ran_function_description(item.value().ra_nfunction_item().ran_function_id, add_func, buf);
  ran_func_def.resize(buf->N_bytes);
  buf->msg[1] = 0x30; // TODO: needed to keep wireshak happy, need better fix
  std::copy(buf->msg, buf->msg + buf->N_bytes, ran_func_def.data());

  std::string oid_str = "ORAN-E2SM-KPM";
  item.value().ra_nfunction_item().ran_function_oid.resize(oid_str.size());
  item.value().ra_nfunction_item().ran_function_oid.from_string(oid_str);

  setup->ra_nfunctions_added.value.push_back(item);

  setup->e2node_component_cfg_addition.crit = asn1::crit_opts::reject;
  auto& list1                               = setup->e2node_component_cfg_addition.value;
  list1.resize(1);
  list1[0].load_info_obj(ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ITEM);
  e2node_component_cfg_addition_item_s& item1 = list1[0].value().e2node_component_cfg_addition_item();
  item1.e2node_component_interface_type       = e2node_component_interface_type_opts::ng;
  item1.e2node_component_id.set_e2node_component_interface_type_ng().amf_name.from_string("nginterf");
  item1.e2node_component_cfg.e2node_component_request_part.from_string("72657170617274");
  item1.e2node_component_cfg.e2node_component_resp_part.from_string("72657370617274");
  return pdu;
}

e2_ap_pdu_c e2ap::generate_subscription_response(ric_subscription_reponse_t ric_subscription_reponse)
{
  e2_ap_pdu_c           pdu;
  successful_outcome_s& success = pdu.set_successful_outcome();
  success.load_info_obj(ASN1_E2AP_ID_RICSUBSCRIPTION);
  success.crit                     = asn1::crit_opts::reject;
  ricsubscription_resp_s& sub_resp = success.value.ricsubscription_resp();

  sub_resp->ri_crequest_id.crit                   = asn1::crit_opts::reject;
  sub_resp->ri_crequest_id.id                     = ASN1_E2AP_ID_RI_CREQUEST_ID;
  sub_resp->ri_crequest_id.value.ric_requestor_id = ric_subscription_reponse.ric_requestor_id;
  sub_resp->ri_crequest_id.value.ric_instance_id  = ric_subscription_reponse.ric_instance_id;

  sub_resp->ra_nfunction_id.crit   = asn1::crit_opts::reject;
  sub_resp->ra_nfunction_id.id     = ASN1_E2AP_ID_RA_NFUNCTION_ID;
  sub_resp->ra_nfunction_id->value = ric_subscription_reponse.ra_nfunction_id;

  sub_resp->ri_cactions_admitted.crit = asn1::crit_opts::reject;
  auto& action_admit_list             = sub_resp->ri_cactions_admitted.value;
  action_admit_list.resize(ric_subscription_reponse.admitted_actions.size());
  for (uint32_t i = 0; i < ric_subscription_reponse.admitted_actions.size(); i++) {
    action_admit_list[i].load_info_obj(ASN1_E2AP_ID_RI_CACTION_ADMITTED_ITEM);
    ri_caction_admitted_item_s& a_item = action_admit_list[i]->ri_caction_admitted_item();
    a_item.ric_action_id               = ric_subscription_reponse.admitted_actions[i];
  }

  if (ric_subscription_reponse.not_admitted_actions.size()) {
    sub_resp->ri_cactions_not_admitted.crit = asn1::crit_opts::reject;
    auto& action_not_admit_list             = sub_resp->ri_cactions_not_admitted.value;
    action_not_admit_list.resize(ric_subscription_reponse.not_admitted_actions.size());
    for (uint32_t i = 0; i < ric_subscription_reponse.not_admitted_actions.size(); i++) {
      action_not_admit_list[i].load_info_obj(ASN1_E2AP_ID_RI_CACTION_NOT_ADMITTED_ITEM);
      ri_caction_not_admitted_item_s& not_a_item = action_not_admit_list[i]->ri_caction_not_admitted_item();
      not_a_item.ric_action_id                   = ric_subscription_reponse.not_admitted_actions[i];
      not_a_item.cause.set_misc(); // TODO: support cause properly
    }
  }

  return pdu;
}

e2_ap_pdu_c e2ap::generate_subscription_delete_response(uint32_t ric_requestor_id,
                                                        uint32_t ric_instance_id,
                                                        uint32_t ra_nfunction_id)
{
  e2_ap_pdu_c           pdu;
  successful_outcome_s& success = pdu.set_successful_outcome();
  success.load_info_obj(ASN1_E2AP_ID_RICSUBSCRIPTION_DELETE);
  success.crit                            = asn1::crit_opts::reject;
  ricsubscription_delete_resp_s& sub_resp = success.value.ricsubscription_delete_resp();

  sub_resp->ri_crequest_id.crit              = asn1::crit_opts::reject;
  sub_resp->ri_crequest_id->ric_requestor_id = ric_requestor_id;
  sub_resp->ri_crequest_id->ric_instance_id  = ric_instance_id;

  sub_resp->ra_nfunction_id.crit   = asn1::crit_opts::reject;
  sub_resp->ra_nfunction_id->value = ra_nfunction_id;

  return pdu;
}

int e2ap::process_setup_response(e2setup_resp_s setup_response)
{
  setup_response_received = true;
  if (setup_procedure_transaction_id == setup_response->transaction_id.value.value) {
    setup_procedure_transaction_id++;
  } else {
    logger.error("Received setup response with wrong transaction id");
  }
  global_ric_id.plmn_id = setup_response->global_ric_id.value.plmn_id.to_number();
  global_ric_id.ric_id  = setup_response->global_ric_id.value.ric_id.to_number();

  if (setup_response->ra_nfunctions_accepted_present) {
    for (int i = 0; i < (int)setup_response->ra_nfunctions_accepted.value.size(); i++) {
      uint32_t ran_func_id = setup_response->ra_nfunctions_accepted.value[i]->ra_nfunction_id_item().ran_function_id;
      if (ran_functions.find(ran_func_id) == ran_functions.end()) {
        logger.error("Received setup response with unknown ran function id %d", ran_func_id);
      } else {
        logger.info("Received setup response with ran function id %d", ran_func_id);
        ran_functions[ran_func_id].accepted = true;
      }
    }
  }
  return 0;
}

int e2ap::process_subscription_request(ricsubscription_request_s subscription_request)
{
  pending_subscription_request = true;

  uint16_t ran_func_id = subscription_request->ra_nfunction_id->value;
  if (ran_functions.count(ran_func_id)) {
    // TODO handle RIC subscription request
  }

  // TODO handle RIC subscription request

  return 0;
}

e2_ap_pdu_c e2ap::generate_indication()
{
  using namespace asn1::e2ap;
  e2_ap_pdu_c pdu;

  init_msg_s& initmsg = pdu.set_init_msg();
  initmsg.load_info_obj(ASN1_E2AP_ID_RI_CIND);
  initmsg.crit = asn1::crit_opts::reject;

  ri_cind_s& indication = initmsg.value.ri_cind();

  indication->ri_crequest_id.crit                   = asn1::crit_opts::reject;
  indication->ri_crequest_id.value.ric_requestor_id = 1021;
  indication->ri_crequest_id.value.ric_instance_id  = 0;

  indication->ra_nfunction_id.crit  = asn1::crit_opts::reject;
  indication->ra_nfunction_id.value = 147;

  indication->ri_caction_id.crit  = asn1::crit_opts::reject;
  indication->ri_caction_id.value = 0;

  indication->ri_cind_type.crit = asn1::crit_opts::reject;
  indication->ri_cind_type.value = ri_cind_type_opts::report;

  indication->ri_cind_hdr.crit = asn1::crit_opts::reject;
  srsran::unique_byte_buffer_t header_buf          = srsran::make_byte_buffer();
  e2sm_.generate_indication_header(header_buf);
  indication->ri_cind_hdr->resize(header_buf->N_bytes);
  std::copy(header_buf->msg, header_buf->msg + header_buf->N_bytes, indication->ri_cind_hdr->data());

  indication->ri_cind_msg.crit = asn1::crit_opts::reject;
  srsran::unique_byte_buffer_t msg_buf          = srsran::make_byte_buffer();
  e2sm_.generate_indication_message(msg_buf);
  indication->ri_cind_msg->resize(msg_buf->N_bytes);
  std::copy(msg_buf->msg, msg_buf->msg + msg_buf->N_bytes, indication->ri_cind_msg->data());

  return pdu;
}

e2_ap_pdu_c e2ap::generate_reset_request()
{
  using namespace asn1::e2ap;
  e2_ap_pdu_c pdu;
  init_msg_s& request = pdu.set_init_msg();
  request.load_info_obj(ASN1_E2AP_ID_RESET);
  reset_request_s& reset_request            = request.value.reset_request();
  reset_request->transaction_id.crit        = asn1::crit_opts::reject;
  reset_request->transaction_id.value.value = reset_transaction_id;
  reset_request->cause.crit                 = asn1::crit_opts::ignore;
  reset_request->cause.value.set_misc();
  return pdu;
}

e2_ap_pdu_c e2ap::generate_reset_response()
{
  e2_ap_pdu_c           pdu;
  successful_outcome_s& response = pdu.set_successful_outcome();
  response.load_info_obj(ASN1_E2AP_ID_RESET);
  reset_resp_s& reset_response               = response.value.reset_resp();
  reset_response->transaction_id.crit        = asn1::crit_opts::reject;
  reset_response->transaction_id.value.value = reset_transaction_id;
  return pdu;
}

int e2ap::process_reset_request(reset_request_s reset_request)
{
  reset_id = reset_request->transaction_id.value;

  // TO DO: Parse and store the cause for future extension of the ric client

  return 0;
}

int e2ap::process_reset_response(reset_resp_s reset_response)
{
  // TO DO process reset response from RIC
  reset_response_received = true;

  return 0;
}

int e2ap::get_reset_id()
{
  return reset_id;
}
