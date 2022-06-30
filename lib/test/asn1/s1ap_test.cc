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

#include "srsran/asn1/s1ap.h"
#include "srsran/common/test_common.h"
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace asn1;
using namespace asn1::s1ap;

/* TESTS */

int test_s1setup_request()
{
  uint8_t  ngap_msg[] = {0x00, 0x11, 0x00, 0x2d, 0x00, 0x00, 0x04, 0x00, 0x3b, 0x00, 0x08, 0x00, 0x09,
                        0xf1, 0x07, 0x00, 0x00, 0x19, 0xb0, 0x00, 0x3c, 0x40, 0x0a, 0x03, 0x80, 0x65,
                        0x6e, 0x62, 0x30, 0x30, 0x31, 0x39, 0x62, 0x00, 0x40, 0x00, 0x07, 0x00, 0x00,
                        0x01, 0xc0, 0x09, 0xf1, 0x07, 0x00, 0x89, 0x40, 0x01, 0x40};
  cbit_ref bref(&ngap_msg[0], sizeof(ngap_msg));
  // 0011002D000004003B00080009F107000019B0003C400A0380656E62303031396200400007000001C009F1070089400140

  s1ap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  TESTASSERT(pdu.type().value == s1ap_pdu_c::types_opts::init_msg);
  TESTASSERT(pdu.init_msg().proc_code == 17);
  TESTASSERT(pdu.init_msg().crit.value == crit_opts::reject);
  s1ap_elem_procs_o::init_msg_c& init_choice = pdu.init_msg().value;
  TESTASSERT(init_choice.type().value == s1ap_elem_procs_o::init_msg_c::types_opts::s1_setup_request);
  s1_setup_request_s& s1req = init_choice.s1_setup_request();
  TESTASSERT(not s1req.ext);
  TESTASSERT(s1req->global_enb_id.id == ASN1_S1AP_ID_GLOBAL_ENB_ID);
  TESTASSERT(s1req->global_enb_id.crit.value == crit_opts::reject);
  TESTASSERT(s1req->global_enb_id.value.enb_id.type().value == enb_id_c::types_opts::macro_enb_id);
  TESTASSERT(s1req->global_enb_id.value.enb_id.macro_enb_id().to_number() == 0x0019B);
  //
  //  //  json_writer js;
  //  //  pdu.to_json(js);
  //  //  printf("PDU json: %s\n", js.to_string().c_str());

  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);

  return 0;
}

