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

#include "rrc_nr_test_helpers.h"
#include "srsran/common/test_common.h"

using namespace asn1::rrc_nr;

namespace srsenb {

void test_rrc_nr_connection_establishment(srsran::task_scheduler& task_sched,
                                          rrc_nr&                 rrc_obj,
                                          rlc_nr_rrc_tester&      rlc,
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
  TESTASSERT(setup_ies.radio_bearer_cfg.srb_to_add_mod_list_present);
  TESTASSERT_EQ(1, setup_ies.radio_bearer_cfg.srb_to_add_mod_list.size());

  const srb_to_add_mod_s& srb1 = setup_ies.radio_bearer_cfg.srb_to_add_mod_list[0];
  TESTASSERT_EQ(srsran::srb_to_lcid(srsran::nr_srb::srb1), srb1.srb_id);

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
  complete_ies.ded_nas_msg.from_string("7E01280E534C337E004109000BF200F110800101347B80802E02F07071002D7E004109000BF200F"
                                       "110800101347B80801001002E02F0702F0201015200F11000006418010174000090530101");

  {
    pdu = srsran::make_byte_buffer();
    asn1::bit_ref bref{pdu->data(), pdu->get_tailroom()};
    TESTASSERT_SUCCESS(ul_dcch_msg.pack(bref));
    pdu->N_bytes = bref.distance_bytes();
  }
  rrc_obj.write_pdu(rnti, 1, std::move(pdu));
}

void test_rrc_nr_security_mode_cmd(srsran::task_scheduler& task_sched,
                                   rrc_nr&                 rrc_obj,
                                   pdcp_nr_rrc_tester&     pdcp,
                                   uint16_t                rnti)
{
  srsran::unique_byte_buffer_t pdu;

  // Test whether there exists the SRB1 initiated in the Connection Establishment
  // We test this as the SRB1 was setup in a different function
  TESTASSERT_EQ(rnti, pdcp.last_sdu_rnti);
  TESTASSERT_EQ(srsran::srb_to_lcid(srsran::nr_srb::srb1), 1);

  // Send SecurityModeCommand (gNB -> UE)
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

  // Send SecurityModeComplete (UE -> gNB)
  ul_dcch_msg_s ul_dcch_msg;
  auto&         sec_cmd_complete_msg = ul_dcch_msg.msg.set_c1().set_security_mode_complete();
  sec_cmd_complete_msg.rrc_transaction_id = dl_dcch_msg.msg.c1().security_mode_cmd().rrc_transaction_id;
  auto& ies_complete                      = sec_cmd_complete_msg.crit_exts.set_security_mode_complete();

  {
    pdu = srsran::make_byte_buffer();
    asn1::bit_ref bref{pdu->data(), pdu->get_tailroom()};
    TESTASSERT_SUCCESS(ul_dcch_msg.pack(bref));
    pdu->N_bytes = bref.distance_bytes();
  }

  // send message to RRC
  rrc_obj.write_pdu(rnti, 1, std::move(pdu));
}

} // namespace srsenb
