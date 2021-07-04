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

#define LOG_HEX_LIMIT (-1)

#define PCAP_CRNTI (0x1001)
#define PCAP_TTI (666)

#include "srsran/common/mac_pcap.h"
#include "srsran/mac/mac_sch_pdu_nr.h"
static std::unique_ptr<srsran::mac_pcap> pcap_handle = nullptr;

int write_pdu_to_pcap(const bool is_dl, const uint32_t lcid, const uint8_t* payload, const uint32_t len)
{
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

using namespace std;
using namespace srsue;
using namespace srsran;
namespace bpo = boost::program_options;

#define MIN_SDU_SIZE (5)
#define MAX_SDU_SIZE (1500)

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
  uint32_t    nof_pdu_tti;
  uint32_t    max_retx;
} stress_test_args_t;

void parse_args(stress_test_args_t* args, int argc, char* argv[])
{
  // Command line only options
  bpo::options_description general("General options");

  general.add_options()("help,h", "Produce help message")("version,v", "Print version information and exit");

  // clang-format off

  // Command line or config file options
  bpo::options_description common("Configuration options");
  common.add_options()
      ("rat",          bpo::value<std::string>(&args->rat)->default_value("LTE"), "The RLC version to use (LTE/NR)")
      ("mode",          bpo::value<std::string>(&args->mode)->default_value("AM"), "Whether to test RLC acknowledged or unacknowledged mode (AM/UM for LTE) (UM6/UM12 for NR)")
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
      ("singletx",      bpo::value<bool>(&args->single_tx)->default_value(false), "If set to true, only one node is generating data")
      ("pcap",          bpo::value<bool>(&args->write_pcap)->default_value(false), "Whether to write all RLC PDU to PCAP file")
      ("zeroseed",      bpo::value<bool>(&args->zero_seed)->default_value(false), "Whether to initialize random seed to zero")
      ("max_retx",      bpo::value<uint32_t>(&args->max_retx)->default_value(32), "Maximum number of RLC retransmission attempts")
      ("nof_pdu_tti",   bpo::value<uint32_t>(&args->nof_pdu_tti)->default_value(1), "Number of PDUs processed in a TTI");
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
    cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << endl << endl;
    cout << common << endl << general << endl;
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
  mac_dummy(rlc_interface_mac* rlc1_,
            rlc_interface_mac* rlc2_,
            stress_test_args_t args_,
            uint32_t           lcid_,
            timer_handler*     timers_,
            rlc_pcap*          pcap_,
            uint32_t           seed_) :
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
    logger.set_hex_dump_max_size(LOG_HEX_LIMIT);
  }

  void stop()
  {
    run_enable = false;
    wait_thread_finish();
  }

  void enqueue_task(srsran::move_task_t task) { pending_tasks.push(std::move(task)); }

