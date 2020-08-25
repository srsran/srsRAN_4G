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

#include "srslte/srslte.h"
#include <complex.h>
#include <math.h>
#include <string.h>

#include "srslte/phy/ue/ue_ul.h"

#define CURRENT_FFTSIZE srslte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN SRSLTE_SF_LEN(CURRENT_FFTSIZE)

#define CURRENT_SLOTLEN_RE SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)
#define CURRENT_SFLEN_RE SRSLTE_NOF_RE(q->cell)

#define MAX_SFLEN SRSLTE_SF_LEN(srslte_symbol_sz(max_prb))

#define DEFAULT_CFO_TOL 1.0 // Hz

static bool srs_tx_enabled(srslte_refsignal_srs_cfg_t* srs_cfg, uint32_t tti);

int srslte_ue_ul_init(srslte_ue_ul_t* q, cf_t* out_buffer, uint32_t max_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSLTE_ERROR;

    bzero(q, sizeof(srslte_ue_ul_t));

    q->sf_symbols = srslte_vec_cf_malloc(SRSLTE_SF_LEN_PRB(max_prb));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit;
    }

    srslte_ofdm_cfg_t ofdm_cfg = {};
    ofdm_cfg.nof_prb           = max_prb;
    ofdm_cfg.in_buffer         = q->sf_symbols;
    ofdm_cfg.out_buffer        = out_buffer;
    ofdm_cfg.cp                = SRSLTE_CP_NORM;
    ofdm_cfg.freq_shift_f      = 0.5f;
    ofdm_cfg.normalize         = true;
    if (srslte_ofdm_tx_init_cfg(&q->fft, &ofdm_cfg)) {
      ERROR("Error initiating FFT\n");
      goto clean_exit;
    }

    if (srslte_cfo_init(&q->cfo, MAX_SFLEN)) {
      ERROR("Error creating CFO object\n");
      goto clean_exit;
    }

    if (srslte_pusch_init_ue(&q->pusch, max_prb)) {
      ERROR("Error creating PUSCH object\n");
      goto clean_exit;
    }
    if (srslte_pucch_init_ue(&q->pucch)) {
      ERROR("Error creating PUSCH object\n");
      goto clean_exit;
    }
    if (srslte_refsignal_ul_init(&q->signals, max_prb)) {
      ERROR("Error initiating srslte_refsignal_ul\n");
      goto clean_exit;
    }
    q->refsignal = srslte_vec_cf_malloc(2 * SRSLTE_NRE * max_prb);
    if (!q->refsignal) {
      perror("malloc");
      goto clean_exit;
    }

    q->srs_signal = srslte_vec_cf_malloc(SRSLTE_NRE * max_prb);
    if (!q->srs_signal) {
      perror("malloc");
      goto clean_exit;
    }
    q->out_buffer           = out_buffer;
    q->signals_pregenerated = false;
    ret                     = SRSLTE_SUCCESS;
  } else {
    ERROR("Invalid parameters\n");
  }

clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_ue_ul_free(q);
  }
  return ret;
}

void srslte_ue_ul_free(srslte_ue_ul_t* q)
{
  if (q) {
    srslte_ofdm_tx_free(&q->fft);
    srslte_pusch_free(&q->pusch);
    srslte_pucch_free(&q->pucch);

    srslte_cfo_free(&q->cfo);
    srslte_refsignal_ul_free(&q->signals);

    if (q->sf_symbols) {
      free(q->sf_symbols);
    }
    if (q->refsignal) {
      free(q->refsignal);
    }
    if (q->srs_signal) {
      free(q->srs_signal);
    }
    if (q->signals_pregenerated) {
      srslte_refsignal_dmrs_pusch_pregen_free(&q->signals, &q->pregen_dmrs);
      srslte_refsignal_srs_pregen_free(&q->signals, &q->pregen_srs);
    }
    srslte_ra_ul_pusch_hopping_free(&q->hopping);

    bzero(q, sizeof(srslte_ue_ul_t));
  }
}

