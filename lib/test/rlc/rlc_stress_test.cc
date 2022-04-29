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

#include "rlc_stress_test.h"
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

static std::unique_ptr<srsran::mac_pcap> pcap_handle = nullptr;
/***********************
 * MAC tester class
 ***********************/
void mac_dummy::run_thread()
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

void mac_dummy::run_tti(srsue::rlc_interface_mac* tx_rlc, srsue::rlc_interface_mac* rx_rlc, bool is_dl)
{
  std::vector<srsran::unique_byte_buffer_t> pdu_list;

  // Run Tx
  run_tx_tti(tx_rlc, rx_rlc, pdu_list);

  // Reverse PDUs
  std::reverse(pdu_list.begin(), pdu_list.end());

  // Run Rx
  run_rx_tti(tx_rlc, rx_rlc, is_dl, pdu_list);
}

void mac_dummy::run_tx_tti(srsue::rlc_interface_mac*                  tx_rlc,
                           srsue::rlc_interface_mac*                  rx_rlc,
                           std::vector<srsran::unique_byte_buffer_t>& pdu_list)
{
  // Generate A number of MAC PDUs
  for (uint32_t i = 0; i < args.nof_pdu_tti; i++) {
    // Create PDU unique buffer
    srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (!pdu) {
      printf("Fatal Error: Could not allocate PDU in %s\n", __FUNCTION__);
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

void mac_dummy::run_rx_tti(srsue::rlc_interface_mac*                  tx_rlc,
                           srsue::rlc_interface_mac*                  rx_rlc,
                           bool                                       is_dl,
                           std::vector<srsran::unique_byte_buffer_t>& pdu_list)
{
  // Sleep if necessary
  if (args.pdu_tx_delay_usec > 0) {
    std::this_thread::sleep_for(std::chrono::microseconds(args.pdu_tx_delay_usec));
  }

  auto it          = pdu_list.begin(); // PDU iterator
  bool skip_action = false;            // Avoid discarding a duplicated or duplicating a discarded

  while (it != pdu_list.end()) {
    // Get PDU unique buffer
    srsran::unique_byte_buffer_t& pdu = *it;

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
      write_pdu_to_pcap(pcap_handle, is_dl, 4, pdu->msg, pdu_len); // Only handles NR rat
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

/***********************
 * RLC tester class
 ***********************/
// PDCP interface
void rlc_tester::write_pdu(uint32_t rx_lcid, srsran::unique_byte_buffer_t sdu)
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
      fprintf(stderr, "Received malformed SDU with size %d, expected data 0x%X\n", sdu->N_bytes, next_expected_sdu);
      fprintf(stdout, "Received malformed SDU with size %d, expected data 0x%X\n", sdu->N_bytes, next_expected_sdu);

      std::this_thread::sleep_for(std::chrono::seconds(1)); // give some time to flush logs
      exit(-1);
    }
  }
  next_expected_sdu += 1;
  rx_pdus++;
}

void rlc_tester::run_thread()
{
  uint32_t pdcp_sn  = 0;
  uint32_t sdu_size = 0;
  uint8_t  payload  = 0x0; // increment for each SDU
  while (run_enable) {
    // SDU queue is full, don't assign PDCP SN
    if (rlc_pdcp->sdu_queue_is_full(lcid)) {
      continue;
    }

    srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (pdu == nullptr) {
      printf("Error: Could not allocate PDU in rlc_tester::run_thread\n\n\n");
      // backoff for a bit
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

void stress_test(stress_test_args_t args)
{
  auto log_sink =
      (args.log_filename == "stdout") ? srslog::create_stdout_sink() : srslog::create_file_sink(args.log_filename);
  if (!log_sink) {
    return;
  }
  srslog::log_channel* chan = srslog::create_log_channel("main_channel", *log_sink);
  if (!chan) {
    return;
  }
  srslog::set_default_sink(*log_sink);

  auto& log1 = srslog::fetch_basic_logger("RLC_1", false);
  log1.set_level(static_cast<srslog::basic_levels>(args.log_level));
  log1.set_hex_dump_max_size(args.log_hex_limit);
  auto& log2 = srslog::fetch_basic_logger("RLC_2", false);
  log2.set_level(static_cast<srslog::basic_levels>(args.log_level));
  log2.set_hex_dump_max_size(args.log_hex_limit);

  srsran::rlc_pcap pcap;
  uint32_t         lcid = 1;

  srsran::rlc_config_t cnfg_ = {};
  if (args.rat == "LTE") {
    if (args.mode == "AM") {
      // config RLC AM bearer
      cnfg_                    = srsran::rlc_config_t::default_rlc_am_config();
      cnfg_.am.max_retx_thresh = args.max_retx;
    } else if (args.mode == "UM") {
      // config UM bearer
      cnfg_ = srsran::rlc_config_t::default_rlc_um_config();
    } else if (args.mode == "TM") {
      // use default LCID in TM
      lcid = 0;
    } else {
      std::cout << "Unsupported RLC mode " << args.mode << ", exiting." << std::endl;
      exit(-1);
    }

    if (args.write_pcap) {
      pcap.open("rlc_stress_test.pcap", cnfg_);
    }
  } else if (args.rat == "NR") {
    if (args.mode == "UM6") {
      cnfg_ = srsran::rlc_config_t::default_rlc_um_nr_config(6);
    } else if (args.mode == "UM12") {
      cnfg_ = srsran::rlc_config_t::default_rlc_um_nr_config(12);
    } else if (args.mode == "AM12") {
      cnfg_                       = srsran::rlc_config_t::default_rlc_am_nr_config(12);
      cnfg_.am_nr.max_retx_thresh = args.max_retx;
    } else if (args.mode == "AM18") {
      cnfg_                       = srsran::rlc_config_t::default_rlc_am_nr_config(18);
      cnfg_.am_nr.max_retx_thresh = args.max_retx;
    } else {
      std::cout << "Unsupported RLC mode " << args.mode << ", exiting." << std::endl;
      exit(-1);
    }

    if (args.write_pcap) {
      pcap_handle = std::unique_ptr<srsran::mac_pcap>(new srsran::mac_pcap());
      pcap_handle->open("rlc_stress_test_nr.pcap");
    }
  } else {
    std::cout << "Unsupported RAT mode " << args.rat << ", exiting." << std::endl;
    exit(-1);
  }

  // generate random seed if needed
  uint32_t seed = 0;
  if (not args.zero_seed) {
    std::random_device rd;
    seed = rd();
  }

  if (args.seed != 0) {
    seed = args.seed;
  }

  srsran::timer_handler timers(8);

  srsran::rlc rlc1(log1.id().c_str());
  srsran::rlc rlc2(log2.id().c_str());

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

  printf("Starting test ... Seed: %u\n", seed);

  tester1.start(7);
  if (!args.single_tx) {
    tester2.start(7);
  }
  mac.start();

  // wait until test is over
  std::this_thread::sleep_for(std::chrono::seconds(args.test_duration_sec));

  srslog::flush();
  fflush(stdout);
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

  srsran::rlc_metrics_t metrics = {};
  rlc1.get_metrics(metrics, 1);

  printf("RLC1 received %" PRIu64 " SDUs in %ds (%.2f/s), Tx=%" PRIu64 " B, Rx=%" PRIu64 " B\n",
         tester1.get_nof_rx_pdus(),
         args.test_duration_sec,
         static_cast<double>(tester1.get_nof_rx_pdus() / args.test_duration_sec),
         metrics.bearer[lcid].num_tx_pdu_bytes,
         metrics.bearer[lcid].num_rx_pdu_bytes);
  rlc_bearer_metrics_print(metrics.bearer[lcid]);

  rlc2.get_metrics(metrics, 1);
  printf("RLC2 received %" PRIu64 " SDUs in %ds (%.2f/s), Tx=%" PRIu64 " B, Rx=%" PRIu64 " B\n",
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
