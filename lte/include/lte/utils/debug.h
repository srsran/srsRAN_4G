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

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include "lte/config.h"

#define VERBOSE_DEBUG 	2
#define VERBOSE_INFO	1
#define VERBOSE_NONE	0

#include <sys/time.h>
LIBLTE_API void get_time_interval(struct timeval * tdata);

#ifndef DEBUG_DISABLED

LIBLTE_API extern int verbose;

#define VERBOSE_ISINFO() (verbose>=VERBOSE_INFO)
#define VERBOSE_ISDEBUG() (verbose>=VERBOSE_DEBUG)

#define PRINT_DEBUG verbose=VERBOSE_DEBUG
#define PRINT_INFO verbose=VERBOSE_INFO
#define PRINT_NONE verbose=VERBOSE_NONE

#define DEBUG(_fmt, ...) if (verbose >= VERBOSE_DEBUG) \
	fprintf(stdout, "[DEBUG]: " _fmt, __VA_ARGS__)

#define INFO(_fmt, ...) if (verbose >= VERBOSE_INFO) \
	fprintf(stdout, "[INFO]:  " _fmt, __VA_ARGS__)

#else // DEBUG_DISABLED

#define DEBUG
#define INFO

#endif // DEBUG_DISABLED

#endif // DEBUG_H
