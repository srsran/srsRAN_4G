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



#include "liblte/graphics/plot/plot_real.h"
#include "Realplot.h"
#include <complex.h>


int plot_real_init(plot_real_t *h) {
	*h = (void*) new Realplot();
	return (*h != NULL)?0:-1;
}

void plot_real_setTitle(plot_real_t *h, char *title) {
	Realplot *plot = static_cast<Realplot*>(*h);
	plot->setTitle(title);
}
void plot_real_setNewData(plot_real_t *h, float *data,
		int num_points) {
	Realplot *plot = static_cast<Realplot*>(*h);
	plot->setNewData(data, num_points);

}

void plot_real_setXAxisAutoScale(plot_real_t *h, bool on) {
	Realplot *plot = static_cast<Realplot*>(*h);
	plot->setXAxisAutoScale(on);
}

void plot_real_setYAxisAutoScale(plot_real_t *h, bool on) {
	Realplot *plot = static_cast<Realplot*>(*h);
	plot->setYAxisAutoScale(on);
}

void plot_real_setXAxisScale(plot_real_t *h, double xMin, double xMax) {
	Realplot *plot = static_cast<Realplot*>(*h);
	plot->setXAxisScale(xMin, xMax);
}

void plot_real_setYAxisScale(plot_real_t *h, double yMin, double yMax) {
	Realplot *plot = static_cast<Realplot*>(*h);
	plot->setYAxisScale(yMin, yMax);
}

void plot_real_setXAxisRange(plot_real_t *h, double xMin, double xMax) {
	Realplot *plot = static_cast<Realplot*>(*h);
	plot->setXAxisRange(xMin, xMax);
}

void plot_real_setLabels(plot_real_t *h, char *xLabel, char *yLabel) {
	Realplot *plot = static_cast<Realplot*>(*h);
	plot->setLabels(xLabel, yLabel);
}
