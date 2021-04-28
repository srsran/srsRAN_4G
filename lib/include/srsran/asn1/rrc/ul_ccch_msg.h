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

/*******************************************************************************
 *
 *                     3GPP TS ASN1 RRC v15.11.0 (2020-09)
 *
 ******************************************************************************/

#ifndef SRSASN1_RRC_ULCCCH_MSG_H
#define SRSASN1_RRC_ULCCCH_MSG_H

#include "paging.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// EstablishmentCause ::= ENUMERATED
struct establishment_cause_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    delay_tolerant_access_v1020,
    mo_voice_call_v1280,
    spare1,
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<establishment_cause_opts> establishment_cause_e;

// EstablishmentCause-5GC ::= ENUMERATED
struct establishment_cause_minus5_gc_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    mo_voice_call,
    spare2,
    spare1,
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<establishment_cause_minus5_gc_opts> establishment_cause_minus5_gc_e;

// InitialUE-Identity ::= CHOICE
struct init_ue_id_c {
  struct types_opts {
    enum options { s_tmsi, random_value, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  init_ue_id_c() = default;
  init_ue_id_c(const init_ue_id_c& other);
  init_ue_id_c& operator=(const init_ue_id_c& other);
  ~init_ue_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  s_tmsi_s& s_tmsi()
  {
    assert_choice_type(types::s_tmsi, type_, "InitialUE-Identity");
    return c.get<s_tmsi_s>();
  }
  fixed_bitstring<40>& random_value()
  {
    assert_choice_type(types::random_value, type_, "InitialUE-Identity");
    return c.get<fixed_bitstring<40> >();
  }
  const s_tmsi_s& s_tmsi() const
  {
    assert_choice_type(types::s_tmsi, type_, "InitialUE-Identity");
    return c.get<s_tmsi_s>();
  }
  const fixed_bitstring<40>& random_value() const
  {
    assert_choice_type(types::random_value, type_, "InitialUE-Identity");
    return c.get<fixed_bitstring<40> >();
  }
  s_tmsi_s&            set_s_tmsi();
  fixed_bitstring<40>& set_random_value();

private:
  types                                          type_;
  choice_buffer_t<fixed_bitstring<40>, s_tmsi_s> c;

  void destroy_();
};

// InitialUE-Identity-5GC ::= CHOICE
struct init_ue_id_minus5_gc_c {
  struct types_opts {
    enum options { ng_minus5_g_s_tmsi_part1, random_value, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  init_ue_id_minus5_gc_c() = default;
  init_ue_id_minus5_gc_c(const init_ue_id_minus5_gc_c& other);
  init_ue_id_minus5_gc_c& operator=(const init_ue_id_minus5_gc_c& other);
  ~init_ue_id_minus5_gc_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<40>& ng_minus5_g_s_tmsi_part1()
  {
    assert_choice_type(types::ng_minus5_g_s_tmsi_part1, type_, "InitialUE-Identity-5GC");
    return c.get<fixed_bitstring<40> >();
  }
  fixed_bitstring<40>& random_value()
  {
    assert_choice_type(types::random_value, type_, "InitialUE-Identity-5GC");
    return c.get<fixed_bitstring<40> >();
  }
  const fixed_bitstring<40>& ng_minus5_g_s_tmsi_part1() const
  {
    assert_choice_type(types::ng_minus5_g_s_tmsi_part1, type_, "InitialUE-Identity-5GC");
    return c.get<fixed_bitstring<40> >();
  }
  const fixed_bitstring<40>& random_value() const
  {
    assert_choice_type(types::random_value, type_, "InitialUE-Identity-5GC");
    return c.get<fixed_bitstring<40> >();
  }
  fixed_bitstring<40>& set_ng_minus5_g_s_tmsi_part1();
  fixed_bitstring<40>& set_random_value();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<40> > c;

