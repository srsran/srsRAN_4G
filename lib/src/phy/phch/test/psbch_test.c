/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
#include <srslte/phy/utils/vector.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "srslte/phy/dft/ofdm.h"

#include "srslte/phy/io/filesource.h"

#include "srslte/phy/ch_estimation/chest_sl.h"
#include "srslte/phy/phch/psbch.h"
#include "srslte/phy/ue/ue_mib_sl.h"

#define SRSLTE_NSUBFRAMES_X_FRAME 10

char*       input_file_name = NULL;
srslte_cp_t cp              = SRSLTE_CP_NORM;
int         sf_n_re, sf_n_samples, fft_size;
uint32_t    N_sl_id = 168, nof_prb = 25;
bool        use_standard_lte_rates = false;

void usage(char* prog)
{
  printf("Usage: %s [cdip]\n", prog);
  printf("\t-c n_sl_id [Default %d]\n", N_sl_id);
  printf("\t-d use_standard_lte_rates [Deafult %i]\n", use_standard_lte_rates);
  printf("\t-i input file (radio frame)\n");
  printf("\t-p nof_prb [Default %d]\n", nof_prb);
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cdip")) != -1) {
    switch (opt) {
      case 'c':
        N_sl_id = atoi(argv[optind]);
        break;
      case 'd':
        use_standard_lte_rates = true;
        break;
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'p':
        nof_prb = atoi(argv[optind]);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);
  srslte_use_standard_symbol_size(use_standard_lte_rates);
  uint32_t symbol_sz = srslte_symbol_sz(nof_prb);
  printf("Symbol SZ: %i\n", symbol_sz);

  sf_n_samples = symbol_sz * 15;
  fft_size     = sf_n_samples * 2;
  sf_n_re      = SRSLTE_CP_NSYMB(SRSLTE_CP_NORM) * 12 * 2 * nof_prb;

  int ret = SRSLTE_ERROR;

  uint8_t* mib_sl_rx = srslte_vec_malloc(sizeof(uint8_t) * SRSLTE_MIB_SL_MAX_LEN);
  bzero(mib_sl_rx, sizeof(uint8_t) * SRSLTE_MIB_SL_MAX_LEN);

  cf_t* sf_buffer_samples = srslte_vec_malloc(sizeof(cf_t) * sf_n_samples);
  bzero(sf_buffer_samples, sizeof(cf_t) * sf_n_samples);

  cf_t* rx_re = srslte_vec_malloc(sizeof(cf_t) * sf_n_re);
  bzero(rx_re, sizeof(cf_t) * sf_n_re);

  cf_t* sf_buffer = srslte_vec_malloc(sizeof(cf_t) * sf_n_re);
  bzero(sf_buffer, sizeof(cf_t) * sf_n_re);

  cf_t* equalized_sf = srslte_vec_malloc(sizeof(cf_t) * sf_n_re);
  bzero(equalized_sf, sizeof(cf_t) * sf_n_re);

  srslte_ofdm_t fft;
  if (srslte_ofdm_rx_init(&fft, SRSLTE_CP_NORM, sf_buffer_samples, rx_re, nof_prb)) {
    fprintf(stderr, "Error creating iFFT object\n");
    return SRSLTE_ERROR;
  }
  srslte_ofdm_set_normalize(&fft, true);
  srslte_ofdm_set_freq_shift(&fft, (float)-0.5);

  srslte_ofdm_t ifft;
  if (srslte_ofdm_tx_init(&ifft, SRSLTE_CP_NORM, sf_buffer, sf_buffer_samples, nof_prb)) {
    fprintf(stderr, "Error creating iFFT object\n");
    return SRSLTE_ERROR;
  }
  srslte_ofdm_set_normalize(&ifft, true);
  srslte_ofdm_set_freq_shift(&ifft, 0.5);

  srslte_psbch_t psbch;
  if (srslte_psbch_init(&psbch, N_sl_id, nof_prb)) {
    printf("Error creating PSBCH object\n");
    return SRSLTE_ERROR;
  }

  srslte_ue_mib_sl_t mib_sl;
  srslte_ue_mib_sl_set(&mib_sl, nof_prb, 0, 0, 0, false);

  srslte_chest_sl_t psbch_est;
  srslte_chest_sl_init_psbch_dmrs(&psbch_est);
  srslte_chest_sl_gen_psbch_dmrs(&psbch_est, SRSLTE_SIDELINK_TM2, nof_prb, 0, N_sl_id);

  if (!input_file_name) {
    // *************************************************************************************************************
    // PSBCH ENCODING
    // *************************************************************************************************************
    srslte_ue_mib_sl_pack(&mib_sl, psbch.a);
    srslte_psbch_encode(&psbch, psbch.a);

    // Map PSBCH to subframe
    srslte_psbch_put(&psbch, sf_buffer);

    // Map PSBCH DMRS to subframe
    srslte_chest_sl_put_psbch_dmrs(&psbch_est, sf_buffer, SRSLTE_SIDELINK_TM2, nof_prb);

    // TS 36.211 Section 9.3.2: The last SC-FDMA symbol in a sidelink subframe serves as a guard period and shall not be
    // used for sidelink transmission.
    bzero(&sf_buffer[SRSLTE_NRE * nof_prb * (SRSLTE_CP_NSYMB(SRSLTE_CP_NORM) * 2 - 1)],
          sizeof(cf_t) * SRSLTE_NRE * nof_prb);
    srslte_ofdm_tx_sf(&ifft);

  } else {
    // *************************************************************************************************************
    // RADIO FRAME FROM MATLAB
    // *************************************************************************************************************
    srslte_filesource_t fsrc;
    if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
      printf("Error opening file %s\n", input_file_name);
      return SRSLTE_ERROR;
    }

    srslte_filesource_read(&fsrc, sf_buffer_samples, sf_n_samples);
    srslte_vec_sc_prod_cfc(sf_buffer_samples, sqrtf(symbol_sz), sf_buffer_samples, (uint32_t)sf_n_samples);

    srslte_filesource_free(&fsrc);
  }

  srslte_ofdm_rx_sf(&fft);
  // TS 36.211 Section 9.3.2: The last SC-FDMA symbol in a sidelink subframe serves as a guard period and shall not be
  // used for sidelink transmission.
  bzero(&rx_re[SRSLTE_NRE * nof_prb * (SRSLTE_CP_NSYMB(SRSLTE_CP_NORM) * 2 - 1)], sizeof(cf_t) * SRSLTE_NRE * nof_prb);

  srslte_chest_sl_psbch_ls_estimate_equalize(&psbch_est, rx_re, equalized_sf, nof_prb);

  srslte_psbch_get(&psbch, equalized_sf);
  if (srslte_psbch_decode(&psbch, mib_sl_rx) == SRSLTE_SUCCESS) {
    printf("RX MIB-SL: ");
    for (int i = 0; i < SRSLTE_MIB_SL_MAX_LEN; ++i) {
      printf("%i", mib_sl_rx[i]);
    }
    printf("\n");
    ret = SRSLTE_SUCCESS;

    srslte_ue_mib_sl_t rx_mib_sl;
    srlste_ue_mib_sl_unpack(&rx_mib_sl, mib_sl_rx);

    printf("Bandwidth: %i\n", rx_mib_sl.sl_bandwidth_r12);
    printf("TDD Config: %i\n", rx_mib_sl.tdd_config_sl_r12);
    printf("Direct Frame Number: %i\n", rx_mib_sl.direct_frame_number_r12);
    printf("Direct Subframe Number: %i\n", rx_mib_sl.direct_subframe_number_r12);
    printf("In Coverage: %i\n", rx_mib_sl.in_coverage_r12);
  }

  srslte_chest_sl_free(&psbch_est);

  srslte_psbch_free(&psbch);
  srslte_ue_mib_sl_free(&mib_sl);

  srslte_ofdm_rx_free(&fft);
  srslte_ofdm_tx_free(&ifft);

  free(rx_re);
  free(mib_sl_rx);
  free(sf_buffer);
  free(equalized_sf);
  free(sf_buffer_samples);

  printf(ret == SRSLTE_ERROR ? "FAILED\n" : "SUCCESS\n");

  return ret;
}
