/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "liblte/phy/phy.h"

char *input_file_name = NULL;
char *matlab_file_name = NULL;


lte_cell_t cell = {
  6,            // nof_prb
  1,            // nof_ports
  0,            // cell_id
  CPNORM        // cyclic prefix
};

int flen;

FILE *fmatlab = NULL;

filesource_t fsrc;
cf_t *input_buffer, *fft_buffer, *ce[MAX_PORTS];
pcfich_t pcfich;
regs_t regs;
lte_fft_t fft;
chest_t chest;

void usage(char *prog) {
  printf("Usage: %s [vcoe] -i input_file\n", prog);
  printf("\t-o output matlab file name [Default Disabled]\n");
  printf("\t-c cell.id [Default %d]\n", cell.id);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-e Set extended prefix [Default Normal]\n");
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "iovcenp")) != -1) {
    switch(opt) {
    case 'i':
      input_file_name = argv[optind];
      break;
    case 'c':
      cell.id = atoi(argv[optind]);
      break;
    case 'n':
      cell.nof_prb = atoi(argv[optind]);
      break;
    case 'p':
      cell.nof_ports = atoi(argv[optind]);
      break;
    case 'o':
      matlab_file_name = argv[optind];
      break;
    case 'v':
      verbose++;
      break;
    case 'e':
      cell.cp = CPEXT;
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

int base_init() {
  int i;
  
  if (filesource_init(&fsrc, input_file_name, COMPLEX_FLOAT_BIN)) {
    fprintf(stderr, "Error opening file %s\n", input_file_name);
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

  flen = SLOT_LEN(lte_symbol_sz(cell.nof_prb), cell.cp);

  input_buffer = malloc(flen * sizeof(cf_t));
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }

  fft_buffer = malloc(CP_NSYMB(cell.cp) * cell.nof_prb * RE_X_RB * sizeof(cf_t));
  if (!fft_buffer) {
    perror("malloc");
    return -1;
  }

  for (i=0;i<MAX_PORTS;i++) {
    ce[i] = malloc(CP_NSYMB(cell.cp) * cell.nof_prb * RE_X_RB * sizeof(cf_t));
    if (!ce[i]) {
      perror("malloc");
      return -1;
    }
  }
  
  if (chest_init_LTEDL(&chest, cell)) {
    fprintf(stderr, "Error initializing equalizer\n");
    return -1;
  }

  if (lte_fft_init(&fft, cell.cp, cell.nof_prb)) {
    fprintf(stderr, "Error initializing FFT\n");
    return -1;
  }

  if (regs_init(&regs, R_1, PHICH_NORM, cell)) {
    fprintf(stderr, "Error initiating REGs\n");
    return -1;
  }

  if (pcfich_init(&pcfich, &regs, cell)) {
    fprintf(stderr, "Error creating PBCH object\n");
    return -1;
  }

  DEBUG("Memory init OK\n",0);
  return 0;
}

void base_free() {
  int i;

  filesource_free(&fsrc);
  if (fmatlab) {
    fclose(fmatlab);
  }

  free(input_buffer);
  free(fft_buffer);

  filesource_free(&fsrc);
  for (i=0;i<MAX_PORTS;i++) {
    free(ce[i]);
  }
  chest_free(&chest);
  lte_fft_free(&fft);

  pcfich_free(&pcfich);
  regs_free(&regs);
}

int main(int argc, char **argv) {
  uint32_t cfi, distance;
  int i, n;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc,argv);

  if (base_init()) {
    fprintf(stderr, "Error initializing receiver\n");
    exit(-1);
  }

  n = filesource_read(&fsrc, input_buffer, flen);

  lte_fft_run_slot(&fft, input_buffer, fft_buffer);

  if (fmatlab) {
    fprintf(fmatlab, "infft=");
    vec_fprint_c(fmatlab, input_buffer, flen);
    fprintf(fmatlab, ";\n");

    fprintf(fmatlab, "outfft=");
    vec_sc_prod_cfc(fft_buffer, 1000.0, fft_buffer, CP_NSYMB(cell.cp) * cell.nof_prb * RE_X_RB);
    vec_fprint_c(fmatlab, fft_buffer, CP_NSYMB(cell.cp) * cell.nof_prb * RE_X_RB);
    fprintf(fmatlab, ";\n");
    vec_sc_prod_cfc(fft_buffer, 0.001, fft_buffer,   CP_NSYMB(cell.cp) * cell.nof_prb * RE_X_RB);
  }

  /* Get channel estimates for each port */
  for (i=0;i<cell.nof_ports;i++) {
    chest_ce_slot_port(&chest, fft_buffer, ce[i], 0, i);
    if (fmatlab) {
      chest_fprint(&chest, fmatlab, 0, i);
    }
  }

  INFO("Decoding PCFICH\n", 0);

  n = pcfich_decode(&pcfich, fft_buffer, ce, 0, &cfi, &distance);
  printf("cfi: %d, distance: %d\n", cfi, distance);

  base_free();

  if (n < 0) {
    fprintf(stderr, "Error decoding PCFICH\n");
    exit(-1);
  } else if (n == 0) {
    printf("Could not decode PCFICH\n");
    exit(-1);
  } else {
    if (distance < 4 && cfi == 1) {
      exit(0);
    } else {
      exit(-1);
    }
  }
}
