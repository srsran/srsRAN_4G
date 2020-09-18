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

#include "srslte/asn1/liblte_mme.h"
#include "srslte/common/log_filter.h"
#include <iostream>
#include <memory>
#include <srslte/common/buffer_pool.h>
#include <srslte/common/int_helpers.h>
#include <srslte/srslte.h>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

int nas_dedicated_eps_bearer_context_setup_request_test()
{
  srslte::log_filter log1("NAS");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  srslte::byte_buffer_pool*    pool = srslte::byte_buffer_pool::get_instance();
  srslte::unique_byte_buffer_t tst_msg, out_msg;
  tst_msg = allocate_unique_buffer(*pool);
  out_msg = allocate_unique_buffer(*pool);

  LIBLTE_MME_ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT ded_bearer_req;

  LIBLTE_ERROR_ENUM err;
  uint8_t           nas_message[] = {0x27, 0x35, 0x11, 0xfd, 0xf6, 0x07, 0x62, 0x00, 0xc5, 0x05, 0x01, 0x01, 0x07, 0x21,
                           0x31, 0x00, 0x03, 0x40, 0x08, 0xae, 0x5d, 0x02, 0x00, 0xc2, 0x81, 0x34, 0x01, 0x4d};
  uint32_t          nas_message_len = sizeof(nas_message);

  // Unpack Activate Dedicated EPS bearer context setup request
  tst_msg->N_bytes = nas_message_len;
  memcpy(tst_msg->msg, nas_message, nas_message_len);
  log1.info_hex(tst_msg->msg, tst_msg->N_bytes, "NAS Activate Dedicated EPS Bearer Context Request original message\n");

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
  srslte::uint8_to_uint16(&ded_bearer_req.tft.packet_filter_list[0].filter[1], &port);
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
  printf("Test NAS Activate Dedicated EPS Bearer Context Request successfull\n");
  return 0;
}

int main(int argc, char** argv)
{
  int result = nas_dedicated_eps_bearer_context_setup_request_test();
  return result;
}
