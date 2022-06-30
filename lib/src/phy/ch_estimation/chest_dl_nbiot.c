/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srsran/config.h"
#include "srsran/phy/utils/debug.h"

#include "srsran/phy/ch_estimation/chest_dl_nbiot.h"
#include "srsran/phy/utils/convolution.h"
#include "srsran/phy/utils/vector.h"

int srsran_chest_dl_nbiot_init(srsran_chest_dl_nbiot_t* q, uint32_t max_prb)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    bzero(q, sizeof(srsran_chest_dl_nbiot_t));

    ret = srsran_refsignal_dl_nbiot_init(&q->nrs_signal);
    if (ret != SRSRAN_SUCCESS) {
      fprintf(stderr, "Error initializing CSR signal (%d)\n", ret);
      goto clean_exit;
    }

    q->tmp_noise = srsran_vec_cf_malloc(SRSRAN_REFSIGNAL_MAX_NUM_SF(max_prb));
    if (!q->tmp_noise) {
      perror("malloc");
      goto clean_exit;
    }
    q->pilot_estimates = srsran_vec_cf_malloc(SRSRAN_REFSIGNAL_MAX_NUM_SF(max_prb) + SRSRAN_CHEST_MAX_SMOOTH_FIL_LEN);
    if (!q->pilot_estimates) {
      perror("malloc");
      goto clean_exit;
    }
    q->pilot_estimates_average =
        srsran_vec_cf_malloc(SRSRAN_REFSIGNAL_MAX_NUM_SF(max_prb) + SRSRAN_CHEST_MAX_SMOOTH_FIL_LEN);
    if (!q->pilot_estimates_average) {
      perror("malloc");
      goto clean_exit;
    }
    for (int i = 0; i < SRSRAN_REFSIGNAL_MAX_NUM_SF(max_prb); i++) {
      q->pilot_estimates_average[i] = 1;
    }

    q->pilot_recv_signal = srsran_vec_cf_malloc(SRSRAN_REFSIGNAL_MAX_NUM_SF(max_prb));
    if (!q->pilot_recv_signal) {
      perror("malloc");
      goto clean_exit;
    }

    if (srsran_interp_linear_vector_init(&q->srsran_interp_linvec, SRSRAN_NRE * max_prb)) {
      fprintf(stderr, "Error initializing vector interpolator\n");
      goto clean_exit;
    }

    if (srsran_interp_linear_init(&q->srsran_interp_lin, 2 * max_prb, SRSRAN_NRE / 2)) {
      fprintf(stderr, "Error initializing interpolator\n");
      goto clean_exit;
    }

    q->noise_alg = SRSRAN_NOISE_ALG_REFS;
    srsran_chest_dl_nbiot_set_smooth_filter3_coeff(q, 0.1);

    ret = SRSRAN_SUCCESS;
  }

clean_exit:
  if (ret != SRSRAN_SUCCESS  && q != NULL) {
    srsran_chest_dl_nbiot_free(q);
  }
  return ret;
}

