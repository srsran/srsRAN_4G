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

#include "srsran/asn1/liblte_mme.h"
#include "srsran/common/test_common.h"
#include "srsran/srslog/srslog.h"
#include <iostream>
#include <srsran/common/buffer_pool.h>
#include <srsran/common/int_helpers.h>

int nas_dedicated_eps_bearer_context_setup_request_test()
{
  auto& nas_logger = srslog::fetch_basic_logger("NAS", false);
  nas_logger.set_level(srslog::basic_levels::debug);
  nas_logger.set_hex_dump_max_size(128);

  srsran::unique_byte_buffer_t tst_msg, out_msg;
  tst_msg = srsran::make_byte_buffer();
  TESTASSERT(tst_msg != nullptr);
  out_msg = srsran::make_byte_buffer();
  TESTASSERT(out_msg != nullptr);

  LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT ded_bearer_req;

  LIBLTE_ERROR_ENUM err;
  uint8_t           nas_message[] = {0x27, 0x35, 0x11, 0xfd, 0xf6, 0x07, 0x62, 0x00, 0xc5, 0x05, 0x01, 0x01, 0x07, 0x21,
                           0x31, 0x00, 0x03, 0x40, 0x08, 0xae, 0x5d, 0x02, 0x00, 0xc2, 0x81, 0x34, 0x01, 0x4d};
  uint32_t          nas_message_len = sizeof(nas_message);

  // Unpack Activate Dedicated EPS bearer context setup request
  tst_msg->N_bytes = nas_message_len;
  memcpy(tst_msg->msg, nas_message, nas_message_len);
  nas_logger.info(tst_msg->msg, tst_msg->N_bytes, "NAS Activate Dedicated EPS Bearer Context Request original message");

  // Test message type and protocol discriminator
  uint8_t pd, msg_type;
  liblte_mme_parse_msg_header((LIBLTE_BYTE_MSG_STRUCT*)tst_msg.get(), &pd, &msg_type);
  TESTASSERT(msg_type == LIBLTE_MME_MSG_TYPE_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST);

  // Unpack message
  err = liblte_mme_unpack_activate_dedicated_eps_bearer_context_request_msg((LIBLTE_BYTE_MSG_STRUCT*)tst_msg.get(),
                                                                            &ded_bearer_req);
  TESTASSERT(err == LIBLTE_SUCCESS);

  // Check EPS bearer identity
  TESTASSERT(ded_bearer_req.eps_bearer_id == 6);

  // Procedure transaction Identity
  TESTASSERT(ded_bearer_req.proc_transaction_id == 0);

  // Linked EPS bearer identity
  TESTASSERT(ded_bearer_req.linked_eps_bearer_id == 5);

  // EPS quality of service
  TESTASSERT(ded_bearer_req.eps_qos.qci == 1);

  // Traffic flow template
  TESTASSERT(ded_bearer_req.tft.tft_op_code == LIBLTE_MME_TFT_OPERATION_CODE_CREATE_NEW_TFT);
  TESTASSERT(ded_bearer_req.tft.parameter_list_size == 0);
  TESTASSERT(ded_bearer_req.tft.packet_filter_list_size == 1);
  TESTASSERT(ded_bearer_req.tft.packet_filter_list[0].dir == LIBLTE_MME_TFT_PACKET_FILTER_DIRECTION_BIDIRECTIONAL);
  TESTASSERT(ded_bearer_req.tft.packet_filter_list[0].id == 1); // TODO Should this be 2?
  TESTASSERT(ded_bearer_req.tft.packet_filter_list[0].eval_precedence == 0);
  TESTASSERT(ded_bearer_req.tft.packet_filter_list[0].filter_size == 3);
  TESTASSERT(ded_bearer_req.tft.packet_filter_list[0].filter[0] ==
             LIBLTE_MME_TFT_PACKET_FILTER_COMPONENT_TYPE_ID_SINGLE_LOCAL_PORT_TYPE);

  uint16_t port;
  srsran::uint8_to_uint16(&ded_bearer_req.tft.packet_filter_list[0].filter[1], &port);
  TESTASSERT(port == 2222);

  // Linked Transaction Identifier
  TESTASSERT(ded_bearer_req.transaction_id_present == true);
  TESTASSERT(ded_bearer_req.transaction_id.ti_flag == LIBLTE_MME_TI_FLAG_SENT_FROM_ORIGINATOR);
  TESTASSERT(ded_bearer_req.transaction_id.tio == 0);
  TESTASSERT(ded_bearer_req.transaction_id.tie == 0x42);

  // Radio Priority
  TESTASSERT(ded_bearer_req.radio_prio_present == true);
  TESTASSERT(ded_bearer_req.radio_prio == 1);

  // Packet flow identifier
  TESTASSERT(ded_bearer_req.packet_flow_id_present == true);
  TESTASSERT(ded_bearer_req.packet_flow_id == 77); // Test flow id

  // NAS Activate Dedicated EPS Bearer Context Setup Request Pack Test (TODO)

  srslog::flush();
  printf("Test NAS Activate Dedicated EPS Bearer Context Request successfull\n");
  return SRSRAN_SUCCESS;
}

