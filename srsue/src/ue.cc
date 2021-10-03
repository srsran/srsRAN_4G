/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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
#include "srsran/build_info.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"
#include "srsran/radio/radio.h"
#include "srsran/radio/radio_null.h"
#include "srsran/srsran.h"
#include "srsue/hdr/phy/phy.h"
#include "srsue/hdr/stack/ue_stack_lte.h"
#include "srsue/hdr/stack/ue_stack_nr.h"
#include <algorithm>
#include <iostream>
#include <string>

using namespace srsran;

namespace srsue {

ue::ue() : logger(srslog::fetch_basic_logger("UE", false)), sys_proc(logger)
{
  // print build info
  std::cout << std::endl << get_build_string() << std::endl << std::endl;
}

ue::~ue()
{
  stack.reset();
}

int ue::init(const all_args_t& args_)
{
  int ret = SRSRAN_SUCCESS;

  // Init UE log
  logger.set_level(srslog::basic_levels::info);
  logger.info("%s", get_build_string().c_str());

  // Validate arguments
  if (parse_args(args_)) {
    srsran::console("Error processing arguments. Please check %s for more details.\n", args_.log.filename.c_str());
    return SRSRAN_ERROR;
  }

  // Instantiate layers and stack together our UE
  std::unique_ptr<ue_stack_lte> lte_stack(new ue_stack_lte);
  if (!lte_stack) {
    srsran::console("Error creating LTE stack instance.\n");
    return SRSRAN_ERROR;
  }

  std::unique_ptr<gw> gw_ptr(new gw(srslog::fetch_basic_logger("GW")));
  if (!gw_ptr) {
    srsran::console("Error creating a GW instance.\n");
    return SRSRAN_ERROR;
  }

  std::unique_ptr<srsue::phy> lte_phy = std::unique_ptr<srsue::phy>(new srsue::phy);
  if (!lte_phy) {
    srsran::console("Error creating LTE PHY instance.\n");
    return SRSRAN_ERROR;
  }

  std::unique_ptr<srsran::radio> lte_radio = std::unique_ptr<srsran::radio>(new srsran::radio);
  if (!lte_radio) {
    srsran::console("Error creating radio multi instance.\n");
    return SRSRAN_ERROR;
  }

  // init layers
  if (lte_radio->init(args.rf, lte_phy.get())) {
    srsran::console("Error initializing radio.\n");
    return SRSRAN_ERROR;
  }

  // from here onwards do not exit immediately if something goes wrong as sub-layers may already use interfaces
  if (lte_phy->init(args.phy, lte_stack.get(), lte_radio.get())) {
    srsran::console("Error initializing PHY.\n");
    ret = SRSRAN_ERROR;
  }

  srsue::phy_args_nr_t phy_args_nr = {};
  phy_args_nr.max_nof_prb          = args.phy.nr_max_nof_prb;
  phy_args_nr.rf_channel_offset    = args.phy.nof_lte_carriers;
  phy_args_nr.nof_carriers         = args.phy.nof_nr_carriers;
  phy_args_nr.nof_phy_threads      = args.phy.nof_phy_threads;
  phy_args_nr.worker_cpu_mask      = args.phy.worker_cpu_mask;
  phy_args_nr.log                  = args.phy.log;
  phy_args_nr.store_pdsch_ko       = args.phy.nr_store_pdsch_ko;
  if (lte_phy->init(phy_args_nr, lte_stack.get(), lte_radio.get())) {
    srsran::console("Error initializing NR PHY.\n");
    ret = SRSRAN_ERROR;
  }

  if (lte_stack->init(args.stack, lte_phy.get(), lte_phy.get(), gw_ptr.get())) {
    srsran::console("Error initializing stack.\n");
    ret = SRSRAN_ERROR;
  }

  if (gw_ptr->init(args.gw, lte_stack.get())) {
    srsran::console("Error initializing GW.\n");
    ret = SRSRAN_ERROR;
  }

  // move ownership
  stack   = std::move(lte_stack);
  gw_inst = std::move(gw_ptr);
  phy     = std::move(lte_phy);
  radio   = std::move(lte_radio);

  if (phy) {
    srsran::console("Waiting PHY to initialize ... ");
    phy->wait_initialize();
    srsran::console("done!\n");
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
      return SRSRAN_ERROR;
    }
    if (args.phy.nof_phy_threads > 2) {
      logger.error("nof_phy_threads = %d, While using MBMS, please set "
                   "number of phy threads to 1 or 2",
                   args.phy.nof_phy_threads);
      return SRSRAN_ERROR;
    }
    if ((0 == args.phy.snr_estim_alg.find("refs"))) {
      logger.error("snr_estim_alg = refs, While using MBMS, please set "
                   "algorithm to pss or empty");
      return SRSRAN_ERROR;
    }
  }

  if (args.rf.nof_antennas > SRSRAN_MAX_PORTS) {
    fprintf(stderr, "Maximum number of antennas exceeded (%d > %d)\n", args.rf.nof_antennas, SRSRAN_MAX_PORTS);
    return SRSRAN_ERROR;
  }

  args.rf.nof_carriers = args.phy.nof_lte_carriers + args.phy.nof_nr_carriers;

