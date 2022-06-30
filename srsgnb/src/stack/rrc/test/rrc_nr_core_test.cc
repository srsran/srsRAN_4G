/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "rrc_nr_test_helpers.h"
#include "srsenb/hdr/enb.h"
#include "srsenb/hdr/stack/upper/gtpu.h"
#include "srsenb/test/rrc/test_helpers.h"
#include "srsgnb/hdr/stack/ngap/ngap.h"
#include "srsgnb/hdr/stack/rrc/rrc_nr_config_utils.h"
#include "srsgnb/src/stack/mac/test/sched_nr_cfg_generators.h"
#include "srsran/common/network_utils.h"
#include "srsran/common/test_common.h"
#include "srsran/interfaces/gnb_rrc_nr_interfaces.h"
#include "srsran/upper/gtpu.h"
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

#include <iostream>

using namespace asn1::rrc_nr;
namespace bpo = boost::program_options;

namespace srsenb { // namespace srsenb

void parse_args(ngap_args_t* ngap_args, int argc, char* argv[])
{
  // temporary helpers for conversion
  std::string config_file;
  std::string gnb_id{};
  std::string gnb_cell_id{};
  std::string gnb_tac{};
  std::string gnb_mcc{};
  std::string gnb_mnc{};

  // Command line only options
  bpo::options_description general("General options");

  general.add_options()("help,h", "Produce help message");

  // Command line or config file options
  bpo::options_description common("Configuration options");

  // clang-format off
  common.add_options()
    
    ("gnb_id",        bpo::value<std::string>(&gnb_id)->default_value("0x0"),                                "gnb ID")
    ("name",          bpo::value<std::string>(&ngap_args->gnb_name)->default_value("srsgnb01"),          "gnb Name")
    ("cell_id",       bpo::value<std::string>(&gnb_cell_id)->default_value("0x0"),                               "Cell ID")
    ("tac",           bpo::value<std::string>(&gnb_tac)->default_value("0x0"),                                   "Tracking Area Code")
    ("mcc",           bpo::value<std::string>(&gnb_mcc)->default_value("001"),                                   "Mobile Country Code")
    ("mnc",           bpo::value<std::string>(&gnb_mnc)->default_value("01"),                                    "Mobile Network Code")
    ("amf_addr",      bpo::value<std::string>(&ngap_args->amf_addr)->default_value("127.0.0.1"),         "IP address of AMF for NG connection")
    ("n1c_bind_addr", bpo::value<std::string>(&ngap_args->ngc_bind_addr)->default_value("192.168.3.1"),  "Local IP address to bind for NGAP connection")
    ("gtp_bind_addr", bpo::value<std::string>(&ngap_args->gtp_bind_addr)->default_value("192.168.3.1"),       "Local IP address to bind for GTP connection");

  bpo::options_description position("Positional options");

  // clang-format on
  bpo::positional_options_description p{};
  p.add("config_file", -1);

  bpo::options_description cmdline_options;
  cmdline_options.add(common).add(position).add(general);

  bpo::variables_map vm{};

  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }

  if (vm.count("help")) {
    std::cout << common << std::endl << general << std::endl;
    exit(0);
  }

  // Convert hex strings
  {
    std::stringstream sstr{};
    sstr << std::hex << vm["gnb_id"].as<std::string>();
    sstr >> ngap_args->gnb_id;
  }
  {
    std::stringstream sstr{};
    sstr << std::hex << vm["cell_id"].as<std::string>();
    uint16_t tmp; // Need intermediate uint16_t as uint8_t is treated as char
    sstr >> tmp;
    ngap_args->cell_id = tmp;
  }
  {
    std::stringstream sstr{};
    sstr << std::hex << vm["tac"].as<std::string>();
    sstr >> ngap_args->tac;
  }

  // Convert MCC/MNC strings
  if (!srsran::string_to_mcc(gnb_mcc, &ngap_args->mcc)) {
    std::cout << "Error parsing mcc:" << gnb_mcc << " - must be a 3-digit string." << std::endl;
  }
  if (!srsran::string_to_mnc(gnb_mnc, &ngap_args->mnc)) {
    std::cout << "Error parsing mnc:" << gnb_mnc << " - must be a 2 or 3-digit string." << std::endl;
  }
}