  void destroy_();
};

// RRCEarlyDataRequest-v1590-IEs ::= SEQUENCE
struct rrc_early_data_request_v1590_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReestabUE-Identity ::= SEQUENCE
struct reestab_ue_id_s {
  fixed_bitstring<16> c_rnti;
  uint16_t            pci = 0;
  fixed_bitstring<16> short_mac_i;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReestablishmentCause ::= ENUMERATED
struct reest_cause_opts {
  enum options { recfg_fail, ho_fail, other_fail, spare1, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<reest_cause_opts> reest_cause_e;

// ResumeCause ::= ENUMERATED
struct resume_cause_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    delay_tolerant_access_v1020,
    mo_voice_call_v1280,
    spare1,
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<resume_cause_opts> resume_cause_e;

// ResumeCause-r15 ::= ENUMERATED
struct resume_cause_r15_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    rna_upd,
    mo_voice_call,
    spare1,
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<resume_cause_r15_opts> resume_cause_r15_e;

// RRCConnectionReestablishmentRequest-r8-IEs ::= SEQUENCE
struct rrc_conn_reest_request_r8_ies_s {
  reestab_ue_id_s    ue_id;
  reest_cause_e      reest_cause;
  fixed_bitstring<2> spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRequest-5GC-r15-IEs ::= SEQUENCE
struct rrc_conn_request_minus5_gc_r15_ies_s {
  init_ue_id_minus5_gc_c          ue_id;
  establishment_cause_minus5_gc_e establishment_cause;
  fixed_bitstring<1>              spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRequest-r8-IEs ::= SEQUENCE
struct rrc_conn_request_r8_ies_s {
  init_ue_id_c          ue_id;
  establishment_cause_e establishment_cause;
  fixed_bitstring<1>    spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeRequest-5GC-r15-IEs ::= SEQUENCE
struct rrc_conn_resume_request_minus5_gc_r15_ies_s {
  struct resume_id_r15_c_ {
    struct types_opts {
      enum options { full_i_rnti_r15, short_i_rnti_r15, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    resume_id_r15_c_() = default;
    resume_id_r15_c_(const resume_id_r15_c_& other);
    resume_id_r15_c_& operator=(const resume_id_r15_c_& other);
    ~resume_id_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<40>& full_i_rnti_r15()
    {
      assert_choice_type(types::full_i_rnti_r15, type_, "resumeIdentity-r15");
      return c.get<fixed_bitstring<40> >();
    }
    fixed_bitstring<24>& short_i_rnti_r15()
    {
      assert_choice_type(types::short_i_rnti_r15, type_, "resumeIdentity-r15");
      return c.get<fixed_bitstring<24> >();
    }
    const fixed_bitstring<40>& full_i_rnti_r15() const
    {
      assert_choice_type(types::full_i_rnti_r15, type_, "resumeIdentity-r15");
      return c.get<fixed_bitstring<40> >();
    }
    const fixed_bitstring<24>& short_i_rnti_r15() const
    {
      assert_choice_type(types::short_i_rnti_r15, type_, "resumeIdentity-r15");
      return c.get<fixed_bitstring<24> >();
    }
    fixed_bitstring<40>& set_full_i_rnti_r15();
    fixed_bitstring<24>& set_short_i_rnti_r15();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<40> > c;

    void destroy_();
  };

  // member variables
  resume_id_r15_c_    resume_id_r15;
  fixed_bitstring<16> short_resume_mac_i_r15;
  resume_cause_r15_e  resume_cause_r15;
  fixed_bitstring<1>  spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeRequest-r13-IEs ::= SEQUENCE
struct rrc_conn_resume_request_r13_ies_s {
  struct resume_id_r13_c_ {
    struct types_opts {
      enum options { resume_id_r13, truncated_resume_id_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    resume_id_r13_c_() = default;
    resume_id_r13_c_(const resume_id_r13_c_& other);
    resume_id_r13_c_& operator=(const resume_id_r13_c_& other);
    ~resume_id_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<40>& resume_id_r13()
    {
      assert_choice_type(types::resume_id_r13, type_, "resumeIdentity-r13");
      return c.get<fixed_bitstring<40> >();
    }
    fixed_bitstring<24>& truncated_resume_id_r13()
    {
      assert_choice_type(types::truncated_resume_id_r13, type_, "resumeIdentity-r13");
      return c.get<fixed_bitstring<24> >();
    }
    const fixed_bitstring<40>& resume_id_r13() const
    {
      assert_choice_type(types::resume_id_r13, type_, "resumeIdentity-r13");
      return c.get<fixed_bitstring<40> >();
    }
    const fixed_bitstring<24>& truncated_resume_id_r13() const
    {
      assert_choice_type(types::truncated_resume_id_r13, type_, "resumeIdentity-r13");
      return c.get<fixed_bitstring<24> >();
    }
    fixed_bitstring<40>& set_resume_id_r13();
    fixed_bitstring<24>& set_truncated_resume_id_r13();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<40> > c;

