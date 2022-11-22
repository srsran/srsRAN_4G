
#include "srsgnb/hdr/stack/ric/e2ap.h"
#include "stdint.h"

e2_ap_pdu_c e2ap::generate_setup_request()
{
  e2_ap_pdu_c pdu;
  init_msg_s& initmsg = pdu.set_init_msg();
  initmsg.load_info_obj(ASN1_E2AP_ID_E2SETUP);

  e2setup_request_s& setup = initmsg.value.e2setup_request();

  setup->transaction_id.crit        = asn1::crit_opts::reject;
  setup->transaction_id.value.value = 1;

  setup->global_e2node_id.crit = asn1::crit_opts::reject;
  auto& gnb_id                 = setup->global_e2node_id.value.set_gnb();
  // gnb_id.ext = true;
  gnb_id.global_g_nb_id.plmn_id.from_number(3617847);
  gnb_id.global_g_nb_id.gnb_id.gnb_id().from_number(381210353);

  setup->ra_nfunctions_added.crit = asn1::crit_opts::reject;
  auto& list                      = setup->ra_nfunctions_added.value;

  setup->ra_nfunctions_added.id = ASN1_E2AP_ID_RA_NFUNCTIONS_ADDED;
  asn1::protocol_ie_single_container_s<ra_nfunction_item_ies_o> item;
  item.load_info_obj(ASN1_E2AP_ID_RA_NFUNCTION_ITEM);
  item.value().ra_nfunction_item().ran_function_id = 0;
  // TODO use E2SM to correctly generate this message
  item.value().ra_nfunction_item().ran_function_definition.from_string(
      "20C04F52414E2D4532534D2D4B504D0000054F494431323305004B504D206D6F6E69746F720860283861AAE33F0060000101070050657269"
      "6F646963207265706F727401051401011D004F2D4455204D6561737572656D656E7420436F6E7461696E657220666F722074686520354743"
      "20636F6E6E6563746564206465706C6F796D656E74010101010001021D004F2D4455204D6561737572656D656E7420436F6E7461696E6572"
      "20666F72207468652045504320636F6E6E6563746564206465706C6F796D656E74010101010001031E804F2D43552D4350204D6561737572"
      "656D656E7420436F6E7461696E657220666F72207468652035474320636F6E6E6563746564206465706C6F796D656E74010101010001041E"
      "804F2D43552D4350204D6561737572656D656E7420436F6E7461696E657220666F72207468652045504320636F6E6E656374656420646570"
      "6C6F796D656E74010101010001051E804F2D43552D5550204D6561737572656D656E7420436F6E7461696E657220666F7220746865203547"
      "4320636F6E6E6563746564206465706C6F796D656E74010101010001061E804F2D43552D5550204D6561737572656D656E7420436F6E7461"
      "696E657220666F72207468652045504320636F6E6E6563746564206465706C6F796D656E7401010101");
  item.value().ra_nfunction_item().ran_function_oid.resize(1);
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

e2_ap_pdu_c e2ap::generate_subscription_response()
{
  e2_ap_pdu_c           pdu;
  successful_outcome_s& success = pdu.set_successful_outcome();
  success.load_info_obj(ASN1_E2AP_ID_RICSUBSCRIPTION);
  ricsubscription_resp_s& sub_resp = success.value.ricsubscription_resp();

  sub_resp->ri_crequest_id.crit                   = asn1::crit_opts::reject;
  sub_resp->ri_crequest_id.id                     = ASN1_E2AP_ID_RI_CREQUEST_ID;
  sub_resp->ri_crequest_id.value.ric_requestor_id = 123;
  sub_resp->ri_crequest_id.value.ric_instance_id  = 21;

  sub_resp->ra_nfunction_id.crit      = asn1::crit_opts::reject;
  sub_resp->ra_nfunction_id.id        = ASN1_E2AP_ID_RA_NFUNCTION_ID;
  sub_resp->ri_cactions_admitted.crit = asn1::crit_opts::reject;
  auto& action_admit_list             = sub_resp->ri_cactions_admitted.value;
  action_admit_list.resize(1);
  action_admit_list[0].load_info_obj(ASN1_E2AP_ID_RI_CACTION_ADMITTED_ITEM);
  action_admit_list[0].value().ri_caction_admitted_item().ric_action_id = 1;
  return pdu;
}
int e2ap::process_setup_response(e2setup_resp_s setup_response)
{
  setup_response_received = true;
  // TODO process setup response
  return 0;
}

int e2ap::process_subscription_request(ricsubscription_request_s subscription_request)
{
  pending_subscription_request = true;
  // TODO process subscription request
  return 0;
}