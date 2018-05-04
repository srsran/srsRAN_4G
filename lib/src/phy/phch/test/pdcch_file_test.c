/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

char *input_file_name = NULL;

srslte_cell_t cell = {
  6,            // cell.cell.cell.nof_prb
  1,            // cell.cell.nof_ports
  0,            // cell.id
  SRSLTE_CP_NORM,       // cyclic prefix
  SRSLTE_PHICH_R_1,          // PHICH resources      
  SRSLTE_PHICH_NORM    // PHICH length
};

uint32_t cfi = 2;
int flen;
uint16_t rnti = SRSLTE_SIRNTI;
int max_frames = 10;

srslte_dci_format_t dci_format = SRSLTE_DCI_FORMAT1A;
srslte_filesource_t fsrc;
srslte_pdcch_t pdcch;
cf_t *input_buffer, *fft_buffer, *ce[SRSLTE_MAX_PORTS];
srslte_regs_t regs;
srslte_ofdm_t fft;
srslte_chest_dl_t chest;

void usage(char *prog) {
  printf("Usage: %s [vcfoe] -i input_file\n", prog);
  printf("\t-c cell.id [Default %d]\n", cell.id);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-o DCI Format [Default %s]\n", srslte_dci_format_string(dci_format));
  printf("\t-r rnti [Default SI-RNTI]\n");
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-m max_frames [Default %d]\n", max_frames);
  printf("\t-e Set extended prefix [Default Normal]\n");
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "irvofcenmp")) != -1) {
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
      dci_format = srslte_dci_format_from_string(argv[optind]);
      if (dci_format == SRSLTE_DCI_NOF_FORMATS) {
        fprintf(stderr, "Error unsupported format %s\n", argv[optind]);
        exit(-1);
      }
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

int base_init() {
  int i;

  if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
    fprintf(stderr, "Error opening file %s\n", input_file_name);
    exit(-1);
  }

  flen = 2 * (SRSLTE_SLOT_LEN(srslte_symbol_sz_power2(cell.nof_prb)));

  input_buffer = malloc(flen * sizeof(cf_t));
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }

  fft_buffer = malloc(SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
  if (!fft_buffer) {
    perror("malloc");
    return -1;
  }

  for (i=0;i<SRSLTE_MAX_PORTS;i++) {
    ce[i] = malloc(SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
    if (!ce[i]) {
      perror("malloc");
      return -1;
    }
  }

  if (srslte_chest_dl_init(&chest, cell.nof_prb)) {
    fprintf(stderr, "Error initializing equalizer\n");
    return -1;
  }
  if (srslte_chest_dl_set_cell(&chest, cell)) {
    fprintf(stderr, "Error initializing equalizer\n");
    return -1;
  }

  if (srslte_ofdm_init_(&fft, cell.cp, input_buffer, fft_buffer, srslte_symbol_sz_power2(cell.nof_prb), cell.nof_prb, SRSLTE_DFT_FORWARD)) {
    fprintf(stderr, "Error initializing FFT\n");
    return -1;
  }

  if (srslte_regs_init(&regs, cell)) {
    fprintf(stderr, "Error initiating regs\n");
    return -1;
  }

  if (srslte_pdcch_init_ue(&pdcch, cell.nof_prb, 1)) {
    fprintf(stderr, "Error creating PDCCH object\n");
    exit(-1);
  }
  if (srslte_pdcch_set_cell(&pdcch, &regs, cell)) {
    fprintf(stderr, "Error creating PDCCH object\n");
    exit(-1);
  }

  DEBUG("Memory init OK\n");
  return 0;
}

void base_free() {
  int i;

  srslte_filesource_free(&fsrc);

  free(input_buffer);
  free(fft_buffer);

  srslte_filesource_free(&fsrc);
  for (i=0;i<SRSLTE_MAX_PORTS;i++) {
    free(ce[i]);
  }
  srslte_chest_dl_free(&chest);
  srslte_ofdm_rx_free(&fft);

  srslte_pdcch_free(&pdcch);
  srslte_regs_free(&regs);
}

int main(int argc, char **argv) {
  srslte_ra_dl_dci_t ra_dl;
  int i;
  int frame_cnt;
  int ret;
  srslte_dci_location_t locations[MAX_CANDIDATES];
  uint32_t nof_locations;
  srslte_dci_msg_t dci_msg; 

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc,argv);

  if (base_init()) {
    fprintf(stderr, "Error initializing memory\n");
    exit(-1);
  }

  ret = -1;
  frame_cnt = 0;
  do {
    srslte_filesource_read(&fsrc, input_buffer, flen);

    INFO("Reading %d samples sub-frame %d\n", flen, frame_cnt);

    srslte_ofdm_rx_sf(&fft);

    /* Get channel estimates for each port */
    srslte_chest_dl_estimate(&chest, fft_buffer, ce, frame_cnt %10);
    
    uint16_t crc_rem = 0;
    if (srslte_pdcch_extract_llr(&pdcch, fft_buffer, 
                          ce, srslte_chest_dl_get_noise_estimate(&chest), 
                          frame_cnt %10, cfi)) {
      fprintf(stderr, "Error extracting LLRs\n");
      return -1;
    }
    if (rnti == SRSLTE_SIRNTI) {
      INFO("Initializing common search space for SI-RNTI\n");
      nof_locations = srslte_pdcch_common_locations(&pdcch, locations, MAX_CANDIDATES, cfi);
    } else {
      INFO("Initializing user-specific search space for RNTI: 0x%x\n", rnti);
      nof_locations = srslte_pdcch_ue_locations(&pdcch, locations, MAX_CANDIDATES, frame_cnt %10, cfi, rnti); 
    }

    for (i=0;i<nof_locations && crc_rem != rnti;i++) {
      if (srslte_pdcch_decode_msg(&pdcch, &dci_msg, &locations[i], dci_format, cfi, &crc_rem)) {
        fprintf(stderr, "Error decoding DCI msg\n");
        return -1;
      }
    }
    
    if (crc_rem == rnti) {
      srslte_dci_msg_type_t type;
      if (srslte_dci_msg_get_type(&dci_msg, &type, cell.nof_prb, rnti)) {
        fprintf(stderr, "Can't get DCI message type\n");
        exit(-1);
      }
      printf("MSG %d: ",i);
      srslte_dci_msg_type_fprint(stdout, type);
      switch(type.type) {
      case SRSLTE_DCI_MSG_TYPE_PDSCH_SCHED:
        bzero(&ra_dl, sizeof(srslte_ra_dl_dci_t));
        if (srslte_dci_msg_unpack_pdsch(&dci_msg, &ra_dl, cell.nof_prb, cell.nof_ports, rnti != SRSLTE_SIRNTI)) {
          fprintf(stderr, "Can't unpack DCI message\n");
        } else {
          srslte_ra_pdsch_fprint(stdout, &ra_dl, cell.nof_prb);
          if (ra_dl.alloc_type == SRSLTE_RA_ALLOC_TYPE2 && ra_dl.type2_alloc.mode == SRSLTE_RA_TYPE2_LOC
              && ra_dl.type2_alloc.riv == 11 && ra_dl.rv_idx == 0
              && ra_dl.harq_process == 0 && ra_dl.mcs_idx == 2) {
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

    frame_cnt++;
  } while (frame_cnt <= max_frames);

  base_free();
  srslte_dft_exit();
  exit(ret);
}
