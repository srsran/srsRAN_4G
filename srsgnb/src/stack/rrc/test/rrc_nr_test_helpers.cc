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

int test_rrc_nr_connection_establishment(srsran::task_scheduler& task_sched,
                                         rrc_nr&                 rrc_obj,
                                         rlc_nr_rrc_tester&      rlc,
                                         uint16_t                rnti)
{
  srsran::unique_byte_buffer_t pdu;

  // Send RRCSetupRequest
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

  rrc_obj.write_pdu(rnti, 0, std::move(pdu));
  task_sched.tic();

  TESTASSERT_EQ(rnti, rlc.last_sdu_rnti);
  TESTASSERT_EQ(srsran::srb_to_lcid(srsran::nr_srb::srb0), rlc.last_sdu_lcid);
  TESTASSERT(rlc.last_sdu->size() > 0);
  dl_ccch_msg_s dl_ccch_msg;
  {
    asn1::cbit_ref bref{rlc.last_sdu->data(), rlc.last_sdu->size()};
    TESTASSERT_SUCCESS(dl_ccch_msg.unpack(bref));
  }
  TESTASSERT_EQ(dl_ccch_msg_type_c::types_opts::c1, dl_ccch_msg.msg.type().value);
  TESTASSERT_EQ(dl_ccch_msg_type_c::c1_c_::types_opts::rrc_setup, dl_ccch_msg.msg.c1().type().value);
  TESTASSERT_EQ(asn1::rrc_nr::rrc_setup_s::crit_exts_c_::types_opts::rrc_setup,
                dl_ccch_msg.msg.c1().rrc_setup().crit_exts.type().value);

  const rrc_setup_ies_s& setup_ies = dl_ccch_msg.msg.c1().rrc_setup().crit_exts.rrc_setup();
  TESTASSERT(setup_ies.radio_bearer_cfg.srb_to_add_mod_list_present);
  TESTASSERT_EQ(1, setup_ies.radio_bearer_cfg.srb_to_add_mod_list.size());

  const srb_to_add_mod_s& srb1 = setup_ies.radio_bearer_cfg.srb_to_add_mod_list[0];
  TESTASSERT_EQ(srsran::srb_to_lcid(srsran::nr_srb::srb1), srb1.srb_id);

  return SRSRAN_SUCCESS;
}

} // namespace srsenb
