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

#include "srsran/asn1/rrc/dl_ccch_msg.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// RRCConnectionReject-v1320-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reject_v1320_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rrc_suspend_ind_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reject_v1320_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rrc_suspend_ind_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void rrc_conn_reject_v1320_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rrc_suspend_ind_r13_present) {
    j.write_str("rrc-SuspendIndication-r13", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// BandClassPriority1XRTT ::= SEQUENCE
SRSASN_CODE band_class_prio1_xrtt_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(band_class.pack(bref));
  HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_class_prio1_xrtt_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(band_class.unpack(bref));
  HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void band_class_prio1_xrtt_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("bandClass", band_class.to_string());
  j.write_int("cellReselectionPriority", cell_resel_prio);
  j.end_obj();
}

// BandClassPriorityHRPD ::= SEQUENCE
SRSASN_CODE band_class_prio_hrpd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(band_class.pack(bref));
  HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_class_prio_hrpd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(band_class.unpack(bref));
  HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void band_class_prio_hrpd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("bandClass", band_class.to_string());
  j.write_int("cellReselectionPriority", cell_resel_prio);
  j.end_obj();
}

// FreqPriorityEUTRA ::= SEQUENCE
SRSASN_CODE freq_prio_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_prio_eutra_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void freq_prio_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  j.write_int("cellReselectionPriority", cell_resel_prio);
  j.end_obj();
}

// FreqPriorityEUTRA-r12 ::= SEQUENCE
SRSASN_CODE freq_prio_eutra_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq_r12, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(pack_integer(bref, cell_resel_prio_r12, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_prio_eutra_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq_r12, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(unpack_integer(cell_resel_prio_r12, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void freq_prio_eutra_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r12", carrier_freq_r12);
  j.write_int("cellReselectionPriority-r12", cell_resel_prio_r12);
  j.end_obj();
}

// FreqPriorityEUTRA-v1310 ::= SEQUENCE
SRSASN_CODE freq_prio_eutra_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_resel_sub_prio_r13_present, 1));

  if (cell_resel_sub_prio_r13_present) {
    HANDLE_CODE(cell_resel_sub_prio_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_prio_eutra_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_resel_sub_prio_r13_present, 1));

  if (cell_resel_sub_prio_r13_present) {
    HANDLE_CODE(cell_resel_sub_prio_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void freq_prio_eutra_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_resel_sub_prio_r13_present) {
    j.write_str("cellReselectionSubPriority-r13", cell_resel_sub_prio_r13.to_string());
  }
  j.end_obj();
}

// FreqPriorityNR-r15 ::= SEQUENCE
SRSASN_CODE freq_prio_nr_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_resel_sub_prio_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r15, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_integer(bref, cell_resel_prio_r15, (uint8_t)0u, (uint8_t)7u));
  if (cell_resel_sub_prio_r15_present) {
    HANDLE_CODE(cell_resel_sub_prio_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_prio_nr_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_resel_sub_prio_r15_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r15, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_integer(cell_resel_prio_r15, bref, (uint8_t)0u, (uint8_t)7u));
  if (cell_resel_sub_prio_r15_present) {
    HANDLE_CODE(cell_resel_sub_prio_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void freq_prio_nr_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r15", carrier_freq_r15);
  j.write_int("cellReselectionPriority-r15", cell_resel_prio_r15);
  if (cell_resel_sub_prio_r15_present) {
    j.write_str("cellReselectionSubPriority-r15", cell_resel_sub_prio_r15.to_string());
  }
  j.end_obj();
}

// FreqPriorityUTRA-FDD ::= SEQUENCE
SRSASN_CODE freq_prio_utra_fdd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint16_t)0u, (uint16_t)16383u));
  HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_prio_utra_fdd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint16_t)0u, (uint16_t)16383u));
  HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void freq_prio_utra_fdd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  j.write_int("cellReselectionPriority", cell_resel_prio);
  j.end_obj();
}

// FreqPriorityUTRA-TDD ::= SEQUENCE
SRSASN_CODE freq_prio_utra_tdd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint16_t)0u, (uint16_t)16383u));
  HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_prio_utra_tdd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint16_t)0u, (uint16_t)16383u));
  HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void freq_prio_utra_tdd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  j.write_int("cellReselectionPriority", cell_resel_prio);
  j.end_obj();
}

// FreqsPriorityGERAN ::= SEQUENCE
SRSASN_CODE freqs_prio_geran_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(carrier_freqs.pack(bref));
  HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE freqs_prio_geran_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(carrier_freqs.unpack(bref));
  HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void freqs_prio_geran_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("carrierFreqs");
  carrier_freqs.to_json(j);
  j.write_int("cellReselectionPriority", cell_resel_prio);
  j.end_obj();
}

