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

/******************************************************************************
 *  File:         sfo.h
 *
 *  Description:  Sampling frequency offset estimation.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_SFO_H
#define SRSLTE_SFO_H

#include "srslte/config.h"

SRSLTE_API float srslte_sfo_estimate(int *t0, 
                                     int len, 
                                     float period);

SRSLTE_API float srslte_sfo_estimate_period(int *t0, 
                                            int *t, 
                                            int len, 
                                            float period);

#endif // SRSLTE_SFO_H
