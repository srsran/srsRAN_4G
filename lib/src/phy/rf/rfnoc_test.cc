//
// Copyright 2016 Ettus Research LLC
// Copyright 2018 Ettus Research, a National Instruments Company
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

// Example UHD/RFNoC application: Connect an rx radio to a tx radio and
// run a loopback.
#include "rf_uhd_rfnoc.h"
#include <boost/program_options.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <srslte/config.h>
#include <srslte/phy/utils/vector.h>
#include <uhd/device3.hpp>
#include <uhd/rfnoc/dma_fifo_block_ctrl.hpp>
#include <uhd/rfnoc/duc_ch2_block_ctrl.hpp>

#define TESTASSERT(cond)                                                                                               \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      printf("[%s][Line %d] Fail at \"%s\"\n", __FUNCTION__, __LINE__, (#cond));                                       \
      return -1;                                                                                                       \
    }                                                                                                                  \
  } while (0)

/****************************************************************************
 * main
 ***************************************************************************/
cf_t cexp_(float arg)
{
  std::complex<float> v = std::polar(0.5f, arg);
  return *((cf_t*)&v);
}

struct rfnoc_custom_testbench_s {
public:
  typedef struct {
    // Main parameters
    std::string ip_address           = "192.168.40.2";
    double      duration_s           = 1.0;
    double      master_clock_rate_hz = 184.32e6;
    double      srate_hz             = 1.92e6;
    double      init_tx_time         = 0.1;
    size_t      period_nsamples      = 19;
    uint32_t    nof_channels         = 2;
    uint32_t    nof_radios           = 1;
    bool        asynchronous_msg     = false;
    bool        blocking             = false;
    size_t      blocking_min_tx      = 1;

    // Getters
    float get_freq_norm() { return 1.0f / float(period_nsamples); }
  } config_s;

private:
  config_s config;

  std::mutex              mutex;
  std::condition_variable tx_cvar;
  std::condition_variable rx_cvar;

  cf_t* tx_buffer[2] = {};
  cf_t* rx_buffer[2] = {};

  std::shared_ptr<rf_uhd_safe_interface> rfnoc;
  size_t                                 packet_size_nsamples = 0;

  // Stats
  size_t txd_nsamples_total = 0;
  size_t txd_count          = 0;
  size_t rxd_nsamples_total = 0;
  size_t rxd_count          = 0;
  bool   running            = true;

  void check_async_msg()
  {
    bool                  valid = false;
    uhd::async_metadata_t async_metadata;

    rfnoc->recv_async_msg(async_metadata, 0.0, valid);

    if (valid) {
      switch (async_metadata.event_code) {

        case uhd::async_metadata_t::EVENT_CODE_BURST_ACK:
          Warning("BURST ACK") break;
        case uhd::async_metadata_t::EVENT_CODE_UNDERFLOW:
          Warning("UNDERFLOW") break;
        case uhd::async_metadata_t::EVENT_CODE_SEQ_ERROR:
          Warning("SEQUENCE ERROR") break;
        case uhd::async_metadata_t::EVENT_CODE_TIME_ERROR:
          Warning("TIME ERROR") break;
        case uhd::async_metadata_t::EVENT_CODE_UNDERFLOW_IN_PACKET:
          Warning("UNDERFLOW IN PACKET") break;
        case uhd::async_metadata_t::EVENT_CODE_SEQ_ERROR_IN_BURST:
          Warning("ERROR IN BURST") break;
        case uhd::async_metadata_t::EVENT_CODE_USER_PAYLOAD:
          Warning("ERROR USER PAYLOAD") break;
      }
    }
  }

