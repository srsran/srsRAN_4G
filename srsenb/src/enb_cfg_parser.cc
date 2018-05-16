/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/asn1/liblte_common.h"
#include "srslte/asn1/liblte_rrc.h"
#include "srsenb/hdr/cfg_parser.h"
#include "srslte/srslte.h"

#include "srsenb/hdr/parser.h"
#include "enb_cfg_parser.h"

namespace srsenb {

int enb::parse_cell_cfg(all_args_t *args, srslte_cell_t *cell) {
  cell->id        = args->enb.pci;
  cell->cp        = SRSLTE_CP_NORM; 
  cell->nof_ports = args->enb.nof_ports;
  cell->nof_prb   = args->enb.n_prb;  
  
  LIBLTE_RRC_PHICH_CONFIG_STRUCT phichcfg; 
  
  parser::section phy_cnfg("phy_cnfg");
  parser::section phich_cnfg("phich_cnfg");
  phy_cnfg.add_subsection(&phich_cnfg);
  phich_cnfg.add_field(
    new parser::field_enum_str<LIBLTE_RRC_PHICH_DURATION_ENUM>
    ("duration", &phichcfg.dur,  liblte_rrc_phich_duration_text, LIBLTE_RRC_PHICH_DURATION_N_ITEMS)
  );
  phich_cnfg.add_field(
    new parser::field_enum_str<LIBLTE_RRC_PHICH_RESOURCE_ENUM>
    ("resources", &phichcfg.res,  liblte_rrc_phich_resource_text, LIBLTE_RRC_PHICH_RESOURCE_N_ITEMS)
  );
  parser::parse_section(args->enb_files.rr_config, &phy_cnfg);

  cell->phich_length    = (srslte_phich_length_t)    (int) phichcfg.dur;
  cell->phich_resources = (srslte_phich_resources_t) (int) phichcfg.res;
  
  if (!srslte_cell_isvalid(cell)) {
    fprintf(stderr, "Invalid cell parameters: nof_prb=%d, cell_id=%d\n", args->enb.n_prb, args->enb.s1ap.cell_id);
    return -1; 
  }
  
  return 0; 
}

int field_sched_info::parse(libconfig::Setting &root) 
{
  data->N_sched_info = root.getLength();
  for (uint32_t i=0;i<data->N_sched_info;i++) {    
    uint32_t periodicity = 0; 
    if (!root[i].lookupValue("si_periodicity", periodicity)) {
      fprintf(stderr, "Missing field si_periodicity in sched_info=%d\n", i);
      return -1; 
    }    
    int k=0;
    while(k<LIBLTE_RRC_SI_PERIODICITY_N_ITEMS && periodicity != liblte_rrc_si_periodicity_num[k])
      k++;
    if (k == LIBLTE_RRC_SI_PERIODICITY_N_ITEMS) {
      fprintf(stderr, "Invalid value %d for si_periodicity\n", periodicity);
      return -1; 
    }
    data->sched_info[i].si_periodicity = (LIBLTE_RRC_SI_PERIODICITY_ENUM) k; 
    if (root[i].exists("si_mapping_info")) {
      data->sched_info[i].N_sib_mapping_info = root[i]["si_mapping_info"].getLength();
      if (data->sched_info[i].N_sib_mapping_info < LIBLTE_RRC_MAX_SIB) {
        for (uint32_t j=0;j<data->sched_info[i].N_sib_mapping_info;j++) {
          uint32_t sib_index = root[i]["si_mapping_info"][j];
          if (sib_index >= 3 && sib_index <= 13) {
            data->sched_info[i].sib_mapping_info[j].sib_type = (LIBLTE_RRC_SIB_TYPE_ENUM) (sib_index-3);
          } else {
            fprintf(stderr, "Invalid SIB index %d for si_mapping_info=%d in sched_info=%d\n", sib_index, j, i);
            return -1; 
          }
        }    
      } else {
        fprintf(stderr, "Number of si_mapping_info values exceeds maximum (%d)\n", LIBLTE_RRC_MAX_SIB);
        return -1; 
      }
    } else {
      data->sched_info[i].N_sib_mapping_info = 0; 
    }
  }
  return 0; 
}


int field_intra_neigh_cell_list::parse(libconfig::Setting &root) 
{
  data->intra_freq_neigh_cell_list_size = root.getLength();
  for (uint32_t i=0;i<data->intra_freq_neigh_cell_list_size && i<LIBLTE_RRC_MAX_CELL_INTRA;i++) {    
    int q_offset_range = 0; 
    if (!root[i].lookupValue("q_offset_range", q_offset_range)) {
      fprintf(stderr, "Missing field q_offset_range in neigh_cell=%d\n", i);
      return -1; 
    }
    
    int k=0;
    while(k<LIBLTE_RRC_Q_OFFSET_RANGE_N_ITEMS && q_offset_range != liblte_rrc_q_offset_range_num[k])
      k++;
    if (k == LIBLTE_RRC_Q_OFFSET_RANGE_N_ITEMS) {
      fprintf(stderr, "Invalid value %d for q_offset_range\n", q_offset_range);
      return -1; 
    }
    data->intra_freq_neigh_cell_list[i].q_offset_range = (LIBLTE_RRC_Q_OFFSET_RANGE_ENUM) k;
    
    int phys_cell_id = 0; 
    if (!root[i].lookupValue("phys_cell_id", phys_cell_id)) {
      fprintf(stderr, "Missing field phys_cell_id in neigh_cell=%d\n", i);
      return -1; 
    }
    data->intra_freq_neigh_cell_list[i].phys_cell_id = (uint16) phys_cell_id;     
  }
  return 0; 
}

int field_intra_black_cell_list::parse(libconfig::Setting &root) 
{
  data->intra_freq_black_cell_list_size = root.getLength();
  for (uint32_t i=0;i<data->intra_freq_black_cell_list_size && i<LIBLTE_RRC_MAX_CELL_BLACK;i++) {    
    int range = 0; 
    if (!root[i].lookupValue("range", range)) {
      fprintf(stderr, "Missing field range in black_cell=%d\n", i);
      return -1; 
    }
    
    int k=0;
    while(k<LIBLTE_RRC_PHYS_CELL_ID_RANGE_N_ITEMS && range != liblte_rrc_phys_cell_id_range_num[k])
      k++;
    if (k == LIBLTE_RRC_PHYS_CELL_ID_RANGE_N_ITEMS) {
      fprintf(stderr, "Invalid value %d for range\n", range);
      return -1; 
    }
    data->intra_freq_black_cell_list[i].range = (LIBLTE_RRC_PHYS_CELL_ID_RANGE_ENUM) k;
    
    int start = 0; 
    if (!root[i].lookupValue("start", start)) {
      fprintf(stderr, "Missing field start in black_cell=%d\n", i);
      return -1; 
    }
    data->intra_freq_black_cell_list[i].start = (uint16) start;     
  }
  return 0; 
}


int enb::parse_sib1(std::string filename, LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *data) 
{  
  parser::section sib1("sib1");

  sib1.add_field(
    new parser::field_enum_str<LIBLTE_RRC_INTRA_FREQ_RESELECTION_ENUM>
    ("intra_freq_reselection", &data->intra_freq_reselection,  liblte_rrc_intra_freq_reselection_text, LIBLTE_RRC_INTRA_FREQ_RESELECTION_N_ITEMS)
  );
  sib1.add_field(
    new parser::field<int16>("q_rx_lev_min", &data->q_rx_lev_min)
  );  
  sib1.add_field(
    new parser::field<int8>("p_max", &data->p_max, &data->p_max_present)
  );  
  sib1.add_field(
    new parser::field_enum_str<LIBLTE_RRC_CELL_BARRED_ENUM>
    ("cell_barred", &data->cell_barred,  liblte_rrc_cell_barred_text, LIBLTE_RRC_CELL_BARRED_N_ITEMS)
  );
  sib1.add_field(
    new parser::field_enum_num<LIBLTE_RRC_SI_WINDOW_LENGTH_ENUM,uint8_t>
    ("si_window_length", &data->si_window_length,  liblte_rrc_si_window_length_num, LIBLTE_RRC_SI_WINDOW_LENGTH_N_ITEMS)
  );
  sib1.add_field(
    new parser::field<uint8_t>("system_info_value_tag", &data->system_info_value_tag)
  );
  
  // sched_info subsection uses a custom field class 
  parser::section sched_info("sched_info");
  sib1.add_subsection(&sched_info);
  sched_info.add_field(new field_sched_info(data));
  
  // Run parser with single section 
  return parser::parse_section(filename, &sib1);
}

int enb::parse_sib2(std::string filename, LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *data) 
{  
  parser::section sib2("sib2");

  sib2.add_field(
    new parser::field_enum_str<LIBLTE_RRC_TIME_ALIGNMENT_TIMER_ENUM>
    ("time_alignment_timer", &data->time_alignment_timer, 
     liblte_rrc_time_alignment_timer_text, LIBLTE_RRC_TIME_ALIGNMENT_TIMER_N_ITEMS)
  );
  
  
  sib2.add_field(
     new parser::field<uint32>
    ("mbsfnSubframeConfigListLength", &data->mbsfn_subfr_cnfg_list_size)
  );
  
  
    parser::section mbsfnSubframeConfigList("mbsfnSubframeConfigList");
      sib2.add_subsection(&mbsfnSubframeConfigList);
      
    mbsfnSubframeConfigList.add_field( 
       new parser::field<uint32>
      ("subframeAllocation", &data->mbsfn_subfr_cnfg_list[0].subfr_alloc)
    );

    mbsfnSubframeConfigList.add_field( 
       new parser::field<uint8>
      ("radioframeAllocationOffset", &data->mbsfn_subfr_cnfg_list[0].radio_fr_alloc_offset)
    );

    mbsfnSubframeConfigList.add_field( 
       new parser::field_enum_str<LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_ENUM>
      ("subframeAllocationNumFrames", &data->mbsfn_subfr_cnfg_list[0].subfr_alloc_num_frames,
       liblte_rrc_subframe_allocation_num_frames_text,LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_N_ITEMS)
    );

    mbsfnSubframeConfigList.add_field( 
       new parser::field_enum_str<LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_ENUM>
      ("radioframeAllocationPeriod", &data->mbsfn_subfr_cnfg_list[0].radio_fr_alloc_period,
       liblte_rrc_radio_frame_allocation_period_text, LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N_ITEMS)
    );
 
  parser::section freqinfo("freqInfo");
  sib2.add_subsection(&freqinfo);
  freqinfo.add_field(
    new parser::field<uint8>
    ("additional_spectrum_emission", &data->additional_spectrum_emission)
  );
  freqinfo.add_field(
    new parser::field<bool> ("ul_carrier_freq_present", &data->arfcn_value_eutra.present)
  );
  freqinfo.add_field(
    new parser::field<bool> ("ul_bw_present", &data->ul_bw.present)
  );
  
  // AC barring configuration 
  parser::section acbarring("ac_barring");
  sib2.add_subsection(&acbarring);
  acbarring.set_optional(&data->ac_barring_info_present);

  acbarring.add_field(
    new parser::field<bool>("ac_barring_for_emergency", &data->ac_barring_for_emergency)
  );
  
  parser::section acbarring_signalling("ac_barring_for_mo_signalling");
  acbarring.add_subsection(&acbarring_signalling);
  acbarring_signalling.set_optional(&data->ac_barring_for_mo_signalling.enabled);
  
  acbarring_signalling.add_field(
    new parser::field_enum_num<LIBLTE_RRC_AC_BARRING_FACTOR_ENUM,double>
    ("factor", &data->ac_barring_for_mo_signalling.factor, 
     liblte_rrc_ac_barring_factor_num, LIBLTE_RRC_AC_BARRING_FACTOR_N_ITEMS)
  );
  acbarring_signalling.add_field(
    new parser::field_enum_num<LIBLTE_RRC_AC_BARRING_TIME_ENUM,uint16>
    ("time", &data->ac_barring_for_mo_signalling.time, 
     liblte_rrc_ac_barring_time_num, LIBLTE_RRC_AC_BARRING_FACTOR_N_ITEMS)
  );
  acbarring_signalling.add_field(
    new parser::field<uint8>("for_special_ac", &data->ac_barring_for_mo_signalling.for_special_ac)
  );

  parser::section acbarring_data("ac_barring_for_mo_data");
  acbarring.add_subsection(&acbarring_data);
  acbarring_data.set_optional(&data->ac_barring_for_mo_data.enabled);
  
  acbarring_data.add_field(
    new parser::field_enum_num<LIBLTE_RRC_AC_BARRING_FACTOR_ENUM,double>
    ("factor", &data->ac_barring_for_mo_data.factor, 
     liblte_rrc_ac_barring_factor_num, LIBLTE_RRC_AC_BARRING_FACTOR_N_ITEMS)
  );
  acbarring_data.add_field(
    new parser::field_enum_num<LIBLTE_RRC_AC_BARRING_TIME_ENUM,uint16>
    ("time", &data->ac_barring_for_mo_data.time, 
     liblte_rrc_ac_barring_time_num, LIBLTE_RRC_AC_BARRING_FACTOR_N_ITEMS)
  );
  acbarring_data.add_field(
    new parser::field<uint8>("for_special_ac", &data->ac_barring_for_mo_data.for_special_ac)
  );
  
  
  // UE timers and constants 
  parser::section uetimers("ue_timers_and_constants");
  sib2.add_subsection(&uetimers);
  uetimers.add_field(
    new parser::field_enum_num<LIBLTE_RRC_T300_ENUM,uint16>
    ("t300", &data->ue_timers_and_constants.t300, liblte_rrc_t300_num, LIBLTE_RRC_T300_N_ITEMS)
  );
  uetimers.add_field(
    new parser::field_enum_num<LIBLTE_RRC_T301_ENUM,uint16>
    ("t301", &data->ue_timers_and_constants.t301, liblte_rrc_t301_num, LIBLTE_RRC_T301_N_ITEMS)
  );
  uetimers.add_field(
    new parser::field_enum_num<LIBLTE_RRC_T310_ENUM,uint16>
    ("t310", &data->ue_timers_and_constants.t310, liblte_rrc_t310_num, LIBLTE_RRC_T310_N_ITEMS)
  );
  uetimers.add_field(
    new parser::field_enum_num<LIBLTE_RRC_N310_ENUM,uint8>
    ("n310", &data->ue_timers_and_constants.n310, liblte_rrc_n310_num, LIBLTE_RRC_N310_N_ITEMS)
  );
  uetimers.add_field(
    new parser::field_enum_num<LIBLTE_RRC_T311_ENUM,uint16>
    ("t311", &data->ue_timers_and_constants.t311, liblte_rrc_t311_num, LIBLTE_RRC_T311_N_ITEMS)
  );
  uetimers.add_field(
    new parser::field_enum_num<LIBLTE_RRC_N311_ENUM,uint8>
    ("n311", &data->ue_timers_and_constants.n311, liblte_rrc_n311_num, LIBLTE_RRC_N311_N_ITEMS)
  );
  
  
  
  
  // Radio-resource configuration section 
  parser::section rr_config("rr_config_common_sib");
  sib2.add_subsection(&rr_config);
  
  rr_config.add_field(
    new parser::field_enum_str<LIBLTE_RRC_UL_CP_LENGTH_ENUM>
    ("ul_cp_length", &data->rr_config_common_sib.ul_cp_length, 
     liblte_rrc_ul_cp_length_text, LIBLTE_RRC_UL_CP_LENGTH_N_ITEMS)
  );
  
  // RACH configuration 
  parser::section rach_cnfg("rach_cnfg");
  rr_config.add_subsection(&rach_cnfg);

  rach_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_ENUM,uint8>
    ("num_ra_preambles", &data->rr_config_common_sib.rach_cnfg.num_ra_preambles, 
     liblte_rrc_number_of_ra_preambles_num, LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N_ITEMS)
  );
  rach_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_ENUM,int8>
    ("preamble_init_rx_target_pwr", &data->rr_config_common_sib.rach_cnfg.preamble_init_rx_target_pwr, 
     liblte_rrc_preamble_initial_received_target_power_num, LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_N_ITEMS)
  );
  rach_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_POWER_RAMPING_STEP_ENUM,uint8>
    ("pwr_ramping_step", &data->rr_config_common_sib.rach_cnfg.pwr_ramping_step, 
     liblte_rrc_power_ramping_step_num, LIBLTE_RRC_POWER_RAMPING_STEP_N_ITEMS)
  );
  rach_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_PREAMBLE_TRANS_MAX_ENUM,uint8>
    ("preamble_trans_max", &data->rr_config_common_sib.rach_cnfg.preamble_trans_max, 
     liblte_rrc_preamble_trans_max_num, LIBLTE_RRC_PREAMBLE_TRANS_MAX_N_ITEMS)
  );
  rach_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_ENUM,uint8>
    ("ra_resp_win_size", &data->rr_config_common_sib.rach_cnfg.ra_resp_win_size, 
     liblte_rrc_ra_response_window_size_num, LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_N_ITEMS)
  );
  rach_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_ENUM,uint8>
    ("mac_con_res_timer", &data->rr_config_common_sib.rach_cnfg.mac_con_res_timer, 
     liblte_rrc_mac_contention_resolution_timer_num, LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_N_ITEMS)
  );
  rach_cnfg.add_field(
    new parser::field<uint8>("max_harq_msg3_tx", &data->rr_config_common_sib.rach_cnfg.max_harq_msg3_tx)
  );
    
  parser::section groupa_cnfg("preambles_group_a_cnfg");
  rach_cnfg.add_subsection(&groupa_cnfg);
  groupa_cnfg.set_optional(&data->rr_config_common_sib.rach_cnfg.preambles_group_a_cnfg.present);
  groupa_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_ENUM,uint8>
    ("size_of_ra", &data->rr_config_common_sib.rach_cnfg.preambles_group_a_cnfg.size_of_ra, 
     liblte_rrc_size_of_ra_preambles_group_a_num, LIBLTE_RRC_SIZE_OF_RA_PREAMBLES_GROUP_A_N_ITEMS)
  );
  groupa_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_MESSAGE_SIZE_GROUP_A_ENUM,uint16>
    ("msg_size", &data->rr_config_common_sib.rach_cnfg.preambles_group_a_cnfg.msg_size, 
     liblte_rrc_message_size_group_a_num, LIBLTE_RRC_MESSAGE_SIZE_GROUP_A_N_ITEMS)
  );
  groupa_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_ENUM,int>
    ("msg_pwr_offset_group_b", &data->rr_config_common_sib.rach_cnfg.preambles_group_a_cnfg.msg_pwr_offset_group_b, 
     liblte_rrc_message_power_offset_group_b_num, LIBLTE_RRC_MESSAGE_POWER_OFFSET_GROUP_B_N_ITEMS)
  );
  

  
  // BCCH configuration 
  parser::section bcch_cnfg("bcch_cnfg");
  rr_config.add_subsection(&bcch_cnfg);
  bcch_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_ENUM,uint8>
    ("modification_period_coeff", &data->rr_config_common_sib.bcch_cnfg.modification_period_coeff, 
     liblte_rrc_modification_period_coeff_num, LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_N_ITEMS)
  );
  
  // PCCH configuration 
  parser::section pcch_cnfg("pcch_cnfg");
  rr_config.add_subsection(&pcch_cnfg);
  pcch_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_DEFAULT_PAGING_CYCLE_ENUM,uint16>
    ("default_paging_cycle", &data->rr_config_common_sib.pcch_cnfg.default_paging_cycle, 
     liblte_rrc_default_paging_cycle_num, LIBLTE_RRC_DEFAULT_PAGING_CYCLE_N_ITEMS)
  );
  pcch_cnfg.add_field(
    new parser::field_enum_str<LIBLTE_RRC_NB_ENUM>
    ("nB", &data->rr_config_common_sib.pcch_cnfg.nB, 
     liblte_rrc_nb_text, LIBLTE_RRC_NB_N_ITEMS)
  );

  // PRACH configuration 
  parser::section prach_cnfg("prach_cnfg");
  rr_config.add_subsection(&prach_cnfg);
  prach_cnfg.add_field(
    new parser::field<uint16>("root_sequence_index", &data->rr_config_common_sib.prach_cnfg.root_sequence_index)
  );
  parser::section prach_cnfg_info("prach_cnfg_info");
  prach_cnfg.add_subsection(&prach_cnfg_info);
  prach_cnfg_info.add_field(
    new parser::field<bool>("high_speed_flag", &data->rr_config_common_sib.prach_cnfg.prach_cnfg_info.high_speed_flag)
  );
  prach_cnfg_info.add_field(
    new parser::field<uint8>("prach_config_index", &data->rr_config_common_sib.prach_cnfg.prach_cnfg_info.prach_config_index)
  );
  prach_cnfg_info.add_field(
    new parser::field<uint8>("prach_freq_offset", &data->rr_config_common_sib.prach_cnfg.prach_cnfg_info.prach_freq_offset)
  );
  prach_cnfg_info.add_field(
    new parser::field<uint8>("zero_correlation_zone_config", &data->rr_config_common_sib.prach_cnfg.prach_cnfg_info.zero_correlation_zone_config)
  );
  
  // PDSCH configuration 
  parser::section pdsch_cnfg("pdsch_cnfg");
  rr_config.add_subsection(&pdsch_cnfg);
  pdsch_cnfg.add_field(
    new parser::field<uint8>("p_b", &data->rr_config_common_sib.pdsch_cnfg.p_b)
  );
  pdsch_cnfg.add_field(
    new parser::field<int8>("rs_power", &data->rr_config_common_sib.pdsch_cnfg.rs_power)
  );
  
  // PUSCH configuration 
  parser::section pusch_cnfg("pusch_cnfg");
  rr_config.add_subsection(&pusch_cnfg);
  pusch_cnfg.add_field(
    new parser::field<uint8>("n_sb", &data->rr_config_common_sib.pusch_cnfg.n_sb)
  );
  pusch_cnfg.add_field(
    new parser::field_enum_str<LIBLTE_RRC_HOPPING_MODE_ENUM>
    ("hopping_mode", &data->rr_config_common_sib.pusch_cnfg.hopping_mode, 
     liblte_rrc_hopping_mode_text, LIBLTE_RRC_HOOPPING_MODE_N_ITEMS)
  );
  pusch_cnfg.add_field(
    new parser::field<uint8>
    ("pusch_hopping_offset", &data->rr_config_common_sib.pusch_cnfg.pusch_hopping_offset)
  );
  pusch_cnfg.add_field(
    new parser::field<bool>
    ("enable_64_qam", &data->rr_config_common_sib.pusch_cnfg.enable_64_qam)
  );

  // PUSCH-ULRS configuration 
  parser::section ulrs_cnfg("ul_rs");
  pusch_cnfg.add_subsection(&ulrs_cnfg);
  ulrs_cnfg.add_field(
    new parser::field<uint8>
    ("cyclic_shift", &data->rr_config_common_sib.pusch_cnfg.ul_rs.cyclic_shift)
  );
  ulrs_cnfg.add_field(
    new parser::field<uint8>
    ("group_assignment_pusch", &data->rr_config_common_sib.pusch_cnfg.ul_rs.group_assignment_pusch)
  );
  ulrs_cnfg.add_field(
    new parser::field<bool>
    ("group_hopping_enabled", &data->rr_config_common_sib.pusch_cnfg.ul_rs.group_hopping_enabled)
  );
  ulrs_cnfg.add_field(
    new parser::field<bool>
    ("sequence_hopping_enabled", &data->rr_config_common_sib.pusch_cnfg.ul_rs.sequence_hopping_enabled)
  );

  // PUCCH configuration 
  parser::section pucch_cnfg("pucch_cnfg");
  rr_config.add_subsection(&pucch_cnfg);
  pucch_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_DELTA_PUCCH_SHIFT_ENUM,uint8>
    ("delta_pucch_shift", &data->rr_config_common_sib.pucch_cnfg.delta_pucch_shift, 
      liblte_rrc_delta_pucch_shift_num,LIBLTE_RRC_DELTA_PUCCH_SHIFT_N_ITEMS)
  );
  pucch_cnfg.add_field(
    new parser::field<uint8>
    ("n_rb_cqi", &data->rr_config_common_sib.pucch_cnfg.n_rb_cqi)
  );
  pucch_cnfg.add_field(
    new parser::field<uint8>
    ("n_cs_an", &data->rr_config_common_sib.pucch_cnfg.n_cs_an)
  );
  pucch_cnfg.add_field(
    new parser::field<uint16>
    ("n1_pucch_an", &data->rr_config_common_sib.pucch_cnfg.n1_pucch_an)
  );

  // UL PWR Ctrl configuration 
  parser::section ul_pwr_ctrl("ul_pwr_ctrl");
  rr_config.add_subsection(&ul_pwr_ctrl);
  ul_pwr_ctrl.add_field(
    new parser::field<int8>
    ("p0_nominal_pusch", &data->rr_config_common_sib.ul_pwr_ctrl.p0_nominal_pusch)
  );
  ul_pwr_ctrl.add_field(
    new parser::field_enum_num<LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_ENUM,double>
    ("alpha", &data->rr_config_common_sib.ul_pwr_ctrl.alpha, 
     liblte_rrc_ul_power_control_alpha_num, LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_N_ITEMS)
  );
  ul_pwr_ctrl.add_field(
    new parser::field<int8>
    ("p0_nominal_pucch", &data->rr_config_common_sib.ul_pwr_ctrl.p0_nominal_pucch)
  );
  ul_pwr_ctrl.add_field(
    new parser::field<int8>
    ("delta_preamble_msg3", &data->rr_config_common_sib.ul_pwr_ctrl.delta_preamble_msg3)
  );

  // Delta Flist PUCCH 
  parser::section delta_flist("delta_flist_pucch");
  ul_pwr_ctrl.add_subsection(&delta_flist);
  delta_flist.add_field(
    new parser::field_enum_num<LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_ENUM,int8>
    ("format_1", &data->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_1, 
     liblte_rrc_delta_f_pucch_format_1_num, LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_N_ITEMS)
  );
  delta_flist.add_field(
    new parser::field_enum_num<LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_ENUM,uint8>
    ("format_1b", &data->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_1b, 
     liblte_rrc_delta_f_pucch_format_1b_num, LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_N_ITEMS)
  );
  delta_flist.add_field(
    new parser::field_enum_num<LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_ENUM,int8>
    ("format_2", &data->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_2, 
     liblte_rrc_delta_f_pucch_format_2_num, LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_N_ITEMS)
  );
  delta_flist.add_field(
    new parser::field_enum_num<LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_ENUM,int8>
    ("format_2a", &data->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_2a, 
     liblte_rrc_delta_f_pucch_format_2a_num, LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_N_ITEMS)
  );
  delta_flist.add_field(
    new parser::field_enum_num<LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_ENUM,int8>
    ("format_2b", &data->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_2b, 
     liblte_rrc_delta_f_pucch_format_2b_num, LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_N_ITEMS)
  );

  // Run parser with single section 
  return parser::parse_section(filename, &sib2);
}