private:
  void run_tx_tti(rlc_interface_mac* tx_rlc, rlc_interface_mac* rx_rlc, std::vector<unique_byte_buffer_t>& pdu_list)
  {
    // Generate A number of MAC PDUs
    for (uint32_t i = 0; i < args.nof_pdu_tti; i++) {
      // Create PDU unique buffer
      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      if (!pdu) {
        printf("Fatal Error: Could not allocate PDU in mac_reader::run_thread\n");
        exit(-1);
      }

      // Get MAC PDU size
      float factor = 1.0f;
      if (args.random_opp) {
        factor = 0.5f + real_dist(mt19937);
      }
      int opp_size = static_cast<int>(args.avg_opp_size * factor);

      // Request data to transmit
      uint32_t buf_state = tx_rlc->get_buffer_state(lcid);
      if (buf_state > 0) {
        pdu->N_bytes = tx_rlc->read_pdu(lcid, pdu->msg, opp_size);

        // Push PDU in the list
        pdu_list.push_back(std::move(pdu));
      }
    }
  }

  void run_rx_tti(rlc_interface_mac*                 tx_rlc,
                  rlc_interface_mac*                 rx_rlc,
                  bool                               is_dl,
                  std::vector<unique_byte_buffer_t>& pdu_list)
  {
    // Sleep if necessary
    if (args.pdu_tx_delay_usec > 0) {
      std::this_thread::sleep_for(std::chrono::microseconds(args.pdu_tx_delay_usec));
    }

    auto it          = pdu_list.begin(); // PDU iterator
    bool skip_action = false;            // Avoid discarding a duplicated or duplicating a discarded

    while (it != pdu_list.end()) {
      // Get PDU unique buffer
      unique_byte_buffer_t& pdu = *it;

      // Drop
      float rnd = real_dist(mt19937);
      if (std::isnan(rnd) || (((rnd > args.pdu_drop_rate) || skip_action) && pdu->N_bytes > 0)) {
        uint32_t pdu_len = pdu->N_bytes;

        // Cut
        if ((real_dist(mt19937) < args.pdu_cut_rate)) {
          int cut_pdu_len = static_cast<int>(pdu_len * real_dist(mt19937));
          logger.info("Cutting MAC PDU len (%d B -> %d B)", pdu_len, cut_pdu_len);
          pdu_len = cut_pdu_len;
        }

        // Write PDU in RX
        rx_rlc->write_pdu(lcid, pdu->msg, pdu_len);

        // Write PCAP
        write_pdu_to_pcap(is_dl, 4, pdu->msg, pdu_len); // Only handles NR rat
        if (is_dl) {
          pcap->write_dl_ccch(pdu->msg, pdu_len);
        } else {
          pcap->write_ul_ccch(pdu->msg, pdu_len);
        }
      } else {
        logger.info(pdu->msg, pdu->N_bytes, "Dropping RLC PDU (%d B)", pdu->N_bytes);
        skip_action = true; // Avoid drop duplicating this PDU
      }

      // Duplicate
      if (real_dist(mt19937) > args.pdu_duplicate_rate || skip_action) {
        it++;
        skip_action = false; // Allow action on the next PDU
      } else {
        logger.info(pdu->msg, pdu->N_bytes, "Duplicating RLC PDU (%d B)", pdu->N_bytes);
        skip_action = true; // Avoid drop of this PDU
      }
    }
  }

  void run_tti(rlc_interface_mac* tx_rlc, rlc_interface_mac* rx_rlc, bool is_dl)
  {
    std::vector<unique_byte_buffer_t> pdu_list;

    // Run Tx
    run_tx_tti(tx_rlc, rx_rlc, pdu_list);

    // Reverse PDUs
    std::reverse(pdu_list.begin(), pdu_list.end());

    // Run Rx
    run_rx_tti(tx_rlc, rx_rlc, is_dl, pdu_list);
  }

  void run_thread() override
  {
    srsran::move_task_t task;
    while (run_enable) {
      // Downlink direction first (RLC1->RLC2)
      run_tti(rlc1, rlc2, true);

      // UL direction (RLC2->RLC1)
      run_tti(rlc2, rlc1, false);

      // step timer
      timers->step_all();

      if (pending_tasks.try_pop(&task)) {
        task();
      }
    }
    if (pending_tasks.try_pop(&task)) {
      task();
    }
  }

  rlc_interface_mac* rlc1 = nullptr;
  rlc_interface_mac* rlc2 = nullptr;

  std::atomic<bool>      run_enable = {false};
  stress_test_args_t     args       = {};
  rlc_pcap*              pcap       = nullptr;
  uint32_t               lcid       = 0;
  srslog::basic_logger&  logger;
  srsran::timer_handler* timers = nullptr;

  srsran::block_queue<srsran::move_task_t> pending_tasks;

  std::mt19937                          mt19937;
  std::uniform_real_distribution<float> real_dist;
};

class rlc_tester : public pdcp_interface_rlc, public rrc_interface_rlc, public srsran::thread
{
public:
  rlc_tester(rlc_interface_pdcp* rlc_pdcp_,
             std::string         name_,
             stress_test_args_t  args_,
             uint32_t            lcid_,
             uint32_t            seed_) :
    logger(srslog::fetch_basic_logger(name_.c_str(), false)),
    rlc_pdcp(rlc_pdcp_),
    name(name_),
    args(args_),
    lcid(lcid_),
    thread("RLC_TESTER"),
    int_dist(MIN_SDU_SIZE, MAX_SDU_SIZE),
    mt19937(seed_)
  {
    logger.set_level(srslog::basic_levels::error);
    logger.set_hex_dump_max_size(LOG_HEX_LIMIT);
  }

  void stop()
  {
    run_enable = false;
    wait_thread_finish();
  }

