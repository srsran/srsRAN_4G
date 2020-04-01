/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include <srslte/phy/utils/random.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "srslte/srslte.h"

// Enable to measure execution time
#define DO_OFDM

#ifdef DO_OFDM
#define NOF_CE_SYMBOLS SRSLTE_SF_LEN_PRB(cell.nof_prb)
#else
#define NOF_CE_SYMBOLS SRSLTE_NOF_RE(cell)
#endif

srslte_cell_t cell = {
    100,                // nof_prb
    1,                  // nof_ports
    1,                  // cell_id
    SRSLTE_CP_EXT,      // cyclic prefix
    SRSLTE_PHICH_NORM,  // PHICH length
    SRSLTE_PHICH_R_1_6, // PHICH resources
    SRSLTE_FDD,

};

uint32_t cfi                          = 2;
uint32_t mcs_idx                      = 2;
uint32_t subframe                     = 1;
int      rv_idx[SRSLTE_MAX_CODEWORDS] = {0, 1};
uint16_t rnti                         = 1234;
uint32_t nof_rx_antennas              = 1;
uint32_t pmi                          = 0;
char*    input_file                   = NULL;
uint32_t mbsfn_area_id                = 1;
uint32_t non_mbsfn_region             = 2;

void usage(char* prog)
{
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

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "fmMcsrtRFpnav")) != -1) {
    switch (opt) {
      case 'f':
        input_file = argv[optind];
        break;
      case 'm':
        mcs_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        subframe = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        rv_idx[0] = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'R':
        rnti = (uint16_t)strtol(argv[optind], NULL, 10);
        break;
      case 'F':
        cfi = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        pmi = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'a':
        nof_rx_antennas = (uint32_t)strtol(argv[optind], NULL, 10);
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

int main(int argc, char** argv)
{
  uint32_t       i, j, k;
  int            ret = -1;
  struct timeval t[3];
  int            M = 1;

  static uint8_t*         data_tx[SRSLTE_MAX_CODEWORDS] = {NULL};
  static uint8_t*         data_rx[SRSLTE_MAX_CODEWORDS] = {NULL};
  srslte_softbuffer_rx_t* softbuffers_rx[SRSLTE_MAX_CODEWORDS];
  srslte_softbuffer_tx_t* softbuffers_tx[SRSLTE_MAX_CODEWORDS];
  srslte_random_t         random = srslte_random_init(0);
#ifdef DO_OFDM
  cf_t* tx_sf_symbols[SRSLTE_MAX_PORTS] = {};
  cf_t* rx_sf_symbols[SRSLTE_MAX_PORTS] = {};
#endif /* DO_OFDM */
  cf_t* tx_slot_symbols[SRSLTE_MAX_PORTS] = {};
  cf_t* rx_slot_symbols[SRSLTE_MAX_PORTS] = {};

  srslte_chest_dl_res_t chest_dl_res;
  srslte_pmch_t         pmch;
  srslte_pmch_cfg_t     pmch_cfg;
  srslte_ofdm_t         ifft_mbsfn[SRSLTE_MAX_PORTS], fft_mbsfn[SRSLTE_MAX_PORTS];

  parse_args(argc, argv);
  /* Initialise to zeros */
  bzero(&pmch, sizeof(srslte_pmch_t));
  bzero(tx_slot_symbols, sizeof(cf_t*) * SRSLTE_MAX_PORTS);
  bzero(rx_slot_symbols, sizeof(cf_t*) * SRSLTE_MAX_PORTS);
  bzero(t, 3 * sizeof(struct timeval));

  cell.nof_ports = 1;

  /* init memory */
  srslte_chest_dl_res_init(&chest_dl_res, cell.nof_prb);
  srslte_chest_dl_res_set_identity(&chest_dl_res);

  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    rx_slot_symbols[i] = srslte_vec_cf_malloc(SRSLTE_NOF_RE(cell));
    if (!rx_slot_symbols[i]) {
      perror("srslte_vec_malloc");
      goto quit;
    }
  }

  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    softbuffers_tx[i] = calloc(sizeof(srslte_softbuffer_tx_t), 1);
    if (!softbuffers_tx[i]) {
      ERROR("Error allocating TX soft buffer\n");
    }

    if (srslte_softbuffer_tx_init(softbuffers_tx[i], cell.nof_prb)) {
      ERROR("Error initiating TX soft buffer\n");
      goto quit;
    }
  }

  for (i = 0; i < cell.nof_ports; i++) {
    tx_slot_symbols[i] = calloc(SRSLTE_NOF_RE(cell), sizeof(cf_t));
    if (!tx_slot_symbols[i]) {
      perror("srslte_vec_malloc");
      goto quit;
    }
  }

  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    softbuffers_rx[i] = calloc(sizeof(srslte_softbuffer_rx_t), 1);
    if (!softbuffers_rx[i]) {
      ERROR("Error allocating RX soft buffer\n");
      goto quit;
    }

    if (srslte_softbuffer_rx_init(softbuffers_rx[i], cell.nof_prb)) {
      ERROR("Error initiating RX soft buffer\n");
      goto quit;
    }
  }

