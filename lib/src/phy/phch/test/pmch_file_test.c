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
  100,            // nof_prb
  1,            // nof_ports
  1,            // cell_id
  SRSLTE_CP_EXT,       // cyclic prefix
  SRSLTE_PHICH_R_1,          // PHICH resources      
  SRSLTE_PHICH_NORM    // PHICH length
};

int flen;

uint32_t cfi = 2;
uint16_t rnti = SRSLTE_SIRNTI;

int max_frames = 150;
uint32_t sf_idx = 1;

uint8_t non_mbsfn_region = 2;
int mbsfn_area_id = 1;

srslte_dci_format_t dci_format = SRSLTE_DCI_FORMAT1A;
srslte_filesource_t fsrc;
srslte_ue_dl_t ue_dl;
cf_t *input_buffer[SRSLTE_MAX_PORTS];

void usage(char *prog) {
  printf("Usage: %s [rovfcenmps] -i input_file\n", prog);
  printf("\t-o DCI format [Default %s]\n", srslte_dci_format_string(dci_format));
  printf("\t-c cell.id [Default %d]\n", cell.id);
  printf("\t-s Start subframe_idx [Default %d]\n", sf_idx);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-r rnti [Default 0x%x]\n",rnti);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-M mbsfn_area_id [Default %d]\n", mbsfn_area_id);
  printf("\t-e Set extended prefix [Default Normal]\n");
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "irovfcenmps")) != -1) {
    switch(opt) {
    case 'i':
      input_file_name = argv[optind];
      break;
    case 'c':
      cell.id = atoi(argv[optind]);
      break;
    case 's':
      sf_idx = atoi(argv[optind]);
      break;
    case 'r':
      rnti = strtoul(argv[optind], NULL, 0);
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
    case 'M':
      mbsfn_area_id = atoi(argv[optind]);
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

  if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
    fprintf(stderr, "Error opening file %s\n", input_file_name);
    exit(-1);
  }

  flen = 2 * (SRSLTE_SLOT_LEN(srslte_symbol_sz(cell.nof_prb)));

  input_buffer[0] = malloc(flen * sizeof(cf_t));
  if (!input_buffer[0]) {
    perror("malloc");
    exit(-1);
  }

  if (srslte_ue_dl_init(&ue_dl, input_buffer, cell.nof_prb, 1)) {
    fprintf(stderr, "Error initializing UE DL\n");
    return -1;
  }
  

  if (srslte_ue_dl_set_cell(&ue_dl, cell)) {
    fprintf(stderr, "Error initializing UE DL\n");
    return -1;
  }

  srslte_ue_dl_set_rnti(&ue_dl, rnti); 

  srslte_ue_dl_set_mbsfn_area_id(&ue_dl, mbsfn_area_id);
  srslte_ue_dl_set_non_mbsfn_region(&ue_dl, non_mbsfn_region);
  
  
  DEBUG("Memory init OK\n");
  return 0;
}

void base_free() {
  srslte_filesource_free(&fsrc);
  srslte_ue_dl_free(&ue_dl);
  free(input_buffer[0]);
}

int main(int argc, char **argv) {
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

  uint8_t *data = malloc(100000);
  if (!data) {
    perror("malloc");
    exit(-1);
  }

  ret = -1;

  srslte_filesource_read(&fsrc, input_buffer[0], flen);
  INFO("Reading %d samples sub-frame %d\n", flen, sf_idx);
  ret = srslte_ue_dl_decode_mbsfn(&ue_dl, data, sf_idx);
  if(ret > 0) {
    printf("PMCH Decoded OK!\n");       
  }  else if (ret < 0) {
    printf("Error decoding PMCH\n");
  }

  base_free();
  if (data != NULL) {
    free(data);
  }
  srslte_dft_exit();
  if (ret > 0) {
    exit(0);
  } else {
    exit(-1); 
  }
}
