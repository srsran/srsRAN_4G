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

#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/config.h"
#include "srslte/phy/utils/debug.h"

#include "srslte/phy/ch_estimation/chest_dl_nbiot.h"
#include "srslte/phy/utils/convolution.h"
#include "srslte/phy/utils/vector.h"

int srslte_chest_dl_nbiot_init(srslte_chest_dl_nbiot_t* q, uint32_t max_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    bzero(q, sizeof(srslte_chest_dl_nbiot_t));

    ret = srslte_refsignal_dl_nbiot_init(&q->nrs_signal);
    if (ret != SRSLTE_SUCCESS) {
      fprintf(stderr, "Error initializing CSR signal (%d)\n", ret);
      goto clean_exit;
    }

    q->tmp_noise = srslte_vec_cf_malloc(SRSLTE_REFSIGNAL_MAX_NUM_SF(max_prb));
    if (!q->tmp_noise) {
      perror("malloc");
      goto clean_exit;
    }
    q->pilot_estimates = srslte_vec_cf_malloc(SRSLTE_REFSIGNAL_MAX_NUM_SF(max_prb) + SRSLTE_CHEST_MAX_SMOOTH_FIL_LEN);
    if (!q->pilot_estimates) {
      perror("malloc");
      goto clean_exit;
    }
    q->pilot_estimates_average =
        srslte_vec_cf_malloc(SRSLTE_REFSIGNAL_MAX_NUM_SF(max_prb) + SRSLTE_CHEST_MAX_SMOOTH_FIL_LEN);
    if (!q->pilot_estimates_average) {
      perror("malloc");
      goto clean_exit;
    }
    for (int i = 0; i < SRSLTE_REFSIGNAL_MAX_NUM_SF(max_prb); i++) {
      q->pilot_estimates_average[i] = 1;
    }

    q->pilot_recv_signal = srslte_vec_cf_malloc(SRSLTE_REFSIGNAL_MAX_NUM_SF(max_prb));
    if (!q->pilot_recv_signal) {
      perror("malloc");
      goto clean_exit;
    }

    if (srslte_interp_linear_vector_init(&q->srslte_interp_linvec, SRSLTE_NRE * max_prb)) {
      fprintf(stderr, "Error initializing vector interpolator\n");
      goto clean_exit;
    }

    if (srslte_interp_linear_init(&q->srslte_interp_lin, 2 * max_prb, SRSLTE_NRE / 2)) {
      fprintf(stderr, "Error initializing interpolator\n");
      goto clean_exit;
    }

    q->noise_alg = SRSLTE_NOISE_ALG_REFS;
    srslte_chest_dl_nbiot_set_smooth_filter3_coeff(q, 0.1);

    ret = SRSLTE_SUCCESS;
  }

clean_exit:
  if (ret != SRSLTE_SUCCESS) {
    srslte_chest_dl_nbiot_free(q);
  }
  return ret;
}

void srslte_chest_dl_nbiot_free(srslte_chest_dl_nbiot_t* q)
{
  srslte_refsignal_dl_nbiot_free(&q->nrs_signal);

  if (q->tmp_noise) {
    free(q->tmp_noise);
  }
  srslte_interp_linear_vector_free(&q->srslte_interp_linvec);
  srslte_interp_linear_free(&q->srslte_interp_lin);

  if (q->pilot_estimates) {
    free(q->pilot_estimates);
  }
  if (q->pilot_estimates_average) {
    free(q->pilot_estimates_average);
  }
  if (q->pilot_recv_signal) {
    free(q->pilot_recv_signal);
  }
}

