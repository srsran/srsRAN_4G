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

#include "srsran/srsran.h"
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srsran/config.h"

#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/utils/convolution.h"
#include "srsran/phy/utils/vector.h"

//#define DEFAULT_FILTER_LEN 3

#ifdef DEFAULT_FILTER_LEN
static void set_default_filter(srsran_chest_dl_t* q, int filter_len)
{
  float fil[SRSRAN_CHEST_DL_MAX_SMOOTH_FIL_LEN];

  for (int i = 0; i < filter_len / 2; i++) {
    fil[i]                      = i + 1;
    fil[i + filter_len / 2 + 1] = filter_len / 2 - i;
  }
  fil[filter_len / 2] = filter_len / 2 + 1;

  float s = 0;
  for (int i = 0; i < filter_len; i++) {
    s += fil[i];
  }
  for (int i = 0; i < filter_len; i++) {
    fil[i] /= s;
  }

  srsran_chest_dl_set_smooth_filter(q, fil, filter_len);
}
#endif

/** 3GPP LTE Downlink channel estimator and equalizer.
 * Estimates the channel in the resource elements transmitting references and interpolates for the rest
 * of the resource grid.
 *
 * The equalizer uses the channel estimates to produce an estimation of the transmitted symbol.
 *
 * This object depends on the srsran_refsignal_t object for creating the LTE CSR signal.
 */
int srsran_chest_dl_init(srsran_chest_dl_t* q, uint32_t max_prb, uint32_t nof_rx_antennas)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    bzero(q, sizeof(srsran_chest_dl_t));

    ret = srsran_refsignal_cs_init(&q->csr_refs, max_prb);
    if (ret != SRSRAN_SUCCESS) {
      ERROR("Error initializing CSR signal (%d)", ret);
      goto clean_exit;
    }

    q->mbsfn_refs = calloc(SRSRAN_MAX_MBSFN_AREA_IDS, sizeof(srsran_refsignal_t*));
    if (!q->mbsfn_refs) {
      ERROR("Calloc error initializing mbsfn_refs (%d)", ret);
      goto clean_exit;
    }

    int pilot_vec_size;
    if (SRSRAN_REFSIGNAL_MAX_NUM_SF_MBSFN(max_prb) > SRSRAN_REFSIGNAL_MAX_NUM_SF(max_prb)) {
      pilot_vec_size = SRSRAN_REFSIGNAL_MAX_NUM_SF_MBSFN(max_prb);
    } else {
      pilot_vec_size = SRSRAN_REFSIGNAL_MAX_NUM_SF(max_prb);
    }

    q->tmp_noise = srsran_vec_cf_malloc(pilot_vec_size);
    if (!q->tmp_noise) {
      perror("malloc");
      goto clean_exit;
    }

    q->tmp_cfo_estimate = srsran_vec_cf_malloc(pilot_vec_size);
    if (!q->tmp_cfo_estimate) {
      perror("malloc");
      goto clean_exit;
    }

    q->pilot_estimates = srsran_vec_cf_malloc(pilot_vec_size);
    if (!q->pilot_estimates) {
      perror("malloc");
      goto clean_exit;
    }

    q->pilot_estimates_average = srsran_vec_cf_malloc(pilot_vec_size);
    if (!q->pilot_estimates_average) {
      perror("malloc");
      goto clean_exit;
    }

    q->pilot_recv_signal = srsran_vec_cf_malloc(pilot_vec_size);
    if (!q->pilot_recv_signal) {
      perror("malloc");
      goto clean_exit;
    }

    if (srsran_interp_linear_vector_init(&q->srsran_interp_linvec, SRSRAN_NRE * max_prb)) {
      ERROR("Error initializing vector interpolator");
      goto clean_exit;
    }

    if (srsran_interp_linear_init(&q->srsran_interp_lin, 2 * max_prb, SRSRAN_NRE / 2)) {
      ERROR("Error initializing interpolator");
      goto clean_exit;
    }

    if (srsran_interp_linear_init(&q->srsran_interp_lin_3, 4 * max_prb, SRSRAN_NRE / 4)) {
      ERROR("Error initializing interpolator");
      goto clean_exit;
    }

    if (srsran_interp_linear_init(&q->srsran_interp_lin_mbsfn, 6 * max_prb, SRSRAN_NRE / 6)) {
      ERROR("Error initializing interpolator");
      goto clean_exit;
    }

    q->wiener_dl = calloc(sizeof(srsran_wiener_dl_t), 1);
    if (q->wiener_dl) {
      srsran_wiener_dl_init(q->wiener_dl, max_prb, 2, nof_rx_antennas);
    } else {
      ERROR("Error allocating wiener filter");
      goto clean_exit;
    }

    q->nof_rx_antennas = nof_rx_antennas;
  }

  ret = SRSRAN_SUCCESS;

clean_exit:
  if (ret != SRSRAN_SUCCESS) {
    srsran_chest_dl_free(q);
  }
  return ret;
}

void srsran_chest_dl_free(srsran_chest_dl_t* q)
{
  if (!q) {
    return;
  }

  srsran_refsignal_free(&q->csr_refs);

  if (q->mbsfn_refs) {
    for (int i = 0; i < SRSRAN_MAX_MBSFN_AREA_IDS; i++) {
      if (q->mbsfn_refs[i]) {
        srsran_refsignal_free(q->mbsfn_refs[i]);
        free(q->mbsfn_refs[i]);
      }
    }
    free(q->mbsfn_refs);
  }

  if (q->tmp_noise) {
    free(q->tmp_noise);
  }
  if (q->tmp_cfo_estimate) {
    free(q->tmp_cfo_estimate);
  }
  srsran_interp_linear_vector_free(&q->srsran_interp_linvec);
  srsran_interp_linear_free(&q->srsran_interp_lin);
  srsran_interp_linear_free(&q->srsran_interp_lin_3);
  srsran_interp_linear_free(&q->srsran_interp_lin_mbsfn);
  if (q->pilot_estimates) {
    free(q->pilot_estimates);
  }
  if (q->pilot_estimates_average) {
    free(q->pilot_estimates_average);
  }
  if (q->pilot_recv_signal) {
    free(q->pilot_recv_signal);
  }
  if (q->wiener_dl) {
    srsran_wiener_dl_free(q->wiener_dl);
    free(q->wiener_dl);
  }
  bzero(q, sizeof(srsran_chest_dl_t));
}

