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
  .nof_prb = 6,                         // nof_prb
  .nof_ports = 1,                       // nof_ports
  .id = 0,                              // cell_id
  .cp = SRSLTE_CP_NORM,                 // cyclic prefix
  .phich_length = SRSLTE_PHICH_NORM,    // PHICH length
  .phich_resources = SRSLTE_PHICH_R_1_6 // PHICH resources
};

srslte_uci_cfg_t uci_cfg = {
    .I_offset_cqi = 6,
    .I_offset_ri = 2,
    .I_offset_ack = 9,
};

srslte_uci_data_t uci_data_tx = {
    .uci_cqi = {0},
    .uci_cqi_len = 0,
    .uci_ri = 0,
    .uci_ri_len = 0,
    .uci_ack = 0,
    .uci_ack_2 = 0,
    .uci_ack_len = 0,
    .ri_periodic_report = false,
    .scheduling_request = false,
    .channel_selection = false,
    .cqi_ack = false
};

uint32_t cfi = 2;
uint32_t tbs = 0;
uint32_t subframe = 1;
srslte_mod_t modulation = SRSLTE_MOD_QPSK;
uint32_t rv_idx = 0;
uint32_t L_prb = 2; 
uint32_t n_prb = 0; 
int freq_hop = -1; 
int riv = -1; 
uint32_t mcs_idx = 0; 
srslte_cqi_value_t cqi_value;

