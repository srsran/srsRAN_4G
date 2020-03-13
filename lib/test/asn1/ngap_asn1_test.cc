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

#include "srslte/asn1/ngap_nr_asn1.h"
#include "srslte/common/test_common.h"

using namespace asn1;
using namespace asn1::ngap_nr;

/* TESTS */

int test_amf_upd()
{
  uint8_t  ngap_msg[] = {0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x03, 0x00, 0x00, 0x11};
  cbit_ref bref(&ngap_msg[0], sizeof(ngap_msg));
  // 0000000A00000100010003000011

  ngap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  TESTASSERT(pdu.type().value == ngap_pdu_c::types_opts::init_msg);
  TESTASSERT(pdu.init_msg().proc_code == 0);
  TESTASSERT(pdu.init_msg().crit.value == crit_opts::reject);
  ngap_elem_procs_o::init_msg_c& init_choice = pdu.init_msg().value;
  TESTASSERT(init_choice.type().value == ngap_elem_procs_o::init_msg_c::types_opts::amf_cfg_upd);
  amf_cfg_upd_s& amf_upd = init_choice.amf_cfg_upd();
  TESTASSERT(not amf_upd.ext);
  auto& amf_name = amf_upd.protocol_ies.amf_name;
  TESTASSERT(amf_upd.protocol_ies.amf_name_present);
  TESTASSERT(amf_name.id == 1);
  TESTASSERT(amf_name.crit == crit_opts::reject);
  TESTASSERT(amf_name.value.size() == 1);
  TESTASSERT(amf_name.value[0] == 17);

  TESTASSERT(ceil(bref.distance_bytes()) == sizeof(ngap_msg));
  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);

  //  json_writer js;
  //  pdu.to_json(js);
  //  printf("PDU json: %s\n", js.to_string().c_str());

  return 0;
}

int test_ngsetup_request()
{
  uint8_t ngap_msg[] = {0x00, 0x15, 0x00, 0x30, 0x00, 0x00, 0x04, 0x00, 0x1b, 0x00, 0x09, 0x00, 0x00,
                        0xf1, 0x10, 0x50, 0x00, 0x00, 0x00, 0x01, 0x00, 0x52, 0x40, 0x06, 0x01, 0x80,
                        0x67, 0x6e, 0x62, 0x31, 0x00, 0x66, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x75,
                        0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x00, 0x08, 0x00, 0x15, 0x40, 0x01, 0x60};
  // 00150030000004001b00090000f1105000000001005240060180676e62310066000d00000000750000f110000000080015400160

  cbit_ref   bref(ngap_msg, sizeof(ngap_msg));
  ngap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  TESTASSERT(pdu.type().value == ngap_pdu_c::types_opts::init_msg);
  TESTASSERT(pdu.init_msg().proc_code == 21);
  TESTASSERT(pdu.init_msg().crit.value == crit_opts::reject);
  TESTASSERT(pdu.init_msg().value.type().value == ngap_elem_procs_o::init_msg_c::types_opts::ng_setup_request);
  ng_setup_request_s& ngsetup = pdu.init_msg().value.ng_setup_request();
  TESTASSERT(not ngsetup.ext);
  // Field 0
  TESTASSERT(ngsetup.protocol_ies.global_ran_node_id.id == 27);
  TESTASSERT(ngsetup.protocol_ies.global_ran_node_id.crit.value == crit_opts::reject);
  TESTASSERT(ngsetup.protocol_ies.global_ran_node_id.value.type().value ==
             global_ran_node_id_c::types_opts::global_gnb_id);
  global_gnb_id_s& global_gnb = ngsetup.protocol_ies.global_ran_node_id.value.global_gnb_id();
  TESTASSERT(global_gnb.plmn_id.to_number() == 0xF110);
  TESTASSERT(global_gnb.gnb_id.type().value == gnb_id_c::types_opts::gnb_id);
  TESTASSERT(global_gnb.gnb_id.gnb_id().to_number() == 1);
  // Field 1
  TESTASSERT(ngsetup.protocol_ies.ran_node_name_present);
  // Field 2
  TESTASSERT(ngsetup.protocol_ies.supported_ta_list.id == 102);
  TESTASSERT(ngsetup.protocol_ies.supported_ta_list.crit.value == crit_opts::reject);
  TESTASSERT(ngsetup.protocol_ies.supported_ta_list.value.size() == 1);
  TESTASSERT(ngsetup.protocol_ies.supported_ta_list.value[0].tac.to_number() == 0x75);
  TESTASSERT(ngsetup.protocol_ies.supported_ta_list.value[0].broadcast_plmn_list.size() == 1);
  auto& bcast_item = ngsetup.protocol_ies.supported_ta_list.value[0].broadcast_plmn_list[0];
  TESTASSERT(bcast_item.plmn_id.to_number() == 0xF110);
  TESTASSERT(bcast_item.tai_slice_support_list.size());
  TESTASSERT(bcast_item.tai_slice_support_list[0].s_nssai.sst.to_number() == 1);
  // Field 3
  TESTASSERT(ngsetup.protocol_ies.default_paging_drx.id == 21);
  TESTASSERT(ngsetup.protocol_ies.default_paging_drx.crit.value == crit_opts::ignore);
  TESTASSERT(ngsetup.protocol_ies.default_paging_drx.value.value == paging_drx_opts::v256);

  TESTASSERT(ceil(bref.distance(ngap_msg) / 8.0) == sizeof(ngap_msg));
  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);

  json_writer js;
  pdu.to_json(js);
  printf("PDU json: %s\n", js.to_string().c_str());

  return 0;
}

