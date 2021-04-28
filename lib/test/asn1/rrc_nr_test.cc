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

#include "srsran/asn1/rrc_nr.h"
#include "srsran/common/test_common.h"
#include <cstdio>

using namespace asn1;
using namespace asn1::rrc_nr;

int test_eutra_nr_capabilities()
{
  struct ue_mrdc_cap_s mrdc_cap;
  band_combination_s   band_combination;
  struct band_params_c band_param_eutra;
  band_param_eutra.set_eutra();
  band_param_eutra.eutra().ca_bw_class_dl_eutra_present = true;
  band_param_eutra.eutra().ca_bw_class_ul_eutra_present = true;
  band_param_eutra.eutra().band_eutra                   = 1;
  band_param_eutra.eutra().ca_bw_class_dl_eutra         = asn1::rrc_nr::ca_bw_class_eutra_opts::options::a;
  band_param_eutra.eutra().ca_bw_class_ul_eutra         = asn1::rrc_nr::ca_bw_class_eutra_opts::options::a;
  band_combination.band_list.push_back(band_param_eutra);
  struct band_params_c band_param_nr;
  band_param_nr.set_nr();
  band_param_nr.nr().ca_bw_class_dl_nr_present = true;
  band_param_nr.nr().ca_bw_class_ul_nr_present = true;
  band_param_nr.nr().band_nr                   = 78;
  band_param_nr.nr().ca_bw_class_dl_nr         = asn1::rrc_nr::ca_bw_class_nr_opts::options::a;
  band_param_nr.nr().ca_bw_class_ul_nr         = asn1::rrc_nr::ca_bw_class_nr_opts::options::a;
  band_combination.band_list.push_back(band_param_nr);

  mrdc_cap.rf_params_mrdc.supported_band_combination_list.push_back(band_combination);
  mrdc_cap.rf_params_mrdc.supported_band_combination_list_present = true;

  mrdc_cap.rf_params_mrdc.ext = true;

  // RF Params MRDC applied_freq_band_list_filt
  freq_band_info_c band_info_eutra;
  band_info_eutra.set_band_info_eutra();
  band_info_eutra.band_info_eutra().ca_bw_class_dl_eutra_present = false;
  band_info_eutra.band_info_eutra().ca_bw_class_ul_eutra_present = false;
  band_info_eutra.band_info_eutra().band_eutra                   = 1;
  mrdc_cap.rf_params_mrdc.applied_freq_band_list_filt.push_back(band_info_eutra);

  freq_band_info_c band_info_nr;
  band_info_nr.set_band_info_nr();
  band_info_nr.band_info_nr().band_nr = 78;
  mrdc_cap.rf_params_mrdc.applied_freq_band_list_filt.push_back(band_info_nr);

  mrdc_cap.rf_params_mrdc.applied_freq_band_list_filt_present = true;

  // rf_params_mrdc supported band combination list v1540

  band_combination_list_v1540_l* band_combination_list_v1450 = new band_combination_list_v1540_l();
  band_combination_v1540_s       band_combination_v1540;

  band_params_v1540_s band_params_a;
  band_params_a.srs_tx_switch_present      = true;
  band_params_a.srs_carrier_switch_present = false;
  band_params_a.srs_tx_switch.supported_srs_tx_port_switch =
      band_params_v1540_s::srs_tx_switch_s_::supported_srs_tx_port_switch_opts::not_supported;
  band_combination_v1540.band_list_v1540.push_back(band_params_a);

  band_params_v1540_s band_params_b;
  band_params_b.srs_tx_switch_present = true;
  band_params_b.srs_tx_switch.supported_srs_tx_port_switch =
      band_params_v1540_s::srs_tx_switch_s_::supported_srs_tx_port_switch_opts::t1r2;
  band_params_b.srs_carrier_switch_present = false;
  band_combination_v1540.band_list_v1540.push_back(band_params_b);

  // clang-format off
  band_combination_v1540.ca_params_nr_v1540_present = false;
  band_combination_v1540.ca_params_nr_v1540.simul_csi_reports_all_cc_present = true;
  band_combination_v1540.ca_params_nr_v1540.csi_rs_im_reception_for_feedback_per_band_comb.max_num_simul_nzp_csi_rs_act_bwp_all_cc_present = true;
  band_combination_v1540.ca_params_nr_v1540.csi_rs_im_reception_for_feedback_per_band_comb.max_num_simul_nzp_csi_rs_act_bwp_all_cc = 5;
  band_combination_v1540.ca_params_nr_v1540.csi_rs_im_reception_for_feedback_per_band_comb.total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc_present = true;
  band_combination_v1540.ca_params_nr_v1540.csi_rs_im_reception_for_feedback_per_band_comb.total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc = 32;
  // clang-format on
  band_combination_list_v1450->push_back(band_combination_v1540);
  mrdc_cap.rf_params_mrdc.supported_band_combination_list_v1540.reset(band_combination_list_v1450);

  feature_set_combination_l feature_set_combination;

  feature_sets_per_band_l feature_sets_per_band;

  feature_set_c feature_set_eutra;
  feature_set_eutra.set_eutra();
  feature_set_eutra.eutra().dl_set_eutra = 1;
  feature_set_eutra.eutra().ul_set_eutra = 1;
  feature_sets_per_band.push_back(feature_set_eutra);

  feature_set_combination.push_back(feature_sets_per_band);

  feature_set_c feature_set_nr;
  feature_set_nr.set_nr();
  feature_set_nr.nr().dl_set_nr = 1;
  feature_set_nr.nr().ul_set_nr = 1;
  feature_sets_per_band.push_back(feature_set_nr);

  feature_set_combination.push_back(feature_sets_per_band);

  mrdc_cap.feature_set_combinations.push_back(feature_set_combination);

  mrdc_cap.feature_set_combinations_present = true;

  // Pack mrdc_cap
  uint8_t       buffer[1024];
  asn1::bit_ref bref(buffer, sizeof(buffer));
  mrdc_cap.pack(bref);

  TESTASSERT(test_pack_unpack_consistency(mrdc_cap) == SRSASN_SUCCESS);

  srslog::fetch_basic_logger("RRC").info(
      buffer, bref.distance_bytes(), "Packed cap struct (%d bytes):", bref.distance_bytes());

  return SRSRAN_SUCCESS;
}

