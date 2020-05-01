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

#include <pthread.h>
#include <srslte/interfaces/sched_interface.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "srsenb/hdr/stack/mac/mac.h"
#include "srslte/common/log.h"
#include "srslte/common/log_helper.h"
#include "srslte/common/rwlock_guard.h"
#include "srslte/common/time_prof.h"

//#define WRITE_SIB_PCAP
using namespace asn1::rrc;

namespace srsenb {

mac::mac() :
  last_rnti(0),
  rar_pdu_msg(sched_interface::MAX_RAR_LIST),
  rar_payload(),
  common_buffers(SRSLTE_MAX_CARRIERS)
{
  pthread_rwlock_init(&rwlock, nullptr);
}

mac::~mac()
{
  stop();
  pthread_rwlock_destroy(&rwlock);
}

bool mac::init(const mac_args_t&        args_,
               const cell_list_t&       cells_,
               phy_interface_stack_lte* phy,
               rlc_interface_mac*       rlc,
               rrc_interface_mac*       rrc,
               stack_interface_mac_lte* stack_,
               srslte::log_ref          log_h_)
{
  started = false;

  if (phy && rlc && log_h_) {
    phy_h = phy;
    rlc_h = rlc;
    rrc_h = rrc;
    stack = stack_;
    log_h = log_h_;

    args  = args_;
    cells = cells_;

    stack_task_queue = stack->make_task_queue();

    scheduler.init(rrc);

    // Set default scheduler configuration
    scheduler.set_sched_cfg(&args.sched);

    // Init softbuffer for SI messages
    common_buffers.resize(cells.size());
    for (auto& cc : common_buffers) {
      for (int i = 0; i < NOF_BCCH_DLSCH_MSG; i++) {
        srslte_softbuffer_tx_init(&cc.bcch_softbuffer_tx[i], args.nof_prb);
      }
      // Init softbuffer for PCCH
      srslte_softbuffer_tx_init(&cc.pcch_softbuffer_tx, args.nof_prb);

      // Init softbuffer for RAR
      srslte_softbuffer_tx_init(&cc.rar_softbuffer_tx, args.nof_prb);
    }

    reset();

    started = true;
  }

  return started;
}

void mac::stop()
{
  srslte::rwlock_write_guard lock(rwlock);
  if (started) {
    ue_db.clear();
    for (auto& cc : common_buffers) {
      for (int i = 0; i < NOF_BCCH_DLSCH_MSG; i++) {
        srslte_softbuffer_tx_free(&cc.bcch_softbuffer_tx[i]);
      }
      srslte_softbuffer_tx_free(&cc.pcch_softbuffer_tx);
      srslte_softbuffer_tx_free(&cc.rar_softbuffer_tx);
      started = false;
    }
  }
}

// Implement Section 5.9
void mac::reset()
{
  Info("Resetting MAC\n");

  last_rnti = 70;

  /* Setup scheduler */
  scheduler.reset();
}

void mac::start_pcap(srslte::mac_pcap* pcap_)
{
  pcap = pcap_;
  // Set pcap in all UEs for UL messages
  for (auto& u : ue_db) {
    u.second->start_pcap(pcap);
  }
}

/********************************************************
 *
 * RLC interface
 *
 *******************************************************/
int mac::rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  srslte::rwlock_read_guard lock(rwlock);
  int                       ret = -1;
  if (ue_db.count(rnti)) {
    if (rnti != SRSLTE_MRNTI) {
      ret = scheduler.dl_rlc_buffer_state(rnti, lc_id, tx_queue, retx_queue);
    } else {
      for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
        if (lc_id == mch.mtch_sched[i].lcid) {
          mch.mtch_sched[i].lcid_buffer_size = tx_queue;
        }
      }
      ret = 0;
    }
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  return ret;
}

