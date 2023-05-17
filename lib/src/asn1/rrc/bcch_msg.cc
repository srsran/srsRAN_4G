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

#include "srsran/asn1/rrc/bcch_msg.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// BCCH-BCH-Message ::= SEQUENCE
SRSASN_CODE bcch_bch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_bch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void bcch_bch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("BCCH-BCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// BandClassInfoCDMA2000 ::= SEQUENCE
SRSASN_CODE band_class_info_cdma2000_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_resel_prio_present, 1));

  HANDLE_CODE(band_class.pack(bref));
  if (cell_resel_prio_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(pack_integer(bref, thresh_x_high, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, thresh_x_low, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_class_info_cdma2000_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_resel_prio_present, 1));

  HANDLE_CODE(band_class.unpack(bref));
  if (cell_resel_prio_present) {
    HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(unpack_integer(thresh_x_high, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(thresh_x_low, bref, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
void band_class_info_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("bandClass", band_class.to_string());
  if (cell_resel_prio_present) {
    j.write_int("cellReselectionPriority", cell_resel_prio);
  }
  j.write_int("threshX-High", thresh_x_high);
  j.write_int("threshX-Low", thresh_x_low);
  j.end_obj();
}

// NeighCellsPerBandclassCDMA2000-r11 ::= SEQUENCE
SRSASN_CODE neigh_cells_per_bandclass_cdma2000_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, arfcn, (uint16_t)0u, (uint16_t)2047u));
  HANDLE_CODE(pack_dyn_seq_of(bref, pci_list_r11, 1, 40, integer_packer<uint16_t>(0, 511)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cells_per_bandclass_cdma2000_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(arfcn, bref, (uint16_t)0u, (uint16_t)2047u));
  HANDLE_CODE(unpack_dyn_seq_of(pci_list_r11, bref, 1, 40, integer_packer<uint16_t>(0, 511)));

  return SRSASN_SUCCESS;
}
void neigh_cells_per_bandclass_cdma2000_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("arfcn", arfcn);
  j.start_array("physCellIdList-r11");
  for (const auto& e1 : pci_list_r11) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// NS-PmaxValue-r10 ::= SEQUENCE
SRSASN_CODE ns_pmax_value_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(add_pmax_r10_present, 1));

  if (add_pmax_r10_present) {
    HANDLE_CODE(pack_integer(bref, add_pmax_r10, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pack_integer(bref, add_spec_emission, (uint8_t)1u, (uint8_t)32u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ns_pmax_value_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(add_pmax_r10_present, 1));

  if (add_pmax_r10_present) {
    HANDLE_CODE(unpack_integer(add_pmax_r10, bref, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(unpack_integer(add_spec_emission, bref, (uint8_t)1u, (uint8_t)32u));

  return SRSASN_SUCCESS;
}
void ns_pmax_value_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (add_pmax_r10_present) {
    j.write_int("additionalPmax-r10", add_pmax_r10);
  }
  j.write_int("additionalSpectrumEmission", add_spec_emission);
  j.end_obj();
}

// NS-PmaxValueNR-r15 ::= SEQUENCE
SRSASN_CODE ns_pmax_value_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(add_pmax_nr_r15_present, 1));

  if (add_pmax_nr_r15_present) {
    HANDLE_CODE(pack_integer(bref, add_pmax_nr_r15, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pack_integer(bref, add_spec_emission_nr_r15, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ns_pmax_value_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(add_pmax_nr_r15_present, 1));

  if (add_pmax_nr_r15_present) {
    HANDLE_CODE(unpack_integer(add_pmax_nr_r15, bref, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(unpack_integer(add_spec_emission_nr_r15, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void ns_pmax_value_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (add_pmax_nr_r15_present) {
    j.write_int("additionalPmaxNR-r15", add_pmax_nr_r15);
  }
  j.write_int("additionalSpectrumEmissionNR-r15", add_spec_emission_nr_r15);
  j.end_obj();
}

// NeighCellCDMA2000-r11 ::= SEQUENCE
SRSASN_CODE neigh_cell_cdma2000_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(band_class.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, neigh_freq_info_list_r11, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cell_cdma2000_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(band_class.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(neigh_freq_info_list_r11, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void neigh_cell_cdma2000_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("bandClass", band_class.to_string());
  j.start_array("neighFreqInfoList-r11");
  for (const auto& e1 : neigh_freq_info_list_r11) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// NeighCellsPerBandclassCDMA2000 ::= SEQUENCE
SRSASN_CODE neigh_cells_per_bandclass_cdma2000_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, arfcn, (uint16_t)0u, (uint16_t)2047u));
  HANDLE_CODE(pack_dyn_seq_of(bref, pci_list, 1, 16, integer_packer<uint16_t>(0, 511)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cells_per_bandclass_cdma2000_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(arfcn, bref, (uint16_t)0u, (uint16_t)2047u));
  HANDLE_CODE(unpack_dyn_seq_of(pci_list, bref, 1, 16, integer_packer<uint16_t>(0, 511)));

  return SRSASN_SUCCESS;
}
void neigh_cells_per_bandclass_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("arfcn", arfcn);
  j.start_array("physCellIdList");
  for (const auto& e1 : pci_list) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// NeighCellsPerBandclassCDMA2000-v920 ::= SEQUENCE
SRSASN_CODE neigh_cells_per_bandclass_cdma2000_v920_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, pci_list_v920, 0, 24, integer_packer<uint16_t>(0, 511)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cells_per_bandclass_cdma2000_v920_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(pci_list_v920, bref, 0, 24, integer_packer<uint16_t>(0, 511)));

  return SRSASN_SUCCESS;
}
void neigh_cells_per_bandclass_cdma2000_v920_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("physCellIdList-v920");
  for (const auto& e1 : pci_list_v920) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// PhysCellIdRangeNR-r16 ::= SEQUENCE
SRSASN_CODE pci_range_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(range_present, 1));

  HANDLE_CODE(pack_integer(bref, start, (uint16_t)0u, (uint16_t)1007u));
  if (range_present) {
    HANDLE_CODE(range.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pci_range_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(range_present, 1));

  HANDLE_CODE(unpack_integer(start, bref, (uint16_t)0u, (uint16_t)1007u));
  if (range_present) {
    HANDLE_CODE(range.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pci_range_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("start", start);
  if (range_present) {
    j.write_str("range", range.to_string());
  }
  j.end_obj();
}

const char* pci_range_nr_r16_s::range_opts::to_string() const
{
  static const char* options[] = {"n4",
                                  "n8",
                                  "n12",
                                  "n16",
                                  "n24",
                                  "n32",
                                  "n48",
                                  "n64",
                                  "n84",
                                  "n96",
                                  "n128",
                                  "n168",
                                  "n252",
                                  "n504",
                                  "n1008",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pci_range_nr_r16_s::range_e_");
}
uint16_t pci_range_nr_r16_s::range_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 12, 16, 24, 32, 48, 64, 84, 96, 128, 168, 252, 504, 1008};
  return map_enum_number(options, 15, value, "pci_range_nr_r16_s::range_e_");
}

// RedistributionNeighCell-r13 ::= SEQUENCE
SRSASN_CODE redist_neigh_cell_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pci_r13, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(pack_integer(bref, redist_factor_cell_r13, (uint8_t)1u, (uint8_t)10u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE redist_neigh_cell_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pci_r13, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(unpack_integer(redist_factor_cell_r13, bref, (uint8_t)1u, (uint8_t)10u));

  return SRSASN_SUCCESS;
}
void redist_neigh_cell_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r13", pci_r13);
  j.write_int("redistributionFactorCell-r13", redist_factor_cell_r13);
  j.end_obj();
}

// AC-BarringConfig1XRTT-r9 ::= SEQUENCE
SRSASN_CODE ac_barr_cfg1_xrtt_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ac_barr0to9_r9, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, ac_barr10_r9, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, ac_barr11_r9, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, ac_barr12_r9, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, ac_barr13_r9, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, ac_barr14_r9, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, ac_barr15_r9, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, ac_barr_msg_r9, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, ac_barr_reg_r9, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, ac_barr_emg_r9, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ac_barr_cfg1_xrtt_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ac_barr0to9_r9, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(ac_barr10_r9, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(ac_barr11_r9, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(ac_barr12_r9, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(ac_barr13_r9, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(ac_barr14_r9, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(ac_barr15_r9, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(ac_barr_msg_r9, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(ac_barr_reg_r9, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(ac_barr_emg_r9, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void ac_barr_cfg1_xrtt_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ac-Barring0to9-r9", ac_barr0to9_r9);
  j.write_int("ac-Barring10-r9", ac_barr10_r9);
  j.write_int("ac-Barring11-r9", ac_barr11_r9);
  j.write_int("ac-Barring12-r9", ac_barr12_r9);
  j.write_int("ac-Barring13-r9", ac_barr13_r9);
  j.write_int("ac-Barring14-r9", ac_barr14_r9);
  j.write_int("ac-Barring15-r9", ac_barr15_r9);
  j.write_int("ac-BarringMsg-r9", ac_barr_msg_r9);
  j.write_int("ac-BarringReg-r9", ac_barr_reg_r9);
  j.write_int("ac-BarringEmg-r9", ac_barr_emg_r9);
  j.end_obj();
}

// BeamMeasConfigIdleNR-r16 ::= SEQUENCE
SRSASN_CODE beam_meas_cfg_idle_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(report_quant_rs_idx_nr_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_report_rs_idx_r16, (uint8_t)0u, (uint8_t)32u));
  HANDLE_CODE(bref.pack(report_rs_idx_results_nr_r16, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE beam_meas_cfg_idle_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(report_quant_rs_idx_nr_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(max_report_rs_idx_r16, bref, (uint8_t)0u, (uint8_t)32u));
  HANDLE_CODE(bref.unpack(report_rs_idx_results_nr_r16, 1));

  return SRSASN_SUCCESS;
}
void beam_meas_cfg_idle_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("reportQuantityRS-IndexNR-r16", report_quant_rs_idx_nr_r16.to_string());
  j.write_int("maxReportRS-Index-r16", max_report_rs_idx_r16);
  j.write_bool("reportRS-IndexResultsNR-r16", report_rs_idx_results_nr_r16);
  j.end_obj();
}

const char* beam_meas_cfg_idle_nr_r16_s::report_quant_rs_idx_nr_r16_opts::to_string() const
{
  static const char* options[] = {"rsrp", "rsrq", "both"};
  return convert_enum_idx(options, 3, value, "beam_meas_cfg_idle_nr_r16_s::report_quant_rs_idx_nr_r16_e_");
}

// CSFB-RegistrationParam1XRTT ::= SEQUENCE
SRSASN_CODE csfb_regist_param1_xrtt_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sid.pack(bref));
  HANDLE_CODE(nid.pack(bref));
  HANDLE_CODE(bref.pack(multiple_sid, 1));
  HANDLE_CODE(bref.pack(multiple_nid, 1));
  HANDLE_CODE(bref.pack(home_reg, 1));
  HANDLE_CODE(bref.pack(foreign_sid_reg, 1));
  HANDLE_CODE(bref.pack(foreign_nid_reg, 1));
  HANDLE_CODE(bref.pack(param_reg, 1));
  HANDLE_CODE(bref.pack(pwr_up_reg, 1));
  HANDLE_CODE(regist_period.pack(bref));
  HANDLE_CODE(regist_zone.pack(bref));
  HANDLE_CODE(total_zone.pack(bref));
  HANDLE_CODE(zone_timer.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csfb_regist_param1_xrtt_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sid.unpack(bref));
  HANDLE_CODE(nid.unpack(bref));
  HANDLE_CODE(bref.unpack(multiple_sid, 1));
  HANDLE_CODE(bref.unpack(multiple_nid, 1));
  HANDLE_CODE(bref.unpack(home_reg, 1));
  HANDLE_CODE(bref.unpack(foreign_sid_reg, 1));
  HANDLE_CODE(bref.unpack(foreign_nid_reg, 1));
  HANDLE_CODE(bref.unpack(param_reg, 1));
  HANDLE_CODE(bref.unpack(pwr_up_reg, 1));
  HANDLE_CODE(regist_period.unpack(bref));
  HANDLE_CODE(regist_zone.unpack(bref));
  HANDLE_CODE(total_zone.unpack(bref));
  HANDLE_CODE(zone_timer.unpack(bref));

  return SRSASN_SUCCESS;
}
void csfb_regist_param1_xrtt_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sid", sid.to_string());
  j.write_str("nid", nid.to_string());
  j.write_bool("multipleSID", multiple_sid);
  j.write_bool("multipleNID", multiple_nid);
  j.write_bool("homeReg", home_reg);
  j.write_bool("foreignSIDReg", foreign_sid_reg);
  j.write_bool("foreignNIDReg", foreign_nid_reg);
  j.write_bool("parameterReg", param_reg);
  j.write_bool("powerUpReg", pwr_up_reg);
  j.write_str("registrationPeriod", regist_period.to_string());
  j.write_str("registrationZone", regist_zone.to_string());
  j.write_str("totalZone", total_zone.to_string());
  j.write_str("zoneTimer", zone_timer.to_string());
  j.end_obj();
}

// CSFB-RegistrationParam1XRTT-v920 ::= SEQUENCE
SRSASN_CODE csfb_regist_param1_xrtt_v920_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE csfb_regist_param1_xrtt_v920_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void csfb_regist_param1_xrtt_v920_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("powerDownReg-r9", "true");
  j.end_obj();
}

// CellReselectionParametersCDMA2000-r11 ::= SEQUENCE
SRSASN_CODE cell_resel_params_cdma2000_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(t_resel_cdma2000_sf_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, band_class_list, 1, 32));
  HANDLE_CODE(pack_dyn_seq_of(bref, neigh_cell_list_r11, 1, 16));
  HANDLE_CODE(pack_integer(bref, t_resel_cdma2000, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_cdma2000_sf_present) {
    HANDLE_CODE(t_resel_cdma2000_sf.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_resel_params_cdma2000_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(t_resel_cdma2000_sf_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(band_class_list, bref, 1, 32));
  HANDLE_CODE(unpack_dyn_seq_of(neigh_cell_list_r11, bref, 1, 16));
  HANDLE_CODE(unpack_integer(t_resel_cdma2000, bref, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_cdma2000_sf_present) {
    HANDLE_CODE(t_resel_cdma2000_sf.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cell_resel_params_cdma2000_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("bandClassList");
  for (const auto& e1 : band_class_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("neighCellList-r11");
  for (const auto& e1 : neigh_cell_list_r11) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("t-ReselectionCDMA2000", t_resel_cdma2000);
  if (t_resel_cdma2000_sf_present) {
    j.write_fieldname("t-ReselectionCDMA2000-SF");
    t_resel_cdma2000_sf.to_json(j);
  }
  j.end_obj();
}

// InterFreqNeighCellInfo ::= SEQUENCE
SRSASN_CODE inter_freq_neigh_cell_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(q_offset_cell.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_neigh_cell_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(q_offset_cell.unpack(bref));

  return SRSASN_SUCCESS;
}
void inter_freq_neigh_cell_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  j.write_str("q-OffsetCell", q_offset_cell.to_string());
  j.end_obj();
}

// InterFreqNeighCellInfo-v1610 ::= SEQUENCE
SRSASN_CODE inter_freq_neigh_cell_info_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rss_meas_pwr_bias_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_neigh_cell_info_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rss_meas_pwr_bias_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void inter_freq_neigh_cell_info_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rss-MeasPowerBias-r16", rss_meas_pwr_bias_r16.to_string());
  j.end_obj();
}

// PLMN-IdentityInfo2-r12 ::= CHOICE
void plmn_id_info2_r12_c::destroy_()
{
  switch (type_) {
    case types::plmn_id_r12:
      c.destroy<plmn_id_s>();
      break;
    default:
      break;
  }
}
void plmn_id_info2_r12_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::plmn_idx_r12:
      break;
    case types::plmn_id_r12:
      c.init<plmn_id_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_info2_r12_c");
  }
}
plmn_id_info2_r12_c::plmn_id_info2_r12_c(const plmn_id_info2_r12_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::plmn_idx_r12:
      c.init(other.c.get<uint8_t>());
      break;
    case types::plmn_id_r12:
      c.init(other.c.get<plmn_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_info2_r12_c");
  }
}
plmn_id_info2_r12_c& plmn_id_info2_r12_c::operator=(const plmn_id_info2_r12_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::plmn_idx_r12:
      c.set(other.c.get<uint8_t>());
      break;
    case types::plmn_id_r12:
      c.set(other.c.get<plmn_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_info2_r12_c");
  }

  return *this;
}
uint8_t& plmn_id_info2_r12_c::set_plmn_idx_r12()
{
  set(types::plmn_idx_r12);
  return c.get<uint8_t>();
}
plmn_id_s& plmn_id_info2_r12_c::set_plmn_id_r12()
{
  set(types::plmn_id_r12);
  return c.get<plmn_id_s>();
}
void plmn_id_info2_r12_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::plmn_idx_r12:
      j.write_int("plmn-Index-r12", c.get<uint8_t>());
      break;
    case types::plmn_id_r12:
      j.write_fieldname("plmnIdentity-r12");
      c.get<plmn_id_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_info2_r12_c");
  }
  j.end_obj();
}
SRSASN_CODE plmn_id_info2_r12_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::plmn_idx_r12:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)6u));
      break;
    case types::plmn_id_r12:
      HANDLE_CODE(c.get<plmn_id_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_info2_r12_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_info2_r12_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::plmn_idx_r12:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)6u));
      break;
    case types::plmn_id_r12:
      HANDLE_CODE(c.get<plmn_id_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_info2_r12_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SystemTimeInfoCDMA2000 ::= SEQUENCE
SRSASN_CODE sys_time_info_cdma2000_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cdma_eutra_synchronisation, 1));
  HANDLE_CODE(cdma_sys_time.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_time_info_cdma2000_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cdma_eutra_synchronisation, 1));
  HANDLE_CODE(cdma_sys_time.unpack(bref));

  return SRSASN_SUCCESS;
}
void sys_time_info_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("cdma-EUTRA-Synchronisation", cdma_eutra_synchronisation);
  j.write_fieldname("cdma-SystemTime");
  cdma_sys_time.to_json(j);
  j.end_obj();
}

void sys_time_info_cdma2000_s::cdma_sys_time_c_::destroy_()
{
  switch (type_) {
    case types::sync_sys_time:
      c.destroy<fixed_bitstring<39> >();
      break;
    case types::async_sys_time:
      c.destroy<fixed_bitstring<49> >();
      break;
    default:
      break;
  }
}
void sys_time_info_cdma2000_s::cdma_sys_time_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sync_sys_time:
      c.init<fixed_bitstring<39> >();
      break;
    case types::async_sys_time:
      c.init<fixed_bitstring<49> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_time_info_cdma2000_s::cdma_sys_time_c_");
  }
}
sys_time_info_cdma2000_s::cdma_sys_time_c_::cdma_sys_time_c_(const sys_time_info_cdma2000_s::cdma_sys_time_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sync_sys_time:
      c.init(other.c.get<fixed_bitstring<39> >());
      break;
    case types::async_sys_time:
      c.init(other.c.get<fixed_bitstring<49> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_time_info_cdma2000_s::cdma_sys_time_c_");
  }
}
sys_time_info_cdma2000_s::cdma_sys_time_c_&
sys_time_info_cdma2000_s::cdma_sys_time_c_::operator=(const sys_time_info_cdma2000_s::cdma_sys_time_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sync_sys_time:
      c.set(other.c.get<fixed_bitstring<39> >());
      break;
    case types::async_sys_time:
      c.set(other.c.get<fixed_bitstring<49> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_time_info_cdma2000_s::cdma_sys_time_c_");
  }

  return *this;
}
fixed_bitstring<39>& sys_time_info_cdma2000_s::cdma_sys_time_c_::set_sync_sys_time()
{
  set(types::sync_sys_time);
  return c.get<fixed_bitstring<39> >();
}
fixed_bitstring<49>& sys_time_info_cdma2000_s::cdma_sys_time_c_::set_async_sys_time()
{
  set(types::async_sys_time);
  return c.get<fixed_bitstring<49> >();
}
void sys_time_info_cdma2000_s::cdma_sys_time_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sync_sys_time:
      j.write_str("synchronousSystemTime", c.get<fixed_bitstring<39> >().to_string());
      break;
    case types::async_sys_time:
      j.write_str("asynchronousSystemTime", c.get<fixed_bitstring<49> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "sys_time_info_cdma2000_s::cdma_sys_time_c_");
  }
  j.end_obj();
}
SRSASN_CODE sys_time_info_cdma2000_s::cdma_sys_time_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sync_sys_time:
      HANDLE_CODE(c.get<fixed_bitstring<39> >().pack(bref));
      break;
    case types::async_sys_time:
      HANDLE_CODE(c.get<fixed_bitstring<49> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_time_info_cdma2000_s::cdma_sys_time_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_time_info_cdma2000_s::cdma_sys_time_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sync_sys_time:
      HANDLE_CODE(c.get<fixed_bitstring<39> >().unpack(bref));
      break;
    case types::async_sys_time:
      HANDLE_CODE(c.get<fixed_bitstring<49> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_time_info_cdma2000_s::cdma_sys_time_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UAC-BarringPerCat-r15 ::= SEQUENCE
SRSASN_CODE uac_barr_per_cat_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, access_category_r15, (uint8_t)1u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, uac_barr_info_set_idx_r15, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_per_cat_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(access_category_r15, bref, (uint8_t)1u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(uac_barr_info_set_idx_r15, bref, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
void uac_barr_per_cat_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("accessCategory-r15", access_category_r15);
  j.write_int("uac-barringInfoSetIndex-r15", uac_barr_info_set_idx_r15);
  j.end_obj();
}

// MBSFN-AreaInfo-r16 ::= SEQUENCE
SRSASN_CODE mbsfn_area_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(time_separation_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, mbsfn_area_id_r16, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(pack_integer(bref, notif_ind_r16, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(mcch_cfg_r16.mcch_repeat_period_r16.pack(bref));
  HANDLE_CODE(mcch_cfg_r16.mcch_mod_period_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, mcch_cfg_r16.mcch_offset_r16, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(mcch_cfg_r16.sf_alloc_info_r16.pack(bref));
  HANDLE_CODE(mcch_cfg_r16.sig_mcs_r16.pack(bref));
  HANDLE_CODE(subcarrier_spacing_mbms_r16.pack(bref));
  if (time_separation_r16_present) {
    HANDLE_CODE(time_separation_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbsfn_area_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(time_separation_r16_present, 1));

  HANDLE_CODE(unpack_integer(mbsfn_area_id_r16, bref, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(unpack_integer(notif_ind_r16, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(mcch_cfg_r16.mcch_repeat_period_r16.unpack(bref));
  HANDLE_CODE(mcch_cfg_r16.mcch_mod_period_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(mcch_cfg_r16.mcch_offset_r16, bref, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(mcch_cfg_r16.sf_alloc_info_r16.unpack(bref));
  HANDLE_CODE(mcch_cfg_r16.sig_mcs_r16.unpack(bref));
  HANDLE_CODE(subcarrier_spacing_mbms_r16.unpack(bref));
  if (time_separation_r16_present) {
    HANDLE_CODE(time_separation_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbsfn_area_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("mbsfn-AreaId-r16", mbsfn_area_id_r16);
  j.write_int("notificationIndicator-r16", notif_ind_r16);
  j.write_fieldname("mcch-Config-r16");
  j.start_obj();
  j.write_str("mcch-RepetitionPeriod-r16", mcch_cfg_r16.mcch_repeat_period_r16.to_string());
  j.write_str("mcch-ModificationPeriod-r16", mcch_cfg_r16.mcch_mod_period_r16.to_string());
  j.write_int("mcch-Offset-r16", mcch_cfg_r16.mcch_offset_r16);
  j.write_str("sf-AllocInfo-r16", mcch_cfg_r16.sf_alloc_info_r16.to_string());
  j.write_str("signallingMCS-r16", mcch_cfg_r16.sig_mcs_r16.to_string());
  j.end_obj();
  j.write_str("subcarrierSpacingMBMS-r16", subcarrier_spacing_mbms_r16.to_string());
  if (time_separation_r16_present) {
    j.write_str("timeSeparation-r16", time_separation_r16.to_string());
  }
  j.end_obj();
}

const char* mbsfn_area_info_r16_s::mcch_cfg_r16_s_::mcch_repeat_period_r16_opts::to_string() const
{
  static const char* options[] = {"rf1",
                                  "rf2",
                                  "rf4",
                                  "rf8",
                                  "rf16",
                                  "rf32",
                                  "rf64",
                                  "rf128",
                                  "rf256",
                                  "spare7",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "mbsfn_area_info_r16_s::mcch_cfg_r16_s_::mcch_repeat_period_r16_e_");
}
uint16_t mbsfn_area_info_r16_s::mcch_cfg_r16_s_::mcch_repeat_period_r16_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 9, value, "mbsfn_area_info_r16_s::mcch_cfg_r16_s_::mcch_repeat_period_r16_e_");
}

const char* mbsfn_area_info_r16_s::mcch_cfg_r16_s_::mcch_mod_period_r16_opts::to_string() const
{
  static const char* options[] = {"rf1",
                                  "rf2",
                                  "rf4",
                                  "rf8",
                                  "rf16",
                                  "rf32",
                                  "rf64",
                                  "rf128",
                                  "rf256",
                                  "rf512",
                                  "rf1024",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "mbsfn_area_info_r16_s::mcch_cfg_r16_s_::mcch_mod_period_r16_e_");
}
uint16_t mbsfn_area_info_r16_s::mcch_cfg_r16_s_::mcch_mod_period_r16_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
  return map_enum_number(options, 11, value, "mbsfn_area_info_r16_s::mcch_cfg_r16_s_::mcch_mod_period_r16_e_");
}

const char* mbsfn_area_info_r16_s::mcch_cfg_r16_s_::sig_mcs_r16_opts::to_string() const
{
  static const char* options[] = {"n2", "n7", "n13", "n19"};
  return convert_enum_idx(options, 4, value, "mbsfn_area_info_r16_s::mcch_cfg_r16_s_::sig_mcs_r16_e_");
}
uint8_t mbsfn_area_info_r16_s::mcch_cfg_r16_s_::sig_mcs_r16_opts::to_number() const
{
  static const uint8_t options[] = {2, 7, 13, 19};
  return map_enum_number(options, 4, value, "mbsfn_area_info_r16_s::mcch_cfg_r16_s_::sig_mcs_r16_e_");
}

const char* mbsfn_area_info_r16_s::subcarrier_spacing_mbms_r16_opts::to_string() const
{
  static const char* options[] = {
      "kHz7dot5", "kHz2dot5", "kHz1dot25", "kHz0dot37", "kHz15-v1710", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "mbsfn_area_info_r16_s::subcarrier_spacing_mbms_r16_e_");
}
float mbsfn_area_info_r16_s::subcarrier_spacing_mbms_r16_opts::to_number() const
{
  static const float options[] = {7.5, 2.5, 1.25, 0.37, 15.0};
  return map_enum_number(options, 5, value, "mbsfn_area_info_r16_s::subcarrier_spacing_mbms_r16_e_");
}
const char* mbsfn_area_info_r16_s::subcarrier_spacing_mbms_r16_opts::to_number_string() const
{
  static const char* options[] = {"7.5", "2.5", "1.25", "0.37", "15"};
  return convert_enum_idx(options, 8, value, "mbsfn_area_info_r16_s::subcarrier_spacing_mbms_r16_e_");
}

const char* mbsfn_area_info_r16_s::time_separation_r16_opts::to_string() const
{
  static const char* options[] = {"sl2", "sl4"};
  return convert_enum_idx(options, 2, value, "mbsfn_area_info_r16_s::time_separation_r16_e_");
}
uint8_t mbsfn_area_info_r16_s::time_separation_r16_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "mbsfn_area_info_r16_s::time_separation_r16_e_");
}

// MTC-SSB2-LP-NR-r16 ::= SEQUENCE
SRSASN_CODE mtc_ssb2_lp_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pci_list_r16_present, 1));

  if (pci_list_r16_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pci_list_r16, 1, 64, integer_packer<uint16_t>(0, 1007)));
  }
  HANDLE_CODE(periodicity_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mtc_ssb2_lp_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pci_list_r16_present, 1));

  if (pci_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pci_list_r16, bref, 1, 64, integer_packer<uint16_t>(0, 1007)));
  }
  HANDLE_CODE(periodicity_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void mtc_ssb2_lp_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pci_list_r16_present) {
    j.start_array("pci-List-r16");
    for (const auto& e1 : pci_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.write_str("periodicity-r16", periodicity_r16.to_string());
  j.end_obj();
}

const char* mtc_ssb2_lp_nr_r16_s::periodicity_r16_opts::to_string() const
{
  static const char* options[] = {"sf10", "sf20", "sf40", "sf80", "sf160", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "mtc_ssb2_lp_nr_r16_s::periodicity_r16_e_");
}
uint8_t mtc_ssb2_lp_nr_r16_s::periodicity_r16_opts::to_number() const
{
  static const uint8_t options[] = {10, 20, 40, 80, 160};
  return map_enum_number(options, 5, value, "mtc_ssb2_lp_nr_r16_s::periodicity_r16_e_");
}

// MeasIdleCarrierNR-r16 ::= SEQUENCE
SRSASN_CODE meas_idle_carrier_nr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_band_list_present, 1));
  HANDLE_CODE(bref.pack(meas_cell_list_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(quality_thres_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(ssb_meas_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(beam_meas_cfg_idle_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_nr_r16, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(subcarrier_spacing_ssb_r16.pack(bref));
  if (freq_band_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list, 1, 32, integer_packer<uint16_t>(1, 1024)));
  }
  if (meas_cell_list_nr_r16_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_cell_list_nr_r16, 1, 8));
  }
  HANDLE_CODE(report_quantities_nr_r16.pack(bref));
  if (quality_thres_nr_r16_present) {
    HANDLE_CODE(bref.pack(quality_thres_nr_r16.idle_rsrp_thres_nr_r16_present, 1));
    HANDLE_CODE(bref.pack(quality_thres_nr_r16.idle_rsrq_thres_nr_r16_present, 1));
    if (quality_thres_nr_r16.idle_rsrp_thres_nr_r16_present) {
      HANDLE_CODE(pack_integer(bref, quality_thres_nr_r16.idle_rsrp_thres_nr_r16, (uint8_t)0u, (uint8_t)127u));
    }
    if (quality_thres_nr_r16.idle_rsrq_thres_nr_r16_present) {
      HANDLE_CODE(pack_integer(bref, quality_thres_nr_r16.idle_rsrq_thres_nr_r16, (uint8_t)0u, (uint8_t)127u));
    }
  }
  if (ssb_meas_cfg_r16_present) {
    HANDLE_CODE(bref.pack(ssb_meas_cfg_r16.max_rs_idx_cell_qual_r16_present, 1));
    HANDLE_CODE(bref.pack(ssb_meas_cfg_r16.thresh_rs_idx_r16_present, 1));
    HANDLE_CODE(bref.pack(ssb_meas_cfg_r16.meas_timing_cfg_r16_present, 1));
    HANDLE_CODE(bref.pack(ssb_meas_cfg_r16.ssb_to_measure_r16_present, 1));
    HANDLE_CODE(bref.pack(ssb_meas_cfg_r16.ss_rssi_meas_r16_present, 1));
    if (ssb_meas_cfg_r16.max_rs_idx_cell_qual_r16_present) {
      HANDLE_CODE(pack_integer(bref, ssb_meas_cfg_r16.max_rs_idx_cell_qual_r16, (uint8_t)1u, (uint8_t)16u));
    }
    if (ssb_meas_cfg_r16.thresh_rs_idx_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.thresh_rs_idx_r16.pack(bref));
    }
    if (ssb_meas_cfg_r16.meas_timing_cfg_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.meas_timing_cfg_r16.pack(bref));
    }
    if (ssb_meas_cfg_r16.ssb_to_measure_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.ssb_to_measure_r16.pack(bref));
    }
    HANDLE_CODE(bref.pack(ssb_meas_cfg_r16.derive_ssb_idx_from_cell_r16, 1));
    if (ssb_meas_cfg_r16.ss_rssi_meas_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.ss_rssi_meas_r16.pack(bref));
    }
  }
  if (beam_meas_cfg_idle_r16_present) {
    HANDLE_CODE(beam_meas_cfg_idle_r16.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= subcarrier_spacing_ssb_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(subcarrier_spacing_ssb_r17_present, 1));
      if (subcarrier_spacing_ssb_r17_present) {
        HANDLE_CODE(subcarrier_spacing_ssb_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_idle_carrier_nr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(freq_band_list_present, 1));
  HANDLE_CODE(bref.unpack(meas_cell_list_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(quality_thres_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(beam_meas_cfg_idle_r16_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_nr_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(subcarrier_spacing_ssb_r16.unpack(bref));
  if (freq_band_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list, bref, 1, 32, integer_packer<uint16_t>(1, 1024)));
  }
  if (meas_cell_list_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_cell_list_nr_r16, bref, 1, 8));
  }
  HANDLE_CODE(report_quantities_nr_r16.unpack(bref));
  if (quality_thres_nr_r16_present) {
    HANDLE_CODE(bref.unpack(quality_thres_nr_r16.idle_rsrp_thres_nr_r16_present, 1));
    HANDLE_CODE(bref.unpack(quality_thres_nr_r16.idle_rsrq_thres_nr_r16_present, 1));
    if (quality_thres_nr_r16.idle_rsrp_thres_nr_r16_present) {
      HANDLE_CODE(unpack_integer(quality_thres_nr_r16.idle_rsrp_thres_nr_r16, bref, (uint8_t)0u, (uint8_t)127u));
    }
    if (quality_thres_nr_r16.idle_rsrq_thres_nr_r16_present) {
      HANDLE_CODE(unpack_integer(quality_thres_nr_r16.idle_rsrq_thres_nr_r16, bref, (uint8_t)0u, (uint8_t)127u));
    }
  }
  if (ssb_meas_cfg_r16_present) {
    HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16.max_rs_idx_cell_qual_r16_present, 1));
    HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16.thresh_rs_idx_r16_present, 1));
    HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16.meas_timing_cfg_r16_present, 1));
    HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16.ssb_to_measure_r16_present, 1));
    HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16.ss_rssi_meas_r16_present, 1));
    if (ssb_meas_cfg_r16.max_rs_idx_cell_qual_r16_present) {
      HANDLE_CODE(unpack_integer(ssb_meas_cfg_r16.max_rs_idx_cell_qual_r16, bref, (uint8_t)1u, (uint8_t)16u));
    }
    if (ssb_meas_cfg_r16.thresh_rs_idx_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.thresh_rs_idx_r16.unpack(bref));
    }
    if (ssb_meas_cfg_r16.meas_timing_cfg_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.meas_timing_cfg_r16.unpack(bref));
    }
    if (ssb_meas_cfg_r16.ssb_to_measure_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.ssb_to_measure_r16.unpack(bref));
    }
    HANDLE_CODE(bref.unpack(ssb_meas_cfg_r16.derive_ssb_idx_from_cell_r16, 1));
    if (ssb_meas_cfg_r16.ss_rssi_meas_r16_present) {
      HANDLE_CODE(ssb_meas_cfg_r16.ss_rssi_meas_r16.unpack(bref));
    }
  }
  if (beam_meas_cfg_idle_r16_present) {
    HANDLE_CODE(beam_meas_cfg_idle_r16.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(subcarrier_spacing_ssb_r17_present, 1));
      if (subcarrier_spacing_ssb_r17_present) {
        HANDLE_CODE(subcarrier_spacing_ssb_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_idle_carrier_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreqNR-r16", carrier_freq_nr_r16);
  j.write_str("subcarrierSpacingSSB-r16", subcarrier_spacing_ssb_r16.to_string());
  if (freq_band_list_present) {
    j.start_array("frequencyBandList");
    for (const auto& e1 : freq_band_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (meas_cell_list_nr_r16_present) {
    j.start_array("measCellListNR-r16");
    for (const auto& e1 : meas_cell_list_nr_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("reportQuantitiesNR-r16", report_quantities_nr_r16.to_string());
  if (quality_thres_nr_r16_present) {
    j.write_fieldname("qualityThresholdNR-r16");
    j.start_obj();
    if (quality_thres_nr_r16.idle_rsrp_thres_nr_r16_present) {
      j.write_int("idleRSRP-ThresholdNR-r16", quality_thres_nr_r16.idle_rsrp_thres_nr_r16);
    }
    if (quality_thres_nr_r16.idle_rsrq_thres_nr_r16_present) {
      j.write_int("idleRSRQ-ThresholdNR-r16", quality_thres_nr_r16.idle_rsrq_thres_nr_r16);
    }
    j.end_obj();
  }
  if (ssb_meas_cfg_r16_present) {
    j.write_fieldname("ssb-MeasConfig-r16");
    j.start_obj();
    if (ssb_meas_cfg_r16.max_rs_idx_cell_qual_r16_present) {
      j.write_int("maxRS-IndexCellQual-r16", ssb_meas_cfg_r16.max_rs_idx_cell_qual_r16);
    }
    if (ssb_meas_cfg_r16.thresh_rs_idx_r16_present) {
      j.write_fieldname("threshRS-Index-r16");
      ssb_meas_cfg_r16.thresh_rs_idx_r16.to_json(j);
    }
    if (ssb_meas_cfg_r16.meas_timing_cfg_r16_present) {
      j.write_fieldname("measTimingConfig-r16");
      ssb_meas_cfg_r16.meas_timing_cfg_r16.to_json(j);
    }
    if (ssb_meas_cfg_r16.ssb_to_measure_r16_present) {
      j.write_fieldname("ssb-ToMeasure-r16");
      ssb_meas_cfg_r16.ssb_to_measure_r16.to_json(j);
    }
    j.write_bool("deriveSSB-IndexFromCell-r16", ssb_meas_cfg_r16.derive_ssb_idx_from_cell_r16);
    if (ssb_meas_cfg_r16.ss_rssi_meas_r16_present) {
      j.write_fieldname("ss-RSSI-Measurement-r16");
      ssb_meas_cfg_r16.ss_rssi_meas_r16.to_json(j);
    }
    j.end_obj();
  }
  if (beam_meas_cfg_idle_r16_present) {
    j.write_fieldname("beamMeasConfigIdle-r16");
    beam_meas_cfg_idle_r16.to_json(j);
  }
  if (ext) {
    if (subcarrier_spacing_ssb_r17_present) {
      j.write_str("subcarrierSpacingSSB-r17", subcarrier_spacing_ssb_r17.to_string());
    }
  }
  j.end_obj();
}

const char* meas_idle_carrier_nr_r16_s::subcarrier_spacing_ssb_r16_opts::to_string() const
{
  static const char* options[] = {"kHz15", "kHz30", "kHz120", "kHz240"};
  return convert_enum_idx(options, 4, value, "meas_idle_carrier_nr_r16_s::subcarrier_spacing_ssb_r16_e_");
}
uint8_t meas_idle_carrier_nr_r16_s::subcarrier_spacing_ssb_r16_opts::to_number() const
{
  static const uint8_t options[] = {15, 30, 120, 240};
  return map_enum_number(options, 4, value, "meas_idle_carrier_nr_r16_s::subcarrier_spacing_ssb_r16_e_");
}

const char* meas_idle_carrier_nr_r16_s::report_quantities_nr_r16_opts::to_string() const
{
  static const char* options[] = {"rsrp", "rsrq", "both"};
  return convert_enum_idx(options, 3, value, "meas_idle_carrier_nr_r16_s::report_quantities_nr_r16_e_");
}

const char* meas_idle_carrier_nr_r16_s::subcarrier_spacing_ssb_r17_opts::to_string() const
{
  static const char* options[] = {"kHz480", "spare1"};
  return convert_enum_idx(options, 2, value, "meas_idle_carrier_nr_r16_s::subcarrier_spacing_ssb_r17_e_");
}
uint16_t meas_idle_carrier_nr_r16_s::subcarrier_spacing_ssb_r17_opts::to_number() const
{
  static const uint16_t options[] = {480};
  return map_enum_number(options, 1, value, "meas_idle_carrier_nr_r16_s::subcarrier_spacing_ssb_r17_e_");
}

// NeighCellCDMA2000 ::= SEQUENCE
SRSASN_CODE neigh_cell_cdma2000_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(band_class.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, neigh_cells_per_freq_list, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cell_cdma2000_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(band_class.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(neigh_cells_per_freq_list, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void neigh_cell_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("bandClass", band_class.to_string());
  j.start_array("neighCellsPerFreqList");
  for (const auto& e1 : neigh_cells_per_freq_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// NeighCellCDMA2000-v920 ::= SEQUENCE
SRSASN_CODE neigh_cell_cdma2000_v920_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, neigh_cells_per_freq_list_v920, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cell_cdma2000_v920_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(neigh_cells_per_freq_list_v920, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void neigh_cell_cdma2000_v920_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("neighCellsPerFreqList-v920");
  for (const auto& e1 : neigh_cells_per_freq_list_v920) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// ParametersCDMA2000-r11 ::= SEQUENCE
SRSASN_CODE params_cdma2000_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sys_time_info_r11_present, 1));
  HANDLE_CODE(bref.pack(params_hrpd_r11_present, 1));
  HANDLE_CODE(bref.pack(params1_xrtt_r11_present, 1));

  if (sys_time_info_r11_present) {
    HANDLE_CODE(sys_time_info_r11.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, search_win_size_r11, (uint8_t)0u, (uint8_t)15u));
  if (params_hrpd_r11_present) {
    HANDLE_CODE(bref.pack(params_hrpd_r11.cell_resel_params_hrpd_r11_present, 1));
    HANDLE_CODE(params_hrpd_r11.pre_regist_info_hrpd_r11.pack(bref));
    if (params_hrpd_r11.cell_resel_params_hrpd_r11_present) {
      HANDLE_CODE(params_hrpd_r11.cell_resel_params_hrpd_r11.pack(bref));
    }
  }
  if (params1_xrtt_r11_present) {
    HANDLE_CODE(bref.pack(params1_xrtt_r11.csfb_regist_param1_xrtt_r11_present, 1));
    HANDLE_CODE(bref.pack(params1_xrtt_r11.csfb_regist_param1_xrtt_ext_r11_present, 1));
    HANDLE_CODE(bref.pack(params1_xrtt_r11.long_code_state1_xrtt_r11_present, 1));
    HANDLE_CODE(bref.pack(params1_xrtt_r11.cell_resel_params1_xrtt_r11_present, 1));
    HANDLE_CODE(bref.pack(params1_xrtt_r11.ac_barr_cfg1_xrtt_r11_present, 1));
    HANDLE_CODE(bref.pack(params1_xrtt_r11.csfb_support_for_dual_rx_ues_r11_present, 1));
    HANDLE_CODE(bref.pack(params1_xrtt_r11.csfb_dual_rx_tx_support_r11_present, 1));
    if (params1_xrtt_r11.csfb_regist_param1_xrtt_r11_present) {
      HANDLE_CODE(params1_xrtt_r11.csfb_regist_param1_xrtt_r11.pack(bref));
    }
    if (params1_xrtt_r11.csfb_regist_param1_xrtt_ext_r11_present) {
      HANDLE_CODE(params1_xrtt_r11.csfb_regist_param1_xrtt_ext_r11.pack(bref));
    }
    if (params1_xrtt_r11.long_code_state1_xrtt_r11_present) {
      HANDLE_CODE(params1_xrtt_r11.long_code_state1_xrtt_r11.pack(bref));
    }
    if (params1_xrtt_r11.cell_resel_params1_xrtt_r11_present) {
      HANDLE_CODE(params1_xrtt_r11.cell_resel_params1_xrtt_r11.pack(bref));
    }
    if (params1_xrtt_r11.ac_barr_cfg1_xrtt_r11_present) {
      HANDLE_CODE(params1_xrtt_r11.ac_barr_cfg1_xrtt_r11.pack(bref));
    }
    if (params1_xrtt_r11.csfb_support_for_dual_rx_ues_r11_present) {
      HANDLE_CODE(bref.pack(params1_xrtt_r11.csfb_support_for_dual_rx_ues_r11, 1));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE params_cdma2000_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sys_time_info_r11_present, 1));
  HANDLE_CODE(bref.unpack(params_hrpd_r11_present, 1));
  HANDLE_CODE(bref.unpack(params1_xrtt_r11_present, 1));

  if (sys_time_info_r11_present) {
    HANDLE_CODE(sys_time_info_r11.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(search_win_size_r11, bref, (uint8_t)0u, (uint8_t)15u));
  if (params_hrpd_r11_present) {
    HANDLE_CODE(bref.unpack(params_hrpd_r11.cell_resel_params_hrpd_r11_present, 1));
    HANDLE_CODE(params_hrpd_r11.pre_regist_info_hrpd_r11.unpack(bref));
    if (params_hrpd_r11.cell_resel_params_hrpd_r11_present) {
      HANDLE_CODE(params_hrpd_r11.cell_resel_params_hrpd_r11.unpack(bref));
    }
  }
  if (params1_xrtt_r11_present) {
    HANDLE_CODE(bref.unpack(params1_xrtt_r11.csfb_regist_param1_xrtt_r11_present, 1));
    HANDLE_CODE(bref.unpack(params1_xrtt_r11.csfb_regist_param1_xrtt_ext_r11_present, 1));
    HANDLE_CODE(bref.unpack(params1_xrtt_r11.long_code_state1_xrtt_r11_present, 1));
    HANDLE_CODE(bref.unpack(params1_xrtt_r11.cell_resel_params1_xrtt_r11_present, 1));
    HANDLE_CODE(bref.unpack(params1_xrtt_r11.ac_barr_cfg1_xrtt_r11_present, 1));
    HANDLE_CODE(bref.unpack(params1_xrtt_r11.csfb_support_for_dual_rx_ues_r11_present, 1));
    HANDLE_CODE(bref.unpack(params1_xrtt_r11.csfb_dual_rx_tx_support_r11_present, 1));
    if (params1_xrtt_r11.csfb_regist_param1_xrtt_r11_present) {
      HANDLE_CODE(params1_xrtt_r11.csfb_regist_param1_xrtt_r11.unpack(bref));
    }
    if (params1_xrtt_r11.csfb_regist_param1_xrtt_ext_r11_present) {
      HANDLE_CODE(params1_xrtt_r11.csfb_regist_param1_xrtt_ext_r11.unpack(bref));
    }
    if (params1_xrtt_r11.long_code_state1_xrtt_r11_present) {
      HANDLE_CODE(params1_xrtt_r11.long_code_state1_xrtt_r11.unpack(bref));
    }
    if (params1_xrtt_r11.cell_resel_params1_xrtt_r11_present) {
      HANDLE_CODE(params1_xrtt_r11.cell_resel_params1_xrtt_r11.unpack(bref));
    }
    if (params1_xrtt_r11.ac_barr_cfg1_xrtt_r11_present) {
      HANDLE_CODE(params1_xrtt_r11.ac_barr_cfg1_xrtt_r11.unpack(bref));
    }
    if (params1_xrtt_r11.csfb_support_for_dual_rx_ues_r11_present) {
      HANDLE_CODE(bref.unpack(params1_xrtt_r11.csfb_support_for_dual_rx_ues_r11, 1));
    }
  }

  return SRSASN_SUCCESS;
}
void params_cdma2000_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sys_time_info_r11_present) {
    j.write_fieldname("systemTimeInfo-r11");
    sys_time_info_r11.to_json(j);
  }
  j.write_int("searchWindowSize-r11", search_win_size_r11);
  if (params_hrpd_r11_present) {
    j.write_fieldname("parametersHRPD-r11");
    j.start_obj();
    j.write_fieldname("preRegistrationInfoHRPD-r11");
    params_hrpd_r11.pre_regist_info_hrpd_r11.to_json(j);
    if (params_hrpd_r11.cell_resel_params_hrpd_r11_present) {
      j.write_fieldname("cellReselectionParametersHRPD-r11");
      params_hrpd_r11.cell_resel_params_hrpd_r11.to_json(j);
    }
    j.end_obj();
  }
  if (params1_xrtt_r11_present) {
    j.write_fieldname("parameters1XRTT-r11");
    j.start_obj();
    if (params1_xrtt_r11.csfb_regist_param1_xrtt_r11_present) {
      j.write_fieldname("csfb-RegistrationParam1XRTT-r11");
      params1_xrtt_r11.csfb_regist_param1_xrtt_r11.to_json(j);
    }
    if (params1_xrtt_r11.csfb_regist_param1_xrtt_ext_r11_present) {
      j.write_fieldname("csfb-RegistrationParam1XRTT-Ext-r11");
      params1_xrtt_r11.csfb_regist_param1_xrtt_ext_r11.to_json(j);
    }
    if (params1_xrtt_r11.long_code_state1_xrtt_r11_present) {
      j.write_str("longCodeState1XRTT-r11", params1_xrtt_r11.long_code_state1_xrtt_r11.to_string());
    }
    if (params1_xrtt_r11.cell_resel_params1_xrtt_r11_present) {
      j.write_fieldname("cellReselectionParameters1XRTT-r11");
      params1_xrtt_r11.cell_resel_params1_xrtt_r11.to_json(j);
    }
    if (params1_xrtt_r11.ac_barr_cfg1_xrtt_r11_present) {
      j.write_fieldname("ac-BarringConfig1XRTT-r11");
      params1_xrtt_r11.ac_barr_cfg1_xrtt_r11.to_json(j);
    }
    if (params1_xrtt_r11.csfb_support_for_dual_rx_ues_r11_present) {
      j.write_bool("csfb-SupportForDualRxUEs-r11", params1_xrtt_r11.csfb_support_for_dual_rx_ues_r11);
    }
    if (params1_xrtt_r11.csfb_dual_rx_tx_support_r11_present) {
      j.write_str("csfb-DualRxTxSupport-r11", "true");
    }
    j.end_obj();
  }
  j.end_obj();
}

void params_cdma2000_r11_s::sys_time_info_r11_c_::set(types::options e)
{
  type_ = e;
}
sys_time_info_cdma2000_s& params_cdma2000_r11_s::sys_time_info_r11_c_::set_explicit_value()
{
  set(types::explicit_value);
  return c;
}
void params_cdma2000_r11_s::sys_time_info_r11_c_::set_default_value()
{
  set(types::default_value);
}
void params_cdma2000_r11_s::sys_time_info_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::explicit_value:
      j.write_fieldname("explicitValue");
      c.to_json(j);
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "params_cdma2000_r11_s::sys_time_info_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE params_cdma2000_r11_s::sys_time_info_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "params_cdma2000_r11_s::sys_time_info_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE params_cdma2000_r11_s::sys_time_info_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "params_cdma2000_r11_s::sys_time_info_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RedistributionInterFreqInfo-r13 ::= SEQUENCE
SRSASN_CODE redist_inter_freq_info_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(redist_factor_freq_r13_present, 1));
  HANDLE_CODE(bref.pack(redist_neigh_cell_list_r13_present, 1));

  if (redist_factor_freq_r13_present) {
    HANDLE_CODE(pack_integer(bref, redist_factor_freq_r13, (uint8_t)1u, (uint8_t)10u));
  }
  if (redist_neigh_cell_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, redist_neigh_cell_list_r13, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE redist_inter_freq_info_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(redist_factor_freq_r13_present, 1));
  HANDLE_CODE(bref.unpack(redist_neigh_cell_list_r13_present, 1));

  if (redist_factor_freq_r13_present) {
    HANDLE_CODE(unpack_integer(redist_factor_freq_r13, bref, (uint8_t)1u, (uint8_t)10u));
  }
  if (redist_neigh_cell_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(redist_neigh_cell_list_r13, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void redist_inter_freq_info_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (redist_factor_freq_r13_present) {
    j.write_int("redistributionFactorFreq-r13", redist_factor_freq_r13);
  }
  if (redist_neigh_cell_list_r13_present) {
    j.start_array("redistributionNeighCellList-r13");
    for (const auto& e1 : redist_neigh_cell_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-DiscConfigOtherInterFreq-r13 ::= SEQUENCE
SRSASN_CODE sl_disc_cfg_other_inter_freq_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tx_pwr_info_r13_present, 1));
  HANDLE_CODE(bref.pack(ref_carrier_common_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_sync_cfg_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_cell_sel_info_r13_present, 1));

  if (tx_pwr_info_r13_present) {
    HANDLE_CODE(pack_fixed_seq_of(bref, &(tx_pwr_info_r13)[0], tx_pwr_info_r13.size()));
  }
  if (disc_sync_cfg_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_sync_cfg_r13, 1, 16));
  }
  if (disc_cell_sel_info_r13_present) {
    HANDLE_CODE(disc_cell_sel_info_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_other_inter_freq_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tx_pwr_info_r13_present, 1));
  HANDLE_CODE(bref.unpack(ref_carrier_common_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_sync_cfg_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_cell_sel_info_r13_present, 1));

  if (tx_pwr_info_r13_present) {
    HANDLE_CODE(unpack_fixed_seq_of(&(tx_pwr_info_r13)[0], bref, tx_pwr_info_r13.size()));
  }
  if (disc_sync_cfg_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(disc_sync_cfg_r13, bref, 1, 16));
  }
  if (disc_cell_sel_info_r13_present) {
    HANDLE_CODE(disc_cell_sel_info_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_disc_cfg_other_inter_freq_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tx_pwr_info_r13_present) {
    j.start_array("txPowerInfo-r13");
    for (const auto& e1 : tx_pwr_info_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ref_carrier_common_r13_present) {
    j.write_str("refCarrierCommon-r13", "pCell");
  }
  if (disc_sync_cfg_r13_present) {
    j.start_array("discSyncConfig-r13");
    for (const auto& e1 : disc_sync_cfg_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (disc_cell_sel_info_r13_present) {
    j.write_fieldname("discCellSelectionInfo-r13");
    disc_cell_sel_info_r13.to_json(j);
  }
  j.end_obj();
}

// TLE-EphemerisParameters-r17 ::= SEQUENCE
SRSASN_CODE tle_ephemeris_params_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, inclination_r17, (uint32_t)0u, (uint32_t)2097151u));
  HANDLE_CODE(pack_integer(bref, argument_perigee_r17, (uint32_t)0u, (uint32_t)4194303u));
  HANDLE_CODE(pack_integer(bref, right_ascension_r17, (uint32_t)0u, (uint32_t)4194303u));
  HANDLE_CODE(pack_integer(bref, mean_anomaly_r17, (uint32_t)0u, (uint32_t)4194303u));
  HANDLE_CODE(pack_integer(bref, eccentricity_r17, (uint32_t)0u, (uint32_t)16777215u));
  HANDLE_CODE(pack_integer(bref, mean_motion_r17, (uint64_t)0u, (uint64_t)17179869183u));
  HANDLE_CODE(pack_integer(bref, bstar_decimal_r17, (int32_t)-99999, (int32_t)99999));
  HANDLE_CODE(pack_integer(bref, bstar_exponent_r17, (int8_t)-9, (int8_t)9));
  HANDLE_CODE(pack_integer(bref, epoch_star_r17, (int32_t)-1048575, (int32_t)1048575));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tle_ephemeris_params_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(inclination_r17, bref, (uint32_t)0u, (uint32_t)2097151u));
  HANDLE_CODE(unpack_integer(argument_perigee_r17, bref, (uint32_t)0u, (uint32_t)4194303u));
  HANDLE_CODE(unpack_integer(right_ascension_r17, bref, (uint32_t)0u, (uint32_t)4194303u));
  HANDLE_CODE(unpack_integer(mean_anomaly_r17, bref, (uint32_t)0u, (uint32_t)4194303u));
  HANDLE_CODE(unpack_integer(eccentricity_r17, bref, (uint32_t)0u, (uint32_t)16777215u));
  HANDLE_CODE(unpack_integer(mean_motion_r17, bref, (uint64_t)0u, (uint64_t)17179869183u));
  HANDLE_CODE(unpack_integer(bstar_decimal_r17, bref, (int32_t)-99999, (int32_t)99999));
  HANDLE_CODE(unpack_integer(bstar_exponent_r17, bref, (int8_t)-9, (int8_t)9));
  HANDLE_CODE(unpack_integer(epoch_star_r17, bref, (int32_t)-1048575, (int32_t)1048575));

  return SRSASN_SUCCESS;
}
void tle_ephemeris_params_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("inclination-r17", inclination_r17);
  j.write_int("argumentPerigee-r17", argument_perigee_r17);
  j.write_int("rightAscension-r17", right_ascension_r17);
  j.write_int("meanAnomaly-r17", mean_anomaly_r17);
  j.write_int("eccentricity-r17", eccentricity_r17);
  j.write_int("meanMotion-r17", mean_motion_r17);
  j.write_int("bStarDecimal-r17", bstar_decimal_r17);
  j.write_int("bStarExponent-r17", bstar_exponent_r17);
  j.write_int("epochStar-r17", epoch_star_r17);
  j.end_obj();
}

// CarrierFreqNBIOT-r16 ::= SEQUENCE
SRSASN_CODE carrier_freq_nbiot_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq_r16, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(carrier_freq_offset_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_nbiot_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq_r16, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(carrier_freq_offset_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void carrier_freq_nbiot_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r16", carrier_freq_r16);
  j.write_str("carrierFreqOffset-r16", carrier_freq_offset_r16.to_string());
  j.end_obj();
}

const char* carrier_freq_nbiot_r16_s::carrier_freq_offset_r16_opts::to_string() const
{
  static const char* options[] = {"v-10", "v-9", "v-8dot5", "v-8",     "v-7",    "v-6", "v-5", "v-4dot5", "v-4",
                                  "v-3",  "v-2", "v-1",     "v-0dot5", "v0",     "v1",  "v2",  "v3",      "v3dot5",
                                  "v4",   "v5",  "v6",      "v7",      "v7dot5", "v8",  "v9"};
  return convert_enum_idx(options, 25, value, "carrier_freq_nbiot_r16_s::carrier_freq_offset_r16_e_");
}
float carrier_freq_nbiot_r16_s::carrier_freq_offset_r16_opts::to_number() const
{
  static const float options[] = {-10.0, -9.0, -8.5, -8.0, -7.0, -6.0, -5.0, -4.5, -4.0, -3.0, -2.0, -1.0, -0.5,
                                  0.0,   1.0,  2.0,  3.0,  3.5,  4.0,  5.0,  6.0,  7.0,  7.5,  8.0,  9.0};
  return map_enum_number(options, 25, value, "carrier_freq_nbiot_r16_s::carrier_freq_offset_r16_e_");
}
const char* carrier_freq_nbiot_r16_s::carrier_freq_offset_r16_opts::to_number_string() const
{
  static const char* options[] = {"-10", "-9", "-8.5", "-8", "-7",  "-6", "-5", "-4.5", "-4", "-3",  "-2", "-1", "-0.5",
                                  "0",   "1",  "2",    "3",  "3.5", "4",  "5",  "6",    "7",  "7.5", "8",  "9"};
  return convert_enum_idx(options, 25, value, "carrier_freq_nbiot_r16_s::carrier_freq_offset_r16_e_");
}

// CarrierFreqNR-r15 ::= SEQUENCE
SRSASN_CODE carrier_freq_nr_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(multi_band_info_list_r15_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_sul_r15_present, 1));
  HANDLE_CODE(bref.pack(meas_timing_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(ss_rssi_meas_r15_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_prio_r15_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_sub_prio_r15_present, 1));
  HANDLE_CODE(bref.pack(thresh_x_q_r15_present, 1));
  HANDLE_CODE(bref.pack(q_rx_lev_min_sul_r15_present, 1));
  HANDLE_CODE(bref.pack(ns_pmax_list_nr_r15_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_r15_present, 1));
  HANDLE_CODE(bref.pack(max_rs_idx_cell_qual_r15_present, 1));
  HANDLE_CODE(bref.pack(thresh_rs_idx_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r15, (uint32_t)0u, (uint32_t)3279165u));
  if (multi_band_info_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_r15, 1, 32, integer_packer<uint16_t>(1, 1024)));
  }
  if (multi_band_info_list_sul_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_sul_r15, 1, 32, integer_packer<uint16_t>(1, 1024)));
  }
  if (meas_timing_cfg_r15_present) {
    HANDLE_CODE(meas_timing_cfg_r15.pack(bref));
  }
  HANDLE_CODE(subcarrier_spacing_ssb_r15.pack(bref));
  if (ss_rssi_meas_r15_present) {
    HANDLE_CODE(ss_rssi_meas_r15.pack(bref));
  }
  if (cell_resel_prio_r15_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_prio_r15, (uint8_t)0u, (uint8_t)7u));
  }
  if (cell_resel_sub_prio_r15_present) {
    HANDLE_CODE(cell_resel_sub_prio_r15.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, thresh_x_high_r15, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, thresh_x_low_r15, (uint8_t)0u, (uint8_t)31u));
  if (thresh_x_q_r15_present) {
    HANDLE_CODE(pack_integer(bref, thresh_x_q_r15.thresh_x_high_q_r15, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(pack_integer(bref, thresh_x_q_r15.thresh_x_low_q_r15, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min_r15, (int8_t)-70, (int8_t)-22));
  if (q_rx_lev_min_sul_r15_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_sul_r15, (int8_t)-70, (int8_t)-22));
  }
  HANDLE_CODE(pack_integer(bref, p_max_nr_r15, (int8_t)-30, (int8_t)33));
  if (ns_pmax_list_nr_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ns_pmax_list_nr_r15, 1, 8));
  }
  if (q_qual_min_r15_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_r15, (int8_t)-43, (int8_t)-12));
  }
  HANDLE_CODE(bref.pack(derive_ssb_idx_from_cell_r15, 1));
  if (max_rs_idx_cell_qual_r15_present) {
    HANDLE_CODE(pack_integer(bref, max_rs_idx_cell_qual_r15, (uint8_t)1u, (uint8_t)16u));
  }
  if (thresh_rs_idx_r15_present) {
    HANDLE_CODE(thresh_rs_idx_r15.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= multi_band_ns_pmax_list_nr_v1550.is_present();
    group_flags[0] |= multi_band_ns_pmax_list_nr_sul_v1550.is_present();
    group_flags[0] |= ssb_to_measure_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(multi_band_ns_pmax_list_nr_v1550.is_present(), 1));
      HANDLE_CODE(bref.pack(multi_band_ns_pmax_list_nr_sul_v1550.is_present(), 1));
      HANDLE_CODE(bref.pack(ssb_to_measure_r15.is_present(), 1));
      if (multi_band_ns_pmax_list_nr_v1550.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *multi_band_ns_pmax_list_nr_v1550, 1, 31, SeqOfPacker<Packer>(1, 8, Packer())));
      }
      if (multi_band_ns_pmax_list_nr_sul_v1550.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *multi_band_ns_pmax_list_nr_sul_v1550, 1, 32, SeqOfPacker<Packer>(1, 8, Packer())));
      }
      if (ssb_to_measure_r15.is_present()) {
        HANDLE_CODE(ssb_to_measure_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_nr_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(multi_band_info_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_sul_r15_present, 1));
  HANDLE_CODE(bref.unpack(meas_timing_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(ss_rssi_meas_r15_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_prio_r15_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_sub_prio_r15_present, 1));
  HANDLE_CODE(bref.unpack(thresh_x_q_r15_present, 1));
  HANDLE_CODE(bref.unpack(q_rx_lev_min_sul_r15_present, 1));
  HANDLE_CODE(bref.unpack(ns_pmax_list_nr_r15_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_r15_present, 1));
  HANDLE_CODE(bref.unpack(max_rs_idx_cell_qual_r15_present, 1));
  HANDLE_CODE(bref.unpack(thresh_rs_idx_r15_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r15, bref, (uint32_t)0u, (uint32_t)3279165u));
  if (multi_band_info_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_r15, bref, 1, 32, integer_packer<uint16_t>(1, 1024)));
  }
  if (multi_band_info_list_sul_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_sul_r15, bref, 1, 32, integer_packer<uint16_t>(1, 1024)));
  }
  if (meas_timing_cfg_r15_present) {
    HANDLE_CODE(meas_timing_cfg_r15.unpack(bref));
  }
  HANDLE_CODE(subcarrier_spacing_ssb_r15.unpack(bref));
  if (ss_rssi_meas_r15_present) {
    HANDLE_CODE(ss_rssi_meas_r15.unpack(bref));
  }
  if (cell_resel_prio_r15_present) {
    HANDLE_CODE(unpack_integer(cell_resel_prio_r15, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (cell_resel_sub_prio_r15_present) {
    HANDLE_CODE(cell_resel_sub_prio_r15.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(thresh_x_high_r15, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(thresh_x_low_r15, bref, (uint8_t)0u, (uint8_t)31u));
  if (thresh_x_q_r15_present) {
    HANDLE_CODE(unpack_integer(thresh_x_q_r15.thresh_x_high_q_r15, bref, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(unpack_integer(thresh_x_q_r15.thresh_x_low_q_r15, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(unpack_integer(q_rx_lev_min_r15, bref, (int8_t)-70, (int8_t)-22));
  if (q_rx_lev_min_sul_r15_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_sul_r15, bref, (int8_t)-70, (int8_t)-22));
  }
  HANDLE_CODE(unpack_integer(p_max_nr_r15, bref, (int8_t)-30, (int8_t)33));
  if (ns_pmax_list_nr_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ns_pmax_list_nr_r15, bref, 1, 8));
  }
  if (q_qual_min_r15_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_r15, bref, (int8_t)-43, (int8_t)-12));
  }
  HANDLE_CODE(bref.unpack(derive_ssb_idx_from_cell_r15, 1));
  if (max_rs_idx_cell_qual_r15_present) {
    HANDLE_CODE(unpack_integer(max_rs_idx_cell_qual_r15, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (thresh_rs_idx_r15_present) {
    HANDLE_CODE(thresh_rs_idx_r15.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool multi_band_ns_pmax_list_nr_v1550_present;
      HANDLE_CODE(bref.unpack(multi_band_ns_pmax_list_nr_v1550_present, 1));
      multi_band_ns_pmax_list_nr_v1550.set_present(multi_band_ns_pmax_list_nr_v1550_present);
      bool multi_band_ns_pmax_list_nr_sul_v1550_present;
      HANDLE_CODE(bref.unpack(multi_band_ns_pmax_list_nr_sul_v1550_present, 1));
      multi_band_ns_pmax_list_nr_sul_v1550.set_present(multi_band_ns_pmax_list_nr_sul_v1550_present);
      bool ssb_to_measure_r15_present;
      HANDLE_CODE(bref.unpack(ssb_to_measure_r15_present, 1));
      ssb_to_measure_r15.set_present(ssb_to_measure_r15_present);
      if (multi_band_ns_pmax_list_nr_v1550.is_present()) {
        HANDLE_CODE(
            unpack_dyn_seq_of(*multi_band_ns_pmax_list_nr_v1550, bref, 1, 31, SeqOfPacker<Packer>(1, 8, Packer())));
      }
      if (multi_band_ns_pmax_list_nr_sul_v1550.is_present()) {
        HANDLE_CODE(
            unpack_dyn_seq_of(*multi_band_ns_pmax_list_nr_sul_v1550, bref, 1, 32, SeqOfPacker<Packer>(1, 8, Packer())));
      }
      if (ssb_to_measure_r15.is_present()) {
        HANDLE_CODE(ssb_to_measure_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void carrier_freq_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r15", carrier_freq_r15);
  if (multi_band_info_list_r15_present) {
    j.start_array("multiBandInfoList-r15");
    for (const auto& e1 : multi_band_info_list_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (multi_band_info_list_sul_r15_present) {
    j.start_array("multiBandInfoListSUL-r15");
    for (const auto& e1 : multi_band_info_list_sul_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (meas_timing_cfg_r15_present) {
    j.write_fieldname("measTimingConfig-r15");
    meas_timing_cfg_r15.to_json(j);
  }
  j.write_str("subcarrierSpacingSSB-r15", subcarrier_spacing_ssb_r15.to_string());
  if (ss_rssi_meas_r15_present) {
    j.write_fieldname("ss-RSSI-Measurement-r15");
    ss_rssi_meas_r15.to_json(j);
  }
  if (cell_resel_prio_r15_present) {
    j.write_int("cellReselectionPriority-r15", cell_resel_prio_r15);
  }
  if (cell_resel_sub_prio_r15_present) {
    j.write_str("cellReselectionSubPriority-r15", cell_resel_sub_prio_r15.to_string());
  }
  j.write_int("threshX-High-r15", thresh_x_high_r15);
  j.write_int("threshX-Low-r15", thresh_x_low_r15);
  if (thresh_x_q_r15_present) {
    j.write_fieldname("threshX-Q-r15");
    j.start_obj();
    j.write_int("threshX-HighQ-r15", thresh_x_q_r15.thresh_x_high_q_r15);
    j.write_int("threshX-LowQ-r15", thresh_x_q_r15.thresh_x_low_q_r15);
    j.end_obj();
  }
  j.write_int("q-RxLevMin-r15", q_rx_lev_min_r15);
  if (q_rx_lev_min_sul_r15_present) {
    j.write_int("q-RxLevMinSUL-r15", q_rx_lev_min_sul_r15);
  }
  j.write_int("p-MaxNR-r15", p_max_nr_r15);
  if (ns_pmax_list_nr_r15_present) {
    j.start_array("ns-PmaxListNR-r15");
    for (const auto& e1 : ns_pmax_list_nr_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (q_qual_min_r15_present) {
    j.write_int("q-QualMin-r15", q_qual_min_r15);
  }
  j.write_bool("deriveSSB-IndexFromCell-r15", derive_ssb_idx_from_cell_r15);
  if (max_rs_idx_cell_qual_r15_present) {
    j.write_int("maxRS-IndexCellQual-r15", max_rs_idx_cell_qual_r15);
  }
  if (thresh_rs_idx_r15_present) {
    j.write_fieldname("threshRS-Index-r15");
    thresh_rs_idx_r15.to_json(j);
  }
  if (ext) {
    if (multi_band_ns_pmax_list_nr_v1550.is_present()) {
      j.start_array("multiBandNsPmaxListNR-v1550");
      for (const auto& e1 : *multi_band_ns_pmax_list_nr_v1550) {
        j.start_array();
        for (const auto& e2 : e1) {
          e2.to_json(j);
        }
        j.end_array();
      }
      j.end_array();
    }
    if (multi_band_ns_pmax_list_nr_sul_v1550.is_present()) {
      j.start_array("multiBandNsPmaxListNR-SUL-v1550");
      for (const auto& e1 : *multi_band_ns_pmax_list_nr_sul_v1550) {
        j.start_array();
        for (const auto& e2 : e1) {
          e2.to_json(j);
        }
        j.end_array();
      }
      j.end_array();
    }
    if (ssb_to_measure_r15.is_present()) {
      j.write_fieldname("ssb-ToMeasure-r15");
      ssb_to_measure_r15->to_json(j);
    }
  }
  j.end_obj();
}

const char* carrier_freq_nr_r15_s::subcarrier_spacing_ssb_r15_opts::to_string() const
{
  static const char* options[] = {"kHz15", "kHz30", "kHz120", "kHz240"};
  return convert_enum_idx(options, 4, value, "carrier_freq_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}
uint8_t carrier_freq_nr_r15_s::subcarrier_spacing_ssb_r15_opts::to_number() const
{
  static const uint8_t options[] = {15, 30, 120, 240};
  return map_enum_number(options, 4, value, "carrier_freq_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}

// CarrierFreqNR-v1610 ::= SEQUENCE
SRSASN_CODE carrier_freq_nr_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(smtc2_lp_r16_present, 1));
  HANDLE_CODE(bref.pack(ssb_position_qcl_common_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(allowed_cell_list_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(high_speed_carrier_nr_r16_present, 1));

  if (smtc2_lp_r16_present) {
    HANDLE_CODE(smtc2_lp_r16.pack(bref));
  }
  if (ssb_position_qcl_common_nr_r16_present) {
    HANDLE_CODE(ssb_position_qcl_common_nr_r16.pack(bref));
  }
  if (allowed_cell_list_nr_r16_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, allowed_cell_list_nr_r16, 1, 16, integer_packer<uint16_t>(0, 1007)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_nr_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(smtc2_lp_r16_present, 1));
  HANDLE_CODE(bref.unpack(ssb_position_qcl_common_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(allowed_cell_list_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(high_speed_carrier_nr_r16_present, 1));

  if (smtc2_lp_r16_present) {
    HANDLE_CODE(smtc2_lp_r16.unpack(bref));
  }
  if (ssb_position_qcl_common_nr_r16_present) {
    HANDLE_CODE(ssb_position_qcl_common_nr_r16.unpack(bref));
  }
  if (allowed_cell_list_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(allowed_cell_list_nr_r16, bref, 1, 16, integer_packer<uint16_t>(0, 1007)));
  }

  return SRSASN_SUCCESS;
}
void carrier_freq_nr_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (smtc2_lp_r16_present) {
    j.write_fieldname("smtc2-LP-r16");
    smtc2_lp_r16.to_json(j);
  }
  if (ssb_position_qcl_common_nr_r16_present) {
    j.write_str("ssb-PositionQCL-CommonNR-r16", ssb_position_qcl_common_nr_r16.to_string());
  }
  if (allowed_cell_list_nr_r16_present) {
    j.start_array("allowedCellListNR-r16");
    for (const auto& e1 : allowed_cell_list_nr_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (high_speed_carrier_nr_r16_present) {
    j.write_str("highSpeedCarrierNR-r16", "true");
  }
  j.end_obj();
}

// CarrierFreqNR-v1700 ::= SEQUENCE
SRSASN_CODE carrier_freq_nr_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nr_freq_neigh_hsdn_cell_list_r17_present, 1));

  if (nr_freq_neigh_hsdn_cell_list_r17_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nr_freq_neigh_hsdn_cell_list_r17, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_nr_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nr_freq_neigh_hsdn_cell_list_r17_present, 1));

  if (nr_freq_neigh_hsdn_cell_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nr_freq_neigh_hsdn_cell_list_r17, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void carrier_freq_nr_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nr_freq_neigh_hsdn_cell_list_r17_present) {
    j.start_array("nr-FreqNeighHSDN-CellList-r17");
    for (const auto& e1 : nr_freq_neigh_hsdn_cell_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// CarrierFreqNR-v1720 ::= SEQUENCE
SRSASN_CODE carrier_freq_nr_v1720_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(subcarrier_spacing_ssb_r17_present, 1));
  HANDLE_CODE(bref.pack(ssb_position_qcl_common_nr_r17_present, 1));

  if (subcarrier_spacing_ssb_r17_present) {
    HANDLE_CODE(subcarrier_spacing_ssb_r17.pack(bref));
  }
  if (ssb_position_qcl_common_nr_r17_present) {
    HANDLE_CODE(ssb_position_qcl_common_nr_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_nr_v1720_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(subcarrier_spacing_ssb_r17_present, 1));
  HANDLE_CODE(bref.unpack(ssb_position_qcl_common_nr_r17_present, 1));

  if (subcarrier_spacing_ssb_r17_present) {
    HANDLE_CODE(subcarrier_spacing_ssb_r17.unpack(bref));
  }
  if (ssb_position_qcl_common_nr_r17_present) {
    HANDLE_CODE(ssb_position_qcl_common_nr_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void carrier_freq_nr_v1720_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (subcarrier_spacing_ssb_r17_present) {
    j.write_str("subcarrierSpacingSSB-r17", subcarrier_spacing_ssb_r17.to_string());
  }
  if (ssb_position_qcl_common_nr_r17_present) {
    j.write_str("ssb-PositionQCL-CommonNR-r17", ssb_position_qcl_common_nr_r17.to_string());
  }
  j.end_obj();
}

const char* carrier_freq_nr_v1720_s::subcarrier_spacing_ssb_r17_opts::to_string() const
{
  static const char* options[] = {"kHz480", "spare1"};
  return convert_enum_idx(options, 2, value, "carrier_freq_nr_v1720_s::subcarrier_spacing_ssb_r17_e_");
}
uint16_t carrier_freq_nr_v1720_s::subcarrier_spacing_ssb_r17_opts::to_number() const
{
  static const uint16_t options[] = {480};
  return map_enum_number(options, 1, value, "carrier_freq_nr_v1720_s::subcarrier_spacing_ssb_r17_e_");
}

// CarrierFreqUTRA-FDD ::= SEQUENCE
SRSASN_CODE carrier_freq_utra_fdd_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_resel_prio_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint16_t)0u, (uint16_t)16383u));
  if (cell_resel_prio_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(pack_integer(bref, thresh_x_high, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, thresh_x_low, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min, (int8_t)-60, (int8_t)-13));
  HANDLE_CODE(pack_integer(bref, p_max_utra, (int8_t)-50, (int8_t)33));
  HANDLE_CODE(pack_integer(bref, q_qual_min, (int8_t)-24, (int8_t)0));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= thresh_x_q_r9.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(thresh_x_q_r9.is_present(), 1));
      if (thresh_x_q_r9.is_present()) {
        HANDLE_CODE(pack_integer(bref, thresh_x_q_r9->thresh_x_high_q_r9, (uint8_t)0u, (uint8_t)31u));
        HANDLE_CODE(pack_integer(bref, thresh_x_q_r9->thresh_x_low_q_r9, (uint8_t)0u, (uint8_t)31u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_utra_fdd_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_resel_prio_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint16_t)0u, (uint16_t)16383u));
  if (cell_resel_prio_present) {
    HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(unpack_integer(thresh_x_high, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(thresh_x_low, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(q_rx_lev_min, bref, (int8_t)-60, (int8_t)-13));
  HANDLE_CODE(unpack_integer(p_max_utra, bref, (int8_t)-50, (int8_t)33));
  HANDLE_CODE(unpack_integer(q_qual_min, bref, (int8_t)-24, (int8_t)0));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool thresh_x_q_r9_present;
      HANDLE_CODE(bref.unpack(thresh_x_q_r9_present, 1));
      thresh_x_q_r9.set_present(thresh_x_q_r9_present);
      if (thresh_x_q_r9.is_present()) {
        HANDLE_CODE(unpack_integer(thresh_x_q_r9->thresh_x_high_q_r9, bref, (uint8_t)0u, (uint8_t)31u));
        HANDLE_CODE(unpack_integer(thresh_x_q_r9->thresh_x_low_q_r9, bref, (uint8_t)0u, (uint8_t)31u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void carrier_freq_utra_fdd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  if (cell_resel_prio_present) {
    j.write_int("cellReselectionPriority", cell_resel_prio);
  }
  j.write_int("threshX-High", thresh_x_high);
  j.write_int("threshX-Low", thresh_x_low);
  j.write_int("q-RxLevMin", q_rx_lev_min);
  j.write_int("p-MaxUTRA", p_max_utra);
  j.write_int("q-QualMin", q_qual_min);
  if (ext) {
    if (thresh_x_q_r9.is_present()) {
      j.write_fieldname("threshX-Q-r9");
      j.start_obj();
      j.write_int("threshX-HighQ-r9", thresh_x_q_r9->thresh_x_high_q_r9);
      j.write_int("threshX-LowQ-r9", thresh_x_q_r9->thresh_x_low_q_r9);
      j.end_obj();
    }
  }
  j.end_obj();
}

// CarrierFreqUTRA-FDD-Ext-r12 ::= SEQUENCE
SRSASN_CODE carrier_freq_utra_fdd_ext_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_resel_prio_r12_present, 1));
  HANDLE_CODE(bref.pack(thresh_x_q_r12_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_r12_present, 1));
  HANDLE_CODE(bref.pack(reduced_meas_performance_r12_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r12, (uint16_t)0u, (uint16_t)16383u));
  if (cell_resel_prio_r12_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_prio_r12, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(pack_integer(bref, thresh_x_high_r12, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, thresh_x_low_r12, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min_r12, (int8_t)-60, (int8_t)-13));
  HANDLE_CODE(pack_integer(bref, p_max_utra_r12, (int8_t)-50, (int8_t)33));
  HANDLE_CODE(pack_integer(bref, q_qual_min_r12, (int8_t)-24, (int8_t)0));
  if (thresh_x_q_r12_present) {
    HANDLE_CODE(pack_integer(bref, thresh_x_q_r12.thresh_x_high_q_r12, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(pack_integer(bref, thresh_x_q_r12.thresh_x_low_q_r12, (uint8_t)0u, (uint8_t)31u));
  }
  if (multi_band_info_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_r12, 1, 8, integer_packer<uint8_t>(1, 86)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_utra_fdd_ext_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_resel_prio_r12_present, 1));
  HANDLE_CODE(bref.unpack(thresh_x_q_r12_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_r12_present, 1));
  HANDLE_CODE(bref.unpack(reduced_meas_performance_r12_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r12, bref, (uint16_t)0u, (uint16_t)16383u));
  if (cell_resel_prio_r12_present) {
    HANDLE_CODE(unpack_integer(cell_resel_prio_r12, bref, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(unpack_integer(thresh_x_high_r12, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(thresh_x_low_r12, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(q_rx_lev_min_r12, bref, (int8_t)-60, (int8_t)-13));
  HANDLE_CODE(unpack_integer(p_max_utra_r12, bref, (int8_t)-50, (int8_t)33));
  HANDLE_CODE(unpack_integer(q_qual_min_r12, bref, (int8_t)-24, (int8_t)0));
  if (thresh_x_q_r12_present) {
    HANDLE_CODE(unpack_integer(thresh_x_q_r12.thresh_x_high_q_r12, bref, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(unpack_integer(thresh_x_q_r12.thresh_x_low_q_r12, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (multi_band_info_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_r12, bref, 1, 8, integer_packer<uint8_t>(1, 86)));
  }

  return SRSASN_SUCCESS;
}
void carrier_freq_utra_fdd_ext_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r12", carrier_freq_r12);
  if (cell_resel_prio_r12_present) {
    j.write_int("cellReselectionPriority-r12", cell_resel_prio_r12);
  }
  j.write_int("threshX-High-r12", thresh_x_high_r12);
  j.write_int("threshX-Low-r12", thresh_x_low_r12);
  j.write_int("q-RxLevMin-r12", q_rx_lev_min_r12);
  j.write_int("p-MaxUTRA-r12", p_max_utra_r12);
  j.write_int("q-QualMin-r12", q_qual_min_r12);
  if (thresh_x_q_r12_present) {
    j.write_fieldname("threshX-Q-r12");
    j.start_obj();
    j.write_int("threshX-HighQ-r12", thresh_x_q_r12.thresh_x_high_q_r12);
    j.write_int("threshX-LowQ-r12", thresh_x_q_r12.thresh_x_low_q_r12);
    j.end_obj();
  }
  if (multi_band_info_list_r12_present) {
    j.start_array("multiBandInfoList-r12");
    for (const auto& e1 : multi_band_info_list_r12) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (reduced_meas_performance_r12_present) {
    j.write_str("reducedMeasPerformance-r12", "true");
  }
  j.end_obj();
}

// CarrierFreqUTRA-TDD ::= SEQUENCE
SRSASN_CODE carrier_freq_utra_tdd_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_resel_prio_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint16_t)0u, (uint16_t)16383u));
  if (cell_resel_prio_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(pack_integer(bref, thresh_x_high, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, thresh_x_low, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min, (int8_t)-60, (int8_t)-13));
  HANDLE_CODE(pack_integer(bref, p_max_utra, (int8_t)-50, (int8_t)33));

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_utra_tdd_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_resel_prio_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint16_t)0u, (uint16_t)16383u));
  if (cell_resel_prio_present) {
    HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(unpack_integer(thresh_x_high, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(thresh_x_low, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(q_rx_lev_min, bref, (int8_t)-60, (int8_t)-13));
  HANDLE_CODE(unpack_integer(p_max_utra, bref, (int8_t)-50, (int8_t)33));

  return SRSASN_SUCCESS;
}
void carrier_freq_utra_tdd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  if (cell_resel_prio_present) {
    j.write_int("cellReselectionPriority", cell_resel_prio);
  }
  j.write_int("threshX-High", thresh_x_high);
  j.write_int("threshX-Low", thresh_x_low);
  j.write_int("q-RxLevMin", q_rx_lev_min);
  j.write_int("p-MaxUTRA", p_max_utra);
  j.end_obj();
}

// CarrierFreqUTRA-TDD-r12 ::= SEQUENCE
SRSASN_CODE carrier_freq_utra_tdd_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_resel_prio_r12_present, 1));
  HANDLE_CODE(bref.pack(reduced_meas_performance_r12_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r12, (uint16_t)0u, (uint16_t)16383u));
  if (cell_resel_prio_r12_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_prio_r12, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(pack_integer(bref, thresh_x_high_r12, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, thresh_x_low_r12, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min_r12, (int8_t)-60, (int8_t)-13));
  HANDLE_CODE(pack_integer(bref, p_max_utra_r12, (int8_t)-50, (int8_t)33));

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_utra_tdd_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_resel_prio_r12_present, 1));
  HANDLE_CODE(bref.unpack(reduced_meas_performance_r12_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r12, bref, (uint16_t)0u, (uint16_t)16383u));
  if (cell_resel_prio_r12_present) {
    HANDLE_CODE(unpack_integer(cell_resel_prio_r12, bref, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(unpack_integer(thresh_x_high_r12, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(thresh_x_low_r12, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(q_rx_lev_min_r12, bref, (int8_t)-60, (int8_t)-13));
  HANDLE_CODE(unpack_integer(p_max_utra_r12, bref, (int8_t)-50, (int8_t)33));

  return SRSASN_SUCCESS;
}
void carrier_freq_utra_tdd_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r12", carrier_freq_r12);
  if (cell_resel_prio_r12_present) {
    j.write_int("cellReselectionPriority-r12", cell_resel_prio_r12);
  }
  j.write_int("threshX-High-r12", thresh_x_high_r12);
  j.write_int("threshX-Low-r12", thresh_x_low_r12);
  j.write_int("q-RxLevMin-r12", q_rx_lev_min_r12);
  j.write_int("p-MaxUTRA-r12", p_max_utra_r12);
  if (reduced_meas_performance_r12_present) {
    j.write_str("reducedMeasPerformance-r12", "true");
  }
  j.end_obj();
}

// CarrierFreqsInfoGERAN ::= SEQUENCE
SRSASN_CODE carrier_freqs_info_geran_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(carrier_freqs.pack(bref));
  HANDLE_CODE(bref.pack(common_info.cell_resel_prio_present, 1));
  HANDLE_CODE(bref.pack(common_info.p_max_geran_present, 1));
  if (common_info.cell_resel_prio_present) {
    HANDLE_CODE(pack_integer(bref, common_info.cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(common_info.ncc_permitted.pack(bref));
  HANDLE_CODE(pack_integer(bref, common_info.q_rx_lev_min, (uint8_t)0u, (uint8_t)45u));
  if (common_info.p_max_geran_present) {
    HANDLE_CODE(pack_integer(bref, common_info.p_max_geran, (uint8_t)0u, (uint8_t)39u));
  }
  HANDLE_CODE(pack_integer(bref, common_info.thresh_x_high, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, common_info.thresh_x_low, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freqs_info_geran_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(carrier_freqs.unpack(bref));
  HANDLE_CODE(bref.unpack(common_info.cell_resel_prio_present, 1));
  HANDLE_CODE(bref.unpack(common_info.p_max_geran_present, 1));
  if (common_info.cell_resel_prio_present) {
    HANDLE_CODE(unpack_integer(common_info.cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(common_info.ncc_permitted.unpack(bref));
  HANDLE_CODE(unpack_integer(common_info.q_rx_lev_min, bref, (uint8_t)0u, (uint8_t)45u));
  if (common_info.p_max_geran_present) {
    HANDLE_CODE(unpack_integer(common_info.p_max_geran, bref, (uint8_t)0u, (uint8_t)39u));
  }
  HANDLE_CODE(unpack_integer(common_info.thresh_x_high, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(common_info.thresh_x_low, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void carrier_freqs_info_geran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("carrierFreqs");
  carrier_freqs.to_json(j);
  j.write_fieldname("commonInfo");
  j.start_obj();
  if (common_info.cell_resel_prio_present) {
    j.write_int("cellReselectionPriority", common_info.cell_resel_prio);
  }
  j.write_str("ncc-Permitted", common_info.ncc_permitted.to_string());
  j.write_int("q-RxLevMin", common_info.q_rx_lev_min);
  if (common_info.p_max_geran_present) {
    j.write_int("p-MaxGERAN", common_info.p_max_geran);
  }
  j.write_int("threshX-High", common_info.thresh_x_high);
  j.write_int("threshX-Low", common_info.thresh_x_low);
  j.end_obj();
  j.end_obj();
}

// EAB-Config-r11 ::= SEQUENCE
SRSASN_CODE eab_cfg_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(eab_category_r11.pack(bref));
  HANDLE_CODE(eab_barr_bitmap_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE eab_cfg_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(eab_category_r11.unpack(bref));
  HANDLE_CODE(eab_barr_bitmap_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void eab_cfg_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("eab-Category-r11", eab_category_r11.to_string());
  j.write_str("eab-BarringBitmap-r11", eab_barr_bitmap_r11.to_string());
  j.end_obj();
}

const char* eab_cfg_r11_s::eab_category_r11_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c"};
  return convert_enum_idx(options, 3, value, "eab_cfg_r11_s::eab_category_r11_e_");
}

// EphemerisOrbitalParameters-r17 ::= SEQUENCE
SRSASN_CODE ephemeris_orbital_params_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, semi_major_axis_r17, (uint64_t)0u, (uint64_t)8589934591u));
  HANDLE_CODE(pack_integer(bref, eccentricity_r17, (uint32_t)0u, (uint32_t)1048575u));
  HANDLE_CODE(pack_integer(bref, periapsis_r17, (uint32_t)0u, (uint32_t)268435455u));
  HANDLE_CODE(pack_integer(bref, longitude_r17, (uint32_t)0u, (uint32_t)268435455u));
  HANDLE_CODE(pack_integer(bref, inclination_r17, (int32_t)-67108864, (int32_t)67108863));
  HANDLE_CODE(pack_integer(bref, anomaly_r17, (uint32_t)0u, (uint32_t)268435455u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ephemeris_orbital_params_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(semi_major_axis_r17, bref, (uint64_t)0u, (uint64_t)8589934591u));
  HANDLE_CODE(unpack_integer(eccentricity_r17, bref, (uint32_t)0u, (uint32_t)1048575u));
  HANDLE_CODE(unpack_integer(periapsis_r17, bref, (uint32_t)0u, (uint32_t)268435455u));
  HANDLE_CODE(unpack_integer(longitude_r17, bref, (uint32_t)0u, (uint32_t)268435455u));
  HANDLE_CODE(unpack_integer(inclination_r17, bref, (int32_t)-67108864, (int32_t)67108863));
  HANDLE_CODE(unpack_integer(anomaly_r17, bref, (uint32_t)0u, (uint32_t)268435455u));

  return SRSASN_SUCCESS;
}
void ephemeris_orbital_params_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("semiMajorAxis-r17", semi_major_axis_r17);
  j.write_int("eccentricity-r17", eccentricity_r17);
  j.write_int("periapsis-r17", periapsis_r17);
  j.write_int("longitude-r17", longitude_r17);
  j.write_int("inclination-r17", inclination_r17);
  j.write_int("anomaly-r17", anomaly_r17);
  j.end_obj();
}

// EphemerisStateVectors-r17 ::= SEQUENCE
SRSASN_CODE ephemeris_state_vectors_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, position_x_r17, (int32_t)-33554432, (int32_t)33554431));
  HANDLE_CODE(pack_integer(bref, position_y_r17, (int32_t)-33554432, (int32_t)33554431));
  HANDLE_CODE(pack_integer(bref, position_z_r17, (int32_t)-33554432, (int32_t)33554431));
  HANDLE_CODE(pack_integer(bref, velocity_vx_r17, (int32_t)-131072, (int32_t)131071));
  HANDLE_CODE(pack_integer(bref, velocity_vy_r17, (int32_t)-131072, (int32_t)131071));
  HANDLE_CODE(pack_integer(bref, velocity_vz_r17, (int32_t)-131072, (int32_t)131071));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ephemeris_state_vectors_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(position_x_r17, bref, (int32_t)-33554432, (int32_t)33554431));
  HANDLE_CODE(unpack_integer(position_y_r17, bref, (int32_t)-33554432, (int32_t)33554431));
  HANDLE_CODE(unpack_integer(position_z_r17, bref, (int32_t)-33554432, (int32_t)33554431));
  HANDLE_CODE(unpack_integer(velocity_vx_r17, bref, (int32_t)-131072, (int32_t)131071));
  HANDLE_CODE(unpack_integer(velocity_vy_r17, bref, (int32_t)-131072, (int32_t)131071));
  HANDLE_CODE(unpack_integer(velocity_vz_r17, bref, (int32_t)-131072, (int32_t)131071));

  return SRSASN_SUCCESS;
}
void ephemeris_state_vectors_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("positionX-r17", position_x_r17);
  j.write_int("positionY-r17", position_y_r17);
  j.write_int("positionZ-r17", position_z_r17);
  j.write_int("velocityVX-r17", velocity_vx_r17);
  j.write_int("velocityVY-r17", velocity_vy_r17);
  j.write_int("velocityVZ-r17", velocity_vz_r17);
  j.end_obj();
}

// InterFreqCarrierFreqInfo ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(p_max_present, 1));
  HANDLE_CODE(bref.pack(t_resel_eutra_sf_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_prio_present, 1));
  HANDLE_CODE(bref.pack(q_offset_freq_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_neigh_cell_list_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_excluded_cell_list_present, 1));

  HANDLE_CODE(pack_integer(bref, dl_carrier_freq, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min, (int8_t)-70, (int8_t)-22));
  if (p_max_present) {
    HANDLE_CODE(pack_integer(bref, p_max, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pack_integer(bref, t_resel_eutra, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_eutra_sf_present) {
    HANDLE_CODE(t_resel_eutra_sf.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, thresh_x_high, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, thresh_x_low, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(allowed_meas_bw.pack(bref));
  HANDLE_CODE(bref.pack(presence_ant_port1, 1));
  if (cell_resel_prio_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(neigh_cell_cfg.pack(bref));
  if (q_offset_freq_present) {
    HANDLE_CODE(q_offset_freq.pack(bref));
  }
  if (inter_freq_neigh_cell_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_cell_list, 1, 16));
  }
  if (inter_freq_excluded_cell_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_excluded_cell_list, 1, 16));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= q_qual_min_r9_present;
    group_flags[0] |= thresh_x_q_r9.is_present();
    group_flags[1] |= q_qual_min_wb_r11_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(q_qual_min_r9_present, 1));
      HANDLE_CODE(bref.pack(thresh_x_q_r9.is_present(), 1));
      if (q_qual_min_r9_present) {
        HANDLE_CODE(pack_integer(bref, q_qual_min_r9, (int8_t)-34, (int8_t)-3));
      }
      if (thresh_x_q_r9.is_present()) {
        HANDLE_CODE(pack_integer(bref, thresh_x_q_r9->thresh_x_high_q_r9, (uint8_t)0u, (uint8_t)31u));
        HANDLE_CODE(pack_integer(bref, thresh_x_q_r9->thresh_x_low_q_r9, (uint8_t)0u, (uint8_t)31u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(q_qual_min_wb_r11_present, 1));
      if (q_qual_min_wb_r11_present) {
        HANDLE_CODE(pack_integer(bref, q_qual_min_wb_r11, (int8_t)-34, (int8_t)-3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(p_max_present, 1));
  HANDLE_CODE(bref.unpack(t_resel_eutra_sf_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_prio_present, 1));
  HANDLE_CODE(bref.unpack(q_offset_freq_present, 1));
  HANDLE_CODE(bref.unpack(inter_freq_neigh_cell_list_present, 1));
  HANDLE_CODE(bref.unpack(inter_freq_excluded_cell_list_present, 1));

  HANDLE_CODE(unpack_integer(dl_carrier_freq, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(unpack_integer(q_rx_lev_min, bref, (int8_t)-70, (int8_t)-22));
  if (p_max_present) {
    HANDLE_CODE(unpack_integer(p_max, bref, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(unpack_integer(t_resel_eutra, bref, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_eutra_sf_present) {
    HANDLE_CODE(t_resel_eutra_sf.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(thresh_x_high, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(thresh_x_low, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(allowed_meas_bw.unpack(bref));
  HANDLE_CODE(bref.unpack(presence_ant_port1, 1));
  if (cell_resel_prio_present) {
    HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(neigh_cell_cfg.unpack(bref));
  if (q_offset_freq_present) {
    HANDLE_CODE(q_offset_freq.unpack(bref));
  }
  if (inter_freq_neigh_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_cell_list, bref, 1, 16));
  }
  if (inter_freq_excluded_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_excluded_cell_list, bref, 1, 16));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(q_qual_min_r9_present, 1));
      bool thresh_x_q_r9_present;
      HANDLE_CODE(bref.unpack(thresh_x_q_r9_present, 1));
      thresh_x_q_r9.set_present(thresh_x_q_r9_present);
      if (q_qual_min_r9_present) {
        HANDLE_CODE(unpack_integer(q_qual_min_r9, bref, (int8_t)-34, (int8_t)-3));
      }
      if (thresh_x_q_r9.is_present()) {
        HANDLE_CODE(unpack_integer(thresh_x_q_r9->thresh_x_high_q_r9, bref, (uint8_t)0u, (uint8_t)31u));
        HANDLE_CODE(unpack_integer(thresh_x_q_r9->thresh_x_low_q_r9, bref, (uint8_t)0u, (uint8_t)31u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(q_qual_min_wb_r11_present, 1));
      if (q_qual_min_wb_r11_present) {
        HANDLE_CODE(unpack_integer(q_qual_min_wb_r11, bref, (int8_t)-34, (int8_t)-3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-CarrierFreq", dl_carrier_freq);
  j.write_int("q-RxLevMin", q_rx_lev_min);
  if (p_max_present) {
    j.write_int("p-Max", p_max);
  }
  j.write_int("t-ReselectionEUTRA", t_resel_eutra);
  if (t_resel_eutra_sf_present) {
    j.write_fieldname("t-ReselectionEUTRA-SF");
    t_resel_eutra_sf.to_json(j);
  }
  j.write_int("threshX-High", thresh_x_high);
  j.write_int("threshX-Low", thresh_x_low);
  j.write_str("allowedMeasBandwidth", allowed_meas_bw.to_string());
  j.write_bool("presenceAntennaPort1", presence_ant_port1);
  if (cell_resel_prio_present) {
    j.write_int("cellReselectionPriority", cell_resel_prio);
  }
  j.write_str("neighCellConfig", neigh_cell_cfg.to_string());
  if (q_offset_freq_present) {
    j.write_str("q-OffsetFreq", q_offset_freq.to_string());
  }
  if (inter_freq_neigh_cell_list_present) {
    j.start_array("interFreqNeighCellList");
    for (const auto& e1 : inter_freq_neigh_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (inter_freq_excluded_cell_list_present) {
    j.start_array("interFreqExcludedCellList");
    for (const auto& e1 : inter_freq_excluded_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (q_qual_min_r9_present) {
      j.write_int("q-QualMin-r9", q_qual_min_r9);
    }
    if (thresh_x_q_r9.is_present()) {
      j.write_fieldname("threshX-Q-r9");
      j.start_obj();
      j.write_int("threshX-HighQ-r9", thresh_x_q_r9->thresh_x_high_q_r9);
      j.write_int("threshX-LowQ-r9", thresh_x_q_r9->thresh_x_low_q_r9);
      j.end_obj();
    }
    if (q_qual_min_wb_r11_present) {
      j.write_int("q-QualMinWB-r11", q_qual_min_wb_r11);
    }
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-r12 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(p_max_r12_present, 1));
  HANDLE_CODE(bref.pack(t_resel_eutra_sf_r12_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_prio_r12_present, 1));
  HANDLE_CODE(bref.pack(q_offset_freq_r12_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_neigh_cell_list_r12_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_excluded_cell_list_r12_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_r12_present, 1));
  HANDLE_CODE(bref.pack(thresh_x_q_r12_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_wb_r12_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_r12_present, 1));
  HANDLE_CODE(bref.pack(reduced_meas_performance_r12_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_rsrq_on_all_symbols_r12_present, 1));

  HANDLE_CODE(pack_integer(bref, dl_carrier_freq_r12, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min_r12, (int8_t)-70, (int8_t)-22));
  if (p_max_r12_present) {
    HANDLE_CODE(pack_integer(bref, p_max_r12, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pack_integer(bref, t_resel_eutra_r12, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_eutra_sf_r12_present) {
    HANDLE_CODE(t_resel_eutra_sf_r12.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, thresh_x_high_r12, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, thresh_x_low_r12, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(allowed_meas_bw_r12.pack(bref));
  HANDLE_CODE(bref.pack(presence_ant_port1_r12, 1));
  if (cell_resel_prio_r12_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_prio_r12, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(neigh_cell_cfg_r12.pack(bref));
  if (q_offset_freq_r12_present) {
    HANDLE_CODE(q_offset_freq_r12.pack(bref));
  }
  if (inter_freq_neigh_cell_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_cell_list_r12, 1, 16));
  }
  if (inter_freq_excluded_cell_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_excluded_cell_list_r12, 1, 16));
  }
  if (q_qual_min_r12_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_r12, (int8_t)-34, (int8_t)-3));
  }
  if (thresh_x_q_r12_present) {
    HANDLE_CODE(pack_integer(bref, thresh_x_q_r12.thresh_x_high_q_r12, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(pack_integer(bref, thresh_x_q_r12.thresh_x_low_q_r12, (uint8_t)0u, (uint8_t)31u));
  }
  if (q_qual_min_wb_r12_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_wb_r12, (int8_t)-34, (int8_t)-3));
  }
  if (multi_band_info_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_r12, 1, 8, integer_packer<uint16_t>(1, 256)));
  }
  if (q_qual_min_rsrq_on_all_symbols_r12_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_rsrq_on_all_symbols_r12, (int8_t)-34, (int8_t)-3));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(p_max_r12_present, 1));
  HANDLE_CODE(bref.unpack(t_resel_eutra_sf_r12_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_prio_r12_present, 1));
  HANDLE_CODE(bref.unpack(q_offset_freq_r12_present, 1));
  HANDLE_CODE(bref.unpack(inter_freq_neigh_cell_list_r12_present, 1));
  HANDLE_CODE(bref.unpack(inter_freq_excluded_cell_list_r12_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_r12_present, 1));
  HANDLE_CODE(bref.unpack(thresh_x_q_r12_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_wb_r12_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_r12_present, 1));
  HANDLE_CODE(bref.unpack(reduced_meas_performance_r12_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_rsrq_on_all_symbols_r12_present, 1));

  HANDLE_CODE(unpack_integer(dl_carrier_freq_r12, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(unpack_integer(q_rx_lev_min_r12, bref, (int8_t)-70, (int8_t)-22));
  if (p_max_r12_present) {
    HANDLE_CODE(unpack_integer(p_max_r12, bref, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(unpack_integer(t_resel_eutra_r12, bref, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_eutra_sf_r12_present) {
    HANDLE_CODE(t_resel_eutra_sf_r12.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(thresh_x_high_r12, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(thresh_x_low_r12, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(allowed_meas_bw_r12.unpack(bref));
  HANDLE_CODE(bref.unpack(presence_ant_port1_r12, 1));
  if (cell_resel_prio_r12_present) {
    HANDLE_CODE(unpack_integer(cell_resel_prio_r12, bref, (uint8_t)0u, (uint8_t)7u));
  }
  HANDLE_CODE(neigh_cell_cfg_r12.unpack(bref));
  if (q_offset_freq_r12_present) {
    HANDLE_CODE(q_offset_freq_r12.unpack(bref));
  }
  if (inter_freq_neigh_cell_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_cell_list_r12, bref, 1, 16));
  }
  if (inter_freq_excluded_cell_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_excluded_cell_list_r12, bref, 1, 16));
  }
  if (q_qual_min_r12_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_r12, bref, (int8_t)-34, (int8_t)-3));
  }
  if (thresh_x_q_r12_present) {
    HANDLE_CODE(unpack_integer(thresh_x_q_r12.thresh_x_high_q_r12, bref, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(unpack_integer(thresh_x_q_r12.thresh_x_low_q_r12, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (q_qual_min_wb_r12_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_wb_r12, bref, (int8_t)-34, (int8_t)-3));
  }
  if (multi_band_info_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_r12, bref, 1, 8, integer_packer<uint16_t>(1, 256)));
  }
  if (q_qual_min_rsrq_on_all_symbols_r12_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_rsrq_on_all_symbols_r12, bref, (int8_t)-34, (int8_t)-3));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-CarrierFreq-r12", dl_carrier_freq_r12);
  j.write_int("q-RxLevMin-r12", q_rx_lev_min_r12);
  if (p_max_r12_present) {
    j.write_int("p-Max-r12", p_max_r12);
  }
  j.write_int("t-ReselectionEUTRA-r12", t_resel_eutra_r12);
  if (t_resel_eutra_sf_r12_present) {
    j.write_fieldname("t-ReselectionEUTRA-SF-r12");
    t_resel_eutra_sf_r12.to_json(j);
  }
  j.write_int("threshX-High-r12", thresh_x_high_r12);
  j.write_int("threshX-Low-r12", thresh_x_low_r12);
  j.write_str("allowedMeasBandwidth-r12", allowed_meas_bw_r12.to_string());
  j.write_bool("presenceAntennaPort1-r12", presence_ant_port1_r12);
  if (cell_resel_prio_r12_present) {
    j.write_int("cellReselectionPriority-r12", cell_resel_prio_r12);
  }
  j.write_str("neighCellConfig-r12", neigh_cell_cfg_r12.to_string());
  if (q_offset_freq_r12_present) {
    j.write_str("q-OffsetFreq-r12", q_offset_freq_r12.to_string());
  }
  if (inter_freq_neigh_cell_list_r12_present) {
    j.start_array("interFreqNeighCellList-r12");
    for (const auto& e1 : inter_freq_neigh_cell_list_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (inter_freq_excluded_cell_list_r12_present) {
    j.start_array("interFreqExcludedCellList-r12");
    for (const auto& e1 : inter_freq_excluded_cell_list_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (q_qual_min_r12_present) {
    j.write_int("q-QualMin-r12", q_qual_min_r12);
  }
  if (thresh_x_q_r12_present) {
    j.write_fieldname("threshX-Q-r12");
    j.start_obj();
    j.write_int("threshX-HighQ-r12", thresh_x_q_r12.thresh_x_high_q_r12);
    j.write_int("threshX-LowQ-r12", thresh_x_q_r12.thresh_x_low_q_r12);
    j.end_obj();
  }
  if (q_qual_min_wb_r12_present) {
    j.write_int("q-QualMinWB-r12", q_qual_min_wb_r12);
  }
  if (multi_band_info_list_r12_present) {
    j.start_array("multiBandInfoList-r12");
    for (const auto& e1 : multi_band_info_list_r12) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (reduced_meas_performance_r12_present) {
    j.write_str("reducedMeasPerformance-r12", "true");
  }
  if (q_qual_min_rsrq_on_all_symbols_r12_present) {
    j.write_int("q-QualMinRSRQ-OnAllSymbols-r12", q_qual_min_rsrq_on_all_symbols_r12);
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v10j0 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v10j0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_info_r10_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_v10j0_present, 1));

  if (freq_band_info_r10_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_info_r10, 1, 8));
  }
  if (multi_band_info_list_v10j0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_v10j0, 1, 8, SeqOfPacker<Packer>(1, 8, Packer())));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v10j0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_band_info_r10_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_v10j0_present, 1));

  if (freq_band_info_r10_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_info_r10, bref, 1, 8));
  }
  if (multi_band_info_list_v10j0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_v10j0, bref, 1, 8, SeqOfPacker<Packer>(1, 8, Packer())));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v10j0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_info_r10_present) {
    j.start_array("freqBandInfo-r10");
    for (const auto& e1 : freq_band_info_r10) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (multi_band_info_list_v10j0_present) {
    j.start_array("multiBandInfoList-v10j0");
    for (const auto& e1 : multi_band_info_list_v10j0) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1250 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reduced_meas_performance_r12_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_rsrq_on_all_symbols_r12_present, 1));

  if (q_qual_min_rsrq_on_all_symbols_r12_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_rsrq_on_all_symbols_r12, (int8_t)-34, (int8_t)-3));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reduced_meas_performance_r12_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_rsrq_on_all_symbols_r12_present, 1));

  if (q_qual_min_rsrq_on_all_symbols_r12_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_rsrq_on_all_symbols_r12, bref, (int8_t)-34, (int8_t)-3));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reduced_meas_performance_r12_present) {
    j.write_str("reducedMeasPerformance-r12", "true");
  }
  if (q_qual_min_rsrq_on_all_symbols_r12_present) {
    j.write_int("q-QualMinRSRQ-OnAllSymbols-r12", q_qual_min_rsrq_on_all_symbols_r12);
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1310 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_resel_sub_prio_r13_present, 1));
  HANDLE_CODE(bref.pack(redist_inter_freq_info_r13_present, 1));
  HANDLE_CODE(bref.pack(cell_sel_info_ce_r13_present, 1));
  HANDLE_CODE(bref.pack(t_resel_eutra_ce_r13_present, 1));

  if (cell_resel_sub_prio_r13_present) {
    HANDLE_CODE(cell_resel_sub_prio_r13.pack(bref));
  }
  if (redist_inter_freq_info_r13_present) {
    HANDLE_CODE(redist_inter_freq_info_r13.pack(bref));
  }
  if (cell_sel_info_ce_r13_present) {
    HANDLE_CODE(cell_sel_info_ce_r13.pack(bref));
  }
  if (t_resel_eutra_ce_r13_present) {
    HANDLE_CODE(pack_integer(bref, t_resel_eutra_ce_r13, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_resel_sub_prio_r13_present, 1));
  HANDLE_CODE(bref.unpack(redist_inter_freq_info_r13_present, 1));
  HANDLE_CODE(bref.unpack(cell_sel_info_ce_r13_present, 1));
  HANDLE_CODE(bref.unpack(t_resel_eutra_ce_r13_present, 1));

  if (cell_resel_sub_prio_r13_present) {
    HANDLE_CODE(cell_resel_sub_prio_r13.unpack(bref));
  }
  if (redist_inter_freq_info_r13_present) {
    HANDLE_CODE(redist_inter_freq_info_r13.unpack(bref));
  }
  if (cell_sel_info_ce_r13_present) {
    HANDLE_CODE(cell_sel_info_ce_r13.unpack(bref));
  }
  if (t_resel_eutra_ce_r13_present) {
    HANDLE_CODE(unpack_integer(t_resel_eutra_ce_r13, bref, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_resel_sub_prio_r13_present) {
    j.write_str("cellReselectionSubPriority-r13", cell_resel_sub_prio_r13.to_string());
  }
  if (redist_inter_freq_info_r13_present) {
    j.write_fieldname("redistributionInterFreqInfo-r13");
    redist_inter_freq_info_r13.to_json(j);
  }
  if (cell_sel_info_ce_r13_present) {
    j.write_fieldname("cellSelectionInfoCE-r13");
    cell_sel_info_ce_r13.to_json(j);
  }
  if (t_resel_eutra_ce_r13_present) {
    j.write_int("t-ReselectionEUTRA-CE-r13", t_resel_eutra_ce_r13);
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1350 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1350_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_sel_info_ce1_r13_present, 1));

  if (cell_sel_info_ce1_r13_present) {
    HANDLE_CODE(cell_sel_info_ce1_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1350_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_sel_info_ce1_r13_present, 1));

  if (cell_sel_info_ce1_r13_present) {
    HANDLE_CODE(cell_sel_info_ce1_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1350_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_sel_info_ce1_r13_present) {
    j.write_fieldname("cellSelectionInfoCE1-r13");
    cell_sel_info_ce1_r13.to_json(j);
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1360 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1360_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_sel_info_ce1_v1360_present, 1));

  if (cell_sel_info_ce1_v1360_present) {
    HANDLE_CODE(cell_sel_info_ce1_v1360.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1360_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_sel_info_ce1_v1360_present, 1));

  if (cell_sel_info_ce1_v1360_present) {
    HANDLE_CODE(cell_sel_info_ce1_v1360.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1360_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_sel_info_ce1_v1360_present) {
    j.write_fieldname("cellSelectionInfoCE1-v1360");
    cell_sel_info_ce1_v1360.to_json(j);
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1530 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_freq_neigh_hsdn_cell_list_r15_present, 1));
  HANDLE_CODE(bref.pack(cell_sel_info_ce_v1530_present, 1));

  HANDLE_CODE(bref.pack(hsdn_ind_r15, 1));
  if (inter_freq_neigh_hsdn_cell_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_hsdn_cell_list_r15, 1, 16));
  }
  if (cell_sel_info_ce_v1530_present) {
    HANDLE_CODE(cell_sel_info_ce_v1530.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_freq_neigh_hsdn_cell_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(cell_sel_info_ce_v1530_present, 1));

  HANDLE_CODE(bref.unpack(hsdn_ind_r15, 1));
  if (inter_freq_neigh_hsdn_cell_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_hsdn_cell_list_r15, bref, 1, 16));
  }
  if (cell_sel_info_ce_v1530_present) {
    HANDLE_CODE(cell_sel_info_ce_v1530.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("hsdn-Indication-r15", hsdn_ind_r15);
  if (inter_freq_neigh_hsdn_cell_list_r15_present) {
    j.start_array("interFreqNeighHSDN-CellList-r15");
    for (const auto& e1 : inter_freq_neigh_hsdn_cell_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cell_sel_info_ce_v1530_present) {
    j.write_fieldname("cellSelectionInfoCE-v1530");
    cell_sel_info_ce_v1530.to_json(j);
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1610 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(alt_cell_resel_prio_r16_present, 1));
  HANDLE_CODE(bref.pack(alt_cell_resel_sub_prio_r16_present, 1));
  HANDLE_CODE(bref.pack(rss_cfg_carrier_info_r16_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_neigh_cell_list_v1610_present, 1));

  if (alt_cell_resel_prio_r16_present) {
    HANDLE_CODE(pack_integer(bref, alt_cell_resel_prio_r16, (uint8_t)0u, (uint8_t)7u));
  }
  if (alt_cell_resel_sub_prio_r16_present) {
    HANDLE_CODE(alt_cell_resel_sub_prio_r16.pack(bref));
  }
  if (rss_cfg_carrier_info_r16_present) {
    HANDLE_CODE(rss_cfg_carrier_info_r16.pack(bref));
  }
  if (inter_freq_neigh_cell_list_v1610_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_cell_list_v1610, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(alt_cell_resel_prio_r16_present, 1));
  HANDLE_CODE(bref.unpack(alt_cell_resel_sub_prio_r16_present, 1));
  HANDLE_CODE(bref.unpack(rss_cfg_carrier_info_r16_present, 1));
  HANDLE_CODE(bref.unpack(inter_freq_neigh_cell_list_v1610_present, 1));

  if (alt_cell_resel_prio_r16_present) {
    HANDLE_CODE(unpack_integer(alt_cell_resel_prio_r16, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (alt_cell_resel_sub_prio_r16_present) {
    HANDLE_CODE(alt_cell_resel_sub_prio_r16.unpack(bref));
  }
  if (rss_cfg_carrier_info_r16_present) {
    HANDLE_CODE(rss_cfg_carrier_info_r16.unpack(bref));
  }
  if (inter_freq_neigh_cell_list_v1610_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_cell_list_v1610, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (alt_cell_resel_prio_r16_present) {
    j.write_int("altCellReselectionPriority-r16", alt_cell_resel_prio_r16);
  }
  if (alt_cell_resel_sub_prio_r16_present) {
    j.write_str("altCellReselectionSubPriority-r16", alt_cell_resel_sub_prio_r16.to_string());
  }
  if (rss_cfg_carrier_info_r16_present) {
    j.write_fieldname("rss-ConfigCarrierInfo-r16");
    rss_cfg_carrier_info_r16.to_json(j);
  }
  if (inter_freq_neigh_cell_list_v1610_present) {
    j.start_array("interFreqNeighCellList-v1610");
    for (const auto& e1 : inter_freq_neigh_cell_list_v1610) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// IntraFreqNeighCellInfo ::= SEQUENCE
SRSASN_CODE intra_freq_neigh_cell_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(q_offset_cell.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_neigh_cell_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(q_offset_cell.unpack(bref));

  return SRSASN_SUCCESS;
}
void intra_freq_neigh_cell_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  j.write_str("q-OffsetCell", q_offset_cell.to_string());
  j.end_obj();
}

// IntraFreqNeighCellInfo-v1610 ::= SEQUENCE
SRSASN_CODE intra_freq_neigh_cell_info_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rss_meas_pwr_bias_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_neigh_cell_info_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rss_meas_pwr_bias_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void intra_freq_neigh_cell_info_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rss-MeasPowerBias-r16", rss_meas_pwr_bias_r16.to_string());
  j.end_obj();
}

// MBMS-CarrierType-r14 ::= SEQUENCE
SRSASN_CODE mbms_carrier_type_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(frame_offset_r14_present, 1));

  HANDLE_CODE(carrier_type_r14.pack(bref));
  if (frame_offset_r14_present) {
    HANDLE_CODE(pack_integer(bref, frame_offset_r14, (uint8_t)0u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_carrier_type_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(frame_offset_r14_present, 1));

  HANDLE_CODE(carrier_type_r14.unpack(bref));
  if (frame_offset_r14_present) {
    HANDLE_CODE(unpack_integer(frame_offset_r14, bref, (uint8_t)0u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
void mbms_carrier_type_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("carrierType-r14", carrier_type_r14.to_string());
  if (frame_offset_r14_present) {
    j.write_int("frameOffset-r14", frame_offset_r14);
  }
  j.end_obj();
}

const char* mbms_carrier_type_r14_s::carrier_type_r14_opts::to_string() const
{
  static const char* options[] = {"mbms", "fembmsMixed", "fembmsDedicated"};
  return convert_enum_idx(options, 3, value, "mbms_carrier_type_r14_s::carrier_type_r14_e_");
}

// MBMS-SAI-InterFreq-r11 ::= SEQUENCE
SRSASN_CODE mbms_sai_inter_freq_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, dl_carrier_freq_r11, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(pack_dyn_seq_of(bref, mbms_sai_list_r11, 1, 64, integer_packer<uint32_t>(0, 65535)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_sai_inter_freq_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(dl_carrier_freq_r11, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(unpack_dyn_seq_of(mbms_sai_list_r11, bref, 1, 64, integer_packer<uint32_t>(0, 65535)));

  return SRSASN_SUCCESS;
}
void mbms_sai_inter_freq_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-CarrierFreq-r11", dl_carrier_freq_r11);
  j.start_array("mbms-SAI-List-r11");
  for (const auto& e1 : mbms_sai_list_r11) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// MBMS-SAI-InterFreq-v1140 ::= SEQUENCE
SRSASN_CODE mbms_sai_inter_freq_v1140_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(multi_band_info_list_r11_present, 1));

  if (multi_band_info_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_r11, 1, 8, integer_packer<uint16_t>(1, 256)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_sai_inter_freq_v1140_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(multi_band_info_list_r11_present, 1));

  if (multi_band_info_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_r11, bref, 1, 8, integer_packer<uint16_t>(1, 256)));
  }

  return SRSASN_SUCCESS;
}
void mbms_sai_inter_freq_v1140_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (multi_band_info_list_r11_present) {
    j.start_array("multiBandInfoList-r11");
    for (const auto& e1 : multi_band_info_list_r11) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// MBSFN-AreaInfo-r17 ::= SEQUENCE
SRSASN_CODE mbsfn_area_info_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(mbsfn_area_info_r17.pack(bref));
  HANDLE_CODE(pmch_bw_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbsfn_area_info_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(mbsfn_area_info_r17.unpack(bref));
  HANDLE_CODE(pmch_bw_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void mbsfn_area_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mbsfn-AreaInfo-r17");
  mbsfn_area_info_r17.to_json(j);
  j.write_str("pmch-Bandwidth-r17", pmch_bw_r17.to_string());
  j.end_obj();
}

const char* mbsfn_area_info_r17_s::pmch_bw_r17_opts::to_string() const
{
  static const char* options[] = {"n40", "n35", "n30", "spare1"};
  return convert_enum_idx(options, 4, value, "mbsfn_area_info_r17_s::pmch_bw_r17_e_");
}
uint8_t mbsfn_area_info_r17_s::pmch_bw_r17_opts::to_number() const
{
  static const uint8_t options[] = {40, 35, 30};
  return map_enum_number(options, 3, value, "mbsfn_area_info_r17_s::pmch_bw_r17_e_");
}

// MBSFN-AreaInfo-r9 ::= SEQUENCE
SRSASN_CODE mbsfn_area_info_r9_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, mbsfn_area_id_r9, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(non_mbsfn_region_len.pack(bref));
  HANDLE_CODE(pack_integer(bref, notif_ind_r9, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(mcch_cfg_r9.mcch_repeat_period_r9.pack(bref));
  HANDLE_CODE(pack_integer(bref, mcch_cfg_r9.mcch_offset_r9, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(mcch_cfg_r9.mcch_mod_period_r9.pack(bref));
  HANDLE_CODE(mcch_cfg_r9.sf_alloc_info_r9.pack(bref));
  HANDLE_CODE(mcch_cfg_r9.sig_mcs_r9.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= mcch_cfg_r14.is_present();
    group_flags[0] |= subcarrier_spacing_mbms_r14_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mcch_cfg_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(subcarrier_spacing_mbms_r14_present, 1));
      if (mcch_cfg_r14.is_present()) {
        HANDLE_CODE(bref.pack(mcch_cfg_r14->mcch_repeat_period_v1430_present, 1));
        HANDLE_CODE(bref.pack(mcch_cfg_r14->mcch_mod_period_v1430_present, 1));
        if (mcch_cfg_r14->mcch_repeat_period_v1430_present) {
          HANDLE_CODE(mcch_cfg_r14->mcch_repeat_period_v1430.pack(bref));
        }
        if (mcch_cfg_r14->mcch_mod_period_v1430_present) {
          HANDLE_CODE(mcch_cfg_r14->mcch_mod_period_v1430.pack(bref));
        }
      }
      if (subcarrier_spacing_mbms_r14_present) {
        HANDLE_CODE(subcarrier_spacing_mbms_r14.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mbsfn_area_info_r9_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(mbsfn_area_id_r9, bref, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(non_mbsfn_region_len.unpack(bref));
  HANDLE_CODE(unpack_integer(notif_ind_r9, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(mcch_cfg_r9.mcch_repeat_period_r9.unpack(bref));
  HANDLE_CODE(unpack_integer(mcch_cfg_r9.mcch_offset_r9, bref, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(mcch_cfg_r9.mcch_mod_period_r9.unpack(bref));
  HANDLE_CODE(mcch_cfg_r9.sf_alloc_info_r9.unpack(bref));
  HANDLE_CODE(mcch_cfg_r9.sig_mcs_r9.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mcch_cfg_r14_present;
      HANDLE_CODE(bref.unpack(mcch_cfg_r14_present, 1));
      mcch_cfg_r14.set_present(mcch_cfg_r14_present);
      HANDLE_CODE(bref.unpack(subcarrier_spacing_mbms_r14_present, 1));
      if (mcch_cfg_r14.is_present()) {
        HANDLE_CODE(bref.unpack(mcch_cfg_r14->mcch_repeat_period_v1430_present, 1));
        HANDLE_CODE(bref.unpack(mcch_cfg_r14->mcch_mod_period_v1430_present, 1));
        if (mcch_cfg_r14->mcch_repeat_period_v1430_present) {
          HANDLE_CODE(mcch_cfg_r14->mcch_repeat_period_v1430.unpack(bref));
        }
        if (mcch_cfg_r14->mcch_mod_period_v1430_present) {
          HANDLE_CODE(mcch_cfg_r14->mcch_mod_period_v1430.unpack(bref));
        }
      }
      if (subcarrier_spacing_mbms_r14_present) {
        HANDLE_CODE(subcarrier_spacing_mbms_r14.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void mbsfn_area_info_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("mbsfn-AreaId-r9", mbsfn_area_id_r9);
  j.write_str("non-MBSFNregionLength", non_mbsfn_region_len.to_string());
  j.write_int("notificationIndicator-r9", notif_ind_r9);
  j.write_fieldname("mcch-Config-r9");
  j.start_obj();
  j.write_str("mcch-RepetitionPeriod-r9", mcch_cfg_r9.mcch_repeat_period_r9.to_string());
  j.write_int("mcch-Offset-r9", mcch_cfg_r9.mcch_offset_r9);
  j.write_str("mcch-ModificationPeriod-r9", mcch_cfg_r9.mcch_mod_period_r9.to_string());
  j.write_str("sf-AllocInfo-r9", mcch_cfg_r9.sf_alloc_info_r9.to_string());
  j.write_str("signallingMCS-r9", mcch_cfg_r9.sig_mcs_r9.to_string());
  j.end_obj();
  if (ext) {
    if (mcch_cfg_r14.is_present()) {
      j.write_fieldname("mcch-Config-r14");
      j.start_obj();
      if (mcch_cfg_r14->mcch_repeat_period_v1430_present) {
        j.write_str("mcch-RepetitionPeriod-v1430", mcch_cfg_r14->mcch_repeat_period_v1430.to_string());
      }
      if (mcch_cfg_r14->mcch_mod_period_v1430_present) {
        j.write_str("mcch-ModificationPeriod-v1430", mcch_cfg_r14->mcch_mod_period_v1430.to_string());
      }
      j.end_obj();
    }
    if (subcarrier_spacing_mbms_r14_present) {
      j.write_str("subcarrierSpacingMBMS-r14", subcarrier_spacing_mbms_r14.to_string());
    }
  }
  j.end_obj();
}

const char* mbsfn_area_info_r9_s::non_mbsfn_region_len_opts::to_string() const
{
  static const char* options[] = {"s1", "s2"};
  return convert_enum_idx(options, 2, value, "mbsfn_area_info_r9_s::non_mbsfn_region_len_e_");
}
uint8_t mbsfn_area_info_r9_s::non_mbsfn_region_len_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "mbsfn_area_info_r9_s::non_mbsfn_region_len_e_");
}

const char* mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_opts::to_string() const
{
  static const char* options[] = {"rf32", "rf64", "rf128", "rf256"};
  return convert_enum_idx(options, 4, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_e_");
}
uint16_t mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_opts::to_number() const
{
  static const uint16_t options[] = {32, 64, 128, 256};
  return map_enum_number(options, 4, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_e_");
}

const char* mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_opts::to_string() const
{
  static const char* options[] = {"rf512", "rf1024"};
  return convert_enum_idx(options, 2, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_e_");
}
uint16_t mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_opts::to_number() const
{
  static const uint16_t options[] = {512, 1024};
  return map_enum_number(options, 2, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_e_");
}

const char* mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_opts::to_string() const
{
  static const char* options[] = {"n2", "n7", "n13", "n19"};
  return convert_enum_idx(options, 4, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_e_");
}
uint8_t mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_opts::to_number() const
{
  static const uint8_t options[] = {2, 7, 13, 19};
  return map_enum_number(options, 4, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_e_");
}

const char* mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_repeat_period_v1430_opts::to_string() const
{
  static const char* options[] = {"rf1", "rf2", "rf4", "rf8", "rf16"};
  return convert_enum_idx(options, 5, value, "mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_repeat_period_v1430_e_");
}
uint8_t mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_repeat_period_v1430_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8, 16};
  return map_enum_number(options, 5, value, "mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_repeat_period_v1430_e_");
}

const char* mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_mod_period_v1430_opts::to_string() const
{
  static const char* options[] = {"rf1", "rf2", "rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "spare7"};
  return convert_enum_idx(options, 10, value, "mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_mod_period_v1430_e_");
}
uint16_t mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_mod_period_v1430_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 9, value, "mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_mod_period_v1430_e_");
}

const char* mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_opts::to_string() const
{
  static const char* options[] = {"kHz7dot5", "kHz1dot25"};
  return convert_enum_idx(options, 2, value, "mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_e_");
}
float mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_opts::to_number() const
{
  static const float options[] = {7.5, 1.25};
  return map_enum_number(options, 2, value, "mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_e_");
}
const char* mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_opts::to_number_string() const
{
  static const char* options[] = {"7.5", "1.25"};
  return convert_enum_idx(options, 2, value, "mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_e_");
}

// PLMN-Info-r16 ::= SEQUENCE
SRSASN_CODE plmn_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nr_band_list_r16_present, 1));

  if (nr_band_list_r16_present) {
    HANDLE_CODE(nr_band_list_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_info_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nr_band_list_r16_present, 1));

  if (nr_band_list_r16_present) {
    HANDLE_CODE(nr_band_list_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void plmn_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nr_band_list_r16_present) {
    j.write_str("nr-BandList-r16", nr_band_list_r16.to_string());
  }
  j.end_obj();
}

// ReselectionInfoRelay-r13 ::= SEQUENCE
SRSASN_CODE resel_info_relay_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(min_hyst_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, q_rx_lev_min_r13, (int8_t)-70, (int8_t)-22));
  HANDLE_CODE(filt_coef_r13.pack(bref));
  if (min_hyst_r13_present) {
    HANDLE_CODE(min_hyst_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE resel_info_relay_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(min_hyst_r13_present, 1));

  HANDLE_CODE(unpack_integer(q_rx_lev_min_r13, bref, (int8_t)-70, (int8_t)-22));
  HANDLE_CODE(filt_coef_r13.unpack(bref));
  if (min_hyst_r13_present) {
    HANDLE_CODE(min_hyst_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void resel_info_relay_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("q-RxLevMin-r13", q_rx_lev_min_r13);
  j.write_str("filterCoefficient-r13", filt_coef_r13.to_string());
  if (min_hyst_r13_present) {
    j.write_str("minHyst-r13", min_hyst_r13.to_string());
  }
  j.end_obj();
}

const char* resel_info_relay_r13_s::min_hyst_r13_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12", "dBinf"};
  return convert_enum_idx(options, 6, value, "resel_info_relay_r13_s::min_hyst_r13_e_");
}
uint8_t resel_info_relay_r13_s::min_hyst_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "resel_info_relay_r13_s::min_hyst_r13_e_");
}

// SIB8-PerPLMN-r11 ::= SEQUENCE
SRSASN_CODE sib8_per_plmn_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, plmn_id_r11, (uint8_t)1u, (uint8_t)6u));
  HANDLE_CODE(params_cdma2000_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib8_per_plmn_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(plmn_id_r11, bref, (uint8_t)1u, (uint8_t)6u));
  HANDLE_CODE(params_cdma2000_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void sib8_per_plmn_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("plmn-Identity-r11", plmn_id_r11);
  j.write_fieldname("parametersCDMA2000-r11");
  params_cdma2000_r11.to_json(j);
  j.end_obj();
}

void sib8_per_plmn_r11_s::params_cdma2000_r11_c_::set(types::options e)
{
  type_ = e;
}
params_cdma2000_r11_s& sib8_per_plmn_r11_s::params_cdma2000_r11_c_::set_explicit_value()
{
  set(types::explicit_value);
  return c;
}
void sib8_per_plmn_r11_s::params_cdma2000_r11_c_::set_default_value()
{
  set(types::default_value);
}
void sib8_per_plmn_r11_s::params_cdma2000_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::explicit_value:
      j.write_fieldname("explicitValue");
      c.to_json(j);
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "sib8_per_plmn_r11_s::params_cdma2000_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE sib8_per_plmn_r11_s::params_cdma2000_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "sib8_per_plmn_r11_s::params_cdma2000_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib8_per_plmn_r11_s::params_cdma2000_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "sib8_per_plmn_r11_s::params_cdma2000_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-CarrierFreqInfo-r12 ::= SEQUENCE
SRSASN_CODE sl_carrier_freq_info_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(plmn_id_list_r12_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r12, (uint32_t)0u, (uint32_t)262143u));
  if (plmn_id_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r12, 1, 6));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_carrier_freq_info_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(plmn_id_list_r12_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r12, bref, (uint32_t)0u, (uint32_t)262143u));
  if (plmn_id_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r12, bref, 1, 6));
  }

  return SRSASN_SUCCESS;
}
void sl_carrier_freq_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r12", carrier_freq_r12);
  if (plmn_id_list_r12_present) {
    j.start_array("plmn-IdentityList-r12");
    for (const auto& e1 : plmn_id_list_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-CarrierFreqInfo-v1310 ::= SEQUENCE
SRSASN_CODE sl_carrier_freq_info_v1310_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(disc_res_non_ps_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_res_ps_r13_present, 1));
  HANDLE_CODE(bref.pack(disc_cfg_other_r13_present, 1));

  if (disc_res_non_ps_r13_present) {
    HANDLE_CODE(disc_res_non_ps_r13.pack(bref));
  }
  if (disc_res_ps_r13_present) {
    HANDLE_CODE(disc_res_ps_r13.pack(bref));
  }
  if (disc_cfg_other_r13_present) {
    HANDLE_CODE(disc_cfg_other_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_carrier_freq_info_v1310_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(disc_res_non_ps_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_res_ps_r13_present, 1));
  HANDLE_CODE(bref.unpack(disc_cfg_other_r13_present, 1));

  if (disc_res_non_ps_r13_present) {
    HANDLE_CODE(disc_res_non_ps_r13.unpack(bref));
  }
  if (disc_res_ps_r13_present) {
    HANDLE_CODE(disc_res_ps_r13.unpack(bref));
  }
  if (disc_cfg_other_r13_present) {
    HANDLE_CODE(disc_cfg_other_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_carrier_freq_info_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (disc_res_non_ps_r13_present) {
    j.write_fieldname("discResourcesNonPS-r13");
    disc_res_non_ps_r13.to_json(j);
  }
  if (disc_res_ps_r13_present) {
    j.write_fieldname("discResourcesPS-r13");
    disc_res_ps_r13.to_json(j);
  }
  if (disc_cfg_other_r13_present) {
    j.write_fieldname("discConfigOther-r13");
    disc_cfg_other_r13.to_json(j);
  }
  j.end_obj();
}

// SL-PPPP-TxConfigIndex-r15 ::= SEQUENCE
SRSASN_CODE sl_pppp_tx_cfg_idx_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, prio_thres_r15, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pack_integer(bref, default_tx_cfg_idx_r15, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, cbr_cfg_idx_r15, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pack_dyn_seq_of(bref, tx_cfg_idx_list_r15, 1, 16, integer_packer<uint8_t>(0, 63)));
  HANDLE_CODE(pack_dyn_seq_of(bref, mcs_pssch_range_list_r15, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pppp_tx_cfg_idx_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(prio_thres_r15, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(unpack_integer(default_tx_cfg_idx_r15, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(cbr_cfg_idx_r15, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(unpack_dyn_seq_of(tx_cfg_idx_list_r15, bref, 1, 16, integer_packer<uint8_t>(0, 63)));
  HANDLE_CODE(unpack_dyn_seq_of(mcs_pssch_range_list_r15, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void sl_pppp_tx_cfg_idx_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("priorityThreshold-r15", prio_thres_r15);
  j.write_int("defaultTxConfigIndex-r15", default_tx_cfg_idx_r15);
  j.write_int("cbr-ConfigIndex-r15", cbr_cfg_idx_r15);
  j.start_array("tx-ConfigIndexList-r15");
  for (const auto& e1 : tx_cfg_idx_list_r15) {
    j.write_int(e1);
  }
  j.end_array();
  j.start_array("mcs-PSSCH-RangeList-r15");
  for (const auto& e1 : mcs_pssch_range_list_r15) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// SatelliteInfo-r17 ::= SEQUENCE
SRSASN_CODE satellite_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, satellite_id_r17, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(bref.pack(service_info_r17.tle_ephemeris_params_r17_present, 1));
  HANDLE_CODE(bref.pack(service_info_r17.t_service_start_r17_present, 1));
  if (service_info_r17.tle_ephemeris_params_r17_present) {
    HANDLE_CODE(service_info_r17.tle_ephemeris_params_r17.pack(bref));
  }
  if (service_info_r17.t_service_start_r17_present) {
    HANDLE_CODE(pack_integer(bref, service_info_r17.t_service_start_r17, (uint32_t)0u, (uint32_t)1048575u));
  }
  HANDLE_CODE(bref.pack(footprint_info_r17.ref_point_r17_present, 1));
  HANDLE_CODE(bref.pack(footprint_info_r17.elevation_angles_r17_present, 1));
  HANDLE_CODE(bref.pack(footprint_info_r17.radius_r17_present, 1));
  if (footprint_info_r17.ref_point_r17_present) {
    HANDLE_CODE(pack_integer(bref, footprint_info_r17.ref_point_r17.longitude_r17, (int32_t)-131072, (int32_t)131071));
    HANDLE_CODE(pack_integer(bref, footprint_info_r17.ref_point_r17.latitude_r17, (int32_t)-131072, (int32_t)131071));
  }
  if (footprint_info_r17.elevation_angles_r17_present) {
    HANDLE_CODE(bref.pack(footprint_info_r17.elevation_angles_r17.elevation_angle_left_r17_present, 1));
    HANDLE_CODE(
        pack_integer(bref, footprint_info_r17.elevation_angles_r17.elevation_angle_right_r17, (int8_t)-14, (int8_t)14));
    if (footprint_info_r17.elevation_angles_r17.elevation_angle_left_r17_present) {
      HANDLE_CODE(pack_integer(
          bref, footprint_info_r17.elevation_angles_r17.elevation_angle_left_r17, (int8_t)-14, (int8_t)14));
    }
  }
  if (footprint_info_r17.radius_r17_present) {
    HANDLE_CODE(pack_integer(bref, footprint_info_r17.radius_r17, (uint16_t)1u, (uint16_t)256u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE satellite_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(satellite_id_r17, bref, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(bref.unpack(service_info_r17.tle_ephemeris_params_r17_present, 1));
  HANDLE_CODE(bref.unpack(service_info_r17.t_service_start_r17_present, 1));
  if (service_info_r17.tle_ephemeris_params_r17_present) {
    HANDLE_CODE(service_info_r17.tle_ephemeris_params_r17.unpack(bref));
  }
  if (service_info_r17.t_service_start_r17_present) {
    HANDLE_CODE(unpack_integer(service_info_r17.t_service_start_r17, bref, (uint32_t)0u, (uint32_t)1048575u));
  }
  HANDLE_CODE(bref.unpack(footprint_info_r17.ref_point_r17_present, 1));
  HANDLE_CODE(bref.unpack(footprint_info_r17.elevation_angles_r17_present, 1));
  HANDLE_CODE(bref.unpack(footprint_info_r17.radius_r17_present, 1));
  if (footprint_info_r17.ref_point_r17_present) {
    HANDLE_CODE(
        unpack_integer(footprint_info_r17.ref_point_r17.longitude_r17, bref, (int32_t)-131072, (int32_t)131071));
    HANDLE_CODE(unpack_integer(footprint_info_r17.ref_point_r17.latitude_r17, bref, (int32_t)-131072, (int32_t)131071));
  }
  if (footprint_info_r17.elevation_angles_r17_present) {
    HANDLE_CODE(bref.unpack(footprint_info_r17.elevation_angles_r17.elevation_angle_left_r17_present, 1));
    HANDLE_CODE(unpack_integer(
        footprint_info_r17.elevation_angles_r17.elevation_angle_right_r17, bref, (int8_t)-14, (int8_t)14));
    if (footprint_info_r17.elevation_angles_r17.elevation_angle_left_r17_present) {
      HANDLE_CODE(unpack_integer(
          footprint_info_r17.elevation_angles_r17.elevation_angle_left_r17, bref, (int8_t)-14, (int8_t)14));
    }
  }
  if (footprint_info_r17.radius_r17_present) {
    HANDLE_CODE(unpack_integer(footprint_info_r17.radius_r17, bref, (uint16_t)1u, (uint16_t)256u));
  }

  return SRSASN_SUCCESS;
}
void satellite_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("satelliteId-r17", satellite_id_r17);
  j.write_fieldname("serviceInfo-r17");
  j.start_obj();
  if (service_info_r17.tle_ephemeris_params_r17_present) {
    j.write_fieldname("tle-EphemerisParameters-r17");
    service_info_r17.tle_ephemeris_params_r17.to_json(j);
  }
  if (service_info_r17.t_service_start_r17_present) {
    j.write_int("t-ServiceStart-r17", service_info_r17.t_service_start_r17);
  }
  j.end_obj();
  j.write_fieldname("footprintInfo-r17");
  j.start_obj();
  if (footprint_info_r17.ref_point_r17_present) {
    j.write_fieldname("referencePoint-r17");
    j.start_obj();
    j.write_int("longitude-r17", footprint_info_r17.ref_point_r17.longitude_r17);
    j.write_int("latitude-r17", footprint_info_r17.ref_point_r17.latitude_r17);
    j.end_obj();
  }
  if (footprint_info_r17.elevation_angles_r17_present) {
    j.write_fieldname("elevationAngles-r17");
    j.start_obj();
    j.write_int("elevationAngleRight-r17", footprint_info_r17.elevation_angles_r17.elevation_angle_right_r17);
    if (footprint_info_r17.elevation_angles_r17.elevation_angle_left_r17_present) {
      j.write_int("elevationAngleLeft-r17", footprint_info_r17.elevation_angles_r17.elevation_angle_left_r17);
    }
    j.end_obj();
  }
  if (footprint_info_r17.radius_r17_present) {
    j.write_int("radius-r17", footprint_info_r17.radius_r17);
  }
  j.end_obj();
  j.end_obj();
}

// UAC-BarringInfoSet-r15 ::= SEQUENCE
SRSASN_CODE uac_barr_info_set_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(uac_barr_factor_r15.pack(bref));
  HANDLE_CODE(uac_barr_time_r15.pack(bref));
  HANDLE_CODE(uac_barr_for_access_id_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_info_set_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(uac_barr_factor_r15.unpack(bref));
  HANDLE_CODE(uac_barr_time_r15.unpack(bref));
  HANDLE_CODE(uac_barr_for_access_id_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void uac_barr_info_set_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("uac-BarringFactor-r15", uac_barr_factor_r15.to_string());
  j.write_str("uac-BarringTime-r15", uac_barr_time_r15.to_string());
  j.write_str("uac-BarringForAccessIdentity-r15", uac_barr_for_access_id_r15.to_string());
  j.end_obj();
}

const char* uac_barr_info_set_r15_s::uac_barr_factor_r15_opts::to_string() const
{
  static const char* options[] = {
      "p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
  return convert_enum_idx(options, 16, value, "uac_barr_info_set_r15_s::uac_barr_factor_r15_e_");
}
float uac_barr_info_set_r15_s::uac_barr_factor_r15_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(options, 16, value, "uac_barr_info_set_r15_s::uac_barr_factor_r15_e_");
}
const char* uac_barr_info_set_r15_s::uac_barr_factor_r15_opts::to_number_string() const
{
  static const char* options[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(options, 16, value, "uac_barr_info_set_r15_s::uac_barr_factor_r15_e_");
}

const char* uac_barr_info_set_r15_s::uac_barr_time_r15_opts::to_string() const
{
  static const char* options[] = {"s4", "s8", "s16", "s32", "s64", "s128", "s256", "s512"};
  return convert_enum_idx(options, 8, value, "uac_barr_info_set_r15_s::uac_barr_time_r15_e_");
}
uint16_t uac_barr_info_set_r15_s::uac_barr_time_r15_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 8, value, "uac_barr_info_set_r15_s::uac_barr_time_r15_e_");
}

// UAC-BarringInfoSet-v1700 ::= SEQUENCE
SRSASN_CODE uac_barr_info_set_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(uac_barr_factor_for_ai3_r17_present, 1));

  if (uac_barr_factor_for_ai3_r17_present) {
    HANDLE_CODE(uac_barr_factor_for_ai3_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_info_set_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(uac_barr_factor_for_ai3_r17_present, 1));

  if (uac_barr_factor_for_ai3_r17_present) {
    HANDLE_CODE(uac_barr_factor_for_ai3_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void uac_barr_info_set_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (uac_barr_factor_for_ai3_r17_present) {
    j.write_str("uac-BarringFactorForAI3-r17", uac_barr_factor_for_ai3_r17.to_string());
  }
  j.end_obj();
}

const char* uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::to_string() const
{
  static const char* options[] = {
      "p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
  return convert_enum_idx(options, 16, value, "uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_e_");
}
float uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(options, 16, value, "uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_e_");
}
const char* uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::to_number_string() const
{
  static const char* options[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(options, 16, value, "uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_e_");
}

// UAC-BarringPerPLMN-r15 ::= SEQUENCE
SRSASN_CODE uac_barr_per_plmn_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(uac_ac_barr_list_type_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, plmn_id_idx_r15, (uint8_t)1u, (uint8_t)6u));
  if (uac_ac_barr_list_type_r15_present) {
    HANDLE_CODE(uac_ac_barr_list_type_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_per_plmn_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(uac_ac_barr_list_type_r15_present, 1));

  HANDLE_CODE(unpack_integer(plmn_id_idx_r15, bref, (uint8_t)1u, (uint8_t)6u));
  if (uac_ac_barr_list_type_r15_present) {
    HANDLE_CODE(uac_ac_barr_list_type_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void uac_barr_per_plmn_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("plmn-IdentityIndex-r15", plmn_id_idx_r15);
  if (uac_ac_barr_list_type_r15_present) {
    j.write_fieldname("uac-AC-BarringListType-r15");
    uac_ac_barr_list_type_r15.to_json(j);
  }
  j.end_obj();
}

void uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::destroy_()
{
  switch (type_) {
    case types::uac_implicit_ac_barr_list_r15:
      c.destroy<uac_implicit_ac_barr_list_r15_l_>();
      break;
    case types::uac_explicit_ac_barr_list_r15:
      c.destroy<uac_barr_per_cat_list_r15_l>();
      break;
    default:
      break;
  }
}
void uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::uac_implicit_ac_barr_list_r15:
      c.init<uac_implicit_ac_barr_list_r15_l_>();
      break;
    case types::uac_explicit_ac_barr_list_r15:
      c.init<uac_barr_per_cat_list_r15_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_");
  }
}
uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::uac_ac_barr_list_type_r15_c_(
    const uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::uac_implicit_ac_barr_list_r15:
      c.init(other.c.get<uac_implicit_ac_barr_list_r15_l_>());
      break;
    case types::uac_explicit_ac_barr_list_r15:
      c.init(other.c.get<uac_barr_per_cat_list_r15_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_");
  }
}
uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_& uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::operator=(
    const uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::uac_implicit_ac_barr_list_r15:
      c.set(other.c.get<uac_implicit_ac_barr_list_r15_l_>());
      break;
    case types::uac_explicit_ac_barr_list_r15:
      c.set(other.c.get<uac_barr_per_cat_list_r15_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_");
  }

  return *this;
}
uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::uac_implicit_ac_barr_list_r15_l_&
uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::set_uac_implicit_ac_barr_list_r15()
{
  set(types::uac_implicit_ac_barr_list_r15);
  return c.get<uac_implicit_ac_barr_list_r15_l_>();
}
uac_barr_per_cat_list_r15_l& uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::set_uac_explicit_ac_barr_list_r15()
{
  set(types::uac_explicit_ac_barr_list_r15);
  return c.get<uac_barr_per_cat_list_r15_l>();
}
void uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::uac_implicit_ac_barr_list_r15:
      j.start_array("uac-ImplicitAC-BarringList-r15");
      for (const auto& e1 : c.get<uac_implicit_ac_barr_list_r15_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::uac_explicit_ac_barr_list_r15:
      j.start_array("uac-ExplicitAC-BarringList-r15");
      for (const auto& e1 : c.get<uac_barr_per_cat_list_r15_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::uac_implicit_ac_barr_list_r15:
      HANDLE_CODE(pack_fixed_seq_of(bref,
                                    &(c.get<uac_implicit_ac_barr_list_r15_l_>())[0],
                                    c.get<uac_implicit_ac_barr_list_r15_l_>().size(),
                                    integer_packer<uint8_t>(1, 8)));
      break;
    case types::uac_explicit_ac_barr_list_r15:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uac_barr_per_cat_list_r15_l>(), 1, 63));
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::uac_implicit_ac_barr_list_r15:
      HANDLE_CODE(unpack_fixed_seq_of(&(c.get<uac_implicit_ac_barr_list_r15_l_>())[0],
                                      bref,
                                      c.get<uac_implicit_ac_barr_list_r15_l_>().size(),
                                      integer_packer<uint8_t>(1, 8)));
      break;
    case types::uac_explicit_ac_barr_list_r15:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uac_barr_per_cat_list_r15_l>(), bref, 1, 63));
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ApplicableDisasterInfo-r17 ::= CHOICE
void applicable_disaster_info_r17_c::set(types::options e)
{
  type_ = e;
}
void applicable_disaster_info_r17_c::set_no_disaster_roaming_r17()
{
  set(types::no_disaster_roaming_r17);
}
void applicable_disaster_info_r17_c::set_disaster_related_ind_r17()
{
  set(types::disaster_related_ind_r17);
}
void applicable_disaster_info_r17_c::set_common_plmns_r17()
{
  set(types::common_plmns_r17);
}
applicable_disaster_info_r17_c::ded_plmns_r17_l_& applicable_disaster_info_r17_c::set_ded_plmns_r17()
{
  set(types::ded_plmns_r17);
  return c;
}
void applicable_disaster_info_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::no_disaster_roaming_r17:
      break;
    case types::disaster_related_ind_r17:
      break;
    case types::common_plmns_r17:
      break;
    case types::ded_plmns_r17:
      j.start_array("dedicatedPLMNs-r17");
      for (const auto& e1 : c) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "applicable_disaster_info_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE applicable_disaster_info_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::no_disaster_roaming_r17:
      break;
    case types::disaster_related_ind_r17:
      break;
    case types::common_plmns_r17:
      break;
    case types::ded_plmns_r17:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 6));
      break;
    default:
      log_invalid_choice_id(type_, "applicable_disaster_info_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE applicable_disaster_info_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::no_disaster_roaming_r17:
      break;
    case types::disaster_related_ind_r17:
      break;
    case types::common_plmns_r17:
      break;
    case types::ded_plmns_r17:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 6));
      break;
    default:
      log_invalid_choice_id(type_, "applicable_disaster_info_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CarrierFreqInfoUTRA-v1250 ::= SEQUENCE
SRSASN_CODE carrier_freq_info_utra_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(reduced_meas_performance_r12_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_info_utra_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(reduced_meas_performance_r12_present, 1));

  return SRSASN_SUCCESS;
}
void carrier_freq_info_utra_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reduced_meas_performance_r12_present) {
    j.write_str("reducedMeasPerformance-r12", "true");
  }
  j.end_obj();
}

// CellReselectionInfoCommon-v1460 ::= SEQUENCE
SRSASN_CODE cell_resel_info_common_v1460_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(s_search_delta_p_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_resel_info_common_v1460_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(s_search_delta_p_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_resel_info_common_v1460_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("s-SearchDeltaP-r14", s_search_delta_p_r14.to_string());
  j.end_obj();
}

const char* cell_resel_info_common_v1460_s::s_search_delta_p_r14_opts::to_string() const
{
  static const char* options[] = {"dB6", "dB9", "dB12", "dB15"};
  return convert_enum_idx(options, 4, value, "cell_resel_info_common_v1460_s::s_search_delta_p_r14_e_");
}
uint8_t cell_resel_info_common_v1460_s::s_search_delta_p_r14_opts::to_number() const
{
  static const uint8_t options[] = {6, 9, 12, 15};
  return map_enum_number(options, 4, value, "cell_resel_info_common_v1460_s::s_search_delta_p_r14_e_");
}

// CellReselectionInfoHSDN-r15 ::= SEQUENCE
SRSASN_CODE cell_resel_info_hsdn_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, cell_equivalent_size_r15, (uint8_t)2u, (uint8_t)16u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_resel_info_hsdn_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(cell_equivalent_size_r15, bref, (uint8_t)2u, (uint8_t)16u));

  return SRSASN_SUCCESS;
}
void cell_resel_info_hsdn_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cellEquivalentSize-r15", cell_equivalent_size_r15);
  j.end_obj();
}

// CellReselectionParametersCDMA2000 ::= SEQUENCE
SRSASN_CODE cell_resel_params_cdma2000_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(t_resel_cdma2000_sf_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, band_class_list, 1, 32));
  HANDLE_CODE(pack_dyn_seq_of(bref, neigh_cell_list, 1, 16));
  HANDLE_CODE(pack_integer(bref, t_resel_cdma2000, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_cdma2000_sf_present) {
    HANDLE_CODE(t_resel_cdma2000_sf.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_resel_params_cdma2000_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(t_resel_cdma2000_sf_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(band_class_list, bref, 1, 32));
  HANDLE_CODE(unpack_dyn_seq_of(neigh_cell_list, bref, 1, 16));
  HANDLE_CODE(unpack_integer(t_resel_cdma2000, bref, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_cdma2000_sf_present) {
    HANDLE_CODE(t_resel_cdma2000_sf.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cell_resel_params_cdma2000_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("bandClassList");
  for (const auto& e1 : band_class_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("neighCellList");
  for (const auto& e1 : neigh_cell_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("t-ReselectionCDMA2000", t_resel_cdma2000);
  if (t_resel_cdma2000_sf_present) {
    j.write_fieldname("t-ReselectionCDMA2000-SF");
    t_resel_cdma2000_sf.to_json(j);
  }
  j.end_obj();
}

// CellReselectionParametersCDMA2000-v920 ::= SEQUENCE
SRSASN_CODE cell_resel_params_cdma2000_v920_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, neigh_cell_list_v920, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_resel_params_cdma2000_v920_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(neigh_cell_list_v920, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void cell_resel_params_cdma2000_v920_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("neighCellList-v920");
  for (const auto& e1 : neigh_cell_list_v920) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// CellReselectionServingFreqInfo-v1310 ::= SEQUENCE
SRSASN_CODE cell_resel_serving_freq_info_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cell_resel_sub_prio_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_resel_serving_freq_info_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cell_resel_sub_prio_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_resel_serving_freq_info_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cellReselectionSubPriority-r13", cell_resel_sub_prio_r13.to_string());
  j.end_obj();
}

// CellReselectionServingFreqInfo-v1610 ::= SEQUENCE
SRSASN_CODE cell_resel_serving_freq_info_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(alt_cell_resel_prio_r16_present, 1));
  HANDLE_CODE(bref.pack(alt_cell_resel_sub_prio_r16_present, 1));

  if (alt_cell_resel_prio_r16_present) {
    HANDLE_CODE(pack_integer(bref, alt_cell_resel_prio_r16, (uint8_t)0u, (uint8_t)7u));
  }
  if (alt_cell_resel_sub_prio_r16_present) {
    HANDLE_CODE(alt_cell_resel_sub_prio_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_resel_serving_freq_info_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(alt_cell_resel_prio_r16_present, 1));
  HANDLE_CODE(bref.unpack(alt_cell_resel_sub_prio_r16_present, 1));

  if (alt_cell_resel_prio_r16_present) {
    HANDLE_CODE(unpack_integer(alt_cell_resel_prio_r16, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (alt_cell_resel_sub_prio_r16_present) {
    HANDLE_CODE(alt_cell_resel_sub_prio_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cell_resel_serving_freq_info_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (alt_cell_resel_prio_r16_present) {
    j.write_int("altCellReselectionPriority-r16", alt_cell_resel_prio_r16);
  }
  if (alt_cell_resel_sub_prio_r16_present) {
    j.write_str("altCellReselectionSubPriority-r16", alt_cell_resel_sub_prio_r16.to_string());
  }
  j.end_obj();
}

// EAB-ConfigPLMN-r11 ::= SEQUENCE
SRSASN_CODE eab_cfg_plmn_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eab_cfg_r11_present, 1));

  if (eab_cfg_r11_present) {
    HANDLE_CODE(eab_cfg_r11.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eab_cfg_plmn_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(eab_cfg_r11_present, 1));

  if (eab_cfg_r11_present) {
    HANDLE_CODE(eab_cfg_r11.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void eab_cfg_plmn_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eab_cfg_r11_present) {
    j.write_fieldname("eab-Config-r11");
    eab_cfg_r11.to_json(j);
  }
  j.end_obj();
}

// MBMS-NotificationConfig-r9 ::= SEQUENCE
SRSASN_CODE mbms_notif_cfg_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(notif_repeat_coeff_r9.pack(bref));
  HANDLE_CODE(pack_integer(bref, notif_offset_r9, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(pack_integer(bref, notif_sf_idx_r9, (uint8_t)1u, (uint8_t)6u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_notif_cfg_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(notif_repeat_coeff_r9.unpack(bref));
  HANDLE_CODE(unpack_integer(notif_offset_r9, bref, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(unpack_integer(notif_sf_idx_r9, bref, (uint8_t)1u, (uint8_t)6u));

  return SRSASN_SUCCESS;
}
void mbms_notif_cfg_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("notificationRepetitionCoeff-r9", notif_repeat_coeff_r9.to_string());
  j.write_int("notificationOffset-r9", notif_offset_r9);
  j.write_int("notificationSF-Index-r9", notif_sf_idx_r9);
  j.end_obj();
}

const char* mbms_notif_cfg_r9_s::notif_repeat_coeff_r9_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "mbms_notif_cfg_r9_s::notif_repeat_coeff_r9_e_");
}
uint8_t mbms_notif_cfg_r9_s::notif_repeat_coeff_r9_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "mbms_notif_cfg_r9_s::notif_repeat_coeff_r9_e_");
}

// MBMS-NotificationConfig-v1430 ::= SEQUENCE
SRSASN_CODE mbms_notif_cfg_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, notif_sf_idx_v1430, (uint8_t)7u, (uint8_t)10u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_notif_cfg_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(notif_sf_idx_v1430, bref, (uint8_t)7u, (uint8_t)10u));

  return SRSASN_SUCCESS;
}
void mbms_notif_cfg_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("notificationSF-Index-v1430", notif_sf_idx_v1430);
  j.end_obj();
}

// MeasIdleConfigSIB-NR-r16 ::= SEQUENCE
SRSASN_CODE meas_idle_cfg_sib_nr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_idle_carrier_list_nr_r16, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_idle_cfg_sib_nr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(meas_idle_carrier_list_nr_r16, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_idle_cfg_sib_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measIdleCarrierListNR-r16");
  for (const auto& e1 : meas_idle_carrier_list_nr_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MeasIdleConfigSIB-r15 ::= SEQUENCE
SRSASN_CODE meas_idle_cfg_sib_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_idle_carrier_list_eutra_r15, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_idle_cfg_sib_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(meas_idle_carrier_list_eutra_r15, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_idle_cfg_sib_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measIdleCarrierListEUTRA-r15");
  for (const auto& e1 : meas_idle_carrier_list_eutra_r15) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// RedistributionServingInfo-r13 ::= SEQUENCE
SRSASN_CODE redist_serving_info_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(redist_factor_cell_r13_present, 1));
  HANDLE_CODE(bref.pack(redistr_on_paging_only_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, redist_factor_serving_r13, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(t360_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE redist_serving_info_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(redist_factor_cell_r13_present, 1));
  HANDLE_CODE(bref.unpack(redistr_on_paging_only_r13_present, 1));

  HANDLE_CODE(unpack_integer(redist_factor_serving_r13, bref, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(t360_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void redist_serving_info_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("redistributionFactorServing-r13", redist_factor_serving_r13);
  if (redist_factor_cell_r13_present) {
    j.write_str("redistributionFactorCell-r13", "true");
  }
  j.write_str("t360-r13", t360_r13.to_string());
  if (redistr_on_paging_only_r13_present) {
    j.write_str("redistrOnPagingOnly-r13", "true");
  }
  j.end_obj();
}

const char* redist_serving_info_r13_s::t360_r13_opts::to_string() const
{
  static const char* options[] = {"min4", "min8", "min16", "min32", "infinity", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "redist_serving_info_r13_s::t360_r13_e_");
}
int8_t redist_serving_info_r13_s::t360_r13_opts::to_number() const
{
  static const int8_t options[] = {4, 8, 16, 32, -1};
  return map_enum_number(options, 5, value, "redist_serving_info_r13_s::t360_r13_e_");
}

// SC-MCCH-SchedulingInfo-r14 ::= SEQUENCE
SRSASN_CODE sc_mcch_sched_info_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(on_dur_timer_scptm_r14.pack(bref));
  HANDLE_CODE(drx_inactivity_timer_scptm_r14.pack(bref));
  HANDLE_CODE(sched_period_start_offset_scptm_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mcch_sched_info_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(on_dur_timer_scptm_r14.unpack(bref));
  HANDLE_CODE(drx_inactivity_timer_scptm_r14.unpack(bref));
  HANDLE_CODE(sched_period_start_offset_scptm_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void sc_mcch_sched_info_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("onDurationTimerSCPTM-r14", on_dur_timer_scptm_r14.to_string());
  j.write_str("drx-InactivityTimerSCPTM-r14", drx_inactivity_timer_scptm_r14.to_string());
  j.write_fieldname("schedulingPeriodStartOffsetSCPTM-r14");
  sched_period_start_offset_scptm_r14.to_json(j);
  j.end_obj();
}

const char* sc_mcch_sched_info_r14_s::on_dur_timer_scptm_r14_opts::to_string() const
{
  static const char* options[] = {"psf10", "psf20", "psf100", "psf300", "psf500", "psf1000", "psf1200", "psf1600"};
  return convert_enum_idx(options, 8, value, "sc_mcch_sched_info_r14_s::on_dur_timer_scptm_r14_e_");
}
uint16_t sc_mcch_sched_info_r14_s::on_dur_timer_scptm_r14_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 100, 300, 500, 1000, 1200, 1600};
  return map_enum_number(options, 8, value, "sc_mcch_sched_info_r14_s::on_dur_timer_scptm_r14_e_");
}

const char* sc_mcch_sched_info_r14_s::drx_inactivity_timer_scptm_r14_opts::to_string() const
{
  static const char* options[] = {"psf0",
                                  "psf1",
                                  "psf2",
                                  "psf4",
                                  "psf8",
                                  "psf16",
                                  "psf32",
                                  "psf64",
                                  "psf128",
                                  "psf256",
                                  "ps512",
                                  "psf1024",
                                  "psf2048",
                                  "psf4096",
                                  "psf8192",
                                  "psf16384"};
  return convert_enum_idx(options, 16, value, "sc_mcch_sched_info_r14_s::drx_inactivity_timer_scptm_r14_e_");
}
uint16_t sc_mcch_sched_info_r14_s::drx_inactivity_timer_scptm_r14_opts::to_number() const
{
  static const uint16_t options[] = {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};
  return map_enum_number(options, 16, value, "sc_mcch_sched_info_r14_s::drx_inactivity_timer_scptm_r14_e_");
}

void sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::destroy_() {}
void sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::sched_period_start_offset_scptm_r14_c_(
    const sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf512:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf1024:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf2048:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf4096:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf8192:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_");
  }
}
sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_&
sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::operator=(
    const sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf512:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf1024:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf2048:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf4096:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf8192:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_");
  }

  return *this;
}
uint8_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf10()
{
  set(types::sf10);
  return c.get<uint8_t>();
}
uint8_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf20()
{
  set(types::sf20);
  return c.get<uint8_t>();
}
uint8_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf32()
{
  set(types::sf32);
  return c.get<uint8_t>();
}
uint8_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf40()
{
  set(types::sf40);
  return c.get<uint8_t>();
}
uint8_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf64()
{
  set(types::sf64);
  return c.get<uint8_t>();
}
uint8_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf80()
{
  set(types::sf80);
  return c.get<uint8_t>();
}
uint8_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf128()
{
  set(types::sf128);
  return c.get<uint8_t>();
}
uint8_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf160()
{
  set(types::sf160);
  return c.get<uint8_t>();
}
uint16_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf256()
{
  set(types::sf256);
  return c.get<uint16_t>();
}
uint16_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf320()
{
  set(types::sf320);
  return c.get<uint16_t>();
}
uint16_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf512()
{
  set(types::sf512);
  return c.get<uint16_t>();
}
uint16_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf640()
{
  set(types::sf640);
  return c.get<uint16_t>();
}
uint16_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf1024()
{
  set(types::sf1024);
  return c.get<uint16_t>();
}
uint16_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf2048()
{
  set(types::sf2048);
  return c.get<uint16_t>();
}
uint16_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf4096()
{
  set(types::sf4096);
  return c.get<uint16_t>();
}
uint16_t& sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf8192()
{
  set(types::sf8192);
  return c.get<uint16_t>();
}
void sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf10:
      j.write_int("sf10", c.get<uint8_t>());
      break;
    case types::sf20:
      j.write_int("sf20", c.get<uint8_t>());
      break;
    case types::sf32:
      j.write_int("sf32", c.get<uint8_t>());
      break;
    case types::sf40:
      j.write_int("sf40", c.get<uint8_t>());
      break;
    case types::sf64:
      j.write_int("sf64", c.get<uint8_t>());
      break;
    case types::sf80:
      j.write_int("sf80", c.get<uint8_t>());
      break;
    case types::sf128:
      j.write_int("sf128", c.get<uint8_t>());
      break;
    case types::sf160:
      j.write_int("sf160", c.get<uint8_t>());
      break;
    case types::sf256:
      j.write_int("sf256", c.get<uint16_t>());
      break;
    case types::sf320:
      j.write_int("sf320", c.get<uint16_t>());
      break;
    case types::sf512:
      j.write_int("sf512", c.get<uint16_t>());
      break;
    case types::sf640:
      j.write_int("sf640", c.get<uint16_t>());
      break;
    case types::sf1024:
      j.write_int("sf1024", c.get<uint16_t>());
      break;
    case types::sf2048:
      j.write_int("sf2048", c.get<uint16_t>());
      break;
    case types::sf4096:
      j.write_int("sf4096", c.get<uint16_t>());
      break;
    case types::sf8192:
      j.write_int("sf8192", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sf40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::sf80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sf160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sf320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf512:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sf640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1024:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::sf2048:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::sf4096:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u));
      break;
    case types::sf8192:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)8191u));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sf40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::sf80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sf160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sf320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf512:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sf640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1024:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::sf2048:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::sf4096:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u));
      break;
    case types::sf8192:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)8191u));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SL-DiscConfigRelayUE-r13 ::= SEQUENCE
SRSASN_CODE sl_disc_cfg_relay_ue_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(thresh_high_r13_present, 1));
  HANDLE_CODE(bref.pack(thresh_low_r13_present, 1));
  HANDLE_CODE(bref.pack(hyst_max_r13_present, 1));
  HANDLE_CODE(bref.pack(hyst_min_r13_present, 1));

  if (thresh_high_r13_present) {
    HANDLE_CODE(pack_integer(bref, thresh_high_r13, (uint8_t)0u, (uint8_t)49u));
  }
  if (thresh_low_r13_present) {
    HANDLE_CODE(pack_integer(bref, thresh_low_r13, (uint8_t)0u, (uint8_t)49u));
  }
  if (hyst_max_r13_present) {
    HANDLE_CODE(hyst_max_r13.pack(bref));
  }
  if (hyst_min_r13_present) {
    HANDLE_CODE(hyst_min_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_relay_ue_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(thresh_high_r13_present, 1));
  HANDLE_CODE(bref.unpack(thresh_low_r13_present, 1));
  HANDLE_CODE(bref.unpack(hyst_max_r13_present, 1));
  HANDLE_CODE(bref.unpack(hyst_min_r13_present, 1));

  if (thresh_high_r13_present) {
    HANDLE_CODE(unpack_integer(thresh_high_r13, bref, (uint8_t)0u, (uint8_t)49u));
  }
  if (thresh_low_r13_present) {
    HANDLE_CODE(unpack_integer(thresh_low_r13, bref, (uint8_t)0u, (uint8_t)49u));
  }
  if (hyst_max_r13_present) {
    HANDLE_CODE(hyst_max_r13.unpack(bref));
  }
  if (hyst_min_r13_present) {
    HANDLE_CODE(hyst_min_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_disc_cfg_relay_ue_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (thresh_high_r13_present) {
    j.write_int("threshHigh-r13", thresh_high_r13);
  }
  if (thresh_low_r13_present) {
    j.write_int("threshLow-r13", thresh_low_r13);
  }
  if (hyst_max_r13_present) {
    j.write_str("hystMax-r13", hyst_max_r13.to_string());
  }
  if (hyst_min_r13_present) {
    j.write_str("hystMin-r13", hyst_min_r13.to_string());
  }
  j.end_obj();
}

const char* sl_disc_cfg_relay_ue_r13_s::hyst_max_r13_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12", "dBinf"};
  return convert_enum_idx(options, 6, value, "sl_disc_cfg_relay_ue_r13_s::hyst_max_r13_e_");
}
uint8_t sl_disc_cfg_relay_ue_r13_s::hyst_max_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_disc_cfg_relay_ue_r13_s::hyst_max_r13_e_");
}

const char* sl_disc_cfg_relay_ue_r13_s::hyst_min_r13_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 5, value, "sl_disc_cfg_relay_ue_r13_s::hyst_min_r13_e_");
}
uint8_t sl_disc_cfg_relay_ue_r13_s::hyst_min_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_disc_cfg_relay_ue_r13_s::hyst_min_r13_e_");
}

// SL-DiscConfigRemoteUE-r13 ::= SEQUENCE
SRSASN_CODE sl_disc_cfg_remote_ue_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(thresh_high_r13_present, 1));
  HANDLE_CODE(bref.pack(hyst_max_r13_present, 1));

  if (thresh_high_r13_present) {
    HANDLE_CODE(pack_integer(bref, thresh_high_r13, (uint8_t)0u, (uint8_t)49u));
  }
  if (hyst_max_r13_present) {
    HANDLE_CODE(hyst_max_r13.pack(bref));
  }
  HANDLE_CODE(resel_info_ic_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_remote_ue_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(thresh_high_r13_present, 1));
  HANDLE_CODE(bref.unpack(hyst_max_r13_present, 1));

  if (thresh_high_r13_present) {
    HANDLE_CODE(unpack_integer(thresh_high_r13, bref, (uint8_t)0u, (uint8_t)49u));
  }
  if (hyst_max_r13_present) {
    HANDLE_CODE(hyst_max_r13.unpack(bref));
  }
  HANDLE_CODE(resel_info_ic_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_disc_cfg_remote_ue_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (thresh_high_r13_present) {
    j.write_int("threshHigh-r13", thresh_high_r13);
  }
  if (hyst_max_r13_present) {
    j.write_str("hystMax-r13", hyst_max_r13.to_string());
  }
  j.write_fieldname("reselectionInfoIC-r13");
  resel_info_ic_r13.to_json(j);
  j.end_obj();
}

const char* sl_disc_cfg_remote_ue_r13_s::hyst_max_r13_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 5, value, "sl_disc_cfg_remote_ue_r13_s::hyst_max_r13_e_");
}
uint8_t sl_disc_cfg_remote_ue_r13_s::hyst_max_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_disc_cfg_remote_ue_r13_s::hyst_max_r13_e_");
}

// ServingSatelliteInfo-r17 ::= SEQUENCE
SRSASN_CODE serving_satellite_info_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(epoch_time_r17_present, 1));
  HANDLE_CODE(bref.pack(k_mac_r17_present, 1));

  HANDLE_CODE(ephemeris_info_r17.pack(bref));
  HANDLE_CODE(bref.pack(nta_common_params_minus17.nta_common_r17_present, 1));
  HANDLE_CODE(bref.pack(nta_common_params_minus17.nta_common_drift_r17_present, 1));
  HANDLE_CODE(bref.pack(nta_common_params_minus17.nta_common_drift_variation_r17_present, 1));
  if (nta_common_params_minus17.nta_common_r17_present) {
    HANDLE_CODE(pack_integer(bref, nta_common_params_minus17.nta_common_r17, (uint32_t)0u, (uint32_t)8316827u));
  }
  if (nta_common_params_minus17.nta_common_drift_r17_present) {
    HANDLE_CODE(pack_integer(bref, nta_common_params_minus17.nta_common_drift_r17, (int32_t)-261935, (int32_t)261935));
  }
  if (nta_common_params_minus17.nta_common_drift_variation_r17_present) {
    HANDLE_CODE(
        pack_integer(bref, nta_common_params_minus17.nta_common_drift_variation_r17, (uint16_t)0u, (uint16_t)29479u));
  }
  HANDLE_CODE(ul_sync_validity_dur_r17.pack(bref));
  if (epoch_time_r17_present) {
    HANDLE_CODE(pack_integer(bref, epoch_time_r17.start_sfn_r17, (uint16_t)0u, (uint16_t)1023u));
    HANDLE_CODE(pack_integer(bref, epoch_time_r17.start_sub_frame_r17, (uint8_t)0u, (uint8_t)9u));
  }
  HANDLE_CODE(pack_integer(bref, k_offset_r17, (uint16_t)0u, (uint16_t)1023u));
  if (k_mac_r17_present) {
    HANDLE_CODE(pack_integer(bref, k_mac_r17, (uint16_t)1u, (uint16_t)512u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_satellite_info_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(epoch_time_r17_present, 1));
  HANDLE_CODE(bref.unpack(k_mac_r17_present, 1));

  HANDLE_CODE(ephemeris_info_r17.unpack(bref));
  HANDLE_CODE(bref.unpack(nta_common_params_minus17.nta_common_r17_present, 1));
  HANDLE_CODE(bref.unpack(nta_common_params_minus17.nta_common_drift_r17_present, 1));
  HANDLE_CODE(bref.unpack(nta_common_params_minus17.nta_common_drift_variation_r17_present, 1));
  if (nta_common_params_minus17.nta_common_r17_present) {
    HANDLE_CODE(unpack_integer(nta_common_params_minus17.nta_common_r17, bref, (uint32_t)0u, (uint32_t)8316827u));
  }
  if (nta_common_params_minus17.nta_common_drift_r17_present) {
    HANDLE_CODE(
        unpack_integer(nta_common_params_minus17.nta_common_drift_r17, bref, (int32_t)-261935, (int32_t)261935));
  }
  if (nta_common_params_minus17.nta_common_drift_variation_r17_present) {
    HANDLE_CODE(
        unpack_integer(nta_common_params_minus17.nta_common_drift_variation_r17, bref, (uint16_t)0u, (uint16_t)29479u));
  }
  HANDLE_CODE(ul_sync_validity_dur_r17.unpack(bref));
  if (epoch_time_r17_present) {
    HANDLE_CODE(unpack_integer(epoch_time_r17.start_sfn_r17, bref, (uint16_t)0u, (uint16_t)1023u));
    HANDLE_CODE(unpack_integer(epoch_time_r17.start_sub_frame_r17, bref, (uint8_t)0u, (uint8_t)9u));
  }
  HANDLE_CODE(unpack_integer(k_offset_r17, bref, (uint16_t)0u, (uint16_t)1023u));
  if (k_mac_r17_present) {
    HANDLE_CODE(unpack_integer(k_mac_r17, bref, (uint16_t)1u, (uint16_t)512u));
  }

  return SRSASN_SUCCESS;
}
void serving_satellite_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ephemerisInfo-r17");
  ephemeris_info_r17.to_json(j);
  j.write_fieldname("nta-CommonParameters-17");
  j.start_obj();
  if (nta_common_params_minus17.nta_common_r17_present) {
    j.write_int("nta-Common-r17", nta_common_params_minus17.nta_common_r17);
  }
  if (nta_common_params_minus17.nta_common_drift_r17_present) {
    j.write_int("nta-CommonDrift-r17", nta_common_params_minus17.nta_common_drift_r17);
  }
  if (nta_common_params_minus17.nta_common_drift_variation_r17_present) {
    j.write_int("nta-CommonDriftVariation-r17", nta_common_params_minus17.nta_common_drift_variation_r17);
  }
  j.end_obj();
  j.write_str("ul-SyncValidityDuration-r17", ul_sync_validity_dur_r17.to_string());
  if (epoch_time_r17_present) {
    j.write_fieldname("epochTime-r17");
    j.start_obj();
    j.write_int("startSFN-r17", epoch_time_r17.start_sfn_r17);
    j.write_int("startSubFrame-r17", epoch_time_r17.start_sub_frame_r17);
    j.end_obj();
  }
  j.write_int("k-Offset-r17", k_offset_r17);
  if (k_mac_r17_present) {
    j.write_int("k-Mac-r17", k_mac_r17);
  }
  j.end_obj();
}

void serving_satellite_info_r17_s::ephemeris_info_r17_c_::destroy_()
{
  switch (type_) {
    case types::state_vectors:
      c.destroy<ephemeris_state_vectors_r17_s>();
      break;
    case types::orbital_params:
      c.destroy<ephemeris_orbital_params_r17_s>();
      break;
    default:
      break;
  }
}
void serving_satellite_info_r17_s::ephemeris_info_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::state_vectors:
      c.init<ephemeris_state_vectors_r17_s>();
      break;
    case types::orbital_params:
      c.init<ephemeris_orbital_params_r17_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_satellite_info_r17_s::ephemeris_info_r17_c_");
  }
}
serving_satellite_info_r17_s::ephemeris_info_r17_c_::ephemeris_info_r17_c_(
    const serving_satellite_info_r17_s::ephemeris_info_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::state_vectors:
      c.init(other.c.get<ephemeris_state_vectors_r17_s>());
      break;
    case types::orbital_params:
      c.init(other.c.get<ephemeris_orbital_params_r17_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_satellite_info_r17_s::ephemeris_info_r17_c_");
  }
}
serving_satellite_info_r17_s::ephemeris_info_r17_c_& serving_satellite_info_r17_s::ephemeris_info_r17_c_::operator=(
    const serving_satellite_info_r17_s::ephemeris_info_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::state_vectors:
      c.set(other.c.get<ephemeris_state_vectors_r17_s>());
      break;
    case types::orbital_params:
      c.set(other.c.get<ephemeris_orbital_params_r17_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_satellite_info_r17_s::ephemeris_info_r17_c_");
  }

  return *this;
}
ephemeris_state_vectors_r17_s& serving_satellite_info_r17_s::ephemeris_info_r17_c_::set_state_vectors()
{
  set(types::state_vectors);
  return c.get<ephemeris_state_vectors_r17_s>();
}
ephemeris_orbital_params_r17_s& serving_satellite_info_r17_s::ephemeris_info_r17_c_::set_orbital_params()
{
  set(types::orbital_params);
  return c.get<ephemeris_orbital_params_r17_s>();
}
void serving_satellite_info_r17_s::ephemeris_info_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::state_vectors:
      j.write_fieldname("stateVectors");
      c.get<ephemeris_state_vectors_r17_s>().to_json(j);
      break;
    case types::orbital_params:
      j.write_fieldname("orbitalParameters");
      c.get<ephemeris_orbital_params_r17_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "serving_satellite_info_r17_s::ephemeris_info_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE serving_satellite_info_r17_s::ephemeris_info_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::state_vectors:
      HANDLE_CODE(c.get<ephemeris_state_vectors_r17_s>().pack(bref));
      break;
    case types::orbital_params:
      HANDLE_CODE(c.get<ephemeris_orbital_params_r17_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "serving_satellite_info_r17_s::ephemeris_info_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_satellite_info_r17_s::ephemeris_info_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::state_vectors:
      HANDLE_CODE(c.get<ephemeris_state_vectors_r17_s>().unpack(bref));
      break;
    case types::orbital_params:
      HANDLE_CODE(c.get<ephemeris_orbital_params_r17_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "serving_satellite_info_r17_s::ephemeris_info_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* serving_satellite_info_r17_s::ul_sync_validity_dur_r17_opts::to_string() const
{
  static const char* options[] = {"s5",
                                  "s10",
                                  "s15",
                                  "s20",
                                  "s25",
                                  "s30",
                                  "s35",
                                  "s40",
                                  "s45",
                                  "s50",
                                  "s55",
                                  "s60",
                                  "s120",
                                  "s180",
                                  "s240",
                                  "s900"};
  return convert_enum_idx(options, 16, value, "serving_satellite_info_r17_s::ul_sync_validity_dur_r17_e_");
}
uint16_t serving_satellite_info_r17_s::ul_sync_validity_dur_r17_opts::to_number() const
{
  static const uint16_t options[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 120, 180, 240, 900};
  return map_enum_number(options, 16, value, "serving_satellite_info_r17_s::ul_sync_validity_dur_r17_e_");
}

// UAC-AC1-SelectAssistInfo-r15 ::= ENUMERATED
const char* uac_ac1_select_assist_info_r15_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c"};
  return convert_enum_idx(options, 3, value, "uac_ac1_select_assist_info_r15_e");
}

// UAC-AC1-SelectAssistInfo-r16 ::= ENUMERATED
const char* uac_ac1_select_assist_info_r16_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c", "notConfigured"};
  return convert_enum_idx(options, 4, value, "uac_ac1_select_assist_info_r16_e");
}

// SystemInformation-v8a0-IEs ::= SEQUENCE
SRSASN_CODE sys_info_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sys_info_v8a0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SystemInformationBlockPos-r15 ::= SEQUENCE
SRSASN_CODE sib_pos_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  HANDLE_CODE(assist_data_sib_elem_r15.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_pos_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(assist_data_sib_elem_r15.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_pos_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("assistanceDataSIB-Element-r15", assist_data_sib_elem_r15.to_string());
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SystemInformationBlockType10 ::= SEQUENCE
SRSASN_CODE sib_type10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dummy_present, 1));

  HANDLE_CODE(msg_id.pack(bref));
  HANDLE_CODE(serial_num.pack(bref));
  HANDLE_CODE(warning_type.pack(bref));
  if (dummy_present) {
    HANDLE_CODE(dummy.pack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

    if (late_non_crit_ext_present) {
      HANDLE_CODE(late_non_crit_ext.pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dummy_present, 1));

  HANDLE_CODE(msg_id.unpack(bref));
  HANDLE_CODE(serial_num.unpack(bref));
  HANDLE_CODE(warning_type.unpack(bref));
  if (dummy_present) {
    HANDLE_CODE(dummy.unpack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

    if (late_non_crit_ext_present) {
      HANDLE_CODE(late_non_crit_ext.unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("messageIdentifier", msg_id.to_string());
  j.write_str("serialNumber", serial_num.to_string());
  j.write_str("warningType", warning_type.to_string());
  if (dummy_present) {
    j.write_str("dummy", dummy.to_string());
  }
  if (ext) {
    if (late_non_crit_ext_present) {
      j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
    }
  }
  j.end_obj();
}

// SystemInformationBlockType11 ::= SEQUENCE
SRSASN_CODE sib_type11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_coding_scheme_present, 1));

  HANDLE_CODE(msg_id.pack(bref));
  HANDLE_CODE(serial_num.pack(bref));
  HANDLE_CODE(warning_msg_segment_type.pack(bref));
  HANDLE_CODE(pack_integer(bref, warning_msg_segment_num, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(warning_msg_segment.pack(bref));
  if (data_coding_scheme_present) {
    HANDLE_CODE(data_coding_scheme.pack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

    if (late_non_crit_ext_present) {
      HANDLE_CODE(late_non_crit_ext.pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_coding_scheme_present, 1));

  HANDLE_CODE(msg_id.unpack(bref));
  HANDLE_CODE(serial_num.unpack(bref));
  HANDLE_CODE(warning_msg_segment_type.unpack(bref));
  HANDLE_CODE(unpack_integer(warning_msg_segment_num, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(warning_msg_segment.unpack(bref));
  if (data_coding_scheme_present) {
    HANDLE_CODE(data_coding_scheme.unpack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

    if (late_non_crit_ext_present) {
      HANDLE_CODE(late_non_crit_ext.unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("messageIdentifier", msg_id.to_string());
  j.write_str("serialNumber", serial_num.to_string());
  j.write_str("warningMessageSegmentType", warning_msg_segment_type.to_string());
  j.write_int("warningMessageSegmentNumber", warning_msg_segment_num);
  j.write_str("warningMessageSegment", warning_msg_segment.to_string());
  if (data_coding_scheme_present) {
    j.write_str("dataCodingScheme", data_coding_scheme.to_string());
  }
  if (ext) {
    if (late_non_crit_ext_present) {
      j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
    }
  }
  j.end_obj();
}

const char* sib_type11_s::warning_msg_segment_type_opts::to_string() const
{
  static const char* options[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(options, 2, value, "sib_type11_s::warning_msg_segment_type_e_");
}

// SystemInformationBlockType12-r9 ::= SEQUENCE
SRSASN_CODE sib_type12_r9_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_coding_scheme_r9_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  HANDLE_CODE(msg_id_r9.pack(bref));
  HANDLE_CODE(serial_num_r9.pack(bref));
  HANDLE_CODE(warning_msg_segment_type_r9.pack(bref));
  HANDLE_CODE(pack_integer(bref, warning_msg_segment_num_r9, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(warning_msg_segment_r9.pack(bref));
  if (data_coding_scheme_r9_present) {
    HANDLE_CODE(data_coding_scheme_r9.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= warning_area_coordinates_segment_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(warning_area_coordinates_segment_r15_present, 1));
      if (warning_area_coordinates_segment_r15_present) {
        HANDLE_CODE(warning_area_coordinates_segment_r15.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type12_r9_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_coding_scheme_r9_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(msg_id_r9.unpack(bref));
  HANDLE_CODE(serial_num_r9.unpack(bref));
  HANDLE_CODE(warning_msg_segment_type_r9.unpack(bref));
  HANDLE_CODE(unpack_integer(warning_msg_segment_num_r9, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(warning_msg_segment_r9.unpack(bref));
  if (data_coding_scheme_r9_present) {
    HANDLE_CODE(data_coding_scheme_r9.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(warning_area_coordinates_segment_r15_present, 1));
      if (warning_area_coordinates_segment_r15_present) {
        HANDLE_CODE(warning_area_coordinates_segment_r15.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type12_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("messageIdentifier-r9", msg_id_r9.to_string());
  j.write_str("serialNumber-r9", serial_num_r9.to_string());
  j.write_str("warningMessageSegmentType-r9", warning_msg_segment_type_r9.to_string());
  j.write_int("warningMessageSegmentNumber-r9", warning_msg_segment_num_r9);
  j.write_str("warningMessageSegment-r9", warning_msg_segment_r9.to_string());
  if (data_coding_scheme_r9_present) {
    j.write_str("dataCodingScheme-r9", data_coding_scheme_r9.to_string());
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (warning_area_coordinates_segment_r15_present) {
      j.write_str("warningAreaCoordinatesSegment-r15", warning_area_coordinates_segment_r15.to_string());
    }
  }
  j.end_obj();
}

const char* sib_type12_r9_s::warning_msg_segment_type_r9_opts::to_string() const
{
  static const char* options[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(options, 2, value, "sib_type12_r9_s::warning_msg_segment_type_r9_e_");
}

// SystemInformationBlockType13-r9 ::= SEQUENCE
SRSASN_CODE sib_type13_r9_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, mbsfn_area_info_list_r9, 1, 8));
  HANDLE_CODE(notif_cfg_r9.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= notif_cfg_v1430.is_present();
    group_flags[1] |= mbsfn_area_info_list_r16.is_present();
    group_flags[2] |= mbsfn_area_info_list_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(notif_cfg_v1430.is_present(), 1));
      if (notif_cfg_v1430.is_present()) {
        HANDLE_CODE(notif_cfg_v1430->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mbsfn_area_info_list_r16.is_present(), 1));
      if (mbsfn_area_info_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *mbsfn_area_info_list_r16, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mbsfn_area_info_list_r17.is_present(), 1));
      if (mbsfn_area_info_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *mbsfn_area_info_list_r17, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type13_r9_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(mbsfn_area_info_list_r9, bref, 1, 8));
  HANDLE_CODE(notif_cfg_r9.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool notif_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(notif_cfg_v1430_present, 1));
      notif_cfg_v1430.set_present(notif_cfg_v1430_present);
      if (notif_cfg_v1430.is_present()) {
        HANDLE_CODE(notif_cfg_v1430->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mbsfn_area_info_list_r16_present;
      HANDLE_CODE(bref.unpack(mbsfn_area_info_list_r16_present, 1));
      mbsfn_area_info_list_r16.set_present(mbsfn_area_info_list_r16_present);
      if (mbsfn_area_info_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*mbsfn_area_info_list_r16, bref, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mbsfn_area_info_list_r17_present;
      HANDLE_CODE(bref.unpack(mbsfn_area_info_list_r17_present, 1));
      mbsfn_area_info_list_r17.set_present(mbsfn_area_info_list_r17_present);
      if (mbsfn_area_info_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*mbsfn_area_info_list_r17, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type13_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("mbsfn-AreaInfoList-r9");
  for (const auto& e1 : mbsfn_area_info_list_r9) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_fieldname("notificationConfig-r9");
  notif_cfg_r9.to_json(j);
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (notif_cfg_v1430.is_present()) {
      j.write_fieldname("notificationConfig-v1430");
      notif_cfg_v1430->to_json(j);
    }
    if (mbsfn_area_info_list_r16.is_present()) {
      j.start_array("mbsfn-AreaInfoList-r16");
      for (const auto& e1 : *mbsfn_area_info_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (mbsfn_area_info_list_r17.is_present()) {
      j.start_array("mbsfn-AreaInfoList-r17");
      for (const auto& e1 : *mbsfn_area_info_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SystemInformationBlockType14-r11 ::= SEQUENCE
SRSASN_CODE sib_type14_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(eab_param_r11_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (eab_param_r11_present) {
    HANDLE_CODE(eab_param_r11.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= eab_per_rsrp_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(eab_per_rsrp_r15_present, 1));
      if (eab_per_rsrp_r15_present) {
        HANDLE_CODE(eab_per_rsrp_r15.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type14_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(eab_param_r11_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (eab_param_r11_present) {
    HANDLE_CODE(eab_param_r11.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(eab_per_rsrp_r15_present, 1));
      if (eab_per_rsrp_r15_present) {
        HANDLE_CODE(eab_per_rsrp_r15.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type14_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eab_param_r11_present) {
    j.write_fieldname("eab-Param-r11");
    eab_param_r11.to_json(j);
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (eab_per_rsrp_r15_present) {
      j.write_str("eab-PerRSRP-r15", eab_per_rsrp_r15.to_string());
    }
  }
  j.end_obj();
}

void sib_type14_r11_s::eab_param_r11_c_::destroy_()
{
  switch (type_) {
    case types::eab_common_r11:
      c.destroy<eab_cfg_r11_s>();
      break;
    case types::eab_per_plmn_list_r11:
      c.destroy<eab_per_plmn_list_r11_l_>();
      break;
    default:
      break;
  }
}
void sib_type14_r11_s::eab_param_r11_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::eab_common_r11:
      c.init<eab_cfg_r11_s>();
      break;
    case types::eab_per_plmn_list_r11:
      c.init<eab_per_plmn_list_r11_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type14_r11_s::eab_param_r11_c_");
  }
}
sib_type14_r11_s::eab_param_r11_c_::eab_param_r11_c_(const sib_type14_r11_s::eab_param_r11_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::eab_common_r11:
      c.init(other.c.get<eab_cfg_r11_s>());
      break;
    case types::eab_per_plmn_list_r11:
      c.init(other.c.get<eab_per_plmn_list_r11_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type14_r11_s::eab_param_r11_c_");
  }
}
sib_type14_r11_s::eab_param_r11_c_&
sib_type14_r11_s::eab_param_r11_c_::operator=(const sib_type14_r11_s::eab_param_r11_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::eab_common_r11:
      c.set(other.c.get<eab_cfg_r11_s>());
      break;
    case types::eab_per_plmn_list_r11:
      c.set(other.c.get<eab_per_plmn_list_r11_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type14_r11_s::eab_param_r11_c_");
  }

  return *this;
}
eab_cfg_r11_s& sib_type14_r11_s::eab_param_r11_c_::set_eab_common_r11()
{
  set(types::eab_common_r11);
  return c.get<eab_cfg_r11_s>();
}
sib_type14_r11_s::eab_param_r11_c_::eab_per_plmn_list_r11_l_&
sib_type14_r11_s::eab_param_r11_c_::set_eab_per_plmn_list_r11()
{
  set(types::eab_per_plmn_list_r11);
  return c.get<eab_per_plmn_list_r11_l_>();
}
void sib_type14_r11_s::eab_param_r11_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::eab_common_r11:
      j.write_fieldname("eab-Common-r11");
      c.get<eab_cfg_r11_s>().to_json(j);
      break;
    case types::eab_per_plmn_list_r11:
      j.start_array("eab-PerPLMN-List-r11");
      for (const auto& e1 : c.get<eab_per_plmn_list_r11_l_>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "sib_type14_r11_s::eab_param_r11_c_");
  }
  j.end_obj();
}
SRSASN_CODE sib_type14_r11_s::eab_param_r11_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::eab_common_r11:
      HANDLE_CODE(c.get<eab_cfg_r11_s>().pack(bref));
      break;
    case types::eab_per_plmn_list_r11:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<eab_per_plmn_list_r11_l_>(), 1, 6));
      break;
    default:
      log_invalid_choice_id(type_, "sib_type14_r11_s::eab_param_r11_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type14_r11_s::eab_param_r11_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::eab_common_r11:
      HANDLE_CODE(c.get<eab_cfg_r11_s>().unpack(bref));
      break;
    case types::eab_per_plmn_list_r11:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<eab_per_plmn_list_r11_l_>(), bref, 1, 6));
      break;
    default:
      log_invalid_choice_id(type_, "sib_type14_r11_s::eab_param_r11_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sib_type14_r11_s::eab_per_rsrp_r15_opts::to_string() const
{
  static const char* options[] = {"thresh0", "thresh1", "thresh2", "thresh3"};
  return convert_enum_idx(options, 4, value, "sib_type14_r11_s::eab_per_rsrp_r15_e_");
}
uint8_t sib_type14_r11_s::eab_per_rsrp_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "sib_type14_r11_s::eab_per_rsrp_r15_e_");
}

// SystemInformationBlockType15-r11 ::= SEQUENCE
SRSASN_CODE sib_type15_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mbms_sai_intra_freq_r11_present, 1));
  HANDLE_CODE(bref.pack(mbms_sai_inter_freq_list_r11_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (mbms_sai_intra_freq_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbms_sai_intra_freq_r11, 1, 64, integer_packer<uint32_t>(0, 65535)));
  }
  if (mbms_sai_inter_freq_list_r11_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbms_sai_inter_freq_list_r11, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= mbms_sai_inter_freq_list_v1140.is_present();
    group_flags[1] |= mbms_intra_freq_carrier_type_r14.is_present();
    group_flags[1] |= mbms_inter_freq_carrier_type_list_r14.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mbms_sai_inter_freq_list_v1140.is_present(), 1));
      if (mbms_sai_inter_freq_list_v1140.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *mbms_sai_inter_freq_list_v1140, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mbms_intra_freq_carrier_type_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(mbms_inter_freq_carrier_type_list_r14.is_present(), 1));
      if (mbms_intra_freq_carrier_type_r14.is_present()) {
        HANDLE_CODE(mbms_intra_freq_carrier_type_r14->pack(bref));
      }
      if (mbms_inter_freq_carrier_type_list_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *mbms_inter_freq_carrier_type_list_r14, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type15_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(mbms_sai_intra_freq_r11_present, 1));
  HANDLE_CODE(bref.unpack(mbms_sai_inter_freq_list_r11_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (mbms_sai_intra_freq_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbms_sai_intra_freq_r11, bref, 1, 64, integer_packer<uint32_t>(0, 65535)));
  }
  if (mbms_sai_inter_freq_list_r11_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbms_sai_inter_freq_list_r11, bref, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mbms_sai_inter_freq_list_v1140_present;
      HANDLE_CODE(bref.unpack(mbms_sai_inter_freq_list_v1140_present, 1));
      mbms_sai_inter_freq_list_v1140.set_present(mbms_sai_inter_freq_list_v1140_present);
      if (mbms_sai_inter_freq_list_v1140.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*mbms_sai_inter_freq_list_v1140, bref, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mbms_intra_freq_carrier_type_r14_present;
      HANDLE_CODE(bref.unpack(mbms_intra_freq_carrier_type_r14_present, 1));
      mbms_intra_freq_carrier_type_r14.set_present(mbms_intra_freq_carrier_type_r14_present);
      bool mbms_inter_freq_carrier_type_list_r14_present;
      HANDLE_CODE(bref.unpack(mbms_inter_freq_carrier_type_list_r14_present, 1));
      mbms_inter_freq_carrier_type_list_r14.set_present(mbms_inter_freq_carrier_type_list_r14_present);
      if (mbms_intra_freq_carrier_type_r14.is_present()) {
        HANDLE_CODE(mbms_intra_freq_carrier_type_r14->unpack(bref));
      }
      if (mbms_inter_freq_carrier_type_list_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*mbms_inter_freq_carrier_type_list_r14, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type15_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbms_sai_intra_freq_r11_present) {
    j.start_array("mbms-SAI-IntraFreq-r11");
    for (const auto& e1 : mbms_sai_intra_freq_r11) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (mbms_sai_inter_freq_list_r11_present) {
    j.start_array("mbms-SAI-InterFreqList-r11");
    for (const auto& e1 : mbms_sai_inter_freq_list_r11) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (mbms_sai_inter_freq_list_v1140.is_present()) {
      j.start_array("mbms-SAI-InterFreqList-v1140");
      for (const auto& e1 : *mbms_sai_inter_freq_list_v1140) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (mbms_intra_freq_carrier_type_r14.is_present()) {
      j.write_fieldname("mbms-IntraFreqCarrierType-r14");
      mbms_intra_freq_carrier_type_r14->to_json(j);
    }
    if (mbms_inter_freq_carrier_type_list_r14.is_present()) {
      j.start_array("mbms-InterFreqCarrierTypeList-r14");
      for (const auto& e1 : *mbms_inter_freq_carrier_type_list_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SystemInformationBlockType16-r11 ::= SEQUENCE
SRSASN_CODE sib_type16_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(time_info_r11_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (time_info_r11_present) {
    HANDLE_CODE(bref.pack(time_info_r11.day_light_saving_time_r11_present, 1));
    HANDLE_CODE(bref.pack(time_info_r11.leap_seconds_r11_present, 1));
    HANDLE_CODE(bref.pack(time_info_r11.local_time_offset_r11_present, 1));
    HANDLE_CODE(pack_integer(bref, time_info_r11.time_info_utc_r11, (uint64_t)0u, (uint64_t)549755813887u));
    if (time_info_r11.day_light_saving_time_r11_present) {
      HANDLE_CODE(time_info_r11.day_light_saving_time_r11.pack(bref));
    }
    if (time_info_r11.leap_seconds_r11_present) {
      HANDLE_CODE(pack_integer(bref, time_info_r11.leap_seconds_r11, (int16_t)-127, (int16_t)128));
    }
    if (time_info_r11.local_time_offset_r11_present) {
      HANDLE_CODE(pack_integer(bref, time_info_r11.local_time_offset_r11, (int8_t)-63, (int8_t)64));
    }
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= time_ref_info_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(time_ref_info_r15.is_present(), 1));
      if (time_ref_info_r15.is_present()) {
        HANDLE_CODE(time_ref_info_r15->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type16_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(time_info_r11_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (time_info_r11_present) {
    HANDLE_CODE(bref.unpack(time_info_r11.day_light_saving_time_r11_present, 1));
    HANDLE_CODE(bref.unpack(time_info_r11.leap_seconds_r11_present, 1));
    HANDLE_CODE(bref.unpack(time_info_r11.local_time_offset_r11_present, 1));
    HANDLE_CODE(unpack_integer(time_info_r11.time_info_utc_r11, bref, (uint64_t)0u, (uint64_t)549755813887u));
    if (time_info_r11.day_light_saving_time_r11_present) {
      HANDLE_CODE(time_info_r11.day_light_saving_time_r11.unpack(bref));
    }
    if (time_info_r11.leap_seconds_r11_present) {
      HANDLE_CODE(unpack_integer(time_info_r11.leap_seconds_r11, bref, (int16_t)-127, (int16_t)128));
    }
    if (time_info_r11.local_time_offset_r11_present) {
      HANDLE_CODE(unpack_integer(time_info_r11.local_time_offset_r11, bref, (int8_t)-63, (int8_t)64));
    }
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool time_ref_info_r15_present;
      HANDLE_CODE(bref.unpack(time_ref_info_r15_present, 1));
      time_ref_info_r15.set_present(time_ref_info_r15_present);
      if (time_ref_info_r15.is_present()) {
        HANDLE_CODE(time_ref_info_r15->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type16_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (time_info_r11_present) {
    j.write_fieldname("timeInfo-r11");
    j.start_obj();
    j.write_int("timeInfoUTC-r11", time_info_r11.time_info_utc_r11);
    if (time_info_r11.day_light_saving_time_r11_present) {
      j.write_str("dayLightSavingTime-r11", time_info_r11.day_light_saving_time_r11.to_string());
    }
    if (time_info_r11.leap_seconds_r11_present) {
      j.write_int("leapSeconds-r11", time_info_r11.leap_seconds_r11);
    }
    if (time_info_r11.local_time_offset_r11_present) {
      j.write_int("localTimeOffset-r11", time_info_r11.local_time_offset_r11);
    }
    j.end_obj();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (time_ref_info_r15.is_present()) {
      j.write_fieldname("timeReferenceInfo-r15");
      time_ref_info_r15->to_json(j);
    }
  }
  j.end_obj();
}

// SystemInformationBlockType17-r12 ::= SEQUENCE
SRSASN_CODE sib_type17_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(wlan_offload_info_per_plmn_list_r12_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (wlan_offload_info_per_plmn_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, wlan_offload_info_per_plmn_list_r12, 1, 6));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type17_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(wlan_offload_info_per_plmn_list_r12_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (wlan_offload_info_per_plmn_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(wlan_offload_info_per_plmn_list_r12, bref, 1, 6));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type17_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (wlan_offload_info_per_plmn_list_r12_present) {
    j.start_array("wlan-OffloadInfoPerPLMN-List-r12");
    for (const auto& e1 : wlan_offload_info_per_plmn_list_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SystemInformationBlockType18-r12 ::= SEQUENCE
SRSASN_CODE sib_type18_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(comm_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (comm_cfg_r12_present) {
    HANDLE_CODE(bref.pack(comm_cfg_r12.comm_tx_pool_normal_common_r12_present, 1));
    HANDLE_CODE(bref.pack(comm_cfg_r12.comm_tx_pool_exceptional_r12_present, 1));
    HANDLE_CODE(bref.pack(comm_cfg_r12.comm_sync_cfg_r12_present, 1));
    HANDLE_CODE(pack_dyn_seq_of(bref, comm_cfg_r12.comm_rx_pool_r12, 1, 16));
    if (comm_cfg_r12.comm_tx_pool_normal_common_r12_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, comm_cfg_r12.comm_tx_pool_normal_common_r12, 1, 4));
    }
    if (comm_cfg_r12.comm_tx_pool_exceptional_r12_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, comm_cfg_r12.comm_tx_pool_exceptional_r12, 1, 4));
    }
    if (comm_cfg_r12.comm_sync_cfg_r12_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, comm_cfg_r12.comm_sync_cfg_r12, 1, 16));
    }
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= comm_tx_pool_normal_common_ext_r13.is_present();
    group_flags[0] |= comm_tx_res_uc_req_allowed_r13_present;
    group_flags[0] |= comm_tx_allow_relay_common_r13_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(comm_tx_pool_normal_common_ext_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(comm_tx_res_uc_req_allowed_r13_present, 1));
      HANDLE_CODE(bref.pack(comm_tx_allow_relay_common_r13_present, 1));
      if (comm_tx_pool_normal_common_ext_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *comm_tx_pool_normal_common_ext_r13, 1, 4));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type18_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(comm_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (comm_cfg_r12_present) {
    HANDLE_CODE(bref.unpack(comm_cfg_r12.comm_tx_pool_normal_common_r12_present, 1));
    HANDLE_CODE(bref.unpack(comm_cfg_r12.comm_tx_pool_exceptional_r12_present, 1));
    HANDLE_CODE(bref.unpack(comm_cfg_r12.comm_sync_cfg_r12_present, 1));
    HANDLE_CODE(unpack_dyn_seq_of(comm_cfg_r12.comm_rx_pool_r12, bref, 1, 16));
    if (comm_cfg_r12.comm_tx_pool_normal_common_r12_present) {
      HANDLE_CODE(unpack_dyn_seq_of(comm_cfg_r12.comm_tx_pool_normal_common_r12, bref, 1, 4));
    }
    if (comm_cfg_r12.comm_tx_pool_exceptional_r12_present) {
      HANDLE_CODE(unpack_dyn_seq_of(comm_cfg_r12.comm_tx_pool_exceptional_r12, bref, 1, 4));
    }
    if (comm_cfg_r12.comm_sync_cfg_r12_present) {
      HANDLE_CODE(unpack_dyn_seq_of(comm_cfg_r12.comm_sync_cfg_r12, bref, 1, 16));
    }
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool comm_tx_pool_normal_common_ext_r13_present;
      HANDLE_CODE(bref.unpack(comm_tx_pool_normal_common_ext_r13_present, 1));
      comm_tx_pool_normal_common_ext_r13.set_present(comm_tx_pool_normal_common_ext_r13_present);
      HANDLE_CODE(bref.unpack(comm_tx_res_uc_req_allowed_r13_present, 1));
      HANDLE_CODE(bref.unpack(comm_tx_allow_relay_common_r13_present, 1));
      if (comm_tx_pool_normal_common_ext_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*comm_tx_pool_normal_common_ext_r13, bref, 1, 4));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type18_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (comm_cfg_r12_present) {
    j.write_fieldname("commConfig-r12");
    j.start_obj();
    j.start_array("commRxPool-r12");
    for (const auto& e1 : comm_cfg_r12.comm_rx_pool_r12) {
      e1.to_json(j);
    }
    j.end_array();
    if (comm_cfg_r12.comm_tx_pool_normal_common_r12_present) {
      j.start_array("commTxPoolNormalCommon-r12");
      for (const auto& e1 : comm_cfg_r12.comm_tx_pool_normal_common_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (comm_cfg_r12.comm_tx_pool_exceptional_r12_present) {
      j.start_array("commTxPoolExceptional-r12");
      for (const auto& e1 : comm_cfg_r12.comm_tx_pool_exceptional_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (comm_cfg_r12.comm_sync_cfg_r12_present) {
      j.start_array("commSyncConfig-r12");
      for (const auto& e1 : comm_cfg_r12.comm_sync_cfg_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (comm_tx_pool_normal_common_ext_r13.is_present()) {
      j.start_array("commTxPoolNormalCommonExt-r13");
      for (const auto& e1 : *comm_tx_pool_normal_common_ext_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (comm_tx_res_uc_req_allowed_r13_present) {
      j.write_str("commTxResourceUC-ReqAllowed-r13", "true");
    }
    if (comm_tx_allow_relay_common_r13_present) {
      j.write_str("commTxAllowRelayCommon-r13", "true");
    }
  }
  j.end_obj();
}

// SystemInformationBlockType19-r12 ::= SEQUENCE
SRSASN_CODE sib_type19_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(disc_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(disc_inter_freq_list_r12_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (disc_cfg_r12_present) {
    HANDLE_CODE(bref.pack(disc_cfg_r12.disc_tx_pool_common_r12_present, 1));
    HANDLE_CODE(bref.pack(disc_cfg_r12.disc_tx_pwr_info_r12_present, 1));
    HANDLE_CODE(bref.pack(disc_cfg_r12.disc_sync_cfg_r12_present, 1));
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_cfg_r12.disc_rx_pool_r12, 1, 16));
    if (disc_cfg_r12.disc_tx_pool_common_r12_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, disc_cfg_r12.disc_tx_pool_common_r12, 1, 4));
    }
    if (disc_cfg_r12.disc_tx_pwr_info_r12_present) {
      HANDLE_CODE(
          pack_fixed_seq_of(bref, &(disc_cfg_r12.disc_tx_pwr_info_r12)[0], disc_cfg_r12.disc_tx_pwr_info_r12.size()));
    }
    if (disc_cfg_r12.disc_sync_cfg_r12_present) {
      HANDLE_CODE(pack_dyn_seq_of(bref, disc_cfg_r12.disc_sync_cfg_r12, 1, 16));
    }
  }
  if (disc_inter_freq_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, disc_inter_freq_list_r12, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= disc_cfg_v1310.is_present();
    group_flags[0] |= disc_cfg_relay_r13.is_present();
    group_flags[0] |= disc_cfg_ps_minus13.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(disc_cfg_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(disc_cfg_relay_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(disc_cfg_ps_minus13.is_present(), 1));
      if (disc_cfg_v1310.is_present()) {
        HANDLE_CODE(bref.pack(disc_cfg_v1310->disc_inter_freq_list_v1310_present, 1));
        HANDLE_CODE(bref.pack(disc_cfg_v1310->gap_requests_allowed_common_present, 1));
        if (disc_cfg_v1310->disc_inter_freq_list_v1310_present) {
          HANDLE_CODE(pack_dyn_seq_of(bref, disc_cfg_v1310->disc_inter_freq_list_v1310, 1, 8));
        }
      }
      if (disc_cfg_relay_r13.is_present()) {
        HANDLE_CODE(disc_cfg_relay_r13->relay_ue_cfg_r13.pack(bref));
        HANDLE_CODE(disc_cfg_relay_r13->remote_ue_cfg_r13.pack(bref));
      }
      if (disc_cfg_ps_minus13.is_present()) {
        HANDLE_CODE(bref.pack(disc_cfg_ps_minus13->disc_tx_pool_ps_common_r13_present, 1));
        HANDLE_CODE(pack_dyn_seq_of(bref, disc_cfg_ps_minus13->disc_rx_pool_ps_r13, 1, 16));
        if (disc_cfg_ps_minus13->disc_tx_pool_ps_common_r13_present) {
          HANDLE_CODE(pack_dyn_seq_of(bref, disc_cfg_ps_minus13->disc_tx_pool_ps_common_r13, 1, 4));
        }
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type19_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(disc_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(disc_inter_freq_list_r12_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (disc_cfg_r12_present) {
    HANDLE_CODE(bref.unpack(disc_cfg_r12.disc_tx_pool_common_r12_present, 1));
    HANDLE_CODE(bref.unpack(disc_cfg_r12.disc_tx_pwr_info_r12_present, 1));
    HANDLE_CODE(bref.unpack(disc_cfg_r12.disc_sync_cfg_r12_present, 1));
    HANDLE_CODE(unpack_dyn_seq_of(disc_cfg_r12.disc_rx_pool_r12, bref, 1, 16));
    if (disc_cfg_r12.disc_tx_pool_common_r12_present) {
      HANDLE_CODE(unpack_dyn_seq_of(disc_cfg_r12.disc_tx_pool_common_r12, bref, 1, 4));
    }
    if (disc_cfg_r12.disc_tx_pwr_info_r12_present) {
      HANDLE_CODE(
          unpack_fixed_seq_of(&(disc_cfg_r12.disc_tx_pwr_info_r12)[0], bref, disc_cfg_r12.disc_tx_pwr_info_r12.size()));
    }
    if (disc_cfg_r12.disc_sync_cfg_r12_present) {
      HANDLE_CODE(unpack_dyn_seq_of(disc_cfg_r12.disc_sync_cfg_r12, bref, 1, 16));
    }
  }
  if (disc_inter_freq_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(disc_inter_freq_list_r12, bref, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool disc_cfg_v1310_present;
      HANDLE_CODE(bref.unpack(disc_cfg_v1310_present, 1));
      disc_cfg_v1310.set_present(disc_cfg_v1310_present);
      bool disc_cfg_relay_r13_present;
      HANDLE_CODE(bref.unpack(disc_cfg_relay_r13_present, 1));
      disc_cfg_relay_r13.set_present(disc_cfg_relay_r13_present);
      bool disc_cfg_ps_minus13_present;
      HANDLE_CODE(bref.unpack(disc_cfg_ps_minus13_present, 1));
      disc_cfg_ps_minus13.set_present(disc_cfg_ps_minus13_present);
      if (disc_cfg_v1310.is_present()) {
        HANDLE_CODE(bref.unpack(disc_cfg_v1310->disc_inter_freq_list_v1310_present, 1));
        HANDLE_CODE(bref.unpack(disc_cfg_v1310->gap_requests_allowed_common_present, 1));
        if (disc_cfg_v1310->disc_inter_freq_list_v1310_present) {
          HANDLE_CODE(unpack_dyn_seq_of(disc_cfg_v1310->disc_inter_freq_list_v1310, bref, 1, 8));
        }
      }
      if (disc_cfg_relay_r13.is_present()) {
        HANDLE_CODE(disc_cfg_relay_r13->relay_ue_cfg_r13.unpack(bref));
        HANDLE_CODE(disc_cfg_relay_r13->remote_ue_cfg_r13.unpack(bref));
      }
      if (disc_cfg_ps_minus13.is_present()) {
        HANDLE_CODE(bref.unpack(disc_cfg_ps_minus13->disc_tx_pool_ps_common_r13_present, 1));
        HANDLE_CODE(unpack_dyn_seq_of(disc_cfg_ps_minus13->disc_rx_pool_ps_r13, bref, 1, 16));
        if (disc_cfg_ps_minus13->disc_tx_pool_ps_common_r13_present) {
          HANDLE_CODE(unpack_dyn_seq_of(disc_cfg_ps_minus13->disc_tx_pool_ps_common_r13, bref, 1, 4));
        }
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type19_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (disc_cfg_r12_present) {
    j.write_fieldname("discConfig-r12");
    j.start_obj();
    j.start_array("discRxPool-r12");
    for (const auto& e1 : disc_cfg_r12.disc_rx_pool_r12) {
      e1.to_json(j);
    }
    j.end_array();
    if (disc_cfg_r12.disc_tx_pool_common_r12_present) {
      j.start_array("discTxPoolCommon-r12");
      for (const auto& e1 : disc_cfg_r12.disc_tx_pool_common_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (disc_cfg_r12.disc_tx_pwr_info_r12_present) {
      j.start_array("discTxPowerInfo-r12");
      for (const auto& e1 : disc_cfg_r12.disc_tx_pwr_info_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (disc_cfg_r12.disc_sync_cfg_r12_present) {
      j.start_array("discSyncConfig-r12");
      for (const auto& e1 : disc_cfg_r12.disc_sync_cfg_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.end_obj();
  }
  if (disc_inter_freq_list_r12_present) {
    j.start_array("discInterFreqList-r12");
    for (const auto& e1 : disc_inter_freq_list_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (disc_cfg_v1310.is_present()) {
      j.write_fieldname("discConfig-v1310");
      j.start_obj();
      if (disc_cfg_v1310->disc_inter_freq_list_v1310_present) {
        j.start_array("discInterFreqList-v1310");
        for (const auto& e1 : disc_cfg_v1310->disc_inter_freq_list_v1310) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (disc_cfg_v1310->gap_requests_allowed_common_present) {
        j.write_str("gapRequestsAllowedCommon", "true");
      }
      j.end_obj();
    }
    if (disc_cfg_relay_r13.is_present()) {
      j.write_fieldname("discConfigRelay-r13");
      j.start_obj();
      j.write_fieldname("relayUE-Config-r13");
      disc_cfg_relay_r13->relay_ue_cfg_r13.to_json(j);
      j.write_fieldname("remoteUE-Config-r13");
      disc_cfg_relay_r13->remote_ue_cfg_r13.to_json(j);
      j.end_obj();
    }
    if (disc_cfg_ps_minus13.is_present()) {
      j.write_fieldname("discConfigPS-13");
      j.start_obj();
      j.start_array("discRxPoolPS-r13");
      for (const auto& e1 : disc_cfg_ps_minus13->disc_rx_pool_ps_r13) {
        e1.to_json(j);
      }
      j.end_array();
      if (disc_cfg_ps_minus13->disc_tx_pool_ps_common_r13_present) {
        j.start_array("discTxPoolPS-Common-r13");
        for (const auto& e1 : disc_cfg_ps_minus13->disc_tx_pool_ps_common_r13) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
    }
  }
  j.end_obj();
}

// SystemInformationBlockType20-r13 ::= SEQUENCE
SRSASN_CODE sib_type20_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sc_mcch_dur_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  HANDLE_CODE(sc_mcch_repeat_period_r13.pack(bref));
  HANDLE_CODE(pack_integer(bref, sc_mcch_offset_r13, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(pack_integer(bref, sc_mcch_first_sf_r13, (uint8_t)0u, (uint8_t)9u));
  if (sc_mcch_dur_r13_present) {
    HANDLE_CODE(pack_integer(bref, sc_mcch_dur_r13, (uint8_t)2u, (uint8_t)9u));
  }
  HANDLE_CODE(sc_mcch_mod_period_r13.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= br_bcch_cfg_r14.is_present();
    group_flags[0] |= sc_mcch_sched_info_r14.is_present();
    group_flags[0] |= pdsch_max_num_repeat_cemode_a_sc_mtch_r14_present;
    group_flags[0] |= pdsch_max_num_repeat_cemode_b_sc_mtch_r14_present;
    group_flags[1] |= sc_mcch_repeat_period_v1470_present;
    group_flags[1] |= sc_mcch_mod_period_v1470_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(br_bcch_cfg_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(sc_mcch_sched_info_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_max_num_repeat_cemode_a_sc_mtch_r14_present, 1));
      HANDLE_CODE(bref.pack(pdsch_max_num_repeat_cemode_b_sc_mtch_r14_present, 1));
      if (br_bcch_cfg_r14.is_present()) {
        HANDLE_CODE(pack_integer(bref, br_bcch_cfg_r14->mpdcch_nb_sc_mcch_r14, (uint8_t)1u, (uint8_t)16u));
        HANDLE_CODE(br_bcch_cfg_r14->mpdcch_num_repeat_sc_mcch_r14.pack(bref));
        HANDLE_CODE(br_bcch_cfg_r14->mpdcch_start_sf_sc_mcch_r14.pack(bref));
        HANDLE_CODE(br_bcch_cfg_r14->mpdcch_pdsch_hop_cfg_sc_mcch_r14.pack(bref));
        HANDLE_CODE(pack_integer(bref, br_bcch_cfg_r14->sc_mcch_carrier_freq_r14, (uint32_t)0u, (uint32_t)262143u));
        HANDLE_CODE(pack_integer(bref, br_bcch_cfg_r14->sc_mcch_offset_br_r14, (uint8_t)0u, (uint8_t)10u));
        HANDLE_CODE(br_bcch_cfg_r14->sc_mcch_repeat_period_br_r14.pack(bref));
        HANDLE_CODE(br_bcch_cfg_r14->sc_mcch_mod_period_br_r14.pack(bref));
      }
      if (sc_mcch_sched_info_r14.is_present()) {
        HANDLE_CODE(sc_mcch_sched_info_r14->pack(bref));
      }
      if (pdsch_max_num_repeat_cemode_a_sc_mtch_r14_present) {
        HANDLE_CODE(pdsch_max_num_repeat_cemode_a_sc_mtch_r14.pack(bref));
      }
      if (pdsch_max_num_repeat_cemode_b_sc_mtch_r14_present) {
        HANDLE_CODE(pdsch_max_num_repeat_cemode_b_sc_mtch_r14.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sc_mcch_repeat_period_v1470_present, 1));
      HANDLE_CODE(bref.pack(sc_mcch_mod_period_v1470_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type20_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sc_mcch_dur_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(sc_mcch_repeat_period_r13.unpack(bref));
  HANDLE_CODE(unpack_integer(sc_mcch_offset_r13, bref, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(unpack_integer(sc_mcch_first_sf_r13, bref, (uint8_t)0u, (uint8_t)9u));
  if (sc_mcch_dur_r13_present) {
    HANDLE_CODE(unpack_integer(sc_mcch_dur_r13, bref, (uint8_t)2u, (uint8_t)9u));
  }
  HANDLE_CODE(sc_mcch_mod_period_r13.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool br_bcch_cfg_r14_present;
      HANDLE_CODE(bref.unpack(br_bcch_cfg_r14_present, 1));
      br_bcch_cfg_r14.set_present(br_bcch_cfg_r14_present);
      bool sc_mcch_sched_info_r14_present;
      HANDLE_CODE(bref.unpack(sc_mcch_sched_info_r14_present, 1));
      sc_mcch_sched_info_r14.set_present(sc_mcch_sched_info_r14_present);
      HANDLE_CODE(bref.unpack(pdsch_max_num_repeat_cemode_a_sc_mtch_r14_present, 1));
      HANDLE_CODE(bref.unpack(pdsch_max_num_repeat_cemode_b_sc_mtch_r14_present, 1));
      if (br_bcch_cfg_r14.is_present()) {
        HANDLE_CODE(unpack_integer(br_bcch_cfg_r14->mpdcch_nb_sc_mcch_r14, bref, (uint8_t)1u, (uint8_t)16u));
        HANDLE_CODE(br_bcch_cfg_r14->mpdcch_num_repeat_sc_mcch_r14.unpack(bref));
        HANDLE_CODE(br_bcch_cfg_r14->mpdcch_start_sf_sc_mcch_r14.unpack(bref));
        HANDLE_CODE(br_bcch_cfg_r14->mpdcch_pdsch_hop_cfg_sc_mcch_r14.unpack(bref));
        HANDLE_CODE(unpack_integer(br_bcch_cfg_r14->sc_mcch_carrier_freq_r14, bref, (uint32_t)0u, (uint32_t)262143u));
        HANDLE_CODE(unpack_integer(br_bcch_cfg_r14->sc_mcch_offset_br_r14, bref, (uint8_t)0u, (uint8_t)10u));
        HANDLE_CODE(br_bcch_cfg_r14->sc_mcch_repeat_period_br_r14.unpack(bref));
        HANDLE_CODE(br_bcch_cfg_r14->sc_mcch_mod_period_br_r14.unpack(bref));
      }
      if (sc_mcch_sched_info_r14.is_present()) {
        HANDLE_CODE(sc_mcch_sched_info_r14->unpack(bref));
      }
      if (pdsch_max_num_repeat_cemode_a_sc_mtch_r14_present) {
        HANDLE_CODE(pdsch_max_num_repeat_cemode_a_sc_mtch_r14.unpack(bref));
      }
      if (pdsch_max_num_repeat_cemode_b_sc_mtch_r14_present) {
        HANDLE_CODE(pdsch_max_num_repeat_cemode_b_sc_mtch_r14.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sc_mcch_repeat_period_v1470_present, 1));
      HANDLE_CODE(bref.unpack(sc_mcch_mod_period_v1470_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type20_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sc-mcch-RepetitionPeriod-r13", sc_mcch_repeat_period_r13.to_string());
  j.write_int("sc-mcch-Offset-r13", sc_mcch_offset_r13);
  j.write_int("sc-mcch-FirstSubframe-r13", sc_mcch_first_sf_r13);
  if (sc_mcch_dur_r13_present) {
    j.write_int("sc-mcch-duration-r13", sc_mcch_dur_r13);
  }
  j.write_str("sc-mcch-ModificationPeriod-r13", sc_mcch_mod_period_r13.to_string());
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (br_bcch_cfg_r14.is_present()) {
      j.write_fieldname("br-BCCH-Config-r14");
      j.start_obj();
      j.write_str("dummy", "rf1");
      j.write_str("dummy2", "rf1");
      j.write_int("mpdcch-Narrowband-SC-MCCH-r14", br_bcch_cfg_r14->mpdcch_nb_sc_mcch_r14);
      j.write_str("mpdcch-NumRepetition-SC-MCCH-r14", br_bcch_cfg_r14->mpdcch_num_repeat_sc_mcch_r14.to_string());
      j.write_fieldname("mpdcch-StartSF-SC-MCCH-r14");
      br_bcch_cfg_r14->mpdcch_start_sf_sc_mcch_r14.to_json(j);
      j.write_str("mpdcch-PDSCH-HoppingConfig-SC-MCCH-r14",
                  br_bcch_cfg_r14->mpdcch_pdsch_hop_cfg_sc_mcch_r14.to_string());
      j.write_int("sc-mcch-CarrierFreq-r14", br_bcch_cfg_r14->sc_mcch_carrier_freq_r14);
      j.write_int("sc-mcch-Offset-BR-r14", br_bcch_cfg_r14->sc_mcch_offset_br_r14);
      j.write_str("sc-mcch-RepetitionPeriod-BR-r14", br_bcch_cfg_r14->sc_mcch_repeat_period_br_r14.to_string());
      j.write_str("sc-mcch-ModificationPeriod-BR-r14", br_bcch_cfg_r14->sc_mcch_mod_period_br_r14.to_string());
      j.end_obj();
    }
    if (sc_mcch_sched_info_r14.is_present()) {
      j.write_fieldname("sc-mcch-SchedulingInfo-r14");
      sc_mcch_sched_info_r14->to_json(j);
    }
    if (pdsch_max_num_repeat_cemode_a_sc_mtch_r14_present) {
      j.write_str("pdsch-maxNumRepetitionCEmodeA-SC-MTCH-r14", pdsch_max_num_repeat_cemode_a_sc_mtch_r14.to_string());
    }
    if (pdsch_max_num_repeat_cemode_b_sc_mtch_r14_present) {
      j.write_str("pdsch-maxNumRepetitionCEmodeB-SC-MTCH-r14", pdsch_max_num_repeat_cemode_b_sc_mtch_r14.to_string());
    }
    if (sc_mcch_repeat_period_v1470_present) {
      j.write_str("sc-mcch-RepetitionPeriod-v1470", "rf1");
    }
    if (sc_mcch_mod_period_v1470_present) {
      j.write_str("sc-mcch-ModificationPeriod-v1470", "rf1");
    }
  }
  j.end_obj();
}

const char* sib_type20_r13_s::sc_mcch_repeat_period_r13_opts::to_string() const
{
  static const char* options[] = {"rf2", "rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "rf256"};
  return convert_enum_idx(options, 8, value, "sib_type20_r13_s::sc_mcch_repeat_period_r13_e_");
}
uint16_t sib_type20_r13_s::sc_mcch_repeat_period_r13_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 8, value, "sib_type20_r13_s::sc_mcch_repeat_period_r13_e_");
}

const char* sib_type20_r13_s::sc_mcch_mod_period_r13_opts::to_string() const
{
  static const char* options[] = {"rf2",
                                  "rf4",
                                  "rf8",
                                  "rf16",
                                  "rf32",
                                  "rf64",
                                  "rf128",
                                  "rf256",
                                  "rf512",
                                  "rf1024",
                                  "r2048",
                                  "rf4096",
                                  "rf8192",
                                  "rf16384",
                                  "rf32768",
                                  "rf65536"};
  return convert_enum_idx(options, 16, value, "sib_type20_r13_s::sc_mcch_mod_period_r13_e_");
}
uint32_t sib_type20_r13_s::sc_mcch_mod_period_r13_opts::to_number() const
{
  static const uint32_t options[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};
  return map_enum_number(options, 16, value, "sib_type20_r13_s::sc_mcch_mod_period_r13_e_");
}

const char* sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_num_repeat_sc_mcch_r14_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "r128", "r256"};
  return convert_enum_idx(options, 9, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_num_repeat_sc_mcch_r14_e_");
}
uint16_t sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_num_repeat_sc_mcch_r14_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 9, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_num_repeat_sc_mcch_r14_e_");
}

void sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::destroy_() {}
void sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::mpdcch_start_sf_sc_mcch_r14_c_(
    const sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fdd_r14:
      c.init(other.c.get<fdd_r14_e_>());
      break;
    case types::tdd_r14:
      c.init(other.c.get<tdd_r14_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_");
  }
}
sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_&
sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::operator=(
    const sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fdd_r14:
      c.set(other.c.get<fdd_r14_e_>());
      break;
    case types::tdd_r14:
      c.set(other.c.get<tdd_r14_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_");
  }

  return *this;
}
sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_&
sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::set_fdd_r14()
{
  set(types::fdd_r14);
  return c.get<fdd_r14_e_>();
}
sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_e_&
sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::set_tdd_r14()
{
  set(types::tdd_r14);
  return c.get<tdd_r14_e_>();
}
void sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fdd_r14:
      j.write_str("fdd-r14", c.get<fdd_r14_e_>().to_string());
      break;
    case types::tdd_r14:
      j.write_str("tdd-r14", c.get<tdd_r14_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fdd_r14:
      HANDLE_CODE(c.get<fdd_r14_e_>().pack(bref));
      break;
    case types::tdd_r14:
      HANDLE_CODE(c.get<tdd_r14_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fdd_r14:
      HANDLE_CODE(c.get<fdd_r14_e_>().unpack(bref));
      break;
    case types::tdd_r14:
      HANDLE_CODE(c.get<tdd_r14_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot5", "v2", "v2dot5", "v4", "v5", "v8", "v10"};
  return convert_enum_idx(
      options, 8, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_");
}
float sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_opts::to_number() const
{
  static const float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return map_enum_number(
      options, 8, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_");
}
const char* sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.5", "2", "2.5", "4", "5", "8", "10"};
  return convert_enum_idx(
      options, 8, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_");
}

const char* sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_opts::to_string() const
{
  static const char* options[] = {"v1", "v2", "v4", "v5", "v8", "v10", "v20"};
  return convert_enum_idx(
      options, 7, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_e_");
}
uint8_t sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return map_enum_number(
      options, 7, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_e_");
}

const char* sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_pdsch_hop_cfg_sc_mcch_r14_opts::to_string() const
{
  static const char* options[] = {"off", "ce-ModeA", "ce-ModeB"};
  return convert_enum_idx(
      options, 3, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_pdsch_hop_cfg_sc_mcch_r14_e_");
}

const char* sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_repeat_period_br_r14_opts::to_string() const
{
  static const char* options[] = {"rf32", "rf128", "rf512", "rf1024", "rf2048", "rf4096", "rf8192", "rf16384"};
  return convert_enum_idx(options, 8, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_repeat_period_br_r14_e_");
}
uint16_t sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_repeat_period_br_r14_opts::to_number() const
{
  static const uint16_t options[] = {32, 128, 512, 1024, 2048, 4096, 8192, 16384};
  return map_enum_number(options, 8, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_repeat_period_br_r14_e_");
}

const char* sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_mod_period_br_r14_opts::to_string() const
{
  static const char* options[] = {"rf32",
                                  "rf128",
                                  "rf256",
                                  "rf512",
                                  "rf1024",
                                  "rf2048",
                                  "rf4096",
                                  "rf8192",
                                  "rf16384",
                                  "rf32768",
                                  "rf65536",
                                  "rf131072",
                                  "rf262144",
                                  "rf524288",
                                  "rf1048576"};
  return convert_enum_idx(options, 15, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_mod_period_br_r14_e_");
}
uint32_t sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_mod_period_br_r14_opts::to_number() const
{
  static const uint32_t options[] = {
      32, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576};
  return map_enum_number(options, 15, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_mod_period_br_r14_e_");
}

const char* sib_type20_r13_s::pdsch_max_num_repeat_cemode_a_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"r16", "r32"};
  return convert_enum_idx(options, 2, value, "sib_type20_r13_s::pdsch_max_num_repeat_cemode_a_sc_mtch_r14_e_");
}
uint8_t sib_type20_r13_s::pdsch_max_num_repeat_cemode_a_sc_mtch_r14_opts::to_number() const
{
  static const uint8_t options[] = {16, 32};
  return map_enum_number(options, 2, value, "sib_type20_r13_s::pdsch_max_num_repeat_cemode_a_sc_mtch_r14_e_");
}

const char* sib_type20_r13_s::pdsch_max_num_repeat_cemode_b_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"r192", "r256", "r384", "r512", "r768", "r1024", "r1536", "r2048"};
  return convert_enum_idx(options, 8, value, "sib_type20_r13_s::pdsch_max_num_repeat_cemode_b_sc_mtch_r14_e_");
}
uint16_t sib_type20_r13_s::pdsch_max_num_repeat_cemode_b_sc_mtch_r14_opts::to_number() const
{
  static const uint16_t options[] = {192, 256, 384, 512, 768, 1024, 1536, 2048};
  return map_enum_number(options, 8, value, "sib_type20_r13_s::pdsch_max_num_repeat_cemode_b_sc_mtch_r14_e_");
}

// SystemInformationBlockType21-r14 ::= SEQUENCE
SRSASN_CODE sib_type21_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_v2x_cfg_common_r14_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (sl_v2x_cfg_common_r14_present) {
    HANDLE_CODE(sl_v2x_cfg_common_r14.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= anchor_carrier_freq_list_nr_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(anchor_carrier_freq_list_nr_r16.is_present(), 1));
      if (anchor_carrier_freq_list_nr_r16.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *anchor_carrier_freq_list_nr_r16, 1, 8, integer_packer<uint32_t>(0, 3279165)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type21_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_v2x_cfg_common_r14_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (sl_v2x_cfg_common_r14_present) {
    HANDLE_CODE(sl_v2x_cfg_common_r14.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool anchor_carrier_freq_list_nr_r16_present;
      HANDLE_CODE(bref.unpack(anchor_carrier_freq_list_nr_r16_present, 1));
      anchor_carrier_freq_list_nr_r16.set_present(anchor_carrier_freq_list_nr_r16_present);
      if (anchor_carrier_freq_list_nr_r16.is_present()) {
        HANDLE_CODE(
            unpack_dyn_seq_of(*anchor_carrier_freq_list_nr_r16, bref, 1, 8, integer_packer<uint32_t>(0, 3279165)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type21_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_v2x_cfg_common_r14_present) {
    j.write_fieldname("sl-V2X-ConfigCommon-r14");
    sl_v2x_cfg_common_r14.to_json(j);
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (anchor_carrier_freq_list_nr_r16.is_present()) {
      j.start_array("anchorCarrierFreqListNR-r16");
      for (const auto& e1 : *anchor_carrier_freq_list_nr_r16) {
        j.write_int(e1);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SystemInformationBlockType24-r15 ::= SEQUENCE
SRSASN_CODE sib_type24_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(carrier_freq_list_nr_r15_present, 1));
  HANDLE_CODE(bref.pack(t_resel_nr_sf_r15_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (carrier_freq_list_nr_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, carrier_freq_list_nr_r15, 1, 8));
  }
  HANDLE_CODE(pack_integer(bref, t_resel_nr_r15, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_nr_sf_r15_present) {
    HANDLE_CODE(t_resel_nr_sf_r15.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= carrier_freq_list_nr_v1610.is_present();
    group_flags[1] |= carrier_freq_list_nr_v1700.is_present();
    group_flags[2] |= carrier_freq_list_nr_v1720.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(carrier_freq_list_nr_v1610.is_present(), 1));
      if (carrier_freq_list_nr_v1610.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *carrier_freq_list_nr_v1610, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(carrier_freq_list_nr_v1700.is_present(), 1));
      if (carrier_freq_list_nr_v1700.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *carrier_freq_list_nr_v1700, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(carrier_freq_list_nr_v1720.is_present(), 1));
      if (carrier_freq_list_nr_v1720.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *carrier_freq_list_nr_v1720, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type24_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(carrier_freq_list_nr_r15_present, 1));
  HANDLE_CODE(bref.unpack(t_resel_nr_sf_r15_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (carrier_freq_list_nr_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(carrier_freq_list_nr_r15, bref, 1, 8));
  }
  HANDLE_CODE(unpack_integer(t_resel_nr_r15, bref, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_nr_sf_r15_present) {
    HANDLE_CODE(t_resel_nr_sf_r15.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool carrier_freq_list_nr_v1610_present;
      HANDLE_CODE(bref.unpack(carrier_freq_list_nr_v1610_present, 1));
      carrier_freq_list_nr_v1610.set_present(carrier_freq_list_nr_v1610_present);
      if (carrier_freq_list_nr_v1610.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*carrier_freq_list_nr_v1610, bref, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool carrier_freq_list_nr_v1700_present;
      HANDLE_CODE(bref.unpack(carrier_freq_list_nr_v1700_present, 1));
      carrier_freq_list_nr_v1700.set_present(carrier_freq_list_nr_v1700_present);
      if (carrier_freq_list_nr_v1700.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*carrier_freq_list_nr_v1700, bref, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool carrier_freq_list_nr_v1720_present;
      HANDLE_CODE(bref.unpack(carrier_freq_list_nr_v1720_present, 1));
      carrier_freq_list_nr_v1720.set_present(carrier_freq_list_nr_v1720_present);
      if (carrier_freq_list_nr_v1720.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*carrier_freq_list_nr_v1720, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type24_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_list_nr_r15_present) {
    j.start_array("carrierFreqListNR-r15");
    for (const auto& e1 : carrier_freq_list_nr_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("t-ReselectionNR-r15", t_resel_nr_r15);
  if (t_resel_nr_sf_r15_present) {
    j.write_fieldname("t-ReselectionNR-SF-r15");
    t_resel_nr_sf_r15.to_json(j);
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (carrier_freq_list_nr_v1610.is_present()) {
      j.start_array("carrierFreqListNR-v1610");
      for (const auto& e1 : *carrier_freq_list_nr_v1610) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (carrier_freq_list_nr_v1700.is_present()) {
      j.start_array("carrierFreqListNR-v1700");
      for (const auto& e1 : *carrier_freq_list_nr_v1700) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (carrier_freq_list_nr_v1720.is_present()) {
      j.start_array("carrierFreqListNR-v1720");
      for (const auto& e1 : *carrier_freq_list_nr_v1720) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SystemInformationBlockType25-r15 ::= SEQUENCE
SRSASN_CODE sib_type25_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(uac_barr_for_common_r15_present, 1));
  HANDLE_CODE(bref.pack(uac_barr_per_plmn_list_r15_present, 1));
  HANDLE_CODE(bref.pack(uac_ac1_select_assist_info_r15_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (uac_barr_for_common_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_for_common_r15, 1, 63));
  }
  if (uac_barr_per_plmn_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_per_plmn_list_r15, 1, 6));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_info_set_list_r15, 1, 8));
  if (uac_ac1_select_assist_info_r15_present) {
    HANDLE_CODE(uac_ac1_select_assist_info_r15.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ab_per_rsrp_r16_present;
    group_flags[1] |= uac_ac1_select_assist_info_r16.is_present();
    group_flags[2] |= uac_barr_info_set_list_v1700.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ab_per_rsrp_r16_present, 1));
      if (ab_per_rsrp_r16_present) {
        HANDLE_CODE(ab_per_rsrp_r16.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(uac_ac1_select_assist_info_r16.is_present(), 1));
      if (uac_ac1_select_assist_info_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *uac_ac1_select_assist_info_r16, 2, 6));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(uac_barr_info_set_list_v1700.is_present(), 1));
      if (uac_barr_info_set_list_v1700.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *uac_barr_info_set_list_v1700, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type25_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(uac_barr_for_common_r15_present, 1));
  HANDLE_CODE(bref.unpack(uac_barr_per_plmn_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(uac_ac1_select_assist_info_r15_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (uac_barr_for_common_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(uac_barr_for_common_r15, bref, 1, 63));
  }
  if (uac_barr_per_plmn_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(uac_barr_per_plmn_list_r15, bref, 1, 6));
  }
  HANDLE_CODE(unpack_dyn_seq_of(uac_barr_info_set_list_r15, bref, 1, 8));
  if (uac_ac1_select_assist_info_r15_present) {
    HANDLE_CODE(uac_ac1_select_assist_info_r15.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ab_per_rsrp_r16_present, 1));
      if (ab_per_rsrp_r16_present) {
        HANDLE_CODE(ab_per_rsrp_r16.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool uac_ac1_select_assist_info_r16_present;
      HANDLE_CODE(bref.unpack(uac_ac1_select_assist_info_r16_present, 1));
      uac_ac1_select_assist_info_r16.set_present(uac_ac1_select_assist_info_r16_present);
      if (uac_ac1_select_assist_info_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*uac_ac1_select_assist_info_r16, bref, 2, 6));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool uac_barr_info_set_list_v1700_present;
      HANDLE_CODE(bref.unpack(uac_barr_info_set_list_v1700_present, 1));
      uac_barr_info_set_list_v1700.set_present(uac_barr_info_set_list_v1700_present);
      if (uac_barr_info_set_list_v1700.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*uac_barr_info_set_list_v1700, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type25_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (uac_barr_for_common_r15_present) {
    j.start_array("uac-BarringForCommon-r15");
    for (const auto& e1 : uac_barr_for_common_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (uac_barr_per_plmn_list_r15_present) {
    j.start_array("uac-BarringPerPLMN-List-r15");
    for (const auto& e1 : uac_barr_per_plmn_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.start_array("uac-BarringInfoSetList-r15");
  for (const auto& e1 : uac_barr_info_set_list_r15) {
    e1.to_json(j);
  }
  j.end_array();
  if (uac_ac1_select_assist_info_r15_present) {
    j.write_fieldname("uac-AC1-SelectAssistInfo-r15");
    uac_ac1_select_assist_info_r15.to_json(j);
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (ab_per_rsrp_r16_present) {
      j.write_str("ab-PerRSRP-r16", ab_per_rsrp_r16.to_string());
    }
    if (uac_ac1_select_assist_info_r16.is_present()) {
      j.start_array("uac-AC1-SelectAssistInfo-r16");
      for (const auto& e1 : *uac_ac1_select_assist_info_r16) {
        j.write_str(e1.to_string());
      }
      j.end_array();
    }
    if (uac_barr_info_set_list_v1700.is_present()) {
      j.start_array("uac-BarringInfoSetList-v1700");
      for (const auto& e1 : *uac_barr_info_set_list_v1700) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

void sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::destroy_()
{
  switch (type_) {
    case types::individual_plmn_list_r15:
      c.destroy<individual_plmn_list_r15_l_>();
      break;
    default:
      break;
  }
}
void sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::plmn_common_r15:
      break;
    case types::individual_plmn_list_r15:
      c.init<individual_plmn_list_r15_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_");
  }
}
sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::uac_ac1_select_assist_info_r15_c_(
    const sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::plmn_common_r15:
      c.init(other.c.get<uac_ac1_select_assist_info_r15_e>());
      break;
    case types::individual_plmn_list_r15:
      c.init(other.c.get<individual_plmn_list_r15_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_");
  }
}
sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_& sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::operator=(
    const sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::plmn_common_r15:
      c.set(other.c.get<uac_ac1_select_assist_info_r15_e>());
      break;
    case types::individual_plmn_list_r15:
      c.set(other.c.get<individual_plmn_list_r15_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_");
  }

  return *this;
}
uac_ac1_select_assist_info_r15_e& sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::set_plmn_common_r15()
{
  set(types::plmn_common_r15);
  return c.get<uac_ac1_select_assist_info_r15_e>();
}
sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::individual_plmn_list_r15_l_&
sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::set_individual_plmn_list_r15()
{
  set(types::individual_plmn_list_r15);
  return c.get<individual_plmn_list_r15_l_>();
}
void sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::plmn_common_r15:
      j.write_str("plmnCommon-r15", c.get<uac_ac1_select_assist_info_r15_e>().to_string());
      break;
    case types::individual_plmn_list_r15:
      j.start_array("individualPLMNList-r15");
      for (const auto& e1 : c.get<individual_plmn_list_r15_l_>()) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::plmn_common_r15:
      HANDLE_CODE(c.get<uac_ac1_select_assist_info_r15_e>().pack(bref));
      break;
    case types::individual_plmn_list_r15:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<individual_plmn_list_r15_l_>(), 2, 6));
      break;
    default:
      log_invalid_choice_id(type_, "sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::plmn_common_r15:
      HANDLE_CODE(c.get<uac_ac1_select_assist_info_r15_e>().unpack(bref));
      break;
    case types::individual_plmn_list_r15:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<individual_plmn_list_r15_l_>(), bref, 2, 6));
      break;
    default:
      log_invalid_choice_id(type_, "sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sib_type25_r15_s::ab_per_rsrp_r16_opts::to_string() const
{
  static const char* options[] = {"thresh0", "thresh1", "thresh2", "thresh3"};
  return convert_enum_idx(options, 4, value, "sib_type25_r15_s::ab_per_rsrp_r16_e_");
}
uint8_t sib_type25_r15_s::ab_per_rsrp_r16_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "sib_type25_r15_s::ab_per_rsrp_r16_e_");
}

// SystemInformationBlockType26-r15 ::= SEQUENCE
SRSASN_CODE sib_type26_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(v2x_inter_freq_info_list_r15_present, 1));
  HANDLE_CODE(bref.pack(cbr_pssch_tx_cfg_list_r15_present, 1));
  HANDLE_CODE(bref.pack(v2x_packet_dupl_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(sync_freq_list_r15_present, 1));
  HANDLE_CODE(bref.pack(slss_tx_multi_freq_r15_present, 1));
  HANDLE_CODE(bref.pack(v2x_freq_sel_cfg_list_r15_present, 1));
  HANDLE_CODE(bref.pack(thresh_s_rssi_cbr_r15_present, 1));

  if (v2x_inter_freq_info_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_inter_freq_info_list_r15, 0, 7));
  }
  if (cbr_pssch_tx_cfg_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cbr_pssch_tx_cfg_list_r15, 1, 8));
  }
  if (v2x_packet_dupl_cfg_r15_present) {
    HANDLE_CODE(v2x_packet_dupl_cfg_r15.pack(bref));
  }
  if (sync_freq_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sync_freq_list_r15, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }
  if (v2x_freq_sel_cfg_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, v2x_freq_sel_cfg_list_r15, 1, 8));
  }
  if (thresh_s_rssi_cbr_r15_present) {
    HANDLE_CODE(pack_integer(bref, thresh_s_rssi_cbr_r15, (uint8_t)0u, (uint8_t)45u));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

    if (late_non_crit_ext_present) {
      HANDLE_CODE(late_non_crit_ext.pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type26_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(v2x_inter_freq_info_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(cbr_pssch_tx_cfg_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(v2x_packet_dupl_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(sync_freq_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(slss_tx_multi_freq_r15_present, 1));
  HANDLE_CODE(bref.unpack(v2x_freq_sel_cfg_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(thresh_s_rssi_cbr_r15_present, 1));

  if (v2x_inter_freq_info_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_inter_freq_info_list_r15, bref, 0, 7));
  }
  if (cbr_pssch_tx_cfg_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cbr_pssch_tx_cfg_list_r15, bref, 1, 8));
  }
  if (v2x_packet_dupl_cfg_r15_present) {
    HANDLE_CODE(v2x_packet_dupl_cfg_r15.unpack(bref));
  }
  if (sync_freq_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sync_freq_list_r15, bref, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }
  if (v2x_freq_sel_cfg_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(v2x_freq_sel_cfg_list_r15, bref, 1, 8));
  }
  if (thresh_s_rssi_cbr_r15_present) {
    HANDLE_CODE(unpack_integer(thresh_s_rssi_cbr_r15, bref, (uint8_t)0u, (uint8_t)45u));
  }

  if (ext) {
    HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

    if (late_non_crit_ext_present) {
      HANDLE_CODE(late_non_crit_ext.unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type26_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (v2x_inter_freq_info_list_r15_present) {
    j.start_array("v2x-InterFreqInfoList-r15");
    for (const auto& e1 : v2x_inter_freq_info_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cbr_pssch_tx_cfg_list_r15_present) {
    j.start_array("cbr-pssch-TxConfigList-r15");
    for (const auto& e1 : cbr_pssch_tx_cfg_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (v2x_packet_dupl_cfg_r15_present) {
    j.write_fieldname("v2x-PacketDuplicationConfig-r15");
    v2x_packet_dupl_cfg_r15.to_json(j);
  }
  if (sync_freq_list_r15_present) {
    j.start_array("syncFreqList-r15");
    for (const auto& e1 : sync_freq_list_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (slss_tx_multi_freq_r15_present) {
    j.write_str("slss-TxMultiFreq-r15", "true");
  }
  if (v2x_freq_sel_cfg_list_r15_present) {
    j.start_array("v2x-FreqSelectionConfigList-r15");
    for (const auto& e1 : v2x_freq_sel_cfg_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (thresh_s_rssi_cbr_r15_present) {
    j.write_int("threshS-RSSI-CBR-r15", thresh_s_rssi_cbr_r15);
  }
  if (ext) {
    if (late_non_crit_ext_present) {
      j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
    }
  }
  j.end_obj();
}

// SystemInformationBlockType26a-r16 ::= SEQUENCE
SRSASN_CODE sib_type26a_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_info_list_r16, 0, 6));
  HANDLE_CODE(pack_dyn_seq_of(bref, band_list_endc_r16, 1, 10, integer_packer<uint16_t>(1, 1024)));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type26a_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(plmn_info_list_r16, bref, 0, 6));
  HANDLE_CODE(unpack_dyn_seq_of(band_list_endc_r16, bref, 1, 10, integer_packer<uint16_t>(1, 1024)));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type26a_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmn-InfoList-r16");
  for (const auto& e1 : plmn_info_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("bandListENDC-r16");
  for (const auto& e1 : band_list_endc_r16) {
    j.write_int(e1);
  }
  j.end_array();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SystemInformationBlockType27-r16 ::= SEQUENCE
SRSASN_CODE sib_type27_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(carrier_freq_list_nbiot_r16_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (carrier_freq_list_nbiot_r16_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, carrier_freq_list_nbiot_r16, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type27_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(carrier_freq_list_nbiot_r16_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (carrier_freq_list_nbiot_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(carrier_freq_list_nbiot_r16, bref, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type27_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_list_nbiot_r16_present) {
    j.start_array("carrierFreqListNBIOT-r16");
    for (const auto& e1 : carrier_freq_list_nbiot_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SystemInformationBlockType28-r16 ::= SEQUENCE
SRSASN_CODE sib_type28_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  HANDLE_CODE(pack_integer(bref, segment_num_r16, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(segment_type_r16.pack(bref));
  HANDLE_CODE(segment_container_r16.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type28_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(unpack_integer(segment_num_r16, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(segment_type_r16.unpack(bref));
  HANDLE_CODE(segment_container_r16.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type28_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("segmentNumber-r16", segment_num_r16);
  j.write_str("segmentType-r16", segment_type_r16.to_string());
  j.write_str("segmentContainer-r16", segment_container_r16.to_string());
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

const char* sib_type28_r16_s::segment_type_r16_opts::to_string() const
{
  static const char* options[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(options, 2, value, "sib_type28_r16_s::segment_type_r16_e_");
}

// SystemInformationBlockType29-r16 ::= SEQUENCE
SRSASN_CODE sib_type29_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(res_reserv_cfg_common_dl_r16_present, 1));
  HANDLE_CODE(bref.pack(res_reserv_cfg_common_ul_r16_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (res_reserv_cfg_common_dl_r16_present) {
    HANDLE_CODE(res_reserv_cfg_common_dl_r16.pack(bref));
  }
  if (res_reserv_cfg_common_ul_r16_present) {
    HANDLE_CODE(res_reserv_cfg_common_ul_r16.pack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type29_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(res_reserv_cfg_common_dl_r16_present, 1));
  HANDLE_CODE(bref.unpack(res_reserv_cfg_common_ul_r16_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (res_reserv_cfg_common_dl_r16_present) {
    HANDLE_CODE(res_reserv_cfg_common_dl_r16.unpack(bref));
  }
  if (res_reserv_cfg_common_ul_r16_present) {
    HANDLE_CODE(res_reserv_cfg_common_ul_r16.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type29_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (res_reserv_cfg_common_dl_r16_present) {
    j.write_fieldname("resourceReservationConfigCommonDL-r16");
    res_reserv_cfg_common_dl_r16.to_json(j);
  }
  if (res_reserv_cfg_common_ul_r16_present) {
    j.write_fieldname("resourceReservationConfigCommonUL-r16");
    res_reserv_cfg_common_ul_r16.to_json(j);
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SystemInformationBlockType3 ::= SEQUENCE
SRSASN_CODE sib_type3_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_resel_info_common.speed_state_resel_pars_present, 1));
  HANDLE_CODE(cell_resel_info_common.q_hyst.pack(bref));
  if (cell_resel_info_common.speed_state_resel_pars_present) {
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.mob_state_params.pack(bref));
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_medium.pack(bref));
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_high.pack(bref));
  }
  HANDLE_CODE(bref.pack(cell_resel_serving_freq_info.s_non_intra_search_present, 1));
  if (cell_resel_serving_freq_info.s_non_intra_search_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.s_non_intra_search, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.thresh_serving_low, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(bref.pack(intra_freq_cell_resel_info.p_max_present, 1));
  HANDLE_CODE(bref.pack(intra_freq_cell_resel_info.s_intra_search_present, 1));
  HANDLE_CODE(bref.pack(intra_freq_cell_resel_info.allowed_meas_bw_present, 1));
  HANDLE_CODE(bref.pack(intra_freq_cell_resel_info.t_resel_eutra_sf_present, 1));
  HANDLE_CODE(pack_integer(bref, intra_freq_cell_resel_info.q_rx_lev_min, (int8_t)-70, (int8_t)-22));
  if (intra_freq_cell_resel_info.p_max_present) {
    HANDLE_CODE(pack_integer(bref, intra_freq_cell_resel_info.p_max, (int8_t)-30, (int8_t)33));
  }
  if (intra_freq_cell_resel_info.s_intra_search_present) {
    HANDLE_CODE(pack_integer(bref, intra_freq_cell_resel_info.s_intra_search, (uint8_t)0u, (uint8_t)31u));
  }
  if (intra_freq_cell_resel_info.allowed_meas_bw_present) {
    HANDLE_CODE(intra_freq_cell_resel_info.allowed_meas_bw.pack(bref));
  }
  HANDLE_CODE(bref.pack(intra_freq_cell_resel_info.presence_ant_port1, 1));
  HANDLE_CODE(intra_freq_cell_resel_info.neigh_cell_cfg.pack(bref));
  HANDLE_CODE(pack_integer(bref, intra_freq_cell_resel_info.t_resel_eutra, (uint8_t)0u, (uint8_t)7u));
  if (intra_freq_cell_resel_info.t_resel_eutra_sf_present) {
    HANDLE_CODE(intra_freq_cell_resel_info.t_resel_eutra_sf.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= late_non_crit_ext_present;
    group_flags[1] |= s_intra_search_v920.is_present();
    group_flags[1] |= s_non_intra_search_v920.is_present();
    group_flags[1] |= q_qual_min_r9_present;
    group_flags[1] |= thresh_serving_low_q_r9_present;
    group_flags[2] |= q_qual_min_wb_r11_present;
    group_flags[3] |= q_qual_min_rsrq_on_all_symbols_r12_present;
    group_flags[4] |= cell_resel_serving_freq_info_v1310.is_present();
    group_flags[4] |= redist_serving_info_r13.is_present();
    group_flags[4] |= cell_sel_info_ce_r13.is_present();
    group_flags[4] |= t_resel_eutra_ce_r13_present;
    group_flags[5] |= cell_sel_info_ce1_r13.is_present();
    group_flags[6] |= cell_sel_info_ce1_v1360.is_present();
    group_flags[7] |= cell_resel_info_common_v1460.is_present();
    group_flags[8] |= cell_resel_info_hsdn_r15.is_present();
    group_flags[8] |= cell_sel_info_ce_v1530.is_present();
    group_flags[8] |= crs_intf_mitig_neigh_cells_ce_r15_present;
    group_flags[9] |= cell_resel_serving_freq_info_v1610.is_present();
    group_flags[10] |= t_service_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(s_intra_search_v920.is_present(), 1));
      HANDLE_CODE(bref.pack(s_non_intra_search_v920.is_present(), 1));
      HANDLE_CODE(bref.pack(q_qual_min_r9_present, 1));
      HANDLE_CODE(bref.pack(thresh_serving_low_q_r9_present, 1));
      if (s_intra_search_v920.is_present()) {
        HANDLE_CODE(pack_integer(bref, s_intra_search_v920->s_intra_search_p_r9, (uint8_t)0u, (uint8_t)31u));
        HANDLE_CODE(pack_integer(bref, s_intra_search_v920->s_intra_search_q_r9, (uint8_t)0u, (uint8_t)31u));
      }
      if (s_non_intra_search_v920.is_present()) {
        HANDLE_CODE(pack_integer(bref, s_non_intra_search_v920->s_non_intra_search_p_r9, (uint8_t)0u, (uint8_t)31u));
        HANDLE_CODE(pack_integer(bref, s_non_intra_search_v920->s_non_intra_search_q_r9, (uint8_t)0u, (uint8_t)31u));
      }
      if (q_qual_min_r9_present) {
        HANDLE_CODE(pack_integer(bref, q_qual_min_r9, (int8_t)-34, (int8_t)-3));
      }
      if (thresh_serving_low_q_r9_present) {
        HANDLE_CODE(pack_integer(bref, thresh_serving_low_q_r9, (uint8_t)0u, (uint8_t)31u));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(q_qual_min_wb_r11_present, 1));
      if (q_qual_min_wb_r11_present) {
        HANDLE_CODE(pack_integer(bref, q_qual_min_wb_r11, (int8_t)-34, (int8_t)-3));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(q_qual_min_rsrq_on_all_symbols_r12_present, 1));
      if (q_qual_min_rsrq_on_all_symbols_r12_present) {
        HANDLE_CODE(pack_integer(bref, q_qual_min_rsrq_on_all_symbols_r12, (int8_t)-34, (int8_t)-3));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cell_resel_serving_freq_info_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(redist_serving_info_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(cell_sel_info_ce_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(t_resel_eutra_ce_r13_present, 1));
      if (cell_resel_serving_freq_info_v1310.is_present()) {
        HANDLE_CODE(cell_resel_serving_freq_info_v1310->pack(bref));
      }
      if (redist_serving_info_r13.is_present()) {
        HANDLE_CODE(redist_serving_info_r13->pack(bref));
      }
      if (cell_sel_info_ce_r13.is_present()) {
        HANDLE_CODE(cell_sel_info_ce_r13->pack(bref));
      }
      if (t_resel_eutra_ce_r13_present) {
        HANDLE_CODE(pack_integer(bref, t_resel_eutra_ce_r13, (uint8_t)0u, (uint8_t)15u));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cell_sel_info_ce1_r13.is_present(), 1));
      if (cell_sel_info_ce1_r13.is_present()) {
        HANDLE_CODE(cell_sel_info_ce1_r13->pack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cell_sel_info_ce1_v1360.is_present(), 1));
      if (cell_sel_info_ce1_v1360.is_present()) {
        HANDLE_CODE(cell_sel_info_ce1_v1360->pack(bref));
      }
    }
    if (group_flags[7]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cell_resel_info_common_v1460.is_present(), 1));
      if (cell_resel_info_common_v1460.is_present()) {
        HANDLE_CODE(cell_resel_info_common_v1460->pack(bref));
      }
    }
    if (group_flags[8]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cell_resel_info_hsdn_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(cell_sel_info_ce_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(crs_intf_mitig_neigh_cells_ce_r15_present, 1));
      if (cell_resel_info_hsdn_r15.is_present()) {
        HANDLE_CODE(cell_resel_info_hsdn_r15->pack(bref));
      }
      if (cell_sel_info_ce_v1530.is_present()) {
        HANDLE_CODE(cell_sel_info_ce_v1530->pack(bref));
      }
    }
    if (group_flags[9]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cell_resel_serving_freq_info_v1610.is_present(), 1));
      if (cell_resel_serving_freq_info_v1610.is_present()) {
        HANDLE_CODE(cell_resel_serving_freq_info_v1610->pack(bref));
      }
    }
    if (group_flags[10]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(t_service_r17_present, 1));
      if (t_service_r17_present) {
        HANDLE_CODE(pack_integer(bref, t_service_r17, (uint32_t)0u, (uint32_t)1048575u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type3_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_resel_info_common.speed_state_resel_pars_present, 1));
  HANDLE_CODE(cell_resel_info_common.q_hyst.unpack(bref));
  if (cell_resel_info_common.speed_state_resel_pars_present) {
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.mob_state_params.unpack(bref));
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_medium.unpack(bref));
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_high.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(cell_resel_serving_freq_info.s_non_intra_search_present, 1));
  if (cell_resel_serving_freq_info.s_non_intra_search_present) {
    HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.s_non_intra_search, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.thresh_serving_low, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(bref.unpack(intra_freq_cell_resel_info.p_max_present, 1));
  HANDLE_CODE(bref.unpack(intra_freq_cell_resel_info.s_intra_search_present, 1));
  HANDLE_CODE(bref.unpack(intra_freq_cell_resel_info.allowed_meas_bw_present, 1));
  HANDLE_CODE(bref.unpack(intra_freq_cell_resel_info.t_resel_eutra_sf_present, 1));
  HANDLE_CODE(unpack_integer(intra_freq_cell_resel_info.q_rx_lev_min, bref, (int8_t)-70, (int8_t)-22));
  if (intra_freq_cell_resel_info.p_max_present) {
    HANDLE_CODE(unpack_integer(intra_freq_cell_resel_info.p_max, bref, (int8_t)-30, (int8_t)33));
  }
  if (intra_freq_cell_resel_info.s_intra_search_present) {
    HANDLE_CODE(unpack_integer(intra_freq_cell_resel_info.s_intra_search, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (intra_freq_cell_resel_info.allowed_meas_bw_present) {
    HANDLE_CODE(intra_freq_cell_resel_info.allowed_meas_bw.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(intra_freq_cell_resel_info.presence_ant_port1, 1));
  HANDLE_CODE(intra_freq_cell_resel_info.neigh_cell_cfg.unpack(bref));
  HANDLE_CODE(unpack_integer(intra_freq_cell_resel_info.t_resel_eutra, bref, (uint8_t)0u, (uint8_t)7u));
  if (intra_freq_cell_resel_info.t_resel_eutra_sf_present) {
    HANDLE_CODE(intra_freq_cell_resel_info.t_resel_eutra_sf.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(11);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool s_intra_search_v920_present;
      HANDLE_CODE(bref.unpack(s_intra_search_v920_present, 1));
      s_intra_search_v920.set_present(s_intra_search_v920_present);
      bool s_non_intra_search_v920_present;
      HANDLE_CODE(bref.unpack(s_non_intra_search_v920_present, 1));
      s_non_intra_search_v920.set_present(s_non_intra_search_v920_present);
      HANDLE_CODE(bref.unpack(q_qual_min_r9_present, 1));
      HANDLE_CODE(bref.unpack(thresh_serving_low_q_r9_present, 1));
      if (s_intra_search_v920.is_present()) {
        HANDLE_CODE(unpack_integer(s_intra_search_v920->s_intra_search_p_r9, bref, (uint8_t)0u, (uint8_t)31u));
        HANDLE_CODE(unpack_integer(s_intra_search_v920->s_intra_search_q_r9, bref, (uint8_t)0u, (uint8_t)31u));
      }
      if (s_non_intra_search_v920.is_present()) {
        HANDLE_CODE(unpack_integer(s_non_intra_search_v920->s_non_intra_search_p_r9, bref, (uint8_t)0u, (uint8_t)31u));
        HANDLE_CODE(unpack_integer(s_non_intra_search_v920->s_non_intra_search_q_r9, bref, (uint8_t)0u, (uint8_t)31u));
      }
      if (q_qual_min_r9_present) {
        HANDLE_CODE(unpack_integer(q_qual_min_r9, bref, (int8_t)-34, (int8_t)-3));
      }
      if (thresh_serving_low_q_r9_present) {
        HANDLE_CODE(unpack_integer(thresh_serving_low_q_r9, bref, (uint8_t)0u, (uint8_t)31u));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(q_qual_min_wb_r11_present, 1));
      if (q_qual_min_wb_r11_present) {
        HANDLE_CODE(unpack_integer(q_qual_min_wb_r11, bref, (int8_t)-34, (int8_t)-3));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(q_qual_min_rsrq_on_all_symbols_r12_present, 1));
      if (q_qual_min_rsrq_on_all_symbols_r12_present) {
        HANDLE_CODE(unpack_integer(q_qual_min_rsrq_on_all_symbols_r12, bref, (int8_t)-34, (int8_t)-3));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cell_resel_serving_freq_info_v1310_present;
      HANDLE_CODE(bref.unpack(cell_resel_serving_freq_info_v1310_present, 1));
      cell_resel_serving_freq_info_v1310.set_present(cell_resel_serving_freq_info_v1310_present);
      bool redist_serving_info_r13_present;
      HANDLE_CODE(bref.unpack(redist_serving_info_r13_present, 1));
      redist_serving_info_r13.set_present(redist_serving_info_r13_present);
      bool cell_sel_info_ce_r13_present;
      HANDLE_CODE(bref.unpack(cell_sel_info_ce_r13_present, 1));
      cell_sel_info_ce_r13.set_present(cell_sel_info_ce_r13_present);
      HANDLE_CODE(bref.unpack(t_resel_eutra_ce_r13_present, 1));
      if (cell_resel_serving_freq_info_v1310.is_present()) {
        HANDLE_CODE(cell_resel_serving_freq_info_v1310->unpack(bref));
      }
      if (redist_serving_info_r13.is_present()) {
        HANDLE_CODE(redist_serving_info_r13->unpack(bref));
      }
      if (cell_sel_info_ce_r13.is_present()) {
        HANDLE_CODE(cell_sel_info_ce_r13->unpack(bref));
      }
      if (t_resel_eutra_ce_r13_present) {
        HANDLE_CODE(unpack_integer(t_resel_eutra_ce_r13, bref, (uint8_t)0u, (uint8_t)15u));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cell_sel_info_ce1_r13_present;
      HANDLE_CODE(bref.unpack(cell_sel_info_ce1_r13_present, 1));
      cell_sel_info_ce1_r13.set_present(cell_sel_info_ce1_r13_present);
      if (cell_sel_info_ce1_r13.is_present()) {
        HANDLE_CODE(cell_sel_info_ce1_r13->unpack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cell_sel_info_ce1_v1360_present;
      HANDLE_CODE(bref.unpack(cell_sel_info_ce1_v1360_present, 1));
      cell_sel_info_ce1_v1360.set_present(cell_sel_info_ce1_v1360_present);
      if (cell_sel_info_ce1_v1360.is_present()) {
        HANDLE_CODE(cell_sel_info_ce1_v1360->unpack(bref));
      }
    }
    if (group_flags[7]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cell_resel_info_common_v1460_present;
      HANDLE_CODE(bref.unpack(cell_resel_info_common_v1460_present, 1));
      cell_resel_info_common_v1460.set_present(cell_resel_info_common_v1460_present);
      if (cell_resel_info_common_v1460.is_present()) {
        HANDLE_CODE(cell_resel_info_common_v1460->unpack(bref));
      }
    }
    if (group_flags[8]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cell_resel_info_hsdn_r15_present;
      HANDLE_CODE(bref.unpack(cell_resel_info_hsdn_r15_present, 1));
      cell_resel_info_hsdn_r15.set_present(cell_resel_info_hsdn_r15_present);
      bool cell_sel_info_ce_v1530_present;
      HANDLE_CODE(bref.unpack(cell_sel_info_ce_v1530_present, 1));
      cell_sel_info_ce_v1530.set_present(cell_sel_info_ce_v1530_present);
      HANDLE_CODE(bref.unpack(crs_intf_mitig_neigh_cells_ce_r15_present, 1));
      if (cell_resel_info_hsdn_r15.is_present()) {
        HANDLE_CODE(cell_resel_info_hsdn_r15->unpack(bref));
      }
      if (cell_sel_info_ce_v1530.is_present()) {
        HANDLE_CODE(cell_sel_info_ce_v1530->unpack(bref));
      }
    }
    if (group_flags[9]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cell_resel_serving_freq_info_v1610_present;
      HANDLE_CODE(bref.unpack(cell_resel_serving_freq_info_v1610_present, 1));
      cell_resel_serving_freq_info_v1610.set_present(cell_resel_serving_freq_info_v1610_present);
      if (cell_resel_serving_freq_info_v1610.is_present()) {
        HANDLE_CODE(cell_resel_serving_freq_info_v1610->unpack(bref));
      }
    }
    if (group_flags[10]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(t_service_r17_present, 1));
      if (t_service_r17_present) {
        HANDLE_CODE(unpack_integer(t_service_r17, bref, (uint32_t)0u, (uint32_t)1048575u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type3_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellReselectionInfoCommon");
  j.start_obj();
  j.write_str("q-Hyst", cell_resel_info_common.q_hyst.to_string());
  if (cell_resel_info_common.speed_state_resel_pars_present) {
    j.write_fieldname("speedStateReselectionPars");
    j.start_obj();
    j.write_fieldname("mobilityStateParameters");
    cell_resel_info_common.speed_state_resel_pars.mob_state_params.to_json(j);
    j.write_fieldname("q-HystSF");
    j.start_obj();
    j.write_str("sf-Medium", cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_medium.to_string());
    j.write_str("sf-High", cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_high.to_string());
    j.end_obj();
    j.end_obj();
  }
  j.end_obj();
  j.write_fieldname("cellReselectionServingFreqInfo");
  j.start_obj();
  if (cell_resel_serving_freq_info.s_non_intra_search_present) {
    j.write_int("s-NonIntraSearch", cell_resel_serving_freq_info.s_non_intra_search);
  }
  j.write_int("threshServingLow", cell_resel_serving_freq_info.thresh_serving_low);
  j.write_int("cellReselectionPriority", cell_resel_serving_freq_info.cell_resel_prio);
  j.end_obj();
  j.write_fieldname("intraFreqCellReselectionInfo");
  j.start_obj();
  j.write_int("q-RxLevMin", intra_freq_cell_resel_info.q_rx_lev_min);
  if (intra_freq_cell_resel_info.p_max_present) {
    j.write_int("p-Max", intra_freq_cell_resel_info.p_max);
  }
  if (intra_freq_cell_resel_info.s_intra_search_present) {
    j.write_int("s-IntraSearch", intra_freq_cell_resel_info.s_intra_search);
  }
  if (intra_freq_cell_resel_info.allowed_meas_bw_present) {
    j.write_str("allowedMeasBandwidth", intra_freq_cell_resel_info.allowed_meas_bw.to_string());
  }
  j.write_bool("presenceAntennaPort1", intra_freq_cell_resel_info.presence_ant_port1);
  j.write_str("neighCellConfig", intra_freq_cell_resel_info.neigh_cell_cfg.to_string());
  j.write_int("t-ReselectionEUTRA", intra_freq_cell_resel_info.t_resel_eutra);
  if (intra_freq_cell_resel_info.t_resel_eutra_sf_present) {
    j.write_fieldname("t-ReselectionEUTRA-SF");
    intra_freq_cell_resel_info.t_resel_eutra_sf.to_json(j);
  }
  j.end_obj();
  if (ext) {
    if (late_non_crit_ext_present) {
      j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
    }
    if (s_intra_search_v920.is_present()) {
      j.write_fieldname("s-IntraSearch-v920");
      j.start_obj();
      j.write_int("s-IntraSearchP-r9", s_intra_search_v920->s_intra_search_p_r9);
      j.write_int("s-IntraSearchQ-r9", s_intra_search_v920->s_intra_search_q_r9);
      j.end_obj();
    }
    if (s_non_intra_search_v920.is_present()) {
      j.write_fieldname("s-NonIntraSearch-v920");
      j.start_obj();
      j.write_int("s-NonIntraSearchP-r9", s_non_intra_search_v920->s_non_intra_search_p_r9);
      j.write_int("s-NonIntraSearchQ-r9", s_non_intra_search_v920->s_non_intra_search_q_r9);
      j.end_obj();
    }
    if (q_qual_min_r9_present) {
      j.write_int("q-QualMin-r9", q_qual_min_r9);
    }
    if (thresh_serving_low_q_r9_present) {
      j.write_int("threshServingLowQ-r9", thresh_serving_low_q_r9);
    }
    if (q_qual_min_wb_r11_present) {
      j.write_int("q-QualMinWB-r11", q_qual_min_wb_r11);
    }
    if (q_qual_min_rsrq_on_all_symbols_r12_present) {
      j.write_int("q-QualMinRSRQ-OnAllSymbols-r12", q_qual_min_rsrq_on_all_symbols_r12);
    }
    if (cell_resel_serving_freq_info_v1310.is_present()) {
      j.write_fieldname("cellReselectionServingFreqInfo-v1310");
      cell_resel_serving_freq_info_v1310->to_json(j);
    }
    if (redist_serving_info_r13.is_present()) {
      j.write_fieldname("redistributionServingInfo-r13");
      redist_serving_info_r13->to_json(j);
    }
    if (cell_sel_info_ce_r13.is_present()) {
      j.write_fieldname("cellSelectionInfoCE-r13");
      cell_sel_info_ce_r13->to_json(j);
    }
    if (t_resel_eutra_ce_r13_present) {
      j.write_int("t-ReselectionEUTRA-CE-r13", t_resel_eutra_ce_r13);
    }
    if (cell_sel_info_ce1_r13.is_present()) {
      j.write_fieldname("cellSelectionInfoCE1-r13");
      cell_sel_info_ce1_r13->to_json(j);
    }
    if (cell_sel_info_ce1_v1360.is_present()) {
      j.write_fieldname("cellSelectionInfoCE1-v1360");
      cell_sel_info_ce1_v1360->to_json(j);
    }
    if (cell_resel_info_common_v1460.is_present()) {
      j.write_fieldname("cellReselectionInfoCommon-v1460");
      cell_resel_info_common_v1460->to_json(j);
    }
    if (cell_resel_info_hsdn_r15.is_present()) {
      j.write_fieldname("cellReselectionInfoHSDN-r15");
      cell_resel_info_hsdn_r15->to_json(j);
    }
    if (cell_sel_info_ce_v1530.is_present()) {
      j.write_fieldname("cellSelectionInfoCE-v1530");
      cell_sel_info_ce_v1530->to_json(j);
    }
    if (crs_intf_mitig_neigh_cells_ce_r15_present) {
      j.write_str("crs-IntfMitigNeighCellsCE-r15", "enabled");
    }
    if (cell_resel_serving_freq_info_v1610.is_present()) {
      j.write_fieldname("cellReselectionServingFreqInfo-v1610");
      cell_resel_serving_freq_info_v1610->to_json(j);
    }
    if (t_service_r17_present) {
      j.write_int("t-Service-r17", t_service_r17);
    }
  }
  j.end_obj();
}

const char* sib_type3_s::cell_resel_info_common_s_::q_hyst_opts::to_string() const
{
  static const char* options[] = {"dB0",
                                  "dB1",
                                  "dB2",
                                  "dB3",
                                  "dB4",
                                  "dB5",
                                  "dB6",
                                  "dB8",
                                  "dB10",
                                  "dB12",
                                  "dB14",
                                  "dB16",
                                  "dB18",
                                  "dB20",
                                  "dB22",
                                  "dB24"};
  return convert_enum_idx(options, 16, value, "sib_type3_s::cell_resel_info_common_s_::q_hyst_e_");
}
uint8_t sib_type3_s::cell_resel_info_common_s_::q_hyst_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(options, 16, value, "sib_type3_s::cell_resel_info_common_s_::q_hyst_e_");
}

const char*
sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4", "dB-2", "dB0"};
  return convert_enum_idx(
      options,
      4,
      value,
      "sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
}
int8_t
sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::to_number() const
{
  static const int8_t options[] = {-6, -4, -2, 0};
  return map_enum_number(
      options,
      4,
      value,
      "sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
}

const char*
sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4", "dB-2", "dB0"};
  return convert_enum_idx(
      options, 4, value, "sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
}
int8_t sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::to_number() const
{
  static const int8_t options[] = {-6, -4, -2, 0};
  return map_enum_number(
      options, 4, value, "sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
}

// SystemInformationBlockType30-r17 ::= SEQUENCE
SRSASN_CODE sib_type30_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(common_plmns_with_disaster_condition_r17_present, 1));
  HANDLE_CODE(bref.pack(applicable_disaster_info_list_r17_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (common_plmns_with_disaster_condition_r17_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, common_plmns_with_disaster_condition_r17, 1, 6));
  }
  if (applicable_disaster_info_list_r17_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, applicable_disaster_info_list_r17, 1, 6));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type30_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(common_plmns_with_disaster_condition_r17_present, 1));
  HANDLE_CODE(bref.unpack(applicable_disaster_info_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (common_plmns_with_disaster_condition_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(common_plmns_with_disaster_condition_r17, bref, 1, 6));
  }
  if (applicable_disaster_info_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(applicable_disaster_info_list_r17, bref, 1, 6));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type30_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (common_plmns_with_disaster_condition_r17_present) {
    j.start_array("commonPLMNsWithDisasterCondition-r17");
    for (const auto& e1 : common_plmns_with_disaster_condition_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (applicable_disaster_info_list_r17_present) {
    j.start_array("applicableDisasterInfoList-r17");
    for (const auto& e1 : applicable_disaster_info_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SystemInformationBlockType31-r17 ::= SEQUENCE
SRSASN_CODE sib_type31_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  HANDLE_CODE(serving_satellite_info_r17.pack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type31_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(serving_satellite_info_r17.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type31_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("servingSatelliteInfo-r17");
  serving_satellite_info_r17.to_json(j);
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SystemInformationBlockType32-r17 ::= SEQUENCE
SRSASN_CODE sib_type32_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(satellite_info_list_r17_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

  if (satellite_info_list_r17_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, satellite_info_list_r17, 1, 4));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type32_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(satellite_info_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (satellite_info_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(satellite_info_list_r17, bref, 1, 4));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type32_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (satellite_info_list_r17_present) {
    j.start_array("satelliteInfoList-r17");
    for (const auto& e1 : satellite_info_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SystemInformationBlockType4 ::= SEQUENCE
SRSASN_CODE sib_type4_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(intra_freq_neigh_cell_list_present, 1));
  HANDLE_CODE(bref.pack(intra_freq_excluded_cell_list_present, 1));
  HANDLE_CODE(bref.pack(csg_pci_range_present, 1));

  if (intra_freq_neigh_cell_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, intra_freq_neigh_cell_list, 1, 16));
  }
  if (intra_freq_excluded_cell_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, intra_freq_excluded_cell_list, 1, 16));
  }
  if (csg_pci_range_present) {
    HANDLE_CODE(csg_pci_range.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= late_non_crit_ext_present;
    group_flags[1] |= intra_freq_neigh_hsdn_cell_list_r15.is_present();
    group_flags[2] |= rss_cfg_carrier_info_r16.is_present();
    group_flags[2] |= intra_freq_neigh_cell_list_v1610.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(intra_freq_neigh_hsdn_cell_list_r15.is_present(), 1));
      if (intra_freq_neigh_hsdn_cell_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_neigh_hsdn_cell_list_r15, 1, 16));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rss_cfg_carrier_info_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(intra_freq_neigh_cell_list_v1610.is_present(), 1));
      if (rss_cfg_carrier_info_r16.is_present()) {
        HANDLE_CODE(rss_cfg_carrier_info_r16->pack(bref));
      }
      if (intra_freq_neigh_cell_list_v1610.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_neigh_cell_list_v1610, 1, 16));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type4_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(intra_freq_neigh_cell_list_present, 1));
  HANDLE_CODE(bref.unpack(intra_freq_excluded_cell_list_present, 1));
  HANDLE_CODE(bref.unpack(csg_pci_range_present, 1));

  if (intra_freq_neigh_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(intra_freq_neigh_cell_list, bref, 1, 16));
  }
  if (intra_freq_excluded_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(intra_freq_excluded_cell_list, bref, 1, 16));
  }
  if (csg_pci_range_present) {
    HANDLE_CODE(csg_pci_range.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool intra_freq_neigh_hsdn_cell_list_r15_present;
      HANDLE_CODE(bref.unpack(intra_freq_neigh_hsdn_cell_list_r15_present, 1));
      intra_freq_neigh_hsdn_cell_list_r15.set_present(intra_freq_neigh_hsdn_cell_list_r15_present);
      if (intra_freq_neigh_hsdn_cell_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_neigh_hsdn_cell_list_r15, bref, 1, 16));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rss_cfg_carrier_info_r16_present;
      HANDLE_CODE(bref.unpack(rss_cfg_carrier_info_r16_present, 1));
      rss_cfg_carrier_info_r16.set_present(rss_cfg_carrier_info_r16_present);
      bool intra_freq_neigh_cell_list_v1610_present;
      HANDLE_CODE(bref.unpack(intra_freq_neigh_cell_list_v1610_present, 1));
      intra_freq_neigh_cell_list_v1610.set_present(intra_freq_neigh_cell_list_v1610_present);
      if (rss_cfg_carrier_info_r16.is_present()) {
        HANDLE_CODE(rss_cfg_carrier_info_r16->unpack(bref));
      }
      if (intra_freq_neigh_cell_list_v1610.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_neigh_cell_list_v1610, bref, 1, 16));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type4_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (intra_freq_neigh_cell_list_present) {
    j.start_array("intraFreqNeighCellList");
    for (const auto& e1 : intra_freq_neigh_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (intra_freq_excluded_cell_list_present) {
    j.start_array("intraFreqExcludedCellList");
    for (const auto& e1 : intra_freq_excluded_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (csg_pci_range_present) {
    j.write_fieldname("csg-PhysCellIdRange");
    csg_pci_range.to_json(j);
  }
  if (ext) {
    if (late_non_crit_ext_present) {
      j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
    }
    if (intra_freq_neigh_hsdn_cell_list_r15.is_present()) {
      j.start_array("intraFreqNeighHSDN-CellList-r15");
      for (const auto& e1 : *intra_freq_neigh_hsdn_cell_list_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (rss_cfg_carrier_info_r16.is_present()) {
      j.write_fieldname("rss-ConfigCarrierInfo-r16");
      rss_cfg_carrier_info_r16->to_json(j);
    }
    if (intra_freq_neigh_cell_list_v1610.is_present()) {
      j.start_array("intraFreqNeighCellList-v1610");
      for (const auto& e1 : *intra_freq_neigh_cell_list_v1610) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SystemInformationBlockType5 ::= SEQUENCE
SRSASN_CODE sib_type5_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_carrier_freq_list, 1, 8));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= late_non_crit_ext_present;
    group_flags[1] |= inter_freq_carrier_freq_list_v1250.is_present();
    group_flags[1] |= inter_freq_carrier_freq_list_ext_r12.is_present();
    group_flags[2] |= inter_freq_carrier_freq_list_ext_v1280.is_present();
    group_flags[3] |= inter_freq_carrier_freq_list_v1310.is_present();
    group_flags[3] |= inter_freq_carrier_freq_list_ext_v1310.is_present();
    group_flags[4] |= inter_freq_carrier_freq_list_v1350.is_present();
    group_flags[4] |= inter_freq_carrier_freq_list_ext_v1350.is_present();
    group_flags[5] |= inter_freq_carrier_freq_list_ext_v1360.is_present();
    group_flags[6] |= scptm_freq_offset_r14_present;
    group_flags[7] |= inter_freq_carrier_freq_list_v1530.is_present();
    group_flags[7] |= inter_freq_carrier_freq_list_ext_v1530.is_present();
    group_flags[7] |= meas_idle_cfg_sib_r15.is_present();
    group_flags[8] |= inter_freq_carrier_freq_list_v1610.is_present();
    group_flags[8] |= inter_freq_carrier_freq_list_ext_v1610.is_present();
    group_flags[8] |= meas_idle_cfg_sib_nr_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_ext_r12.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1250.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1250, 1, 8));
      }
      if (inter_freq_carrier_freq_list_ext_r12.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_ext_r12, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_ext_v1280.is_present(), 1));
      if (inter_freq_carrier_freq_list_ext_v1280.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_ext_v1280, 1, 8));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_ext_v1310.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1310.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1310, 1, 8));
      }
      if (inter_freq_carrier_freq_list_ext_v1310.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_ext_v1310, 1, 8));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1350.is_present(), 1));
      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_ext_v1350.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1350.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1350, 1, 8));
      }
      if (inter_freq_carrier_freq_list_ext_v1350.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_ext_v1350, 1, 8));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_ext_v1360.is_present(), 1));
      if (inter_freq_carrier_freq_list_ext_v1360.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_ext_v1360, 1, 8));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(scptm_freq_offset_r14_present, 1));
      if (scptm_freq_offset_r14_present) {
        HANDLE_CODE(pack_integer(bref, scptm_freq_offset_r14, (uint8_t)1u, (uint8_t)8u));
      }
    }
    if (group_flags[7]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_ext_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_idle_cfg_sib_r15.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1530.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1530, 1, 8));
      }
      if (inter_freq_carrier_freq_list_ext_v1530.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_ext_v1530, 1, 8));
      }
      if (meas_idle_cfg_sib_r15.is_present()) {
        HANDLE_CODE(meas_idle_cfg_sib_r15->pack(bref));
      }
    }
    if (group_flags[8]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1610.is_present(), 1));
      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_ext_v1610.is_present(), 1));
      HANDLE_CODE(bref.pack(meas_idle_cfg_sib_nr_r16.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1610.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1610, 1, 8));
      }
      if (inter_freq_carrier_freq_list_ext_v1610.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_ext_v1610, 1, 8));
      }
      if (meas_idle_cfg_sib_nr_r16.is_present()) {
        HANDLE_CODE(meas_idle_cfg_sib_nr_r16->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type5_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(inter_freq_carrier_freq_list, bref, 1, 8));

  if (ext) {
    ext_groups_unpacker_guard group_flags(9);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool inter_freq_carrier_freq_list_v1250_present;
      HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_v1250_present, 1));
      inter_freq_carrier_freq_list_v1250.set_present(inter_freq_carrier_freq_list_v1250_present);
      bool inter_freq_carrier_freq_list_ext_r12_present;
      HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_ext_r12_present, 1));
      inter_freq_carrier_freq_list_ext_r12.set_present(inter_freq_carrier_freq_list_ext_r12_present);
      if (inter_freq_carrier_freq_list_v1250.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1250, bref, 1, 8));
      }
      if (inter_freq_carrier_freq_list_ext_r12.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_ext_r12, bref, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool inter_freq_carrier_freq_list_ext_v1280_present;
      HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_ext_v1280_present, 1));
      inter_freq_carrier_freq_list_ext_v1280.set_present(inter_freq_carrier_freq_list_ext_v1280_present);
      if (inter_freq_carrier_freq_list_ext_v1280.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_ext_v1280, bref, 1, 8));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool inter_freq_carrier_freq_list_v1310_present;
      HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_v1310_present, 1));
      inter_freq_carrier_freq_list_v1310.set_present(inter_freq_carrier_freq_list_v1310_present);
      bool inter_freq_carrier_freq_list_ext_v1310_present;
      HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_ext_v1310_present, 1));
      inter_freq_carrier_freq_list_ext_v1310.set_present(inter_freq_carrier_freq_list_ext_v1310_present);
      if (inter_freq_carrier_freq_list_v1310.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1310, bref, 1, 8));
      }
      if (inter_freq_carrier_freq_list_ext_v1310.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_ext_v1310, bref, 1, 8));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool inter_freq_carrier_freq_list_v1350_present;
      HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_v1350_present, 1));
      inter_freq_carrier_freq_list_v1350.set_present(inter_freq_carrier_freq_list_v1350_present);
      bool inter_freq_carrier_freq_list_ext_v1350_present;
      HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_ext_v1350_present, 1));
      inter_freq_carrier_freq_list_ext_v1350.set_present(inter_freq_carrier_freq_list_ext_v1350_present);
      if (inter_freq_carrier_freq_list_v1350.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1350, bref, 1, 8));
      }
      if (inter_freq_carrier_freq_list_ext_v1350.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_ext_v1350, bref, 1, 8));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool inter_freq_carrier_freq_list_ext_v1360_present;
      HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_ext_v1360_present, 1));
      inter_freq_carrier_freq_list_ext_v1360.set_present(inter_freq_carrier_freq_list_ext_v1360_present);
      if (inter_freq_carrier_freq_list_ext_v1360.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_ext_v1360, bref, 1, 8));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(scptm_freq_offset_r14_present, 1));
      if (scptm_freq_offset_r14_present) {
        HANDLE_CODE(unpack_integer(scptm_freq_offset_r14, bref, (uint8_t)1u, (uint8_t)8u));
      }
    }
    if (group_flags[7]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool inter_freq_carrier_freq_list_v1530_present;
      HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_v1530_present, 1));
      inter_freq_carrier_freq_list_v1530.set_present(inter_freq_carrier_freq_list_v1530_present);
      bool inter_freq_carrier_freq_list_ext_v1530_present;
      HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_ext_v1530_present, 1));
      inter_freq_carrier_freq_list_ext_v1530.set_present(inter_freq_carrier_freq_list_ext_v1530_present);
      bool meas_idle_cfg_sib_r15_present;
      HANDLE_CODE(bref.unpack(meas_idle_cfg_sib_r15_present, 1));
      meas_idle_cfg_sib_r15.set_present(meas_idle_cfg_sib_r15_present);
      if (inter_freq_carrier_freq_list_v1530.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1530, bref, 1, 8));
      }
      if (inter_freq_carrier_freq_list_ext_v1530.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_ext_v1530, bref, 1, 8));
      }
      if (meas_idle_cfg_sib_r15.is_present()) {
        HANDLE_CODE(meas_idle_cfg_sib_r15->unpack(bref));
      }
    }
    if (group_flags[8]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool inter_freq_carrier_freq_list_v1610_present;
      HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_v1610_present, 1));
      inter_freq_carrier_freq_list_v1610.set_present(inter_freq_carrier_freq_list_v1610_present);
      bool inter_freq_carrier_freq_list_ext_v1610_present;
      HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_ext_v1610_present, 1));
      inter_freq_carrier_freq_list_ext_v1610.set_present(inter_freq_carrier_freq_list_ext_v1610_present);
      bool meas_idle_cfg_sib_nr_r16_present;
      HANDLE_CODE(bref.unpack(meas_idle_cfg_sib_nr_r16_present, 1));
      meas_idle_cfg_sib_nr_r16.set_present(meas_idle_cfg_sib_nr_r16_present);
      if (inter_freq_carrier_freq_list_v1610.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1610, bref, 1, 8));
      }
      if (inter_freq_carrier_freq_list_ext_v1610.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_ext_v1610, bref, 1, 8));
      }
      if (meas_idle_cfg_sib_nr_r16.is_present()) {
        HANDLE_CODE(meas_idle_cfg_sib_nr_r16->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type5_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("interFreqCarrierFreqList");
  for (const auto& e1 : inter_freq_carrier_freq_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (late_non_crit_ext_present) {
      j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
    }
    if (inter_freq_carrier_freq_list_v1250.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1250");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1250) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_ext_r12.is_present()) {
      j.start_array("interFreqCarrierFreqListExt-r12");
      for (const auto& e1 : *inter_freq_carrier_freq_list_ext_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_ext_v1280.is_present()) {
      j.start_array("interFreqCarrierFreqListExt-v1280");
      for (const auto& e1 : *inter_freq_carrier_freq_list_ext_v1280) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_v1310.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1310");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1310) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_ext_v1310.is_present()) {
      j.start_array("interFreqCarrierFreqListExt-v1310");
      for (const auto& e1 : *inter_freq_carrier_freq_list_ext_v1310) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_v1350.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1350");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1350) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_ext_v1350.is_present()) {
      j.start_array("interFreqCarrierFreqListExt-v1350");
      for (const auto& e1 : *inter_freq_carrier_freq_list_ext_v1350) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_ext_v1360.is_present()) {
      j.start_array("interFreqCarrierFreqListExt-v1360");
      for (const auto& e1 : *inter_freq_carrier_freq_list_ext_v1360) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (scptm_freq_offset_r14_present) {
      j.write_int("scptm-FreqOffset-r14", scptm_freq_offset_r14);
    }
    if (inter_freq_carrier_freq_list_v1530.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1530");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1530) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_ext_v1530.is_present()) {
      j.start_array("interFreqCarrierFreqListExt-v1530");
      for (const auto& e1 : *inter_freq_carrier_freq_list_ext_v1530) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_idle_cfg_sib_r15.is_present()) {
      j.write_fieldname("measIdleConfigSIB-r15");
      meas_idle_cfg_sib_r15->to_json(j);
    }
    if (inter_freq_carrier_freq_list_v1610.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1610");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1610) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_ext_v1610.is_present()) {
      j.start_array("interFreqCarrierFreqListExt-v1610");
      for (const auto& e1 : *inter_freq_carrier_freq_list_ext_v1610) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (meas_idle_cfg_sib_nr_r16.is_present()) {
      j.write_fieldname("measIdleConfigSIB-NR-r16");
      meas_idle_cfg_sib_nr_r16->to_json(j);
    }
  }
  j.end_obj();
}

// SystemInformationBlockType6 ::= SEQUENCE
SRSASN_CODE sib_type6_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(carrier_freq_list_utra_fdd_present, 1));
  HANDLE_CODE(bref.pack(carrier_freq_list_utra_tdd_present, 1));
  HANDLE_CODE(bref.pack(t_resel_utra_sf_present, 1));

  if (carrier_freq_list_utra_fdd_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, carrier_freq_list_utra_fdd, 1, 16));
  }
  if (carrier_freq_list_utra_tdd_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, carrier_freq_list_utra_tdd, 1, 16));
  }
  HANDLE_CODE(pack_integer(bref, t_resel_utra, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_utra_sf_present) {
    HANDLE_CODE(t_resel_utra_sf.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= late_non_crit_ext_present;
    group_flags[1] |= carrier_freq_list_utra_fdd_v1250.is_present();
    group_flags[1] |= carrier_freq_list_utra_tdd_v1250.is_present();
    group_flags[1] |= carrier_freq_list_utra_fdd_ext_r12.is_present();
    group_flags[1] |= carrier_freq_list_utra_tdd_ext_r12.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(carrier_freq_list_utra_fdd_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(carrier_freq_list_utra_tdd_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(carrier_freq_list_utra_fdd_ext_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(carrier_freq_list_utra_tdd_ext_r12.is_present(), 1));
      if (carrier_freq_list_utra_fdd_v1250.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *carrier_freq_list_utra_fdd_v1250, 1, 16));
      }
      if (carrier_freq_list_utra_tdd_v1250.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *carrier_freq_list_utra_tdd_v1250, 1, 16));
      }
      if (carrier_freq_list_utra_fdd_ext_r12.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *carrier_freq_list_utra_fdd_ext_r12, 1, 16));
      }
      if (carrier_freq_list_utra_tdd_ext_r12.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *carrier_freq_list_utra_tdd_ext_r12, 1, 16));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type6_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(carrier_freq_list_utra_fdd_present, 1));
  HANDLE_CODE(bref.unpack(carrier_freq_list_utra_tdd_present, 1));
  HANDLE_CODE(bref.unpack(t_resel_utra_sf_present, 1));

  if (carrier_freq_list_utra_fdd_present) {
    HANDLE_CODE(unpack_dyn_seq_of(carrier_freq_list_utra_fdd, bref, 1, 16));
  }
  if (carrier_freq_list_utra_tdd_present) {
    HANDLE_CODE(unpack_dyn_seq_of(carrier_freq_list_utra_tdd, bref, 1, 16));
  }
  HANDLE_CODE(unpack_integer(t_resel_utra, bref, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_utra_sf_present) {
    HANDLE_CODE(t_resel_utra_sf.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool carrier_freq_list_utra_fdd_v1250_present;
      HANDLE_CODE(bref.unpack(carrier_freq_list_utra_fdd_v1250_present, 1));
      carrier_freq_list_utra_fdd_v1250.set_present(carrier_freq_list_utra_fdd_v1250_present);
      bool carrier_freq_list_utra_tdd_v1250_present;
      HANDLE_CODE(bref.unpack(carrier_freq_list_utra_tdd_v1250_present, 1));
      carrier_freq_list_utra_tdd_v1250.set_present(carrier_freq_list_utra_tdd_v1250_present);
      bool carrier_freq_list_utra_fdd_ext_r12_present;
      HANDLE_CODE(bref.unpack(carrier_freq_list_utra_fdd_ext_r12_present, 1));
      carrier_freq_list_utra_fdd_ext_r12.set_present(carrier_freq_list_utra_fdd_ext_r12_present);
      bool carrier_freq_list_utra_tdd_ext_r12_present;
      HANDLE_CODE(bref.unpack(carrier_freq_list_utra_tdd_ext_r12_present, 1));
      carrier_freq_list_utra_tdd_ext_r12.set_present(carrier_freq_list_utra_tdd_ext_r12_present);
      if (carrier_freq_list_utra_fdd_v1250.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*carrier_freq_list_utra_fdd_v1250, bref, 1, 16));
      }
      if (carrier_freq_list_utra_tdd_v1250.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*carrier_freq_list_utra_tdd_v1250, bref, 1, 16));
      }
      if (carrier_freq_list_utra_fdd_ext_r12.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*carrier_freq_list_utra_fdd_ext_r12, bref, 1, 16));
      }
      if (carrier_freq_list_utra_tdd_ext_r12.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*carrier_freq_list_utra_tdd_ext_r12, bref, 1, 16));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type6_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_list_utra_fdd_present) {
    j.start_array("carrierFreqListUTRA-FDD");
    for (const auto& e1 : carrier_freq_list_utra_fdd) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (carrier_freq_list_utra_tdd_present) {
    j.start_array("carrierFreqListUTRA-TDD");
    for (const auto& e1 : carrier_freq_list_utra_tdd) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("t-ReselectionUTRA", t_resel_utra);
  if (t_resel_utra_sf_present) {
    j.write_fieldname("t-ReselectionUTRA-SF");
    t_resel_utra_sf.to_json(j);
  }
  if (ext) {
    if (late_non_crit_ext_present) {
      j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
    }
    if (carrier_freq_list_utra_fdd_v1250.is_present()) {
      j.start_array("carrierFreqListUTRA-FDD-v1250");
      for (const auto& e1 : *carrier_freq_list_utra_fdd_v1250) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (carrier_freq_list_utra_tdd_v1250.is_present()) {
      j.start_array("carrierFreqListUTRA-TDD-v1250");
      for (const auto& e1 : *carrier_freq_list_utra_tdd_v1250) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (carrier_freq_list_utra_fdd_ext_r12.is_present()) {
      j.start_array("carrierFreqListUTRA-FDD-Ext-r12");
      for (const auto& e1 : *carrier_freq_list_utra_fdd_ext_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (carrier_freq_list_utra_tdd_ext_r12.is_present()) {
      j.start_array("carrierFreqListUTRA-TDD-Ext-r12");
      for (const auto& e1 : *carrier_freq_list_utra_tdd_ext_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SystemInformationBlockType7 ::= SEQUENCE
SRSASN_CODE sib_type7_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(t_resel_geran_sf_present, 1));
  HANDLE_CODE(bref.pack(carrier_freqs_info_list_present, 1));

  HANDLE_CODE(pack_integer(bref, t_resel_geran, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_geran_sf_present) {
    HANDLE_CODE(t_resel_geran_sf.pack(bref));
  }
  if (carrier_freqs_info_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, carrier_freqs_info_list, 1, 16));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

    if (late_non_crit_ext_present) {
      HANDLE_CODE(late_non_crit_ext.pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type7_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(t_resel_geran_sf_present, 1));
  HANDLE_CODE(bref.unpack(carrier_freqs_info_list_present, 1));

  HANDLE_CODE(unpack_integer(t_resel_geran, bref, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_geran_sf_present) {
    HANDLE_CODE(t_resel_geran_sf.unpack(bref));
  }
  if (carrier_freqs_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(carrier_freqs_info_list, bref, 1, 16));
  }

  if (ext) {
    HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

    if (late_non_crit_ext_present) {
      HANDLE_CODE(late_non_crit_ext.unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type7_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("t-ReselectionGERAN", t_resel_geran);
  if (t_resel_geran_sf_present) {
    j.write_fieldname("t-ReselectionGERAN-SF");
    t_resel_geran_sf.to_json(j);
  }
  if (carrier_freqs_info_list_present) {
    j.start_array("carrierFreqsInfoList");
    for (const auto& e1 : carrier_freqs_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (late_non_crit_ext_present) {
      j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
    }
  }
  j.end_obj();
}

// SystemInformationBlockType8 ::= SEQUENCE
SRSASN_CODE sib_type8_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sys_time_info_present, 1));
  HANDLE_CODE(bref.pack(search_win_size_present, 1));
  HANDLE_CODE(bref.pack(params_hrpd_present, 1));
  HANDLE_CODE(bref.pack(params1_xrtt_present, 1));

  if (sys_time_info_present) {
    HANDLE_CODE(sys_time_info.pack(bref));
  }
  if (search_win_size_present) {
    HANDLE_CODE(pack_integer(bref, search_win_size, (uint8_t)0u, (uint8_t)15u));
  }
  if (params_hrpd_present) {
    HANDLE_CODE(bref.pack(params_hrpd.cell_resel_params_hrpd_present, 1));
    HANDLE_CODE(params_hrpd.pre_regist_info_hrpd.pack(bref));
    if (params_hrpd.cell_resel_params_hrpd_present) {
      HANDLE_CODE(params_hrpd.cell_resel_params_hrpd.pack(bref));
    }
  }
  if (params1_xrtt_present) {
    HANDLE_CODE(bref.pack(params1_xrtt.csfb_regist_param1_xrtt_present, 1));
    HANDLE_CODE(bref.pack(params1_xrtt.long_code_state1_xrtt_present, 1));
    HANDLE_CODE(bref.pack(params1_xrtt.cell_resel_params1_xrtt_present, 1));
    if (params1_xrtt.csfb_regist_param1_xrtt_present) {
      HANDLE_CODE(params1_xrtt.csfb_regist_param1_xrtt.pack(bref));
    }
    if (params1_xrtt.long_code_state1_xrtt_present) {
      HANDLE_CODE(params1_xrtt.long_code_state1_xrtt.pack(bref));
    }
    if (params1_xrtt.cell_resel_params1_xrtt_present) {
      HANDLE_CODE(params1_xrtt.cell_resel_params1_xrtt.pack(bref));
    }
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= late_non_crit_ext_present;
    group_flags[1] |= csfb_support_for_dual_rx_ues_r9_present;
    group_flags[1] |= cell_resel_params_hrpd_v920.is_present();
    group_flags[1] |= cell_resel_params1_xrtt_v920.is_present();
    group_flags[1] |= csfb_regist_param1_xrtt_v920.is_present();
    group_flags[1] |= ac_barr_cfg1_xrtt_r9.is_present();
    group_flags[2] |= csfb_dual_rx_tx_support_r10_present;
    group_flags[3] |= sib8_per_plmn_list_r11.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csfb_support_for_dual_rx_ues_r9_present, 1));
      HANDLE_CODE(bref.pack(cell_resel_params_hrpd_v920.is_present(), 1));
      HANDLE_CODE(bref.pack(cell_resel_params1_xrtt_v920.is_present(), 1));
      HANDLE_CODE(bref.pack(csfb_regist_param1_xrtt_v920.is_present(), 1));
      HANDLE_CODE(bref.pack(ac_barr_cfg1_xrtt_r9.is_present(), 1));
      if (csfb_support_for_dual_rx_ues_r9_present) {
        HANDLE_CODE(bref.pack(csfb_support_for_dual_rx_ues_r9, 1));
      }
      if (cell_resel_params_hrpd_v920.is_present()) {
        HANDLE_CODE(cell_resel_params_hrpd_v920->pack(bref));
      }
      if (cell_resel_params1_xrtt_v920.is_present()) {
        HANDLE_CODE(cell_resel_params1_xrtt_v920->pack(bref));
      }
      if (csfb_regist_param1_xrtt_v920.is_present()) {
        HANDLE_CODE(csfb_regist_param1_xrtt_v920->pack(bref));
      }
      if (ac_barr_cfg1_xrtt_r9.is_present()) {
        HANDLE_CODE(ac_barr_cfg1_xrtt_r9->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csfb_dual_rx_tx_support_r10_present, 1));
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sib8_per_plmn_list_r11.is_present(), 1));
      if (sib8_per_plmn_list_r11.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *sib8_per_plmn_list_r11, 1, 6));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type8_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sys_time_info_present, 1));
  HANDLE_CODE(bref.unpack(search_win_size_present, 1));
  HANDLE_CODE(bref.unpack(params_hrpd_present, 1));
  HANDLE_CODE(bref.unpack(params1_xrtt_present, 1));

  if (sys_time_info_present) {
    HANDLE_CODE(sys_time_info.unpack(bref));
  }
  if (search_win_size_present) {
    HANDLE_CODE(unpack_integer(search_win_size, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (params_hrpd_present) {
    HANDLE_CODE(bref.unpack(params_hrpd.cell_resel_params_hrpd_present, 1));
    HANDLE_CODE(params_hrpd.pre_regist_info_hrpd.unpack(bref));
    if (params_hrpd.cell_resel_params_hrpd_present) {
      HANDLE_CODE(params_hrpd.cell_resel_params_hrpd.unpack(bref));
    }
  }
  if (params1_xrtt_present) {
    HANDLE_CODE(bref.unpack(params1_xrtt.csfb_regist_param1_xrtt_present, 1));
    HANDLE_CODE(bref.unpack(params1_xrtt.long_code_state1_xrtt_present, 1));
    HANDLE_CODE(bref.unpack(params1_xrtt.cell_resel_params1_xrtt_present, 1));
    if (params1_xrtt.csfb_regist_param1_xrtt_present) {
      HANDLE_CODE(params1_xrtt.csfb_regist_param1_xrtt.unpack(bref));
    }
    if (params1_xrtt.long_code_state1_xrtt_present) {
      HANDLE_CODE(params1_xrtt.long_code_state1_xrtt.unpack(bref));
    }
    if (params1_xrtt.cell_resel_params1_xrtt_present) {
      HANDLE_CODE(params1_xrtt.cell_resel_params1_xrtt.unpack(bref));
    }
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(csfb_support_for_dual_rx_ues_r9_present, 1));
      bool cell_resel_params_hrpd_v920_present;
      HANDLE_CODE(bref.unpack(cell_resel_params_hrpd_v920_present, 1));
      cell_resel_params_hrpd_v920.set_present(cell_resel_params_hrpd_v920_present);
      bool cell_resel_params1_xrtt_v920_present;
      HANDLE_CODE(bref.unpack(cell_resel_params1_xrtt_v920_present, 1));
      cell_resel_params1_xrtt_v920.set_present(cell_resel_params1_xrtt_v920_present);
      bool csfb_regist_param1_xrtt_v920_present;
      HANDLE_CODE(bref.unpack(csfb_regist_param1_xrtt_v920_present, 1));
      csfb_regist_param1_xrtt_v920.set_present(csfb_regist_param1_xrtt_v920_present);
      bool ac_barr_cfg1_xrtt_r9_present;
      HANDLE_CODE(bref.unpack(ac_barr_cfg1_xrtt_r9_present, 1));
      ac_barr_cfg1_xrtt_r9.set_present(ac_barr_cfg1_xrtt_r9_present);
      if (csfb_support_for_dual_rx_ues_r9_present) {
        HANDLE_CODE(bref.unpack(csfb_support_for_dual_rx_ues_r9, 1));
      }
      if (cell_resel_params_hrpd_v920.is_present()) {
        HANDLE_CODE(cell_resel_params_hrpd_v920->unpack(bref));
      }
      if (cell_resel_params1_xrtt_v920.is_present()) {
        HANDLE_CODE(cell_resel_params1_xrtt_v920->unpack(bref));
      }
      if (csfb_regist_param1_xrtt_v920.is_present()) {
        HANDLE_CODE(csfb_regist_param1_xrtt_v920->unpack(bref));
      }
      if (ac_barr_cfg1_xrtt_r9.is_present()) {
        HANDLE_CODE(ac_barr_cfg1_xrtt_r9->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(csfb_dual_rx_tx_support_r10_present, 1));
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool sib8_per_plmn_list_r11_present;
      HANDLE_CODE(bref.unpack(sib8_per_plmn_list_r11_present, 1));
      sib8_per_plmn_list_r11.set_present(sib8_per_plmn_list_r11_present);
      if (sib8_per_plmn_list_r11.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sib8_per_plmn_list_r11, bref, 1, 6));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type8_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sys_time_info_present) {
    j.write_fieldname("systemTimeInfo");
    sys_time_info.to_json(j);
  }
  if (search_win_size_present) {
    j.write_int("searchWindowSize", search_win_size);
  }
  if (params_hrpd_present) {
    j.write_fieldname("parametersHRPD");
    j.start_obj();
    j.write_fieldname("preRegistrationInfoHRPD");
    params_hrpd.pre_regist_info_hrpd.to_json(j);
    if (params_hrpd.cell_resel_params_hrpd_present) {
      j.write_fieldname("cellReselectionParametersHRPD");
      params_hrpd.cell_resel_params_hrpd.to_json(j);
    }
    j.end_obj();
  }
  if (params1_xrtt_present) {
    j.write_fieldname("parameters1XRTT");
    j.start_obj();
    if (params1_xrtt.csfb_regist_param1_xrtt_present) {
      j.write_fieldname("csfb-RegistrationParam1XRTT");
      params1_xrtt.csfb_regist_param1_xrtt.to_json(j);
    }
    if (params1_xrtt.long_code_state1_xrtt_present) {
      j.write_str("longCodeState1XRTT", params1_xrtt.long_code_state1_xrtt.to_string());
    }
    if (params1_xrtt.cell_resel_params1_xrtt_present) {
      j.write_fieldname("cellReselectionParameters1XRTT");
      params1_xrtt.cell_resel_params1_xrtt.to_json(j);
    }
    j.end_obj();
  }
  if (ext) {
    if (late_non_crit_ext_present) {
      j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
    }
    if (csfb_support_for_dual_rx_ues_r9_present) {
      j.write_bool("csfb-SupportForDualRxUEs-r9", csfb_support_for_dual_rx_ues_r9);
    }
    if (cell_resel_params_hrpd_v920.is_present()) {
      j.write_fieldname("cellReselectionParametersHRPD-v920");
      cell_resel_params_hrpd_v920->to_json(j);
    }
    if (cell_resel_params1_xrtt_v920.is_present()) {
      j.write_fieldname("cellReselectionParameters1XRTT-v920");
      cell_resel_params1_xrtt_v920->to_json(j);
    }
    if (csfb_regist_param1_xrtt_v920.is_present()) {
      j.write_fieldname("csfb-RegistrationParam1XRTT-v920");
      csfb_regist_param1_xrtt_v920->to_json(j);
    }
    if (ac_barr_cfg1_xrtt_r9.is_present()) {
      j.write_fieldname("ac-BarringConfig1XRTT-r9");
      ac_barr_cfg1_xrtt_r9->to_json(j);
    }
    if (csfb_dual_rx_tx_support_r10_present) {
      j.write_str("csfb-DualRxTxSupport-r10", "true");
    }
    if (sib8_per_plmn_list_r11.is_present()) {
      j.start_array("sib8-PerPLMN-List-r11");
      for (const auto& e1 : *sib8_per_plmn_list_r11) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SystemInformationBlockType9 ::= SEQUENCE
SRSASN_CODE sib_type9_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(hnb_name_present, 1));

  if (hnb_name_present) {
    HANDLE_CODE(hnb_name.pack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));

    if (late_non_crit_ext_present) {
      HANDLE_CODE(late_non_crit_ext.pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type9_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(hnb_name_present, 1));

  if (hnb_name_present) {
    HANDLE_CODE(hnb_name.unpack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

    if (late_non_crit_ext_present) {
      HANDLE_CODE(late_non_crit_ext.unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void sib_type9_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (hnb_name_present) {
    j.write_str("hnb-Name", hnb_name.to_string());
  }
  if (ext) {
    if (late_non_crit_ext_present) {
      j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
    }
  }
  j.end_obj();
}

// PosSystemInformation-r15-IEs ::= SEQUENCE
SRSASN_CODE pos_sys_info_r15_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, pos_sib_type_and_info_r15, 1, 32));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sys_info_r15_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(pos_sib_type_and_info_r15, bref, 1, 32));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pos_sys_info_r15_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("posSIB-TypeAndInfo-r15");
  for (const auto& e1 : pos_sib_type_and_info_r15) {
    e1.to_json(j);
  }
  j.end_array();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

void pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::destroy_()
{
  switch (type_) {
    case types::pos_sib1_minus1_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus2_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus3_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus4_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus5_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus6_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus7_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus1_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus2_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus3_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus4_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus5_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus6_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus7_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus8_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus9_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus10_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus11_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus12_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus13_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus14_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus15_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus16_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus17_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus18_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus19_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib3_minus1_r15:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus8_v1610:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus20_v1610:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus21_v1610:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus22_v1610:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus23_v1610:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus24_v1610:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus25_v1610:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib4_minus1_v1610:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib5_minus1_v1610:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus9_v1700:
      c.destroy<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus10_v1700:
      c.destroy<sib_pos_r15_s>();
      break;
    default:
      break;
  }
}
void pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::pos_sib1_minus1_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus2_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus3_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus4_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus5_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus6_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus7_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus1_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus2_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus3_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus4_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus5_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus6_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus7_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus8_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus9_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus10_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus11_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus12_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus13_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus14_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus15_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus16_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus17_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus18_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus19_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib3_minus1_r15:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus8_v1610:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus20_v1610:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus21_v1610:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus22_v1610:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus23_v1610:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus24_v1610:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib2_minus25_v1610:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib4_minus1_v1610:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib5_minus1_v1610:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus9_v1700:
      c.init<sib_pos_r15_s>();
      break;
    case types::pos_sib1_minus10_v1700:
      c.init<sib_pos_r15_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_");
  }
}
pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::pos_sib_type_and_info_r15_item_c_(
    const pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pos_sib1_minus1_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus2_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus3_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus4_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus5_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus6_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus7_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus1_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus2_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus3_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus4_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus5_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus6_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus7_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus8_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus9_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus10_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus11_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus12_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus13_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus14_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus15_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus16_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus17_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus18_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus19_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib3_minus1_r15:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus8_v1610:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus20_v1610:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus21_v1610:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus22_v1610:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus23_v1610:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus24_v1610:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus25_v1610:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib4_minus1_v1610:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib5_minus1_v1610:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus9_v1700:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus10_v1700:
      c.init(other.c.get<sib_pos_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_");
  }
}
pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_&
pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::operator=(
    const pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pos_sib1_minus1_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus2_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus3_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus4_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus5_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus6_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus7_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus1_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus2_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus3_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus4_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus5_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus6_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus7_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus8_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus9_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus10_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus11_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus12_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus13_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus14_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus15_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus16_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus17_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus18_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus19_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib3_minus1_r15:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus8_v1610:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus20_v1610:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus21_v1610:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus22_v1610:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus23_v1610:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus24_v1610:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib2_minus25_v1610:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib4_minus1_v1610:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib5_minus1_v1610:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus9_v1700:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::pos_sib1_minus10_v1700:
      c.set(other.c.get<sib_pos_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_");
  }

  return *this;
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib1_minus1_r15()
{
  set(types::pos_sib1_minus1_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib1_minus2_r15()
{
  set(types::pos_sib1_minus2_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib1_minus3_r15()
{
  set(types::pos_sib1_minus3_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib1_minus4_r15()
{
  set(types::pos_sib1_minus4_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib1_minus5_r15()
{
  set(types::pos_sib1_minus5_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib1_minus6_r15()
{
  set(types::pos_sib1_minus6_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib1_minus7_r15()
{
  set(types::pos_sib1_minus7_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus1_r15()
{
  set(types::pos_sib2_minus1_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus2_r15()
{
  set(types::pos_sib2_minus2_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus3_r15()
{
  set(types::pos_sib2_minus3_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus4_r15()
{
  set(types::pos_sib2_minus4_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus5_r15()
{
  set(types::pos_sib2_minus5_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus6_r15()
{
  set(types::pos_sib2_minus6_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus7_r15()
{
  set(types::pos_sib2_minus7_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus8_r15()
{
  set(types::pos_sib2_minus8_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus9_r15()
{
  set(types::pos_sib2_minus9_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus10_r15()
{
  set(types::pos_sib2_minus10_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus11_r15()
{
  set(types::pos_sib2_minus11_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus12_r15()
{
  set(types::pos_sib2_minus12_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus13_r15()
{
  set(types::pos_sib2_minus13_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus14_r15()
{
  set(types::pos_sib2_minus14_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus15_r15()
{
  set(types::pos_sib2_minus15_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus16_r15()
{
  set(types::pos_sib2_minus16_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus17_r15()
{
  set(types::pos_sib2_minus17_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus18_r15()
{
  set(types::pos_sib2_minus18_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus19_r15()
{
  set(types::pos_sib2_minus19_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib3_minus1_r15()
{
  set(types::pos_sib3_minus1_r15);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib1_minus8_v1610()
{
  set(types::pos_sib1_minus8_v1610);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus20_v1610()
{
  set(types::pos_sib2_minus20_v1610);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus21_v1610()
{
  set(types::pos_sib2_minus21_v1610);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus22_v1610()
{
  set(types::pos_sib2_minus22_v1610);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus23_v1610()
{
  set(types::pos_sib2_minus23_v1610);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus24_v1610()
{
  set(types::pos_sib2_minus24_v1610);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib2_minus25_v1610()
{
  set(types::pos_sib2_minus25_v1610);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib4_minus1_v1610()
{
  set(types::pos_sib4_minus1_v1610);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib5_minus1_v1610()
{
  set(types::pos_sib5_minus1_v1610);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib1_minus9_v1700()
{
  set(types::pos_sib1_minus9_v1700);
  return c.get<sib_pos_r15_s>();
}
sib_pos_r15_s& pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::set_pos_sib1_minus10_v1700()
{
  set(types::pos_sib1_minus10_v1700);
  return c.get<sib_pos_r15_s>();
}
void pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pos_sib1_minus1_r15:
      j.write_fieldname("posSib1-1-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib1_minus2_r15:
      j.write_fieldname("posSib1-2-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib1_minus3_r15:
      j.write_fieldname("posSib1-3-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib1_minus4_r15:
      j.write_fieldname("posSib1-4-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib1_minus5_r15:
      j.write_fieldname("posSib1-5-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib1_minus6_r15:
      j.write_fieldname("posSib1-6-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib1_minus7_r15:
      j.write_fieldname("posSib1-7-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus1_r15:
      j.write_fieldname("posSib2-1-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus2_r15:
      j.write_fieldname("posSib2-2-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus3_r15:
      j.write_fieldname("posSib2-3-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus4_r15:
      j.write_fieldname("posSib2-4-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus5_r15:
      j.write_fieldname("posSib2-5-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus6_r15:
      j.write_fieldname("posSib2-6-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus7_r15:
      j.write_fieldname("posSib2-7-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus8_r15:
      j.write_fieldname("posSib2-8-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus9_r15:
      j.write_fieldname("posSib2-9-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus10_r15:
      j.write_fieldname("posSib2-10-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus11_r15:
      j.write_fieldname("posSib2-11-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus12_r15:
      j.write_fieldname("posSib2-12-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus13_r15:
      j.write_fieldname("posSib2-13-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus14_r15:
      j.write_fieldname("posSib2-14-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus15_r15:
      j.write_fieldname("posSib2-15-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus16_r15:
      j.write_fieldname("posSib2-16-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus17_r15:
      j.write_fieldname("posSib2-17-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus18_r15:
      j.write_fieldname("posSib2-18-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus19_r15:
      j.write_fieldname("posSib2-19-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib3_minus1_r15:
      j.write_fieldname("posSib3-1-r15");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib1_minus8_v1610:
      j.write_fieldname("posSib1-8-v1610");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus20_v1610:
      j.write_fieldname("posSib2-20-v1610");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus21_v1610:
      j.write_fieldname("posSib2-21-v1610");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus22_v1610:
      j.write_fieldname("posSib2-22-v1610");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus23_v1610:
      j.write_fieldname("posSib2-23-v1610");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus24_v1610:
      j.write_fieldname("posSib2-24-v1610");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib2_minus25_v1610:
      j.write_fieldname("posSib2-25-v1610");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib4_minus1_v1610:
      j.write_fieldname("posSib4-1-v1610");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib5_minus1_v1610:
      j.write_fieldname("posSib5-1-v1610");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib1_minus9_v1700:
      j.write_fieldname("posSib1-9-v1700");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    case types::pos_sib1_minus10_v1700:
      j.write_fieldname("posSib1-10-v1700");
      c.get<sib_pos_r15_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_");
  }
  j.end_obj();
}
SRSASN_CODE pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pos_sib1_minus1_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib1_minus2_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib1_minus3_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib1_minus4_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib1_minus5_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib1_minus6_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib1_minus7_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus1_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus2_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus3_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus4_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus5_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus6_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus7_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus8_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus9_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus10_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus11_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus12_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus13_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus14_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus15_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus16_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus17_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus18_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib2_minus19_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib3_minus1_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
      break;
    case types::pos_sib1_minus8_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
    } break;
    case types::pos_sib2_minus20_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
    } break;
    case types::pos_sib2_minus21_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
    } break;
    case types::pos_sib2_minus22_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
    } break;
    case types::pos_sib2_minus23_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
    } break;
    case types::pos_sib2_minus24_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
    } break;
    case types::pos_sib2_minus25_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
    } break;
    case types::pos_sib4_minus1_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
    } break;
    case types::pos_sib5_minus1_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
    } break;
    case types::pos_sib1_minus9_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
    } break;
    case types::pos_sib1_minus10_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pos_sib1_minus1_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib1_minus2_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib1_minus3_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib1_minus4_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib1_minus5_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib1_minus6_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib1_minus7_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus1_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus2_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus3_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus4_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus5_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus6_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus7_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus8_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus9_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus10_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus11_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus12_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus13_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus14_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus15_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus16_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus17_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus18_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib2_minus19_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib3_minus1_r15:
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
      break;
    case types::pos_sib1_minus8_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
    } break;
    case types::pos_sib2_minus20_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
    } break;
    case types::pos_sib2_minus21_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
    } break;
    case types::pos_sib2_minus22_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
    } break;
    case types::pos_sib2_minus23_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
    } break;
    case types::pos_sib2_minus24_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
    } break;
    case types::pos_sib2_minus25_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
    } break;
    case types::pos_sib4_minus1_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
    } break;
    case types::pos_sib5_minus1_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
    } break;
    case types::pos_sib1_minus9_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
    } break;
    case types::pos_sib1_minus10_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r15_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sib_info_item_c::destroy_()
{
  switch (type_) {
    case types::sib2:
      c.destroy<sib_type2_s>();
      break;
    case types::sib3:
      c.destroy<sib_type3_s>();
      break;
    case types::sib4:
      c.destroy<sib_type4_s>();
      break;
    case types::sib5:
      c.destroy<sib_type5_s>();
      break;
    case types::sib6:
      c.destroy<sib_type6_s>();
      break;
    case types::sib7:
      c.destroy<sib_type7_s>();
      break;
    case types::sib8:
      c.destroy<sib_type8_s>();
      break;
    case types::sib9:
      c.destroy<sib_type9_s>();
      break;
    case types::sib10:
      c.destroy<sib_type10_s>();
      break;
    case types::sib11:
      c.destroy<sib_type11_s>();
      break;
    case types::sib12_v920:
      c.destroy<sib_type12_r9_s>();
      break;
    case types::sib13_v920:
      c.destroy<sib_type13_r9_s>();
      break;
    case types::sib14_v1130:
      c.destroy<sib_type14_r11_s>();
      break;
    case types::sib15_v1130:
      c.destroy<sib_type15_r11_s>();
      break;
    case types::sib16_v1130:
      c.destroy<sib_type16_r11_s>();
      break;
    case types::sib17_v1250:
      c.destroy<sib_type17_r12_s>();
      break;
    case types::sib18_v1250:
      c.destroy<sib_type18_r12_s>();
      break;
    case types::sib19_v1250:
      c.destroy<sib_type19_r12_s>();
      break;
    case types::sib20_v1310:
      c.destroy<sib_type20_r13_s>();
      break;
    case types::sib21_v1430:
      c.destroy<sib_type21_r14_s>();
      break;
    case types::sib24_v1530:
      c.destroy<sib_type24_r15_s>();
      break;
    case types::sib25_v1530:
      c.destroy<sib_type25_r15_s>();
      break;
    case types::sib26_v1530:
      c.destroy<sib_type26_r15_s>();
      break;
    case types::sib26a_v1610:
      c.destroy<sib_type26a_r16_s>();
      break;
    case types::sib27_v1610:
      c.destroy<sib_type27_r16_s>();
      break;
    case types::sib28_v1610:
      c.destroy<sib_type28_r16_s>();
      break;
    case types::sib29_v1610:
      c.destroy<sib_type29_r16_s>();
      break;
    case types::sib30_v1700:
      c.destroy<sib_type30_r17_s>();
      break;
    case types::sib31_v1700:
      c.destroy<sib_type31_r17_s>();
      break;
    case types::sib32_v1700:
      c.destroy<sib_type32_r17_s>();
      break;
    default:
      break;
  }
}
void sib_info_item_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sib2:
      c.init<sib_type2_s>();
      break;
    case types::sib3:
      c.init<sib_type3_s>();
      break;
    case types::sib4:
      c.init<sib_type4_s>();
      break;
    case types::sib5:
      c.init<sib_type5_s>();
      break;
    case types::sib6:
      c.init<sib_type6_s>();
      break;
    case types::sib7:
      c.init<sib_type7_s>();
      break;
    case types::sib8:
      c.init<sib_type8_s>();
      break;
    case types::sib9:
      c.init<sib_type9_s>();
      break;
    case types::sib10:
      c.init<sib_type10_s>();
      break;
    case types::sib11:
      c.init<sib_type11_s>();
      break;
    case types::sib12_v920:
      c.init<sib_type12_r9_s>();
      break;
    case types::sib13_v920:
      c.init<sib_type13_r9_s>();
      break;
    case types::sib14_v1130:
      c.init<sib_type14_r11_s>();
      break;
    case types::sib15_v1130:
      c.init<sib_type15_r11_s>();
      break;
    case types::sib16_v1130:
      c.init<sib_type16_r11_s>();
      break;
    case types::sib17_v1250:
      c.init<sib_type17_r12_s>();
      break;
    case types::sib18_v1250:
      c.init<sib_type18_r12_s>();
      break;
    case types::sib19_v1250:
      c.init<sib_type19_r12_s>();
      break;
    case types::sib20_v1310:
      c.init<sib_type20_r13_s>();
      break;
    case types::sib21_v1430:
      c.init<sib_type21_r14_s>();
      break;
    case types::sib24_v1530:
      c.init<sib_type24_r15_s>();
      break;
    case types::sib25_v1530:
      c.init<sib_type25_r15_s>();
      break;
    case types::sib26_v1530:
      c.init<sib_type26_r15_s>();
      break;
    case types::sib26a_v1610:
      c.init<sib_type26a_r16_s>();
      break;
    case types::sib27_v1610:
      c.init<sib_type27_r16_s>();
      break;
    case types::sib28_v1610:
      c.init<sib_type28_r16_s>();
      break;
    case types::sib29_v1610:
      c.init<sib_type29_r16_s>();
      break;
    case types::sib30_v1700:
      c.init<sib_type30_r17_s>();
      break;
    case types::sib31_v1700:
      c.init<sib_type31_r17_s>();
      break;
    case types::sib32_v1700:
      c.init<sib_type32_r17_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_info_item_c");
  }
}
sib_info_item_c::sib_info_item_c(const sib_info_item_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sib2:
      c.init(other.c.get<sib_type2_s>());
      break;
    case types::sib3:
      c.init(other.c.get<sib_type3_s>());
      break;
    case types::sib4:
      c.init(other.c.get<sib_type4_s>());
      break;
    case types::sib5:
      c.init(other.c.get<sib_type5_s>());
      break;
    case types::sib6:
      c.init(other.c.get<sib_type6_s>());
      break;
    case types::sib7:
      c.init(other.c.get<sib_type7_s>());
      break;
    case types::sib8:
      c.init(other.c.get<sib_type8_s>());
      break;
    case types::sib9:
      c.init(other.c.get<sib_type9_s>());
      break;
    case types::sib10:
      c.init(other.c.get<sib_type10_s>());
      break;
    case types::sib11:
      c.init(other.c.get<sib_type11_s>());
      break;
    case types::sib12_v920:
      c.init(other.c.get<sib_type12_r9_s>());
      break;
    case types::sib13_v920:
      c.init(other.c.get<sib_type13_r9_s>());
      break;
    case types::sib14_v1130:
      c.init(other.c.get<sib_type14_r11_s>());
      break;
    case types::sib15_v1130:
      c.init(other.c.get<sib_type15_r11_s>());
      break;
    case types::sib16_v1130:
      c.init(other.c.get<sib_type16_r11_s>());
      break;
    case types::sib17_v1250:
      c.init(other.c.get<sib_type17_r12_s>());
      break;
    case types::sib18_v1250:
      c.init(other.c.get<sib_type18_r12_s>());
      break;
    case types::sib19_v1250:
      c.init(other.c.get<sib_type19_r12_s>());
      break;
    case types::sib20_v1310:
      c.init(other.c.get<sib_type20_r13_s>());
      break;
    case types::sib21_v1430:
      c.init(other.c.get<sib_type21_r14_s>());
      break;
    case types::sib24_v1530:
      c.init(other.c.get<sib_type24_r15_s>());
      break;
    case types::sib25_v1530:
      c.init(other.c.get<sib_type25_r15_s>());
      break;
    case types::sib26_v1530:
      c.init(other.c.get<sib_type26_r15_s>());
      break;
    case types::sib26a_v1610:
      c.init(other.c.get<sib_type26a_r16_s>());
      break;
    case types::sib27_v1610:
      c.init(other.c.get<sib_type27_r16_s>());
      break;
    case types::sib28_v1610:
      c.init(other.c.get<sib_type28_r16_s>());
      break;
    case types::sib29_v1610:
      c.init(other.c.get<sib_type29_r16_s>());
      break;
    case types::sib30_v1700:
      c.init(other.c.get<sib_type30_r17_s>());
      break;
    case types::sib31_v1700:
      c.init(other.c.get<sib_type31_r17_s>());
      break;
    case types::sib32_v1700:
      c.init(other.c.get<sib_type32_r17_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_info_item_c");
  }
}
sib_info_item_c& sib_info_item_c::operator=(const sib_info_item_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sib2:
      c.set(other.c.get<sib_type2_s>());
      break;
    case types::sib3:
      c.set(other.c.get<sib_type3_s>());
      break;
    case types::sib4:
      c.set(other.c.get<sib_type4_s>());
      break;
    case types::sib5:
      c.set(other.c.get<sib_type5_s>());
      break;
    case types::sib6:
      c.set(other.c.get<sib_type6_s>());
      break;
    case types::sib7:
      c.set(other.c.get<sib_type7_s>());
      break;
    case types::sib8:
      c.set(other.c.get<sib_type8_s>());
      break;
    case types::sib9:
      c.set(other.c.get<sib_type9_s>());
      break;
    case types::sib10:
      c.set(other.c.get<sib_type10_s>());
      break;
    case types::sib11:
      c.set(other.c.get<sib_type11_s>());
      break;
    case types::sib12_v920:
      c.set(other.c.get<sib_type12_r9_s>());
      break;
    case types::sib13_v920:
      c.set(other.c.get<sib_type13_r9_s>());
      break;
    case types::sib14_v1130:
      c.set(other.c.get<sib_type14_r11_s>());
      break;
    case types::sib15_v1130:
      c.set(other.c.get<sib_type15_r11_s>());
      break;
    case types::sib16_v1130:
      c.set(other.c.get<sib_type16_r11_s>());
      break;
    case types::sib17_v1250:
      c.set(other.c.get<sib_type17_r12_s>());
      break;
    case types::sib18_v1250:
      c.set(other.c.get<sib_type18_r12_s>());
      break;
    case types::sib19_v1250:
      c.set(other.c.get<sib_type19_r12_s>());
      break;
    case types::sib20_v1310:
      c.set(other.c.get<sib_type20_r13_s>());
      break;
    case types::sib21_v1430:
      c.set(other.c.get<sib_type21_r14_s>());
      break;
    case types::sib24_v1530:
      c.set(other.c.get<sib_type24_r15_s>());
      break;
    case types::sib25_v1530:
      c.set(other.c.get<sib_type25_r15_s>());
      break;
    case types::sib26_v1530:
      c.set(other.c.get<sib_type26_r15_s>());
      break;
    case types::sib26a_v1610:
      c.set(other.c.get<sib_type26a_r16_s>());
      break;
    case types::sib27_v1610:
      c.set(other.c.get<sib_type27_r16_s>());
      break;
    case types::sib28_v1610:
      c.set(other.c.get<sib_type28_r16_s>());
      break;
    case types::sib29_v1610:
      c.set(other.c.get<sib_type29_r16_s>());
      break;
    case types::sib30_v1700:
      c.set(other.c.get<sib_type30_r17_s>());
      break;
    case types::sib31_v1700:
      c.set(other.c.get<sib_type31_r17_s>());
      break;
    case types::sib32_v1700:
      c.set(other.c.get<sib_type32_r17_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_info_item_c");
  }

  return *this;
}
sib_type2_s& sib_info_item_c::set_sib2()
{
  set(types::sib2);
  return c.get<sib_type2_s>();
}
sib_type3_s& sib_info_item_c::set_sib3()
{
  set(types::sib3);
  return c.get<sib_type3_s>();
}
sib_type4_s& sib_info_item_c::set_sib4()
{
  set(types::sib4);
  return c.get<sib_type4_s>();
}
sib_type5_s& sib_info_item_c::set_sib5()
{
  set(types::sib5);
  return c.get<sib_type5_s>();
}
sib_type6_s& sib_info_item_c::set_sib6()
{
  set(types::sib6);
  return c.get<sib_type6_s>();
}
sib_type7_s& sib_info_item_c::set_sib7()
{
  set(types::sib7);
  return c.get<sib_type7_s>();
}
sib_type8_s& sib_info_item_c::set_sib8()
{
  set(types::sib8);
  return c.get<sib_type8_s>();
}
sib_type9_s& sib_info_item_c::set_sib9()
{
  set(types::sib9);
  return c.get<sib_type9_s>();
}
sib_type10_s& sib_info_item_c::set_sib10()
{
  set(types::sib10);
  return c.get<sib_type10_s>();
}
sib_type11_s& sib_info_item_c::set_sib11()
{
  set(types::sib11);
  return c.get<sib_type11_s>();
}
sib_type12_r9_s& sib_info_item_c::set_sib12_v920()
{
  set(types::sib12_v920);
  return c.get<sib_type12_r9_s>();
}
sib_type13_r9_s& sib_info_item_c::set_sib13_v920()
{
  set(types::sib13_v920);
  return c.get<sib_type13_r9_s>();
}
sib_type14_r11_s& sib_info_item_c::set_sib14_v1130()
{
  set(types::sib14_v1130);
  return c.get<sib_type14_r11_s>();
}
sib_type15_r11_s& sib_info_item_c::set_sib15_v1130()
{
  set(types::sib15_v1130);
  return c.get<sib_type15_r11_s>();
}
sib_type16_r11_s& sib_info_item_c::set_sib16_v1130()
{
  set(types::sib16_v1130);
  return c.get<sib_type16_r11_s>();
}
sib_type17_r12_s& sib_info_item_c::set_sib17_v1250()
{
  set(types::sib17_v1250);
  return c.get<sib_type17_r12_s>();
}
sib_type18_r12_s& sib_info_item_c::set_sib18_v1250()
{
  set(types::sib18_v1250);
  return c.get<sib_type18_r12_s>();
}
sib_type19_r12_s& sib_info_item_c::set_sib19_v1250()
{
  set(types::sib19_v1250);
  return c.get<sib_type19_r12_s>();
}
sib_type20_r13_s& sib_info_item_c::set_sib20_v1310()
{
  set(types::sib20_v1310);
  return c.get<sib_type20_r13_s>();
}
sib_type21_r14_s& sib_info_item_c::set_sib21_v1430()
{
  set(types::sib21_v1430);
  return c.get<sib_type21_r14_s>();
}
sib_type24_r15_s& sib_info_item_c::set_sib24_v1530()
{
  set(types::sib24_v1530);
  return c.get<sib_type24_r15_s>();
}
sib_type25_r15_s& sib_info_item_c::set_sib25_v1530()
{
  set(types::sib25_v1530);
  return c.get<sib_type25_r15_s>();
}
sib_type26_r15_s& sib_info_item_c::set_sib26_v1530()
{
  set(types::sib26_v1530);
  return c.get<sib_type26_r15_s>();
}
sib_type26a_r16_s& sib_info_item_c::set_sib26a_v1610()
{
  set(types::sib26a_v1610);
  return c.get<sib_type26a_r16_s>();
}
sib_type27_r16_s& sib_info_item_c::set_sib27_v1610()
{
  set(types::sib27_v1610);
  return c.get<sib_type27_r16_s>();
}
sib_type28_r16_s& sib_info_item_c::set_sib28_v1610()
{
  set(types::sib28_v1610);
  return c.get<sib_type28_r16_s>();
}
sib_type29_r16_s& sib_info_item_c::set_sib29_v1610()
{
  set(types::sib29_v1610);
  return c.get<sib_type29_r16_s>();
}
sib_type30_r17_s& sib_info_item_c::set_sib30_v1700()
{
  set(types::sib30_v1700);
  return c.get<sib_type30_r17_s>();
}
sib_type31_r17_s& sib_info_item_c::set_sib31_v1700()
{
  set(types::sib31_v1700);
  return c.get<sib_type31_r17_s>();
}
sib_type32_r17_s& sib_info_item_c::set_sib32_v1700()
{
  set(types::sib32_v1700);
  return c.get<sib_type32_r17_s>();
}
void sib_info_item_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sib2:
      j.write_fieldname("sib2");
      c.get<sib_type2_s>().to_json(j);
      break;
    case types::sib3:
      j.write_fieldname("sib3");
      c.get<sib_type3_s>().to_json(j);
      break;
    case types::sib4:
      j.write_fieldname("sib4");
      c.get<sib_type4_s>().to_json(j);
      break;
    case types::sib5:
      j.write_fieldname("sib5");
      c.get<sib_type5_s>().to_json(j);
      break;
    case types::sib6:
      j.write_fieldname("sib6");
      c.get<sib_type6_s>().to_json(j);
      break;
    case types::sib7:
      j.write_fieldname("sib7");
      c.get<sib_type7_s>().to_json(j);
      break;
    case types::sib8:
      j.write_fieldname("sib8");
      c.get<sib_type8_s>().to_json(j);
      break;
    case types::sib9:
      j.write_fieldname("sib9");
      c.get<sib_type9_s>().to_json(j);
      break;
    case types::sib10:
      j.write_fieldname("sib10");
      c.get<sib_type10_s>().to_json(j);
      break;
    case types::sib11:
      j.write_fieldname("sib11");
      c.get<sib_type11_s>().to_json(j);
      break;
    case types::sib12_v920:
      j.write_fieldname("sib12-v920");
      c.get<sib_type12_r9_s>().to_json(j);
      break;
    case types::sib13_v920:
      j.write_fieldname("sib13-v920");
      c.get<sib_type13_r9_s>().to_json(j);
      break;
    case types::sib14_v1130:
      j.write_fieldname("sib14-v1130");
      c.get<sib_type14_r11_s>().to_json(j);
      break;
    case types::sib15_v1130:
      j.write_fieldname("sib15-v1130");
      c.get<sib_type15_r11_s>().to_json(j);
      break;
    case types::sib16_v1130:
      j.write_fieldname("sib16-v1130");
      c.get<sib_type16_r11_s>().to_json(j);
      break;
    case types::sib17_v1250:
      j.write_fieldname("sib17-v1250");
      c.get<sib_type17_r12_s>().to_json(j);
      break;
    case types::sib18_v1250:
      j.write_fieldname("sib18-v1250");
      c.get<sib_type18_r12_s>().to_json(j);
      break;
    case types::sib19_v1250:
      j.write_fieldname("sib19-v1250");
      c.get<sib_type19_r12_s>().to_json(j);
      break;
    case types::sib20_v1310:
      j.write_fieldname("sib20-v1310");
      c.get<sib_type20_r13_s>().to_json(j);
      break;
    case types::sib21_v1430:
      j.write_fieldname("sib21-v1430");
      c.get<sib_type21_r14_s>().to_json(j);
      break;
    case types::sib24_v1530:
      j.write_fieldname("sib24-v1530");
      c.get<sib_type24_r15_s>().to_json(j);
      break;
    case types::sib25_v1530:
      j.write_fieldname("sib25-v1530");
      c.get<sib_type25_r15_s>().to_json(j);
      break;
    case types::sib26_v1530:
      j.write_fieldname("sib26-v1530");
      c.get<sib_type26_r15_s>().to_json(j);
      break;
    case types::sib26a_v1610:
      j.write_fieldname("sib26a-v1610");
      c.get<sib_type26a_r16_s>().to_json(j);
      break;
    case types::sib27_v1610:
      j.write_fieldname("sib27-v1610");
      c.get<sib_type27_r16_s>().to_json(j);
      break;
    case types::sib28_v1610:
      j.write_fieldname("sib28-v1610");
      c.get<sib_type28_r16_s>().to_json(j);
      break;
    case types::sib29_v1610:
      j.write_fieldname("sib29-v1610");
      c.get<sib_type29_r16_s>().to_json(j);
      break;
    case types::sib30_v1700:
      j.write_fieldname("sib30-v1700");
      c.get<sib_type30_r17_s>().to_json(j);
      break;
    case types::sib31_v1700:
      j.write_fieldname("sib31-v1700");
      c.get<sib_type31_r17_s>().to_json(j);
      break;
    case types::sib32_v1700:
      j.write_fieldname("sib32-v1700");
      c.get<sib_type32_r17_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sib_info_item_c");
  }
  j.end_obj();
}
SRSASN_CODE sib_info_item_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sib2:
      HANDLE_CODE(c.get<sib_type2_s>().pack(bref));
      break;
    case types::sib3:
      HANDLE_CODE(c.get<sib_type3_s>().pack(bref));
      break;
    case types::sib4:
      HANDLE_CODE(c.get<sib_type4_s>().pack(bref));
      break;
    case types::sib5:
      HANDLE_CODE(c.get<sib_type5_s>().pack(bref));
      break;
    case types::sib6:
      HANDLE_CODE(c.get<sib_type6_s>().pack(bref));
      break;
    case types::sib7:
      HANDLE_CODE(c.get<sib_type7_s>().pack(bref));
      break;
    case types::sib8:
      HANDLE_CODE(c.get<sib_type8_s>().pack(bref));
      break;
    case types::sib9:
      HANDLE_CODE(c.get<sib_type9_s>().pack(bref));
      break;
    case types::sib10:
      HANDLE_CODE(c.get<sib_type10_s>().pack(bref));
      break;
    case types::sib11:
      HANDLE_CODE(c.get<sib_type11_s>().pack(bref));
      break;
    case types::sib12_v920: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type12_r9_s>().pack(bref));
    } break;
    case types::sib13_v920: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type13_r9_s>().pack(bref));
    } break;
    case types::sib14_v1130: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type14_r11_s>().pack(bref));
    } break;
    case types::sib15_v1130: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type15_r11_s>().pack(bref));
    } break;
    case types::sib16_v1130: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type16_r11_s>().pack(bref));
    } break;
    case types::sib17_v1250: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type17_r12_s>().pack(bref));
    } break;
    case types::sib18_v1250: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type18_r12_s>().pack(bref));
    } break;
    case types::sib19_v1250: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type19_r12_s>().pack(bref));
    } break;
    case types::sib20_v1310: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type20_r13_s>().pack(bref));
    } break;
    case types::sib21_v1430: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type21_r14_s>().pack(bref));
    } break;
    case types::sib24_v1530: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type24_r15_s>().pack(bref));
    } break;
    case types::sib25_v1530: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type25_r15_s>().pack(bref));
    } break;
    case types::sib26_v1530: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type26_r15_s>().pack(bref));
    } break;
    case types::sib26a_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type26a_r16_s>().pack(bref));
    } break;
    case types::sib27_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type27_r16_s>().pack(bref));
    } break;
    case types::sib28_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type28_r16_s>().pack(bref));
    } break;
    case types::sib29_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type29_r16_s>().pack(bref));
    } break;
    case types::sib30_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type30_r17_s>().pack(bref));
    } break;
    case types::sib31_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type31_r17_s>().pack(bref));
    } break;
    case types::sib32_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type32_r17_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "sib_info_item_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_info_item_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sib2:
      HANDLE_CODE(c.get<sib_type2_s>().unpack(bref));
      break;
    case types::sib3:
      HANDLE_CODE(c.get<sib_type3_s>().unpack(bref));
      break;
    case types::sib4:
      HANDLE_CODE(c.get<sib_type4_s>().unpack(bref));
      break;
    case types::sib5:
      HANDLE_CODE(c.get<sib_type5_s>().unpack(bref));
      break;
    case types::sib6:
      HANDLE_CODE(c.get<sib_type6_s>().unpack(bref));
      break;
    case types::sib7:
      HANDLE_CODE(c.get<sib_type7_s>().unpack(bref));
      break;
    case types::sib8:
      HANDLE_CODE(c.get<sib_type8_s>().unpack(bref));
      break;
    case types::sib9:
      HANDLE_CODE(c.get<sib_type9_s>().unpack(bref));
      break;
    case types::sib10:
      HANDLE_CODE(c.get<sib_type10_s>().unpack(bref));
      break;
    case types::sib11:
      HANDLE_CODE(c.get<sib_type11_s>().unpack(bref));
      break;
    case types::sib12_v920: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type12_r9_s>().unpack(bref));
    } break;
    case types::sib13_v920: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type13_r9_s>().unpack(bref));
    } break;
    case types::sib14_v1130: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type14_r11_s>().unpack(bref));
    } break;
    case types::sib15_v1130: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type15_r11_s>().unpack(bref));
    } break;
    case types::sib16_v1130: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type16_r11_s>().unpack(bref));
    } break;
    case types::sib17_v1250: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type17_r12_s>().unpack(bref));
    } break;
    case types::sib18_v1250: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type18_r12_s>().unpack(bref));
    } break;
    case types::sib19_v1250: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type19_r12_s>().unpack(bref));
    } break;
    case types::sib20_v1310: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type20_r13_s>().unpack(bref));
    } break;
    case types::sib21_v1430: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type21_r14_s>().unpack(bref));
    } break;
    case types::sib24_v1530: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type24_r15_s>().unpack(bref));
    } break;
    case types::sib25_v1530: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type25_r15_s>().unpack(bref));
    } break;
    case types::sib26_v1530: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type26_r15_s>().unpack(bref));
    } break;
    case types::sib26a_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type26a_r16_s>().unpack(bref));
    } break;
    case types::sib27_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type27_r16_s>().unpack(bref));
    } break;
    case types::sib28_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type28_r16_s>().unpack(bref));
    } break;
    case types::sib29_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type29_r16_s>().unpack(bref));
    } break;
    case types::sib30_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type30_r17_s>().unpack(bref));
    } break;
    case types::sib31_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type31_r17_s>().unpack(bref));
    } break;
    case types::sib32_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_type32_r17_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "sib_info_item_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SystemInformation-r8-IEs ::= SEQUENCE
SRSASN_CODE sys_info_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, sib_type_and_info, 1, 32));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(sib_type_and_info, bref, 1, 32));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sys_info_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("sib-TypeAndInfo");
  for (const auto& e1 : sib_type_and_info) {
    e1.to_json(j);
  }
  j.end_array();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformation ::= SEQUENCE
SRSASN_CODE sys_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void sys_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void sys_info_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::sys_info_r8:
      c.destroy<sys_info_r8_ies_s>();
      break;
    case types::crit_exts_future_r15:
      c.destroy<crit_exts_future_r15_c_>();
      break;
    default:
      break;
  }
}
void sys_info_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sys_info_r8:
      c.init<sys_info_r8_ies_s>();
      break;
    case types::crit_exts_future_r15:
      c.init<crit_exts_future_r15_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
  }
}
sys_info_s::crit_exts_c_::crit_exts_c_(const sys_info_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sys_info_r8:
      c.init(other.c.get<sys_info_r8_ies_s>());
      break;
    case types::crit_exts_future_r15:
      c.init(other.c.get<crit_exts_future_r15_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
  }
}
sys_info_s::crit_exts_c_& sys_info_s::crit_exts_c_::operator=(const sys_info_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sys_info_r8:
      c.set(other.c.get<sys_info_r8_ies_s>());
      break;
    case types::crit_exts_future_r15:
      c.set(other.c.get<crit_exts_future_r15_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
  }

  return *this;
}
sys_info_r8_ies_s& sys_info_s::crit_exts_c_::set_sys_info_r8()
{
  set(types::sys_info_r8);
  return c.get<sys_info_r8_ies_s>();
}
sys_info_s::crit_exts_c_::crit_exts_future_r15_c_& sys_info_s::crit_exts_c_::set_crit_exts_future_r15()
{
  set(types::crit_exts_future_r15);
  return c.get<crit_exts_future_r15_c_>();
}
void sys_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sys_info_r8:
      j.write_fieldname("systemInformation-r8");
      c.get<sys_info_r8_ies_s>().to_json(j);
      break;
    case types::crit_exts_future_r15:
      j.write_fieldname("criticalExtensionsFuture-r15");
      c.get<crit_exts_future_r15_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE sys_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sys_info_r8:
      HANDLE_CODE(c.get<sys_info_r8_ies_s>().pack(bref));
      break;
    case types::crit_exts_future_r15:
      HANDLE_CODE(c.get<crit_exts_future_r15_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sys_info_r8:
      HANDLE_CODE(c.get<sys_info_r8_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future_r15:
      HANDLE_CODE(c.get<crit_exts_future_r15_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sys_info_s::crit_exts_c_::crit_exts_future_r15_c_::set(types::options e)
{
  type_ = e;
}
pos_sys_info_r15_ies_s& sys_info_s::crit_exts_c_::crit_exts_future_r15_c_::set_pos_sys_info_r15()
{
  set(types::pos_sys_info_r15);
  return c;
}
void sys_info_s::crit_exts_c_::crit_exts_future_r15_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void sys_info_s::crit_exts_c_::crit_exts_future_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pos_sys_info_r15:
      j.write_fieldname("posSystemInformation-r15");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_::crit_exts_future_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE sys_info_s::crit_exts_c_::crit_exts_future_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pos_sys_info_r15:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_::crit_exts_future_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_s::crit_exts_c_::crit_exts_future_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pos_sys_info_r15:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_::crit_exts_future_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// BCCH-DL-SCH-MessageType ::= CHOICE
void bcch_dl_sch_msg_type_c::set(types::options e)
{
  type_ = e;
}
bcch_dl_sch_msg_type_c::c1_c_& bcch_dl_sch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void bcch_dl_sch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void bcch_dl_sch_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE bcch_dl_sch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void bcch_dl_sch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::sys_info:
      c.destroy<sys_info_s>();
      break;
    case types::sib_type1:
      c.destroy<sib_type1_s>();
      break;
    default:
      break;
  }
}
void bcch_dl_sch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sys_info:
      c.init<sys_info_s>();
      break;
    case types::sib_type1:
      c.init<sib_type1_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }
}
bcch_dl_sch_msg_type_c::c1_c_::c1_c_(const bcch_dl_sch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sys_info:
      c.init(other.c.get<sys_info_s>());
      break;
    case types::sib_type1:
      c.init(other.c.get<sib_type1_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }
}
bcch_dl_sch_msg_type_c::c1_c_& bcch_dl_sch_msg_type_c::c1_c_::operator=(const bcch_dl_sch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sys_info:
      c.set(other.c.get<sys_info_s>());
      break;
    case types::sib_type1:
      c.set(other.c.get<sib_type1_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }

  return *this;
}
sys_info_s& bcch_dl_sch_msg_type_c::c1_c_::set_sys_info()
{
  set(types::sys_info);
  return c.get<sys_info_s>();
}
sib_type1_s& bcch_dl_sch_msg_type_c::c1_c_::set_sib_type1()
{
  set(types::sib_type1);
  return c.get<sib_type1_s>();
}
void bcch_dl_sch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sys_info:
      j.write_fieldname("systemInformation");
      c.get<sys_info_s>().to_json(j);
      break;
    case types::sib_type1:
      j.write_fieldname("systemInformationBlockType1");
      c.get<sib_type1_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE bcch_dl_sch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sys_info:
      HANDLE_CODE(c.get<sys_info_s>().pack(bref));
      break;
    case types::sib_type1:
      HANDLE_CODE(c.get<sib_type1_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sys_info:
      HANDLE_CODE(c.get<sys_info_s>().unpack(bref));
      break;
    case types::sib_type1:
      HANDLE_CODE(c.get<sib_type1_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// BCCH-DL-SCH-Message ::= SEQUENCE
SRSASN_CODE bcch_dl_sch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void bcch_dl_sch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("BCCH-DL-SCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// BCCH-DL-SCH-MessageType-BR-r13 ::= CHOICE
void bcch_dl_sch_msg_type_br_r13_c::set(types::options e)
{
  type_ = e;
}
bcch_dl_sch_msg_type_br_r13_c::c1_c_& bcch_dl_sch_msg_type_br_r13_c::set_c1()
{
  set(types::c1);
  return c;
}
void bcch_dl_sch_msg_type_br_r13_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void bcch_dl_sch_msg_type_br_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_br_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE bcch_dl_sch_msg_type_br_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_br_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_type_br_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_br_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void bcch_dl_sch_msg_type_br_r13_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::sys_info_br_r13:
      c.destroy<sys_info_br_r13_s>();
      break;
    case types::sib_type1_br_r13:
      c.destroy<sib_type1_br_r13_s>();
      break;
    default:
      break;
  }
}
void bcch_dl_sch_msg_type_br_r13_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sys_info_br_r13:
      c.init<sys_info_br_r13_s>();
      break;
    case types::sib_type1_br_r13:
      c.init<sib_type1_br_r13_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_br_r13_c::c1_c_");
  }
}
bcch_dl_sch_msg_type_br_r13_c::c1_c_::c1_c_(const bcch_dl_sch_msg_type_br_r13_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sys_info_br_r13:
      c.init(other.c.get<sys_info_br_r13_s>());
      break;
    case types::sib_type1_br_r13:
      c.init(other.c.get<sib_type1_br_r13_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_br_r13_c::c1_c_");
  }
}
bcch_dl_sch_msg_type_br_r13_c::c1_c_&
bcch_dl_sch_msg_type_br_r13_c::c1_c_::operator=(const bcch_dl_sch_msg_type_br_r13_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sys_info_br_r13:
      c.set(other.c.get<sys_info_br_r13_s>());
      break;
    case types::sib_type1_br_r13:
      c.set(other.c.get<sib_type1_br_r13_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_br_r13_c::c1_c_");
  }

  return *this;
}
sys_info_br_r13_s& bcch_dl_sch_msg_type_br_r13_c::c1_c_::set_sys_info_br_r13()
{
  set(types::sys_info_br_r13);
  return c.get<sys_info_br_r13_s>();
}
sib_type1_br_r13_s& bcch_dl_sch_msg_type_br_r13_c::c1_c_::set_sib_type1_br_r13()
{
  set(types::sib_type1_br_r13);
  return c.get<sib_type1_br_r13_s>();
}
void bcch_dl_sch_msg_type_br_r13_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sys_info_br_r13:
      j.write_fieldname("systemInformation-BR-r13");
      c.get<sys_info_br_r13_s>().to_json(j);
      break;
    case types::sib_type1_br_r13:
      j.write_fieldname("systemInformationBlockType1-BR-r13");
      c.get<sib_type1_br_r13_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_br_r13_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE bcch_dl_sch_msg_type_br_r13_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sys_info_br_r13:
      HANDLE_CODE(c.get<sys_info_br_r13_s>().pack(bref));
      break;
    case types::sib_type1_br_r13:
      HANDLE_CODE(c.get<sib_type1_br_r13_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_br_r13_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_type_br_r13_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sys_info_br_r13:
      HANDLE_CODE(c.get<sys_info_br_r13_s>().unpack(bref));
      break;
    case types::sib_type1_br_r13:
      HANDLE_CODE(c.get<sib_type1_br_r13_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_br_r13_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// BCCH-DL-SCH-Message-BR ::= SEQUENCE
SRSASN_CODE bcch_dl_sch_msg_br_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_br_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void bcch_dl_sch_msg_br_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("BCCH-DL-SCH-Message-BR");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// SIB-Type-MBMS-r14 ::= ENUMERATED
const char* sib_type_mbms_r14_opts::to_string() const
{
  static const char* options[] = {
      "sibType10", "sibType11", "sibType12-v920", "sibType13-v920", "sibType15-v1130", "sibType16-v1130"};
  return convert_enum_idx(options, 6, value, "sib_type_mbms_r14_e");
}
uint8_t sib_type_mbms_r14_opts::to_number() const
{
  static const uint8_t options[] = {10, 11, 12, 13, 15, 16};
  return map_enum_number(options, 6, value, "sib_type_mbms_r14_e");
}

// SchedulingInfo-MBMS-r14 ::= SEQUENCE
SRSASN_CODE sched_info_mbms_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(si_periodicity_r14.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, sib_map_info_r14, 0, 31));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_info_mbms_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(si_periodicity_r14.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(sib_map_info_r14, bref, 0, 31));

  return SRSASN_SUCCESS;
}
void sched_info_mbms_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("si-Periodicity-r14", si_periodicity_r14.to_string());
  j.start_array("sib-MappingInfo-r14");
  for (const auto& e1 : sib_map_info_r14) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

const char* sched_info_mbms_r14_s::si_periodicity_r14_opts::to_string() const
{
  static const char* options[] = {"rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(options, 6, value, "sched_info_mbms_r14_s::si_periodicity_r14_e_");
}
uint16_t sched_info_mbms_r14_s::si_periodicity_r14_opts::to_number() const
{
  static const uint16_t options[] = {16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 6, value, "sched_info_mbms_r14_s::si_periodicity_r14_e_");
}

// NonMBSFN-SubframeConfig-r14 ::= SEQUENCE
SRSASN_CODE non_mbsfn_sf_cfg_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(radio_frame_alloc_period_r14.pack(bref));
  HANDLE_CODE(pack_integer(bref, radio_frame_alloc_offset_r14, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(sf_alloc_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE non_mbsfn_sf_cfg_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(radio_frame_alloc_period_r14.unpack(bref));
  HANDLE_CODE(unpack_integer(radio_frame_alloc_offset_r14, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(sf_alloc_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void non_mbsfn_sf_cfg_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("radioFrameAllocationPeriod-r14", radio_frame_alloc_period_r14.to_string());
  j.write_int("radioFrameAllocationOffset-r14", radio_frame_alloc_offset_r14);
  j.write_str("subframeAllocation-r14", sf_alloc_r14.to_string());
  j.end_obj();
}

const char* non_mbsfn_sf_cfg_r14_s::radio_frame_alloc_period_r14_opts::to_string() const
{
  static const char* options[] = {"rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "rf512"};
  return convert_enum_idx(options, 7, value, "non_mbsfn_sf_cfg_r14_s::radio_frame_alloc_period_r14_e_");
}
uint16_t non_mbsfn_sf_cfg_r14_s::radio_frame_alloc_period_r14_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 512};
  return map_enum_number(options, 7, value, "non_mbsfn_sf_cfg_r14_s::radio_frame_alloc_period_r14_e_");
}

// SystemInformationBlockType1-MBMS-r14 ::= SEQUENCE
SRSASN_CODE sib_type1_mbms_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(multi_band_info_list_r14_present, 1));
  HANDLE_CODE(bref.pack(non_mbsfn_sf_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(sib_type13_r14_present, 1));
  HANDLE_CODE(bref.pack(cell_access_related_info_list_r14_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, cell_access_related_info_r14.plmn_id_list_r14, 1, 6));
  HANDLE_CODE(cell_access_related_info_r14.tac_r14.pack(bref));
  HANDLE_CODE(cell_access_related_info_r14.cell_id_r14.pack(bref));
  HANDLE_CODE(pack_integer(bref, freq_band_ind_r14, (uint16_t)1u, (uint16_t)256u));
  if (multi_band_info_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_r14, 1, 8, integer_packer<uint16_t>(1, 256)));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, sched_info_list_mbms_r14, 1, 32));
  HANDLE_CODE(si_win_len_r14.pack(bref));
  HANDLE_CODE(pack_integer(bref, sys_info_value_tag_r14, (uint8_t)0u, (uint8_t)31u));
  if (non_mbsfn_sf_cfg_r14_present) {
    HANDLE_CODE(non_mbsfn_sf_cfg_r14.pack(bref));
  }
  HANDLE_CODE(pdsch_cfg_common_r14.pack(bref));
  if (sib_type13_r14_present) {
    HANDLE_CODE(sib_type13_r14.pack(bref));
  }
  if (cell_access_related_info_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cell_access_related_info_list_r14, 1, 5));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_mbms_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(multi_band_info_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_mbsfn_sf_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(sib_type13_r14_present, 1));
  HANDLE_CODE(bref.unpack(cell_access_related_info_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(cell_access_related_info_r14.plmn_id_list_r14, bref, 1, 6));
  HANDLE_CODE(cell_access_related_info_r14.tac_r14.unpack(bref));
  HANDLE_CODE(cell_access_related_info_r14.cell_id_r14.unpack(bref));
  HANDLE_CODE(unpack_integer(freq_band_ind_r14, bref, (uint16_t)1u, (uint16_t)256u));
  if (multi_band_info_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_r14, bref, 1, 8, integer_packer<uint16_t>(1, 256)));
  }
  HANDLE_CODE(unpack_dyn_seq_of(sched_info_list_mbms_r14, bref, 1, 32));
  HANDLE_CODE(si_win_len_r14.unpack(bref));
  HANDLE_CODE(unpack_integer(sys_info_value_tag_r14, bref, (uint8_t)0u, (uint8_t)31u));
  if (non_mbsfn_sf_cfg_r14_present) {
    HANDLE_CODE(non_mbsfn_sf_cfg_r14.unpack(bref));
  }
  HANDLE_CODE(pdsch_cfg_common_r14.unpack(bref));
  if (sib_type13_r14_present) {
    HANDLE_CODE(sib_type13_r14.unpack(bref));
  }
  if (cell_access_related_info_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cell_access_related_info_list_r14, bref, 1, 5));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_mbms_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellAccessRelatedInfo-r14");
  j.start_obj();
  j.start_array("plmn-IdentityList-r14");
  for (const auto& e1 : cell_access_related_info_r14.plmn_id_list_r14) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("trackingAreaCode-r14", cell_access_related_info_r14.tac_r14.to_string());
  j.write_str("cellIdentity-r14", cell_access_related_info_r14.cell_id_r14.to_string());
  j.end_obj();
  j.write_int("freqBandIndicator-r14", freq_band_ind_r14);
  if (multi_band_info_list_r14_present) {
    j.start_array("multiBandInfoList-r14");
    for (const auto& e1 : multi_band_info_list_r14) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.start_array("schedulingInfoList-MBMS-r14");
  for (const auto& e1 : sched_info_list_mbms_r14) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("si-WindowLength-r14", si_win_len_r14.to_string());
  j.write_int("systemInfoValueTag-r14", sys_info_value_tag_r14);
  if (non_mbsfn_sf_cfg_r14_present) {
    j.write_fieldname("nonMBSFN-SubframeConfig-r14");
    non_mbsfn_sf_cfg_r14.to_json(j);
  }
  j.write_fieldname("pdsch-ConfigCommon-r14");
  pdsch_cfg_common_r14.to_json(j);
  if (sib_type13_r14_present) {
    j.write_fieldname("systemInformationBlockType13-r14");
    sib_type13_r14.to_json(j);
  }
  if (cell_access_related_info_list_r14_present) {
    j.start_array("cellAccessRelatedInfoList-r14");
    for (const auto& e1 : cell_access_related_info_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* sib_type1_mbms_r14_s::si_win_len_r14_opts::to_string() const
{
  static const char* options[] = {"ms1", "ms2", "ms5", "ms10", "ms15", "ms20", "ms40", "ms80"};
  return convert_enum_idx(options, 8, value, "sib_type1_mbms_r14_s::si_win_len_r14_e_");
}
uint8_t sib_type1_mbms_r14_s::si_win_len_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 5, 10, 15, 20, 40, 80};
  return map_enum_number(options, 8, value, "sib_type1_mbms_r14_s::si_win_len_r14_e_");
}

// CarrierFreqInfoUTRA-FDD-v8h0 ::= SEQUENCE
SRSASN_CODE carrier_freq_info_utra_fdd_v8h0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(multi_band_info_list_present, 1));

  if (multi_band_info_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list, 1, 8, integer_packer<uint8_t>(1, 86)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_info_utra_fdd_v8h0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(multi_band_info_list_present, 1));

  if (multi_band_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list, bref, 1, 8, integer_packer<uint8_t>(1, 86)));
  }

  return SRSASN_SUCCESS;
}
void carrier_freq_info_utra_fdd_v8h0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (multi_band_info_list_present) {
    j.start_array("multiBandInfoList");
    for (const auto& e1 : multi_band_info_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// NS-PmaxValue-v10l0 ::= SEQUENCE
SRSASN_CODE ns_pmax_value_v10l0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(add_spec_emission_v10l0_present, 1));

  if (add_spec_emission_v10l0_present) {
    HANDLE_CODE(pack_integer(bref, add_spec_emission_v10l0, (uint16_t)33u, (uint16_t)288u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ns_pmax_value_v10l0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(add_spec_emission_v10l0_present, 1));

  if (add_spec_emission_v10l0_present) {
    HANDLE_CODE(unpack_integer(add_spec_emission_v10l0, bref, (uint16_t)33u, (uint16_t)288u));
  }

  return SRSASN_SUCCESS;
}
void ns_pmax_value_v10l0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (add_spec_emission_v10l0_present) {
    j.write_int("additionalSpectrumEmission-v10l0", add_spec_emission_v10l0);
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v10l0 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v10l0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_info_v10l0_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_v10l0_present, 1));

  if (freq_band_info_v10l0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_info_v10l0, 1, 8));
  }
  if (multi_band_info_list_v10l0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_v10l0, 1, 8, SeqOfPacker<Packer>(1, 8, Packer())));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v10l0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_band_info_v10l0_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_v10l0_present, 1));

  if (freq_band_info_v10l0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_info_v10l0, bref, 1, 8));
  }
  if (multi_band_info_list_v10l0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_v10l0, bref, 1, 8, SeqOfPacker<Packer>(1, 8, Packer())));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v10l0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_info_v10l0_present) {
    j.start_array("freqBandInfo-v10l0");
    for (const auto& e1 : freq_band_info_v10l0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (multi_band_info_list_v10l0_present) {
    j.start_array("multiBandInfoList-v10l0");
    for (const auto& e1 : multi_band_info_list_v10l0) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v8h0 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v8h0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(multi_band_info_list_present, 1));

  if (multi_band_info_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list, 1, 8, integer_packer<uint8_t>(1, 64)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v8h0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(multi_band_info_list_present, 1));

  if (multi_band_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list, bref, 1, 8, integer_packer<uint8_t>(1, 64)));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v8h0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (multi_band_info_list_present) {
    j.start_array("multiBandInfoList");
    for (const auto& e1 : multi_band_info_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// MultiBandInfo-v9e0 ::= SEQUENCE
SRSASN_CODE multi_band_info_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_ind_v9e0_present, 1));

  if (freq_band_ind_v9e0_present) {
    HANDLE_CODE(pack_integer(bref, freq_band_ind_v9e0, (uint16_t)65u, (uint16_t)256u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE multi_band_info_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_band_ind_v9e0_present, 1));

  if (freq_band_ind_v9e0_present) {
    HANDLE_CODE(unpack_integer(freq_band_ind_v9e0, bref, (uint16_t)65u, (uint16_t)256u));
  }

  return SRSASN_SUCCESS;
}
void multi_band_info_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_ind_v9e0_present) {
    j.write_int("freqBandIndicator-v9e0", freq_band_ind_v9e0);
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v9e0 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dl_carrier_freq_v9e0_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_v9e0_present, 1));

  if (dl_carrier_freq_v9e0_present) {
    HANDLE_CODE(pack_integer(bref, dl_carrier_freq_v9e0, (uint32_t)65536u, (uint32_t)262143u));
  }
  if (multi_band_info_list_v9e0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_v9e0, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dl_carrier_freq_v9e0_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_v9e0_present, 1));

  if (dl_carrier_freq_v9e0_present) {
    HANDLE_CODE(unpack_integer(dl_carrier_freq_v9e0, bref, (uint32_t)65536u, (uint32_t)262143u));
  }
  if (multi_band_info_list_v9e0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_v9e0, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_carrier_freq_v9e0_present) {
    j.write_int("dl-CarrierFreq-v9e0", dl_carrier_freq_v9e0);
  }
  if (multi_band_info_list_v9e0_present) {
    j.start_array("multiBandInfoList-v9e0");
    for (const auto& e1 : multi_band_info_list_v9e0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SIB-Type-v12j0 ::= ENUMERATED
const char* sib_type_v12j0_opts::to_string() const
{
  static const char* options[] = {"sibType19-v1250",
                                  "sibType20-v1310",
                                  "sibType21-v1430",
                                  "sibType24-v1530",
                                  "sibType25-v1530",
                                  "sibType26-v1530",
                                  "sibType26a-v1610",
                                  "sibType27-v1610",
                                  "sibType28-v1610",
                                  "sibType29-v1610",
                                  "sibType30-v1700",
                                  "sibType31-v1700",
                                  "sibType32-v1700",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sib_type_v12j0_e");
}

// SchedulingInfo-v12j0 ::= SEQUENCE
SRSASN_CODE sched_info_v12j0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sib_map_info_v12j0_present, 1));

  if (sib_map_info_v12j0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sib_map_info_v12j0, 1, 31));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_info_v12j0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sib_map_info_v12j0_present, 1));

  if (sib_map_info_v12j0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sib_map_info_v12j0, bref, 1, 31));
  }

  return SRSASN_SUCCESS;
}
void sched_info_v12j0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sib_map_info_v12j0_present) {
    j.start_array("sib-MappingInfo-v12j0");
    for (const auto& e1 : sib_map_info_v12j0) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  j.end_obj();
}

// SchedulingInfoExt-r12 ::= SEQUENCE
SRSASN_CODE sched_info_ext_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(si_periodicity_r12.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, sib_map_info_r12, 1, 31));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_info_ext_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(si_periodicity_r12.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(sib_map_info_r12, bref, 1, 31));

  return SRSASN_SUCCESS;
}
void sched_info_ext_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("si-Periodicity-r12", si_periodicity_r12.to_string());
  j.start_array("sib-MappingInfo-r12");
  for (const auto& e1 : sib_map_info_r12) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// SystemInformationBlockType1-v15g0-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v15g0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(bw_reduced_access_related_info_v15g0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (bw_reduced_access_related_info_v15g0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, bw_reduced_access_related_info_v15g0.pos_sched_info_list_br_r15, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v15g0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(bw_reduced_access_related_info_v15g0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (bw_reduced_access_related_info_v15g0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(bw_reduced_access_related_info_v15g0.pos_sched_info_list_br_r15, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v15g0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (bw_reduced_access_related_info_v15g0_present) {
    j.write_fieldname("bandwidthReducedAccessRelatedInfo-v15g0");
    j.start_obj();
    j.start_array("posSchedulingInfoList-BR-r15");
    for (const auto& e1 : bw_reduced_access_related_info_v15g0.pos_sched_info_list_br_r15) {
      e1.to_json(j);
    }
    j.end_array();
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SystemInformationBlockType1-v12j0-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v12j0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sched_info_list_v12j0_present, 1));
  HANDLE_CODE(bref.pack(sched_info_list_ext_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sched_info_list_v12j0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sched_info_list_v12j0, 1, 32));
  }
  if (sched_info_list_ext_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sched_info_list_ext_r12, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v12j0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sched_info_list_v12j0_present, 1));
  HANDLE_CODE(bref.unpack(sched_info_list_ext_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sched_info_list_v12j0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sched_info_list_v12j0, bref, 1, 32));
  }
  if (sched_info_list_ext_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sched_info_list_ext_r12, bref, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v12j0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sched_info_list_v12j0_present) {
    j.start_array("schedulingInfoList-v12j0");
    for (const auto& e1 : sched_info_list_v12j0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sched_info_list_ext_r12_present) {
    j.start_array("schedulingInfoListExt-r12");
    for (const auto& e1 : sched_info_list_ext_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType1-v10x0-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v10x0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v10x0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v10x0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType1-v10l0-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v10l0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_info_v10l0_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_v10l0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (freq_band_info_v10l0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_info_v10l0, 1, 8));
  }
  if (multi_band_info_list_v10l0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_v10l0, 1, 8, SeqOfPacker<Packer>(1, 8, Packer())));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v10l0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_band_info_v10l0_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_v10l0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (freq_band_info_v10l0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_info_v10l0, bref, 1, 8));
  }
  if (multi_band_info_list_v10l0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_v10l0, bref, 1, 8, SeqOfPacker<Packer>(1, 8, Packer())));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v10l0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_info_v10l0_present) {
    j.start_array("freqBandInfo-v10l0");
    for (const auto& e1 : freq_band_info_v10l0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (multi_band_info_list_v10l0_present) {
    j.start_array("multiBandInfoList-v10l0");
    for (const auto& e1 : multi_band_info_list_v10l0) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType1-v10j0-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v10j0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_info_r10_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_v10j0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (freq_band_info_r10_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_info_r10, 1, 8));
  }
  if (multi_band_info_list_v10j0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_v10j0, 1, 8, SeqOfPacker<Packer>(1, 8, Packer())));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v10j0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_band_info_r10_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_v10j0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (freq_band_info_r10_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_info_r10, bref, 1, 8));
  }
  if (multi_band_info_list_v10j0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_v10j0, bref, 1, 8, SeqOfPacker<Packer>(1, 8, Packer())));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v10j0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_info_r10_present) {
    j.start_array("freqBandInfo-r10");
    for (const auto& e1 : freq_band_info_r10) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (multi_band_info_list_v10j0_present) {
    j.start_array("multiBandInfoList-v10j0");
    for (const auto& e1 : multi_band_info_list_v10j0) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType1-v9e0-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v9e0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_ind_v9e0_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_v9e0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (freq_band_ind_v9e0_present) {
    HANDLE_CODE(pack_integer(bref, freq_band_ind_v9e0, (uint16_t)65u, (uint16_t)256u));
  }
  if (multi_band_info_list_v9e0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_v9e0, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v9e0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_band_ind_v9e0_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_v9e0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (freq_band_ind_v9e0_present) {
    HANDLE_CODE(unpack_integer(freq_band_ind_v9e0, bref, (uint16_t)65u, (uint16_t)256u));
  }
  if (multi_band_info_list_v9e0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_v9e0, bref, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v9e0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_ind_v9e0_present) {
    j.write_int("freqBandIndicator-v9e0", freq_band_ind_v9e0);
  }
  if (multi_band_info_list_v9e0_present) {
    j.start_array("multiBandInfoList-v9e0");
    for (const auto& e1 : multi_band_info_list_v9e0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType1-v8h0-IEs ::= SEQUENCE
SRSASN_CODE sib_type1_v8h0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(multi_band_info_list_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (multi_band_info_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list, 1, 8, integer_packer<uint8_t>(1, 64)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type1_v8h0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(multi_band_info_list_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (multi_band_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list, bref, 1, 8, integer_packer<uint8_t>(1, 64)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type1_v8h0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (multi_band_info_list_present) {
    j.start_array("multiBandInfoList");
    for (const auto& e1 : multi_band_info_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType2-v13c0-IEs ::= SEQUENCE
SRSASN_CODE sib_type2_v13c0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_pwr_ctrl_common_v13c0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_pwr_ctrl_common_v13c0_present) {
    HANDLE_CODE(ul_pwr_ctrl_common_v13c0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type2_v13c0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_pwr_ctrl_common_v13c0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_pwr_ctrl_common_v13c0_present) {
    HANDLE_CODE(ul_pwr_ctrl_common_v13c0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type2_v13c0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_pwr_ctrl_common_v13c0_present) {
    j.write_fieldname("uplinkPowerControlCommon-v13c0");
    ul_pwr_ctrl_common_v13c0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SystemInformationBlockType2-v10n0-IEs ::= SEQUENCE
SRSASN_CODE sib_type2_v10n0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type2_v10n0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type2_v10n0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType2-v10m0-IEs ::= SEQUENCE
SRSASN_CODE sib_type2_v10m0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_info_v10l0_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_v10l0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (freq_info_v10l0_present) {
    HANDLE_CODE(pack_integer(bref, freq_info_v10l0.add_spec_emission_v10l0, (uint16_t)33u, (uint16_t)288u));
  }
  if (multi_band_info_list_v10l0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_v10l0, 1, 8, integer_packer<uint16_t>(33, 288)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type2_v10m0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_info_v10l0_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_v10l0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (freq_info_v10l0_present) {
    HANDLE_CODE(unpack_integer(freq_info_v10l0.add_spec_emission_v10l0, bref, (uint16_t)33u, (uint16_t)288u));
  }
  if (multi_band_info_list_v10l0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_v10l0, bref, 1, 8, integer_packer<uint16_t>(33, 288)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type2_v10m0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_info_v10l0_present) {
    j.write_fieldname("freqInfo-v10l0");
    j.start_obj();
    j.write_int("additionalSpectrumEmission-v10l0", freq_info_v10l0.add_spec_emission_v10l0);
    j.end_obj();
  }
  if (multi_band_info_list_v10l0_present) {
    j.start_array("multiBandInfoList-v10l0");
    for (const auto& e1 : multi_band_info_list_v10l0) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType2-v9i0-IEs ::= SEQUENCE
SRSASN_CODE sib_type2_v9i0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(dummy_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type2_v9i0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(dummy_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type2_v9i0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_crit_ext_present) {
    j.write_str("nonCriticalExtension", non_crit_ext.to_string());
  }
  if (dummy_present) {
    j.write_fieldname("dummy");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SystemInformationBlockType2-v9e0-IEs ::= SEQUENCE
SRSASN_CODE sib_type2_v9e0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_carrier_freq_v9e0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ul_carrier_freq_v9e0_present) {
    HANDLE_CODE(pack_integer(bref, ul_carrier_freq_v9e0, (uint32_t)65536u, (uint32_t)262143u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type2_v9e0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_carrier_freq_v9e0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ul_carrier_freq_v9e0_present) {
    HANDLE_CODE(unpack_integer(ul_carrier_freq_v9e0, bref, (uint32_t)65536u, (uint32_t)262143u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type2_v9e0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_carrier_freq_v9e0_present) {
    j.write_int("ul-CarrierFreq-v9e0", ul_carrier_freq_v9e0);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType2-v8h0-IEs ::= SEQUENCE
SRSASN_CODE sib_type2_v8h0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(multi_band_info_list_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (multi_band_info_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list, 1, 8, integer_packer<uint8_t>(1, 32)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type2_v8h0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(multi_band_info_list_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (multi_band_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list, bref, 1, 8, integer_packer<uint8_t>(1, 32)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type2_v8h0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (multi_band_info_list_present) {
    j.start_array("multiBandInfoList");
    for (const auto& e1 : multi_band_info_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType3-v10l0-IEs ::= SEQUENCE
SRSASN_CODE sib_type3_v10l0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_info_v10l0_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_v10l0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (freq_band_info_v10l0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_info_v10l0, 1, 8));
  }
  if (multi_band_info_list_v10l0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_v10l0, 1, 8, SeqOfPacker<Packer>(1, 8, Packer())));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type3_v10l0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_band_info_v10l0_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_v10l0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (freq_band_info_v10l0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_info_v10l0, bref, 1, 8));
  }
  if (multi_band_info_list_v10l0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_v10l0, bref, 1, 8, SeqOfPacker<Packer>(1, 8, Packer())));
  }

  return SRSASN_SUCCESS;
}
void sib_type3_v10l0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_info_v10l0_present) {
    j.start_array("freqBandInfo-v10l0");
    for (const auto& e1 : freq_band_info_v10l0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (multi_band_info_list_v10l0_present) {
    j.start_array("multiBandInfoList-v10l0");
    for (const auto& e1 : multi_band_info_list_v10l0) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SystemInformationBlockType3-v10j0-IEs ::= SEQUENCE
SRSASN_CODE sib_type3_v10j0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_info_r10_present, 1));
  HANDLE_CODE(bref.pack(multi_band_info_list_v10j0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (freq_band_info_r10_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_info_r10, 1, 8));
  }
  if (multi_band_info_list_v10j0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_band_info_list_v10j0, 1, 8, SeqOfPacker<Packer>(1, 8, Packer())));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type3_v10j0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_band_info_r10_present, 1));
  HANDLE_CODE(bref.unpack(multi_band_info_list_v10j0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (freq_band_info_r10_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_info_r10, bref, 1, 8));
  }
  if (multi_band_info_list_v10j0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_band_info_list_v10j0, bref, 1, 8, SeqOfPacker<Packer>(1, 8, Packer())));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type3_v10j0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_info_r10_present) {
    j.start_array("freqBandInfo-r10");
    for (const auto& e1 : freq_band_info_r10) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (multi_band_info_list_v10j0_present) {
    j.start_array("multiBandInfoList-v10j0");
    for (const auto& e1 : multi_band_info_list_v10j0) {
      j.start_array();
      for (const auto& e2 : e1) {
        e2.to_json(j);
      }
      j.end_array();
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType5-v13a0-IEs ::= SEQUENCE
SRSASN_CODE sib_type5_v13a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v13a0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (inter_freq_carrier_freq_list_v13a0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_carrier_freq_list_v13a0, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type5_v13a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_v13a0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (inter_freq_carrier_freq_list_v13a0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_carrier_freq_list_v13a0, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void sib_type5_v13a0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (inter_freq_carrier_freq_list_v13a0_present) {
    j.start_array("interFreqCarrierFreqList-v13a0");
    for (const auto& e1 : inter_freq_carrier_freq_list_v13a0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SystemInformationBlockType5-v10l0-IEs ::= SEQUENCE
SRSASN_CODE sib_type5_v10l0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v10l0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (inter_freq_carrier_freq_list_v10l0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_carrier_freq_list_v10l0, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type5_v10l0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_v10l0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (inter_freq_carrier_freq_list_v10l0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_carrier_freq_list_v10l0, bref, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type5_v10l0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (inter_freq_carrier_freq_list_v10l0_present) {
    j.start_array("interFreqCarrierFreqList-v10l0");
    for (const auto& e1 : inter_freq_carrier_freq_list_v10l0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType5-v10j0-IEs ::= SEQUENCE
SRSASN_CODE sib_type5_v10j0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v10j0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (inter_freq_carrier_freq_list_v10j0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_carrier_freq_list_v10j0, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type5_v10j0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_v10j0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (inter_freq_carrier_freq_list_v10j0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_carrier_freq_list_v10j0, bref, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type5_v10j0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (inter_freq_carrier_freq_list_v10j0_present) {
    j.start_array("interFreqCarrierFreqList-v10j0");
    for (const auto& e1 : inter_freq_carrier_freq_list_v10j0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType5-v9e0-IEs ::= SEQUENCE
SRSASN_CODE sib_type5_v9e0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v9e0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (inter_freq_carrier_freq_list_v9e0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_carrier_freq_list_v9e0, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type5_v9e0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_v9e0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (inter_freq_carrier_freq_list_v9e0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_carrier_freq_list_v9e0, bref, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type5_v9e0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (inter_freq_carrier_freq_list_v9e0_present) {
    j.start_array("interFreqCarrierFreqList-v9e0");
    for (const auto& e1 : inter_freq_carrier_freq_list_v9e0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType5-v8h0-IEs ::= SEQUENCE
SRSASN_CODE sib_type5_v8h0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v8h0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (inter_freq_carrier_freq_list_v8h0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_carrier_freq_list_v8h0, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type5_v8h0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(inter_freq_carrier_freq_list_v8h0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (inter_freq_carrier_freq_list_v8h0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_carrier_freq_list_v8h0, bref, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_type5_v8h0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (inter_freq_carrier_freq_list_v8h0_present) {
    j.start_array("interFreqCarrierFreqList-v8h0");
    for (const auto& e1 : inter_freq_carrier_freq_list_v8h0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SystemInformationBlockType6-v8h0-IEs ::= SEQUENCE
SRSASN_CODE sib_type6_v8h0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_list_utra_fdd_v8h0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (carrier_freq_list_utra_fdd_v8h0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, carrier_freq_list_utra_fdd_v8h0, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type6_v8h0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_list_utra_fdd_v8h0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (carrier_freq_list_utra_fdd_v8h0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(carrier_freq_list_utra_fdd_v8h0, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void sib_type6_v8h0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_list_utra_fdd_v8h0_present) {
    j.start_array("carrierFreqListUTRA-FDD-v8h0");
    for (const auto& e1 : carrier_freq_list_utra_fdd_v8h0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}
