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

#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/srslte.h"

#include "srsenb/hdr/phy/cc_worker.h"

#define Error(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->debug(fmt, ##__VA_ARGS__)

using namespace std;

// Enable this to log SI
//#define LOG_THIS(a) 1

// Enable this one to skip SI-RNTI
#define LOG_THIS(rnti) (rnti != 0xFFFF)

/*********************************************/

using namespace asn1::rrc;

//#define DEBUG_WRITE_FILE

namespace srsenb {

cc_worker::cc_worker()
{
  reset();
}

cc_worker::~cc_worker()
{
  srslte_softbuffer_tx_free(&temp_mbsfn_softbuffer);
  srslte_enb_dl_free(&enb_dl);
  srslte_enb_ul_free(&enb_ul);

  for (int p = 0; p < SRSLTE_MAX_PORTS; p++) {
    if (signal_buffer_rx[p]) {
      free(signal_buffer_rx[p]);
    }
    if (signal_buffer_tx[p]) {
      free(signal_buffer_tx[p]);
    }
  }

  // Delete all users
  for (auto& it : ue_db) {
    delete it.second;
  }
}

#ifdef DEBUG_WRITE_FILE
FILE* f;
#endif

void cc_worker::init(phy_common* phy_, srslte::log* log_h_, uint32_t cc_idx_)
{
  phy                   = phy_;
  log_h                 = log_h_;
  cc_idx                = cc_idx_;
  srslte_cell_t cell    = phy_->get_cell(cc_idx);
  uint32_t      nof_prb = phy_->get_nof_prb(cc_idx);
  uint32_t      sf_len  = SRSLTE_SF_LEN_PRB(nof_prb);

  // Init cell here
  for (uint32_t p = 0; p < phy->get_nof_ports(cc_idx); p++) {
    signal_buffer_rx[p] = srslte_vec_cf_malloc(2 * sf_len);
    if (!signal_buffer_rx[p]) {
      ERROR("Error allocating memory\n");
      return;
    }
    srslte_vec_cf_zero(signal_buffer_rx[p], 2 * sf_len);
    signal_buffer_tx[p] = srslte_vec_cf_malloc(2 * sf_len);
    if (!signal_buffer_tx[p]) {
      ERROR("Error allocating memory\n");
      return;
    }
    srslte_vec_cf_zero(signal_buffer_tx[p], 2 * sf_len);
  }
  if (srslte_enb_dl_init(&enb_dl, signal_buffer_tx, nof_prb)) {
    ERROR("Error initiating ENB DL\n");
    return;
  }
  if (srslte_enb_dl_set_cell(&enb_dl, cell)) {
    ERROR("Error initiating ENB DL\n");
    return;
  }
  if (srslte_enb_ul_init(&enb_ul, signal_buffer_rx[0], nof_prb)) {
    ERROR("Error initiating ENB UL\n");
    return;
  }

  if (srslte_enb_ul_set_cell(&enb_ul, cell, &phy->dmrs_pusch_cfg)) {
    ERROR("Error initiating ENB UL\n");
    return;
  }

  /* Setup SI-RNTI in PHY */
  add_rnti(SRSLTE_SIRNTI, false, false);

  /* Setup P-RNTI in PHY */
  add_rnti(SRSLTE_PRNTI, false, false);

  /* Setup RA-RNTI in PHY */
  for (int i = SRSLTE_RARNTI_START; i <= SRSLTE_RARNTI_END; i++) {
    add_rnti(i, false, false);
  }

  if (srslte_softbuffer_tx_init(&temp_mbsfn_softbuffer, nof_prb)) {
    ERROR("Error initiating soft buffer\n");
    exit(-1);
  }

  srslte_softbuffer_tx_reset(&temp_mbsfn_softbuffer);

  Info("Component Carrier Worker %d configured cell %d PRB\n", cc_idx, nof_prb);

  if (phy->params.pusch_8bit_decoder) {
    enb_ul.pusch.llr_is_8bit        = true;
    enb_ul.pusch.ul_sch.llr_is_8bit = true;
  }
  initiated = true;

#ifdef DEBUG_WRITE_FILE
  f = fopen("test.dat", "w");
#endif
}

void cc_worker::reset()
{
  initiated = false;
  ue_db.clear();
}

cf_t* cc_worker::get_buffer_rx(uint32_t antenna_idx)
{
  return signal_buffer_rx[antenna_idx];
}

cf_t* cc_worker::get_buffer_tx(uint32_t antenna_idx)
{
  return signal_buffer_tx[antenna_idx];
}

void cc_worker::set_tti(uint32_t tti_)
{
  tti_rx    = tti_;
  tti_tx_dl = TTI_ADD(tti_rx, FDD_HARQ_DELAY_UL_MS);
  tti_tx_ul = TTI_RX_ACK(tti_rx);
}

int cc_worker::add_rnti(uint16_t rnti, bool is_pcell, bool is_temporal)
{

  if (not is_temporal) {
    if (srslte_enb_dl_add_rnti(&enb_dl, rnti)) {
      return -1;
    }
    if (srslte_enb_ul_add_rnti(&enb_ul, rnti)) {
      return -1;
    }
  }

  mutex.lock();
  // Create user unless already exists
  if (!ue_db.count(rnti)) {
    ue_db[rnti] = new ue(rnti, is_pcell);
  }
  mutex.unlock();

  return SRSLTE_SUCCESS;
}

void cc_worker::rem_rnti(uint16_t rnti)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (ue_db.count(rnti)) {

    delete ue_db[rnti];
    ue_db.erase(rnti);

    srslte_enb_dl_rem_rnti(&enb_dl, rnti);
    srslte_enb_ul_rem_rnti(&enb_ul, rnti);

  } else {
    Error("Removing user: rnti=0x%x does not exist\n", rnti);
  }
}

