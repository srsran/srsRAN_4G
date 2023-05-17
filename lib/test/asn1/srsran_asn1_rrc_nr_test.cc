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

#include "srsran/asn1/rrc_nr.h"
#include "srsran/common/test_common.h"
#include <cstdio>

#define JSON_OUTPUT 0

#define HAVE_PCAP 0
#if HAVE_PCAP
#include "srsran/common/test_pcap.h"
#endif

using namespace asn1;
using namespace asn1::rrc_nr;

void test_rrc_setup_complete()
{
  uint8_t msg[] = {0x10, 0xc0, 0x10, 0x00, 0x20, 0x25, 0x97, 0xe0, 0x1e, 0x1e, 0x34, 0xb5, 0x30, 0xb7, 0xe0, 0x04,
                   0x10, 0x90, 0x00, 0xbf, 0x20, 0x0f, 0x11, 0x08, 0x00, 0x10, 0x15, 0x66, 0x75, 0xf7, 0x12, 0xe0,
                   0x4f, 0x07, 0x0f, 0x07, 0x07, 0x10, 0x03, 0x87, 0xe0, 0x04, 0x10, 0x90, 0x00, 0xbf, 0x20, 0x0f,
                   0x11, 0x08, 0x00, 0x10, 0x15, 0x66, 0x75, 0xf7, 0x11, 0x00, 0x10, 0x32, 0xe0, 0x4f, 0x07, 0x0f,
                   0x07, 0x02, 0xf0, 0x20, 0x10, 0x15, 0x20, 0x0f, 0x11, 0x00, 0x00, 0x06, 0x41, 0x70, 0x7f, 0x07,
                   0x00, 0x00, 0x01, 0x88, 0x0b, 0x01, 0x80, 0x10, 0x17, 0x40, 0x00, 0x09, 0x05, 0x30, 0x10, 0x10};
  // 10c01000202597e01e1e34b530b7e004109000bf200f11080010156675f712e04f070f0707100387e004109000bf200f11080010156675f711001032e04f070f0702f0201015200f1100000641707f07000001880b0180101740000905301010

  asn1::cbit_ref              bref{msg, sizeof(msg)};
  asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg;
  TESTASSERT_SUCCESS(ul_dcch_msg.unpack(bref));

  TESTASSERT_EQ(ul_dcch_msg_type_c::types_opts::c1, ul_dcch_msg.msg.type().value);
  TESTASSERT_EQ(ul_dcch_msg_type_c::c1_c_::types_opts::rrc_setup_complete, ul_dcch_msg.msg.c1().type().value);

  TESTASSERT_SUCCESS(test_pack_unpack_consistency(ul_dcch_msg));
}

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
#if JSON_OUTPUT
  json_writer jw;
  rrc_recfg.to_json(jw);
  srslog::fetch_basic_logger("RRC").info("RRC Reconfig: \n %s", jw.to_string().c_str());
#endif

  TESTASSERT(rrc_recfg.crit_exts.type() == asn1::rrc_nr::rrc_recfg_s::crit_exts_c_::types::rrc_recfg);
  TESTASSERT(rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.size() > 0);

  cell_group_cfg_s cell_group_cfg;
  cbit_ref         bref0(rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.data(),
                 rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.size());
  TESTASSERT(cell_group_cfg.unpack(bref0) == SRSASN_SUCCESS);
#if JSON_OUTPUT
  json_writer jw1;
  cell_group_cfg.to_json(jw1);
  srslog::fetch_basic_logger("RRC").info("RRC Secondary Cell Group: \n %s", jw1.to_string().c_str());
#endif
  TESTASSERT(cell_group_cfg.cell_group_id == 1);
  TESTASSERT(cell_group_cfg.rlc_bearer_to_add_mod_list.size() == 1);
  TESTASSERT(cell_group_cfg.mac_cell_group_cfg_present == true);
  TESTASSERT(cell_group_cfg.phys_cell_group_cfg_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg_present == true);
  return SRSRAN_SUCCESS;
}

int test_radio_bearer_config()
{
  uint8_t            rrc_msg[] = "\x14\x09\x28\x17\x87\xc0\x0c\x28";
  cbit_ref           bref(&rrc_msg[0], sizeof(rrc_msg));
  radio_bearer_cfg_s radio_bearer_cfg;
  TESTASSERT(radio_bearer_cfg.unpack(bref) == SRSASN_SUCCESS);
#if JSON_OUTPUT
  json_writer jw;
  radio_bearer_cfg.to_json(jw);
  srslog::fetch_basic_logger("RRC").info("RRC Bearer CFG Message: \n %s", jw.to_string().c_str());
#endif
  TESTASSERT(radio_bearer_cfg.drb_to_add_mod_list.size() == 1);
  TESTASSERT(radio_bearer_cfg.security_cfg_present == true);
  TESTASSERT(radio_bearer_cfg.security_cfg.security_algorithm_cfg_present == true);
  TESTASSERT(radio_bearer_cfg.security_cfg.key_to_use_present == true);

  // full RRC reconfig pack
  rrc_recfg_s reconfig;
  reconfig.rrc_transaction_id = 0;
  rrc_recfg_ies_s& recfg_ies  = reconfig.crit_exts.set_rrc_recfg();

  recfg_ies.radio_bearer_cfg_present = true;
  recfg_ies.radio_bearer_cfg.drb_to_add_mod_list.resize(1);

  auto& drb_item                                = recfg_ies.radio_bearer_cfg.drb_to_add_mod_list[0];
  drb_item.drb_id                               = 1;
  drb_item.cn_assoc_present                     = true;
  drb_item.cn_assoc.set_eps_bearer_id()         = 5;
  drb_item.pdcp_cfg_present                     = true;
  drb_item.pdcp_cfg.ciphering_disabled_present  = true;
  drb_item.pdcp_cfg.drb_present                 = true;
  drb_item.pdcp_cfg.drb.pdcp_sn_size_dl_present = true;
  drb_item.pdcp_cfg.drb.pdcp_sn_size_dl         = asn1::rrc_nr::pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::len18bits;
  drb_item.pdcp_cfg.drb.pdcp_sn_size_ul_present = true;
  drb_item.pdcp_cfg.drb.pdcp_sn_size_ul         = asn1::rrc_nr::pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::len18bits;
  drb_item.pdcp_cfg.drb.discard_timer_present   = true;
  drb_item.pdcp_cfg.drb.discard_timer           = asn1::rrc_nr::pdcp_cfg_s::drb_s_::discard_timer_opts::ms100;
  drb_item.pdcp_cfg.drb.hdr_compress.set_not_used();
  drb_item.pdcp_cfg.t_reordering_present = true;
  drb_item.pdcp_cfg.t_reordering         = asn1::rrc_nr::pdcp_cfg_s::t_reordering_opts::ms0;

  recfg_ies.radio_bearer_cfg.security_cfg_present            = true;
  recfg_ies.radio_bearer_cfg.security_cfg.key_to_use_present = true;
  recfg_ies.radio_bearer_cfg.security_cfg.key_to_use         = asn1::rrc_nr::security_cfg_s::key_to_use_opts::secondary;
  recfg_ies.radio_bearer_cfg.security_cfg.security_algorithm_cfg_present             = true;
  recfg_ies.radio_bearer_cfg.security_cfg.security_algorithm_cfg.ciphering_algorithm = ciphering_algorithm_opts::nea2;

  uint8_t       buffer[1024];
  asn1::bit_ref bref_pack(buffer, sizeof(buffer));
  TESTASSERT(reconfig.pack(bref_pack) == asn1::SRSASN_SUCCESS);
  TESTASSERT(test_pack_unpack_consistency(reconfig) == SRSASN_SUCCESS);

#if JSON_OUTPUT
  reconfig.to_json(jw);
  srslog::fetch_basic_logger("RRC").info("RRC Reconfig Message: \n %s", jw.to_string().c_str());
#endif

  // only pack the radio bearer config to compare against TV
  asn1::bit_ref       bref_pack2(buffer, sizeof(buffer));
  radio_bearer_cfg_s& radio_bearer_cfg_pack = recfg_ies.radio_bearer_cfg;
  TESTASSERT(radio_bearer_cfg_pack.pack(bref_pack2) == asn1::SRSASN_SUCCESS);

#if JSON_OUTPUT
  radio_bearer_cfg_pack.to_json(jw);
  srslog::fetch_basic_logger("RRC").info("Radio bearer config Message: \n %s", jw.to_string().c_str());
#endif

  // TODO: messages don't match yet
  // TESTASSERT(bref_pack2.distance_bytes() == sizeof(rrc_msg));
  // TESTASSERT(memcmp(rrc_msg, buffer, sizeof(rrc_msg)) == 0);

  return SRSRAN_SUCCESS;
}

