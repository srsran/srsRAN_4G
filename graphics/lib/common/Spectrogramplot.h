/**
 * \file lib/generic/graphics/qt/common/Spectrogramplot.h
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

#ifndef SPECTROGRAMPLOT_H
#define SPECTROGRAMPLOT_H

#include <qwt_plot.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_spectrogram.h>
#include "WaterfallData.h"

class Spectrogramplot
  :public QwtPlot
{
    Q_OBJECT

public:
    Spectrogramplot(int numDataPoints, int numRows, QWidget * = NULL);
    void appendData(double* data, int n);
    void setXAxisRange(double xMin, double xMax);
    void setYAxisRange(double yMin, double yMax);
    void setZAxisScale(double zMin, double zMax);
    double min();
    double max();
    void autoscale();

private:
    QwtPlotZoomer* zoomer_;
    QwtPlotPanner *panner_;
    QwtPlotMagnifier *magnifier_;
    QwtPlotSpectrogram *spectrogram_;
    WaterfallData* data_;
    int nData_;
    int nRows_;
    double xMin_;
    double xMax_;
    double yMin_;
    double yMax_;
    double zMin_;
    double zMax_;
};

#endif // SPECTROGRAMPLOT_H