  if (args.rf.nof_carriers > SRSRAN_MAX_CARRIERS) {
    fprintf(stderr,
            "Maximum number of carriers exceeded (%d > %d) (nof_lte_carriers %d + nof_nr_carriers %d)\n",
            args.rf.nof_carriers,
            SRSRAN_MAX_CARRIERS,
            args.phy.nof_lte_carriers,
            args.phy.nof_nr_carriers);
    return SRSRAN_ERROR;
  }

  // replicate some RF parameter to make them available to PHY
  args.phy.nof_rx_ant = args.rf.nof_antennas;
  args.phy.agc_enable = args.rf.rx_gain < 0.0f;

  // populate DL EARFCN list
  if (not args.phy.dl_earfcn.empty()) {
    // Parse DL-EARFCN list
    srsran::string_parse_list(args.phy.dl_earfcn, ',', args.phy.dl_earfcn_list);

    // Populates supported bands
    args.stack.rrc.nof_supported_bands = 0;
    for (uint32_t& earfcn : args.phy.dl_earfcn_list) {
      uint8_t band = srsran_band_get_band(earfcn);
      // Try to find band, if not appends it
      if (std::find(args.stack.rrc.supported_bands.begin(), args.stack.rrc.supported_bands.end(), band) ==
          args.stack.rrc.supported_bands.end()) {
        args.stack.rrc.supported_bands[args.stack.rrc.nof_supported_bands++] = band;
      }
      // RRC NR needs also information about supported eutra bands
      if (std::find(args.stack.rrc_nr.supported_bands_eutra.begin(),
                    args.stack.rrc_nr.supported_bands_eutra.end(),
                    band) == args.stack.rrc_nr.supported_bands_eutra.end()) {
        args.stack.rrc_nr.supported_bands_eutra.push_back(band);
      }
    }
  } else {
    logger.error("Error: dl_earfcn list is empty");
    srsran::console("Error: dl_earfcn list is empty\n");
    return SRSRAN_ERROR;
  }

  // populate UL EARFCN list
  if (not args.phy.ul_earfcn.empty()) {
    std::vector<uint32_t> ul_earfcn_list;
    srsran::string_parse_list(args.phy.ul_earfcn, ',', ul_earfcn_list);

    // For each parsed UL-EARFCN links it to the corresponding DL-EARFCN
    args.phy.ul_earfcn_map.clear();
    for (size_t i = 0; i < SRSRAN_MIN(ul_earfcn_list.size(), args.phy.dl_earfcn_list.size()); i++) {
      args.phy.ul_earfcn_map[args.phy.dl_earfcn_list[i]] = ul_earfcn_list[i];
    }
  }

  // populate NR DL ARFCNs
  if (args.phy.nof_nr_carriers > 0) {
    if (not args.stack.rrc_nr.supported_bands_nr_str.empty()) {
      // Populates supported bands
      srsran::string_parse_list(args.stack.rrc_nr.supported_bands_nr_str, ',', args.stack.rrc_nr.supported_bands_nr);
      args.stack.rrc.supported_bands_nr = args.stack.rrc_nr.supported_bands_nr;
    } else {
      logger.error("Error: rat.nr.bands list is empty");
      srsran::console("Error: rat.nr.bands list is empty\n");
      return SRSRAN_ERROR;
    }
  }

  // Set UE category
  args.stack.rrc.ue_category = (uint32_t)strtoul(args.stack.rrc.ue_category_str.c_str(), nullptr, 10);

  // Consider Carrier Aggregation support if more than one
  args.stack.rrc.support_ca = (args.phy.nof_lte_carriers > 1);

  return SRSRAN_SUCCESS;
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

  // send switch off
  stack->switch_off();

  // wait for max. 5s for it to be sent (according to TS 24.301 Sec 25.5.2.2)
  int             cnt = 0, timeout_s = 5;
  stack_metrics_t metrics = {};
  stack->get_metrics(&metrics);

  while (metrics.rrc.state != RRC_STATE_IDLE && ++cnt <= timeout_s) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    stack->get_metrics(&metrics);
  }

  if (metrics.rrc.state != RRC_STATE_IDLE) {
    srslog::fetch_basic_logger("NAS").warning("Detach couldn't be sent after %ds.", timeout_s);
    return false;
  }

  return true;
}

void ue::start_plot()
{
  phy->start_plot();
}

bool ue::get_metrics(ue_metrics_t* m)
{
  bzero(m, sizeof(ue_metrics_t));
  phy->get_metrics(srsran::srsran_rat_t::lte, &m->phy);
  phy->get_metrics(srsran::srsran_rat_t::nr, &m->phy_nr);
  radio->get_metrics(&m->rf);
  stack->get_metrics(&m->stack);
  gw_inst->get_metrics(m->gw, m->stack.mac[0].nof_tti);
  m->sys = sys_proc.get_metrics();
  return true;
}

std::string ue::get_build_mode()
{
  return std::string(srsran_get_build_mode());
}

std::string ue::get_build_info()
{
  if (std::string(srsran_get_build_info()).find("  ") != std::string::npos) {
    return std::string(srsran_get_version());
  }
  return std::string(srsran_get_build_info());
}

std::string ue::get_build_string()
{
  std::stringstream ss;
  ss << "Built in " << get_build_mode() << " mode using " << get_build_info() << ".";
  return ss.str();
}

} // namespace srsue