int test_cell_group_config_tdd()
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
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.pci_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.pci == 500);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp_present == true);
  TESTASSERT(
      cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common_present ==
      true);

  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common
                 .is_setup());

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

#if JSON_OUTPUT
  asn1::json_writer json_writer;
  cell_group_cfg.to_json(json_writer);
  srslog::fetch_basic_logger("RRC").info("RRC Secondary Cell Group: Content: %s\n", json_writer.to_string().c_str());
#endif

  // pack it again
  cell_group_cfg_s cell_group_cfg_pack;

  // RLC for DRB1
  cell_group_cfg_pack.rlc_bearer_to_add_mod_list.resize(1);
  auto& rlc                       = cell_group_cfg_pack.rlc_bearer_to_add_mod_list[0];
  rlc.lc_ch_id                    = 1;
  rlc.served_radio_bearer_present = true;
  rlc.served_radio_bearer.set_drb_id();
  rlc.served_radio_bearer.drb_id() = 1;
  rlc.rlc_cfg_present              = true;
  rlc.rlc_cfg.set_um_bi_dir();
  rlc.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len_present = true;
  rlc.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len         = sn_field_len_um_opts::size12;
  rlc.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len_present = true;
  rlc.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len         = sn_field_len_um_opts::size12;
  rlc.rlc_cfg.um_bi_dir().dl_um_rlc.t_reassembly         = t_reassembly_opts::ms50;

  // MAC logical channel config
  rlc.mac_lc_ch_cfg_present                    = true;
  rlc.mac_lc_ch_cfg.ul_specific_params_present = true;
  rlc.mac_lc_ch_cfg.ul_specific_params.prio    = 11;
  rlc.mac_lc_ch_cfg.ul_specific_params.prioritised_bit_rate =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::kbps0;
  rlc.mac_lc_ch_cfg.ul_specific_params.bucket_size_dur =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::ms100;
  rlc.mac_lc_ch_cfg.ul_specific_params.lc_ch_group_present      = true;
  rlc.mac_lc_ch_cfg.ul_specific_params.lc_ch_group              = 6;
  rlc.mac_lc_ch_cfg.ul_specific_params.sched_request_id_present = true;
  rlc.mac_lc_ch_cfg.ul_specific_params.sched_request_id         = 0;

  // mac-CellGroup-Config
  cell_group_cfg_pack.mac_cell_group_cfg_present = true;
  auto& mac_cell_group                           = cell_group_cfg_pack.mac_cell_group_cfg;
  mac_cell_group.sched_request_cfg_present       = true;
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list.resize(1);
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list[0].sched_request_id = 0;
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list[0].sr_trans_max =
      asn1::rrc_nr::sched_request_to_add_mod_s::sr_trans_max_opts::n64;
  mac_cell_group.bsr_cfg_present            = true;
  mac_cell_group.bsr_cfg.periodic_bsr_timer = asn1::rrc_nr::bsr_cfg_s::periodic_bsr_timer_opts::sf20;
  mac_cell_group.bsr_cfg.retx_bsr_timer     = asn1::rrc_nr::bsr_cfg_s::retx_bsr_timer_opts::sf320;
  // Skip TAG and PHR config

  cell_group_cfg_pack.sp_cell_cfg_present               = true;
  cell_group_cfg_pack.sp_cell_cfg.serv_cell_idx_present = true;

  // SP Cell Dedicated config
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded_present             = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp_present = true;

  // PDCCH config
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg_present = true;
  auto& pdcch_cfg_dedicated = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg;
  pdcch_cfg_dedicated.set_setup();
  pdcch_cfg_dedicated.setup().ctrl_res_set_to_add_mod_list.resize(1);
  pdcch_cfg_dedicated.setup().ctrl_res_set_to_add_mod_list[0].ctrl_res_set_id = 2;
  pdcch_cfg_dedicated.setup().ctrl_res_set_to_add_mod_list[0].freq_domain_res.from_number(
      0b111111110000000000000000000000000000000000000);
  pdcch_cfg_dedicated.setup().ctrl_res_set_to_add_mod_list[0].dur = 1;
  pdcch_cfg_dedicated.setup().ctrl_res_set_to_add_mod_list[0].cce_reg_map_type.set_non_interleaved();
  pdcch_cfg_dedicated.setup().ctrl_res_set_to_add_mod_list[0].precoder_granularity =
      asn1::rrc_nr::ctrl_res_set_s::precoder_granularity_opts::same_as_reg_bundle;

  // search spaces
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list.resize(1);
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].search_space_id                                = 2;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].ctrl_res_set_id_present                        = true;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].ctrl_res_set_id                                = 2;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].monitoring_slot_periodicity_and_offset_present = true;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].monitoring_slot_periodicity_and_offset.set_sl1();
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].monitoring_symbols_within_slot_present = true;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].monitoring_symbols_within_slot.from_number(
      0b10000000000000);
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].nrof_candidates_present = true;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level1 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level1_opts::n0;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level2 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level2_opts::n2;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level4 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level4_opts::n1;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level8 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level8_opts::n0;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level16 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level16_opts::n0;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].search_space_type_present = true;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].search_space_type.set_ue_specific();
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].search_space_type.ue_specific().dci_formats = asn1::
      rrc_nr::search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_opts::formats0_minus0_and_minus1_minus0;

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg_present = true;
  auto& pdsch_cfg_dedicated = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg;

  pdsch_cfg_dedicated.set_setup();
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a_present = true;
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a.set_setup();
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a.setup().dmrs_add_position_present = true;
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a.setup().dmrs_add_position =
      asn1::rrc_nr::dmrs_dl_cfg_s::dmrs_add_position_opts::pos1;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list.resize(1);
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].tci_state_id = 0;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].qcl_type1.ref_sig.set_ssb();
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].qcl_type1.ref_sig.ssb() = 0;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].qcl_type1.qcl_type =
      asn1::rrc_nr::qcl_info_s::qcl_type_opts::type_d;
  pdsch_cfg_dedicated.setup().res_alloc = pdsch_cfg_s::res_alloc_opts::res_alloc_type1;
  pdsch_cfg_dedicated.setup().rbg_size  = asn1::rrc_nr::pdsch_cfg_s::rbg_size_opts::cfg1;
  pdsch_cfg_dedicated.setup().prb_bundling_type.set_static_bundling();
  pdsch_cfg_dedicated.setup().prb_bundling_type.static_bundling().bundle_size_present = true;
  pdsch_cfg_dedicated.setup().prb_bundling_type.static_bundling().bundle_size =
      asn1::rrc_nr::pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_::bundle_size_opts::wideband;

  // ZP-CSI
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list.resize(1);
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].zp_csi_rs_res_id = 0;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.set_row4();
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.row4().from_number(0b100);
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p4;

  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.first_ofdm_symbol_in_time_domain = 8;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::fd_cdm2;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.density.set_one();

  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.start_rb     = 0;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.nrof_rbs     = 52;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset_present = true;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.set_slots80();
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.slots80() = 1;
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set_present                                       = true;
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set.set_setup();
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set.setup().zp_csi_rs_res_set_id = 0;
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set.setup().zp_csi_rs_res_id_list.resize(1);

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id         = 1;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg_present                 = true;

  // UL config dedicated
  // PUCCH
  auto& ul_config                         = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg;
  ul_config.init_ul_bwp_present           = true;
  ul_config.init_ul_bwp.pucch_cfg_present = true;
  ul_config.init_ul_bwp.pucch_cfg.set_setup();
  ul_config.init_ul_bwp.pucch_cfg.setup().format2_present = true;
  ul_config.init_ul_bwp.pucch_cfg.setup().format2.set_setup();
  ul_config.init_ul_bwp.pucch_cfg.setup().format2.setup().max_code_rate_present = true;
  ul_config.init_ul_bwp.pucch_cfg.setup().format2.setup().max_code_rate         = pucch_max_code_rate_opts::zero_dot25;

  // SR resources
  ul_config.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_add_mod_list.resize(1);
  auto& sr_res1                          = ul_config.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_add_mod_list[0];
  sr_res1.sched_request_res_id           = 1;
  sr_res1.sched_request_id               = 0;
  sr_res1.periodicity_and_offset_present = true;
  sr_res1.periodicity_and_offset.set_sl40();
  sr_res1.periodicity_and_offset.sl40() = 7;
  sr_res1.res_present                   = true;
  sr_res1.res                           = 0; // only PUCCH resource we have defined so far

  // DL data
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack.resize(5);
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[0] = 8;
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[1] = 7;
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[2] = 6;
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[3] = 5;
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[4] = 4;

  // PUCCH resources (only one format1 for the moment)
  ul_config.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list.resize(1);
  auto& pucch_res1        = ul_config.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list[0];
  pucch_res1.pucch_res_id = 0;
  pucch_res1.start_prb    = 0;
  pucch_res1.format.set_format1();
  pucch_res1.format.format1().init_cyclic_shift = 0;
  pucch_res1.format.format1().nrof_symbols      = 14;
  pucch_res1.format.format1().start_symbol_idx  = 0;
  pucch_res1.format.format1().time_domain_occ   = 0;

  // PUSCH config
  ul_config.init_ul_bwp.pusch_cfg_present = true;
  ul_config.init_ul_bwp.pusch_cfg.set_setup();
  auto& pusch_cfg_ded                                = ul_config.init_ul_bwp.pusch_cfg.setup();
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a_present = true;
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a.set_setup();
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a.setup().dmrs_add_position_present = true;
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a.setup().dmrs_add_position = dmrs_ul_cfg_s::dmrs_add_position_opts::pos1;
  // PUSH power control skipped
  pusch_cfg_ded.res_alloc = pusch_cfg_s::res_alloc_opts::res_alloc_type1;

  // UCI
  pusch_cfg_ded.uci_on_pusch_present = true;
  pusch_cfg_ded.uci_on_pusch.set_setup();
  pusch_cfg_ded.uci_on_pusch.setup().beta_offsets_present = true;
  pusch_cfg_ded.uci_on_pusch.setup().beta_offsets.set_semi_static();
  auto& beta_offset_semi_static                        = pusch_cfg_ded.uci_on_pusch.setup().beta_offsets.semi_static();
  beta_offset_semi_static.beta_offset_ack_idx1_present = true;
  beta_offset_semi_static.beta_offset_ack_idx1         = 9;
  beta_offset_semi_static.beta_offset_ack_idx2_present = true;
  beta_offset_semi_static.beta_offset_ack_idx2         = 9;
  beta_offset_semi_static.beta_offset_ack_idx3_present = true;
  beta_offset_semi_static.beta_offset_ack_idx3         = 9;
  beta_offset_semi_static.beta_offset_csi_part1_idx1_present = true;
  beta_offset_semi_static.beta_offset_csi_part1_idx2_present = true;
  beta_offset_semi_static.beta_offset_csi_part1_idx1         = 6;
  beta_offset_semi_static.beta_offset_csi_part1_idx2         = 6;
  beta_offset_semi_static.beta_offset_csi_part2_idx1_present = true;
  beta_offset_semi_static.beta_offset_csi_part2_idx1         = 6;
  beta_offset_semi_static.beta_offset_csi_part2_idx2_present = true;
  beta_offset_semi_static.beta_offset_csi_part2_idx2         = 6;
  pusch_cfg_ded.uci_on_pusch.setup().scaling                 = uci_on_pusch_s::scaling_opts::f1;

  ul_config.first_active_ul_bwp_id_present = true;
  ul_config.first_active_ul_bwp_id         = 0;

  // Serving cell config (only to setup)
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdcch_serving_cell_cfg_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdcch_serving_cell_cfg.set_setup();

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.set_setup();
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().nrof_harq_processes_for_pdsch_present =
      true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().nrof_harq_processes_for_pdsch =
      pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::n16;

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.set_setup();

  // nzp-CSI-RS Resource
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_to_add_mod_list.resize(1);
  auto& nzp_csi_res =
      cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_to_add_mod_list[0];
  nzp_csi_res.nzp_csi_rs_res_id = 0;
  nzp_csi_res.res_map.freq_domain_alloc.set_row2();
  nzp_csi_res.res_map.freq_domain_alloc.row2().from_number(0b100000000000);
  nzp_csi_res.res_map.nrof_ports                       = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
  nzp_csi_res.res_map.first_ofdm_symbol_in_time_domain = 4;
  nzp_csi_res.res_map.cdm_type                         = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
  nzp_csi_res.res_map.density.set_one();
  nzp_csi_res.res_map.freq_band.start_rb = 0;
  nzp_csi_res.res_map.freq_band.nrof_rbs = 52;
  nzp_csi_res.pwr_ctrl_offset            = 0;
  // Skip pwr_ctrl_offset_ss_present
  nzp_csi_res.scrambling_id                  = 0;
  nzp_csi_res.periodicity_and_offset_present = true;
  nzp_csi_res.periodicity_and_offset.set_slots80();
  nzp_csi_res.periodicity_and_offset.slots80() = 0;
  // optional
  nzp_csi_res.qcl_info_periodic_csi_rs_present = true;
  nzp_csi_res.qcl_info_periodic_csi_rs         = 0;

  // nzp-CSI-RS ResourceSet
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_set_to_add_mod_list.resize(1);
  auto& nzp_csi_res_set =
      cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_set_to_add_mod_list[0];
  nzp_csi_res_set.nzp_csi_res_set_id = 0;
  nzp_csi_res_set.nzp_csi_rs_res.resize(1);
  nzp_csi_res_set.nzp_csi_rs_res[0] = 0;
  // Skip TRS info

  // CSI report config
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().csi_report_cfg_to_add_mod_list.resize(1);
  auto& csi_report =
      cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().csi_report_cfg_to_add_mod_list[0];
  csi_report.report_cfg_id                       = 0;
  csi_report.res_for_ch_meas                     = 0;
  csi_report.csi_im_res_for_interference_present = true;
  csi_report.csi_im_res_for_interference         = 1;
  csi_report.report_cfg_type.set_periodic();
  csi_report.report_cfg_type.periodic().report_slot_cfg.set_slots80();
  csi_report.report_cfg_type.periodic().report_slot_cfg.slots80() = 8;
  csi_report.report_cfg_type.periodic().pucch_csi_res_list.resize(1);
  csi_report.report_cfg_type.periodic().pucch_csi_res_list[0].ul_bw_part_id = 0;
  csi_report.report_cfg_type.periodic().pucch_csi_res_list[0].pucch_res     = 17;
  csi_report.report_quant.set_cri_ri_pmi_cqi();
  csi_report.report_freq_cfg_present                = true;
  csi_report.report_freq_cfg.cqi_format_ind_present = true;
  csi_report.report_freq_cfg.cqi_format_ind =
      asn1::rrc_nr::csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::wideband_cqi;
  csi_report.time_restrict_for_ch_meass = asn1::rrc_nr::csi_report_cfg_s::time_restrict_for_ch_meass_opts::not_cfgured;
  csi_report.time_restrict_for_interference_meass =
      asn1::rrc_nr::csi_report_cfg_s::time_restrict_for_interference_meass_opts::not_cfgured;
  csi_report.group_based_beam_report.set_disabled();
  csi_report.cqi_table    = asn1::rrc_nr::csi_report_cfg_s::cqi_table_opts::table2;
  csi_report.subband_size = asn1::rrc_nr::csi_report_cfg_s::subband_size_opts::value1;

  // Reconfig with Sync
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync_present   = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.new_ue_id = 17943;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.smtc.release();
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.t304 = recfg_with_sync_s::t304_opts::ms1000;

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common_present           = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ss_pbch_block_pwr = 0;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dmrs_type_a_position =
      asn1::rrc_nr::serving_cell_cfg_common_s::dmrs_type_a_position_opts::pos2;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.pci_present                    = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.pci                            = 500;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_subcarrier_spacing_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_subcarrier_spacing =
      subcarrier_spacing_opts::khz30;

  // DL config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common_present              = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl
      .absolute_freq_ssb_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl.absolute_freq_ssb =
      632640;

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl.freq_band_list
      .push_back(78);
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl.absolute_freq_point_a =
      632316;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl
      .scs_specific_carrier_list.resize(1);
  auto& dl_carrier = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl
                         .scs_specific_carrier_list[0];
  dl_carrier.offset_to_carrier  = 0;
  dl_carrier.subcarrier_spacing = subcarrier_spacing_opts::khz15;
  dl_carrier.carrier_bw         = 52;

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params
      .location_and_bw = 14025;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params
      .subcarrier_spacing = subcarrier_spacing_opts::khz15;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp
      .pdcch_cfg_common_present = true;
  auto& pdcch_cfg_common =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common;
  pdcch_cfg_common.set_setup();
  pdcch_cfg_common.setup().ext                                 = false;
  pdcch_cfg_common.setup().common_ctrl_res_set_present         = true;
  pdcch_cfg_common.setup().common_ctrl_res_set.ctrl_res_set_id = 1;
  pdcch_cfg_common.setup().common_ctrl_res_set.freq_domain_res.from_number(
      0b111111110000000000000000000000000000000000000);
  pdcch_cfg_common.setup().common_ctrl_res_set.dur = 1;
  pdcch_cfg_common.setup().common_ctrl_res_set.cce_reg_map_type.set_non_interleaved();
  pdcch_cfg_common.setup().common_ctrl_res_set.precoder_granularity =
      asn1::rrc_nr::ctrl_res_set_s::precoder_granularity_opts::same_as_reg_bundle;

  // common search space list
  pdcch_cfg_common.setup().common_search_space_list.resize(1);
  pdcch_cfg_common.setup().common_search_space_list[0].search_space_id           = 1;
  pdcch_cfg_common.setup().common_search_space_list[0].ctrl_res_set_id_present   = true;
  pdcch_cfg_common.setup().common_search_space_list[0].ctrl_res_set_id           = 1;
  pdcch_cfg_common.setup().common_search_space_list[0].search_space_type_present = true;
  pdcch_cfg_common.setup().common_search_space_list[0].search_space_type.set_common();
  pdcch_cfg_common.setup()
      .common_search_space_list[0]
      .search_space_type.common()
      .dci_format0_minus0_and_format1_minus0_present                           = true;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates_present = true;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level1 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level1_opts::n1;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level2 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level2_opts::n1;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level4 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level4_opts::n1;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level8 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level8_opts::n0;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level16 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level16_opts::n0;
  pdcch_cfg_common.setup().common_search_space_list[0].monitoring_slot_periodicity_and_offset_present = true;
  pdcch_cfg_common.setup().common_search_space_list[0].monitoring_slot_periodicity_and_offset.set_sl1();
  pdcch_cfg_common.setup().common_search_space_list[0].monitoring_symbols_within_slot_present = true;
  pdcch_cfg_common.setup().common_search_space_list[0].monitoring_symbols_within_slot.from_number(0b10000000000000);
  pdcch_cfg_common.setup().ra_search_space_present = true;
  pdcch_cfg_common.setup().ra_search_space         = 1;

  // PDSCH config common
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp
      .pdsch_cfg_common_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdsch_cfg_common
      .set_setup();
  auto& pdsch_cfg_common = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp
                               .pdsch_cfg_common.setup();
  pdsch_cfg_common.pdsch_time_domain_alloc_list.resize(1);
  pdsch_cfg_common.pdsch_time_domain_alloc_list[0].map_type = pdsch_time_domain_res_alloc_s::map_type_opts::type_a;
  pdsch_cfg_common.pdsch_time_domain_alloc_list[0].start_symbol_and_len = 40;

  // UL config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.dummy = time_align_timer_opts::ms500;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul
      .scs_specific_carrier_list.resize(1);
  auto& ul_carrier = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul
                         .scs_specific_carrier_list[0];
  ul_carrier.offset_to_carrier  = 0;
  ul_carrier.subcarrier_spacing = subcarrier_spacing_opts::khz15;
  ul_carrier.carrier_bw         = 52;

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params
      .location_and_bw = 14025;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params
      .subcarrier_spacing = subcarrier_spacing_opts::khz15;

  // RACH config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common_present =
      true;
  auto& rach_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common;

  rach_cfg_common_pack.set_setup();
  rach_cfg_common_pack.setup().rach_cfg_generic.prach_cfg_idx             = 0;
  rach_cfg_common_pack.setup().rach_cfg_generic.msg1_fdm                  = rach_cfg_generic_s::msg1_fdm_opts::one;
  rach_cfg_common_pack.setup().rach_cfg_generic.msg1_freq_start           = 1;
  rach_cfg_common_pack.setup().rach_cfg_generic.zero_correlation_zone_cfg = 0;
  rach_cfg_common_pack.setup().rach_cfg_generic.preamb_rx_target_pwr      = -110;
  rach_cfg_common_pack.setup().rach_cfg_generic.preamb_trans_max =
      asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n7;
  rach_cfg_common_pack.setup().rach_cfg_generic.pwr_ramp_step =
      asn1::rrc_nr::rach_cfg_generic_s::pwr_ramp_step_opts::db4;
  rach_cfg_common_pack.setup().rach_cfg_generic.ra_resp_win = asn1::rrc_nr::rach_cfg_generic_s::ra_resp_win_opts::sl10;
  rach_cfg_common_pack.setup().ra_contention_resolution_timer =
      asn1::rrc_nr::rach_cfg_common_s::ra_contention_resolution_timer_opts::sf64;
  rach_cfg_common_pack.setup().prach_root_seq_idx.set(
      asn1::rrc_nr::rach_cfg_common_s::prach_root_seq_idx_c_::types_opts::l839);
  rach_cfg_common_pack.setup().prach_root_seq_idx.set_l839() = 1;
  rach_cfg_common_pack.setup().restricted_set_cfg =
      asn1::rrc_nr::rach_cfg_common_s::restricted_set_cfg_opts::unrestricted_set;

  // PUSCH config common
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp
      .pusch_cfg_common_present = true;
  auto& pusch_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common;
  pusch_cfg_common_pack.set_setup();
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list.resize(2);
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].k2_present = true;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].k2         = 4;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::type_a;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].start_symbol_and_len = 27;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].k2_present           = true;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].k2                   = 3;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::type_a;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].start_symbol_and_len = 27;
  pusch_cfg_common_pack.setup().p0_nominal_with_grant                                = -90;

  // PUCCH config common
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp
      .pucch_cfg_common_present = true;
  auto& pucch_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.pucch_cfg_common;
  pucch_cfg_common_pack.set_setup();
  pucch_cfg_common_pack.setup().pucch_group_hop    = asn1::rrc_nr::pucch_cfg_common_s::pucch_group_hop_opts::neither;
  pucch_cfg_common_pack.setup().p0_nominal_present = true;
  pucch_cfg_common_pack.setup().p0_nominal         = -90;

  // SSB config (optional)
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_positions_in_burst_present = true;
  auto& ssb_pos_in_burst = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_positions_in_burst;
  ssb_pos_in_burst.set_medium_bitmap().from_number(0b10000000);
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_periodicity_serving_cell_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_periodicity_serving_cell =
      serving_cell_cfg_common_s::ssb_periodicity_serving_cell_opts::ms20;

  // TDD UL-DL config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.tdd_ul_dl_cfg_common_present = true;
  auto& tdd_config = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.tdd_ul_dl_cfg_common;
  tdd_config.ref_subcarrier_spacing        = subcarrier_spacing_e::khz15;
  tdd_config.pattern1.dl_ul_tx_periodicity = asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms10;
  tdd_config.pattern1.nrof_dl_slots        = 6;
  tdd_config.pattern1.nrof_dl_symbols      = 0;
  tdd_config.pattern1.nrof_ul_slots        = 3;
  tdd_config.pattern1.nrof_ul_symbols      = 0;

  // pack only cell group info
  asn1::dyn_octstring packed_cell_group;
  packed_cell_group.resize(256);
  asn1::bit_ref bref_pack(packed_cell_group.data(), packed_cell_group.size());
  TESTASSERT(cell_group_cfg_pack.pack(bref_pack) == asn1::SRSASN_SUCCESS);
  TESTASSERT(test_pack_unpack_consistency(cell_group_cfg_pack) == SRSASN_SUCCESS);
  packed_cell_group.resize(bref_pack.distance_bytes());

