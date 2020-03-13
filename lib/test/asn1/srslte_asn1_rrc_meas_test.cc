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

#include "srslte/asn1/rrc_asn1.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/log_filter.h"
#include "srslte/interfaces/rrc_interface_types.h"
#include <iostream>
#include <srslte/srslte.h>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

using namespace asn1;
using namespace asn1::rrc;

int meas_obj_test()
{
  srslte::log_filter log1("RRC");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  uint8_t rrc_msg[] = {
      0x08, 0x10, 0x49, 0x3C, 0x0D, 0x97, 0x89, 0x83, 0xC0, 0x84, 0x20, 0x82, 0x08, 0x21, 0x00, 0x01, 0xBC, 0x48};
  uint32_t rrc_msg_len = sizeof(rrc_msg);
  // 0810493C0D978983C084208208210001BC48

  cbit_ref bref(&rrc_msg[0], sizeof(rrc_msg));

  ul_dcch_msg_s ul_dcch_msg;
  ul_dcch_msg.unpack(bref);

  TESTASSERT(ul_dcch_msg.msg.type() == ul_dcch_msg_type_c::types::c1);
  TESTASSERT(ul_dcch_msg.msg.c1().type() == ul_dcch_msg_type_c::c1_c_::types::meas_report);

  meas_report_s* rep = &ul_dcch_msg.msg.c1().meas_report();
  TESTASSERT(rep->crit_exts.type() == meas_report_s::crit_exts_c_::types::c1);
  TESTASSERT(rep->crit_exts.c1().type() == meas_report_s::crit_exts_c_::c1_c_::types::meas_report_r8);
  TESTASSERT(not rep->crit_exts.c1().meas_report_r8().non_crit_ext_present);
  meas_results_s* meas = &rep->crit_exts.c1().meas_report_r8().meas_results;

  TESTASSERT(not meas->ext);
  TESTASSERT(meas->meas_id == 1);
  TESTASSERT(meas->meas_result_pcell.rsrp_result == 73);
  TESTASSERT(meas->meas_result_pcell.rsrq_result == 15);
  TESTASSERT(meas->meas_result_neigh_cells_present);
  TESTASSERT(meas->meas_result_neigh_cells.type() ==
             meas_results_s::meas_result_neigh_cells_c_::types::meas_result_list_eutra);
  meas_result_list_eutra_l& meas_list = meas->meas_result_neigh_cells.meas_result_list_eutra();
  TESTASSERT(meas_list.size() == 1);
  TESTASSERT(meas_list[0].pci == 357);
  TESTASSERT(meas_list[0].cgi_info_present);
  TESTASSERT(meas_list[0].cgi_info.plmn_id_list_present);
  TESTASSERT(meas_list[0].cgi_info.cell_global_id.plmn_id.mcc_present);
  srslte::plmn_id_t plmn = srslte::make_plmn_id_t(meas_list[0].cgi_info.cell_global_id.plmn_id);
  TESTASSERT(plmn.to_string() == "89878");
  TESTASSERT(meas_list[0].cgi_info.cell_global_id.cell_id.to_number() == 0x1084104);
  TESTASSERT(meas_list[0].cgi_info.tac.to_number() == 0x1042);
  TESTASSERT(meas_list[0].cgi_info.plmn_id_list.size() == 1);
  TESTASSERT(not meas_list[0].cgi_info.plmn_id_list[0].mcc_present);
  TESTASSERT(not meas_list[0].meas_result.ext);
  TESTASSERT(meas_list[0].meas_result.rsrp_result_present);
  TESTASSERT(meas_list[0].meas_result.rsrp_result == 60);
  TESTASSERT(meas_list[0].meas_result.rsrq_result_present);
  TESTASSERT(meas_list[0].meas_result.rsrq_result == 18);

  uint8_t rrc_msg2[rrc_msg_len];
  bzero(rrc_msg2, rrc_msg_len);
  bit_ref bref2(&rrc_msg2[0], sizeof(rrc_msg2));
  ul_dcch_msg.pack(bref2);
  TESTASSERT(bref.distance() == bref2.distance());
  TESTASSERT(memcmp(rrc_msg2, rrc_msg, rrc_msg_len) == 0);

  return 0;
}

int main(int argc, char** argv)
{
  TESTASSERT(meas_obj_test() == 0);
  return 0;
}
