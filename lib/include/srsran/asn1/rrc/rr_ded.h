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

#ifndef SRSASN1_RRC_RRCFG_H
#define SRSASN1_RRC_RRCFG_H

#include "phy_ded.h"
#include "rr_common.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// PollByte-r14 ::= ENUMERATED
struct poll_byte_r14_opts {
  enum options {
    kb1,
    kb2,
    kb5,
    kb8,
    kb10,
    kb15,
    kb3500,
    kb4000,
    kb4500,
    kb5000,
    kb5500,
    kb6000,
    kb6500,
    kb7000,
    kb7500,
    kb8000,
    kb9000,
    kb10000,
    kb11000,
    kb12000,
    kb13000,
    kb14000,
    kb15000,
    kb16000,
    kb17000,
    kb18000,
    kb19000,
    kb20000,
    kb25000,
    kb30000,
    kb35000,
    kb40000,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<poll_byte_r14_opts> poll_byte_r14_e;

// PollPDU-r15 ::= ENUMERATED
struct poll_pdu_r15_opts {
  enum options {
    p4,
    p8,
    p16,
    p32,
    p64,
    p128,
    p256,
    p512,
    p1024,
    p2048_r15,
    p4096_r15,
    p6144_r15,
    p8192_r15,
    p12288_r15,
    p16384_r15,
    pinfinity,
    nulltype
  } value;
  typedef int16_t number_type;

  const char* to_string() const;
  int16_t     to_number() const;
};
typedef enumerated<poll_pdu_r15_opts> poll_pdu_r15_e;

// SN-FieldLength ::= ENUMERATED
struct sn_field_len_opts {
  enum options { size5, size10, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<sn_field_len_opts> sn_field_len_e;

// SN-FieldLength-r15 ::= ENUMERATED
struct sn_field_len_r15_opts {
  enum options { size5, size10, size16_r15, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<sn_field_len_r15_opts> sn_field_len_r15_e;

// T-PollRetransmit ::= ENUMERATED
struct t_poll_retx_opts {
  enum options {
    ms5,
    ms10,
    ms15,
    ms20,
    ms25,
    ms30,
    ms35,
    ms40,
    ms45,
    ms50,
    ms55,
    ms60,
    ms65,
    ms70,
    ms75,
    ms80,
    ms85,
    ms90,
    ms95,
    ms100,
    ms105,
    ms110,
    ms115,
    ms120,
    ms125,
    ms130,
    ms135,
    ms140,
    ms145,
    ms150,
    ms155,
    ms160,
    ms165,
    ms170,
    ms175,
    ms180,
    ms185,
    ms190,
    ms195,
    ms200,
    ms205,
    ms210,
    ms215,
    ms220,
    ms225,
    ms230,
    ms235,
    ms240,
    ms245,
    ms250,
    ms300,
    ms350,
    ms400,
    ms450,
    ms500,
    ms800_v1310,
    ms1000_v1310,
    ms2000_v1310,
    ms4000_v1310,
    spare5,
    spare4,
    spare3,
    spare2,
    spare1,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<t_poll_retx_opts> t_poll_retx_e;

// T-Reordering ::= ENUMERATED
struct t_reordering_opts {
  enum options {
    ms0,
    ms5,
    ms10,
    ms15,
    ms20,
    ms25,
    ms30,
    ms35,
    ms40,
    ms45,
    ms50,
    ms55,
    ms60,
    ms65,
    ms70,
    ms75,
    ms80,
    ms85,
    ms90,
    ms95,
    ms100,
    ms110,
    ms120,
    ms130,
    ms140,
    ms150,
    ms160,
    ms170,
    ms180,
    ms190,
    ms200,
    ms1600_v1310,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<t_reordering_opts> t_reordering_e;

// T-StatusProhibit ::= ENUMERATED
struct t_status_prohibit_opts {
  enum options {
    ms0,
    ms5,
    ms10,
    ms15,
    ms20,
    ms25,
    ms30,
    ms35,
    ms40,
    ms45,
    ms50,
    ms55,
    ms60,
    ms65,
    ms70,
    ms75,
    ms80,
    ms85,
    ms90,
    ms95,
    ms100,
    ms105,
    ms110,
    ms115,
    ms120,
    ms125,
    ms130,
    ms135,
    ms140,
    ms145,
    ms150,
    ms155,
    ms160,
    ms165,
    ms170,
    ms175,
    ms180,
    ms185,
    ms190,
    ms195,
    ms200,
    ms205,
    ms210,
    ms215,
    ms220,
    ms225,
    ms230,
    ms235,
    ms240,
    ms245,
    ms250,
    ms300,
    ms350,
    ms400,
    ms450,
    ms500,
    ms800_v1310,
    ms1000_v1310,
    ms1200_v1310,
    ms1600_v1310,
    ms2000_v1310,
    ms2400_v1310,
    spare2,
    spare1,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<t_status_prohibit_opts> t_status_prohibit_e;

// DL-AM-RLC-r15 ::= SEQUENCE
struct dl_am_rlc_r15_s {
  t_reordering_e      t_reordering_r15;
  t_status_prohibit_e t_status_prohibit_r15;
  bool                extended_rlc_li_field_r15 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const dl_am_rlc_r15_s& other) const;
  bool        operator!=(const dl_am_rlc_r15_s& other) const { return not(*this == other); }
};

// DL-UM-RLC-r15 ::= SEQUENCE
struct dl_um_rlc_r15_s {
  sn_field_len_r15_e sn_field_len_r15;
  t_reordering_e     t_reordering_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const dl_um_rlc_r15_s& other) const;
  bool        operator!=(const dl_um_rlc_r15_s& other) const { return not(*this == other); }
};

// PollByte ::= ENUMERATED
struct poll_byte_opts {
  enum options {
    kb25,
    kb50,
    kb75,
    kb100,
    kb125,
    kb250,
    kb375,
    kb500,
    kb750,
    kb1000,
    kb1250,
    kb1500,
    kb2000,
    kb3000,
    kbinfinity,
    spare1,
    nulltype
  } value;
  typedef int16_t number_type;

  const char* to_string() const;
  int16_t     to_number() const;
};
typedef enumerated<poll_byte_opts> poll_byte_e;

// PollPDU ::= ENUMERATED
struct poll_pdu_opts {
  enum options { p4, p8, p16, p32, p64, p128, p256, pinfinity, nulltype } value;
  typedef int16_t number_type;

  const char* to_string() const;
  int16_t     to_number() const;
};
typedef enumerated<poll_pdu_opts> poll_pdu_e;

// UL-AM-RLC-r15 ::= SEQUENCE
struct ul_am_rlc_r15_s {
  struct max_retx_thres_r15_opts {
    enum options { t1, t2, t3, t4, t6, t8, t16, t32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_retx_thres_r15_opts> max_retx_thres_r15_e_;

  // member variables
  t_poll_retx_e         t_poll_retx_r15;
  poll_pdu_r15_e        poll_pdu_r15;
  poll_byte_r14_e       poll_byte_r15;
  max_retx_thres_r15_e_ max_retx_thres_r15;
  bool                  extended_rlc_li_field_r15 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_am_rlc_r15_s& other) const;
  bool        operator!=(const ul_am_rlc_r15_s& other) const { return not(*this == other); }
};

// UL-UM-RLC ::= SEQUENCE
struct ul_um_rlc_s {
  sn_field_len_e sn_field_len;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_um_rlc_s& other) const;
  bool        operator!=(const ul_um_rlc_s& other) const { return not(*this == other); }
};

// DL-AM-RLC ::= SEQUENCE
struct dl_am_rlc_s {
  t_reordering_e      t_reordering;
  t_status_prohibit_e t_status_prohibit;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const dl_am_rlc_s& other) const;
  bool        operator!=(const dl_am_rlc_s& other) const { return not(*this == other); }
};

// DL-UM-RLC ::= SEQUENCE
struct dl_um_rlc_s {
  sn_field_len_e sn_field_len;
  t_reordering_e t_reordering;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const dl_um_rlc_s& other) const;
  bool        operator!=(const dl_um_rlc_s& other) const { return not(*this == other); }
};

// DiscardTimerExt-r17 ::= ENUMERATED
struct discard_timer_ext_r17_opts {
  enum options { ms2000, spare, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<discard_timer_ext_r17_opts> discard_timer_ext_r17_e;

// EthernetHeaderCompression-r16 ::= SEQUENCE
struct ethernet_hdr_compress_r16_s {
  struct ehc_common_r16_s_ {
    struct ehc_cid_len_r16_opts {
      enum options { bits7, bits15, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<ehc_cid_len_r16_opts> ehc_cid_len_r16_e_;

    // member variables
    ehc_cid_len_r16_e_ ehc_cid_len_r16;
  };
  struct ehc_dl_r16_s_ {
    bool drb_continue_ehc_dl_r16_present = false;
  };
  struct ehc_ul_r16_s_ {
    bool     drb_continue_ehc_ul_r16_present = false;
    uint16_t max_cid_ehc_ul_r16              = 1;
  };

  // member variables
  bool              ext                = false;
  bool              ehc_dl_r16_present = false;
  bool              ehc_ul_r16_present = false;
  ehc_common_r16_s_ ehc_common_r16;
  ehc_dl_r16_s_     ehc_dl_r16;
  ehc_ul_r16_s_     ehc_ul_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ethernet_hdr_compress_r16_s& other) const;
  bool        operator!=(const ethernet_hdr_compress_r16_s& other) const { return not(*this == other); }
};

// LogicalChannelConfig ::= SEQUENCE
struct lc_ch_cfg_s {
  struct ul_specific_params_s_ {
    struct prioritised_bit_rate_opts {
      enum options {
        kbps0,
        kbps8,
        kbps16,
        kbps32,
        kbps64,
        kbps128,
        kbps256,
        infinity,
        kbps512_v1020,
        kbps1024_v1020,
        kbps2048_v1020,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef int16_t number_type;

      const char* to_string() const;
      int16_t     to_number() const;
    };
    typedef enumerated<prioritised_bit_rate_opts> prioritised_bit_rate_e_;
    struct bucket_size_dur_opts {
      enum options { ms50, ms100, ms150, ms300, ms500, ms1000, spare2, spare1, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<bucket_size_dur_opts> bucket_size_dur_e_;

    // member variables
    bool                    lc_ch_group_present = false;
    uint8_t                 prio                = 1;
    prioritised_bit_rate_e_ prioritised_bit_rate;
    bucket_size_dur_e_      bucket_size_dur;
    uint8_t                 lc_ch_group = 0;
  };
  struct bit_rate_query_prohibit_timer_r14_opts {
    enum options { s0, s0dot4, s0dot8, s1dot6, s3, s6, s12, s30, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<bit_rate_query_prohibit_timer_r14_opts> bit_rate_query_prohibit_timer_r14_e_;
  struct allowed_tti_lens_r15_c_ {
    struct setup_s_ {
      bool short_tti_r15 = false;
      bool sf_tti_r15    = false;
    };
    using types = setup_e;

    // choice methods
    allowed_tti_lens_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const allowed_tti_lens_r15_c_& other) const;
    bool        operator!=(const allowed_tti_lens_r15_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "allowedTTI-Lengths-r15");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "allowedTTI-Lengths-r15");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct lc_ch_sr_restrict_r15_c_ {
    struct setup_opts {
      enum options { spucch, pucch, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<setup_opts> setup_e_;
    using types = setup_e;

    // choice methods
    lc_ch_sr_restrict_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const lc_ch_sr_restrict_r15_c_& other) const;
    bool        operator!=(const lc_ch_sr_restrict_r15_c_& other) const { return not(*this == other); }
    // getters
    setup_e_& setup()
    {
      assert_choice_type(types::setup, type_, "logicalChannelSR-Restriction-r15");
      return c;
    }
    const setup_e_& setup() const
    {
      assert_choice_type(types::setup, type_, "logicalChannelSR-Restriction-r15");
      return c;
    }
    void      set_release();
    setup_e_& set_setup();

  private:
    types    type_;
    setup_e_ c;
  };
  struct ch_access_prio_r15_c_ {
    using types = setup_e;

    // choice methods
    ch_access_prio_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const ch_access_prio_r15_c_& other) const;
    bool        operator!=(const ch_access_prio_r15_c_& other) const { return not(*this == other); }
    // getters
    uint8_t& setup()
    {
      assert_choice_type(types::setup, type_, "channelAccessPriority-r15");
      return c;
    }
    const uint8_t& setup() const
    {
      assert_choice_type(types::setup, type_, "channelAccessPriority-r15");
      return c;
    }
    void     set_release();
    uint8_t& set_setup();

  private:
    types   type_;
    uint8_t c;
  };
  struct bit_rate_multiplier_r16_opts {
    enum options { x40, x70, x100, x200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<bit_rate_multiplier_r16_opts> bit_rate_multiplier_r16_e_;

  // member variables
  bool                  ext                        = false;
  bool                  ul_specific_params_present = false;
  ul_specific_params_s_ ul_specific_params;
  // ...
  // group 0
  bool lc_ch_sr_mask_r9_present = false;
  // group 1
  bool lc_ch_sr_prohibit_r12_present = false;
  bool lc_ch_sr_prohibit_r12         = false;
  // group 2
  bool                                 laa_ul_allowed_r14_present                = false;
  bool                                 bit_rate_query_prohibit_timer_r14_present = false;
  bool                                 laa_ul_allowed_r14                        = false;
  bit_rate_query_prohibit_timer_r14_e_ bit_rate_query_prohibit_timer_r14;
  // group 3
  bool                               lch_cell_restrict_r15_present = false;
  copy_ptr<allowed_tti_lens_r15_c_>  allowed_tti_lens_r15;
  copy_ptr<lc_ch_sr_restrict_r15_c_> lc_ch_sr_restrict_r15;
  copy_ptr<ch_access_prio_r15_c_>    ch_access_prio_r15;
  fixed_bitstring<32>                lch_cell_restrict_r15;
  // group 4
  bool                       bit_rate_multiplier_r16_present = false;
  bit_rate_multiplier_r16_e_ bit_rate_multiplier_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const lc_ch_cfg_s& other) const;
  bool        operator!=(const lc_ch_cfg_s& other) const { return not(*this == other); }
};

// P-a ::= ENUMERATED
struct p_a_opts {
  enum options { db_minus6, db_minus4dot77, db_minus3, db_minus1dot77, db0, db1, db2, db3, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
typedef enumerated<p_a_opts> p_a_e;

// PollPDU-v1310 ::= ENUMERATED
struct poll_pdu_v1310_opts {
  enum options { p512, p1024, p2048, p4096, p6144, p8192, p12288, p16384, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<poll_pdu_v1310_opts> poll_pdu_v1310_e;

// RLC-Config-r15 ::= SEQUENCE
struct rlc_cfg_r15_s {
  struct mode_r15_c_ {
    struct am_r15_s_ {
      ul_am_rlc_r15_s ul_am_rlc_r15;
      dl_am_rlc_r15_s dl_am_rlc_r15;
    };
    struct um_bi_dir_r15_s_ {
      ul_um_rlc_s     ul_um_rlc_r15;
      dl_um_rlc_r15_s dl_um_rlc_r15;
    };
    struct um_uni_dir_ul_r15_s_ {
      ul_um_rlc_s ul_um_rlc_r15;
    };
    struct um_uni_dir_dl_r15_s_ {
      dl_um_rlc_r15_s dl_um_rlc_r15;
    };
    struct types_opts {
      enum options { am_r15, um_bi_dir_r15, um_uni_dir_ul_r15, um_uni_dir_dl_r15, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    mode_r15_c_() = default;
    mode_r15_c_(const mode_r15_c_& other);
    mode_r15_c_& operator=(const mode_r15_c_& other);
    ~mode_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const mode_r15_c_& other) const;
    bool        operator!=(const mode_r15_c_& other) const { return not(*this == other); }
    // getters
    am_r15_s_& am_r15()
    {
      assert_choice_type(types::am_r15, type_, "mode-r15");
      return c.get<am_r15_s_>();
    }
    um_bi_dir_r15_s_& um_bi_dir_r15()
    {
      assert_choice_type(types::um_bi_dir_r15, type_, "mode-r15");
      return c.get<um_bi_dir_r15_s_>();
    }
    um_uni_dir_ul_r15_s_& um_uni_dir_ul_r15()
    {
      assert_choice_type(types::um_uni_dir_ul_r15, type_, "mode-r15");
      return c.get<um_uni_dir_ul_r15_s_>();
    }
    um_uni_dir_dl_r15_s_& um_uni_dir_dl_r15()
    {
      assert_choice_type(types::um_uni_dir_dl_r15, type_, "mode-r15");
      return c.get<um_uni_dir_dl_r15_s_>();
    }
    const am_r15_s_& am_r15() const
    {
      assert_choice_type(types::am_r15, type_, "mode-r15");
      return c.get<am_r15_s_>();
    }
    const um_bi_dir_r15_s_& um_bi_dir_r15() const
    {
      assert_choice_type(types::um_bi_dir_r15, type_, "mode-r15");
      return c.get<um_bi_dir_r15_s_>();
    }
    const um_uni_dir_ul_r15_s_& um_uni_dir_ul_r15() const
    {
      assert_choice_type(types::um_uni_dir_ul_r15, type_, "mode-r15");
      return c.get<um_uni_dir_ul_r15_s_>();
    }
    const um_uni_dir_dl_r15_s_& um_uni_dir_dl_r15() const
    {
      assert_choice_type(types::um_uni_dir_dl_r15, type_, "mode-r15");
      return c.get<um_uni_dir_dl_r15_s_>();
    }
    am_r15_s_&            set_am_r15();
    um_bi_dir_r15_s_&     set_um_bi_dir_r15();
    um_uni_dir_ul_r15_s_& set_um_uni_dir_ul_r15();
    um_uni_dir_dl_r15_s_& set_um_uni_dir_dl_r15();

  private:
    types                                                                                    type_;
    choice_buffer_t<am_r15_s_, um_bi_dir_r15_s_, um_uni_dir_dl_r15_s_, um_uni_dir_ul_r15_s_> c;

    void destroy_();
  };

  // member variables
  bool        ext                                   = false;
  bool        reestablish_rlc_r15_present           = false;
  bool        rlc_out_of_order_delivery_r15_present = false;
  mode_r15_c_ mode_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rlc_cfg_r15_s& other) const;
  bool        operator!=(const rlc_cfg_r15_s& other) const { return not(*this == other); }
};

// T-ReorderingExt-r17 ::= ENUMERATED
struct t_reordering_ext_r17_opts {
  enum options { ms2200, ms3200, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<t_reordering_ext_r17_opts> t_reordering_ext_r17_e;

// UL-AM-RLC ::= SEQUENCE
struct ul_am_rlc_s {
  struct max_retx_thres_opts {
    enum options { t1, t2, t3, t4, t6, t8, t16, t32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_retx_thres_opts> max_retx_thres_e_;

  // member variables
  t_poll_retx_e     t_poll_retx;
  poll_pdu_e        poll_pdu;
  poll_byte_e       poll_byte;
  max_retx_thres_e_ max_retx_thres;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_am_rlc_s& other) const;
  bool        operator!=(const ul_am_rlc_s& other) const { return not(*this == other); }
};

// CRS-AssistanceInfo-r11 ::= SEQUENCE
struct crs_assist_info_r11_s {
  struct ant_ports_count_r11_opts {
    enum options { an1, an2, an4, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ant_ports_count_r11_opts> ant_ports_count_r11_e_;

  // member variables
  bool                   ext     = false;
  uint16_t               pci_r11 = 0;
  ant_ports_count_r11_e_ ant_ports_count_r11;
  mbsfn_sf_cfg_list_l    mbsfn_sf_cfg_list_r11;
  // ...
  // group 0
  copy_ptr<mbsfn_sf_cfg_list_v1430_l> mbsfn_sf_cfg_list_v1430;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CRS-AssistanceInfo-r13 ::= SEQUENCE
struct crs_assist_info_r13_s {
  struct ant_ports_count_r13_opts {
    enum options { an1, an2, an4, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ant_ports_count_r13_opts> ant_ports_count_r13_e_;

  // member variables
  bool                   ext                           = false;
  bool                   mbsfn_sf_cfg_list_r13_present = false;
  uint16_t               pci_r13                       = 0;
  ant_ports_count_r13_e_ ant_ports_count_r13;
  mbsfn_sf_cfg_list_l    mbsfn_sf_cfg_list_r13;
  // ...
  // group 0
  copy_ptr<mbsfn_sf_cfg_list_v1430_l> mbsfn_sf_cfg_list_v1430;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const crs_assist_info_r13_s& other) const;
  bool        operator!=(const crs_assist_info_r13_s& other) const { return not(*this == other); }
};

// CRS-AssistanceInfo-r15 ::= SEQUENCE
struct crs_assist_info_r15_s {
  bool     crs_intf_mitig_enabled_r15_present = false;
  uint16_t pci_r15                            = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const crs_assist_info_r15_s& other) const;
  bool        operator!=(const crs_assist_info_r15_s& other) const { return not(*this == other); }
};

// N1PUCCH-AN-PersistentList ::= SEQUENCE (SIZE (1..4)) OF INTEGER (0..2047)
using n1_pucch_an_persistent_list_l = bounded_array<uint16_t, 4>;

// N1SPUCCH-AN-PersistentList-r15 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (0..2047)
using n1_spucch_an_persistent_list_r15_l = bounded_array<uint16_t, 4>;

// NeighCellsInfo-r12 ::= SEQUENCE
struct neigh_cells_info_r12_s {
  struct crs_ports_count_r12_opts {
    enum options { n1, n2, n4, spare, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<crs_ports_count_r12_opts> crs_ports_count_r12_e_;
  using p_a_list_r12_l_ = bounded_array<p_a_e, 3>;

