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

#include "srsenb/hdr/stack/rrc/nr/cell_asn1_config.h"

using namespace asn1::rrc_nr;

namespace srsenb {

int fill_csi_meas_from_enb_cfg(const rrc_nr_cfg_t& cfg, csi_meas_cfg_s& csi_meas_cfg)
{
  // Fill NZP-CSI Resources
  csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list_present = true;
  if (cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
    csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list.resize(5);
    auto& nzp_csi_res = csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list;
    // item 0
    nzp_csi_res[0].nzp_csi_rs_res_id = 0;
    nzp_csi_res[0].res_map.freq_domain_alloc.set_row2();
    nzp_csi_res[0].res_map.freq_domain_alloc.row2().from_number(0b100000000000);
    nzp_csi_res[0].res_map.nrof_ports                       = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
    nzp_csi_res[0].res_map.first_ofdm_symbol_in_time_domain = 4;
    nzp_csi_res[0].res_map.cdm_type                         = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
    nzp_csi_res[0].res_map.density.set_one();
    nzp_csi_res[0].res_map.freq_band.start_rb = 0;
    nzp_csi_res[0].res_map.freq_band.nrof_rbs = 52;
    nzp_csi_res[0].pwr_ctrl_offset            = 0;
    // Skip pwr_ctrl_offset_ss_present
    nzp_csi_res[0].scrambling_id                  = cfg.cell_list[0].phy_cell.cell_id;
    nzp_csi_res[0].periodicity_and_offset_present = true;
    nzp_csi_res[0].periodicity_and_offset.set_slots80();
    nzp_csi_res[0].periodicity_and_offset.slots80() = 1;
    // optional
    nzp_csi_res[0].qcl_info_periodic_csi_rs_present = true;
    nzp_csi_res[0].qcl_info_periodic_csi_rs         = 0;
    // item 1
    nzp_csi_res[1].nzp_csi_rs_res_id = 1;
    nzp_csi_res[1].res_map.freq_domain_alloc.set_row1();
    nzp_csi_res[1].res_map.freq_domain_alloc.row1().from_number(0b0001);
    nzp_csi_res[1].res_map.nrof_ports                       = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
    nzp_csi_res[1].res_map.first_ofdm_symbol_in_time_domain = 4;
    nzp_csi_res[1].res_map.cdm_type                         = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
    nzp_csi_res[1].res_map.density.set_three();
    nzp_csi_res[1].res_map.freq_band.start_rb = 0;
    nzp_csi_res[1].res_map.freq_band.nrof_rbs = 52;
    nzp_csi_res[1].pwr_ctrl_offset            = 0;
    // Skip pwr_ctrl_offset_ss_present
    nzp_csi_res[1].scrambling_id                  = cfg.cell_list[0].phy_cell.cell_id;
    nzp_csi_res[1].periodicity_and_offset_present = true;
    nzp_csi_res[1].periodicity_and_offset.set_slots40();
    nzp_csi_res[1].periodicity_and_offset.slots40() = 11;
    // optional
    nzp_csi_res[1].qcl_info_periodic_csi_rs_present = true;
    nzp_csi_res[1].qcl_info_periodic_csi_rs         = 0;
    // item 2
    nzp_csi_res[2].nzp_csi_rs_res_id = 2;
    nzp_csi_res[2].res_map.freq_domain_alloc.set_row1();
    nzp_csi_res[2].res_map.freq_domain_alloc.row1().from_number(0b0001);
    nzp_csi_res[2].res_map.nrof_ports                       = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
    nzp_csi_res[2].res_map.first_ofdm_symbol_in_time_domain = 8;
    nzp_csi_res[2].res_map.cdm_type                         = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
    nzp_csi_res[2].res_map.density.set_three();
    nzp_csi_res[2].res_map.freq_band.start_rb = 0;
    nzp_csi_res[2].res_map.freq_band.nrof_rbs = 52;
    nzp_csi_res[2].pwr_ctrl_offset            = 0;
    // Skip pwr_ctrl_offset_ss_present
    nzp_csi_res[2].scrambling_id                  = cfg.cell_list[0].phy_cell.cell_id;
    nzp_csi_res[2].periodicity_and_offset_present = true;
    nzp_csi_res[2].periodicity_and_offset.set_slots40();
    nzp_csi_res[2].periodicity_and_offset.slots40() = 11;
    // optional
    nzp_csi_res[2].qcl_info_periodic_csi_rs_present = true;
    nzp_csi_res[2].qcl_info_periodic_csi_rs         = 0;
    // item 3
    nzp_csi_res[3].nzp_csi_rs_res_id = 3;
    nzp_csi_res[3].res_map.freq_domain_alloc.set_row1();
    nzp_csi_res[3].res_map.freq_domain_alloc.row1().from_number(0b0001);
    nzp_csi_res[3].res_map.nrof_ports                       = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
    nzp_csi_res[3].res_map.first_ofdm_symbol_in_time_domain = 4;
    nzp_csi_res[3].res_map.cdm_type                         = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
    nzp_csi_res[3].res_map.density.set_three();
    nzp_csi_res[3].res_map.freq_band.start_rb = 0;
    nzp_csi_res[3].res_map.freq_band.nrof_rbs = 52;
    nzp_csi_res[3].pwr_ctrl_offset            = 0;
    // Skip pwr_ctrl_offset_ss_present
    nzp_csi_res[3].scrambling_id                  = cfg.cell_list[0].phy_cell.cell_id;
    nzp_csi_res[3].periodicity_and_offset_present = true;
    nzp_csi_res[3].periodicity_and_offset.set_slots40();
    nzp_csi_res[3].periodicity_and_offset.slots40() = 12;
    // optional
    nzp_csi_res[3].qcl_info_periodic_csi_rs_present = true;
    nzp_csi_res[3].qcl_info_periodic_csi_rs         = 0;
    // item 4
    nzp_csi_res[4].nzp_csi_rs_res_id = 4;
    nzp_csi_res[4].res_map.freq_domain_alloc.set_row1();
    nzp_csi_res[4].res_map.freq_domain_alloc.row1().from_number(0b0001);
    nzp_csi_res[4].res_map.nrof_ports                       = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
    nzp_csi_res[4].res_map.first_ofdm_symbol_in_time_domain = 8;
    nzp_csi_res[4].res_map.cdm_type                         = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
    nzp_csi_res[4].res_map.density.set_three();
    nzp_csi_res[4].res_map.freq_band.start_rb = 0;
    nzp_csi_res[4].res_map.freq_band.nrof_rbs = 52;
    nzp_csi_res[4].pwr_ctrl_offset            = 0;
    // Skip pwr_ctrl_offset_ss_present
    nzp_csi_res[4].scrambling_id                  = cfg.cell_list[0].phy_cell.cell_id;
    nzp_csi_res[4].periodicity_and_offset_present = true;
    nzp_csi_res[4].periodicity_and_offset.set_slots40();
    nzp_csi_res[4].periodicity_and_offset.slots40() = 12;
    // optional
    nzp_csi_res[4].qcl_info_periodic_csi_rs_present = true;
    nzp_csi_res[4].qcl_info_periodic_csi_rs         = 0;
  } else {
    csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list.resize(1);
    auto& nzp_csi_res                = csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list;
    nzp_csi_res[0].nzp_csi_rs_res_id = 0;
    nzp_csi_res[0].res_map.freq_domain_alloc.set_row2();
    nzp_csi_res[0].res_map.freq_domain_alloc.row2().from_number(0b100000000000);
    nzp_csi_res[0].res_map.nrof_ports                       = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
    nzp_csi_res[0].res_map.first_ofdm_symbol_in_time_domain = 4;
    nzp_csi_res[0].res_map.cdm_type                         = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
    nzp_csi_res[0].res_map.density.set_one();
    nzp_csi_res[0].res_map.freq_band.start_rb = 0;
    nzp_csi_res[0].res_map.freq_band.nrof_rbs = 52;
    nzp_csi_res[0].pwr_ctrl_offset            = 0;
    // Skip pwr_ctrl_offset_ss_present
    nzp_csi_res[0].periodicity_and_offset_present       = true;
    nzp_csi_res[0].periodicity_and_offset.set_slots80() = 0;
    // optional
    nzp_csi_res[0].qcl_info_periodic_csi_rs_present = true;
    nzp_csi_res[0].qcl_info_periodic_csi_rs         = 0;
  }

  // Fill NZP-CSI Resource Sets
  csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list_present = true;
  if (cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
    csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list.resize(2);
    auto& nzp_csi_res_set = csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list;
    // item 0
    nzp_csi_res_set[0].nzp_csi_res_set_id = 0;
    nzp_csi_res_set[0].nzp_csi_rs_res.resize(1);
    nzp_csi_res_set[0].nzp_csi_rs_res[0] = 0;
    // item 1
    nzp_csi_res_set[1].nzp_csi_res_set_id = 1;
    nzp_csi_res_set[1].nzp_csi_rs_res.resize(4);
    nzp_csi_res_set[1].nzp_csi_rs_res[0] = 1;
    nzp_csi_res_set[1].nzp_csi_rs_res[1] = 2;
    nzp_csi_res_set[1].nzp_csi_rs_res[2] = 3;
    nzp_csi_res_set[1].nzp_csi_rs_res[3] = 4;
    // Skip TRS info
  } else {
    csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list.resize(1);
    auto& nzp_csi_res_set                 = csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list;
    nzp_csi_res_set[0].nzp_csi_res_set_id = 0;
    nzp_csi_res_set[0].nzp_csi_rs_res.resize(1);
    nzp_csi_res_set[0].nzp_csi_rs_res[0] = 0;
    // Skip TRS info
  }
  return SRSRAN_SUCCESS;
}

int fill_serv_cell_from_enb_cfg(const rrc_nr_cfg_t& cfg, serving_cell_cfg_s& serv_cell)
{
  serv_cell.csi_meas_cfg_present = true;
  return fill_csi_meas_from_enb_cfg(cfg, serv_cell.csi_meas_cfg.set_setup());
}

} // namespace srsenb
