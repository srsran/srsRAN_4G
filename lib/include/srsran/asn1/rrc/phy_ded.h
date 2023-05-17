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

#ifndef SRSASN1_RRC_PHYCFG_H
#define SRSASN1_RRC_PHYCFG_H

#include "rr_common.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// PeriodicityStartPos-r16 ::= CHOICE
struct periodicity_start_pos_r16_c {
  struct types_opts {
    enum options {
      periodicity10ms,
      periodicity20ms,
      periodicity40ms,
      periodicity80ms,
      periodicity160ms,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  periodicity_start_pos_r16_c() = default;
  periodicity_start_pos_r16_c(const periodicity_start_pos_r16_c& other);
  periodicity_start_pos_r16_c& operator=(const periodicity_start_pos_r16_c& other);
  ~periodicity_start_pos_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& periodicity20ms()
  {
    assert_choice_type(types::periodicity20ms, type_, "PeriodicityStartPos-r16");
    return c.get<uint8_t>();
  }
  uint8_t& periodicity40ms()
  {
    assert_choice_type(types::periodicity40ms, type_, "PeriodicityStartPos-r16");
    return c.get<uint8_t>();
  }
  uint8_t& periodicity80ms()
  {
    assert_choice_type(types::periodicity80ms, type_, "PeriodicityStartPos-r16");
    return c.get<uint8_t>();
  }
  uint8_t& periodicity160ms()
  {
    assert_choice_type(types::periodicity160ms, type_, "PeriodicityStartPos-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& periodicity20ms() const
  {
    assert_choice_type(types::periodicity20ms, type_, "PeriodicityStartPos-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& periodicity40ms() const
  {
    assert_choice_type(types::periodicity40ms, type_, "PeriodicityStartPos-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& periodicity80ms() const
  {
    assert_choice_type(types::periodicity80ms, type_, "PeriodicityStartPos-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& periodicity160ms() const
  {
    assert_choice_type(types::periodicity160ms, type_, "PeriodicityStartPos-r16");
    return c.get<uint8_t>();
  }
  void     set_periodicity10ms();
  uint8_t& set_periodicity20ms();
  uint8_t& set_periodicity40ms();
  uint8_t& set_periodicity80ms();
  uint8_t& set_periodicity160ms();
  void     set_spare3();
  void     set_spare2();
  void     set_spare1();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// ResourceReservationConfigDL-r16 ::= SEQUENCE
struct res_reserv_cfg_dl_r16_s {
  struct res_reserv_freq_r16_c_ {
    struct types_opts {
      enum options {
        rbg_bitmap1dot4,
        rbg_bitmap3,
        rbg_bitmap5,
        rbg_bitmap10,
        rbg_bitmap15,
        rbg_bitmap20,
        nulltype
      } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    res_reserv_freq_r16_c_() = default;
    res_reserv_freq_r16_c_(const res_reserv_freq_r16_c_& other);
    res_reserv_freq_r16_c_& operator=(const res_reserv_freq_r16_c_& other);
    ~res_reserv_freq_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<6>& rbg_bitmap1dot4()
    {
      assert_choice_type(types::rbg_bitmap1dot4, type_, "resourceReservationFreq-r16");
      return c.get<fixed_bitstring<6> >();
    }
    fixed_bitstring<8>& rbg_bitmap3()
    {
      assert_choice_type(types::rbg_bitmap3, type_, "resourceReservationFreq-r16");
      return c.get<fixed_bitstring<8> >();
    }
    fixed_bitstring<13>& rbg_bitmap5()
    {
      assert_choice_type(types::rbg_bitmap5, type_, "resourceReservationFreq-r16");
      return c.get<fixed_bitstring<13> >();
    }
    fixed_bitstring<17>& rbg_bitmap10()
    {
      assert_choice_type(types::rbg_bitmap10, type_, "resourceReservationFreq-r16");
      return c.get<fixed_bitstring<17> >();
    }
    fixed_bitstring<19>& rbg_bitmap15()
    {
      assert_choice_type(types::rbg_bitmap15, type_, "resourceReservationFreq-r16");
      return c.get<fixed_bitstring<19> >();
    }
    fixed_bitstring<25>& rbg_bitmap20()
    {
      assert_choice_type(types::rbg_bitmap20, type_, "resourceReservationFreq-r16");
      return c.get<fixed_bitstring<25> >();
    }
    const fixed_bitstring<6>& rbg_bitmap1dot4() const
    {
      assert_choice_type(types::rbg_bitmap1dot4, type_, "resourceReservationFreq-r16");
      return c.get<fixed_bitstring<6> >();
    }
    const fixed_bitstring<8>& rbg_bitmap3() const
    {
      assert_choice_type(types::rbg_bitmap3, type_, "resourceReservationFreq-r16");
      return c.get<fixed_bitstring<8> >();
    }
    const fixed_bitstring<13>& rbg_bitmap5() const
    {
      assert_choice_type(types::rbg_bitmap5, type_, "resourceReservationFreq-r16");
      return c.get<fixed_bitstring<13> >();
    }
    const fixed_bitstring<17>& rbg_bitmap10() const
    {
      assert_choice_type(types::rbg_bitmap10, type_, "resourceReservationFreq-r16");
      return c.get<fixed_bitstring<17> >();
    }
    const fixed_bitstring<19>& rbg_bitmap15() const
    {
      assert_choice_type(types::rbg_bitmap15, type_, "resourceReservationFreq-r16");
      return c.get<fixed_bitstring<19> >();
    }
    const fixed_bitstring<25>& rbg_bitmap20() const
    {
      assert_choice_type(types::rbg_bitmap20, type_, "resourceReservationFreq-r16");
      return c.get<fixed_bitstring<25> >();
    }
    fixed_bitstring<6>&  set_rbg_bitmap1dot4();
    fixed_bitstring<8>&  set_rbg_bitmap3();
    fixed_bitstring<13>& set_rbg_bitmap5();
    fixed_bitstring<17>& set_rbg_bitmap10();
    fixed_bitstring<19>& set_rbg_bitmap15();
    fixed_bitstring<25>& set_rbg_bitmap20();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<25> > c;

    void destroy_();
  };
  struct slot_bitmap_r16_c_ {
    struct types_opts {
      enum options { slot_pattern10ms, slot_pattern40ms, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    slot_bitmap_r16_c_() = default;
    slot_bitmap_r16_c_(const slot_bitmap_r16_c_& other);
    slot_bitmap_r16_c_& operator=(const slot_bitmap_r16_c_& other);
    ~slot_bitmap_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<20>& slot_pattern10ms()
    {
      assert_choice_type(types::slot_pattern10ms, type_, "slotBitmap-r16");
      return c.get<fixed_bitstring<20> >();
    }
    fixed_bitstring<80>& slot_pattern40ms()
    {
      assert_choice_type(types::slot_pattern40ms, type_, "slotBitmap-r16");
      return c.get<fixed_bitstring<80> >();
    }
    const fixed_bitstring<20>& slot_pattern10ms() const
    {
      assert_choice_type(types::slot_pattern10ms, type_, "slotBitmap-r16");
      return c.get<fixed_bitstring<20> >();
    }
    const fixed_bitstring<80>& slot_pattern40ms() const
    {
      assert_choice_type(types::slot_pattern40ms, type_, "slotBitmap-r16");
      return c.get<fixed_bitstring<80> >();
    }
    fixed_bitstring<20>& set_slot_pattern10ms();
    fixed_bitstring<80>& set_slot_pattern40ms();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<80> > c;

    void destroy_();
  };

  // member variables
  bool                        ext                         = false;
  bool                        res_reserv_freq_r16_present = false;
  bool                        symbol_bitmap1_r16_present  = false;
  bool                        symbol_bitmap2_r16_present  = false;
  periodicity_start_pos_r16_c periodicity_start_pos_r16;
  res_reserv_freq_r16_c_      res_reserv_freq_r16;
  slot_bitmap_r16_c_          slot_bitmap_r16;
  fixed_bitstring<7>          symbol_bitmap1_r16;
  fixed_bitstring<7>          symbol_bitmap2_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceReservationConfigUL-r16 ::= SEQUENCE
struct res_reserv_cfg_ul_r16_s {
  struct slot_bitmap_r16_c_ {
    struct types_opts {
      enum options { slot_pattern10ms, slot_pattern40ms, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    slot_bitmap_r16_c_() = default;
    slot_bitmap_r16_c_(const slot_bitmap_r16_c_& other);
    slot_bitmap_r16_c_& operator=(const slot_bitmap_r16_c_& other);
    ~slot_bitmap_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<20>& slot_pattern10ms()
    {
      assert_choice_type(types::slot_pattern10ms, type_, "slotBitmap-r16");
      return c.get<fixed_bitstring<20> >();
    }
    fixed_bitstring<80>& slot_pattern40ms()
    {
      assert_choice_type(types::slot_pattern40ms, type_, "slotBitmap-r16");
      return c.get<fixed_bitstring<80> >();
    }
    const fixed_bitstring<20>& slot_pattern10ms() const
    {
      assert_choice_type(types::slot_pattern10ms, type_, "slotBitmap-r16");
      return c.get<fixed_bitstring<20> >();
    }
    const fixed_bitstring<80>& slot_pattern40ms() const
    {
      assert_choice_type(types::slot_pattern40ms, type_, "slotBitmap-r16");
      return c.get<fixed_bitstring<80> >();
    }
    fixed_bitstring<20>& set_slot_pattern10ms();
    fixed_bitstring<80>& set_slot_pattern40ms();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<80> > c;

    void destroy_();
  };

  // member variables
  bool                        ext                        = false;
  bool                        slot_bitmap_r16_present    = false;
  bool                        symbol_bitmap1_r16_present = false;
  bool                        symbol_bitmap2_r16_present = false;
  periodicity_start_pos_r16_c periodicity_start_pos_r16;
  slot_bitmap_r16_c_          slot_bitmap_r16;
  fixed_bitstring<7>          symbol_bitmap1_r16;
  fixed_bitstring<7>          symbol_bitmap2_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NZP-FrequencyDensity-r14 ::= ENUMERATED
struct nzp_freq_density_r14_opts {
  enum options { d1, d2, d3, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<nzp_freq_density_r14_opts> nzp_freq_density_r14_e;

// P-C-AndCBSR-r13 ::= SEQUENCE
struct p_c_and_cbsr_r13_s {
  struct cbsr_sel_r13_c_ {
    struct non_precoded_r13_s_ {
      dyn_bitstring codebook_subset_restrict1_r13;
      dyn_bitstring codebook_subset_restrict2_r13;
    };
    struct bf_k1a_r13_s_ {
      dyn_bitstring codebook_subset_restrict3_r13;
    };
    struct bf_kn_r13_s_ {
      dyn_bitstring codebook_subset_restrict_r13;
    };
    struct types_opts {
      enum options { non_precoded_r13, bf_k1a_r13, bf_kn_r13, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    cbsr_sel_r13_c_() = default;
    cbsr_sel_r13_c_(const cbsr_sel_r13_c_& other);
    cbsr_sel_r13_c_& operator=(const cbsr_sel_r13_c_& other);
    ~cbsr_sel_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const cbsr_sel_r13_c_& other) const;
    bool        operator!=(const cbsr_sel_r13_c_& other) const { return not(*this == other); }
    // getters
    non_precoded_r13_s_& non_precoded_r13()
    {
      assert_choice_type(types::non_precoded_r13, type_, "cbsr-Selection-r13");
      return c.get<non_precoded_r13_s_>();
    }
    bf_k1a_r13_s_& bf_k1a_r13()
    {
      assert_choice_type(types::bf_k1a_r13, type_, "cbsr-Selection-r13");
      return c.get<bf_k1a_r13_s_>();
    }
    bf_kn_r13_s_& bf_kn_r13()
    {
      assert_choice_type(types::bf_kn_r13, type_, "cbsr-Selection-r13");
      return c.get<bf_kn_r13_s_>();
    }
    const non_precoded_r13_s_& non_precoded_r13() const
    {
      assert_choice_type(types::non_precoded_r13, type_, "cbsr-Selection-r13");
      return c.get<non_precoded_r13_s_>();
    }
    const bf_k1a_r13_s_& bf_k1a_r13() const
    {
      assert_choice_type(types::bf_k1a_r13, type_, "cbsr-Selection-r13");
      return c.get<bf_k1a_r13_s_>();
    }
    const bf_kn_r13_s_& bf_kn_r13() const
    {
      assert_choice_type(types::bf_kn_r13, type_, "cbsr-Selection-r13");
      return c.get<bf_kn_r13_s_>();
    }
    non_precoded_r13_s_& set_non_precoded_r13();
    bf_k1a_r13_s_&       set_bf_k1a_r13();
    bf_kn_r13_s_&        set_bf_kn_r13();

  private:
    types                                                             type_;
    choice_buffer_t<bf_k1a_r13_s_, bf_kn_r13_s_, non_precoded_r13_s_> c;

    void destroy_();
  };

  // member variables
  bool            ext     = false;
  int8_t          p_c_r13 = -8;
  cbsr_sel_r13_c_ cbsr_sel_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const p_c_and_cbsr_r13_s& other) const;
  bool        operator!=(const p_c_and_cbsr_r13_s& other) const { return not(*this == other); }
};

// CSI-RS-Config-NZP-v1430 ::= SEQUENCE
struct csi_rs_cfg_nzp_v1430_s {
  bool                   tx_comb_r14_present      = false;
  bool                   freq_density_r14_present = false;
  uint8_t                tx_comb_r14              = 0;
  nzp_freq_density_r14_e freq_density_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_nzp_v1430_s& other) const;
  bool        operator!=(const csi_rs_cfg_nzp_v1430_s& other) const { return not(*this == other); }
};

// CSI-RS-ConfigNZP-Activation-r14 ::= SEQUENCE
struct csi_rs_cfg_nzp_activation_r14_s {
  struct csi_rs_nzp_mode_r14_opts {
    enum options { semi_persistent, aperiodic, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<csi_rs_nzp_mode_r14_opts> csi_rs_nzp_mode_r14_e_;

  // member variables
  csi_rs_nzp_mode_r14_e_ csi_rs_nzp_mode_r14;
  uint8_t                activ_res_r14 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_nzp_activation_r14_s& other) const;
  bool        operator!=(const csi_rs_cfg_nzp_activation_r14_s& other) const { return not(*this == other); }
};

// CSI-RS-ConfigNZP-r11 ::= SEQUENCE
struct csi_rs_cfg_nzp_r11_s {
  struct ant_ports_count_r11_opts {
    enum options { an1, an2, an4, an8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ant_ports_count_r11_opts> ant_ports_count_r11_e_;
  struct qcl_crs_info_r11_s_ {
    struct crs_ports_count_r11_opts {
      enum options { n1, n2, n4, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<crs_ports_count_r11_opts> crs_ports_count_r11_e_;
    struct mbsfn_sf_cfg_list_r11_c_ {
      struct setup_s_ {
        mbsfn_sf_cfg_list_l sf_cfg_list;
      };
      using types = setup_e;

      // choice methods
      mbsfn_sf_cfg_list_r11_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const mbsfn_sf_cfg_list_r11_c_& other) const;
      bool        operator!=(const mbsfn_sf_cfg_list_r11_c_& other) const { return not(*this == other); }
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "mbsfn-SubframeConfigList-r11");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "mbsfn-SubframeConfigList-r11");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };

    // member variables
    bool                     mbsfn_sf_cfg_list_r11_present = false;
    uint16_t                 qcl_scrambling_id_r11         = 0;
    crs_ports_count_r11_e_   crs_ports_count_r11;
    mbsfn_sf_cfg_list_r11_c_ mbsfn_sf_cfg_list_r11;
  };
  struct mbsfn_sf_cfg_list_v1430_c_ {
    struct setup_s_ {
      mbsfn_sf_cfg_list_v1430_l sf_cfg_list_v1430;
    };
    using types = setup_e;

    // choice methods
    mbsfn_sf_cfg_list_v1430_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const mbsfn_sf_cfg_list_v1430_c_& other) const;
    bool        operator!=(const mbsfn_sf_cfg_list_v1430_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "mbsfn-SubframeConfigList-v1430");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "mbsfn-SubframeConfigList-v1430");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                   ext                      = false;
  bool                   qcl_crs_info_r11_present = false;
  uint8_t                csi_rs_cfg_nzp_id_r11    = 1;
  ant_ports_count_r11_e_ ant_ports_count_r11;
  uint8_t                res_cfg_r11       = 0;
  uint8_t                sf_cfg_r11        = 0;
  uint16_t               scrambling_id_r11 = 0;
  qcl_crs_info_r11_s_    qcl_crs_info_r11;
  // ...
  // group 0
  bool    csi_rs_cfg_nzp_id_v1310_present = false;
  uint8_t csi_rs_cfg_nzp_id_v1310         = 4;
  // group 1
  bool                   tx_comb_r14_present      = false;
  bool                   freq_density_r14_present = false;
  uint8_t                tx_comb_r14              = 0;
  nzp_freq_density_r14_e freq_density_r14;
  // group 2
  copy_ptr<mbsfn_sf_cfg_list_v1430_c_> mbsfn_sf_cfg_list_v1430;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_nzp_r11_s& other) const;
  bool        operator!=(const csi_rs_cfg_nzp_r11_s& other) const { return not(*this == other); }
};

// NZP-ResourceConfig-r13 ::= SEQUENCE
struct nzp_res_cfg_r13_s {
  bool    ext         = false;
  uint8_t res_cfg_r13 = 0;
  // ...
  // group 0
  bool                   tx_comb_r14_present      = false;
  bool                   freq_density_r14_present = false;
  uint8_t                tx_comb_r14              = 0;
  nzp_freq_density_r14_e freq_density_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const nzp_res_cfg_r13_s& other) const;
  bool        operator!=(const nzp_res_cfg_r13_s& other) const { return not(*this == other); }
};

// P-C-AndCBSR-Pair-r13 ::= SEQUENCE (SIZE (1..2)) OF P-C-AndCBSR-r13
using p_c_and_cbsr_pair_r13_l = dyn_array<p_c_and_cbsr_r13_s>;

// P-C-AndCBSR-r15 ::= SEQUENCE
struct p_c_and_cbsr_r15_s {
  int8_t        p_c_r15 = -8;
  dyn_bitstring codebook_subset_restrict4_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const p_c_and_cbsr_r15_s& other) const;
  bool        operator!=(const p_c_and_cbsr_r15_s& other) const { return not(*this == other); }
};

// CSI-RS-ConfigBeamformed-r14 ::= SEQUENCE
struct csi_rs_cfg_bf_r14_s {
  using csi_rs_cfg_nzp_id_list_ext_r14_l_    = bounded_array<uint8_t, 7>;
  using csi_im_cfg_id_list_r14_l_            = bounded_array<uint8_t, 8>;
  using p_c_and_cbsr_per_res_cfg_list_r14_l_ = dyn_array<p_c_and_cbsr_pair_r13_l>;
  using ace_for4_tx_per_res_cfg_list_r14_l_  = bounded_array<bool, 7>;
  using csi_rs_cfg_nzp_ap_list_r14_l_        = dyn_array<csi_rs_cfg_nzp_r11_s>;

  // member variables
  bool                                 csi_rs_cfg_nzp_id_list_ext_r14_present    = false;
  bool                                 csi_im_cfg_id_list_r14_present            = false;
  bool                                 p_c_and_cbsr_per_res_cfg_list_r14_present = false;
  bool                                 ace_for4_tx_per_res_cfg_list_r14_present  = false;
  bool                                 alt_codebook_enabled_bf_r14_present       = false;
  bool                                 ch_meas_restrict_r14_present              = false;
  bool                                 csi_rs_cfg_nzp_ap_list_r14_present        = false;
  bool                                 nzp_res_cfg_original_v1430_present        = false;
  bool                                 csi_rs_nzp_activation_r14_present         = false;
  csi_rs_cfg_nzp_id_list_ext_r14_l_    csi_rs_cfg_nzp_id_list_ext_r14;
  csi_im_cfg_id_list_r14_l_            csi_im_cfg_id_list_r14;
  p_c_and_cbsr_per_res_cfg_list_r14_l_ p_c_and_cbsr_per_res_cfg_list_r14;
  ace_for4_tx_per_res_cfg_list_r14_l_  ace_for4_tx_per_res_cfg_list_r14;
  csi_rs_cfg_nzp_ap_list_r14_l_        csi_rs_cfg_nzp_ap_list_r14;
  csi_rs_cfg_nzp_v1430_s               nzp_res_cfg_original_v1430;
  csi_rs_cfg_nzp_activation_r14_s      csi_rs_nzp_activation_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_bf_r14_s& other) const;
  bool        operator!=(const csi_rs_cfg_bf_r14_s& other) const { return not(*this == other); }
};

// CSI-RS-ConfigNZP-EMIMO-r13 ::= CHOICE
struct csi_rs_cfg_nzp_emimo_r13_c {
  struct setup_s_ {
    using nzp_res_cfg_list_r13_l_ = dyn_array<nzp_res_cfg_r13_s>;
    struct cdm_type_r13_opts {
      enum options { cdm2, cdm4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<cdm_type_r13_opts> cdm_type_r13_e_;

    // member variables
    bool                    cdm_type_r13_present = false;
    nzp_res_cfg_list_r13_l_ nzp_res_cfg_list_r13;
    cdm_type_r13_e_         cdm_type_r13;
  };
  using types = setup_e;

  // choice methods
  csi_rs_cfg_nzp_emimo_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_nzp_emimo_r13_c& other) const;
  bool        operator!=(const csi_rs_cfg_nzp_emimo_r13_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigNZP-EMIMO-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigNZP-EMIMO-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CSI-RS-ConfigNZP-EMIMO-v1430 ::= SEQUENCE
struct csi_rs_cfg_nzp_emimo_v1430_s {
  using nzp_res_cfg_list_ext_r14_l_ = dyn_array<nzp_res_cfg_r13_s>;

  // member variables
  bool                        cdm_type_v1430_present = false;
  nzp_res_cfg_list_ext_r14_l_ nzp_res_cfg_list_ext_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_nzp_emimo_v1430_s& other) const;
  bool        operator!=(const csi_rs_cfg_nzp_emimo_v1430_s& other) const { return not(*this == other); }
};

// P-C-AndCBSR-Pair-r15 ::= SEQUENCE (SIZE (1..2)) OF P-C-AndCBSR-r15
using p_c_and_cbsr_pair_r15_l = dyn_array<p_c_and_cbsr_r15_s>;

// CQI-ReportModeAperiodic ::= ENUMERATED
struct cqi_report_mode_aperiodic_opts {
  enum options { rm12, rm20, rm22, rm30, rm31, rm32_v1250, rm10_v1310, rm11_v1310, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<cqi_report_mode_aperiodic_opts> cqi_report_mode_aperiodic_e;

// CSI-RS-ConfigBeamformed-r13 ::= SEQUENCE
struct csi_rs_cfg_bf_r13_s {
  using csi_rs_cfg_nzp_id_list_ext_r13_l_    = bounded_array<uint8_t, 7>;
  using csi_im_cfg_id_list_r13_l_            = bounded_array<uint8_t, 8>;
  using p_c_and_cbsr_per_res_cfg_list_r13_l_ = dyn_array<p_c_and_cbsr_pair_r13_l>;
  using ace_for4_tx_per_res_cfg_list_r13_l_  = bounded_array<bool, 7>;

  // member variables
  bool                                 csi_rs_cfg_nzp_id_list_ext_r13_present    = false;
  bool                                 csi_im_cfg_id_list_r13_present            = false;
  bool                                 p_c_and_cbsr_per_res_cfg_list_r13_present = false;
  bool                                 ace_for4_tx_per_res_cfg_list_r13_present  = false;
  bool                                 alt_codebook_enabled_bf_r13_present       = false;
  bool                                 ch_meas_restrict_r13_present              = false;
  csi_rs_cfg_nzp_id_list_ext_r13_l_    csi_rs_cfg_nzp_id_list_ext_r13;
  csi_im_cfg_id_list_r13_l_            csi_im_cfg_id_list_r13;
  p_c_and_cbsr_per_res_cfg_list_r13_l_ p_c_and_cbsr_per_res_cfg_list_r13;
  ace_for4_tx_per_res_cfg_list_r13_l_  ace_for4_tx_per_res_cfg_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_bf_r13_s& other) const;
  bool        operator!=(const csi_rs_cfg_bf_r13_s& other) const { return not(*this == other); }
};

// CSI-RS-ConfigBeamformed-v1430 ::= SEQUENCE
struct csi_rs_cfg_bf_v1430_s {
  using csi_rs_cfg_nzp_ap_list_r14_l_ = dyn_array<csi_rs_cfg_nzp_r11_s>;

  // member variables
  bool                            csi_rs_cfg_nzp_ap_list_r14_present = false;
  bool                            nzp_res_cfg_original_v1430_present = false;
  bool                            csi_rs_nzp_activation_r14_present  = false;
  csi_rs_cfg_nzp_ap_list_r14_l_   csi_rs_cfg_nzp_ap_list_r14;
  csi_rs_cfg_nzp_v1430_s          nzp_res_cfg_original_v1430;
  csi_rs_cfg_nzp_activation_r14_s csi_rs_nzp_activation_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_bf_v1430_s& other) const;
  bool        operator!=(const csi_rs_cfg_bf_v1430_s& other) const { return not(*this == other); }
};

// CSI-RS-ConfigEMIMO2-r14 ::= CHOICE
struct csi_rs_cfg_emimo2_r14_c {
  using types = setup_e;

  // choice methods
  csi_rs_cfg_emimo2_r14_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_emimo2_r14_c& other) const;
  bool        operator!=(const csi_rs_cfg_emimo2_r14_c& other) const { return not(*this == other); }
  // getters
  csi_rs_cfg_bf_r14_s& setup()
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigEMIMO2-r14");
    return c;
  }
  const csi_rs_cfg_bf_r14_s& setup() const
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigEMIMO2-r14");
    return c;
  }
  void                 set_release();
  csi_rs_cfg_bf_r14_s& set_setup();

private:
  types               type_;
  csi_rs_cfg_bf_r14_s c;
};

// CSI-RS-ConfigNonPrecoded-r13 ::= SEQUENCE
struct csi_rs_cfg_non_precoded_r13_s {
  struct codebook_cfg_n1_r13_opts {
    enum options { n1, n2, n3, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<codebook_cfg_n1_r13_opts> codebook_cfg_n1_r13_e_;
  struct codebook_cfg_n2_r13_opts {
    enum options { n1, n2, n3, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<codebook_cfg_n2_r13_opts> codebook_cfg_n2_r13_e_;
  struct codebook_over_sampling_rate_cfg_o1_r13_opts {
    enum options { n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<codebook_over_sampling_rate_cfg_o1_r13_opts> codebook_over_sampling_rate_cfg_o1_r13_e_;
  struct codebook_over_sampling_rate_cfg_o2_r13_opts {
    enum options { n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<codebook_over_sampling_rate_cfg_o2_r13_opts> codebook_over_sampling_rate_cfg_o2_r13_e_;
  using csi_im_cfg_id_list_r13_l_ = bounded_array<uint8_t, 2>;

  // member variables
  bool                                      p_c_and_cbsr_list_r13_present                  = false;
  bool                                      codebook_over_sampling_rate_cfg_o1_r13_present = false;
  bool                                      codebook_over_sampling_rate_cfg_o2_r13_present = false;
  bool                                      csi_im_cfg_id_list_r13_present                 = false;
  bool                                      csi_rs_cfg_nzp_emimo_r13_present               = false;
  p_c_and_cbsr_pair_r13_l                   p_c_and_cbsr_list_r13;
  codebook_cfg_n1_r13_e_                    codebook_cfg_n1_r13;
  codebook_cfg_n2_r13_e_                    codebook_cfg_n2_r13;
  codebook_over_sampling_rate_cfg_o1_r13_e_ codebook_over_sampling_rate_cfg_o1_r13;
  codebook_over_sampling_rate_cfg_o2_r13_e_ codebook_over_sampling_rate_cfg_o2_r13;
  uint8_t                                   codebook_cfg_r13 = 1;
  csi_im_cfg_id_list_r13_l_                 csi_im_cfg_id_list_r13;
  csi_rs_cfg_nzp_emimo_r13_c                csi_rs_cfg_nzp_emimo_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_non_precoded_r13_s& other) const;
  bool        operator!=(const csi_rs_cfg_non_precoded_r13_s& other) const { return not(*this == other); }
};

// CSI-RS-ConfigNonPrecoded-v1430 ::= SEQUENCE
struct csi_rs_cfg_non_precoded_v1430_s {
  struct codebook_cfg_n1_v1430_opts {
    enum options { n5, n6, n7, n10, n12, n14, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<codebook_cfg_n1_v1430_opts> codebook_cfg_n1_v1430_e_;
  struct codebook_cfg_n2_v1430_opts {
    enum options { n5, n6, n7, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<codebook_cfg_n2_v1430_opts> codebook_cfg_n2_v1430_e_;

  // member variables
  bool                         csi_rs_cfg_nzp_emimo_v1430_present = false;
  csi_rs_cfg_nzp_emimo_v1430_s csi_rs_cfg_nzp_emimo_v1430;
  codebook_cfg_n1_v1430_e_     codebook_cfg_n1_v1430;
  codebook_cfg_n2_v1430_e_     codebook_cfg_n2_v1430;
  csi_rs_cfg_nzp_v1430_s       nzp_res_cfg_tm9_original_v1430;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_non_precoded_v1430_s& other) const;
  bool        operator!=(const csi_rs_cfg_non_precoded_v1430_s& other) const { return not(*this == other); }
};

// CSI-RS-ConfigNonPrecoded-v1480 ::= SEQUENCE
struct csi_rs_cfg_non_precoded_v1480_s {
  struct codebook_cfg_n1_v1480_opts {
    enum options { n5, n6, n7, n10, n12, n14, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<codebook_cfg_n1_v1480_opts> codebook_cfg_n1_v1480_e_;
  struct codebook_cfg_n2_r1480_opts {
    enum options { n5, n6, n7, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<codebook_cfg_n2_r1480_opts> codebook_cfg_n2_r1480_e_;

  // member variables
  bool                         csi_rs_cfg_nzp_emimo_v1480_present = false;
  bool                         codebook_cfg_n1_v1480_present      = false;
  bool                         codebook_cfg_n2_r1480_present      = false;
  csi_rs_cfg_nzp_emimo_v1430_s csi_rs_cfg_nzp_emimo_v1480;
  codebook_cfg_n1_v1480_e_     codebook_cfg_n1_v1480;
  codebook_cfg_n2_r1480_e_     codebook_cfg_n2_r1480;
  csi_rs_cfg_nzp_v1430_s       nzp_res_cfg_tm9_original_v1480;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_non_precoded_v1480_s& other) const;
  bool        operator!=(const csi_rs_cfg_non_precoded_v1480_s& other) const { return not(*this == other); }
};

// CSI-RS-ConfigNonPrecoded-v1530 ::= SEQUENCE
struct csi_rs_cfg_non_precoded_v1530_s {
  bool                    p_c_and_cbsr_list_r15_present = false;
  p_c_and_cbsr_pair_r15_l p_c_and_cbsr_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_non_precoded_v1530_s& other) const;
  bool        operator!=(const csi_rs_cfg_non_precoded_v1530_s& other) const { return not(*this == other); }
};

// P-C-AndCBSR-r11 ::= SEQUENCE
struct p_c_and_cbsr_r11_s {
  int8_t        p_c_r11 = -8;
  dyn_bitstring codebook_subset_restrict_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const p_c_and_cbsr_r11_s& other) const;
  bool        operator!=(const p_c_and_cbsr_r11_s& other) const { return not(*this == other); }
};

// CQI-ReportAperiodicProc-r11 ::= SEQUENCE
struct cqi_report_aperiodic_proc_r11_s {
  cqi_report_mode_aperiodic_e cqi_report_mode_aperiodic_r11;
  bool                        trigger01_r11 = false;
  bool                        trigger10_r11 = false;
  bool                        trigger11_r11 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_aperiodic_proc_r11_s& other) const;
  bool        operator!=(const cqi_report_aperiodic_proc_r11_s& other) const { return not(*this == other); }
};

// CQI-ReportAperiodicProc-v1310 ::= SEQUENCE
struct cqi_report_aperiodic_proc_v1310_s {
  bool trigger001_r13 = false;
  bool trigger010_r13 = false;
  bool trigger011_r13 = false;
  bool trigger100_r13 = false;
  bool trigger101_r13 = false;
  bool trigger110_r13 = false;
  bool trigger111_r13 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_aperiodic_proc_v1310_s& other) const;
  bool        operator!=(const cqi_report_aperiodic_proc_v1310_s& other) const { return not(*this == other); }
};

// CQI-ReportBothProc-r11 ::= SEQUENCE
struct cqi_report_both_proc_r11_s {
  bool    ri_ref_csi_process_id_r11_present = false;
  bool    pmi_ri_report_r11_present         = false;
  uint8_t ri_ref_csi_process_id_r11         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_both_proc_r11_s& other) const;
  bool        operator!=(const cqi_report_both_proc_r11_s& other) const { return not(*this == other); }
};

// CRI-ReportConfig-r13 ::= CHOICE
struct cri_report_cfg_r13_c {
  struct setup_s_ {
    bool     cri_cfg_idx2_r13_present = false;
    uint16_t cri_cfg_idx_r13          = 0;
    uint16_t cri_cfg_idx2_r13         = 0;
  };
  using types = setup_e;

  // choice methods
  cri_report_cfg_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cri_report_cfg_r13_c& other) const;
  bool        operator!=(const cri_report_cfg_r13_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "CRI-ReportConfig-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "CRI-ReportConfig-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CSI-RS-ConfigEMIMO-Hybrid-r14 ::= CHOICE
struct csi_rs_cfg_emimo_hybrid_r14_c {
  struct setup_s_ {
    bool                    periodicity_offset_idx_r14_present = false;
    bool                    emimo_type2_r14_present            = false;
    uint16_t                periodicity_offset_idx_r14         = 0;
    csi_rs_cfg_emimo2_r14_c emimo_type2_r14;
  };
  using types = setup_e;

  // choice methods
  csi_rs_cfg_emimo_hybrid_r14_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_emimo_hybrid_r14_c& other) const;
  bool        operator!=(const csi_rs_cfg_emimo_hybrid_r14_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigEMIMO-Hybrid-r14");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigEMIMO-Hybrid-r14");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CSI-RS-ConfigEMIMO-r13 ::= CHOICE
struct csi_rs_cfg_emimo_r13_c {
  struct setup_c_ {
    struct types_opts {
      enum options { non_precoded_r13, bf_r13, nulltype } value;

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
    bool        operator==(const setup_c_& other) const;
    bool        operator!=(const setup_c_& other) const { return not(*this == other); }
    // getters
    csi_rs_cfg_non_precoded_r13_s& non_precoded_r13()
    {
      assert_choice_type(types::non_precoded_r13, type_, "setup");
      return c.get<csi_rs_cfg_non_precoded_r13_s>();
    }
    csi_rs_cfg_bf_r13_s& bf_r13()
    {
      assert_choice_type(types::bf_r13, type_, "setup");
      return c.get<csi_rs_cfg_bf_r13_s>();
    }
    const csi_rs_cfg_non_precoded_r13_s& non_precoded_r13() const
    {
      assert_choice_type(types::non_precoded_r13, type_, "setup");
      return c.get<csi_rs_cfg_non_precoded_r13_s>();
    }
    const csi_rs_cfg_bf_r13_s& bf_r13() const
    {
      assert_choice_type(types::bf_r13, type_, "setup");
      return c.get<csi_rs_cfg_bf_r13_s>();
    }
    csi_rs_cfg_non_precoded_r13_s& set_non_precoded_r13();
    csi_rs_cfg_bf_r13_s&           set_bf_r13();

  private:
    types                                                               type_;
    choice_buffer_t<csi_rs_cfg_bf_r13_s, csi_rs_cfg_non_precoded_r13_s> c;

    void destroy_();
  };
  using types = setup_e;

  // choice methods
  csi_rs_cfg_emimo_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_emimo_r13_c& other) const;
  bool        operator!=(const csi_rs_cfg_emimo_r13_c& other) const { return not(*this == other); }
  // getters
  setup_c_& setup()
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigEMIMO-r13");
    return c;
  }
  const setup_c_& setup() const
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigEMIMO-r13");
    return c;
  }
  void      set_release();
  setup_c_& set_setup();

private:
  types    type_;
  setup_c_ c;
};

// CSI-RS-ConfigEMIMO-v1430 ::= CHOICE
struct csi_rs_cfg_emimo_v1430_c {
  struct setup_c_ {
    struct types_opts {
      enum options { non_precoded_v1430, bf_v1430, nulltype } value;

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
    bool        operator==(const setup_c_& other) const;
    bool        operator!=(const setup_c_& other) const { return not(*this == other); }
    // getters
    csi_rs_cfg_non_precoded_v1430_s& non_precoded_v1430()
    {
      assert_choice_type(types::non_precoded_v1430, type_, "setup");
      return c.get<csi_rs_cfg_non_precoded_v1430_s>();
    }
    csi_rs_cfg_bf_v1430_s& bf_v1430()
    {
      assert_choice_type(types::bf_v1430, type_, "setup");
      return c.get<csi_rs_cfg_bf_v1430_s>();
    }
    const csi_rs_cfg_non_precoded_v1430_s& non_precoded_v1430() const
    {
      assert_choice_type(types::non_precoded_v1430, type_, "setup");
      return c.get<csi_rs_cfg_non_precoded_v1430_s>();
    }
    const csi_rs_cfg_bf_v1430_s& bf_v1430() const
    {
      assert_choice_type(types::bf_v1430, type_, "setup");
      return c.get<csi_rs_cfg_bf_v1430_s>();
    }
    csi_rs_cfg_non_precoded_v1430_s& set_non_precoded_v1430();
    csi_rs_cfg_bf_v1430_s&           set_bf_v1430();

  private:
    types                                                                   type_;
    choice_buffer_t<csi_rs_cfg_bf_v1430_s, csi_rs_cfg_non_precoded_v1430_s> c;

    void destroy_();
  };
  using types = setup_e;

  // choice methods
  csi_rs_cfg_emimo_v1430_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_emimo_v1430_c& other) const;
  bool        operator!=(const csi_rs_cfg_emimo_v1430_c& other) const { return not(*this == other); }
  // getters
  setup_c_& setup()
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigEMIMO-v1430");
    return c;
  }
  const setup_c_& setup() const
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigEMIMO-v1430");
    return c;
  }
  void      set_release();
  setup_c_& set_setup();

private:
  types    type_;
  setup_c_ c;
};

// CSI-RS-ConfigEMIMO-v1480 ::= CHOICE
struct csi_rs_cfg_emimo_v1480_c {
  struct setup_c_ {
    struct types_opts {
      enum options { non_precoded_v1480, bf_v1480, nulltype } value;

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
    bool        operator==(const setup_c_& other) const;
    bool        operator!=(const setup_c_& other) const { return not(*this == other); }
    // getters
    csi_rs_cfg_non_precoded_v1480_s& non_precoded_v1480()
    {
      assert_choice_type(types::non_precoded_v1480, type_, "setup");
      return c.get<csi_rs_cfg_non_precoded_v1480_s>();
    }
    csi_rs_cfg_bf_v1430_s& bf_v1480()
    {
      assert_choice_type(types::bf_v1480, type_, "setup");
      return c.get<csi_rs_cfg_bf_v1430_s>();
    }
    const csi_rs_cfg_non_precoded_v1480_s& non_precoded_v1480() const
    {
      assert_choice_type(types::non_precoded_v1480, type_, "setup");
      return c.get<csi_rs_cfg_non_precoded_v1480_s>();
    }
    const csi_rs_cfg_bf_v1430_s& bf_v1480() const
    {
      assert_choice_type(types::bf_v1480, type_, "setup");
      return c.get<csi_rs_cfg_bf_v1430_s>();
    }
    csi_rs_cfg_non_precoded_v1480_s& set_non_precoded_v1480();
    csi_rs_cfg_bf_v1430_s&           set_bf_v1480();

  private:
    types                                                                   type_;
    choice_buffer_t<csi_rs_cfg_bf_v1430_s, csi_rs_cfg_non_precoded_v1480_s> c;

    void destroy_();
  };
  using types = setup_e;

  // choice methods
  csi_rs_cfg_emimo_v1480_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_emimo_v1480_c& other) const;
  bool        operator!=(const csi_rs_cfg_emimo_v1480_c& other) const { return not(*this == other); }
  // getters
  setup_c_& setup()
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigEMIMO-v1480");
    return c;
  }
  const setup_c_& setup() const
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigEMIMO-v1480");
    return c;
  }
  void      set_release();
  setup_c_& set_setup();

private:
  types    type_;
  setup_c_ c;
};

// CSI-RS-ConfigEMIMO-v1530 ::= CHOICE
struct csi_rs_cfg_emimo_v1530_c {
  struct setup_c_ {
    struct types_opts {
      enum options { non_precoded_v1530, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::non_precoded_v1530; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const setup_c_& other) const;
    bool        operator!=(const setup_c_& other) const { return not(*this == other); }
    // getters
    csi_rs_cfg_non_precoded_v1530_s&       non_precoded_v1530() { return c; }
    const csi_rs_cfg_non_precoded_v1530_s& non_precoded_v1530() const { return c; }

  private:
    csi_rs_cfg_non_precoded_v1530_s c;
  };
  using types = setup_e;

  // choice methods
  csi_rs_cfg_emimo_v1530_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_emimo_v1530_c& other) const;
  bool        operator!=(const csi_rs_cfg_emimo_v1530_c& other) const { return not(*this == other); }
  // getters
  setup_c_& setup()
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigEMIMO-v1530");
    return c;
  }
  const setup_c_& setup() const
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigEMIMO-v1530");
    return c;
  }
  void      set_release();
  setup_c_& set_setup();

private:
  types    type_;
  setup_c_ c;
};

// P-C-AndCBSR-Pair-r13a ::= SEQUENCE (SIZE (1..2)) OF P-C-AndCBSR-r11
using p_c_and_cbsr_pair_r13a_l = dyn_array<p_c_and_cbsr_r11_s>;

// CQI-ReportPeriodicProcExt-r11 ::= SEQUENCE
struct cqi_report_periodic_proc_ext_r11_s {
  struct cqi_format_ind_periodic_r11_c_ {
    struct wideband_cqi_r11_s_ {
      struct csi_report_mode_r11_opts {
        enum options { submode1, submode2, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<csi_report_mode_r11_opts> csi_report_mode_r11_e_;

      // member variables
      bool                   csi_report_mode_r11_present = false;
      csi_report_mode_r11_e_ csi_report_mode_r11;
    };
    struct subband_cqi_r11_s_ {
      struct periodicity_factor_r11_opts {
        enum options { n2, n4, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<periodicity_factor_r11_opts> periodicity_factor_r11_e_;

      // member variables
      uint8_t                   k = 1;
      periodicity_factor_r11_e_ periodicity_factor_r11;
    };
    struct types_opts {
      enum options { wideband_cqi_r11, subband_cqi_r11, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    cqi_format_ind_periodic_r11_c_() = default;
    cqi_format_ind_periodic_r11_c_(const cqi_format_ind_periodic_r11_c_& other);
    cqi_format_ind_periodic_r11_c_& operator=(const cqi_format_ind_periodic_r11_c_& other);
    ~cqi_format_ind_periodic_r11_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const cqi_format_ind_periodic_r11_c_& other) const;
    bool        operator!=(const cqi_format_ind_periodic_r11_c_& other) const { return not(*this == other); }
    // getters
    wideband_cqi_r11_s_& wideband_cqi_r11()
    {
      assert_choice_type(types::wideband_cqi_r11, type_, "cqi-FormatIndicatorPeriodic-r11");
      return c.get<wideband_cqi_r11_s_>();
    }
    subband_cqi_r11_s_& subband_cqi_r11()
    {
      assert_choice_type(types::subband_cqi_r11, type_, "cqi-FormatIndicatorPeriodic-r11");
      return c.get<subband_cqi_r11_s_>();
    }
    const wideband_cqi_r11_s_& wideband_cqi_r11() const
    {
      assert_choice_type(types::wideband_cqi_r11, type_, "cqi-FormatIndicatorPeriodic-r11");
      return c.get<wideband_cqi_r11_s_>();
    }
    const subband_cqi_r11_s_& subband_cqi_r11() const
    {
      assert_choice_type(types::subband_cqi_r11, type_, "cqi-FormatIndicatorPeriodic-r11");
      return c.get<subband_cqi_r11_s_>();
    }
    wideband_cqi_r11_s_& set_wideband_cqi_r11();
    subband_cqi_r11_s_&  set_subband_cqi_r11();

  private:
    types                                                    type_;
    choice_buffer_t<subband_cqi_r11_s_, wideband_cqi_r11_s_> c;

    void destroy_();
  };
  struct csi_cfg_idx_r11_c_ {
    struct setup_s_ {
      bool     ri_cfg_idx2_r11_present = false;
      uint16_t cqi_pmi_cfg_idx2_r11    = 0;
      uint16_t ri_cfg_idx2_r11         = 0;
    };
    using types = setup_e;

    // choice methods
    csi_cfg_idx_r11_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const csi_cfg_idx_r11_c_& other) const;
    bool        operator!=(const csi_cfg_idx_r11_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "csi-ConfigIndex-r11");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "csi-ConfigIndex-r11");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct periodicity_factor_wb_r13_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<periodicity_factor_wb_r13_opts> periodicity_factor_wb_r13_e_;

  // member variables
  bool                           ext                                 = false;
  bool                           ri_cfg_idx_r11_present              = false;
  bool                           csi_cfg_idx_r11_present             = false;
  uint8_t                        cqi_report_periodic_proc_ext_id_r11 = 1;
  uint16_t                       cqi_pmi_cfg_idx_r11                 = 0;
  cqi_format_ind_periodic_r11_c_ cqi_format_ind_periodic_r11;
  uint16_t                       ri_cfg_idx_r11 = 0;
  csi_cfg_idx_r11_c_             csi_cfg_idx_r11;
  // ...
  // group 0
  copy_ptr<cri_report_cfg_r13_c> cri_report_cfg_r13;
  // group 1
  bool                         periodicity_factor_wb_r13_present = false;
  periodicity_factor_wb_r13_e_ periodicity_factor_wb_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_periodic_proc_ext_r11_s& other) const;
  bool        operator!=(const cqi_report_periodic_proc_ext_r11_s& other) const { return not(*this == other); }
};

// CSI-IM-Config-r11 ::= SEQUENCE
struct csi_im_cfg_r11_s {
  bool    ext               = false;
  uint8_t csi_im_cfg_id_r11 = 1;
  uint8_t res_cfg_r11       = 0;
  uint8_t sf_cfg_r11        = 0;
  // ...
  // group 0
  bool interference_meas_restrict_r13_present = false;
  bool interference_meas_restrict_r13         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_im_cfg_r11_s& other) const;
  bool        operator!=(const csi_im_cfg_r11_s& other) const { return not(*this == other); }
};

// CSI-IM-ConfigExt-r12 ::= SEQUENCE
struct csi_im_cfg_ext_r12_s {
  bool    ext                 = false;
  uint8_t csi_im_cfg_id_v1250 = 4;
  uint8_t res_cfg_r12         = 0;
  uint8_t sf_cfg_r12          = 0;
  // ...
  // group 0
  bool    interference_meas_restrict_r13_present = false;
  bool    csi_im_cfg_id_v1310_present            = false;
  bool    interference_meas_restrict_r13         = false;
  uint8_t csi_im_cfg_id_v1310                    = 5;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_im_cfg_ext_r12_s& other) const;
  bool        operator!=(const csi_im_cfg_ext_r12_s& other) const { return not(*this == other); }
};

// CSI-Process-r11 ::= SEQUENCE
struct csi_process_r11_s {
  struct csi_im_cfg_id_list_r12_c_ {
    using setup_l_ = bounded_array<uint8_t, 2>;
    using types    = setup_e;

    // choice methods
    csi_im_cfg_id_list_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const csi_im_cfg_id_list_r12_c_& other) const;
    bool        operator!=(const csi_im_cfg_id_list_r12_c_& other) const { return not(*this == other); }
    // getters
    setup_l_& setup()
    {
      assert_choice_type(types::setup, type_, "csi-IM-ConfigIdList-r12");
      return c;
    }
    const setup_l_& setup() const
    {
      assert_choice_type(types::setup, type_, "csi-IM-ConfigIdList-r12");
      return c;
    }
    void      set_release();
    setup_l_& set_setup();

  private:
    types    type_;
    setup_l_ c;
  };
  struct cqi_report_aperiodic_proc2_r12_c_ {
    using types = setup_e;

    // choice methods
    cqi_report_aperiodic_proc2_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const cqi_report_aperiodic_proc2_r12_c_& other) const;
    bool        operator!=(const cqi_report_aperiodic_proc2_r12_c_& other) const { return not(*this == other); }
    // getters
    cqi_report_aperiodic_proc_r11_s& setup()
    {
      assert_choice_type(types::setup, type_, "cqi-ReportAperiodicProc2-r12");
      return c;
    }
    const cqi_report_aperiodic_proc_r11_s& setup() const
    {
      assert_choice_type(types::setup, type_, "cqi-ReportAperiodicProc2-r12");
      return c;
    }
    void                             set_release();
    cqi_report_aperiodic_proc_r11_s& set_setup();

  private:
    types                           type_;
    cqi_report_aperiodic_proc_r11_s c;
  };
  struct cqi_report_aperiodic_proc_v1310_c_ {
    using types = setup_e;

    // choice methods
    cqi_report_aperiodic_proc_v1310_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const cqi_report_aperiodic_proc_v1310_c_& other) const;
    bool        operator!=(const cqi_report_aperiodic_proc_v1310_c_& other) const { return not(*this == other); }
    // getters
    cqi_report_aperiodic_proc_v1310_s& setup()
    {
      assert_choice_type(types::setup, type_, "cqi-ReportAperiodicProc-v1310");
      return c;
    }
    const cqi_report_aperiodic_proc_v1310_s& setup() const
    {
      assert_choice_type(types::setup, type_, "cqi-ReportAperiodicProc-v1310");
      return c;
    }
    void                               set_release();
    cqi_report_aperiodic_proc_v1310_s& set_setup();

  private:
    types                             type_;
    cqi_report_aperiodic_proc_v1310_s c;
  };
  struct cqi_report_aperiodic_proc2_v1310_c_ {
    using types = setup_e;

    // choice methods
    cqi_report_aperiodic_proc2_v1310_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const cqi_report_aperiodic_proc2_v1310_c_& other) const;
    bool        operator!=(const cqi_report_aperiodic_proc2_v1310_c_& other) const { return not(*this == other); }
    // getters
    cqi_report_aperiodic_proc_v1310_s& setup()
    {
      assert_choice_type(types::setup, type_, "cqi-ReportAperiodicProc2-v1310");
      return c;
    }
    const cqi_report_aperiodic_proc_v1310_s& setup() const
    {
      assert_choice_type(types::setup, type_, "cqi-ReportAperiodicProc2-v1310");
      return c;
    }
    void                               set_release();
    cqi_report_aperiodic_proc_v1310_s& set_setup();

  private:
    types                             type_;
    cqi_report_aperiodic_proc_v1310_s c;
  };

  // member variables
  bool                            ext                                     = false;
  bool                            cqi_report_both_proc_r11_present        = false;
  bool                            cqi_report_periodic_proc_id_r11_present = false;
  bool                            cqi_report_aperiodic_proc_r11_present   = false;
  uint8_t                         csi_process_id_r11                      = 1;
  uint8_t                         csi_rs_cfg_nzp_id_r11                   = 1;
  uint8_t                         csi_im_cfg_id_r11                       = 1;
  p_c_and_cbsr_pair_r13a_l        p_c_and_cbsr_list_r11;
  cqi_report_both_proc_r11_s      cqi_report_both_proc_r11;
  uint8_t                         cqi_report_periodic_proc_id_r11 = 0;
  cqi_report_aperiodic_proc_r11_s cqi_report_aperiodic_proc_r11;
  // ...
  // group 0
  bool                                        alt_codebook_enabled_for4_tx_proc_r12_present = false;
  copy_ptr<csi_im_cfg_id_list_r12_c_>         csi_im_cfg_id_list_r12;
  copy_ptr<cqi_report_aperiodic_proc2_r12_c_> cqi_report_aperiodic_proc2_r12;
  // group 1
  copy_ptr<cqi_report_aperiodic_proc_v1310_c_>  cqi_report_aperiodic_proc_v1310;
  copy_ptr<cqi_report_aperiodic_proc2_v1310_c_> cqi_report_aperiodic_proc2_v1310;
  copy_ptr<csi_rs_cfg_emimo_r13_c>              emimo_type_r13;
  // group 2
  bool                                    advanced_codebook_enabled_r14_present = false;
  copy_ptr<csi_rs_cfg_emimo_v1430_c>      dummy;
  copy_ptr<csi_rs_cfg_emimo_hybrid_r14_c> emimo_hybrid_r14;
  bool                                    advanced_codebook_enabled_r14 = false;
  // group 3
  copy_ptr<csi_rs_cfg_emimo_v1480_c> emimo_type_v1480;
  // group 4
  bool                               fe_comp_csi_enabled_v1530_present = false;
  bool                               fe_comp_csi_enabled_v1530         = false;
  copy_ptr<csi_rs_cfg_emimo_v1530_c> emimo_type_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_process_r11_s& other) const;
  bool        operator!=(const csi_process_r11_s& other) const { return not(*this == other); }
};

// CQI-ReportPeriodicProcExtToAddModList-r11 ::= SEQUENCE (SIZE (1..3)) OF CQI-ReportPeriodicProcExt-r11
using cqi_report_periodic_proc_ext_to_add_mod_list_r11_l = dyn_array<cqi_report_periodic_proc_ext_r11_s>;

// CQI-ReportPeriodicProcExtToReleaseList-r11 ::= SEQUENCE (SIZE (1..3)) OF INTEGER (1..3)
using cqi_report_periodic_proc_ext_to_release_list_r11_l = bounded_array<uint8_t, 3>;

// CSI-IM-ConfigToAddModList-r11 ::= SEQUENCE (SIZE (1..3)) OF CSI-IM-Config-r11
using csi_im_cfg_to_add_mod_list_r11_l = dyn_array<csi_im_cfg_r11_s>;

// CSI-IM-ConfigToAddModListExt-r13 ::= SEQUENCE (SIZE (1..20)) OF CSI-IM-ConfigExt-r12
using csi_im_cfg_to_add_mod_list_ext_r13_l = dyn_array<csi_im_cfg_ext_r12_s>;

// CSI-IM-ConfigToReleaseList-r11 ::= SEQUENCE (SIZE (1..3)) OF INTEGER (1..3)
using csi_im_cfg_to_release_list_r11_l = bounded_array<uint8_t, 3>;

// CSI-IM-ConfigToReleaseListExt-r13 ::= SEQUENCE (SIZE (1..20)) OF INTEGER (5..24)
using csi_im_cfg_to_release_list_ext_r13_l = bounded_array<uint8_t, 20>;

// CSI-ProcessToAddModList-r11 ::= SEQUENCE (SIZE (1..4)) OF CSI-Process-r11
using csi_process_to_add_mod_list_r11_l = dyn_array<csi_process_r11_s>;

// CSI-ProcessToReleaseList-r11 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (1..4)
using csi_process_to_release_list_r11_l = bounded_array<uint8_t, 4>;

// DCI7-CandidatesPerAL-SPDCCH-r15 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (0..6)
using dci7_candidates_per_al_spdcch_r15_l = bounded_array<uint8_t, 4>;

// N4SPUCCH-Resource-r15 ::= SEQUENCE
struct n4_spucch_res_r15_s {
  uint8_t n4start_prb_r15 = 0;
  uint8_t n4nof_prb_r15   = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const n4_spucch_res_r15_s& other) const;
  bool        operator!=(const n4_spucch_res_r15_s& other) const { return not(*this == other); }
};

// ZeroTxPowerCSI-RS-r12 ::= SEQUENCE
struct zero_tx_pwr_csi_rs_r12_s {
  fixed_bitstring<16> zero_tx_pwr_res_cfg_list_r12;
  uint8_t             zero_tx_pwr_sf_cfg_r12 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const zero_tx_pwr_csi_rs_r12_s& other) const;
  bool        operator!=(const zero_tx_pwr_csi_rs_r12_s& other) const { return not(*this == other); }
};

// CQI-ReportAperiodic-r10 ::= CHOICE
struct cqi_report_aperiodic_r10_c {
  struct setup_s_ {
    struct aperiodic_csi_trigger_r10_s_ {
      fixed_bitstring<8> trigger1_r10;
      fixed_bitstring<8> trigger2_r10;
    };

    // member variables
    bool                         aperiodic_csi_trigger_r10_present = false;
    cqi_report_mode_aperiodic_e  cqi_report_mode_aperiodic_r10;
    aperiodic_csi_trigger_r10_s_ aperiodic_csi_trigger_r10;
  };
  using types = setup_e;

  // choice methods
  cqi_report_aperiodic_r10_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_aperiodic_r10_c& other) const;
  bool        operator!=(const cqi_report_aperiodic_r10_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "CQI-ReportAperiodic-r10");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "CQI-ReportAperiodic-r10");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CQI-ReportAperiodic-v1250 ::= CHOICE
struct cqi_report_aperiodic_v1250_c {
  struct setup_s_ {
    struct aperiodic_csi_trigger_v1250_s_ {
      struct trigger_sf_set_ind_r12_opts {
        enum options { s1, s2, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<trigger_sf_set_ind_r12_opts> trigger_sf_set_ind_r12_e_;

      // member variables
      trigger_sf_set_ind_r12_e_ trigger_sf_set_ind_r12;
      fixed_bitstring<8>        trigger1_sf_set_ind_r12;
      fixed_bitstring<8>        trigger2_sf_set_ind_r12;
    };

    // member variables
    aperiodic_csi_trigger_v1250_s_ aperiodic_csi_trigger_v1250;
  };
  using types = setup_e;

  // choice methods
  cqi_report_aperiodic_v1250_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_aperiodic_v1250_c& other) const;
  bool        operator!=(const cqi_report_aperiodic_v1250_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "CQI-ReportAperiodic-v1250");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "CQI-ReportAperiodic-v1250");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CQI-ReportAperiodic-v1310 ::= CHOICE
struct cqi_report_aperiodic_v1310_c {
  struct setup_s_ {
    struct aperiodic_csi_trigger_v1310_s_ {
      fixed_bitstring<32> trigger1_r13;
      fixed_bitstring<32> trigger2_r13;
      fixed_bitstring<32> trigger3_r13;
      fixed_bitstring<32> trigger4_r13;
      fixed_bitstring<32> trigger5_r13;
      fixed_bitstring<32> trigger6_r13;
    };
    struct aperiodic_csi_trigger2_r13_c_ {
      struct setup_s_ {
        fixed_bitstring<32> trigger1_sf_set_ind_r13;
        fixed_bitstring<32> trigger2_sf_set_ind_r13;
        fixed_bitstring<32> trigger3_sf_set_ind_r13;
        fixed_bitstring<32> trigger4_sf_set_ind_r13;
        fixed_bitstring<32> trigger5_sf_set_ind_r13;
        fixed_bitstring<32> trigger6_sf_set_ind_r13;
      };
      using types = setup_e;

      // choice methods
      aperiodic_csi_trigger2_r13_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const aperiodic_csi_trigger2_r13_c_& other) const;
      bool        operator!=(const aperiodic_csi_trigger2_r13_c_& other) const { return not(*this == other); }
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "aperiodicCSI-Trigger2-r13");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "aperiodicCSI-Trigger2-r13");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };

    // member variables
    bool                           aperiodic_csi_trigger_v1310_present = false;
    bool                           aperiodic_csi_trigger2_r13_present  = false;
    aperiodic_csi_trigger_v1310_s_ aperiodic_csi_trigger_v1310;
    aperiodic_csi_trigger2_r13_c_  aperiodic_csi_trigger2_r13;
  };
  using types = setup_e;

  // choice methods
  cqi_report_aperiodic_v1310_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_aperiodic_v1310_c& other) const;
  bool        operator!=(const cqi_report_aperiodic_v1310_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "CQI-ReportAperiodic-v1310");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "CQI-ReportAperiodic-v1310");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CQI-ReportAperiodicHybrid-r14 ::= SEQUENCE
struct cqi_report_aperiodic_hybrid_r14_s {
  struct triggers_r14_c_ {
    struct one_bit_r14_s_ {
      fixed_bitstring<8> trigger1_ind_r14;
    };
    struct two_bit_r14_s_ {
      fixed_bitstring<8> trigger01_ind_r14;
      fixed_bitstring<8> trigger10_ind_r14;
      fixed_bitstring<8> trigger11_ind_r14;
    };
    struct three_bit_r14_s_ {
      fixed_bitstring<32> trigger001_ind_r14;
      fixed_bitstring<32> trigger010_ind_r14;
      fixed_bitstring<32> trigger011_ind_r14;
      fixed_bitstring<32> trigger100_ind_r14;
      fixed_bitstring<32> trigger101_ind_r14;
      fixed_bitstring<32> trigger110_ind_r14;
      fixed_bitstring<32> trigger111_ind_r14;
    };
    struct types_opts {
      enum options { one_bit_r14, two_bit_r14, three_bit_r14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    triggers_r14_c_() = default;
    triggers_r14_c_(const triggers_r14_c_& other);
    triggers_r14_c_& operator=(const triggers_r14_c_& other);
    ~triggers_r14_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const triggers_r14_c_& other) const;
    bool        operator!=(const triggers_r14_c_& other) const { return not(*this == other); }
    // getters
    one_bit_r14_s_& one_bit_r14()
    {
      assert_choice_type(types::one_bit_r14, type_, "triggers-r14");
      return c.get<one_bit_r14_s_>();
    }
    two_bit_r14_s_& two_bit_r14()
    {
      assert_choice_type(types::two_bit_r14, type_, "triggers-r14");
      return c.get<two_bit_r14_s_>();
    }
    three_bit_r14_s_& three_bit_r14()
    {
      assert_choice_type(types::three_bit_r14, type_, "triggers-r14");
      return c.get<three_bit_r14_s_>();
    }
    const one_bit_r14_s_& one_bit_r14() const
    {
      assert_choice_type(types::one_bit_r14, type_, "triggers-r14");
      return c.get<one_bit_r14_s_>();
    }
    const two_bit_r14_s_& two_bit_r14() const
    {
      assert_choice_type(types::two_bit_r14, type_, "triggers-r14");
      return c.get<two_bit_r14_s_>();
    }
    const three_bit_r14_s_& three_bit_r14() const
    {
      assert_choice_type(types::three_bit_r14, type_, "triggers-r14");
      return c.get<three_bit_r14_s_>();
    }
    one_bit_r14_s_&   set_one_bit_r14();
    two_bit_r14_s_&   set_two_bit_r14();
    three_bit_r14_s_& set_three_bit_r14();

  private:
    types                                                             type_;
    choice_buffer_t<one_bit_r14_s_, three_bit_r14_s_, two_bit_r14_s_> c;

    void destroy_();
  };

  // member variables
  bool            triggers_r14_present = false;
  triggers_r14_c_ triggers_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_aperiodic_hybrid_r14_s& other) const;
  bool        operator!=(const cqi_report_aperiodic_hybrid_r14_s& other) const { return not(*this == other); }
};

// CQI-ReportBoth-r11 ::= SEQUENCE
struct cqi_report_both_r11_s {
  bool                              csi_im_cfg_to_release_list_r11_present  = false;
  bool                              csi_im_cfg_to_add_mod_list_r11_present  = false;
  bool                              csi_process_to_release_list_r11_present = false;
  bool                              csi_process_to_add_mod_list_r11_present = false;
  csi_im_cfg_to_release_list_r11_l  csi_im_cfg_to_release_list_r11;
  csi_im_cfg_to_add_mod_list_r11_l  csi_im_cfg_to_add_mod_list_r11;
  csi_process_to_release_list_r11_l csi_process_to_release_list_r11;
  csi_process_to_add_mod_list_r11_l csi_process_to_add_mod_list_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_both_r11_s& other) const;
  bool        operator!=(const cqi_report_both_r11_s& other) const { return not(*this == other); }
};

// CQI-ReportBoth-v1250 ::= SEQUENCE
struct cqi_report_both_v1250_s {
  bool                 csi_im_cfg_to_release_list_ext_r12_present = false;
  bool                 csi_im_cfg_to_add_mod_list_ext_r12_present = false;
  uint8_t              csi_im_cfg_to_release_list_ext_r12         = 4;
  csi_im_cfg_ext_r12_s csi_im_cfg_to_add_mod_list_ext_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_both_v1250_s& other) const;
  bool        operator!=(const cqi_report_both_v1250_s& other) const { return not(*this == other); }
};

// CQI-ReportBoth-v1310 ::= SEQUENCE
struct cqi_report_both_v1310_s {
  bool                                 csi_im_cfg_to_release_list_ext_r13_present = false;
  bool                                 csi_im_cfg_to_add_mod_list_ext_r13_present = false;
  csi_im_cfg_to_release_list_ext_r13_l csi_im_cfg_to_release_list_ext_r13;
  csi_im_cfg_to_add_mod_list_ext_r13_l csi_im_cfg_to_add_mod_list_ext_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_both_v1310_s& other) const;
  bool        operator!=(const cqi_report_both_v1310_s& other) const { return not(*this == other); }
};

// CQI-ReportPeriodic-r10 ::= CHOICE
struct cqi_report_periodic_r10_c {
  struct setup_s_ {
    struct cqi_format_ind_periodic_r10_c_ {
      struct wideband_cqi_r10_s_ {
        struct csi_report_mode_r10_opts {
          enum options { submode1, submode2, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<csi_report_mode_r10_opts> csi_report_mode_r10_e_;

