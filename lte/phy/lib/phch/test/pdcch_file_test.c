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
int cell_id = 0;
int cfi = 2;
lte_cp_t cp = CPNORM;
int nof_prb = 6;
int nof_ports = 1;
int flen;
unsigned short rnti = SIRNTI;
int max_frames = 10;
FILE *fmatlab = NULL;

filesource_t fsrc;
pdcch_t pdcch;
cf_t *input_buffer, *fft_buffer, *ce[MAX_PORTS];
regs_t regs;
lte_fft_t fft;
chest_t chest;
dci_t dci_rx;

void usage(char *prog) {
  printf("Usage: %s [vcfoe] -i input_file\n", prog);
  printf("\t-o output matlab file name [Default Disabled]\n");
  printf("\t-c cell_id [Default %d]\n", cell_id);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-r rnti [Default SI-RNTI]\n");
  printf("\t-p nof_ports [Default %d]\n", nof_ports);
  printf("\t-n nof_prb [Default %d]\n", nof_prb);
  printf("\t-m max_frames [Default %d]\n", max_frames);
  printf("\t-e Set extended prefix [Default Normal]\n");
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "irovfcenmp")) != -1) {
    switch(opt) {
    case 'i':
      input_file_name = argv[optind];
      break;
    case 'c':
      cell_id = atoi(argv[optind]);
      break;
    case 'r':
      rnti = strtoul(argv[optind], NULL, 0);
      break;
    case 'm':
      max_frames = strtoul(argv[optind], NULL, 0);
      break;
    case 'f':
      cfi = atoi(argv[optind]);
      break;
    case 'n':
      nof_prb = atoi(argv[optind]);
      break;
    case 'p':
      nof_ports = atoi(argv[optind]);
      break;
    case 'o':
      matlab_file_name = argv[optind];
      break;
    case 'v':
      verbose++;
      break;
    case 'e':
      cp = CPEXT;
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

  flen = 2 * (SLOT_LEN(lte_symbol_sz(nof_prb), cp));

  input_buffer = malloc(flen * sizeof(cf_t));
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }

  fft_buffer = malloc(CP_NSYMB(cp) * nof_prb * RE_X_RB * sizeof(cf_t));
  if (!fft_buffer) {
    perror("malloc");
    return -1;
  }

  for (i=0;i<MAX_PORTS;i++) {
    ce[i] = malloc(CP_NSYMB(cp) * nof_prb * RE_X_RB * sizeof(cf_t));
    if (!ce[i]) {
      perror("malloc");
      return -1;
    }
  }

  if (chest_init(&chest, LINEAR, cp, nof_prb, nof_ports)) {
    fprintf(stderr, "Error initializing equalizer\n");
    return -1;
  }

  if (chest_ref_LTEDL(&chest, cell_id)) {
    fprintf(stderr, "Error initializing reference signal\n");
    return -1;
  }

  if (lte_fft_init(&fft, cp, nof_prb)) {
    fprintf(stderr, "Error initializing FFT\n");
    return -1;
  }

  if (regs_init(&regs, cell_id, nof_prb, nof_ports, R_1, PHICH_NORM, cp)) {
    fprintf(stderr, "Error initiating regs\n");
    return -1;
  }

  if (regs_set_cfi(&regs, cfi)) {
    fprintf(stderr, "Error setting CFI %d\n", cfi);
    return -1;
  }
  if (pdcch_init(&pdcch, &regs, nof_prb, nof_ports, cell_id, cp)) {
    fprintf(stderr, "Error creating PDCCH object\n");
    exit(-1);
  }
  if (pdcch_set_cfi(&pdcch, cfi)) {
    fprintf(stderr, "Error setting CFI %d\n", cfi);
    return -1;    
  }

  dci_init(&dci_rx, 10);

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

  dci_free(&dci_rx);
  pdcch_free(&pdcch);
  regs_free(&regs);
}

int main(int argc, char **argv) {
  ra_pdsch_t ra_dl;
  int i;
  int nof_dcis;
  int nof_frames;
  int ret;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc,argv);

  if (base_init()) {
    fprintf(stderr, "Error initializing memory\n");
    exit(-1);
  }

  if (rnti == SIRNTI) {
    INFO("Initializing common search space for SI-RNTI\n",0);
    pdcch_init_search_si(&pdcch);
  } else {
    INFO("Initializing user-specific search space for RNTI: 0x%x\n", rnti);
    pdcch_init_search_ue(&pdcch, rnti);
  }
  ret = -1;
  nof_frames = 0;
  do {
    filesource_read(&fsrc, input_buffer, flen);
    if (nof_frames == 5) {
      INFO("Reading %d samples sub-frame %d\n", flen, nof_frames);

      lte_fft_run_slot(&fft, input_buffer, fft_buffer);

      if (fmatlab) {
        fprintf(fmatlab, "infft%d=", nof_frames);
        vec_fprint_c(fmatlab, input_buffer, flen);
        fprintf(fmatlab, ";\n");

        fprintf(fmatlab, "outfft%d=", nof_frames);
        vec_sc_prod_cfc(fft_buffer, 1000.0, fft_buffer, CP_NSYMB(cp) * nof_prb * RE_X_RB);
        vec_fprint_c(fmatlab, fft_buffer, CP_NSYMB(cp) * nof_prb * RE_X_RB);
        fprintf(fmatlab, ";\n");
        vec_sc_prod_cfc(fft_buffer, 0.001, fft_buffer,   CP_NSYMB(cp) * nof_prb * RE_X_RB);
      }

      /* Get channel estimates for each port */
      for (i=0;i<nof_ports;i++) {
        chest_ce_slot_port(&chest, fft_buffer, ce[i], 2*nof_frames, i);
        if (fmatlab) {
          chest_fprint(&chest, fmatlab, 2*nof_frames, i);
        }
      }

      nof_dcis = pdcch_decode(&pdcch, fft_buffer, ce, &dci_rx, nof_frames%10);

      INFO("Received %d DCI messages\n", nof_dcis);

      for (i=0;i<nof_dcis;i++) {
        dci_msg_type_t type;
        if (dci_msg_get_type(&dci_rx.msg[i], &type, nof_prb, 1234)) {
          fprintf(stderr, "Can't get DCI message type\n");
          exit(-1);
        }
        printf("MSG %d: ",i);
        dci_msg_type_fprint(stdout, type);
        switch(type.type) {
        case PDSCH_SCHED:
          bzero(&ra_dl, sizeof(ra_pdsch_t));
          if (dci_msg_unpack_pdsch(&dci_rx.msg[i], &ra_dl, nof_prb, rnti != SIRNTI)) {
            fprintf(stderr, "Can't unpack PDSCH message\n");
          } else {
            ra_pdsch_fprint(stdout, &ra_dl, nof_prb);
            if (ra_dl.alloc_type == alloc_type2 && ra_dl.type2_alloc.mode == t2_loc
                && ra_dl.type2_alloc.riv == 11 && ra_dl.rv_idx == 0
                && ra_dl.harq_process == 0 && ra_dl.mcs.mcs_idx == 2) {
              printf("This is the file signal.1.92M.amar.dat\n");
              ret = 0;
            }
          }
          break;
        default:
          fprintf(stderr, "Unsupported message type\n");
          break;
        }
      }


    }

    nof_frames++;
  } while (nof_frames <= max_frames);

  base_free();
  exit(ret);
}
