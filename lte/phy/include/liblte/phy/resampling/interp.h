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

#ifndef INTERP_H
#define INTERP_H_

#include "liblte/config.h"

typedef _Complex float cf_t;


LIBLTE_API void interp_linear_offset(cf_t *input, cf_t *output, int M, int len, int off_st, int off_end);
LIBLTE_API void interp_linear(cf_t *input, cf_t *output, int M, int len);
LIBLTE_API void interp_linear_f(float *input, float *output, int M, int len);

#endif // INTERP_H
