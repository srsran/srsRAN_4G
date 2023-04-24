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

#include "srsenb/hdr/metrics_csv.h"
#include "srsenb/hdr/metrics_stdout.h"
#include "srsran/common/metrics_hub.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/srsran.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

using namespace srsenb;

namespace srsenb {

char* csv_file_name = NULL;

#define NUM_METRICS (4)

// fake classes
class enb_dummy : public enb_metrics_interface
{
public:
  enb_dummy()
  {
    // first entry
    metrics[0].rf.rf_o = 10;
    metrics[0].stack.rrc.ues.resize(2);
    metrics[0].stack.mac.ues.resize(metrics[0].stack.rrc.ues.size());
    metrics[0].stack.mac.ues[0].rnti      = 0x46;
    metrics[0].stack.mac.ues[0].tx_pkts   = 1000;
    metrics[0].stack.mac.ues[0].tx_errors = 1000;
    metrics[0].stack.mac.ues[0].tx_brate  = 0;
    metrics[0].stack.mac.ues[0].rx_pkts   = 50;
    metrics[0].stack.mac.ues[0].rx_errors = 49;
    metrics[0].stack.mac.ues[0].rx_brate  = 2;
    metrics[0].stack.mac.ues[0].ul_buffer = 100;
    metrics[0].stack.mac.ues[0].dl_buffer = 200;
    metrics[0].stack.mac.ues[0].dl_cqi    = 15.9;
    metrics[0].stack.mac.ues[0].dl_ri     = 1.5;
    metrics[0].stack.mac.ues[0].dl_pmi    = 1.0;
    metrics[0].stack.mac.ues[0].phr       = 12.0;
    metrics[0].phy.resize(2);
    metrics[0].phy[0].dl.mcs        = 28.0;
    metrics[0].phy[0].ul.mcs        = 20.2;
    metrics[0].phy[0].ul.pucch_sinr = 14.2;
    metrics[0].phy[0].ul.pusch_sinr = 14.2;

    metrics[0].rf.rf_o = 10;
    metrics[0].nr_stack.mac.ues.resize(1);
    metrics[0].nr_stack.mac.ues[0].rnti       = 0x4601;
    metrics[0].nr_stack.mac.ues[0].tx_pkts    = 2000;
    metrics[0].nr_stack.mac.ues[0].tx_errors  = 2000;
    metrics[0].nr_stack.mac.ues[0].tx_brate   = 0;
    metrics[0].nr_stack.mac.ues[0].rx_pkts    = 50;
    metrics[0].nr_stack.mac.ues[0].rx_errors  = 49;
    metrics[0].nr_stack.mac.ues[0].rx_brate   = 2;
    metrics[0].nr_stack.mac.ues[0].ul_buffer  = 100;
    metrics[0].nr_stack.mac.ues[0].dl_buffer  = 200;
    metrics[0].nr_stack.mac.ues[0].dl_cqi     = 15.9;
    metrics[0].nr_stack.mac.ues[0].dl_ri      = 1.5;
    metrics[0].nr_stack.mac.ues[0].dl_pmi     = 1.0;
    metrics[0].nr_stack.mac.ues[0].phr        = 12.0;
    metrics[0].nr_stack.mac.ues[0].dl_mcs     = 28;
    metrics[0].nr_stack.mac.ues[0].ul_mcs     = 22;
    metrics[0].nr_stack.mac.ues[0].pusch_sinr = 14;
    metrics[0].nr_stack.mac.ues[0].pucch_sinr = 14.7;

    // second
    metrics[1].rf.rf_o = 10;
    metrics[1].stack.rrc.ues.resize(1);
    metrics[1].stack.mac.ues.resize(metrics[1].stack.rrc.ues.size());
    metrics[1].stack.mac.ues[0].rnti      = 0xffff;
    metrics[1].stack.mac.ues[0].tx_pkts   = 100;
    metrics[1].stack.mac.ues[0].tx_errors = 0;
    metrics[1].stack.mac.ues[0].tx_brate  = 1e6;
    metrics[1].stack.mac.ues[0].rx_pkts   = 50;
    metrics[1].stack.mac.ues[0].rx_errors = 10;
    metrics[1].stack.mac.ues[0].rx_brate  = 1e3;
    metrics[1].stack.mac.ues[0].ul_buffer = 100;
    metrics[1].stack.mac.ues[0].dl_buffer = 200;
    metrics[1].stack.mac.ues[0].dl_cqi    = 1.2;
    metrics[1].stack.mac.ues[0].dl_ri     = 1.5;
    metrics[1].stack.mac.ues[0].dl_pmi    = 1.0;
    metrics[1].stack.mac.ues[0].phr       = 99.1;
    metrics[1].phy.resize(1);
    metrics[1].phy[0].dl.mcs        = 6.2;
    metrics[1].phy[0].ul.mcs        = 28.0;
    metrics[1].phy[0].ul.pucch_sinr = 22.2;
    metrics[1].phy[0].ul.pusch_sinr = 22.2;

    // third entry
    metrics[2].rf.rf_o = 10;
    metrics[2].stack.rrc.ues.resize(1);
    metrics[2].stack.mac.ues.resize(metrics[2].stack.rrc.ues.size());
    metrics[2].stack.mac.ues[0].rnti      = 0x1;
    metrics[2].stack.mac.ues[0].tx_pkts   = 9999;
    metrics[2].stack.mac.ues[0].tx_errors = 1;
    metrics[2].stack.mac.ues[0].tx_brate  = 776;
    metrics[2].stack.mac.ues[0].rx_pkts   = 50;
    metrics[2].stack.mac.ues[0].rx_errors = 0;
    metrics[2].stack.mac.ues[0].rx_brate  = 1e3;
    metrics[2].stack.mac.ues[0].ul_buffer = 100;
    metrics[2].stack.mac.ues[0].dl_buffer = 200;
    metrics[2].stack.mac.ues[0].dl_cqi    = 15.9;
    metrics[2].stack.mac.ues[0].dl_ri     = 1.5;
    metrics[2].stack.mac.ues[0].dl_pmi    = 1.0;
    metrics[2].stack.mac.ues[0].phr       = 12.0;
    metrics[2].phy.resize(1);
    metrics[2].phy[0].dl.mcs        = 28.0;
    metrics[2].phy[0].ul.mcs        = 20.2;
    metrics[2].phy[0].ul.pusch_sinr = 14.2;
    metrics[2].phy[0].ul.pucch_sinr = 14.2;

    // fourth entry with incomple PHY and MAC stats
    metrics[3].rf.rf_o = 10;
    metrics[3].stack.rrc.ues.resize(1);
    metrics[3].stack.mac.ues.resize(metrics[3].stack.rrc.ues.size());
    metrics[3].stack.mac.ues[0].rnti      = 0x1;
    metrics[3].stack.mac.ues[0].tx_pkts   = 9999;
    metrics[3].stack.mac.ues[0].tx_errors = 1;
    metrics[3].stack.mac.ues[0].tx_brate  = 776;
    metrics[3].stack.mac.ues[0].rx_pkts   = 50;
    metrics[3].stack.mac.ues[0].rx_errors = 0;
    metrics[3].stack.mac.ues[0].rx_brate  = 1e3;
    metrics[3].stack.mac.ues[0].ul_buffer = 100;
    metrics[3].stack.mac.ues[0].dl_buffer = 200;
    metrics[3].stack.mac.ues[0].dl_cqi    = 15.9;
    metrics[3].stack.mac.ues[0].dl_ri     = 1.5;
    metrics[3].stack.mac.ues[0].dl_pmi    = 1.0;
    metrics[3].stack.mac.ues[0].phr       = 12.0;
    metrics[3].phy.resize(0); // no PHY metrics for this UE
  }

