/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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

#include "srsran/srsran.h"

char* input_file_name = NULL;

srsran_cell_t cell = {
    100,               // nof_prb
    1,                 // nof_ports
    1,                 // cell_id
    SRSRAN_CP_EXT,     // cyclic prefix
    SRSRAN_PHICH_NORM, // PHICH length
    SRSRAN_PHICH_R_1_6,
    SRSRAN_FDD,

};

int flen;

uint32_t cfi = 2;

int      max_frames = 150;
uint32_t sf_idx     = 1;

uint8_t non_mbsfn_region = 2;
int     mbsfn_area_id    = 1;

srsran_softbuffer_rx_t softbuffer_rx = {};
srsran_filesource_t    fsrc;
srsran_ue_dl_t         ue_dl;
cf_t*                  input_buffer[SRSRAN_MAX_PORTS];

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
  printf("\t-v [set srsran_verbose to debug, default none]\n");
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
        increase_srsran_verbose_level();
        break;
      case 'e':
        cell.cp = SRSRAN_CP_EXT;
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
  if (srsran_filesource_init(&fsrc, input_file_name, SRSRAN_COMPLEX_FLOAT_BIN)) {
    ERROR("Error opening file %s", input_file_name);
    exit(-1);
  }

  flen = 2 * (SRSRAN_SLOT_LEN(srsran_symbol_sz(cell.nof_prb)));

  input_buffer[0] = srsran_vec_cf_malloc(flen);
  if (!input_buffer[0]) {
    perror("malloc");
    exit(-1);
  }

  if (srsran_ue_dl_init(&ue_dl, input_buffer, cell.nof_prb, 1)) {
    ERROR("Error initializing UE DL");
    return -1;
  }

  if (srsran_ue_dl_set_cell(&ue_dl, cell)) {
    ERROR("Error initializing UE DL");
    return -1;
  }

  srsran_ue_dl_set_mbsfn_area_id(&ue_dl, mbsfn_area_id);
  srsran_ue_dl_set_non_mbsfn_region(&ue_dl, non_mbsfn_region);

  srsran_softbuffer_rx_init(&softbuffer_rx, cell.nof_prb);
  srsran_softbuffer_rx_reset(&softbuffer_rx);

  DEBUG("Memory init OK");
  return 0;
}

void base_free()
{
  srsran_filesource_free(&fsrc);
  srsran_ue_dl_free(&ue_dl);
  srsran_softbuffer_rx_free(&softbuffer_rx);
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
  srsran_use_standard_symbol_size(false);
  if (base_init()) {
    ERROR("Error initializing memory");
    exit(-1);
  }

  uint8_t* data = malloc(100000);
  if (!data) {
    perror("malloc");
    exit(-1);
  }

  ret = -1;

  srsran_filesource_read(&fsrc, input_buffer[0], flen);
  INFO("Reading %d samples sub-frame %d", flen, sf_idx);

  srsran_dl_sf_cfg_t dl_sf;
  ZERO_OBJECT(dl_sf);

  dl_sf.cfi     = cfi;
  dl_sf.tti     = sf_idx;
  dl_sf.sf_type = SRSRAN_SF_MBSFN;

  srsran_ue_dl_cfg_t ue_dl_cfg;
  ZERO_OBJECT(ue_dl_cfg);
  ue_dl_cfg.chest_cfg.mbsfn_area_id = mbsfn_area_id;

  // Special configuration for MBSFN channel estimation
  ue_dl_cfg.chest_cfg.filter_type    = SRSRAN_CHEST_FILTER_TRIANGLE;
  ue_dl_cfg.chest_cfg.filter_coef[0] = 0.1;
  ue_dl_cfg.chest_cfg.estimator_alg  = SRSRAN_ESTIMATOR_ALG_INTERPOLATE;
  ue_dl_cfg.chest_cfg.noise_alg      = SRSRAN_NOISE_ALG_PSS;

  if ((ret = srsran_ue_dl_decode_fft_estimate(&ue_dl, &dl_sf, &ue_dl_cfg)) < 0) {
    return ret;
  }
  dl_sf.cfi = cfi;
  srsran_pmch_cfg_t pmch_cfg;
  ZERO_OBJECT(pmch_cfg);
  pmch_cfg.area_id                     = mbsfn_area_id;
  pmch_cfg.pdsch_cfg.softbuffers.rx[0] = &softbuffer_rx;

  srsran_dci_dl_t dci;
  ZERO_OBJECT(dci);
  dci.rnti                    = SRSRAN_MRNTI;
  dci.format                  = SRSRAN_DCI_FORMAT1;
  dci.alloc_type              = SRSRAN_RA_ALLOC_TYPE0;
  dci.type0_alloc.rbg_bitmask = 0xffffffff;
  dci.tb[0].mcs_idx           = 2;
  SRSRAN_DCI_TB_DISABLE(dci.tb[1]);
  srsran_ra_dl_dci_to_grant(&cell, &dl_sf, SRSRAN_TM1, false, &dci, &pmch_cfg.pdsch_cfg.grant);

  srsran_pdsch_res_t pdsch_res = {};
  pdsch_res.payload            = data;

  srsran_pdsch_res_t pdsch_res_vec[SRSRAN_MAX_CODEWORDS];
  pdsch_res_vec[0] = pdsch_res;
  ret              = srsran_ue_dl_decode_pmch(&ue_dl, &dl_sf, &pmch_cfg, pdsch_res_vec);
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
