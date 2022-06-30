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

#include <complex.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/chest_ul.h"
#include "srsran/phy/dft/dft_precoding.h"
#include "srsran/phy/utils/convolution.h"
#include "srsran/phy/utils/vector.h"
#include "srsran/srsran.h"

#define NOF_REFS_SYM (q->cell.nof_prb * SRSRAN_NRE)
#define NOF_REFS_SF (NOF_REFS_SYM * 2) // 2 reference symbols per subframe

#define MAX_REFS_SYM (max_prb * SRSRAN_NRE)
#define MAX_REFS_SF (max_prb * SRSRAN_NRE * 2) // 2 reference symbols per subframe

/** 3GPP LTE Downlink channel estimator and equalizer.
 * Estimates the channel in the resource elements transmitting references and interpolates for the rest
 * of the resource grid.
 *
 * The equalizer uses the channel estimates to produce an estimation of the transmitted symbol.
 *
 * This object depends on the srsran_refsignal_t object for creating the LTE CSR signal.
 */

int srsran_chest_ul_init(srsran_chest_ul_t* q, uint32_t max_prb)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    bzero(q, sizeof(srsran_chest_ul_t));

    q->tmp_noise = srsran_vec_cf_malloc(MAX_REFS_SF);
    if (!q->tmp_noise) {
      perror("malloc");
      goto clean_exit;
    }
    q->pilot_estimates = srsran_vec_cf_malloc(MAX_REFS_SF);
    if (!q->pilot_estimates) {
      perror("malloc");
      goto clean_exit;
    }
    for (int i = 0; i < 4; i++) {
      q->pilot_estimates_tmp[i] = srsran_vec_cf_malloc(MAX_REFS_SF);
      if (!q->pilot_estimates_tmp[i]) {
        perror("malloc");
        goto clean_exit;
      }
    }
    q->pilot_recv_signal = srsran_vec_cf_malloc(MAX_REFS_SF + 1);
    if (!q->pilot_recv_signal) {
      perror("malloc");
      goto clean_exit;
    }

    q->pilot_known_signal = srsran_vec_cf_malloc(MAX_REFS_SF + 1);
    if (!q->pilot_known_signal) {
      perror("malloc");
      goto clean_exit;
    }

    if (srsran_interp_linear_vector_init(&q->srsran_interp_linvec, MAX_REFS_SYM)) {
      ERROR("Error initializing vector interpolator");
      goto clean_exit;
    }

    q->smooth_filter_len = 3;
    srsran_chest_set_smooth_filter3_coeff(q->smooth_filter, 0.3333);

    q->dmrs_signal_configured = false;

    if (srsran_refsignal_dmrs_pusch_pregen_init(&q->dmrs_pregen, max_prb)) {
      ERROR("Error allocating memory for pregenerated signals");
      goto clean_exit;
    }
  }

  ret = SRSRAN_SUCCESS;

clean_exit:
  if (ret != SRSRAN_SUCCESS) {
    srsran_chest_ul_free(q);
  }
  return ret;
}

void srsran_chest_ul_free(srsran_chest_ul_t* q)
{
  srsran_refsignal_dmrs_pusch_pregen_free(&q->dmrs_signal, &q->dmrs_pregen);

  if (q->tmp_noise) {
    free(q->tmp_noise);
  }
  srsran_interp_linear_vector_free(&q->srsran_interp_linvec);

  if (q->pilot_estimates) {
    free(q->pilot_estimates);
  }
  for (int i = 0; i < 4; i++) {
    if (q->pilot_estimates_tmp[i]) {
      free(q->pilot_estimates_tmp[i]);
    }
  }
  if (q->pilot_recv_signal) {
    free(q->pilot_recv_signal);
  }
  if (q->pilot_known_signal) {
    free(q->pilot_known_signal);
  }
  bzero(q, sizeof(srsran_chest_ul_t));
}

int srsran_chest_ul_res_init(srsran_chest_ul_res_t* q, uint32_t max_prb)
{
  bzero(q, sizeof(srsran_chest_ul_res_t));
  q->nof_re = SRSRAN_SF_LEN_RE(max_prb, SRSRAN_CP_NORM);
  q->ce     = srsran_vec_cf_malloc(q->nof_re);
  if (!q->ce) {
    perror("malloc");
    return -1;
  }
  return 0;
}

