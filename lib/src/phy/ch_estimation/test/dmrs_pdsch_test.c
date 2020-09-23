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
#include "srslte/srslte.h"
#include <complex.h>
#include <srslte/phy/ch_estimation/dmrs_pdsch.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

static srslte_cell_t cell = {50,             // nof_prb
                             1,              // nof_ports
                             1,              // cell_id
                             SRSLTE_CP_NORM, // cyclic prefix
                             SRSLTE_PHICH_NORM,
                             SRSLTE_PHICH_R_1, // PHICH length
                             SRSLTE_FDD};

typedef struct {
  srslte_dmrs_pdsch_mapping_type_t mapping_type;
  srslte_dmrs_pdsch_typeA_pos_t    typeA_pos;
  srslte_dmrs_pdsch_len_t          max_length;
  srslte_dmrs_pdsch_add_pos_t      additional_pos;
  srslte_dmrs_pdsch_type_t         type;
  uint32_t                         symbol_idx[SRSLTE_DMRS_PDSCH_MAX_SYMBOLS];
  uint32_t                         nof_symbols;
  uint32_t                         sc_idx[SRSLTE_NRE];
  uint32_t                         nof_sc;
} golden_t;

// Golden values extracted from https://www.sharetechnote.com/html/5G/5G_PDSCH_DMRS.html
static const golden_t gold[] = {{.mapping_type   = srslte_dmrs_pdsch_mapping_type_A,
                                 .typeA_pos      = srslte_dmrs_pdsch_typeA_pos_2,
                                 .max_length     = srslte_dmrs_pdsch_len_1,
                                 .additional_pos = srslte_dmrs_pdsch_add_pos_0,
                                 .type           = srslte_dmrs_pdsch_type_2,
                                 .nof_symbols    = 1,
                                 .symbol_idx     = {2},
                                 .nof_sc         = 4,
                                 .sc_idx         = {0, 1, 6, 7}},
                                {.mapping_type   = srslte_dmrs_pdsch_mapping_type_A,
                                 .typeA_pos      = srslte_dmrs_pdsch_typeA_pos_3,
                                 .max_length     = srslte_dmrs_pdsch_len_1,
                                 .additional_pos = srslte_dmrs_pdsch_add_pos_0,
                                 .type           = srslte_dmrs_pdsch_type_2,
                                 .nof_symbols    = 1,
                                 .symbol_idx     = {3},
                                 .nof_sc         = 4,
                                 .sc_idx         = {0, 1, 6, 7}},
                                {.mapping_type   = srslte_dmrs_pdsch_mapping_type_A,
                                 .typeA_pos      = srslte_dmrs_pdsch_typeA_pos_2,
                                 .max_length     = srslte_dmrs_pdsch_len_2,
                                 .additional_pos = srslte_dmrs_pdsch_add_pos_0,
                                 .type           = srslte_dmrs_pdsch_type_2,
                                 .nof_symbols    = 2,
                                 .symbol_idx     = {2, 3},
                                 .nof_sc         = 4,
                                 .sc_idx         = {0, 1, 6, 7}},
                                {.mapping_type   = srslte_dmrs_pdsch_mapping_type_A,
                                 .typeA_pos      = srslte_dmrs_pdsch_typeA_pos_2,
                                 .max_length     = srslte_dmrs_pdsch_len_1,
                                 .additional_pos = srslte_dmrs_pdsch_add_pos_1,
                                 .type           = srslte_dmrs_pdsch_type_2,
                                 .nof_symbols    = 2,
                                 .symbol_idx     = {2, 11},
                                 .nof_sc         = 4,
                                 .sc_idx         = {0, 1, 6, 7}},
                                {.mapping_type   = srslte_dmrs_pdsch_mapping_type_A,
                                 .typeA_pos      = srslte_dmrs_pdsch_typeA_pos_2,
                                 .max_length     = srslte_dmrs_pdsch_len_1,
                                 .additional_pos = srslte_dmrs_pdsch_add_pos_2,
                                 .type           = srslte_dmrs_pdsch_type_2,
                                 .nof_symbols    = 3,
                                 .symbol_idx     = {2, 7, 11},
                                 .nof_sc         = 4,
                                 .sc_idx         = {0, 1, 6, 7}},
                                {.mapping_type   = srslte_dmrs_pdsch_mapping_type_A,
                                 .typeA_pos      = srslte_dmrs_pdsch_typeA_pos_2,
                                 .max_length     = srslte_dmrs_pdsch_len_1,
                                 .additional_pos = srslte_dmrs_pdsch_add_pos_3,
                                 .type           = srslte_dmrs_pdsch_type_2,
                                 .nof_symbols    = 4,
                                 .symbol_idx     = {2, 5, 8, 11},
                                 .nof_sc         = 4,
                                 .sc_idx         = {0, 1, 6, 7}},
                                {.mapping_type   = srslte_dmrs_pdsch_mapping_type_A,
                                 .typeA_pos      = srslte_dmrs_pdsch_typeA_pos_2,
                                 .max_length     = srslte_dmrs_pdsch_len_1,
                                 .additional_pos = srslte_dmrs_pdsch_add_pos_0,
                                 .type           = srslte_dmrs_pdsch_type_1,
                                 .nof_symbols    = 1,
                                 .symbol_idx     = {2},
                                 .nof_sc         = 6,
                                 .sc_idx         = {0, 2, 4, 6, 8, 10}},
                                {.mapping_type   = srslte_dmrs_pdsch_mapping_type_A,
                                 .typeA_pos      = srslte_dmrs_pdsch_typeA_pos_2,
                                 .max_length     = srslte_dmrs_pdsch_len_2,
                                 .additional_pos = srslte_dmrs_pdsch_add_pos_0,
                                 .type           = srslte_dmrs_pdsch_type_1,
                                 .nof_symbols    = 2,
                                 .symbol_idx     = {2, 3},
                                 .nof_sc         = 6,
                                 .sc_idx         = {0, 2, 4, 6, 8, 10}},
                                {.mapping_type   = srslte_dmrs_pdsch_mapping_type_A,
                                 .typeA_pos      = srslte_dmrs_pdsch_typeA_pos_2,
                                 .max_length     = srslte_dmrs_pdsch_len_1,
                                 .additional_pos = srslte_dmrs_pdsch_add_pos_3,
                                 .type           = srslte_dmrs_pdsch_type_1,
                                 .nof_symbols    = 4,
                                 .symbol_idx     = {2, 5, 8, 11},
                                 .nof_sc         = 6,
                                 .sc_idx         = {0, 2, 4, 6, 8, 10}},
                                {}};

