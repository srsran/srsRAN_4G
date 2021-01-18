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

#include "srslte/common/test_common.h"
#include "srslte/phy/phch/pucch_nr.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <unistd.h>

static srslte_carrier_nr_t carrier = {
    0, // cell_id
    0, // numerology
    6, // nof_prb
    0, // start
    1  // max_mimo_layers
};

static uint32_t starting_prb_stride    = 4;
static uint32_t starting_symbol_stride = 4;

static int test_pucch_format0(srslte_pucch_nr_t* pucch, const srslte_pucch_nr_common_cfg_t* cfg, cf_t* slot_symbols)
{
  srslte_dl_slot_cfg_t               slot     = {};
  srslte_pucch_nr_resource_format0_t resource = {};

  for (slot.idx = 0; slot.idx < SRSLTE_NSLOTS_PER_FRAME_NR(carrier.numerology); slot.idx++) {
    for (resource.starting_prb = 0; resource.starting_prb < carrier.nof_prb;
         resource.starting_prb += starting_prb_stride) {
      for (resource.nof_symbols = 1; resource.nof_symbols <= 2; resource.nof_symbols++) {
        for (resource.start_symbol_idx = 0;
             resource.start_symbol_idx <= SRSLTE_NSYMB_PER_SLOT_NR - resource.nof_symbols;
             resource.start_symbol_idx += starting_symbol_stride) {
          for (resource.initial_cyclic_shift = 0; resource.initial_cyclic_shift <= 11;
               resource.initial_cyclic_shift++) {
            for (uint32_t m_cs = 0; m_cs <= 6; m_cs += 2) {
              TESTASSERT(srslte_pucch_nr_format0_put(pucch, &carrier, cfg, &slot, &resource, m_cs, slot_symbols) ==
                         SRSLTE_SUCCESS);

              // Measure PUCCH format 0 for all possible values of m_cs
              for (uint32_t m_cs_test = 0; m_cs_test <= 6; m_cs_test += 2) {
                srslte_pucch_nr_measure_t measure = {};
                TESTASSERT(srslte_pucch_nr_format0_measure(
                               pucch, &carrier, cfg, &slot, &resource, m_cs_test, slot_symbols, &measure) ==
                           SRSLTE_SUCCESS);

                if (m_cs == m_cs_test) {
                  TESTASSERT(fabsf(measure.epre - 1) < 0.001);
                  TESTASSERT(fabsf(measure.rsrp - 1) < 0.001);
                  TESTASSERT(fabsf(measure.norm_corr - 1) < 0.001);
                } else {
                  TESTASSERT(fabsf(measure.epre - 1) < 0.001);
                  TESTASSERT(fabsf(measure.rsrp - 0) < 0.1);
                  TESTASSERT(fabsf(measure.norm_corr - 0) < 0.1);
                }
              }
            }
          }
        }
      }
    }
  }

  return SRSLTE_SUCCESS;
}

static void usage(char* prog)
{
  printf("Usage: %s [csNnv]\n", prog);
  printf("\t-c cell id [Default %d]\n", carrier.id);
  printf("\t-n nof_prb [Default %d]\n", carrier.nof_prb);
  printf("\t-v [set verbose to debug, default none]\n");
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cnv")) != -1) {
    switch (opt) {
      case 'c':
        carrier.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        carrier.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
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

int main(int argc, char** argv)
{
  int ret = SRSLTE_ERROR;
  parse_args(argc, argv);

  uint32_t          nof_re    = carrier.nof_prb * SRSLTE_NRE * SRSLTE_NSYMB_PER_SLOT_NR;
  cf_t*             slot_symb = srslte_vec_cf_malloc(nof_re);
  srslte_pucch_nr_t pucch     = {};

  if (slot_symb == NULL) {
    ERROR("Alloc\n");
    goto clean_exit;
  }

  if (srslte_pucch_nr_init(&pucch) < SRSLTE_SUCCESS) {
    ERROR("PUCCH init\n");
    goto clean_exit;
  }

  srslte_pucch_nr_common_cfg_t common_cfg = {};
  if (test_pucch_format0(&pucch, &common_cfg, slot_symb) < SRSLTE_SUCCESS) {
    ERROR("Failed PUCCH format 0\n");
    goto clean_exit;
  }

  ret = SRSLTE_SUCCESS;
clean_exit:
  if (slot_symb) {
    free(slot_symb);
  }

  srslte_pucch_nr_free(&pucch);

  if (ret == SRSLTE_SUCCESS) {
    printf("Test passed!\n");
  } else {
    printf("Test failed!\n");
  }

  return ret;
}