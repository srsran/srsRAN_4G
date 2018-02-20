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
#include "srslte/srslte.h"
#include <pthread.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>

using namespace srslte;

namespace srsue{

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

  if (!args->log.filename.compare("stdout")) {
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
    srslte::log_filter *mylog = new srslte::log_filter;
    char tmp[16];
    sprintf(tmp, "PHY%d",i);
    mylog->init(tmp, logger, true);
    phy_log.push_back((void*) mylog);
  }

  mac_log.init("MAC ", logger, true);
  rlc_log.init("RLC ", logger);
  pdcp_log.init("PDCP", logger);
  rrc_log.init("RRC ", logger);
  nas_log.init("NAS ", logger);
  gw_log.init("GW  ", logger);
  usim_log.init("USIM", logger);

  // Init logs
  rf_log.set_level(srslte::LOG_LEVEL_INFO);
  rf_log.info("Starting UE\n");
  for (int i=0;i<args->expert.phy.nof_phy_threads;i++) {
    ((srslte::log_filter*) phy_log[i])->set_level(level(args->log.phy_level));
  }
  
  /* here we add a log layer to handle logging from the phy library*/
  srslte::log_filter *lib_log = new srslte::log_filter;
  char tmp[16];
  sprintf(tmp, "PHY_LIB");
  lib_log->init(tmp, logger, true);
  phy_log.push_back((void*) lib_log);
  ((srslte::log_filter*) phy_log[args->expert.phy.nof_phy_threads])->set_level(level(args->log.phy_lib_level));
 
  
  mac_log.set_level(level(args->log.mac_level));
  rlc_log.set_level(level(args->log.rlc_level));
  pdcp_log.set_level(level(args->log.pdcp_level));
  rrc_log.set_level(level(args->log.rrc_level));
  nas_log.set_level(level(args->log.nas_level));
  gw_log.set_level(level(args->log.gw_level));
  usim_log.set_level(level(args->log.usim_level));

  for (int i=0;i<args->expert.phy.nof_phy_threads + 1;i++) {
    ((srslte::log_filter*) phy_log[i])->set_hex_limit(args->log.phy_hex_limit);
  }
  mac_log.set_hex_limit(args->log.mac_hex_limit);
  rlc_log.set_hex_limit(args->log.rlc_hex_limit);
  pdcp_log.set_hex_limit(args->log.pdcp_hex_limit);
  rrc_log.set_hex_limit(args->log.rrc_hex_limit);
  nas_log.set_hex_limit(args->log.nas_hex_limit);
  gw_log.set_hex_limit(args->log.gw_hex_limit);
  usim_log.set_hex_limit(args->log.usim_hex_limit);

  // Set up pcap and trace
  if(args->pcap.enable) {
    mac_pcap.open(args->pcap.filename.c_str());
    mac.start_pcap(&mac_pcap);
  }
  if(args->pcap.nas_enable) {
    nas_pcap.open(args->pcap.nas_filename.c_str());
    nas.start_pcap(&nas_pcap);
  }
  if(args->trace.enable) {
    phy.start_trace();
    radio.start_trace();
  }
  
  // Init layers

  if (args->rf.rx_gain < 0) {
    phy.set_agc_enable(true);
  }

    // PHY initis in background, start before radio
  args->expert.phy.nof_rx_ant = args->rf.nof_rx_ant;
  phy.init(&radio, &mac, &rrc, phy_log, &args->expert.phy);

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
  if(!radio.init_multi(args->rf.nof_rx_ant, dev_args, dev_name)) {
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

  if (args->rf.tx_gain > 0) {
    args->expert.phy.ul_pwr_ctrl_en = false; 
  } else {
    args->expert.phy.ul_pwr_ctrl_en = true; 
  }

  if (args->rf.rx_gain < 0) {
    radio.start_agc(false);    
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
  radio.set_freq_offset(args->rf.freq_offset);

  mac.init(&phy, &rlc, &rrc, &mac_log);
  rlc.init(&pdcp, &rrc, this, &rlc_log, &mac, 0 /* RB_ID_SRB0 */);
  pdcp.init(&rlc, &rrc, &gw, &pdcp_log, 0 /* RB_ID_SRB0 */, SECURITY_DIRECTION_UPLINK);

  usim.init(&args->usim, &usim_log);
  srslte_nas_config_t nas_cfg(1, args->apn); /* RB_ID_SRB1 */
  nas.init(&usim, &rrc, &gw, &nas_log, nas_cfg);
  gw.init(&pdcp, &nas, &gw_log, 3 /* RB_ID_DRB1 */);

  gw.set_netmask(args->expert.ip_netmask);

  rrc.init(&phy, &mac, &rlc, &pdcp, &nas, &usim, &mac, &rrc_log);

  // Get current band from provided EARFCN
  args->rrc.supported_bands[0] = srslte_band_get_band(args->rf.dl_earfcn);
  args->rrc.nof_supported_bands = 1;
  args->rrc.ue_category = atoi(args->ue_category_str.c_str());
  rrc.set_args(&args->rrc);

  // Currently EARFCN list is set to only one frequency as indicated in ue.conf
  std::vector<uint32_t> earfcn_list;
  earfcn_list.push_back(args->rf.dl_earfcn);
  phy.set_earfcn(earfcn_list);

  if (args->rf.dl_freq > 0 && args->rf.ul_freq > 0) {
    phy.force_freq(args->rf.dl_freq, args->rf.ul_freq);
  }

  printf("Waiting PHY to initialize...\n");
  phy.wait_initialize();
  phy.configure_ul_params();

  printf("...\n");
  nas.attach_request();

  started = true;
  return true;
}

void ue::pregenerate_signals(bool enable)
{
  phy.enable_pregen_signals(enable);
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
    if(args->pcap.enable) {
       mac_pcap.close();
    }
    if(args->pcap.nas_enable) {
       nas_pcap.close();
    }
    if(args->trace.enable) {
      phy.write_trace(args->trace.phy_filename);
      radio.write_trace(args->trace.radio_filename);
    }
    started = false;
  }
}

bool ue::is_attached()
{
  return (RRC_STATE_CONNECTED == rrc.get_state());
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
    if(RRC_STATE_CONNECTED == rrc.get_state()) {
      phy.get_metrics(m.phy);
      mac.get_metrics(m.mac);
      rlc.get_metrics(m.rlc);
      gw.get_metrics(m.gw);
      return true;
    }
  }
  return false;
}

void ue::radio_overflow() {
  phy.radio_overflow();
}

void ue::rf_msg(srslte_rf_error_t error)
{
  ue_base *ue = ue_base::get_instance(LTE);
  ue->handle_rf_msg(error);
  if(error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_OVERFLOW) {
    ue->radio_overflow();
  }
}

} // namespace srsue