  bool get_metrics(enb_metrics_t* m)
  {
    // fill dummy values
    *m = metrics[counter % NUM_METRICS];
    counter++;
    return true;
  }

private:
  int           counter              = 0;
  enb_metrics_t metrics[NUM_METRICS] = {};
};

} // namespace srsenb

void usage(char* prog)
{
  printf("Usage: %s -o csv_output_file\n", prog);
}

void parse_args(int argc, char** argv)
{
  int opt;

  while ((opt = getopt(argc, argv, "o")) != -1) {
    switch (opt) {
      case 'o':
        csv_file_name = argv[optind];
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (!csv_file_name) {
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  float     period = 1.0;
  enb_dummy enb;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc, argv);

  // the default metrics type for stdout output
  metrics_stdout metrics_screen;
  metrics_screen.set_handle(&enb);

  // the CSV file writer
  metrics_csv metrics_file(csv_file_name, &enb);

  // create metrics hub and register metrics for stdout
  srsran::metrics_hub<enb_metrics_t> metricshub;
  metricshub.init(&enb, period);
  metricshub.add_listener(&metrics_screen);
  metricshub.add_listener(&metrics_file);

  // enable printing
  metrics_screen.toggle_print(true);

  std::cout << "Running for 2 seconds .." << std::endl;
  usleep(4e6);

  metricshub.stop();
  return 0;
}