#ifdef DO_OFDM

  for (i = 0; i < cell.nof_ports; i++) {
    tx_sf_symbols[i] = srslte_vec_cf_malloc(SRSLTE_SF_LEN_PRB(cell.nof_prb));
    srslte_vec_cf_zero(tx_sf_symbols[i], SRSLTE_SF_LEN_PRB(cell.nof_prb));
    if (srslte_ofdm_tx_init_mbsfn(&ifft_mbsfn[i], SRSLTE_CP_EXT, tx_slot_symbols[i], tx_sf_symbols[i], cell.nof_prb)) {
      ERROR("Error creating iFFT object\n");
      exit(-1);
    }

    srslte_ofdm_set_non_mbsfn_region(&ifft_mbsfn[i], non_mbsfn_region);
    srslte_ofdm_set_normalize(&ifft_mbsfn[i], true);
  }

  for (i = 0; i < nof_rx_antennas; i++) {
    rx_sf_symbols[i] = srslte_vec_cf_malloc(SRSLTE_SF_LEN_PRB(cell.nof_prb));
    srslte_vec_cf_zero(rx_sf_symbols[i], SRSLTE_SF_LEN_PRB(cell.nof_prb));
    if (srslte_ofdm_rx_init_mbsfn(&fft_mbsfn[i], SRSLTE_CP_EXT, rx_sf_symbols[i], rx_slot_symbols[i], cell.nof_prb)) {
      ERROR("Error creating iFFT object\n");
      exit(-1);
    }

    srslte_ofdm_set_non_mbsfn_region(&fft_mbsfn[i], non_mbsfn_region);
    srslte_ofdm_set_normalize(&fft_mbsfn[i], true);
  }

#endif /* DO_OFDM */

  /* Configure PDSCH */

  srslte_dl_sf_cfg_t dl_sf;
  ZERO_OBJECT(dl_sf);

  dl_sf.cfi     = cfi;
  dl_sf.tti     = subframe;
  dl_sf.sf_type = SRSLTE_SF_MBSFN;

  ZERO_OBJECT(pmch_cfg);
  pmch_cfg.area_id                     = mbsfn_area_id;
  pmch_cfg.pdsch_cfg.softbuffers.tx[0] = softbuffers_tx[0];

  srslte_dci_dl_t dci;
  ZERO_OBJECT(dci);
  dci.rnti                    = SRSLTE_MRNTI;
  dci.format                  = SRSLTE_DCI_FORMAT1;
  dci.alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
  dci.type0_alloc.rbg_bitmask = 0xffffffff;
  dci.tb[0].mcs_idx           = mcs_idx;
  SRSLTE_DCI_TB_DISABLE(dci.tb[1]);
  srslte_ra_dl_dci_to_grant(&cell, &dl_sf, SRSLTE_TM1, false, &dci, &pmch_cfg.pdsch_cfg.grant);

  for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    if (pmch_cfg.pdsch_cfg.grant.tb[i].enabled) {
      data_tx[i] = srslte_vec_u8_malloc((uint32_t)pmch_cfg.pdsch_cfg.grant.tb[i].tbs);
      if (!data_tx[i]) {
        perror("srslte_vec_malloc");
        goto quit;
      }
      bzero(data_tx[i], sizeof(uint8_t) * pmch_cfg.pdsch_cfg.grant.tb[i].tbs);

      data_rx[i] = srslte_vec_u8_malloc((uint32_t)pmch_cfg.pdsch_cfg.grant.tb[i].tbs);
      if (!data_rx[i]) {
        perror("srslte_vec_malloc");
        goto quit;
      }
      bzero(data_rx[i], sizeof(uint8_t) * pmch_cfg.pdsch_cfg.grant.tb[i].tbs);
    }
  }

  INFO(" Global:\n");
  INFO("         nof_prb=%d\n", cell.nof_prb);
  INFO("       nof_ports=%d\n", cell.nof_ports);
  INFO("              id=%d\n", cell.id);
  INFO("              cp=%s\n", srslte_cp_string(cell.cp));
  INFO("    phich_length=%d\n", (int)cell.phich_length);
  INFO(" phich_resources=%d\n", (int)cell.phich_resources);
  INFO("         nof_prb=%d\n", pmch_cfg.pdsch_cfg.grant.nof_prb);
  INFO("          sf_idx=%d\n", subframe);
  INFO("          nof_tb=%d\n", pmch_cfg.pdsch_cfg.grant.nof_tb);

  INFO("         mcs.idx=0x%X\n", pmch_cfg.pdsch_cfg.grant.tb[0].mcs_idx);
  INFO("         mcs.tbs=%d\n", pmch_cfg.pdsch_cfg.grant.tb[0].tbs);
  INFO("         mcs.mod=%s\n", srslte_mod_string(pmch_cfg.pdsch_cfg.grant.tb[0].mod));
  INFO("              rv=%d\n", pmch_cfg.pdsch_cfg.grant.tb[0].rv);
  INFO("        nof_bits=%d\n", pmch_cfg.pdsch_cfg.grant.tb[0].nof_bits);
  INFO("          nof_re=%d\n", pmch_cfg.pdsch_cfg.grant.nof_re);

  if (srslte_pmch_init(&pmch, cell.nof_prb, 1)) {
    ERROR("Error creating PMCH object\n");
  }
  srslte_pmch_set_area_id(&pmch, mbsfn_area_id);

  for (int tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
    if (pmch_cfg.pdsch_cfg.grant.tb[tb].enabled) {
      for (int byte = 0; byte < pmch_cfg.pdsch_cfg.grant.tb[tb].tbs / 8; byte++) {
        data_tx[tb][byte] = (uint8_t)srslte_random_uniform_int_dist(random, 0, 255);
      }
    }
  }

  if (srslte_pmch_encode(&pmch, &dl_sf, &pmch_cfg, data_tx[0], tx_slot_symbols)) {
    ERROR("Error encoding PDSCH\n");
    exit(-1);
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  printf("ENCODED in %.2f (PHY bitrate=%.2f Mbps. Processing bitrate=%.2f Mbps)\n",
         (float)t[0].tv_usec / M,
         (float)(pmch_cfg.pdsch_cfg.grant.tb[0].tbs + pmch_cfg.pdsch_cfg.grant.tb[1].tbs) / 1000.0f,
         (float)(pmch_cfg.pdsch_cfg.grant.tb[0].tbs + pmch_cfg.pdsch_cfg.grant.tb[1].tbs) * M / t[0].tv_usec);

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
        rx_sf_symbols[j][k] += tx_sf_symbols[i][k];
      }
    }
  }

