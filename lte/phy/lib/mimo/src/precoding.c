/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <stdlib.h>
#include <assert.h>
#include <complex.h>
#include <string.h>
#include <math.h>

#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/mimo/precoding.h"
#include "liblte/phy/utils/vector.h"


/************************************************
 * 
 * RECEIVER SIDE FUNCTIONS
 * 
 **************************************************/

int precoding_init(precoding_t *q, uint32_t max_frame_len) {
  if (q) {
    bzero(q, sizeof(precoding_t));
    
    q->h_mod = vec_malloc(sizeof(cf_t) * max_frame_len);
    if (!q->h_mod) {
      perror("malloc");
      goto clean_exit; 
    }
    q->y_mod = vec_malloc(sizeof(float) * max_frame_len);
    if (!q->y_mod) {
      perror("malloc");
      goto clean_exit; 
    }
    q->z_real = vec_malloc(sizeof(float) * max_frame_len);
    if (!q->z_real) {
      perror("malloc");
      goto clean_exit; 
    }
    q->z_imag = vec_malloc(sizeof(float) * max_frame_len);
    if (!q->z_imag) {
      perror("malloc");
      goto clean_exit; 
    }
    q->max_frame_len = max_frame_len; 
    return LIBLTE_SUCCESS; 
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS; 
  }
clean_exit:
  precoding_free(q);
  return LIBLTE_ERROR; 
}

void precoding_free(precoding_t *q) {
  if (q->h_mod) {
    free(q->h_mod);
  }
  if (q->y_mod) {
    free(q->y_mod);
  }
  if (q->z_real) {
    free(q->z_real);
  }
  if (q->z_imag) {
    free(q->z_imag);
  }
  bzero(q, sizeof(precoding_t));
}


/* ZF SISO equalizer: x=y/h */
int predecoding_single_zf(precoding_t *q, cf_t *y, cf_t *h, cf_t *x, int nof_symbols) {
  if (nof_symbols <= q->max_frame_len) {
    vec_div_ccc(y, h, q->y_mod, x, q->z_real, q->z_imag, nof_symbols);    
    return nof_symbols;
  } else {
    return LIBLTE_ERROR; 
  }
}

/* MMSE SISO equalizer x=y*h'/(h*h'+no) */
int predecoding_single_mmse(precoding_t *q, cf_t *y, cf_t *h, cf_t *x, int nof_symbols, float noise_estimate) {
  if (nof_symbols <= q->max_frame_len) {
    // h*h'
    vec_prod_conj_ccc(h, h, q->h_mod, nof_symbols);
    // real(h*h')
    vec_deinterleave_real_cf(q->h_mod, q->y_mod, nof_symbols);
    // (h*h' + n0)
    vec_sc_add_fff(q->y_mod, noise_estimate, q->y_mod, nof_symbols);
    // y*h'
    vec_prod_conj_ccc(y, h, x, nof_symbols);
    // decompose real/imag parts
    vec_deinterleave_cf(x, q->z_real, q->z_imag, nof_symbols);
    // real and imag division
    vec_div_fff(q->z_real, q->y_mod, q->z_real, nof_symbols);
    vec_div_fff(q->z_imag, q->y_mod, q->z_imag, nof_symbols);
    // interleave again 
    vec_interleave_cf(q->z_real, q->z_imag, x, nof_symbols);
    return nof_symbols;
  } else {
    return LIBLTE_ERROR; 
  }
}

/* ZF STBC equalizer */
int predecoding_diversity_zf(precoding_t *q, cf_t *y, cf_t *h[MAX_PORTS], cf_t *x[MAX_LAYERS],
    int nof_ports, int nof_symbols) {
  int i;
  cf_t h0, h1, h2, h3, r0, r1, r2, r3;
  float hh, hh02, hh13;
  if (nof_ports == 2) {
    /* TODO: Use VOLK here */
    for (i = 0; i < nof_symbols / 2; i++) {
      h0 = h[0][2 * i];
      h1 = h[1][2 * i];
      hh = crealf(h0) * crealf(h0) + cimagf(h0) * cimagf(h0)
          + crealf(h1) * crealf(h1) + cimagf(h1) * cimagf(h1);
      r0 = y[2 * i];
      r1 = y[2 * i + 1];
      if (hh == 0) {
        hh = 1e-2;
      }
      x[0][i] = (conjf(h0) * r0 + h1 * conjf(r1)) / hh * sqrt(2);
      x[1][i] = (-h1 * conj(r0) + conj(h0) * r1) / hh * sqrt(2);
    }
    return i;
  } else if (nof_ports == 4) {

    int m_ap = (nof_symbols % 4) ? ((nof_symbols - 2) / 4) : nof_symbols / 4;
    for (i = 0; i < m_ap; i++) {
      h0 = h[0][4 * i];
      h1 = h[1][4 * i + 2];
      h2 = h[2][4 * i];
      h3 = h[3][4 * i + 2];
      hh02 = crealf(h0) * crealf(h0) + cimagf(h0) * cimagf(h0)
          + crealf(h2) * crealf(h2) + cimagf(h2) * cimagf(h2);
      hh13 = crealf(h1) * crealf(h1) + cimagf(h1) * cimagf(h1)
          + crealf(h3) * crealf(h3) + cimagf(h3) * cimagf(h3);
      r0 = y[4 * i];
      r1 = y[4 * i + 1];
      r2 = y[4 * i + 2];
      r3 = y[4 * i + 3];

      x[0][i] = (conjf(h0) * r0 + h2 * conjf(r1)) / hh02 * sqrt(2);
      x[1][i] = (-h2 * conjf(r0) + conjf(h0) * r1) / hh02 * sqrt(2);
      x[2][i] = (conjf(h1) * r2 + h3 * conjf(r3)) / hh13 * sqrt(2);
      x[3][i] = (-h3 * conjf(r2) + conjf(h1) * r3) / hh13 * sqrt(2);

    }
    return i;
  } else {
    fprintf(stderr, "Number of ports must be 2 or 4 for transmit diversity (nof_ports=%d)\n", nof_ports);
    return -1;
  }
}