int enb::parse_sib3(std::string filename, LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT *data) 
{  
  parser::section sib3("sib3");
  
  // CellReselectionInfoCommon
  parser::section resel_common("cell_reselection_common");
  sib3.add_subsection(&resel_common);

  resel_common.add_field(
    new parser::field_enum_num<LIBLTE_RRC_Q_HYST_ENUM,uint8>
    ("q_hyst", &data->q_hyst, 
     liblte_rrc_q_hyst_num, LIBLTE_RRC_Q_HYST_N_ITEMS)
  );
  
  parser::section speed_resel("speed_state_resel_params");
  resel_common.add_subsection(&speed_resel);
  resel_common.set_optional(&data->speed_state_resel_params.present);
  
  
  parser::section q_hyst_sf("q_hyst_sf");
  speed_resel.add_subsection(&q_hyst_sf);
  q_hyst_sf.add_field(
    new parser::field_enum_num<LIBLTE_RRC_SF_MEDIUM_ENUM,int8>
    ("medium", &data->speed_state_resel_params.q_hyst_sf.medium, 
     liblte_rrc_sf_medium_num, LIBLTE_RRC_SF_MEDIUM_N_ITEMS)
  );
  q_hyst_sf.add_field(
    new parser::field_enum_num<LIBLTE_RRC_SF_HIGH_ENUM,int8>
    ("high", &data->speed_state_resel_params.q_hyst_sf.high, 
     liblte_rrc_sf_high_num, LIBLTE_RRC_SF_HIGH_N_ITEMS)
  );

  
  parser::section mob_params("mobility_state_params");
  speed_resel.add_subsection(&mob_params);
  mob_params.add_field(
    new parser::field_enum_num<LIBLTE_RRC_T_EVALUATION_ENUM,int16>
    ("t_eval", &data->speed_state_resel_params.mobility_state_params.t_eval, 
     liblte_rrc_t_evaluation_num, LIBLTE_RRC_T_EVALUATION_N_ITEMS)
  );
  mob_params.add_field(
    new parser::field_enum_num<LIBLTE_RRC_T_HYST_NORMAL_ENUM,int16>
    ("t_hyst_normal", &data->speed_state_resel_params.mobility_state_params.t_hyst_normal, 
     liblte_rrc_t_hyst_normal_num, LIBLTE_RRC_T_HYST_NORMAL_N_ITEMS)
  );
  mob_params.add_field(
    new parser::field<uint8>
    ("n_cell_change_medium", &data->speed_state_resel_params.mobility_state_params.n_cell_change_medium)
  );
  mob_params.add_field(
    new parser::field<uint8>
    ("n_cell_change_high", &data->speed_state_resel_params.mobility_state_params.n_cell_change_high)
  );

  // CellReselectionServingFreqInfo
  parser::section resel_serving("cell_reselection_serving");
  sib3.add_subsection(&resel_serving);

  resel_serving.add_field(
    new parser::field<uint8> ("s_non_intra_search", &data->s_non_intra_search, &data->s_non_intra_search_present)
  );
  resel_serving.add_field(
    new parser::field<uint8>("thresh_serving_low", &data->thresh_serving_low)
  );
  resel_serving.add_field(
    new parser::field<uint8>("cell_resel_prio", &data->cell_resel_prio)
  );
  
  
  // intraFreqCellReselectionInfo
  parser::section intra_freq("intra_freq_reselection");
  sib3.add_subsection(&intra_freq);
  
  intra_freq.add_field(
    new parser::field<int16>("q_rx_lev_min", &data->q_rx_lev_min)
  );
  intra_freq.add_field(
    new parser::field<int8>("p_max", &data->p_max, &data->p_max_present)
  );
  intra_freq.add_field(
    new parser::field<uint8>("s_intra_search", &data->s_intra_search, &data->s_intra_search_present)
  );
  intra_freq.add_field(
    new parser::field_enum_num<LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_ENUM,double>
    ("allowed_meas_bw", &data->allowed_meas_bw, 
     liblte_rrc_allowed_meas_bandwidth_num, LIBLTE_RRC_ALLOWED_MEAS_BANDWIDTH_N_ITEMS, 
     &data->allowed_meas_bw_present)
  );
  intra_freq.add_field(
    new parser::field<bool>("presence_ant_port_1", &data->presence_ant_port_1)
  );
  intra_freq.add_field(
    new parser::field<uint8>("neigh_cell_cnfg", &data->neigh_cell_cnfg)
  );
  intra_freq.add_field(
    new parser::field<uint8>("t_resel_eutra", &data->t_resel_eutra)
  );
  parser::section t_resel_eutra_sf("t_resel_eutra_sf");
  intra_freq.add_subsection(&t_resel_eutra_sf);
  t_resel_eutra_sf.set_optional(&data->t_resel_eutra_sf_present);
  
  t_resel_eutra_sf.add_field(
    new parser::field_enum_num<LIBLTE_RRC_SSSF_MEDIUM_ENUM,double>
    ("sf_medium", &data->t_resel_eutra_sf.sf_medium, 
     liblte_rrc_sssf_medium_num, LIBLTE_RRC_SSSF_MEDIUM_N_ITEMS)
  );
  t_resel_eutra_sf.add_field(
    new parser::field_enum_num<LIBLTE_RRC_SSSF_HIGH_ENUM,double>
    ("sf_high", &data->t_resel_eutra_sf.sf_high, 
     liblte_rrc_sssf_high_num, LIBLTE_RRC_SSSF_HIGH_N_ITEMS)
  );
  
  // Run parser with single section 
  return parser::parse_section(filename, &sib3);
}