int test_ngsetup_response()
{
  uint8_t ngap_msg[] = {0x20, 0x15, 0x00, 0x5e, 0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x3a, 0x1b, 0x80, 0x61,
                        0x6d, 0x66, 0x31, 0x2e, 0x63, 0x6c, 0x75, 0x73, 0x74, 0x65, 0x72, 0x31, 0x2e, 0x6e,
                        0x65, 0x74, 0x32, 0x2e, 0x61, 0x6d, 0x66, 0x2e, 0x35, 0x67, 0x63, 0x2e, 0x6d, 0x6e,
                        0x63, 0x30, 0x30, 0x31, 0x2e, 0x6d, 0x63, 0x63, 0x30, 0x30, 0x31, 0x2e, 0x33, 0x67,
                        0x70, 0x70, 0x6e, 0x65, 0x74, 0x77, 0x6f, 0x72, 0x6b, 0x2e, 0x6f, 0x72, 0x67, 0x00,
                        0x60, 0x00, 0x08, 0x00, 0x00, 0x00, 0xf1, 0x10, 0x38, 0x08, 0x97, 0x00, 0x56, 0x40,
                        0x01, 0x05, 0x00, 0x50, 0x00, 0x08, 0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x00, 0x08};
  // 2015005e0000040001003a1b80616d66312e636c7573746572312e6e6574322e616d662e3567632e6d6e633030312e6d63633030312e336770706e6574776f726b2e6f726700600008000000f1103808970056400105005000080000f11000000008

  cbit_ref   bref(ngap_msg, sizeof(ngap_msg));
  ngap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  // Check Fields
  TESTASSERT(pdu.type().value == ngap_pdu_c::types_opts::successful_outcome);
  TESTASSERT(pdu.successful_outcome().proc_code == 21);
  TESTASSERT(pdu.successful_outcome().crit.value == crit_opts::reject);
  TESTASSERT(pdu.successful_outcome().value.type().value ==
             ngap_elem_procs_o::successful_outcome_c::types_opts::ng_setup_resp);
  ng_setup_resp_s& resp = pdu.successful_outcome().value.ng_setup_resp();
  // field 0
  TESTASSERT(resp.protocol_ies.amf_name.id == 1);
  TESTASSERT(resp.protocol_ies.amf_name.crit.value == crit_opts::reject);
  TESTASSERT(resp.protocol_ies.amf_name.value.size() == 56);
  TESTASSERT(resp.protocol_ies.amf_name.value.to_string() ==
             "amf1.cluster1.net2.amf.5gc.mnc001.mcc001.3gppnetwork.org");
  // field 1
  TESTASSERT(resp.protocol_ies.served_guami_list.id == 96);
  TESTASSERT(resp.protocol_ies.served_guami_list.crit.value == crit_opts::reject);
  TESTASSERT(resp.protocol_ies.served_guami_list.value.size() == 1);
  TESTASSERT(resp.protocol_ies.served_guami_list.value[0].guami.plmn_id.to_number() == 0xF110);
  TESTASSERT(resp.protocol_ies.served_guami_list.value[0].guami.amf_region_id.to_number() == 0b111000);
  TESTASSERT(resp.protocol_ies.served_guami_list.value[0].guami.amf_set_id.to_number() == 0b100010);
  TESTASSERT(resp.protocol_ies.served_guami_list.value[0].guami.amf_pointer.to_number() == 0b10111);
  // field 2
  // ...

  TESTASSERT(ceil(bref.distance(ngap_msg) / 8.0) == sizeof(ngap_msg));
  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);
  return 0;
}