#if JSON_OUTPUT
  asn1::json_writer json_writer2;
  cell_group_cfg_pack.to_json(json_writer2);
  srslog::fetch_basic_logger("RRC").info(packed_cell_group.data(),
                                         packed_cell_group.size(),
                                         "Cell group config repacked (%d B): \n %s",
                                         packed_cell_group.size(),
                                         json_writer2.to_string().c_str());
#endif

#if HAVE_PCAP
  // pack full DL-DCCH with RRC reconfig for PCAP output
  dl_dcch_msg_s dcch;
  dcch.msg.set_c1().set_rrc_recfg();
  rrc_recfg_s& reconfig       = dcch.msg.c1().rrc_recfg();
  reconfig.rrc_transaction_id = 0;
  reconfig.crit_exts.set_rrc_recfg();
  rrc_recfg_ies_s& recfg_ies             = reconfig.crit_exts.rrc_recfg();
  recfg_ies.secondary_cell_group_present = true;
  recfg_ies.secondary_cell_group         = packed_cell_group;

  asn1::dyn_octstring packed_dcch;
  packed_dcch.resize(1024);
  asn1::bit_ref bref_dcch_pack(packed_dcch.data(), packed_dcch.size());
  TESTASSERT(dcch.pack(bref_dcch_pack) == asn1::SRSASN_SUCCESS);
  packed_dcch.resize(bref_dcch_pack.distance_bytes() + 10);

  asn1::json_writer json_writer3;
  dcch.to_json(json_writer3);
  srslog::fetch_basic_logger("RRC").info(packed_dcch.data(),
                                         packed_dcch.size(),
                                         "Full DCCH repacked (%d B): \n %s",
                                         packed_dcch.size(),
                                         json_writer3.to_string().c_str());

  srsran::write_pdcp_sdu_nr(1, packed_dcch.data(), packed_dcch.size());
