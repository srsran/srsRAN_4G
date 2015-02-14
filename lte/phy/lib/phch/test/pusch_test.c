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
#include <sys/time.h>

#include "liblte/phy/phy.h"

lte_cell_t cell = {
  6,            // nof_prb
  1,            // nof_ports
  0,            // cell_id
  CPNORM,       // cyclic prefix
  R_1_6,          // PHICH resources      
  PHICH_NORM    // PHICH length
};

uint32_t cfi = 2;
uint32_t tbs = 0;
uint32_t subframe = 1;
lte_mod_t modulation = LTE_QPSK;
uint32_t rv_idx = 0;

void usage(char *prog) {
  printf("Usage: %s [csrnfvmt] -l TBS \n", prog);
  printf("\t-m modulation (1: BPSK, 2: QPSK, 3: QAM16, 4: QAM64) [Default BPSK]\n");
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-s subframe [Default %d]\n", subframe);
  printf("\t-r rv_idx [Default %d]\n", rv_idx);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "lcnfvmtsr")) != -1) {
    switch(opt) {
    case 'm':
      switch(atoi(argv[optind])) {
      case 1:
        modulation = LTE_BPSK;
        break;
      case 2:
        modulation = LTE_QPSK;
        break;
      case 4:
        modulation = LTE_QAM16;
        break;
      case 6:
        modulation = LTE_QAM64;
        break;
      default:
        fprintf(stderr, "Invalid modulation %d. Possible values: "
            "(1: BPSK, 2: QPSK, 3: QAM16, 4: QAM64)\n", atoi(argv[optind]));
        break;
      }
      break;
    case 's':
      subframe = atoi(argv[optind]);
      break;
    case 'r':
      rv_idx = atoi(argv[optind]);
      break;
    case 'l':
      tbs = atoi(argv[optind]);
      break;
    case 'n':
      cell.nof_prb = atoi(argv[optind]);
      break;
    case 'c':
      cell.id = atoi(argv[optind]);
      break;
    case 'v':
      verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}

int main(int argc, char **argv) {
  pusch_t pusch;
  uint8_t *data = NULL;
  cf_t *sf_symbols = NULL;
  int ret = -1;
  struct timeval t[3];
  ra_mcs_t mcs;
  ra_prb_t prb_alloc;
  harq_t harq_process;
  
  parse_args(argc,argv);

  mcs.tbs = tbs;
  mcs.mod = modulation;
  
  bzero(&prb_alloc, sizeof(ra_prb_t));
  prb_alloc.slot[0].nof_prb = 2;
  memcpy(&prb_alloc.slot[1], &prb_alloc.slot[0], sizeof(ra_prb_slot_t));

  if (pusch_init(&pusch, cell)) {
    fprintf(stderr, "Error creating PDSCH object\n");
    goto quit;
  }
  pusch_set_rnti(&pusch, 1234);
  
  if (harq_init(&harq_process, cell)) {
    fprintf(stderr, "Error initiating HARQ process\n");
    goto quit;
  }

  printf("Encoding rv_idx=%d\n",rv_idx);
  
  uint8_t tmp[20];
  for (uint32_t i=0;i<20;i++) {
    tmp[i] = 1;
  }
  uci_data_t uci_data; 
  bzero(&uci_data, sizeof(uci_data_t));
  uci_data.beta_cqi = 2.0; 
  uci_data.beta_ri = 2.0; 
  uci_data.beta_ack = 2.0; 
  
  uci_data.uci_cqi_len = 0; 
  uci_data.uci_ri_len = 0; 
  uci_data.uci_ack_len = 0; 

  uci_data.uci_cqi = tmp;
  uci_data.uci_ri = 1; 
  uci_data.uci_ack = 1; 
  
  if (harq_setup_ul(&harq_process, mcs, 0, subframe, &prb_alloc)) {
    fprintf(stderr, "Error configuring HARQ process\n");
    goto quit;
  }

  uint32_t nof_re = RE_X_RB*cell.nof_prb*2*CP_NSYMB(cell.cp);
  sf_symbols = vec_malloc(sizeof(cf_t) * nof_re);
  if (!sf_symbols) {
    perror("malloc");
    goto quit;
  }

  data = malloc(sizeof(uint8_t) * mcs.tbs);
  if (!data) {
    perror("malloc");
    goto quit;
  }
  
  for (uint32_t i=0;i<mcs.tbs;i++) {
    data[i] = 1;
  }

  if (pusch_uci_encode(&pusch, &harq_process, data, uci_data, sf_symbols)) {
    fprintf(stderr, "Error encoding TB\n");
    exit(-1);
  }

  if (rv_idx > 0) {
    if (harq_setup_ul(&harq_process, mcs, rv_idx, subframe, &prb_alloc)) {
      fprintf(stderr, "Error configuring HARQ process\n");
      goto quit;
    }

    if (pusch_uci_encode(&pusch, &harq_process, data, uci_data, sf_symbols)) {
      fprintf(stderr, "Error encoding TB\n");
      exit(-1);
    }
  }
  
  gettimeofday(&t[1], NULL);
  //int r = pusch_decode(&pusch, slot_symbols[0], ce, 0, data, subframe, &harq_process, rv);
  int r = 0; 
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  if (r) {
    printf("Error decoding\n");
    ret = -1;
    goto quit;
  } else {
    printf("DECODED OK in %d:%d (%.2f Mbps)\n", (int) t[0].tv_sec, (int) t[0].tv_usec, (float) mcs.tbs/t[0].tv_usec);
  }
  
  ret = 0;
quit:
  pusch_free(&pusch);
  harq_free(&harq_process);
  
  if (sf_symbols) {
    free(sf_symbols);
  }
  if (data) {
    free(data);
  }
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  exit(ret);
}
