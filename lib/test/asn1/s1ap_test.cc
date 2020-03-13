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
#include "srslte/common/log_filter.h"
#include "srslte/common/test_common.h"
#include <assert.h>
#include <string.h>

using namespace asn1;

int unpack_test_served_gummeis_with_multiple_plmns()
{
  uint8_t pdu[] = {0x20, 0x11, 0x00, 0x26, 0x00, 0x00, 0x02, 0x00, 0x69, 0x00, 0x1a, 0x01, 0x40, 0x00,
                   0xf1, 0x10, 0x00, 0xf1, 0x10, 0x00, 0xf1, 0x10, 0x00, 0xf1, 0x10, 0x00, 0xf1, 0x10,
                   0x00, 0xf1, 0x10, 0x00, 0x00, 0x88, 0x88, 0x00, 0x7b, 0x00, 0x57, 0x40, 0x01, 0xff};

  asn1::cbit_ref         bref(pdu, sizeof(pdu));
  asn1::s1ap::s1ap_pdu_c input_pdu;

  TESTASSERT(input_pdu.unpack(bref) == SRSASN_SUCCESS);
  TESTASSERT(asn1::test_pack_unpack_consistency(input_pdu) == SRSASN_SUCCESS);

  return SRSLTE_SUCCESS;
}

int test_load_info_obj()
{
  asn1::s1ap::init_context_setup_resp_ies_container container;
  container.erab_failed_to_setup_list_ctxt_su_res.value.resize(1);
  container.erab_failed_to_setup_list_ctxt_su_res.value[0].load_info_obj(ASN1_S1AP_ID_ERAB_ITEM);

  TESTASSERT(container.erab_failed_to_setup_list_ctxt_su_res.value[0].id == ASN1_S1AP_ID_ERAB_ITEM);
  TESTASSERT(container.erab_failed_to_setup_list_ctxt_su_res.value[0].crit.value == s1ap::crit_opts::reject);

  return SRSLTE_SUCCESS;
}

int test_initial_ctxt_setup_response()
{
  asn1::s1ap::s1ap_pdu_c tx_pdu;

  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_INIT_CONTEXT_SETUP);

  // Fill in the MME and eNB IDs
  auto& container                = tx_pdu.successful_outcome().value.init_context_setup_resp().protocol_ies;
  container.mme_ue_s1ap_id.value = 1;
  container.enb_ue_s1ap_id.value = 1;

  container.erab_setup_list_ctxt_su_res.value.resize(1);
  // Fill in the GTP bind address for all bearers
  for (uint32_t i = 0; i < container.erab_setup_list_ctxt_su_res.value.size(); ++i) {
    container.erab_setup_list_ctxt_su_res.value[i].load_info_obj(ASN1_S1AP_ID_ERAB_SETUP_ITEM_CTXT_SU_RES);
    auto& item   = container.erab_setup_list_ctxt_su_res.value[i].value.erab_setup_item_ctxt_su_res();
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
  TESTASSERT(tx_pdu.pack(bref) == SRSLTE_SUCCESS);

  srslte::logmap::get("TEST")->info_hex(
      buffer, bref.distance_bytes(), "message (nof bytes = %d):\n", bref.distance_bytes());

  return SRSLTE_SUCCESS;
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

  srslte::logmap::get("TEST")->info_hex(buffer, bref.distance_bytes(), "Packed cell id:\n");

  return SRSLTE_SUCCESS;
}

int main()
{
  srslte::logmap::set_default_log_level(srslte::LOG_LEVEL_DEBUG);
  srslte::logmap::set_default_hex_limit(1024);

  TESTASSERT(unpack_test_served_gummeis_with_multiple_plmns() == SRSLTE_SUCCESS);
  TESTASSERT(test_initial_ctxt_setup_response() == SRSLTE_SUCCESS);
  TESTASSERT(test_eci_pack() == SRSLTE_SUCCESS);
  printf("Success\n");
}
