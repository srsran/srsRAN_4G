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
 * Implementation of a plot of complex data values as points on an IQ axis.
 */

#include "Pointplot.h"
#include <algorithm>

using namespace std;

class MyZoomer: public QwtPlotZoomer
{
public:
    MyZoomer(QwtPlotCanvas *canvas):
        QwtPlotZoomer(canvas)
    {
        setTrackerMode(AlwaysOn);
    }

    virtual QwtText trackerTextF(const QPointF &pos) const
    {
        QColor bg(Qt::white);
        bg.setAlpha(200);

        QwtText text = QwtPlotZoomer::trackerTextF(pos);
        text.setBackgroundBrush( QBrush( bg ));
        return text;
    }
};

Pointplot::Pointplot(QWidget *parent)
  :QwtPlot(parent)
{
  counter_ = 0;
  numPoints_ = 1;
  realPoints_ = new double[numPoints_];
  imagPoints_ = new double[numPoints_];

  QPalette palette;
  palette.setColor(canvas()->backgroundRole(), QColor("white"));
  canvas()->setPalette(palette);

  setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  setAxisTitle(QwtPlot::xBottom, "In-phase");

  setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
  setAxisTitle(QwtPlot::yLeft, "Quadrature");

  curve_ = new QwtPlotCurve("Constellation Points");
  curve_->attach(this);
  curve_->setPen(QPen(Qt::blue, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  curve_->setStyle(QwtPlotCurve::Dots);
  curve_->setRawSamples(realPoints_, imagPoints_, numPoints_);

  memset(realPoints_, 0x0, numPoints_*sizeof(double));
  memset(imagPoints_, 0x0, numPoints_*sizeof(double));

  zoomer_ = new MyZoomer(canvas());
  zoomer_->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
  zoomer_->setMousePattern(QwtEventPattern::MouseSelect2, Qt::LeftButton,
                           Qt::ControlModifier);

  panner_ = new QwtPlotPanner(canvas());
  panner_->setMouseButton(Qt::RightButton);

  magnifier_ = new QwtPlotMagnifier(canvas());
  magnifier_->setMouseButton(Qt::NoButton);

}

Pointplot::~Pointplot()
{
  delete[] realPoints_;
  delete[] imagPoints_;
}

void Pointplot::setData(double* iData, double* qData, int n)
{
  if(numPoints_ != n)
  {
    numPoints_ = n;
    delete[] realPoints_;
    delete[] imagPoints_;
    realPoints_ = new double[numPoints_];
    imagPoints_ = new double[numPoints_];
  }

  copy(iData, iData+n, realPoints_);
  copy(qData, qData+n, imagPoints_);
  //Need to setRawSamples again for autoscaling to work
  curve_->setRawSamples(realPoints_, imagPoints_, numPoints_);
  zoomer_->setZoomBase(curve_->boundingRect());
}
