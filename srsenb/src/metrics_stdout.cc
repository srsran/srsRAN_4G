/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

// Define iszero() here since it's not defined in some platforms
static bool iszero(float x)
{
  return fabsf(x) < 2 * DBL_EPSILON;
}

void metrics_stdout::set_metrics(const enb_metrics_t& metrics, const uint32_t period_usec)
{
  if (!do_print || enb == nullptr) {
    return;
  }

  if (metrics.rf.rf_error) {
    fmt::print("RF status: O={}, U={}, L={}\n", metrics.rf.rf_o, metrics.rf.rf_u, metrics.rf.rf_l);
  }

  if (metrics.stack.rrc.ues.size() == 0) {
    return;
  }

  if (++n_reports > 10) {
    n_reports = 0;
    fmt::print("\n");
    fmt::print("-------------------DL--------------------|-------------------------UL-------------------------\n");
    fmt::print("rnti  cqi  ri  mcs  brate   ok  nok  (%) | pusch  pucch  phr  mcs  brate   ok  nok  (%)    bsr\n");
  }

  for (size_t i = 0; i < metrics.stack.rrc.ues.size(); i++) {
    // make sure we have stats for MAC and PHY layer too
    if (i >= metrics.stack.mac.ues.size() || i >= metrics.phy.size()) {
      break;
    }
    if (metrics.stack.mac.ues[i].tx_errors > metrics.stack.mac.ues[i].tx_pkts) {
      fmt::print("tx caution errors {} > {}\n", metrics.stack.mac.ues[i].tx_errors, metrics.stack.mac.ues[i].tx_pkts);
    }
    if (metrics.stack.mac.ues[i].rx_errors > metrics.stack.mac.ues[i].rx_pkts) {
      fmt::print("rx caution errors {} > {}\n", metrics.stack.mac.ues[i].rx_errors, metrics.stack.mac.ues[i].rx_pkts);
    }

    fmt::print("{:>4x}", metrics.stack.mac.ues[i].rnti);
    if (not iszero(metrics.stack.mac.ues[i].dl_cqi)) {
      fmt::print("  {:>3}", int(metrics.stack.mac.ues[i].dl_cqi));
    } else {
      fmt::print("  {:>3.3}", "n/a");
    }
    fmt::print("   {:>1}", int(metrics.stack.mac.ues[i].dl_ri));
    if (not isnan(metrics.phy[i].dl.mcs)) {
      fmt::print("   {:>2}", int(metrics.phy[i].dl.mcs));
    } else {
      fmt::print("   {:>2}", 0);
    }
    if (metrics.stack.mac.ues[i].tx_brate > 0) {
      fmt::print(
          " {:>6.6}",
          float_to_eng_string((float)metrics.stack.mac.ues[i].tx_brate / (metrics.stack.mac.ues[i].nof_tti * 1e-3), 1));
    } else {
      fmt::print(" {:>6}", 0);
    }
    fmt::print(" {:>4}", metrics.stack.mac.ues[i].tx_pkts - metrics.stack.mac.ues[i].tx_errors);
    fmt::print(" {:>4}", metrics.stack.mac.ues[i].tx_errors);
    if (metrics.stack.mac.ues[i].tx_pkts > 0 && metrics.stack.mac.ues[i].tx_errors) {
      fmt::print(" {:>3}%", int((float)100 * metrics.stack.mac.ues[i].tx_errors / metrics.stack.mac.ues[i].tx_pkts));
    } else {
      fmt::print(" {:>3}%", 0);
    }

    fmt::print(" |");

    auto clamp_sinr = [](float sinr) {
      if (sinr > 99.9f) {
        return 99.9f;
      }
      if (sinr < -99.9f) {
        return -99.9f;
      }
      return sinr;
    };

    if (not isnan(metrics.phy[i].ul.pusch_sinr) and not iszero(metrics.phy[i].ul.pusch_sinr)) {
      fmt::print(" {:>5.1f}", clamp_sinr(metrics.phy[i].ul.pusch_sinr));
    } else {
      fmt::print(" {:>5.5}", "n/a");
    }

    if (not isnan(metrics.phy[i].ul.pucch_sinr) and not iszero(metrics.phy[i].ul.pucch_sinr)) {
      fmt::print("  {:>5.1f}", clamp_sinr(metrics.phy[i].ul.pucch_sinr));
    } else {
      fmt::print("  {:>5.5}", "n/a");
    }

    fmt::print("  {:>3}", int(metrics.stack.mac.ues[i].phr));
    if (not isnan(metrics.phy[i].ul.mcs)) {
      fmt::print("   {:>2}", int(metrics.phy[i].ul.mcs));
    } else {
      fmt::print("   {:>2}", 0);
    }
    if (metrics.stack.mac.ues[i].rx_brate > 0) {
      fmt::print(
          " {:>6.6}",
          float_to_eng_string((float)metrics.stack.mac.ues[i].rx_brate / (metrics.stack.mac.ues[i].nof_tti * 1e-3), 1));
    } else {
      fmt::print(" {:>6}", 0);
    }
    fmt::print(" {:>4}", metrics.stack.mac.ues[i].rx_pkts - metrics.stack.mac.ues[i].rx_errors);
    fmt::print(" {:>4}", metrics.stack.mac.ues[i].rx_errors);

    if (metrics.stack.mac.ues[i].rx_pkts > 0 && metrics.stack.mac.ues[i].rx_errors > 0) {
      fmt::print(" {:>3}%", int((float)100 * metrics.stack.mac.ues[i].rx_errors / metrics.stack.mac.ues[i].rx_pkts));
    } else {
      fmt::print(" {:>3}%", 0);
    }
    fmt::print(" {:>6.6}", float_to_eng_string(metrics.stack.mac.ues[i].ul_buffer, 2));
    fmt::print("\n");
  }
}

std::string metrics_stdout::float_to_string(float f, int digits, int field_width)
{
  std::ostringstream os;
  int                precision;
  if (isnan(f) or fabs(f) < 0.0001) {
    f         = 0.0;
    precision = digits - 1;
  } else {
    precision = digits - (int)(log10f(fabs(f + 0.0001)) - 2 * DBL_EPSILON);
  }
  if (precision == -1) {
    precision = 0;
  }
  os << std::setw(field_width) << std::fixed << std::setprecision(precision) << f;
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