#endif

  return SRSRAN_SUCCESS;
}

int test_cell_group_config_fdd()
{
  uint8_t cell_group_config_raw[] = "\x5c\x40\xb1\xc0\x7d\x48\x3a\x04\xc0\x3e\x01\x04\x54\x1e\xb5\x80"
                                    "\x02\xe8\x53\xb8\x9f\x46\x85\x60\xa4\x00\x40\xab\x41\x00\x00\x00"
                                    "\xcd\x8d\xb2\x44\xa2\x01\xff\x00\x00\x00\x00\x01\x1b\x82\x21\x00"
                                    "\x01\x24\x04\x00\xd0\x14\x6c\x00\x10\x28\x9d\xc0\x00\x00\x33\x71"
                                    "\xb6\x48\x90\x04\x00\x08\x2e\x25\x18\xf0\x02\x4a\x31\x06\xe1\x8d"
                                    "\xb8\x44\x70\x01\x08\x4c\x23\x06\xdd\x40\x01\x01\xc0\x24\xb8\x19"
                                    "\x50\x00\x2f\xf0\x00\x00\x00\x00\x10\x6e\x11\x04\x00\x01\x10\x24"
                                    "\xa0\x04\x19\x04\x00\x00\x40\xd3\x02\x02\x8a\x14\x00\x1c\x90\x30"
                                    "\x00\x02\x66\xaa\xc9\x08\x38\x00\x20\x81\x84\x0a\x18\x39\x38\x81"
                                    "\x22\x85\x8c\x1a\x38\x79\x10\x00\x00\x85\x00\x00\x80\x0a\x50\x00"
                                    "\x10\x00\xc5\x00\x01\x80\x08\x50\x10\x20\x00\xa5\x01\x02\x80\x0c"
                                    "\x50\x10\x30\x00\x85\x02\x03\x80\x0a\x50\x20\x40\xcd\x04\x01\x23"
                                    "\x34\x12\x05\x0c\xd0\x50\x16\x33\x41\x60\x60\xcd\x06\x01\xa3\x34"
                                    "\x1a\x07\x0c\xd0\x70\x1e\x01\x41\x00\x80\x00\xc5\x02\x08\x80\x50"
                                    "\x4a\x04\x84\x30\x28\x42\x01\x22\x80\x14\x92\x1e\x2e\xe0\x0c\x10"
                                    "\xe0\x00\x00\x01\xff\xd2\x94\x98\xc6\x37\x28\x16\x00\x00\x21\x97"
                                    "\x00\x00\x00\x00\x00\x00\x06\x2f\x00\xfa\x08\x48\xad\x54\x50\x04"
                                    "\x70\x01\x80\x00\x82\x00\x0e\x21\x7d\x24\x08\x07\x01\x01\x08\x40"
                                    "\x00\xe2\x17\xd1\xcb\x00\xe0\x40\x22\x08\x00\x1c\x42\xfa\x39\x60"
                                    "\x1c\x0c\x04\x21\x00\x03\x88\x5f\x47\x30\x03\x82\x00\x88\x20\x00"
                                    "\x71\x0b\xe8\xe6\x00\x04\x00\x00\x00\x41\x0c\x04\x08\x0c\x10\x0e"
                                    "\x0d\x00\x00\xe4\x81\x00\x00\x00\x20\x04\x00\x08\x06\x00\x08\x09"
                                    "\x00\x22\x00\xa4\x00\x00\x23\x85\x01\x13\x1c";

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
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.pci_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.pci == 500);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common_present == true);
  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp_present == true);
  TESTASSERT(
      cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common_present ==
      true);

  TESTASSERT(cell_group_cfg.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common
                 .is_setup());

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

