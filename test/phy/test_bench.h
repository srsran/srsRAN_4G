/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_TEST_BENCH_H
#define SRSRAN_TEST_BENCH_H

#include "dummy_phy_common.h"
#include "srsenb/hdr/phy/nr/worker_pool.h"
#include "srsran/radio/radio_dummy.h"
#include "srsue/hdr/phy/nr/worker_pool.h"
#include "srsue/hdr/phy/phy_nr_sa.h"

class test_bench
{
private:
  const std::string       UE_PHY_COM_LOG_NAME  = "UE /PHY/COM";
  const std::string       GNB_PHY_COM_LOG_NAME = "GNB/PHY/COM";
  const std::string       CHANNEL_LOG_NAME     = "CHANNEL";
  uint32_t                slot_idx             = 0;
  uint64_t                slot_count           = 0;
  uint64_t                duration_slots       = 0;
  gnb_dummy_stack         gnb_stack;
  srsenb::nr::worker_pool gnb_phy;
  phy_common              gnb_phy_com;
  ue_dummy_stack          ue_stack;
  srsue::phy_nr_sa        ue_phy;
  srsran::radio_dummy     ue_radio;
  srsran::rf_timestamp_t  gnb_rx_time = {};

  bool                initialised = false;
  uint32_t            sf_sz       = 0;
  srsran::rf_buffer_t rf_buffer;
  // Channel simulator
  srsran::channel dl_channel;
  srsran::channel ul_channel;

public:
  struct args_t {
    double                          srate_hz     = (double)(768 * SRSRAN_SUBC_SPACING_NR(0));
    uint32_t                        nof_channels = 1;
    uint32_t                        buffer_sz_ms = 10;
    bool                            valid        = false;
    srsran::phy_cfg_nr_t            phy_cfg      = {};
    srsenb::phy_cell_cfg_list_nr_t  cell_list    = {};
    srsenb::nr::worker_pool::args_t gnb_phy;
    gnb_dummy_stack::args_t         gnb_stack;
    srsue::phy_args_nr_t            ue_phy;
    ue_dummy_stack::args_t          ue_stack;
    std::string                     gnb_phy_com_log_level = "info";
    std::string                     ue_radio_log_level    = "info";
    std::string                     phy_lib_log_level     = "none";
    uint64_t                        durations_slots       = 100;

    // channel simulator args
    srsran::channel::args_t dl_channel;
    srsran::channel::args_t ul_channel;
    args_t(int argc, char** argv);
  };

  struct metrics_t {
    gnb_dummy_stack::metrics_t gnb_stack = {};
    ue_dummy_stack::metrics_t  ue_stack  = {};
    srsue::phy_metrics_t       ue_phy    = {};
  };

  test_bench(const args_t& args) :
    gnb_stack(args.gnb_stack),
    gnb_phy(gnb_phy_com, gnb_stack, srslog::get_default_sink(), args.gnb_phy.nof_phy_threads),
    ue_stack(args.ue_stack, ue_phy),
    ue_phy("PHY"),
    ue_radio(),
    gnb_phy_com(phy_common::args_t(args.srate_hz, args.buffer_sz_ms, args.nof_channels),
                srslog::fetch_basic_logger(GNB_PHY_COM_LOG_NAME, srslog::get_default_sink(), false)),
    sf_sz((uint32_t)std::round(args.srate_hz * 1e-3)),
    duration_slots(args.durations_slots),
    dl_channel(args.dl_channel, 1, srslog::fetch_basic_logger(CHANNEL_LOG_NAME, srslog::get_default_sink(), false)),
    ul_channel(args.ul_channel, 1, srslog::fetch_basic_logger(CHANNEL_LOG_NAME, srslog::get_default_sink(), false)),
    rf_buffer(1)
  {
    srslog::fetch_basic_logger(UE_PHY_COM_LOG_NAME).set_level(srslog::str_to_basic_level(args.gnb_phy_com_log_level));
    srslog::fetch_basic_logger(GNB_PHY_COM_LOG_NAME).set_level(srslog::str_to_basic_level(args.gnb_phy_com_log_level));
    srslog::fetch_basic_logger(CHANNEL_LOG_NAME).set_level(srslog::basic_levels::error);

    if (not gnb_phy.init(args.gnb_phy, args.cell_list)) {
      return;
    }

    srsenb::phy_interface_rrc_nr::common_cfg_t common_cfg = {};
    common_cfg.carrier                                    = args.phy_cfg.carrier;
    common_cfg.pdcch                                      = args.phy_cfg.pdcch;
    common_cfg.prach                                      = args.phy_cfg.prach;
    common_cfg.duplex_mode                                = args.phy_cfg.duplex.mode;
    common_cfg.ssb                                        = args.phy_cfg.get_ssb_cfg();

    if (gnb_phy.set_common_cfg(common_cfg) < SRSRAN_SUCCESS) {
      return;
    }

    // Initialise radio
    srsran::rf_args_t rf_args = {};
    rf_args.nof_antennas      = 1;
    rf_args.nof_carriers      = 1;
    rf_args.srate_hz          = args.srate_hz;
    rf_args.log_level         = args.ue_radio_log_level;
    if (ue_radio.init(rf_args, &ue_phy) != SRSRAN_SUCCESS) {
      return;
    }

    // Initialise UE PHY
    if (ue_phy.init(args.ue_phy, &ue_stack, &ue_radio) != SRSRAN_SUCCESS) {
      return;
    }

    // Wait for PHY to initialise
    ue_phy.wait_initialize();

    // Set UE configuration
    if (not ue_phy.set_config(args.phy_cfg)) {
      return;
    }

    // Wait for UE to notify stack that the configuration is completed
    ue_stack.wait_phy_config_complete();

    // Make sure PHY log is not set by UE or gNb PHY
    set_handler_enabled(false);
    if (args.phy_lib_log_level == "info") {
      set_srsran_verbose_level(SRSRAN_VERBOSE_INFO);
    } else if (args.phy_lib_log_level == "debug") {
      set_srsran_verbose_level(SRSRAN_VERBOSE_DEBUG);
    } else {
      set_srsran_verbose_level(SRSRAN_VERBOSE_NONE);
    }

    // Configure channel
    dl_channel.set_srate((uint32_t)args.srate_hz);
    ul_channel.set_srate((uint32_t)args.srate_hz);
    initialised = true;
  }