    void destroy_();
  };

  // member variables
  resume_id_r13_c_    resume_id_r13;
  fixed_bitstring<16> short_resume_mac_i_r13;
  resume_cause_e      resume_cause_r13;
  fixed_bitstring<1>  spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCEarlyDataRequest-r15-IEs ::= SEQUENCE
struct rrc_early_data_request_r15_ies_s {
  struct establishment_cause_r15_opts {
    enum options { mo_data, delay_tolerant_access, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<establishment_cause_r15_opts> establishment_cause_r15_e_;

  // member variables
  bool                               non_crit_ext_present = false;
  s_tmsi_s                           s_tmsi_r15;
  establishment_cause_r15_e_         establishment_cause_r15;
  dyn_octstring                      ded_info_nas_r15;
  rrc_early_data_request_v1590_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentRequest ::= SEQUENCE
struct rrc_conn_reest_request_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_conn_reest_request_r8, crit_exts_future, nulltype } value;

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
    rrc_conn_reest_request_r8_ies_s& rrc_conn_reest_request_r8()
    {
      assert_choice_type(types::rrc_conn_reest_request_r8, type_, "criticalExtensions");
      return c;
    }
    const rrc_conn_reest_request_r8_ies_s& rrc_conn_reest_request_r8() const
    {
      assert_choice_type(types::rrc_conn_reest_request_r8, type_, "criticalExtensions");
      return c;
    }
    rrc_conn_reest_request_r8_ies_s& set_rrc_conn_reest_request_r8();
    void                             set_crit_exts_future();

  private:
    types                           type_;
    rrc_conn_reest_request_r8_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRequest ::= SEQUENCE
struct rrc_conn_request_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_conn_request_r8, rrc_conn_request_r15, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_conn_request_r8_ies_s& rrc_conn_request_r8()
    {
      assert_choice_type(types::rrc_conn_request_r8, type_, "criticalExtensions");
      return c.get<rrc_conn_request_r8_ies_s>();
    }
    rrc_conn_request_minus5_gc_r15_ies_s& rrc_conn_request_r15()
    {
      assert_choice_type(types::rrc_conn_request_r15, type_, "criticalExtensions");
      return c.get<rrc_conn_request_minus5_gc_r15_ies_s>();
    }
    const rrc_conn_request_r8_ies_s& rrc_conn_request_r8() const
    {
      assert_choice_type(types::rrc_conn_request_r8, type_, "criticalExtensions");
      return c.get<rrc_conn_request_r8_ies_s>();
    }
    const rrc_conn_request_minus5_gc_r15_ies_s& rrc_conn_request_r15() const
    {
      assert_choice_type(types::rrc_conn_request_r15, type_, "criticalExtensions");
      return c.get<rrc_conn_request_minus5_gc_r15_ies_s>();
    }
    rrc_conn_request_r8_ies_s&            set_rrc_conn_request_r8();
    rrc_conn_request_minus5_gc_r15_ies_s& set_rrc_conn_request_r15();