int downlink_generic_nas_transport_unpacking_test()
{
  uint8_t nas_message[] = {
      0x27, 0xae, 0x80, 0xc8, 0xf9, 0x06, 0x07, 0x68, 0x01, 0x00, 0x06, 0xf0, 0x00, 0x00, 0x00, 0x08, 0x70};
  srsran::unique_byte_buffer_t                         buf;
  LIBLTE_MME_DOWNLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT dl_generic_nas_transport;
  LIBLTE_ERROR_ENUM                                    err;

  copy_msg_to_buffer(buf, nas_message);
  err = liblte_mme_unpack_downlink_generic_nas_transport_msg((LIBLTE_BYTE_MSG_STRUCT*)buf.get(),
                                                             &dl_generic_nas_transport);
  TESTASSERT(err == LIBLTE_SUCCESS);
  TESTASSERT(dl_generic_nas_transport.generic_msg_cont_type == 1);
  TESTASSERT(dl_generic_nas_transport.generic_msg_cont.N_bytes == 6);
  TESTASSERT(dl_generic_nas_transport.add_info_present == false);

  return SRSRAN_SUCCESS;
}

int downlink_generic_nas_transport_packing_test()
{
  uint8_t nas_message[] = {
      0x27, 0x00, 0x00, 0x00, 0x00, 0xff, 0x07, 0x68, 0x01, 0x00, 0x06, 0xf0, 0x00, 0x00, 0x00, 0x08, 0x70};
  uint8_t                                              generic_msg_cont[] = {0xf0, 0x00, 0x00, 0x00, 0x08, 0x70};
  LIBLTE_BYTE_MSG_STRUCT                               buf                = {};
  LIBLTE_MME_DOWNLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT dl_generic_nas_transport;
  LIBLTE_ERROR_ENUM                                    err;

  dl_generic_nas_transport.generic_msg_cont_type    = 1;
  dl_generic_nas_transport.generic_msg_cont.N_bytes = sizeof(generic_msg_cont);
  memcpy(dl_generic_nas_transport.generic_msg_cont.msg, generic_msg_cont, sizeof(generic_msg_cont));
  dl_generic_nas_transport.add_info_present = false;

  err = liblte_mme_pack_downlink_generic_nas_transport_msg(
      &dl_generic_nas_transport, LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED, 0xffffffff, &buf);

  TESTASSERT(err == LIBLTE_SUCCESS);
  TESTASSERT(buf.N_bytes == sizeof(nas_message));
  TESTASSERT(memcmp(buf.msg, nas_message, buf.N_bytes) == 0);
  return SRSRAN_SUCCESS;
}