uint32_t cc_worker::get_nof_rnti()
{
  std::lock_guard<std::mutex> lock(mutex);
  return ue_db.size();
}

void cc_worker::work_ul(const srslte_ul_sf_cfg_t& ul_sf_cfg, stack_interface_phy_lte::ul_sched_t& ul_grants)
{
  std::lock_guard<std::mutex> lock(mutex);
  ul_sf = ul_sf_cfg;
  log_h->step(ul_sf.tti);

  for (auto& ue : ue_db) {
    ue.second->is_grant_available = false;
  }

  // Process UL signal
  srslte_enb_ul_fft(&enb_ul);

  // Decode pending UL grants for the tti they were scheduled
  decode_pusch(ul_grants.pusch, ul_grants.nof_grants);

  // Decode remaining PUCCH ACKs not associated with PUSCH transmission and SR signals
  decode_pucch();
}

void cc_worker::work_dl(const srslte_dl_sf_cfg_t&            dl_sf_cfg,
                        stack_interface_phy_lte::dl_sched_t& dl_grants,
                        stack_interface_phy_lte::ul_sched_t& ul_grants,
                        srslte_mbsfn_cfg_t*                  mbsfn_cfg)
{
  std::lock_guard<std::mutex> lock(mutex);
  dl_sf = dl_sf_cfg;

  // Put base signals (references, PBCH, PCFICH and PSS/SSS) into the resource grid
  srslte_enb_dl_put_base(&enb_dl, &dl_sf);

  // Put DL grants to resource grid. PDSCH data will be encoded as well.
  if (dl_sf_cfg.sf_type == SRSLTE_SF_NORM) {
    encode_pdcch_dl(dl_grants.pdsch, dl_grants.nof_grants);
    encode_pdsch(dl_grants.pdsch, dl_grants.nof_grants);
  } else {
    if (mbsfn_cfg->enable) {
      encode_pmch(dl_grants.pdsch, mbsfn_cfg);
    }
  }

  // Put UL grants to resource grid.
  encode_pdcch_ul(ul_grants.pusch, ul_grants.nof_grants);

  // Put pending PHICH HARQ ACK/NACK indications into subframe
  encode_phich(ul_grants.phich, ul_grants.nof_phich);

  // Generate signal and transmit
  srslte_enb_dl_gen_signal(&enb_dl);
}