  private:
    types                                                                            type_;
    choice_buffer_t<rrc_conn_request_minus5_gc_r15_ies_s, rrc_conn_request_r8_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeRequest-r13 ::= SEQUENCE
struct rrc_conn_resume_request_r13_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_conn_resume_request_r13, rrc_conn_resume_request_r15, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_conn_resume_request_r13_ies_s& rrc_conn_resume_request_r13()
    {
      assert_choice_type(types::rrc_conn_resume_request_r13, type_, "criticalExtensions");
      return c.get<rrc_conn_resume_request_r13_ies_s>();
    }
    rrc_conn_resume_request_minus5_gc_r15_ies_s& rrc_conn_resume_request_r15()
    {
      assert_choice_type(types::rrc_conn_resume_request_r15, type_, "criticalExtensions");
      return c.get<rrc_conn_resume_request_minus5_gc_r15_ies_s>();
    }
    const rrc_conn_resume_request_r13_ies_s& rrc_conn_resume_request_r13() const
    {
      assert_choice_type(types::rrc_conn_resume_request_r13, type_, "criticalExtensions");
      return c.get<rrc_conn_resume_request_r13_ies_s>();
    }
    const rrc_conn_resume_request_minus5_gc_r15_ies_s& rrc_conn_resume_request_r15() const
    {
      assert_choice_type(types::rrc_conn_resume_request_r15, type_, "criticalExtensions");
      return c.get<rrc_conn_resume_request_minus5_gc_r15_ies_s>();
    }
    rrc_conn_resume_request_r13_ies_s&           set_rrc_conn_resume_request_r13();
    rrc_conn_resume_request_minus5_gc_r15_ies_s& set_rrc_conn_resume_request_r15();

  private:
    types                                                                                           type_;
    choice_buffer_t<rrc_conn_resume_request_minus5_gc_r15_ies_s, rrc_conn_resume_request_r13_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCEarlyDataRequest-r15 ::= SEQUENCE
struct rrc_early_data_request_r15_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_early_data_request_r15, crit_exts_future, nulltype } value;

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
    rrc_early_data_request_r15_ies_s& rrc_early_data_request_r15()
    {
      assert_choice_type(types::rrc_early_data_request_r15, type_, "criticalExtensions");
      return c;
    }
    const rrc_early_data_request_r15_ies_s& rrc_early_data_request_r15() const
    {
      assert_choice_type(types::rrc_early_data_request_r15, type_, "criticalExtensions");
      return c;
    }
    rrc_early_data_request_r15_ies_s& set_rrc_early_data_request_r15();
    void                              set_crit_exts_future();

  private:
    types                            type_;
    rrc_early_data_request_r15_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-CCCH-MessageType ::= CHOICE
struct ul_ccch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { rrc_conn_reest_request, rrc_conn_request, nulltype } value;

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
    rrc_conn_reest_request_s& rrc_conn_reest_request()
    {
      assert_choice_type(types::rrc_conn_reest_request, type_, "c1");
      return c.get<rrc_conn_reest_request_s>();
    }
    rrc_conn_request_s& rrc_conn_request()
    {
      assert_choice_type(types::rrc_conn_request, type_, "c1");
      return c.get<rrc_conn_request_s>();
    }
    const rrc_conn_reest_request_s& rrc_conn_reest_request() const
    {
      assert_choice_type(types::rrc_conn_reest_request, type_, "c1");
      return c.get<rrc_conn_reest_request_s>();
    }
    const rrc_conn_request_s& rrc_conn_request() const
    {
      assert_choice_type(types::rrc_conn_request, type_, "c1");
      return c.get<rrc_conn_request_s>();
    }
    rrc_conn_reest_request_s& set_rrc_conn_reest_request();
    rrc_conn_request_s&       set_rrc_conn_request();

  private:
    types                                                         type_;
    choice_buffer_t<rrc_conn_reest_request_s, rrc_conn_request_s> c;

    void destroy_();
  };
  struct msg_class_ext_c_ {
    struct c2_c_ {
      struct types_opts {
        enum options { rrc_conn_resume_request_r13, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      types       type() const { return types::rrc_conn_resume_request_r13; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      rrc_conn_resume_request_r13_s&       rrc_conn_resume_request_r13() { return c; }
      const rrc_conn_resume_request_r13_s& rrc_conn_resume_request_r13() const { return c; }

