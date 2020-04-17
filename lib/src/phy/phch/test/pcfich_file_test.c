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

char* input_file_name  = NULL;
char* matlab_file_name = NULL;

srslte_cell_t cell = {.nof_prb         = 6,
                      .nof_ports       = 1,
                      .cp              = SRSLTE_CP_NORM,
                      .phich_length    = SRSLTE_PHICH_NORM,
                      .phich_resources = SRSLTE_PHICH_R_1,
                      .frame_type      = SRSLTE_FDD};

int flen;

FILE* fmatlab = NULL;

srslte_filesource_t   fsrc;
cf_t *                input_buffer, *fft_buffer[SRSLTE_MAX_CODEWORDS];
srslte_pcfich_t       pcfich;
srslte_regs_t         regs;
srslte_ofdm_t         fft;
srslte_chest_dl_t     chest;
srslte_chest_dl_res_t chest_res;
bool                  use_standard_lte_rates = false;

void usage(char* prog)
{
  printf("Usage: %s [vcdoe] -i input_file\n", prog);
  printf("\t-o output matlab file name [Default Disabled]\n");
  printf("\t-c cell.id [Default %d]\n", cell.id);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-e Set extended prefix [Default Normal]\n");
  printf("\n-d Use standard LTE rates [Default No]\n");
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "iovcdenp")) != -1) {
    switch (opt) {
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'd':
        use_standard_lte_rates = true;
        break;
      case 'n':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        cell.nof_ports = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'o':
        matlab_file_name = argv[optind];
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
  srslte_use_standard_symbol_size(use_standard_lte_rates);

  if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
    ERROR("Error opening file %s\n", input_file_name);
    exit(-1);
  }

  if (matlab_file_name) {
    fmatlab = fopen(matlab_file_name, "w");
    if (!fmatlab) {
      perror("fopen");
      return -1;
    }
  } else {
    fmatlab = NULL;
  }

  flen = SRSLTE_SF_LEN(srslte_symbol_sz(cell.nof_prb));

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
    ERROR("Error initiating REGs\n");
    return -1;
  }

  if (srslte_pcfich_init(&pcfich, 1)) {
    ERROR("Error creating PBCH object\n");
    return -1;
  }
  if (srslte_pcfich_set_cell(&pcfich, &regs, cell)) {
    ERROR("Error creating PBCH object\n");
    return -1;
  }

  DEBUG("Memory init OK\n");
  return 0;
}

void base_free()
{

  srslte_filesource_free(&fsrc);
  if (fmatlab) {
    fclose(fmatlab);
  }

  free(input_buffer);
  free(fft_buffer[0]);

  srslte_filesource_free(&fsrc);

  srslte_chest_dl_res_free(&chest_res);
  srslte_chest_dl_free(&chest);
  srslte_ofdm_rx_free(&fft);

  srslte_pcfich_free(&pcfich);
  srslte_regs_free(&regs);
}

int main(int argc, char** argv)
{
  float cfi_corr;
  int   n;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc, argv);

  if (base_init()) {
    ERROR("Error initializing receiver\n");
    exit(-1);
  }

  n = srslte_filesource_read(&fsrc, input_buffer, flen);

  srslte_ofdm_rx_sf(&fft);

  if (fmatlab) {
    fprintf(fmatlab, "infft=");
    srslte_vec_fprint_c(fmatlab, input_buffer, flen);
    fprintf(fmatlab, ";\n");

    fprintf(fmatlab, "outfft=");
    srslte_vec_sc_prod_cfc(fft_buffer[0], 1000.0, fft_buffer[0], SRSLTE_CP_NSYMB(cell.cp) * cell.nof_prb * SRSLTE_NRE);
    srslte_vec_fprint_c(fmatlab, fft_buffer[0], SRSLTE_CP_NSYMB(cell.cp) * cell.nof_prb * SRSLTE_NRE);
    fprintf(fmatlab, ";\n");
    srslte_vec_sc_prod_cfc(fft_buffer[0], 0.001, fft_buffer[0], SRSLTE_CP_NSYMB(cell.cp) * cell.nof_prb * SRSLTE_NRE);
  }

  srslte_dl_sf_cfg_t dl_sf;
  ZERO_OBJECT(dl_sf);

  /* Get channel estimates for each port */
  srslte_chest_dl_estimate(&chest, &dl_sf, fft_buffer, &chest_res);

  INFO("Decoding PCFICH\n");

  n = srslte_pcfich_decode(&pcfich, &dl_sf, &chest_res, fft_buffer, &cfi_corr);
  printf("cfi: %d, distance: %f\n", dl_sf.cfi, cfi_corr);

  srslte_vec_save_file("input", input_buffer, SRSLTE_SF_LEN_PRB(cell.nof_prb) * sizeof(cf_t));
  srslte_vec_save_file("chest", chest_res.ce[0][0], SRSLTE_SF_LEN(cell.nof_prb) * sizeof(cf_t));
  srslte_vec_save_file("fft", fft_buffer[0], SRSLTE_NOF_RE(cell) * sizeof(cf_t));
  srslte_vec_save_file("d", pcfich.d, pcfich.nof_symbols * sizeof(cf_t));

  base_free();

  if (n < 0) {
    ERROR("Error decoding PCFICH\n");
    exit(-1);
  } else if (n == 0) {
    printf("Could not decode PCFICH\n");
    exit(-1);
  } else {
    if (cfi_corr > 2.8 && dl_sf.cfi == 2) {
      exit(0);
    } else {
      exit(-1);
    }
  }
}