// RRCConnectionReject-v1130-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reject_v1130_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(depriorit_req_r11_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (depriorit_req_r11_present) {
    HANDLE_CODE(depriorit_req_r11.depriorit_type_r11.pack(bref));
    HANDLE_CODE(depriorit_req_r11.depriorit_timer_r11.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reject_v1130_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(depriorit_req_r11_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (depriorit_req_r11_present) {
    HANDLE_CODE(depriorit_req_r11.depriorit_type_r11.unpack(bref));
    HANDLE_CODE(depriorit_req_r11.depriorit_timer_r11.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reject_v1130_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (depriorit_req_r11_present) {
    j.write_fieldname("deprioritisationReq-r11");
    j.start_obj();
    j.write_str("deprioritisationType-r11", depriorit_req_r11.depriorit_type_r11.to_string());
    j.write_str("deprioritisationTimer-r11", depriorit_req_r11.depriorit_timer_r11.to_string());
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_type_r11_opts::to_string() const
{
  static const char* options[] = {"frequency", "e-utra"};
  return convert_enum_idx(
      options, 2, value, "rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_type_r11_e_");
}

const char* rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_timer_r11_opts::to_string() const
{
  static const char* options[] = {"min5", "min10", "min15", "min30"};
  return convert_enum_idx(
      options, 4, value, "rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_timer_r11_e_");
}
uint8_t rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_timer_r11_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 15, 30};
  return map_enum_number(
      options, 4, value, "rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_timer_r11_e_");
}

// FreqPriorityEUTRA-v9e0 ::= SEQUENCE
SRSASN_CODE freq_prio_eutra_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_v9e0_present, 1));

  if (carrier_freq_v9e0_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_v9e0, (uint32_t)65536u, (uint32_t)262143u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_prio_eutra_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_v9e0_present, 1));

  if (carrier_freq_v9e0_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_v9e0, bref, (uint32_t)65536u, (uint32_t)262143u));
  }

  return SRSASN_SUCCESS;
}
void freq_prio_eutra_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_v9e0_present) {
    j.write_int("carrierFreq-v9e0", carrier_freq_v9e0);
  }
  j.end_obj();
}