/* 36.211 v10.3.0 Section 6.3.4 */
int predecoding_type(precoding_t *q, cf_t *y, cf_t *h[MAX_PORTS], cf_t *x[MAX_LAYERS],
    int nof_ports, int nof_layers, int nof_symbols, lte_mimo_type_t type) {

  if (nof_ports > MAX_PORTS) {
    fprintf(stderr, "Maximum number of ports is %d (nof_ports=%d)\n", MAX_PORTS,
        nof_ports);
    return -1;
  }
  if (nof_layers > MAX_LAYERS) {
    fprintf(stderr, "Maximum number of layers is %d (nof_layers=%d)\n",
        MAX_LAYERS, nof_layers);
    return -1;
  }

  switch (type) {
  case SINGLE_ANTENNA:
    if (nof_ports == 1 && nof_layers == 1) {
      return predecoding_single_zf(q, y, h[0], x[0], nof_symbols);
    } else {
      fprintf(stderr,
          "Number of ports and layers must be 1 for transmission on single antenna ports\n");
      return -1;
    }
    break;
  case TX_DIVERSITY:
    if (nof_ports == nof_layers) {
      return predecoding_diversity_zf(q, y, h, x, nof_ports, nof_symbols);
    } else {
      fprintf(stderr,
          "Error number of layers must equal number of ports in transmit diversity\n");
      return -1;
    }
    break;
  case SPATIAL_MULTIPLEX:
    fprintf(stderr, "Spatial multiplexing not supported\n");
    return -1;
  }
  return 0;
}






/************************************************
 * 
 * TRANSMITTER SIDE FUNCTIONS
 * 
 **************************************************/

int precoding_single(precoding_t *q, cf_t *x, cf_t *y, int nof_symbols) {
  memcpy(y, x, nof_symbols * sizeof(cf_t));
  return nof_symbols;
}
int precoding_diversity(precoding_t *q, cf_t *x[MAX_LAYERS], cf_t *y[MAX_PORTS], int nof_ports,
    int nof_symbols) {
  int i;
  if (nof_ports == 2) {
    /* FIXME: Use VOLK here */
    for (i = 0; i < nof_symbols; i++) {
      y[0][2 * i] = x[0][i] / sqrtf(2);
      y[1][2 * i] = -conjf(x[1][i]) / sqrtf(2);
      y[0][2 * i + 1] = x[1][i] / sqrtf(2);
      y[1][2 * i + 1] = conjf(x[0][i]) / sqrtf(2);
    }
    return 2 * i;
  } else if (nof_ports == 4) {
    //int m_ap = (nof_symbols%4)?(nof_symbols*4-2):nof_symbols*4;
    int m_ap = 4 * nof_symbols;
    for (i = 0; i < m_ap / 4; i++) {
      y[0][4 * i] = x[0][i] / sqrtf(2);
      y[1][4 * i] = 0;
      y[2][4 * i] = -conjf(x[1][i]) / sqrtf(2);
      y[3][4 * i] = 0;

      y[0][4 * i + 1] = x[1][i] / sqrtf(2);
      y[1][4 * i + 1] = 0;
      y[2][4 * i + 1] = conjf(x[0][i]) / sqrtf(2);
      y[3][4 * i + 1] = 0;

      y[0][4 * i + 2] = 0;
      y[1][4 * i + 2] = x[2][i] / sqrtf(2);
      y[2][4 * i + 2] = 0;
      y[3][4 * i + 2] = -conjf(x[3][i]) / sqrtf(2);

      y[0][4 * i + 3] = 0;
      y[1][4 * i + 3] = x[3][i] / sqrtf(2);
      y[2][4 * i + 3] = 0;
      y[3][4 * i + 3] = conjf(x[2][i]) / sqrtf(2);
    }
    return 4 * i;
  } else {
    fprintf(stderr, "Number of ports must be 2 or 4 for transmit diversity (nof_ports=%d)\n", nof_ports);
    return -1;
  }
}

/* 36.211 v10.3.0 Section 6.3.4 */
int precoding_type(precoding_t *q, cf_t *x[MAX_LAYERS], cf_t *y[MAX_PORTS], int nof_layers,
    int nof_ports, int nof_symbols, lte_mimo_type_t type) {

  if (nof_ports > MAX_PORTS) {
    fprintf(stderr, "Maximum number of ports is %d (nof_ports=%d)\n", MAX_PORTS,
        nof_ports);
    return -1;
  }
  if (nof_layers > MAX_LAYERS) {
    fprintf(stderr, "Maximum number of layers is %d (nof_layers=%d)\n",
        MAX_LAYERS, nof_layers);
    return -1;
  }

  switch (type) {
  case SINGLE_ANTENNA:
    if (nof_ports == 1 && nof_layers == 1) {
      return precoding_single(q, x[0], y[0], nof_symbols);
    } else {
      fprintf(stderr,
          "Number of ports and layers must be 1 for transmission on single antenna ports\n");
      return -1;
    }
    break;
  case TX_DIVERSITY:
    if (nof_ports == nof_layers) {
      return precoding_diversity(q, x, y, nof_ports, nof_symbols);
    } else {
      fprintf(stderr,
          "Error number of layers must equal number of ports in transmit diversity\n");
      return -1;
    }
  case SPATIAL_MULTIPLEX:
    fprintf(stderr, "Spatial multiplexing not supported\n");
    return -1;
  }
  return 0;
}

