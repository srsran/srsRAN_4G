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

#ifndef PRECODING_H_
#define PRECODING_H_

#include "liblte/config.h"

typedef _Complex float cf_t;

/** The precoder takes as input nlayers vectors "x" from the
 * layer mapping and generates nports vectors "y" to be mapped onto
 * resources on each of the antenna ports.
 */


typedef struct {
  cf_t *h_mod; 
  float *y_mod; 
  float *z_real; 
  float *z_imag; 
  uint32_t max_frame_len;
}precoding_t; 


LIBLTE_API int precoding_init(precoding_t *q, 
                              uint32_t max_frame_len); 

LIBLTE_API void precoding_free(precoding_t *q);

/* Generates the vector "y" from the input vector "x"
 */
LIBLTE_API int precoding_single(precoding_t *q, 
                                cf_t *x, 
                                cf_t *y, 
                                int nof_symbols);

LIBLTE_API int precoding_diversity(precoding_t *q, 
                                   cf_t *x[MAX_LAYERS], 
                                   cf_t *y[MAX_PORTS], 
                                   int nof_ports, int nof_symbols);

LIBLTE_API int precoding_type(precoding_t *q, 
                              cf_t *x[MAX_LAYERS], 
                              cf_t *y[MAX_PORTS], 
                              int nof_layers,
                              int nof_ports, 
                              int nof_symbols, 
                              lte_mimo_type_t type);

/* Estimates the vector "x" based on the received signal "y" and the channel estimates "h"
 */
LIBLTE_API int predecoding_single_zf(precoding_t *q, 
                                     cf_t *y, 
                                     cf_t *h, 
                                     cf_t *x, 
                                     int nof_symbols);

LIBLTE_API int predecoding_single_mmse(precoding_t *q, 
                                       cf_t *y, 
                                       cf_t *h, 
                                       cf_t *x, 
                                       int nof_symbols, 
                                       float noise_estimate);

LIBLTE_API int predecoding_diversity_zf(precoding_t *q, 
                                        cf_t *y, 
                                        cf_t *h[MAX_PORTS], 
                                        cf_t *x[MAX_LAYERS],    
                                        int nof_ports, 
                                        int nof_symbols);

LIBLTE_API int predecoding_type(precoding_t *q, 
                                cf_t *y, 
                                cf_t *h[MAX_PORTS], 
                                cf_t *x[MAX_LAYERS],
                                int nof_ports, 
                                int nof_layers, 
                                int nof_symbols, 
                                lte_mimo_type_t type);

#endif /* PRECODING_H_ */
