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

int rrc_conn_reconfig_ho_test1()
{
  srslte::log_filter log1("RRC");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  uint8_t rrc_msg[] = {0x20, 0x1b, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x01, 0xa9, 0x08, 0x80, 0x00, 0x00, 0x29, 0x00,
                       0x97, 0x80, 0x00, 0x00, 0x00, 0x01, 0x04, 0x22, 0x14, 0x00, 0xf8, 0x02, 0x0a, 0xc0, 0x60,
                       0x00, 0xa0, 0x0c, 0x80, 0x42, 0x02, 0x9f, 0x43, 0x07, 0xda, 0xbc, 0xf8, 0x4b, 0x32, 0x18,
                       0x34, 0xc0, 0x00, 0x2d, 0x68, 0x08, 0x5e, 0x18, 0x00, 0x16, 0x80, 0x00};
  // 20 1b 3f 80 00 00 00 01 a9 08 80 00 00 29 00 97 80 00 00 00 01 04 22 14 00 f8 02 0a c0 60 00 a0 0c 80 42 02 9f 43
  // 07 da bc f8 4b 32 18 34 c0 00 2d 68 08 5e 18 00 16 80 00

  cbit_ref bref(rrc_msg, sizeof(rrc_msg));

  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.unpack(bref);

  TESTASSERT(dl_dcch_msg.msg.type() == dl_dcch_msg_type_c::types::c1);
  TESTASSERT(dl_dcch_msg.msg.c1().type() == dl_dcch_msg_type_c::c1_c_::types::rrc_conn_recfg);

  // assign to stack-allocated variable
  asn1::rrc::rrc_conn_recfg_s mob_reconf;
  mob_reconf = dl_dcch_msg.msg.c1().rrc_conn_recfg();

  // decode same message and assign again
  cbit_ref      bref2(rrc_msg, sizeof(rrc_msg));
  dl_dcch_msg_s dl_dcch_msg2;
  dl_dcch_msg2.unpack(bref2);
  TESTASSERT(dl_dcch_msg2.msg.type() == dl_dcch_msg_type_c::types::c1);
  TESTASSERT(dl_dcch_msg2.msg.c1().type() == dl_dcch_msg_type_c::c1_c_::types::rrc_conn_recfg);
  mob_reconf = dl_dcch_msg2.msg.c1().rrc_conn_recfg();

  return 0;
}

int main(int argc, char** argv)
{
  TESTASSERT(rrc_conn_reconfig_ho_test1() == 0);
  return 0;
}
