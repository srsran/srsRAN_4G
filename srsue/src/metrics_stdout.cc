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
    n_reports = 0;
    cout << endl;
    cout << "--Signal--------------DL------------------------------UL----------------------" << endl;
    cout << "  rsrp    pl    cfo   mcs   snr turbo  brate   bler   mcs   buff  brate   bler" << endl;
  }
  cout << float_to_string(metrics.phy.dl.rsrp, 2);
  cout << float_to_string(metrics.phy.dl.pathloss, 2);
  cout << float_to_eng_string(metrics.phy.sync.cfo, 2);
  cout << float_to_string(metrics.phy.dl.mcs, 2);
  cout << float_to_string(metrics.phy.dl.sinr, 2);
  cout << float_to_string(metrics.phy.dl.turbo_iters, 2);
  cout << float_to_eng_string((float) metrics.mac.rx_brate/period_usec*1e6, 2);
  if (metrics.mac.rx_pkts > 0) {
    cout << float_to_string((float) 100*metrics.mac.rx_errors/metrics.mac.rx_pkts, 1) << "%";
  } else {
    cout << float_to_string(0, 1) << "%";
  }
  cout << float_to_string(metrics.phy.ul.mcs, 2);
  cout << float_to_eng_string((float) metrics.mac.ul_buffer, 2);
  cout << float_to_eng_string((float) metrics.mac.tx_brate/period_usec*1e6, 2);
  if (metrics.mac.tx_pkts > 0) {
    cout << float_to_string((float) 100*metrics.mac.tx_errors/metrics.mac.tx_pkts, 1) << "%";
  } else {
    cout << float_to_string(0, 1) << "%";
  }
  cout << endl;

  if(metrics.rf.rf_error) {
    printf("RF status: O=%d, U=%d, L=%d\n", metrics.rf.rf_o, metrics.rf.rf_u, metrics.rf.rf_l);
  }
  
}

std::string metrics_stdout::float_to_string(float f, int digits)
{
  std::ostringstream os;
  const int    precision = (f == 0.0) ? digits-1 : digits - log10(fabs(f))-2*DBL_EPSILON;
  os << std::setw(6) << std::fixed << std::setprecision(precision) << f;
  return os.str();
}

std::string metrics_stdout::float_to_eng_string(float f, int digits)
{
  const int degree = (f == 0.0) ? 0 : lrint( floor( log10( fabs( f ) ) / 3) );

  std::string factor;

  if ( abs( degree ) < 9 )
  {
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
