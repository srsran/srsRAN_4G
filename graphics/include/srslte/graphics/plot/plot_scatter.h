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


#ifndef _plot_scatter_h
#define _plot_scatter_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "srslte/config.h"

typedef void* plot_scatter_t;

LIBLTE_API int plot_scatter_init(plot_scatter_t *h);
LIBLTE_API void plot_scatter_setTitle(plot_scatter_t *h, char *title);
LIBLTE_API void plot_scatter_setNewData(plot_scatter_t *h, _Complex float *data,
    int num_points);
LIBLTE_API void plot_scatter_setXAxisAutoScale(plot_scatter_t *h, bool on);
LIBLTE_API void plot_scatter_setYAxisAutoScale(plot_scatter_t *h, bool on);
LIBLTE_API void plot_scatter_setXAxisScale(plot_scatter_t *h, double xMin, double xMax);
LIBLTE_API void plot_scatter_setYAxisScale(plot_scatter_t *h, double yMin, double yMax);
LIBLTE_API void plot_scatter_setAxisLabels(plot_scatter_t *h, char *xLabel, char *yLabel);

#ifdef __cplusplus
}
#endif

#endif