int cc_worker::decode_pusch(stack_interface_phy_lte::ul_sched_grant_t* grants, uint32_t nof_pusch)
{
  srslte_pusch_res_t pusch_res;
  for (uint32_t i = 0; i < nof_pusch; i++) {
    // Get grant itself and RNTI
    auto&    ul_grant = grants[i];
    uint16_t rnti     = ul_grant.dci.rnti;

    if (rnti && ue_db.count(rnti)) {
      // Get UE configuration
      srslte_ul_cfg_t ul_cfg = phy->ue_db.get_ul_config(rnti, cc_idx);

      // mark this tti as having an ul dci to avoid pucch
      ue_db[rnti]->is_grant_available = true;

      // Fill UCI configuration
      phy->ue_db.fill_uci_cfg(tti_rx, cc_idx, rnti, grants->dci.cqi_request, true, ul_cfg.pusch.uci_cfg);

      // Compute UL grant
      srslte_pusch_grant_t& grant = ul_cfg.pusch.grant;
      if (srslte_ra_ul_dci_to_grant(&enb_ul.cell, &ul_sf, &ul_cfg.hopping, &ul_grant.dci, &grant)) {
        Error("Computing PUSCH dci\n");
        return SRSLTE_ERROR;
      }

      uint32_t ul_pid = TTI_RX(ul_sf.tti) % SRSLTE_FDD_NOF_HARQ;

      // Handle Format0 adaptive retx
      // Use last TBS for this TB in case of mcs>28
      if (ul_grant.dci.tb.mcs_idx > 28) {
        grant.tb = phy->ue_db.get_last_ul_tb(rnti, cc_idx, ul_pid);
        Info("RETX: mcs=%d, old_tbs=%d pid=%d\n", grants[i].dci.tb.mcs_idx, grant.tb.tbs, ul_pid);
      }
      phy->ue_db.set_last_ul_tb(rnti, cc_idx, ul_pid, grant.tb);

      // Run PUSCH decoder
      pusch_res                   = {};
      ul_cfg.pusch.softbuffers.rx = grants[i].softbuffer_rx;
      pusch_res.data              = grants[i].data;
      if (pusch_res.data) {
        if (srslte_enb_ul_get_pusch(&enb_ul, &ul_sf, &ul_cfg.pusch, &pusch_res)) {
          Error("Decoding PUSCH\n");
          return SRSLTE_ERROR;
        }
      }

      // Save PHICH scheduling for this user. Each user can have just 1 PUSCH dci per TTI
      ue_db[rnti]->phich_grant.n_prb_lowest = grant.n_prb_tilde[0];
      ue_db[rnti]->phich_grant.n_dmrs       = grants[i].dci.n_dmrs;

      float snr_db = enb_ul.chest_res.snr_db;

      // Notify MAC of RL status
      if (snr_db >= PUSCH_RL_SNR_DB_TH) {
        // Notify MAC UL channel quality
        phy->stack->snr_info(ul_sf.tti, rnti, cc_idx, snr_db);

        if (grants[i].dci.tb.rv == 0) {
          if (!pusch_res.crc) {
            Debug("PUSCH: Radio-Link failure snr=%.1f dB\n", snr_db);
            phy->stack->rl_failure(rnti);
          } else {
            phy->stack->rl_ok(rnti);

            // Notify MAC of Time Alignment only if it enabled and valid measurement, ignore value otherwise
            if (ul_cfg.pusch.meas_ta_en and not std::isnan(enb_ul.chest_res.ta_us) and
                not std::isinf(enb_ul.chest_res.ta_us)) {
              phy->stack->ta_info(ul_sf.tti, rnti, enb_ul.chest_res.ta_us);
            }
          }
        }
      }

      // Send UCI data to MAC
      phy->ue_db.send_uci_data(tti_rx, rnti, cc_idx, ul_cfg.pusch.uci_cfg, pusch_res.uci);

      // Notify MAC new received data and HARQ Indication value
      if (pusch_res.data) {
        phy->stack->crc_info(tti_rx, rnti, cc_idx, grant.tb.tbs / 8, pusch_res.crc);

        // Save metrics stats
        ue_db[rnti]->metrics_ul(grants[i].dci.tb.mcs_idx, 0, snr_db, pusch_res.avg_iterations_block);

        // Logging
        if (log_h->get_level() >= srslte::LOG_LEVEL_INFO) {
          char str[512];
          srslte_pusch_rx_info(&ul_cfg.pusch, &pusch_res, &enb_ul.chest_res, str, sizeof(str));
          log_h->info("PUSCH: cc=%d, %s\n", cc_idx, str);
        }
      }
    }
  }
  return SRSLTE_SUCCESS;
}

