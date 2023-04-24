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

#include "srsran/common/block_queue.h"
#include "srsran/common/crash_handler.h"
#include "srsran/common/rlc_pcap.h"
#include "srsran/common/test_common.h"
#include "srsran/common/threads.h"
#include "srsran/common/tsan_options.h"
#include "srsran/rlc/rlc.h"
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <random>

#include "srsran/common/mac_pcap.h"
#include "srsran/mac/mac_sch_pdu_nr.h"

inline int write_pdu_to_pcap(const std::unique_ptr<srsran::mac_pcap>& pcap_handle,
                             const bool                               is_dl,
                             const uint32_t                           lcid,
                             const uint8_t*                           payload,
                             const uint32_t                           len)
{
  const uint32_t PCAP_CRNTI = 0x1001;
  const uint32_t PCAP_TTI   = 666;
  if (pcap_handle) {
    srsran::byte_buffer_t  tx_buffer;
    srsran::mac_sch_pdu_nr tx_pdu;
    tx_pdu.init_tx(&tx_buffer, len + 10);
    tx_pdu.add_sdu(lcid, payload, len);
    tx_pdu.pack();
    if (is_dl) {
      pcap_handle->write_dl_crnti_nr(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
    } else {
      pcap_handle->write_ul_crnti_nr(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
    }

    return SRSRAN_SUCCESS;
  }
  return SRSRAN_ERROR;
}

typedef struct {
  std::string rat;
  std::string mode;
  int32_t     sdu_size;
  uint32_t    test_duration_sec;
  float       pdu_drop_rate;
  float       pdu_cut_rate;
  float       pdu_duplicate_rate;
  uint32_t    sdu_gen_delay_usec;
  uint32_t    pdu_tx_delay_usec;
  uint32_t    log_level;
  bool        single_tx;
  bool        write_pcap;
  uint32_t    avg_opp_size;
  bool        random_opp;
  bool        zero_seed;
  uint32_t    seed;
  uint32_t    nof_pdu_tti;
  uint32_t    max_retx;
  int32_t     log_hex_limit;
  std::string log_filename;
  uint32_t    min_sdu_size;
  uint32_t    max_sdu_size;
} stress_test_args_t;

void parse_args(stress_test_args_t* args, int argc, char* argv[])
{
  namespace bpo = boost::program_options;
  // Command line only options
  bpo::options_description general("General options");

  general.add_options()("help,h", "Produce help message")("version,v", "Print version information and exit");

  // clang-format off

  // Command line or config file options
  bpo::options_description common("Configuration options");
  common.add_options()
      ("rat",          bpo::value<std::string>(&args->rat)->default_value("LTE"), "The RLC version to use (LTE/NR)")
      ("mode",          bpo::value<std::string>(&args->mode)->default_value("AM"), "Whether to test RLC acknowledged or unacknowledged mode (AM/UM for LTE) (UM6/UM12/AM12/AM18 for NR)")
      ("duration",      bpo::value<uint32_t>(&args->test_duration_sec)->default_value(5), "Duration (sec)")
      ("sdu_size",      bpo::value<int32_t>(&args->sdu_size)->default_value(-1), "Size of SDUs (-1 means random)")
      ("random_opp",    bpo::value<bool>(&args->random_opp)->default_value(true), "Whether to generate random MAC opportunities")
      ("avg_opp_size",  bpo::value<uint32_t>(&args->avg_opp_size)->default_value(1505), "Size of the MAC opportunity (if not random)")
      ("sdu_gen_delay", bpo::value<uint32_t>(&args->sdu_gen_delay_usec)->default_value(0), "SDU generation delay (usec)")
      ("pdu_tx_delay",  bpo::value<uint32_t>(&args->pdu_tx_delay_usec)->default_value(0), "Delay in MAC for transfering PDU from tx'ing RLC to rx'ing RLC (usec)")
      ("pdu_drop_rate", bpo::value<float>(&args->pdu_drop_rate)->default_value(0.1), "Rate at which RLC PDUs are dropped")
      ("pdu_cut_rate",  bpo::value<float>(&args->pdu_cut_rate)->default_value(0.0), "Rate at which RLC PDUs are chopped in length")
      ("pdu_duplicate_rate",  bpo::value<float>(&args->pdu_duplicate_rate)->default_value(0.0), "Rate at which RLC PDUs are duplicated")
      ("loglevel",      bpo::value<uint32_t>(&args->log_level)->default_value((int)srslog::basic_levels::debug), "Log level (1=Error,2=Warning,3=Info,4=Debug)")
      ("log_filename",  bpo::value<std::string>(&args->log_filename)->default_value("stdout"), "Filename to save log to")
      ("singletx",      bpo::value<bool>(&args->single_tx)->default_value(false), "If set to true, only one node is generating data")
      ("pcap",          bpo::value<bool>(&args->write_pcap)->default_value(false), "Whether to write all RLC PDU to PCAP file")
      ("zeroseed",      bpo::value<bool>(&args->zero_seed)->default_value(false), "Whether to initialize random seed to zero")
      ("seed",      bpo::value<uint32_t>(&args->seed)->default_value(0), "Seed to use in run. 0 means the seed is randomly generated")
      ("max_retx",      bpo::value<uint32_t>(&args->max_retx)->default_value(32), "Maximum number of RLC retransmission attempts")
      ("nof_pdu_tti",   bpo::value<uint32_t>(&args->nof_pdu_tti)->default_value(1), "Number of PDUs processed in a TTI")
      ("log_hex_limit",   bpo::value<int32_t>(&args->log_hex_limit)->default_value(-1), "Maximum bytes in hex log")
      ("min_sdu_size",   bpo::value<uint32_t>(&args->min_sdu_size)->default_value(5), "Minimum SDU size")
      ("max_sdu_size",   bpo::value<uint32_t>(&args->max_sdu_size)->default_value(1500), "Maximum SDU size");
  // clang-format on

  // these options are allowed on the command line
  bpo::options_description cmdline_options;
  cmdline_options.add(common).add(general);

  // parse the command line and store result in vm
  bpo::variables_map vm;
  bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).run(), vm);
  bpo::notify(vm);

  // help option was given - print usage and exit
  if (vm.count("help") > 0) {
    std::cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << std::endl << std::endl;
    std::cout << common << std::endl << general << std::endl;
    exit(0);
  }

  if (args->log_level > 4) {
    args->log_level = 4;
    printf("Set log level to %d (%s)\n",
           args->log_level,
           srslog::basic_level_to_string(static_cast<srslog::basic_levels>(args->log_level)));
  }

  // convert mode to upper case
  for (auto& c : args->mode) {
    c = toupper(c);
  }
}