void usage(char *prog) {
  printf("Usage: %s [csrnfvmtLNF] \n", prog);
  printf("\n\tCell specific parameters:\n");
  printf("\t\t-n number of PRB [Default %d]\n", cell.nof_prb);
  printf("\t\t-c cell id [Default %d]\n", cell.id);

  printf("\n\tGrant parameters:\n");
  printf("\t\t-m MCS index (0-28) [Default %d]\n", mcs_idx);
  printf("\t\t-L L_prb [Default %d]\n", L_prb);
  printf("\t\t-N n_prb [Default %d]\n", n_prb);
  printf("\t\t-F frequency hopping [Default %d]\n", freq_hop);
  printf("\t\t-R RIV [Default %d]\n", riv);
  printf("\t\t-r rv_idx (0-3) [Default %d]\n", rv_idx);
  printf("\t\t-f cfi [Default %d]\n", cfi);

  printf("\n\tCQI/RI/ACK Reporting indexes parameters:\n");
  printf("\t\t-p I_offset_cqi (0-15) [Default %d]\n", uci_cfg.I_offset_cqi);
  printf("\t\t-p I_offset_ri (0-15) [Default %d]\n", uci_cfg.I_offset_ri);
  printf("\t\t-p I_offset_ack (0-15) [Default %d]\n", uci_cfg.I_offset_ack);

  printf("\n\tCQI/RI/ACK Reporting contents:\n");
  printf("\t\t-p uci_cqi (zeros, ones, random) [Default zeros]\n");
  printf("\t\t-p uci_cqi_len (0-64) [Default %d]\n", uci_data_tx.uci_cqi_len);
  printf("\t\t-p uci_ri (0-1) (zeros, ones, random) [Default none]\n");
  printf("\t\t-p uci_ack (0-1) [Default none]\n");
  printf("\t\t-p uci_ack_2 (0-1) [Default none]\n");

  printf("\n\tOther parameters:\n");
  printf("\t\t-s subframe [Default %d]\n", subframe);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_extensive_param (char *param, char *arg) {
  int ext_code = SRSLTE_SUCCESS;
  if (!strcmp(param, "I_offset_cqi")) {
    uci_cfg.I_offset_cqi = (uint32_t) atoi(arg);
    if (uci_cfg.I_offset_cqi > 15) {
      ext_code = SRSLTE_ERROR;
    }
  } else if (!strcmp(param, "I_offset_ri")) {
    uci_cfg.I_offset_ri = (uint32_t) atoi(arg);
    if (uci_cfg.I_offset_ri > 15) {
      ext_code = SRSLTE_ERROR;
    }
  } else if (!strcmp(param, "I_offset_ack")) {
    uci_cfg.I_offset_ack = (uint32_t) atoi(arg);
    if (uci_cfg.I_offset_ack > 15) {
      ext_code = SRSLTE_ERROR;
    }
  } else if (!strcmp(param, "uci_cqi")) {
    if (!strcmp(arg, "wideband")) {
      cqi_value.type = SRSLTE_CQI_TYPE_WIDEBAND;
      cqi_value.wideband.wideband_cqi = (uint8_t) (rand() & 0x03);
      uci_data_tx.uci_cqi_len = (uint32_t) srslte_cqi_value_unpack(uci_data_tx.uci_cqi, &cqi_value);
    } else {
      ext_code = SRSLTE_ERROR;
    }
  } else if (!strcmp(param, "uci_cqi_len")) {
    uci_data_tx.uci_cqi_len = (uint32_t) atol(arg);
    if (uci_data_tx.uci_cqi_len >= SRSLTE_CQI_MAX_BITS) {
      ext_code = SRSLTE_ERROR;
    }
  } else if (!strcmp(param, "uci_ri")) {
    uci_data_tx.uci_ri = (uint8_t) atol(arg);
    if (uci_data_tx.uci_ri > 1) {
      ext_code = SRSLTE_ERROR;
    } else {
      uci_data_tx.uci_ri_len = 1;
    }
  } else if (!strcmp(param, "uci_ack")) {
    uci_data_tx.uci_ack = (uint8_t) atol(arg);
    if (uci_data_tx.uci_ack > 1) {
      ext_code = SRSLTE_ERROR;
    } else {
      uci_data_tx.uci_ack_len++;
      if (uci_data_tx.uci_ack_len > 2) {
        uci_data_tx.uci_ack_len = 2;
      }
    }
  } else if (!strcmp(param, "uci_ack_2")) {
    uci_data_tx.uci_ack_2 = (uint8_t) atol(arg);
    if (uci_data_tx.uci_ack_2 > 1) {
      ext_code = SRSLTE_ERROR;
    } else {
      uci_data_tx.uci_ack_len++;
      if (uci_data_tx.uci_ack_len > 2) {
        uci_data_tx.uci_ack_len = 2;
      }
    }
  } else {
    ext_code = SRSLTE_ERROR;
  }

  if (ext_code) {
    fprintf(stderr, "Error parsing parameter '%s' and argument '%s'\n", param, arg);
    exit(ext_code);
  }
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "msLNRFrncpv")) != -1) {
    switch(opt) {
    case 'm':
      mcs_idx = atoi(argv[optind]);
      break;
    case 's':
      subframe = atoi(argv[optind]);
      break;
    case 'L':
      L_prb = atoi(argv[optind]);
      break;
    case 'N':
      n_prb = atoi(argv[optind]);
      break;
    case 'R':
      riv = atoi(argv[optind]);
      break;
    case 'F':
      freq_hop = atoi(argv[optind]);
      break;
    case 'r':
      rv_idx = atoi(argv[optind]);
      break;
    case 'n':
      cell.nof_prb = atoi(argv[optind]);
      break;
    case 'c':
      cell.id = atoi(argv[optind]);
      break;
    case 'p':
      parse_extensive_param(argv[optind], argv[optind + 1]);
      optind++;
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
  srslte_pusch_t pusch_tx;
  srslte_pusch_t pusch_rx;
  uint8_t *data = NULL;
  cf_t *sf_symbols = NULL;
  cf_t *ce = NULL; 
  int ret = -1;
  struct timeval t[3];
  srslte_pusch_cfg_t cfg; 
  srslte_softbuffer_tx_t softbuffer_tx;
  srslte_softbuffer_rx_t softbuffer_rx; 
  
  parse_args(argc,argv);

  bzero(&cfg, sizeof(srslte_pusch_cfg_t));

  srslte_dft_load();

  srslte_ra_ul_dci_t dci;
  dci.freq_hop_fl = freq_hop;
  if (riv < 0) {
    dci.type2_alloc.L_crb = L_prb; 
    dci.type2_alloc.RB_start = n_prb;    
  } else {
    dci.type2_alloc.riv = riv;
  }
  dci.mcs_idx = mcs_idx;
  
  srslte_ra_ul_grant_t grant; 
  if (srslte_ra_ul_dci_to_grant(&dci, cell.nof_prb, 0, &grant)) {
    fprintf(stderr, "Error computing resource allocation\n");
    return ret;
  }
      
  srslte_pusch_hopping_cfg_t ul_hopping; 
  ul_hopping.n_sb = 1; 
  ul_hopping.hopping_offset = 0;
  ul_hopping.hop_mode = SRSLTE_PUSCH_HOP_MODE_INTER_SF;
  
  if (srslte_pusch_init_ue(&pusch_tx, cell.nof_prb)) {
    fprintf(stderr, "Error creating PUSCH object\n");
    goto quit;
  }
  if (srslte_pusch_set_cell(&pusch_tx, cell)) {
    fprintf(stderr, "Error creating PUSCH object\n");
    goto quit;
  }
  if (srslte_pusch_init_enb(&pusch_rx, cell.nof_prb)) {
    fprintf(stderr, "Error creating PUSCH object\n");
    goto quit;
  }
  if (srslte_pusch_set_cell(&pusch_rx, cell)) {
    fprintf(stderr, "Error creating PUSCH object\n");
    goto quit;
  }

  /* Configure PUSCH */
    
  if (srslte_pusch_cfg(&pusch_tx, &cfg, &grant, &uci_cfg, &ul_hopping, NULL, subframe, 0, 0)) {
    fprintf(stderr, "Error configuring PDSCH\n");
    exit(-1);
  }
  if (srslte_pusch_cfg(&pusch_rx, &cfg, &grant, &uci_cfg, &ul_hopping, NULL, subframe, 0, 0)) {
    fprintf(stderr, "Error configuring PDSCH\n");
    exit(-1);
  }

  uint16_t rnti = 1234; 
  srslte_pusch_set_rnti(&pusch_tx, rnti);
  srslte_pusch_set_rnti(&pusch_rx, rnti);

  srslte_uci_data_t uci_data_rx;
  memcpy(&uci_data_rx, &uci_data_tx, sizeof(srslte_uci_data_t));
    
  uint32_t nof_re = SRSLTE_NRE*cell.nof_prb*2*SRSLTE_CP_NSYMB(cell.cp);
  sf_symbols = srslte_vec_malloc(sizeof(cf_t) * nof_re);
  if (!sf_symbols) {
    perror("malloc");
    exit(-1);
  }
  
  data = srslte_vec_malloc(sizeof(uint8_t) * (cfg.grant.mcs.tbs+24));
  if (!data) {
    perror("malloc");
    exit(-1);
  }
  
  for (uint32_t i=0;i<cfg.grant.mcs.tbs/8;i++) {
    data[i] = 1;
  }

  if (srslte_softbuffer_tx_init(&softbuffer_tx, 100)) {
    fprintf(stderr, "Error initiating soft buffer\n");
    goto quit;
  }
  srslte_softbuffer_tx_reset(&softbuffer_tx);
  if (srslte_softbuffer_rx_init(&softbuffer_rx, 100)) {
    fprintf(stderr, "Error initiating soft buffer\n");
    goto quit;
  }
  srslte_softbuffer_rx_reset(&softbuffer_rx);
  
  uint32_t ntrials = 100; 

  if (srslte_pusch_encode(&pusch_tx, &cfg, &softbuffer_tx, data, uci_data_tx, rnti, sf_symbols)) {
    fprintf(stderr, "Error encoding TB\n");
    exit(-1);
  }
  if (rv_idx > 0) {
    cfg.rv = rv_idx; 
    if (srslte_pusch_encode(&pusch_tx, &cfg, &softbuffer_tx, data, uci_data_tx, rnti, sf_symbols)) {
      fprintf(stderr, "Error encoding TB\n");
      exit(-1);
    }
  }

  ce = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp));
  if (!ce) {
    perror("srslte_vec_malloc");
    goto quit;
  }
  for (int j=0;j<SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp);j++) {
    ce[j] = 1;
  }
  
  gettimeofday(&t[1], NULL);
  int r = srslte_pusch_decode(&pusch_rx,
                              &cfg,
                              &softbuffer_rx,
                              sf_symbols,
                              ce,
                              0,
                              rnti,
                              data,
                              (uci_data_tx.uci_cqi_len) ? &cqi_value : NULL,
                              &uci_data_rx);
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  if (r) {
    printf("Error decoding\n");
    ret = -1;
  } else {
    ret = 0;
    printf("DECODED OK in %d:%d (TBS: %d bits, TX: %.2f Mbps, Processing: %.2f Mbps)\n", (int) t[0].tv_sec, 
           (int) t[0].tv_usec/ntrials, 
           cfg.grant.mcs.tbs,
           (float) cfg.grant.mcs.tbs/1000,
           (float) cfg.grant.mcs.tbs/t[0].tv_usec*ntrials);    
  }
  if (uci_data_tx.uci_ack_len) {
    if (uci_data_tx.uci_ack != uci_data_rx.uci_ack) {
      printf("UCI ACK bit error: %d != %d\n", uci_data_tx.uci_ack, uci_data_rx.uci_ack);
      ret = SRSLTE_ERROR;
    }
  }
  if (uci_data_tx.uci_ack_len > 1) {
    if (uci_data_tx.uci_ack_2 != uci_data_rx.uci_ack_2) {
      printf("UCI ACK 2 bit error: %d != %d\n", uci_data_tx.uci_ack_2, uci_data_rx.uci_ack_2);
      ret = SRSLTE_ERROR;
    }
  }
  if (uci_data_tx.uci_ri_len) {
    if (uci_data_tx.uci_ri != uci_data_rx.uci_ri) {
      printf("UCI RI bit error: %d != %d\n", uci_data_tx.uci_ri, uci_data_rx.uci_ri);
      ret = SRSLTE_ERROR;
    }
  }
  if (uci_data_tx.uci_cqi_len) {
    if (memcmp(uci_data_tx.uci_cqi, uci_data_rx.uci_cqi, uci_data_tx.uci_cqi_len)) {
      printf("cqi_tx=");
      srslte_vec_fprint_b(stdout, uci_data_tx.uci_cqi, uci_data_tx.uci_cqi_len);
      printf("cqi_rx=");
      srslte_vec_fprint_b(stdout, uci_data_rx.uci_cqi, uci_data_rx.uci_cqi_len);
      ret = SRSLTE_ERROR;
    }
  }

quit:
  srslte_pusch_free(&pusch_tx);
  srslte_pusch_free(&pusch_rx);
  srslte_softbuffer_tx_free(&softbuffer_tx);
  srslte_softbuffer_rx_free(&softbuffer_rx);
  
  if (sf_symbols) {
    free(sf_symbols);
  }
  if (data) {
    free(data);
  }
  if (ce) {
    free(ce);
  }
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  srslte_dft_exit();
  exit(ret);
}
