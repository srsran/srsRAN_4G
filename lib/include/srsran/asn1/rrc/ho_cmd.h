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

/*******************************************************************************
 *
 *                      3GPP TS ASN1 RRC v17.4.0 (2023-03)
 *
 ******************************************************************************/

#ifndef SRSASN1_RRC_HO_CMD_H
#define SRSASN1_RRC_HO_CMD_H

#include "dl_dcch_msg.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// SCG-Config-v13c0-IEs ::= SEQUENCE
struct scg_cfg_v13c0_ies_s {
  bool                     scg_radio_cfg_v13c0_present = false;
  bool                     non_crit_ext_present        = false;
  scg_cfg_part_scg_v13c0_s scg_radio_cfg_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-Config-v12i0a-IEs ::= SEQUENCE
struct scg_cfg_v12i0a_ies_s {
  bool                late_non_crit_ext_present = false;
  bool                non_crit_ext_present      = false;
  dyn_octstring       late_non_crit_ext;
  scg_cfg_v13c0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-Config-r12-IEs ::= SEQUENCE
struct scg_cfg_r12_ies_s {
  bool                   scg_radio_cfg_r12_present = false;
  bool                   non_crit_ext_present      = false;
  scg_cfg_part_scg_r12_s scg_radio_cfg_r12;
  scg_cfg_v12i0a_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Config-v1550 ::= SEQUENCE
struct as_cfg_v1550_s {
  struct tdm_pattern_cfg_r15_s_ {
    sf_assign_r15_e sf_assign_r15;
    uint8_t         harq_offset_r15 = 0;
  };

  // member variables
  bool                   tdm_pattern_cfg_r15_present = false;
  bool                   p_max_eutra_r15_present     = false;
  tdm_pattern_cfg_r15_s_ tdm_pattern_cfg_r15;
  int8_t                 p_max_eutra_r15 = -30;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-ConfigNR-r15 ::= SEQUENCE
struct as_cfg_nr_r15_s {
  bool          source_rb_cfg_nr_r15_present       = false;
  bool          source_rb_cfg_sn_nr_r15_present    = false;
  bool          source_other_cfg_sn_nr_r15_present = false;
  dyn_octstring source_rb_cfg_nr_r15;
  dyn_octstring source_rb_cfg_sn_nr_r15;
  dyn_octstring source_other_cfg_sn_nr_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-ConfigNR-v1570 ::= SEQUENCE
struct as_cfg_nr_v1570_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-ConfigNR-v1620 ::= SEQUENCE
struct as_cfg_nr_v1620_s {
  tdm_pattern_cfg_r15_c tdm_pattern_cfg2_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-Config-r12 ::= SEQUENCE
struct scg_cfg_r12_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { scg_cfg_r12, spare7, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      scg_cfg_r12_ies_s& scg_cfg_r12()
      {
        assert_choice_type(types::scg_cfg_r12, type_, "c1");
        return c;
      }
      const scg_cfg_r12_ies_s& scg_cfg_r12() const
      {
        assert_choice_type(types::scg_cfg_r12, type_, "c1");
        return c;
      }
      scg_cfg_r12_ies_s& set_scg_cfg_r12();
      void               set_spare7();
      void               set_spare6();
      void               set_spare5();
      void               set_spare4();
      void               set_spare3();
      void               set_spare2();
      void               set_spare1();

    private:
      types             type_;
      scg_cfg_r12_ies_s c;
    };
    using types = c1_or_crit_ext_e;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    const c1_c_& c1() const
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    c1_c_& set_c1();
    void   set_crit_exts_future();