int cc_worker::decode_pucch()
{
  srslte_pucch_res_t pucch_res = {};

  for (auto& iter : ue_db) {
    uint16_t rnti = iter.first;

    // If it's a User RNTI and doesn't have PUSCH grant in this TTI
    if (SRSLTE_RNTI_ISUSER(rnti) && !ue_db[rnti]->is_grant_available && ue_db[rnti]->is_pcell()) {
      srslte_ul_cfg_t ul_cfg = phy->ue_db.get_ul_config(rnti, cc_idx);

      // Check if user needs to receive PUCCH
      if (phy->ue_db.fill_uci_cfg(tti_rx, cc_idx, rnti, false, false, ul_cfg.pucch.uci_cfg)) {
        // Decode PUCCH
        if (srslte_enb_ul_get_pucch(&enb_ul, &ul_sf, &ul_cfg.pucch, &pucch_res)) {
          ERROR("Error getting PUCCH\n");
          return SRSLTE_ERROR;
        }

        // Notify MAC of RL status (skip SR subframes)
        if (!ul_cfg.pucch.uci_cfg.is_scheduling_request_tti) {
          if (pucch_res.correlation < PUCCH_RL_CORR_TH) {
            Debug("PUCCH: Radio-Link failure corr=%.1f\n", pucch_res.correlation);
            phy->stack->rl_failure(rnti);
          } else {
            phy->stack->rl_ok(rnti);
          }
        }

        // Send UCI data to MAC
        phy->ue_db.send_uci_data(tti_rx, rnti, cc_idx, ul_cfg.pucch.uci_cfg, pucch_res.uci_data);

        // Logging
        if (log_h->get_level() >= srslte::LOG_LEVEL_INFO) {
          char str[512];
          srslte_pucch_rx_info(&ul_cfg.pucch, &pucch_res, str, sizeof(str));
          log_h->info("PUCCH: cc=%d; %s\n", cc_idx, str);
        }
      }
    }
  }
  return 0;
}

int cc_worker::encode_phich(stack_interface_phy_lte::ul_sched_ack_t* acks, uint32_t nof_acks)
{
  for (uint32_t i = 0; i < nof_acks; i++) {
    if (acks[i].rnti && ue_db.count(acks[i].rnti)) {
      srslte_enb_dl_put_phich(&enb_dl, &ue_db[acks[i].rnti]->phich_grant, acks[i].ack);

      Info("PHICH: rnti=0x%x, hi=%d, I_lowest=%d, n_dmrs=%d, tti_tx_dl=%d\n",
           acks[i].rnti,
           acks[i].ack,
           ue_db[acks[i].rnti]->phich_grant.n_prb_lowest,
           ue_db[acks[i].rnti]->phich_grant.n_dmrs,
           tti_tx_dl);
    }
  }
  return SRSLTE_SUCCESS;
}

int cc_worker::encode_pdcch_ul(stack_interface_phy_lte::ul_sched_grant_t* grants, uint32_t nof_grants)
{
  for (uint32_t i = 0; i < nof_grants; i++) {
    if (grants[i].needs_pdcch) {
      srslte_dci_cfg_t dci_cfg = phy->ue_db.get_dci_ul_config(grants[i].dci.rnti, cc_idx);
      if (srslte_enb_dl_put_pdcch_ul(&enb_dl, &dci_cfg, &grants[i].dci)) {
        ERROR("Error putting PUSCH %d\n", i);
        return SRSLTE_ERROR;
      }

      // Logging
      if (log_h->get_level() >= srslte::LOG_LEVEL_INFO) {
        char str[512];
        srslte_dci_ul_info(&grants[i].dci, str, 512);
        log_h->info("PDCCH: cc=%d, %s, tti_tx_dl=%d\n", cc_idx, str, tti_tx_dl);
      }
    }
  }
  return SRSLTE_SUCCESS;
}

