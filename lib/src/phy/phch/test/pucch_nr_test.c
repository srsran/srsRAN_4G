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

#include "srsran/common/test_common.h"
#include "srsran/phy/ch_estimation/dmrs_pucch.h"
#include "srsran/phy/channel/ch_awgn.h"
#include "srsran/phy/phch/pucch_nr.h"
#include "srsran/phy/phch/ra_ul_nr.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/random.h"
#include "srsran/phy/utils/vector.h"
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

static srsran_carrier_nr_t carrier = SRSRAN_DEFAULT_CARRIER_NR;

static uint32_t              starting_prb_stride    = 4;
static uint32_t              starting_symbol_stride = 4;
static srsran_random_t       random_gen             = NULL;
static int                   format                 = -1;
static float                 snr_db                 = 20.0f;
static srsran_channel_awgn_t awgn                   = {};

static int test_pucch_format0(srsran_pucch_nr_t* pucch, const srsran_pucch_nr_common_cfg_t* cfg, cf_t* slot_symbols)
{
  srsran_slot_cfg_t          slot     = {};
  srsran_pucch_nr_resource_t resource = {};
  resource.format                     = SRSRAN_PUCCH_NR_FORMAT_0;

  for (slot.idx = 0; slot.idx < SRSRAN_NSLOTS_PER_FRAME_NR(carrier.scs); slot.idx++) {
    for (resource.starting_prb = 0; resource.starting_prb < carrier.nof_prb;
         resource.starting_prb += starting_prb_stride) {
      for (resource.nof_symbols = 1; resource.nof_symbols <= 2; resource.nof_symbols++) {
        for (resource.start_symbol_idx = 0;
             resource.start_symbol_idx <= SRSRAN_NSYMB_PER_SLOT_NR - resource.nof_symbols;
             resource.start_symbol_idx += starting_symbol_stride) {
          for (resource.initial_cyclic_shift = 0; resource.initial_cyclic_shift <= 11;
               resource.initial_cyclic_shift++) {
            for (uint32_t m_cs = 0; m_cs <= 6; m_cs += 2) {
              TESTASSERT(srsran_pucch_nr_format0_encode(pucch, cfg, &slot, &resource, m_cs, slot_symbols) ==
                         SRSRAN_SUCCESS);

              // Measure PUCCH format 0 for all possible values of m_cs
              for (uint32_t m_cs_test = 0; m_cs_test <= 6; m_cs_test += 2) {
                srsran_pucch_nr_measure_t measure = {};
                TESTASSERT(srsran_pucch_nr_format0_measure(
                               pucch, cfg, &slot, &resource, m_cs_test, slot_symbols, &measure) == SRSRAN_SUCCESS);

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

  return SRSRAN_SUCCESS;
}

static int test_pucch_format1(srsran_pucch_nr_t*                  pucch,
                              const srsran_pucch_nr_common_cfg_t* cfg,
                              srsran_chest_ul_res_t*              chest_res,
                              cf_t*                               slot_symbols,
                              bool                                enable_intra_slot_hopping)
{
  srsran_slot_cfg_t          slot     = {};
  srsran_pucch_nr_resource_t resource = {};
  resource.format                     = SRSRAN_PUCCH_NR_FORMAT_1;
  resource.intra_slot_hopping         = enable_intra_slot_hopping;

  for (slot.idx = 0; slot.idx < SRSRAN_NSLOTS_PER_FRAME_NR(carrier.scs); slot.idx++) {
    for (resource.starting_prb = 0; resource.starting_prb < carrier.nof_prb;
         resource.starting_prb += starting_prb_stride) {
      for (resource.second_hop_prb = 0; resource.second_hop_prb < (enable_intra_slot_hopping) ? carrier.nof_prb : 0;
           resource.second_hop_prb += starting_prb_stride) {
        for (resource.nof_symbols = SRSRAN_PUCCH_NR_FORMAT1_MIN_NSYMB;
             resource.nof_symbols <= SRSRAN_PUCCH_NR_FORMAT1_MAX_NSYMB;
             resource.nof_symbols++) {
          for (resource.start_symbol_idx = 0;
               resource.start_symbol_idx <=
               SRSRAN_MIN(SRSRAN_PUCCH_NR_FORMAT1_MAX_STARTSYMB, SRSRAN_NSYMB_PER_SLOT_NR - resource.nof_symbols);
               resource.start_symbol_idx += starting_symbol_stride) {
            for (resource.time_domain_occ = 0; resource.time_domain_occ <= SRSRAN_PUCCH_NR_FORMAT1_MAX_TOCC;
                 resource.time_domain_occ++) {
              for (resource.initial_cyclic_shift = 0; resource.initial_cyclic_shift <= SRSRAN_PUCCH_NR_FORMAT1_MAX_CS;
                   resource.initial_cyclic_shift++) {
                for (uint32_t nof_bits = 1; nof_bits <= SRSRAN_PUCCH_NR_FORMAT1_MAX_NOF_BITS; nof_bits++) {
                  for (uint32_t word = 0; word < (1U << nof_bits); word++) {
                    // Generate bits
                    uint8_t b[SRSRAN_PUCCH_NR_FORMAT1_MAX_NOF_BITS] = {};
                    for (uint32_t i = 0; i < nof_bits; i++) {
                      b[i] = (word >> i) & 1U;
                    }

                    // Encode PUCCH
                    TESTASSERT(srsran_pucch_nr_format1_encode(
                                   pucch, cfg, &slot, &resource, b, nof_bits, slot_symbols) == SRSRAN_SUCCESS);

                    // Put DMRS
                    TESTASSERT(srsran_dmrs_pucch_format1_put(pucch, &carrier, cfg, &slot, &resource, slot_symbols) ==
                               SRSRAN_SUCCESS);

                    // Apply AWGN
                    srsran_channel_awgn_run_c(
                        &awgn, slot_symbols, slot_symbols, carrier.nof_prb * SRSRAN_NRE * SRSRAN_NSYMB_PER_SLOT_NR);

                    // Estimate channel
                    TESTASSERT(srsran_dmrs_pucch_format1_estimate(
                                   pucch, cfg, &slot, &resource, slot_symbols, chest_res) == SRSRAN_SUCCESS);

                    TESTASSERT(fabsf(chest_res->rsrp_dBfs - 0.0f) < 3.0f);
                    TESTASSERT(fabsf(chest_res->epre_dBfs - 0.0f) < 3.0f);
                    TESTASSERT(fabsf(chest_res->snr_db - snr_db) < 10.0f);

                    // Decode PUCCH
                    uint8_t b_rx[SRSRAN_PUCCH_NR_FORMAT1_MAX_NOF_BITS];
                    TESTASSERT(srsran_pucch_nr_format1_decode(
                                   pucch, cfg, &slot, &resource, chest_res, slot_symbols, b_rx, nof_bits, NULL) ==
                               SRSRAN_SUCCESS);

                    // Check received bits
                    for (uint32_t i = 0; i < nof_bits; i++) {
                      if (b[i] != b_rx[i]) {
                        printf("aaa");
                      }
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
  }

  return SRSRAN_SUCCESS;
}

static int test_pucch_format2(srsran_pucch_nr_t*                  pucch,
                              const srsran_pucch_nr_common_cfg_t* cfg,
                              srsran_chest_ul_res_t*              chest_res,
                              cf_t*                               slot_symbols)
{
  srsran_slot_cfg_t          slot     = {};
  srsran_pucch_nr_resource_t resource = {};
  resource.format                     = SRSRAN_PUCCH_NR_FORMAT_2;

  for (slot.idx = 0; slot.idx < SRSRAN_NSLOTS_PER_FRAME_NR(carrier.scs); slot.idx++) {
    for (resource.nof_symbols = SRSRAN_PUCCH_NR_FORMAT2_MIN_NSYMB;
         resource.nof_symbols <= SRSRAN_PUCCH_NR_FORMAT2_MAX_NSYMB;
         resource.nof_symbols++) {
      for (resource.start_symbol_idx = 0;
           resource.start_symbol_idx <=
           SRSRAN_MIN(SRSRAN_PUCCH_NR_FORMAT2_MAX_STARTSYMB, SRSRAN_NSYMB_PER_SLOT_NR - resource.nof_symbols);
           resource.start_symbol_idx += starting_symbol_stride) {
        srsran_uci_cfg_nr_t uci_cfg = {};

        for (uci_cfg.ack.count = SRSRAN_PUCCH_NR_FORMAT2_MIN_NOF_BITS;
             uci_cfg.ack.count <= SRSRAN_HARQ_ACK_MAX_NOF_BITS;
             uci_cfg.ack.count++) {
          srsran_uci_value_nr_t uci_value = {};

          // Maximum code rate is reserved
          uint32_t max_code_rate_end = SRSRAN_PUCCH_NR_MAX_CODE_RATE;
          if (uci_cfg.ack.count == 11) {
            max_code_rate_end = SRSRAN_PUCCH_NR_MAX_CODE_RATE - 1;
          }

          for (resource.max_code_rate = 0; resource.max_code_rate < max_code_rate_end; resource.max_code_rate++) {
            // Skip case if not enough PRB are used
            int min_nof_prb = srsran_ra_ul_nr_pucch_format_2_3_min_prb(&resource, &uci_cfg);
            TESTASSERT(min_nof_prb > SRSRAN_SUCCESS);

            for (resource.nof_prb = min_nof_prb;
                 resource.nof_prb < SRSRAN_MIN(carrier.nof_prb, SRSRAN_PUCCH_NR_FORMAT2_MAX_NPRB);
                 resource.nof_prb++) {
              for (resource.starting_prb = 0; resource.starting_prb < (carrier.nof_prb - resource.nof_prb);
                   resource.starting_prb += starting_prb_stride) {
                // Generate ACKs
                for (uint32_t i = 0; i < uci_cfg.ack.count; i++) {
                  uci_value.ack[i] = (uint8_t)srsran_random_uniform_int_dist(random_gen, 0, 1);
                }

                // Encode PUCCH
                TESTASSERT(srsran_pucch_nr_format_2_3_4_encode(
                               pucch, cfg, &slot, &resource, &uci_cfg, &uci_value, slot_symbols) == SRSRAN_SUCCESS);

                // Put DMRS
                TESTASSERT(srsran_dmrs_pucch_format2_put(pucch, &carrier, cfg, &slot, &resource, slot_symbols) ==
                           SRSRAN_SUCCESS);

                // Apply AWGN
                srsran_channel_awgn_run_c(
                    &awgn, slot_symbols, slot_symbols, carrier.nof_prb * SRSRAN_NRE * SRSRAN_NSYMB_PER_SLOT_NR);

                // Estimate channel
                TESTASSERT(srsran_dmrs_pucch_format2_estimate(pucch, cfg, &slot, &resource, slot_symbols, chest_res) ==
                           SRSRAN_SUCCESS);
                INFO("RSRP=%+.2f; EPRE=%+.2f; SNR=%+.2f;",
                     chest_res->rsrp_dBfs,
                     chest_res->epre_dBfs,
                     chest_res->snr_db);
                TESTASSERT(fabsf(chest_res->rsrp_dBfs - 0.0f) < 3.0f);
                TESTASSERT(fabsf(chest_res->epre_dBfs - 0.0f) < 3.0f);
                TESTASSERT(fabsf(chest_res->snr_db - snr_db) < 20.0f);

                // Decode PUCCH
                srsran_uci_value_nr_t uci_value_rx = {};
                TESTASSERT(srsran_pucch_nr_format_2_3_4_decode(
                               pucch, cfg, &slot, &resource, &uci_cfg, chest_res, slot_symbols, &uci_value_rx) ==
                           SRSRAN_SUCCESS);

                TESTASSERT(uci_value_rx.valid == true);

                // Check received ACKs
                for (uint32_t i = 0; i < uci_cfg.ack.count; i++) {
                  TESTASSERT(uci_value.ack[i] == uci_value_rx.ack[i]);
                }
              }
            }
          }
        }
      }
    }
  }
  return SRSRAN_SUCCESS;
}

static void usage(char* prog)
{
  printf("Usage: %s [csNnv]\n", prog);
  printf("\t-c cell id [Default %d]\n", carrier.pci);
  printf("\t-n nof_prb [Default %d]\n", carrier.nof_prb);
  printf("\t-f format [Default %d]\n", format);
  printf("\t-s SNR in dB [Default %.2f]\n", snr_db);
  printf("\t-v [set verbose to debug, default none]\n");
}

static void parse_args(int argc, char** argv)
{
  // Limit default number of RB
  carrier.nof_prb = 6;

  int opt;
  while ((opt = getopt(argc, argv, "cnfsv")) != -1) {
    switch (opt) {
      case 'c':
        carrier.pci = (uint32_t)strtol(argv[optind], NULL, 10);
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
        increase_srsran_verbose_level();
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  int ret = SRSRAN_ERROR;
  parse_args(argc, argv);

  uint32_t              nof_re    = carrier.nof_prb * SRSRAN_NRE * SRSRAN_NSYMB_PER_SLOT_NR;
  cf_t*                 slot_symb = srsran_vec_cf_malloc(nof_re);
  srsran_pucch_nr_t     pucch     = {};
  srsran_chest_ul_res_t chest_res = {};

  random_gen = srsran_random_init(0x1234);
  if (random_gen == NULL) {
    ERROR("Random init");
    goto clean_exit;
  }

  if (slot_symb == NULL) {
    ERROR("Alloc");
    goto clean_exit;
  }

  srsran_pucch_nr_args_t pucch_args = {};
  if (srsran_pucch_nr_init(&pucch, &pucch_args) < SRSRAN_SUCCESS) {
    ERROR("PUCCH init");
    goto clean_exit;
  }

  if (srsran_pucch_nr_set_carrier(&pucch, &carrier) < SRSRAN_SUCCESS) {
    ERROR("PUCCH set carrier");
    goto clean_exit;
  }

  if (srsran_chest_ul_res_init(&chest_res, carrier.nof_prb)) {
    ERROR("Chest UL");
    goto clean_exit;
  }

  if (srsran_channel_awgn_init(&awgn, 1234) < SRSRAN_SUCCESS) {
    ERROR("AWGN init");
    goto clean_exit;
  }

  if (srsran_channel_awgn_set_n0(&awgn, -snr_db) < SRSRAN_SUCCESS) {
    ERROR("AWGN set N0");
    goto clean_exit;
  }

  srsran_pucch_nr_common_cfg_t common_cfg = {};

  // Test Format 0
  if (format < 0 || format == 0) {
    if (test_pucch_format0(&pucch, &common_cfg, slot_symb) < SRSRAN_SUCCESS) {
      ERROR("Failed PUCCH format 0");
      goto clean_exit;
    }
  }

  // Test Format 1 with and without intra slot frequency hopping
  if (format < 0 || format == 1) {
    if (test_pucch_format1(&pucch, &common_cfg, &chest_res, slot_symb, false) < SRSRAN_SUCCESS) {
      ERROR("Failed PUCCH format 1");
      goto clean_exit;
    }
    if (test_pucch_format1(&pucch, &common_cfg, &chest_res, slot_symb, true) < SRSRAN_SUCCESS) {
      ERROR("Failed PUCCH format 1");
      goto clean_exit;
    }
  }

  // Test Format 2
  if (format < 0 || format == 2) {
    if (test_pucch_format2(&pucch, &common_cfg, &chest_res, slot_symb) < SRSRAN_SUCCESS) {
      ERROR("Failed PUCCH format 2");
      goto clean_exit;
    }
  }

  ret = SRSRAN_SUCCESS;
clean_exit:
  if (slot_symb) {
    free(slot_symb);
  }

  srsran_pucch_nr_free(&pucch);
  srsran_chest_ul_res_free(&chest_res);
  srsran_channel_awgn_free(&awgn);
  srsran_random_free(random_gen);

  if (ret == SRSRAN_SUCCESS) {
    printf("Test passed!\n");
  } else {
    printf("Test failed!\n");
  }

  return ret;
}