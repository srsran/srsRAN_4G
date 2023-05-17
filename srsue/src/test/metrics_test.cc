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

#include "srsran/common/metrics_hub.h"
#include "srsran/common/test_common.h"
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

static char* csv_file_name = NULL;
static float period        = 1.0;
static int   duration_s    = 5;

// fake classes
class eutra_ue_dummy : public ue_metrics_interface
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

    m->phy.info[1].pci        = UINT32_MAX;
    m->stack.mac[1].rx_pkts   = 100;
    m->stack.mac[1].rx_errors = 100;
    m->stack.mac[1].rx_brate  = 150;
    m->stack.mac[1].nof_tti   = 1;

    // random neighbour cells
    if (rand() % 2 == 0) {
      phy_meas_t neighbor = {};
      neighbor.pci        = 8;
      neighbor.rsrp       = -33;
      m->stack.rrc.neighbour_cells.push_back(neighbor);
      m->stack.rrc.neighbour_cells.push_back(neighbor); // need to add twice since we use CA
    }

    m->phy.nof_active_cc         = 1;
    m->phy.ch[0].rsrp            = -10.0f;
    m->phy.ch[0].pathloss        = 32;

    m->stack.rrc.state = (rand() % 2 == 0) ? RRC_STATE_CONNECTED : RRC_STATE_IDLE;

    return true;
  }
};

class nsa_ue_dummy : public ue_metrics_interface
{
public:
  bool get_metrics(ue_metrics_t* m)
  {
    *m = {};

    // fill dummy values
    m->rf.rf_o                = 10;
    m->phy.nof_active_cc      = 1;
    m->phy.ch[0].rsrp         = -10.0f;
    m->phy.ch[0].pathloss     = 74;
    m->stack.mac[0].rx_pkts   = 100;
    m->stack.mac[0].rx_errors = 0;
    m->stack.mac[0].rx_brate  = 200;
    m->stack.mac[0].nof_tti   = 1;

    m->phy.info[1].pci        = UINT32_MAX;
    m->stack.mac[1].rx_pkts   = 100;
    m->stack.mac[1].rx_errors = 100;
    m->stack.mac[1].rx_brate  = 150;
    m->stack.mac[1].nof_tti   = 1;

    // random neighbour cells
    if (rand() % 2 == 0) {
      phy_meas_t neighbor = {};
      neighbor.pci        = 8;
      neighbor.rsrp       = -33;
      m->stack.rrc.neighbour_cells.push_back(neighbor);
      m->stack.rrc.neighbour_cells.push_back(neighbor); // need to add twice since we use CA
    }

    m->phy.nof_active_cc  = 1;
    m->phy.ch[0].rsrp     = -10.0f;
    m->phy.ch[0].pathloss = 32;

    // NR
    m->phy_nr.nof_active_cc      = 1;
    m->stack.mac_nr[0].rx_pkts   = 100;
    m->stack.mac_nr[0].rx_errors = 2;
    m->stack.mac_nr[0].rx_brate  = 223;
    m->stack.mac_nr[0].nof_tti   = 1;

    m->stack.rrc.state = (rand() % 2 == 0) ? RRC_STATE_CONNECTED : RRC_STATE_IDLE;

    return true;
  }
};

class sa_ue_dummy : public ue_metrics_interface
{
public:
  bool get_metrics(ue_metrics_t* m)
  {
    *m = {};

    // fill dummy values
    m->rf.rf_o           = 10;
    m->phy.nof_active_cc = 0;

    // NR
    m->phy_nr.nof_active_cc      = 1;
    m->phy_nr.info[0].pci        = 501;
    m->stack.mac_nr[0].rx_pkts   = 100;
    m->stack.mac_nr[0].rx_errors = 2;
    m->stack.mac_nr[0].rx_brate  = 223;
    m->stack.mac_nr[0].nof_tti   = 1;

    m->stack.rrc_nr.state = (rand() % 2 == 0) ? RRC_NR_STATE_CONNECTED : RRC_NR_STATE_IDLE;

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

int ue_test(std::string name, ue_metrics_interface* ue)
{
  // the default metrics type for stdout output
  metrics_stdout metrics_screen;
  metrics_screen.set_ue_handle(ue);

  // the CSV file writer
  metrics_csv metrics_file(csv_file_name);
  metrics_file.set_ue_handle(ue);

  // create metrics hub and register metrics for stdout
  srsran::metrics_hub<ue_metrics_t> metricshub;
  metricshub.init(ue, period);
  metricshub.add_listener(&metrics_screen);
  metricshub.add_listener(&metrics_file);

  // enable printing
  metrics_screen.toggle_print(true);

  std::cout << "Running " << name << " UE test for " << duration_s << " seconds .." << std::endl;
  usleep(duration_s * 1e6);

  metricshub.stop();

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc, argv);

  eutra_ue_dummy eutra_ue;
  TESTASSERT_SUCCESS(ue_test("EUTRA", &eutra_ue));

  nsa_ue_dummy nsa_ue;
  TESTASSERT_SUCCESS(ue_test("NSA", &nsa_ue));

  sa_ue_dummy sa_ue;
  TESTASSERT_SUCCESS(ue_test("SA", &sa_ue));

  return SRSRAN_SUCCESS;
}