#if JSON_OUTPUT
  asn1::json_writer json_writer;
  cell_group_cfg.to_json(json_writer);
  srslog::fetch_basic_logger("RRC").info("RRC Secondary Cell Group: Content: %s\n", json_writer.to_string().c_str());
#endif

  // pack it again
  cell_group_cfg_s cell_group_cfg_pack;

  // RLC for DRB1
  cell_group_cfg_pack.rlc_bearer_to_add_mod_list.resize(1);
  auto& rlc                       = cell_group_cfg_pack.rlc_bearer_to_add_mod_list[0];
  rlc.lc_ch_id                    = 4;
  rlc.served_radio_bearer_present = true;
  rlc.served_radio_bearer.set_drb_id();
  rlc.served_radio_bearer.drb_id() = 1;
  rlc.rlc_cfg_present              = true;
  rlc.rlc_cfg.set_um_bi_dir();
  rlc.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len_present = true;
  rlc.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len         = sn_field_len_um_opts::size12;
  rlc.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len_present = true;
  rlc.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len         = sn_field_len_um_opts::size12;
  rlc.rlc_cfg.um_bi_dir().dl_um_rlc.t_reassembly         = t_reassembly_opts::ms50;

  // MAC logical channel config
  rlc.mac_lc_ch_cfg_present                    = true;
  rlc.mac_lc_ch_cfg.ul_specific_params_present = true;
  rlc.mac_lc_ch_cfg.ul_specific_params.prio    = 11;
  rlc.mac_lc_ch_cfg.ul_specific_params.prioritised_bit_rate =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::kbps0;
  rlc.mac_lc_ch_cfg.ul_specific_params.bucket_size_dur =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::ms100;
  rlc.mac_lc_ch_cfg.ul_specific_params.lc_ch_group_present      = true;
  rlc.mac_lc_ch_cfg.ul_specific_params.lc_ch_group              = 6;
  rlc.mac_lc_ch_cfg.ul_specific_params.sched_request_id_present = true;
  rlc.mac_lc_ch_cfg.ul_specific_params.sched_request_id         = 0;

  // mac-CellGroup-Config
  cell_group_cfg_pack.mac_cell_group_cfg_present = true;
  auto& mac_cell_group                           = cell_group_cfg_pack.mac_cell_group_cfg;
  mac_cell_group.sched_request_cfg_present       = true;
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list.resize(1);
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list[0].sched_request_id = 0;
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list[0].sr_trans_max =
      asn1::rrc_nr::sched_request_to_add_mod_s::sr_trans_max_opts::n64;
  mac_cell_group.bsr_cfg_present            = true;
  mac_cell_group.bsr_cfg.periodic_bsr_timer = asn1::rrc_nr::bsr_cfg_s::periodic_bsr_timer_opts::sf20;
  mac_cell_group.bsr_cfg.retx_bsr_timer     = asn1::rrc_nr::bsr_cfg_s::retx_bsr_timer_opts::sf320;
  // Skip TAG and PHR config

  cell_group_cfg_pack.sp_cell_cfg_present               = true;
  cell_group_cfg_pack.sp_cell_cfg.serv_cell_idx_present = true;

  // SP Cell Dedicated config
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded_present             = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp_present = true;

  // PDCCH config
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg_present = true;
  auto& pdcch_cfg_dedicated = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg;
  pdcch_cfg_dedicated.set_setup();
  pdcch_cfg_dedicated.setup().ctrl_res_set_to_add_mod_list.resize(1);
  pdcch_cfg_dedicated.setup().ctrl_res_set_to_add_mod_list[0].ctrl_res_set_id = 2;
  pdcch_cfg_dedicated.setup().ctrl_res_set_to_add_mod_list[0].freq_domain_res.from_number(
      0b111111110000000000000000000000000000000000000);
  pdcch_cfg_dedicated.setup().ctrl_res_set_to_add_mod_list[0].dur = 1;
  pdcch_cfg_dedicated.setup().ctrl_res_set_to_add_mod_list[0].cce_reg_map_type.set_non_interleaved();
  pdcch_cfg_dedicated.setup().ctrl_res_set_to_add_mod_list[0].precoder_granularity =
      asn1::rrc_nr::ctrl_res_set_s::precoder_granularity_opts::same_as_reg_bundle;

  // search spaces
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list.resize(1);
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].search_space_id                                = 2;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].ctrl_res_set_id_present                        = true;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].ctrl_res_set_id                                = 2;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].monitoring_slot_periodicity_and_offset_present = true;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].monitoring_slot_periodicity_and_offset.set_sl1();
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].monitoring_symbols_within_slot_present = true;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].monitoring_symbols_within_slot.from_number(
      0b10000000000000);
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].nrof_candidates_present = true;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level1 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level1_opts::n0;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level2 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level2_opts::n2;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level4 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level4_opts::n1;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level8 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level8_opts::n0;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level16 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level16_opts::n0;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].search_space_type_present = true;
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].search_space_type.set_ue_specific();
  pdcch_cfg_dedicated.setup().search_spaces_to_add_mod_list[0].search_space_type.ue_specific().dci_formats = asn1::
      rrc_nr::search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_opts::formats0_minus0_and_minus1_minus0;

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg_present = true;
  auto& pdsch_cfg_dedicated = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg;

  pdsch_cfg_dedicated.set_setup();
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a_present = true;
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a.set_setup();
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a.setup().dmrs_add_position_present = true;
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a.setup().dmrs_add_position =
      asn1::rrc_nr::dmrs_dl_cfg_s::dmrs_add_position_opts::pos1;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list.resize(1);
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].tci_state_id = 0;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].qcl_type1.ref_sig.set_ssb();
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].qcl_type1.ref_sig.ssb() = 0;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].qcl_type1.qcl_type =
      asn1::rrc_nr::qcl_info_s::qcl_type_opts::type_d;
  pdsch_cfg_dedicated.setup().res_alloc = pdsch_cfg_s::res_alloc_opts::res_alloc_type1;
  pdsch_cfg_dedicated.setup().rbg_size  = asn1::rrc_nr::pdsch_cfg_s::rbg_size_opts::cfg1;
  pdsch_cfg_dedicated.setup().prb_bundling_type.set_static_bundling();
  pdsch_cfg_dedicated.setup().prb_bundling_type.static_bundling().bundle_size_present = true;
  pdsch_cfg_dedicated.setup().prb_bundling_type.static_bundling().bundle_size =
      asn1::rrc_nr::pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_::bundle_size_opts::wideband;

  // ZP-CSI
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list.resize(1);
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].zp_csi_rs_res_id = 0;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.set_row4();
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.row4().from_number(0b100);
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p4;

  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.first_ofdm_symbol_in_time_domain = 8;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::fd_cdm2;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.density.set_one();

  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.start_rb     = 0;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.nrof_rbs     = 52;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset_present = true;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.set_slots80();
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.slots80() = 1;
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set_present                                       = true;
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set.set_setup();
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set.setup().zp_csi_rs_res_set_id = 0;
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set.setup().zp_csi_rs_res_id_list.resize(1);

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id         = 0;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg_present                 = true;

  // UL config dedicated
  // PUCCH
  auto& ul_config                         = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg;
  ul_config.init_ul_bwp_present           = true;
  ul_config.init_ul_bwp.pucch_cfg_present = true;
  ul_config.init_ul_bwp.pucch_cfg.set_setup();
  ul_config.init_ul_bwp.pucch_cfg.setup().format2_present = true;
  ul_config.init_ul_bwp.pucch_cfg.setup().format2.set_setup();
  ul_config.init_ul_bwp.pucch_cfg.setup().format2.setup().max_code_rate_present = true;
  ul_config.init_ul_bwp.pucch_cfg.setup().format2.setup().max_code_rate         = pucch_max_code_rate_opts::zero_dot25;

  // SR resources
  ul_config.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_add_mod_list.resize(1);
  auto& sr_res1                          = ul_config.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_add_mod_list[0];
  sr_res1.sched_request_res_id           = 1;
  sr_res1.sched_request_id               = 0;
  sr_res1.periodicity_and_offset_present = true;
  sr_res1.periodicity_and_offset.set_sl40();
  sr_res1.periodicity_and_offset.sl40() = 4;
  sr_res1.res_present                   = true;
  sr_res1.res                           = 16;

  // DL data
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack.resize(1);
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[0] = 4;

  // TODO?
  //  PUCCH resources (only one format1 for the moment)
  ul_config.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list.resize(1);
  auto& pucch_res1        = ul_config.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list[0];
  pucch_res1.pucch_res_id = 0;
  pucch_res1.start_prb    = 0;
  pucch_res1.format.set_format1();
  pucch_res1.format.format1().init_cyclic_shift = 0;
  pucch_res1.format.format1().nrof_symbols      = 14;
  pucch_res1.format.format1().start_symbol_idx  = 0;
  pucch_res1.format.format1().time_domain_occ   = 0;

  // PUSCH config
  ul_config.init_ul_bwp.pusch_cfg_present = true;
  ul_config.init_ul_bwp.pusch_cfg.set_setup();
  auto& pusch_cfg_ded                                = ul_config.init_ul_bwp.pusch_cfg.setup();
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a_present = true;
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a.set_setup();
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a.setup().dmrs_add_position_present = true;
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a.setup().dmrs_add_position = dmrs_ul_cfg_s::dmrs_add_position_opts::pos1;
  // PUSH power control skipped
  pusch_cfg_ded.res_alloc = pusch_cfg_s::res_alloc_opts::res_alloc_type1;

  // UCI
  pusch_cfg_ded.uci_on_pusch_present = true;
  pusch_cfg_ded.uci_on_pusch.set_setup();
  pusch_cfg_ded.uci_on_pusch.setup().beta_offsets_present = true;
  pusch_cfg_ded.uci_on_pusch.setup().beta_offsets.set_semi_static();
  auto& beta_offset_semi_static                        = pusch_cfg_ded.uci_on_pusch.setup().beta_offsets.semi_static();
  beta_offset_semi_static.beta_offset_ack_idx1_present = true;
  beta_offset_semi_static.beta_offset_ack_idx1         = 9;
  beta_offset_semi_static.beta_offset_ack_idx2_present = true;
  beta_offset_semi_static.beta_offset_ack_idx2         = 9;
  beta_offset_semi_static.beta_offset_ack_idx3_present = true;
  beta_offset_semi_static.beta_offset_ack_idx3         = 9;
  beta_offset_semi_static.beta_offset_csi_part1_idx1_present = true;
  beta_offset_semi_static.beta_offset_csi_part1_idx1         = 6;
  beta_offset_semi_static.beta_offset_csi_part1_idx2_present = true;
  beta_offset_semi_static.beta_offset_csi_part1_idx2         = 6;
  beta_offset_semi_static.beta_offset_csi_part2_idx1_present = true;
  beta_offset_semi_static.beta_offset_csi_part2_idx1         = 6;
  beta_offset_semi_static.beta_offset_csi_part2_idx2_present = true;
  beta_offset_semi_static.beta_offset_csi_part2_idx2         = 6;
  pusch_cfg_ded.uci_on_pusch.setup().scaling                 = uci_on_pusch_s::scaling_opts::f1;

  ul_config.first_active_ul_bwp_id_present = true;
  ul_config.first_active_ul_bwp_id         = 0;

  // Serving cell config (only to setup)
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdcch_serving_cell_cfg_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdcch_serving_cell_cfg.set_setup();

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.set_setup();
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().nrof_harq_processes_for_pdsch_present =
      true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().nrof_harq_processes_for_pdsch =
      pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::n16;

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.set_setup();

  // nzp-CSI-RS Resource
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_to_add_mod_list.resize(5);
  auto& nzp_csi_res = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup();
  // item 0
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].nzp_csi_rs_res_id = 0;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.set_row2();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.row2().from_number(0b100000000000);
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.first_ofdm_symbol_in_time_domain = 4;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.density.set_one();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.start_rb = 0;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.nrof_rbs = 52;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].pwr_ctrl_offset            = 0;
  // Skip pwr_ctrl_offset_ss_present
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].scrambling_id                  = 500;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.set_slots80();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.slots80() = 1;
  // optional
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].qcl_info_periodic_csi_rs_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].qcl_info_periodic_csi_rs         = 0;
  // item 1
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].nzp_csi_rs_res_id = 1;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.freq_domain_alloc.set_row1();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.freq_domain_alloc.row1().from_number(0b0001);
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.first_ofdm_symbol_in_time_domain = 4;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.density.set_three();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.freq_band.start_rb = 0;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.freq_band.nrof_rbs = 52;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].pwr_ctrl_offset            = 0;
  // Skip pwr_ctrl_offset_ss_present
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].scrambling_id                  = 500;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].periodicity_and_offset_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].periodicity_and_offset.set_slots40();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].periodicity_and_offset.slots40() = 11;
  // optional
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].qcl_info_periodic_csi_rs_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].qcl_info_periodic_csi_rs         = 0;
  // item 2
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].nzp_csi_rs_res_id = 2;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.freq_domain_alloc.set_row1();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.freq_domain_alloc.row1().from_number(0b0001);
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.first_ofdm_symbol_in_time_domain = 8;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.density.set_three();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.freq_band.start_rb = 0;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.freq_band.nrof_rbs = 52;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].pwr_ctrl_offset            = 0;
  // Skip pwr_ctrl_offset_ss_present
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].scrambling_id                  = 500;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].periodicity_and_offset_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].periodicity_and_offset.set_slots40();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].periodicity_and_offset.slots40() = 11;
  // optional
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].qcl_info_periodic_csi_rs_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].qcl_info_periodic_csi_rs         = 0;
  // item 3
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].nzp_csi_rs_res_id = 3;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.freq_domain_alloc.set_row1();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.freq_domain_alloc.row1().from_number(0b0001);
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.first_ofdm_symbol_in_time_domain = 4;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.density.set_three();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.freq_band.start_rb = 0;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.freq_band.nrof_rbs = 52;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].pwr_ctrl_offset            = 0;
  // Skip pwr_ctrl_offset_ss_present
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].scrambling_id                  = 500;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].periodicity_and_offset_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].periodicity_and_offset.set_slots40();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].periodicity_and_offset.slots40() = 12;
  // optional
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].qcl_info_periodic_csi_rs_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].qcl_info_periodic_csi_rs         = 0;
  // item 4
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].nzp_csi_rs_res_id = 4;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.freq_domain_alloc.set_row1();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.freq_domain_alloc.row1().from_number(0b0001);
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.first_ofdm_symbol_in_time_domain = 8;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.density.set_three();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.freq_band.start_rb = 0;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.freq_band.nrof_rbs = 52;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].pwr_ctrl_offset            = 0;
  // Skip pwr_ctrl_offset_ss_present
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].scrambling_id                  = 500;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].periodicity_and_offset_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].periodicity_and_offset.set_slots40();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].periodicity_and_offset.slots40() = 12;
  // optional
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].qcl_info_periodic_csi_rs_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].qcl_info_periodic_csi_rs         = 0;

  // nzp-CSI-RS ResourceSet
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_set_to_add_mod_list.resize(2);
  auto& nzp_csi_res_set = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup();
  // item 0
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[0].nzp_csi_res_set_id = 0;
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[0].nzp_csi_rs_res.resize(1);
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[0].nzp_csi_rs_res[0] = 0;
  // item 1
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[1].nzp_csi_res_set_id = 1;
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[1].nzp_csi_rs_res.resize(4);
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[1].nzp_csi_rs_res[0] = 1;
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[1].nzp_csi_rs_res[1] = 2;
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[1].nzp_csi_rs_res[2] = 3;
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[1].nzp_csi_rs_res[3] = 4;
  // Skip TRS info

  // CSI IM config
  // TODO: add csi im config

  // CSI resource config
  // TODO: add csi resource config

  // CSI report config
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().csi_report_cfg_to_add_mod_list.resize(1);
  auto& csi_report =
      cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().csi_report_cfg_to_add_mod_list[0];
  csi_report.report_cfg_id                       = 0;
  csi_report.res_for_ch_meas                     = 0;
  csi_report.csi_im_res_for_interference_present = true;
  csi_report.csi_im_res_for_interference         = 1;
  csi_report.report_cfg_type.set_periodic();
  csi_report.report_cfg_type.periodic().report_slot_cfg.set_slots80();
  csi_report.report_cfg_type.periodic().report_slot_cfg.slots80() = 5;
  csi_report.report_cfg_type.periodic().pucch_csi_res_list.resize(1);
  csi_report.report_cfg_type.periodic().pucch_csi_res_list[0].ul_bw_part_id = 0;
  csi_report.report_cfg_type.periodic().pucch_csi_res_list[0].pucch_res     = 17;
  csi_report.report_quant.set_cri_ri_pmi_cqi();
  csi_report.report_freq_cfg_present                = true;
  csi_report.report_freq_cfg.cqi_format_ind_present = true;
  csi_report.report_freq_cfg.cqi_format_ind =
      asn1::rrc_nr::csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::wideband_cqi;
  csi_report.time_restrict_for_ch_meass = asn1::rrc_nr::csi_report_cfg_s::time_restrict_for_ch_meass_opts::not_cfgured;
  csi_report.time_restrict_for_interference_meass =
      asn1::rrc_nr::csi_report_cfg_s::time_restrict_for_interference_meass_opts::not_cfgured;
  csi_report.group_based_beam_report.set_disabled();
  csi_report.cqi_table    = asn1::rrc_nr::csi_report_cfg_s::cqi_table_opts::table2;
  csi_report.subband_size = asn1::rrc_nr::csi_report_cfg_s::subband_size_opts::value1;

  // Reconfig with Sync
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync_present   = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.new_ue_id = 17933;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.t304      = recfg_with_sync_s::t304_opts::ms1000;

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common_present           = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ss_pbch_block_pwr = -36;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dmrs_type_a_position =
      asn1::rrc_nr::serving_cell_cfg_common_s::dmrs_type_a_position_opts::pos2;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.pci_present                    = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.pci                            = 500;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_subcarrier_spacing_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_subcarrier_spacing =
      subcarrier_spacing_opts::khz30;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.n_timing_advance_offset =
      asn1::rrc_nr::serving_cell_cfg_common_s::n_timing_advance_offset_opts::n0;

  // DL config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common_present              = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl
      .absolute_freq_ssb_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl.absolute_freq_ssb =
      176210;

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl.freq_band_list
      .push_back(5);
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl.absolute_freq_point_a =
      175364;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl
      .scs_specific_carrier_list.resize(1);
  auto& dl_carrier = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl
                         .scs_specific_carrier_list[0];
  dl_carrier.offset_to_carrier  = 0;
  dl_carrier.subcarrier_spacing = subcarrier_spacing_opts::khz15;
  dl_carrier.carrier_bw         = 52;

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params
      .location_and_bw = 14025;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params
      .subcarrier_spacing = subcarrier_spacing_opts::khz15;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp
      .pdcch_cfg_common_present = true;
  auto& pdcch_cfg_common =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common;
  pdcch_cfg_common.set_setup();
  pdcch_cfg_common.setup().common_ctrl_res_set_present         = true;
  pdcch_cfg_common.setup().common_ctrl_res_set.ctrl_res_set_id = 1;
  pdcch_cfg_common.setup().common_ctrl_res_set.freq_domain_res.from_number(
      0b111111110000000000000000000000000000000000000);
  pdcch_cfg_common.setup().common_ctrl_res_set.dur = 1;
  pdcch_cfg_common.setup().common_ctrl_res_set.cce_reg_map_type.set_non_interleaved();
  pdcch_cfg_common.setup().common_ctrl_res_set.precoder_granularity =
      asn1::rrc_nr::ctrl_res_set_s::precoder_granularity_opts::same_as_reg_bundle;

  // common search space list
  pdcch_cfg_common.setup().common_search_space_list.resize(1);
  pdcch_cfg_common.setup().common_search_space_list[0].search_space_id           = 1;
  pdcch_cfg_common.setup().common_search_space_list[0].ctrl_res_set_id_present   = true;
  pdcch_cfg_common.setup().common_search_space_list[0].ctrl_res_set_id           = 1;
  pdcch_cfg_common.setup().common_search_space_list[0].search_space_type_present = true;
  pdcch_cfg_common.setup().common_search_space_list[0].search_space_type.set_common();
  pdcch_cfg_common.setup()
      .common_search_space_list[0]
      .search_space_type.common()
      .dci_format0_minus0_and_format1_minus0_present                           = true;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates_present = true;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level1 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level1_opts::n1;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level2 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level2_opts::n1;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level4 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level4_opts::n1;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level8 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level8_opts::n0;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level16 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level16_opts::n0;
  pdcch_cfg_common.setup().common_search_space_list[0].monitoring_slot_periodicity_and_offset_present = true;
  pdcch_cfg_common.setup().common_search_space_list[0].monitoring_slot_periodicity_and_offset.set_sl1();
  pdcch_cfg_common.setup().common_search_space_list[0].monitoring_symbols_within_slot_present = true;
  pdcch_cfg_common.setup().common_search_space_list[0].monitoring_symbols_within_slot.from_number(0b10000000000000);
  pdcch_cfg_common.setup().ra_search_space_present = true;
  pdcch_cfg_common.setup().ra_search_space         = 1;

  // PDSCH config common
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp
      .pdsch_cfg_common_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdsch_cfg_common
      .set_setup();
  auto& pdsch_cfg_common = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp
                               .pdsch_cfg_common.setup();
  pdsch_cfg_common.pdsch_time_domain_alloc_list.resize(1);
  pdsch_cfg_common.pdsch_time_domain_alloc_list[0].map_type = pdsch_time_domain_res_alloc_s::map_type_opts::type_a;
  pdsch_cfg_common.pdsch_time_domain_alloc_list[0].start_symbol_and_len = 40;

  // UL config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.dummy = time_align_timer_opts::ms500;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul.freq_band_list
      .push_back(5);
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul.absolute_freq_point_a =
      166364;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul
      .scs_specific_carrier_list.resize(1);
  auto& ul_carrier = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul
                         .scs_specific_carrier_list[0];
  ul_carrier.offset_to_carrier  = 0;
  ul_carrier.subcarrier_spacing = subcarrier_spacing_opts::khz15;
  ul_carrier.carrier_bw         = 52;

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params
      .location_and_bw = 14025;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params
      .subcarrier_spacing = subcarrier_spacing_opts::khz15;

  // RACH config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common_present =
      true;
  auto& rach_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common;

  rach_cfg_common_pack.set_setup();
  rach_cfg_common_pack.setup().rach_cfg_generic.prach_cfg_idx             = 16;
  rach_cfg_common_pack.setup().rach_cfg_generic.msg1_fdm                  = rach_cfg_generic_s::msg1_fdm_opts::one;
  rach_cfg_common_pack.setup().rach_cfg_generic.msg1_freq_start           = 1;
  rach_cfg_common_pack.setup().rach_cfg_generic.zero_correlation_zone_cfg = 0;
  rach_cfg_common_pack.setup().rach_cfg_generic.preamb_rx_target_pwr      = -110;
  rach_cfg_common_pack.setup().rach_cfg_generic.preamb_trans_max =
      asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n7;
  rach_cfg_common_pack.setup().rach_cfg_generic.pwr_ramp_step =
      asn1::rrc_nr::rach_cfg_generic_s::pwr_ramp_step_opts::db4;
  rach_cfg_common_pack.setup().rach_cfg_generic.ra_resp_win = asn1::rrc_nr::rach_cfg_generic_s::ra_resp_win_opts::sl10;
  rach_cfg_common_pack.setup().ra_contention_resolution_timer =
      asn1::rrc_nr::rach_cfg_common_s::ra_contention_resolution_timer_opts::sf64;
  rach_cfg_common_pack.setup().prach_root_seq_idx.set(
      asn1::rrc_nr::rach_cfg_common_s::prach_root_seq_idx_c_::types_opts::l839);
  rach_cfg_common_pack.setup().prach_root_seq_idx.set_l839() = 1;
  rach_cfg_common_pack.setup().restricted_set_cfg =
      asn1::rrc_nr::rach_cfg_common_s::restricted_set_cfg_opts::unrestricted_set;

  // PUSCH config common
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp
      .pusch_cfg_common_present = true;
  auto& pusch_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common;
  pusch_cfg_common_pack.set_setup();
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list.resize(2);
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].k2_present = true;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].k2         = 4;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::type_a;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].start_symbol_and_len = 27;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].k2_present           = true;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].k2                   = 3;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::type_a;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].start_symbol_and_len = 27;
  pusch_cfg_common_pack.setup().p0_nominal_with_grant                                = -90;

  // PUCCH config common
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp
      .pucch_cfg_common_present = true;
  auto& pucch_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.pucch_cfg_common;
  pucch_cfg_common_pack.set_setup();
  pucch_cfg_common_pack.setup().pucch_group_hop    = asn1::rrc_nr::pucch_cfg_common_s::pucch_group_hop_opts::neither;
  pucch_cfg_common_pack.setup().p0_nominal_present = true;
  pucch_cfg_common_pack.setup().p0_nominal         = -90;

  // SSB config (optional)
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_positions_in_burst_present = true;
  auto& ssb_pos_in_burst = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_positions_in_burst;
  ssb_pos_in_burst.set_short_bitmap().from_number(0b1000);
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_periodicity_serving_cell_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_periodicity_serving_cell =
      serving_cell_cfg_common_s::ssb_periodicity_serving_cell_opts::ms20;

  // pack only cell group info
  asn1::dyn_octstring packed_cell_group;
  packed_cell_group.resize(256);
  asn1::bit_ref bref_pack(packed_cell_group.data(), packed_cell_group.size());
  TESTASSERT(cell_group_cfg_pack.pack(bref_pack) == asn1::SRSASN_SUCCESS);
  TESTASSERT(test_pack_unpack_consistency(cell_group_cfg_pack) == SRSASN_SUCCESS);
  packed_cell_group.resize(bref_pack.distance_bytes());

