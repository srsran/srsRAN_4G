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

#ifndef SRSRAN_TEST_BENCH_H
#define SRSRAN_TEST_BENCH_H

#include "dummy_phy_common.h"
#include "srsenb/hdr/phy/nr/worker_pool.h"
#include "srsue/hdr/phy/nr/worker_pool.h"

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
  srsue::nr::worker_pool  ue_phy;
  phy_common              ue_phy_com;
  bool                    initialised = false;
  uint32_t                sf_sz       = 0;
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
    std::string                     phy_com_log_level = "info";
    std::string                     phy_lib_log_level = "none";
    uint64_t                        durations_slots   = 100;

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
    ue_phy(args.ue_phy.nof_phy_threads),

    ue_phy_com(phy_common::args_t(args.srate_hz, args.buffer_sz_ms, args.nof_channels),
               srslog::fetch_basic_logger(UE_PHY_COM_LOG_NAME, srslog::get_default_sink(), false)),
    gnb_phy_com(phy_common::args_t(args.srate_hz, args.buffer_sz_ms, args.nof_channels),
                srslog::fetch_basic_logger(GNB_PHY_COM_LOG_NAME, srslog::get_default_sink(), false)),
    sf_sz((uint32_t)std::round(args.srate_hz * 1e-3)),
    duration_slots(args.durations_slots),
    dl_channel(args.dl_channel, 1, srslog::fetch_basic_logger(CHANNEL_LOG_NAME, srslog::get_default_sink(), false)),
    ul_channel(args.ul_channel, 1, srslog::fetch_basic_logger(CHANNEL_LOG_NAME, srslog::get_default_sink(), false))
  {
    srslog::fetch_basic_logger(UE_PHY_COM_LOG_NAME).set_level(srslog::str_to_basic_level(args.phy_com_log_level));
    srslog::fetch_basic_logger(GNB_PHY_COM_LOG_NAME).set_level(srslog::str_to_basic_level(args.phy_com_log_level));
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

    // Initialise UE PHY
    if (not ue_phy.init(args.ue_phy, ue_phy_com, &ue_stack, 31)) {
      return;
    }

    // Set UE configuration
    if (not ue_phy.set_config(args.phy_cfg)) {
      return;
    }

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

  void stop()
  {
    ue_phy_com.stop();
    gnb_phy_com.stop();
    gnb_phy.stop();
    ue_phy.stop();
  }

  ~test_bench() = default;

  bool is_initialised() const { return ue_stack.is_valid() and gnb_stack.is_valid() and initialised; }

  bool run_tti()
  {
    // Get gNb worker
    srsenb::nr::slot_worker* gnb_worker = gnb_phy.wait_worker(slot_idx);
    if (gnb_worker == nullptr) {
      return false;
    }

    // Feed gNb the UE transmitted signal
    srsran::rf_timestamp_t gnb_time = {};
    std::vector<cf_t*>     gnb_rx_buffers(1);
    gnb_rx_buffers[0] = gnb_worker->get_buffer_rx(0);
    ue_phy_com.read(gnb_rx_buffers, sf_sz, gnb_time);

    // Set gNb time
    gnb_time.add(TX_ENB_DELAY * 1e-3);

    // Run the UL channel simulator
    ul_channel.run(gnb_rx_buffers.data(), gnb_rx_buffers.data(), (uint32_t)sf_sz, gnb_time.get(0));

    // Set gnb context
    srsran::phy_common_interface::worker_context_t gnb_context;
    gnb_context.sf_idx     = slot_idx;
    gnb_context.worker_ptr = gnb_worker;
    gnb_context.last       = true; // Set last if standalone
    gnb_context.tx_time.copy(gnb_time);
    gnb_worker->set_context(gnb_context);

    // Start gNb work
    gnb_phy_com.push_semaphore(gnb_worker);
    gnb_phy.start_worker(gnb_worker);

    // Get UE worker
    srsue::nr::sf_worker* ue_worker = ue_phy.wait_worker(slot_idx);
    if (ue_worker == nullptr) {
      return false;
    }

    // Feed UE the gNb transmitted signal
    srsran::rf_timestamp_t ue_time = {};
    std::vector<cf_t*>     ue_rx_buffers(1);
    ue_rx_buffers[0] = ue_worker->get_buffer(0, 0);
    gnb_phy_com.read(ue_rx_buffers, sf_sz, ue_time);

    // Set UE time
    ue_time.add(TX_ENB_DELAY * 1e-3);

    // Run the DL channel simulator
    dl_channel.run(ue_rx_buffers.data(), ue_rx_buffers.data(), (uint32_t)sf_sz, ue_time.get(0));

    // Set gnb context
    srsran::phy_common_interface::worker_context_t ue_context;
    ue_context.sf_idx     = slot_idx;
    ue_context.worker_ptr = ue_worker;
    ue_context.last       = true; // Set last if standalone
    ue_context.tx_time.copy(gnb_time);
    ue_worker->set_context(ue_context);

    // Run UE stack
    ue_stack.run_tti(slot_idx);

    // Start UE work
    ue_phy_com.push_semaphore(ue_worker);
    ue_phy.start_worker(ue_worker);

    slot_count++;
    slot_idx = slot_count % (1024 * SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz));
    return slot_count <= duration_slots;
  }

  metrics_t get_metrics()
  {
    metrics_t metrics = {};
    metrics.gnb_stack = gnb_stack.get_metrics();
    metrics.ue_stack  = ue_stack.get_metrics();
    ue_phy.get_metrics(metrics.ue_phy); // get the metrics from the ue_phy
    return metrics;
  }
};

#endif // SRSRAN_TEST_BENCH_H
