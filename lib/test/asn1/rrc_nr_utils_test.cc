/**
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

#include <getopt.h>
#include <iostream>

#include "srslte/asn1/rrc_nr.h"
#include "srslte/asn1/rrc_nr_utils.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/logmap.h"
#include "srslte/common/test_common.h"

using namespace srslte;

int test_rlc_config()
{
  asn1::rrc_nr::rlc_cfg_c rlc_cfg_asn1;
  rlc_cfg_asn1.set_um_bi_dir();
  rlc_cfg_asn1.um_bi_dir().dl_um_rlc.sn_field_len_present = true;
  rlc_cfg_asn1.um_bi_dir().dl_um_rlc.sn_field_len         = asn1::rrc_nr::sn_field_len_um_e::size12;
  rlc_cfg_asn1.um_bi_dir().dl_um_rlc.t_reassembly         = asn1::rrc_nr::t_reassembly_e::ms50;
  rlc_cfg_asn1.um_bi_dir().ul_um_rlc.sn_field_len_present = true;
  rlc_cfg_asn1.um_bi_dir().ul_um_rlc.sn_field_len         = asn1::rrc_nr::sn_field_len_um_e::size12;
  asn1::json_writer jw;
  rlc_cfg_asn1.to_json(jw);
  logmap::get("RRC")->info_long("RLC NR Config: \n %s \n", jw.to_string().c_str());

  rlc_config_t rlc_cfg = make_rlc_config_t(rlc_cfg_asn1);
  TESTASSERT(rlc_cfg.rat == srslte_rat_t::nr);
  TESTASSERT(rlc_cfg.um_nr.sn_field_length == rlc_um_nr_sn_size_t::size12bits);
  TESTASSERT(rlc_cfg.um_nr.UM_Window_Size == 2048);
  return SRSLTE_SUCCESS;
}

int main()
{
  srslte::logmap::set_default_log_level(srslte::LOG_LEVEL_DEBUG);

  TESTASSERT(test_rlc_config() == 0);

  printf("Success\n");
  return 0;
}