int srsran_chest_dl_res_init(srsran_chest_dl_res_t* q, uint32_t max_prb)
{
  return srsran_chest_dl_res_init_re(q, SRSRAN_SF_LEN_RE(max_prb, SRSRAN_CP_NORM));
}

int srsran_chest_dl_res_init_re(srsran_chest_dl_res_t* q, uint32_t nof_re)
{
  bzero(q, sizeof(srsran_chest_dl_res_t));
  q->nof_re = nof_re;
  for (uint32_t i = 0; i < SRSRAN_MAX_PORTS; i++) {
    for (uint32_t j = 0; j < SRSRAN_MAX_PORTS; j++) {
      q->ce[i][j] = srsran_vec_cf_malloc(q->nof_re);
      if (!q->ce[i][j]) {
        perror("malloc");
        return -1;
      }
      srsran_vec_cf_zero(q->ce[i][j], nof_re);
    }
  }
  return 0;
}

void srsran_chest_dl_res_set_identity(srsran_chest_dl_res_t* q)
{
  for (uint32_t i = 0; i < SRSRAN_MAX_PORTS; i++) {
    for (uint32_t j = 0; j < SRSRAN_MAX_PORTS; j++) {
      for (uint32_t k = 0; k < q->nof_re; k++) {
        q->ce[i][j][k] = (i == j) ? 1.0f : 0.0f;
      }
    }
  }
}

void srsran_chest_dl_res_set_ones(srsran_chest_dl_res_t* q)
{
  for (uint32_t i = 0; i < SRSRAN_MAX_PORTS; i++) {
    for (uint32_t j = 0; j < SRSRAN_MAX_PORTS; j++) {
      for (uint32_t k = 0; k < q->nof_re; k++) {
        q->ce[i][j][k] = 1.0f;
      }
    }
  }
}

void srsran_chest_dl_res_free(srsran_chest_dl_res_t* q)
{
  for (uint32_t i = 0; i < SRSRAN_MAX_PORTS; i++) {
    for (uint32_t j = 0; j < SRSRAN_MAX_PORTS; j++) {
      if (q->ce[i][j]) {
        free(q->ce[i][j]);
      }
    }
  }
}

int srsran_chest_dl_set_mbsfn_area_id(srsran_chest_dl_t* q, uint16_t mbsfn_area_id)
{
  if (mbsfn_area_id < SRSRAN_MAX_MBSFN_AREA_IDS) {
    if (!q->mbsfn_refs[mbsfn_area_id]) {
      q->mbsfn_refs[mbsfn_area_id] = calloc(1, sizeof(srsran_refsignal_t));
      if (srsran_refsignal_mbsfn_init(q->mbsfn_refs[mbsfn_area_id], q->cell.nof_prb)) {
        return SRSRAN_ERROR;
      }
      if (srsran_refsignal_mbsfn_set_cell(q->mbsfn_refs[mbsfn_area_id], q->cell, mbsfn_area_id)) {
        return SRSRAN_ERROR;
      }
    }
    return SRSRAN_SUCCESS;
  }
  return SRSRAN_ERROR;
}

