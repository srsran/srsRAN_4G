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

#ifndef SRSENB_PARSER_H
#define SRSENB_PARSER_H

#include "srslte/asn1/asn1_utils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <libconfig.h++>
#include <list>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <typeinfo>

namespace srsenb {

using namespace libconfig;

class parser
{
public:
  class field_itf
  {
  public:
    virtual ~field_itf()                     = default;
    virtual int         parse(Setting& root) = 0;
    virtual const char* get_name()           = 0;
  };

  template <class T>
  class field_enum_str : public field_itf
  {
  public:
    field_enum_str(const char*  name_,
                   T*           store_ptr_,
                   const char** value_str_,
                   uint32_t     nof_items_,
                   bool*        enabled_value_ = NULL)
    {
      name          = name_;
      store_ptr     = store_ptr_;
      value_str     = value_str_;
      nof_items     = nof_items_;
      enabled_value = enabled_value_;
    }

    const char* get_name() { return name; }

    int parse(Setting& root)
    {
      std::string val;
      if (root.exists(name)) {

        if (enabled_value) {
          *enabled_value = true;
        }

        if (root.lookupValue(name, val)) {
          bool found = false;
          // find value
          for (uint32_t i = 0; i < nof_items && !found; i++) {
            if (!strcmp(value_str[i], val.c_str())) {
              *store_ptr = (T)i;
              found      = true;
            }
          }
          if (!found) {
            fprintf(stderr, "Invalid option: %s for field %s\n", val.c_str(), name);
            fprintf(stderr, "Valid options:  %s", value_str[0]);
            for (uint32_t i = 1; i < nof_items; i++) {
              fprintf(stderr, ", %s", value_str[i]);
            }
            fprintf(stderr, "\n");
            return -1;
          }
          return 0;
        } else {
          return -1;
        }
      } else {
        if (enabled_value) {
          *enabled_value = false;
          return 0;
        } else {
          return -1;
        }
      }
    }

  private:
    const char*  name;
    T*           store_ptr;
    const char** value_str;
    uint32_t     nof_items;
    bool*        enabled_value;
  };

  template <class T, class S>
  class field_enum_num : public field_itf
  {
  public:
    field_enum_num(const char* name_,
                   T*          store_ptr_,
                   const S*    value_str_,
                   uint32_t    nof_items_,
                   bool*       enabled_value_ = NULL)
    {
      name          = name_;
      store_ptr     = store_ptr_;
      value_str     = value_str_;
      nof_items     = nof_items_;
      enabled_value = enabled_value_;
    }

    const char* get_name() { return name; }

    int parse(Setting& root)
    {
      S val;
      if (root.exists(name)) {

        if (enabled_value) {
          *enabled_value = true;
        }
        if (parser::lookupValue(root, name, &val)) {
          bool found = false;
          // find value
          for (uint32_t i = 0; i < nof_items && !found; i++) {
            if (value_str[i] == val) {
              *store_ptr = (T)i;
              found      = true;
            }
          }
          if (!found) {
            std::cout << "Invalid option: " << +val << " for field " << name << std::endl;
            std::cout << "Valid options:  ";
            for (uint32_t i = 0; i < nof_items; i++) {
              std::cout << +value_str[i] << ", ";
            }
            std::cout << std::endl;
            return -1;
          }
          return 0;
        } else {
          return -1;
        }
      } else {
        if (enabled_value) {
          *enabled_value = false;
          return 0;
        } else {
          return -1;
        }
      }
    }

  private:
    const char* name;
    T*          store_ptr;
    const S*    value_str;
    uint32_t    nof_items;
    bool*       enabled_value;
  };

  template <class T>
  class field : public field_itf
  {
  public:
    field(const char* name_, T* store_ptr_, bool* enabled_value_ = NULL)
    {
      name          = name_;
      store_ptr     = store_ptr_;
      enabled_value = enabled_value_;
    }

    const char* get_name() { return name; }

    int parse(Setting& root)
    {
      if (root.exists(name)) {
        if (enabled_value) {
          *enabled_value = true;
        }
        if (!parser::lookupValue(root, name, store_ptr)) {
          return -1;
        } else {
          return 0;
        }
      } else {
        if (enabled_value) {
          *enabled_value = false;
          return 0;
        } else {
          return -1;
        }
      }
    }