#else
  /* combine outputs */
  for (j = 0; j < nof_rx_antennas; j++) {
    for (k = 0; k < SRSLTE_NOF_RE(cell); k++) {
      rx_slot_symbols[j][k] = 0.0f;
      for (i = 0; i < cell.nof_ports; i++) {
        rx_slot_symbols[j][k] += tx_slot_symbols[i][k] * ce[i][j][k];
      }
    }
  }
#endif

  int r = 0;
  gettimeofday(&t[1], NULL);

#ifdef DO_OFDM
  /* For each Rx antenna demodulate OFDM */
  for (i = 0; i < nof_rx_antennas; i++) {
    srslte_ofdm_rx_sf(&fft_mbsfn[i]);
  }
#endif
  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    if (pmch_cfg.pdsch_cfg.grant.tb[i].enabled) {
      srslte_softbuffer_rx_reset_tbs(softbuffers_rx[i], (uint32_t)pmch_cfg.pdsch_cfg.grant.tb[i].tbs);
    }
  }

  pmch_cfg.pdsch_cfg.softbuffers.rx[0] = softbuffers_rx[0];

  srslte_pdsch_res_t pdsch_res[2];
  pdsch_res[0].payload = data_rx[0];

  r = srslte_pmch_decode(&pmch, &dl_sf, &pmch_cfg, &chest_dl_res, rx_slot_symbols, pdsch_res);
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  printf("DECODED %s in %.2f (PHY bitrate=%.2f Mbps. Processing bitrate=%.2f Mbps)\n",
         r ? "Error" : "OK",
         (float)t[0].tv_usec / M,
         (float)(pmch_cfg.pdsch_cfg.grant.tb[0].tbs + pmch_cfg.pdsch_cfg.grant.tb[1].tbs) / 1000.0f,
         (float)(pmch_cfg.pdsch_cfg.grant.tb[0].tbs + pmch_cfg.pdsch_cfg.grant.tb[1].tbs) * M / t[0].tv_usec);

  /* If there is an error in PDSCH decode */
  if (r) {
    ret = -1;
    goto quit;
  }

  /* Check Tx and Rx bytes */
  for (int tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
    if (pmch_cfg.pdsch_cfg.grant.tb[tb].enabled) {
      for (int byte = 0; byte < pmch_cfg.pdsch_cfg.grant.tb[tb].tbs / 8; byte++) {
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

  for (i = 0; i < nof_rx_antennas; i++) {
    srslte_ofdm_tx_free(&ifft_mbsfn[i]);
    srslte_ofdm_rx_free(&fft_mbsfn[i]);
    if (rx_sf_symbols[i]) {
      free(rx_sf_symbols[i]);
    }
  }

  srslte_pmch_free(&pmch);
  srslte_chest_dl_res_free(&chest_dl_res);
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

  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    if (tx_sf_symbols[i]) {
      free(tx_sf_symbols[i]);
    }
    if (tx_slot_symbols[i]) {
      free(tx_slot_symbols[i]);
    }
    if (rx_slot_symbols[i]) {
      free(rx_slot_symbols[i]);
    }
  }
  srslte_random_free(random);
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }


  exit(ret);
}