int test_init_ctxt_setup_req()
{
  uint8_t s1ap_msg[] = {
      0x00, 0x09, 0x00, 0x80, 0xc6, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x64, 0x00, 0x08, 0x00, 0x02, 0x00,
      0x01, 0x00, 0x42, 0x00, 0x0a, 0x18, 0x3b, 0x9a, 0xca, 0x00, 0x60, 0x3b, 0x9a, 0xca, 0x00, 0x00, 0x18, 0x00, 0x78,
      0x00, 0x00, 0x34, 0x00, 0x73, 0x45, 0x00, 0x09, 0x3c, 0x0f, 0x80, 0x0a, 0x00, 0x21, 0xf0, 0xb7, 0x36, 0x1c, 0x56,
      0x64, 0x27, 0x3e, 0x5b, 0x04, 0xb7, 0x02, 0x07, 0x42, 0x02, 0x3e, 0x06, 0x00, 0x09, 0xf1, 0x07, 0x00, 0x07, 0x00,
      0x37, 0x52, 0x66, 0xc1, 0x01, 0x09, 0x1b, 0x07, 0x74, 0x65, 0x73, 0x74, 0x31, 0x32, 0x33, 0x06, 0x6d, 0x6e, 0x63,
      0x30, 0x37, 0x30, 0x06, 0x6d, 0x63, 0x63, 0x39, 0x30, 0x31, 0x04, 0x67, 0x70, 0x72, 0x73, 0x05, 0x01, 0xc0, 0xa8,
      0x03, 0x02, 0x27, 0x0e, 0x80, 0x80, 0x21, 0x0a, 0x03, 0x00, 0x00, 0x0a, 0x81, 0x06, 0x08, 0x08, 0x08, 0x08, 0x50,
      0x0b, 0xf6, 0x09, 0xf1, 0x07, 0x80, 0x01, 0x01, 0xf6, 0x7e, 0x72, 0x69, 0x13, 0x09, 0xf1, 0x07, 0x00, 0x01, 0x23,
      0x05, 0xf4, 0xf6, 0x7e, 0x72, 0x69, 0x00, 0x6b, 0x00, 0x05, 0x18, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x49, 0x00, 0x20,
      0x45, 0x25, 0xe4, 0x9a, 0x77, 0xc8, 0xd5, 0xcf, 0x26, 0x33, 0x63, 0xeb, 0x5b, 0xb9, 0xc3, 0x43, 0x9b, 0x9e, 0xb3,
      0x86, 0x1f, 0xa8, 0xa7, 0xcf, 0x43, 0x54, 0x07, 0xae, 0x42, 0x2b, 0x63, 0xb9};
  // 00090080c60000060000000200640008000200010042000a183b9aca00603b9aca000018007800003400734500093c0f800a0021f0b7361c5664273e5b04b7020742023e060009f107000700375266c101091b0774657374313233066d6e63303730066d636339303104677072730501c0a80302270e8080210a0300000a810608080808500bf609f107800101f67e72691309f10700012305f4f67e7269006b000518000c0000004900204525e49a77c8d5cf263363eb5bb9c3439b9eb3861fa8a7cf435407ae422b63b9

  cbit_ref   bref(&s1ap_msg[0], sizeof(s1ap_msg));
  s1ap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  TESTASSERT(pdu.type().value == s1ap_pdu_c::types_opts::init_msg);
  TESTASSERT(pdu.init_msg().proc_code == 9);
  TESTASSERT(pdu.init_msg().crit.value == crit_opts::reject);
  s1ap_elem_procs_o::init_msg_c& init_choice = pdu.init_msg().value;
  auto&                          ctxt_setup  = init_choice.init_context_setup_request();
  TESTASSERT(ctxt_setup->ue_security_cap.id == 107);
  TESTASSERT(ctxt_setup->ue_security_cap.value.encryption_algorithms.to_string() == "1100000000000000");
  TESTASSERT(ctxt_setup->ue_security_cap.value.integrity_protection_algorithms.to_string() == "1100000000000000");

  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);

  return SRSRAN_SUCCESS;
}

int test_ue_ctxt_release_req()
{
  uint8_t s1ap_msg[] = {0x00, 0x12, 0x40, 0x15, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01,
                        0x00, 0x08, 0x00, 0x02, 0x00, 0x01, 0x00, 0x02, 0x40, 0x02, 0x02, 0x80};
  // 00124015000003000000020001000800020001000240020280

  cbit_ref   bref(&s1ap_msg[0], sizeof(s1ap_msg));
  s1ap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  TESTASSERT(pdu.type().value == s1ap_pdu_c::types_opts::init_msg);
  TESTASSERT(pdu.init_msg().proc_code == ASN1_S1AP_ID_UE_CONTEXT_RELEASE_REQUEST);
  auto& req = pdu.init_msg().value.ue_context_release_request();
  TESTASSERT(req->mme_ue_s1ap_id.value.value == 1);
  TESTASSERT(req->enb_ue_s1ap_id.value.value == 1);
  TESTASSERT(req->cause.value.type().value == cause_c::types_opts::radio_network);
  TESTASSERT(req->cause.value.radio_network().value == cause_radio_network_opts::user_inactivity);

  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);

  return SRSRAN_SUCCESS;
}

template <typename T, typename U>
bool is_same_type(U& u)
{
  return std::is_same<T, U>::value;
}