int enb::parse_sib4(std::string filename, LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT *data) 
{  
  parser::section sib4("sib4");
  
  // csg-PhysCellIdRange 
  parser::section csg_range("csg_phys_cell_id_range");
  sib4.add_subsection(&csg_range);
  csg_range.set_optional(&data->csg_phys_cell_id_range_present);
  csg_range.add_field(
    new parser::field_enum_num<LIBLTE_RRC_PHYS_CELL_ID_RANGE_ENUM,int16>
    ("range", &data->csg_phys_cell_id_range.range, 
     liblte_rrc_phys_cell_id_range_num, LIBLTE_RRC_PHYS_CELL_ID_RANGE_N_ITEMS)
  );
  csg_range.add_field(
    new parser::field<uint16>("start", &data->csg_phys_cell_id_range.start)
  );

  // intraFreqNeighCellList
  parser::section intra_neigh("intra_freq_neigh_cell_list");
  sib4.add_subsection(&intra_neigh);
  bool dummy_bool = false; 
  intra_neigh.set_optional(&dummy_bool);
  intra_neigh.add_field(new field_intra_neigh_cell_list(data));  

  // intraFreqBlackCellList
  parser::section intra_black("intra_freq_black_cell_list");
  sib4.add_subsection(&intra_black);
  intra_black.set_optional(&dummy_bool);
  intra_black.add_field(new field_intra_black_cell_list(data));  

  // Run parser with single section 
  return parser::parse_section(filename, &sib4);
}


