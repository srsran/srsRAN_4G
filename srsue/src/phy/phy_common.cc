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

#include <sstream>
#include <string.h>

#include "srsran/srsran.h"
#include "srsue/hdr/phy/phy_common.h"

#define Error(fmt, ...)                                                                                                \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.debug(fmt, ##__VA_ARGS__)

namespace srsue {

static srsran::rf_buffer_t zeros_multi(1);

phy_common::phy_common(srslog::basic_logger& logger) : logger(logger), ta(logger)
{
  reset();
}

phy_common::~phy_common() = default;

void phy_common::init(phy_args_t*                  _args,
                      srsran::radio_interface_phy* _radio,
                      stack_interface_phy_lte*     _stack,
                      rsrp_insync_itf*             _chest_loop)
{
  radio_h    = _radio;
  stack      = _stack;
  args       = _args;
  insync_itf = _chest_loop;
  sr.reset();

  // Instantiate UL channel emulator
  if (args->ul_channel_args.enable) {
    ul_channel = srsran::channel_ptr(
        new srsran::channel(args->ul_channel_args, args->nof_lte_carriers * args->nof_rx_ant, logger));
  }

  // Init the CFR config struct with the CFR args
  cfr_config.cfr_enable  = args->cfr_args.enable;
  cfr_config.cfr_mode    = args->cfr_args.mode;
  cfr_config.alpha       = args->cfr_args.strength;
  cfr_config.manual_thr  = args->cfr_args.manual_thres;
  cfr_config.max_papr_db = args->cfr_args.auto_target_papr;
  cfr_config.ema_alpha   = args->cfr_args.ema_alpha;
}

void phy_common::set_ue_dl_cfg(srsran_ue_dl_cfg_t* ue_dl_cfg)
{
  ue_dl_cfg->snr_to_cqi_offset = args->snr_to_cqi_offset;

  srsran_chest_dl_cfg_t* chest_cfg = &ue_dl_cfg->chest_cfg;

  // Setup estimator filter
  bzero(chest_cfg, sizeof(srsran_chest_dl_cfg_t));

  if (args->estimator_fil_auto) {
    chest_cfg->filter_coef[0] = 0;
  } else {
    chest_cfg->filter_coef[0] = args->estimator_fil_order;
    chest_cfg->filter_coef[1] = args->estimator_fil_stddev;
  }
  chest_cfg->filter_type = SRSRAN_CHEST_FILTER_GAUSS;

  if (args->snr_estim_alg == "refs") {
    chest_cfg->noise_alg = SRSRAN_NOISE_ALG_REFS;
  } else if (args->snr_estim_alg == "empty") {
    chest_cfg->noise_alg = SRSRAN_NOISE_ALG_EMPTY;
  } else {
    chest_cfg->noise_alg = SRSRAN_NOISE_ALG_PSS;
  }

  chest_cfg->rsrp_neighbour    = false;
  chest_cfg->sync_error_enable = args->correct_sync_error;
  chest_cfg->estimator_alg =
      args->interpolate_subframe_enabled ? SRSRAN_ESTIMATOR_ALG_INTERPOLATE : SRSRAN_ESTIMATOR_ALG_AVERAGE;
  chest_cfg->cfo_estimate_enable  = args->cfo_ref_mask != 0;
  chest_cfg->cfo_estimate_sf_mask = args->cfo_ref_mask;
}

void phy_common::set_pdsch_cfg(srsran_pdsch_cfg_t* pdsch_cfg)
{
  pdsch_cfg->csi_enable         = args->pdsch_csi_enabled;
  pdsch_cfg->max_nof_iterations = args->pdsch_max_its;
  pdsch_cfg->meas_evm_en        = args->meas_evm;
  pdsch_cfg->decoder_type       = (args->equalizer_mode == "zf") ? SRSRAN_MIMO_DECODER_ZF : SRSRAN_MIMO_DECODER_MMSE;
}

void phy_common::set_ue_ul_cfg(srsran_ue_ul_cfg_t* ue_ul_cfg)
{
  // Setup uplink configuration
  bzero(ue_ul_cfg, sizeof(srsran_ue_ul_cfg_t));
  ue_ul_cfg->cfo_en = true;
  if (args->force_ul_amplitude > 0.0f) {
    ue_ul_cfg->force_peak_amplitude = args->force_ul_amplitude;
    ue_ul_cfg->normalize_mode       = SRSRAN_UE_UL_NORMALIZE_MODE_FORCE_AMPLITUDE;
  } else {
    ue_ul_cfg->normalize_mode = SRSRAN_UE_UL_NORMALIZE_MODE_AUTO;
  }
  ue_ul_cfg->ul_cfg.pucch.ack_nack_feedback_mode = SRSRAN_PUCCH_ACK_NACK_FEEDBACK_MODE_NORMAL;
}

srsran::radio_interface_phy* phy_common::get_radio()
{
  return radio_h;
}

// Unpack RAR dci as defined in Section 6.2 of 36.213
void phy_common::set_rar_grant(uint8_t             grant_payload[SRSRAN_RAR_GRANT_LEN],
                               uint16_t            rnti,
                               srsran_tdd_config_t tdd_config)
{
#if MSG3_DELAY_MS < 0
#error "Error MSG3_DELAY_MS can't be negative"
#endif /* MSG3_DELAY_MS < 0 */

  if (rar_grant_tti < 0) {
    Error("Must call set_rar_grant_tti before set_rar_grant");
  }

  srsran_dci_ul_t        dci_ul;
  srsran_dci_rar_grant_t rar_grant;
  srsran_dci_rar_unpack(grant_payload, &rar_grant);

  if (srsran_dci_rar_to_ul_dci(&cell, &rar_grant, &dci_ul)) {
    Error("Converting RAR message to UL dci");
    return;
  }

  dci_ul.format = SRSRAN_DCI_FORMAT_RAR; // Use this format to identify a RAR grant
  dci_ul.rnti   = rnti;

  uint32_t msg3_tx_tti;
  if (rar_grant.ul_delay) {
    msg3_tx_tti = (TTI_TX(rar_grant_tti) + MSG3_DELAY_MS + 1) % 10240;
  } else {
    msg3_tx_tti = (TTI_TX(rar_grant_tti) + MSG3_DELAY_MS) % 10240;
  }

  if (cell.frame_type == SRSRAN_TDD) {
    while (srsran_sfidx_tdd_type(tdd_config, msg3_tx_tti % 10) != SRSRAN_TDD_SF_U) {
      msg3_tx_tti++;
    }
  }

  // Save Msg3 UL dci
  std::lock_guard<std::mutex> lock(pending_ul_grant_mutex);
  pending_ul_grant_t&         pending_grant = pending_ul_grant[0][msg3_tx_tti];
  if (!pending_grant.enable) {
    Debug("RAR grant rar_grant=%d, msg3_tti=%d, stored in index=%d", rar_grant_tti, msg3_tx_tti, TTIMOD(msg3_tx_tti));
    pending_grant.pid    = ul_pidof(msg3_tx_tti, &tdd_config);
    pending_grant.dci    = dci_ul;
    pending_grant.enable = true;
  } else {
    Warning("set_rar_grant: sf->tti=%d, cc=%d already in use", msg3_tx_tti, 0);
  }

  rar_grant_tti = -1;
}

// Table 8-2
const static uint32_t k_pusch[7][10] = {
    {4, 6, 0, 0, 0, 4, 6, 0, 0, 0},
    {0, 6, 0, 0, 4, 0, 6, 0, 0, 4},
    {0, 0, 0, 4, 0, 0, 0, 0, 4, 0},
    {4, 0, 0, 0, 0, 0, 0, 0, 4, 4},
    {0, 0, 0, 0, 0, 0, 0, 0, 4, 4},
    {0, 0, 0, 0, 0, 0, 0, 0, 4, 0},
    {7, 7, 0, 0, 0, 7, 7, 0, 0, 5},
};

const static uint32_t k_phich[7][10] = {{0, 0, 4, 7, 6, 0, 0, 4, 7, 6},
                                        {0, 0, 4, 6, 0, 0, 0, 4, 6, 0},
                                        {0, 0, 6, 0, 0, 0, 0, 6, 0, 0},
                                        {0, 0, 6, 6, 6, 0, 0, 0, 0, 0},
                                        {0, 0, 6, 6, 0, 0, 0, 0, 0, 0},
                                        {0, 0, 6, 0, 0, 0, 0, 0, 0, 0},
                                        {0, 0, 4, 6, 6, 0, 0, 4, 7, 0}};

uint32_t phy_common::ul_pidof(uint32_t tti, srsran_tdd_config_t* tdd_config)
{
  if (tdd_config->configured) {
    /* In TDD modes 1-5, each PID is associated with a unique subframe and the number of harq processes equals the
     * number of UL subframes Modes 0 and 6 have more processes than UL subframes and PID depends on sfn
     */
    uint32_t sf_idx = tti % 10;
    uint32_t sfn    = tti / 10;
    uint32_t cycle_idx;
    switch (tdd_config->sf_config) {
      case 0:
        cycle_idx = 7 - sfn % 7;
        if (sf_idx < 5) {
          return (cycle_idx + sf_idx - 2) % 7;
        } else {
          return (cycle_idx + sf_idx - 4) % 7;
        }
      case 1:
        if (sf_idx < 5) {
          return sf_idx - 2;
        } else {
          return sf_idx - 5;
        }
      case 2:
        if (sf_idx < 5) {
          return 0;
        } else {
          return 1;
        }
      case 3:
      case 4:
      case 5:
        return sf_idx - 2;
      case 6:
        cycle_idx = 6 - sfn % 6;
        if (sf_idx < 5) {
          return (cycle_idx + sf_idx - 2) % 6;
        } else {
          return (cycle_idx + sf_idx - 4) % 6;
        }
      default:
        Error("Invalid SF configuration %d", tdd_config->sf_config);
    }
  } else {
    return tti % SRSRAN_FDD_NOF_HARQ;
  }
  return 0;
}

// Computes SF->TTI at which PHICH will be received according to 9.1.2 of 36.213
#define tti_phich(sf)                                                                                                  \
  (sf->tti + (cell.frame_type == SRSRAN_FDD ? FDD_HARQ_DELAY_UL_MS : k_phich[sf->tdd_config.sf_config][sf->tti % 10]))

// Here SF->TTI is when PUSCH is transmitted
void phy_common::set_ul_pending_ack(srsran_ul_sf_cfg_t*  sf,
                                    uint32_t             cc_idx,
                                    srsran_phich_grant_t phich_grant,
                                    srsran_dci_ul_t*     dci_ul)
{
  // Use a lock here because subframe 4 and 9 of TDD config 0 accept multiple PHICH from multiple frames
  std::lock_guard<std::mutex> lock(pending_ul_ack_mutex);

  pending_ul_ack_t& pending_ack = pending_ul_ack[cc_idx][phich_grant.I_phich][tti_phich(sf)];

  if (!pending_ack.enable) {
    pending_ack.dci_ul      = *dci_ul;
    pending_ack.phich_grant = phich_grant;
    pending_ack.enable      = true;
    Debug("Set pending ACK for sf->tti=%d n_dmrs=%d, I_phich=%d, cc_idx=%d",
          sf->tti,
          phich_grant.n_dmrs,
          phich_grant.I_phich,
          cc_idx);
  } else {
    Warning("set_ul_pending_ack: sf->tti=%d, cc=%d already in use", sf->tti, cc_idx);
  }
}

// Here SF->TTI is when PHICH is being transmitted so that's DL subframe
bool phy_common::get_ul_pending_ack(srsran_dl_sf_cfg_t*   sf,
                                    uint32_t              cc_idx,
                                    srsran_phich_grant_t* phich_grant,
                                    srsran_dci_ul_t*      dci_ul)
{
  std::lock_guard<std::mutex> lock(pending_ul_ack_mutex);
  bool                        ret         = false;
  pending_ul_ack_t&           pending_ack = pending_ul_ack[cc_idx][phich_grant->I_phich][sf->tti];
  if (pending_ack.enable) {
    *phich_grant       = pending_ack.phich_grant;
    *dci_ul            = pending_ack.dci_ul;
    ret                = true;
    pending_ack.enable = false;
    Debug("Get pending ACK for sf->tti=%d n_dmrs=%d, I_phich=%d", sf->tti, phich_grant->n_dmrs, phich_grant->I_phich);
  }
  return ret;
}

bool phy_common::is_any_ul_pending_ack()
{
  std::lock_guard<std::mutex> lock(pending_ul_ack_mutex);

  for (const auto& i : pending_ul_ack) {
    for (const auto& j : i) {
      if (std::any_of(j.begin(), j.end(), [](const pending_ul_ack_t& ack) { return ack.enable; })) {
        return true;
      }
    }
  }

  return false;
}

// Computes SF->TTI at which PUSCH will be transmitted according to Section 8 of 36.213
#define tti_pusch_hi(sf)                                                                                               \
  (sf->tti +                                                                                                           \
   (cell.frame_type == SRSRAN_FDD ? FDD_HARQ_DELAY_UL_MS                                                               \
    : I_phich                     ? 7                                                                                  \
                                  : k_pusch[sf->tdd_config.sf_config][sf->tti % 10]) +                                                     \
   (FDD_HARQ_DELAY_DL_MS - FDD_HARQ_DELAY_UL_MS))
#define tti_pusch_gr(sf)                                                                                               \
  (sf->tti +                                                                                                           \
   (cell.frame_type == SRSRAN_FDD ? FDD_HARQ_DELAY_UL_MS                                                               \
    : dci->ul_idx == 1            ? 7                                                                                  \
                                  : k_pusch[sf->tdd_config.sf_config][sf->tti % 10]) +                                            \
   (FDD_HARQ_DELAY_DL_MS - FDD_HARQ_DELAY_UL_MS))

// SF->TTI is at which Format0 dci is received
void phy_common::set_ul_pending_grant(srsran_dl_sf_cfg_t* sf, uint32_t cc_idx, srsran_dci_ul_t* dci)
{
  std::lock_guard<std::mutex> lock(pending_ul_grant_mutex);

  // Calculate PID for this SF->TTI
  uint32_t            pid           = ul_pidof(tti_pusch_gr(sf), &sf->tdd_config);
  pending_ul_grant_t& pending_grant = pending_ul_grant[cc_idx][tti_pusch_gr(sf)];

  if (!pending_grant.enable) {
    pending_grant.pid    = pid;
    pending_grant.dci    = *dci;
    pending_grant.enable = true;
    Debug("Set ul pending grant for sf->tti=%d current_tti=%d, pid=%d", tti_pusch_gr(sf), sf->tti, pid);
  } else {
    Info("set_ul_pending_grant: sf->tti=%d, cc=%d already in use", sf->tti, cc_idx);
  }
}

// SF->TTI at which PUSCH should be transmitted
bool phy_common::get_ul_pending_grant(srsran_ul_sf_cfg_t* sf, uint32_t cc_idx, uint32_t* pid, srsran_dci_ul_t* dci)
{
  std::lock_guard<std::mutex> lock(pending_ul_grant_mutex);
  bool                        ret           = false;
  pending_ul_grant_t&         pending_grant = pending_ul_grant[cc_idx][sf->tti];

  if (pending_grant.enable) {
    Debug("Reading grant sf->tti=%d idx=%d", sf->tti, TTIMOD(sf->tti));
    if (pid) {
      *pid = pending_grant.pid;
    }
    if (dci) {
      *dci = pending_grant.dci;
    }
    pending_grant.enable = false;
    ret                  = true;
  }

  return ret;
}

uint32_t phy_common::get_ul_uci_cc(uint32_t tti_tx) const
{
  std::lock_guard<std::mutex> lock(pending_ul_grant_mutex);
  for (uint32_t cc = 0; cc < args->nof_lte_carriers; cc++) {
    const pending_ul_grant_t& grant = pending_ul_grant[cc][tti_tx];
    if (grant.enable) {
      return cc;
    }
  }
  return 0; // Return Primary cell
}

// SF->TTI at which PHICH is received
void phy_common::set_ul_received_ack(srsran_dl_sf_cfg_t* sf,
                                     uint32_t            cc_idx,
                                     bool                ack_value,
                                     uint32_t            I_phich,
                                     srsran_dci_ul_t*    dci_ul)
{
  std::lock_guard<std::mutex> lock(received_ul_ack_mutex);
  received_ul_ack_t&          received_ack = received_ul_ack[cc_idx][tti_pusch_hi(sf)];

  received_ack.hi_present = true;
  received_ack.hi_value   = ack_value;
  received_ack.dci_ul     = *dci_ul;
  Debug("Set ul received ack for sf->tti=%d, current_tti=%d", tti_pusch_hi(sf), sf->tti);
}

// SF->TTI at which PUSCH will be transmitted
bool phy_common::get_ul_received_ack(srsran_ul_sf_cfg_t* sf, uint32_t cc_idx, bool* ack_value, srsran_dci_ul_t* dci_ul)
{
  std::lock_guard<std::mutex> lock(received_ul_ack_mutex);
  bool                        ret          = false;
  received_ul_ack_t&          received_ack = received_ul_ack[cc_idx][sf->tti];

  if (received_ack.hi_present) {
    if (ack_value) {
      *ack_value = received_ack.hi_value;
    }
    if (dci_ul) {
      *dci_ul = received_ack.dci_ul;
    }
    Debug("Get ul received ack for current_tti=%d", sf->tti);
    received_ack.hi_present = false;
    ret                     = true;
  }

  return ret;
}

// SF->TTI at which PDSCH is decoded and ACK generated
void phy_common::set_dl_pending_ack(srsran_dl_sf_cfg_t*         sf,
                                    uint32_t                    cc_idx,
                                    uint8_t                     value[SRSRAN_MAX_CODEWORDS],
                                    srsran_pdsch_ack_resource_t resource)
{
  std::lock_guard<std::mutex> lock(pending_dl_ack_mutex);
  received_ack_t&             pending_ack = pending_dl_ack[cc_idx][sf->tti];

  if (!pending_ack.enable) {
    pending_ack.enable   = true;
    pending_ack.resource = resource;
    memcpy(pending_ack.value, value, SRSRAN_MAX_CODEWORDS * sizeof(uint8_t));
    Debug("Set dl pending ack for sf->tti=%d, value=%d, ncce=%d", sf->tti, value[0], resource.n_cce);
  } else {
    Warning("pending_dl_ack: sf->tti=%d, cc=%d already in use", sf->tti, cc_idx);
  }
}

void phy_common::set_rar_grant_tti(uint32_t tti)
{
  rar_grant_tti = tti;
}

void phy_common::set_dl_pending_grant(uint32_t               tti,
                                      uint32_t               cc_idx,
                                      uint32_t               grant_cc_idx,
                                      const srsran_dci_dl_t* dl_dci)
{
  std::lock_guard<std::mutex> lock(pending_dl_grant_mutex);
  if (!pending_dl_grant[tti % FDD_HARQ_DELAY_UL_MS][cc_idx].enable) {
    pending_dl_grant[tti % FDD_HARQ_DELAY_UL_MS][cc_idx].dl_dci       = *dl_dci;
    pending_dl_grant[tti % FDD_HARQ_DELAY_UL_MS][cc_idx].grant_cc_idx = grant_cc_idx;
    pending_dl_grant[tti % FDD_HARQ_DELAY_UL_MS][cc_idx].enable       = true;
  } else {
    Info("set_dl_pending_grant: cc=%d already exists", cc_idx);
  }
}

bool phy_common::get_dl_pending_grant(uint32_t tti, uint32_t cc_idx, uint32_t* grant_cc_idx, srsran_dci_dl_t* dl_dci)
{
  std::lock_guard<std::mutex> lock(pending_dl_grant_mutex);
  if (pending_dl_grant[tti % FDD_HARQ_DELAY_UL_MS][cc_idx].enable) {
    // Read grant
    if (dl_dci) {
      *dl_dci = pending_dl_grant[tti % FDD_HARQ_DELAY_UL_MS][cc_idx].dl_dci;
    }
    if (grant_cc_idx) {
      *grant_cc_idx = pending_dl_grant[tti % FDD_HARQ_DELAY_UL_MS][cc_idx].grant_cc_idx;
    }
    // Reset read flag
    pending_dl_grant[tti % FDD_HARQ_DELAY_UL_MS][cc_idx].enable = false;
    return true;
  } else {
    return false;
  }
}

typedef struct {
  uint32_t M;
  uint32_t K[9];
} das_index_t;

// Downlink association set index, Table 10.1-1 36.213
das_index_t das_table[7][10] = {
    {{0, {}}, {0, {}}, {1, {6}}, {0, {}}, {1, {4}}, {0, {}}, {0, {}}, {1, {6}}, {0, {}}, {1, {4}}},
    {{0, {}}, {0, {}}, {2, {7, 6}}, {1, {4}}, {0, {}}, {0, {}}, {0, {}}, {2, {7, 6}}, {1, {4}}, {0, {}}},
    {{0, {}}, {0, {}}, {4, {8, 7, 4, 6}}, {0, {}}, {0, {}}, {0, {}}, {0, {}}, {4, {8, 7, 4, 6}}, {0, {}}, {0, {}}},
    {{0, {}}, {0, {}}, {3, {7, 6, 11}}, {2, {6, 5}}, {2, {5, 4}}, {0, {}}, {0, {}}, {0, {}}, {0, {}}, {0, {}}},
    {{0, {}}, {0, {}}, {4, {12, 8, 7, 11}}, {4, {6, 5, 4, 7}}, {0, {}}, {0, {}}, {0, {}}, {0, {}}, {0, {}}, {0, {}}},
    {{0, {}},
     {0, {}},
     {9, {13, 12, 9, 8, 7, 5, 4, 11, 6}},
     {0, {}},
     {0, {}},
     {0, {}},
     {0, {}},
     {0, {}},
     {0, {}},
     {0, {}}},
    {{0, {}}, {0, {}}, {1, {7}}, {1, {7}}, {1, {5}}, {0, {}}, {0, {}}, {1, {7}}, {1, {7}}, {0, {}}}};

// SF->TTI at which ACK/NACK would be transmitted
bool phy_common::get_dl_pending_ack(srsran_ul_sf_cfg_t* sf, uint32_t cc_idx, srsran_pdsch_ack_cc_t* ack)
{
  std::lock_guard<std::mutex> lock(pending_dl_ack_mutex);
  bool                        ret = false;
  uint32_t                    M;
  if (cell.frame_type == SRSRAN_FDD) {
    M = 1;
  } else {
    M = das_table[sf->tdd_config.sf_config][sf->tti % 10].M;
  }
  for (uint32_t i = 0; i < M; i++) {
    uint32_t k =
        (cell.frame_type == SRSRAN_FDD) ? FDD_HARQ_DELAY_UL_MS : das_table[sf->tdd_config.sf_config][sf->tti % 10].K[i];
    uint32_t        pdsch_tti   = TTI_SUB(sf->tti, k + (FDD_HARQ_DELAY_DL_MS - FDD_HARQ_DELAY_UL_MS));
    received_ack_t& pending_ack = pending_dl_ack[cc_idx][pdsch_tti];
    if (pending_ack.enable) {
      ack->m[i].present  = true;
      ack->m[i].k        = k;
      ack->m[i].resource = pending_ack.resource;
      memcpy(ack->m[i].value, pending_ack.value, SRSRAN_MAX_CODEWORDS * sizeof(uint8_t));
      Debug("Get dl pending ack for sf->tti=%d, i=%d, k=%d, pdsch_tti=%d, value=%d, ncce=%d, v_dai=%d",
            sf->tti,
            i,
            k,
            pdsch_tti,
            ack->m[i].value[0],
            ack->m[i].resource.n_cce,
            ack->m[i].resource.v_dai_dl);
      ret = true;
    }
    pending_ack = {};
  }
  ack->M = ret ? M : 0;
  return ret;
}

/* The transmission of UL subframes must be in sequence. The correct sequence is guaranteed by a chain of N semaphores,
 * one per SF->TTI%max_workers. Each threads waits for the semaphore for the current thread and after transmission
 * allows next SF->TTI to be transmitted
 *
 * Each worker uses this function to indicate that all processing is done and data is ready for transmission or
 * there is no transmission at all (tx_enable). In that case, the end of burst message will be sent to the radio
 */
void phy_common::worker_end(const worker_context_t& w_ctx, const bool& tx_enable, srsran::rf_buffer_t& buffer)
{
  // Wait for the green light to transmit in the current TTI
  semaphore.wait(w_ctx.worker_ptr);

  // For each channel set or combine baseband
  if (tx_enable) {
    tx_buffer.set_combine(buffer);

    // Flag transmit enabled
    tx_enabled = true;
  }

  // If the current worker is not the last one, skip transmission
  if (not w_ctx.last) {
    if (tx_enable) {
      reset_last_worker();
    }

    // Release semaphore and let next worker to get in
    semaphore.release();

    // If this worker transmitted, hold the worker until last SF worker finishes
    if (tx_enable) {
      wait_last_worker();
    }

    return;
  }

  // Add current time alignment
  srsran::rf_timestamp_t tx_time = w_ctx.tx_time; // get transmit time from the last worker
  tx_time.sub((double)ta.get_sec());

  // Check if any worker had a transmission
  if (tx_enabled) {
    // Set number of samples to the latest transmit buffer
    tx_buffer.set_nof_samples(buffer.get_nof_samples());

    // Run uplink channel emulator
    if (ul_channel) {
      ul_channel->run(tx_buffer.to_cf_t(), tx_buffer.to_cf_t(), tx_buffer.get_nof_samples(), tx_time.get(0));
    }

    // Actual baseband transmission
    radio_h->tx(tx_buffer, tx_time);

  } else {
    if (radio_h->is_continuous_tx()) {
      if (is_pending_tx_end) {
        radio_h->tx_end();
        is_pending_tx_end = false;
      } else {
        if (!radio_h->get_is_start_of_burst()) {
          if (ul_channel) {
            srsran_vec_cf_zero(zeros_multi.get(0), buffer.get_nof_samples());
            ul_channel->run(zeros_multi.to_cf_t(), zeros_multi.to_cf_t(), buffer.get_nof_samples(), tx_time.get(0));
          }

          zeros_multi.set_nof_samples(buffer.get_nof_samples());
          radio_h->tx(zeros_multi, tx_time);
        }
      }
    } else {
      radio_h->tx_end();
    }
  }

  // Notify that last SF worker finished. Releases all the threads waiting.
  last_worker();

  // Reset tx buffer to prevent next SF uses previous data
  tx_enabled = false;
  for (uint32_t ch = 0; ch < SRSRAN_MAX_CHANNELS; ch++) {
    tx_buffer.set(ch, nullptr);
  }

  // Allow next TTI to transmit
  semaphore.release();
}

void phy_common::set_cell(const srsran_cell_t& c)
{
  cell = c;

  if (ul_channel) {
    ul_channel->set_srate((uint32_t)srsran_sampling_freq_hz(cell.nof_prb));
  }
}

void phy_common::update_cfo_measurement(uint32_t cc_idx, float cfo_hz)
{
  std::unique_lock<std::mutex> lock(meas_mutex);

  // Use SNR EMA coefficient for averaging
  avg_cfo_hz[cc_idx] = SRSRAN_VEC_SAFE_EMA(cfo_hz, avg_cfo_hz[cc_idx], args->snr_ema_coeff);
}

void phy_common::reset_measurements(uint32_t cc_idx)
{
  // If the CC index exceeds the maximum number of carriers, reset them all
  if (cc_idx >= SRSRAN_MAX_CARRIERS) {
    for (uint32_t cc = 0; cc < SRSRAN_MAX_CARRIERS; cc++) {
      reset_measurements(cc);
    }
  }

  // Default all metrics to NAN to prevent providing invalid information on traces and other layers
  std::unique_lock<std::mutex> lock(meas_mutex);
  pathloss[cc_idx]       = NAN;
  avg_rsrp[cc_idx]       = NAN;
  avg_rsrp_dbm[cc_idx]   = NAN;
  avg_rsrq_db[cc_idx]    = NAN;
  avg_rssi_dbm[cc_idx]   = NAN;
  avg_cfo_hz[cc_idx]     = NAN;
  avg_sinr_db[cc_idx]    = NAN;
  avg_snr_db[cc_idx]     = NAN;
  avg_noise[cc_idx]      = NAN;
  avg_rsrp_neigh[cc_idx] = NAN;
}

void phy_common::update_measurements(uint32_t                     cc_idx,
                                     const srsran_chest_dl_res_t& chest_res,
                                     srsran_dl_sf_cfg_t           sf_cfg_dl,
                                     float                        tx_crs_power,
                                     std::vector<phy_meas_t>&     serving_cells,
                                     cf_t*                        rssi_power_buffer)
{
  bool insync = true;
  {
    std::unique_lock<std::mutex> lock(meas_mutex);

    float snr_ema_coeff = args->snr_ema_coeff;

    // In TDD, ignore special subframes without PDSCH
    if (srsran_sfidx_tdd_type(sf_cfg_dl.tdd_config, sf_cfg_dl.tti % 10) == SRSRAN_TDD_SF_S &&
        srsran_sfidx_tdd_nof_dw(sf_cfg_dl.tdd_config) < 4) {
      return;
    }

    // Only worker 0 updates RX gain offset every 10 ms
    if (rssi_power_buffer) {
      if (!update_rxgain_cnt) {
        // Average RSSI over all symbols in antenna port 0 (make sure SF length is non-zero)
        float rssi_dbm = SRSRAN_SF_LEN_PRB(cell.nof_prb) > 0
                             ? (srsran_convert_power_to_dB(
                                    srsran_vec_avg_power_cf(rssi_power_buffer, SRSRAN_SF_LEN_PRB(cell.nof_prb))) +
                                30)
                             : 0;
        if (std::isnormal(rssi_dbm)) {
          avg_rssi_dbm[0] = SRSRAN_VEC_SAFE_EMA(rssi_dbm, avg_rssi_dbm[0], args->snr_ema_coeff);
        }

        rx_gain_offset = get_radio()->get_rx_gain() + args->rx_gain_offset;
      }
      update_rxgain_cnt++;
      if (update_rxgain_cnt >= update_rxgain_period) {
        update_rxgain_cnt = 0;
      }
    }

    // Average RSRQ over DEFAULT_MEAS_PERIOD_MS then sent to RRC
    float rsrq_db = chest_res.rsrq_db;
    if (std::isnormal(rsrq_db)) {
      // Reset average RSRQ measurement
      if (sf_cfg_dl.tti % pcell_report_period == 0) {
        avg_rsrq_db[cc_idx] = NAN;
      }
      avg_rsrq_db[cc_idx] = SRSRAN_VEC_SAFE_CMA(rsrq_db, avg_rsrq_db[cc_idx], sf_cfg_dl.tti % pcell_report_period);
    }

    // Average RSRP taken from CRS
    float rsrp_lin = chest_res.rsrp;
    if (std::isnormal(rsrp_lin)) {
      avg_rsrp[cc_idx] = SRSRAN_VEC_SAFE_EMA(rsrp_lin, avg_rsrp[cc_idx], snr_ema_coeff);
    }

    /* Correct absolute power measurements by RX gain offset */
    float rsrp_dbm = chest_res.rsrp_dbm - rx_gain_offset;

    // Serving cell RSRP measurements are averaged over DEFAULT_MEAS_PERIOD_MS then sent to RRC
    if (std::isnormal(rsrp_dbm)) {
      // Reset average RSRP measurement
      if (sf_cfg_dl.tti % pcell_report_period == 0) {
        avg_rsrp_dbm[cc_idx] = NAN;
      }
      avg_rsrp_dbm[cc_idx] = SRSRAN_VEC_SAFE_CMA(rsrp_dbm, avg_rsrp_dbm[cc_idx], sf_cfg_dl.tti % pcell_report_period);
    }

    // Compute PL
    pathloss[cc_idx] = tx_crs_power - avg_rsrp_dbm[cc_idx];

    // Average noise
    float cur_noise = chest_res.noise_estimate;
    if (std::isnormal(cur_noise)) {
      avg_noise[cc_idx] = SRSRAN_VEC_SAFE_EMA(cur_noise, avg_noise[cc_idx], snr_ema_coeff);
    }

    // Calculate SINR using CRS from neighbours if are detected
    float sinr_db = chest_res.snr_db;
    if (std::isnormal(avg_rsrp_neigh[cc_idx])) {
      cur_noise /= srsran_convert_dB_to_power(rx_gain_offset - 30);

      // Normalize the measured power ot the fraction of CRS pilots per PRB. Assume all neighbours have the same
      // number of ports and CP length
      uint32_t nof_re_x_prb = SRSRAN_NRE * (SRSRAN_CP_NSYMB(cell.cp));
      float    factor       = nof_re_x_prb / (srsran_refsignal_cs_nof_pilots_x_slot(cell.nof_ports));
      sinr_db = avg_rsrp_dbm[cc_idx] - srsran_convert_power_to_dB(avg_rsrp_neigh[cc_idx] / factor + cur_noise);
    }

    // Average sinr in the log domain
    if (std::isnormal(sinr_db)) {
      avg_sinr_db[cc_idx] = SRSRAN_VEC_SAFE_EMA(sinr_db, avg_sinr_db[cc_idx], snr_ema_coeff);
    }

    // Average snr in the log domain
    if (std::isnormal(chest_res.snr_db)) {
      avg_snr_db[cc_idx] = SRSRAN_VEC_SAFE_EMA(chest_res.snr_db, avg_snr_db[cc_idx], snr_ema_coeff);
    }

    // Store metrics
    ch_metrics_t ch = {};
    ch.n            = avg_noise[cc_idx];
    ch.rsrp         = avg_rsrp_dbm[cc_idx];
    ch.rsrq         = avg_rsrq_db[cc_idx];
    ch.rssi         = avg_rssi_dbm[cc_idx];
    ch.pathloss     = pathloss[cc_idx];
    ch.sinr         = avg_sinr_db[cc_idx];
    ch.sync_err     = chest_res.sync_error;

    set_ch_metrics(cc_idx, ch);

    // Prepare measurements for serving cells - skip if any measurement is invalid assuming pure zeros are not possible
    if (std::isnormal(avg_rsrp_dbm[cc_idx]) and
        std::isnormal(avg_cfo_hz[cc_idx] and ((sf_cfg_dl.tti % pcell_report_period) == pcell_report_period - 1))) {
      phy_meas_t meas = {};
      meas.rsrp       = avg_rsrp_dbm[cc_idx];
      meas.rsrq       = avg_rsrq_db[cc_idx];
      meas.cfo_hz     = avg_cfo_hz[cc_idx];

      // Save PCI and EARFCN (if available)
      if (cc_idx == 0) {
        meas.pci = cell.id;
      } else {
        meas.earfcn = cell_state.get_earfcn(cc_idx);
        meas.pci    = cell_state.get_pci(cc_idx);
      }
      serving_cells.push_back(meas);
    }

    // Check in-sync / out-sync conditions. Use SNR instead of SINR for RLF threshold
    if (cc_idx == 0) {
      if (avg_rsrp_dbm[0] > args->in_sync_rsrp_dbm_th && avg_snr_db[0] > args->in_sync_snr_db_th) {
        logger.debug("SNR=%.1f dB, RSRP=%.1f dBm sync=in-sync from channel estimator", avg_snr_db[0], avg_rsrp_dbm[0]);
      } else {
        logger.warning(
            "SNR=%.1f dB RSRP=%.1f dBm, sync=out-of-sync from channel estimator", avg_snr_db[0], avg_rsrp_dbm[0]);
        insync = false;
      }
    }
  }

  // Report in-sync status to the stack outside the mutex lock
  if (insync_itf && cc_idx == 0) {
    if (insync) {
      insync_itf->in_sync();
    } else {
      insync_itf->out_of_sync();
    }
  }

  // Call feedback loop for chest
  if (cc_idx == 0) {
    if (insync_itf && ((1U << (sf_cfg_dl.tti % 10U)) & args->cfo_ref_mask)) {
      insync_itf->set_cfo(chest_res.cfo);
    }
  }
}

void phy_common::set_dl_metrics(uint32_t cc_idx, const dl_metrics_t& m)
{
  std::unique_lock<std::mutex> lock(metrics_mutex);
  dl_metrics[cc_idx].set(m);
}

void phy_common::get_dl_metrics(dl_metrics_t::array_t& m)
{
  std::unique_lock<std::mutex> lock(metrics_mutex);

  for (uint32_t i = 0; i < args->nof_lte_carriers; i++) {
    m[i] = dl_metrics[i];
    dl_metrics[i].reset();
  }
}

void phy_common::set_ch_metrics(uint32_t cc_idx, const ch_metrics_t& m)
{
  std::unique_lock<std::mutex> lock(metrics_mutex);
  ch_metrics[cc_idx].set(m);
}

void phy_common::get_ch_metrics(ch_metrics_t::array_t& m)
{
  std::unique_lock<std::mutex> lock(metrics_mutex);

  for (uint32_t i = 0; i < args->nof_lte_carriers; i++) {
    m[i] = ch_metrics[i];
    ch_metrics[i].reset();
  }
}

void phy_common::set_ul_metrics(uint32_t cc_idx, const ul_metrics_t& m)
{
  std::unique_lock<std::mutex> lock(metrics_mutex);
  ul_metrics[cc_idx].set(m);
}

void phy_common::get_ul_metrics(ul_metrics_t::array_t& m)
{
  std::unique_lock<std::mutex> lock(metrics_mutex);

  for (uint32_t i = 0; i < args->nof_lte_carriers; i++) {
    m[i] = ul_metrics[i];
    ul_metrics[i].reset();
  }
}

void phy_common::set_sync_metrics(const uint32_t& cc_idx, const sync_metrics_t& m)
{
  std::unique_lock<std::mutex> lock(metrics_mutex);
  sync_metrics[cc_idx].set(m);
}

void phy_common::get_sync_metrics(sync_metrics_t::array_t& m)
{
  std::unique_lock<std::mutex> lock(metrics_mutex);

  for (uint32_t i = 0; i < args->nof_lte_carriers; i++) {
    m[i] = sync_metrics[i];
    sync_metrics[i].reset();
  }
}

void phy_common::reset_radio()
{
  // End Tx streams even if they are continuous
  // Since is_first_of_burst is set to true, the radio need to send
  // end of burst in order to stall correctly the Tx stream.
  // This is required for UHD version 3.10 and newer.
  is_pending_tx_end = true;
}

void phy_common::reset()
{
  reset_radio();

  sr.reset();
  {
    std::unique_lock<std::mutex> lock(meas_mutex);
    cur_pathloss    = 0;
    cur_pusch_power = 0;
  }
  last_ri = 0;

  // Reset all measurements
  reset_measurements(SRSRAN_MAX_CARRIERS);

  // Reset all SCell states
  cell_state.reset();

  // Note: Using memset to reset these members is forbidden because they are real objects, not plain arrays.
  {
    std::lock_guard<std::mutex> lock(pending_dl_ack_mutex);
    for (auto& i : pending_dl_ack) {
      i = {};
    }
  }
  for (auto& i : pending_dl_dai) {
    i = {};
  }
  {
    std::lock_guard<std::mutex> lock(pending_ul_ack_mutex);
    for (auto& i : pending_ul_ack) {
      for (auto& j : i) {
        j = {};
      }
    }
  }
  {
    std::lock_guard<std::mutex> lock(pending_ul_grant_mutex);
    for (auto& i : pending_ul_grant) {
      i = {};
    }
  }
}

/*  Convert 6-bit maps to 10-element subframe tables
    bitmap         = |0|0|0|0|0|0|
    subframe index = |1|2|3|6|7|8|
*/
void phy_common::build_mch_table()
{
  // First reset tables
  bzero(&mch_table[0], sizeof(uint8_t) * 40);

  // 40 element table represents 4 frames (40 subframes)
  if (mbsfn_config.mbsfn_subfr_cnfg.nof_alloc_subfrs == srsran::mbsfn_sf_cfg_t::sf_alloc_type_t::one_frame) {
    generate_mch_table(&mch_table[0], (uint32_t)mbsfn_config.mbsfn_subfr_cnfg.sf_alloc, 1u);
  } else if (mbsfn_config.mbsfn_subfr_cnfg.nof_alloc_subfrs == srsran::mbsfn_sf_cfg_t::sf_alloc_type_t::four_frames) {
    generate_mch_table(&mch_table[0], (uint32_t)mbsfn_config.mbsfn_subfr_cnfg.sf_alloc, 4u);
  } else {
    logger.error("The subframe config has not been set for MBSFN");
  }
  // Debug

  std::stringstream ss;
  ss << "|";
  for (uint32_t j = 0; j < 40; j++) {
    ss << (int)mch_table[j] << "|";
  }
  Info("MCH table: %s", ss.str().c_str());
}

void phy_common::build_mcch_table()
{
  // First reset tables
  bzero(&mcch_table[0], sizeof(uint8_t) * 10);
  generate_mcch_table(&mcch_table[0], (uint32_t)mbsfn_config.mbsfn_area_info.mcch_cfg.sf_alloc_info);
  // Debug
  std::stringstream ss;
  ss << "|";
  for (uint32_t j = 0; j < 10; j++) {
    ss << (int)mcch_table[j] << "|";
  }
  Info("MCCH table: %s", ss.str().c_str());
  sib13_configured = true;
}

void phy_common::set_mcch()
{
  mcch_configured = true;
}

void phy_common::set_mch_period_stop(uint32_t stop)
{
  std::lock_guard<std::mutex> lock(mtch_mutex);
  have_mtch_stop  = true;
  mch_period_stop = stop;
  mtch_cvar.notify_one();
}

uint32_t phy_common::get_ul_earfcn(uint32_t dl_earfcn)
{
  // Set default UL-EARFCN
  uint32_t ul_earfcn = srsran_band_ul_earfcn(dl_earfcn);

  // Try to find current DL-EARFCN in the map
  auto it = args->ul_earfcn_map.find(dl_earfcn);
  if (it != args->ul_earfcn_map.end()) {
    // If found UL EARFCN in the map, use it
    ul_earfcn = it->second;
  }

  return ul_earfcn;
}

bool phy_common::is_mch_subframe(srsran_mbsfn_cfg_t* cfg, uint32_t phy_tti)
{
  uint32_t sfn; // System Frame Number
  uint8_t  sf;  // Subframe
  uint8_t  offset;
  uint8_t  period;

  sfn = phy_tti / 10;
  sf  = phy_tti % 10;

  // Set some defaults
  cfg->mbsfn_area_id           = 0;
  cfg->non_mbsfn_region_length = 1;
  cfg->mbsfn_mcs               = 2;
  cfg->enable                  = false;
  cfg->is_mcch                 = false;

  // Check for MCCH
  if (is_mcch_subframe(cfg, phy_tti)) {
    cfg->is_mcch = true;
    return true;
  }

  // Not MCCH, check for MCH
  if (sib13_configured) {
    srsran::mbsfn_sf_cfg_t&    subfr_cnfg = mbsfn_config.mbsfn_subfr_cnfg;
    srsran::mbsfn_area_info_t& area_info  = mbsfn_config.mbsfn_area_info;
    offset                                = subfr_cnfg.radioframe_alloc_offset;
    period                                = srsran::enum_to_number(subfr_cnfg.radioframe_alloc_period);
    if (period == (uint8_t)-1) {
      return false;
    }

    if (subfr_cnfg.nof_alloc_subfrs == srsran::mbsfn_sf_cfg_t::sf_alloc_type_t::one_frame) {
      if ((sfn % period == offset) && (mch_table[sf] > 0)) {
        cfg->mbsfn_area_id           = area_info.mbsfn_area_id;
        cfg->non_mbsfn_region_length = enum_to_number(area_info.non_mbsfn_region_len);
        if (mcch_configured) {
          // Iterate through PMCH configs to see which one applies in the current frame
          srsran::mcch_msg_t& mcch = mbsfn_config.mcch;
          uint32_t            mbsfn_per_frame =
              mcch.pmch_info_list[0].sf_alloc_end / enum_to_number(mcch.pmch_info_list[0].mch_sched_period);
          uint32_t                     frame_alloc_idx = sfn % enum_to_number(mcch.common_sf_alloc_period);
          uint32_t                     sf_alloc_idx = frame_alloc_idx * mbsfn_per_frame + ((sf < 4) ? sf - 1 : sf - 3);
          std::unique_lock<std::mutex> lock(mtch_mutex);
          while (!have_mtch_stop) {
            mtch_cvar.wait(lock);
          }
          lock.unlock();

          for (uint32_t i = 0; i < mcch.nof_pmch_info; i++) {
            if (sf_alloc_idx <= mch_period_stop) {
              // trigger conditional variable, has ot be untriggered by mtch stop location
              cfg->mbsfn_mcs = mcch.pmch_info_list[i].data_mcs;
              cfg->enable    = true;
            } else {
              // have_mtch_stop = false;
            }
          }
          Debug("MCH subframe TTI:%d", phy_tti);
        }
        return true;
      }
    } else if (subfr_cnfg.nof_alloc_subfrs == srsran::mbsfn_sf_cfg_t::sf_alloc_type_t::four_frames) {
      uint8_t idx = sfn % period;
      if ((idx >= offset) && (idx < offset + 4)) {
        if (mch_table[(idx * 10) + sf] > 0) {
          cfg->mbsfn_area_id           = area_info.mbsfn_area_id;
          cfg->non_mbsfn_region_length = enum_to_number(area_info.non_mbsfn_region_len);
          // TODO: check for MCCH configuration, set MCS and decode
          return true;
        }
      }
    } else {
      logger.error("The subframe allocation type is not yet configured");
    }
  }

  return false;
}

bool phy_common::is_mcch_subframe(srsran_mbsfn_cfg_t* cfg, uint32_t phy_tti)
{
  uint32_t sfn; // System Frame Number
  uint8_t  sf;  // Subframe
  uint8_t  offset;
  uint16_t period;

  sfn = phy_tti / 10;
  sf  = (uint8_t)(phy_tti % 10);

  if (sib13_configured) {
    srsran::mbsfn_area_info_t& area_info = mbsfn_config.mbsfn_area_info;

    offset = area_info.mcch_cfg.mcch_offset;
    period = enum_to_number(area_info.mcch_cfg.mcch_repeat_period);

    if ((sfn % period == offset) && mcch_table[sf] > 0) {
      cfg->mbsfn_area_id           = area_info.mbsfn_area_id;
      cfg->non_mbsfn_region_length = enum_to_number(area_info.non_mbsfn_region_len);
      cfg->mbsfn_mcs               = enum_to_number(area_info.mcch_cfg.sig_mcs);
      cfg->enable                  = true;
      have_mtch_stop               = false;
      Debug("MCCH subframe TTI:%d", phy_tti);
      return true;
    }
  }
  return false;
}

bool phy_common::is_mbsfn_sf(srsran_mbsfn_cfg_t* cfg, uint32_t phy_tti)
{
  return is_mch_subframe(cfg, phy_tti);
}

} // namespace srsue
