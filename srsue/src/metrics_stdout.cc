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

#include "srsue/hdr/metrics_stdout.h"

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

char const * const prefixes[2][9] =
{
  {   "",   "m",   "u",   "n",    "p",    "f",    "a",    "z",    "y", },
  {   "",   "k",   "M",   "G",    "T",    "P",    "E",    "Z",    "Y", },
};

metrics_stdout::metrics_stdout()
    :do_print(false)
    ,n_reports(10)
    ,ue(NULL)
{
}

void metrics_stdout::set_ue_handle(ue_metrics_interface *ue_)
{
  ue = ue_;
}

void metrics_stdout::toggle_print(bool b)
{
  do_print = b;
}

void metrics_stdout::set_metrics(ue_metrics_t &metrics, const uint32_t period_usec)
{
  if(!do_print || ue == NULL)
    return;

  if (!ue->is_attached()) {
    cout << "--- disconnected ---" << endl;
    return;
  }

  if(++n_reports > 10)
  {
    cout << endl << "quemetrics:------------------------------------------------------------------------------------------------" << endl;

    for(size_t n = 0; n < SRSLTE_N_RADIO_BEARERS; ++n) {
       // use capacity to determine if lcid is active
       if(metrics.rlc.metrics[n].qmetrics.capacity) {
           cout  << "bearer"     << std::setw(2) << n;
           cout  << ", mode="    << std::setw(1) << metrics.rlc.metrics[n].mode;
           cout  << ", cap="     << std::setw(3) << metrics.rlc.metrics[n].qmetrics.capacity;
           cout  << ", depth="   << std::setw(3) << metrics.rlc.metrics[n].qmetrics.currsize;
           cout  << ", hw="      << std::setw(3) << metrics.rlc.metrics[n].qmetrics.highwater;
           cout  << ", cleared=" << std::setw(3) << metrics.rlc.metrics[n].qmetrics.num_cleared;
           cout  << ", pushed="  << std::setw(3) << metrics.rlc.metrics[n].qmetrics.num_push;
           cout  << ", pusherr=" << std::setw(3) << metrics.rlc.metrics[n].qmetrics.num_push_fail;
           cout  << ", poped="   << std::setw(3) << metrics.rlc.metrics[n].qmetrics.num_pop;
           cout  << ", poperr="  << std::setw(3) << metrics.rlc.metrics[n].qmetrics.num_pop_fail;
           cout << endl;
       }
    }
    cout << "-----------------------------------------------------------------------------------------------------------" << endl;

    cout << endl << "mrb quemetrics:--------------------------------------------------------------------------------------------" << endl;

    for(size_t n = 0; n < SRSLTE_N_MCH_LCIDS; ++n) {
        // use capacity to determine if lcid is active
        if(metrics.rlc.mrb_metrics[n].qmetrics.capacity) {
           cout << "bearer"     << std::setw(2) << n;
           cout << ", mode="    << std::setw(1) << metrics.rlc.mrb_metrics[n].mode;
           cout << ", cap="     << std::setw(3) << metrics.rlc.mrb_metrics[n].qmetrics.capacity;
           cout << ", depth="   << std::setw(3) << metrics.rlc.mrb_metrics[n].qmetrics.currsize;
           cout << ", hw="      << std::setw(3) << metrics.rlc.mrb_metrics[n].qmetrics.highwater;
           cout << ", cleared=" << std::setw(3) << metrics.rlc.mrb_metrics[n].qmetrics.num_cleared;
           cout << ", pushed="  << std::setw(3) << metrics.rlc.mrb_metrics[n].qmetrics.num_push;
           cout << ", pusherr=" << std::setw(3) << metrics.rlc.mrb_metrics[n].qmetrics.num_push_fail;
           cout << ", poped="   << std::setw(3) << metrics.rlc.mrb_metrics[n].qmetrics.num_pop;
           cout << ", poperr="  << std::setw(3) << metrics.rlc.mrb_metrics[n].qmetrics.num_pop_fail;
           cout << endl;
         }
      }
    cout << "-----------------------------------------------------------------------------------------------------------" << endl;


    n_reports = 0;
    cout << endl;
    cout << "----Signal--------------DL-------------------------------------UL----------------------" << endl;
    cout << "cc  rsrp    pl    cfo   mcs   snr turbo  brate   bler   ta_us  mcs   buff  brate   bler" << endl;
  }
  for (uint32_t r = 0; r < metrics.phy.nof_active_cc; r++) {
    cout << " " << r;
    cout << float_to_string(metrics.phy.dl[r].rsrp, 2);
    cout << float_to_string(metrics.phy.dl[r].pathloss, 2);
    cout << float_to_eng_string(metrics.phy.sync.cfo, 2);
    cout << float_to_string(metrics.phy.dl[r].mcs, 2);
    cout << float_to_string(metrics.phy.dl[r].sinr, 2);
    cout << float_to_string(metrics.phy.dl[r].turbo_iters, 2);

    cout << float_to_eng_string((float)metrics.mac[r].rx_brate / period_usec * 1e6, 2);
    if (metrics.mac[r].rx_pkts > 0) {
      cout << float_to_string((float)100 * metrics.mac[r].rx_errors / metrics.mac[r].rx_pkts, 1) << "%";
    } else {
      cout << float_to_string(0, 1) << "%";
    }

    cout << float_to_string(metrics.phy.sync.ta_us, 2);

    cout << float_to_string(metrics.phy.ul[r].mcs, 2);
    cout << float_to_eng_string((float)metrics.mac[r].ul_buffer, 2);
    cout << float_to_eng_string((float)metrics.mac[r].tx_brate / period_usec * 1e6, 2);
    if (metrics.mac[r].tx_pkts > 0) {
      cout << float_to_string((float)100 * metrics.mac[r].tx_errors / metrics.mac[r].tx_pkts, 1) << "%";
    } else {
      cout << float_to_string(0, 1) << "%";
    }
    cout << endl;
  }

  if (metrics.rf.rf_error) {
    printf("RF status: O=%d, U=%d, L=%d\n", metrics.rf.rf_o, metrics.rf.rf_u, metrics.rf.rf_l);
  }
}

std::string metrics_stdout::float_to_string(float f, int digits)
{
  std::ostringstream os;
  const int precision = SRSLTE_MIN((int)((f == 0.0f) ? digits - 1 : digits - log10f(fabsf(f)) - 2 * FLT_EPSILON), 3);
  os << std::setw(6) << std::fixed << std::setprecision(precision) << f;
  return os.str();
}

std::string metrics_stdout::float_to_eng_string(float f, int digits)
{
  const int degree = (f == 0.0) ? 0 : lrint( floor( log10( fabs( f ) ) / 3) );

  std::string factor;

  if (abs(degree) < 9) {
    if(degree < 0)
      factor = prefixes[0][ abs( degree ) ];
    else
      factor = prefixes[1][ abs( degree ) ];
  } else {
    return "failed";
  }

  const double scaled = f * pow( 1000.0, -degree );
  if (degree != 0) {
    return float_to_string(scaled, digits) + factor;
  } else {
    return " " + float_to_string(scaled, digits) + factor;
  }
}

} // namespace srsue