int test_proc_id_consistency(srsran::log_sink_spy& spy)
{
  s1ap_pdu_c pdu;

  /* Check IDs */
  TESTASSERT(ASN1_S1AP_ID_HO_PREP == 0);
  TESTASSERT(ASN1_S1AP_ID_HO_RES_ALLOC == 1);
  TESTASSERT(ASN1_S1AP_ID_ERAB_SETUP == 5);
  TESTASSERT(ASN1_S1AP_ID_INIT_CONTEXT_SETUP == 9);

  /* Unsuccessful case */
  unsuccessful_outcome_s& unsuc = pdu.set_unsuccessful_outcome();
  // HandoverPreparation
  TESTASSERT(unsuc.load_info_obj(ASN1_S1AP_ID_HO_PREP));
  TESTASSERT(unsuc.proc_code == ASN1_S1AP_ID_HO_PREP);
  TESTASSERT(unsuc.crit.value == crit_opts::reject);
  TESTASSERT(unsuc.value.type().value == s1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ho_prep_fail);
  TESTASSERT(is_same_type<ho_prep_fail_s>(unsuc.value.ho_prep_fail()));
  // HandoverResourceAllocation
  TESTASSERT(unsuc.load_info_obj(ASN1_S1AP_ID_HO_RES_ALLOC));
  TESTASSERT(unsuc.proc_code == ASN1_S1AP_ID_HO_RES_ALLOC);
  TESTASSERT(unsuc.crit.value == crit_opts::reject);
  TESTASSERT(unsuc.value.type().value == s1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ho_fail);
  TESTASSERT(is_same_type<ho_fail_s>(unsuc.value.ho_fail()));
  // e-RABSetup (No Unsuccessful Outcome)
  spy.reset_counters();
  TESTASSERT(not unsuc.load_info_obj(ASN1_S1AP_ID_ERAB_SETUP));
  TESTASSERT(spy.get_error_counter() == 1);
  // initialContextSetup
  TESTASSERT(unsuc.load_info_obj(ASN1_S1AP_ID_INIT_CONTEXT_SETUP));
  TESTASSERT(unsuc.proc_code == ASN1_S1AP_ID_INIT_CONTEXT_SETUP);
  TESTASSERT(unsuc.crit.value == crit_opts::reject);
  TESTASSERT(is_same_type<init_context_setup_fail_s>(unsuc.value.init_context_setup_fail()));

  return SRSRAN_SUCCESS;
}

int test_ho_request()
{
  uint8_t s1ap_msg[] = {
      0x00, 0x01, 0x00, 0x80, 0xe6, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x02, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00,
      0x00, 0x02, 0x40, 0x02, 0x00, 0x00, 0x00, 0x42, 0x00, 0x0a, 0x18, 0x3b, 0x9a, 0xca, 0x00, 0x60, 0x3b, 0x9a, 0xca,
      0x00, 0x00, 0x35, 0x00, 0x19, 0x00, 0x00, 0x1b, 0x00, 0x14, 0x4a, 0x1f, 0x0a, 0x00, 0x21, 0xf0, 0xb7, 0x36, 0x1c,
      0x56, 0x00, 0x09, 0x3c, 0x00, 0x00, 0x00, 0x8f, 0x40, 0x01, 0x00, 0x00, 0x68, 0x00, 0x75, 0x74, 0x00, 0x5f, 0x0a,
      0x10, 0x0c, 0x81, 0xa0, 0x00, 0x00, 0x18, 0x00, 0x02, 0xe8, 0x7f, 0xe4, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x05,
      0x91, 0x00, 0x00, 0x02, 0x90, 0x09, 0x78, 0x00, 0x00, 0x00, 0x62, 0x7c, 0x1f, 0x50, 0x29, 0x8f, 0x00, 0xe9, 0xce,
      0x02, 0x13, 0x00, 0x00, 0x95, 0x01, 0x00, 0x46, 0x40, 0x00, 0x00, 0x01, 0x90, 0x13, 0x84, 0x00, 0x1c, 0x00, 0x67,
      0x00, 0xa0, 0x51, 0x80, 0x41, 0x40, 0x06, 0x70, 0xdf, 0xbc, 0x44, 0x00, 0x6b, 0x01, 0x40, 0x00, 0x80, 0x02, 0x08,
      0x00, 0xc1, 0x4c, 0xa2, 0xd5, 0x4e, 0x28, 0x03, 0x51, 0x72, 0x40, 0xe0, 0x59, 0x14, 0x01, 0x21, 0x7b, 0x00, 0x00,
      0x09, 0xf1, 0x07, 0x00, 0x19, 0xb0, 0x10, 0x00, 0x09, 0xf1, 0x07, 0x00, 0x19, 0xc0, 0x21, 0x00, 0x00, 0x1f, 0x00,
      0x6b, 0x00, 0x05, 0x18, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x28, 0x00, 0x21, 0x10, 0x8b, 0x0d, 0xab, 0xd7, 0xe5, 0x98,
      0x34, 0xb3, 0xef, 0x6c, 0xc1, 0xaa, 0xa7, 0x27, 0xfb, 0xf4, 0x53, 0x08, 0xff, 0x74, 0x94, 0x7c, 0xa7, 0x1b, 0xd9,
      0xb4, 0x37, 0xb9, 0x02, 0x78, 0x62, 0x12};
  // 00010080E600000800000002006400010001000002400200000042000A183B9ACA00603B9ACA000035001900001B00144A1F0A0021F0B7361C5600093C0000008F4001000068007574005F0A100C81A00000180002E87FE40000150000000591000002900978000000627C1F50298F00E9CE021300009501004640000001901384001C006700A0518041400670DFBC44006B01400080020800C14CA2D54E2803517240E0591401217B000009F1070019B0100009F1070019C02100001F006B000518000C000000280021108B0DABD7E59834B3EF6CC1AAA727FBF45308FF74947CA71BD9B437B902786212

  cbit_ref   bref(&s1ap_msg[0], sizeof(s1ap_msg));
  s1ap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  TESTASSERT(pdu.type().value == s1ap_pdu_c::types_opts::init_msg);
  TESTASSERT(pdu.init_msg().proc_code == ASN1_S1AP_ID_HO_RES_ALLOC);
  TESTASSERT(pdu.init_msg().crit.value == crit_opts::reject);
  auto& horeq = pdu.init_msg().value.ho_request();

  auto& erab_item = horeq->erab_to_be_setup_list_ho_req.value[0]->erab_to_be_setup_item_ho_req();
  TESTASSERT(erab_item.erab_id == 5);
  TESTASSERT(erab_item.gtp_teid.to_string() == "b7361c56");

  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);

  return SRSRAN_SUCCESS;
}

