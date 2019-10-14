/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srsenb/hdr/phy/sf_worker.h"

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

/* Define GUI-related things */
#ifdef ENABLE_GUI
#include "srsgui/srsgui.h"
#include <semaphore.h>

#include "srslte/srslte.h"

void       init_plots(srsenb::sf_worker* worker);
pthread_t  plot_thread;
sem_t      plot_sem;
static int plot_worker_id = -1;
#else
#pragma message "Compiling without srsGUI support"
#endif
/*********************************************/

using namespace asn1::rrc;

//#define DEBUG_WRITE_FILE

namespace srsenb {

sf_worker::sf_worker()
{
  phy = NULL;

  bzero(&enb_dl, sizeof(enb_dl));
  bzero(&enb_ul, sizeof(enb_ul));
  bzero(&tx_time, sizeof(tx_time));

  reset();
}

#ifdef DEBUG_WRITE_FILE
FILE* f;
#endif

void sf_worker::init(phy_common* phy_, srslte::log* log_h_)
{
  phy   = phy_;
  log_h = log_h_;

  pthread_mutex_init(&mutex, NULL);

  // Init cell here
  for (int p = 0; p < SRSLTE_MAX_PORTS; p++) {
    signal_buffer_rx[p] = (cf_t*)srslte_vec_malloc(2 * SRSLTE_SF_LEN_PRB(phy->cell.nof_prb) * sizeof(cf_t));
    if (!signal_buffer_rx[p]) {
      ERROR("Error allocating memory\n");
      return;
    }
    bzero(signal_buffer_rx[p], 2 * SRSLTE_SF_LEN_PRB(phy->cell.nof_prb) * sizeof(cf_t));
    signal_buffer_tx[p] = (cf_t*)srslte_vec_malloc(2 * SRSLTE_SF_LEN_PRB(phy->cell.nof_prb) * sizeof(cf_t));
    if (!signal_buffer_tx[p]) {
      ERROR("Error allocating memory\n");
      return;
    }
    bzero(signal_buffer_tx[p], 2 * SRSLTE_SF_LEN_PRB(phy->cell.nof_prb) * sizeof(cf_t));
  }
  if (srslte_enb_dl_init(&enb_dl, signal_buffer_tx, phy->cell.nof_prb)) {
    ERROR("Error initiating ENB DL\n");
    return;
  }
  if (srslte_enb_dl_set_cell(&enb_dl, phy->cell)) {
    ERROR("Error initiating ENB DL\n");
    return;
  }
  if (srslte_enb_ul_init(&enb_ul, signal_buffer_rx[0], phy->cell.nof_prb)) {
    ERROR("Error initiating ENB UL\n");
    return;
  }
  if (srslte_enb_ul_set_cell(&enb_ul, phy->cell, &phy->ul_cfg_com.dmrs)) {
    ERROR("Error initiating ENB UL\n");
    return;
  }

  /* Setup SI-RNTI in PHY */
  add_rnti(SRSLTE_SIRNTI, false);

  /* Setup P-RNTI in PHY */
  add_rnti(SRSLTE_PRNTI, false);

  /* Setup RA-RNTI in PHY */
  for (int i = 0; i < 10; i++) {
    add_rnti(1 + i, false);
  }

  if (srslte_softbuffer_tx_init(&temp_mbsfn_softbuffer, phy->cell.nof_prb)) {
    ERROR("Error initiating soft buffer\n");
    exit(-1);
  }

  srslte_softbuffer_tx_reset(&temp_mbsfn_softbuffer);

  Info("Worker %d configured cell %d PRB\n", get_id(), phy->cell.nof_prb);

  if (phy->params.pusch_8bit_decoder) {
    enb_ul.pusch.llr_is_8bit        = true;
    enb_ul.pusch.ul_sch.llr_is_8bit = true;
  }
  initiated = true;
  running   = true;

#ifdef DEBUG_WRITE_FILE
  f = fopen("test.dat", "w");
#endif
}

void sf_worker::stop()
{
  running = false;
  pthread_mutex_lock(&mutex);

  int cnt = 0;
  while (is_worker_running && cnt < 100) {
    usleep(1000);
    cnt++;
  }

  if (!is_worker_running) {
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
    for (std::map<uint16_t, ue*>::iterator it = ue_db.begin(); it != ue_db.end(); it++) {
      delete it->second;
    }
  } else {
    log_h->console("Warning could not stop properly PHY\n");
  }
  pthread_mutex_unlock(&mutex);
  pthread_mutex_destroy(&mutex);
}
void sf_worker::reset()
{
  initiated = false;
  ue_db.clear();
}

cf_t* sf_worker::get_buffer_rx(uint32_t antenna_idx)
{
  return signal_buffer_rx[antenna_idx];
}

void sf_worker::set_time(uint32_t tti_, uint32_t tx_worker_cnt_, srslte_timestamp_t tx_time_)
{
  tti_rx    = tti_;
  tti_tx_dl = TTI_TX(tti_rx);
  tti_tx_ul = TTI_RX_ACK(tti_rx);

  t_tx_dl = TTIMOD(tti_tx_dl);
  t_rx    = TTIMOD(tti_rx);
  t_tx_ul = TTIMOD(tti_tx_ul);

  tx_worker_cnt = tx_worker_cnt_;
  memcpy(&tx_time, &tx_time_, sizeof(srslte_timestamp_t));
}

int sf_worker::add_rnti(uint16_t rnti, bool is_temporal)
{

  if (!is_temporal) {
    if (srslte_enb_dl_add_rnti(&enb_dl, rnti)) {
      return -1;
    }
    if (srslte_enb_ul_add_rnti(&enb_ul, rnti)) {
      return -1;
    }
  }

  // Create user unless already exists
  pthread_mutex_lock(&mutex);
  if (!ue_db.count(rnti)) {
    ue_db[rnti] = new ue(rnti, phy);
  }
  pthread_mutex_unlock(&mutex);

  return SRSLTE_SUCCESS;
}

void sf_worker::rem_rnti(uint16_t rnti)
{
  pthread_mutex_lock(&mutex);
  if (ue_db.count(rnti)) {

    delete ue_db[rnti];
    ue_db.erase(rnti);

    srslte_enb_dl_rem_rnti(&enb_dl, rnti);
    srslte_enb_ul_rem_rnti(&enb_ul, rnti);

    // remove any pending dci for each subframe
    for (uint32_t i = 0; i < TTIMOD_SZ; i++) {
      for (uint32_t j = 0; j < phy->ul_grants[i].nof_grants; j++) {
        if (phy->ul_grants[i].pusch[j].dci.rnti == rnti) {
          phy->ul_grants[i].pusch[j].dci.rnti = 0;
        }
      }
      for (uint32_t j = 0; j < phy->dl_grants[i].nof_grants; j++) {
        if (phy->dl_grants[i].pdsch[j].dci.rnti == rnti) {
          phy->dl_grants[i].pdsch[j].dci.rnti = 0;
        }
      }
    }
  } else {
    Error("Removing user: rnti=0x%x does not exist\n", rnti);
  }
  pthread_mutex_unlock(&mutex);
}

uint32_t sf_worker::get_nof_rnti()
{
  return ue_db.size();
}

void sf_worker::set_config_dedicated(uint16_t rnti, asn1::rrc::phys_cfg_ded_s* dedicated)
{
  pthread_mutex_lock(&mutex);
  if (ue_db.count(rnti)) {

    if (dedicated->pusch_cfg_ded_present && dedicated->sched_request_cfg_present) {
      ue_db[rnti]->ul_cfg.pusch.uci_offset.I_offset_ack = dedicated->pusch_cfg_ded.beta_offset_ack_idx;
      ue_db[rnti]->ul_cfg.pusch.uci_offset.I_offset_cqi = dedicated->pusch_cfg_ded.beta_offset_cqi_idx;
      ue_db[rnti]->ul_cfg.pusch.uci_offset.I_offset_ri  = dedicated->pusch_cfg_ded.beta_offset_ri_idx;

      ue_db[rnti]->ul_cfg.pucch.n_pucch_2  = dedicated->cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx;
      ue_db[rnti]->ul_cfg.pucch.n_pucch_sr = dedicated->sched_request_cfg.setup().sr_pucch_res_idx;
    }

    if (dedicated->sched_request_cfg_present) {
      ue_db[rnti]->ul_cfg.pucch.I_sr          = dedicated->sched_request_cfg.setup().sr_cfg_idx;
      ue_db[rnti]->ul_cfg.pucch.sr_configured = true;
    }

    /* CQI Reporting */
    if (dedicated->cqi_report_cfg.cqi_report_periodic_present and
        dedicated->cqi_report_cfg.cqi_report_periodic.type() == setup_e::setup) {
      ue_db[rnti]->dl_cfg.cqi_report.periodic_configured = true;
      ue_db[rnti]->dl_cfg.cqi_report.pmi_idx = dedicated->cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx;
      ue_db[rnti]->ul_cfg.pucch.simul_cqi_ack =
          dedicated->cqi_report_cfg.cqi_report_periodic.setup().simul_ack_nack_and_cqi;
    } else {
      ue_db[rnti]->dl_cfg.cqi_report.pmi_idx = 0;
    }

    /* RI reporting */
    if (dedicated->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx_present) {
      ue_db[rnti]->dl_cfg.cqi_report.ri_idx         = dedicated->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx;
      ue_db[rnti]->dl_cfg.cqi_report.ri_idx_present = true;
    } else {
      ue_db[rnti]->dl_cfg.cqi_report.ri_idx_present = false;
    }

    if (dedicated->ant_info_present) {
      /* If default antenna info then follow 3GPP 36.331 clause 9.2.4 Default physical channel configuration */
      if (dedicated->ant_info.type() == phys_cfg_ded_s::ant_info_c_::types::default_value) {
        if (enb_dl.cell.nof_ports == 1) {
          ue_db[rnti]->dl_cfg.tm = SRSLTE_TM1;
        } else {
          ue_db[rnti]->dl_cfg.tm = SRSLTE_TM2;
        }
        ue_db[rnti]->dl_cfg.cqi_report.ri_idx         = 0;
        ue_db[rnti]->dl_cfg.cqi_report.ri_idx_present = false;
      } else {
        /* Physical channel reconfiguration according to 3GPP 36.331 clause 5.3.10.6 */
        switch (dedicated->ant_info.explicit_value().tx_mode) {
          case ant_info_ded_s::tx_mode_e_::tm1:
            ue_db[rnti]->dl_cfg.cqi_report.ri_idx         = 0;
            ue_db[rnti]->dl_cfg.cqi_report.ri_idx_present = false;
            ue_db[rnti]->dl_cfg.tm                        = SRSLTE_TM1;
            break;
          case ant_info_ded_s::tx_mode_e_::tm2:
            ue_db[rnti]->dl_cfg.cqi_report.ri_idx         = 0;
            ue_db[rnti]->dl_cfg.cqi_report.ri_idx_present = false;
            ue_db[rnti]->dl_cfg.tm                        = SRSLTE_TM2;
            break;
          case ant_info_ded_s::tx_mode_e_::tm3:
            ue_db[rnti]->dl_cfg.tm = SRSLTE_TM3;
            break;
          case ant_info_ded_s::tx_mode_e_::tm4:
            ue_db[rnti]->dl_cfg.tm = SRSLTE_TM4;
            break;
          default:
            ue_db[rnti]->dl_cfg.tm = SRSLTE_TM1;
            Error("TM mode %s not supported\n", dedicated->ant_info.explicit_value().tx_mode.to_string().c_str());
            break;
        }
      }
    }
  } else {
    Error("Setting config dedicated: rnti=0x%x does not exist\n", rnti);
  }
  pthread_mutex_unlock(&mutex);
}

void sf_worker::work_imp()
{
  if (!running) {
    return;
  }

  pthread_mutex_lock(&mutex);
  is_worker_running = true;

  srslte_mbsfn_cfg_t mbsfn_cfg;
  srslte_sf_t        sf_type = phy->is_mbsfn_sf(&mbsfn_cfg, tti_tx_dl) ? SRSLTE_SF_MBSFN : SRSLTE_SF_NORM;

  stack_interface_phy_lte::ul_sched_t* ul_grants = phy->ul_grants;
  stack_interface_phy_lte::dl_sched_t* dl_grants = phy->dl_grants;
  stack_interface_phy_lte*             stack     = phy->stack;

  log_h->step(tti_rx);

  Debug("Worker %d running\n", get_id());

  for (std::map<uint16_t, ue*>::iterator iter = ue_db.begin(); iter != ue_db.end(); ++iter) {
    uint16_t rnti                   = (uint16_t)iter->first;
    ue_db[rnti]->is_grant_available = false;
  }

  // Configure UL subframe
  ZERO_OBJECT(ul_sf);
  ul_sf.tti = tti_rx;

  // Process UL signal
  srslte_enb_ul_fft(&enb_ul);

  // Decode pending UL grants for the tti they were scheduled
  decode_pusch(ul_grants[t_rx].pusch, ul_grants[t_rx].nof_grants);

  // Decode remaining PUCCH ACKs not associated with PUSCH transmission and SR signals
  decode_pucch();

  // Get DL scheduling for the TX TTI from MAC

  if (sf_type == SRSLTE_SF_NORM) {
    if (stack->get_dl_sched(tti_tx_dl, &dl_grants[t_tx_dl]) < 0) {
      Error("Getting DL scheduling from MAC\n");
      goto unlock;
    }
  } else {
    dl_grants[t_tx_dl].cfi = mbsfn_cfg.non_mbsfn_region_length;
    if (stack->get_mch_sched(tti_tx_dl, mbsfn_cfg.is_mcch, &dl_grants[t_tx_dl])) {
      Error("Getting MCH packets from MAC\n");
      goto unlock;
    }
  }

  if (dl_grants[t_tx_dl].cfi < 1 || dl_grants[t_tx_dl].cfi > 3) {
    Error("Invalid CFI=%d\n", dl_grants[t_tx_dl].cfi);
    goto unlock;
  }

  // Get UL scheduling for the TX TTI from MAC
  if (stack->get_ul_sched(tti_tx_ul, &ul_grants[t_tx_ul]) < 0) {
    Error("Getting UL scheduling from MAC\n");
    goto unlock;
  }

  // Configure DL subframe
  ZERO_OBJECT(dl_sf);
  dl_sf.tti              = tti_tx_dl;
  dl_sf.cfi              = dl_grants[t_tx_dl].cfi;
  dl_sf.sf_type          = sf_type;
  dl_sf.non_mbsfn_region = mbsfn_cfg.non_mbsfn_region_length;

  // Put base signals (references, PBCH, PCFICH and PSS/SSS) into the resource grid
  srslte_enb_dl_put_base(&enb_dl, &dl_sf);

  // Put DL grants to resource grid. PDSCH data will be encoded as well.
  if (sf_type == SRSLTE_SF_NORM) {
    encode_pdcch_dl(dl_grants[t_tx_dl].pdsch, dl_grants[t_tx_dl].nof_grants);
    encode_pdsch(dl_grants[t_tx_dl].pdsch, dl_grants[t_tx_dl].nof_grants);
  } else {
    if (mbsfn_cfg.enable) {
      encode_pmch(&dl_grants[t_tx_dl].pdsch[0], &mbsfn_cfg);
    }
  }

  // Put UL grants to resource grid.
  encode_pdcch_ul(ul_grants[t_tx_ul].pusch, ul_grants[t_tx_ul].nof_grants);

  // Put pending PHICH HARQ ACK/NACK indications into subframe
  encode_phich(ul_grants[t_tx_ul].phich, ul_grants[t_tx_ul].nof_phich);

  // Generate signal and transmit
  srslte_enb_dl_gen_signal(&enb_dl);

  pthread_mutex_unlock(&mutex);

  Debug("Sending to radio\n");
  phy->worker_end(tx_worker_cnt, signal_buffer_tx, SRSLTE_SF_LEN_PRB(phy->cell.nof_prb), tx_time);

  is_worker_running = false;

#ifdef DEBUG_WRITE_FILE
  fwrite(signal_buffer_tx, SRSLTE_SF_LEN_PRB(phy->cell.nof_prb) * sizeof(cf_t), 1, f);
#endif

#ifdef DEBUG_WRITE_FILE
  if (tti_tx_dl == 10) {
    fclose(f);
    exit(-1);
  }
#endif

  /* Tell the plotting thread to draw the plots */
#ifdef ENABLE_GUI
  if ((int)get_id() == plot_worker_id) {
    sem_post(&plot_sem);
  }
#endif

unlock:
  if (is_worker_running) {
    is_worker_running = false;
    pthread_mutex_unlock(&mutex);
  }
}

bool sf_worker::fill_uci_cfg(uint16_t rnti, bool aperiodic_cqi_request, srslte_uci_cfg_t* uci_cfg)
{
  bool uci_required = false;

  bzero(uci_cfg, sizeof(srslte_uci_cfg_t));

  // Check if SR opportunity (will only be used in PUCCH)
  uci_cfg->is_scheduling_request_tti = (srslte_ue_ul_sr_send_tti(&ue_db[rnti]->ul_cfg.pucch, tti_rx) == 1);

  uci_required |= uci_cfg->is_scheduling_request_tti;

  // Get pending ACKs with an associated PUSCH transmission
  // TODO: Use ue_dl procedures to compute uci_ack_cfg for TDD and CA
  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
    uci_cfg->ack[0].pending_tb[tb] = phy->ue_db_is_ack_pending(tti_rx, rnti, tb, &uci_cfg->ack[0].ncce[0]);
    Debug("ACK: is pending tti=%d, mod=%d, value=%d\n", tti_rx, TTIMOD(tti_rx), uci_cfg->ack[0].pending_tb[tb]);
    if (uci_cfg->ack[0].pending_tb[tb]) {
      uci_cfg->ack[0].nof_acks++;
      uci_required = true;
    }
  }