uint32_t HexToBytes(const std::string& str, uint8_t *char_value, uint32_t buff_len) {
  uint32_t i=0; 
  for (i = 0; i < str.length() && i<buff_len; i += 2) {
    std::string byteString = str.substr(i, 2);
    char byte = (char) strtol(byteString.c_str(), NULL, 16);
    char_value[i/2]=byte; 
  }

  return i/2;
}


int enb::parse_sib9(std::string filename, LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9_STRUCT *data) 
{  
  parser::section sib9("sib9");

  bool name_enabled, hex_enabled; 
  std::string hnb_name, hex_value; 
  
  sib9.add_field(new parser::field<std::string>("hnb_name", &hnb_name, &name_enabled));
  sib9.add_field(new parser::field<std::string>("hex_value", &hex_value, &hex_enabled));

  // Run parser with single section 
  if (!parser::parse_section(filename, &sib9)) {
    data->hnb_name_present = true; 
    if (name_enabled) {
      strncpy((char*) data->hnb_name, hnb_name.c_str(), 47);
      data->hnb_name[47] = 0;
      data->hnb_name_size = strnlen(hnb_name.c_str(), 48);         
    } else if (hex_enabled) {
      data->hnb_name_size = HexToBytes(hex_value, data->hnb_name, 48);         
    } else {
      data->hnb_name_present = false; 
    }
    return 0; 
  } else {
    return -1;
  }
}

