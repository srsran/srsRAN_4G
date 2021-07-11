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

#include "srsran/asn1/rrc/dl_dcch_msg.h"
#include "srsran/asn1/rrc/ul_dcch_msg.h"
#include "srsran/asn1/rrc_utils.h"
#include "srsran/common/bcd_helpers.h"
#include "srsran/common/test_common.h"
#include "srsran/interfaces/rrc_interface_types.h"
#include <iostream>

#define JSON_OUTPUT 0

using namespace asn1;
using namespace asn1::rrc;

int meas_obj_test()
{
  uint8_t rrc_msg[] = {
      0x08, 0x10, 0x49, 0x3C, 0x0D, 0x97, 0x89, 0x83, 0xC0, 0x84, 0x20, 0x82, 0x08, 0x21, 0x00, 0x01, 0xBC, 0x48};
  uint32_t rrc_msg_len = sizeof(rrc_msg);
  // 0810493C0D978983C084208208210001BC48

  cbit_ref bref(&rrc_msg[0], sizeof(rrc_msg));

  ul_dcch_msg_s ul_dcch_msg;
  ul_dcch_msg.unpack(bref);

  TESTASSERT(ul_dcch_msg.msg.type() == ul_dcch_msg_type_c::types::c1);
  TESTASSERT(ul_dcch_msg.msg.c1().type() == ul_dcch_msg_type_c::c1_c_::types::meas_report);

  meas_report_s* rep = &ul_dcch_msg.msg.c1().meas_report();
  TESTASSERT(rep->crit_exts.type() == meas_report_s::crit_exts_c_::types::c1);
  TESTASSERT(rep->crit_exts.c1().type() == meas_report_s::crit_exts_c_::c1_c_::types::meas_report_r8);
  TESTASSERT(not rep->crit_exts.c1().meas_report_r8().non_crit_ext_present);
  meas_results_s* meas = &rep->crit_exts.c1().meas_report_r8().meas_results;

  TESTASSERT(not meas->ext);
  TESTASSERT(meas->meas_id == 1);
  TESTASSERT(meas->meas_result_pcell.rsrp_result == 73);
  TESTASSERT(meas->meas_result_pcell.rsrq_result == 15);
  TESTASSERT(meas->meas_result_neigh_cells_present);
  TESTASSERT(meas->meas_result_neigh_cells.type() ==
             meas_results_s::meas_result_neigh_cells_c_::types::meas_result_list_eutra);
  meas_result_list_eutra_l& meas_list = meas->meas_result_neigh_cells.meas_result_list_eutra();
  TESTASSERT(meas_list.size() == 1);
  TESTASSERT(meas_list[0].pci == 357);
  TESTASSERT(meas_list[0].cgi_info_present);
  TESTASSERT(meas_list[0].cgi_info.plmn_id_list_present);
  TESTASSERT(meas_list[0].cgi_info.cell_global_id.plmn_id.mcc_present);
  srsran::plmn_id_t plmn = srsran::make_plmn_id_t(meas_list[0].cgi_info.cell_global_id.plmn_id);
  TESTASSERT(plmn.to_string() == "89878");
  TESTASSERT(meas_list[0].cgi_info.cell_global_id.cell_id.to_number() == 0x1084104);
  TESTASSERT(meas_list[0].cgi_info.tac.to_number() == 0x1042);
  TESTASSERT(meas_list[0].cgi_info.plmn_id_list.size() == 1);
  TESTASSERT(not meas_list[0].cgi_info.plmn_id_list[0].mcc_present);
  TESTASSERT(not meas_list[0].meas_result.ext);
  TESTASSERT(meas_list[0].meas_result.rsrp_result_present);
  TESTASSERT(meas_list[0].meas_result.rsrp_result == 60);
  TESTASSERT(meas_list[0].meas_result.rsrq_result_present);
  TESTASSERT(meas_list[0].meas_result.rsrq_result == 18);

  uint8_t rrc_msg2[rrc_msg_len];
  bzero(rrc_msg2, rrc_msg_len);
  bit_ref bref2(&rrc_msg2[0], sizeof(rrc_msg2));
  ul_dcch_msg.pack(bref2);
  TESTASSERT(bref.distance() == bref2.distance());
  TESTASSERT(memcmp(rrc_msg2, rrc_msg, rrc_msg_len) == 0);

  return 0;
}

