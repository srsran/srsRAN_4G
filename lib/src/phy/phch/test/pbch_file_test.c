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

#include "srslte/srslte.h"

char* input_file_name = NULL;

srslte_cell_t cell = {
    6,                 // nof_prb
    2,                 // nof_ports
    150,               // cell_id
    SRSLTE_CP_NORM,    // cyclic prefix
    SRSLTE_PHICH_NORM, // PHICH length
    SRSLTE_PHICH_R_1,  // PHICH resources
    SRSLTE_FDD,

};

int nof_frames = 1;

uint8_t bch_payload_file[SRSLTE_BCH_PAYLOAD_LEN] = {0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1,
                                                    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#define FLEN (10 * SRSLTE_SF_LEN(srslte_symbol_sz(cell.nof_prb)))

srslte_filesource_t   fsrc;
cf_t *                input_buffer, *fft_buffer[SRSLTE_MAX_CODEWORDS];
srslte_pbch_t         pbch;
srslte_ofdm_t         fft;
srslte_chest_dl_t     chest;
srslte_chest_dl_res_t chest_res;

void usage(char* prog)
{
  printf("Usage: %s [vcoe] -i input_file\n", prog);
  printf("\t-c cell_id [Default %d]\n", cell.id);
  printf("\t-p nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-e Set extended prefix [Default Normal]\n");
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;

  while ((opt = getopt(argc, argv, "ivcpne")) != -1) {
    switch (opt) {
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        nof_frames = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      case 'e':
        cell.cp = SRSLTE_CP_EXT;
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

  if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
    ERROR("Error opening file %s\n", input_file_name);
    exit(-1);
  }

  input_buffer = srslte_vec_cf_malloc(FLEN);
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }

  fft_buffer[0] = srslte_vec_cf_malloc(SRSLTE_NOF_RE(cell));
  if (!fft_buffer[0]) {
    perror("malloc");
    return -1;
  }

  if (!srslte_cell_isvalid(&cell)) {
    ERROR("Invalid cell properties\n");
    return -1;
  }

  if (srslte_chest_dl_init(&chest, cell.nof_prb, 1)) {
    ERROR("Error initializing equalizer\n");
    return -1;
  }
  if (srslte_chest_dl_res_init(&chest_res, cell.nof_prb)) {
    ERROR("Error initializing equalizer\n");
    return -1;
  }
  if (srslte_chest_dl_set_cell(&chest, cell)) {
    ERROR("Error initializing equalizer\n");
    return -1;
  }

  if (srslte_ofdm_rx_init(&fft, cell.cp, input_buffer, fft_buffer[0], cell.nof_prb)) {
    ERROR("Error initializing FFT\n");
    return -1;
  }

  if (srslte_pbch_init(&pbch)) {
    ERROR("Error initiating PBCH\n");
    return -1;
  }
  if (srslte_pbch_set_cell(&pbch, cell)) {
    ERROR("Error initiating PBCH\n");
    return -1;
  }

  DEBUG("Memory init OK\n");
  return 0;
}

void base_free()
{
  srslte_filesource_free(&fsrc);

  free(input_buffer);
  free(fft_buffer[0]);

  srslte_filesource_free(&fsrc);
  srslte_chest_dl_res_free(&chest_res);
  srslte_chest_dl_free(&chest);
  srslte_ofdm_rx_free(&fft);

  srslte_pbch_free(&pbch);
}

int main(int argc, char** argv)
{
  uint8_t  bch_payload[SRSLTE_BCH_PAYLOAD_LEN];
  int      n;
  uint32_t nof_tx_ports;
  int      sfn_offset;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc, argv);

  if (base_init()) {
    ERROR("Error initializing receiver\n");
    exit(-1);
  }

  int frame_cnt        = 0;
  int nof_decoded_mibs = 0;
  int nread            = 0;
  do {
    nread = srslte_filesource_read(&fsrc, input_buffer, FLEN);

    if (nread > 0) {
      // process 1st subframe only
      srslte_ofdm_rx_sf(&fft);

      srslte_dl_sf_cfg_t dl_sf;
      ZERO_OBJECT(dl_sf);

      /* Get channel estimates for each port */
      srslte_chest_dl_estimate(&chest, &dl_sf, fft_buffer, &chest_res);

      INFO("Decoding PBCH\n");

      srslte_pbch_decode_reset(&pbch);
      n = srslte_pbch_decode(&pbch, &chest_res, fft_buffer, bch_payload, &nof_tx_ports, &sfn_offset);

      if (n == 1) {
        nof_decoded_mibs++;
      } else if (n < 0) {
        ERROR("Error decoding PBCH\n");
        exit(-1);
      }
      frame_cnt++;
    } else if (nread < 0) {
      ERROR("Error reading from file\n");
      exit(-1);
    }
  } while (nread > 0 && frame_cnt < nof_frames);

  base_free();

  if (frame_cnt == 1) {
    if (n == 0) {
      printf("Could not decode PBCH\n");
      exit(-1);
    } else {
      printf("MIB decoded OK. Nof ports: %d. SFN offset: %d Payload: ", nof_tx_ports, sfn_offset);
      srslte_vec_fprint_hex(stdout, bch_payload, SRSLTE_BCH_PAYLOAD_LEN);
      if (nof_tx_ports == 2 && sfn_offset == 0 && !memcmp(bch_payload, bch_payload_file, SRSLTE_BCH_PAYLOAD_LEN)) {
        printf("This is the signal.1.92M.dat file\n");
        exit(0);
      } else {
        printf("This is an unknown file\n");
        exit(-1);
      }
    }
  } else {
    printf("Decoded %d/%d MIBs\n", nof_decoded_mibs, frame_cnt);
  }
}
