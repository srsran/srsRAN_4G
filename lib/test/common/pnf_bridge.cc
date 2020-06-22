/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#include <assert.h>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <chrono>
#include <iostream>
#include <signal.h>
#include <thread>

#include "srslte/common/basic_pnf.h"

using namespace std;
namespace bpo = boost::program_options;

struct pnf_args_t {
  std::string gnb_vnf_addr;
  std::string ue_vnf_addr;
  uint16_t    gnb_vnf_port;
  uint16_t    ue_vnf_port;
  uint32_t    sf_interval;
  int32_t     num_sf;
  uint32_t    tb_len;
};

void parse_args(pnf_args_t* args, int argc, char* argv[])
{
  // Command line only options
  bpo::options_description general("General options");

  general.add_options()("help,h", "Produce help message")("version,v", "Print version information and exit");

  // Command line or config file options
  bpo::options_description common("Configuration options");

  // clang-format off
  common.add_options()
          ("vnf.gnb_addr", bpo::value<string>(&args->gnb_vnf_addr)->default_value("127.0.0.1"), "VNF address")
          ("vnf.ue_addr", bpo::value<string>(&args->ue_vnf_addr)->default_value("127.0.0.1"), "VNF address")
          ("vnf.gnb_port", bpo::value<uint16_t>(&args->gnb_vnf_port)->default_value(3333), "gNB VNF port")
          ("vnf.ue_port", bpo::value<uint16_t>(&args->ue_vnf_port)->default_value(3334), "UE VNF port")
          ("sf_interval", bpo::value<uint32_t>(&args->sf_interval)->default_value(1000), "Interval between subframes in us")
          ("num_sf", bpo::value<int32_t>(&args->num_sf)->default_value(-1), "Number of subframes to signal (-1 infinity)")
          ("tb_len", bpo::value<uint32_t>(&args->tb_len)->default_value(1600), "TB lenth (0 for random size)");
  // clang-format on

  // these options are allowed on the command line
  bpo::options_description cmdline_options;
  cmdline_options.add(common).add(general);

  // parse the command line and store result in vm
  bpo::variables_map vm;
  bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).run(), vm);
  bpo::notify(vm);

  // help option was given - print usage and exit
  if (vm.count("help")) {
    cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << endl << endl;
    cout << common << endl << general << endl;
    exit(0);
  }
}

bool running = true;
void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    running = false;
  }
}

int main(int argc, char** argv)
{
  signal(SIGINT, sig_int_handler);

  pnf_args_t args;
  parse_args(&args, argc, argv);

  srslte::srslte_basic_pnf ue_pnf("ue", args.ue_vnf_addr, args.ue_vnf_port, args.sf_interval, args.num_sf, args.tb_len);
  srslte::srslte_basic_pnf gnb_pnf(
      "gnb", args.gnb_vnf_addr, args.gnb_vnf_port, args.sf_interval, args.num_sf, args.tb_len);

  gnb_pnf.connect_out_rf_queue(ue_pnf.get_in_rf_queue());

  ue_pnf.start();
  gnb_pnf.start();

  while (running) {
    for (uint32_t i = 0; i < 2; ++i) {
      srslte::pnf_metrics_t metrics = (i == 0) ? ue_pnf.get_metrics() : gnb_pnf.get_metrics();
      printf("%s: RTT=%d, #Error=%d, #PDUs=%d, Total TB size=%d, Rate=%.2f Mbit/s\n",
             i == 0 ? "UE" : "gNB",
             metrics.avg_rtt_us,
             metrics.num_timing_errors,
             metrics.num_pdus,
             metrics.tb_size,
             metrics.tb_size * 8 / 1.0e6);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  ue_pnf.stop();
  gnb_pnf.stop();

  return 0;
}