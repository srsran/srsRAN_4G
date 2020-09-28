/*
 * Copyright 2013-2020 Software Radio Systems Limited
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
#include "srslte/common/string_helpers.h"
#include "srslte/radio/radio.h"
#include "srslte/radio/radio_null.h"
#include "srslte/srslte.h"
#include "srsue/hdr/phy/phy.h"
#include "srsue/hdr/stack/ue_stack_lte.h"
#ifdef HAVE_5GNR
#include "srsue/hdr/phy/vnf_phy_nr.h"
#include "srsue/hdr/stack/ue_stack_nr.h"
#endif
#include <algorithm>
#include <iostream>
#include <string>

using namespace srslte;

namespace srsue {

ue::ue() : logger(nullptr)
{
  // print build info
  std::cout << std::endl << get_build_string() << std::endl;
  pool = byte_buffer_pool::get_instance();
}

ue::~ue()
{
  stack.reset();
}

int ue::init(const all_args_t& args_, srslte::logger* logger_)
{
  int ret = SRSLTE_SUCCESS;
  logger  = logger_;

  // Init UE log
  log.init("UE  ", logger);
  log.set_level(srslte::LOG_LEVEL_INFO);
  log.info("%s", get_build_string().c_str());

  // Validate arguments
  if (parse_args(args_)) {
    srslte::console("Error processing arguments. Please check %s for more details.\n", args_.log.filename.c_str());
    return SRSLTE_ERROR;
  }

  // Instantiate layers and stack together our UE
  if (args.stack.type == "lte") {
    std::unique_ptr<ue_stack_lte> lte_stack(new ue_stack_lte());
    if (!lte_stack) {
      srslte::console("Error creating LTE stack instance.\n");
      return SRSLTE_ERROR;
    }

    std::unique_ptr<gw> gw_ptr(new gw());
    if (!gw_ptr) {
      srslte::console("Error creating a GW instance.\n");
      return SRSLTE_ERROR;
    }

    std::unique_ptr<srsue::phy> lte_phy = std::unique_ptr<srsue::phy>(new srsue::phy(logger));
    if (!lte_phy) {
      srslte::console("Error creating LTE PHY instance.\n");
      return SRSLTE_ERROR;
    }

    std::unique_ptr<srslte::radio> lte_radio = std::unique_ptr<srslte::radio>(new srslte::radio(logger));
    if (!lte_radio) {
      srslte::console("Error creating radio multi instance.\n");
      return SRSLTE_ERROR;
    }

    // init layers
    if (lte_radio->init(args.rf, lte_phy.get())) {
      srslte::console("Error initializing radio.\n");
      return SRSLTE_ERROR;
    }

    // from here onwards do not exit immediately if something goes wrong as sub-layers may already use interfaces
    if (lte_phy->init(args.phy, lte_stack.get(), lte_radio.get())) {
      srslte::console("Error initializing PHY.\n");
      ret = SRSLTE_ERROR;
    }

    if (lte_stack->init(args.stack, logger, lte_phy.get(), gw_ptr.get())) {
      srslte::console("Error initializing stack.\n");
      ret = SRSLTE_ERROR;
    }

    if (gw_ptr->init(args.gw, logger, lte_stack.get())) {
      srslte::console("Error initializing GW.\n");
      ret = SRSLTE_ERROR;
    }

    // move ownership
    stack   = std::move(lte_stack);
    gw_inst = std::move(gw_ptr);
    phy     = std::move(lte_phy);
    radio   = std::move(lte_radio);
  } else if (args.stack.type == "nr") {
    log.info("Initializing NR stack.\n");
#ifdef HAVE_5GNR
    std::unique_ptr<srsue::ue_stack_nr> nr_stack(new srsue::ue_stack_nr(logger));
    std::unique_ptr<srslte::radio_null> nr_radio(new srslte::radio_null(logger));
    std::unique_ptr<srsue::vnf_phy_nr>  nr_phy(new srsue::vnf_phy_nr(logger));
    std::unique_ptr<gw>                 gw_ptr(new gw());

    // Init layers
    if (nr_radio->init(args.rf, nullptr)) {
      srslte::console("Error initializing radio.\n");
      return SRSLTE_ERROR;
    }

    if (nr_phy->init(args.phy, nr_stack.get())) {
      srslte::console("Error initializing PHY.\n");
      return SRSLTE_ERROR;
    }

    if (nr_stack->init(args.stack, nr_phy.get(), gw_ptr.get())) {
      srslte::console("Error initializing stack.\n");
      return SRSLTE_ERROR;
    }

    if (gw_ptr->init(args.gw, logger, nr_stack.get())) {
      srslte::console("Error initializing GW.\n");
      return SRSLTE_ERROR;
    }

    // move ownership
    stack   = std::move(nr_stack);
    gw_inst = std::move(gw_ptr);
    phy     = std::move(nr_phy);
    radio   = std::move(nr_radio);
#else
    srslte::console("ERROR: 5G NR stack not compiled. Please, activate CMAKE HAVE_5GNR flag.\n");
    log.error("5G NR stack not compiled. Please, activate CMAKE HAVE_5GNR flag.\n");
#endif
  } else {
    srslte::console("Invalid stack type %s. Supported values are [lte].\n", args.stack.type.c_str());
    ret = SRSLTE_ERROR;
  }

  if (phy) {
    srslte::console("Waiting PHY to initialize ... ");
    phy->wait_initialize();
    srslte::console("done!\n");
  }

  return ret;
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

  if (args.rf.nof_antennas > SRSLTE_MAX_PORTS) {
    fprintf(stderr, "Maximum number of antennas exceeded (%d > %d)\n", args.rf.nof_antennas, SRSLTE_MAX_PORTS);
    return SRSLTE_ERROR;
  }

  if (args.rf.nof_carriers > SRSLTE_MAX_CARRIERS) {
    fprintf(stderr, "Maximum number of carriers exceeded (%d > %d)\n", args.rf.nof_carriers, SRSLTE_MAX_CARRIERS);
    return SRSLTE_ERROR;
  }

  // replicate some RF parameter to make them available to PHY
  args.phy.nof_carriers = args.rf.nof_carriers;
  args.phy.nof_rx_ant   = args.rf.nof_antennas;
  args.phy.agc_enable   = args.rf.rx_gain < 0.0f;

  // populate DL EARFCN list
  if (not args.phy.dl_earfcn.empty()) {
    // Parse DL-EARFCN list
    srslte::string_parse_list(args.phy.dl_earfcn, ',', args.phy.dl_earfcn_list);

    // Populates supported bands
    args.stack.rrc.nof_supported_bands = 0;
    for (uint32_t& earfcn : args.phy.dl_earfcn_list) {
      uint8_t band = srslte_band_get_band(earfcn);
      // Try to find band, if not appends it
      if (std::find(args.stack.rrc.supported_bands.begin(), args.stack.rrc.supported_bands.end(), band) ==
          args.stack.rrc.supported_bands.end()) {
        args.stack.rrc.supported_bands[args.stack.rrc.nof_supported_bands++] = band;
      }
    }
  } else {
    log.error("Error: dl_earfcn list is empty\n");
    srslte::console("Error: dl_earfcn list is empty\n");
    return SRSLTE_ERROR;
  }

  // populate UL EARFCN list
  if (not args.phy.ul_earfcn.empty()) {
    std::vector<uint32_t> ul_earfcn_list;
    srslte::string_parse_list(args.phy.ul_earfcn, ',', ul_earfcn_list);

    // For each parsed UL-EARFCN links it to the corresponding DL-EARFCN
    args.phy.ul_earfcn_map.clear();
    for (size_t i = 0; i < SRSLTE_MIN(ul_earfcn_list.size(), args.phy.dl_earfcn_list.size()); i++) {
      args.phy.ul_earfcn_map[args.phy.dl_earfcn_list[i]] = ul_earfcn_list[i];
    }
  }

  // Set UE category
  args.stack.rrc.ue_category = (uint32_t)strtoul(args.stack.rrc.ue_category_str.c_str(), nullptr, 10);

  // Consider Carrier Aggregation support if more than one
  args.stack.rrc.support_ca = (args.rf.nof_carriers > 1);

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
