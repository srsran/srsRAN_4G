/**
 * \file lib/generic/graphics/qt/common/Lineplot.cpp
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
 * Implementation of a simple line plotted using a QwtPlot.
 */

#include "Lineplot.h"

#include <algorithm>

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

Lineplot::Lineplot(QWidget *parent)
  :QwtPlot(parent)
  ,xMin_(0)
  ,xMax_(0)
{
  counter_ = 0;
  numPoints_ = 1;
  indexPoints_ = new double[numPoints_];
  dataPoints_ = new double[numPoints_];

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QPalette palette;
  palette.setColor(canvas()->backgroundRole(), QColor("white"));
  canvas()->setPalette(palette);

  curve_ = new QwtPlotCurve("Curve");
  curve_->setPen(QPen(Qt::blue, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  curve_->setStyle(QwtPlotCurve::Lines);
  curve_->setRawSamples(indexPoints_, dataPoints_, numPoints_);
  curve_->setYAxis(QwtPlot::yLeft);
  curve_->attach(this);

  memset(dataPoints_, 0x0, numPoints_*sizeof(double));
  for(int i=0;i<numPoints_;i++)
    indexPoints_[i] = i;

  enableAxis(QwtPlot::yRight);
  QwtScaleWidget *leftAxis = axisWidget(QwtPlot::yLeft);
  connect(leftAxis, SIGNAL(scaleDivChanged()), this, SLOT(linkScales()));

  setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
  setAxisScaleEngine(QwtPlot::yRight, new QwtLinearScaleEngine);

  axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating,true);
  axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Floating,true);
  axisScaleEngine(QwtPlot::yRight)->setAttribute(QwtScaleEngine::Floating,true);

  zoomer_ = new MyZoomer(canvas());
  zoomer_->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
  zoomer_->setMousePattern(QwtEventPattern::MouseSelect2, Qt::LeftButton,
                           Qt::ControlModifier);

  panner_ = new QwtPlotPanner(canvas());
  panner_->setMouseButton(Qt::RightButton);

  magnifier_ = new QwtPlotMagnifier(canvas());
  magnifier_->setMouseButton(Qt::NoButton);

}

Lineplot::~Lineplot()
{
  delete[] indexPoints_;
  delete[] dataPoints_;
}

void Lineplot::setData(double* data, int n)
{
  if(numPoints_ != n)
  {
    numPoints_ = n;
    delete[] indexPoints_;
    delete[] dataPoints_;
    indexPoints_ = new double[numPoints_];
    dataPoints_ = new double[numPoints_];
    if(xMin_==xMax_)
    {
      for(int i=0;i<numPoints_;i++)
        indexPoints_[i] = i;
    }
    else
    {
      double step = (xMax_-xMin_)/numPoints_;
      double val = xMin_;
      for(int i=0;i<numPoints_;i++,val+=step)
        indexPoints_[i] = val;
    }
  }

  memcpy(dataPoints_, data, numPoints_*sizeof(double));
  //Need to setRawSamples again for autoscaling to work
  curve_->setRawSamples(indexPoints_, dataPoints_, numPoints_);
  resetZoom();
}

void Lineplot::setXAxisRange(double xMin, double xMax)
{
  xMin_ = xMin;
  xMax_ = xMax;
  double step = (xMax_-xMin_)/numPoints_;
  double val = xMin_;
  for(int i=0;i<numPoints_;i++,val+=step)
    indexPoints_[i] = val;
  curve_->setRawSamples(indexPoints_, dataPoints_, numPoints_);
}

void Lineplot::resetZoom()
{
  zoomer_->setZoomBase(curve_->boundingRect());
}

void Lineplot::linkScales()
{
  setAxisScaleDiv(QwtPlot::yRight, *axisScaleDiv(QwtPlot::yLeft));
}
