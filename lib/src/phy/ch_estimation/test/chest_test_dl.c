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

#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "srsran/srsran.h"

srsran_cell_t cell = {6,              // nof_prb
                      1,              // nof_ports
                      1000,           // cell_id
                      SRSRAN_CP_NORM, // cyclic prefix
                      SRSRAN_PHICH_NORM,
                      SRSRAN_PHICH_R_1_6,
                      SRSRAN_FDD};

char* output_matlab = NULL;

void usage(char* prog)
{
  printf("Usage: %s [recov]\n", prog);

  printf("\t-r nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-e extended cyclic prefix [Default normal]\n");

  printf("\t-c cell_id (1000 tests all). [Default %d]\n", cell.id);

  printf("\t-o output matlab file [Default %s]\n", output_matlab ? output_matlab : "None");
  printf("\t-v increase verbosity\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "recov")) != -1) {
    switch (opt) {
      case 'r':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'e':
        cell.cp = SRSRAN_CP_EXT;
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'o':
        output_matlab = argv[optind];
        break;
      case 'v':
        increase_srsran_verbose_level();
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  srsran_chest_dl_t est;
  cf_t *            input = NULL, *ce = NULL, *h = NULL, *output = NULL;
  int               i, j;
  int               ret = -1;
  int               max_cid;
  FILE*             fmatlab = NULL;
  uint32_t          cid     = 0;

  parse_args(argc, argv);

  if (output_matlab) {
    fmatlab = fopen(output_matlab, "w");
    if (!fmatlab) {
      perror("fopen");
      goto do_exit;
    }
  }

  uint32_t num_re = 2 * cell.nof_prb * SRSRAN_NRE * SRSRAN_CP_NSYMB(cell.cp);

  input = srsran_vec_cf_malloc(num_re);
  if (!input) {
    perror("srsran_vec_malloc");
    goto do_exit;
  }
  output = srsran_vec_cf_malloc(num_re);
  if (!output) {
    perror("srsran_vec_malloc");
    goto do_exit;
  }
  h = srsran_vec_cf_malloc(num_re);
  if (!h) {
    perror("srsran_vec_malloc");
    goto do_exit;
  }
  ce = srsran_vec_cf_malloc(num_re);
  if (!ce) {
    perror("srsran_vec_malloc");
    goto do_exit;
  }

  if (cell.id == 1000) {
    cid     = 0;
    max_cid = 504;
  } else {
    cid     = cell.id;
    max_cid = cell.id;
  }
  if (srsran_chest_dl_init(&est, cell.nof_prb, 1)) {
    ERROR("Error initializing equalizer");
    goto do_exit;
  }
  while (cid <= max_cid) {
    cell.id = cid;
    if (srsran_chest_dl_set_cell(&est, cell)) {
      ERROR("Error initializing equalizer");
      goto do_exit;
    }

    for (uint32_t sf_idx = 0; sf_idx < 1; sf_idx++) {
      srsran_dl_sf_cfg_t sf_cfg;
      ZERO_OBJECT(sf_cfg);
      sf_cfg.tti = sf_idx;

      for (uint32_t n_port = 0; n_port < cell.nof_ports; n_port++) {
        srsran_vec_cf_zero(input, num_re);
        for (i = 0; i < num_re; i++) {
          input[i] = 0.5 - rand() / (float)RAND_MAX + I * (0.5 - rand() / (float)RAND_MAX);
        }

        srsran_vec_cf_zero(ce, num_re);
        srsran_vec_cf_zero(h, num_re);

        srsran_refsignal_cs_put_sf(&est.csr_refs, &sf_cfg, n_port, input);

        for (i = 0; i < 2 * SRSRAN_CP_NSYMB(cell.cp); i++) {
          for (j = 0; j < cell.nof_prb * SRSRAN_NRE; j++) {
            float x = -1 + (float)i / SRSRAN_CP_NSYMB(cell.cp) + cosf(2 * M_PI * (float)j / cell.nof_prb / SRSRAN_NRE);
            h[i * cell.nof_prb * SRSRAN_NRE + j] = (3 + x) * cexpf(I * x);
            input[i * cell.nof_prb * SRSRAN_NRE + j] *= h[i * cell.nof_prb * SRSRAN_NRE + j];
          }
        }
      }

      srsran_chest_dl_res_t res;

      res.ce[0][0] = ce;

      cf_t* input_m[SRSRAN_MAX_PORTS];
      input_m[0] = input;

      struct timeval t[3];
      gettimeofday(&t[1], NULL);
      for (int k = 0; k < 100; k++) {
        srsran_chest_dl_estimate(&est, &sf_cfg, input_m, &res);
      }
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      printf("CHEST: %f us\n", (float)t[0].tv_usec / 100);

      gettimeofday(&t[1], NULL);
      for (int k = 0; k < 100; k++) {
        srsran_predecoding_single(input, ce, output, NULL, num_re, 1.0f, 0);
      }
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      printf("CHEQ-ZF: %f us\n", (float)t[0].tv_usec / 100);

      float mse = 0;
      for (i = 0; i < num_re; i++) {
        mse += cabsf(input[i] - output[i]);
      }
      mse /= num_re;
      printf("MSE: %f\n", mse);

      gettimeofday(&t[1], NULL);
      for (int k = 0; k < 100; k++) {
        srsran_predecoding_single(input, ce, output, NULL, num_re, 1.0f, res.noise_estimate);
      }
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      printf("CHEQ-MMSE: %f us\n", (float)t[0].tv_usec / 100);

      mse = 0;
      for (i = 0; i < num_re; i++) {
        mse += cabsf(input[i] - output[i]);
      }
      mse /= num_re;
      printf("MSE: %f\n", mse);

      if (mse > 2.0) {
        goto do_exit;
      }

      if (fmatlab) {
        fprintf(fmatlab, "input=");
        srsran_vec_fprint_c(fmatlab, input, num_re);
        fprintf(fmatlab, ";\n");
        fprintf(fmatlab, "h=");
        srsran_vec_fprint_c(fmatlab, h, num_re);
        fprintf(fmatlab, ";\n");
        fprintf(fmatlab, "ce=");
        srsran_vec_fprint_c(fmatlab, ce, num_re);
        fprintf(fmatlab, ";\n");
      }
    }
    cid += 10;
    INFO("cid=%d", cid);
  }
  srsran_chest_dl_free(&est);
  ret = 0;

do_exit:

  if (output) {
    free(output);
  }
  if (ce) {
    free(ce);
  }
  if (input) {
    free(input);
  }
  if (h) {
    free(h);
  }

  if (!ret) {
    printf("OK\n");
  } else {
    printf("Error at cid=%d\n", cid);
  }

  exit(ret);
}
