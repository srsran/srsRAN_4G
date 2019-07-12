/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include <unistd.h>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <iomanip>
#include <iostream>

#include <stdio.h>

using namespace std;

namespace srsue{

metrics_csv::metrics_csv(std::string filename)
  :n_reports(0)
  ,metrics_report_period(1.0)
  ,ue(NULL)
{
  file.open(filename.c_str(), std::ios_base::out);
  pthread_mutex_init(&mutex, NULL);
}

metrics_csv::~metrics_csv()
{
  stop();
  pthread_mutex_destroy(&mutex);
}

void metrics_csv::set_ue_handle(ue_metrics_interface *ue_)
{
  ue = ue_;
}

void metrics_csv::stop()
{
  pthread_mutex_lock(&mutex);
  if (file.is_open()) {
    file << "#eof\n";
    file.flush();
    file.close();
  }
  pthread_mutex_unlock(&mutex);
}

void metrics_csv::set_metrics(ue_metrics_t &metrics, const uint32_t period_usec)
{
  pthread_mutex_lock(&mutex);
  if (file.is_open() && ue != NULL) {
    if(n_reports == 0) {
      file << "time;rsrp;pl;cfo;dl_mcs;dl_snr;dl_turbo;dl_brate;dl_bler;ul_ta;ul_mcs;ul_buff;ul_brate;ul_bler;rf_o;rf_u;rf_l;is_attached\n";
    }

    file << (metrics_report_period * n_reports) << ";";

    // Print PHY metrics for first CC
    file << float_to_string(metrics.phy.dl[0].rsrp, 2);
    file << float_to_string(metrics.phy.dl[0].pathloss, 2);
    file << float_to_string(metrics.phy.sync[0].cfo, 2);
    file << float_to_string(metrics.phy.dl[0].mcs, 2);
    file << float_to_string(metrics.phy.dl[0].sinr, 2);
    file << float_to_string(metrics.phy.dl[0].turbo_iters, 2);

    // Sum DL rate for all CCs
    float rx_brate = 0;
    for (uint32_t r = 0; r < metrics.phy.nof_active_cc; r++) {
      rx_brate += metrics.stack.mac[r].rx_brate;
    }
    file << float_to_string(rx_brate / period_usec * 1e6, 2);

    // Sum BLER for all CCs
    int rx_pkts   = 0;
    int rx_errors = 0;
    for (uint32_t r = 0; r < metrics.phy.nof_active_cc; r++) {
      rx_pkts += metrics.stack.mac[r].rx_pkts;
      rx_errors += metrics.stack.mac[r].rx_errors;
    }
    if (rx_pkts > 0) {
      file << float_to_string((float)100 * rx_errors / rx_pkts, 1);
    } else {
      file << float_to_string(0, 2);
    }

    file << float_to_string(metrics.phy.sync[0].ta_us, 2);
    file << float_to_string(metrics.phy.ul[0].mcs, 2);
    file << float_to_string((float)metrics.stack.mac[0].ul_buffer, 2);

    // Sum UL rate for all CCs
    float tx_brate = 0;
    for (uint32_t r = 0; r < metrics.phy.nof_active_cc; r++) {
      tx_brate += metrics.stack.mac[r].tx_brate;
    }
    file << float_to_string(tx_brate / period_usec * 1e6, 2);

    // Sum UL BLER for all CCs
    int tx_pkts   = 0;
    int tx_errors = 0;
    for (uint32_t r = 0; r < metrics.phy.nof_active_cc; r++) {
      tx_pkts += metrics.stack.mac[r].tx_pkts;
      tx_errors += metrics.stack.mac[r].tx_errors;
    }
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

    n_reports++;
  } else {
    std::cout << "couldn't write CSV file." << std::endl;
  }
  pthread_mutex_unlock(&mutex);
}

std::string metrics_csv::float_to_string(float f, int digits, bool add_semicolon)
{
  std::ostringstream os;
  const int precision = (f == 0.0) ? digits-1 : digits - log10(fabs(f))-2*DBL_EPSILON;
  os << std::fixed << std::setprecision(precision) << f;
  if (add_semicolon)
    os << ';';
  return os.str();
}

} // namespace srsue
