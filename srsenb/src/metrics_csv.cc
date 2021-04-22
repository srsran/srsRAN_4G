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

#include "srsenb/hdr/metrics_csv.h"
#include "srsran/phy/utils/vector.h"

#include <float.h>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

using namespace std;

namespace srsenb {

metrics_csv::metrics_csv(std::string filename) : n_reports(0), metrics_report_period(1.0), enb(NULL)
{
  file.open(filename.c_str(), std::ios_base::out);
}

metrics_csv::~metrics_csv()
{
  stop();
}

void metrics_csv::set_handle(enb_metrics_interface* enb_)
{
  enb = enb_;
}

void metrics_csv::stop()
{
  if (file.is_open()) {
    file << "#eof\n";
    file.flush();
    file.close();
  }
}

void metrics_csv::set_metrics(const enb_metrics_t& metrics, const uint32_t period_usec)
{
  if (file.is_open() && enb != NULL) {
    if (n_reports == 0) {
      file << "time;nof_ue;dl_brate;ul_brate;"
              "proc_rmem;proc_rmem_kB;proc_vmem_kB;sys_mem;system_load;thread_count";

      // Add the cpus
      for (uint32_t i = 0, e = metrics.sys.cpu_count; i != e; ++i) {
        file << ";cpu_" << std::to_string(i);
      }

      // Add the new line.
      file << "\n";
    }

    // Time
    file << (metrics_report_period * n_reports) << ";";

    // UEs
    file << (metrics.stack.rrc.ues.size()) << ";";

    // Sum up rates for all UEs
    float dl_rate_sum = 0.0, ul_rate_sum = 0.0;
    for (size_t i = 0; i < metrics.stack.rrc.ues.size(); i++) {
      dl_rate_sum += metrics.stack.mac.ues[i].tx_brate / (metrics.stack.mac.ues[i].nof_tti * 1e-3);
      ul_rate_sum += metrics.stack.mac.ues[i].rx_brate / (metrics.stack.mac.ues[i].nof_tti * 1e-3);
    }

    // DL rate
    if (dl_rate_sum > 0) {
      file << float_to_string(SRSRAN_MAX(0.1, (float)dl_rate_sum), 2);
    } else {
      file << float_to_string(0, 2);
    }

    // UL rate
    if (ul_rate_sum > 0) {
      file << float_to_string(SRSRAN_MAX(0.1, (float)ul_rate_sum), 2);
    } else {
      file << float_to_string(0, 2);
    }

    // Write system metrics.
    const srsran::sys_metrics_t& m = metrics.sys;
    file << float_to_string(m.process_realmem, 2);
    file << std::to_string(m.process_realmem_kB) << ";";
    file << std::to_string(m.process_virtualmem_kB) << ";";
    file << float_to_string(m.system_mem, 2);
    file << float_to_string(m.process_cpu_usage, 2);
    file << std::to_string(m.thread_count) << ";";

    // Write the cpu metrics.
    for (uint32_t i = 0, e = m.cpu_count, last_cpu_index = e - 1; i != e; ++i) {
      file << float_to_string(m.cpu_load[i], 2, (i != last_cpu_index));
    }

    file << "\n";

    n_reports++;
  } else {
    std::cout << "Error, couldn't write CSV file." << std::endl;
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

} // namespace srsenb