int enb::parse_sib13(std::string filename, LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT *data)
{
  parser::section sib13("sib13");
  
  sib13.add_field(
     new parser::field<uint8>
    ("mbsfn_area_info_list_size", &data->mbsfn_area_info_list_r9_size)
  );
  
  parser::section mbsfn_notification_config("mbsfn_notification_config");
  sib13.add_subsection(&mbsfn_notification_config);
  
  
  mbsfn_notification_config.add_field( 
     new parser::field_enum_str<LIBLTE_RRC_NOTIFICATION_REPETITION_COEFF_R9_ENUM>
    ("mbsfn_notification_repetition_coeff", &data->mbsfn_notification_config.repetition_coeff, liblte_rrc_notification_repetition_coeff_r9_text,LIBLTE_RRC_NOTIFICATION_REPETITION_COEFF_R9_N_ITEMS)
  );
  
  mbsfn_notification_config.add_field( 
     new parser::field<uint8>
    ("mbsfn_notification_offset", &data->mbsfn_notification_config.offset)
  );
  
  mbsfn_notification_config.add_field( 
     new parser::field<uint8>
    ("mbsfn_notification_sf_index", &data->mbsfn_notification_config.sf_index)
  );
  
  
  parser::section mbsfn_area_info_list("mbsfn_area_info_list");
  sib13.add_subsection(&mbsfn_area_info_list);
  
  mbsfn_area_info_list.add_field(
   new parser::field_enum_str<LIBLTE_RRC_NON_MBSFN_REGION_LENGTH_ENUM>
    ("non_mbsfn_region_length", &data->mbsfn_area_info_list_r9[0].non_mbsfn_region_length,
          liblte_rrc_non_mbsfn_region_length_text,LIBLTE_RRC_NON_MBSFN_REGION_LENGTH_N_ITEMS)
  );
  
  mbsfn_area_info_list.add_field(
   new parser::field_enum_str<LIBLTE_RRC_MCCH_REPETITION_PERIOD_ENUM>
    ("mcch_repetition_period", &data->mbsfn_area_info_list_r9[0].mcch_repetition_period_r9,
          liblte_rrc_mcch_repetition_period_r9_text,LIBLTE_RRC_MCCH_REPETITION_PERIOD_N_ITEMS)
  );
  
  
  mbsfn_area_info_list.add_field(
   new parser::field_enum_str<LIBLTE_RRC_MCCH_MODIFICATION_PERIOD_ENUM>
    ("mcch_modification_period", &data->mbsfn_area_info_list_r9[0].mcch_modification_period_r9,
          liblte_rrc_mcch_modification_period_r9_text,LIBLTE_RRC_MCCH_MODIFICATION_PERIOD_N_ITEMS)
  );
  
  mbsfn_area_info_list.add_field(
   new parser::field_enum_str<LIBLTE_RRC_MCCH_SIGNALLING_MCS_ENUM>
    ("signalling_mcs", &data->mbsfn_area_info_list_r9[0].signalling_mcs_r9,
          liblte_rrc_mcch_signalling_mcs_r9_text,LIBLTE_RRC_MCCH_SIGNALLING_MCS_N_ITEMS)
  );
  
  
  mbsfn_area_info_list.add_field( 
     new parser::field<uint8>
    ("mbsfn_area_id", &data->mbsfn_area_info_list_r9[0].mbsfn_area_id_r9)
  );

  mbsfn_area_info_list.add_field( 
     new parser::field<uint8>
    ("notification_indicator", &data->mbsfn_area_info_list_r9[0].notification_indicator_r9)
  );
  
  mbsfn_area_info_list.add_field( 
     new parser::field<uint8>
    ("mcch_offset", &data->mbsfn_area_info_list_r9[0].mcch_offset_r9)
  );
  
  mbsfn_area_info_list.add_field( 
     new parser::field<uint8>
    ("sf_alloc_info", &data->mbsfn_area_info_list_r9[0].sf_alloc_info_r9)
  );
  return parser::parse_section(filename, &sib13);
}

