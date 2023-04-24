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

#include "srsue/hdr/metrics_stdout.h"

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

void metrics_stdout::set_ue_handle(ue_metrics_interface* ue_)
{
  std::lock_guard<std::mutex> lock(mutex);
  ue = ue_;
}

void metrics_stdout::toggle_print(bool b)
{
  std::lock_guard<std::mutex> lock(mutex);
  do_print = b;
}

void metrics_stdout::print_table(const bool display_neighbours, const bool is_nr)
{
  if (is_nr) {
    if (display_neighbours) {
      fmt::print(
          "---------Signal-----------|-Neighbour-|-----------------DL-----------------|-----------UL-----------\n");
      fmt::print(
          "rat  pci  rsrp   pl   cfo | pci  rsrp | mcs  snr  iter  brate  bler  ta_us | mcs   buff  brate  bler\n");
    } else {
      fmt::print("---------Signal-----------|-----------------DL-----------------|-----------UL-----------\n");
      fmt::print("rat  pci  rsrp   pl   cfo | mcs  snr  iter  brate  bler  ta_us | mcs   buff  brate  bler\n");
    }
  } else {
    if (display_neighbours) {
      fmt::print(
          "---------Signal-----------|-Neighbour-|-----------------DL-----------------|-----------UL-----------\n");
      fmt::print(
          " cc  pci  rsrp   pl   cfo | pci  rsrp | mcs  snr  iter  brate  bler  ta_us | mcs   buff  brate  bler\n");
    } else {
      fmt::print("---------Signal-----------|-----------------DL-----------------|-----------UL-----------\n");
      fmt::print(" cc  pci  rsrp   pl   cfo | mcs  snr  iter  brate  bler  ta_us | mcs   buff  brate  bler\n");
    }
  }
  table_has_neighbours = display_neighbours;
  n_reports            = 0;
}

void metrics_stdout::set_metrics_helper(const phy_metrics_t& phy,
                                        const mac_metrics_t  mac[SRSRAN_MAX_CARRIERS],
                                        const rrc_metrics_t& rrc,
                                        bool                 display_neighbours,
                                        const uint32_t       r,
                                        bool                 is_carrier_nr,
                                        bool                 print_carrier_num)
{
  if (print_carrier_num) {
    fmt::print("{:>3}", r);
  } else {
    fmt::print("{:>3.3}", (is_carrier_nr) ? "nr" : "lte");
  }

  if (phy.info[r].pci != UINT32_MAX) {
    fmt::print("  {:>3}", phy.info[r].pci);
  } else {
    fmt::print("  {:>3.3}", "n/a");
  }

  fmt::print("  {:>4}", int(phy.ch[r].rsrp));
  fmt::print(" {:>4}", int(phy.ch[r].pathloss));
  fmt::print(" {:>5.5}", float_to_eng_string(phy.sync[r].cfo, 2));

  // Find strongest neighbour for this EARFCN (cells are ordered)
  if (display_neighbours) {
    bool has_neighbour = false;
    fmt::print(" |");
    for (auto& c : rrc.neighbour_cells) {
      if (c.earfcn == phy.info[r].dl_earfcn && c.pci != phy.info[r].pci) {
        fmt::print(" {:>3}", c.pci);
        fmt::print("  {:>4}", int(c.rsrp));
        has_neighbour = true;
        break;
      }
    }
    if (!has_neighbour) {
      fmt::print(" {:>3.3}", "n/a");
      fmt::print("  {:>4.4}", "n/a");
    }
  }

  fmt::print(" |");

  fmt::print("  {:>2}", int(phy.dl[r].mcs));
  if (std::isnan(phy.ch[r].sinr) || std::isinf(phy.ch[r].sinr)) {
    fmt::print("  {:>3}", "n/a");
  } else {
    fmt::print("  {:>3}", int(phy.ch[r].sinr));
  }
  fmt::print("  {:>4.1f}", phy.dl[r].fec_iters);

  fmt::print(" {:>6.6}", float_to_eng_string((float)mac[r].rx_brate / (mac[r].nof_tti * 1e-3), 2));
  if (mac[r].rx_pkts > 0) {
    fmt::print("  {:>3}%", int((float)100 * mac[r].rx_errors / mac[r].rx_pkts));
  } else {
    fmt::print("  {:>3}%", 0);
  }

  fmt::print("   {:>4.1f}", phy.sync[r].ta_us);

  fmt::print(" |");

  fmt::print("  {:>2}", int(phy.ul[r].mcs));
  fmt::print(" {:>6.6}", float_to_eng_string((float)mac[r].ul_buffer, 2));
  fmt::print(" {:>6.6}", float_to_eng_string((float)mac[r].tx_brate / (mac[r].nof_tti * 1e-3), 2));

  if (mac[r].tx_pkts > 0) {
    fmt::print("  {:>3}%", int((float)100 * mac[r].tx_errors / mac[r].tx_pkts));
  } else {
    fmt::print("  {:>3}%", 0);
  }
  fmt::print("\n");
}

void metrics_stdout::set_metrics(const ue_metrics_t& metrics, const uint32_t period_usec)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (ue == nullptr) {
    return;
  }

  // always print RF error
  if (metrics.rf.rf_error) {
    fmt::print("RF status: O={}, U={}, L={}\n", metrics.rf.rf_o, metrics.rf.rf_u, metrics.rf.rf_l);
  }

  if (!do_print) {
    return;
  }

  if (metrics.stack.rrc.state != RRC_STATE_CONNECTED && metrics.stack.rrc_nr.state != RRC_NR_STATE_CONNECTED) {
    fmt::print("--- disconnected ---\n");
    return;
  }

  bool display_neighbours = FORCE_NEIGHBOUR_CELL;
  if (metrics.phy.nof_active_cc > 1) {
    display_neighbours |= metrics.stack.rrc.neighbour_cells.size() > metrics.phy.nof_active_cc - 1;
  } else {
    display_neighbours |= metrics.stack.rrc.neighbour_cells.size() > 0;
  }

  bool has_lte = metrics.phy.nof_active_cc > 0;
  bool has_nr  = metrics.phy_nr.nof_active_cc > 0;

  // print table header every 10 reports
  if (++n_reports > 10) {
    print_table(display_neighbours, has_nr);
  }

  // also print table header if neighbours are added/removed in between
  if (display_neighbours != table_has_neighbours) {
    print_table(display_neighbours, has_nr);
  }

  if (has_lte) {
    for (uint32_t r = 0; r < metrics.phy.nof_active_cc; r++) {
      set_metrics_helper(metrics.phy, metrics.stack.mac, metrics.stack.rrc, display_neighbours, r, false, !has_nr);
    }
  }

  if (has_nr) {
    for (uint32_t r = 0; r < metrics.phy_nr.nof_active_cc; r++) {
      // Assumption LTE is followed by the NR carriers.
      set_metrics_helper(metrics.phy_nr, metrics.stack.mac_nr, metrics.stack.rrc, display_neighbours, r, true, !has_nr);
    }
  }

  if (metrics.rf.rf_error) {
    fmt::print("RF status: O={}, U={}, L={}\n", metrics.rf.rf_o, metrics.rf.rf_u, metrics.rf.rf_l);
  }
}

std::string metrics_stdout::float_to_string(float f, int digits)
{
  std::ostringstream os;
  const int          precision =
      SRSRAN_MIN((int)((f == 0.0f || f == 100.0f) ? digits - 1 : digits - log10f(fabsf(f)) - 2 * FLT_EPSILON), 3);
  os << std::fixed << std::setprecision(precision) << f;
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
  return float_to_string(scaled, digits) + factor;
}

} // namespace srsue
