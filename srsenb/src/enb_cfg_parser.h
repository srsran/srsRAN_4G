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

#ifndef ENB_CFG_PARSER_SIB1_H
#define ENB_CFG_PARSER_SIB1_H

#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <libconfig.h++>
#include <string.h>
#include <iostream>
#include "srsenb/hdr/parser.h"

#include "srsenb/hdr/upper/rrc.h"
#include "srslte/asn1/liblte_rrc.h"

namespace srsenb {
  
using namespace libconfig;

class field_sched_info : public parser::field_itf
{
public:
  field_sched_info(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *data_) { data = data_; }
  ~field_sched_info() {}
  int parse(Setting &root);    
  const char* get_name() {
    return "sched_info"; 
  }

private: 
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *data; 
};

class field_intra_neigh_cell_list : public parser::field_itf
{
public:
  field_intra_neigh_cell_list(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT *data_) { data = data_; }
  ~field_intra_neigh_cell_list(){}
  int parse(Setting &root);    
  const char* get_name() {
    return "intra_neigh_cell_list"; 
  }

private: 
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT *data; 
};

class field_intra_black_cell_list : public parser::field_itf
{
public:
  field_intra_black_cell_list(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT *data_) { data = data_; }
  ~field_intra_black_cell_list(){}
  int parse(Setting &root);    
  const char* get_name() {
    return "intra_black_cell_list"; 
  }

private: 
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT *data; 
};

class field_sf_mapping : public parser::field_itf
{
public:
  field_sf_mapping(uint32_t *sf_mapping_, uint32_t *nof_subframes_) { sf_mapping = sf_mapping_; nof_subframes = nof_subframes_; }
  ~field_sf_mapping(){}
  int parse(Setting &root);    
  const char* get_name() {
    return "sf_mapping"; 
  }

private: 
  uint32_t *sf_mapping; 
  uint32_t *nof_subframes; 
};

class field_qci : public parser::field_itf
{
public:
  field_qci(rrc_cfg_qci_t *cfg_) { cfg = cfg_; }
  ~field_qci(){}
  const char* get_name() {
    return "field_cqi"; 
  }

  int parse(Setting &root);    
private: 
  rrc_cfg_qci_t *cfg; 
};


}

#endif

