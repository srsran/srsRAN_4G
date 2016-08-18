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

// Enable to measure execution time
//#define DO_OFDM

srslte_cell_t cell = {
  6,            // nof_prb
  1,            // nof_ports
  0,
  0,            // cell_id
  SRSLTE_CP_NORM,       // cyclic prefix
  SRSLTE_PHICH_R_1_6,          // PHICH resources      
  SRSLTE_PHICH_NORM    // PHICH length
};

uint32_t cfi = 2;
uint32_t mcs = 0;
uint32_t subframe = 1;
uint32_t rv_idx = 0;
uint16_t rnti = 1234; 
char *input_file = NULL; 

void usage(char *prog) {
  printf("Usage: %s [fmcsrRFpnv] \n", prog);
  printf("\t-f read signal from file [Default generate it with pdsch_encode()]\n");
  printf("\t-m MCS [Default %d]\n", mcs);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-s subframe [Default %d]\n", subframe);
  printf("\t-r rv_idx [Default %d]\n", rv_idx);
  printf("\t-R rnti [Default %d]\n", rnti);
  printf("\t-F cfi [Default %d]\n", cfi);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "fmcsrRFpnv")) != -1) {
    switch(opt) {
    case 'f':
      input_file = argv[optind];
      break;
    case 'm':
      mcs = atoi(argv[optind]);
      break;
    case 's':
      subframe = atoi(argv[optind]);
      break;
    case 'r':
      rv_idx = atoi(argv[optind]);
      break;
    case 'R':
      rnti = atoi(argv[optind]);
      break;
    case 'F':
      cfi = atoi(argv[optind]);
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
}

uint8_t *data = NULL;
cf_t *ce[SRSLTE_MAX_PORTS];
srslte_softbuffer_rx_t softbuffer_rx;
srslte_ra_dl_grant_t grant; 
srslte_pdsch_cfg_t pdsch_cfg; 
cf_t *sf_symbols;
cf_t *slot_symbols[SRSLTE_MAX_PORTS];
srslte_pdsch_t pdsch;
srslte_ofdm_t ofdm_tx, ofdm_rx; 

int main(int argc, char **argv) {
  uint32_t i, j;
  int ret = -1;
  struct timeval t[3];
  srslte_softbuffer_tx_t softbuffer_tx;
  
  parse_args(argc,argv);

  bzero(&pdsch, sizeof(srslte_pdsch_t));
  bzero(&pdsch_cfg, sizeof(srslte_pdsch_cfg_t));
  bzero(ce, sizeof(cf_t*)*SRSLTE_MAX_PORTS);
  bzero(slot_symbols, sizeof(cf_t*)*SRSLTE_MAX_PORTS);
  bzero(&softbuffer_rx, sizeof(srslte_softbuffer_rx_t));
  bzero(&softbuffer_tx, sizeof(srslte_softbuffer_tx_t));
  
  srslte_ra_dl_dci_t dci;
  bzero(&dci, sizeof(srslte_ra_dl_dci_t));
  dci.mcs_idx = mcs;
  dci.rv_idx = rv_idx;
  dci.type0_alloc.rbg_bitmask = 0xffffffff;
  if (srslte_ra_dl_dci_to_grant(&dci, cell.nof_prb, rnti, &grant)) {
    fprintf(stderr, "Error computing resource allocation\n");
    return ret;
  }
  
  srslte_ofdm_tx_init(&ofdm_tx, cell.cp, cell.nof_prb);
  srslte_ofdm_rx_init(&ofdm_rx, cell.cp, cell.nof_prb);

  sf_symbols=srslte_vec_malloc(sizeof(cf_t)*SRSLTE_SF_LEN_PRB(cell.nof_prb));
  
  /* Configure PDSCH */
  if (srslte_pdsch_cfg(&pdsch_cfg, cell, &grant, cfi, subframe, rv_idx)) {
    fprintf(stderr, "Error configuring PDSCH\n");
    exit(-1);
  }

  /* init memory */
  for (i=0;i<SRSLTE_MAX_PORTS;i++) {
    ce[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp));
    if (!ce[i]) {
      perror("srslte_vec_malloc");
      goto quit;
    }
    for (j=0;j<SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp);j++) {
      ce[i][j] = 1;
    }
    slot_symbols[i] = srslte_vec_malloc(sizeof(cf_t)*SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp));
    if (!slot_symbols[i]) {
      perror("srslte_vec_malloc");
      goto quit;
    }
  }
  
  data = srslte_vec_malloc(sizeof(uint8_t) * grant.mcs.tbs/8);
  if (!data) {
    perror("srslte_vec_malloc");
    goto quit;
  }

  if (srslte_pdsch_init(&pdsch, cell)) {
    fprintf(stderr, "Error creating PDSCH object\n");
    goto quit;
  }
  
  srslte_pdsch_set_rnti(&pdsch, rnti);
  
  if (srslte_softbuffer_rx_init(&softbuffer_rx, cell.nof_prb)) {
    fprintf(stderr, "Error initiating RX soft buffer\n");
    goto quit;
  }

  if (input_file) {
    srslte_filesource_t fsrc;
    if (srslte_filesource_init(&fsrc, input_file, SRSLTE_COMPLEX_FLOAT_BIN)) {
      fprintf(stderr, "Error opening file %s\n", input_file);
      exit(-1);
    }
#ifdef DO_OFDM
    srslte_filesource_read(&fsrc, sf_symbols, SRSLTE_SF_LEN_PRB(cell.nof_prb));
#else
    srslte_filesource_read(&fsrc, slot_symbols[0], SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp));
