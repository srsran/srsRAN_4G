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
#include <unistd.h>
#include <sys/time.h>
#include <srslte/phy/phch/ra.h>

#include "srslte/srslte.h"

// Enable to measure execution time
#define DO_OFDM

#ifdef DO_OFDM
#define NOF_CE_SYMBOLS SRSLTE_SF_LEN_PRB(cell.nof_prb)
#else
#define NOF_CE_SYMBOLS SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp)
#endif

srslte_cell_t cell = {
  100,            // nof_prb
  1,            // nof_ports
  1,            // cell_id
  SRSLTE_CP_EXT,       // cyclic prefix
  SRSLTE_PHICH_NORM,    // PHICH length
  SRSLTE_PHICH_R_1_6    // PHICH resources
};

char mimo_type_str [32] = "single";
srslte_mimo_type_t mimo_type = SRSLTE_MIMO_TYPE_SINGLE_ANTENNA;
uint32_t cfi = 2;
uint32_t mcs_idx = 2;
uint32_t subframe = 1;
int rv_idx[SRSLTE_MAX_CODEWORDS] = {0, 1};
uint16_t rnti = 1234;
uint32_t nof_rx_antennas = 1;
uint32_t pmi = 0;
char *input_file = NULL; 
uint32_t mbsfn_area_id = 1;
uint32_t non_mbsfn_region = 2;
void usage(char *prog) {
  printf("Usage: %s [fmMcsrtRFpnwav] \n", prog);
  printf("\t-f read signal from file [Default generate it with pdsch_encode()]\n");
  printf("\t-m MCS [Default %d]\n", mcs_idx);
  printf("\t-M mbsfn area id [Default %d]\n", mbsfn_area_id);
  printf("\t-N non mbsfn region [Default %d]\n", non_mbsfn_region);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-s subframe [Default %d]\n", subframe);
  printf("\t-r rv_idx [Default %d]\n", rv_idx[0]);
  printf("\t-R rnti [Default %d]\n", rnti);
  printf("\t-F cfi [Default %d]\n", cfi);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-a nof_rx_antennas [Default %d]\n", nof_rx_antennas);
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
      mcs_idx = (uint32_t) atoi(argv[optind]);
      break;
    case 's':
      subframe = atoi(argv[optind]);
      break;
    case 'r':
      rv_idx[0] = (uint32_t) atoi(argv[optind]);
      break;
    case 'R':
      rnti = atoi(argv[optind]);
      break;
    case 'F':
      cfi = atoi(argv[optind]);
      break;
    case 'x':
      strncpy(mimo_type_str, argv[optind], 31);
      mimo_type_str[31] = 0;
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

static uint8_t *data_tx[SRSLTE_MAX_CODEWORDS] = {NULL};
static uint8_t *data_rx[SRSLTE_MAX_CODEWORDS] = {NULL};
cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
srslte_softbuffer_rx_t *softbuffers_rx[SRSLTE_MAX_CODEWORDS];
srslte_ra_dl_grant_t grant; 
#ifdef DO_OFDM
cf_t *tx_sf_symbols[SRSLTE_MAX_PORTS];
cf_t *rx_sf_symbols[SRSLTE_MAX_PORTS];
#endif /* DO_OFDM */
cf_t *tx_slot_symbols[SRSLTE_MAX_PORTS];
cf_t *rx_slot_symbols[SRSLTE_MAX_PORTS];
srslte_pmch_t pmch_tx, pmch_rx;
srslte_pdsch_cfg_t  pmch_cfg;
srslte_ofdm_t ifft_mbsfn[SRSLTE_MAX_PORTS], fft_mbsfn[SRSLTE_MAX_PORTS];

int main(int argc, char **argv) {
  uint32_t i, j, k;
  int ret = -1;
  struct timeval t[3];
  srslte_softbuffer_tx_t *softbuffers_tx[SRSLTE_MAX_CODEWORDS];
  int M=1;
  
  parse_args(argc,argv);
  /* Initialise to zeros */
  bzero(&pmch_tx, sizeof(srslte_pmch_t));
  bzero(&pmch_rx, sizeof(srslte_pmch_t));
  bzero(&pmch_cfg, sizeof(srslte_pdsch_cfg_t));
  bzero(ce, sizeof(cf_t*)*SRSLTE_MAX_PORTS);
  bzero(tx_slot_symbols, sizeof(cf_t*)*SRSLTE_MAX_PORTS);
  bzero(rx_slot_symbols, sizeof(cf_t*)*SRSLTE_MAX_PORTS);
  bzero(t, 3 * sizeof(struct timeval));

  cell.nof_ports = 1;

  srslte_ra_dl_dci_t dci;
  bzero(&dci, sizeof(srslte_ra_dl_dci_t));
  dci.type0_alloc.rbg_bitmask = 0xffffffff;
  

  /* If transport block 0 is enabled */
    grant.tb_en[0] = true;
    grant.tb_en[1] = false;
    grant.mcs[0].idx = mcs_idx;

    grant.nof_prb = cell.nof_prb;
    grant.sf_type = SRSLTE_SF_MBSFN;

    srslte_dl_fill_ra_mcs(&grant.mcs[0], cell.nof_prb);
    grant.Qm[0] = srslte_mod_bits_x_symbol(grant.mcs[0].mod);
    for(int i = 0; i < 2; i++){
      for(int j = 0; j < grant.nof_prb; j++){
        grant.prb_idx[i][j] = true;
      }
    }

  /* init memory */
  for (i=0;i<SRSLTE_MAX_PORTS;i++) {
    for (j = 0; j < SRSLTE_MAX_PORTS; j++) {
      ce[i][j] = srslte_vec_malloc(sizeof(cf_t) * NOF_CE_SYMBOLS);
      if (!ce[i][j]) {
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

  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    softbuffers_tx[i] = calloc(sizeof(srslte_softbuffer_tx_t), 1);
    if (!softbuffers_tx[i]) {
      fprintf(stderr, "Error allocating TX soft buffer\n");
    }

    if (srslte_softbuffer_tx_init(softbuffers_tx[i], cell.nof_prb)) {
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

  for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    if (grant.tb_en[i]) {
      data_tx[i] = srslte_vec_malloc(sizeof(uint8_t) * grant.mcs[i].tbs);
      if (!data_tx[i]) {
        perror("srslte_vec_malloc");
        goto quit;
      }
      bzero(data_tx[i], sizeof(uint8_t) * grant.mcs[i].tbs);

      data_rx[i] = srslte_vec_malloc(sizeof(uint8_t) * grant.mcs[i].tbs);
      if (!data_rx[i]) {
        perror("srslte_vec_malloc");
        goto quit;
      }
      bzero(data_rx[i], sizeof(uint8_t) * grant.mcs[i].tbs);

    }
  }



  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    softbuffers_rx[i] = calloc(sizeof(srslte_softbuffer_rx_t), 1);
    if (!softbuffers_rx[i]) {
      fprintf(stderr, "Error allocating RX soft buffer\n");
      goto quit;
    }

    if (srslte_softbuffer_rx_init(softbuffers_rx[i], cell.nof_prb)) {
      fprintf(stderr, "Error initiating RX soft buffer\n");
      goto quit;
    }
  }

#ifdef DO_OFDM

  for (i = 0; i < cell.nof_ports; i++) {
    tx_sf_symbols[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));

    if (srslte_ofdm_tx_init_mbsfn(&ifft_mbsfn[i], SRSLTE_CP_EXT, tx_slot_symbols[i], tx_sf_symbols[i], cell.nof_prb)) {
      fprintf(stderr, "Error creating iFFT object\n");
      exit(-1);
    }

    srslte_ofdm_set_non_mbsfn_region(&ifft_mbsfn[i], non_mbsfn_region);
    srslte_ofdm_set_normalize(&ifft_mbsfn[i], true);
  }

  for (i = 0; i < nof_rx_antennas; i++) {
    rx_sf_symbols[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));

    if (srslte_ofdm_rx_init_mbsfn(&fft_mbsfn[i], SRSLTE_CP_EXT, rx_sf_symbols[i], rx_slot_symbols[i], cell.nof_prb)) {
      fprintf(stderr, "Error creating iFFT object\n");
      exit(-1);
    }

    srslte_ofdm_set_non_mbsfn_region(&fft_mbsfn[i], non_mbsfn_region);
    srslte_ofdm_set_normalize(&fft_mbsfn[i], true);
  }




#endif /* DO_OFDM */

  /* Configure PDSCH */
  
  if (srslte_pmch_cfg(&pmch_cfg, cell, &grant, cfi, subframe)) {
    fprintf(stderr, "Error configuring PMCH\n");
    exit(-1);
  }
  
  if (srslte_pmch_cfg(&pmch_cfg, cell, &grant, cfi, subframe)) {
      fprintf(stderr, "Error configuring PMCH\n");
      exit(-1);
    }
    

  INFO(" Global:\n");
  INFO("         nof_prb=%d\n", cell.nof_prb);
  INFO("       nof_ports=%d\n", cell.nof_ports);
  INFO("              id=%d\n", cell.id);
  INFO("              cp=%s\n", srslte_cp_string(cell.cp));
  INFO("    phich_length=%d\n", (int) cell.phich_length);
  INFO(" phich_resources=%d\n", (int) cell.phich_resources);
  INFO("         nof_prb=%d\n", pmch_cfg.grant.nof_prb);
  INFO("          sf_idx=%d\n", pmch_cfg.sf_idx);
  INFO("       mimo_type=%s\n", srslte_mimotype2str(pmch_cfg.mimo_type));
  INFO("      nof_layers=%d\n", pmch_cfg.nof_layers);
  INFO("          nof_tb=%d\n", SRSLTE_RA_DL_GRANT_NOF_TB(&pmch_cfg.grant));  

  INFO("              Qm=%d\n", pmch_cfg.grant.Qm[0]);
  INFO("         mcs.idx=0x%X\n", pmch_cfg.grant.mcs[0].idx);
  INFO("         mcs.tbs=%d\n", pmch_cfg.grant.mcs[0].tbs);
  INFO("         mcs.mod=%s\n", srslte_mod_string(pmch_cfg.grant.mcs[0].mod));
  INFO("              rv=%d\n", pmch_cfg.rv[0]);
  INFO("          lstart=%d\n", pmch_cfg.nbits[0].lstart);
  INFO("        nof_bits=%d\n", pmch_cfg.nbits[0].nof_bits);
  INFO("          nof_re=%d\n", pmch_cfg.nbits[0].nof_re);
  INFO("        nof_symb=%d\n", pmch_cfg.nbits[0].nof_symb);



  if (srslte_pmch_init(&pmch_tx, cell.nof_prb)) {
    fprintf(stderr, "Error creating PMCH object\n");
  }
  srslte_pmch_set_area_id(&pmch_tx, mbsfn_area_id);

  if (srslte_pmch_init(&pmch_rx, cell.nof_prb)) {
    fprintf(stderr, "Error creating PMCH object\n");
  }
  srslte_pmch_set_area_id(&pmch_rx, mbsfn_area_id);




  for (int tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
    if (grant.tb_en[tb]) {
      for (int byte = 0; byte < grant.mcs[tb].tbs / 8; byte++) {
        data_tx[tb][byte] = (uint8_t) (rand() % 256);
      }
    }
  }

  if (srslte_pmch_encode(&pmch_tx, &pmch_cfg, softbuffers_tx[0], data_tx[0], mbsfn_area_id, tx_slot_symbols)) {
    fprintf(stderr, "Error encoding PDSCH\n");
    exit(-1);
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  printf("ENCODED in %.2f (PHY bitrate=%.2f Mbps. Processing bitrate=%.2f Mbps)\n",
         (float) t[0].tv_usec/M, (float) (grant.mcs[0].tbs + grant.mcs[1].tbs)/1000.0f,
         (float) (grant.mcs[0].tbs + grant.mcs[1].tbs)*M/t[0].tv_usec);

#ifdef DO_OFDM
  for (i = 0; i < cell.nof_ports; i++) {
    /* For each Tx antenna modulate OFDM */
    srslte_ofdm_tx_sf(&ifft_mbsfn[i]);
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


  
  int r=0;
  gettimeofday(&t[1], NULL);

#ifdef DO_OFDM
    /* For each Rx antenna demodulate OFDM */
    for (i = 0; i < nof_rx_antennas; i++) {
      srslte_ofdm_rx_sf(&fft_mbsfn[i]);
    }
#endif
  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    if (grant.tb_en[i]) {
      srslte_softbuffer_rx_reset_tbs(softbuffers_rx[i], (uint32_t) grant.mcs[i].tbs);
    }
  }

  r = srslte_pmch_decode(&pmch_rx, &pmch_cfg, softbuffers_rx[0],rx_slot_symbols[0], ce[0],0,mbsfn_area_id, data_rx[0]);
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  printf("DECODED %s in %.2f (PHY bitrate=%.2f Mbps. Processing bitrate=%.2f Mbps)\n", r?"Error":"OK",
         (float) t[0].tv_usec/M, (float) (grant.mcs[0].tbs + grant.mcs[1].tbs)/1000.0f,
         (float) (grant.mcs[0].tbs + grant.mcs[1].tbs)*M/t[0].tv_usec);

  /* If there is an error in PDSCH decode */
  if (r) {
    ret = -1;
    goto quit;
  }

  /* Check Tx and Rx bytes */
  for (int tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
    if (grant.tb_en[tb]) {
      for (int byte = 0; byte < grant.mcs[tb].tbs / 8; byte++) {
        if (data_tx[tb][byte] != data_rx[tb][byte]) {
          ERROR("Found BYTE error in TB %d (%02X != %02X), quiting...", tb, data_tx[tb][byte], data_rx[tb][byte]);
          ret = SRSLTE_ERROR;
          goto quit;
        }
      }
    }
  }

  ret = SRSLTE_SUCCESS;

quit:
  srslte_pmch_free(&pmch_tx);
  srslte_pmch_free(&pmch_rx);
  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    srslte_softbuffer_tx_free(softbuffers_tx[i]);
    if (softbuffers_tx[i]) {
      free(softbuffers_tx[i]);
    }

    srslte_softbuffer_rx_free(softbuffers_rx[i]);
    if (softbuffers_rx[i]) {
      free(softbuffers_rx[i]);
    }

    if (data_tx[i]) {
      free(data_tx[i]);
    }

    if (data_rx[i]) {
      free(data_rx[i]);
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

  srslte_dft_exit();

  exit(ret);
}
