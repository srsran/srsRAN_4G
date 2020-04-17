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

char* input_file_name  = NULL;
char* matlab_file_name = NULL;

srslte_cell_t cell = {
    50,                // cell.nof_prb
    2,                 // cell.nof_ports
    150,               // cell.id
    SRSLTE_CP_NORM,    // cyclic prefix
    SRSLTE_PHICH_NORM, // PHICH length
    SRSLTE_PHICH_R_1,  // PHICH resources
    SRSLTE_FDD,

};

int flen;
int nof_ctrl_symbols = 1;
int numsubframe      = 0;

FILE* fmatlab = NULL;

srslte_filesource_t   fsrc;
cf_t *                input_buffer, *fft_buffer[SRSLTE_MAX_CODEWORDS];
srslte_phich_t        phich;
srslte_regs_t         regs;
srslte_ofdm_t         fft;
srslte_chest_dl_t     chest;
srslte_chest_dl_res_t chest_res;

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
  printf("\t-v [set srslte_verbose to debug, default none]\n");
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
          cell.phich_resources = SRSLTE_PHICH_R_1_6;
        } else if (!strcmp(argv[optind], "1/2")) {
          cell.phich_resources = SRSLTE_PHICH_R_1_2;
        } else if (!strcmp(argv[optind], "1")) {
          cell.phich_resources = SRSLTE_PHICH_R_1;
        } else if (!strcmp(argv[optind], "2")) {
          cell.phich_resources = SRSLTE_PHICH_R_2;
        } else {
          ERROR("Invalid phich ng factor %s. Setting to default.\n", argv[optind]);
        }
        break;
      case 'e':
        cell.phich_length = SRSLTE_PHICH_EXT;
        break;
      case 'n':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        cell.nof_ports = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      case 'l':
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

  if (matlab_file_name) {
    fmatlab = fopen(matlab_file_name, "w");
    if (!fmatlab) {
      perror("fopen");
      return -1;
    }
  } else {
    fmatlab = NULL;
  }

  flen = SRSLTE_SF_LEN(srslte_symbol_sz(cell.nof_prb));

  input_buffer = srslte_vec_cf_malloc(flen);
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }

  fft_buffer[0] = srslte_vec_cf_malloc(SRSLTE_NOF_RE(cell));
  if (!fft_buffer[0]) {
    perror("malloc");
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

  if (srslte_regs_init(&regs, cell)) {
    ERROR("Error initiating regs\n");
    return -1;
  }

  if (srslte_phich_init(&phich, 1)) {
    ERROR("Error creating PBCH object\n");
    return -1;
  }
  if (srslte_phich_set_cell(&phich, &regs, cell)) {
    ERROR("Error creating PBCH object\n");
    return -1;
  }

  DEBUG("Memory init OK\n");
  return 0;
}

void base_free()
{

  srslte_filesource_free(&fsrc);
  if (fmatlab) {
    fclose(fmatlab);
  }

  free(input_buffer);
  free(fft_buffer[0]);

  srslte_filesource_free(&fsrc);

  srslte_chest_dl_res_free(&chest_res);
  srslte_chest_dl_free(&chest);
  srslte_ofdm_rx_free(&fft);

  srslte_phich_free(&phich);
  srslte_regs_free(&regs);
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

  max_nseq = SRSLTE_CP_ISNORM(cell.cp) ? SRSLTE_PHICH_NORM_NSEQUENCES : SRSLTE_PHICH_EXT_NSEQUENCES;

  if (base_init()) {
    ERROR("Error initializing memory\n");
    exit(-1);
  }

  n = srslte_filesource_read(&fsrc, input_buffer, flen);

  srslte_ofdm_rx_sf(&fft);

  if (fmatlab) {
    fprintf(fmatlab, "infft=");
    srslte_vec_fprint_c(fmatlab, input_buffer, flen);
    fprintf(fmatlab, ";\n");

    fprintf(fmatlab, "outfft=");
    srslte_vec_fprint_c(fmatlab, fft_buffer[0], SRSLTE_CP_NSYMB(cell.cp) * cell.nof_prb * SRSLTE_NRE);
    fprintf(fmatlab, ";\n");
  }

  srslte_dl_sf_cfg_t dl_sf;
  ZERO_OBJECT(dl_sf);
  dl_sf.tti = numsubframe;

  /* Get channel estimates for each port */
  srslte_chest_dl_estimate(&chest, &dl_sf, fft_buffer, &chest_res);

  INFO("Decoding PHICH\n");

  /* Receive all PHICH groups and sequence numbers */
  for (ngroup = 0; ngroup < srslte_phich_ngroups(&phich); ngroup++) {
    for (nseq = 0; nseq < max_nseq; nseq++) {

      srslte_phich_resource_t resource;
      resource.ngroup = ngroup;
      resource.nseq   = nseq;

      srslte_phich_res_t res;

      if (srslte_phich_decode(&phich, &dl_sf, &chest_res, resource, fft_buffer, &res) < 0) {
        printf("Error decoding ACK\n");
        exit(-1);
      }

      INFO("%d/%d, ack_rx: %d, ns: %d, distance: %f\n", ngroup, nseq, res.ack_value, numsubframe, res.distance);
    }
  }

  base_free();

  if (n < 0) {
    ERROR("Error decoding phich\n");
    exit(-1);
  } else if (n == 0) {
    printf("Could not decode phich\n");
    exit(-1);
  } else {
    exit(0);
  }
}