int srslte_ue_ul_set_cell(srslte_ue_ul_t* q, srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_cell_isvalid(&cell)) {
    if (q->cell.id != cell.id || q->cell.nof_prb == 0) {
      q->cell = cell;

      if (srslte_ofdm_tx_set_prb(&q->fft, q->cell.cp, q->cell.nof_prb)) {
        ERROR("Error resizing FFT\n");
        return SRSLTE_ERROR;
      }
      if (srslte_cfo_resize(&q->cfo, SRSLTE_SF_LEN_PRB(q->cell.nof_prb))) {
        ERROR("Error resizing CFO object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_pusch_set_cell(&q->pusch, q->cell)) {
        ERROR("Error resizing PUSCH object\n");
        return SRSLTE_ERROR;
      }
      if (srslte_pucch_set_cell(&q->pucch, q->cell)) {
        ERROR("Error resizing PUSCH object\n");
        return SRSLTE_ERROR;
      }
      if (srslte_refsignal_ul_set_cell(&q->signals, q->cell)) {
        ERROR("Error resizing srslte_refsignal_ul\n");
        return SRSLTE_ERROR;
      }

      if (srslte_ra_ul_pusch_hopping_init(&q->hopping, q->cell)) {
        ERROR("Error setting hopping procedure cell\n");
        return SRSLTE_ERROR;
      }
      q->signals_pregenerated = false;
    }
    ret = SRSLTE_SUCCESS;
  } else {
    ERROR("Invalid cell properties ue_ul: Id=%d, Ports=%d, PRBs=%d\n", cell.id, cell.nof_ports, cell.nof_prb);
  }
  return ret;
}

/* Precalculate the PUSCH scramble sequences for a given RNTI. This function takes a while
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 * For the connection procedure, use srslte_pusch_encode_rnti() or srslte_pusch_decode_rnti() functions
 */
void srslte_ue_ul_set_rnti(srslte_ue_ul_t* q, uint16_t rnti)
{
  srslte_pusch_set_rnti(&q->pusch, rnti);
  srslte_pucch_set_rnti(&q->pucch, rnti);
  q->current_rnti = rnti;
}

int srslte_ue_ul_pregen_signals(srslte_ue_ul_t* q, srslte_ue_ul_cfg_t* cfg)
{
  if (q->signals_pregenerated) {
    srslte_refsignal_dmrs_pusch_pregen_free(&q->signals, &q->pregen_dmrs);
    srslte_refsignal_srs_pregen_free(&q->signals, &q->pregen_srs);
  }
  if (srslte_refsignal_dmrs_pusch_pregen(&q->signals, &q->pregen_dmrs, &cfg->ul_cfg.dmrs)) {
    return SRSLTE_ERROR;
  }
  if (srslte_refsignal_srs_pregen(&q->signals, &q->pregen_srs, &cfg->ul_cfg.srs, &cfg->ul_cfg.dmrs)) {
    return SRSLTE_ERROR;
  }
  q->signals_pregenerated = true;
  return SRSLTE_SUCCESS;
}

int srslte_ue_ul_dci_to_pusch_grant(srslte_ue_ul_t*       q,
                                    srslte_ul_sf_cfg_t*   sf,
                                    srslte_ue_ul_cfg_t*   cfg,
                                    srslte_dci_ul_t*      dci,
                                    srslte_pusch_grant_t* grant)
{
  // Convert DCI to Grant
  if (srslte_ra_ul_dci_to_grant(&q->cell, sf, &cfg->ul_cfg.hopping, dci, grant)) {
    return SRSLTE_ERROR;
  }

  // Update shortened before computing grant
  srslte_refsignal_srs_pusch_shortened(&q->signals, sf, &cfg->ul_cfg.srs, &cfg->ul_cfg.pusch);

  // Update RE assuming if shortened is true
  if (sf->shortened) {
    srslte_ra_ul_compute_nof_re(grant, q->cell.cp, true);
  }

  // Assert Grant is valid
  return srslte_pusch_assert_grant(grant);
}

