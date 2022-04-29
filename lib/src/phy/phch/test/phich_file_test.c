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

char* input_file_name  = NULL;
char* matlab_file_name = NULL;

srsran_cell_t cell = {
    50,                // cell.nof_prb
    2,                 // cell.nof_ports
    150,               // cell.id
    SRSRAN_CP_NORM,    // cyclic prefix
    SRSRAN_PHICH_NORM, // PHICH length
    SRSRAN_PHICH_R_1,  // PHICH resources
    SRSRAN_FDD,

};

int flen;
int nof_ctrl_symbols = 1;
int numsubframe      = 0;

FILE* fmatlab = NULL;

srsran_filesource_t   fsrc;
cf_t *                input_buffer, *fft_buffer[SRSRAN_MAX_PORTS];
srsran_phich_t        phich;
srsran_regs_t         regs;
srsran_ofdm_t         fft;
srsran_chest_dl_t     chest;
srsran_chest_dl_res_t chest_res;

void usage(char* prog)
{
  printf("Usage: %s [vcoe] -i input_file\n", prog);
  printf("\t-o output matlab file name [Default Disabled]\n");
  printf("\t-c cell.id [Default %d]\n", cell.id);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-f nof control symbols [Default %d]\n", nof_ctrl_symbols);
  printf("\t-g phich ng factor: 1/6, 1/2, 1, 2 [Default 1]\n");
  printf("\t-e phich extended length [Default normal]\n");
  printf("\t-l extended cyclic prefix [Default normal]\n");
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "iovcenpfgl")) != -1) {
    switch (opt) {
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'o':
        matlab_file_name = argv[optind];
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        nof_ctrl_symbols = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'g':
        if (!strcmp(argv[optind], "1/6")) {
          cell.phich_resources = SRSRAN_PHICH_R_1_6;
        } else if (!strcmp(argv[optind], "1/2")) {
          cell.phich_resources = SRSRAN_PHICH_R_1_2;
        } else if (!strcmp(argv[optind], "1")) {
          cell.phich_resources = SRSRAN_PHICH_R_1;
        } else if (!strcmp(argv[optind], "2")) {
          cell.phich_resources = SRSRAN_PHICH_R_2;
        } else {
          ERROR("Invalid phich ng factor %s. Setting to default.", argv[optind]);
        }
        break;
      case 'e':
        cell.phich_length = SRSRAN_PHICH_EXT;
        break;
      case 'n':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        cell.nof_ports = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        increase_srsran_verbose_level();
        break;
      case 'l':
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

  if (matlab_file_name) {
    fmatlab = fopen(matlab_file_name, "w");
    if (!fmatlab) {
      perror("fopen");
      return -1;
    }
  } else {
    fmatlab = NULL;
  }

  flen = SRSRAN_SF_LEN(srsran_symbol_sz(cell.nof_prb));

  input_buffer = srsran_vec_cf_malloc(flen);
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }

  fft_buffer[0] = srsran_vec_cf_malloc(SRSRAN_NOF_RE(cell));
  if (!fft_buffer[0]) {
    perror("malloc");
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

  if (srsran_regs_init(&regs, cell)) {
    ERROR("Error initiating regs");
    return -1;
  }

  if (srsran_phich_init(&phich, 1)) {
    ERROR("Error creating PBCH object");
    return -1;
  }
  if (srsran_phich_set_cell(&phich, &regs, cell)) {
    ERROR("Error creating PBCH object");
    return -1;
  }

  DEBUG("Memory init OK");
  return 0;
}

void base_free()
{
  srsran_filesource_free(&fsrc);
  if (fmatlab) {
    fclose(fmatlab);
  }

  free(input_buffer);
  free(fft_buffer[0]);

  srsran_filesource_free(&fsrc);

  srsran_chest_dl_res_free(&chest_res);
  srsran_chest_dl_free(&chest);
  srsran_ofdm_rx_free(&fft);

  srsran_phich_free(&phich);
  srsran_regs_free(&regs);
}

int main(int argc, char** argv)
{
  int      n;
  uint32_t ngroup, nseq, max_nseq;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc, argv);

  max_nseq = SRSRAN_CP_ISNORM(cell.cp) ? SRSRAN_PHICH_NORM_NSEQUENCES : SRSRAN_PHICH_EXT_NSEQUENCES;

  if (base_init()) {
    ERROR("Error initializing memory");
    exit(-1);
  }

  n = srsran_filesource_read(&fsrc, input_buffer, flen);

  srsran_ofdm_rx_sf(&fft);

  if (fmatlab) {
    fprintf(fmatlab, "infft=");
    srsran_vec_fprint_c(fmatlab, input_buffer, flen);
    fprintf(fmatlab, ";\n");

    fprintf(fmatlab, "outfft=");
    srsran_vec_fprint_c(fmatlab, fft_buffer[0], SRSRAN_CP_NSYMB(cell.cp) * cell.nof_prb * SRSRAN_NRE);
    fprintf(fmatlab, ";\n");
  }

  srsran_dl_sf_cfg_t dl_sf;
  ZERO_OBJECT(dl_sf);
  dl_sf.tti = numsubframe;

  /* Get channel estimates for each port */
  srsran_chest_dl_estimate(&chest, &dl_sf, fft_buffer, &chest_res);

  INFO("Decoding PHICH");

  /* Receive all PHICH groups and sequence numbers */
  for (ngroup = 0; ngroup < srsran_phich_ngroups(&phich); ngroup++) {
    for (nseq = 0; nseq < max_nseq; nseq++) {
      srsran_phich_resource_t resource;
      resource.ngroup = ngroup;
      resource.nseq   = nseq;

      srsran_phich_res_t res;

      if (srsran_phich_decode(&phich, &dl_sf, &chest_res, resource, fft_buffer, &res) < 0) {
        printf("Error decoding ACK\n");
        exit(-1);
      }

      INFO("%d/%d, ack_rx: %d, ns: %d, distance: %f", ngroup, nseq, res.ack_value, numsubframe, res.distance);
    }
  }

  base_free();

  if (n < 0) {
    ERROR("Error decoding phich");
    exit(-1);
  } else if (n == 0) {
    printf("Could not decode phich\n");
    exit(-1);
  } else {
    exit(0);
  }
}