int test_ue_mrdc_capabilities()
{
  uint8_t msg[] = {0x01, 0x1c, 0x04, 0x81, 0x60, 0x00, 0x1c, 0x4d, 0x00, 0x00, 0x00, 0x04,
                   0x00, 0x40, 0x04, 0x04, 0xd0, 0x10, 0x74, 0x06, 0x14, 0xe8, 0x1b, 0x10,
                   0x78, 0x00, 0x00, 0x20, 0x00, 0x10, 0x08, 0x08, 0x01, 0x00, 0x20};
  // 011c048160001c4d0000000400400404d010740614e81b107800002000100808010020

  asn1::cbit_ref bref{msg, sizeof(msg)};
  ue_mrdc_cap_s  mrdc_cap;

  TESTASSERT(mrdc_cap.unpack(bref) == SRSASN_SUCCESS);

  TESTASSERT(test_pack_unpack_consistency(mrdc_cap) == SRSASN_SUCCESS);

  return SRSRAN_SUCCESS;
}

int test_ue_rrc_reconfiguration()
{
  uint8_t rrc_msg[] = "\x08\x81\x7c\x5c\x40\xb1\xc0\x7d\x48\x3a\x04\xc0\x3e\x01\x04\x54"
                      "\x1e\xb5\x00\x02\xe8\x53\x98\xdf\x46\x93\x4b\x80\x04\xd2\x69\x34"
                      "\x00\x00\x08\xc9\x8d\x6d\x8c\xa2\x01\xff\x00\x00\x00\x00\x01\x1b"
                      "\x82\x21\x00\x00\x04\x04\x00\xd1\x14\x0e\x70\x00\x00\x08\xc9\xc6"
                      "\xb6\xc6\x44\xa0\x00\x1e\xb8\x95\x63\xe0\x24\x94\x22\x0d\xb8\x44"
                      "\x70\x0c\x02\x10\xb0\x1d\x80\x48\xf1\x18\x06\xea\x00\x08\x0e\x01"
                      "\x25\xc0\xc8\x80\x37\x08\x42\x00\x00\x88\x16\x50\x02\x0c\x82\x00"
                      "\x00\x20\x69\x81\x01\x45\x0a\x00\x0e\x48\x18\x00\x01\x33\x55\x64"
                      "\x84\x1c\x00\x10\x40\xc2\x05\x0c\x1c\x9c\x40\x91\x42\xc6\x0d\x1c"
                      "\x3c\x8e\x00\x00\x32\x21\x40\x30\x20\x01\x91\x4a\x01\x82\x00\x0c"
                      "\x8c\x50\x0c\x18\x00\x64\x42\x80\xe1\x00\x03\x22\x94\x07\x0a\x00"
                      "\x19\x18\xa0\x38\x60\x00\xc8\x85\x02\xc3\x80\x06\x45\x28\x16\x20"
                      "\x64\x00\x41\x6c\x48\x04\x62\x82\x18\xa0\x08\xc5\x04\xb1\x60\x11"
                      "\x8a\x0a\x63\x00\x23\x14\x16\xc6\x80\x46\x28\x31\x8e\x00\x8c\x50"
                      "\x6b\x1e\x01\x18\xa0\xe6\x40\x00\x32\x31\x40\xb2\x23\x10\x0a\x08"
                      "\x40\x90\x86\x05\x10\x43\xcc\x3b\x2a\x6e\x4d\x01\xa4\x92\x1e\x2e"
                      "\xe0\x0c\x10\xe0\x00\x00\x01\x8f\xfd\x29\x49\x8c\x63\x72\x81\x60"
                      "\x00\x02\x19\x70\x00\x00\x00\x00\x00\x00\x52\xf0\x0f\xa0\x84\x8a"
                      "\xd5\x45\x00\x47\x00\x18\x00\x08\x20\x00\xe2\x10\x02\x40\x80\x70"
                      "\x10\x10\x84\x00\x0e\x21\x00\x1c\xb0\x0e\x04\x02\x20\x80\x01\xc4"
                      "\x20\x03\x96\x01\xc0\xc0\x42\x10\x00\x38\x84\x00\x73\x00\x38\x20"
                      "\x08\x82\x00\x07\x10\x80\x0e\x60\x00\x40\x00\x00\x04\x10\xc0\x40"
                      "\x80\xc1\x00\xe0\xd0\x00\x0e\x48\x10\x00\x00\x02\x00\x40\x00\x80"
                      "\x60\x00\x80\x90\x02\x20\x0a\x40\x00\x02\x38\x90\x11\x31\xc8";

  uint32_t    rrc_msg_len = sizeof(rrc_msg);
  cbit_ref    bref(&rrc_msg[0], sizeof(rrc_msg));
  rrc_recfg_s rrc_recfg;

  TESTASSERT(rrc_recfg.unpack(bref) == SRSASN_SUCCESS);
  TESTASSERT(rrc_recfg.rrc_transaction_id == 0);
  json_writer jw;
  rrc_recfg.to_json(jw);
  srslog::fetch_basic_logger("RRC").info("RRC Reconfig: \n %s", jw.to_string().c_str());

  TESTASSERT(rrc_recfg.crit_exts.type() == asn1::rrc_nr::rrc_recfg_s::crit_exts_c_::types::rrc_recfg);
  TESTASSERT(rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group_present == true);

  cell_group_cfg_s cell_group_cfg;
  cbit_ref         bref0(rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.data(),
                 rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.size());
  TESTASSERT(cell_group_cfg.unpack(bref0) == SRSASN_SUCCESS);
  // json_writer jw1;
  // cell_group_cfg.to_json(jw1);
  // srslog::fetch_basic_logger("RRC").info("RRC Secondary Cell Group: \n %s", jw1.to_string().c_str());
  TESTASSERT(cell_group_cfg.cell_group_id == 1);
  TESTASSERT(cell_group_cfg.rlc_bearer_to_add_mod_list_present == true);
  TESTASSERT(cell_group_cfg.rlc_bearer_to_add_mod_list.size() == 1);
  TESTASSERT(cell_group_cfg.mac_cell_group_cfg_present == true);
  TESTASSERT(cell_group_cfg.phys_cell_group_cfg_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg_present == true);
  return SRSRAN_SUCCESS;
}

