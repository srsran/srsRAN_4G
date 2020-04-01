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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include <stdbool.h>

#include "srslte/phy/sync/psss.h"
#include "srslte/phy/sync/ssss.h"
#include "srslte/srslte.h"

char*          input_file_name;
int32_t        N_sl_id                = 168;
uint32_t       offset                 = 0;
float          frequency_offset       = 0.0;
float          snr                    = 100.0;
srslte_cp_t    cp                     = SRSLTE_CP_NORM;
uint32_t       nof_prb                = 6;
bool           use_standard_lte_rates = false;
srslte_sl_tm_t tm                     = SRSLTE_SIDELINK_TM2;

srslte_filesource_t fsrc;

void usage(char* prog)
{
  printf("Usage: %s [cdefiopstv]\n", prog);
  printf("\t-i input_file_name\n");
  printf("\t-p nof_prb [Default %d]\n", nof_prb);
  printf("\t-o offset [Default %d]\n", offset);
  printf("\t-e extended CP [Default normal]\n");
  printf("\t-d use_standard_lte_rates [Default %i]\n", use_standard_lte_rates);
  printf("\t-v srslte_verbose\n");
  printf("\nSelf-tests only:\n");
  printf("\t-c N_sl_id [Default %d]\n", N_sl_id);
  printf("\t-t Sidelink transmission mode {1,2,3,4} [Default %d]\n", (tm + 1));
  printf("\t-f frequency_offset [Default %.3f]\n", frequency_offset);
  printf("\t-s snr [Default %.3f]\n", snr);
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cdefiopstv")) != -1) {
    switch (opt) {
      case 'c':
        N_sl_id = (int32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'd':
        use_standard_lte_rates = true;
        break;
      case 'e':
        cp = SRSLTE_CP_EXT;
        break;
      case 'f':
        frequency_offset = strtof(argv[optind], NULL);
        break;
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'o':
        offset = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        snr = strtof(argv[optind], NULL);
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

  parse_args(argc, argv);
  srslte_use_standard_symbol_size(use_standard_lte_rates);

  int32_t symbol_sz = srslte_symbol_sz(nof_prb);
  printf("Symbol SZ: %i\n", symbol_sz);

  uint32_t sf_n_samples = srslte_symbol_sz(nof_prb) * 15;
  printf("sf_n_samples: %i\n", sf_n_samples);

  uint32_t sf_n_re   = SRSLTE_CP_NSYMB(SRSLTE_CP_NORM) * SRSLTE_NRE * 2 * nof_prb;
  cf_t*    sf_buffer = srslte_vec_cf_malloc(sf_n_re);

  cf_t* input_buffer      = srslte_vec_cf_malloc(sf_n_samples);
  cf_t* input_buffer_temp = srslte_vec_cf_malloc(sf_n_samples);

  uint32_t output_buffer_len = 0;
  cf_t*    output_buffer     = srslte_vec_cf_malloc(sf_n_samples);

  srslte_ofdm_t ifft;
  if (srslte_ofdm_tx_init(&ifft, cp, sf_buffer, output_buffer, nof_prb)) {
    ERROR("Error creating IFFT object\n");
    return SRSLTE_ERROR;
  }
  srslte_ofdm_set_normalize(&ifft, true);
  srslte_ofdm_set_freq_shift(&ifft, 0.5);

  srslte_psss_t psss;
  srslte_psss_init(&psss, nof_prb, cp);

  srslte_ssss_t ssss;
  srslte_ssss_init(&ssss, nof_prb, cp, tm);

  if (input_file_name) {
    if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
      printf("Error opening file %s\n", input_file_name);
      return SRSLTE_ERROR;
    }
  } else {
    // Self-test with a single subframe (but can be extended to a radio frame or a PSCCH Period)

    srslte_vec_cf_zero(sf_buffer, sf_n_re);

    srslte_psss_put_sf_buffer(psss.psss_signal[((N_sl_id < 168) ? 0 : 1)], sf_buffer, nof_prb, cp);
    srslte_ssss_put_sf_buffer(ssss.ssss_signal[N_sl_id], sf_buffer, nof_prb, cp);

    // TS 36.211 Section 9.3.2: The last SC-FDMA symbol in a sidelink subframe serves as a guard period and shall
    // not be used for sidelink transmission.
    srslte_vec_cf_zero(&sf_buffer[SRSLTE_NRE * nof_prb * (SRSLTE_CP_NSYMB(cp) * 2 - 1)], SRSLTE_NRE * nof_prb);
    srslte_ofdm_tx_sf(&ifft);

    output_buffer_len = sf_n_samples;

    // ADD CHANNEL NOISE
    if (snr < 50) {
      float std_dev = powf(10.0f, -(snr + 3.0f) / 20.0f);
      srslte_ch_awgn_c(output_buffer, output_buffer, std_dev, output_buffer_len);
    }

    // ADD FREQUENCY OFFSET
    if (frequency_offset != 0) {
      for (uint32_t i = 0; i < output_buffer_len; i++) {
        output_buffer[i] = output_buffer[i] * cexpf(I * (float)(2 * M_PI / (symbol_sz * 15e3)) * frequency_offset * i);
      }
    }
  }

  int search_window_size = sf_n_samples - (2 * (symbol_sz + SRSLTE_CP_SZ(symbol_sz, cp)));

  int32_t  samples_read = 0;
  uint32_t samples_to_read;
  uint32_t offset_pos        = 0;
  uint32_t output_buffer_pos = 0;

  struct timeval t[3];
  gettimeofday(&t[1], NULL);

  bool sync = false;

  while (sync == false) {

    samples_to_read = sf_n_samples - offset_pos;

    if (offset < samples_to_read) {
      srslte_vec_cf_zero(input_buffer, offset);
      samples_to_read = sf_n_samples - offset_pos - offset;

      if (input_file_name) {
        // Read and normalize samples from file
        samples_read = srslte_filesource_read(&fsrc, &input_buffer[offset + offset_pos], samples_to_read);
        // Normalization factor for third party input files
        srslte_vec_sc_prod_cfc(
            &input_buffer[offset + offset_pos], sqrtf(symbol_sz), &input_buffer[offset + offset_pos], samples_read);
      } else {
        // Self-test
        if ((output_buffer_len - output_buffer_pos) >= samples_to_read) {
          memcpy(&input_buffer[offset + offset_pos], &output_buffer[output_buffer_pos], sizeof(cf_t) * samples_to_read);
          samples_read = samples_to_read;
          output_buffer_pos += samples_read;
        } else {
          samples_read = 0;
        }
      }

      offset = 0;
    } else {
      samples_read = samples_to_read;
      offset       = offset - samples_read;
      srslte_vec_cf_zero(input_buffer, samples_read);
    }

    if (samples_read != samples_to_read) {
      printf("Couldn't read %i samples\n", samples_to_read);
      goto clean_exit;
    }

    // Find sync signals
    if (srslte_psss_find(&psss, input_buffer, nof_prb, cp) == SRSLTE_SUCCESS) {

      printf("PSSS correlation peak pos: %d value: %f N_id_2: %d\n",
             psss.corr_peak_pos,
             psss.corr_peak_value,
             psss.N_id_2);

      if (psss.corr_peak_pos - sf_n_samples == 0) {
        // Find SSSS
        if (srslte_ssss_find(&ssss, input_buffer, nof_prb, psss.N_id_2, cp) == SRSLTE_SUCCESS) {

          printf("SSSS correlation peak pos: %d value: %f N_sl_id: %d \n",
                 ssss.corr_peak_pos,
                 ssss.corr_peak_value,
                 ssss.N_sl_id);

          gettimeofday(&t[2], NULL);
          get_time_interval(t);
          printf("\nSLSS found with N_sl_id: %d (in %.0f usec)\n\n",
                 ssss.N_sl_id,
                 (int)t[0].tv_sec * 1e6 + (int)t[0].tv_usec);

          sync = true;
        }
      } else {
        // Sample offset correction
        offset_pos = sf_n_samples - (psss.corr_peak_pos - sf_n_samples);
        memcpy(input_buffer_temp, &input_buffer[sf_n_samples - offset_pos], sizeof(cf_t) * offset_pos);
        memcpy(input_buffer, input_buffer_temp, sizeof(cf_t) * offset_pos);
      }
    } else {
      // Next search window
      offset_pos = sf_n_samples - search_window_size;
      memcpy(input_buffer_temp, &input_buffer[sf_n_samples - offset_pos], sizeof(cf_t) * offset_pos);
      memcpy(input_buffer, input_buffer_temp, sizeof(cf_t) * offset_pos);
    }
  }

clean_exit:
  srslte_filesource_free(&fsrc);
  srslte_ofdm_tx_free(&ifft);
  srslte_ssss_free(&ssss);
  srslte_psss_free(&psss);
  free(input_buffer);
  free(input_buffer_temp);
  free(sf_buffer);
  free(output_buffer);

  if (sync) {
    return SRSLTE_SUCCESS;
  }

  return SRSLTE_ERROR;
}
