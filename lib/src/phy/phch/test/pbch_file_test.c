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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "srsran/srsran.h"

char* input_file_name = NULL;

srsran_cell_t cell = {
    6,                 // nof_prb
    2,                 // nof_ports
    150,               // cell_id
    SRSRAN_CP_NORM,    // cyclic prefix
    SRSRAN_PHICH_NORM, // PHICH length
    SRSRAN_PHICH_R_1,  // PHICH resources
    SRSRAN_FDD,

};

int nof_frames = 1;

uint8_t bch_payload_file[SRSRAN_BCH_PAYLOAD_LEN] = {0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1,
                                                    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#define FLEN (10 * SRSRAN_SF_LEN(srsran_symbol_sz(cell.nof_prb)))

srsran_filesource_t   fsrc;
cf_t *                input_buffer, *fft_buffer[SRSRAN_MAX_PORTS];
srsran_pbch_t         pbch;
srsran_ofdm_t         fft;
srsran_chest_dl_t     chest;
srsran_chest_dl_res_t chest_res;

void usage(char* prog)
{
  printf("Usage: %s [vcoe] -i input_file\n", prog);
  printf("\t-c cell_id [Default %d]\n", cell.id);
  printf("\t-p nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-e Set extended prefix [Default Normal]\n");
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-v [set srsran_verbose to debug, default none]\n");
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
        increase_srsran_verbose_level();
        break;
      case 'e':
        cell.cp = SRSRAN_CP_EXT;
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
  if (srsran_filesource_init(&fsrc, input_file_name, SRSRAN_COMPLEX_FLOAT_BIN)) {
    ERROR("Error opening file %s", input_file_name);
    exit(-1);
  }

  input_buffer = srsran_vec_cf_malloc(FLEN);
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }

  fft_buffer[0] = srsran_vec_cf_malloc(SRSRAN_NOF_RE(cell));
  if (!fft_buffer[0]) {
    perror("malloc");
    return -1;
  }

  if (!srsran_cell_isvalid(&cell)) {
    ERROR("Invalid cell properties");
    return -1;
  }

  if (srsran_chest_dl_init(&chest, cell.nof_prb, 1)) {
    ERROR("Error initializing equalizer");
    return -1;
  }
  if (srsran_chest_dl_res_init(&chest_res, cell.nof_prb)) {
    ERROR("Error initializing equalizer");
    return -1;
  }
  if (srsran_chest_dl_set_cell(&chest, cell)) {
    ERROR("Error initializing equalizer");
    return -1;
  }

  if (srsran_ofdm_rx_init(&fft, cell.cp, input_buffer, fft_buffer[0], cell.nof_prb)) {
    ERROR("Error initializing FFT");
    return -1;
  }

  if (srsran_pbch_init(&pbch)) {
    ERROR("Error initiating PBCH");
    return -1;
  }
  if (srsran_pbch_set_cell(&pbch, cell)) {
    ERROR("Error initiating PBCH");
    return -1;
  }

  DEBUG("Memory init OK");
  return 0;
}

void base_free()
{
  srsran_filesource_free(&fsrc);

  free(input_buffer);
  free(fft_buffer[0]);

  srsran_filesource_free(&fsrc);
  srsran_chest_dl_res_free(&chest_res);
  srsran_chest_dl_free(&chest);
  srsran_ofdm_rx_free(&fft);

  srsran_pbch_free(&pbch);
}

int main(int argc, char** argv)
{
  uint8_t  bch_payload[SRSRAN_BCH_PAYLOAD_LEN];
  int      n;
  uint32_t nof_tx_ports;
  int      sfn_offset;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc, argv);

  if (base_init()) {
    ERROR("Error initializing receiver");
    exit(-1);
  }

  int frame_cnt        = 0;
  int nof_decoded_mibs = 0;
  int nread            = 0;
  do {
    nread = srsran_filesource_read(&fsrc, input_buffer, FLEN);

    if (nread > 0) {
      // process 1st subframe only
      srsran_ofdm_rx_sf(&fft);

      srsran_dl_sf_cfg_t dl_sf;
      ZERO_OBJECT(dl_sf);

      /* Get channel estimates for each port */
      srsran_chest_dl_estimate(&chest, &dl_sf, fft_buffer, &chest_res);

      INFO("Decoding PBCH");

      srsran_pbch_decode_reset(&pbch);
      n = srsran_pbch_decode(&pbch, &chest_res, fft_buffer, bch_payload, &nof_tx_ports, &sfn_offset);

      if (n == 1) {
        nof_decoded_mibs++;
      } else if (n < 0) {
        ERROR("Error decoding PBCH");
        exit(-1);
      }
      frame_cnt++;
    } else if (nread < 0) {
      ERROR("Error reading from file");
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
      srsran_vec_fprint_hex(stdout, bch_payload, SRSRAN_BCH_PAYLOAD_LEN);
      if (nof_tx_ports == 2 && sfn_offset == 0 && !memcmp(bch_payload, bch_payload_file, SRSRAN_BCH_PAYLOAD_LEN)) {
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
