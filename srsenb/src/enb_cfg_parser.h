/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef ENB_CFG_PARSER_SIB1_H
#define ENB_CFG_PARSER_SIB1_H

#include "srsenb/hdr/parser.h"
#include <iostream>
#include <libconfig.h++>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "srsenb/hdr/stack/rrc/rrc.h"
#include "srslte/asn1/asn1_utils.h"

namespace srsenb {

using namespace libconfig;

struct all_args_t;
struct phy_cfg_t;

bool sib_is_present(const asn1::rrc::sched_info_list_l& l, asn1::rrc::sib_type_e sib_num);

// enb.conf parsing
namespace enb_conf_sections {

int parse_cell_cfg(all_args_t* args_, srslte_cell_t* cell);
int parse_cfg_files(all_args_t* args_, rrc_cfg_t* rrc_cfg_, phy_cfg_t* phy_cfg_);
int set_derived_args(all_args_t* args_, rrc_cfg_t* rrc_cfg_, phy_cfg_t* phy_cfg_, const srslte_cell_t& cell_cfg_);

} // namespace enb_conf_sections

// sib.conf parsing
namespace sib_sections {

int parse_sib1(std::string filename, asn1::rrc::sib_type1_s* data);
int parse_sib2(std::string filename, asn1::rrc::sib_type2_s* data);
int parse_sib3(std::string filename, asn1::rrc::sib_type3_s* data);
int parse_sib4(std::string filename, asn1::rrc::sib_type4_s* data);
int parse_sib7(std::string filename, asn1::rrc::sib_type7_s* data);
int parse_sib9(std::string filename, asn1::rrc::sib_type9_s* data);
int parse_sib13(std::string filename, asn1::rrc::sib_type13_r9_s* data);
int parse_sibs(all_args_t* args_, rrc_cfg_t* rrc_cfg_, srsenb::phy_cfg_t* phy_config_common);

} // namespace sib_sections

// drb.conf parsing
namespace drb_sections {

int parse_drb(all_args_t* args, rrc_cfg_t* rrc_cfg);
} // namespace drb_sections

// rr.conf parsing
namespace rr_sections {

int parse_rr(all_args_t* args_, rrc_cfg_t* rrc_cfg_);

// rrc_cnfg
class cell_list_section final : public parser::field_itf
{
public:
  explicit cell_list_section(all_args_t* all_args_, rrc_cfg_t* rrc_cfg_) : args(all_args_), rrc_cfg(rrc_cfg_) {}

  int parse(Setting& root) override;

  const char* get_name() override { return "meas_cell_list"; }

private:
  rrc_cfg_t*  rrc_cfg;
  all_args_t* args;
};

} // namespace rr_sections

class field_sched_info final : public parser::field_itf
{
public:
  explicit field_sched_info(asn1::rrc::sib_type1_s* data_) { data = data_; }
  int         parse(Setting& root) override;
  const char* get_name() override { return "sched_info"; }

private:
  asn1::rrc::sib_type1_s* data;
};

class field_intra_neigh_cell_list final : public parser::field_itf
{
public:
  explicit field_intra_neigh_cell_list(asn1::rrc::sib_type4_s* data_) { data = data_; }
  int         parse(Setting& root) override;
  const char* get_name() override { return "intra_neigh_cell_list"; }

private:
  asn1::rrc::sib_type4_s* data;
};

class field_intra_black_cell_list final : public parser::field_itf
{
public:
  explicit field_intra_black_cell_list(asn1::rrc::sib_type4_s* data_) { data = data_; }
  int         parse(Setting& root) override;
  const char* get_name() override { return "intra_black_cell_list"; }

private:
  asn1::rrc::sib_type4_s* data;
};

class field_carrier_freqs_info_list final : public parser::field_itf
{
public:
  explicit field_carrier_freqs_info_list(asn1::rrc::sib_type7_s* data_) { data = data_; }
  int         parse(Setting& root) override;
  const char* get_name() override { return "carrier_freqs_info_list"; }

private:
  asn1::rrc::sib_type7_s* data;
};

class field_sf_mapping : public parser::field_itf
{
public:
  field_sf_mapping(uint32_t* sf_mapping_, uint32_t* nof_subframes_)
  {
    sf_mapping    = sf_mapping_;
    nof_subframes = nof_subframes_;
  }
  int         parse(Setting& root) override;
  const char* get_name() override { return "sf_mapping"; }

private:
  uint32_t* sf_mapping;
  uint32_t* nof_subframes;
};

class field_qci final : public parser::field_itf
{
public:
  explicit field_qci(rrc_cfg_qci_t* cfg_) { cfg = cfg_; }
  const char* get_name() override { return "field_cqi"; }

