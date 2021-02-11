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
#include "srslte/phy/phch/pdcch_nr.h"
#include "srslte/phy/utils/random.h"
#include <getopt.h>

static srslte_carrier_nr_t carrier = {
    0,  // cell_id
    0,  // numerology
    50, // nof_prb
    0,  // start
    1   // max_mimo_layers
};

static uint16_t rnti       = 0x1234;
static bool     fast_sweep = true;

typedef struct {
  uint64_t time_us;
  uint64_t count;
} proc_time_t;

static proc_time_t enc_time[SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR] = {};
static proc_time_t dec_time[SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR] = {};

static int test(srslte_pdcch_nr_t*      tx,
                srslte_pdcch_nr_t*      rx,
                cf_t*                   grid,
                srslte_dmrs_pdcch_ce_t* ce,
                srslte_dci_msg_nr_t*    dci_msg_tx)
{
  // Encode PDCCH
  TESTASSERT(srslte_pdcch_nr_encode(tx, dci_msg_tx, grid) == SRSLTE_SUCCESS);

  enc_time[dci_msg_tx->location.L].time_us += tx->meas_time_us;
  enc_time[dci_msg_tx->location.L].count++;

  // Init Rx MSG
  srslte_pdcch_nr_res_t res        = {};
  srslte_dci_msg_nr_t   dci_msg_rx = *dci_msg_tx;
  srslte_vec_u8_zero(dci_msg_rx.payload, dci_msg_rx.nof_bits);

  // Decode PDCCH
  TESTASSERT(srslte_pdcch_nr_decode(rx, grid, ce, &dci_msg_rx, &res) == SRSLTE_SUCCESS);

  dec_time[dci_msg_tx->location.L].time_us += rx->meas_time_us;
  dec_time[dci_msg_tx->location.L].count++;

  // Assert
  TESTASSERT(res.evm < 0.01f);
  TESTASSERT(res.crc);

  return SRSLTE_SUCCESS;
}

