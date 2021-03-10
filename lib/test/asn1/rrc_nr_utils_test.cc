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
  srslog::fetch_basic_logger("RRC").info("RLC NR Config: \n %s", jw.to_string().c_str());

  rlc_config_t rlc_cfg = make_rlc_config_t(rlc_cfg_asn1);
  TESTASSERT(rlc_cfg.rat == srslte_rat_t::nr);
  TESTASSERT(rlc_cfg.um_nr.sn_field_length == rlc_um_nr_sn_size_t::size12bits);
  TESTASSERT(rlc_cfg.um_nr.UM_Window_Size == 2048);
  return SRSLTE_SUCCESS;
}

int test_mac_rach_common_config()
{
  asn1::rrc_nr::rach_cfg_common_s rach_common_config_asn1;
  rach_common_config_asn1.ra_contention_resolution_timer =
      asn1::rrc_nr::rach_cfg_common_s::ra_contention_resolution_timer_opts::sf64;
  rach_common_config_asn1.rach_cfg_generic.ra_resp_win   = asn1::rrc_nr::rach_cfg_generic_s::ra_resp_win_opts::sl10;
  rach_common_config_asn1.rach_cfg_generic.prach_cfg_idx = 160;
  rach_common_config_asn1.rach_cfg_generic.preamb_rx_target_pwr = -110;
  rach_common_config_asn1.rach_cfg_generic.pwr_ramp_step = asn1::rrc_nr::rach_cfg_generic_s::pwr_ramp_step_opts::db4;
  rach_common_config_asn1.rach_cfg_generic.preamb_trans_max =
      asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n7;

  asn1::json_writer jw;
  rach_common_config_asn1.to_json(jw);
  srslog::fetch_basic_logger("RRC").info("MAC NR RACH Common config: \n %s", jw.to_string().c_str());

  rach_nr_cfg_t rach_nr_cfg = make_mac_rach_cfg(rach_common_config_asn1);
  TESTASSERT(rach_nr_cfg.ra_responseWindow == 10);
  TESTASSERT(rach_nr_cfg.ra_ContentionResolutionTimer == 64);
  TESTASSERT(rach_nr_cfg.prach_ConfigurationIndex == 160);
  TESTASSERT(rach_nr_cfg.PreambleReceivedTargetPower == -110);
  TESTASSERT(rach_nr_cfg.preambleTransMax == 7);
  TESTASSERT(rach_nr_cfg.powerRampingStep == 4);
  return SRSLTE_SUCCESS;
}

int main()
{
  auto& asn1_logger = srslog::fetch_basic_logger("ASN1", false);
  asn1_logger.set_level(srslog::basic_levels::debug);
  asn1_logger.set_hex_dump_max_size(-1);
  auto& rrc_logger = srslog::fetch_basic_logger("RRC", false);
  rrc_logger.set_level(srslog::basic_levels::debug);
  rrc_logger.set_hex_dump_max_size(-1);

  // Start the log backend.
  srslog::init();

  TESTASSERT(test_rlc_config() == SRSLTE_SUCCESS);
  TESTASSERT(test_mac_rach_common_config() == SRSLTE_SUCCESS);

  srslog::flush();

  printf("Success\n");
  return 0;
}