  // Get pending CQI reports for this TTI
  if (srslte_enb_dl_gen_cqi_periodic(
          &enb_dl.cell, &ue_db[rnti]->dl_cfg, tti_rx, phy->ue_db_get_ri(rnti), &uci_cfg->cqi)) {
    uci_required = true;
  } else if (aperiodic_cqi_request) {
    srslte_enb_dl_gen_cqi_aperiodic(&enb_dl.cell, &ue_db[rnti]->dl_cfg, phy->ue_db_get_ri(rnti), &uci_cfg->cqi);
    uci_required = true;
  }

  return uci_required;
}

void sf_worker::send_uci_data(uint16_t rnti, srslte_uci_cfg_t* uci_cfg, srslte_uci_value_t* uci_value)
{
  // Notify SR
  if (uci_cfg->is_scheduling_request_tti && uci_value->scheduling_request) {
    phy->stack->sr_detected(tti_rx, rnti);
  }

  /* If only one ACK is required, it can be for TB0 or TB1 */
  uint32_t ack_idx = 0;
  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
    if (uci_cfg->ack[0].pending_tb[tb]) {
      bool ack   = uci_value->ack.ack_value[ack_idx];
      bool valid = uci_value->ack.valid;
      phy->stack->ack_info(tti_rx, rnti, tb, ack && valid);
      ack_idx++;
    }
  }

  // Notify CQI only if CRC is valid
  if (uci_value->cqi.data_crc) {
    if (uci_cfg->cqi.data_enable) {
      uint8_t cqi_value = 0;
      switch (uci_cfg->cqi.type) {
        case SRSLTE_CQI_TYPE_WIDEBAND:
          cqi_value = uci_value->cqi.wideband.wideband_cqi;
          break;
        case SRSLTE_CQI_TYPE_SUBBAND:
          cqi_value = uci_value->cqi.subband.subband_cqi;
          break;
        case SRSLTE_CQI_TYPE_SUBBAND_HL:
          cqi_value = uci_value->cqi.subband_hl.wideband_cqi_cw0;
          break;
        case SRSLTE_CQI_TYPE_SUBBAND_UE:
          cqi_value = uci_value->cqi.subband_ue.wideband_cqi;
          break;
      }
      phy->stack->cqi_info(tti_rx, rnti, cqi_value);
    }
    if (uci_cfg->cqi.ri_len) {
      phy->stack->ri_info(tti_rx, rnti, uci_value->ri);
      phy->ue_db_set_ri(rnti, uci_value->ri);
    }
    if (uci_cfg->cqi.pmi_present) {
      uint8_t pmi_value = 0;
      switch (uci_cfg->cqi.type) {
        case SRSLTE_CQI_TYPE_WIDEBAND:
          pmi_value = uci_value->cqi.wideband.pmi;
          break;
        case SRSLTE_CQI_TYPE_SUBBAND_HL:
          pmi_value = uci_value->cqi.subband_hl.pmi;
          break;
        default:
          Error("CQI type=%d not implemented for PMI\n", uci_cfg->cqi.type);
          break;
      }
      phy->stack->pmi_info(tti_rx, rnti, pmi_value);
    }
  }
}