  // member variables
  bool                   ext                      = false;
  bool                   mbsfn_sf_cfg_r12_present = false;
  uint16_t               pci_r12                  = 0;
  uint8_t                p_b_r12                  = 0;
  crs_ports_count_r12_e_ crs_ports_count_r12;
  mbsfn_sf_cfg_list_l    mbsfn_sf_cfg_r12;
  p_a_list_r12_l_        p_a_list_r12;
  fixed_bitstring<8>     tx_mode_list_r12;
  uint8_t                res_alloc_granularity_r12 = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const neigh_cells_info_r12_s& other) const;
  bool        operator!=(const neigh_cells_info_r12_s& other) const { return not(*this == other); }
};

// PDCP-Config ::= SEQUENCE
struct pdcp_cfg_s {
  struct discard_timer_opts {
    enum options { ms50, ms100, ms150, ms300, ms500, ms750, ms1500, infinity, nulltype } value;
    typedef int16_t number_type;

    const char* to_string() const;
    int16_t     to_number() const;
  };
  typedef enumerated<discard_timer_opts> discard_timer_e_;
  struct rlc_am_s_ {
    bool status_report_required = false;
  };
  struct rlc_um_s_ {
    struct pdcp_sn_size_opts {
      enum options { len7bits, len12bits, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<pdcp_sn_size_opts> pdcp_sn_size_e_;

    // member variables
    pdcp_sn_size_e_ pdcp_sn_size;
  };
  struct hdr_compress_c_ {
    struct rohc_s_ {
      struct profiles_s_ {
        bool profile0x0001 = false;
        bool profile0x0002 = false;
        bool profile0x0003 = false;
        bool profile0x0004 = false;
        bool profile0x0006 = false;
        bool profile0x0101 = false;
        bool profile0x0102 = false;
        bool profile0x0103 = false;
        bool profile0x0104 = false;
      };

      // member variables
      bool        ext             = false;
      bool        max_cid_present = false;
      uint16_t    max_cid         = 1;
      profiles_s_ profiles;
      // ...
    };
    struct types_opts {
      enum options { not_used, rohc, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    hdr_compress_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const hdr_compress_c_& other) const;
    bool        operator!=(const hdr_compress_c_& other) const { return not(*this == other); }
    // getters
    rohc_s_& rohc()
    {
      assert_choice_type(types::rohc, type_, "headerCompression");
      return c;
    }
    const rohc_s_& rohc() const
    {
      assert_choice_type(types::rohc, type_, "headerCompression");
      return c;
    }
    void     set_not_used();
    rohc_s_& set_rohc();

  private:
    types   type_;
    rohc_s_ c;
  };
  struct t_reordering_r12_opts {
    enum options {
      ms0,
      ms20,
      ms40,
      ms60,
      ms80,
      ms100,
      ms120,
      ms140,
      ms160,
      ms180,
      ms200,
      ms220,
      ms240,
      ms260,
      ms280,
      ms300,
      ms500,
      ms750,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t_reordering_r12_opts> t_reordering_r12_e_;
  struct ul_data_split_thres_r13_c_ {
    struct setup_opts {
      enum options {
        b0,
        b100,
        b200,
        b400,
        b800,
        b1600,
        b3200,
        b6400,
        b12800,
        b25600,
        b51200,
        b102400,
        b204800,
        b409600,
        b819200,
        spare1,
        nulltype
      } value;
      typedef uint32_t number_type;

      const char* to_string() const;
      uint32_t    to_number() const;
    };
    typedef enumerated<setup_opts> setup_e_;
    using types = setup_e;

    // choice methods
    ul_data_split_thres_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const ul_data_split_thres_r13_c_& other) const;
    bool        operator!=(const ul_data_split_thres_r13_c_& other) const { return not(*this == other); }
    // getters
    setup_e_& setup()
    {
      assert_choice_type(types::setup, type_, "ul-DataSplitThreshold-r13");
      return c;
    }
    const setup_e_& setup() const
    {
      assert_choice_type(types::setup, type_, "ul-DataSplitThreshold-r13");
      return c;
    }
    void      set_release();
    setup_e_& set_setup();

  private:
    types    type_;
    setup_e_ c;
  };
  struct status_feedback_r13_c_ {
    struct setup_s_ {
      struct status_pdu_type_for_polling_r13_opts {
        enum options { type1, type2, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<status_pdu_type_for_polling_r13_opts> status_pdu_type_for_polling_r13_e_;
      struct status_pdu_periodicity_type1_r13_opts {
        enum options {
          ms5,
          ms10,
          ms20,
          ms30,
          ms40,
          ms50,
          ms60,
          ms70,
          ms80,
          ms90,
          ms100,
          ms150,
          ms200,
          ms300,
          ms500,
          ms1000,
          ms2000,
          ms5000,
          ms10000,
          ms20000,
          ms50000,
          nulltype
        } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<status_pdu_periodicity_type1_r13_opts> status_pdu_periodicity_type1_r13_e_;
      struct status_pdu_periodicity_type2_r13_opts {
        enum options {
          ms5,
          ms10,
          ms20,
          ms30,
          ms40,
          ms50,
          ms60,
          ms70,
          ms80,
          ms90,
          ms100,
          ms150,
          ms200,
          ms300,
          ms500,
          ms1000,
          ms2000,
          ms5000,
          ms10000,
          ms20000,
          ms50000,
          nulltype
        } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<status_pdu_periodicity_type2_r13_opts> status_pdu_periodicity_type2_r13_e_;
      struct status_pdu_periodicity_offset_r13_opts {
        enum options { ms1, ms2, ms5, ms10, ms25, ms50, ms100, ms250, ms500, ms2500, ms5000, ms25000, nulltype } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<status_pdu_periodicity_offset_r13_opts> status_pdu_periodicity_offset_r13_e_;

      // member variables
      bool                                 status_pdu_type_for_polling_r13_present   = false;
      bool                                 status_pdu_periodicity_type1_r13_present  = false;
      bool                                 status_pdu_periodicity_type2_r13_present  = false;
      bool                                 status_pdu_periodicity_offset_r13_present = false;
      status_pdu_type_for_polling_r13_e_   status_pdu_type_for_polling_r13;
      status_pdu_periodicity_type1_r13_e_  status_pdu_periodicity_type1_r13;
      status_pdu_periodicity_type2_r13_e_  status_pdu_periodicity_type2_r13;
      status_pdu_periodicity_offset_r13_e_ status_pdu_periodicity_offset_r13;
    };
    using types = setup_e;

    // choice methods
    status_feedback_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const status_feedback_r13_c_& other) const;
    bool        operator!=(const status_feedback_r13_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "statusFeedback-r13");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "statusFeedback-r13");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct ul_lwa_cfg_r14_c_ {
    struct setup_s_ {
      struct ul_lwa_data_split_thres_r14_opts {
        enum options {
          b0,
          b100,
          b200,
          b400,
          b800,
          b1600,
          b3200,
          b6400,
          b12800,
          b25600,
          b51200,
          b102400,
          b204800,
          b409600,
          b819200,
          nulltype
        } value;
        typedef uint32_t number_type;

        const char* to_string() const;
        uint32_t    to_number() const;
      };
      typedef enumerated<ul_lwa_data_split_thres_r14_opts> ul_lwa_data_split_thres_r14_e_;

      // member variables
      bool                           ul_lwa_data_split_thres_r14_present = false;
      bool                           ul_lwa_drb_via_wlan_r14             = false;
      ul_lwa_data_split_thres_r14_e_ ul_lwa_data_split_thres_r14;
    };
    using types = setup_e;

    // choice methods
    ul_lwa_cfg_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const ul_lwa_cfg_r14_c_& other) const;
    bool        operator!=(const ul_lwa_cfg_r14_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "ul-LWA-Config-r14");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "ul-LWA-Config-r14");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct ul_only_hdr_compress_r14_c_ {
    struct rohc_r14_s_ {
      struct profiles_r14_s_ {
        bool profile0x0006_r14 = false;
      };

      // member variables
      bool            ext                 = false;
      bool            max_cid_r14_present = false;
      uint16_t        max_cid_r14         = 1;
      profiles_r14_s_ profiles_r14;
      // ...
    };
    struct types_opts {
      enum options { not_used_r14, rohc_r14, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ul_only_hdr_compress_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const ul_only_hdr_compress_r14_c_& other) const;
    bool        operator!=(const ul_only_hdr_compress_r14_c_& other) const { return not(*this == other); }
    // getters
    rohc_r14_s_& rohc_r14()
    {
      assert_choice_type(types::rohc_r14, type_, "uplinkOnlyHeaderCompression-r14");
      return c;
    }
    const rohc_r14_s_& rohc_r14() const
    {
      assert_choice_type(types::rohc_r14, type_, "uplinkOnlyHeaderCompression-r14");
      return c;
    }
    void         set_not_used_r14();
    rohc_r14_s_& set_rohc_r14();

  private:
    types       type_;
    rohc_r14_s_ c;
  };
  struct ul_data_compress_r15_s_ {
    struct buffer_size_r15_opts {
      enum options { kbyte2, kbyte4, kbyte8, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<buffer_size_r15_opts> buffer_size_r15_e_;
    struct dictionary_r15_opts {
      enum options { sip_sdp, operator_value, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<dictionary_r15_opts> dictionary_r15_e_;

    // member variables
    bool               ext                    = false;
    bool               dictionary_r15_present = false;
    buffer_size_r15_e_ buffer_size_r15;
    dictionary_r15_e_  dictionary_r15;
    // ...
  };
  struct pdcp_dupl_cfg_r15_c_ {
    struct setup_s_ {
      struct pdcp_dupl_r15_opts {
        enum options { cfgured, activ, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<pdcp_dupl_r15_opts> pdcp_dupl_r15_e_;

      // member variables
      pdcp_dupl_r15_e_ pdcp_dupl_r15;
    };
    using types = setup_e;

    // choice methods
    pdcp_dupl_cfg_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const pdcp_dupl_cfg_r15_c_& other) const;
    bool        operator!=(const pdcp_dupl_cfg_r15_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "pdcp-DuplicationConfig-r15");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "pdcp-DuplicationConfig-r15");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool             ext                   = false;
  bool             discard_timer_present = false;
  bool             rlc_am_present        = false;
  bool             rlc_um_present        = false;
  discard_timer_e_ discard_timer;
  rlc_am_s_        rlc_am;
  rlc_um_s_        rlc_um;
  hdr_compress_c_  hdr_compress;
  // ...
  // group 0
  bool rn_integrity_protection_r10_present = false;
  // group 1
  bool pdcp_sn_size_v1130_present = false;
  // group 2
  bool                ul_data_split_drb_via_scg_r12_present = false;
  bool                t_reordering_r12_present              = false;
  bool                ul_data_split_drb_via_scg_r12         = false;
  t_reordering_r12_e_ t_reordering_r12;
  // group 3
  bool                                 pdcp_sn_size_v1310_present = false;
  copy_ptr<ul_data_split_thres_r13_c_> ul_data_split_thres_r13;
  copy_ptr<status_feedback_r13_c_>     status_feedback_r13;
  // group 4
  copy_ptr<ul_lwa_cfg_r14_c_>           ul_lwa_cfg_r14;
  copy_ptr<ul_only_hdr_compress_r14_c_> ul_only_hdr_compress_r14;
  // group 5
  copy_ptr<ul_data_compress_r15_s_> ul_data_compress_r15;
  copy_ptr<pdcp_dupl_cfg_r15_c_>    pdcp_dupl_cfg_r15;
  // group 6
  copy_ptr<setup_release_c<ethernet_hdr_compress_r16_s> > ethernet_hdr_compress_r16;
  // group 7
  copy_ptr<setup_release_c<discard_timer_ext_r17_e> > discard_timer_ext_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdcp_cfg_s& other) const;
  bool        operator!=(const pdcp_cfg_s& other) const { return not(*this == other); }
};

// RLC-BearerConfig-r15 ::= CHOICE
struct rlc_bearer_cfg_r15_c {
  struct setup_s_ {
    struct lc_ch_id_cfg_r15_c_ {
      struct types_opts {
        enum options { lc_ch_id_r15, lc_ch_id_ext_r15, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      lc_ch_id_cfg_r15_c_() = default;
      lc_ch_id_cfg_r15_c_(const lc_ch_id_cfg_r15_c_& other);
      lc_ch_id_cfg_r15_c_& operator=(const lc_ch_id_cfg_r15_c_& other);
      ~lc_ch_id_cfg_r15_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const lc_ch_id_cfg_r15_c_& other) const;
      bool        operator!=(const lc_ch_id_cfg_r15_c_& other) const { return not(*this == other); }
      // getters
      uint8_t& lc_ch_id_r15()
      {
        assert_choice_type(types::lc_ch_id_r15, type_, "logicalChannelIdentityConfig-r15");
        return c.get<uint8_t>();
      }
      uint8_t& lc_ch_id_ext_r15()
      {
        assert_choice_type(types::lc_ch_id_ext_r15, type_, "logicalChannelIdentityConfig-r15");
        return c.get<uint8_t>();
      }
      const uint8_t& lc_ch_id_r15() const
      {
        assert_choice_type(types::lc_ch_id_r15, type_, "logicalChannelIdentityConfig-r15");
        return c.get<uint8_t>();
      }
      const uint8_t& lc_ch_id_ext_r15() const
      {
        assert_choice_type(types::lc_ch_id_ext_r15, type_, "logicalChannelIdentityConfig-r15");
        return c.get<uint8_t>();
      }
      uint8_t& set_lc_ch_id_r15();
      uint8_t& set_lc_ch_id_ext_r15();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };

    // member variables
    bool                rlc_cfg_r15_present   = false;
    bool                lc_ch_cfg_r15_present = false;
    rlc_cfg_r15_s       rlc_cfg_r15;
    lc_ch_id_cfg_r15_c_ lc_ch_id_cfg_r15;
    lc_ch_cfg_s         lc_ch_cfg_r15;
  };
  using types = setup_e;

  // choice methods
  rlc_bearer_cfg_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rlc_bearer_cfg_r15_c& other) const;
  bool        operator!=(const rlc_bearer_cfg_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "RLC-BearerConfig-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "RLC-BearerConfig-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// RLC-Config ::= CHOICE
struct rlc_cfg_c {
  struct am_s_ {
    ul_am_rlc_s ul_am_rlc;
    dl_am_rlc_s dl_am_rlc;
  };
  struct um_bi_dir_s_ {
    ul_um_rlc_s ul_um_rlc;
    dl_um_rlc_s dl_um_rlc;
  };
  struct um_uni_dir_ul_s_ {
    ul_um_rlc_s ul_um_rlc;
  };
  struct um_uni_dir_dl_s_ {
    dl_um_rlc_s dl_um_rlc;
  };
  struct types_opts {
    enum options { am, um_bi_dir, um_uni_dir_ul, um_uni_dir_dl, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  rlc_cfg_c() = default;
  rlc_cfg_c(const rlc_cfg_c& other);
  rlc_cfg_c& operator=(const rlc_cfg_c& other);
  ~rlc_cfg_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rlc_cfg_c& other) const;
  bool        operator!=(const rlc_cfg_c& other) const { return not(*this == other); }
  // getters
  am_s_& am()
  {
    assert_choice_type(types::am, type_, "RLC-Config");
    return c.get<am_s_>();
  }
  um_bi_dir_s_& um_bi_dir()
  {
    assert_choice_type(types::um_bi_dir, type_, "RLC-Config");
    return c.get<um_bi_dir_s_>();
  }
  um_uni_dir_ul_s_& um_uni_dir_ul()
  {
    assert_choice_type(types::um_uni_dir_ul, type_, "RLC-Config");
    return c.get<um_uni_dir_ul_s_>();
  }
  um_uni_dir_dl_s_& um_uni_dir_dl()
  {
    assert_choice_type(types::um_uni_dir_dl, type_, "RLC-Config");
    return c.get<um_uni_dir_dl_s_>();
  }
  const am_s_& am() const
  {
    assert_choice_type(types::am, type_, "RLC-Config");
    return c.get<am_s_>();
  }
  const um_bi_dir_s_& um_bi_dir() const
  {
    assert_choice_type(types::um_bi_dir, type_, "RLC-Config");
    return c.get<um_bi_dir_s_>();
  }
  const um_uni_dir_ul_s_& um_uni_dir_ul() const
  {
    assert_choice_type(types::um_uni_dir_ul, type_, "RLC-Config");
    return c.get<um_uni_dir_ul_s_>();
  }
  const um_uni_dir_dl_s_& um_uni_dir_dl() const
  {
    assert_choice_type(types::um_uni_dir_dl, type_, "RLC-Config");
    return c.get<um_uni_dir_dl_s_>();
  }
  am_s_&            set_am();
  um_bi_dir_s_&     set_um_bi_dir();
  um_uni_dir_ul_s_& set_um_uni_dir_ul();
  um_uni_dir_dl_s_& set_um_uni_dir_dl();

private:
  types                                                                    type_;
  choice_buffer_t<am_s_, um_bi_dir_s_, um_uni_dir_dl_s_, um_uni_dir_ul_s_> c;

  void destroy_();
};

// RLC-Config-v1250 ::= SEQUENCE
struct rlc_cfg_v1250_s {
  bool ul_extended_rlc_li_field_r12 = false;
  bool dl_extended_rlc_li_field_r12 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rlc_cfg_v1250_s& other) const;
  bool        operator!=(const rlc_cfg_v1250_s& other) const { return not(*this == other); }
};

// RLC-Config-v1310 ::= SEQUENCE
struct rlc_cfg_v1310_s {
  bool             poll_pdu_v1310_present    = false;
  bool             ul_extended_rlc_am_sn_r13 = false;
  bool             dl_extended_rlc_am_sn_r13 = false;
  poll_pdu_v1310_e poll_pdu_v1310;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rlc_cfg_v1310_s& other) const;
  bool        operator!=(const rlc_cfg_v1310_s& other) const { return not(*this == other); }
};

// RLC-Config-v1430 ::= CHOICE
struct rlc_cfg_v1430_c {
  struct setup_s_ {
    poll_byte_r14_e poll_byte_r14;
  };
  using types = setup_e;

  // choice methods
  rlc_cfg_v1430_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rlc_cfg_v1430_c& other) const;
  bool        operator!=(const rlc_cfg_v1430_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "RLC-Config-v1430");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "RLC-Config-v1430");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// RLC-Config-v1510 ::= SEQUENCE
struct rlc_cfg_v1510_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rlc_cfg_v1510_s& other) const;
  bool        operator!=(const rlc_cfg_v1510_s& other) const { return not(*this == other); }
};

// RLC-Config-v1530 ::= CHOICE
struct rlc_cfg_v1530_c {
  struct setup_s_ {};
  using types = setup_e;

  // choice methods
  rlc_cfg_v1530_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rlc_cfg_v1530_c& other) const;
  bool        operator!=(const rlc_cfg_v1530_c& other) const { return not(*this == other); }
  // getters
  void set_release();
  void set_setup();

private:
  types type_;
};

// RLC-Config-v1700 ::= SEQUENCE
struct rlc_cfg_v1700_s {
  setup_release_c<t_reordering_ext_r17_e> t_reordering_ext_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rlc_cfg_v1700_s& other) const;
  bool        operator!=(const rlc_cfg_v1700_s& other) const { return not(*this == other); }
};

// SPS-ConfigSL-r14 ::= SEQUENCE
struct sps_cfg_sl_r14_s {
  struct semi_persist_sched_interv_sl_r14_opts {
    enum options {
      sf20,
      sf50,
      sf100,
      sf200,
      sf300,
      sf400,
      sf500,
      sf600,
      sf700,
      sf800,
      sf900,
      sf1000,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<semi_persist_sched_interv_sl_r14_opts> semi_persist_sched_interv_sl_r14_e_;