int test_enb_status_transfer()
{
  s1ap_pdu_c pdu;

  TESTASSERT(pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_ENB_STATUS_TRANSFER));
  auto& enb_status_transfer                       = pdu.init_msg().value.enb_status_transfer();
  enb_status_transfer->mme_ue_s1ap_id.value.value = 1;
  enb_status_transfer->enb_ue_s1ap_id.value.value = 1;
  enb_status_transfer->enb_status_transfer_transparent_container.value.bearers_subject_to_status_transfer_list.resize(
      1);
  auto& bearer =
      enb_status_transfer->enb_status_transfer_transparent_container.value.bearers_subject_to_status_transfer_list[0];

  TESTASSERT(bearer.load_info_obj(ASN1_S1AP_ID_BEARERS_SUBJECT_TO_STATUS_TRANSFER_ITEM));
  auto& bearer_item = bearer->bearers_subject_to_status_transfer_item();

  bearer_item.erab_id                = 5;
  bearer_item.dl_coun_tvalue.pdcp_sn = 5;
  bearer_item.dl_coun_tvalue.hfn     = 0;
  bearer_item.ul_coun_tvalue.pdcp_sn = 4;
  bearer_item.ul_coun_tvalue.hfn     = 0;

  uint8_t       buffer[2048];
  asn1::bit_ref bref{buffer, sizeof(buffer)};
  TESTASSERT(pdu.pack(bref) == SRSASN_SUCCESS);

  srslog::fetch_basic_logger("ASN1").info(
      buffer, bref.distance_bytes(), "eNB Status Transfer (%d bytes)", (int)bref.distance_bytes());

  asn1::cbit_ref bref2{buffer, sizeof(buffer)};
  s1ap_pdu_c     pdu2;
  TESTASSERT(pdu2.unpack(bref2) == SRSASN_SUCCESS);

  auto& bearer2 = pdu2.init_msg()
                      .value.enb_status_transfer()
                      ->enb_status_transfer_transparent_container.value.bearers_subject_to_status_transfer_list[0];
  auto& bearer_item2 = bearer2->bearers_subject_to_status_transfer_item();
  TESTASSERT(bearer_item2.dl_coun_tvalue.hfn == bearer_item.dl_coun_tvalue.hfn);
  TESTASSERT(bearer_item2.dl_coun_tvalue.hfn == 0);

  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);

  return SRSRAN_SUCCESS;
}