int sf_worker::decode_pusch(stack_interface_phy_lte::ul_sched_grant_t* grants, uint32_t nof_pusch)
{
  srslte_pusch_res_t pusch_res;

  for (uint32_t i = 0; i < nof_pusch; i++) {
    uint16_t rnti = grants[i].dci.rnti;
    if (rnti) {
      // mark this tti as having an ul dci to avoid pucch
      ue_db[rnti]->is_grant_available = true;

      fill_uci_cfg(rnti, grants->dci.cqi_request, &ue_db[rnti]->ul_cfg.pusch.uci_cfg);

      // Compute UL grant
      srslte_pusch_grant_t* grant = &ue_db[rnti]->ul_cfg.pusch.grant;
      if (srslte_ra_ul_dci_to_grant(&phy->cell, &ul_sf, &ue_db[rnti]->ul_cfg.hopping, &grants[i].dci, grant)) {
        Error("Computing PUSCH dci\n");
        return SRSLTE_ERROR;
      }

      uint32_t ul_pid = TTI_RX(tti_rx) % SRSLTE_FDD_NOF_HARQ;

      // Handle Format0 adaptive retx
      // Use last TBS for this TB in case of mcs>28
      if (grants[i].dci.tb.mcs_idx > 28) {
        grant->tb = phy->ue_db_get_last_ul_tb(rnti, ul_pid);
        Info("RETX: mcs=%d, old_tbs=%d pid=%d\n", grants[i].dci.tb.mcs_idx, grant->tb.tbs, ul_pid);
      }
      phy->ue_db_set_last_ul_tb(rnti, ul_pid, grant->tb);

      // Run PUSCH decoder
      ue_db[rnti]->ul_cfg.pusch.softbuffers.rx = grants[i].softbuffer_rx;
      pusch_res.data                           = grants[i].data;
      if (srslte_enb_ul_get_pusch(&enb_ul, &ul_sf, &ue_db[rnti]->ul_cfg.pusch, &pusch_res)) {
        Error("Decoding PUSCH\n");
        return SRSLTE_ERROR;
      }

      // Save PHICH scheduling for this user. Each user can have just 1 PUSCH dci per TTI
      ue_db[rnti]->phich_grant.n_prb_lowest = grant->n_prb_tilde[0];
      ue_db[rnti]->phich_grant.n_dmrs       = grants[i].dci.n_dmrs;

      float snr_db = enb_ul.chest_res.snr_db;

      // Notify MAC of RL status
      if (snr_db >= PUSCH_RL_SNR_DB_TH) {
        phy->stack->snr_info(tti_rx, rnti, snr_db);

        if (grants[i].dci.tb.rv == 0) {
          if (!pusch_res.crc) {
            Debug("PUSCH: Radio-Link failure snr=%.1f dB\n", snr_db);
            phy->stack->rl_failure(rnti);
          } else {
            phy->stack->rl_ok(rnti);
          }
        }
      }

      // Notify MAC new received data and HARQ Indication value
      phy->stack->crc_info(tti_rx, rnti, grant->tb.tbs / 8, pusch_res.crc);

      // Send UCI data to MAC
      send_uci_data(rnti, &ue_db[rnti]->ul_cfg.pusch.uci_cfg, &pusch_res.uci);

      // Save metrics stats
      ue_db[rnti]->metrics_ul(grants[i].dci.tb.mcs_idx, 0, snr_db, pusch_res.avg_iterations_block);

      // Logging
      char str[512];
      srslte_pusch_rx_info(&ue_db[rnti]->ul_cfg.pusch, &pusch_res, str, 512);
      Info("PUSCH: %s, snr=%.1f dB\n", str, snr_db);
    }
  }
  return SRSLTE_SUCCESS;
}