int mac::bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg)
{
  int                       ret = -1;
  srslte::rwlock_read_guard lock(rwlock);
  if (ue_db.count(rnti)) {
    // configure BSR group in UE
    ue_db[rnti]->set_lcg(lc_id, (uint32_t)cfg->group);
    ret = scheduler.bearer_ue_cfg(rnti, lc_id, cfg);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  return ret;
}

int mac::bearer_ue_rem(uint16_t rnti, uint32_t lc_id)
{
  srslte::rwlock_read_guard lock(rwlock);
  int                       ret = -1;
  if (ue_db.count(rnti)) {
    ret = scheduler.bearer_ue_rem(rnti, lc_id);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  return ret;
}

void mac::phy_config_enabled(uint16_t rnti, bool enabled)
{
  scheduler.phy_config_enabled(rnti, enabled);
}

// Update UE configuration
int mac::ue_cfg(uint16_t rnti, sched_interface::ue_cfg_t* cfg)
{
  srslte::rwlock_read_guard lock(rwlock);

  auto it     = ue_db.find(rnti);
  ue*  ue_ptr = nullptr;
  if (it == ue_db.end()) {
    Error("User rnti=0x%x not found\n", rnti);
    return SRSLTE_ERROR;
  }
  ue_ptr = it->second.get();

  // Start TA FSM in UE entity
  ue_ptr->start_ta();

  // Add RNTI to the PHY (pregenerate signals) now instead of after PRACH
  if (not ue_ptr->is_phy_added) {
    Info("Registering RNTI=0x%X to PHY...\n", rnti);
    // Register new user in PHY with first CC index
    if (phy_h->add_rnti(rnti, (SRSLTE_MRNTI) ? 0 : cfg->supported_cc_list.front().enb_cc_idx, false) == SRSLTE_ERROR) {
      Error("Registering new UE RNTI=0x%X to PHY\n", rnti);
    }
    Info("Done registering RNTI=0x%X to PHY...\n", rnti);
    ue_ptr->is_phy_added = true;
  }

  // Update Scheduler configuration
  if (cfg != nullptr and scheduler.ue_cfg(rnti, *cfg) == SRSLTE_ERROR) {
    Error("Registering new UE rnti=0x%x to SCHED\n", rnti);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

// Removes UE from DB
int mac::ue_rem(uint16_t rnti)
{
  int ret = -1;
  {
    srslte::rwlock_read_guard lock(rwlock);
    if (ue_db.count(rnti)) {
      phy_h->rem_rnti(rnti);
      scheduler.ue_rem(rnti);
      ret = 0;
    } else {
      Error("User rnti=0x%x not found\n", rnti);
    }
  }
  if (ret) {
    return ret;
  }
  srslte::rwlock_write_guard lock(rwlock);
  if (ue_db.count(rnti)) {
    ue_db.erase(rnti);
    Info("User rnti=0x%x removed from MAC/PHY\n", rnti);
  } else {
    Error("User rnti=0x%x already removed\n", rnti);
  }
  return 0;
}

// Called after Msg3
int mac::ue_set_crnti(uint16_t temp_crnti, uint16_t crnti, sched_interface::ue_cfg_t* cfg)
{
  int ret = ue_cfg(crnti, cfg);
  if (ret != SRSLTE_SUCCESS) {
    return ret;
  }
  srslte::rwlock_read_guard lock(rwlock);
  if (temp_crnti == crnti) {
    // if RNTI is maintained, Msg3 contained a RRC Setup Request
    scheduler.dl_mac_buffer_state(crnti, (uint32_t)srslte::dl_sch_lcid::CON_RES_ID);
  } else {
    // C-RNTI corresponds to older user. Handover scenario.
    phy_h->rem_rnti(crnti);
    phy_h->add_rnti(crnti, cfg->supported_cc_list[0].enb_cc_idx, false);
    scheduler.dl_mac_buffer_state(crnti, (uint32_t)srslte::dl_sch_lcid::CON_RES_ID);
  }
  return ret;
}

int mac::cell_cfg(const std::vector<sched_interface::cell_cfg_t>& cell_cfg_)
{
  cell_config = cell_cfg_;
  return scheduler.cell_cfg(cell_config);
}

void mac::get_metrics(mac_metrics_t metrics[ENB_METRICS_MAX_USERS])
{
  srslte::rwlock_read_guard lock(rwlock);
  int                       cnt = 0;
  for (auto& u : ue_db) {
    u.second->metrics_read(&metrics[cnt]);
    cnt++;
  }
}

/********************************************************
 *
 * PHY interface
 *
 *******************************************************/

void mac::rl_failure(uint16_t rnti)
{
  srslte::rwlock_read_guard lock(rwlock);
  if (ue_db.count(rnti)) {
    uint32_t nof_fails = ue_db[rnti]->rl_failure();
    if (nof_fails >= (uint32_t)args.link_failure_nof_err && args.link_failure_nof_err > 0) {
      Info("Detected Uplink failure for rnti=0x%x\n", rnti);
      rrc_h->rl_failure(rnti);
      ue_db[rnti]->rl_failure_reset();
    }
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
}

void mac::rl_ok(uint16_t rnti)
{
  srslte::rwlock_read_guard lock(rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti]->rl_failure_reset();
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
}

int mac::ack_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack)
{
  srslte::rwlock_read_guard lock(rwlock);
  log_h->step(tti);
  if (ue_db.count(rnti)) {
    uint32_t nof_bytes = scheduler.dl_ack_info(tti, rnti, enb_cc_idx, tb_idx, ack);
    ue_db[rnti]->metrics_tx(ack, nof_bytes);

    if (ack) {
      if (nof_bytes > 64) { // do not count RLC status messages only
        rrc_h->set_activity_user(rnti);
        log_h->debug("DL activity rnti=0x%x, n_bytes=%d\n", rnti, nof_bytes);
      }
    }
  }
  return SRSLTE_SUCCESS;
}

int mac::crc_info(uint32_t tti_rx, uint16_t rnti, uint32_t enb_cc_idx, uint32_t nof_bytes, bool crc)
{
  int ret = SRSLTE_ERROR;
  log_h->step(tti_rx);
  srslte::rwlock_read_guard lock(rwlock);

  if (ue_db.count(rnti)) {
    ue_db[rnti]->set_tti(tti_rx);
    ue_db[rnti]->metrics_rx(crc, nof_bytes);

    std::array<int, SRSLTE_MAX_CARRIERS> enb_ue_cc_map = scheduler.get_enb_ue_cc_map(rnti);
    if (enb_ue_cc_map[enb_cc_idx] < 0) {
      Error("User rnti=0x%x is not activated for carrier %d\n", rnti, enb_cc_idx);
      return ret;
    }
    uint32_t ue_cc_idx = enb_ue_cc_map[enb_cc_idx];

    // push the pdu through the queue if received correctly
    if (crc) {
      Info("Pushing PDU rnti=0x%x, tti_rx=%d, nof_bytes=%d\n", rnti, tti_rx, nof_bytes);
      ue_db[rnti]->push_pdu(ue_cc_idx, tti_rx, nof_bytes);
      stack_task_queue.push([this]() { process_pdus(); });
    } else {
      ue_db[rnti]->deallocate_pdu(ue_cc_idx, tti_rx);
    }

    // Scheduler uses eNB's CC mapping
    ret = scheduler.ul_crc_info(tti_rx, rnti, enb_cc_idx, crc);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }

  return ret;
}

int mac::ri_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t ri_value)
{
  int ret = SRSLTE_ERROR;
  log_h->step(tti);
  srslte::rwlock_read_guard lock(rwlock);

  if (ue_db.count(rnti)) {
    scheduler.dl_ri_info(tti, rnti, enb_cc_idx, ri_value);
    ue_db[rnti]->metrics_dl_ri(ri_value);
    ret = SRSLTE_SUCCESS;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }

  return ret;
}

int mac::pmi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t pmi_value)
{
  int ret = SRSLTE_ERROR;
  log_h->step(tti);
  srslte::rwlock_read_guard lock(rwlock);

  if (ue_db.count(rnti)) {
    scheduler.dl_pmi_info(tti, rnti, enb_cc_idx, pmi_value);
    ue_db[rnti]->metrics_dl_pmi(pmi_value);
    ret = SRSLTE_SUCCESS;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }

  return ret;
}

int mac::cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi_value)
{
  int ret = SRSLTE_ERROR;
  log_h->step(tti);
  srslte::rwlock_read_guard lock(rwlock);

  if (ue_db.count(rnti)) {
    scheduler.dl_cqi_info(tti, rnti, enb_cc_idx, cqi_value);
    ue_db[rnti]->metrics_dl_cqi(cqi_value);
    ret = SRSLTE_SUCCESS;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }

  return ret;
}

int mac::snr_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, float snr)
{
  int ret = SRSLTE_ERROR;
  log_h->step(tti);
  srslte::rwlock_read_guard lock(rwlock);

  if (ue_db.count(rnti)) {
    uint32_t cqi = srslte_cqi_from_snr(snr);
    scheduler.ul_cqi_info(tti, rnti, enb_cc_idx, cqi, 0);
    ret = SRSLTE_SUCCESS;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }

  return ret;
}

