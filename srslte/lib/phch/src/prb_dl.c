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


#include <stdbool.h>
#include <string.h>

#include "prb_dl.h"
#include "srslte/common/phy_common.h"

//#define DEBUG_IDX

#ifdef DEBUG_IDX
extern cf_t *offset_original;
SRSLTE_API int indices[100000];
SRSLTE_API int indices_ptr=0;
#endif

void print_indexes(cf_t *offset, int len) {
#ifdef DEBUG_IDX
  for (int i=0;i<len;i++) {
    indices[(i+indices_ptr)%100000]=offset-offset_original+i;
  }
  indices_ptr+=len;
#endif
}

void prb_cp_ref(cf_t **input, cf_t **output, int offset, int nof_refs,
    int nof_intervals, bool advance_output) {
  int i;

  int ref_interval = ((SRSLTE_NRE / nof_refs) - 1);
  memcpy(*output, *input, offset * sizeof(cf_t));
  print_indexes(*input, offset);
  *input += offset;
  *output += offset;
  for (i = 0; i < nof_intervals - 1; i++) {
    if (advance_output) {
      (*output)++;
    } else {
      (*input)++;
    }
    memcpy(*output, *input, ref_interval * sizeof(cf_t));
    print_indexes(*input, ref_interval);
    *output += ref_interval;
    *input += ref_interval;
  }
  if (ref_interval - offset > 0) {
    if (advance_output) {
      (*output)++;
    } else {
      (*input)++;
    }
    memcpy(*output, *input, (ref_interval - offset) * sizeof(cf_t));
    print_indexes(*input, ref_interval-offset);
    *output += (ref_interval - offset);
    *input += (ref_interval - offset);
  }
}

void prb_cp(cf_t **input, cf_t **output, int nof_prb) {
  memcpy(*output, *input, sizeof(cf_t) * SRSLTE_NRE * nof_prb);
  print_indexes(*input, SRSLTE_NRE);
  *input += nof_prb * SRSLTE_NRE;
  *output += nof_prb * SRSLTE_NRE;
}


void prb_cp_half(cf_t **input, cf_t **output, int nof_prb) {
  memcpy(*output, *input, sizeof(cf_t) * SRSLTE_NRE * nof_prb / 2);
  print_indexes(*input, SRSLTE_NRE/2);
  *input += nof_prb * SRSLTE_NRE / 2;
  *output += nof_prb * SRSLTE_NRE / 2;
}

void prb_put_ref_(cf_t **input, cf_t **output, int offset, int nof_refs,
    int nof_intervals) {
  prb_cp_ref(input, output, offset, nof_refs, nof_intervals, false);
}

void prb_get_ref_(cf_t **input, cf_t **output, int offset, int nof_refs,
    int nof_intervals) {
  prb_cp_ref(input, output, offset, nof_refs, nof_intervals, true);
}