int sf_worker::decode_pucch()
{
  srslte_pucch_res_t pucch_res;
  ZERO_OBJECT(pucch_res);

  for (std::map<uint16_t, ue*>::iterator iter = ue_db.begin(); iter != ue_db.end(); ++iter) {
    uint16_t rnti = (uint16_t)iter->first;

    // If it's a User RNTI and doesn't have PUSCH grant in this TTI
    if (SRSLTE_RNTI_ISUSER(rnti) && !ue_db[rnti]->is_grant_available) {
      // Check if user needs to receive PUCCH
      if (fill_uci_cfg(rnti, false, &ue_db[rnti]->ul_cfg.pucch.uci_cfg)) {
        // Decode PUCCH
        if (srslte_enb_ul_get_pucch(&enb_ul, &ul_sf, &ue_db[rnti]->ul_cfg.pucch, &pucch_res)) {
          ERROR("Error getting PUCCH\n");
          return SRSLTE_ERROR;
        }

        // Notify MAC of RL status (skip SR subframes)
        if (!ue_db[rnti]->ul_cfg.pucch.uci_cfg.is_scheduling_request_tti) {
          if (pucch_res.correlation < PUCCH_RL_CORR_TH) {
            Debug("PUCCH: Radio-Link failure corr=%.1f\n", pucch_res.correlation);
            phy->stack->rl_failure(rnti);
          } else {
            phy->stack->rl_ok(rnti);
          }
        }

        // Send UCI data to MAC
        send_uci_data(rnti, &ue_db[rnti]->ul_cfg.pucch.uci_cfg, &pucch_res.uci_data);

        // Logging
        char str[512];
        srslte_pucch_rx_info(&ue_db[rnti]->ul_cfg.pucch, &pucch_res.uci_data, str, 512);
        Info("PUCCH: %s, corr=%.1f\n", str, pucch_res.correlation);
      }
    }
  }
  return 0;
}