        // member variables
        bool                   csi_report_mode_r10_present = false;
        csi_report_mode_r10_e_ csi_report_mode_r10;
      };
      struct subband_cqi_r10_s_ {
        struct periodicity_factor_r10_opts {
          enum options { n2, n4, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<periodicity_factor_r10_opts> periodicity_factor_r10_e_;

        // member variables
        uint8_t                   k = 1;
        periodicity_factor_r10_e_ periodicity_factor_r10;
      };
      struct types_opts {
        enum options { wideband_cqi_r10, subband_cqi_r10, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      cqi_format_ind_periodic_r10_c_() = default;
      cqi_format_ind_periodic_r10_c_(const cqi_format_ind_periodic_r10_c_& other);
      cqi_format_ind_periodic_r10_c_& operator=(const cqi_format_ind_periodic_r10_c_& other);
      ~cqi_format_ind_periodic_r10_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const cqi_format_ind_periodic_r10_c_& other) const;
      bool        operator!=(const cqi_format_ind_periodic_r10_c_& other) const { return not(*this == other); }
      // getters
      wideband_cqi_r10_s_& wideband_cqi_r10()
      {
        assert_choice_type(types::wideband_cqi_r10, type_, "cqi-FormatIndicatorPeriodic-r10");
        return c.get<wideband_cqi_r10_s_>();
      }
      subband_cqi_r10_s_& subband_cqi_r10()
      {
        assert_choice_type(types::subband_cqi_r10, type_, "cqi-FormatIndicatorPeriodic-r10");
        return c.get<subband_cqi_r10_s_>();
      }
      const wideband_cqi_r10_s_& wideband_cqi_r10() const
      {
        assert_choice_type(types::wideband_cqi_r10, type_, "cqi-FormatIndicatorPeriodic-r10");
        return c.get<wideband_cqi_r10_s_>();
      }
      const subband_cqi_r10_s_& subband_cqi_r10() const
      {
        assert_choice_type(types::subband_cqi_r10, type_, "cqi-FormatIndicatorPeriodic-r10");
        return c.get<subband_cqi_r10_s_>();
      }
      wideband_cqi_r10_s_& set_wideband_cqi_r10();
      subband_cqi_r10_s_&  set_subband_cqi_r10();