int test_init_ue_msg()
{
  uint8_t ngap_msg[] = {
      0x00, 0x0f, 0x40, 0x80, 0xa2, 0x00, 0x00, 0x04, 0x00, 0x55, 0x00, 0x02, 0x00, 0x01, 0x00, 0x26, 0x00, 0x7d, 0x7c,
      0x7e, 0x00, 0x41, 0x71, 0x00, 0x76, 0x01, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x01, 0x01, 0x4d, 0x43, 0x6f, 0x77, 0x42,
      0x51, 0x59, 0x44, 0x4b, 0x32, 0x56, 0x75, 0x41, 0x79, 0x45, 0x41, 0x6e, 0x36, 0x36, 0x48, 0x39, 0x6b, 0x7a, 0x48,
      0x54, 0x61, 0x46, 0x5a, 0x4b, 0x30, 0x35, 0x37, 0x41, 0x49, 0x72, 0x37, 0x41, 0x2b, 0x6e, 0x6c, 0x73, 0x61, 0x49,
      0x58, 0x78, 0x52, 0x33, 0x4e, 0x69, 0x73, 0x36, 0x4c, 0x56, 0x6f, 0x75, 0x46, 0x69, 0x42, 0x34, 0x3d, 0xdf, 0xab,
      0xf5, 0xcd, 0x65, 0x2e, 0xb2, 0x54, 0x14, 0x91, 0x48, 0x4d, 0x41, 0x43, 0x2d, 0x53, 0x48, 0x41, 0x00, 0x85, 0x8b,
      0xbb, 0x1f, 0x42, 0xf1, 0x25, 0x6f, 0x9a, 0x37, 0x53, 0x1a, 0x77, 0x2a, 0x2c, 0xf2, 0xb7, 0x8f, 0xf1, 0x60, 0x48,
      0x84, 0x02, 0xed, 0x48, 0x93, 0x99, 0xb6, 0xb7, 0x37, 0x42, 0x00, 0x79, 0x00, 0x0f, 0x40, 0x00, 0xf1, 0x10, 0x00,
      0x00, 0x00, 0x00, 0x10, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x75, 0x00, 0x5a, 0x40, 0x01, 0x18};
  // 000f4080a20000040055000200010026007d7c7e00417100760100f110000001014d436f77425159444b325675417945416e363648396b7a485461465a4b30353741497237412b6e6c736149587852334e6973364c566f75466942343ddfabf5cd652eb2541491484d41432d53484100858bbb1f42f1256f9a37531a772a2cf2b78ff160488402ed489399b6b737420079000f4000f110000000001000f110000075005a400118

  cbit_ref   bref(ngap_msg, sizeof(ngap_msg));
  ngap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  // Check Fields
  TESTASSERT(pdu.type().value == ngap_pdu_c::types_opts::init_msg);
  TESTASSERT(pdu.init_msg().proc_code == 15);
  TESTASSERT(pdu.init_msg().crit.value == crit_opts::ignore);
  TESTASSERT(pdu.init_msg().value.type().value == ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
  auto& container = pdu.init_msg().value.init_ue_msg().protocol_ies;
  // Field 0
  TESTASSERT(container.ran_ue_ngap_id.id == 85);
  TESTASSERT(container.ran_ue_ngap_id.crit.value == crit_opts::reject);
  TESTASSERT(container.ran_ue_ngap_id.value == 1);
  // Field 1
  TESTASSERT(container.nas_pdu.value.size() == 124);
  // Field 2
  TESTASSERT(container.user_location_info.value.type().value ==
             user_location_info_c::types_opts::user_location_info_nr);
  TESTASSERT(container.user_location_info.value.user_location_info_nr().nr_cgi.plmn_id.to_number() == 0xF110);

  TESTASSERT(ceil(bref.distance(ngap_msg) / 8.0) == sizeof(ngap_msg));
  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);
  return 0;
}