#endif
    
    srslte_chest_dl_t chest; 
    if (srslte_chest_dl_init(&chest, cell)) {
      printf("Error initializing equalizer\n");
      exit(-1);
    }
    srslte_chest_dl_estimate(&chest, slot_symbols[0], ce, subframe);    
    srslte_chest_dl_free(&chest);
    
    srslte_filesource_free(&fsrc);
  } else { 
    if (srslte_softbuffer_tx_init(&softbuffer_tx, cell.nof_prb)) {
      fprintf(stderr, "Error initiating TX soft buffer\n");
      goto quit;
    }

    for (i=0;i<grant.mcs.tbs/8;i++) {
      data[i] = rand()%256;
    }
    
    uint8_t databit[100000];
    srslte_bit_unpack_vector(data, databit, grant.mcs.tbs);
    srslte_vec_save_file("data_in", databit, grant.mcs.tbs);
    
    if (!input_file) {
      
      if (rv_idx) {
        /* Do 1st transmission for rv_idx!=0 */
        pdsch_cfg.rv = 0;
        if (srslte_pdsch_encode(&pdsch, &pdsch_cfg, &softbuffer_tx, data, slot_symbols)) {
          fprintf(stderr, "Error encoding PDSCH\n");
          goto quit;
        }
      }
      pdsch_cfg.rv = rv_idx; 
      
      if (srslte_pdsch_encode(&pdsch, &pdsch_cfg, &softbuffer_tx, data, slot_symbols)) {
        fprintf(stderr, "Error encoding PDSCH\n");
        goto quit;
      }
    }
    
    /* combine outputs */
    for (i=0;i<cell.nof_ports;i++) {
      for (j=0;j<SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp);j++) {
        if (i > 0) {
          slot_symbols[0][j] += slot_symbols[i][j];
        }
        ce[i][j] = 1;
      }
    }
    
  #ifdef DO_OFDM
    srslte_ofdm_tx_sf(&ofdm_tx, slot_symbols[0], sf_symbols);
  #endif
  } 
  int M=1;
  int r=0; 
  srslte_sch_set_max_noi(&pdsch.dl_sch, 10);
  gettimeofday(&t[1], NULL);
  for (i=0;i<M;i++) {
  #ifdef DO_OFDM
    srslte_ofdm_rx_sf(&ofdm_rx, sf_symbols, slot_symbols[1]);
  #endif
    srslte_softbuffer_rx_reset_tbs(&softbuffer_rx, grant.mcs.tbs);    
    r = srslte_pdsch_decode(&pdsch, &pdsch_cfg, &softbuffer_rx, slot_symbols[0], ce, 0, data);
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  printf("DECODED %s in %.2f (PHY bitrate=%.2f Mbps. Processing bitrate=%.2f Mbps)\n", r?"Error":"OK",
            (float) t[0].tv_usec/M, (float) grant.mcs.tbs/1000, (float) grant.mcs.tbs*M/t[0].tv_usec);                
  if (r) {
    ret = -1;
    goto quit;
  } 

  ret = 0;
quit:
  srslte_pdsch_free(&pdsch);
  srslte_softbuffer_tx_free(&softbuffer_tx);
  srslte_softbuffer_rx_free(&softbuffer_rx);
  
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
