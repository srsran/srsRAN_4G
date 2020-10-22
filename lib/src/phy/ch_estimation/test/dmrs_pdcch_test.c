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

#include "srslte/common/test_common.h"
#include "srslte/phy/ch_estimation/dmrs_pdcch.h"
#include "srslte/phy/phch/pdcch_nr.h"
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

static srslte_carrier_nr_t carrier = {};

static uint16_t rnti = 0x1234;

void usage(char* prog)
{
  printf("Usage: %s [recov]\n", prog);

  printf("\t-r nof_prb [Default %d]\n", carrier.nof_prb);
  printf("\t-e extended cyclic prefix [Default normal]\n");

  printf("\t-c cell_id [Default %d]\n", carrier.id);

  printf("\t-v increase verbosity\n");
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "recov")) != -1) {
    switch (opt) {
      case 'r':
        carrier.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        carrier.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

static int run_test(srslte_dmrs_pdcch_estimator_t* estimator, srslte_pdcch_cfg_nr_t* cfg, cf_t* sf_symbols, cf_t* h)
{
  for (uint32_t slot_idx = 0; slot_idx < SRSLTE_NSLOTS_PER_FRAME_NR(cfg->carrier.numerology); slot_idx++) {
    uint32_t locations[SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR] = {};

    int nof_locations = srslte_pdcch_nr_locations_coreset(
        &cfg->coreset, &cfg->search_space, cfg->rnti, cfg->aggregation_level, slot_idx, locations);

    TESTASSERT(nof_locations == cfg->search_space.nof_candidates[cfg->aggregation_level]);

    for (uint32_t candidate = 0; candidate < nof_locations; candidate++) {
      cfg->n_cce = locations[candidate];

      uint32_t nof_re = carrier.nof_prb * SRSLTE_NRE * SRSLTE_NSYMB_PER_SLOT_NR;
      srslte_vec_cf_zero(sf_symbols, nof_re);

      TESTASSERT(srslte_dmrs_pdcch_put(cfg, slot_idx, sf_symbols) == SRSLTE_SUCCESS);

      TESTASSERT(srslte_dmrs_pdcch_estimate(estimator, slot_idx, sf_symbols) == SRSLTE_SUCCESS);

      srslte_dmrs_pdcch_measure_t measure = {};
      TESTASSERT(
          srslte_dmrs_pdcch_get_measure(
              estimator, &cfg->search_space, slot_idx, cfg->aggregation_level, cfg->n_cce, cfg->rnti, &measure) ==
          SRSLTE_SUCCESS);

      if (fabsf(measure.rsrp - 1.0f) > 1e-2) {
        printf("EPRE=%f; RSRP=%f; CFO=%f; SYNC_ERR=%f;\n",
               measure.epre,
               measure.rsrp,
               measure.cfo_hz,
               measure.sync_error_us);
      }
      TESTASSERT(fabsf(measure.epre - 1.0f) < 1e-3f);
      TESTASSERT(fabsf(measure.rsrp - 1.0f) < 1e-3f);
      TESTASSERT(fabsf(measure.cfo_hz) < 1e-3f);
      TESTASSERT(fabsf(measure.sync_error_us) < 1e-3f);
    }
  }

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  int ret = SRSLTE_ERROR;

  carrier.nof_prb = 50;

  parse_args(argc, argv);

  srslte_pdcch_cfg_nr_t         cfg       = {};
  srslte_dmrs_pdcch_estimator_t estimator = {};

  uint32_t nof_re     = carrier.nof_prb * SRSLTE_NRE * SRSLTE_NSYMB_PER_SLOT_NR;
  cf_t*    sf_symbols = srslte_vec_cf_malloc(nof_re);
  cf_t*    h          = srslte_vec_cf_malloc(nof_re);

  uint32_t test_counter = 0;
  uint32_t test_passed  = 0;

  cfg.carrier              = carrier;
  cfg.rnti                 = rnti;
  cfg.coreset.mapping_type = srslte_coreset_mapping_type_non_interleaved;

  uint32_t nof_frequency_resource = SRSLTE_MIN(SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE, carrier.nof_prb / 6);
  for (uint32_t frequency_resources = 1; frequency_resources < (1U << nof_frequency_resource); frequency_resources++) {
    uint32_t nof_freq_resources = 0;
    for (uint32_t i = 0; i < nof_frequency_resource; i++) {
      uint32_t mask                 = ((frequency_resources >> i) & 1U);
      cfg.coreset.freq_resources[i] = (mask == 1);
      nof_freq_resources += mask;
    }

    for (cfg.coreset.duration = 1; cfg.coreset.duration <= 3; cfg.coreset.duration++) {

      for (cfg.search_space.type = srslte_search_space_type_common;
           cfg.search_space.type <= srslte_search_space_type_ue;
           cfg.search_space.type++) {

        for (uint32_t i = 0; i < SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR; i++) {
          uint32_t L                         = 1U << i;
          uint32_t nof_reg                   = cfg.coreset.duration * nof_freq_resources * 6;
          uint32_t nof_cce                   = nof_reg / 6;
          cfg.search_space.nof_candidates[i] = SRSLTE_MIN(nof_cce / L, SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR);
        }

        for (cfg.aggregation_level = 0; cfg.aggregation_level < SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR;
             cfg.aggregation_level++) {

          srslte_dmrs_pdcch_estimator_init(&estimator, &cfg.carrier, &cfg.coreset);

          if (run_test(&estimator, &cfg, sf_symbols, h)) {
            ERROR("Test %d failed\n", test_counter);
          } else {
            test_passed++;
          }
          test_counter++;
        }
      }
    }
  }

  srslte_dmrs_pdcch_estimator_free(&estimator);

  if (sf_symbols) {
    free(sf_symbols);
  }

  if (h) {
    free(h);
  }

  ret = test_passed == test_counter ? SRSLTE_SUCCESS : SRSLTE_ERROR;
  printf("%s, %d of %d test passed successfully.\n", ret ? "Failed" : "Passed", test_passed, test_counter);

  return ret;
}
