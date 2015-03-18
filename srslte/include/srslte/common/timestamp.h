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


#ifndef TIMESTAMP_
#define TIMESTAMP_

#include <time.h>
#include "srslte/config.h"

/*!
 * A simple timestamp struct with separate variables for full and frac seconds.
 *
 * Separate variables are used to avoid loss of precision in our frac seconds.
 * Only positive timestamps are supported.
 */

typedef struct SRSLTE_API{
  time_t full_secs;
  double frac_secs;
}timestamp_t;

SRSLTE_API int timestamp_init(timestamp_t *t, time_t full_secs, double frac_secs);
SRSLTE_API int timestamp_copy(timestamp_t *dest, timestamp_t *src);
SRSLTE_API int timestamp_add(timestamp_t *t, time_t full_secs, double frac_secs);
SRSLTE_API int timestamp_sub(timestamp_t *t, time_t full_secs, double frac_secs);
SRSLTE_API double timestamp_real(timestamp_t *t);

#endif // TIMESTAMP_
