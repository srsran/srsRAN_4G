/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsue/hdr/ue.h"
#include "srslte/build_info.h"
#include "srslte/common/band_helper.h"
#include "srslte/common/string_helpers.h"
#include "srslte/radio/radio.h"
#include "srslte/radio/radio_null.h"
#include "srslte/srslte.h"
#include "srsue/hdr/phy/phy.h"
#include "srsue/hdr/phy/vnf_phy_nr.h"
#include "srsue/hdr/stack/ue_stack_lte.h"
#include "srsue/hdr/stack/ue_stack_nr.h"
#include <algorithm>
#include <iostream>
#include <string>

using namespace srslte;

namespace srsue {

ue::ue() : old_logger(nullptr), logger(srslog::fetch_basic_logger("UE", false))
{
  // print build info
  std::cout << std::endl << get_build_string() << std::endl << std::endl;
}

ue::~ue()
{
  stack.reset();
}

int ue::init(const all_args_t& args_, srslte::logger* logger_)
{
  int ret    = SRSLTE_SUCCESS;
  old_logger = logger_;

  // Init UE log
  logger.set_level(srslog::basic_levels::info);
  logger.info("%s", get_build_string().c_str());

  // Validate arguments
  if (parse_args(args_)) {
    srslte::console("Error processing arguments. Please check %s for more details.\n", args_.log.filename.c_str());
    return SRSLTE_ERROR;
  }

  // Instantiate layers and stack together our UE
  if (args.stack.type == "lte") {
    std::unique_ptr<ue_stack_lte> lte_stack(new ue_stack_lte);
    if (!lte_stack) {
      srslte::console("Error creating LTE stack instance.\n");
      return SRSLTE_ERROR;
    }

    std::unique_ptr<gw> gw_ptr(new gw());
    if (!gw_ptr) {
      srslte::console("Error creating a GW instance.\n");
      return SRSLTE_ERROR;
    }

    std::unique_ptr<srsue::phy> lte_phy = std::unique_ptr<srsue::phy>(new srsue::phy);
    if (!lte_phy) {
      srslte::console("Error creating LTE PHY instance.\n");
      return SRSLTE_ERROR;
    }

    std::unique_ptr<srslte::radio> lte_radio = std::unique_ptr<srslte::radio>(new srslte::radio);
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

    srsue::phy_args_nr_t phy_args_nr = {};
    phy_args_nr.nof_prb              = args.phy.nr_nof_prb;
    phy_args_nr.nof_carriers         = args.phy.nof_nr_carriers;
    phy_args_nr.nof_phy_threads      = args.phy.nof_phy_threads;
    phy_args_nr.worker_cpu_mask      = args.phy.worker_cpu_mask;
    phy_args_nr.log                  = args.phy.log;
    if (lte_phy->init(phy_args_nr, lte_stack.get(), lte_radio.get())) {
      srslte::console("Error initializing NR PHY.\n");
      ret = SRSLTE_ERROR;
    }

    if (lte_stack->init(args.stack, lte_phy.get(), lte_phy.get(), gw_ptr.get())) {
      srslte::console("Error initializing stack.\n");
      ret = SRSLTE_ERROR;
    }

    if (gw_ptr->init(args.gw, lte_stack.get())) {
      srslte::console("Error initializing GW.\n");
      ret = SRSLTE_ERROR;
    }

    // move ownership
    stack   = std::move(lte_stack);
    gw_inst = std::move(gw_ptr);
    phy     = std::move(lte_phy);
    radio   = std::move(lte_radio);
  } else if (args.stack.type == "nr") {
    logger.info("Initializing NR stack");
    std::unique_ptr<srsue::ue_stack_nr> nr_stack(new srsue::ue_stack_nr(old_logger));
    std::unique_ptr<srslte::radio_null> nr_radio(new srslte::radio_null);
    std::unique_ptr<srsue::vnf_phy_nr>  nr_phy(new srsue::vnf_phy_nr);
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

    if (gw_ptr->init(args.gw, nr_stack.get())) {
      srslte::console("Error initializing GW.\n");
      return SRSLTE_ERROR;
    }

    // move ownership
    stack   = std::move(nr_stack);
    gw_inst = std::move(gw_ptr);
    phy     = std::move(nr_phy);
    radio   = std::move(nr_radio);
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
      logger.error("interpolate_subframe_enabled = %d, While using MBMS, "
                   "please set interpolate_subframe_enabled to true",
                   args.phy.interpolate_subframe_enabled);
      return SRSLTE_ERROR;
    }
    if (args.phy.nof_phy_threads > 2) {
      logger.error("nof_phy_threads = %d, While using MBMS, please set "
                   "number of phy threads to 1 or 2",
                   args.phy.nof_phy_threads);
      return SRSLTE_ERROR;
    }
    if ((0 == args.phy.snr_estim_alg.find("refs"))) {
      logger.error("snr_estim_alg = refs, While using MBMS, please set "
                   "algorithm to pss or empty");
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

  if (args.rf.nof_carriers <= args.phy.nof_nr_carriers) {
    fprintf(stderr,
            "Maximum number of carriers enough for NR and LTE (%d <= %d)\n",
            args.rf.nof_carriers,
            args.phy.nof_nr_carriers);
    return SRSLTE_ERROR;
  }

  // replicate some RF parameter to make them available to PHY
  args.phy.nof_lte_carriers = args.rf.nof_carriers - args.phy.nof_nr_carriers;
  args.phy.nof_rx_ant       = args.rf.nof_antennas;
  args.phy.agc_enable       = args.rf.rx_gain < 0.0f;

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
    logger.error("Error: dl_earfcn list is empty");
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

  srslte_band_helper bands_helper;

  // populate NR DL ARFCNs
  if (args.phy.nof_nr_carriers > 0) {
    if (not args.phy.dl_nr_arfcn.empty()) {
      // Parse list
      srslte::string_parse_list(args.phy.dl_nr_arfcn, ',', args.phy.dl_nr_arfcn_list);

      // Populates supported bands
      for (uint32_t& arfcn : args.phy.dl_nr_arfcn_list) {
        std::vector<uint32_t> bands = bands_helper.get_bands_nr(arfcn);
        for (const auto& band : bands) {
          // make sure we don't add duplicates
          if (std::find(args.stack.rrc_nr.supported_bands.begin(), args.stack.rrc_nr.supported_bands.end(), band) ==
              args.stack.rrc_nr.supported_bands.end()) {
            args.stack.rrc_nr.supported_bands.push_back(band);
          }
        }
      }
    } else {
      logger.error("Error: dl_nr_arfcn list is empty");
      srslte::console("Error: dl_nr_arfcn list is empty\n");
      return SRSLTE_ERROR;
    }
  }

  // Set UE category
  args.stack.rrc.ue_category = (uint32_t)strtoul(args.stack.rrc.ue_category_str.c_str(), nullptr, 10);

  // Consider Carrier Aggregation support if more than one
  args.stack.rrc.support_ca = (args.phy.nof_lte_carriers > 1);

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
  gw_inst->get_metrics(m->gw, m->stack.mac[0].nof_tti);
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
  ss << "Built in " << get_build_mode() << " mode using " << get_build_info() << ".";
  return ss.str();
}

} // namespace srsue