int srsran_chest_dl_set_cell(srsran_chest_dl_t* q, srsran_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL && srsran_cell_isvalid(&cell)) {
    if (q->cell.id != cell.id || q->cell.nof_prb == 0) {
      q->cell = cell;
      ret     = srsran_refsignal_cs_set_cell(&q->csr_refs, cell);
      if (ret != SRSRAN_SUCCESS) {
        ERROR("Error initializing CSR signal (%d)", ret);
        return SRSRAN_ERROR;
      }
      if (srsran_pss_generate(q->pss_signal, cell.id % 3)) {
        ERROR("Error initializing PSS signal for noise estimation");
        return SRSRAN_ERROR;
      }
      if (srsran_interp_linear_vector_resize(&q->srsran_interp_linvec, SRSRAN_NRE * q->cell.nof_prb)) {
        ERROR("Error initializing vector interpolator");
        return SRSRAN_ERROR;
      }

      if (srsran_interp_linear_resize(&q->srsran_interp_lin, 2 * q->cell.nof_prb, SRSRAN_NRE / 2)) {
        ERROR("Error initializing interpolator");
        return SRSRAN_ERROR;
      }

      if (srsran_interp_linear_resize(&q->srsran_interp_lin_3, 4 * q->cell.nof_prb, SRSRAN_NRE / 4)) {
        ERROR("Error initializing interpolator");
        return SRSRAN_ERROR;
      }
      if (srsran_interp_linear_resize(&q->srsran_interp_lin_mbsfn, 6 * q->cell.nof_prb, SRSRAN_NRE / 6)) {
        fprintf(stderr, "Error initializing interpolator\n");
        return SRSRAN_ERROR;
      }

      if (srsran_wiener_dl_set_cell(q->wiener_dl, cell)) {
        fprintf(stderr, "Error initializing interpolator\n");
        return SRSRAN_ERROR;
      }
    }
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

/* Uses the difference between the averaged and non-averaged pilot estimates */
static float estimate_noise_pilots(srsran_chest_dl_t* q, srsran_dl_sf_cfg_t* sf, uint32_t port_id)
{
  srsran_sf_t ch_mode   = sf->sf_type;
  const float weight    = 1.0f;
  float       sum_power = 0.0f;
  uint32_t    count     = 0;
  uint32_t    npilots   = (ch_mode == SRSRAN_SF_MBSFN) ? SRSRAN_REFSIGNAL_NUM_SF_MBSFN(q->cell.nof_prb, port_id)
                                                       : srsran_refsignal_cs_nof_re(&q->csr_refs, sf, port_id);
  uint32_t    nsymbols  = (ch_mode == SRSRAN_SF_MBSFN) ? srsran_refsignal_mbsfn_nof_symbols()
                                                       : srsran_refsignal_cs_nof_symbols(&q->csr_refs, sf, port_id);
  if (nsymbols == 0) {
    ERROR("Invalid number of CRS symbols\n");
    return SRSRAN_ERROR;
  }

  uint32_t nref = npilots / nsymbols;
  uint32_t fidx =
      (ch_mode == SRSRAN_SF_MBSFN) ? srsran_refsignal_mbsfn_fidx(1) : srsran_refsignal_cs_fidx(q->cell, 0, port_id, 0);
  cf_t* tmp_noise = q->tmp_noise;

  // Special case for 1 or 2 symbol
  if (nsymbols < 3) {
    srsran_vec_sc_prod_cfc(q->pilot_estimates + 1, weight, tmp_noise, nref - 2);
    srsran_vec_sum_ccc(q->pilot_estimates + 0, tmp_noise, tmp_noise, nref - 2);
    srsran_vec_sum_ccc(q->pilot_estimates + 2, tmp_noise, tmp_noise, nref - 2);
    srsran_vec_sc_prod_cfc(tmp_noise, 1.0f / (weight + 2.0f), tmp_noise, nref - 2);
    srsran_vec_sub_ccc(q->pilot_estimates + 1, tmp_noise, tmp_noise, nref - 2);
    sum_power = srsran_vec_avg_power_cf(tmp_noise, nref - 2);
    return sum_power;
  }

  // Convert pilots to 2D to ease access
  cf_t* input2d[4]; // The maximum number of symbols is 4
  for (int i = 0; i < nsymbols; i++) {
    input2d[i] = &q->pilot_estimates[i * nref];
  }

  // Compares surrounding pilots in time/frequency. It requires at least 3 symbols with pilots.
  for (int i = 1; i < nsymbols - 1; i++) {
    // Calculate previous and next symbol indexes offset
    uint32_t offset = ((fidx < 3) ^ (i & 1)) ? 0 : 1;

    // Write estimates from this symbols
    srsran_vec_sc_prod_cfc(input2d[i], weight, tmp_noise, nref);

    // Add the previous symbol estimates and extrapolate first/last element
    srsran_vec_sum_ccc(&input2d[i - 1][0], &tmp_noise[offset], &tmp_noise[offset], nref - offset);
    srsran_vec_sum_ccc(&input2d[i - 1][1 - offset], &tmp_noise[0], &tmp_noise[0], nref + offset - 1);
    if (offset) {
      tmp_noise[0] += 2.0f * input2d[i - 1][0] - input2d[i - 1][1];
    } else {
      tmp_noise[nref - 1] += 2.0f * input2d[i - 1][nref - 2] - input2d[i - 1][nref - 1];
    }

    // Add the next symbol estimates and extrapolate first/last element
    srsran_vec_sum_ccc(&input2d[i + 1][0], &tmp_noise[offset], &tmp_noise[offset], nref - offset);
    srsran_vec_sum_ccc(&input2d[i + 1][1 - offset], &tmp_noise[0], &tmp_noise[0], nref + offset - 1);
    if (offset) {
      tmp_noise[0] += 2.0f * input2d[i + 1][0] - input2d[i + 1][1];
    } else {
      tmp_noise[nref - 1] += 2.0f * input2d[i + 1][nref - 2] - input2d[i + 1][nref - 1];
    }

    // Scale to normalise to this symbol
    srsran_vec_sc_prod_cfc(tmp_noise, 1.0f / (weight + 4.0f), tmp_noise, nref);

    // Subtract this symbol
    srsran_vec_sub_ccc(input2d[i], tmp_noise, tmp_noise, nref);

    // The left signal after the subtraction can be considered noise
    sum_power += srsran_vec_avg_power_cf(tmp_noise, nref);
    count++;
  }

  return sum_power / (float)count;
}

static float estimate_noise_pss(srsran_chest_dl_t* q, cf_t* input, cf_t* ce)
{
  /* Get PSS from received signal */
  srsran_pss_get_slot(input, q->tmp_pss, q->cell.nof_prb, q->cell.cp);

  /* Get channel estimates for PSS position */
  srsran_pss_get_slot(ce, q->tmp_pss_noisy, q->cell.nof_prb, q->cell.cp);

  /* Multiply known PSS by channel estimates */
  srsran_vec_prod_ccc(q->tmp_pss_noisy, q->pss_signal, q->tmp_pss_noisy, SRSRAN_PSS_LEN);

  /* Substract received signal */
  srsran_vec_sub_ccc(q->tmp_pss_noisy, q->tmp_pss, q->tmp_pss_noisy, SRSRAN_PSS_LEN);

  /* Compute average power */
  float power = q->cell.nof_ports * srsran_vec_avg_power_cf(q->tmp_pss_noisy, SRSRAN_PSS_LEN) * M_SQRT1_2;
  return power;
}

/* Uses the 5 empty transmitted SC before and after the SSS and PSS sequences for noise estimation */
static float estimate_noise_empty_sc(srsran_chest_dl_t* q, cf_t* input)
{
  int k_sss = (SRSRAN_CP_NSYMB(q->cell.cp) - 2) * q->cell.nof_prb * SRSRAN_NRE + q->cell.nof_prb * SRSRAN_NRE / 2 - 31;
  float noise_power = 0;
  noise_power += srsran_vec_avg_power_cf(&input[k_sss - 5], 5);  // 5 empty SC before SSS
  noise_power += srsran_vec_avg_power_cf(&input[k_sss + 62], 5); // 5 empty SC after SSS
  int k_pss = (SRSRAN_CP_NSYMB(q->cell.cp) - 1) * q->cell.nof_prb * SRSRAN_NRE + q->cell.nof_prb * SRSRAN_NRE / 2 - 31;
  noise_power += srsran_vec_avg_power_cf(&input[k_pss - 5], 5);  // 5 empty SC before PSS
  noise_power += srsran_vec_avg_power_cf(&input[k_pss + 62], 5); // 5 empty SC after PSS

  return noise_power;
}

#define cesymb(i) ce[SRSRAN_RE_IDX(q->cell.nof_prb, i, 0)]

static void interpolate_pilots(srsran_chest_dl_t*     q,
                               srsran_dl_sf_cfg_t*    sf,
                               srsran_chest_dl_cfg_t* cfg,
                               cf_t*                  pilot_estimates,
                               cf_t*                  ce,
                               uint32_t               port_id)
{
  /* interpolate the symbols with references in the freq domain */
  uint32_t nsymbols    = (sf->sf_type == SRSRAN_SF_MBSFN) ? srsran_refsignal_mbsfn_nof_symbols() + 1
                                                          : srsran_refsignal_cs_nof_symbols(&q->csr_refs, sf, port_id);
  uint32_t fidx_offset = 0;

  /* Interpolate in the frequency domain */

  uint32_t freq_nsymbols = nsymbols;
  if (cfg->estimator_alg == SRSRAN_ESTIMATOR_ALG_AVERAGE) {
    freq_nsymbols = 1;
  }

  // we add one to nsymbols to allow for inclusion of the non-mbms references in the channel estimation
  for (uint32_t l = 0; l < freq_nsymbols; l++) {
    if (sf->sf_type == SRSRAN_SF_MBSFN) {
      if (l == 0) {
        fidx_offset = srsran_refsignal_cs_fidx(q->cell, l, port_id, 0);
        srsran_interp_linear_offset(
            &q->srsran_interp_lin,
            &pilot_estimates[2 * q->cell.nof_prb * l],
            &ce[srsran_refsignal_cs_nsymbol(l, q->cell.cp, port_id) * q->cell.nof_prb * SRSRAN_NRE],
            fidx_offset,
            SRSRAN_NRE / 2 - fidx_offset);
      } else {
        fidx_offset = srsran_refsignal_mbsfn_fidx(l - 1);
        srsran_interp_linear_offset(&q->srsran_interp_lin_mbsfn,
                                    &pilot_estimates[(2 * q->cell.nof_prb) + 6 * q->cell.nof_prb * (l - 1)],
                                    &ce[srsran_refsignal_mbsfn_nsymbol(l - 1) * q->cell.nof_prb * SRSRAN_NRE],
                                    fidx_offset,
                                    (fidx_offset) ? 1 : 2);
      }
    } else {
      if (cfg->estimator_alg == SRSRAN_ESTIMATOR_ALG_AVERAGE) {
        if (nsymbols > 1) {
          fidx_offset = q->cell.id % 3;
          srsran_interp_linear_offset(
              &q->srsran_interp_lin_3, pilot_estimates, ce, fidx_offset, SRSRAN_NRE / 4 - fidx_offset);
        } else {
          fidx_offset = srsran_refsignal_cs_fidx(q->cell, l, port_id, 0);
          srsran_interp_linear_offset(&q->srsran_interp_lin,
                                      &pilot_estimates[2 * q->cell.nof_prb * l],
                                      ce,
                                      fidx_offset,
                                      SRSRAN_NRE / 2 - fidx_offset);
        }
      } else {
        if (nsymbols < 2) {
          fidx_offset = srsran_refsignal_cs_fidx(q->cell, l, port_id, 0);
          srsran_interp_linear_offset(&q->srsran_interp_lin,
                                      &pilot_estimates[2 * q->cell.nof_prb * l],
                                      ce,
                                      fidx_offset,
                                      SRSRAN_NRE / 2 - fidx_offset);
        } else {
          fidx_offset = srsran_refsignal_cs_fidx(q->cell, l, port_id, 0);
          srsran_interp_linear_offset(
              &q->srsran_interp_lin,
              &pilot_estimates[2 * q->cell.nof_prb * l],
              &ce[srsran_refsignal_cs_nsymbol(l, q->cell.cp, port_id) * q->cell.nof_prb * SRSRAN_NRE],
              fidx_offset,
              SRSRAN_NRE / 2 - fidx_offset);
        }
      }
    }
  }

  /* Now interpolate in the time domain between symbols */
  if (sf->sf_type == SRSRAN_SF_NORM && (cfg->estimator_alg == SRSRAN_ESTIMATOR_ALG_AVERAGE || nsymbols < 2)) {
    // If we average per subframe, just copy the estimates in the time domain
    for (uint32_t l = 1; l < 2 * SRSRAN_CP_NSYMB(q->cell.cp); l++) {
      memcpy(&ce[l * SRSRAN_NRE * q->cell.nof_prb], ce, sizeof(cf_t) * SRSRAN_NRE * q->cell.nof_prb);
    }
  } else {
    if (sf->sf_type == SRSRAN_SF_MBSFN) {
      srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(0), &cesymb(2), &cesymb(1), 2, 1);
      srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(2), &cesymb(6), &cesymb(3), 4, 3);
      srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(6), &cesymb(10), &cesymb(7), 4, 3);
      srsran_interp_linear_vector2(&q->srsran_interp_linvec, &cesymb(6), &cesymb(10), &cesymb(10), &cesymb(11), 4, 1);
    } else {
      if (SRSRAN_CP_ISNORM(q->cell.cp)) {
        if (port_id < 2) {
          srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(0), &cesymb(4), &cesymb(1), 4, 3);
          srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(4), &cesymb(7), &cesymb(5), 3, 2);
          if (nsymbols == 4) {
            srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(7), &cesymb(11), &cesymb(8), 4, 3);
            srsran_interp_linear_vector2(
                &q->srsran_interp_linvec, &cesymb(7), &cesymb(11), &cesymb(11), &cesymb(12), 4, 2);
          } else {
            srsran_interp_linear_vector2(
                &q->srsran_interp_linvec, &cesymb(4), &cesymb(7), &cesymb(7), &cesymb(8), 3, 6);
          }
        } else {
          srsran_interp_linear_vector2(&q->srsran_interp_linvec, &cesymb(8), &cesymb(1), &cesymb(1), &cesymb(0), 7, 1);
          srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(1), &cesymb(8), &cesymb(2), 7, 6);
          srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(1), &cesymb(8), &cesymb(9), 7, 5);
        }
      } else {
        if (port_id < 2) {
          // TODO: TDD and extended cyclic prefix
          srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(0), &cesymb(3), &cesymb(1), 3, 2);
          srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(3), &cesymb(6), &cesymb(4), 3, 2);
          srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(6), &cesymb(9), &cesymb(7), 3, 2);
          srsran_interp_linear_vector2(&q->srsran_interp_linvec, &cesymb(6), &cesymb(9), &cesymb(9), &cesymb(10), 3, 2);
        } else {
          srsran_interp_linear_vector2(&q->srsran_interp_linvec, &cesymb(7), &cesymb(1), &cesymb(1), &cesymb(0), 6, 1);
          srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(1), &cesymb(7), &cesymb(2), 6, 5);
          srsran_interp_linear_vector(&q->srsran_interp_linvec, &cesymb(1), &cesymb(7), &cesymb(8), 6, 4);
        }
      }
    }
  }
}