void usage(char* prog)
{
  printf("Usage: %s [recov]\n", prog);

  printf("\t-r nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-e extended cyclic prefix [Default normal]\n");

  printf("\t-c cell_id [Default %d]\n", cell.id);

  printf("\t-v increase verbosity\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "recov")) != -1) {
    switch (opt) {
      case 'r':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'e':
        cell.cp = SRSLTE_CP_EXT;
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
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

static int assert_cfg(srslte_dmrs_pdsch_t* dmrs_pdsch)
{
  for (uint32_t i = 0; gold[i].nof_sc != 0; i++) {
    if (dmrs_pdsch->cfg.mapping_type != gold[i].mapping_type) {
      continue;
    }

    if (dmrs_pdsch->cfg.typeA_pos != gold[i].typeA_pos) {
      continue;
    }

    if (dmrs_pdsch->cfg.additional_pos != gold[i].additional_pos) {
      continue;
    }

    if (dmrs_pdsch->cfg.length != gold[i].max_length) {
      continue;
    }

    if (dmrs_pdsch->cfg.type != gold[i].type) {
      continue;
    }

    TESTASSERT(dmrs_pdsch->nof_symbols == gold[i].nof_symbols);

    for (uint32_t j = 0; j < gold[i].nof_symbols; j++) {
      TESTASSERT(dmrs_pdsch->symbols_idx[j] == gold[i].symbol_idx[j]);
    }

    for (uint32_t j = 0; j < gold[i].nof_sc; j++) {
      TESTASSERT(dmrs_pdsch->sc_idx[j] == gold[i].sc_idx[j]);
    }

    return SRSLTE_SUCCESS;
  }

  return SRSLTE_SUCCESS;
}

static int run_test(srslte_dmrs_pdsch_t* dmrs_pdsch, cf_t* sf_symbols, cf_t* h)
{
  TESTASSERT(dmrs_pdsch->nof_symbols > 0);
  TESTASSERT(dmrs_pdsch->nof_sc > 0);

  TESTASSERT(assert_cfg(dmrs_pdsch) == SRSLTE_SUCCESS);

  srslte_dl_sf_cfg_t dl_sf = {};
  for (dl_sf.tti = 0; dl_sf.tti < SRSLTE_NOF_SF_X_FRAME; dl_sf.tti++) {
    srslte_dmrs_pdsch_put_sf(dmrs_pdsch, &dl_sf, sf_symbols);

    srslte_dmrs_pdsch_get_sf(dmrs_pdsch, &dl_sf, sf_symbols, h);

    float mse = 0.0f;
    for (uint32_t i = 0; i < dmrs_pdsch->nof_symbols * dmrs_pdsch->nof_sc * SRSLTE_NRE; i++) {
      cf_t err = h[i] - 1.0f;
      mse += cabsf(err);
    }
    mse /= (float)dmrs_pdsch->nof_symbols * dmrs_pdsch->nof_sc;

    TESTASSERT(!isnan(mse));
    TESTASSERT(mse < 1e-6f);
  }

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  int ret = SRSLTE_ERROR;

  parse_args(argc, argv);

  srslte_dmrs_pdsch_cfg_t cfg = {};
  cfg.duration                = SRSLTE_NOF_SLOTS_PER_SF * SRSLTE_MAX_NSYMB;
  cfg.nof_prb                 = cell.nof_prb;

  uint32_t nof_re     = cell.nof_prb * SRSLTE_NRE * SRSLTE_NOF_SLOTS_PER_SF * SRSLTE_MAX_NSYMB;
  cf_t*    sf_symbols = srslte_vec_cf_malloc(nof_re);
  cf_t*    h          = srslte_vec_cf_malloc(nof_re);

  uint32_t test_counter = 0;
  uint32_t test_passed  = 0;

  for (cfg.type = srslte_dmrs_pdsch_type_1; cfg.type <= srslte_dmrs_pdsch_type_2; cfg.type++) {
    srslte_dmrs_pdsch_typeA_pos_t typeA_pos_begin = srslte_dmrs_pdsch_typeA_pos_2;
    srslte_dmrs_pdsch_typeA_pos_t typeA_pos_end   = srslte_dmrs_pdsch_typeA_pos_3;

    for (cfg.typeA_pos = typeA_pos_begin; cfg.typeA_pos <= typeA_pos_end; cfg.typeA_pos++) {
      srslte_dmrs_pdsch_add_pos_t add_pos_begin = srslte_dmrs_pdsch_add_pos_2;
      srslte_dmrs_pdsch_add_pos_t add_pos_end   = srslte_dmrs_pdsch_add_pos_3;

      if (cfg.typeA_pos == srslte_dmrs_pdsch_typeA_pos_3) {
        add_pos_end = srslte_dmrs_pdsch_add_pos_1;
      }

      for (cfg.additional_pos = add_pos_begin; cfg.additional_pos <= add_pos_end; cfg.additional_pos++) {

        srslte_dmrs_pdsch_len_t max_len_begin = srslte_dmrs_pdsch_len_1;
        srslte_dmrs_pdsch_len_t max_len_end   = srslte_dmrs_pdsch_len_2;

        for (cfg.length = max_len_begin; cfg.length <= max_len_end; cfg.length++) {
          srslte_dmrs_pdsch_t dmrs_pdsch = {};

          // Initialise object with current configuration
          if (srslte_dmrs_pdsch_init(&dmrs_pdsch, &cfg)) {
            ERROR("Error initialising PDSCH DMRS\n");
            continue;
          }
          int n = run_test(&dmrs_pdsch, sf_symbols, h);

          if (n == SRSLTE_SUCCESS) {
            test_passed++;
          } else {
            char str[64] = {};
            srslte_dmrs_pdsch_cfg_to_str(&cfg, str, 64);

            ERROR("Test %d failed. %s.\n", test_counter, str);
          }

          test_counter++;
        }
      }
    }
  }

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
