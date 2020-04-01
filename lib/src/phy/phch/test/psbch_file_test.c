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

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <srslte/phy/ch_estimation/chest_sl.h>
#include <srslte/phy/dft/ofdm.h>
#include <srslte/phy/io/filesource.h>
#include <srslte/phy/phch/mib_sl.h>
#include <srslte/phy/phch/psbch.h>
#include <srslte/phy/sync/cfo.h>
#include <srslte/phy/utils/debug.h>
#include <srslte/phy/utils/vector.h>

char*          input_file_name;
int32_t        N_sl_id                = 168;
uint32_t       offset                 = 0;
float          frequency_offset       = 0.0;
float          snr                    = 100.0;
srslte_cp_t    cp                     = SRSLTE_CP_NORM;
uint32_t       nof_prb                = 6;
bool           use_standard_lte_rates = false;
bool           do_equalization        = true;
srslte_sl_tm_t tm                     = SRSLTE_SIDELINK_TM2;

srslte_filesource_t fsrc;

void usage(char* prog)
{
  printf("Usage: %s [cdeipt]\n", prog);
  printf("\t-i input_file_name\n");
  printf("\t-p nof_prb [Default %d]\n", nof_prb);
  printf("\t-e extended CP [Default normal]\n");
  printf("\t-d use_standard_lte_rates [Default %i]\n", use_standard_lte_rates);
  printf("\t-s skip equalization [Default no]\n");
  printf("\t-c N_sl_id [Default %d]\n", N_sl_id);
  printf("\t-t Sidelink transmission mode {1,2,3,4} [Default %d]\n", (tm + 1));
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cdeisptv")) != -1) {
    switch (opt) {
      case 'c':
        N_sl_id = (int32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'd':
        use_standard_lte_rates = true;
        break;
      case 's':
        do_equalization = false;
        break;
      case 'e':
        cp = SRSLTE_CP_EXT;
        break;
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'p':
        nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 't':
        switch (strtol(argv[optind], NULL, 10)) {
          case 1:
            tm = SRSLTE_SIDELINK_TM1;
            break;
          case 2:
            tm = SRSLTE_SIDELINK_TM2;
            break;
          case 3:
            tm = SRSLTE_SIDELINK_TM3;
            break;
          case 4:
            tm = SRSLTE_SIDELINK_TM4;
            break;
          default:
            usage(argv[0]);
            exit(-1);
        }
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
  srslte_use_standard_symbol_size(use_standard_lte_rates);

  int32_t symbol_sz = srslte_symbol_sz(nof_prb);
  printf("Symbol SZ: %i\n", symbol_sz);

  uint32_t sf_n_samples = srslte_symbol_sz(nof_prb) * 15;
  printf("sf_n_samples: %i\n", sf_n_samples);

  uint32_t sf_n_re             = SRSLTE_CP_NSYMB(cp) * SRSLTE_NRE * 2 * nof_prb;
  cf_t*    sf_buffer           = srslte_vec_cf_malloc(sf_n_re);
  cf_t*    equalized_sf_buffer = srslte_vec_cf_malloc(sf_n_re);

  cf_t* input_buffer  = srslte_vec_cf_malloc(sf_n_samples);
  cf_t* output_buffer = srslte_vec_cf_malloc(sf_n_samples);

  // TX
  srslte_ofdm_t ifft;
  if (srslte_ofdm_tx_init(&ifft, cp, sf_buffer, output_buffer, nof_prb)) {
    ERROR("Error creating IFFT object\n");
    return SRSLTE_ERROR;
  }
  srslte_ofdm_set_normalize(&ifft, true);
  srslte_ofdm_set_freq_shift(&ifft, 0.5);

  // RX
  srslte_ofdm_t fft;
  if (srslte_ofdm_rx_init(&fft, cp, input_buffer, sf_buffer, nof_prb)) {
    fprintf(stderr, "Error creating FFT object\n");
    return SRSLTE_ERROR;
  }
  srslte_ofdm_set_normalize(&fft, true);
  srslte_ofdm_set_freq_shift(&fft, -0.5);

  // PSBCH
  srslte_psbch_t psbch;
  if (srslte_psbch_init(&psbch, nof_prb, N_sl_id, tm, SRSLTE_CP_NORM) != SRSLTE_SUCCESS) {
    ERROR("Error in psbch init\n");
    return SRSLTE_ERROR;
  }

  // PSCBH DMRS
  srslte_chest_sl_t psbch_chest;
  if (srslte_chest_sl_init_psbch_dmrs(&psbch_chest) != SRSLTE_SUCCESS) {
    ERROR("Error in psbch dmrs init\n");
    return SRSLTE_ERROR;
  }

  // Read subframe from third party implementations
  if (!input_file_name || srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
    printf("Error opening file %s\n", input_file_name);
    return SRSLTE_ERROR;
  }

  srslte_filesource_read(&fsrc, input_buffer, sf_n_samples);
  // srslte_vec_sc_prod_cfc(input_buffer, sqrtf(symbol_sz), input_buffer, sf_n_samples);

  // Run FFT
  srslte_ofdm_rx_sf(&fft);

  // Equalize
  if (do_equalization) {
    srslte_chest_sl_gen_psbch_dmrs(&psbch_chest, tm, N_sl_id);
    srslte_chest_sl_psbch_ls_estimate_equalize(&psbch_chest, sf_buffer, equalized_sf_buffer, nof_prb, tm, cp);
  } else {
    // just copy symbols
    memcpy(equalized_sf_buffer, sf_buffer, sizeof(cf_t) * sf_n_re);
  }

  // prepare Rx buffer
  uint8_t mib_sl_rx[SRSLTE_MIB_SL_MAX_LEN] = {};

  // Decode PSBCH
  if (srslte_psbch_decode(&psbch, equalized_sf_buffer, mib_sl_rx, sizeof(mib_sl_rx)) == SRSLTE_SUCCESS) {
    printf("Rx payload: ");
    srslte_vec_fprint_hex(stdout, mib_sl_rx, sizeof(mib_sl_rx));

    // Unpack and print MIB-SL
    srslte_mib_sl_t mib_sl;
    srslte_mib_sl_init(&mib_sl, tm);
    srslte_mib_sl_unpack(&mib_sl, mib_sl_rx);
    srslte_mib_sl_printf(stdout, &mib_sl);

    // check decoded bandwidth matches user configured value
    if (srslte_mib_sl_bandwith_to_prb[mib_sl.sl_bandwidth_r12] == nof_prb) {
      ret = SRSLTE_SUCCESS;
    }
  }

  if (SRSLTE_VERBOSE_ISDEBUG()) {
    char* filename = (do_equalization) ? "psbch_rx_syms_eq_on.bin" : "psbch_rx_syms_eq_off.bin";
    printf("Saving PSBCH symbols (%d) to %s\n", psbch.E / psbch.Qm, filename);
    srslte_vec_save_file(filename, psbch.mod_symbols, psbch.E / psbch.Qm * sizeof(cf_t));
  }

  srslte_ofdm_tx_free(&ifft);
  srslte_ofdm_rx_free(&fft);

  srslte_filesource_free(&fsrc);

  srslte_chest_sl_free(&psbch_chest);
  srslte_psbch_free(&psbch);

  free(sf_buffer);
  free(equalized_sf_buffer);
  free(input_buffer);
  free(output_buffer);

  return ret;
}