  int parse(Setting& root) override;

private:
  rrc_cfg_qci_t* cfg;
};

// ASN1 parsers

class field_asn1 : public parser::field_itf
{
public:
  explicit field_asn1(const char* name_, bool* enabled_value_ = nullptr)
  {
    name          = name_;
    enabled_value = enabled_value_;
  }

  const char* get_name() override { return name; }

  int parse(Setting& root) override
  {
    if (root.exists(name)) {

      if (enabled_value) {
        *enabled_value = true;
      }
      return parse_value(root);
    } else {
      if (enabled_value) {
        *enabled_value = false;
        return 0;
      } else {
        return -1;
      }
    }
  }

  virtual int parse_value(Setting& root) = 0;

protected:
  const char* name;
  bool*       enabled_value;
};

template <class ListType>
class field_asn1_seqof_size : public field_asn1
{
  ListType* store_ptr;

public:
  field_asn1_seqof_size(const char* name_, ListType* store_ptr_, bool* enabled_value_ = nullptr) :
    field_asn1(name_, enabled_value_),
    store_ptr(store_ptr_)
  {
  }

  int parse_value(Setting& root) override
  {
    uint32_t size_val;
    if (root.lookupValue(name, size_val)) {
      store_ptr->resize(size_val);
      return 0;
    }
    return -1;
  }
};

template <class ListType>
field_asn1_seqof_size<ListType>*
make_asn1_seqof_size_parser(const char* name, ListType* store_ptr, bool* enabled = nullptr)
{
  return new field_asn1_seqof_size<ListType>(name, store_ptr, enabled);
}

template <class ChoiceType, class NumberType>
class field_asn1_choice_type_number : public field_asn1
{
  ChoiceType* store_ptr;

public:
  field_asn1_choice_type_number(const char* name_, ChoiceType* store_ptr_, bool* enabled_value_ = nullptr) :
    field_asn1(name_, enabled_value_),
    store_ptr(store_ptr_)
  {
  }

  int parse_value(Setting& root) override
  {
    NumberType val;
    if (root.lookupValue(name, val)) {
      typename ChoiceType::types type;
      bool                       found = asn1::number_to_enum(type, val);
      if (not found) {
        fprintf(stderr, "Invalid option: %s for field \"%s\"\n", val.c_str(), name);
        fprintf(stderr, "Valid options:  %d", ChoiceType::types(0).to_number());
        for (uint32_t i = 1; i < ChoiceType::types::nof_types; i++) {
          fprintf(stderr, ", %s", ChoiceType::types(i).to_number());
        }
        fprintf(stderr, "\n");
        return -1;
      }
      store_ptr->set(type);
      return 0;
    }
    return -1;
  }
};

template <class EnumType>
bool parse_enum_by_number(EnumType& enum_val, const char* name, Setting& root)
{
  typename EnumType::number_type val;
  if (parser::lookupValue(root, name, &val)) {
    bool found = asn1::number_to_enum(enum_val, val);
    if (not found) {
      std::ostringstream ss;
      ss << val;
      fprintf(stderr, "Invalid option: %s for field \"%s\"\n", ss.str().c_str(), name);
      ss.str("");
      ss << EnumType((typename EnumType::options)0).to_number();
      fprintf(stderr, "Valid options:  %s", ss.str().c_str());
      for (uint32_t i = 1; i < EnumType::nof_types; i++) {
        ss.str("");
        ss << EnumType((typename EnumType::options)i).to_number();
        fprintf(stderr, ", %s", ss.str().c_str());
      }
      fprintf(stderr, "\n");
      return false;
    }
    return true;
  } else {
    std::string str_val;
    if (parser::lookupValue(root, name, &str_val)) {
      fprintf(stderr, "Expected a number for field %s but received a string %s\n", name, str_val.c_str());
    }
  }
  return false;
}

template <class EnumType>
bool parse_enum_by_number_str(EnumType& enum_val, const char* name, Setting& root)
{
  std::string val;
  if (parser::lookupValue(root, name, &val)) {
    bool found = asn1::number_string_to_enum(enum_val, val);
    if (not found) {
      fprintf(stderr, "PARSER ERROR: Invalid option: \"%s\" for field \"%s\"\n", val.c_str(), name);
      fprintf(stderr, "Valid options:  \"%s\"", EnumType((typename EnumType::options)0).to_number_string().c_str());
      for (uint32_t i = 1; i < EnumType::nof_types; i++) {
        fprintf(stderr, ", \"%s\"", EnumType((typename EnumType::options)i).to_number_string().c_str());
      }
      fprintf(stderr, "\n");
      return false;
    }
    return true;
  }
  return false;
}

template <class EnumType>
bool parse_enum_by_str(EnumType& enum_val, const char* name, Setting& root)
{
  std::string val;
  if (root.lookupValue(name, val)) {
    bool found = asn1_parsers::nowhitespace_string_to_enum(enum_val, val);
    if (not found) {
      fprintf(stderr, "PARSER ERROR: Invalid option: \"%s\" for field \"%s\"\n", val.c_str(), name);
      fprintf(stderr, "Valid options:  \"%s\"", EnumType((typename EnumType::options)0).to_string().c_str());
      for (uint32_t i = 1; i < EnumType::nof_types; i++) {
        fprintf(stderr, ", \"%s\"", EnumType((typename EnumType::options)i).to_string().c_str());
      }
      fprintf(stderr, "\n");
      return false;
    }
    return true;
  }
  return false;
}

template <class EnumType>
class field_asn1_enum_number : public field_asn1
{
  EnumType* store_ptr;

public:
  field_asn1_enum_number(const char* name_, EnumType* store_ptr_, bool* enabled_value_ = nullptr) :
    field_asn1(name_, enabled_value_),
    store_ptr(store_ptr_)
  {
  }