static void usage(char* prog)
{
  printf("Usage: %s [pFv] \n", prog);
  printf("\t-p Number of carrier PRB [Default %d]\n", carrier.nof_prb);
  printf("\t-F Fast CORESET frequency resource sweeping [Default %s]\n", fast_sweep ? "Enabled" : "Disabled");
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

static int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "pFv")) != -1) {
    switch (opt) {
      case 'p':
        carrier.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'F':
        fast_sweep ^= true;
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        return SRSLTE_ERROR;
    }
  }

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  int ret = SRSLTE_ERROR;

  srslte_pdcch_nr_args_t args = {};
  args.disable_simd           = false;
  args.measure_evm            = true;
  args.measure_time           = true;

  srslte_pdcch_nr_t pdcch_tx = {};
  srslte_pdcch_nr_t pdcch_rx = {};

  if (parse_args(argc, argv) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  uint32_t                grid_sz  = carrier.nof_prb * SRSLTE_NRE * SRSLTE_NSYMB_PER_SLOT_NR;
  srslte_random_t         rand_gen = srslte_random_init(1234);
  srslte_dmrs_pdcch_ce_t* ce       = SRSLTE_MEM_ALLOC(srslte_dmrs_pdcch_ce_t, 1);
  cf_t*                   buffer   = srslte_vec_cf_malloc(grid_sz);
  if (rand_gen == NULL || ce == NULL || buffer == NULL) {
    ERROR("Error malloc");
    goto clean_exit;
  }

  SRSLTE_MEM_ZERO(ce, srslte_dmrs_pdcch_ce_t, 1);

  if (srslte_pdcch_nr_init_tx(&pdcch_tx, &args) < SRSLTE_SUCCESS) {
    ERROR("Error init");
    goto clean_exit;
  }

  if (srslte_pdcch_nr_init_rx(&pdcch_rx, &args) < SRSLTE_SUCCESS) {
    ERROR("Error init");
    goto clean_exit;
  }

  srslte_coreset_t coreset                = {};
  uint32_t         nof_frequency_resource = SRSLTE_MIN(SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE, carrier.nof_prb / 6);
  for (uint32_t frequency_resources = 1; frequency_resources < (1U << nof_frequency_resource);
       frequency_resources          = (fast_sweep) ? ((frequency_resources << 1U) | 1U) : (frequency_resources + 1)) {
    for (uint32_t i = 0; i < nof_frequency_resource; i++) {
      uint32_t mask             = ((frequency_resources >> i) & 1U);
      coreset.freq_resources[i] = (mask == 1);
    }
    for (coreset.duration = SRSLTE_CORESET_DURATION_MIN; coreset.duration <= SRSLTE_CORESET_DURATION_MAX;
         coreset.duration++) {
      srslte_search_space_t search_space = {};
      search_space.type                  = srslte_search_space_type_ue;

      if (srslte_pdcch_nr_set_carrier(&pdcch_tx, &carrier, &coreset) < SRSLTE_SUCCESS) {
        ERROR("Error setting carrier");
        goto clean_exit;
      }

      if (srslte_pdcch_nr_set_carrier(&pdcch_rx, &carrier, &coreset) < SRSLTE_SUCCESS) {
        ERROR("Error setting carrier");
        goto clean_exit;
      }

      // Fill search space maximum number of candidates
      for (uint32_t aggregation_level = 0; aggregation_level < SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR;
           aggregation_level++) {
        search_space.nof_candidates[aggregation_level] =
            srslte_pdcch_nr_max_candidates_coreset(&coreset, aggregation_level);
      }

      for (uint32_t aggregation_level = 0; aggregation_level < SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR;
           aggregation_level++) {
        uint32_t L = 1U << aggregation_level;

        for (uint32_t slot_idx = 0; slot_idx < SRSLTE_NSLOTS_PER_FRAME_NR(carrier.numerology); slot_idx++) {
          uint32_t dci_locations[SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR] = {};

          // Calculate candidate locations
          int n = srslte_pdcch_nr_locations_coreset(
              &coreset, &search_space, rnti, aggregation_level, slot_idx, dci_locations);
          if (n < SRSLTE_SUCCESS) {
            ERROR("Error calculating locations in CORESET");
            goto clean_exit;
          }

          // Skip if no candidates
          if (n == 0) {
            continue;
          }

          for (uint32_t ncce_idx = 0; ncce_idx < n; ncce_idx++) {
            // Init MSG
            srslte_dci_msg_nr_t dci_msg = {};
            dci_msg.format              = srslte_dci_format_nr_1_0;
            dci_msg.rnti_type           = srslte_rnti_type_c;
            dci_msg.location.L          = aggregation_level;
            dci_msg.location.ncce       = dci_locations[ncce_idx];
            dci_msg.nof_bits            = srslte_dci_nr_format_1_0_sizeof(&carrier, &coreset, dci_msg.rnti_type);

            // Generate random payload
            for (uint32_t i = 0; i < dci_msg.nof_bits; i++) {
              dci_msg.payload[i] = srslte_random_uniform_int_dist(rand_gen, 0, 1);
            }

            // Set channel estimate number of elements and set out-of-range values to zero
            ce->nof_re = (SRSLTE_NRE - 3) * 6 * L;
            for (uint32_t i = 0; i < SRSLTE_PDCCH_MAX_RE; i++) {
              ce->ce[i] = (i < ce->nof_re) ? 1.0f : 0.0f;
            }
            ce->noise_var = 0.0f;

            if (test(&pdcch_tx, &pdcch_rx, buffer, ce, &dci_msg) < SRSLTE_SUCCESS) {
              ERROR("test failed");
              goto clean_exit;
            }
          }
        }
      }
    }
  }

  printf("+--------+--------+--------+--------+\n");
  printf("| %6s | %6s | %6s | %6s |\n", " ", " ", " Time ", " Time ");
  printf("| %6s | %6s | %6s | %6s |\n", "  L  ", "Count", "Encode", "Decode");
  printf("| %6s | %6s | %6s | %6s |\n", " ", " ", " (us) ", " (us) ");
  printf("+--------+--------+--------+--------+\n");
  for (uint32_t i = 0; i < SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR; i++) {
    if (enc_time[i].count > 0 && dec_time[i].count) {
      printf("| %6" PRIu32 "| %6" PRIu64 " | %6.1f | %6.1f |\n",
             i,
             enc_time[i].count,
             (double)enc_time[i].time_us / (double)enc_time[i].count,
             (double)dec_time[i].time_us / (double)dec_time[i].count);
    }
  }
  printf("+--------+--------+--------+--------+\n");

  ret = SRSLTE_SUCCESS;
clean_exit:
  srslte_random_free(rand_gen);

  if (ce) {
    free(ce);
  }

  if (buffer) {
    free(buffer);
  }

  srslte_pdcch_nr_free(&pdcch_tx);
  srslte_pdcch_nr_free(&pdcch_rx);

  if (ret == SRSLTE_SUCCESS) {
    printf("Passed!\n");
  } else {
    printf("Failed!\n");
  }

  return ret;
}