  private:
    types type_;
    c1_c_ c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Config ::= SEQUENCE
struct as_cfg_s {
  bool                     ext = false;
  meas_cfg_s               source_meas_cfg;
  rr_cfg_ded_s             source_rr_cfg;
  security_algorithm_cfg_s source_security_algorithm_cfg;
  fixed_bitstring<16>      source_ue_id;
  mib_s                    source_mib;
  sib_type1_s              source_sib_type1;
  sib_type2_s              source_sib_type2;
  ant_info_common_s        ant_info_common;
  uint32_t                 source_dl_carrier_freq = 0;
  // ...
  // group 0
  bool                     source_sib_type1_ext_present = false;
  dyn_octstring            source_sib_type1_ext;
  copy_ptr<other_cfg_r9_s> source_other_cfg_r9;
  // group 1
  copy_ptr<scell_to_add_mod_list_r10_l> source_scell_cfg_list_r10;
  // group 2
  copy_ptr<scg_cfg_r12_s> source_cfg_scg_r12;
  // group 3
  copy_ptr<as_cfg_nr_r15_s> as_cfg_nr_r15;
  // group 4
  copy_ptr<as_cfg_v1550_s> as_cfg_v1550;
  // group 5
  copy_ptr<as_cfg_nr_v1570_s> as_cfg_nr_v1570;
  // group 6
  copy_ptr<as_cfg_nr_v1620_s> as_cfg_nr_v1620;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Config-v10j0 ::= SEQUENCE
struct as_cfg_v10j0_s {
  bool                 ant_info_ded_pcell_v10i0_present = false;
  ant_info_ded_v10i0_s ant_info_ded_pcell_v10i0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Config-v1250 ::= SEQUENCE
struct as_cfg_v1250_s {
  bool                   source_wlan_offload_cfg_r12_present = false;
  bool                   source_sl_comm_cfg_r12_present      = false;
  bool                   source_sl_disc_cfg_r12_present      = false;
  wlan_offload_cfg_r12_s source_wlan_offload_cfg_r12;
  sl_comm_cfg_r12_s      source_sl_comm_cfg_r12;
  sl_disc_cfg_r12_s      source_sl_disc_cfg_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Config-v1320 ::= SEQUENCE
struct as_cfg_v1320_s {
  bool                            source_scell_cfg_list_r13_present = false;
  bool                            source_rclwi_cfg_r13_present      = false;
  scell_to_add_mod_list_ext_r13_l source_scell_cfg_list_r13;
  rclwi_cfg_r13_c                 source_rclwi_cfg_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Config-v13c0 ::= SEQUENCE
struct as_cfg_v13c0_s {
  bool                              rr_cfg_ded_v13c01_present               = false;
  bool                              rr_cfg_ded_v13c02_present               = false;
  bool                              scell_to_add_mod_list_v13c0_present     = false;
  bool                              scell_to_add_mod_list_ext_v13c0_present = false;
  rr_cfg_ded_v1370_s                rr_cfg_ded_v13c01;
  rr_cfg_ded_v13c0_s                rr_cfg_ded_v13c02;
  scell_to_add_mod_list_v13c0_l     scell_to_add_mod_list_v13c0;
  scell_to_add_mod_list_ext_v13c0_l scell_to_add_mod_list_ext_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Config-v1430 ::= SEQUENCE
struct as_cfg_v1430_s {
  bool                        source_sl_v2x_comm_cfg_r14_present  = false;
  bool                        source_lwa_cfg_r14_present          = false;
  bool                        source_wlan_meas_result_r14_present = false;
  sl_v2x_cfg_ded_r14_s        source_sl_v2x_comm_cfg_r14;
  lwa_cfg_r13_s               source_lwa_cfg_r14;
  meas_result_list_wlan_r13_l source_wlan_meas_result_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Config-v9e0 ::= SEQUENCE
struct as_cfg_v9e0_s {
  uint32_t source_dl_carrier_freq_v9e0 = 65536;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AdditionalReestabInfo ::= SEQUENCE
struct add_reestab_info_s {
  fixed_bitstring<28>  cell_id;
  fixed_bitstring<256> key_e_node_b_star;
  fixed_bitstring<16>  short_mac_i;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AdditionalReestabInfoList ::= SEQUENCE (SIZE (1..32)) OF AdditionalReestabInfo
using add_reestab_info_list_l = dyn_array<add_reestab_info_s>;

// ReestablishmentInfo ::= SEQUENCE
struct reest_info_s {
  bool                    ext                           = false;
  bool                    add_reestab_info_list_present = false;
  uint16_t                source_pci                    = 0;
  fixed_bitstring<16>     target_cell_short_mac_i;
  add_reestab_info_list_l add_reestab_info_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Context ::= SEQUENCE
struct as_context_s {
  bool         reest_info_present = false;
  reest_info_s reest_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Context-v1130 ::= SEQUENCE
struct as_context_v1130_s {
  bool          ext                           = false;
  bool          idc_ind_r11_present           = false;
  bool          mbms_interest_ind_r11_present = false;
  bool          ue_assist_info_r11_present    = false;
  dyn_octstring idc_ind_r11;
  dyn_octstring mbms_interest_ind_r11;
  dyn_octstring ue_assist_info_r11;
  // ...
  // group 0
  bool          sidelink_ue_info_r12_present = false;
  dyn_octstring sidelink_ue_info_r12;
  // group 1
  bool          source_context_en_dc_r15_present = false;
  dyn_octstring source_context_en_dc_r15;
  // group 2
  bool          selband_combination_info_en_dc_v1540_present = false;
  dyn_octstring selband_combination_info_en_dc_v1540;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Context-v1320 ::= SEQUENCE
struct as_context_v1320_s {
  bool          wlan_conn_status_report_r13_present = false;
  dyn_octstring wlan_conn_status_report_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConfigRestrictInfoDAPS-r16 ::= SEQUENCE
struct cfg_restrict_info_daps_r16_s {
  bool    max_sch_tb_bits_dl_r16_present = false;
  bool    max_sch_tb_bits_ul_r16_present = false;
  uint8_t max_sch_tb_bits_dl_r16         = 1;
  uint8_t max_sch_tb_bits_ul_r16         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Context-v1610 ::= SEQUENCE
struct as_context_v1610_s {
  bool                         sidelink_ue_info_nr_r16_present    = false;
  bool                         ue_assist_info_nr_r16_present      = false;
  bool                         cfg_restrict_info_daps_r16_present = false;
  dyn_octstring                sidelink_ue_info_nr_r16;
  dyn_octstring                ue_assist_info_nr_r16;
  cfg_restrict_info_daps_r16_s cfg_restrict_info_daps_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConfigRestrictInfoDAPS-v1630 ::= SEQUENCE
struct cfg_restrict_info_daps_v1630_s {
  bool                             daps_pwr_coordination_info_r16_present = false;
  daps_pwr_coordination_info_r16_s daps_pwr_coordination_info_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Context-v1630 ::= SEQUENCE
struct as_context_v1630_s {
  bool                           cfg_restrict_info_daps_v1630_present = false;
  cfg_restrict_info_daps_v1630_s cfg_restrict_info_daps_v1630;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateCellInfo-r10 ::= SEQUENCE
struct candidate_cell_info_r10_s {
  bool     ext                     = false;
  bool     rsrp_result_r10_present = false;
  bool     rsrq_result_r10_present = false;
  uint16_t pci_r10                 = 0;
  uint32_t dl_carrier_freq_r10     = 0;
  uint8_t  rsrp_result_r10         = 0;
  uint8_t  rsrq_result_r10         = 0;
  // ...
  // group 0
  bool     dl_carrier_freq_v1090_present = false;
  uint32_t dl_carrier_freq_v1090         = 65536;
  // group 1
  bool   rsrq_result_v1250_present = false;
  int8_t rsrq_result_v1250         = -30;
  // group 2
  bool    rs_sinr_result_r13_present = false;
  uint8_t rs_sinr_result_r13         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateCellInfoList-r10 ::= SEQUENCE (SIZE (1..8)) OF CandidateCellInfo-r10
using candidate_cell_info_list_r10_l = dyn_array<candidate_cell_info_r10_s>;

// HandoverCommand-r8-IEs ::= SEQUENCE
struct ho_cmd_r8_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ho_cmd_msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCommand ::= SEQUENCE
struct ho_cmd_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ho_cmd_r8, spare7, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      ho_cmd_r8_ies_s& ho_cmd_r8()
      {
        assert_choice_type(types::ho_cmd_r8, type_, "c1");
        return c;
      }
      const ho_cmd_r8_ies_s& ho_cmd_r8() const
      {
        assert_choice_type(types::ho_cmd_r8, type_, "c1");
        return c;
      }
      ho_cmd_r8_ies_s& set_ho_cmd_r8();
      void             set_spare7();
      void             set_spare6();
      void             set_spare5();
      void             set_spare4();
      void             set_spare3();
      void             set_spare2();
      void             set_spare1();

