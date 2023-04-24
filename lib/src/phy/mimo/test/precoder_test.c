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

#include <math.h>
#include <srsran/phy/utils/random.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srsran/phy/channel/ch_awgn.h"
#include "srsran/srsran.h"

#define MSE_THRESHOLD 0.0005

int                    nof_symbols  = 1000;
uint32_t               codebook_idx = 0;
int                    nof_layers = 1, nof_tx_ports = 1, nof_rx_ports = 1, nof_re = 1;
char*                  mimo_type_name        = NULL;
char                   decoder_type_name[17] = "zf";
float                  snr_db                = 100.0f;
float                  scaling               = 0.1f;
static srsran_random_t random_gen            = NULL;

void usage(char* prog)
{
  printf("Usage: %s -m [%s|%s|%s|%s] -l [nof_layers] -p [nof_tx_ports]\n"
         " -r [nof_rx_ports] -g [scaling]\n",
         prog,
         srsran_mimotype2str(0),
         srsran_mimotype2str(1),
         srsran_mimotype2str(2),
         srsran_mimotype2str(3));
  printf("\t-n num_symbols [Default %d]\n", nof_symbols);
  printf("\t-c codebook_idx [Default %d]\n", codebook_idx);
  printf("\t-s SNR in dB [Default %.1fdB]*\n", snr_db);
  printf("\t-g Scaling [Default %.1f]*\n", scaling);
  printf("\t-d decoder type [zf|mmse] [Default %s]\n", decoder_type_name);
  printf("\n");
  printf("* Performance test example:\n\t for snr in {0..20..1}; do ./precoding_test -m single -s $snr; done; \n\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "mplnrcdsg")) != -1) {
    switch (opt) {
      case 'n':
        nof_symbols = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        nof_tx_ports = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        nof_rx_ports = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'l':
        nof_layers = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        mimo_type_name = argv[optind];
        break;
      case 'c':
        codebook_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'd':
        strncpy(decoder_type_name, argv[optind], 15);
        decoder_type_name[15] = 0;
        break;
      case 's':
        snr_db = strtof(argv[optind], NULL);
        break;
      case 'g':
        scaling = strtof(argv[optind], NULL);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (!mimo_type_name) {
    usage(argv[0]);
    exit(-1);
  }
}

void populate_channel_cdd(cf_t* h[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS], uint32_t n)
{
  int i, j, k;

  for (i = 0; i < nof_tx_ports; i++) {
    for (j = 0; j < nof_rx_ports; j++) {
      for (k = 0; k < n; k++) {
        h[i][j][k] = srsran_random_uniform_complex_dist(random_gen, -1.0f, +1.0f);
      }
    }
  }
}

void populate_channel_diversity(cf_t* h[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS], uint32_t n)
{
  int i, j, k, l;

  for (i = 0; i < nof_tx_ports; i++) {
    for (j = 0; j < nof_rx_ports; j++) {
      for (k = 0; k < n / nof_layers; k++) {
        cf_t hsymb = srsran_random_uniform_complex_dist(random_gen, -1.0f, +1.0f);
        for (l = 0; l < nof_layers; l++) {
          // assume the channel is the same for all symbols
          h[i][j][k * nof_layers + l] = hsymb;
        }
      }
    }
  }
}

void populate_channel_single(cf_t* h)
{
  int i;

  for (i = 0; i < nof_re; i++) {
    h[i] = srsran_random_uniform_complex_dist(random_gen, -1.0f, +1.0f);
  }
}

void populate_channel(srsran_tx_scheme_t type, cf_t* h[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS])
{
  switch (type) {
    case SRSRAN_TXSCHEME_SPATIALMUX:
    case SRSRAN_TXSCHEME_CDD:
      populate_channel_cdd(h, (uint32_t)nof_re);
      break;
    case SRSRAN_TXSCHEME_DIVERSITY:
      populate_channel_diversity(h, (uint32_t)nof_re);
      break;
    case SRSRAN_TXSCHEME_PORT0:
    default:
      populate_channel_single(h[0][0]);
  }
}

static void awgn(cf_t* y[SRSRAN_MAX_PORTS], uint32_t n, float snr)
{
  int   i;
  float var = srsran_convert_dB_to_power(-snr) * scaling * scaling;

  for (i = 0; i < nof_rx_ports; i++) {
    srsran_ch_awgn_c(y[i], y[i], var, n);
  }
}

int main(int argc, char** argv)
{
  int   i, j, k, nof_errors = 0, ret = SRSRAN_SUCCESS;
  float mse;
  cf_t *x[SRSRAN_MAX_LAYERS], *r[SRSRAN_MAX_PORTS], *y[SRSRAN_MAX_PORTS], *h[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS],
      *xr[SRSRAN_MAX_LAYERS];
  srsran_tx_scheme_t type;

  parse_args(argc, argv);

  /* Check input ranges */
  if (nof_tx_ports > SRSRAN_MAX_PORTS || nof_rx_ports > SRSRAN_MAX_PORTS || nof_layers > SRSRAN_MAX_LAYERS) {
    ERROR("Invalid number of layers or ports");
    exit(-1);
  }

  /* Parse MIMO Type */
  if (srsran_str2mimotype(mimo_type_name, &type)) {
    ERROR("Invalid MIMO type %s", mimo_type_name);
    exit(-1);
  }

  /* Check scenario conditions are OK */
  switch (type) {
    case SRSRAN_TXSCHEME_DIVERSITY:
      nof_re = nof_layers * nof_symbols;
      break;
    case SRSRAN_TXSCHEME_SPATIALMUX:
      nof_re = nof_symbols;
      break;
    case SRSRAN_TXSCHEME_CDD:
      nof_re = nof_symbols * nof_tx_ports / nof_layers;
      if (nof_rx_ports != 2 || nof_tx_ports != 2) {
        ERROR("CDD nof_tx_ports=%d nof_rx_ports=%d is not currently supported", nof_tx_ports, nof_rx_ports);
        exit(-1);
      }
      break;
    default:
      nof_re = nof_symbols * nof_layers;
  }

  /* Allocate x and xr (received symbols) in memory for each layer */
  for (i = 0; i < nof_layers; i++) {
    /* Source data */
    x[i] = srsran_vec_cf_malloc(nof_symbols);
    if (!x[i]) {
      perror("srsran_vec_malloc");
      exit(-1);
    }

    /* Sink data */
    xr[i] = srsran_vec_cf_malloc(nof_symbols);
    if (!xr[i]) {
      perror("srsran_vec_malloc");
      exit(-1);
    }
  }

  /* Allocate y in memory for tx each port */
  for (i = 0; i < nof_tx_ports; i++) {
    y[i] = srsran_vec_cf_malloc(nof_re);
    if (!y[i]) {
      perror("srsran_vec_malloc");
      exit(-1);
    }
  }

  /* Allocate h in memory for each cross channel and layer */
  for (i = 0; i < nof_tx_ports; i++) {
    for (j = 0; j < nof_rx_ports; j++) {
      h[i][j] = srsran_vec_cf_malloc(nof_re);
      if (!h[i][j]) {
        perror("srsran_vec_malloc");
        exit(-1);
      }
    }
  }

  /* Allocate r */
  for (i = 0; i < nof_rx_ports; i++) {
    r[i] = srsran_vec_cf_malloc(nof_re);
    if (!r[i]) {
      perror("srsran_vec_malloc");
      exit(-1);
    }
  }

  /* Generate source random data */
  random_gen = srsran_random_init(0);
  for (i = 0; i < nof_layers; i++) {
    for (j = 0; j < nof_symbols; j++) {
      __real__ x[i][j] = (2 * srsran_random_uniform_int_dist(random_gen, 0, 1) - 1) * M_SQRT1_2;
      __imag__ x[i][j] = (2 * srsran_random_uniform_int_dist(random_gen, 0, 1) - 1) * M_SQRT1_2;
    }
  }

  /* Execute Precoding (Tx) */
  if (srsran_precoding_type(x, y, nof_layers, nof_tx_ports, codebook_idx, nof_symbols, scaling, type) < 0) {
    ERROR("Error layer mapper encoder");
    exit(-1);
  }

  /* generate channel */
  populate_channel(type, h);

  /* pass signal through channel
   (we are in the frequency domain so it's a multiplication) */
  for (i = 0; i < nof_rx_ports; i++) {
    for (k = 0; k < nof_re; k++) {
      r[i][k] = (cf_t)(0.0 + 0.0 * _Complex_I);
      for (j = 0; j < nof_tx_ports; j++) {
        r[i][k] += y[j][k] * h[j][i][k];
      }
    }
  }

  awgn(r, (uint32_t)nof_re, snr_db);

  /* If CDD or Spatial muliplex choose decoder */
  if (strncmp(decoder_type_name, "zf", 16) == 0) {
    srsran_predecoding_set_mimo_decoder(SRSRAN_MIMO_DECODER_ZF);
  } else if (strncmp(decoder_type_name, "mmse", 16) == 0) {
    srsran_predecoding_set_mimo_decoder(SRSRAN_MIMO_DECODER_MMSE);
  } else {
    ret = SRSRAN_ERROR;
    goto quit;
  }

  /* predecoding / equalization */
  struct timeval t[3];
  gettimeofday(&t[1], NULL);
  srsran_predecoding_type(r,
                          h,
                          xr,
                          NULL,
                          nof_rx_ports,
                          nof_tx_ports,
                          nof_layers,
                          codebook_idx,
                          nof_re,
                          type,
                          scaling,
                          srsran_convert_dB_to_power(-snr_db));
  gettimeofday(&t[2], NULL);
  get_time_interval(t);

  /* check errors */
  mse = 0;
  for (i = 0; i < nof_layers; i++) {
    for (j = 0; j < nof_symbols; j++) {
      mse += cabsf(xr[i][j] - x[i][j]);

      if ((crealf(xr[i][j]) > 0) != (crealf(x[i][j]) > 0)) {
        nof_errors++;
      }
      if ((cimagf(xr[i][j]) > 0) != (cimagf(x[i][j]) > 0)) {
        nof_errors++;
      }
    }
  }
  printf("SNR: %5.1fdB;\tExecution time: %5ldus;\tMSE: %.6f;\tBER: %.6f\n",
         snr_db,
         t[0].tv_usec,
         mse / nof_layers / nof_symbols,
         (float)nof_errors / (4.0f * nof_re));
  if (mse / nof_layers / nof_symbols > MSE_THRESHOLD) {
    ret = SRSRAN_ERROR;
  }

quit:
  srsran_random_free(random_gen);

  /* Free all data */
  for (i = 0; i < nof_layers; i++) {
    free(x[i]);
    free(xr[i]);
  }

  for (i = 0; i < nof_rx_ports; i++) {
    free(r[i]);
  }

  for (i = 0; i < nof_rx_ports; i++) {
    for (j = 0; j < nof_tx_ports; j++) {
      free(h[j][i]);
    }
  }

  for (i = 0; i < nof_tx_ports; i++) {
    if (y[i]) {
      free(y[i]);
    }
  }

  exit(ret);
}