int mac::ta_info(uint32_t tti, uint16_t rnti, float ta_us)
{
  srslte::rwlock_read_guard lock(rwlock);
  if (ue_db.count(rnti)) {
    uint32_t nof_ta_count = ue_db[rnti]->set_ta_us(ta_us);
    if (nof_ta_count) {
      scheduler.dl_mac_buffer_state(rnti, (uint32_t)srslte::dl_sch_lcid::TA_CMD, nof_ta_count);
    }
  }
  return SRSLTE_SUCCESS;
}

int mac::sr_detected(uint32_t tti, uint16_t rnti)
{
  log_h->step(tti);
  int                       ret = -1;
  srslte::rwlock_read_guard lock(rwlock);
  if (ue_db.count(rnti)) {
    scheduler.ul_sr_info(tti, rnti);
    ret = 0;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  return ret;
}

uint16_t mac::allocate_rnti()
{
  std::lock_guard<std::mutex> lock(rnti_mutex);

  // Assign a c-rnti
  uint16_t rnti = last_rnti++;
  if (last_rnti >= 60000) {
    last_rnti = 70;
  }

  return rnti;
}

void mac::rach_detected(uint32_t tti, uint32_t enb_cc_idx, uint32_t preamble_idx, uint32_t time_adv)
{
  static srslte::mutexed_tprof<srslte::avg_time_stats> rach_tprof("rach_tprof", "MAC", 1);
  log_h->step(tti);
  auto rach_tprof_meas = rach_tprof.start();

  uint16_t rnti = allocate_rnti();

  // Create new UE
  std::unique_ptr<ue> ue_ptr{new ue(rnti, args.nof_prb, &scheduler, rrc_h, rlc_h, phy_h, log_h, cells.size())};

  // Set PCAP if available
  if (pcap != nullptr) {
    ue_ptr->start_pcap(pcap);
  }

  {
    srslte::rwlock_write_guard lock(rwlock);
    ue_db[rnti] = std::move(ue_ptr);
  }

  stack_task_queue.push([this, rnti, tti, enb_cc_idx, preamble_idx, time_adv, rach_tprof_meas]() mutable {
    rach_tprof_meas.defer_stop();
    // Generate RAR data
    sched_interface::dl_sched_rar_info_t rar_info = {};
    rar_info.preamble_idx                         = preamble_idx;
    rar_info.ta_cmd                               = time_adv;
    rar_info.temp_crnti                           = rnti;
    rar_info.msg3_size                            = 7;
    rar_info.prach_tti                            = tti;

    // Add new user to the scheduler so that it can RX/TX SRB0
    sched_interface::ue_cfg_t ue_cfg = {};
    ue_cfg.supported_cc_list.emplace_back();
    ue_cfg.supported_cc_list.back().active     = true;
    ue_cfg.supported_cc_list.back().enb_cc_idx = enb_cc_idx;
    ue_cfg.ue_bearers[0].direction             = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
    ue_cfg.dl_cfg.tm                           = SRSLTE_TM1;
    if (scheduler.ue_cfg(rnti, ue_cfg) != SRSLTE_SUCCESS) {
      Error("Registering new user rnti=0x%x to SCHED\n", rnti);
      return;
    }

    // Register new user in RRC
    rrc_h->add_user(rnti, ue_cfg);

    // Add temporal rnti to the PHY
    if (phy_h->add_rnti(rnti, enb_cc_idx, true) != SRSLTE_SUCCESS) {
      Error("Registering temporal-rnti=0x%x to PHY\n", rnti);
      return;
    }

    // Trigger scheduler RACH
    scheduler.dl_rach_info(enb_cc_idx, rar_info);

    log_h->info("RACH:  tti=%d, preamble=%d, offset=%d, temp_crnti=0x%x\n", tti, preamble_idx, time_adv, rnti);
    log_h->console("RACH:  tti=%d, preamble=%d, offset=%d, temp_crnti=0x%x\n", tti, preamble_idx, time_adv, rnti);
  });
}

int mac::get_dl_sched(uint32_t tti_tx_dl, dl_sched_list_t& dl_sched_res_list)
{
  if (!started) {
    return 0;
  }

  log_h->step(TTI_SUB(tti_tx_dl, FDD_HARQ_DELAY_UL_MS));

  for (uint32_t enb_cc_idx = 0; enb_cc_idx < cell_config.size(); enb_cc_idx++) {
    // Run scheduler with current info
    sched_interface::dl_sched_res_t sched_result = {};
    if (scheduler.dl_sched(tti_tx_dl, enb_cc_idx, sched_result) < 0) {
      Error("Running scheduler\n");
      return SRSLTE_ERROR;
    }

    int         n            = 0;
    dl_sched_t* dl_sched_res = &dl_sched_res_list[enb_cc_idx];

    {
      srslte::rwlock_read_guard lock(rwlock);

      // Copy data grants
      for (uint32_t i = 0; i < sched_result.nof_data_elems; i++) {

        // Get UE
        uint16_t rnti = sched_result.data[i].dci.rnti;

        if (ue_db.count(rnti)) {
          // Copy dci info
          dl_sched_res->pdsch[n].dci = sched_result.data[i].dci;

          for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
            dl_sched_res->pdsch[n].softbuffer_tx[tb] =
                ue_db[rnti]->get_tx_softbuffer(sched_result.data[i].dci.ue_cc_idx, sched_result.data[i].dci.pid, tb);

            if (sched_result.data[i].nof_pdu_elems[tb] > 0) {
              /* Get PDU if it's a new transmission */
              dl_sched_res->pdsch[n].data[tb] = ue_db[rnti]->generate_pdu(sched_result.data[i].dci.ue_cc_idx,
                                                                          sched_result.data[i].dci.pid,
                                                                          tb,
                                                                          sched_result.data[i].pdu[tb],
                                                                          sched_result.data[i].nof_pdu_elems[tb],
                                                                          sched_result.data[i].tbs[tb]);

              if (!dl_sched_res->pdsch[n].data[tb]) {
                Error("Error! PDU was not generated (rnti=0x%04x, tb=%d)\n", rnti, tb);
              }

              if (pcap) {
                pcap->write_dl_crnti(
                    dl_sched_res->pdsch[n].data[tb], sched_result.data[i].tbs[tb], rnti, true, tti_tx_dl, enb_cc_idx);
              }

            } else {
              /* TB not enabled OR no data to send: set pointers to NULL  */
              dl_sched_res->pdsch[n].data[tb] = nullptr;
            }
          }
          n++;
        } else {
          Warning("Invalid DL scheduling result. User 0x%x does not exist\n", rnti);
        }
      }

      // No more uses of shared ue_db beyond here
    }

    // Copy RAR grants
    for (uint32_t i = 0; i < sched_result.nof_rar_elems; i++) {
      // Copy dci info
      dl_sched_res->pdsch[n].dci = sched_result.rar[i].dci;

      // Set softbuffer (there are no retx in RAR but a softbuffer is required)
      dl_sched_res->pdsch[n].softbuffer_tx[0] = &common_buffers[enb_cc_idx].rar_softbuffer_tx;

      // Assemble PDU
      dl_sched_res->pdsch[n].data[0] = assemble_rar(
          sched_result.rar[i].msg3_grant, sched_result.rar[i].nof_grants, i, sched_result.rar[i].tbs, tti_tx_dl);

      if (pcap) {
        pcap->write_dl_ranti(dl_sched_res->pdsch[n].data[0],
                             sched_result.rar[i].tbs,
                             dl_sched_res->pdsch[n].dci.rnti,
                             true,
                             tti_tx_dl,
                             enb_cc_idx);
      }

      n++;
    }

    // Copy SI and Paging grants
    for (uint32_t i = 0; i < sched_result.nof_bc_elems; i++) {
      // Copy dci info
      dl_sched_res->pdsch[n].dci = sched_result.bc[i].dci;

      // Set softbuffer
      if (sched_result.bc[i].type == sched_interface::dl_sched_bc_t::BCCH) {
        dl_sched_res->pdsch[n].softbuffer_tx[0] =
            &common_buffers[enb_cc_idx].bcch_softbuffer_tx[sched_result.bc[i].index];
        dl_sched_res->pdsch[n].data[0] = rrc_h->read_pdu_bcch_dlsch(enb_cc_idx, sched_result.bc[i].index);
#ifdef WRITE_SIB_PCAP
        if (pcap) {
          pcap->write_dl_sirnti(dl_sched_res->pdsch[n].data[0], sched_result.bc[i].tbs, true, tti_tx_dl, enb_cc_idx);
        }
#endif
      } else {
        dl_sched_res->pdsch[n].softbuffer_tx[0] = &common_buffers[enb_cc_idx].pcch_softbuffer_tx;
        dl_sched_res->pdsch[n].data[0]          = common_buffers[enb_cc_idx].pcch_payload_buffer;
        rlc_h->read_pdu_pcch(common_buffers[enb_cc_idx].pcch_payload_buffer, pcch_payload_buffer_len);

        if (pcap) {
          pcap->write_dl_pch(dl_sched_res->pdsch[n].data[0], sched_result.bc[i].tbs, true, tti_tx_dl, enb_cc_idx);
        }
      }

      n++;
    }

    dl_sched_res->nof_grants = n;

    // Number of CCH symbols
    dl_sched_res->cfi = sched_result.cfi;
  }

  // Count number of TTIs for all active users
  for (auto& u : ue_db) {
    u.second->metrics_cnt();
  }

  return SRSLTE_SUCCESS;
}