int test_dl_nas_transport()
{
  uint8_t ngap_msg[] = {0x00, 0x04, 0x40, 0x3e, 0x00, 0x00, 0x03, 0x00, 0x0a, 0x00, 0x02, 0x00, 0x01, 0x00,
                        0x55, 0x00, 0x02, 0x00, 0x01, 0x00, 0x26, 0x00, 0x2b, 0x2a, 0x7e, 0x00, 0x56, 0x00,
                        0x02, 0x00, 0x00, 0x21, 0x68, 0x1c, 0xd4, 0x89, 0x65, 0x0f, 0xdc, 0xc7, 0xc7, 0x0e,
                        0xca, 0x8f, 0xa9, 0xbe, 0x44, 0x70, 0x20, 0x10, 0xc7, 0xf0, 0x79, 0x1f, 0xa8, 0x52,
                        0xe2, 0x82, 0x84, 0x7c, 0x9f, 0x4c, 0xe5, 0xc1, 0x94, 0x51};
  // 0004403e000003000a000200010055000200010026002b2a7e00560002000021681cd489650fdcc7c70eca8fa9be44702010c7f0791fa852e282847c9f4ce5c19451

  cbit_ref   bref(ngap_msg, sizeof(ngap_msg));
  ngap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  // Check Fields
  TESTASSERT(pdu.type().value == ngap_pdu_c::types_opts::init_msg);
  TESTASSERT(pdu.init_msg().proc_code == 4);
  TESTASSERT(pdu.init_msg().crit.value == crit_opts::ignore);
  TESTASSERT(pdu.init_msg().value.type().value == ngap_elem_procs_o::init_msg_c::types_opts::dl_nas_transport);
  auto& dl_nas = pdu.init_msg().value.dl_nas_transport();
  // Field 0
  // ...
  // Field 1
  TESTASSERT(dl_nas.protocol_ies.nas_pdu.value.size() == 42);

  TESTASSERT(ceil(bref.distance(ngap_msg) / 8.0) == sizeof(ngap_msg));
  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);
  return 0;
}