  int parse_value(Setting& root) override
  {
    bool found = parse_enum_by_number(*store_ptr, name, root);
    return found ? 0 : -1;
  }
};

template <class EnumType>
field_asn1_enum_number<EnumType>*
make_asn1_enum_number_parser(const char* name, EnumType* store_ptr, bool* enabled = nullptr)
{
  return new field_asn1_enum_number<EnumType>(name, store_ptr, enabled);
}

template <class EnumType>
class field_asn1_enum_str : public field_asn1
{
  EnumType* store_ptr;

public:
  field_asn1_enum_str(const char* name_, EnumType* store_ptr_, bool* enabled_value_ = nullptr) :
    field_asn1(name_, enabled_value_),
    store_ptr(store_ptr_)
  {
  }

  int parse_value(Setting& root) override
  {
    bool found = parse_enum_by_str(*store_ptr, name, root);
    return found ? 0 : -1;
  }
};

template <class EnumType>
field_asn1_enum_str<EnumType>* make_asn1_enum_str_parser(const char* name, EnumType* store_ptr, bool* enabled = nullptr)
{
  return new field_asn1_enum_str<EnumType>(name, store_ptr, enabled);
}

template <class EnumType>
class field_asn1_enum_number_str : public field_asn1
{
  EnumType* store_ptr;

public:
  field_asn1_enum_number_str(const char* name_, EnumType* store_ptr_, bool* enabled_value_ = nullptr) :
    field_asn1(name_, enabled_value_),
    store_ptr(store_ptr_)
  {
  }

  int parse_value(Setting& root) override
  {
    bool found = parse_enum_by_number_str(*store_ptr, name, root);
    return found ? 0 : -1;
  }
};

template <class EnumType>
field_asn1_enum_number_str<EnumType>*
make_asn1_enum_number_str_parser(const char* name, EnumType* store_ptr, bool* enabled = nullptr)
{
  return new field_asn1_enum_number_str<EnumType>(name, store_ptr, enabled);
}

template <class ChoiceType>
class field_asn1_choice_str : public field_asn1
{
public:
  typedef bool (*func_ptr)(ChoiceType*, const char*, Setting&);
  field_asn1_choice_str(const char* name_,
                        const char* choicetypename_,
                        func_ptr    f_,
                        ChoiceType* store_ptr_,
                        bool*       enabled_value_ = nullptr) :
    field_asn1(name_, enabled_value_),
    store_ptr(store_ptr_),
    choicetypename(choicetypename_),
    f(f_)
  {
  }

