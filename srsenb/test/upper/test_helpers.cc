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

#include "test_helpers.h"
#include "srsenb/hdr/enb.h"
#include "srsran/common/test_common.h"

namespace argparse {

std::string          repository_dir;
srslog::basic_levels log_level;

} // namespace argparse

namespace test_helpers {

int parse_default_cfg_phy(rrc_cfg_t* rrc_cfg, phy_cfg_t* phy_cfg, srsenb::all_args_t& args)
{
  *rrc_cfg                  = {};
  args.enb_files.sib_config = argparse::repository_dir + "/sib.conf.example";
  args.enb_files.rr_config  = argparse::repository_dir + "/rr.conf.example";
  args.enb_files.drb_config = argparse::repository_dir + "/drb.conf.example";
  srslog::fetch_basic_logger("TEST").debug("sib file path=%s", args.enb_files.sib_config.c_str());

  args.enb.enb_id = 0x19B;
  TESTASSERT(srsran::string_to_mcc("001", &args.stack.s1ap.mcc));
  TESTASSERT(srsran::string_to_mnc("01", &args.stack.s1ap.mnc));
  args.general.eia_pref_list = "EIA2, EIA1, EIA0";
  args.general.eea_pref_list = "EEA0, EEA2, EEA1";

  args.general.rrc_inactivity_timer = 60000;

  return enb_conf_sections::parse_cfg_files(&args, rrc_cfg, phy_cfg);
}

int parse_default_cfg(rrc_cfg_t* rrc_cfg, srsenb::all_args_t& args)
{
  args                      = {};
  *rrc_cfg                  = {};
  args.enb_files.sib_config = argparse::repository_dir + "/sib.conf.example";
  args.enb_files.rr_config  = argparse::repository_dir + "/rr.conf.example";
  args.enb_files.drb_config = argparse::repository_dir + "/drb.conf.example";
  srslog::fetch_basic_logger("TEST").debug("sib file path=%s", args.enb_files.sib_config.c_str());

  args.enb.enb_id    = 0x19B;
  args.enb.dl_earfcn = 3400;
  args.enb.n_prb     = 50;
  TESTASSERT(srsran::string_to_mcc("001", &args.stack.s1ap.mcc));
  TESTASSERT(srsran::string_to_mnc("01", &args.stack.s1ap.mnc));
  args.enb.transmission_mode = 1;
  args.enb.nof_ports         = 1;
  args.general.eia_pref_list = "EIA2, EIA1, EIA0";
  args.general.eea_pref_list = "EEA0, EEA2, EEA1";
  args.stack.mac.max_nof_ues = 2;

  args.general.rrc_inactivity_timer = 60000;

  phy_cfg_t phy_cfg;

  return enb_conf_sections::parse_cfg_files(&args, rrc_cfg, &phy_cfg);
}

int bring_rrc_to_reconf_state(srsenb::rrc& rrc, srsran::timer_handler& timers, uint16_t rnti)
{
  srsran::unique_byte_buffer_t pdu;

  // Send RRCConnectionRequest
  uint8_t rrc_conn_request[] = {0x40, 0x12, 0xf6, 0xfb, 0xe2, 0xc6};
  copy_msg_to_buffer(pdu, rrc_conn_request);
  rrc.write_pdu(rnti, 0, std::move(pdu));
  timers.step_all();
  rrc.tti_clock();

  // Send RRCConnectionSetupComplete
  uint8_t rrc_conn_setup_complete[] = {0x20, 0x00, 0x40, 0x2e, 0x90, 0x50, 0x49, 0xe8, 0x06, 0x0e, 0x82, 0xa2,
                                       0x17, 0xec, 0x13, 0xe2, 0x0f, 0x00, 0x02, 0x02, 0x5e, 0xdf, 0x7c, 0x58,
                                       0x05, 0xc0, 0xc0, 0x00, 0x08, 0x04, 0x03, 0xa0, 0x23, 0x23, 0xc0};
  copy_msg_to_buffer(pdu, rrc_conn_setup_complete);
  rrc.write_pdu(rnti, 1, std::move(pdu));
  timers.step_all();
  rrc.tti_clock();

  // S1AP receives InitialContextSetupRequest and forwards it to RRC
  uint8_t s1ap_init_ctxt_setup_req[] = {
      0x00, 0x09, 0x00, 0x80, 0xc6, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x64, 0x00, 0x08, 0x00, 0x02, 0x00,
      0x01, 0x00, 0x42, 0x00, 0x0a, 0x18, 0x3b, 0x9a, 0xca, 0x00, 0x60, 0x3b, 0x9a, 0xca, 0x00, 0x00, 0x18, 0x00, 0x78,
      0x00, 0x00, 0x34, 0x00, 0x73, 0x45, 0x00, 0x09, 0x3c, 0x0f, 0x80, 0x0a, 0x00, 0x21, 0xf0, 0xb7, 0x36, 0x1c, 0x56,
      0x64, 0x27, 0x3e, 0x5b, 0x04, 0xb7, 0x02, 0x07, 0x42, 0x02, 0x3e, 0x06, 0x00, 0x09, 0xf1, 0x07, 0x00, 0x07, 0x00,
      0x37, 0x52, 0x66, 0xc1, 0x01, 0x09, 0x1b, 0x07, 0x74, 0x65, 0x73, 0x74, 0x31, 0x32, 0x33, 0x06, 0x6d, 0x6e, 0x63,
      0x30, 0x37, 0x30, 0x06, 0x6d, 0x63, 0x63, 0x39, 0x30, 0x31, 0x04, 0x67, 0x70, 0x72, 0x73, 0x05, 0x01, 0xc0, 0xa8,
      0x03, 0x02, 0x27, 0x0e, 0x80, 0x80, 0x21, 0x0a, 0x03, 0x00, 0x00, 0x0a, 0x81, 0x06, 0x08, 0x08, 0x08, 0x08, 0x50,
      0x0b, 0xf6, 0x09, 0xf1, 0x07, 0x80, 0x01, 0x01, 0xf6, 0x7e, 0x72, 0x69, 0x13, 0x09, 0xf1, 0x07, 0x00, 0x01, 0x23,
      0x05, 0xf4, 0xf6, 0x7e, 0x72, 0x69, 0x00, 0x6b, 0x00, 0x05, 0x18, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x49, 0x00, 0x20,
      0x45, 0x25, 0xe4, 0x9a, 0x77, 0xc8, 0xd5, 0xcf, 0x26, 0x33, 0x63, 0xeb, 0x5b, 0xb9, 0xc3, 0x43, 0x9b, 0x9e, 0xb3,
      0x86, 0x1f, 0xa8, 0xa7, 0xcf, 0x43, 0x54, 0x07, 0xae, 0x42, 0x2b, 0x63, 0xb9};
  asn1::s1ap::s1ap_pdu_c s1ap_pdu;
  srsran::byte_buffer_t  byte_buf;
  byte_buf.N_bytes = sizeof(s1ap_init_ctxt_setup_req);
  memcpy(byte_buf.msg, s1ap_init_ctxt_setup_req, byte_buf.N_bytes);
  asn1::cbit_ref bref(byte_buf.msg, byte_buf.N_bytes);
  TESTASSERT(s1ap_pdu.unpack(bref) == asn1::SRSASN_SUCCESS);
  rrc.setup_ue_ctxt(rnti, s1ap_pdu.init_msg().value.init_context_setup_request());
  for (auto& item :
       s1ap_pdu.init_msg().value.init_context_setup_request().protocol_ies.erab_to_be_setup_list_ctxt_su_req.value) {
    const auto&         erab = item.value.erab_to_be_setup_item_ctxt_su_req();
    asn1::s1ap::cause_c cause;
    TESTASSERT(rrc.setup_erab(rnti,
                              erab.erab_id,
                              erab.erab_level_qos_params,
                              erab.nas_pdu,
                              erab.transport_layer_address,
                              erab.gtp_teid.to_number(),
                              cause) == SRSRAN_SUCCESS);
  }
  timers.step_all();
  rrc.tti_clock();

  // Send SecurityModeComplete
  uint8_t sec_mode_complete[] = {0x28, 0x00};
  copy_msg_to_buffer(pdu, sec_mode_complete);
  rrc.write_pdu(rnti, 1, std::move(pdu));
  timers.step_all();
  rrc.tti_clock();

  // send UE cap info
  uint8_t ue_cap_info[] = {0x38, 0x01, 0x01, 0x0c, 0x98, 0x00, 0x00, 0x18, 0x00, 0x0f,
                           0x30, 0x20, 0x80, 0x00, 0x01, 0x00, 0x0e, 0x01, 0x00, 0x00};
  copy_msg_to_buffer(pdu, ue_cap_info);
  rrc.write_pdu(rnti, 1, std::move(pdu));
  timers.step_all();
  rrc.tti_clock();

  // RRCConnectionReconfiguration was sent. Send RRCConnectionReconfigurationComplete
  uint8_t rrc_conn_reconf_complete[] = {0x10, 0x00};
  copy_msg_to_buffer(pdu, rrc_conn_reconf_complete);
  rrc.write_pdu(rnti, 1, std::move(pdu));
  timers.step_all();
  rrc.tti_clock();

  return SRSRAN_SUCCESS;
}

} // namespace test_helpers