int unpack_test_served_gummeis_with_multiple_plmns()
{
  uint8_t pdu[] = {0x20, 0x11, 0x00, 0x26, 0x00, 0x00, 0x02, 0x00, 0x69, 0x00, 0x1a, 0x01, 0x40, 0x00,
                   0xf1, 0x10, 0x00, 0xf1, 0x10, 0x00, 0xf1, 0x10, 0x00, 0xf1, 0x10, 0x00, 0xf1, 0x10,
                   0x00, 0xf1, 0x10, 0x00, 0x00, 0x88, 0x88, 0x00, 0x7b, 0x00, 0x57, 0x40, 0x01, 0xff};

  asn1::cbit_ref         bref(pdu, sizeof(pdu));
  asn1::s1ap::s1ap_pdu_c input_pdu;

  TESTASSERT(input_pdu.unpack(bref) == SRSASN_SUCCESS);
  TESTASSERT(asn1::test_pack_unpack_consistency(input_pdu) == SRSASN_SUCCESS);

  return SRSRAN_SUCCESS;
}

int test_load_info_obj()
{
  asn1::s1ap::init_context_setup_resp_ies_container container;
  container.erab_failed_to_setup_list_ctxt_su_res.value.resize(1);
  container.erab_failed_to_setup_list_ctxt_su_res.value[0].load_info_obj(ASN1_S1AP_ID_ERAB_ITEM);

  TESTASSERT_EQ(ASN1_S1AP_ID_ERAB_ITEM, container.erab_failed_to_setup_list_ctxt_su_res.value[0].id());

  return SRSRAN_SUCCESS;
}

int test_initial_ctxt_setup_response()
{
  asn1::s1ap::s1ap_pdu_c tx_pdu;

  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_INIT_CONTEXT_SETUP);

  // Fill in the MME and eNB IDs
  auto& container                 = tx_pdu.successful_outcome().value.init_context_setup_resp();
  container->mme_ue_s1ap_id.value = 1;
  container->enb_ue_s1ap_id.value = 1;

  container->erab_setup_list_ctxt_su_res.value.resize(1);
  // Fill in the GTP bind address for all bearers
  for (uint32_t i = 0; i < container->erab_setup_list_ctxt_su_res.value.size(); ++i) {
    container->erab_setup_list_ctxt_su_res.value[i].load_info_obj(ASN1_S1AP_ID_ERAB_SETUP_ITEM_CTXT_SU_RES);
    auto& item   = container->erab_setup_list_ctxt_su_res.value[i]->erab_setup_item_ctxt_su_res();
    item.erab_id = 1;
    // uint32_to_uint8(teid_in, item.gtp_teid.data());
    item.transport_layer_address.resize(32);
    uint8_t addr[4];
    inet_pton(AF_INET, "127.0.0.1", addr);
    memcpy(item.transport_layer_address.data(), addr, sizeof(addr));
    //    liblte_unpack(addr, 4, item.transport_layer_address.data());
  }

  uint8_t       buffer[1024];
  asn1::bit_ref bref(buffer, sizeof(buffer));
  TESTASSERT(tx_pdu.pack(bref) == SRSRAN_SUCCESS);

  srslog::fetch_basic_logger("TEST").info(
      buffer, bref.distance_bytes(), "message (nof bytes = %d):", bref.distance_bytes());

  return SRSRAN_SUCCESS;
}