  private:
    const char* name;
    T*          store_ptr;
    bool*       enabled_value;
  };

  class section
  {
  public:
    section(std::string name);
    ~section();
    void set_optional(bool* enabled_value);
    void add_subsection(section* s);
    void add_field(field_itf* f);
    int  parse(Setting& root);

  private:
    std::string           name;
    bool*                 enabled_value;
    std::list<section*>   sub_sections;
    std::list<field_itf*> fields;
  };

  parser(std::string filename);
  int  parse();
  void add_section(section* s);

  static int parse_section(std::string filename, section* s);

  static bool lookupValue(Setting& root, const char* name, std::string* val) { return root.lookupValue(name, *val); }
  static bool lookupValue(Setting& root, const char* name, uint8_t* val)
  {
    uint32_t t;
    bool     r = root.lookupValue(name, t);
    *val       = (uint8_t)t;
    return r;
  }
  static bool lookupValue(Setting& root, const char* name, uint16_t* val)
  {
    uint32_t t;
    bool     r = root.lookupValue(name, t);
    *val       = (uint16_t)t;
    return r;
  }
  static bool lookupValue(Setting& root, const char* name, uint32_t* val)
  {
    uint32_t t;
    bool     r = root.lookupValue(name, t);
    *val       = t;
    return r;
  }
  static bool lookupValue(Setting& root, const char* name, int8_t* val)
  {
    int32_t t;
    bool    r = root.lookupValue(name, t);
    *val      = (int8_t)t;
    return r;
  }
  static bool lookupValue(Setting& root, const char* name, int16_t* val)
  {
    int32_t t;
    bool    r = root.lookupValue(name, t);
    *val      = (int16_t)t;
    return r;
  }
  static bool lookupValue(Setting& root, const char* name, int32_t* val)
  {
    int32_t t;
    bool    r = root.lookupValue(name, t);
    *val      = t;
    return r;
  }
  static bool lookupValue(Setting& root, const char* name, double* val)
  {
    double t;
    bool   r = root.lookupValue(name, t);
    *val     = t;
    return r;
  }
  static bool lookupValue(Setting& root, const char* name, float* val)
  {
    double t;
    bool   r = root.lookupValue(name, t);
    *val     = t;
    return r;
  }
  static bool lookupValue(Setting& root, const char* name, bool* val)
  {
    bool t;
    bool r = root.lookupValue(name, t);
    *val   = t;
    return r;
  }

private:
  std::list<section*> sections;
  std::string         filename;
};

template <typename T>
struct DefaultFieldParser {
  int operator()(T& obj, Setting& root) const
  {
    obj = (T)root;
    return 0;
  }
};
// specialization for uint16_t bc Setting does not have overloaded uint16_t operator
template <>
struct DefaultFieldParser<uint16_t> {
  int operator()(uint16_t& obj, Setting& root) const
  {
    obj = (uint32_t)root;
    return 0;
  }
};

template <typename T, typename Parser = DefaultFieldParser<T> >
int parse_opt_field(T&            obj,
                    Setting&      root,
                    const char*   field_name,
                    const Parser& field_parser  = {},
                    bool*         presence_flag = nullptr)
{
  bool exists = root.exists(field_name);
  if (presence_flag != nullptr) {
    *presence_flag = exists;
  }
  return exists ? field_parser(obj, root[field_name]) : 0;
}

//! Parse field if it exists. If not, default to "default_val"
template <typename T, typename Parser = DefaultFieldParser<T> >
int parse_default_field(T&            obj,
                        Setting&      root,
                        const char*   fieldname,
                        const T&      default_val,
                        const Parser& field_parser  = {},
                        bool*         presence_flag = nullptr)
{
  bool opt_flag;
  int  ret = parse_opt_field(obj, root, fieldname, field_parser, &opt_flag);
  if (not opt_flag) {
    obj = default_val;
  }
  if (presence_flag != nullptr) {
    *presence_flag = opt_flag;
  }
  return ret;
}

//! Parse required field and return error if field isn't specified
template <typename T, typename Parser = DefaultFieldParser<T> >
int parse_required_field(T& obj, Setting& root, const char* field_name, const Parser& field_parser = {})
{
  if (not root.exists(field_name)) {
    fprintf(stderr, "PARSER ERROR: Field \"%s\" doesn't exist.\n", field_name);
    return -1;
  }
  return field_parser(obj, root[field_name]);
}

template <typename T>
int parse_bounded_number(T& number, Setting& fieldroot, T num_min, T num_max)
{
  number = (T)fieldroot;
  if (number < num_min or number > num_max) {
    std::cout << "Parser Warning: Value of " << fieldroot.getName() << " must be within bound [" << num_min << ", "
              << num_max << "]\n";
    number = (number > num_max) ? num_max : num_min;
  }
  return 0;
}

namespace asn1_parsers {

template <class EnumType>
bool nowhitespace_string_to_enum(EnumType& e, const std::string& s)
{
  std::string s_nows = s;
  std::transform(s_nows.begin(), s_nows.end(), s_nows.begin(), ::tolower);
  s_nows.erase(std::remove(s_nows.begin(), s_nows.end(), ' '), s_nows.end());
  s_nows.erase(std::remove(s_nows.begin(), s_nows.end(), '-'), s_nows.end());
  for (uint32_t i = 0; i < EnumType::nof_types; ++i) {
    e                   = (typename EnumType::options)i;
    std::string s_nows2 = e.to_string();
    std::transform(s_nows2.begin(), s_nows2.end(), s_nows2.begin(), ::tolower);
    s_nows2.erase(std::remove(s_nows2.begin(), s_nows2.end(), ' '), s_nows2.end());
    s_nows2.erase(std::remove(s_nows2.begin(), s_nows2.end(), '-'), s_nows2.end());
    if (s_nows2 == s_nows) {
      return true;
    }
  }
  return false;
}

template <class EnumType>
int str_to_enum(EnumType& enum_val, Setting& root)
{
  std::string val   = root.c_str();
  bool        found = nowhitespace_string_to_enum(enum_val, val);
  if (not found) {
    fprintf(stderr, "PARSER ERROR: Invalid option: \"%s\" for asn1 enum type\n", val.c_str());
    fprintf(stderr, "Valid options:  \"%s\"", EnumType((typename EnumType::options)0).to_string().c_str());
    for (uint32_t i = 1; i < EnumType::nof_types; i++) {
      fprintf(stderr, ", \"%s\"", EnumType((typename EnumType::options)i).to_string().c_str());
    }
    fprintf(stderr, "\n");
  }
  return found ? 0 : -1;
}

template <typename EnumType>
int opt_str_to_enum(EnumType& enum_val, bool& presence_flag, Setting& root, const char* name)
{
  return parse_opt_field(enum_val, root, name, str_to_enum<EnumType>, &presence_flag);
}

template <typename EnumType>
int number_to_enum(EnumType& enum_val, Setting& root)
{
  if (root.isNumber()) {
    typename EnumType::number_type val = 0;
    if (root.getType() == Setting::TypeInt64) {
      val = (long int)root;
    } else if (root.getType() == Setting::TypeInt) {
      val = (int)root;
    }
    bool found = asn1::number_to_enum(enum_val, val);
    if (not found) {
      std::ostringstream ss;
      ss << val;
      fprintf(stderr, "Invalid option: %s for enum field \"%s\"\n", ss.str().c_str(), root.getName());
      ss.str("");
      ss << EnumType((typename EnumType::options)0).to_number();
      fprintf(stderr, "Valid options:  %s", ss.str().c_str());
      for (uint32_t i = 1; i < EnumType::nof_types; i++) {
        ss.str("");
        ss << EnumType((typename EnumType::options)i).to_number();
        fprintf(stderr, ", %s", ss.str().c_str());
      }
      fprintf(stderr, "\n");
    }
    return found ? 0 : -1;
  } else {
    std::string str_val = root.c_str();
    fprintf(stderr, "Expected a number for enum field %s but received a string %s\n", root.getName(), str_val.c_str());
  }
  return -1;
}

template <typename EnumType>
int opt_number_to_enum(EnumType& enum_val, bool& presence_flag, Setting& root, const char* name)
{
  return parse_opt_field(enum_val, root, name, number_to_enum<EnumType>, &presence_flag);
}

} // namespace asn1_parsers

} // namespace srsenb
#endif // PARSER_H