    private:
      types                                                    type_;
      choice_buffer_t<subband_cqi_r10_s_, wideband_cqi_r10_s_> c;

      void destroy_();
    };
    struct csi_cfg_idx_r10_c_ {
      struct setup_s_ {
        bool     ri_cfg_idx2_r10_present = false;
        uint16_t cqi_pmi_cfg_idx2_r10    = 0;
        uint16_t ri_cfg_idx2_r10         = 0;
      };
      using types = setup_e;

      // choice methods
      csi_cfg_idx_r10_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const csi_cfg_idx_r10_c_& other) const;
      bool        operator!=(const csi_cfg_idx_r10_c_& other) const { return not(*this == other); }
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "csi-ConfigIndex-r10");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "csi-ConfigIndex-r10");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };

    // member variables
    bool                           cqi_pucch_res_idx_p1_r10_present = false;
    bool                           ri_cfg_idx_present               = false;
    bool                           cqi_mask_r9_present              = false;
    bool                           csi_cfg_idx_r10_present          = false;
    uint16_t                       cqi_pucch_res_idx_r10            = 0;
    uint16_t                       cqi_pucch_res_idx_p1_r10         = 0;
    uint16_t                       cqi_pmi_cfg_idx                  = 0;
    cqi_format_ind_periodic_r10_c_ cqi_format_ind_periodic_r10;
    uint16_t                       ri_cfg_idx             = 0;
    bool                           simul_ack_nack_and_cqi = false;
    csi_cfg_idx_r10_c_             csi_cfg_idx_r10;
  };
  using types = setup_e;

  // choice methods
  cqi_report_periodic_r10_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_periodic_r10_c& other) const;
  bool        operator!=(const cqi_report_periodic_r10_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "CQI-ReportPeriodic-r10");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "CQI-ReportPeriodic-r10");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CQI-ReportPeriodic-v1130 ::= SEQUENCE
struct cqi_report_periodic_v1130_s {
  bool                                               simul_ack_nack_and_cqi_format3_r11_present               = false;
  bool                                               cqi_report_periodic_proc_ext_to_release_list_r11_present = false;
  bool                                               cqi_report_periodic_proc_ext_to_add_mod_list_r11_present = false;
  cqi_report_periodic_proc_ext_to_release_list_r11_l cqi_report_periodic_proc_ext_to_release_list_r11;
  cqi_report_periodic_proc_ext_to_add_mod_list_r11_l cqi_report_periodic_proc_ext_to_add_mod_list_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_periodic_v1130_s& other) const;
  bool        operator!=(const cqi_report_periodic_v1130_s& other) const { return not(*this == other); }
};

// CQI-ReportPeriodic-v1310 ::= SEQUENCE
struct cqi_report_periodic_v1310_s {
  bool                 cri_report_cfg_r13_present                         = false;
  bool                 simul_ack_nack_and_cqi_format4_format5_r13_present = false;
  cri_report_cfg_r13_c cri_report_cfg_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_periodic_v1310_s& other) const;
  bool        operator!=(const cqi_report_periodic_v1310_s& other) const { return not(*this == other); }
};

// CQI-ReportPeriodic-v1320 ::= SEQUENCE
struct cqi_report_periodic_v1320_s {
  struct periodicity_factor_wb_r13_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<periodicity_factor_wb_r13_opts> periodicity_factor_wb_r13_e_;

  // member variables
  bool                         periodicity_factor_wb_r13_present = false;
  periodicity_factor_wb_r13_e_ periodicity_factor_wb_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_periodic_v1320_s& other) const;
  bool        operator!=(const cqi_report_periodic_v1320_s& other) const { return not(*this == other); }
};

// SPDCCH-Elements-r15 ::= CHOICE
struct spdcch_elems_r15_c {
  struct setup_s_ {
    struct spdcch_set_ref_sig_r15_opts {
      enum options { crs, dmrs, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<spdcch_set_ref_sig_r15_opts> spdcch_set_ref_sig_r15_e_;
    struct tx_type_r15_opts {
      enum options { localised, distributed, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<tx_type_r15_opts> tx_type_r15_e_;
    using dci7_candidates_per_al_pdcch_r15_l_      = bounded_array<uint8_t, 4>;
    using dci7_candidate_sets_per_al_spdcch_r15_l_ = dyn_array<dci7_candidates_per_al_spdcch_r15_l>;
    struct res_block_assign_r15_s_ {
      uint8_t             num_rb_in_freq_domain_r15 = 2;
      fixed_bitstring<98> res_block_assign_r15;
    };
    using al_start_point_spdcch_r15_l_ = bounded_array<uint8_t, 4>;
    struct sf_type_r15_opts {
      enum options { mbsfn, nonmbsfn, all, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<sf_type_r15_opts> sf_type_r15_e_;
    struct rate_matching_mode_r15_opts {
      enum options { m1, m2, m3, m4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<rate_matching_mode_r15_opts> rate_matching_mode_r15_e_;

    // member variables
    bool                                     ext                                           = false;
    bool                                     spdcch_set_cfg_id_r15_present                 = false;
    bool                                     spdcch_set_ref_sig_r15_present                = false;
    bool                                     tx_type_r15_present                           = false;
    bool                                     spdcch_no_of_symbols_r15_present              = false;
    bool                                     dmrs_scrambling_seq_int_r15_present           = false;
    bool                                     dci7_candidates_per_al_pdcch_r15_present      = false;
    bool                                     dci7_candidate_sets_per_al_spdcch_r15_present = false;
    bool                                     res_block_assign_r15_present                  = false;
    bool                                     subslot_applicability_r15_present             = false;
    bool                                     al_start_point_spdcch_r15_present             = false;
    bool                                     sf_type_r15_present                           = false;
    bool                                     rate_matching_mode_r15_present                = false;
    uint8_t                                  spdcch_set_cfg_id_r15                         = 0;
    spdcch_set_ref_sig_r15_e_                spdcch_set_ref_sig_r15;
    tx_type_r15_e_                           tx_type_r15;
    uint8_t                                  spdcch_no_of_symbols_r15    = 1;
    uint16_t                                 dmrs_scrambling_seq_int_r15 = 0;
    dci7_candidates_per_al_pdcch_r15_l_      dci7_candidates_per_al_pdcch_r15;
    dci7_candidate_sets_per_al_spdcch_r15_l_ dci7_candidate_sets_per_al_spdcch_r15;
    res_block_assign_r15_s_                  res_block_assign_r15;
    fixed_bitstring<5>                       subslot_applicability_r15;
    al_start_point_spdcch_r15_l_             al_start_point_spdcch_r15;
    sf_type_r15_e_                           sf_type_r15;
    rate_matching_mode_r15_e_                rate_matching_mode_r15;
    // ...
  };
  using types = setup_e;

  // choice methods
  spdcch_elems_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const spdcch_elems_r15_c& other) const;
  bool        operator!=(const spdcch_elems_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SPDCCH-Elements-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SPDCCH-Elements-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SPUCCH-Elements-r15 ::= CHOICE
struct spucch_elems_r15_c {
  struct setup_s_ {
    using n1_subslot_spucch_an_list_r15_l_ = bounded_array<uint16_t, 4>;
    using n4_spucch_slot_res_r15_l_        = dyn_array<n4_spucch_res_r15_s>;
    using n4_spucch_subslot_res_r15_l_     = dyn_array<n4_spucch_res_r15_s>;

    // member variables
    bool                             n1_subslot_spucch_an_list_r15_present              = false;
    bool                             n1_slot_spucch_fh_an_list_r15_present              = false;
    bool                             n1_slot_spucch_no_fh_an_list_r15_present           = false;
    bool                             n3_spucch_an_list_r15_present                      = false;
    bool                             n4_spucch_slot_res_r15_present                     = false;
    bool                             n4_spucch_subslot_res_r15_present                  = false;
    bool                             n4max_coderate_slot_pucch_r15_present              = false;
    bool                             n4max_coderate_subslot_pucch_r15_present           = false;
    bool                             n4max_coderate_multi_res_slot_pucch_r15_present    = false;
    bool                             n4max_coderate_multi_res_subslot_pucch_r15_present = false;
    n1_subslot_spucch_an_list_r15_l_ n1_subslot_spucch_an_list_r15;
    uint16_t                         n1_slot_spucch_fh_an_list_r15    = 0;
    uint16_t                         n1_slot_spucch_no_fh_an_list_r15 = 0;
    uint16_t                         n3_spucch_an_list_r15            = 0;
    n4_spucch_slot_res_r15_l_        n4_spucch_slot_res_r15;
    n4_spucch_subslot_res_r15_l_     n4_spucch_subslot_res_r15;
    uint8_t                          n4max_coderate_slot_pucch_r15              = 0;
    uint8_t                          n4max_coderate_subslot_pucch_r15           = 0;
    uint8_t                          n4max_coderate_multi_res_slot_pucch_r15    = 0;
    uint8_t                          n4max_coderate_multi_res_subslot_pucch_r15 = 0;
  };
  using types = setup_e;

  // choice methods
  spucch_elems_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const spucch_elems_r15_c& other) const;
  bool        operator!=(const spucch_elems_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SPUCCH-Elements-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SPUCCH-Elements-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// TPC-Index ::= CHOICE
struct tpc_idx_c {
  struct types_opts {
    enum options { idx_of_format3, idx_of_format3_a, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  tpc_idx_c() = default;
  tpc_idx_c(const tpc_idx_c& other);
  tpc_idx_c& operator=(const tpc_idx_c& other);
  ~tpc_idx_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const tpc_idx_c& other) const;
  bool        operator!=(const tpc_idx_c& other) const { return not(*this == other); }
  // getters
  uint8_t& idx_of_format3()
  {
    assert_choice_type(types::idx_of_format3, type_, "TPC-Index");
    return c.get<uint8_t>();
  }
  uint8_t& idx_of_format3_a()
  {
    assert_choice_type(types::idx_of_format3_a, type_, "TPC-Index");
    return c.get<uint8_t>();
  }
  const uint8_t& idx_of_format3() const
  {
    assert_choice_type(types::idx_of_format3, type_, "TPC-Index");
    return c.get<uint8_t>();
  }
  const uint8_t& idx_of_format3_a() const
  {
    assert_choice_type(types::idx_of_format3_a, type_, "TPC-Index");
    return c.get<uint8_t>();
  }
  uint8_t& set_idx_of_format3();
  uint8_t& set_idx_of_format3_a();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// ZeroTxPowerCSI-RS-Conf-r12 ::= CHOICE
struct zero_tx_pwr_csi_rs_conf_r12_c {
  using types = setup_e;

  // choice methods
  zero_tx_pwr_csi_rs_conf_r12_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const zero_tx_pwr_csi_rs_conf_r12_c& other) const;
  bool        operator!=(const zero_tx_pwr_csi_rs_conf_r12_c& other) const { return not(*this == other); }
  // getters
  zero_tx_pwr_csi_rs_r12_s& setup()
  {
    assert_choice_type(types::setup, type_, "ZeroTxPowerCSI-RS-Conf-r12");
    return c;
  }
  const zero_tx_pwr_csi_rs_r12_s& setup() const
  {
    assert_choice_type(types::setup, type_, "ZeroTxPowerCSI-RS-Conf-r12");
    return c;
  }
  void                      set_release();
  zero_tx_pwr_csi_rs_r12_s& set_setup();

private:
  types                    type_;
  zero_tx_pwr_csi_rs_r12_s c;
};

// CQI-ReportConfig-r10 ::= SEQUENCE
struct cqi_report_cfg_r10_s {
  struct csi_sf_pattern_cfg_r10_c_ {
    struct setup_s_ {
      meas_sf_pattern_r10_c csi_meas_sf_set1_r10;
      meas_sf_pattern_r10_c csi_meas_sf_set2_r10;
    };
    using types = setup_e;

    // choice methods
    csi_sf_pattern_cfg_r10_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const csi_sf_pattern_cfg_r10_c_& other) const;
    bool        operator!=(const csi_sf_pattern_cfg_r10_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "csi-SubframePatternConfig-r10");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "csi-SubframePatternConfig-r10");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                       cqi_report_aperiodic_r10_present = false;
  bool                       cqi_report_periodic_r10_present  = false;
  bool                       pmi_ri_report_r9_present         = false;
  bool                       csi_sf_pattern_cfg_r10_present   = false;
  cqi_report_aperiodic_r10_c cqi_report_aperiodic_r10;
  int8_t                     nom_pdsch_rs_epre_offset = -1;
  cqi_report_periodic_r10_c  cqi_report_periodic_r10;
  csi_sf_pattern_cfg_r10_c_  csi_sf_pattern_cfg_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_cfg_r10_s& other) const;
  bool        operator!=(const cqi_report_cfg_r10_s& other) const { return not(*this == other); }
};

// CQI-ReportConfig-v1130 ::= SEQUENCE
struct cqi_report_cfg_v1130_s {
  cqi_report_periodic_v1130_s cqi_report_periodic_v1130;
  cqi_report_both_r11_s       cqi_report_both_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_cfg_v1130_s& other) const;
  bool        operator!=(const cqi_report_cfg_v1130_s& other) const { return not(*this == other); }
};

// CQI-ReportConfig-v1250 ::= SEQUENCE
struct cqi_report_cfg_v1250_s {
  struct csi_sf_pattern_cfg_r12_c_ {
    struct setup_s_ {
      fixed_bitstring<10> csi_meas_sf_sets_r12;
    };
    using types = setup_e;

    // choice methods
    csi_sf_pattern_cfg_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const csi_sf_pattern_cfg_r12_c_& other) const;
    bool        operator!=(const csi_sf_pattern_cfg_r12_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "csi-SubframePatternConfig-r12");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "csi-SubframePatternConfig-r12");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct alt_cqi_table_r12_opts {
    enum options { all_sfs, csi_sf_set1, csi_sf_set2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<alt_cqi_table_r12_opts> alt_cqi_table_r12_e_;

  // member variables
  bool                         csi_sf_pattern_cfg_r12_present     = false;
  bool                         cqi_report_both_v1250_present      = false;
  bool                         cqi_report_aperiodic_v1250_present = false;
  bool                         alt_cqi_table_r12_present          = false;
  csi_sf_pattern_cfg_r12_c_    csi_sf_pattern_cfg_r12;
  cqi_report_both_v1250_s      cqi_report_both_v1250;
  cqi_report_aperiodic_v1250_c cqi_report_aperiodic_v1250;
  alt_cqi_table_r12_e_         alt_cqi_table_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_cfg_v1250_s& other) const;
  bool        operator!=(const cqi_report_cfg_v1250_s& other) const { return not(*this == other); }
};

// CQI-ReportConfig-v1310 ::= SEQUENCE
struct cqi_report_cfg_v1310_s {
  bool                         cqi_report_both_v1310_present      = false;
  bool                         cqi_report_aperiodic_v1310_present = false;
  bool                         cqi_report_periodic_v1310_present  = false;
  cqi_report_both_v1310_s      cqi_report_both_v1310;
  cqi_report_aperiodic_v1310_c cqi_report_aperiodic_v1310;
  cqi_report_periodic_v1310_s  cqi_report_periodic_v1310;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_cfg_v1310_s& other) const;
  bool        operator!=(const cqi_report_cfg_v1310_s& other) const { return not(*this == other); }
};

// CQI-ReportConfig-v1320 ::= SEQUENCE
struct cqi_report_cfg_v1320_s {
  bool                        cqi_report_periodic_v1320_present = false;
  cqi_report_periodic_v1320_s cqi_report_periodic_v1320;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_cfg_v1320_s& other) const;
  bool        operator!=(const cqi_report_cfg_v1320_s& other) const { return not(*this == other); }
};

// CQI-ReportConfig-v1430 ::= SEQUENCE
struct cqi_report_cfg_v1430_s {
  bool                              cqi_report_aperiodic_hybrid_r14_present = false;
  cqi_report_aperiodic_hybrid_r14_s cqi_report_aperiodic_hybrid_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_cfg_v1430_s& other) const;
  bool        operator!=(const cqi_report_cfg_v1430_s& other) const { return not(*this == other); }
};

// CSI-RS-Config-r10 ::= SEQUENCE
struct csi_rs_cfg_r10_s {
  struct csi_rs_r10_c_ {
    struct setup_s_ {
      struct ant_ports_count_r10_opts {
        enum options { an1, an2, an4, an8, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<ant_ports_count_r10_opts> ant_ports_count_r10_e_;

      // member variables
      ant_ports_count_r10_e_ ant_ports_count_r10;
      uint8_t                res_cfg_r10 = 0;
      uint8_t                sf_cfg_r10  = 0;
      int8_t                 p_c_r10     = -8;
    };
    using types = setup_e;

    // choice methods
    csi_rs_r10_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const csi_rs_r10_c_& other) const;
    bool        operator!=(const csi_rs_r10_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "csi-RS-r10");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "csi-RS-r10");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                          csi_rs_r10_present             = false;
  bool                          zero_tx_pwr_csi_rs_r10_present = false;
  csi_rs_r10_c_                 csi_rs_r10;
  zero_tx_pwr_csi_rs_conf_r12_c zero_tx_pwr_csi_rs_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_r10_s& other) const;
  bool        operator!=(const csi_rs_cfg_r10_s& other) const { return not(*this == other); }
};

// CSI-RS-Config-v1250 ::= SEQUENCE
struct csi_rs_cfg_v1250_s {
  struct ds_zero_tx_pwr_csi_rs_r12_c_ {
    struct setup_s_ {
      using zero_tx_pwr_csi_rs_list_r12_l_ = dyn_array<zero_tx_pwr_csi_rs_r12_s>;

      // member variables
      zero_tx_pwr_csi_rs_list_r12_l_ zero_tx_pwr_csi_rs_list_r12;
    };
    using types = setup_e;

    // choice methods
    ds_zero_tx_pwr_csi_rs_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const ds_zero_tx_pwr_csi_rs_r12_c_& other) const;
    bool        operator!=(const ds_zero_tx_pwr_csi_rs_r12_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "ds-ZeroTxPowerCSI-RS-r12");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "ds-ZeroTxPowerCSI-RS-r12");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                          zero_tx_pwr_csi_rs2_r12_present   = false;
  bool                          ds_zero_tx_pwr_csi_rs_r12_present = false;
  zero_tx_pwr_csi_rs_conf_r12_c zero_tx_pwr_csi_rs2_r12;
  ds_zero_tx_pwr_csi_rs_r12_c_  ds_zero_tx_pwr_csi_rs_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_v1250_s& other) const;
  bool        operator!=(const csi_rs_cfg_v1250_s& other) const { return not(*this == other); }
};

// CSI-RS-Config-v1310 ::= SEQUENCE
struct csi_rs_cfg_v1310_s {
  bool                   emimo_type_r13_present = false;
  csi_rs_cfg_emimo_r13_c emimo_type_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_v1310_s& other) const;
  bool        operator!=(const csi_rs_cfg_v1310_s& other) const { return not(*this == other); }
};

// CSI-RS-Config-v1430 ::= SEQUENCE
struct csi_rs_cfg_v1430_s {
  bool                          dummy_present                         = false;
  bool                          emimo_hybrid_r14_present              = false;
  bool                          advanced_codebook_enabled_r14_present = false;
  csi_rs_cfg_emimo_v1430_c      dummy;
  csi_rs_cfg_emimo_hybrid_r14_c emimo_hybrid_r14;
  bool                          advanced_codebook_enabled_r14 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_v1430_s& other) const;
  bool        operator!=(const csi_rs_cfg_v1430_s& other) const { return not(*this == other); }
};

// CSI-RS-ConfigZP-r11 ::= SEQUENCE
struct csi_rs_cfg_zp_r11_s {
  bool                ext                  = false;
  uint8_t             csi_rs_cfg_zp_id_r11 = 1;
  fixed_bitstring<16> res_cfg_list_r11;
  uint8_t             sf_cfg_r11 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_zp_r11_s& other) const;
  bool        operator!=(const csi_rs_cfg_zp_r11_s& other) const { return not(*this == other); }
};

// DeltaTxD-OffsetListSPUCCH-r15 ::= SEQUENCE
struct delta_tx_d_offset_list_spucch_r15_s {
  struct delta_tx_d_offset_spucch_format1_r15_opts {
    enum options { db0, db_minus2, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_tx_d_offset_spucch_format1_r15_opts> delta_tx_d_offset_spucch_format1_r15_e_;
  struct delta_tx_d_offset_spucch_format1a_r15_opts {
    enum options { db0, db_minus2, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_tx_d_offset_spucch_format1a_r15_opts> delta_tx_d_offset_spucch_format1a_r15_e_;
  struct delta_tx_d_offset_spucch_format1b_r15_opts {
    enum options { db0, db_minus2, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_tx_d_offset_spucch_format1b_r15_opts> delta_tx_d_offset_spucch_format1b_r15_e_;
  struct delta_tx_d_offset_spucch_format3_r15_opts {
    enum options { db0, db_minus2, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_tx_d_offset_spucch_format3_r15_opts> delta_tx_d_offset_spucch_format3_r15_e_;

  // member variables
  bool                                     ext = false;
  delta_tx_d_offset_spucch_format1_r15_e_  delta_tx_d_offset_spucch_format1_r15;
  delta_tx_d_offset_spucch_format1a_r15_e_ delta_tx_d_offset_spucch_format1a_r15;
  delta_tx_d_offset_spucch_format1b_r15_e_ delta_tx_d_offset_spucch_format1b_r15;
  delta_tx_d_offset_spucch_format3_r15_e_  delta_tx_d_offset_spucch_format3_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const delta_tx_d_offset_list_spucch_r15_s& other) const;
  bool        operator!=(const delta_tx_d_offset_list_spucch_r15_s& other) const { return not(*this == other); }
};

// EPDCCH-SetConfig-r11 ::= SEQUENCE
struct epdcch_set_cfg_r11_s {
  struct tx_type_r11_opts {
    enum options { localised, distributed, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<tx_type_r11_opts> tx_type_r11_e_;
  struct res_block_assign_r11_s_ {
    struct num_prb_pairs_r11_opts {
      enum options { n2, n4, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<num_prb_pairs_r11_opts> num_prb_pairs_r11_e_;

    // member variables
    num_prb_pairs_r11_e_     num_prb_pairs_r11;
    bounded_bitstring<4, 38> res_block_assign_r11;
  };
  struct csi_rs_cfg_zp_id2_r12_c_ {
    using types = setup_e;

    // choice methods
    csi_rs_cfg_zp_id2_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const csi_rs_cfg_zp_id2_r12_c_& other) const;
    bool        operator!=(const csi_rs_cfg_zp_id2_r12_c_& other) const { return not(*this == other); }
    // getters
    uint8_t& setup()
    {
      assert_choice_type(types::setup, type_, "csi-RS-ConfigZPId2-r12");
      return c;
    }
    const uint8_t& setup() const
    {
      assert_choice_type(types::setup, type_, "csi-RS-ConfigZPId2-r12");
      return c;
    }
    void     set_release();
    uint8_t& set_setup();