class mac_dummy : public srsran::thread
{
public:
  mac_dummy(srsue::rlc_interface_mac* rlc1_,
            srsue::rlc_interface_mac* rlc2_,
            stress_test_args_t        args_,
            uint32_t                  lcid_,
            srsran::timer_handler*    timers_,
            srsran::rlc_pcap*         pcap_,
            uint32_t                  seed_) :
    run_enable(true),
    rlc1(rlc1_),
    rlc2(rlc2_),
    args(args_),
    pcap(pcap_),
    lcid(lcid_),
    timers(timers_),
    logger(srslog::fetch_basic_logger("MAC", false)),
    thread("MAC_DUMMY"),
    real_dist(0.0, 1.0),
    mt19937(seed_)
  {
    logger.set_level(static_cast<srslog::basic_levels>(args.log_level));
    logger.set_hex_dump_max_size(args.log_hex_limit);
  }

  void stop()
  {
    run_enable = false;
    wait_thread_finish();
  }

  void enqueue_task(srsran::move_task_t task) { pending_tasks.push(std::move(task)); }

private:
  void run_thread() final;

  void run_tti(srsue::rlc_interface_mac* tx_rlc, srsue::rlc_interface_mac* rx_rlc, bool is_dl);

  void run_tx_tti(srsue::rlc_interface_mac*                  tx_rlc,
                  srsue::rlc_interface_mac*                  rx_rlc,
                  std::vector<srsran::unique_byte_buffer_t>& pdu_list);

