/**
 * Copyright 2013-2021 Software Radio Systems Limited
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
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "srsran/phy/ch_estimation/chest_sl.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/io/filesource.h"
#include "srsran/phy/phch/mib_sl.h"
#include "srsran/phy/phch/psbch.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

srsran_cell_sl_t cell = {.nof_prb = 6, .N_sl_id = 168, .tm = SRSRAN_SIDELINK_TM2, .cp = SRSRAN_CP_NORM};
char*            input_file_name;
uint32_t         offset                 = 0;
float            frequency_offset       = 0.0;
float            snr                    = 100.0;
bool             use_standard_lte_rates = false;
bool             do_equalization        = true;

srsran_filesource_t fsrc;

void usage(char* prog)
{
  printf("Usage: %s [cdeipt]\n", prog);
  printf("\t-i input_file_name\n");
  printf("\t-p nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-e extended CP [Default normal]\n");
  printf("\t-d use_standard_lte_rates [Default %i]\n", use_standard_lte_rates);
  printf("\t-s skip equalization [Default no]\n");
  printf("\t-c N_sl_id [Default %d]\n", cell.N_sl_id);
  printf("\t-t Sidelink transmission mode {1,2,3,4} [Default %d]\n", (cell.tm + 1));
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cdeisptv")) != -1) {
    switch (opt) {
      case 'c':
        cell.N_sl_id = (int32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'd':
        use_standard_lte_rates = true;
        break;
      case 's':
        do_equalization = false;
        break;
      case 'e':
        cell.cp = SRSRAN_CP_EXT;
        break;
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'p':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 't':
        switch (strtol(argv[optind], NULL, 10)) {
          case 1:
            cell.tm = SRSRAN_SIDELINK_TM1;
            break;
          case 2:
            cell.tm = SRSRAN_SIDELINK_TM2;
            break;
          case 3:
            cell.tm = SRSRAN_SIDELINK_TM3;
            break;
          case 4:
            cell.tm = SRSRAN_SIDELINK_TM4;
            break;
          default:
            usage(argv[0]);
            exit(-1);
        }
        break;
      case 'v':
        increase_srsran_verbose_level();
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (SRSRAN_CP_ISEXT(cell.cp) && cell.tm >= SRSRAN_SIDELINK_TM3) {
    ERROR("Selected TM does not support extended CP");
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  int ret = SRSRAN_ERROR;

  parse_args(argc, argv);
  srsran_use_standard_symbol_size(use_standard_lte_rates);

  int32_t symbol_sz = srsran_symbol_sz(cell.nof_prb);
  printf("Symbol SZ: %i\n", symbol_sz);

  uint32_t sf_n_samples = srsran_symbol_sz(cell.nof_prb) * 15;
  printf("sf_n_samples: %i\n", sf_n_samples);

  uint32_t sf_n_re             = SRSRAN_SF_LEN_RE(cell.nof_prb, cell.cp);
  cf_t*    sf_buffer           = srsran_vec_cf_malloc(sf_n_re);
  cf_t*    equalized_sf_buffer = srsran_vec_cf_malloc(sf_n_re);

  cf_t* input_buffer  = srsran_vec_cf_malloc(sf_n_samples);
  cf_t* output_buffer = srsran_vec_cf_malloc(sf_n_samples);

  // TX
  srsran_ofdm_t ifft;
  if (srsran_ofdm_tx_init(&ifft, cell.cp, sf_buffer, output_buffer, cell.nof_prb)) {
    ERROR("Error creating IFFT object");
    return SRSRAN_ERROR;
  }
  srsran_ofdm_set_normalize(&ifft, true);
  srsran_ofdm_set_freq_shift(&ifft, 0.5);

  // RX
  srsran_ofdm_t fft;
  if (srsran_ofdm_rx_init(&fft, cell.cp, input_buffer, sf_buffer, cell.nof_prb)) {
    fprintf(stderr, "Error creating FFT object\n");
    return SRSRAN_ERROR;
  }
  srsran_ofdm_set_normalize(&fft, true);
  srsran_ofdm_set_freq_shift(&fft, -0.5);

  // PSBCH
  srsran_psbch_t psbch;
  if (srsran_psbch_init(&psbch, cell.nof_prb, cell.N_sl_id, cell.tm, cell.cp) != SRSRAN_SUCCESS) {
    ERROR("Error in psbch init");
    return SRSRAN_ERROR;
  }

  // PSCBH DMRS
  srsran_sl_comm_resource_pool_t sl_comm_resource_pool;
  if (srsran_sl_comm_resource_pool_get_default_config(&sl_comm_resource_pool, cell) != SRSRAN_SUCCESS) {
    ERROR("Error initializing sl_comm_resource_pool");
    return SRSRAN_ERROR;
  }

  srsran_chest_sl_t psbch_chest;
  if (srsran_chest_sl_init(&psbch_chest, SRSRAN_SIDELINK_PSBCH, cell, &sl_comm_resource_pool) != SRSRAN_SUCCESS) {
    ERROR("Error in chest PSBCH init");
    return SRSRAN_ERROR;
  }

  // Read subframe from third party implementations
  if (!input_file_name || srsran_filesource_init(&fsrc, input_file_name, SRSRAN_COMPLEX_FLOAT_BIN)) {
    printf("Error opening file %s\n", input_file_name);
    return SRSRAN_ERROR;
  }

  srsran_filesource_read(&fsrc, input_buffer, sf_n_samples);
  // srsran_vec_sc_prod_cfc(input_buffer, sqrtf(symbol_sz), input_buffer, sf_n_samples);

  // Run FFT
  srsran_ofdm_rx_sf(&fft);

  // Equalize
  if (do_equalization) {
    srsran_chest_sl_ls_estimate_equalize(&psbch_chest, sf_buffer, equalized_sf_buffer);
  } else {
    // just copy symbols
    memcpy(equalized_sf_buffer, sf_buffer, sizeof(cf_t) * sf_n_re);
  }

  // prepare Rx buffer
  uint8_t mib_sl_rx[SRSRAN_MIB_SL_MAX_LEN] = {};

  // Decode PSBCH
  if (srsran_psbch_decode(&psbch, equalized_sf_buffer, mib_sl_rx, sizeof(mib_sl_rx)) == SRSRAN_SUCCESS) {
    printf("Rx payload: ");
    srsran_vec_fprint_hex(stdout, mib_sl_rx, sizeof(mib_sl_rx));

    // Unpack and print MIB-SL
    srsran_mib_sl_t mib_sl;
    srsran_mib_sl_init(&mib_sl, cell.tm);
    srsran_mib_sl_unpack(&mib_sl, mib_sl_rx);
    srsran_mib_sl_printf(stdout, &mib_sl);

    // check decoded bandwidth matches user configured value
    if (srsran_mib_sl_bandwith_to_prb[mib_sl.sl_bandwidth_r12] == cell.nof_prb) {
      ret = SRSRAN_SUCCESS;
    }
  }

  if (SRSRAN_VERBOSE_ISDEBUG()) {
    char* filename = (do_equalization) ? "psbch_rx_syms_eq_on.bin" : "psbch_rx_syms_eq_off.bin";
    printf("Saving PSBCH symbols (%d) to %s\n", psbch.E / psbch.Qm, filename);
    srsran_vec_save_file(filename, psbch.mod_symbols, psbch.E / psbch.Qm * sizeof(cf_t));
  }

  srsran_ofdm_tx_free(&ifft);
  srsran_ofdm_rx_free(&fft);

  srsran_filesource_free(&fsrc);

  srsran_chest_sl_free(&psbch_chest);
  srsran_psbch_free(&psbch);

  free(sf_buffer);
  free(equalized_sf_buffer);
  free(input_buffer);
  free(output_buffer);

  return ret;
}
