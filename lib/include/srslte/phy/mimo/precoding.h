/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

/******************************************************************************
 *  File:         precoding.h
 *
 *  Description:  MIMO precoding and deprecoding.
 *                Single antenna and tx diversity supported.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.3.4
 *****************************************************************************/

#ifndef SRSLTE_PRECODING_H
#define SRSLTE_PRECODING_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"

/** The precoder takes as input nlayers vectors "x" from the
 * layer mapping and generates nports vectors "y" to be mapped onto
 * resources on each of the antenna ports.
 */


/* Generates the vector "y" from the input vector "x"
 */
SRSLTE_API int srslte_precoding_single(cf_t *x, 
                                       cf_t *y, 
                                       int nof_symbols,
                                       float scaling);

SRSLTE_API int srslte_precoding_diversity(cf_t *x[SRSLTE_MAX_LAYERS], 
                                          cf_t *y[SRSLTE_MAX_PORTS], 
                                          int nof_ports, 
                                          int nof_symbols,
                                          float scaling);

SRSLTE_API int srslte_precoding_cdd(cf_t *x[SRSLTE_MAX_LAYERS], 
                                    cf_t *y[SRSLTE_MAX_PORTS], 
                                    int nof_layers, 
                                    int nof_ports, 
                                    int nof_symbols,
                                    float scaling);

SRSLTE_API int srslte_precoding_type(cf_t *x[SRSLTE_MAX_LAYERS], 
                                     cf_t *y[SRSLTE_MAX_PORTS], 
                                     int nof_layers,
                                     int nof_ports,
                                     int codebook_idx,
                                     int nof_symbols,
                                     float scaling,
                                     srslte_mimo_type_t type);

/* Estimates the vector "x" based on the received signal "y" and the channel estimates "h"
 */
SRSLTE_API int srslte_predecoding_single(cf_t *y, 
                                         cf_t *h, 
                                         cf_t *x, 
                                         float *csi,
                                         int nof_symbols, 
                                         float scaling,
                                         float noise_estimate);

SRSLTE_API int srslte_predecoding_single_multi(cf_t *y[SRSLTE_MAX_PORTS], 
                                               cf_t *h[SRSLTE_MAX_PORTS], 
                                               cf_t *x, 
                                               float *csi[SRSLTE_MAX_CODEWORDS],
                                               int nof_rxant,
                                               int nof_symbols, 
                                               float scaling,
                                               float noise_estimate);

SRSLTE_API int srslte_predecoding_diversity(cf_t *y, 
                                            cf_t *h[SRSLTE_MAX_PORTS], 
                                            cf_t *x[SRSLTE_MAX_LAYERS],    
                                            int nof_ports, 
                                            int nof_symbols,
                                            float scaling);

SRSLTE_API int srslte_predecoding_diversity_multi(cf_t *y[SRSLTE_MAX_PORTS], 
                                                  cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], 
                                                  cf_t *x[SRSLTE_MAX_LAYERS],
                                                  float *csi[SRSLTE_MAX_LAYERS],
                                                  int nof_rxant,
                                                  int nof_ports, 
                                                  int nof_symbols,
                                                  float scaling);

SRSLTE_API void srslte_predecoding_set_mimo_decoder (srslte_mimo_decoder_t _mimo_decoder);

SRSLTE_API int srslte_predecoding_type(cf_t *y[SRSLTE_MAX_PORTS],
                                       cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                       cf_t *x[SRSLTE_MAX_LAYERS],
                                       float *csi[SRSLTE_MAX_CODEWORDS],
                                       int nof_rxant,
                                       int nof_ports,
                                       int nof_layers,
                                       int codebook_idx,
                                       int nof_symbols,
                                       srslte_mimo_type_t type,
                                       float scaling,
                                       float noise_estimate);

SRSLTE_API int srslte_precoding_pmi_select(cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                           uint32_t nof_symbols,
                                           float noise_estimate,
                                           int nof_layers,
                                           uint32_t *pmi,
                                           float sinr[SRSLTE_MAX_CODEBOOKS]);

SRSLTE_API int srslte_precoding_cn(cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                   uint32_t nof_tx_antennas,
                                   uint32_t nof_rx_antennas,
                                   uint32_t nof_symbols,
                                   float *cn);


#endif // SRSLTE_PRECODING_H
