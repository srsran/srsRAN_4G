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

#include "srsenb/hdr/enb.h"
#include "srsenb/hdr/stack/enb_stack_lte.h"
#include "srsenb/src/enb_cfg_parser.h"
#include "srslte/build_info.h"
#include "srslte/radio/radio_null.h"
#ifdef HAVE_5GNR
#include "srsenb/hdr/phy/vnf_phy_nr.h"
#include "srsenb/hdr/stack/gnb_stack_nr.h"
#endif
#include <iostream>

namespace srsenb {

enb::enb() : started(false), pool(srslte::byte_buffer_pool::get_instance(ENB_POOL_SIZE))
{
  // print build info
  std::cout << std::endl << get_build_string() << std::endl;
}

enb::~enb()
{
  stack.reset();
}

int enb::init(const all_args_t& args_, srslte::logger* logger_)
{
  int ret = SRSLTE_SUCCESS;
  logger  = logger_;

  // Init eNB log
  srslte::logmap::set_default_logger(logger);
  log = srslte::logmap::get("ENB");
  log->set_level(srslte::LOG_LEVEL_INFO);
  log->info("%s", get_build_string().c_str());

  // Validate arguments
  if (parse_args(args_)) {
    srslte::console("Error processing arguments.\n");
    return SRSLTE_ERROR;
  }

  pool_log.init("POOL", logger);
  pool_log.set_level(srslte::LOG_LEVEL_ERROR);
  pool->set_log(&pool_log);

  // Create layers
  if (args.stack.type == "lte") {
    std::unique_ptr<enb_stack_lte> lte_stack(new enb_stack_lte(logger));
    if (!lte_stack) {
      srslte::console("Error creating eNB stack.\n");
      return SRSLTE_ERROR;
    }

    std::unique_ptr<srslte::radio> lte_radio = std::unique_ptr<srslte::radio>(new srslte::radio(logger));
    if (!lte_radio) {
      srslte::console("Error creating radio multi instance.\n");
      return SRSLTE_ERROR;
    }

    std::unique_ptr<srsenb::phy> lte_phy = std::unique_ptr<srsenb::phy>(new srsenb::phy(logger));
    if (!lte_phy) {
      srslte::console("Error creating LTE PHY instance.\n");
      return SRSLTE_ERROR;
    }

    // Init Radio
    if (lte_radio->init(args.rf, lte_phy.get())) {
      srslte::console("Error initializing radio.\n");
      return SRSLTE_ERROR;
    }

    // Only Init PHY if radio couldn't be initialized
    if (ret == SRSLTE_SUCCESS) {
      if (lte_phy->init(args.phy, phy_cfg, lte_radio.get(), lte_stack.get())) {
        srslte::console("Error initializing PHY.\n");
        ret = SRSLTE_ERROR;
      }
    }

    // Only init Stack if both radio and PHY could be initialized
    if (ret == SRSLTE_SUCCESS) {
      if (lte_stack->init(args.stack, rrc_cfg, lte_phy.get())) {
        srslte::console("Error initializing stack.\n");
        ret = SRSLTE_ERROR;
      }
    }

    stack = std::move(lte_stack);
    phy   = std::move(lte_phy);
    radio = std::move(lte_radio);

  } else if (args.stack.type == "nr") {
#ifdef HAVE_5GNR
    std::unique_ptr<srsenb::gnb_stack_nr> nr_stack(new srsenb::gnb_stack_nr(logger));
    std::unique_ptr<srslte::radio_null>   nr_radio(new srslte::radio_null(logger));
    std::unique_ptr<srsenb::vnf_phy_nr>   nr_phy(new srsenb::vnf_phy_nr(logger));

    // Init layers
    if (nr_radio->init(args.rf, nullptr)) {
      srslte::console("Error initializing radio.\n");
      return SRSLTE_ERROR;
    }

    // TODO: where do we put this?
    srsenb::nr_phy_cfg_t nr_phy_cfg = {};

    args.phy.vnf_args.type          = "gnb";
    args.phy.vnf_args.log_level     = args.phy.log.phy_level;
    args.phy.vnf_args.log_hex_limit = args.phy.log.phy_hex_limit;
    if (nr_phy->init(args.phy, nr_phy_cfg, nr_stack.get())) {
      srslte::console("Error initializing PHY.\n");
      return SRSLTE_ERROR;
    }

    // Same here, where do we put this?
    srsenb::rrc_nr_cfg_t rrc_nr_cfg = {};
    rrc_nr_cfg.coreless             = args.stack.coreless;

    if (nr_stack->init(args.stack, rrc_nr_cfg, nr_phy.get())) {
      srslte::console("Error initializing stack.\n");
      return SRSLTE_ERROR;
    }

    stack = std::move(nr_stack);
    phy   = std::move(nr_phy);
    radio = std::move(nr_radio);
#else
    srslte::console("ERROR: 5G NR stack not compiled. Please, activate CMAKE HAVE_5GNR flag.\n");
    log->error("5G NR stack not compiled. Please, activate CMAKE HAVE_5GNR flag.\n");
#endif
  }

  started = true; // set to true in any case to allow stopping the eNB if an error happened

  if (ret == SRSLTE_SUCCESS) {
    srslte::console("\n==== eNodeB started ===\n");
    srslte::console("Type <t> to view trace\n");
  } else {
    // if any of the layers failed to start, make sure the rest is stopped in a controlled manner
    stop();
  }
  return ret;
}

void enb::stop()
{
  if (started) {
    // tear down in reverse order
    if (phy) {
      phy->stop();
    }

    if (stack) {
      stack->stop();
    }

    if (radio) {
      radio->stop();
    }

    started = false;
  }
}

int enb::parse_args(const all_args_t& args_)
{
  // set member variable
  args = args_;
  return enb_conf_sections::parse_cfg_files(&args, &rrc_cfg, &phy_cfg);
}

void enb::start_plot()
{
  phy->start_plot();
}

void enb::print_pool()
{
  srslte::byte_buffer_pool::get_instance()->print_all_buffers();
}

bool enb::get_metrics(enb_metrics_t* m)
{
  radio->get_metrics(&m->rf);
  phy->get_metrics(m->phy);
  stack->get_metrics(&m->stack);
  m->running = started;
  return true;
}

void enb::cmd_cell_gain(uint32_t cell_id, float gain)
{
  phy->cmd_cell_gain(cell_id, gain);
}

srslte::LOG_LEVEL_ENUM enb::level(std::string l)
{
  std::transform(l.begin(), l.end(), l.begin(), ::toupper);
  if ("NONE" == l) {
    return srslte::LOG_LEVEL_NONE;
  } else if ("ERROR" == l) {
    return srslte::LOG_LEVEL_ERROR;
  } else if ("WARNING" == l) {
    return srslte::LOG_LEVEL_WARNING;
  } else if ("INFO" == l) {
    return srslte::LOG_LEVEL_INFO;
  } else if ("DEBUG" == l) {
    return srslte::LOG_LEVEL_DEBUG;
  } else {
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
