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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <unistd.h>

#include "srslte/phy/ch_estimation/chest_dl_nbiot.h"
#include "srslte/phy/ch_estimation/refsignal_dl.h"
#include "srslte/phy/ch_estimation/refsignal_dl_nbiot.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/io/filesource.h"
#include "srslte/phy/phch/npdsch.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define SRSLTE_NBIOT_SFLEN SRSLTE_SF_LEN(srslte_symbol_sz(cell.base.nof_prb))
#define SFN 0
#define SF_IDX 1

#define SRSLTE_NBIOT_NOF_RE_X_PRB (SRSLTE_CP_NORM_SF_NSYMB * SRSLTE_NRE)

// Enable to measure execution time
//#define DO_OFDM

uint32_t            nof_ports_lte   = 0;
uint32_t            nof_ports_nbiot = 1;
srslte_nbiot_mode_t mode            = SRSLTE_NBIOT_MODE_INBAND_SAME_PCI;
uint32_t            expected_nof_re = 0;

uint32_t n_id_ncell = 0;
uint32_t cfi        = 2;
uint32_t subframe   = 1;
uint32_t rv_idx     = 0;
uint16_t rnti       = 1234;
uint16_t i_tbs_val  = 0;
char*    input_file = NULL;

void usage(char* prog)
{
  printf("Usage: %s [fmMlsrRFpnv] \n", prog);
  printf("\t-f read signal from file [Default generate it with pdsch_encode()]\n");
  printf("\t-m i_tbs value [Default %d]\n", i_tbs_val);
  printf("\t-M NB-IoT deployment mode (0=InbandSamePCI,1=InbandDifferentPCI,2=GuardBand,3=Standalone) [Default %d]\n",
         mode);
  printf("\t-l n_id_ncell [Default %d]\n", n_id_ncell);
  printf("\t-s subframe [Default %d]\n", subframe);
  printf("\t-r rv_idx [Default %d]\n", rv_idx);
  printf("\t-R rnti [Default %d]\n", rnti);
  printf("\t-p Base cell cell nof_ports [Default %d]\n", nof_ports_lte);
  printf("\t-P NB-IoT cell nof_ports [Default %d]\n", nof_ports_nbiot);
  printf("\t-x Expected number of resource elements [Default %d]\n", expected_nof_re);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "fmMlsrRpPvx")) != -1) {
    switch (opt) {
      case 'f':
        input_file = argv[optind];
        break;
      case 's':
        subframe = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        rv_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'R':
        rnti = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        nof_ports_lte = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'P':
        nof_ports_nbiot = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'M':
        mode = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        i_tbs_val = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'l':
        n_id_ncell = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'x':
        expected_nof_re = (uint32_t)strtol(argv[optind], NULL, 10);
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

/* fills a list with RE indices containing either NRS or CRS given a particular cell configration
 * @return number of RE containing reference signals
 */
int get_ref_res(srslte_nbiot_cell_t cell, int32_t* re_with_refs)
{
  bzero(re_with_refs, SRSLTE_NBIOT_MAX_NUM_RE_WITH_REFS * sizeof(uint32_t));
  uint32_t num_ref = 0;

  // add all RE that contain NRS
  for (int p = 0; p < cell.nof_ports; p++) {
    DEBUG("Adding NRS for port=%d n_id_ncell=%d\n", p, cell.n_id_ncell);
    uint32_t nof_syms = srslte_refsignal_dl_nbiot_nof_symbols(p);
    for (int l = 0; l < nof_syms; l++) {
      uint32_t nsymbol = srslte_refsignal_nrs_nsymbol(l);
      DEBUG("  - adding NRS for symbol=%d\n", nsymbol);

      // two references per symbol
      for (int m = 0; m < SRSLTE_NBIOT_NUM_NRS_X_SYM_X_PORT; m++) {
        uint32_t fidx   = srslte_refsignal_dl_nbiot_fidx(cell, l, p, m);
        uint32_t re_idx = SRSLTE_RE_IDX(cell.base.nof_prb, nsymbol, fidx);
        DEBUG("    - adding NRS at re_idx=%d with fidx=%d\n", re_idx, fidx);
        re_with_refs[num_ref] = re_idx;
        num_ref++;
      }
    }
  }

  // add all RE that contain CRS
  for (int p = 0; p < cell.base.nof_ports; p++) {
    DEBUG("Adding CRS for port=%d cell_id=%d\n", p, cell.base.id);
    uint32_t nof_syms = srslte_refsignal_cs_nof_symbols(NULL, NULL, p);
    for (int l = 0; l < nof_syms; l++) {
      uint32_t nsymbol = srslte_refsignal_cs_nsymbol(l, cell.base.cp, p);
      DEBUG("  - adding CRS for symbol=%d\n", nsymbol);

      // two references per symbol
      for (int m = 0; m < 2; m++) {
        uint32_t fidx   = ((srslte_refsignal_cs_v(p, l) + (cell.base.id % 6)) % 6) + m * 6;
        uint32_t re_idx = SRSLTE_RE_IDX(cell.base.nof_prb, nsymbol, fidx);
        DEBUG("    - adding CRS at re_idx=%d with fidx=%d\n", re_idx, fidx);
        re_with_refs[num_ref] = re_idx;
        num_ref++;
      }
    }
  }

  // print all RE's that contain references
  printf("RE that contain reference signals:\n");
  srslte_vec_fprint_i(stdout, re_with_refs, num_ref);

  return num_ref;
}

int extract_re(srslte_nbiot_cell_t cell, uint32_t l_start, uint32_t expected_nof_re)
{
  srslte_npdsch_t npdsch;
  bzero(&npdsch, sizeof(srslte_npdsch_t));
  npdsch.cell = cell;

  if (!srslte_nbiot_cell_isvalid(&npdsch.cell)) {
    printf("cell is not properly configured\n");
    return SRSLTE_ERROR;
  }

  // fill dummy values in RE
  cf_t sf_syms[SRSLTE_NBIOT_NOF_RE_X_PRB];
  for (int i = 0; i < SRSLTE_NBIOT_NOF_RE_X_PRB; i++) {
    sf_syms[i] = i;
  }

  srslte_ra_nbiot_dl_grant_t dl_grant;
  bzero(&dl_grant, sizeof(srslte_ra_nbiot_dl_grant_t));
  dl_grant.l_start = l_start;

  cf_t npdsch_syms[SRSLTE_NPDSCH_MAX_RE];
  int  nof_ext_syms = srslte_npdsch_cp(&npdsch, sf_syms, npdsch_syms, &dl_grant, false);

#if 0
  if (SRSLTE_VERBOSE_ISDEBUG()) {
    for (int i = 0; i < nof_ext_syms; i++) {
      printf("re%d:", i);
      srslte_vec_fprint_c(stdout, &npdsch_syms[i], 1);
    }
  }
#endif

  // check number of extracted REs
  if (nof_ext_syms != expected_nof_re) {
    printf("RE extraction failed (expected %d, but got %d)!\n", expected_nof_re, nof_ext_syms);
    return SRSLTE_ERROR;
  }

  // check that there are not REs containing NRS or CRS
  int32_t re_with_refs[SRSLTE_NBIOT_MAX_NUM_RE_WITH_REFS];
  int     num_re_with_refs = get_ref_res(cell, re_with_refs);

  // for each extracted data symbol, check if it collides with a reference signal
  for (int i = 0; i < num_re_with_refs; i++) {
    for (int l = 0; l < nof_ext_syms; l++) {
      if (npdsch_syms[l] == re_with_refs[i]) {
        printf("Extracted data RE is also a reference signal %d: ", l);
        srslte_vec_fprint_c(stdout, &npdsch_syms[l], 1);
        return SRSLTE_ERROR;
      }
    }
  }

  return SRSLTE_SUCCESS;
}

int re_extract_test(int argc, char** argv)
{
  parse_args(argc, argv);

  if (expected_nof_re == 0) {
    printf("Skipping RE extraction test because number of expected REs isn't given!\n");
    return SRSLTE_SUCCESS;
  }

  srslte_nbiot_cell_t cell = {};

  // Standalone mode with l_start=0 gives the maximum number of REs
  cell.mode           = mode;
  cell.base.nof_prb   = 1;
  cell.base.id        = (mode == SRSLTE_NBIOT_MODE_INBAND_SAME_PCI) ? n_id_ncell : rand() % SRSLTE_NUM_PCI;
  cell.n_id_ncell     = n_id_ncell;
  cell.nof_ports      = nof_ports_nbiot;
  cell.base.nof_ports = nof_ports_lte;
  uint32_t l_start    = 0;

  if (extract_re(cell, l_start, expected_nof_re) != SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int coding_test(int argc, char** argv)
{
  int                        ret = SRSLTE_ERROR;
  struct timeval             t[3];
  srslte_ra_nbiot_dl_grant_t grant;
  srslte_npdsch_cfg_t        npdsch_cfg;
  srslte_npdsch_t            npdsch;
  uint8_t*                   data    = NULL;
  uint8_t*                   rx_data = NULL;
  cf_t*                      ce[SRSLTE_MAX_PORTS]           = {NULL};
  cf_t*                      sf_symbols                     = NULL;
  cf_t*                      slot_symbols[SRSLTE_MAX_PORTS] = {NULL};
  srslte_ofdm_t              ofdm_tx, ofdm_rx;

  parse_args(argc, argv);

  // setup cell config for this test
  srslte_nbiot_cell_t cell = {};
  cell.base.nof_prb   = 1;
  cell.base.cp        = SRSLTE_CP_NORM;
  cell.base.nof_ports = 1;
  cell.nof_ports      = 1;
  cell.mode           = SRSLTE_NBIOT_MODE_STANDALONE;
  cell.n_id_ncell     = n_id_ncell;

  if (!srslte_nbiot_cell_isvalid(&cell)) {
    printf("Cell is not properly configured\n");
    return ret;
  }

  bzero(&npdsch, sizeof(srslte_npdsch_t));
  bzero(&npdsch_cfg, sizeof(srslte_npdsch_cfg_t));
  bzero(ce, sizeof(cf_t*) * SRSLTE_MAX_PORTS);
  bzero(slot_symbols, sizeof(cf_t*) * SRSLTE_MAX_PORTS);

  srslte_ra_nbiot_dl_dci_t dci;
  bzero(&dci, sizeof(srslte_ra_nbiot_dl_dci_t));
  dci.mcs_idx = i_tbs_val;
  dci.rv_idx  = rv_idx;
  if (srslte_ra_nbiot_dl_dci_to_grant(&dci, &grant, SFN, SF_IDX, DUMMY_R_MAX, false, cell.mode)) {
    fprintf(stderr, "Error computing resource allocation\n");
    return ret;
  }

  if (!srslte_nbiot_cell_isvalid(&cell)) {
    fprintf(stderr, "Cell configuration invalid.\n");
    exit(-1);
  }

  /* Configure NPDSCH */
  if (srslte_npdsch_cfg(&npdsch_cfg, cell, &grant, subframe)) {
    fprintf(stderr, "Error configuring NPDSCH\n");
    exit(-1);
  }

  /* init memory */
  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
    ce[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_RE(cell.base.nof_prb, cell.base.cp));
    if (!ce[i]) {
      perror("srslte_vec_malloc");
      goto quit;
    }
    for (int j = 0; j < SRSLTE_SF_LEN_RE(cell.base.nof_prb, cell.base.cp); j++) {
      ce[i][j] = 1;
    }
    slot_symbols[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_RE(cell.base.nof_prb, cell.base.cp));
    if (!slot_symbols[i]) {
      perror("srslte_vec_malloc");
      goto quit;
    }
    memset(slot_symbols[i], 0, sizeof(cf_t) * SRSLTE_SF_LEN_RE(cell.base.nof_prb, cell.base.cp));
  }

  sf_symbols = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_NBIOT_SFLEN);
  memset(sf_symbols, 0, sizeof(cf_t) * SRSLTE_NBIOT_SFLEN);

  data = srslte_vec_malloc(sizeof(uint8_t) * grant.mcs[0].tbs / 8);
  if (!data) {
    perror("srslte_vec_malloc");
    goto quit;
  }

  rx_data = srslte_vec_malloc(sizeof(uint8_t) * grant.mcs[0].tbs / 8);
  if (!rx_data) {
    perror("srslte_vec_malloc");
    goto quit;
  }

  srslte_ofdm_tx_init(&ofdm_tx, cell.base.cp, slot_symbols[0], sf_symbols, cell.base.nof_prb);
  srslte_ofdm_rx_init(&ofdm_rx, cell.base.cp, sf_symbols, slot_symbols[0], cell.base.nof_prb);

  if (srslte_npdsch_init(&npdsch)) {
    fprintf(stderr, "Error creating NPDSCH object\n");
    goto quit;
  }

  if (srslte_npdsch_set_cell(&npdsch, cell)) {
    fprintf(stderr, "Error configuring NPDSCH object\n");
    goto quit;
  }

  srslte_npdsch_set_rnti(&npdsch, rnti);

  if (input_file) {
    srslte_filesource_t fsrc;
    if (srslte_filesource_init(&fsrc, input_file, SRSLTE_COMPLEX_FLOAT_BIN)) {
      fprintf(stderr, "Error opening file %s\n", input_file);
      exit(-1);
    }
#ifdef DO_OFDM
    srslte_filesource_read(&fsrc, sf_symbols, SRSLTE_SF_LEN_PRB(cell.base.nof_prb));
#else
    srslte_filesource_read(&fsrc, slot_symbols[0], SRSLTE_SF_LEN_RE(cell.base.nof_prb, cell.base.cp));
#endif

    srslte_chest_dl_nbiot_t chest;
    if (srslte_chest_dl_nbiot_init(&chest, SRSLTE_NBIOT_MAX_PRB)) {
      printf("Error initializing equalizer\n");
      exit(-1);
    }
    if (srslte_chest_dl_nbiot_set_cell(&chest, cell) != SRSLTE_SUCCESS) {
      fprintf(stderr, "Error setting channel estimator's cell configuration\n");
      return -1;
    }

    srslte_chest_dl_nbiot_estimate(&chest, slot_symbols[0], ce, subframe);
    srslte_chest_dl_nbiot_free(&chest);

    srslte_filesource_free(&fsrc);
  }

  // generate random data
  srand(time(NULL));
  for (int i = 0; i < grant.mcs[0].tbs / 8; i++) {
    data[i] = rand() % 256;
  }

  if (!input_file) {
    if (srslte_npdsch_encode(&npdsch, &npdsch_cfg, NULL, data, slot_symbols)) {
      fprintf(stderr, "Error encoding NPDSCH\n");
      goto quit;
    }
  }

#ifdef DO_OFDM
  // Transform to OFDM symbols
  srslte_ofdm_tx_sf(&ofdm_tx);
#endif

  if (SRSLTE_VERBOSE_ISDEBUG()) {
    DEBUG("SAVED FILE npdsch_tx_sf.bin: transmitted subframe symbols\n");
    srslte_vec_save_file("npdsch_tx_sf.bin", sf_symbols, SRSLTE_NBIOT_SFLEN * sizeof(cf_t));
  }

  int M = 1;
  int r = 0;
  gettimeofday(&t[1], NULL);

  for (int i = 0; i < M; i++) {
#ifdef DO_OFDM
    srslte_ofdm_rx_sf(&ofdm_rx);
#endif
    r = srslte_npdsch_decode(&npdsch, &npdsch_cfg, NULL, slot_symbols[0], ce, 0, 0, rx_data);
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);

  // compare transmitted and received data
  printf("Tx payload: ");
  srslte_vec_fprint_b(stdout, data, grant.mcs[0].tbs / 8);
  printf("Rx payload: ");
  srslte_vec_fprint_b(stdout, rx_data, grant.mcs[0].tbs / 8);

  if (memcmp(data, rx_data, sizeof(uint8_t) * grant.mcs[0].tbs / 8) == 0) {
    printf("DECODED in %.2f (PHY bitrate=%.2f Mbps. Processing bitrate=%.2f Mbps)\n",
           (float)t[0].tv_usec / M,
           (float)grant.mcs[0].tbs / 1000,
           (float)grant.mcs[0].tbs * M / t[0].tv_usec);
  } else {
    printf("Error!\n");
  }
  if (r) {
    ret = SRSLTE_ERROR;
    goto quit;
  }

  ret = SRSLTE_SUCCESS;
quit:
  srslte_npdsch_free(&npdsch);

  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
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
  if (rx_data) {
    free(rx_data);
  }

  if (sf_symbols) {
    free(sf_symbols);
  }

  srslte_ofdm_tx_free(&ofdm_tx);
  srslte_ofdm_rx_free(&ofdm_rx);
  return ret;
}

int main(int argc, char** argv)
{
  int ret = SRSLTE_ERROR;

  if (re_extract_test(argc, argv) != SRSLTE_SUCCESS) {
    printf("Resource element extraction test failed!\n");
    return ret;
  }

  if (coding_test(argc, argv) != SRSLTE_SUCCESS) {
    printf("NPDSCH En- and Decoding test failed!\n");
    return ret;
  }

  ret = SRSLTE_SUCCESS;
  return ret;
}