  // PDCP interface
  void write_pdu(uint32_t rx_lcid, unique_byte_buffer_t sdu)
  {
    assert(rx_lcid == lcid);
    if (args.mode != "AM") {
      // Only AM will guarantee to deliver SDUs, take first byte as reference for other modes
      next_expected_sdu = sdu->msg[0];
    }

    // check SDU content (consider faster alternative)
    for (uint32_t i = 0; i < sdu->N_bytes; ++i) {
      if (sdu->msg[i] != next_expected_sdu) {
        logger.error(sdu->msg,
                     sdu->N_bytes,
                     "Received malformed SDU with size %d, expected data 0x%X",
                     sdu->N_bytes,
                     next_expected_sdu);
        fprintf(stderr, "Received malformed SDU with size %d\n", sdu->N_bytes);
        fprintf(stdout, "Received malformed SDU with size %d\n", sdu->N_bytes);
        std::this_thread::sleep_for(std::chrono::seconds(1)); // give some time to flush logs
        exit(-1);
      }
    }
    next_expected_sdu += 1;
    rx_pdus++;
  }
  void write_pdu_bcch_bch(unique_byte_buffer_t sdu) {}
  void write_pdu_bcch_dlsch(unique_byte_buffer_t sdu) {}
  void write_pdu_pcch(unique_byte_buffer_t sdu) {}
  void write_pdu_mch(uint32_t lcid_, srsran::unique_byte_buffer_t sdu) {}
  void notify_delivery(uint32_t lcid_, const srsran::pdcp_sn_vector_t& pdcp_sns) {}
  void notify_failure(uint32_t lcid_, const srsran::pdcp_sn_vector_t& pdcp_sns) {}

  // RRC interface
  void max_retx_attempted()
  {
    logger.error(
        "Maximum number of RLC retransmission reached. Consider increasing threshold or lowering channel drop rate.");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    exit(1);
  }
  void protocol_failure()
  {
    logger.error("RLC protocol error detected.");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    exit(1);
  }
  const char* get_rb_name(uint32_t rx_lcid) { return "DRB1"; }

  int get_nof_rx_pdus() { return rx_pdus; }

private:
  const static size_t max_pdcp_sn = 262143u; // 18bit SN
  void                run_thread()
  {
    uint32_t pdcp_sn  = 0;
    uint32_t sdu_size = 0;
    uint8_t  payload  = 0x0; // increment for each SDU
    while (run_enable) {
      // SDU queue is full, don't assign PDCP SN
      if (rlc_pdcp->sdu_queue_is_full(lcid)) {
        continue;
      }

      unique_byte_buffer_t pdu = srsran::make_byte_buffer();
      if (pdu == NULL) {
        printf("Error: Could not allocate PDU in rlc_tester::run_thread\n\n\n");
        // backoff for a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        continue;
      }
      pdu->md.pdcp_sn = pdcp_sn;

      // random or fixed SDU size
      if (args.sdu_size < 1) {
        sdu_size = int_dist(mt19937);
      } else {
        sdu_size = args.sdu_size;
      }

      for (uint32_t i = 0; i < sdu_size; i++) {
        pdu->msg[i] = payload;
      }
      pdu->N_bytes = sdu_size;
      payload++;

      rlc_pdcp->write_sdu(lcid, std::move(pdu));
      pdcp_sn = (pdcp_sn + 1) % max_pdcp_sn;
      if (args.sdu_gen_delay_usec > 0) {
        std::this_thread::sleep_for(std::chrono::microseconds(args.sdu_gen_delay_usec));
      }
    }
  }

  std::atomic<bool> run_enable = {true};

  /// Tx uses thread-local PDCP SN to set SDU content, the Rx uses this variable to check received SDUs
  uint8_t               next_expected_sdu = 0;
  uint64_t              rx_pdus           = 0;
  uint32_t              lcid              = 0;
  srslog::basic_logger& logger;

  std::string name;

  stress_test_args_t args = {};

  rlc_interface_pdcp* rlc_pdcp = nullptr; // used by run_thread to push PDCP SDUs to RLC

  std::mt19937                    mt19937;
  std::uniform_int_distribution<> int_dist;
};