static void average_pilots(srsran_chest_dl_t*     q,
                           srsran_dl_sf_cfg_t*    sf,
                           srsran_chest_dl_cfg_t* cfg,
                           cf_t*                  input,
                           cf_t*                  output,
                           uint32_t               port_id,
                           float*                 filter,
                           uint32_t               filter_len)
{
  uint32_t nsymbols = (sf->sf_type == SRSRAN_SF_MBSFN) ? srsran_refsignal_mbsfn_nof_symbols()
                                                       : srsran_refsignal_cs_nof_symbols(&q->csr_refs, sf, port_id);
  uint32_t nref     = (sf->sf_type == SRSRAN_SF_MBSFN) ? 6 * q->cell.nof_prb : 2 * q->cell.nof_prb;

  // Average in the time domain if enabled
  if (cfg->estimator_alg == SRSRAN_ESTIMATOR_ALG_AVERAGE && nsymbols > 1) {
    cf_t* temp = output; // Use output as temporal buffer

    if (srsran_refsignal_cs_fidx(q->cell, 0, port_id, 0) < 3) {
      srsran_vec_interleave(input, &input[nref], temp, nref);
      for (int l = 2; l < nsymbols - 1; l += 2) {
        srsran_vec_interleave_add(&input[l * nref], &input[(l + 1) * nref], temp, nref);
      }
    } else {
      srsran_vec_interleave(&input[nref], input, temp, nref);
      for (int l = 2; l < nsymbols - 1; l += 2) {
        srsran_vec_interleave_add(&input[(l + 1) * nref], &input[l * nref], temp, nref);
      }
    }
    nref *= 2;
    srsran_vec_sc_prod_cfc(temp, 2.0f / (float)nsymbols, input, nref);

    nsymbols = 1;
  }

  uint32_t skip = (sf->sf_type == SRSRAN_SF_MBSFN) ? 2 * q->cell.nof_prb : 0;
  if (sf->sf_type == SRSRAN_SF_MBSFN) {
    memcpy(&output[0], &input[0], skip * sizeof(cf_t));
  }

  // Average in the frequency domain
  for (int l = 0; l < nsymbols; l++) {
    srsran_conv_same_cf(&input[l * nref + skip], filter, &output[l * nref + skip], nref, filter_len);
  }
}