    private:
      rrc_conn_resume_request_r13_s c;
    };
    struct msg_class_ext_future_r13_c_ {
      struct c3_c_ {
        struct types_opts {
          enum options { rrc_early_data_request_r15, spare3, spare2, spare1, nulltype } value;

          const char* to_string() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        c3_c_() = default;
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        rrc_early_data_request_r15_s& rrc_early_data_request_r15()
        {
          assert_choice_type(types::rrc_early_data_request_r15, type_, "c3");
          return c;
        }
        const rrc_early_data_request_r15_s& rrc_early_data_request_r15() const
        {
          assert_choice_type(types::rrc_early_data_request_r15, type_, "c3");
          return c;
        }
        rrc_early_data_request_r15_s& set_rrc_early_data_request_r15();
        void                          set_spare3();
        void                          set_spare2();
        void                          set_spare1();

      private:
        types                        type_;
        rrc_early_data_request_r15_s c;
      };
      struct types_opts {
        enum options { c3, msg_class_ext_future_r15, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      msg_class_ext_future_r13_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      c3_c_& c3()
      {
        assert_choice_type(types::c3, type_, "messageClassExtensionFuture-r13");
        return c;
      }
      const c3_c_& c3() const
      {
        assert_choice_type(types::c3, type_, "messageClassExtensionFuture-r13");
        return c;
      }
      c3_c_& set_c3();
      void   set_msg_class_ext_future_r15();

    private:
      types type_;
      c3_c_ c;
    };
    struct types_opts {
      enum options { c2, msg_class_ext_future_r13, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    msg_class_ext_c_() = default;
    msg_class_ext_c_(const msg_class_ext_c_& other);
    msg_class_ext_c_& operator=(const msg_class_ext_c_& other);
    ~msg_class_ext_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c2_c_& c2()
    {
      assert_choice_type(types::c2, type_, "messageClassExtension");
      return c.get<c2_c_>();
    }
    msg_class_ext_future_r13_c_& msg_class_ext_future_r13()
    {
      assert_choice_type(types::msg_class_ext_future_r13, type_, "messageClassExtension");
      return c.get<msg_class_ext_future_r13_c_>();
    }
    const c2_c_& c2() const
    {
      assert_choice_type(types::c2, type_, "messageClassExtension");
      return c.get<c2_c_>();
    }
    const msg_class_ext_future_r13_c_& msg_class_ext_future_r13() const
    {
      assert_choice_type(types::msg_class_ext_future_r13, type_, "messageClassExtension");
      return c.get<msg_class_ext_future_r13_c_>();
    }
    c2_c_&                       set_c2();
    msg_class_ext_future_r13_c_& set_msg_class_ext_future_r13();

  private:
    types                                               type_;
    choice_buffer_t<c2_c_, msg_class_ext_future_r13_c_> c;

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
  ul_ccch_msg_type_c() = default;
  ul_ccch_msg_type_c(const ul_ccch_msg_type_c& other);
  ul_ccch_msg_type_c& operator=(const ul_ccch_msg_type_c& other);
  ~ul_ccch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "UL-CCCH-MessageType");
    return c.get<c1_c_>();
  }
  msg_class_ext_c_& msg_class_ext()
  {
    assert_choice_type(types::msg_class_ext, type_, "UL-CCCH-MessageType");
    return c.get<msg_class_ext_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "UL-CCCH-MessageType");
    return c.get<c1_c_>();
  }
  const msg_class_ext_c_& msg_class_ext() const
  {
    assert_choice_type(types::msg_class_ext, type_, "UL-CCCH-MessageType");
    return c.get<msg_class_ext_c_>();
  }
  c1_c_&            set_c1();
  msg_class_ext_c_& set_msg_class_ext();

private:
  types                                    type_;
  choice_buffer_t<c1_c_, msg_class_ext_c_> c;

  void destroy_();
};

// UL-CCCH-Message ::= SEQUENCE
struct ul_ccch_msg_s {
  ul_ccch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_ULCCCH_MSG_H
