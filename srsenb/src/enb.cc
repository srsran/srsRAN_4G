/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/enb.h"
#include "srsenb/hdr/stack/enb_stack_lte.h"
#include "srsenb/hdr/stack/gnb_stack_nr.h"
#include "srsenb/src/enb_cfg_parser.h"
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
  stack.reset();
}

int enb::init(const all_args_t& args_)
{
  int ret = SRSRAN_SUCCESS;

  // Init eNB log
  enb_log.set_level(srslog::basic_levels::info);
  enb_log.info("%s", get_build_string().c_str());

  // Validate arguments
  if (parse_args(args_, rrc_cfg)) {
    srsran::console("Error processing arguments.\n");
    return SRSRAN_ERROR;
  }

  srsran::byte_buffer_pool::get_instance()->enable_logger(true);

  // Create layers
  if (args.stack.type == "lte") {
    std::unique_ptr<enb_stack_lte> lte_stack(new enb_stack_lte(log_sink));
    if (!lte_stack) {
      srsran::console("Error creating eNB stack.\n");
      return SRSRAN_ERROR;
    }

    std::unique_ptr<srsran::radio> lte_radio = std::unique_ptr<srsran::radio>(new srsran::radio);
    if (!lte_radio) {
      srsran::console("Error creating radio multi instance.\n");
      return SRSRAN_ERROR;
    }

    std::unique_ptr<srsenb::phy> lte_phy = std::unique_ptr<srsenb::phy>(new srsenb::phy(log_sink));
    if (!lte_phy) {
      srsran::console("Error creating LTE PHY instance.\n");
      return SRSRAN_ERROR;
    }

    if (ret == SRSRAN_SUCCESS) {
      if (lte_stack->init(args.stack, rrc_cfg, lte_phy.get()) != SRSRAN_SUCCESS) {
        srsran::console("Error initializing stack.\n");
        ret = SRSRAN_ERROR;
      }
    }

    // Init Radio
    if (lte_radio->init(args.rf, lte_phy.get())) {
      srsran::console("Error initializing radio.\n");
      return SRSRAN_ERROR;
    }

    // Only Init PHY if radio couldn't be initialized
    if (ret == SRSRAN_SUCCESS) {
      if (lte_phy->init(args.phy, phy_cfg, lte_radio.get(), lte_stack.get())) {
        srsran::console("Error initializing PHY.\n");
        ret = SRSRAN_ERROR;
      }
    }

    stack = std::move(lte_stack);
    phy   = std::move(lte_phy);
    radio = std::move(lte_radio);

  } else {
    srsran::console("Stack type %s not supported.\n", args.stack.type.c_str());
    return SRSRAN_ERROR;
  }

  started = true; // set to true in any case to allow stopping the eNB if an error happened

  // Now that everything is setup, log sector start events.
  for (unsigned i = 0, e = rrc_cfg.cell_list.size(); i != e; ++i) {
    event_logger::get().log_sector_start(i, rrc_cfg.cell_list[i].pci, rrc_cfg.cell_list[i].cell_id);
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

    if (stack) {
      stack->stop();
    }

    if (radio) {
      radio->stop();
    }

    // Now that everything is teared down, log sector stop events.
    for (unsigned i = 0, e = rrc_cfg.cell_list.size(); i != e; ++i) {
      event_logger::get().log_sector_stop(i, rrc_cfg.cell_list[i].pci, rrc_cfg.cell_list[i].cell_id);
    }

    started = false;
  }
}

int enb::parse_args(const all_args_t& args_, rrc_cfg_t& _rrc_cfg)
{
  // set member variable
  args = args_;
  return enb_conf_sections::parse_cfg_files(&args, &_rrc_cfg, &phy_cfg);
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
  radio->get_metrics(&m->rf);
  phy->get_metrics(m->phy);
  stack->get_metrics(&m->stack);
  m->running = started;
  m->sys     = sys_proc.get_metrics();
  return true;
}

void enb::cmd_cell_gain(uint32_t cell_id, float gain)
{
  phy->cmd_cell_gain(cell_id, gain);
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
  stack->toggle_padding();
}

} // namespace srsenb