void mac::build_mch_sched(uint32_t tbs)
{
  int sfs_per_sched_period = mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].pmch_cfg_r9.sf_alloc_end_r9;
  int bytes_per_sf         = tbs / 8 - 6; // leave 6 bytes for header

  int total_space_avail_bytes = sfs_per_sched_period * bytes_per_sf;

  int total_bytes_to_tx = 0;

  // calculate total bytes to be scheduled
  for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
    total_bytes_to_tx += mch.mtch_sched[i].lcid_buffer_size;
    mch.mtch_sched[i].stop = 0;
  }

  int last_mtch_stop = 0;

  if (total_bytes_to_tx > 0 && total_bytes_to_tx >= total_space_avail_bytes) {
    for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
      double ratio           = mch.mtch_sched[i].lcid_buffer_size / total_bytes_to_tx;
      float  assigned_sfs    = floor(sfs_per_sched_period * ratio);
      mch.mtch_sched[i].stop = last_mtch_stop + (uint32_t)assigned_sfs;
      last_mtch_stop         = mch.mtch_sched[i].stop;
    }
  } else {
    for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
      float assigned_sfs     = ceil(((float)mch.mtch_sched[i].lcid_buffer_size) / ((float)bytes_per_sf));
      mch.mtch_sched[i].stop = last_mtch_stop + (uint32_t)assigned_sfs;
      last_mtch_stop         = mch.mtch_sched[i].stop;
    }
  }
}