void stress_test(stress_test_args_t args)
{
  auto& log1 = srslog::fetch_basic_logger("RLC_1", false);
  log1.set_level(static_cast<srslog::basic_levels>(args.log_level));
  log1.set_hex_dump_max_size(LOG_HEX_LIMIT);
  auto& log2 = srslog::fetch_basic_logger("RLC_2", false);
  log2.set_level(static_cast<srslog::basic_levels>(args.log_level));
  log2.set_hex_dump_max_size(LOG_HEX_LIMIT);

  rlc_pcap pcap;
  uint32_t lcid = 1;

  rlc_config_t cnfg_ = {};
  if (args.rat == "LTE") {
    if (args.mode == "AM") {
      // config RLC AM bearer
      cnfg_                    = rlc_config_t::default_rlc_am_config();
      cnfg_.am.max_retx_thresh = args.max_retx;
    } else if (args.mode == "UM") {
      // config UM bearer
      cnfg_ = rlc_config_t::default_rlc_um_config();
    } else if (args.mode == "TM") {
      // use default LCID in TM
      lcid = 0;
    } else {
      cout << "Unsupported RLC mode " << args.mode << ", exiting." << endl;
      exit(-1);
    }

    if (args.write_pcap) {
      pcap.open("rlc_stress_test.pcap", cnfg_);
    }
  } else if (args.rat == "NR") {
    if (args.mode == "UM6") {
      cnfg_ = rlc_config_t::default_rlc_um_nr_config(6);
    } else if (args.mode == "UM12") {
      cnfg_ = rlc_config_t::default_rlc_um_nr_config(12);
    } else {
      cout << "Unsupported RLC mode " << args.mode << ", exiting." << endl;
      exit(-1);
    }

    if (args.write_pcap) {
      pcap_handle = std::unique_ptr<srsran::mac_pcap>(new srsran::mac_pcap());
      pcap_handle->open("rlc_stress_test_nr.pcap");
    }
  } else {
    cout << "Unsupported RAT mode " << args.rat << ", exiting." << endl;
    exit(-1);
  }

  // generate random seed if needed
  uint32_t seed = 0;
  if (not args.zero_seed) {
    std::random_device rd;
    seed = rd();
  }

  srsran::timer_handler timers(8);

  rlc rlc1(log1.id().c_str());
  rlc rlc2(log2.id().c_str());

  rlc_tester tester1(&rlc1, "tester1", args, lcid, seed);
  rlc_tester tester2(&rlc2, "tester2", args, lcid, seed);
  mac_dummy  mac(&rlc1, &rlc2, args, lcid, &timers, &pcap, seed);

  rlc1.init(&tester1, &tester1, &timers, 0);
  rlc2.init(&tester2, &tester2, &timers, 0);

  // only add AM and UM bearers
  if (args.mode != "TM") {
    rlc1.add_bearer(lcid, cnfg_);
    rlc2.add_bearer(lcid, cnfg_);
  }

  printf("Starting test ..\n");

  tester1.start(7);
  if (!args.single_tx) {
    tester2.start(7);
  }
  mac.start();

  // wait until test is over
  std::this_thread::sleep_for(std::chrono::seconds(args.test_duration_sec));

  printf("Test finished, tearing down ..\n");

  // Stop RLC instances first to release blocking writers
  mac.enqueue_task([&rlc1, &rlc2]() {
    rlc1.stop();
    rlc2.stop();
  });

  printf("RLC entities stopped.\n");

  // Stop upper layer writers
  tester1.stop();
  tester2.stop();

  printf("Writers stopped.\n");

  mac.stop();
  if (args.write_pcap) {
    pcap.close();
  }

  rlc_metrics_t metrics = {};
  rlc1.get_metrics(metrics, 1);

  printf("RLC1 received %d SDUs in %ds (%.2f/s), Tx=%" PRIu64 " B, Rx=%" PRIu64 " B\n",
         tester1.get_nof_rx_pdus(),
         args.test_duration_sec,
         static_cast<double>(tester1.get_nof_rx_pdus() / args.test_duration_sec),
         metrics.bearer[lcid].num_tx_pdu_bytes,
         metrics.bearer[lcid].num_rx_pdu_bytes);
  rlc_bearer_metrics_print(metrics.bearer[lcid]);

  rlc2.get_metrics(metrics, 1);
  printf("RLC2 received %d SDUs in %ds (%.2f/s), Tx=%" PRIu64 " B, Rx=%" PRIu64 " B\n",
         tester2.get_nof_rx_pdus(),
         args.test_duration_sec,
         static_cast<double>(tester2.get_nof_rx_pdus() / args.test_duration_sec),
         metrics.bearer[lcid].num_tx_pdu_bytes,
         metrics.bearer[lcid].num_rx_pdu_bytes);
  rlc_bearer_metrics_print(metrics.bearer[lcid]);
}

int main(int argc, char** argv)
{
  srsran_debug_handle_crash(argc, argv);

  stress_test_args_t args = {};
  parse_args(&args, argc, argv);

  srslog::init();

  stress_test(args);

  exit(0);
}
