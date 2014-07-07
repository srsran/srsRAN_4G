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

uint32_t cfi = 2;
uint16_t rnti = SIRNTI;

int max_frames = 10;
FILE *fmatlab = NULL;

filesource_t fsrc;
pdcch_t pdcch;
pdsch_t pdsch;
pdsch_harq_t harq_process;
cf_t *input_buffer, *fft_buffer, *ce[MAX_PORTS];
regs_t regs;
lte_fft_t fft;
chest_t chest;

void usage(char *prog) {
  printf("Usage: %s [vcfoe] -i input_file\n", prog);
  printf("\t-o output matlab file name [Default Disabled]\n");
  printf("\t-c cell.id [Default %d]\n", cell.id);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-r rnti [Default SI-RNTI]\n");
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
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
      cell.id = atoi(argv[optind]);
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

  flen = 2 * (SLOT_LEN(lte_symbol_sz(cell.nof_prb), cell.cp));

  input_buffer = malloc(flen * sizeof(cf_t));
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }

  fft_buffer = malloc(2 * CP_NSYMB(cell.cp) * cell.nof_prb * RE_X_RB * sizeof(cf_t));
  if (!fft_buffer) {
    perror("malloc");
    return -1;
  }

  for (i=0;i<MAX_PORTS;i++) {
    ce[i] = malloc(2 * CP_NSYMB(cell.cp) * cell.nof_prb * RE_X_RB * sizeof(cf_t));
    if (!ce[i]) {
      perror("malloc");
      return -1;
    }
  }

  if (chest_init_LTEDL(&chest, LINEAR, cell)) {
    fprintf(stderr, "Error initializing equalizer\n");
    return -1;
  }

  if (lte_fft_init(&fft, cell.cp, cell.nof_prb)) {
    fprintf(stderr, "Error initializing FFT\n");
    return -1;
  }

  if (regs_init(&regs, R_1, PHICH_NORM, cell)) {
    fprintf(stderr, "Error initiating regs\n");
    return -1;
  }

  if (regs_set_cfi(&regs, cfi)) {
    fprintf(stderr, "Error setting CFI %d\n", cfi);
    return -1;
  }

  if (pdcch_init(&pdcch, &regs, cell)) {
    fprintf(stderr, "Error creating PDCCH object\n");
    exit(-1);
  }

  if (pdsch_init(&pdsch, rnti, cell)) {
    fprintf(stderr, "Error creating PDSCH object\n");
    exit(-1);
  }
  
  if (pdsch_harq_init(&harq_process, &pdsch)) {
    fprintf(stderr, "Error initiating HARQ process\n");
    exit(-1);
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

  pdcch_free(&pdcch);
  pdsch_free(&pdsch);
  pdsch_harq_free(&harq_process);
  regs_free(&regs);
}

int main(int argc, char **argv) {
  ra_pdsch_t ra_dl;
  ra_prb_t prb_alloc;
  int i;
  int nof_frames;
  int ret;
  char *data;
  dci_location_t locations[10];
  uint32_t nof_locations;
  dci_msg_t dci_msg; 
  
  data = malloc(10000);

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
    nof_locations = pdcch_common_locations(&pdcch, locations, 10, cfi);
  } else {
    // For ue-specific, generate locations for subframe 5
    INFO("Initializing user-specific search space for RNTI: 0x%x\n", rnti);
    nof_locations = pdcch_ue_locations(&pdcch, locations, 10, 5, cfi, rnti); 
  }
  
  ret = -1;
  nof_frames = 0;
  do {
    filesource_read(&fsrc, input_buffer, flen);
    if (nof_frames == 5) {
      INFO("Reading %d samples sub-frame %d\n", flen, nof_frames);

      lte_fft_run_sf(&fft, input_buffer, fft_buffer);

      if (fmatlab) {
        fprintf(fmatlab, "infft%d=", nof_frames);
        vec_fprint_c(fmatlab, input_buffer, flen);
        fprintf(fmatlab, ";\n");

        fprintf(fmatlab, "outfft%d=", nof_frames);
        vec_sc_prod_cfc(fft_buffer, 1000.0, fft_buffer, CP_NSYMB(cell.cp) * cell.nof_prb * RE_X_RB);
        vec_fprint_c(fmatlab, fft_buffer, CP_NSYMB(cell.cp) * cell.nof_prb * RE_X_RB);
        fprintf(fmatlab, ";\n");
        vec_sc_prod_cfc(fft_buffer, 0.001, fft_buffer,  CP_NSYMB(cell.cp) * cell.nof_prb * RE_X_RB);
      }

      /* Get channel estimates for each port */
      for (i=0;i<cell.nof_ports;i++) {
        chest_ce_slot_port(&chest, fft_buffer, ce[i], 2*nof_frames, i);
        chest_ce_slot_port(&chest, &fft_buffer[CP_NSYMB(cell.cp) * cell.nof_prb * RE_X_RB],
            &ce[i][CP_NSYMB(cell.cp) * cell.nof_prb * RE_X_RB], 2*nof_frames+1, i);
        if (fmatlab) {
          chest_fprint(&chest, fmatlab, 2*nof_frames+1, i);
        }
      }
      
      
      uint16_t crc_rem = 0;
      for (i=0;i<nof_locations && crc_rem != rnti;i++) {
        if (pdcch_extract_llr(&pdcch, fft_buffer, ce, locations[i], nof_frames, cfi)) {
          fprintf(stderr, "Error extracting LLRs\n");
          return -1;
        }
        if (pdcch_decode_msg(&pdcch, &dci_msg, Format1A, &crc_rem)) {
          fprintf(stderr, "Error decoding DCI msg\n");
          return -1;
        }
      }
      
      if (crc_rem == rnti) {
        dci_msg_type_t type;
        if (dci_msg_get_type(&dci_msg, &type, cell.nof_prb, rnti, 1234)) {
          fprintf(stderr, "Can't get DCI message type\n");
          goto goout;
        }
        
        dci_msg_type_fprint(stdout, type);
        switch(type.type) {
        case PDSCH_SCHED:
          bzero(&ra_dl, sizeof(ra_pdsch_t));
          if (dci_msg_unpack_pdsch(&dci_msg, &ra_dl, cell.nof_prb, rnti != SIRNTI)) {
            fprintf(stderr, "Can't unpack PDSCH message\n");
          } else {
            ra_pdsch_fprint(stdout, &ra_dl, cell.nof_prb);
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
        if (ra_prb_get_dl(&prb_alloc, &ra_dl, cell.nof_prb)) {
          fprintf(stderr, "Error computing resource allocation\n");
          goto goout;
        }
        ra_prb_get_re_dl(&prb_alloc, cell.nof_prb, cell.nof_ports, cell.nof_prb<10?(cfi+1):cfi, cell.cp);

        if (pdsch_harq_setup(&harq_process, ra_dl.mcs, &prb_alloc)) {
          fprintf(stderr, "Error configuring HARQ process\n");
          goto goout;
        }
        if (pdsch_decode(&pdsch, fft_buffer, ce, data, nof_frames%10, &harq_process, ra_dl.rv_idx)) {
          fprintf(stderr, "Error decoding PDSCH\n");
          goto goout;
        } else {
          printf("PDSCH Decoded OK!\n");
        }
      }
    }

    nof_frames++;
  } while (nof_frames <= max_frames);

goout:
  base_free();
  exit(ret);
}
