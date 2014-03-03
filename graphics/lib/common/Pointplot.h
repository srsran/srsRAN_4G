/**
 * \file lib/generic/graphics/qt/common/Pointplot.h
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
 * A plot of complex data values as points on an IQ axis.
 */

#ifndef POINTPLOT_H
#define POINTPLOT_H

#include <qapplication.h>
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <complex>

class Pointplot
  : public QwtPlot
{
  Q_OBJECT

public:
  Pointplot(QWidget* parent = 0);
  virtual ~Pointplot();
  void setData(double* iData, double* qData, int n);

private:
  QwtPlotCurve* curve_;

  QwtPlotPanner* panner_;
  QwtPlotZoomer* zoomer_;
  QwtPlotMagnifier* magnifier_;

  struct opReal{double operator()(std::complex<double> i) const{return real(i);}};
  struct opImag{double operator()(std::complex<double> i) const{return imag(i);}};

  double* realPoints_;
  double* imagPoints_;

  int numPoints_;
  int counter_;
};

#endif // POINTPLOT_H
