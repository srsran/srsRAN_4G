/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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

#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/dft.h"
#include "srsran/phy/dft/dft_precoding.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

/* Create DFT plans for transform precoding */

int srsran_dft_precoding_init(srsran_dft_precoding_t* q, uint32_t max_prb, bool is_tx)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  bzero(q, sizeof(srsran_dft_precoding_t));

  if (max_prb <= SRSRAN_MAX_PRB) {
    ret = SRSRAN_ERROR;
    for (uint32_t i = 1; i <= max_prb; i++) {
      if (srsran_dft_precoding_valid_prb(i)) {
        DEBUG("Initiating DFT precoding plan for %d PRBs", i);
        if (srsran_dft_plan_c(&q->dft_plan[i], i * SRSRAN_NRE, is_tx ? SRSRAN_DFT_FORWARD : SRSRAN_DFT_BACKWARD)) {
          ERROR("Error: Creating DFT plan %d", i);
          goto clean_exit;
        }
        srsran_dft_plan_set_norm(&q->dft_plan[i], true);
      }
    }
    q->max_prb = max_prb;
    ret        = SRSRAN_SUCCESS;
  }

clean_exit:
  if (ret == SRSRAN_ERROR) {
    srsran_dft_precoding_free(q);
  }
  return ret;
}

int srsran_dft_precoding_init_rx(srsran_dft_precoding_t* q, uint32_t max_prb)
{
  return srsran_dft_precoding_init(q, max_prb, false);
}

int srsran_dft_precoding_init_tx(srsran_dft_precoding_t* q, uint32_t max_prb)
{
  return srsran_dft_precoding_init(q, max_prb, true);
}

/* Free DFT plans for transform precoding */
void srsran_dft_precoding_free(srsran_dft_precoding_t* q)
{
  for (uint32_t i = 1; i <= q->max_prb; i++) {
    if (srsran_dft_precoding_valid_prb(i)) {
      srsran_dft_plan_free(&q->dft_plan[i]);
    }
  }
  bzero(q, sizeof(srsran_dft_precoding_t));
}

static bool valid_prb[101] = {true,  true,  true,  true,  true,  true,  true,  false, true,  true,  true,  false, true,
                              false, false, true,  true,  false, true,  false, true,  false, false, false, true,  true,
                              false, true,  false, false, true,  false, true,  false, false, false, true,  false, false,
                              false, true,  false, false, false, false, true,  false, false, true,  false, true,  false,
                              false, false, true,  false, false, false, false, false, true,  false, false, false, true,
                              false, false, false, false, false, false, false, true,  false, false, true,  false, false,
                              false, false, true,  true,  false, false, false, false, false, false, false, false, true,
                              false, false, false, false, false, true,  false, false, false, true};

bool srsran_dft_precoding_valid_prb(uint32_t nof_prb)
{
  if (nof_prb <= 100) {
    return valid_prb[nof_prb];
  }
  return false;
}

/* Return largest integer that fulfills the DFT precoding PRB criterion (TS 36.213 Section 14.1.1.4C) */
uint32_t srsran_dft_precoding_get_valid_prb(uint32_t nof_prb)
{
  while (srsran_dft_precoding_valid_prb(nof_prb) == false) {
    nof_prb--;
  }
  return nof_prb;
}

int srsran_dft_precoding(srsran_dft_precoding_t* q, cf_t* input, cf_t* output, uint32_t nof_prb, uint32_t nof_symbols)
{
  if (!srsran_dft_precoding_valid_prb(nof_prb) && nof_prb <= q->max_prb) {
    ERROR("Error invalid number of PRB (%d)", nof_prb);
    return SRSRAN_ERROR;
  }

  for (uint32_t i = 0; i < nof_symbols; i++) {
    srsran_dft_run_c(&q->dft_plan[nof_prb], &input[i * SRSRAN_NRE * nof_prb], &output[i * SRSRAN_NRE * nof_prb]);
  }

  return SRSRAN_SUCCESS;
}