int mac::get_mch_sched(uint32_t tti, bool is_mcch, dl_sched_list_t& dl_sched_res_list)
{
  dl_sched_t* dl_sched_res = &dl_sched_res_list[0];
  log_h->step(tti);
  srslte_ra_tb_t mcs      = {};
  srslte_ra_tb_t mcs_data = {};
  mcs.mcs_idx             = this->sib13.mbsfn_area_info_list_r9[0].mcch_cfg_r9.sig_mcs_r9.to_number();
  mcs_data.mcs_idx        = this->mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].pmch_cfg_r9.data_mcs_r9;
  srslte_dl_fill_ra_mcs(&mcs, 0, cell_config[0].cell.nof_prb, false);
  srslte_dl_fill_ra_mcs(&mcs_data, 0, cell_config[0].cell.nof_prb, false);
  if (is_mcch) {
    build_mch_sched(mcs_data.tbs);
    mch.mcch_payload              = mcch_payload_buffer;
    mch.current_sf_allocation_num = 1;
    Info("MCH Sched Info: LCID: %d, Stop: %d, tti is %d \n",
         mch.mtch_sched[0].lcid,
         mch.mtch_sched[mch.num_mtch_sched - 1].stop,
         tti);
    phy_h->set_mch_period_stop(mch.mtch_sched[mch.num_mtch_sched - 1].stop);
    for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
      mch.pdu[i].lcid = (uint32_t)srslte::mch_lcid::MCH_SCHED_INFO;
      // m1u.mtch_sched[i].lcid = 1+i;
    }

    mch.pdu[mch.num_mtch_sched].lcid   = 0;
    mch.pdu[mch.num_mtch_sched].nbytes = current_mcch_length;
    dl_sched_res->pdsch[0].dci.rnti    = SRSLTE_MRNTI;

    // we use TTI % HARQ to make sure we use different buffers for consecutive TTIs to avoid races between PHY workers
    ue_db[SRSLTE_MRNTI]->metrics_tx(true, mcs.tbs);
    dl_sched_res->pdsch[0].data[0] =
        ue_db[SRSLTE_MRNTI]->generate_mch_pdu(tti % SRSLTE_FDD_NOF_HARQ, mch, mch.num_mtch_sched + 1, mcs.tbs / 8);

  } else {
    uint32_t current_lcid = 1;
    uint32_t mtch_index   = 0;
    uint32_t mtch_stop    = mch.mtch_sched[mch.num_mtch_sched - 1].stop;

    for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
      if (mch.current_sf_allocation_num <= mch.mtch_sched[i].stop) {
        current_lcid = mch.mtch_sched[i].lcid;
        mtch_index   = i;
        break;
      }
    }
    if (mch.current_sf_allocation_num <= mtch_stop) {
      int requested_bytes = (mcs_data.tbs / 8 > (int)mch.mtch_sched[mtch_index].lcid_buffer_size)
                                ? (mch.mtch_sched[mtch_index].lcid_buffer_size)
                                : ((mcs_data.tbs / 8) - 2);
      int bytes_received = ue_db[SRSLTE_MRNTI]->read_pdu(current_lcid, mtch_payload_buffer, requested_bytes);
      mch.pdu[0].lcid    = current_lcid;
      mch.pdu[0].nbytes  = bytes_received;
      mch.mtch_sched[0].mtch_payload  = mtch_payload_buffer;
      dl_sched_res->pdsch[0].dci.rnti = SRSLTE_MRNTI;
      if (bytes_received) {
        ue_db[SRSLTE_MRNTI]->metrics_tx(true, mcs.tbs);
        dl_sched_res->pdsch[0].data[0] =
            ue_db[SRSLTE_MRNTI]->generate_mch_pdu(tti % SRSLTE_FDD_NOF_HARQ, mch, 1, mcs_data.tbs / 8);
      }
    } else {
      dl_sched_res->pdsch[0].dci.rnti = 0;
      dl_sched_res->pdsch[0].data[0]  = nullptr;
    }
    mch.current_sf_allocation_num++;
  }

  // Count number of TTIs for all active users
  for (auto& u : ue_db) {
    u.second->metrics_cnt();
  }
  return SRSLTE_SUCCESS;
}