int enb::parse_sibs(all_args_t *args, rrc_cfg_t *rrc_cfg, phy_cfg_t *phy_config_common)
{
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *sib1 = &rrc_cfg->sibs[0].sib.sib1;
  rrc_cfg->sibs[0].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1; 
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *sib2 = &rrc_cfg->sibs[1].sib.sib2;
  rrc_cfg->sibs[1].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT *sib3 = &rrc_cfg->sibs[2].sib.sib3;
  rrc_cfg->sibs[2].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT *sib4 = &rrc_cfg->sibs[3].sib.sib4;
  rrc_cfg->sibs[3].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9_STRUCT *sib9 = &rrc_cfg->sibs[8].sib.sib9;
  rrc_cfg->sibs[8].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT *sib13 = &rrc_cfg->sibs[12].sib.sib13;
  rrc_cfg->sibs[12].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13;
  
  // Read SIB1 configuration from file 
  bzero(sib1, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT));
  if (parse_sib1(args->enb_files.sib_config, sib1)) {
    return -1; 
  }
  
  // Fill rest of data from enb config 
  sib1->cell_id = (args->enb.s1ap.enb_id << 8) + args->enb.s1ap.cell_id;
  sib1->tracking_area_code = args->enb.s1ap.tac;
  sib1->freq_band_indicator = srslte_band_get_band(args->rf.dl_earfcn); 
  sib1->N_plmn_ids = 1; 
  sib1->plmn_id[0].id.mcc = args->enb.s1ap.mcc;  
  sib1->plmn_id[0].id.mnc = args->enb.s1ap.mnc;
  sib1->plmn_id[0].resv_for_oper = LIBLTE_RRC_NOT_RESV_FOR_OPER;
  sib1->cell_barred = LIBLTE_RRC_CELL_NOT_BARRED;
  sib1->q_rx_lev_min_offset = 0; 
   
  // Generate SIB2
  bzero(sib2, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT));
  if (parse_sib2(args->enb_files.sib_config, sib2)) {
    return -1; 
  }
 
  // SRS not yet supported 
  sib2->rr_config_common_sib.srs_ul_cnfg.present = false; 
  if (sib2->ul_bw.present) {
    switch(args->enb.n_prb) {
      case 6:
        sib2->ul_bw.bw = LIBLTE_RRC_UL_BW_N6;
        break;
      case 15:
        sib2->ul_bw.bw = LIBLTE_RRC_UL_BW_N15;
        break;
      case 25:
        sib2->ul_bw.bw = LIBLTE_RRC_UL_BW_N25;
        break;
      case 50:
        sib2->ul_bw.bw = LIBLTE_RRC_UL_BW_N50;
        break;
      case 75:
        sib2->ul_bw.bw = LIBLTE_RRC_UL_BW_N75;
        break;
      case 100:
        sib2->ul_bw.bw = LIBLTE_RRC_UL_BW_N100;
        break;
    }
  }
  if (sib2->arfcn_value_eutra.present) {
    sib2->arfcn_value_eutra.value = args->rf.ul_earfcn;
  }
  
  // Generate SIB3 if defined in mapping info 
  if (sib_is_present(sib1->sched_info, sib1->N_sched_info, LIBLTE_RRC_SIB_TYPE_3)) {
    bzero(sib3, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT));
    if (parse_sib3(args->enb_files.sib_config, sib3)) {
      return -1; 
    }    
  }

  // Generate SIB4 if defined in mapping info 
  if (sib_is_present(sib1->sched_info, sib1->N_sched_info, LIBLTE_RRC_SIB_TYPE_4)) {
    bzero(sib4, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT));
    if (parse_sib4(args->enb_files.sib_config, sib4)) {
      return -1; 
    }    
  }

  // Generate SIB9 if defined in mapping info 
  if (sib_is_present(sib1->sched_info, sib1->N_sched_info, LIBLTE_RRC_SIB_TYPE_9)) {
    bzero(sib9, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9_STRUCT));
    if (parse_sib9(args->enb_files.sib_config, sib9)) {
      return -1; 
    }    
  }
  
  if (sib_is_present(sib1->sched_info, sib1->N_sched_info, LIBLTE_RRC_SIB_TYPE_13_v920)) {
    bzero(sib13, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT));
    if (parse_sib13(args->enb_files.sib_config, sib13)) {
      return -1; 
    }    
  }
  // Copy PHY common configuration 
  bzero(phy_config_common, sizeof(phy_cfg_t));  
  memcpy(&phy_config_common->prach_cnfg, &sib2->rr_config_common_sib.prach_cnfg, sizeof(LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT));
  memcpy(&phy_config_common->pdsch_cnfg, &sib2->rr_config_common_sib.pdsch_cnfg, sizeof(LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT));
  memcpy(&phy_config_common->pusch_cnfg, &sib2->rr_config_common_sib.pusch_cnfg, sizeof(LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT));
  memcpy(&phy_config_common->pucch_cnfg, &sib2->rr_config_common_sib.pucch_cnfg, sizeof(LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT));
  memcpy(&phy_config_common->srs_ul_cnfg, &sib2->rr_config_common_sib.srs_ul_cnfg, sizeof(LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT));
  
  return 0; 
}

bool enb::sib_is_present(LIBLTE_RRC_SCHEDULING_INFO_STRUCT *sched_info, uint32_t nof_sched_info, LIBLTE_RRC_SIB_TYPE_ENUM sib_num) 
{
  for (uint32_t i=0;i<nof_sched_info;i++) {
    for (uint32_t j=0;j<sched_info[i].N_sib_mapping_info;j++) {
      if (sched_info[i].sib_mapping_info[j].sib_type == sib_num) {
        return true; 
      }
    }
  }
  return false; 
}

