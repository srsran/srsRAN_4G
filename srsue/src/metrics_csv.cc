/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2015 The srsUE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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
}

metrics_csv::~metrics_csv()
{
  stop();
}

void metrics_csv::set_ue_handle(ue_metrics_interface *ue_)
{
  ue = ue_;
}

void metrics_csv::stop()
{
  if (file.is_open()) {
    file << "#eof\n";
    file.flush();
    file.close();
  }
}

void metrics_csv::set_metrics(ue_metrics_t &metrics, const uint32_t period_usec)
{
  if (file.is_open() && ue != NULL) {
    if(n_reports == 0) {
      file << "time;rsrp;pl;cfo;dl_mcs;dl_snr;dl_turbo;dl_brate;dl_bler;ul_ta;ul_mcs;ul_buff;ul_brate;ul_bler;rf_o;rf_u;rf_l;is_attached\n";
    }
    file << (metrics_report_period*n_reports) << ";";
    file << float_to_string(metrics.phy.dl.rsrp, 2);
    file << float_to_string(metrics.phy.dl.pathloss, 2);
    file << float_to_string(metrics.phy.sync.cfo, 2);
    file << float_to_string(metrics.phy.dl.mcs, 2);
    file << float_to_string(metrics.phy.dl.sinr, 2);
    file << float_to_string(metrics.phy.dl.turbo_iters, 2);
    file << float_to_string((float) metrics.mac.rx_brate/period_usec*1e6, 2);
    if (metrics.mac.rx_pkts > 0) {
      file << float_to_string((float) 100*metrics.mac.rx_errors/metrics.mac.rx_pkts, 1);
    } else {
      file << float_to_string(0, 2);
    }
    file << float_to_string(metrics.phy.sync.ta_us, 2);
    file << float_to_string(metrics.phy.ul.mcs, 2);
    file << float_to_string((float) metrics.mac.ul_buffer, 2);
    file << float_to_string((float) metrics.mac.tx_brate/period_usec*1e6, 2);
      if (metrics.mac.tx_pkts > 0) {
      file << float_to_string((float) 100*metrics.mac.tx_errors/metrics.mac.tx_pkts, 1);
    } else {
      file << float_to_string(0, 2);
    }
    file << float_to_string(metrics.rf.rf_o, 2);
    file << float_to_string(metrics.rf.rf_u, 2);
    file << float_to_string(metrics.rf.rf_l, 2);
    file << (ue->is_attached() ? "1.0" : "0.0");
    file << "\n";

    n_reports++;
  } else {
    std::cout << "Error, couldn't write CSV file." << std::endl;
  }
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