uint8_t* mac::assemble_rar(sched_interface::dl_sched_rar_grant_t* grants,
                           uint32_t                               nof_grants,
                           int                                    rar_idx,
                           uint32_t                               pdu_len,
                           uint32_t                               tti)
{
  uint8_t grant_buffer[64] = {};
  if (pdu_len < rar_payload_len) {
    srslte::rar_pdu* pdu = &rar_pdu_msg[rar_idx];
    rar_payload[rar_idx].clear();
    pdu->init_tx(&rar_payload[rar_idx], pdu_len);
    for (uint32_t i = 0; i < nof_grants; i++) {
      srslte_dci_rar_pack(&grants[i].grant, grant_buffer);
      if (pdu->new_subh()) {
        pdu->get()->set_rapid(grants[i].data.preamble_idx);
        pdu->get()->set_ta_cmd(grants[i].data.ta_cmd);
        pdu->get()->set_temp_crnti(grants[i].data.temp_crnti);
        pdu->get()->set_sched_grant(grant_buffer);
      }
    }
    pdu->write_packet(rar_payload[rar_idx].msg);
    return rar_payload[rar_idx].msg;
  } else {
    Error("Assembling RAR: pdu_len > rar_payload_len (%d>%d)\n", pdu_len, rar_payload_len);
    return nullptr;
  }
}

