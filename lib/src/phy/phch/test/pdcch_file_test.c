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
    6,                 // cell.cell.cell.nof_prb
    1,                 // cell.cell.nof_ports
    0,                 // cell.id
    SRSLTE_CP_NORM,    // cyclic prefix
    SRSLTE_PHICH_NORM, // PHICH length
    SRSLTE_PHICH_R_1,  // PHICH resources
    SRSLTE_FDD,

};

uint32_t cfi = 2;
int      flen;
uint16_t rnti       = SRSLTE_SIRNTI;
int      max_frames = 10;

srslte_dci_format_t   dci_format = SRSLTE_DCI_FORMAT1A;
srslte_filesource_t   fsrc;
srslte_pdcch_t        pdcch;
cf_t *                input_buffer, *fft_buffer[SRSLTE_MAX_CODEWORDS];
srslte_regs_t         regs;
srslte_ofdm_t         fft;
srslte_chest_dl_t     chest;
srslte_chest_dl_res_t chest_res;

void usage(char* prog)
{
  printf("Usage: %s [vcfoe] -i input_file\n", prog);
  printf("\t-c cell.id [Default %d]\n", cell.id);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-o DCI Format [Default %s]\n", srslte_dci_format_string(dci_format));
  printf("\t-r rnti [Default SI-RNTI]\n");
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-m max_frames [Default %d]\n", max_frames);
  printf("\t-e Set extended prefix [Default Normal]\n");
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "irvofcenmp")) != -1) {
    switch (opt) {
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        rnti = strtoul(argv[optind], NULL, 0);
        break;
      case 'm':
        max_frames = strtoul(argv[optind], NULL, 0);
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
      case 'o':
        dci_format = srslte_dci_format_from_string(argv[optind]);
        if (dci_format == SRSLTE_DCI_NOF_FORMATS) {
          ERROR("Error unsupported format %s\n", argv[optind]);
          exit(-1);
        }
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

  input_buffer = srslte_vec_cf_malloc(flen);
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }

  fft_buffer[0] = srslte_vec_cf_malloc(SRSLTE_NOF_RE(cell));
  if (!fft_buffer[0]) {
    perror("malloc");
    return -1;
  }

  if (srslte_chest_dl_init(&chest, cell.nof_prb, 1)) {
    ERROR("Error initializing equalizer\n");
    return -1;
  }
  if (srslte_chest_dl_res_init(&chest_res, cell.nof_prb)) {
    ERROR("Error initializing equalizer\n");
    return -1;
  }
  if (srslte_chest_dl_set_cell(&chest, cell)) {
    ERROR("Error initializing equalizer\n");
    return -1;
  }

  if (srslte_ofdm_rx_init(&fft, cell.cp, input_buffer, fft_buffer[0], cell.nof_prb)) {
    ERROR("Error initializing FFT\n");
    return -1;
  }

  if (srslte_regs_init(&regs, cell)) {
    ERROR("Error initiating regs\n");
    return -1;
  }

  if (srslte_pdcch_init_ue(&pdcch, cell.nof_prb, 1)) {
    ERROR("Error creating PDCCH object\n");
    exit(-1);
  }
  if (srslte_pdcch_set_cell(&pdcch, &regs, cell)) {
    ERROR("Error creating PDCCH object\n");
    exit(-1);
  }

  DEBUG("Memory init OK\n");
  return 0;
}

void base_free()
{

  srslte_filesource_free(&fsrc);

  free(input_buffer);
  free(fft_buffer[0]);

  srslte_filesource_free(&fsrc);

  srslte_chest_dl_res_free(&chest_res);
  srslte_chest_dl_free(&chest);
  srslte_ofdm_rx_free(&fft);

  srslte_pdcch_free(&pdcch);
  srslte_regs_free(&regs);
}

int main(int argc, char** argv)
{
  int                   i;
  int                   frame_cnt;
  int                   ret;
  srslte_dci_location_t locations[MAX_CANDIDATES];
  uint32_t              nof_locations;
  srslte_dci_msg_t      dci_msg;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc, argv);

  if (base_init()) {
    ERROR("Error initializing memory\n");
    exit(-1);
  }

  ret       = -1;
  frame_cnt = 0;
  do {
    srslte_filesource_read(&fsrc, input_buffer, flen);

    INFO("Reading %d samples sub-frame %d\n", flen, frame_cnt);

    srslte_ofdm_rx_sf(&fft);

    srslte_dl_sf_cfg_t dl_sf;
    ZERO_OBJECT(dl_sf);
    dl_sf.tti = frame_cnt;
    dl_sf.cfi = cfi;

    /* Get channel estimates for each port */
    srslte_chest_dl_estimate(&chest, &dl_sf, fft_buffer, &chest_res);

    if (srslte_pdcch_extract_llr(&pdcch, &dl_sf, &chest_res, fft_buffer)) {
      ERROR("Error extracting LLRs\n");
      return -1;
    }
    if (rnti == SRSLTE_SIRNTI) {
      INFO("Initializing common search space for SI-RNTI\n");
      nof_locations = srslte_pdcch_common_locations(&pdcch, locations, MAX_CANDIDATES, cfi);
    } else {
      INFO("Initializing user-specific search space for RNTI: 0x%x\n", rnti);
      nof_locations = srslte_pdcch_ue_locations(&pdcch, &dl_sf, locations, MAX_CANDIDATES, rnti);
    }

    srslte_dci_cfg_t dci_cfg;
    ZERO_OBJECT(dci_cfg);

    ZERO_OBJECT(dci_msg);

    for (i = 0; i < nof_locations && dci_msg.rnti != rnti; i++) {
      dci_msg.location = locations[i];
      dci_msg.format   = dci_format;
      if (srslte_pdcch_decode_msg(&pdcch, &dl_sf, &dci_cfg, &dci_msg)) {
        ERROR("Error decoding DCI msg\n");
        return -1;
      }
    }

    if (dci_msg.rnti == rnti) {

      srslte_dci_dl_t dci;
      bzero(&dci, sizeof(srslte_dci_dl_t));
      if (srslte_dci_msg_unpack_pdsch(&cell, &dl_sf, &dci_cfg, &dci_msg, &dci)) {
        ERROR("Can't unpack DCI message\n");
      } else {
        if (dci.alloc_type == SRSLTE_RA_ALLOC_TYPE2 && dci.type2_alloc.mode == SRSLTE_RA_TYPE2_LOC &&
            dci.type2_alloc.riv == 11 && dci.tb[0].rv == 0 && dci.pid == 0 && dci.tb[0].mcs_idx == 2) {
          printf("This is the file signal.1.92M.amar.dat\n");
          ret = 0;
        }
      }
    }

    frame_cnt++;
  } while (frame_cnt <= max_frames);

  base_free();
  exit(ret);
}