  private:
    types   type_;
    uint8_t c;
  };
  struct num_prb_pairs_v1310_c_ {
    using types = setup_e;

    // choice methods
    num_prb_pairs_v1310_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const num_prb_pairs_v1310_c_& other) const;
    bool        operator!=(const num_prb_pairs_v1310_c_& other) const { return not(*this == other); }
    // getters
    void set_release();
    void set_setup();

  private:
    types type_;
  };
  struct mpdcch_cfg_r13_c_ {
    struct setup_s_ {
      struct csi_num_repeat_ce_r13_opts {
        enum options { sf1, sf2, sf4, sf8, sf16, sf32, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<csi_num_repeat_ce_r13_opts> csi_num_repeat_ce_r13_e_;
      struct mpdcch_pdsch_hop_cfg_r13_opts {
        enum options { on, off, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<mpdcch_pdsch_hop_cfg_r13_opts> mpdcch_pdsch_hop_cfg_r13_e_;
      struct mpdcch_start_sf_uess_r13_c_ {
        struct fdd_r13_opts {
          enum options { v1, v1dot5, v2, v2dot5, v4, v5, v8, v10, nulltype } value;
          typedef float number_type;

          const char* to_string() const;
          float       to_number() const;
          const char* to_number_string() const;
        };
        typedef enumerated<fdd_r13_opts> fdd_r13_e_;
        struct tdd_r13_opts {
          enum options { v1, v2, v4, v5, v8, v10, v20, spare1, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<tdd_r13_opts> tdd_r13_e_;
        struct types_opts {
          enum options { fdd_r13, tdd_r13, nulltype } value;

          const char* to_string() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        mpdcch_start_sf_uess_r13_c_() = default;
        mpdcch_start_sf_uess_r13_c_(const mpdcch_start_sf_uess_r13_c_& other);
        mpdcch_start_sf_uess_r13_c_& operator=(const mpdcch_start_sf_uess_r13_c_& other);
        ~mpdcch_start_sf_uess_r13_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        bool        operator==(const mpdcch_start_sf_uess_r13_c_& other) const;
        bool        operator!=(const mpdcch_start_sf_uess_r13_c_& other) const { return not(*this == other); }
        // getters
        fdd_r13_e_& fdd_r13()
        {
          assert_choice_type(types::fdd_r13, type_, "mpdcch-StartSF-UESS-r13");
          return c.get<fdd_r13_e_>();
        }
        tdd_r13_e_& tdd_r13()
        {
          assert_choice_type(types::tdd_r13, type_, "mpdcch-StartSF-UESS-r13");
          return c.get<tdd_r13_e_>();
        }
        const fdd_r13_e_& fdd_r13() const
        {
          assert_choice_type(types::fdd_r13, type_, "mpdcch-StartSF-UESS-r13");
          return c.get<fdd_r13_e_>();
        }
        const tdd_r13_e_& tdd_r13() const
        {
          assert_choice_type(types::tdd_r13, type_, "mpdcch-StartSF-UESS-r13");
          return c.get<tdd_r13_e_>();
        }
        fdd_r13_e_& set_fdd_r13();
        tdd_r13_e_& set_tdd_r13();

      private:
        types               type_;
        pod_choice_buffer_t c;

        void destroy_();
      };
      struct mpdcch_num_repeat_r13_opts {
        enum options { r1, r2, r4, r8, r16, r32, r64, r128, r256, nulltype } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<mpdcch_num_repeat_r13_opts> mpdcch_num_repeat_r13_e_;

      // member variables
      csi_num_repeat_ce_r13_e_    csi_num_repeat_ce_r13;
      mpdcch_pdsch_hop_cfg_r13_e_ mpdcch_pdsch_hop_cfg_r13;
      mpdcch_start_sf_uess_r13_c_ mpdcch_start_sf_uess_r13;
      mpdcch_num_repeat_r13_e_    mpdcch_num_repeat_r13;
      uint8_t                     mpdcch_nb_r13 = 1;
    };
    using types = setup_e;

    // choice methods
    mpdcch_cfg_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const mpdcch_cfg_r13_c_& other) const;
    bool        operator!=(const mpdcch_cfg_r13_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "mpdcch-config-r13");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "mpdcch-config-r13");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                    ext                           = false;
  bool                    re_map_qcl_cfg_id_r11_present = false;
  uint8_t                 set_cfg_id_r11                = 0;
  tx_type_r11_e_          tx_type_r11;
  res_block_assign_r11_s_ res_block_assign_r11;
  uint16_t                dmrs_scrambling_seq_int_r11 = 0;
  uint16_t                pucch_res_start_offset_r11  = 0;
  uint8_t                 re_map_qcl_cfg_id_r11       = 1;
  // ...
  // group 0
  copy_ptr<csi_rs_cfg_zp_id2_r12_c_> csi_rs_cfg_zp_id2_r12;
  // group 1
  copy_ptr<num_prb_pairs_v1310_c_> num_prb_pairs_v1310;
  copy_ptr<mpdcch_cfg_r13_c_>      mpdcch_cfg_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const epdcch_set_cfg_r11_s& other) const;
  bool        operator!=(const epdcch_set_cfg_r11_s& other) const { return not(*this == other); }
};

// Enable256QAM-r14 ::= CHOICE
struct enable256_qam_r14_c {
  struct setup_c_ {
    struct tpc_sf_set_cfgured_r14_s_ {
      bool sf_set1_dci_format0_r14 = false;
      bool sf_set1_dci_format4_r14 = false;
      bool sf_set2_dci_format0_r14 = false;
      bool sf_set2_dci_format4_r14 = false;
    };
    struct tpc_sf_set_not_cfgured_r14_s_ {
      bool dci_format0_r14 = false;
      bool dci_format4_r14 = false;
    };
    struct types_opts {
      enum options { tpc_sf_set_cfgured_r14, tpc_sf_set_not_cfgured_r14, nulltype } value;

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
    bool        operator==(const setup_c_& other) const;
    bool        operator!=(const setup_c_& other) const { return not(*this == other); }
    // getters
    tpc_sf_set_cfgured_r14_s_& tpc_sf_set_cfgured_r14()
    {
      assert_choice_type(types::tpc_sf_set_cfgured_r14, type_, "setup");
      return c.get<tpc_sf_set_cfgured_r14_s_>();
    }
    tpc_sf_set_not_cfgured_r14_s_& tpc_sf_set_not_cfgured_r14()
    {
      assert_choice_type(types::tpc_sf_set_not_cfgured_r14, type_, "setup");
      return c.get<tpc_sf_set_not_cfgured_r14_s_>();
    }
    const tpc_sf_set_cfgured_r14_s_& tpc_sf_set_cfgured_r14() const
    {
      assert_choice_type(types::tpc_sf_set_cfgured_r14, type_, "setup");
      return c.get<tpc_sf_set_cfgured_r14_s_>();
    }
    const tpc_sf_set_not_cfgured_r14_s_& tpc_sf_set_not_cfgured_r14() const
    {
      assert_choice_type(types::tpc_sf_set_not_cfgured_r14, type_, "setup");
      return c.get<tpc_sf_set_not_cfgured_r14_s_>();
    }
    tpc_sf_set_cfgured_r14_s_&     set_tpc_sf_set_cfgured_r14();
    tpc_sf_set_not_cfgured_r14_s_& set_tpc_sf_set_not_cfgured_r14();

  private:
    types                                                                     type_;
    choice_buffer_t<tpc_sf_set_cfgured_r14_s_, tpc_sf_set_not_cfgured_r14_s_> c;

    void destroy_();
  };
  using types = setup_e;

  // choice methods
  enable256_qam_r14_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const enable256_qam_r14_c& other) const;
  bool        operator!=(const enable256_qam_r14_c& other) const { return not(*this == other); }
  // getters
  setup_c_& setup()
  {
    assert_choice_type(types::setup, type_, "Enable256QAM-r14");
    return c;
  }
  const setup_c_& setup() const
  {
    assert_choice_type(types::setup, type_, "Enable256QAM-r14");
    return c;
  }
  void      set_release();
  setup_c_& set_setup();

private:
  types    type_;
  setup_c_ c;
};

// PDSCH-RE-MappingQCL-Config-r11 ::= SEQUENCE
struct pdsch_re_map_qcl_cfg_r11_s {
  struct optional_set_of_fields_r11_s_ {
    struct crs_ports_count_r11_opts {
      enum options { n1, n2, n4, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<crs_ports_count_r11_opts> crs_ports_count_r11_e_;
    struct mbsfn_sf_cfg_list_r11_c_ {
      struct setup_s_ {
        mbsfn_sf_cfg_list_l sf_cfg_list;
      };
      using types = setup_e;

      // choice methods
      mbsfn_sf_cfg_list_r11_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const mbsfn_sf_cfg_list_r11_c_& other) const;
      bool        operator!=(const mbsfn_sf_cfg_list_r11_c_& other) const { return not(*this == other); }
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "mbsfn-SubframeConfigList-r11");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "mbsfn-SubframeConfigList-r11");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };
    struct pdsch_start_r11_opts {
      enum options { reserved, n1, n2, n3, n4, assigned, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<pdsch_start_r11_opts> pdsch_start_r11_e_;

    // member variables
    bool                     mbsfn_sf_cfg_list_r11_present = false;
    crs_ports_count_r11_e_   crs_ports_count_r11;
    uint8_t                  crs_freq_shift_r11 = 0;
    mbsfn_sf_cfg_list_r11_c_ mbsfn_sf_cfg_list_r11;
    pdsch_start_r11_e_       pdsch_start_r11;
  };
  struct mbsfn_sf_cfg_list_v1430_c_ {
    struct setup_s_ {
      mbsfn_sf_cfg_list_v1430_l sf_cfg_list_v1430;
    };
    using types = setup_e;

