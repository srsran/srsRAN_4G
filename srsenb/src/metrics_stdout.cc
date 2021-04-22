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

#include "srsenb/hdr/metrics_stdout.h"
#include "srsran/phy/utils/vector.h"

#include <float.h>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

using namespace std;

namespace srsenb {

char const* const prefixes[2][9] = {
    {
        "",
        "m",
        "u",
        "n",
        "p",
        "f",
        "a",
        "z",
        "y",
    },
    {
        "",
        "k",
        "M",
        "G",
        "T",
        "P",
        "E",
        "Z",
        "Y",
    },
};

metrics_stdout::metrics_stdout() : do_print(false), n_reports(10), enb(NULL) {}

void metrics_stdout::set_handle(enb_metrics_interface* enb_)
{
  enb = enb_;
}

void metrics_stdout::toggle_print(bool b)
{
  do_print = b;
}

void metrics_stdout::set_metrics(const enb_metrics_t& metrics, const uint32_t period_usec)
{
  if (!do_print || enb == nullptr) {
    return;
  }

  if (metrics.rf.rf_error) {
    printf("RF status: O=%d, U=%d, L=%d\n", metrics.rf.rf_o, metrics.rf.rf_u, metrics.rf.rf_l);
  }

  if (metrics.stack.rrc.ues.size() == 0) {
    return;
  }

  std::ios::fmtflags f(cout.flags()); // For avoiding Coverity defect: Not restoring ostream format

  if (++n_reports > 10) {
    n_reports = 0;
    cout << endl;
    cout << "------DL-------------------------------UL--------------------------------------------" << endl;
    cout << "rnti cqi  ri mcs brate   ok  nok  (%)  pusch pucch phr mcs brate   ok  nok  (%)   bsr" << endl;
  }

  for (size_t i = 0; i < metrics.stack.rrc.ues.size(); i++) {
    // make sure we have stats for MAC and PHY layer too
    if (i >= metrics.stack.mac.ues.size() || i >= metrics.phy.size()) {
      break;
    }
    if (metrics.stack.mac.ues[i].tx_errors > metrics.stack.mac.ues[i].tx_pkts) {
      printf("tx caution errors %d > %d\n", metrics.stack.mac.ues[i].tx_errors, metrics.stack.mac.ues[i].tx_pkts);
    }
    if (metrics.stack.mac.ues[i].rx_errors > metrics.stack.mac.ues[i].rx_pkts) {
      printf("rx caution errors %d > %d\n", metrics.stack.mac.ues[i].rx_errors, metrics.stack.mac.ues[i].rx_pkts);
    }

    cout << int_to_hex_string(metrics.stack.mac.ues[i].rnti, 4) << " ";
    cout << float_to_string(SRSRAN_MAX(0.1, metrics.stack.mac.ues[i].dl_cqi), 1, 3);
    cout << float_to_string(metrics.stack.mac.ues[i].dl_ri, 1, 4);
    if (not isnan(metrics.phy[i].dl.mcs)) {
      cout << float_to_string(SRSRAN_MAX(0.1, metrics.phy[i].dl.mcs), 1, 4);
    } else {
      cout << float_to_string(0, 2, 4);
    }
    if (metrics.stack.mac.ues[i].tx_brate > 0) {
      cout << float_to_eng_string(
          SRSRAN_MAX(0.1, (float)metrics.stack.mac.ues[i].tx_brate / (metrics.stack.mac.ues[i].nof_tti * 1e-3)), 1);
    } else {
      cout << float_to_string(0, 1, 6) << "";
    }
    cout << std::setw(5) << metrics.stack.mac.ues[i].tx_pkts - metrics.stack.mac.ues[i].tx_errors;
    cout << std::setw(5) << metrics.stack.mac.ues[i].tx_errors;
    if (metrics.stack.mac.ues[i].tx_pkts > 0 && metrics.stack.mac.ues[i].tx_errors) {
      cout << float_to_string(
                  SRSRAN_MAX(0.1, (float)100 * metrics.stack.mac.ues[i].tx_errors / metrics.stack.mac.ues[i].tx_pkts), 1, 4)
           << "%";
    } else {
      cout << float_to_string(0, 1, 4) << "%";
    }
    cout << "  ";

    if (not isnan(metrics.phy[i].ul.pusch_sinr)) {
      cout << float_to_string(SRSRAN_MAX(0.1, metrics.phy[i].ul.pusch_sinr), 2, 5);
    } else {
      cout << float_to_string(0, 2, 5);
    }

    if (not isnan(metrics.phy[i].ul.pucch_sinr)) {
      cout << float_to_string(SRSRAN_MAX(0.1, metrics.phy[i].ul.pucch_sinr), 2, 5);
    } else {
      cout << float_to_string(0, 2, 5);
    }
    cout << " ";

    cout << float_to_string(metrics.stack.mac.ues[i].phr, 2, 5);
    if (not isnan(metrics.phy[i].ul.mcs)) {
      cout << float_to_string(SRSRAN_MAX(0.1, metrics.phy[i].ul.mcs), 1, 4);
    } else {
      cout << float_to_string(0, 1, 4);
    }
    if (metrics.stack.mac.ues[i].rx_brate > 0) {
      cout << float_to_eng_string(
          SRSRAN_MAX(0.1, (float)metrics.stack.mac.ues[i].rx_brate / (metrics.stack.mac.ues[i].nof_tti * 1e-3)), 1);
    } else {
      cout << float_to_string(0, 1) << "";
    }
    cout << std::setw(5) << metrics.stack.mac.ues[i].rx_pkts - metrics.stack.mac.ues[i].rx_errors;
    cout << std::setw(5) << metrics.stack.mac.ues[i].rx_errors;

    if (metrics.stack.mac.ues[i].rx_pkts > 0 && metrics.stack.mac.ues[i].rx_errors > 0) {
      cout << float_to_string(
                  SRSRAN_MAX(0.1, (float)100 * metrics.stack.mac.ues[i].rx_errors / metrics.stack.mac.ues[i].rx_pkts), 1, 4)
           << "%";
    } else {
      cout << float_to_string(0, 1, 4) << "%";
    }
    cout << float_to_eng_string(metrics.stack.mac.ues[i].ul_buffer, 2);
    cout << endl;
  }

  cout.flags(f); // For avoiding Coverity defect: Not restoring ostream format
}

std::string metrics_stdout::float_to_string(float f, int digits, int field_width)
{
  std::ostringstream os;
  int                precision;
  if (isnan(f) or fabs(f) < 0.0001) {
    f         = 0.0;
    precision = digits - 1;
  } else {
    precision = digits - (int)(log10f(fabs(f)) - 2 * DBL_EPSILON);
  }
  if (precision == -1) {
    precision = 0;
  }
  os << std::setw(field_width) << std::fixed << std::setprecision(precision) << f;
  return os.str();
}

std::string metrics_stdout::int_to_hex_string(int value, int field_width)
{
  std::ostringstream os;
  os << std::hex << std::setw(field_width) << value;
  return os.str();
}

std::string metrics_stdout::float_to_eng_string(float f, int digits)
{
  const int degree = (f == 0.0) ? 0 : lrint(floor(log10f(fabs(f)) / 3));

  std::string factor;

  if (abs(degree) < 9) {
    if (degree < 0)
      factor = prefixes[0][abs(degree)];
    else
      factor = prefixes[1][abs(degree)];
  } else {
    return "failed";
  }

  const double scaled = f * pow(1000.0, -degree);
  if (degree != 0) {
    return float_to_string(scaled, digits, 5) + factor;
  } else {
    return " " + float_to_string(scaled, digits, 5 - factor.length()) + factor;
  }
}

} // namespace srsenb