int test_radio_bearer_config()
{
  uint8_t            rrc_msg[]   = "\x14\x09\x28\x17\x87\xc0\x0c\x28";
  uint32_t           rrc_msg_len = sizeof(rrc_msg);
  cbit_ref           bref(&rrc_msg[0], sizeof(rrc_msg));
  radio_bearer_cfg_s radio_bearer_cfg;
  TESTASSERT(radio_bearer_cfg.unpack(bref) == SRSASN_SUCCESS);
  // json_writer jw;
  // radio_bearer_cfg.to_json(jw);
  // srslog::fetch_basic_logger("RRC").info("RRC Bearer CFG Message: \n %s", jw.to_string().c_str());
  TESTASSERT(radio_bearer_cfg.drb_to_add_mod_list_present == true);
  TESTASSERT(radio_bearer_cfg.drb_to_add_mod_list.size() == 1);
  TESTASSERT(radio_bearer_cfg.security_cfg_present == true);
  TESTASSERT(radio_bearer_cfg.security_cfg.security_algorithm_cfg_present == true);
  TESTASSERT(radio_bearer_cfg.security_cfg.key_to_use_present == true);
  return SRSRAN_SUCCESS;
}

int test_cell_group_config()
{
  uint8_t cell_group_config_raw[] = "\x5c\x40\xb1\xc0\x33\xc8\x53\xe0\x12\x0f\x05\x38\x0f\x80\x41\x15"
                                    "\x07\xad\x40\x00\xba\x14\xe6\x37\xd1\xa4\xd3\xa0\x01\x34\x9a\x5f"
                                    "\xc0\x00\x00\x33\x63\x6c\x91\x28\x80\x7f\xc0\x00\x00\x00\x00\x46"
                                    "\xe0\x88\x40\x00\x01\x01\x00\x34\x45\x03\x9c\x00\x00\x00\x33\x71"
                                    "\xb6\x48\x90\x04\x00\x08\x2e\x25\x18\xf0\x02\x4a\x31\x06\xe2\x8d"
                                    "\xb8\x44\x70\x0c\x02\x10\x38\x1d\x80\x48\xf1\x18\x5e\xea\x00\x08"
                                    "\x0e\x01\x25\xc0\xca\x80\x01\x7f\x80\x00\x00\x00\x00\x83\x70\x88"
                                    "\x20\x00\x08\x81\x65\x00\x20\xc8\x20\x00\x02\x06\x98\x10\x14\x50"
                                    "\xa0\x00\xe4\x81\x80\x00\x13\x35\x56\x48\x41\xc0\x01\x04\x0c\x20"
                                    "\x50\xc1\xc9\xc4\x09\x14\x2c\x60\xd1\xc3\xc8\xe0\x00\x03\x32\x14"
                                    "\x03\x02\x00\x19\x94\xa0\x18\x20\x00\xcc\xc5\x00\xc1\x80\x06\x64"
                                    "\x28\x0e\x10\x00\x33\x29\x40\x70\xa0\x01\x99\x8a\x03\x86\x00\x0c"
                                    "\xc8\x50\x2c\x38\x00\x66\x52\x81\x62\x06\x60\x04\x16\xc4\x80\x46"
                                    "\x48\x21\x8a\x00\x8c\x90\x4b\x16\x01\x19\x20\xa6\x30\x02\x32\x41"
                                    "\x6c\x68\x04\x64\x83\x18\xe0\x08\xc9\x06\xb1\xe0\x11\x92\x0e\x64"
                                    "\x00\x03\x33\x14\x0b\x22\x32\x00\xa0\x84\x09\x08\x60\x51\x04\x34"
                                    "\x3b\x2a\x65\xcd\x01\xa4\x92\x1e\x2e\xe0\x0c\x10\xe0\x00\x00\x01"
                                    "\x8f\xfd\x29\x49\x8c\x63\x72\x81\x60\x00\x02\x19\x70\x00\x00\x00"
                                    "\x00\x00\x00\x62\xf0\x0f\xa0\x84\x8a\xd5\x45\x00\x47\x00\x18\x00"
                                    "\x08\x20\x00\xe2\x10\x02\x40\x80\x70\x10\x10\x84\x00\x0e\x21\x00"
                                    "\x1c\xb0\x0e\x04\x02\x20\x80\x01\xc4\x20\x03\x96\x01\xc0\xc0\x42"
                                    "\x10\x00\x38\x84\x00\x73\x00\x38\x20\x08\x82\x00\x07\x10\x80\x0e"
                                    "\x60\x00\x40\x00\x00\x04\x10\xc0\x40\x80\xc1\x00\xe0\xd0\x00\x0e"
                                    "\x48\x10\x00\x00\x02\x00\x40\x00\x80\x60\x00\x80\x90\x02\x20\x0a"
                                    "\x40\x00\x02\x38\x90\x11\x31\xc8";

  asn1::SRSASN_CODE err;

  cbit_ref         bref(&cell_group_config_raw[0], sizeof(cell_group_config_raw));
  cell_group_cfg_s cell_group_cfg;

  TESTASSERT(cell_group_cfg.unpack(bref) == SRSASN_SUCCESS);

  TESTASSERT(test_pack_unpack_consistency(cell_group_cfg) == SRSASN_SUCCESS);

  TESTASSERT(cell_group_cfg.sp_cell_cfg_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.serv_cell_idx_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.sp_cell_cfg_ded_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.sp_cell_cfg_ded.pdcch_serving_cell_cfg_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync_present = true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.pci_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.pci == 500);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp_present == true);
  TESTASSERT(
      cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common_present ==
      true);

  TESTASSERT(
      cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common.type() ==
      asn1::rrc_nr::setup_release_c<rach_cfg_common_s>::types_opts::setup);

  asn1::rrc_nr::rach_cfg_common_s& rach_cfg_common =
      cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common.setup();

  TESTASSERT(rach_cfg_common.rach_cfg_generic.prach_cfg_idx == 16);
  TESTASSERT(rach_cfg_common.rach_cfg_generic.msg1_fdm == asn1::rrc_nr::rach_cfg_generic_s::msg1_fdm_opts::one);
  TESTASSERT(rach_cfg_common.rach_cfg_generic.zero_correlation_zone_cfg == 0);
  TESTASSERT(rach_cfg_common.rach_cfg_generic.preamb_rx_target_pwr == -110);
  TESTASSERT(rach_cfg_common.rach_cfg_generic.preamb_trans_max ==
             asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n7);
  TESTASSERT(rach_cfg_common.rach_cfg_generic.pwr_ramp_step ==
             asn1::rrc_nr::rach_cfg_generic_s::pwr_ramp_step_opts::db4);
  TESTASSERT(rach_cfg_common.rach_cfg_generic.ra_resp_win == asn1::rrc_nr::rach_cfg_generic_s::ra_resp_win_opts::sl10);
  TESTASSERT(rach_cfg_common.ssb_per_rach_occasion_and_cb_preambs_per_ssb_present == true);

  // asn1::json_writer json_writer;
  // cell_group_cfg.to_json(json_writer);
  // srslog::fetch_basic_logger("RRC").info("RRC Secondary Cell Group: Content: %s\n", json_writer.to_string().c_str());
  return SRSRAN_SUCCESS;
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

  TESTASSERT(test_eutra_nr_capabilities() == SRSRAN_SUCCESS);
  TESTASSERT(test_ue_mrdc_capabilities() == SRSRAN_SUCCESS);
  TESTASSERT(test_ue_rrc_reconfiguration() == SRSRAN_SUCCESS);
  TESTASSERT(test_radio_bearer_config() == SRSRAN_SUCCESS);
  TESTASSERT(test_cell_group_config() == SRSRAN_SUCCESS);

  srslog::flush();

  printf("Success\n");
  return 0;
}
