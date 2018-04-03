/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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

#include <stdarg.h>
#include <string>
#include <list>
#include <stdlib.h>
#include <stdint.h>
#include <typeinfo>
#include <libconfig.h++>
#include <string.h>
#include <iostream>
#include <fstream>


namespace srsenb {
  
using namespace libconfig;

class parser
{
public: 
  
  class field_itf 
  {
  public: 
    virtual ~field_itf(){} 
    virtual int parse(Setting &root) = 0; 
    virtual const char* get_name() = 0;
  };
  
  template<class T>
  class field_enum_str : public field_itf
  {
  public:
    field_enum_str(const char* name_, T *store_ptr_, const char (*value_str_)[20], uint32_t nof_items_, bool *enabled_value_ = NULL)
    {
      name      = name_; 
      store_ptr = store_ptr_; 
      value_str = value_str_; 
      nof_items = nof_items_; 
      enabled_value = enabled_value_; 
    }
    
    const char* get_name() {
      return name; 
    }

    int parse(Setting &root)
    {  
      std::string val;       
      if (root.exists(name)) {
        
        if (enabled_value) {
          *enabled_value = true; 
        }
        
        if (root.lookupValue(name, val)) {  
          bool found = false; 
          // find value 
          for (uint32_t i=0;i<nof_items && !found;i++) {
            if (!strcmp(value_str[i], val.c_str())) {
              *store_ptr = (T) i;
              found = true; 
            }
          }
          if (!found) {
            fprintf(stderr, "Invalid option: %s for field %s\n", val.c_str(), name);
            fprintf(stderr, "Valid options:  %s", value_str[0]);
            for (uint32_t i=1;i<nof_items;i++) {
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
    const char*    name; 
    T              *store_ptr;
    const char     (*value_str)[20];
    uint32_t       nof_items; 
    bool          *enabled_value; 
  };

  template<class T, class S>
  class field_enum_num : public field_itf
  {
  public:
    field_enum_num(const char* name_, T *store_ptr_, const S *value_str_, uint32_t nof_items_, bool *enabled_value_ = NULL)
    {
      name      = name_; 
      store_ptr = store_ptr_; 
      value_str = value_str_; 
      nof_items = nof_items_; 
      enabled_value = enabled_value_; 
    }
    
    const char* get_name() {
      return name; 
    }

    int parse(Setting &root)
    {  
      S val; 
      if (root.exists(name)) {
        
        if (enabled_value) {
          *enabled_value = true; 
        }
        if (parser::lookupValue(root, name, &val)) {
          bool found = false; 
          // find value 
          for (uint32_t i=0;i<nof_items && !found;i++) {
            if (value_str[i] == val) {
              *store_ptr = (T) i;
              found = true; 
            }
          }
          if (!found) {
            std::cout << "Invalid option: " << +val << " for field " << name << std::endl;          
            std::cout << "Valid options:  "; 
            for (uint32_t i=0;i<nof_items;i++) {
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
    const char*    name; 
    T              *store_ptr;
    const S        *value_str;
    uint32_t       nof_items; 
    bool          *enabled_value; 
  };

  
  template<class T>
  class field : public field_itf
  {
  public:
    field(const char* name_, T *store_ptr_, bool *enabled_value_ = NULL)
    {
      name          = name_; 
      store_ptr     = store_ptr_; 
      enabled_value = enabled_value_; 
    }
    
    const char* get_name() {
      return name; 
    }

    int parse(Setting &root)
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
    const char*    name; 
    T              *store_ptr;
    bool *enabled_value; 
  };
  
  class section
  {
  public: 
    section(std::string name);
    ~section();
    void set_optional(bool *enabled_value);
    void add_subsection(section *s);
    void add_field(field_itf *f);
    int parse(Setting &root);
  private: 
    std::string name; 
    bool *enabled_value; 
    std::list<section*> sub_sections; 
    std::list<field_itf*> fields; 
  }; 
  
  
  parser(std::string filename);
  int  parse();
  void add_section(section *s);
  
  static int parse_section(std::string filename, section *s);  
  
  static bool lookupValue(Setting &root, const char *name, std::string *val) {
    return root.lookupValue(name, *val);
  }
  static bool lookupValue(Setting &root, const char *name, uint8_t *val) {
    uint32_t t; 
    bool r = root.lookupValue(name, t);
    *val = (uint8_t) t; 
    return r; 
  }
  static bool lookupValue(Setting &root, const char *name, uint16_t *val) {
    uint32_t t; 
    bool r = root.lookupValue(name, t);
    *val = (uint16_t) t; 
    return r; 
  }
  static bool lookupValue(Setting &root, const char *name, uint32_t *val) {
    uint32_t t; 
    bool r = root.lookupValue(name, t);
    *val = t;
    return r; 
  }
  static bool lookupValue(Setting &root, const char *name, int8_t *val) {
    int32_t t; 
    bool r = root.lookupValue(name, t);
    *val = (int8_t) t; 
    return r; 
  }
  static bool lookupValue(Setting &root, const char *name, int16_t *val) {
    int32_t t; 
    bool r = root.lookupValue(name, t);
    *val = (int16_t) t; 
    return r; 
  }
  static bool lookupValue(Setting &root, const char *name, int32_t *val) {
    int32_t t; 
    bool r = root.lookupValue(name, t);
    *val = t;
    return r; 
  }
  static bool lookupValue(Setting &root, const char *name, double *val) {
    double t; 
    bool r = root.lookupValue(name, t);
    *val = t;
    return r; 
  }
  static bool lookupValue(Setting &root, const char *name, bool *val) {
    bool t; 
    bool r = root.lookupValue(name, t);
    *val = t;
    return r; 
  }

  
private: 
  std::list<section*> sections;   
  std::string filename;
};
}
#endif // PARSER_H
