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
#include "srslte/phy/ch_estimation/dmrs_pucch.h"
#include "srslte/phy/channel/ch_awgn.h"
#include "srslte/phy/phch/pucch_nr.h"
#include "srslte/phy/phch/ra_ul_nr.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/random.h"
#include "srslte/phy/utils/vector.h"
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

static srslte_carrier_nr_t carrier = {
    0, // cell_id
    0, // numerology
    6, // nof_prb
    0, // start
    1  // max_mimo_layers
};

static uint32_t              starting_prb_stride    = 4;
static uint32_t              starting_symbol_stride = 4;
static srslte_random_t       random_gen             = NULL;
static int                   format                 = -1;
static float                 snr_db                 = 20.0f;
static srslte_channel_awgn_t awgn                   = {};

static int test_pucch_format0(srslte_pucch_nr_t* pucch, const srslte_pucch_nr_common_cfg_t* cfg, cf_t* slot_symbols)
{
  srslte_slot_cfg_t          slot     = {};
  srslte_pucch_nr_resource_t resource = {};
  resource.format                     = SRSLTE_PUCCH_NR_FORMAT_0;

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
              TESTASSERT(srslte_pucch_nr_format0_encode(pucch, cfg, &slot, &resource, m_cs, slot_symbols) ==
                         SRSLTE_SUCCESS);

              // Measure PUCCH format 0 for all possible values of m_cs
              for (uint32_t m_cs_test = 0; m_cs_test <= 6; m_cs_test += 2) {
                srslte_pucch_nr_measure_t measure = {};
                TESTASSERT(srslte_pucch_nr_format0_measure(
                               pucch, cfg, &slot, &resource, m_cs_test, slot_symbols, &measure) == SRSLTE_SUCCESS);

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

static int test_pucch_format1(srslte_pucch_nr_t*                  pucch,
                              const srslte_pucch_nr_common_cfg_t* cfg,
                              srslte_chest_ul_res_t*              chest_res,
                              cf_t*                               slot_symbols)
{
  srslte_slot_cfg_t          slot     = {};
  srslte_pucch_nr_resource_t resource = {};
  resource.format                     = SRSLTE_PUCCH_NR_FORMAT_1;

  for (slot.idx = 0; slot.idx < SRSLTE_NSLOTS_PER_FRAME_NR(carrier.numerology); slot.idx++) {
    for (resource.starting_prb = 0; resource.starting_prb < carrier.nof_prb;
         resource.starting_prb += starting_prb_stride) {
      for (resource.nof_symbols = SRSLTE_PUCCH_NR_FORMAT1_MIN_NSYMB;
           resource.nof_symbols <= SRSLTE_PUCCH_NR_FORMAT1_MAX_NSYMB;
           resource.nof_symbols++) {
        for (resource.start_symbol_idx = 0;
             resource.start_symbol_idx <=
             SRSLTE_MIN(SRSLTE_PUCCH_NR_FORMAT1_MAX_STARTSYMB, SRSLTE_NSYMB_PER_SLOT_NR - resource.nof_symbols);
             resource.start_symbol_idx += starting_symbol_stride) {
          for (resource.time_domain_occ = 0; resource.time_domain_occ <= SRSLTE_PUCCH_NR_FORMAT1_MAX_TOCC;
               resource.time_domain_occ++) {
            for (resource.initial_cyclic_shift = 0; resource.initial_cyclic_shift <= SRSLTE_PUCCH_NR_FORMAT1_MAX_CS;
                 resource.initial_cyclic_shift++) {
              for (uint32_t nof_bits = 1; nof_bits <= SRSLTE_PUCCH_NR_FORMAT1_MAX_NOF_BITS; nof_bits++) {
                for (uint32_t word = 0; word < (1U << nof_bits); word++) {
                  // Generate bits
                  uint8_t b[SRSLTE_PUCCH_NR_FORMAT1_MAX_NOF_BITS] = {};
                  for (uint32_t i = 0; i < nof_bits; i++) {
                    b[i] = (word >> i) & 1U;
                  }

                  // Encode PUCCH
                  TESTASSERT(srslte_pucch_nr_format1_encode(pucch, cfg, &slot, &resource, b, nof_bits, slot_symbols) ==
                             SRSLTE_SUCCESS);

                  // Put DMRS
                  TESTASSERT(srslte_dmrs_pucch_format1_put(pucch, &carrier, cfg, &slot, &resource, slot_symbols) ==
                             SRSLTE_SUCCESS);

                  // Apply AWGN
                  srslte_channel_awgn_run_c(
                      &awgn, slot_symbols, slot_symbols, carrier.nof_prb * SRSLTE_NRE * SRSLTE_NSYMB_PER_SLOT_NR);

                  // Estimate channel
                  TESTASSERT(srslte_dmrs_pucch_format1_estimate(
                                 pucch, &carrier, cfg, &slot, &resource, slot_symbols, chest_res) == SRSLTE_SUCCESS);

                  TESTASSERT(fabsf(chest_res->rsrp_dBfs - 0.0f) < 3.0f);
                  TESTASSERT(fabsf(chest_res->epre_dBfs - 0.0f) < 3.0f);
                  TESTASSERT(fabsf(chest_res->snr_db - snr_db) < 10.0f);

                  // Decode PUCCH
                  uint8_t b_rx[SRSLTE_PUCCH_NR_FORMAT1_MAX_NOF_BITS];
                  TESTASSERT(srslte_pucch_nr_format1_decode(
                                 pucch, cfg, &slot, &resource, chest_res, slot_symbols, b_rx, nof_bits) ==
                             SRSLTE_SUCCESS);

                  // Check received bits
                  for (uint32_t i = 0; i < nof_bits; i++) {
                    TESTASSERT(b[i] == b_rx[i]);
                  }
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

static int test_pucch_format2(srslte_pucch_nr_t*                  pucch,
                              const srslte_pucch_nr_common_cfg_t* cfg,
                              srslte_chest_ul_res_t*              chest_res,
                              cf_t*                               slot_symbols)
{
  srslte_slot_cfg_t          slot     = {};
  srslte_pucch_nr_resource_t resource = {};
  resource.format                     = SRSLTE_PUCCH_NR_FORMAT_2;

  for (slot.idx = 0; slot.idx < SRSLTE_NSLOTS_PER_FRAME_NR(carrier.numerology); slot.idx++) {
    for (resource.nof_symbols = SRSLTE_PUCCH_NR_FORMAT2_MIN_NSYMB;
         resource.nof_symbols <= SRSLTE_PUCCH_NR_FORMAT2_MAX_NSYMB;
         resource.nof_symbols++) {
      for (resource.start_symbol_idx = 0;
           resource.start_symbol_idx <=
           SRSLTE_MIN(SRSLTE_PUCCH_NR_FORMAT2_MAX_STARTSYMB, SRSLTE_NSYMB_PER_SLOT_NR - resource.nof_symbols);
           resource.start_symbol_idx += starting_symbol_stride) {
        srslte_uci_cfg_nr_t uci_cfg = {};

        for (uci_cfg.o_ack = SRSLTE_PUCCH_NR_FORMAT2_MIN_NOF_BITS; uci_cfg.o_ack <= SRSLTE_UCI_NR_MAX_ACK_BITS;
             uci_cfg.o_ack++) {
          srslte_uci_value_nr_t uci_value = {};

          // Maximum code rate is reserved
          uint32_t max_code_rate_end = SRSLTE_PUCCH_NR_MAX_CODE_RATE;
          if (uci_cfg.o_ack == 11) {
            max_code_rate_end = SRSLTE_PUCCH_NR_MAX_CODE_RATE - 1;
          }

          for (resource.max_code_rate = 0; resource.max_code_rate < max_code_rate_end; resource.max_code_rate++) {
            // Skip case if not enough PRB are used
            int min_nof_prb = srslte_ra_ul_nr_pucch_format_2_3_min_prb(&resource, &uci_cfg);
            TESTASSERT(min_nof_prb > SRSLTE_SUCCESS);

            for (resource.nof_prb = min_nof_prb;
                 resource.nof_prb < SRSLTE_MIN(carrier.nof_prb, SRSLTE_PUCCH_NR_FORMAT2_MAX_NPRB);
                 resource.nof_prb++) {
              for (resource.starting_prb = 0; resource.starting_prb < (carrier.nof_prb - resource.nof_prb);
                   resource.starting_prb += starting_prb_stride) {
                // Generate ACKs
                for (uint32_t i = 0; i < uci_cfg.o_ack; i++) {
                  uci_value.ack[i] = (uint8_t)srslte_random_uniform_int_dist(random_gen, 0, 1);
                }

                // Encode PUCCH
                TESTASSERT(srslte_pucch_nr_format_2_3_4_encode(
                               pucch, cfg, &slot, &resource, &uci_cfg, &uci_value, slot_symbols) == SRSLTE_SUCCESS);

                // Put DMRS
                TESTASSERT(srslte_dmrs_pucch_format2_put(pucch, &carrier, cfg, &slot, &resource, slot_symbols) ==
                           SRSLTE_SUCCESS);

                // Apply AWGN
                srslte_channel_awgn_run_c(
                    &awgn, slot_symbols, slot_symbols, carrier.nof_prb * SRSLTE_NRE * SRSLTE_NSYMB_PER_SLOT_NR);

                // Estimate channel
                TESTASSERT(srslte_dmrs_pucch_format2_estimate(
                               pucch, &carrier, cfg, &slot, &resource, slot_symbols, chest_res) == SRSLTE_SUCCESS);
                INFO("RSRP=%+.2f; EPRE=%+.2f; SNR=%+.2f;",
                     chest_res->rsrp_dBfs,
                     chest_res->epre_dBfs,
                     chest_res->snr_db);
                TESTASSERT(fabsf(chest_res->rsrp_dBfs - 0.0f) < 3.0f);
                TESTASSERT(fabsf(chest_res->epre_dBfs - 0.0f) < 3.0f);
                TESTASSERT(fabsf(chest_res->snr_db - snr_db) < 20.0f);

                // Decode PUCCH
                srslte_uci_value_nr_t uci_value_rx = {};
                TESTASSERT(srslte_pucch_nr_format_2_3_4_decode(
                               pucch, cfg, &slot, &resource, &uci_cfg, chest_res, slot_symbols, &uci_value_rx) ==
                           SRSLTE_SUCCESS);

                TESTASSERT(uci_value_rx.valid == true);

                // Check received ACKs
                for (uint32_t i = 0; i < uci_cfg.o_ack; i++) {
                  TESTASSERT(uci_value.ack[i] == uci_value_rx.ack[i]);
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
  printf("\t-f format [Default %d]\n", format);
  printf("\t-s SNR in dB [Default %.2f]\n", snr_db);
  printf("\t-v [set verbose to debug, default none]\n");
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cnfsv")) != -1) {
    switch (opt) {
      case 'c':
        carrier.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        carrier.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        format = (int)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        snr_db = strtof(argv[optind], NULL);
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

  uint32_t              nof_re    = carrier.nof_prb * SRSLTE_NRE * SRSLTE_NSYMB_PER_SLOT_NR;
  cf_t*                 slot_symb = srslte_vec_cf_malloc(nof_re);
  srslte_pucch_nr_t     pucch     = {};
  srslte_chest_ul_res_t chest_res = {};

  random_gen = srslte_random_init(0x1234);
  if (random_gen == NULL) {
    ERROR("Random init");
    goto clean_exit;
  }

  if (slot_symb == NULL) {
    ERROR("Alloc");
    goto clean_exit;
  }

  srslte_pucch_nr_args_t pucch_args = {};
  if (srslte_pucch_nr_init(&pucch, &pucch_args) < SRSLTE_SUCCESS) {
    ERROR("PUCCH init");
    goto clean_exit;
  }

  if (srslte_pucch_nr_set_carrier(&pucch, &carrier) < SRSLTE_SUCCESS) {
    ERROR("PUCCH set carrier");
    goto clean_exit;
  }

  if (srslte_chest_ul_res_init(&chest_res, carrier.nof_prb)) {
    ERROR("Chest UL");
    goto clean_exit;
  }

  if (srslte_channel_awgn_init(&awgn, 1234) < SRSLTE_SUCCESS) {
    ERROR("AWGN init");
    goto clean_exit;
  }

  if (srslte_channel_awgn_set_n0(&awgn, -snr_db) < SRSLTE_SUCCESS) {
    ERROR("AWGN set N0");
    goto clean_exit;
  }

  srslte_pucch_nr_common_cfg_t common_cfg = {};

  // Test Format 0
  if (format < 0 || format == 0) {
    if (test_pucch_format0(&pucch, &common_cfg, slot_symb) < SRSLTE_SUCCESS) {
      ERROR("Failed PUCCH format 0");
      goto clean_exit;
    }
  }

  // Test Format 1
  if (format < 0 || format == 1) {
    if (test_pucch_format1(&pucch, &common_cfg, &chest_res, slot_symb) < SRSLTE_SUCCESS) {
      ERROR("Failed PUCCH format 1");
      goto clean_exit;
    }
  }

  // Test Format 2
  if (format < 0 || format == 2) {
    if (test_pucch_format2(&pucch, &common_cfg, &chest_res, slot_symb) < SRSLTE_SUCCESS) {
      ERROR("Failed PUCCH format 2");
      goto clean_exit;
    }
  }

  ret = SRSLTE_SUCCESS;
clean_exit:
  if (slot_symb) {
    free(slot_symb);
  }

  srslte_pucch_nr_free(&pucch);
  srslte_chest_ul_res_free(&chest_res);
  srslte_channel_awgn_free(&awgn);
  srslte_random_free(random_gen);

  if (ret == SRSLTE_SUCCESS) {
    printf("Test passed!\n");
  } else {
    printf("Test failed!\n");
  }

  return ret;
}