int enb::parse_rr(all_args_t* args, rrc_cfg_t* rrc_cfg)
{

  /* Transmission mode config section */
  if (args->enb.transmission_mode < 1 || args->enb.transmission_mode > 4) {
    ERROR("Invalid transmission mode (%d). Only indexes 1-4 are implemented.\n", args->enb.transmission_mode);
    return SRSLTE_ERROR;
  } else if (args->enb.transmission_mode == 1 && args->enb.nof_ports > 1) {
    ERROR("Invalid number of ports (%d) for transmission mode (%d). Only one antenna port is allowed.\n",
          args->enb.nof_ports, args->enb.transmission_mode);
    return SRSLTE_ERROR;
  } else if (args->enb.transmission_mode > 1 && args->enb.nof_ports != 2) {
    ERROR("The selected number of ports (%d) are insufficient for the selected transmission mode (%d).\n",
          args->enb.nof_ports, args->enb.transmission_mode);
    return SRSLTE_ERROR;
  }

  bzero(&rrc_cfg->antenna_info, sizeof(LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT));
  rrc_cfg->antenna_info.tx_mode = (LIBLTE_RRC_TRANSMISSION_MODE_ENUM) (args->enb.transmission_mode - 1);

  if (rrc_cfg->antenna_info.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_3) {
    rrc_cfg->antenna_info.ue_tx_antenna_selection_setup = LIBLTE_RRC_UE_TX_ANTENNA_SELECTION_OPEN_LOOP;
    rrc_cfg->antenna_info.ue_tx_antenna_selection_setup_present = false;

    rrc_cfg->antenna_info.codebook_subset_restriction_choice = LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM3;
    rrc_cfg->antenna_info.codebook_subset_restriction = 0b11;
    rrc_cfg->antenna_info.codebook_subset_restriction_present = true;
  } else if (rrc_cfg->antenna_info.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_4) {
    rrc_cfg->antenna_info.ue_tx_antenna_selection_setup = LIBLTE_RRC_UE_TX_ANTENNA_SELECTION_CLOSED_LOOP;
    rrc_cfg->antenna_info.ue_tx_antenna_selection_setup_present = true;

    rrc_cfg->antenna_info.codebook_subset_restriction_choice = LIBLTE_RRC_CODEBOOK_SUBSET_RESTRICTION_N2_TM4;
    rrc_cfg->antenna_info.codebook_subset_restriction = 0b111111;
    rrc_cfg->antenna_info.codebook_subset_restriction_present = true;
  }

  /* Parse power allocation */
  rrc_cfg->pdsch_cfg = LIBLTE_RRC_PDSCH_CONFIG_P_A_N_ITEMS;
  for (int i = 0; i < LIBLTE_RRC_PDSCH_CONFIG_P_A_N_ITEMS; i++) {
    if (args->enb.p_a == liblte_rrc_pdsch_config_p_a_num[i]) {
      rrc_cfg->pdsch_cfg = (LIBLTE_RRC_PDSCH_CONFIG_P_A_ENUM) i;
    }
  }
  if (rrc_cfg->pdsch_cfg == LIBLTE_RRC_PDSCH_CONFIG_P_A_N_ITEMS) {
    ERROR("Invalid p_a value (%f) only -6, -4.77, -3, -1.77, 0, 1, 2, 3 values allowed.", args->enb.p_a);
    return SRSLTE_ERROR;
  }

  /* MAC config section */
  parser::section mac_cnfg("mac_cnfg");

  parser::section phr_cnfg("phr_cnfg");
  mac_cnfg.add_subsection(&phr_cnfg);
  mac_cnfg.set_optional(&rrc_cfg->mac_cnfg.phr_cnfg.setup_present);
  phr_cnfg.add_field(
    new parser::field_enum_str<LIBLTE_RRC_DL_PATHLOSS_CHANGE_ENUM>
    ("dl_pathloss_change", &rrc_cfg->mac_cnfg.phr_cnfg.dl_pathloss_change, 
     liblte_rrc_dl_pathloss_change_text, LIBLTE_RRC_DL_PATHLOSS_CHANGE_N_ITEMS)
  );
  phr_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_PERIODIC_PHR_TIMER_ENUM,int>
    ("periodic_phr_timer", &rrc_cfg->mac_cnfg.phr_cnfg.periodic_phr_timer, 
     liblte_rrc_periodic_phr_timer_num, LIBLTE_RRC_PERIODIC_PHR_TIMER_N_ITEMS)
  );
  phr_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_PROHIBIT_PHR_TIMER_ENUM,int>
    ("prohibit_phr_timer", &rrc_cfg->mac_cnfg.phr_cnfg.prohibit_phr_timer, 
     liblte_rrc_prohibit_phr_timer_num, LIBLTE_RRC_PROHIBIT_PHR_TIMER_N_ITEMS)
  );
  
  parser::section ulsch_cnfg("ulsch_cnfg");
  mac_cnfg.add_subsection(&ulsch_cnfg);
  
  rrc_cfg->mac_cnfg.ulsch_cnfg.tti_bundling = false; 
  ulsch_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_MAX_HARQ_TX_ENUM,int8>
    ("max_harq_tx", &rrc_cfg->mac_cnfg.ulsch_cnfg.max_harq_tx, 
     liblte_rrc_max_harq_tx_num, LIBLTE_RRC_MAX_HARQ_TX_N_ITEMS, 
     &rrc_cfg->mac_cnfg.ulsch_cnfg.max_harq_tx_present)
  );
  ulsch_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_PERIODIC_BSR_TIMER_ENUM,int32>
    ("periodic_bsr_timer", &rrc_cfg->mac_cnfg.ulsch_cnfg.periodic_bsr_timer, 
     liblte_rrc_periodic_bsr_timer_num, LIBLTE_RRC_PERIODIC_BSR_TIMER_N_ITEMS, 
     &rrc_cfg->mac_cnfg.ulsch_cnfg.periodic_bsr_timer_present)
  );

  ulsch_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_ENUM,int32>
    ("retx_bsr_timer", &rrc_cfg->mac_cnfg.ulsch_cnfg.retx_bsr_timer, 
     liblte_rrc_retransmission_bsr_timer_num, LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_N_ITEMS)
  );

  mac_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_TIME_ALIGNMENT_TIMER_ENUM,int32>
    ("time_alignment_timer", &rrc_cfg->mac_cnfg.time_alignment_timer, 
     liblte_rrc_time_alignment_timer_num, LIBLTE_RRC_TIME_ALIGNMENT_TIMER_N_ITEMS)
  );


  /* PHY config section */
  parser::section phy_cfg("phy_cnfg");

  parser::section pusch_cnfg_ded("pusch_cnfg_ded");
  phy_cfg.add_subsection(&pusch_cnfg_ded);

  pusch_cnfg_ded.add_field(new parser::field<uint8> ("beta_offset_ack_idx", &rrc_cfg->pusch_cfg.beta_offset_ack_idx));
  pusch_cnfg_ded.add_field(new parser::field<uint8> ("beta_offset_ri_idx", &rrc_cfg->pusch_cfg.beta_offset_ri_idx));
  pusch_cnfg_ded.add_field(new parser::field<uint8> ("beta_offset_cqi_idx", &rrc_cfg->pusch_cfg.beta_offset_cqi_idx));
  
  parser::section sched_request_cnfg("sched_request_cnfg");
  phy_cfg.add_subsection(&sched_request_cnfg);

  sched_request_cnfg.add_field(
    new parser::field_enum_num<LIBLTE_RRC_DSR_TRANS_MAX_ENUM,int32>
    ("dsr_trans_max", &rrc_cfg->sr_cfg.dsr_max, 
     liblte_rrc_dsr_trans_max_num, LIBLTE_RRC_DSR_TRANS_MAX_N_ITEMS)
  );
  sched_request_cnfg.add_field(new parser::field<uint32> ("period", &rrc_cfg->sr_cfg.period));
  sched_request_cnfg.add_field(new parser::field<uint32> ("nof_prb", &rrc_cfg->sr_cfg.nof_prb));
  sched_request_cnfg.add_field(new field_sf_mapping(rrc_cfg->sr_cfg.sf_mapping, &rrc_cfg->sr_cfg.nof_subframes));
  
  parser::section cqi_report_cnfg("cqi_report_cnfg");
  phy_cfg.add_subsection(&cqi_report_cnfg);

  cqi_report_cnfg.add_field(
    new parser::field_enum_str<rrc_cfg_cqi_mode_t>
    ("mode", &rrc_cfg->cqi_cfg.mode, 
     rrc_cfg_cqi_mode_text, RRC_CFG_CQI_MODE_N_ITEMS)
  );
  cqi_report_cnfg.add_field(new parser::field<uint32> ("period", &rrc_cfg->cqi_cfg.period));
  cqi_report_cnfg.add_field(new parser::field<uint32> ("nof_prb", &rrc_cfg->cqi_cfg.nof_prb));
  cqi_report_cnfg.add_field(new parser::field<bool> ("simultaneousAckCQI", &rrc_cfg->cqi_cfg.simultaneousAckCQI));
  cqi_report_cnfg.add_field(new field_sf_mapping(rrc_cfg->cqi_cfg.sf_mapping, &rrc_cfg->cqi_cfg.nof_subframes));
  
  // Run parser with two sections
  parser p(args->enb_files.rr_config);
  p.add_section(&mac_cnfg);
  p.add_section(&phy_cfg);
  return p.parse();
}

int field_sf_mapping::parse(libconfig::Setting &root) 
{
  *nof_subframes = root["subframe"].getLength();
  for (uint32_t i=0;i<*nof_subframes;i++) {    
    sf_mapping[i] = root["subframe"][i];
  }
  return 0; 
}







int enb::parse_drb(all_args_t* args, rrc_cfg_t* rrc_cfg) 
{
  parser::section qci("qci_config");
  qci.add_field(new field_qci(rrc_cfg->qci_cfg));
  return parser::parse_section(args->enb_files.drb_config, &qci);
}

