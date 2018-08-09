/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <iostream>
#include <srslte/srslte.h>
#include "srslte/common/log_filter.h"
#include "srslte/asn1/liblte_rrc.h"
#include "srslte/asn1/liblte_mme.h"

void nas_test() {
  srslte::log_filter log1("NAS");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(-1);

  uint32_t nas_message_len  = 73;
  uint8_t  nas_message[128] = {0x27, 0x4f, 0xab, 0xef, 0x59, 0x01, 0x07, 0x42,
                               0x01, 0x49, 0x06, 0x40, 0x00, 0xf1, 0x10, 0x31,
                               0x32, 0x00, 0x22, 0x52, 0x01, 0xc1, 0x05, 0x07,
                               0xff, 0xff, 0xff, 0xff, 0x0c, 0x0b, 0x76, 0x7a,
                               0x77, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65,
                               0x74, 0x05, 0x01, 0x0e, 0x0e, 0x0e, 0x01, 0x5e,
                               0x04, 0xfe, 0xfe, 0x81, 0x4e, 0x50, 0x0b, 0xf6,
                               0x00, 0xf1, 0x10, 0x00, 0x02, 0x01, 0x01, 0x00,
                               0x00, 0x62, 0x17, 0x2c, 0x59, 0x49, 0x64, 0x01,
                               0x03};

  uint8                               pd;
  uint8                               msg_type;
  LIBLTE_BYTE_MSG_STRUCT              buf;
  LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT attach_accept;
  bzero(&attach_accept, sizeof(LIBLTE_MME_ATTACH_ACCEPT_MSG_STRUCT));
  LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT  act_def_eps_bearer_context_req;
  bzero(&act_def_eps_bearer_context_req, sizeof(LIBLTE_MME_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_MSG_STRUCT));

  bzero(&buf, sizeof(LIBLTE_BYTE_MSG_STRUCT));
  memcpy(buf.msg, nas_message, nas_message_len);
  buf.N_bytes = nas_message_len;
  liblte_mme_parse_msg_header(&buf, &pd, &msg_type);
  switch(msg_type)
  {
  case LIBLTE_MME_MSG_TYPE_ATTACH_ACCEPT:
      liblte_mme_unpack_attach_accept_msg(&buf, &attach_accept);
      liblte_mme_unpack_activate_default_eps_bearer_context_request_msg(&attach_accept.esm_msg, &act_def_eps_bearer_context_req);
      break;
  case LIBLTE_MME_MSG_TYPE_ATTACH_REJECT:
      break;
  case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REQUEST:

      break;
  case LIBLTE_MME_MSG_TYPE_AUTHENTICATION_REJECT:

      break;
  case LIBLTE_MME_MSG_TYPE_IDENTITY_REQUEST:

      break;
  case LIBLTE_MME_MSG_TYPE_SECURITY_MODE_COMMAND:

      break;
  case LIBLTE_MME_MSG_TYPE_SERVICE_REJECT:

      break;
  case LIBLTE_MME_MSG_TYPE_ESM_INFORMATION_REQUEST:

      break;
  case LIBLTE_MME_MSG_TYPE_EMM_INFORMATION:

      break;
  default:
      break;
  }
}

void basic_test() {
  srslte::log_filter log1("RRC");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(-1);

  LIBLTE_BIT_MSG_STRUCT           bit_buf;
  LIBLTE_RRC_DL_DCCH_MSG_STRUCT   dl_dcch_msg;

  uint32_t rrc_message_len  = 147;
  uint8_t  rrc_message[256] = {0x22, 0x16, 0x95, 0xa0, 0x18, 0x00, 0x05, 0xaa,
                               0x50, 0x36, 0x00, 0x61, 0x08, 0x9c, 0xe3, 0x40,
                               0xb0, 0x84, 0x4e, 0x71, 0xc0, 0x30, 0x84, 0x6e,
                               0x71, 0xe0, 0x70, 0x84, 0x6e, 0x70, 0x6c, 0x63,
                               0x1a, 0xc6, 0xb9, 0x8e, 0x7b, 0x1e, 0x84, 0xc0,
                               0x01, 0x24, 0x9d, 0x3e, 0xaf, 0xbd, 0x64, 0x04,
                               0x1d, 0x08, 0x05, 0x24, 0x19, 0x00, 0x03, 0xc4,
                               0x40, 0xc4, 0xc8, 0x00, 0x89, 0x48, 0x07, 0x04,
                               0x14, 0x1f, 0xff, 0xff, 0xff, 0xfc, 0x30, 0x2d,
                               0xd9, 0xe9, 0xdd, 0xa5, 0xb9, 0xd1, 0x95, 0xc9,
                               0xb9, 0x95, 0xd0, 0x14, 0x04, 0x38, 0x38, 0x38,
                               0x05, 0x78, 0x13, 0xfb, 0xfa, 0x05, 0x39, 0x40,
                               0x2f, 0xd8, 0x03, 0xc4, 0x40, 0x00, 0x08, 0x04,
                               0x04, 0x00, 0x01, 0x88, 0x5c, 0xb1, 0x65, 0x25,
                               0x90, 0x04, 0x0d, 0xa9, 0xc0, 0x2a, 0x9a, 0x01,
                               0x99, 0x3b, 0x01, 0xf5, 0x12, 0xf0, 0x85, 0x0d,
                               0x85, 0xef, 0xc0, 0x01, 0xf2, 0x20, 0x60, 0x18,
                               0x07, 0x97, 0x09, 0x1f, 0xc3, 0x06, 0x00, 0x81,
                               0x00, 0x00, 0x11};

  srslte_bit_unpack_vector(rrc_message, bit_buf.msg, rrc_message_len*8);
  bit_buf.N_bits = rrc_message_len*8;
  liblte_rrc_unpack_dl_dcch_msg((LIBLTE_BIT_MSG_STRUCT*)&bit_buf, &dl_dcch_msg);

  printf("done\n");
}


int main(int argc, char **argv) {
  basic_test();
  nas_test();
}
