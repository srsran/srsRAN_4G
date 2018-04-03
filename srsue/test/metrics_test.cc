/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include "srsue/hdr/ue_metrics_interface.h"
#include "srslte/common/metrics_hub.h"
#include "srsue/hdr/metrics_stdout.h"
#include "srsue/hdr/metrics_csv.h"

using namespace srsue;

namespace srsue {

char *csv_file_name = NULL;

// fake classes
class ue_dummy : public ue_metrics_interface
{
public:
  bool get_metrics(ue_metrics_t &m)
  {
    // fill dummy values
    bzero(&m, sizeof(ue_metrics_t));
    m.rf.rf_o = 10;
    m.phy.dl.rsrp = -10.0;
    m.phy.dl.pathloss = 74;
    return true;
  }

  bool is_attached()
  {
    return (rand() % 2 == 0);
  }
};
}

void usage(char *prog) {
  printf("Usage: %s -o csv_output_file\n", prog);
}

void parse_args(int argc, char **argv) {
  int opt;

  while ((opt = getopt(argc, argv, "o")) != -1) {
    switch(opt) {
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


int main(int argc, char **argv)
{
  float period = 1.0;
  ue_dummy ue;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc,argv);

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

  std::cout << "Running for 2 seconds .." << std::endl;
  usleep(2e6);

  metricshub.stop();
  return 0;
}
