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

#include "srsenb/hdr/enb.h"
#include "srsenb/hdr/radio/enb_radio_multi.h"
#include "srsenb/hdr/stack/enb_stack_lte.h"
#include "srsenb/src/enb_cfg_parser.h"
#include "srslte/build_info.h"
#include <boost/algorithm/string.hpp>
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
  return (instance);
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

enb::enb() : started(false), pool(srslte::byte_buffer_pool::get_instance(ENB_POOL_SIZE))
{
  // print build info
  std::cout << std::endl << get_build_string() << std::endl;

  srslte_dft_load();
}

enb::~enb() {}

int enb::init(const all_args_t& args_)
{
  // Init UE log
  log.init("UE  ", logger);
  log.set_level(srslte::LOG_LEVEL_INFO);
  log.info("%s", get_build_string().c_str());

  // Validate arguments
  if (parse_args(args_)) {
    log.console("Error processing arguments.\n");
    return SRSLTE_ERROR;
  }

  // set logger
  if (args.log.filename == "stdout") {
    logger = &logger_stdout;
  } else {
    logger_file.init(args.log.filename, args.log.file_max_size);
    logger_file.log_char("\n\n");
    logger_file.log_char(get_build_string().c_str());
    logger = &logger_file;
  }

  pool_log.init("POOL", logger);
  pool_log.set_level(srslte::LOG_LEVEL_ERROR);
  pool->set_log(&pool_log);

  // Create layers
  std::unique_ptr<enb_stack_lte> lte_stack(new enb_stack_lte(logger));
  if (!lte_stack) {
    log.console("Error creating eNB stack.\n");
    return SRSLTE_ERROR;
  }

  std::unique_ptr<enb_radio_multi> lte_radio = std::unique_ptr<enb_radio_multi>(new enb_radio_multi(logger));
  if (!lte_radio) {
    log.console("Error creating radio multi instance.\n");
    return SRSLTE_ERROR;
  }

  std::unique_ptr<srsenb::phy> lte_phy = std::unique_ptr<srsenb::phy>(new srsenb::phy(logger));
  if (!lte_phy) {
    log.console("Error creating LTE PHY instance.\n");
    return SRSLTE_ERROR;
  }

  // Init layers
  if (lte_radio->init(args.rf, lte_phy.get())) {
    log.console("Error initializing radio.\n");
    return SRSLTE_ERROR;
  }

  if (lte_phy->init(args.phy, phy_cfg, lte_radio.get(), lte_stack.get())) {
    log.console("Error initializing PHY.\n");
    return SRSLTE_ERROR;
  }

  if (lte_stack->init(args.stack, rrc_cfg, lte_phy.get())) {
    log.console("Error initializing stack.\n");
    return SRSLTE_ERROR;
  }

  stack = std::move(lte_stack);
  phy   = std::move(lte_phy);
  radio = std::move(lte_radio);

  log.console("\n==== eNodeB started ===\n");
  log.console("Type <t> to view trace\n");

  started = true;

  return SRSLTE_SUCCESS;
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

srslte::LOG_LEVEL_ENUM enb::level(std::string l)
{
  boost::to_upper(l);
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