static float chest_dl_rssi(srsran_chest_dl_t* q, srsran_dl_sf_cfg_t* sf, cf_t* input, uint32_t port_id)
{
  uint32_t l;

  float    rssi     = 0;
  uint32_t nsymbols = srsran_refsignal_cs_nof_symbols(&q->csr_refs, sf, port_id);
  for (l = 0; l < nsymbols; l++) {
    cf_t* tmp = &input[srsran_refsignal_cs_nsymbol(l, q->cell.cp, port_id) * q->cell.nof_prb * SRSRAN_NRE];
    rssi += srsran_vec_dot_prod_conj_ccc(tmp, tmp, q->cell.nof_prb * SRSRAN_NRE);
  }
  return rssi / nsymbols;
}

// CFO estimation algorithm taken from "Carrier Frequency Synchronization in the
// Downlink of 3GPP LTE", Qi Wang, C. Mehlfuhrer, M. Rupp
static float chest_estimate_cfo(srsran_chest_dl_t* q)
{
  float n  = (float)srsran_symbol_sz(q->cell.nof_prb);
  float ns = (float)SRSRAN_CP_NSYMB(q->cell.cp);
  float ng = (float)SRSRAN_CP_LEN_NORM(1, n);

  srsran_dl_sf_cfg_t sf_cfg;
  ZERO_OBJECT(sf_cfg);

  uint32_t npilots = srsran_refsignal_cs_nof_re(&q->csr_refs, &sf_cfg, 0);

  // Compute angles between slots
  for (int i = 0; i < 2; i++) {
    srsran_vec_prod_conj_ccc(&q->pilot_estimates[i * npilots / 4],
                             &q->pilot_estimates[(i + 2) * npilots / 4],
                             &q->tmp_cfo_estimate[i * npilots / 4],
                             npilots / 4);
  }
  // Average all angles
  cf_t sum = srsran_vec_acc_cc(q->tmp_cfo_estimate, npilots / 2);

  // Compute CFO
  return -cargf(sum) * n / (ns * (n + ng)) / 2 / M_PI;
}

