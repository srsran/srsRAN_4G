/**
 * \file lib/generic/modulation/Crc_test.cpp
 * \version 1.0
 *
 * \section COPYRIGHT
 *
 * Copyright 2012-2013 The Iris Project Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution
 * and at http://www.softwareradiosystems.com/iris/copyright.html.
 *
 * \section LICENSE
 *
 * This file is part of the Iris Project.
 *
 * Iris is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Iris is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 * \section DESCRIPTION
 *
 * Main test file for Complexplot class.
 */

#define BOOST_TEST_MODULE Complexplot_Test

#include "Complexplot.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/progress.hpp>
#include <boost/bind.hpp>
#include <qapplication.h>
#include <cstdlib>
#include <complex>
#include <vector>
#include <boost/test/unit_test.hpp>

#define PI     3.14159265358979323846

using namespace std;

typedef vector< complex<float> > FloatVec;

void threadMain1()
{
  Complexplot plot;
  plot.setTitle("Float");
  plot.setXAxisRange(0,2);
  plot.setYAxisScale(Complexplot::Magnitude, 0.9, 1.1);

  int n=1024;
  float step = 2.0*PI/n;
  complex<float>* data = new complex<float>[n];
  for(int i=0;i<n;i++)
    data[i] = polar(1.0f,step*i);

  plot.setNewData(data, n);

  for(int i=0;i<n;i++)
  {
    rotate(data, data+1, data+n);
    plot.setNewData(data, n);
    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
  }
}

void threadMain2()
{
  Complexplot plot;
  plot.setTitle("Double");
  plot.setXAxisRange(0,2);
  plot.setYAxisScale(Complexplot::Magnitude, 0.9, 1.1);

  int n=1024;
  double step = 2.0*PI/n;
  complex<double>* data = new complex<double>[n];
  for(int i=0;i<n;i++)
    data[i] = polar(1.0,step*i);

  plot.setNewData(data, n);

  for(int i=0;i<n;i++)
  {
    rotate(data, data+1, data+n);
    plot.setNewData(data, n);
    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
  }
}

void threadMain3()
{

  Complexplot plot;
  plot.setTitle("FloatVec");
  plot.setXAxisRange(0,2);
  plot.setYAxisScale(Complexplot::Magnitude, 0.9, 1.1);

  FloatVec data(1024);
  int n=data.size();
  float step = 2.0*PI/n;
  for(int i=0;i<n;i++)
    data[i] = polar(1.0f,step*i);

  plot.setNewData(data.begin(), data.end());

  for(int i=0;i<n;i++)
  {
    rotate(data.begin(), data.begin()+1, data.end());
    plot.setNewData(data.begin(), data.end());
    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
  }
}

BOOST_AUTO_TEST_SUITE (Complexplot_Test)

BOOST_AUTO_TEST_CASE(Complexplot_Basic_Test)
{
  int argc = 1;
  char* argv[] = { const_cast<char *>("Compleplot_Basic_Test"), NULL };
  QApplication a(argc, argv);

  boost::scoped_ptr< boost::thread > thread1_;
  boost::scoped_ptr< boost::thread > thread2_;
  boost::scoped_ptr< boost::thread > thread3_;

  thread1_.reset( new boost::thread( &threadMain1 ) );
  thread2_.reset( new boost::thread( &threadMain2 ) );
  thread3_.reset( new boost::thread( &threadMain3 ) );

  qApp->exec();
  thread1_->join();
  thread2_->join();
  thread3_->join();
}

BOOST_AUTO_TEST_SUITE_END()
