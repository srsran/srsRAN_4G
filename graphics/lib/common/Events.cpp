/**
 * \file lib/generic/graphics/qt/common/Events.cpp
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
 * Implementation of events used to pass data to Qt-based classes.
 */

#include "Events.h"

using namespace std;

const QEvent::Type RealDataEvent::type = static_cast<QEvent::Type>(10000);

RealDataEvent::RealDataEvent(double* dataPoints, int numPoints)
  : QEvent(QEvent::Type(type))
{
  dataPoints_ = new double[numPoints];
  numPoints_ = numPoints;
  memcpy(dataPoints_, dataPoints, numPoints*sizeof(double));
}

RealDataEvent::RealDataEvent(float* dataPoints, int numPoints)
  : QEvent(QEvent::Type(type))
{
  dataPoints_ = new double[numPoints];
  numPoints_ = numPoints;

  for(int i=0;i<numPoints_;i++)
  {
    dataPoints_[i] = (double)(dataPoints[i]);
  }
}

RealDataEvent::~RealDataEvent()
{
  delete[] dataPoints_;
}


const QEvent::Type ComplexDataEvent::type = static_cast<QEvent::Type>(10001);

ComplexDataEvent::ComplexDataEvent(complex<double>* dataPoints,
                                   int numPoints)
  : QEvent(QEvent::Type(type))
{
  dataPoints_ = new complex<double>[numPoints];
  numPoints_ = numPoints;
  memcpy(dataPoints_, dataPoints, numPoints*sizeof(complex<double>));
}

ComplexDataEvent::ComplexDataEvent(complex<float>* dataPoints,
                                   int numPoints)
  : QEvent(QEvent::Type(type))
{
  dataPoints_ = new complex<double>[numPoints];
  numPoints_ = numPoints;

  for(int i=0;i<numPoints_;i++)
  {
    dataPoints_[i] = complex<double>(dataPoints[i].real(),
                                     dataPoints[i].imag());
  }
}

ComplexDataEvent::~ComplexDataEvent()
{
  delete[] dataPoints_;
}