void srsran_chest_dl_nbiot_free(srsran_chest_dl_nbiot_t* q)
{
  srsran_refsignal_dl_nbiot_free(&q->nrs_signal);

  if (q->tmp_noise) {
    free(q->tmp_noise);
  }
  srsran_interp_linear_vector_free(&q->srsran_interp_linvec);
  srsran_interp_linear_free(&q->srsran_interp_lin);

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

int srsran_chest_dl_nbiot_set_cell(srsran_chest_dl_nbiot_t* q, srsran_nbiot_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  // set ports configuration
  if (cell.nof_ports == 0) {
    cell.nof_ports = SRSRAN_NBIOT_MAX_PORTS;
  }

  if (q != NULL && srsran_nbiot_cell_isvalid(&cell)) {
    if (q->cell.n_id_ncell != cell.n_id_ncell || q->cell.base.nof_prb == 0) {
      q->cell = cell;

      ret = srsran_refsignal_dl_nbiot_set_cell(&q->nrs_signal, cell);
      if (ret != SRSRAN_SUCCESS) {
        fprintf(stderr, "Error initializing NRS signal (%d)\n", ret);
        return SRSRAN_ERROR;
      }

      if (srsran_interp_linear_vector_resize(&q->srsran_interp_linvec, SRSRAN_NRE * cell.base.nof_prb)) {
        fprintf(stderr, "Error initializing vector interpolator\n");
        return SRSRAN_ERROR;
      }

      if (srsran_interp_linear_resize(&q->srsran_interp_lin, 2 * cell.base.nof_prb, SRSRAN_NRE / 2)) {
        fprintf(stderr, "Error initializing interpolator\n");
        return SRSRAN_ERROR;
      }
    }
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

/* Uses the difference between the averaged and non-averaged pilot estimates */
static float estimate_noise_pilots(srsran_chest_dl_nbiot_t* q, uint32_t port_id)
{
  int nref = srsran_refsignal_cs_nof_re(NULL, NULL, port_id);

  // Substract noisy pilot estimates
  srsran_vec_sub_ccc(q->pilot_estimates_average, q->pilot_estimates, q->tmp_noise, nref);

  // Compute average power. Normalized for filter len 3 using ML
  float norm = 1;
  if (q->smooth_filter_len == 3) {
    float a     = q->smooth_filter[0];
    float norm3 = 6.143 * a * a + 0.04859 * a - 0.002774;
    norm /= norm3;
  }
  float power = norm * q->cell.nof_ports * srsran_vec_avg_power_cf(q->tmp_noise, nref);
  return power;
}

#define cesymb(i) ce[SRSRAN_RE_IDX(q->cell.base.nof_prb, (i), 0)]

static void interpolate_pilots(srsran_chest_dl_nbiot_t* q, cf_t* pilot_estimates, cf_t* ce, uint32_t port_id)
{
  uint32_t nsymbols = srsran_refsignal_cs_nof_symbols(NULL, NULL, port_id);
  int      num_ces  = q->cell.base.nof_prb * SRSRAN_NRE;
  cf_t     ce_avg[2][num_ces];

  // interpolate the symbols with references in the freq domain
  DEBUG("Interpolating %d pilots in %d symbols at port %d.", nsymbols * 2, nsymbols, port_id);
  for (int l = 0; l < nsymbols; l++) {
    uint32_t fidx_offset = srsran_refsignal_dl_nbiot_fidx(q->cell, l, port_id, 0);
    // points to the RE of the beginning of the l'th symbol containing a ref
    uint32_t ce_idx = srsran_refsignal_nrs_nsymbol(l) * q->cell.base.nof_prb * SRSRAN_NRE;
    ce_idx += q->cell.nbiot_prb * SRSRAN_NRE;
    DEBUG("  - freq.-dmn interp. in sym %d at RE %d", srsran_refsignal_nrs_nsymbol(l), ce_idx + fidx_offset);
    srsran_interp_linear_offset(
        &q->srsran_interp_lin, &pilot_estimates[2 * l], &ce[ce_idx], fidx_offset, SRSRAN_NRE / 2 - fidx_offset);
  }

  // average the two neighbor subcarriers containing pilots
  for (int l = 0; l < nsymbols / 2; l++) {
    int sym_idx = srsran_refsignal_nrs_nsymbol(l * 2);
    srsran_vec_sum_ccc(&cesymb(sym_idx), &cesymb(sym_idx + 1), ce_avg[l], num_ces);
    // TODO: use vector operation for this
    for (int k = 0; k < num_ces; k++) {
      ce_avg[l][k] /= 2;
    }
    // use avarage for both symbols
    memcpy(&cesymb(sym_idx), ce_avg[l], num_ces * sizeof(cf_t));
    memcpy(&cesymb(sym_idx + 1), ce_avg[l], num_ces * sizeof(cf_t));
  }

  // now interpolate in the time domain between symbols
  srsran_interp_linear_vector(&q->srsran_interp_linvec, ce_avg[0], ce_avg[1], &cesymb(0), 5, 5);
  srsran_interp_linear_vector(&q->srsran_interp_linvec, ce_avg[0], ce_avg[1], &cesymb(7), 5, 5);
}

void srsran_chest_dl_nbiot_set_smooth_filter(srsran_chest_dl_nbiot_t* q, float* filter, uint32_t filter_len)
{
  if (filter_len < SRSRAN_CHEST_MAX_SMOOTH_FIL_LEN) {
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
            SRSRAN_CHEST_MAX_SMOOTH_FIL_LEN);
  }
}

void srsran_chest_dl_nbiot_set_noise_alg(srsran_chest_dl_nbiot_t* q, srsran_chest_dl_noise_alg_t noise_estimation_alg)
{
  q->noise_alg = noise_estimation_alg;
}

void srsran_chest_dl_nbiot_set_smooth_filter3_coeff(srsran_chest_dl_nbiot_t* q, float w)
{
  q->smooth_filter_len = 3;
  q->smooth_filter[0]  = w;
  q->smooth_filter[2]  = w;
  q->smooth_filter[1]  = 1 - 2 * w;
}

static void average_pilots(srsran_chest_dl_nbiot_t* q, cf_t* input, cf_t* output, uint32_t port_id)
{
  uint32_t nsymbols = srsran_refsignal_cs_nof_symbols(NULL, NULL, port_id);
  uint32_t nref     = 2;

  // Average in the frequency domain
  for (int l = 0; l < nsymbols; l++) {
    srsran_conv_same_cf(&input[l * nref], q->smooth_filter, &output[l * nref], nref, q->smooth_filter_len);
  }
}

float srsran_chest_nbiot_dl_rssi(srsran_chest_dl_nbiot_t* q, cf_t* input, uint32_t port_id)
{
  float    rssi     = 0;
  uint32_t nsymbols = srsran_refsignal_cs_nof_symbols(NULL, NULL, port_id);
  for (uint32_t l = 0; l < nsymbols; l++) {
    cf_t* tmp = &input[srsran_refsignal_nrs_nsymbol(l) * SRSRAN_NRE];
    rssi += srsran_vec_dot_prod_conj_ccc(tmp, tmp, SRSRAN_NRE);
  }
  return rssi / nsymbols;
}

int srsran_chest_dl_nbiot_estimate_port(srsran_chest_dl_nbiot_t* q,
                                        cf_t*                    input,
                                        cf_t*                    ce,
                                        uint32_t                 sf_idx,
                                        uint32_t                 port_id)
{
  DEBUG("x.%d: Estimating DL channel on port %d.", sf_idx, port_id);
  int nref = srsran_refsignal_nbiot_cs_nof_re(&q->cell, port_id);

  // Get references from the input signal
  srsran_refsignal_nrs_get_sf(q->cell, port_id, input, q->pilot_recv_signal);

  // Use the known NRS signal to compute Least-squares estimates
  srsran_vec_prod_conj_ccc(q->pilot_recv_signal, q->nrs_signal.pilots[port_id][sf_idx], q->pilot_estimates, nref);

  if (SRSRAN_VERBOSE_ISDEBUG()) {
    printf("Rx pilots:\n");
    srsran_vec_fprint_c(stdout, q->pilot_recv_signal, nref);
    printf("Tx pilots on port=%d for sf_idx=%d:\n", port_id, sf_idx);
    srsran_vec_fprint_c(stdout, q->nrs_signal.pilots[port_id][sf_idx], nref);
    printf("Estimates:\n");
    srsran_vec_fprint_c(stdout, q->pilot_estimates, nref);
    printf("Average estimates:\n");
    srsran_vec_fprint_c(stdout, q->pilot_estimates_average, nref);
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
    if (q->noise_alg == SRSRAN_NOISE_ALG_REFS && q->smooth_filter_len > 0) {
      q->noise_estimate[port_id] = estimate_noise_pilots(q, port_id);
    }
  }

  if (SRSRAN_VERBOSE_ISDEBUG()) {
    printf("Updated Average estimates:\n");
    srsran_vec_fprint_c(stdout, q->pilot_estimates_average, nref);
  }

  // Compute RSRP for the channel estimates in this port
  q->rsrp[port_id] = srsran_vec_avg_power_cf(q->pilot_recv_signal, nref);
  if (port_id == 0) {
    // compute rssi only for port 0
    q->rssi[port_id] = srsran_chest_nbiot_dl_rssi(q, input, port_id);
  }

  return 0;
}

int srsran_chest_dl_nbiot_estimate(srsran_chest_dl_nbiot_t* q, cf_t* input, cf_t** ce, uint32_t sf_idx)
{
  for (uint32_t port_id = 0; port_id < q->cell.nof_ports; port_id++) {
    srsran_chest_dl_nbiot_estimate_port(q, input, ce[port_id], sf_idx, port_id);
  }
  return SRSRAN_SUCCESS;
}

float srsran_chest_dl_nbiot_get_noise_estimate(srsran_chest_dl_nbiot_t* q)
{
  return srsran_vec_acc_ff(q->noise_estimate, q->cell.nof_ports) / q->cell.nof_ports;
}

float srsran_chest_dl_nbiot_get_snr(srsran_chest_dl_nbiot_t* q)
{
  return srsran_chest_dl_nbiot_get_rsrp(q) / srsran_chest_dl_nbiot_get_noise_estimate(q);
}

float srsran_chest_dl_nbiot_get_rssi(srsran_chest_dl_nbiot_t* q)
{
  return 4 * q->rssi[0] / SRSRAN_NRE;
}

/* q->rssi[0] is the average power in all RE in all symbol containing references for port 0.
 * q->rssi[0]/q->cell.nof_prb is the average power per PRB
 * q->rsrp[0] is the average power of RE containing references only (for port 0).
 */
float srsran_chest_dl_nbiot_get_rsrq(srsran_chest_dl_nbiot_t* q)
{
  return q->rsrp[0] / q->rssi[0];
}

float srsran_chest_dl_nbiot_get_rsrp(srsran_chest_dl_nbiot_t* q)
{
  // return sum of power received from all tx ports
  return srsran_vec_acc_ff(q->rsrp, q->cell.nof_ports);
}