static void chest_interpolate_noise_est(srsran_chest_dl_t*     q,
                                        srsran_dl_sf_cfg_t*    sf,
                                        srsran_chest_dl_cfg_t* cfg,
                                        cf_t*                  input,
                                        cf_t*                  ce,
                                        uint32_t               port_id,
                                        uint32_t               rxant_id)
{
  float       filter[SRSRAN_CHEST_MAX_SMOOTH_FIL_LEN];
  uint32_t    filter_len = 0;
  uint32_t    sf_idx     = sf->tti % 10;
  srsran_sf_t ch_mode    = sf->sf_type;

  if (cfg->cfo_estimate_enable && ((1 << sf_idx) & cfg->cfo_estimate_sf_mask) && ch_mode != SRSRAN_SF_MBSFN) {
    q->cfo = chest_estimate_cfo(q);
  }

  /* Estimate noise */
  if (cfg->noise_alg == SRSRAN_NOISE_ALG_REFS) {
    if (ch_mode == SRSRAN_SF_MBSFN) {
      ERROR("Warning: REFS noise estimation algorithm not supported in MBSFN subframes");
    }

    q->noise_estimate[rxant_id][port_id] = estimate_noise_pilots(q, sf, port_id);
  }

  if (q->wiener_dl && ch_mode == SRSRAN_SF_NORM && cfg->estimator_alg == SRSRAN_ESTIMATOR_ALG_WIENER) {
    bool     ready   = q->wiener_dl->ready;
    uint32_t nre     = q->cell.nof_prb * SRSRAN_NRE;
    uint32_t nref    = q->cell.nof_prb * 2;
    uint32_t shift   = srsran_refsignal_cs_fidx(q->cell, 0, port_id, 0);
    uint32_t nsymb   = srsran_refsignal_cs_nof_symbols(&q->csr_refs, sf, port_id);
    float    snr_lin = +INFINITY;

    if (isnormal(q->noise_estimate[rxant_id][port_id]) && isnormal(q->rsrp[rxant_id][port_id])) {
      snr_lin = q->rsrp[rxant_id][port_id] / q->noise_estimate[rxant_id][port_id] / 2;
    }

    for (uint32_t m = 0, l = 0; m < 2 * SRSRAN_CP_NORM_NSYMB + 4; m++) {
      uint32_t ce_idx = 0;

      if (m >= 4) {
        ce_idx = (m - 4) * nre;
      }

      uint32_t k = srsran_refsignal_cs_nsymbol(l, q->cell.cp, port_id);
      srsran_wiener_dl_run(
          q->wiener_dl, port_id, rxant_id, m, shift, &q->pilot_estimates[nref * l], &ce[ce_idx], snr_lin);

      if (m == k) {
        l = (l + 1) % nsymb;
      }
    }
    if (ready) {
      return;
    }
  }

  if (ce != NULL) {
    switch (cfg->filter_type) {
      case SRSRAN_CHEST_FILTER_GAUSS:
        if (ch_mode == SRSRAN_SF_MBSFN) {
          ERROR("Warning: Gauss filter not supported in MBSFN subframes");
        }
        if (cfg->filter_coef[0] <= 0) {
          filter_len = srsran_chest_set_smooth_filter_gauss(filter, 4, q->noise_estimate[rxant_id][port_id] * 200.0f);
        } else {
          filter_len = srsran_chest_set_smooth_filter_gauss(filter, (uint32_t)cfg->filter_coef[0], cfg->filter_coef[1]);
        }
        break;
      case SRSRAN_CHEST_FILTER_TRIANGLE:
        filter_len = srsran_chest_set_smooth_filter3_coeff(filter, cfg->filter_coef[0]);
        break;
      default:
        break;
    }

    if (cfg->estimator_alg != SRSRAN_ESTIMATOR_ALG_INTERPOLATE && ch_mode == SRSRAN_SF_MBSFN) {
      ERROR("Warning: Subframe interpolation must be enabled in MBSFN subframes");
    }

    /* Smooth estimates (if applicable) and interpolate */
    if (cfg->filter_type == SRSRAN_CHEST_FILTER_NONE) {
      interpolate_pilots(q, sf, cfg, q->pilot_estimates, ce, port_id);
    } else {
      average_pilots(q, sf, cfg, q->pilot_estimates, q->pilot_estimates_average, port_id, filter, filter_len);
      interpolate_pilots(q, sf, cfg, q->pilot_estimates_average, ce, port_id);
    }

    /* Estimate noise for PSS and EMPTY algorithms */
    switch (cfg->noise_alg) {
      case SRSRAN_NOISE_ALG_PSS:
        if (sf_idx == 0 || sf_idx == 5) {
          q->noise_estimate[rxant_id][port_id] = estimate_noise_pss(q, input, ce);
        }
        break;
      case SRSRAN_NOISE_ALG_EMPTY:
        if (sf_idx == 0 || sf_idx == 5) {
          q->noise_estimate[rxant_id][port_id] = estimate_noise_empty_sc(q, input);
        }
        break;
      default:
        break;
    }
  }
}