int test_meas_config()
{
  // RRC reconfig with NR meas config
  uint8_t tv[] = {0x20, 0x10, 0x15, 0xc0, 0x40, 0x00, 0x00, 0x96, 0x32, 0x18, 0x10, 0xa8, 0x04, 0xd6, 0xa0,
                  0x10, 0x02, 0x01, 0x02, 0x18, 0x9a, 0x00, 0x03, 0x41, 0x81, 0x0e, 0x00, 0x01, 0x38, 0x00,
                  0xc0, 0x40, 0x61, 0xc0, 0x00, 0x83, 0x00, 0x31, 0x02, 0x90, 0x60, 0x88, 0x00};

  asn1::SRSASN_CODE err;

  cbit_ref bref(tv, sizeof(tv));

  dl_dcch_msg_s recfg_msg_unpacked;

  TESTASSERT(recfg_msg_unpacked.unpack(bref) == SRSASN_SUCCESS);

  TESTASSERT(test_pack_unpack_consistency(recfg_msg_unpacked) == SRSASN_SUCCESS);

#if JSON_OUTPUT
  int               unpacked_len = bref.distance_bytes();
  asn1::json_writer json_writer1;
  recfg_msg_unpacked.to_json(json_writer1);
  srslog::fetch_basic_logger("ASN1").info(
      tv, sizeof(tv), "RRC config unpacked (%d B): \n %s", unpacked_len, json_writer1.to_string().c_str());
#endif

  dl_dcch_msg_s recfg_msg_packed;

  recfg_msg_packed.msg.set_c1();
  recfg_msg_packed.msg.c1().set_rrc_conn_recfg(); // = dl_dcch_msg_type_c::c1_c_::types::rrc_conn_recfg;
  recfg_msg_packed.msg.c1().rrc_conn_recfg().crit_exts.set_c1();
  recfg_msg_packed.msg.c1().rrc_conn_recfg().crit_exts.c1().set_rrc_conn_recfg_r8();
  recfg_msg_packed.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8().meas_cfg_present = true;

  meas_cfg_s& meas_cfg = recfg_msg_packed.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8().meas_cfg;

  meas_cfg.meas_obj_to_add_mod_list_present = true;
  meas_cfg.meas_obj_to_add_mod_list.resize(2);

  auto& meas_obj       = meas_cfg.meas_obj_to_add_mod_list[0];
  meas_obj.meas_obj_id = 1;
  meas_obj.meas_obj.set_meas_obj_eutra();
  meas_obj.meas_obj.meas_obj_eutra().carrier_freq       = 300;
  meas_obj.meas_obj.meas_obj_eutra().allowed_meas_bw    = allowed_meas_bw_opts::mbw50;
  meas_obj.meas_obj.meas_obj_eutra().presence_ant_port1 = false;
  meas_obj.meas_obj.meas_obj_eutra().neigh_cell_cfg.from_number(0b01);

  auto& meas_obj2       = meas_cfg.meas_obj_to_add_mod_list[1];
  meas_obj2.meas_obj_id = 2;
  meas_obj2.meas_obj.set_meas_obj_nr_r15();
  meas_obj2.meas_obj.meas_obj_nr_r15().carrier_freq_r15 = 634176;
  meas_obj2.meas_obj.meas_obj_nr_r15().rs_cfg_ssb_r15.meas_timing_cfg_r15.periodicity_and_offset_r15.set_sf20_r15();
  meas_obj2.meas_obj.meas_obj_nr_r15().rs_cfg_ssb_r15.meas_timing_cfg_r15.ssb_dur_r15 =
      asn1::rrc::mtc_ssb_nr_r15_s::ssb_dur_r15_opts::sf1;
  meas_obj2.meas_obj.meas_obj_nr_r15().rs_cfg_ssb_r15.subcarrier_spacing_ssb_r15 =
      asn1::rrc::rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_opts::khz30;
  meas_obj2.meas_obj.meas_obj_nr_r15().ext = true;
  meas_obj2.meas_obj.meas_obj_nr_r15().band_nr_r15.set_present(true);
  meas_obj2.meas_obj.meas_obj_nr_r15().band_nr_r15.get()->set_setup() = 78;

  // report config
  meas_cfg.report_cfg_to_add_mod_list_present = true;
  meas_cfg.report_cfg_to_add_mod_list.resize(1);
  auto& report_cfg = meas_cfg.report_cfg_to_add_mod_list[0];

  report_cfg.report_cfg_id = 1;
  report_cfg.report_cfg.set_report_cfg_inter_rat();
  report_cfg.report_cfg.report_cfg_inter_rat().trigger_type.set_event();
  report_cfg.report_cfg.report_cfg_inter_rat().trigger_type.event().event_id.set_event_b1_nr_r15();
  report_cfg.report_cfg.report_cfg_inter_rat()
      .trigger_type.event()
      .event_id.event_b1_nr_r15()
      .b1_thres_nr_r15.set_nr_rsrp_r15();
  report_cfg.report_cfg.report_cfg_inter_rat()
      .trigger_type.event()
      .event_id.event_b1_nr_r15()
      .b1_thres_nr_r15.nr_rsrp_r15() = 56;
  report_cfg.report_cfg.report_cfg_inter_rat().trigger_type.event().event_id.event_b1_nr_r15().report_on_leave_r15 =
      false;
  report_cfg.report_cfg.report_cfg_inter_rat().trigger_type.event().hysteresis      = 0;
  report_cfg.report_cfg.report_cfg_inter_rat().trigger_type.event().time_to_trigger = time_to_trigger_opts::ms100;

  report_cfg.report_cfg.report_cfg_inter_rat().max_report_cells = 8;
  report_cfg.report_cfg.report_cfg_inter_rat().report_interv    = report_interv_opts::ms120;
  report_cfg.report_cfg.report_cfg_inter_rat().report_amount    = report_cfg_inter_rat_s::report_amount_opts::r1;
  report_cfg.report_cfg.report_cfg_inter_rat().ext              = true;
  report_cfg.report_cfg.report_cfg_inter_rat().report_quant_cell_nr_r15.set_present(true);
  report_cfg.report_cfg.report_cfg_inter_rat().report_quant_cell_nr_r15.get()->ss_rsrp = true;
  report_cfg.report_cfg.report_cfg_inter_rat().report_quant_cell_nr_r15.get()->ss_rsrq = true;
  report_cfg.report_cfg.report_cfg_inter_rat().report_quant_cell_nr_r15.get()->ss_sinr = true;

  // measIdToAddModList
  meas_cfg.meas_id_to_add_mod_list_present = true;
  meas_cfg.meas_id_to_add_mod_list.resize(1);
  auto& meas_id         = meas_cfg.meas_id_to_add_mod_list[0];
  meas_id.meas_id       = 1;
  meas_id.meas_obj_id   = 2;
  meas_id.report_cfg_id = 1;

  // quantityConfig
  meas_cfg.quant_cfg_present                 = true;
  meas_cfg.quant_cfg.quant_cfg_eutra_present = true;
  meas_cfg.quant_cfg.ext                     = true;
  meas_cfg.quant_cfg.quant_cfg_nr_list_r15.set_present(true);
  meas_cfg.quant_cfg.quant_cfg_nr_list_r15.get()->resize(1);
  auto& meas_quant                                                 = meas_cfg.quant_cfg.quant_cfg_nr_list_r15.get()[0];
  meas_quant[0].meas_quant_cell_nr_r15.filt_coeff_rsrp_r15_present = true;
  meas_quant[0].meas_quant_cell_nr_r15.filt_coeff_rsrp_r15         = filt_coef_opts::fc3;

  // measGapConfig
  meas_cfg.meas_gap_cfg_present = true;
  meas_cfg.meas_gap_cfg.set_setup();
  meas_cfg.meas_gap_cfg.setup().gap_offset.set_gp0() = 16;

  uint8_t pack_buffer[1024];
  bit_ref bref2(pack_buffer, sizeof(pack_buffer));
  recfg_msg_packed.pack(bref2);
  int packed_len = bref2.distance_bytes();
  TESTASSERT(sizeof(tv) == packed_len);
  TESTASSERT(memcmp(pack_buffer, tv, packed_len) == 0);

#if JSON_OUTPUT
  asn1::json_writer json_writer2;
  recfg_msg_packed.to_json(json_writer2);
  srslog::fetch_basic_logger("ASN1").info(
      pack_buffer, packed_len, "RRC config packed (%d B): \n %s", packed_len, json_writer2.to_string().c_str());
#endif

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  auto& asn1_logger = srslog::fetch_basic_logger("ASN1", false);
  asn1_logger.set_level(srslog::basic_levels::debug);
  asn1_logger.set_hex_dump_max_size(-1);

  srslog::init();

  TESTASSERT(meas_obj_test() == SRSRAN_SUCCESS);
  TESTASSERT(test_meas_config() == SRSRAN_SUCCESS);

  return 0;
}
