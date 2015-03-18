/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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

#include "srslte/config.h"

typedef _Complex float cf_t;

/** The precoder takes as input nlayers vectors "x" from the
 * layer mapping and generates nports vectors "y" to be mapped onto
 * resources on each of the antenna ports.
 */


typedef struct {
  cf_t *h_mod; 
  cf_t *tmp1; 
  cf_t *tmp2; 
  cf_t *tmp3; 
  float *y_mod; 
  float *z_real; 
  float *z_imag; 
  uint32_t max_frame_len;
}precoding_t; 


SRSLTE_API int precoding_init(precoding_t *q, 
                              uint32_t max_frame_len); 

SRSLTE_API void precoding_free(precoding_t *q);

/* Generates the vector "y" from the input vector "x"
 */
SRSLTE_API int precoding_single(precoding_t *q, 
                                cf_t *x, 
                                cf_t *y, 
                                int nof_symbols);

SRSLTE_API int precoding_diversity(precoding_t *q, 
                                   cf_t *x[SRSLTE_MAX_LAYERS], 
                                   cf_t *y[SRSLTE_MAX_PORTS], 
                                   int nof_ports, int nof_symbols);

SRSLTE_API int precoding_type(precoding_t *q, 
                              cf_t *x[SRSLTE_MAX_LAYERS], 
                              cf_t *y[SRSLTE_MAX_PORTS], 
                              int nof_layers,
                              int nof_ports, 
                              int nof_symbols, 
                              srslte_mimo_type_t type);

/* Estimates the vector "x" based on the received signal "y" and the channel estimates "h"
 */
SRSLTE_API int predecoding_single(precoding_t *q, 
                                  cf_t *y, 
                                  cf_t *h, 
                                  cf_t *x, 
                                  int nof_symbols, 
                                  float noise_estimate);

SRSLTE_API int predecoding_diversity(precoding_t *q, 
                                     cf_t *y, 
                                     cf_t *h[SRSLTE_MAX_PORTS], 
                                     cf_t *x[SRSLTE_MAX_LAYERS],    
                                     int nof_ports, 
                                     int nof_symbols, 
                                     float noise_estimate);

SRSLTE_API int predecoding_type(precoding_t *q, 
                                cf_t *y, 
                                cf_t *h[SRSLTE_MAX_PORTS], 
                                cf_t *x[SRSLTE_MAX_LAYERS],
                                int nof_ports, 
                                int nof_layers, 
                                int nof_symbols, 
                                srslte_mimo_type_t type, 
                                float noise_estimate);

#endif /* PRECODING_H_ */
