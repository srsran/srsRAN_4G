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
  uint32_t                slot_idx             = 0;
  uint64_t                slot_count           = 0;
  uint64_t                duration_slots;
  gnb_dummy_stack         gnb_stack;
  srsenb::nr::worker_pool gnb_phy;
  phy_common              gnb_phy_com;
  ue_dummy_stack          ue_stack;
  srsue::nr::worker_pool  ue_phy;
  phy_common              ue_phy_com;
  bool                    initialised = false;
  uint32_t                sf_sz       = 0;

public:
  struct args_t {
    double                          srate_hz     = 11.52e6;
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
    uint64_t                        durations_slots   = 100;

    args_t(int argc, char** argv);
  };

  test_bench(const args_t& args) :
    gnb_stack(args.gnb_stack),
    gnb_phy(gnb_phy_com, gnb_stack, srslog::get_default_sink(), args.gnb_phy.nof_phy_threads),
    ue_stack(args.ue_stack),
    ue_phy(args.ue_phy.nof_phy_threads),

    ue_phy_com(phy_common::args_t(args.srate_hz, args.buffer_sz_ms, args.nof_channels),
               srslog::fetch_basic_logger(UE_PHY_COM_LOG_NAME, srslog::get_default_sink(), false)),
    gnb_phy_com(phy_common::args_t(args.srate_hz, args.buffer_sz_ms, args.nof_channels),
                srslog::fetch_basic_logger(GNB_PHY_COM_LOG_NAME, srslog::get_default_sink(), false)),
    sf_sz((uint32_t)std::round(args.srate_hz * 1e-3)),
    duration_slots(args.durations_slots)
  {
    srslog::fetch_basic_logger(UE_PHY_COM_LOG_NAME).set_level(srslog::str_to_basic_level(args.phy_com_log_level));
    srslog::fetch_basic_logger(GNB_PHY_COM_LOG_NAME).set_level(srslog::str_to_basic_level(args.phy_com_log_level));

    if (not gnb_phy.init(args.gnb_phy, args.cell_list)) {
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
    gnb_worker->set_time(slot_idx, gnb_time);

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
    ue_worker->set_tti(slot_idx);
    ue_worker->set_tx_time(ue_time);

    // Start gNb work
    ue_phy_com.push_semaphore(ue_worker);
    ue_phy.start_worker(ue_worker);

    slot_count++;
    slot_idx = slot_count % (1024 * SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz));
    return slot_count <= duration_slots;
  }

  srsenb::mac_ue_metrics_t get_gnb_metrics() { return gnb_stack.get_metrics(); }
};

#endif // SRSRAN_TEST_BENCH_H