void test_rrc_sa_ngap_integration(ngap_args_t ngap_args)
{
  // This takes the existing RRC-NR tests and exercises the NGAP integration with a real core network.
  // The test currently runs down untill the RRCReconfiguration.
  srsran::task_scheduler task_sched;

  phy_nr_dummy       phy_obj;
  mac_nr_dummy       mac_obj;
  rlc_nr_rrc_tester  rlc_obj;
  pdcp_nr_rrc_tester pdcp_obj;
  rrc_nr             rrc_obj(&task_sched);
  enb_bearer_manager bearer_mapper;

  // NGAP Setup
  auto& ngap_logger = srslog::fetch_basic_logger("NGAP");
  ngap_logger.set_level(srslog::basic_levels::debug);
  ngap_logger.set_hex_dump_max_size(-1);
  auto& gtpu_logger = srslog::fetch_basic_logger("GTPU");
  gtpu_logger.set_level(srslog::basic_levels::debug);
  gtpu_logger.set_hex_dump_max_size(-1);

  srsran::socket_manager rx_sockets;
  srsenb::ngap           ngap_obj(&task_sched, ngap_logger, &rx_sockets);
  srsenb::gtpu           gtpu_obj(&task_sched, gtpu_logger, &rx_sockets);

  gtpu_args_t gtpu_args;
  gtpu_args.embms_enable  = false;
  gtpu_args.mme_addr      = ngap_args.amf_addr;
  gtpu_args.gtp_bind_addr = ngap_args.gtp_bind_addr;
  TESTASSERT(gtpu_obj.init(gtpu_args, &pdcp_obj) == SRSRAN_SUCCESS);
  TESTASSERT(ngap_obj.init(ngap_args, &rrc_obj, &gtpu_obj) == SRSRAN_SUCCESS);
  task_sched.run_next_task();

  // set cfg
  rrc_nr_cfg_t rrc_cfg_nr = rrc_nr_cfg_t{};
  rrc_cfg_nr.cell_list.emplace_back();
  generate_default_nr_cell(rrc_cfg_nr.cell_list[0]);
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.pci     = 500;
  rrc_cfg_nr.cell_list[0].dl_arfcn                 = 368500;
  rrc_cfg_nr.cell_list[0].band                     = 3;
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.nof_prb = 52;
  rrc_cfg_nr.cell_list[0].duplex_mode              = SRSRAN_DUPLEX_MODE_FDD;
  rrc_cfg_nr.is_standalone                         = true;
  set_derived_nr_cell_params(rrc_cfg_nr.is_standalone, rrc_cfg_nr.cell_list[0]);

  TESTASSERT(
      rrc_obj.init(rrc_cfg_nr, &phy_obj, &mac_obj, &rlc_obj, &pdcp_obj, &ngap_obj, &gtpu_obj, bearer_mapper, nullptr) ==
      SRSRAN_SUCCESS);

  TESTASSERT_SUCCESS(rrc_obj.add_user(0x4601, 0));

  // RRCSetupComplete triggers NGAP Initial UE Message with NAS-PDU: Registration Request
  ngap_rrc_tester ngap_dummy;
  test_rrc_nr_connection_establishment(task_sched, rrc_obj, rlc_obj, mac_obj, ngap_dummy, 0x4601);
  task_sched.run_next_task();

  // ULInformationTransfer -> UplinkNASTransport(NAS Authentication Response)
  srsran::unique_byte_buffer_t auth_resp_pdu;
  auth_resp_pdu = srsran::make_byte_buffer();
  asn1::bit_ref bref_ar{auth_resp_pdu->data(), auth_resp_pdu->get_tailroom()};
  ul_dcch_msg_s ul_dcch_msg_auth_resp;

  ul_dcch_msg_auth_resp.msg.set_c1().set_ul_info_transfer().crit_exts.set_ul_info_transfer();
  ul_dcch_msg_auth_resp.msg.c1().ul_info_transfer().crit_exts.ul_info_transfer().ded_nas_msg.from_string(
      "7e00572d10db165fffdb7b74c326e3fc3f154117fe");

  TESTASSERT_SUCCESS(ul_dcch_msg_auth_resp.pack(bref_ar));
  auth_resp_pdu->N_bytes = bref_ar.distance_bytes();
  rrc_obj.write_pdu(0x4601, 1, std::move(auth_resp_pdu));
  task_sched.run_next_task();

  // ULInformationTransfer -> UplinkNASTransport(NAS Security Mode Complete)
  srsran::unique_byte_buffer_t sec_complete_pdu;
  sec_complete_pdu = srsran::make_byte_buffer();
  asn1::bit_ref bref_smc{sec_complete_pdu->data(), sec_complete_pdu->get_tailroom()};
  ul_dcch_msg_s ul_dcch_msg_smc;
  ul_dcch_msg_smc.msg.set_c1().set_ul_info_transfer().crit_exts.set_ul_info_transfer();
  ul_dcch_msg_smc.msg.c1().ul_info_transfer().crit_exts.ul_info_transfer().ded_nas_msg.from_string(
      "7e046b3737af017e005e7700093535940096783351f37100237e004179000d0100f11000000000103254760810030000002e02e0602f0201"
      "01530100");
  TESTASSERT_SUCCESS(ul_dcch_msg_smc.pack(bref_smc));
  sec_complete_pdu->N_bytes = bref_smc.distance_bytes();
  rrc_obj.write_pdu(0x4601, 1, std::move(sec_complete_pdu));
  task_sched.run_next_task();

  test_rrc_nr_security_mode_cmd(task_sched, rrc_obj, pdcp_obj, 0x4601);
}
} // namespace srsenb

int main(int argc, char** argv)
{
  auto& logger = srslog::fetch_basic_logger("ASN1");
  logger.set_level(srslog::basic_levels::info);
  auto& rrc_logger = srslog::fetch_basic_logger("RRC-NR");
  rrc_logger.set_level(srslog::basic_levels::debug);

  srslog::init();
  ngap_args_t ngap_args = {};
  srsenb::parse_args(&ngap_args, argc, argv);
  srsenb::test_rrc_sa_ngap_integration(ngap_args);

  return SRSRAN_SUCCESS;
}