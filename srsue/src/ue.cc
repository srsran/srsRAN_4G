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


#include "ue.h"
//#include "srslte_version_check.h"
#include "srslte/srslte.h"
#include <pthread.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>

using namespace srslte;

namespace srsue{

ue*           ue::instance = NULL;
pthread_mutex_t ue_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

ue* ue::get_instance(void)
{
  pthread_mutex_lock(&ue_instance_mutex);
  if(NULL == instance) {
      instance = new ue();
  }
  pthread_mutex_unlock(&ue_instance_mutex);
  return(instance);
}
void ue::cleanup(void)
{
  pthread_mutex_lock(&ue_instance_mutex);
  if(NULL != instance) {
      delete instance;
      instance = NULL;
  }
  pthread_mutex_unlock(&ue_instance_mutex);
}

ue::ue()
    :started(false)
{
  pool = byte_buffer_pool::get_instance();
}

ue::~ue()
{
  byte_buffer_pool::cleanup();
}

bool ue::init(all_args_t *args_)
{
  args     = args_;
  
  logger.init(args->log.filename);
  rf_log.init("RF  ", &logger);
  phy_log.init("PHY ", &logger, true);
  mac_log.init("MAC ", &logger, true);
  rlc_log.init("RLC ", &logger);
  pdcp_log.init("PDCP", &logger);
  rrc_log.init("RRC ", &logger);
  nas_log.init("NAS ", &logger);
  gw_log.init("GW  ", &logger);
  usim_log.init("USIM", &logger);

  // Init logs
  logger.log("\n\n");
  rf_log.set_level(srslte::LOG_LEVEL_INFO);
  phy_log.set_level(level(args->log.phy_level));
  mac_log.set_level(level(args->log.mac_level));
  rlc_log.set_level(level(args->log.rlc_level));
  pdcp_log.set_level(level(args->log.pdcp_level));
  rrc_log.set_level(level(args->log.rrc_level));
  nas_log.set_level(level(args->log.nas_level));
  gw_log.set_level(level(args->log.gw_level));
  usim_log.set_level(level(args->log.usim_level));

  phy_log.set_hex_limit(args->log.phy_hex_limit);
  mac_log.set_hex_limit(args->log.mac_hex_limit);
  rlc_log.set_hex_limit(args->log.rlc_hex_limit);
  pdcp_log.set_hex_limit(args->log.pdcp_hex_limit);
  rrc_log.set_hex_limit(args->log.rrc_hex_limit);
  nas_log.set_hex_limit(args->log.nas_hex_limit);
  gw_log.set_hex_limit(args->log.gw_hex_limit);
  usim_log.set_hex_limit(args->log.usim_hex_limit);

  // Set up pcap and trace
  if(args->pcap.enable)
  {
    mac_pcap.open(args->pcap.filename.c_str());
    mac.start_pcap(&mac_pcap);
  }
  if(args->trace.enable)
  {
    phy.start_trace();
    radio.start_trace();
  }
  
  // Init layers
  
  /* Start Radio */
  char *dev_name = NULL;
  if (args->rf.device_name.compare("auto")) {
    dev_name = (char*) args->rf.device_name.c_str();
  }
  
  char *dev_args = NULL;
  if (args->rf.device_args.compare("auto")) {
    dev_args = (char*) args->rf.device_args.c_str();
  }
  
  printf("Opening RF device with %d RX antennas...\n", args->rf.nof_rx_ant);
  if(!radio.init_multi(args->rf.nof_rx_ant, dev_args, dev_name))
  {
    printf("Failed to find device %s with args %s\n",
           args->rf.device_name.c_str(), args->rf.device_args.c_str());
    return false;
  }    
  
  // Set RF options
  if (args->rf.time_adv_nsamples.compare("auto")) {
    radio.set_tx_adv(atoi(args->rf.time_adv_nsamples.c_str()));
  }  
  if (args->rf.burst_preamble.compare("auto")) {
    radio.set_burst_preamble(atof(args->rf.burst_preamble.c_str()));    
  }
  
  radio.set_manual_calibration(&args->rf_cal);

  // Set PHY options
  args->expert.phy.nof_rx_ant = args->rf.nof_rx_ant;

  if (args->rf.tx_gain > 0) {
    args->expert.phy.ul_pwr_ctrl_en = false; 
  } else {
    args->expert.phy.ul_pwr_ctrl_en = true; 
  }
  phy.init(&radio, &mac, &rrc, &phy_log, &args->expert.phy);
  
  if (args->rf.rx_gain < 0) {
    radio.start_agc(false);    
    radio.set_tx_rx_gain_offset(10);
    phy.set_agc_enable(true);
  } else {
    radio.set_rx_gain(args->rf.rx_gain);
  }
  if (args->rf.tx_gain > 0) {
    radio.set_tx_gain(args->rf.tx_gain);    
  } else {
    radio.set_tx_gain(args->rf.rx_gain);
    std::cout << std::endl << 
                "Warning: TX gain was not set. " << 
                "Using open-loop power control (not working properly)" << std::endl << std::endl; 
  }

  radio.register_error_handler(rf_msg);

  radio.set_rx_freq(args->rf.dl_freq);
  radio.set_tx_freq(args->rf.ul_freq);

  phy_log.console("Setting frequency: DL=%.1f Mhz, UL=%.1f MHz\n", args->rf.dl_freq/1e6, args->rf.ul_freq/1e6);

  mac.init(&phy, &rlc, &rrc, &mac_log);
  rlc.init(&pdcp, &rrc, this, &rlc_log, &mac);
  pdcp.init(&rlc, &rrc, &gw, &pdcp_log, SECURITY_DIRECTION_UPLINK);
  rrc.init(&phy, &mac, &rlc, &pdcp, &nas, &usim, &mac, &rrc_log);
  
  rrc.set_ue_category(args->expert.ue_cateogry);
  
  nas.init(&usim, &rrc, &gw, &nas_log);
  gw.init(&pdcp, &rrc, this, &gw_log);
  usim.init(&args->usim, &usim_log);

  started = true;
  return true;
}

void ue::pregenerate_signals(bool enable)
{
  phy.enable_pregen_signals(enable);
}

void ue::test_con_restablishment() {
  rrc.test_con_restablishment();
}

void ue::stop()
{
  if(started)
  {
    usim.stop();
    nas.stop();
    rrc.stop();
    
    // Caution here order of stop is very important to avoid locks

    
    // Stop RLC and PDCP before GW to avoid locking on queue
    rlc.stop();
    pdcp.stop();
    gw.stop();

    // PHY must be stopped before radio otherwise it will lock on rf_recv()
    mac.stop();
    phy.stop();
    radio.stop();
    
    usleep(1e5);
    if(args->pcap.enable)
    {
       mac_pcap.close();
    }
    if(args->trace.enable)
    {
      phy.write_trace(args->trace.phy_filename);
      radio.write_trace(args->trace.radio_filename);
    }
    started = false;
  }
}

bool ue::is_attached()
{
  return (EMM_STATE_REGISTERED == nas.get_state());
}

void ue::start_plot() {
  phy.start_plot();
}

bool ue::get_metrics(ue_metrics_t &m)
{
  m.rf = rf_metrics;
  bzero(&rf_metrics, sizeof(rf_metrics_t));
  rf_metrics.rf_error = false; // Reset error flag

  if(EMM_STATE_REGISTERED == nas.get_state()) {
    if(RRC_STATE_RRC_CONNECTED == rrc.get_state()) {
      phy.get_metrics(m.phy);
      mac.get_metrics(m.mac);
      rlc.get_metrics(m.rlc);
      gw.get_metrics(m.gw);
      return true;
    }
  }
  return false;
}

void ue::rf_msg(srslte_rf_error_t error)
{
  ue *u = ue::get_instance();
  u->handle_rf_msg(error);
}

void ue::handle_rf_msg(srslte_rf_error_t error)
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
    rf_log.info(str);
  }
}

srslte::LOG_LEVEL_ENUM ue::level(std::string l)
{
  std::transform(l.begin(), l.end(), l.begin(), ::toupper);
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

} // namespace srsue