int test_ul_ran_status_transfer()
{
  uint8_t ngap_msg[] = {0x00, 0x2e, 0x40, 0x3c, 0x00, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x55, 0x00,
                        0x02, 0x00, 0x01, 0x00, 0x26, 0x00, 0x16, 0x15, 0x7e, 0x00, 0x57, 0x2d, 0x10, 0x5e, 0x86, 0x21,
                        0x9e, 0x7d, 0xda, 0x99, 0x95, 0xe3, 0x85, 0x03, 0x84, 0xcf, 0xbe, 0xa5, 0x3b, 0x00, 0x79, 0x40,
                        0x0f, 0x40, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x75};
  // 002e403c000004000a0002000100550002000100260016157e00572d105e86219e7dda9995e3850384cfbea53b0079400f4000f110000000001000f110000075

  cbit_ref   bref(ngap_msg, sizeof(ngap_msg));
  ngap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  // Check Fields
  TESTASSERT(pdu.type().value == ngap_pdu_c::types_opts::init_msg);
  TESTASSERT(pdu.init_msg().proc_code == 46);
  TESTASSERT(pdu.init_msg().crit.value == crit_opts::ignore);
  TESTASSERT(pdu.init_msg().value.type().value == ngap_elem_procs_o::init_msg_c::types_opts::ul_nas_transport);
  auto& ul_nas = pdu.init_msg().value.ul_nas_transport();
  // Field 1
  TESTASSERT(ul_nas.protocol_ies.nas_pdu.value.size() == 21);

  TESTASSERT(ceil(bref.distance(ngap_msg) / 8.0) == sizeof(ngap_msg));
  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);
  return 0;
}

int test_ue_context_release()
{
  uint8_t ngap_msg[] = {0x00, 0x29, 0x00, 0x10, 0x00, 0x00, 0x02, 0x00, 0x72, 0x00,
                        0x04, 0x00, 0x01, 0x00, 0x01, 0x00, 0x0f, 0x40, 0x01, 0x48};
  // 002900100000020072000400010001000f400148

  cbit_ref   bref(ngap_msg, sizeof(ngap_msg));
  ngap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  // Check Fields
  TESTASSERT(pdu.type().value == ngap_pdu_c::types_opts::init_msg);
  TESTASSERT(pdu.init_msg().proc_code == 41);
  TESTASSERT(pdu.init_msg().crit.value == crit_opts::reject);
  TESTASSERT(pdu.init_msg().value.type().value == ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd);

  TESTASSERT(ceil(bref.distance(ngap_msg) / 8.0) == sizeof(ngap_msg));
  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);
  return 0;
}

int test_ue_context_release_complete()
{
  uint8_t ngap_msg[] = {
      0x20, 0x29, 0x00, 0x0f, 0x00, 0x00, 0x02, 0x00, 0x0a, 0x40, 0x02, 0x00, 0x01, 0x00, 0x55, 0x40, 0x02, 0x00, 0x01};
  // 2029000f000002000a40020001005540020001

  cbit_ref   bref(ngap_msg, sizeof(ngap_msg));
  ngap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  // Check Fields
  TESTASSERT(pdu.type().value == ngap_pdu_c::types_opts::successful_outcome);
  TESTASSERT(pdu.successful_outcome().proc_code == 41);
  TESTASSERT(pdu.successful_outcome().crit.value == crit_opts::reject);
  TESTASSERT(pdu.successful_outcome().value.type().value ==
             ngap_elem_procs_o::successful_outcome_c::types_opts::ue_context_release_complete);

  TESTASSERT(ceil(bref.distance(ngap_msg) / 8.0) == sizeof(ngap_msg));
  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);
  return 0;
}