    private:
      types           type_;
      ho_cmd_r8_ies_s c;
    };
    using types = c1_or_crit_ext_e;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    const c1_c_& c1() const
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    c1_c_& set_c1();
    void   set_crit_exts_future();

  private:
    types type_;
    c1_c_ c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Config-v1700 ::= SEQUENCE
struct as_cfg_v1700_s {
  bool scg_state_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v1700-IEs ::= SEQUENCE
struct ho_prep_info_v1700_ies_s {
  bool           as_cfg_v1700_present = false;
  bool           non_crit_ext_present = false;
  as_cfg_v1700_s as_cfg_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Context-v1620 ::= SEQUENCE
struct as_context_v1620_s {
  bool          ue_assist_info_nr_scg_r16_present = false;
  dyn_octstring ue_assist_info_nr_scg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v1630-IEs ::= SEQUENCE
struct ho_prep_info_v1630_ies_s {
  bool                     as_context_v1630_present = false;
  bool                     non_crit_ext_present     = false;
  as_context_v1630_s       as_context_v1630;
  ho_prep_info_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v1620-IEs ::= SEQUENCE
struct ho_prep_info_v1620_ies_s {
  bool                     as_context_v1620_present = false;
  bool                     non_crit_ext_present     = false;
  as_context_v1620_s       as_context_v1620;
  ho_prep_info_v1630_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v1610-IEs ::= SEQUENCE
struct ho_prep_info_v1610_ies_s {
  bool                     as_context_v1610_present = false;
  bool                     non_crit_ext_present     = false;
  as_context_v1610_s       as_context_v1610;
  ho_prep_info_v1620_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v1540-IEs ::= SEQUENCE
struct ho_prep_info_v1540_ies_s {
  bool                     source_rb_cfg_intra5_gc_r15_present = false;
  bool                     non_crit_ext_present                = false;
  dyn_octstring            source_rb_cfg_intra5_gc_r15;
  ho_prep_info_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v1530-IEs ::= SEQUENCE
struct ho_prep_info_v1530_ies_s {
  bool                      ran_notif_area_info_r15_present = false;
  bool                      non_crit_ext_present            = false;
  ran_notif_area_info_r15_c ran_notif_area_info_r15;
  ho_prep_info_v1540_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v1430-IEs ::= SEQUENCE
struct ho_prep_info_v1430_ies_s {
  bool                     as_cfg_v1430_present              = false;
  bool                     make_before_break_req_r14_present = false;
  bool                     non_crit_ext_present              = false;
  as_cfg_v1430_s           as_cfg_v1430;
  ho_prep_info_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v1320-IEs ::= SEQUENCE
struct ho_prep_info_v1320_ies_s {
  bool                     as_cfg_v1320_present     = false;
  bool                     as_context_v1320_present = false;
  bool                     non_crit_ext_present     = false;
  as_cfg_v1320_s           as_cfg_v1320;
  as_context_v1320_s       as_context_v1320;
  ho_prep_info_v1430_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v1250-IEs ::= SEQUENCE
struct ho_prep_info_v1250_ies_s {
  bool                     ue_supported_earfcn_r12_present = false;
  bool                     as_cfg_v1250_present            = false;
  bool                     non_crit_ext_present            = false;
  uint32_t                 ue_supported_earfcn_r12         = 0;
  as_cfg_v1250_s           as_cfg_v1250;
  ho_prep_info_v1320_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v1130-IEs ::= SEQUENCE
struct ho_prep_info_v1130_ies_s {
  bool                     as_context_v1130_present = false;
  bool                     non_crit_ext_present     = false;
  as_context_v1130_s       as_context_v1130;
  ho_prep_info_v1250_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v9e0-IEs ::= SEQUENCE
struct ho_prep_info_v9e0_ies_s {
  bool                     as_cfg_v9e0_present  = false;
  bool                     non_crit_ext_present = false;
  as_cfg_v9e0_s            as_cfg_v9e0;
  ho_prep_info_v1130_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v9d0-IEs ::= SEQUENCE
struct ho_prep_info_v9d0_ies_s {
  bool                    late_non_crit_ext_present = false;
  bool                    non_crit_ext_present      = false;
  dyn_octstring           late_non_crit_ext;
  ho_prep_info_v9e0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v920-IEs ::= SEQUENCE
struct ho_prep_info_v920_ies_s {
  struct ue_cfg_release_r9_opts {
    enum options {
      rel9,
      rel10,
      rel11,
      rel12,
      v10j0,
      v11e0,
      v1280,
      rel13,
      /*...*/ rel14,
      rel15,
      rel16,
      rel17,
      nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<ue_cfg_release_r9_opts, true, 4> ue_cfg_release_r9_e_;

