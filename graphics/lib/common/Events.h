/**
 * \file lib/generic/graphics/qt/common/Events.h
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
 * Events used to pass data to Qt-based classes.
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <QEvent>
#include <complex>

class RealDataEvent
  : public QEvent
{
public:
  const static QEvent::Type type;

  RealDataEvent(double* dataPoints,
                int numPoints);
  RealDataEvent(float* dataPoints,
                int numPoints);
  virtual ~RealDataEvent();

  double* dataPoints_;
  int numPoints_;
};

class ComplexDataEvent
  : public QEvent
{
public:
  const static QEvent::Type type;

  ComplexDataEvent(std::complex<double>* dataPoints,
                   int numPoints);
  ComplexDataEvent(std::complex<float>* dataPoints,
                   int numPoints);
  virtual ~ComplexDataEvent();

  std::complex<double>* dataPoints_;
  int numPoints_;
};

#endif // EVENTS_H
