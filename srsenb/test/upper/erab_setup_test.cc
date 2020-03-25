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

#include "srsenb/hdr/enb.h"
#include "srsenb/src/enb_cfg_parser.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/test_common.h"
#include "test_helpers.h"
#include <iostream>

int test_erab_setup(bool qci_exists)
{
  printf("\n===== TEST: test_erab_setup()  =====\n");
  srslte::scoped_log<srslte::test_log_filter> rrc_log("RRC ");
  srslte::timer_handler                       timers;
  srslte::unique_byte_buffer_t                pdu;

  srsenb::all_args_t args;
  rrc_cfg_t          cfg;
  TESTASSERT(test_helpers::parse_default_cfg(&cfg, args) == SRSLTE_SUCCESS);

  srsenb::rrc                       rrc;
  mac_dummy                         mac;
  rlc_dummy                         rlc;
  test_dummies::pdcp_mobility_dummy pdcp;
  phy_dummy                         phy;
  test_dummies::s1ap_mobility_dummy s1ap;
  gtpu_dummy                        gtpu;
  rrc_log->set_level(srslte::LOG_LEVEL_INFO);
  rrc_log->set_hex_limit(1024);
  rrc.init(cfg, &phy, &mac, &rlc, &pdcp, &s1ap, &gtpu, &timers);

  auto tic = [&timers, &rrc] {
    timers.step_all();
    rrc.tti_clock();
  };

  uint16_t                  rnti = 0x46;
  sched_interface::ue_cfg_t ue_cfg;
  ue_cfg.supported_cc_list.resize(1);
  ue_cfg.supported_cc_list[0].active     = true;
  ue_cfg.supported_cc_list[0].enb_cc_idx = 0;
  rrc.add_user(rnti, ue_cfg);

  rrc_log->set_level(srslte::LOG_LEVEL_NONE); // mute all the startup log

  // Do all the handshaking until the first RRC Connection Reconf
  test_helpers::bring_rrc_to_reconf_state(rrc, timers, rnti);

  rrc_log->set_level(srslte::LOG_LEVEL_DEBUG);
  rrc_log->set_hex_limit(1024);

  // MME sends 2nd ERAB Setup request for DRB2 (QCI exists in config)
  uint8_t drb2_erab_setup_request_ok[] = {
      0x00, 0x05, 0x00, 0x66, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x08, 0x00, 0x02, 0x00,
      0x02, 0x00, 0x10, 0x00, 0x53, 0x00, 0x00, 0x11, 0x00, 0x4e, 0x0c, 0x00, 0x09, 0x21, 0x0f, 0x80, 0x7f, 0x00,
      0x00, 0x02, 0x00, 0x00, 0x00, 0x13, 0x3f, 0x27, 0x67, 0x90, 0x99, 0xf5, 0x05, 0x62, 0x02, 0xc1, 0x01, 0x09,
      0x09, 0x08, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x05, 0x01, 0x2d, 0x2d, 0x00, 0x0b, 0x27, 0x22,
      0x80, 0x80, 0x21, 0x10, 0x02, 0x00, 0x00, 0x10, 0x81, 0x06, 0x08, 0x08, 0x08, 0x08, 0x83, 0x06, 0x08, 0x08,
      0x04, 0x04, 0x00, 0x0d, 0x04, 0x08, 0x08, 0x08, 0x08, 0x00, 0x0d, 0x04, 0x08, 0x08, 0x04, 0x04};

  // QCI doesn't exist (in default eNB DRB config)
  uint8_t drb2_erab_setup_request_fail[] = {
      0x00, 0x05, 0x00, 0x75, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x64, 0x00, 0x08, 0x00, 0x02, 0x00,
      0x01, 0x00, 0x42, 0x00, 0x0a, 0x18, 0x3b, 0x9a, 0xca, 0x00, 0x60, 0x3b, 0x9a, 0xca, 0x00, 0x00, 0x10, 0x00,
      0x54, 0x00, 0x00, 0x11, 0x00, 0x4f, 0x0c, 0x00, 0x05, 0x3c, 0x0f, 0x80, 0x7f, 0x00, 0x01, 0x64, 0xb3, 0xce,
      0xf1, 0xc9, 0x40, 0x27, 0xfe, 0x2a, 0x3b, 0xd1, 0x03, 0x62, 0x70, 0xc1, 0x01, 0x05, 0x17, 0x03, 0x69, 0x6d,
      0x73, 0x06, 0x6d, 0x6e, 0x63, 0x30, 0x37, 0x30, 0x06, 0x6d, 0x63, 0x63, 0x39, 0x30, 0x31, 0x04, 0x67, 0x70,
      0x72, 0x73, 0x05, 0x01, 0xc0, 0xa8, 0x04, 0x02, 0x27, 0x15, 0x80, 0x80, 0x21, 0x0a, 0x03, 0x00, 0x00, 0x0a,
      0x81, 0x06, 0x08, 0x08, 0x08, 0x08, 0x00, 0x0d, 0x04, 0x08, 0x08, 0x08, 0x08};

  asn1::s1ap::s1ap_pdu_c s1ap_pdu;
  srslte::byte_buffer_t  byte_buf;
  if (qci_exists) {
    byte_buf.N_bytes = sizeof(drb2_erab_setup_request_ok);
    memcpy(byte_buf.msg, drb2_erab_setup_request_ok, byte_buf.N_bytes);
  } else {
    byte_buf.N_bytes = sizeof(drb2_erab_setup_request_fail);
    memcpy(byte_buf.msg, drb2_erab_setup_request_fail, byte_buf.N_bytes);
  }
  asn1::cbit_ref bref(byte_buf.msg, byte_buf.N_bytes);

  TESTASSERT(s1ap_pdu.unpack(bref) == asn1::SRSASN_SUCCESS);
  rrc.setup_ue_erabs(rnti, s1ap_pdu.init_msg().value.erab_setup_request());

  if (qci_exists) {
    TESTASSERT(rrc_log->error_counter == 0);
  } else {
    TESTASSERT(rrc_log->error_counter == 2);
  }

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  srslte::logmap::set_default_log_level(srslte::LOG_LEVEL_INFO);

  if (argc < 3) {
    argparse::usage(argv[0]);
    return -1;
  }
  argparse::parse_args(argc, argv);
  TESTASSERT(test_erab_setup(true) == SRSLTE_SUCCESS);
  TESTASSERT(test_erab_setup(false) == SRSLTE_SUCCESS);

  printf("\nSuccess\n");

  srslte::byte_buffer_pool::get_instance()->cleanup();

  return SRSLTE_SUCCESS;
}