  // member variables
  bool                    ue_cfg_release_r9_present = false;
  bool                    non_crit_ext_present      = false;
  ue_cfg_release_r9_e_    ue_cfg_release_r9;
  ho_prep_info_v9d0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRM-Config ::= SEQUENCE
struct rrm_cfg_s {
  struct ue_inactive_time_opts {
    enum options {
      s1,
      s2,
      s3,
      s5,
      s7,
      s10,
      s15,
      s20,
      s25,
      s30,
      s40,
      s50,
      min1,
      min1s20c,
      min1s40,
      min2,
      min2s30,
      min3,
      min3s30,
      min4,
      min5,
      min6,
      min7,
      min8,
      min9,
      min10,
      min12,
      min14,
      min17,
      min20,
      min24,
      min28,
      min33,
      min38,
      min44,
      min50,
      hr1,
      hr1min30,
      hr2,
      hr2min30,
      hr3,
      hr3min30,
      hr4,
      hr5,
      hr6,
      hr8,
      hr10,
      hr13,
      hr16,
      hr20,
      day1,
      day1hr12,
      day2,
      day2hr12,
      day3,
      day4,
      day5,
      day7,
      day10,
      day14,
      day19,
      day24,
      day30,
      day_more_than30,
      nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<ue_inactive_time_opts> ue_inactive_time_e_;

