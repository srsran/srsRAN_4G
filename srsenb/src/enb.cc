/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsenb/hdr/enb.h"
#include "srsenb/hdr/stack/enb_stack_lte.h"
#include "srsenb/hdr/x2_adapter.h"
#include "srsenb/src/enb_cfg_parser.h"
#include "srsgnb/hdr/stack/gnb_stack_nr.h"
#include "srsran/build_info.h"
#include "srsran/common/enb_events.h"
#include "srsran/radio/radio_null.h"
#include <iostream>

namespace srsenb {

enb::enb(srslog::sink& log_sink) :
  started(false), log_sink(log_sink), enb_log(srslog::fetch_basic_logger("ENB", log_sink, false)), sys_proc(enb_log)
{
  // print build info
  std::cout << std::endl << get_build_string() << std::endl << std::endl;
}

enb::~enb()
{
  eutra_stack.reset();
  nr_stack.reset();
}

int enb::init(const all_args_t& args_)
{
  int ret = SRSRAN_SUCCESS;

  // Init eNB log
  enb_log.set_level(srslog::basic_levels::info);
  enb_log.info("%s", get_build_string().c_str());

  // Validate arguments
  if (parse_args(args_, rrc_cfg, rrc_nr_cfg)) {
    srsran::console("Error processing arguments.\n");
    return SRSRAN_ERROR;
  }

  srsran::byte_buffer_pool::get_instance()->enable_logger(true);

  // Create layers
  std::unique_ptr<enb_stack_lte> tmp_eutra_stack;
  if (not rrc_cfg.cell_list.empty()) {
    // add EUTRA stack
    tmp_eutra_stack.reset(new enb_stack_lte(log_sink));
    if (tmp_eutra_stack == nullptr) {
      srsran::console("Error creating EUTRA stack.\n");
      return SRSRAN_ERROR;
    }
  }

  std::unique_ptr<gnb_stack_nr> tmp_nr_stack;
  if (not rrc_nr_cfg.cell_list.empty()) {
    // add NR stack
    tmp_nr_stack.reset(new gnb_stack_nr(log_sink));
    if (tmp_nr_stack == nullptr) {
      srsran::console("Error creating NR stack.\n");
      return SRSRAN_ERROR;
    }
  }

  // If NR and EUTRA stacks were initiated, create an X2 adapter between the two.
  if (tmp_nr_stack != nullptr and tmp_eutra_stack != nullptr) {
    x2.reset(new x2_adapter(tmp_eutra_stack.get(), tmp_nr_stack.get()));
  }

  // Radio and PHY are RAT agnostic
  std::unique_ptr<srsran::radio> tmp_radio = std::unique_ptr<srsran::radio>(new srsran::radio);
  if (tmp_radio == nullptr) {
    srsran::console("Error creating radio multi instance.\n");
    return SRSRAN_ERROR;
  }

  std::unique_ptr<srsenb::phy> tmp_phy = std::unique_ptr<srsenb::phy>(new srsenb::phy(log_sink));
  if (tmp_phy == nullptr) {
    srsran::console("Error creating PHY instance.\n");
    return SRSRAN_ERROR;
  }

  // initialize layers, if they exist
  if (tmp_eutra_stack) {
    if (tmp_eutra_stack->init(args.stack, rrc_cfg, tmp_phy.get(), x2.get()) != SRSRAN_SUCCESS) {
      srsran::console("Error initializing EUTRA stack.\n");
      ret = SRSRAN_ERROR;
    }
  }

  if (tmp_nr_stack) {
    if (tmp_nr_stack->init(args.nr_stack, rrc_nr_cfg, tmp_phy.get(), x2.get()) != SRSRAN_SUCCESS) {
      srsran::console("Error initializing NR stack.\n");
      ret = SRSRAN_ERROR;
    }
  }

  // Init Radio
  if (tmp_radio->init(args.rf, tmp_phy.get())) {
    srsran::console("Error initializing radio.\n");
    return SRSRAN_ERROR;
  }

  // Only Init PHY if radio could be initialized
  if (ret == SRSRAN_SUCCESS) {
    if (tmp_phy->init(args.phy, phy_cfg, tmp_radio.get(), tmp_eutra_stack.get(), *tmp_nr_stack, this)) {
      srsran::console("Error initializing PHY.\n");
      ret = SRSRAN_ERROR;
    }
  }

  if (tmp_eutra_stack) {
    eutra_stack = std::move(tmp_eutra_stack);
  }
  if (tmp_nr_stack) {
    nr_stack = std::move(tmp_nr_stack);
  }
  phy   = std::move(tmp_phy);
  radio = std::move(tmp_radio);

  started = true; // set to true in any case to allow stopping the eNB if an error happened

  // Now that everything is setup, log sector start events.
  const std::string& sib9_hnb_name =
      rrc_cfg.sibs[8].sib9().hnb_name_present ? rrc_cfg.sibs[8].sib9().hnb_name.to_string() : "";
  for (unsigned i = 0, e = rrc_cfg.cell_list.size(); i != e; ++i) {
    event_logger::get().log_sector_start(i, rrc_cfg.cell_list[i].pci, rrc_cfg.cell_list[i].cell_id, sib9_hnb_name);
  }

  if (ret == SRSRAN_SUCCESS) {
    srsran::console("\n==== eNodeB started ===\n");
    srsran::console("Type <t> to view trace\n");
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

    if (radio) {
      radio->stop();
    }

    if (eutra_stack) {
      eutra_stack->stop();
    }

    if (nr_stack) {
      nr_stack->stop();
    }

    // Now that everything is teared down, log sector stop events.
    const std::string& sib9_hnb_name =
        rrc_cfg.sibs[8].sib9().hnb_name_present ? rrc_cfg.sibs[8].sib9().hnb_name.to_string() : "";
    for (unsigned i = 0, e = rrc_cfg.cell_list.size(); i != e; ++i) {
      event_logger::get().log_sector_stop(i, rrc_cfg.cell_list[i].pci, rrc_cfg.cell_list[i].cell_id, sib9_hnb_name);
    }

    started = false;
  }
}

int enb::parse_args(const all_args_t& args_, rrc_cfg_t& rrc_cfg_, rrc_nr_cfg_t& rrc_cfg_nr_)
{
  // set member variable
  args = args_;
  return enb_conf_sections::parse_cfg_files(&args, &rrc_cfg_, &rrc_cfg_nr_, &phy_cfg);
}

void enb::start_plot()
{
  phy->start_plot();
}

void enb::print_pool()
{
  srsran::byte_buffer_pool::get_instance()->print_all_buffers();
}

bool enb::get_metrics(enb_metrics_t* m)
{
  if (!started) {
    return false;
  }
  radio->get_metrics(&m->rf);
  phy->get_metrics(m->phy);
  if (eutra_stack) {
    eutra_stack->get_metrics(&m->stack);
  }
  if (nr_stack) {
    nr_stack->get_metrics(&m->nr_stack);
  }
  m->running = true;
  m->sys     = sys_proc.get_metrics();
  return true;
}

void enb::cmd_cell_gain(uint32_t cell_id, float gain)
{
  phy->cmd_cell_gain(cell_id, gain);
}

void enb::cmd_cell_measure()
{
  phy->cmd_cell_measure();
}

std::string enb::get_build_mode()
{
  return std::string(srsran_get_build_mode());
}

std::string enb::get_build_info()
{
  if (std::string(srsran_get_build_info()).find("  ") != std::string::npos) {
    return std::string(srsran_get_version());
  }
  return std::string(srsran_get_build_info());
}

std::string enb::get_build_string()
{
  std::stringstream ss;
  ss << "Built in " << get_build_mode() << " mode using " << get_build_info() << ".";
  return ss.str();
}

void enb::toggle_padding()
{
  if (!started) {
    return;
  }
  if (eutra_stack) {
    eutra_stack->toggle_padding();
  }
}

void enb::tti_clock()
{
  if (!started) {
    return;
  }
  if (eutra_stack) {
    eutra_stack->tti_clock();
  }
  if (nr_stack) {
    nr_stack->tti_clock();
  }
}

} // namespace srsenb
