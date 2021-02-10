/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/dft.h"
#include "srslte/phy/dft/dft_precoding.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

/* Create DFT plans for transform precoding */

int srslte_dft_precoding_init(srslte_dft_precoding_t* q, uint32_t max_prb, bool is_tx)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  bzero(q, sizeof(srslte_dft_precoding_t));

  if (max_prb <= SRSLTE_MAX_PRB) {
    ret = SRSLTE_ERROR;
    for (uint32_t i = 1; i <= max_prb; i++) {
      if (srslte_dft_precoding_valid_prb(i)) {
        DEBUG("Initiating DFT precoding plan for %d PRBs", i);
        if (srslte_dft_plan_c(&q->dft_plan[i], i * SRSLTE_NRE, is_tx ? SRSLTE_DFT_FORWARD : SRSLTE_DFT_BACKWARD)) {
          ERROR("Error: Creating DFT plan %d", i);
          goto clean_exit;
        }
        srslte_dft_plan_set_norm(&q->dft_plan[i], true);
      }
    }
    q->max_prb = max_prb;
    ret        = SRSLTE_SUCCESS;
  }

clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_dft_precoding_free(q);
  }
  return ret;
}

int srslte_dft_precoding_init_rx(srslte_dft_precoding_t* q, uint32_t max_prb)
{
  return srslte_dft_precoding_init(q, max_prb, false);
}

int srslte_dft_precoding_init_tx(srslte_dft_precoding_t* q, uint32_t max_prb)
{
  return srslte_dft_precoding_init(q, max_prb, true);
}

/* Free DFT plans for transform precoding */
void srslte_dft_precoding_free(srslte_dft_precoding_t* q)
{
  for (uint32_t i = 1; i <= q->max_prb; i++) {
    if (srslte_dft_precoding_valid_prb(i)) {
      srslte_dft_plan_free(&q->dft_plan[i]);
    }
  }
  bzero(q, sizeof(srslte_dft_precoding_t));
}

static bool valid_prb[101] = {true,  true,  true,  true,  true,  true,  true,  false, true,  true,  true,  false, true,
                              false, false, true,  true,  false, true,  false, true,  false, false, false, true,  true,
                              false, true,  false, false, true,  false, true,  false, false, false, true,  false, false,
                              false, true,  false, false, false, false, true,  false, false, true,  false, true,  false,
                              false, false, true,  false, false, false, false, false, true,  false, false, false, true,
                              false, false, false, false, false, false, false, true,  false, false, true,  false, false,
                              false, false, true,  true,  false, false, false, false, false, false, false, false, true,
                              false, false, false, false, false, true,  false, false, false, true};

bool srslte_dft_precoding_valid_prb(uint32_t nof_prb)
{
  if (nof_prb <= 100) {
    return valid_prb[nof_prb];
  }
  return false;
}

/* Return largest integer that fulfills the DFT precoding PRB criterion (TS 36.213 Section 14.1.1.4C) */
uint32_t srslte_dft_precoding_get_valid_prb(uint32_t nof_prb)
{
  while (srslte_dft_precoding_valid_prb(nof_prb) == false) {
    nof_prb--;
  }
  return nof_prb;
}

int srslte_dft_precoding(srslte_dft_precoding_t* q, cf_t* input, cf_t* output, uint32_t nof_prb, uint32_t nof_symbols)
{
  if (!srslte_dft_precoding_valid_prb(nof_prb) && nof_prb <= q->max_prb) {
    ERROR("Error invalid number of PRB (%d)", nof_prb);
    return SRSLTE_ERROR;
  }

  for (uint32_t i = 0; i < nof_symbols; i++) {
    srslte_dft_run_c(&q->dft_plan[nof_prb], &input[i * SRSLTE_NRE * nof_prb], &output[i * SRSLTE_NRE * nof_prb]);
  }

  return SRSLTE_SUCCESS;
}