  int parse_value(Setting& root) override
  {
    typename ChoiceType::types type;
    bool                       found = parse_enum_by_str(type, choicetypename, root);
    if (not found) {
      return -1;
    }
    store_ptr->set(type);
    if (f(store_ptr, name, root)) {
      return 0;
    }
    return -1;
  }

private:
  ChoiceType* store_ptr;
  const char* choicetypename;
  func_ptr    f;
};

template <class ChoiceType, class FuncOper>
field_asn1_choice_str<ChoiceType>* make_asn1_choice_str_parser(const char* name,
                                                               const char* choicetypename,
                                                               ChoiceType* store_ptr,
                                                               FuncOper    f,
                                                               bool*       enabled = nullptr)
{
  return new field_asn1_choice_str<ChoiceType>(name, choicetypename, f, store_ptr, enabled);
}

template <class ChoiceType>
class field_asn1_choice_number : public field_asn1
{
public:
  typedef bool (*func_ptr)(ChoiceType*, const char*, Setting&);
  field_asn1_choice_number(const char* name_,
                           const char* choicetypename_,
                           func_ptr    f_,
                           ChoiceType* store_ptr_,
                           bool*       enabled_value_ = nullptr) :
    field_asn1(name_, enabled_value_),
    store_ptr(store_ptr_),
    choicetypename(choicetypename_),
    f(f_)
  {
  }

  int parse_value(Setting& root) override
  {
    typename ChoiceType::types type;
    bool                       found = parse_enum_by_number(type, choicetypename, root);
    if (not found) {
      return -1;
    }
    store_ptr->set(type);
    if (f(store_ptr, name, root)) {
      return 0;
    }
    return -1;
  }

private:
  ChoiceType* store_ptr;
  const char* choicetypename;
  func_ptr    f;
};

template <class BitString, class NumType>
class field_asn1_bitstring_number : public field_asn1
{
  BitString* store_ptr;

public:
  field_asn1_bitstring_number(const char* name_, BitString* store_ptr_, bool* enabled_value_ = nullptr) :
    field_asn1(name_, enabled_value_),
    store_ptr(store_ptr_)
  {
  }

  int parse_value(Setting& root) override
  {
    NumType val;
    if (parser::lookupValue(root, name, &val)) {
      store_ptr->from_number(val);
      return 0;
    } else {
      std::string str_val;
      if (parser::lookupValue(root, name, &str_val)) {
        fprintf(
            stderr, "PARSER ERROR: Expected number for field %s but got the string \"%s\"\n", name, str_val.c_str());
      }
    }
    return -1;
  }
};

template <class BitString>
field_asn1_bitstring_number<BitString, uint32_t>* make_asn1_bitstring_number_parser(const char* name,
                                                                                    BitString*  store_ptr)
{
  return new field_asn1_bitstring_number<BitString, uint32_t>(name, store_ptr);
}

class phr_cnfg_parser : public parser::field_itf
{
public:
  explicit phr_cnfg_parser(asn1::rrc::mac_main_cfg_s::phr_cfg_c_* phr_cfg_) { phr_cfg = phr_cfg_; }
  int         parse(Setting& root) override;
  const char* get_name() override { return "phr_cnfg"; }

private:
  asn1::rrc::mac_main_cfg_s::phr_cfg_c_* phr_cfg;
};

class mbsfn_sf_cfg_list_parser : public parser::field_itf
{
public:
  mbsfn_sf_cfg_list_parser(asn1::rrc::mbsfn_sf_cfg_list_l* mbsfn_list_, bool* enabled_) :
    mbsfn_list(mbsfn_list_),
    enabled(enabled_)
  {
  }
  int         parse(Setting& root) override;
  const char* get_name() override { return "mbsfnSubframeConfigList"; }

private:
  asn1::rrc::mbsfn_sf_cfg_list_l* mbsfn_list;
  bool*                           enabled;
};

class mbsfn_area_info_list_parser final : public parser::field_itf
{
public:
  mbsfn_area_info_list_parser(asn1::rrc::mbsfn_area_info_list_r9_l* mbsfn_list_, bool* enabled_) :
    mbsfn_list(mbsfn_list_),
    enabled(enabled_)
  {
  }
  int         parse(Setting& root) override;
  const char* get_name() override { return "mbsfn_area_info_list"; }

private:
  asn1::rrc::mbsfn_area_info_list_r9_l* mbsfn_list;
  bool*                                 enabled;
};
} // namespace srsenb

#endif