int mac::get_ul_sched(uint32_t tti_tx_ul, ul_sched_list_t& ul_sched_res_list)
{
  if (!started) {
    return SRSLTE_SUCCESS;
  }

  log_h->step(TTI_SUB(tti_tx_ul, FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS));

  // Execute TA FSM
  for (auto& ue : ue_db) {
    uint32_t nof_ta_count = ue.second->tick_ta_fsm();
    if (nof_ta_count) {
      scheduler.dl_mac_buffer_state(ue.first, (uint32_t)srslte::dl_sch_lcid::TA_CMD, nof_ta_count);
    }
  }

  for (uint32_t enb_cc_idx = 0; enb_cc_idx < cell_config.size(); enb_cc_idx++) {
    ul_sched_t* phy_ul_sched_res = &ul_sched_res_list[enb_cc_idx];

    // Run scheduler with current info
    sched_interface::ul_sched_res_t sched_result = {};
    if (scheduler.ul_sched(tti_tx_ul, enb_cc_idx, sched_result) < 0) {
      Error("Running scheduler\n");
      return SRSLTE_ERROR;
    }

    {
      srslte::rwlock_read_guard lock(rwlock);

      // Copy DCI grants
      phy_ul_sched_res->nof_grants = 0;
      int n                        = 0;
      for (uint32_t i = 0; i < sched_result.nof_dci_elems; i++) {

        if (sched_result.pusch[i].tbs > 0) {
          // Get UE
          uint16_t rnti = sched_result.pusch[i].dci.rnti;

          if (ue_db.count(rnti)) {
            // Copy grant info
            phy_ul_sched_res->pusch[n].current_tx_nb = sched_result.pusch[i].current_tx_nb;
            phy_ul_sched_res->pusch[n].needs_pdcch   = sched_result.pusch[i].needs_pdcch;
            phy_ul_sched_res->pusch[n].dci           = sched_result.pusch[i].dci;
            phy_ul_sched_res->pusch[n].softbuffer_rx =
                ue_db[rnti]->get_rx_softbuffer(sched_result.pusch[i].dci.ue_cc_idx, tti_tx_ul);
            if (sched_result.pusch[n].current_tx_nb == 0) {
              srslte_softbuffer_rx_reset_tbs(phy_ul_sched_res->pusch[n].softbuffer_rx, sched_result.pusch[i].tbs * 8);
            }
            phy_ul_sched_res->pusch[n].data =
                ue_db[rnti]->request_buffer(sched_result.pusch[i].dci.ue_cc_idx, tti_tx_ul, sched_result.pusch[i].tbs);
            phy_ul_sched_res->nof_grants++;
            n++;
          } else {
            Warning("Invalid UL scheduling result. User 0x%x does not exist\n", rnti);
          }

        } else {
          Warning("Grant %d for rnti=0x%x has zero TBS\n", i, sched_result.pusch[i].dci.rnti);
        }
      }

      // No more uses of ue_db beyond here
    }

    // Copy PHICH actions
    for (uint32_t i = 0; i < sched_result.nof_phich_elems; i++) {
      phy_ul_sched_res->phich[i].ack  = sched_result.phich[i].phich == sched_interface::ul_sched_phich_t::ACK;
      phy_ul_sched_res->phich[i].rnti = sched_result.phich[i].rnti;
    }
    phy_ul_sched_res->nof_phich = sched_result.nof_phich_elems;
  }
  return SRSLTE_SUCCESS;
}