int sf_worker::encode_phich(stack_interface_phy_lte::ul_sched_ack_t* acks, uint32_t nof_acks)
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

int sf_worker::encode_pdcch_ul(stack_interface_phy_lte::ul_sched_grant_t* grants, uint32_t nof_grants)
{
  for (uint32_t i = 0; i < nof_grants; i++) {
    if (grants[i].needs_pdcch) {
      if (srslte_enb_dl_put_pdcch_ul(&enb_dl, &grants[i].dci_cfg, &grants[i].dci)) {
        ERROR("Error putting PUSCH %d\n", i);
        return SRSLTE_ERROR;
      }

      // Logging
      char str[512];
      srslte_dci_ul_info(&grants[i].dci, str, 512);
      Info("PDCCH: %s, tti_tx_dl=%d\n", str, tti_tx_dl);
    }
  }
  return SRSLTE_SUCCESS;
}

int sf_worker::encode_pdcch_dl(stack_interface_phy_lte::dl_sched_grant_t* grants, uint32_t nof_grants)
{
  for (uint32_t i = 0; i < nof_grants; i++) {
    uint16_t rnti = grants[i].dci.rnti;
    if (rnti) {
      if (srslte_enb_dl_put_pdcch_dl(&enb_dl, &grants[i].dci_cfg, &grants[i].dci)) {
        ERROR("Error putting PDCCH %d\n", i);
        return SRSLTE_ERROR;
      }

      if (LOG_THIS(rnti)) {
        // Logging
        char str[512];
        srslte_dci_dl_info(&grants[i].dci, str, 512);
        Info("PDCCH: %s, tti_tx_dl=%d\n", str, tti_tx_dl);
      }
    }
  }
  return 0;
}

