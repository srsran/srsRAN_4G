/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSASN1_RRC_SECURITY_H
#define SRSASN1_RRC_SECURITY_H

#include "common.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// SecurityAlgorithmConfig ::= SEQUENCE
struct security_algorithm_cfg_s {
  struct integrity_prot_algorithm_opts {
    enum options { eia0_v920, eia1, eia2, eia3_v1130, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<integrity_prot_algorithm_opts, true> integrity_prot_algorithm_e_;

  // member variables
  ciphering_algorithm_r12_e   ciphering_algorithm;
  integrity_prot_algorithm_e_ integrity_prot_algorithm;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityConfigHO-v1530 ::= SEQUENCE
struct security_cfg_ho_v1530_s {
  struct handov_type_v1530_c_ {
    struct intra5_gc_r15_s_ {
      bool                     security_algorithm_cfg_r15_present = false;
      bool                     nas_container_r15_present          = false;
      security_algorithm_cfg_s security_algorithm_cfg_r15;
      bool                     key_change_ind_r15          = false;
      uint8_t                  next_hop_chaining_count_r15 = 0;
      dyn_octstring            nas_container_r15;
    };
    struct fivegc_to_epc_r15_s_ {
      security_algorithm_cfg_s security_algorithm_cfg_r15;
      uint8_t                  next_hop_chaining_count_r15 = 0;
    };
    struct epc_to5_gc_r15_s_ {
      security_algorithm_cfg_s security_algorithm_cfg_r15;
      dyn_octstring            nas_container_r15;
    };
    struct types_opts {
      enum options { intra5_gc_r15, fivegc_to_epc_r15, epc_to5_gc_r15, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    handov_type_v1530_c_() = default;
    handov_type_v1530_c_(const handov_type_v1530_c_& other);
    handov_type_v1530_c_& operator=(const handov_type_v1530_c_& other);
    ~handov_type_v1530_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    intra5_gc_r15_s_& intra5_gc_r15()
    {
      assert_choice_type(types::intra5_gc_r15, type_, "handoverType-v1530");
      return c.get<intra5_gc_r15_s_>();
    }
    fivegc_to_epc_r15_s_& fivegc_to_epc_r15()
    {
      assert_choice_type(types::fivegc_to_epc_r15, type_, "handoverType-v1530");
      return c.get<fivegc_to_epc_r15_s_>();
    }
    epc_to5_gc_r15_s_& epc_to5_gc_r15()
    {
      assert_choice_type(types::epc_to5_gc_r15, type_, "handoverType-v1530");
      return c.get<epc_to5_gc_r15_s_>();
    }
    const intra5_gc_r15_s_& intra5_gc_r15() const
    {
      assert_choice_type(types::intra5_gc_r15, type_, "handoverType-v1530");
      return c.get<intra5_gc_r15_s_>();
    }
    const fivegc_to_epc_r15_s_& fivegc_to_epc_r15() const
    {
      assert_choice_type(types::fivegc_to_epc_r15, type_, "handoverType-v1530");
      return c.get<fivegc_to_epc_r15_s_>();
    }
    const epc_to5_gc_r15_s_& epc_to5_gc_r15() const
    {
      assert_choice_type(types::epc_to5_gc_r15, type_, "handoverType-v1530");
      return c.get<epc_to5_gc_r15_s_>();
    }
    intra5_gc_r15_s_&     set_intra5_gc_r15();
    fivegc_to_epc_r15_s_& set_fivegc_to_epc_r15();
    epc_to5_gc_r15_s_&    set_epc_to5_gc_r15();

  private:
    types                                                                      type_;
    choice_buffer_t<epc_to5_gc_r15_s_, fivegc_to_epc_r15_s_, intra5_gc_r15_s_> c;

    void destroy_();
  };

