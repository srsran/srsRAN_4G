/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */



#ifndef _plot_complex_h
#define _plot_complex_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "srslte/config.h"

typedef enum {
  Ip, Q, Magnitude, Phase
} plot_complex_id_t;

typedef void* plot_complex_t;

SRSLTE_API int plot_complex_init(plot_complex_t *h);
SRSLTE_API void plot_complex_setTitle(plot_complex_t *h, char *title);
SRSLTE_API void plot_complex_setNewData(plot_complex_t *h, _Complex float *data,
    int num_points);
SRSLTE_API void plot_complex_setXAxisAutoScale(plot_complex_t *h, plot_complex_id_t id, bool on);
SRSLTE_API void plot_complex_setYAxisAutoScale(plot_complex_t *h, plot_complex_id_t id, bool on);
SRSLTE_API void plot_complex_setXAxisScale(plot_complex_t *h, plot_complex_id_t id, double xMin, double xMax);
SRSLTE_API void plot_complex_setYAxisScale(plot_complex_t *h, plot_complex_id_t id, double yMin, double yMax);
SRSLTE_API void plot_complex_setXAxisRange(plot_complex_t *h, double xMin, double xMax);

#ifdef __cplusplus
}
#endif

#endif
