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
#include "srslte/common/log_filter.h"
#include <assert.h>
#include <string.h>

void unpack_test_served_gummeis_with_multiple_plmns() {
  srslte::log_filter log1("Log");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(1024);
  LIBLTE_S1AP_S1AP_PDU_STRUCT compare_pdu;
  LIBLTE_S1AP_S1AP_PDU_STRUCT output_pdu;
  LIBLTE_BYTE_MSG_STRUCT input_pdu;

  uint8_t pdu[] = {0x20, 0x11, 0x00, 0x26, 0x00, 0x00, 0x02, 0x00, 0x69,
                   0x00, 0x1a, 0x01, 0x40, 0x00, 0xf1, 0x10, 0x00, 0xf1,
                   0x10, 0x00, 0xf1, 0x10, 0x00, 0xf1, 0x10, 0x00, 0xf1,
                   0x10, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x88, 0x88, 0x00,
                   0x7b, 0x00, 0x57, 0x40, 0x01, 0xff};

  input_pdu.N_bytes = sizeof(pdu);

  memcpy(&input_pdu.msg, pdu, sizeof(pdu));

  log1.debug_hex(input_pdu.msg, input_pdu.N_bytes, "Input message len = %d",
                 input_pdu.N_bytes);
  assert(liblte_s1ap_unpack_s1ap_pdu((LIBLTE_BYTE_MSG_STRUCT *)&input_pdu,
                                     &output_pdu) == LIBLTE_SUCCESS);
}

int main(int argc, char **argv) {
  unpack_test_served_gummeis_with_multiple_plmns();
}