  void                      run_rx_tti(srsue::rlc_interface_mac*                  tx_rlc,
                                       srsue::rlc_interface_mac*                  rx_rlc,
                                       bool                                       is_dl,
                                       std::vector<srsran::unique_byte_buffer_t>& pdu_list);
  srsue::rlc_interface_mac* rlc1 = nullptr;
  srsue::rlc_interface_mac* rlc2 = nullptr;

  std::atomic<bool>      run_enable = {false};
  stress_test_args_t     args       = {};
  srsran::rlc_pcap*      pcap       = nullptr;
  uint32_t               lcid       = 0;
  srslog::basic_logger&  logger;
  srsran::timer_handler* timers = nullptr;

  srsran::block_queue<srsran::move_task_t> pending_tasks;

  std::mt19937                          mt19937;
  std::uniform_real_distribution<float> real_dist;
};

class rlc_tester : public srsue::pdcp_interface_rlc, public srsue::rrc_interface_rlc, public srsran::thread
{
public:
  rlc_tester(srsue::rlc_interface_pdcp* rlc_pdcp_,
             std::string                name_,
             stress_test_args_t         args_,
             uint32_t                   lcid_,
             uint32_t                   seed_) :
    logger(srslog::fetch_basic_logger(name_.c_str(), false)),
    rlc_pdcp(rlc_pdcp_),
    name(name_),
    args(args_),
    lcid(lcid_),
    thread("RLC_TESTER"),
    int_dist(args_.min_sdu_size, args_.max_sdu_size),
    mt19937(seed_)
  {
    logger.set_level(static_cast<srslog::basic_levels>(args.log_level));
    logger.set_hex_dump_max_size(args_.log_hex_limit);
  }

  void stop()
  {
    run_enable = false;
    wait_thread_finish();
  }

  // PDCP interface
  void write_pdu(uint32_t rx_lcid, srsran::unique_byte_buffer_t sdu) final;
  void write_pdu_bcch_bch(srsran::unique_byte_buffer_t sdu) final {}
  void write_pdu_bcch_dlsch(srsran::unique_byte_buffer_t sdu) final {}
  void write_pdu_pcch(srsran::unique_byte_buffer_t sdu) final {}
  void write_pdu_mch(uint32_t lcid_, srsran::unique_byte_buffer_t sdu) final {}
  void notify_delivery(uint32_t lcid_, const srsran::pdcp_sn_vector_t& pdcp_sns) final {}
  void notify_failure(uint32_t lcid_, const srsran::pdcp_sn_vector_t& pdcp_sns) final {}

  // RRC interface
  void max_retx_attempted() final
  {
    fprintf(
        stderr,
        "Maximum number of RLC retransmission reached. Consider increasing threshold or lowering channel drop rate.");
    logger.error(
        "Maximum number of RLC retransmission reached. Consider increasing threshold or lowering channel drop rate.");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    exit(1);
  }
  void protocol_failure() final
  {
    logger.error("RLC protocol error detected.");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    exit(1);
  }
  const char* get_rb_name(uint32_t lcid) final { return "DRB1"; }

  uint64_t get_nof_rx_pdus() const { return rx_pdus; }

private:
  const static size_t max_pdcp_sn = 262143U; // 18bit SN
  void                run_thread() final;

  std::atomic<bool> run_enable = {true};

  /// Tx uses thread-local PDCP SN to set SDU content, the Rx uses this variable to check received SDUs
  uint8_t               next_expected_sdu = 0;
  uint64_t              rx_pdus           = 0;
  uint32_t              lcid              = 0;
  srslog::basic_logger& logger;

  std::string name;

  stress_test_args_t args = {};

  srsue::rlc_interface_pdcp* rlc_pdcp = nullptr; // used by run_thread to push PDCP SDUs to RLC

  std::mt19937                    mt19937;
  std::uniform_int_distribution<> int_dist;
};

