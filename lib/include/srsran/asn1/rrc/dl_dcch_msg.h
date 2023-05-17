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

#ifndef SRSASN1_RRC_DLDCCH_MSG_H
#define SRSASN1_RRC_DLDCCH_MSG_H

#include "bcch_msg.h"
#include "dl_ccch_msg.h"
#include "security.h"
#include "si.h"
#include "uecap.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// CondReconfigurationAddMod-r16 ::= SEQUENCE
struct cond_recfg_add_mod_r16_s {
  using trigger_condition_r16_l_ = bounded_array<uint8_t, 2>;

  // member variables
  bool                     ext                             = false;
  bool                     trigger_condition_r16_present   = false;
  bool                     cond_recfg_to_apply_r16_present = false;
  uint8_t                  cond_recfg_id_r16               = 1;
  trigger_condition_r16_l_ trigger_condition_r16;
  dyn_octstring            cond_recfg_to_apply_r16;
  // ...
  // group 0
  bool          trigger_condition_sn_r17_present = false;
  dyn_octstring trigger_condition_sn_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CondReconfigurationToAddModList-r16 ::= SEQUENCE (SIZE (1..8)) OF CondReconfigurationAddMod-r16
using cond_recfg_to_add_mod_list_r16_l = dyn_array<cond_recfg_add_mod_r16_s>;

// CondReconfigurationToRemoveList-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using cond_recfg_to_rem_list_r16_l = bounded_array<uint8_t, 8>;

// SCellConfigCommon-r15 ::= SEQUENCE
struct scell_cfg_common_r15_s {
  bool                      rr_cfg_common_scell_r15_present = false;
  bool                      rr_cfg_ded_scell_r15_present    = false;
  bool                      ant_info_ded_scell_r15_present  = false;
  rr_cfg_common_scell_r10_s rr_cfg_common_scell_r15;
  rr_cfg_ded_scell_r10_s    rr_cfg_ded_scell_r15;
  ant_info_ded_v10i0_s      ant_info_ded_scell_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellToReleaseListExt-r13 ::= SEQUENCE (SIZE (1..31)) OF INTEGER (1..31)
using scell_to_release_list_ext_r13_l = bounded_array<uint8_t, 31>;

// SL-DiscTxPoolToAddMod-r12 ::= SEQUENCE
struct sl_disc_tx_pool_to_add_mod_r12_s {
  uint8_t                pool_id_r12 = 1;
  sl_disc_res_pool_r12_s pool_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TF-IndexPair-r12b ::= SEQUENCE
struct sl_tf_idx_pair_r12b_s {
  bool    disc_sf_idx_r12b_present  = false;
  bool    disc_prb_idx_r12b_present = false;
  uint8_t disc_sf_idx_r12b          = 0;
  uint8_t disc_prb_idx_r12b         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SubframeAssignment-r15 ::= ENUMERATED
struct sf_assign_r15_opts {
  enum options { sa0, sa1, sa2, sa3, sa4, sa5, sa6, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<sf_assign_r15_opts> sf_assign_r15_e;

// ConditionalReconfiguration-r16 ::= SEQUENCE
struct conditional_recfg_r16_s {
  bool                             ext                                    = false;
  bool                             cond_recfg_to_add_mod_list_r16_present = false;
  bool                             cond_recfg_to_rem_list_r16_present     = false;
  bool                             attempt_cond_reconf_r16_present        = false;
  cond_recfg_to_add_mod_list_r16_l cond_recfg_to_add_mod_list_r16;
  cond_recfg_to_rem_list_r16_l     cond_recfg_to_rem_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-v1700-IEs ::= SEQUENCE
struct rrc_conn_recfg_v1700_ies_s {
  bool          sib_type31_ded_r17_present = false;
  bool          scg_state_r17_present      = false;
  bool          non_crit_ext_present       = false;
  dyn_octstring sib_type31_ded_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RSRP-ChangeThresh-r16 ::= ENUMERATED
struct rsrp_change_thresh_r16_opts {
  enum options {
    db4,
    db6,
    db8,
    db10,
    db14,
    db18,
    db22,
    db26,
    db30,
    db34,
    spare6,
    spare5,
    spare4,
    spare3,
    spare2,
    spare1,
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<rsrp_change_thresh_r16_opts> rsrp_change_thresh_r16_e;

// SCellGroupToAddMod-r15 ::= SEQUENCE
struct scell_group_to_add_mod_r15_s {
  bool                            scell_cfg_common_r15_present      = false;
  bool                            scell_to_release_list_r15_present = false;
  bool                            scell_to_add_mod_list_r15_present = false;
  uint8_t                         scell_group_idx_r15               = 1;
  scell_cfg_common_r15_s          scell_cfg_common_r15;
  scell_to_release_list_ext_r13_l scell_to_release_list_r15;
  scell_to_add_mod_list_ext_r13_l scell_to_add_mod_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DiscTxPoolToAddModList-r12 ::= SEQUENCE (SIZE (1..4)) OF SL-DiscTxPoolToAddMod-r12
using sl_disc_tx_pool_to_add_mod_list_r12_l = dyn_array<sl_disc_tx_pool_to_add_mod_r12_s>;

// SL-HoppingConfigDisc-r12 ::= SEQUENCE
struct sl_hop_cfg_disc_r12_s {
  struct c_r12_opts {
    enum options { n1, n5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<c_r12_opts> c_r12_e_;

  // member variables
  uint8_t  a_r12 = 1;
  uint8_t  b_r12 = 1;
  c_r12_e_ c_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TF-IndexPairList-r12b ::= SEQUENCE (SIZE (1..64)) OF SL-TF-IndexPair-r12b
using sl_tf_idx_pair_list_r12b_l = dyn_array<sl_tf_idx_pair_r12b_s>;

// SL-TxPoolToReleaseList-r12 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (1..4)
using sl_tx_pool_to_release_list_r12_l = bounded_array<uint8_t, 4>;

// TDM-PatternConfig-r15 ::= CHOICE
struct tdm_pattern_cfg_r15_c {
  struct setup_s_ {
    sf_assign_r15_e sf_assign_r15;
    uint8_t         harq_offset_r15 = 0;
  };
  using types = setup_e;

  // choice methods
  tdm_pattern_cfg_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "TDM-PatternConfig-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "TDM-PatternConfig-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// IKE-Identity-r13 ::= SEQUENCE
struct ike_id_r13_s {
  dyn_octstring id_i_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IP-Address-r13 ::= CHOICE
struct ip_address_r13_c {
  struct types_opts {
    enum options { ipv4_r13, ipv6_r13, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ip_address_r13_c() = default;
  ip_address_r13_c(const ip_address_r13_c& other);
  ip_address_r13_c& operator=(const ip_address_r13_c& other);
  ~ip_address_r13_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<32>& ipv4_r13()
  {
    assert_choice_type(types::ipv4_r13, type_, "IP-Address-r13");
    return c.get<fixed_bitstring<32> >();
  }
  fixed_bitstring<128>& ipv6_r13()
  {
    assert_choice_type(types::ipv6_r13, type_, "IP-Address-r13");
    return c.get<fixed_bitstring<128> >();
  }
  const fixed_bitstring<32>& ipv4_r13() const
  {
    assert_choice_type(types::ipv4_r13, type_, "IP-Address-r13");
    return c.get<fixed_bitstring<32> >();
  }
  const fixed_bitstring<128>& ipv6_r13() const
  {
    assert_choice_type(types::ipv6_r13, type_, "IP-Address-r13");
    return c.get<fixed_bitstring<128> >();
  }
  fixed_bitstring<32>&  set_ipv4_r13();
  fixed_bitstring<128>& set_ipv6_r13();

private:
  types                                  type_;
  choice_buffer_t<fixed_bitstring<128> > c;

  void destroy_();
};

// PUR-MPDCCH-Config-r16 ::= SEQUENCE
struct pur_mpdcch_cfg_r16_s {
  struct mpdcch_prb_pairs_cfg_r16_s_ {
    struct num_prb_pairs_r16_opts {
      enum options { n2, n4, n6, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<num_prb_pairs_r16_opts> num_prb_pairs_r16_e_;

