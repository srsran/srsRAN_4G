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

#include "srsue/hdr/ue.h"
#include "srslte/build_info.h"
#include "srslte/radio/radio_multi.h"
#include "srslte/srslte.h"
#include "srsue/hdr/phy/phy.h"
#include "srsue/hdr/stack/ue_stack_lte.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <pthread.h>
#include <sstream>
#include <string>

using namespace srslte;

namespace srsue {

ue::ue() : logger(nullptr)
{
  // print build info
  std::cout << std::endl << get_build_string() << std::endl;

  // load FFTW wisdom
  srslte_dft_load();

  // save FFTW wisdom when UE exits
  atexit(srslte_dft_exit);

  pool = byte_buffer_pool::get_instance();
}

ue::~ue()
{
  // destruct stack components before cleaning buffer pool
  stack.reset();
  byte_buffer_pool::cleanup();

}

int ue::init(const all_args_t& args_, srslte::logger* logger_)
{
  logger = logger_;

  // Init UE log
  log.init("UE  ", logger);
  log.set_level(srslte::LOG_LEVEL_INFO);
  log.info("%s", get_build_string().c_str());

  // Validate arguments
  if (parse_args(args_)) {
    log.console("Error processing arguments.\n");
    return SRSLTE_ERROR;
  }

  // Instantiate layers and stack together our UE
  if (args.stack.type == "lte") {
    std::unique_ptr<ue_stack_lte> lte_stack(new ue_stack_lte());
    if (!lte_stack) {
      log.console("Error creating LTE stack instance.\n");
      return SRSLTE_ERROR;
    }

    std::unique_ptr<gw> gw_ptr(new gw());
    if (!gw_ptr) {
      log.console("Error creating a GW instance.\n");
      return SRSLTE_ERROR;
    }

    std::unique_ptr<srsue::phy> lte_phy = std::unique_ptr<srsue::phy>(new srsue::phy(logger));
    if (!lte_phy) {
      log.console("Error creating LTE PHY instance.\n");
      return SRSLTE_ERROR;
    }

    std::unique_ptr<radio_multi> lte_radio = std::unique_ptr<radio_multi>(new radio_multi(logger));
    if (!lte_radio) {
      log.console("Error creating radio multi instance.\n");
      return SRSLTE_ERROR;
    }

    // init layers
    if (lte_radio->init(args.rf, lte_phy.get())) {
      log.console("Error initializing radio.\n");
      return SRSLTE_ERROR;
    }

    if (lte_phy->init(args.phy, lte_stack.get(), lte_radio.get())) {
      log.console("Error initializing PHY.\n");
      return SRSLTE_ERROR;
    }

    if (lte_stack->init(args.stack, logger, lte_phy.get(), gw_ptr.get())) {
      log.console("Error initializing stack.\n");
      return SRSLTE_ERROR;
    }

    if (gw_ptr->init(args.gw, logger, lte_stack.get())) {
      log.console("Error initializing GW.\n");
      return SRSLTE_ERROR;
    }

    // move ownership
    stack   = std::move(lte_stack);
    gw_inst = std::move(gw_ptr);
    phy     = std::move(lte_phy);
    radio   = std::move(lte_radio);
  } else {
    log.console("Invalid stack type %s. Supported values are [lte].\n", args.stack.type.c_str());
    return SRSLTE_ERROR;
  }

  log.console("Waiting PHY to initialize ... ");
  phy->wait_initialize();
  log.console("done!\n");

  return SRSLTE_SUCCESS;
}

int ue::parse_args(const all_args_t& args_)
{
  // set member variable
  args = args_;

  // carry out basic sanity checks
  if (args.stack.rrc.mbms_service_id > -1) {
    if (!args.phy.interpolate_subframe_enabled) {
      log.error("interpolate_subframe_enabled = %d, While using MBMS, "
                "please set interpolate_subframe_enabled to true\n",
                args.phy.interpolate_subframe_enabled);
      return SRSLTE_ERROR;
    }
    if (args.phy.nof_phy_threads > 2) {
      log.error("nof_phy_threads = %d, While using MBMS, please set "
                "number of phy threads to 1 or 2\n",
                args.phy.nof_phy_threads);
      return SRSLTE_ERROR;
    }
    if ((0 == args.phy.snr_estim_alg.find("refs"))) {
      log.error("snr_estim_alg = refs, While using MBMS, please set "
                "algorithm to pss or empty \n");
      return SRSLTE_ERROR;
    }
  }

  // replicate some RF parameter to make them available to PHY
  args.phy.nof_rx_ant  = args.rf.nof_rx_ant;
  args.phy.agc_enable  = args.rf.rx_gain < 0.0f;

  // Calculate number of carriers available in all radios
  args.phy.nof_radios      = args.rf.nof_radios;
  args.phy.nof_rf_channels = args.rf.nof_rf_channels;
  args.phy.nof_carriers    = args.rf.nof_radios * args.rf.nof_rf_channels;

  if (args.phy.nof_carriers > SRSLTE_MAX_CARRIERS) {
    log.error("Too many carriers (%d > %d)\n", args.phy.nof_carriers, SRSLTE_MAX_CARRIERS);
    return SRSLTE_ERROR;
  }

  // Generate RF-Channel to Carrier map
  for (uint32_t i = 0; i < args.phy.nof_carriers; i++) {
    carrier_map_t* m = &args.phy.carrier_map[i];
    m->radio_idx     = i / args.rf.nof_rf_channels;
    m->channel_idx   = (i % args.rf.nof_rf_channels) * args.rf.nof_rx_ant;
    log.debug("Mapping carrier %d to channel %d in radio %d\n", i, m->channel_idx, m->radio_idx);
  }

  // populate EARFCN list
  if (!args.phy.dl_earfcn.empty()) {
    std::stringstream ss(args.phy.dl_earfcn);
    uint32_t          idx = 0;
    while (ss.good()) {
      std::string substr;
      getline(ss, substr, ',');
      auto earfcn                         = (uint32_t)strtoul(substr.c_str(), nullptr, 10);
      args.stack.rrc.supported_bands[idx] = srslte_band_get_band(earfcn);
      args.stack.rrc.nof_supported_bands  = ++idx;
      args.phy.earfcn_list.push_back(earfcn);
    }
  } else {
    log.error("Error: dl_earfcn list is empty\n");
    log.console("Error: dl_earfcn list is empty\n");
    return SRSLTE_ERROR;
  }

  // Set UE category
  args.stack.rrc.ue_category = (uint32_t)strtoul(args.stack.rrc.ue_category_str.c_str(), nullptr, 10);

  // Consider Carrier Aggregation support if more than one
  args.stack.rrc.support_ca = (args.rf.nof_rf_channels * args.rf.nof_radios) > 1;

  return SRSLTE_SUCCESS;
}

void ue::stop()
{
  // tear down UE in reverse order
  if (stack) {
    stack->stop();
  }

  if (gw_inst) {
    gw_inst->stop();
  }

  if (phy) {
    phy->stop();
  }

  if (radio) {
    radio->stop();
  }
}

bool ue::switch_on()
{
  return stack->switch_on();
}

bool ue::switch_off()
{
  if (gw_inst) {
    gw_inst->stop();
  }
  return stack->switch_off();
}

void ue::start_plot()
{
  phy->start_plot();
}

bool ue::get_metrics(ue_metrics_t* m)
{
  bzero(m, sizeof(ue_metrics_t));
  phy->get_metrics(&m->phy);
  radio->get_metrics(&m->rf);
  stack->get_metrics(&m->stack);
  gw_inst->get_metrics(m->gw);
  return true;
}

std::string ue::get_build_mode()
{
  return std::string(srslte_get_build_mode());
}

std::string ue::get_build_info()
{
  if (std::string(srslte_get_build_info()).find("  ") != std::string::npos) {
    return std::string(srslte_get_version());
  }
  return std::string(srslte_get_build_info());
}

std::string ue::get_build_string()
{
  std::stringstream ss;
  ss << "Built in " << get_build_mode() << " mode using " << get_build_info() << "." << std::endl;
  return ss.str();
}

} // namespace srsue