  // member variables
  bool                 ext = false;
  handov_type_v1530_c_ handov_type_v1530;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityConfigHO ::= SEQUENCE
struct security_cfg_ho_s {
  struct handov_type_c_ {
    struct intra_lte_s_ {
      bool                     security_algorithm_cfg_present = false;
      security_algorithm_cfg_s security_algorithm_cfg;
      bool                     key_change_ind          = false;
      uint8_t                  next_hop_chaining_count = 0;
    };
    struct inter_rat_s_ {
      security_algorithm_cfg_s security_algorithm_cfg;
      fixed_octstring<6>       nas_security_param_to_eutra;
    };
    struct types_opts {
      enum options { intra_lte, inter_rat, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    handov_type_c_() = default;
    handov_type_c_(const handov_type_c_& other);
    handov_type_c_& operator=(const handov_type_c_& other);
    ~handov_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    intra_lte_s_& intra_lte()
    {
      assert_choice_type(types::intra_lte, type_, "handoverType");
      return c.get<intra_lte_s_>();
    }
    inter_rat_s_& inter_rat()
    {
      assert_choice_type(types::inter_rat, type_, "handoverType");
      return c.get<inter_rat_s_>();
    }
    const intra_lte_s_& intra_lte() const
    {
      assert_choice_type(types::intra_lte, type_, "handoverType");
      return c.get<intra_lte_s_>();
    }
    const inter_rat_s_& inter_rat() const
    {
      assert_choice_type(types::inter_rat, type_, "handoverType");
      return c.get<inter_rat_s_>();
    }
    intra_lte_s_& set_intra_lte();
    inter_rat_s_& set_inter_rat();

  private:
    types                                       type_;
    choice_buffer_t<inter_rat_s_, intra_lte_s_> c;

    void destroy_();
  };

  // member variables
  bool           ext = false;
  handov_type_c_ handov_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityConfigSMC ::= SEQUENCE
struct security_cfg_smc_s {
  bool                     ext = false;
  security_algorithm_cfg_s security_algorithm_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeCommand-v8a0-IEs ::= SEQUENCE
struct security_mode_cmd_v8a0_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeCommand-r8-IEs ::= SEQUENCE
struct security_mode_cmd_r8_ies_s {
  bool                         non_crit_ext_present = false;
  security_cfg_smc_s           security_cfg_smc;
  security_mode_cmd_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeCommand ::= SEQUENCE
struct security_mode_cmd_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { security_mode_cmd_r8, spare3, spare2, spare1, nulltype } value;

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
      security_mode_cmd_r8_ies_s& security_mode_cmd_r8()
      {
        assert_choice_type(types::security_mode_cmd_r8, type_, "c1");
        return c;
      }
      const security_mode_cmd_r8_ies_s& security_mode_cmd_r8() const
      {
        assert_choice_type(types::security_mode_cmd_r8, type_, "c1");
        return c;
      }
      security_mode_cmd_r8_ies_s& set_security_mode_cmd_r8();
      void                        set_spare3();
      void                        set_spare2();
      void                        set_spare1();

    private:
      types                      type_;
      security_mode_cmd_r8_ies_s c;
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

// SecurityModeComplete-v8a0-IEs ::= SEQUENCE
struct security_mode_complete_v8a0_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeFailure-v8a0-IEs ::= SEQUENCE
struct security_mode_fail_v8a0_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeComplete-r8-IEs ::= SEQUENCE
struct security_mode_complete_r8_ies_s {
  bool                              non_crit_ext_present = false;
  security_mode_complete_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeFailure-r8-IEs ::= SEQUENCE
struct security_mode_fail_r8_ies_s {
  bool                          non_crit_ext_present = false;
  security_mode_fail_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeComplete ::= SEQUENCE
struct security_mode_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { security_mode_complete_r8, crit_exts_future, nulltype } value;

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
    security_mode_complete_r8_ies_s& security_mode_complete_r8()
    {
      assert_choice_type(types::security_mode_complete_r8, type_, "criticalExtensions");
      return c;
    }
    const security_mode_complete_r8_ies_s& security_mode_complete_r8() const
    {
      assert_choice_type(types::security_mode_complete_r8, type_, "criticalExtensions");
      return c;
    }
    security_mode_complete_r8_ies_s& set_security_mode_complete_r8();
    void                             set_crit_exts_future();

  private:
    types                           type_;
    security_mode_complete_r8_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeFailure ::= SEQUENCE
struct security_mode_fail_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { security_mode_fail_r8, crit_exts_future, nulltype } value;

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
    security_mode_fail_r8_ies_s& security_mode_fail_r8()
    {
      assert_choice_type(types::security_mode_fail_r8, type_, "criticalExtensions");
      return c;
    }
    const security_mode_fail_r8_ies_s& security_mode_fail_r8() const
    {
      assert_choice_type(types::security_mode_fail_r8, type_, "criticalExtensions");
      return c;
    }
    security_mode_fail_r8_ies_s& set_security_mode_fail_r8();
    void                         set_crit_exts_future();

  private:
    types                       type_;
    security_mode_fail_r8_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarShortMAC-Input ::= SEQUENCE
struct var_short_mac_input_s {
  fixed_bitstring<28> cell_id;
  uint16_t            pci = 0;
  fixed_bitstring<16> c_rnti;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_SECURITY_H