int sf_worker::encode_pmch(stack_interface_phy_lte::dl_sched_grant_t* grant, srslte_mbsfn_cfg_t* mbsfn_cfg)
{
  srslte_pmch_cfg_t      pmch_cfg;
  ZERO_OBJECT(pmch_cfg);
  srslte_configure_pmch(&pmch_cfg, &phy->cell, mbsfn_cfg);
  srslte_ra_dl_compute_nof_re(&phy->cell, &dl_sf, &pmch_cfg.pdsch_cfg.grant);

  // Set soft buffer
  pmch_cfg.pdsch_cfg.softbuffers.tx[0] = &temp_mbsfn_softbuffer;

  // Encode PMCH
  if (srslte_enb_dl_put_pmch(&enb_dl, &pmch_cfg, grant->data[0])) {
    Error("Error putting PMCH\n");
    return SRSLTE_ERROR;
  }

  // Logging
  char str[512];
  srslte_pdsch_tx_info(&pmch_cfg.pdsch_cfg, str, 512);
  Info("pmch: %s\n", str);

  return SRSLTE_SUCCESS;
}

int sf_worker::encode_pdsch(stack_interface_phy_lte::dl_sched_grant_t* grants, uint32_t nof_grants)
{

  /* Scales the Resources Elements affected by the power allocation (p_b) */
  // srslte_enb_dl_prepare_power_allocation(&enb_dl);

  // Prepare for receive ACK for DL grants in t_tx_dl+4
  phy->ue_db_clear(tti_tx_ul);

  for (uint32_t i = 0; i < nof_grants; i++) {
    uint16_t rnti = grants[i].dci.rnti;
    if (rnti) {

      // Compute DL grant
      if (srslte_ra_dl_dci_to_grant(
              &phy->cell, &dl_sf, ue_db[rnti]->dl_cfg.tm, false, &grants[i].dci, &ue_db[rnti]->dl_cfg.pdsch.grant)) {
        Error("Computing DL grant\n");
      }

      // Set soft buffer
      for (uint32_t j = 0; j < SRSLTE_MAX_CODEWORDS; j++) {
        ue_db[rnti]->dl_cfg.pdsch.softbuffers.tx[j] = grants[i].softbuffer_tx[j];
      }

      // Encode PDSCH
      if (srslte_enb_dl_put_pdsch(&enb_dl, &ue_db[rnti]->dl_cfg.pdsch, grants[i].data)) {
        Error("Error putting PDSCH %d\n", i);
        return SRSLTE_ERROR;
      }

      // Save pending ACK
      if (SRSLTE_RNTI_ISUSER(rnti)) {
        /* For each TB */
        for (uint32_t tb_idx = 0; tb_idx < SRSLTE_MAX_TB; tb_idx++) {
          /* If TB enabled, set pending ACK */
          if (ue_db[rnti]->dl_cfg.pdsch.grant.tb[tb_idx].enabled) {
            Debug("ACK: set pending tti=%d, mod=%d\n", tti_tx_ul, TTIMOD(tti_tx_ul));
            phy->ue_db_set_ack_pending(tti_tx_ul, rnti, tb_idx, grants[i].dci.location.ncce);
          }
        }
      }

      if (LOG_THIS(rnti)) {
        // Logging
        char str[512];
        srslte_pdsch_tx_info(&ue_db[rnti]->dl_cfg.pdsch, str, 512);
        Info("PDSCH: %s, tti_tx_dl=%d\n", str, tti_tx_dl);
      }

      // Save metrics stats
      ue_db[rnti]->metrics_dl(grants[i].dci.tb[0].mcs_idx);
    }
  }

  // srslte_enb_dl_apply_power_allocation(&enb_dl);

  return SRSLTE_SUCCESS;
}

