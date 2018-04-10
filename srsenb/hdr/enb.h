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
#include "mac/mac.h"
#include "upper/rrc.h"
#include "upper/gtpu.h"
#include "upper/s1ap.h"
#include "upper/rlc.h"
#include "upper/pdcp.h"

#include "srslte/radio/radio.h"

#include "srslte/common/bcd_helpers.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/logger_file.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/interfaces/sched_interface.h"
#include "srslte/interfaces/enb_metrics_interface.h"

namespace srsenb {

/*******************************************************************************
  eNodeB Parameters
*******************************************************************************/

typedef struct {
  s1ap_args_t s1ap; 
  uint32_t    n_prb; 
  uint32_t    pci; 
  uint32_t    nof_ports;
  uint32_t    transmission_mode;
  float       p_a;
}enb_args_t;

typedef struct {
  std::string sib_config;
  std::string rr_config; 
  std::string drb_config; 
} enb_files_t; 

typedef struct {
  uint32_t      dl_earfcn;
  uint32_t      ul_earfcn; 
  float         dl_freq; 
  float         ul_freq; 
  float         rx_gain;
  float         tx_gain;
  std::string   device_name; 
  std::string   device_args; 
  std::string   time_adv_nsamples; 
  std::string   burst_preamble; 
}rf_args_t;

typedef struct {
  bool          enable;
  std::string   filename;
}pcap_args_t;

typedef struct {
  std::string   phy_level;
  std::string   phy_lib_level;
  std::string   mac_level;
  std::string   rlc_level;
  std::string   pdcp_level;
  std::string   rrc_level;
  std::string   gtpu_level;
  std::string   s1ap_level;
  std::string   all_level;
  int           phy_hex_limit;
  int           mac_hex_limit;
  int           rlc_hex_limit;
  int           pdcp_hex_limit;
  int           rrc_hex_limit;
  int           gtpu_hex_limit;
  int           s1ap_hex_limit;
  int           all_hex_limit;
  int           file_max_size;
  std::string   filename;
}log_args_t;

typedef struct {
  bool          enable;
}gui_args_t;

typedef struct {
  phy_args_t phy; 
  mac_args_t mac; 
  uint32_t   rrc_inactivity_timer;
  float      metrics_period_secs;
}expert_args_t;

typedef struct { 
  enb_args_t    enb;
  enb_files_t   enb_files; 
  rf_args_t     rf;
  rf_cal_t      rf_cal; 
  pcap_args_t   pcap;
  log_args_t    log;
  gui_args_t    gui;
  expert_args_t expert;
}all_args_t;

/*******************************************************************************
  Main UE class
*******************************************************************************/

class enb
    :public enb_metrics_interface {
public:
  static enb *get_instance(void);

  static void cleanup(void);

  bool init(all_args_t *args_);

  void stop();

  void start_plot();

  static void rf_msg(srslte_rf_error_t error);

  void handle_rf_msg(srslte_rf_error_t error);

  // eNodeB metrics interface
  bool get_metrics(enb_metrics_t &m);

  void pregenerate_signals(bool enable);


private:
  static enb *instance;

  enb();

  virtual ~enb();

  srslte::radio radio;
  srsenb::phy phy;
  srsenb::mac mac;
  srslte::mac_pcap mac_pcap;
  srsenb::rlc rlc;
  srsenb::pdcp pdcp;
  srsenb::rrc rrc;
  srsenb::gtpu gtpu;
  srsenb::s1ap s1ap;

  srslte::logger_stdout logger_stdout;
  srslte::logger_file   logger_file;
  srslte::logger        *logger;

  srslte::log_filter  rf_log;
  std::vector<srslte::log_filter*>  phy_log;
  srslte::log_filter  mac_log;
  srslte::log_filter  rlc_log;
  srslte::log_filter  pdcp_log;
  srslte::log_filter  rrc_log;
  srslte::log_filter  gtpu_log;
  srslte::log_filter  s1ap_log;

  srslte::byte_buffer_pool *pool;

  all_args_t       *args;
  bool              started;
  rf_metrics_t      rf_metrics;

  srslte::LOG_LEVEL_ENUM level(std::string l);
  
  bool check_srslte_version();
  int parse_sib1(std::string filename, LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *data);
  int parse_sib2(std::string filename, LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *data); 
  int parse_sib3(std::string filename, LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT *data);
  int parse_sib4(std::string filename, LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_4_STRUCT *data);
  int parse_sib9(std::string filename, LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_9_STRUCT *data);
  int parse_sibs(all_args_t *args, rrc_cfg_t *rrc_cfg, phy_cfg_t *phy_config_common); 
  int parse_rr(all_args_t *args, rrc_cfg_t *rrc_cfg);
  int parse_drb(all_args_t *args, rrc_cfg_t *rrc_cfg); 
  bool sib_is_present(LIBLTE_RRC_SCHEDULING_INFO_STRUCT *sched_info, uint32_t nof_sched_info, LIBLTE_RRC_SIB_TYPE_ENUM sib_num);
  int parse_cell_cfg(all_args_t *args, srslte_cell_t *cell); 
};

} // namespace srsenb

#endif // SRSENB_ENB_H
  