  // member variables
  bool                ext                      = false;
  bool                ue_inactive_time_present = false;
  ue_inactive_time_e_ ue_inactive_time;
  // ...
  // group 0
  copy_ptr<candidate_cell_info_list_r10_l> candidate_cell_info_list_r10;
  // group 1
  copy_ptr<meas_result_serv_freq_list_nr_r15_l> candidate_cell_info_list_nr_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-r8-IEs ::= SEQUENCE
struct ho_prep_info_r8_ies_s {
  bool                        as_cfg_present       = false;
  bool                        rrm_cfg_present      = false;
  bool                        as_context_present   = false;
  bool                        non_crit_ext_present = false;
  ue_cap_rat_container_list_l ue_radio_access_cap_info;
  as_cfg_s                    as_cfg;
  rrm_cfg_s                   rrm_cfg;
  as_context_s                as_context;
  ho_prep_info_v920_ies_s     non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation ::= SEQUENCE
struct ho_prep_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ho_prep_info_r8, spare7, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      ho_prep_info_r8_ies_s& ho_prep_info_r8()
      {
        assert_choice_type(types::ho_prep_info_r8, type_, "c1");
        return c;
      }
      const ho_prep_info_r8_ies_s& ho_prep_info_r8() const
      {
        assert_choice_type(types::ho_prep_info_r8, type_, "c1");
        return c;
      }
      ho_prep_info_r8_ies_s& set_ho_prep_info_r8();
      void                   set_spare7();
      void                   set_spare6();
      void                   set_spare5();
      void                   set_spare4();
      void                   set_spare3();
      void                   set_spare2();
      void                   set_spare1();