#if JSON_OUTPUT
  asn1::json_writer json_writer2;
  cell_group_cfg_pack.to_json(json_writer2);
  srslog::fetch_basic_logger("RRC").info(packed_cell_group.data(),
                                         packed_cell_group.size(),
                                         "Cell group config repacked (%d B): \n %s",
                                         packed_cell_group.size(),
                                         json_writer2.to_string().c_str());
#endif

#if HAVE_PCAP
  // pack full DL-DCCH with RRC reconfig for PCAP output
  dl_dcch_msg_s dcch;
  dcch.msg.set_c1().set_rrc_recfg();
  rrc_recfg_s& reconfig       = dcch.msg.c1().rrc_recfg();
  reconfig.rrc_transaction_id = 0;
  reconfig.crit_exts.set_rrc_recfg();
  rrc_recfg_ies_s& recfg_ies             = reconfig.crit_exts.rrc_recfg();
  recfg_ies.secondary_cell_group_present = true;
  recfg_ies.secondary_cell_group         = packed_cell_group;

  asn1::dyn_octstring packed_dcch;
  packed_dcch.resize(1024);
  asn1::bit_ref bref_dcch_pack(packed_dcch.data(), packed_dcch.size());
  TESTASSERT(dcch.pack(bref_dcch_pack) == asn1::SRSASN_SUCCESS);
  packed_dcch.resize(bref_dcch_pack.distance_bytes() + 10);

  asn1::json_writer json_writer3;
  dcch.to_json(json_writer3);
  srslog::fetch_basic_logger("RRC").info(packed_dcch.data(),
                                         packed_dcch.size(),
                                         "Full DCCH repacked (%d B): \n %s",
                                         packed_dcch.size(),
                                         json_writer3.to_string().c_str());

  srsran::write_pdcp_sdu_nr(1, packed_dcch.data(), packed_dcch.size());
#endif

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

#if HAVE_PCAP
  pcap_handle = std::unique_ptr<srsran::mac_pcap>(new srsran::mac_pcap());
  pcap_handle->open("srsran_asn1_rrc_nr_test.pcap");
#endif

  test_rrc_setup_complete();
  TESTASSERT(test_eutra_nr_capabilities() == SRSRAN_SUCCESS);
  TESTASSERT(test_ue_mrdc_capabilities() == SRSRAN_SUCCESS);
  TESTASSERT(test_ue_rrc_reconfiguration() == SRSRAN_SUCCESS);
  TESTASSERT(test_radio_bearer_config() == SRSRAN_SUCCESS);
  TESTASSERT(test_cell_group_config_tdd() == SRSRAN_SUCCESS);
  TESTASSERT(test_cell_group_config_fdd() == SRSRAN_SUCCESS);

  srslog::flush();

  printf("Success\n");

#if HAVE_PCAP
  if (pcap_handle) {
    pcap_handle->close();
  }
#endif

  return SRSRAN_SUCCESS;
}
