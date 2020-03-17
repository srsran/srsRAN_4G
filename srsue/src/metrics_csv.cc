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

#include "srsue/hdr/metrics_csv.h"

#include <float.h>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

using namespace std;

namespace srsue {

metrics_csv::metrics_csv(std::string filename, bool append_)
{
  std::ios_base::openmode flags = std::ios_base::out;
  if (append_) {
    // check if file exists
    ifstream f(filename.c_str());
    if (f.good()) {
      file_exists = true;
      flags |= std::ios_base::app;
    }
  }
  file.open(filename.c_str(), flags);
}

metrics_csv::~metrics_csv()
{
  stop();
}

void metrics_csv::set_ue_handle(ue_metrics_interface* ue_)
{
  ue = ue_;
}

void metrics_csv::set_flush_period(const uint32_t flush_period_sec_)
{
  flush_period_sec = flush_period_sec_;
}

void metrics_csv::stop()
{
  std::unique_lock<std::mutex> lock(mutex);
  if (file.is_open()) {
    file << "#eof\n";
    file.flush();
    file.close();
  }
}

void metrics_csv::set_metrics(const ue_metrics_t& metrics, const uint32_t period_usec)
{
  std::unique_lock<std::mutex> lock(mutex);

  time_ms += period_usec / 1000;

  if (file.is_open() && ue != NULL) {
    if (n_reports == 0 && !file_exists) {
      file << "time;cc;pci;earfcn;rsrp;pl;cfo;dl_mcs;dl_snr;dl_turbo;dl_brate;dl_bler;ul_ta;ul_mcs;ul_buff;ul_brate;ul_"
              "bler;"
              "rf_o;rf_"
              "u;rf_l;is_attached\n";
    }

    for (uint32_t r = 0; r < metrics.phy.nof_active_cc; r++) {
      file << time_ms << ";";

      // CC and PCI
      file << r << ";";
      file << metrics.phy.info[r].pci << ";";
      file << metrics.phy.info[r].dl_earfcn << ";";

      // Print PHY metrics for first CC
      file << float_to_string(metrics.phy.dl[r].rsrp, 2);
      file << float_to_string(metrics.phy.dl[r].pathloss, 2);
      file << float_to_string(metrics.phy.sync[r].cfo, 2);
      file << float_to_string(metrics.phy.dl[r].mcs, 2);
      file << float_to_string(metrics.phy.dl[r].sinr, 2);
      file << float_to_string(metrics.phy.dl[r].turbo_iters, 2);

      if (metrics.stack.mac[r].rx_brate > 0) {
        file << float_to_string(metrics.stack.mac[r].rx_brate / (metrics.stack.mac[r].nof_tti * 1e-3), 2);
      } else {
        file << float_to_string(0, 2);
      }

      int rx_pkts   = metrics.stack.mac[r].rx_pkts;
      int rx_errors = metrics.stack.mac[r].rx_errors;
      if (rx_pkts > 0) {
        file << float_to_string((float)100 * rx_errors / rx_pkts, 1);
      } else {
        file << float_to_string(0, 2);
      }

      file << float_to_string(metrics.phy.sync[r].ta_us, 2);
      file << float_to_string(metrics.phy.ul[r].mcs, 2);
      file << float_to_string((float)metrics.stack.mac[r].ul_buffer, 2);

      if (metrics.stack.mac[r].tx_brate > 0) {
        file << float_to_string(metrics.stack.mac[r].tx_brate / (metrics.stack.mac[r].nof_tti * 1e-3), 2);
      } else {
        file << float_to_string(0, 2);
      }

      // Sum UL BLER for all CCs
      int tx_pkts   = metrics.stack.mac[r].tx_pkts;
      int tx_errors = metrics.stack.mac[r].tx_errors;
      if (tx_pkts > 0) {
        file << float_to_string((float)100 * tx_errors / tx_pkts, 1);
      } else {
        file << float_to_string(0, 2);
      }

      file << float_to_string(metrics.rf.rf_o, 2);
      file << float_to_string(metrics.rf.rf_u, 2);
      file << float_to_string(metrics.rf.rf_l, 2);
      file << (metrics.stack.rrc.state == RRC_STATE_CONNECTED ? "1.0" : "0.0");
      file << "\n";
    }

    n_reports++;

    if (flush_period_sec > 0) {
      flush_time_ms += period_usec / 1000;
      if (flush_time_ms / 1000 >= flush_period_sec) {
        file.flush();
        flush_time_ms -= flush_period_sec * 1000;
      }
    }
  } else {
    std::cout << "couldn't write CSV file." << std::endl;
  }
}

std::string metrics_csv::float_to_string(float f, int digits, bool add_semicolon)
{
  std::ostringstream os;
  const int          precision = (f == 0.0) ? digits - 1 : digits - log10f(fabs(f)) - 2 * DBL_EPSILON;
  os << std::fixed << std::setprecision(precision) << f;
  if (add_semicolon)
    os << ';';
  return os.str();
}

} // namespace srsue
