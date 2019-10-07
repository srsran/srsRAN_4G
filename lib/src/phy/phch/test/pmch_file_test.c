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
#include <unistd.h>

#include "srslte/srslte.h"

char* input_file_name = NULL;

srslte_cell_t cell = {
    100,               // nof_prb
    1,                 // nof_ports
    1,                 // cell_id
    SRSLTE_CP_EXT,     // cyclic prefix
    SRSLTE_PHICH_NORM, // PHICH length
    SRSLTE_PHICH_R_1_6,
    SRSLTE_FDD,

};

int flen;

uint32_t cfi = 2;

int      max_frames = 150;
uint32_t sf_idx     = 1;

uint8_t non_mbsfn_region = 2;
int     mbsfn_area_id    = 1;

srslte_softbuffer_rx_t softbuffer_rx = {};
srslte_filesource_t    fsrc;
srslte_ue_dl_t         ue_dl;
cf_t*                  input_buffer[SRSLTE_MAX_PORTS];

void usage(char* prog)
{
  printf("Usage: %s [rvfcenmps] -i input_file\n", prog);
  printf("\t-c cell.id [Default %d]\n", cell.id);
  printf("\t-s Start subframe_idx [Default %d]\n", sf_idx);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-M mbsfn_area_id [Default %d]\n", mbsfn_area_id);
  printf("\t-e Set extended prefix [Default Normal]\n");
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "ivfcenmps")) != -1) {
    switch (opt) {
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        sf_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        cfi = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        cell.nof_ports = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'M':
        mbsfn_area_id = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      case 'e':
        cell.cp = SRSLTE_CP_EXT;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (!input_file_name) {
    usage(argv[0]);
    exit(-1);
  }
}

int base_init()
{

  if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
    ERROR("Error opening file %s\n", input_file_name);
    exit(-1);
  }

  flen = 2 * (SRSLTE_SLOT_LEN(srslte_symbol_sz(cell.nof_prb)));

  input_buffer[0] = srslte_vec_cf_malloc(flen);
  if (!input_buffer[0]) {
    perror("malloc");
    exit(-1);
  }

  if (srslte_ue_dl_init(&ue_dl, input_buffer, cell.nof_prb, 1)) {
    ERROR("Error initializing UE DL\n");
    return -1;
  }

  if (srslte_ue_dl_set_cell(&ue_dl, cell)) {
    ERROR("Error initializing UE DL\n");
    return -1;
  }

  srslte_ue_dl_set_mbsfn_area_id(&ue_dl, mbsfn_area_id);
  srslte_ue_dl_set_non_mbsfn_region(&ue_dl, non_mbsfn_region);

  srslte_softbuffer_rx_init(&softbuffer_rx, cell.nof_prb);
  srslte_softbuffer_rx_reset(&softbuffer_rx);

  DEBUG("Memory init OK\n");
  return 0;
}

void base_free()
{
  srslte_filesource_free(&fsrc);
  srslte_ue_dl_free(&ue_dl);
  srslte_softbuffer_rx_free(&softbuffer_rx);
  free(input_buffer[0]);
}

int main(int argc, char** argv)
{
  int ret;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }
  parse_args(argc, argv);
  srslte_use_standard_symbol_size(false);
  if (base_init()) {
    ERROR("Error initializing memory\n");
    exit(-1);
  }

  uint8_t* data = malloc(100000);
  if (!data) {
    perror("malloc");
    exit(-1);
  }

  ret = -1;

  srslte_filesource_read(&fsrc, input_buffer[0], flen);
  INFO("Reading %d samples sub-frame %d\n", flen, sf_idx);

  srslte_dl_sf_cfg_t dl_sf;
  ZERO_OBJECT(dl_sf);

  dl_sf.cfi     = cfi;
  dl_sf.tti     = sf_idx;
  dl_sf.sf_type = SRSLTE_SF_MBSFN;

  srslte_ue_dl_cfg_t ue_dl_cfg;
  ZERO_OBJECT(ue_dl_cfg);
  ue_dl_cfg.chest_cfg.mbsfn_area_id = mbsfn_area_id;

  // Special configuration for MBSFN channel estimation
  ue_dl_cfg.chest_cfg.filter_type          = SRSLTE_CHEST_FILTER_TRIANGLE;
  ue_dl_cfg.chest_cfg.filter_coef[0]       = 0.1;
  ue_dl_cfg.chest_cfg.estimator_alg        = SRSLTE_ESTIMATOR_ALG_INTERPOLATE;
  ue_dl_cfg.chest_cfg.noise_alg            = SRSLTE_NOISE_ALG_PSS;

  if ((ret = srslte_ue_dl_decode_fft_estimate(&ue_dl, &dl_sf, &ue_dl_cfg)) < 0) {
    return ret;
  }
  dl_sf.cfi = cfi;
  srslte_pmch_cfg_t pmch_cfg;
  ZERO_OBJECT(pmch_cfg);
  pmch_cfg.area_id                     = mbsfn_area_id;
  pmch_cfg.pdsch_cfg.softbuffers.rx[0] = &softbuffer_rx;

  srslte_dci_dl_t dci;
  ZERO_OBJECT(dci);
  dci.rnti                    = SRSLTE_MRNTI;
  dci.format                  = SRSLTE_DCI_FORMAT1;
  dci.alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
  dci.type0_alloc.rbg_bitmask = 0xffffffff;
  dci.tb[0].mcs_idx           = 2;
  SRSLTE_DCI_TB_DISABLE(dci.tb[1]);
  srslte_ra_dl_dci_to_grant(&cell, &dl_sf, SRSLTE_TM1, false, &dci, &pmch_cfg.pdsch_cfg.grant);

  srslte_pdsch_res_t pdsch_res;
  pdsch_res.payload = data;
  ret               = srslte_ue_dl_decode_pmch(&ue_dl, &dl_sf, &pmch_cfg, &pdsch_res);
  if (pdsch_res.crc == 1) {
    printf("PMCH Decoded OK!\n");
  } else if (pdsch_res.crc == 0) {
    printf("Error decoding PMCH\n");
  }

  base_free();
  if (data != NULL) {
    free(data);
  }
  if (ret >= 0) {
    exit(0);
  } else {
    exit(-1);
  }
}
