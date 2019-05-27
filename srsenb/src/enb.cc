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

#include <boost/algorithm/string.hpp>
#include "srsenb/hdr/enb.h"
#include "srslte/build_info.h"
#include <iostream>
#include <sstream>

namespace srsenb {

enb*            enb::instance      = nullptr;
pthread_mutex_t enb_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

enb* enb::get_instance()
{
  pthread_mutex_lock(&enb_instance_mutex);
  if (nullptr == instance) {
    instance = new enb();
  }
  pthread_mutex_unlock(&enb_instance_mutex);
  return(instance);
}
void enb::cleanup()
{
  srslte_dft_exit();
  pthread_mutex_lock(&enb_instance_mutex);
  if (nullptr != instance) {
    delete instance;
    instance = nullptr;
  }
  srslte::byte_buffer_pool::cleanup(); // pool has to be cleaned after enb is deleted
  pthread_mutex_unlock(&enb_instance_mutex);
}

enb::enb() : started(false) {
  // print build info
  std::cout << std::endl << get_build_string() << std::endl;

  srslte_dft_load();
  pool = srslte::byte_buffer_pool::get_instance(ENB_POOL_SIZE);

  logger = nullptr;
  args   = nullptr;

  bzero(&rf_metrics, sizeof(rf_metrics));
}

enb::~enb()
{
  for (auto& i : phy_log) {
    delete i;
  }
}

bool enb::init(all_args_t *args_)
{
  args = args_;

  if (args->log.filename == "stdout") {
    logger = &logger_stdout;
  } else {
    logger_file.init(args->log.filename, args->log.file_max_size);
    logger_file.log("\n\n");
    logger_file.log(get_build_string().c_str());
    logger = &logger_file;
  }

  rf_log.init("RF  ", logger);
  
  // Create array of pointers to phy_logs 
  for (int i=0;i<args->expert.phy.nof_phy_threads;i++) {
    auto* mylog = new srslte::log_filter;
    char tmp[16];
    sprintf(tmp, "PHY%d",i);
    mylog->init(tmp, logger, true);
    phy_log.push_back(mylog);
  }
  mac_log.init("MAC ", logger, true);
  rlc_log.init("RLC ", logger);
  pdcp_log.init("PDCP", logger);
  rrc_log.init("RRC ", logger);
  gtpu_log.init("GTPU", logger);
  s1ap_log.init("S1AP", logger);

  pool_log.init("POOL", logger);
  pool_log.set_level(srslte::LOG_LEVEL_ERROR);
  pool->set_log(&pool_log);

  // Init logs
  rf_log.set_level(srslte::LOG_LEVEL_INFO);
  for (int i=0;i<args->expert.phy.nof_phy_threads;i++) {
    ((srslte::log_filter*) phy_log[i])->set_level(level(args->log.phy_level));
  }
  mac_log.set_level(level(args->log.mac_level));
  rlc_log.set_level(level(args->log.rlc_level));
  pdcp_log.set_level(level(args->log.pdcp_level));
  rrc_log.set_level(level(args->log.rrc_level));
  gtpu_log.set_level(level(args->log.gtpu_level));
  s1ap_log.set_level(level(args->log.s1ap_level));

  for (int i=0;i<args->expert.phy.nof_phy_threads;i++) {
    ((srslte::log_filter*) phy_log[i])->set_hex_limit(args->log.phy_hex_limit);
  }
  mac_log.set_hex_limit(args->log.mac_hex_limit);
  rlc_log.set_hex_limit(args->log.rlc_hex_limit);
  pdcp_log.set_hex_limit(args->log.pdcp_hex_limit);
  rrc_log.set_hex_limit(args->log.rrc_hex_limit);
  gtpu_log.set_hex_limit(args->log.gtpu_hex_limit);
  s1ap_log.set_hex_limit(args->log.s1ap_hex_limit);

  // Parse config files
  srslte_cell_t cell_cfg;
  phy_cfg_t     phy_cfg;
  rrc_cfg_t     rrc_cfg;

  if (parse_cell_cfg(args, &cell_cfg)) {
    fprintf(stderr, "Error parsing Cell configuration\n");
    return false;
  }
  if (parse_sibs(args, &rrc_cfg, &phy_cfg)) {
    fprintf(stderr, "Error parsing SIB configuration\n");
    return false;
  }
  if (parse_rr(args, &rrc_cfg)) {
    fprintf(stderr, "Error parsing Radio Resources configuration\n");
    return false;
  }
  if (parse_drb(args, &rrc_cfg)) {
    fprintf(stderr, "Error parsing DRB configuration\n");
    return false;
  }

  if (args->enb.transmission_mode == 1) {
    phy_cfg.pdsch_cnfg.p_b = 0; // Default TM1
  } else {
    phy_cfg.pdsch_cnfg.p_b = 1; // Default TM2,3,4
  }

  uint32_t prach_freq_offset = rrc_cfg.sibs[1].sib2().rr_cfg_common.prach_cfg.prach_cfg_info.prach_freq_offset;

  if (cell_cfg.nof_prb > 10) {
    uint32_t lower_bound = SRSLTE_MAX(rrc_cfg.sr_cfg.nof_prb, rrc_cfg.cqi_cfg.nof_prb);
    uint32_t upper_bound = cell_cfg.nof_prb - lower_bound;
    if (prach_freq_offset + 6 > upper_bound or prach_freq_offset < lower_bound) {
      fprintf(stderr,
              "ERROR: Invalid PRACH configuration - prach_freq_offset=%d collides with PUCCH.\n",
              prach_freq_offset);
      fprintf(stderr,
              "       Consider changing \"prach_freq_offset\" in sib.conf to a value between %d and %d.\n",
              lower_bound,
              upper_bound);
      return false;
    }
  } else { // 6 PRB case
    if (prach_freq_offset + 6 > cell_cfg.nof_prb) {
      fprintf(stderr,
              "ERROR: Invalid PRACH configuration - prach=(%d, %d) does not fit into the eNB PRBs=(0, %d).\n",
              prach_freq_offset,
              prach_freq_offset + 6,
              cell_cfg.nof_prb);
      fprintf(
          stderr,
          "       Consider changing the \"prach_freq_offset\" value to 0 in the sib.conf file when using 6 PRBs.\n");
      return false;
    }
  }

  rrc_cfg.inactivity_timeout_ms = args->expert.rrc_inactivity_timer;
  rrc_cfg.enable_mbsfn          = args->expert.enable_mbsfn;

  // Check number of control symbols
  if (cell_cfg.nof_prb < 50 && args->expert.mac.sched.nof_ctrl_symbols != 3) {
    args->expert.mac.sched.nof_ctrl_symbols = 3;
    mac_log.info("Setting number of control symbols to %d for %d PRB cell.\n",
                 args->expert.mac.sched.nof_ctrl_symbols,
                 cell_cfg.nof_prb);
  }

  // Parse EEA preference list
  std::vector<std::string> eea_pref_list;
  boost::split(eea_pref_list, args->expert.eea_pref_list,
               boost::is_any_of(","));
  int i = 0;
  for (auto it = eea_pref_list.begin(); it != eea_pref_list.end() && i < srslte::CIPHERING_ALGORITHM_ID_N_ITEMS; it++) {
    boost::trim_left(*it);
    if ((*it) == "EEA0") {
      rrc_cfg.eea_preference_list[i] = srslte::CIPHERING_ALGORITHM_ID_EEA0;
      i++;
    } else if ((*it) == "EEA1") {
      rrc_cfg.eea_preference_list[i] = srslte::CIPHERING_ALGORITHM_ID_128_EEA1;
      i++;
    } else if ((*it) == "EEA2") {
      rrc_cfg.eea_preference_list[i] = srslte::CIPHERING_ALGORITHM_ID_128_EEA2;
      i++;
    } else {
      fprintf(stderr, "Failed to parse EEA prefence list %s \n",
              args->expert.eea_pref_list.c_str());
      return false;
    }
  }

  // Parse EIA preference list
  std::vector<std::string> eia_pref_list;
  boost::split(eia_pref_list, args->expert.eia_pref_list,
               boost::is_any_of(","));
  i = 0;
  for (auto it = eia_pref_list.begin(); it != eia_pref_list.end() && i < srslte::INTEGRITY_ALGORITHM_ID_N_ITEMS; it++) {
    boost::trim_left(*it);
    if ((*it) == "EIA0") {
      rrc_cfg.eia_preference_list[i] = srslte::INTEGRITY_ALGORITHM_ID_EIA0;
      i++;
    } else if ((*it) == "EIA1") {
      rrc_cfg.eia_preference_list[i] = srslte::INTEGRITY_ALGORITHM_ID_128_EIA1;
      i++;
    } else if ((*it) == "EIA2") {
      rrc_cfg.eia_preference_list[i] = srslte::INTEGRITY_ALGORITHM_ID_128_EIA2;
      i++;
    } else {
      fprintf(stderr, "Failed to parse EIA prefence list %s \n",
              args->expert.eia_pref_list.c_str());
      return false;
    }
  }

  // Copy cell struct to rrc and phy
  memcpy(&rrc_cfg.cell, &cell_cfg, sizeof(srslte_cell_t));
  memcpy(&phy_cfg.cell, &cell_cfg, sizeof(srslte_cell_t));

  // Set up pcap and trace
  if(args->pcap.enable)
  {
    mac_pcap.open(args->pcap.filename.c_str());
    mac.start_pcap(&mac_pcap);
  }

  // Init layers
  
  /* Start Radio */
  char* dev_name = nullptr;
  if (args->rf.device_name != "auto") {
    dev_name = (char*) args->rf.device_name.c_str();
  }

  char* dev_args = nullptr;
  if (args->rf.device_args != "auto") {
    dev_args = (char*) args->rf.device_args.c_str();
  }

  if (!radio.init(phy_log[0], dev_args, dev_name, args->enb.nof_ports)) {
    phy_log[0]->console(
        "Failed to find device %s with args %s\n", args->rf.device_name.c_str(), args->rf.device_args.c_str());
    return false;
  }    
  
  // Set RF options
  if (args->rf.time_adv_nsamples != "auto") {
    radio.set_tx_adv((int)strtol(args->rf.time_adv_nsamples.c_str(), nullptr, 10));
  }
  if (args->rf.burst_preamble != "auto") {
    radio.set_burst_preamble(strtof(args->rf.burst_preamble.c_str(), nullptr));
  }

  radio.set_rx_gain(args->rf.rx_gain);
  radio.set_tx_gain(args->rf.tx_gain);
  ((srslte::log_filter*) phy_log[0])->console("Setting frequency: DL=%.1f Mhz, UL=%.1f MHz\n", args->rf.dl_freq/1e6, args->rf.ul_freq/1e6);

  radio.set_tx_freq(args->enb.nof_ports, args->rf.dl_freq);
  radio.set_rx_freq(args->enb.nof_ports, args->rf.ul_freq);

  radio.register_error_handler(rf_msg);

  // Init all layers   
  phy.init(&args->expert.phy, &phy_cfg, &radio, &mac, phy_log);
  mac.init(&args->expert.mac, &cell_cfg, &phy, &rlc, &rrc, &mac_log);
  rlc.init(&pdcp, &rrc, &mac, &mac, &rlc_log);
  pdcp.init(&rlc, &rrc, &gtpu, &pdcp_log);
  rrc.init(&rrc_cfg, &phy, &mac, &rlc, &pdcp, &s1ap, &gtpu, &rrc_log);
  s1ap.init(args->enb.s1ap, &rrc, &s1ap_log);
  gtpu.init(args->enb.s1ap.gtp_bind_addr, args->enb.s1ap.mme_addr, args->expert.m1u_multiaddr, args->expert.m1u_if_addr, &pdcp, &gtpu_log, args->expert.enable_mbsfn);

  started = true;
  return true;
}

void enb::stop()
{
  if(started)
  {
    s1ap.stop();
    gtpu.stop();
    phy.stop();
    mac.stop();
    usleep(50000);

    rlc.stop();
    pdcp.stop();
    rrc.stop();

    usleep(10000);
    if(args->pcap.enable)
    {
       mac_pcap.close();
    }
    radio.stop();
    started = false;
  }
}

void enb::start_plot() {
  phy.start_plot();
}

void enb::print_pool() {
  srslte::byte_buffer_pool::get_instance()->print_all_buffers();
}

bool enb::get_metrics(enb_metrics_t* m)
{
  m->rf = rf_metrics;
  bzero(&rf_metrics, sizeof(rf_metrics_t));
  rf_metrics.rf_error = false; // Reset error flag

  phy.get_metrics(m->phy);
  mac.get_metrics(m->mac);
  rrc.get_metrics(m->rrc);
  s1ap.get_metrics(m->s1ap);

  m->running = started;
  return true;
}

void enb::rf_msg(srslte_rf_error_t error)
{
  enb *u = enb::get_instance();
  u->handle_rf_msg(error);
}

void enb::handle_rf_msg(srslte_rf_error_t error)
{
  if(error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_OVERFLOW) {
    rf_metrics.rf_o++;
    rf_metrics.rf_error = true;
    rf_log.warning("Overflow\n");
  }else if(error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_UNDERFLOW) {
    rf_metrics.rf_u++;
    rf_metrics.rf_error = true;
    rf_log.warning("Underflow\n");
  } else if(error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_LATE) {
    rf_metrics.rf_l++;
    rf_metrics.rf_error = true;
    rf_log.warning("Late\n");
  } else if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_OTHER) {
    std::string str(error.msg);
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    str.push_back('\n');
    rf_log.info("%s\n", str.c_str());
  }
}

srslte::LOG_LEVEL_ENUM enb::level(std::string l)
{
  boost::to_upper(l);
  if("NONE" == l){
    return srslte::LOG_LEVEL_NONE;
  }else if("ERROR" == l){
    return srslte::LOG_LEVEL_ERROR;
  }else if("WARNING" == l){
    return srslte::LOG_LEVEL_WARNING;
  }else if("INFO" == l){
    return srslte::LOG_LEVEL_INFO;
  }else if("DEBUG" == l){
    return srslte::LOG_LEVEL_DEBUG;
  }else{
    return srslte::LOG_LEVEL_NONE;
  }
}

std::string enb::get_build_mode()
{
  return std::string(srslte_get_build_mode());
}

std::string enb::get_build_info()
{
  if (std::string(srslte_get_build_info()).find("  ") != std::string::npos) {
    return std::string(srslte_get_version());
  }
  return std::string(srslte_get_build_info());
}

std::string enb::get_build_string()
{
  std::stringstream ss;
  ss << "Built in " << get_build_mode() << " mode using " << get_build_info() << "." << std::endl;
  return ss.str();
}

} // namespace srsenb
