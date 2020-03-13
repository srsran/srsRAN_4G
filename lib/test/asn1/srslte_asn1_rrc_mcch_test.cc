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

using namespace asn1::rrc;

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

int meas_obj_test()
{
  srslte::log_filter log1("RRC");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(1024);

  uint8_t  known_reference[]   = {0x0d, 0x8f, 0xdf, 0xff, 0xff, 0xff, 0xe2, 0x2f, 0xfc, 0x38,
                               0x5e, 0x61, 0xec, 0xa8, 0x00, 0x00, 0x02, 0x02, 0x10, 0x00,
                               0x20, 0x05, 0xe6, 0x1e, 0xca, 0x80, 0x00, 0x00, 0x40, 0x42};
  uint32_t known_reference_len = sizeof(known_reference);
  // 0d8fdfffffffe22ffc385e61eca80000020210002005e61eca8000004042

  asn1::cbit_ref        bref(&known_reference[0], sizeof(known_reference));
  asn1::rrc::mcch_msg_s mcch_msg;

  mcch_msg.unpack(bref);

  TESTASSERT(mcch_msg.msg.type() == mcch_msg_type_c::types::c1);
  TESTASSERT(mcch_msg.msg.c1().type() == mcch_msg_type_c::c1_c_::types::mbsfn_area_cfg_r9);
  mbsfn_area_cfg_r9_s* area_cfg_r9 = &mcch_msg.msg.c1().mbsfn_area_cfg_r9();
  TESTASSERT(not area_cfg_r9->non_crit_ext_present);
  TESTASSERT(area_cfg_r9->common_sf_alloc_r9.size() == 2);
  TESTASSERT(area_cfg_r9->common_sf_alloc_r9[0].radioframe_alloc_period ==
             mbsfn_sf_cfg_s::radioframe_alloc_period_e_::n32);
  TESTASSERT(area_cfg_r9->common_sf_alloc_r9[0].radioframe_alloc_offset == 4);
  TESTASSERT(area_cfg_r9->common_sf_alloc_r9[0].sf_alloc.type() == mbsfn_sf_cfg_s::sf_alloc_c_::types::one_frame);
  TESTASSERT(area_cfg_r9->common_sf_alloc_r9[0].sf_alloc.one_frame().to_string() == "111111");
  TESTASSERT(area_cfg_r9->common_sf_alloc_r9[1].radioframe_alloc_period ==
             mbsfn_sf_cfg_s::radioframe_alloc_period_e_::n8);
  TESTASSERT(area_cfg_r9->common_sf_alloc_r9[1].radioframe_alloc_offset == 7);
  TESTASSERT(area_cfg_r9->common_sf_alloc_r9[1].sf_alloc.type() == mbsfn_sf_cfg_s::sf_alloc_c_::types::four_frames);
  TESTASSERT(area_cfg_r9->common_sf_alloc_r9[1].sf_alloc.four_frames().to_string() == "111111111111111111111111");
  TESTASSERT(area_cfg_r9->common_sf_alloc_period_r9 == mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_e_::rf256);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9.size() == 2);
  TESTASSERT(not area_cfg_r9->pmch_info_list_r9[0].ext);
  TESTASSERT(not area_cfg_r9->pmch_info_list_r9[0].pmch_cfg_r9.ext);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[0].pmch_cfg_r9.sf_alloc_end_r9 == 1535);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[0].pmch_cfg_r9.data_mcs_r9 == 16);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[0].pmch_cfg_r9.mch_sched_period_r9 ==
             pmch_cfg_r9_s::mch_sched_period_r9_e_::rf1024);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[0].mbms_session_info_list_r9.size() == 1);
  TESTASSERT(not area_cfg_r9->pmch_info_list_r9[0].mbms_session_info_list_r9[0].ext);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[0].mbms_session_info_list_r9[0].session_id_r9_present);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[0].mbms_session_info_list_r9[0].tmgi_r9.plmn_id_r9.type() ==
             tmgi_r9_s::plmn_id_r9_c_::types::explicit_value_r9);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[0]
                 .mbms_session_info_list_r9[0]
                 .tmgi_r9.plmn_id_r9.explicit_value_r9()
                 .mcc_present);
  srslte::plmn_id_t plmn = srslte::make_plmn_id_t(
      area_cfg_r9->pmch_info_list_r9[0].mbms_session_info_list_r9[0].tmgi_r9.plmn_id_r9.explicit_value_r9());
  TESTASSERT(plmn.to_string() == "987654");
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[0].mbms_session_info_list_r9[0].tmgi_r9.service_id_r9.to_string() ==
             "000001");
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[0].mbms_session_info_list_r9[0].session_id_r9.to_string() == "01");
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[0].mbms_session_info_list_r9[0].lc_ch_id_r9 == 1);

  TESTASSERT(not area_cfg_r9->pmch_info_list_r9[1].ext);
  TESTASSERT(not area_cfg_r9->pmch_info_list_r9[1].pmch_cfg_r9.ext);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[1].pmch_cfg_r9.sf_alloc_end_r9 == 0);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[1].pmch_cfg_r9.data_mcs_r9 == 8);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[1].pmch_cfg_r9.mch_sched_period_r9 ==
             pmch_cfg_r9_s::mch_sched_period_r9_e_::rf8);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[1].mbms_session_info_list_r9.size() == 1);
  TESTASSERT(not area_cfg_r9->pmch_info_list_r9[1].mbms_session_info_list_r9[0].ext);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[1].mbms_session_info_list_r9[0].session_id_r9_present);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[1].mbms_session_info_list_r9[0].tmgi_r9.plmn_id_r9.type() ==
             tmgi_r9_s::plmn_id_r9_c_::types::explicit_value_r9);
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[1]
                 .mbms_session_info_list_r9[0]
                 .tmgi_r9.plmn_id_r9.explicit_value_r9()
                 .mcc_present);
  plmn = srslte::make_plmn_id_t(
      area_cfg_r9->pmch_info_list_r9[0].mbms_session_info_list_r9[0].tmgi_r9.plmn_id_r9.explicit_value_r9());
  TESTASSERT(plmn.to_string() == "987654");
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[1].mbms_session_info_list_r9[0].tmgi_r9.service_id_r9.to_string() ==
             "000002");
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[1].mbms_session_info_list_r9[0].session_id_r9.to_string() == "02");
  TESTASSERT(area_cfg_r9->pmch_info_list_r9[1].mbms_session_info_list_r9[0].lc_ch_id_r9 == 2);

  // log1.info_hex(byte_buf.msg, byte_buf.N_bytes, "MCCH packed message:");

  uint8_t rrc_msg2[known_reference_len];
  bzero(rrc_msg2, sizeof(rrc_msg2));
  asn1::bit_ref bref2(&rrc_msg2[0], sizeof(rrc_msg2));
  mcch_msg.pack(bref2);
  TESTASSERT(bref.distance() == bref2.distance());
  TESTASSERT(memcmp(rrc_msg2, known_reference, known_reference_len) == 0);

  return 0;
}

int main(int argc, char** argv)
{
  TESTASSERT(meas_obj_test() == 0);
  return 0;
}