    // choice methods
    mbsfn_sf_cfg_list_v1430_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const mbsfn_sf_cfg_list_v1430_c_& other) const;
    bool        operator!=(const mbsfn_sf_cfg_list_v1430_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "mbsfn-SubframeConfigList-v1430");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "mbsfn-SubframeConfigList-v1430");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct codeword_one_cfg_v1530_c_ {
    struct setup_s_ {
      struct crs_ports_count_v1530_opts {
        enum options { n1, n2, n4, spare1, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<crs_ports_count_v1530_opts> crs_ports_count_v1530_e_;
      struct pdsch_start_v1530_opts {
        enum options { reserved, n1, n2, n3, n4, assigned, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<pdsch_start_v1530_opts> pdsch_start_v1530_e_;

      // member variables
      bool                      mbsfn_sf_cfg_list_v1530_present     = false;
      bool                      mbsfn_sf_cfg_list_ext_v1530_present = false;
      bool                      qcl_csi_rs_cfg_nzp_id_v1530_present = false;
      crs_ports_count_v1530_e_  crs_ports_count_v1530;
      uint8_t                   crs_freq_shift_v1530 = 0;
      mbsfn_sf_cfg_list_l       mbsfn_sf_cfg_list_v1530;
      mbsfn_sf_cfg_list_v1430_l mbsfn_sf_cfg_list_ext_v1530;
      pdsch_start_v1530_e_      pdsch_start_v1530;
      uint8_t                   csi_rs_cfg_zp_id_v1530      = 1;
      uint8_t                   qcl_csi_rs_cfg_nzp_id_v1530 = 1;
    };
    using types = setup_e;

    // choice methods
    codeword_one_cfg_v1530_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const codeword_one_cfg_v1530_c_& other) const;
    bool        operator!=(const codeword_one_cfg_v1530_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "codewordOneConfig-v1530");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "codewordOneConfig-v1530");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                          ext                                = false;
  bool                          optional_set_of_fields_r11_present = false;
  bool                          qcl_csi_rs_cfg_nzp_id_r11_present  = false;
  uint8_t                       pdsch_re_map_qcl_cfg_id_r11        = 1;
  optional_set_of_fields_r11_s_ optional_set_of_fields_r11;
  uint8_t                       csi_rs_cfg_zp_id_r11      = 1;
  uint8_t                       qcl_csi_rs_cfg_nzp_id_r11 = 1;
  // ...
  // group 0
  copy_ptr<mbsfn_sf_cfg_list_v1430_c_> mbsfn_sf_cfg_list_v1430;
  // group 1
  copy_ptr<codeword_one_cfg_v1530_c_> codeword_one_cfg_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdsch_re_map_qcl_cfg_r11_s& other) const;
  bool        operator!=(const pdsch_re_map_qcl_cfg_r11_s& other) const { return not(*this == other); }
};

// SPDCCH-Set-r15 ::= SEQUENCE (SIZE (1..4)) OF SPDCCH-Elements-r15
using spdcch_set_r15_l = dyn_array<spdcch_elems_r15_c>;

// SPUCCH-Set-r15 ::= SEQUENCE (SIZE (1..4)) OF SPUCCH-Elements-r15
using spucch_set_r15_l = dyn_array<spucch_elems_r15_c>;

// SR-SubslotSPUCCH-ResourceList-r15 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (0..1319)
using sr_subslot_spucch_res_list_r15_l = bounded_array<uint16_t, 4>;

// SRS-AntennaPort ::= ENUMERATED
struct srs_ant_port_opts {
  enum options { an1, an2, an4, spare1, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<srs_ant_port_opts> srs_ant_port_e;

// ShortTTI-Length-r15 ::= ENUMERATED
struct short_tti_len_r15_opts {
  enum options { slot, subslot, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<short_tti_len_r15_opts> short_tti_len_r15_e;

// TPC-PDCCH-Config ::= CHOICE
struct tpc_pdcch_cfg_c {
  struct setup_s_ {
    fixed_bitstring<16> tpc_rnti;
    tpc_idx_c           tpc_idx;
  };
  using types = setup_e;

  // choice methods
  tpc_pdcch_cfg_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const tpc_pdcch_cfg_c& other) const;
  bool        operator!=(const tpc_pdcch_cfg_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "TPC-PDCCH-Config");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "TPC-PDCCH-Config");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// AntennaInfoDedicatedSTTI-r15 ::= CHOICE
struct ant_info_ded_stti_r15_c {
  struct setup_s_ {
    struct tx_mode_dl_mbsfn_r15_opts {
      enum options { tm9, tm10, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<tx_mode_dl_mbsfn_r15_opts> tx_mode_dl_mbsfn_r15_e_;
    struct tx_mode_dl_non_mbsfn_r15_opts {
      enum options { tm1, tm2, tm3, tm4, tm6, tm8, tm9, tm10, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<tx_mode_dl_non_mbsfn_r15_opts> tx_mode_dl_non_mbsfn_r15_e_;
    struct codebook_subset_restrict_c_ {
      struct types_opts {
        enum options {
          n2_tx_ant_tm3_r15,
          n4_tx_ant_tm3_r15,
          n2_tx_ant_tm4_r15,
          n4_tx_ant_tm4_r15,
          n2_tx_ant_tm5_r15,
          n4_tx_ant_tm5_r15,
          n2_tx_ant_tm6_r15,
          n4_tx_ant_tm6_r15,
          n2_tx_ant_tm8_r15,
          n4_tx_ant_tm8_r15,
          n2_tx_ant_tm9and10_r15,
          n4_tx_ant_tm9and10_r15,
          n8_tx_ant_tm9and10_r15,
          nulltype
        } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      codebook_subset_restrict_c_() = default;
      codebook_subset_restrict_c_(const codebook_subset_restrict_c_& other);
      codebook_subset_restrict_c_& operator=(const codebook_subset_restrict_c_& other);
      ~codebook_subset_restrict_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const codebook_subset_restrict_c_& other) const;
      bool        operator!=(const codebook_subset_restrict_c_& other) const { return not(*this == other); }
      // getters
      fixed_bitstring<2>& n2_tx_ant_tm3_r15()
      {
        assert_choice_type(types::n2_tx_ant_tm3_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<2> >();
      }
      fixed_bitstring<4>& n4_tx_ant_tm3_r15()
      {
        assert_choice_type(types::n4_tx_ant_tm3_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<4> >();
      }
      fixed_bitstring<6>& n2_tx_ant_tm4_r15()
      {
        assert_choice_type(types::n2_tx_ant_tm4_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<6> >();
      }
      fixed_bitstring<64>& n4_tx_ant_tm4_r15()
      {
        assert_choice_type(types::n4_tx_ant_tm4_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<64> >();
      }
      fixed_bitstring<4>& n2_tx_ant_tm5_r15()
      {
        assert_choice_type(types::n2_tx_ant_tm5_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<4> >();
      }
      fixed_bitstring<16>& n4_tx_ant_tm5_r15()
      {
        assert_choice_type(types::n4_tx_ant_tm5_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<16> >();
      }
      fixed_bitstring<4>& n2_tx_ant_tm6_r15()
      {
        assert_choice_type(types::n2_tx_ant_tm6_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<4> >();
      }
      fixed_bitstring<16>& n4_tx_ant_tm6_r15()
      {
        assert_choice_type(types::n4_tx_ant_tm6_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<16> >();
      }
      fixed_bitstring<6>& n2_tx_ant_tm8_r15()
      {
        assert_choice_type(types::n2_tx_ant_tm8_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<6> >();
      }
      fixed_bitstring<64>& n4_tx_ant_tm8_r15()
      {
        assert_choice_type(types::n4_tx_ant_tm8_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<64> >();
      }
      fixed_bitstring<6>& n2_tx_ant_tm9and10_r15()
      {
        assert_choice_type(types::n2_tx_ant_tm9and10_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<6> >();
      }
      fixed_bitstring<96>& n4_tx_ant_tm9and10_r15()
      {
        assert_choice_type(types::n4_tx_ant_tm9and10_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<96> >();
      }
      fixed_bitstring<109>& n8_tx_ant_tm9and10_r15()
      {
        assert_choice_type(types::n8_tx_ant_tm9and10_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<109> >();
      }
      const fixed_bitstring<2>& n2_tx_ant_tm3_r15() const
      {
        assert_choice_type(types::n2_tx_ant_tm3_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<2> >();
      }
      const fixed_bitstring<4>& n4_tx_ant_tm3_r15() const
      {
        assert_choice_type(types::n4_tx_ant_tm3_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<4> >();
      }
      const fixed_bitstring<6>& n2_tx_ant_tm4_r15() const
      {
        assert_choice_type(types::n2_tx_ant_tm4_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<6> >();
      }
      const fixed_bitstring<64>& n4_tx_ant_tm4_r15() const
      {
        assert_choice_type(types::n4_tx_ant_tm4_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<64> >();
      }
      const fixed_bitstring<4>& n2_tx_ant_tm5_r15() const
      {
        assert_choice_type(types::n2_tx_ant_tm5_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<4> >();
      }
      const fixed_bitstring<16>& n4_tx_ant_tm5_r15() const
      {
        assert_choice_type(types::n4_tx_ant_tm5_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<16> >();
      }
      const fixed_bitstring<4>& n2_tx_ant_tm6_r15() const
      {
        assert_choice_type(types::n2_tx_ant_tm6_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<4> >();
      }
      const fixed_bitstring<16>& n4_tx_ant_tm6_r15() const
      {
        assert_choice_type(types::n4_tx_ant_tm6_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<16> >();
      }
      const fixed_bitstring<6>& n2_tx_ant_tm8_r15() const
      {
        assert_choice_type(types::n2_tx_ant_tm8_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<6> >();
      }
      const fixed_bitstring<64>& n4_tx_ant_tm8_r15() const
      {
        assert_choice_type(types::n4_tx_ant_tm8_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<64> >();
      }
      const fixed_bitstring<6>& n2_tx_ant_tm9and10_r15() const
      {
        assert_choice_type(types::n2_tx_ant_tm9and10_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<6> >();
      }
      const fixed_bitstring<96>& n4_tx_ant_tm9and10_r15() const
      {
        assert_choice_type(types::n4_tx_ant_tm9and10_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<96> >();
      }
      const fixed_bitstring<109>& n8_tx_ant_tm9and10_r15() const
      {
        assert_choice_type(types::n8_tx_ant_tm9and10_r15, type_, "codebookSubsetRestriction");
        return c.get<fixed_bitstring<109> >();
      }
      fixed_bitstring<2>&   set_n2_tx_ant_tm3_r15();
      fixed_bitstring<4>&   set_n4_tx_ant_tm3_r15();
      fixed_bitstring<6>&   set_n2_tx_ant_tm4_r15();
      fixed_bitstring<64>&  set_n4_tx_ant_tm4_r15();
      fixed_bitstring<4>&   set_n2_tx_ant_tm5_r15();
      fixed_bitstring<16>&  set_n4_tx_ant_tm5_r15();
      fixed_bitstring<4>&   set_n2_tx_ant_tm6_r15();
      fixed_bitstring<16>&  set_n4_tx_ant_tm6_r15();
      fixed_bitstring<6>&   set_n2_tx_ant_tm8_r15();
      fixed_bitstring<64>&  set_n4_tx_ant_tm8_r15();
      fixed_bitstring<6>&   set_n2_tx_ant_tm9and10_r15();
      fixed_bitstring<96>&  set_n4_tx_ant_tm9and10_r15();
      fixed_bitstring<109>& set_n8_tx_ant_tm9and10_r15();

    private:
      types                                  type_;
      choice_buffer_t<fixed_bitstring<109> > c;

      void destroy_();
    };
    struct max_layers_mimo_stti_r15_opts {
      enum options { two_layers, four_layers, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<max_layers_mimo_stti_r15_opts> max_layers_mimo_stti_r15_e_;

    // member variables
    bool                        tx_mode_dl_mbsfn_r15_present     = false;
    bool                        tx_mode_dl_non_mbsfn_r15_present = false;
    bool                        codebook_subset_restrict_present = false;
    bool                        max_layers_mimo_stti_r15_present = false;
    tx_mode_dl_mbsfn_r15_e_     tx_mode_dl_mbsfn_r15;
    tx_mode_dl_non_mbsfn_r15_e_ tx_mode_dl_non_mbsfn_r15;
    codebook_subset_restrict_c_ codebook_subset_restrict;
    max_layers_mimo_stti_r15_e_ max_layers_mimo_stti_r15;
    bool                        slot_subslot_pdsch_tx_div_minus2_layer_r15 = false;
    bool                        slot_subslot_pdsch_tx_div_minus4_layer_r15 = false;
  };
  using types = setup_e;

  // choice methods
  ant_info_ded_stti_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ant_info_ded_stti_r15_c& other) const;
  bool        operator!=(const ant_info_ded_stti_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "AntennaInfoDedicatedSTTI-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "AntennaInfoDedicatedSTTI-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// AntennaInfoUL-STTI-r15 ::= SEQUENCE
struct ant_info_ul_stti_r15_s {
  struct tx_mode_ul_stti_r15_opts {
    enum options { tm1, tm2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<tx_mode_ul_stti_r15_opts> tx_mode_ul_stti_r15_e_;

  // member variables
  bool                   tx_mode_ul_stti_r15_present = false;
  tx_mode_ul_stti_r15_e_ tx_mode_ul_stti_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ant_info_ul_stti_r15_s& other) const;
  bool        operator!=(const ant_info_ul_stti_r15_s& other) const { return not(*this == other); }
};

// CE-PDSCH-14HARQ-Config-r17 ::= SEQUENCE
struct ce_pdsch_minus14_harq_cfg_r17_s {
  struct ce_harq_ack_delay_r17_opts {
    enum options { alt_minus1, alt_minus2e, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<ce_harq_ack_delay_r17_opts> ce_harq_ack_delay_r17_e_;

  // member variables
  ce_harq_ack_delay_r17_e_ ce_harq_ack_delay_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CE-PDSCH-MultiTB-Config-r16 ::= SEQUENCE
struct ce_pdsch_multi_tb_cfg_r16_s {
  bool interleaving_r16_present      = false;
  bool harq_ack_bundling_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CE-PUSCH-MultiTB-Config-r16 ::= SEQUENCE
struct ce_pusch_multi_tb_cfg_r16_s {
  bool interleaving_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CQI-ReportConfig-r15 ::= CHOICE
struct cqi_report_cfg_r15_c {
  struct setup_s_ {
    struct alt_cqi_table_minus1024_qam_r15_opts {
      enum options { all_sfs, csi_sf_set1, csi_sf_set2, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<alt_cqi_table_minus1024_qam_r15_opts> alt_cqi_table_minus1024_qam_r15_e_;

    // member variables
    bool                               cqi_report_cfg_r10_present              = false;
    bool                               cqi_report_cfg_v1130_present            = false;
    bool                               cqi_report_cfg_pcell_v1250_present      = false;
    bool                               cqi_report_cfg_v1310_present            = false;
    bool                               cqi_report_cfg_v1320_present            = false;
    bool                               cqi_report_cfg_v1430_present            = false;
    bool                               alt_cqi_table_minus1024_qam_r15_present = false;
    cqi_report_cfg_r10_s               cqi_report_cfg_r10;
    cqi_report_cfg_v1130_s             cqi_report_cfg_v1130;
    cqi_report_cfg_v1250_s             cqi_report_cfg_pcell_v1250;
    cqi_report_cfg_v1310_s             cqi_report_cfg_v1310;
    cqi_report_cfg_v1320_s             cqi_report_cfg_v1320;
    cqi_report_cfg_v1430_s             cqi_report_cfg_v1430;
    alt_cqi_table_minus1024_qam_r15_e_ alt_cqi_table_minus1024_qam_r15;
  };
  using types = setup_e;

  // choice methods
  cqi_report_cfg_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_cfg_r15_c& other) const;
  bool        operator!=(const cqi_report_cfg_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "CQI-ReportConfig-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "CQI-ReportConfig-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CQI-ReportPeriodic ::= CHOICE
struct cqi_report_periodic_c {
  struct setup_s_ {
    struct cqi_format_ind_periodic_c_ {
      struct subband_cqi_s_ {
        uint8_t k = 1;
      };
      struct types_opts {
        enum options { wideband_cqi, subband_cqi, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      cqi_format_ind_periodic_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      subband_cqi_s_& subband_cqi()
      {
        assert_choice_type(types::subband_cqi, type_, "cqi-FormatIndicatorPeriodic");
        return c;
      }
      const subband_cqi_s_& subband_cqi() const
      {
        assert_choice_type(types::subband_cqi, type_, "cqi-FormatIndicatorPeriodic");
        return c;
      }
      void            set_wideband_cqi();
      subband_cqi_s_& set_subband_cqi();

    private:
      types          type_;
      subband_cqi_s_ c;
    };

    // member variables
    bool                       ri_cfg_idx_present = false;
    uint16_t                   cqi_pucch_res_idx  = 0;
    uint16_t                   cqi_pmi_cfg_idx    = 0;
    cqi_format_ind_periodic_c_ cqi_format_ind_periodic;
    uint16_t                   ri_cfg_idx             = 0;
    bool                       simul_ack_nack_and_cqi = false;
  };
  using types = setup_e;

  // choice methods
  cqi_report_periodic_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "CQI-ReportPeriodic");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "CQI-ReportPeriodic");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CSI-RS-Config-r15 ::= CHOICE
struct csi_rs_cfg_r15_c {
  struct setup_s_ {
    bool               csi_rs_cfg_r10_present   = false;
    bool               csi_rs_cfg_v1250_present = false;
    bool               csi_rs_cfg_v1310_present = false;
    bool               csi_rs_cfg_v1430_present = false;
    csi_rs_cfg_r10_s   csi_rs_cfg_r10;
    csi_rs_cfg_v1250_s csi_rs_cfg_v1250;
    csi_rs_cfg_v1310_s csi_rs_cfg_v1310;
    csi_rs_cfg_v1430_s csi_rs_cfg_v1430;
  };
  using types = setup_e;

  // choice methods
  csi_rs_cfg_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_r15_c& other) const;
  bool        operator!=(const csi_rs_cfg_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "CSI-RS-Config-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "CSI-RS-Config-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CSI-RS-ConfigNZPToAddModList-r15 ::= SEQUENCE (SIZE (1..24)) OF CSI-RS-ConfigNZP-r11
using csi_rs_cfg_nzp_to_add_mod_list_r15_l = dyn_array<csi_rs_cfg_nzp_r11_s>;

// CSI-RS-ConfigNZPToReleaseList-r15 ::= SEQUENCE (SIZE (1..24)) OF INTEGER (1..24)
using csi_rs_cfg_nzp_to_release_list_r15_l = bounded_array<uint8_t, 24>;

// CSI-RS-ConfigZP-ApList-r14 ::= CHOICE
struct csi_rs_cfg_zp_ap_list_r14_c {
  using setup_l_ = dyn_array<csi_rs_cfg_zp_r11_s>;
  using types    = setup_e;

  // choice methods
  csi_rs_cfg_zp_ap_list_r14_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_zp_ap_list_r14_c& other) const;
  bool        operator!=(const csi_rs_cfg_zp_ap_list_r14_c& other) const { return not(*this == other); }
  // getters
  setup_l_& setup()
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigZP-ApList-r14");
    return c;
  }
  const setup_l_& setup() const
  {
    assert_choice_type(types::setup, type_, "CSI-RS-ConfigZP-ApList-r14");
    return c;
  }
  void      set_release();
  setup_l_& set_setup();

private:
  types    type_;
  setup_l_ c;
};

// CSI-RS-ConfigZPToAddModList-r11 ::= SEQUENCE (SIZE (1..4)) OF CSI-RS-ConfigZP-r11
using csi_rs_cfg_zp_to_add_mod_list_r11_l = dyn_array<csi_rs_cfg_zp_r11_s>;

// CSI-RS-ConfigZPToReleaseList-r11 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (1..4)
using csi_rs_cfg_zp_to_release_list_r11_l = bounded_array<uint8_t, 4>;

// DMRS-Config-r11 ::= CHOICE
struct dmrs_cfg_r11_c {
  struct setup_s_ {
    uint16_t scrambling_id_r11  = 0;
    uint16_t scrambling_id2_r11 = 0;
  };
  using types = setup_e;

  // choice methods
  dmrs_cfg_r11_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const dmrs_cfg_r11_c& other) const;
  bool        operator!=(const dmrs_cfg_r11_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "DMRS-Config-r11");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "DMRS-Config-r11");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// DMRS-Config-v1310 ::= SEQUENCE
struct dmrs_cfg_v1310_s {
  bool dmrs_table_alt_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const dmrs_cfg_v1310_s& other) const;
  bool        operator!=(const dmrs_cfg_v1310_s& other) const { return not(*this == other); }
};

// DeltaTxD-OffsetListPUCCH-r10 ::= SEQUENCE
struct delta_tx_d_offset_list_pucch_r10_s {
  struct delta_tx_d_offset_pucch_format1_r10_opts {
    enum options { db0, db_minus2, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_tx_d_offset_pucch_format1_r10_opts> delta_tx_d_offset_pucch_format1_r10_e_;
  struct delta_tx_d_offset_pucch_format1a1b_r10_opts {
    enum options { db0, db_minus2, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_tx_d_offset_pucch_format1a1b_r10_opts> delta_tx_d_offset_pucch_format1a1b_r10_e_;
  struct delta_tx_d_offset_pucch_format22a2b_r10_opts {
    enum options { db0, db_minus2, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_tx_d_offset_pucch_format22a2b_r10_opts> delta_tx_d_offset_pucch_format22a2b_r10_e_;
  struct delta_tx_d_offset_pucch_format3_r10_opts {
    enum options { db0, db_minus2, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_tx_d_offset_pucch_format3_r10_opts> delta_tx_d_offset_pucch_format3_r10_e_;

  // member variables
  bool                                       ext = false;
  delta_tx_d_offset_pucch_format1_r10_e_     delta_tx_d_offset_pucch_format1_r10;
  delta_tx_d_offset_pucch_format1a1b_r10_e_  delta_tx_d_offset_pucch_format1a1b_r10;
  delta_tx_d_offset_pucch_format22a2b_r10_e_ delta_tx_d_offset_pucch_format22a2b_r10;
  delta_tx_d_offset_pucch_format3_r10_e_     delta_tx_d_offset_pucch_format3_r10;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const delta_tx_d_offset_list_pucch_r10_s& other) const;
  bool        operator!=(const delta_tx_d_offset_list_pucch_r10_s& other) const { return not(*this == other); }
};

// DeltaTxD-OffsetListPUCCH-v1130 ::= SEQUENCE
struct delta_tx_d_offset_list_pucch_v1130_s {
  struct delta_tx_d_offset_pucch_format1b_cs_r11_opts {
    enum options { db0, db_minus1, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_tx_d_offset_pucch_format1b_cs_r11_opts> delta_tx_d_offset_pucch_format1b_cs_r11_e_;

  // member variables
  delta_tx_d_offset_pucch_format1b_cs_r11_e_ delta_tx_d_offset_pucch_format1b_cs_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const delta_tx_d_offset_list_pucch_v1130_s& other) const;
  bool        operator!=(const delta_tx_d_offset_list_pucch_v1130_s& other) const { return not(*this == other); }
};

// EIMTA-MainConfig-r12 ::= CHOICE
struct eimta_main_cfg_r12_c {
  struct setup_s_ {
    struct eimta_cmd_periodicity_r12_opts {
      enum options { sf10, sf20, sf40, sf80, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<eimta_cmd_periodicity_r12_opts> eimta_cmd_periodicity_r12_e_;

    // member variables
    fixed_bitstring<16>          eimta_rnti_r12;
    eimta_cmd_periodicity_r12_e_ eimta_cmd_periodicity_r12;
    fixed_bitstring<10>          eimta_cmd_sf_set_r12;
  };
  using types = setup_e;

  // choice methods
  eimta_main_cfg_r12_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const eimta_main_cfg_r12_c& other) const;
  bool        operator!=(const eimta_main_cfg_r12_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "EIMTA-MainConfig-r12");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "EIMTA-MainConfig-r12");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// EIMTA-MainConfigServCell-r12 ::= CHOICE
struct eimta_main_cfg_serv_cell_r12_c {
  struct setup_s_ {
    struct eimta_harq_ref_cfg_r12_opts {
      enum options { sa2, sa4, sa5, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<eimta_harq_ref_cfg_r12_opts> eimta_harq_ref_cfg_r12_e_;
    struct mbsfn_sf_cfg_list_v1250_c_ {
      struct setup_s_ {
        mbsfn_sf_cfg_list_l sf_cfg_list_r12;
      };
      using types = setup_e;

      // choice methods
      mbsfn_sf_cfg_list_v1250_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const mbsfn_sf_cfg_list_v1250_c_& other) const;
      bool        operator!=(const mbsfn_sf_cfg_list_v1250_c_& other) const { return not(*this == other); }
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "mbsfn-SubframeConfigList-v1250");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "mbsfn-SubframeConfigList-v1250");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };

    // member variables
    uint8_t                    eimta_ul_dl_cfg_idx_r12 = 1;
    eimta_harq_ref_cfg_r12_e_  eimta_harq_ref_cfg_r12;
    mbsfn_sf_cfg_list_v1250_c_ mbsfn_sf_cfg_list_v1250;
  };
  using types = setup_e;

  // choice methods
  eimta_main_cfg_serv_cell_r12_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const eimta_main_cfg_serv_cell_r12_c& other) const;
  bool        operator!=(const eimta_main_cfg_serv_cell_r12_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "EIMTA-MainConfigServCell-r12");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "EIMTA-MainConfigServCell-r12");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// EPDCCH-SetConfigToAddModList-r11 ::= SEQUENCE (SIZE (1..2)) OF EPDCCH-SetConfig-r11
using epdcch_set_cfg_to_add_mod_list_r11_l = dyn_array<epdcch_set_cfg_r11_s>;

// EPDCCH-SetConfigToReleaseList-r11 ::= SEQUENCE (SIZE (1..2)) OF INTEGER (0..1)
using epdcch_set_cfg_to_release_list_r11_l = bounded_array<uint8_t, 2>;

// Format4-resource-r13 ::= SEQUENCE
struct format4_res_r13_s {
  uint8_t start_prb_format4_r13 = 0;
  uint8_t nof_prb_format4_r13   = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const format4_res_r13_s& other) const;
  bool        operator!=(const format4_res_r13_s& other) const { return not(*this == other); }
};

// Format5-resource-r13 ::= SEQUENCE
struct format5_res_r13_s {
  uint8_t start_prb_format5_r13 = 0;
  uint8_t cdm_idx_format5_r13   = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const format5_res_r13_s& other) const;
  bool        operator!=(const format5_res_r13_s& other) const { return not(*this == other); }
};

// N1PUCCH-AN-CS-r10 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (0..2047)
using n1_pucch_an_cs_r10_l = bounded_array<uint16_t, 4>;

// PDCCH-CandidateReductionValue-r13 ::= ENUMERATED
struct pdcch_candidate_reduction_value_r13_opts {
  enum options { n0, n33, n66, n100, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<pdcch_candidate_reduction_value_r13_opts> pdcch_candidate_reduction_value_r13_e;

// PUCCH-ConfigDedicated-v1530 ::= SEQUENCE
struct pucch_cfg_ded_v1530_s {
  struct codebooksize_determination_stti_r15_opts {
    enum options { dai, cc, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<codebooksize_determination_stti_r15_opts> codebooksize_determination_stti_r15_e_;

  // member variables
  bool                                   n1_pucch_an_spt_r15_present                 = false;
  bool                                   codebooksize_determination_stti_r15_present = false;
  uint16_t                               n1_pucch_an_spt_r15                         = 0;
  codebooksize_determination_stti_r15_e_ codebooksize_determination_stti_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pucch_cfg_ded_v1530_s& other) const;
  bool        operator!=(const pucch_cfg_ded_v1530_s& other) const { return not(*this == other); }
};

// PUCCH-Format3-Conf-r13 ::= SEQUENCE
struct pucch_format3_conf_r13_s {
  using n3_pucch_an_list_r13_l_ = bounded_array<uint16_t, 4>;
  struct two_ant_port_activ_pucch_format3_r13_c_ {
    struct setup_s_ {
      using n3_pucch_an_list_p1_r13_l_ = bounded_array<uint16_t, 4>;

      // member variables
      n3_pucch_an_list_p1_r13_l_ n3_pucch_an_list_p1_r13;
    };
    using types = setup_e;

    // choice methods
    two_ant_port_activ_pucch_format3_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const two_ant_port_activ_pucch_format3_r13_c_& other) const;
    bool        operator!=(const two_ant_port_activ_pucch_format3_r13_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "twoAntennaPortActivatedPUCCH-Format3-r13");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "twoAntennaPortActivatedPUCCH-Format3-r13");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                                    n3_pucch_an_list_r13_present                 = false;
  bool                                    two_ant_port_activ_pucch_format3_r13_present = false;
  n3_pucch_an_list_r13_l_                 n3_pucch_an_list_r13;
  two_ant_port_activ_pucch_format3_r13_c_ two_ant_port_activ_pucch_format3_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pucch_format3_conf_r13_s& other) const;
  bool        operator!=(const pucch_format3_conf_r13_s& other) const { return not(*this == other); }
};

// RE-MappingQCLConfigToAddModList-r11 ::= SEQUENCE (SIZE (1..4)) OF PDSCH-RE-MappingQCL-Config-r11
using re_map_qcl_cfg_to_add_mod_list_r11_l = dyn_array<pdsch_re_map_qcl_cfg_r11_s>;

// RE-MappingQCLConfigToReleaseList-r11 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (1..4)
using re_map_qcl_cfg_to_release_list_r11_l = bounded_array<uint8_t, 4>;

// SPDCCH-Config-r15 ::= CHOICE
struct spdcch_cfg_r15_c {
  struct setup_s_ {
    struct spdcch_l1_reuse_ind_r15_opts {
      enum options { n0, n1, n2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<spdcch_l1_reuse_ind_r15_opts> spdcch_l1_reuse_ind_r15_e_;

    // member variables
    bool                       spdcch_l1_reuse_ind_r15_present = false;
    bool                       spdcch_set_cfg_r15_present      = false;
    spdcch_l1_reuse_ind_r15_e_ spdcch_l1_reuse_ind_r15;
    spdcch_set_r15_l           spdcch_set_cfg_r15;
  };
  using types = setup_e;

  // choice methods
  spdcch_cfg_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const spdcch_cfg_r15_c& other) const;
  bool        operator!=(const spdcch_cfg_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SPDCCH-Config-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SPDCCH-Config-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SPUCCH-Config-r15 ::= CHOICE
struct spucch_cfg_r15_c {
  struct setup_s_ {
    struct dummy_s_ {
      using n3_spucch_an_list_r15_l_ = bounded_array<uint16_t, 4>;

      // member variables
      n3_spucch_an_list_r15_l_ n3_spucch_an_list_r15;
    };

    // member variables
    bool             spucch_set_r15_present                           = false;
    bool             two_ant_port_activ_spucch_format1a1b_r15_present = false;
    spucch_set_r15_l spucch_set_r15;
    dummy_s_         dummy;
  };
  using types = setup_e;

  // choice methods
  spucch_cfg_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const spucch_cfg_r15_c& other) const;
  bool        operator!=(const spucch_cfg_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SPUCCH-Config-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SPUCCH-Config-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SRS-CC-SetIndex-r14 ::= SEQUENCE
struct srs_cc_set_idx_r14_s {
  uint8_t cc_set_idx_r14           = 0;
  uint8_t cc_idx_in_one_cc_set_r14 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_cc_set_idx_r14_s& other) const;
  bool        operator!=(const srs_cc_set_idx_r14_s& other) const { return not(*this == other); }
};

// SRS-ConfigAdd-r16 ::= SEQUENCE
struct srs_cfg_add_r16_s {
  struct srs_rep_num_add_r16_opts {
    enum options { n1, n2, n3, n4, n6, n7, n8, n9, n12, n13, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<srs_rep_num_add_r16_opts> srs_rep_num_add_r16_e_;
  struct srs_bw_add_r16_opts {
    enum options { bw0, bw1, bw2, bw3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<srs_bw_add_r16_opts> srs_bw_add_r16_e_;
  struct srs_hop_bw_add_r16_opts {
    enum options { hbw0, hbw1, hbw2, hbw3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<srs_hop_bw_add_r16_opts> srs_hop_bw_add_r16_e_;
  struct srs_cyclic_shift_add_r16_opts {
    enum options { cs0, cs1, cs2, cs3, cs4, cs5, cs6, cs7, cs8, cs9, cs10, cs11, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<srs_cyclic_shift_add_r16_opts> srs_cyclic_shift_add_r16_e_;
  struct srs_tx_comb_num_add_r16_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<srs_tx_comb_num_add_r16_opts> srs_tx_comb_num_add_r16_e_;

  // member variables
  bool                        srs_guard_symbol_as_add_r16_present = false;
  bool                        srs_guard_symbol_fh_add_r16_present = false;
  srs_rep_num_add_r16_e_      srs_rep_num_add_r16;
  srs_bw_add_r16_e_           srs_bw_add_r16;
  srs_hop_bw_add_r16_e_       srs_hop_bw_add_r16;
  uint8_t                     srs_freq_domain_pos_add_r16 = 0;
  srs_ant_port_e              srs_ant_port_add_r16;
  srs_cyclic_shift_add_r16_e_ srs_cyclic_shift_add_r16;
  srs_tx_comb_num_add_r16_e_  srs_tx_comb_num_add_r16;
  uint8_t                     srs_tx_comb_add_r16   = 0;
  uint8_t                     srs_start_pos_add_r16 = 1;
  uint8_t                     srs_dur_add_r16       = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_cfg_add_r16_s& other) const;
  bool        operator!=(const srs_cfg_add_r16_s& other) const { return not(*this == other); }
};

// SRS-ConfigAp-r10 ::= SEQUENCE
struct srs_cfg_ap_r10_s {
  struct srs_bw_ap_r10_opts {
    enum options { bw0, bw1, bw2, bw3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<srs_bw_ap_r10_opts> srs_bw_ap_r10_e_;
  struct cyclic_shift_ap_r10_opts {
    enum options { cs0, cs1, cs2, cs3, cs4, cs5, cs6, cs7, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<cyclic_shift_ap_r10_opts> cyclic_shift_ap_r10_e_;

  // member variables
  srs_ant_port_e         srs_ant_port_ap_r10;
  srs_bw_ap_r10_e_       srs_bw_ap_r10;
  uint8_t                freq_domain_position_ap_r10 = 0;
  uint8_t                tx_comb_ap_r10              = 0;
  cyclic_shift_ap_r10_e_ cyclic_shift_ap_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_cfg_ap_r10_s& other) const;
  bool        operator!=(const srs_cfg_ap_r10_s& other) const { return not(*this == other); }
};

// SRS-ConfigAp-r13 ::= SEQUENCE
struct srs_cfg_ap_r13_s {
  struct srs_bw_ap_r13_opts {
    enum options { bw0, bw1, bw2, bw3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<srs_bw_ap_r13_opts> srs_bw_ap_r13_e_;
  struct cyclic_shift_ap_r13_opts {
    enum options { cs0, cs1, cs2, cs3, cs4, cs5, cs6, cs7, cs8, cs9, cs10, cs11, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<cyclic_shift_ap_r13_opts> cyclic_shift_ap_r13_e_;
  struct tx_comb_num_r13_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<tx_comb_num_r13_opts> tx_comb_num_r13_e_;

  // member variables
  srs_ant_port_e         srs_ant_port_ap_r13;
  srs_bw_ap_r13_e_       srs_bw_ap_r13;
  uint8_t                freq_domain_position_ap_r13 = 0;
  uint8_t                tx_comb_ap_r13              = 0;
  cyclic_shift_ap_r13_e_ cyclic_shift_ap_r13;
  tx_comb_num_r13_e_     tx_comb_num_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_cfg_ap_r13_s& other) const;
  bool        operator!=(const srs_cfg_ap_r13_s& other) const { return not(*this == other); }
};

// SRS-ConfigAp-v1310 ::= SEQUENCE
struct srs_cfg_ap_v1310_s {
  struct cyclic_shift_ap_v1310_opts {
    enum options { cs8, cs9, cs10, cs11, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<cyclic_shift_ap_v1310_opts> cyclic_shift_ap_v1310_e_;
  struct tx_comb_num_r13_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<tx_comb_num_r13_opts> tx_comb_num_r13_e_;

  // member variables
  bool                     tx_comb_ap_v1310_present      = false;
  bool                     cyclic_shift_ap_v1310_present = false;
  bool                     tx_comb_num_r13_present       = false;
  uint8_t                  tx_comb_ap_v1310              = 2;
  cyclic_shift_ap_v1310_e_ cyclic_shift_ap_v1310;
  tx_comb_num_r13_e_       tx_comb_num_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_cfg_ap_v1310_s& other) const;
  bool        operator!=(const srs_cfg_ap_v1310_s& other) const { return not(*this == other); }
};

// SchedulingRequestConfig-v1530 ::= CHOICE
struct sched_request_cfg_v1530_c {
  struct setup_s_ {
    struct dssr_trans_max_r15_opts {
      enum options { n4, n8, n16, n32, n64, spare3, spare2, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<dssr_trans_max_r15_opts> dssr_trans_max_r15_e_;

    // member variables
    bool                             sr_slot_spucch_idx_fh_r15_present      = false;
    bool                             sr_slot_spucch_idx_no_fh_r15_present   = false;
    bool                             sr_subslot_spucch_res_list_r15_present = false;
    bool                             sr_cfg_idx_slot_r15_present            = false;
    bool                             sr_cfg_idx_subslot_r15_present         = false;
    uint16_t                         sr_slot_spucch_idx_fh_r15              = 0;
    uint16_t                         sr_slot_spucch_idx_no_fh_r15           = 0;
    sr_subslot_spucch_res_list_r15_l sr_subslot_spucch_res_list_r15;
    uint8_t                          sr_cfg_idx_slot_r15    = 0;
    uint8_t                          sr_cfg_idx_subslot_r15 = 0;
    dssr_trans_max_r15_e_            dssr_trans_max_r15;
  };
  using types = setup_e;

  // choice methods
  sched_request_cfg_v1530_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const sched_request_cfg_v1530_c& other) const;
  bool        operator!=(const sched_request_cfg_v1530_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SchedulingRequestConfig-v1530");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SchedulingRequestConfig-v1530");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// ShortTTI-r15 ::= SEQUENCE
struct short_tti_r15_s {
  bool                dl_stti_len_r15_present = false;
  bool                ul_stti_len_r15_present = false;
  short_tti_len_r15_e dl_stti_len_r15;
  short_tti_len_r15_e ul_stti_len_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const short_tti_r15_s& other) const;
  bool        operator!=(const short_tti_r15_s& other) const { return not(*this == other); }
};

// SlotOrSubslotPDSCH-Config-r15 ::= CHOICE
struct slot_or_subslot_pdsch_cfg_r15_c {
  struct setup_s_ {
    struct alt_cqi_table_stti_r15_opts {
      enum options { all_sfs, csi_sf_set1, csi_sf_set2, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<alt_cqi_table_stti_r15_opts> alt_cqi_table_stti_r15_e_;
    struct alt_cqi_table1024_qam_stti_r15_opts {
      enum options { all_sfs, csi_sf_set1, csi_sf_set2, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<alt_cqi_table1024_qam_stti_r15_opts> alt_cqi_table1024_qam_stti_r15_e_;
    struct res_alloc_r15_opts {
      enum options { res_alloc_type0, res_alloc_type2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<res_alloc_r15_opts> res_alloc_r15_e_;

    // member variables
    bool                              ext                                    = false;
    bool                              alt_cqi_table_stti_r15_present         = false;
    bool                              alt_cqi_table1024_qam_stti_r15_present = false;
    bool                              res_alloc_r15_present                  = false;
    bool                              tbs_idx_alt_stti_r15_present           = false;
    bool                              tbs_idx_alt2_stti_r15_present          = false;
    bool                              tbs_idx_alt3_stti_r15_present          = false;
    alt_cqi_table_stti_r15_e_         alt_cqi_table_stti_r15;
    alt_cqi_table1024_qam_stti_r15_e_ alt_cqi_table1024_qam_stti_r15;
    res_alloc_r15_e_                  res_alloc_r15;
    // ...
  };
  using types = setup_e;

  // choice methods
  slot_or_subslot_pdsch_cfg_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const slot_or_subslot_pdsch_cfg_r15_c& other) const;
  bool        operator!=(const slot_or_subslot_pdsch_cfg_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SlotOrSubslotPDSCH-Config-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SlotOrSubslotPDSCH-Config-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SlotOrSubslotPUSCH-Config-r15 ::= CHOICE
struct slot_or_subslot_pusch_cfg_r15_c {
  struct setup_s_ {
    using beta_offset_subslot_ack_idx_r15_l_  = bounded_array<uint8_t, 2>;
    using beta_offset2_subslot_ack_idx_r15_l_ = bounded_array<uint8_t, 2>;
    using beta_offset_subslot_ri_idx_r15_l_   = bounded_array<uint8_t, 2>;

    // member variables
    bool                                ext                                       = false;
    bool                                beta_offset_slot_ack_idx_r15_present      = false;
    bool                                beta_offset2_slot_ack_idx_r15_present     = false;
    bool                                beta_offset_subslot_ack_idx_r15_present   = false;
    bool                                beta_offset2_subslot_ack_idx_r15_present  = false;
    bool                                beta_offset_slot_ri_idx_r15_present       = false;
    bool                                beta_offset_subslot_ri_idx_r15_present    = false;
    bool                                beta_offset_slot_cqi_idx_r15_present      = false;
    bool                                beta_offset_subslot_cqi_idx_r15_present   = false;
    bool                                enable256_qam_slot_or_subslot_r15_present = false;
    bool                                res_alloc_offset_r15_present              = false;
    uint8_t                             beta_offset_slot_ack_idx_r15              = 0;
    uint8_t                             beta_offset2_slot_ack_idx_r15             = 0;
    beta_offset_subslot_ack_idx_r15_l_  beta_offset_subslot_ack_idx_r15;
    beta_offset2_subslot_ack_idx_r15_l_ beta_offset2_subslot_ack_idx_r15;
    uint8_t                             beta_offset_slot_ri_idx_r15 = 0;
    beta_offset_subslot_ri_idx_r15_l_   beta_offset_subslot_ri_idx_r15;
    uint8_t                             beta_offset_slot_cqi_idx_r15    = 0;
    uint8_t                             beta_offset_subslot_cqi_idx_r15 = 0;
    enable256_qam_r14_c                 enable256_qam_slot_or_subslot_r15;
    uint8_t                             res_alloc_offset_r15              = 1;
    bool                                ul_dmrs_ifdma_slot_or_subslot_r15 = false;
    // ...
  };
  using types = setup_e;

  // choice methods
  slot_or_subslot_pusch_cfg_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const slot_or_subslot_pusch_cfg_r15_c& other) const;
  bool        operator!=(const slot_or_subslot_pusch_cfg_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SlotOrSubslotPUSCH-Config-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SlotOrSubslotPUSCH-Config-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// TDD-PUSCH-UpPTS-r14 ::= CHOICE
struct tdd_pusch_up_pts_r14_c {
  struct setup_s_ {
    struct sym_pusch_up_pts_r14_opts {
      enum options { sym1, sym2, sym3, sym4, sym5, sym6, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<sym_pusch_up_pts_r14_opts> sym_pusch_up_pts_r14_e_;

    // member variables
    bool                    sym_pusch_up_pts_r14_present     = false;
    bool                    dmrs_less_up_pts_cfg_r14_present = false;
    sym_pusch_up_pts_r14_e_ sym_pusch_up_pts_r14;
  };
  using types = setup_e;

  // choice methods
  tdd_pusch_up_pts_r14_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "TDD-PUSCH-UpPTS-r14");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "TDD-PUSCH-UpPTS-r14");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// UplinkPowerControlDedicatedSTTI-r15 ::= SEQUENCE
struct ul_pwr_ctrl_ded_stti_r15_s {
  bool                                delta_tx_d_offset_list_spucch_r15_present = false;
  bool                                accumulation_enabled_stti_r15             = false;
  delta_tx_d_offset_list_spucch_r15_s delta_tx_d_offset_list_spucch_r15;
  bool                                ul_pwr_csi_payload = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pwr_ctrl_ded_stti_r15_s& other) const;
  bool        operator!=(const ul_pwr_ctrl_ded_stti_r15_s& other) const { return not(*this == other); }
};

// AntennaInfoDedicated ::= SEQUENCE
struct ant_info_ded_s {
  struct tx_mode_opts {
    enum options { tm1, tm2, tm3, tm4, tm5, tm6, tm7, tm8_v920, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<tx_mode_opts> tx_mode_e_;
  struct codebook_subset_restrict_c_ {
    struct types_opts {
      enum options {
        n2_tx_ant_tm3,
        n4_tx_ant_tm3,
        n2_tx_ant_tm4,
        n4_tx_ant_tm4,
        n2_tx_ant_tm5,
        n4_tx_ant_tm5,
        n2_tx_ant_tm6,
        n4_tx_ant_tm6,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    codebook_subset_restrict_c_() = default;
    codebook_subset_restrict_c_(const codebook_subset_restrict_c_& other);
    codebook_subset_restrict_c_& operator=(const codebook_subset_restrict_c_& other);
    ~codebook_subset_restrict_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<2>& n2_tx_ant_tm3()
    {
      assert_choice_type(types::n2_tx_ant_tm3, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<2> >();
    }
    fixed_bitstring<4>& n4_tx_ant_tm3()
    {
      assert_choice_type(types::n4_tx_ant_tm3, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<4> >();
    }
    fixed_bitstring<6>& n2_tx_ant_tm4()
    {
      assert_choice_type(types::n2_tx_ant_tm4, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<6> >();
    }
    fixed_bitstring<64>& n4_tx_ant_tm4()
    {
      assert_choice_type(types::n4_tx_ant_tm4, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<64> >();
    }
    fixed_bitstring<4>& n2_tx_ant_tm5()
    {
      assert_choice_type(types::n2_tx_ant_tm5, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<4> >();
    }
    fixed_bitstring<16>& n4_tx_ant_tm5()
    {
      assert_choice_type(types::n4_tx_ant_tm5, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<16> >();
    }
    fixed_bitstring<4>& n2_tx_ant_tm6()
    {
      assert_choice_type(types::n2_tx_ant_tm6, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<4> >();
    }
    fixed_bitstring<16>& n4_tx_ant_tm6()
    {
      assert_choice_type(types::n4_tx_ant_tm6, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<16> >();
    }
    const fixed_bitstring<2>& n2_tx_ant_tm3() const
    {
      assert_choice_type(types::n2_tx_ant_tm3, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<2> >();
    }
    const fixed_bitstring<4>& n4_tx_ant_tm3() const
    {
      assert_choice_type(types::n4_tx_ant_tm3, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<4> >();
    }
    const fixed_bitstring<6>& n2_tx_ant_tm4() const
    {
      assert_choice_type(types::n2_tx_ant_tm4, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<6> >();
    }
    const fixed_bitstring<64>& n4_tx_ant_tm4() const
    {
      assert_choice_type(types::n4_tx_ant_tm4, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<64> >();
    }
    const fixed_bitstring<4>& n2_tx_ant_tm5() const
    {
      assert_choice_type(types::n2_tx_ant_tm5, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<4> >();
    }
    const fixed_bitstring<16>& n4_tx_ant_tm5() const
    {
      assert_choice_type(types::n4_tx_ant_tm5, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<16> >();
    }
    const fixed_bitstring<4>& n2_tx_ant_tm6() const
    {
      assert_choice_type(types::n2_tx_ant_tm6, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<4> >();
    }
    const fixed_bitstring<16>& n4_tx_ant_tm6() const
    {
      assert_choice_type(types::n4_tx_ant_tm6, type_, "codebookSubsetRestriction");
      return c.get<fixed_bitstring<16> >();
    }
    fixed_bitstring<2>&  set_n2_tx_ant_tm3();
    fixed_bitstring<4>&  set_n4_tx_ant_tm3();
    fixed_bitstring<6>&  set_n2_tx_ant_tm4();
    fixed_bitstring<64>& set_n4_tx_ant_tm4();
    fixed_bitstring<4>&  set_n2_tx_ant_tm5();
    fixed_bitstring<16>& set_n4_tx_ant_tm5();
    fixed_bitstring<4>&  set_n2_tx_ant_tm6();
    fixed_bitstring<16>& set_n4_tx_ant_tm6();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<64> > c;

    void destroy_();
  };
  struct ue_tx_ant_sel_c_ {
    struct setup_opts {
      enum options { closed_loop, open_loop, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<setup_opts> setup_e_;
    using types = setup_e;

    // choice methods
    ue_tx_ant_sel_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_e_& setup()
    {
      assert_choice_type(types::setup, type_, "ue-TransmitAntennaSelection");
      return c;
    }
    const setup_e_& setup() const
    {
      assert_choice_type(types::setup, type_, "ue-TransmitAntennaSelection");
      return c;
    }
    void      set_release();
    setup_e_& set_setup();

  private:
    types    type_;
    setup_e_ c;
  };

  // member variables
  bool                        codebook_subset_restrict_present = false;
  tx_mode_e_                  tx_mode;
  codebook_subset_restrict_c_ codebook_subset_restrict;
  ue_tx_ant_sel_c_            ue_tx_ant_sel;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AntennaInfoDedicated-r10 ::= SEQUENCE
struct ant_info_ded_r10_s {
  struct tx_mode_r10_opts {
    enum options {
      tm1,
      tm2,
      tm3,
      tm4,
      tm5,
      tm6,
      tm7,
      tm8_v920,
      tm9_v1020,
      tm10_v1130,
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
  typedef enumerated<tx_mode_r10_opts> tx_mode_r10_e_;
  struct ue_tx_ant_sel_c_ {
    struct setup_opts {
      enum options { closed_loop, open_loop, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<setup_opts> setup_e_;
    using types = setup_e;

    // choice methods
    ue_tx_ant_sel_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const ue_tx_ant_sel_c_& other) const;
    bool        operator!=(const ue_tx_ant_sel_c_& other) const { return not(*this == other); }
    // getters
    setup_e_& setup()
    {
      assert_choice_type(types::setup, type_, "ue-TransmitAntennaSelection");
      return c;
    }
    const setup_e_& setup() const
    {
      assert_choice_type(types::setup, type_, "ue-TransmitAntennaSelection");
      return c;
    }
    void      set_release();
    setup_e_& set_setup();

  private:
    types    type_;
    setup_e_ c;
  };

  // member variables
  bool             codebook_subset_restrict_r10_present = false;
  tx_mode_r10_e_   tx_mode_r10;
  dyn_bitstring    codebook_subset_restrict_r10;
  ue_tx_ant_sel_c_ ue_tx_ant_sel;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ant_info_ded_r10_s& other) const;
  bool        operator!=(const ant_info_ded_r10_s& other) const { return not(*this == other); }
};

// AntennaInfoDedicated-v1250 ::= SEQUENCE
struct ant_info_ded_v1250_s {
  bool alt_codebook_enabled_for4_tx_r12 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ant_info_ded_v1250_s& other) const;
  bool        operator!=(const ant_info_ded_v1250_s& other) const { return not(*this == other); }
};

// AntennaInfoDedicated-v1430 ::= SEQUENCE
struct ant_info_ded_v1430_s {
  bool ce_ue_tx_ant_sel_cfg_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AntennaInfoDedicated-v1530 ::= CHOICE
struct ant_info_ded_v1530_c {
  struct setup_c_ {
    struct ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_opts {
      enum options { two, three, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_opts>
        ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_;
    struct types_opts {
      enum options {
        ue_tx_ant_sel_srs_minus1_t4_r_cfg_r15,
        ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15,
        nulltype
      } value;
      typedef int8_t number_type;

      const char* to_string() const;
      int8_t      to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    setup_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_& ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15()
    {
      assert_choice_type(types::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15, type_, "setup");
      return c;
    }
    const ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_& ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15() const
    {
      assert_choice_type(types::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15, type_, "setup");
      return c;
    }
    void                                              set_ue_tx_ant_sel_srs_minus1_t4_r_cfg_r15();
    ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_& set_ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15();

  private:
    types                                            type_;
    ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_ c;
  };
  using types = setup_e;

  // choice methods
  ant_info_ded_v1530_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_c_& setup()
  {
    assert_choice_type(types::setup, type_, "AntennaInfoDedicated-v1530");
    return c;
  }
  const setup_c_& setup() const
  {
    assert_choice_type(types::setup, type_, "AntennaInfoDedicated-v1530");
    return c;
  }
  void      set_release();
  setup_c_& set_setup();

private:
  types    type_;
  setup_c_ c;
};

// AntennaInfoDedicated-v920 ::= SEQUENCE
struct ant_info_ded_v920_s {
  struct codebook_subset_restrict_v920_c_ {
    struct types_opts {
      enum options { n2_tx_ant_tm8_r9, n4_tx_ant_tm8_r9, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    codebook_subset_restrict_v920_c_() = default;
    codebook_subset_restrict_v920_c_(const codebook_subset_restrict_v920_c_& other);
    codebook_subset_restrict_v920_c_& operator=(const codebook_subset_restrict_v920_c_& other);
    ~codebook_subset_restrict_v920_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<6>& n2_tx_ant_tm8_r9()
    {
      assert_choice_type(types::n2_tx_ant_tm8_r9, type_, "codebookSubsetRestriction-v920");
      return c.get<fixed_bitstring<6> >();
    }
    fixed_bitstring<32>& n4_tx_ant_tm8_r9()
    {
      assert_choice_type(types::n4_tx_ant_tm8_r9, type_, "codebookSubsetRestriction-v920");
      return c.get<fixed_bitstring<32> >();
    }
    const fixed_bitstring<6>& n2_tx_ant_tm8_r9() const
    {
      assert_choice_type(types::n2_tx_ant_tm8_r9, type_, "codebookSubsetRestriction-v920");
      return c.get<fixed_bitstring<6> >();
    }
    const fixed_bitstring<32>& n4_tx_ant_tm8_r9() const
    {
      assert_choice_type(types::n4_tx_ant_tm8_r9, type_, "codebookSubsetRestriction-v920");
      return c.get<fixed_bitstring<32> >();
    }
    fixed_bitstring<6>&  set_n2_tx_ant_tm8_r9();
    fixed_bitstring<32>& set_n4_tx_ant_tm8_r9();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<32> > c;

    void destroy_();
  };

  // member variables
  bool                             codebook_subset_restrict_v920_present = false;
  codebook_subset_restrict_v920_c_ codebook_subset_restrict_v920;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AntennaInfoUL-r10 ::= SEQUENCE
struct ant_info_ul_r10_s {
  struct tx_mode_ul_r10_opts {
    enum options { tm1, tm2, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<tx_mode_ul_r10_opts> tx_mode_ul_r10_e_;

  // member variables
  bool              tx_mode_ul_r10_present          = false;
  bool              four_ant_port_activ_r10_present = false;
  tx_mode_ul_r10_e_ tx_mode_ul_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ant_info_ul_r10_s& other) const;
  bool        operator!=(const ant_info_ul_r10_s& other) const { return not(*this == other); }
};

// CFI-Config-r15 ::= SEQUENCE
struct cfi_cfg_r15_s {
  bool    cfi_sf_non_mbsfn_r15_present           = false;
  bool    cfi_slot_subslot_non_mbsfn_r15_present = false;
  bool    cfi_sf_mbsfn_r15_present               = false;
  bool    cfi_slot_subslot_mbsfn_r15_present     = false;
  uint8_t cfi_sf_non_mbsfn_r15                   = 1;
  uint8_t cfi_slot_subslot_non_mbsfn_r15         = 1;
  uint8_t cfi_sf_mbsfn_r15                       = 1;
  uint8_t cfi_slot_subslot_mbsfn_r15             = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cfi_cfg_r15_s& other) const;
  bool        operator!=(const cfi_cfg_r15_s& other) const { return not(*this == other); }
};

// CFI-PatternConfig-r15 ::= SEQUENCE
struct cfi_pattern_cfg_r15_s {
  using cfi_pattern_sf_r15_l_           = std::array<uint8_t, 10>;
  using cfi_pattern_slot_subslot_r15_l_ = std::array<uint8_t, 10>;

  // member variables
  bool                            cfi_pattern_sf_r15_present           = false;
  bool                            cfi_pattern_slot_subslot_r15_present = false;
  cfi_pattern_sf_r15_l_           cfi_pattern_sf_r15;
  cfi_pattern_slot_subslot_r15_l_ cfi_pattern_slot_subslot_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cfi_pattern_cfg_r15_s& other) const;
  bool        operator!=(const cfi_pattern_cfg_r15_s& other) const { return not(*this == other); }
};

// CQI-ReportConfig ::= SEQUENCE
struct cqi_report_cfg_s {
  bool                        cqi_report_mode_aperiodic_present = false;
  bool                        cqi_report_periodic_present       = false;
  cqi_report_mode_aperiodic_e cqi_report_mode_aperiodic;
  int8_t                      nom_pdsch_rs_epre_offset = -1;
  cqi_report_periodic_c       cqi_report_periodic;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CQI-ReportConfig-v1530 ::= SEQUENCE
struct cqi_report_cfg_v1530_s {
  struct alt_cqi_table_minus1024_qam_r15_opts {
    enum options { all_sfs, csi_sf_set1, csi_sf_set2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<alt_cqi_table_minus1024_qam_r15_opts> alt_cqi_table_minus1024_qam_r15_e_;

  // member variables
  bool                               alt_cqi_table_minus1024_qam_r15_present = false;
  alt_cqi_table_minus1024_qam_r15_e_ alt_cqi_table_minus1024_qam_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_cfg_v1530_s& other) const;
  bool        operator!=(const cqi_report_cfg_v1530_s& other) const { return not(*this == other); }
};

// CQI-ReportConfig-v920 ::= SEQUENCE
struct cqi_report_cfg_v920_s {
  bool cqi_mask_r9_present      = false;
  bool pmi_ri_report_r9_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-Config-v1480 ::= SEQUENCE
struct csi_rs_cfg_v1480_s {
  bool                     emimo_type_v1480_present = false;
  csi_rs_cfg_emimo_v1480_c emimo_type_v1480;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_v1480_s& other) const;
  bool        operator!=(const csi_rs_cfg_v1480_s& other) const { return not(*this == other); }
};

// CSI-RS-Config-v1530 ::= SEQUENCE
struct csi_rs_cfg_v1530_s {
  bool                     emimo_type_v1530_present = false;
  csi_rs_cfg_emimo_v1530_c emimo_type_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csi_rs_cfg_v1530_s& other) const;
  bool        operator!=(const csi_rs_cfg_v1530_s& other) const { return not(*this == other); }
};

// CSI-RS-ConfigNZPToAddModList-r11 ::= SEQUENCE (SIZE (1..3)) OF CSI-RS-ConfigNZP-r11
using csi_rs_cfg_nzp_to_add_mod_list_r11_l = dyn_array<csi_rs_cfg_nzp_r11_s>;

// CSI-RS-ConfigNZPToAddModListExt-r13 ::= SEQUENCE (SIZE (1..21)) OF CSI-RS-ConfigNZP-r11
using csi_rs_cfg_nzp_to_add_mod_list_ext_r13_l = dyn_array<csi_rs_cfg_nzp_r11_s>;

// CSI-RS-ConfigNZPToReleaseList-r11 ::= SEQUENCE (SIZE (1..3)) OF INTEGER (1..3)
using csi_rs_cfg_nzp_to_release_list_r11_l = bounded_array<uint8_t, 3>;

// CSI-RS-ConfigNZPToReleaseListExt-r13 ::= SEQUENCE (SIZE (1..21)) OF INTEGER (4..24)
using csi_rs_cfg_nzp_to_release_list_ext_r13_l = bounded_array<uint8_t, 21>;

// EPDCCH-Config-r11 ::= SEQUENCE
struct epdcch_cfg_r11_s {
  struct cfg_r11_c_ {
    struct setup_s_ {
      struct sf_pattern_cfg_r11_c_ {
        struct setup_s_ {
          meas_sf_pattern_r10_c sf_pattern_r11;
        };
        using types = setup_e;

        // choice methods
        sf_pattern_cfg_r11_c_() = default;
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        bool        operator==(const sf_pattern_cfg_r11_c_& other) const;
        bool        operator!=(const sf_pattern_cfg_r11_c_& other) const { return not(*this == other); }
        // getters
        setup_s_& setup()
        {
          assert_choice_type(types::setup, type_, "subframePatternConfig-r11");
          return c;
        }
        const setup_s_& setup() const
        {
          assert_choice_type(types::setup, type_, "subframePatternConfig-r11");
          return c;
        }
        void      set_release();
        setup_s_& set_setup();

      private:
        types    type_;
        setup_s_ c;
      };

      // member variables
      bool                                 sf_pattern_cfg_r11_present          = false;
      bool                                 start_symbol_r11_present            = false;
      bool                                 set_cfg_to_release_list_r11_present = false;
      bool                                 set_cfg_to_add_mod_list_r11_present = false;
      sf_pattern_cfg_r11_c_                sf_pattern_cfg_r11;
      uint8_t                              start_symbol_r11 = 1;
      epdcch_set_cfg_to_release_list_r11_l set_cfg_to_release_list_r11;
      epdcch_set_cfg_to_add_mod_list_r11_l set_cfg_to_add_mod_list_r11;
    };
    using types = setup_e;

    // choice methods
    cfg_r11_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const cfg_r11_c_& other) const;
    bool        operator!=(const cfg_r11_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "config-r11");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "config-r11");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  cfg_r11_c_ cfg_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const epdcch_cfg_r11_s& other) const;
  bool        operator!=(const epdcch_cfg_r11_s& other) const { return not(*this == other); }
};

// PDCCH-CandidateReductions-r13 ::= CHOICE
struct pdcch_candidate_reductions_r13_c {
  struct setup_s_ {
    pdcch_candidate_reduction_value_r13_e pdcch_candidate_reduction_al1_r13;
    pdcch_candidate_reduction_value_r13_e pdcch_candidate_reduction_al2_r13;
    pdcch_candidate_reduction_value_r13_e pdcch_candidate_reduction_al3_r13;
    pdcch_candidate_reduction_value_r13_e pdcch_candidate_reduction_al4_r13;
    pdcch_candidate_reduction_value_r13_e pdcch_candidate_reduction_al5_r13;
  };
  using types = setup_e;

  // choice methods
  pdcch_candidate_reductions_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdcch_candidate_reductions_r13_c& other) const;
  bool        operator!=(const pdcch_candidate_reductions_r13_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "PDCCH-CandidateReductions-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "PDCCH-CandidateReductions-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// PDSCH-ConfigDedicated ::= SEQUENCE
struct pdsch_cfg_ded_s {
  struct p_a_opts {
    enum options { db_minus6, db_minus4dot77, db_minus3, db_minus1dot77, db0, db1, db2, db3, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<p_a_opts> p_a_e_;

  // member variables
  p_a_e_ p_a;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdsch_cfg_ded_s& other) const;
  bool        operator!=(const pdsch_cfg_ded_s& other) const { return not(*this == other); }
};

// PDSCH-ConfigDedicated-v1130 ::= SEQUENCE
struct pdsch_cfg_ded_v1130_s {
  struct qcl_operation_opts {
    enum options { type_a, type_b, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<qcl_operation_opts> qcl_operation_e_;

  // member variables
  bool                                 dmrs_cfg_pdsch_r11_present                 = false;
  bool                                 qcl_operation_present                      = false;
  bool                                 re_map_qcl_cfg_to_release_list_r11_present = false;
  bool                                 re_map_qcl_cfg_to_add_mod_list_r11_present = false;
  dmrs_cfg_r11_c                       dmrs_cfg_pdsch_r11;
  qcl_operation_e_                     qcl_operation;
  re_map_qcl_cfg_to_release_list_r11_l re_map_qcl_cfg_to_release_list_r11;
  re_map_qcl_cfg_to_add_mod_list_r11_l re_map_qcl_cfg_to_add_mod_list_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdsch_cfg_ded_v1130_s& other) const;
  bool        operator!=(const pdsch_cfg_ded_v1130_s& other) const { return not(*this == other); }
};

// PDSCH-ConfigDedicated-v1280 ::= SEQUENCE
struct pdsch_cfg_ded_v1280_s {
  struct tbs_idx_alt_r12_opts {
    enum options { a26, a33, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<tbs_idx_alt_r12_opts> tbs_idx_alt_r12_e_;

  // member variables
  bool               tbs_idx_alt_r12_present = false;
  tbs_idx_alt_r12_e_ tbs_idx_alt_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdsch_cfg_ded_v1280_s& other) const;
  bool        operator!=(const pdsch_cfg_ded_v1280_s& other) const { return not(*this == other); }
};

// PDSCH-ConfigDedicated-v1310 ::= SEQUENCE
struct pdsch_cfg_ded_v1310_s {
  bool             dmrs_cfg_pdsch_v1310_present = false;
  dmrs_cfg_v1310_s dmrs_cfg_pdsch_v1310;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdsch_cfg_ded_v1310_s& other) const;
  bool        operator!=(const pdsch_cfg_ded_v1310_s& other) const { return not(*this == other); }
};

// PDSCH-ConfigDedicated-v1430 ::= SEQUENCE
struct pdsch_cfg_ded_v1430_s {
  struct ce_pdsch_max_bw_r14_opts {
    enum options { bw5, bw20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ce_pdsch_max_bw_r14_opts> ce_pdsch_max_bw_r14_e_;
  struct ce_sched_enhancement_r14_opts {
    enum options { range1, range2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ce_sched_enhancement_r14_opts> ce_sched_enhancement_r14_e_;

  // member variables
  bool                        ce_pdsch_max_bw_r14_present        = false;
  bool                        ce_pdsch_ten_processes_r14_present = false;
  bool                        ce_harq_ack_bundling_r14_present   = false;
  bool                        ce_sched_enhancement_r14_present   = false;
  bool                        tbs_idx_alt2_r14_present           = false;
  ce_pdsch_max_bw_r14_e_      ce_pdsch_max_bw_r14;
  ce_sched_enhancement_r14_e_ ce_sched_enhancement_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-ConfigDedicated-v1530 ::= SEQUENCE
struct pdsch_cfg_ded_v1530_s {
  struct alt_mcs_table_scaling_cfg_r15_opts {
    enum options { odot5, odot625, odot75, odot875, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<alt_mcs_table_scaling_cfg_r15_opts> alt_mcs_table_scaling_cfg_r15_e_;

  // member variables
  bool                             qcl_operation_v1530_present                   = false;
  bool                             tbs_idx_alt3_r15_present                      = false;
  bool                             ce_cqi_alt_table_cfg_r15_present              = false;
  bool                             ce_pdsch_minus64_qam_cfg_r15_present          = false;
  bool                             ce_pdsch_flex_start_prb_alloc_cfg_r15_present = false;
  bool                             alt_mcs_table_scaling_cfg_r15_present         = false;
  alt_mcs_table_scaling_cfg_r15_e_ alt_mcs_table_scaling_cfg_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdsch_cfg_ded_v1530_s& other) const;
  bool        operator!=(const pdsch_cfg_ded_v1530_s& other) const { return not(*this == other); }
};

// PDSCH-ConfigDedicated-v1610 ::= SEQUENCE
struct pdsch_cfg_ded_v1610_s {
  setup_release_c<ce_pdsch_multi_tb_cfg_r16_s> ce_pdsch_multi_tb_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-ConfigDedicated-v1700 ::= SEQUENCE
struct pdsch_cfg_ded_v1700_s {
  bool                                             ce_pdsch_minus14_harq_cfg_r17_present = false;
  bool                                             ce_pdsch_max_tbs_r17_present          = false;
  setup_release_c<ce_pdsch_minus14_harq_cfg_r17_s> ce_pdsch_minus14_harq_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ConfigDedicated ::= SEQUENCE
struct pucch_cfg_ded_s {
  struct ack_nack_repeat_c_ {
    struct setup_s_ {
      struct repeat_factor_opts {
        enum options { n2, n4, n6, spare1, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<repeat_factor_opts> repeat_factor_e_;

      // member variables
      repeat_factor_e_ repeat_factor;
      uint16_t         n1_pucch_an_rep = 0;
    };
    using types = setup_e;

    // choice methods
    ack_nack_repeat_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "ackNackRepetition");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "ackNackRepetition");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct tdd_ack_nack_feedback_mode_opts {
    enum options { bundling, mux, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<tdd_ack_nack_feedback_mode_opts> tdd_ack_nack_feedback_mode_e_;

  // member variables
  bool                          tdd_ack_nack_feedback_mode_present = false;
  ack_nack_repeat_c_            ack_nack_repeat;
  tdd_ack_nack_feedback_mode_e_ tdd_ack_nack_feedback_mode;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ConfigDedicated-r13 ::= SEQUENCE
struct pucch_cfg_ded_r13_s {
  struct ack_nack_repeat_r13_c_ {
    struct setup_s_ {
      struct repeat_factor_r13_opts {
        enum options { n2, n4, n6, spare1, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<repeat_factor_r13_opts> repeat_factor_r13_e_;

      // member variables
      repeat_factor_r13_e_ repeat_factor_r13;
      uint16_t             n1_pucch_an_rep_r13 = 0;
    };
    using types = setup_e;

    // choice methods
    ack_nack_repeat_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const ack_nack_repeat_r13_c_& other) const;
    bool        operator!=(const ack_nack_repeat_r13_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "ackNackRepetition-r13");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "ackNackRepetition-r13");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct tdd_ack_nack_feedback_mode_r13_opts {
    enum options { bundling, mux, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<tdd_ack_nack_feedback_mode_r13_opts> tdd_ack_nack_feedback_mode_r13_e_;
  struct pucch_format_r13_c_ {
    struct format3_r13_s_ {
      using n3_pucch_an_list_r13_l_ = bounded_array<uint16_t, 4>;
      struct two_ant_port_activ_pucch_format3_r13_c_ {
        struct setup_s_ {
          using n3_pucch_an_list_p1_r13_l_ = bounded_array<uint16_t, 4>;

          // member variables
          n3_pucch_an_list_p1_r13_l_ n3_pucch_an_list_p1_r13;
        };
        using types = setup_e;

        // choice methods
        two_ant_port_activ_pucch_format3_r13_c_() = default;
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        bool        operator==(const two_ant_port_activ_pucch_format3_r13_c_& other) const;
        bool operator!=(const two_ant_port_activ_pucch_format3_r13_c_& other) const { return not(*this == other); }
        // getters
        setup_s_& setup()
        {
          assert_choice_type(types::setup, type_, "twoAntennaPortActivatedPUCCH-Format3-r13");
          return c;
        }
        const setup_s_& setup() const
        {
          assert_choice_type(types::setup, type_, "twoAntennaPortActivatedPUCCH-Format3-r13");
          return c;
        }
        void      set_release();
        setup_s_& set_setup();

      private:
        types    type_;
        setup_s_ c;
      };

      // member variables
      bool                                    n3_pucch_an_list_r13_present                 = false;
      bool                                    two_ant_port_activ_pucch_format3_r13_present = false;
      n3_pucch_an_list_r13_l_                 n3_pucch_an_list_r13;
      two_ant_port_activ_pucch_format3_r13_c_ two_ant_port_activ_pucch_format3_r13;
    };
    struct ch_sel_r13_s_ {
      struct n1_pucch_an_cs_r13_c_ {
        struct setup_s_ {
          using n1_pucch_an_cs_list_r13_l_ = dyn_array<n1_pucch_an_cs_r10_l>;
          using dummy1_l_                  = bounded_array<uint16_t, 4>;

          // member variables
          n1_pucch_an_cs_list_r13_l_ n1_pucch_an_cs_list_r13;
          dummy1_l_                  dummy1;
        };
        using types = setup_e;

        // choice methods
        n1_pucch_an_cs_r13_c_() = default;
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        bool        operator==(const n1_pucch_an_cs_r13_c_& other) const;
        bool        operator!=(const n1_pucch_an_cs_r13_c_& other) const { return not(*this == other); }
        // getters
        setup_s_& setup()
        {
          assert_choice_type(types::setup, type_, "n1PUCCH-AN-CS-r13");
          return c;
        }
        const setup_s_& setup() const
        {
          assert_choice_type(types::setup, type_, "n1PUCCH-AN-CS-r13");
          return c;
        }
        void      set_release();
        setup_s_& set_setup();

      private:
        types    type_;
        setup_s_ c;
      };

      // member variables
      bool                  n1_pucch_an_cs_r13_present = false;
      n1_pucch_an_cs_r13_c_ n1_pucch_an_cs_r13;
    };
    struct format4_r13_s_ {
      using format4_res_cfg_r13_l_           = std::array<format4_res_r13_s, 4>;
      using format4_multi_csi_res_cfg_r13_l_ = dyn_array<format4_res_r13_s>;

      // member variables
      bool                             format4_multi_csi_res_cfg_r13_present = false;
      format4_res_cfg_r13_l_           format4_res_cfg_r13;
      format4_multi_csi_res_cfg_r13_l_ format4_multi_csi_res_cfg_r13;
    };
    struct format5_r13_s_ {
      using format5_res_cfg_r13_l_ = std::array<format5_res_r13_s, 4>;

      // member variables
      bool                   format5_multi_csi_res_cfg_r13_present = false;
      format5_res_cfg_r13_l_ format5_res_cfg_r13;
      format5_res_r13_s      format5_multi_csi_res_cfg_r13;
    };
    struct types_opts {
      enum options { format3_r13, ch_sel_r13, format4_r13, format5_r13, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    pucch_format_r13_c_() = default;
    pucch_format_r13_c_(const pucch_format_r13_c_& other);
    pucch_format_r13_c_& operator=(const pucch_format_r13_c_& other);
    ~pucch_format_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const pucch_format_r13_c_& other) const;
    bool        operator!=(const pucch_format_r13_c_& other) const { return not(*this == other); }
    // getters
    format3_r13_s_& format3_r13()
    {
      assert_choice_type(types::format3_r13, type_, "pucch-Format-r13");
      return c.get<format3_r13_s_>();
    }
    ch_sel_r13_s_& ch_sel_r13()
    {
      assert_choice_type(types::ch_sel_r13, type_, "pucch-Format-r13");
      return c.get<ch_sel_r13_s_>();
    }
    format4_r13_s_& format4_r13()
    {
      assert_choice_type(types::format4_r13, type_, "pucch-Format-r13");
      return c.get<format4_r13_s_>();
    }
    format5_r13_s_& format5_r13()
    {
      assert_choice_type(types::format5_r13, type_, "pucch-Format-r13");
      return c.get<format5_r13_s_>();
    }
    const format3_r13_s_& format3_r13() const
    {
      assert_choice_type(types::format3_r13, type_, "pucch-Format-r13");
      return c.get<format3_r13_s_>();
    }
    const ch_sel_r13_s_& ch_sel_r13() const
    {
      assert_choice_type(types::ch_sel_r13, type_, "pucch-Format-r13");
      return c.get<ch_sel_r13_s_>();
    }
    const format4_r13_s_& format4_r13() const
    {
      assert_choice_type(types::format4_r13, type_, "pucch-Format-r13");
      return c.get<format4_r13_s_>();
    }
    const format5_r13_s_& format5_r13() const
    {
      assert_choice_type(types::format5_r13, type_, "pucch-Format-r13");
      return c.get<format5_r13_s_>();
    }
    format3_r13_s_& set_format3_r13();
    ch_sel_r13_s_&  set_ch_sel_r13();
    format4_r13_s_& set_format4_r13();
    format5_r13_s_& set_format5_r13();

  private:
    types                                                                          type_;
    choice_buffer_t<ch_sel_r13_s_, format3_r13_s_, format4_r13_s_, format5_r13_s_> c;

    void destroy_();
  };
  struct npucch_param_r13_c_ {
    struct setup_s_ {
      uint16_t npucch_id_r13   = 0;
      uint16_t n1_pucch_an_r13 = 0;
    };
    using types = setup_e;

    // choice methods
    npucch_param_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const npucch_param_r13_c_& other) const;
    bool        operator!=(const npucch_param_r13_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "nPUCCH-Param-r13");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "nPUCCH-Param-r13");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct nka_pucch_param_r13_c_ {
    struct setup_s_ {
      uint16_t nka_pucch_an_r13 = 0;
    };
    using types = setup_e;

    // choice methods
    nka_pucch_param_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const nka_pucch_param_r13_c_& other) const;
    bool        operator!=(const nka_pucch_param_r13_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "nkaPUCCH-Param-r13");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "nkaPUCCH-Param-r13");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct codebooksize_determination_r13_opts {
    enum options { dai, cc, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<codebooksize_determination_r13_opts> codebooksize_determination_r13_e_;
  struct pucch_num_repeat_ce_r13_c_ {
    struct setup_c_ {
      struct mode_a_s_ {
        struct pucch_num_repeat_ce_format1_r13_opts {
          enum options { r1, r2, r4, r8, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<pucch_num_repeat_ce_format1_r13_opts> pucch_num_repeat_ce_format1_r13_e_;
        struct pucch_num_repeat_ce_format2_r13_opts {
          enum options { r1, r2, r4, r8, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<pucch_num_repeat_ce_format2_r13_opts> pucch_num_repeat_ce_format2_r13_e_;

        // member variables
        pucch_num_repeat_ce_format1_r13_e_ pucch_num_repeat_ce_format1_r13;
        pucch_num_repeat_ce_format2_r13_e_ pucch_num_repeat_ce_format2_r13;
      };
      struct mode_b_s_ {
        struct pucch_num_repeat_ce_format1_r13_opts {
          enum options { r4, r8, r16, r32, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<pucch_num_repeat_ce_format1_r13_opts> pucch_num_repeat_ce_format1_r13_e_;
        struct pucch_num_repeat_ce_format2_r13_opts {
          enum options { r4, r8, r16, r32, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<pucch_num_repeat_ce_format2_r13_opts> pucch_num_repeat_ce_format2_r13_e_;

        // member variables
        pucch_num_repeat_ce_format1_r13_e_ pucch_num_repeat_ce_format1_r13;
        pucch_num_repeat_ce_format2_r13_e_ pucch_num_repeat_ce_format2_r13;
      };
      struct types_opts {
        enum options { mode_a, mode_b, nulltype } value;

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
      bool        operator==(const setup_c_& other) const;
      bool        operator!=(const setup_c_& other) const { return not(*this == other); }
      // getters
      mode_a_s_& mode_a()
      {
        assert_choice_type(types::mode_a, type_, "setup");
        return c.get<mode_a_s_>();
      }
      mode_b_s_& mode_b()
      {
        assert_choice_type(types::mode_b, type_, "setup");
        return c.get<mode_b_s_>();
      }
      const mode_a_s_& mode_a() const
      {
        assert_choice_type(types::mode_a, type_, "setup");
        return c.get<mode_a_s_>();
      }
      const mode_b_s_& mode_b() const
      {
        assert_choice_type(types::mode_b, type_, "setup");
        return c.get<mode_b_s_>();
      }
      mode_a_s_& set_mode_a();
      mode_b_s_& set_mode_b();

    private:
      types                                 type_;
      choice_buffer_t<mode_a_s_, mode_b_s_> c;

      void destroy_();
    };
    using types = setup_e;

    // choice methods
    pucch_num_repeat_ce_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const pucch_num_repeat_ce_r13_c_& other) const;
    bool        operator!=(const pucch_num_repeat_ce_r13_c_& other) const { return not(*this == other); }
    // getters
    setup_c_& setup()
    {
      assert_choice_type(types::setup, type_, "pucch-NumRepetitionCE-r13");
      return c;
    }
    const setup_c_& setup() const
    {
      assert_choice_type(types::setup, type_, "pucch-NumRepetitionCE-r13");
      return c;
    }
    void      set_release();
    setup_c_& set_setup();

  private:
    types    type_;
    setup_c_ c;
  };

  // member variables
  bool                              tdd_ack_nack_feedback_mode_r13_present          = false;
  bool                              pucch_format_r13_present                        = false;
  bool                              two_ant_port_activ_pucch_format1a1b_r13_present = false;
  bool                              simul_pucch_pusch_r13_present                   = false;
  bool                              n1_pucch_an_rep_p1_r13_present                  = false;
  bool                              npucch_param_r13_present                        = false;
  bool                              nka_pucch_param_r13_present                     = false;
  bool                              codebooksize_determination_r13_present          = false;
  bool                              maximum_payload_coderate_r13_present            = false;
  bool                              pucch_num_repeat_ce_r13_present                 = false;
  ack_nack_repeat_r13_c_            ack_nack_repeat_r13;
  tdd_ack_nack_feedback_mode_r13_e_ tdd_ack_nack_feedback_mode_r13;
  pucch_format_r13_c_               pucch_format_r13;
  uint16_t                          n1_pucch_an_rep_p1_r13 = 0;
  npucch_param_r13_c_               npucch_param_r13;
  nka_pucch_param_r13_c_            nka_pucch_param_r13;
  bool                              spatial_bundling_pucch_r13 = false;
  bool                              spatial_bundling_pusch_r13 = false;
  bool                              harq_timing_tdd_r13        = false;
  codebooksize_determination_r13_e_ codebooksize_determination_r13;
  uint8_t                           maximum_payload_coderate_r13 = 0;
  pucch_num_repeat_ce_r13_c_        pucch_num_repeat_ce_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pucch_cfg_ded_r13_s& other) const;
  bool        operator!=(const pucch_cfg_ded_r13_s& other) const { return not(*this == other); }
};

// PUCCH-ConfigDedicated-v1020 ::= SEQUENCE
struct pucch_cfg_ded_v1020_s {
  struct pucch_format_r10_c_ {
    struct ch_sel_r10_s_ {
      struct n1_pucch_an_cs_r10_c_ {
        struct setup_s_ {
          using n1_pucch_an_cs_list_r10_l_ = dyn_array<n1_pucch_an_cs_r10_l>;

          // member variables
          n1_pucch_an_cs_list_r10_l_ n1_pucch_an_cs_list_r10;
        };
        using types = setup_e;

        // choice methods
        n1_pucch_an_cs_r10_c_() = default;
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        setup_s_& setup()
        {
          assert_choice_type(types::setup, type_, "n1PUCCH-AN-CS-r10");
          return c;
        }
        const setup_s_& setup() const
        {
          assert_choice_type(types::setup, type_, "n1PUCCH-AN-CS-r10");
          return c;
        }
        void      set_release();
        setup_s_& set_setup();

      private:
        types    type_;
        setup_s_ c;
      };

      // member variables
      bool                  n1_pucch_an_cs_r10_present = false;
      n1_pucch_an_cs_r10_c_ n1_pucch_an_cs_r10;
    };
    struct types_opts {
      enum options { format3_r10, ch_sel_r10, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    pucch_format_r10_c_() = default;
    pucch_format_r10_c_(const pucch_format_r10_c_& other);
    pucch_format_r10_c_& operator=(const pucch_format_r10_c_& other);
    ~pucch_format_r10_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pucch_format3_conf_r13_s& format3_r10()
    {
      assert_choice_type(types::format3_r10, type_, "pucch-Format-r10");
      return c.get<pucch_format3_conf_r13_s>();
    }
    ch_sel_r10_s_& ch_sel_r10()
    {
      assert_choice_type(types::ch_sel_r10, type_, "pucch-Format-r10");
      return c.get<ch_sel_r10_s_>();
    }
    const pucch_format3_conf_r13_s& format3_r10() const
    {
      assert_choice_type(types::format3_r10, type_, "pucch-Format-r10");
      return c.get<pucch_format3_conf_r13_s>();
    }
    const ch_sel_r10_s_& ch_sel_r10() const
    {
      assert_choice_type(types::ch_sel_r10, type_, "pucch-Format-r10");
      return c.get<ch_sel_r10_s_>();
    }
    pucch_format3_conf_r13_s& set_format3_r10();
    ch_sel_r10_s_&            set_ch_sel_r10();

  private:
    types                                                    type_;
    choice_buffer_t<ch_sel_r10_s_, pucch_format3_conf_r13_s> c;

    void destroy_();
  };

  // member variables
  bool                pucch_format_r10_present                        = false;
  bool                two_ant_port_activ_pucch_format1a1b_r10_present = false;
  bool                simul_pucch_pusch_r10_present                   = false;
  bool                n1_pucch_an_rep_p1_r10_present                  = false;
  pucch_format_r10_c_ pucch_format_r10;
  uint16_t            n1_pucch_an_rep_p1_r10 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ConfigDedicated-v1130 ::= SEQUENCE
struct pucch_cfg_ded_v1130_s {
  struct n1_pucch_an_cs_v1130_c_ {
    struct setup_s_ {
      using n1_pucch_an_cs_list_p1_r11_l_ = bounded_array<uint16_t, 4>;

      // member variables
      n1_pucch_an_cs_list_p1_r11_l_ n1_pucch_an_cs_list_p1_r11;
    };
    using types = setup_e;

    // choice methods
    n1_pucch_an_cs_v1130_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "n1PUCCH-AN-CS-v1130");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "n1PUCCH-AN-CS-v1130");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct npucch_param_r11_c_ {
    struct setup_s_ {
      uint16_t npucch_id_r11   = 0;
      uint16_t n1_pucch_an_r11 = 0;
    };
    using types = setup_e;

    // choice methods
    npucch_param_r11_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "nPUCCH-Param-r11");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "nPUCCH-Param-r11");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                    n1_pucch_an_cs_v1130_present = false;
  bool                    npucch_param_r11_present     = false;
  n1_pucch_an_cs_v1130_c_ n1_pucch_an_cs_v1130;
  npucch_param_r11_c_     npucch_param_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ConfigDedicated-v1250 ::= SEQUENCE
struct pucch_cfg_ded_v1250_s {
  struct nka_pucch_param_r12_c_ {
    struct setup_s_ {
      uint16_t nka_pucch_an_r12 = 0;
    };
    using types = setup_e;

    // choice methods
    nka_pucch_param_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "nkaPUCCH-Param-r12");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "nkaPUCCH-Param-r12");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  nka_pucch_param_r12_c_ nka_pucch_param_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ConfigDedicated-v1430 ::= SEQUENCE
struct pucch_cfg_ded_v1430_s {
  struct pucch_num_repeat_ce_format1_r14_opts {
    enum options { r64, r128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pucch_num_repeat_ce_format1_r14_opts> pucch_num_repeat_ce_format1_r14_e_;

  // member variables
  bool                               pucch_num_repeat_ce_format1_r14_present = false;
  pucch_num_repeat_ce_format1_r14_e_ pucch_num_repeat_ce_format1_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-ConfigDedicated ::= SEQUENCE
struct pusch_cfg_ded_s {
  uint8_t beta_offset_ack_idx = 0;
  uint8_t beta_offset_ri_idx  = 0;
  uint8_t beta_offset_cqi_idx = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-ConfigDedicated-r13 ::= SEQUENCE
struct pusch_cfg_ded_r13_s {
  struct beta_offset_mc_r13_s_ {
    bool    beta_offset2_ack_idx_mc_r13_present = false;
    uint8_t beta_offset_ack_idx_mc_r13          = 0;
    uint8_t beta_offset2_ack_idx_mc_r13         = 0;
    uint8_t beta_offset_ri_idx_mc_r13           = 0;
    uint8_t beta_offset_cqi_idx_mc_r13          = 0;
  };
  struct pusch_dmrs_r11_c_ {
    struct setup_s_ {
      uint16_t npusch_id_r13    = 0;
      uint16_t ndmrs_csh_id_r13 = 0;
    };
    using types = setup_e;

    // choice methods
    pusch_dmrs_r11_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const pusch_dmrs_r11_c_& other) const;
    bool        operator!=(const pusch_dmrs_r11_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "pusch-DMRS-r11");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "pusch-DMRS-r11");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct uci_on_pusch_c_ {
    struct setup_s_ {
      struct beta_offset_mc_r12_s_ {
        bool    beta_offset2_ack_idx_mc_sf_set2_r13_present = false;
        uint8_t beta_offset_ack_idx_mc_sf_set2_r13          = 0;
        uint8_t beta_offset2_ack_idx_mc_sf_set2_r13         = 0;
        uint8_t beta_offset_ri_idx_mc_sf_set2_r13           = 0;
        uint8_t beta_offset_cqi_idx_mc_sf_set2_r13          = 0;
      };

      // member variables
      bool                  beta_offset2_ack_idx_sf_set2_r13_present = false;
      bool                  beta_offset_mc_r12_present               = false;
      uint8_t               beta_offset_ack_idx_sf_set2_r13          = 0;
      uint8_t               beta_offset2_ack_idx_sf_set2_r13         = 0;
      uint8_t               beta_offset_ri_idx_sf_set2_r13           = 0;
      uint8_t               beta_offset_cqi_idx_sf_set2_r13          = 0;
      beta_offset_mc_r12_s_ beta_offset_mc_r12;
    };
    using types = setup_e;

    // choice methods
    uci_on_pusch_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const uci_on_pusch_c_& other) const;
    bool        operator!=(const uci_on_pusch_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "uciOnPUSCH");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "uciOnPUSCH");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                  beta_offset2_ack_idx_r13_present = false;
  bool                  beta_offset_mc_r13_present       = false;
  bool                  group_hop_disabled_r13_present   = false;
  bool                  dmrs_with_occ_activ_r13_present  = false;
  bool                  pusch_dmrs_r11_present           = false;
  bool                  uci_on_pusch_present             = false;
  bool                  pusch_hop_cfg_r13_present        = false;
  uint8_t               beta_offset_ack_idx_r13          = 0;
  uint8_t               beta_offset2_ack_idx_r13         = 0;
  uint8_t               beta_offset_ri_idx_r13           = 0;
  uint8_t               beta_offset_cqi_idx_r13          = 0;
  beta_offset_mc_r13_s_ beta_offset_mc_r13;
  pusch_dmrs_r11_c_     pusch_dmrs_r11;
  uci_on_pusch_c_       uci_on_pusch;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pusch_cfg_ded_r13_s& other) const;
  bool        operator!=(const pusch_cfg_ded_r13_s& other) const { return not(*this == other); }
};

// PUSCH-ConfigDedicated-v1020 ::= SEQUENCE
struct pusch_cfg_ded_v1020_s {
  struct beta_offset_mc_r10_s_ {
    uint8_t beta_offset_ack_idx_mc_r10 = 0;
    uint8_t beta_offset_ri_idx_mc_r10  = 0;
    uint8_t beta_offset_cqi_idx_mc_r10 = 0;
  };

  // member variables
  bool                  beta_offset_mc_r10_present      = false;
  bool                  group_hop_disabled_r10_present  = false;
  bool                  dmrs_with_occ_activ_r10_present = false;
  beta_offset_mc_r10_s_ beta_offset_mc_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-ConfigDedicated-v1130 ::= SEQUENCE
struct pusch_cfg_ded_v1130_s {
  struct pusch_dmrs_r11_c_ {
    struct setup_s_ {
      uint16_t npusch_id_r11    = 0;
      uint16_t ndmrs_csh_id_r11 = 0;
    };
    using types = setup_e;

    // choice methods
    pusch_dmrs_r11_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const pusch_dmrs_r11_c_& other) const;
    bool        operator!=(const pusch_dmrs_r11_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "pusch-DMRS-r11");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "pusch-DMRS-r11");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  pusch_dmrs_r11_c_ pusch_dmrs_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pusch_cfg_ded_v1130_s& other) const;
  bool        operator!=(const pusch_cfg_ded_v1130_s& other) const { return not(*this == other); }
};

// PUSCH-ConfigDedicated-v1250 ::= SEQUENCE
struct pusch_cfg_ded_v1250_s {
  struct uci_on_pusch_c_ {
    struct setup_s_ {
      struct beta_offset_mc_r12_s_ {
        uint8_t beta_offset_ack_idx_mc_sf_set2_r12 = 0;
        uint8_t beta_offset_ri_idx_mc_sf_set2_r12  = 0;
        uint8_t beta_offset_cqi_idx_mc_sf_set2_r12 = 0;
      };

      // member variables
      bool                  beta_offset_mc_r12_present      = false;
      uint8_t               beta_offset_ack_idx_sf_set2_r12 = 0;
      uint8_t               beta_offset_ri_idx_sf_set2_r12  = 0;
      uint8_t               beta_offset_cqi_idx_sf_set2_r12 = 0;
      beta_offset_mc_r12_s_ beta_offset_mc_r12;
    };
    using types = setup_e;

    // choice methods
    uci_on_pusch_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "uciOnPUSCH");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "uciOnPUSCH");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  uci_on_pusch_c_ uci_on_pusch;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-ConfigDedicated-v1430 ::= SEQUENCE
struct pusch_cfg_ded_v1430_s {
  bool                   ce_pusch_nb_max_tbs_r14_present = false;
  bool                   ce_pusch_max_bw_r14_present     = false;
  bool                   tdd_pusch_up_pts_r14_present    = false;
  bool                   enable256_qam_r14_present       = false;
  tdd_pusch_up_pts_r14_c tdd_pusch_up_pts_r14;
  bool                   ul_dmrs_ifdma_r14 = false;
  enable256_qam_r14_c    enable256_qam_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-ConfigDedicated-v1530 ::= SEQUENCE
struct pusch_cfg_ded_v1530_s {
  struct ce_pusch_flex_start_prb_alloc_cfg_r15_c_ {
    struct setup_s_ {
      bool   offset_ce_mode_b_r15_present = false;
      int8_t offset_ce_mode_b_r15         = -1;
    };
    using types = setup_e;

    // choice methods
    ce_pusch_flex_start_prb_alloc_cfg_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "ce-PUSCH-FlexibleStartPRB-AllocConfig-r15");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "ce-PUSCH-FlexibleStartPRB-AllocConfig-r15");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct ce_pusch_sub_prb_cfg_r15_c_ {
    struct setup_s_ {
      bool    location_ce_mode_b_r15_present = false;
      uint8_t location_ce_mode_b_r15         = 0;
      uint8_t six_tone_cyclic_shift_r15      = 0;
      uint8_t three_tone_cyclic_shift_r15    = 0;
    };
    using types = setup_e;

    // choice methods
    ce_pusch_sub_prb_cfg_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "ce-PUSCH-SubPRB-Config-r15");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "ce-PUSCH-SubPRB-Config-r15");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                                     ce_pusch_sub_prb_cfg_r15_present = false;
  ce_pusch_flex_start_prb_alloc_cfg_r15_c_ ce_pusch_flex_start_prb_alloc_cfg_r15;
  ce_pusch_sub_prb_cfg_r15_c_              ce_pusch_sub_prb_cfg_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-ConfigDedicated-v1610 ::= SEQUENCE
struct pusch_cfg_ded_v1610_s {
  setup_release_c<ce_pusch_multi_tb_cfg_r16_s> ce_pusch_multi_tb_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-EnhancementsConfig-r14 ::= CHOICE
struct pusch_enhance_cfg_r14_c {
  struct setup_s_ {
    struct interv_ul_hop_pusch_enh_r14_c_ {
      struct interv_fdd_pusch_enh_r14_opts {
        enum options { int1, int2, int4, int8, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<interv_fdd_pusch_enh_r14_opts> interv_fdd_pusch_enh_r14_e_;
      struct interv_tdd_pusch_enh_r14_opts {
        enum options { int1, int5, int10, int20, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<interv_tdd_pusch_enh_r14_opts> interv_tdd_pusch_enh_r14_e_;
      struct types_opts {
        enum options { interv_fdd_pusch_enh_r14, interv_tdd_pusch_enh_r14, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      interv_ul_hop_pusch_enh_r14_c_() = default;
      interv_ul_hop_pusch_enh_r14_c_(const interv_ul_hop_pusch_enh_r14_c_& other);
      interv_ul_hop_pusch_enh_r14_c_& operator=(const interv_ul_hop_pusch_enh_r14_c_& other);
      ~interv_ul_hop_pusch_enh_r14_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      interv_fdd_pusch_enh_r14_e_& interv_fdd_pusch_enh_r14()
      {
        assert_choice_type(types::interv_fdd_pusch_enh_r14, type_, "interval-ULHoppingPUSCH-Enh-r14");
        return c.get<interv_fdd_pusch_enh_r14_e_>();
      }
      interv_tdd_pusch_enh_r14_e_& interv_tdd_pusch_enh_r14()
      {
        assert_choice_type(types::interv_tdd_pusch_enh_r14, type_, "interval-ULHoppingPUSCH-Enh-r14");
        return c.get<interv_tdd_pusch_enh_r14_e_>();
      }
      const interv_fdd_pusch_enh_r14_e_& interv_fdd_pusch_enh_r14() const
      {
        assert_choice_type(types::interv_fdd_pusch_enh_r14, type_, "interval-ULHoppingPUSCH-Enh-r14");
        return c.get<interv_fdd_pusch_enh_r14_e_>();
      }
      const interv_tdd_pusch_enh_r14_e_& interv_tdd_pusch_enh_r14() const
      {
        assert_choice_type(types::interv_tdd_pusch_enh_r14, type_, "interval-ULHoppingPUSCH-Enh-r14");
        return c.get<interv_tdd_pusch_enh_r14_e_>();
      }
      interv_fdd_pusch_enh_r14_e_& set_interv_fdd_pusch_enh_r14();
      interv_tdd_pusch_enh_r14_e_& set_interv_tdd_pusch_enh_r14();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };

    // member variables
    bool                           pusch_hop_offset_pusch_enh_r14_present = false;
    bool                           interv_ul_hop_pusch_enh_r14_present    = false;
    uint8_t                        pusch_hop_offset_pusch_enh_r14         = 1;
    interv_ul_hop_pusch_enh_r14_c_ interv_ul_hop_pusch_enh_r14;
  };
  using types = setup_e;

  // choice methods
  pusch_enhance_cfg_r14_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "PUSCH-EnhancementsConfig-r14");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "PUSCH-EnhancementsConfig-r14");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// PhysicalConfigDedicatedSTTI-r15 ::= CHOICE
struct phys_cfg_ded_stti_r15_c {
  struct setup_s_ {
    bool                                 ant_info_ded_stti_r15_present              = false;
    bool                                 ant_info_ul_stti_r15_present               = false;
    bool                                 pucch_cfg_ded_v1530_present                = false;
    bool                                 sched_request_cfg_v1530_present            = false;
    bool                                 ul_pwr_ctrl_ded_stti_r15_present           = false;
    bool                                 cqi_report_cfg_r15_present                 = false;
    bool                                 csi_rs_cfg_r15_present                     = false;
    bool                                 csi_rs_cfg_nzp_to_release_list_r15_present = false;
    bool                                 csi_rs_cfg_nzp_to_add_mod_list_r15_present = false;
    bool                                 csi_rs_cfg_zp_to_release_list_r15_present  = false;
    bool                                 csi_rs_cfg_zp_to_add_mod_list_r11_present  = false;
    bool                                 csi_rs_cfg_zp_ap_list_r15_present          = false;
    bool                                 eimta_main_cfg_r12_present                 = false;
    bool                                 eimta_main_cfg_serv_cell_r15_present       = false;
    bool                                 slot_or_subslot_pdsch_cfg_r15_present      = false;
    bool                                 slot_or_subslot_pusch_cfg_r15_present      = false;
    bool                                 spdcch_cfg_r15_present                     = false;
    bool                                 spucch_cfg_r15_present                     = false;
    bool                                 short_tti_r15_present                      = false;
    ant_info_ded_stti_r15_c              ant_info_ded_stti_r15;
    ant_info_ul_stti_r15_s               ant_info_ul_stti_r15;
    pucch_cfg_ded_v1530_s                pucch_cfg_ded_v1530;
    sched_request_cfg_v1530_c            sched_request_cfg_v1530;
    ul_pwr_ctrl_ded_stti_r15_s           ul_pwr_ctrl_ded_stti_r15;
    cqi_report_cfg_r15_c                 cqi_report_cfg_r15;
    csi_rs_cfg_r15_c                     csi_rs_cfg_r15;
    csi_rs_cfg_nzp_to_release_list_r15_l csi_rs_cfg_nzp_to_release_list_r15;
    csi_rs_cfg_nzp_to_add_mod_list_r15_l csi_rs_cfg_nzp_to_add_mod_list_r15;
    csi_rs_cfg_zp_to_release_list_r11_l  csi_rs_cfg_zp_to_release_list_r15;
    csi_rs_cfg_zp_to_add_mod_list_r11_l  csi_rs_cfg_zp_to_add_mod_list_r11;
    csi_rs_cfg_zp_ap_list_r14_c          csi_rs_cfg_zp_ap_list_r15;
    eimta_main_cfg_r12_c                 eimta_main_cfg_r12;
    eimta_main_cfg_serv_cell_r12_c       eimta_main_cfg_serv_cell_r15;
    bool                                 semi_open_loop_stti_r15 = false;
    slot_or_subslot_pdsch_cfg_r15_c      slot_or_subslot_pdsch_cfg_r15;
    slot_or_subslot_pusch_cfg_r15_c      slot_or_subslot_pusch_cfg_r15;
    spdcch_cfg_r15_c                     spdcch_cfg_r15;
    spucch_cfg_r15_c                     spucch_cfg_r15;
    bool                                 srs_dci7_trigger_cfg_r15  = false;
    bool                                 short_processing_time_r15 = false;
    short_tti_r15_s                      short_tti_r15;
  };
  using types = setup_e;

  // choice methods
  phys_cfg_ded_stti_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const phys_cfg_ded_stti_r15_c& other) const;
  bool        operator!=(const phys_cfg_ded_stti_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "PhysicalConfigDedicatedSTTI-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "PhysicalConfigDedicatedSTTI-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// ResourceReservationConfigDedicatedDL-r16 ::= SEQUENCE
struct res_reserv_cfg_ded_dl_r16_s {
  bool                    res_reserv_ded_dl_r16_present = false;
  res_reserv_cfg_dl_r16_s res_reserv_ded_dl_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceReservationConfigDedicatedUL-r16 ::= SEQUENCE
struct res_reserv_cfg_ded_ul_r16_s {
  bool                    res_reserv_ded_ul_r16_present = false;
  res_reserv_cfg_ul_r16_s res_reserv_ded_ul_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SPUCCH-Config-v1550 ::= CHOICE
struct spucch_cfg_v1550_c {
  struct setup_s_ {
    struct two_ant_port_activ_spucch_format3_v1550_s_ {
      using n3_spucch_an_list_v1550_l_ = bounded_array<uint16_t, 4>;

      // member variables
      n3_spucch_an_list_v1550_l_ n3_spucch_an_list_v1550;
    };

    // member variables
    two_ant_port_activ_spucch_format3_v1550_s_ two_ant_port_activ_spucch_format3_v1550;
  };
  using types = setup_e;

  // choice methods
  spucch_cfg_v1550_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const spucch_cfg_v1550_c& other) const;
  bool        operator!=(const spucch_cfg_v1550_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SPUCCH-Config-v1550");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SPUCCH-Config-v1550");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SRS-TPC-PDCCH-Config-r14 ::= CHOICE
struct srs_tpc_pdcch_cfg_r14_c {
  struct setup_s_ {
    using srs_cc_set_idxlist_r14_l_ = dyn_array<srs_cc_set_idx_r14_s>;

    // member variables
    bool                      srs_cc_set_idxlist_r14_present = false;
    fixed_bitstring<16>       srs_tpc_rnti_r14;
    uint8_t                   start_bit_of_format3_b_r14 = 0;
    uint8_t                   field_type_format3_b_r14   = 1;
    srs_cc_set_idxlist_r14_l_ srs_cc_set_idxlist_r14;
  };
  using types = setup_e;

  // choice methods
  srs_tpc_pdcch_cfg_r14_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_tpc_pdcch_cfg_r14_c& other) const;
  bool        operator!=(const srs_tpc_pdcch_cfg_r14_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SRS-TPC-PDCCH-Config-r14");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SRS-TPC-PDCCH-Config-r14");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SchedulingRequestConfig ::= CHOICE
struct sched_request_cfg_c {
  struct setup_s_ {
    struct dsr_trans_max_opts {
      enum options { n4, n8, n16, n32, n64, spare3, spare2, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<dsr_trans_max_opts> dsr_trans_max_e_;

    // member variables
    uint16_t         sr_pucch_res_idx = 0;
    uint8_t          sr_cfg_idx       = 0;
    dsr_trans_max_e_ dsr_trans_max;
  };
  using types = setup_e;

  // choice methods
  sched_request_cfg_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SchedulingRequestConfig");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SchedulingRequestConfig");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SchedulingRequestConfig-v1020 ::= SEQUENCE
struct sched_request_cfg_v1020_s {
  bool     sr_pucch_res_idx_p1_r10_present = false;
  uint16_t sr_pucch_res_idx_p1_r10         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SoundingRS-UL-ConfigDedicated ::= CHOICE
struct srs_ul_cfg_ded_c {
  struct setup_s_ {
    struct srs_bw_opts {
      enum options { bw0, bw1, bw2, bw3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<srs_bw_opts> srs_bw_e_;
    struct srs_hop_bw_opts {
      enum options { hbw0, hbw1, hbw2, hbw3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<srs_hop_bw_opts> srs_hop_bw_e_;
    struct cyclic_shift_opts {
      enum options { cs0, cs1, cs2, cs3, cs4, cs5, cs6, cs7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<cyclic_shift_opts> cyclic_shift_e_;

    // member variables
    srs_bw_e_       srs_bw;
    srs_hop_bw_e_   srs_hop_bw;
    uint8_t         freq_domain_position = 0;
    bool            dur                  = false;
    uint16_t        srs_cfg_idx          = 0;
    uint8_t         tx_comb              = 0;
    cyclic_shift_e_ cyclic_shift;
  };
  using types = setup_e;

  // choice methods
  srs_ul_cfg_ded_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_ul_cfg_ded_c& other) const;
  bool        operator!=(const srs_ul_cfg_ded_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicated");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicated");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SoundingRS-UL-ConfigDedicated-v1020 ::= SEQUENCE
struct srs_ul_cfg_ded_v1020_s {
  srs_ant_port_e srs_ant_port_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_ul_cfg_ded_v1020_s& other) const;
  bool        operator!=(const srs_ul_cfg_ded_v1020_s& other) const { return not(*this == other); }
};

// SoundingRS-UL-ConfigDedicated-v1310 ::= CHOICE
struct srs_ul_cfg_ded_v1310_c {
  struct setup_s_ {
    struct cyclic_shift_v1310_opts {
      enum options { cs8, cs9, cs10, cs11, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<cyclic_shift_v1310_opts> cyclic_shift_v1310_e_;
    struct tx_comb_num_r13_opts {
      enum options { n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<tx_comb_num_r13_opts> tx_comb_num_r13_e_;

    // member variables
    bool                  tx_comb_v1310_present      = false;
    bool                  cyclic_shift_v1310_present = false;
    bool                  tx_comb_num_r13_present    = false;
    uint8_t               tx_comb_v1310              = 2;
    cyclic_shift_v1310_e_ cyclic_shift_v1310;
    tx_comb_num_r13_e_    tx_comb_num_r13;
  };
  using types = setup_e;

  // choice methods
  srs_ul_cfg_ded_v1310_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_ul_cfg_ded_v1310_c& other) const;
  bool        operator!=(const srs_ul_cfg_ded_v1310_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicated-v1310");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicated-v1310");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SoundingRS-UL-ConfigDedicatedAdd-r16 ::= SEQUENCE
struct srs_ul_cfg_ded_add_r16_s {
  using srs_cfg_ap_dci_format4_r16_l_ = dyn_array<srs_cfg_add_r16_s>;
  struct srs_activ_ap_r13_c_ {
    struct setup_s_ {
      srs_cfg_add_r16_s srs_cfg_ap_dci_format0_r16;
      srs_cfg_add_r16_s srs_cfg_ap_dci_format1a2b2c_r16;
    };
    using types = setup_e;

    // choice methods
    srs_activ_ap_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const srs_activ_ap_r13_c_& other) const;
    bool        operator!=(const srs_activ_ap_r13_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "srs-ActivateAp-r13");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "srs-ActivateAp-r13");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                          srs_cfg_ap_dci_format4_r16_present = false;
  bool                          srs_activ_ap_r13_present           = false;
  uint8_t                       srs_cfg_idx_ap_r16                 = 0;
  srs_cfg_ap_dci_format4_r16_l_ srs_cfg_ap_dci_format4_r16;
  srs_activ_ap_r13_c_           srs_activ_ap_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_ul_cfg_ded_add_r16_s& other) const;
  bool        operator!=(const srs_ul_cfg_ded_add_r16_s& other) const { return not(*this == other); }
};

// SoundingRS-UL-ConfigDedicatedAperiodic-r10 ::= CHOICE
struct srs_ul_cfg_ded_aperiodic_r10_c {
  struct setup_s_ {
    using srs_cfg_ap_dci_format4_r10_l_ = dyn_array<srs_cfg_ap_r10_s>;
    struct srs_activ_ap_r10_c_ {
      struct setup_s_ {
        bool             ext = false;
        srs_cfg_ap_r10_s srs_cfg_ap_dci_format0_r10;
        srs_cfg_ap_r10_s srs_cfg_ap_dci_format1a2b2c_r10;
        // ...
      };
      using types = setup_e;

      // choice methods
      srs_activ_ap_r10_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const srs_activ_ap_r10_c_& other) const;
      bool        operator!=(const srs_activ_ap_r10_c_& other) const { return not(*this == other); }
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "srs-ActivateAp-r10");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "srs-ActivateAp-r10");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };

    // member variables
    bool                          srs_cfg_ap_dci_format4_r10_present = false;
    bool                          srs_activ_ap_r10_present           = false;
    uint8_t                       srs_cfg_idx_ap_r10                 = 0;
    srs_cfg_ap_dci_format4_r10_l_ srs_cfg_ap_dci_format4_r10;
    srs_activ_ap_r10_c_           srs_activ_ap_r10;
  };
  using types = setup_e;

  // choice methods
  srs_ul_cfg_ded_aperiodic_r10_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_ul_cfg_ded_aperiodic_r10_c& other) const;
  bool        operator!=(const srs_ul_cfg_ded_aperiodic_r10_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicatedAperiodic-r10");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicatedAperiodic-r10");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SoundingRS-UL-ConfigDedicatedAperiodic-v1310 ::= CHOICE
struct srs_ul_cfg_ded_aperiodic_v1310_c {
  struct setup_s_ {
    using srs_cfg_ap_dci_format4_v1310_l_ = dyn_array<srs_cfg_ap_v1310_s>;
    struct srs_activ_ap_v1310_c_ {
      struct setup_s_ {
        bool               srs_cfg_ap_dci_format0_v1310_present      = false;
        bool               srs_cfg_ap_dci_format1a2b2c_v1310_present = false;
        srs_cfg_ap_v1310_s srs_cfg_ap_dci_format0_v1310;
        srs_cfg_ap_v1310_s srs_cfg_ap_dci_format1a2b2c_v1310;
      };
      using types = setup_e;

      // choice methods
      srs_activ_ap_v1310_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const srs_activ_ap_v1310_c_& other) const;
      bool        operator!=(const srs_activ_ap_v1310_c_& other) const { return not(*this == other); }
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "srs-ActivateAp-v1310");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "srs-ActivateAp-v1310");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };

    // member variables
    bool                            srs_cfg_ap_dci_format4_v1310_present = false;
    bool                            srs_activ_ap_v1310_present           = false;
    srs_cfg_ap_dci_format4_v1310_l_ srs_cfg_ap_dci_format4_v1310;
    srs_activ_ap_v1310_c_           srs_activ_ap_v1310;
  };
  using types = setup_e;

  // choice methods
  srs_ul_cfg_ded_aperiodic_v1310_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_ul_cfg_ded_aperiodic_v1310_c& other) const;
  bool        operator!=(const srs_ul_cfg_ded_aperiodic_v1310_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicatedAperiodic-v1310");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicatedAperiodic-v1310");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SoundingRS-UL-ConfigDedicatedAperiodicUpPTsExt-r13 ::= CHOICE
struct srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c {
  struct setup_s_ {
    struct srs_up_pts_add_r13_opts {
      enum options { sym2, sym4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<srs_up_pts_add_r13_opts> srs_up_pts_add_r13_e_;
    using srs_cfg_ap_dci_format4_r13_l_ = dyn_array<srs_cfg_ap_r13_s>;
    struct srs_activ_ap_r13_c_ {
      struct setup_s_ {
        srs_cfg_ap_r13_s srs_cfg_ap_dci_format0_r13;
        srs_cfg_ap_r13_s srs_cfg_ap_dci_format1a2b2c_r13;
      };
      using types = setup_e;

      // choice methods
      srs_activ_ap_r13_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const srs_activ_ap_r13_c_& other) const;
      bool        operator!=(const srs_activ_ap_r13_c_& other) const { return not(*this == other); }
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "srs-ActivateAp-r13");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "srs-ActivateAp-r13");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };

    // member variables
    bool                          srs_cfg_ap_dci_format4_r13_present = false;
    bool                          srs_activ_ap_r13_present           = false;
    srs_up_pts_add_r13_e_         srs_up_pts_add_r13;
    uint8_t                       srs_cfg_idx_ap_r13 = 0;
    srs_cfg_ap_dci_format4_r13_l_ srs_cfg_ap_dci_format4_r13;
    srs_activ_ap_r13_c_           srs_activ_ap_r13;
  };
  using types = setup_e;

  // choice methods
  srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c& other) const;
  bool        operator!=(const srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicatedAperiodicUpPTsExt-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicatedAperiodicUpPTsExt-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SoundingRS-UL-ConfigDedicatedUpPTsExt-r13 ::= CHOICE
struct srs_ul_cfg_ded_up_pts_ext_r13_c {
  struct setup_s_ {
    struct srs_up_pts_add_r13_opts {
      enum options { sym2, sym4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<srs_up_pts_add_r13_opts> srs_up_pts_add_r13_e_;
    struct srs_bw_r13_opts {
      enum options { bw0, bw1, bw2, bw3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<srs_bw_r13_opts> srs_bw_r13_e_;
    struct srs_hop_bw_r13_opts {
      enum options { hbw0, hbw1, hbw2, hbw3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<srs_hop_bw_r13_opts> srs_hop_bw_r13_e_;
    struct cyclic_shift_r13_opts {
      enum options { cs0, cs1, cs2, cs3, cs4, cs5, cs6, cs7, cs8, cs9, cs10, cs11, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<cyclic_shift_r13_opts> cyclic_shift_r13_e_;
    struct tx_comb_num_r13_opts {
      enum options { n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<tx_comb_num_r13_opts> tx_comb_num_r13_e_;

    // member variables
    srs_up_pts_add_r13_e_ srs_up_pts_add_r13;
    srs_bw_r13_e_         srs_bw_r13;
    srs_hop_bw_r13_e_     srs_hop_bw_r13;
    uint8_t               freq_domain_position_r13 = 0;
    bool                  dur_r13                  = false;
    uint16_t              srs_cfg_idx_r13          = 0;
    uint8_t               tx_comb_r13              = 0;
    cyclic_shift_r13_e_   cyclic_shift_r13;
    srs_ant_port_e        srs_ant_port_r13;
    tx_comb_num_r13_e_    tx_comb_num_r13;
  };
  using types = setup_e;

  // choice methods
  srs_ul_cfg_ded_up_pts_ext_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_ul_cfg_ded_up_pts_ext_r13_c& other) const;
  bool        operator!=(const srs_ul_cfg_ded_up_pts_ext_r13_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicatedUpPTsExt-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicatedUpPTsExt-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SoundingRS-VirtualCellID-r16 ::= SEQUENCE
struct srs_virtual_cell_id_r16_s {
  uint16_t srs_virtual_cell_id_r16         = 0;
  bool     srs_virtual_cell_id_all_srs_r16 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_virtual_cell_id_r16_s& other) const;
  bool        operator!=(const srs_virtual_cell_id_r16_s& other) const { return not(*this == other); }
};

// UplinkPowerControlAddSRS-r16 ::= SEQUENCE
struct ul_pwr_ctrl_add_srs_r16_s {
  bool      tpc_idx_srs_add_r16_present                = false;
  bool      start_bit_of_format3_b_srs_add_r16_present = false;
  bool      field_type_format3_b_srs_add_r16_present   = false;
  bool      p0_ue_srs_add_r16_present                  = false;
  tpc_idx_c tpc_idx_srs_add_r16;
  uint8_t   start_bit_of_format3_b_srs_add_r16 = 0;
  uint8_t   field_type_format3_b_srs_add_r16   = 1;
  int8_t    p0_ue_srs_add_r16                  = -16;
  bool      accumulation_enabled_srs_add_r16   = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pwr_ctrl_add_srs_r16_s& other) const;
  bool        operator!=(const ul_pwr_ctrl_add_srs_r16_s& other) const { return not(*this == other); }
};

// UplinkPowerControlDedicated ::= SEQUENCE
struct ul_pwr_ctrl_ded_s {
  struct delta_mcs_enabled_opts {
    enum options { en0, en1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<delta_mcs_enabled_opts> delta_mcs_enabled_e_;

  // member variables
  bool                 filt_coef_present = false;
  int8_t               p0_ue_pusch       = -8;
  delta_mcs_enabled_e_ delta_mcs_enabled;
  bool                 accumulation_enabled = false;
  int8_t               p0_ue_pucch          = -8;
  uint8_t              psrs_offset          = 0;
  filt_coef_e          filt_coef;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkPowerControlDedicated-v1020 ::= SEQUENCE
struct ul_pwr_ctrl_ded_v1020_s {
  bool                               delta_tx_d_offset_list_pucch_r10_present = false;
  bool                               psrs_offset_ap_r10_present               = false;
  delta_tx_d_offset_list_pucch_r10_s delta_tx_d_offset_list_pucch_r10;
  uint8_t                            psrs_offset_ap_r10 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkPowerControlDedicated-v1130 ::= SEQUENCE
struct ul_pwr_ctrl_ded_v1130_s {
  bool                                 psrs_offset_v1130_present                  = false;
  bool                                 psrs_offset_ap_v1130_present               = false;
  bool                                 delta_tx_d_offset_list_pucch_v1130_present = false;
  uint8_t                              psrs_offset_v1130                          = 16;
  uint8_t                              psrs_offset_ap_v1130                       = 16;
  delta_tx_d_offset_list_pucch_v1130_s delta_tx_d_offset_list_pucch_v1130;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pwr_ctrl_ded_v1130_s& other) const;
  bool        operator!=(const ul_pwr_ctrl_ded_v1130_s& other) const { return not(*this == other); }
};

// UplinkPowerControlDedicated-v1250 ::= SEQUENCE
struct ul_pwr_ctrl_ded_v1250_s {
  struct set2_pwr_ctrl_param_c_ {
    struct setup_s_ {
      fixed_bitstring<10> tpc_sf_set_r12;
      int8_t              p0_nominal_pusch_sf_set2_r12 = -126;
      alpha_r12_e         alpha_sf_set2_r12;
      int8_t              p0_ue_pusch_sf_set2_r12 = -8;
    };
    using types = setup_e;

    // choice methods
    set2_pwr_ctrl_param_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const set2_pwr_ctrl_param_c_& other) const;
    bool        operator!=(const set2_pwr_ctrl_param_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "set2PowerControlParameter");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "set2PowerControlParameter");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  set2_pwr_ctrl_param_c_ set2_pwr_ctrl_param;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pwr_ctrl_ded_v1250_s& other) const;
  bool        operator!=(const ul_pwr_ctrl_ded_v1250_s& other) const { return not(*this == other); }
};

// UplinkPowerControlDedicated-v1530 ::= SEQUENCE
struct ul_pwr_ctrl_ded_v1530_s {
  bool        alpha_ue_r15_present    = false;
  bool        p0_ue_pusch_r15_present = false;
  alpha_r12_e alpha_ue_r15;
  int8_t      p0_ue_pusch_r15 = -16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pwr_ctrl_ded_v1530_s& other) const;
  bool        operator!=(const ul_pwr_ctrl_ded_v1530_s& other) const { return not(*this == other); }
};

// WidebandPRG-r16 ::= SEQUENCE
struct wideband_prg_r16_s {
  bool wideband_prg_sf_r16           = false;
  bool wideband_prg_slot_subslot_r16 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const wideband_prg_r16_s& other) const;
  bool        operator!=(const wideband_prg_r16_s& other) const { return not(*this == other); }
};

// PhysicalConfigDedicated ::= SEQUENCE
struct phys_cfg_ded_s {
  struct ant_info_c_ {
    struct types_opts {
      enum options { explicit_value, default_value, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ant_info_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ant_info_ded_s& explicit_value()
    {
      assert_choice_type(types::explicit_value, type_, "antennaInfo");
      return c;
    }
    const ant_info_ded_s& explicit_value() const
    {
      assert_choice_type(types::explicit_value, type_, "antennaInfo");
      return c;
    }
    ant_info_ded_s& set_explicit_value();
    void            set_default_value();

  private:
    types          type_;
    ant_info_ded_s c;
  };
  struct ant_info_r10_c_ {
    struct types_opts {
      enum options { explicit_value_r10, default_value, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ant_info_r10_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ant_info_ded_r10_s& explicit_value_r10()
    {
      assert_choice_type(types::explicit_value_r10, type_, "antennaInfo-r10");
      return c;
    }
    const ant_info_ded_r10_s& explicit_value_r10() const
    {
      assert_choice_type(types::explicit_value_r10, type_, "antennaInfo-r10");
      return c;
    }
    ant_info_ded_r10_s& set_explicit_value_r10();
    void                set_default_value();

  private:
    types              type_;
    ant_info_ded_r10_s c;
  };
  struct add_spec_emission_ca_r10_c_ {
    struct setup_s_ {
      uint8_t add_spec_emission_pcell_r10 = 1;
    };
    using types = setup_e;

    // choice methods
    add_spec_emission_ca_r10_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "additionalSpectrumEmissionCA-r10");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "additionalSpectrumEmissionCA-r10");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct ce_mode_r13_c_ {
    struct setup_opts {
      enum options { ce_mode_a, ce_mode_b, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<setup_opts> setup_e_;
    using types = setup_e;

    // choice methods
    ce_mode_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_e_& setup()
    {
      assert_choice_type(types::setup, type_, "ce-Mode-r13");
      return c;
    }
    const setup_e_& setup() const
    {
      assert_choice_type(types::setup, type_, "ce-Mode-r13");
      return c;
    }
    void      set_release();
    setup_e_& set_setup();

  private:
    types    type_;
    setup_e_ c;
  };
  struct type_a_srs_tpc_pdcch_group_r14_c_ {
    using setup_l_ = dyn_array<srs_tpc_pdcch_cfg_r14_c>;
    using types    = setup_e;

    // choice methods
    type_a_srs_tpc_pdcch_group_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_l_& setup()
    {
      assert_choice_type(types::setup, type_, "typeA-SRS-TPC-PDCCH-Group-r14");
      return c;
    }
    const setup_l_& setup() const
    {
      assert_choice_type(types::setup, type_, "typeA-SRS-TPC-PDCCH-Group-r14");
      return c;
    }
    void      set_release();
    setup_l_& set_setup();

  private:
    types    type_;
    setup_l_ c;
  };
  struct must_cfg_r14_c_ {
    struct setup_s_ {
      struct k_max_r14_opts {
        enum options { l1, l3, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<k_max_r14_opts> k_max_r14_e_;
      struct p_a_must_r14_opts {
        enum options { db_minus6, db_minus4dot77, db_minus3, db_minus1dot77, db0, db1, db2, db3, nulltype } value;
        typedef float number_type;

        const char* to_string() const;
        float       to_number() const;
        const char* to_number_string() const;
      };
      typedef enumerated<p_a_must_r14_opts> p_a_must_r14_e_;

      // member variables
      bool            p_a_must_r14_present = false;
      k_max_r14_e_    k_max_r14;
      p_a_must_r14_e_ p_a_must_r14;
    };
    using types = setup_e;

    // choice methods
    must_cfg_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "must-Config-r14");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "must-Config-r14");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  using srs_ul_periodic_cfg_ded_list_r14_l_            = dyn_array<srs_ul_cfg_ded_c>;
  using srs_ul_periodic_cfg_ded_up_pts_ext_list_r14_l_ = dyn_array<srs_ul_cfg_ded_up_pts_ext_r13_c>;
  using srs_ul_aperiodic_cfg_ded_list_r14_l_           = dyn_array<srs_ul_cfg_ded_aperiodic_r10_c>;
  using srs_ul_cfg_ded_ap_up_pts_ext_list_r14_l_       = dyn_array<srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c>;
  struct semi_static_cfi_cfg_r15_c_ {
    struct setup_c_ {
      struct types_opts {
        enum options { cfi_cfg_r15, cfi_pattern_cfg_r15, nulltype } value;

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
      cfi_cfg_r15_s& cfi_cfg_r15()
      {
        assert_choice_type(types::cfi_cfg_r15, type_, "setup");
        return c.get<cfi_cfg_r15_s>();
      }
      cfi_pattern_cfg_r15_s& cfi_pattern_cfg_r15()
      {
        assert_choice_type(types::cfi_pattern_cfg_r15, type_, "setup");
        return c.get<cfi_pattern_cfg_r15_s>();
      }
      const cfi_cfg_r15_s& cfi_cfg_r15() const
      {
        assert_choice_type(types::cfi_cfg_r15, type_, "setup");
        return c.get<cfi_cfg_r15_s>();
      }
      const cfi_pattern_cfg_r15_s& cfi_pattern_cfg_r15() const
      {
        assert_choice_type(types::cfi_pattern_cfg_r15, type_, "setup");
        return c.get<cfi_pattern_cfg_r15_s>();
      }
      cfi_cfg_r15_s&         set_cfi_cfg_r15();
      cfi_pattern_cfg_r15_s& set_cfi_pattern_cfg_r15();

    private:
      types                                                 type_;
      choice_buffer_t<cfi_cfg_r15_s, cfi_pattern_cfg_r15_s> c;

      void destroy_();
    };
    using types = setup_e;

    // choice methods
    semi_static_cfi_cfg_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_c_& setup()
    {
      assert_choice_type(types::setup, type_, "semiStaticCFI-Config-r15");
      return c;
    }
    const setup_c_& setup() const
    {
      assert_choice_type(types::setup, type_, "semiStaticCFI-Config-r15");
      return c;
    }
    void      set_release();
    setup_c_& set_setup();

  private:
    types    type_;
    setup_c_ c;
  };
  struct blind_pdsch_repeat_cfg_r15_c_ {
    struct setup_s_ {
      struct max_num_sf_pdsch_repeats_r15_opts {
        enum options { n4, n6, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<max_num_sf_pdsch_repeats_r15_opts> max_num_sf_pdsch_repeats_r15_e_;
      struct max_num_slot_subslot_pdsch_repeats_r15_opts {
        enum options { n4, n6, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<max_num_slot_subslot_pdsch_repeats_r15_opts> max_num_slot_subslot_pdsch_repeats_r15_e_;
      struct rv_sf_pdsch_repeats_r15_opts {
        enum options { dlrvseq1, dlrvseq2, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<rv_sf_pdsch_repeats_r15_opts> rv_sf_pdsch_repeats_r15_e_;
      struct rv_slotsublot_pdsch_repeats_r15_opts {
        enum options { dlrvseq1, dlrvseq2, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<rv_slotsublot_pdsch_repeats_r15_opts> rv_slotsublot_pdsch_repeats_r15_e_;
      struct mcs_restrict_sf_pdsch_repeats_r15_opts {
        enum options { n0, n1, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<mcs_restrict_sf_pdsch_repeats_r15_opts> mcs_restrict_sf_pdsch_repeats_r15_e_;
      struct mcs_restrict_slot_subslot_pdsch_repeats_r15_opts {
        enum options { n0, n1, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<mcs_restrict_slot_subslot_pdsch_repeats_r15_opts>
          mcs_restrict_slot_subslot_pdsch_repeats_r15_e_;

      // member variables
      bool                                           max_num_sf_pdsch_repeats_r15_present                 = false;
      bool                                           max_num_slot_subslot_pdsch_repeats_r15_present       = false;
      bool                                           rv_sf_pdsch_repeats_r15_present                      = false;
      bool                                           rv_slotsublot_pdsch_repeats_r15_present              = false;
      bool                                           nof_processes_sf_pdsch_repeats_r15_present           = false;
      bool                                           nof_processes_slot_subslot_pdsch_repeats_r15_present = false;
      bool                                           mcs_restrict_sf_pdsch_repeats_r15_present            = false;
      bool                                           mcs_restrict_slot_subslot_pdsch_repeats_r15_present  = false;
      bool                                           blind_sf_pdsch_repeats_r15                           = false;
      bool                                           blind_slot_subslot_pdsch_repeats_r15                 = false;
      max_num_sf_pdsch_repeats_r15_e_                max_num_sf_pdsch_repeats_r15;
      max_num_slot_subslot_pdsch_repeats_r15_e_      max_num_slot_subslot_pdsch_repeats_r15;
      rv_sf_pdsch_repeats_r15_e_                     rv_sf_pdsch_repeats_r15;
      rv_slotsublot_pdsch_repeats_r15_e_             rv_slotsublot_pdsch_repeats_r15;
      uint8_t                                        nof_processes_sf_pdsch_repeats_r15           = 1;
      uint8_t                                        nof_processes_slot_subslot_pdsch_repeats_r15 = 1;
      mcs_restrict_sf_pdsch_repeats_r15_e_           mcs_restrict_sf_pdsch_repeats_r15;
      mcs_restrict_slot_subslot_pdsch_repeats_r15_e_ mcs_restrict_slot_subslot_pdsch_repeats_r15;
    };
    using types = setup_e;

    // choice methods
    blind_pdsch_repeat_cfg_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "blindPDSCH-Repetition-Config-r15");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "blindPDSCH-Repetition-Config-r15");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct ntn_cfg_ded_r17_s_ {
    bool                                pucch_tx_dur_r17_present = false;
    bool                                pusch_tx_dur_r17_present = false;
    setup_release_c<pucch_tx_dur_r17_s> pucch_tx_dur_r17;
    setup_release_c<pusch_tx_dur_r17_s> pusch_tx_dur_r17;
  };
  struct ul_segmented_precompensation_gap_r17_opts {
    enum options { sym1, sl1, sf1, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ul_segmented_precompensation_gap_r17_opts> ul_segmented_precompensation_gap_r17_e_;

  // member variables
  bool                ext                         = false;
  bool                pdsch_cfg_ded_present       = false;
  bool                pucch_cfg_ded_present       = false;
  bool                pusch_cfg_ded_present       = false;
  bool                ul_pwr_ctrl_ded_present     = false;
  bool                tpc_pdcch_cfg_pucch_present = false;
  bool                tpc_pdcch_cfg_pusch_present = false;
  bool                cqi_report_cfg_present      = false;
  bool                srs_ul_cfg_ded_present      = false;
  bool                ant_info_present            = false;
  bool                sched_request_cfg_present   = false;
  pdsch_cfg_ded_s     pdsch_cfg_ded;
  pucch_cfg_ded_s     pucch_cfg_ded;
  pusch_cfg_ded_s     pusch_cfg_ded;
  ul_pwr_ctrl_ded_s   ul_pwr_ctrl_ded;
  tpc_pdcch_cfg_c     tpc_pdcch_cfg_pucch;
  tpc_pdcch_cfg_c     tpc_pdcch_cfg_pusch;
  cqi_report_cfg_s    cqi_report_cfg;
  srs_ul_cfg_ded_c    srs_ul_cfg_ded;
  ant_info_c_         ant_info;
  sched_request_cfg_c sched_request_cfg;
  // ...
  // group 0
  copy_ptr<cqi_report_cfg_v920_s> cqi_report_cfg_v920;
  copy_ptr<ant_info_ded_v920_s>   ant_info_v920;
  // group 1
  bool                                     cif_presence_r10_present = false;
  copy_ptr<ant_info_r10_c_>                ant_info_r10;
  copy_ptr<ant_info_ul_r10_s>              ant_info_ul_r10;
  bool                                     cif_presence_r10 = false;
  copy_ptr<cqi_report_cfg_r10_s>           cqi_report_cfg_r10;
  copy_ptr<csi_rs_cfg_r10_s>               csi_rs_cfg_r10;
  copy_ptr<pucch_cfg_ded_v1020_s>          pucch_cfg_ded_v1020;
  copy_ptr<pusch_cfg_ded_v1020_s>          pusch_cfg_ded_v1020;
  copy_ptr<sched_request_cfg_v1020_s>      sched_request_cfg_v1020;
  copy_ptr<srs_ul_cfg_ded_v1020_s>         srs_ul_cfg_ded_v1020;
  copy_ptr<srs_ul_cfg_ded_aperiodic_r10_c> srs_ul_cfg_ded_aperiodic_r10;
  copy_ptr<ul_pwr_ctrl_ded_v1020_s>        ul_pwr_ctrl_ded_v1020;
  // group 2
  copy_ptr<add_spec_emission_ca_r10_c_> add_spec_emission_ca_r10;
  // group 3
  copy_ptr<csi_rs_cfg_nzp_to_release_list_r11_l> csi_rs_cfg_nzp_to_release_list_r11;
  copy_ptr<csi_rs_cfg_nzp_to_add_mod_list_r11_l> csi_rs_cfg_nzp_to_add_mod_list_r11;
  copy_ptr<csi_rs_cfg_zp_to_release_list_r11_l>  csi_rs_cfg_zp_to_release_list_r11;
  copy_ptr<csi_rs_cfg_zp_to_add_mod_list_r11_l>  csi_rs_cfg_zp_to_add_mod_list_r11;
  copy_ptr<epdcch_cfg_r11_s>                     epdcch_cfg_r11;
  copy_ptr<pdsch_cfg_ded_v1130_s>                pdsch_cfg_ded_v1130;
  copy_ptr<cqi_report_cfg_v1130_s>               cqi_report_cfg_v1130;
  copy_ptr<pucch_cfg_ded_v1130_s>                pucch_cfg_ded_v1130;
  copy_ptr<pusch_cfg_ded_v1130_s>                pusch_cfg_ded_v1130;
  copy_ptr<ul_pwr_ctrl_ded_v1130_s>              ul_pwr_ctrl_ded_v1130;
  // group 4
  copy_ptr<ant_info_ded_v1250_s>           ant_info_v1250;
  copy_ptr<eimta_main_cfg_r12_c>           eimta_main_cfg_r12;
  copy_ptr<eimta_main_cfg_serv_cell_r12_c> eimta_main_cfg_pcell_r12;
  copy_ptr<pucch_cfg_ded_v1250_s>          pucch_cfg_ded_v1250;
  copy_ptr<cqi_report_cfg_v1250_s>         cqi_report_cfg_pcell_v1250;
  copy_ptr<ul_pwr_ctrl_ded_v1250_s>        ul_pwr_ctrl_ded_v1250;
  copy_ptr<pusch_cfg_ded_v1250_s>          pusch_cfg_ded_v1250;
  copy_ptr<csi_rs_cfg_v1250_s>             csi_rs_cfg_v1250;
  // group 5
  copy_ptr<pdsch_cfg_ded_v1280_s> pdsch_cfg_ded_v1280;
  // group 6
  copy_ptr<pdsch_cfg_ded_v1310_s>                     pdsch_cfg_ded_v1310;
  copy_ptr<pucch_cfg_ded_r13_s>                       pucch_cfg_ded_r13;
  copy_ptr<pusch_cfg_ded_r13_s>                       pusch_cfg_ded_r13;
  copy_ptr<pdcch_candidate_reductions_r13_c>          pdcch_candidate_reductions_r13;
  copy_ptr<cqi_report_cfg_v1310_s>                    cqi_report_cfg_v1310;
  copy_ptr<srs_ul_cfg_ded_v1310_c>                    srs_ul_cfg_ded_v1310;
  copy_ptr<srs_ul_cfg_ded_up_pts_ext_r13_c>           srs_ul_cfg_ded_up_pts_ext_r13;
  copy_ptr<srs_ul_cfg_ded_aperiodic_v1310_c>          srs_ul_cfg_ded_aperiodic_v1310;
  copy_ptr<srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c> srs_ul_cfg_ded_aperiodic_up_pts_ext_r13;
  copy_ptr<csi_rs_cfg_v1310_s>                        csi_rs_cfg_v1310;
  copy_ptr<ce_mode_r13_c_>                            ce_mode_r13;
  copy_ptr<csi_rs_cfg_nzp_to_add_mod_list_ext_r13_l>  csi_rs_cfg_nzp_to_add_mod_list_ext_r13;
  copy_ptr<csi_rs_cfg_nzp_to_release_list_ext_r13_l>  csi_rs_cfg_nzp_to_release_list_ext_r13;
  // group 7
  copy_ptr<cqi_report_cfg_v1320_s> cqi_report_cfg_v1320;
  // group 8
  bool                                                     ce_pdsch_pusch_enhancement_cfg_r14_present = false;
  bool                                                     semi_open_loop_r14_present                 = false;
  copy_ptr<type_a_srs_tpc_pdcch_group_r14_c_>              type_a_srs_tpc_pdcch_group_r14;
  copy_ptr<must_cfg_r14_c_>                                must_cfg_r14;
  copy_ptr<pusch_enhance_cfg_r14_c>                        pusch_enhance_cfg_r14;
  copy_ptr<ant_info_ded_v1430_s>                           ant_info_v1430;
  copy_ptr<pucch_cfg_ded_v1430_s>                          pucch_cfg_ded_v1430;
  copy_ptr<pdsch_cfg_ded_v1430_s>                          pdsch_cfg_ded_v1430;
  copy_ptr<pusch_cfg_ded_v1430_s>                          pusch_cfg_ded_v1430;
  copy_ptr<srs_ul_periodic_cfg_ded_list_r14_l_>            srs_ul_periodic_cfg_ded_list_r14;
  copy_ptr<srs_ul_periodic_cfg_ded_up_pts_ext_list_r14_l_> srs_ul_periodic_cfg_ded_up_pts_ext_list_r14;
  copy_ptr<srs_ul_aperiodic_cfg_ded_list_r14_l_>           srs_ul_aperiodic_cfg_ded_list_r14;
  copy_ptr<srs_ul_cfg_ded_ap_up_pts_ext_list_r14_l_>       srs_ul_cfg_ded_ap_up_pts_ext_list_r14;
  copy_ptr<csi_rs_cfg_v1430_s>                             csi_rs_cfg_v1430;
  copy_ptr<csi_rs_cfg_zp_ap_list_r14_c>                    csi_rs_cfg_zp_ap_list_r14;
  copy_ptr<cqi_report_cfg_v1430_s>                         cqi_report_cfg_v1430;
  bool                                                     semi_open_loop_r14 = false;
  // group 9
  copy_ptr<csi_rs_cfg_v1480_s> csi_rs_cfg_v1480;
  // group 10
  copy_ptr<phys_cfg_ded_stti_r15_c>       phys_cfg_ded_stti_r15;
  copy_ptr<pdsch_cfg_ded_v1530_s>         pdsch_cfg_ded_v1530;
  copy_ptr<pusch_cfg_ded_v1530_s>         pusch_cfg_ded_v1530;
  copy_ptr<cqi_report_cfg_v1530_s>        cqi_report_cfg_v1530;
  copy_ptr<ant_info_ded_v1530_c>          ant_info_v1530;
  copy_ptr<csi_rs_cfg_v1530_s>            csi_rs_cfg_v1530;
  copy_ptr<ul_pwr_ctrl_ded_v1530_s>       ul_pwr_ctrl_ded_v1530;
  copy_ptr<semi_static_cfi_cfg_r15_c_>    semi_static_cfi_cfg_r15;
  copy_ptr<blind_pdsch_repeat_cfg_r15_c_> blind_pdsch_repeat_cfg_r15;
  // group 11
  copy_ptr<spucch_cfg_v1550_c> spucch_cfg_v1550;
  // group 12
  bool                                                    ce_csi_rs_feedback_r16_present = false;
  copy_ptr<pdsch_cfg_ded_v1610_s>                         pdsch_cfg_ded_v1610;
  copy_ptr<pusch_cfg_ded_v1610_s>                         pusch_cfg_ded_v1610;
  copy_ptr<setup_release_c<res_reserv_cfg_ded_dl_r16_s> > res_reserv_cfg_ded_dl_r16;
  copy_ptr<setup_release_c<res_reserv_cfg_ded_ul_r16_s> > res_reserv_cfg_ded_ul_r16;
  copy_ptr<setup_release_c<srs_ul_cfg_ded_add_r16_s> >    srs_ul_cfg_ded_add_r16;
  copy_ptr<setup_release_c<ul_pwr_ctrl_add_srs_r16_s> >   ul_pwr_ctrl_add_srs_r16;
  copy_ptr<setup_release_c<srs_virtual_cell_id_r16_s> >   srs_virtual_cell_id_r16;
  copy_ptr<setup_release_c<wideband_prg_r16_s> >          wideband_prg_r16;
  // group 13
  copy_ptr<pdsch_cfg_ded_v1700_s> pdsch_cfg_ded_v1700;
  copy_ptr<ntn_cfg_ded_r17_s_>    ntn_cfg_ded_r17;
  // group 14
  bool                                    ul_segmented_precompensation_gap_r17_present = false;
  ul_segmented_precompensation_gap_r17_e_ ul_segmented_precompensation_gap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_PHYCFG_H