int cc_worker::encode_pdcch_dl(stack_interface_phy_lte::dl_sched_grant_t* grants, uint32_t nof_grants)
{
  for (uint32_t i = 0; i < nof_grants; i++) {
    uint16_t rnti = grants[i].dci.rnti;
    if (rnti) {
      srslte_dci_cfg_t dci_cfg = phy->ue_db.get_dci_dl_config(grants[i].dci.rnti, cc_idx);
      if (srslte_enb_dl_put_pdcch_dl(&enb_dl, &dci_cfg, &grants[i].dci)) {
        ERROR("Error putting PDCCH %d\n", i);
        return SRSLTE_ERROR;
      }

      if (LOG_THIS(rnti) and log_h->get_level() >= srslte::LOG_LEVEL_INFO) {
        // Logging
        char str[512];
        srslte_dci_dl_info(&grants[i].dci, str, 512);
        log_h->info("PDCCH: cc=%d, %s, tti_tx_dl=%d\n", cc_idx, str, tti_tx_dl);
      }
    }
  }
  return 0;
}

int cc_worker::encode_pmch(stack_interface_phy_lte::dl_sched_grant_t* grant, srslte_mbsfn_cfg_t* mbsfn_cfg)
{
  srslte_pmch_cfg_t pmch_cfg;
  ZERO_OBJECT(pmch_cfg);
  srslte_configure_pmch(&pmch_cfg, &enb_dl.cell, mbsfn_cfg);
  srslte_ra_dl_compute_nof_re(&enb_dl.cell, &dl_sf, &pmch_cfg.pdsch_cfg.grant);

  // Set soft buffer
  pmch_cfg.pdsch_cfg.softbuffers.tx[0] = &temp_mbsfn_softbuffer;

  // Encode PMCH
  if (srslte_enb_dl_put_pmch(&enb_dl, &pmch_cfg, grant->data[0])) {
    Error("Error putting PMCH\n");
    return SRSLTE_ERROR;
  }

  // Logging
  if (log_h->get_level() >= srslte::LOG_LEVEL_INFO) {
    char str[512];
    srslte_pdsch_tx_info(&pmch_cfg.pdsch_cfg, str, 512);
    log_h->info("PMCH: %s\n", str);
  }

  // Save metrics stats
  ue_db[SRSLTE_MRNTI]->metrics_dl(mbsfn_cfg->mbsfn_mcs);
  return SRSLTE_SUCCESS;
}

int cc_worker::encode_pdsch(stack_interface_phy_lte::dl_sched_grant_t* grants, uint32_t nof_grants)
{

  /* Scales the Resources Elements affected by the power allocation (p_b) */
  // srslte_enb_dl_prepare_power_allocation(&enb_dl);
  for (uint32_t i = 0; i < nof_grants; i++) {
    uint16_t rnti = grants[i].dci.rnti;

    if (rnti && ue_db.count(rnti)) {
      srslte_dl_cfg_t dl_cfg = phy->ue_db.get_dl_config(rnti, cc_idx);

      // Compute DL grant
      if (srslte_ra_dl_dci_to_grant(
              &enb_dl.cell, &dl_sf, dl_cfg.tm, dl_cfg.pdsch.use_tbs_index_alt, &grants[i].dci, &dl_cfg.pdsch.grant)) {
        Error("Computing DL grant\n");
      }

      // Set soft buffer
      for (uint32_t j = 0; j < SRSLTE_MAX_CODEWORDS; j++) {
        dl_cfg.pdsch.softbuffers.tx[j] = grants[i].softbuffer_tx[j];
      }

      // Encode PDSCH
      if (srslte_enb_dl_put_pdsch(&enb_dl, &dl_cfg.pdsch, grants[i].data)) {
        Error("Error putting PDSCH %d\n", i);
        return SRSLTE_ERROR;
      }

      // Save pending ACK
      if (SRSLTE_RNTI_ISUSER(rnti)) {
        // Push whole DCI
        phy->ue_db.set_ack_pending(tti_tx_ul, cc_idx, grants[i].dci);
      }

      if (LOG_THIS(rnti) and log_h->get_level() >= srslte::LOG_LEVEL_INFO) {
        // Logging
        char str[512];
        srslte_pdsch_tx_info(&dl_cfg.pdsch, str, 512);
        log_h->info("PDSCH: cc=%d, %s, tti_tx_dl=%d\n", cc_idx, str, tti_tx_dl);
      }

      // Save metrics stats
      ue_db[rnti]->metrics_dl(grants[i].dci.tb[0].mcs_idx);
    } else {
      Error("User rnti=0x%x not found in cc_worker=%d\n", rnti, cc_idx);
    }
  }

  // srslte_enb_dl_apply_power_allocation(&enb_dl);

  return SRSLTE_SUCCESS;
}