static void
chest_dl_estimate_correct_sync_error(srsran_chest_dl_t* q, srsran_dl_sf_cfg_t* sf, cf_t* input, uint32_t rxant_id)
{
  float pwr_sum  = 0.0f;
  float sync_err = 0.0f;

  // For each cell port...
  for (uint32_t cell_port_id = 0; cell_port_id < q->cell.nof_ports; cell_port_id++) {
    uint32_t npilots = srsran_refsignal_cs_nof_re(&q->csr_refs, sf, cell_port_id);
    uint32_t nsymb   = srsran_refsignal_cs_nof_symbols(&q->csr_refs, sf, cell_port_id);

    // Get references from the input signal
    srsran_refsignal_cs_get_sf(&q->csr_refs, sf, cell_port_id, input, q->pilot_recv_signal);

    // Use the known CSR signal to compute Least-squares estimates
    srsran_vec_prod_conj_ccc(
        q->pilot_recv_signal, q->csr_refs.pilots[cell_port_id / 2][sf->tti % 10], q->pilot_estimates, npilots);

    // Estimate synchronization error from the phase shift
    float k   = (float)srsran_symbol_sz(q->cell.nof_prb) / 6.0f;
    float sum = 0.0f;
    for (uint32_t i = 0; i < nsymb; i++) {
      sum += srsran_vec_estimate_frequency(q->pilot_estimates + i * npilots / nsymb, npilots / nsymb) * k;
    }
    float pwr = srsran_vec_avg_power_cf(q->pilot_estimates, npilots);

    // Average symbol sum
    q->sync_err[rxant_id][cell_port_id] = sum / nsymb;

    // Accumulate if the result is valid
    if (!isinf(sum) && !isnan(sum) && !isinf(pwr) && !isnan(pwr)) {
      sync_err += q->sync_err[rxant_id][cell_port_id] * pwr;
      pwr_sum += pwr;
    }
  }

  // Average the valid measurements
  if (isnormal(pwr_sum)) {
    sync_err /= pwr_sum;
  }

  // Correct time synchronization error if estimated is not NAN, not INF and greater than 0.05 samples
  if (isnormal(sync_err) && fabsf(sync_err) > 0.05f) {
    // Compute required frequency shift, convert from sample error to normalised sine
    float    cfo   = sync_err / (float)srsran_symbol_sz(q->cell.nof_prb);
    uint32_t nre   = SRSRAN_NRE * q->cell.nof_prb;
    uint32_t nsymb = SRSRAN_CP_NSYMB(q->cell.cp) * SRSRAN_NOF_SLOTS_PER_SF;

    // For each symbol...
    for (uint32_t i = 0; i < nsymb; i++) {
      // Do a frequency shift
      cf_t* ptr = &input[i * nre];
      srsran_vec_apply_cfo(ptr, cfo, ptr, nre);
    }
  }
}

static int estimate_port(srsran_chest_dl_t*     q,
                         srsran_dl_sf_cfg_t*    sf,
                         srsran_chest_dl_cfg_t* cfg,
                         cf_t*                  input,
                         cf_t*                  ce,
                         uint32_t               port_id,
                         uint32_t               rxant_id)
{
  uint32_t npilots = srsran_refsignal_cs_nof_re(&q->csr_refs, sf, port_id);

  /* Get references from the input signal */
  srsran_refsignal_cs_get_sf(&q->csr_refs, sf, port_id, input, q->pilot_recv_signal);

  /* Use the known CSR signal to compute Least-squares estimates */
  srsran_vec_prod_conj_ccc(
      q->pilot_recv_signal, q->csr_refs.pilots[port_id / 2][sf->tti % 10], q->pilot_estimates, npilots);

  /* Compute RSRP for the channel estimates in this port */
  if (cfg->rsrp_neighbour) {
    double energy                   = cabsf(srsran_vec_acc_cc(q->pilot_estimates, npilots) / npilots);
    q->rsrp_corr[rxant_id][port_id] = energy * energy;
  }
  q->rsrp[rxant_id][port_id] = srsran_vec_avg_power_cf(q->pilot_recv_signal, npilots);
  q->rssi[rxant_id][port_id] = chest_dl_rssi(q, sf, input, port_id);

  chest_interpolate_noise_est(q, sf, cfg, input, ce, port_id, rxant_id);

  return 0;
}

static int estimate_port_mbsfn(srsran_chest_dl_t*     q,
                               srsran_dl_sf_cfg_t*    sf,
                               srsran_chest_dl_cfg_t* cfg,
                               cf_t*                  input,
                               cf_t*                  ce,
                               uint32_t               port_id,
                               uint32_t               rxant_id)
{
  uint32_t sf_idx        = sf->tti % 10;
  uint16_t mbsfn_area_id = cfg->mbsfn_area_id;

  if (!q->mbsfn_refs[mbsfn_area_id]) {
    ERROR("Error in chest_dl: MBSFN area id=%d not initialized", cfg->mbsfn_area_id);
  }

  /* Use the known CSR signal to compute Least-squares estimates */
  srsran_refsignal_mbsfn_get_sf(q->cell, port_id, input, q->pilot_recv_signal);
  // estimate for non-mbsfn section of subframe
  srsran_vec_prod_conj_ccc(
      q->pilot_recv_signal, q->csr_refs.pilots[port_id / 2][sf_idx], q->pilot_estimates, (2 * q->cell.nof_prb));

  srsran_vec_prod_conj_ccc(&q->pilot_recv_signal[(2 * q->cell.nof_prb)],
                           q->mbsfn_refs[mbsfn_area_id]->pilots[port_id / 2][sf_idx],
                           &q->pilot_estimates[(2 * q->cell.nof_prb)],
                           SRSRAN_REFSIGNAL_NUM_SF_MBSFN(q->cell.nof_prb, port_id) - (2 * q->cell.nof_prb));

  chest_interpolate_noise_est(q, sf, cfg, input, ce, port_id, rxant_id);

  return 0;
}

static float get_noise(srsran_chest_dl_t* q)
{
  float n = 0;
  for (int i = 0; i < q->nof_rx_antennas; i++) {
    n += srsran_vec_acc_ff(q->noise_estimate[i], q->cell.nof_ports) / q->cell.nof_ports;
  }
  if (q->nof_rx_antennas) {
    n /= q->nof_rx_antennas;
  }
  return n;
}

static float get_rssi(srsran_chest_dl_t* q)
{
  float n = 0;
  for (int i = 0; i < q->nof_rx_antennas; i++) {
    n += 4 * q->rssi[i][0] / q->cell.nof_prb / SRSRAN_NRE;
  }
  return n / q->nof_rx_antennas;
}

/* q->rssi[0] is the average power in all RE in all symbol containing references for port 0 . q->rssi[0]/q->cell.nof_prb
 * is the average power per PRB q->rsrp[0] is the average power of RE containing references only (for port 0).
 */
