/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "pmi_select_test.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

int main(int argc, char** argv)
{
  cf_t*    h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  float    noise_estimate;
  float    sinr_1l[SRSLTE_MAX_CODEBOOKS];
  float    sinr_2l[SRSLTE_MAX_CODEBOOKS];
  float    cn;
  uint32_t pmi[2];
  uint32_t nof_symbols = (uint32_t)SRSLTE_SF_LEN_RE(6, SRSLTE_CP_NORM);
  int      ret         = SRSLTE_ERROR;

  /* Allocate channels */
  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
    for (int j = 0; j < SRSLTE_MAX_PORTS; j++) {
      h[i][j] = srslte_vec_cf_malloc(nof_symbols);
      if (!h[i][j]) {
        goto clean;
      }
      srslte_vec_cf_zero(h[i][j], nof_symbols);
    }
  }

  for (int c = 0; c < PMI_SELECT_TEST_NOF_CASES; c++) {
    pmi_select_test_case_gold_t* gold = &pmi_select_test_case_gold[c];

    /* Set channel */
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
        cf_t hij = gold->h[i][j];

        for (int k = 0; k < nof_symbols; k++) {
          h[i][j][k] = hij;
        }
      }
    }

    /* Set noise estimate */
    noise_estimate = gold->n;

    /* PMI select for 1 layer */
    ret = srslte_precoding_pmi_select(h, nof_symbols, noise_estimate, 1, &pmi[0], sinr_1l);
    if (ret < 0) {
      ERROR("During PMI selection for 1 layer");
      goto clean;
    }

    /* Check SINR for 1 layer */
    for (int i = 0; i < ret; i++) {
      float err = fabsf(gold->snri_1l[i] - sinr_1l[i]);

      // Normalise to prevent floating point rounding error
      if (gold->snri_1l[i] > 1000.0f) {
        err /= gold->snri_1l[i];
      }

      if (err > 0.1f) {
        ERROR("Test case %d failed computing 1 layer SINR for codebook %d (test=%.2f; gold=%.2f)\n",
              c + 1,
              i,
              sinr_1l[i],
              gold->snri_1l[i]);
        goto clean;
      }
    }

    /* Check PMI select for 1 layer*/
    if (pmi[0] != gold->pmi[0]) {
      ERROR("Test case %d failed computing 1 layer PMI (test=%d; gold=%d)\n", c + 1, pmi[0], gold->pmi[0]);
      goto clean;
    }

    /* PMI select for 2 layer */
    ret = srslte_precoding_pmi_select(h, nof_symbols, noise_estimate, 2, &pmi[1], sinr_2l);
    if (ret < 0) {
      ERROR("During PMI selection for 2 layer");
      goto clean;
    }

    /* Check SINR for 2 layer */
    for (int i = 0; i < ret; i++) {
      float err = fabsf(gold->snri_2l[i] - sinr_2l[i]);

      // Normalise to prevent floating point rounding error
      if (gold->snri_2l[i] > 1000.0f) {
        err /= gold->snri_2l[i];
      }

      if (err > 0.1f) {
        ERROR("Test case %d failed computing 2 layer SINR for codebook %d (test=%.2f; gold=%.2f)\n",
              c + 1,
              i,
              sinr_2l[i],
              gold->snri_2l[i]);
        goto clean;
      }
    }

    /* Check PMI select for 2 layer*/
    if (pmi[1] != gold->pmi[1]) {
      ERROR("Test case %d failed computing 2 layer PMI (test=%d; gold=%d)\n", c + 1, pmi[1], gold->pmi[1]);
      goto clean;
    }

    /* Condition number */
    if (srslte_precoding_cn(h, 2, 2, nof_symbols, &cn)) {
      ERROR("Test case %d condition number returned error\n", c + 1);
      goto clean;
    }

    /* Check condition number */
    if (fabsf(gold->k - cn) > 0.1) {
      ERROR("Test case %d failed computing condition number (test=%.2f; gold=%.2f)\n", c + 1, cn, gold->k);
      goto clean;
    }
  }

  /* Test passed */
  ret = SRSLTE_SUCCESS;

clean:
  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
    for (int j = 0; j < SRSLTE_MAX_PORTS; j++) {
      if (h[i][j]) {
        free(h[i][j]);
      }
    }
  }

  if (ret) {
    printf("Failed!\n");
  } else {
    printf("Passed!\n");
  }

  return ret;
}