/************ METRICS interface ********************/
uint32_t cc_worker::get_metrics(phy_metrics_t metrics[ENB_METRICS_MAX_USERS])
{
  std::lock_guard<std::mutex> lock(mutex);
  uint32_t                    cnt = 0;
  for (auto& ue : ue_db) {
    if ((SRSLTE_RNTI_ISUSER(ue.first) || ue.first == SRSLTE_MRNTI) && cnt < ENB_METRICS_MAX_USERS) {
      ue.second->metrics_read(&metrics[cnt]);
      cnt++;
    }
  }
  return cnt;
}

void cc_worker::ue::metrics_read(phy_metrics_t* metrics_)
{
  if (metrics_) {
    *metrics_ = metrics;
  }
  bzero(&metrics, sizeof(phy_metrics_t));
}

void cc_worker::ue::metrics_dl(uint32_t mcs)
{
  metrics.dl.mcs = SRSLTE_VEC_CMA(mcs, metrics.dl.mcs, metrics.dl.n_samples);
  metrics.dl.n_samples++;
}

void cc_worker::ue::metrics_ul(uint32_t mcs, float rssi, float sinr, float turbo_iters)
{
  metrics.ul.mcs         = SRSLTE_VEC_CMA((float)mcs, metrics.ul.mcs, metrics.ul.n_samples);
  metrics.ul.sinr        = SRSLTE_VEC_CMA((float)sinr, metrics.ul.sinr, metrics.ul.n_samples);
  metrics.ul.rssi        = SRSLTE_VEC_CMA((float)rssi, metrics.ul.rssi, metrics.ul.n_samples);
  metrics.ul.turbo_iters = SRSLTE_VEC_CMA((float)turbo_iters, metrics.ul.turbo_iters, metrics.ul.n_samples);
  metrics.ul.n_samples++;
}

int cc_worker::read_ce_abs(float* ce_abs)
{
  int sz = srslte_symbol_sz(phy->get_nof_prb(cc_idx));
  srslte_vec_f_zero(ce_abs, sz);
  int g = (sz - SRSLTE_NRE * phy->get_nof_prb(cc_idx)) / 2;
  srslte_vec_abs_dB_cf(enb_ul.chest_res.ce, -80.0f, &ce_abs[g], SRSLTE_NRE * phy->get_nof_prb(cc_idx));
  return sz;
}

int cc_worker::read_ce_arg(float* ce_arg)
{
  int sz = srslte_symbol_sz(phy->get_nof_prb(cc_idx));
  srslte_vec_f_zero(ce_arg, sz);
  int g = (sz - SRSLTE_NRE * phy->get_nof_prb(cc_idx)) / 2;
  srslte_vec_arg_deg_cf(enb_ul.chest_res.ce, -80.0f, &ce_arg[g], SRSLTE_NRE * phy->get_nof_prb(cc_idx));
  return sz;
}

int cc_worker::read_pusch_d(cf_t* pdsch_d)
{
  int nof_re = enb_ul.pusch.max_re;
  memcpy(pdsch_d, enb_ul.pusch.d, nof_re * sizeof(cf_t));
  return nof_re;
}

int cc_worker::read_pucch_d(cf_t* pdsch_d)
{
  int nof_re = SRSLTE_PUCCH_MAX_BITS / 2;
  memcpy(pdsch_d, enb_ul.pucch.z_tmp, nof_re * sizeof(cf_t));
  return nof_re;
}

} // namespace srsenb
