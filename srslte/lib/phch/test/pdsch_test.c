/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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

#include "srslte/srslte.h"

srslte_cell_t cell = {
  6,            // nof_prb
  1,            // nof_ports
  0,            // cell_id
  SRSLTE_CP_NORM,       // cyclic prefix
  SRSLTE_PHICH_SRSLTE_PHICH_R_1_6,          // PHICH resources      
  SRSLTE_PHICH_NORM    // PHICH length
};

uint32_t cfi = 2;
uint32_t tbs = 0;
uint32_t nof_tbs = 0; 
uint32_t subframe = 1;
srslte_mod_t modulation = SRSLTE_MOD_BPSK;
uint32_t rv_idx = 0;

void usage(char *prog) {
  printf("Usage: %s [Lcpsrnfvmt] -l TBS \n", prog);
  printf("\t-L number of consequent TBS [Default 0]\n");
  printf("\t-m modulation (1: BPSK, 2: QPSK, 3: QAM16, 4: QAM64) [Default BPSK]\n");
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-s subframe [Default %d]\n", subframe);
  printf("\t-r rv_idx [Default %d]\n", rv_idx);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "lLcpnfvmtsr")) != -1) {
    switch(opt) {
    case 'm':
      switch(atoi(argv[optind])) {
      case 1:
        modulation = SRSLTE_MOD_BPSK;
        break;
      case 2:
        modulation = SRSLTE_MOD_QPSK;
        break;
      case 4:
        modulation = SRSLTE_MOD_16QAM;
        break;
      case 6:
        modulation = SRSLTE_MOD_64QAM;
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
    case 'L':
      nof_tbs = atoi(argv[optind]);
      break;
    case 'p':
      cell.nof_ports = atoi(argv[optind]);
      break;
    case 'n':
      cell.nof_prb = atoi(argv[optind]);
      break;
    case 'c':
      cell.id = atoi(argv[optind]);
      break;
    case 'v':
      srslte_verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (tbs == 0) {
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char **argv) {
  srslte_pdsch_t pdsch;
  uint32_t i, j;
  uint8_t *data = NULL;
  cf_t *ce[SRSLTE_MAX_PORTS];
  uint32_t nof_re;
  cf_t *slot_symbols[SRSLTE_MAX_PORTS];
  int ret = -1;
  struct timeval t[3];
  srslte_ra_mcs_t mcs;
  srslte_ra_dl_alloc_t prb_alloc;
  srslte_harq_t harq_process;
  uint32_t rv;

  parse_args(argc,argv);

  bzero(&pdsch, sizeof(srslte_pdsch_t));
  bzero(&harq_process, sizeof(srslte_harq_t));
  bzero(ce, sizeof(cf_t*)*SRSLTE_MAX_PORTS);
  bzero(slot_symbols, sizeof(cf_t*)*SRSLTE_MAX_PORTS);
  
  nof_re = 2 * SRSLTE_CP_NORM_NSYMB * cell.nof_prb * SRSLTE_NRE;

  mcs.mod = modulation;
  
  prb_alloc.slot[0].nof_prb = cell.nof_prb;
  for (i=0;i<prb_alloc.slot[0].nof_prb;i++) {
    prb_alloc.slot[0].prb_idx[i] = true;
  }
  memcpy(&prb_alloc.slot[1], &prb_alloc.slot[0], sizeof(srslte_ra_prb_slot_t));

  srslte_ra_dl_alloc_re(&prb_alloc, cell.nof_prb, cell.nof_ports, cell.nof_prb<10?(cfi+1):cfi, cell.cp);
  
  /* init memory */
  for (i=0;i<cell.nof_ports;i++) {
    ce[i] = malloc(sizeof(cf_t) * nof_re);
    if (!ce[i]) {
      perror("malloc");
      goto quit;
    }
    for (j=0;j<nof_re;j++) {
      ce[i][j] = 1;
    }
    slot_symbols[i] = calloc(sizeof(cf_t) , nof_re);
    if (!slot_symbols[i]) {
      perror("malloc");
      goto quit;
    }
  }
  
  data = malloc(sizeof(uint8_t) * (tbs+nof_tbs));
  if (!data) {
    perror("malloc");
    goto quit;
  }

  if (srslte_pdsch_init(&pdsch, cell)) {
    fprintf(stderr, "Error creating PDSCH object\n");
    goto quit;
  }
  
  srslte_pdsch_set_rnti(&pdsch, 1234);
  
  if (srslte_harq_init(&harq_process, cell)) {
    fprintf(stderr, "Error initiating HARQ process\n");
    goto quit;
  }
  
  for (mcs.tbs = tbs;mcs.tbs<=tbs+nof_tbs;mcs.tbs++) {
    if (SRSLTE_VERBOSE_ISNONE()) {
      printf("Decoding TBS: %d\r",mcs.tbs);
    }
    for (i=0;i<mcs.tbs;i++) {
      data[i] = rand()%2;
    }

    for (rv=0;rv<=rv_idx;rv++) {
      if (srslte_harq_setup_dl(&harq_process, mcs, rv, subframe, &prb_alloc)) {
        fprintf(stderr, "Error configuring HARQ process\n");
        goto quit;
      }

      if (srslte_pdsch_encode(&pdsch, &harq_process, data, slot_symbols)) {
        fprintf(stderr, "Error encoding PDSCH\n");
        goto quit;
      }

      /* combine outputs */
      for (i=0;i<cell.nof_ports;i++) {
        for (j=0;j<nof_re;j++) {
          if (i > 0) {
            slot_symbols[0][j] += slot_symbols[i][j];
          }
          ce[i][j] = 1;
        }
      }
      
      gettimeofday(&t[1], NULL);
      int r = srslte_pdsch_decode(&pdsch, &harq_process, slot_symbols[0], ce, 0, data);
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      if (r) {
        printf("Error decoding TBS: %d\n", mcs.tbs);
        ret = -1;
        goto quit;
      } else {
        if (nof_tbs == 0) {
          printf("DECODED OK in %d:%d (%.2f Mbps)\n", (int) t[0].tv_sec, (int) t[0].tv_usec, (float) mcs.tbs/t[0].tv_usec);          
        }
      }
    } 
  }
  ret = 0;
quit:
  srslte_pdsch_free(&pdsch);
  srslte_harq_free(&harq_process);
  
  for (i=0;i<cell.nof_ports;i++) {
    if (ce[i]) {
      free(ce[i]);
    }
    if (slot_symbols[i]) {
      free(slot_symbols[i]);
    }
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