  void* tx_routine()
  {
    size_t txd_nsamples   = 0;
    void*  tx_buffs_ptr[] = {tx_buffer[0], tx_buffer[1]};

    uhd::tx_metadata_t tx_metadata;
    tx_metadata.has_time_spec  = true;
    tx_metadata.time_spec      = config.init_tx_time;
    tx_metadata.start_of_burst = true;

    while (tx_metadata.time_spec < config.duration_s + config.init_tx_time and running) {
      cf_t v = cexp_(2.0f * M_PI * float(txd_nsamples_total % config.period_nsamples) * config.get_freq_norm());
      srslte_vec_gen_sine(v, config.get_freq_norm(), tx_buffer[0], packet_size_nsamples);
      srslte_vec_sc_prod_cfc(tx_buffer[0], -1, tx_buffer[1], packet_size_nsamples);

      if (config.blocking) {
        std::unique_lock<std::mutex> lock(mutex);
        while (rxd_count + config.blocking_min_tx <= txd_count and running) {
          std::cv_status status = rx_cvar.wait_for(lock, std::chrono::milliseconds(100));
          if (status == std::cv_status::timeout) {
            running = false;
          }
        }
      }

      // Actual transmission
      rfnoc->send(tx_buffs_ptr, packet_size_nsamples, tx_metadata, 0.1, txd_nsamples);
      tx_metadata.start_of_burst = false;

      // Update Tx state
      mutex.lock();
      txd_nsamples_total += txd_nsamples;
      txd_count++;
      tx_cvar.notify_all();
      mutex.unlock();
      tx_metadata.time_spec += packet_size_nsamples / config.srate_hz;
      tx_metadata.start_of_burst = false;

      if (not config.asynchronous_msg) {
        check_async_msg();
      }
    }

    tx_metadata.end_of_burst = true;
    rfnoc->send(tx_buffs_ptr, 0, tx_metadata, 0.1, txd_nsamples);

    return nullptr;
  }

  void* rx_routine()
  {
    void* rx_buffs_ptr[] = {rx_buffer[0], rx_buffer[1]};

    do {
      uhd::rx_metadata_t rx_metadata;
      size_t             rxd_nsamples = 0;

      if (config.blocking) {
        std::unique_lock<std::mutex> lock(mutex);
        while (rxd_count >= txd_count and running) {
          std::cv_status status = tx_cvar.wait_for(lock, std::chrono::milliseconds(100));
          if (status == std::cv_status::timeout) {
            running = false;
          }
        }
      }

      rfnoc->receive(rx_buffs_ptr, packet_size_nsamples, rx_metadata, 0.1, false, rxd_nsamples);

      std::unique_lock<std::mutex> lock(mutex);
      if (rx_metadata.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) {
        running = false;
      } else {
        // Update stats
        rxd_nsamples_total += rxd_nsamples;
        rxd_count++;
        float avg_power = srslte_vec_avg_power_cf(rx_buffer[0], rxd_nsamples);
        if (std::isnan(avg_power) or avg_power > 1e-6) {
          Warning("-- RX " << rxd_count << " Received " << avg_power);
        }
      }
      rx_cvar.notify_all();
    } while (running);

    return nullptr;
  }

