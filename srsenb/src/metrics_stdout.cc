/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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

#include "srsenb/hdr/metrics_stdout.h"

#include <unistd.h>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <iomanip>
#include <iostream>

#include <stdio.h>
#include <string.h>

using namespace std;

namespace srsenb{

char const * const prefixes[2][9] =
{
  {   "",   "m",   "u",   "n",    "p",    "f",    "a",    "z",    "y", },
  {   "",   "k",   "M",   "G",    "T",    "P",    "E",    "Z",    "Y", },
};

metrics_stdout::metrics_stdout() : started(false) ,do_print(false), metrics_report_period(0.0f),n_reports(10)
{
  enb_ = NULL;
  bzero(&metrics_thread, sizeof(metrics_thread));
  bzero(&metrics, sizeof(metrics));
}

bool metrics_stdout::init(enb_metrics_interface *u, float report_period_secs)
{
  enb_ = u;
  metrics_report_period = report_period_secs;

  started = true;
  pthread_create(&metrics_thread, NULL, &metrics_thread_start, this);
  return true;
}

void metrics_stdout::stop()
{
  if(started)
  {
    started = false;
    pthread_join(metrics_thread, NULL);
  }
}

void metrics_stdout::toggle_print(bool b)
{
  do_print = b;
}

void* metrics_stdout::metrics_thread_start(void *m_)
{
  metrics_stdout *m = (metrics_stdout*)m_;
  m->metrics_thread_run();
  return NULL;
}

void metrics_stdout::metrics_thread_run()
{
  while(started)
  {
    usleep(metrics_report_period*1e6);
    if(enb_->get_metrics(metrics)) {
      if (metrics.rrc.n_ues > 0) {
        print_metrics();
      }
    } else {
      print_disconnect();
    }
  }
}

void metrics_stdout::print_metrics()
{
  std::ios::fmtflags f(cout.flags()); // For avoiding Coverity defect: Not restoring ostream format

  if(!do_print)
    return;

  if(++n_reports > 10)
  {
    n_reports = 0;
    cout << endl;
    cout << "------DL-------------------------------UL--------------------------------" << endl;
    cout << "rnti   cqi ri      mcs  brate   bler   snr  phr   mcs  brate   bler   bsr" << endl;
  }
  if (metrics.rrc.n_ues > 0) {
    
    for (int i=0;i<metrics.rrc.n_ues;i++) {
      if (metrics.mac[i].tx_errors > metrics.mac[i].tx_pkts) {
        printf("tx caution errors %d > %d\n", metrics.mac[i].tx_errors, metrics.mac[i].tx_pkts);
      }
      if (metrics.mac[i].rx_errors > metrics.mac[i].rx_pkts) {
        printf("rx caution errors %d > %d\n", metrics.mac[i].rx_errors, metrics.mac[i].rx_pkts);
      }
    
      cout << std::hex << metrics.mac[i].rnti << " ";
      cout << float_to_string(metrics.mac[i].dl_cqi, 2);
      cout << float_to_string(metrics.mac[i].dl_ri, 3);
      cout << float_to_string(metrics.phy[i].dl.mcs, 2);
      if (metrics.mac[i].tx_brate > 0 && metrics_report_period) {
        cout << float_to_eng_string((float) metrics.mac[i].tx_brate/metrics_report_period, 2);
      } else {
        cout << float_to_string(0, 2);                
      }
      if (metrics.mac[i].tx_pkts > 0 && metrics.mac[i].tx_errors) {
        cout << float_to_string((float) 100*metrics.mac[i].tx_errors/metrics.mac[i].tx_pkts, 1) << "%";
      } else {
        cout << float_to_string(0, 1) << "%";
      }
      cout << float_to_string(metrics.phy[i].ul.sinr, 2);
      cout << float_to_string(metrics.mac[i].phr, 2);
      cout << float_to_string(metrics.phy[i].ul.mcs, 2);
      if (metrics.mac[i].rx_brate > 0 && metrics_report_period) {
        cout << float_to_eng_string((float) metrics.mac[i].rx_brate/metrics_report_period, 2);
      } else {        
        cout << float_to_string(0, 2);        
      }
      if (metrics.mac[i].rx_pkts > 0 && metrics.mac[i].rx_errors > 0) {
        cout << float_to_string((float) 100*metrics.mac[i].rx_errors/metrics.mac[i].rx_pkts, 1) << "%";
      } else {
        cout << float_to_string(0, 1) << "%";
      }
      cout << float_to_eng_string(metrics.mac[i].ul_buffer, 2);
      cout << endl;
    }
  } else {
    cout << "--- No users ---" << endl; 
  }
  if(metrics.rf.rf_error) {
    printf("RF status: O=%d, U=%d, L=%d\n", metrics.rf.rf_o, metrics.rf.rf_u, metrics.rf.rf_l);
  }

  cout.flags(f); // For avoiding Coverity defect: Not restoring ostream format
}

void metrics_stdout::print_disconnect()
{
  if(do_print) {
    cout << "--- disconnected ---" << endl;
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