  srsue::rrc_interface_phy_nr::cell_select_result_t run_cell_select(const srsran_carrier_nr_t& carrier,
                                                                    const srsran_ssb_cfg_t&    ssb_cfg)
  {
    // Prepare return value
    srsue::rrc_interface_phy_nr::cell_select_result_t ret = {};

    // Prepare cell selection arguments
    srsue::phy_interface_rrc_nr::cell_select_args_t cs_args = {};
    cs_args.carrier                                         = carrier;
    cs_args.ssb_cfg                                         = ssb_cfg;

    // Start cell selection procedure
    if (not ue_phy.start_cell_select(cs_args)) {
      // Return unsuccessful cell select result
      return {};
    }

    // Run test bench until the cell selection is completed
    while (not ue_stack.get_cell_select_finished()) {
      run_tti();
    }

    // It is now the right time to start scheduling
    gnb_stack.start_scheduling();

    // Reset slot counting
    slot_count = 0;

    return ue_stack.get_cell_select_result();
  }

  void stop()
  {
    ue_stack.stop();
    ue_radio.stop();
    gnb_phy_com.stop();
    gnb_phy.stop();
    ue_phy.stop();
    gnb_stack.stop();
  }

  ~test_bench() = default;

  bool is_initialised()
  {
    return ue_stack.is_valid() and ue_radio.is_init() and ue_phy.is_initialized() and gnb_stack.is_valid() and
           initialised;
  }

  bool run_tti()
  {
    // Get gNb worker
    srsenb::nr::slot_worker* gnb_worker = gnb_phy.wait_worker(slot_idx);
    if (gnb_worker == nullptr) {
      return false;
    }

    // Feed gNb the UE transmitted signal
    std::vector<cf_t*> gnb_rx_buffers(1);
    gnb_rx_buffers[0] = gnb_worker->get_buffer_rx(0);
    ue_radio.read_tx(gnb_rx_buffers.data(), sf_sz);

    // Run the UL channel simulator
    ul_channel.run(gnb_rx_buffers.data(), gnb_rx_buffers.data(), (uint32_t)sf_sz, gnb_rx_time.get(0));

    // Set gNb TX time
    srsran::rf_timestamp_t gnb_time = gnb_rx_time;
    gnb_time.add(TX_ENB_DELAY * 1e-3);

    // Advance gNb Rx time
    gnb_rx_time.add(1e-3);

    // Set gNb context
    srsran::phy_common_interface::worker_context_t gnb_context;
    gnb_context.sf_idx     = slot_idx;
    gnb_context.worker_ptr = gnb_worker;
    gnb_context.last       = true; // Set last if standalone
    gnb_context.tx_time.copy(gnb_time);
    gnb_worker->set_context(gnb_context);

    // Start gNb work
    gnb_phy_com.push_semaphore(gnb_worker);
    gnb_phy.start_worker(gnb_worker);

    // Feed UE the gNb transmitted signal
    srsran::rf_timestamp_t ue_time = {};
    std::vector<cf_t*>     ue_rx_buffers(1);
    ue_rx_buffers[0] = rf_buffer.get(0);
    gnb_phy_com.read(ue_rx_buffers, sf_sz, ue_time);

    // Run the DL channel simulator
    dl_channel.run(ue_rx_buffers.data(), ue_rx_buffers.data(), (uint32_t)sf_sz, ue_time.get(0));

    // Write signal in UE radio buffer, this triggers UE to work
    ue_radio.write_rx(ue_rx_buffers.data(), sf_sz);

    // Throttle UE PHY by running stack tick
    ue_stack.tick();

    // Increment slot index, the slot index shall be continuous
    slot_idx = (slot_idx + 1) % (1024 * SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz));

    // Increment slot counter and determine end of execution
    slot_count++;
    return slot_count <= duration_slots;
  }

  metrics_t get_metrics()
  {
    metrics_t metrics = {};
    metrics.gnb_stack = gnb_stack.get_metrics();
    metrics.ue_stack  = ue_stack.get_metrics();
    ue_phy.get_metrics(srsran::srsran_rat_t::nr, &metrics.ue_phy); // get the metrics from the ue_phy
    return metrics;
  }
};

#endif // SRSRAN_TEST_BENCH_H