  // member variables
  uint8_t                             sps_cfg_idx_r14 = 1;
  semi_persist_sched_interv_sl_r14_e_ semi_persist_sched_interv_sl_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SPS-ConfigUL ::= CHOICE
struct sps_cfg_ul_c {
  struct setup_s_ {
    struct semi_persist_sched_interv_ul_opts {
      enum options {
        sf10,
        sf20,
        sf32,
        sf40,
        sf64,
        sf80,
        sf128,
        sf160,
        sf320,
        sf640,
        sf1_v1430,
        sf2_v1430,
        sf3_v1430,
        sf4_v1430,
        sf5_v1430,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<semi_persist_sched_interv_ul_opts> semi_persist_sched_interv_ul_e_;
    struct implicit_release_after_opts {
      enum options { e2, e3, e4, e8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<implicit_release_after_opts> implicit_release_after_e_;
    struct p0_persistent_s_ {
      int8_t p0_nominal_pusch_persistent = -126;
      int8_t p0_ue_pusch_persistent      = -8;
    };
    struct p0_persistent_sf_set2_r12_c_ {
      struct setup_s_ {
        int8_t p0_nominal_pusch_persistent_sf_set2_r12 = -126;
        int8_t p0_ue_pusch_persistent_sf_set2_r12      = -8;
      };
      using types = setup_e;

      // choice methods
      p0_persistent_sf_set2_r12_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const p0_persistent_sf_set2_r12_c_& other) const;
      bool        operator!=(const p0_persistent_sf_set2_r12_c_& other) const { return not(*this == other); }
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "p0-PersistentSubframeSet2-r12");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "p0-PersistentSubframeSet2-r12");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };
    struct semi_persist_sched_interv_ul_v1430_opts {
      enum options {
        sf50,
        sf100,
        sf200,
        sf300,
        sf400,
        sf500,
        sf600,
        sf700,
        sf800,
        sf900,
        sf1000,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<semi_persist_sched_interv_ul_v1430_opts> semi_persist_sched_interv_ul_v1430_e_;
    struct cyclic_shift_sps_r15_opts {
      enum options { cs0, cs1, cs2, cs3, cs4, cs5, cs6, cs7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<cyclic_shift_sps_r15_opts> cyclic_shift_sps_r15_e_;
    struct rv_sps_ul_repeats_r15_opts {
      enum options { ulrvseq1, ulrvseq2, ulrvseq3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<rv_sps_ul_repeats_r15_opts> rv_sps_ul_repeats_r15_e_;
    struct total_num_pusch_sps_ul_repeats_r15_opts {
      enum options { n2, n3, n4, n6, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<total_num_pusch_sps_ul_repeats_r15_opts> total_num_pusch_sps_ul_repeats_r15_e_;

    // member variables
    bool                            ext                     = false;
    bool                            p0_persistent_present   = false;
    bool                            two_intervs_cfg_present = false;
    semi_persist_sched_interv_ul_e_ semi_persist_sched_interv_ul;
    implicit_release_after_e_       implicit_release_after;
    p0_persistent_s_                p0_persistent;
    // ...
    // group 0
    copy_ptr<p0_persistent_sf_set2_r12_c_> p0_persistent_sf_set2_r12;
    // group 1
    bool    nof_conf_ul_sps_processes_r13_present = false;
    uint8_t nof_conf_ul_sps_processes_r13         = 1;
    // group 2
    bool                                  fixed_rv_non_adaptive_r14_present          = false;
    bool                                  sps_cfg_idx_r14_present                    = false;
    bool                                  semi_persist_sched_interv_ul_v1430_present = false;
    uint8_t                               sps_cfg_idx_r14                            = 1;
    semi_persist_sched_interv_ul_v1430_e_ semi_persist_sched_interv_ul_v1430;
    // group 3
    bool                                  cyclic_shift_sps_r15_present               = false;
    bool                                  harq_proc_id_offset_r15_present            = false;
    bool                                  rv_sps_ul_repeats_r15_present              = false;
    bool                                  total_num_pusch_sps_ul_repeats_r15_present = false;
    bool                                  sps_cfg_idx_r15_present                    = false;
    cyclic_shift_sps_r15_e_               cyclic_shift_sps_r15;
    uint8_t                               harq_proc_id_offset_r15 = 0;
    rv_sps_ul_repeats_r15_e_              rv_sps_ul_repeats_r15;
    copy_ptr<tpc_pdcch_cfg_c>             tpc_pdcch_cfg_pusch_sps_r15;
    total_num_pusch_sps_ul_repeats_r15_e_ total_num_pusch_sps_ul_repeats_r15;
    uint8_t                               sps_cfg_idx_r15 = 1;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const setup_s_& other) const;
    bool        operator!=(const setup_s_& other) const { return not(*this == other); }
  };
  using types = setup_e;

  // choice methods
  sps_cfg_ul_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const sps_cfg_ul_c& other) const;
  bool        operator!=(const sps_cfg_ul_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SPS-ConfigUL");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SPS-ConfigUL");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SPS-ConfigUL-STTI-r15 ::= CHOICE
struct sps_cfg_ul_stti_r15_c {
  struct setup_s_ {
    struct semi_persist_sched_interv_ul_stti_r15_opts {
      enum options {
        stti1,
        stti2,
        stti3,
        stti4,
        stti6,
        stti8,
        stti12,
        stti16,
        stti20,
        stti40,
        stti60,
        stti80,
        stti120,
        stti240,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<semi_persist_sched_interv_ul_stti_r15_opts> semi_persist_sched_interv_ul_stti_r15_e_;
    struct implicit_release_after_opts {
      enum options { e2, e3, e4, e8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<implicit_release_after_opts> implicit_release_after_e_;
    struct p0_persistent_r15_s_ {
      int8_t p0_nominal_spusch_persistent_r15 = -126;
      int8_t p0_ue_spusch_persistent_r15      = -8;
    };
    struct p0_persistent_sf_set2_r15_c_ {
      struct setup_s_ {
        int8_t p0_nominal_spusch_persistent_sf_set2_r15 = -126;
        int8_t p0_ue_spusch_persistent_sf_set2_r15      = -8;
      };
      using types = setup_e;

      // choice methods
      p0_persistent_sf_set2_r15_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const p0_persistent_sf_set2_r15_c_& other) const;
      bool        operator!=(const p0_persistent_sf_set2_r15_c_& other) const { return not(*this == other); }
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "p0-PersistentSubframeSet2-r15");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "p0-PersistentSubframeSet2-r15");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };
    struct cyclic_shift_sps_s_tti_r15_opts {
      enum options { cs0, cs1, cs2, cs3, cs4, cs5, cs6, cs7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<cyclic_shift_sps_s_tti_r15_opts> cyclic_shift_sps_s_tti_r15_e_;
    struct rv_sps_stti_ul_repeats_r15_opts {
      enum options { ulrvseq1, ulrvseq2, ulrvseq3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<rv_sps_stti_ul_repeats_r15_opts> rv_sps_stti_ul_repeats_r15_e_;
    struct tbs_scaling_factor_subslot_sps_ul_repeats_r15_opts {
      enum options { n6, n12, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<tbs_scaling_factor_subslot_sps_ul_repeats_r15_opts>
        tbs_scaling_factor_subslot_sps_ul_repeats_r15_e_;
    struct total_num_pusch_sps_stti_ul_repeats_r15_opts {
      enum options { n2, n3, n4, n6, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<total_num_pusch_sps_stti_ul_repeats_r15_opts> total_num_pusch_sps_stti_ul_repeats_r15_e_;

    // member variables
    bool                                             ext                                                   = false;
    bool                                             p0_persistent_r15_present                             = false;
    bool                                             two_intervs_cfg_r15_present                           = false;
    bool                                             p0_persistent_sf_set2_r15_present                     = false;
    bool                                             nof_conf_ul_sps_processes_stti_r15_present            = false;
    bool                                             tpc_pdcch_cfg_pusch_sps_r15_present                   = false;
    bool                                             cyclic_shift_sps_s_tti_r15_present                    = false;
    bool                                             ifdma_cfg_sps_r15_present                             = false;
    bool                                             harq_proc_id_offset_r15_present                       = false;
    bool                                             rv_sps_stti_ul_repeats_r15_present                    = false;
    bool                                             sps_cfg_idx_r15_present                               = false;
    bool                                             tbs_scaling_factor_subslot_sps_ul_repeats_r15_present = false;
    bool                                             total_num_pusch_sps_stti_ul_repeats_r15_present       = false;
    semi_persist_sched_interv_ul_stti_r15_e_         semi_persist_sched_interv_ul_stti_r15;
    implicit_release_after_e_                        implicit_release_after;
    p0_persistent_r15_s_                             p0_persistent_r15;
    p0_persistent_sf_set2_r15_c_                     p0_persistent_sf_set2_r15;
    uint8_t                                          nof_conf_ul_sps_processes_stti_r15 = 1;
    uint8_t                                          stti_start_time_ul_r15             = 0;
    tpc_pdcch_cfg_c                                  tpc_pdcch_cfg_pusch_sps_r15;
    cyclic_shift_sps_s_tti_r15_e_                    cyclic_shift_sps_s_tti_r15;
    bool                                             ifdma_cfg_sps_r15       = false;
    uint8_t                                          harq_proc_id_offset_r15 = 0;
    rv_sps_stti_ul_repeats_r15_e_                    rv_sps_stti_ul_repeats_r15;
    uint8_t                                          sps_cfg_idx_r15 = 1;
    tbs_scaling_factor_subslot_sps_ul_repeats_r15_e_ tbs_scaling_factor_subslot_sps_ul_repeats_r15;
    total_num_pusch_sps_stti_ul_repeats_r15_e_       total_num_pusch_sps_stti_ul_repeats_r15;
    // ...
  };
  using types = setup_e;

