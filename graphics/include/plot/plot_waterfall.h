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


#ifndef _plot_waterfall_h
#define _plot_waterfall_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef void* plot_waterfall_t;

int plot_waterfall_init(plot_waterfall_t *h);
void plot_waterfall_setTitle(plot_waterfall_t *h, char *title);
void plot_waterfall_appendNewData(plot_waterfall_t *h, float *data,
		int num_points);
void plot_complex_setPlotXLabel(plot_waterfall_t *h, char *xLabel);
void plot_complex_setPlotYLabel(plot_waterfall_t *h, char *yLabel);
void plot_waterfall_setPlotXAxisRange(plot_waterfall_t *h, double xMin, double xMax);
void plot_waterfall_setPlotXAxisScale(plot_waterfall_t *h, double xMin, double xMax);
void plot_waterfall_setPlotYAxisScale(plot_waterfall_t *h, double yMin, double yMax);

void plot_waterfall_setSpectrogramXLabel(plot_waterfall_t *h, char* xLabel);
void plot_waterfall_setSpectrogramYLabel(plot_waterfall_t *h, char* yLabel);
void plot_waterfall_setSpectrogramXAxisRange(plot_waterfall_t *h, double xMin, double xMax);
void plot_waterfall_setSpectrogramYAxisRange(plot_waterfall_t *h, double yMin, double yMax);
void plot_waterfall_setSpectrogramZAxisScale(plot_waterfall_t *h, double zMin, double zMax);


#ifdef __cplusplus
}
#endif

#endif