int test_eci_pack()
{
  uint8_t buffer[128];

  uint32_t                         target_cellid = 0x19C02;
  fixed_bitstring<28, false, true> cell_id;
  cell_id.from_number(target_cellid);
  TESTASSERT(cell_id.to_number() == target_cellid);
  asn1::bit_ref bref(buffer, sizeof(buffer));
  cell_id.pack(bref);
  TESTASSERT(buffer[0] == 0);
  TESTASSERT(buffer[1] == 0x19);
  TESTASSERT(buffer[2] == 0xC0);
  TESTASSERT(buffer[3] == 0x20);

  uint32_t eci = 0x19b01;
  cell_id.from_number(eci);
  TESTASSERT(cell_id.to_number() == eci);
  TESTASSERT(cell_id.to_string() == "0000000000011001101100000001");

  bref = asn1::bit_ref(buffer, sizeof(buffer));
  cell_id.pack(bref);
  TESTASSERT(buffer[0] == 0);
  TESTASSERT(buffer[1] == 0x19);
  TESTASSERT(buffer[2] == 0xB0);
  TESTASSERT(buffer[3] == 0x10);

  uint32_t                         macroenbid = 0x19C;
  fixed_bitstring<20, false, true> macro_id;
  macro_id.from_number(macroenbid);
  TESTASSERT(macro_id.to_number() == macroenbid);
  bref = asn1::bit_ref(buffer, sizeof(buffer));
  macro_id.pack(bref);
  TESTASSERT(buffer[0] == 0);
  TESTASSERT(buffer[1] == 0x19);
  TESTASSERT(buffer[2] == 0xC0);

  srslog::fetch_basic_logger("TEST").info(buffer, bref.distance_bytes(), "Packed cell id:");

  return SRSRAN_SUCCESS;
}

int test_paging()
{
  uint8_t buffer[] = {0x00, 0x0a, 0x40, 0x2a, 0x00, 0x00, 0x04, 0x00, 0x50, 0x40, 0x02, 0xb4, 0xc0, 0x00, 0x2b, 0x40,
                      0x09, 0x68, 0x54, 0x02, 0x04, 0x30, 0x68, 0x74, 0x05, 0xf7, 0x00, 0x6d, 0x40, 0x01, 0x00, 0x00,
                      0x2e, 0x40, 0x0b, 0x00, 0x00, 0x2f, 0x40, 0x06, 0x00, 0x54, 0xf2, 0x40, 0x04, 0xd2};
  // 000A402A00000400504002B4C0002B40096854020430687405F7006D400100002E400B00002F40060054F24004D2

  asn1::cbit_ref         bref(buffer, sizeof(buffer));
  asn1::s1ap::s1ap_pdu_c input_pdu;

  TESTASSERT(input_pdu.unpack(bref) == SRSASN_SUCCESS);
  TESTASSERT(asn1::test_pack_unpack_consistency(input_pdu) == SRSASN_SUCCESS);

  return SRSRAN_SUCCESS;
}

int main()
{
  // Setup the log spy to intercept error and warning log entries.
  if (!srslog::install_custom_sink(
          srsran::log_sink_spy::name(),
          std::unique_ptr<srsran::log_sink_spy>(new srsran::log_sink_spy(srslog::get_default_log_formatter())))) {
    return SRSRAN_ERROR;
  }

  auto* spy = static_cast<srsran::log_sink_spy*>(srslog::find_sink(srsran::log_sink_spy::name()));
  if (!spy) {
    return SRSRAN_ERROR;
  }

  auto& asn1_logger = srslog::fetch_basic_logger("ASN1", *spy, false);
  asn1_logger.set_level(srslog::basic_levels::debug);
  asn1_logger.set_hex_dump_max_size(-1);
  auto& test_logger = srslog::fetch_basic_logger("TEST", false);
  test_logger.set_level(srslog::basic_levels::debug);
  test_logger.set_hex_dump_max_size(-1);

  // Start the log backend.
  srslog::init();

  TESTASSERT(test_s1setup_request() == 0);
  TESTASSERT(test_init_ctxt_setup_req() == 0);
  TESTASSERT(test_ue_ctxt_release_req() == 0);
  TESTASSERT(test_proc_id_consistency(*spy) == 0);
  TESTASSERT(test_ho_request() == 0);
  TESTASSERT(test_enb_status_transfer() == 0);
  TESTASSERT(unpack_test_served_gummeis_with_multiple_plmns() == 0);
  TESTASSERT(test_load_info_obj() == 0);
  TESTASSERT(test_initial_ctxt_setup_response() == 0);
  TESTASSERT(test_eci_pack() == 0);
  TESTASSERT(test_paging() == 0);

  srslog::flush();

  printf("Success\n");
  return 0;
}