namespace srsenb {

meas_cell_cfg_t generate_cell1()
{
  meas_cell_cfg_t cell1{};
  cell1.earfcn   = 3400;
  cell1.pci      = 1;
  cell1.q_offset = 0;
  cell1.eci      = 0x19C01;
  return cell1;
}

report_cfg_eutra_s generate_rep1()
{
  report_cfg_eutra_s rep{};
  rep.report_amount.value = report_cfg_eutra_s::report_amount_opts::r16;
  rep.report_interv.value = report_interv_opts::ms240;
  rep.max_report_cells    = 2;
  rep.report_quant.value  = report_cfg_eutra_s::report_quant_opts::both;
  rep.trigger_quant.value = report_cfg_eutra_s::trigger_quant_opts::rsrp;
  rep.trigger_type.set_event().event_id.set_event_a3();
  rep.trigger_type.event().time_to_trigger.value               = time_to_trigger_opts::ms100;
  rep.trigger_type.event().hysteresis                          = 0;
  rep.trigger_type.event().event_id.event_a3().a3_offset       = 5;
  rep.trigger_type.event().event_id.event_a3().report_on_leave = true;
  return rep;
}

bool is_cell_cfg_equal(const meas_cell_cfg_t& cfg, const cells_to_add_mod_s& cell)
{
  return cfg.pci == cell.pci and cell.cell_individual_offset.to_number() == (int8_t)round(cfg.q_offset);
}

} // namespace srsenb