    // member variables
    num_prb_pairs_r16_e_ num_prb_pairs_r16;
    fixed_bitstring<4>   res_block_assign_r16;
  };
  struct mpdcch_num_repeat_r16_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, r128, r256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mpdcch_num_repeat_r16_opts> mpdcch_num_repeat_r16_e_;
  struct mpdcch_start_sf_uess_r16_c_ {
    struct fdd_opts {
      enum options { v1, v1dot5, v2, v2dot5, v4, v5, v8, v10, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<fdd_opts> fdd_e_;
    struct tdd_opts {
      enum options { v1, v2, v4, v5, v8, v10, v20, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<tdd_opts> tdd_e_;
    struct types_opts {
      enum options { fdd, tdd, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    mpdcch_start_sf_uess_r16_c_() = default;
    mpdcch_start_sf_uess_r16_c_(const mpdcch_start_sf_uess_r16_c_& other);
    mpdcch_start_sf_uess_r16_c_& operator=(const mpdcch_start_sf_uess_r16_c_& other);
    ~mpdcch_start_sf_uess_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fdd_e_& fdd()
    {
      assert_choice_type(types::fdd, type_, "mpdcch-StartSF-UESS-r16");
      return c.get<fdd_e_>();
    }
    tdd_e_& tdd()
    {
      assert_choice_type(types::tdd, type_, "mpdcch-StartSF-UESS-r16");
      return c.get<tdd_e_>();
    }
    const fdd_e_& fdd() const
    {
      assert_choice_type(types::fdd, type_, "mpdcch-StartSF-UESS-r16");
      return c.get<fdd_e_>();
    }
    const tdd_e_& tdd() const
    {
      assert_choice_type(types::tdd, type_, "mpdcch-StartSF-UESS-r16");
      return c.get<tdd_e_>();
    }
    fdd_e_& set_fdd();
    tdd_e_& set_tdd();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct mpdcch_offset_pur_ss_r16_opts {
    enum options {
      zero,
      one_eighth,
      one_quarter,
      three_eighth,
      one_half,
      five_eighth,
      three_quarter,
      seven_eighth,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<mpdcch_offset_pur_ss_r16_opts> mpdcch_offset_pur_ss_r16_e_;

  // member variables
  bool                        mpdcch_freq_hop_r16 = false;
  uint8_t                     mpdcch_nb_r16       = 1;
  mpdcch_prb_pairs_cfg_r16_s_ mpdcch_prb_pairs_cfg_r16;
  mpdcch_num_repeat_r16_e_    mpdcch_num_repeat_r16;
  mpdcch_start_sf_uess_r16_c_ mpdcch_start_sf_uess_r16;
  mpdcch_offset_pur_ss_r16_e_ mpdcch_offset_pur_ss_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUR-PUCCH-Config-r16 ::= SEQUENCE
struct pur_pucch_cfg_r16_s {
  struct pucch_num_repeat_ce_format1_r16_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pucch_num_repeat_ce_format1_r16_opts> pucch_num_repeat_ce_format1_r16_e_;

  // member variables
  bool                               n1_pucch_an_r16_present                 = false;
  bool                               pucch_num_repeat_ce_format1_r16_present = false;
  uint16_t                           n1_pucch_an_r16                         = 0;
  pucch_num_repeat_ce_format1_r16_e_ pucch_num_repeat_ce_format1_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUR-PUSCH-Config-r16 ::= SEQUENCE
struct pur_pusch_cfg_r16_s {
  struct pur_grant_info_r16_c_ {
    struct ce_mode_a_s_ {
      fixed_bitstring<2>  num_rus_r16;
      fixed_bitstring<10> prb_alloc_info_r16;
      fixed_bitstring<4>  mcs_r16;
      fixed_bitstring<3>  num_repeats_r16;
    };
    struct ce_mode_b_s_ {
      bool               sub_prb_alloc_r16 = false;
      bool               num_rus_r16       = false;
      fixed_bitstring<8> prb_alloc_info_r16;
      fixed_bitstring<4> mcs_r16;
      fixed_bitstring<3> num_repeats_r16;
    };
    struct types_opts {
      enum options { ce_mode_a, ce_mode_b, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    pur_grant_info_r16_c_() = default;
    pur_grant_info_r16_c_(const pur_grant_info_r16_c_& other);
    pur_grant_info_r16_c_& operator=(const pur_grant_info_r16_c_& other);
    ~pur_grant_info_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ce_mode_a_s_& ce_mode_a()
    {
      assert_choice_type(types::ce_mode_a, type_, "pur-GrantInfo-r16");
      return c.get<ce_mode_a_s_>();
    }
    ce_mode_b_s_& ce_mode_b()
    {
      assert_choice_type(types::ce_mode_b, type_, "pur-GrantInfo-r16");
      return c.get<ce_mode_b_s_>();
    }
    const ce_mode_a_s_& ce_mode_a() const
    {
      assert_choice_type(types::ce_mode_a, type_, "pur-GrantInfo-r16");
      return c.get<ce_mode_a_s_>();
    }
    const ce_mode_b_s_& ce_mode_b() const
    {
      assert_choice_type(types::ce_mode_b, type_, "pur-GrantInfo-r16");
      return c.get<ce_mode_b_s_>();
    }
    ce_mode_a_s_& set_ce_mode_a();
    ce_mode_b_s_& set_ce_mode_b();

  private:
    types                                       type_;
    choice_buffer_t<ce_mode_a_s_, ce_mode_b_s_> c;

    void destroy_();
  };
  struct pusch_cyclic_shift_r16_opts {
    enum options { n0, n6, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pusch_cyclic_shift_r16_opts> pusch_cyclic_shift_r16_e_;

  // member variables
  bool                      pur_grant_info_r16_present     = false;
  bool                      location_ce_mode_b_r16_present = false;
  pur_grant_info_r16_c_     pur_grant_info_r16;
  bool                      pur_pusch_freq_hop_r16 = false;
  int8_t                    p0_ue_pusch_r16        = -8;
  alpha_r12_e               alpha_r16;
  pusch_cyclic_shift_r16_e_ pusch_cyclic_shift_r16;
  bool                      pusch_nb_max_tbs_r16   = false;
  uint8_t                   location_ce_mode_b_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUR-RSRP-ChangeThreshold-r16 ::= SEQUENCE
struct pur_rsrp_change_thres_r16_s {
  bool                     decrease_thresh_r16_present = false;
  rsrp_change_thresh_r16_e increase_thresh_r16;
  rsrp_change_thresh_r16_e decrease_thresh_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhysicalConfigDedicated-v1370 ::= SEQUENCE
struct phys_cfg_ded_v1370_s {
  bool                  pucch_cfg_ded_v1370_present = false;
  pucch_cfg_ded_v1370_s pucch_cfg_ded_v1370;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAN-AreaConfig-r15 ::= SEQUENCE
struct ran_area_cfg_r15_s {
  using ran_area_code_list_r15_l_ = bounded_array<uint16_t, 32>;

  // member variables
  bool                      ran_area_code_list_r15_present = false;
  fixed_bitstring<24>       tac_minus5_gc_r15;
  ran_area_code_list_r15_l_ ran_area_code_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-v1610-IEs ::= SEQUENCE
struct rrc_conn_recfg_v1610_ies_s {
  bool                       conditional_recfg_r16_present   = false;
  bool                       daps_source_release_r16_present = false;
  bool                       tdm_pattern_cfg2_r16_present    = false;
  bool                       sl_cfg_ded_for_nr_r16_present   = false;
  bool                       sl_ssb_prio_eutra_r16_present   = false;
  bool                       non_crit_ext_present            = false;
  conditional_recfg_r16_s    conditional_recfg_r16;
  tdm_pattern_cfg_r15_c      tdm_pattern_cfg2_r16;
  dyn_octstring              sl_cfg_ded_for_nr_r16;
  uint8_t                    sl_ssb_prio_eutra_r16 = 1;
  rrc_conn_recfg_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigCommonSCell-v1440 ::= SEQUENCE
struct rr_cfg_common_scell_v1440_s {
  struct ul_cfg_v1440_s_ {
    struct ul_freq_info_v1440_s_ {
      uint16_t add_spec_emission_scell_v1440 = 33;
    };

    // member variables
    ul_freq_info_v1440_s_ ul_freq_info_v1440;
  };

  // member variables
  ul_cfg_v1440_s_ ul_cfg_v1440;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellGroupToAddModList-r15 ::= SEQUENCE (SIZE (1..4)) OF SCellGroupToAddMod-r15
using scell_group_to_add_mod_list_r15_l = dyn_array<scell_group_to_add_mod_r15_s>;

// SCellGroupToReleaseList-r15 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (1..4)
using scell_group_to_release_list_r15_l = bounded_array<uint8_t, 4>;

// SL-DiscTxConfigScheduled-r13 ::= SEQUENCE
struct sl_disc_tx_cfg_sched_r13_s {
  bool                       ext                          = false;
  bool                       disc_tx_cfg_r13_present      = false;
  bool                       disc_tf_idx_list_r13_present = false;
  bool                       disc_hop_cfg_r13_present     = false;
  sl_disc_res_pool_r12_s     disc_tx_cfg_r13;
  sl_tf_idx_pair_list_r12b_l disc_tf_idx_list_r13;
  sl_hop_cfg_disc_r12_s      disc_hop_cfg_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DiscTxPoolDedicated-r13 ::= SEQUENCE
struct sl_disc_tx_pool_ded_r13_s {
  bool                                  pool_to_release_list_r13_present = false;
  bool                                  pool_to_add_mod_list_r13_present = false;
  sl_tx_pool_to_release_list_r12_l      pool_to_release_list_r13;
  sl_disc_tx_pool_to_add_mod_list_r12_l pool_to_add_mod_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxPoolToAddMod-r14 ::= SEQUENCE
struct sl_tx_pool_to_add_mod_r14_s {
  uint8_t                    pool_id_r14 = 1;
  sl_comm_res_pool_v2x_r14_s pool_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkPowerControlCommonPSCell-r12 ::= SEQUENCE
struct ul_pwr_ctrl_common_ps_cell_r12_s {
  struct delta_f_pucch_format3_r12_opts {
    enum options {
      delta_f_minus1,
      delta_f0,
      delta_f1,
      delta_f2,
      delta_f3,
      delta_f4,
      delta_f5,
      delta_f6,
      nulltype
    } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_f_pucch_format3_r12_opts> delta_f_pucch_format3_r12_e_;
  struct delta_f_pucch_format1b_cs_r12_opts {
    enum options { delta_f1, delta_f2, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<delta_f_pucch_format1b_cs_r12_opts> delta_f_pucch_format1b_cs_r12_e_;

  // member variables
  delta_f_pucch_format3_r12_e_     delta_f_pucch_format3_r12;
  delta_f_pucch_format1b_cs_r12_e_ delta_f_pucch_format1b_cs_r12;
  int8_t                           p0_nominal_pucch_r12 = -127;
  delta_flist_pucch_s              delta_flist_pucch_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLAN-SuspendConfig-r14 ::= SEQUENCE
struct wlan_suspend_cfg_r14_s {
  bool wlan_suspend_resume_allowed_r14_present         = false;
  bool wlan_suspend_triggers_status_report_r14_present = false;
  bool wlan_suspend_resume_allowed_r14                 = false;
  bool wlan_suspend_triggers_status_report_r14         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogicalChGroupInfoList-r13 ::= SEQUENCE (SIZE (1..4)) OF SL-PriorityList-r13
using lc_ch_group_info_list_r13_l = dyn_array<sl_prio_list_r13_l>;

// LogicalChGroupInfoList-v1530 ::= SEQUENCE (SIZE (1..4)) OF SL-ReliabilityList-r15
using lc_ch_group_info_list_v1530_l = dyn_array<sl_reliability_list_r15_l>;

// MAC-MainConfigSL-r12 ::= SEQUENCE
struct mac_main_cfg_sl_r12_s {
  bool                     periodic_bsr_timer_sl_present = false;
  periodic_bsr_timer_r12_e periodic_bsr_timer_sl;
  retx_bsr_timer_r12_e     retx_bsr_timer_sl;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-RAN-AreaCell-r15 ::= SEQUENCE
struct plmn_ran_area_cell_r15_s {
  using ran_area_cells_r15_l_ = bounded_array<fixed_bitstring<28>, 32>;

  // member variables
  bool                  plmn_id_r15_present = false;
  plmn_id_s             plmn_id_r15;
  ran_area_cells_r15_l_ ran_area_cells_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-RAN-AreaConfig-r15 ::= SEQUENCE
struct plmn_ran_area_cfg_r15_s {
  using ran_area_r15_l_ = dyn_array<ran_area_cfg_r15_s>;

  // member variables
  bool            plmn_id_r15_present = false;
  plmn_id_s       plmn_id_r15;
  ran_area_r15_l_ ran_area_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUR-Config-r16 ::= SEQUENCE
struct pur_cfg_r16_s {
  struct pur_implicit_release_after_r16_opts {
    enum options { n2, n4, n8, spare, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pur_implicit_release_after_r16_opts> pur_implicit_release_after_r16_e_;
  struct pur_start_time_params_r16_s_ {
    pur_periodicity_and_offset_r16_c periodicity_and_offset_r16;
    uint16_t                         start_sfn_r16       = 0;
    uint8_t                          start_sub_frame_r16 = 0;
    fixed_bitstring<1>               hsfn_lsb_info_r16;
  };
  struct pur_num_occasions_r16_opts {
    enum options { one, infinite, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pur_num_occasions_r16_opts> pur_num_occasions_r16_e_;
  struct pur_resp_win_timer_r16_opts {
    enum options { sf240, sf480, sf960, sf1920, sf3840, sf5760, sf7680, sf10240, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<pur_resp_win_timer_r16_opts> pur_resp_win_timer_r16_e_;

  // member variables
  bool                                         ext                                    = false;
  bool                                         pur_cfg_id_r16_present                 = false;
  bool                                         pur_implicit_release_after_r16_present = false;
  bool                                         pur_start_time_params_r16_present      = false;
  bool                                         pur_rnti_r16_present                   = false;
  bool                                         pur_time_align_timer_r16_present       = false;
  bool                                         pur_rsrp_change_thres_r16_present      = false;
  bool                                         pur_resp_win_timer_r16_present         = false;
  bool                                         pur_mpdcch_cfg_r16_present             = false;
  bool                                         pur_pucch_cfg_r16_present              = false;
  bool                                         pur_pusch_cfg_r16_present              = false;
  fixed_bitstring<20>                          pur_cfg_id_r16;
  pur_implicit_release_after_r16_e_            pur_implicit_release_after_r16;
  pur_start_time_params_r16_s_                 pur_start_time_params_r16;
  pur_num_occasions_r16_e_                     pur_num_occasions_r16;
  fixed_bitstring<16>                          pur_rnti_r16;
  uint8_t                                      pur_time_align_timer_r16 = 1;
  setup_release_c<pur_rsrp_change_thres_r16_s> pur_rsrp_change_thres_r16;
  pur_resp_win_timer_r16_e_                    pur_resp_win_timer_r16;
  pur_mpdcch_cfg_r16_s                         pur_mpdcch_cfg_r16;
  bool                                         pur_pdsch_freq_hop_r16 = false;
  pur_pucch_cfg_r16_s                          pur_pucch_cfg_r16;
  pur_pusch_cfg_r16_s                          pur_pusch_cfg_r16;
  // ...
  // group 0
  bool pur_pdsch_max_tbs_r17_present = false;
  bool pur_pdsch_max_tbs_r17         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRC-InactiveConfig-v1610 ::= SEQUENCE
struct rrc_inactive_cfg_v1610_s {
  struct ran_paging_cycle_v1610_opts {
    enum options { rf512, rf1024, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<ran_paging_cycle_v1610_opts> ran_paging_cycle_v1610_e_;

  // member variables
  ran_paging_cycle_v1610_e_ ran_paging_cycle_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-v1530-IEs ::= SEQUENCE
struct rrc_conn_recfg_v1530_ies_s {
  using ded_info_nas_list_r15_l_ = bounded_array<dyn_octstring, 15>;

  // member variables
  bool                              security_cfg_ho_v1530_present           = false;
  bool                              scell_group_to_release_list_r15_present = false;
  bool                              scell_group_to_add_mod_list_r15_present = false;
  bool                              ded_info_nas_list_r15_present           = false;
  bool                              p_max_ue_fr1_r15_present                = false;
  bool                              smtc_r15_present                        = false;
  bool                              non_crit_ext_present                    = false;
  security_cfg_ho_v1530_s           security_cfg_ho_v1530;
  scell_group_to_release_list_r15_l scell_group_to_release_list_r15;
  scell_group_to_add_mod_list_r15_l scell_group_to_add_mod_list_r15;
  ded_info_nas_list_r15_l_          ded_info_nas_list_r15;
  int8_t                            p_max_ue_fr1_r15 = -30;
  mtc_ssb_nr_r15_s                  smtc_r15;
  rrc_conn_recfg_v1610_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-v1650-IEs ::= SEQUENCE
struct rrc_conn_release_v1650_ies_s {
  bool mps_prio_ind_r16_present = false;
  bool non_crit_ext_present     = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigCommonPSCell-r12 ::= SEQUENCE
struct rr_cfg_common_ps_cell_r12_s {
  bool                             ext = false;
  rr_cfg_common_scell_r10_s        basic_fields_r12;
  pucch_cfg_common_s               pucch_cfg_common_r12;
  rach_cfg_common_s                rach_cfg_common_r12;
  ul_pwr_ctrl_common_ps_cell_r12_s ul_pwr_ctrl_common_ps_cell_r12;
  // ...
  // group 0
  copy_ptr<ul_pwr_ctrl_common_v1310_s> ul_pwr_ctrl_common_ps_cell_v1310;
  // group 1
  copy_ptr<ul_pwr_ctrl_common_v1530_s> ul_pwr_ctrl_common_ps_cell_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigCommonPSCell-v1440 ::= SEQUENCE
struct rr_cfg_common_ps_cell_v1440_s {
  rr_cfg_common_scell_v1440_s basic_fields_v1440;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigDedicatedPSCell-r12 ::= SEQUENCE
struct rr_cfg_ded_ps_cell_r12_s {
  bool                    ext                              = false;
  bool                    phys_cfg_ded_ps_cell_r12_present = false;
  bool                    sps_cfg_r12_present              = false;
  bool                    naics_info_r12_present           = false;
  phys_cfg_ded_s          phys_cfg_ded_ps_cell_r12;
  sps_cfg_s               sps_cfg_r12;
  naics_assist_info_r12_c naics_info_r12;
  // ...
  // group 0
  copy_ptr<neigh_cells_crs_info_r13_c> neigh_cells_crs_info_ps_cell_r13;
  // group 1
  copy_ptr<sps_cfg_v1430_s> sps_cfg_v1430;
  // group 2
  bool                                 crs_intf_mitig_enabled_r15_present = false;
  copy_ptr<sps_cfg_v1530_s>            sps_cfg_v1530;
  bool                                 crs_intf_mitig_enabled_r15 = false;
  copy_ptr<neigh_cells_crs_info_r15_c> neigh_cells_crs_info_r15;
  // group 3
  copy_ptr<sps_cfg_v1540_s> sps_cfg_v1540;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigDedicatedPSCell-v1370 ::= SEQUENCE
struct rr_cfg_ded_ps_cell_v1370_s {
  bool                 phys_cfg_ded_ps_cell_v1370_present = false;
  phys_cfg_ded_v1370_s phys_cfg_ded_ps_cell_v1370;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigDedicatedPSCell-v13c0 ::= SEQUENCE
struct rr_cfg_ded_ps_cell_v13c0_s {
  phys_cfg_ded_v13c0_s phys_cfg_ded_ps_cell_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DiscTxRefCarrierDedicated-r13 ::= CHOICE
struct sl_disc_tx_ref_carrier_ded_r13_c {
  struct types_opts {
    enum options { pcell, scell, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  sl_disc_tx_ref_carrier_ded_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& scell()
  {
    assert_choice_type(types::scell, type_, "SL-DiscTxRefCarrierDedicated-r13");
    return c;
  }
  const uint8_t& scell() const
  {
    assert_choice_type(types::scell, type_, "SL-DiscTxRefCarrierDedicated-r13");
    return c;
  }
  void     set_pcell();
  uint8_t& set_scell();

private:
  types   type_;
  uint8_t c;
};

// SL-DiscTxResource-r13 ::= CHOICE
struct sl_disc_tx_res_r13_c {
  struct setup_c_ {
    struct types_opts {
      enum options { sched_r13, ue_sel_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    setup_c_() = default;
    setup_c_(const setup_c_& other);
    setup_c_& operator=(const setup_c_& other);
    ~setup_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_disc_tx_cfg_sched_r13_s& sched_r13()
    {
      assert_choice_type(types::sched_r13, type_, "setup");
      return c.get<sl_disc_tx_cfg_sched_r13_s>();
    }
    sl_disc_tx_pool_ded_r13_s& ue_sel_r13()
    {
      assert_choice_type(types::ue_sel_r13, type_, "setup");
      return c.get<sl_disc_tx_pool_ded_r13_s>();
    }
    const sl_disc_tx_cfg_sched_r13_s& sched_r13() const
    {
      assert_choice_type(types::sched_r13, type_, "setup");
      return c.get<sl_disc_tx_cfg_sched_r13_s>();
    }
    const sl_disc_tx_pool_ded_r13_s& ue_sel_r13() const
    {
      assert_choice_type(types::ue_sel_r13, type_, "setup");
      return c.get<sl_disc_tx_pool_ded_r13_s>();
    }
    sl_disc_tx_cfg_sched_r13_s& set_sched_r13();
    sl_disc_tx_pool_ded_r13_s&  set_ue_sel_r13();

  private:
    types                                                                  type_;
    choice_buffer_t<sl_disc_tx_cfg_sched_r13_s, sl_disc_tx_pool_ded_r13_s> c;

    void destroy_();
  };
  using types = setup_e;

  // choice methods
  sl_disc_tx_res_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_c_& setup()
  {
    assert_choice_type(types::setup, type_, "SL-DiscTxResource-r13");
    return c;
  }
  const setup_c_& setup() const
  {
    assert_choice_type(types::setup, type_, "SL-DiscTxResource-r13");
    return c;
  }
  void      set_release();
  setup_c_& set_setup();

private:
  types    type_;
  setup_c_ c;
};

// SL-TxPoolToAddModListV2X-r14 ::= SEQUENCE (SIZE (1..8)) OF SL-TxPoolToAddMod-r14
using sl_tx_pool_to_add_mod_list_v2x_r14_l = dyn_array<sl_tx_pool_to_add_mod_r14_s>;

// SL-TxPoolToReleaseListV2X-r14 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using sl_tx_pool_to_release_list_v2x_r14_l = bounded_array<uint8_t, 8>;

// TunnelConfigLWIP-r13 ::= SEQUENCE
struct tunnel_cfg_lwip_r13_s {
  bool             ext = false;
  ip_address_r13_c ip_address_r13;
  ike_id_r13_s     ike_id_r13;
  // ...
  // group 0
  bool     lwip_counter_r13_present = false;
  uint32_t lwip_counter_r13         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ValidityCellList-r16 ::= SEQUENCE (SIZE (1..8)) OF PhysCellIdRange
using validity_cell_list_r16_l = dyn_array<pci_range_s>;

// WLAN-MobilityConfig-r13 ::= SEQUENCE
struct wlan_mob_cfg_r13_s {
  struct assoc_timer_r13_opts {
    enum options { s10, s30, s60, s120, s240, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<assoc_timer_r13_opts> assoc_timer_r13_e_;

  // member variables
  bool               ext                                  = false;
  bool               wlan_to_release_list_r13_present     = false;
  bool               wlan_to_add_list_r13_present         = false;
  bool               assoc_timer_r13_present              = false;
  bool               success_report_requested_r13_present = false;
  wlan_id_list_r13_l wlan_to_release_list_r13;
  wlan_id_list_r13_l wlan_to_add_list_r13;
  assoc_timer_r13_e_ assoc_timer_r13;
  // ...
  // group 0
  copy_ptr<wlan_suspend_cfg_r14_s> wlan_suspend_cfg_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LWA-Config-r13 ::= SEQUENCE
struct lwa_cfg_r13_s {
  bool               ext                        = false;
  bool               lwa_mob_cfg_r13_present    = false;
  bool               lwa_wt_counter_r13_present = false;
  wlan_mob_cfg_r13_s lwa_mob_cfg_r13;
  uint32_t           lwa_wt_counter_r13 = 0;
  // ...
  // group 0
  bool               wt_mac_address_r14_present = false;
  fixed_octstring<6> wt_mac_address_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LWIP-Config-r13 ::= SEQUENCE
struct lwip_cfg_r13_s {
  bool                  ext                         = false;
  bool                  lwip_mob_cfg_r13_present    = false;
  bool                  tunnel_cfg_lwip_r13_present = false;
  wlan_mob_cfg_r13_s    lwip_mob_cfg_r13;
  tunnel_cfg_lwip_r13_s tunnel_cfg_lwip_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityControlInfoSCG-r12 ::= SEQUENCE
struct mob_ctrl_info_scg_r12_s {
  struct t307_r12_opts {
    enum options { ms50, ms100, ms150, ms200, ms500, ms1000, ms2000, spare1, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t307_r12_opts> t307_r12_e_;

  // member variables
  bool                      ext                                 = false;
  bool                      ue_id_scg_r12_present               = false;
  bool                      rach_cfg_ded_r12_present            = false;
  bool                      ciphering_algorithm_scg_r12_present = false;
  t307_r12_e_               t307_r12;
  fixed_bitstring<16>       ue_id_scg_r12;
  rach_cfg_ded_s            rach_cfg_ded_r12;
  ciphering_algorithm_r12_e ciphering_algorithm_scg_r12;
  // ...
  // group 0
  bool                      make_before_break_scg_r14_present = false;
  copy_ptr<rach_skip_r14_s> rach_skip_scg_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-RAN-AreaCellList-r15 ::= SEQUENCE (SIZE (1..8)) OF PLMN-RAN-AreaCell-r15
using plmn_ran_area_cell_list_r15_l = dyn_array<plmn_ran_area_cell_r15_s>;

// PLMN-RAN-AreaConfigList-r15 ::= SEQUENCE (SIZE (1..8)) OF PLMN-RAN-AreaConfig-r15
using plmn_ran_area_cfg_list_r15_l = dyn_array<plmn_ran_area_cfg_r15_s>;

// PSCellToAddMod-r12 ::= SEQUENCE
struct ps_cell_to_add_mod_r12_s {
  struct cell_identif_r12_s_ {
    uint16_t pci_r12             = 0;
    uint32_t dl_carrier_freq_r12 = 0;
  };

  // member variables
  bool                        ext                               = false;
  bool                        cell_identif_r12_present          = false;
  bool                        rr_cfg_common_ps_cell_r12_present = false;
  bool                        rr_cfg_ded_ps_cell_r12_present    = false;
  uint8_t                     scell_idx_r12                     = 1;
  cell_identif_r12_s_         cell_identif_r12;
  rr_cfg_common_ps_cell_r12_s rr_cfg_common_ps_cell_r12;
  rr_cfg_ded_ps_cell_r12_s    rr_cfg_ded_ps_cell_r12;
  // ...
  // group 0
  copy_ptr<ant_info_ded_v10i0_s> ant_info_ded_ps_cell_v1280;
  // group 1
  bool    scell_idx_r13_present = false;
  uint8_t scell_idx_r13         = 1;
  // group 2
  copy_ptr<rr_cfg_ded_ps_cell_v1370_s> rr_cfg_ded_ps_cell_v1370;
  // group 3
  copy_ptr<rr_cfg_ded_ps_cell_v13c0_s> rr_cfg_ded_ps_cell_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PSCellToAddMod-v1440 ::= SEQUENCE
struct ps_cell_to_add_mod_v1440_s {
  bool                          rr_cfg_common_ps_cell_r14_present = false;
  rr_cfg_common_ps_cell_v1440_s rr_cfg_common_ps_cell_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RCLWI-Config-r13 ::= SEQUENCE
struct rclwi_cfg_r13_s {
  struct cmd_c_ {
    struct steer_to_wlan_r13_s_ {
      wlan_id_list_r12_l mob_cfg_r13;
    };
    struct types_opts {
      enum options { steer_to_wlan_r13, steer_to_lte_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    cmd_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    steer_to_wlan_r13_s_& steer_to_wlan_r13()
    {
      assert_choice_type(types::steer_to_wlan_r13, type_, "command");
      return c;
    }
    const steer_to_wlan_r13_s_& steer_to_wlan_r13() const
    {
      assert_choice_type(types::steer_to_wlan_r13, type_, "command");
      return c;
    }
    steer_to_wlan_r13_s_& set_steer_to_wlan_r13();
    void                  set_steer_to_lte_r13();

  private:
    types                type_;
    steer_to_wlan_r13_s_ c;
  };

  // member variables
  bool   ext = false;
  cmd_c_ cmd;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-v1510-IEs ::= SEQUENCE
struct rrc_conn_recfg_v1510_ies_s {
  struct nr_cfg_r15_c_ {
    struct setup_s_ {
      bool          nr_secondary_cell_group_cfg_r15_present = false;
      bool          p_max_eutra_r15_present                 = false;
      bool          endc_release_and_add_r15                = false;
      dyn_octstring nr_secondary_cell_group_cfg_r15;
      int8_t        p_max_eutra_r15 = -30;
    };
    using types = setup_e;

    // choice methods
    nr_cfg_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "nr-Config-r15");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "nr-Config-r15");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                       nr_cfg_r15_present               = false;
  bool                       sk_counter_r15_present           = false;
  bool                       nr_radio_bearer_cfg1_r15_present = false;
  bool                       nr_radio_bearer_cfg2_r15_present = false;
  bool                       tdm_pattern_cfg_r15_present      = false;
  bool                       non_crit_ext_present             = false;
  nr_cfg_r15_c_              nr_cfg_r15;
  uint32_t                   sk_counter_r15 = 0;
  dyn_octstring              nr_radio_bearer_cfg1_r15;
  dyn_octstring              nr_radio_bearer_cfg2_r15;
  tdm_pattern_cfg_r15_c      tdm_pattern_cfg_r15;
  rrc_conn_recfg_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-v1610-IEs ::= SEQUENCE
struct rrc_conn_release_v1610_ies_s {
  struct t323_r16_opts {
    enum options { min5, min10, min20, min30, min60, min120, min180, min720, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t323_r16_opts> t323_r16_e_;

  // member variables
  bool                           full_i_rnti_r16_present           = false;
  bool                           short_i_rnti_r16_present          = false;
  bool                           pur_cfg_r16_present               = false;
  bool                           rrc_inactive_cfg_v1610_present    = false;
  bool                           release_idle_meas_cfg_r16_present = false;
  bool                           alt_freq_priorities_r16_present   = false;
  bool                           t323_r16_present                  = false;
  bool                           non_crit_ext_present              = false;
  fixed_bitstring<40>            full_i_rnti_r16;
  fixed_bitstring<24>            short_i_rnti_r16;
  setup_release_c<pur_cfg_r16_s> pur_cfg_r16;
  rrc_inactive_cfg_v1610_s       rrc_inactive_cfg_v1610;
  t323_r16_e_                    t323_r16;
  rrc_conn_release_v1650_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellToReleaseList-r10 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (1..7)
using scell_to_release_list_r10_l = bounded_array<uint8_t, 4>;

// SL-CommTxPoolToAddMod-r12 ::= SEQUENCE
struct sl_comm_tx_pool_to_add_mod_r12_s {
  uint8_t                pool_id_r12 = 1;
  sl_comm_res_pool_r12_s pool_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-CommTxPoolToAddModExt-r13 ::= SEQUENCE
struct sl_comm_tx_pool_to_add_mod_ext_r13_s {
  uint8_t                pool_id_v1310 = 5;
  sl_comm_res_pool_r12_s pool_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DiscTxResourceInfoPerFreq-r13 ::= SEQUENCE
struct sl_disc_tx_res_info_per_freq_r13_s {
  bool                             ext                                 = false;
  bool                             disc_tx_res_r13_present             = false;
  bool                             disc_tx_res_ps_r13_present          = false;
  bool                             disc_tx_ref_carrier_ded_r13_present = false;
  bool                             disc_cell_sel_info_r13_present      = false;
  uint32_t                         disc_tx_carrier_freq_r13            = 0;
  sl_disc_tx_res_r13_c             disc_tx_res_r13;
  sl_disc_tx_res_r13_c             disc_tx_res_ps_r13;
  sl_disc_tx_ref_carrier_ded_r13_c disc_tx_ref_carrier_ded_r13;
  cell_sel_info_nfreq_r13_s        disc_cell_sel_info_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TF-IndexPair-r12 ::= SEQUENCE
struct sl_tf_idx_pair_r12_s {
  bool    disc_sf_idx_r12_present  = false;
  bool    disc_prb_idx_r12_present = false;
  uint8_t disc_sf_idx_r12          = 1;
  uint8_t disc_prb_idx_r12         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-V2X-ConfigDedicated-r14 ::= SEQUENCE
struct sl_v2x_cfg_ded_r14_s {
  struct comm_tx_res_r14_c_ {
    struct setup_c_ {
      struct sched_r14_s_ {
        bool                        v2x_sched_pool_r14_present = false;
        bool                        mcs_r14_present            = false;
        fixed_bitstring<16>         sl_v_rnti_r14;
        mac_main_cfg_sl_r12_s       mac_main_cfg_r14;
        sl_comm_res_pool_v2x_r14_s  v2x_sched_pool_r14;
        uint8_t                     mcs_r14 = 0;
        lc_ch_group_info_list_r13_l lc_ch_group_info_list_r14;
      };
      struct ue_sel_r14_s_ {
        struct v2x_comm_tx_pool_normal_ded_r14_s_ {
          bool                                 pool_to_release_list_r14_present         = false;
          bool                                 pool_to_add_mod_list_r14_present         = false;
          bool                                 v2x_comm_tx_pool_sensing_cfg_r14_present = false;
          sl_tx_pool_to_release_list_v2x_r14_l pool_to_release_list_r14;
          sl_tx_pool_to_add_mod_list_v2x_r14_l pool_to_add_mod_list_r14;
          sl_comm_tx_pool_sensing_cfg_r14_s    v2x_comm_tx_pool_sensing_cfg_r14;
        };

        // member variables
        v2x_comm_tx_pool_normal_ded_r14_s_ v2x_comm_tx_pool_normal_ded_r14;
      };
      struct types_opts {
        enum options { sched_r14, ue_sel_r14, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      setup_c_() = default;
      setup_c_(const setup_c_& other);
      setup_c_& operator=(const setup_c_& other);
      ~setup_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      sched_r14_s_& sched_r14()
      {
        assert_choice_type(types::sched_r14, type_, "setup");
        return c.get<sched_r14_s_>();
      }
      ue_sel_r14_s_& ue_sel_r14()
      {
        assert_choice_type(types::ue_sel_r14, type_, "setup");
        return c.get<ue_sel_r14_s_>();
      }
      const sched_r14_s_& sched_r14() const
      {
        assert_choice_type(types::sched_r14, type_, "setup");
        return c.get<sched_r14_s_>();
      }
      const ue_sel_r14_s_& ue_sel_r14() const
      {
        assert_choice_type(types::ue_sel_r14, type_, "setup");
        return c.get<ue_sel_r14_s_>();
      }
      sched_r14_s_&  set_sched_r14();
      ue_sel_r14_s_& set_ue_sel_r14();

    private:
      types                                        type_;
      choice_buffer_t<sched_r14_s_, ue_sel_r14_s_> c;

      void destroy_();
    };
    using types = setup_e;

    // choice methods
    comm_tx_res_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_c_& setup()
    {
      assert_choice_type(types::setup, type_, "commTxResources-r14");
      return c;
    }
    const setup_c_& setup() const
    {
      assert_choice_type(types::setup, type_, "commTxResources-r14");
      return c;
    }
    void      set_release();
    setup_c_& set_setup();

  private:
    types    type_;
    setup_c_ c;
  };
  struct comm_tx_res_v1530_c_ {
    struct setup_c_ {
      struct sched_v1530_s_ {
        bool                          lc_ch_group_info_list_v1530_present = false;
        bool                          mcs_r15_present                     = false;
        lc_ch_group_info_list_v1530_l lc_ch_group_info_list_v1530;
        uint8_t                       mcs_r15 = 0;
      };
      struct ue_sel_v1530_s_ {
        bool                           v2x_freq_sel_cfg_list_r15_present = false;
        sl_v2x_freq_sel_cfg_list_r15_l v2x_freq_sel_cfg_list_r15;
      };
      struct types_opts {
        enum options { sched_v1530, ue_sel_v1530, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      setup_c_() = default;
      setup_c_(const setup_c_& other);
      setup_c_& operator=(const setup_c_& other);
      ~setup_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      sched_v1530_s_& sched_v1530()
      {
        assert_choice_type(types::sched_v1530, type_, "setup");
        return c.get<sched_v1530_s_>();
      }
      ue_sel_v1530_s_& ue_sel_v1530()
      {
        assert_choice_type(types::ue_sel_v1530, type_, "setup");
        return c.get<ue_sel_v1530_s_>();
      }
      const sched_v1530_s_& sched_v1530() const
      {
        assert_choice_type(types::sched_v1530, type_, "setup");
        return c.get<sched_v1530_s_>();
      }
      const ue_sel_v1530_s_& ue_sel_v1530() const
      {
        assert_choice_type(types::ue_sel_v1530, type_, "setup");
        return c.get<ue_sel_v1530_s_>();
      }
      sched_v1530_s_&  set_sched_v1530();
      ue_sel_v1530_s_& set_ue_sel_v1530();

    private:
      types                                            type_;
      choice_buffer_t<sched_v1530_s_, ue_sel_v1530_s_> c;

      void destroy_();
    };
    using types = setup_e;

    // choice methods
    comm_tx_res_v1530_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_c_& setup()
    {
      assert_choice_type(types::setup, type_, "commTxResources-v1530");
      return c;
    }
    const setup_c_& setup() const
    {
      assert_choice_type(types::setup, type_, "commTxResources-v1530");
      return c;
    }
    void      set_release();
    setup_c_& set_setup();

  private:
    types    type_;
    setup_c_ c;
  };

  // member variables
  bool                              ext                                    = false;
  bool                              comm_tx_res_r14_present                = false;
  bool                              v2x_inter_freq_info_list_r14_present   = false;
  bool                              thres_sl_tx_prioritization_r14_present = false;
  bool                              type_tx_sync_r14_present               = false;
  bool                              cbr_ded_tx_cfg_list_r14_present        = false;
  comm_tx_res_r14_c_                comm_tx_res_r14;
  sl_inter_freq_info_list_v2x_r14_l v2x_inter_freq_info_list_r14;
  uint8_t                           thres_sl_tx_prioritization_r14 = 1;
  sl_type_tx_sync_r14_e             type_tx_sync_r14;
  sl_cbr_common_tx_cfg_list_r14_s   cbr_ded_tx_cfg_list_r14;
  // ...
  // group 0
  bool                                   slss_tx_multi_freq_r15_present = false;
  copy_ptr<comm_tx_res_v1530_c_>         comm_tx_res_v1530;
  copy_ptr<sl_v2x_packet_dupl_cfg_r15_s> v2x_packet_dupl_cfg_r15;
  copy_ptr<sl_v2x_sync_freq_list_r15_l>  sync_freq_list_r15;
  // group 1
  bool slss_tx_disabled_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ValidityArea-r16 ::= SEQUENCE
struct validity_area_r16_s {
  bool                     validity_cell_list_r16_present = false;
  uint32_t                 carrier_freq_r16               = 0;
  validity_cell_list_r16_l validity_cell_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LWA-Configuration-r13 ::= CHOICE
struct lwa_cfg_r13_c {
  struct setup_s_ {
    lwa_cfg_r13_s lwa_cfg_r13;
  };
  using types = setup_e;

  // choice methods
  lwa_cfg_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "LWA-Configuration-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "LWA-Configuration-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// LWIP-Configuration-r13 ::= CHOICE
struct lwip_cfg_r13_c {
  struct setup_s_ {
    lwip_cfg_r13_s lwip_cfg_r13;
  };
  using types = setup_e;

  // choice methods
  lwip_cfg_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "LWIP-Configuration-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "LWIP-Configuration-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// PowerCoordinationInfo-r12 ::= SEQUENCE
struct pwr_coordination_info_r12_s {
  uint8_t p_menb_r12        = 1;
  uint8_t p_senb_r12        = 1;
  uint8_t pwr_ctrl_mode_r12 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAN-NotificationAreaInfo-r15 ::= CHOICE
struct ran_notif_area_info_r15_c {
  struct types_opts {
    enum options { cell_list, ran_area_cfg_list, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ran_notif_area_info_r15_c() = default;
  ran_notif_area_info_r15_c(const ran_notif_area_info_r15_c& other);
  ran_notif_area_info_r15_c& operator=(const ran_notif_area_info_r15_c& other);
  ~ran_notif_area_info_r15_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  plmn_ran_area_cell_list_r15_l& cell_list()
  {
    assert_choice_type(types::cell_list, type_, "RAN-NotificationAreaInfo-r15");
    return c.get<plmn_ran_area_cell_list_r15_l>();
  }
  plmn_ran_area_cfg_list_r15_l& ran_area_cfg_list()
  {
    assert_choice_type(types::ran_area_cfg_list, type_, "RAN-NotificationAreaInfo-r15");
    return c.get<plmn_ran_area_cfg_list_r15_l>();
  }
  const plmn_ran_area_cell_list_r15_l& cell_list() const
  {
    assert_choice_type(types::cell_list, type_, "RAN-NotificationAreaInfo-r15");
    return c.get<plmn_ran_area_cell_list_r15_l>();
  }
  const plmn_ran_area_cfg_list_r15_l& ran_area_cfg_list() const
  {
    assert_choice_type(types::ran_area_cfg_list, type_, "RAN-NotificationAreaInfo-r15");
    return c.get<plmn_ran_area_cfg_list_r15_l>();
  }
  plmn_ran_area_cell_list_r15_l& set_cell_list();
  plmn_ran_area_cfg_list_r15_l&  set_ran_area_cfg_list();

private:
  types                                                                        type_;
  choice_buffer_t<plmn_ran_area_cell_list_r15_l, plmn_ran_area_cfg_list_r15_l> c;

  void destroy_();
};

// RCLWI-Configuration-r13 ::= CHOICE
struct rclwi_cfg_r13_c {
  struct setup_s_ {
    rclwi_cfg_r13_s rclwi_cfg_r13;
  };
  using types = setup_e;

  // choice methods
  rclwi_cfg_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "RCLWI-Configuration-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "RCLWI-Configuration-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// RRCConnectionReconfiguration-v1430-IEs ::= SEQUENCE
struct rrc_conn_recfg_v1430_ies_s {
  bool                              sl_v2x_cfg_ded_r14_present              = false;
  bool                              scell_to_add_mod_list_ext_v1430_present = false;
  bool                              per_cc_gap_ind_request_r14_present      = false;
  bool                              sib_type2_ded_r14_present               = false;
  bool                              non_crit_ext_present                    = false;
  sl_v2x_cfg_ded_r14_s              sl_v2x_cfg_ded_r14;
  scell_to_add_mod_list_ext_v1430_l scell_to_add_mod_list_ext_v1430;
  dyn_octstring                     sib_type2_ded_r14;
  rrc_conn_recfg_v1510_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-v15b0-IEs ::= SEQUENCE
struct rrc_conn_release_v15b0_ies_s {
  bool                         no_last_cell_upd_r15_present = false;
  bool                         non_crit_ext_present         = false;
  rrc_conn_release_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-ConfigPartSCG-r12 ::= SEQUENCE
struct scg_cfg_part_scg_r12_s {
  bool                        ext                                   = false;
  bool                        rr_cfg_ded_scg_r12_present            = false;
  bool                        scell_to_release_list_scg_r12_present = false;
  bool                        pscell_to_add_mod_r12_present         = false;
  bool                        scell_to_add_mod_list_scg_r12_present = false;
  bool                        mob_ctrl_info_scg_r12_present         = false;
  rr_cfg_ded_scg_r12_s        rr_cfg_ded_scg_r12;
  scell_to_release_list_r10_l scell_to_release_list_scg_r12;
  ps_cell_to_add_mod_r12_s    pscell_to_add_mod_r12;
  scell_to_add_mod_list_r10_l scell_to_add_mod_list_scg_r12;
  mob_ctrl_info_scg_r12_s     mob_ctrl_info_scg_r12;
  // ...
  // group 0
  copy_ptr<scell_to_release_list_ext_r13_l> scell_to_release_list_scg_ext_r13;
  copy_ptr<scell_to_add_mod_list_ext_r13_l> scell_to_add_mod_list_scg_ext_r13;
  // group 1
  copy_ptr<scell_to_add_mod_list_ext_v1370_l> scell_to_add_mod_list_scg_ext_v1370;
  // group 2
  copy_ptr<ps_cell_to_add_mod_v1440_s> pscell_to_add_mod_v1440;
  // group 3
  copy_ptr<scell_group_to_release_list_r15_l> scell_group_to_release_list_scg_r15;
  copy_ptr<scell_group_to_add_mod_list_r15_l> scell_group_to_add_mod_list_scg_r15;
  // group 4
  copy_ptr<meas_cfg_s>            meas_cfg_sn_r15;
  copy_ptr<tdm_pattern_cfg_r15_c> tdm_pattern_cfg_ne_dc_r15;
  // group 5
  bool   p_max_eutra_r15_present = false;
  int8_t p_max_eutra_r15         = -30;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-CommTxPoolToAddModList-r12 ::= SEQUENCE (SIZE (1..4)) OF SL-CommTxPoolToAddMod-r12
using sl_comm_tx_pool_to_add_mod_list_r12_l = dyn_array<sl_comm_tx_pool_to_add_mod_r12_s>;

// SL-CommTxPoolToAddModListExt-r13 ::= SEQUENCE (SIZE (1..4)) OF SL-CommTxPoolToAddModExt-r13
using sl_comm_tx_pool_to_add_mod_list_ext_r13_l = dyn_array<sl_comm_tx_pool_to_add_mod_ext_r13_s>;

// SL-DiscSysInfoToReportFreqList-r13 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..262143)
using sl_disc_sys_info_to_report_freq_list_r13_l = bounded_array<uint32_t, 8>;

// SL-DiscTxInfoInterFreqListAdd-r13 ::= SEQUENCE
struct sl_disc_tx_info_inter_freq_list_add_r13_s {
  using disc_tx_freq_to_add_mod_list_r13_l_ = dyn_array<sl_disc_tx_res_info_per_freq_r13_s>;
  using disc_tx_freq_to_release_list_r13_l_ = bounded_array<uint32_t, 8>;

  // member variables
  bool                                ext                                      = false;
  bool                                disc_tx_freq_to_add_mod_list_r13_present = false;
  bool                                disc_tx_freq_to_release_list_r13_present = false;
  disc_tx_freq_to_add_mod_list_r13_l_ disc_tx_freq_to_add_mod_list_r13;
  disc_tx_freq_to_release_list_r13_l_ disc_tx_freq_to_release_list_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-GapConfig-r13 ::= SEQUENCE
struct sl_gap_cfg_r13_s {
  sl_gap_pattern_list_r13_l gap_pattern_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TF-IndexPairList-r12 ::= SEQUENCE (SIZE (1..64)) OF SL-TF-IndexPair-r12
using sl_tf_idx_pair_list_r12_l = dyn_array<sl_tf_idx_pair_r12_s>;

// SL-TxPoolToReleaseListExt-r13 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (5..8)
using sl_tx_pool_to_release_list_ext_r13_l = bounded_array<uint8_t, 4>;

// ValidityAreaList-r16 ::= SEQUENCE (SIZE (1..8)) OF ValidityArea-r16
using validity_area_list_r16_l = dyn_array<validity_area_r16_s>;

// FlightPathInfoReportConfig-r15 ::= SEQUENCE
struct flight_path_info_report_cfg_r15_s {
  bool    include_time_stamp_r15_present = false;
  uint8_t max_way_point_num_r15          = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasIdleConfigDedicated-r15 ::= SEQUENCE
struct meas_idle_cfg_ded_r15_s {
  struct meas_idle_dur_r15_opts {
    enum options { sec10, sec30, sec60, sec120, sec180, sec240, sec300, spare, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<meas_idle_dur_r15_opts> meas_idle_dur_r15_e_;

  // member variables
  bool                     ext                                      = false;
  bool                     meas_idle_carrier_list_eutra_r15_present = false;
  eutra_carrier_list_r15_l meas_idle_carrier_list_eutra_r15;
  meas_idle_dur_r15_e_     meas_idle_dur_r15;
  // ...
  // group 0
  copy_ptr<nr_carrier_list_r16_l>    meas_idle_carrier_list_nr_r16;
  copy_ptr<validity_area_list_r16_l> validity_area_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRC-InactiveConfig-r15 ::= SEQUENCE
struct rrc_inactive_cfg_r15_s {
  struct ran_paging_cycle_r15_opts {
    enum options { rf32, rf64, rf128, rf256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<ran_paging_cycle_r15_opts> ran_paging_cycle_r15_e_;
  struct periodic_rnau_timer_r15_opts {
    enum options { min5, min10, min20, min30, min60, min120, min360, min720, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<periodic_rnau_timer_r15_opts> periodic_rnau_timer_r15_e_;

  // member variables
  bool                       ran_paging_cycle_r15_present        = false;
  bool                       ran_notif_area_info_r15_present     = false;
  bool                       periodic_rnau_timer_r15_present     = false;
  bool                       next_hop_chaining_count_r15_present = false;
  bool                       dummy_present                       = false;
  fixed_bitstring<40>        full_i_rnti_r15;
  fixed_bitstring<24>        short_i_rnti_r15;
  ran_paging_cycle_r15_e_    ran_paging_cycle_r15;
  ran_notif_area_info_r15_c  ran_notif_area_info_r15;
  periodic_rnau_timer_r15_e_ periodic_rnau_timer_r15;
  uint8_t                    next_hop_chaining_count_r15 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-v1310-IEs ::= SEQUENCE
struct rrc_conn_recfg_v1310_ies_s {
  bool                            scell_to_release_list_ext_r13_present = false;
  bool                            scell_to_add_mod_list_ext_r13_present = false;
  bool                            lwa_cfg_r13_present                   = false;
  bool                            lwip_cfg_r13_present                  = false;
  bool                            rclwi_cfg_r13_present                 = false;
  bool                            non_crit_ext_present                  = false;
  scell_to_release_list_ext_r13_l scell_to_release_list_ext_r13;
  scell_to_add_mod_list_ext_r13_l scell_to_add_mod_list_ext_r13;
  lwa_cfg_r13_c                   lwa_cfg_r13;
  lwip_cfg_r13_c                  lwip_cfg_r13;
  rclwi_cfg_r13_c                 rclwi_cfg_r13;
  rrc_conn_recfg_v1430_ies_s      non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-v1540-IEs ::= SEQUENCE
struct rrc_conn_release_v1540_ies_s {
  bool                         wait_time_present    = false;
  bool                         non_crit_ext_present = false;
  uint8_t                      wait_time            = 1;
  rrc_conn_release_v15b0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-Configuration-r12 ::= CHOICE
struct scg_cfg_r12_c {
  struct setup_s_ {
    struct scg_cfg_part_mcg_r12_s_ {
      bool                        ext                               = false;
      bool                        scg_counter_r12_present           = false;
      bool                        pwr_coordination_info_r12_present = false;
      uint32_t                    scg_counter_r12                   = 0;
      pwr_coordination_info_r12_s pwr_coordination_info_r12;
      // ...
    };

    // member variables
    bool                    scg_cfg_part_mcg_r12_present = false;
    bool                    scg_cfg_part_scg_r12_present = false;
    scg_cfg_part_mcg_r12_s_ scg_cfg_part_mcg_r12;
    scg_cfg_part_scg_r12_s  scg_cfg_part_scg_r12;
  };
  using types = setup_e;

  // choice methods
  scg_cfg_r12_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SCG-Configuration-r12");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SCG-Configuration-r12");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SCellToAddMod-r16 ::= SEQUENCE
struct scell_to_add_mod_r16_s {
  struct cell_identif_r16_s_ {
    uint16_t pci_r16             = 0;
    uint32_t dl_carrier_freq_r16 = 0;
  };
  struct scell_state_r16_opts {
    enum options { activ, dormant, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<scell_state_r16_opts> scell_state_r16_e_;

  // member variables
  bool                      ext                                       = false;
  bool                      cell_identif_r16_present                  = false;
  bool                      rr_cfg_common_scell_r16_present           = false;
  bool                      rr_cfg_ded_scell_r16_present              = false;
  bool                      ant_info_ded_scell_r16_present            = false;
  bool                      srs_switch_from_serv_cell_idx_r16_present = false;
  bool                      scell_state_r16_present                   = false;
  uint8_t                   scell_idx_r16                             = 1;
  cell_identif_r16_s_       cell_identif_r16;
  rr_cfg_common_scell_r10_s rr_cfg_common_scell_r16;
  rr_cfg_ded_scell_r10_s    rr_cfg_ded_scell_r16;
  ant_info_ded_v10i0_s      ant_info_ded_scell_r16;
  uint8_t                   srs_switch_from_serv_cell_idx_r16 = 0;
  scell_state_r16_e_        scell_state_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-CommConfig-r12 ::= SEQUENCE
struct sl_comm_cfg_r12_s {
  struct comm_tx_res_r12_c_ {
    struct setup_c_ {
      struct sched_r12_s_ {
        bool                   mcs_r12_present = false;
        fixed_bitstring<16>    sl_rnti_r12;
        mac_main_cfg_sl_r12_s  mac_main_cfg_r12;
        sl_comm_res_pool_r12_s sc_comm_tx_cfg_r12;
        uint8_t                mcs_r12 = 0;
      };
      struct ue_sel_r12_s_ {
        struct comm_tx_pool_normal_ded_r12_s_ {
          bool                                  pool_to_release_list_r12_present = false;
          bool                                  pool_to_add_mod_list_r12_present = false;
          sl_tx_pool_to_release_list_r12_l      pool_to_release_list_r12;
          sl_comm_tx_pool_to_add_mod_list_r12_l pool_to_add_mod_list_r12;
        };

        // member variables
        comm_tx_pool_normal_ded_r12_s_ comm_tx_pool_normal_ded_r12;
      };
      struct types_opts {
        enum options { sched_r12, ue_sel_r12, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      setup_c_() = default;
      setup_c_(const setup_c_& other);
      setup_c_& operator=(const setup_c_& other);
      ~setup_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      sched_r12_s_& sched_r12()
      {
        assert_choice_type(types::sched_r12, type_, "setup");
        return c.get<sched_r12_s_>();
      }
      ue_sel_r12_s_& ue_sel_r12()
      {
        assert_choice_type(types::ue_sel_r12, type_, "setup");
        return c.get<ue_sel_r12_s_>();
      }
      const sched_r12_s_& sched_r12() const
      {
        assert_choice_type(types::sched_r12, type_, "setup");
        return c.get<sched_r12_s_>();
      }
      const ue_sel_r12_s_& ue_sel_r12() const
      {
        assert_choice_type(types::ue_sel_r12, type_, "setup");
        return c.get<ue_sel_r12_s_>();
      }
      sched_r12_s_&  set_sched_r12();
      ue_sel_r12_s_& set_ue_sel_r12();

    private:
      types                                        type_;
      choice_buffer_t<sched_r12_s_, ue_sel_r12_s_> c;

      void destroy_();
    };
    using types = setup_e;

    // choice methods
    comm_tx_res_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_c_& setup()
    {
      assert_choice_type(types::setup, type_, "commTxResources-r12");
      return c;
    }
    const setup_c_& setup() const
    {
      assert_choice_type(types::setup, type_, "commTxResources-r12");
      return c;
    }
    void      set_release();
    setup_c_& set_setup();

  private:
    types    type_;
    setup_c_ c;
  };
  struct comm_tx_res_v1310_c_ {
    struct setup_c_ {
      struct sched_v1310_s_ {
        lc_ch_group_info_list_r13_l lc_ch_group_info_list_r13;
        bool                        multiple_tx_r13 = false;
      };
      struct ue_sel_v1310_s_ {
        struct comm_tx_pool_normal_ded_ext_r13_s_ {
          bool                                      pool_to_release_list_ext_r13_present = false;
          bool                                      pool_to_add_mod_list_ext_r13_present = false;
          sl_tx_pool_to_release_list_ext_r13_l      pool_to_release_list_ext_r13;
          sl_comm_tx_pool_to_add_mod_list_ext_r13_l pool_to_add_mod_list_ext_r13;
        };

        // member variables
        comm_tx_pool_normal_ded_ext_r13_s_ comm_tx_pool_normal_ded_ext_r13;
      };
      struct types_opts {
        enum options { sched_v1310, ue_sel_v1310, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      setup_c_() = default;
      setup_c_(const setup_c_& other);
      setup_c_& operator=(const setup_c_& other);
      ~setup_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      sched_v1310_s_& sched_v1310()
      {
        assert_choice_type(types::sched_v1310, type_, "setup");
        return c.get<sched_v1310_s_>();
      }
      ue_sel_v1310_s_& ue_sel_v1310()
      {
        assert_choice_type(types::ue_sel_v1310, type_, "setup");
        return c.get<ue_sel_v1310_s_>();
      }
      const sched_v1310_s_& sched_v1310() const
      {
        assert_choice_type(types::sched_v1310, type_, "setup");
        return c.get<sched_v1310_s_>();
      }
      const ue_sel_v1310_s_& ue_sel_v1310() const
      {
        assert_choice_type(types::ue_sel_v1310, type_, "setup");
        return c.get<ue_sel_v1310_s_>();
      }
      sched_v1310_s_&  set_sched_v1310();
      ue_sel_v1310_s_& set_ue_sel_v1310();

    private:
      types                                            type_;
      choice_buffer_t<sched_v1310_s_, ue_sel_v1310_s_> c;

      void destroy_();
    };
    using types = setup_e;

    // choice methods
    comm_tx_res_v1310_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_c_& setup()
    {
      assert_choice_type(types::setup, type_, "commTxResources-v1310");
      return c;
    }
    const setup_c_& setup() const
    {
      assert_choice_type(types::setup, type_, "commTxResources-v1310");
      return c;
    }
    void      set_release();
    setup_c_& set_setup();

  private:
    types    type_;
    setup_c_ c;
  };

  // member variables
  bool               ext                     = false;
  bool               comm_tx_res_r12_present = false;
  comm_tx_res_r12_c_ comm_tx_res_r12;
  // ...
  // group 0
  bool                           comm_tx_allow_relay_ded_r13_present = false;
  copy_ptr<comm_tx_res_v1310_c_> comm_tx_res_v1310;
  bool                           comm_tx_allow_relay_ded_r13 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DiscConfig-r12 ::= SEQUENCE
struct sl_disc_cfg_r12_s {
  struct disc_tx_res_r12_c_ {
    struct setup_c_ {
      struct sched_r12_s_ {
        bool                      disc_tx_cfg_r12_present      = false;
        bool                      disc_tf_idx_list_r12_present = false;
        bool                      disc_hop_cfg_r12_present     = false;
        sl_disc_res_pool_r12_s    disc_tx_cfg_r12;
        sl_tf_idx_pair_list_r12_l disc_tf_idx_list_r12;
        sl_hop_cfg_disc_r12_s     disc_hop_cfg_r12;
      };
      struct ue_sel_r12_s_ {
        struct disc_tx_pool_ded_r12_s_ {
          bool                                  pool_to_release_list_r12_present = false;
          bool                                  pool_to_add_mod_list_r12_present = false;
          sl_tx_pool_to_release_list_r12_l      pool_to_release_list_r12;
          sl_disc_tx_pool_to_add_mod_list_r12_l pool_to_add_mod_list_r12;
        };

        // member variables
        bool                    disc_tx_pool_ded_r12_present = false;
        disc_tx_pool_ded_r12_s_ disc_tx_pool_ded_r12;
      };
      struct types_opts {
        enum options { sched_r12, ue_sel_r12, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      setup_c_() = default;
      setup_c_(const setup_c_& other);
      setup_c_& operator=(const setup_c_& other);
      ~setup_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      sched_r12_s_& sched_r12()
      {
        assert_choice_type(types::sched_r12, type_, "setup");
        return c.get<sched_r12_s_>();
      }
      ue_sel_r12_s_& ue_sel_r12()
      {
        assert_choice_type(types::ue_sel_r12, type_, "setup");
        return c.get<ue_sel_r12_s_>();
      }
      const sched_r12_s_& sched_r12() const
      {
        assert_choice_type(types::sched_r12, type_, "setup");
        return c.get<sched_r12_s_>();
      }
      const ue_sel_r12_s_& ue_sel_r12() const
      {
        assert_choice_type(types::ue_sel_r12, type_, "setup");
        return c.get<ue_sel_r12_s_>();
      }
      sched_r12_s_&  set_sched_r12();
      ue_sel_r12_s_& set_ue_sel_r12();

    private:
      types                                        type_;
      choice_buffer_t<sched_r12_s_, ue_sel_r12_s_> c;

      void destroy_();
    };
    using types = setup_e;

    // choice methods
    disc_tx_res_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_c_& setup()
    {
      assert_choice_type(types::setup, type_, "discTxResources-r12");
      return c;
    }
    const setup_c_& setup() const
    {
      assert_choice_type(types::setup, type_, "discTxResources-r12");
      return c;
    }
    void      set_release();
    setup_c_& set_setup();

  private:
    types    type_;
    setup_c_ c;
  };
  struct disc_tf_idx_list_v1260_c_ {
    struct setup_s_ {
      sl_tf_idx_pair_list_r12b_l disc_tf_idx_list_r12b;
    };
    using types = setup_e;

    // choice methods
    disc_tf_idx_list_v1260_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "discTF-IndexList-v1260");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "discTF-IndexList-v1260");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct disc_tx_res_ps_r13_c_ {
    struct setup_c_ {
      struct ue_sel_r13_s_ {
        sl_disc_tx_pool_ded_r13_s disc_tx_pool_ps_ded_r13;
      };
      struct types_opts {
        enum options { sched_r13, ue_sel_r13, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      setup_c_() = default;
      setup_c_(const setup_c_& other);
      setup_c_& operator=(const setup_c_& other);
      ~setup_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      sl_disc_tx_cfg_sched_r13_s& sched_r13()
      {
        assert_choice_type(types::sched_r13, type_, "setup");
        return c.get<sl_disc_tx_cfg_sched_r13_s>();
      }
      ue_sel_r13_s_& ue_sel_r13()
      {
        assert_choice_type(types::ue_sel_r13, type_, "setup");
        return c.get<ue_sel_r13_s_>();
      }
      const sl_disc_tx_cfg_sched_r13_s& sched_r13() const
      {
        assert_choice_type(types::sched_r13, type_, "setup");
        return c.get<sl_disc_tx_cfg_sched_r13_s>();
      }
      const ue_sel_r13_s_& ue_sel_r13() const
      {
        assert_choice_type(types::ue_sel_r13, type_, "setup");
        return c.get<ue_sel_r13_s_>();
      }
      sl_disc_tx_cfg_sched_r13_s& set_sched_r13();
      ue_sel_r13_s_&              set_ue_sel_r13();

    private:
      types                                                      type_;
      choice_buffer_t<sl_disc_tx_cfg_sched_r13_s, ue_sel_r13_s_> c;

      void destroy_();
    };
    using types = setup_e;

    // choice methods
    disc_tx_res_ps_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_c_& setup()
    {
      assert_choice_type(types::setup, type_, "discTxResourcesPS-r13");
      return c;
    }
    const setup_c_& setup() const
    {
      assert_choice_type(types::setup, type_, "discTxResourcesPS-r13");
      return c;
    }
    void      set_release();
    setup_c_& set_setup();

  private:
    types    type_;
    setup_c_ c;
  };
  struct disc_tx_inter_freq_info_r13_c_ {
    struct setup_s_ {
      bool                                      disc_tx_carrier_freq_r13_present             = false;
      bool                                      disc_tx_ref_carrier_ded_r13_present          = false;
      bool                                      disc_tx_info_inter_freq_list_add_r13_present = false;
      uint32_t                                  disc_tx_carrier_freq_r13                     = 0;
      sl_disc_tx_ref_carrier_ded_r13_c          disc_tx_ref_carrier_ded_r13;
      sl_disc_tx_info_inter_freq_list_add_r13_s disc_tx_info_inter_freq_list_add_r13;
    };
    using types = setup_e;

    // choice methods
    disc_tx_inter_freq_info_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "discTxInterFreqInfo-r13");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "discTxInterFreqInfo-r13");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct disc_rx_gap_cfg_r13_c_ {
    using types = setup_e;

    // choice methods
    disc_rx_gap_cfg_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_gap_cfg_r13_s& setup()
    {
      assert_choice_type(types::setup, type_, "discRxGapConfig-r13");
      return c;
    }
    const sl_gap_cfg_r13_s& setup() const
    {
      assert_choice_type(types::setup, type_, "discRxGapConfig-r13");
      return c;
    }
    void              set_release();
    sl_gap_cfg_r13_s& set_setup();

  private:
    types            type_;
    sl_gap_cfg_r13_s c;
  };
  struct disc_tx_gap_cfg_r13_c_ {
    using types = setup_e;

    // choice methods
    disc_tx_gap_cfg_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_gap_cfg_r13_s& setup()
    {
      assert_choice_type(types::setup, type_, "discTxGapConfig-r13");
      return c;
    }
    const sl_gap_cfg_r13_s& setup() const
    {
      assert_choice_type(types::setup, type_, "discTxGapConfig-r13");
      return c;
    }
    void              set_release();
    sl_gap_cfg_r13_s& set_setup();

  private:
    types            type_;
    sl_gap_cfg_r13_s c;
  };
  struct disc_sys_info_to_report_cfg_r13_c_ {
    using types = setup_e;

    // choice methods
    disc_sys_info_to_report_cfg_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_disc_sys_info_to_report_freq_list_r13_l& setup()
    {
      assert_choice_type(types::setup, type_, "discSysInfoToReportConfig-r13");
      return c;
    }
    const sl_disc_sys_info_to_report_freq_list_r13_l& setup() const
    {
      assert_choice_type(types::setup, type_, "discSysInfoToReportConfig-r13");
      return c;
    }
    void                                        set_release();
    sl_disc_sys_info_to_report_freq_list_r13_l& set_setup();

  private:
    types                                      type_;
    sl_disc_sys_info_to_report_freq_list_r13_l c;
  };

  // member variables
  bool               ext                     = false;
  bool               disc_tx_res_r12_present = false;
  disc_tx_res_r12_c_ disc_tx_res_r12;
  // ...
  // group 0
  copy_ptr<disc_tf_idx_list_v1260_c_> disc_tf_idx_list_v1260;
  // group 1
  bool                                         gap_requests_allowed_ded_r13_present = false;
  copy_ptr<disc_tx_res_ps_r13_c_>              disc_tx_res_ps_r13;
  copy_ptr<disc_tx_inter_freq_info_r13_c_>     disc_tx_inter_freq_info_r13;
  bool                                         gap_requests_allowed_ded_r13 = false;
  copy_ptr<disc_rx_gap_cfg_r13_c_>             disc_rx_gap_cfg_r13;
  copy_ptr<disc_tx_gap_cfg_r13_c_>             disc_tx_gap_cfg_r13;
  copy_ptr<disc_sys_info_to_report_cfg_r13_c_> disc_sys_info_to_report_cfg_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-SyncTxControl-r12 ::= SEQUENCE
struct sl_sync_tx_ctrl_r12_s {
  struct network_ctrl_sync_tx_r12_opts {
    enum options { on, off, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<network_ctrl_sync_tx_r12_opts> network_ctrl_sync_tx_r12_e_;

  // member variables
  bool                        network_ctrl_sync_tx_r12_present = false;
  network_ctrl_sync_tx_r12_e_ network_ctrl_sync_tx_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationRequest-v1710-IEs ::= SEQUENCE
struct ue_info_request_v1710_ies_s {
  bool coarse_location_req_r17_present = false;
  bool non_crit_ext_present            = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateServingFreqListNR-r15 ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..3279165)
using candidate_serving_freq_list_nr_r15_l = bounded_array<uint32_t, 32>;

// RRCConnectionReconfiguration-v1250-IEs ::= SEQUENCE
struct rrc_conn_recfg_v1250_ies_s {
  struct wlan_offload_info_r12_c_ {
    struct setup_s_ {
      struct t350_r12_opts {
        enum options { min5, min10, min20, min30, min60, min120, min180, spare1, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<t350_r12_opts> t350_r12_e_;

      // member variables
      bool                   t350_r12_present = false;
      wlan_offload_cfg_r12_s wlan_offload_cfg_ded_r12;
      t350_r12_e_            t350_r12;
    };
    using types = setup_e;

    // choice methods
    wlan_offload_info_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "wlan-OffloadInfo-r12");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "wlan-OffloadInfo-r12");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                       wlan_offload_info_r12_present = false;
  bool                       scg_cfg_r12_present           = false;
  bool                       sl_sync_tx_ctrl_r12_present   = false;
  bool                       sl_disc_cfg_r12_present       = false;
  bool                       sl_comm_cfg_r12_present       = false;
  bool                       non_crit_ext_present          = false;
  wlan_offload_info_r12_c_   wlan_offload_info_r12;
  scg_cfg_r12_c              scg_cfg_r12;
  sl_sync_tx_ctrl_r12_s      sl_sync_tx_ctrl_r12;
  sl_disc_cfg_r12_s          sl_disc_cfg_r12;
  sl_comm_cfg_r12_s          sl_comm_cfg_r12;
  rrc_conn_recfg_v1310_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-v1530-IEs ::= SEQUENCE
struct rrc_conn_release_v1530_ies_s {
  struct cn_type_r15_opts {
    enum options { epc, fivegc, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<cn_type_r15_opts> cn_type_r15_e_;

  // member variables
  bool                         drb_continue_rohc_r15_present       = false;
  bool                         next_hop_chaining_count_r15_present = false;
  bool                         meas_idle_cfg_r15_present           = false;
  bool                         rrc_inactive_cfg_r15_present        = false;
  bool                         cn_type_r15_present                 = false;
  bool                         non_crit_ext_present                = false;
  uint8_t                      next_hop_chaining_count_r15         = 0;
  meas_idle_cfg_ded_r15_s      meas_idle_cfg_r15;
  rrc_inactive_cfg_r15_s       rrc_inactive_cfg_r15;
  cn_type_r15_e_               cn_type_r15;
  rrc_conn_release_v1540_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResume-v1700-IEs ::= SEQUENCE
struct rrc_conn_resume_v1700_ies_s {
  bool scg_state_r17_present = false;
  bool non_crit_ext_present  = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellToAddModList-r16 ::= SEQUENCE (SIZE (1..31)) OF SCellToAddMod-r16
using scell_to_add_mod_list_r16_l = dyn_array<scell_to_add_mod_r16_s>;

// SystemInfoListGERAN ::= SEQUENCE (SIZE (1..10)) OF OCTET STRING (SIZE (1..23))
using sys_info_list_geran_l = bounded_array<bounded_octstring<1, 23>, 10>;

// TargetMBSFN-Area-r12 ::= SEQUENCE
struct target_mbsfn_area_r12_s {
  bool     ext                       = false;
  bool     mbsfn_area_id_r12_present = false;
  uint16_t mbsfn_area_id_r12         = 0;
  uint32_t carrier_freq_r12          = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationRequest-v1530-IEs ::= SEQUENCE
struct ue_info_request_v1530_ies_s {
  bool                              idle_mode_meas_req_r15_present   = false;
  bool                              flight_path_info_req_r15_present = false;
  bool                              non_crit_ext_present             = false;
  flight_path_info_report_cfg_r15_s flight_path_info_req_r15;
  ue_info_request_v1710_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellInfoGERAN-r9 ::= SEQUENCE
struct cell_info_geran_r9_s {
  pci_geran_s           pci_r9;
  carrier_freq_geran_s  carrier_freq_r9;
  sys_info_list_geran_l sys_info_r9;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellInfoUTRA-FDD-r9 ::= SEQUENCE
struct cell_info_utra_fdd_r9_s {
  uint16_t      pci_r9 = 0;
  dyn_octstring utra_bcch_container_r9;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellInfoUTRA-TDD-r10 ::= SEQUENCE
struct cell_info_utra_tdd_r10_s {
  uint8_t       pci_r10          = 0;
  uint16_t      carrier_freq_r10 = 0;
  dyn_octstring utra_bcch_container_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellInfoUTRA-TDD-r9 ::= SEQUENCE
struct cell_info_utra_tdd_r9_s {
  uint8_t       pci_r9 = 0;
  dyn_octstring utra_bcch_container_r9;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-CountMSB-Info ::= SEQUENCE
struct drb_count_msb_info_s {
  uint8_t  drb_id       = 1;
  uint32_t count_msb_ul = 0;
  uint32_t count_msb_dl = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IDC-Config-r11 ::= SEQUENCE
struct idc_cfg_r11_s {
  struct autonomous_denial_params_r11_s_ {
    struct autonomous_denial_sfs_r11_opts {
      enum options { n2, n5, n10, n15, n20, n30, spare2, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<autonomous_denial_sfs_r11_opts> autonomous_denial_sfs_r11_e_;
    struct autonomous_denial_validity_r11_opts {
      enum options { sf200, sf500, sf1000, sf2000, spare4, spare3, spare2, spare1, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<autonomous_denial_validity_r11_opts> autonomous_denial_validity_r11_e_;

    // member variables
    autonomous_denial_sfs_r11_e_      autonomous_denial_sfs_r11;
    autonomous_denial_validity_r11_e_ autonomous_denial_validity_r11;
  };
  struct idc_ind_mrdc_r15_c_ {
    using types = setup_e;

    // choice methods
    idc_ind_mrdc_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    candidate_serving_freq_list_nr_r15_l& setup()
    {
      assert_choice_type(types::setup, type_, "idc-Indication-MRDC-r15");
      return c;
    }
    const candidate_serving_freq_list_nr_r15_l& setup() const
    {
      assert_choice_type(types::setup, type_, "idc-Indication-MRDC-r15");
      return c;
    }
    void                                  set_release();
    candidate_serving_freq_list_nr_r15_l& set_setup();

  private:
    types                                type_;
    candidate_serving_freq_list_nr_r15_l c;
  };

  // member variables
  bool                            ext                                  = false;
  bool                            idc_ind_r11_present                  = false;
  bool                            autonomous_denial_params_r11_present = false;
  autonomous_denial_params_r11_s_ autonomous_denial_params_r11;
  // ...
  // group 0
  bool idc_ind_ul_ca_r11_present = false;
  // group 1
  bool idc_hardware_sharing_ind_r13_present = false;
  // group 2
  copy_ptr<idc_ind_mrdc_r15_c_> idc_ind_mrdc_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ObtainLocationConfig-r11 ::= SEQUENCE
struct obtain_location_cfg_r11_s {
  bool obtain_location_r11_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PowerPrefIndicationConfig-r11 ::= CHOICE
struct pwr_pref_ind_cfg_r11_c {
  struct setup_s_ {
    struct pwr_pref_ind_timer_r11_opts {
      enum options {
        s0,
        s0dot5,
        s1,
        s2,
        s5,
        s10,
        s20,
        s30,
        s60,
        s90,
        s120,
        s300,
        s600,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<pwr_pref_ind_timer_r11_opts> pwr_pref_ind_timer_r11_e_;

    // member variables
    pwr_pref_ind_timer_r11_e_ pwr_pref_ind_timer_r11;
  };
  using types = setup_e;

  // choice methods
  pwr_pref_ind_cfg_r11_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "PowerPrefIndicationConfig-r11");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "PowerPrefIndicationConfig-r11");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// RRCConnectionReconfiguration-v1130-IEs ::= SEQUENCE
struct rrc_conn_recfg_v1130_ies_s {
  bool                       sib_type1_ded_r11_present = false;
  bool                       non_crit_ext_present      = false;
  dyn_octstring              sib_type1_ded_r11;
  rrc_conn_recfg_v1250_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-v1320-IEs ::= SEQUENCE
struct rrc_conn_release_v1320_ies_s {
  bool                         resume_id_r13_present = false;
  bool                         non_crit_ext_present  = false;
  fixed_bitstring<40>          resume_id_r13;
  rrc_conn_release_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResume-v1610-IEs ::= SEQUENCE
struct rrc_conn_resume_v1610_ies_s {
  bool                              idle_mode_meas_req_r16_present          = false;
  bool                              restore_mcg_scells_r16_present          = false;
  bool                              restore_scg_r16_present                 = false;
  bool                              scell_to_add_mod_list_r16_present       = false;
  bool                              scell_to_release_list_r16_present       = false;
  bool                              scell_group_to_release_list_r16_present = false;
  bool                              scell_group_to_add_mod_list_r16_present = false;
  bool                              nr_secondary_cell_group_cfg_r16_present = false;
  bool                              p_max_eutra_r16_present                 = false;
  bool                              p_max_ue_fr1_r16_present                = false;
  bool                              tdm_pattern_cfg_r16_present             = false;
  bool                              tdm_pattern_cfg2_r16_present            = false;
  bool                              non_crit_ext_present                    = false;
  scell_to_add_mod_list_r16_l       scell_to_add_mod_list_r16;
  scell_to_release_list_ext_r13_l   scell_to_release_list_r16;
  scell_group_to_release_list_r15_l scell_group_to_release_list_r16;
  scell_group_to_add_mod_list_r15_l scell_group_to_add_mod_list_r16;
  dyn_octstring                     nr_secondary_cell_group_cfg_r16;
  int8_t                            p_max_eutra_r16  = -30;
  int8_t                            p_max_ue_fr1_r16 = -30;
  tdm_pattern_cfg_r15_c             tdm_pattern_cfg_r16;
  tdm_pattern_cfg_r15_c             tdm_pattern_cfg2_r16;
  rrc_conn_resume_v1700_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportProximityConfig-r9 ::= SEQUENCE
struct report_proximity_cfg_r9_s {
  bool proximity_ind_eutra_r9_present = false;
  bool proximity_ind_utra_r9_present  = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-DeactivationPreferenceConfig-r17 ::= SEQUENCE
struct scg_deactivation_pref_cfg_r17_s {
  struct scg_deactivation_pref_prohibit_timer_r17_opts {
    enum options { s0, s1, s2, s4, s8, s10, s20, s30, s60, s120, s180, s240, s300, s600, s900, s1800, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<scg_deactivation_pref_prohibit_timer_r17_opts> scg_deactivation_pref_prohibit_timer_r17_e_;

  // member variables
  scg_deactivation_pref_prohibit_timer_r17_e_ scg_deactivation_pref_prohibit_timer_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargetMBSFN-AreaList-r12 ::= SEQUENCE (SIZE (0..8)) OF TargetMBSFN-Area-r12
using target_mbsfn_area_list_r12_l = dyn_array<target_mbsfn_area_r12_s>;

// TrackingAreaCodeList-v1130 ::= SEQUENCE
struct tac_list_v1130_s {
  using plmn_id_per_tac_list_r11_l_ = dyn_array<plmn_id_s>;

  // member variables
  plmn_id_per_tac_list_r11_l_ plmn_id_per_tac_list_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationRequest-v1250-IEs ::= SEQUENCE
struct ue_info_request_v1250_ies_s {
  bool                        mob_history_report_req_r12_present = false;
  bool                        non_crit_ext_present               = false;
  ue_info_request_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaConfiguration-v1130 ::= SEQUENCE
struct area_cfg_v1130_s {
  tac_list_v1130_s tac_list_v1130;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellInfoListGERAN-r9 ::= SEQUENCE (SIZE (1..32)) OF CellInfoGERAN-r9
using cell_info_list_geran_r9_l = dyn_array<cell_info_geran_r9_s>;

// CellInfoListUTRA-FDD-r9 ::= SEQUENCE (SIZE (1..16)) OF CellInfoUTRA-FDD-r9
using cell_info_list_utra_fdd_r9_l = dyn_array<cell_info_utra_fdd_r9_s>;

// CellInfoListUTRA-TDD-r10 ::= SEQUENCE (SIZE (1..16)) OF CellInfoUTRA-TDD-r10
using cell_info_list_utra_tdd_r10_l = dyn_array<cell_info_utra_tdd_r10_s>;

// CellInfoListUTRA-TDD-r9 ::= SEQUENCE (SIZE (1..16)) OF CellInfoUTRA-TDD-r9
using cell_info_list_utra_tdd_r9_l = dyn_array<cell_info_utra_tdd_r9_s>;

// DRB-CountMSB-InfoListExt-r15 ::= SEQUENCE (SIZE (1..4)) OF DRB-CountMSB-Info
using drb_count_msb_info_list_ext_r15_l = dyn_array<drb_count_msb_info_s>;

// HandoverFromEUTRAPreparationRequest-v1020-IEs ::= SEQUENCE
struct ho_from_eutra_prep_request_v1020_ies_s {
  bool                    dual_rx_tx_redirect_ind_r10_present               = false;
  bool                    redirect_carrier_cdma2000_minus1_xrtt_r10_present = false;
  bool                    non_crit_ext_present                              = false;
  carrier_freq_cdma2000_s redirect_carrier_cdma2000_minus1_xrtt_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMeasurementConfiguration-v1250-IEs ::= SEQUENCE
struct logged_meas_cfg_v1250_ies_s {
  bool                         target_mbsfn_area_list_r12_present = false;
  bool                         non_crit_ext_present               = false;
  target_mbsfn_area_list_r12_l target_mbsfn_area_list_r12;
  logged_meas_cfg_v1530_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromEUTRACommand-v1530-IEs ::= SEQUENCE
struct mob_from_eutra_cmd_v1530_ies_s {
  bool             smtc_r15_present     = false;
  bool             non_crit_ext_present = false;
  mtc_ssb_nr_r15_s smtc_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OtherConfig-r9 ::= SEQUENCE
struct other_cfg_r9_s {
  struct bw_pref_ind_timer_r14_opts {
    enum options {
      s0,
      s0dot5,
      s1,
      s2,
      s5,
      s10,
      s20,
      s30,
      s60,
      s90,
      s120,
      s300,
      s600,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<bw_pref_ind_timer_r14_opts> bw_pref_ind_timer_r14_e_;
  struct delay_budget_report_cfg_r14_c_ {
    struct setup_s_ {
      struct delay_budget_report_prohibit_timer_r14_opts {
        enum options { s0, s0dot4, s0dot8, s1dot6, s3, s6, s12, s30, nulltype } value;
        typedef float number_type;

        const char* to_string() const;
        float       to_number() const;
        const char* to_number_string() const;
      };
      typedef enumerated<delay_budget_report_prohibit_timer_r14_opts> delay_budget_report_prohibit_timer_r14_e_;

      // member variables
      delay_budget_report_prohibit_timer_r14_e_ delay_budget_report_prohibit_timer_r14;
    };
    using types = setup_e;

    // choice methods
    delay_budget_report_cfg_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "delayBudgetReportingConfig-r14");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "delayBudgetReportingConfig-r14");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct rlm_report_cfg_r14_c_ {
    struct setup_s_ {
      struct rlm_report_timer_r14_opts {
        enum options {
          s0,
          s0dot5,
          s1,
          s2,
          s5,
          s10,
          s20,
          s30,
          s60,
          s90,
          s120,
          s300,
          s600,
          spare3,
          spare2,
          spare1,
          nulltype
        } value;
        typedef float number_type;

        const char* to_string() const;
        float       to_number() const;
        const char* to_number_string() const;
      };
      typedef enumerated<rlm_report_timer_r14_opts> rlm_report_timer_r14_e_;

      // member variables
      bool                    rlm_report_rep_mpdcch_r14_present = false;
      rlm_report_timer_r14_e_ rlm_report_timer_r14;
    };
    using types = setup_e;

    // choice methods
    rlm_report_cfg_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "rlm-ReportConfig-r14");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "rlm-ReportConfig-r14");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct overheat_assist_cfg_r14_c_ {
    struct setup_s_ {
      struct overheat_ind_prohibit_timer_r14_opts {
        enum options {
          s0,
          s0dot5,
          s1,
          s2,
          s5,
          s10,
          s20,
          s30,
          s60,
          s90,
          s120,
          s300,
          s600,
          spare3,
          spare2,
          spare1,
          nulltype
        } value;
        typedef float number_type;

        const char* to_string() const;
        float       to_number() const;
        const char* to_number_string() const;
      };
      typedef enumerated<overheat_ind_prohibit_timer_r14_opts> overheat_ind_prohibit_timer_r14_e_;

      // member variables
      overheat_ind_prohibit_timer_r14_e_ overheat_ind_prohibit_timer_r14;
    };
    using types = setup_e;

    // choice methods
    overheat_assist_cfg_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "overheatingAssistanceConfig-r14");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "overheatingAssistanceConfig-r14");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct meas_cfg_app_layer_r15_c_ {
    struct setup_s_ {
      struct service_type_r15_opts {
        enum options { qoe, qoemtsi, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<service_type_r15_opts> service_type_r15_e_;

      // member variables
      bounded_octstring<1, 1000> meas_cfg_app_layer_container_r15;
      service_type_r15_e_        service_type_r15;
    };
    using types = setup_e;

    // choice methods
    meas_cfg_app_layer_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "measConfigAppLayer-r15");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "measConfigAppLayer-r15");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                      ext                             = false;
  bool                      report_proximity_cfg_r9_present = false;
  report_proximity_cfg_r9_s report_proximity_cfg_r9;
  // ...
  // group 0
  copy_ptr<idc_cfg_r11_s>             idc_cfg_r11;
  copy_ptr<pwr_pref_ind_cfg_r11_c>    pwr_pref_ind_cfg_r11;
  copy_ptr<obtain_location_cfg_r11_s> obtain_location_cfg_r11;
  // group 1
  bool                                     bw_pref_ind_timer_r14_present      = false;
  bool                                     sps_assist_info_report_r14_present = false;
  bw_pref_ind_timer_r14_e_                 bw_pref_ind_timer_r14;
  bool                                     sps_assist_info_report_r14 = false;
  copy_ptr<delay_budget_report_cfg_r14_c_> delay_budget_report_cfg_r14;
  copy_ptr<rlm_report_cfg_r14_c_>          rlm_report_cfg_r14;
  // group 2
  copy_ptr<overheat_assist_cfg_r14_c_> overheat_assist_cfg_r14;
  // group 3
  bool                                ailc_bit_cfg_r15_present = false;
  copy_ptr<meas_cfg_app_layer_r15_c_> meas_cfg_app_layer_r15;
  bool                                ailc_bit_cfg_r15 = false;
  copy_ptr<bt_name_list_cfg_r15_c>    bt_name_list_cfg_r15;
  copy_ptr<wlan_name_list_cfg_r15_c>  wlan_name_list_cfg_r15;
  // group 4
  bool overheat_assist_cfg_for_scg_r16_present = false;
  bool overheat_assist_cfg_for_scg_r16         = false;
  // group 5
  bool                                                        meas_uncom_bar_pre_r17_present = false;
  bool                                                        meas_uncom_bar_pre_r17         = false;
  copy_ptr<setup_release_c<scg_deactivation_pref_cfg_r17_s> > scg_deactivation_pref_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityList3-r11 ::= SEQUENCE (SIZE (1..16)) OF PLMN-Identity
using plmn_id_list3_r11_l = dyn_array<plmn_id_s>;

// RRCConnectionReconfiguration-v1020-IEs ::= SEQUENCE
struct rrc_conn_recfg_v1020_ies_s {
  bool                        scell_to_release_list_r10_present = false;
  bool                        scell_to_add_mod_list_r10_present = false;
  bool                        non_crit_ext_present              = false;
  scell_to_release_list_r10_l scell_to_release_list_r10;
  scell_to_add_mod_list_r10_l scell_to_add_mod_list_r10;
  rrc_conn_recfg_v1130_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-v1020-IEs ::= SEQUENCE
struct rrc_conn_release_v1020_ies_s {
  bool                         extended_wait_time_r10_present = false;
  bool                         non_crit_ext_present           = false;
  uint16_t                     extended_wait_time_r10         = 1;
  rrc_conn_release_v1320_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResume-v1530-IEs ::= SEQUENCE
struct rrc_conn_resume_v1530_ies_s {
  bool                        full_cfg_r15_present = false;
  bool                        non_crit_ext_present = false;
  rrc_conn_resume_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationRequest-v1130-IEs ::= SEQUENCE
struct ue_info_request_v1130_ies_s {
  bool                        conn_est_fail_report_req_r11_present = false;
  bool                        non_crit_ext_present                 = false;
  ue_info_request_v1250_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierInfoNR-r15 ::= SEQUENCE
struct carrier_info_nr_r15_s {
  struct subcarrier_spacing_ssb_r15_opts {
    enum options { khz15, khz30, khz120, khz240, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<subcarrier_spacing_ssb_r15_opts> subcarrier_spacing_ssb_r15_e_;

  // member variables
  bool                          smtc_r15_present = false;
  uint32_t                      carrier_freq_r15 = 0;
  subcarrier_spacing_ssb_r15_e_ subcarrier_spacing_ssb_r15;
  mtc_ssb_nr_r15_s              smtc_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierInfoNR-r17 ::= SEQUENCE
struct carrier_info_nr_r17_s {
  struct subcarrier_spacing_ssb_r17_opts {
    enum options { khz15, khz30, khz120, khz240, khz480, spare1, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<subcarrier_spacing_ssb_r17_opts> subcarrier_spacing_ssb_r17_e_;

  // member variables
  bool                          smtc_r17_present = false;
  uint32_t                      carrier_freq_r17 = 0;
  subcarrier_spacing_ssb_r17_e_ subcarrier_spacing_ssb_r17;
  mtc_ssb_nr_r15_s              smtc_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellGlobalIdList-r10 ::= SEQUENCE (SIZE (1..32)) OF CellGlobalIdEUTRA
using cell_global_id_list_r10_l = dyn_array<cell_global_id_eutra_s>;

// CounterCheck-v1530-IEs ::= SEQUENCE
struct counter_check_v1530_ies_s {
  bool                              drb_count_msb_info_list_ext_r15_present = false;
  bool                              non_crit_ext_present                    = false;
  drb_count_msb_info_list_ext_r15_l drb_count_msb_info_list_ext_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer-v1610-IEs ::= SEQUENCE
struct dl_info_transfer_v1610_ies_s {
  bool          ded_info_f1c_r16_present = false;
  bool          non_crit_ext_present     = false;
  dyn_octstring ded_info_f1c_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverFromEUTRAPreparationRequest-v920-IEs ::= SEQUENCE
struct ho_from_eutra_prep_request_v920_ies_s {
  bool                                   concurr_prep_cdma2000_hrpd_r9_present = false;
  bool                                   non_crit_ext_present                  = false;
  bool                                   concurr_prep_cdma2000_hrpd_r9         = false;
  ho_from_eutra_prep_request_v1020_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMeasurementConfiguration-v1130-IEs ::= SEQUENCE
struct logged_meas_cfg_v1130_ies_s {
  bool                        plmn_id_list_r11_present = false;
  bool                        area_cfg_v1130_present   = false;
  bool                        non_crit_ext_present     = false;
  plmn_id_list3_r11_l         plmn_id_list_r11;
  area_cfg_v1130_s            area_cfg_v1130;
  logged_meas_cfg_v1250_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromEUTRACommand-v8d0-IEs ::= SEQUENCE
struct mob_from_eutra_cmd_v8d0_ies_s {
  bool             band_ind_present     = false;
  bool             non_crit_ext_present = false;
  band_ind_geran_e band_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromEUTRACommand-v960-IEs ::= SEQUENCE
struct mob_from_eutra_cmd_v960_ies_s {
  bool                           band_ind_present     = false;
  bool                           non_crit_ext_present = false;
  band_ind_geran_e               band_ind;
  mob_from_eutra_cmd_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-v920-IEs ::= SEQUENCE
struct rrc_conn_recfg_v920_ies_s {
  bool                       other_cfg_r9_present = false;
  bool                       full_cfg_r9_present  = false;
  bool                       non_crit_ext_present = false;
  other_cfg_r9_s             other_cfg_r9;
  rrc_conn_recfg_v1020_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-v920-IEs ::= SEQUENCE
struct rrc_conn_release_v920_ies_s {
  struct cell_info_list_r9_c_ {
    struct types_opts {
      enum options { geran_r9, utra_fdd_r9, utra_tdd_r9, /*...*/ utra_tdd_r10, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true, 1> types;

    // choice methods
    cell_info_list_r9_c_() = default;
    cell_info_list_r9_c_(const cell_info_list_r9_c_& other);
    cell_info_list_r9_c_& operator=(const cell_info_list_r9_c_& other);
    ~cell_info_list_r9_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cell_info_list_geran_r9_l& geran_r9()
    {
      assert_choice_type(types::geran_r9, type_, "cellInfoList-r9");
      return c.get<cell_info_list_geran_r9_l>();
    }
    cell_info_list_utra_fdd_r9_l& utra_fdd_r9()
    {
      assert_choice_type(types::utra_fdd_r9, type_, "cellInfoList-r9");
      return c.get<cell_info_list_utra_fdd_r9_l>();
    }
    cell_info_list_utra_tdd_r9_l& utra_tdd_r9()
    {
      assert_choice_type(types::utra_tdd_r9, type_, "cellInfoList-r9");
      return c.get<cell_info_list_utra_tdd_r9_l>();
    }
    cell_info_list_utra_tdd_r10_l& utra_tdd_r10()
    {
      assert_choice_type(types::utra_tdd_r10, type_, "cellInfoList-r9");
      return c.get<cell_info_list_utra_tdd_r10_l>();
    }
    const cell_info_list_geran_r9_l& geran_r9() const
    {
      assert_choice_type(types::geran_r9, type_, "cellInfoList-r9");
      return c.get<cell_info_list_geran_r9_l>();
    }
    const cell_info_list_utra_fdd_r9_l& utra_fdd_r9() const
    {
      assert_choice_type(types::utra_fdd_r9, type_, "cellInfoList-r9");
      return c.get<cell_info_list_utra_fdd_r9_l>();
    }
    const cell_info_list_utra_tdd_r9_l& utra_tdd_r9() const
    {
      assert_choice_type(types::utra_tdd_r9, type_, "cellInfoList-r9");
      return c.get<cell_info_list_utra_tdd_r9_l>();
    }
    const cell_info_list_utra_tdd_r10_l& utra_tdd_r10() const
    {
      assert_choice_type(types::utra_tdd_r10, type_, "cellInfoList-r9");
      return c.get<cell_info_list_utra_tdd_r10_l>();
    }
    cell_info_list_geran_r9_l&     set_geran_r9();
    cell_info_list_utra_fdd_r9_l&  set_utra_fdd_r9();
    cell_info_list_utra_tdd_r9_l&  set_utra_tdd_r9();
    cell_info_list_utra_tdd_r10_l& set_utra_tdd_r10();

  private:
    types type_;
    choice_buffer_t<cell_info_list_geran_r9_l,
                    cell_info_list_utra_fdd_r9_l,
                    cell_info_list_utra_tdd_r10_l,
                    cell_info_list_utra_tdd_r9_l>
        c;

    void destroy_();
  };

  // member variables
  bool                         cell_info_list_r9_present = false;
  bool                         non_crit_ext_present      = false;
  cell_info_list_r9_c_         cell_info_list_r9;
  rrc_conn_release_v1020_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResume-v1510-IEs ::= SEQUENCE
struct rrc_conn_resume_v1510_ies_s {
  bool                        sk_counter_r15_present           = false;
  bool                        nr_radio_bearer_cfg1_r15_present = false;
  bool                        nr_radio_bearer_cfg2_r15_present = false;
  bool                        non_crit_ext_present             = false;
  uint32_t                    sk_counter_r15                   = 0;
  dyn_octstring               nr_radio_bearer_cfg1_r15;
  dyn_octstring               nr_radio_bearer_cfg2_r15;
  rrc_conn_resume_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SI-OrPSI-GERAN ::= CHOICE
struct si_or_psi_geran_c {
  struct types_opts {
    enum options { si, psi, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  si_or_psi_geran_c() = default;
  si_or_psi_geran_c(const si_or_psi_geran_c& other);
  si_or_psi_geran_c& operator=(const si_or_psi_geran_c& other);
  ~si_or_psi_geran_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  sys_info_list_geran_l& si()
  {
    assert_choice_type(types::si, type_, "SI-OrPSI-GERAN");
    return c.get<sys_info_list_geran_l>();
  }
  sys_info_list_geran_l& psi()
  {
    assert_choice_type(types::psi, type_, "SI-OrPSI-GERAN");
    return c.get<sys_info_list_geran_l>();
  }
  const sys_info_list_geran_l& si() const
  {
    assert_choice_type(types::si, type_, "SI-OrPSI-GERAN");
    return c.get<sys_info_list_geran_l>();
  }
  const sys_info_list_geran_l& psi() const
  {
    assert_choice_type(types::psi, type_, "SI-OrPSI-GERAN");
    return c.get<sys_info_list_geran_l>();
  }
  sys_info_list_geran_l& set_si();
  sys_info_list_geran_l& set_psi();

private:
  types                                  type_;
  choice_buffer_t<sys_info_list_geran_l> c;

  void destroy_();
};

// TrackingAreaCodeList-r10 ::= SEQUENCE (SIZE (1..8)) OF BIT STRING (SIZE (16))
using tac_list_r10_l = bounded_array<fixed_bitstring<16>, 8>;

// UEInformationRequest-v1020-IEs ::= SEQUENCE
struct ue_info_request_v1020_ies_s {
  bool                        log_meas_report_req_r10_present = false;
  bool                        non_crit_ext_present            = false;
  ue_info_request_v1130_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaConfiguration-r10 ::= CHOICE
struct area_cfg_r10_c {
  struct types_opts {
    enum options { cell_global_id_list_r10, tac_list_r10, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  area_cfg_r10_c() = default;
  area_cfg_r10_c(const area_cfg_r10_c& other);
  area_cfg_r10_c& operator=(const area_cfg_r10_c& other);
  ~area_cfg_r10_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_global_id_list_r10_l& cell_global_id_list_r10()
  {
    assert_choice_type(types::cell_global_id_list_r10, type_, "AreaConfiguration-r10");
    return c.get<cell_global_id_list_r10_l>();
  }
  tac_list_r10_l& tac_list_r10()
  {
    assert_choice_type(types::tac_list_r10, type_, "AreaConfiguration-r10");
    return c.get<tac_list_r10_l>();
  }
  const cell_global_id_list_r10_l& cell_global_id_list_r10() const
  {
    assert_choice_type(types::cell_global_id_list_r10, type_, "AreaConfiguration-r10");
    return c.get<cell_global_id_list_r10_l>();
  }
  const tac_list_r10_l& tac_list_r10() const
  {
    assert_choice_type(types::tac_list_r10, type_, "AreaConfiguration-r10");
    return c.get<tac_list_r10_l>();
  }
  cell_global_id_list_r10_l& set_cell_global_id_list_r10();
  tac_list_r10_l&            set_tac_list_r10();

private:
  types                                                      type_;
  choice_buffer_t<cell_global_id_list_r10_l, tac_list_r10_l> c;

  void destroy_();
};

// CSFBParametersResponseCDMA2000-v8a0-IEs ::= SEQUENCE
struct csfb_params_resp_cdma2000_v8a0_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellChangeOrder ::= SEQUENCE
struct cell_change_order_s {
  struct t304_opts {
    enum options { ms100, ms200, ms500, ms1000, ms2000, ms4000, ms8000, ms10000_v1310, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t304_opts> t304_e_;
  struct target_rat_type_c_ {
    struct geran_s_ {
      bool                 network_ctrl_order_present = false;
      bool                 sys_info_present           = false;
      pci_geran_s          pci;
      carrier_freq_geran_s carrier_freq;
      fixed_bitstring<2>   network_ctrl_order;
      si_or_psi_geran_c    sys_info;
    };
    struct types_opts {
      enum options { geran, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    types       type() const { return types::geran; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    geran_s_&       geran() { return c; }
    const geran_s_& geran() const { return c; }

  private:
    geran_s_ c;
  };

  // member variables
  t304_e_            t304;
  target_rat_type_c_ target_rat_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheck-v8a0-IEs ::= SEQUENCE
struct counter_check_v8a0_ies_s {
  bool                      late_non_crit_ext_present = false;
  bool                      non_crit_ext_present      = false;
  dyn_octstring             late_non_crit_ext;
  counter_check_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer-v8a0-IEs ::= SEQUENCE
struct dl_info_transfer_v8a0_ies_s {
  bool                         late_non_crit_ext_present = false;
  bool                         non_crit_ext_present      = false;
  dyn_octstring                late_non_crit_ext;
  dl_info_transfer_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-CountMSB-InfoList ::= SEQUENCE (SIZE (1..11)) OF DRB-CountMSB-Info
using drb_count_msb_info_list_l = dyn_array<drb_count_msb_info_s>;

// E-CSFB-r9 ::= SEQUENCE
struct e_csfb_r9_s {
  struct mob_cdma2000_hrpd_r9_opts {
    enum options { ho, redirection, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<mob_cdma2000_hrpd_r9_opts> mob_cdma2000_hrpd_r9_e_;

  // member variables
  bool                    msg_cont_cdma2000_minus1_xrtt_r9_present  = false;
  bool                    mob_cdma2000_hrpd_r9_present              = false;
  bool                    msg_cont_cdma2000_hrpd_r9_present         = false;
  bool                    redirect_carrier_cdma2000_hrpd_r9_present = false;
  dyn_octstring           msg_cont_cdma2000_minus1_xrtt_r9;
  mob_cdma2000_hrpd_r9_e_ mob_cdma2000_hrpd_r9;
  dyn_octstring           msg_cont_cdma2000_hrpd_r9;
  carrier_freq_cdma2000_s redirect_carrier_cdma2000_hrpd_r9;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Handover ::= SEQUENCE
struct ho_s {
  struct target_rat_type_opts {
    enum options {
      utra,
      geran,
      cdma2000_minus1_xrtt,
      cdma2000_hrpd,
      nr,
      eutra,
      spare2,
      spare1,
      /*...*/ nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<target_rat_type_opts, true> target_rat_type_e_;

  // member variables
  bool               nas_security_param_from_eutra_present = false;
  bool               sys_info_present                      = false;
  target_rat_type_e_ target_rat_type;
  dyn_octstring      target_rat_msg_container;
  fixed_octstring<1> nas_security_param_from_eutra;
  si_or_psi_geran_c  sys_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverFromEUTRAPreparationRequest-v890-IEs ::= SEQUENCE
struct ho_from_eutra_prep_request_v890_ies_s {
  bool                                  late_non_crit_ext_present = false;
  bool                                  non_crit_ext_present      = false;
  dyn_octstring                         late_non_crit_ext;
  ho_from_eutra_prep_request_v920_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMeasurementConfiguration-v1080-IEs ::= SEQUENCE
struct logged_meas_cfg_v1080_ies_s {
  bool                        late_non_crit_ext_r10_present = false;
  bool                        non_crit_ext_present          = false;
  dyn_octstring               late_non_crit_ext_r10;
  logged_meas_cfg_v1130_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggingDuration-r10 ::= ENUMERATED
struct logging_dur_r10_opts {
  enum options { min10, min20, min40, min60, min90, min120, spare2, spare1, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<logging_dur_r10_opts> logging_dur_r10_e;

// LoggingInterval-r10 ::= ENUMERATED
struct logging_interv_r10_opts {
  enum options { ms1280, ms2560, ms5120, ms10240, ms20480, ms30720, ms40960, ms61440, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<logging_interv_r10_opts> logging_interv_r10_e;

// MobilityFromEUTRACommand-v8a0-IEs ::= SEQUENCE
struct mob_from_eutra_cmd_v8a0_ies_s {
  bool                          late_non_crit_ext_present = false;
  bool                          non_crit_ext_present      = false;
  dyn_octstring                 late_non_crit_ext;
  mob_from_eutra_cmd_v8d0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromEUTRACommand-v930-IEs ::= SEQUENCE
struct mob_from_eutra_cmd_v930_ies_s {
  bool                          late_non_crit_ext_present = false;
  bool                          non_crit_ext_present      = false;
  dyn_octstring                 late_non_crit_ext;
  mob_from_eutra_cmd_v960_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RN-SubframeConfig-r10 ::= SEQUENCE
struct rn_sf_cfg_r10_s {
  struct sf_cfg_pattern_r10_c_ {
    struct types_opts {
      enum options { sf_cfg_pattern_fdd_r10, sf_cfg_pattern_tdd_r10, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sf_cfg_pattern_r10_c_() = default;
    sf_cfg_pattern_r10_c_(const sf_cfg_pattern_r10_c_& other);
    sf_cfg_pattern_r10_c_& operator=(const sf_cfg_pattern_r10_c_& other);
    ~sf_cfg_pattern_r10_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<8>& sf_cfg_pattern_fdd_r10()
    {
      assert_choice_type(types::sf_cfg_pattern_fdd_r10, type_, "subframeConfigPattern-r10");
      return c.get<fixed_bitstring<8> >();
    }
    uint8_t& sf_cfg_pattern_tdd_r10()
    {
      assert_choice_type(types::sf_cfg_pattern_tdd_r10, type_, "subframeConfigPattern-r10");
      return c.get<uint8_t>();
    }
    const fixed_bitstring<8>& sf_cfg_pattern_fdd_r10() const
    {
      assert_choice_type(types::sf_cfg_pattern_fdd_r10, type_, "subframeConfigPattern-r10");
      return c.get<fixed_bitstring<8> >();
    }
    const uint8_t& sf_cfg_pattern_tdd_r10() const
    {
      assert_choice_type(types::sf_cfg_pattern_tdd_r10, type_, "subframeConfigPattern-r10");
      return c.get<uint8_t>();
    }
    fixed_bitstring<8>& set_sf_cfg_pattern_fdd_r10();
    uint8_t&            set_sf_cfg_pattern_tdd_r10();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<8> > c;

    void destroy_();
  };
  struct rpdcch_cfg_r10_s_ {
    struct res_alloc_type_r10_opts {
      enum options { type0, type1, type2_localized, type2_distributed, spare4, spare3, spare2, spare1, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<res_alloc_type_r10_opts> res_alloc_type_r10_e_;
    struct res_block_assign_r10_c_ {
      struct type01_r10_c_ {
        struct types_opts {
          enum options { nrb6_r10, nrb15_r10, nrb25_r10, nrb50_r10, nrb75_r10, nrb100_r10, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        type01_r10_c_() = default;
        type01_r10_c_(const type01_r10_c_& other);
        type01_r10_c_& operator=(const type01_r10_c_& other);
        ~type01_r10_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        fixed_bitstring<6>& nrb6_r10()
        {
          assert_choice_type(types::nrb6_r10, type_, "type01-r10");
          return c.get<fixed_bitstring<6> >();
        }
        fixed_bitstring<8>& nrb15_r10()
        {
          assert_choice_type(types::nrb15_r10, type_, "type01-r10");
          return c.get<fixed_bitstring<8> >();
        }
        fixed_bitstring<13>& nrb25_r10()
        {
          assert_choice_type(types::nrb25_r10, type_, "type01-r10");
          return c.get<fixed_bitstring<13> >();
        }
        fixed_bitstring<17>& nrb50_r10()
        {
          assert_choice_type(types::nrb50_r10, type_, "type01-r10");
          return c.get<fixed_bitstring<17> >();
        }
        fixed_bitstring<19>& nrb75_r10()
        {
          assert_choice_type(types::nrb75_r10, type_, "type01-r10");
          return c.get<fixed_bitstring<19> >();
        }
        fixed_bitstring<25>& nrb100_r10()
        {
          assert_choice_type(types::nrb100_r10, type_, "type01-r10");
          return c.get<fixed_bitstring<25> >();
        }
        const fixed_bitstring<6>& nrb6_r10() const
        {
          assert_choice_type(types::nrb6_r10, type_, "type01-r10");
          return c.get<fixed_bitstring<6> >();
        }
        const fixed_bitstring<8>& nrb15_r10() const
        {
          assert_choice_type(types::nrb15_r10, type_, "type01-r10");
          return c.get<fixed_bitstring<8> >();
        }
        const fixed_bitstring<13>& nrb25_r10() const
        {
          assert_choice_type(types::nrb25_r10, type_, "type01-r10");
          return c.get<fixed_bitstring<13> >();
        }
        const fixed_bitstring<17>& nrb50_r10() const
        {
          assert_choice_type(types::nrb50_r10, type_, "type01-r10");
          return c.get<fixed_bitstring<17> >();
        }
        const fixed_bitstring<19>& nrb75_r10() const
        {
          assert_choice_type(types::nrb75_r10, type_, "type01-r10");
          return c.get<fixed_bitstring<19> >();
        }
        const fixed_bitstring<25>& nrb100_r10() const
        {
          assert_choice_type(types::nrb100_r10, type_, "type01-r10");
          return c.get<fixed_bitstring<25> >();
        }
        fixed_bitstring<6>&  set_nrb6_r10();
        fixed_bitstring<8>&  set_nrb15_r10();
        fixed_bitstring<13>& set_nrb25_r10();
        fixed_bitstring<17>& set_nrb50_r10();
        fixed_bitstring<19>& set_nrb75_r10();
        fixed_bitstring<25>& set_nrb100_r10();

      private:
        types                                 type_;
        choice_buffer_t<fixed_bitstring<25> > c;

        void destroy_();
      };
      struct type2_r10_c_ {
        struct types_opts {
          enum options { nrb6_r10, nrb15_r10, nrb25_r10, nrb50_r10, nrb75_r10, nrb100_r10, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        type2_r10_c_() = default;
        type2_r10_c_(const type2_r10_c_& other);
        type2_r10_c_& operator=(const type2_r10_c_& other);
        ~type2_r10_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        fixed_bitstring<5>& nrb6_r10()
        {
          assert_choice_type(types::nrb6_r10, type_, "type2-r10");
          return c.get<fixed_bitstring<5> >();
        }
        fixed_bitstring<7>& nrb15_r10()
        {
          assert_choice_type(types::nrb15_r10, type_, "type2-r10");
          return c.get<fixed_bitstring<7> >();
        }
        fixed_bitstring<9>& nrb25_r10()
        {
          assert_choice_type(types::nrb25_r10, type_, "type2-r10");
          return c.get<fixed_bitstring<9> >();
        }
        fixed_bitstring<11>& nrb50_r10()
        {
          assert_choice_type(types::nrb50_r10, type_, "type2-r10");
          return c.get<fixed_bitstring<11> >();
        }
        fixed_bitstring<12>& nrb75_r10()
        {
          assert_choice_type(types::nrb75_r10, type_, "type2-r10");
          return c.get<fixed_bitstring<12> >();
        }
        fixed_bitstring<13>& nrb100_r10()
        {
          assert_choice_type(types::nrb100_r10, type_, "type2-r10");
          return c.get<fixed_bitstring<13> >();
        }
        const fixed_bitstring<5>& nrb6_r10() const
        {
          assert_choice_type(types::nrb6_r10, type_, "type2-r10");
          return c.get<fixed_bitstring<5> >();
        }
        const fixed_bitstring<7>& nrb15_r10() const
        {
          assert_choice_type(types::nrb15_r10, type_, "type2-r10");
          return c.get<fixed_bitstring<7> >();
        }
        const fixed_bitstring<9>& nrb25_r10() const
        {
          assert_choice_type(types::nrb25_r10, type_, "type2-r10");
          return c.get<fixed_bitstring<9> >();
        }
        const fixed_bitstring<11>& nrb50_r10() const
        {
          assert_choice_type(types::nrb50_r10, type_, "type2-r10");
          return c.get<fixed_bitstring<11> >();
        }
        const fixed_bitstring<12>& nrb75_r10() const
        {
          assert_choice_type(types::nrb75_r10, type_, "type2-r10");
          return c.get<fixed_bitstring<12> >();
        }
        const fixed_bitstring<13>& nrb100_r10() const
        {
          assert_choice_type(types::nrb100_r10, type_, "type2-r10");
          return c.get<fixed_bitstring<13> >();
        }
        fixed_bitstring<5>&  set_nrb6_r10();
        fixed_bitstring<7>&  set_nrb15_r10();
        fixed_bitstring<9>&  set_nrb25_r10();
        fixed_bitstring<11>& set_nrb50_r10();
        fixed_bitstring<12>& set_nrb75_r10();
        fixed_bitstring<13>& set_nrb100_r10();

      private:
        types                                 type_;
        choice_buffer_t<fixed_bitstring<13> > c;

        void destroy_();
      };
      struct types_opts {
        enum options { type01_r10, type2_r10, /*...*/ nulltype } value;
        typedef float number_type;

        const char* to_string() const;
        float       to_number() const;
        const char* to_number_string() const;
      };
      typedef enumerated<types_opts, true> types;

      // choice methods
      res_block_assign_r10_c_() = default;
      res_block_assign_r10_c_(const res_block_assign_r10_c_& other);
      res_block_assign_r10_c_& operator=(const res_block_assign_r10_c_& other);
      ~res_block_assign_r10_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      type01_r10_c_& type01_r10()
      {
        assert_choice_type(types::type01_r10, type_, "resourceBlockAssignment-r10");
        return c.get<type01_r10_c_>();
      }
      type2_r10_c_& type2_r10()
      {
        assert_choice_type(types::type2_r10, type_, "resourceBlockAssignment-r10");
        return c.get<type2_r10_c_>();
      }
      const type01_r10_c_& type01_r10() const
      {
        assert_choice_type(types::type01_r10, type_, "resourceBlockAssignment-r10");
        return c.get<type01_r10_c_>();
      }
      const type2_r10_c_& type2_r10() const
      {
        assert_choice_type(types::type2_r10, type_, "resourceBlockAssignment-r10");
        return c.get<type2_r10_c_>();
      }
      type01_r10_c_& set_type01_r10();
      type2_r10_c_&  set_type2_r10();

    private:
      types                                        type_;
      choice_buffer_t<type01_r10_c_, type2_r10_c_> c;

      void destroy_();
    };
    struct demod_rs_r10_c_ {
      struct no_interleaving_r10_opts {
        enum options { crs, dmrs, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<no_interleaving_r10_opts> no_interleaving_r10_e_;
      struct types_opts {
        enum options { interleaving_r10, no_interleaving_r10, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      demod_rs_r10_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      no_interleaving_r10_e_& no_interleaving_r10()
      {
        assert_choice_type(types::no_interleaving_r10, type_, "demodulationRS-r10");
        return c;
      }
      const no_interleaving_r10_e_& no_interleaving_r10() const
      {
        assert_choice_type(types::no_interleaving_r10, type_, "demodulationRS-r10");
        return c;
      }
      void                    set_interleaving_r10();
      no_interleaving_r10_e_& set_no_interleaving_r10();

    private:
      types                  type_;
      no_interleaving_r10_e_ c;
    };
    struct pucch_cfg_r10_c_ {
      struct tdd_c_ {
        struct ch_sel_mux_bundling_s_ {
          using n1_pucch_an_list_r10_l_ = bounded_array<uint16_t, 4>;

          // member variables
          n1_pucch_an_list_r10_l_ n1_pucch_an_list_r10;
        };
        struct fallback_for_format3_s_ {
          bool     n1_pucch_an_p1_r10_present = false;
          uint16_t n1_pucch_an_p0_r10         = 0;
          uint16_t n1_pucch_an_p1_r10         = 0;
        };
        struct types_opts {
          enum options { ch_sel_mux_bundling, fallback_for_format3, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        tdd_c_() = default;
        tdd_c_(const tdd_c_& other);
        tdd_c_& operator=(const tdd_c_& other);
        ~tdd_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        ch_sel_mux_bundling_s_& ch_sel_mux_bundling()
        {
          assert_choice_type(types::ch_sel_mux_bundling, type_, "tdd");
          return c.get<ch_sel_mux_bundling_s_>();
        }
        fallback_for_format3_s_& fallback_for_format3()
        {
          assert_choice_type(types::fallback_for_format3, type_, "tdd");
          return c.get<fallback_for_format3_s_>();
        }
        const ch_sel_mux_bundling_s_& ch_sel_mux_bundling() const
        {
          assert_choice_type(types::ch_sel_mux_bundling, type_, "tdd");
          return c.get<ch_sel_mux_bundling_s_>();
        }
        const fallback_for_format3_s_& fallback_for_format3() const
        {
          assert_choice_type(types::fallback_for_format3, type_, "tdd");
          return c.get<fallback_for_format3_s_>();
        }
        ch_sel_mux_bundling_s_&  set_ch_sel_mux_bundling();
        fallback_for_format3_s_& set_fallback_for_format3();

      private:
        types                                                            type_;
        choice_buffer_t<ch_sel_mux_bundling_s_, fallback_for_format3_s_> c;

        void destroy_();
      };
      struct fdd_s_ {
        bool     n1_pucch_an_p1_r10_present = false;
        uint16_t n1_pucch_an_p0_r10         = 0;
        uint16_t n1_pucch_an_p1_r10         = 0;
      };
      struct types_opts {
        enum options { tdd, fdd, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      pucch_cfg_r10_c_() = default;
      pucch_cfg_r10_c_(const pucch_cfg_r10_c_& other);
      pucch_cfg_r10_c_& operator=(const pucch_cfg_r10_c_& other);
      ~pucch_cfg_r10_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      tdd_c_& tdd()
      {
        assert_choice_type(types::tdd, type_, "pucch-Config-r10");
        return c.get<tdd_c_>();
      }
      fdd_s_& fdd()
      {
        assert_choice_type(types::fdd, type_, "pucch-Config-r10");
        return c.get<fdd_s_>();
      }
      const tdd_c_& tdd() const
      {
        assert_choice_type(types::tdd, type_, "pucch-Config-r10");
        return c.get<tdd_c_>();
      }
      const fdd_s_& fdd() const
      {
        assert_choice_type(types::fdd, type_, "pucch-Config-r10");
        return c.get<fdd_s_>();
      }
      tdd_c_& set_tdd();
      fdd_s_& set_fdd();

    private:
      types                           type_;
      choice_buffer_t<fdd_s_, tdd_c_> c;

      void destroy_();
    };

    // member variables
    bool                    ext = false;
    res_alloc_type_r10_e_   res_alloc_type_r10;
    res_block_assign_r10_c_ res_block_assign_r10;
    demod_rs_r10_c_         demod_rs_r10;
    uint8_t                 pdsch_start_r10 = 1;
    pucch_cfg_r10_c_        pucch_cfg_r10;
    // ...
  };

  // member variables
  bool                  ext                        = false;
  bool                  sf_cfg_pattern_r10_present = false;
  bool                  rpdcch_cfg_r10_present     = false;
  sf_cfg_pattern_r10_c_ sf_cfg_pattern_r10;
  rpdcch_cfg_r10_s_     rpdcch_cfg_r10;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RN-SystemInfo-r10 ::= SEQUENCE
struct rn_sys_info_r10_s {
  bool          ext                   = false;
  bool          sib_type1_r10_present = false;
  bool          sib_type2_r10_present = false;
  dyn_octstring sib_type1_r10;
  sib_type2_s   sib_type2_r10;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-v890-IEs ::= SEQUENCE
struct rrc_conn_recfg_v890_ies_s {
  bool                      late_non_crit_ext_present = false;
  bool                      non_crit_ext_present      = false;
  dyn_octstring             late_non_crit_ext;
  rrc_conn_recfg_v920_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-v890-IEs ::= SEQUENCE
struct rrc_conn_release_v890_ies_s {
  bool                        late_non_crit_ext_present = false;
  bool                        non_crit_ext_present      = false;
  dyn_octstring               late_non_crit_ext;
  rrc_conn_release_v920_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResume-v1430-IEs ::= SEQUENCE
struct rrc_conn_resume_v1430_ies_s {
  bool                        other_cfg_r14_present             = false;
  bool                        rrc_conn_resume_v1510_ies_present = false;
  other_cfg_r9_s              other_cfg_r14;
  rrc_conn_resume_v1510_ies_s rrc_conn_resume_v1510_ies;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedirectedCarrierInfo ::= CHOICE
struct redirected_carrier_info_c {
  struct types_opts {
    enum options {
      eutra,
      geran,
      utra_fdd,
      utra_tdd,
      cdma2000_hrpd,
      cdma2000_minus1x_rtt,
      // ...
      utra_tdd_r10,
      nr_r15,
      nr_r17,
      nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true, 3> types;

  // choice methods
  redirected_carrier_info_c() = default;
  redirected_carrier_info_c(const redirected_carrier_info_c& other);
  redirected_carrier_info_c& operator=(const redirected_carrier_info_c& other);
  ~redirected_carrier_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint32_t& eutra()
  {
    assert_choice_type(types::eutra, type_, "RedirectedCarrierInfo");
    return c.get<uint32_t>();
  }
  carrier_freqs_geran_s& geran()
  {
    assert_choice_type(types::geran, type_, "RedirectedCarrierInfo");
    return c.get<carrier_freqs_geran_s>();
  }
  uint16_t& utra_fdd()
  {
    assert_choice_type(types::utra_fdd, type_, "RedirectedCarrierInfo");
    return c.get<uint16_t>();
  }
  uint16_t& utra_tdd()
  {
    assert_choice_type(types::utra_tdd, type_, "RedirectedCarrierInfo");
    return c.get<uint16_t>();
  }
  carrier_freq_cdma2000_s& cdma2000_hrpd()
  {
    assert_choice_type(types::cdma2000_hrpd, type_, "RedirectedCarrierInfo");
    return c.get<carrier_freq_cdma2000_s>();
  }
  carrier_freq_cdma2000_s& cdma2000_minus1x_rtt()
  {
    assert_choice_type(types::cdma2000_minus1x_rtt, type_, "RedirectedCarrierInfo");
    return c.get<carrier_freq_cdma2000_s>();
  }
  carrier_freq_list_utra_tdd_r10_l& utra_tdd_r10()
  {
    assert_choice_type(types::utra_tdd_r10, type_, "RedirectedCarrierInfo");
    return c.get<carrier_freq_list_utra_tdd_r10_l>();
  }
  carrier_info_nr_r15_s& nr_r15()
  {
    assert_choice_type(types::nr_r15, type_, "RedirectedCarrierInfo");
    return c.get<carrier_info_nr_r15_s>();
  }
  carrier_info_nr_r17_s& nr_r17()
  {
    assert_choice_type(types::nr_r17, type_, "RedirectedCarrierInfo");
    return c.get<carrier_info_nr_r17_s>();
  }
  const uint32_t& eutra() const
  {
    assert_choice_type(types::eutra, type_, "RedirectedCarrierInfo");
    return c.get<uint32_t>();
  }
  const carrier_freqs_geran_s& geran() const
  {
    assert_choice_type(types::geran, type_, "RedirectedCarrierInfo");
    return c.get<carrier_freqs_geran_s>();
  }
  const uint16_t& utra_fdd() const
  {
    assert_choice_type(types::utra_fdd, type_, "RedirectedCarrierInfo");
    return c.get<uint16_t>();
  }
  const uint16_t& utra_tdd() const
  {
    assert_choice_type(types::utra_tdd, type_, "RedirectedCarrierInfo");
    return c.get<uint16_t>();
  }
  const carrier_freq_cdma2000_s& cdma2000_hrpd() const
  {
    assert_choice_type(types::cdma2000_hrpd, type_, "RedirectedCarrierInfo");
    return c.get<carrier_freq_cdma2000_s>();
  }
  const carrier_freq_cdma2000_s& cdma2000_minus1x_rtt() const
  {
    assert_choice_type(types::cdma2000_minus1x_rtt, type_, "RedirectedCarrierInfo");
    return c.get<carrier_freq_cdma2000_s>();
  }
  const carrier_freq_list_utra_tdd_r10_l& utra_tdd_r10() const
  {
    assert_choice_type(types::utra_tdd_r10, type_, "RedirectedCarrierInfo");
    return c.get<carrier_freq_list_utra_tdd_r10_l>();
  }
  const carrier_info_nr_r15_s& nr_r15() const
  {
    assert_choice_type(types::nr_r15, type_, "RedirectedCarrierInfo");
    return c.get<carrier_info_nr_r15_s>();
  }
  const carrier_info_nr_r17_s& nr_r17() const
  {
    assert_choice_type(types::nr_r17, type_, "RedirectedCarrierInfo");
    return c.get<carrier_info_nr_r17_s>();
  }
  uint32_t&                         set_eutra();
  carrier_freqs_geran_s&            set_geran();
  uint16_t&                         set_utra_fdd();
  uint16_t&                         set_utra_tdd();
  carrier_freq_cdma2000_s&          set_cdma2000_hrpd();
  carrier_freq_cdma2000_s&          set_cdma2000_minus1x_rtt();
  carrier_freq_list_utra_tdd_r10_l& set_utra_tdd_r10();
  carrier_info_nr_r15_s&            set_nr_r15();
  carrier_info_nr_r17_s&            set_nr_r17();

private:
  types type_;
  choice_buffer_t<carrier_freq_cdma2000_s,
                  carrier_freq_list_utra_tdd_r10_l,
                  carrier_freqs_geran_s,
                  carrier_info_nr_r15_s,
                  carrier_info_nr_r17_s>
      c;

  void destroy_();
};

// ReleaseCause ::= ENUMERATED
struct release_cause_opts {
  enum options { load_balancing_ta_urequired, other, cs_fallback_high_prio_v1020, rrc_suspend_v1320, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<release_cause_opts> release_cause_e;

// UEInformationRequest-v930-IEs ::= SEQUENCE
struct ue_info_request_v930_ies_s {
  bool                        late_non_crit_ext_present = false;
  bool                        non_crit_ext_present      = false;
  dyn_octstring               late_non_crit_ext;
  ue_info_request_v1020_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSFBParametersResponseCDMA2000-r8-IEs ::= SEQUENCE
struct csfb_params_resp_cdma2000_r8_ies_s {
  bool                                 non_crit_ext_present = false;
  fixed_bitstring<32>                  rand;
  dyn_octstring                        mob_params;
  csfb_params_resp_cdma2000_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheck-r8-IEs ::= SEQUENCE
struct counter_check_r8_ies_s {
  bool                      non_crit_ext_present = false;
  drb_count_msb_info_list_l drb_count_msb_info_list;
  counter_check_v8a0_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLDedicatedMessageSegment-r16-IEs ::= SEQUENCE
struct dl_ded_msg_segment_r16_ies_s {
  struct rrc_msg_segment_type_r16_opts {
    enum options { not_last_segment, last_segment, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<rrc_msg_segment_type_r16_opts> rrc_msg_segment_type_r16_e_;

  // member variables
  bool                        late_non_crit_ext_present = false;
  bool                        non_crit_ext_present      = false;
  uint8_t                     segment_num_r16           = 0;
  dyn_octstring               rrc_msg_segment_container_r16;
  rrc_msg_segment_type_r16_e_ rrc_msg_segment_type_r16;
  dyn_octstring               late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer-r15-IEs ::= SEQUENCE
struct dl_info_transfer_r15_ies_s {
  struct ded_info_type_r15_c_ {
    struct types_opts {
      enum options { ded_info_nas, ded_info_cdma2000_minus1_xrtt, ded_info_cdma2000_hrpd, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ded_info_type_r15_c_() = default;
    ded_info_type_r15_c_(const ded_info_type_r15_c_& other);
    ded_info_type_r15_c_& operator=(const ded_info_type_r15_c_& other);
    ~ded_info_type_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_octstring& ded_info_nas()
    {
      assert_choice_type(types::ded_info_nas, type_, "dedicatedInfoType-r15");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ded_info_cdma2000_minus1_xrtt()
    {
      assert_choice_type(types::ded_info_cdma2000_minus1_xrtt, type_, "dedicatedInfoType-r15");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ded_info_cdma2000_hrpd()
    {
      assert_choice_type(types::ded_info_cdma2000_hrpd, type_, "dedicatedInfoType-r15");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ded_info_nas() const
    {
      assert_choice_type(types::ded_info_nas, type_, "dedicatedInfoType-r15");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ded_info_cdma2000_minus1_xrtt() const
    {
      assert_choice_type(types::ded_info_cdma2000_minus1_xrtt, type_, "dedicatedInfoType-r15");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ded_info_cdma2000_hrpd() const
    {
      assert_choice_type(types::ded_info_cdma2000_hrpd, type_, "dedicatedInfoType-r15");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& set_ded_info_nas();
    dyn_octstring& set_ded_info_cdma2000_minus1_xrtt();
    dyn_octstring& set_ded_info_cdma2000_hrpd();

  private:
    types                          type_;
    choice_buffer_t<dyn_octstring> c;

    void destroy_();
  };

  // member variables
  bool                        ded_info_type_r15_present = false;
  bool                        time_ref_info_r15_present = false;
  bool                        non_crit_ext_present      = false;
  ded_info_type_r15_c_        ded_info_type_r15;
  time_ref_info_r15_s         time_ref_info_r15;
  dl_info_transfer_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer-r8-IEs ::= SEQUENCE
struct dl_info_transfer_r8_ies_s {
  struct ded_info_type_c_ {
    struct types_opts {
      enum options { ded_info_nas, ded_info_cdma2000_minus1_xrtt, ded_info_cdma2000_hrpd, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ded_info_type_c_() = default;
    ded_info_type_c_(const ded_info_type_c_& other);
    ded_info_type_c_& operator=(const ded_info_type_c_& other);
    ~ded_info_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_octstring& ded_info_nas()
    {
      assert_choice_type(types::ded_info_nas, type_, "dedicatedInfoType");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ded_info_cdma2000_minus1_xrtt()
    {
      assert_choice_type(types::ded_info_cdma2000_minus1_xrtt, type_, "dedicatedInfoType");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ded_info_cdma2000_hrpd()
    {
      assert_choice_type(types::ded_info_cdma2000_hrpd, type_, "dedicatedInfoType");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ded_info_nas() const
    {
      assert_choice_type(types::ded_info_nas, type_, "dedicatedInfoType");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ded_info_cdma2000_minus1_xrtt() const
    {
      assert_choice_type(types::ded_info_cdma2000_minus1_xrtt, type_, "dedicatedInfoType");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ded_info_cdma2000_hrpd() const
    {
      assert_choice_type(types::ded_info_cdma2000_hrpd, type_, "dedicatedInfoType");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& set_ded_info_nas();
    dyn_octstring& set_ded_info_cdma2000_minus1_xrtt();
    dyn_octstring& set_ded_info_cdma2000_hrpd();

  private:
    types                          type_;
    choice_buffer_t<dyn_octstring> c;

    void destroy_();
  };

  // member variables
  bool                        non_crit_ext_present = false;
  ded_info_type_c_            ded_info_type;
  dl_info_transfer_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverFromEUTRAPreparationRequest-r8-IEs ::= SEQUENCE
struct ho_from_eutra_prep_request_r8_ies_s {
  bool                                  rand_present         = false;
  bool                                  mob_params_present   = false;
  bool                                  non_crit_ext_present = false;
  cdma2000_type_e                       cdma2000_type;
  fixed_bitstring<32>                   rand;
  dyn_octstring                         mob_params;
  ho_from_eutra_prep_request_v890_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMeasurementConfiguration-r10-IEs ::= SEQUENCE
struct logged_meas_cfg_r10_ies_s {
  bool                        area_cfg_r10_present = false;
  bool                        non_crit_ext_present = false;
  trace_ref_r10_s             trace_ref_r10;
  fixed_octstring<2>          trace_recording_session_ref_r10;
  fixed_octstring<1>          tce_id_r10;
  fixed_bitstring<48>         absolute_time_info_r10;
  area_cfg_r10_c              area_cfg_r10;
  logging_dur_r10_e           logging_dur_r10;
  logging_interv_r10_e        logging_interv_r10;
  logged_meas_cfg_v1080_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromEUTRACommand-r8-IEs ::= SEQUENCE
struct mob_from_eutra_cmd_r8_ies_s {
  struct purpose_c_ {
    struct types_opts {
      enum options { ho, cell_change_order, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    purpose_c_() = default;
    purpose_c_(const purpose_c_& other);
    purpose_c_& operator=(const purpose_c_& other);
    ~purpose_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ho_s& ho()
    {
      assert_choice_type(types::ho, type_, "purpose");
      return c.get<ho_s>();
    }
    cell_change_order_s& cell_change_order()
    {
      assert_choice_type(types::cell_change_order, type_, "purpose");
      return c.get<cell_change_order_s>();
    }
    const ho_s& ho() const
    {
      assert_choice_type(types::ho, type_, "purpose");
      return c.get<ho_s>();
    }
    const cell_change_order_s& cell_change_order() const
    {
      assert_choice_type(types::cell_change_order, type_, "purpose");
      return c.get<cell_change_order_s>();
    }
    ho_s&                set_ho();
    cell_change_order_s& set_cell_change_order();

  private:
    types                                      type_;
    choice_buffer_t<cell_change_order_s, ho_s> c;

    void destroy_();
  };

  // member variables
  bool                          non_crit_ext_present = false;
  bool                          cs_fallback_ind      = false;
  purpose_c_                    purpose;
  mob_from_eutra_cmd_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromEUTRACommand-r9-IEs ::= SEQUENCE
struct mob_from_eutra_cmd_r9_ies_s {
  struct purpose_c_ {
    struct types_opts {
      enum options { ho, cell_change_order, e_csfb_r9, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    purpose_c_() = default;
    purpose_c_(const purpose_c_& other);
    purpose_c_& operator=(const purpose_c_& other);
    ~purpose_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ho_s& ho()
    {
      assert_choice_type(types::ho, type_, "purpose");
      return c.get<ho_s>();
    }
    cell_change_order_s& cell_change_order()
    {
      assert_choice_type(types::cell_change_order, type_, "purpose");
      return c.get<cell_change_order_s>();
    }
    e_csfb_r9_s& e_csfb_r9()
    {
      assert_choice_type(types::e_csfb_r9, type_, "purpose");
      return c.get<e_csfb_r9_s>();
    }
    const ho_s& ho() const
    {
      assert_choice_type(types::ho, type_, "purpose");
      return c.get<ho_s>();
    }
    const cell_change_order_s& cell_change_order() const
    {
      assert_choice_type(types::cell_change_order, type_, "purpose");
      return c.get<cell_change_order_s>();
    }
    const e_csfb_r9_s& e_csfb_r9() const
    {
      assert_choice_type(types::e_csfb_r9, type_, "purpose");
      return c.get<e_csfb_r9_s>();
    }
    ho_s&                set_ho();
    cell_change_order_s& set_cell_change_order();
    e_csfb_r9_s&         set_e_csfb_r9();

  private:
    types                                                   type_;
    choice_buffer_t<cell_change_order_s, e_csfb_r9_s, ho_s> c;

    void destroy_();
  };

  // member variables
  bool                          non_crit_ext_present = false;
  bool                          cs_fallback_ind      = false;
  purpose_c_                    purpose;
  mob_from_eutra_cmd_v930_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RNReconfiguration-r10-IEs ::= SEQUENCE
struct rn_recfg_r10_ies_s {
  bool              rn_sys_info_r10_present   = false;
  bool              rn_sf_cfg_r10_present     = false;
  bool              late_non_crit_ext_present = false;
  bool              non_crit_ext_present      = false;
  rn_sys_info_r10_s rn_sys_info_r10;
  rn_sf_cfg_r10_s   rn_sf_cfg_r10;
  dyn_octstring     late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-r8-IEs ::= SEQUENCE
struct rrc_conn_recfg_r8_ies_s {
  using ded_info_nas_list_l_ = bounded_array<dyn_octstring, 11>;

  // member variables
  bool                      meas_cfg_present          = false;
  bool                      mob_ctrl_info_present     = false;
  bool                      ded_info_nas_list_present = false;
  bool                      rr_cfg_ded_present        = false;
  bool                      security_cfg_ho_present   = false;
  bool                      non_crit_ext_present      = false;
  meas_cfg_s                meas_cfg;
  mob_ctrl_info_s           mob_ctrl_info;
  ded_info_nas_list_l_      ded_info_nas_list;
  rr_cfg_ded_s              rr_cfg_ded;
  security_cfg_ho_s         security_cfg_ho;
  rrc_conn_recfg_v890_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-r8-IEs ::= SEQUENCE
struct rrc_conn_release_r8_ies_s {
  bool                        redirected_carrier_info_present = false;
  bool                        idle_mode_mob_ctrl_info_present = false;
  bool                        non_crit_ext_present            = false;
  release_cause_e             release_cause;
  redirected_carrier_info_c   redirected_carrier_info;
  idle_mode_mob_ctrl_info_s   idle_mode_mob_ctrl_info;
  rrc_conn_release_v890_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResume-r13-IEs ::= SEQUENCE
struct rrc_conn_resume_r13_ies_s {
  bool                        rr_cfg_ded_r13_present            = false;
  bool                        meas_cfg_r13_present              = false;
  bool                        ant_info_ded_pcell_r13_present    = false;
  bool                        drb_continue_rohc_r13_present     = false;
  bool                        late_non_crit_ext_present         = false;
  bool                        rrc_conn_resume_v1430_ies_present = false;
  rr_cfg_ded_s                rr_cfg_ded_r13;
  uint8_t                     next_hop_chaining_count_r13 = 0;
  meas_cfg_s                  meas_cfg_r13;
  ant_info_ded_v10i0_s        ant_info_ded_pcell_r13;
  dyn_octstring               late_non_crit_ext;
  rrc_conn_resume_v1430_ies_s rrc_conn_resume_v1430_ies;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationRequest-r9-IEs ::= SEQUENCE
struct ue_info_request_r9_ies_s {
  bool                       non_crit_ext_present = false;
  bool                       rach_report_req_r9   = false;
  bool                       rlf_report_req_r9    = false;
  ue_info_request_v930_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSFBParametersResponseCDMA2000 ::= SEQUENCE
struct csfb_params_resp_cdma2000_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { csfb_params_resp_cdma2000_r8, crit_exts_future, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    csfb_params_resp_cdma2000_r8_ies_s& csfb_params_resp_cdma2000_r8()
    {
      assert_choice_type(types::csfb_params_resp_cdma2000_r8, type_, "criticalExtensions");
      return c;
    }
    const csfb_params_resp_cdma2000_r8_ies_s& csfb_params_resp_cdma2000_r8() const
    {
      assert_choice_type(types::csfb_params_resp_cdma2000_r8, type_, "criticalExtensions");
      return c;
    }
    csfb_params_resp_cdma2000_r8_ies_s& set_csfb_params_resp_cdma2000_r8();
    void                                set_crit_exts_future();

  private:
    types                              type_;
    csfb_params_resp_cdma2000_r8_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheck ::= SEQUENCE
struct counter_check_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { counter_check_r8, spare3, spare2, spare1, nulltype } value;

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
      counter_check_r8_ies_s& counter_check_r8()
      {
        assert_choice_type(types::counter_check_r8, type_, "c1");
        return c;
      }
      const counter_check_r8_ies_s& counter_check_r8() const
      {
        assert_choice_type(types::counter_check_r8, type_, "c1");
        return c;
      }
      counter_check_r8_ies_s& set_counter_check_r8();
      void                    set_spare3();
      void                    set_spare2();
      void                    set_spare1();

    private:
      types                  type_;
      counter_check_r8_ies_s c;
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
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLDedicatedMessageSegment-r16 ::= SEQUENCE
struct dl_ded_msg_segment_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { dl_ded_msg_segment_r16, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dl_ded_msg_segment_r16_ies_s& dl_ded_msg_segment_r16()
    {
      assert_choice_type(types::dl_ded_msg_segment_r16, type_, "criticalExtensions");
      return c;
    }
    const dl_ded_msg_segment_r16_ies_s& dl_ded_msg_segment_r16() const
    {
      assert_choice_type(types::dl_ded_msg_segment_r16, type_, "criticalExtensions");
      return c;
    }
    dl_ded_msg_segment_r16_ies_s& set_dl_ded_msg_segment_r16();
    void                          set_crit_exts_future();

  private:
    types                        type_;
    dl_ded_msg_segment_r16_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer ::= SEQUENCE
struct dl_info_transfer_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { dl_info_transfer_r8, dl_info_transfer_r15, spare2, spare1, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      c1_c_(const c1_c_& other);
      c1_c_& operator=(const c1_c_& other);
      ~c1_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      dl_info_transfer_r8_ies_s& dl_info_transfer_r8()
      {
        assert_choice_type(types::dl_info_transfer_r8, type_, "c1");
        return c.get<dl_info_transfer_r8_ies_s>();
      }
      dl_info_transfer_r15_ies_s& dl_info_transfer_r15()
      {
        assert_choice_type(types::dl_info_transfer_r15, type_, "c1");
        return c.get<dl_info_transfer_r15_ies_s>();
      }
      const dl_info_transfer_r8_ies_s& dl_info_transfer_r8() const
      {
        assert_choice_type(types::dl_info_transfer_r8, type_, "c1");
        return c.get<dl_info_transfer_r8_ies_s>();
      }
      const dl_info_transfer_r15_ies_s& dl_info_transfer_r15() const
      {
        assert_choice_type(types::dl_info_transfer_r15, type_, "c1");
        return c.get<dl_info_transfer_r15_ies_s>();
      }
      dl_info_transfer_r8_ies_s&  set_dl_info_transfer_r8();
      dl_info_transfer_r15_ies_s& set_dl_info_transfer_r15();
      void                        set_spare2();
      void                        set_spare1();

    private:
      types                                                                  type_;
      choice_buffer_t<dl_info_transfer_r15_ies_s, dl_info_transfer_r8_ies_s> c;

      void destroy_();
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
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverFromEUTRAPreparationRequest ::= SEQUENCE
struct ho_from_eutra_prep_request_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ho_from_eutra_prep_request_r8, spare3, spare2, spare1, nulltype } value;

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
      ho_from_eutra_prep_request_r8_ies_s& ho_from_eutra_prep_request_r8()
      {
        assert_choice_type(types::ho_from_eutra_prep_request_r8, type_, "c1");
        return c;
      }
      const ho_from_eutra_prep_request_r8_ies_s& ho_from_eutra_prep_request_r8() const
      {
        assert_choice_type(types::ho_from_eutra_prep_request_r8, type_, "c1");
        return c;
      }
      ho_from_eutra_prep_request_r8_ies_s& set_ho_from_eutra_prep_request_r8();
      void                                 set_spare3();
      void                                 set_spare2();
      void                                 set_spare1();

    private:
      types                               type_;
      ho_from_eutra_prep_request_r8_ies_s c;
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
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMeasurementConfiguration-r10 ::= SEQUENCE
struct logged_meas_cfg_r10_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { logged_meas_cfg_r10, spare3, spare2, spare1, nulltype } value;

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
      logged_meas_cfg_r10_ies_s& logged_meas_cfg_r10()
      {
        assert_choice_type(types::logged_meas_cfg_r10, type_, "c1");
        return c;
      }
      const logged_meas_cfg_r10_ies_s& logged_meas_cfg_r10() const
      {
        assert_choice_type(types::logged_meas_cfg_r10, type_, "c1");
        return c;
      }
      logged_meas_cfg_r10_ies_s& set_logged_meas_cfg_r10();
      void                       set_spare3();
      void                       set_spare2();
      void                       set_spare1();

    private:
      types                     type_;
      logged_meas_cfg_r10_ies_s c;
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

// MobilityFromEUTRACommand ::= SEQUENCE
struct mob_from_eutra_cmd_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { mob_from_eutra_cmd_r8, mob_from_eutra_cmd_r9, spare2, spare1, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      c1_c_(const c1_c_& other);
      c1_c_& operator=(const c1_c_& other);
      ~c1_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      mob_from_eutra_cmd_r8_ies_s& mob_from_eutra_cmd_r8()
      {
        assert_choice_type(types::mob_from_eutra_cmd_r8, type_, "c1");
        return c.get<mob_from_eutra_cmd_r8_ies_s>();
      }
      mob_from_eutra_cmd_r9_ies_s& mob_from_eutra_cmd_r9()
      {
        assert_choice_type(types::mob_from_eutra_cmd_r9, type_, "c1");
        return c.get<mob_from_eutra_cmd_r9_ies_s>();
      }
      const mob_from_eutra_cmd_r8_ies_s& mob_from_eutra_cmd_r8() const
      {
        assert_choice_type(types::mob_from_eutra_cmd_r8, type_, "c1");
        return c.get<mob_from_eutra_cmd_r8_ies_s>();
      }
      const mob_from_eutra_cmd_r9_ies_s& mob_from_eutra_cmd_r9() const
      {
        assert_choice_type(types::mob_from_eutra_cmd_r9, type_, "c1");
        return c.get<mob_from_eutra_cmd_r9_ies_s>();
      }
      mob_from_eutra_cmd_r8_ies_s& set_mob_from_eutra_cmd_r8();
      mob_from_eutra_cmd_r9_ies_s& set_mob_from_eutra_cmd_r9();
      void                         set_spare2();
      void                         set_spare1();

    private:
      types                                                                     type_;
      choice_buffer_t<mob_from_eutra_cmd_r8_ies_s, mob_from_eutra_cmd_r9_ies_s> c;

      void destroy_();
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
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RNReconfiguration-r10 ::= SEQUENCE
struct rn_recfg_r10_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rn_recfg_r10, spare3, spare2, spare1, nulltype } value;

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
      rn_recfg_r10_ies_s& rn_recfg_r10()
      {
        assert_choice_type(types::rn_recfg_r10, type_, "c1");
        return c;
      }
      const rn_recfg_r10_ies_s& rn_recfg_r10() const
      {
        assert_choice_type(types::rn_recfg_r10, type_, "c1");
        return c;
      }
      rn_recfg_r10_ies_s& set_rn_recfg_r10();
      void                set_spare3();
      void                set_spare2();
      void                set_spare1();

    private:
      types              type_;
      rn_recfg_r10_ies_s c;
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
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration ::= SEQUENCE
struct rrc_conn_recfg_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_recfg_r8, spare7, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

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
      rrc_conn_recfg_r8_ies_s& rrc_conn_recfg_r8()
      {
        assert_choice_type(types::rrc_conn_recfg_r8, type_, "c1");
        return c;
      }
      const rrc_conn_recfg_r8_ies_s& rrc_conn_recfg_r8() const
      {
        assert_choice_type(types::rrc_conn_recfg_r8, type_, "c1");
        return c;
      }
      rrc_conn_recfg_r8_ies_s& set_rrc_conn_recfg_r8();
      void                     set_spare7();
      void                     set_spare6();
      void                     set_spare5();
      void                     set_spare4();
      void                     set_spare3();
      void                     set_spare2();
      void                     set_spare1();

    private:
      types                   type_;
      rrc_conn_recfg_r8_ies_s c;
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
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease ::= SEQUENCE
struct rrc_conn_release_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_release_r8, spare3, spare2, spare1, nulltype } value;

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
      rrc_conn_release_r8_ies_s& rrc_conn_release_r8()
      {
        assert_choice_type(types::rrc_conn_release_r8, type_, "c1");
        return c;
      }
      const rrc_conn_release_r8_ies_s& rrc_conn_release_r8() const
      {
        assert_choice_type(types::rrc_conn_release_r8, type_, "c1");
        return c;
      }
      rrc_conn_release_r8_ies_s& set_rrc_conn_release_r8();
      void                       set_spare3();
      void                       set_spare2();
      void                       set_spare1();

    private:
      types                     type_;
      rrc_conn_release_r8_ies_s c;
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
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResume-r13 ::= SEQUENCE
struct rrc_conn_resume_r13_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_resume_r13, spare3, spare2, spare1, nulltype } value;

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
      rrc_conn_resume_r13_ies_s& rrc_conn_resume_r13()
      {
        assert_choice_type(types::rrc_conn_resume_r13, type_, "c1");
        return c;
      }
      const rrc_conn_resume_r13_ies_s& rrc_conn_resume_r13() const
      {
        assert_choice_type(types::rrc_conn_resume_r13, type_, "c1");
        return c;
      }
      rrc_conn_resume_r13_ies_s& set_rrc_conn_resume_r13();
      void                       set_spare3();
      void                       set_spare2();
      void                       set_spare1();

    private:
      types                     type_;
      rrc_conn_resume_r13_ies_s c;
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
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationRequest-r9 ::= SEQUENCE
struct ue_info_request_r9_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ue_info_request_r9, spare3, spare2, spare1, nulltype } value;

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
      ue_info_request_r9_ies_s& ue_info_request_r9()
      {
        assert_choice_type(types::ue_info_request_r9, type_, "c1");
        return c;
      }
      const ue_info_request_r9_ies_s& ue_info_request_r9() const
      {
        assert_choice_type(types::ue_info_request_r9, type_, "c1");
        return c;
      }
      ue_info_request_r9_ies_s& set_ue_info_request_r9();
      void                      set_spare3();
      void                      set_spare2();
      void                      set_spare1();

    private:
      types                    type_;
      ue_info_request_r9_ies_s c;
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
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-DCCH-MessageType ::= CHOICE
struct dl_dcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        csfb_params_resp_cdma2000,
        dl_info_transfer,
        ho_from_eutra_prep_request,
        mob_from_eutra_cmd,
        rrc_conn_recfg,
        rrc_conn_release,
        security_mode_cmd,
        ue_cap_enquiry,
        counter_check,
        ue_info_request_r9,
        logged_meas_cfg_r10,
        rn_recfg_r10,
        rrc_conn_resume_r13,
        dl_ded_msg_segment_r16,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    c1_c_() = default;
    c1_c_(const c1_c_& other);
    c1_c_& operator=(const c1_c_& other);
    ~c1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    csfb_params_resp_cdma2000_s& csfb_params_resp_cdma2000()
    {
      assert_choice_type(types::csfb_params_resp_cdma2000, type_, "c1");
      return c.get<csfb_params_resp_cdma2000_s>();
    }
    dl_info_transfer_s& dl_info_transfer()
    {
      assert_choice_type(types::dl_info_transfer, type_, "c1");
      return c.get<dl_info_transfer_s>();
    }
    ho_from_eutra_prep_request_s& ho_from_eutra_prep_request()
    {
      assert_choice_type(types::ho_from_eutra_prep_request, type_, "c1");
      return c.get<ho_from_eutra_prep_request_s>();
    }
    mob_from_eutra_cmd_s& mob_from_eutra_cmd()
    {
      assert_choice_type(types::mob_from_eutra_cmd, type_, "c1");
      return c.get<mob_from_eutra_cmd_s>();
    }
    rrc_conn_recfg_s& rrc_conn_recfg()
    {
      assert_choice_type(types::rrc_conn_recfg, type_, "c1");
      return c.get<rrc_conn_recfg_s>();
    }
    rrc_conn_release_s& rrc_conn_release()
    {
      assert_choice_type(types::rrc_conn_release, type_, "c1");
      return c.get<rrc_conn_release_s>();
    }
    security_mode_cmd_s& security_mode_cmd()
    {
      assert_choice_type(types::security_mode_cmd, type_, "c1");
      return c.get<security_mode_cmd_s>();
    }
    ue_cap_enquiry_s& ue_cap_enquiry()
    {
      assert_choice_type(types::ue_cap_enquiry, type_, "c1");
      return c.get<ue_cap_enquiry_s>();
    }
    counter_check_s& counter_check()
    {
      assert_choice_type(types::counter_check, type_, "c1");
      return c.get<counter_check_s>();
    }
    ue_info_request_r9_s& ue_info_request_r9()
    {
      assert_choice_type(types::ue_info_request_r9, type_, "c1");
      return c.get<ue_info_request_r9_s>();
    }
    logged_meas_cfg_r10_s& logged_meas_cfg_r10()
    {
      assert_choice_type(types::logged_meas_cfg_r10, type_, "c1");
      return c.get<logged_meas_cfg_r10_s>();
    }
    rn_recfg_r10_s& rn_recfg_r10()
    {
      assert_choice_type(types::rn_recfg_r10, type_, "c1");
      return c.get<rn_recfg_r10_s>();
    }
    rrc_conn_resume_r13_s& rrc_conn_resume_r13()
    {
      assert_choice_type(types::rrc_conn_resume_r13, type_, "c1");
      return c.get<rrc_conn_resume_r13_s>();
    }
    dl_ded_msg_segment_r16_s& dl_ded_msg_segment_r16()
    {
      assert_choice_type(types::dl_ded_msg_segment_r16, type_, "c1");
      return c.get<dl_ded_msg_segment_r16_s>();
    }
    const csfb_params_resp_cdma2000_s& csfb_params_resp_cdma2000() const
    {
      assert_choice_type(types::csfb_params_resp_cdma2000, type_, "c1");
      return c.get<csfb_params_resp_cdma2000_s>();
    }
    const dl_info_transfer_s& dl_info_transfer() const
    {
      assert_choice_type(types::dl_info_transfer, type_, "c1");
      return c.get<dl_info_transfer_s>();
    }
    const ho_from_eutra_prep_request_s& ho_from_eutra_prep_request() const
    {
      assert_choice_type(types::ho_from_eutra_prep_request, type_, "c1");
      return c.get<ho_from_eutra_prep_request_s>();
    }
    const mob_from_eutra_cmd_s& mob_from_eutra_cmd() const
    {
      assert_choice_type(types::mob_from_eutra_cmd, type_, "c1");
      return c.get<mob_from_eutra_cmd_s>();
    }
    const rrc_conn_recfg_s& rrc_conn_recfg() const
    {
      assert_choice_type(types::rrc_conn_recfg, type_, "c1");
      return c.get<rrc_conn_recfg_s>();
    }
    const rrc_conn_release_s& rrc_conn_release() const
    {
      assert_choice_type(types::rrc_conn_release, type_, "c1");
      return c.get<rrc_conn_release_s>();
    }
    const security_mode_cmd_s& security_mode_cmd() const
    {
      assert_choice_type(types::security_mode_cmd, type_, "c1");
      return c.get<security_mode_cmd_s>();
    }
    const ue_cap_enquiry_s& ue_cap_enquiry() const
    {
      assert_choice_type(types::ue_cap_enquiry, type_, "c1");
      return c.get<ue_cap_enquiry_s>();
    }
    const counter_check_s& counter_check() const
    {
      assert_choice_type(types::counter_check, type_, "c1");
      return c.get<counter_check_s>();
    }
    const ue_info_request_r9_s& ue_info_request_r9() const
    {
      assert_choice_type(types::ue_info_request_r9, type_, "c1");
      return c.get<ue_info_request_r9_s>();
    }
    const logged_meas_cfg_r10_s& logged_meas_cfg_r10() const
    {
      assert_choice_type(types::logged_meas_cfg_r10, type_, "c1");
      return c.get<logged_meas_cfg_r10_s>();
    }
    const rn_recfg_r10_s& rn_recfg_r10() const
    {
      assert_choice_type(types::rn_recfg_r10, type_, "c1");
      return c.get<rn_recfg_r10_s>();
    }
    const rrc_conn_resume_r13_s& rrc_conn_resume_r13() const
    {
      assert_choice_type(types::rrc_conn_resume_r13, type_, "c1");
      return c.get<rrc_conn_resume_r13_s>();
    }
    const dl_ded_msg_segment_r16_s& dl_ded_msg_segment_r16() const
    {
      assert_choice_type(types::dl_ded_msg_segment_r16, type_, "c1");
      return c.get<dl_ded_msg_segment_r16_s>();
    }
    csfb_params_resp_cdma2000_s&  set_csfb_params_resp_cdma2000();
    dl_info_transfer_s&           set_dl_info_transfer();
    ho_from_eutra_prep_request_s& set_ho_from_eutra_prep_request();
    mob_from_eutra_cmd_s&         set_mob_from_eutra_cmd();
    rrc_conn_recfg_s&             set_rrc_conn_recfg();
    rrc_conn_release_s&           set_rrc_conn_release();
    security_mode_cmd_s&          set_security_mode_cmd();
    ue_cap_enquiry_s&             set_ue_cap_enquiry();
    counter_check_s&              set_counter_check();
    ue_info_request_r9_s&         set_ue_info_request_r9();
    logged_meas_cfg_r10_s&        set_logged_meas_cfg_r10();
    rn_recfg_r10_s&               set_rn_recfg_r10();
    rrc_conn_resume_r13_s&        set_rrc_conn_resume_r13();
    dl_ded_msg_segment_r16_s&     set_dl_ded_msg_segment_r16();
    void                          set_spare2();
    void                          set_spare1();

  private:
    types type_;
    choice_buffer_t<counter_check_s,
                    csfb_params_resp_cdma2000_s,
                    dl_ded_msg_segment_r16_s,
                    dl_info_transfer_s,
                    ho_from_eutra_prep_request_s,
                    logged_meas_cfg_r10_s,
                    mob_from_eutra_cmd_s,
                    rn_recfg_r10_s,
                    rrc_conn_recfg_s,
                    rrc_conn_release_s,
                    rrc_conn_resume_r13_s,
                    security_mode_cmd_s,
                    ue_cap_enquiry_s,
                    ue_info_request_r9_s>
        c;

    void destroy_();
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  dl_dcch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "DL-DCCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "DL-DCCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// DL-DCCH-Message ::= SEQUENCE
struct dl_dcch_msg_s {
  dl_dcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigCommonPSCell-v12f0 ::= SEQUENCE
struct rr_cfg_common_ps_cell_v12f0_s {
  rr_cfg_common_scell_v10l0_s basic_fields_v12f0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PSCellToAddMod-v12f0 ::= SEQUENCE
struct ps_cell_to_add_mod_v12f0_s {
  bool                          rr_cfg_common_ps_cell_r12_present = false;
  rr_cfg_common_ps_cell_v12f0_s rr_cfg_common_ps_cell_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-ConfigPartSCG-v13c0 ::= SEQUENCE
struct scg_cfg_part_scg_v13c0_s {
  bool                              scell_to_add_mod_list_scg_v13c0_present     = false;
  bool                              scell_to_add_mod_list_scg_ext_v13c0_present = false;
  scell_to_add_mod_list_v13c0_l     scell_to_add_mod_list_scg_v13c0;
  scell_to_add_mod_list_ext_v13c0_l scell_to_add_mod_list_scg_ext_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-Configuration-v13c0 ::= CHOICE
struct scg_cfg_v13c0_c {
  struct setup_s_ {
    bool                     scg_cfg_part_scg_v13c0_present = false;
    scg_cfg_part_scg_v13c0_s scg_cfg_part_scg_v13c0;
  };
  using types = setup_e;

  // choice methods
  scg_cfg_v13c0_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SCG-Configuration-v13c0");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SCG-Configuration-v13c0");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// RRCConnectionReconfiguration-v13c0-IEs ::= SEQUENCE
struct rrc_conn_recfg_v13c0_ies_s {
  bool                              rr_cfg_ded_v13c0_present                = false;
  bool                              scell_to_add_mod_list_v13c0_present     = false;
  bool                              scell_to_add_mod_list_ext_v13c0_present = false;
  bool                              scg_cfg_v13c0_present                   = false;
  bool                              non_crit_ext_present                    = false;
  rr_cfg_ded_v13c0_s                rr_cfg_ded_v13c0;
  scell_to_add_mod_list_v13c0_l     scell_to_add_mod_list_v13c0;
  scell_to_add_mod_list_ext_v13c0_l scell_to_add_mod_list_ext_v13c0;
  scg_cfg_v13c0_c                   scg_cfg_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigDedicated-v1370 ::= SEQUENCE
struct rr_cfg_ded_v1370_s {
  bool                 phys_cfg_ded_v1370_present = false;
  phys_cfg_ded_v1370_s phys_cfg_ded_v1370;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-ConfigPartSCG-v12f0 ::= SEQUENCE
struct scg_cfg_part_scg_v12f0_s {
  bool                          pscell_to_add_mod_v12f0_present         = false;
  bool                          scell_to_add_mod_list_scg_v12f0_present = false;
  ps_cell_to_add_mod_v12f0_s    pscell_to_add_mod_v12f0;
  scell_to_add_mod_list_v10l0_l scell_to_add_mod_list_scg_v12f0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-v1370-IEs ::= SEQUENCE
struct rrc_conn_recfg_v1370_ies_s {
  bool                              rr_cfg_ded_v1370_present                = false;
  bool                              scell_to_add_mod_list_ext_v1370_present = false;
  bool                              non_crit_ext_present                    = false;
  rr_cfg_ded_v1370_s                rr_cfg_ded_v1370;
  scell_to_add_mod_list_ext_v1370_l scell_to_add_mod_list_ext_v1370;
  rrc_conn_recfg_v13c0_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-Configuration-v12f0 ::= CHOICE
struct scg_cfg_v12f0_c {
  struct setup_s_ {
    bool                     scg_cfg_part_scg_v12f0_present = false;
    scg_cfg_part_scg_v12f0_s scg_cfg_part_scg_v12f0;
  };
  using types = setup_e;

  // choice methods
  scg_cfg_v12f0_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SCG-Configuration-v12f0");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SCG-Configuration-v12f0");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// RRCConnectionReconfiguration-v12f0-IEs ::= SEQUENCE
struct rrc_conn_recfg_v12f0_ies_s {
  bool                       scg_cfg_v12f0_present     = false;
  bool                       late_non_crit_ext_present = false;
  bool                       non_crit_ext_present      = false;
  scg_cfg_v12f0_c            scg_cfg_v12f0;
  dyn_octstring              late_non_crit_ext;
  rrc_conn_recfg_v1370_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-v10l0-IEs ::= SEQUENCE
struct rrc_conn_recfg_v10l0_ies_s {
  bool                          mob_ctrl_info_v10l0_present         = false;
  bool                          scell_to_add_mod_list_v10l0_present = false;
  bool                          late_non_crit_ext_present           = false;
  bool                          non_crit_ext_present                = false;
  mob_ctrl_info_v10l0_s         mob_ctrl_info_v10l0;
  scell_to_add_mod_list_v10l0_l scell_to_add_mod_list_v10l0;
  dyn_octstring                 late_non_crit_ext;
  rrc_conn_recfg_v12f0_ies_s    non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-v10i0-IEs ::= SEQUENCE
struct rrc_conn_recfg_v10i0_ies_s {
  bool                       ant_info_ded_pcell_v10i0_present = false;
  bool                       non_crit_ext_present             = false;
  ant_info_ded_v10i0_s       ant_info_ded_pcell_v10i0;
  rrc_conn_recfg_v10l0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-v8m0-IEs ::= SEQUENCE
struct rrc_conn_recfg_v8m0_ies_s {
  bool                       late_non_crit_ext_present = false;
  bool                       non_crit_ext_present      = false;
  dyn_octstring              late_non_crit_ext;
  rrc_conn_recfg_v10i0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedirectedCarrierInfo-v9e0 ::= SEQUENCE
struct redirected_carrier_info_v9e0_s {
  uint32_t eutra_v9e0 = 65536;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-v9e0-IEs ::= SEQUENCE
struct rrc_conn_release_v9e0_ies_s {
  bool                           redirected_carrier_info_v9e0_present = false;
  bool                           idle_mode_mob_ctrl_info_v9e0_present = false;
  bool                           non_crit_ext_present                 = false;
  redirected_carrier_info_v9e0_s redirected_carrier_info_v9e0;
  idle_mode_mob_ctrl_info_v9e0_s idle_mode_mob_ctrl_info_v9e0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_DLDCCH_MSG_H
