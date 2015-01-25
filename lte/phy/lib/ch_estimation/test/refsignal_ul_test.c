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
#include <strings.h>
#include <unistd.h>
#include <complex.h>

#include "liblte/phy/phy.h"

lte_cell_t cell = {
  6,            // nof_prb
  MAX_PORTS,    // nof_ports
  0,         // cell_id
  CPNORM        // cyclic prefix
};

void usage(char *prog) {
  printf("Usage: %s [recv]\n", prog);

  printf("\t-r nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-e extended cyclic prefix [Default normal]\n");

  printf("\t-c cell_id (1000 tests all). [Default %d]\n", cell.id);

  printf("\t-v increase verbosity\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "recv")) != -1) {
    switch(opt) {
    case 'r':
      cell.nof_prb = atoi(argv[optind]);
      break;
    case 'e':
      cell.cp = CPEXT;
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

lte_hopping_method_t hopping_modes[3]={HOPPING_OFF, HOPPING_GROUP, HOPPING_SEQUENCE};

int main(int argc, char **argv) {
  refsignal_ul_t refs;
  refsignal_drms_pusch_cfg_t pusch_cfg;
  cf_t *signal = NULL;
  int i, j;
  int ret = -1;
  
  parse_args(argc,argv);

  signal = malloc(RE_X_RB * cell.nof_prb * sizeof(cf_t));
  if (!signal) {
    perror("malloc");
    goto do_exit;
  }
  
  if (refsignal_ul_init(&refs, cell)) {
    fprintf(stderr, "Error initializing UL reference signal\n");
    goto do_exit;
  }
  
  printf("Running tests for %d PRB\n", cell.nof_prb);
    
  for (int n=3;n<cell.nof_prb;n++) {
    for (int delta_ss=0;delta_ss<NOF_DELTA_SS;delta_ss++) {
      for (int cshift=0;cshift<NOF_CSHIFT;cshift++) {
        for (int h=0;h<3;h++) {
          for (int ns=0;ns<NSLOTS_X_FRAME;ns++) {
            for (int cshift_drms=5;cshift_drms<NOF_CSHIFT;cshift_drms++) {
              pusch_cfg.beta_pusch = 1.0;
              pusch_cfg.nof_prb = n;
              pusch_cfg.common.cyclic_shift = cshift;
              pusch_cfg.common.cyclic_shift_for_drms = cshift_drms;
              pusch_cfg.common.delta_ss = delta_ss;            
              pusch_cfg.hopping_method = hopping_modes[h];
              pusch_cfg.common.en_drms_2 = true; 
              printf("Beta: %f, ",pusch_cfg.beta_pusch);
              printf("nof_prb: %d, ",pusch_cfg.nof_prb);
              printf("cyclic_shift: %d, ",pusch_cfg.common.cyclic_shift);
              printf("cyclic_shift_for_drms: %d, ",pusch_cfg.common.cyclic_shift_for_drms);
              printf("delta_ss: %d, ",pusch_cfg.common.delta_ss);
              printf("hopping_method: %d, ",pusch_cfg.hopping_method);
              printf("Slot: %d\n", ns);
              refsignal_dmrs_pusch_gen(&refs, &pusch_cfg, ns, signal);              
            exit(0);
            }
          }
        }
      }
    }
  }

  ret = 0;

do_exit:

  if (signal) {
    free(signal);
  }

  refsignal_ul_free(&refs);
  
  if (!ret) {
    printf("OK\n");
  } 
  exit(ret);
}