    private:
      types                 type_;
      ho_prep_info_r8_ies_s c;
    };
    using types = c1_or_crit_ext_e;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    const c1_c_& c1() const
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    c1_c_& set_c1();
    void   set_crit_exts_future();

  private:
    types type_;
    c1_c_ c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v13c0-IEs ::= SEQUENCE
struct ho_prep_info_v13c0_ies_s {
  bool           as_cfg_v13c0_present = false;
  bool           non_crit_ext_present = false;
  as_cfg_v13c0_s as_cfg_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v10x0-IEs ::= SEQUENCE
struct ho_prep_info_v10x0_ies_s {
  bool                     late_non_crit_ext_present = false;
  bool                     non_crit_ext_present      = false;
  dyn_octstring            late_non_crit_ext;
  ho_prep_info_v13c0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v10j0-IEs ::= SEQUENCE
struct ho_prep_info_v10j0_ies_s {
  bool                     as_cfg_v10j0_present = false;
  bool                     non_crit_ext_present = false;
  as_cfg_v10j0_s           as_cfg_v10j0;
  ho_prep_info_v10x0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-v9j0-IEs ::= SEQUENCE
struct ho_prep_info_v9j0_ies_s {
  bool                     late_non_crit_ext_present = false;
  bool                     non_crit_ext_present      = false;
  dyn_octstring            late_non_crit_ext;
  ho_prep_info_v10j0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasConfig ::= SEQUENCE
struct var_meas_cfg_s {
  struct speed_state_pars_c_ {
    struct setup_s_ {
      mob_state_params_s          mob_state_params;
      speed_state_scale_factors_s time_to_trigger_sf;
    };
    using types = setup_e;

    // choice methods
    speed_state_pars_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "speedStatePars");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "speedStatePars");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                                meas_id_list_present            = false;
  bool                                meas_id_list_ext_r12_present    = false;
  bool                                meas_id_list_v1310_present      = false;
  bool                                meas_id_list_ext_v1310_present  = false;
  bool                                meas_obj_list_present           = false;
  bool                                meas_obj_list_ext_r13_present   = false;
  bool                                meas_obj_list_v9i0_present      = false;
  bool                                report_cfg_list_present         = false;
  bool                                quant_cfg_present               = false;
  bool                                meas_scale_factor_r12_present   = false;
  bool                                s_measure_present               = false;
  bool                                speed_state_pars_present        = false;
  bool                                allow_interruptions_r11_present = false;
  meas_id_to_add_mod_list_l           meas_id_list;
  meas_id_to_add_mod_list_ext_r12_l   meas_id_list_ext_r12;
  meas_id_to_add_mod_list_v1310_l     meas_id_list_v1310;
  meas_id_to_add_mod_list_ext_v1310_l meas_id_list_ext_v1310;
  meas_obj_to_add_mod_list_l          meas_obj_list;
  meas_obj_to_add_mod_list_ext_r13_l  meas_obj_list_ext_r13;
  meas_obj_to_add_mod_list_v9e0_l     meas_obj_list_v9i0;
  report_cfg_to_add_mod_list_l        report_cfg_list;
  quant_cfg_s                         quant_cfg;
  meas_scale_factor_r12_e             meas_scale_factor_r12;
  int16_t                             s_measure = -140;
  speed_state_pars_c_                 speed_state_pars;
  bool                                allow_interruptions_r11 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_HO_CMD_H
