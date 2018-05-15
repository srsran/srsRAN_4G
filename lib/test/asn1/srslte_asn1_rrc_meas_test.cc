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

#include <assert.h>
#include <iostream>
#include <srslte/srslte.h>
#include "srslte/common/log_filter.h"
#include "srslte/asn1/liblte_rrc.h"


void basic_test() {
  srslte::log_filter log1("RRC");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  LIBLTE_BIT_MSG_STRUCT           bit_buf;
  LIBLTE_BIT_MSG_STRUCT           bit_buf2;
  LIBLTE_BYTE_MSG_STRUCT          byte_buf;
  LIBLTE_RRC_UL_DCCH_MSG_STRUCT   ul_dcch_msg;

  uint32_t rrc_message_len  = 18;
  uint8_t  rrc_message[] = {0x08, 0x10, 0x49, 0x3C, 0x0D, 0x97, 0x89, 0x83,
                            0xC0, 0x84, 0x20, 0x82, 0x08, 0x21, 0x00, 0x01,
                            0xBC, 0x48};

  srslte_bit_unpack_vector(rrc_message, bit_buf.msg, rrc_message_len*8);
  bit_buf.N_bits = rrc_message_len*8;
  liblte_rrc_unpack_ul_dcch_msg((LIBLTE_BIT_MSG_STRUCT*)&bit_buf, &ul_dcch_msg);

  assert(ul_dcch_msg.msg_type == LIBLTE_RRC_UL_DCCH_MSG_TYPE_MEASUREMENT_REPORT);
  LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT *rep = &ul_dcch_msg.msg.measurement_report;
  assert(rep->meas_id == 1);
  assert(rep->pcell_rsrp_result == 73);
  assert(rep->pcell_rsrq_result == 15);
  assert(rep->have_meas_result_neigh_cells);
  assert(rep->meas_result_neigh_cells_choice == LIBLTE_RRC_MEAS_RESULT_LIST_EUTRA);
  LIBLTE_RRC_MEAS_RESULT_LIST_EUTRA_STRUCT *eutra = &rep->meas_result_neigh_cells.eutra;
  assert(eutra->n_result == 1);
  assert(eutra->result_eutra_list[0].phys_cell_id == 357);
  assert(eutra->result_eutra_list[0].have_cgi_info);
  assert(eutra->result_eutra_list[0].cgi_info.have_plmn_identity_list);
  assert(eutra->result_eutra_list[0].cgi_info.cell_global_id.plmn_id.mcc == 0xF898);
  assert(eutra->result_eutra_list[0].cgi_info.cell_global_id.plmn_id.mnc == 0xFF78);
  assert(eutra->result_eutra_list[0].cgi_info.cell_global_id.cell_id == 0x1084104);
  assert(eutra->result_eutra_list[0].cgi_info.tracking_area_code == 0x1042);
  assert(eutra->result_eutra_list[0].cgi_info.have_plmn_identity_list);
  assert(eutra->result_eutra_list[0].cgi_info.n_plmn_identity_list == 1);
  assert(eutra->result_eutra_list[0].cgi_info.plmn_identity_list[0].mcc == 0xFFFF);
  assert(eutra->result_eutra_list[0].cgi_info.plmn_identity_list[0].mnc == 0xFF00);
  assert(eutra->result_eutra_list[0].meas_result.have_rsrp);
  assert(eutra->result_eutra_list[0].meas_result.rsrp_result == 60);
  assert(eutra->result_eutra_list[0].meas_result.have_rsrp);
  assert(eutra->result_eutra_list[0].meas_result.rsrq_result == 18);

  liblte_rrc_pack_ul_dcch_msg(&ul_dcch_msg, (LIBLTE_BIT_MSG_STRUCT*)&bit_buf2);
  srslte_bit_pack_vector(bit_buf2.msg, byte_buf.msg, bit_buf2.N_bits);
  byte_buf.N_bytes = (bit_buf2.N_bits+7)/8;
  log1.info_hex(byte_buf.msg, byte_buf.N_bytes, "UL_DCCH Packed message\n");

  for(uint32_t i=0; i<rrc_message_len; i++) {
    assert(byte_buf.msg[i] == rrc_message[i]);
  }
}


int main(int argc, char **argv) {
  basic_test();
}