int field_qci::parse(libconfig::Setting &root) 
{
  uint32_t nof_qci = root.getLength();
  
  bzero(cfg, sizeof(rrc_cfg_qci_t)*MAX_NOF_QCI);
  
  for (uint32_t i=0;i<nof_qci;i++) {
    libconfig::Setting &q = root[i];
    
    uint32_t qci = q["qci"]; 
    
    
    // Parse PDCP section     
    if (!q.exists("pdcp_config")) {
      fprintf(stderr, "Error section pdcp_config not found for qci=%d\n", qci);
      return -1; 
    }
    parser::field_enum_num<LIBLTE_RRC_DISCARD_TIMER_ENUM,int32> discard_timer
        ("discard_timer", &cfg[qci].pdcp_cfg.discard_timer, 
        liblte_rrc_discard_timer_num, LIBLTE_RRC_DISCARD_TIMER_N_ITEMS);      
    if (discard_timer.parse(q["pdcp_config"])) {
      cfg[qci].pdcp_cfg.discard_timer_present = false; 
    } else {
      cfg[qci].pdcp_cfg.discard_timer_present = true; 
    }

    parser::field_enum_num<LIBLTE_RRC_PDCP_SN_SIZE_ENUM,int8> pdcp_sn_size
        ("pdcp_sn_size", &cfg[qci].pdcp_cfg.rlc_um_pdcp_sn_size, 
        liblte_rrc_pdcp_sn_size_num, LIBLTE_RRC_PDCP_SN_SIZE_N_ITEMS); 
        
    if (pdcp_sn_size.parse(q["pdcp_config"])) {
      cfg[qci].pdcp_cfg.rlc_um_pdcp_sn_size_present = false; 
    } else {
      cfg[qci].pdcp_cfg.rlc_um_pdcp_sn_size_present = true; 
    }
    
    if (q["pdcp_config"].lookupValue("status_report_required", cfg[qci].pdcp_cfg.rlc_am_status_report_required)) {
      cfg[qci].pdcp_cfg.rlc_am_status_report_required_present = true; 
    } else {
      cfg[qci].pdcp_cfg.rlc_am_status_report_required_present = false;
    }

    // Parse RLC section 
    if (q["rlc_config"].exists("ul_am")) {
      cfg[qci].rlc_cfg.rlc_mode = LIBLTE_RRC_RLC_MODE_AM;   
    } else if (q["rlc_config"].exists("ul_um") && q["rlc_config"].exists("dl_um")) {
      cfg[qci].rlc_cfg.rlc_mode = LIBLTE_RRC_RLC_MODE_UM_BI;   
    } else if (q["rlc_config"].exists("ul_um") && !q["rlc_config"].exists("dl_um")) {
      cfg[qci].rlc_cfg.rlc_mode = LIBLTE_RRC_RLC_MODE_UM_UNI_UL;   
    } else if (!q["rlc_config"].exists("ul_um") && q["rlc_config"].exists("dl_um")) {
      cfg[qci].rlc_cfg.rlc_mode = LIBLTE_RRC_RLC_MODE_UM_UNI_DL;   
    } else {
      fprintf(stderr, "Invalid combination of UL/DL UM/AM for qci=%d\n", qci);
      return -1; 
    }
    
    // Parse RLC-UM section 
    if (q["rlc_config"].exists("ul_um")) {
      
      LIBLTE_RRC_UL_UM_RLC_STRUCT *rlc_cfg = &cfg[qci].rlc_cfg.ul_um_bi_rlc; 
      if (cfg[qci].rlc_cfg.rlc_mode == LIBLTE_RRC_RLC_MODE_UM_UNI_UL) {
        rlc_cfg = &cfg[qci].rlc_cfg.ul_um_bi_rlc;
      }
      
      parser::field_enum_num<LIBLTE_RRC_SN_FIELD_LENGTH_ENUM,uint8> sn_field_len
        ("sn_field_length", &rlc_cfg->sn_field_len, 
        liblte_rrc_sn_field_length_num, LIBLTE_RRC_SN_FIELD_LENGTH_N_ITEMS);      
      if (sn_field_len.parse(q["rlc_config"]["ul_um"])) {
        fprintf(stderr, "Error can't find sn_field_length in section ul_um\n");
      }
    }
    
    if (q["rlc_config"].exists("dl_um")) {
      
      LIBLTE_RRC_DL_UM_RLC_STRUCT *rlc_cfg = &cfg[qci].rlc_cfg.dl_um_bi_rlc; 
      if (cfg[qci].rlc_cfg.rlc_mode == LIBLTE_RRC_RLC_MODE_UM_UNI_DL) {
        rlc_cfg = &cfg[qci].rlc_cfg.dl_um_bi_rlc;
      }
      
      parser::field_enum_num<LIBLTE_RRC_SN_FIELD_LENGTH_ENUM,uint8> sn_field_len
        ("sn_field_length", &rlc_cfg->sn_field_len, 
        liblte_rrc_sn_field_length_num, LIBLTE_RRC_SN_FIELD_LENGTH_N_ITEMS);      
      if (sn_field_len.parse(q["rlc_config"]["dl_um"])) {
        fprintf(stderr, "Error can't find sn_field_length in section dl_um\n");
      }
      
      parser::field_enum_num<LIBLTE_RRC_T_REORDERING_ENUM,int32> t_reordering
        ("t_reordering", &rlc_cfg->t_reordering, 
        liblte_rrc_t_reordering_num, LIBLTE_RRC_T_REORDERING_N_ITEMS);      
      if (t_reordering.parse(q["rlc_config"]["dl_um"])) {
        fprintf(stderr, "Error can't find t_reordering in section dl_um\n");
      }
    }    
      
    // Parse RLC-AM section 
    if (q["rlc_config"].exists("ul_am")) {
      LIBLTE_RRC_UL_AM_RLC_STRUCT *rlc_cfg = &cfg[qci].rlc_cfg.ul_am_rlc; 
      
      parser::field_enum_num<LIBLTE_RRC_T_POLL_RETRANSMIT_ENUM,int32> t_poll_retx
        ("t_poll_retx", &rlc_cfg->t_poll_retx, 
        liblte_rrc_t_poll_retransmit_num, LIBLTE_RRC_T_POLL_RETRANSMIT_N_ITEMS);      
      if (t_poll_retx.parse(q["rlc_config"]["ul_am"])) {
        fprintf(stderr, "Error can't find t_poll_retx in section ul_am\n");
      }
      
      parser::field_enum_num<LIBLTE_RRC_POLL_PDU_ENUM,int32> poll_pdu
        ("poll_pdu", &rlc_cfg->poll_pdu, 
        liblte_rrc_poll_pdu_num, LIBLTE_RRC_POLL_PDU_N_ITEMS);      
      if (poll_pdu.parse(q["rlc_config"]["ul_am"])) {
        fprintf(stderr, "Error can't find poll_pdu in section ul_am\n");
      }

      parser::field_enum_num<LIBLTE_RRC_POLL_BYTE_ENUM,int32> poll_byte
        ("poll_byte", &rlc_cfg->poll_byte, 
        liblte_rrc_poll_byte_num, LIBLTE_RRC_POLL_BYTE_N_ITEMS);      
      if (poll_byte.parse(q["rlc_config"]["ul_am"])) {
        fprintf(stderr, "Error can't find poll_byte in section ul_am\n");
      }

      parser::field_enum_num<LIBLTE_RRC_MAX_RETX_THRESHOLD_ENUM,uint32_t> max_retx_thresh
        ("max_retx_thresh", &rlc_cfg->max_retx_thresh, 
        liblte_rrc_max_retx_threshold_num, LIBLTE_RRC_MAX_RETX_THRESHOLD_N_ITEMS);      
      if (max_retx_thresh.parse(q["rlc_config"]["ul_am"])) {
        fprintf(stderr, "Error can't find max_retx_thresh in section ul_am\n");
      }
    }
    
    if (q["rlc_config"].exists("dl_am")) {      
      LIBLTE_RRC_DL_AM_RLC_STRUCT *rlc_cfg = &cfg[qci].rlc_cfg.dl_am_rlc; 
      
      parser::field_enum_num<LIBLTE_RRC_T_REORDERING_ENUM,int32> t_reordering
        ("t_reordering", &rlc_cfg->t_reordering, 
        liblte_rrc_t_reordering_num, LIBLTE_RRC_T_REORDERING_N_ITEMS);      
      if (t_reordering.parse(q["rlc_config"]["dl_am"])) {
        fprintf(stderr, "Error can't find t_reordering in section dl_am\n");
      }
      
      parser::field_enum_num<LIBLTE_RRC_T_STATUS_PROHIBIT_ENUM,int32> t_status_prohibit
        ("t_status_prohibit", &rlc_cfg->t_status_prohibit, 
        liblte_rrc_t_status_prohibit_num, LIBLTE_RRC_T_STATUS_PROHIBIT_N_ITEMS);      
      if (t_status_prohibit.parse(q["rlc_config"]["dl_am"])) {
        fprintf(stderr, "Error can't find t_status_prohibit in section dl_am\n");
      }
    }    
    

    // Parse logical channel configuration section 
    if (!q.exists("logical_channel_config")) {
      fprintf(stderr, "Error section logical_channel_config not found for qci=%d\n", qci);
      return -1; 
    }
    LIBLTE_RRC_UL_SPECIFIC_PARAMETERS_STRUCT *lc_cfg = &cfg[qci].lc_cfg; 

    parser::field<uint8> priority ("priority", &lc_cfg->priority);      
    if (priority.parse(q["logical_channel_config"])) {
      fprintf(stderr, "Error can't find logical_channel_config in section priority\n");
    }
    
    parser::field_enum_num<LIBLTE_RRC_PRIORITIZED_BIT_RATE_ENUM,int32> prioritized_bit_rate
        ("prioritized_bit_rate", &lc_cfg->prioritized_bit_rate, 
        liblte_rrc_prioritized_bit_rate_num, LIBLTE_RRC_PRIORITIZED_BIT_RATE_N_ITEMS);      
    if (prioritized_bit_rate.parse(q["logical_channel_config"])) {
      fprintf(stderr, "Error can't find prioritized_bit_rate in section logical_channel_config\n");
    }

    parser::field_enum_num<LIBLTE_RRC_BUCKET_SIZE_DURATION_ENUM,int16> bucket_size_duration
        ("bucket_size_duration", &lc_cfg->bucket_size_duration, 
        liblte_rrc_bucket_size_duration_num, LIBLTE_RRC_BUCKET_SIZE_DURATION_N_ITEMS);      
    if (bucket_size_duration.parse(q["logical_channel_config"])) {
      fprintf(stderr, "Error can't find bucket_size_duration in section logical_channel_config\n");
    }

    parser::field<uint8> log_chan_group ("log_chan_group", &lc_cfg->log_chan_group);      
    if (log_chan_group.parse(q["logical_channel_config"])) {
      lc_cfg->log_chan_group_present = false; 
    } else {
      lc_cfg->log_chan_group_present = true; 
    }
    
    


    cfg[qci].configured = true; 
  }
  
  return 0; 
}

}
