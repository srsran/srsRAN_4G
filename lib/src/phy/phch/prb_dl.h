/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#ifndef SRSRAN_PRB_DL_H_
#define SRSRAN_PRB_DL_H_

#include "srsran/config.h"

void prb_cp_ref(cf_t** input, cf_t** output, int offset, int nof_refs, int nof_intervals, bool advance_input);
void prb_cp(cf_t** input, cf_t** output, int nof_prb);
void prb_cp_half(cf_t** input, cf_t** output, int nof_prb);
void prb_put_ref_(cf_t** input, cf_t** output, int offset, int nof_refs, int nof_intervals);

#endif /* SRSRAN_PRB_DL_H_ */
