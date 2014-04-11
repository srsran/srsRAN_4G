/**
 * \file lib/generic/graphics/qt/common/Spectrogramplot.cpp
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
 * A spectrogram plot which acts as a waterfall. New data is plotted
 * at the top row of the spectrogram and all old data is shifted
 * downwards.
 */

#include <qprinter.h>
#include <qprintdialog.h>
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include <qwt_matrix_raster_data.h>
#include "Spectrogramplot.h"

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

//Set up a colormap to use the "jet" colormap from matlab
class ColorMap
  :public QwtLinearColorMap
{
public:
  ColorMap()
    :QwtLinearColorMap(QColor(0,0,189), QColor(132,0,0))
  {
    double pos;
    pos = 1.0/13.0*1.0; addColorStop(pos, QColor(0,0,255));
    pos = 1.0/13.0*2.0; addColorStop(pos, QColor(0,66,255));
    pos = 1.0/13.0*3.0; addColorStop(pos, QColor(0,132,255));
    pos = 1.0/13.0*4.0; addColorStop(pos, QColor(0,189,255));
    pos = 1.0/13.0*5.0; addColorStop(pos, QColor(0,255,255));
    pos = 1.0/13.0*6.0; addColorStop(pos, QColor(66,255,189));
    pos = 1.0/13.0*7.0; addColorStop(pos, QColor(132,255,132));
    pos = 1.0/13.0*8.0; addColorStop(pos, QColor(189,255,66));
    pos = 1.0/13.0*9.0; addColorStop(pos, QColor(255,255,0));
    pos = 1.0/13.0*10.0; addColorStop(pos, QColor(255,189,0));
    pos = 1.0/13.0*12.0; addColorStop(pos, QColor(255,66,0));
    pos = 1.0/13.0*13.0; addColorStop(pos, QColor(189,0,0));
  }
};

Spectrogramplot::Spectrogramplot(int numDataPoints, int numRows, QWidget *parent)
  :QwtPlot(parent)
  ,nData_(numDataPoints)
  ,nRows_(numRows)
{
  spectrogram_ = new QwtPlotSpectrogram();
  spectrogram_->setRenderThreadCount(0); // set system specific thread count
  data_ = new WaterfallData(nData_, nRows_);
  spectrogram_->attach(this);

  setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
  setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);

  axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating,true);
  axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Floating,true);

  spectrogram_->setColorMap(new ColorMap());
  spectrogram_->setData(data_);

  setXAxisRange(0, nData_);
  setYAxisRange(0, nRows_);
  setZAxisScale(-1,1);

  // LeftButton for the zooming
  // MidButton for the panning
  // RightButton: zoom out by 1
  // Ctrl+RighButton: zoom out to full size

  zoomer_ = new MyZoomer(canvas());
  zoomer_->setMousePattern(QwtEventPattern::MouseSelect1,
      Qt::LeftButton);
  zoomer_->setMousePattern(QwtEventPattern::MouseSelect2,
      Qt::LeftButton, Qt::ControlModifier);

  panner_ = new QwtPlotPanner(canvas());
  panner_->setAxisEnabled(QwtPlot::yRight, false);
  panner_->setMouseButton(Qt::RightButton);

  magnifier_ = new QwtPlotMagnifier(canvas());
  magnifier_->setMouseButton(Qt::NoButton);

  // Avoid jumping when labels with more/less digits
  // appear/disappear when scrolling vertically

  const QFontMetrics fm(axisWidget(QwtPlot::yLeft)->font());
  QwtScaleDraw *sd = axisScaleDraw(QwtPlot::yLeft);
  sd->setMinimumExtent( fm.width("100.00") );

  const QColor c(Qt::darkBlue);
  zoomer_->setRubberBandPen(c);
  zoomer_->setTrackerPen(c);
}

void Spectrogramplot::appendData(double* data, int n)
{
  data_->appendData(data, n);
}

void Spectrogramplot::setXAxisRange(double xMin, double xMax)
{
  xMin_ = xMin;
  xMax_ = xMax;
  data_->setInterval( Qt::XAxis, QwtInterval( xMin_, xMax_ ) );
  plotLayout()->setAlignCanvasToScales(true);
  replot();
}

void Spectrogramplot::setYAxisRange(double yMin, double yMax)
{
  yMin_ = yMin;
  yMax_ = yMax;
  data_->setInterval( Qt::YAxis, QwtInterval( yMin_, yMax_ ) );
  plotLayout()->setAlignCanvasToScales(true);
  replot();
}

void Spectrogramplot::setZAxisScale(double zMin, double zMax)
{
  zMin_ = zMin;
  zMax_ = zMax;
  data_->setInterval( Qt::ZAxis, QwtInterval( zMin_, zMax_ ) );

  //Set up the intensity bar on the right
  const QwtInterval zInterval = spectrogram_->data()->interval( Qt::ZAxis );
  QwtScaleWidget *rightAxis = axisWidget(QwtPlot::yRight);
  rightAxis->setColorBarEnabled(true);
  rightAxis->setColorMap( zInterval, new ColorMap());
  setAxisScale(QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
  enableAxis(QwtPlot::yRight);

  plotLayout()->setAlignCanvasToScales(true);
  replot();
}

double Spectrogramplot::min()
{
  return data_->min();
}

double Spectrogramplot::max()
{
  return data_->max();
}

void Spectrogramplot::autoscale()
{
  setZAxisScale(min(),max());
}