bool mac::process_pdus()
{
  srslte::rwlock_read_guard lock(rwlock);
  bool                      ret = false;
  for (auto& u : ue_db) {
    ret |= u.second->process_pdus();
  }
  return ret;
}

void mac::write_mcch(sib_type2_s* sib2_, sib_type13_r9_s* sib13_, mcch_msg_s* mcch_)
{
  mcch               = *mcch_;
  mch.num_mtch_sched = this->mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].mbms_session_info_list_r9.size();
  for (uint32_t i = 0; i < mch.num_mtch_sched; ++i) {
    mch.mtch_sched[i].lcid =
        this->mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].mbms_session_info_list_r9[i].lc_ch_id_r9;
  }
  sib2  = *sib2_;
  sib13 = *sib13_;

  const int     rlc_header_len = 1;
  asn1::bit_ref bref(&mcch_payload_buffer[rlc_header_len], sizeof(mcch_payload_buffer) - rlc_header_len);
  mcch.pack(bref);
  current_mcch_length = bref.distance_bytes(&mcch_payload_buffer[1]);
  current_mcch_length = current_mcch_length + rlc_header_len;
  ue_db[SRSLTE_MRNTI] =
      std::unique_ptr<ue>{new ue(SRSLTE_MRNTI, args.nof_prb, &scheduler, rrc_h, rlc_h, phy_h, log_h, cells.size())};

  rrc_h->add_user(SRSLTE_MRNTI, {});
}

} // namespace srsenb
