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

#include "srslte/asn1/s1ap_asn1.h"
#include "srslte/common/test_common.h"

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
  TESTASSERT(s1req.protocol_ies.global_enb_id.id == ASN1_S1AP_ID_GLOBAL_ENB_ID);
  TESTASSERT(s1req.protocol_ies.global_enb_id.crit.value == crit_opts::reject);
  TESTASSERT(s1req.protocol_ies.global_enb_id.value.enb_id.type().value == enb_id_c::types_opts::macro_enb_id);
  TESTASSERT(s1req.protocol_ies.global_enb_id.value.enb_id.macro_enb_id().to_number() == 0x0019B);
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
  auto&                          ctxt_setup  = init_choice.init_context_setup_request().protocol_ies;
  TESTASSERT(ctxt_setup.ue_security_cap.id == 107);
  TESTASSERT(ctxt_setup.ue_security_cap.value.encryption_algorithms.to_string() == "1100000000000000");
  TESTASSERT(ctxt_setup.ue_security_cap.value.integrity_protection_algorithms.to_string() == "1100000000000000");

  TESTASSERT(test_pack_unpack_consistency(pdu) == SRSASN_SUCCESS);

  return SRSLTE_SUCCESS;
}

template <typename T, typename U>
bool is_same_type(U& u)
{
  return std::is_same<T, U>::value;
}

int test_proc_id_consistency()
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
  {
    srslte::scoped_log<srslte::nullsink_log> sink("ASN1");
    TESTASSERT(not unsuc.load_info_obj(ASN1_S1AP_ID_ERAB_SETUP));
    TESTASSERT(sink->error_counter == 1);
  }
  // initialContextSetup
  TESTASSERT(unsuc.load_info_obj(ASN1_S1AP_ID_INIT_CONTEXT_SETUP));
  TESTASSERT(unsuc.proc_code == ASN1_S1AP_ID_INIT_CONTEXT_SETUP);
  TESTASSERT(unsuc.crit.value == crit_opts::reject);
  TESTASSERT(is_same_type<init_context_setup_fail_s>(unsuc.value.init_context_setup_fail()));

  return SRSLTE_SUCCESS;
}

int main()
{
  srslte::logmap::set_default_log_level(srslte::LOG_LEVEL_DEBUG);
  TESTASSERT(srslte::logmap::get("ASN1")->get_level() == srslte::LOG_LEVEL_DEBUG);

  TESTASSERT(test_s1setup_request() == 0);
  TESTASSERT(test_init_ctxt_setup_req() == 0);
  TESTASSERT(test_proc_id_consistency() == 0);

  printf("Success\n");
  return 0;
}
