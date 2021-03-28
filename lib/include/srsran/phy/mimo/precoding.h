/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

#ifndef SRSRAN_PRECODING_H
#define SRSRAN_PRECODING_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"

/** The precoder takes as input nlayers vectors "x" from the
 * layer mapping and generates nports vectors "y" to be mapped onto
 * resources on each of the antenna ports.
 */

/* Generates the vector "y" from the input vector "x"
 */
SRSRAN_API int srsran_precoding_single(cf_t* x, cf_t* y, int nof_symbols, float scaling);

SRSRAN_API int srsran_precoding_diversity(cf_t* x[SRSRAN_MAX_LAYERS],
                                          cf_t* y[SRSRAN_MAX_PORTS],
                                          int   nof_ports,
                                          int   nof_symbols,
                                          float scaling);

SRSRAN_API int srsran_precoding_cdd(cf_t* x[SRSRAN_MAX_LAYERS],
                                    cf_t* y[SRSRAN_MAX_PORTS],
                                    int   nof_layers,
                                    int   nof_ports,
                                    int   nof_symbols,
                                    float scaling);

SRSRAN_API int srsran_precoding_type(cf_t*              x[SRSRAN_MAX_LAYERS],
                                     cf_t*              y[SRSRAN_MAX_PORTS],
                                     int                nof_layers,
                                     int                nof_ports,
                                     int                codebook_idx,
                                     int                nof_symbols,
                                     float              scaling,
                                     srsran_tx_scheme_t type);

/* Estimates the vector "x" based on the received signal "y" and the channel estimates "h"
 */
SRSRAN_API int
srsran_predecoding_single(cf_t* y, cf_t* h, cf_t* x, float* csi, int nof_symbols, float scaling, float noise_estimate);

SRSRAN_API int srsran_predecoding_single_multi(cf_t*  y[SRSRAN_MAX_PORTS],
                                               cf_t*  h[SRSRAN_MAX_PORTS],
                                               cf_t*  x,
                                               float* csi[SRSRAN_MAX_CODEWORDS],
                                               int    nof_rxant,
                                               int    nof_symbols,
                                               float  scaling,
                                               float  noise_estimate);

SRSRAN_API int srsran_predecoding_diversity(cf_t* y,
                                            cf_t* h[SRSRAN_MAX_PORTS],
                                            cf_t* x[SRSRAN_MAX_LAYERS],
                                            int   nof_ports,
                                            int   nof_symbols,
                                            float scaling);

SRSRAN_API int srsran_predecoding_diversity_multi(cf_t*  y[SRSRAN_MAX_PORTS],
                                                  cf_t*  h[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS],
                                                  cf_t*  x[SRSRAN_MAX_LAYERS],
                                                  float* csi[SRSRAN_MAX_LAYERS],
                                                  int    nof_rxant,
                                                  int    nof_ports,
                                                  int    nof_symbols,
                                                  float  scaling);

SRSRAN_API void srsran_predecoding_set_mimo_decoder(srsran_mimo_decoder_t _mimo_decoder);

SRSRAN_API int srsran_predecoding_type(cf_t*              y[SRSRAN_MAX_PORTS],
                                       cf_t*              h[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS],
                                       cf_t*              x[SRSRAN_MAX_LAYERS],
                                       float*             csi[SRSRAN_MAX_CODEWORDS],
                                       int                nof_rxant,
                                       int                nof_ports,
                                       int                nof_layers,
                                       int                codebook_idx,
                                       int                nof_symbols,
                                       srsran_tx_scheme_t type,
                                       float              scaling,
                                       float              noise_estimate);

SRSRAN_API int srsran_precoding_pmi_select(cf_t*     h[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS],
                                           uint32_t  nof_symbols,
                                           float     noise_estimate,
                                           int       nof_layers,
                                           uint32_t* pmi,
                                           float     sinr[SRSRAN_MAX_CODEBOOKS]);

SRSRAN_API int srsran_precoding_cn(cf_t*    h[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS],
                                   uint32_t nof_tx_antennas,
                                   uint32_t nof_rx_antennas,
                                   uint32_t nof_symbols,
                                   float*   cn);

#endif // SRSRAN_PRECODING_H
