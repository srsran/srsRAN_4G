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


#ifndef _plot_h
#define _plot_h

#ifdef __cplusplus
extern "C" {
#endif

#include "liblte/config.h"

#include "liblte/graphics/plot/plot_real.h"
#include "liblte/graphics/plot/plot_scatter.h"
#include "liblte/graphics/plot/plot_complex.h"
#include "liblte/graphics/plot/plot_waterfall.h"

LIBLTE_API int plot_init();
LIBLTE_API void plot_exit();

#ifdef __cplusplus
}
#endif

#endif
