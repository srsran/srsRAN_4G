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

#include <iostream>

#include "srslte/asn1/liblte_rrc.h"
#include "srslte/common/log_filter.h"
#include "srslte/srslte.h"

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

int rrc_connection_reconf_test()
{
  srslte::log_filter log1("RRC");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  LIBLTE_BIT_MSG_STRUCT         bit_buf;
  LIBLTE_BIT_MSG_STRUCT         bit_buf2;
  LIBLTE_BYTE_MSG_STRUCT        byte_buf;
  LIBLTE_RRC_DL_DCCH_MSG_STRUCT dl_dcch_msg;

  uint8_t  rrc_message[]   = {0x20, 0x02, 0x01, 0x80, 0x01, 0x10, 0x10, 0x08, 0x21, 0x60, 0xCA, 0x32, 0x00, 0x06, 0x60};
  uint32_t rrc_message_len = sizeof(rrc_message);
  // 20020180011010082160CA32000660

  bzero(&dl_dcch_msg, sizeof(dl_dcch_msg));
  srslte_bit_unpack_vector(rrc_message, bit_buf.msg, rrc_message_len * 8);
  bit_buf.N_bits = rrc_message_len * 8;
  liblte_rrc_unpack_dl_dcch_msg((LIBLTE_BIT_MSG_STRUCT*)&bit_buf, &dl_dcch_msg);
  TESTASSERT(dl_dcch_msg.msg_type == LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RECONFIG);

  LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT* reconf = &dl_dcch_msg.msg.rrc_con_reconfig;
  TESTASSERT(reconf->rrc_transaction_id == 0);
  TESTASSERT(not reconf->meas_cnfg_present);
  TESTASSERT(not reconf->mob_ctrl_info_present);
  TESTASSERT(reconf->N_ded_info_nas == 0);
  TESTASSERT(reconf->rr_cnfg_ded_present);
  TESTASSERT(not reconf->sec_cnfg_ho_present);

  // RR Config
  TESTASSERT(reconf->rr_cnfg_ded.phy_cnfg_ded_present);
  TESTASSERT(reconf->rr_cnfg_ded.drb_to_add_mod_list_size == 0);

  TESTASSERT(reconf->rr_cnfg_ded.phy_cnfg_ded.ext);
  TESTASSERT(reconf->rr_cnfg_ded.phy_cnfg_ded.ext_groups.N_groups == 5);
  TESTASSERT(reconf->rr_cnfg_ded.phy_cnfg_ded.ext_groups.groups_present[1]);
  TESTASSERT(reconf->rr_cnfg_ded.phy_cnfg_ded.ext_groups.group1.pucch_config_dedicated_v1020_present);
  LIBLTE_RRC_PUCCH_CONFIG_DEDICATED_v1020* v1020 =
      &reconf->rr_cnfg_ded.phy_cnfg_ded.ext_groups.group1.pucch_config_dedicated_v1020;

  TESTASSERT(v1020->pucch_format_r10_present);
  TESTASSERT(v1020->pucch_format_r10_choice == LIBLTE_RRC_PUCCH_CONFIG_DEDICATED_v1020::format3_r10);
  TESTASSERT(v1020->pucch_format_r10.format3_r10.N_n3_pucch_an_list_r13 == 4);
  TESTASSERT(v1020->pucch_format_r10.format3_r10.n3_pucch_an_list_r13[0] == 25);
  TESTASSERT(v1020->pucch_format_r10.format3_r10.n3_pucch_an_list_r13[1] == 281);
  TESTASSERT(v1020->pucch_format_r10.format3_r10.n3_pucch_an_list_r13[2] == 0);
  TESTASSERT(v1020->pucch_format_r10.format3_r10.n3_pucch_an_list_r13[3] == 51);

  return 0;
}

int main(int argc, char** argv)
{
  TESTASSERT(rrc_connection_reconf_test() == 0);
}