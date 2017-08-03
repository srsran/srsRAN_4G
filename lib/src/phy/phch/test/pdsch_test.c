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
#include <srslte/phy/phch/ra.h>

#include "srslte/srslte.h"

// Enable to measure execution time
//#define DO_OFDM

#ifdef DO_OFDM
#define NOF_CE_SYMBOLS SRSLTE_SF_LEN_PRB(cell.nof_prb)
#else
#define NOF_CE_SYMBOLS SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp)
#endif

srslte_cell_t cell = {
  6,            // nof_prb
  1,            // nof_ports
  0,            // cell_id
  SRSLTE_CP_NORM,       // cyclic prefix
  SRSLTE_PHICH_NORM,    // PHICH length
  SRSLTE_PHICH_R_1_6    // PHICH resources
};

char mimo_type_str [32] = "single";
srslte_mimo_type_t mimo_type = SRSLTE_MIMO_TYPE_SINGLE_ANTENNA;
uint32_t cfi = 2;
uint32_t mcs = 0;
uint32_t mcs2 = 0;
uint32_t subframe = 1;
uint32_t rv_idx = 0;
uint32_t rv_idx2 = 1;
uint16_t rnti = 1234;
uint32_t nof_rx_antennas = 1;
uint32_t pmi = 0;
char *input_file = NULL; 