  void* async_msg_routine()
  {
    while (running) {
      check_async_msg();

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return nullptr;
  }

public:
  rfnoc_custom_testbench_s(const config_s& config_)
  {
    rfnoc = std::make_shared<rf_uhd_rfnoc>();

    config = config_;
  }
  int init()
  {
    uhd::device_addr_t hint;
    hint["ip_addr"]            = config.ip_address;
    hint["master_clock_rate"]  = std::to_string(config.master_clock_rate_hz);
    hint["rfnoc_nof_channels"] = std::to_string(config.nof_channels);
    hint["rfnoc_nof_radios"]   = std::to_string(config.nof_radios);
    hint["loopback"]           = "true";
    uhd::log::set_console_level(uhd::log::severity_level::trace);

    if (rfnoc->usrp_make(hint, config.nof_channels * config.nof_radios) != UHD_ERROR_NONE) {
      Warning(rfnoc->last_error);
      return SRSLTE_ERROR;
    }

    if (rfnoc->set_tx_rate(config.srate_hz) != UHD_ERROR_NONE) {
      Warning(rfnoc->last_error);
      return SRSLTE_ERROR;
    }
    if (rfnoc->set_rx_rate(config.srate_hz) != UHD_ERROR_NONE) {
      Warning(rfnoc->last_error);
      return SRSLTE_ERROR;
    }

    size_t nof_tx_samples = 0;
    size_t nof_rx_samples = 0;
    rfnoc->get_tx_stream(nof_tx_samples);
    rfnoc->get_rx_stream(nof_rx_samples);
    packet_size_nsamples = SRSLTE_MIN(nof_tx_samples, nof_rx_samples);

    for (size_t j = 0; j < config.nof_channels; j++) {
      tx_buffer[j] = srslte_vec_cf_malloc(packet_size_nsamples);
      rx_buffer[j] = srslte_vec_cf_malloc(packet_size_nsamples);
    }

    if (rfnoc->start_rx_stream(config.init_tx_time) != UHD_ERROR_NONE) {
      Warning(rfnoc->last_error);
      return SRSLTE_ERROR;
    }

    return SRSLTE_SUCCESS;
  }

  int run()
  {
    running = true;

    std::thread tx_thread(&rfnoc_custom_testbench_s::tx_routine, this);
    std::thread rx_thread(&rfnoc_custom_testbench_s::rx_routine, this);

    if (config.asynchronous_msg) {
      std::thread async_msg_thread(&rfnoc_custom_testbench_s::async_msg_routine, this);
      async_msg_thread.join();
    }

    tx_thread.join();
    rx_thread.join();

    Info("Tx packets: " << txd_count);
    Info("Tx samples: " << txd_nsamples_total);

    Info("Rx packets: " << rxd_count);
    Info("Rx samples: " << rxd_nsamples_total);

    return SRSLTE_SUCCESS;
  }

  ~rfnoc_custom_testbench_s()
  {
    for (size_t j = 0; j < config.nof_channels; j++) {
      if (tx_buffer[j] != nullptr) {
        free(tx_buffer[j]);
      }
      if (rx_buffer[j] != nullptr) {
        free(rx_buffer[j]);
      }
    }
  }
};

static int test(const rfnoc_custom_testbench_s::config_s& config)
{
  rfnoc_custom_testbench_s context(config);

  TESTASSERT(context.init() == SRSLTE_SUCCESS);
  TESTASSERT(context.run() == SRSLTE_SUCCESS);

  return SRSLTE_SUCCESS;
}

namespace bpo = boost::program_options;

static int parse_args(int argc, char** argv, rfnoc_custom_testbench_s::config_s& args)
{
  int ret = SRSLTE_SUCCESS;

  bpo::options_description options;
  bpo::options_description common("Common execution options");

  // clang-format off
  common.add_options()
      ("ip_addr",           bpo::value<std::string>(&args.ip_address),       "Device 3 IP address")
      ("duration",          bpo::value<double>(&args.duration_s),            "Duration of the execution in seconds")
      ("master_clock_rate", bpo::value<double>(&args.master_clock_rate_hz),  "Device master clock rate in Hz")
      ("srate",             bpo::value<double>(&args.srate_hz),              "Sampling rate in Hz")
      ("init_time",         bpo::value<double>(&args.init_tx_time),          "Initial time in seconds")
      ("period_nsamples",   bpo::value<size_t>(&args.period_nsamples),       "Sine wave period in number of samples")
      ("asynch_msg",   bpo::value<bool>(&args.asynchronous_msg),             "Enable/disable asynchronous message")
      ("blocking",   bpo::value<bool>(&args.blocking),                       "Block transmitter/receiver for every transaction")
      ("blocking_min_tx",   bpo::value<size_t>(&args.blocking_min_tx),"Ensure a minimum of enqueued Tx")
      ;
  options.add(common).add_options()("help", "Show this message");
  // clang-format on

  bpo::variables_map vm;
  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(options).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error& e) {
    std::cerr << e.what() << std::endl;
    ret = SRSLTE_ERROR;
  }

  // help option was given or error - print usage and exit
  if (vm.count("help") || ret) {
    std::cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << std::endl << std::endl;
    std::cout << options << std::endl << std::endl;
    ret = SRSLTE_ERROR;
  }

  return ret;
}

int main(int argc, char* argv[])
{
  rfnoc_custom_testbench_s::config_s config;

  TESTASSERT(parse_args(argc, argv, config) == SRSLTE_SUCCESS);

  TESTASSERT(test(config) == SRSLTE_SUCCESS);

  return EXIT_SUCCESS;
}
