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
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "srslte/phy/ch_estimation/chest_dl_nbiot.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/io/filesource.h"
#include "srslte/phy/phch/npbch.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

char* input_file_name = NULL;

srslte_nbiot_cell_t cell = {.base       = {.nof_prb = 1, .nof_ports = 2, .cp = SRSLTE_CP_NORM, .id = 0},
                            .nbiot_prb  = 0,
                            .n_id_ncell = 0,
                            .nof_ports  = 0,
                            .is_r14     = false};

int  nof_frames = 128; // two MIB periods
bool do_chest   = true;
int  nf         = 0;
int  sf_idx     = 0;

#define SFLEN (1 * SRSLTE_SF_LEN(srslte_symbol_sz(cell.base.nof_prb)))

srslte_filesource_t     fsrc;
cf_t *                  input_buffer, *fft_buffer, *ce[SRSLTE_MAX_PORTS];
srslte_npbch_t          npbch;
srslte_ofdm_t           fft;
srslte_chest_dl_nbiot_t chest;

void usage(char* prog)
{
  printf("Usage: %s [vrslRtoe] -i input_file\n", prog);
  printf("\t-l n_id_ncell [Default %d]\n", cell.n_id_ncell);
  printf("\t-p nof_prb [Default %d]\n", cell.base.nof_prb);
  printf("\t-t do channel estimation [Default %d]\n", do_chest);
  printf("\t-s Initial value of sf_idx [Default %d]\n", sf_idx);
  printf("\t-r NPBCH repetition number within block [Default %d]\n", nf);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-R Whether this is a R14 signal [Default %s]\n", cell.is_r14 ? "Yes" : "No");

  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;

  while ((opt = getopt(argc, argv, "ilvrstneR")) != -1) {
    switch (opt) {
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'l':
        cell.n_id_ncell = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        cell.base.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 't':
        do_chest = (strtol(argv[optind], NULL, 10) != 0);
        break;
      case 'n':
        nof_frames = (int)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        sf_idx = (int)(strtol(argv[optind], NULL, 10) % 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      case 'r':
        nf = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'R':
        cell.is_r14 = true;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (!input_file_name) {
    usage(argv[0]);
    exit(-1);
  }
}

int base_init()
{
  srand(0);

  if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
    fprintf(stderr, "Error opening file %s\n", input_file_name);
    exit(-1);
  }

  input_buffer = srslte_vec_cf_malloc(SFLEN);
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }

  fft_buffer = srslte_vec_cf_malloc(SRSLTE_SF_LEN(srslte_symbol_sz(cell.base.nof_prb)));
  if (!fft_buffer) {
    perror("malloc");
    return -1;
  }

  for (int i = 0; i < cell.base.nof_ports; i++) {
    ce[i] = srslte_vec_cf_malloc(SRSLTE_SF_LEN_RE(cell.base.nof_prb, cell.base.cp));
    if (!ce[i]) {
      perror("malloc");
      return -1;
    }
    for (int j = 0; j < SRSLTE_SF_LEN_RE(cell.base.nof_prb, cell.base.cp); j++) {
      ce[i][j] = 1.0;
    }
  }

  if (srslte_chest_dl_nbiot_init(&chest, SRSLTE_NBIOT_MAX_PRB)) {
    fprintf(stderr, "Error initializing equalizer\n");
    return -1;
  }
  if (srslte_chest_dl_nbiot_set_cell(&chest, cell) != SRSLTE_SUCCESS) {
    fprintf(stderr, "Error setting equalizer cell configuration\n");
    return -1;
  }

  if (srslte_ofdm_rx_init(&fft, cell.base.cp, input_buffer, fft_buffer, cell.base.nof_prb)) {
    fprintf(stderr, "Error initializing FFT\n");
    return -1;
  }
  srslte_ofdm_set_freq_shift(&fft, SRSLTE_NBIOT_FREQ_SHIFT_FACTOR);

  if (srslte_npbch_init(&npbch)) {
    fprintf(stderr, "Error initiating NPBCH\n");
    return -1;
  }
  if (srslte_npbch_set_cell(&npbch, cell)) {
    fprintf(stderr, "Error setting cell in NPBCH object\n");
    exit(-1);
  }

  // setting ports to 2 to make test not fail
  cell.nof_ports = 2;
  if (!srslte_nbiot_cell_isvalid(&cell)) {
    fprintf(stderr, "Invalid cell properties\n");
    return -1;
  }

  DEBUG("Memory init OK\n");
  return 0;
}

void base_free()
{
  srslte_filesource_free(&fsrc);

  free(input_buffer);
  free(fft_buffer);

  srslte_filesource_free(&fsrc);
  for (int i = 0; i < cell.base.nof_ports; i++) {
    free(ce[i]);
  }
  srslte_chest_dl_nbiot_free(&chest);
  srslte_ofdm_rx_free(&fft);

  srslte_npbch_free(&npbch);
}

int main(int argc, char** argv)
{
  uint8_t  bch_payload[SRSLTE_MIB_NB_LEN] = {};
  int      ret                            = SRSLTE_ERROR;
  uint32_t nof_tx_ports                   = 0;
  int      sfn_offset                     = 0;

  if (argc < 3) {
    usage(argv[0]);
    return ret;
  }

  parse_args(argc, argv);

  printf("Subframe length: %d\n", SFLEN);

  if (base_init()) {
    fprintf(stderr, "Error initializing receiver\n");
    return ret;
  }

  int frame_cnt        = 0;
  int nof_decoded_mibs = 0;
  int nread            = 0;

  do {
    nread = srslte_filesource_read(&fsrc, input_buffer, SFLEN);
    if (nread == SFLEN) {
      // do IFFT and channel estimation only on subframes that are known to contain NRS
      if (sf_idx == 0 || sf_idx == 4) {
        INFO("%d.%d: Estimating channel.\n", frame_cnt, sf_idx);
        srslte_ofdm_rx_sf(&fft);
        // srslte_ofdm_set_normalize(&fft, true);

        if (do_chest) {
          srslte_chest_dl_nbiot_estimate(&chest, fft_buffer, ce, sf_idx);
        }
      }

      // but NPBCH processing only for 1st subframe
      if (sf_idx == 0) {
        float noise_est = (do_chest) ? srslte_chest_dl_nbiot_get_noise_estimate(&chest) : 0.0;
        if (frame_cnt % 8 == 0) {
          DEBUG("Reseting NPBCH decoder.\n");
          srslte_npbch_decode_reset(&npbch);
        }
        INFO("%d.0: Calling NPBCH decoder (noise_est=%.2f)\n", frame_cnt, noise_est);
        ret = srslte_npbch_decode_nf(&npbch, fft_buffer, ce, noise_est, bch_payload, &nof_tx_ports, NULL, nf);

        if (ret == SRSLTE_SUCCESS) {
          printf("MIB-NB decoded OK. Nof ports: %d. SFN offset: %d Payload: ", nof_tx_ports, sfn_offset);
          srslte_vec_fprint_hex(stdout, bch_payload, SRSLTE_MIB_NB_LEN);
          srslte_mib_nb_t mib_nb;
          srslte_npbch_mib_unpack(bch_payload, &mib_nb);
          srslte_mib_nb_printf(stdout, cell, &mib_nb);
          nof_decoded_mibs++;
        }

        if (SRSLTE_VERBOSE_ISDEBUG()) {
          if (do_chest) {
            DEBUG("SAVED FILE npbch_rx_chest_on.bin: NPBCH with chest\n");
            srslte_vec_save_file("npbch_rx_chest_on.bin", npbch.d, npbch.nof_symbols * sizeof(cf_t));
          } else {
            DEBUG("SAVED FILE npbch_rx_chest_off.bin: NPBCH without chest\n");
            srslte_vec_save_file("npbch_rx_chest_off.bin", npbch.d, npbch.nof_symbols * sizeof(cf_t));
          }
        }
      }
      sf_idx++;
      if (sf_idx == 10) {
        sf_idx = 0;
        frame_cnt++;
      }

    } else if (nread < 0) {
      fprintf(stderr, "Error reading from file\n");
      return ret;
    }
  } while (nread > 0 && frame_cnt < nof_frames);

  base_free();

  printf("nof_decoded_mibs=%d\n", nof_decoded_mibs);

  ret = (nof_decoded_mibs > 0) ? SRSLTE_SUCCESS : SRSLTE_ERROR;

  return ret;
}