// RRCConnectionReject-v1020-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reject_v1020_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(extended_wait_time_r10_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (extended_wait_time_r10_present) {
    HANDLE_CODE(pack_integer(bref, extended_wait_time_r10, (uint16_t)1u, (uint16_t)1800u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reject_v1020_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(extended_wait_time_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (extended_wait_time_r10_present) {
    HANDLE_CODE(unpack_integer(extended_wait_time_r10, bref, (uint16_t)1u, (uint16_t)1800u));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reject_v1020_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (extended_wait_time_r10_present) {
    j.write_int("extendedWaitTime-r10", extended_wait_time_r10);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// IdleModeMobilityControlInfo ::= SEQUENCE
SRSASN_CODE idle_mode_mob_ctrl_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_prio_list_eutra_present, 1));
  HANDLE_CODE(bref.pack(freq_prio_list_geran_present, 1));
  HANDLE_CODE(bref.pack(freq_prio_list_utra_fdd_present, 1));
  HANDLE_CODE(bref.pack(freq_prio_list_utra_tdd_present, 1));
  HANDLE_CODE(bref.pack(band_class_prio_list_hrpd_present, 1));
  HANDLE_CODE(bref.pack(band_class_prio_list1_xrtt_present, 1));
  HANDLE_CODE(bref.pack(t320_present, 1));

  if (freq_prio_list_eutra_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_prio_list_eutra, 1, 8));
  }
  if (freq_prio_list_geran_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_prio_list_geran, 1, 16));
  }
  if (freq_prio_list_utra_fdd_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_prio_list_utra_fdd, 1, 16));
  }
  if (freq_prio_list_utra_tdd_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_prio_list_utra_tdd, 1, 16));
  }
  if (band_class_prio_list_hrpd_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_class_prio_list_hrpd, 1, 32));
  }
  if (band_class_prio_list1_xrtt_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, band_class_prio_list1_xrtt, 1, 32));
  }
  if (t320_present) {
    HANDLE_CODE(t320.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= freq_prio_list_ext_eutra_r12.is_present();
    group_flags[1] |= freq_prio_list_eutra_v1310.is_present();
    group_flags[1] |= freq_prio_list_ext_eutra_v1310.is_present();
    group_flags[2] |= freq_prio_list_nr_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(freq_prio_list_ext_eutra_r12.is_present(), 1));
      if (freq_prio_list_ext_eutra_r12.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *freq_prio_list_ext_eutra_r12, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(freq_prio_list_eutra_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(freq_prio_list_ext_eutra_v1310.is_present(), 1));
      if (freq_prio_list_eutra_v1310.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *freq_prio_list_eutra_v1310, 1, 8));
      }
      if (freq_prio_list_ext_eutra_v1310.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *freq_prio_list_ext_eutra_v1310, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(freq_prio_list_nr_r15.is_present(), 1));
      if (freq_prio_list_nr_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *freq_prio_list_nr_r15, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE idle_mode_mob_ctrl_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(freq_prio_list_eutra_present, 1));
  HANDLE_CODE(bref.unpack(freq_prio_list_geran_present, 1));
  HANDLE_CODE(bref.unpack(freq_prio_list_utra_fdd_present, 1));
  HANDLE_CODE(bref.unpack(freq_prio_list_utra_tdd_present, 1));
  HANDLE_CODE(bref.unpack(band_class_prio_list_hrpd_present, 1));
  HANDLE_CODE(bref.unpack(band_class_prio_list1_xrtt_present, 1));
  HANDLE_CODE(bref.unpack(t320_present, 1));

  if (freq_prio_list_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_prio_list_eutra, bref, 1, 8));
  }
  if (freq_prio_list_geran_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_prio_list_geran, bref, 1, 16));
  }
  if (freq_prio_list_utra_fdd_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_prio_list_utra_fdd, bref, 1, 16));
  }
  if (freq_prio_list_utra_tdd_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_prio_list_utra_tdd, bref, 1, 16));
  }
  if (band_class_prio_list_hrpd_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_class_prio_list_hrpd, bref, 1, 32));
  }
  if (band_class_prio_list1_xrtt_present) {
    HANDLE_CODE(unpack_dyn_seq_of(band_class_prio_list1_xrtt, bref, 1, 32));
  }
  if (t320_present) {
    HANDLE_CODE(t320.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool freq_prio_list_ext_eutra_r12_present;
      HANDLE_CODE(bref.unpack(freq_prio_list_ext_eutra_r12_present, 1));
      freq_prio_list_ext_eutra_r12.set_present(freq_prio_list_ext_eutra_r12_present);
      if (freq_prio_list_ext_eutra_r12.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*freq_prio_list_ext_eutra_r12, bref, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool freq_prio_list_eutra_v1310_present;
      HANDLE_CODE(bref.unpack(freq_prio_list_eutra_v1310_present, 1));
      freq_prio_list_eutra_v1310.set_present(freq_prio_list_eutra_v1310_present);
      bool freq_prio_list_ext_eutra_v1310_present;
      HANDLE_CODE(bref.unpack(freq_prio_list_ext_eutra_v1310_present, 1));
      freq_prio_list_ext_eutra_v1310.set_present(freq_prio_list_ext_eutra_v1310_present);
      if (freq_prio_list_eutra_v1310.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*freq_prio_list_eutra_v1310, bref, 1, 8));
      }
      if (freq_prio_list_ext_eutra_v1310.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*freq_prio_list_ext_eutra_v1310, bref, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool freq_prio_list_nr_r15_present;
      HANDLE_CODE(bref.unpack(freq_prio_list_nr_r15_present, 1));
      freq_prio_list_nr_r15.set_present(freq_prio_list_nr_r15_present);
      if (freq_prio_list_nr_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*freq_prio_list_nr_r15, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void idle_mode_mob_ctrl_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_prio_list_eutra_present) {
    j.start_array("freqPriorityListEUTRA");
    for (const auto& e1 : freq_prio_list_eutra) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (freq_prio_list_geran_present) {
    j.start_array("freqPriorityListGERAN");
    for (const auto& e1 : freq_prio_list_geran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (freq_prio_list_utra_fdd_present) {
    j.start_array("freqPriorityListUTRA-FDD");
    for (const auto& e1 : freq_prio_list_utra_fdd) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (freq_prio_list_utra_tdd_present) {
    j.start_array("freqPriorityListUTRA-TDD");
    for (const auto& e1 : freq_prio_list_utra_tdd) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (band_class_prio_list_hrpd_present) {
    j.start_array("bandClassPriorityListHRPD");
    for (const auto& e1 : band_class_prio_list_hrpd) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (band_class_prio_list1_xrtt_present) {
    j.start_array("bandClassPriorityList1XRTT");
    for (const auto& e1 : band_class_prio_list1_xrtt) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (t320_present) {
    j.write_str("t320", t320.to_string());
  }
  if (ext) {
    if (freq_prio_list_ext_eutra_r12.is_present()) {
      j.start_array("freqPriorityListExtEUTRA-r12");
      for (const auto& e1 : *freq_prio_list_ext_eutra_r12) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (freq_prio_list_eutra_v1310.is_present()) {
      j.start_array("freqPriorityListEUTRA-v1310");
      for (const auto& e1 : *freq_prio_list_eutra_v1310) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (freq_prio_list_ext_eutra_v1310.is_present()) {
      j.start_array("freqPriorityListExtEUTRA-v1310");
      for (const auto& e1 : *freq_prio_list_ext_eutra_v1310) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (freq_prio_list_nr_r15.is_present()) {
      j.start_array("freqPriorityListNR-r15");
      for (const auto& e1 : *freq_prio_list_nr_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* idle_mode_mob_ctrl_info_s::t320_opts::to_string() const
{
  static const char* options[] = {"min5", "min10", "min20", "min30", "min60", "min120", "min180", "spare1"};
  return convert_enum_idx(options, 8, value, "idle_mode_mob_ctrl_info_s::t320_e_");
}
uint8_t idle_mode_mob_ctrl_info_s::t320_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 30, 60, 120, 180};
  return map_enum_number(options, 7, value, "idle_mode_mob_ctrl_info_s::t320_e_");
}

// IdleModeMobilityControlInfo-v9e0 ::= SEQUENCE
SRSASN_CODE idle_mode_mob_ctrl_info_v9e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, freq_prio_list_eutra_v9e0, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE idle_mode_mob_ctrl_info_v9e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(freq_prio_list_eutra_v9e0, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void idle_mode_mob_ctrl_info_v9e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("freqPriorityListEUTRA-v9e0");
  for (const auto& e1 : freq_prio_list_eutra_v9e0) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// RRCConnectionReestablishment-v8a0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_v8a0_ies_s::to_json(json_writer& j) const
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

// RRCConnectionReestablishmentReject-v8a0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_reject_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_reject_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_reject_v8a0_ies_s::to_json(json_writer& j) const
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

// RRCConnectionReject-v8a0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reject_v8a0_ies_s::pack(bit_ref& bref) const
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
SRSASN_CODE rrc_conn_reject_v8a0_ies_s::unpack(cbit_ref& bref)
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
void rrc_conn_reject_v8a0_ies_s::to_json(json_writer& j) const
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

// RRCConnectionSetup-v8a0-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_v8a0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_v8a0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_v8a0_ies_s::to_json(json_writer& j) const
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

// RRCEarlyDataComplete-v1590-IEs ::= SEQUENCE
SRSASN_CODE rrc_early_data_complete_v1590_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_complete_v1590_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_early_data_complete_v1590_ies_s::to_json(json_writer& j) const
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

// RedirectedCarrierInfo-r15-IEs ::= CHOICE
void redirected_carrier_info_r15_ies_c::destroy_()
{
  switch (type_) {
    case types::geran_r15:
      c.destroy<carrier_freqs_geran_s>();
      break;
    case types::cdma2000_hrpd_r15:
      c.destroy<carrier_freq_cdma2000_s>();
      break;
    case types::cdma2000_minus1x_rtt_r15:
      c.destroy<carrier_freq_cdma2000_s>();
      break;
    case types::utra_tdd_r15:
      c.destroy<carrier_freq_list_utra_tdd_r10_l>();
      break;
    default:
      break;
  }
}
void redirected_carrier_info_r15_ies_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::eutra_r15:
      break;
    case types::geran_r15:
      c.init<carrier_freqs_geran_s>();
      break;
    case types::utra_fdd_r15:
      break;
    case types::cdma2000_hrpd_r15:
      c.init<carrier_freq_cdma2000_s>();
      break;
    case types::cdma2000_minus1x_rtt_r15:
      c.init<carrier_freq_cdma2000_s>();
      break;
    case types::utra_tdd_r15:
      c.init<carrier_freq_list_utra_tdd_r10_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_r15_ies_c");
  }
}
redirected_carrier_info_r15_ies_c::redirected_carrier_info_r15_ies_c(const redirected_carrier_info_r15_ies_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::eutra_r15:
      c.init(other.c.get<uint32_t>());
      break;
    case types::geran_r15:
      c.init(other.c.get<carrier_freqs_geran_s>());
      break;
    case types::utra_fdd_r15:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cdma2000_hrpd_r15:
      c.init(other.c.get<carrier_freq_cdma2000_s>());
      break;
    case types::cdma2000_minus1x_rtt_r15:
      c.init(other.c.get<carrier_freq_cdma2000_s>());
      break;
    case types::utra_tdd_r15:
      c.init(other.c.get<carrier_freq_list_utra_tdd_r10_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_r15_ies_c");
  }
}
redirected_carrier_info_r15_ies_c&
redirected_carrier_info_r15_ies_c::operator=(const redirected_carrier_info_r15_ies_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::eutra_r15:
      c.set(other.c.get<uint32_t>());
      break;
    case types::geran_r15:
      c.set(other.c.get<carrier_freqs_geran_s>());
      break;
    case types::utra_fdd_r15:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cdma2000_hrpd_r15:
      c.set(other.c.get<carrier_freq_cdma2000_s>());
      break;
    case types::cdma2000_minus1x_rtt_r15:
      c.set(other.c.get<carrier_freq_cdma2000_s>());
      break;
    case types::utra_tdd_r15:
      c.set(other.c.get<carrier_freq_list_utra_tdd_r10_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_r15_ies_c");
  }

  return *this;
}
uint32_t& redirected_carrier_info_r15_ies_c::set_eutra_r15()
{
  set(types::eutra_r15);
  return c.get<uint32_t>();
}
carrier_freqs_geran_s& redirected_carrier_info_r15_ies_c::set_geran_r15()
{
  set(types::geran_r15);
  return c.get<carrier_freqs_geran_s>();
}
uint16_t& redirected_carrier_info_r15_ies_c::set_utra_fdd_r15()
{
  set(types::utra_fdd_r15);
  return c.get<uint16_t>();
}
carrier_freq_cdma2000_s& redirected_carrier_info_r15_ies_c::set_cdma2000_hrpd_r15()
{
  set(types::cdma2000_hrpd_r15);
  return c.get<carrier_freq_cdma2000_s>();
}
carrier_freq_cdma2000_s& redirected_carrier_info_r15_ies_c::set_cdma2000_minus1x_rtt_r15()
{
  set(types::cdma2000_minus1x_rtt_r15);
  return c.get<carrier_freq_cdma2000_s>();
}
carrier_freq_list_utra_tdd_r10_l& redirected_carrier_info_r15_ies_c::set_utra_tdd_r15()
{
  set(types::utra_tdd_r15);
  return c.get<carrier_freq_list_utra_tdd_r10_l>();
}
void redirected_carrier_info_r15_ies_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::eutra_r15:
      j.write_int("eutra-r15", c.get<uint32_t>());
      break;
    case types::geran_r15:
      j.write_fieldname("geran-r15");
      c.get<carrier_freqs_geran_s>().to_json(j);
      break;
    case types::utra_fdd_r15:
      j.write_int("utra-FDD-r15", c.get<uint16_t>());
      break;
    case types::cdma2000_hrpd_r15:
      j.write_fieldname("cdma2000-HRPD-r15");
      c.get<carrier_freq_cdma2000_s>().to_json(j);
      break;
    case types::cdma2000_minus1x_rtt_r15:
      j.write_fieldname("cdma2000-1xRTT-r15");
      c.get<carrier_freq_cdma2000_s>().to_json(j);
      break;
    case types::utra_tdd_r15:
      j.start_array("utra-TDD-r15");
      for (const auto& e1 : c.get<carrier_freq_list_utra_tdd_r10_l>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_r15_ies_c");
  }
  j.end_obj();
}
SRSASN_CODE redirected_carrier_info_r15_ies_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::eutra_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)262143u));
      break;
    case types::geran_r15:
      HANDLE_CODE(c.get<carrier_freqs_geran_s>().pack(bref));
      break;
    case types::utra_fdd_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)16383u));
      break;
    case types::cdma2000_hrpd_r15:
      HANDLE_CODE(c.get<carrier_freq_cdma2000_s>().pack(bref));
      break;
    case types::cdma2000_minus1x_rtt_r15:
      HANDLE_CODE(c.get<carrier_freq_cdma2000_s>().pack(bref));
      break;
    case types::utra_tdd_r15:
      HANDLE_CODE(
          pack_dyn_seq_of(bref, c.get<carrier_freq_list_utra_tdd_r10_l>(), 1, 6, integer_packer<uint16_t>(0, 16383)));
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_r15_ies_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE redirected_carrier_info_r15_ies_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::eutra_r15:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)262143u));
      break;
    case types::geran_r15:
      HANDLE_CODE(c.get<carrier_freqs_geran_s>().unpack(bref));
      break;
    case types::utra_fdd_r15:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)16383u));
      break;
    case types::cdma2000_hrpd_r15:
      HANDLE_CODE(c.get<carrier_freq_cdma2000_s>().unpack(bref));
      break;
    case types::cdma2000_minus1x_rtt_r15:
      HANDLE_CODE(c.get<carrier_freq_cdma2000_s>().unpack(bref));
      break;
    case types::utra_tdd_r15:
      HANDLE_CODE(
          unpack_dyn_seq_of(c.get<carrier_freq_list_utra_tdd_r10_l>(), bref, 1, 6, integer_packer<uint16_t>(0, 16383)));
      break;
    default:
      log_invalid_choice_id(type_, "redirected_carrier_info_r15_ies_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionReestablishment-r8-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(rr_cfg_ded.pack(bref));
  HANDLE_CODE(pack_integer(bref, next_hop_chaining_count, (uint8_t)0u, (uint8_t)7u));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(rr_cfg_ded.unpack(bref));
  HANDLE_CODE(unpack_integer(next_hop_chaining_count, bref, (uint8_t)0u, (uint8_t)7u));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("radioResourceConfigDedicated");
  rr_cfg_ded.to_json(j);
  j.write_int("nextHopChainingCount", next_hop_chaining_count);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReestablishmentReject-r8-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_reject_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_reject_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_reject_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReject-r8-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reject_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_integer(bref, wait_time, (uint8_t)1u, (uint8_t)16u));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reject_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_integer(wait_time, bref, (uint8_t)1u, (uint8_t)16u));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_reject_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("waitTime", wait_time);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionSetup-r8-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(rr_cfg_ded.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(rr_cfg_ded.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("radioResourceConfigDedicated");
  rr_cfg_ded.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCEarlyDataComplete-r15-IEs ::= SEQUENCE
SRSASN_CODE rrc_early_data_complete_r15_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ded_info_nas_r15_present, 1));
  HANDLE_CODE(bref.pack(extended_wait_time_r15_present, 1));
  HANDLE_CODE(bref.pack(idle_mode_mob_ctrl_info_r15_present, 1));
  HANDLE_CODE(bref.pack(idle_mode_mob_ctrl_info_ext_r15_present, 1));
  HANDLE_CODE(bref.pack(redirected_carrier_info_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ded_info_nas_r15_present) {
    HANDLE_CODE(ded_info_nas_r15.pack(bref));
  }
  if (extended_wait_time_r15_present) {
    HANDLE_CODE(pack_integer(bref, extended_wait_time_r15, (uint16_t)1u, (uint16_t)1800u));
  }
  if (idle_mode_mob_ctrl_info_r15_present) {
    HANDLE_CODE(idle_mode_mob_ctrl_info_r15.pack(bref));
  }
  if (idle_mode_mob_ctrl_info_ext_r15_present) {
    HANDLE_CODE(idle_mode_mob_ctrl_info_ext_r15.pack(bref));
  }
  if (redirected_carrier_info_r15_present) {
    HANDLE_CODE(redirected_carrier_info_r15.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_complete_r15_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ded_info_nas_r15_present, 1));
  HANDLE_CODE(bref.unpack(extended_wait_time_r15_present, 1));
  HANDLE_CODE(bref.unpack(idle_mode_mob_ctrl_info_r15_present, 1));
  HANDLE_CODE(bref.unpack(idle_mode_mob_ctrl_info_ext_r15_present, 1));
  HANDLE_CODE(bref.unpack(redirected_carrier_info_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ded_info_nas_r15_present) {
    HANDLE_CODE(ded_info_nas_r15.unpack(bref));
  }
  if (extended_wait_time_r15_present) {
    HANDLE_CODE(unpack_integer(extended_wait_time_r15, bref, (uint16_t)1u, (uint16_t)1800u));
  }
  if (idle_mode_mob_ctrl_info_r15_present) {
    HANDLE_CODE(idle_mode_mob_ctrl_info_r15.unpack(bref));
  }
  if (idle_mode_mob_ctrl_info_ext_r15_present) {
    HANDLE_CODE(idle_mode_mob_ctrl_info_ext_r15.unpack(bref));
  }
  if (redirected_carrier_info_r15_present) {
    HANDLE_CODE(redirected_carrier_info_r15.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rrc_early_data_complete_r15_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ded_info_nas_r15_present) {
    j.write_str("dedicatedInfoNAS-r15", ded_info_nas_r15.to_string());
  }
  if (extended_wait_time_r15_present) {
    j.write_int("extendedWaitTime-r15", extended_wait_time_r15);
  }
  if (idle_mode_mob_ctrl_info_r15_present) {
    j.write_fieldname("idleModeMobilityControlInfo-r15");
    idle_mode_mob_ctrl_info_r15.to_json(j);
  }
  if (idle_mode_mob_ctrl_info_ext_r15_present) {
    j.write_fieldname("idleModeMobilityControlInfoExt-r15");
    idle_mode_mob_ctrl_info_ext_r15.to_json(j);
  }
  if (redirected_carrier_info_r15_present) {
    j.write_fieldname("redirectedCarrierInfo-r15");
    redirected_carrier_info_r15.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// RRCConnectionReestablishment ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_reest_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_reest_s::crit_exts_c_::c1_c_& rrc_conn_reest_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void rrc_conn_reest_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_conn_reest_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reest_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_reest_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_reest_r8_ies_s& rrc_conn_reest_s::crit_exts_c_::c1_c_::set_rrc_conn_reest_r8()
{
  set(types::rrc_conn_reest_r8);
  return c;
}
void rrc_conn_reest_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void rrc_conn_reest_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void rrc_conn_reest_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void rrc_conn_reest_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void rrc_conn_reest_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void rrc_conn_reest_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void rrc_conn_reest_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void rrc_conn_reest_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reest_r8:
      j.write_fieldname("rrcConnectionReestablishment-r8");
      c.to_json(j);
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reest_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reest_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reest_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionReestablishmentReject ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_reject_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_reject_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_reject_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_reest_reject_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_reest_reject_r8_ies_s& rrc_conn_reest_reject_s::crit_exts_c_::set_rrc_conn_reest_reject_r8()
{
  set(types::rrc_conn_reest_reject_r8);
  return c;
}
void rrc_conn_reest_reject_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_conn_reest_reject_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reest_reject_r8:
      j.write_fieldname("rrcConnectionReestablishmentReject-r8");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_reject_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reest_reject_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reest_reject_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_reject_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_reject_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reest_reject_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_reject_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionReject ::= SEQUENCE
SRSASN_CODE rrc_conn_reject_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reject_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_reject_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_reject_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_reject_s::crit_exts_c_::c1_c_& rrc_conn_reject_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void rrc_conn_reject_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_conn_reject_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reject_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reject_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_reject_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_reject_r8_ies_s& rrc_conn_reject_s::crit_exts_c_::c1_c_::set_rrc_conn_reject_r8()
{
  set(types::rrc_conn_reject_r8);
  return c;
}
void rrc_conn_reject_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void rrc_conn_reject_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void rrc_conn_reject_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void rrc_conn_reject_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reject_r8:
      j.write_fieldname("rrcConnectionReject-r8");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reject_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reject_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reject_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reject_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reject_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionSetup ::= SEQUENCE
SRSASN_CODE rrc_conn_setup_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, rrc_transaction_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(rrc_transaction_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_setup_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rrc-TransactionIdentifier", rrc_transaction_id);
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_setup_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_setup_s::crit_exts_c_::c1_c_& rrc_conn_setup_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void rrc_conn_setup_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_conn_setup_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_setup_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_conn_setup_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
rrc_conn_setup_r8_ies_s& rrc_conn_setup_s::crit_exts_c_::c1_c_::set_rrc_conn_setup_r8()
{
  set(types::rrc_conn_setup_r8);
  return c;
}
void rrc_conn_setup_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void rrc_conn_setup_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void rrc_conn_setup_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void rrc_conn_setup_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void rrc_conn_setup_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void rrc_conn_setup_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void rrc_conn_setup_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void rrc_conn_setup_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_setup_r8:
      j.write_fieldname("rrcConnectionSetup-r8");
      c.to_json(j);
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_setup_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_setup_r8:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_setup_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_setup_r8:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_setup_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCEarlyDataComplete-r15 ::= SEQUENCE
SRSASN_CODE rrc_early_data_complete_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_complete_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_early_data_complete_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_early_data_complete_r15_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_early_data_complete_r15_ies_s& rrc_early_data_complete_r15_s::crit_exts_c_::set_rrc_early_data_complete_r15()
{
  set(types::rrc_early_data_complete_r15);
  return c;
}
void rrc_early_data_complete_r15_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_early_data_complete_r15_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      j.write_fieldname("rrcEarlyDataComplete-r15");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_complete_r15_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_early_data_complete_r15_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_complete_r15_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_complete_r15_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_complete_r15_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DL-CCCH-MessageType ::= CHOICE
void dl_ccch_msg_type_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    case types::msg_class_ext:
      c.destroy<msg_class_ext_c_>();
      break;
    default:
      break;
  }
}
void dl_ccch_msg_type_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::msg_class_ext:
      c.init<msg_class_ext_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c");
  }
}
dl_ccch_msg_type_c::dl_ccch_msg_type_c(const dl_ccch_msg_type_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      c.init(other.c.get<msg_class_ext_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c");
  }
}
dl_ccch_msg_type_c& dl_ccch_msg_type_c::operator=(const dl_ccch_msg_type_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      c.set(other.c.get<msg_class_ext_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c");
  }

  return *this;
}
dl_ccch_msg_type_c::c1_c_& dl_ccch_msg_type_c::set_c1()
{
  set(types::c1);
  return c.get<c1_c_>();
}
dl_ccch_msg_type_c::msg_class_ext_c_& dl_ccch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
  return c.get<msg_class_ext_c_>();
}
void dl_ccch_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::msg_class_ext:
      j.write_fieldname("messageClassExtension");
      c.get<msg_class_ext_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE dl_ccch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::msg_class_ext:
      HANDLE_CODE(c.get<msg_class_ext_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::msg_class_ext:
      HANDLE_CODE(c.get<msg_class_ext_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void dl_ccch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_reest:
      c.destroy<rrc_conn_reest_s>();
      break;
    case types::rrc_conn_reest_reject:
      c.destroy<rrc_conn_reest_reject_s>();
      break;
    case types::rrc_conn_reject:
      c.destroy<rrc_conn_reject_s>();
      break;
    case types::rrc_conn_setup:
      c.destroy<rrc_conn_setup_s>();
      break;
    default:
      break;
  }
}
void dl_ccch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_reest:
      c.init<rrc_conn_reest_s>();
      break;
    case types::rrc_conn_reest_reject:
      c.init<rrc_conn_reest_reject_s>();
      break;
    case types::rrc_conn_reject:
      c.init<rrc_conn_reject_s>();
      break;
    case types::rrc_conn_setup:
      c.init<rrc_conn_setup_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
  }
}
dl_ccch_msg_type_c::c1_c_::c1_c_(const dl_ccch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_reest:
      c.init(other.c.get<rrc_conn_reest_s>());
      break;
    case types::rrc_conn_reest_reject:
      c.init(other.c.get<rrc_conn_reest_reject_s>());
      break;
    case types::rrc_conn_reject:
      c.init(other.c.get<rrc_conn_reject_s>());
      break;
    case types::rrc_conn_setup:
      c.init(other.c.get<rrc_conn_setup_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
  }
}
dl_ccch_msg_type_c::c1_c_& dl_ccch_msg_type_c::c1_c_::operator=(const dl_ccch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_reest:
      c.set(other.c.get<rrc_conn_reest_s>());
      break;
    case types::rrc_conn_reest_reject:
      c.set(other.c.get<rrc_conn_reest_reject_s>());
      break;
    case types::rrc_conn_reject:
      c.set(other.c.get<rrc_conn_reject_s>());
      break;
    case types::rrc_conn_setup:
      c.set(other.c.get<rrc_conn_setup_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
  }

  return *this;
}
rrc_conn_reest_s& dl_ccch_msg_type_c::c1_c_::set_rrc_conn_reest()
{
  set(types::rrc_conn_reest);
  return c.get<rrc_conn_reest_s>();
}
rrc_conn_reest_reject_s& dl_ccch_msg_type_c::c1_c_::set_rrc_conn_reest_reject()
{
  set(types::rrc_conn_reest_reject);
  return c.get<rrc_conn_reest_reject_s>();
}
rrc_conn_reject_s& dl_ccch_msg_type_c::c1_c_::set_rrc_conn_reject()
{
  set(types::rrc_conn_reject);
  return c.get<rrc_conn_reject_s>();
}
rrc_conn_setup_s& dl_ccch_msg_type_c::c1_c_::set_rrc_conn_setup()
{
  set(types::rrc_conn_setup);
  return c.get<rrc_conn_setup_s>();
}
void dl_ccch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reest:
      j.write_fieldname("rrcConnectionReestablishment");
      c.get<rrc_conn_reest_s>().to_json(j);
      break;
    case types::rrc_conn_reest_reject:
      j.write_fieldname("rrcConnectionReestablishmentReject");
      c.get<rrc_conn_reest_reject_s>().to_json(j);
      break;
    case types::rrc_conn_reject:
      j.write_fieldname("rrcConnectionReject");
      c.get<rrc_conn_reject_s>().to_json(j);
      break;
    case types::rrc_conn_setup:
      j.write_fieldname("rrcConnectionSetup");
      c.get<rrc_conn_setup_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_ccch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reest:
      HANDLE_CODE(c.get<rrc_conn_reest_s>().pack(bref));
      break;
    case types::rrc_conn_reest_reject:
      HANDLE_CODE(c.get<rrc_conn_reest_reject_s>().pack(bref));
      break;
    case types::rrc_conn_reject:
      HANDLE_CODE(c.get<rrc_conn_reject_s>().pack(bref));
      break;
    case types::rrc_conn_setup:
      HANDLE_CODE(c.get<rrc_conn_setup_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reest:
      HANDLE_CODE(c.get<rrc_conn_reest_s>().unpack(bref));
      break;
    case types::rrc_conn_reest_reject:
      HANDLE_CODE(c.get<rrc_conn_reest_reject_s>().unpack(bref));
      break;
    case types::rrc_conn_reject:
      HANDLE_CODE(c.get<rrc_conn_reject_s>().unpack(bref));
      break;
    case types::rrc_conn_setup:
      HANDLE_CODE(c.get<rrc_conn_setup_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void dl_ccch_msg_type_c::msg_class_ext_c_::set(types::options e)
{
  type_ = e;
}
dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_& dl_ccch_msg_type_c::msg_class_ext_c_::set_c2()
{
  set(types::c2);
  return c;
}
void dl_ccch_msg_type_c::msg_class_ext_c_::set_msg_class_ext_future_r15()
{
  set(types::msg_class_ext_future_r15);
}
void dl_ccch_msg_type_c::msg_class_ext_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c2:
      j.write_fieldname("c2");
      c.to_json(j);
      break;
    case types::msg_class_ext_future_r15:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::msg_class_ext_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_ccch_msg_type_c::msg_class_ext_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c2:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext_future_r15:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::msg_class_ext_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_type_c::msg_class_ext_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c2:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext_future_r15:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::msg_class_ext_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::set(types::options e)
{
  type_ = e;
}
rrc_early_data_complete_r15_s& dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::set_rrc_early_data_complete_r15()
{
  set(types::rrc_early_data_complete_r15);
  return c;
}
void dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare3()
{
  set(types::spare3);
}
void dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare2()
{
  set(types::spare2);
}
void dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare1()
{
  set(types::spare1);
}
void dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      j.write_fieldname("rrcEarlyDataComplete-r15");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_");
  }
  j.end_obj();
}
SRSASN_CODE dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_early_data_complete_r15:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DL-CCCH-Message ::= SEQUENCE
SRSASN_CODE dl_ccch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_ccch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void dl_ccch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("DL-CCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
