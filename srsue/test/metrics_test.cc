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

#include "srslte/common/metrics_hub.h"
#include "srslte/srslte.h"
#include "srsue/hdr/metrics_csv.h"
#include "srsue/hdr/metrics_stdout.h"
#include "srsue/hdr/ue_metrics_interface.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

using namespace srsue;

namespace srsue {

char* csv_file_name = NULL;

// fake classes
class ue_dummy : public ue_metrics_interface
{
public:
  bool get_metrics(ue_metrics_t* m)
  {
    *m = {};

    // fill dummy values
    m->rf.rf_o                = 10;
    m->phy.nof_active_cc      = 2;
    m->phy.ch[0].rsrp         = -10.0f;
    m->phy.ch[0].pathloss     = 74;
    m->stack.mac[0].rx_pkts   = 100;
    m->stack.mac[0].rx_errors = 0;
    m->stack.mac[0].rx_brate  = 200;
    m->stack.mac[0].nof_tti   = 1;

    m->stack.mac[1].rx_pkts   = 100;
    m->stack.mac[1].rx_errors = 100;
    m->stack.mac[1].rx_brate  = 150;
    m->stack.mac[1].nof_tti   = 1;

    // random neighbour cells
    if (rand() % 2 == 0) {
      rrc_interface_phy_lte::phy_meas_t neighbor = {};
      neighbor.pci                               = 8;
      neighbor.rsrp                              = -33;
      m->stack.rrc.neighbour_cells.push_back(neighbor);
      m->stack.rrc.neighbour_cells.push_back(neighbor); // need to add twice since we use CA
    }

    m->stack.rrc.state = (rand() % 2 == 0) ? RRC_STATE_CONNECTED : RRC_STATE_IDLE;

    return true;
  }
};

} // namespace srsue

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
  float    period = 1.0;
  ue_dummy ue;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc, argv);

  // the default metrics type for stdout output
  metrics_stdout metrics_screen;
  metrics_screen.set_ue_handle(&ue);

  // the CSV file writer
  metrics_csv metrics_file(csv_file_name);
  metrics_file.set_ue_handle(&ue);

  // create metrics hub and register metrics for stdout
  srslte::metrics_hub<ue_metrics_t> metricshub;
  metricshub.init(&ue, period);
  metricshub.add_listener(&metrics_screen);
  metricshub.add_listener(&metrics_file);

  // enable printing
  metrics_screen.toggle_print(true);

  std::cout << "Running for 5 seconds .." << std::endl;
  usleep(5e6);

  metricshub.stop();
  return 0;
}
