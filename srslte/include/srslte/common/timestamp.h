/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/**********************************************************************************************
 *  File:         timestamp.h
 *
 *  Description:  A simple timestamp struct with separate variables for full and frac seconds.
 *                Separate variables are used to avoid loss of precision in our frac seconds.
 *                Only positive timestamps are supported.
 *
 *  Reference:
 *********************************************************************************************/

#ifndef TIMESTAMP_
#define TIMESTAMP_

#include <time.h>
#include <stdint.h>
#include "srslte/config.h"

typedef struct SRSLTE_API{
  time_t full_secs;
  double frac_secs;
}srslte_timestamp_t;

SRSLTE_API int srslte_timestamp_init(srslte_timestamp_t *t, 
                                     time_t full_secs, 
                                     double frac_secs);

SRSLTE_API int srslte_timestamp_copy(srslte_timestamp_t *dest, 
                                     srslte_timestamp_t *src);

SRSLTE_API int srslte_timestamp_add(srslte_timestamp_t *t, 
                                    time_t full_secs, 
                                    double frac_secs);

SRSLTE_API int srslte_timestamp_sub(srslte_timestamp_t *t, 
                                    time_t full_secs, 
                                    double frac_secs);

SRSLTE_API double srslte_timestamp_real(srslte_timestamp_t *t);

SRSLTE_API uint32_t srslte_timestamp_uint32(srslte_timestamp_t *t);

#endif // TIMESTAMP_