int test_session_res_setup_request()
{
  uint8_t ngap_msg[] = {0x00, 0x1d, 0x00, 0x6c, 0x00, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x55, 0x00,
                        0x02, 0x00, 0x01, 0x00, 0x26, 0x00, 0x2e, 0x2d, 0x7e, 0x00, 0x68, 0x01, 0x00, 0x25, 0x2e, 0x01,
                        0x00, 0xc2, 0x11, 0x00, 0x06, 0x01, 0x00, 0x03, 0x30, 0x01, 0x01, 0x06, 0x06, 0x03, 0xe8, 0x06,
                        0x03, 0xe8, 0x29, 0x05, 0x01, 0xc0, 0xa8, 0x0c, 0x7b, 0x25, 0x08, 0x07, 0x64, 0x65, 0x66, 0x61,
                        0x75, 0x6c, 0x74, 0x12, 0x01, 0x00, 0x4a, 0x00, 0x27, 0x00, 0x00, 0x01, 0x00, 0x00, 0x21, 0x00,
                        0x00, 0x03, 0x00, 0x8b, 0x00, 0x0a, 0x01, 0xf0, 0xc0, 0xa8, 0x11, 0xd2, 0x00, 0x00, 0x00, 0x01,
                        0x00, 0x86, 0x00, 0x01, 0x10, 0x00, 0x88, 0x00, 0x07, 0x00, 0x01, 0x00, 0x00, 0x09, 0x00, 0x00};
  // 001d006c000004000a000200010055000200010026002e2d7e00680100252e0100c2110006010003300101060603e80603e8290501c0a80c7b25080764656661756c741201004a0027000001000021000003008b000a01f0c0a811d20000000100860001100088000700010000090000

  cbit_ref   bref(ngap_msg, sizeof(ngap_msg));
  ngap_pdu_c pdu;
  TESTASSERT(pdu.unpack(bref) == SRSASN_SUCCESS);

  // Check Fields
  TESTASSERT(pdu.type().value == ngap_pdu_c::types_opts::init_msg);
  TESTASSERT(pdu.init_msg().proc_code == 29);
  TESTASSERT(pdu.init_msg().crit.value == crit_opts::reject);
  TESTASSERT(pdu.init_msg().value.type().value ==
             ngap_elem_procs_o::init_msg_c::types_opts::pdu_session_res_setup_request);
  auto& container = pdu.init_msg().value.pdu_session_res_setup_request().protocol_ies;
  TESTASSERT(container.pdu_session_res_setup_list_su_req.id == ASN1_NGAP_NR_ID_PDU_SESSION_RES_SETUP_LIST_SU_REQ);
  TESTASSERT(container.pdu_session_res_setup_list_su_req.value.size() == 1);
  auto& item = container.pdu_session_res_setup_list_su_req.value[0];
  TESTASSERT(item.pdu_session_id == 1);
  TESTASSERT(item.s_nssai.sst.to_number() == 0);
  TESTASSERT(item.pdu_session_res_setup_request_transfer.to_string() ==
             "000003008b000a01f0c0a811d20000000100860001100088000700010000090000");

  cbit_ref                                 bref2(item.pdu_session_res_setup_request_transfer.data(),
                 item.pdu_session_res_setup_request_transfer.size());
  pdu_session_res_setup_request_transfer_s req;
  TESTASSERT(req.unpack(bref2) == SRSASN_SUCCESS);
  TESTASSERT(req.protocol_ies.ul_ngu_up_tnl_info.id == 139);
  TESTASSERT(req.protocol_ies.ul_ngu_up_tnl_info.value.type().value ==
             up_transport_layer_info_c::types_opts::gtp_tunnel);
  TESTASSERT(req.protocol_ies.ul_ngu_up_tnl_info.value.gtp_tunnel().transport_layer_address.to_string() ==
             "11000000101010000001000111010010");

  TESTASSERT(bref2.distance_bytes() == (int)item.pdu_session_res_setup_request_transfer.size());
  TESTASSERT(test_pack_unpack_consistency(req) == SRSASN_SUCCESS);

  TESTASSERT(bref.distance_bytes() == sizeof(ngap_msg));
  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);
  return 0;
}

int main()
{
  srslte::logmap::set_default_log_level(srslte::LOG_LEVEL_DEBUG);

  TESTASSERT(test_amf_upd() == 0);
  TESTASSERT(test_ngsetup_request() == 0);
  TESTASSERT(test_ngsetup_response() == 0);
  TESTASSERT(test_init_ue_msg() == 0);
  TESTASSERT(test_dl_nas_transport() == 0);
  TESTASSERT(test_ul_ran_status_transfer() == 0);
  TESTASSERT(test_ue_context_release() == 0);
  TESTASSERT(test_ue_context_release_complete() == 0);
  TESTASSERT(test_session_res_setup_request() == 0);

  printf("Success\n");
  return 0;
}