void srslte_ue_ul_pusch_hopping(srslte_ue_ul_t*       q,
                                srslte_ul_sf_cfg_t*   sf,
                                srslte_ue_ul_cfg_t*   cfg,
                                srslte_pusch_grant_t* grant)
{
  if (cfg->ul_cfg.srs.configured && cfg->ul_cfg.hopping.hopping_enabled) {
    ERROR("UL SRS and frequency hopping not currently supported\n");
  }
  return srslte_ra_ul_pusch_hopping(&q->hopping, sf, &cfg->ul_cfg.hopping, grant);
}

static float limit_norm_factor(srslte_ue_ul_t* q, float norm_factor, cf_t* output_signal)
{
  uint32_t p   = srslte_vec_max_abs_fi((float*)output_signal, 2 * SRSLTE_SF_LEN_PRB(q->cell.nof_prb));
  float    amp = fabsf(*((float*)output_signal + p));

  if (amp * norm_factor > 0.95) {
    norm_factor = 0.95 / amp;
  }
  if (amp * norm_factor < 0.1) {
    norm_factor = 0.1 / amp;
  }
  return norm_factor;
}

static void apply_cfo(srslte_ue_ul_t* q, srslte_ue_ul_cfg_t* cfg)
{
  if (cfg->cfo_en) {
    srslte_cfo_set_tol(&q->cfo, cfg->cfo_tol / (15000.0 * srslte_symbol_sz(q->cell.nof_prb)));
    srslte_cfo_correct(&q->cfo, q->out_buffer, q->out_buffer, cfg->cfo_value / srslte_symbol_sz(q->cell.nof_prb));
  }
}

static void apply_norm(srslte_ue_ul_t* q, srslte_ue_ul_cfg_t* cfg, float norm_factor)
{
  uint32_t sf_len               = SRSLTE_SF_LEN_PRB(q->cell.nof_prb);
  float*   buf                  = NULL;
  float    force_peak_amplitude = cfg->force_peak_amplitude > 0 ? cfg->force_peak_amplitude : 1.0f;

  switch (cfg->normalize_mode) {
    case SRSLTE_UE_UL_NORMALIZE_MODE_AUTO:
    default:
      // Automatic normalization (default)
      norm_factor = limit_norm_factor(q, norm_factor, q->out_buffer);
      srslte_vec_sc_prod_cfc(q->out_buffer, norm_factor, q->out_buffer, sf_len);
      break;
    case SRSLTE_UE_UL_NORMALIZE_MODE_FORCE_AMPLITUDE:
      // Force amplitude
      // Typecast buffer
      buf = (float*)q->out_buffer;

      // Get index of maximum absolute sample
      uint32_t idx = srslte_vec_max_abs_fi(buf, sf_len * 2);

      // Get maximum value
      float scale = fabsf(buf[idx]);

      // Avoid zero division
      if (scale != 0.0f && scale != INFINITY) {
        // Apply maximum peak amplitude
        srslte_vec_sc_prod_cfc(q->out_buffer, force_peak_amplitude / scale, q->out_buffer, sf_len);
      }
      break;
  }
}

static void add_srs(srslte_ue_ul_t* q, srslte_ue_ul_cfg_t* cfg, uint32_t tti)
{
  if (srs_tx_enabled(&cfg->ul_cfg.srs, tti)) {
    if (q->signals_pregenerated) {
      srslte_refsignal_srs_pregen_put(&q->signals, &q->pregen_srs, &cfg->ul_cfg.srs, tti, q->sf_symbols);
    } else {
      srslte_refsignal_srs_gen(&q->signals, &cfg->ul_cfg.srs, &cfg->ul_cfg.dmrs, tti % 10, q->srs_signal);
      srslte_refsignal_srs_put(&q->signals, &cfg->ul_cfg.srs, tti, q->srs_signal, q->sf_symbols);
    }
  }
}

