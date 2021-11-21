/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

using namespace asn1::rrc_nr;

namespace srsenb {

void test_rrc_nr_connection_establishment(srsran::task_scheduler& task_sched,
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
  TESTASSERT_EQ(rrc_setup_s::crit_exts_c_::types_opts::rrc_setup,
                dl_ccch_msg.msg.c1().rrc_setup().crit_exts.type().value);

  const rrc_setup_ies_s& setup_ies = dl_ccch_msg.msg.c1().rrc_setup().crit_exts.rrc_setup();
  TESTASSERT(setup_ies.radio_bearer_cfg.srb_to_add_mod_list_present);
  TESTASSERT_EQ(1, setup_ies.radio_bearer_cfg.srb_to_add_mod_list.size());

  const srb_to_add_mod_s& srb1 = setup_ies.radio_bearer_cfg.srb_to_add_mod_list[0];
  TESTASSERT_EQ(srsran::srb_to_lcid(srsran::nr_srb::srb1), srb1.srb_id);

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

} // namespace srsenb
