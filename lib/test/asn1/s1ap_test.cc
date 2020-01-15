/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/asn1/liblte_s1ap.h"
#include "srslte/asn1/s1ap_asn1.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/test_common.h"
#include <assert.h>
#include <string.h>

using namespace asn1;

srslte::scoped_tester_log test_logger("TEST");

int unpack_test_served_gummeis_with_multiple_plmns()
{
  uint8_t pdu[] = {0x20, 0x11, 0x00, 0x26, 0x00, 0x00, 0x02, 0x00, 0x69, 0x00, 0x1a, 0x01, 0x40, 0x00,
                   0xf1, 0x10, 0x00, 0xf1, 0x10, 0x00, 0xf1, 0x10, 0x00, 0xf1, 0x10, 0x00, 0xf1, 0x10,
                   0x00, 0xf1, 0x10, 0x00, 0x00, 0x88, 0x88, 0x00, 0x7b, 0x00, 0x57, 0x40, 0x01, 0xff};

  asn1::bit_ref          bref(pdu, sizeof(pdu));
  asn1::s1ap::s1ap_pdu_c input_pdu;

  TESTASSERT(input_pdu.unpack(bref) == SRSASN_SUCCESS);
  TESTASSERT(asn1::test_pack_unpack_consistency(input_pdu) == SRSASN_SUCCESS);

  return SRSLTE_SUCCESS;
}

int test_load_info_obj()
{
  asn1::s1ap::init_context_setup_resp_ies_container container;
  container.e_rab_failed_to_setup_list_ctxt_su_res.value.resize(1);
  container.e_rab_failed_to_setup_list_ctxt_su_res.value[0].load_info_obj(ASN1_S1AP_ID_E_RAB_ITEM);

  TESTASSERT(container.e_rab_failed_to_setup_list_ctxt_su_res.value[0].id == ASN1_S1AP_ID_E_RAB_ITEM);
  TESTASSERT(container.e_rab_failed_to_setup_list_ctxt_su_res.value[0].crit.value == s1ap::crit_opts::reject);

  return SRSLTE_SUCCESS;
}

int test_initial_ctxt_setup_response()
{
  asn1::s1ap::s1ap_pdu_c tx_pdu;

  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_INIT_CONTEXT_SETUP);

  // Fill in the MME and eNB IDs
  auto& container                = tx_pdu.successful_outcome().value.init_context_setup_request().protocol_ies;
  container.mme_ue_s1ap_id.value = 1;
  container.enb_ue_s1ap_id.value = 1;

  container.e_rab_setup_list_ctxt_su_res.value.resize(1);
  // Fill in the GTP bind address for all bearers
  for (uint32_t i = 0; i < container.e_rab_setup_list_ctxt_su_res.value.size(); ++i) {
    container.e_rab_setup_list_ctxt_su_res.value[i].load_info_obj(ASN1_S1AP_ID_E_RAB_SETUP_ITEM_CTXT_SU_RES);
    auto& item    = container.e_rab_setup_list_ctxt_su_res.value[i].value.e_rab_setup_item_ctxt_su_res();
    item.e_rab_id = 1;
    // uint32_to_uint8(teid_in, item.gtp_teid.data());
    item.transport_layer_address.resize(32);
    uint8_t addr[4];
    inet_pton(AF_INET, "127.0.0.1", addr);
    liblte_unpack(addr, 4, item.transport_layer_address.data());
  }

  uint8_t       buffer[1024];
  asn1::bit_ref bref(buffer, sizeof(buffer));
  TESTASSERT(tx_pdu.pack(bref) == SRSLTE_SUCCESS);

  return SRSLTE_SUCCESS;
}

int main()
{
  test_logger.set_level(LOG_LEVEL_DEBUG);

  TESTASSERT(unpack_test_served_gummeis_with_multiple_plmns() == SRSLTE_SUCCESS);
  printf("Success\n");
}
