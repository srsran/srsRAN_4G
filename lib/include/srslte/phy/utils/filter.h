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
 *  File:         debug.h
 *
 *  Description:  Debug output utilities.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_FILTER_H
#define SRSLTE_FILTER_H

#include <stdlib.h>
#include <stdio.h>
#include "srslte/config.h"
#include <stdbool.h>
#include "srslte/phy/utils/vector.h"
typedef struct SRSLTE_API{
    cf_t *filter_input;
    cf_t *downsampled_input;
    cf_t *filter_output;
    bool is_decimator;
    int factor;
    int num_taps;
    float *taps;
    
}srslte_filt_cc_t;

void srslte_filt_decim_cc_init(srslte_filt_cc_t *q, int factor, int order);

void srslte_filt_decim_cc_free(srslte_filt_cc_t *q);

void srslte_filt_decim_cc_execute(srslte_filt_cc_t *q, cf_t *input, cf_t *downsampled_input, cf_t *output, int size);

void srslte_downsample_cc(cf_t *input, cf_t *output, int M, int size) ;
#endif // SRSLTE_FILTER_H