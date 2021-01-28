/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
