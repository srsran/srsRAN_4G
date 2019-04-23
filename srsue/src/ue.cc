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


#include "srsue/hdr/ue.h"
#include "srslte/srslte.h"
#include <pthread.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>

using namespace srslte;

namespace srsue{

ue::ue()
    :started(false), mac_log()
{
  usim = NULL;
  logger = NULL;
  args = NULL;
}

ue::~ue()
{
  for (uint32_t i = 0; i < phy_log.size(); i++) {
    if (phy_log[i]) {
      delete(phy_log[i]);
    }
  }
  if (usim) {
    delete usim;
  }
}

bool ue::init(all_args_t *args_) {
  args = args_;

  int nof_phy_threads = args->expert.phy.nof_phy_threads;
  if (nof_phy_threads > srsue::phy::MAX_WORKERS) {
    nof_phy_threads = srsue::phy::MAX_WORKERS;
  }

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
  for (int i=0;i<nof_phy_threads;i++) {
    srslte::log_filter *mylog = new srslte::log_filter;
    char tmp[16];
    sprintf(tmp, "PHY%d",i);
    mylog->init(tmp, logger, true);
    phy_log.push_back(mylog);
  }

  mac_log.init("MAC ", logger, true);
  rlc_log.init("RLC ", logger);
  pdcp_log.init("PDCP", logger);
  rrc_log.init("RRC ", logger);
  nas_log.init("NAS ", logger);
  gw_log.init("GW  ", logger);
  usim_log.init("USIM", logger);

  pool_log.init("POOL", logger);
  pool_log.set_level(srslte::LOG_LEVEL_ERROR);
  byte_buffer_pool::get_instance()->set_log(&pool_log);

  // Init logs
  rf_log.set_level(srslte::LOG_LEVEL_INFO);
  rf_log.info("Starting UE\n");
  for (int i=0;i<nof_phy_threads;i++) {
    ((srslte::log_filter*) phy_log[i])->set_level(level(args->log.phy_level));
  }
  
  /* here we add a log layer to handle logging from the phy library*/
  if (level(args->log.phy_lib_level) != LOG_LEVEL_NONE) {
    srslte::log_filter *lib_log = new srslte::log_filter;
    char tmp[16];
    sprintf(tmp, "PHY_LIB");
    lib_log->init(tmp, logger, true);
    phy_log.push_back(lib_log);
    ((srslte::log_filter*) phy_log[nof_phy_threads])->set_level(level(args->log.phy_lib_level));
  } else {
    phy_log.push_back(NULL);
  }

  mac_log.set_level(level(args->log.mac_level));
  rlc_log.set_level(level(args->log.rlc_level));
  pdcp_log.set_level(level(args->log.pdcp_level));
  rrc_log.set_level(level(args->log.rrc_level));
  nas_log.set_level(level(args->log.nas_level));
  gw_log.set_level(level(args->log.gw_level));
  usim_log.set_level(level(args->log.usim_level));

  for (int i=0;i<nof_phy_threads + 1;i++) {
    if (phy_log[i]) {
      ((srslte::log_filter*) phy_log[i])->set_hex_limit(args->log.phy_hex_limit);
    }
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

  // populate EARFCN list
  std::vector<uint32_t> earfcn_list;
  if (!args->rf.dl_earfcn.empty()) {
    std::stringstream ss(args->rf.dl_earfcn);
    int idx = 0;
    while (ss.good()) {
      std::string substr;
      getline(ss, substr, ',');
      const int earfcn               = atoi(substr.c_str());
      args->rrc.supported_bands[idx] = srslte_band_get_band(earfcn);
      args->rrc.nof_supported_bands  = ++idx;
      earfcn_list.push_back(earfcn);
    }
  } else {
    rrc_log.error("Error: dl_earfcn list is empty\n");
    rrc_log.console("Error: dl_earfcn list is empty\n");
    return false;
  }

  // Consider Carrier Aggregation support if more than one
  args->rrc.support_ca = (args->rf.nof_rf_channels * args->rf.nof_radios) > 1;

  // Init layers

  // Init USIM first to allow early exit in case reader couldn't be found
  usim = usim_base::get_instance(&args->usim, &usim_log);
  if (usim->init(&args->usim, &usim_log)) {
    usim_log.console("Failed to initialize USIM.\n");
    return false;
  }

  // PHY inits in background, start before radio
  args->expert.phy.nof_rx_ant = args->rf.nof_rx_ant;
  args->expert.phy.ue_category = args->rrc.ue_category;
  phy.init(radios, &mac, &rrc, phy_log, &args->expert.phy);

  // Calculate number of carriers available in all radios
  args->expert.phy.nof_radios      = args->rf.nof_radios;
  args->expert.phy.nof_rf_channels = args->rf.nof_rf_channels;
  args->expert.phy.nof_carriers    = args->rf.nof_radios * args->rf.nof_rf_channels;

  // Generate RF-Channel to Carrier map
  for (uint32_t i = 0; i < args->expert.phy.nof_carriers; i++) {
    carrier_map_t* m = &args->expert.phy.carrier_map[i];
    m->radio_idx     = i / args->rf.nof_rf_channels;
    m->channel_idx   = (i % args->rf.nof_rf_channels) * args->rf.nof_rx_ant;
    phy_log[0]->debug("Mapping carrier %d to channel %d in radio %d\n", i, m->channel_idx, m->radio_idx);
  }

  /* Start Radio */
  char *dev_name = NULL;
  if (args->rf.device_name.compare("auto")) {
    dev_name = (char*) args->rf.device_name.c_str();
  }

  char* dev_args[SRSLTE_MAX_RADIOS] = {NULL};
  for (int i = 0; i < SRSLTE_MAX_RADIOS; i++) {
    if (args->rf.device_args[0].compare("auto")) {
      dev_args[i] = (char*)args->rf.device_args[i].c_str();
    }
  }

  phy_log[0]->console("Opening %d RF devices with %d RF channels...\n",
                      args->rf.nof_radios,
                      args->rf.nof_rf_channels * args->rf.nof_rx_ant);
  for (uint32_t r = 0; r < args->rf.nof_radios; r++) {
    if (!radios[r].init(phy_log[0], dev_args[r], dev_name, args->rf.nof_rf_channels * args->rf.nof_rx_ant)) {
      phy_log[0]->console(
          "Failed to find device %s with args %s\n", args->rf.device_name.c_str(), args->rf.device_args[0].c_str());
      return false;
    }

    // Set RF options
    if (args->rf.time_adv_nsamples.compare("auto")) {
      int t = atoi(args->rf.time_adv_nsamples.c_str());
      radios[r].set_tx_adv(abs(t));
      radios[r].set_tx_adv_neg(t < 0);
    }
    if (args->rf.burst_preamble.compare("auto")) {
      radios[r].set_burst_preamble(atof(args->rf.burst_preamble.c_str()));
    }
    if (args->rf.continuous_tx.compare("auto")) {
      phy_log[0]->console("set continuous %s\n", args->rf.continuous_tx.c_str());
      radios[r].set_continuous_tx(args->rf.continuous_tx.compare("yes") ? false : true);
    }

    // Set PHY options

    if (args->rf.rx_gain < 0) {
      radios[r].start_agc(false);
    } else {
      radios[r].set_rx_gain(args->rf.rx_gain);
    }
    if (args->rf.tx_gain > 0) {
      radios[r].set_tx_gain(args->rf.tx_gain);
    } else {
      radios[r].set_tx_gain(args->rf.rx_gain);
      phy_log[0]->console("\nWarning: TX gain was not set. Using open-loop power control (not working properly)\n\n");
    }

    radios[r].register_error_handler(rf_msg);
    radios[r].set_freq_offset(args->rf.freq_offset);
  }

  if (args->rf.tx_gain > 0) {
    args->expert.phy.ul_pwr_ctrl_en = false;
  } else {
    args->expert.phy.ul_pwr_ctrl_en = true;
  }

  mac.init(&phy, &rlc, &rrc, &mac_log, args->expert.phy.nof_carriers);
  rlc.init(&pdcp, &rrc, this, &rlc_log, &mac, 0 /* RB_ID_SRB0 */);
  pdcp.init(&rlc, &rrc, &gw, &pdcp_log, 0 /* RB_ID_SRB0 */, SECURITY_DIRECTION_UPLINK);
  nas.init(usim, &rrc, &gw, &nas_log, args->nas);
  gw.init(&pdcp, &nas, &gw_log, 3 /* RB_ID_DRB1 */);
  gw.set_netmask(args->expert.ip_netmask);
  gw.set_tundevname(args->expert.ip_devname);

  // set args and initialize RRC
  rrc.init(&phy, &mac, &rlc, &pdcp, &nas, usim, &gw, &mac, &rrc_log, &args->rrc);

  phy.set_earfcn(earfcn_list);

  if (args->rf.dl_freq > 0 && args->rf.ul_freq > 0) {
    phy.force_freq(args->rf.dl_freq, args->rf.ul_freq);
  }

  phy_log[0]->console("Waiting PHY to initialize...\n");
  phy.wait_initialize();

  // Enable AGC once PHY is initialized
  if (args->rf.rx_gain < 0) {
    phy.set_agc_enable(true);
  }

  phy_log[0]->console("...\n");

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
    usim->stop();
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
    for (uint32_t r = 0; r < args->rf.nof_radios; r++) {
      radios[r].stop();
    }

    usleep(1e5);
    if(args->pcap.enable) {
       mac_pcap.close();
    }
    if(args->pcap.nas_enable) {
       nas_pcap.close();
    }
    started = false;
  }
}

bool ue::switch_on() {
  return nas.attach_request();
}

bool ue::switch_off() {
  // stop UL data
  gw.stop();

  // generate detach request
  nas.detach_request();

  // wait for max. 5s for it to be sent (according to TS 24.301 Sec 25.5.2.2)
  const uint32_t RB_ID_SRB1 = 1;
  int cnt = 0, timeout = 5;
  while (rlc.has_data(RB_ID_SRB1) && ++cnt <= timeout) {
    sleep(1);
  }
  bool detach_sent = true;
  if (rlc.has_data(RB_ID_SRB1)) {
    nas_log.warning("Detach couldn't be sent after %ds.\n", timeout);
    detach_sent = false;
  }

  return detach_sent;
}

bool ue::is_attached()
{
  return rrc.is_connected();
}

void ue::start_plot() {
  phy.start_plot();
}

void ue::print_pool() {
  byte_buffer_pool::get_instance()->print_all_buffers();
}

bool ue::get_metrics(ue_metrics_t &m)
{
  bzero(&m, sizeof(ue_metrics_t));
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
void ue::print_mbms()
{
  rrc.print_mbms();
}

bool ue::mbms_service_start(uint32_t serv, uint32_t port)
{
  return rrc.mbms_service_start(serv, port);
}

void ue::rf_msg(srslte_rf_error_t error)
{
  ue_base *ue = ue_base::get_instance(LTE);
  ue->handle_rf_msg(error);
  if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_OVERFLOW) {
    ue->radio_overflow();
  } else
  if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_RX) {
    ue->stop();
    ue->cleanup();
    exit(-1);
  }
}

} // namespace srsue