int srslte_chest_dl_nbiot_set_cell(srslte_chest_dl_nbiot_t* q, srslte_nbiot_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  // set ports configuration
  if (cell.nof_ports == 0) {
    cell.nof_ports = SRSLTE_NBIOT_MAX_PORTS;
  }

  if (q != NULL && srslte_nbiot_cell_isvalid(&cell)) {
    if (q->cell.n_id_ncell != cell.n_id_ncell || q->cell.base.nof_prb == 0) {
      q->cell = cell;

      ret = srslte_refsignal_dl_nbiot_set_cell(&q->nrs_signal, cell);
      if (ret != SRSLTE_SUCCESS) {
        fprintf(stderr, "Error initializing NRS signal (%d)\n", ret);
        return SRSLTE_ERROR;
      }

      if (srslte_interp_linear_vector_resize(&q->srslte_interp_linvec, SRSLTE_NRE * cell.base.nof_prb)) {
        fprintf(stderr, "Error initializing vector interpolator\n");
        return SRSLTE_ERROR;
      }

      if (srslte_interp_linear_resize(&q->srslte_interp_lin, 2 * cell.base.nof_prb, SRSLTE_NRE / 2)) {
        fprintf(stderr, "Error initializing interpolator\n");
        return SRSLTE_ERROR;
      }
    }
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

/* Uses the difference between the averaged and non-averaged pilot estimates */
static float estimate_noise_pilots(srslte_chest_dl_nbiot_t* q, uint32_t port_id)
{
  int nref = srslte_refsignal_cs_nof_re(NULL, NULL, port_id);

  // Substract noisy pilot estimates
  srslte_vec_sub_ccc(q->pilot_estimates_average, q->pilot_estimates, q->tmp_noise, nref);

  // Compute average power. Normalized for filter len 3 using ML
  float norm = 1;
  if (q->smooth_filter_len == 3) {
    float a     = q->smooth_filter[0];
    float norm3 = 6.143 * a * a + 0.04859 * a - 0.002774;
    norm /= norm3;
  }
  float power = norm * q->cell.nof_ports * srslte_vec_avg_power_cf(q->tmp_noise, nref);
  return power;
}

#define cesymb(i) ce[SRSLTE_RE_IDX(q->cell.base.nof_prb, (i), 0)]

static void interpolate_pilots(srslte_chest_dl_nbiot_t* q, cf_t* pilot_estimates, cf_t* ce, uint32_t port_id)
{
  uint32_t nsymbols = srslte_refsignal_cs_nof_symbols(NULL, NULL, port_id);
  int      num_ces  = q->cell.base.nof_prb * SRSLTE_NRE;
  cf_t     ce_avg[2][num_ces];

  // interpolate the symbols with references in the freq domain
  DEBUG("Interpolating %d pilots in %d symbols at port %d.\n", nsymbols * 2, nsymbols, port_id);
  for (int l = 0; l < nsymbols; l++) {
    uint32_t fidx_offset = srslte_refsignal_dl_nbiot_fidx(q->cell, l, port_id, 0);
    // points to the RE of the beginning of the l'th symbol containing a ref
    uint32_t ce_idx = srslte_refsignal_nrs_nsymbol(l) * q->cell.base.nof_prb * SRSLTE_NRE;
    ce_idx += q->cell.nbiot_prb * SRSLTE_NRE;
    DEBUG("  - freq.-dmn interp. in sym %d at RE %d\n", srslte_refsignal_nrs_nsymbol(l), ce_idx + fidx_offset);
    srslte_interp_linear_offset(
        &q->srslte_interp_lin, &pilot_estimates[2 * l], &ce[ce_idx], fidx_offset, SRSLTE_NRE / 2 - fidx_offset);
  }

  // average the two neighbor subcarriers containing pilots
  for (int l = 0; l < nsymbols / 2; l++) {
    int sym_idx = srslte_refsignal_nrs_nsymbol(l * 2);
    srslte_vec_sum_ccc(&cesymb(sym_idx), &cesymb(sym_idx + 1), ce_avg[l], num_ces);
    // TODO: use vector operation for this
    for (int k = 0; k < num_ces; k++) {
      ce_avg[l][k] /= 2;
    }
    // use avarage for both symbols
    memcpy(&cesymb(sym_idx), ce_avg[l], num_ces * sizeof(cf_t));
    memcpy(&cesymb(sym_idx + 1), ce_avg[l], num_ces * sizeof(cf_t));
  }

  // now interpolate in the time domain between symbols
  srslte_interp_linear_vector(&q->srslte_interp_linvec, ce_avg[0], ce_avg[1], &cesymb(0), 5, 5);
  srslte_interp_linear_vector(&q->srslte_interp_linvec, ce_avg[0], ce_avg[1], &cesymb(7), 5, 5);
}

void srslte_chest_dl_nbiot_set_smooth_filter(srslte_chest_dl_nbiot_t* q, float* filter, uint32_t filter_len)
{
  if (filter_len < SRSLTE_CHEST_MAX_SMOOTH_FIL_LEN) {
    if (filter) {
      memcpy(q->smooth_filter, filter, filter_len * sizeof(float));
      q->smooth_filter_len = filter_len;
    } else {
      q->smooth_filter_len = 0;
    }
  } else {
    fprintf(stderr,
            "Error setting smoothing filter: filter len exceeds maximum (%d>%d)\n",
            filter_len,
            SRSLTE_CHEST_MAX_SMOOTH_FIL_LEN);
  }
}

void srslte_chest_dl_nbiot_set_noise_alg(srslte_chest_dl_nbiot_t* q, srslte_chest_dl_noise_alg_t noise_estimation_alg)
{
  q->noise_alg = noise_estimation_alg;
}

void srslte_chest_dl_nbiot_set_smooth_filter3_coeff(srslte_chest_dl_nbiot_t* q, float w)
{
  q->smooth_filter_len = 3;
  q->smooth_filter[0]  = w;
  q->smooth_filter[2]  = w;
  q->smooth_filter[1]  = 1 - 2 * w;
}

static void average_pilots(srslte_chest_dl_nbiot_t* q, cf_t* input, cf_t* output, uint32_t port_id)
{
  uint32_t nsymbols = srslte_refsignal_cs_nof_symbols(NULL, NULL, port_id);
  uint32_t nref     = 2;

  // Average in the frequency domain
  for (int l = 0; l < nsymbols; l++) {
    srslte_conv_same_cf(&input[l * nref], q->smooth_filter, &output[l * nref], nref, q->smooth_filter_len);
  }
}

float srslte_chest_nbiot_dl_rssi(srslte_chest_dl_nbiot_t* q, cf_t* input, uint32_t port_id)
{
  float    rssi     = 0;
  uint32_t nsymbols = srslte_refsignal_cs_nof_symbols(NULL, NULL, port_id);
  for (uint32_t l = 0; l < nsymbols; l++) {
    cf_t* tmp = &input[srslte_refsignal_nrs_nsymbol(l) * SRSLTE_NRE];
    rssi += srslte_vec_dot_prod_conj_ccc(tmp, tmp, SRSLTE_NRE);
  }
  return rssi / nsymbols;
}

int srslte_chest_dl_nbiot_estimate_port(srslte_chest_dl_nbiot_t* q,
                                        cf_t*                    input,
                                        cf_t*                    ce,
                                        uint32_t                 sf_idx,
                                        uint32_t                 port_id)
{
  DEBUG("x.%d: Estimating DL channel on port %d.\n", sf_idx, port_id);
  int nref = srslte_refsignal_nbiot_cs_nof_re(&q->cell, port_id);

  // Get references from the input signal
  srslte_refsignal_nrs_get_sf(q->cell, port_id, input, q->pilot_recv_signal);

  // Use the known NRS signal to compute Least-squares estimates
  srslte_vec_prod_conj_ccc(q->pilot_recv_signal, q->nrs_signal.pilots[port_id][sf_idx], q->pilot_estimates, nref);

  if (SRSLTE_VERBOSE_ISDEBUG()) {
    printf("Rx pilots:\n");
    srslte_vec_fprint_c(stdout, q->pilot_recv_signal, nref);
    printf("Tx pilots on port=%d for sf_idx=%d:\n", port_id, sf_idx);
    srslte_vec_fprint_c(stdout, q->nrs_signal.pilots[port_id][sf_idx], nref);
    printf("Estimates:\n");
    srslte_vec_fprint_c(stdout, q->pilot_estimates, nref);
    printf("Average estimates:\n");
    srslte_vec_fprint_c(stdout, q->pilot_estimates_average, nref);
  }

  if (ce != NULL) {
    // Smooth estimates (if applicable) and interpolate
    if (q->smooth_filter_len == 0 || (q->smooth_filter_len == 3 && q->smooth_filter[0] == 0)) {
      interpolate_pilots(q, q->pilot_estimates, ce, port_id);
    } else {
      average_pilots(q, q->pilot_estimates, q->pilot_estimates_average, port_id);
      interpolate_pilots(q, q->pilot_estimates_average, ce, port_id);
    }

    // Estimate noise power
    if (q->noise_alg == SRSLTE_NOISE_ALG_REFS && q->smooth_filter_len > 0) {
      q->noise_estimate[port_id] = estimate_noise_pilots(q, port_id);
    }
  }

  if (SRSLTE_VERBOSE_ISDEBUG()) {
    printf("Updated Average estimates:\n");
    srslte_vec_fprint_c(stdout, q->pilot_estimates_average, nref);
  }

  // Compute RSRP for the channel estimates in this port
  q->rsrp[port_id] = srslte_vec_avg_power_cf(q->pilot_recv_signal, nref);
  if (port_id == 0) {
    // compute rssi only for port 0
    q->rssi[port_id] = srslte_chest_nbiot_dl_rssi(q, input, port_id);
  }

  return 0;
}

int srslte_chest_dl_nbiot_estimate(srslte_chest_dl_nbiot_t* q, cf_t* input, cf_t** ce, uint32_t sf_idx)
{
  for (uint32_t port_id = 0; port_id < q->cell.nof_ports; port_id++) {
    srslte_chest_dl_nbiot_estimate_port(q, input, ce[port_id], sf_idx, port_id);
  }
  return SRSLTE_SUCCESS;
}

float srslte_chest_dl_nbiot_get_noise_estimate(srslte_chest_dl_nbiot_t* q)
{
  return srslte_vec_acc_ff(q->noise_estimate, q->cell.nof_ports) / q->cell.nof_ports;
}

float srslte_chest_dl_nbiot_get_snr(srslte_chest_dl_nbiot_t* q)
{
  return srslte_chest_dl_nbiot_get_rsrp(q) / srslte_chest_dl_nbiot_get_noise_estimate(q);
}

float srslte_chest_dl_nbiot_get_rssi(srslte_chest_dl_nbiot_t* q)
{
  return 4 * q->rssi[0] / SRSLTE_NRE;
}

/* q->rssi[0] is the average power in all RE in all symbol containing references for port 0.
 * q->rssi[0]/q->cell.nof_prb is the average power per PRB
 * q->rsrp[0] is the average power of RE containing references only (for port 0).
 */
float srslte_chest_dl_nbiot_get_rsrq(srslte_chest_dl_nbiot_t* q)
{
  return q->rsrp[0] / q->rssi[0];
}

float srslte_chest_dl_nbiot_get_rsrp(srslte_chest_dl_nbiot_t* q)
{
  // return sum of power received from all tx ports
  return srslte_vec_acc_ff(q->rsrp, q->cell.nof_ports);
}