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
#include <sys/time.h>

#include "srslte/srslte.h"

srslte_cell_t cell = {
  25,            // nof_prb
  1,            // nof_ports
  2,            // bw_idx = 5 MHz
  1,            // cell_id
  SRSLTE_CP_NORM,       // cyclic prefix
  SRSLTE_PHICH_R_1_6,          // PHICH resources      
  SRSLTE_PHICH_NORM    // PHICH length
};

uint32_t subframe = 0;

void usage(char *prog) {
  printf("Usage: %s [csNnv]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-s subframe [Default %d]\n", subframe);
  printf("\t-n nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "csNnv")) != -1) {
    switch(opt) {
    case 's':
      subframe = atoi(argv[optind]);
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
}

int main(int argc, char **argv) {
  srslte_pucch_t pucch;
  srslte_pucch_cfg_t pucch_cfg;
  srslte_refsignal_ul_t dmrs;
  uint8_t bits[SRSLTE_PUCCH_MAX_BITS];
  uint8_t pucch2_bits[2]; 
  cf_t *sf_symbols = NULL;
  cf_t pucch_dmrs[2*SRSLTE_NRE*3];
  int ret = -1;
  
  parse_args(argc,argv);

  if (srslte_pucch_init(&pucch, cell)) {
    fprintf(stderr, "Error creating PDSCH object\n");
    exit(-1);
  }
  if (srslte_refsignal_ul_init(&dmrs, cell)) {
    fprintf(stderr, "Error creating PDSCH object\n");
    exit(-1);
  }
  
  bzero(&pucch_cfg, sizeof(srslte_pucch_cfg_t));
  
  for (int i=0;i<SRSLTE_PUCCH_MAX_BITS;i++) {
    bits[i] = i%2;
  }
        
  for (int i=0;i<2;i++) {
    pucch2_bits[i] = i%2;
  }
  
  if (srslte_pucch_set_crnti(&pucch, 11)) {
    fprintf(stderr, "Error setting C-RNTI\n");
    goto quit; 
  }
  
  sf_symbols = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp));
  if (!sf_symbols) {
    goto quit; 
  }

  srslte_pucch_format_t format; 
  for (format=0;format<=SRSLTE_PUCCH_FORMAT_2B;format++) {
    for (uint32_t d=1;d<=3;d++) {
      for (uint32_t ncs=0;ncs<8;ncs+=d) {
        for (uint32_t n_pucch=1;n_pucch<130;n_pucch+=50) {
        
          struct timeval t[3]; 
          
          pucch_cfg.delta_pucch_shift = d; 
          bool group_hopping_en = false; 
          pucch_cfg.N_cs = ncs; 
          pucch_cfg.n_rb_2 = 0; 
    
          gettimeofday(&t[1], NULL);
          if (!srslte_pucch_set_cfg(&pucch, &pucch_cfg, group_hopping_en)) {
            fprintf(stderr, "Error setting PUCCH config\n");
            goto quit; 
          }
          
          if (srslte_pucch_encode(&pucch, format, n_pucch, subframe, bits, sf_symbols)) {
            fprintf(stderr, "Error encoding PUCCH\n");
            goto quit; 
          }
          
          srslte_refsignal_dmrs_pusch_cfg_t pusch_cfg; 
          pusch_cfg.group_hopping_en = group_hopping_en; 
          pusch_cfg.sequence_hopping_en = false; 
          srslte_refsignal_ul_set_cfg(&dmrs, &pusch_cfg, &pucch_cfg, NULL);
          
          if (srslte_refsignal_dmrs_pucch_gen(&dmrs, format, n_pucch, subframe, pucch2_bits, pucch_dmrs)) {
            fprintf(stderr, "Error encoding PUCCH\n");
            goto quit; 
          }
          if (srslte_refsignal_dmrs_pucch_put(&dmrs, format, n_pucch, pucch_dmrs, sf_symbols)) {
            fprintf(stderr, "Error encoding PUCCH\n");
            goto quit; 
          }     
          gettimeofday(&t[2], NULL);
          get_time_interval(t);
          INFO("format %d, n_pucch: %d, ncs: %d, d: %d, t_exec=%d us\n", format, n_pucch, ncs, d, t[0].tv_usec);
        }
      }
    }    
  }

  ret = 0;
quit:
  srslte_pucch_free(&pucch);
  srslte_refsignal_ul_free(&dmrs);
  
  if (sf_symbols) {
    free(sf_symbols);
  }
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  exit(ret);
}
