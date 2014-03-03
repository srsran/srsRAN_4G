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
 * Main test file for Waterfallplot class.
 */

#define BOOST_TEST_MODULE Waterfallplot_Test

#include "Waterfallplot.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/progress.hpp>
#include <boost/bind.hpp>
#include <qapplication.h>
#include <cstdlib>
#include <vector>
#include <boost/test/unit_test.hpp>

#define PI     3.14159265358979323846

using namespace std;

void threadMain1()
{
  int n=2048;
  Waterfallplot plot(n,n);
  plot.setTitle("Float");

  float step = 1.0*PI/n;
  float* data = new float[n*2];
  for(int i=0;i<n*2;i++)
    data[i] = sinf(step*i);

  for(int i=0;i<n;i++)
  {
    plot.appendNewData(data+i, n);
    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
  }
}

void threadMain2()
{
  int n=2048;
  Waterfallplot plot(n,n);
  plot.setTitle("Double");

  double step = 2.0*PI/n;
  double* data = new double[n*2];
  for(int i=0;i<n*2;i++)
    data[i] = sin(step*i);

  for(int i=0;i<n;i++)
  {
    plot.appendNewData(data+i, n);
    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
  }
}

void threadMain3()
{
  int n=2048;
  Waterfallplot plot(n,n);
  plot.setTitle("FloatVec");

  double step = 2.0*PI/n;
  std::vector<float> data;
  data.resize(n*2);
  for(int i=0;i<n*2;i++)
    data[i] = sin(step*i);

  for(int i=0;i<n;i++)
  {
    plot.appendNewData(data.begin()+i, data.begin()+i+n);
    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
  }
}

BOOST_AUTO_TEST_SUITE (Waterfallplot_Test)

BOOST_AUTO_TEST_CASE(Waterfallplot_Init_Test)
{
  int argc = 1;
  char* argv[] = { const_cast<char *>("Waterfallplot_Init_Test"), NULL };
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