static float get_rsrq(srsran_chest_dl_t* q)
{
  float n = 0;
  for (int i = 0; i < q->nof_rx_antennas; i++) {
    n += q->cell.nof_prb * q->rsrp[i][0] / q->rssi[i][0];
  }
  return n / q->nof_rx_antennas;
}

static float get_rsrp_port(srsran_chest_dl_t* q, uint32_t port)
{
  float sum = 0.0f;
  for (int j = 0; j < q->nof_rx_antennas; ++j) {
    sum += q->rsrp[j][port];
  }

  if (q->nof_rx_antennas) {
    sum /= q->nof_rx_antennas;
  }

  return sum;
}

static float get_rsrp_neighbour_port(srsran_chest_dl_t* q, uint32_t port)
{
  float sum = 0.0f;
  for (int j = 0; j < q->cell.nof_ports; ++j) {
    sum += q->rsrp_corr[port][j];
  }

  if (q->cell.nof_ports) {
    sum /= q->cell.nof_ports;
  }

  return sum;
}

static float get_rsrp(srsran_chest_dl_t* q)
{
  float max = -1e9;
  for (int i = 0; i < q->cell.nof_ports; ++i) {
    float v = get_rsrp_port(q, i);
    if (v > max) {
      max = v;
    }
  }
  return max;
}

static float get_snr(srsran_chest_dl_t* q)
{
#ifdef FREQ_SEL_SNR
  int nref = SRSRAN_REFSIGNAL_NUM_SF(q->cell.nof_prb, 0);
  return srsran_vec_acc_ff(q->snr_vector, nref) / nref;
#else
  return get_rsrp(q) / get_noise(q);
#endif
}

static float get_rsrp_neighbour(srsran_chest_dl_t* q)
{
  float max = -1e9;
  for (int i = 0; i < q->nof_rx_antennas; ++i) {
    float v = get_rsrp_neighbour_port(q, i);
    if (v > max) {
      max = v;
    }
  }
  return max;
}

static void fill_res(srsran_chest_dl_t* q, srsran_chest_dl_res_t* res)
{
  res->noise_estimate     = get_noise(q);
  res->noise_estimate_dbm = srsran_convert_power_to_dBm(res->noise_estimate);
  res->cfo                = q->cfo;
  res->rsrp               = get_rsrp(q);
  res->rsrp_dbm           = srsran_convert_power_to_dBm(res->rsrp);
  res->rsrp_neigh         = get_rsrp_neighbour(q);
  res->rsrq               = get_rsrq(q);
  res->rsrq_db            = srsran_convert_power_to_dB(res->rsrq);
  res->snr_db             = srsran_convert_power_to_dB(get_snr(q));
  res->rssi_dbm           = srsran_convert_power_to_dBm(get_rssi(q));
  res->sync_error         = q->sync_err[0][0]; // Take only the channel used for synch

  for (uint32_t port_id = 0; port_id < q->cell.nof_ports; port_id++) {
    res->rsrp_port_dbm[port_id] = srsran_convert_power_to_dBm(get_rsrp_port(q, port_id));
    for (uint32_t a = 0; a < q->nof_rx_antennas; a++) {
      res->snr_ant_port_db[a][port_id] =
          srsran_convert_power_to_dB(q->rsrp[a][port_id] / q->noise_estimate[a][port_id]);
      res->rsrp_ant_port_dbm[a][port_id] = srsran_convert_power_to_dBm(q->rsrp[a][port_id]);
      res->rsrq_ant_port_db[a][port_id] =
          srsran_convert_power_to_dB(q->cell.nof_prb * q->rsrp[a][port_id] / q->rssi[a][port_id]);
    }
  }
}

int srsran_chest_dl_estimate(srsran_chest_dl_t*     q,
                             srsran_dl_sf_cfg_t*    sf,
                             cf_t*                  input[SRSRAN_MAX_PORTS],
                             srsran_chest_dl_res_t* res)
{
  srsran_chest_dl_cfg_t cfg;
  ZERO_OBJECT(cfg);

  return srsran_chest_dl_estimate_cfg(q, sf, &cfg, input, res);
}

int srsran_chest_dl_estimate_cfg(srsran_chest_dl_t*     q,
                                 srsran_dl_sf_cfg_t*    sf,
                                 srsran_chest_dl_cfg_t* cfg,
                                 cf_t*                  input[SRSRAN_MAX_PORTS],
                                 srsran_chest_dl_res_t* res)
{
  for (uint32_t rxant_id = 0; rxant_id < q->nof_rx_antennas; rxant_id++) {
    // Estimate and correct synchronization error if enabled
    if (cfg->sync_error_enable) {
      chest_dl_estimate_correct_sync_error(q, sf, input[rxant_id], rxant_id);
    }

    for (uint32_t port_id = 0; port_id < q->cell.nof_ports; port_id++) {
      if (sf->sf_type == SRSRAN_SF_MBSFN) {
        if (estimate_port_mbsfn(q, sf, cfg, input[rxant_id], res->ce[port_id][rxant_id], port_id, rxant_id)) {
          return SRSRAN_ERROR;
        }
      } else {
        if (estimate_port(q, sf, cfg, input[rxant_id], res->ce[port_id][rxant_id], port_id, rxant_id)) {
          return SRSRAN_ERROR;
        }
      }
    }
  }

  fill_res(q, res);

  return SRSRAN_SUCCESS;
}

srsran_chest_dl_estimator_alg_t srsran_chest_dl_str2estimator_alg(const char* str)
{
  srsran_chest_dl_estimator_alg_t ret = SRSRAN_ESTIMATOR_ALG_AVERAGE;

  if (str) {
    if (strcmp(str, "interpolate") == 0) {
      ret = SRSRAN_ESTIMATOR_ALG_INTERPOLATE;
    } else if (strcmp(str, "average") == 0) {
      ret = SRSRAN_ESTIMATOR_ALG_AVERAGE;
    } else if (strcmp(str, "wiener") == 0) {
      ret = SRSRAN_ESTIMATOR_ALG_WIENER;
    }
  }

  return ret;
}
