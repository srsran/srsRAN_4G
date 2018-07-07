/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
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

#ifndef SRSUE_PHY_H
#define SRSUE_PHY_H

#include "srslte/srslte.h"
#include "srslte/common/log_filter.h"
#include "phy_metrics.h"
#include "phch_recv.h"
#include "prach.h"
#include "phch_worker.h"
#include "phch_common.h"
#include "srslte/radio/radio.h"
#include "srslte/common/task_dispatcher.h"
#include "srslte/common/trace.h"
#include "srslte/interfaces/ue_interfaces.h"

namespace srsue {

typedef _Complex float cf_t;

class phy
    : public phy_interface_mac
    , public phy_interface_rrc
    , public thread
{
public:
  phy();
  bool init(srslte::radio_multi *radio_handler, 
            mac_interface_phy *mac, 
            rrc_interface_phy *rrc,
            std::vector<srslte::log_filter*> log_vec,
            phy_args_t *args = NULL);
  
  void stop();

  void wait_initialize();
  bool is_initiated();

  void set_agc_enable(bool enabled);

  void get_metrics(phy_metrics_t &m);
  void srslte_phy_logger(phy_logger_level_t log_level, char *str);
  
  
  static uint32_t tti_to_SFN(uint32_t tti);
  static uint32_t tti_to_subf(uint32_t tti);

  void enable_pregen_signals(bool enable); 
  
  void start_trace();
  void write_trace(std::string filename);

  void set_earfcn(std::vector<uint32_t> earfcns);
  void force_freq(float dl_freq, float ul_freq);

  void radio_overflow();

  /********** RRC INTERFACE ********************/
  void    reset();
  void    configure_ul_params(bool pregen_disabled = false);
  cell_search_ret_t cell_search(phy_cell_t *cell);
  bool    cell_select(phy_cell_t *cell);

  void    meas_reset();
  int     meas_start(uint32_t earfcn, int pci);
  int     meas_stop(uint32_t earfcn, int pci);

  // also MAC interface
  bool    cell_is_camping();

  /********** MAC INTERFACE ********************/
  /* Sets a C-RNTI allowing the PHY to pregenerate signals if necessary */
  void    set_crnti(uint16_t rnti);
  
  /* Instructs the PHY to configure using the parameters written by set_param() */
  void    configure_prach_params();
  
  /* Transmits PRACH in the next opportunity */
  void    prach_send(uint32_t preamble_idx, int allowed_subframe = -1, float target_power_dbm = 0.0);  
  int     prach_tx_tti();
  
  /* Indicates the transmission of a SR signal in the next opportunity */
  void    sr_send();  
  int     sr_last_tx_tti();

  // Time advance commands
  void    set_timeadv_rar(uint32_t ta_cmd);
  void    set_timeadv(uint32_t ta_cmd);
  
  /* Sets RAR grant payload */
  void    set_rar_grant(uint32_t tti, uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN]); 

  /* Instruct the PHY to decode PDCCH with the CRC scrambled with given RNTI */  
  void    pdcch_ul_search(srslte_rnti_type_t rnti_type, uint16_t rnti, int tti_start = -1, int tti_end = -1);
  void    pdcch_dl_search(srslte_rnti_type_t rnti_type, uint16_t rnti, int tti_start = -1, int tti_end = -1);
  void    pdcch_ul_search_reset();
  void    pdcch_dl_search_reset();
  
  /* Get/Set PHY parameters interface from RRC */  
  void get_config(phy_cfg_t *phy_cfg); 
  void set_config(phy_cfg_t *phy_cfg); 
  void set_config_dedicated(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *dedicated);
  void set_config_common(phy_cfg_common_t *common); 
  void set_config_tdd(LIBLTE_RRC_TDD_CONFIG_STRUCT *tdd); 
  void set_config_64qam_en(bool enable);
  void set_config_mbsfn_sib2(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *sib2);
  void set_config_mbsfn_sib13(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT *sib13);
  void set_config_mbsfn_mcch(LIBLTE_RRC_MCCH_MSG_STRUCT *mcch);
  
  /*Set MAC->PHY MCH period  stopping point*/
  void set_mch_period_stop(uint32_t stop);
  

  float   get_phr();
  float   get_pathloss_db();
    
  uint32_t get_current_tti();

  void     get_current_cell(srslte_cell_t *cell, uint32_t *current_earfcn = NULL);
  uint32_t get_current_earfcn();
  uint32_t get_current_pci();
  
  void    start_plot();
    
private:

  void run_thread();

  bool     initiated;
  uint32_t nof_workers; 
  uint32_t nof_coworkers;

  const static int MAX_WORKERS         = 3;
  const static int DEFAULT_WORKERS     = 2;
  
  const static int SF_RECV_THREAD_PRIO = 1;
  const static int WORKERS_THREAD_PRIO = 0; 
  
  srslte::radio_multi      *radio_handler;
  std::vector<srslte::log_filter*>        log_vec;
  srslte::log              *log_h;
  srslte::log              *log_phy_lib_h;
  srsue::mac_interface_phy *mac;
  srsue::rrc_interface_phy *rrc;

  srslte::thread_pool      workers_pool;
  std::vector<phch_worker> workers;
  phch_common              workers_common; 
  phch_recv                sf_recv; 
  prach                    prach_buffer; 
  
  srslte_cell_t cell;
  
  phy_cfg_t  config;
  phy_args_t *args;
  phy_args_t default_args; 
  
  /* Current time advance */
  uint32_t     n_ta;

  bool init_(srslte::radio *radio_handler, mac_interface_phy *mac, srslte::log *log_h, bool do_agc, uint32_t nof_workers);
  void set_default_args(phy_args_t *args);
  bool check_args(phy_args_t *args); 

};

} // namespace srsue

#endif // SRSUE_PHY_H
