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

#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "srslte/phy/ch_estimation/chest_dl_nbiot.h"
#include "srslte/phy/channel/ch_awgn.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      fprintf(stderr, "[%s][Line %d]: FAIL %s\n", __FUNCTION__, __LINE__, (#cond));                                    \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

srslte_nbiot_cell_t cell = {.base           = {.nof_prb = 1, .cp = SRSLTE_CP_NORM, .id = 0},
                            .base.nof_ports = 1,
                            .nbiot_prb      = 0,
                            .n_id_ncell     = 0};

bool  have_ofdm     = false;
bool  have_channel  = false;
float snr_db        = 100.0f;
char* output_matlab = NULL;

void usage(char* prog)
{
  printf("Usage: %s [recovnm]\n", prog);

  printf("\t-r nof_prb [Default %d]\n", cell.base.nof_prb);
  printf("\t-e extended cyclic prefix [Default normal]\n");

  printf("\t-c n_id_ncell (1000 tests all). [Default %d]\n", cell.n_id_ncell);

  printf("\t-n Enable channel. [Default %s]\n", have_channel ? "Yes" : "No");
  printf("\t-s SNR in dB [Default %.1fdB]*\n", snr_db);
  printf("\t-m Enable OFDM. [Default %s]\n", have_ofdm ? "Yes" : "No");

  printf("\t-o output matlab file [Default %s]\n", output_matlab ? output_matlab : "None");
  printf("\t-v increase verbosity\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "recosvnm")) != -1) {
    switch (opt) {
      case 'r':
        cell.base.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        cell.n_id_ncell = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        have_channel = true;
        break;
      case 'm':
        have_ofdm = true;
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
  srslte_chest_dl_nbiot_t est   = {};
  int                     ret   = SRSLTE_ERROR;
  cf_t *                  input = NULL, *ce = NULL, *h = NULL, *output = NULL, *sf_buffer = NULL;

  parse_args(argc, argv);

  uint32_t num_re = 2 * cell.base.nof_prb * SRSLTE_NRE * SRSLTE_CP_NSYMB(cell.base.cp);

  input = srslte_vec_cf_malloc(num_re);
  if (!input) {
    perror("srslte_vec_malloc");
    goto do_exit;
  }
  output = srslte_vec_cf_malloc(num_re);
  if (!output) {
    perror("srslte_vec_malloc");
    goto do_exit;
  }
  sf_buffer = srslte_vec_cf_malloc(2U * SRSLTE_SLOT_LEN(srslte_symbol_sz(cell.base.nof_prb)));
  if (!sf_buffer) {
    perror("malloc");
    return -1;
  }
  h = srslte_vec_cf_malloc(num_re);
  if (!h) {
    perror("srslte_vec_malloc");
    goto do_exit;
  }
  ce = srslte_vec_cf_malloc(num_re);
  if (!ce) {
    perror("srslte_vec_malloc");
    goto do_exit;
  }
  for (int j = 0; j < num_re; j++) {
    ce[j] = 1;
  }

  if (SRSLTE_VERBOSE_ISDEBUG()) {
    DEBUG("SAVED FILE chest_start.bin: channel estimates start\n");
    srslte_vec_save_file("chest_start.bin", ce, num_re * sizeof(cf_t));
  }

  if (srslte_chest_dl_nbiot_init(&est, SRSLTE_NBIOT_MAX_PRB)) {
    fprintf(stderr, "Error initializing equalizer\n");
    goto do_exit;
  }
  if (srslte_chest_dl_nbiot_set_cell(&est, cell) != SRSLTE_SUCCESS) {
    fprintf(stderr, "Error setting channel estimator's cell configuration\n");
    return -1;
  }

  for (int sf_idx = 0; sf_idx < 1; sf_idx++) {
    for (int n_port = 0; n_port < cell.base.nof_ports; n_port++) {
      srslte_vec_cf_zero(input, num_re);
      for (int i = 0; i < num_re; i++) {
        input[i] = 0.5 - rand() / RAND_MAX + I * (0.5 - rand() / RAND_MAX);
      }

      srslte_vec_cf_zero(ce, num_re);
      srslte_vec_cf_zero(h, num_re);

      srslte_ofdm_t ifft, fft;
      if (have_ofdm) {
        if (srslte_ofdm_tx_init(&ifft, cell.base.cp, input, sf_buffer, cell.base.nof_prb)) {
          fprintf(stderr, "Error initializing IFFT\n");
          return -1;
        }
        if (srslte_ofdm_rx_init(&fft, cell.base.cp, sf_buffer, input, cell.base.nof_prb)) {
          fprintf(stderr, "Error initializing FFT\n");
          return -1;
        }
        srslte_ofdm_set_normalize(&ifft, true);
        srslte_ofdm_set_normalize(&fft, true);
      }

      srslte_refsignal_nrs_put_sf(cell, n_port, est.nrs_signal.pilots[0][0], input);

      if (have_channel) {
        // Add noise
        float std_dev = srslte_convert_dB_to_amplitude(-(snr_db + 3.0f)) * 0.1f;
        srslte_ch_awgn_c(est.pilot_recv_signal, est.pilot_recv_signal, std_dev, SRSLTE_REFSIGNAL_MAX_NUM_SF(1));
      }

      if (have_ofdm) {
        srslte_ofdm_tx_sf(&ifft);
        srslte_ofdm_rx_sf(&fft);
      }

      // check length of LTE CSR signal
      for (int i = 0; i < SRSLTE_NBIOT_MAX_PORTS; i++) {
        TESTASSERT(srslte_refsignal_nbiot_cs_nof_re(&cell, i) == 8);
      }

      srslte_chest_dl_nbiot_estimate_port(&est, input, ce, sf_idx, n_port);

      float rsrq  = srslte_chest_dl_nbiot_get_rsrq(&est);
      float rsrp  = srslte_chest_dl_nbiot_get_rsrp(&est);
      float noise = srslte_chest_dl_nbiot_get_noise_estimate(&est);
      float snr   = srslte_chest_dl_nbiot_get_snr(&est);
      DEBUG("rsrq=%4.2f, rsrp=%4.2f, noise=%4.2f, snr=%4.2f\n", rsrq, rsrp, noise, snr);

      if (SRSLTE_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE chest_final.bin: channel after estimation\n");
        srslte_vec_save_file("chest_final.bin", ce, num_re * sizeof(cf_t));
      }

      // use ZF equalizer
      srslte_predecoding_single(input, ce, output, NULL, num_re, 1.0, 0);

      if (!have_channel) {
        if (memcmp(est.nrs_signal.pilots[0][0], est.pilot_recv_signal, 8) == 0) {
          printf("ok\n");
        } else {
          printf("nok\n");
          goto do_exit;
        }
      }

      if (have_ofdm) {
        srslte_ofdm_tx_free(&ifft);
        srslte_ofdm_rx_free(&fft);
      }
    }
  }

  srslte_chest_dl_nbiot_free(&est);

  ret = SRSLTE_SUCCESS;

do_exit:

  if (output) {
    free(output);
  }
  if (ce) {
    free(ce);
  }
  if (input) {
    free(input);
  }
  if (h) {
    free(h);
  }
  if (sf_buffer) {
    free(sf_buffer);
  }

  return ret;
}
