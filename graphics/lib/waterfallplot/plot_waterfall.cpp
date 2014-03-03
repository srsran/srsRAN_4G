/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */




#include "plot/plot_waterfall.h"
#include "Waterfallplot.h"
#include <complex.h>


int plot_waterfall_init(plot_waterfall_t *h, int numDataPoints, int numRows) {
	*h = (void*) new Waterfallplot(numDataPoints, numRows);
	return (*h != NULL)?0:-1;
}

void plot_waterfall_setTitle(plot_waterfall_t *h, char *title) {
	Waterfallplot *plot = static_cast<Waterfallplot*>(*h);
	plot->setTitle(title);
}

void plot_waterfall_appendNewData(plot_waterfall_t *h, float *data,
		int num_points) {
	Waterfallplot *plot = static_cast<Waterfallplot*>(*h);
	plot->appendNewData(data, num_points);
}

void plot_complex_setPlotXLabel(plot_waterfall_t *h, char *xLabel) {
	Waterfallplot *plot = static_cast<Waterfallplot*>(*h);
	plot->setPlotXLabel(xLabel);
}

void plot_complex_setPlotYLabel(plot_waterfall_t *h, char *yLabel) {
	Waterfallplot *plot = static_cast<Waterfallplot*>(*h);
	plot->setPlotXLabel(yLabel);
}

void plot_waterfall_setPlotXAxisRange(plot_waterfall_t *h, double xMin, double xMax) {
	Waterfallplot *plot = static_cast<Waterfallplot*>(*h);
	plot->setPlotXAxisRange(xMin, xMax);
}

void plot_waterfall_setPlotXAxisScale(plot_waterfall_t *h, double xMin, double xMax) {
	Waterfallplot *plot = static_cast<Waterfallplot*>(*h);
	plot->setPlotXAxisScale(xMin, xMax);
}

void plot_waterfall_setPlotYAxisScale(plot_waterfall_t *h, double yMin, double yMax) {
	Waterfallplot *plot = static_cast<Waterfallplot*>(*h);
	plot->setPlotYAxisScale(yMin, yMax);
}


void plot_waterfall_setSpectrogramXLabel(plot_waterfall_t *h, char* xLabel) {
	Waterfallplot *plot = static_cast<Waterfallplot*>(*h);
	plot->setSpectrogramXLabel(xLabel);
}

void plot_waterfall_setSpectrogramYLabel(plot_waterfall_t *h, char* yLabel) {
	Waterfallplot *plot = static_cast<Waterfallplot*>(*h);
	plot->setSpectrogramYLabel(yLabel);
}

void plot_waterfall_setSpectrogramXAxisRange(plot_waterfall_t *h, double xMin, double xMax) {
	Waterfallplot *plot = static_cast<Waterfallplot*>(*h);
	plot->setSpectrogramXAxisRange(xMin, xMax);
}

void plot_waterfall_setSpectrogramYAxisRange(plot_waterfall_t *h, double yMin, double yMax) {
	Waterfallplot *plot = static_cast<Waterfallplot*>(*h);
	plot->setSpectrogramYAxisRange(yMin, yMax);
}

void plot_waterfall_setSpectrogramZAxisScale(plot_waterfall_t *h, double zMin, double zMax) {
	Waterfallplot *plot = static_cast<Waterfallplot*>(*h);
	plot->setSpectrogramZAxisScale(zMin, zMax);
}