  // choice methods
  sps_cfg_ul_stti_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const sps_cfg_ul_stti_r15_c& other) const;
  bool        operator!=(const sps_cfg_ul_stti_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SPS-ConfigUL-STTI-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SPS-ConfigUL-STTI-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// STAG-ToAddMod-r11 ::= SEQUENCE
struct stag_to_add_mod_r11_s {
  bool               ext         = false;
  uint8_t            stag_id_r11 = 1;
  time_align_timer_e time_align_timer_stag_r11;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CRS-AssistanceInfoList-r11 ::= SEQUENCE (SIZE (1..8)) OF CRS-AssistanceInfo-r11
using crs_assist_info_list_r11_l = dyn_array<crs_assist_info_r11_s>;

// CRS-AssistanceInfoList-r13 ::= SEQUENCE (SIZE (1..8)) OF CRS-AssistanceInfo-r13
using crs_assist_info_list_r13_l = dyn_array<crs_assist_info_r13_s>;

// CRS-AssistanceInfoList-r15 ::= SEQUENCE (SIZE (1..8)) OF CRS-AssistanceInfo-r15
using crs_assist_info_list_r15_l = dyn_array<crs_assist_info_r15_s>;

// DRB-ToAddMod ::= SEQUENCE
struct drb_to_add_mod_s {
  struct drb_type_lwip_r13_opts {
    enum options { lwip, lwip_dl_only, lwip_ul_only, eutran, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<drb_type_lwip_r13_opts> drb_type_lwip_r13_e_;
  struct lwa_wlan_ac_r14_opts {
    enum options { ac_bk, ac_be, ac_vi, ac_vo, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<lwa_wlan_ac_r14_opts> lwa_wlan_ac_r14_e_;

  // member variables
  bool        ext                   = false;
  bool        eps_bearer_id_present = false;
  bool        pdcp_cfg_present      = false;
  bool        rlc_cfg_present       = false;
  bool        lc_ch_id_present      = false;
  bool        lc_ch_cfg_present     = false;
  uint8_t     eps_bearer_id         = 0;
  uint8_t     drb_id                = 1;
  pdcp_cfg_s  pdcp_cfg;
  rlc_cfg_c   rlc_cfg;
  uint8_t     lc_ch_id = 3;
  lc_ch_cfg_s lc_ch_cfg;
  // ...
  // group 0
  bool                      drb_type_change_r12_present = false;
  copy_ptr<rlc_cfg_v1250_s> rlc_cfg_v1250;
  // group 1
  bool                      drb_type_lwa_r13_present  = false;
  bool                      drb_type_lwip_r13_present = false;
  copy_ptr<rlc_cfg_v1310_s> rlc_cfg_v1310;
  bool                      drb_type_lwa_r13 = false;
  drb_type_lwip_r13_e_      drb_type_lwip_r13;
  // group 2
  bool                      lwip_ul_aggregation_r14_present = false;
  bool                      lwip_dl_aggregation_r14_present = false;
  bool                      lwa_wlan_ac_r14_present         = false;
  copy_ptr<rlc_cfg_v1430_c> rlc_cfg_v1430;
  bool                      lwip_ul_aggregation_r14 = false;
  bool                      lwip_dl_aggregation_r14 = false;
  lwa_wlan_ac_r14_e_        lwa_wlan_ac_r14;
  // group 3
  copy_ptr<rlc_cfg_v1510_s> rlc_cfg_v1510;
  // group 4
  bool                           lc_ch_id_r15_present = false;
  copy_ptr<rlc_cfg_v1530_c>      rlc_cfg_v1530;
  copy_ptr<rlc_bearer_cfg_r15_c> rlc_bearer_cfg_secondary_r15;
  uint8_t                        lc_ch_id_r15 = 32;
  // group 5
  bool daps_ho_r16_present = false;
  // group 6
  copy_ptr<rlc_cfg_v1700_s> rlc_cfg_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const drb_to_add_mod_s& other) const;
  bool        operator!=(const drb_to_add_mod_s& other) const { return not(*this == other); }
};

// DRX-Config ::= CHOICE
struct drx_cfg_c {
  struct setup_s_ {
    struct on_dur_timer_opts {
      enum options {
        psf1,
        psf2,
        psf3,
        psf4,
        psf5,
        psf6,
        psf8,
        psf10,
        psf20,
        psf30,
        psf40,
        psf50,
        psf60,
        psf80,
        psf100,
        psf200,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<on_dur_timer_opts> on_dur_timer_e_;
    struct drx_inactivity_timer_opts {
      enum options {
        psf1,
        psf2,
        psf3,
        psf4,
        psf5,
        psf6,
        psf8,
        psf10,
        psf20,
        psf30,
        psf40,
        psf50,
        psf60,
        psf80,
        psf100,
        psf200,
        psf300,
        psf500,
        psf750,
        psf1280,
        psf1920,
        psf2560,
        psf0_v1020,
        spare9,
        spare8,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<drx_inactivity_timer_opts> drx_inactivity_timer_e_;
    struct drx_retx_timer_opts {
      enum options { psf1, psf2, psf4, psf6, psf8, psf16, psf24, psf33, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<drx_retx_timer_opts> drx_retx_timer_e_;
    struct long_drx_cycle_start_offset_c_ {
      struct types_opts {
        enum options {
          sf10,
          sf20,
          sf32,
          sf40,
          sf64,
          sf80,
          sf128,
          sf160,
          sf256,
          sf320,
          sf512,
          sf640,
          sf1024,
          sf1280,
          sf2048,
          sf2560,
          nulltype
        } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      long_drx_cycle_start_offset_c_() = default;
      long_drx_cycle_start_offset_c_(const long_drx_cycle_start_offset_c_& other);
      long_drx_cycle_start_offset_c_& operator=(const long_drx_cycle_start_offset_c_& other);
      ~long_drx_cycle_start_offset_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      uint8_t& sf10()
      {
        assert_choice_type(types::sf10, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      uint8_t& sf20()
      {
        assert_choice_type(types::sf20, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      uint8_t& sf32()
      {
        assert_choice_type(types::sf32, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      uint8_t& sf40()
      {
        assert_choice_type(types::sf40, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      uint8_t& sf64()
      {
        assert_choice_type(types::sf64, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      uint8_t& sf80()
      {
        assert_choice_type(types::sf80, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      uint8_t& sf128()
      {
        assert_choice_type(types::sf128, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      uint8_t& sf160()
      {
        assert_choice_type(types::sf160, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      uint16_t& sf256()
      {
        assert_choice_type(types::sf256, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      uint16_t& sf320()
      {
        assert_choice_type(types::sf320, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      uint16_t& sf512()
      {
        assert_choice_type(types::sf512, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      uint16_t& sf640()
      {
        assert_choice_type(types::sf640, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      uint16_t& sf1024()
      {
        assert_choice_type(types::sf1024, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      uint16_t& sf1280()
      {
        assert_choice_type(types::sf1280, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      uint16_t& sf2048()
      {
        assert_choice_type(types::sf2048, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      uint16_t& sf2560()
      {
        assert_choice_type(types::sf2560, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      const uint8_t& sf10() const
      {
        assert_choice_type(types::sf10, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& sf20() const
      {
        assert_choice_type(types::sf20, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& sf32() const
      {
        assert_choice_type(types::sf32, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& sf40() const
      {
        assert_choice_type(types::sf40, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& sf64() const
      {
        assert_choice_type(types::sf64, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& sf80() const
      {
        assert_choice_type(types::sf80, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& sf128() const
      {
        assert_choice_type(types::sf128, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& sf160() const
      {
        assert_choice_type(types::sf160, type_, "longDRX-CycleStartOffset");
        return c.get<uint8_t>();
      }
      const uint16_t& sf256() const
      {
        assert_choice_type(types::sf256, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      const uint16_t& sf320() const
      {
        assert_choice_type(types::sf320, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      const uint16_t& sf512() const
      {
        assert_choice_type(types::sf512, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      const uint16_t& sf640() const
      {
        assert_choice_type(types::sf640, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      const uint16_t& sf1024() const
      {
        assert_choice_type(types::sf1024, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      const uint16_t& sf1280() const
      {
        assert_choice_type(types::sf1280, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      const uint16_t& sf2048() const
      {
        assert_choice_type(types::sf2048, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      const uint16_t& sf2560() const
      {
        assert_choice_type(types::sf2560, type_, "longDRX-CycleStartOffset");
        return c.get<uint16_t>();
      }
      uint8_t&  set_sf10();
      uint8_t&  set_sf20();
      uint8_t&  set_sf32();
      uint8_t&  set_sf40();
      uint8_t&  set_sf64();
      uint8_t&  set_sf80();
      uint8_t&  set_sf128();
      uint8_t&  set_sf160();
      uint16_t& set_sf256();
      uint16_t& set_sf320();
      uint16_t& set_sf512();
      uint16_t& set_sf640();
      uint16_t& set_sf1024();
      uint16_t& set_sf1280();
      uint16_t& set_sf2048();
      uint16_t& set_sf2560();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };
    struct short_drx_s_ {
      struct short_drx_cycle_opts {
        enum options {
          sf2,
          sf5,
          sf8,
          sf10,
          sf16,
          sf20,
          sf32,
          sf40,
          sf64,
          sf80,
          sf128,
          sf160,
          sf256,
          sf320,
          sf512,
          sf640,
          nulltype
        } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<short_drx_cycle_opts> short_drx_cycle_e_;

      // member variables
      short_drx_cycle_e_ short_drx_cycle;
      uint8_t            drx_short_cycle_timer = 1;
    };

    // member variables
    bool                           short_drx_present = false;
    on_dur_timer_e_                on_dur_timer;
    drx_inactivity_timer_e_        drx_inactivity_timer;
    drx_retx_timer_e_              drx_retx_timer;
    long_drx_cycle_start_offset_c_ long_drx_cycle_start_offset;
    short_drx_s_                   short_drx;
  };
  using types = setup_e;

  // choice methods
  drx_cfg_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "DRX-Config");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "DRX-Config");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// DRX-Config-r13 ::= SEQUENCE
struct drx_cfg_r13_s {
  struct on_dur_timer_v1310_opts {
    enum options { psf300, psf400, psf500, psf600, psf800, psf1000, psf1200, psf1600, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<on_dur_timer_v1310_opts> on_dur_timer_v1310_e_;
  struct drx_retx_timer_v1310_opts {
    enum options { psf40, psf64, psf80, psf96, psf112, psf128, psf160, psf320, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<drx_retx_timer_v1310_opts> drx_retx_timer_v1310_e_;
  struct drx_ul_retx_timer_r13_opts {
    enum options {
      psf0,
      psf1,
      psf2,
      psf4,
      psf6,
      psf8,
      psf16,
      psf24,
      psf33,
      psf40,
      psf64,
      psf80,
      psf96,
      psf112,
      psf128,
      psf160,
      psf320,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<drx_ul_retx_timer_r13_opts> drx_ul_retx_timer_r13_e_;

  // member variables
  bool                     on_dur_timer_v1310_present    = false;
  bool                     drx_retx_timer_v1310_present  = false;
  bool                     drx_ul_retx_timer_r13_present = false;
  on_dur_timer_v1310_e_    on_dur_timer_v1310;
  drx_retx_timer_v1310_e_  drx_retx_timer_v1310;
  drx_ul_retx_timer_r13_e_ drx_ul_retx_timer_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-Config-r15 ::= SEQUENCE
struct drx_cfg_r15_s {
  struct drx_retx_timer_short_tti_r15_opts {
    enum options { tti10, tti20, tti40, tti64, tti80, tti96, tti112, tti128, tti160, tti320, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<drx_retx_timer_short_tti_r15_opts> drx_retx_timer_short_tti_r15_e_;
  struct drx_ul_retx_timer_short_tti_r15_opts {
    enum options {
      tti0,
      tti1,
      tti2,
      tti4,
      tti6,
      tti8,
      tti16,
      tti24,
      tti33,
      tti40,
      tti64,
      tti80,
      tti96,
      tti112,
      tti128,
      tti160,
      tti320,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<drx_ul_retx_timer_short_tti_r15_opts> drx_ul_retx_timer_short_tti_r15_e_;

  // member variables
  bool                               drx_retx_timer_short_tti_r15_present    = false;
  bool                               drx_ul_retx_timer_short_tti_r15_present = false;
  drx_retx_timer_short_tti_r15_e_    drx_retx_timer_short_tti_r15;
  drx_ul_retx_timer_short_tti_r15_e_ drx_ul_retx_timer_short_tti_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-Config-v1130 ::= SEQUENCE
struct drx_cfg_v1130_s {
  struct long_drx_cycle_start_offset_v1130_c_ {
    struct types_opts {
      enum options { sf60_v1130, sf70_v1130, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    long_drx_cycle_start_offset_v1130_c_() = default;
    long_drx_cycle_start_offset_v1130_c_(const long_drx_cycle_start_offset_v1130_c_& other);
    long_drx_cycle_start_offset_v1130_c_& operator=(const long_drx_cycle_start_offset_v1130_c_& other);
    ~long_drx_cycle_start_offset_v1130_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sf60_v1130()
    {
      assert_choice_type(types::sf60_v1130, type_, "longDRX-CycleStartOffset-v1130");
      return c.get<uint8_t>();
    }
    uint8_t& sf70_v1130()
    {
      assert_choice_type(types::sf70_v1130, type_, "longDRX-CycleStartOffset-v1130");
      return c.get<uint8_t>();
    }
    const uint8_t& sf60_v1130() const
    {
      assert_choice_type(types::sf60_v1130, type_, "longDRX-CycleStartOffset-v1130");
      return c.get<uint8_t>();
    }
    const uint8_t& sf70_v1130() const
    {
      assert_choice_type(types::sf70_v1130, type_, "longDRX-CycleStartOffset-v1130");
      return c.get<uint8_t>();
    }
    uint8_t& set_sf60_v1130();
    uint8_t& set_sf70_v1130();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                                 drx_retx_timer_v1130_present              = false;
  bool                                 long_drx_cycle_start_offset_v1130_present = false;
  bool                                 short_drx_cycle_v1130_present             = false;
  long_drx_cycle_start_offset_v1130_c_ long_drx_cycle_start_offset_v1130;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-Config-v1310 ::= SEQUENCE
struct drx_cfg_v1310_s {
  struct long_drx_cycle_start_offset_v1310_s_ {
    uint8_t sf60_v1310 = 0;
  };

  // member variables
  bool                                 long_drx_cycle_start_offset_v1310_present = false;
  long_drx_cycle_start_offset_v1310_s_ long_drx_cycle_start_offset_v1310;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataInactivityTimer-r14 ::= ENUMERATED
struct data_inactivity_timer_r14_opts {
  enum options { s1, s2, s3, s5, s7, s10, s15, s20, s40, s50, s60, s80, s100, s120, s150, s180, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<data_inactivity_timer_r14_opts> data_inactivity_timer_r14_e;

// NeighCellsToAddModList-r12 ::= SEQUENCE (SIZE (1..8)) OF NeighCellsInfo-r12
using neigh_cells_to_add_mod_list_r12_l = dyn_array<neigh_cells_info_r12_s>;

// NeighCellsToReleaseList-r12 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..503)
using neigh_cells_to_release_list_r12_l = bounded_array<uint16_t, 8>;

// OffsetThresholdTA-r17 ::= ENUMERATED
struct offset_thres_ta_r17_opts {
  enum options {
    ms0dot5,
    ms1,
    ms2,
    ms3,
    ms4,
    ms5,
    ms6,
    ms7,
    ms8,
    ms9,
    ms10,
    ms11,
    ms12,
    ms13,
    ms14,
    ms15,
    nulltype
  } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
typedef enumerated<offset_thres_ta_r17_opts> offset_thres_ta_r17_e;

// PeriodicBSR-Timer-r12 ::= ENUMERATED
struct periodic_bsr_timer_r12_opts {
  enum options {
    sf5,
    sf10,
    sf16,
    sf20,
    sf32,
    sf40,
    sf64,
    sf80,
    sf128,
    sf160,
    sf320,
    sf640,
    sf1280,
    sf2560,
    infinity,
    spare1,
    nulltype
  } value;
  typedef int16_t number_type;

  const char* to_string() const;
  int16_t     to_number() const;
};
typedef enumerated<periodic_bsr_timer_r12_opts> periodic_bsr_timer_r12_e;

// RetxBSR-Timer-r12 ::= ENUMERATED
struct retx_bsr_timer_r12_opts {
  enum options { sf320, sf640, sf1280, sf2560, sf5120, sf10240, spare2, spare1, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<retx_bsr_timer_r12_opts> retx_bsr_timer_r12_e;

// SPS-ConfigDL ::= CHOICE
struct sps_cfg_dl_c {
  struct setup_s_ {
    struct semi_persist_sched_interv_dl_opts {
      enum options {
        sf10,
        sf20,
        sf32,
        sf40,
        sf64,
        sf80,
        sf128,
        sf160,
        sf320,
        sf640,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<semi_persist_sched_interv_dl_opts> semi_persist_sched_interv_dl_e_;
    struct two_ant_port_activ_r10_c_ {
      struct setup_s_ {
        n1_pucch_an_persistent_list_l n1_pucch_an_persistent_list_p1_r10;
      };
      using types = setup_e;

      // choice methods
      two_ant_port_activ_r10_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const two_ant_port_activ_r10_c_& other) const;
      bool        operator!=(const two_ant_port_activ_r10_c_& other) const { return not(*this == other); }
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "twoAntennaPortActivated-r10");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "twoAntennaPortActivated-r10");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };

    // member variables
    bool                            ext = false;
    semi_persist_sched_interv_dl_e_ semi_persist_sched_interv_dl;
    uint8_t                         nof_conf_sps_processes = 1;
    n1_pucch_an_persistent_list_l   n1_pucch_an_persistent_list;
    // ...
    // group 0
    copy_ptr<two_ant_port_activ_r10_c_> two_ant_port_activ_r10;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const setup_s_& other) const;
    bool        operator!=(const setup_s_& other) const { return not(*this == other); }
  };
  using types = setup_e;

  // choice methods
  sps_cfg_dl_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const sps_cfg_dl_c& other) const;
  bool        operator!=(const sps_cfg_dl_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SPS-ConfigDL");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SPS-ConfigDL");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SPS-ConfigDL-STTI-r15 ::= CHOICE
struct sps_cfg_dl_stti_r15_c {
  struct setup_s_ {
    struct semi_persist_sched_interv_dl_stti_r15_opts {
      enum options {
        stti1,
        stti2,
        stti3,
        stti4,
        stti6,
        stti8,
        stti12,
        stti16,
        stti20,
        stti40,
        stti60,
        stti80,
        stti120,
        stti240,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<semi_persist_sched_interv_dl_stti_r15_opts> semi_persist_sched_interv_dl_stti_r15_e_;
    struct two_ant_port_activ_r15_c_ {
      struct setup_s_ {
        n1_spucch_an_persistent_list_r15_l n1_spucch_an_persistent_list_p1_r15;
      };
      using types = setup_e;

      // choice methods
      two_ant_port_activ_r15_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "twoAntennaPortActivated-r15");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "twoAntennaPortActivated-r15");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };

    // member variables
    bool                                     ext                                 = false;
    bool                                     two_ant_port_activ_r15_present      = false;
    bool                                     tpc_pdcch_cfg_pucch_sps_r15_present = false;
    semi_persist_sched_interv_dl_stti_r15_e_ semi_persist_sched_interv_dl_stti_r15;
    uint8_t                                  nof_conf_sps_processes_stti_r15 = 1;
    two_ant_port_activ_r15_c_                two_ant_port_activ_r15;
    uint8_t                                  stti_start_time_dl_r15 = 0;
    tpc_pdcch_cfg_c                          tpc_pdcch_cfg_pucch_sps_r15;
    // ...
  };
  using types = setup_e;

  // choice methods
  sps_cfg_dl_stti_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SPS-ConfigDL-STTI-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SPS-ConfigDL-STTI-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SPS-ConfigSL-ToAddModList-r14 ::= SEQUENCE (SIZE (1..8)) OF SPS-ConfigSL-r14
using sps_cfg_sl_to_add_mod_list_r14_l = dyn_array<sps_cfg_sl_r14_s>;

// SPS-ConfigSL-ToReleaseList-r14 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using sps_cfg_sl_to_release_list_r14_l = bounded_array<uint8_t, 8>;

// SPS-ConfigUL-STTI-ToAddModList-r15 ::= SEQUENCE (SIZE (1..6)) OF SPS-ConfigUL-STTI-r15
using sps_cfg_ul_stti_to_add_mod_list_r15_l = dyn_array<sps_cfg_ul_stti_r15_c>;

// SPS-ConfigUL-STTI-ToReleaseList-r15 ::= SEQUENCE (SIZE (1..6)) OF INTEGER (1..6)
using sps_cfg_ul_stti_to_release_list_r15_l = bounded_array<uint8_t, 6>;

// SPS-ConfigUL-ToAddModList-r14 ::= SEQUENCE (SIZE (1..8)) OF SPS-ConfigUL
using sps_cfg_ul_to_add_mod_list_r14_l = dyn_array<sps_cfg_ul_c>;

// SPS-ConfigUL-ToAddModList-r15 ::= SEQUENCE (SIZE (1..6)) OF SPS-ConfigUL
using sps_cfg_ul_to_add_mod_list_r15_l = dyn_array<sps_cfg_ul_c>;

// SPS-ConfigUL-ToReleaseList-r14 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using sps_cfg_ul_to_release_list_r14_l = bounded_array<uint8_t, 8>;

// SPS-ConfigUL-ToReleaseList-r15 ::= SEQUENCE (SIZE (1..6)) OF INTEGER (1..6)
using sps_cfg_ul_to_release_list_r15_l = bounded_array<uint8_t, 6>;

// SR-ProhibitTimerOffset-r17 ::= ENUMERATED
struct sr_prohibit_timer_offset_r17_opts {
  enum options { ms90, ms180, ms270, ms360, ms450, ms540, ms1080, spare, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<sr_prohibit_timer_offset_r17_opts> sr_prohibit_timer_offset_r17_e;

// SRB-ToAddMod ::= SEQUENCE
struct srb_to_add_mod_s {
  struct rlc_cfg_c_ {
    struct types_opts {
      enum options { explicit_value, default_value, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    rlc_cfg_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const rlc_cfg_c_& other) const;
    bool        operator!=(const rlc_cfg_c_& other) const { return not(*this == other); }
    // getters
    rlc_cfg_c& explicit_value()
    {
      assert_choice_type(types::explicit_value, type_, "rlc-Config");
      return c;
    }
    const rlc_cfg_c& explicit_value() const
    {
      assert_choice_type(types::explicit_value, type_, "rlc-Config");
      return c;
    }
    rlc_cfg_c& set_explicit_value();
    void       set_default_value();

  private:
    types     type_;
    rlc_cfg_c c;
  };
  struct lc_ch_cfg_c_ {
    struct types_opts {
      enum options { explicit_value, default_value, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    lc_ch_cfg_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const lc_ch_cfg_c_& other) const;
    bool        operator!=(const lc_ch_cfg_c_& other) const { return not(*this == other); }
    // getters
    lc_ch_cfg_s& explicit_value()
    {
      assert_choice_type(types::explicit_value, type_, "logicalChannelConfig");
      return c;
    }
    const lc_ch_cfg_s& explicit_value() const
    {
      assert_choice_type(types::explicit_value, type_, "logicalChannelConfig");
      return c;
    }
    lc_ch_cfg_s& set_explicit_value();
    void         set_default_value();

  private:
    types       type_;
    lc_ch_cfg_s c;
  };

  // member variables
  bool         ext               = false;
  bool         rlc_cfg_present   = false;
  bool         lc_ch_cfg_present = false;
  uint8_t      srb_id            = 1;
  rlc_cfg_c_   rlc_cfg;
  lc_ch_cfg_c_ lc_ch_cfg;
  // ...
  // group 0
  bool                           pdcp_ver_change_r15_present = false;
  bool                           srb_id_v1530_present        = false;
  copy_ptr<rlc_cfg_v1530_c>      rlc_cfg_v1530;
  copy_ptr<rlc_bearer_cfg_r15_c> rlc_bearer_cfg_secondary_r15;
  uint8_t                        srb_id_v1530 = 4;
  // group 1
  copy_ptr<rlc_cfg_v1510_s> rlc_cfg_v1560;
  // group 2
  copy_ptr<rlc_cfg_v1700_s> rlc_cfg_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srb_to_add_mod_s& other) const;
  bool        operator!=(const srb_to_add_mod_s& other) const { return not(*this == other); }
};

// STAG-ToAddModList-r11 ::= SEQUENCE (SIZE (1..3)) OF STAG-ToAddMod-r11
using stag_to_add_mod_list_r11_l = dyn_array<stag_to_add_mod_r11_s>;

// STAG-ToReleaseList-r11 ::= SEQUENCE (SIZE (1..3)) OF INTEGER (1..3)
using stag_to_release_list_r11_l = bounded_array<uint8_t, 3>;

// CRS-ChEstMPDCCH-ConfigDedicated-r16 ::= SEQUENCE
struct crs_ch_est_mpdcch_cfg_ded_r16_s {
  struct pwr_ratio_r16_opts {
    enum options { db_minus4dot77, db_minus3, db_minus1dot77, db0, db1, db2, db3, db4dot77, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<pwr_ratio_r16_opts> pwr_ratio_r16_e_;
  struct localized_map_type_r16_opts {
    enum options { predefined, csi_based, reciprocity_based, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<localized_map_type_r16_opts> localized_map_type_r16_e_;

  // member variables
  bool                      pwr_ratio_r16_present          = false;
  bool                      localized_map_type_r16_present = false;
  pwr_ratio_r16_e_          pwr_ratio_r16;
  localized_map_type_r16_e_ localized_map_type_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-ToAddModList ::= SEQUENCE (SIZE (1..11)) OF DRB-ToAddMod
using drb_to_add_mod_list_l = dyn_array<drb_to_add_mod_s>;

// DRB-ToAddModList-r15 ::= SEQUENCE (SIZE (1..15)) OF DRB-ToAddMod
using drb_to_add_mod_list_r15_l = dyn_array<drb_to_add_mod_s>;

// DRB-ToReleaseList ::= SEQUENCE (SIZE (1..11)) OF INTEGER (1..32)
using drb_to_release_list_l = bounded_array<uint8_t, 11>;

// DRB-ToReleaseList-r15 ::= SEQUENCE (SIZE (1..15)) OF INTEGER (1..32)
using drb_to_release_list_r15_l = bounded_array<uint8_t, 15>;

// MAC-MainConfig ::= SEQUENCE
struct mac_main_cfg_s {
  struct ul_sch_cfg_s_ {
    struct max_harq_tx_opts {
      enum options { n1, n2, n3, n4, n5, n6, n7, n8, n10, n12, n16, n20, n24, n28, spare2, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<max_harq_tx_opts> max_harq_tx_e_;

    // member variables
    bool                     max_harq_tx_present        = false;
    bool                     periodic_bsr_timer_present = false;
    max_harq_tx_e_           max_harq_tx;
    periodic_bsr_timer_r12_e periodic_bsr_timer;
    retx_bsr_timer_r12_e     retx_bsr_timer;
    bool                     tti_bundling = false;
  };
  struct phr_cfg_c_ {
    struct setup_s_ {
      struct periodic_phr_timer_opts {
        enum options { sf10, sf20, sf50, sf100, sf200, sf500, sf1000, infinity, nulltype } value;
        typedef int16_t number_type;

        const char* to_string() const;
        int16_t     to_number() const;
      };
      typedef enumerated<periodic_phr_timer_opts> periodic_phr_timer_e_;
      struct prohibit_phr_timer_opts {
        enum options { sf0, sf10, sf20, sf50, sf100, sf200, sf500, sf1000, nulltype } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<prohibit_phr_timer_opts> prohibit_phr_timer_e_;
      struct dl_pathloss_change_opts {
        enum options { db1, db3, db6, infinity, nulltype } value;
        typedef int8_t number_type;

        const char* to_string() const;
        int8_t      to_number() const;
      };
      typedef enumerated<dl_pathloss_change_opts> dl_pathloss_change_e_;

      // member variables
      periodic_phr_timer_e_ periodic_phr_timer;
      prohibit_phr_timer_e_ prohibit_phr_timer;
      dl_pathloss_change_e_ dl_pathloss_change;
    };
    using types = setup_e;

    // choice methods
    phr_cfg_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "phr-Config");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "phr-Config");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct mac_main_cfg_v1020_s_ {
    struct scell_deactivation_timer_r10_opts {
      enum options { rf2, rf4, rf8, rf16, rf32, rf64, rf128, spare, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scell_deactivation_timer_r10_opts> scell_deactivation_timer_r10_e_;

    // member variables
    bool                            scell_deactivation_timer_r10_present = false;
    bool                            extended_bsr_sizes_r10_present       = false;
    bool                            extended_phr_r10_present             = false;
    scell_deactivation_timer_r10_e_ scell_deactivation_timer_r10;
  };
  struct dual_connect_phr_c_ {
    struct setup_s_ {
      struct phr_mode_other_cg_r12_opts {
        enum options { real, virtual_value, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<phr_mode_other_cg_r12_opts> phr_mode_other_cg_r12_e_;

      // member variables
      phr_mode_other_cg_r12_e_ phr_mode_other_cg_r12;
    };
    using types = setup_e;

    // choice methods
    dual_connect_phr_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "dualConnectivityPHR");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "dualConnectivityPHR");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct lc_ch_sr_cfg_r12_c_ {
    struct setup_s_ {
      struct lc_ch_sr_prohibit_timer_r12_opts {
        enum options { sf20, sf40, sf64, sf128, sf512, sf1024, sf2560, spare1, nulltype } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<lc_ch_sr_prohibit_timer_r12_opts> lc_ch_sr_prohibit_timer_r12_e_;

      // member variables
      lc_ch_sr_prohibit_timer_r12_e_ lc_ch_sr_prohibit_timer_r12;
    };
    using types = setup_e;

    // choice methods
    lc_ch_sr_cfg_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "logicalChannelSR-Config-r12");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "logicalChannelSR-Config-r12");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct edrx_cfg_cycle_start_offset_r13_c_ {
    struct setup_c_ {
      struct types_opts {
        enum options { sf5120, sf10240, nulltype } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
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
      uint8_t& sf5120()
      {
        assert_choice_type(types::sf5120, type_, "setup");
        return c.get<uint8_t>();
      }
      uint8_t& sf10240()
      {
        assert_choice_type(types::sf10240, type_, "setup");
        return c.get<uint8_t>();
      }
      const uint8_t& sf5120() const
      {
        assert_choice_type(types::sf5120, type_, "setup");
        return c.get<uint8_t>();
      }
      const uint8_t& sf10240() const
      {
        assert_choice_type(types::sf10240, type_, "setup");
        return c.get<uint8_t>();
      }
      uint8_t& set_sf5120();
      uint8_t& set_sf10240();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };
    using types = setup_e;

    // choice methods
    edrx_cfg_cycle_start_offset_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_c_& setup()
    {
      assert_choice_type(types::setup, type_, "eDRX-Config-CycleStartOffset-r13");
      return c;
    }
    const setup_c_& setup() const
    {
      assert_choice_type(types::setup, type_, "eDRX-Config-CycleStartOffset-r13");
      return c;
    }
    void      set_release();
    setup_c_& set_setup();

  private:
    types    type_;
    setup_c_ c;
  };
  struct drx_cfg_r13_c_ {
    using types = setup_e;

    // choice methods
    drx_cfg_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drx_cfg_r13_s& setup()
    {
      assert_choice_type(types::setup, type_, "drx-Config-r13");
      return c;
    }
    const drx_cfg_r13_s& setup() const
    {
      assert_choice_type(types::setup, type_, "drx-Config-r13");
      return c;
    }
    void           set_release();
    drx_cfg_r13_s& set_setup();

  private:
    types         type_;
    drx_cfg_r13_s c;
  };
  struct skip_ul_tx_r14_c_ {
    struct setup_s_ {
      bool skip_ul_tx_sps_r14_present     = false;
      bool skip_ul_tx_dynamic_r14_present = false;
    };
    using types = setup_e;

    // choice methods
    skip_ul_tx_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "skipUplinkTx-r14");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "skipUplinkTx-r14");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct data_inactivity_timer_cfg_r14_c_ {
    struct setup_s_ {
      data_inactivity_timer_r14_e data_inactivity_timer_r14;
    };
    using types = setup_e;

    // choice methods
    data_inactivity_timer_cfg_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "dataInactivityTimerConfig-r14");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "dataInactivityTimerConfig-r14");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct short_tti_and_spt_r15_c_ {
    struct setup_s_ {
      struct periodic_bsr_timer_r15_opts {
        enum options {
          sf1,
          sf5,
          sf10,
          sf16,
          sf20,
          sf32,
          sf40,
          sf64,
          sf80,
          sf128,
          sf160,
          sf320,
          sf640,
          sf1280,
          sf2560,
          infinity,
          nulltype
        } value;
        typedef int16_t number_type;

        const char* to_string() const;
        int16_t     to_number() const;
      };
      typedef enumerated<periodic_bsr_timer_r15_opts> periodic_bsr_timer_r15_e_;
      struct proc_timeline_r15_opts {
        enum options { nplus4set1, nplus6set1, nplus6set2, nplus8set2, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<proc_timeline_r15_opts> proc_timeline_r15_e_;

      // member variables
      bool                      drx_cfg_r15_present            = false;
      bool                      periodic_bsr_timer_r15_present = false;
      bool                      proc_timeline_r15_present      = false;
      bool                      ssr_prohibit_timer_r15_present = false;
      drx_cfg_r15_s             drx_cfg_r15;
      periodic_bsr_timer_r15_e_ periodic_bsr_timer_r15;
      proc_timeline_r15_e_      proc_timeline_r15;
      uint8_t                   ssr_prohibit_timer_r15 = 0;
    };
    using types = setup_e;

    // choice methods
    short_tti_and_spt_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "shortTTI-AndSPT-r15");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "shortTTI-AndSPT-r15");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct dormant_state_timers_r15_c_ {
    struct setup_s_ {
      struct scell_hibernation_timer_r15_opts {
        enum options { rf2, rf4, rf8, rf16, rf32, rf64, rf128, spare, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<scell_hibernation_timer_r15_opts> scell_hibernation_timer_r15_e_;
      struct dormant_scell_deactivation_timer_r15_opts {
        enum options {
          rf2,
          rf4,
          rf8,
          rf16,
          rf32,
          rf64,
          rf128,
          rf320,
          rf640,
          rf1280,
          rf2560,
          rf5120,
          rf10240,
          spare3,
          spare2,
          spare1,
          nulltype
        } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<dormant_scell_deactivation_timer_r15_opts> dormant_scell_deactivation_timer_r15_e_;

      // member variables
      bool                                    scell_hibernation_timer_r15_present          = false;
      bool                                    dormant_scell_deactivation_timer_r15_present = false;
      scell_hibernation_timer_r15_e_          scell_hibernation_timer_r15;
      dormant_scell_deactivation_timer_r15_e_ dormant_scell_deactivation_timer_r15;
    };
    using types = setup_e;

    // choice methods
    dormant_state_timers_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "dormantStateTimers-r15");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "dormantStateTimers-r15");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool               ext                = false;
  bool               ul_sch_cfg_present = false;
  bool               drx_cfg_present    = false;
  bool               phr_cfg_present    = false;
  ul_sch_cfg_s_      ul_sch_cfg;
  drx_cfg_c          drx_cfg;
  time_align_timer_e time_align_timer_ded;
  phr_cfg_c_         phr_cfg;
  // ...
  // group 0
  bool    sr_prohibit_timer_r9_present = false;
  uint8_t sr_prohibit_timer_r9         = 0;
  // group 1
  copy_ptr<mac_main_cfg_v1020_s_> mac_main_cfg_v1020;
  // group 2
  copy_ptr<stag_to_release_list_r11_l> stag_to_release_list_r11;
  copy_ptr<stag_to_add_mod_list_r11_l> stag_to_add_mod_list_r11;
  copy_ptr<drx_cfg_v1130_s>            drx_cfg_v1130;
  // group 3
  bool                          e_harq_pattern_r12_present = false;
  bool                          e_harq_pattern_r12         = false;
  copy_ptr<dual_connect_phr_c_> dual_connect_phr;
  copy_ptr<lc_ch_sr_cfg_r12_c_> lc_ch_sr_cfg_r12;
  // group 4
  bool                                         extended_phr2_r13_present = false;
  copy_ptr<drx_cfg_v1310_s>                    drx_cfg_v1310;
  bool                                         extended_phr2_r13 = false;
  copy_ptr<edrx_cfg_cycle_start_offset_r13_c_> edrx_cfg_cycle_start_offset_r13;
  // group 5
  copy_ptr<drx_cfg_r13_c_> drx_cfg_r13;
  // group 6
  copy_ptr<skip_ul_tx_r14_c_>                skip_ul_tx_r14;
  copy_ptr<data_inactivity_timer_cfg_r14_c_> data_inactivity_timer_cfg_r14;
  // group 7
  bool rai_activation_r14_present = false;
  // group 8
  bool                                  mpdcch_ul_harq_ack_feedback_cfg_r15_present = false;
  copy_ptr<short_tti_and_spt_r15_c_>    short_tti_and_spt_r15;
  bool                                  mpdcch_ul_harq_ack_feedback_cfg_r15 = false;
  copy_ptr<dormant_state_timers_r15_c_> dormant_state_timers_r15;
  // group 9
  bool ce_etws_cmas_rx_in_conn_r16_present = false;
  // group 10
  copy_ptr<setup_release_c<offset_thres_ta_r17_e> >          offset_thres_ta_r17;
  copy_ptr<setup_release_c<sr_prohibit_timer_offset_r17_e> > sr_prohibit_timer_offset_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasSubframePatternPCell-r10 ::= CHOICE
struct meas_sf_pattern_pcell_r10_c {
  using types = setup_e;

  // choice methods
  meas_sf_pattern_pcell_r10_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  meas_sf_pattern_r10_c& setup()
  {
    assert_choice_type(types::setup, type_, "MeasSubframePatternPCell-r10");
    return c;
  }
  const meas_sf_pattern_r10_c& setup() const
  {
    assert_choice_type(types::setup, type_, "MeasSubframePatternPCell-r10");
    return c;
  }
  void                   set_release();
  meas_sf_pattern_r10_c& set_setup();

private:
  types                 type_;
  meas_sf_pattern_r10_c c;
};

// NAICS-AssistanceInfo-r12 ::= CHOICE
struct naics_assist_info_r12_c {
  struct setup_s_ {
    bool                              neigh_cells_to_release_list_r12_present = false;
    bool                              neigh_cells_to_add_mod_list_r12_present = false;
    bool                              serv_cellp_a_r12_present                = false;
    neigh_cells_to_release_list_r12_l neigh_cells_to_release_list_r12;
    neigh_cells_to_add_mod_list_r12_l neigh_cells_to_add_mod_list_r12;
    p_a_e                             serv_cellp_a_r12;
  };
  using types = setup_e;

  // choice methods
  naics_assist_info_r12_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const naics_assist_info_r12_c& other) const;
  bool        operator!=(const naics_assist_info_r12_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "NAICS-AssistanceInfo-r12");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "NAICS-AssistanceInfo-r12");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// NeighCellsCRS-Info-r11 ::= CHOICE
struct neigh_cells_crs_info_r11_c {
  using types = setup_e;

  // choice methods
  neigh_cells_crs_info_r11_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  crs_assist_info_list_r11_l& setup()
  {
    assert_choice_type(types::setup, type_, "NeighCellsCRS-Info-r11");
    return c;
  }
  const crs_assist_info_list_r11_l& setup() const
  {
    assert_choice_type(types::setup, type_, "NeighCellsCRS-Info-r11");
    return c;
  }
  void                        set_release();
  crs_assist_info_list_r11_l& set_setup();

private:
  types                      type_;
  crs_assist_info_list_r11_l c;
};

// NeighCellsCRS-Info-r13 ::= CHOICE
struct neigh_cells_crs_info_r13_c {
  using types = setup_e;

  // choice methods
  neigh_cells_crs_info_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const neigh_cells_crs_info_r13_c& other) const;
  bool        operator!=(const neigh_cells_crs_info_r13_c& other) const { return not(*this == other); }
  // getters
  crs_assist_info_list_r13_l& setup()
  {
    assert_choice_type(types::setup, type_, "NeighCellsCRS-Info-r13");
    return c;
  }
  const crs_assist_info_list_r13_l& setup() const
  {
    assert_choice_type(types::setup, type_, "NeighCellsCRS-Info-r13");
    return c;
  }
  void                        set_release();
  crs_assist_info_list_r13_l& set_setup();

private:
  types                      type_;
  crs_assist_info_list_r13_l c;
};

// NeighCellsCRS-Info-r15 ::= CHOICE
struct neigh_cells_crs_info_r15_c {
  using types = setup_e;

  // choice methods
  neigh_cells_crs_info_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const neigh_cells_crs_info_r15_c& other) const;
  bool        operator!=(const neigh_cells_crs_info_r15_c& other) const { return not(*this == other); }
  // getters
  crs_assist_info_list_r15_l& setup()
  {
    assert_choice_type(types::setup, type_, "NeighCellsCRS-Info-r15");
    return c;
  }
  const crs_assist_info_list_r15_l& setup() const
  {
    assert_choice_type(types::setup, type_, "NeighCellsCRS-Info-r15");
    return c;
  }
  void                        set_release();
  crs_assist_info_list_r15_l& set_setup();

private:
  types                      type_;
  crs_assist_info_list_r15_l c;
};

// RLF-TimersAndConstants-r13 ::= CHOICE
struct rlf_timers_and_consts_r13_c {
  struct setup_s_ {
    struct t301_v1310_opts {
      enum options { ms2500, ms3000, ms3500, ms4000, ms5000, ms6000, ms8000, ms10000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t301_v1310_opts> t301_v1310_e_;
    struct t310_v1330_opts {
      enum options { ms4000, ms6000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t310_v1330_opts> t310_v1330_e_;

    // member variables
    bool          ext = false;
    t301_v1310_e_ t301_v1310;
    // ...
    // group 0
    bool          t310_v1330_present = false;
    t310_v1330_e_ t310_v1330;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
  using types = setup_e;

  // choice methods
  rlf_timers_and_consts_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "RLF-TimersAndConstants-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "RLF-TimersAndConstants-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// RLF-TimersAndConstants-r9 ::= CHOICE
struct rlf_timers_and_consts_r9_c {
  struct setup_s_ {
    struct t301_r9_opts {
      enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t301_r9_opts> t301_r9_e_;
    struct t310_r9_opts {
      enum options { ms0, ms50, ms100, ms200, ms500, ms1000, ms2000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t310_r9_opts> t310_r9_e_;
    struct n310_r9_opts {
      enum options { n1, n2, n3, n4, n6, n8, n10, n20, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<n310_r9_opts> n310_r9_e_;
    struct t311_r9_opts {
      enum options { ms1000, ms3000, ms5000, ms10000, ms15000, ms20000, ms30000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t311_r9_opts> t311_r9_e_;
    struct n311_r9_opts {
      enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<n311_r9_opts> n311_r9_e_;

    // member variables
    bool       ext = false;
    t301_r9_e_ t301_r9;
    t310_r9_e_ t310_r9;
    n310_r9_e_ n310_r9;
    t311_r9_e_ t311_r9;
    n311_r9_e_ n311_r9;
    // ...
  };
  using types = setup_e;

  // choice methods
  rlf_timers_and_consts_r9_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "RLF-TimersAndConstants-r9");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "RLF-TimersAndConstants-r9");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// RLF-TimersAndConstantsMCG-Failure-r16 ::= CHOICE
struct rlf_timers_and_consts_mcg_fail_r16_c {
  struct setup_s_ {
    struct t316_r16_opts {
      enum options { ms50, ms100, ms200, ms300, ms400, ms500, ms600, ms1000, ms1500, ms2000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t316_r16_opts> t316_r16_e_;

    // member variables
    bool        ext = false;
    t316_r16_e_ t316_r16;
    // ...
  };
  using types = setup_e;

  // choice methods
  rlf_timers_and_consts_mcg_fail_r16_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "RLF-TimersAndConstantsMCG-Failure-r16");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "RLF-TimersAndConstantsMCG-Failure-r16");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SPS-Config ::= SEQUENCE
struct sps_cfg_s {
  bool                semi_persist_sched_c_rnti_present = false;
  bool                sps_cfg_dl_present                = false;
  bool                sps_cfg_ul_present                = false;
  fixed_bitstring<16> semi_persist_sched_c_rnti;
  sps_cfg_dl_c        sps_cfg_dl;
  sps_cfg_ul_c        sps_cfg_ul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SPS-Config-v1430 ::= SEQUENCE
struct sps_cfg_v1430_s {
  bool                             ul_sps_v_rnti_r14_present              = false;
  bool                             sl_sps_v_rnti_r14_present              = false;
  bool                             sps_cfg_ul_to_add_mod_list_r14_present = false;
  bool                             sps_cfg_ul_to_release_list_r14_present = false;
  bool                             sps_cfg_sl_to_add_mod_list_r14_present = false;
  bool                             sps_cfg_sl_to_release_list_r14_present = false;
  fixed_bitstring<16>              ul_sps_v_rnti_r14;
  fixed_bitstring<16>              sl_sps_v_rnti_r14;
  sps_cfg_ul_to_add_mod_list_r14_l sps_cfg_ul_to_add_mod_list_r14;
  sps_cfg_ul_to_release_list_r14_l sps_cfg_ul_to_release_list_r14;
  sps_cfg_sl_to_add_mod_list_r14_l sps_cfg_sl_to_add_mod_list_r14;
  sps_cfg_sl_to_release_list_r14_l sps_cfg_sl_to_release_list_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SPS-Config-v1530 ::= SEQUENCE
struct sps_cfg_v1530_s {
  bool                                  semi_persist_sched_c_rnti_r15_present       = false;
  bool                                  sps_cfg_dl_r15_present                      = false;
  bool                                  sps_cfg_ul_stti_to_add_mod_list_r15_present = false;
  bool                                  sps_cfg_ul_stti_to_release_list_r15_present = false;
  bool                                  sps_cfg_ul_to_add_mod_list_r15_present      = false;
  bool                                  sps_cfg_ul_to_release_list_r15_present      = false;
  fixed_bitstring<16>                   semi_persist_sched_c_rnti_r15;
  sps_cfg_dl_c                          sps_cfg_dl_r15;
  sps_cfg_ul_stti_to_add_mod_list_r15_l sps_cfg_ul_stti_to_add_mod_list_r15;
  sps_cfg_ul_stti_to_release_list_r15_l sps_cfg_ul_stti_to_release_list_r15;
  sps_cfg_ul_to_add_mod_list_r15_l      sps_cfg_ul_to_add_mod_list_r15;
  sps_cfg_ul_to_release_list_r15_l      sps_cfg_ul_to_release_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const sps_cfg_v1530_s& other) const;
  bool        operator!=(const sps_cfg_v1530_s& other) const { return not(*this == other); }
};

// SPS-Config-v1540 ::= SEQUENCE
struct sps_cfg_v1540_s {
  bool                  sps_cfg_dl_stti_r15_present = false;
  sps_cfg_dl_stti_r15_c sps_cfg_dl_stti_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRB-ToAddModList ::= SEQUENCE (SIZE (1..2)) OF SRB-ToAddMod
using srb_to_add_mod_list_l = dyn_array<srb_to_add_mod_s>;

// SRB-ToAddModListExt-r15 ::= SEQUENCE (SIZE (1)) OF SRB-ToAddMod
using srb_to_add_mod_list_ext_r15_l = std::array<srb_to_add_mod_s, 1>;

// RadioResourceConfigDedicated ::= SEQUENCE
struct rr_cfg_ded_s {
  struct mac_main_cfg_c_ {
    struct types_opts {
      enum options { explicit_value, default_value, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    mac_main_cfg_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mac_main_cfg_s& explicit_value()
    {
      assert_choice_type(types::explicit_value, type_, "mac-MainConfig");
      return c;
    }
    const mac_main_cfg_s& explicit_value() const
    {
      assert_choice_type(types::explicit_value, type_, "mac-MainConfig");
      return c;
    }
    mac_main_cfg_s& set_explicit_value();
    void            set_default_value();

  private:
    types          type_;
    mac_main_cfg_s c;
  };
  struct crs_intf_mitig_cfg_r15_c_ {
    struct setup_c_ {
      struct crs_intf_mitig_num_prbs_opts {
        enum options { n6, n24, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<crs_intf_mitig_num_prbs_opts> crs_intf_mitig_num_prbs_e_;
      struct types_opts {
        enum options { crs_intf_mitig_enabled, crs_intf_mitig_num_prbs, nulltype } value;

        const char* to_string() const;
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
      crs_intf_mitig_num_prbs_e_& crs_intf_mitig_num_prbs()
      {
        assert_choice_type(types::crs_intf_mitig_num_prbs, type_, "setup");
        return c;
      }
      const crs_intf_mitig_num_prbs_e_& crs_intf_mitig_num_prbs() const
      {
        assert_choice_type(types::crs_intf_mitig_num_prbs, type_, "setup");
        return c;
      }
      void                        set_crs_intf_mitig_enabled();
      crs_intf_mitig_num_prbs_e_& set_crs_intf_mitig_num_prbs();

    private:
      types                      type_;
      crs_intf_mitig_num_prbs_e_ c;
    };
    using types = setup_e;

    // choice methods
    crs_intf_mitig_cfg_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_c_& setup()
    {
      assert_choice_type(types::setup, type_, "crs-IntfMitigConfig-r15");
      return c;
    }
    const setup_c_& setup() const
    {
      assert_choice_type(types::setup, type_, "crs-IntfMitigConfig-r15");
      return c;
    }
    void      set_release();
    setup_c_& set_setup();

  private:
    types    type_;
    setup_c_ c;
  };
  using dummy_l_ = bounded_array<uint8_t, 2>;

  // member variables
  bool                  ext                         = false;
  bool                  srb_to_add_mod_list_present = false;
  bool                  drb_to_add_mod_list_present = false;
  bool                  drb_to_release_list_present = false;
  bool                  mac_main_cfg_present        = false;
  bool                  sps_cfg_present             = false;
  bool                  phys_cfg_ded_present        = false;
  srb_to_add_mod_list_l srb_to_add_mod_list;
  drb_to_add_mod_list_l drb_to_add_mod_list;
  drb_to_release_list_l drb_to_release_list;
  mac_main_cfg_c_       mac_main_cfg;
  sps_cfg_s             sps_cfg;
  phys_cfg_ded_s        phys_cfg_ded;
  // ...
  // group 0
  copy_ptr<rlf_timers_and_consts_r9_c> rlf_timers_and_consts_r9;
  // group 1
  copy_ptr<meas_sf_pattern_pcell_r10_c> meas_sf_pattern_pcell_r10;
  // group 2
  copy_ptr<neigh_cells_crs_info_r11_c> neigh_cells_crs_info_r11;
  // group 3
  copy_ptr<naics_assist_info_r12_c> naics_info_r12;
  // group 4
  copy_ptr<neigh_cells_crs_info_r13_c>  neigh_cells_crs_info_r13;
  copy_ptr<rlf_timers_and_consts_r13_c> rlf_timers_and_consts_r13;
  // group 5
  copy_ptr<sps_cfg_v1430_s> sps_cfg_v1430;
  // group 6
  bool                                    srb_to_release_list_ext_r15_present = false;
  copy_ptr<srb_to_add_mod_list_ext_r15_l> srb_to_add_mod_list_ext_r15;
  uint8_t                                 srb_to_release_list_ext_r15 = 4;
  copy_ptr<sps_cfg_v1530_s>               sps_cfg_v1530;
  copy_ptr<crs_intf_mitig_cfg_r15_c_>     crs_intf_mitig_cfg_r15;
  copy_ptr<neigh_cells_crs_info_r15_c>    neigh_cells_crs_info_r15;
  copy_ptr<drb_to_add_mod_list_r15_l>     drb_to_add_mod_list_r15;
  copy_ptr<drb_to_release_list_r15_l>     drb_to_release_list_r15;
  copy_ptr<dummy_l_>                      dummy;
  // group 7
  copy_ptr<sps_cfg_v1540_s> sps_cfg_v1540;
  // group 8
  bool                                                        new_ue_id_r16_present = false;
  copy_ptr<rlf_timers_and_consts_mcg_fail_r16_c>              rlf_timers_and_consts_mcg_fail_r16;
  copy_ptr<setup_release_c<crs_ch_est_mpdcch_cfg_ded_r16_s> > crs_ch_est_mpdcch_cfg_ded_r16;
  fixed_bitstring<16>                                         new_ue_id_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-CandidateReductionValue-r14 ::= ENUMERATED
struct pdcch_candidate_reduction_value_r14_opts {
  enum options { n0, n50, n100, n150, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<pdcch_candidate_reduction_value_r14_opts> pdcch_candidate_reduction_value_r14_e;

// PDCCH-CandidateReductionsLAA-UL-r14 ::= CHOICE
struct pdcch_candidate_reductions_laa_ul_r14_c {
  struct setup_s_ {
    pdcch_candidate_reduction_value_r13_e pdcch_candidate_reduction_al1_r14;
    pdcch_candidate_reduction_value_r13_e pdcch_candidate_reduction_al2_r14;
    pdcch_candidate_reduction_value_r14_e pdcch_candidate_reduction_al3_r14;
    pdcch_candidate_reduction_value_r14_e pdcch_candidate_reduction_al4_r14;
    pdcch_candidate_reduction_value_r14_e pdcch_candidate_reduction_al5_r14;
  };
  using types = setup_e;

  // choice methods
  pdcch_candidate_reductions_laa_ul_r14_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdcch_candidate_reductions_laa_ul_r14_c& other) const;
  bool        operator!=(const pdcch_candidate_reductions_laa_ul_r14_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "PDCCH-CandidateReductionsLAA-UL-r14");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "PDCCH-CandidateReductionsLAA-UL-r14");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// AUL-Config-r15 ::= CHOICE
struct aul_cfg_r15_c {
  struct setup_s_ {
    struct tx_mode_ul_aul_r15_opts {
      enum options { tm1, tm2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<tx_mode_ul_aul_r15_opts> tx_mode_ul_aul_r15_e_;
    struct aul_start_partial_bw_inside_mcot_r15_opts {
      enum options { o34, o43, o52, o61, oos1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<aul_start_partial_bw_inside_mcot_r15_opts> aul_start_partial_bw_inside_mcot_r15_e_;
    struct aul_start_partial_bw_outside_mcot_r15_opts {
      enum options { o16, o25, o34, o43, o52, o61, oos1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<aul_start_partial_bw_outside_mcot_r15_opts> aul_start_partial_bw_outside_mcot_r15_e_;
    struct aul_retx_timer_r15_opts {
      enum options {
        psf4,
        psf5,
        psf6,
        psf8,
        psf10,
        psf12,
        psf20,
        psf28,
        psf37,
        psf44,
        psf68,
        psf84,
        psf100,
        psf116,
        psf132,
        psf164,
        psf324,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<aul_retx_timer_r15_opts> aul_retx_timer_r15_e_;
    struct contention_win_size_timer_r15_opts {
      enum options { n0, n5, n10, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<contention_win_size_timer_r15_opts> contention_win_size_timer_r15_e_;

    // member variables
    fixed_bitstring<16>                      aul_crnti_r15;
    fixed_bitstring<40>                      aul_sfs_r15;
    uint8_t                                  aul_harq_processes_r15 = 1;
    tx_mode_ul_aul_r15_e_                    tx_mode_ul_aul_r15;
    fixed_bitstring<5>                       aul_start_full_bw_inside_mcot_r15;
    fixed_bitstring<7>                       aul_start_full_bw_outside_mcot_r15;
    aul_start_partial_bw_inside_mcot_r15_e_  aul_start_partial_bw_inside_mcot_r15;
    aul_start_partial_bw_outside_mcot_r15_e_ aul_start_partial_bw_outside_mcot_r15;
    aul_retx_timer_r15_e_                    aul_retx_timer_r15;
    uint8_t                                  ending_symbol_aul_r15     = 12;
    uint8_t                                  sf_offset_cot_sharing_r15 = 2;
    contention_win_size_timer_r15_e_         contention_win_size_timer_r15;
  };
  using types = setup_e;

  // choice methods
  aul_cfg_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const aul_cfg_r15_c& other) const;
  bool        operator!=(const aul_cfg_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "AUL-Config-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "AUL-Config-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CQI-ReportPeriodicSCell-r15 ::= CHOICE
struct cqi_report_periodic_scell_r15_c {
  struct setup_s_ {
    struct csi_sf_pattern_dormant_r15_c_ {
      struct setup_s_ {
        meas_sf_pattern_r10_c csi_meas_sf_set1_r15;
        meas_sf_pattern_r10_c csi_meas_sf_set2_r15;
      };
      using types = setup_e;

      // choice methods
      csi_sf_pattern_dormant_r15_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const csi_sf_pattern_dormant_r15_c_& other) const;
      bool        operator!=(const csi_sf_pattern_dormant_r15_c_& other) const { return not(*this == other); }
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "csi-SubframePatternDormant-r15");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "csi-SubframePatternDormant-r15");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };
    struct cqi_format_ind_dormant_r15_c_ {
      struct wideband_cqi_r15_s_ {
        struct csi_report_mode_r15_opts {
          enum options { submode1, submode2, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<csi_report_mode_r15_opts> csi_report_mode_r15_e_;

        // member variables
        bool                   csi_report_mode_r15_present = false;
        csi_report_mode_r15_e_ csi_report_mode_r15;
      };
      struct subband_cqi_r15_s_ {
        struct periodicity_factor_r15_opts {
          enum options { n2, n4, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<periodicity_factor_r15_opts> periodicity_factor_r15_e_;

        // member variables
        uint8_t                   k_r15 = 1;
        periodicity_factor_r15_e_ periodicity_factor_r15;
      };
      struct types_opts {
        enum options { wideband_cqi_r15, subband_cqi_r15, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      cqi_format_ind_dormant_r15_c_() = default;
      cqi_format_ind_dormant_r15_c_(const cqi_format_ind_dormant_r15_c_& other);
      cqi_format_ind_dormant_r15_c_& operator=(const cqi_format_ind_dormant_r15_c_& other);
      ~cqi_format_ind_dormant_r15_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const cqi_format_ind_dormant_r15_c_& other) const;
      bool        operator!=(const cqi_format_ind_dormant_r15_c_& other) const { return not(*this == other); }
      // getters
      wideband_cqi_r15_s_& wideband_cqi_r15()
      {
        assert_choice_type(types::wideband_cqi_r15, type_, "cqi-FormatIndicatorDormant-r15");
        return c.get<wideband_cqi_r15_s_>();
      }
      subband_cqi_r15_s_& subband_cqi_r15()
      {
        assert_choice_type(types::subband_cqi_r15, type_, "cqi-FormatIndicatorDormant-r15");
        return c.get<subband_cqi_r15_s_>();
      }
      const wideband_cqi_r15_s_& wideband_cqi_r15() const
      {
        assert_choice_type(types::wideband_cqi_r15, type_, "cqi-FormatIndicatorDormant-r15");
        return c.get<wideband_cqi_r15_s_>();
      }
      const subband_cqi_r15_s_& subband_cqi_r15() const
      {
        assert_choice_type(types::subband_cqi_r15, type_, "cqi-FormatIndicatorDormant-r15");
        return c.get<subband_cqi_r15_s_>();
      }
      wideband_cqi_r15_s_& set_wideband_cqi_r15();
      subband_cqi_r15_s_&  set_subband_cqi_r15();

    private:
      types                                                    type_;
      choice_buffer_t<subband_cqi_r15_s_, wideband_cqi_r15_s_> c;

      void destroy_();
    };

    // member variables
    bool                          ri_cfg_idx_dormant_r15_present     = false;
    bool                          csi_sf_pattern_dormant_r15_present = false;
    bool                          cqi_format_ind_dormant_r15_present = false;
    uint16_t                      cqi_pmi_cfg_idx_dormant_r15        = 0;
    uint16_t                      ri_cfg_idx_dormant_r15             = 0;
    csi_sf_pattern_dormant_r15_c_ csi_sf_pattern_dormant_r15;
    cqi_format_ind_dormant_r15_c_ cqi_format_ind_dormant_r15;
  };
  using types = setup_e;

  // choice methods
  cqi_report_periodic_scell_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_periodic_scell_r15_c& other) const;
  bool        operator!=(const cqi_report_periodic_scell_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "CQI-ReportPeriodicSCell-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "CQI-ReportPeriodicSCell-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CrossCarrierSchedulingConfigLAA-UL-r14 ::= SEQUENCE
struct cross_carrier_sched_cfg_laa_ul_r14_s {
  uint8_t sched_cell_id_r14     = 0;
  uint8_t cif_in_sched_cell_r14 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cross_carrier_sched_cfg_laa_ul_r14_s& other) const;
  bool        operator!=(const cross_carrier_sched_cfg_laa_ul_r14_s& other) const { return not(*this == other); }
};

// LBT-Config-r14 ::= CHOICE
struct lbt_cfg_r14_c {
  struct types_opts {
    enum options { max_energy_detection_thres_r14, energy_detection_thres_offset_r14, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  lbt_cfg_r14_c() = default;
  lbt_cfg_r14_c(const lbt_cfg_r14_c& other);
  lbt_cfg_r14_c& operator=(const lbt_cfg_r14_c& other);
  ~lbt_cfg_r14_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const lbt_cfg_r14_c& other) const;
  bool        operator!=(const lbt_cfg_r14_c& other) const { return not(*this == other); }
  // getters
  int8_t& max_energy_detection_thres_r14()
  {
    assert_choice_type(types::max_energy_detection_thres_r14, type_, "LBT-Config-r14");
    return c.get<int8_t>();
  }
  int8_t& energy_detection_thres_offset_r14()
  {
    assert_choice_type(types::energy_detection_thres_offset_r14, type_, "LBT-Config-r14");
    return c.get<int8_t>();
  }
  const int8_t& max_energy_detection_thres_r14() const
  {
    assert_choice_type(types::max_energy_detection_thres_r14, type_, "LBT-Config-r14");
    return c.get<int8_t>();
  }
  const int8_t& energy_detection_thres_offset_r14() const
  {
    assert_choice_type(types::energy_detection_thres_offset_r14, type_, "LBT-Config-r14");
    return c.get<int8_t>();
  }
  int8_t& set_max_energy_detection_thres_r14();
  int8_t& set_energy_detection_thres_offset_r14();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// PDCCH-ConfigLAA-r14 ::= SEQUENCE
struct pdcch_cfg_laa_r14_s {
  struct max_nof_sched_sfs_format0_b_r14_opts {
    enum options { sf2, sf3, sf4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_nof_sched_sfs_format0_b_r14_opts> max_nof_sched_sfs_format0_b_r14_e_;
  struct max_nof_sched_sfs_format4_b_r14_opts {
    enum options { sf2, sf3, sf4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_nof_sched_sfs_format4_b_r14_opts> max_nof_sched_sfs_format4_b_r14_e_;

  // member variables
  bool                                    max_nof_sched_sfs_format0_b_r14_present          = false;
  bool                                    max_nof_sched_sfs_format4_b_r14_present          = false;
  bool                                    skip_monitoring_dci_format0_a_r14_present        = false;
  bool                                    skip_monitoring_dci_format4_a_r14_present        = false;
  bool                                    pdcch_candidate_reductions_format0_a_r14_present = false;
  bool                                    pdcch_candidate_reductions_format4_a_r14_present = false;
  bool                                    pdcch_candidate_reductions_format0_b_r14_present = false;
  bool                                    pdcch_candidate_reductions_format4_b_r14_present = false;
  max_nof_sched_sfs_format0_b_r14_e_      max_nof_sched_sfs_format0_b_r14;
  max_nof_sched_sfs_format4_b_r14_e_      max_nof_sched_sfs_format4_b_r14;
  pdcch_candidate_reductions_r13_c        pdcch_candidate_reductions_format0_a_r14;
  pdcch_candidate_reductions_laa_ul_r14_c pdcch_candidate_reductions_format4_a_r14;
  pdcch_candidate_reductions_laa_ul_r14_c pdcch_candidate_reductions_format0_b_r14;
  pdcch_candidate_reductions_laa_ul_r14_c pdcch_candidate_reductions_format4_b_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdcch_cfg_laa_r14_s& other) const;
  bool        operator!=(const pdcch_cfg_laa_r14_s& other) const { return not(*this == other); }
};

// PUSCH-ModeConfigLAA-r15 ::= SEQUENCE
struct pusch_mode_cfg_laa_r15_s {
  bool laa_pusch_mode1 = false;
  bool laa_pusch_mode2 = false;
  bool laa_pusch_mode3 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pusch_mode_cfg_laa_r15_s& other) const;
  bool        operator!=(const pusch_mode_cfg_laa_r15_s& other) const { return not(*this == other); }
};

// SoundingRS-UL-ConfigDedicatedAperiodic-v1430 ::= CHOICE
struct srs_ul_cfg_ded_aperiodic_v1430_c {
  struct setup_s_ {
    bool    srs_sf_ind_r14_present = false;
    uint8_t srs_sf_ind_r14         = 1;
  };
  using types = setup_e;

  // choice methods
  srs_ul_cfg_ded_aperiodic_v1430_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_ul_cfg_ded_aperiodic_v1430_c& other) const;
  bool        operator!=(const srs_ul_cfg_ded_aperiodic_v1430_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicatedAperiodic-v1430");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigDedicatedAperiodic-v1430");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CQI-ReportConfigSCell-r10 ::= SEQUENCE
struct cqi_report_cfg_scell_r10_s {
  bool                        cqi_report_mode_aperiodic_r10_present = false;
  bool                        cqi_report_periodic_scell_r10_present = false;
  bool                        pmi_ri_report_r10_present             = false;
  cqi_report_mode_aperiodic_e cqi_report_mode_aperiodic_r10;
  int8_t                      nom_pdsch_rs_epre_offset_r10 = -1;
  cqi_report_periodic_r10_c   cqi_report_periodic_scell_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_cfg_scell_r10_s& other) const;
  bool        operator!=(const cqi_report_cfg_scell_r10_s& other) const { return not(*this == other); }
};

// CQI-ReportConfigSCell-r15 ::= SEQUENCE
struct cqi_report_cfg_scell_r15_s {
  struct alt_cqi_table_minus1024_qam_r15_opts {
    enum options { all_sfs, csi_sf_set1, csi_sf_set2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<alt_cqi_table_minus1024_qam_r15_opts> alt_cqi_table_minus1024_qam_r15_e_;

  // member variables
  bool                               cqi_report_periodic_scell_r15_present   = false;
  bool                               alt_cqi_table_minus1024_qam_r15_present = false;
  cqi_report_periodic_scell_r15_c    cqi_report_periodic_scell_r15;
  alt_cqi_table_minus1024_qam_r15_e_ alt_cqi_table_minus1024_qam_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_cfg_scell_r15_s& other) const;
  bool        operator!=(const cqi_report_cfg_scell_r15_s& other) const { return not(*this == other); }
};

// CQI-ReportPeriodicSCell-v1730 ::= SEQUENCE
struct cqi_report_periodic_scell_v1730_s {
  bool     ri_cfg_idx2_dormant_r17_present = false;
  uint16_t cqi_pmi_cfg_idx2_dormant_r17    = 0;
  uint16_t ri_cfg_idx2_dormant_r17         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_report_periodic_scell_v1730_s& other) const;
  bool        operator!=(const cqi_report_periodic_scell_v1730_s& other) const { return not(*this == other); }
};

// CQI-ShortConfigSCell-r15 ::= CHOICE
struct cqi_short_cfg_scell_r15_c {
  struct setup_s_ {
    struct cqi_format_ind_short_r15_c_ {
      struct wideband_cqi_short_r15_s_ {
        struct csi_report_mode_short_r15_opts {
          enum options { submode1, submode2, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<csi_report_mode_short_r15_opts> csi_report_mode_short_r15_e_;

        // member variables
        bool                         csi_report_mode_short_r15_present = false;
        csi_report_mode_short_r15_e_ csi_report_mode_short_r15;
      };
      struct subband_cqi_short_r15_s_ {
        struct periodicity_factor_r15_opts {
          enum options { n2, n4, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<periodicity_factor_r15_opts> periodicity_factor_r15_e_;

        // member variables
        uint8_t                   k_r15 = 1;
        periodicity_factor_r15_e_ periodicity_factor_r15;
      };
      struct types_opts {
        enum options { wideband_cqi_short_r15, subband_cqi_short_r15, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      cqi_format_ind_short_r15_c_() = default;
      cqi_format_ind_short_r15_c_(const cqi_format_ind_short_r15_c_& other);
      cqi_format_ind_short_r15_c_& operator=(const cqi_format_ind_short_r15_c_& other);
      ~cqi_format_ind_short_r15_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const cqi_format_ind_short_r15_c_& other) const;
      bool        operator!=(const cqi_format_ind_short_r15_c_& other) const { return not(*this == other); }
      // getters
      wideband_cqi_short_r15_s_& wideband_cqi_short_r15()
      {
        assert_choice_type(types::wideband_cqi_short_r15, type_, "cqi-FormatIndicatorShort-r15");
        return c.get<wideband_cqi_short_r15_s_>();
      }
      subband_cqi_short_r15_s_& subband_cqi_short_r15()
      {
        assert_choice_type(types::subband_cqi_short_r15, type_, "cqi-FormatIndicatorShort-r15");
        return c.get<subband_cqi_short_r15_s_>();
      }
      const wideband_cqi_short_r15_s_& wideband_cqi_short_r15() const
      {
        assert_choice_type(types::wideband_cqi_short_r15, type_, "cqi-FormatIndicatorShort-r15");
        return c.get<wideband_cqi_short_r15_s_>();
      }
      const subband_cqi_short_r15_s_& subband_cqi_short_r15() const
      {
        assert_choice_type(types::subband_cqi_short_r15, type_, "cqi-FormatIndicatorShort-r15");
        return c.get<subband_cqi_short_r15_s_>();
      }
      wideband_cqi_short_r15_s_& set_wideband_cqi_short_r15();
      subband_cqi_short_r15_s_&  set_subband_cqi_short_r15();

    private:
      types                                                                type_;
      choice_buffer_t<subband_cqi_short_r15_s_, wideband_cqi_short_r15_s_> c;

      void destroy_();
    };

    // member variables
    bool                        ri_cfg_idx_short_r15_present     = false;
    bool                        cqi_format_ind_short_r15_present = false;
    uint16_t                    cqi_pmi_cfg_idx_short_r15        = 0;
    uint16_t                    ri_cfg_idx_short_r15             = 0;
    cqi_format_ind_short_r15_c_ cqi_format_ind_short_r15;
  };
  using types = setup_e;

  // choice methods
  cqi_short_cfg_scell_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cqi_short_cfg_scell_r15_c& other) const;
  bool        operator!=(const cqi_short_cfg_scell_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "CQI-ShortConfigSCell-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "CQI-ShortConfigSCell-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// CrossCarrierSchedulingConfig-r10 ::= SEQUENCE
struct cross_carrier_sched_cfg_r10_s {
  struct sched_cell_info_r10_c_ {
    struct own_r10_s_ {
      bool cif_presence_r10 = false;
    };
    struct other_r10_s_ {
      uint8_t sched_cell_id_r10 = 0;
      uint8_t pdsch_start_r10   = 1;
    };
    struct types_opts {
      enum options { own_r10, other_r10, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sched_cell_info_r10_c_() = default;
    sched_cell_info_r10_c_(const sched_cell_info_r10_c_& other);
    sched_cell_info_r10_c_& operator=(const sched_cell_info_r10_c_& other);
    ~sched_cell_info_r10_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const sched_cell_info_r10_c_& other) const;
    bool        operator!=(const sched_cell_info_r10_c_& other) const { return not(*this == other); }
    // getters
    own_r10_s_& own_r10()
    {
      assert_choice_type(types::own_r10, type_, "schedulingCellInfo-r10");
      return c.get<own_r10_s_>();
    }
    other_r10_s_& other_r10()
    {
      assert_choice_type(types::other_r10, type_, "schedulingCellInfo-r10");
      return c.get<other_r10_s_>();
    }
    const own_r10_s_& own_r10() const
    {
      assert_choice_type(types::own_r10, type_, "schedulingCellInfo-r10");
      return c.get<own_r10_s_>();
    }
    const other_r10_s_& other_r10() const
    {
      assert_choice_type(types::other_r10, type_, "schedulingCellInfo-r10");
      return c.get<other_r10_s_>();
    }
    own_r10_s_&   set_own_r10();
    other_r10_s_& set_other_r10();

  private:
    types                                     type_;
    choice_buffer_t<other_r10_s_, own_r10_s_> c;

    void destroy_();
  };

  // member variables
  sched_cell_info_r10_c_ sched_cell_info_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cross_carrier_sched_cfg_r10_s& other) const;
  bool        operator!=(const cross_carrier_sched_cfg_r10_s& other) const { return not(*this == other); }
};

// CrossCarrierSchedulingConfig-r13 ::= SEQUENCE
struct cross_carrier_sched_cfg_r13_s {
  struct sched_cell_info_r13_c_ {
    struct own_r13_s_ {
      bool cif_presence_r13 = false;
    };
    struct other_r13_s_ {
      uint8_t sched_cell_id_r13     = 0;
      uint8_t pdsch_start_r13       = 1;
      uint8_t cif_in_sched_cell_r13 = 1;
    };
    struct types_opts {
      enum options { own_r13, other_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sched_cell_info_r13_c_() = default;
    sched_cell_info_r13_c_(const sched_cell_info_r13_c_& other);
    sched_cell_info_r13_c_& operator=(const sched_cell_info_r13_c_& other);
    ~sched_cell_info_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const sched_cell_info_r13_c_& other) const;
    bool        operator!=(const sched_cell_info_r13_c_& other) const { return not(*this == other); }
    // getters
    own_r13_s_& own_r13()
    {
      assert_choice_type(types::own_r13, type_, "schedulingCellInfo-r13");
      return c.get<own_r13_s_>();
    }
    other_r13_s_& other_r13()
    {
      assert_choice_type(types::other_r13, type_, "schedulingCellInfo-r13");
      return c.get<other_r13_s_>();
    }
    const own_r13_s_& own_r13() const
    {
      assert_choice_type(types::own_r13, type_, "schedulingCellInfo-r13");
      return c.get<own_r13_s_>();
    }
    const other_r13_s_& other_r13() const
    {
      assert_choice_type(types::other_r13, type_, "schedulingCellInfo-r13");
      return c.get<other_r13_s_>();
    }
    own_r13_s_&   set_own_r13();
    other_r13_s_& set_other_r13();

  private:
    types                                     type_;
    choice_buffer_t<other_r13_s_, own_r13_s_> c;

    void destroy_();
  };

  // member variables
  sched_cell_info_r13_c_ sched_cell_info_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cross_carrier_sched_cfg_r13_s& other) const;
  bool        operator!=(const cross_carrier_sched_cfg_r13_s& other) const { return not(*this == other); }
};

// LAA-SCellConfiguration-r13 ::= SEQUENCE
struct laa_scell_cfg_r13_s {
  struct sf_start_position_r13_opts {
    enum options { s0, s07, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<sf_start_position_r13_opts> sf_start_position_r13_e_;

  // member variables
  sf_start_position_r13_e_ sf_start_position_r13;
  fixed_bitstring<8>       laa_scell_sf_cfg_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const laa_scell_cfg_r13_s& other) const;
  bool        operator!=(const laa_scell_cfg_r13_s& other) const { return not(*this == other); }
};

// LAA-SCellConfiguration-v1430 ::= SEQUENCE
struct laa_scell_cfg_v1430_s {
  struct cross_carrier_sched_cfg_ul_r14_c_ {
    struct setup_s_ {
      cross_carrier_sched_cfg_laa_ul_r14_s cross_carrier_sched_cfg_laa_ul_r14;
    };
    using types = setup_e;

    // choice methods
    cross_carrier_sched_cfg_ul_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const cross_carrier_sched_cfg_ul_r14_c_& other) const;
    bool        operator!=(const cross_carrier_sched_cfg_ul_r14_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "crossCarrierSchedulingConfig-UL-r14");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "crossCarrierSchedulingConfig-UL-r14");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                              cross_carrier_sched_cfg_ul_r14_present      = false;
  bool                              lbt_cfg_r14_present                         = false;
  bool                              pdcch_cfg_laa_r14_present                   = false;
  bool                              absence_of_any_other_technology_r14_present = false;
  bool                              srs_ul_cfg_ded_aperiodic_v1430_present      = false;
  cross_carrier_sched_cfg_ul_r14_c_ cross_carrier_sched_cfg_ul_r14;
  lbt_cfg_r14_c                     lbt_cfg_r14;
  pdcch_cfg_laa_r14_s               pdcch_cfg_laa_r14;
  srs_ul_cfg_ded_aperiodic_v1430_c  srs_ul_cfg_ded_aperiodic_v1430;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const laa_scell_cfg_v1430_s& other) const;
  bool        operator!=(const laa_scell_cfg_v1430_s& other) const { return not(*this == other); }
};

// LAA-SCellConfiguration-v1530 ::= SEQUENCE
struct laa_scell_cfg_v1530_s {
  bool                     aul_cfg_r15_present            = false;
  bool                     pusch_mode_cfg_laa_r15_present = false;
  aul_cfg_r15_c            aul_cfg_r15;
  pusch_mode_cfg_laa_r15_s pusch_mode_cfg_laa_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const laa_scell_cfg_v1530_s& other) const;
  bool        operator!=(const laa_scell_cfg_v1530_s& other) const { return not(*this == other); }
};

// PDCCH-ConfigSCell-r13 ::= SEQUENCE
struct pdcch_cfg_scell_r13_s {
  bool skip_monitoring_dci_format0_minus1_a_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdcch_cfg_scell_r13_s& other) const;
  bool        operator!=(const pdcch_cfg_scell_r13_s& other) const { return not(*this == other); }
};

// PDSCH-ConfigDedicatedSCell-v1430 ::= SEQUENCE
struct pdsch_cfg_ded_scell_v1430_s {
  bool tbs_idx_alt2_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdsch_cfg_ded_scell_v1430_s& other) const;
  bool        operator!=(const pdsch_cfg_ded_scell_v1430_s& other) const { return not(*this == other); }
};

// PUCCH-ConfigDedicated-v1370 ::= SEQUENCE
struct pucch_cfg_ded_v1370_s {
  struct pucch_format_v1370_c_ {
    using types = setup_e;

    // choice methods
    pucch_format_v1370_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const pucch_format_v1370_c_& other) const;
    bool        operator!=(const pucch_format_v1370_c_& other) const { return not(*this == other); }
    // getters
    pucch_format3_conf_r13_s& setup()
    {
      assert_choice_type(types::setup, type_, "pucch-Format-v1370");
      return c;
    }
    const pucch_format3_conf_r13_s& setup() const
    {
      assert_choice_type(types::setup, type_, "pucch-Format-v1370");
      return c;
    }
    void                      set_release();
    pucch_format3_conf_r13_s& set_setup();

  private:
    types                    type_;
    pucch_format3_conf_r13_s c;
  };

  // member variables
  pucch_format_v1370_c_ pucch_format_v1370;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pucch_cfg_ded_v1370_s& other) const;
  bool        operator!=(const pucch_cfg_ded_v1370_s& other) const { return not(*this == other); }
};

// PUSCH-ConfigDedicatedSCell-r10 ::= SEQUENCE
struct pusch_cfg_ded_scell_r10_s {
  bool group_hop_disabled_r10_present  = false;
  bool dmrs_with_occ_activ_r10_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pusch_cfg_ded_scell_r10_s& other) const;
  bool        operator!=(const pusch_cfg_ded_scell_r10_s& other) const { return not(*this == other); }
};

// PUSCH-ConfigDedicatedSCell-v1430 ::= SEQUENCE
struct pusch_cfg_ded_scell_v1430_s {
  bool                enable256_qam_r14_present = false;
  enable256_qam_r14_c enable256_qam_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pusch_cfg_ded_scell_v1430_s& other) const;
  bool        operator!=(const pusch_cfg_ded_scell_v1430_s& other) const { return not(*this == other); }
};

// PUSCH-ConfigDedicatedScell-v1530 ::= SEQUENCE
struct pusch_cfg_ded_scell_v1530_s {
  struct uci_on_pusch_r15_c_ {
    struct setup_s_ {
      uint8_t beta_offset_aul_r15 = 0;
    };
    using types = setup_e;

    // choice methods
    uci_on_pusch_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const uci_on_pusch_r15_c_& other) const;
    bool        operator!=(const uci_on_pusch_r15_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "uci-OnPUSCH-r15");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "uci-OnPUSCH-r15");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  uci_on_pusch_r15_c_ uci_on_pusch_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pusch_cfg_ded_scell_v1530_s& other) const;
  bool        operator!=(const pusch_cfg_ded_scell_v1530_s& other) const { return not(*this == other); }
};

// SchedulingRequestConfigSCell-r13 ::= CHOICE
struct sched_request_cfg_scell_r13_c {
  struct setup_s_ {
    struct dsr_trans_max_r13_opts {
      enum options { n4, n8, n16, n32, n64, spare3, spare2, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<dsr_trans_max_r13_opts> dsr_trans_max_r13_e_;

    // member variables
    bool                 sr_pucch_res_idx_p1_r13_present = false;
    uint16_t             sr_pucch_res_idx_r13            = 0;
    uint16_t             sr_pucch_res_idx_p1_r13         = 0;
    uint8_t              sr_cfg_idx_r13                  = 0;
    dsr_trans_max_r13_e_ dsr_trans_max_r13;
  };
  using types = setup_e;

  // choice methods
  sched_request_cfg_scell_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const sched_request_cfg_scell_r13_c& other) const;
  bool        operator!=(const sched_request_cfg_scell_r13_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SchedulingRequestConfigSCell-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SchedulingRequestConfigSCell-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SoundingRS-AperiodicSet-r14 ::= SEQUENCE
struct srs_aperiodic_set_r14_s {
  using srs_cc_set_idx_list_r14_l_ = dyn_array<srs_cc_set_idx_r14_s>;

  // member variables
  bool                           srs_cc_set_idx_list_r14_present = false;
  srs_cc_set_idx_list_r14_l_     srs_cc_set_idx_list_r14;
  srs_ul_cfg_ded_aperiodic_r10_c srs_ul_cfg_ded_aperiodic_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_aperiodic_set_r14_s& other) const;
  bool        operator!=(const srs_aperiodic_set_r14_s& other) const { return not(*this == other); }
};

// SoundingRS-AperiodicSetUpPTsExt-r14 ::= SEQUENCE
struct srs_aperiodic_set_up_pts_ext_r14_s {
  using srs_cc_set_idx_list_r14_l_ = dyn_array<srs_cc_set_idx_r14_s>;

  // member variables
  bool                                      srs_cc_set_idx_list_r14_present = false;
  srs_cc_set_idx_list_r14_l_                srs_cc_set_idx_list_r14;
  srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c srs_ul_cfg_ded_aperiodic_up_pts_ext_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_aperiodic_set_up_pts_ext_r14_s& other) const;
  bool        operator!=(const srs_aperiodic_set_up_pts_ext_r14_s& other) const { return not(*this == other); }
};

// TPC-PDCCH-ConfigSCell-r13 ::= CHOICE
struct tpc_pdcch_cfg_scell_r13_c {
  struct setup_s_ {
    tpc_idx_c tpc_idx_pucch_scell_r13;
  };
  using types = setup_e;

  // choice methods
  tpc_pdcch_cfg_scell_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const tpc_pdcch_cfg_scell_r13_c& other) const;
  bool        operator!=(const tpc_pdcch_cfg_scell_r13_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "TPC-PDCCH-ConfigSCell-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "TPC-PDCCH-ConfigSCell-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// UplinkPUSCH-LessPowerControlDedicated-v1430 ::= SEQUENCE
struct ul_pusch_less_pwr_ctrl_ded_v1430_s {
  bool   p0_ue_periodic_srs_r14_present  = false;
  bool   p0_ue_aperiodic_srs_r14_present = false;
  int8_t p0_ue_periodic_srs_r14          = -8;
  int8_t p0_ue_aperiodic_srs_r14         = -8;
  bool   accumulation_enabled_r14        = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pusch_less_pwr_ctrl_ded_v1430_s& other) const;
  bool        operator!=(const ul_pusch_less_pwr_ctrl_ded_v1430_s& other) const { return not(*this == other); }
};

// UplinkPowerControlDedicatedSCell-r10 ::= SEQUENCE
struct ul_pwr_ctrl_ded_scell_r10_s {
  struct delta_mcs_enabled_r10_opts {
    enum options { en0, en1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<delta_mcs_enabled_r10_opts> delta_mcs_enabled_r10_e_;
  struct pathloss_ref_linking_r10_opts {
    enum options { pcell, scell, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<pathloss_ref_linking_r10_opts> pathloss_ref_linking_r10_e_;

  // member variables
  bool                        psrs_offset_ap_r10_present = false;
  bool                        filt_coef_r10_present      = false;
  int8_t                      p0_ue_pusch_r10            = -8;
  delta_mcs_enabled_r10_e_    delta_mcs_enabled_r10;
  bool                        accumulation_enabled_r10 = false;
  uint8_t                     psrs_offset_r10          = 0;
  uint8_t                     psrs_offset_ap_r10       = 0;
  filt_coef_e                 filt_coef_r10;
  pathloss_ref_linking_r10_e_ pathloss_ref_linking_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pwr_ctrl_ded_scell_r10_s& other) const;
  bool        operator!=(const ul_pwr_ctrl_ded_scell_r10_s& other) const { return not(*this == other); }
};

// UplinkPowerControlDedicatedSCell-v1310 ::= SEQUENCE
struct ul_pwr_ctrl_ded_scell_v1310_s {
  bool                               delta_tx_d_offset_list_pucch_r10_present = false;
  int8_t                             p0_ue_pucch                              = -8;
  delta_tx_d_offset_list_pucch_r10_s delta_tx_d_offset_list_pucch_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pwr_ctrl_ded_scell_v1310_s& other) const;
  bool        operator!=(const ul_pwr_ctrl_ded_scell_v1310_s& other) const { return not(*this == other); }
};

// MAC-MainConfigSCell-r11 ::= SEQUENCE
struct mac_main_cfg_scell_r11_s {
  bool    ext                 = false;
  bool    stag_id_r11_present = false;
  uint8_t stag_id_r11         = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const mac_main_cfg_scell_r11_s& other) const;
  bool        operator!=(const mac_main_cfg_scell_r11_s& other) const { return not(*this == other); }
};

// PhysicalConfigDedicatedSCell-r10 ::= SEQUENCE
struct phys_cfg_ded_scell_r10_s {
  struct non_ul_cfg_r10_s_ {
    bool                          ant_info_r10_present                = false;
    bool                          cross_carrier_sched_cfg_r10_present = false;
    bool                          csi_rs_cfg_r10_present              = false;
    bool                          pdsch_cfg_ded_r10_present           = false;
    ant_info_ded_r10_s            ant_info_r10;
    cross_carrier_sched_cfg_r10_s cross_carrier_sched_cfg_r10;
    csi_rs_cfg_r10_s              csi_rs_cfg_r10;
    pdsch_cfg_ded_s               pdsch_cfg_ded_r10;
  };
  struct ul_cfg_r10_s_ {
    bool                           ant_info_ul_r10_present              = false;
    bool                           pusch_cfg_ded_scell_r10_present      = false;
    bool                           ul_pwr_ctrl_ded_scell_r10_present    = false;
    bool                           cqi_report_cfg_scell_r10_present     = false;
    bool                           srs_ul_cfg_ded_r10_present           = false;
    bool                           srs_ul_cfg_ded_v1020_present         = false;
    bool                           srs_ul_cfg_ded_aperiodic_r10_present = false;
    ant_info_ul_r10_s              ant_info_ul_r10;
    pusch_cfg_ded_scell_r10_s      pusch_cfg_ded_scell_r10;
    ul_pwr_ctrl_ded_scell_r10_s    ul_pwr_ctrl_ded_scell_r10;
    cqi_report_cfg_scell_r10_s     cqi_report_cfg_scell_r10;
    srs_ul_cfg_ded_c               srs_ul_cfg_ded_r10;
    srs_ul_cfg_ded_v1020_s         srs_ul_cfg_ded_v1020;
    srs_ul_cfg_ded_aperiodic_r10_c srs_ul_cfg_ded_aperiodic_r10;
  };
  struct pucch_scell_c_ {
    struct setup_s_ {
      bool                          pucch_cfg_ded_r13_present             = false;
      bool                          sched_request_cfg_r13_present         = false;
      bool                          tpc_pdcch_cfg_pucch_scell_r13_present = false;
      bool                          pusch_cfg_ded_r13_present             = false;
      bool                          ul_pwr_ctrl_ded_r13_present           = false;
      pucch_cfg_ded_r13_s           pucch_cfg_ded_r13;
      sched_request_cfg_scell_r13_c sched_request_cfg_r13;
      tpc_pdcch_cfg_scell_r13_c     tpc_pdcch_cfg_pucch_scell_r13;
      pusch_cfg_ded_r13_s           pusch_cfg_ded_r13;
      ul_pwr_ctrl_ded_scell_v1310_s ul_pwr_ctrl_ded_r13;
    };
    using types = setup_e;

    // choice methods
    pucch_scell_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const pucch_scell_c_& other) const;
    bool        operator!=(const pucch_scell_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "pucch-SCell");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "pucch-SCell");
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
  using srs_ul_aperiodic_cfg_ded_list_r14_l_           = dyn_array<srs_aperiodic_set_r14_s>;
  using srs_ul_cfg_ded_ap_up_pts_ext_list_r14_l_       = dyn_array<srs_aperiodic_set_up_pts_ext_r14_s>;
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
    bool        operator==(const must_cfg_r14_c_& other) const;
    bool        operator!=(const must_cfg_r14_c_& other) const { return not(*this == other); }
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
      bool        operator==(const setup_c_& other) const;
      bool        operator!=(const setup_c_& other) const { return not(*this == other); }
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
    bool        operator==(const semi_static_cfi_cfg_r15_c_& other) const;
    bool        operator!=(const semi_static_cfi_cfg_r15_c_& other) const { return not(*this == other); }
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
    bool        operator==(const blind_pdsch_repeat_cfg_r15_c_& other) const;
    bool        operator!=(const blind_pdsch_repeat_cfg_r15_c_& other) const { return not(*this == other); }
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

  // member variables
  bool              ext                    = false;
  bool              non_ul_cfg_r10_present = false;
  bool              ul_cfg_r10_present     = false;
  non_ul_cfg_r10_s_ non_ul_cfg_r10;
  ul_cfg_r10_s_     ul_cfg_r10;
  // ...
  // group 0
  copy_ptr<csi_rs_cfg_nzp_to_release_list_r11_l> csi_rs_cfg_nzp_to_release_list_r11;
  copy_ptr<csi_rs_cfg_nzp_to_add_mod_list_r11_l> csi_rs_cfg_nzp_to_add_mod_list_r11;
  copy_ptr<csi_rs_cfg_zp_to_release_list_r11_l>  csi_rs_cfg_zp_to_release_list_r11;
  copy_ptr<csi_rs_cfg_zp_to_add_mod_list_r11_l>  csi_rs_cfg_zp_to_add_mod_list_r11;
  copy_ptr<epdcch_cfg_r11_s>                     epdcch_cfg_r11;
  copy_ptr<pdsch_cfg_ded_v1130_s>                pdsch_cfg_ded_v1130;
  copy_ptr<cqi_report_cfg_v1130_s>               cqi_report_cfg_v1130;
  copy_ptr<pusch_cfg_ded_v1130_s>                pusch_cfg_ded_v1130;
  copy_ptr<ul_pwr_ctrl_ded_v1130_s>              ul_pwr_ctrl_ded_scell_v1130;
  // group 1
  copy_ptr<ant_info_ded_v1250_s>           ant_info_v1250;
  copy_ptr<eimta_main_cfg_serv_cell_r12_c> eimta_main_cfg_scell_r12;
  copy_ptr<cqi_report_cfg_v1250_s>         cqi_report_cfg_scell_v1250;
  copy_ptr<ul_pwr_ctrl_ded_v1250_s>        ul_pwr_ctrl_ded_scell_v1250;
  copy_ptr<csi_rs_cfg_v1250_s>             csi_rs_cfg_v1250;
  // group 2
  copy_ptr<pdsch_cfg_ded_v1280_s> pdsch_cfg_ded_v1280;
  // group 3
  bool                                                pucch_cell_r13_present = false;
  copy_ptr<pucch_scell_c_>                            pucch_scell;
  copy_ptr<cross_carrier_sched_cfg_r13_s>             cross_carrier_sched_cfg_r13;
  copy_ptr<pdcch_cfg_scell_r13_s>                     pdcch_cfg_scell_r13;
  copy_ptr<cqi_report_cfg_v1310_s>                    cqi_report_cfg_v1310;
  copy_ptr<pdsch_cfg_ded_v1310_s>                     pdsch_cfg_ded_v1310;
  copy_ptr<srs_ul_cfg_ded_v1310_c>                    srs_ul_cfg_ded_v1310;
  copy_ptr<srs_ul_cfg_ded_up_pts_ext_r13_c>           srs_ul_cfg_ded_up_pts_ext_r13;
  copy_ptr<srs_ul_cfg_ded_aperiodic_v1310_c>          srs_ul_cfg_ded_aperiodic_v1310;
  copy_ptr<srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c> srs_ul_cfg_ded_aperiodic_up_pts_ext_r13;
  copy_ptr<csi_rs_cfg_v1310_s>                        csi_rs_cfg_v1310;
  copy_ptr<laa_scell_cfg_r13_s>                       laa_scell_cfg_r13;
  copy_ptr<csi_rs_cfg_nzp_to_add_mod_list_ext_r13_l>  csi_rs_cfg_nzp_to_add_mod_list_ext_r13;
  copy_ptr<csi_rs_cfg_nzp_to_release_list_ext_r13_l>  csi_rs_cfg_nzp_to_release_list_ext_r13;
  // group 4
  copy_ptr<cqi_report_cfg_v1320_s> cqi_report_cfg_v1320;
  // group 5
  bool                                                     semi_open_loop_r14_present = false;
  copy_ptr<laa_scell_cfg_v1430_s>                          laa_scell_cfg_v1430;
  copy_ptr<srs_tpc_pdcch_cfg_r14_c>                        type_b_srs_tpc_pdcch_cfg_r14;
  copy_ptr<ul_pusch_less_pwr_ctrl_ded_v1430_s>             ul_pusch_less_pwr_ctrl_ded_v1430;
  copy_ptr<srs_ul_periodic_cfg_ded_list_r14_l_>            srs_ul_periodic_cfg_ded_list_r14;
  copy_ptr<srs_ul_periodic_cfg_ded_up_pts_ext_list_r14_l_> srs_ul_periodic_cfg_ded_up_pts_ext_list_r14;
  copy_ptr<srs_ul_aperiodic_cfg_ded_list_r14_l_>           srs_ul_aperiodic_cfg_ded_list_r14;
  copy_ptr<srs_ul_cfg_ded_ap_up_pts_ext_list_r14_l_>       srs_ul_cfg_ded_ap_up_pts_ext_list_r14;
  copy_ptr<must_cfg_r14_c_>                                must_cfg_r14;
  copy_ptr<pusch_cfg_ded_scell_v1430_s>                    pusch_cfg_ded_v1430;
  copy_ptr<csi_rs_cfg_v1430_s>                             csi_rs_cfg_v1430;
  copy_ptr<csi_rs_cfg_zp_ap_list_r14_c>                    csi_rs_cfg_zp_ap_list_r14;
  copy_ptr<cqi_report_cfg_v1430_s>                         cqi_report_cfg_v1430;
  bool                                                     semi_open_loop_r14 = false;
  copy_ptr<pdsch_cfg_ded_scell_v1430_s>                    pdsch_cfg_ded_scell_v1430;
  // group 6
  copy_ptr<csi_rs_cfg_v1480_s> csi_rs_cfg_v1480;
  // group 7
  copy_ptr<phys_cfg_ded_stti_r15_c>       phys_cfg_ded_stti_r15;
  copy_ptr<pdsch_cfg_ded_v1530_s>         pdsch_cfg_ded_v1530;
  copy_ptr<cqi_report_cfg_v1530_s>        dummy;
  copy_ptr<cqi_report_cfg_scell_r15_s>    cqi_report_cfg_scell_r15;
  copy_ptr<cqi_short_cfg_scell_r15_c>     cqi_short_cfg_scell_r15;
  copy_ptr<csi_rs_cfg_v1530_s>            csi_rs_cfg_v1530;
  copy_ptr<ul_pwr_ctrl_ded_v1530_s>       ul_pwr_ctrl_ded_scell_v1530;
  copy_ptr<laa_scell_cfg_v1530_s>         laa_scell_cfg_v1530;
  copy_ptr<pusch_cfg_ded_scell_v1530_s>   pusch_cfg_ded_v1530;
  copy_ptr<semi_static_cfi_cfg_r15_c_>    semi_static_cfi_cfg_r15;
  copy_ptr<blind_pdsch_repeat_cfg_r15_c_> blind_pdsch_repeat_cfg_r15;
  // group 8
  copy_ptr<spucch_cfg_v1550_c> spucch_cfg_v1550;
  // group 9
  copy_ptr<setup_release_c<srs_ul_cfg_ded_add_r16_s> >  srs_ul_cfg_ded_add_r16;
  copy_ptr<setup_release_c<ul_pwr_ctrl_add_srs_r16_s> > ul_pwr_ctrl_add_srs_r16;
  copy_ptr<setup_release_c<srs_virtual_cell_id_r16_s> > srs_virtual_cell_id_r16;
  copy_ptr<setup_release_c<wideband_prg_r16_s> >        wideband_prg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const phys_cfg_ded_scell_r10_s& other) const;
  bool        operator!=(const phys_cfg_ded_scell_r10_s& other) const { return not(*this == other); }
};

// PhysicalConfigDedicatedSCell-v1370 ::= SEQUENCE
struct phys_cfg_ded_scell_v1370_s {
  struct pucch_scell_v1370_c_ {
    struct setup_s_ {
      bool                  pucch_cfg_ded_v1370_present = false;
      pucch_cfg_ded_v1370_s pucch_cfg_ded_v1370;
    };
    using types = setup_e;

    // choice methods
    pucch_scell_v1370_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const pucch_scell_v1370_c_& other) const;
    bool        operator!=(const pucch_scell_v1370_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "pucch-SCell-v1370");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "pucch-SCell-v1370");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  pucch_scell_v1370_c_ pucch_scell_v1370;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const phys_cfg_ded_scell_v1370_s& other) const;
  bool        operator!=(const phys_cfg_ded_scell_v1370_s& other) const { return not(*this == other); }
};

// PhysicalConfigDedicatedSCell-v1730 ::= SEQUENCE
struct phys_cfg_ded_scell_v1730_s {
  setup_release_c<cqi_report_periodic_scell_v1730_s> cqi_report_periodic_scell_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const phys_cfg_ded_scell_v1730_s& other) const;
  bool        operator!=(const phys_cfg_ded_scell_v1730_s& other) const { return not(*this == other); }
};

// AntennaInfoDedicated-v10i0 ::= SEQUENCE
struct ant_info_ded_v10i0_s {
  struct max_layers_mimo_r10_opts {
    enum options { two_layers, four_layers, eight_layers, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_layers_mimo_r10_opts> max_layers_mimo_r10_e_;

  // member variables
  bool                   max_layers_mimo_r10_present = false;
  max_layers_mimo_r10_e_ max_layers_mimo_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ant_info_ded_v10i0_s& other) const;
  bool        operator!=(const ant_info_ded_v10i0_s& other) const { return not(*this == other); }
};

// RadioResourceConfigDedicatedSCell-r10 ::= SEQUENCE
struct rr_cfg_ded_scell_r10_s {
  bool                     ext                            = false;
  bool                     phys_cfg_ded_scell_r10_present = false;
  phys_cfg_ded_scell_r10_s phys_cfg_ded_scell_r10;
  // ...
  // group 0
  copy_ptr<mac_main_cfg_scell_r11_s> mac_main_cfg_scell_r11;
  // group 1
  copy_ptr<naics_assist_info_r12_c> naics_info_r12;
  // group 2
  copy_ptr<neigh_cells_crs_info_r13_c> neigh_cells_crs_info_scell_r13;
  // group 3
  copy_ptr<phys_cfg_ded_scell_v1370_s> phys_cfg_ded_scell_v1370;
  // group 4
  bool                                 crs_intf_mitig_enabled_r15_present = false;
  bool                                 crs_intf_mitig_enabled_r15         = false;
  copy_ptr<neigh_cells_crs_info_r15_c> neigh_cells_crs_info_r15;
  copy_ptr<sps_cfg_v1530_s>            sps_cfg_v1530;
  // group 5
  copy_ptr<phys_cfg_ded_scell_v1730_s> phys_cfg_ded_scell_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rr_cfg_ded_scell_r10_s& other) const;
  bool        operator!=(const rr_cfg_ded_scell_r10_s& other) const { return not(*this == other); }
};

// SCellToAddModExt-r13 ::= SEQUENCE
struct scell_to_add_mod_ext_r13_s {
  struct cell_identif_r13_s_ {
    uint16_t pci_r13             = 0;
    uint32_t dl_carrier_freq_r13 = 0;
  };

  // member variables
  bool                      cell_identif_r13_present        = false;
  bool                      rr_cfg_common_scell_r13_present = false;
  bool                      rr_cfg_ded_scell_r13_present    = false;
  bool                      ant_info_ded_scell_r13_present  = false;
  uint8_t                   scell_idx_r13                   = 1;
  cell_identif_r13_s_       cell_identif_r13;
  rr_cfg_common_scell_r10_s rr_cfg_common_scell_r13;
  rr_cfg_ded_scell_r10_s    rr_cfg_ded_scell_r13;
  ant_info_ded_v10i0_s      ant_info_ded_scell_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellToAddModListExt-r13 ::= SEQUENCE (SIZE (1..31)) OF SCellToAddModExt-r13
using scell_to_add_mod_list_ext_r13_l = dyn_array<scell_to_add_mod_ext_r13_s>;

// PUCCH-ConfigDedicated-v13c0 ::= SEQUENCE
struct pucch_cfg_ded_v13c0_s {
  struct ch_sel_v13c0_s_ {
    struct n1_pucch_an_cs_v13c0_c_ {
      struct setup_s_ {
        using n1_pucch_an_cs_list_p1_v13c0_l_ = bounded_array<uint16_t, 4>;

        // member variables
        n1_pucch_an_cs_list_p1_v13c0_l_ n1_pucch_an_cs_list_p1_v13c0;
      };
      using types = setup_e;

      // choice methods
      n1_pucch_an_cs_v13c0_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      setup_s_& setup()
      {
        assert_choice_type(types::setup, type_, "n1PUCCH-AN-CS-v13c0");
        return c;
      }
      const setup_s_& setup() const
      {
        assert_choice_type(types::setup, type_, "n1PUCCH-AN-CS-v13c0");
        return c;
      }
      void      set_release();
      setup_s_& set_setup();

    private:
      types    type_;
      setup_s_ c;
    };

    // member variables
    n1_pucch_an_cs_v13c0_c_ n1_pucch_an_cs_v13c0;
  };

  // member variables
  ch_sel_v13c0_s_ ch_sel_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-ToAddModSCG-r12 ::= SEQUENCE
struct drb_to_add_mod_scg_r12_s {
  struct drb_type_r12_c_ {
    struct scg_r12_s_ {
      bool       eps_bearer_id_r12_present = false;
      bool       pdcp_cfg_r12_present      = false;
      uint8_t    eps_bearer_id_r12         = 0;
      pdcp_cfg_s pdcp_cfg_r12;
    };
    struct types_opts {
      enum options { split_r12, scg_r12, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    drb_type_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scg_r12_s_& scg_r12()
    {
      assert_choice_type(types::scg_r12, type_, "drb-Type-r12");
      return c;
    }
    const scg_r12_s_& scg_r12() const
    {
      assert_choice_type(types::scg_r12, type_, "drb-Type-r12");
      return c;
    }
    void        set_split_r12();
    scg_r12_s_& set_scg_r12();

  private:
    types      type_;
    scg_r12_s_ c;
  };

  // member variables
  bool            ext                       = false;
  bool            drb_type_r12_present      = false;
  bool            rlc_cfg_scg_r12_present   = false;
  bool            rlc_cfg_v1250_present     = false;
  bool            lc_ch_id_scg_r12_present  = false;
  bool            lc_ch_cfg_scg_r12_present = false;
  uint8_t         drb_id_r12                = 1;
  drb_type_r12_c_ drb_type_r12;
  rlc_cfg_c       rlc_cfg_scg_r12;
  rlc_cfg_v1250_s rlc_cfg_v1250;
  uint8_t         lc_ch_id_scg_r12 = 3;
  lc_ch_cfg_s     lc_ch_cfg_scg_r12;
  // ...
  // group 0
  copy_ptr<rlc_cfg_v1430_c> rlc_cfg_v1430;
  // group 1
  bool                           lc_ch_id_scg_r15_present = false;
  uint8_t                        lc_ch_id_scg_r15         = 32;
  copy_ptr<rlc_cfg_v1530_c>      rlc_cfg_v1530;
  copy_ptr<rlc_bearer_cfg_r15_c> rlc_bearer_cfg_secondary_r15;
  // group 2
  copy_ptr<rlc_cfg_v1510_s> rlc_cfg_v1560;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhysicalConfigDedicated-v13c0 ::= SEQUENCE
struct phys_cfg_ded_v13c0_s {
  pucch_cfg_ded_v13c0_s pucch_cfg_ded_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigCommonSCell-v10l0 ::= SEQUENCE
struct rr_cfg_common_scell_v10l0_s {
  struct ul_cfg_v10l0_s_ {
    uint16_t add_spec_emission_scell_v10l0 = 33;
  };

  // member variables
  ul_cfg_v10l0_s_ ul_cfg_v10l0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-ToAddModListSCG-r12 ::= SEQUENCE (SIZE (1..11)) OF DRB-ToAddModSCG-r12
using drb_to_add_mod_list_scg_r12_l = dyn_array<drb_to_add_mod_scg_r12_s>;

// DRB-ToAddModListSCG-r15 ::= SEQUENCE (SIZE (1..15)) OF DRB-ToAddModSCG-r12
using drb_to_add_mod_list_scg_r15_l = dyn_array<drb_to_add_mod_scg_r12_s>;

// RLF-TimersAndConstantsSCG-r12 ::= CHOICE
struct rlf_timers_and_consts_scg_r12_c {
  struct setup_s_ {
    struct t313_r12_opts {
      enum options { ms0, ms50, ms100, ms200, ms500, ms1000, ms2000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t313_r12_opts> t313_r12_e_;
    struct n313_r12_opts {
      enum options { n1, n2, n3, n4, n6, n8, n10, n20, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<n313_r12_opts> n313_r12_e_;
    struct n314_r12_opts {
      enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<n314_r12_opts> n314_r12_e_;

    // member variables
    bool        ext = false;
    t313_r12_e_ t313_r12;
    n313_r12_e_ n313_r12;
    n314_r12_e_ n314_r12;
    // ...
  };
  using types = setup_e;

  // choice methods
  rlf_timers_and_consts_scg_r12_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "RLF-TimersAndConstantsSCG-r12");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "RLF-TimersAndConstantsSCG-r12");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SCellToAddMod-r10 ::= SEQUENCE
struct scell_to_add_mod_r10_s {
  struct cell_identif_r10_s_ {
    uint16_t pci_r10             = 0;
    uint32_t dl_carrier_freq_r10 = 0;
  };
  struct scell_state_r15_opts {
    enum options { activ, dormant, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<scell_state_r15_opts> scell_state_r15_e_;

  // member variables
  bool                      ext                             = false;
  bool                      cell_identif_r10_present        = false;
  bool                      rr_cfg_common_scell_r10_present = false;
  bool                      rr_cfg_ded_scell_r10_present    = false;
  uint8_t                   scell_idx_r10                   = 1;
  cell_identif_r10_s_       cell_identif_r10;
  rr_cfg_common_scell_r10_s rr_cfg_common_scell_r10;
  rr_cfg_ded_scell_r10_s    rr_cfg_ded_scell_r10;
  // ...
  // group 0
  bool     dl_carrier_freq_v1090_present = false;
  uint32_t dl_carrier_freq_v1090         = 65536;
  // group 1
  copy_ptr<ant_info_ded_v10i0_s> ant_info_ded_scell_v10i0;
  // group 2
  bool    srs_switch_from_serv_cell_idx_r14_present = false;
  uint8_t srs_switch_from_serv_cell_idx_r14         = 0;
  // group 3
  bool               scell_state_r15_present = false;
  scell_state_r15_e_ scell_state_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const scell_to_add_mod_r10_s& other) const;
  bool        operator!=(const scell_to_add_mod_r10_s& other) const { return not(*this == other); }
};

// SCellToAddModExt-v1370 ::= SEQUENCE
struct scell_to_add_mod_ext_v1370_s {
  bool                        rr_cfg_common_scell_v1370_present = false;
  rr_cfg_common_scell_v10l0_s rr_cfg_common_scell_v1370;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellToAddModExt-v1430 ::= SEQUENCE
struct scell_to_add_mod_ext_v1430_s {
  struct scell_state_r15_opts {
    enum options { activ, dormant, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<scell_state_r15_opts> scell_state_r15_e_;

  // member variables
  bool    ext                                       = false;
  bool    srs_switch_from_serv_cell_idx_r14_present = false;
  uint8_t srs_switch_from_serv_cell_idx_r14         = 0;
  // ...
  // group 0
  bool               scell_state_r15_present = false;
  scell_state_r15_e_ scell_state_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRB-ToReleaseList-r15 ::= SEQUENCE (SIZE (1..2)) OF INTEGER (1..2)
using srb_to_release_list_r15_l = bounded_array<uint8_t, 2>;

// RadioResourceConfigDedicatedSCG-r12 ::= SEQUENCE
struct rr_cfg_ded_scg_r12_s {
  bool                            ext                                   = false;
  bool                            drb_to_add_mod_list_scg_r12_present   = false;
  bool                            mac_main_cfg_scg_r12_present          = false;
  bool                            rlf_timers_and_consts_scg_r12_present = false;
  drb_to_add_mod_list_scg_r12_l   drb_to_add_mod_list_scg_r12;
  mac_main_cfg_s                  mac_main_cfg_scg_r12;
  rlf_timers_and_consts_scg_r12_c rlf_timers_and_consts_scg_r12;
  // ...
  // group 0
  copy_ptr<drb_to_add_mod_list_scg_r15_l> drb_to_add_mod_list_scg_r15;
  // group 1
  copy_ptr<srb_to_add_mod_list_l>     srb_to_add_mod_list_scg_r15;
  copy_ptr<srb_to_release_list_r15_l> srb_to_release_list_scg_r15;
  // group 2
  copy_ptr<drb_to_release_list_r15_l> drb_to_release_list_scg_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellToAddModList-r10 ::= SEQUENCE (SIZE (1..4)) OF SCellToAddMod-r10
using scell_to_add_mod_list_r10_l = dyn_array<scell_to_add_mod_r10_s>;

// SCellToAddModListExt-v1370 ::= SEQUENCE (SIZE (1..31)) OF SCellToAddModExt-v1370
using scell_to_add_mod_list_ext_v1370_l = dyn_array<scell_to_add_mod_ext_v1370_s>;

// SCellToAddModListExt-v1430 ::= SEQUENCE (SIZE (1..31)) OF SCellToAddModExt-v1430
using scell_to_add_mod_list_ext_v1430_l = dyn_array<scell_to_add_mod_ext_v1430_s>;

// PhysicalConfigDedicatedSCell-v13c0 ::= SEQUENCE
struct phys_cfg_ded_scell_v13c0_s {
  struct pucch_scell_v13c0_c_ {
    struct setup_s_ {
      pucch_cfg_ded_v13c0_s pucch_cfg_ded_v13c0;
    };
    using types = setup_e;

    // choice methods
    pucch_scell_v13c0_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "pucch-SCell-v13c0");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "pucch-SCell-v13c0");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  pucch_scell_v13c0_c_ pucch_scell_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigDedicatedSCell-v13c0 ::= SEQUENCE
struct rr_cfg_ded_scell_v13c0_s {
  phys_cfg_ded_scell_v13c0_s phys_cfg_ded_scell_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellToAddMod-v13c0 ::= SEQUENCE
struct scell_to_add_mod_v13c0_s {
  bool                     rr_cfg_ded_scell_v13c0_present = false;
  rr_cfg_ded_scell_v13c0_s rr_cfg_ded_scell_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellToAddModList-v13c0 ::= SEQUENCE (SIZE (1..4)) OF SCellToAddMod-v13c0
using scell_to_add_mod_list_v13c0_l = dyn_array<scell_to_add_mod_v13c0_s>;

// SCellToAddModListExt-v13c0 ::= SEQUENCE (SIZE (1..31)) OF SCellToAddMod-v13c0
using scell_to_add_mod_list_ext_v13c0_l = dyn_array<scell_to_add_mod_v13c0_s>;

// SCellToAddMod-v10l0 ::= SEQUENCE
struct scell_to_add_mod_v10l0_s {
  bool                        rr_cfg_common_scell_v10l0_present = false;
  rr_cfg_common_scell_v10l0_s rr_cfg_common_scell_v10l0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigDedicated-v13c0 ::= SEQUENCE
struct rr_cfg_ded_v13c0_s {
  phys_cfg_ded_v13c0_s phys_cfg_ded_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellToAddModList-v10l0 ::= SEQUENCE (SIZE (1..4)) OF SCellToAddMod-v10l0
using scell_to_add_mod_list_v10l0_l = dyn_array<scell_to_add_mod_v10l0_s>;

// Cell-ToAddMod-r12 ::= SEQUENCE
struct cell_to_add_mod_r12_s {
  struct cell_identif_r12_s_ {
    uint16_t pci_r12             = 0;
    uint32_t dl_carrier_freq_r12 = 0;
  };
  struct meas_result_cell_to_add_r12_s_ {
    uint8_t rsrp_result_r12 = 0;
    uint8_t rsrq_result_r12 = 0;
  };
  struct meas_result_cell_to_add_v1310_s_ {
    uint8_t rs_sinr_result_r13 = 0;
  };

  // member variables
  bool                           ext                                 = false;
  bool                           cell_identif_r12_present            = false;
  bool                           meas_result_cell_to_add_r12_present = false;
  uint8_t                        scell_idx_r12                       = 1;
  cell_identif_r12_s_            cell_identif_r12;
  meas_result_cell_to_add_r12_s_ meas_result_cell_to_add_r12;
  // ...
  // group 0
  bool                                       scell_idx_r13_present = false;
  uint8_t                                    scell_idx_r13         = 1;
  copy_ptr<meas_result_cell_to_add_v1310_s_> meas_result_cell_to_add_v1310;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellToAddModListSCG-Ext-r13 ::= SEQUENCE (SIZE (1..31)) OF Cell-ToAddMod-r12
using scell_to_add_mod_list_scg_ext_r13_l = dyn_array<cell_to_add_mod_r12_s>;

// SCellToAddModListSCG-r12 ::= SEQUENCE (SIZE (1..4)) OF Cell-ToAddMod-r12
using scell_to_add_mod_list_scg_r12_l = dyn_array<cell_to_add_mod_r12_s>;

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_RRCFG_H
