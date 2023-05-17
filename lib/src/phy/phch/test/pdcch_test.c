/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "srsran/common/test_common.h"
#include "srsran/srsran.h"

// Test parameters
static uint32_t         pci         = 1;
static uint16_t         rnti        = 0x46;
static uint32_t         cfi         = 2;
static uint32_t         nof_ports   = 1;
static srsran_dci_cfg_t dci_cfg     = {};
static uint32_t         nof_prb     = 100;
static float            snr_dB      = NAN;
static uint32_t         repetitions = 1;
static bool             false_check = false;

// Test objects
static srsran_random_t       random_gen                     = NULL;
static srsran_pdcch_t        pdcch_tx                       = {};
static srsran_pdcch_t        pdcch_rx                       = {};
static srsran_chest_dl_res_t chest_dl_res                   = {};
static srsran_channel_awgn_t awgn                           = {};
static cf_t*                 slot_symbols[SRSRAN_MAX_PORTS] = {};

static void usage(char* prog)
{
  printf("Usage: %s [pfncxv]\n", prog);
  printf("\t-c cell id [Default %d]\n", pci);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-p cell.nof_ports [Default %d]\n", nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", nof_prb);
  printf("\t-x Enable/Disable Cross-scheduling [Default %s]\n", dci_cfg.cif_enabled ? "enabled" : "disabled");
  printf("\t-F False detection check [Default %s]\n", false_check ? "enabled" : "disabled");
  printf("\t-R Repetitions [Default %d]\n", repetitions);
  printf("\t-S SNR in dB [Default %+.1f]\n", snr_dB);
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "pfncxvFRS")) != -1) {
    switch (opt) {
      case 'p':
        nof_ports = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        cfi = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        pci = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'x':
        dci_cfg.cif_enabled = !dci_cfg.cif_enabled;
        break;
      case 'F':
        false_check = !false_check;
        break;
      case 'R':
        repetitions = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'S':
        snr_dB = (float)strtof(argv[optind], NULL);
        break;
      case 'v':
        increase_srsran_verbose_level();
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  printf("params - pci=%d; rnti=0x%04x; cfi=%d; nof_ports=%d; cif_enabled=%d; nof_prb=%d; snr_db=%+.1f; "
         "repetitions=%d; false_check=%d;\n",
         pci,
         rnti,
         cfi,
         nof_ports,
         dci_cfg.cif_enabled,
         nof_prb,
         snr_dB,
         repetitions,
         false_check);
}

static void print_dci_msg(const char* desc, const srsran_dci_msg_t* dci_msg)
{
  printf("%srnti=0x%04x; format=%s; L=%d; ncce=%d; payload=",
         desc,
         rnti,
         srsran_dci_format_string(dci_msg->format),
         dci_msg->location.L,
         dci_msg->location.ncce);
  srsran_vec_fprint_byte(stdout, dci_msg->payload, dci_msg->nof_bits);
}

static int assert_payload_size(const srsran_cell_t* cell, srsran_dl_sf_cfg_t* dl_sf)
{
  const srsran_dci_format_t formats[] = {
      SRSRAN_DCI_FORMAT0, SRSRAN_DCI_FORMAT1, SRSRAN_DCI_FORMAT1A, SRSRAN_DCI_FORMAT1C, SRSRAN_DCI_FORMAT2A};
  const int prb[6]       = {6, 15, 25, 50, 75, 100};
  const int dci_sz[6][5] = {{21, 19, 21, 8, 28},
                            {22, 23, 22, 10, 31},
                            {25, 27, 25, 12, 36},
                            {27, 31, 27, 13, 41},
                            {27, 33, 27, 14, 42},
                            {28, 39, 28, 15, 48}};

  // Skip if special options are requested
  if (dci_cfg.cif_enabled || dci_cfg.multiple_csi_request_enabled) {
    return SRSRAN_SUCCESS;
  }

  // Skip if MIMO is enabled
  if (cell->nof_ports > 1) {
    return SRSRAN_SUCCESS;
  }

  for (uint32_t i = 0; i < 6; i++) {
    if (prb[i] != cell->nof_prb) {
      continue;
    }
    int n = prb[i];

    uint32_t x[5];
    for (uint32_t j = 0; j < 5; j++) {
      x[j] = srsran_dci_format_sizeof(cell, dl_sf, &dci_cfg, formats[j]);
      if (x[j] != dci_sz[i][j]) {
        ERROR("Invalid DCI payload size for %s and %d PRB. Is %d and should be %d",
              srsran_dci_format_string(formats[j]),
              n,
              x[j],
              dci_sz[i][j]);
        return SRSRAN_ERROR;
      }
    }
    return SRSRAN_SUCCESS;
  }

  return 0;
}

static const srsran_dci_format_t formats[] = {SRSRAN_DCI_FORMAT0,
                                              SRSRAN_DCI_FORMAT1A,
                                              SRSRAN_DCI_FORMAT1,
                                              SRSRAN_DCI_FORMAT2A,
                                              SRSRAN_DCI_FORMAT2,
                                              SRSRAN_DCI_NOF_FORMATS};

static float get_snr_dB(uint32_t L)
{
  static const float snr_table_dB[4] = {15.0f, 6.0f, 5.0f, 0.0f};

  if (isnormal(snr_dB) && L < 4) {
    return snr_dB;
  } else if (L < 4) {
    return snr_table_dB[L];
  } else {
    ERROR("L >= 4\n");
    return 0.0f;
  }
}

static int test_case1()
{
  uint32_t nof_re = SRSRAN_NOF_RE(pdcch_tx.cell);

  // Iterate all possible subframes
  for (uint32_t f_idx = 0; formats[f_idx] != SRSRAN_DCI_NOF_FORMATS; f_idx++) {
    srsran_dci_format_t format                 = formats[f_idx];
    struct timeval      t[3]                   = {};
    uint64_t            t_encode_us            = 0;
    uint64_t            t_encode_count         = 0;
    uint64_t            t_llr_us               = 0;
    uint64_t            t_decode_us            = 0;
    uint64_t            t_decode_count         = 0;
    uint32_t            false_alarm_corr_count = 0;
    float               min_corr               = INFINITY;

    for (uint32_t sf_idx = 0; sf_idx < repetitions * SRSRAN_NOF_SF_X_FRAME; sf_idx++) {
      srsran_dl_sf_cfg_t dl_sf_cfg = {};
      dl_sf_cfg.cfi                = cfi;
      dl_sf_cfg.tti                = sf_idx % 10240;

      // Generate PDCCH locations
      srsran_dci_location_t locations[SRSRAN_MAX_CANDIDATES] = {};
      uint32_t              locations_count                  = 0;
      locations_count +=
          srsran_pdcch_common_locations(&pdcch_tx, &locations[locations_count], SRSRAN_MAX_CANDIDATES_COM, cfi);
      locations_count +=
          srsran_pdcch_ue_locations(&pdcch_tx, &dl_sf_cfg, &locations[locations_count], SRSRAN_MAX_CANDIDATES_UE, rnti);

      // Iterate all possible locations
      for (uint32_t loc = 0; loc < locations_count; loc++) {
        srsran_dci_msg_t dci_tx = {};
        dci_tx.nof_bits         = srsran_dci_format_sizeof(&pdcch_tx.cell, &dl_sf_cfg, &dci_cfg, format);
        dci_tx.location         = locations[loc];
        dci_tx.format           = format;
        dci_tx.rnti             = rnti;

        // Assert DCI size
        TESTASSERT(assert_payload_size(&pdcch_tx.cell, &dl_sf_cfg) == SRSRAN_SUCCESS);

        // Initialise resource grid for each Tx port
        for (uint32_t p = 0; p < nof_ports; p++) {
          srsran_vec_cf_zero(slot_symbols[p], nof_re);
        }

        // Generate Tx DCI
        srsran_random_bit_vector(random_gen, dci_tx.payload, dci_tx.nof_bits);

        // Encode
        gettimeofday(&t[1], NULL);
        TESTASSERT(srsran_pdcch_encode(&pdcch_tx, &dl_sf_cfg, &dci_tx, slot_symbols) == SRSRAN_SUCCESS);
        gettimeofday(&t[2], NULL);
        get_time_interval(t);
        t_encode_us += (size_t)(t[0].tv_sec * 1e6 + t[0].tv_usec);
        t_encode_count++;

        // Set noise level according to aggregation level
        float n0_dB = -get_snr_dB(locations[loc].L);
        TESTASSERT(srsran_channel_awgn_set_n0(&awgn, n0_dB) == SRSRAN_SUCCESS);
        chest_dl_res.noise_estimate = srsran_convert_dB_to_power(n0_dB);

        // Apply AWGN
        for (uint32_t p = 0; p < nof_ports; p++) {
          srsran_channel_awgn_run_c(&awgn, slot_symbols[p], slot_symbols[p], nof_re);
        }

        // Extract LLR
        gettimeofday(&t[1], NULL);
        TESTASSERT(srsran_pdcch_extract_llr(&pdcch_rx, &dl_sf_cfg, &chest_dl_res, slot_symbols) == SRSRAN_SUCCESS);
        gettimeofday(&t[2], NULL);
        get_time_interval(t);
        t_llr_us += (size_t)(t[0].tv_sec * 1e6 + t[0].tv_usec);

        // Try decoding the PDCCH in all possible locations
        for (uint32_t loc_rx = 0; loc_rx < locations_count; loc_rx++) {
          // Skip location if:
          // - False check is disabled and Tx/Rx dont match
          // - Tx aggregation level is bigger than Rx aggregation level
          if ((!false_check && loc_rx != loc) || locations[loc_rx].L < locations[loc].L) {
            continue;
          }

          // Prepare DCI message context
          srsran_dci_msg_t dci_rx = {};
          dci_rx.location         = locations[loc_rx];
          dci_rx.format           = format;

          // Try to decode PDCCH message
          gettimeofday(&t[1], NULL);
          TESTASSERT(srsran_pdcch_decode_msg(&pdcch_rx, &dl_sf_cfg, &dci_cfg, &dci_rx) == SRSRAN_SUCCESS);
          gettimeofday(&t[2], NULL);
          get_time_interval(t);
          t_decode_us += (size_t)(t[0].tv_sec * 1e6 + t[0].tv_usec);
          t_decode_count++;

          // Compute LLR correlation
          float corr = srsran_pdcch_msg_corr(&pdcch_rx, &dci_rx);

          bool rnti_match     = (dci_tx.rnti == dci_rx.rnti);
          bool location_match = (loc == loc_rx);
          bool payload_match  = (memcmp(dci_tx.payload, dci_rx.payload, dci_tx.nof_bits) == 0);
          bool corr_thr       = corr > 0.5f;

          // Skip location if the decoding is not successful in a different location than transmitted
          if (!location_match && !rnti_match) {
            continue;
          }

          // Skip location if the correlation does not surpass the threshold
          if (!location_match && !corr_thr) {
            false_alarm_corr_count++;
            continue;
          }

          // Assert correlation only if location matches
          if (location_match) {
            TESTASSERT(corr_thr);
            if (location_match && corr < min_corr) {
              min_corr = corr;
            }
          }

          if (get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO || !payload_match) {
            // If payload is not match and there is no logging, set logging to info and run the decoder again
            if (get_srsran_verbose_level() < SRSRAN_VERBOSE_INFO) {
              printf("-- Detected payload was not matched, repeating decode with INFO logs (n0: %+.1f dB, corr: %f)\n",
                     n0_dB,
                     corr);
              set_srsran_verbose_level(SRSRAN_VERBOSE_INFO);
              srsran_pdcch_decode_msg(&pdcch_rx, &dl_sf_cfg, &dci_cfg, &dci_rx);
            }
            print_dci_msg("Tx: ", &dci_tx);
            print_dci_msg("Rx: ", &dci_rx);
          }

          // Assert received message
          TESTASSERT(payload_match);
        }
      }
    }

    if (!t_encode_count || !t_decode_count) {
      ERROR("Error in test case 1: undefined division");
      return SRSRAN_ERROR;
    }

    printf("test_case_1 - format %s - passed - %.1f usec/encode; %.1f usec/llr; %.1f usec/decode; min_corr=%f; "
           "false_alarm_prob=%f;\n",
           srsran_dci_format_string(format),
           (double)t_encode_us / (double)(t_encode_count),
           (double)t_llr_us / (double)(t_encode_count),
           (double)t_decode_us / (double)(t_decode_count),
           min_corr,
           (double)false_alarm_corr_count / (double)t_decode_count);
  }

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  srsran_regs_t regs = {};
  int           i    = 0;
  int           ret  = SRSRAN_ERROR;

  parse_args(argc, argv);
  random_gen = srsran_random_init(0x1234);

  // Create cell
  srsran_cell_t cell   = {};
  cell.nof_prb         = nof_prb;
  cell.nof_ports       = nof_ports;
  cell.cp              = SRSRAN_CP_NORM;
  cell.phich_resources = SRSRAN_PHICH_R_1;
  cell.phich_length    = SRSRAN_PHICH_NORM;

  // Initialise channel estimates with identity matrix
  if (srsran_chest_dl_res_init(&chest_dl_res, cell.nof_prb) < SRSRAN_SUCCESS) {
    ERROR("Error channel estimates");
    goto quit;
  }
  srsran_chest_dl_res_set_identity(&chest_dl_res);

  // Allocate grid
  uint32_t nof_re = SRSRAN_NOF_RE(cell);
  for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
    slot_symbols[i] = srsran_vec_cf_malloc(nof_re);
    if (slot_symbols[i] == NULL) {
      ERROR("malloc");
      goto quit;
    }
  }

  if (srsran_regs_init(&regs, cell)) {
    ERROR("Error initiating regs");
    goto quit;
  }

  if (srsran_pdcch_init_enb(&pdcch_tx, cell.nof_prb)) {
    ERROR("Error creating PDCCH object");
    goto quit;
  }
  if (srsran_pdcch_set_cell(&pdcch_tx, &regs, cell)) {
    ERROR("Error setting cell in PDCCH object");
    goto quit;
  }

  if (srsran_pdcch_init_ue(&pdcch_rx, cell.nof_prb, nof_ports)) {
    ERROR("Error creating PDCCH object");
    goto quit;
  }

  if (srsran_pdcch_set_cell(&pdcch_rx, &regs, cell)) {
    ERROR("Error setting cell in PDCCH object");
    goto quit;
  }

  if (srsran_channel_awgn_init(&awgn, 0x1234) < SRSRAN_SUCCESS) {
    ERROR("Error init AWGN");
    goto quit;
  }

  // Execute actual test cases
  if (test_case1() < SRSRAN_SUCCESS) {
    ERROR("Test case 1 failed");
    goto quit;
  }

  ret = SRSRAN_SUCCESS;

quit:
  srsran_pdcch_free(&pdcch_tx);
  srsran_pdcch_free(&pdcch_rx);
  srsran_chest_dl_res_free(&chest_dl_res);
  srsran_regs_free(&regs);
  srsran_random_free(random_gen);
  srsran_channel_awgn_free(&awgn);

  for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
    if (slot_symbols[i]) {
      free(slot_symbols[i]);
    }
  }
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  exit(ret);
}
