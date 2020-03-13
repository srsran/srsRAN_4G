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
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/log_filter.h"
#include <iostream>

using namespace asn1;
using namespace asn1::rrc;

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

int rrc_conn_setup_test1()
{
  srslte::log_filter log1("RRC");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  uint8_t  rrc_msg[]   = {0x60, 0x12, 0x98, 0x0b, 0xfd, 0xd2, 0x04, 0xfa, 0x18, 0x3e, 0xd5, 0xe6, 0xc2,
                       0x59, 0x90, 0xc1, 0xa6, 0x00, 0x01, 0x31, 0x40, 0x42, 0x50, 0x80, 0x00, 0xf8};
  uint32_t rrc_msg_len = sizeof(rrc_msg);
  // 6012980bfdd204fa183ed5e6c25990c1a60001314042508000f8

  cbit_ref bref(&rrc_msg[0], sizeof(rrc_msg));

  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.unpack(bref);

  TESTASSERT(dl_ccch_msg.msg.type() == dl_ccch_msg_type_c::types::c1);
  TESTASSERT(dl_ccch_msg.msg.c1().type() == dl_ccch_msg_type_c::c1_c_::types::rrc_conn_setup);

  rrc_conn_setup_s* setup = &dl_ccch_msg.msg.c1().rrc_conn_setup();

  // TODO: add test for setup

  rr_cfg_ded_s* cnfg = &setup->crit_exts.c1().rrc_conn_setup_r8().rr_cfg_ded;
  TESTASSERT(cnfg->phys_cfg_ded_present);

  // TODO: add tests for RR config dedicated

  phys_cfg_ded_s* phy_cnfg = &cnfg->phys_cfg_ded;
  TESTASSERT(phy_cnfg->cqi_report_cfg_present);

  // Test CQI config
  cqi_report_cfg_s* cqi_cfg = &phy_cnfg->cqi_report_cfg;
  TESTASSERT(cqi_cfg->cqi_report_periodic_present);
  TESTASSERT(cqi_cfg->nom_pdsch_rs_epre_offset == 0);
  TESTASSERT(cqi_cfg->cqi_report_periodic.type() == setup_e::setup);
  TESTASSERT(cqi_cfg->cqi_report_periodic.setup().cqi_pucch_res_idx == 0);
  TESTASSERT(cqi_cfg->cqi_report_periodic.setup().cqi_pmi_cfg_idx == 38);

  // test repacking
  uint8_t rrc_msg2[rrc_msg_len];
  bzero(rrc_msg2, sizeof(rrc_msg2));
  bit_ref bref2(&rrc_msg2[0], sizeof(rrc_msg2));
  if (dl_ccch_msg.pack(bref2) != SRSASN_SUCCESS) {
    return -1;
  }
  TESTASSERT(bref.distance() == bref2.distance());
  TESTASSERT(memcmp(rrc_msg2, rrc_msg, rrc_msg_len) == 0);

  return 0;
}

// Only packing implemented
int rrc_reestablishment_reject_test()
{
  srslte::log_filter log1("RRC");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set(dl_ccch_msg_type_c::types::c1);

  dl_ccch_msg.msg.c1().set(dl_ccch_msg_type_c::c1_c_::types::rrc_conn_reest_reject);
  dl_ccch_msg.msg.c1().rrc_conn_reest_reject().crit_exts.set(
      rrc_conn_reest_reject_s::crit_exts_c_::types::rrc_conn_reest_reject_r8);
  dl_ccch_msg.msg.c1().rrc_conn_reest_reject().crit_exts.rrc_conn_reest_reject_r8();

  // test repacking
  uint8_t rrc_msg[32];
  bzero(rrc_msg, sizeof(rrc_msg));
  bit_ref bref(rrc_msg, sizeof(rrc_msg));
  if (dl_ccch_msg.pack(bref) != SRSASN_SUCCESS) {
    return -1;
  }

  int actual_len = bref.distance_bytes(rrc_msg);
  log1.info_hex(rrc_msg, actual_len, "DL-CCCH message (%d/%zd B)\n", actual_len, sizeof(rrc_msg));

  return 0;
}

int main(int argc, char** argv)
{
  TESTASSERT(rrc_conn_setup_test1() == 0);
  TESTASSERT(rrc_reestablishment_reject_test() == 0);
  return 0;
}