/************ METRICS interface ********************/
uint32_t sf_worker::get_metrics(phy_metrics_t metrics[ENB_METRICS_MAX_USERS])
{
  pthread_mutex_lock(&mutex);
  uint32_t cnt = 0;
  for (std::map<uint16_t, ue*>::iterator iter = ue_db.begin(); iter != ue_db.end(); ++iter) {
    ue*      u    = iter->second;
    uint16_t rnti = iter->first;
    if (SRSLTE_RNTI_ISUSER(rnti) && cnt < ENB_METRICS_MAX_USERS) {
      u->metrics_read(&metrics[cnt]);
      cnt++;
    }
  }
  pthread_mutex_unlock(&mutex);
  return cnt;
}

void sf_worker::ue::metrics_read(phy_metrics_t* metrics_)
{
  if (metrics_) {
    *metrics_ = metrics;
  }
  bzero(&metrics, sizeof(phy_metrics_t));
}

void sf_worker::ue::metrics_dl(uint32_t mcs)
{
  metrics.dl.mcs = SRSLTE_VEC_CMA(mcs, metrics.dl.mcs, metrics.dl.n_samples);
  metrics.dl.n_samples++;
}

void sf_worker::ue::metrics_ul(uint32_t mcs, float rssi, float sinr, uint32_t turbo_iters)
{
  metrics.ul.mcs         = SRSLTE_VEC_CMA((float)mcs, metrics.ul.mcs, metrics.ul.n_samples);
  metrics.ul.sinr        = SRSLTE_VEC_CMA((float)sinr, metrics.ul.sinr, metrics.ul.n_samples);
  metrics.ul.rssi        = SRSLTE_VEC_CMA((float)sinr, metrics.ul.rssi, metrics.ul.n_samples);
  metrics.ul.turbo_iters = SRSLTE_VEC_CMA((float)turbo_iters, metrics.ul.turbo_iters, metrics.ul.n_samples);
  metrics.ul.n_samples++;
}

void sf_worker::start_plot()
{
#ifdef ENABLE_GUI
  if (plot_worker_id == -1) {
    plot_worker_id = get_id();
    log_h->console("Starting plot for worker_id=%d\n", plot_worker_id);
    init_plots(this);
  } else {
    log_h->console("Trying to start a plot but already started by worker_id=%d\n", plot_worker_id);
  }
#else
  log_h->console("Trying to start a plot but plots are disabled (ENABLE_GUI constant in sf_worker.cc)\n");
#endif
}

int sf_worker::read_ce_abs(float* ce_abs)
{
  uint32_t i  = 0;
  int      sz = srslte_symbol_sz(phy->cell.nof_prb);
  bzero(ce_abs, sizeof(float) * sz);
  int g = (sz - 12 * phy->cell.nof_prb) / 2;
  for (i = 0; i < 12 * phy->cell.nof_prb; i++) {
    ce_abs[g + i] = 20 * log10(std::abs(std::complex<double>(enb_ul.chest_res.ce[i])));
    if (isinf(ce_abs[g + i])) {
      ce_abs[g + i] = -80;
    }
  }
  return sz;
}

