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


#ifndef _plot_real_h
#define _plot_real_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "lte/config.h"

typedef void* plot_real_t;

LIBLTE_API int plot_real_init(plot_real_t *h);
LIBLTE_API void plot_real_setTitle(plot_real_t *h, char *title);
LIBLTE_API void plot_real_setNewData(plot_real_t *h, float *data,
		int num_points);
LIBLTE_API void plot_real_setXAxisAutoScale(plot_real_t *h, bool on);
LIBLTE_API void plot_real_setYAxisAutoScale(plot_real_t *h, bool on);
LIBLTE_API void plot_real_setXAxisScale(plot_real_t *h, double xMin, double xMax);
LIBLTE_API void plot_real_setYAxisScale(plot_real_t *h, double yMin, double yMax);
LIBLTE_API void plot_real_setXAxisRange(plot_real_t *h, double xMin, double xMax);
LIBLTE_API void plot_real_setLabels(plot_real_t *h, char *xLabel, char *yLabel);

#ifdef __cplusplus
}
#endif

#endif