void srsran_chest_ul_res_set_identity(srsran_chest_ul_res_t* q)
{
  for (uint32_t i = 0; i < q->nof_re; i++) {
    q->ce[i] = 1.0;
  }
}

void srsran_chest_ul_res_free(srsran_chest_ul_res_t* q)
{
  if (q->ce) {
    free(q->ce);
  }
}

int srsran_chest_ul_set_cell(srsran_chest_ul_t* q, srsran_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL && srsran_cell_isvalid(&cell)) {
    if (cell.id != q->cell.id || q->cell.nof_prb == 0) {
      q->cell = cell;
      ret     = srsran_refsignal_ul_set_cell(&q->dmrs_signal, cell);
      if (ret != SRSRAN_SUCCESS) {
        ERROR("Error initializing CSR signal (%d)", ret);
        return SRSRAN_ERROR;
      }

      if (srsran_interp_linear_vector_resize(&q->srsran_interp_linvec, NOF_REFS_SYM)) {
        ERROR("Error initializing vector interpolator");
        return SRSRAN_ERROR;
      }
    }
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

void srsran_chest_ul_pregen(srsran_chest_ul_t*                 q,
                            srsran_refsignal_dmrs_pusch_cfg_t* cfg,
                            srsran_refsignal_srs_cfg_t*        srs_cfg)
{
  srsran_refsignal_dmrs_pusch_pregen(&q->dmrs_signal, &q->dmrs_pregen, cfg);
  q->dmrs_signal_configured = true;

  if (srs_cfg) {
    srsran_refsignal_srs_pregen(&q->dmrs_signal, &q->srs_pregen, srs_cfg, cfg);
    q->srs_signal_configured = true;
  }
}

/* Uses the difference between the averaged and non-averaged pilot estimates */
static float estimate_noise_pilots(srsran_chest_ul_t* q, cf_t* ce, uint32_t nslots, uint32_t nrefs, uint32_t n_prb[2])
{
  float power = 0;
  for (int i = 0; i < nslots; i++) {
    power += srsran_chest_estimate_noise_pilots(
        &q->pilot_estimates[i * nrefs],
        &ce[SRSRAN_REFSIGNAL_UL_L(i, q->cell.cp) * q->cell.nof_prb * SRSRAN_NRE + n_prb[i] * SRSRAN_NRE],
        q->tmp_noise,
        nrefs);
  }

  power /= nslots;

  if (q->smooth_filter_len == 3) {
    // Calibrated for filter length 3
    float w = q->smooth_filter[0];
    float a = 7.419 * w * w + 0.1117 * w - 0.005387;
    return (power / (a * 0.8));
  } else {
    return power;
  }
}

// The interpolator currently only supports same frequency allocation for each subframe
#define cesymb(i) ce[SRSRAN_RE_IDX(q->cell.nof_prb, i, n_prb[0] * SRSRAN_NRE)]
static void interpolate_pilots(srsran_chest_ul_t* q, cf_t* ce, uint32_t nslots, uint32_t nrefs, uint32_t n_prb[2])
{
#ifdef DO_LINEAR_INTERPOLATION
  uint32_t L1 = SRSRAN_REFSIGNAL_UL_L(0, q->cell.cp);
  uint32_t L2 = SRSRAN_REFSIGNAL_UL_L(1, q->cell.cp);
  uint32_t NL = 2 * SRSRAN_CP_NSYMB(q->cell.cp);

  /* Interpolate in the time domain between symbols */
  srsran_interp_linear_vector3(
      &q->srsran_interp_linvec, &cesymb(L2), &cesymb(L1), &cesymb(L1), &cesymb(L1 - 1), (L2 - L1), L1, false, nrefs);
  srsran_interp_linear_vector3(
      &q->srsran_interp_linvec, &cesymb(L1), &cesymb(L2), NULL, &cesymb(L1 + 1), (L2 - L1), (L2 - L1) - 1, true, nrefs);
  srsran_interp_linear_vector3(&q->srsran_interp_linvec,
                               &cesymb(L1),
                               &cesymb(L2),
                               &cesymb(L2),
                               &cesymb(L2 + 1),
                               (L2 - L1),
                               (NL - L2) - 1,
                               true,
                               nrefs);
#else
  // Instead of a linear interpolation, we just copy the estimates to all symbols in that subframe
  for (int s = 0; s < nslots; s++) {
    for (int i = 0; i < SRSRAN_CP_NSYMB(q->cell.cp); i++) {
      int src_symb = SRSRAN_REFSIGNAL_UL_L(s, q->cell.cp);
      int dst_symb = i + s * SRSRAN_CP_NSYMB(q->cell.cp);

      // skip the symbol with the estimates
      if (dst_symb != src_symb) {
        srsran_vec_cf_copy(&ce[(dst_symb * q->cell.nof_prb + n_prb[s]) * SRSRAN_NRE],
                           &ce[(src_symb * q->cell.nof_prb + n_prb[s]) * SRSRAN_NRE],
                           nrefs);
      }
    }
  }
#endif
}

static void
average_pilots(srsran_chest_ul_t* q, cf_t* input, cf_t* ce, uint32_t nslots, uint32_t nrefs, uint32_t n_prb[2])
{
  for (uint32_t i = 0; i < nslots; i++) {
    srsran_chest_average_pilots(
        &input[i * nrefs],
        &ce[SRSRAN_REFSIGNAL_UL_L(i, q->cell.cp) * q->cell.nof_prb * SRSRAN_NRE + n_prb[i] * SRSRAN_NRE],
        q->smooth_filter,
        nrefs,
        1,
        q->smooth_filter_len);
  }
}

/**
 * Generic PUSCH and DMRS channel estimation. It assumes q->pilot_estimates has been populated with the Least Square
 * Estimates
 *
 * @param q Uplink Channel estimation instance
 * @param nslots number of slots (2 for DMRS, 1 for SRS)
 * @param nrefs_sym number of reference resource elements per symbols (depends on configuration)
 * @param stride sub-carrier distance between reference signal resource elements (1 for DMRS, 2 for SRS)
 * @param meas_ta_en enables or disables the Time Alignment error measurement
 * @param write_estimates Write channel estimation in res, (true for DMRS and false for SRS)
 * @param n_prb Resource block start for the grant, set to zero for Sounding Reference Signals
 * @param res UL channel estimation result
 */
static void chest_ul_estimate(srsran_chest_ul_t*     q,
                              uint32_t               nslots,
                              uint32_t               nrefs_sym,
                              uint32_t               stride,
                              bool                   meas_ta_en,
                              bool                   write_estimates,
                              uint32_t               n_prb[SRSRAN_NOF_SLOTS_PER_SF],
                              srsran_chest_ul_res_t* res)
{
  // Calculate CFO
  if (nslots == 2) {
    float phase = cargf(srsran_vec_dot_prod_conj_ccc(
        &q->pilot_estimates[0 * nrefs_sym], &q->pilot_estimates[1 * nrefs_sym], nrefs_sym));
    res->cfo_hz = phase / (2.0f * (float)M_PI * 0.0005f);
  } else {
    res->cfo_hz = NAN;
  }

  // Calculate time alignment error
  float ta_err = 0.0f;
  if (meas_ta_en) {
    for (int i = 0; i < nslots; i++) {
      ta_err += srsran_vec_estimate_frequency(&q->pilot_estimates[i * nrefs_sym], nrefs_sym) / nslots;
    }
  }

  // Calculate actual time alignment error in micro-seconds
  if (isnormal(ta_err) && stride > 0) {
    ta_err /= (float)stride;                     // Divide by the pilot spacing
    ta_err /= 15e3f;                             // Convert from normalized frequency to seconds
    ta_err *= 1e6f;                              // Convert to micro-seconds
    ta_err     = roundf(ta_err * 10.0f) / 10.0f; // Round to one tenth of micro-second
    res->ta_us = ta_err;
  } else {
    res->ta_us = 0.0f;
  }

  // Check if intra-subframe frequency hopping is enabled
  if (n_prb[0] != n_prb[1]) {
    ERROR("ERROR: intra-subframe frequency hopping not supported in the estimator!!");
  }

  if (res->ce != NULL) {
    if (q->smooth_filter_len > 0) {
      average_pilots(q, q->pilot_estimates, res->ce, nslots, nrefs_sym, n_prb);

      if (write_estimates) {
        interpolate_pilots(q, res->ce, nslots, nrefs_sym, n_prb);
      }

      // If averaging, compute noise from difference between received and averaged estimates
      res->noise_estimate = estimate_noise_pilots(q, res->ce, nslots, nrefs_sym, n_prb);
    } else {
      // Copy estimates to CE vector without averaging
      for (int i = 0; i < nslots; i++) {
        srsran_vec_cf_copy(
            &res->ce[SRSRAN_REFSIGNAL_UL_L(i, q->cell.cp) * q->cell.nof_prb * SRSRAN_NRE + n_prb[i] * SRSRAN_NRE],
            &q->pilot_estimates[i * nrefs_sym],
            nrefs_sym);
      }
      if (write_estimates) {
        interpolate_pilots(q, res->ce, nslots, nrefs_sym, n_prb);
      }
      res->noise_estimate = 0;
    }
  }

  // Measure reference signal RE average power
  cf_t  corr     = srsran_vec_acc_cc(q->pilot_recv_signal, nslots * nrefs_sym) / (nslots * nrefs_sym);
  float rsrp_avg = __real__ corr * __real__ corr + __imag__ corr * __imag__ corr;

  // Measure EPRE
  float epre = srsran_vec_avg_power_cf(q->pilot_recv_signal, nslots * nrefs_sym);

  // RSRP shall not be greater than EPRE
  rsrp_avg = SRSRAN_MIN(rsrp_avg, epre);

  // Calculate SNR
  if (isnormal(res->noise_estimate)) {
    res->snr = epre / res->noise_estimate;
  } else {
    res->snr = NAN;
  }

  // Set EPRE and RSRP
  res->epre                = epre;
  res->epre_dBfs           = srsran_convert_power_to_dB(res->epre);
  res->rsrp                = rsrp_avg;
  res->rsrp_dBfs           = srsran_convert_power_to_dB(res->rsrp);
  res->snr_db              = srsran_convert_power_to_dB(res->snr);
  res->noise_estimate_dbFs = srsran_convert_power_to_dBm(res->noise_estimate);
}

int srsran_chest_ul_estimate_pusch(srsran_chest_ul_t*     q,
                                   srsran_ul_sf_cfg_t*    sf,
                                   srsran_pusch_cfg_t*    cfg,
                                   cf_t*                  input,
                                   srsran_chest_ul_res_t* res)
{
  if (!q->dmrs_signal_configured) {
    ERROR("Error must call srsran_chest_ul_set_cfg() before using the UL estimator");
    return SRSRAN_ERROR;
  }

  uint32_t nof_prb = cfg->grant.L_prb;

  if (!srsran_dft_precoding_valid_prb(nof_prb)) {
    ERROR("Error invalid nof_prb=%d", nof_prb);
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  int nrefs_sym = nof_prb * SRSRAN_NRE;
  int nrefs_sf  = nrefs_sym * SRSRAN_NOF_SLOTS_PER_SF;

  /* Get references from the input signal */
  srsran_refsignal_dmrs_pusch_get(&q->dmrs_signal, cfg, input, q->pilot_recv_signal);

  // Use the known DMRS signal to compute Least-squares estimates
  srsran_vec_prod_conj_ccc(q->pilot_recv_signal,
                           q->dmrs_pregen.r[cfg->grant.n_dmrs][sf->tti % SRSRAN_NOF_SF_X_FRAME][nof_prb],
                           q->pilot_estimates,
                           nrefs_sf);

  // Estimate
  chest_ul_estimate(q, SRSRAN_NOF_SLOTS_PER_SF, nrefs_sym, 1, cfg->meas_ta_en, true, cfg->grant.n_prb, res);

  return 0;
}

static float
estimate_noise_pilots_pucch(srsran_chest_ul_t* q, cf_t* ce, uint32_t n_rs, uint32_t n_prb[SRSRAN_NOF_SLOTS_PER_SF])
{
  float power = 0;
  for (int ns = 0; ns < SRSRAN_NOF_SLOTS_PER_SF; ns++) {
    for (int i = 0; i < n_rs; i++) {
      // All CE are the same, so pick the first symbol of the first slot always and compare with the noisy estimates
      power += srsran_chest_estimate_noise_pilots(
          &q->pilot_estimates[(i + ns * n_rs) * SRSRAN_NRE],
          &ce[SRSRAN_RE_IDX(q->cell.nof_prb, ns * SRSRAN_CP_NSYMB(q->cell.cp), n_prb[ns] * SRSRAN_NRE)],
          q->tmp_noise,
          SRSRAN_NRE);
    }
  }

  power /= (SRSRAN_NOF_SLOTS_PER_SF * n_rs);

  if (q->smooth_filter_len == 3) {
    // Calibrated for filter length 3
    float w = q->smooth_filter[0];
    float a = 7.419 * w * w + 0.1117 * w - 0.005387;
    return (power / (a * 0.8));
  } else {
    return power;
  }
}

int srsran_chest_ul_estimate_pucch(srsran_chest_ul_t*     q,
                                   srsran_ul_sf_cfg_t*    sf,
                                   srsran_pucch_cfg_t*    cfg,
                                   cf_t*                  input,
                                   srsran_chest_ul_res_t* res)
{
  int n_rs = srsran_refsignal_dmrs_N_rs(cfg->format, q->cell.cp);
  if (!n_rs) {
    ERROR("Error computing N_rs");
    return SRSRAN_ERROR;
  }
  int nrefs_sf = SRSRAN_NRE * n_rs * SRSRAN_NOF_SLOTS_PER_SF;

  /* Get references from the input signal */
  srsran_refsignal_dmrs_pucch_get(&q->dmrs_signal, cfg, input, q->pilot_recv_signal);

  /* Generate known pilots */
  if (cfg->format == SRSRAN_PUCCH_FORMAT_2A || cfg->format == SRSRAN_PUCCH_FORMAT_2B) {
    float max   = -1e9;
    int   i_max = 0;

    int m = 0;
    if (cfg->format == SRSRAN_PUCCH_FORMAT_2A) {
      m = 2;
    } else {
      m = 4;
    }

    for (int i = 0; i < m; i++) {
      cfg->pucch2_drs_bits[0] = i % 2;
      cfg->pucch2_drs_bits[1] = i / 2;
      srsran_refsignal_dmrs_pucch_gen(&q->dmrs_signal, sf, cfg, q->pilot_known_signal);
      srsran_vec_prod_conj_ccc(q->pilot_recv_signal, q->pilot_known_signal, q->pilot_estimates_tmp[i], nrefs_sf);
      float x = cabsf(srsran_vec_acc_cc(q->pilot_estimates_tmp[i], nrefs_sf));
      if (x >= max) {
        max   = x;
        i_max = i;
      }
    }
    memcpy(q->pilot_estimates, q->pilot_estimates_tmp[i_max], nrefs_sf * sizeof(cf_t));
    cfg->pucch2_drs_bits[0] = i_max % 2;
    cfg->pucch2_drs_bits[1] = i_max / 2;

  } else {
    srsran_refsignal_dmrs_pucch_gen(&q->dmrs_signal, sf, cfg, q->pilot_known_signal);
    /* Use the known DMRS signal to compute Least-squares estimates */
    srsran_vec_prod_conj_ccc(q->pilot_recv_signal, q->pilot_known_signal, q->pilot_estimates, nrefs_sf);
  }

  // Measure reference signal RE average power
  cf_t corr = srsran_vec_acc_cc(q->pilot_estimates, SRSRAN_NOF_SLOTS_PER_SF * SRSRAN_NRE * n_rs) /
              (SRSRAN_NOF_SLOTS_PER_SF * SRSRAN_NRE * n_rs);
  float rsrp_avg = __real__ corr * __real__ corr + __imag__ corr * __imag__ corr;

  // Measure EPRE
  float epre = srsran_vec_avg_power_cf(q->pilot_estimates, SRSRAN_NOF_SLOTS_PER_SF * SRSRAN_NRE * n_rs);

  // RSRP shall not be greater than EPRE
  rsrp_avg = SRSRAN_MIN(rsrp_avg, epre);

  // Set EPRE and RSRP
  res->epre      = epre;
  res->epre_dBfs = srsran_convert_power_to_dB(res->epre);
  res->rsrp      = rsrp_avg;
  res->rsrp_dBfs = srsran_convert_power_to_dB(res->rsrp);

  // Estimate time alignment
  if (cfg->meas_ta_en) {
    float ta_err = 0.0f;
    for (int ns = 0; ns < SRSRAN_NOF_SLOTS_PER_SF; ns++) {
      for (int i = 0; i < n_rs; i++) {
        ta_err += srsran_vec_estimate_frequency(&q->pilot_estimates[(i + ns * n_rs) * SRSRAN_NRE], SRSRAN_NRE) /
                  (float)(SRSRAN_NOF_SLOTS_PER_SF * n_rs);
      }
    }

    // Calculate actual time alignment error in micro-seconds
    if (isnormal(ta_err)) {
      ta_err /= 15e3f;                             // Convert from normalized frequency to seconds
      ta_err *= 1e6f;                              // Convert to micro-seconds
      ta_err     = roundf(ta_err * 10.0f) / 10.0f; // Round to one tenth of micro-second
      res->ta_us = ta_err;
    } else {
      res->ta_us = 0.0f;
    }
  }

  if (res->ce != NULL) {
    uint32_t n_prb[2] = {};

    /* TODO: Currently averaging entire slot, performance good enough? */
    for (int ns = 0; ns < 2; ns++) {
      // Average all slot
      for (int i = 1; i < n_rs; i++) {
        srsran_vec_sum_ccc(&q->pilot_estimates[ns * n_rs * SRSRAN_NRE],
                           &q->pilot_estimates[(i + ns * n_rs) * SRSRAN_NRE],
                           &q->pilot_estimates[ns * n_rs * SRSRAN_NRE],
                           SRSRAN_NRE);
      }
      srsran_vec_sc_prod_ccc(&q->pilot_estimates[ns * n_rs * SRSRAN_NRE],
                             (float)1.0 / n_rs,
                             &q->pilot_estimates[ns * n_rs * SRSRAN_NRE],
                             SRSRAN_NRE);

      // Average in freq domain
      srsran_chest_average_pilots(&q->pilot_estimates[ns * n_rs * SRSRAN_NRE],
                                  &q->pilot_recv_signal[ns * n_rs * SRSRAN_NRE],
                                  q->smooth_filter,
                                  SRSRAN_NRE,
                                  1,
                                  q->smooth_filter_len);

      // Determine n_prb
      n_prb[ns] = srsran_pucch_n_prb(&q->cell, cfg, ns);

      // copy estimates to slot
      for (int i = 0; i < SRSRAN_CP_NSYMB(q->cell.cp); i++) {
        srsran_vec_cf_copy(
            &res->ce[SRSRAN_RE_IDX(q->cell.nof_prb, i + ns * SRSRAN_CP_NSYMB(q->cell.cp), n_prb[ns] * SRSRAN_NRE)],
            &q->pilot_recv_signal[ns * n_rs * SRSRAN_NRE],
            SRSRAN_NRE);
      }
    }

    // Estimate noise/interference
    res->noise_estimate = estimate_noise_pilots_pucch(q, res->ce, n_rs, n_prb);
    if (fpclassify(res->noise_estimate) == FP_ZERO) {
      res->noise_estimate = FLT_MIN;
    }
    res->noise_estimate_dbFs = srsran_convert_power_to_dBm(res->noise_estimate);

    // Estimate SINR
    if (isnormal(res->noise_estimate)) {
      res->snr    = res->epre / res->noise_estimate;
      res->snr_db = srsran_convert_power_to_dB(res->snr);
    } else {
      res->snr    = NAN;
      res->snr_db = NAN;
    }
  }

  return 0;
}

int srsran_chest_ul_estimate_srs(srsran_chest_ul_t*                 q,
                                 srsran_ul_sf_cfg_t*                sf,
                                 srsran_refsignal_srs_cfg_t*        cfg,
                                 srsran_refsignal_dmrs_pusch_cfg_t* pusch_cfg,
                                 cf_t*                              input,
                                 srsran_chest_ul_res_t*             res)
{
  if (q == NULL || sf == NULL || cfg == NULL || pusch_cfg == NULL || input == NULL || res == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Extract parameters
  uint32_t n_srs_re = srsran_refsignal_srs_M_sc(&q->dmrs_signal, cfg);

  // Extract Sounding Reference Signal
  if (srsran_refsignal_srs_get(&q->dmrs_signal, cfg, sf->tti, q->pilot_recv_signal, input) != SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Get Known pilots
  cf_t* known_pilots = q->pilot_known_signal;
  if (q->srs_signal_configured) {
    known_pilots = q->srs_pregen.r[sf->tti % SRSRAN_NOF_SF_X_FRAME];
  } else {
    srsran_refsignal_srs_gen(&q->dmrs_signal, cfg, pusch_cfg, sf->tti % SRSRAN_NOF_SF_X_FRAME, known_pilots);
  }

  // Compute least squares estimates
  srsran_vec_prod_conj_ccc(q->pilot_recv_signal, known_pilots, q->pilot_estimates, n_srs_re);

  // Estimate
  uint32_t n_prb[2] = {};
  chest_ul_estimate(q, 1, n_srs_re, 1, true, false, n_prb, res);

  return SRSRAN_SUCCESS;
}
