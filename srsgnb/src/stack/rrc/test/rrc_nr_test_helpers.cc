/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
#include "srsran/common/test_common.h"
#include <string>

#define NAS_SEC_CMD_STR "d9119b97d7bb59fc842d5b9cc12f00c27e9d5e4c80ee4cceb99a0dbbc6e0b54daa21a5d9e36d2e3b"

using namespace asn1::rrc_nr;

namespace srsenb {

void test_rrc_nr_connection_establishment(srsran::task_scheduler& task_sched,
                                          rrc_nr&                 rrc_obj,
                                          rlc_nr_rrc_tester&      rlc,
                                          mac_nr_dummy&           mac,
                                          ngap_rrc_tester&        ngap,
                                          uint16_t                rnti)
{
  srsran::unique_byte_buffer_t pdu;

  // Step 1 - Send RRCSetupRequest (UE -> gNB)
  ul_ccch_msg_s            setup_msg;
  rrc_setup_request_ies_s& setup  = setup_msg.msg.set_c1().set_rrc_setup_request().rrc_setup_request;
  setup.establishment_cause.value = establishment_cause_opts::mo_data;
  setup.ue_id.set_random_value().from_number(0);
  {
    pdu = srsran::make_byte_buffer();
    asn1::bit_ref bref{pdu->data(), pdu->get_tailroom()};
    TESTASSERT_SUCCESS(setup_msg.pack(bref));
    pdu->N_bytes = bref.distance_bytes();
  }

  // Pass message to RRC
  rrc_obj.write_pdu(rnti, 0, std::move(pdu));
  task_sched.tic();

  // Step 2 - RRCSetup (gNB -> UE)
  // The response to RRCSetupRequest has been sent by RRC - check if this message (RRCSetup) is correct
  TESTASSERT_EQ(rnti, rlc.last_sdu_rnti);
  TESTASSERT_EQ(srsran::srb_to_lcid(srsran::nr_srb::srb0), rlc.last_sdu_lcid);
  TESTASSERT(rlc.last_sdu->size() > 0);

  // dl_ccch_msg will store the unpacked RRCSetup msg sent by the RRC
  dl_ccch_msg_s dl_ccch_msg;
  {
    asn1::cbit_ref bref{rlc.last_sdu->data(), rlc.last_sdu->size()};
    TESTASSERT_SUCCESS(dl_ccch_msg.unpack(bref));
  }
  // Test if the RRC sent the correct RRCSetup msg
  TESTASSERT_EQ(dl_ccch_msg_type_c::types_opts::c1, dl_ccch_msg.msg.type().value);
  TESTASSERT_EQ(dl_ccch_msg_type_c::c1_c_::types_opts::rrc_setup, dl_ccch_msg.msg.c1().type().value);
  TESTASSERT_EQ(rrc_setup_s::crit_exts_c_::types_opts::rrc_setup,
                dl_ccch_msg.msg.c1().rrc_setup().crit_exts.type().value);

  const rrc_setup_ies_s& setup_ies = dl_ccch_msg.msg.c1().rrc_setup().crit_exts.rrc_setup();
  TESTASSERT(setup_ies.radio_bearer_cfg.srb_to_add_mod_list.size() > 0);
  TESTASSERT_EQ(1, setup_ies.radio_bearer_cfg.srb_to_add_mod_list.size());

  const srb_to_add_mod_s& srb1 = setup_ies.radio_bearer_cfg.srb_to_add_mod_list[0];
  TESTASSERT_EQ(srsran::srb_to_lcid(srsran::nr_srb::srb1), srb1.srb_id);
  // Test UE context in MAC
  TESTASSERT_EQ(rnti, mac.last_ue_cfg_rnti);
  // Only LCID=1 is added
  TESTASSERT_EQ(1, mac.last_ue_cfg.lc_ch_to_add.size());
  TESTASSERT_EQ(1, mac.last_ue_cfg.lc_ch_to_add.front().lcid);
  TESTASSERT_EQ(mac_lc_ch_cfg_t::BOTH, mac.last_ue_cfg.lc_ch_to_add.front().cfg.direction);
  bool found_common_ul_dci_format = false;
  for (uint32_t ss_id = 0; ss_id < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; ++ss_id) {
    if (mac.last_ue_cfg.phy_cfg.pdcch.search_space_present[ss_id]) {
      const srsran_search_space_t& ss = mac.last_ue_cfg.phy_cfg.pdcch.search_space[ss_id];
      // Ensure MAC ue_cfg does not have yet any UE-specific SS
      TESTASSERT_NEQ(srsran_search_space_type_ue, ss.type);
      for (uint32_t f = 0; f < ss.nof_formats; ++f) {
        found_common_ul_dci_format |= ss.formats[f] == srsran_dci_format_nr_0_0;
      }
    }
  }
  TESTASSERT(found_common_ul_dci_format);

  // Step 3 - RRCSetupComplete (UE -> gNB) - Configure the msg and send it to RRC
  ul_dcch_msg_s         ul_dcch_msg;
  rrc_setup_complete_s& complete         = ul_dcch_msg.msg.set_c1().set_rrc_setup_complete();
  complete.rrc_transaction_id            = dl_ccch_msg.msg.c1().rrc_setup().rrc_transaction_id;
  rrc_setup_complete_ies_s& complete_ies = complete.crit_exts.set_rrc_setup_complete();
  complete_ies.sel_plmn_id               = 1; // First PLMN in list
  complete_ies.registered_amf_present    = true;
  complete_ies.registered_amf.amf_id.from_number(0x800101);
  complete_ies.guami_type_present = true;
  complete_ies.guami_type.value   = rrc_setup_complete_ies_s::guami_type_opts::native;
  std::string NAS_msg_str = "7E01280E534C337E004109000BF200F110800101347B80802E02F07071002D7E004109000BF200F11080010134"
                            "7B80801001002E02F0702F0201015200F11000006418010174000090530101";
  auto&       ded_nas_msg = complete_ies.ded_nas_msg.from_string(NAS_msg_str);

  {
    pdu = srsran::make_byte_buffer();
    asn1::bit_ref bref{pdu->data(), pdu->get_tailroom()};
    TESTASSERT_SUCCESS(ul_dcch_msg.pack(bref));
    pdu->N_bytes = bref.distance_bytes();
  }
  rrc_obj.write_pdu(rnti, 1, std::move(pdu));

  // Test new UE context in MAC
  bool ss_ue_found = false;
  for (uint32_t ss_id = 0; ss_id < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; ++ss_id) {
    if (mac.last_ue_cfg.phy_cfg.pdcch.search_space_present[ss_id]) {
      const srsran_search_space_t& ss = mac.last_ue_cfg.phy_cfg.pdcch.search_space[ss_id];
      if (ss.type == srsran_search_space_type_ue) {
        ss_ue_found = true;
      }
    }
  }
  TESTASSERT(ss_ue_found); /// Ensure UE-specific SearchSpace was added

  // Check here if the MSG sent to NGAP is correct
  // Create a unbounded_octstring<false> for the expected MSG
  asn1::unbounded_octstring<false> expected;
  expected.from_string(NAS_msg_str);
  TESTASSERT(expected == ngap.last_pdu);
}

void test_rrc_nr_info_transfer(srsran::task_scheduler& task_sched,
                               rrc_nr&                 rrc_obj,
                               pdcp_nr_rrc_tester&     pdcp,
                               ngap_rrc_tester&        ngap,
                               uint16_t                rnti)
{
  // STEP 1 : Send DLInformationTransfer (gNB -> UE)
  // generate sdu to pass as NAS message in DLInformationTransfer
  srsran::unique_byte_buffer_t nsa_sdu;
  nsa_sdu = srsran::make_byte_buffer();

  // create an unbounded_octstring object that contains a random NAS message (we simulate a NAS message)
  asn1::unbounded_octstring<false> NAS_DL_msg;
  NAS_DL_msg.from_string("21d9dfe07800371095c79a751be8352fb44aba7d69b836f5aad594ede9e72b8e34105ca8d7669d5c");
  nsa_sdu->append_bytes(NAS_DL_msg.data(), NAS_DL_msg.size());

  // trigger the RRC to send the DLInformationTransfer
  rrc_obj.write_dl_info(rnti, std::move(nsa_sdu));

  // Test whether there exists the SRB1 initiated in the Connection Establishment
  // We test this as the SRB1 was setup in a different function
  TESTASSERT_EQ(rnti, pdcp.last_sdu_rnti);
  TESTASSERT_EQ(srsran::srb_to_lcid(srsran::nr_srb::srb1), pdcp.last_sdu_lcid);

  // Send SecurityModeCommand (gNB -> UE)
  dl_dcch_msg_s dl_dcch_msg;
  {
    asn1::cbit_ref bref{pdcp.last_sdu->data(), pdcp.last_sdu->size()};
    TESTASSERT_SUCCESS(dl_dcch_msg.unpack(bref));
  }

  // Test if the unpacked message retrived from PCDP is correct
  TESTASSERT_EQ(dl_dcch_msg_type_c::types_opts::c1, dl_dcch_msg.msg.type().value);
  TESTASSERT_EQ(dl_dcch_msg_type_c::c1_c_::types_opts::dl_info_transfer, dl_dcch_msg.msg.c1().type().value);
  TESTASSERT_EQ(dl_info_transfer_s::crit_exts_c_::types_opts::dl_info_transfer,
                dl_dcch_msg.msg.c1().dl_info_transfer().crit_exts.type().value);

  dl_info_transfer_ies_s& ies_DL = dl_dcch_msg.msg.c1().dl_info_transfer().crit_exts.dl_info_transfer();
  TESTASSERT(ies_DL.ded_nas_msg.size() > 0);
  TESTASSERT(NAS_DL_msg == ies_DL.ded_nas_msg);

  // STEP 2: Send ULInformationTransfer (UE -> gNB)
  ul_dcch_msg_s ul_dcch_msg;
  auto&         ies_UL = ul_dcch_msg.msg.set_c1().set_ul_info_transfer().crit_exts.set_ul_info_transfer();

  // Create an unbounded_octstring object that contains a random NAS message (we simulate a NAS message)
  // We reuse ies_UL below to compare the string with the message sent to and unpacked by the gNB
  ies_UL.ded_nas_msg.from_string("6671f8bc80b1860f29b3a8b3b8563ce6c36a591bb1a3dc6612674448fb958d274426d326356aa9aa");

  srsran::unique_byte_buffer_t pdu;
  {
    pdu = srsran::make_byte_buffer();
    asn1::bit_ref bref{pdu->data(), pdu->get_tailroom()};
    TESTASSERT_SUCCESS(ul_dcch_msg.pack(bref));
    pdu->N_bytes = bref.distance_bytes();
  }

  // send message to RRC
  rrc_obj.write_pdu(rnti, 1, std::move(pdu));

  // compare if the actual transmitted matches with the MSG created from the original string
  TESTASSERT(ies_UL.ded_nas_msg == ngap.last_pdu);
}

void test_rrc_nr_security_mode_cmd(srsran::task_scheduler& task_sched,
                                   rrc_nr&                 rrc_obj,
                                   pdcp_nr_rrc_tester&     pdcp,
                                   uint16_t                rnti)
{
  // create an unbounded_octstring object that contains a random NAS message (we simulate a NAS message)
  asn1::unbounded_octstring<false> NAS_msg;
  NAS_msg.from_string(NAS_SEC_CMD_STR);
  srsran::unique_byte_buffer_t nas_pdu;
  nas_pdu = srsran::make_byte_buffer();
  nas_pdu->append_bytes(NAS_msg.data(), NAS_msg.size());

  // Trigger Send SecurityCommand (simulate request from NGAP)
  rrc_obj.start_security_mode_procedure(rnti, std::move(nas_pdu));

  // Test whether there exists the SRB1 initiated in the Connection Establishment
  // We test this as the SRB1 was setup in a different function
  TESTASSERT_EQ(rnti, pdcp.last_sdu_rnti);
  TESTASSERT_EQ(srsran::srb_to_lcid(srsran::nr_srb::srb1), pdcp.last_sdu_lcid);

  // STEP 1 - Send SecurityModeCommand (gNB -> UE)
  dl_dcch_msg_s dl_dcch_msg;
  {
    asn1::cbit_ref bref{pdcp.last_sdu->data(), pdcp.last_sdu->size()};
    TESTASSERT_SUCCESS(dl_dcch_msg.unpack(bref));
  }
  TESTASSERT_EQ(dl_dcch_msg_type_c::types_opts::c1, dl_dcch_msg.msg.type().value);
  TESTASSERT_EQ(dl_dcch_msg_type_c::c1_c_::types_opts::security_mode_cmd, dl_dcch_msg.msg.c1().type().value);
  TESTASSERT_EQ(security_mode_cmd_s::crit_exts_c_::types_opts::security_mode_cmd,
                dl_dcch_msg.msg.c1().security_mode_cmd().crit_exts.type().value);

  security_mode_cmd_ies_s& ies = dl_dcch_msg.msg.c1().security_mode_cmd().crit_exts.security_mode_cmd();
  TESTASSERT_EQ(true, ies.security_cfg_smc.security_algorithm_cfg.integrity_prot_algorithm_present);
  TESTASSERT_EQ(integrity_prot_algorithm_opts::nia0,
                ies.security_cfg_smc.security_algorithm_cfg.integrity_prot_algorithm.value);
  TESTASSERT_EQ(ciphering_algorithm_opts::nea0, ies.security_cfg_smc.security_algorithm_cfg.ciphering_algorithm.value);

  // STEP 2 - Send SecurityModeComplete (UE -> gNB)
  ul_dcch_msg_s ul_dcch_msg;
  auto&         sec_cmd_complete_msg      = ul_dcch_msg.msg.set_c1().set_security_mode_complete();
  sec_cmd_complete_msg.rrc_transaction_id = dl_dcch_msg.msg.c1().security_mode_cmd().rrc_transaction_id;
  auto& ies_complete                      = sec_cmd_complete_msg.crit_exts.set_security_mode_complete();

  srsran::unique_byte_buffer_t pdu;
  {
    pdu = srsran::make_byte_buffer();
    asn1::bit_ref bref{pdu->data(), pdu->get_tailroom()};
    TESTASSERT_SUCCESS(ul_dcch_msg.pack(bref));
    pdu->N_bytes = bref.distance_bytes();
  }

  // send message to RRC
  rrc_obj.write_pdu(rnti, 1, std::move(pdu));
}

void test_rrc_nr_ue_capability_enquiry(srsran::task_scheduler& task_sched,
                                       rrc_nr&                 rrc_obj,
                                       pdcp_nr_rrc_tester&     pdcp,
                                       uint16_t                rnti)
{
  dl_dcch_msg_s dl_dcch_msg;
  {
    asn1::cbit_ref bref{pdcp.last_sdu->data(), pdcp.last_sdu->size()};
    TESTASSERT_SUCCESS(dl_dcch_msg.unpack(bref));
  }

  // Check if unpacked message is correct (ueCapabilityEnquiry | gNB -> UE)
  TESTASSERT_EQ(dl_dcch_msg_type_c::types_opts::c1, dl_dcch_msg.msg.type().value);
  TESTASSERT_EQ(dl_dcch_msg_type_c::c1_c_::types_opts::ue_cap_enquiry, dl_dcch_msg.msg.c1().type().value);
  TESTASSERT_EQ(ue_cap_enquiry_s::crit_exts_c_::types_opts::ue_cap_enquiry,
                dl_dcch_msg.msg.c1().ue_cap_enquiry().crit_exts.type().value);

  // Send response (ueCapabilityInformation | UE -> gNB)
  ul_dcch_msg_s ul_dcch_msg;
  auto&         ue_capability_information      = ul_dcch_msg.msg.set_c1().set_ue_cap_info();
  ue_capability_information.rrc_transaction_id = dl_dcch_msg.msg.c1().ue_cap_enquiry().rrc_transaction_id;
  ue_capability_information.crit_exts.set_ue_cap_info();

  srsran::unique_byte_buffer_t pdu;
  {
    pdu = srsran::make_byte_buffer();
    asn1::bit_ref bref{pdu->data(), pdu->get_tailroom()};
    TESTASSERT_SUCCESS(ul_dcch_msg.pack(bref));
    pdu->N_bytes = bref.distance_bytes();
  }

  // send message to RRC
  rrc_obj.write_pdu(rnti, 1, std::move(pdu));
}

void test_rrc_nr_reconfiguration(srsran::task_scheduler& task_sched,
                                 rrc_nr&                 rrc_obj,
                                 pdcp_nr_rrc_tester&     pdcp,
                                 ngap_rrc_tester&        ngap,
                                 uint16_t                rnti)
{
  // Test whether there exists the SRB1 initiated in the Connection Establishment
  // We test this as the SRB1 was set up in a different function
  TESTASSERT_EQ(rnti, pdcp.last_sdu_rnti);
  TESTASSERT_EQ(srsran::srb_to_lcid(srsran::nr_srb::srb1), pdcp.last_sdu_lcid);

  dl_dcch_msg_s dl_dcch_msg;
  {
    asn1::cbit_ref bref{pdcp.last_sdu->data(), pdcp.last_sdu->size()};
    TESTASSERT_SUCCESS(dl_dcch_msg.unpack(bref));
  }

  // Test whether the unpacked message is correct
  TESTASSERT_EQ(dl_dcch_msg_type_c::types_opts::c1, dl_dcch_msg.msg.type().value);
  TESTASSERT_EQ(dl_dcch_msg_type_c::c1_c_::types_opts::rrc_recfg, dl_dcch_msg.msg.c1().type().value);
  TESTASSERT_EQ(rrc_recfg_s::crit_exts_c_::types_opts::rrc_recfg,
                dl_dcch_msg.msg.c1().rrc_recfg().crit_exts.type().value);
  const rrc_recfg_ies_s& reconf_ies = dl_dcch_msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg();

  // create an unbounded_octstring object that contains the same NAS message as in SecurityModeCommand
  // The RRCreconfiguration reads the SecurityModeCommand NAS msg previously saved in the queue
  asn1::unbounded_octstring<false> NAS_msg;
  NAS_msg.from_string(NAS_SEC_CMD_STR);
  TESTASSERT(reconf_ies.non_crit_ext.ded_nas_msg_list.size() > 0);
  // Test if NAS_msg is the same as the one sent in SecurityModeCommand
  TESTASSERT(NAS_msg == reconf_ies.non_crit_ext.ded_nas_msg_list[0]);

  // STEP 2 - Send RRCReconfiguration (UE -> gNB)
  ul_dcch_msg_s ul_dcch_msg;
  auto&         RRC_recfg_complete      = ul_dcch_msg.msg.set_c1().set_rrc_recfg_complete();
  RRC_recfg_complete.rrc_transaction_id = dl_dcch_msg.msg.c1().rrc_recfg().rrc_transaction_id;
  RRC_recfg_complete.crit_exts.set_rrc_recfg_complete();

  srsran::unique_byte_buffer_t pdu;
  {
    pdu = srsran::make_byte_buffer();
    asn1::bit_ref bref{pdu->data(), pdu->get_tailroom()};
    TESTASSERT_SUCCESS(ul_dcch_msg.pack(bref));
    pdu->N_bytes = bref.distance_bytes();
  }

  // send message to RRC
  rrc_obj.write_pdu(rnti, 1, std::move(pdu));

  // Verify the NGAP gets notified for the RRCReconfigurationComplete
  TESTASSERT_EQ(true, ngap.last_rrc_recnf_complete);
}

void test_rrc_nr_2nd_reconfiguration(srsran::task_scheduler& task_sched,
                                     rrc_nr&                 rrc_obj,
                                     pdcp_nr_rrc_tester&     pdcp,
                                     ngap_rrc_tester&        ngap,
                                     uint16_t                rnti)
{
  // Make sure the NGAP RRCReconfigurationComplete bool is reset to false
  ngap.last_rrc_recnf_complete = false;

  // create an unbounded_octstring object that contains a NAS message (we simulate a random NAS nas)
  asn1::unbounded_octstring<false> NAS_msg;
  NAS_msg.from_string("c574defc80ba722bffb8eacb6f8a163e3222cf1542ac529f6980bb15e0bf12d9f2b29f11fb458ec9");

  // Test whether there exists the SRB1 initiated in the Connection Establishment
  // We test this as the SRB1 was set up in a different function
  TESTASSERT_EQ(rnti, pdcp.last_sdu_rnti);
  TESTASSERT_EQ(srsran::srb_to_lcid(srsran::nr_srb::srb1), pdcp.last_sdu_lcid);

  // STEP 2 -  Trigger and send RRCReconfiguration command (gNB -> UE)
  rrc_obj.establish_rrc_bearer(rnti, 1, NAS_msg, 4, 9);

  dl_dcch_msg_s dl_dcch_msg;
  {
    asn1::cbit_ref bref{pdcp.last_sdu->data(), pdcp.last_sdu->size()};
    TESTASSERT_SUCCESS(dl_dcch_msg.unpack(bref));
  }

  // Test whether the unpacked message is correct
  TESTASSERT_EQ(dl_dcch_msg_type_c::types_opts::c1, dl_dcch_msg.msg.type().value);
  TESTASSERT_EQ(dl_dcch_msg_type_c::c1_c_::types_opts::rrc_recfg, dl_dcch_msg.msg.c1().type().value);
  TESTASSERT_EQ(rrc_recfg_s::crit_exts_c_::types_opts::rrc_recfg,
                dl_dcch_msg.msg.c1().rrc_recfg().crit_exts.type().value);
  const rrc_recfg_ies_s& reconf_ies = dl_dcch_msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg();
  TESTASSERT_EQ(true, reconf_ies.radio_bearer_cfg_present);
  TESTASSERT(reconf_ies.radio_bearer_cfg.srb_to_add_mod_list.size() > 0);
  TESTASSERT_EQ(1, reconf_ies.radio_bearer_cfg.srb_to_add_mod_list.size());
  TESTASSERT_EQ(2, reconf_ies.radio_bearer_cfg.srb_to_add_mod_list[0].srb_id);
  TESTASSERT_EQ(1, reconf_ies.radio_bearer_cfg.drb_to_add_mod_list.size());
  auto& drb = reconf_ies.radio_bearer_cfg.drb_to_add_mod_list[0];
  TESTASSERT_EQ(1, drb.drb_id);

  TESTASSERT_EQ(true, reconf_ies.non_crit_ext_present);
  TESTASSERT(reconf_ies.non_crit_ext.master_cell_group.size() > 0);
  auto& master_group_msg = reconf_ies.non_crit_ext.master_cell_group;

  cell_group_cfg_s master_cell_group;
  {
    asn1::cbit_ref bref{master_group_msg.data(), master_group_msg.size()};
    TESTASSERT_SUCCESS(master_cell_group.unpack(bref));
  }

  // Test if the master_cell_group SRB and DRB IDs match those in the RadioBearerConfig
  TESTASSERT_EQ(0, master_cell_group.cell_group_id);
  TESTASSERT(master_cell_group.rlc_bearer_to_add_mod_list.size() > 0);
  auto& rlc_srb = master_cell_group.rlc_bearer_to_add_mod_list[0];
  TESTASSERT_EQ(reconf_ies.radio_bearer_cfg.srb_to_add_mod_list[0].srb_id, rlc_srb.served_radio_bearer.srb_id());
  auto& rlc_drb = master_cell_group.rlc_bearer_to_add_mod_list[1];
  TESTASSERT_EQ(reconf_ies.radio_bearer_cfg.drb_to_add_mod_list[0].drb_id, rlc_drb.served_radio_bearer.drb_id());

  // Test if NAS_msg is the same as the one sent in DLInformationTransfer
  TESTASSERT(reconf_ies.non_crit_ext.ded_nas_msg_list.size() > 0);
  TESTASSERT(NAS_msg == reconf_ies.non_crit_ext.ded_nas_msg_list[0]);

  // STEP 2 - Send RRCReconfiguration (UE -> gNB)
  ul_dcch_msg_s ul_dcch_msg;
  auto&         RRC_recfg_complete      = ul_dcch_msg.msg.set_c1().set_rrc_recfg_complete();
  RRC_recfg_complete.rrc_transaction_id = dl_dcch_msg.msg.c1().rrc_recfg().rrc_transaction_id;
  RRC_recfg_complete.crit_exts.set_rrc_recfg_complete();

  srsran::unique_byte_buffer_t pdu;
  {
    pdu = srsran::make_byte_buffer();
    asn1::bit_ref bref{pdu->data(), pdu->get_tailroom()};
    TESTASSERT_SUCCESS(ul_dcch_msg.pack(bref));
    pdu->N_bytes = bref.distance_bytes();
  }

  // send message to RRC
  rrc_obj.write_pdu(rnti, 1, std::move(pdu));

  // Verify the NGAP gets notified for the RRCReconfigurationComplete
  TESTASSERT_EQ(true, ngap.last_rrc_recnf_complete);
}

} // namespace srsenb
