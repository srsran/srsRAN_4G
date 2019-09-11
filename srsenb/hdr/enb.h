/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

/******************************************************************************
 * File:        enb.h
 * Description: Top-level eNodeB class. Creates and links all
 *              layers and helpers.
 *****************************************************************************/

#ifndef SRSENB_ENB_H
#define SRSENB_ENB_H

#include <stdarg.h>
#include <string>
#include <pthread.h>

#include "phy/phy.h"
#include "srsenb/hdr/stack/rrc/rrc.h"

#include "srslte/radio/radio_base.h"

#include "srsenb/hdr/phy/enb_phy_base.h"
#include "srsenb/hdr/stack/enb_stack_base.h"
#include "srsenb/hdr/stack/enb_stack_lte.h"

#include "srslte/common/bcd_helpers.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/logger_file.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/common/security.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/interfaces/sched_interface.h"
#include "srslte/interfaces/ue_interfaces.h"

namespace srsenb {

/*******************************************************************************
  eNodeB Parameters
*******************************************************************************/

struct enb_args_t {
  uint32_t    dl_earfcn;
  uint32_t    ul_earfcn;
  uint32_t    n_prb;
  uint32_t    pci;
  uint32_t    nof_ports;
  uint32_t    transmission_mode;
  float       p_a;
};

struct enb_files_t {
  std::string sib_config;
  std::string rr_config;
  std::string drb_config;
};

struct log_args_t {
  std::string all_level;
  int         phy_hex_limit;

  int         all_hex_limit;
  int         file_max_size;
  std::string filename;
};

struct gui_args_t {
  bool enable;
};

struct general_args_t {
  uint32_t    rrc_inactivity_timer;
  float       metrics_period_secs;
  bool        metrics_csv_enable;
  std::string metrics_csv_filename;
  bool        print_buffer_state;
  std::string eia_pref_list;
  std::string eea_pref_list;
};

struct all_args_t {
  enb_args_t        enb;
  enb_files_t       enb_files;
  srslte::rf_args_t rf;
  log_args_t        log;
  gui_args_t        gui;
  general_args_t    general;
  phy_args_t        phy;
  stack_args_t      stack;
};

/*******************************************************************************
  Main eNB class
*******************************************************************************/

class enb : public enb_metrics_interface
{
public:
  static enb *get_instance(void);

  static void cleanup(void);

  int init(const all_args_t& args_);

  void stop();

  void start_plot();

  void print_pool();

  static void rf_msg(srslte_rf_error_t error);

  void handle_rf_msg(srslte_rf_error_t error);

  // eNodeB metrics interface
  bool get_metrics(enb_metrics_t* m);


private:
  static enb *instance;

  const static int ENB_POOL_SIZE = 1024*10;

  enb();

  virtual ~enb();

  int parse_args(const all_args_t& args_);

  // eNB components
  std::unique_ptr<enb_stack_base>     stack = nullptr;
  std::unique_ptr<srslte::radio_base> radio = nullptr;
  std::unique_ptr<enb_phy_base>       phy   = nullptr;

  srslte::logger_stdout logger_stdout;
  srslte::logger_file   logger_file;
  srslte::logger*       logger = nullptr;
  srslte::log_filter    log; // Own logger for eNB

  srslte::log_filter  pool_log;

  srslte::byte_buffer_pool* pool = nullptr;

  all_args_t args    = {};
  bool       started = false;

  phy_cfg_t phy_cfg = {};
  rrc_cfg_t rrc_cfg = {};

  srslte::LOG_LEVEL_ENUM level(std::string l);

  //  bool check_srslte_version();
  int  parse_sib1(std::string filename, asn1::rrc::sib_type1_s* data);
  int  parse_sib2(std::string filename, asn1::rrc::sib_type2_s* data);
  int  parse_sib3(std::string filename, asn1::rrc::sib_type3_s* data);
  int  parse_sib4(std::string filename, asn1::rrc::sib_type4_s* data);
  int  parse_sib7(std::string filename, asn1::rrc::sib_type7_s* data);
  int  parse_sib9(std::string filename, asn1::rrc::sib_type9_s* data);
  int  parse_sib13(std::string filename, asn1::rrc::sib_type13_r9_s* data);
  int  parse_sibs(all_args_t* args, rrc_cfg_t* rrc_cfg, phy_cfg_t* phy_config_common);
  int  parse_rr(all_args_t* args, rrc_cfg_t* rrc_cfg);
  int  parse_drb(all_args_t* args, rrc_cfg_t* rrc_cfg);
  bool sib_is_present(const asn1::rrc::sched_info_list_l& l, asn1::rrc::sib_type_e sib_num);
  int  parse_cell_cfg(all_args_t* args, srslte_cell_t* cell);

  std::string get_build_mode();
  std::string get_build_info();
  std::string get_build_string();
};

} // namespace srsenb

#endif // SRSENB_ENB_H
  
