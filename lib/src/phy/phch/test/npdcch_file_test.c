/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/phy/ch_estimation/chest_dl_nbiot.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/io/filesource.h"
#include "srsran/phy/phch/dci_nbiot.h"
#include "srsran/phy/phch/npdcch.h"
#include "srsran/phy/phch/ra_nbiot.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

char*                 input_file_name = NULL;
srsran_dci_format_t   dci_format      = SRSRAN_DCI_FORMATN0;
uint16_t              rnti            = 0;
uint32_t              tti             = 0;
int                   nof_frames      = 1;
srsran_dci_location_t dci_location    = {};

srsran_nbiot_cell_t cell = {.base       = {.nof_prb = 1, .nof_ports = 1, .cp = SRSRAN_CP_NORM, .id = 0},
                            .nbiot_prb  = 0,
                            .n_id_ncell = 0,
                            .nof_ports  = 1,
                            .mode       = SRSRAN_NBIOT_MODE_STANDALONE};

void usage(char* prog)
{
  printf("Usage: %s [cprndv] -i input_file\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.base.id);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.base.nof_ports);
  printf("\t-o DCI Format [Default %s]\n", srsran_dci_format_string(dci_format));
  printf("\t-L DCI location L value [Default %d]\n", dci_location.L);
  printf("\t-l DCI location ncee value [Default %d]\n", dci_location.ncce);
  printf("\t-r rnti [Default %d]\n", rnti);
  printf("\t-t tti [Default %d]\n", tti);
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "icplLrontv")) != -1) {
    switch (opt) {
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'p':
        cell.base.nof_ports = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        cell.base.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'l':
        dci_location.ncce = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'L':
        dci_location.L = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        rnti = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'o':
        dci_format = srsran_dci_format_from_string(argv[optind]);
        if (dci_format == SRSRAN_DCI_NOF_FORMATS) {
          ERROR("Error unsupported format %s", argv[optind]);
          exit(-1);
        }
        break;
      case 't':
        tti = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        increase_srsran_verbose_level();
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

int main(int argc, char** argv)
{
  cf_t *                  input_buffer = NULL, *fft_buffer = NULL, *ce[SRSRAN_MAX_PORTS] = {NULL};
  srsran_filesource_t     fsrc;
  srsran_chest_dl_nbiot_t chest;
  srsran_ofdm_t           fft;
  srsran_npdcch_t         npdcch           = {};
  srsran_dci_msg_t        dci_rx           = {};
  int                     ret              = SRSRAN_ERROR;
  int                     frame_cnt        = 0;
  int                     nof_decoded_dcis = 0;
  int                     nread            = 0;

  parse_args(argc, argv);

  // we need to allocate RE's for a full 6 PRB cell
  int nof_re = 6 * SRSRAN_SF_LEN_RE(cell.base.nof_prb, cell.base.cp);

  // init memory
  for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
    ce[i] = srsran_vec_cf_malloc(nof_re);
    if (!ce[i]) {
      perror("malloc");
      goto quit;
    }
    for (int j = 0; j < nof_re; j++) {
      ce[i][j] = 1;
    }
  }

  if (srsran_filesource_init(&fsrc, input_file_name, SRSRAN_COMPLEX_FLOAT_BIN)) {
    fprintf(stderr, "Error opening file %s\n", input_file_name);
    goto quit;
  }

  uint32_t sf_len = SRSRAN_SF_LEN(srsran_symbol_sz(cell.base.nof_prb));

  input_buffer = srsran_vec_cf_malloc(sf_len);
  if (!input_buffer) {
    perror("malloc");
    goto quit;
  }

  fft_buffer = srsran_vec_cf_malloc(sf_len);
  if (!fft_buffer) {
    perror("malloc");
    goto quit;
  }

  if (srsran_chest_dl_nbiot_init(&chest, SRSRAN_NBIOT_MAX_PRB)) {
    fprintf(stderr, "Error initializing equalizer\n");
    goto quit;
  }
  if (srsran_chest_dl_nbiot_set_cell(&chest, cell) != SRSRAN_SUCCESS) {
    fprintf(stderr, "Error setting equalizer cell configuration\n");
    goto quit;
  }

  if (srsran_ofdm_rx_init(&fft, cell.base.cp, input_buffer, fft_buffer, cell.base.nof_prb)) {
    fprintf(stderr, "Error initializing FFT\n");
    goto quit;
  }
  srsran_ofdm_set_freq_shift(&fft, SRSRAN_NBIOT_FREQ_SHIFT_FACTOR);

  if (srsran_npdcch_init(&npdcch)) {
    fprintf(stderr, "Error creating NPDCCH object\n");
    goto quit;
  }

  if (srsran_npdcch_set_cell(&npdcch, cell)) {
    fprintf(stderr, "Error configuring NPDCCH object\n");
    goto quit;
  }

  do {
    nread = srsran_filesource_read(&fsrc, input_buffer, sf_len);

    if (nread == sf_len) {
      // Run FFT and estimate channel
      srsran_ofdm_rx_sf(&fft);

      INFO("%d.%d: Estimating channel.", frame_cnt, tti % 10);
      srsran_chest_dl_nbiot_estimate(&chest, fft_buffer, ce, tti % 10);

      // Extract LLR
      float noise_est = srsran_chest_dl_nbiot_get_noise_estimate(&chest);
      if (srsran_npdcch_extract_llr(&npdcch, fft_buffer, ce, noise_est, tti % 10)) {
        fprintf(stderr, "Error extracting LLRs\n");
        goto quit;
      }

      uint16_t crc_rem = 0;
      if (srsran_npdcch_decode_msg(&npdcch, &dci_rx, &dci_location, dci_format, &crc_rem)) {
        fprintf(stderr, "Error decoding DCI message\n");
        goto quit;
      }
      if (crc_rem != rnti) {
        printf("Received invalid DCI CRC 0x%x\n", crc_rem);
        goto quit;
      } else {
        if (dci_format == SRSRAN_DCI_FORMATN0) {
          // process as UL grant
          srsran_ra_nbiot_ul_dci_t   dci_unpacked;
          srsran_ra_nbiot_ul_grant_t grant;
          // Creates the UL NPUSCH resource allocation grant from a DCI format N0 message
          if (srsran_nbiot_dci_msg_to_ul_grant(&dci_rx, &dci_unpacked, &grant, tti, SRSRAN_NPUSCH_SC_SPACING_15000)) {
            fprintf(stderr, "Error unpacking DCI\n");
            goto quit;
          }
        } else {
          // process as DL grant
          srsran_ra_nbiot_dl_dci_t   dci_unpacked;
          srsran_ra_nbiot_dl_grant_t grant;
          if (srsran_nbiot_dci_msg_to_dl_grant(
                  &dci_rx, rnti, &dci_unpacked, &grant, tti / 10, tti % 10, 64 /* TODO: remove */, cell.mode)) {
            fprintf(stderr, "Error unpacking DCI\n");
            goto quit;
          }
        }
        nof_decoded_dcis++;
      }

      tti++;
      if (tti == 10240) {
        tti = 0;
        frame_cnt++;
      }
    }

  } while (nread > 0 && frame_cnt < nof_frames);

quit:
  srsran_npdcch_free(&npdcch);
  srsran_filesource_free(&fsrc);
  free(input_buffer);
  free(fft_buffer);
  srsran_chest_dl_nbiot_free(&chest);
  srsran_ofdm_rx_free(&fft);

  for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
    if (ce[i]) {
      free(ce[i]);
    }
  }
  if (nof_decoded_dcis > 0) {
    printf("Ok\n");
    ret = SRSRAN_SUCCESS;
  } else {
    printf("Error\n");
  }

  return ret;
}