void usage(char *prog) {
  printf("Usage: %s [fmMcsrtRFpnwav] \n", prog);
  printf("\t-f read signal from file [Default generate it with pdsch_encode()]\n");
  printf("\t-m MCS [Default %d]\n", mcs);
  printf("\t-M MCS2 [Default %d]\n", mcs2);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-s subframe [Default %d]\n", subframe);
  printf("\t-r rv_idx [Default %d]\n", rv_idx);
  printf("\t-t rv_idx2 [Default %d]\n", rv_idx2);
  printf("\t-R rnti [Default %d]\n", rnti);
  printf("\t-F cfi [Default %d]\n", cfi);
  printf("\t-x Transmission mode [single|diversity|cdd|multiplex] [Default %s]\n", mimo_type_str);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-a nof_rx_antennas [Default %d]\n", nof_rx_antennas);
  printf("\t-p pmi (multiplex only)  [Default %d]\n", pmi);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "fmMcsrtRFpnavx")) != -1) {
    switch(opt) {
    case 'f':
      input_file = argv[optind];
      break;
    case 'm':
      mcs = atoi(argv[optind]);
      break;
    case 'M':
      mcs2 = (uint32_t) atoi(argv[optind]);
      break;
    case 's':
      subframe = atoi(argv[optind]);
      break;
    case 'r':
      rv_idx = atoi(argv[optind]);
      break;
    case 't':
      rv_idx2 = (uint32_t) atoi(argv[optind]);
      break;
    case 'R':
      rnti = atoi(argv[optind]);
      break;
    case 'F':
      cfi = atoi(argv[optind]);
      break;
    case 'x':
      strncpy(mimo_type_str, argv[optind], 32);
      break;
    case 'p':
      pmi = (uint32_t) atoi(argv[optind]);
      break;
    case 'n':
      cell.nof_prb = atoi(argv[optind]);
      break;
    case 'c':
      cell.id = atoi(argv[optind]);
      break;
    case 'a':
      nof_rx_antennas = (uint32_t) atoi(argv[optind]);
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

uint8_t *data[SRSLTE_MAX_CODEWORDS] = {NULL};
cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
srslte_softbuffer_rx_t softbuffers_rx[SRSLTE_MAX_CODEWORDS];
srslte_ra_dl_grant_t grant; 
srslte_pdsch_cfg_t pdsch_cfg; 
#ifdef DO_OFDM
cf_t *tx_sf_symbols[SRSLTE_MAX_PORTS];
cf_t *rx_sf_symbols[SRSLTE_MAX_PORTS];
#endif /* DO_OFDM */
cf_t *tx_slot_symbols[SRSLTE_MAX_PORTS];
cf_t *rx_slot_symbols[SRSLTE_MAX_PORTS];
srslte_pdsch_t pdsch_tx, pdsch_rx;
srslte_ofdm_t ofdm_tx, ofdm_rx; 

int main(int argc, char **argv) {
  uint32_t i, j, k;
  int ret = -1;
  struct timeval t[3];
  srslte_softbuffer_tx_t softbuffers_tx[SRSLTE_MAX_CODEWORDS];
  
  parse_args(argc,argv);

  /* Initialise to zeros */
  bzero(&pdsch_tx, sizeof(srslte_pdsch_t));
  bzero(&pdsch_rx, sizeof(srslte_pdsch_t));
  bzero(&pdsch_cfg, sizeof(srslte_pdsch_cfg_t));
  bzero(ce, sizeof(cf_t*)*SRSLTE_MAX_PORTS);
  bzero(tx_slot_symbols, sizeof(cf_t*)*SRSLTE_MAX_PORTS);
  bzero(rx_slot_symbols, sizeof(cf_t*)*SRSLTE_MAX_PORTS);
  bzero(softbuffers_tx, sizeof(srslte_softbuffer_tx_t)*SRSLTE_MAX_CODEWORDS);
  bzero(softbuffers_rx, sizeof(srslte_softbuffer_rx_t)*SRSLTE_MAX_CODEWORDS);

  /* Parse transmission mode */
  if (srslte_str2mimotype(mimo_type_str, &mimo_type)) {
    ERROR("Wrong transmission mode.");
    goto quit;
  }

  switch(mimo_type) {

    case SRSLTE_MIMO_TYPE_SINGLE_ANTENNA:
      cell.nof_ports = 1;
      break;
    case SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX:
    case SRSLTE_MIMO_TYPE_CDD:
      if (nof_rx_antennas < 2) {
        ERROR("At least two receiving antennas are required");
        goto quit;
      }
    case SRSLTE_MIMO_TYPE_TX_DIVERSITY:
    default:
      cell.nof_ports = 2;
      break;
  }

  srslte_ra_dl_dci_t dci;
  bzero(&dci, sizeof(srslte_ra_dl_dci_t));
  dci.type0_alloc.rbg_bitmask = 0xffffffff;

  /* If transport block 0 is enabled */
  if (mcs != 0 || rv_idx != 1) {
    dci.mcs_idx = mcs;
    dci.rv_idx = rv_idx;
    dci.tb_en[0] = true;
  }

  /* If transport block 0 is disabled */
  if (mcs2 != 0 || rv_idx2 != 1) {
    dci.mcs_idx_1 = mcs2;
    dci.rv_idx_1 = rv_idx2;
    dci.tb_en[1] = true;
  }

  /* Generate grant from DCI */
  if (srslte_ra_dl_dci_to_grant(&dci, cell.nof_prb, rnti, &grant)) {
    fprintf(stderr, "Error computing resource allocation\n");
    return ret;
  }



#ifdef DO_OFDM
  srslte_ofdm_tx_init(&ofdm_tx, cell.cp, cell.nof_prb);
  srslte_ofdm_rx_init(&ofdm_rx, cell.cp, cell.nof_prb);

  srslte_ofdm_set_normalize(&ofdm_tx, true);
  srslte_ofdm_set_normalize(&ofdm_rx, true);

  for (i = 0; i < cell.nof_ports; i++) {
    tx_sf_symbols[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
  }

  for (i = 0; i < nof_rx_antennas; i++) {
    rx_sf_symbols[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
  }
#endif /* DO_OFDM */

  /* Configure PDSCH */
  if (srslte_pdsch_cfg_multi(&pdsch_cfg, cell, &grant, cfi, subframe, rv_idx, rv_idx2, mimo_type, pmi)) {
    fprintf(stderr, "Error configuring PDSCH\n");
    goto quit;
  }

  /* init memory */
  for (i=0;i<SRSLTE_MAX_PORTS;i++) {
    for (j = 0; j < SRSLTE_MAX_PORTS; j++) {
      ce[i][j] = srslte_vec_malloc(sizeof(cf_t) * NOF_CE_SYMBOLS);
      if (!ce[i]) {
        perror("srslte_vec_malloc");
        goto quit;
      }
      for (k = 0; k < NOF_CE_SYMBOLS; k++) {
        ce[i][j][k] = (i == j) ? 1.0f : 0.0f;
      }
    }
    rx_slot_symbols[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp));
    if (!rx_slot_symbols[i]) {
      perror("srslte_vec_malloc");
      goto quit;
    }
  }


  if (grant.mcs.tbs) {
    data[0] = srslte_vec_malloc(sizeof(uint8_t) * grant.mcs.tbs);
    if (!data[0]) {
      perror("srslte_vec_malloc");
      goto quit;
    }
  }

  if (grant.mcs2.tbs) {
    data[1] = srslte_vec_malloc(sizeof(uint8_t) * grant.mcs2.tbs);
    if (!data[1]) {
      perror("srslte_vec_malloc");
      goto quit;
    }
  }

  if (srslte_pdsch_init_rx_multi(&pdsch_rx, cell, nof_rx_antennas)) {
    fprintf(stderr, "Error creating PDSCH object\n");
    goto quit;
  }

  srslte_pdsch_set_rnti(&pdsch_rx, rnti);

  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    if (srslte_softbuffer_rx_init(&softbuffers_rx[i], cell.nof_prb)) {
      fprintf(stderr, "Error initiating RX soft buffer\n");
      goto quit;
    }
  }

  INFO("         nof_prb=%d\n", cell.nof_prb);
  INFO("       nof_ports=%d\n", cell.nof_ports);
  INFO("              id=%d\n", cell.id);
  INFO("              cp=%s\n", srslte_cp_string(cell.cp));
  INFO("    phich_length=%d\n", (int) cell.phich_length);
  INFO(" phich_resources=%d\n", (int) cell.phich_resources);
  INFO("          nof_tb=%d\n", pdsch_cfg.grant.nof_tb);
  INFO("         nof_prb=%d\n", pdsch_cfg.grant.nof_prb);
  INFO("              Qm=%d\n", pdsch_cfg.grant.Qm);
  INFO("             Qm2=%d\n", pdsch_cfg.grant.Qm2);
  INFO("         mcs.idx=0x%X\n", pdsch_cfg.grant.mcs.idx);
  INFO("         mcs.tbs=%d\n", pdsch_cfg.grant.mcs.tbs);
  INFO("         mcs.mod=%s\n", srslte_mod_string(pdsch_cfg.grant.mcs.mod));
  INFO("        mcs2.idx=0x%X\n", pdsch_cfg.grant.mcs2.idx);
  INFO("        mcs2.tbs=%d\n", pdsch_cfg.grant.mcs2.tbs);
  INFO("        mcs2.mod=%s\n", srslte_mod_string(pdsch_cfg.grant.mcs2.mod));
  INFO("      nof_layers=%d\n", pdsch_cfg.nof_layers);
  INFO("              rv=%d\n", pdsch_cfg.rv);
  INFO("             rv2=%d\n", pdsch_cfg.rv2);
  INFO("          sf_idx=%d\n", pdsch_cfg.sf_idx);
  INFO("       mimo_type=%d\n", (int) pdsch_cfg.mimo_type);
  INFO("          nof_tb=%d\n", pdsch_cfg.grant.nof_tb);
  INFO("          lstart=%d\n", pdsch_cfg.nbits.lstart);
  INFO("        nof_bits=%d\n", pdsch_cfg.nbits.nof_bits);
  INFO("          nof_re=%d\n", pdsch_cfg.nbits.nof_re);
  INFO("        nof_symb=%d\n", pdsch_cfg.nbits.nof_symb);
  INFO("          lstart=%d\n", pdsch_cfg.nbits2.lstart);
  INFO("        nof_bits=%d\n", pdsch_cfg.nbits2.nof_bits);
  INFO("          nof_re=%d\n", pdsch_cfg.nbits2.nof_re);
  INFO("        nof_symb=%d\n", pdsch_cfg.nbits2.nof_symb);

  if (input_file) {
    srslte_filesource_t fsrc;
    if (srslte_filesource_init(&fsrc, input_file, SRSLTE_COMPLEX_FLOAT_BIN)) {
      fprintf(stderr, "Error opening file %s\n", input_file);
      goto quit;
    }
#ifdef DO_OFDM
    srslte_filesource_read(&fsrc, rx_slot_symbols, SRSLTE_SF_LEN_PRB(cell.nof_prb));
#else
    srslte_filesource_read(&fsrc, rx_slot_symbols[0], SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp));
#endif
    
    srslte_chest_dl_t chest; 
    if (srslte_chest_dl_init(&chest, cell)) {
      fprintf(stderr, "Error initializing equalizer\n");
      exit(-1);
    }
    srslte_chest_dl_estimate(&chest, rx_slot_symbols[0], ce[0], subframe);
    srslte_chest_dl_free(&chest);
    
    srslte_filesource_free(&fsrc);
  } else {

    if (srslte_pdsch_init_tx_multi(&pdsch_tx, cell)) {
      fprintf(stderr, "Error creating PDSCH object\n");
      goto quit;
    }

    srslte_pdsch_set_rnti(&pdsch_tx, rnti);

    for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      if (srslte_softbuffer_tx_init(&softbuffers_tx[i], cell.nof_prb)) {
        fprintf(stderr, "Error initiating TX soft buffer\n");
        goto quit;
      }
    }

    for (i = 0; i < cell.nof_ports; i++) {
      tx_slot_symbols[i] = calloc(SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp), sizeof(cf_t));
      if (!tx_slot_symbols[i]) {
        perror("srslte_vec_malloc");
        goto quit;
      }
    }

    for (i = 0; i < grant.mcs.tbs / 8; i++) {
      data[0][i] = (uint8_t) (rand() % 256);
    }

    for (i = 0; i < grant.mcs2.tbs / 8; i++) {
      data[1][i] = (uint8_t) (rand() % 256);
    }

    /*uint8_t databit[100000];
    srslte_bit_unpack_vector(data, databit, grant.mcs.tbs);
    srslte_vec_save_file("data_in", databit, grant.mcs.tbs);*/
    
    if (rv_idx) {
      /* Do 1st transmission for rv_idx!=0 */
      pdsch_cfg.rv = 0;
      pdsch_cfg.rv2 = 0;
      if (srslte_pdsch_encode_multi(&pdsch_tx, &pdsch_cfg, softbuffers_tx, data, rnti, tx_slot_symbols)) {
        fprintf(stderr, "Error encoding PDSCH\n");
        goto quit;
      }
    }
    pdsch_cfg.rv = rv_idx;
    pdsch_cfg.rv2 = rv_idx2;
    if (srslte_pdsch_encode_multi(&pdsch_tx, &pdsch_cfg, softbuffers_tx, data, rnti, tx_slot_symbols)) {
      fprintf(stderr, "Error encoding PDSCH\n");
      goto quit;
    }

  #ifdef DO_OFDM
    for (i = 0; i < cell.nof_ports; i++) {
      /* For each Tx antenna modulate OFDM */
      srslte_ofdm_tx_sf(&ofdm_tx, tx_slot_symbols[i], tx_sf_symbols[i]);
    }

    /* combine outputs */
    for (j = 0; j < nof_rx_antennas; j++) {
      for (k = 0; k < NOF_CE_SYMBOLS; k++) {
        rx_sf_symbols[j][k] = 0.0f;
        for (i = 0; i < cell.nof_ports; i++) {
          rx_sf_symbols[j][k] += tx_sf_symbols[i][k] * ce[i][j][k];
        }
      }
    }
  #else
    /* combine outputs */
    for (j = 0; j < nof_rx_antennas; j++) {
      for (k = 0; k < SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp); k++) {
        rx_slot_symbols[j][k] = 0.0f;
        for (i = 0; i < cell.nof_ports; i++) {
          rx_slot_symbols[j][k] += tx_slot_symbols[i][k] * ce[i][j][k];
        }
      }
    }
  #endif


  }
  int M=10;
  int r=0; 
  srslte_sch_set_max_noi(&pdsch_rx.dl_sch, 10);
  gettimeofday(&t[1], NULL);
  for (k = 0; k < M; k++) {
#ifdef DO_OFDM
    /* For each Rx antenna demodulate OFDM */
    for (i = 0; i < nof_rx_antennas; i++) {
      srslte_ofdm_rx_sf(&ofdm_rx, tx_sf_symbols[i], rx_slot_symbols[i]);
    }
#endif
    if (grant.mcs.tbs) {
      srslte_softbuffer_rx_reset_tbs(&softbuffers_rx[0], (uint32_t) grant.mcs.tbs);
    }
    if (grant.mcs2.tbs) {
      srslte_softbuffer_rx_reset_tbs(&softbuffers_rx[1], (uint32_t) grant.mcs2.tbs);
    }
    r = srslte_pdsch_decode_multi(&pdsch_rx, &pdsch_cfg, softbuffers_rx, rx_slot_symbols, ce, 0, rnti, data);
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  INFO("DECODED %s in %.2f (PHY bitrate=%.2f Mbps. Processing bitrate=%.2f Mbps)\n", r?"Error":"OK",
         (float) t[0].tv_usec/M, (float) (grant.mcs.tbs + grant.mcs2.tbs)/1000.0f,
         (float) (grant.mcs.tbs + grant.mcs2.tbs)*M/t[0].tv_usec);
  if (r) {
    ret = -1;
    goto quit;
  } 

  ret = 0;

quit:
  srslte_pdsch_free(&pdsch_tx);
  srslte_pdsch_free(&pdsch_rx);
  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    srslte_softbuffer_tx_free(&softbuffers_tx[i]);
    srslte_softbuffer_rx_free(&softbuffers_rx[i]);

    if (data[i]) {
      free(data[i]);
    }
  }

  for (i=0;i<SRSLTE_MAX_PORTS;i++) {
    for (j = 0; j < SRSLTE_MAX_PORTS; j++) {
      if (ce[i][j]) {
        free(ce[i][j]);
      }
    }
    if (tx_slot_symbols[i]) {
      free(tx_slot_symbols[i]);
    }
    if (rx_slot_symbols[i]) {
      free(rx_slot_symbols[i]);
    }
  }
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  exit(ret);
}
