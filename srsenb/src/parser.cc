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

#include "srsenb/hdr/parser.h"
#include <iostream>

namespace srsenb {

using namespace libconfig;

int parser::parse_section(std::string filename, parser::section *s) 
{
  parser p(filename);
  p.add_section(s);
  return p.parse();
}
    
parser::parser(std::string filename_)
{
  filename = filename_; 
}

void parser::add_section(parser::section* s)
{
  sections.push_back(s);
}

int parser::parse()
{
  // open file 
  Config cfg;
  
  try
  {
    cfg.readFile(filename.c_str());
  }
  catch(const FileIOException &fioex)
  {
    std::cerr << "I/O error while reading file: " << filename << std::endl;
    return(-1);
  }
  catch(const ParseException &pex)
  {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;
    return(-1);
  }
  
  for (std::list<section*>::iterator ci = sections.begin(); ci != sections.end(); ++ci) {
    section *s = *ci;
    if (s->parse(cfg.getRoot())) {
      return -1; 
    }
  }
  return 0; 
}

parser::section::section(std::string name_)
{
  name = name_; 
  enabled_value = NULL; 
}

// Fields are allocated dynamically, free all fields added to the section
parser::section::~section() 
{
  for (std::list<field_itf*>::iterator ci = fields.begin(); ci != fields.end(); ++ci) {
    delete *ci;      
  }
}

void parser::section::add_field(field_itf* f)
{
  fields.push_back(f);
}

void parser::section::add_subsection(parser::section* s)
{
  sub_sections.push_back(s);
}

void parser::section::set_optional(bool* enabled_value_)
{
  enabled_value = enabled_value_; 
}

int parser::section::parse(Setting &root)
{
  try {
    for (std::list<field_itf*>::iterator ci = fields.begin(); ci != fields.end(); ++ci) {
      field_itf *f = *ci;
      if (f->parse(root[name.c_str()])) {
        fprintf(stderr, "Error parsing field %s in section %s\n", f->get_name(), name.c_str());
        return -1; 
      }
    }
    for (std::list<section*>::iterator ci = sub_sections.begin(); ci != sub_sections.end(); ++ci) {
      section *s = *ci;
      if (s->parse(root[name.c_str()])) {
        fprintf(stderr, "Error parsing section %s\n", name.c_str());
        return -1; 
      }
    }
    if (enabled_value) {
      *enabled_value = true; 
    }
  } catch(const SettingNotFoundException ex) {
    if (enabled_value) {
      *enabled_value = false;       
      return 0; 
    } else {
      std::cerr << "Error section " << name.c_str() << " not found." << std::endl;
      return -1;
    }
  }
  return 0; 
}



  
  
}