int downlink_generic_nas_transport_with_add_info_unpacking_test()
{
  uint8_t                      nas_message[] = {0x27, 0xae, 0x80, 0xc8, 0xf9, 0x06, 0x07, 0x68, 0x01, 0x00, 0x06,
                           0xf0, 0x00, 0x00, 0x00, 0x08, 0x70, 0x65, 0x02, 0x11, 0x11};
  srsran::unique_byte_buffer_t buf;
  LIBLTE_MME_DOWNLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT dl_generic_nas_transport;
  LIBLTE_ERROR_ENUM                                    err;

  copy_msg_to_buffer(buf, nas_message);
  err = liblte_mme_unpack_downlink_generic_nas_transport_msg((LIBLTE_BYTE_MSG_STRUCT*)buf.get(),
                                                             &dl_generic_nas_transport);
  TESTASSERT(err == LIBLTE_SUCCESS);
  TESTASSERT(dl_generic_nas_transport.generic_msg_cont_type == 1);
  TESTASSERT(dl_generic_nas_transport.generic_msg_cont.N_bytes == 6);
  TESTASSERT(dl_generic_nas_transport.add_info_present == true);
  TESTASSERT(dl_generic_nas_transport.add_info.N_octets == 2);

  return SRSRAN_SUCCESS;
}

int downlink_generic_nas_transport_with_add_info_packing_test()
{
  uint8_t                nas_message[]      = {0x27, 0x00, 0x00, 0x00, 0x00, 0xff, 0x07, 0x68, 0x01, 0x00, 0x06,
                           0xf0, 0x00, 0x00, 0x00, 0x08, 0x70, 0x65, 0x02, 0x11, 0x11};
  uint8_t                generic_msg_cont[] = {0xf0, 0x00, 0x00, 0x00, 0x08, 0x70};
  uint8_t                add_info[]         = {0x11, 0x11};
  LIBLTE_BYTE_MSG_STRUCT buf                = {};
  LIBLTE_MME_DOWNLINK_GENERIC_NAS_TRANSPORT_MSG_STRUCT dl_generic_nas_transport;
  LIBLTE_ERROR_ENUM                                    err;

  dl_generic_nas_transport.generic_msg_cont_type    = 1;
  dl_generic_nas_transport.generic_msg_cont.N_bytes = sizeof(generic_msg_cont);
  memcpy(dl_generic_nas_transport.generic_msg_cont.msg, generic_msg_cont, sizeof(generic_msg_cont));
  dl_generic_nas_transport.add_info_present  = true;
  dl_generic_nas_transport.add_info.N_octets = sizeof(add_info);
  memcpy(dl_generic_nas_transport.add_info.info, add_info, sizeof(add_info));

  err = liblte_mme_pack_downlink_generic_nas_transport_msg(
      &dl_generic_nas_transport, LIBLTE_MME_SECURITY_HDR_TYPE_INTEGRITY_AND_CIPHERED, 0xffffffff, &buf);

  TESTASSERT(err == LIBLTE_SUCCESS);
  TESTASSERT(buf.N_bytes == sizeof(nas_message));
  TESTASSERT(memcmp(buf.msg, nas_message, buf.N_bytes) == 0);
  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  auto& asn1_logger = srslog::fetch_basic_logger("ASN1", false);
  asn1_logger.set_level(srslog::basic_levels::debug);
  asn1_logger.set_hex_dump_max_size(-1);

  srsran::test_init(argc, argv);

  TESTASSERT(nas_dedicated_eps_bearer_context_setup_request_test() == SRSRAN_SUCCESS);
  TESTASSERT(downlink_generic_nas_transport_unpacking_test() == SRSRAN_SUCCESS);
  TESTASSERT(downlink_generic_nas_transport_packing_test() == SRSRAN_SUCCESS);
  TESTASSERT(downlink_generic_nas_transport_with_add_info_unpacking_test() == SRSRAN_SUCCESS);
  TESTASSERT(downlink_generic_nas_transport_with_add_info_packing_test() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}