int sf_worker::read_ce_arg(float* ce_arg)
{
  uint32_t i  = 0;
  int      sz = srslte_symbol_sz(phy->cell.nof_prb);
  bzero(ce_arg, sizeof(float) * sz);
  int g = (sz - 12 * phy->cell.nof_prb) / 2;
  for (i = 0; i < 12 * phy->cell.nof_prb; i++) {
    ce_arg[g + i] = std::arg(std::complex<float>(enb_ul.chest_res.ce[i])) * 180.0f / (float)M_PI;
    if (isinf(ce_arg[g + i])) {
      ce_arg[g + i] = -80;
    }
  }
  return sz;
}

int sf_worker::read_pusch_d(cf_t* pdsch_d)
{
  int nof_re = 400; // enb_ul.ul_cfg.pusch.nbits.nof_re
  memcpy(pdsch_d, enb_ul.pusch.d, nof_re * sizeof(cf_t));
  return nof_re;
}

int sf_worker::read_pucch_d(cf_t* pdsch_d)
{
  int nof_re = SRSLTE_PUCCH_MAX_BITS / 2; // enb_ul.ul_cfg.pusch.nbits.nof_re
  memcpy(pdsch_d, enb_ul.pucch.z_tmp, nof_re * sizeof(cf_t));
  return nof_re;
}

} // namespace srsenb

/***********************************************************
 *
 * PLOT TO VISUALIZE THE CHANNEL RESPONSEE
 *
 ***********************************************************/

#ifdef ENABLE_GUI
plot_real_t    pce, pce_arg;
plot_scatter_t pconst;
plot_scatter_t pconst2;
#define SCATTER_PUSCH_BUFFER_LEN (20 * 6 * SRSLTE_SF_LEN_RE(SRSLTE_MAX_PRB, SRSLTE_CP_NORM))
#define SCATTER_PUSCH_PLOT_LEN 4000
float tmp_plot[SCATTER_PUSCH_BUFFER_LEN];
float tmp_plot_arg[SCATTER_PUSCH_BUFFER_LEN];
cf_t  tmp_plot2[SRSLTE_SF_LEN_RE(SRSLTE_MAX_PRB, SRSLTE_CP_NORM)];
cf_t  tmp_pucch_plot[SRSLTE_PUCCH_MAX_BITS / 2];

void* plot_thread_run(void* arg)
{
  srsenb::sf_worker* worker = (srsenb::sf_worker*)arg;

  sdrgui_init_title("srsENB");
  plot_real_init(&pce);
  plot_real_setTitle(&pce, (char*)"Channel Response - Magnitude");
  plot_real_setLabels(&pce, (char*)"Index", (char*)"dB");
  plot_real_setYAxisScale(&pce, -40, 40);

  plot_real_init(&pce_arg);
  plot_real_setTitle(&pce_arg, (char*)"Channel Response - Argument");
  plot_real_setLabels(&pce_arg, (char*)"Angle", (char*)"deg");
  plot_real_setYAxisScale(&pce_arg, -180, 180);

  plot_scatter_init(&pconst);
  plot_scatter_setTitle(&pconst, (char*)"PUSCH - Equalized Symbols");
  plot_scatter_setXAxisScale(&pconst, -4, 4);
  plot_scatter_setYAxisScale(&pconst, -4, 4);

  plot_scatter_init(&pconst2);
  plot_scatter_setTitle(&pconst2, (char*)"PUCCH - Equalized Symbols");
  plot_scatter_setXAxisScale(&pconst2, -4, 4);
  plot_scatter_setYAxisScale(&pconst2, -4, 4);

  plot_real_addToWindowGrid(&pce, (char*)"srsenb", 0, 0);
  plot_real_addToWindowGrid(&pce_arg, (char*)"srsenb", 1, 0);
  plot_scatter_addToWindowGrid(&pconst, (char*)"srsenb", 0, 1);
  plot_scatter_addToWindowGrid(&pconst2, (char*)"srsenb", 1, 1);

  int n, n_arg, n_pucch;
  int readed_pusch_re = 0;
  while (1) {
    sem_wait(&plot_sem);

    n       = worker->read_pusch_d(tmp_plot2);
    n_pucch = worker->read_pucch_d(tmp_pucch_plot);
    plot_scatter_setNewData(&pconst, tmp_plot2, n);
    plot_scatter_setNewData(&pconst2, tmp_pucch_plot, n_pucch);

    n = worker->read_ce_abs(tmp_plot);
    plot_real_setNewData(&pce, tmp_plot, n);

    n_arg = worker->read_ce_arg(tmp_plot_arg);
    plot_real_setNewData(&pce_arg, tmp_plot_arg, n_arg);
  }
  return NULL;
}

void init_plots(srsenb::sf_worker* worker)
{

  if (sem_init(&plot_sem, 0, 0)) {
    perror("sem_init");
    exit(-1);
  }

  pthread_attr_t     attr;
  struct sched_param param;
  param.sched_priority = 0;
  pthread_attr_init(&attr);
  pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
  pthread_attr_setschedparam(&attr, &param);
  if (pthread_create(&plot_thread, &attr, plot_thread_run, worker)) {
    perror("pthread_create");
    exit(-1);
  }
}
#endif
