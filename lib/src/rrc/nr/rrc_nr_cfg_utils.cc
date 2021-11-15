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

#include "srsran/rrc/nr/rrc_nr_cfg_utils.h"
#include "srsran/asn1/rrc_nr.h"
#include "srsran/asn1/rrc_nr_utils.h"

using namespace asn1::rrc_nr;

namespace srsran {

void generate_default_pdcch_cfg_common(const basic_cell_args_t& args, pdcch_cfg_common_s& cfg)
{
  cfg.ctrl_res_set_zero_present   = true;
  cfg.ctrl_res_set_zero           = 0;
  cfg.common_ctrl_res_set_present = false;

  cfg.search_space_zero_present = true;
  cfg.search_space_zero         = 0;

  cfg.common_search_space_list_present = true;
  cfg.common_search_space_list.resize(1);
  search_space_s& ss                                = cfg.common_search_space_list[0];
  ss.search_space_id                                = 1;
  ss.ctrl_res_set_id_present                        = true;
  ss.ctrl_res_set_id                                = 0;
  ss.monitoring_slot_periodicity_and_offset_present = true;
  ss.monitoring_slot_periodicity_and_offset.set_sl1();
  ss.monitoring_symbols_within_slot_present = true;
  ss.monitoring_symbols_within_slot.from_number(0x2000);
  ss.nrof_candidates_present                   = true;
  ss.nrof_candidates.aggregation_level1.value  = search_space_s::nrof_candidates_s_::aggregation_level1_opts::n0;
  ss.nrof_candidates.aggregation_level2.value  = search_space_s::nrof_candidates_s_::aggregation_level2_opts::n0;
  ss.nrof_candidates.aggregation_level4.value  = search_space_s::nrof_candidates_s_::aggregation_level4_opts::n1;
  ss.nrof_candidates.aggregation_level8.value  = search_space_s::nrof_candidates_s_::aggregation_level8_opts::n0;
  ss.nrof_candidates.aggregation_level16.value = search_space_s::nrof_candidates_s_::aggregation_level16_opts::n0;
  ss.search_space_type_present                 = true;
  auto& common                                 = ss.search_space_type.set_common();
  common.dci_format0_minus0_and_format1_minus0_present = true;

  cfg.search_space_sib1_present           = true;
  cfg.search_space_sib1                   = 0;
  cfg.search_space_other_sys_info_present = true;
  cfg.search_space_other_sys_info         = 1;
  cfg.paging_search_space_present         = true;
  cfg.paging_search_space                 = 1;
  cfg.ra_search_space_present             = true;
  cfg.ra_search_space                     = 1;
}

void generate_default_pdsch_cfg_common(const basic_cell_args_t& args, pdsch_cfg_common_s& cfg)
{
  cfg.pdsch_time_domain_alloc_list_present = true;
  cfg.pdsch_time_domain_alloc_list.resize(1);
  cfg.pdsch_time_domain_alloc_list[0].map_type.value       = pdsch_time_domain_res_alloc_s::map_type_opts::type_a;
  cfg.pdsch_time_domain_alloc_list[0].start_symbol_and_len = 40;
}

void generate_default_init_dl_bwp(const basic_cell_args_t& args, bwp_dl_common_s& cfg)
{
  cfg.generic_params.location_and_bw = 14025;
  asn1::number_to_enum(cfg.generic_params.subcarrier_spacing, args.scs);

  cfg.pdcch_cfg_common_present = true;
  generate_default_pdcch_cfg_common(args, cfg.pdcch_cfg_common.set_setup());
  cfg.pdsch_cfg_common_present = true;
  generate_default_pdsch_cfg_common(args, cfg.pdsch_cfg_common.set_setup());
}

void generate_default_dl_cfg_common(dl_cfg_common_s& cfg, const basic_cell_args_t& args)
{
  cfg.init_dl_bwp_present = true;
  generate_default_init_dl_bwp(args, cfg.init_dl_bwp);
}

void generate_default_dl_cfg_common_sib(const basic_cell_args_t& args, dl_cfg_common_sib_s& cfg)
{
  cfg.freq_info_dl.freq_band_list.resize(1);
  cfg.freq_info_dl.freq_band_list[0].freq_band_ind_nr_present = true;
  cfg.freq_info_dl.freq_band_list[0].freq_band_ind_nr         = 20;
  cfg.freq_info_dl.offset_to_point_a                          = 24;
  cfg.freq_info_dl.scs_specific_carrier_list.resize(1);
  cfg.freq_info_dl.scs_specific_carrier_list[0].offset_to_carrier = 0;
  asn1::number_to_enum(cfg.freq_info_dl.scs_specific_carrier_list[0].subcarrier_spacing, args.scs);
  cfg.freq_info_dl.scs_specific_carrier_list[0].carrier_bw = args.nof_prbs;

  generate_default_init_dl_bwp(args, cfg.init_dl_bwp);
  // disable InitialBWP-Only fields
  cfg.init_dl_bwp.pdcch_cfg_common.setup().ctrl_res_set_zero_present = false;
  cfg.init_dl_bwp.pdcch_cfg_common.setup().search_space_zero_present = false;

  cfg.bcch_cfg.mod_period_coeff.value = bcch_cfg_s::mod_period_coeff_opts::n4;

  cfg.pcch_cfg.default_paging_cycle.value = paging_cycle_opts::rf128;
  cfg.pcch_cfg.nand_paging_frame_offset.set_one_t();
  cfg.pcch_cfg.ns.value = pcch_cfg_s::ns_opts::one;
}

void generate_default_rach_cfg_common(const basic_cell_args_t& args, rach_cfg_common_s& cfg)
{
  cfg.rach_cfg_generic.prach_cfg_idx                       = 16;
  cfg.rach_cfg_generic.msg1_fdm.value                      = rach_cfg_generic_s::msg1_fdm_opts::one;
  cfg.rach_cfg_generic.msg1_freq_start                     = 0;
  cfg.rach_cfg_generic.zero_correlation_zone_cfg           = 15;
  cfg.rach_cfg_generic.preamb_rx_target_pwr                = -110;
  cfg.rach_cfg_generic.preamb_trans_max.value              = rach_cfg_generic_s::preamb_trans_max_opts::n7;
  cfg.rach_cfg_generic.pwr_ramp_step.value                 = rach_cfg_generic_s::pwr_ramp_step_opts::db4;
  cfg.rach_cfg_generic.ra_resp_win.value                   = rach_cfg_generic_s::ra_resp_win_opts::sl10;
  cfg.ssb_per_rach_occasion_and_cb_preambs_per_ssb_present = true;
  cfg.ssb_per_rach_occasion_and_cb_preambs_per_ssb.set_one().value =
      rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_opts::n8;
  cfg.ra_contention_resolution_timer.value = rach_cfg_common_s::ra_contention_resolution_timer_opts::sf64;
  cfg.prach_root_seq_idx.set_l839()        = 1;
  cfg.restricted_set_cfg.value             = rach_cfg_common_s::restricted_set_cfg_opts::unrestricted_set;
}

void generate_default_ul_cfg_common_sib(const basic_cell_args_t& args, ul_cfg_common_sib_s& cfg)
{
  cfg.freq_info_ul.scs_specific_carrier_list.resize(1);
  cfg.freq_info_ul.scs_specific_carrier_list[0].offset_to_carrier = 0;
  asn1::number_to_enum(cfg.freq_info_ul.scs_specific_carrier_list[0].subcarrier_spacing, args.scs);
  cfg.freq_info_ul.scs_specific_carrier_list[0].carrier_bw = args.nof_prbs;

  cfg.init_ul_bwp.generic_params.location_and_bw = 14025;
  asn1::number_to_enum(cfg.init_ul_bwp.generic_params.subcarrier_spacing, args.scs);

  cfg.init_ul_bwp.rach_cfg_common_present = true;
  generate_default_rach_cfg_common(args, cfg.init_ul_bwp.rach_cfg_common.set_setup());

  cfg.init_ul_bwp.pusch_cfg_common_present   = true;
  pusch_cfg_common_s& pusch                  = cfg.init_ul_bwp.pusch_cfg_common.set_setup();
  pusch.pusch_time_domain_alloc_list_present = true;
  pusch.pusch_time_domain_alloc_list.resize(1);
  pusch.pusch_time_domain_alloc_list[0].k2_present           = true;
  pusch.pusch_time_domain_alloc_list[0].k2                   = 4;
  pusch.pusch_time_domain_alloc_list[0].map_type.value       = pusch_time_domain_res_alloc_s::map_type_opts::type_a;
  pusch.pusch_time_domain_alloc_list[0].start_symbol_and_len = 27;
  pusch.p0_nominal_with_grant_present                        = true;
  pusch.p0_nominal_with_grant                                = -76;

  cfg.init_ul_bwp.pucch_cfg_common_present = true;
  pucch_cfg_common_s& pucch                = cfg.init_ul_bwp.pucch_cfg_common.set_setup();
  pucch.pucch_res_common_present           = true;
  pucch.pucch_res_common                   = 11;
  pucch.pucch_group_hop.value              = pucch_cfg_common_s::pucch_group_hop_opts::neither;
  pucch.p0_nominal_present                 = true;
  pucch.p0_nominal                         = -90;

  cfg.time_align_timer_common.value = time_align_timer_opts::infinity;
}

void generate_default_serv_cell_cfg_common_sib(const basic_cell_args_t& args, serving_cell_cfg_common_sib_s& cfg)
{
  generate_default_dl_cfg_common_sib(args, cfg.dl_cfg_common);

  cfg.ul_cfg_common_present = true;
  generate_default_ul_cfg_common_sib(args, cfg.ul_cfg_common);

  cfg.ssb_positions_in_burst.in_one_group.from_number(0x80);

  cfg.ssb_periodicity_serving_cell.value = serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell_opts::ms20;

  cfg.ss_pbch_block_pwr = -16;
}

void generate_default_mib(uint32_t pdcch_scs, uint32_t coreset0_idx, mib_s& cfg)
{
  bool ret = asn1::number_to_enum(cfg.sub_carrier_spacing_common, pdcch_scs);
  srsran_assert(ret, "Invalid SCS=%d kHz", pdcch_scs);
  cfg.ssb_subcarrier_offset            = 0;
  cfg.intra_freq_resel.value           = mib_s::intra_freq_resel_opts::allowed;
  cfg.cell_barred.value                = mib_s::cell_barred_opts::not_barred;
  cfg.pdcch_cfg_sib1.search_space_zero = 0;
  cfg.pdcch_cfg_sib1.ctrl_res_set_zero = coreset0_idx;
  cfg.dmrs_type_a_position.value       = mib_s::dmrs_type_a_position_opts::pos2;
  cfg.sys_frame_num.from_number(0);
}

void generate_default_sib1(const basic_cell_args_t& args, sib1_s& cfg)
{
  cfg.cell_sel_info_present            = true;
  cfg.cell_sel_info.q_rx_lev_min       = -70;
  cfg.cell_sel_info.q_qual_min_present = true;
  cfg.cell_sel_info.q_qual_min         = -20;

  cfg.cell_access_related_info.plmn_id_list.resize(1);
  cfg.cell_access_related_info.plmn_id_list[0].plmn_id_list.resize(1);
  srsran::plmn_id_t plmn;
  plmn.from_string(args.plmn);
  srsran::to_asn1(&cfg.cell_access_related_info.plmn_id_list[0].plmn_id_list[0], plmn);
  cfg.cell_access_related_info.plmn_id_list[0].tac_present = true;
  cfg.cell_access_related_info.plmn_id_list[0].tac.from_number(args.tac);
  cfg.cell_access_related_info.plmn_id_list[0].cell_id.from_number(args.cell_id);
  cfg.cell_access_related_info.plmn_id_list[0].cell_reserved_for_oper.value =
      plmn_id_info_s::cell_reserved_for_oper_opts::not_reserved;

  cfg.conn_est_fail_ctrl_present                   = true;
  cfg.conn_est_fail_ctrl.conn_est_fail_count.value = conn_est_fail_ctrl_s::conn_est_fail_count_opts::n1;
  cfg.conn_est_fail_ctrl.conn_est_fail_offset_validity.value =
      conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::s30;
  cfg.conn_est_fail_ctrl.conn_est_fail_offset_present = true;
  cfg.conn_est_fail_ctrl.conn_est_fail_offset         = 1;

  //  cfg.si_sched_info_present                                  = true;
  //  cfg.si_sched_info.si_request_cfg.rach_occasions_si_present = true;
  //  cfg.si_sched_info.si_request_cfg.rach_occasions_si.rach_cfg_si.ra_resp_win.value =
  //      rach_cfg_generic_s::ra_resp_win_opts::sl8;
  //  cfg.si_sched_info.si_win_len.value = si_sched_info_s::si_win_len_opts::s20;
  //  cfg.si_sched_info.sched_info_list.resize(1);
  //  cfg.si_sched_info.sched_info_list[0].si_broadcast_status.value =
  //  sched_info_s::si_broadcast_status_opts::broadcasting; cfg.si_sched_info.sched_info_list[0].si_periodicity.value =
  //  sched_info_s::si_periodicity_opts::rf16; cfg.si_sched_info.sched_info_list[0].sib_map_info.resize(1);
  //  // scheduling of SI messages
  //  cfg.si_sched_info.sched_info_list[0].sib_map_info[0].type.value        = sib_type_info_s::type_opts::sib_type2;
  //  cfg.si_sched_info.sched_info_list[0].sib_map_info[0].value_tag_present = true;
  //  cfg.si_sched_info.sched_info_list[0].sib_map_info[0].value_tag         = 0;

  cfg.serving_cell_cfg_common_present = true;
  generate_default_serv_cell_cfg_common_sib(args, cfg.serving_cell_cfg_common);

  cfg.ue_timers_and_consts_present    = true;
  cfg.ue_timers_and_consts.t300.value = ue_timers_and_consts_s::t300_opts::ms1000;
  cfg.ue_timers_and_consts.t301.value = ue_timers_and_consts_s::t301_opts::ms1000;
  cfg.ue_timers_and_consts.t310.value = ue_timers_and_consts_s::t310_opts::ms1000;
  cfg.ue_timers_and_consts.n310.value = ue_timers_and_consts_s::n310_opts::n1;
  cfg.ue_timers_and_consts.t311.value = ue_timers_and_consts_s::t311_opts::ms30000;
  cfg.ue_timers_and_consts.n311.value = ue_timers_and_consts_s::n311_opts::n1;
  cfg.ue_timers_and_consts.t319.value = ue_timers_and_consts_s::t319_opts::ms1000;
}

} // namespace srsran