static int pusch_encode(srslte_ue_ul_t* q, srslte_ul_sf_cfg_t* sf, srslte_ue_ul_cfg_t* cfg, srslte_pusch_data_t* data)
{

  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {

    srslte_vec_cf_zero(q->sf_symbols, SRSLTE_NOF_RE(q->cell));

    if (srslte_pusch_encode(&q->pusch, sf, &cfg->ul_cfg.pusch, data, q->sf_symbols)) {
      ERROR("Error encoding PUSCH\n");
      return SRSLTE_ERROR;
    }

    if (q->signals_pregenerated) {
      srslte_refsignal_dmrs_pusch_pregen_put(&q->signals, sf, &q->pregen_dmrs, &cfg->ul_cfg.pusch, q->sf_symbols);
    } else {
      if (srslte_refsignal_dmrs_pusch_gen(&q->signals,
                                          &cfg->ul_cfg.dmrs,
                                          cfg->ul_cfg.pusch.grant.L_prb,
                                          sf->tti % 10,
                                          cfg->ul_cfg.pusch.grant.n_dmrs,
                                          q->refsignal)) {
        ERROR("Error generating PUSCH DMRS signals\n");
        return ret;
      }
      srslte_refsignal_dmrs_pusch_put(&q->signals, &cfg->ul_cfg.pusch, q->refsignal, q->sf_symbols);
    }

    add_srs(q, cfg, sf->tti);

    srslte_ofdm_tx_sf(&q->fft);

    apply_cfo(q, cfg);
    apply_norm(q, cfg, q->cell.nof_prb / 15 / sqrtf(cfg->ul_cfg.pusch.grant.L_prb) / 2);

    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

/* Returns the transmission power for PUSCH for this subframe as defined in Section 5.1.1 of 36.213 */
float srslte_ue_ul_pusch_power(srslte_ue_ul_t* q, srslte_ue_ul_cfg_t* cfg, float PL, float p0_preamble)
{
  float p0_pusch, alpha;
  if (p0_preamble) {
    p0_pusch = p0_preamble + cfg->ul_cfg.power_ctrl.delta_preamble_msg3;
    alpha    = 1;
  } else {
    alpha    = cfg->ul_cfg.power_ctrl.alpha;
    p0_pusch = cfg->ul_cfg.power_ctrl.p0_nominal_pusch + cfg->ul_cfg.power_ctrl.p0_ue_pusch;
  }
  float delta = 0;
#ifdef kk
  if (ul_cfg->ul_cfg.power_ctrl.delta_mcs_based) {
    float beta_offset_pusch = 1;
    float MPR = q->pusch.cb_segm.K1 * q->pusch_cfg.cb_segm.C1 + q->pusch_cfg.cb_segm.K2 * q->pusch_cfg.cb_segm.C2;
    if (q->pusch.dci.cw.tbs == 0) {
      beta_offset_pusch = srslte_sch_beta_cqi(q->pusch.uci_offset.I_offset_cqi);
      MPR               = q->pusch_cfg.last_O_cqi;
    }
    MPR /= q->pusch.dci.nof_re;
    delta = 10 * log10f((powf(2, MPR * 1.25) - 1) * beta_offset_pusch);
  }
#else
  printf("Do this in pusch??");
#endif
  // TODO: This implements closed-loop power control
  float f = 0;

  float pusch_power = 10 * log10f(cfg->ul_cfg.pusch.grant.L_prb) + p0_pusch + alpha * PL + delta + f;
  DEBUG("PUSCH: P=%f -- 10M=%f, p0=%f,alpha=%f,PL=%f,\n",
        pusch_power,
        10 * log10f(cfg->ul_cfg.pusch.grant.L_prb),
        p0_pusch,
        alpha,
        PL);
  return SRSLTE_MIN(SRSLTE_PC_MAX, pusch_power);
}

/* Returns the transmission power for PUCCH for this subframe as defined in Section 5.1.2 of 36.213 */
float srslte_ue_ul_pucch_power(srslte_ue_ul_t* q, srslte_ue_ul_cfg_t* cfg, srslte_uci_cfg_t* uci_cfg, float PL)
{
  srslte_pucch_format_t format = cfg->ul_cfg.pucch.format;

  float p0_pucch = cfg->ul_cfg.power_ctrl.p0_nominal_pucch + cfg->ul_cfg.power_ctrl.p0_ue_pucch;

  uint8_t format_idx = (uint8_t)((format == 0) ? (0) : ((uint8_t)format - 1));

  float delta_f = cfg->ul_cfg.power_ctrl.delta_f_pucch[format_idx];

  float h;
  int   n_cqi  = srslte_cqi_size(&uci_cfg->cqi);
  int   n_harq = srslte_uci_cfg_total_ack(uci_cfg);

  if (format <= SRSLTE_PUCCH_FORMAT_1B) {
    h = 0;
  } else {
    if (SRSLTE_CP_ISNORM(q->cell.cp)) {
      if (n_cqi >= 4) {
        h = 10.0f * log10f(n_cqi / 4.0f);
      } else {
        h = 0;
      }
    } else {
      if (n_cqi + n_harq >= 4) {
        h = 10.0f * log10f((n_cqi + n_harq) / 4.0f);
      } else {
        h = 0;
      }
    }
  }

  // TODO: This implements closed-loop power control
  float g = 0;

  float pucch_power = p0_pucch + PL + h + delta_f + g;

  DEBUG("PUCCH: P=%f -- p0=%f, PL=%f, delta_f=%f, h=%f, g=%f\n", pucch_power, p0_pucch, PL, delta_f, h, g);

  return 0;
}

static int srs_encode(srslte_ue_ul_t* q, uint32_t tti, srslte_ue_ul_cfg_t* cfg)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q && cfg) {

    srslte_vec_cf_zero(q->sf_symbols, SRSLTE_NOF_RE(q->cell));

    add_srs(q, cfg, tti);

    srslte_ofdm_tx_sf(&q->fft);

    apply_cfo(q, cfg);
    apply_norm(q, cfg, (float)q->cell.nof_prb / 15 / sqrtf(srslte_refsignal_srs_M_sc(&q->signals, &cfg->ul_cfg.srs)));

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

static bool srs_tx_enabled(srslte_refsignal_srs_cfg_t* srs_cfg, uint32_t tti)
{
  if (srs_cfg->configured) {
    if (srslte_refsignal_srs_send_cs(srs_cfg->subframe_config, tti % 10) == 1 &&
        srslte_refsignal_srs_send_ue(srs_cfg->I_srs, tti) == 1) {
      return true;
    }
  }
  return false;
}

/* Returns the transmission power for SRS for this subframe as defined in Section 5.1.3 of 36.213 */
float srs_power(srslte_ue_ul_t* q, srslte_ue_ul_cfg_t* cfg, float PL)
{
  float alpha    = cfg->ul_cfg.power_ctrl.alpha;
  float p0_pusch = cfg->ul_cfg.power_ctrl.p0_nominal_pusch + cfg->ul_cfg.power_ctrl.p0_ue_pusch;

  // TODO: This implements closed-loop power control
  float f = 0;

  uint32_t M_sc = srslte_refsignal_srs_M_sc(&q->signals, &cfg->ul_cfg.srs);

  float p_srs_offset;
  if (cfg->ul_cfg.power_ctrl.delta_mcs_based) {
    p_srs_offset = -3 + cfg->ul_cfg.power_ctrl.p_srs_offset;
  } else {
    p_srs_offset = -10.5 + 1.5 * cfg->ul_cfg.power_ctrl.p_srs_offset;
  }

  float p_srs = p_srs_offset + 10 * log10f(M_sc) + p0_pusch + alpha * PL + f;

  DEBUG("SRS: P=%f -- p_offset=%f, 10M=%f, p0_pusch=%f, alpha=%f, PL=%f, f=%f\n",
        p_srs,
        p_srs_offset,
        10 * log10f(M_sc),
        p0_pusch,
        alpha,
        PL,
        f);

  return p_srs;
}

/* Procedure for determining PUCCH assignment 10.1 36.213 */
void srslte_ue_ul_pucch_resource_selection(const srslte_cell_t*      cell,
                                           srslte_pucch_cfg_t*       cfg,
                                           const srslte_uci_cfg_t*   uci_cfg,
                                           const srslte_uci_value_t* uci_value,
                                           uint8_t                   b[SRSLTE_UCI_MAX_ACK_BITS])
{
  // Get PUCCH Resources
  cfg->format  = srslte_pucch_proc_select_format(cell, cfg, uci_cfg, uci_value);
  cfg->n_pucch = srslte_pucch_proc_get_npucch(cell, cfg, uci_cfg, uci_value, b);
}

/* Choose PUCCH format as in Sec 10.1 of 36.213 and generate PUCCH signal
 */
static int
pucch_encode(srslte_ue_ul_t* q, srslte_ul_sf_cfg_t* sf, srslte_ue_ul_cfg_t* cfg, srslte_uci_value_t* uci_data)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && cfg != NULL) {
    srslte_uci_value_t uci_value2 = *uci_data; ///< Make copy of UCI Data, so the original input does not get altered
    ret                           = SRSLTE_ERROR;

    if (!srslte_pucch_cfg_isvalid(&cfg->ul_cfg.pucch, q->cell.nof_prb)) {
      ERROR("Invalid PUCCH configuration\n");
      return ret;
    }

    srslte_vec_cf_zero(q->sf_symbols, SRSLTE_NOF_RE(q->cell));

    // Prepare configuration
    srslte_ue_ul_pucch_resource_selection(
        &q->cell, &cfg->ul_cfg.pucch, &cfg->ul_cfg.pucch.uci_cfg, uci_data, uci_value2.ack.ack_value);

    srslte_refsignal_srs_pucch_shortened(&q->signals, sf, &cfg->ul_cfg.srs, &cfg->ul_cfg.pucch);

    if (srslte_pucch_encode(&q->pucch, sf, &cfg->ul_cfg.pucch, &uci_value2, q->sf_symbols)) {
      ERROR("Error encoding TB\n");
      return ret;
    }

    if (srslte_refsignal_dmrs_pucch_gen(&q->signals, sf, &cfg->ul_cfg.pucch, q->refsignal)) {
      ERROR("Error generating PUSCH DMRS signals\n");
      return ret;
    }
    srslte_refsignal_dmrs_pucch_put(&q->signals, &cfg->ul_cfg.pucch, q->refsignal, q->sf_symbols);

    add_srs(q, cfg, sf->tti);

    srslte_ofdm_tx_sf(&q->fft);

    apply_cfo(q, cfg);
    apply_norm(q, cfg, (float)q->cell.nof_prb / 15 / 10);

    char txt[256];
    srslte_pucch_tx_info(&cfg->ul_cfg.pucch, uci_data, txt, sizeof(txt));
    INFO("[PUCCH] Encoded %s\n", txt);

    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

/* Returns 1 if a SR needs to be sent at current_tti given I_sr, as defined in Section 10.1 of 36.213 */
int srslte_ue_ul_sr_send_tti(const srslte_pucch_cfg_t* cfg, uint32_t current_tti)
{
  if (!cfg->sr_configured) {
    return SRSLTE_SUCCESS;
  }
  uint32_t sr_periodicity;
  uint32_t sr_N_offset;
  uint32_t I_sr = cfg->I_sr;

  if (I_sr < 5) {
    sr_periodicity = 5;
    sr_N_offset    = I_sr;
  } else if (I_sr < 15) {
    sr_periodicity = 10;
    sr_N_offset    = I_sr - 5;
  } else if (I_sr < 35) {
    sr_periodicity = 20;
    sr_N_offset    = I_sr - 15;
  } else if (I_sr < 75) {
    sr_periodicity = 40;
    sr_N_offset    = I_sr - 35;
  } else if (I_sr < 155) {
    sr_periodicity = 80;
    sr_N_offset    = I_sr - 75;
  } else if (I_sr < 157) {
    sr_periodicity = 2;
    sr_N_offset    = I_sr - 155;
  } else if (I_sr == 157) {
    sr_periodicity = 1;
    sr_N_offset    = I_sr - 157;
  } else {
    return SRSLTE_ERROR;
  }
  if (current_tti >= sr_N_offset) {
    if ((current_tti - sr_N_offset) % sr_periodicity == 0) {
      return 1;
    }
  }
  return SRSLTE_SUCCESS;
}

bool srslte_ue_ul_gen_sr(srslte_ue_ul_cfg_t* cfg, srslte_ul_sf_cfg_t* sf, srslte_uci_data_t* uci_data, bool sr_request)
{
  uci_data->value.scheduling_request = false;
  if (srslte_ue_ul_sr_send_tti(&cfg->ul_cfg.pucch, sf->tti)) {
    if (sr_request) {
      // Get I_sr parameter
      uci_data->value.scheduling_request = true;
    }
    uci_data->cfg.is_scheduling_request_tti = true;
    return true;
  }
  return false;
}

#define uci_pending(cfg) (srslte_uci_cfg_total_ack(&cfg) > 0 || cfg.cqi.data_enable || cfg.cqi.ri_len > 0)

int srslte_ue_ul_encode(srslte_ue_ul_t* q, srslte_ul_sf_cfg_t* sf, srslte_ue_ul_cfg_t* cfg, srslte_pusch_data_t* data)
{
  int ret = SRSLTE_SUCCESS;

  /* Convert DTX to NACK in channel-selection mode (Release 10 only)*/
  if (cfg->ul_cfg.pucch.ack_nack_feedback_mode != SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_NORMAL) {
    uint32_t dtx_count = 0;
    for (uint32_t a = 0; a < srslte_uci_cfg_total_ack(&cfg->ul_cfg.pusch.uci_cfg); a++) {
      if (data->uci.ack.ack_value[a] == 2) {
        data->uci.ack.ack_value[a] = 0;
        dtx_count++;
      }
    }

    /* If all bits are DTX, do not transmit HARQ */
    if (dtx_count == srslte_uci_cfg_total_ack(&cfg->ul_cfg.pusch.uci_cfg)) {
      for (int i = 0; i < SRSLTE_MAX_CARRIERS; i++) {
        cfg->ul_cfg.pusch.uci_cfg.ack[i].nof_acks = 0;
      }
    }
  }

  if (cfg->grant_available) {
    ret = pusch_encode(q, sf, cfg, data) ? -1 : 1;
  } else if ((uci_pending(cfg->ul_cfg.pucch.uci_cfg) || data->uci.scheduling_request) &&
             cfg->cc_idx == 0) { // Send PUCCH over PCell only
    if (!cfg->ul_cfg.pucch.rnti) {
      cfg->ul_cfg.pucch.rnti = q->current_rnti;
    }
    ret = pucch_encode(q, sf, cfg, &data->uci) ? -1 : 1;
  } else if (srs_tx_enabled(&cfg->ul_cfg.srs, sf->tti)) {
    ret = srs_encode(q, sf->tti, cfg) ? -1 : 1;
  } else {
    // Set Zero output buffer if no UL transmission is required so the buffer does not keep previous transmission data
    if (q->cell.nof_prb) {
      srslte_vec_cf_zero(q->out_buffer, SRSLTE_SF_LEN_PRB(q->cell.nof_prb));
    }
  }

  return ret;
}

bool srslte_ue_ul_info(srslte_ue_ul_cfg_t* cfg,
                       srslte_ul_sf_cfg_t* sf,
                       srslte_uci_value_t* uci_data,
                       char*               str,
                       uint32_t            str_len)
{
  uint32_t n   = 0;
  bool     ret = false;

  if (cfg->grant_available) {
    n = snprintf(str, str_len, "PUSCH: cc=%d, tti_tx=%d, %s", cfg->cc_idx, sf->tti, sf->shortened ? "shortened, " : "");
    srslte_pusch_tx_info(&cfg->ul_cfg.pusch, uci_data, &str[n], str_len - n);
    ret = true;
  } else if ((uci_pending(cfg->ul_cfg.pucch.uci_cfg) || uci_data->scheduling_request) &&
             cfg->cc_idx == 0) { // Send PUCCH over PCell only
    n = sprintf(str, "PUCCH: cc=0, tti_tx=%d, %s", sf->tti, sf->shortened ? "shortened, " : "");
    srslte_pucch_tx_info(&cfg->ul_cfg.pucch, uci_data, &str[n], str_len - n);
    ret = true;
  } else if (srs_tx_enabled(&cfg->ul_cfg.srs, sf->tti)) {
    n   = sprintf(str, "SRS: tx_tti=%d", sf->tti);
    ret = true;
  }
  return ret;
}
